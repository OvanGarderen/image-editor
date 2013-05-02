
#include <stdlib.h>
#include <glib.h>
#include <pthread.h>

#include "logging.h"
#include "modeswitch.h"
#include "funcdefs.h"
#include "internal_api.h"

Modelist* create_Modelist(void) {
  Modelist* list = malloc(sizeof(Modelist));
  list->table = g_hash_table_new(g_str_hash, g_str_equal);
  return list;
}

void add_Modespec(Modelist* ml, Modespec* spec) {
  g_hash_table_insert(ml->table, spec->name, spec);
}

Modespec* get_Modespec(Modelist* ml, char* name) {
  Modespec* spec = g_hash_table_lookup(ml->table,name);
  if(spec && spec->loading){
      pthread_join(*spec->loading,NULL);
      spec->loading = NULL;
  }
  return spec;
}

void clean_Modelist(Modelist* ml) {
  g_hash_table_foreach(ml->table,_destruct_Modespec,NULL);
  g_hash_table_destroy(ml->table);
  free(ml);
}

void _destruct_Modespec(gpointer key, gpointer value, gpointer userdata) {
  if(value) {
    if(((Modespec*)value)->cleanup)
      method_call(((Modespec*) value),cleanup);
    free(((Modespec*)value)->name);
    free(value);
  }
}

int fill_Modelist(Modelist* ml, Modespec_el* array, int num) {
  int i = 0;
  for(; i<num; i++) {
    Modespec* _spec = method_call(&array[i],init);
    if(_spec) {
      add_Modespec(ml,_spec);
      logsub("init %s succesfull",array[i].name);
    } else {
      logerror("init %s failed",array[i].name);
    }
  }
  return 0;
}

Modespec* init__Modespec(Modespec_el* context) {
  Modespec* spec = malloc(sizeof(Modespec));
  if(spec) {
    /* set all uninit. vars to 0 (resolves missing methods)*/
    memset(spec,0,sizeof(Modespec));
    /* name */
    spec->name = malloc(strlen(context->name));
    spec->global = &global;
    spec->vars = NULL;
    strcpy(spec->name,context->name);
  }
  return spec;
}

int call__Modespec(Modelist* ml, char* fullcomm) {
  int ret = -1;

  int argnum = 0;
  char** argarr = NULL;

  Modespec* spec;
  char* cpy;

  if(fullcomm && ml) {
    /* copy over the command */
    cpy = malloc(strlen(fullcomm)+1);
    strcpy(cpy,fullcomm);

    /* build tokenlist from command */
    argarr = build_tokenlist(cpy,&argnum);

    if(argarr) {
      spec = get_Modespec(ml,argarr[0]);
      if(spec && spec->call) {
        ret = method_call(spec,call,argnum,argarr);
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
