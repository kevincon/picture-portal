#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

#include "picture-portal.h"

char *serial_path;
int serial_fd;
int image_index = 0;
int num_images = 0;

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
  int num_files = -2; //skip . and ..
  int i = 0;
  DIR *d;
  struct dirent *dir;
  d = opendir(".");
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      num_files++;
      printf("%s\n", dir->d_name);
      //TODO only look at image files (e.g. having a .BMP extension)
      //TODO compile a list of file name strings to return from this function
    }
    closedir(d);
    
    printf("num files: %d\n", num_files);
  }
}

void send_image() {
  list_images();
  
  //TODO pick the next image (use image_index on the list of images)
  
  //TODO send the image over serial, pixel by pixel
  
  if (image_index++ >= num_images) 
    image_index = 0;
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
  
  //loop();
  list_images();
  
  close(serial_fd);
  
  return(0);
}
