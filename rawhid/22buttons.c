#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#if defined(OS_LINUX) || defined(OS_MACOSX)
#include <sys/ioctl.h>
#include <termios.h>
#elif defined(OS_WINDOWS)
#include <conio.h>
#endif

#include "hid.h"
#include <time.h>


// button names:
// L9 L8 L7       R7 R8 R9
// L6 L5 L4 LS RS R4 R5 R6
// L3 L2 L1 L0 R0 R1 R2 R3
// 
// button ports:
// F5 F4 F1       B1 B2 B3
// B5 B6 F7 F0 B0 D0 D1 D2
// D4 D7 B4 F6 B7 D3 C6 D5
// 
// PINB is bufindex 0
// PINC and PIND are index 1
// PINF is bufindex 2


int i, j, r, num;
char mode = 'a';// 'a' for lowercase letters, 'A' for uppercase, 'n' for numeric
char /*c,*/ buf[64];
char prevbuf[64];
int longpressunit  = 200e6;
//int holdrepeatunit = 200e6;
struct timespec currenttime;


typedef struct{
	int bufindex;
	int mask;
	char pressed;
	char longpressed;
	int  holdrepeats;
	struct timespec presstimestamp;
	struct timespec releasetimestamp;
	void (*onpress)();
	void (*onlongpress)();
	void (*onholdrepeat)();
	void (*onrelease)();
} button;

button thebuttons[24];

