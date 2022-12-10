#pragma once

#include <Arduino.h>

#define UP_SHIFT_SIG    128     // signal mapped to up-shift led
#define DOWN_SHIFT_SIG  1       // signal mapped to down-shift led
#define RADIO_SIG       64      // signal mapped to radio transmission led

class Controller {
    private:
        /*  pin used for the shift up button (HIGH -> perform upshift) */ 
        uint8_t upshift_pin;
        
        /*  pin used for the shift down button (HIGH -> perform downshift) */
        uint8_t downshift_pin;

        /* 
         *  pin used for the radio communication button (HIGH -> keep radio on; 
         *  LOW -> turn off radio)
         */
        uint8_t radio_pin;

    public:
        /* 
         *  creates a Controller object and initialize its pins
         *
         *  attributes:
         *      - us_bp: upshift button pin
         *      - ds_bp: downshift button pin
         *      - rd_bp: radio button pin
         */
        Controller(uint8_t us_bp, uint8_t ds_bp, uint8_t rd_bp);

        /*
         *  returns the corresponding payload to the current pressed-button
         *  configuration
         */
        uint8_t get_payload();
};