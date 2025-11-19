/************************************************************************/
/* File Name : lc4_disassembler.c 										*/
/* Purpose   : This file implements the reverse assembler 				*/
/*             for LC4 assembly.  It will be called by main()			*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include "lc4_hash.h"
#include <stdlib.h>

int reverse_assemble (lc4_memory_segmented* memory) 
{
	//assemble every bucket
	for(int i = 0; i < memory->num_of_buckets; i++){
		//inside each bucket, first search for all Arithmetic Ops
		row_of_memory* node = search_opcode(memory->buckets[i], 0x0001);
		while(node != NULL){
			//Get the registers, do malloc, and identify the opcode
			unsigned short instr = node->contents;
			unsigned short rd = (instr >> 9) & 0x7;
			unsigned short rs = (instr >> 6) & 0x7;
			unsigned short operation = (instr >> 3) & 0x7;
			char* assembly = malloc(32);
			//check if it's an imm operation or register operation
			if(instr & 0x20){
				short imm5 = (instr) & 0x1F;
				if(imm5 & 0x10) imm5 |= 0xFFE0; //sign extention
				sprintf(assembly, "ADD R%d, R%d, #%d", rd, rs, imm5);;
			}else{
				//get rt and load string accordingly
				unsigned short rt = (instr) & 0x7;
				switch(operation){
					case 0x0: sprintf(assembly, "ADD R%d, R%d, R%d", rd, rs, rt);break;
					case 0x1: sprintf(assembly, "MUL R%d, R%d, R%d", rd, rs, rt);break;
					case 0x2: sprintf(assembly, "SUB R%d, R%d, R%d", rd, rs, rt);break;
					case 0x3: sprintf(assembly, "DIV R%d, R%d, R%d", rd, rs, rt);break;
				}
			}
			node->assembly = assembly;
			//find the next arithmetic operation
			node = search_opcode(memory->buckets[i], 0x0001);
		}
		//then search for all Logical Ops
		node = search_opcode(memory->buckets[i], 0x05);
		while(node != NULL){
			//Get the registers, do malloc, and identify the opcode
			unsigned short instr = node->contents;
			unsigned short rd = (instr >> 9) & 0x7;
			unsigned short rs = (instr >> 6) & 0x7;
			unsigned short operation = (instr >> 3) & 0x7;
			char* assembly = malloc(32);
			//check if it's an imm operation or register operation
			if(instr & 0x20){
				short imm5 = (instr) & 0x1F;
				if(imm5 & 0x10) imm5 |= 0xFFE0; //sign extention
				sprintf(assembly, "AND R%d, R%d, #%d", rd, rs, imm5);;
			}else{
				//get rt and load string accordingly
				unsigned short rt = (instr) & 0x7;
				switch(operation){
					case 0x0: sprintf(assembly, "AND R%d, R%d, R%d", rd, rs, rt);break;
					case 0x1: sprintf(assembly, "NOT R%d, R%d", rd, rs);break;
					case 0x2: sprintf(assembly, "OR R%d, R%d, R%d", rd, rs, rt);break;
					case 0x3: sprintf(assembly, "XOR R%d, R%d, R%d", rd, rs, rt);break;
				}
			}
			node->assembly = assembly;
			//find the next logical operation
			node = search_opcode(memory->buckets[i], 0x05);
		}
	}	
	return 0 ;
}
