#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define DEBUG 1

/*
 *  Signal to be sent if the corresponding button is pressed:
 *    - up_shift_sig: mapped to up-shift led
 *    - down_shift_sig: mapped to down-shift led
 *    - radio_sig: mapped to radio transmission led
 */
const uint8_t up_shift_sig    = 128;
const uint8_t down_shift_sig  = 1;
const uint8_t radio_sig       = 64;

/*
 *  7: CE (chip enable)
 *  8: CSN
 */
RF24 radio(7,8);
const byte address[6] = "00001";

/*
 *  shift_up: 0 if corresponding button is not pressed (default), 1 otherwise
 *  shift_down: 0 if corresponding button is not pressed (default), 1 otherwise
 *  radio_tx: 0 if corresponding button is not pressed (default), 1 otherwise
 */
uint8_t shift_up = 0;
uint8_t shift_down = 0;
uint8_t radio_tx = 0;

/*
 *  b_su: button shift up;
 *  b_sd: button shift down;
 *  b_rd: button radio;
 */
uint8_t b_su = 2, b_sd = 3, b_rd = 4;

/*
 *  payload: identifies the payload to be sent
 *  status: value of return of the radio.write(), 1 if ACK returned, 0 otherwise
 */
uint8_t payload = 0, status = 0;

void setup() {
  Serial.begin(9600);

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
  status = 0;

  shift_up = digitalRead(b_su);
  shift_down = digitalRead(b_sd);
  radio_tx = digitalRead(b_rd);

  payload = shift_up*up_shift_sig + shift_down*down_shift_sig 
          + radio_tx*radio_sig;

  status = radio.write(&payload, sizeof(payload));

  #if DEBUG
    Serial.print((status)?
                  "Message received correctly\n":"Message not received\n");

    Serial.println(payload);
  #endif

  if(shift_up || shift_down || radio_tx) delay(5);
  // delay(500);
}