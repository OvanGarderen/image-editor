#include "logging.h"
#include "misc.h"

#include "internal_api.h"

#include "SDL_gfxPrimitives.h"
#include "colors.h"
#include "modeswitch.h"

#include "mode_color.h"

Colorvars colorvars;

Modespec* init__color(Modespec_el* context) {
  Modespec* spec = init__Modespec(context);
  if(spec) {
    /* initialise vars */
    spec->vars = &colorvars;
    colorvars.ccur = c_black;
    colorvars.pref_alpha = 255;
    colorvars.pref_saturation = 255;
    colorvars.pref_hue = 0;
    colorvars.quickselect = 0;

    modelog("color","registered");
    logsub("draw/activate/click/mouse/key/call");
    spec->draw = draw__color;
    spec->activate = activate__color;
    spec->clickhandler = clickhandler__color;
    spec->mousehandler = mousehandler__color;
    spec->keyhandler = keyhandler__color;
    spec->call = call__color;

    logsub("color as global color");
    register__global_color(&global,&colorvars.ccur);
  }
  return spec;
}

void activate__color(Modespec* self) {
  colorvars.globe_select = 0;
}

void draw__color(Modespec* self) {
  if(colorvars.globe_select) {
    filledCircleColor(global.screen, global.mc.x, global.mc.y,40,
		      intColor(colorvars.ccur));
    /* draw saturation */
    boxColor(global.screen,
	     global.mc.x + 50, 
	     global.mc.y + 40 - (80.0/255.0)*colorvars.pref_saturation,
	     global.mc.x + 60, 
	     global.mc.y + 40,
	     intColor(colorvars.ccur));
    /* draw alpha */
    boxColor(global.screen,
	     global.mc.x + 65, 
	     global.mc.y + 40 - (80.0/255.0)*colorvars.pref_alpha,
	     global.mc.x + 75, 
	     global.mc.y + 40,
	     intColor(colorvars.ccur));
  }

  colorvars.quickselect = (colorvars.quickselect < 0)? global.colorlist.size :
    colorvars.quickselect % (global.colorlist.size+1);

  int maincol_size = 50;
  if( colorvars.quickselect == 0 )
    boxColor(global.screen,0, global.screenh - 16 - maincol_size,
	     maincol_size, global.screenh - 16, intColor(c_cyan));
  else
    boxColor(global.screen,0, global.screenh - 16 - maincol_size,
	     maincol_size, global.screenh - 16, intColor(c_dkishgray));
  boxColor(global.screen,5, global.screenh - 16 - maincol_size + 5,
	     maincol_size - 5, global.screenh - 16 - 5, intColor(colorvars.ccur));

  char* selectedname = "current";

  boxColor(global.screen,maincol_size+1, global.screenh - 16 - 32, maincol_size+1 + 
	   (global.colorlist.size)*24 -1, global.screenh - 16, intColor(c_dkishgray));

  int i = 0;
  Colordef* _node = global.colorlist.start;
  for(; _node != NULL && i<40; _node = _node->next) {
    if(colorvars.quickselect == i+1){
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
  if( mbev->button == SDL_BUTTON_LEFT )
    colorvars.globe_select = mbev->state;
  return 0;
}

int mousehandler__color(Modespec* self, SDL_MouseMotionEvent* motion) {  
  if(colorvars.globe_select){
    update_color_from_mouse(&colorvars.ccur,
			    colorvars.pref_saturation,
			    colorvars.pref_alpha);
    colorvars.quickselect = 0;
  }
  return 0;
}

int keyhandler__color(Modespec* self, SDL_KeyboardEvent* key) {
  int* sat;
  if( key->state == SDL_PRESSED ) {
    switch(key->keysym.sym) {
    case SDLK_RETURN:
      if(colorvars.quickselect != 0)
	colorvars.ccur = index_Colordef(&global.colorlist,colorvars.quickselect-1);
      pull_cur_mode();
      break;
    case SDLK_a:
      sat = & colorvars.pref_saturation;
      *sat = (*sat + 10 < 255)? *sat +10 : 255;
      set_UImess("changed saturation to %d", colorvars.pref_saturation);
      break;
    case SDLK_z:
      sat = & colorvars.pref_saturation;
      *sat = (*sat - 10 > 0)? *sat - 10 : 0;
      set_UImess("changed saturation to %d", colorvars.pref_saturation);
      break;
    case SDLK_s:
      colorvars.pref_alpha = 
	(colorvars.pref_alpha < 245) ? 
	colorvars.pref_alpha + 10 : 255;
      set_UImess("changed alpha to %d", colorvars.pref_alpha);
      break;
    case SDLK_x:
      colorvars.pref_alpha = 
	(colorvars.pref_alpha > 10) ? 
	colorvars.pref_alpha - 10 : 0;
      set_UImess("changed alpha to %d", colorvars.pref_alpha);
      break;
    case SDLK_LEFT:
      colorvars.quickselect--;
      break;
    case SDLK_RIGHT:
      colorvars.quickselect++;
      break;
    default:
      return 0;
    }
  }
  return 1;
}

int call__color(Modespec* self, int argnum, char** args) {
  /* given no argument, gives current color values */
  if(argnum == 1) {
    set_UImess("rgba: %d,%d,%d,%d",
	       colorvars.ccur.r,colorvars.ccur.g,colorvars.ccur.b,
	       colorvars.ccur.a);
    return 0;
  }

  /* given a character $ enter color-define mode */
  if(args[1][0] == '$') {
    /* if contains another $ make name */
    char* name = modname(args[1]+1,'c');
    
    if(!name) {
      set_UImess("Bad name-specifier.");
      return -1;
    }

    if(find_Colordef(&global.colorlist,name)) {
      set_UImess("Color already defined.");
      return -1;
    }

    if( argnum >= 3 ) {
      Color c;
      if(parse_Color(&global.colorlist,args[2],&c)) {
        add_Colordef(&global.colorlist,name,c);
        set_UImess("Defined color %s as #%x.",name,intColor(c));
      }
      else {
        set_UImess("Unknown color-format.");
      }
    }
    else {
      /* define color as current color */
      add_Colordef(&global.colorlist,name,colorvars.ccur);
      set_UImess("Defined color %s as current color (#%x)",
		 name,intColor(*global.color));
    }
  }
  /* no special cases : find color with specified name */
  else {
    if(parse_Color(&global.colorlist,args[1],&colorvars.ccur))
      set_UImess("Set color to %s. (#%x)",args[1],intColor(colorvars.ccur));
    else
      set_UImess("Undefined color or unknown color-format.");
  }
  return 0;
}
