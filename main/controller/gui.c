#include <esp_log.h>

#include "peripherals/keyboard.h"
#include "view/view.h"


static const char *TAG = "Gui";


void controller_gui_manage(void) {
    (void)TAG;

    keypad_event_t event = {0};
    if (keyboard_get_last_event(&event)) {
        view_keyboard_event(&event);
    }
}

