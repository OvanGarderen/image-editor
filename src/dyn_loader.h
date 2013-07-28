#include <libconfig.h>

typedef struct plugincontainer Plugincontainer;
typedef struct pluginprimer Pluginprimer;
typedef struct dyn_load_info Dyn_load_info;

struct pluginprimer {
  const char* name;
  const char* libfile;

  int methodnum;
  char* methods[10]; /* names of active methods */

  int funcnum;
  char* funcs[100]; /* what functions to load*/
};

/* main interface to dynamic-loader to be used 
 * in the general moduler-loader function
 * by setting the Modespec_el->context to the plugin
 * config-filename
 */
Modespec* init__dynamic(Modespec_el* self);

/* helper functions */
int primer_fillfrom_config(config_t* cfg, Pluginprimer* primer);
int method_inarray(int size, char** array, char* method); 

/* load plugin via pthread */
struct dyn_load_info {
  const char* config;
  Modespec* mode;
};
void* dyn_dispatch_loader(void* in); 

/* worker function 
 * fills mode with variables and methods as described in
 * the config-file
 */
int dyn_load_plugin(const char* cfgfilename, Modespec* mode);

/* load up the methods of a plugin */
int dyn_build_Modespec(void* plugin_handle, Pluginprimer* primer,
		       Modespec* spec);
void* dyn_add_method(void* plugin_handle,const char* object, 
		   const char* method);

/* load up functions of plugin */
int dyn_add_funchook(void* plugin_handle, char* name); 




