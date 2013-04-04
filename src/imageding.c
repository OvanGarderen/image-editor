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

#include "common_commands.h"
#include "internal_api.h"
#include "default_modes.h"

/*
 * Function Definitions
 */

void init_global(void);
void process_input(SDL_Event e, int handled);

/* registered UI functions */
void redraw_screen(void);

/*
 * Commandlist
 */

Funcdef_el command_array[] = {
  {"lel",lel},
  {"brush-change-size",comm__brush_change_size},
  {"mode-set",set_UImode},
  {"message-set",comm__set_UImess},
  {"screen-clear",comm__clearscreen},
  {"message-random",comm__message_random},
  {"quit",comm__quit},
  {"q",comm__quit},
};

Modespec_el mode_array[] = {
  {"brush",NULL,init__brush},
  {"color",NULL,init__color}
};

/*
 * Main
 */

Uint8 templel = 0;

int main(int argc, char** argv){
  srand(time(NULL));

  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_VIDEO))
    print_SDL_error("SDL");

  init_global();

  printf("screen pixformat:");
  print_SurfaceFormat(global.screen);

  /* Main Loop */
  while(global.active){
    int handled = false;
    Modespec* mode_cur = get_Modespec(global.modelist,global.UImode);

    if(SDL_PollEvent(&global.event)){
      switch(global.event.type){
        /* handle window resize */
      case SDL_VIDEORESIZE:
        resize_window(global.event.resize.w,
                      global.event.resize.h); break;
        /* handle mouse clicks */
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
        xsafe_method_call(mode_cur,clickhandler,&global.event.button);
        /* update mouse state */
        global.mb_down = (global.event.button.state == SDL_PRESSED);
        /* update mouse position */
        global.mc.x = global.event.button.x;
        global.mc.y = global.event.button.y;
        break;
        /* handle mouse motion */
      case SDL_MOUSEMOTION:
        xsafe_method_call(mode_cur,mousehandler,&global.event.motion);
        /* update mouse movement */
	global.mprev = global.m;
        global.m.x = global.event.motion.x;
        global.m.y = global.event.motion.y;
        break;
        /* handle key presses */
      case SDL_KEYDOWN:
      case SDL_KEYUP:
	if(!(global.event.key.keysym.mod & KMOD_CTRL)){ 
	  if(mode_cur){if(mode_cur->keyhandler){
	      handled = method_call(mode_cur,keyhandler,&global.event.key);
	    }}}
        process_input(global.event,handled);
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
  global.saved = true;

  /* init modes */
  global.modelist = create_Modelist();
  fill_Modelist(global.modelist,mode_array,arraylen(mode_array));

  global.screenw = 1000;
  global.screenh = 1000;

  /* init colors */
  global.colorlist.end = NULL;
  add_standard_colors(&global.colorlist);
  global.color = c_orange;

  /* load functions in list */
  global.funcs.end = NULL;
  fill_Funclist(&global.funcs,command_array,arraylen(command_array));

  /* brush mode activate :3 */
  set_UImode("brush");
  set_UImmode("");
  set_filename("\0");
  set_random_UImess();

  redraw_screen();
}

void redraw_screen(void){
  if(global.pic.updated)
    global.saved = false;

  clear_window(c_ltgray);
  SDL_BlitSurface(global.pic.surface,NULL,global.screen,NULL);
  global.pic.updated = 0;

  Modespec* mode_cur = get_Modespec(global.modelist,global.UImode);
  xsafe_method_call(mode_cur,draw);

  /* draw little UI box */
  boxColor(global.screen,0,global.screenh - 16,global.screenw,global.screenh,intColor(c_dkgray));
  update_UIstr();
  stringColor(global.screen,3,global.screenh - 12,global.UIstr,intColor(c_white));

  SDL_Flip(global.screen);
}


void process_input(SDL_Event e, int handled){
  if( e.key.state == SDL_PRESSED && !handled ) {
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

	if(succes == -404){
	  pull_cur_mode();
	  Modespec* cur_mode = get_Modespec(global.modelist,global.UImode);
	  if(cur_mode && cur_mode->call){
	    char* temp = malloc(strlen(global.inputbuffer));
	    strcpy(temp,global.inputbuffer);
	    succes = method_call(cur_mode,call,temp);
	    free(temp);
	  }
	  push_cur_mode();
	}
        if(succes == -404)
	  succes = call__Modespec(global.modelist,global.inputbuffer);
	if(succes == -404)
          set_UImess("Unknown command.");
	else if(succes < -1)
	  set_UImess("...");
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
    else if(e.key.keysym.sym == SDLK_c){
      /*    clear_picture(global.pic,c_white); // this is fucking dangerous as fuck, don't fuck with */
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
}
