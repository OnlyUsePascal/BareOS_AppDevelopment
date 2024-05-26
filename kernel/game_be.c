// === BACK-END ===
#include "../lib/game_be.h"
#include "../lib/game_fe.h"
#include "../lib/framebf.h"
#include "../lib/def.h"
#include "../lib/utils.h"
#include "../lib/font.h"
#include "../lib/data/game/game_menu.h"
#include "../lib/data/game/maze.h"
#include "../lib/data/game/maze_state.h"
#include "../lib/data/game/player.h"
#include "../lib/data/game/item.h"
#include "../lib/data/data_menu_background.h"
#include "../lib/data/help_dialog.h"


// ===== BACK-END =====
const char directionKey[] = {'w', 'a', 's', 'd'};
const int yOffset[] = {-1, 0, 1, 0};
const int xOffset[] = {0, -1, 0, 1};
uint16_t currentRadius = LIGHT_RADIUS;


void render_scene(const Maze *maze, const Asset *asset, const bool isFOVShown);
void debug_pos(Position pos);
void update_pos(Position *des, Direction dir);
void handle_collision(ItemMeta *item, Maze *maze, Player *player);
int cmp_pos(Position pos1, Position pos2);
void debug_item(ItemMeta item);
void game_over(int *_optIdx);

void game_enter() {
    // init
    framebf_init(GAME_W, GAME_H, GAME_W, GAME_H);
    framebf_drawImg(0, 0, MENU_BACKGROUND_SIZE, MENU_BACKGROUND_SIZE, bitmap_menu_background);
    
    // maze1
    Asset playerAsset = {ASSET_HIDDEN, ASSET_HIDDEN, PLAYER_SZ, PLAYER_SZ, bitmap_player};
    Position playerPos = {0, MAZE_SZ_CELL / 2};
    Player player = {&playerAsset, &playerPos};
    
    Asset visionAsset1 = {ASSET_HIDDEN, ASSET_HIDDEN, ITEM_SZ, ITEM_SZ, bitmap_vision};
    Position visionPos1 = {1,9}; 
    ItemMeta visionMeta1 = {&visionAsset1, &visionPos1, VISION, false};
    
    Asset visionAsset12 = {ASSET_HIDDEN, ASSET_HIDDEN, ITEM_SZ, ITEM_SZ, bitmap_vision};
    Position visionPos12 = {5,1}; 
    ItemMeta visionMeta12 = {&visionAsset12, &visionPos12, VISION, false};
    
    Maze mz1 = {.level = 1, .pathColor = -1, .bitmap = bitmap_maze1, .bitmapState = bitmap_mazeState1,
                .itemMetas = {&visionMeta1, &visionMeta12}, .itemMetasSz = 2, 
                .player = &player};

    // maze2
    Asset visionAsset2 = {ASSET_HIDDEN, ASSET_HIDDEN, ITEM_SZ, ITEM_SZ, bitmap_vision};
    Position visionPos2 = {1, 8};
    ItemMeta visionMeta2 = {&visionAsset2, &visionPos2, VISION, false};
    
    Maze mz2 = {.level = 1, .pathColor = -1, .bitmap = bitmap_maze2, .bitmapState = bitmap_mazeState2,
                .itemMetas = {&visionMeta2}, .itemMetasSz = 1, 
                .player = &player};

    // maze 3
    Asset bombAsset3 = {ASSET_HIDDEN, ASSET_HIDDEN, ITEM_SZ, ITEM_SZ, bitmap_bomb};
    Position bombPos3 = {9,1}, bombWall3 = {8,5}; 
    ItemMeta bombMeta3 = {&bombAsset3, &bombPos3, BOMB, false};
    Bomb bomb3 = {&bombMeta3, &bombWall3, false};
    
    // Asset portalAsset3 = {ASSET_HIDDEN, ASSET_HIDDEN, ITEM_SZ, ITEM_SZ, bitmap_portal};
    // Position portalPos3 = {2, 5}, portalDes3 = {4,5};
    // ItemMeta portalMeta3 = {&portalAsset3, &portalPos3, PORTAL, false};
    // Portal portal3 = {&portalMeta3, &portalDes3};
    
    Maze mz3 = {.level = 1, .pathColor = -1, .bitmap = bitmap_maze3, .bitmapState = bitmap_mazeState3,
                .itemMetas = {&bombMeta3}, .itemMetasSz = 1, .bomb = &bomb3,
                .player = &player};
                
    Maze *mazes[] = {&mz1, &mz2, &mz3};
    
    // menu
    int menuPosX = 220, menuPosY = 250, yOffset = 50;
    char *opts[] = {"Start", "Choose Level", "How To Play?", "Exit"};
    int optSz = sizeof(opts) / sizeof(opts[0]);
    
    int optIdx = -1;
    while (1) {
        if (optIdx == -1) {
            drawMenu(menuPosX, menuPosY, yOffset, opts, 
                        optSz, MENU_FOREGND, MENU_BACKGND, true);
            optIdx = getMenuOpt(menuPosX - 50, menuPosY, yOffset, 
                                optSz, MENU_FOREGND, MENU_BACKGND);
            drawMenu(menuPosX, menuPosY, yOffset, opts, 
                        optSz, MENU_FOREGND, MENU_BACKGND, false);
        }

        switch (optIdx) {
            case 0: //start
                game_start(mazes[0], &optIdx);
                uart_puts("> optIdx:"); uart_dec(optIdx); uart_puts("\n");
                break;

            case 1: //continue
                optIdx = -1;
                game_continue();
                break;

            case 2: //help
                optIdx = -1;
                game_help();
                break;

            case 3: //exit
                optIdx = -1;
                game_exit();
                return;
        }

    }
}


