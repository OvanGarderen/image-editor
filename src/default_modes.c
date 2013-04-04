#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include "modeswitch.h"
#include "internal_api.h"
#include "brush.h"
#include "default_modes.h"
#include "common_commands.h"

/*
 * BRUSH MODE
 */

Modespec* init__brush(Modespec_el* context){
  Modespec* spec = init__Modespec(context);
  if(spec) {
    /* initialise vars */
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
    if(!brushvars_of(self)->suppres){
      apply_brush(global.pic.surface,
		  & brushvars_of(self)->brush,
		  pos,
		  brushvars_of(self)->brush_col);
    }
    brushvars_of(self)->active = true;
  } 
  else if( mbev->button == SDL_BUTTON_LEFT && mbev->state == SDL_RELEASED) {
    brushvars_of(self)->active = false;
  }
  return 0;
}

int mousehandler__brush(Modespec* self, SDL_MouseMotionEvent* motion) {
  if( brushvars_of(self)->active && !brushvars_of(self)->suppres ){
    Point fromwhence = global.mprev;
    Point from = global.m;
    Point to = {motion->x,
		motion->y};
    Point towards = subs_Point( subs_Point( mult_Point(2,to),
				mult_Point(2,fromwhence)),
				from);

    /*    stroke_brush_smooth(&global.pic,
			& brushvars_of(self)->brush,
			fromwhence,from,
			to, towards,
			brushvars_of(self)->brush_col); */
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
    case SDLK_TAB:
      brushvars_of(self)->suppres = true;
      break;
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
  } else {
    switch(key->keysym.sym){
    case SDLK_TAB:
      brushvars_of(self)->suppres = false;
      break;
    default:
      return 0;
    }
  }
  return 1;
}

/*
 * COLOR MODE
 */

Modespec* init__color(Modespec_el* context){
  Modespec* spec = init__Modespec(context);
  if(spec) {
    /* initialise vars */
    spec->vars = &colorvars;
    colorvars.col_current = c_red;

    spec->draw = draw__color;
    spec->activate = activate__color;
    spec->clickhandler = clickhandler__color;
    spec->mousehandler = mousehandler__color;
    spec->keyhandler = keyhandler__color;
    spec->call = call__color;
  }
  return spec;
}

#define colorvars_of(object) (ACC_VARS(object,Colorvars))

void activate__color(Modespec* self){
  colorvars_of(self)->globe_select = false;
  colorvars_of(self)->col_current = global.color;
  colorvars_of(self)->pref_saturation = 255;
  colorvars_of(self)->pref_hue = 255;
  colorvars_of(self)->pref_value = 255;
  colorvars_of(self)->pref_alpha = global.color.a;
}

void draw__color(Modespec* self){
  Color* c = &colorvars_of(self)->col_current;
  c->a = colorvars_of(self)->pref_alpha;
  global.color = *c;

  if( global.mb_down ){
    filledCircleColor(global.screen, global.mc.x, global.mc.y,40,
		      intColor(colorvars_of(self)->col_current));
    /* draw saturation */
    boxColor(global.screen,
	     global.mc.x + 50, 
	     global.mc.y + 40 - (80.0/255.0)*colorvars_of(self)->pref_saturation,
	     global.mc.x + 60, 
	     global.mc.y + 40,
	     intColor(colorvars_of(self)->col_current));
    /* draw alpha */
    boxColor(global.screen,
	     global.mc.x + 50, 
	     global.mc.y + 40 - (80.0/255.0)*colorvars_of(self)->pref_alpha,
	     global.mc.x + 60, 
	     global.mc.y + 40,
	     intColor(colorvars_of(self)->col_current));

    global.pic.updated = 1;
  }

  int i = 0;
  Colordef* _node = global.colorlist.start;  
  for(; _node != NULL && i<20; _node = _node->next){
    if(strstr(_node->name,"temp:c")){
      boxColor(global.screen,
	       i*30     , global.screenh - 16 - 32,
	       i*30 + 30, global.screenh - 16, intColor(_node->c));
      i++;
    }
  }
}

