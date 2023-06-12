#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED


#include <stdint.h>
#include "keypad.h"


#define KEYBOARD_BUTTON_NUM   (KEYBOARD_BUTTON_PLUS_MINUS_MOTOR - KEYBOARD_BUTTON_MINUS + 1)
#define KEYBOARD_LED_NUM      (KEYBOARD_LED_5 - KEYBOARD_LED_1 + 1)
#define KEYBOARD_LED_SET(l)   keyboard_led_update(l, 1);
#define KEYBOARD_LED_CLEAR(l) keyboard_led_update(l, 0);


typedef enum {
    KEYBOARD_BUTTON_MINUS = 0,
    KEYBOARD_BUTTON_PLUS,
    KEYBOARD_BUTTON_MOTOR,
    KEYBOARD_BUTTON_LIGHT,
    KEYBOARD_BUTTON_PLUS_MINUS_LIGHT,
    KEYBOARD_BUTTON_PLUS_MINUS_MOTOR,
} keyboard_button_t;


typedef enum {
    KEYBOARD_LED_1 = 0,
    KEYBOARD_LED_2,
    KEYBOARD_LED_3,
    KEYBOARD_LED_4,
    KEYBOARD_LED_5,
} keyboard_led_t;


void    keyboard_init(void);
void    keyboard_led_update(keyboard_led_t led, uint8_t value);
uint8_t keyboard_get_last_event(keypad_event_t *event);


#endif
