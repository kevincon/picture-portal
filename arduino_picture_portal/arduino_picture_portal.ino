/* ********************************************************************** */
/*                          Picture Portal                                */
/*                        Arduino Program Code                            */
/*                                                                        */
/* Authors : Kevin Conley (BSE '12)                                       */
/*           William Etter (MSE '12)                                      */
/*           Teddy Zhang (BSE/MSE '12)                                    */
/*                                                                        */
/*                      University of Pennsylvania                        */
/* CIS-542 - Embedded Systems Programming (Spring 2012)                   */
/* Date : March 23, 2011                                                  */
/* Version : 1.0                                                          */
/* Hardware : Arduino UNO, 2.8" TFT Touch Screen (ILI9325)                */
/* ********************************************************************** */

#define DEBUGGING 0

/* ****************************************************************************** */
/* *******************************   About  ************************************* */
/* ****************************************************************************** */
// This is the Arduino Program code for the Picture Portal system, submitted as the
// final project for CIS-542 at the University of Pennsylvania 2012.


/* ****************************************************************************** */
/* ******************************   TO DO  ************************************** */
/* ****************************************************************************** */
// Set up Serial communication between computer server and arduino
// Display startup logo on LCD
// Display controls on screen
// Display image on screen
// Swipe motion to switch images


/* ****************************************************************************** */
/* ****************************** Includes ************************************** */
/* ****************************************************************************** */
#include "arduino_picture_portal.h"
char alternate;

/* ****************************************************************************** */
/* **************************** Setup Program *********************************** */
/* ****************************************************************************** */
void setup(void) {
  initPicturePortal();                // Initialize the Picture Portal Display

  alternate = 0;
  
  // Display Logo on Screen
  
  
  /*
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
  tft.fillRect(5, 5, BOXSIZE, BOXSIZE, YELLOW);
  tft.fillRect(10, 10, BOXSIZE, BOXSIZE, GREEN);
  tft.fillRect(15, 15, BOXSIZE, BOXSIZE, CYAN);
  tft.fillRect(20, 20, BOXSIZE, BOXSIZE, BLUE);
  tft.fillRect(25, 25, BOXSIZE, BOXSIZE, MAGENTA);
*/
 

 currentcolor = RED;
 /*
 for(uint16_t test = 0; test < 240; test++){
    imagerowdata.imagedata[test] = RED;
  }
 
 
 for(uint16_t col = 0; col < 240; col++){
    tft.drawPixel(col, 4,imagerowdata.imagedata[col]);
  }
  
  for(uint16_t test = 0; test < 240; test++){
    imagerowdata.imagedata[test] = BLUE;
  }
  
  for(uint16_t col = 0; col < 240; col++){
    tft.drawPixel(col, 0,imagerowdata.imagedata[col]);
  }
  
  for(uint16_t test = 0; test < 240; test++){
    imagerowdata.imagedata[test] = GREEN;
  }
  
  for(uint16_t col = 0; col < 240; col++){
    tft.drawPixel(col, 100,imagerowdata.imagedata[col]);
  }
  
  for(uint16_t test = 0; test < 240; test++){
    imagerowdata.imagedata[test] = MAGENTA;
  }
  
  for(uint16_t col = 0; col < 240; col++){
    tft.drawPixel(col, 319,imagerowdata.imagedata[col]);
  }
 */
}



/* ****************************************************************************** */
/* **************************** Main Program ************************************ */
/* ****************************************************************************** */
void loop()
{
  delay(LOOPDELAY);
  
  // Check for Data
  dataReceived = receiveData();
  if(dataReceived == 1){
    // Received Image Data
    dispImageRow();
  }else if(dataReceived == 2){
    // Received Filename Data
  }else if(dataReceived == 3){
    // Received Location Data
  }else if(dataReceived == 4){
    // Received Send/Receive Data (i.e. ACK, NACK)
      //Serial.print("Received SEND DATA!");
  }
  
  // Check for Button Pressed
  PPgetPoint();       // Get the pressed point
  if (newPoint){
    if (((p.y-PENRADIUS) > BOXSIZE) && ((p.y+PENRADIUS) < tft.height())) {
      tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
      if(alternate){
        sendACK();
        alternate = 0;
      }else{
        sendNACK();
        alternate = 1;
      }
    }
  }
  
  
}