int clickhandler__color(Modespec* self, SDL_MouseButtonEvent* mbev) {
  if( mbev->button == SDL_BUTTON_LEFT )
    colorvars_of(self)->globe_select = (mbev->state == SDL_PRESSED);
  return 0;
}

int mousehandler__color(Modespec* self, SDL_MouseMotionEvent* motion) {  
  if(colorvars_of(self)->globe_select)
    update_color_from_mouse(& colorvars_of(self)->col_current,
			    colorvars_of(self)->pref_saturation,
			    colorvars_of(self)->pref_alpha);
  return 0;
}

int keyhandler__color(Modespec* self, SDL_KeyboardEvent* key){
  int* sat;
  if( key->state == SDL_PRESSED ) {
    switch(key->keysym.sym){
    case SDLK_RETURN:
      global.color = colorvars_of(self)->col_current;
      pull_cur_mode();
      break;
    case SDLK_a:
      sat = & colorvars_of(self)->pref_saturation;
      *sat = (*sat + 10 < 255)? *sat +10 : 255;
      set_UImess("changed saturation to %d", colorvars_of(self)->pref_saturation);
      break;
    case SDLK_z:
      sat = & colorvars_of(self)->pref_saturation;
      *sat = (*sat - 10 > 0)? *sat - 10 : 0;
      set_UImess("changed saturation to %d", colorvars_of(self)->pref_saturation);
      break;
    case SDLK_s:
      colorvars_of(self)->pref_alpha = 
	(colorvars_of(self)->pref_alpha < 245) ? 
	colorvars_of(self)->pref_alpha + 10 : 255;
      set_UImess("changed alpha to %d", colorvars_of(self)->pref_alpha);
      break;
    case SDLK_x:
      colorvars_of(self)->pref_alpha = 
	(colorvars_of(self)->pref_alpha > 10) ? 
	colorvars_of(self)->pref_alpha - 10 : 0;
      set_UImess("changed alpha to %d", colorvars_of(self)->pref_alpha);
      break;
    default:
      return 0;
    }
  }
  return 1;
}

int call__color(Modespec* self, char* dummyargs){
  char* arg = NULL;

  /* try to parse argument */
  if(dummyargs)
    arg = strtok(dummyargs," ");

  /* given no argument, gives current color values */
  if(!arg) {
    set_UImess("rgba: %d,%d,%d,%d",
	       global.color.r,global.color.g,global.color.b,global.color.a);
    return 0;
  }

  /* given a character $ enter color-define mode */
  if(arg[0] == '$') {
    /* if contains another $ make name */
    char* name = modname(arg+1,'c');
    
    if(!name){
      set_UImess("Bad name-specifier.");
      return -1;
    }

    if(find_Colordef(&global.colorlist,name)) {
      set_UImess("Color already defined.");
      return -1;
    }

    char* value = strtok(NULL," ");
    if( value ) {
      Color c;
      if(parse_Color(&global.colorlist,value,&c)) {
        add_Colordef(&global.colorlist,name,c);
        set_UImess("Defined color %s as #%x.",name,intColor(c));
      }
      else {
        set_UImess("Unknown color-format.",name,intColor(c));
      }
    }
    else {
      add_Colordef(&global.colorlist,name,global.color);
      set_UImess("Defined color %s as current color (#%x)",name,
		 intColor(global.color));
    }
  }
  /* no special cases : find color with specified name */
  else {
    if(parse_Color(&global.colorlist,arg,
		   &colorvars_of(self)->col_current)){
      global.color = colorvars_of(self)->col_current;
      set_UImess("Set color to %s. (#%x)",arg,intColor(global.color));
    }
    else
      set_UImess("Undefined color or bad color-format.");
  }
  return 0;
}

