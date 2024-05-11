#ifndef BAREOS_APPDEVELOPMENT_IMAGE_H
#define BAREOS_APPDEVELOPMENT_IMAGE_H

#include "lib/gpio.h"
#include "lib/game_be.h"

#define STEP 10

extern void draw_picture(const uint64_t *img, const uint64_t w, const uint64_t h);
extern void move_picture(const Direction d);

#endif
