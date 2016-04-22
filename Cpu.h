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

	static void resolve();

	enum CPU_STATE {
		RUN, EXIT
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