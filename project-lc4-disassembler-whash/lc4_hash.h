/************************************************************************/
/* File Name : lc4_hash.h		 										*/
/* Purpose   : This header contains the structure for a hash table  	*/
/* 			   that mimic's the 4 segments of the LC4's memory:  		*/
/* 			   User Prog/Data Mem, OS Prog/Data Mem 	 				*/
/* Author(s) : tjf 														*/
/************************************************************************/

#ifndef LC4_HASH_H
#define LC4_HASH_H
#include "lc4_memory.h"

/*
 * definition of hash table struct
 */
typedef struct {
    int num_of_buckets ;  							// number of buckets
    row_of_memory** buckets ;  						// “array” of head pointers
	int (*hash_function)(void* table, void *key) ;  // pointer to hashing function 
} lc4_memory_segmented ;


/*
 * declaration of hash table helper functions
 */

lc4_memory_segmented* create_hash_table (int num_of_buckets, 
             					         int (*hash_function)(void* table, void *key) ) ;

int add_entry_to_tbl 					(lc4_memory_segmented* table, 
					  					 unsigned short int address,
			    	  					 unsigned short int contents) ;

row_of_memory* search_tbl_by_address 	(lc4_memory_segmented* table,
			                   			 unsigned short int address ) ;

void print_bucket	 					(lc4_memory_segmented* table, 
					  					 int bucket_number,
									     FILE* output_file ) ;

void print_table	 					(lc4_memory_segmented* table, 
										 FILE* output_file ) ;

void delete_table    					(lc4_memory_segmented* table ) ;

#endif
