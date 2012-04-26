#include <Magick++.h> 
#include <iostream> 
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <asm/ioctls.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DEFAULT_SERIAL_PORT     "/dev/ttyACM0"
#define PROGRAM                 "picture portal"

using namespace std; 
using namespace Magick; 

typedef struct {
  uint16_t rownum;
  uint16_t data[240];
} rowpacket;


char *serial_path;
int serial_fd;
struct termios options;


// structs to represent the server and client
struct sockaddr_in server_addr;
struct sockaddr_in client_addr;
int sock; // socket descriptor
int sin_size;
int fd;
// buffer to read data into
uint8_t recv_data;


//#################################################

//    Serial Communication Functions

//#################################################

void open_port() {
	serial_fd = open(serial_path, O_RDWR | O_NOCTTY | O_NDELAY); //O_NDELAY: don't care if something's connected
	if (serial_fd == -1) {
		//Could not open serial port
		perror(PROGRAM);
		exit(0);
	} else
	   

    /*
     * Get the current options for the port...
     */

    tcgetattr(serial_fd, &options);

    /*
     * Set the baud rates to 19200...
     */

    cfsetispeed(&options, B38400);
    cfsetospeed(&options, B38400);

    /*
     * Enable the receiver and set local mode...
     */

    options.c_cflag |= (CLOCAL | CREAD);

    /*
     * Set the new options for the port...
     */

    tcsetattr(serial_fd, TCSANOW, &options);
	
		fcntl(serial_fd, F_SETFL, 0); //FNDELAY); //don't block while reading
}

void close_port() {
	close(serial_fd);
}

/* Read size bytes from serial, store in dest
 * Return number of bytes available in serial buffer
 * Daemon exits on error
 */
int read_bytes(uint8_t *dest, const int size) {
	int val;

	val = read(serial_fd, dest, size);
	//printf("got char: %x\n", *dest);
	if (val >= 0) //include -1 for "resource temporarily unavailable"		
		return val;
	else {
		printf("%d\n", val);
		printf("error in read_bytes\n");
		printf("dest: %x size: %x\n", *dest, size);
		perror(PROGRAM);
		exit(0);
	}
} 
/*
int available() {
	int bytes;
	std::ioctl(serial_fd, FIONREAD, &bytes);
	return bytes;
}
*/

/* data is pointer to data
 * type = AA (img row) || BB (location) || CC (command)
 */
void send_data(uint8_t *data, uint8_t type) {
	int i, j, val, len;
	uint8_t buf, CS;

	//first write preamble 
	//printf("writing preamble...\n");
	buf = 0xA1;
	val = write(serial_fd, &buf, 1);
	buf = 0xB2;
	val = write(serial_fd, &buf, 1);
	buf = 0xC3;
	val = write(serial_fd, &buf, 1);

	//then write type of data being sent
	//printf("writing type: %d...\n", type);
	buf = type;
	val = write(serial_fd, &buf, 1);

	//now send the data
	//printf("writing data...\n");
	CS = type;
	
	switch (type) {
	  case 0xAA:
	    len = 482;
	    break;
	  case 0xBB:
	    len = 30;
	    break;
    case 0xCC:
      len = 1;
      break;
    default:
      printf("error, unexpected serial data type: %x\n", type);
      return;
	}
	
	for(j = 0; j < len; j++) {
		CS ^= *(data + j);
		buf = *(data + j);
		val = write(serial_fd, &buf, 1);
		//printf("byte %d: %d\n", j, buf);
	  struct timespec tim;
    tim.tv_sec = 0;
    tim.tv_nsec = 1000000;
    nanosleep(&tim, NULL);
	}
	//lastly, write checksum
	//printf("writing checksum %x...\n", CS);
	buf = CS;
	val = write(serial_fd, &buf, 1);
	tcsetattr(serial_fd, TCSAFLUSH, &options);
}

int send_image(const char* path) {
	//give me a row packet to hold the pixels to send to the lcd screen
	rowpacket* row = (rowpacket *) malloc(sizeof(rowpacket));

	if (row == NULL)
		return -1;

	try { 
		//Open the image in question 
		Image image(path);

		//This lets us modify the image
		image.modifyImage();

		//Resize the image to the size of the LCD screen
		image.resize("240x240");

		Pixels my_pixel_cache(image); // allocate an image pixel cache associated with my_image
		PixelPacket* pixels; // 'pixels' is a pointer to a PixelPacket array

		// define the view area that will be accessed via the image pixel cache
		int start_x = 0, size_x = 240, size_y = 1; 
		int start_y = 0;

		// vars to use in the loop below
		int i = 0;
		uint16_t newpixel;

		for (start_y = 0; start_y < 240; start_y++) {
		  //set row num in serial packet struct
		  row->rownum = start_y;
		
			// return a pointer to the pixels of the defined pixel cache
			pixels = my_pixel_cache.get(start_x, start_y, size_x, size_y);

			uint16_t newpixel;
			for (i = 0; i < 240; i++) { 
				//lcd pixel format is 16-bit word: RRRR RGGG GG_B BBBB
				newpixel = 0;
				
				//red
				uint16_t newred = pixels[i].red & 0xF800;
				newpixel |= newred;
	
				//green
				uint16_t newgreen = (pixels[i].green &= 0xF800) >> 5;
				newpixel |= newgreen; 
	
				//blue 
				uint16_t newblue = (pixels[i].blue &= 0xF800) >> 11;
				newpixel |= newblue;
				
				//printf("new pixel %d: %x\n", i, newpixel);
	
				//add this pixel to the array for this row of the image
				(row->data)[i] = newpixel;
			}
		
			//TODO send the serial packet
			//printf("test: sending serial packet for row %d\n", start_y);
			send_data((uint8_t *) row, 0xAA);
		}
		free(row);
		return 0;
	} 
	catch( Exception &error_ ) 
	{ 
		cout << "Caught exception: " << error_.what() << endl; 
		free(row);
		return -1; 
	}
}

//#################################################

//    Network Communication Functions

//#################################################

void init_network(int PORT_NUMBER){
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

int receive_network(){
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

void close_network(void){
      // 7. close: close the socket connection
      close(fd);
      printf("Server closed connection\n");
}


int main(int argc,char **argv) {
	if (argc > 1) {
		serial_path = argv[1];
	} else {
		serial_path = (char *) DEFAULT_SERIAL_PORT;
	}
	
  printf("Opening serial port: %s\n", serial_path);
  open_port();  
  
  sleep(5);
	send_image("chegg.jpg");	
	
	close_port();
	
  //TODO test Teddy
	/*
	init_network(1337);
  
  receive_network();
  
  close_network();
	*/
}
