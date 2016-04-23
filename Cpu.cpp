#include "Cpu.h"

const std::array<uint8_t, 126> program = {
	0x1D, 0x90, // addi $a0, $a0, 16
	0x14, 0x9F, // addi $v0, $v0, 31
	0x1F, 0xC5, // addi $a1, $a1, 5
	0x07, 0x98, // add $v1, $v1, $a0
	0x19, 0x0F, // addi $v2, $v2, 15
	0x14, 0x9F, // addi $v0, $v0, 31
	0x30, 0xDF, // sll $v1, $v1, 7
	0x31, 0x6C, // sll $v3, $v2, 4
	0x30, 0xD9, // sll $v1, $v1, 1
	0x14, 0x82, // addi $v0, $v0, 2
	0x16, 0xD0, // addi $v1, $v1, 16
	0x8E, 0x06, // //WHILE: blez $a1, EXIT (PC + (Offset() << 2) + 2) = PC relative location
	0xF0, 0x00, // nop
	0xF0, 0x00, // nop
	0x02, 0x4C, // xor $t1, $t1, $t1
	0x12, 0x41, // addi $t1, $t1, 1
	0x0E, 0x79, // sub $a1, $a1, $t1
	0x71, 0x80, // lw $t0, 0($a0)
	0x02, 0x4C, // xor $t1, $t1, $t1
	0x12, 0x50, // addi $t1, $t1, 16
	0x30, 0x4C, // sll $t1, $t1, 4
	0x90, 0x04, // beq $t0, $t1, ELSE
	0x22, 0x08, // slt $t1, $t1, $t0
	0x82, 0x01, // blez $t1, CONT (PC + (Offset() << 2) + 2) = PC relative location
	0xF0, 0x00, // nop
	0xF0, 0x00, // nop
	0xA0, 0x25, // j ELSE
	0x50, 0x93, // CONT: srl $v0, $v0, 3
	0x06, 0x9B, // or $v1, $v1, $v0
	0x02, 0x4C, // xor $t1, $t1, $t1
	0x12, 0x5E, // addi $t1, $t1, 30
	0x30, 0x4B, // sll $t1, $t1, 3
	0x12, 0x4F, // addi $t1, $t1, 15
	0x30, 0x4F, // sll $t1, $t1, 7
	0x30, 0x49, // sll $t1, $t1, 1
	0x6A, 0x40, // sw $t1, 0($a0)
	0xF0, 0x00, // nop
	0xF0, 0x00, // nop
	0xA0, 0x32, // j PELSE (PC + 2)|Upper 4b + Offset() = Address
	0x31, 0x22, // ELSE: sll $v2, $v2, 2
	0x0B, 0x2C, // xor $v3, $v3, $v2
	0x02, 0x4C, // xor $t1, $t1, $t1
	0x12, 0x5E, // addi $t1, $t1, 30
	0x30, 0x4B, // sll $t1, $t1, 3
	0x12, 0x4F, // addi $t1, $t1, 15
	0x6A, 0x40, // sw $t1, 0($a0)
	0x1D, 0x82, // PELSE: addi $a0, $a0, 2
	0xF0, 0x00, // nop
	0xF0, 0x00, // nop
	0xA0, 0x28, // j WHILE
	0xF0, 0x00, // nop
	0xFF, 0xFF //EXIT: This is a psuedo-instruction that will signal the end of the code for our processor
};

uint16_t CPU::pc;
bool CPU::g_step = false;

CPU::CPU_STATE CPU::state;

void CPU::Init(){

	//Set our process counter to 0
	pc = 0;

	//Initialize registers to 0
	Memory::registers.fill(0);
	
	//Copy our program into instruction memory
	std::copy(std::begin(program), std::end(program), std::begin(Memory::instr));
	
	//Initialize our memory to 0 and setup the data for $a1 in memory
	Memory::data.fill(0);
	Memory::data[5] = 0x0101;
	Memory::data[6] = 0x0110;
	Memory::data[7] = 0x0011;
	Memory::data[8] = 0x00F0;
	Memory::data[9] = 0x00FF;

	//Set step based on user input
	g_step = true;

	state = RUN;

	Run();
}

