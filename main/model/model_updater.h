#ifndef MODEL_UPDATER_H_INCLUDED
#define MODEL_UPDATER_H_INCLUDED


#include "model.h"


typedef struct model_updater *model_updater_t;


model_updater_t model_updater_init(mut_model_t *pmodel);
model_t        *model_updater_get(model_updater_t updater);
void            model_updater_speed_mod(model_updater_t updater, int mod);
void            model_updater_toggle_light(model_updater_t updater);
void            model_updater_toggle_motor(model_updater_t updater);
void            model_updater_manage(model_updater_t updater);
void            model_updater_initial_speed_mod(model_updater_t updater, int mod);
void            model_updater_toggle_motor_test(model_updater_t updater);
void            model_updater_cleaning_config_mod(model_updater_t updater, int mod);


#endif
