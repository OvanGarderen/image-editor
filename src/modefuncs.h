#pragma once

typedef struct funcdef_el Funcdef_el;
typedef struct funcdef Funcdef;
typedef struct funclist Funclist;
typedef int (*Modefunc)(char* args); 

struct funclist{
  int size;
  Funcdef* start, *end;
};

struct funcdef{
  char* name;
  Modefunc func;
  Funcdef* next;
};

struct funcdef_el{
  char* name;
  Modefunc func;
};

Funcdef* find_Funcdef(Funclist* fl, char* name); 
void add_Funcdef(Funclist* fl, char* name, Modefunc func);
int call_Modefunc(Funclist * fl, char * comm);
void fill_Funclist(Funclist* func_list, Funcdef_el* func_array, int num);
int lel(char* attr);
