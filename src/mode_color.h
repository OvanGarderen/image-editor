#include "modeswitch.h"

typedef struct colorvars Colorvars;
struct colorvars {
  Color ccur;
  int quickselect;
  int globe_select;
  int pref_hue;
  int pref_value;
  int pref_saturation;
  int pref_alpha;
};

#define colorvars_of(object) (ACC_VARS(object,Colorvars))
#define modelog(mode,fmt,args...) writelog(globallog,-1,"mode: %s :: " fmt,mode,##args)

Modespec* init__color(Modespec_el* context);
void activate__color(Modespec* self, char* arg);
void draw__color(Modespec* self);
int clickhandler__color(Modespec* self, SDL_MouseButtonEvent* mbev);
int mousehandler__color(Modespec* self, SDL_MouseMotionEvent* motion);
int keyhandler__color(Modespec* self, SDL_KeyboardEvent* key);
int call__color(Modespec* self, int argnum, char** args);