// L0, F6
void L0onpress() {
	switch (mode) {
		case 'a':
			printf("<autocomp a>");
			break;
		case 'A':
			printf("<autocomp A>");
			break;
		case 'n':
			printf("<autocomp n>");
			break;
	}
}
void L0onlongpress() {
	switch (mode) {
		case 'a':
			printf("<fastautocomp a>");
			break;
		case 'A':
			printf("<fastautocomp A>");
			break;
		case 'n':
			printf("<fastautocomp n>");
			break;
	}
}
// L1, B4
void L1onpress() {
	switch (mode) {
		case 'a':
			printf("\033[1D");
			break;
		case 'A':
			printf("\033[1D");
			break;
		case 'n':
			printf("0");
			break;
	}
}
void L1onlongpress() {
	switch (mode) {
		case 'a':
			break;
		case 'A':
			break;
		case 'n':
			printf("\033[1D'");
			break;
	}
}
// L2, D7
void L2onpress() {
	switch (mode) {
		case 'a':
			mode = 'A';
			break;
		case 'A':
			mode = 'a';
			break;
		case 'n':
			printf(".");
			break;
	}
}
void L2onlongpress() {
	switch (mode) {
		case 'a':
			break;
		case 'A':
			break;
		case 'n':
			printf("\033[1D,");
			break;
	}
}
// L3, D4
void L3onpress() {
	switch (mode) {
		case 'a':
			mode = 'n';
			break;
		case 'A':
			mode = 'n';
			break;
		case 'n':
			mode = 'a';
			break;
	}
}
void L3onlongpress() {
	switch (mode) {
		case 'a':
			break;
		case 'A':
			break;
		case 'n':
			break;
	}
}
// L4, F7
void L4onpress() {
	switch (mode) {
		case 'a':
			printf("s");
			break;
		case 'A':
			printf("S");
			break;
		case 'n':
			printf("-");
			break;
	}
}
void L4onlongpress() {
	switch (mode) {
		case 'a':
			printf("\033[1Dp");
			break;
		case 'A':
			printf("\033[1DP");
			break;
		case 'n':
			printf("\033[1D=");
			break;
	}
}
// L5, B6
void L5onpress() {
	switch (mode) {
		case 'a':
			printf("a");
			break;
		case 'A':
			printf("A");
			break;
		case 'n':
			printf("/");
			break;
	}
}
void L5onlongpress() {
	switch (mode) {
		case 'a':
			printf("\033[1Dm");
			break;
		case 'A':
			printf("\033[1DM");
			break;
		case 'n':
			printf("\033[1D^");
			break;
	}
}
// L6, B5
void L6onpress() {
	switch (mode) {
		case 'a':
			printf("n");
			break;
		case 'A':
			printf("N");
			break;
		case 'n':
			printf("\033[1D%%");
			break;
	}
}
void L6onlongpress() {
	switch (mode) {
		case 'a':
			printf("\033[1Dk");
			break;
		case 'A':
			printf("\033[1DK");
			break;
		case 'n':
			printf("\033[1D<");
			break;
	}
}
// L7, F1
void L7onpress() {
	switch (mode) {
		case 'a':
			printf("h");
			break;
		case 'A':
			printf("H");
			break;
		case 'n':
			printf("+");
			break;
	}
}
void L7onlongpress() {
	switch (mode) {
		case 'a':
			printf("\033[1Dj");
			break;
		case 'A':
			printf("\033[1DJ");
			break;
		case 'n':
			printf("\033[1D:");
			break;
	}
}
// L8, F4
void L8onpress() {
	switch (mode) {
		case 'a':
			printf("r");
			break;
		case 'A':
			printf("R");
			break;
		case 'n':
			printf("*");
			break;
	}
}
void L8onlongpress() {
	switch (mode) {
		case 'a':
			printf("\033[1Db");
			break;
		case 'A':
			printf("\033[1DB");
			break;
		case 'n':
			printf("\033[1D!");
			break;
	}
}
// L9, F5
void L9onpress() {
	switch (mode) {
		case 'a':
			printf("o");
			break;
		case 'A':
			printf("O");
			break;
		case 'n':
			printf("^");
			break;
	}
}
void L9onlongpress() {
	switch (mode) {
		case 'a':
			printf("\033[1Df");
			break;
		case 'A':
			printf("\033[1DF");
			break;
		case 'n':
			printf("\033[1D?");
			break;
	}
}
// R0, B7
void R0onpress() {
	switch (mode) {
		case 'a':
			printf("R0");
			break;
		case 'A':
			printf("R0");
			break;
		case 'n':
			printf("R0");
			break;
	}
}
void R0onlongpress() {
	switch (mode) {
		case 'a':
			break;
		case 'A':
			break;
		case 'n':
			break;
	}
}
// R1, D3
void R1onpress() {
	switch (mode) {
		case 'a':
			printf("\033[1C");
			break;
		case 'A':
			printf("\033[1C");
			break;
		case 'n':
			printf("1");
			break;
	}
}
void R1onlongpress() {
	switch (mode) {
		case 'a':
			break;
		case 'A':
			break;
		case 'n':
			printf("\033[1D|");
			break;
	}
}
// R2, C6
void R2onpress() {
	switch (mode) {
		case 'a':
			printf("l");
			break;
		case 'A':
			printf("L");
			break;
		case 'n':
			printf("2");
			break;
	}
}
void R2onlongpress() {
	switch (mode) {
		case 'a':
			printf("\033[1Dx");
			break;
		case 'A':
			printf("\033[1DX");
			break;
		case 'n':
			printf("\033[1D(");
			break;
	}
}
// R3, D5
void R3onpress() {
	switch (mode) {
		case 'a':
			printf("c");
			break;
		case 'A':
			printf("C");
			break;
		case 'n':
			printf("3");
			break;
	}
}
void R3onlongpress() {
	switch (mode) {
		case 'a':
			printf("\033[1Dz");
			break;
		case 'A':
			printf("\033[1DZ");
			break;
		case 'n':
			printf("\033[1D)");
			break;
	}
}
// R4, D0
void R4onpress() {
	switch (mode) {
		case 'a':
			printf("t");
			break;
		case 'A':
			printf("T");
			break;
		case 'n':
			printf("4");
			break;
	}
}
void R4onlongpress() {
	switch (mode) {
		case 'a':
			printf("\033[1Dg");
			break;
		case 'A':
			printf("\033[1DG");
			break;
		case 'n':
			printf("\033[1D#");
			break;
	}
}
// R5, D1
void R5onpress() {
	switch (mode) {
		case 'a':
			printf(" ");
			break;
		case 'A':
			printf(" ");
			break;
		case 'n':
			printf("5");
			break;
	}
}
void R5onlongpress() {
	switch (mode) {
		case 'a':
			printf("\033[1D\n");
			break;
		case 'A':
			printf("\033[1D\n");
			break;
		case 'n':
			printf("\033[1D[");
			break;
	}
}
// R6, D2
void R6onpress() {
	switch (mode) {
		case 'a':
			printf("u");
			break;
		case 'A':
			printf("U");
			break;
		case 'n':
			printf("6");
			break;
	}
}
void R6onlongpress() {
	switch (mode) {
		case 'a':
			printf("\033[1Dq");
			break;
		case 'A':
			printf("\033[1DQ");
			break;
		case 'n':
			printf("\033[1D]");
			break;
	}
}
// R7, B1
void R7onpress() {
	switch (mode) {
		case 'a':
			printf("i");
			break;
		case 'A':
			printf("I");
			break;
		case 'n':
			printf("7");
			break;
	}
}
void R7onlongpress() {
	switch (mode) {
		case 'a':
			printf("\033[1Dv");
			break;
		case 'A':
			printf("\033[1DV");
			break;
		case 'n':
			printf("\033[1D~");
			break;
	}
}
// R8, B2
void R8onpress() {
	switch (mode) {
		case 'a':
			printf("e");
			break;
		case 'A':
			printf("E");
			break;
		case 'n':
			printf("8");
			break;
	}
}
void R8onlongpress() {
	switch (mode) {
		case 'a':
			printf("\033[1Dw");
			break;
		case 'A':
			printf("\033[1DW");
			break;
		case 'n':
			printf("\033[1D_");
			break;
	}
}
// R9, B3
void R9onpress() {
	switch (mode) {
		case 'a':
			printf("d");
			break;
		case 'A':
			printf("D");
			break;
		case 'n':
			printf("9");
			break;
	}
}
void R9onlongpress() {
	switch (mode) {
		case 'a':
			printf("\033[1Dy");
			break;
		case 'A':
			printf("\033[1DY");
			break;
		case 'n':
			printf("\033[1D@");
			break;
	}
}
// LS, F0
void LSonpress() {
	switch (mode) {
		case 'a':
			printf("LS");
			break;
		case 'A':
			printf("LS");
			break;
		case 'n':
			printf("LS");
			break;
	}
}
void LSonlongpress() {
	switch (mode) {
		case 'a':
			break;
		case 'A':
			break;
		case 'n':
			break;
	}
}
// RS, B0
void RSonpress() {
	switch (mode) {
		case 'a':
			printf("RS");
			break;
		case 'A':
			printf("RS");
			break;
		case 'n':
			printf("RS");
			break;
	}
}
void RSonlongpress() {
	switch (mode) {
		case 'a':
			break;
		case 'A':
			break;
		case 'n':
			break;
	}
}

