
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_gfxPrimitives.h>

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "internal_api.h"

#include "colors.h"
#include "point.h"
#include "picture.h"
#include "brush.h"
#include "modestack.h"
#include "funcdefs.h"
#include "modeswitch.h"

/*
 * SCREEN AND SDL
 */

SDL_Surface* init_videomode(int w, int h) {
  SDL_Surface* scr = SDL_SetVideoMode(w,h,32,
                                      SDL_SWSURFACE | SDL_RESIZABLE);
  if(!scr) {
    logerror("failed to allocate videosurface");
  }

  SDL_WM_SetCaption("Image Editor", NULL );

  return scr;
}

void print_SurfaceFormat(SDL_Surface* surf) {
  SDL_PixelFormat * fmt = surf->format;
  printf("bpp:\t\t %d\n",fmt->BitsPerPixel);
  printf("Bpp:\t\t %d\n",fmt->BytesPerPixel);
  printf("maskRGBA:\t %x,%x,%x,%x\n",fmt->Rmask,fmt->Gmask,fmt->Bmask,fmt->Amask);
  printf("shiftRGBA:\t %d,%d,%d,%d\n",fmt->Rshift,fmt->Gshift,fmt->Bshift,fmt->Ashift);
  printf("lossRGBA:\t %d,%d,%d,%d\n",fmt->Rloss,fmt->Gloss,fmt->Bloss,fmt->Aloss);
  printf("colorkey:\t %x\n",fmt->colorkey);
  printf("alpha:\t %d\n",fmt->alpha);
}

void resize_window(int w, int h) {
  SDL_Surface* new = SDL_SetVideoMode(w,h,32,SDL_SWSURFACE | SDL_RESIZABLE);
  SDL_FreeSurface(global.screen);

  /* set several environment variables */
  global.screen = new;
  global.screenw= w;
  global.screenh= h;
  clear_window(c_white);
}

void clear_window(Color c) {
  SDL_FillRect(global.screen,NULL,
               intColor_fmt(c,global.screen->format));
}

/*
 * IO
 */

int execute_text(char* text){
  if(!text)
    return -1;
  lognote("executing ``%s''",text);
  int succes = execute(text);
  
  if(succes == -404){
    logsub("passing to mode",text);  
    succes = call__Modespec(global.modelist,text);
  } 
  if(succes == -404) {
    logsub("passing to previously called mode",text);  
    pull_cur_mode();
    char* temp = malloc(strlen(global.UImode) + strlen(text) + 2);
    sprintf(temp,"%s %s",global.UImode,text);
    succes = call__Modespec(global.modelist,temp);
    free(temp);
    push_cur_mode();
  }
  if(succes == -404)
    set_UImess("Unknown command.");
  else if(succes < -1)
    set_UImess("...");
  pull_cur_mode();
  return succes;
}

void update_inputbuffer(SDL_keysym k, char* message) {
  if(strlen(global.inputbuffer) < 255) {
    /* implement backspace */
    if(k.sym == SDLK_BACKSPACE) {
      if(strlen(global.inputbuffer))
        global.inputbuffer[strlen(global.inputbuffer)-1] = 0;
    }
    /* implement caps*/
    else if((k.mod & KMOD_SHIFT)
            && k.sym >= SDLK_a && k.sym <= SDLK_z)
      global.inputbuffer[strlen(global.inputbuffer)] = k.sym - SDLK_a + 1 + SDLK_AT;
    /* implement other modifiers */
    else if((k.mod & KMOD_SHIFT)) {
      char other = 0;
      switch(k.sym) {
      case SDLK_1:            other = SDLK_EXCLAIM; break;
      case SDLK_2:            other = SDLK_AT; break;
      case SDLK_3:            other = SDLK_HASH; break;
      case SDLK_4:            other = SDLK_DOLLAR; break;
      case SDLK_6:            other = SDLK_CARET; break;
      case SDLK_7:            other = SDLK_AMPERSAND; break;
      case SDLK_8:            other = SDLK_ASTERISK; break;
      case SDLK_9:            other = SDLK_LEFTPAREN; break;
      case SDLK_0:            other = SDLK_RIGHTPAREN; break;
      case SDLK_QUOTE:        other = SDLK_QUOTEDBL; break;
      case SDLK_SEMICOLON:    other = SDLK_COLON; break;
      case SDLK_COMMA:        other = SDLK_LESS; break;
      case SDLK_PERIOD:       other = SDLK_GREATER; break;
      case SDLK_SLASH:        other = SDLK_QUESTION; break;
      case SDLK_LEFTBRACKET:  other = '{'; break;
      case SDLK_RIGHTBRACKET: other = '}'; break;
      case SDLK_BACKSLASH:    other = '|'; break;
      case SDLK_5:            other = '%'; break;
      case SDLK_BACKQUOTE:    other = '~'; break;
      default: break;
      }
      if(other)
        global.inputbuffer[strlen(global.inputbuffer)] = other;
    }
    /* normal keys */
    else if(k.sym >= SDLK_SPACE && k.sym <= SDLK_z)
      global.inputbuffer[strlen(global.inputbuffer)] = k.sym;
  }

  if(message) {
    set_UImess("%s : %s",message,global.inputbuffer);
  } else {
    set_UImess("%s",global.inputbuffer);
  }
}

