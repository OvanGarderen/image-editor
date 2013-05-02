/*
 * imageding.c -- drawing program
 * written by Ogier van Garderen
 * -- no warranty even for usability etc. --
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
#include "funcdefs.h"
#include "modeswitch.h"

#include "common_commands.h"
#include "internal_api.h"

#include "default_modes.h"

#include "drawing.h"
#include "inputhandler.h"

#include "dyn_loader.h"

#include "logging.h"
#include "misc.h"

int init_global(void);
void process_input(SDL_Event e, int handled);
int handle_arguments(int argc,char** argv);
void redraw_screen(void);
void exit_global(void);

Funcdef_el command_array[] = {
  {"lel",comm__lel},
  {"save",comm__save},
  {"load",comm__load},
  {"brush-size",comm__brush_change_size},
  {"mode",comm__set_UImode},
  {"message",comm__set_UImess},
  {"screen-clear",comm__clearscreen},{"clear-screen",comm__clearscreen},
  {"message-random",comm__message_random},
  {"quit",comm__quit},{"q",comm__quit},{"c",comm__quit}
};

Modespec_el mode_array[] = {
  {"file" ,NULL,init__file},
  {"brush",NULL,init__brush},
  {"color",NULL,init__color},
  {"select","/home/okke/projects/imageding/configs/select.cfg",init__dynamic},
};

/*
 * Main
 */

int main(int argc, char** argv) {
  log_init(&globallog,"logimg",0);

  srand(time(NULL));

  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_VIDEO)){
    logerror("failed to initialise SDL");
    exit(1);
  }

  loggeneral("initialising global variables");
  init_global();

  loggeneral("reading user input");
  handle_arguments(argc,argv);

  loggeneral("starting main loop");
  while(global.active) {
    int handled = false;
    Modespec* mode_cur = get_Modespec(global.modelist,global.UImode);

    if(SDL_PollEvent(&global.event)) {
      switch(global.event.type) {
      case SDL_VIDEORESIZE:
        resize_window(&global, global.event.resize.w,
                      global.event.resize.h); break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
        xsafe_method_call(mode_cur,clickhandler,&global.event.button);

        /* update mouse */
        global.mb_down = global.event.button.state;
        global.mc.x = global.event.button.x;
        global.mc.y = global.event.button.y;
        global.m = global.mc;
        global.mprev = global.m;
        break;
      case SDL_MOUSEMOTION:
        xsafe_method_call(mode_cur,mousehandler,&global.event.motion);

        /* update mouse movement */
        global.mprev = global.m;
        global.m.x = global.event.motion.x;
        global.m.y = global.event.motion.y;
        break;
      case SDL_KEYDOWN:
      case SDL_KEYUP:
        /* if keybinding isn't escaped try to acces keybinds of current mode */
        if(!(global.event.key.keysym.mod & (KMOD_CTRL|KMOD_ALT) ) && mode_cur && mode_cur->keyhandler) {
          handled = method_call(mode_cur,keyhandler,&global.event.key);
        }
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
  loggeneral("quitting main loop");
  loggeneral("cleaning up");
  exit_global();
  loggeneral("killing SDL");
  SDL_Quit();
  loggeneral("stopped succesfully");
  log_close(&globallog);
  return 0;
}

int init_global(void) {
  lognote("initialising videomode");
  global.screenw = 1000;
  global.screenh = 1000;
  logsub("size is %d x%d",global.screenw,global.screenh);
  global.screen = init_videomode(global.screenw,global.screenh);
  clear_window(&global, c_ltgray);

  lognote("constructing list of modes");
  global.modelist = create_Modelist();
  fill_Modelist(global.modelist,mode_array,arraylen(mode_array));

  lognote("initialising picture");
  init_Picture(&global.pic,600,600);
  clear_Picture(&global.pic,c_white);

  lognote("starting input buffer");
  init_Inputbuffer();

  lognote("setting various booleans");
  global.active = true;
  global.mb_down= false;
  global.suppres= false;

  lognote("constructing list of colors");
  global.colorlist.end = NULL;
  add_standard_colors(&global.colorlist);
  *global.color = c_orange;

  lognote("constructing list of callable functions");
  global.funcs.end = NULL;
  fill_Funclist(&global.funcs,command_array,arraylen(command_array));

  lognote("starting UI");
  set_UImode(&global,"brush","");
  set_UImmode(&global,"");
  set_random_UImess(&global);

  redraw_screen();
  return 0;
}

int handle_arguments(int argc,char** argv){
  if(argc>1){
    lognote("got argument, loading file %s",argv[1]);
    int succes = load_buffer(&global,argv[1]);
    if(succes < 0)
      logerror("failed to load file");
    return succes;
  } else {
    lognote("no arguments given");
    return 0;
  }
}

void exit_global(void) {
  lognote("destructing picture");
  destruct_Picture(&global.pic);
  lognote("emptying colorlist");
  clean_Colorlist(&global.colorlist);
  lognote("emptying mode list");
  clean_Modelist(global.modelist);
  lognote("popping modestack");
  destruct_Modestack(global.modestack);
  lognote("freeing screen memory");
  SDL_FreeSurface(global.screen);
}

void redraw_screen(void) {
  if(global.pic.updated)
    *global.saved = false;

  clear_window(&global,c_ltgray);
  blit_Picture(global.screen,&global.pic);

  Modespec* mode_cur = get_Modespec(global.modelist,global.UImode);
  xsafe_method_call(mode_cur,draw);

  cbqueue_draw(&global.drawqueue);
  
  /* test of the spline function */
  draw_test(global.screen,global.m);

  /* draw little UI box */
  boxColor(global.screen,0,global.screenh - 16,global.screenw,global.screenh,intColor(c_dkgray));
  update_UIstr(&global);
  stringColor(global.screen,3,global.screenh - 12,global.UIstr,intColor(c_white));

  SDL_Flip(global.screen);
}


void process_input(SDL_Event e, int handled) {
  if( e.key.state == SDL_PRESSED && !handled ) {
    if( e.key.keysym.sym == SDLK_ESCAPE ) {
      break_Inputbuffer();
      set_UImess(&global,"","");
      pull_cur_mode(&global);
    }
    else if( e.key.keysym.sym == SDLK_RETURN && *global.UI2input) {
      send_Inputbuffer();
    }
    else if( *global.UI2input ) {
      update_Inputbuffer(e.key.keysym);
    }
    else if(e.key.keysym.sym == SDLK_m) {
      push_cur_mode(&global);
      set_UImode(&global,"select","");
    }
    else if(e.key.keysym.sym == SDLK_x) {
      push_cur_mode(&global);
      set_UImode(&global,"comm","");
      Bufferspec spec = (Bufferspec) {CBtype_command,"exec","",NULL};
      start_Inputbuffer(spec);
    }
    else if(e.key.keysym.sym == SDLK_h) {
      push_cur_mode(&global);
      set_UImode(&global,"color","");
    }
    else if(e.key.keysym.sym == SDLK_b) {
      push_cur_mode(&global);
      set_UImode(&global,"brush","");
    }
    else if(e.key.keysym.sym == SDLK_s) {
      push_cur_mode(&global);
      set_UImode(&global,"file","save");
    }
    else if(e.key.keysym.sym == SDLK_l) {
      push_cur_mode(&global);
      set_UImode(&global,"file","load");
      set_UImmode(&global,"");
      Bufferspec spec = (Bufferspec) {CBtype_command,"loading","load",NULL};
      start_Inputbuffer(spec);
    }
  }
}
