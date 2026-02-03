/*
*
*/

#ifndef BLINKER_H
#define BLINKER_H

#include <stdbool.h>

struct blinker_t {
  int pin;
};

bool blinker_init(struct blinker_t *self, int pin);

void blinker_on(struct blinker_t *self);

void blinker_off(struct blinker_t *self);

#endif /* BLINKER_H */
