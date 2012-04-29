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
/* Version : 1.1                                                          */
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
#define LOOPDELAY 3                               // Define the Loop Delay value
#define BOXSIZE 40
#define PENRADIUS 4
char alternate;


/************************* SERIAL *************************/
// PACKET STRUCTURE = (P1) + (P2) + (P3) + (PACKET TYPE) + (DATA) + (CHECKSUM)
#define USBBAUD 115200                             // USBCommunication Baud Rate            // 38400 at 500us works well
#define IMAGE_LOCATION_LENGTH 19                   // Size of Image Location Data in bytes
char location[IMAGE_LOCATION_LENGTH];              // Char array to hold Image Location Data
typedef struct{                                    // Struct for holding Image Row Data
  uint16_t row;                                    // Row numbers
  uint16_t imagedata[240];                         // Row Data
} rowpacket;
rowpacket imagerowdata;                            // Struct
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
uint16_t rx_index;                                 // Received data index
uint8_t CS;                                        // Checksum data
uint8_t receivedCS;                                // Received Checksum
#define IMAGE_ROW_PACKET_SIZE 482                  // Image+Row Struct Size in Bytes
boolean canPress = true;
char strtest[5] = {'H','e','r','e','!'};
String tester;
uint16_t rx_counter = 0;                           // Rx Counter
boolean alternator = false;

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
#define ORANGE 0xFC80                              // Screen ORANGE Color
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

void drawLogoText(void){
  tft.fillRect(4,244,232, 32, WHITE);  // White Background
  char picportal[15] = {'P','i','c','t','u','r','e',' ','P','o','r','t','a','l','\0'};  // "Picture Portal" Text
  tft.drawString(43, 254, picportal, BLACK, 2);// Picture Portal Text Render
  tft.fillCircle(20, 262, 10,BLUE);
  tft.fillCircle(20, 257, 10,BLUE);
  tft.fillCircle(20, 262, 7, BLACK);
  tft.fillCircle(20, 257, 7, BLACK);
  
  tft.fillCircle(220, 262, 10,ORANGE);
  tft.fillCircle(220, 257, 10,ORANGE);
  tft.fillCircle(220, 262, 7, BLACK);
  tft.fillCircle(220, 257, 7, BLACK);
  
  tft.drawHorizontalLine(23, 262, 20, BLUE);
  tft.drawHorizontalLine(22, 260, 21, BLUE);
  tft.drawHorizontalLine(21, 258, 22, BLUE);
  tft.drawHorizontalLine(23, 264, 20, BLUE);
}

void drawLoadingText(void){
  tft.fillRect(4,244,232, 32, WHITE);  // White Background
  char picportal[12] = {' ',' ',' ',' ','L','O','A','D','I','N','G','\0'};  // "Picture Portal" Text
  tft.drawString(43, 254, picportal, BLACK, 2);// Picture Portal Text Render
  tft.fillCircle(20, 262, 10,BLUE);
  tft.fillCircle(20, 257, 10,BLUE);
  tft.fillCircle(20, 262, 7, BLACK);
  tft.fillCircle(20, 257, 7, BLACK);
  
  tft.fillCircle(220, 262, 10,ORANGE);
  tft.fillCircle(220, 257, 10,ORANGE);
  tft.fillCircle(220, 262, 7, BLACK);
  tft.fillCircle(220, 257, 7, BLACK);
  
  tft.drawHorizontalLine(23, 262, 68, BLUE);
  tft.drawHorizontalLine(22, 260, 69, BLUE);
  tft.drawHorizontalLine(21, 258, 70, BLUE);
  tft.drawHorizontalLine(23, 264, 68, BLUE);
}


void dispLocation(void){
  tft.fillRect(4,244,232, 32, WHITE);  // White Background
  tft.drawString(10, 254, location, BLACK, 2);// Picture Portal Text Render
}


