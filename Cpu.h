#include <array>
#include <cstdint>

#include "Alu.h"
#include "Memory.h"

class CPU {
private:
	static void fetchInstr();
	static void decodeInstr();
	static void control();
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
	static uint16_t fetchdec; //Instruction
	static uint16_t decexe[2][7]; //SREG 0, TREG 1, DREG 2, SHFUNC 3, IMM6 4, IMM12 5, OPCODE 6
	static uint16_t exemem[2][3]; //RESULT 0, Unk 1, Unk 2 TODO: Figure out what we may need here
	static uint16_t memwb[2];
}