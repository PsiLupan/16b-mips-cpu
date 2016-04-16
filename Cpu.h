#include <array>
#include <cstdint>

#include "Alu.h"
#include "Memory.h"

class CPU {
private:
	static void swapBuffers();

	static void fetchInstr();
	static void decodeInstr();
	static void execute();
	static void memory();
	static void writeback();

	enum CPU_STATE {
		RUN, EXIT
	};
	static CPU_STATE state;

public:
	void Init();
	void Shutdown();
	void Run();
	static CPU* getInstance();

	static std::array<uint16_t, 8> registers;
	/*For all arrays: index 0 is new, index 1 is old*/

	static uint16_t pc;
	static bool g_step;
};

namespace Buffer {
	static uint16_t fetch[7]; //SREG 0, TREG 1, DREG 2, SHFUNC 3, IMM6 4, IMM12 5, OPCODE 6
	static uint16_t decode[2][9]; //For decode[1]: SREG VAL 0, TREG VAL 1, DREG 2, SHFUNC 3, IMM6 4, IMM12 5, OPCODE 6, SREG 7, TREG 8
	static uint16_t exec[2][9];
	static uint16_t memory[2][9];
	static uint16_t write[2][9];
}