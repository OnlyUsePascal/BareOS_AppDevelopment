#ifndef KERNEL_H
#define KERNEL_H

void cli();
void cli_processCmd(const char *cmd, int cmdSz);
enum Cmd cli_findCmd(const char *cmd);
void cli_help(const char *cmd);
void cli_clrscr();
void cli_img();
void cli_vid();
void cli_game();

#define VID_W 640
#define VID_H 480


#endif