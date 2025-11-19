/************************************************************************/
/* File Name : lc4_memory.h		 										*/
/* Purpose   : This program contains the structure for the LC4 memory's	*/
/* 			   linked list and the declarations of the helper functions */
/*             															*/
/* Author(s) : tjf 														*/
/************************************************************************/

#ifndef LC4_MEMORY_H
#define LC4_MEMORY_H

/*
 * definition of linked list node
 */

typedef struct row_of_memory_struct {
	unsigned short int address ;
	char * label ;
	unsigned short int contents ;
	char * assembly ;
	struct row_of_memory_struct *next ;
} row_of_memory ;


/*
 * declaration of linked list helper functions
 */

int add_to_list    	          (row_of_memory** head,
			                   unsigned short int address,
			                   unsigned short int contents) ;

row_of_memory* search_address (row_of_memory* head,
			                   unsigned short int address ) ;

row_of_memory* search_opcode  (row_of_memory* head,
			                   unsigned short int opcode  ) ;

int delete_from_list    	  (row_of_memory** head,
			                   unsigned short int address ) ;

void print_list		          (row_of_memory* head,
							   FILE* output_file ) ;

void delete_list    	      (row_of_memory** head ) ;

#endif
