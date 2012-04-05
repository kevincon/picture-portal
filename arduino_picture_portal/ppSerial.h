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
#ifndef _PPSERIAL_H_
#define _PPSERIAL_H_

/* ****************************************************************************** */
/* ********************   Configuration Definitions  **************************** */
/* ****************************************************************************** */

uint8_t rx_array_index;         //index for RX parsing buffer
uint8_t imagerowpacketsize;     // Packet size of image row (not including header, type indicator, or CS)
uint8_t filenamepacketsize;     // Packet size of filename information (not including header, type indicator, or CS)
uint8_t locationpacketsize;     // Packet size of location information (not including header, type indicator, or CS)
uint8_t sendpacketsize;         // Packet size of sent data (not including header, type indicator, or CS)
uint8_t CS;                     // Checksum for sending data
uint8_t receivedCS;             // Checksum for determining if received packet is correct

uint8_t * rx_buf_imagerow;      // Temporary storage for received image data
uint8_t * rx_buf_filename;      // Temporary storage for received filename data
uint8_t * rx_buf_location;      // Temporary storage for received location data
uint8_t * rx_buf_send;          // Temporary storage for received ack data
uint8_t rx_index;               // Received data index
uint8_t rx_type;                // Recieved packet type



/* Image Row Packet 
-Header:  H1, H2, H3
-Packet Type: 0xAA
-Data (482 Bytes): 
    - Int defining row number (2 Bytes)
    - Array of 240 Ints (480 Bytes)
-CheckSum

 * File Name Packet
-Header:  H1, H2, H3
-Packet Type: 0xBB
-Data (30 Bytes):
  - Char Array containing filename (30 Bytes)
-CheckSum

 * Location Packet
-Header:  H1, H2, H3
-Packet Type: 0xCC
-Data (30 Bytes):
  - Char Array containing location (30 Bytes)
-CheckSum

 * Send Packet (to request data or send acknowledgements)
-Header:  H1, H2, H3
-Packet Type: 0xDD
-Data (1 Byte):
  - Char representing the data
          0x06 = ACK (Positive acknowledgement of correct data)
          0x15 = NACK (Negative acknowledgement of wrong data)
          0x16 = SO (Shift Out = Previous Image Request)
          0x17 = SI (Shift In = Next Image Request)
-CheckSum
*/

/* ****************************************************************************** */
/* ******************************   Functions  ********************************** */
/* ****************************************************************************** */

/* Initialize the variables for sending/receiving  */
void initppSerial(){
  imagerowpacketsize = 482;
  filenamepacketsize = 30;
  locationpacketsize = 30;
  sendpacketsize = 1;
  rx_buf_imagerow = (uint8_t*) malloc(imagerowpacketsize);
  rx_buf_filename = (uint8_t*) malloc(filenamepacketsize);
  rx_buf_send = (uint8_t*) malloc(sendpacketsize);
  rx_array_index = 0;
  rx_type = 0;
  rx_index = 0;
}

/* Sends an Positive Acknowledgement of received data */
void sendACK(){
  CS = 0xDD;
  Serial.write(H1);     // HEADER
  Serial.write(H2);     // HEADER
  Serial.write(H3);     // HEADER
  Serial.write(0xDD);   // PACKET TYPE
  CS^=ACK;
  Serial.write(ACK);    // Acknowledgement
  Serial.write(CS);     // Checksum
  
  // DEBUGGING USE
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, GREEN);
}

/* Sends an Negative Acknowledgement of not receiving correct data */
void sendNACK(){
  CS = 0xDD;
  Serial.write(H1);     // HEADER
  Serial.write(H2);     // HEADER
  Serial.write(H3);     // HEADER
  Serial.write(0xDD);   // PACKET TYPE
  CS^=NACK;
  Serial.write(NACK);    // Acknowledgement
  Serial.write(CS);     // Checksum
  
  // DEBUGGING USE
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
}

/* Requests the Previous Image */
void requestPrevious(){
  CS = 0xDD;
  Serial.write(H1);     // HEADER
  Serial.write(H2);     // HEADER
  Serial.write(H3);     // HEADER
  Serial.write(0xDD);   // PACKET TYPE
  CS^=PREVIOUS_IMAGE;
  Serial.write(PREVIOUS_IMAGE);    // Acknowledgement
  Serial.write(CS);     // Checksum
}

/* Requests the Next Image */
void requestNext(){
  CS = 0xDD;
  Serial.write(H1);     // HEADER
  Serial.write(H2);     // HEADER
  Serial.write(H3);     // HEADER
  Serial.write(0xDD);   // PACKET TYPE
  CS^=NEXT_IMAGE;
  Serial.write(NEXT_IMAGE);    // Acknowledgement
  Serial.write(CS);     // Checksum
}

