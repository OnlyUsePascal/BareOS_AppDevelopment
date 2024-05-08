#include "helper.h"
#include "utils.h"
#include "framebf.h"
#include "game.h"


void cli_processCmd(const char *cmd, int cmdSz);
enum Cmd cli_findCmd(const char *cmd);
void cli_help(const char *cmd);
void cli_clrscr();
void cli_img();
void cli_vid();
void cli_game();

#define VID_W 640
#define VID_H 480






