/* 
This code primarily comes from 
http://www.prasannatech.net/2008/07/socket-programming-tutorial.html
and
http://www.binarii.com/files/papers/c_sockets.txt
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "portcomm.h"
#include "serialcomm.h"

// structs to represent the server and client
struct sockaddr_in server_addr,client_addr;
int sock; // socket descriptor
int sin_size;
int fd;
// buffer to read data into
uint8_t recv_data;


void init_portcomm(int PORT_NUMBER){
      
      //struct sockaddr_in server_addr,client_addr;    
      
      //int sock; // socket descriptor

      // 1. socket: creates a socket descriptor that you later use to make other system calls
      if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	perror("Socket");
	exit(1);
      }
      int temp;
      if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&temp,sizeof(int)) == -1) {
	perror("Setsockopt");
	exit(1);
      }

      // configure the server
      server_addr.sin_port = htons(PORT_NUMBER); // specify port number
      server_addr.sin_family = AF_INET;         
      server_addr.sin_addr.s_addr = INADDR_ANY; 
      bzero(&(server_addr.sin_zero),8); 
      
      // 2. bind: use the socket and associate it with the port number
      if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
	perror("Unable to bind");
	exit(1);
      }

      // 3. listen: indicates that we want to listn to the port to which we bound; second arg is number of allowed connections
      if (listen(sock, 5) == -1) {
	perror("Listen");
	exit(1);
      }
          
      printf("\nServer waiting for connection on port %d\n", PORT_NUMBER);
      fflush(stdout);
     

      // 4. accept: wait until we get a connection on that port
      sin_size = sizeof(struct sockaddr_in);
      fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);
      printf("Server got a connection from (%s, %d)\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
      
      
}


int receive_portcomm(){
      // 5. recv: read incoming message into buffer
      int bytes_received = recv(fd,&recv_data,1,0);
      // null-terminate the string
      //recv_data[bytes_received] = '\0';
      printf("Server received num bytes: %d\n", bytes_received);
      printf("Server received byte: %x\n", recv_data);

      //sendACK();
           
      // echo back the message to the client
      //char *send_data = "Thank you!\n";

      // 6. send: send a message over the socket
      //send(fd, send_data, strlen(send_data), 0);

      //printf("Server sent message: %s\n", send_data);
      //close_portcomm();
  return bytes_received;
}

void close_portcomm(void){
      // 7. close: close the socket connection
      close(fd);
      printf("Server closed connection\n");
}

