# Telekhambion

This repository is the firmware for the transmitter 
part of the remote control system for Cerberus' gearbox.

It is written using ST framework.

## Boards

- MCU: STM32 NUCLEO-L031K6
- RF module: nRF24L01+

### Buttons

- PA12 : upshift pin, defined as B_UPSHIFT
- PB0 : downshift pin, defined as B_DOWNSHIFT
- PB7 : radio pin, defined as B_RADIO

### nRF24L01+ and SPI connections

- PA2 : nrf24l01+ chip enable
- PA1 : spi chip select
- PA5 : spi serial clock
- PA7 : spi MOSI
- PA6 : spi MISO

### Build and Flash
The project uses the extension ```stm32-for-vscode``` for building and flashing.

---

## Correlated repositories

- [Greta repository](https://github.com/policumbent/greta)
- [Aurora repository](https://github.com/policumbent/aurora)
