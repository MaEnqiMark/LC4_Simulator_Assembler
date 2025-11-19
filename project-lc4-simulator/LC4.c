/*
 * LC4.c: Defines simulator functions for executing instructions
 */

#include "LC4.h"
#include <stdio.h>
extern unsigned char isCODE[65535 + 1];
extern unsigned char isDATA[65535 + 1];
/*
 * Reset the machine state as Pennsim would do
 */
void Reset(MachineState* CPU)
{
		CPU->PC = 0x8200;
		CPU->PSR = 0x8002; // PSR[15] = 1, NZP = 010
		for(int i = 0; i <8; i++)CPU->R[i] = 0;
		ClearSignals(CPU);
}


/*
 * Clear all of the control signals (set to 0)
 */
void ClearSignals(MachineState* CPU)
{
		CPU->regFile_WE = 0;
		CPU->NZP_WE = 0;
		CPU->regInputVal = 0;
		CPU->DATA_WE = 0;
		CPU->rsMux_CTL = 0;
		CPU->rtMux_CTL = 0;
		CPU->rdMux_CTL = 0;
		CPU->NZPVal = 0;
		CPU->dmemAddr = 0;
		CPU->dmemValue = 0;
}


/*
 * This function should write out the current state of the CPU to the file output.
 */
void WriteOut(MachineState* CPU, FILE* output)
{
	fprintf(output, "%04X ", CPU->PC);
	unsigned short instr = CPU->memory[CPU->PC];
	for(int i = 15; i >= 0; i--) fprintf(output, "%d", (instr>>i) & 0x1 );
	fprintf(output, " %d", CPU->regFile_WE);
	if(((instr>> 12) & 0xF) == 0xF || ((instr>> 12) & 0xF) == 0x4){
		fprintf(output, " 7"); //trap calls
	}else{
		fprintf(output, " %d", CPU->regFile_WE ? ((instr >> 9)&0x7) : 0);
	}
	fprintf(output, " %04X", CPU->regFile_WE ? (CPU->regInputVal) : 0);
	fprintf(output, " %d", CPU->NZP_WE);
	fprintf(output, " %d", CPU->NZP_WE ? CPU->NZPVal : 0);
	if(((instr>> 12) & 0xF) == 0x6){
		fprintf(output, " 0"); //ldr call that has 0??? ed says mimic this
	}else{
		fprintf(output, " %d", CPU->DATA_WE);
	}
	fprintf(output, " %04X", CPU->DATA_WE ? CPU->dmemAddr :0);
	fprintf(output, " %04X\n", CPU->DATA_WE ? CPU->dmemValue :0);
}

void LDROp(MachineState* CPU, FILE* output){
	// if(isCODE[CPU->PC]){
	// 		printf("ERROR:treating code as data\n");
	// 		exit(-1);
	// }
	unsigned short instr = CPU->memory[CPU->PC];
	unsigned short rd = (instr>>9)&0x7;
	unsigned short rs =  (instr>>6)&0x7;
	short offset = instr & 0x3F;
	if(offset & 0x20) offset |= 0xFFC0;

	unsigned short addr = (CPU->R[rs] + offset) & 0xFFFF;
		if((CPU->PC & 0x8000) == 0 && addr > 0x8000){
			printf("ERROR:going into OS but not privilege\n");
			exit(-1);
		}
	CPU->regInputVal = CPU->memory[addr];
	CPU->R[rd] = CPU->regInputVal;
	CPU->regFile_WE = 1;
		CPU->DATA_WE = 1;
	CPU->dmemAddr = addr;
	CPU->dmemValue = CPU->regInputVal;
	SetNZP(CPU, CPU->regInputVal);
	WriteOut(CPU, output);
	CPU->PC = (CPU->PC + 1)&0xFFFF;
}

void STROp(MachineState* CPU, FILE* output){
	// if(isCODE[CPU->PC]){
	// 		printf("ERROR:treating code as data\n");
	// 		exit(-1);
	// }

	unsigned short instr = CPU->memory[CPU->PC];
	unsigned short rt = (instr>>9)&0x7;
	unsigned short rs =  (instr>>6)&0x7;
	short offset = instr & 0x3F;
	if(offset & 0x20) offset |= 0xFFC0;

	short value = CPU->R[rt];
	unsigned short addr = (CPU->R[rs] + offset) & 0xFFFF;

		if((CPU->PC & 0x8000) == 0 && addr > 0x8000){
			printf("ERROR:going into OS but not privilege\n");
			exit(-1);
		}

	CPU->memory[addr] = value;
	CPU->DATA_WE = 1;
	CPU->dmemAddr = addr;
	CPU->dmemValue = value;
	WriteOut(CPU, output);
	CPU->PC = (CPU->PC + 1)&0xFFFF;
}

