// === BACK-END ===
#include "../lib/game_be.h"
#include "../lib/game_fe.h"
#include "../lib/framebf.h"
#include "../lib/def.h"
#include "../lib/util_str.h"
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

    // menu
    int menuPosX = 220, menuPosY = 250, yOffset = 50;
    char *opts[] = {"Start", "Continue", "How To Play?", "Exit"};
    int optSz = sizeof(opts) / sizeof(opts[0]);

    while (1) {
        drawMenu(menuPosX, menuPosY, yOffset, opts, optSz);
        int optIdx = getMenuOpt(menuPosX - 50, menuPosY, yOffset, optSz, MENU_FOREGND, MENU_BACKGND);

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
}


int game_menu_enter() {
    // init
    framebf_init(GAME_W, GAME_H, GAME_W, GAME_H);
    framebf_drawImg(115, 130, GAME_MENU_SZ_W, GAME_MENU_SZ_H, bitmap_game_menu);

    // menu
    int menuPosX = 220, menuPosY = 220, yOffset = 50;
    char *opts[] = {"Continue", "Exit to menu"};
    int optSz = sizeof(opts) / sizeof(opts[0]);

    while (1) {
        drawMenu(menuPosX, menuPosY, yOffset, opts, optSz);
        int optIdx = getMenuOpt(menuPosX - 50, menuPosY, yOffset, optSz, MENU_FOREGND, GAME_MENU_BACKGND);

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


void game_start(){
    uart_puts("Starting Game...\n");
    bool isFOVShown = true;
    Asset playerAsset = {ASSET_HIDDEN, ASSET_HIDDEN, PLAYER_SZ, PLAYER_SZ, bitmap_player};
    Position playerPos = {0, MAZE_SZ_CELL / 2};
    Player player = {&playerAsset, &playerPos};

    Asset bombAsset = {ASSET_HIDDEN, ASSET_HIDDEN, ITEM_SZ, ITEM_SZ, bitmap_bomb};
    Position bombPos = {1, 9};
    Item bomb = {&bombAsset, &bombPos, BOMB, 0};
    
    Asset visionAsset = {ASSET_HIDDEN, ASSET_HIDDEN, ITEM_SZ, ITEM_SZ, bitmap_vision};
    Position visionPos = {2, 5};
    Item vision = {&visionAsset, &visionPos, VISION, 0};
    
    Asset portalAsset = {ASSET_HIDDEN, ASSET_HIDDEN, ITEM_SZ, ITEM_SZ, bitmap_portal};
    Position portalPos = {7, 5};
    Item portal = {&portalAsset, &portalPos, PORTAL, 0};
    
    Maze maze1 = {1, -1, bitmap_maze, {&bomb, &vision, &portal}, 3};
    getMazePathColor(&maze1);
    
    posBeToFe(player.pos, player.asset);
    
    // TODO: init loop for every maze item
    posBeToFe(&bombPos, &bombAsset);
    posBeToFe(&visionPos, &visionAsset);
    posBeToFe(&portalPos, &portalAsset);
    
    embedAsset(&maze1, bomb.asset, true);
    embedAsset(&maze1, vision.asset, true);
    embedAsset(&maze1, portal.asset, true);
    
    render_scene(&maze1, player.asset, isFOVShown);
        
    // movement 
    while (1) {
        uart_puts("---\n");
        char c = uart_getc();
        debug_pos(*player.pos);
        
        // DEBUG / screen shading
        if(c == 'o'){
            adjustBrightness(&maze1, player.asset, true);
        }
        else if(c == 'p'){
            adjustBrightness(&maze1, player.asset, false);
        } 
        else if (c == 27) { // game menu
            int game_stage = game_menu_enter();   
            switch (game_stage) {
            case 0:
                render_scene(&maze1, player.asset, isFOVShown);
                break;
            case 1:
                clearScreen();
                framebf_drawImg(0, 0, MENU_BACKGROUND_SIZE, MENU_BACKGROUND_SIZE, bitmap_menu_background);
                return;
            }
            // str_debug_num(game_stage);
        } 
        else if (c == 'k') {
            isFOVShown = !isFOVShown;
            render_scene(&maze1, player.asset, isFOVShown);
        }
        else {
            //TODO: movement
            Direction dir = -1;
            for (int i = 0; i < 4; i++){
                if (directionKey[i] == c){
                    dir = i; break;
                }
            }
        
            if (dir == -1) continue; 
            Position posTmp = {player.pos->posX, player.pos->posY};
            update_pos(&posTmp, dir);
            uart_puts("> "); debug_pos(posTmp);
            
            if (posTmp.posX < 0 || posTmp.posY < 0) continue;
            //TODO: get map state base on its level
            int mazeState = bitmap_mazeState1[MAZE_SZ_CELL * posTmp.posY + posTmp.posX];
            str_debug_num(mazeState);
            if (mazeState == 0) {
                str_debug("hit wall!"); continue;
            } 
            
            update_pos(player.pos, dir);
            Item *collidedItem = detect_collision(*player.pos, maze1.items, maze1.itemsSz);
            drawMovement(&maze1, player.asset, dir, collidedItem);
            handle_collision(collidedItem, &maze1, &player);
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
    str_debug("item vision");
    currentRadius = currentRadius + 20;
    adjustBrightness(maze, player->asset, false);
}















// =============================== GAME FLOW
void render_scene(const Maze *maze, const Asset *asset, const bool isFOVShown) {
    if (!isFOVShown) {
        framebf_drawImg(0,0, MAZE_SZ, MAZE_SZ, bitmap_maze);
    } else {
        clearScreen();
        drawFOV(maze, asset);
    }

    drawAsset(asset);
}


Item* detect_collision(Position playerPos, Item *items[], int itemsSz) {
    for (int i = 0 ; i < itemsSz; i++){
        if (cmp_pos(playerPos, *(items[i]->pos)) 
                && items[i]->collided == 0) {
            return items[i];
        }
    }
    return NULL;  
}


void handle_collision(Item *item, Maze *maze, Player *player) {
    if (item == NULL) return;

    debug_item(*item);
    item->collided = 1;

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

    if (!(seeing_item_first_time & (1 << item->id))) {
        seeing_item_first_time |= (1 << item->id);

        drawDialog(item_names[item->id], item_descs[item->id]);
        while (true) {
            char c = uart_getc();
            if (c == '\n') {
                // removeDialog(item->pos);
                render_scene(maze, player->asset, true);
                break;
            }
        }
    }
    
    // TODO: EFFECT
    switch (item->id) {
        case VISION:
            effect_vision(maze, player);
            break;
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


void debug_item(Item item) {
    uart_puts("[Item:");
    switch (item.id) {
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







