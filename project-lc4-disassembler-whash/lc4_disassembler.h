/************************************************************************/
/* File Name : lc4_disassembler.h 										*/
/* Purpose   : This function declares functions for lc4_dissembler.c	*/
/*      																*/
/*             															*/
/* Author(s) : tjf 														*/
/************************************************************************/

#include <stdio.h>
#include "lc4_hash.h"

/*
 * declarations of functions that must defined in lc4_disassembler.c
 */



/**
 * translates the hex representation of arithmetic instructions
 * into their assembly equivalent in the right buckets of the hashtable.
 *
 * returns 0 upon success, and non-zero if an error occurs.
 */
int reverse_assemble (lc4_memory_segmented* memory) ;

