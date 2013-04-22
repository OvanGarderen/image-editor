#include <stdlib.h>
#include "misc.h"

char* strappend(char* str, char* append) {
  return strcat( strgrow(str,strlen(append)), append);
}

char* strgrow(char* str, int grow) {
  return realloc(str,strlen(str) + 1 + grow);
}

char* strclone(char* str) {
  return strcpy( malloc(strlen(str)+1), str);
}
