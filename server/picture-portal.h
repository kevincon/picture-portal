#ifndef PICTURE_PORTAL_H
#define PICTURE_PORTAL_H

#include <stdint.h>

#define DEFAULT_SERIAL_PORT         "/dev/ttyACM0"
#define IMAGE_MAX_FILENAME_LENGTH   30

typedef char * string;

typedef struct pp_image{
  uint16_t color;
  uint8_t x;
  uint8_t y; 
  char filename[IMAGE_MAX_FILENAME_LENGTH]; 
} pixel;

#endif
