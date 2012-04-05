/* Kevin Conley, William Etter, Teddy Zhang
 * CIS-542 - Final Project
 * Spring 2012
 * Serial Communication Implementation - C File
 */

#include "serialcomm.h"
#include "picture-portal.h"
#include <sys/filio.h>

char temp;
char Headerone;
char Headertwo;
char Headerthree;

/*
 * Initialize the variables for sending/receiving
 */
void initppSerial(void){
    imagerowpacketsize = (uint8_t)482;
    filenamepacketsize = 30;
    locationpacketsize = 30;
    sendpacketsize = 1;
    rx_buf_imagerow = (uint8_t*) malloc(imagerowpacketsize);
    rx_buf_filename = (uint8_t*) malloc(filenamepacketsize);
    rx_buf_send = (uint8_t*) malloc(sendpacketsize);
    rx_array_index = 0;
    rx_type = 0;
    rx_index = 0;
    Headerone = H1;
    Headertwo = H2;
    Headerthree = H3;
}
 
 
 /* 
  * Sends an Positive Acknowledgement of received data 
  */
void sendACK(void){
    CS = 0xDD;
    write(serial_fd, &Headerone, 1);      // HEADER
    write(serial_fd, &Headertwo, 1);      // HEADER
    write(serial_fd, &Headerthree, 1);      // HEADER
    temp = 0xDD;
    write(serial_fd, &temp, 1);    // PACKET TYPE
    CS^=ACK;
    temp = ACK;
    write(serial_fd, &temp, 1);    // Acknowledgement
    write(serial_fd, &CS, 1);
}

/*
 * Sends an Negative Acknowledgement of not receiving correct data
 */
void sendNACK(void){
    CS = 0xDD;
    write(serial_fd, &Headerone, 1);      // HEADER
    write(serial_fd, &Headertwo, 1);      // HEADER
    write(serial_fd, &Headerthree, 1);      // HEADER
    temp = 0xDD;
    write(serial_fd, &temp, 1);    // PACKET TYPE
    CS^=NACK;
    temp = NACK;
    write(serial_fd, &temp, 1);    // Acknowledgement
    write(serial_fd, &CS, 1);
}

/*
 * Read 1 byte from serial, store in dest
 * Return number of bytes available in serial buffer
 */
char read_bytes(uint8_t *dest) {
	char val = read(serial_fd, dest, 1);
	if (val >= 0) //include -1 for "resource temporarily unavailable"		
		return val;
	else {
		return NULL;
	}
}






/* 
 * Receives the data
 *
char receiveData(void){
    char readval;
    if(rx_type == 0){
    // Haven't found data header yet
    do{
        if(readval == Headerone){
            // Found H1
            break;
        }
    
    }while(read_bytes(&readval) != NULL);
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
*/