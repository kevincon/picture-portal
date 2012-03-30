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



/* ****************************************************************************** */
/* ********************   Configuration Definitions  **************************** */
/* ****************************************************************************** */
uint8_t * address;  //address of struct
uint8_t size;       //size of struct
uint8_t * rx_buffer; //address for temporary storage and parsing buffer
uint8_t rx_array_inx;  //index for RX parsing buffer
uint8_t rx_len;		//RX packet length according to the packet
uint8_t calc_CS;	   //calculated Chacksum

/* ****************************************************************************** */
/* ******************************   Functions  ********************************** */
/* ****************************************************************************** */


//Captures address and size of struct
void EasyTransfer::begin(uint8_t * ptr, uint8_t length, HardwareSerial *theSerial){
	address = ptr;
	size = length;
	_serial = theSerial;

	//dynamic creation of rx parsing buffer in RAM
	rx_buffer = (uint8_t*) malloc(size);

}




//Sends out struct in binary, with header, length info and checksum
void EasyTransfer::sendData(){
  uint8_t CS = size;
  _serial->write(0x06);
  _serial->write(0x85);
  _serial->write(size);
  for(int i = 0; i<size; i++){
    CS^=*(address+i);
    _serial->write(*(address+i));
  }
  _serial->write(CS);
  
}

boolean EasyTransfer::receiveData(){
  
  //start off by looking for the header bytes. If they were already found in a previous call, skip it.
  if(rx_len == 0){
  //this size check may be redundant due to the size check below, but for now I'll leave it the way it is.
    if(_serial->available() >= 3){
	//this will block until a 0x06 is found or buffer size becomes less then 3.
      while(_serial->read() != 0x06) {
		//This will trash any preamble junk in the serial buffer
		//but we need to make sure there is enough in the buffer to process while we trash the rest
		//if the buffer becomes too empty, we will escape and try again on the next call
		if(_serial->available() < 3)
			return false;
		}
      if (_serial->read() == 0x85){
        rx_len = _serial->read();
		//make sure the binary structs on both Arduinos are the same size.
        if(rx_len != size){
          rx_len = 0;
          return false;
        }
      }
    }
  }
  
  
  
  if(rx_len != 0){
    while(_serial->available() && rx_array_inx <= rx_len){
      rx_buffer[rx_array_inx++] = _serial->read();
    }
    
    if(rx_len == (rx_array_inx-1)){
      //seem to have got whole message
      //last uint8_t is CS
      calc_CS = rx_len;
      for (int i = 0; i<rx_len; i++){
        calc_CS^=rx_buffer[i];
      } 
      
      if(calc_CS == rx_buffer[rx_array_inx-1]){//CS good
        memcpy(address,rx_buffer,size);
		rx_len = 0;
		rx_array_inx = 0;
		return true;
		}
        
	  else{
	  //failed checksum, need to clear this out anyway
		rx_len = 0;
		rx_array_inx = 0;
		return false;
	  }	
		
    }
  }
  
  return false;
}
