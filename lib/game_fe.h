#include "../lib/game_be.h"

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

#define MAZE_SZ_CELL_PIXEL 40 
#define PLAYER_SZ 30
#define ITEM_SZ 20


void clearScreen();
void drawMenu(int posX, int posY, int yOffset, char *opts[], int optSz);
int getMenuOpt(int markPosX, int markPosY, int yOffset, int optSz);

void drawFOVMovement(Position initialPlayerPosition, Direction dir);
void removeFOV(const Asset *asset);
void drawFOV(const Maze *maze, const Asset *asset);

void drawMovement(Maze *maze, Asset *asset, Direction dir, Item *collidedItem);
void removeAsset(const Asset *asset);
void drawAsset(const Asset *asset);
void embedAsset(const Maze *maze, const Asset *asset, bool fill);
void updateAssetPos(Asset *asset, int x, int y);
void drawMoveAnimation(Asset *playerAsset, Direction dir, int order);
void posBeToFe(Position *pos, Asset *asset);
void debugAsset(Asset asset);
void getMazePathColor(Maze *maze);

void adjustBrightness(const Maze *maze, const Asset *asset, bool darken);
uint64_t darkenPixel(uint64_t color, const float factor);

extern float cur_darken;
extern const float darken_factor;
extern float cur_lighten;
extern const float lighten_factor;

#endif