
#include <SDL.h>
#include "inputhandler.h"
#include "internal_api.h"

struct inputbuffer {
#define buffer_maxlen 300
  int activated;
  int filled;
  char buffer[buffer_maxlen + 1];
  Bufferspec spec;
} input;

void init_Inputbuffer(void){
  register__global_UI2input(&global,&input.activated);
}

void start_Inputbuffer(Bufferspec spec) {
  input.spec = spec;
  input.activated = 1;
  input.filled = 0;
  input.buffer[0] = '\0';
  update_UImess(&global,"%s : %s",input.spec.name,input.buffer);
}

void update_Inputbuffer(SDL_keysym k) {
  if(input.activated && input.filled < buffer_maxlen) {
    if(k.sym == SDLK_BACKSPACE){
      input.filled = (input.filled>0)? input.filled - 1 : 0;
      input.buffer[input.filled] = '\0';
    } else if(k.sym < SDLK_UP ){
      char add = SDLK_getascii(k);
      input.buffer[input.filled] = add;
      input.filled++;
      input.buffer[input.filled] = '\0';
    }
  }
  update_UImess(&global,"%s : %s",input.spec.name,input.buffer);
}

void send_Inputbuffer(void) {
  if(input.activated) {
    input.activated = 0;
    char* temp;

    temp = malloc(input.filled + strlen(input.spec.returncomm) + 2);
    sprintf(temp,"%s %s",input.spec.returncomm,input.buffer);
    
    set_UImess(&global,"sending... %s",temp);

    switch(input.spec.callbacktype) {
    case CBtype_command:
      execute_text(&global,temp);
      break;
    case CBtype_funcp:
      if(input.spec.callback) {
	input.spec.callback(temp);
      } else {
	set_UImess(&global,"Error.");
      }
      break;
    }
    free(temp);
  }
}



void break_Inputbuffer(void){
  input.activated = 0;
}

char SDLK_getascii(SDL_keysym k){
  if((k.mod & KMOD_SHIFT)){
    char add = ' ';
    if(k.sym >= SDLK_a && k.sym <= SDLK_z)
      add = k.sym - SDLK_a + 1 + SDLK_AT;
    else{
      switch(k.sym) {
      case SDLK_1:            add = '!'; break;
      case SDLK_2:            add = '@'; break;
      case SDLK_3:            add = '#'; break;
      case SDLK_4:            add = '$'; break;
      case SDLK_5:            add = '%'; break;
      case SDLK_6:            add = '^'; break;
      case SDLK_7:            add = '&'; break;
      case SDLK_8:            add = '*'; break;
      case SDLK_9:            add = '('; break;
      case SDLK_0:            add = ')'; break;
      case SDLK_QUOTE:        add = '"'; break;
      case SDLK_SEMICOLON:    add = ':'; break;
      case SDLK_COMMA:        add = '<'; break;
      case SDLK_PERIOD:       add = '>'; break;
      case SDLK_SLASH:        add = '?'; break;
      case SDLK_LEFTBRACKET:  add = '{'; break;
      case SDLK_RIGHTBRACKET: add = '}'; break;
      case SDLK_BACKSLASH:    add = '|'; break;
      case SDLK_BACKQUOTE:    add = '~'; break;
      case SDLK_MINUS:        add = '_'; break;
      case SDLK_EQUALS:       add = '+'; break;
      default: add = k.sym; break;
      } 
    }
    return add;
  } else {
    return k.sym;
  }
}
