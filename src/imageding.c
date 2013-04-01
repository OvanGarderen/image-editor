/*
 * imageding.c -- drawing program
 * written by Ogier van Garderen
 */

/*
 * Includes
 */

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_gfxPrimitives.h>

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "colors.h"
#include "point.h"
#include "picture.h"
#include "brush.h"
#include "modestack.h"
#include "modefuncs.h"
#include "modeswitch.h"

#include "internal_api.h"
#include "default_modes.h"

/*
 * Function Definitions
 */

void init_global(void);
void process_input(SDL_Event e);

/* registered UI functions */
int comm__brush_change_size(char* args);
int comm__color(char* args);
int comm__clearscreen(char* args);
void update_color_from_mouse(void);

/* UI interaction */
int comm__set_UImess(char* mess);
int comm__set_UImode(char* mode);
int save_buffer(char* filename);
void update_inputbuffer(SDL_keysym k, char* message);

void redraw_screen(void);

/*
 * Commandlist
 */

Funcdef_el command_array[] = {
  {"lel",lel},
  {"brush-change-size",comm__brush_change_size},
  {"mode-set",set_UImode},
  {"message-set",comm__set_UImess},
  {"color",comm__color},
  {"screen-clear",comm__clearscreen}
};

Modespec_el mode_array[] = {
  {"brush",NULL,init__brush}
};

/*
 * Main
 */

Uint8 templel = 0;

int main(int argc, char** argv){
  srand(time(NULL));

  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_VIDEO))
    print_SDL_error("SDL");

  printf("Initialised\n");
  init_global();

  /* Main Loop */
  while(global.active){
    Modespec* mode_cur = get_Modespec(global.modelist,global.UImode);
    if(SDL_PollEvent(&global.event)){
      switch(global.event.type){
        /* handle window resize */
      case SDL_VIDEORESIZE:
        resize_window(global.event.resize.w,
                      global.event.resize.h); break;
      case SDL_MOUSEBUTTONDOWN:
	xsafe_method_call(mode_cur,clickhandler,&global.event.button);
        global.mb_down = true;
        global.mc.x = global.event.button.x;
        global.mc.y = global.event.button.y;
        break;
      case SDL_MOUSEBUTTONUP:
	xsafe_method_call(mode_cur,clickhandler,&global.event.button);
        if(!strcmp(global.UImmode,"!"))
          set_UImmode("");
        global.mc.x = global.event.button.x;
        global.mc.y = global.event.button.y;
        global.mb_down = false; break;
      case SDL_MOUSEMOTION:
	xsafe_method_call(mode_cur,mousehandler,&global.event.motion);
        /* update mouse movement */
        global.m.x = global.event.motion.x;
        global.m.y = global.event.motion.y;
        /* draw */
	if(global.mb_down && !strcmp(global.UImode,"color")){
          update_color_from_mouse();
        }
        break;

      case SDL_KEYDOWN:
	xsafe_method_call(mode_cur,keyhandler,&global.event.key);
        process_input(global.event);break;

      case SDL_KEYUP:
        if(global.event.key.keysym.sym == SDLK_TAB){
          global.suppres = false;
          strncpy(global.UImmode,"",10);
        }
        break;

      case SDL_QUIT:
        global.active = false;
        break;
      }
    } else {
      redraw_screen();
      SDL_Delay(20);
    }
  }


  SDL_Quit();

  printf("Uninitialised\n");
  return 0;
}

void init_global(void){
  global.screen = init_videomode();
  clear_window(c_ltgray);

  global.pic = init_Picture(600,600);
  clear_picture(global.pic,c_white);

  /* miscellanious booleans */
  global.active = true;
  global.UI2input = false;
  global.mb_down= false;
  global.suppres= false;

  /* init modes */
  global.modelist = create_Modelist();
  fill_Modelist(global.modelist,mode_array,arraylen(mode_array));

  global.screenw = 1000;
  global.screenh = 1000;

  /* init colors */
  global.colorlist.end = NULL;
  add_standard_colors(&global.colorlist);
  global.color = c_orange;

  /* surprise the user with a random message */
  set_random_UImess();

  /* load functions in list */
  global.funcs.end = NULL;
  fill_Funclist(&global.funcs,command_array,arraylen(command_array));

  /* brush mode activate :3 */
  set_UImode("brush");
  set_UImmode("");

  redraw_screen();
}

int save_buffer(char* filename){
  char* fn = malloc(strlen(filename)+strlen(".bmp")+1);
  sprintf(fn,"%s.bmp",filename);

  int succes = SDL_SaveBMP(global.pic.surface,fn);
  free(fn);
  return succes;
}

int load_buffer(char* filename){
  SDL_Surface* img = IMG_Load(filename);  
  if(img){
    SDL_BlitSurface(img,NULL,global.pic.surface,NULL);
    SDL_FreeSurface(img);
    return 0;
  }
  return -1;
}