int initButtons() {
	/*
	button indices
	L0 22
	L1  4
	L2 15
	L3 12
	L4 23
	L5  6
	L6  5
	L7 17
	L8 20
	L9 21
	R0  7
	R1 11
	R2 14
	R3 13
	R4  8
	R5  9
	R6 10
	R7  1
	R8  2
	R9  3
	LS 16
	RS  0
	(18 and 19 are unused)
	*/
	
	// L0, F6
	thebuttons[22].bufindex = 2;
	thebuttons[22].mask = 1<<6;
	thebuttons[22].onpress = L0onpress;
	thebuttons[22].onlongpress = L0onlongpress;
	// L1, B4
	thebuttons[4].bufindex = 0;
	thebuttons[4].mask = 1<<4;
	thebuttons[4].onpress = L1onpress;
	thebuttons[4].onlongpress = L1onlongpress;
	// L2, D7
	thebuttons[15].bufindex = 1;
	thebuttons[15].mask = 1<<7;
	thebuttons[15].onpress = L2onpress;
	thebuttons[15].onlongpress = L2onlongpress;
	// L3, D4
	thebuttons[12].bufindex = 1;
	thebuttons[12].mask = 1<<4;
	thebuttons[12].onpress = L3onpress;
	thebuttons[12].onlongpress = L3onlongpress;
	// L4, F7
	thebuttons[23].bufindex = 2;
	thebuttons[23].mask = 1<<7;
	thebuttons[23].onpress = L4onpress;
	thebuttons[23].onlongpress = L4onlongpress;
	// L5, B6
	thebuttons[6].bufindex = 0;
	thebuttons[6].mask = 1<<6;
	thebuttons[6].onpress = L5onpress;
	thebuttons[6].onlongpress = L5onlongpress;
	// L6, B5
	thebuttons[5].bufindex = 0;
	thebuttons[5].mask = 1<<5;
	thebuttons[5].onpress = L6onpress;
	thebuttons[5].onlongpress = L6onlongpress;
	// L7, F1
	thebuttons[17].bufindex = 2;
	thebuttons[17].mask = 1<<1;
	thebuttons[17].onpress = L7onpress;
	thebuttons[17].onlongpress = L7onlongpress;
	// L8, F4
	thebuttons[20].bufindex = 2;
	thebuttons[20].mask = 1<<4;
	thebuttons[20].onpress = L8onpress;
	thebuttons[20].onlongpress = L8onlongpress;
	// L9, F5
	thebuttons[21].bufindex = 2;
	thebuttons[21].mask = 1<<5;
	thebuttons[21].onpress = L9onpress;
	thebuttons[21].onlongpress = L9onlongpress;
	// R0, B7
	thebuttons[7].bufindex = 0;
	thebuttons[7].mask = 1<<7;
	thebuttons[7].onpress = R0onpress;
	thebuttons[7].onlongpress = R0onlongpress;
	// R1, D3
	thebuttons[11].bufindex = 1;
	thebuttons[11].mask = 1<<3;
	thebuttons[11].onpress = R1onpress;
	thebuttons[11].onlongpress = R1onlongpress;
	// R2, C6
	thebuttons[14].bufindex = 1;
	thebuttons[14].mask = 1<<6;
	thebuttons[14].onpress = R2onpress;
	thebuttons[14].onlongpress = R2onlongpress;
	// R3, D5
	thebuttons[13].bufindex = 1;
	thebuttons[13].mask = 1<<5;
	thebuttons[13].onpress = R3onpress;
	thebuttons[13].onlongpress = R3onlongpress;
	// R4, D0
	thebuttons[8].bufindex = 1;
	thebuttons[8].mask = 1;
	thebuttons[8].onpress = R4onpress;
	thebuttons[8].onlongpress = R4onlongpress;
	// R5, D1
	thebuttons[9].bufindex = 1;
	thebuttons[9].mask = 1<<1;
	thebuttons[9].onpress = R5onpress;
	thebuttons[9].onlongpress = R5onlongpress;
	// R6, D2
	thebuttons[10].bufindex = 1;
	thebuttons[10].mask = 1<<2;
	thebuttons[10].onpress = R6onpress;
	thebuttons[10].onlongpress = R6onlongpress;
	// R7, B1
	thebuttons[1].bufindex = 0;
	thebuttons[1].mask = 1<<1;
	thebuttons[1].onpress = R7onpress;
	thebuttons[1].onlongpress = R7onlongpress;
	// R8, B2
	thebuttons[2].bufindex = 0;
	thebuttons[2].mask = 1<<2;
	thebuttons[2].onpress = R8onpress;
	thebuttons[2].onlongpress = R8onlongpress;
	// R9, B3
	thebuttons[3].bufindex = 0;
	thebuttons[3].mask = 1<<3;
	thebuttons[3].onpress = R9onpress;
	thebuttons[3].onlongpress = R9onlongpress;
	// LS, F0
	thebuttons[16].bufindex = 2;
	thebuttons[16].mask = 1;
	thebuttons[16].onpress = LSonpress;
	thebuttons[16].onlongpress = LSonlongpress;
	// RS, B0
	thebuttons[0].bufindex = 0;
	thebuttons[0].mask = 1;
	thebuttons[0].onpress = RSonpress;
	thebuttons[0].onlongpress = RSonlongpress;
	
	return 0;
}

