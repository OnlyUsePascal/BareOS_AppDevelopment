#ifndef BAREOS_APPDEVELOPMENT_IMAGE_H
#define BAREOS_APPDEVELOPMENT_IMAGE_H

#include "gpio.h"

#define STEP 10

typedef enum img_direction {
    IMG_UP = 'w',
    IMG_DOWN = 's',
    IMG_LEFT = 'a',
    IMG_RIGHT = 'd'
} img_direction;

extern void draw_picture(const uint64_t *img, const uint64_t w, const uint64_t h);
extern void move_picture(const img_direction d);

#endif
