// === FRONT-END ===
#include "../lib/game_fe.h"
#include "../lib/game_be.h"
#include "../lib/utils.h"
#include "../lib/def.h"

void clearScreen(){
  framebf_drawRect(0, 0, GAME_W, GAME_H, MENU_BACKGND, 1);
}



void drawMenu(int posX, int posY, int spacing, char *opts[], int optSz){
  for (int i = 0; i < optSz; i++){
    font_drawString(posX, posY + i * spacing, opts[i], MENU_FOREGND, 2);
  }
}



int getMenuOpt(int markPosX, int markPosY, int yOffset, int optSz) {
  int actionIdx = 0; 
  font_drawChar(markPosX, markPosY, '>', MENU_FOREGND, 2);

  while (1){ 
    char c = uart_getc();
    uart_sendc(c); uart_sendc('\n');
    if (c == 'w' || c == 'a') {
      font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', MENU_BACKGND, 2);
      actionIdx = (actionIdx - 1) % optSz;
      if (actionIdx < 0) actionIdx += optSz;
      font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', MENU_FOREGND, 2);
    } else if (c == 's' || c == 'd') {
      font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', MENU_BACKGND, 2);
      actionIdx = (actionIdx + 1) % optSz;
      font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', MENU_FOREGND, 2);
    } else if (c == '\n') {
      break;
    }
  }
  
  return actionIdx;
}



void drawAsset(int x, int y, int w, int h, const uint64_t *image){
  str_debug_num(x);
  str_debug_num(y);
 
  for (int i = x, posX = 0; posX < w; i++, posX++) {
    for (int j = y, posY = 0; posY < h; j++, posY++) {
      if (image[posX + posY * w] != 0) 
        framebf_drawPixel(i, j, image[posX + posY * w]); 
    }
  }
}














