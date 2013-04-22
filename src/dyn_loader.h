typedef struct plugincontainer Plugincontainer;
typedef struct pluginprimer Pluginprimer;

struct pluginprimer {
  const char* name;
  const char* libfile;

  int methodnum;
  char* methods[10]; /* names of active methods */

  int funcnum;
  char* funcs[100]; /* what functions to load*/
};

Modespec* init__dynamic(Modespec_el* self);
int dyn_load_plugin(const char* cfgfilename, Modespec* mode);
