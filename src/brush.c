#include <SDL/SDL.h>
#include <math.h>
#include <SDL/SDL_gfxPrimitives.h>
#include "brush.h"
#include "point.h"
#include "picture.h"
#include "drawing.h"

void init_brush(Brush* b){
  if(b){
    b->size = 5;
    b->shape = BShape_Round;
  }
}

void brush_change_size(Brush* b, int change){
  if(b){
    b->size = (b->size + change >= 0) ? b->size + change : -1;
  }
}

void apply_brush(SDL_Surface* surf, Brush* b, Point p, Color c){
  if(b->size >= 0)
    draw_filledcircle(surf,p,(float)b->size,c);
  //    filledCircleColor(surf,(int) p.x, (int) p.y,b->size,intColor(c));
}

void stroke_brush_straight(Picture* pic, Brush* b, Point fro, Point to, Color c){
  if(b->size){
    Color cc = c;
    cc.a = c.a /2;
    
    Line l = make_Line(to,fro);
    draw_thickline(pic->surface,&l,b->size,c);
    apply_brush(pic->surface,b,to,cc);
    apply_brush(pic->surface,b,fro,cc);
  } else {
    lineColor(pic->surface,to.x,to.y,fro.x,fro.y,intColor(c));
  }
  pic->updated = 1;
}

void stroke_brush_smooth(Picture* pic, Brush* b, Point fromwhence, 
			 Point fro, Point to, Point towards, Color c){
  draw_B_spline(pic->surface,fromwhence,fro,to,fromwhence,c);
}