int main()
{
	setbuf(stdout, NULL);
	
	initButtons();
	
	// C-based example is 16C0:0480:FFAB:0200
	r = rawhid_open(1, 0x16C0, 0x0480, 0xFFAB, 0x0200);
	if (r <= 0) {
		// Arduino-based example is 16C0:0486:FFAB:0200
		//r = rawhid_open(1, 0x16C0, 0x0486, 0xFFAB, 0x0200);
		//if (r <= 0) {
			printf("no rawhid device found\n");
			return -1;
		//}
	}
	printf("found rawhid device\n");
	
	while (1) {
		// check if any Raw HID packet has arrived
		num = rawhid_recv(0, buf, 64, 2);
		if (num < 0) {
			printf("\nerror reading, device went offline\n");
			rawhid_close(0);
			return 0;
		}
		if (num > 0) {
			
			for (i=0; i<3; i++) {
				for (j=0; j<8; j++) {
					if ( ( buf[i] & 1<<j) != (prevbuf[i] & 1<<j) ) {
						if ( buf[i] & 1<<j ) {
							clock_gettime(CLOCK_MONOTONIC, &thebuttons[ j + i*8 ].presstimestamp);
							thebuttons[ j + i*8 ].pressed = 1;
							thebuttons[ j + i*8 ].onpress();
						}
						else {
							//clock_gettime(CLOCK_MONOTONIC, &thebuttons[ j + i*8 ].releasetimestamp);
							thebuttons[ j + i*8 ].pressed = 0;
							thebuttons[ j + i*8 ].longpressed = 0;
							//thebuttons[ j + i*8 ].onrelease();
						}
					}
				}
				prevbuf[i] = buf[i];
			}
			
			for (i=0; i<24; i++) {
				if ( 
					thebuttons[i].pressed &&
					!clock_gettime(CLOCK_MONOTONIC, &currenttime) &&
					(currenttime.tv_sec - thebuttons[i].presstimestamp.tv_sec) * 1000e6 + currenttime.tv_nsec - thebuttons[i].presstimestamp.tv_nsec > longpressunit
				) {
					if (thebuttons[i].longpressed) {
					}
					else {
						thebuttons[i].longpressed = 1;
						thebuttons[i].onlongpress();
					}
				}
			}
			
		}
	}
}