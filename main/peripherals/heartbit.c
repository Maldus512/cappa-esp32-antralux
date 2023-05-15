#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "hal/gpio_types.h"
#include "hardwareprofile.h"
#include "heartbit.h"


static const char   *TAG       = "Heartbeat";
static TimerHandle_t timer     = NULL;
static unsigned long hb_period = 1000UL;

/**
 * Timer di attivita'. Accende e spegne il led di attivita'
 */
static void heartbeat_timer(TimerHandle_t timer) {
    static int blink = 0;

    gpio_set_level(HAP_LED_RUN, blink);
    blink = !blink;

    xTimerChangePeriod(timer, pdMS_TO_TICKS(blink ? hb_period : 50UL), portMAX_DELAY);
}


void heartbeat_init(size_t period_ms) {
    gpio_config_t config = {
        .intr_type    = GPIO_INTR_DISABLE,
        .mode         = GPIO_MODE_OUTPUT,
        .pin_bit_mask = BIT64(HAP_LED_RUN),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&config));

    gpio_set_direction(HAP_LED_RUN, GPIO_MODE_OUTPUT);
    hb_period = period_ms;
    timer     = xTimerCreate(TAG, pdMS_TO_TICKS(hb_period), pdTRUE, NULL, heartbeat_timer);
    xTimerStart(timer, portMAX_DELAY);

    ESP_LOGI(TAG, "Started heartbit timer");
}
