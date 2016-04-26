#include <array>
#include <cstdint>

namespace Memory {
	static std::array<uint8_t, 128> instr; //256 bytes of instruction memory
	static std::array<uint8_t, 64> data; //64 bytes of data memory

	static std::array<uint16_t, 8> registers;

	/*For all arrays: index 0 is new, index 1 is old*/
	static uint16_t fetch[7]; //SREG 0, TREG 1, DREG 2, SHFUNC 3, IMM6 4, IMM12 5, OPCODE 6
	static uint16_t decode[2][9]; //For decode[1]: SREG VAL 0, TREG VAL 1, DREG 2, SHFUNC 3, IMM6 4, IMM12 5, OPCODE 6, SREG 7, TREG 8
	static uint16_t exec[2][9];
	static uint16_t mem[2][9];
	static uint16_t write[9];
}