#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <controller.h>

#define DEBUG 1

#define US_PB PA12  // shift up pin button
#define DS_PB PB0  // shift down pin button
#define RD_PB PB7  // radio pin button

/*
 * nrf identifies the nRF24L01 object
 * 
 * Attributes:
 *   - PB6: CE (chip enable)
 *   - PB1: CSN (chip select)
 */
RF24 nrf(PB6, PB1);
/*
 * identifies the addresses to which the controller will send the payloads:
 *     - 00001: gearbox address
 *     - 00010: radio address
 */
const byte address[][6] = {"00001","00010"};

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

  // check nRF24 connection
  #if DEBUG
    Serial.print(nrf.isChipConnected()? "nRF connected\n" :
                                        "nRF error\n");
  #endif

}

void loop() {
  status = 0;
  payload = controller.encode_payload();

  // opens pipe for gearbox communication and sends the message
  nrf.openWritingPipe(address[0]);
  status = nrf.write(&payload, sizeof(payload));

  #if DEBUG
    Serial.print((status) ? "Message (gearbox) received correctly\n" :
                            "Message (gearbox) not received\n");
    Serial.println(payload);
  #endif

  // opens pipe for radio communciation and sends the message
  nrf.openWritingPipe(address[1]);
  status = nrf.write(&payload, sizeof(payload));

  #if DEBUG
    Serial.print((status) ? "Message (radio) received correctly\n" :
                            "Message (radio) not received\n");
    Serial.println(payload);
  #endif
}