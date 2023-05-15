#ifndef RELE_H_INCLUDED
#define RELE_H_INCLUDED


#include <stdint.h>


#define RELE_NUM      (RELE_MOTOR - RELE_GAS + 1)
#define RELE_SET(r)   rele_update(r, 1)
#define RELE_CLEAR(r) rele_update(r, 0)


typedef enum {
    RELE_GAS = 0,
    RELE_LIGHT,
    RELE_MOTOR,
} rele_t;


void rele_init(void);
void rele_update(rele_t rele, uint8_t level);


#endif
