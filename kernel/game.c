#include "../lib/game.h"
#include "../lib/font.h"

void game_enter(){
  // init
  framebf_init(GAME_W, GAME_H, GAME_W, GAME_H);
  
  
  // background + headline 
  
  // menu
  int posX = 150;
  int posY = 200;
  int offsetY = 50;
  char *actions[] = {"Start", "Continue", "How To Play?", "Exit"};
  int actionsSz = 4;
  for (int i = 0; i < 4; i++){
    font_drawString(posX, posY + i * offsetY, actions[i], GAME_MENU_FOREGND, 2);
  }
  
  // menu option choose (start, continue, help, exit)
  int actionIdx = 0; 
  int ptrPosX = posX - 50;
  font_drawChar(ptrPosX, posY, '>', GAME_MENU_FOREGND, 2);

  while (1){ 
    char c = uart_getc();
    uart_sendc(c); uart_sendc('\n');
    if (c == 'w' || c == 'a') {
      font_drawChar(ptrPosX, posY + actionIdx * offsetY, '>', GAME_MENU_BACKGND, 2);
      actionIdx = (actionIdx - 1) % actionsSz;
      if (actionIdx < 0) actionIdx += actionsSz;
      font_drawChar(ptrPosX, posY + actionIdx * offsetY, '>', GAME_MENU_FOREGND, 2);
    } else if (c == 's' || c == 'd') {
      font_drawChar(ptrPosX, posY + actionIdx * offsetY, '>', GAME_MENU_BACKGND, 2);
      actionIdx = (actionIdx + 1) % actionsSz;
      font_drawChar(ptrPosX, posY + actionIdx * offsetY, '>', GAME_MENU_FOREGND, 2);
    } else if (c == '\n') {
      break;
    }
  }
  
  switch (actionIdx) {
    case 0: //start
      game_start();
      break;
    
    case 1: //continue
      game_continue();
      break;
    
    case 2: //help
      game_help();
      break;
    
    case 3: //exit
      game_exit();
      return;
  }
}


void game_start(){
  
}


void game_continue(){
  
}


void game_help(){
  
}


void game_exit(){
  framebf_drawRect(0, 0, GAME_W, GAME_H, GAME_MENU_BACKGND, 1);  
  uart_puts("Exiting game...\n");
  font_drawString(200, 200, "Banh truong linh vuc\n", GAME_MENU_FOREGND, 2);
}


void game_clr(){
  framebf_drawRect(0, 0, GAME_W, GAME_H, GAME_MENU_BACKGND, 1);  
}