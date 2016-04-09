#include "Alu.h"
#include "Cpu.h"

//exewb Buffer Indices
#define RESULT 0

//decexe Buffer Indices
#define SREG 0
#define TREG 1
#define DREG 2
#define SHFUNC 3
#define IMM6 4
#define IMM12 5

void ALU::runWithFunc(){
	switch (Buffer::decexe[1][SHFUNC]){
		case 0: //ADD
			Buffer::exemem[0][RESULT] = Buffer::decexe[1][SREG] + Buffer::decexe[1][TREG];
			break;
		case 1: //SUB
			Buffer::exemem[0][RESULT] = Buffer::decexe[1][SREG] - Buffer::decexe[1][TREG];
			break;
		case 2: //AND
			Buffer::exemem[0][RESULT] = Buffer::decexe[1][SREG] & Buffer::decexe[1][TREG];
			break;
		case 3: //OR
			Buffer::exemem[0][RESULT] = Buffer::decexe[1][SREG] | Buffer::decexe[1][TREG];
			break;
		case 4: //XOR
			Buffer::exemem[0][RESULT] = Buffer::decexe[1][SREG] ^ Buffer::decexe[1][TREG];
			break;
	}
}

void ALU::addi(){
	Buffer::exemem[0][RESULT] = Buffer::decexe[1][TREG] + Buffer::decexe[1][IMM6];
}

/*
*/
void ALU::sll() {
	Buffer::exemem[0][RESULT] = Buffer::decexe[1][TREG] << Buffer::decexe[1][SHFUNC];
}

/* This relies on the fact that Visual Studio compiles a right-shift as a an arithmatic shift, if it's a signed value.
Therefore, this may not work if you compile with another compiler.
*/
void ALU::sra() {
	Buffer::exemem[0][RESULT] = (int16_t)Buffer::decexe[1][TREG] >> Buffer::decexe[1][SHFUNC];
}

/*See above comment regarding SRA*/
void ALU::srl() {
	Buffer::exemem[0][RESULT] = Buffer::decexe[1][TREG] >> Buffer::decexe[1][SHFUNC];
}