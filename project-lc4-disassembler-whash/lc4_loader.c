    /************************************************************************/
/* File Name : lc4_loader.c		 										*/
/* Purpose   : This file implements the loader (ld) from PennSim		*/
/*             It will be called by main()								*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "lc4_memory.h"
#include "lc4_hash.h"

/* declarations of functions that must defined in lc4_loader.c */

FILE* open_file(char* file_name)
{
	if(file_name == NULL){
		return NULL; // return null if file doesn't exist
	}
	FILE* myFile = fopen(file_name, "rb");
	return myFile; // if file doesn't exist, it will return NULL;
}

int parse_file (FILE* my_obj_file, lc4_memory_segmented* memory)
{
	if(my_obj_file == NULL || memory == NULL){
		return 1;
	}

	while(!feof(my_obj_file)){
		unsigned short int header, address, n;
		// read the header
		// if fread doesn't give back 1, it means something went wrong and we break from while
		if(fread(&header, sizeof(header), 1, my_obj_file)!= 1) break; 
		if(fread(&address, sizeof(address), 1, my_obj_file)!= 1) break;
		if(fread(&n, sizeof(n), 1, my_obj_file)!= 1) break;
		//handleing endianess
		header = (header >> 8) | (header << 8);
		address = (address >> 8) | (address << 8);
		n = (n >> 8) | (n << 8);
		//check which section are we on
		if(header == 0xDADA || header == 0xCADE){
			for(int i = 0; i < n; i++){
				unsigned short contents;
				//read the contents in; return error if failed;
				if(fread(&contents, sizeof(contents), 1, my_obj_file)!= 1) return 2;
				contents = (contents >> 8) | (contents << 8); // handle endianess
				add_entry_to_tbl(memory, address + i, contents); // call add entry to tbl
			}
		}else if(header == 0xC3B7){
			char* label = malloc(n+1); //allocation, +1 because of \0
			// if didn't read n char, return error;
			if(fread(label, sizeof(char), n, my_obj_file) != n) return 2; 
			label[n] = '\0';
			//assign to label to corresponding node
			row_of_memory* node = search_tbl_by_address(memory, address); //search for the node
			if(node != NULL){
				node->label = label; //assign label
			}else{
				free(label);	// we didn't find the node, just free the label
				//(the label would freed when the node gets deleted if we found it)
			}
		}else{
			fseek(my_obj_file, n, SEEK_CUR); //else we skip n lines (? not sure if neccessary)
		}
	}
	//fclose(my_obj_file);  //I'll let main handle this
	return 0 ;
}
