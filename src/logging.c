#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "logging.h"

int log_init(LOG* log, char* filename, int append) {
  FILE* file;
  if(append)
    file = fopen(filename,"a");
  else
    file = fopen(filename,"w");
  if(!file)
    return 0;
  *log = file;
  writelog(*log,logtype_global,"Opening log");
  return 1;
}

int log_close(LOG* log){
  if(!log)
    return 0;
  writelog(*log,logtype_global,"Closing log");
  fclose(*log);
  return 1;
}

void writelog(LOG log,int type,const char* fmt,...){
  if(!log)
    return;
  va_list ap;
  va_start(ap,fmt);
  switch(type){
  case logtype_global:
    fprintf(log,"-- "); break;
  case logtype_error:
    fprintf(log,"!! error !!\t "); break;
  case logtype_warning:
    fprintf(log,".. warning ..\t "); break;
  case logtype_note:
    fprintf(log,"  "); break;
  case logtype_sub:
    fprintf(log,"\t"); break;
  }
  vfprintf(log,fmt,ap);
  if(type == logtype_global)
      fprintf(log," --");
  fprintf(log,"\n");
  va_end(ap);
}
