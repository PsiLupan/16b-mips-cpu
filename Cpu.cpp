#include "Cpu.h"

const std::array<uint16_t, 45> program = {
	0x149F, // addi $v0, $v0, 31
	0xF000, // nop - Inserted based on requirement to use the instruction
	0xF000, // nop ^
	0x149F, // addi $v0, $v0, 31
	0x1482, // addi $v0, $v0, 2
	0x1D90, // addi $a0, $a0, 16
	0x0798, // add $v1, $v1, $a1
	0x30DF, // sll $v1, $v1, 7
	0x30D9, // sll $v1, $v1, 1
	0x16D0, // addi $v1, $v1, 16
	0x190F, // addi $v2, $v2, 15
	0x316C, // sll $v3, $v2, 4
	0x1FC5, // addi $a1, $a1, 5
	//WHILE:
	0x8E00, // blez $a1, EXIT (PC + 2 + Offset() << 2) = PC relative location
	0x024C, // xor $t1, $t1, $t1
	0x1241, // addi $t1, $t1, 1
	0x0E79, // sub $a1, $a1, $t1
	0x7180, // lw $t0, 0($a0)	
	0x024C, // xor $t1, $t1, $t1
	0x1250, // addi $t1, $t1, 16
	0x304C, // sll $t1, $t1, 4
	0x9000, // beq $t0, $t1, ELSE
	0x2208, // slt $t1, $t1, $t0
	0x8202, // blez $t1, CONT (PC + 2 + Offset() << 2) = PC relative location
	0xA000, // j ELSE
	//CONT:
	0x5093, // srl $v0, $v0, 3
	0x069B, // or $v1, $v1, $v0
	0x024C, // xor $t1, $t1, $t1
	0x125E, // addi $t1, $t1, 30
	0x304B, // sll $t1, $t1, 3
	0x124F, // addi $t1, $t1, 15
	0x304F, // sll $t1, $t1, 7
	0x3049, // sll $t1, $t1, 1
	0x6A40, // sw $t1, 0($a0)
	0xA05C, // j PELSE (PC + 2)|Upper 4b + Offset(92) = Address
	//ELSE:
	0x4122, // sla $v2, $v2, 2
	0x0B2C, // xor $v3, $v3, $v2
	0x024C, // xor $t1, $t1, $t1
	0x125E, // addi $t1, $t1, 30
	0x304B, // sll $t1, $t1, 3
	0x124F, // addi $t1, $t1, 15
	0x6A40, // sw $t1, 0($a0)
	//PELSE:
	0x1D82, // addi $a0, $a0, 2
	0xA018, // j WHILE
	//EXIT:
	0xFFFF // This is a psuedo-instruction that will signal the end of the code for our processor
};

void CPU::Init(){
	//Set our process counter to 0
	pc = 0;

	//Initialize registers to 0
	registers.fill(0);
	
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
	g_step = false;

	state = RUN;

	Run();
}

void CPU::Shutdown(){
	//Print our final registers, memory, etc. here
}

void CPU::Run(){
	while (true){
		switch (state){
			case RUN:
				fetchdec_buf[0] = fetchInstr(); // IF/ID_new
				decodeInstr();
				control();
				writeback();
				
				if (g_step){
					getchar(); //Wait for the user to input something before we step again
				}
				break;
			case EXIT:
				return; //Exit our loop
		}
	}
	Shutdown();
}

/*Instruction Fetch*/
uint16_t CPU::fetchInstr(){
	uint16_t instr = Memory::instr[pc];
	pc += 1; // We're incrementing by 1 here, because we work with 2 bytes instead of 1 at a time
	return instr;
}

/* Instruction Decode
R-type: 4b op, 3b rs, 3b rt, 3b rt, 3b func
I-type: 4b op, 3b rs, 3b rt, 6b imm
J-type: 4b op, 12b imm
*/
void CPU::decodeInstr(){
	uint16_t instr = fetchdec_buf[0];
	uint16_t opcode = (instr & 0xFF00) >> 12;
	
	uint16_t sreg = (instr & 0x0E00) >> 9;
	uint16_t treg = (instr & 0x01C0) >> 6;
	uint16_t dreg = (instr & 0x0038) >> 3;
	uint16_t shfunc = instr & 0x0007;
	uint16_t imm6 = instr & 0x003F;
	uint16_t imm12 = instr & 0x0FFF;

	fetchdec_buf[1] = instr; //Update IF/ID_old


	decexe_buf[0][0] = registers[sreg];
	decexe_buf[0][1] = registers[treg];
	decexe_buf[0][2] = registers[dreg];
	decexe_buf[0][3] = shfunc;
	decexe_buf[0][4] = imm6;
	decexe_buf[0][5] = imm12;
	decexe_buf[0][6] = opcode;
}

/*Control Unit*/
void CPU::control(){
	switch (decexe_buf[0][6]){ //Index 6 = Opcode
	case 0x0: //ALU Func
		ALU::runWithFunc();
		break;
	case 0x1: //ADDI
		ALU::addi();
		break;
	case 0x2: //SLT
		break;
	case 0x3: //SLL
		break;
	case 0x4: //SLA
		break;
	case 0x5: //SRL
		break;
	case 0x6: //SW
		break;
	case 0x7: //LW
		break;
	case 0x8: //BLEZ
		break;
	case 0x9: //BEQ
		break;
	case 0xA: //J
		break;
	case 0xF: //NOP or our pseudo-end instruction
		if (decexe_buf[0][5] != 0){ //Assume that if IMM12 if anything other than 0, it's our pseudo-instruction
			state = EXIT;
		}
		return;
	default:
		//Throw some kind of Invalid Instruction error, print the PC & instruction
		break;
	}
}

/*Writeback Stage*/
void CPU::writeback(){

}


CPU *CPU::getInstance(){
	static CPU instance;
	return &instance;
}