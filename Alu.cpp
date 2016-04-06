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
	switch (CPU::decexe_buf[1][SHFUNC]){
		case 0: //ADD
			CPU::exemem_buf[0][RESULT] = CPU::decexe_buf[1][SREG] + CPU::decexe_buf[1][TREG];
			break;
		case 1: //SUB
			CPU::exemem_buf[0][RESULT] = CPU::decexe_buf[1][SREG] - CPU::decexe_buf[1][TREG];
			break;
		case 2: //AND
			CPU::exemem_buf[0][RESULT] = CPU::decexe_buf[1][SREG] & CPU::decexe_buf[1][TREG];
			break;
		case 3: //OR
			CPU::exemem_buf[0][RESULT] = CPU::decexe_buf[1][SREG] | CPU::decexe_buf[1][TREG];
			break;
		case 4: //XOR
			CPU::exemem_buf[0][RESULT] = CPU::decexe_buf[1][SREG] ^ CPU::decexe_buf[1][TREG];
			break;
	}
}

void ALU::addi(){
	CPU::exemem_buf[0][RESULT] = CPU::decexe_buf[1][TREG] + CPU::decexe_buf[1][IMM6];
}
