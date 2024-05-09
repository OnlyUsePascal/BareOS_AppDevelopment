#include "../lib/font.h"
#include "../lib/utils.h"
#include "../lib/framebf.h"
#include "../lib/data/data_font.h"

/* Functions to display text on the screen */
void font_drawChar(int x, int y, char ch, unsigned int attr, int zoom) {
  unsigned char *glyph = (unsigned char *)&font + (ch < FONT_NUMGLYPHS ? ch : 0) * FONT_BPG;

  for (int i = 1; i <= (FONT_HEIGHT*zoom); i++) {
    for (int j = 0; j< (FONT_WIDTH*zoom); j++) {
      unsigned char mask = 1 << (j/zoom);
      if (*glyph & mask) { //only draw pixels belong to the character glyph
        framebf_drawPixel(x + j, y + i, attr);
      }
    }
    glyph += (i % zoom) ? 0 : FONT_BPL;
  }
}


void font_drawString(int x, int y, char *str, unsigned int attr, int zoom) {
  while (*str) {
    if (*str == '\r') {
      x = 0;
    } else if (*str == '\n') {
      x = 0; 
      y += (FONT_HEIGHT*zoom);
    } else {
      font_drawChar(x, y, *str, attr, zoom);
      x += (FONT_WIDTH*zoom);
    }
    str++;
  }
}


