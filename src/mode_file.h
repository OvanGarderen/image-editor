#pragma once

#include "modeswitch.h"
#include "internal_api.h"

typedef struct filevars Filevars;
struct filevars {
  char filename[200];
  int filetype;
  int saved;
};

#define modelog(mode,fmt,args...) writelog(globallog,-1,"mode: %s :: " fmt,mode,##args)
#define filevars_of(object) (ACC_VARS(object,Filevars))

/* METHODS */

Modespec* init__file(Modespec_el* context);
void activate__file(Modespec* self, char* arg);
int call__file(Modespec* self, int argnum, char** args);

void call__file_save(int argnum, char** args);
void call__file_load(int argnum, char** args);

/* FUNCTIONS */

int comm__save(int argnum, char** args);
int comm__load(int argnum, char** args);
