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

#include <pthread.h>

#include "linkedlist.h"

#define DEBUG	0

#define DEFAULT_SERIAL_PORT     "/dev/ttyACM0"
#define PROGRAM                 "picture portal"

using namespace std; 
using namespace Magick; 

typedef struct {
  uint16_t rownum;
  uint16_t data[240];
} rowpacket;

//thread-related global vars
pthread_t thread_serial_id; // id of serial thread
pthread_t thread_network_id; // id of network thread
pthread_mutex_t lcd_lock; // lock for threads communicating with lcd

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
//uint8_t recv_data;

//#################################################

//    Local Image Functions

//#################################################

void find_images() {
  //int i = 0;
  DIR *d;
  struct dirent *dir;
  //char path[50] = "images/";
  d = opendir("./images");
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if(!strcmp(".",dir->d_name) || !(strcmp("..",dir->d_name) || !(strcmp("aaaaa_pplogo.jpg", dir->d_name)))) {
        //printf("skipping %s...\n", dir->d_name);
      } else {
        printf("adding %s...\n", dir->d_name);
        char * path = (char *) malloc(50);
        memcpy(path, "images/", 8);
        strcat(path, dir->d_name);
        add(path);
        if (path != NULL) {
        	free(path);
        	path = NULL;
        }
      }
    }
    closedir(d);
  }
}

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

    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

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
	if (DEBUG)
		printf("got char: %x\n", *dest);
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

/* Try to receive data and put it at location marked by dest
 * Returns immediately if no data available to be read
 * Return size of data received, otherwise 0
 */
int receive_data(uint8_t** dest) {
	uint8_t buf, calc_CS;
	int i, size;

  	uint8_t type = 0;
  	uint8_t rx_array_inx = 0;
  	//printf("in receive data\n");
  	//if(available() == 0)
    	//return 0;
  	//printf("data was available\n");
	//try to receive preamble
	buf = 0;
	while(buf != 0xA1) {
		if (DEBUG)
			printf("trying to receive 0xA1\n");
  		read_bytes(&buf, 1);
	}

	if (DEBUG)
		printf("trying to receive 0xB2\n");
	read_bytes(&buf, 1);
	if (buf == 0xB2) {
		if (DEBUG)
			printf("trying to receive 0xC3\n");
		read_bytes(&buf, 1);
	  	if (buf == 0xC3) {
	  		read_bytes(&type, 1);

	  		switch (type) {
		  		case 0xAA:
		    		size = 482;
		    		break;
		  		case 0xBB:
		    		size = 30;
		    		break;
	    		case 0xCC:
	      			size = 1;
	      			break;
	    		default:
	      			printf("error, received unexpected serial data type: %x\n", type);
	      			return 0;
			}

	    	//malloc dest to this size
		  	*dest = (uint8_t *) malloc(size);	
		  	//printf("malloc'd dest to size %x\n", rx_len);
	    	if (DEBUG)
	    		printf("got preamble\n");
	    
	    	while(rx_array_inx <= size){
				    read_bytes((uint8_t *)(*dest) + rx_array_inx++, 1);
	    	}
		    
		    if (DEBUG)
	    		printf("receiving data:\n");
		    
		    if (DEBUG) {
			    for (i = 0; i < size; i++) {
			      printf("%x\n", (*dest)[i]);
			    }
		    }

	    	if(size == (rx_array_inx - 1)){
		    	//last uint8_t is CS
		    	if (DEBUG)
		    		printf("seemed to have gotten whole message\n");
		    	calc_CS = type;
		    	for (i = 0; i < size; i++){
				    calc_CS ^= (*dest)[i];
		    	} 

		    	if(calc_CS == (*dest)[rx_array_inx - 1]){
		    		//CS good
	        		return size;
	      		} else {
			    	//failed checksum
	        		return 0;
	      		}
	    	}
	  	}
  	}
  	//printf("failed to get preamble, returning\n");
	return 0;
}

/* data is pointer to data
 * type = AA (img row) || BB (location) || CC (command)
 */
