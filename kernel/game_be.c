// === BACK-END ===
#include "../lib/game_be.h"
#include "../lib/game_fe.h"
#include "../lib/framebf.h"
#include "../lib/def.h"
#include "../lib/utils.h"
// #include "../lib/util_str.h"
#include "../lib/data/game/game_menu.h"
#include "../lib/data/game/maze.h"
#include "../lib/data/game/maze_state.h"
#include "../lib/data/game/player.h"
#include "../lib/data/game/item.h"
#include "../lib/data/data_menu_background.h"


// ===== BACK-END =====
const char directionKey[] = {'w', 'a', 's', 'd'};
const int yOffset[] = {-1, 0, 1, 0};
const int xOffset[] = {0, -1, 0, 1};
uint16_t currentRadius = 40;



void game_enter() {
    // init
    framebf_init(GAME_W, GAME_H, GAME_W, GAME_H);
    framebf_drawImg(0, 0, MENU_BACKGROUND_SIZE, MENU_BACKGROUND_SIZE, bitmap_menu_background);
    curDarken = 1.0f;
    currentRadius = 40;
    
    // maze1
    Asset playerAsset = {ASSET_HIDDEN, ASSET_HIDDEN, PLAYER_SZ, PLAYER_SZ, bitmap_player};
    Position playerPos = {0, MAZE_SZ_CELL / 2};
    Player player = {&playerAsset, &playerPos};
    
    Asset bombAsset = {ASSET_HIDDEN, ASSET_HIDDEN, ITEM_SZ, ITEM_SZ, bitmap_bomb};
    Position bombPos = {1, 9}, bombWall = {2,8};
    ItemMeta bombMeta = {&bombAsset, &bombPos, BOMB, 0};
    Bomb bomb = {&bombMeta, &bombWall, false};
    
    Asset visionAsset = {ASSET_HIDDEN, ASSET_HIDDEN, ITEM_SZ, ITEM_SZ, bitmap_vision};
    Position visionPos = {7, 5};
    ItemMeta visionMeta = {&visionAsset, &visionPos, VISION, 0};
    
    Asset portalAsset = {ASSET_HIDDEN, ASSET_HIDDEN, ITEM_SZ, ITEM_SZ, bitmap_portal};
    Position portalPos = {2, 5}, portalDes = {4,5};
    ItemMeta portalMeta = {&portalAsset, &portalPos, PORTAL, 0};
    Portal portal = {&portalMeta, &portalDes};
    
    Maze mz1 = {.level = 1, .pathColor = -1, .bitmap = bitmap_maze1, .bitmapState = bitmap_mazeState1,
                .itemMetas = {&bombMeta, &visionMeta, &portalMeta}, .itemMetasSz = 3, 
                .portal = &portal, .bomb = &bomb,
                .player = &player};
    
    // maze2
    Maze mz2 = {};
    Maze mz3 = {};
    
    Maze *mazes[] = {&mz1, &mz2, &mz3};
    
    
    
    
    
    
    
    // menu
    int menuPosX = 220, menuPosY = 250, yOffset = 50;
    char *opts[] = {"Start", "Continue", "How To Play?", "Exit"};
    int optSz = sizeof(opts) / sizeof(opts[0]);

    while (1) {
        drawMenu(menuPosX, menuPosY, yOffset, opts, optSz);
        int optIdx = getMenuOpt(menuPosX - 50, menuPosY, yOffset, 
                                optSz, MENU_FOREGND, MENU_BACKGND);

        switch (optIdx) {
            case 0: //start
                game_start(mazes[0]);
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
}


int game_menu_enter() {
    // init
    framebf_init(GAME_W, GAME_H, GAME_W, GAME_H);
    framebf_drawImg(115, 130, GAME_MENU_SZ_W, 
                    GAME_MENU_SZ_H, bitmap_game_menu);

    // menu
    int menuPosX = 220, menuPosY = 220, yOffset = 50;
    char *opts[] = {"Continue", "Exit to menu"};
    int optSz = sizeof(opts) / sizeof(opts[0]);

    while (1) {
        drawMenu(menuPosX, menuPosY, yOffset, opts, optSz);
        int optIdx = getMenuOpt(menuPosX - 50, menuPosY, yOffset, 
                                optSz, MENU_FOREGND, GAME_MENU_BACKGND);

        switch (optIdx) {
            case 0: //start
                return 0;
                break;

            case 1: //exit to menu
                return 1;
                break;
        }
    }
}


void game_start(Maze *mz){
    uart_puts("Starting Game...\n");
    bool isFOVShown = true;
    Player *pl = mz->player;
    getMazePathColor(mz);
    
    posBeToFe(pl->pos, pl->asset);
    for (int i = 0 ; i < mz->itemMetasSz; i++){
        ItemMeta *itemMeta = mz->itemMetas[i];        
        posBeToFe(itemMeta->pos, itemMeta->asset);
        if(!(itemMeta->collided)) embedAsset(mz, itemMeta->asset, true);
    }
    
    render_scene(mz, pl->asset, isFOVShown);
    // TODO: paint breakable wall
    
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
            Direction dir = -1;
            for (int i = 0; i < 4; i++){
                if (directionKey[i] == c){
                    dir = i; break;
                }
            }
        
            if (dir == -1) continue; 
            Position posTmp = {pl->pos->posX, pl->pos->posY};
            update_pos(&posTmp, dir);
            uart_puts("> "); debug_pos(posTmp);
            
            if (posTmp.posX < 0 || posTmp.posY < 0) continue;
            //TODO: get map state base on its level
            int mazeState = mz->bitmapState[MAZE_SZ_CELL * posTmp.posY + posTmp.posX];
            str_debug_num(mazeState);
            if (mazeState == 0) {
                str_debug("hit wall!"); continue;
            } 
            
            update_pos(pl->pos, dir);
            ItemMeta *collidedItem = detect_collision(*(pl->pos), mz->itemMetas, mz->itemMetasSz);
            drawMovement(mz, pl->asset, dir, collidedItem);
            handle_collision(collidedItem, mz, pl);
        }
    } 
}


