#include <SDL.h>
#include <SDL_gfxPrimitives.h>

#include "mode_select.h"
#include "colors.h"
#include "selection.h"
#include "internal_api.h"

Selectvars selectvars;
#define get(obj,var) (((Selectvars*) (obj)->vars)->var)
#define get_global(obj,var) ((obj)->global->var) 

void set_selection(Modespec* self, Rect selection) {
  get(self,select).area = selection;
  get(self,select).active = 1;
}

void shut_selection(Modespec* self) {
  get(self,select.active) = 0;
}

/* METHODS */
void vars__select(Modespec* self) {
  self->vars = calloc(1,sizeof(Selectvars));
  get(self,select) = (Selection) {.active=0};
  register__global_selection(self->global,&get(self,select));
  fprintf(stderr,"registring\n");
  register__draw(self->global,draw__select,self);
  fprintf(stderr,"tis gedaan\n");
}

void draw__select(Modespec* self) {
  if(get(self,select).active)
    draw_selection(get_global(self,screen),
		   &get(self,select),c_green);
  else if(get(self,clickstatus))
    draw_selection(get_global(self,screen),
		   &get(self,select),c_fuchsia);
}

int clickhandler__select(Modespec* self, SDL_MouseButtonEvent* mbev) {
  get(self,clickstatus) = mbev->state;
  if(get(self,clickstatus)) {
    get(self,clickpoint)[0] = mbev->x;
    get(self,clickpoint)[1] = mbev->y;
    get(self,select).area.w = 0;
    get(self,select).area.h = 0;
    get(self,select).active = 0;
  } else {
    if(get(self,select).area.w && get(self,select).area.h)
      get(self,select).active = 1;
    else 
      get(self,select).active = 0;
  }
  return 1;
}

int mousehandler__select(Modespec* self, SDL_MouseMotionEvent* motion) {
  if(get(self,clickstatus)) {
    int dx = motion->x - get(self,clickpoint)[0];
    int dy = motion->y - get(self,clickpoint)[1];
    
    if(dx > 0) {
      get(self,select).area.x = get(self,clickpoint)[0];
      get(self,select).area.w = dx;
    } else {
      get(self,select).area.x = motion->x;
      get(self,select).area.w = -dx;
    }
    
    if(dy > 0) {
      get(self,select).area.y = get(self,clickpoint)[1];
      get(self,select).area.h = dy;
    } else {
      get(self,select).area.y = motion->y;
      get(self,select).area.h = -dy;
    }
  }
  return 1;
}

int keyhandler__select(Modespec* self, SDL_KeyboardEvent* key) {
  SDL_Surface* surf;
  
  if( key->state == SDL_PRESSED ) {
    switch(key->keysym.sym) {
    case SDLK_x:
      surf = get_drawingsurf_Picture(&get_global(self,pic));
      SDL_FillRect(surf, &get(self,select).area,
		   intColor_fmt(*get_global(self,color),surf->format));
      commit_Picture(&get_global(self,pic),&get(self,select));
      break;
    case SDLK_z:
      surf = get_drawingsurf_Picture(&get_global(self,pic));
      Rect whole = {.x = 0, .y = 0, .w = surf->w, .h = surf->h};
      SDL_FillRect(surf, &whole ,
		   intColor_fmt(*get_global(self,color),surf->format));
      SDL_FillRect(surf, &get(self,select).area,
		   intColor_fmt(c_trans,surf->format));
      int temp = get(self,select).active;
      get(self,select).active = 0;
      commit_Picture(&get_global(self,pic),&get(self,select));
      get(self,select).active = temp;
      break;
    default:
      return 0;
    }
  }
  return 1;
}