int game_menu_enter() {
    framebf_drawImg(115, 130, GAME_MENU_SZ_W, 
                    GAME_MENU_SZ_H, bitmap_game_menu);

    // menu
    int menuPosX = 220, menuPosY = 220, yOffset = 50;
    char *opts[] = {"Continue", "Exit to menu"};
    int optSz = sizeof(opts) / sizeof(opts[0]);

    drawMenu(menuPosX, menuPosY, yOffset, opts, 
                        optSz, MENU_FOREGND, MENU_BACKGND, true);
    return getMenuOpt(menuPosX - 50, menuPosY, yOffset, 
                        optSz, MENU_FOREGND, GAME_MENU_BACKGND);
}


void game_start(Maze *mz, int *_optIdx){
    uart_puts("Starting Game...\n");
    bool isFOVShown = true;
    Player *pl = mz->player;
    *_optIdx = -1;
    
    // reset player
    pl->pos->posX = 0; pl->pos->posY = MAZE_SZ_CELL / 2;
    pl->step = 0;
    
    // reset maze
    curDarken = 1;
    currentRadius = LIGHT_RADIUS;
    getMazePathColor(mz);
    // TODO: reset item status
    for (int i = 0 ; i < mz->itemMetasSz; i++){
        ItemMeta *meta = mz->itemMetas[i];
        meta->collided = false; 
        
        switch (meta->id) {
            case BOMB:
                mz->bomb->used = false;
                // TODO: reset bomb weak wall ?
                // use copy of map to restore broken wall ?
                break;
            default:
                break;
        }
    }

    // init maze front end
    posBeToFe(pl->pos, pl->asset);
    for (int i = 0 ; i < mz->itemMetasSz; i++){
        ItemMeta *meta = mz->itemMetas[i];        
        posBeToFe(meta->pos, meta->asset);
    }
    render_scene(mz, pl->asset, isFOVShown);
    // TODO: paint breakable wall !!!
    
    
    // movement 
    while (1) {
        uart_puts("---\n");
        char c = uart_getc();
        debug_pos(*(pl->pos));
        
        // DEBUG / screen shading
        if(c == 'o'){
            adjustBrightness(mz, pl->asset, true);
        }
        else if(c == 'p'){
            adjustBrightness(mz, pl->asset, false);
        } 
        else if (c == 'k') {
            isFOVShown = !isFOVShown;
            render_scene(mz, pl->asset, isFOVShown);
        }
        else if (c == 27) { // game menu
            int game_stage = game_menu_enter();   
            switch (game_stage) {
                case 0:
                    render_scene(mz, pl->asset, isFOVShown);
                    break;
                case 1:
                    clearScreen();
                    framebf_drawImg(0, 0, MENU_BACKGROUND_SIZE, 
                                    MENU_BACKGROUND_SIZE, bitmap_menu_background);
                    return;
            }
            // str_debug_num(game_stage);
        }
        else if (c == 'q'){
            effect_bomb(mz, pl);
        } 
        else {
            // TODO: spinning function
            Direction dir = -1;
            for (int i = 0; i < 4; i++){
                if (directionKey[i] == c){
                    dir = i; break;
                }
            }
            if (dir == -1) continue; 
            drawFOV(mz, pl->asset);
            drawMovementFrame(pl->asset, dir, 2);
            
            // TODO: update position function
            Position posTmp = {pl->pos->posX, pl->pos->posY};
            update_pos(&posTmp, dir); uart_puts("-> "); debug_pos(posTmp);
            if (posTmp.posX < 0 || posTmp.posY < 0 || 
                posTmp.posX >= MAZE_SZ_CELL || posTmp.posY >= MAZE_SZ_CELL) continue;
                
            int mazeState = mz->bitmapState[MAZE_SZ_CELL * posTmp.posY + posTmp.posX];
            uart_puts("maze state:"); str_debug_num(mazeState);
            if (mazeState == 0) continue;
            
            // post-moving
            update_pos(pl->pos, dir);
            ItemMeta *collidedItem = detect_collision(*(pl->pos), mz->itemMetas, mz->itemMetasSz);
            drawMovement(mz, pl->asset, dir, collidedItem);
            
            // darken screen interval + game over
            pl->step += 1; 
            if (pl->step % 7 == 0) {
                adjustBrightness(mz, pl->asset, true);
                if (curDarken <= LIGHT_THRESHOLD) {
                    game_over(_optIdx);
                    return;
                }
            }
            
            // colision 
            handle_collision(collidedItem, mz, pl);
        }
    } 
}


