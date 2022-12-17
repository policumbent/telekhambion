#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <controller.h>

#define DEBUG 1

#define US_PB 2  // shift up pin button
#define DS_PB 3  // shift down pin button
#define RD_PB 4  // radio pin button

/*
 *  nrf identifies the nRF24L01 object
 *  
 *  Attributes:
 *    - 7: CE (chip enable)
 *    - 8: CSN (chip select)
 */
RF24 nrf(7, 8);
const byte address[][6] = {"00001","00010"};


/*  Controller object */
Controller controller(US_PB, DS_PB, RD_PB);

/*  identifies the payload to be sent */
uint8_t payload = 0;

/*  radio.write() return value: 1 if ACK returned; 0 otherwise */
uint8_t status = 0;

void setup() {
  Serial.begin(9600);

  // nRF24 init (Shift)
  nrf.begin();
  nrf.setPALevel(RF24_PA_MIN);
  nrf.stopListening();
}

void loop() {
  status = 0;
  payload = controller.encode_payload();
  

  nrf.openWritingPipe(address[0]);
  status = nrf.write(&payload, sizeof(payload));

  #if DEBUG
    Serial.print((status)?
                  "Message (shift) received correctly\n":"Message (shift) not received\n");
    Serial.println(payload);
  #endif

  nrf.openWritingPipe(address[1]);
  status = nrf.write(&payload, sizeof(payload));

  #if DEBUG
    Serial.print((status)?
                  "Message (radio) received correctly\n":"Message not received\n");
    Serial.println(payload);
  #endif
}