/* 
 * UI interaction 
 */

void update_UIstr() {
    snprintf(global.UIstr,300,"%s> [%s] :: %s :: %s%c",
	     global.UImode,global.UImmode,global.UImess,global.filename,
	     (*global.saved ? ' ' : '*') );
}

int set_UImess(const char* mess, ...) {
  va_list ap;
  int n;
  va_start(ap,mess);
  n = vsnprintf(global.UImess,200,mess,ap);
  va_end(ap);
  writelog(globallog,-1,"~ %s",global.UImess);
  return n;
}

int update_UImess(const char* mess, ...){
  va_list ap;
  int n;
  va_start(ap,mess);
  n = vsnprintf(global.UImess,200,mess,ap);
  va_end(ap);
  return n;
}

int set_UImode(char* mode) {
  strncpy(global.UImode,mode,30);
  Modespec* mode_cur = get_Modespec(global.modelist,mode);
  xsafe_method_call(mode_cur,activate);
  return 0;
}

int set_filename(char* fn) {
  strncpy(global.filename,fn,200);
  return 0;
}

int set_UImmode(char* mmode) {
  strncpy(global.UImmode,mmode,10);
  return 0;
}

Modestack* push_cur_mode() {
  global.modestack = push_Modestack(global.modestack,global.UImode,global.UImmode);
  return global.modestack;
}

Modestack* pull_cur_mode() {
  if(global.modestack) {
    set_UImode(global.modestack->mode);
    set_UImmode(global.modestack->mmode);
  }
  return global.modestack = pop_Modestack(global.modestack);
}

void set_random_UImess(void) {
#define RANDOM_MESSAGES 4
  switch(rand() % RANDOM_MESSAGES) {
  case 0:
    set_UImess("This isn't even my final form!"); break;
  case 1:
    set_UImess(">2012 >still using photoshop ISHYGDDT"); break;
  case 2:
    set_UImess("Please refrain from making sudden mouse movements; it may upset the tigers."); break;
  case 3:
    set_UImess("...now available in white."); break;
  }
}

/* MISC */
char* modname(char* name,char identifier) {
  static int counter = 0;
  static char container[] = "temp:-00000";

  if(!name)
    return NULL;
  
  if(name[0] != '\0') {
    /* another $ signifies : give a unique identifier */
    if(name[0] == '$') {
      snprintf(container,sizeof(container),"temp:%c%d",identifier,counter);
      counter++;
      return container;
    }    
    return name;
  }
  return NULL;
}

void update_color_from_mouse(Color* c,int sat,int alpha) {
  int norm = fmin(255.0, 3*norm_Point(add_Point( global.mc, min_Point(global.m))));
  int arg = (127.0/M_PI) *(M_PI + arg_Point(add_Point( global.mc, min_Point(global.m))));
  *c = hsv2Color(arg,sat,norm,alpha);
}

int save_buffer(char* filename) {
  commit_Picture(&global.pic);
  int succes = SDL_SaveBMP(global.pic.primary,filename);

  *global.saved = true;
  set_filename(filename);
  return succes;
}

int load_buffer(char* filename) {
  SDL_Surface* img = IMG_Load(filename);
  if(img) {
    SDL_BlitSurface(img,NULL,global.pic.primary,NULL);
    SDL_FreeSurface(img);
    *global.saved = true;
    set_filename(filename);
    return 0;
  }
  return -1;
}

/* REGISTERING GLOBAL VARS */
int register__global_brush(Brush* b) {
  if(b) {
    global.brush = b;
    return 0;
  }
  return 1;
}

int register__global_color(Color* c) {
  if(c) {
    global.color = c;
    return 0;
  }
  return 1;
}

int register__global_filename(char* filename) {
  if(filename) {
    global.filename = filename;
    return 0;
  }
  return 1;
}

int register__global_saved(int* saved) {
  if(saved) {
    global.saved = saved;
    return 0;
  }
  return 1;
}

int register__global_UI2input(int* ui2input){
  if(ui2input){
    global.UI2input = ui2input;
    return 0;
  }
  return 1;
}

/* AND OTHER THINGS */
int register__function(char* name, Funkyfunc f){
  if(find_Funcdef(&global.funcs,name)) {
    return -1;
  } 
  add_Funcdef(&global.funcs,name,f);
  return 0;
}
