/* Kevin Conley, William Etter, Teddy Zhang
 * CIS-542 - Final Project
 * Spring 2012
 * Serial Communication Implementation - H File
 */
#ifndef _PORT_COMM_H_
#define _PORT_COMM_H_

void init_portcomm(int PORT_NUMBER);
int receive_portcomm(void);
void close_portcomm(void);

#endif