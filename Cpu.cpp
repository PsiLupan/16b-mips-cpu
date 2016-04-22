#include "Cpu.h"

const std::array<uint16_t, 53> program = {
	0x149F, // addi $v0, $v0, 31
	0x1FC5, // addi $a1, $a1, 5
	0x1D90, // addi $a0, $a0, 16
	0x0798, // add $v1, $v1, $a1 //TODO: FIX ME
	0x1482, // addi $v0, $v0, 2
	0x30DF, // sll $v1, $v1, 7
	0x149F, // addi $v0, $v0, 31
	0x30D9, // sll $v1, $v1, 1
	0x190F, // addi $v2, $v2, 15
	0x316C, // sll $v3, $v2, 4
	0x16D0, // addi $v1, $v1, 16
	0x8E09, // //WHILE: blez $a1, EXIT (PC + 2 + Offset() << 2) = PC relative location
	0xF000, // nop
	0xF000, // nop
	0x024C, // xor $t1, $t1, $t1
	0x1241, // addi $t1, $t1, 1
	0x0E79, // sub $a1, $a1, $t1
	0x7180, // lw $t0, 0($a0)	
	0x024C, // xor $t1, $t1, $t1
	0x1250, // addi $t1, $t1, 16
	0x304C, // sll $t1, $t1, 4
	0x9000, // beq $t0, $t1, ELSE
	0x2208, // slt $t1, $t1, $t0
	0xF000, // nop
	0xF000, // nop
	0x8204, // blez $t1, CONT (PC + 2 + Offset() << 2) = PC relative location
	0xF000, // nop
	0xF000, // nop
	0xA047, // j ELSE
	0x5093, // CONT: srl $v0, $v0, 3
	0x069B, // or $v1, $v1, $v0
	0x024C, // xor $t1, $t1, $t1
	0x125E, // addi $t1, $t1, 30
	0x304B, // sll $t1, $t1, 3
	0x124F, // addi $t1, $t1, 15
	0x304F, // sll $t1, $t1, 7
	0x3049, // sll $t1, $t1, 1
	0x6A40, // sw $t1, 0($a0)
	0xF000, // nop
	0xF000, // nop
	0xA05C, // j PELSE (PC + 2)|Upper 4b + Offset(92) = Address
	0x3122, // ELSE: sll $v2, $v2, 2
	0x0B2C, // xor $v3, $v3, $v2
	0x024C, // xor $t1, $t1, $t1
	0x125E, // addi $t1, $t1, 30
	0x304B, // sll $t1, $t1, 3
	0x124F, // addi $t1, $t1, 15
	0x6A40, // sw $t1, 0($a0)
	//PELSE:
	0x1D82, // addi $a0, $a0, 2
	0xF000, // nop
	0xF000, // nop
	0xA018, // j WHILE
	0xFFFF //EXIT: This is a psuedo-instruction that will signal the end of the code for our processor
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
	printf("|     PC: %X | Instr: %X |\n", pc - 1, Memory::instr[pc - 1]);
	printf("--------------------------\n");

	printf("-----------------\t----------------\n");
	printf("|     MEMORY    |\t| REGISTER FILE |\n");
	printf("-----------------\t----------------\n");
	for (int i = 0; i < 64; i++) {
		if (i < 8) {
			printf("| [%X] %X \t|\t| $R%d: %d \t|\n", i, Memory::data[i], i, Memory::registers[i]);
		}
		else if (i == 8) {
			printf("| [%X] %X \t|\t----------------\n", i, Memory::data[i]);
		}
		else {
			printf("| [%X] %X \t|\n", i, Memory::data[i]);
		}
	}

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
	Memory::decode[0][8] = 0;
	Memory::decode[0][9] = 0;

	for (int i = 0; i < 9; i++) {
		Memory::exec[0][i] = Memory::decode[1][i];
		Memory::mem[0][i] = Memory::exec[1][i];
		Memory::write[0][i] = Memory::mem[1][i];
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
R-type: 4b op, 3b rs, 3b rt, 3b rt, 3b func
I-type: 4b op, 3b rs, 3b rt, 6b imm
J-type: 4b op, 12b imm
*/
void CPU::fetchInstr(){
	uint16_t instr = Memory::instr[pc];

	Memory::fetch[0] = (instr & 0x0E00) >> 9; //SREG
	Memory::fetch[1] = (instr & 0x01C0) >> 6; //TREG
	Memory::fetch[2] = (instr & 0x0038) >> 3; //DREG
	Memory::fetch[3] = instr & 0x0007; //SHFUNC
	Memory::fetch[4] = instr & 0x003F; //IMM6
	Memory::fetch[5] = instr & 0x0FFF; //IMM12
	Memory::fetch[6] = (instr & 0xFF00) >> 12; //OPCODE
	pc += 1; // We're incrementing by 1 here, because we work with 2 bytes instead of 1 at a time
}

/* Instruction Decode
*/
void CPU::decodeInstr(){

	Memory::decode[1][0] = Memory::registers[Memory::decode[0][0]]; // SREG Val
	Memory::decode[1][1] = Memory::registers[Memory::decode[0][1]]; // TREG Val
	Memory::decode[1][2] = Memory::decode[0][2]; // DREG
	Memory::decode[1][3] = Memory::decode[0][3];
	Memory::decode[1][4] = Memory::decode[0][4];
	Memory::decode[1][5] = Memory::decode[0][5];
	Memory::decode[1][6] = Memory::decode[0][6];
	Memory::decode[1][7] = Memory::decode[0][0]; //SREG
	Memory::decode[1][8] = Memory::decode[0][1]; //TREG
}

/*Execution*/
void CPU::execute(){

	Memory::exec[0][0] = Memory::registers[Memory::exec[0][7]]; //For some reason, this wasn't working in decode stage, so we do it again in exec
	Memory::exec[0][1] = Memory::registers[Memory::exec[0][8]];
	for (int i = 0; i < 9; i++){
		Memory::exec[1][i] = Memory::exec[0][i];
	}

	switch (Memory::exec[1][6]){ //Index 6 = Opcode
	case 0x0: //ALU Func
		ALU::runWithFunc(Memory::exec);
		break;
	case 0x1: //ADDI
		ALU::addi(Memory::exec);
		break;
	case 0x2: //SLT
		ALU::slt(Memory::exec);
		break;
	case 0x3: //SLL
		ALU::sll(Memory::exec);
		break;
	case 0x4: //SRA
		/*Our instructions currently do not use this, so it will not fire.*/
		ALU::sra(Memory::exec);
		break;
	case 0x5: //SRL
		ALU::srl(Memory::exec);
		break;
	case 0x6: //SW
	case 0x7: //LW
		ALU::mem(Memory::exec);
		break;
	case 0x8: //BLEZ
		if ((int16_t)Memory::exec[0][0] <= 0) { //SREGV <= 0
			pc = pc + (Memory::exec[0][4] << 2); //pc = pc + imm6 << 2
		}
		break;
	case 0x9: //BEQ
		if (Memory::exec[0][0] == Memory::exec[0][1]) { //SREGV == TREGV
			pc = pc + (Memory::exec[0][4] << 2); //pc = pc + imm6 << 2
		}
		break;
	case 0xA: //J
		pc = (pc & 0xf000) | ((int16_t)Memory::exec[0][5] /*<< 2*/); //nPC = (PC & 0xf000) | (target << 2);
		break;
	case 0xF: //NOP or our pseudo-end instruction
		if (Memory::exec[0][5] != 0){ //Assume that if IMM12 if anything other than 0, it's our pseudo-instruction
			state = EXIT;
		}
		return;
	default:
		//Throw some kind of Invalid Instruction error, print the PC & instruction
		break;
	}
}

/*Memory Stage*/
void CPU::memory() {
	for (int i = 0; i < 9; i++){
		Memory::mem[1][i] = Memory::mem[0][i];
	}

	/*Instead of using Control Bits, we'll just act with the current OPCode*/
	switch (Memory::mem[0][6]){
		case 0x6: //SW
			Memory::data[Memory::mem[0][0]] = Memory::mem[0][1];
			break;
		case 0x7: //LW
			Memory::mem[1][0] = Memory::data[Memory::mem[0][0]];
			break;	
	}
}

/*Writeback Stage*/
void CPU::writeback(){
	/*Copy WB to the last buffer to be used by forwarding*/
	for (int i = 0; i < 9; i++){
		Memory::write[1][i] = Memory::write[0][i];
	}

	if (Memory::write[0][6] == 0x7){
		Memory::registers[Memory::write[0][8]] = Memory::write[0][0]; //TREG = Result
	}
	else if (Memory::write[0][6] == 0x0){
		Memory::registers[Memory::write[0][2]] = Memory::write[0][0]; //DREG = Result
	}
	else if (Memory::write[0][6] <= 0x5){
		Memory::registers[Memory::write[0][8]] = Memory::write[0][0]; //TREG = Result
	}
}

void CPU::resolve(){
	/*IF/ID Forward*/
	if (Memory::decode[1][6] == 0x07 //If OPCode == LW, it's our only mem_read
		&& (Memory::decode[1][2] == Memory::fetch[0] || Memory::decode[1][2] == Memory::fetch[1])){
		for (int i = 0; i < 7; i++){
			Memory::fetch[i] = 0;
		}
		Memory::fetch[6] = 0xF;
	}

	/*ID/EX Forward*/
	if (Memory::exec[1][6] <= 0x05){ /*If OPCode <= 0x05, it's a reg_write*/
		if (Memory::exec[1][7] == Memory::decode[1][7]){ /*If exec SREG == decode SREG*/
			Memory::decode[1][0] = Memory::exec[1][0]; /*SREGVAL = ALUResult*/
		}
		else if (Memory::exec[1][8] == Memory::decode[1][8]){  /*If exec TREG == decode TREG*/
			Memory::decode[1][1] = Memory::exec[1][0];  /*TREGVAL = ALUResult*/
		}
	}

	/*Mem Forward*/
	if (Memory::mem[1][6] <= 0x06){ /*If OPCode <= 0x06, it's a reg_write*/
		if (Memory::exec[1][2] != Memory::decode[1][7] && Memory::mem[1][2] == Memory::decode[1][7]){ //DREG == SREG
			if (Memory::mem[1][6] == 0x07){ //If OPCode == LW (mem_read)
				Memory::decode[1][0] = Memory::mem[1][0];
			}
			else{
				Memory::decode[1][0] = Memory::mem[1][1];
			}
		}
		else if (Memory::exec[1][2] != Memory::decode[1][8] && Memory::mem[1][2] == Memory::decode[1][8]){ //DREG == TREG
			if (Memory::mem[1][6] == 0x07){ //If OPCode == LW (mem_read)
				Memory::decode[1][1] = Memory::mem[1][0];
			}
			else{
				Memory::decode[1][1] = Memory::mem[1][1];
			}
		}
	}
}

CPU *CPU::getInstance(){
	static CPU instance;
	return &instance;
}