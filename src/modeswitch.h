#pragma once

#include <stdlib.h>
#include <SDL.h>
#include <glib.h>
#include <pthread.h>

typedef struct modespec Modespec;
typedef struct modelist Modelist;
typedef struct modespec_el Modespec_el;

#include "globalstruct.h"

struct modelist {
  GHashTable *table;
};

struct modespec {
  char* name;
  void* vars;
  Globalstruct* global;
  pthread_t* loading;
  int (*call)(Modespec* self, int numargs, char** args);
  void (*draw)(Modespec* self);
  void (*activate)(Modespec* self, char* arg);
  int (*keyhandler)(Modespec* self, SDL_KeyboardEvent* key);
  int (*mousehandler)(Modespec* self, SDL_MouseMotionEvent* motion);
  int (*clickhandler)(Modespec* self, SDL_MouseButtonEvent* button);
  void (*step)(Modespec* self);
  void (*cleanup)(Modespec* self);
};

struct modespec_el {
  char* name;
  char* aux;
  Modespec* (*init)(Modespec_el* self);
};

#define method_call(object,method,...) ((object)->method((object) , ##__VA_ARGS__))
#define xsafe_method_call(object,method,...) if((object) && (object)->method){method_call(object,method ,##__VA_ARGS__);}

Modelist* create_Modelist(void);
void add_Modespec(Modelist* ml, Modespec* spec);
Modespec* get_Modespec(Modelist* ml, char* name);
void clean_Modelist(Modelist* ml);
void _destruct_Modespec(gpointer key, gpointer value, gpointer userdata);

int fill_Modelist(Modelist* ml, Modespec_el* array, int num);
Modespec* init__Modespec(Modespec_el* context);
int call__Modespec(Modelist* ml, char* fullcomm);

#define modelog(mode,fmt,args...) writelog(globallog,-1,"[%s] " fmt, mode , ##args)
