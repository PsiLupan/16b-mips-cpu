#include <array>
#include <cstdint>

#include "Alu.h"
#include "Memory.h"

class CPU {
private:
	static uint16_t fetchInstr();
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
	static uint16_t fetchdec_buf[2];
	static uint16_t decexe_buf[2][7];
	static uint16_t exewb_buf[2][3];

	static uint16_t pc;
	static bool g_step;
};