#include "telekhambion.h"


uint32_t but_last_time_ms = 0;
uint32_t but_curr_time_ms = 0;

button_t but_downshift;
button_t but_upshift;
button_t but_radio;


uint8_t text_buf[40];


static void button_val_init(button_t *button, GPIO_TypeDef *port, uint16_t pin);
static uint8_t button_update_val(button_t *button);

/**
 * @brief Initializes buttons
 */
void controller_button_init() {
    button_val_init(&but_downshift, B_DOWNSHIFT_GPIO_Port, B_DOWNSHIFT_Pin);
    button_val_init(&but_upshift, B_UPSHIFT_GPIO_Port, B_UPSHIFT_Pin);
    button_val_init(&but_radio, B_RADIO_GPIO_Port, B_RADIO_Pin);

    but_last_time_ms = HAL_GetTick();
}

/**
 * @brief Polls every BUTTON_POLLING_PERIOD for button press and performs
 *        debouncing through button_update_val()
 * @param payload variable by reference in which will be stored the payload that
 * has to be sent
 * @retval 1 if data has to be send; 0 otherwise
 */
uint8_t controller_encode_payload(uint8_t *payload) {
    but_curr_time_ms = HAL_GetTick();
    *payload = 0;

    if (but_curr_time_ms - but_last_time_ms < BUTTON_POLLING_PERIOD) {
        return 0;
    }

    uint8_t downshift_pressed = button_update_val(&but_downshift);
    uint8_t upshift_pressed = button_update_val(&but_upshift);
    uint8_t radio_pressed = button_update_val(&but_radio);

    #if DEBUG
        sprintf((char *)text_buf, "DS: %d; US: %d; RA: %d\r\n",
                downshift_pressed, upshift_pressed, radio_pressed);
        HAL_UART_Transmit(&huart2, text_buf, strlen((char *)text_buf), 100);
    #endif /* DEBUG */

    but_last_time_ms = but_curr_time_ms;

    (*payload) = ((downshift_pressed << DOWNSHIFT_MASK) |
                  (upshift_pressed << UPSHIFT_MASK) |
                  (radio_pressed << RADIO_MASK));

    return 1;
}

/**
 * @brief Initializes button_t structs
 * @param button: by reference, the button struct to initialize
 * @param port: GPIO port
 * @param pin: GPIO pin
 */
static void button_val_init(button_t *button, GPIO_TypeDef *port, uint16_t pin) {
    button->val_previous = 0;
    button->val_current = 0;
    button->val_debounced = 0;
    button->is_toggled = 0;
    button->port = port;
    button->pin = pin;
}

/**
 * @brief FSM for button status
 * @param button: button_t struct
 * @retval uint8_t storing the debounced button value
 */
static uint8_t button_update_val(button_t *button) {
    button->val_current = HAL_GPIO_ReadPin(button->port, button->pin);

    if (button->val_previous != button->val_current) {
        button->is_toggled = 1;
        button->val_previous = button->val_current;
    } else if (button->is_toggled) {
        button->is_toggled = 0;
        button->val_debounced = button->val_previous;
    }

    return button->val_debounced;
}