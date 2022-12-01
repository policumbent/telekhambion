#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const uint8_t up_shift_sig = 255;
const uint8_t down_shift_sig = 1;
const uint8_t radio_sig = 127;

RF24 radio(7,8);  //CE=7, CSN=8
const byte address[6] = "00001";

int shift_up = 0;
int shift_down = 0;
int radio_tx = 0;

/*
 *  b_su: button shift up;
 *  b_sd: button shift down;
 *  b_rd: button radio;
 */
int b_su = 2, b_sd = 3, b_rd = 4;

void setup() {
  // input pins (buttons) init
  pinMode(b_su, INPUT);
  pinMode(b_sd, INPUT);
  pinMode(b_rd, INPUT);

  // nRF24 init
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  shift_up = digitalRead(b_su);
  shift_down = digitalRead(b_sd);
  radio_tx = digitalRead(b_rd);

  if(shift_up) radio.write(&up_shift_sig, sizeof(uint8_t));
  else if(shift_down) radio.write(&down_shift_sig, sizeof(uint8_t));

  if(radio_tx) radio.write(&radio_sig, sizeof(uint8_t));
}