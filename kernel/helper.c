#include "../lib/helper.h"

void helper_welcome(){
	uart_puts("\n");
  uart_puts("d88888b d88888b d88888b d888888b .d888b.   j88D  .d888b.  .d88b.  \n");
	uart_puts("88'     88'     88'     `~~88~~' VP  `8D  j8~88  88' `8D .8P  88. \n");
	uart_puts("88ooooo 88ooooo 88ooooo    88       odD' j8' 88  `V8o88' 88  d'88 \n");
	uart_puts("88~~~~~ 88~~~~~ 88~~~~~    88     .88'   V88888D    d8'  88 d' 88 \n");
	uart_puts("88.     88.     88.        88    j88.        88    d8'   `88  d8' \n");
	uart_puts("88.     88.     88.        88    j88.        88    d8'   `88  d8' \n");
	uart_puts("Y88888P Y88888P Y88888P    YP    888888D     VP   d8'     `Y88P'  \n");
	uart_puts("\n");
	uart_puts("d8888b.  .d8b.  d8888b. d88888b    .d88b.  .d8888.                \n");
	uart_puts("88  `8D d8' `8b 88  `8D 88'       .8P  Y8. 88'  YP                \n");
	uart_puts("88oooY' 88ooo88 88oobY' 88ooooo   88    88 `8bo.                  \n");
	uart_puts("88~~~b. 88~~~88 88`8b   88~~~~~   88    88   `Y8b.                \n");
	uart_puts("88   8D 88   88 88 `88. 88.       `8b  d8' db   8D                \n");
	uart_puts("Y8888P' YP   YP 88   YD Y88888P    `Y88P'  `8888Y'                \n");
	uart_puts("\n");
	uart_puts("DEVELOPED BY DAT PHAM        - S3927188\n");
	uart_puts("             HUAN NGUYEN     - S3927467\n");
	uart_puts("             NHAN TRUONG     - S3929215\n");
	uart_puts("             PAVEL POTEMKIN  - S3963284\n");
	uart_puts("\n");
}

// help general
void helper_generic(){
	uart_puts(
		"help <command name>  --> Full information of a command\n"
		"clear                --> Clear screen\n"
		"setcolor             --> Set text or background color\n"
		"showinfo             --> Inspect board information\n"
		"config             	--> Configure UART\n"
	);
}

// help with specific cmd
void helper_specific(enum Cmd cmdIdx){
  switch (cmdIdx) {
    case CLEAR:{
      uart_puts("-> Your terminal will be super clean!\n");
      break;
		}
  }
}