void game_continue() {
    int menuPosX = 220, menuPosY = 250, yOffset = 50;
    char *opts[] = {"Level 1", "Level 2", "Level 3", "Back to Menu"};
    int optSz = sizeof(opts) / sizeof(opts[0]);
    drawMenu(menuPosX, menuPosY, yOffset, opts, 
                optSz, MENU_FOREGND, MENU_BACKGND, true);

    while (1) {
        int optIdx = getMenuOpt(menuPosX - 50, menuPosY, yOffset, optSz, MENU_FOREGND, MENU_BACKGND);
        font_drawChar(menuPosX - 50, menuPosY + optIdx * yOffset, '>', MENU_BACKGND, 2, 1);

        switch (optIdx) {
            case 0:
                uart_puts("level 1\n");
                break;

            case 1:
                uart_puts("level 2\n");
                break;

            case 2:
                uart_puts("level 3\n");
                break;

            case 3: // back to menu
                drawMenu(menuPosX, menuPosY, yOffset, opts, 
                            optSz, MENU_FOREGND, MENU_BACKGND, false);
                return;
        }
    }
}


void game_help() {
    uart_puts("Game Instruction...\n");

    Asset dialog = {
        GAME_W / 2 - HELP_DIALOG_WIDTH / 2,
        GAME_H / 2 - HELP_DIALOG_HEIGHT / 2,
        HELP_DIALOG_WIDTH,
        HELP_DIALOG_HEIGHT,
        bitmap_help_dialog
    };

    drawAsset(&dialog);

    char c = 0;
    while ((c = uart_getc()) != '\n') {}

    for (int i = GAME_W / 2 - HELP_DIALOG_WIDTH / 2, posX = 0; posX < HELP_DIALOG_WIDTH; i++, posX++) {
        for (int j = GAME_H / 2 - HELP_DIALOG_HEIGHT / 2, posY = 0; posY < HELP_DIALOG_HEIGHT; j++, posY++) {
            framebf_drawPixel(i, j, bitmap_menu_background[i + j * MENU_BACKGROUND_SIZE]);
        }
    }
}


void game_exit() {
    clearScreen();
    uart_puts("Exiting game...\n");
    font_drawString(150, 150, "Ta reng Ta reng Ta reng", MENU_FOREGND, 2, 1);
}


void game_over(int *_optIdx) {
    str_debug("game over :(");
    wait_msec(1000000);
    clearScreen(); 
    
    font_drawString(150, 150, "Game Over :(", MENU_FOREGND, 2, 1);
    int menuPosX = 220, menuPosY = 250, yOffset = 50;
    char *opts[] = {"Start Over", "Rage Quit ?"};
    int optSz = sizeof(opts) / sizeof(opts[0]);

    drawMenu(menuPosX, menuPosY, yOffset, opts, 
                optSz, MENU_FOREGND, MENU_BACKGND, true);
    int optIdx = getMenuOpt(menuPosX - 50, menuPosY, yOffset, 
                            optSz, MENU_FOREGND, MENU_BACKGND);

    clearScreen();  
    switch (optIdx) {
        case 0: {
            *_optIdx = 0;
            return;
        }
        case 1: {
            //quit
            framebf_drawImg(0, 0, MENU_BACKGROUND_SIZE, 
                            MENU_BACKGROUND_SIZE, bitmap_menu_background);
            return;
        }
    }
}












// ============================== GAME ITEM
void effect_vision(Maze *maze, Player *player){
    currentRadius = currentRadius + 20;
    adjustBrightness(maze, player->asset, false);
}


void effect_portal(Maze *mz, Player *pl){
    wait_msec(125000);   
    Position *des = mz->portal->des;

    // erase fov -> change position in FE, BE -> draw again 
    removeFOV(pl->asset);
    pl->pos->posX = des->posX;
    pl->pos->posY = des->posY;
    posBeToFe(pl->pos, pl->asset);
    drawFOV(mz, pl->asset);
    drawAsset(pl->asset);
}


