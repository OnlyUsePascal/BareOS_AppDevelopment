#include "../lib/font.h"
#include "../lib/utils.h"
#include "../lib/framebf.h"
#include "../lib/data/data_font.h"
#include "../lib/data/data_font2.h"
#include "../lib/utils.h"

extern unsigned char (*font_)[8] = font;
extern unsigned int font_numglyphs = FONT_NUMGLYPHS;
extern unsigned int font_bpg = FONT_BPG;
extern unsigned int font_bpl = FONT_BPL;
extern unsigned int font_width = FONT_WIDTH;
extern unsigned int font_height = FONT_HEIGHT;

void get_font_type(int type) {
  switch (type) {
    case 1:
      font_ = font;
      font_numglyphs = FONT_NUMGLYPHS;
      font_bpg = FONT_BPG;
      font_bpl = FONT_BPL;
      font_width = FONT_WIDTH;
      font_height = FONT_HEIGHT;
      break;
    case 2:
      font_ = font2;
      font_numglyphs = FONT2_NUMGLYPHS;
      font_bpg = FONT2_BPG;
      font_bpl = FONT2_BPL;
      font_width = FONT2_WIDTH;
      font_height = FONT2_HEIGHT;
      break;
    default:
      font_ = font;
      font_numglyphs = FONT_NUMGLYPHS;
      font_bpg = FONT_BPG;
      font_bpl = FONT_BPL;
      font_width = FONT_WIDTH;
      font_height = FONT_HEIGHT;
      break;
  }
}

/* Functions to display text on the screen */
void font_drawChar(int x, int y, char ch, unsigned int attr, int zoom, int type) {
  get_font_type(type);
  unsigned char *glyph = (unsigned char *)font_ + (ch < font_numglyphs ? ch : 0) * font_bpg;
  for (int i = 1; i <= (font_height*zoom); i++) {
    for (int j = 0; j< (font_width*zoom); j++) {
      unsigned char mask = 1 << (j/zoom);
      if (*glyph & mask) { //only draw pixels belong to the character glyph
        framebf_drawPixel(x + j, y + i, attr);
      }
    }
    glyph += (i % zoom) ? 0 : font_bpl;
  }
}


void font_drawString(int x, int y, char *str, unsigned int attr, int zoom, int type) {
  get_font_type(type);
  while (*str) {
    if (*str == '\r') {
      x = 0;
    } else if (*str == '\n') {
      x = 0; 
      y += (font_height*zoom);
    } else {
      font_drawChar(x, y, *str, attr, zoom, type);
      x += (font_width*zoom);
    }
    str++;
  }
}




