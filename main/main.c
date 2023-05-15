#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_log.h"

#include "model/model_updater.h"
#include "controller/controller.h"
#include "peripherals/keyboard.h"
#include "peripherals/heartbit.h"
#include "peripherals/rele.h"
#include "peripherals/phase_cut.h"


static const char *TAG = "Main";


void app_main(void) {
    mut_model_t     model;
    model_updater_t updater = model_updater_init(&model);

    heartbeat_init(2000UL);
    phase_cut_init();
    keyboard_init();
    rele_init();

    model_init(&model);
    controller_init(updater, &model);
    view_init(updater, controller_gui_cb);

    ESP_LOGI(TAG, "Begin main loop");
    for (;;) {
        controller_gui_manage();
        controller_manage(updater);
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}
