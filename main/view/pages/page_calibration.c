#include <esp_log.h>
#include <string.h>

#include "model/model_updater.h"
#include "view/view.h"


static struct page_data {
    uint8_t blink;
} _page_data;


static uint8_t led_bitmap(model_t *pmodel, struct page_data *pdata);


static const char *TAG = "PageCalibration";


static void *create_page(pman_handle_t handle, void *extra) {
    (void)TAG;
    (void)extra;
    return &_page_data;
}


static void open_page(pman_handle_t handle, void *state) {
    (void)handle;
    (void)state;
    view_register_timer(100, 0);
}


static void close_page(void *state) {
    (void)state;
    view_unregister_timer();
}


static pman_msg_t process_page_event(pman_handle_t handle, void *state, pman_event_t event) {
    pman_msg_t        msg     = {0};
    model_updater_t   updater = pman_get_user_data(handle);
    model_t          *pmodel  = model_updater_get(updater);
    struct page_data *pdata   = state;

    msg.user_msg = view_page_led_update(led_bitmap(pmodel, pdata));

    switch (event.tag) {
        case PMAN_EVENT_TAG_OPEN:
            break;

        case PMAN_EVENT_TAG_USER: {
            view_event_t *view_event = event.as.user;

            switch (view_event->tag) {
                case VIEW_EVENT_TAG_BUTTON: {
                    keypad_event_t *pevent = &view_event->as.button;
                    switch (pevent->tag) {
                        case KEYPAD_EVENT_TAG_CLICK: {
                            switch (pevent->code) {
                                case KEYBOARD_BUTTON_LIGHT:
                                    msg.stack_msg.tag = PMAN_STACK_MSG_TAG_BACK;
                                    break;

                                case KEYBOARD_BUTTON_MOTOR:
                                    model_updater_toggle_motor_test(updater);
                                    break;

                                case KEYBOARD_BUTTON_MINUS:
                                    model_updater_initial_speed_mod(updater, -1);
                                    msg.user_msg = view_page_led_update(led_bitmap(pmodel, pdata));
                                    break;

                                case KEYBOARD_BUTTON_PLUS:
                                    model_updater_initial_speed_mod(updater, +1);
                                    msg.user_msg = view_page_led_update(led_bitmap(pmodel, pdata));
                                    break;

                                case KEYBOARD_BUTTON_PLUS_MINUS_MOTOR:
                                case KEYBOARD_BUTTON_PLUS_MINUS_LIGHT:
                                    msg.stack_msg.tag                 = PMAN_STACK_MSG_TAG_PUSH_PAGE;
                                    msg.stack_msg.as.destination.page = &page_config;
                                    break;
                            }
                            break;
                        }

                        default:
                            break;
                    }
                    break;
                }

                case VIEW_EVENT_TAG_TIMER:
                    pdata->blink = !pdata->blink;
                    msg.user_msg = view_page_led_update(led_bitmap(pmodel, pdata));
                    break;

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


static uint8_t led_bitmap(model_t *pmodel, struct page_data *pdata) {
    uint8_t result = 0;

    for (size_t i = 0; i < 4; i++) {
        result |= (model_get_initial_speed_correction(pmodel) & (1 << i));
    }

    result |= (pdata->blink) << 4;

    return result;
}


const pman_page_t page_calibration = {
    .create        = create_page,
    .open          = open_page,
    .close         = close_page,
    .process_event = process_page_event,
};
