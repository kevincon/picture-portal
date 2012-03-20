#ifndef PICTURE_PORTAL_H
#define PICTURE_PORTAL_H

#define DEFAULT_SERIAL_PORT         "/dev/ttyACM0"
#define IMAGE_MAX_FILENAME_LENGTH   50

typedef char * string;

typedef struct pp_image{
  //int frame[IMAGE_WIDTH][IMAGE_HEIGHT];
  char filename[IMAGE_MAX_FILENAME_LENGTH]; 
} img;

#endif