void CPU::PrintState() {
	system("cls"); //Clearing console with Windows only function. If compiled elsewhere, it will fail.
	printf("--------------------------\n");
	printf("|     PC: %X | Instr: %X%X |\n", pc - 2, Memory::instr[pc - 2], Memory::instr[pc - 1]);
	printf("--------------------------\n");
	if (pc >= 8) {
		printf("------------------------------\n");
		printf("|     WBPC: %X | Instr: %X%X |\n", pc - 8, Memory::instr[pc - 8], Memory::instr[pc - 7]);
		printf("------------------------------\n");
	}
	else {
		printf("------------------------------\n");
		printf("|     WBPC: 0 | Instr: 0      |\n");
		printf("------------------------------\n");
	}

	printf("-----------------\t-------------------------\n");
	printf("|     MEMORY    |\t|   REGISTER FILE \t|\n");
	printf("-----------------\t-------------------------\n");
	for (int i = 0; i < 32; i++) {
		if (i < 8) {
			printf("| [%X] %X \t|\t| $R%d: %X(%d) \t|\n", i * 2, Memory::data[i], i, Memory::registers[i], Memory::registers[i]);
		}
		else if (i == 8) {
			printf("| [%X] %X \t|\t-------------------------\n", i, Memory::data[i]);
		}
		else {
			printf("| [%X] %X \t|\n", i, Memory::data[i]);
		}
	}

	printf("\nPress any key to continue");
	getchar(); //Wait for the user to input something before we cont
}

void CPU::Shutdown(){
	PrintState();
}

void CPU::swapBuffers() {
	/*Move Fetch results to Buffer new*/
	for (int i = 0; i < 7; i++) {
		Memory::decode[0][i] = Memory::fetch[i];
	}
	Memory::decode[0][7] = 0;
	Memory::decode[0][8] = 0;

	for (int i = 0; i < 9; i++) {
		Memory::exec[0][i] = Memory::decode[1][i];
		Memory::mem[0][i] = Memory::exec[1][i];
		Memory::write[i] = Memory::mem[1][i];
	}
}

void CPU::Run(){
	while (true){
		switch (state){
			case RUN:
				memory();
				writeback();
				fetchInstr();
				decodeInstr();
				execute();

				resolve();
				
				if (g_step){
					PrintState();
				}

				swapBuffers();
				break;
			case EXIT:
				Shutdown();
				return; //Exit our loop
		}
	}
}

/*Instruction Fetch
R-type: 4b op, 3b rs, 3b rt, 3b rd, 3b func
I-type: 4b op, 3b rs, 3b rt, 6b imm
J-type: 4b op, 12b imm
*/
void CPU::fetchInstr(){
	uint16_t inshi = Memory::instr[pc] << 8;
	uint16_t inslo = Memory::instr[pc + 1];
	
	uint16_t instr = inshi | inslo;

	Memory::fetch[0] = (instr & 0x0E00) >> 9; //SREG
	Memory::fetch[1] = (instr & 0x01C0) >> 6; //TREG
	Memory::fetch[2] = (instr & 0x0038) >> 3; //DREG
	Memory::fetch[3] = instr & 0x0007; //SHFUNC
	Memory::fetch[4] = instr & 0x003F; //IMM6
	Memory::fetch[5] = instr & 0x0FFF; //IMM12
	Memory::fetch[6] = (instr & 0xFF00) >> 12; //OPCODE
	
	pc = pc + 2;
}

/* Instruction Decode
*/
void CPU::decodeInstr(){
	uint16_t sreg = Memory::decode[0][0];
	uint16_t treg = Memory::decode[0][1];

	Memory::decode[1][SREGVAL] = Memory::registers[sreg];
	Memory::decode[1][TREGVAL] = Memory::registers[treg];
	Memory::decode[1][DREG] = Memory::decode[0][2];
	Memory::decode[1][SHFUNC] = Memory::decode[0][3];
	Memory::decode[1][IMM6] = Memory::decode[0][4];
	Memory::decode[1][IMM12] = Memory::decode[0][5];
	Memory::decode[1][OPCODE] = Memory::decode[0][6];
	Memory::decode[1][SREG] = Memory::decode[0][0];
	Memory::decode[1][TREG] = Memory::decode[0][1];
}