void game_continue() {
    uart_puts("Continueing Game...\n");
}


void game_help() {
    uart_puts("Game Instruction...\n");
}


void game_exit() {
    clearScreen();
    uart_puts("Exiting game...\n");
    font_drawString(150, 150, "Ta reng Ta reng Ta reng", MENU_FOREGND, 2, 1);
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
    Position *wall = bo->wall;
    bool usable = (bo->itemMeta->collided && !bo->used);
    // can use = collided = 1 && used = 0
    // cannot use collied = 0 || used = 1
    // brighten the wall, 
    
    if (bo->used) return;
    
    // modified FOV
    int lowerX = wall->posX * MAZE_SZ_CELL_PIXEL, 
        upperX = (wall->posX + 1) * MAZE_SZ_CELL_PIXEL,
        lowerY = wall->posY * MAZE_SZ_CELL_PIXEL,
        upperY = (wall->posY + 1) * MAZE_SZ_CELL_PIXEL;
    int lx = pl->asset->posX - currentRadius + pl->asset->height/2,
        ux = pl->asset->posX + currentRadius + pl->asset->height/2,
        ly = pl->asset->posY - currentRadius + pl->asset->height/2,
        uy = pl->asset->posY + currentRadius + pl->asset->height/2;
    
    // uart_puts("["); uart_dec(lowerX); uart_puts(","); uart_dec(upperX); uart_puts(","); uart_dec(lowerY); uart_puts(","); uart_dec(upperY); uart_puts(","); uart_puts("]\n");
    // uart_puts("["); uart_dec(lx); uart_puts(","); uart_dec(ux); uart_puts(","); uart_dec(ly); uart_puts(","); uart_dec(uy); uart_puts(","); uart_puts("]\n");
    
    for (int x = lx; x < ux; x++) {
        for (int y = ly; y < uy; y++) {
            if (x >= 0 && y >= 0 && x < MAZE_SZ && y < MAZE_SZ) {
                int dx = x - pl->asset->posX - pl->asset->height/2, 
                    dy = y - pl->asset->posY - pl->asset->height/2;
                if (dx * dx + dy * dy <= currentRadius * currentRadius) {
                    float darkenLevel = (x >= lowerX && x < upperX 
                                        && y >= lowerY && y < upperY) 
                                        ? (curDarken / darkenFactor) : curDarken;
                    framebf_drawPixel(x, y, darkenPixel(mz->bitmap[y * MAZE_SZ + x], darkenLevel));
                }
            }
        }
    }
    drawAsset(pl->asset);
    
    // then draw the original wall (or blow it up if not used)
    wait_msec(250000);
    if (usable) {
        for (int x = lowerX; x < upperX; x++){
            for (int y = lowerY; y < upperY; y++){
                mz->bitmap[y * MAZE_SZ + x] = mz->pathColor;
            }
        }
    }
    drawFOV(mz, pl->asset);
    drawAsset(pl->asset);
    
    // TODO: check if pl stand near
    // update BE
    if (usable){
        bo->used = true;
        mz->bitmapState[wall->posY * MAZE_SZ_CELL + wall->posX] = 1;
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







