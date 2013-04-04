#include <SDL.h>
#include "internal_api.h"

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
      set_UImess("Defined color %s as current color (#%x)",name,
		 intColor(global.color));
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

int comm__message_random(char* args){
  set_random_UImess();
  return 0;
}

int comm__clearscreen(char* args){
  Color used = c_white;
  parse_Color(&global.colorlist,args,&used);
  clear_picture(global.pic,used);
  return 0;
}

int comm__set_UImess(char* mess){
  if(mess)
    set_UImess("%s",mess);
  else
    set_UImess("");
  return 0;
}

int comm__quit(char* args){
  if(global.saved == false){
    char buff[230] = ".stashed/";
    snprintf(buff,230,".stashed/~%s",global.filename);
    save_buffer(buff);
  }
  global.active = false;
  return 0;
}
