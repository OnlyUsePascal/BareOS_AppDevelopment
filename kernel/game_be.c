// === BACK-END ===
#include "../lib/game_be.h"
#include "../lib/game_fe.h"
#include "../lib/framebf.h"
#include "../lib/font.h"
#include "../lib/def.h"
#include "../lib/data/game/maze.h"
#include "../lib/data/game/player.h"

// ===== BACK-END =====

void game_enter(){
  // init
  framebf_init(GAME_W, GAME_H, GAME_W, GAME_H);

  // background + headline 
  clearScreen();
  
  // menu
  int menuPosX = 150, menuPosY = 200, yOffset = 50;
  char *opts[] = {"Start", "Continue", "How To Play?", "Exit"};
  int optSz = sizeof(opts) / sizeof(opts[0]);
  drawMenu(menuPosX, menuPosY, yOffset, opts, optSz);

  int optIdx = getMenuOpt(menuPosX - 50, menuPosY, yOffset, optSz);  
  switch (optIdx) {
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
  // MAP AREA = (EDGE SZ * 2 + 1)^2
  uart_puts("Starting Game...\n");
  clearScreen();
  framebf_drawImg(0,0, MAZE_SZ, MAZE_SZ, bitmap_maze);
  int characterSz = 20;
  drawAsset(0 + (MAZE_SZ_CELL_PIXEL - characterSz) / 2, 
            MAZE_SZ_CELL_PIXEL * (MAZE_SZ_CELL / 2) + (MAZE_SZ_CELL_PIXEL - characterSz) / 2, 
            characterSz, characterSz, bitmap_player); 
}


void game_continue(){
  uart_puts("Continueing Game...\n");
  
}


void game_help(){
  uart_puts("Game Instruction...\n");
  
  
}


void game_exit(){
  clearScreen();
  uart_puts("Exiting game...\n");
  font_drawString(150, 150, "Ta reng Ta reng Ta reng", MENU_FOREGND, 2);
}

// ===== MAIN AREA =====
