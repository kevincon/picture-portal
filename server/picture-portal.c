/* Kevin Conley, William Etter, Teddy Zhang
 * CIS-542 - Final Project
 * Spring 2012
 */


#include "picture-portal.h"
#include "linkedlist.h"     /* Linked List Implementation */
#include "serialcomm.h"
#include "portcomm.h"

char *serial_path;
FILE * image_fd;
int image_index = 0;
int num_images = 0;
node *current;
pixel image_data;


void open_port() {
  serial_fd = open(serial_path, O_RDWR | O_NOCTTY | O_NDELAY);
  if (serial_fd == -1) {
	  //Could not open serial port
	  perror("open_port: Unable to open serial device");
	  exit(0);
  } else
	  fcntl(serial_fd, F_SETFL, FNDELAY); //don't block while reading
}

char read_byte() {
  char buf[1];
  int val;
  
  val = read(serial_fd, buf, 1);
  if (val > 0)
    return buf[0];
  else
    return val;
} 

void find_images() {
  //int i = 0;
  DIR *d;
  struct dirent *dir;
  d = opendir("./images");
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if(!strcmp(".",dir->d_name) || !(strcmp("..",dir->d_name))) {
        printf("skipping %s...\n", dir->d_name);
      } else {
        printf("adding %s...\n", dir->d_name);
        add(dir->d_name);
      }
    }
    closedir(d);
  }
}

//return 0 if fail to open, -1 if path null, otherwise return fd
/*
int open_image() {
  //current = iterating();
  if(current == NULL) 
    return -1;
  strcpy(image_data.filename, current->filename);
  image_fd = fopen(image_data.filename, "r");
  if(!image_fd)
    return 0;
  else
    return (int) image_fd;
}
*/


/*
void send_image() {
  int i, x, y;
  char CS = sizeof(image_data);
  uint8_t buf;
  uint8_t width, height, bit_mode, byte_count;
  fread(&width, 1, 1, image_fd);
  fread(&height, 1, 1, image_fd);
  fread(&bit_mode, 1, 1, image_fd);
  fread(&byte_count, 1, 1, image_fd);
  
  if (!(width == 0x80 && height == 0x80 && bit_mode == 0x04 && byte_count == 0x02)) {
    printf("error: image not right format\n");
    return;
  }
  
  printf("attempting to send: %s\n", image_data.filename);
  
  for (x = 0; i < width; x++) {
    for (y= 0; y < height; y++) {
      //grab a pixel    
      image_data.x = x;
      image_data.y = y;
      fread(&image_data.color, 3, 1, image_fd);
      
      //send it over serial using EasyTransfer method  
      buf = 0x06;
      write(serial_fd, &buf, 1);
      buf = 0x85;
      write(serial_fd, &buf, 1);
      buf = sizeof(image_data);
      write(serial_fd, &buf, 1);
      
      uint8_t *addr = (uint8_t*)(&image_data);
      for(i = 0; i < sizeof(image_data); i++) {
        CS ^= *(addr + i);
        buf = *(addr + i);
        write(serial_fd, &buf, 1);
      }
      buf = CS;
      write(serial_fd, &buf, 1);
    }
  }
}
*/

void close_image() {
  fclose(image_fd);
}

int main(int argc, char *argv[]) {

  char test;
  if (argc > 1) {
    serial_path = argv[1];
  } else {
    serial_path = DEFAULT_SERIAL_PORT;
  }
  
  initppSerial();
  open_port();

  init_portcomm(PORT_NUM);
  
  //node * test = (node*) malloc(sizeof(node));
  //test->filename = "test.jpg";
  //char test[50] = "test.jpg";
  //add(test);
  //find_images();
  while(1) {
     receive_portcomm();
     if(receiveData()){
	printf("WHAAT!?!?\n");
     }

	//if(available()){
	//test = read_byte();
	//printf("%c\n",test);
     //}
     //test = read_byte();
     // if(test>0){
//	printf("%c\n",test);
    /*
    if(receiveData()){
	printf("Got into here");
    }
*/
/*
    if(read_byte() == 0x1B) {
      //if(open_image() <= 0)
        //continue;
      //send_image();
      //close_image();
    }
*/
    //TODO listen over internet for a new image
    
    //TODO if image received, add to list of images and send to arduino immediately
  }
  
  close(serial_fd);
  close_portcomm();
  
  return(0);
}
