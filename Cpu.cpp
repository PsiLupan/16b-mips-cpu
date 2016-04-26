#include "Cpu.h"

const std::array<uint8_t, 120> program = {
	0x1D, 0x90, //00 addi $a0, $a0, 16
	0x14, 0x9F, //02 addi $v0, $v0, 31
	0x07, 0x98, //04 add $v1, $v1, $a0
	0x19, 0x0F, //06 addi $v2, $v2, 15
	0x14, 0x9F, //08 addi $v0, $v0, 31
	0x30, 0xDF, //0A sll $v1, $v1, 7
	0x1B, 0x4F, //0C addi $v3, $v3, 15
	0x1F, 0xC5, //0E addi $a1, $a1, 5
	0x30, 0xD9, //10 sll $v1, $v1, 1
	0x31, 0x6C, //12 sll $v3, $v3, 4
	0x14, 0x82, //14 addi $v0, $v0, 2
	0x16, 0xD0, //16 addi $v1, $v1, 16
	0x8E, 0x14, //18 WHILE: blez $a1, EXIT (PC + (Offset(21) << 2) + 2) = 24 + (84) + 2 = 114 = 0x6E
	0xF0, 0x00, //1A nop
	0x02, 0x4C, //1C xor $t1, $t1, $t1
	0x12, 0x41, //1E addi $t1, $t1, 1
	0xF0, 0x00, //20 nop
	0x0E, 0x79, //22 sub $a1, $a1, $t1
	0x7C, 0x00, //24 lw $t0, 0($a0)
	0x02, 0x4C, //26 xor $t1, $t1, $t1
	0x12, 0x50, //28 addi $t1, $t1, 16
	0x30, 0x4C, //2A sll $t1, $t1, 4
	0x92, 0x09, //2C beq $t0, $t1, ELSE (PC + (Offset(9) << 2) + 2) = 44 + 40 + 2 = 86 = 0x56
	0xF0, 0x00, //2E nop
	0xF0, 0x00, //30 nop
	0x22, 0x08, //32 slt $t1, $t1, $t0
	0x82, 0x02, //34 blez $t1, CONT (PC + (Offset(2) << 2) + 2) = 52 + 8 + 2 = 62 = 0x3E
	0xF0, 0x00, //36 nop
	0xF0, 0x00, //38 nop
	0xA0, 0x58, //3A j ELSE (PC + 2)|Upper 4b + Offset(58) = 0x58
	0xF0, 0x00, //3C nop
	0xF0, 0x00, //3E nop
	0x50, 0x93, //40 CONT: srl $v0, $v0, 3
	0x06, 0x9B, //42 or $v1, $v1, $v0
	0x02, 0x4C, //44 xor $t1, $t1, $t1
	0x12, 0x5E, //46 addi $t1, $t1, 30
	0x30, 0x4B, //48 sll $t1, $t1, 3
	0x12, 0x4F, //4A addi $t1, $t1, 15
	0x30, 0x4F, //4C sll $t1, $t1, 7
	0x30, 0x49, //4E sll $t1, $t1, 1
	0x6A, 0x40, //50 sw $t1, 0($a0)
	0xA0, 0x66, //52 j PELSE (PC + 2)|Upper 4b + Offset(5E) = 0x66
	0xF0, 0x00, //54 nop
	0xF0, 0x00, //56 nop
	0x31, 0x22, //58 ELSE: sll $v2, $v2, 2
	0x0B, 0x2C, //5A xor $v3, $v3, $v2
	0x02, 0x4C, //5C xor $t1, $t1, $t1
	0x12, 0x5E, //5E addi $t1, $t1, 30
	0x30, 0x4B, //60 sll $t1, $t1, 3
	0x12, 0x4F, //62 addi $t1, $t1, 15
	0x6C, 0x40, //64 sw $t1, 0($a0)
	0x1D, 0x82, //66 PELSE: addi $a0, $a0, 2
	0xF0, 0x00, //68 nop
	0xF0, 0x00, //6A nop
	0xA0, 0x18, //6C j WHILE
	0xF0, 0x00, //6E nop
	0xF0, 0x00, //70 nop
	0xFF, 0xFF  //72 EXIT: This is a psuedo-instruction that will signal the end of the code for our processor
};

uint16_t CPU::pc;
bool CPU::g_step = false;

CPU::CPU_STATE CPU::state;

