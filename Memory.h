#include <array>
#include <cstdint>

namespace Memory {
	static std::array<uint16_t, 64> instr; //256 bytes of instruction memory
	static std::array<uint16_t, 64> data; //256 bytes of data memory
}