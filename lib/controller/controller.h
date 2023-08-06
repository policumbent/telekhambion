#pragma once

#include <Arduino.h>

#define UP_SHIFT_MSK    7       // shift for the mask mapped to up-shift led
#define DOWN_SHIFT_MSK  0       // shift for the mask mapped to down-shift led
#define RADIO_MSK       6       // shift for the mask mapped to radio 
                                //     transmission led

/*  Class for the remote controller */
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
         * creates a Controller object and initialize its pins
         *
         * @param us_bp upshift button pin
         * @param ds_bp downshift button pin
         * @param rd_bp radio button pin
         */
        Controller(uint8_t us_bp, uint8_t ds_bp, uint8_t rd_bp);
        /*
         *  returns the corresponding payload to the current pressed-button
         *  configuration
         */
        uint8_t encode_payload();
};