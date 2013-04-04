#include "modeswitch.h"
#include "internal_api.h"
#include "brush.h"

#define ACC_VARS(object,type) ((type*) (object)->vars)

/*
 * BRUSH MODE
 */

typedef struct brushvars Brushvars;
struct brushvars {
  bool suppres;
  bool active;
  Color brush_col;
  Brush brush;
} brushvars;

Modespec* init__brush(Modespec_el* context);
void activate__brush(Modespec* self);
void draw__brush(Modespec* self);
int clickhandler__brush(Modespec* self, SDL_MouseButtonEvent* mbev);
int mousehandler__brush(Modespec* self, SDL_MouseMotionEvent* motion);
int keyhandler__brush(Modespec* self, SDL_KeyboardEvent* key);

/*
 * COLOR MODE
 */

typedef struct colorvars Colorvars;
struct colorvars {
  Color col_current;
  int globe_select;
  int pref_hue;
  int pref_value;
  int pref_saturation;
  int pref_alpha;
} colorvars;

Modespec* init__color(Modespec_el* context);
void activate__color(Modespec* self);
void draw__color(Modespec* self);
int clickhandler__color(Modespec* self, SDL_MouseButtonEvent* mbev);
int mousehandler__color(Modespec* self, SDL_MouseMotionEvent* motion);
int keyhandler__color(Modespec* self, SDL_KeyboardEvent* key);
int call__color(Modespec* self, char* args);
