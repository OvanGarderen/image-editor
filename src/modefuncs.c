#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "modefuncs.h"

Funcdef* find_Funcdef(Funclist* fl, char* name){
  if(fl){
    Funcdef* _node = fl->start;
    for(; _node!=NULL; _node = _node->next){
      if(!strcmp(name,_node->name))
	return _node;
    }
  }
  return NULL;
}

void add_Funcdef(Funclist* fl, char* name, Modefunc func){
  if(fl){
    Funcdef* f = malloc(sizeof(Funcdef));
    f->name = malloc(strlen(name));
    strcpy(f->name,name);
    f->func = func;
    f->next = NULL;

    if(fl->end){
      fl->end->next = f;
      fl->end = f;
    } else {
      fl->start = f;
      fl->end = f;
    }

    fl->size++;
  }
}

int call_Modefunc(Funclist * fl, char * comm){
  int ret;
  char* cpy;
  char* name;
  char* args;
  Funcdef* f;

  if(comm && fl){
    cpy = malloc(strlen(comm));
    strcpy(cpy,comm);

    name = strtok(cpy," ");
    args = (strlen(name) < strlen(comm)) ? cpy + strlen(name) + 1 : NULL;

    f = find_Funcdef(fl,name);
    if(f){
      ret = f->func(args);
    } else {
      ret = -404; // not found
    }

    free(cpy);
  } else {
    ret = -1; // fail
  }
  return ret;
}

void fill_Funclist(Funclist* func_list, Funcdef_el* func_array, int num){
  int i=0;
  for(; i<num; i++)
    add_Funcdef(func_list, func_array[i].name, func_array[i].func);
}

int lel(char* attr) {
  printf("lel\n");
  return 666;
}
