#pragma once

#include <stdio.h>

#define closelog(A) log_close(&(A))
#define uselog(A) log_init(&A, "log/" #A ".log", 1)
#define usedump(A) log_init(&A, "log/" #A ".log", 0)

typedef enum {logtype_global,logtype_error,logtype_warning,logtype_note,logtype_sub} logtype;
typedef FILE* LOG;

int log_init(LOG* log, char* filename, int append);
int log_close(LOG* log);
void writelog(LOG log,int type,const char* fmt,...);

#define logliteral(log,lit) writelog(log,logtype_sub,#lit); lit

LOG globallog;

#define logerror(fmt,args...) writelog(globallog,logtype_error,fmt , ##args)
#define logwarning(fmt,args...) writelog(globallog,logtype_warning,fmt , ##args)
#define lognote(fmt,args...) writelog(globallog,logtype_note,fmt , ##args)
#define logsub(fmt,args...)  writelog(globallog,logtype_sub,fmt , ##args)
#define loggeneral(fmt,args...) writelog(globallog,logtype_global,fmt , ##args)
