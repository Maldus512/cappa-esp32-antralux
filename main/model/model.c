#include <assert.h>
#include "model.h"


#define CLEANING_PERIOD_SEC 15


void model_init(mut_model_t *pmodel) {
    pmodel->motor_speed              = 0;
    pmodel->motor_state              = MOTOR_STATE_OFF;
    pmodel->light_state              = 0;
    pmodel->initial_speed_correction = 5;

    pmodel->percentages[0] = 20;
    pmodel->percentages[1] = 40;
    pmodel->percentages[2] = 60;
    pmodel->percentages[3] = 80;
    pmodel->percentages[4] = 100;

    pmodel->cleaning_config = CLEANING_CONFIG_FULL;
}


uint8_t model_get_current_speed_percentage(model_t *pmodel) {
    assert(pmodel != NULL);
    if (model_get_motor_speed(pmodel) == 0) {
        return pmodel->percentages[0] + pmodel->initial_speed_correction;
    } else {
        return pmodel->percentages[model_get_motor_speed(pmodel)];
    }
}


uint8_t model_get_cleaning_period_start(model_t *pmodel) {
    assert(pmodel != NULL);
    switch (pmodel->cleaning_config) {
        case CLEANING_CONFIG_FULL:
        case CLEANING_CONFIG_START:
            return CLEANING_PERIOD_SEC;
        case CLEANING_CONFIG_STOP:
        case CLEANING_CONFIG_NONE:
            return 0;
    }
    return 0;
}


uint8_t model_get_cleaning_period_stop(model_t *pmodel) {
    assert(pmodel != NULL);
    switch (pmodel->cleaning_config) {
        case CLEANING_CONFIG_NONE:
        case CLEANING_CONFIG_START:
            return 0;
        case CLEANING_CONFIG_STOP:
        case CLEANING_CONFIG_FULL:
            return CLEANING_PERIOD_SEC;
    }
    return 0;
}
