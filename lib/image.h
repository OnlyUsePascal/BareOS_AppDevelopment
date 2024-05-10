#ifndef BAREOS_APPDEVELOPMENT_IMAGE_H
#define BAREOS_APPDEVELOPMENT_IMAGE_H

#include "lib/gpio.h"

typedef struct image {
    const uint64_t *data, w, h;
} image;

void show_picture(const image *img);

#endif
