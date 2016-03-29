#include <array>
#include <cstdint>

class CPU {
private:
	static uint16_t fetchInstr();
	static void decodeInstr(uint16_t instr);

	enum CPU_STATE {
		RUN, STEP, EXIT
	};
	static CPU_STATE state;

public:
	void Init();
	void Shutdown();
	void Run();
	static CPU* getInstance();

	static std::array<uint16_t, 8> registers;

	static uint16_t pc;
};