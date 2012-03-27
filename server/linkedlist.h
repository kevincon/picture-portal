/* Kevin Conley, William Etter, Teddy Zhang
 * CIS-542 - Final Project
 * Spring 2012
 * Linked List Implementation - H File
 */

#include "picture-portal.h"

// Linked List Struct Definition
typedef struct linked_list{
    char filename[IMAGE_MAX_FILENAME_LENGTH];
    struct linked_list * next;
} node;

// Current, Head, Tail, and Iterate Nodes
node * head;
node * tail; 
node * iterate;

/* Function Declarations */
void add(char *);
node * iterating(void);
void resetIterator(void);