// USB Cable At Top = X is from Left to Right, Y is from Top to Bottom
void dispImageRow(void){
  uint16_t rownum = (uint16_t)imagerowdata.row;
  if(rownum > 240 || rownum < 0){
     return; 
  }
  
  
  // Loop through and update
  for(uint8_t col = 0; col < 240; col++){
    tft.drawPixel((uint16_t)col, rownum,imagerowdata.imagedata[col]);
  }
  
  if(rownum > 0 && canPress){
    // Blank Screen
    tft.fillRect(0,0,240,240,BLACK);
        
    // Gray out buttons
    tft.fillRect(4,280,114, 36, GRAY);
    tft.fillRect(122,280,114,36,GRAY);
    drawLoadingText();
    canPress = false;
  }
  
  if(rownum < 239){
    if(rownum%2 == 0){
      tft.drawHorizontalLine(0, rownum+1, 240, BLUE);
    }else{
      tft.drawHorizontalLine(0, rownum+1, 240, ORANGE);
    }
  }
  
  if(rownum == 239){
    // User can press buttons again
    canPress = true;
    // UN-GRAY OUT BUTTONS
    // Left Button
    tft.fillRect(4,280,114, 36, WHITE);  // White Background
    tft.fillRect(49,292,55,10, BLACK);  // Black Bar
    tft.fillTriangle(50,310,50,284,20,297, BLACK);  // Black Triangle
  
    // Right Button
    tft.fillRect(122,280,114,36,WHITE);  // White Background
    tft.fillRect(135,292,55,10, BLACK);  // Black Bar
    tft.fillTriangle(190,310,190,284,220,297, BLACK);  // Black Triangle
    drawLogoText();
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
  if(command == NACK){
    if(alternator){
       alternator = false;
       tft.drawHorizontalLine(0, 241, 240, MAGENTA);
    }else{
       alternator = true;
       tft.drawHorizontalLine(0, 241, 240, CYAN);
         
    }
    
  }
  else if(command == ACK){
    tft.drawHorizontalLine(0, 241, 240, BLACK);
  }
}

/* Receives the data */
char receiveData(void){
  // Check if found Preamble already
  if(dataTypeReceived == 0){
    // Haven't found data Preamble yet
    if(Serial.available() >= 4){
      // Enough bytes present for the Preamble
      while(Serial.read() != P1){
        // Check if too few bytes present in buffer
        if(Serial.available() < 4){
          sendCommand(NACK);
          return 0;
        }
      }
      // Found first byte of Preamble
      if(Serial.read() == P2){
        if(Serial.read() == P3){
          // Found entire preamble, next byte is the packet data size
          dataTypeReceived = Serial.read();
        }
      }      
    }
  }  
  
  if(dataTypeReceived == IMAGE_ROW_PACKET_TYPE){
    // Found Image+Row Packet, read in data
    rx_counter++;
    
    while(Serial.available() && rx_index<=IMAGE_ROW_PACKET_SIZE){
        rx_buf_imagerow[rx_index++] = Serial.read();
      }
      // To exit out if not recieved;
      if(rx_counter > (60)){   //60 at 57600baud
        dataTypeReceived = 0;
        rx_counter = 0;
        sendCommand(NACK);
      }
      if(IMAGE_ROW_PACKET_SIZE <= (rx_index - 1)){
        // Received the entire message, now check checksum
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
          Serial.flush();
          sendCommand(ACK);
          
          return 1;
        }else{
          // Incorrect packet!
          dataTypeReceived = 0;
          rx_index = 0;
          
          // SEND NEGATIVE ACKNOWLEDGEMENT
          Serial.flush();
          sendCommand(NACK);
          return 0;
        }
      }
  }else if(dataTypeReceived == LOCATION_PACKET_TYPE){
    // Found Location Packet, read in data
    while(Serial.available() && rx_index<=IMAGE_LOCATION_LENGTH){
        rx_buf_location[rx_index++] = Serial.read();
    }
    if(IMAGE_LOCATION_LENGTH <= (rx_index - 1)){
      // Received the entire message, now check checksum
      receivedCS = dataTypeReceived;
      for(int i = 0; i < IMAGE_LOCATION_LENGTH; i++){
        receivedCS^=rx_buf_location[i];
      }
      
      if(receivedCS == rx_buf_location[rx_index-1]){
          // Found correct packet!
          memcpy(&location, rx_buf_location, IMAGE_LOCATION_LENGTH);
          dataTypeReceived = 0;
          rx_index = 0;
          return 2;
        }else{
          // Incorrect packet!
          dataTypeReceived = 0;
          rx_index = 0;
          return 0;
        }
    }
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
      return 3;
    }else{
      // Incorrect Packet Found
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
  
  // Logo
  drawLogoText();
  
  // Left Button
  tft.fillRect(4,280,114, 36, WHITE);  // White Background
  tft.fillRect(49,292,55,10, BLACK);  // Black Bar
  tft.fillTriangle(50,310,50,284,20,297, BLACK);  // Black Triangle

  // Right Button
  tft.fillRect(122,280,114,36,WHITE);  // White Background
  tft.fillRect(135,292,55,10, BLACK);  // Black Bar
  tft.fillTriangle(190,310,190,284,220,297, BLACK);  // Black Triangle 
}


/* ****************************************************************************** */
/* **************************** Main Program ************************************ */
/* ****************************************************************************** */
void loop()
{ 
  // Check for Data
  dataReturn = receiveData();
  if(dataReturn == 1){       // Received Image Data
    dispImageRow();
  }else if(dataReturn == 2){ // Received Location Data
    dispLocation();
  }else if(dataReturn == 3){ // Received Command Data
      //Serial.print("Received DATA!");
      if(commandChar == ACK){
        sendCommand(ACK);
      }    
  }
  
  // Check for Button Pressed
  PPgetPoint();       // Get the pressed point
  if (newPoint && canPress){
    if(p.y > 280){
      if(p.x < 120){
        sendCommand(PREVIOUS_IMAGE);
        canPress = false;
      }
      else{
        sendCommand(NEXT_IMAGE);
        canPress = false;
      }
      if(!canPress){
        // Blank Screen
        tft.fillRect(0,0,240,240,BLACK);
        
        // Gray Out Buttons
        tft.fillRect(4,280,114, 36, GRAY);
        tft.fillRect(122,280,114,36,GRAY);
        drawLoadingText();
      }
    }
  }
    
} /**** END OF PICTURE PORTAL ****/
