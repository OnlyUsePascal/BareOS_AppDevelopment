#ifndef GAME_FE
#define GAME_FE

#define GAME_W 600
#define GAME_H 600
#define MENU_BACKGND 0xFF000B18
#define MENU_FOREGND 0x00FFFFFF
#define MAZE_SZ 440

typedef struct {
  int posX;
  int posY;
  int width;
  int height;
} PositionDisplay;

void clearScreen();
void drawMenu(int posX, int posY, int yOffset, char *opts[], int optSz);
int getMenuOpt(int markPosX, int markPosY, int yOffset, int optSz);
// void drawAsset(int x, int y, int w, int h, const uint64_t *image);

// ANIMATION, FRAME INTERVAL, 

#endif