/************************************************************************/
/* File Name : lc4.c 													*/
/* Purpose   : This file contains the main() for this project			*/
/*             main() will call the loader and disassembler functions	*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lc4_memory.h"
#include "lc4_hash.h"
#include "lc4_loader.h"
#include "lc4_disassembler.h"

//hash function
int hash_function (void* table, void* key){
	//make key from void* into int
	unsigned short int address = (unsigned short int)(uintptr_t)key;
	//apply hashing accordingly
	if(address <= 0x1FFF){
		return 0;
	}else if(address <= 0x7FFF){
		return 1;
	}else if(address <= 0x9FFF){
		return 2;
	}else{
		return 3;
	}
}

/* program to mimic pennsim loader and disassemble object files */
int main (int argc, char** argv) {
	/**
	 * main() holds the hashtable &
	 * only calls functions in other files
	 */
	/* step 1: create a pointer to the hashtable: memory 	*/
	lc4_memory_segmented* memory = NULL ;

	/* step 2: call create_hash_table() and create 4 buckets 	*/
	memory = create_hash_table(4, hash_function);
	if(memory == NULL){
		printf("error: cant create table\n");
		return 1;
	}

	/* step 3: determine filename, then open it		*/
	/*   TODO: extract filename from argv, pass it to open_file() */
	for(int i = 2; i < argc; i++){
		FILE* objFile = open_file(argv[i]);
		if(objFile == NULL){
			printf("error: cant open file\n");
			delete_table(memory);
			return 1;
		}
		/* step 4: call function: parse_file() in lc4_loader.c 	*/
		/*   TODO: call function & check for errors		*/
		int a = parse_file(objFile, memory);
		if(a != 0){
			printf("error: parse file failed\n");
			fclose(objFile);
			delete_table(memory);
			return 1;
		}
		fclose(objFile);
		/* step 5: repeat steps 3 and 4 for each .OBJ file in argv[] 	*/
	}

	/* step 6: call function: reverse_assemble() in lc4_disassembler.c */
	/*   TODO: call function & check for errors		*/
	if(reverse_assemble(memory) != 0){
		printf("error: reverse assemble failed\n");
		delete_table(memory);
		return 1;
	}

	/* step 7: call function: print out the hashtable to output file */
	/*   TODO: call function 				*/
	FILE* output = fopen(argv[1], "w");
	if(output == NULL){
		printf("error: output file not opened\n");
		delete_table(memory);
		return 1;
	}
	print_table(memory, output);
	fclose(output);

	/* step 8: call function: delete_table() in lc4_hash.c */
	/*   TODO: call function & check for errors		*/
	delete_table(memory);

	/* only return 0 if everything works properly */
	return 0 ;
}
