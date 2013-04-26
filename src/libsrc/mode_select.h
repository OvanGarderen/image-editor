#include <SDL.h>

#include "modeswitch.h"
#include "internal_api.h"
#include "colors.h"

typedef struct selectvars Selectvars;

struct selectvars {
  int clickstatus;
  int clickpoint[2];
  Selection select;
};

/* PUBLIC FUNCS */
void shut_selection(Modespec* self);
void set_selection(Modespec* self, Rect selection);

/* METHODS */
Modespec* init__select(Modespec_el* context);
void vars__select(Modespec* vars);
void draw__select(Modespec* self);
int clickhandler__select(Modespec* self, SDL_MouseButtonEvent* mbev);
int mousehandler__select(Modespec* self, SDL_MouseMotionEvent* motion);
int keyhandler__select(Modespec* self, SDL_KeyboardEvent* key);
