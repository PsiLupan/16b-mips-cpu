#include "Alu.h"
#include "Cpu.h"

//Buffer "age"
#define NEW 0
#define OLD 1

//exewb Buffer Indices
#define RESULT 0

//decexe Buffer Indices
#define SREG 0
#define TREG 1
#define DREG 2
#define SHFUNC 3
#define IMM6 4
#define IMM12 5

void ALU::updateBuf(){
	for (int i = 0; i < 6; i++){
		CPU::decexe_buf[OLD][i] = CPU::decexe_buf[NEW][i]; //Set the old buf
	}
}

void ALU::runWithFunc(){
	switch (CPU::decexe_buf[NEW][SHFUNC]){
		case 0: //ADD
			CPU::exewb_buf[NEW][RESULT] = CPU::decexe_buf[NEW][SREG] + CPU::decexe_buf[NEW][TREG];
			break;
		case 1: //SUB
			CPU::exewb_buf[NEW][RESULT] = CPU::decexe_buf[NEW][SREG] - CPU::decexe_buf[NEW][TREG];
			break;
		case 2: //AND
			CPU::exewb_buf[NEW][RESULT] = CPU::decexe_buf[NEW][SREG] & CPU::decexe_buf[NEW][TREG];
			break;
		case 3: //OR
			CPU::exewb_buf[NEW][RESULT] = CPU::decexe_buf[NEW][SREG] | CPU::decexe_buf[NEW][TREG];
			break;
		case 4: //XOR
			CPU::exewb_buf[NEW][RESULT] = CPU::decexe_buf[NEW][SREG] ^ CPU::decexe_buf[NEW][TREG];
			break;
	}
	updateBuf();
}

void ALU::addi(){
	CPU::exewb_buf[NEW][RESULT] = CPU::decexe_buf[NEW][TREG] + CPU::decexe_buf[NEW][IMM6];
	updateBuf();
}
