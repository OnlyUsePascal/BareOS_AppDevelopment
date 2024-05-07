#include "helper.h"
#include "utils.h"
#include "framebf.h"

void cli_processCmd(const char *cmd, int cmdSz);
enum Cmd cli_findCmd(const char *cmd);
void cli_help(const char *cmd);
void cli_clrscr();
void cli_img();
void cli_vid();

#define VID_WIDTH 640
#define VID_HEIGHT 480



