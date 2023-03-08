# Telekhambion

This repository is used for the testing of the nRF24L01+ module for the
transmitter part of the remote control system for Cerberus' gearbox.

It is written using Platformio and the Arduino framework.

## Boards

- MCU: STM32 NUCLEO-L031K6
- RF module: nRF24L01+

## MCU Pinout

The pinout is written following the schema:
```
ST_PinName (Arduino_PinName): function
```

We include two names for the pins because the Nucleo boards are
Arduino-compatible and the name written on the board itself is different from
the real name of the pin that one can find on the MCU datasheet.

### Buttons

- PA12 (D2): upshift pin, defined as US_PB
- PB0 (D3): downshift pin, defined as DS_PB
- PB7 (D4): radio pin, defined as RD_PB

### nRF24L01+ and SPI connections

- PB6 (D5): chip enable
- PB1 (D6): chip select
- PB3 (D13): serial clock
- PB5 (D11): MOSI
- PB4 (D12): MISO

---

## Correlated repositories

- [Greta repository](https://github.com/policumbent/greta)
- [Aurora repository](https://github.com/policumbent/aurora)
