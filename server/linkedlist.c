/* Kevin Conley, William Etter, Teddy Zhang
 * CIS-542 - Final Project
 * Spring 2012
 * Linked List Implementation - C File
 */

/*
 * Adds the filename to the Linked List
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linkedlist.h"

void add(char * filen){
    node * curr = tail;
    tail = (node *)malloc(sizeof(node));
    strcpy(tail->filename, filen);
    if(curr == NULL){
        head = tail;
    }else{
        curr->next = tail;
    }
}

/*
 * Returns the next node
 */
 node * iterating(void){
    if(iterate == NULL){
        resetIterator();
        return iterate;
    }
    node * returnnode = iterate;
    iterate = iterate->next;
    if(iterate == NULL){
        // Reset Linked List
        resetIterator();
    }
    return returnnode;
}
 
/*
 * Reset Iterator
 */
 void resetIterator(void){
    iterate = head;
}
