#include <driver/gpio.h>
#include <esp_log.h>

#include "esp_err.h"
#include "hardwareprofile.h"
#include "rele.h"


static const char *TAG = "Rele";


void rele_init(void) {
    gpio_config_t config = {
        .intr_type    = GPIO_INTR_DISABLE,
        .mode         = GPIO_MODE_OUTPUT,
        .pin_bit_mask = BIT64(HAP_O_1) | BIT64(HAP_O_2) | BIT64(HAP_REL_M),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&config));

    RELE_CLEAR(RELE_GAS);
    RELE_CLEAR(RELE_LIGHT);
    RELE_CLEAR(RELE_MOTOR);
}


void rele_update(rele_t rele, uint8_t level) {
    gpio_num_t gpios[RELE_NUM] = {HAP_O_1, HAP_O_2, HAP_REL_M};
    ESP_LOGI(TAG, "Setting rele %i to %i", rele, level);
    ESP_ERROR_CHECK(gpio_set_level(gpios[rele], level));
}
