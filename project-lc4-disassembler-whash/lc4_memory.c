/************************************************************************/
/* File Name : lc4_memory.c		 										*/
/* Purpose   : This file implements the linked_list helper functions	*/
/* 			   to manage the LC4 memory									*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "lc4_memory.h"


/*
 * adds a new node to linked list pointed to by head
 */
int add_to_list (row_of_memory** head,
		 		 unsigned short int address,
		 		 unsigned short int contents)
{
	/* allocate memory for a single node */
	row_of_memory* node = malloc(sizeof(row_of_memory));
	if(node == NULL){
		return -1;
	}


	/* populate fields in newly allocated node with arguments: address/contents */
	node->address = address;
	node->contents = contents;
    /* make certain to set other fields in structure to NULL */
	node->label = NULL;
	node->assembly = NULL;
	node->next = NULL;
	/* if head==NULL, node created is the new head of the list! */
	if(*head == NULL || (*head)->address >address){
		node->next = *head;
		*head = node;
		return 0;
	}
	row_of_memory* curr = *head;
	row_of_memory* prev = NULL;
	/* otherwise, insert the node into the linked list keeping it in order of ascending addresses */
	while(curr != NULL && curr->address < node->address){
		prev = curr;
		curr = curr->next;
	}

	//if the address is the same, update the old one and free the new one
	if(curr != NULL && curr->address == node -> address){
		curr->address = address;
		curr->contents = contents;
		free(node);
		return 0;
	}

	prev->next = node;
	node->next = curr;
	/* return 0 for success, -1 if malloc fails */
	return 0 ;
}


/*
 * search linked list by address field, returns node if found
 */
row_of_memory* search_address (row_of_memory* head,
			        		   unsigned short int address )
{
	/* traverse linked list, searching each node for "address"  */
	row_of_memory* curr = head;
	while(curr != NULL && curr->address != address){
		curr = curr->next;
	}
	/* return pointer to node in the list if item is found */
	if(curr != NULL){
		return curr;
	}
	/* return NULL if list is empty or if "address" isn't found */
	return NULL ;
}

/*
 * search linked list by opcode field, returns node if found
 */
row_of_memory* search_opcode (row_of_memory* head,
				      		  unsigned short int opcode  )
{
	/* traverse linked list until node is found with matching opcode
	   AND "assembly" field of node is empty */
	row_of_memory* curr = head;
	while(curr != NULL){
		/* return pointer to node in the list if item is found */
		if((curr->contents >> 12) == opcode && curr->assembly == NULL){
			return curr;
		}
		curr = curr->next;
	}
	/* return NULL if list is empty or if no matching nodes */
	return NULL ;
}

/*
 * delete the node from the linked list with matching address
 */
int delete_from_list (row_of_memory** head,
			          unsigned short int address ) 
{
	if(head == NULL){
		return -1;
	}
	row_of_memory* curr = *head;
	if(curr->address == address){ 	/* make certain to update the head pointer - if original was deleted */
		*head = curr->next;
		free(curr->label);
		free(curr->assembly);
		free(curr);
		return 0;
	}
	/* if head isn't NULL, traverse linked list until node is found with matching address */
	while(curr != NULL && curr->next->address != address){
		curr = curr->next;
	}
	/* delete the matching node, re-link the list */
	if(curr != NULL && curr -> next != NULL){
		row_of_memory* next = curr->next;
		curr->next = curr->next->next;
		free(next->label);
		free(next->assembly);
		free(next);
		return 0;
	}

	/* return 0 if successfully deleted the node from list, -1 if node wasn't found */
	return -1 ;
}

void print_list (row_of_memory* head, 
				 FILE* output_file )
{
	/* make sure head isn't NULL */
	if(head == NULL || output_file == NULL){
		return;
	}
	/* print out a header to output_file */
	fprintf(output_file, "<label>               <address>    <contents>   <assembly>\n");
	/* traverse linked list, print contents of each node to output_file */
	row_of_memory* curr = head;
	int count = 0;
	while(curr != NULL){
		fprintf(output_file, 
		"%-20s  0x%04X       0x%04X       %s\n", 
		(curr->label == NULL) ? "" : curr->label,  curr->address, curr->contents,
		(curr->assembly == NULL) ? "" : curr->assembly);
		curr = curr->next;
		count += 1;
	}
	return ;
}

/*
 * delete entire linked list
 */
void delete_list (row_of_memory** head )
{
	/* delete entire list node by node */
	while(*head != NULL){
		delete_from_list(head, (*head)->address);
	}
	/* set head = NULL upon deletion */
	*head = NULL;
	return;
}