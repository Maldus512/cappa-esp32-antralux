#include <assert.h>
#include "model.h"


void model_init(mut_model_t *pmodel) {
    pmodel->motor_speed = 0;
    pmodel->motor_state = MOTOR_STATE_OFF;
    pmodel->light_state = 0;

    pmodel->percentages[0] = 20;
    pmodel->percentages[1] = 40;
    pmodel->percentages[2] = 60;
    pmodel->percentages[3] = 80;
    pmodel->percentages[4] = 100;

    pmodel->cleaning_period_start = 15;
    pmodel->cleaning_period_stop  = 15;
}


uint8_t model_get_current_speed_percentage(model_t *pmodel) {
    assert(pmodel != NULL);
    return pmodel->percentages[model_get_motor_speed(pmodel)];
}
