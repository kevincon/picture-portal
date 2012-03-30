/* ********************************************************************** */
/*                          Picture Portal                                */
/*                      Arduino Program Header                            */
/*                                                                        */
/* Authors : Kevin Conley (BSE '12)                                       */
/*           William Etter (MSE '12)                                      */
/*           Teddy Zhang (BSE/MSE '12)                                    */
/*                                                                        */
/*                      University of Pennsylvania                        */
/* CIS-542 - Embedded Systems Programming (Spring 2012)                   */
/* Date : March 23, 2011                                                  */
/* Version : 1.0                                                          */
/* Hardware : Arduino UNO, 2.8" TFT Touch Screen                          */
/* ********************************************************************** */


/* ****************************************************************************** */
/* ****************************** Includes ************************************** */
/* ****************************************************************************** */
#include "Arduino.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>

#include "TFTLCD.h"
#include "TouchScreen.h"
//#include "SoftEasyTransfer.h"


/* ****************************************************************************** */
/* ********************   Configuration Definitions  **************************** */
/* ****************************************************************************** */
#define LOOPDELAY 3                        // Define the Loop Delay value

#define USBBAUD 9600                         // USBCommunication Baud Rate
#define SERIAL_REQUEST_IMAGE_BYTE   0x1B     // Communication Request Image Byte
#define IMAGE_MAX_FILENAME_LENGTH   30

//EasyTransfer ET;

struct pp_image{
  //int frame[IMAGE_WIDTH][IMAGE_HEIGHT];
  char filename[IMAGE_MAX_FILENAME_LENGTH]; 
};

//this global var will hold data about the current image
pp_image image_data;



/************************* DISPLAY *************************/
// Display Shield Pinout
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0

// Display Object
TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, 0);

// Screen Color Definitions
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0 
#define WHITE           0xFFFF

int WIDTH;              // Display Width
int HEIGHT;             // Display Height

#define IMAGE_WIDTH 240
#define IMAGE_HEIGHT 320


/*********************** TOUCHSCREEN ***********************/
// Touchscreen Shield Pinout
#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

// Touchscreen Min/Max Locations
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

// Touchscreen Min/Max Pressure Values
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// Touchscreen resistance across X+ and X-
#define TOUCHRESISTANCE 300

// Touchscreen Object
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Touchscreen Point Object (where a press occurs)
Point p;

boolean newPoint;






#define BOXSIZE 40
#define PENRADIUS 4
int oldcolor, currentcolor;


/* ****************************************************************************** */
/* ***************************   System Functions  ****************************** */
/* ****************************************************************************** */


// Picture Portal Initialization Function
void initPicturePortal(){
  Serial.begin(USBBAUD);                     // Initialize Serial Communication with computer
  //ET.begin(details(image_data), &Serial);    // Initialize Easy-Transfer
  tft.reset();                               // Reset the Display Interface
  tft.initDisplay();                         // Initialize the Display
  tft.fillScreen(BLACK);                     // Fill the Display with black
  pinMode(13, OUTPUT);
  
  
  // Set Display Width and Height
  WIDTH = tft.width();
  HEIGHT = tft.height();
  
  if(DEBUGGING){
    Serial.println("IN DEBUGGING MODE");
  }
}

// Picture Portal Get Touched Point Function
void PPgetPoint(void){
  digitalWrite(13, HIGH);
  p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  if(p.z > MINPRESSURE && p.z < MAXPRESSURE){
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
    newPoint = true;
    if(DEBUGGING){
      Serial.print("X = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print("\tPressure = "); Serial.println(p.z);
    }
  }else{
    newPoint = false;
  }
}
