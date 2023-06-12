#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED


#include <stdint.h>
#include <stdlib.h>


#define GETTER(name, field)                                                                                            \
    static inline __attribute__((always_inline, const)) typeof(((mut_model_t *)0)->field) model_get_##name(            \
        const model_t *pmodel) {                                                                                       \
        assert(pmodel != NULL);                                                                                        \
        return pmodel->field;                                                                                          \
    }

#define SETTER(name, field)                                                                                            \
    static inline __attribute__((always_inline))                                                                       \
    uint8_t model_set_##name(mut_model_t *pmodel, typeof(((model_t *)0)->field) value) {                               \
        assert(pmodel != NULL);                                                                                        \
        if (pmodel->field != value) {                                                                                  \
            pmodel->field = value;                                                                                     \
            return 1;                                                                                                  \
        } else {                                                                                                       \
            return 0;                                                                                                  \
        }                                                                                                              \
    }

#define TOGGLER(name, field)                                                                                           \
    static inline __attribute__((always_inline)) void model_toggle_##name(mut_model_t *pmodel) {                       \
        assert(pmodel != NULL);                                                                                        \
        pmodel->field = !pmodel->field;                                                                                \
    }

#define GETTERNSETTER(name, field)                                                                                     \
    GETTER(name, field)                                                                                                \
    SETTER(name, field)



#define MOTOR_STATE_NUM (MOTOR_STATE_ON - MOTOR_STATE_OFF + 1)
#define MOTOR_MAX_SPEED 5


typedef enum {
    MOTOR_STATE_OFF = 0,
    MOTOR_STATE_STARTING,
    MOTOR_STATE_STOPPING,
    MOTOR_STATE_CLEANING_START,
    MOTOR_STATE_CLEANING_STOP,
    MOTOR_STATE_ON,
} motor_state_t;


typedef enum {
    CLEANING_CONFIG_FULL = 0,
    CLEANING_CONFIG_START,
    CLEANING_CONFIG_STOP,
    CLEANING_CONFIG_NONE,
} cleaning_config_t;


struct model {
    motor_state_t motor_state;
    size_t        motor_speed;
    uint8_t       light_state;
    uint8_t       percentages[MOTOR_MAX_SPEED];
    uint8_t       initial_speed_correction;
    uint8_t       cleaning_config;
};

typedef const struct model model_t;
typedef struct model       mut_model_t;

void    model_init(mut_model_t *model);
uint8_t model_get_current_speed_percentage(model_t *pmodel);
uint8_t model_get_cleaning_period_start(model_t *pmodel);
uint8_t model_get_cleaning_period_stop(model_t *pmodel);

GETTERNSETTER(motor_state, motor_state);
GETTERNSETTER(motor_speed, motor_speed);
GETTERNSETTER(light_state, light_state);
GETTERNSETTER(initial_speed_correction, initial_speed_correction);
GETTERNSETTER(cleaning_config, cleaning_config);

#endif
