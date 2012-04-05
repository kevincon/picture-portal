/* Kevin Conley, William Etter, Teddy Zhang
 * CIS-542 - Final Project
 * Spring 2012
 * Linked List Implementation - H File
 */
#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include "picture-portal.h"

// Linked List Struct Definition
typedef struct linked_list{
    char filename[IMAGE_MAX_FILENAME_LENGTH];
    struct linked_list * next;
    struct linked_list * previous;
} node;

// Current, Head, Tail, and Iterate Nodes
node * head;
node * tail; 
node * iterate;

/* Function Declarations */
void add(char *);
node * nextNode(void);
node * previousNode(void);
void resetIterator(void);

#endif