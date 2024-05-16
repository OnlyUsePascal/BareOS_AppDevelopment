#ifndef FONT_H
#define FONT_H

void get_font_type(int type);
void font_drawChar(int x, int y, char ch, unsigned int attr, int zoom, int type);
void font_drawString(int x, int y, char *str, unsigned int attr, int zoom, int type);

extern unsigned char (*font_)[];
extern unsigned int font_numglyphs;
extern unsigned int font_bpg;
extern unsigned int font_bpl;
extern unsigned int font_width;
extern unsigned int font_height;


#endif