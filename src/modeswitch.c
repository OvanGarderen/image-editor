#include <stdlib.h>
#include <glib.h>

#include "modeswitch.h"

Modelist* create_Modelist(void) {
  Modelist* list = malloc(sizeof(Modelist));
  list->table = g_hash_table_new(g_str_hash, g_str_equal);
  return list;
}

void add_Modespec(Modelist* ml, Modespec* spec) {
  g_hash_table_insert(ml->table, spec->name, spec);
}

Modespec* get_Modespec(Modelist* ml, char* name) {
  return g_hash_table_lookup(ml->table,name);
}

void clean_Modelist(Modelist* ml){
  g_hash_table_foreach(ml->table,_destruct_Modespec,NULL); 
  g_hash_table_destroy(ml->table);
  free(ml);
}

void _destruct_Modespec(gpointer key, gpointer value, gpointer userdata){
  if(value){
    method_call(((Modespec*) value),cleanup);
    free(value);
  }
}

int fill_Modelist(Modelist* ml, Modespec_el* array, int num){
  int i = 0;
  for(; i<num; i++){
      Modespec* _spec = method_call(&array[i],init);
      if(_spec){
	add_Modespec(ml,_spec);
      }
  }
  return 0;
}

Modespec* init__Modespec(Modespec_el* context){
  Modespec* spec = malloc(sizeof(Modespec));
  if(spec){
    /* set all uninit. vars to 0 (resolves missing methods)*/
    memset(spec,0,sizeof(Modespec));
    /* name */
    spec->name = malloc(strlen(context->name));
    strcpy(spec->name,context->name);
  }
  return spec;
}

int call__Modespec(Modelist* ml, char* fullcomm) {
  int ret;
  char* cpy;
  char* name;
  char* args;
  Modespec* spec;

  if(fullcomm && ml) {
    cpy = malloc(strlen(fullcomm));
    strcpy(cpy,fullcomm);

    name = strtok(cpy," ");
    args = (strlen(name) < strlen(fullcomm)) ? cpy + strlen(name) + 1 : NULL;

    spec = get_Modespec(ml,name);
    if(spec){
      if(spec->call)
	ret = method_call(spec,call,args);
      else
	ret = -404;
    } else {
      ret = -404; // not found
    }

    free(cpy);
  } else {
    ret = -1; // fail
  }
  return ret;
}