void effect_bomb(Maze *mz, Player *pl){
    Bomb *bo = mz->bomb;
    Position *weakWall = bo->weakWall;
    bool usable = (bo->itemMeta->collided && !bo->used);
    // can use = collided = 1 && used = 0
    // cannot use collied = 0 || used = 1
    
    // check if pl stand near
    int dx = weakWall->posX - pl->pos->posX,
        dy = weakWall->posY - pl->pos->posY;
    str_debug_num(dx); str_debug_num(dy);
    if (dx * dx + dy * dy != 1) return; //should stand adjacent
    if (bo->used) return;
    
    // highlight weak wall
    drawFOVWeakWall(mz, pl->asset, bo->weakWall);
    drawAsset(pl->asset);
    
    // then draw the original wall (or blow it up)
    wait_msec(250000);
    int wallLowerX = weakWall->posX * MAZE_SZ_CELL_PIXEL, 
        wallUpperX = (weakWall->posX + 1) * MAZE_SZ_CELL_PIXEL,
        wallLowerY = weakWall->posY * MAZE_SZ_CELL_PIXEL,
        wallUpperY = (weakWall->posY + 1) * MAZE_SZ_CELL_PIXEL;
    if (usable) {
        for (int x = wallLowerX; x < wallUpperX; x++){
            for (int y = wallLowerY; y < wallUpperY; y++){
                mz->bitmap[y * MAZE_SZ + x] = mz->pathColor;
            }
        }
    }
    drawFOV(mz, pl->asset);
    drawAsset(pl->asset);
    
    // update BE
    if (usable){
        bo->used = true;
        mz->bitmapState[weakWall->posY * MAZE_SZ_CELL + weakWall->posX] = 1;
    }
}












// =============================== GAME FLOW
void render_scene(const Maze *maze, const Asset *asset, const bool isFOVShown) {
    if (!isFOVShown) {
        framebf_drawImg(0,0, MAZE_SZ, MAZE_SZ, bitmap_maze1);
    } else {
        clearScreen();
        drawFOV(maze, asset);
    }

    drawAsset(asset);
}


ItemMeta* detect_collision(Position playerPos, ItemMeta *itemMetas[], int itemMetasSz) {
    for (int i = 0 ; i < itemMetasSz; i++){
        if (cmp_pos(playerPos, *(itemMetas[i]->pos)) 
                && itemMetas[i]->collided == 0) {
            return itemMetas[i];
        }
    }
    return NULL;  
}


void handle_collision(ItemMeta *itemMeta, Maze *maze, Player *player) {
    if (itemMeta == NULL) return;

    debug_item(*itemMeta);
    itemMeta->collided = 1;

    // INSTRUCTION
    static unsigned char seeing_item_first_time = 0;
    static const char *item_names[] = {
        "TRAP",
        "BOMB",
        "VISION",
        "PORTAL"
    };
    static const char *item_descs[] = {
        "TRAP DESCRIPTION",
        "BOMB DESCRIPTION",
        "VISION DESCRIPTION",
        "PORTAL DESCRIPTION",
    };

    if (!(seeing_item_first_time & (1 << itemMeta->id))) {
        seeing_item_first_time |= (1 << itemMeta->id);

        drawDialog(item_names[itemMeta->id], item_descs[itemMeta->id]);
        while (true) {
            char c = uart_getc();
            if (c == '\n') {
                // removeDialog(itemMeta->pos);
                render_scene(maze, player->asset, true);
                break;
            }
        }
    }
    
    // TODO: EFFECT
    switch (itemMeta->id) {
        case VISION:
            effect_vision(maze, player);
            break;
        case PORTAL:
            effect_portal(maze, player);
            break;
        case BOMB:{
            // effect_bomb(maze, player);
            break;
        }
    }
}


void update_pos(Position *des, Direction dir) {
    des->posX += xOffset[dir];
    des->posY += yOffset[dir];
}


void debugPos(Position pos) {
    uart_puts("[");
    uart_dec(pos.posX);
    uart_puts(",");
    uart_dec(pos.posY);
    uart_puts("]\n");
}


int cmp_pos(Position pos1, Position pos2) {
    return (pos1.posX == pos2.posX && pos1.posY == pos2.posY);
}


void debug_pos(Position pos){
    uart_puts("["); uart_dec(pos.posX); uart_puts(","); uart_dec(pos.posY); uart_puts("]\n");
}


void debug_item(ItemMeta itemMeta) {
    uart_puts("[ItemMeta:");
    switch (itemMeta.id) {
        case BOMB:
            uart_puts("Bomb");
            break;
        case VISION:
            uart_puts("Vision");
            break;
        case PORTAL:
            uart_puts("Portal");
            break;
        default:
            break;
    }
    uart_puts("]\n");
}







