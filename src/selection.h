#pragma once

#include <SDL.h>
#include "colors.h"

typedef SDL_Rect Rect;
typedef struct selection Selection;

struct selection {
  int strict; /* saves whether we can draw outside selection */
  int active;
  int type;
  Rect area;
};

SDL_Surface* limit_Selection(SDL_Surface* origin, Selection* select);
void draw_selection(SDL_Surface* where, Selection* select, Color c);
