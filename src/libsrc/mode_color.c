#include <libconfig.h>

#include "logging.h"
#include "misc.h"

#include "internal_api.h"

#include "SDL_gfxPrimitives.h"
#include "colors.h"
#include "modeswitch.h"

#include "mode_color.h"

void vars__color(Modespec* self,config_t* cfg) {
  Colorvars* vars = self->vars = calloc(1,sizeof(Colorvars));
  
  vars->ccur = c_black;
  vars->pref_alpha = 255;
  vars->pref_saturation = 255;
  vars->pref_hue = 0;
  vars->quickselect = 0;
  
  register__global_color(&global,&vars->ccur);
}

void activate__color(Modespec* self, char* arg) {
  Colorvars* vars = self->vars;
  vars->globe_select = 0;
}

void draw__color(Modespec* self) {
  Colorvars* vars = self->vars;

  if(vars->globe_select) {
    filledCircleColor(global.screen, global.mc.x, global.mc.y,40,
		      intColor(vars->ccur));
    /* draw saturation */
    boxColor(global.screen,
	     global.mc.x + 50, 
	     global.mc.y + 40 - (80.0/255.0)*vars->pref_saturation,
	     global.mc.x + 60, 
	     global.mc.y + 40,
	     intColor(vars->ccur));
    /* draw alpha */
    boxColor(global.screen,
	     global.mc.x + 65, 
	     global.mc.y + 40 - (80.0/255.0)*vars->pref_alpha,
	     global.mc.x + 75, 
	     global.mc.y + 40,
	     intColor(vars->ccur));
  }

  vars->quickselect = (vars->quickselect < 0)? global.colorlist.size :
    vars->quickselect % (global.colorlist.size+1);

  int maincol_size = 50;
  if( vars->quickselect == 0 )
    boxColor(global.screen,0, global.screenh - 16 - maincol_size,
	     maincol_size, global.screenh - 16, intColor(c_cyan));
  else
    boxColor(global.screen,0, global.screenh - 16 - maincol_size,
	     maincol_size, global.screenh - 16, intColor(c_dkishgray));
  boxColor(global.screen,5, global.screenh - 16 - maincol_size + 5,
	     maincol_size - 5, global.screenh - 16 - 5, intColor(vars->ccur));

  char* selectedname = "current";

  boxColor(global.screen,maincol_size+1, global.screenh - 16 - 32, maincol_size+1 + 
	   (global.colorlist.size)*24 -1, global.screenh - 16, intColor(c_dkishgray));

  int i = 0;
  Colordef* _node = global.colorlist.start;
  for(; _node != NULL && i<40; _node = _node->next) {
    if(vars->quickselect == i+1){
      boxColor(global.screen,maincol_size+1 + i*24 - 1, global.screenh - 16 - 32,
	       maincol_size+1 + (i+1)*24 + 1, global.screenh - 16, intColor(c_cyan));
      selectedname = _node->name;
    }
    boxColor(global.screen,maincol_size+1+ i*24 + 2 , global.screenh - 16 - 28,
	     maincol_size+1+ (i+1)*24 - 2, global.screenh - 16, intColor(_node->c));
    i++;
  }

  stringColor(global.screen,maincol_size+global.colorlist.size*24+10,global.screenh - 36,
		selectedname,intColor(c_white));
}

int clickhandler__color(Modespec* self, SDL_MouseButtonEvent* mbev) {
  Colorvars* vars = self->vars;

  if( mbev->button == SDL_BUTTON_LEFT )
    vars->globe_select = mbev->state;
  return 0;
}

int mousehandler__color(Modespec* self, SDL_MouseMotionEvent* motion) {  
  Colorvars* vars = self->vars;

  if(vars->globe_select){
    update_color_from_mouse(&global,&vars->ccur,
			    vars->pref_saturation,
			    vars->pref_alpha);
    vars->quickselect = 0;
  }
  return 0;
}

int keyhandler__color(Modespec* self, SDL_KeyboardEvent* key) {
  Colorvars* vars = self->vars;

  int* sat;
  if( key->state == SDL_PRESSED ) {
    switch(key->keysym.sym) {
    case SDLK_RETURN:
      if(vars->quickselect != 0)
	vars->ccur = index_Colordef(&global.colorlist,vars->quickselect-1);
      pull_cur_mode(&global);
      break;
    case SDLK_a:
      sat = & vars->pref_saturation;
      *sat = (*sat + 10 < 255)? *sat +10 : 255;
      set_UImess(&global,"changed saturation to %d", vars->pref_saturation);
      break;
    case SDLK_z:
      sat = & vars->pref_saturation;
      *sat = (*sat - 10 > 0)? *sat - 10 : 0;
      set_UImess(&global,"changed saturation to %d", vars->pref_saturation);
      break;
    case SDLK_s:
      vars->pref_alpha = 
	(vars->pref_alpha < 245) ? 
	vars->pref_alpha + 10 : 255;
      set_UImess(&global,"changed alpha to %d", vars->pref_alpha);
      break;
    case SDLK_x:
      vars->pref_alpha = 
	(vars->pref_alpha > 10) ? 
	vars->pref_alpha - 10 : 0;
      set_UImess(&global,"changed alpha to %d", vars->pref_alpha);
      break;
    case SDLK_LEFT:
      vars->quickselect--;
      break;
    case SDLK_RIGHT:
      vars->quickselect++;
      break;
    default:
      return 0;
    }
  }
  return 1;
}

int call__color(Modespec* self, int argnum, char** args) {
  Colorvars* vars = self->vars;

  /* given no argument, gives current color values */
  if(argnum == 1) {
    set_UImess(&global,"rgba: %d,%d,%d,%d",
	       vars->ccur.r,vars->ccur.g,vars->ccur.b,
	       vars->ccur.a);
    return 0;
  }

  /* given a character $ enter color-define mode */
  if(args[1][0] == '$') {
    /* if contains another $ make name */
    char* name = modname(args[1]+1,'c');
    
    if(!name) {
      set_UImess(&global,"Bad name-specifier.");
      return -1;
    }

    if(find_Colordef(&global.colorlist,name)) {
      set_UImess(&global,"Color already defined.");
      return -1;
    }

    if( argnum >= 3 ) {
      Color c;
      if(parse_Color(&global.colorlist,args[2],&c)) {
        add_Colordef(&global.colorlist,name,c);
        set_UImess(&global,"Defined color %s as #%x.",name,intColor(c));
      }
      else {
        set_UImess(&global,"Unknown color-format.");
      }
    }
    else {
      /* define color as current color */
      add_Colordef(&global.colorlist,name,vars->ccur);
      set_UImess(&global,"Defined color %s as current color (#%x)",
		 name,intColor(*global.color));
    }
  }
  /* no special cases : find color with specified name */
  else {
    if(parse_Color(&global.colorlist,args[1],&vars->ccur))
      set_UImess(&global,"Set color to %s. (#%x)",args[1],intColor(vars->ccur));
    else
      set_UImess(&global,"Undefined color or unknown color-format.");
  }
  return 0;
}
