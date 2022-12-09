#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <controller.h>

#define DEBUG 1

/*
 *  7: CE (chip enable)
 *  8: CSN (chip select)
 */
RF24 radio(7,8);
const byte address[6] = "00001";

#define US_PB 2  // shift up pin button
#define DS_PB 3  // shift down pin button
#define RD_PB 4  // radio pin button

Controller controller(US_PB, DS_PB, RD_PB);

/*
 *  payload: identifies the payload to be sent
 *  status: value of return of the radio.write(), 1 if ACK returned, 0 otherwise
 */
uint8_t payload = 0, status = 0;

void setup() {
  Serial.begin(9600);

  // nRF24 init
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}


void loop() {
  status = 0;

  payload = controller.get_payload();
  status = radio.write(&payload, sizeof(payload));

  #if DEBUG
    Serial.print((status)?
                  "Message received correctly\n":"Message not received\n");
    Serial.println(payload);
  #endif
}