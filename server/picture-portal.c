/* Kevin Conley, William Etter, Teddy Zhang
 * CIS-542 - Final Project
 * Spring 2012
 */

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <stdint.h>

#include "picture-portal.h"
#include "linkedlist.h"     /* Linked List Implementation */

char *serial_path;
int serial_fd;
int image_index = 0;
int num_images = 0;
node *current;
img image_data;

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

void list_images() {
  //int num_files = -2; //skip . and ..
  int i = 0;
  DIR *d;
  struct dirent *dir;
  d = opendir("./images");
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      //num_files++;
      printf("%s\n", dir->d_name);
      add(dir->d_name);
      //TODO only look at image files (e.g. having a .BMP extension)
      //TODO compile a list of file name strings to return from this function
    }
    closedir(d);
    
    //printf("num files: %d\n", num_files);
  }
}

void send_image() {
  char CS = sizeof(image_data);
  uint8_t buf;
  
  //list_images();
  
  //TODO pick the next image (use image_index on the list of images)
  current = iterating();
  
  //For demo, send filename
  //strncpy(image_data.filename, current->filename, sizeof(image_data.filename));
  strcpy(image_data.filename, current->filename);
  printf("attempting to send: %s\n", image_data.filename);
  buf = 0x06;
  write(serial_fd, &buf, 1);
  buf = 0x85;
  write(serial_fd, &buf, 1);
  buf = sizeof(image_data);
  write(serial_fd, &buf, 1);
  int i;
  uint8_t *addr = (uint8_t*)(&image_data);
  //printf("about to enter for loop\n");
  for(i = 0; i < sizeof(image_data); i++) {
    //printf("got in\n");
    //printf("sending data byte number %d, value = %c\n", i, *(addr + i));
    CS ^= *(addr + i);
    buf = *(addr + i);
    write(serial_fd, &buf, 1);
  }
  buf = CS;
  //printf("about to write cs\n");
  write(serial_fd, &buf, 1);
  //printf("done writing cs: %x\n", CS);
  /*
  buf = 0xAA;
  write(serial_fd, &buf, 1);
  write(serial_fd, &buf, 1);
  write(serial_fd, &buf, 1);
  */
  //TODO send the image over serial, pixel by pixel
}

void loop() {
  while(serial_fd) {
    if (read_byte() == 0x1B) {
      send_image();
    }
    //TODO listen over internet for a new image
    
    //TODO if image received, add to list of images and send to arduino immediately
  }
}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    serial_path = argv[1];
  } else {
    serial_path = DEFAULT_SERIAL_PORT;
  }
  
  open_port();
  
  //node * test = (node*) malloc(sizeof(node));
  //test->filename = "test.jpg";
  //char test[50] = "test.jpg";
  //add(test);
  list_images();
  while(1) {
    send_image();
    sleep(3);
  }
  
  //loop();
  list_images();
  
  close(serial_fd);
  
  return(0);
}
