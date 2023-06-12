#ifndef VIEW_H_INCLUDED
#define VIEW_H_INCLUDED

#include <stdint.h>

#include "model/model_updater.h"
#include "page_manager.h"
#include "keypad.h"
#include "peripherals/keyboard.h"


#define VIEW_LED_1 0x01
#define VIEW_LED_2 0x02
#define VIEW_LED_3 0x04
#define VIEW_LED_4 0x08
#define VIEW_LED_5 0x10


typedef enum {
    VIEW_EVENT_TAG_BUTTON = 0,
    VIEW_EVENT_TAG_TIMER,
} view_event_tag_t;


typedef struct {
    view_event_tag_t tag;
    union {
        int            timer;
        keypad_event_t button;
    } as;
} view_event_t;


typedef struct {
    uint8_t led_bitmap;
} view_page_update_t;


void  view_init(model_updater_t updater, pman_user_msg_cb_t command_cb);
void *view_page_led_update(uint8_t led_bitmap);
void  view_keyboard_event(keypad_event_t *event);
void  view_manage(void);
void  view_register_timer(unsigned long period, int code);
void  view_unregister_timer(void);


extern const pman_page_t page_main, page_calibration, page_config;


#endif
