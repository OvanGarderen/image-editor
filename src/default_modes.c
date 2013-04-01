#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include "modeswitch.h"
#include "internal_api.h"
#include "brush.h"
#include "default_modes.h"

Modespec* init__brush(Modespec_el* context){
  Modespec* spec = malloc(sizeof(Modespec));
  if(spec){
    /* set all uninit. vars to 0 */
    memset(spec,0,sizeof(Modespec));

    /* name */
    spec->name = malloc(strlen(context->name));
    strcpy(spec->name,context->name);

    /* vars */
    spec->vars = &brushvars;
    brushvars.brush = brush_std;

    spec->draw = draw__brush;
    spec->activate = activate__brush;
    spec->clickhandler = clickhandler__brush;
    spec->mousehandler = mousehandler__brush;
    spec->keyhandler = keyhandler__brush;
  }
  return spec;
}

#define brushvars_of(object) (ACC_VARS(object,Brushvars))

void activate__brush(Modespec* self){
  brushvars_of(self)->suppres = false;
  brushvars_of(self)->active = false;
  brushvars_of(self)->brush_col = global.color;
}

void draw__brush(Modespec* self){
  //printf("drawing color #%x at position %d,%d\n",intColor(brushvars_of(self)->brush_col),global.m.x,global.m.y);
  filledCircleColor(global.screen,global.m.x,global.m.y,
                    brushvars_of(self)->brush.size,
                    intColor(
                             brushvars_of(self)->brush_col));
}

int clickhandler__brush(Modespec* self, SDL_MouseButtonEvent* mbev) {
  if( mbev->button == SDL_BUTTON_LEFT && mbev->state == SDL_PRESSED ){
    Point pos = {mbev->x,mbev->y};
    apply_brush(global.pic.surface,
		& brushvars_of(self)->brush,
		pos,
		brushvars_of(self)->brush_col);
    brushvars_of(self)->active = true;
  } 
  else if( mbev->button == SDL_BUTTON_LEFT && mbev->state == SDL_RELEASED) {
    brushvars_of(self)->active = false;
  }
  return 0;
}

int mousehandler__brush(Modespec* self, SDL_MouseMotionEvent* motion) {
  if( brushvars_of(self)->active ){
    Point from = {global.m.x,
		  global.m.y};
    Point to = {motion->x,
		motion->y};
    global.pic.updated = 1;
    stroke_brush_straight(&global.pic,
			  & brushvars_of(self)->brush,
			  from, to,
			brushvars_of(self)->brush_col);
  }
  return 0;
}

int keyhandler__brush(Modespec* self, SDL_KeyboardEvent* key){
  if( key->state == SDL_PRESSED ) {
    switch(key->keysym.sym){
    case SDLK_a:
      brush_change_size(& (brushvars_of(self)->brush),1); 
      set_UImess("changed brush size to %d", brushvars_of(self)->brush.size);
      break;
    case SDLK_z:
      brush_change_size(& (brushvars_of(self)->brush),-1); 
      set_UImess("changed brush size to %d", brushvars_of(self)->brush.size);
      break;
    default:
      return 0;
    }
  }
  return 1;
}
