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
#include "Arduino.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include "TFTLCD.h"
#include "TouchScreen.h"


/* ****************************************************************************** */
/* ********************   Configuration Definitions  **************************** */
/* ****************************************************************************** */
/************************** MISC **************************/
#define LOOPDELAY 5                               // Define the Loop Delay value
#define BOXSIZE 40
#define PENRADIUS 4
char alternate;


/************************* SERIAL *************************/
// PACKET STRUCTURE = (P1) + (P2) + (P3) + (PACKET TYPE) + (DATA) + (CHECKSUM)
#define USBBAUD 38400                             // USBCommunication Baud Rate            // 38400 at 500us works well
#define IMAGE_LOCATION_LENGTH 30                   // Size of Image Location Data in bytes
char location[IMAGE_LOCATION_LENGTH];              // Char array to hold Image Location Data
typedef struct{                   // Struct for holding Image Row Data
  uint16_t row;                 // Row numbers
  uint16_t imagedata[240];  // Row Data
} rowpacket;
rowpacket imagerowdata;             // Struct
#define P1 0xA1                                    // Preamble Byte 1
#define P2 0xB2                                    // Preamble Byte 2
#define P3 0xC3                                    // Preamble Byte 3
#define IMAGE_ROW_PACKET_TYPE 0xAA                 // Image+Row Packet Type
#define LOCATION_PACKET_TYPE 0xBB                  // Location Packet Type
#define COMMAND_PACKET_TYPE 0xCC                   // Command Packet Type
#define NEXT_IMAGE 0x17                            // Next Image Request Byte
#define PREVIOUS_IMAGE 0x16                        // Previous Image Request Byte
#define ACK 0x06                                   // Positive Acknowledgement Byte
#define NACK 0x15                                  // Negative Acknowledgement Byte
uint8_t * rx_buf_imagerow;                         // Temporary storage for received image data
uint8_t * rx_buf_location;                         // Temporary storage for received location data
uint8_t tempChar;                                  // Temporary Char Data
uint8_t commandChar;                               // Command Char Data
uint8_t dataTypeReceived;                          // Data Type (Image Data, Location Data, Command Data)
uint8_t dataReturn;                                // Data Read Correctly (1,2,3) or Not yet (0)
uint16_t rx_index;                                  // Received data index
uint8_t CS;                                        // Checksum data
uint8_t receivedCS;                                // Received Checksum
#define IMAGE_ROW_PACKET_SIZE 482                  // Image+Row Struct Size in Bytes
boolean canPress = true;
char strtest[5] = {'H','e','r','e','!'};
String tester;

/************************* DISPLAY *************************/
// Display Shield Pinout
#define LCD_CS A3                                  // Display Shield Pinout
#define LCD_CD A2                                  // Display Shield Pinout
#define LCD_WR A1                                  // Display Shield Pinout
#define LCD_RD A0                                  // Display Shield Pinout
TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, 0);     // Display Object
#define	BLACK 0x0000                               // Screen BLACK Color
#define	BLUE 0x001F                                // Screen BLUE Color
#define	RED 0xF800                                 // Screen RED Color
#define	GREEN 0x07E0                               // Screen GREEN Color
#define CYAN 0x07FF                                // Screen CYAN Color
#define MAGENTA 0xF81F                             // Screen MAGENTA Color
#define YELLOW 0xFFE0                              // Screen YELLOW Color
#define WHITE 0xFFFF                               // Screen WHITE Color
#define GRAY 0xA514                                // Screen GRAY Color
int WIDTH;                                         // Display Width
int HEIGHT;                                        // Display Height
#define IMAGE_WIDTH 240                            // Image Width
#define IMAGE_HEIGHT 320                           // Image Height

