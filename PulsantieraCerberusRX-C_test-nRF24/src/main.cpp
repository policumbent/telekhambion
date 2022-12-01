#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7,8);  //CE=7, CSN=8
const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);                 // start baud rate for serial monitor
  
  // setup nRF24L01 as RX
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

uint8_t sig = 0;

void loop() {
  if(radio.available()) {
    radio.read(&sig, sizeof(sig));
    
    switch (sig) {
    case 255:
      Serial.print("Up-shift!\n");
      break;

    case 1:
      Serial.print("Down-shift!\n");
      break;

    case 127:
      Serial.print("Radio!\n");
      break;
    
    default:
      Serial.print("Corrupted signal!");
      break;
    }
  }
}