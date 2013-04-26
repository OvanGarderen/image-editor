
typedef struct brushvars Brushvars;
struct brushvars {
  int suppres;
  int active;
  Color brush_col;
  Brush brush;
};

#define brushvars_of(object) (ACC_VARS(object,Brushvars))

/* METHODS */

Modespec* init__brush(Modespec_el* context);
void activate__brush(Modespec* self, char* arg);
void draw__brush(Modespec* self);
int clickhandler__brush(Modespec* self, SDL_MouseButtonEvent* mbev);
int mousehandler__brush(Modespec* self, SDL_MouseMotionEvent* motion);
int keyhandler__brush(Modespec* self, SDL_KeyboardEvent* key);
