#include "internal_api.h"

#include "brush.h"
#include "drawing.h"
#include "misc.h"
#include "logging.h"
#include "modeswitch.h"

#include "mode_brush.h"

Brushvars brushvars;

Modespec* init__brush(Modespec_el* context) {
  Modespec* spec = init__Modespec(context);
  if(spec) {
    /* initialise vars */
    spec->vars = &brushvars;
    brushvars.brush = brush_std;

    modelog("brush","registered");
    logsub("draw/activate/click/mouse/key");
    spec->draw = draw__brush;
    spec->activate = activate__brush;
    spec->clickhandler = clickhandler__brush;
    spec->mousehandler = mousehandler__brush;
    spec->keyhandler = keyhandler__brush;
    logsub("brush as global brush");
    register__global_brush(&global,&brushvars.brush);
  }
  return spec;
}

void activate__brush(Modespec* self) {
  brushvars.suppres = 0;
  brushvars.active = 0;
  brushvars.brush_col = *global.color;
}

void draw__brush(Modespec* self) {
  draw_ring(global.screen,global.m,
                    (float) brushvars.brush.size,
	    (float) brushvars.brush.size - fmin(5.0,brushvars.brush.size/1.11),
		    brushvars.brush_col);
}

int clickhandler__brush(Modespec* self, SDL_MouseButtonEvent* mbev) {
  if( mbev->button == SDL_BUTTON_LEFT && mbev->state ) {
    Point pos = {mbev->x,mbev->y};
    if(!brushvars.suppres) {
      apply_brush(get_drawingsurf_Picture(&global.pic),
		  & brushvars.brush,
		  pos,
		  brushvars.brush_col);
    }
    brushvars.active = 1;
  } 
  else if( mbev->button == SDL_BUTTON_LEFT) {
    commit_Picture(&global.pic,global.select);
    brushvars.active = 0;
  }
  return 0;
}

int mousehandler__brush(Modespec* self, SDL_MouseMotionEvent* motion) {
  if( brushvars.active && !brushvars.suppres ) {
    Point from = global.m;
    Point to = {motion->x,
		motion->y};

    Point mid = subs_Point(mult_Point(2.0,global.m),global.mprev);

    /*    stroke_brush_smooth(&global.pic,
			& brushvars.brush,
			global.mprev,from,
			to, subs_Point(mult_Point(2.0,to),from),
			brushvars.brush_col); */
    stroke_brush_straight(&global.pic,
			  & brushvars.brush,
			  from, to,
			  brushvars.brush_col);
  }
  return 0;
}

int keyhandler__brush(Modespec* self, SDL_KeyboardEvent* key) {
  if( key->state == SDL_PRESSED ) {
    switch(key->keysym.sym) {
    case SDLK_TAB:
      brushvars.suppres = 1;
      break;
    case SDLK_a:
      brush_change_size(& (brushvars.brush),1); 
      set_UImess("changed brush size to %d", brushvars.brush.size);
      break;
    case SDLK_z:
      brush_change_size(& (brushvars.brush),-1); 
      set_UImess("changed brush size to %d", brushvars.brush.size);
      break;
    default:
      return 0;
    }
  } else {
    switch(key->keysym.sym) {
    case SDLK_TAB:
      brushvars.suppres = 0;
      break;
    default:
      return 0;
    }
  }
  return 1;
}
