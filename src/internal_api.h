#pragma once

#include "colors.h"
#include "point.h"
#include "picture.h"
#include "brush.h"
#include "modestack.h"
#include "funcdefs.h"
#include "modeswitch.h"

#include "logging.h"

/* GLOBAL VARIABLES */

struct {
  SDL_Surface *screen;
  int screenw,screenh;
  Picture pic;
  int active;
  int mb_down;
  int suppres;
  SDL_Event event;
  Brush* brush;
  Color* color;
  Point m;
  Point mprev;
  Point mc;
  Modestack* modestack;
  Modelist* modelist;
  char* filename;
  int* saved;
  char UIstr[301];
  char UImode[30];
  char UImmode[10];
  char UImess[200];
  int* UI2input;
  char inputbuffer[256];
  Funclist funcs;
  Colorlist colorlist;
  LOG log;
} global;

/*
 * SCREEN AND SDL
 */

SDL_Surface* init_videomode(int w, int h);
void resize_window(int w, int h);
void clear_window(Color c);
void print_SurfaceFormat(SDL_Surface* surf);

/* IO */
int execute_text(char* text);

/* UI INTERACTION */
void update_UIstr(void);
int set_UImess(const char* mess, ...);
int set_UImmode(char* mmode);
int set_UImode(char* mode);
int set_filename(char* filename);
Modestack* pull_cur_mode();
Modestack* push_cur_mode();
void set_random_UImess(void);

/* MISC */
char* modname(char* name,char identifier);
void update_color_from_mouse(Color* c,int sat,int alpha);
int save_buffer(char* filename);
int load_buffer(char* filename);

/* REGISTERING GLOBAL VARS */
int register__global_brush(Brush* b);
int register__global_color(Color* c);
int register__global_filename(char* filename);
int register__global_saved(int* saved);
int register__global_UI2input(int* ui2input);

/* AND OTHER THINGS */
int register__function(char* name, Funkyfunc f);

/* MACROS */
#define execute(A) call_Funcdef(&global.funcs,A)
#define arraylen(A) (sizeof(A)/sizeof(A[0]))



