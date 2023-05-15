#ifndef CONTROLLER_H_INCLUDED
#define CONTROLLER_H_INCLUDED

#include "model/model_updater.h"
#include "view/view.h"


void controller_init(model_updater_t updater, mut_model_t *pmodel);
void controller_manage(model_updater_t updater);
void controller_gui_manage(void);
void controller_gui_cb(pman_handle_t handle, void *msg);


#endif
