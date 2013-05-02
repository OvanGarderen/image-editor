#pragma once

#include <SDL.h>

typedef struct inputbuffer Inputbuffer;
typedef struct bufferspec Bufferspec;
typedef int (*Callback)(char* args);

typedef enum {CBtype_command,CBtype_funcp} callbacktype;

struct bufferspec {
  int callbacktype;
  char* name;
  char* returncomm;
  Callback callback;
};

void init_Inputbuffer(void);
void start_Inputbuffer(Bufferspec spec);
void update_Inputbuffer(SDL_keysym k);
void send_Inputbuffer(void);
void break_Inputbuffer(void);
char SDLK_getascii(SDL_keysym sym);
