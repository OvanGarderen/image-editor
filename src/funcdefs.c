#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcdefs.h"

#include "logging.h"

Funcdef* find_Funcdef(Funclist* fl, char* name) {
  if(fl) {
    Funcdef* _node = fl->start;
    for(; _node!=NULL; _node = _node->next) {
      if(!strcmp(name,_node->name))
	return _node;
    }
  }
  return NULL;
}

void add_Funcdef(Funclist* fl, char* name, Funkyfunc func) {
  if(fl) {
    Funcdef* f = malloc(sizeof(Funcdef));
    f->name = malloc(strlen(name));
    strcpy(f->name,name);
    f->func = func;
    f->next = NULL;

    logsub("added %s",name);

    if(fl->end) {
      fl->end->next = f;
      fl->end = f;
    } else {
      fl->start = f;
      fl->end = f;
    }

    fl->size++;
  }
}

int call_Funcdef(Funclist * fl, char * comm) {
  int ret = -1;

  int argnum = 0;
  char** argarr = NULL;

  Funcdef* f;
  char* cpy;

  if(comm && fl) {
    cpy = malloc(strlen(comm)+1);
    strcpy(cpy,comm);

    argarr = build_tokenlist(cpy,&argnum);
    if(argarr) {
      f = find_Funcdef(fl,argarr[0]);
      if(f) {
	ret = f->func(argnum,argarr);
      } else {
	/* !! not found */
	ret = -404; 
      }

      /* cleanup */
      destruct_tokenlist(argnum,argarr);
      free(cpy);
    } else {
      /* !! bad input */
      ret = -1; 
    }
  }
  return ret;
}

void fill_Funclist(Funclist* fl, Funcdef_el* func_array, int num) {
  int i=0;
  for(; i<num; i++)
    add_Funcdef(fl, func_array[i].name, func_array[i].func);
}

void destruct_Funclist(Funclist* fl) {
  if(fl) {
    Funcdef* _node = fl->start;
    for(; _node != NULL;) {
      Funcdef* next = _node->next;
      free(_node->name);
      free(_node);
      _node = next;
    }
    free(fl);
  }
}

/* building a tokenlist */

int count_tokens(char* str,char delim) {
  int mode = 0;
  int ret  = 0;
  while( *str ) {
    if( *str == delim ) {
      mode = 0;
    } else if( mode == 0 ) {
      mode = 1;
      ret++;
    }
    str++;
  }
  return ret;
}

char** build_tokenlist(char* string, int* num) { /* destructs string watch out */
  char** strarray = NULL;

  char* current = NULL;
  char* currcpy = NULL;

  int index = 0;
  int count = count_tokens(string,' ');

  if(count) {
    strarray = calloc(count+1,sizeof(char*));
   
    current = strtok(string," ");
    while(current) {
      currcpy = malloc(strlen(current));
      strcpy(currcpy,current);
     
      strarray[index] = currcpy;
      index++;
 
      current = strtok(NULL," ");
    }
    *num = count;
  }
  return strarray;
}

#include "misc.h"

char* collapse_tokenlist(int argnum, char** args){ /* allocates memory! */
  char* temp = strclone(args[0]);
  int i = 1;
  while(i<argnum) {
    temp = strappend(temp," ");
    temp = strappend(temp,args[i]);
    i++;
  }
  return temp;
}

char** escape_tokenlist(int argnum, char** args, int* newnum){
  /* escapes spaces escaped with a backslash */
  char** newargs = calloc(argnum,sizeof(char*));
  int i,j=0,index = 0;
  while(j<argnum) {
    i = 1;
    while(i+j<argnum
	  && args[j][strlen(args[j])-1] == '\\'
	  && args[j][strlen(args[j])-2] != '\\') {
      args[j][strlen(args[j])-1] = ' ';
      args[j] = realloc(args[j],strlen(args[j]) + strlen(args[j+i]) + 2);
      strcat(args[j],args[j+i]);
      free(args[j+i]);
      i++;
    }
    newargs[index] = args[j];
    index++;
    j += i;
  }
  newargs = realloc(newargs,index*sizeof(char*));
  *newnum = index;
  return newargs;
}

void destruct_tokenlist(int argnum, char** args) {
  int i=0;
  for(; i<argnum; i++) {
    free(args[i]);
  }
  free(args);
}

void print_tokenlist(int argnum, char** args) {
  int i=0;
  for(; i<argnum; i++) {
    printf("[%s]\n",args[i]);
  }
}
