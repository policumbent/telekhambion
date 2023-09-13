#include "controller.h"

Controller::Controller(uint8_t us_bp, uint8_t ds_bp, uint8_t rd_bp) {
    upshift_pin = us_bp;
    downshift_pin = ds_bp;
    radio_pin = rd_bp;

    pinMode(upshift_pin, INPUT_PULLDOWN);
    pinMode(downshift_pin, INPUT_PULLDOWN);
    pinMode(radio_pin, INPUT_PULLDOWN);
}

uint8_t Controller::encode_payload() {
    
    uint8_t shift_up = !digitalRead(upshift_pin);
    uint8_t shift_down = !digitalRead(downshift_pin);
    uint8_t radio_tx = !digitalRead(radio_pin);

    uint8_t payload = ((!shift_up)<<UP_SHIFT_MSK) | ((!shift_down)<<DOWN_SHIFT_MSK) |
                      ((!radio_tx)<<RADIO_MSK);

    return payload;
}