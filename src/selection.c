#include <SDL_gfxPrimitives.h>
#include "selection.h"

SDL_Surface*
limit_Selection(SDL_Surface* origin, Selection* select){
  if(origin && select && select->active) {
    SDL_Rect left = {.x = 0, .y = 0,
                     .w = select->area.x, .h = origin->h};
    SDL_Rect right = {.x = select->area.x + select->area.w, .y=0,
                      .w = origin->w - (select->area.x + select->area.w), .h=origin->h};
    SDL_Rect upper = {.x = select->area.x, .y= 0,
                      .w = select->area.w, .h= select->area.y};
    SDL_Rect lower = {.x = select->area.x, .y= select->area.y + select->area.h,
                      .w = select->area.w, .h= origin->h - (select->area.y + select->area.h)};
    SDL_FillRect(origin, &left, intColor_fmt(c_trans,origin->format));
    SDL_FillRect(origin, &right, intColor_fmt(c_trans,origin->format));
    SDL_FillRect(origin, &upper, intColor_fmt(c_trans,origin->format));
    SDL_FillRect(origin, &lower, intColor_fmt(c_trans,origin->format));
  }
  return origin;
}


void draw_selection(SDL_Surface* where, Selection* select, Color c){
  rectangleColor(where,select->area.x,select->area.y,
                 select->area.x + select->area.w,
                 select->area.y + select->area.h,intColor(c));
}
