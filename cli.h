#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include "CEC_Device.h"
#include "WProgram.h"

void cliParseInput();
void cliCharInput();
void cliProcessInput();
void cliSwitchCase(unsigned char c);


#define CLIBUFLEN 64
#define BACKSPACE 0x7F
#define SPACE 0x20

typedef struct clibuffer_t {
  char len;
  boolean complete;
  char position;
  char buffer[CLIBUFLEN + 1];

};
