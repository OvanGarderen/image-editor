#pragma once

#include <stdbool.h>

#include "colors.h"
#include "point.h"
#include "picture.h"
#include "brush.h"
#include "modestack.h"
#include "modefuncs.h"
#include "modeswitch.h"

/* GLOBAL VARIABLES */

struct {
  SDL_Surface *screen;
  int screenw,screenh;
  Picture pic;
  bool active;
  bool mb_down;
  bool suppres;
  SDL_Event event;
  Brush brush;
  Color color;
  Point m;
  Point mc;
  Modestack* modestack;
  Modelist* modelist;
  char UIstr[301];
  char UImode[30];
  char UImmode[10];
  char UImess[200];
  bool UI2input;
  char inputbuffer[256];
  Funclist funcs;
  Colorlist colorlist;
} global;

/*
 * SCREEN AND SDL
 */

SDL_Surface* init_videomode();
void print_SDL_error(char * type);
void resize_window(int w, int h);
void clear_window(Color c);

/* IO */
void update_inputbuffer(SDL_keysym k, char* message);

/* UI INTERACTION */
void update_UIstr(void);
int set_UImess(const char* mess, ...);
int set_UImmode(char* mmode);
int set_UImode(char* mode);
Modestack* pull_cur_mode();
Modestack* push_cur_mode();
void set_random_UImess(void);

/* MISC */
char* modname(char* name,char identifier);

/* MACROS */
#define execute(A) call_Modefunc(&global.funcs,A)
#define arraylen(A) (sizeof(A)/sizeof(A[0]))


