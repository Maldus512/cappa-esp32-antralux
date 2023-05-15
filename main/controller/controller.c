#include <esp_log.h>

#include "controller.h"
#include "model/model_updater.h"
#include "peripherals/keyboard.h"
#include "peripherals/rele.h"
#include "peripherals/phase_cut.h"
#include "view/view.h"
#include "utils/utils.h"
#include "watcher.h"


static void light_state_changed(void *old_mem, const void *mem, uint16_t size, void *user_ptr, void *arg);
static void motor_state_changed(void *old_mem, const void *mem, uint16_t size, void *user_ptr, void *arg);


static watcher_t   watcher;
static const char *TAG = "Controller";


void controller_init(model_updater_t updater, mut_model_t *pmodel) {
    watcher_init(&watcher, pmodel);

    WATCHER_ADD_ENTRY(&watcher, &pmodel->light_state, light_state_changed, NULL);
    WATCHER_ADD_ENTRY(&watcher, &pmodel->motor_state, motor_state_changed, NULL);
    WATCHER_ADD_ENTRY(&watcher, &pmodel->motor_speed, motor_state_changed, NULL);
}


void controller_manage(model_updater_t updater) {
    (void)updater;
    watcher_watch(&watcher, get_millis());
}


void controller_gui_cb(pman_handle_t handle, void *msg) {
    (void)handle;

    view_page_update_t *update = msg;

    for (keyboard_led_t led = KEYBOARD_LED_1; led < KEYBOARD_LED_NUM; led++) {
        keyboard_led_update(led, (update->led_bitmap & (1 << led)) > 0);
    }
}


static void light_state_changed(void *old_mem, const void *mem, uint16_t size, void *user_ptr, void *arg) {
    (void)old_mem;
    (void)size;
    (void)arg;
    (void)user_ptr;
    (void)arg;

    uint8_t light_state = *(uint8_t *)mem;

    ESP_LOGI(TAG, "Light %i", light_state);
    rele_update(RELE_LIGHT, light_state);
}


static void motor_state_changed(void *old_mem, const void *mem, uint16_t size, void *user_ptr, void *arg) {
    (void)old_mem;
    (void)mem;
    (void)size;
    (void)arg;

    model_t *pmodel = user_ptr;

    motor_state_t motor_state = model_get_motor_state(pmodel);
    uint8_t       percentage  = model_get_current_speed_percentage(pmodel);
    ESP_LOGI(TAG, "Motor state %i (%i%%)", motor_state, percentage);

    switch (motor_state) {
        case MOTOR_STATE_OFF:
            RELE_CLEAR(RELE_GAS);
            RELE_CLEAR(RELE_MOTOR);
            break;
        case MOTOR_STATE_ON:
            RELE_SET(RELE_GAS);
            RELE_SET(RELE_MOTOR);
            phase_cut_set_percentage(percentage);
            break;
    }
}
