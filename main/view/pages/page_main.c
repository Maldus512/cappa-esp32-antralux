#include <stdio.h>
#include <esp_log.h>
#include <string.h>

#include "model/model_updater.h"
#include "view/view.h"


#define PASSWORD_LENGTH 2


static struct page_data {
    uint8_t           blink;
    size_t            password_index;
    keyboard_button_t inserted_password[PASSWORD_LENGTH];
} _page_data;


static uint8_t led_bitmap(model_t *pmodel, struct page_data *pdata);
static void    reset_password(struct page_data *pdata);


static const keyboard_button_t password[PASSWORD_LENGTH] = {
    KEYBOARD_BUTTON_PLUS_MINUS_LIGHT,
    KEYBOARD_BUTTON_PLUS_MINUS_MOTOR,
    //KEYBOARD_BUTTON_PLUS_MINUS_LIGHT,
    //KEYBOARD_BUTTON_PLUS_MINUS_MOTOR,
};
static const char *TAG = "PageMain";


static void *create_page(pman_handle_t handle, void *extra) {
    (void)TAG;
    (void)extra;
    _page_data.blink = 0;
    return &_page_data;
}


static void open_page(pman_handle_t handle, void *state) {
    (void)handle;

    struct page_data *pdata = state;

    reset_password(pdata);
    view_register_timer(1000UL, 0);
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
                                    reset_password(pdata);

                                    model_updater_toggle_light(updater);
                                    break;

                                case KEYBOARD_BUTTON_MOTOR:
                                    reset_password(pdata);

                                    pdata->blink = 1;
                                    model_updater_toggle_motor(updater);
                                    msg.user_msg = view_page_led_update(led_bitmap(pmodel, pdata));
                                    break;

                                case KEYBOARD_BUTTON_MINUS:
                                    reset_password(pdata);

                                    model_updater_speed_mod(updater, -1);
                                    msg.user_msg = view_page_led_update(led_bitmap(pmodel, pdata));
                                    break;

                                case KEYBOARD_BUTTON_PLUS:
                                    reset_password(pdata);

                                    model_updater_speed_mod(updater, +1);
                                    msg.user_msg = view_page_led_update(led_bitmap(pmodel, pdata));
                                    break;

                                case KEYBOARD_BUTTON_PLUS_MINUS_MOTOR:
                                case KEYBOARD_BUTTON_PLUS_MINUS_LIGHT: {
                                    if (model_get_motor_state(pmodel) != MOTOR_STATE_OFF) {
                                        // Only when stopped
                                        reset_password(pdata);
                                        break;
                                    }

                                    uint8_t password_ok = 1;

                                    pdata->inserted_password[pdata->password_index] = pevent->code;
                                    pdata->password_index = (pdata->password_index + 1) % PASSWORD_LENGTH;

                                    for (size_t i = 0; i < PASSWORD_LENGTH; i++) {
                                        size_t position = (pdata->password_index + i) % PASSWORD_LENGTH;
                                        if (pdata->inserted_password[position] != password[position]) {
                                            password_ok = 0;
                                            break;
                                        }
                                    }

                                    if (password_ok) {
                                        msg.stack_msg.tag                 = PMAN_STACK_MSG_TAG_CHANGE_PAGE;
                                        msg.stack_msg.as.destination.page = &page_calibration;
                                    }
                                    break;
                                }
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

    switch (model_get_motor_state(pmodel)) {
        case MOTOR_STATE_OFF:
            result = 0;
            break;

        case MOTOR_STATE_STARTING:
        case MOTOR_STATE_STOPPING:
        case MOTOR_STATE_CLEANING_START:
        case MOTOR_STATE_CLEANING_STOP:
            if (pdata->blink) {
                for (size_t i = 0; i < model_get_motor_speed(pmodel) + 1; i++) {
                    result |= 1 << i;
                }
            } else {
                result = 0;
            }
            break;

        case MOTOR_STATE_ON:
            for (size_t i = 0; i < model_get_motor_speed(pmodel) + 1; i++) {
                result |= 1 << i;
            }
            break;
    }

    return result;
}


static void reset_password(struct page_data *pdata) {
    pdata->password_index = 0;
    memset(pdata->inserted_password, 0, sizeof(pdata->inserted_password));
}


const pman_page_t page_main = {
    .create        = create_page,
    .open          = open_page,
    .close         = close_page,
    .process_event = process_page_event,
};
