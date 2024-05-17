#ifndef FRAMEBF_H_
#define FRAMEBF_H_

#include "mbox.h"

void framebf_init(int phyW, int phyH, int virW, int virH);
void framebf_drawPixel(int x, int y, unsigned int attr);
void framebf_drawRect(int x1, int y1, int x2, int y2, uint32_t attr, int fill);
void framebf_drawImg(int x, int y, int w, int h, const uint64_t *image);

#endif
