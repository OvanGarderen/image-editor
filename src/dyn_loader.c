#include <dlfcn.h>
#include <libconfig.h>
#include <pthread.h>
#include <sys/stat.h>

#include "misc.h"
#include "logging.h"
#include "funcdefs.h"
#include "modeswitch.h"
#include "internal_api.h"

#include "dyn_loader.h"

int
primer_fillfrom_config(config_t* cfg, Pluginprimer* primer) 
{
  int steady = 1;
  config_setting_t* funclist;
  config_setting_t* methodlist;

  steady &= config_lookup_string(cfg,"name",&primer->name);
  steady &= config_lookup_string(cfg,"file",&primer->libfile);
  if(steady) {
    methodlist = config_lookup(cfg,"methods");
    if(methodlist) {
      for(primer->methodnum = 0;
	  primer->methodnum<10 && (primer->methods[primer->methodnum] = (char*)
				   config_setting_get_string_elem(methodlist,primer->methodnum));
	  primer->methodnum++);
    }

    funclist = config_lookup(cfg,"functions");
    if(funclist) {
      for(primer->funcnum = 0;
	  primer->funcnum<100 && (primer->funcs[primer->funcnum] = (char*)
				  config_setting_get_string_elem(funclist,primer->funcnum));
	  primer->funcnum++);
    }
  } else {
    logerror("config file doesn't provide plugin info");
  }
  return steady;
}

int
dyn_add_funchook(void* plugin_handle, char* name) 
{
  Funkyfunc fn = (Funkyfunc) dlsym(plugin_handle,name);

  if(fn) {
    logsub("added function %s",name);
    register__function(&global,name,fn);
    return 0;
  } else {
    logerror("could not find function %s",name);
  }
  return 1;
}

int
method_inarray(int size, char** array, char* method) 
{
  int i=0;
  for(; i<size; i++) {
    if(!strcmp(method,array[i]))
      return 1;
  }
  return 0;
}

void*
dyn_add_method(void* plugin_handle,const char* object, const char* method)
{
  char* funcname;
  void* method_handle;
  with(funcname,strappend(strappend(strclone((char*) method),"__"),(char*) object)) {
    method_handle = dlsym(plugin_handle,funcname);
    if(method_handle) {
      modelog(object,"added method %s (%p)",method,method_handle);
    } else {
      logwarning("could not find method %s",method);
    }
  }
  return method_handle;
}

int
dyn_build_Modespec(void* plugin_handle, Pluginprimer* primer,Modespec* spec) 
{
#define cond_add_method(whichmethod) if(method_inarray(primer->methodnum,primer->methods,#whichmethod)) \
    spec->whichmethod = dyn_add_method(plugin_handle,primer->name, #whichmethod );

  cond_add_method(draw);
  cond_add_method(activate);
  cond_add_method(keyhandler);
  cond_add_method(mousehandler);
  cond_add_method(clickhandler);
  cond_add_method(step);
  cond_add_method(cleanup);

  return 1;
}

int
dyn_load_plugin(const char* cfgfilename, Modespec* mode) 
{
  int greatsucces = 1;

  FILE* cfgfile = fopen(cfgfilename,"r");
  if(!cfgfile) {
    logerror("unable to open config file: %s",cfgfilename);
    return -1;
  }

  modelog(mode->name,"building from config file %s",cfgfilename);

  Pluginprimer primer;
  config_t cfg;
  config_init(&cfg);

  /* error prone process b/c of I/O
     break from it during error
  */
  do{
    /* try parsing file into config */
    if(!config_read(&cfg,cfgfile)) {
      logerror("failed parsing config file\n@%d\t%s\n\tabandoning plugin",
               config_error_line(&cfg),config_error_text(&cfg));
      greatsucces = -1;
      break;
    }

    /* try gathering info from config */
    if(!primer_fillfrom_config(&cfg,&primer)) {
      logerror("failed to find some definitions");
      greatsucces = -1;
      break;
    }

    /* try opening plugin shared object */
    void* plugin_handle = dlopen(primer.libfile,RTLD_NOW);
    if(!plugin_handle) {
      logerror("could not load shared library at %s\n\t%s",primer.libfile,dlerror());
      greatsucces = -1;
      break;
    }

    /* add functions */
    for(int i=0; i<primer.funcnum; i++)
      dyn_add_funchook(plugin_handle,primer.funcs[i]);

    /* build mode */
    dyn_build_Modespec(plugin_handle,&primer,mode);

    /* initialise variables of mode */
    void (*initvars)(Modespec* self, config_t* cfg) = dyn_add_method(plugin_handle,primer.name,"vars");
    if(initvars) {
      modelog(mode->name,"settling variables");
      initvars(mode,&cfg);
    }
  } while(0);

  config_destroy(&cfg);
  return greatsucces;
}

void* 
dyn_dispatch_loader(void* in) 
{
  Dyn_load_info* info = (Dyn_load_info*) in;

  dyn_load_plugin(info->config,info->mode);

  modelog(info->mode->name,"finished loading");
  free(info);

  pthread_exit(NULL);
}

Modespec*
init__dynamic(Modespec_el* context) 
{
  modelog(context->name,"dynamically building");
  if(context->aux) {
    Modespec* spec = init__Modespec(context);

    Dyn_load_info* info = malloc(sizeof(Dyn_load_info));
    info->config = context->aux;
    info->mode = spec;

    pthread_t* thread = malloc(sizeof(pthread_t));
    spec->loading = thread;
    pthread_create(thread,NULL,dyn_dispatch_loader,info);

    return spec;
  }
  return NULL;
}