/*Execution*/
void CPU::execute(){
	for (int i = 0; i < 9; i++) {
		Memory::exec[1][i] = Memory::exec[0][i];
	}

	switch (Memory::exec[0][OPCODE]){
	
	case 0x0: //ALU Func
		switch (Memory::exec[0][SHFUNC]) {
			case 0: //ADD
				Memory::exec[1][RESULT] = (int16_t)Memory::exec[0][SREGVAL] + (int16_t)Memory::exec[0][TREGVAL];
				break;
			case 1: //SUB
				Memory::exec[1][RESULT] = (int16_t)Memory::exec[0][SREGVAL] - (int16_t)Memory::exec[0][TREGVAL];
				break;
			case 2: //AND
				Memory::exec[1][RESULT] = Memory::exec[0][SREGVAL] & Memory::exec[0][TREGVAL];
				break;
			case 3: //OR
				Memory::exec[1][RESULT] = Memory::exec[0][SREGVAL] | Memory::exec[0][TREGVAL];
				break;
			case 4: //XOR
				Memory::exec[1][RESULT] = Memory::exec[0][SREGVAL] ^ Memory::exec[0][TREGVAL];
				break;
			default:
				printf("Invalid ALU Function");
				state = EXIT;
				return;
		}
		break;
	
	case 0x1: //ADDI
		Memory::exec[1][RESULT] = (int16_t)Memory::exec[0][SREGVAL] + (int16_t)Memory::exec[0][IMM6];
		break;
	
	case 0x2: //SLT
		Memory::exec[1][RESULT] = (int16_t)Memory::exec[0][TREGVAL] < (int16_t)Memory::exec[0][SREGVAL];
		break;
	
	case 0x3: //SLL
		Memory::exec[1][RESULT] = Memory::exec[0][TREGVAL] << Memory::exec[0][SHFUNC];
		break;
	
	case 0x4: //SRA
		/*Our instructions currently do not use this, so it will not fire.
		This relies on the fact that Visual Studio compiles a right-shift as a an arithmatic shift, if it's a signed value.
		Therefore, this may not work if you compile with another compiler.*/
		Memory::exec[1][RESULT] = (int16_t)Memory::exec[0][TREGVAL] >> Memory::exec[0][SHFUNC];
		break;
	
	case 0x5: //SRL
		Memory::exec[1][RESULT] = Memory::exec[0][TREGVAL] >> Memory::exec[0][SHFUNC];
		break;
	
	case 0x6: //SW
	case 0x7: //LW
		Memory::exec[1][RESULT] = (int16_t)Memory::exec[0][SREGVAL] + (int16_t)Memory::exec[0][IMM6];
		break;
	
	case 0x8: //BLEZ
		if ((int16_t)Memory::exec[0][SREGVAL] <= 0) { //SREGV <= 0
			pc = pc + ((int16_t)Memory::exec[0][IMM6] << 2) + 2; //pc = pc + imm6 << 2
		}
		break;
	
	case 0x9: //BEQ
		if (Memory::exec[0][SREGVAL] == Memory::exec[0][TREGVAL]) { //SREGV == TREGV
			pc = pc + ((int16_t)Memory::exec[0][IMM6] << 2) + 2;
		}
		break;
	
	case 0xA: //J
		pc = (pc & 0xF000) + ((int16_t)Memory::exec[0][IMM12]);
		break;
	
	case 0xF: //NOP or our pseudo-end instruction
		if (Memory::exec[0][IMM12] != 0){ //Assume that if IMM12 if anything other than 0, it's our pseudo-instruction
			state = EXIT;
		}
		break;
	
	default:
		printf("Invalid Instruction!");
		state = EXIT;
		break;
	}
}

/*Memory Stage*/
void CPU::memory() {
	for (int i = 0; i < 9; i++) {
		Memory::mem[1][i] = Memory::mem[0][i];
	}

	/*Instead of using Control Bits, we'll just act with the current OPCode*/
	switch (Memory::mem[0][OPCODE]){
		case 0x6: //SW
			Memory::data.at(Memory::mem[0][RESULT]) = Memory::mem[0][TREGVAL];
			break;
		case 0x7: //LW
			Memory::mem[1][RESULT] = Memory::data.at(Memory::mem[0][TREGVAL]);
			break;	
	}
}

