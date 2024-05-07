#include "../lib/cli.h"
#include "../lib/data/data_vid.h"

// calibrate cmd (trim space) then call suitable func
void cli_processCmd(const char *cmd, int cmdSz){
  // trim space
  char *cmdLastChar = cmd + cmdSz;
  while (*cmd == ' ') cmd++; //trim first spaces
  if (cmdLastChar > cmd){ //trim last spaces
    while (*(cmdLastChar-1) == ' ') cmdLastChar--;
    *cmdLastChar = '\0';
  }
  
  // retrieve base command (first word)
  char cmdBase[CMD_SZ];
  str_find_nth_word(cmd, cmdBase, 0);
  enum Cmd cmdIdx = cli_findCmd(cmdBase);
  
  switch (cmdIdx) {
    case HELP:
      cli_help(cmd);
      break;

    case CLEAR:
      cli_clrscr();
      break;
    
    case IMAGE:
      cli_img();
      break;
      
    case VIDEO:
      cli_vid();
      break;
    
    case FONT: 
      cli_font();
      break;
      
    default:
      uart_puts("Command not found!\n");
      break;
  }
}


// find index of given cmd in avalablie cmd list
enum Cmd cli_findCmd(const char *cmd){
  enum Cmd cmdIdx = -1; 
  for (int i = 0; i < cmdListSz; i++){
    if (str_cmp(cmd, cmdList[i]) == 0) {
      cmdIdx = i;
      break;    
    }
  }
  
  return cmdIdx;
}


// cmd help
void cli_help(const char *cmd){
  uart_puts("-- Help --\n");
  // retrieve command name
  char cmdBase[CMD_SZ];
  str_find_nth_word(cmd, cmdBase, 1); 

  // generic help
  if (str_len(cmdBase) == 0){ 
    helper_generic();
    return;
  }
  
  helper_specific(cli_findCmd(cmdBase));  
}


void cli_clrscr() {
  uart_puts("\033[2J\033[H");
}


void cli_img(){
  uart_puts("--> cli image!!\n");
}


void cli_vid(){
  uart_puts("--> cli video!!\n");
  
  framebf_init(VID_WIDTH, VID_HEIGHT, VID_WIDTH, VID_HEIGHT);
  // play video 3 times
  for (int i = 0 ; i < 3 ; i++) {
    for (int frame = 0; frame < epd_bitmap_allArray_LEN; frame++){
      framebf_drawImg(0,0,VID_WIDTH, VID_HEIGHT, epd_bitmap_allArray[frame]);
      wait_msec(62500);
    }
    wait_msec(1000000);
  }
}


void cli_font(){
  uart_puts("--> cli font!!\n");
}


