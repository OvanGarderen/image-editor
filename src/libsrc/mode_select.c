#include <SDL_gfxPrimitives.h>

#include "mode_select.h"
#include "colors.h"
#include "selection.h"
#include "internal_api.h"

Selectvars selectvars;
#define get(obj,var) ((Selectvars*) (obj))->var

void set_selection(Modespec* self, Rect selection){
  get(self->vars,select).area = selection;
  get(self->vars,select).active = 1;
}

void shut_selection(Modespec* self){
  get(self->vars,select.active) = 0;
}

/* METHODS */

/*Modespec* init__select(Modespec_el* context) {
  Modespec* spec = init__Modespec(context);
  if(spec) {
    /* initialise vars */
    spec->vars = &selectvars;
    get(spec->vars,select) = (Selection) {.active=0};
    
    modelog("select","registered");
    logsub("draw/activate/click/mouse");
    spec->draw = draw__select;
    spec->clickhandler = clickhandler__select;
    spec->mousehandler = mousehandler__select;
    logsub("registered selection as global");
  }
  return spec;
}*/

void vars__select(Modespec* self) {
  self->vars = calloc(1,sizeof(Selectvars));
  get(self->vars,select) = (Selection) {.active=0};
  register__global_selection(self->global,&get(self->vars,select));
}

void draw__select(Modespec* self) {
  if(get(self->vars,select).active)
    draw_selection(self->global->screen,&get(self->vars,select),c_green);
  else if(get(self->vars,clickstatus))
    draw_selection(self->global->screen,&get(self->vars,select),c_fuchsia);
}

int clickhandler__select(Modespec* self, SDL_MouseButtonEvent* mbev) {
  get(self->vars,clickstatus) = mbev->state;
  if(get(self->vars,clickstatus)){
    get(self->vars,clickpoint)[0] = mbev->x;
    get(self->vars,clickpoint)[1] = mbev->y;
    get(self->vars,select).active = 0;
  } else {
    if(get(self->vars,select).area.w && get(self->vars,select).area.h)
      get(self->vars,select).active = 1;
  }
  return 1;
}

int mousehandler__select(Modespec* self, SDL_MouseMotionEvent* motion){
  if(get(self->vars,clickstatus)) {
    int dx = motion->x - get(self->vars,clickpoint)[0];
    int dy = motion->y - get(self->vars,clickpoint)[1];
    
    if(dx > 0) {
      get(self->vars,select).area.x = get(self->vars,clickpoint)[0];
      get(self->vars,select).area.w = dx;
    } else {
      get(self->vars,select).area.x = motion->x;
      get(self->vars,select).area.w = -dx;
    }
    
    if(dy > 0) {
      get(self->vars,select).area.y = get(self->vars,clickpoint)[1];
      get(self->vars,select).area.h = dy;
    } else {
      get(self->vars,select).area.y = motion->y;
      get(self->vars,select).area.h = -dy;
    }
  }
  return 1;
}
