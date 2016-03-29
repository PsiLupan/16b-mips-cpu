#include "Cpu.h"

void main(){
	CPU * const cpu = CPU::getInstance();
	cpu->Init();
}