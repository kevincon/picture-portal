/* Kevin Conley, William Etter, Teddy Zhang
 * CIS-542 - Final Project
 * Spring 2012
 * Linked List Implementation - H File
 */


/* Function Declarations */
void add(char * filen);
node * iterate(void);
void resetIterator(void);


// Linked List Struct Definition
typedef struct linked_list{
    char[50] filename;
    struct linked_list * next;
} node;

// Current, Head, Tail, and Iterate Nodes
node * curr, head, tail, iterate;