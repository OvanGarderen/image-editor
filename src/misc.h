#define stralloc(stringinit) malloc(strlen(stringinit))
#define with(var, init) for(var = init; var; var = realloc(var,0) )

char* strgrow(char* str, int grow);
char* strappend(char* str, char* append);
char* strclone(char* str);


