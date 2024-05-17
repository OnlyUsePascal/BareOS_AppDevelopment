#ifndef GAME_FE
#define GAME_FE

#include "gpio.h"
#include "game_be.h"

#define GAME_W 600
#define GAME_H 600
#define MENU_BACKGND 0xFF000B18
#define MENU_FOREGND 0x00FFFFFF
#define MAZE_SZ 440
#define ASSET_HIDDEN -1

// typedef struct {
//   int posX;
//   int posY;
// } PositionDisplay;

typedef struct {
  int posX; // x in maze
  int posY; // y in maze
  int width; 
  int height; 
  unsigned long *bitmap;
} Asset; 

void clearScreen();
void drawMenu(int posX, int posY, int yOffset, char *opts[], int optSz);
int getMenuOpt(int markPosX, int markPosY, int yOffset, int optSz);
void updateAssetPos(Asset *asset, int x, int y);
void drawMovement(Asset *asset, Direction dir);
void drawFOVMovement(Position initialPlayerPosition, Direction dir);
void removeAsset(const Asset *asset);
void drawAsset(const Asset *asset);
void removeFOV(const Position playerPos);
void drawFOV(const Position playerPos);

// ANIMATION, FRAME INTERVAL, 

#endif