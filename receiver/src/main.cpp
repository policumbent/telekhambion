/*
 *  Test: nRF24L01 for Cerberus gearbox and radio controller
 *  Task: use buttons to light up corresponding leds that simulate one
 *        function (details in l_su, l_sd, r_tx declaration)
 */

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define DEBUG 1

/*
 *  0: CE (chip enable)
 *  15: CSN
 */
RF24 radio(2, 15);
const byte address[6] = "00001";

uint8_t data = 0;

/*
 *  l_su: pin for shift up led
 *  l_sd: pin for shift down led
 *  r_tx: pin for radio transmission led
 */
uint8_t l_su = 16, l_sd = 5, r_tx = 4;

uint8_t radio_status = 0;

void setup() {
  Serial.begin(9600);

  pinMode(l_su, OUTPUT);
  pinMode(l_sd, OUTPUT);
  pinMode(r_tx, OUTPUT);
  
  // setup nRF24L01 as RX
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  if(radio.available()) {
    radio.read(&data, sizeof(data));


    Serial.println(data);

    // radio comms open
    if(!radio_status && ((data & 64) == 64)) {
      radio_status = 1;
      digitalWrite(r_tx, HIGH);

    } else if (radio_status && ((data & 64) != 64)) {
      radio_status = 0;
      digitalWrite(r_tx, LOW);
    }

    // check gear shifts signals
    if((data & 1) == 1) {
      digitalWrite(l_sd, HIGH);

      #if DEBUG
        delay(10);  // useful for visualize the led lighting up
      #endif

      digitalWrite(l_sd, LOW);

    } else if((data & 128) == 128) {
      digitalWrite(l_su, HIGH);
      
      #if DEBUG
        delay(10);  // useful for visualize the led lighting up
      #endif

      digitalWrite(l_su, LOW);
    }
  }
}