void send_data(uint8_t *data, uint8_t type, int fast) {
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
	  //struct timespec tim;
    //tim.tv_sec = 0;
    //if (fast)
    	//tim.tv_nsec = 10000;
    //else
    	//tim.tv_nsec = 1000000;
    //nanosleep(&tim, NULL);
	}
	//lastly, write checksum
	//printf("writing checksum %x...\n", CS);
	buf = CS;
	val = write(serial_fd, &buf, 1);
	tcsetattr(serial_fd, TCSAFLUSH, &options);
}

//fast = 0 for slow, 1 for fast
int send_image(const char* path, int fast) {
	printf("Sending image %s...", path);

	//give me a row packet to hold the pixels to send to the lcd screen
	rowpacket* row = (rowpacket *) malloc(sizeof(rowpacket));

	if (row == NULL)
		return -1;

	try { 
		//Open the image in question 
		Image image(path);

		//This lets us modify the image
		image.modifyImage();

		int width = image.columns();
		int height = image.rows();
		printf("width: %d\n", width);
		printf("height: %d\n", height);

		if (width != height) {
			printf("cropping image");
			//crop image so height = width
			//(width, height, xOffset, yOffset)
			image.crop(Geometry(height < width ? height : width,height < width ? height : width));
			printf("new width: %d\n", image.columns());
			printf("new height: %d\n", image.rows());
		}

		//Resize the image to the size of the LCD screen
		printf("resizing\n");
		image.resize("240x240");
		printf("new width: %d\n", image.columns());
		printf("new height: %d\n", image.rows());

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
			uint8_t *data;	
			//send_data((uint8_t *) row, 0xAA, fast);
			
			while (1) {
				if (DEBUG)
					printf("trying to send row %i\n", start_y);
				send_data((uint8_t *) row, 0xAA, fast);
				receive_data(&data);
				if (data != NULL) {
					if (*data == 0x06) {
						if (DEBUG)
							printf("got ACK!\n");
						free(data);
						data = NULL;
						break;
					}
				}
				if (data != NULL) {
					free(data);
					data = NULL;
				}
			}
			
		}
		free(row);
		printf("done.\n");
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
	double recv_data;
	int bytes_received = recv(fd,&recv_data,8, MSG_WAITALL);
	// null-terminate the string
	//recv_data[bytes_received] = '\0';
	printf("Server received num bytes: %d\n", bytes_received);
	printf("Server received double: %x\n", recv_data);

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


//#################################################

//    Thread Functions

//#################################################

void* serial_thread(void *dummy) {
	uint8_t* data = NULL;
	int ret = 0;
	node *n = NULL;

	while(1) {
		if(data != NULL) {
			free(data);
      		data = NULL;
    	}
		pthread_mutex_lock(&lcd_lock);

		ret = receive_data(&data);

		if (ret == 1 && data != NULL) {
			switch (*data) {
				case 0x16:
					// <-
					//move back one image if not null
					n = previousNode();
					if (n != NULL) {
						send_image(n->filename, 0);
					}
					break;
				case 0x17:
					// ->
					//move forward one image if not null
					n = nextNode();
					if (n != NULL) {
						send_image(n->filename, 0);
					}
					break;
				case 0x06:
					//ACK
					break;
				case 0x15:
					//NAK
					break;
			}
		}
		pthread_mutex_unlock(&lcd_lock);
	}
}

void* network_thread(void *dummy) {

}

int main(int argc,char **argv) {
	if (argc > 1) {
		serial_path = argv[1];
	} else {
		serial_path = (char *) DEFAULT_SERIAL_PORT;
	}
	
	//TODO GOOD
	/*
	printf("Opening serial port: %s\n", serial_path);
	open_port();
	
	sleep(2);

	//printf("Loading Picture Portal logo...\n");
	//char init[50] = "logo.jpg";
	//send_image(init, 0);

	printf("Looking for existing images...\n");
	find_images();

	//create thread for serial communication
  	pthread_create(&thread_serial_id, NULL, &serial_thread, NULL);
	
	//create thread for network communication
	pthread_create(&thread_network_id, NULL, &network_thread, NULL);
	
	//wait for threads to finish (never)
	pthread_join(thread_network_id, NULL);
	pthread_join(thread_serial_id, NULL);
	
	printf("Closing serial port.\n");
	close_port();
	*/
	
	//TODO test Teddy
	init_network(1337);

	receive_network();

	close_network();
	//printf("size of long: %d\n", sizeof(double));
}