void RTIOp(MachineState* CPU, FILE* output){
	if((CPU->PSR & 0x8000)==0){
		printf("RTI used in usermode");
		exit(1);
	}
	CPU->PSR = CPU->PSR & 0x7FFF;
	WriteOut(CPU, output);
	CPU->PC = CPU->R[7];
}

void CONSTOp(MachineState* CPU, FILE* output){
	unsigned short instr = CPU->memory[CPU->PC];
	unsigned short rd = (instr>>9)&0x7;
	short imm9 = instr & 0x01FF;
	if(imm9 & 0x100) imm9 |= 0xFE00;

	CPU->regInputVal = imm9;
	CPU->R[rd] = CPU->regInputVal;
	CPU->regFile_WE = 1;
	SetNZP(CPU, CPU->regInputVal);
		WriteOut(CPU, output);
	CPU->PC = (CPU->PC + 1)&0xFFFF;
}

void HICONSTOp(MachineState* CPU, FILE* output){
	unsigned short instr = CPU->memory[CPU->PC];
	unsigned short rd = (instr>>9)&0x7;
	unsigned short imm8 = instr & 0x00FF;

	CPU->regInputVal = (imm8<<8) | (CPU->R[rd] & 0x00FF);
	CPU->R[rd] = CPU->regInputVal;
	CPU->regFile_WE = 1;
	SetNZP(CPU, CPU->regInputVal);
		WriteOut(CPU, output);
	CPU->PC = (CPU->PC + 1)&0xFFFF;
}

void JSRJSRROp(MachineState* CPU, FILE* output){
	unsigned short instr = CPU->memory[CPU->PC];
	CPU->regInputVal = CPU->PC +1;

	CPU->regFile_WE = 1;
	CPU->NZP_WE = 1;
	SetNZP(CPU, CPU->regInputVal);

	if((instr >> 11) & 0x1){
		//JSR
		CPU->R[7] = CPU->regInputVal;
		JSROp(CPU, output);
	}else{
		//JSRR
		unsigned short rs = (instr>>6) & 0x7;
		unsigned short target = CPU->R[rs];
		CPU->R[7] = CPU->regInputVal;
			WriteOut(CPU, output);
		CPU->PC = target;
	}
}

void JMPJMPROp(MachineState* CPU, FILE* output){
		unsigned short instr = CPU->memory[CPU->PC];
		if((instr >> 11) & 0x1){
			JumpOp(CPU, output);
		}else{
			//JMPR
			unsigned short rs = (instr>>6) & 0x7;
				WriteOut(CPU, output);
			CPU->PC = CPU->R[rs];
		}
}

void TRAPOp(MachineState* CPU, FILE* output){
		unsigned short instr = CPU->memory[CPU->PC];
		unsigned short imm8 = instr &0x00FF;
		CPU->regInputVal = CPU->PC +1;
		CPU->R[7] = CPU->regInputVal;
		CPU->regFile_WE = 1;
		CPU->PSR = (CPU->PSR | 0x8000);
		SetNZP(CPU, CPU->regInputVal);
		WriteOut(CPU, output);
		CPU->PC = (0x8000) | (imm8);
}

/*
 * This function should execute one LC4 datapath cycle.
 */
