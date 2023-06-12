#include "view.h"
#include "timer.h"
#include "utils/utils.h"


static void timer_callback(stopwatch_timer_t *timer, void *user_ptr, void *arg);


static pman_t             pman = {0};
static view_page_update_t page_update;
static stopwatch_timer_t  timer      = {0};
static view_event_t       view_event = {0};


void view_init(model_updater_t updater, pman_user_msg_cb_t command_cb) {
    pman_init(&pman, updater, command_cb);
    pman_change_page(&pman, page_main);
}


void *view_page_led_update(uint8_t led_bitmap) {
    page_update.led_bitmap = led_bitmap;
    return &page_update;
}


void view_keyboard_event(keypad_event_t *event) {
    view_event.tag       = VIEW_EVENT_TAG_BUTTON;
    view_event.as.button = *event;

    pman_event(&pman, (pman_event_t){.tag = PMAN_EVENT_TAG_USER, .as = {.user = &view_event}});
}


void view_register_timer(unsigned long period, int code) {
    stopwatch_timer_init(&timer, period, timer_callback, (void *)(uintptr_t)code);
    stopwatch_timer_set_autoreload(&timer, 1);
    stopwatch_timer_start(&timer, get_millis());
}


void view_unregister_timer(void) {
    stopwatch_timer_stop(&timer);
}


void view_manage(void) {
    stopwatch_timer_manage(&timer, get_millis(), NULL);
}


static void timer_callback(stopwatch_timer_t *timer, void *user_ptr, void *arg) {
    (void)user_ptr;

    view_event.tag      = VIEW_EVENT_TAG_TIMER;
    view_event.as.timer = (int)(uintptr_t)arg;

    pman_event(&pman, (pman_event_t){.tag = PMAN_EVENT_TAG_USER, .as = {.user = &view_event}});
}