void redraw_screen(void){  
  clear_window(c_ltgray);
  SDL_BlitSurface(global.pic.surface,NULL,global.screen,NULL);
  global.pic.updated = 0;

  if(!strcmp(global.UImode,"color") && global.mb_down ){
    Color inv;
    inv.r = 255-global.color.r; inv.g = 255-global.color.g; inv.b = 255-global.color.b;
    filledCircleColor(global.screen, global.mc.x, global.mc.y,22,intColor(inv));
    filledCircleColor(global.screen, global.mc.x, global.mc.y,20,intColor(global.color));

    boxColor(global.screen, 
	     global.mc.x + 30, global.mc.y + 20 - templel , 
	     global.mc.x + 40, global.mc.y + 20,intColor(global.color));
    global.pic.updated = 1;
  } 
  if( !strcmp(global.UImode,"color")){
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

  Modespec* mode_cur = get_Modespec(global.modelist,global.UImode);
  xsafe_method_call(mode_cur,draw);

  /* draw little UI box */
  boxColor(global.screen,0,global.screenh - 16,global.screenw,global.screenh,intColor(c_dkgray));
  update_UIstr();
  stringColor(global.screen,3,global.screenh - 12,global.UIstr,intColor(c_white));


  SDL_Flip(global.screen);
}

int comm__brush_change_size(char* args){
  if(args){
    int change = atoi(args);
    brush_change_size(&global.brush,change);
  }
  set_UImess("Brush size changed to %d",global.brush.size);
  return 0;
}

int comm__color(char* args){
  /* given no argument, gives current color values */
  if(!args) {
    set_UImess("rgba: %d,%d,%d,%d",global.color.r,global.color.g,global.color.b,global.color.a);
    return 0;
  }

  /* else parse first argument */
  char* arg = strtok(args," ");

  /* catch extra spaces ==> no argument */
  if(!arg) {
    set_UImess("rgba: %d,%d,%d,%d",global.color.r,global.color.g,global.color.b,global.color.a);
    return 0;
  }

  /* given a character $ enter color-define mode */
  if(arg[0] == '$') {
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
      set_UImess("Defined color %s as current color (#%x)",name,intColor(global.color));
    }
  }
  /* no special cases : find color with specified name */
  else {
    if(parse_Color(&global.colorlist,arg,&global.color))
      set_UImess("Set color to %s.",arg);
    else
      set_UImess("Undefined color or bad color-format.");
  }
  return 0;
}

int comm__set_UImode(char* mode){
  set_UImode(mode);
  return 0;
}

int comm__clearscreen(char* args){
  Color used = c_white;
  parse_Color(&global.colorlist,args,&used);
  clear_picture(global.pic,used);
  return 0;
}

int comm__set_UImess(char* mess){
  set_UImess("%s",mess);
  return 0;
}

void update_color_from_mouse(void){
  int norm = fmin(255.0, norm_Point(add_Point( global.mc, min_Point(global.m))));
  int arg = (127.0/M_PI) *(M_PI + arg_Point(add_Point( global.mc, min_Point(global.m))));
  global.color = hsv2Color(arg,templel,norm,global.color.a);
}

void process_input(SDL_Event e){
  int handled = false;
  Modespec* cur_mode = get_Modespec(global.modelist,global.UImode);
  if( cur_mode ){
    if( cur_mode->keyhandler ){
      if( method_call(cur_mode,keyhandler,&(e.key)) ){
	handled = true;
      }
    }
  }

  if( e.key.keysym.sym == SDLK_ESCAPE ) {
    pull_cur_mode();
    global.UI2input = false;
  }
  else if( e.key.keysym.sym == SDLK_RETURN && global.UI2input){
    global.UI2input = false;
    if(!strcmp(global.UImode,"save")){
      int succes = save_buffer(global.inputbuffer);
      pull_cur_mode();
      if(succes == -1)
        set_UImess("There was a problem saving the buffer.");
      else
        set_UImess("Buffer saved.");
    } 
    else if(!strcmp(global.UImode,"load")){
      int succes = load_buffer(global.inputbuffer);
      pull_cur_mode();
      if(succes == -1)
        set_UImess("There was a problem loading the buffer.");
      else
        set_UImess("Buffer loaded from %s.",global.inputbuffer);
    }
    else if(!strcmp(global.UImode,"comm")){
      int succes = execute(global.inputbuffer);
      
      pull_cur_mode();
      if(succes == -404)
	set_UImess("Unknown command...");
    }
  }
  else if( global.UI2input ){
    char* mess = NULL;
    if(!strcmp(global.UImode,"save"))
      mess = "saving to";
    if(!strcmp(global.UImode,"load"))
      mess = "loading file";
    if(!strcmp(global.UImode,"comm"))
      mess = "input command";
    update_inputbuffer(e.key.keysym,mess);
  }
  else if(e.key.keysym.sym == SDLK_q){
    global.active = false;
  }
  else if(e.key.keysym.sym == SDLK_c){
    /*    clear_picture(global.pic,c_white); // this is fucking dangerous as fuck, don't fuck with */
  }
  else if(e.key.keysym.sym == SDLK_a){
    if(!strcmp(global.UImode,"color")){
      update_color_from_mouse();
      templel+=15;
    }
    else
      brush_change_size(&global.brush,1);
  }
  else if(e.key.keysym.sym == SDLK_z){
    if(!strcmp(global.UImode,"color")){
      update_color_from_mouse();
      templel-=15;
    }
    else
      brush_change_size(&global.brush,-1);
  }
  else if(e.key.keysym.sym == SDLK_x){
    push_cur_mode();
    memset(global.inputbuffer,0,256);
    set_UImode("comm");
    set_UImess("input command : ");
    global.UI2input = true;
  }
  else if(e.key.keysym.sym == SDLK_h){
    push_cur_mode();
    set_UImode("color");
  }
  else if(e.key.keysym.sym == SDLK_b){
    push_cur_mode();
    set_UImode("brush");
  }
  else if(e.key.keysym.sym == SDLK_TAB){
    global.suppres = true;
    set_UImmode("0");
  }
  else if(e.key.keysym.sym == SDLK_s){
    push_cur_mode();
    memset(global.inputbuffer,0,256);
    set_UImode("save");
    set_UImmode("bmp");
    set_UImess("saving to : ");
    global.UI2input = true;
  }
  else if(e.key.keysym.sym == SDLK_l){
    push_cur_mode();
    memset(global.inputbuffer,0,256);
    set_UImode("load");
    set_UImmode("bmp");
    set_UImess("loading file : ");
    global.UI2input = true;
  }
}
