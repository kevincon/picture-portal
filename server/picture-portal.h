#ifndef _PICTURE_PORTAL_H_
#define _PICTURE_PORTAL_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

#define DEFAULT_SERIAL_PORT         "/dev/ttyACM0"
#define IMAGE_MAX_FILENAME_LENGTH   30
#define PORT_NUM 1337

int serial_fd;


typedef char * string;

typedef struct pp_image{
  uint16_t color;
  uint8_t x;
  uint8_t y; 
  char filename[IMAGE_MAX_FILENAME_LENGTH]; 
} pixel;

#endif
