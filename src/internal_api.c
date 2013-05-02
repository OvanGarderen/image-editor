
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

void resize_window(Globalstruct* glob, int w, int h) {
  SDL_Surface* new = SDL_SetVideoMode(w,h,32,SDL_SWSURFACE | SDL_RESIZABLE);
  SDL_FreeSurface(glob->screen);

  /* set several environment variables */
  glob->screen = new;
  glob->screenw= w;
  glob->screenh= h;
  clear_window(glob,c_white);
}

void clear_window(Globalstruct* glob, Color c) {
  SDL_FillRect(glob->screen,NULL,
               intColor_fmt(c,glob->screen->format));
}

/*
 * IO
 */

int execute_text(Globalstruct* glob, char* text) {
  if(!text)
    return -1;
  lognote("executing ``%s''",text);
  int succes = execute(glob,text);
  
  if(succes == -404) {
    logsub("passing to mode",text);  
    succes = call__Modespec(glob->modelist,text);
  } 
  if(succes == -404) {
    logsub("passing to previously called mode",text);  
    pull_cur_mode(glob);
    char* temp = malloc(strlen(glob->UImode) + strlen(text) + 2);
    sprintf(temp,"%s %s",glob->UImode,text);
    succes = call__Modespec(glob->modelist,temp);
    free(temp);
    push_cur_mode(glob);
  }
  if(succes == -404)
    set_UImess(glob,"Unknown command.");
  else if(succes < -1)
    set_UImess(glob,"...");
  pull_cur_mode(glob);
  return succes;
}

/* 
 * UI interaction 
 */

void update_UIstr(Globalstruct* glob) {
    snprintf(glob->UIstr,300,"%s> [%s] :: %s :: %s%c",
	     glob->UImode,glob->UImmode,glob->UImess,glob->filename,
	     (*glob->saved ? ' ' : '*') );
}

int set_UImess(Globalstruct* glob, const char* mess, ...) {
  va_list ap;
  int n;
  va_start(ap,mess);
  n = vsnprintf(glob->UImess,200,mess,ap);
  va_end(ap);
  writelog(globallog,-1,"~ %s",glob->UImess);
  return n;
}

int update_UImess(Globalstruct* glob, const char* mess, ...) {
  va_list ap;
  int n;
  va_start(ap,mess);
  n = vsnprintf(glob->UImess,200,mess,ap);
  va_end(ap);
  return n;
}

int set_UImode(Globalstruct* glob, char* mode, char* arg) {
  strncpy(glob->UImode,mode,30);
  Modespec* mode_cur = get_Modespec(glob->modelist,mode);
  xsafe_method_call(mode_cur,activate,arg);
  return 0;
}

int set_filename(Globalstruct* glob, char* fn) {
  strncpy(glob->filename,fn,200);
  return 0;
}

int set_UImmode(Globalstruct* glob, char* mmode) {
  strncpy(glob->UImmode,mmode,10);
  return 0;
}

Modestack* push_cur_mode(Globalstruct* glob) {
  glob->modestack = push_Modestack(glob->modestack,glob->UImode,glob->UImmode);
  return glob->modestack;
}

Modestack* pull_cur_mode(Globalstruct* glob) {
  if(glob->modestack) {
    set_UImode(glob,glob->modestack->mode,"");
    set_UImmode(glob,glob->modestack->mmode);
  }
  return glob->modestack = pop_Modestack(glob->modestack);
}

void set_random_UImess(Globalstruct* glob) {
#define RANDOM_MESSAGES 4
  switch(rand() % RANDOM_MESSAGES) {
  case 0:
    set_UImess(glob,"This isn't even my final form!"); break;
  case 1:
    set_UImess(glob,">2012 >still using photoshop ISHYGDDT"); break;
  case 2:
    set_UImess(glob,"Please refrain from making sudden mouse movements; it may upset the tigers."); break;
  case 3:
    set_UImess(glob,"...now available in white."); break;
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

void update_color_from_mouse(Globalstruct* glob, Color* c,int sat,int alpha) {
  int norm = fmin(255.0, 3*norm_Point(add_Point( glob->mc, min_Point(glob->m))));
  int arg = (127.0/M_PI) *(M_PI + arg_Point(add_Point( glob->mc, min_Point(glob->m))));
  *c = hsv2Color(arg,sat,norm,alpha);
}

int save_buffer(Globalstruct* glob, char* filename) {
  commit_Picture(&glob->pic,glob->select);
  int succes = SDL_SaveBMP(glob->pic.primary,filename);

  if(succes) {
    *glob->saved = true;
    set_filename(glob,filename);
  } else {
    set_UImess(glob,"Could not save file %s",filename);
  }
  return succes;
}

int load_buffer(Globalstruct* glob, char* filename) {
  SDL_Surface* img = IMG_Load(filename);
  if(img) {
    SDL_BlitSurface(img,NULL,glob->pic.primary,NULL);
    SDL_FreeSurface(img);
    *glob->saved = true;
    set_filename(glob,filename);
    return 0;
  } else {
    set_UImess(glob,"Not found file %s",filename);
  }
  return -1;
}

/* REGISTERING GLOBAL VARS */
int register__global_brush(Globalstruct* glob, Brush* b) {
  if(b) {
    glob->brush = b;
    return 0;
  }
  return 1;
}

int register__global_selection(Globalstruct* glob, Selection* select) {
  if(select) {
    glob->select = select;
    return 0;
  }
  return 1;
}

int register__global_color(Globalstruct* glob, Color* c) {
  if(c) {
    glob->color = c;
    return 0;
  }
  return 1;
}

int register__global_filename(Globalstruct* glob, char* filename) {
  if(filename) {
    glob->filename = filename;
    return 0;
  }
  return 1;
}

int register__global_saved(Globalstruct* glob, int* saved) {
  if(saved) {
    glob->saved = saved;
    return 0;
  }
  return 1;
}

int register__global_UI2input(Globalstruct* glob, int* ui2input) {
  if(ui2input) {
    glob->UI2input = ui2input;
    return 0;
  }
  return 1;
}

/* REGISTER METHODS AS GLOBAL */
int register__draw(Globalstruct* glob, void (*draw)(Modespec*), Modespec* origin) {
  printf("%p == %p, %s\n",draw,origin->draw,origin->name);
  cbqueue_add(&(glob->drawqueue),draw, origin);
  return 0;
}

/* AND OTHER THINGS */
int register__function(Globalstruct* glob, char* name, Funkyfunc f) {
  if(find_Funcdef(&glob->funcs,name)) {
    return -1;
  } 
  add_Funcdef(&glob->funcs,name,f);
  return 0;
}

/* misc */
int execute(Globalstruct* glob, char* command) {
  return call_Funcdef(&glob->funcs,command);
}
