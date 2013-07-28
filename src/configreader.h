#include <libconfig.h>

typedef enum { cfg_int, cfg_string, cfg_float, cfg_col } cfg_t;

int cfg_varparse( config_t* cfg, const char* name, cfg_t type, void* dest);
