#include "../lib/game_be.h"

#ifndef GAME_FE
#define GAME_FE

#define GAME_W 600
#define GAME_H 600
#define MENU_BACKGND 0xFF000B18
#define MENU_FOREGND 0x00FFFFFF
#define MAZE_SZ 440
#define ASSET_HIDDEN -1

#define MAZE_SZ_CELL_PIXEL 40 
#define PLAYER_SZ 20
#define ITEM_SZ 20


void clearScreen();
void drawMenu(int posX, int posY, int yOffset, char *opts[], int optSz);
int getMenuOpt(int markPosX, int markPosY, int yOffset, int optSz);

void drawAsset(Asset *asset);
void removeAsset(Asset *asset);
void drawMovement(Asset *asset, Direction dir, Item *collidedItem);
void updateAssetPos(Asset *asset, int x, int y);

void posBeToFe(Position *pos, Asset *asset);
void debugAsset(Asset asset);

void resetScreenDarkness();
void moreScreenDarkness();

extern float cur_darken;
extern float darken_factor;
extern float cur_lighten;
extern float lighten_factor;

#endif