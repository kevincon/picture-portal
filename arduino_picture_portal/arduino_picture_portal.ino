#include "SoftEasyTransfer.h"
#include <SoftwareSerial.h>

#define BUTTON_PIN                  3

#define SERIAL_TX_PIN               6
#define SERIAL_RX_PIN               7
#define SERIAL_REQUEST_IMAGE_BYTE   0x1B

#define IMAGE_WIDTH                 128
#define IMAGE_HEIGHT                128
#define IMAGE_MAX_FILENAME_LENGTH   30 

SoftwareSerial serialComm(SERIAL_RX_PIN, SERIAL_TX_PIN);

SoftEasyTransfer ET; 

struct pp_image{
  //int frame[IMAGE_WIDTH][IMAGE_HEIGHT];
  char filename[IMAGE_MAX_FILENAME_LENGTH]; 
};

//this global var will hold data about the current image
pp_image image_data;

void setup() {
  serialComm.begin(9600);
  Serial.begin(9600);
  //start the library, pass in the data details and the name of the serial port.
  ET.begin(details(image_data), &serialComm);
  
  //setup lcd shield button for input, pull-up resistor, with interrupt on falling edge
  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(BUTTON_PIN, HIGH);
  attachInterrupt(1, button_press, FALLING);
  Serial.println("reset");
}

/* 
 * Callback for button press. Button is active-low.
 */
void button_press() {
  //poor-man's debouncing, might need to change to an empty while loop
  if(!digitalRead(BUTTON_PIN)) {
    Serial.println("Requesting new image from server...");
    serialComm.write(SERIAL_REQUEST_IMAGE_BYTE); 
  }
}

void loop() {
  
  if(ET.receiveData()){ //if we've received an image
    Serial.print("Received image: ");
    Serial.println(image_data.filename);
  }
  delay(2500);
  
  /*
  uint8_t buf;
  if(serialComm.available()) {
    buf = serialComm.read();
    Serial.println(buf, HEX);
  }
  */
}
