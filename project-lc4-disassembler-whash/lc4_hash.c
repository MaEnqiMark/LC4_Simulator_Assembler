/************************************************************************/
/* File Name : lc4_hash.c		 										*/
/* Purpose   : This file contains the definitions for the hash table  	*/
/*																		*/
/* Author(s) : tjf 														*/
/************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "lc4_hash.h"

/*
 * creates a new hash table with num_of_buckets requested
 */
lc4_memory_segmented* create_hash_table (int num_of_buckets, 
             					         int (*hash_function)(void* table, void *key) ) 
{
	// allocate a single hash table struct
	lc4_memory_segmented* table = malloc(sizeof(lc4_memory_segmented));
	if(table == NULL) return NULL;
	table->num_of_buckets = num_of_buckets;

	// allocate memory for the buckets (head pointers)
	table->buckets = malloc(sizeof(row_of_memory*) * num_of_buckets);
	if(table->buckets == NULL) return NULL;

	//make buckets NULL
	for(int i = 0; i <num_of_buckets; i++){
		table->buckets[i] = NULL;
	}

	// assign function pointer to call back hashing function
	table->hash_function = hash_function;

	// return table for success, NULL for failure
	return table;
}


/*
 * adds a new entry to the table
 */
int add_entry_to_tbl (lc4_memory_segmented* table, 
					  unsigned short int address,
			    	  unsigned short int contents) 
{
	if(table == NULL || table->hash_function == NULL) return -1;
	// apply hashing function to determine proper bucket #
	int hashCode = table->hash_function(table, (void*)(uintptr_t)address);
	// add to bucket's linked list using linked list add_to_list() helper function
	return add_to_list(&(table->buckets[hashCode]), address, contents);
}

/*
 * search for an address in the hash table
 */
row_of_memory* search_tbl_by_address 	(lc4_memory_segmented* table,
			                   			 unsigned short int address ) 
{
	if(table == NULL || table->hash_function == NULL) return NULL;
	// apply hashing function to determine bucket # item must be located in
	int hashCode = table->hash_function(table, (void*)(uintptr_t)address);
	// invoked linked_lists helper function, search_by_address() to return return proper node
	return search_address((table->buckets[hashCode]), address);
}

/*
 * prints the linked list in a particular bucket
 */

void print_bucket (lc4_memory_segmented* table, 
				   int bucket_number,
				   FILE* output_file ) 
{
	// call the linked list helper function to print linked list
	print_list(table->buckets[bucket_number], output_file); 
	return;
}

/*
 * print the entire table (all buckets)
 */
void print_table (lc4_memory_segmented* table, 
				  FILE* output_file ) 
{
	// call the linked list helper function to print linked list to output file for each bucket
	for(int i = 0; i < table->num_of_buckets; i++){
		print_list(table->buckets[i], output_file); 
	}
	return ;
}

/*
 * delete the entire table and underlying linked lists
 */

void delete_table (lc4_memory_segmented* table ) 
{
	// call linked list delete_list() on each bucket in hash table
	for(int i = 0; i < table->num_of_buckets; i++){
		delete_list(&(table->buckets[i])); 
	}
	// then delete the table itself
	free(table->buckets);
	free(table);
	return ;
}
