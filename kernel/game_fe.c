// === FRONT-END ===
#include "../lib/game_fe.h"
#include "../lib/utils.h"
#include "../lib/def.h"
#include "../lib/framebf.h"
#include "../lib/data/game/maze.h"

float cur_darken = 1.0f; 
float darken_factor = 0.8f; 
float cur_lighten = 1.0f; 
float lighten_factor = 1.25f;


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
  font_drawChar(markPosX, markPosY, '>', MENU_FOREGND, 2, 1);

  while (1){ 
    char c = uart_getc();
    uart_sendc(c); uart_sendc('\n');
    if (c == 'w' || c == 'a') {
      font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', MENU_BACKGND, 2, 1);
      actionIdx = (actionIdx - 1) % optSz;
      if (actionIdx < 0) actionIdx += optSz;
      font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', MENU_FOREGND, 2, 1);
    } else if (c == 's' || c == 'd') {
      font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', MENU_BACKGND, 2, 1);
      actionIdx = (actionIdx + 1) % optSz;
      font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', MENU_FOREGND, 2, 1);
    } else if (c == '\n') {
      break;
    }
  }
  
  return actionIdx;
}


void drawAsset(Asset *asset) {
  for (int i = asset->posX, posX = 0; posX < asset->width; i++, posX++){
    for (int j = asset->posY, posY = 0; posY < asset->height; j++, posY++){
      if (asset->bitmap[posX + posY * asset->width] != 0)
        framebf_drawPixel(i, j, asset->bitmap[posX + posY * asset->width]);
    }
  }
}


void removeAsset(Asset *asset) {
  //replace with maze pixel
  // TODO: move to storage files
  Asset maze = {0,0, MAZE_SZ_CELL * MAZE_SZ_CELL_PIXEL, 
                  MAZE_SZ_CELL * MAZE_SZ_CELL_PIXEL, bitmap_maze};
                  
  for (int i = asset->posX, posX = 0; posX < asset->width; i++, posX++){
    for (int j = asset->posY, posY = 0; posY < asset->height; j++, posY++){
      framebf_drawPixel(i, j, maze.bitmap[i + j * maze.width]);
    } 
  }  
}


void moreScreenDarkness() {
  cur_darken *= darken_factor;
  for (int i = 0; i < MAZE_SZ; i++){
    for (int j = 0; j < MAZE_SZ; j++){

      unsigned long color = bitmap_maze[i + j * MAZE_SZ];
      // uart_hex(color); uart_puts("\n");
      unsigned long r = (color & 0xFF0000) >> 16;
      unsigned long g = (color & 0x00FF00) >> 8;
      unsigned long b = (color & 0x0000FF);

      // Darken the RGB values       
      r = (unsigned long)(r * cur_darken);
      g = (unsigned long)(g * cur_darken);
      b = (unsigned long)(b * cur_darken);

      // Ensure the values are within the valid range
      if (r < 0) r = 0;
      if (g < 0) g = 0;
      if (b < 0) b = 0;

      // Combine the RGB values back into a single color value
      unsigned long new_color = (r << 16) | (g << 8) | b;
      // uart_dec(new_color); uart_puts("\n");
      framebf_drawPixel(i, j, new_color);
    }
  }
}


void resetScreenDarkness() {
  for (int i = 0; i < MAZE_SZ; i++){
    for (int j = 0; j < MAZE_SZ; j++){

      unsigned long color = bitmap_maze[i + j * MAZE_SZ];
      // uart_hex(color); uart_puts("\n");
      unsigned long r = (color & 0xFF0000) >> 16;
      unsigned long g = (color & 0x00FF00) >> 8;
      unsigned long b = (color & 0x0000FF);

      // Darken the RGB values       
      r = (unsigned long)(r * cur_lighten);
      g = (unsigned long)(g * cur_lighten);
      b = (unsigned long)(b * cur_lighten);

      // Ensure the values are within the valid range
      if (r < 0) r = 0;
      if (g < 0) g = 0;
      if (b < 0) b = 0;

      // Combine the RGB values back into a single color value
      unsigned long new_color = (r << 16) | (g << 8) | b;
      // uart_dec(new_color); uart_puts("\n");
      framebf_drawPixel(i, j, new_color);
    }
  }
  // clearScreen();
  // framebf_drawImg(0,0, MAZE_SZ, MAZE_SZ, bitmap_maze);
  // drawAsset(playerAsset);
  // cur_darken = 0.8f;
}


void drawMovement(Asset *player, Direction dir, Item *collidedItem){
  // TODO: animation with frame
  int step = 3;
  int stepOffset = MAZE_SZ_CELL_PIXEL / step;
  int posXFinal = player->posX + xOffset[dir] * MAZE_SZ_CELL_PIXEL;
  int posYFinal = player->posY + yOffset[dir] * MAZE_SZ_CELL_PIXEL;
  
  // walk the middle
  for (int i = 0 ; i < step - 1; i++){
    removeAsset(player);
    updateAssetPos(player, player->posX + xOffset[dir] * stepOffset, 
                          player->posY + yOffset[dir] * stepOffset);
    drawAsset(player);
    // wait_msec(37250);
    wait_msec(62500);
  }
  
  // walk the last step
  removeAsset(player);
  if (collidedItem != NULL) {
    removeAsset(collidedItem->asset); 
  }
  updateAssetPos(player, posXFinal, posYFinal);
  drawAsset(player);
}
  

void updateAssetPos(Asset *asset, int x, int y) {
  asset->posX = x;
  asset->posY = y;
}


void posBeToFe(Position *pos, Asset *asset){
  asset->posX = pos->posX * MAZE_SZ_CELL_PIXEL + (MAZE_SZ_CELL_PIXEL - asset->width) / 2;
  asset->posY = pos->posY * MAZE_SZ_CELL_PIXEL + (MAZE_SZ_CELL_PIXEL - asset->height) / 2;
}


void debugAsset(Asset asset){
  uart_puts("["); uart_dec(asset.posX); uart_puts(","); uart_dec(asset.posY); uart_puts("]\n");
}

