#ifndef FONT_H
#define FONT_H

void font_drawChar(int x, int y, char ch, unsigned int attr, int zoom);
void font_drawString(int x, int y, char *str, unsigned int attr, int zoom);
void font2_drawChar(int x, int y, char ch, unsigned int attr, int zoom);
void font2_drawString(int x, int y, char *str, unsigned int attr, int zoom);


#endif