#ifndef HARDWAREPROFILE_H_INCLUDED
#define HARDWAREPROFILE_H_INCLUDED

/*
 * Definizioni dei pin da utilizzare
 */


#include <driver/gpio.h>

// Outputs
#define HAP_O_1 GPIO_NUM_17
#define HAP_O_2 GPIO_NUM_16
#define HAP_O_3 GPIO_NUM_15
#define HAP_O_4 GPIO_NUM_14
#define HAP_O_5 GPIO_NUM_13
#define HAP_O_6 GPIO_NUM_12
#define HAP_O_7 GPIO_NUM_11

#define HAP_P1 GPIO_NUM_18
#define HAP_P2 GPIO_NUM_19
#define HAP_P3 GPIO_NUM_20
#define HAP_P4 GPIO_NUM_21

#define HAP_REL_M GPIO_NUM_33

#define HAP_LED_RUN GPIO_NUM_38

#define HAP_INT0 GPIO_NUM_41
#define HAP_PWM  GPIO_NUM_40

#endif
