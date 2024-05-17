#ifndef GAME_BE_H
#define GAME_BE_H

#define MAZE_SZ_CELL 11 

typedef struct {
  int posX; // x in maze
  int posY; // y in maze
  int width; 
  int height; 
  unsigned long *bitmap;
} Asset; 


typedef struct {
  int posX;
  int posY;
  
} Position;


typedef enum {
  TRAP,
  BOMB,
  VISION,
  COIN
} ItemId;


typedef struct {
  Asset *asset;
  Position *pos;
  ItemId id;
  int collided;
} Item;


typedef struct {
  int radius;
  int level; 
  int fadingSpeed; // ???
} Light;


typedef struct {
  Position pos;
  Light light;
  int speed; // ???
} Player;


typedef enum {
  UP,
  LEFT,
  DOWN,
  RIGHT
} Direction;


typedef struct {
  int level;
  // int pathColor;
  unsigned long *bitmap;
  Item *items[10];
  int itemsSz;
} Maze;

extern const int xOffset[];
extern const int yOffset[];
extern const char directionKey[];

void game_enter();
void game_start();
void game_continue();
void game_help();
void game_exit();
void clearScreen();
Item* detect_collision(Position playerPos, Item *items[], int itemsSz);



#endif