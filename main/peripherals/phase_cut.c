#include "driver/gpio.h"
#include "driver/timer.h"
#include "freertos/FreeRTOS.h"
#include "hal/gpio_types.h"
#include "hardwareprofile.h"
#include "esp_log.h"
#include "phase_cut.h"
#include <stdbool.h>
#include <sys/_stdint.h>

static void zcross_isr_handler(void *arg);

static volatile uint32_t period   = 0;
static volatile uint32_t counter1 = {0};
static volatile size_t   triac    = 0;
static int               full     = 0;

static const char *TAG = "PhaseCut";

#define TIMER_ZCROSS_GROUP  TIMER_GROUP_0
#define TIMER_ZCROSS_IDX    TIMER_0
#define TIMER_RESOLUTION_HZ 1000000     // 1MHz resolution
#define TIMER_USECS(x)      (x * (TIMER_RESOLUTION_HZ / 1000000))
#define TIMER_PERIOD        TIMER_USECS(100)
#define PHASE_HALFPERIOD    TIMER_USECS(10000)

#define MAX_PERIOD 9500UL
#define MIN_PERIOD 500UL

static const uint16_t sine_percentage_linearization[100] = {
    9999, 9361, 9095, 8890, 8716, 8563, 8423, 8294, 8173, 8058, 7950, 7846, 7746, 7650, 7556, 7466, 7378,
    7292, 7208, 7126, 7046, 6967, 6889, 6813, 6738, 6664, 6591, 6519, 6447, 6377, 6307, 6238, 6169, 6101,
    6034, 5967, 5900, 5834, 5768, 5703, 5638, 5573, 5508, 5444, 5380, 5315, 5252, 5188, 5124, 5060, 4996,
    4933, 4869, 4805, 4741, 4677, 4613, 4549, 4485, 4420, 4355, 4290, 4225, 4159, 4093, 4026, 3959, 3892,
    3824, 3755, 3686, 3616, 3546, 3474, 3402, 3329, 3255, 3180, 3104, 3026, 2947, 2867, 2785, 2701, 2615,
    2527, 2437, 2343, 2247, 2147, 2043, 1935, 1820, 1699, 1570, 1430, 1276, 1103, 898,  632,
};


static bool IRAM_ATTR timer_phasecut_callback(void *args) {
    (void)args;

    if (triac > 0) {
        triac--;
        if (triac == 0) {
            gpio_set_level(HAP_PWM, 0);
        }
    }

    if (counter1 > 0) {
        if (counter1 > 100) {
            counter1 -= 100;
        } else {
            counter1 = 0;
        }
        if (counter1 == 0) {
            gpio_set_level(HAP_PWM, 1);
            triac = 5;
        }
    }

    return 0;
}

void phase_cut_set_period(uint32_t usecs) {
    period = usecs;
    // ESP_LOGI(TAG,"%lu", period);
}

void phase_cut_timer_enable(int enable) {
    if (enable) {
        // For the timer counter to a initial value
        ESP_ERROR_CHECK(timer_set_counter_value(TIMER_ZCROSS_GROUP, TIMER_ZCROSS_IDX, 0));
        // Start timer
        ESP_ERROR_CHECK(timer_start(TIMER_ZCROSS_GROUP, TIMER_ZCROSS_IDX));
    } else if (!enable) {
        timer_pause(TIMER_ZCROSS_GROUP, TIMER_ZCROSS_IDX);
    }
}


void phase_cut_init(void) {
    timer_config_t config = {
        .divider     = APB_CLK_FREQ / TIMER_RESOLUTION_HZ,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en  = TIMER_PAUSE,
        .alarm_en    = TIMER_ALARM_EN,
        .auto_reload = true,
    };
    ESP_ERROR_CHECK(timer_init(TIMER_ZCROSS_GROUP, TIMER_ZCROSS_IDX, &config));
    ESP_ERROR_CHECK(timer_isr_callback_add(TIMER_ZCROSS_GROUP, TIMER_ZCROSS_IDX, timer_phasecut_callback, NULL, 0));
    ESP_ERROR_CHECK(timer_enable_intr(TIMER_ZCROSS_GROUP, TIMER_ZCROSS_IDX));

    // For the timer counter to a initial value
    ESP_ERROR_CHECK(timer_set_counter_value(TIMER_ZCROSS_GROUP, TIMER_ZCROSS_IDX, 0));
    ESP_ERROR_CHECK(timer_set_alarm_value(TIMER_ZCROSS_GROUP, TIMER_ZCROSS_IDX, TIMER_PERIOD));
    ESP_ERROR_CHECK(timer_set_alarm(TIMER_ZCROSS_GROUP, TIMER_ZCROSS_IDX, 1));

    ESP_ERROR_CHECK(timer_start(TIMER_ZCROSS_GROUP, TIMER_ZCROSS_IDX));


    gpio_config_t io_conf_input = {
        // interrupt of falling edge
        .intr_type = GPIO_INTR_ANYEDGE,
        // bit mask of the pins
        .pin_bit_mask = (1ULL << HAP_INT0),
        // set as input mode
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = 0,
        .pull_down_en = 0,
    };
    gpio_config(&io_conf_input);

    gpio_config_t io_conf_output = {
        // interrupt of falling edge
        .intr_type = GPIO_INTR_DISABLE,
        // bit mask of the pins
        .pin_bit_mask = (1ULL << HAP_PWM),
        // set as input mode
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = 0,
        .pull_down_en = 0,
    };
    gpio_config(&io_conf_output);

    // install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    // hook isr handler for specific gpio pin
    gpio_isr_handler_add(HAP_INT0, zcross_isr_handler, NULL);
}


void phase_cut_set_percentage(unsigned int perc) {
    if (perc >= 100) {
        perc = 100;
        full = 1;
        phase_cut_timer_enable(0);
    } else if (perc == 0) {
        full = 0;
        phase_cut_timer_enable(0);
    } else {
        full   = 0;
        //perc   = 100 - perc;
        period = sine_percentage_linearization[perc - 1];
        phase_cut_timer_enable(1);
    }
    ESP_LOGI(TAG, "%i %6lu (%i)", perc, period, full);
}


static void IRAM_ATTR zcross_isr_handler(void *arg) {
    if (full) {
        gpio_set_level(HAP_PWM, 1);

    } else {
        // gpio_set_level(HAP_PWM, 0);
        // gpio_set_level(HAP_POMPA2, 0);
    }

    counter1 = period;
}
