#include <cstdint>
#include <math.h>

//EXEC 0 Buffer Indices
#define SREGVAL 0
#define TREGVAL 1
#define DREG 2
#define SHFUNC 3
#define IMM6 4
#define IMM12 5
#define OPCODE 6
#define SREG 7
#define TREG 8

//EXEC 1 Buffer Indices
#define RESULT 0

namespace ALU {
	void runWithFunc(uint16_t exec[2][9]);
	void addi(uint16_t exec[2][9]);
	void slt(uint16_t exec[2][9]);
	void sll(uint16_t exec[2][9]);
	void sra(uint16_t exec[2][9]);
	void srl(uint16_t exec[2][9]);
	void mem(uint16_t exec[2][9]);
}