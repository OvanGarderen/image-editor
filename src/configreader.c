#include <libconfig.h>
#include <misc.h>
#include <internal_api.h>

#include "configreader.h"

int cfg_varparse( config_t* cfg, const char* name, cfg_t type, void* dest) {
  if(cfg && name && dest) {
    char* spec;
    char* temps;
    with(spec,strappend(strclone("vars."), (char*) name)) {
      switch(type) {
      case cfg_int:
	config_lookup_int(cfg,spec,(int*)dest);
	break;
      case cfg_string:
	if(config_lookup_string(cfg,spec,(const char**) &temps))
	  *((char**) dest) = strclone(temps);
	break;
      case cfg_float:
	config_lookup_float(cfg,spec,(double*) dest);
	break;
      case cfg_col:
	if(config_lookup_string(cfg,spec,(const char**) &temps))
	  parse_Color(&global.colorlist,temps,(Color*)dest);
	break;
      default:
	return 1;
      }
    } 
  }
  return 0;
};
