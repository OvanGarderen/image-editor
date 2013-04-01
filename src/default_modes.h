#include "modeswitch.h"
#include "internal_api.h"
#include "brush.h"

typedef struct brushvars Brushvars;
struct brushvars {
  bool suppres;
  bool active;
  Color brush_col;
  Brush brush;
} brushvars;

#define ACC_VARS(object,type) ((type*) (object)->vars)

Modespec* init__brush(Modespec_el* context);
void activate__brush(Modespec* self);
void draw__brush(Modespec* self);
int clickhandler__brush(Modespec* self, SDL_MouseButtonEvent* mbev);
int mousehandler__brush(Modespec* self, SDL_MouseMotionEvent* motion);
int keyhandler__brush(Modespec* self, SDL_KeyboardEvent* key);