/*********************** TOUCHSCREEN ***********************/
#define YP A1                                      // Touchscreen Shield Pinout
#define XM A2                                      // Touchscreen Shield Pinout
#define YM 7                                       // Touchscreen Shield Pinout
#define XP 6                                       // Touchscreen Shield Pinout
#define TS_MINX 150                                // Touchscreen Min X Location
#define TS_MINY 120                                // Touchscreen Min Y Location
#define TS_MAXX 920                                // Touchscreen Max X Location
#define TS_MAXY 940                                // Touchscreen Max Y Location
#define MINPRESSURE 10                             // Touchscreen Min Pressure
#define MAXPRESSURE 1000                           // Touchscreen Max Pressure
#define TOUCHRESISTANCE 300                        // Touchscreen Resistance across X+ and X-
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); // Touchscreen Object
Point p;                                           // Touchscreen Point Object (where a press occurs)
boolean newPoint;                                  // Whether the screen is being pressed or not


/* ****************************************************************************** */
/* **********************   System Display Functions  *************************** */
/* ****************************************************************************** */
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
  }else{
    newPoint = false;
  }
}

// USB Cable At Top = X is from Left to Right, Y is from Top to Bottom
void dispImageRow(void){
  uint16_t rownum = (uint16_t)imagerowdata.row;
  if(rownum > 240 || rownum < 0){
     return; 
  }
  if(rownum > 0 && rownum < 238){
    // Prevent user from pressing buttons
    canPress = false;
    // GRAY OUT BUTTONS
  }
  if(rownum > 238){
    // User can press buttons again
    canPress = true;
    // UN-GRAY OUT BUTTONS
  }
  // Loop through and update
  for(uint8_t col = 0; col < 240; col++){
    tft.drawPixel((uint16_t)col, rownum,imagerowdata.imagedata[col]);
  }
}


/* ****************************************************************************** */
/* **********************   System Serial Functions  *************************** */
/* ****************************************************************************** */
/* Sends an Positive Acknowledgement of received data */
void sendCommand(uint8_t command){
  CS = COMMAND_PACKET_TYPE;
  Serial.write(P1);
  Serial.write(P2);
  Serial.write(P3);
  Serial.write(COMMAND_PACKET_TYPE);
  CS^=command;
  Serial.write(command);
  Serial.write(CS);
  
  if(command == ACK){
    tft.fillRect(60,60,BOXSIZE+60, BOXSIZE+60,GREEN);
  }
  if(command == NACK){
    tft.fillRect(60,60,BOXSIZE+60, BOXSIZE+60,RED);
  }
}

/* Receives the data */
char receiveData(void){
  // Check if found Preamble already
  if(dataTypeReceived == 0){
    // Haven't found data Preamble yet
    if(Serial.available() >= 3){
      // Enough bytes present for the Preamble
      while(Serial.read() != P1){
        // Check if too few bytes present in buffer
        if(Serial.available() < 2){
          return 0;
        }
      }
      // Found first byte of Preamble
      if(Serial.read() == P2){
        if(Serial.read() == P3){
          // Found entire preamble, next byte is the packet data size
          dataTypeReceived = Serial.read();
          //Serial.println("Found Header");
          //tft.fillRect(160,160,BOXSIZE, BOXSIZE,BLUE);
          //tft.drawString(300, 100, strtest, BLUE, 2);
        }
      }
    }
  }  
  
  if(dataTypeReceived == IMAGE_ROW_PACKET_TYPE){
    // Found Image+Row Packet, read in data
    //Serial.println("Getting Data");
    while(Serial.available() && rx_index<=IMAGE_ROW_PACKET_SIZE){
        rx_buf_imagerow[rx_index++] = Serial.read();
      }
      //Serial.print("Image row size = ");
      //Serial.println(IMAGE_ROW_PACKET_SIZE);
      
      //Serial.print("Index = ");
      //Serial.println(rx_index);
      //tft.fillRect(300,100,40, 40,BLACK);
      //tester = String(rx_index);
      //tester.toCharArray(strtest, 50);
      //tft.drawString(300, 100, strtest, BLUE, 2);
      
      
      if(IMAGE_ROW_PACKET_SIZE <= (rx_index - 1)){
        // Received the entire message, now check checksum
        //Serial.println("Checking checksum");
        receivedCS = dataTypeReceived;
        for(int i = 0; i < IMAGE_ROW_PACKET_SIZE; i++){
          receivedCS^=rx_buf_imagerow[i];
        }
        
        if(receivedCS == rx_buf_imagerow[rx_index-1]){
          // Found correct packet!
          memcpy(&imagerowdata, rx_buf_imagerow, IMAGE_ROW_PACKET_SIZE);
          dataTypeReceived = 0;
          rx_index = 0;
          
          // SEND POSITIVE ACKNOWLEDGEMENT
          //sendCommand(ACK);
          
          return 1;
        }else{
          // Incorrect packet!
          dataTypeReceived = 0;
          rx_index = 0;
          
          // SEND NEGATIVE ACKNOWLEDGEMENT
          //sendCommand(NACK);
          return 0;
        }
      }
  }else if(dataTypeReceived == LOCATION_PACKET_TYPE){
    // Found Location Packet, read in data
    
    
  }else if(dataTypeReceived == COMMAND_PACKET_TYPE){
    // Found Command Packet, read in data
    commandChar = Serial.read();
    receivedCS = Serial.read();
    CS = dataTypeReceived;
    CS ^= commandChar;
    dataTypeReceived = 0;
    rx_index = 0;
    if(receivedCS == CS){
      // Correct Packet Found
      //sendCommand(ACK);
      return 3;
    }else{
      // Incorrect Packet Found
      //sendCommand(NACK);
      return 0;
    }
  }else{
    dataTypeReceived = 0;
    rx_index = 0;
  }
  return 0;
}  
          

