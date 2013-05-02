#pragma once

typedef struct funcdef_el Funcdef_el;
typedef struct funcdef Funcdef;
typedef struct funclist Funclist;
typedef int (*Funkyfunc)(int argnum, char** args); 

struct funclist{
  int size;
  Funcdef* start, *end;
};

struct funcdef{
  char* name;
  Funkyfunc func;
  Funcdef* next;
};

struct funcdef_el{
  char* name;
  Funkyfunc func;
};

Funcdef* find_Funcdef(Funclist* fl, char* name); 
void add_Funcdef(Funclist* fl, char* name, Funkyfunc func);
int call_Funcdef(Funclist * fl, char * comm);
void fill_Funclist(Funclist* fl, Funcdef_el* func_array, int num);
void destruct_Funclist(Funclist* fl);

char** escape_tokenlist(int argnum, char** args, int* newnum);
char* collapse_tokenlist(int argnum, char** args); /* allocates memory */
int count_tokens(char* str,char delim);
char** build_tokenlist(char* string, int* num);
void destruct_tokenlist(int size, char** tokenlist);
void print_tokenlist(int argnum, char** args);
