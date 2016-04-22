#include "Alu.h"
#include "Cpu.h"

void ALU::runWithFunc(uint16_t exec[2][9]){
	switch (exec[0][SHFUNC]){
		case 0: //ADD
			exec[1][RESULT] = (int16_t)exec[0][SREGVAL] + (int16_t)exec[0][TREGVAL];
			break;
		case 1: //SUB
			exec[1][RESULT] = (int16_t)exec[0][SREGVAL] - (int16_t)exec[0][TREGVAL];
			break;
		case 2: //AND
			exec[1][RESULT] = exec[0][SREGVAL] & exec[0][TREGVAL];
			break;
		case 3: //OR
			exec[1][RESULT] = exec[0][SREGVAL] | exec[0][TREGVAL];
			break;
		case 4: //XOR
			exec[1][RESULT] = exec[0][SREGVAL] ^ exec[0][TREGVAL];
			break;
	}
}

void ALU::addi(uint16_t exec[2][9]){
	exec[1][RESULT] = (int16_t)exec[0][SREGVAL] + (int16_t)exec[0][IMM6];
}

void ALU::slt(uint16_t exec[2][9]) {
	exec[1][RESULT] = (int16_t)exec[0][TREGVAL] < (int16_t)exec[0][SREGVAL];
}

void ALU::sll(uint16_t exec[2][9]) {
	exec[1][RESULT] = exec[0][TREGVAL] << exec[0][SHFUNC];
}

/* This relies on the fact that Visual Studio compiles a right-shift as a an arithmatic shift, if it's a signed value.
Therefore, this may not work if you compile with another compiler.
*/
void ALU::sra(uint16_t exec[2][9]) {
	exec[1][RESULT] = (int16_t)exec[0][TREGVAL] >> exec[0][SHFUNC];
}

/*See above comment regarding SRA*/
void ALU::srl(uint16_t exec[2][9]) {
	exec[1][RESULT] = exec[0][TREGVAL] >> exec[0][SHFUNC];
}

void ALU::mem(uint16_t exec[2][9]) {
	exec[1][RESULT] = (int16_t)exec[0][SREGVAL] + (int16_t)exec[0][IMM6];
}