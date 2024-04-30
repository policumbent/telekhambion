#ifndef __TELEKHAMBION_H__
#define __TELEKHAMBION_H__


#include "main.h"
#include "usart.h"


#define BUTTON_POLLING_PERIOD 50u
#define DOWNSHIFT_MASK 7u
#define UPSHIFT_MASK   4u
#define RADIO_MASK     0u


/**
 * @brief button_t: struct that contains pin state for polling-implemented
 * debouncing
 */
typedef struct {
    uint8_t val_previous;
    uint8_t val_current;
    uint8_t val_debounced;
    uint8_t is_toggled;
    GPIO_TypeDef *port;
    uint16_t pin;
} button_t;


void controller_button_init();
uint8_t controller_encode_payload(uint8_t *update);


#endif /* __TELEKHAMBION_H__ */