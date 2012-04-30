/* Kevin Conley, William Etter, Teddy Zhang
 * CIS-542 - Final Project
 * Spring 2012
 * Linked List Implementation - C File
 */

#include "linkedlist.h"

// Current, Head, Tail, and Iterate Nodes
node * head;
node * tail; 
node * iterate;

/*
 * Adds the filename to the Linked List
 */
void add(char * filen){
    node * curr = tail;
    tail = (node *)malloc(sizeof(node));
    strcpy(tail->filename, filen);
    if(curr == NULL){
        head = tail;
    }else{
        curr->next = tail;
        tail->previous = curr;
    }
}

/*
 * Returns the next node
 */
 node * nextNode(void){
    node * returnnode;
    if(iterate == NULL || iterate == tail){
        resetIterator();
        //return iterate;
    } else {
        iterate = iterate->next;
    }

    returnnode = iterate;
/*
    if(iterate != NULL) {    
        iterate = iterate->next;
        //if(iterate == NULL){
            // Reset Linked List
        //    resetIterator();
        //}
    }
*/
    return returnnode;
}

/*
 * Returns the previous node
 */
 node * previousNode(void){
    node * returnnode;
    if(iterate == NULL || iterate == head){
        iterate = tail;
        //return iterate;
    } else {
        iterate = iterate->previous;
    }
    
    returnnode = iterate;

    return returnnode;
}
 
/*
 * Reset Iterator
 */
 void resetIterator(void){
    iterate = head;
}

/*
 * Advance iterate node to tail
 */
void jumpTail(void) {
    iterate = tail;
}