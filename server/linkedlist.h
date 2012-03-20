/* Kevin Conley, William Etter, Teddy Zhang
 * CIS-542 - Final Project
 * Spring 2012
 * Linked List Implementation - H File
 */

// Linked List Struct Definition
typedef struct linked_list{
    char filename[30];
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
