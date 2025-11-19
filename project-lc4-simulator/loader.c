/*
 * loader.c : Defines loader functions for opening and loading object files
 */

#include "loader.h"

// memory array location
unsigned short memoryAddress;

unsigned short readEndian(FILE* file){
		unsigned char left = fgetc(file);
		unsigned char right = fgetc(file);
		return (left<<8) | right;
}

unsigned char isDATA[65535 + 1] = {0};
unsigned char isCODE[65535 + 1] = {0};

/*
 * Read an object file and modify the machine state as described in the writeup
 */
int ReadObjectFile(char* filename, MachineState* CPU)
{
  FILE* file = fopen(filename, "rb");
	if(file==NULL){
		return -1;
	}
	while(!feof(file)){
		unsigned short header = readEndian(file);
		if(feof(file)){
			break;
		}
		if(header == 0xCADE || header == 0xDADA){
			unsigned short address = readEndian(file);
			unsigned short n = readEndian(file);
			int i = 0;
			for(i = 0; i < n; i++){
				(*CPU).memory[address] = readEndian(file);
				if(header == 0xCADE){
					isCODE[address] = 1;
				}else{
					isDATA[address] = 1;
				}
				address++;
			}
		}else if(header == 0xC3B7 || header == 0xF17E){
			unsigned short first = readEndian(file);
			unsigned short n = (header == 0xF17E) ? first : readEndian(file);
			int i = 0;
			for(i = 0; i <n; i++){
				fgetc(file);
			}
		}else if(header == 0x715E){
			readEndian(file);
			readEndian(file);
			readEndian(file);
		}else{
			break;
		}
	}
	fclose(file);
  return 0;
}
