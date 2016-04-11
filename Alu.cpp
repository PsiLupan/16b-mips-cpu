#include "Alu.h"
#include "Cpu.h"

void ALU::runWithFunc(){
	switch (Buffer::exec[0][SHFUNC]){
		case 0: //ADD
			Buffer::exec[1][RESULT] = Buffer::exec[0][SREGVAL] + Buffer::exec[0][TREGVAL];
			break;
		case 1: //SUB
			Buffer::exec[1][RESULT] = Buffer::exec[0][SREGVAL] - Buffer::exec[0][TREGVAL];
			break;
		case 2: //AND
			Buffer::exec[1][RESULT] = Buffer::exec[0][SREGVAL] & Buffer::exec[0][TREGVAL];
			break;
		case 3: //OR
			Buffer::exec[1][RESULT] = Buffer::exec[0][SREGVAL] | Buffer::exec[0][TREGVAL];
			break;
		case 4: //XOR
			Buffer::exec[1][RESULT] = Buffer::exec[0][SREGVAL] ^ Buffer::exec[0][TREGVAL];
			break;
	}
}

void ALU::addi(){
	Buffer::exec[1][RESULT] = Buffer::exec[0][TREGVAL] + Buffer::exec[0][IMM6];
}

void ALU::slt() {
	Buffer::exec[1][RESULT] = Buffer::exec[0][TREGVAL] < Buffer::exec[0][SREGVAL];
}

void ALU::sll() {
	Buffer::exec[1][RESULT] = Buffer::exec[0][TREGVAL] << Buffer::exec[0][SHFUNC];
}

/* This relies on the fact that Visual Studio compiles a right-shift as a an arithmatic shift, if it's a signed value.
Therefore, this may not work if you compile with another compiler.
*/
void ALU::sra() {
	Buffer::exec[1][RESULT] = (int16_t)Buffer::exec[0][TREGVAL] >> Buffer::exec[0][SHFUNC];
}

/*See above comment regarding SRA*/
void ALU::srl() {
	Buffer::exec[1][RESULT] = Buffer::exec[0][TREGVAL] >> Buffer::exec[0][SHFUNC];
}