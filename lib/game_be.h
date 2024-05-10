
#define MAZE_SZ_CELL 11 
#define MAZE_SZ_CELL_PIXEL 40 

typedef struct {
  int level;
  int pathColor;
  
} Maze;

typedef struct {
  int posX;
  int posY;
  
} Position;

typedef enum {
  TRAP,
  BOMB,
  LIGHTER,
  SPEEDUP,
  COIN
} ItemId;

typedef struct {
  Position pos;
  ItemId id;
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
};


void game_enter();
void game_start();
void game_continue();
void game_help();
void game_exit();
void clearScreen();



