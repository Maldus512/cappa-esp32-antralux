#include "view.h"


static pman_t pman = {0};


static view_page_update_t page_update;


void view_init(model_updater_t updater, pman_user_msg_cb_t command_cb) {
    pman_init(&pman, updater, command_cb);
    pman_change_page(&pman, page_main);
}


void *view_page_led_update(uint8_t led_bitmap) {
    page_update.led_bitmap = led_bitmap;
    return &page_update;
}


void view_keyboard_event(keypad_event_t *event) {
    pman_event(&pman, (pman_event_t){.tag = PMAN_EVENT_TAG_USER, .as = {.user = event}});
}