int UpdateMachineState(MachineState* CPU, FILE* output)
{
    if(CPU->PC == 0x80FF){
			return -1;
		}

		if((CPU->PC & 0x8000) && !(CPU->PSR & 0x8000)){
			printf("ERROR:going into OS but not privilege\n");
			return -1;
		}

		if(CPU->memory[CPU->PC] == 0){
			WriteOut(CPU, output);
			CPU->PC+= 1;
			return 0;
		}

		unsigned short instr = CPU->memory[CPU->PC];
		ClearSignals(CPU);
		unsigned short opcode = (instr>>12) & 0xF;

		switch(opcode){
			case 0x0: BranchOp(CPU, output);break;
			case 0x1: ArithmeticOp(CPU, output);break;
			case 0x2: ComparativeOp(CPU, output);break;
			case 0x3: return 0; 
			case 0x4: JSRJSRROp(CPU, output);break;
			case 0x5: LogicalOp(CPU, output);break; 
			case 0x6: LDROp(CPU, output); break;
			case 0x7: STROp(CPU, output); break;
			case 0x8: RTIOp(CPU, output); break;
			case 0x9: CONSTOp(CPU, output); break;
			case 0xA: ShiftModOp(CPU, output);break;
			case 0xB: return 0; 
			case 0xC: JMPJMPROp(CPU, output);break;
			case 0xD: HICONSTOp(CPU, output);break;
			case 0xE: return 0; 
			case 0xF: TRAPOp(CPU, output);break;
		}

    return 0;
}



//////////////// PARSING HELPER FUNCTIONS ///////////////////////////

/*
 * Parses rest of branch operation and updates state of machine.
 */
void BranchOp(MachineState* CPU, FILE* output)
{
    unsigned short instr = CPU->memory[CPU->PC];
		 short imm9 = instr & 0x01FF;
		if(imm9 & 0x100) imm9 |= 0xFE00;

		unsigned int n = (instr>>11) & 0x1;
		unsigned int z = (instr>>10) & 0x1;
		unsigned int p = (instr>>9) & 0x1;

		unsigned int current_nzp  = CPU->PSR &0x0007;

		if((n && (current_nzp & 0x4)) ||
			 (z && (current_nzp & 0x2)) || 
			 (p && (current_nzp & 0x1))){
					WriteOut(CPU, output);
				CPU->PC = CPU->PC + 1 + imm9;
		}else{
				WriteOut(CPU, output);
			CPU->PC = (CPU->PC + 1) & 0xFFFF;
		}
}

/*
 * Parses rest of arithmetic operation and prints out.
 */
void ArithmeticOp(MachineState* CPU, FILE* output)
{
    unsigned short instr = CPU->memory[CPU->PC];
		unsigned short rd = (instr>>9)&0x7;
		unsigned short rs =  (instr>>6)&0x7;
		unsigned short isAddImm = (instr>>5) &0x1;

		short result = 0;
		if(isAddImm){
			short imm5 = instr & 0x1F;
			if(imm5 &0x10) imm5 |= 0xFFE0;
			result = CPU->R[rs] +imm5;
		}else{
			unsigned short rt = instr & 0x7;
			unsigned short operation = (instr >> 3) &0x7;
			switch(operation){
				case 0x0: result = CPU->R[rs] + CPU->R[rt]; break;
				case 0x1: result = CPU->R[rs] * CPU->R[rt]; break;
				case 0x2: result = CPU->R[rs] - CPU->R[rt]; break;
				case 0x3: if(CPU->R[rt] == 0){
					result = 0;
				}else
				{result = CPU->R[rs] / CPU->R[rt];} break;
				default: result = 0; break;
			}

		}

		CPU->regInputVal = result;
		CPU->regFile_WE = 1;
		CPU->R[rd] = result;

		SetNZP(CPU, result);
			WriteOut(CPU, output);
		CPU->PC = (CPU->PC + 1) & 0xFFFF;
}

/*
 * Parses rest of comparative operation and prints out.
 */
void ComparativeOp(MachineState* CPU, FILE* output)
{
    unsigned short instr = CPU->memory[CPU->PC];
		unsigned short rs = (instr>>9)&0x7;

		short result = 0;
		unsigned short rt;

		unsigned short operation = (instr >> 7) &0x3;
		switch(operation){
				case 0x0: 
						rt =  (instr)&0x7;
						short vals = CPU->R[rs];
						short valt = CPU->R[rt];
						result = vals - valt;
					  break;
				case 0x1: 
						rt =  (instr)&0x7;
						unsigned short uvals = CPU->R[rs];
						unsigned short uvalt = CPU->R[rt];
						result = (uvals > uvalt) ? 1 : (uvals < uvalt) ?  -1 : 0;
					  break;
				case 0x2: {
						short imm7 = (instr & 0x7F);
						if(imm7 & 0x40)imm7 |= 0xFF80;
						result = (short)CPU->R[rs] - imm7;
					  break;
				}
				case 0x3: {
						short imm7_2 = (instr & 0x7F);
						result = (CPU->R[rs] > imm7_2) ? 1 : (CPU->R[rs] < imm7_2) ?  -1 : 0; 
						break;
				}
				default: result = 0; break;
		}

		CPU->regInputVal = result;
		CPU->regFile_WE = 0;

		SetNZP(CPU, result);
			WriteOut(CPU, output);
		CPU->PC = (CPU->PC + 1) & 0xFFFF;
}

