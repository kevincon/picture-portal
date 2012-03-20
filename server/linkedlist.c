/* Kevin Conley, William Etter, Teddy Zhang
 * CIS-542 - Final Project
 * Spring 2012
 * Linked List Implementation - C File
 */

/*
 * Adds the filename to the Linked List
 */
void add(char * filen){
    node * curr = tail;
    tail = (node *)malloc(sizeof(node));
    tail->filename = filen;
    if(curr == NULL){
        head = tail;
    }else{
        curr->next = tail;
    }
}

/*
 * Returns the next node
 */
 node * iterate(void){
    if(iterate == NULL){
        return NULL;
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