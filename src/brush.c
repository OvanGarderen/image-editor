#include <SDL/SDL.h>
#include <math.h>
#include <SDL/SDL_gfxPrimitives.h>
#include "brush.h"
#include "point.h"
#include "picture.h"

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
    filledCircleColor(surf,(int) p.x, (int) p.y,b->size,intColor(c));
}

void stroke_brush_straight(Picture* pic, Brush* b, Point fro, Point to, Color c){
  /*  Point cur = fro;
  float dist = sqrt( pow(to.x-fro.x,2) + pow(to.y-fro.y,2) );

  if(dist == 0 || b->size<0) return;

  int i=0;
  for(; i<2*dist/(1 + 1*b->size);i++){
    apply_brush(pic->surface,b,cur,c);
    cur.x += (to.x - fro.x)/dist * 0.5* (1 + b->size);
    cur.y += (to.y - fro.y)/dist * 0.5* (1 + b->size);
  }
  pic->updated = 1; old */

#define half_PI (M_PI/2)

  /* calc direction */
  Point dir  = subs_Point(fro,to);
  float dist = norm_Point(dir);

  Point start1 = add_Point(fro, mult_Point(b->size/dist, turn_Point(half_PI,dir)));
  Point start2 = add_Point(fro,mult_Point(b->size/dist, turn_Point(-half_PI,dir)));
  
  Point end1 = add_Point(start1,dir);
  Point end2 = add_Point(start2,dir);

  lineColor(pic->surface,start1.x,start1.y,end1.x,end1.y,intColor(c_black));
  lineColor(pic->surface,start2.x,start2.y,end2.x,end2.y,intColor(c_black));

  apply_brush(pic->surface,b,to,c);
  apply_brush(pic->surface,b,fro,c);
  lineColor(pic->surface,fro.x,fro.y,to.x,to.y,intColor(c_black));
  
  pic->updated = 1;
}

