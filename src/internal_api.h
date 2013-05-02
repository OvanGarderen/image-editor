#pragma once

#include "colors.h"
#include "point.h"
#include "picture.h"
#include "brush.h"
#include "modestack.h"
#include "funcdefs.h"
#include "modeswitch.h"
#include "cbqueue.h"
#include "selection.h"

#include "logging.h"

/*
 *   GLOBAL VARIABLES 
 */

#include "globalstruct.h"

Globalstruct global;

/*
 * SCREEN AND SDL
 */

SDL_Surface* init_videomode(int w, int h);
void resize_window(Globalstruct* glob, int w, int h);
void clear_window(Globalstruct* glob, Color c);
void print_SurfaceFormat(SDL_Surface* surf);

/* IO */
int execute_text(Globalstruct* glob, char* text);

/* UI INTERACTION */
void update_UIstr(Globalstruct* glob);
int set_UImess(Globalstruct* glob, const char* mess, ...);
int set_UImmode(Globalstruct* glob, char* mmode);
int set_UImode(Globalstruct* glob, char* mode, char* arg);
int set_filename(Globalstruct* glob, char* filename);
void set_random_UImess(Globalstruct* glob);
Modestack* pull_cur_mode(Globalstruct* glob);
Modestack* push_cur_mode(Globalstruct* glob);

int update_UImess(Globalstruct* glob, const char* mess, ...);

/* MISC */
char* modname(char* name,char identifier);
void update_color_from_mouse(Globalstruct* glob, Color* c,int sat,int alpha);
int save_buffer(Globalstruct* glob, char* filename);
int load_buffer(Globalstruct* glob, char* filename);

/* REGISTERING GLOBAL VARS */
int register__global_brush(Globalstruct* glob, Brush* b);
int register__global_selection(Globalstruct* glob, Selection* select);
int register__global_color(Globalstruct* glob, Color* c);
int register__global_filename(Globalstruct* glob, char* filename);
int register__global_saved(Globalstruct* glob, int* saved);
int register__global_UI2input(Globalstruct* glob, int* ui2input);

/* REGISTER CALLBACKS */
int register__draw(Globalstruct* glob, void (*draw)(Modespec*), Modespec* origin);

/* AND OTHER THINGS */
int register__function(Globalstruct* glob, char* name, Funkyfunc f);

/* MACROS */
int execute(Globalstruct* glob, char* command);