void CPU::Init() {

	//Set our process counter to 0
	pc = 0;

	//Initialize registers to 0
	Memory::registers.fill(0);

	//Copy our program into instruction memory
	std::copy(std::begin(program), std::end(program), std::begin(Memory::instr));

	//Initialize our memory to 0 and setup the data for $a1 in memory
	Memory::data.fill(0);
	Memory::data[16] = 0x01;
	Memory::data[17] = 0x01;
	Memory::data[18] = 0x01;
	Memory::data[19] = 0x10;
	Memory::data[20] = 0x00;
	Memory::data[21] = 0x11;
	Memory::data[22] = 0x00;
	Memory::data[23] = 0xF0;
	Memory::data[24] = 0x00;
	Memory::data[25] = 0xFF;

	printf("Enable Step Mode? (y/n then enter): ");
	char opt = getchar();
	PrintState(); //Print the initial memory as per project requirements
	getchar();
	if (opt == 'y'){
		g_step = true;
	}
	else {
		g_step = false;
	}

	state = RUN;
	Run();
}

void CPU::PrintState() {
	system("cls"); //Clearing console with Windows only function. If compiled elsewhere, it will fail.
	if (state == EXIT) {
		printf("------------FINISHED----------\n");
	}
	else if(pc == 0) {
		printf("-------------INITIAL----------\n");
	}
	else {
		printf("------------------------------\n");
	}
	printf("|     PC: %04X | Instr: %02X%02X |\n", pc, Memory::instr[pc], Memory::instr[pc + 1]);
	printf("------------------------------\n");

	printf("-------------------------\n");
	printf("| REGISTER FILE \t | \n");
	printf("-------------------------\n");
	for (int i = 0; i < 8; i++) {
		printf("R%d: %04X (%d) \t\n", i, Memory::registers[i], Memory::registers[i]);
	}
	printf("-------------------------\n");

	printf("-----------------|\n");
	printf("|     MEMORY     |\n");
	printf("-----------------|\n");
	for (int i = 0; i <= 32; i+=2) {
		printf("| [%X] %02X%02X \t|\n", i, Memory::data[i], Memory::data[i+1]);
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
			case BADEXIT:
				return;
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
				printf("ERROR: Invalid ALU Function at PC: %X\n", pc - 2);
				state = BADEXIT;
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
		printf("ERROR: Invalid Instruction at PC: %X\n", pc - 2);
		state = BADEXIT;
		break;
	}
}

/*Memory Stage*/
void CPU::memory() {
	for (int i = 0; i < 9; i++) {
		Memory::mem[1][i] = Memory::mem[0][i];
	}
	uint16_t dhi;
	uint16_t dlow;

	/*Instead of using Control Bits, we'll just act with the current OPCode*/
	switch (Memory::mem[0][OPCODE]){
		case 0x6: //SW
			dhi = Memory::mem[0][TREGVAL] >> 8;
			dlow = Memory::mem[0][TREGVAL] & 0x00FF;
			
			Memory::data.at(Memory::mem[0][RESULT]) = (uint8_t)dhi;
			Memory::data.at(Memory::mem[0][RESULT] + 1) = (uint8_t)dlow;
			break;
		case 0x7: //LW
			dhi = Memory::data.at(Memory::mem[0][TREGVAL]) << 8;
			dlow = Memory::data.at(Memory::mem[0][TREGVAL] + 1);
			
			Memory::mem[1][RESULT] = dhi | dlow;
			break;	
	}
}

/*Writeback Stage*/
void CPU::writeback() {
	if (Memory::write[OPCODE] == 0x1 || Memory::write[OPCODE] == 0x7) { //I - Type
		Memory::registers[Memory::write[TREG]] = Memory::write[RESULT]; //TREG = Result
	}
	else if (Memory::write[OPCODE] == 0x0 || Memory::write[OPCODE] <= 0x5) { //R - Type
		Memory::registers[Memory::write[DREG]] = Memory::write[RESULT]; //DREG = Result
	}
}

void CPU::resolve(){
	/*IF/ID Data Hazard*/
	if (Memory::decode[1][OPCODE] == 0x7 //If OPCode == LW, it's our only mem_read
		&& (Memory::decode[1][TREG] == Memory::fetch[0] || Memory::decode[1][TREG] == Memory::fetch[1])){
		for (int i = 0; i < 6; i++){
			Memory::fetch[i] = 0;
		}
		Memory::fetch[6] = 0xF;
		
		pc = pc - 4; //Go back 2 instruction
	}

	/*EX -> ID Forward*/
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

	/*Mem -> ID Forward*/
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