/*
 * Parses rest of logical operation and prints out.
 */
void LogicalOp(MachineState* CPU, FILE* output)
{
    unsigned short instr = CPU->memory[CPU->PC];
		unsigned short rd = (instr>>9)&0x7;
		unsigned short rs =  (instr>>6)&0x7;
		unsigned short isAndImm = (instr>>5) &0x1;

		short result = 0;
		if(isAndImm){
			short imm5 = instr & 0x1F;
			if(imm5 &0x10) imm5 |= 0xFFE0;
			result = CPU->R[rs] & imm5;
		}else{
			unsigned short rt = instr & 0x7;
			unsigned short operation = (instr >> 3) &0x7;
			switch(operation){
				case 0x0: result = CPU->R[rs] & CPU->R[rt]; break;
				case 0x1: result = ~(CPU->R[rs]); break;
				case 0x2: result = CPU->R[rs] | CPU->R[rt]; break;
				case 0x3: result = CPU->R[rs] ^ CPU->R[rt]; break;
				default: result = 0; break;
			}

		}

		CPU->regInputVal = result;
		CPU->regFile_WE = 1;
		CPU->R[rd] = result;

		SetNZP(CPU, result);
			WriteOut(CPU, output);
		CPU->PC = (CPU->PC + 1) & 0xFFFF;
}

/*
 * Parses rest of jump operation and prints out.
 */
void JumpOp(MachineState* CPU, FILE* output)
{
		unsigned short instr = CPU->memory[CPU->PC];
    unsigned short imm11 = instr &0x07FF;
			if(imm11 & 0x400) imm11 |= 0xF800;
			//JMP
			WriteOut(CPU, output);
			CPU->PC = (CPU->PC + 1 + imm11) & 0xFFFF;
}

/*
 * Parses rest of JSR operation and prints out.
 */
void JSROp(MachineState* CPU, FILE* output)
{
    unsigned short instr = CPU->memory[CPU->PC];
		unsigned short imm11 = instr &0x07FF;
			WriteOut(CPU, output);
		CPU->PC = (CPU->PC & 0x8000) | (imm11 << 4);
}

/*
 * Parses rest of shift/mod operations and prints out.
 */
void ShiftModOp(MachineState* CPU, FILE* output)
{
    unsigned short instr = CPU->memory[CPU->PC];
		unsigned short rd = (instr>>9)&0x7;
		unsigned short rs =  (instr>>6)&0x7;

		short result = 0;
		unsigned short rt = instr & 0x7;
		unsigned short imm4 = instr & 0xF;

		unsigned short operation = (instr >> 4) &0x3;

		short rs_val = CPU->R[rs];
		short rt_val = CPU->R[rt];

		switch(operation){
				case 0x0: result = rs_val << imm4; break;
				case 0x1: result = (rs_val) >> imm4; break;
				case 0x2: result = (unsigned short)(rs_val) >> imm4; break;
				case 0x3: 
				if(rt_val == 0){
					result = 0;
				}else{
					result = rs_val % rt_val;
				}
			  break;
				default: result = 0; break;
		}

		CPU->regInputVal = result;
		CPU->regFile_WE = 1;
		CPU->R[rd] = result;

		SetNZP(CPU, result);
		WriteOut(CPU, output);
		CPU->PC = (CPU->PC + 1) & 0xFFFF;
}


/*
 * Set the NZP bits in the PSR.
 */
void SetNZP(MachineState* CPU, short result)
{
    unsigned short nzp = 0;
		if(result < 0){
			nzp = 0x4;
		}else if(result == 0){
			nzp = 0x2;
		}else{
			nzp = 0x1;
		}
		CPU->PSR = (CPU->PSR & 0xFFF8) | nzp;
		CPU->NZPVal = nzp;
		CPU->NZP_WE = 1;
}
