/* Kevin Conley, William Etter, Teddy Zhang
 * CIS-542 - Final Project
 * Spring 2012
 * Linked List Implementation - C File
 */

/*
 * Adds the filename to the Linked List
 */
void add(char * filen){
    head = (node *)malloc(sizeof(node));
    head->filename = fn;
    if(curr == NULL){
        tail = head;
    }else{
        curr->next = head;
    }
    curr = head;
}


/*
 * Returns the next node
 */
 node * iterate(void){
    node * returnnode = iterate;
    iterate = iterate->next;
    return returnnode;
}
 
 
/*
 * Reset Iterator
 */
 void resetIterator(void){
    iterate = tail;
}