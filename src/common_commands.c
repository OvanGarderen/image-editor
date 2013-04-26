#include <SDL.h>
#include "internal_api.h"
#include "misc.h"
#include "logging.h"

int comm__lel(int argnum, char** args) {
  printf("lelxD %d:%s\n",argnum,args[1]);
  return 666;
}

int comm__brush_change_size(int argnum, char** args) {
  if(global.brush) {
    if(argnum > 1) {
      int newsize = atoi(args[1]);
      global.brush->size = newsize;
      set_UImess(&global,"Brush size changed to %d",global.brush->size);
    } else{
      set_UImess(&global,"Brush size is %d",global.brush->size);
    }
    return 0;
  }
  set_UImess(&global,"No brush set.");
  return -20;
}

int comm__set_UImode(int argnum, char** args) {
  if(argnum > 1){
    set_UImode(&global,args[1]);
    push_cur_mode(&global);
  }
  else
    set_UImode(&global,"");
  return 0;
}

int comm__message_random(int argnum, char** args) {
  set_random_UImess(&global);
  return 0;
}

int comm__clearscreen(int argnum, char** args) {
  Color used = c_white;
  if(argnum > 1)
    parse_Color(&global.colorlist,args[1],&used);
  clear_Picture(&global.pic,used);
  return 0;
}

int comm__set_UImess(int argnum, char** args) {
  if(argnum > 1)
    set_UImess(&global,"%s",args[1]);
  else
    set_UImess(&global,"");
  return 0;
}

int comm__quit(int argnum, char** args) {
  int nostash = 0;

  if(argnum > 1 && !strcmp(args[1],"nostash")) {
    nostash = 1;
  }
  
  if(*global.saved == 0 && nostash == 0) {
    char buff[230] = ".stashed/";
    snprintf(buff,230,".stashed/~%s",global.filename);
    save_buffer(&global,buff);
  }
  global.active = 0;
  return 0;
}
