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

#define DEBUGGING 1

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


/* ****************************************************************************** */
/* **************************** Setup Program *********************************** */
/* ****************************************************************************** */
void setup(void) {
  initPicturePortal();                // Initialize the Picture Portal Display
  
  
  
  // Display Logo on Screen
  
  
  
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
  tft.fillRect(5, 5, BOXSIZE, BOXSIZE, YELLOW);
  tft.fillRect(10, 10, BOXSIZE, BOXSIZE, GREEN);
  tft.fillRect(15, 15, BOXSIZE, BOXSIZE, CYAN);
  tft.fillRect(20, 20, BOXSIZE, BOXSIZE, BLUE);
  tft.fillRect(25, 25, BOXSIZE, BOXSIZE, MAGENTA);

 

 currentcolor = RED;
 
}



/* ****************************************************************************** */
/* **************************** Main Program ************************************ */
/* ****************************************************************************** */
void loop()
{
  delay(LOOPDELAY);
  
  
  // Check for Data
  if(DEBUGGING){
    uint8_t buf;
    if(Serial.available()) {
      buf = Serial.read();
      Serial.println(buf, HEX);
    }
  }else{
    //if(ET.receiveData()){ //if we've received an image
      Serial.print("Received image: ");
      Serial.println(image_data.filename);
    //}
  }
  
  
  // Check for Button Pressed
  PPgetPoint();       // Get the pressed point
  if (newPoint){
    if (((p.y-PENRADIUS) > BOXSIZE) && ((p.y+PENRADIUS) < tft.height())) {
      tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
    }
  }
  
  
}
