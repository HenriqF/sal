#ifndef DIFF
#define DIFF

#define UTIL_FILE
#define UTIL_IMP

#include "../util.h"
#include <string.h>
#include <stdio.h>

#define MODE_ADD 1
#define MODE_RMV 2
#define MODE_DFT 3

void show_diff(char* fa, char* fb, int mode);

#endif