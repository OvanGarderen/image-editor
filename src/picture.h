#pragma once

#include <SDL/SDL.h>
#include "colors.h"
#include "selection.h"

typedef struct {	
  SDL_Surface* primary;
  SDL_Surface* drawing;
  int updated;
} Picture;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define  rmask 0xff000000
#define  gmask 0x00ff0000
#define  bmask 0x0000ff00
#define  amask 0x000000ff
#else
#define  rmask 0x000000ff
#define  gmask 0x0000ff00
#define  bmask 0x00ff0000
#define  amask 0xff000000
#endif

int init_Picture(Picture* p, unsigned int w, unsigned int h);
SDL_Surface* get_drawingsurf_Picture(Picture* p);
void blit_Picture(SDL_Surface* dest, Picture* p);
void commit_Picture(Picture* p, Selection* select);
void clear_Picture(Picture* p,Color c);
void destruct_Picture(Picture* p);
