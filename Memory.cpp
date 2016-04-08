#include "Memory.h"

/* Returns the content of Memory[addr / 2], since our addresses are a DWORD, instead of referencing a single WORD.
*/
uint16_t Memory::getContent(uint16_t addr){
	return Memory::data[addr / 2];
}

/* Stores the data at Memory[addr / 2].
*/
void Memory::storeContent(uint16_t addr, uint16_t data){
	Memory::data[addr / 2] = data;
}