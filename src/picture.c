#include <SDL/SDL.h>
#include "picture.h"

Picture init_Picture(unsigned int w, unsigned int h){
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
  Picture ret;
  ret.w = w;
  ret.h = h;
  SDL_Surface* temp = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA,w,h,32,
				     rmask,gmask,bmask,amask);
  puts("before");

  ret.surface = temp;
  ret.updated = 0;
  return ret;
}

void clear_picture(Picture p,Color c){
  /* somehow SDL fucks up red and blue but fuck it*/
  Color dummy = c;
  //dummy.r = c.b;
  //dummy.b = c.r;
  SDL_FillRect(p.surface,NULL,
	       SDL_SwapBE32(intColor(dummy)));
}
