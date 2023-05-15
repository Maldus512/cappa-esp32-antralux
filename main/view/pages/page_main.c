#include <esp_log.h>

#include "model/model_updater.h"
#include "view/view.h"


static uint8_t led_bitmap(model_t *pmodel);


static struct {

} _page_data;


static const char *TAG = "PageMain";


static void *create_page(pman_handle_t handle, void *extra) {
    (void)TAG;
    (void)extra;
    return &_page_data;
}


static void close_page(void *state) {
    (void)state;
    // TODO: turn off the motor
}


static pman_msg_t process_page_event(pman_handle_t handle, void *state, pman_event_t event) {
    pman_msg_t      msg     = {0};
    model_updater_t updater = pman_get_user_data(handle);
    model_t        *pmodel  = model_updater_get(updater);

    msg.user_msg = view_page_led_update(led_bitmap(pmodel));


    switch (event.tag) {
        case PMAN_EVENT_TAG_USER: {
            keypad_event_t *pevent = event.as.user;
            switch (pevent->tag) {
                case KEYPAD_EVENT_TAG_CLICK: {
                    switch (pevent->code) {
                        case KEYBOARD_BUTTON_LIGHT:
                            model_updater_toggle_light(updater);
                            break;

                        case KEYBOARD_BUTTON_MOTOR:
                            model_updater_toggle_motor(updater);
                            break;

                        case KEYBOARD_BUTTON_MINUS:
                            model_updater_speed_mod(updater, -1);
                            msg.user_msg = view_page_led_update(led_bitmap(pmodel));
                            break;

                        case KEYBOARD_BUTTON_PLUS:
                            model_updater_speed_mod(updater, +1);
                            msg.user_msg = view_page_led_update(led_bitmap(pmodel));
                            break;
                    }
                    break;
                }

                default:
                    break;
            }
            break;
        }

        default:
            break;
    }

    return msg;
}


static uint8_t led_bitmap(model_t *pmodel) {
    uint8_t result = 0;
    for (size_t i = 0; i < model_get_motor_speed(pmodel) + 1; i++) {
        result |= 1 << i;
    }
    return result;
}


const pman_page_t page_main = {
    .create        = create_page,
    .close         = close_page,
    .process_event = process_page_event,
};
