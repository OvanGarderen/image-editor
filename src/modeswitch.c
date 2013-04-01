#include <stdlib.h>
#include <glib.h>

#include "modeswitch.h"

Modelist* create_Modelist(void) {
  Modelist* list = malloc(sizeof(Modelist));
  list->table = g_hash_table_new(g_str_hash, g_str_equal);
  return list;
}

void add_Modespec(Modelist* ml, Modespec* spec) {
  printf("\tadding mode %s to list\n",spec->name);
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
  printf("filling list at %p with %d values\n",ml,num);
  for(; i<num; i++){
      Modespec* _spec = method_call(&array[i],init);
      if(_spec){
	printf("\tadding mode %s to list\n",array[i].name);
	add_Modespec(ml,_spec);
      }
      else{
	fprintf(stderr,"Failed to init %s",array[i].name);
      }
  }
}
