#pragma once

#include <SDL/SDL.h>
#include "colors.h"

typedef struct {	
  SDL_Surface* surface;
  int updated;
  unsigned int w,h;
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

Picture init_Picture(unsigned int w, unsigned int h);
void clear_picture(Picture p,Color c);