/* ****************************************************************************** */
/* **************************** Setup Program *********************************** */
/* ****************************************************************************** */
void setup(void) {
  Serial.begin(USBBAUD);   // Initialize Serial Communication with computer
  rx_buf_imagerow = (uint8_t*) malloc(IMAGE_ROW_PACKET_SIZE);   // Allocate Image+Row Struct Buffer
  rx_buf_location = (uint8_t*) malloc(IMAGE_LOCATION_LENGTH);   // Allocate Location Buffer
  dataTypeReceived = 0;                                         // Reset Data Type Received
  rx_index = 0;                                                 // Reset Receive Array Index
  tft.reset();                                                  // Reset the Display Interface
  tft.initDisplay();                                            // Initialize the Display
  tft.fillScreen(BLACK);                                        // Fill the Display with black
  pinMode(13, OUTPUT);                                          // Set Display Pin Mode
  
  // Set Display Width and Height
  WIDTH = tft.width();
  HEIGHT = tft.height();
  
  // White Background for Location Data
  tft.fillRect(4,244,232, 32, WHITE);
  
  // White Background for Buttons
  tft.fillRect(4,280,114, 36, WHITE);
  tft.fillRect(122,280,114,36,WHITE);
  


  
  // Buttons
  

  
  


 
 
 
}


/* ****************************************************************************** */
/* **************************** Main Program ************************************ */
/* ****************************************************************************** */
void loop()
{
  delay(LOOPDELAY);
  
  // Check for Data
  dataReturn = receiveData();
  if(dataReturn == 1){       // Received Image Data
    dispImageRow();
  }else if(dataReturn == 2){ // Received Location Data
  
  }else if(dataReturn == 3){ // Received Command Data
      //Serial.print("Received DATA!");
      if(commandChar == ACK){
        //tft.fillRect(0,0,BOXSIZE, BOXSIZE,GREEN);
      }
      if(commandChar == NACK){
        //tft.fillRect(0,0,BOXSIZE, BOXSIZE, RED);
      }
      
  }
  
  // Check for Button Pressed
  PPgetPoint();       // Get the pressed point
  if (newPoint){
    if (((p.y-PENRADIUS) > BOXSIZE) && ((p.y+PENRADIUS) < tft.height())) {
      tft.fillCircle(p.x, p.y, PENRADIUS, RED);
      if(alternate){
        //sendACK();
        alternate = 0;
      }else{
        //sendNACK();
        alternate = 1;
      }
    }
  }
  
} /**** END OF PICTURE PORTAL ****/