/*Writeback Stage*/
void CPU::writeback(){
	if (Memory::write[OPCODE] == 0x0){ //ALU OP
		Memory::registers[Memory::write[DREG]] = Memory::write[RESULT]; //DREG = Result
	}
	else if (Memory::write[OPCODE] <= 0x5 || Memory::write[OPCODE] == 0x7){ //LW
		Memory::registers[Memory::write[TREG]] = Memory::write[RESULT]; //TREG = Result
	}
}

void CPU::resolve(){
	/*IF/ID Data Hazard*/
	if (Memory::decode[1][6] == 0x7 //If OPCode == LW, it's our only mem_read
		&& (Memory::decode[1][TREG] == Memory::fetch[0] || Memory::decode[1][TREG] == Memory::fetch[1])){
		for (int i = 0; i < 6; i++){
			Memory::fetch[i] = 0;
		}
		Memory::fetch[6] = 0xF;
		
		pc = pc - 4; //Go back 2 instruction
	}

	/*ID/EX Forward*/
	if (Memory::exec[1][OPCODE] <= 0x5 && Memory::exec[1][OPCODE] != 0x1){ //If OPCode <= 0x5 || 0x7 it's a reg_write
		if (Memory::exec[1][DREG] == Memory::decode[1][SREG]){
			Memory::decode[1][SREGVAL] = Memory::exec[1][RESULT];
		}
		else if (Memory::exec[1][DREG] == Memory::decode[1][TREG]){
			Memory::decode[1][TREGVAL] = Memory::exec[1][RESULT]; 
		}
	}
	else if (Memory::exec[1][OPCODE] == 0x1 || Memory::exec[1][OPCODE] == 0x7) {
		if (Memory::exec[1][TREG] == Memory::decode[1][SREG]) {
			Memory::decode[1][SREGVAL] = Memory::exec[1][RESULT];
		}
		else if (Memory::exec[1][TREG] == Memory::decode[1][TREG]) {
			Memory::decode[1][TREGVAL] = Memory::exec[1][RESULT];
		}
	}

	/*Mem Forward*/
	if (Memory::mem[1][OPCODE] <= 0x5 &&  Memory::mem[1][OPCODE] != 0x1){ //If OPCode <= 0x5 || 0x7, it's a reg_write
		if (Memory::exec[1][DREG] != Memory::decode[1][SREG] && Memory::mem[1][DREG] == Memory::decode[1][SREG]){
			Memory::decode[1][SREGVAL] = Memory::mem[1][0]; //SREGVAL = ALUResult
		}
		else if (Memory::exec[1][DREG] != Memory::decode[1][TREG] && Memory::mem[1][DREG] == Memory::decode[1][TREG]){
			Memory::decode[1][TREGVAL] = Memory::mem[1][0];
		}
	}
	else if (Memory::mem[1][OPCODE] == 0x1 || Memory::mem[1][OPCODE] == 0x7) {
		if (Memory::exec[1][TREG] != Memory::decode[1][SREG] && Memory::mem[1][TREG] == Memory::decode[1][SREG]) {
			if (Memory::mem[1][OPCODE] == 0x7) { //LW
				Memory::decode[1][SREGVAL] = Memory::mem[1][0]; //SREGVAL = Memory Data
			}
			else {
				Memory::decode[1][SREGVAL] = Memory::mem[1][0]; //SREGVAL = ALUResult
			}
		}
		else if (Memory::exec[1][TREG] != Memory::decode[1][TREG] && Memory::mem[1][TREG] == Memory::decode[1][TREG]) {
			if (Memory::mem[1][OPCODE] == 0x7) { //If OPCode == LW (mem_read)
				Memory::decode[1][TREGVAL] = Memory::mem[1][0];
			}
			else {
				Memory::decode[1][TREGVAL] = Memory::mem[1][0];
			}
		}
	}
}


CPU *CPU::getInstance(){
	static CPU instance;
	return &instance;
}