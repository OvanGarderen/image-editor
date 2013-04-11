#include <SDL/SDL.h>
#include "picture.h"

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define  picture_rmask 0xff000000
#define  picture_gmask 0x00ff0000
#define  picture_bmask 0x0000ff00
#define  picture_amask 0x000000ff
#else
#define  picture_rmask 0x000000ff
#define  picture_gmask 0x0000ff00
#define  picture_bmask 0x00ff0000
#define  picture_amask 0xff000000
#endif

int init_Picture(Picture* p, unsigned int w, unsigned int h) {
  SDL_Surface* primary = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA ,w,h,32,
				     picture_rmask,picture_gmask,picture_bmask,picture_amask);
  SDL_Surface* buffer  = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA ,w,h,32,
				     picture_rmask,picture_gmask,picture_bmask,picture_amask);
  if(primary && buffer){
    p->primary = primary;
    p->drawing = buffer;
    p->updated = 0;
    SDL_FillRect(buffer,NULL,
               intColor_fmt(c_trans,buffer->format));
    return 0;
  } 
  return -1;
}

SDL_Surface* get_drawingsurf_Picture(Picture* p){
  if(p){
    p->updated = 1;
    return p->drawing;
  }
  return NULL;
}

void clean_drawingsurf(Picture* p){
  SDL_FillRect(p->drawing,NULL,
               intColor_fmt(c_trans,p->drawing->format));
}

void commit_Picture(Picture* p){
  if(p) {
    SDL_BlitSurface(p->drawing,NULL,p->primary,NULL);
    p->updated = 0;
    clean_drawingsurf(p);
  }
}

void blit_Picture(SDL_Surface* dest, Picture* p){
  if(dest && p) {
    SDL_BlitSurface(p->primary,NULL,dest,NULL);
    if(p->updated)
      SDL_BlitSurface(p->drawing,NULL,dest,NULL);
  }
}

void clear_Picture(Picture* p,Color c) {
  if(p) {
    SDL_FillRect(p->primary,NULL,
		 SDL_SwapBE32(intColor(c)));
  }
}

void destruct_Picture(Picture* p) {
  if(p) {
    SDL_FreeSurface(p->primary);
    SDL_FreeSurface(p->drawing);
    p->primary = NULL;
    p->drawing = NULL;
  }
}
