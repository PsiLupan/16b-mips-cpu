#include <array>
#include <cstdint>

#include "Memory.h"

/*Decode[1] and beyond*/
#define SREGVAL 0
#define TREGVAL 1
#define DREG 2
#define SHFUNC 3
#define IMM6 4
#define IMM12 5
#define OPCODE 6
#define SREG 7
#define TREG 8

/*Specifically for Exec[1] and beyond*/
#define RESULT 0

class CPU {
private:
	static void swapBuffers();

	static void fetchInstr();
	static void decodeInstr();
	static void execute();
	static void memory();
	static void writeback();

	static void resolve();

	enum CPU_STATE {
		RUN, EXIT, BADEXIT
	};
	static CPU_STATE state;

public:
	void Init();
	void PrintState();
	void Shutdown();
	void Run();
	static CPU* getInstance();

	static uint16_t pc;
	static bool g_step;
};