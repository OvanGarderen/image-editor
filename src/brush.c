#include <SDL/SDL.h>
#include <math.h>
#include <SDL/SDL_gfxPrimitives.h>
#include "brush.h"
#include "point.h"
#include "picture.h"
#include "drawing.h"

void init_brush(Brush* b) {
  if(b) {
    b->size = 5;
    b->shape = BShape_Round;
  }
}

void brush_change_size(Brush* b, int change) {
  if(b) {
    b->size = (b->size + change >= 0) ? b->size + change : -1;
  }
}

void apply_brush(SDL_Surface* surf, Brush* b, Point p, Color c) {
  if(b->size >= 0)
    draw_filledcircle(surf,p,(float) b->size,c);
}

void stroke_brush_straight(Picture* pic, Brush* b, Point fro, Point to, Color c) {
  SDL_Surface* target = get_drawingsurf_Picture(pic);
  if(b->size) {
    Line l = make_Line(fro,to);

    draw_thickline(target,&l,b->size,c);
    apply_brush(target,b,fro,c);
    apply_brush(target,b,to,c);

    /*    float t = 0.0;
    Point dir = subs_Point(to,fro);
    float dist = dist_Point(to,fro);
    while(t<=1.01){
      Point cur = add_Point(fro,mult_Point(t,dir));
      apply_brush(target,b,cur,c);
      t+= 1.0/dist;
      }*/
  } else {
    lineColor(target,to.x,to.y,fro.x,fro.y,intColor(c));
  }
  pic->updated = 1;
}

void stroke_brush_smooth(Picture* pic, Brush* b, Point fromwhence, 
			 Point fro, Point to, Point towards, Color c) {
  SDL_Surface* target = get_drawingsurf_Picture(pic);
  draw_B_spline(target,fro,fromwhence,
		to,towards,c);
}
