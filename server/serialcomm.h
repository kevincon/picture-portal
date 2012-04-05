/* Kevin Conley, William Etter, Teddy Zhang
 * CIS-542 - Final Project
 * Spring 2012
 * Serial Communication Implementation - H File
 */
#ifndef _SERIAL_COMM_H_
#define _SERIAL_COMM_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

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


#define H1 0xA1
#define H2 0xB2
#define H3 0xC3


#define USBBAUD 9600                         // USBCommunication Baud Rate
#define SERIAL_REQUEST_IMAGE_BYTE 0x1B       // Communication Request Image Byte
#define IMAGE_MAX_FILENAME_LENGTH 30
#define IMAGE_LOCATION_LENGTH 30

char filename[IMAGE_MAX_FILENAME_LENGTH];
char location[IMAGE_LOCATION_LENGTH];

typedef struct IMAGE_ROW_DATA_STRUCTURE{
  uint16_t row;
  uint16_t imagedata[240];
}rowdata_t;

//give a name to the group of data
rowdata_t imagerowdata;


char command;
char dataReceived;




#define NEXT_IMAGE 0x17
#define PREVIOUS_IMAGE 0x16
#define ACK 0x06
#define NACK 0x15


/* Function Declarations */
void initppSerial(void);
void sendACK(void);
void sendNACK(void);
char receiveData(void);

#endif