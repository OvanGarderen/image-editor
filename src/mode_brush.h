#define modelog(mode,fmt,args...) writelog(globallog,-1,"mode: %s :: " fmt,mode,##args)

typedef struct brushvars Brushvars;
struct brushvars {
  bool suppres;
  bool active;
  Color brush_col;
  Brush brush;
};

#define brushvars_of(object) (ACC_VARS(object,Brushvars))

Modespec* init__brush(Modespec_el* context);
void activate__brush(Modespec* self);
void draw__brush(Modespec* self);
int clickhandler__brush(Modespec* self, SDL_MouseButtonEvent* mbev);
int mousehandler__brush(Modespec* self, SDL_MouseMotionEvent* motion);
int keyhandler__brush(Modespec* self, SDL_KeyboardEvent* key);
