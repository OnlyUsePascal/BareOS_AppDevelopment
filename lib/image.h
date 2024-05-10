#ifndef BAREOS_APPDEVELOPMENT_IMAGE_H
#define BAREOS_APPDEVELOPMENT_IMAGE_H

#include "lib/gpio.h"

#define STEP 10

typedef enum direction {
    UP = 'w',
    DOWN = 's',
    LEFT = 'a',
    RIGHT = 'd'
} direction;

extern void draw_picture(const uint64_t *img, const uint64_t w, const uint64_t h);
extern void move_picture(const direction d);

#endif
