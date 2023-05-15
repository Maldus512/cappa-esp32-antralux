#include <assert.h>
#include <esp_log.h>

#include "model/model.h"
#include "model_updater.h"
#include "state_machine.h"
#include "utils/utils.h"


typedef enum {
    STATE_MACHINE_EVENT_TAG_STOP,
    STATE_MACHINE_EVENT_TAG_TOGGLE,
} state_machine_event_t;


STATE_MACHINE_DEFINE(motor, state_machine_event_t);


struct model_updater {
    mut_model_t          *pmodel;
    motor_state_machine_t sm;
};


static void state_machine_event(model_updater_t updater, state_machine_event_t event);
static int  off_event_manager(state_machine_event_t event, void *user_ptr);
static int  on_event_manager(state_machine_event_t event, void *user_ptr);


static const char *TAG = "Updater";

static motor_node_t nodes[MOTOR_STATE_NUM] = {
    [MOTOR_STATE_OFF] = STATE_MACHINE_EVENT_MANAGER(off_event_manager),
    //[MOTOR_STATE_CLEANING_START] = STATE_MACHINE_EVENT_MANAGER(off_event_manager),
    //[MOTOR_STATE_CLEANING_STOP] = STATE_MACHINE_EVENT_MANAGER(off_event_manager),
    [MOTOR_STATE_ON]  = STATE_MACHINE_EVENT_MANAGER(on_event_manager),
};


model_updater_t model_updater_init(mut_model_t *pmodel) {
    model_updater_t updater = malloc(sizeof(struct model_updater));
    updater->pmodel         = pmodel;

    updater->sm.nodes      = nodes;
    updater->sm.node_index = MOTOR_STATE_OFF;

    return updater;
}


model_t *model_updater_get(model_updater_t updater) {
    assert(updater != NULL);
    return (model_t *)updater->pmodel;
}


void model_updater_speed_mod(model_updater_t updater, int mod) {
    assert(updater != NULL);
    mut_model_t *pmodel = updater->pmodel;

    // Speed can only be changed while running
    if (model_get_motor_state(pmodel) == MOTOR_STATE_ON) {
        int speed = model_get_motor_speed(pmodel);
        if (speed + mod >= MOTOR_MAX_SPEED) {
            model_set_motor_speed(pmodel, MOTOR_MAX_SPEED - 1);
        } else if (speed + mod < 0) {
            model_set_motor_speed(pmodel, 0);
        } else {
            model_set_motor_speed(pmodel, speed + mod);
        }
    }
}


void model_updater_toggle_motor(model_updater_t updater) {
    assert(updater != NULL);
    state_machine_event(updater, STATE_MACHINE_EVENT_TAG_TOGGLE);
}


void model_updater_toggle_light(model_updater_t updater) {
    assert(updater != NULL);
    model_set_light_state(updater->pmodel, !model_get_light_state(updater->pmodel));
}


static void state_machine_event(model_updater_t updater, state_machine_event_t event) {
    assert(updater != NULL);

    if (motor_send_event(&updater->sm, updater, event)) {
        ESP_LOGI(TAG, "New state %i", updater->sm.node_index);
        model_set_motor_state(updater->pmodel, updater->sm.node_index);
    }
}


static int off_event_manager(state_machine_event_t event, void *user_ptr) {
    model_updater_t updater = user_ptr;
    (void)updater;

    switch (event) {
        case STATE_MACHINE_EVENT_TAG_TOGGLE:
            return MOTOR_STATE_ON;

        default:
            return -1;
    }
}


static int on_event_manager(state_machine_event_t event, void *user_ptr) {
    model_updater_t updater = user_ptr;
    (void)updater;

    switch (event) {
        case STATE_MACHINE_EVENT_TAG_STOP:
        case STATE_MACHINE_EVENT_TAG_TOGGLE:
            return MOTOR_STATE_OFF;

        default:
            break;
    }

    return -1;
}
