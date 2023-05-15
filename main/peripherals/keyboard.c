#include <assert.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <freertos/queue.h>
#include <esp_log.h>

#include "hardwareprofile.h"
#include "keyboard.h"
#include "keypad.h"
#include "utils/utils.h"


#define EVENT_QUEUE_SIZE 16


static void keypad_timer(TimerHandle_t timer);


static const char *TAG = "buttons";

static keypad_key_t keyboard[KEYBOARD_BUTTON_NUM + 1] = {
    {.bitvalue = 0x02, .code = KEYBOARD_BUTTON_MINUS},
    {.bitvalue = 0x01, .code = KEYBOARD_BUTTON_PLUS},
    {.bitvalue = 0x04, .code = KEYBOARD_BUTTON_MOTOR},
    {.bitvalue = 0x08, .code = KEYBOARD_BUTTON_LIGHT},
    KEYPAD_NULL_KEY,
};

static QueueHandle_t queue = NULL;


void keyboard_init(void) {
    assert(queue == NULL);

    gpio_config_t in_config = {
        .intr_type    = GPIO_INTR_DISABLE,
        .mode         = GPIO_MODE_INPUT,
        .pin_bit_mask = BIT64(HAP_P1) | BIT64(HAP_P2) | BIT64(HAP_P3) | BIT64(HAP_P4),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&in_config));

    gpio_config_t out_config = {
        .intr_type    = GPIO_INTR_DISABLE,
        .mode         = GPIO_MODE_OUTPUT,
        .pin_bit_mask = BIT64(HAP_O_3) | BIT64(HAP_O_4) | BIT64(HAP_O_5) | BIT64(HAP_O_6) | BIT64(HAP_O_7),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&out_config));

    keypad_reset_keys(keyboard);

    static StaticQueue_t static_queue;
    static uint8_t       queue_buffer[sizeof(keypad_event_t) * EVENT_QUEUE_SIZE] = {0};
    queue = xQueueCreateStatic(EVENT_QUEUE_SIZE, sizeof(keypad_event_t), queue_buffer, &static_queue);

    static StaticTimer_t static_timer;
    TimerHandle_t        timer = xTimerCreateStatic(TAG, pdMS_TO_TICKS(5), 1, NULL, keypad_timer, &static_timer);
    xTimerStart(timer, portMAX_DELAY);
}


void keyboard_led_update(keyboard_led_t led, uint8_t value) {
    gpio_num_t gpios[KEYBOARD_LED_NUM] = {HAP_O_6, HAP_O_7, HAP_O_3, HAP_O_4, HAP_O_5};
    gpio_set_level(gpios[led], value);
}


uint8_t keyboard_get_last_event(keypad_event_t *event) {
    return xQueueReceive(queue, event, 0);
}


static void keypad_timer(TimerHandle_t timer) {
    (void)timer;

    uint8_t bitmap = gpio_get_level(HAP_P1) | (gpio_get_level(HAP_P2) << 1) | (gpio_get_level(HAP_P3) << 2) |
                     (gpio_get_level(HAP_P4) << 3);
    bitmap = (~bitmap) & 0xF;
    keypad_event_t event = keypad_routine(keyboard, 40, 1500, 100, get_millis(), bitmap);
    if (event.tag != KEYPAD_EVENT_TAG_NOTHING) {
        xQueueSend(queue, &event, 0);
    }
}
