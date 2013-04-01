#include <stdlib.h>
#include <stdio.h>
#include "modestack.h"

Modestack* push_Modestack(Modestack* ms, char* mode, char* mmode){
  Modestack* new = malloc(sizeof(Modestack));
  if(new){
    snprintf(new->mode,MODE_LEN,"%s",mode);
    snprintf(new->mmode,MMODE_LEN,"%s",mmode);
    if(ms)
      new->pos = ms->pos+1;
    else
      new->pos = 0;
    new->next = ms;
  } 
  return new;
}

Modestack* pop_Modestack(Modestack* ms){
  if(ms){
    Modestack* next = ms->next;
    free(ms);
    return next;
  }
  return NULL;
}