/* Receives the struct data */
char receiveData(void){
  if(rx_type == 0){
    // Haven't found data header yet
    if(Serial.available() >= 3){
      // Found enough bytes for the header
      while(Serial.read() != H1){
        // Found first byte
        if(Serial.available() < 2){
          return 0;
        }
      }
      if(Serial.read() == H2){
        if(Serial.read() == H3){
          // Found header, next byte is the packet type
          rx_type = Serial.read();
          if(rx_type != 0xAA && rx_type != 0xBB && rx_type != 0xCC && rx_type != 0xDD){
            // Wrong Packet Type
            Serial.write(rx_type);
            rx_type = 0;
            return 0;
          }
          // Otherwise, found correct packet and can now read in the rest of the data
        }
      }
    }
  }
  
  if(rx_type == 0xAA){
    // Found Image Row Packet, read in data
      while(Serial.available() && rx_index<=imagerowpacketsize){
        rx_buf_imagerow[rx_index++] = Serial.read();
      }
      
      if(imagerowpacketsize == (rx_index - 1)){
        // Received the entire message, now check checksum
        receivedCS = rx_type;
        for(int i = 0; i < imagerowpacketsize; i++){
          receivedCS^=rx_buf_imagerow[i];
        }
        
        if(receivedCS == rx_buf_imagerow[rx_index-1]){
          // Found correct packet!
          memcpy(&imagerowdata, rx_buf_imagerow, imagerowpacketsize);
          rx_type = 0;
          rx_index = 0;
          
          // SEND POSITIVE ACKNOWLEDGEMENT
          sendACK();
          
          return 1;
        }else{
          // Incorrect packet!
          rx_type = 0;
          rx_index = 0;
          
          // SEND NEGATIVE ACKNOWLEDGEMENT
          sendNACK();
          return 0;
        }
      }
  }else if(rx_type == 0xBB){
    // Found File Name Packet, read in data
      while(Serial.available() && rx_index<=filenamepacketsize){
        rx_buf_filename[rx_index++] = Serial.read();
      }
      
      if(filenamepacketsize == (rx_index - 1)){
        // Received the entire message, now check checksum
        receivedCS = rx_type;
        for(int i = 0; i < filenamepacketsize; i++){
          receivedCS^=rx_buf_filename[i];
        }
        
        if(receivedCS == rx_buf_filename[rx_index-1]){
          // Found correct packet!
          memcpy(&filename, rx_buf_filename, filenamepacketsize);
          rx_type = 0;
          rx_index = 0;
          
          // SEND POSITIVE ACKNOWLEDGEMENT
          sendACK();
          
          return 2;
        }else{
          // Incorrect packet!
          rx_type = 0;
          rx_index = 0;
          
          // SEND NEGATIVE ACKNOWLEDGEMENT
          sendNACK();
          return 0;
        }
      }
  }else if(rx_type == 0xCC){
    // Found Location Packet, read in data
      while(Serial.available() && rx_index<=locationpacketsize){
        rx_buf_location[rx_index++] = Serial.read();
      }
      
      if(locationpacketsize == (rx_index - 1)){
        // Received the entire message, now check checksum
        receivedCS = rx_type;
        for(int i = 0; i < locationpacketsize; i++){
          receivedCS^=rx_buf_location[i];
        }
        
        if(receivedCS == rx_buf_location[rx_index-1]){
          // Found correct packet!
          memcpy(&location, rx_buf_location, locationpacketsize);
          rx_type = 0;
          rx_index = 0;
          
          // SEND POSITIVE ACKNOWLEDGEMENT
          sendACK();
          
          return 3;
        }else{
          // Incorrect packet!
          rx_type = 0;
          rx_index = 0;
          
          // SEND NEGATIVE ACKNOWLEDGEMENT
          sendNACK();
          return 0;
        }
      }
  }else if(rx_type == 0xDD){
    // Found Send Packet, read in data
      while(Serial.available() && rx_index<=sendpacketsize){
        rx_buf_send[rx_index++] = Serial.read();
      }
      
      if(sendpacketsize == (rx_index - 1)){
        // Received the entire message, now check checksum
        receivedCS = rx_type;        
        for(int i = 0; i < sendpacketsize; i++){
          receivedCS^=rx_buf_send[i];
        }
        
        if(receivedCS == rx_buf_send[rx_index-1]){
          // Found correct packet!
          memcpy(&command, rx_buf_send, sendpacketsize);
          rx_type = 0;
          rx_index = 0;
          
          // SEND POSITIVE ACKNOWLEDGEMENT
          sendACK();
          
          return 4;
        }else{
          // Incorrect packet!
          rx_type = 0;
          rx_index = 0;
          
          // SEND NEGATIVE ACKNOWLEDGEMENT
          sendNACK();
          return 0;
        }
      }
  }
  return 0;
}
#endif
