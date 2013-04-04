#include <SDL_gfxPrimitives.h>
#include "colors.h"
#include "point.h"
#include "drawing.h"

void set_pixel_blending(SDL_Surface* surf, int x, int y, Color c){
  if( x >= 0 && y >= 0 && x < surf->w && y < surf->h ){
    Uint32* pix = ((Uint32*) surf->pixels + x + (surf->pitch/4) * y);
    if(c.a <254){
      Color p = Colorint_fmt(*pix,surf->format);
      
      p.r = ((255 - c.a) * p.r + c.a * c.r)/255;
      p.g = ((255 - c.a) * p.g + c.a * c.g)/255;
      p.b = ((255 - c.a) * p.b + c.a * c.b)/255;
      p.a = imax(p.a,c.a);
      
      *pix = intColor_fmt(p,surf->format);
    } else {
      *pix = intColor_fmt(c,surf->format);
    }
  }
}

int imin(int a,int b){
  return (a<b)? a : b;
}

int imax(int a,int b){
  return (a>b)? a : b;
}

float splinefunc0(float t){
  return -pow(t,3)+3*pow(t,2)-3*t + 1;
}
float splinefunc1(float t){
  return 3*pow(t,3)-6*pow(t,2)+3*t;
}
float splinefunc2(float t){
  return -3*pow(t,3)+3*pow(t,2);
}
float splinefunc3(float t){
  return pow(t,3);
}

void draw_B_spline(SDL_Surface* s, 
		      Point start, Point startfrom,
		      Point end  , Point endto, Color c){
  Point p1 = subs_Point(mult_Point(2.0,start),startfrom);
  Point p2;
  if(endto.x == startfrom.x && endto.y == startfrom.y)
    p2 = p1;
  else
    p2 = subs_Point(mult_Point(2.0,end),endto);
  
  float t = 0;
  Point prev = start;
  for(; t<=1.0; t+=0.1){
    Point cur = add_Point(
			  add_Point(mult_Point(splinefunc0(t),start),
				    mult_Point(splinefunc1(t),p1)),
			  add_Point(
				    mult_Point(splinefunc2(t),p2),
				    mult_Point(splinefunc3(t),end)));
    lineColor(s,prev.x,prev.y,cur.x,cur.y,intColor(c));
    prev = cur;
  }
}

void draw_thickline(SDL_Surface* s, Line* l, float thickness,Color c) {
  int xmin =  imin(l->end.x,l->start.x) - thickness;
  int ymin =  imin(l->end.y,l->start.y) - thickness;
  int xmax = imax(l->end.x,l->start.x) + thickness;
  int ymax = imax(l->end.y,l->start.y) + thickness;

  Color cc = c;
  cc.a = c.a /2;

  int x,y;
  for(x=xmin; x < xmax; x++){
    for(y=ymin; y < ymax; y++){
      Point inv = dilarot_inv_Point(l->length,thickness,l->arg,
				    subs_Point((Point){x,y},l->start));

      if(inv.x > 0 && inv.y < 1 && inv.y > -1 && inv.x < 1){
	set_pixel_blending(s,x,y,c);
      } else if( inv.x >= -0.1 && inv.y <= 1.1 &&
		 inv.x <= 1.1 && inv.y >=-0.9){
	set_pixel_blending(s,x,y,cc);
      }
    }
  }
}

void draw_filledcircle(SDL_Surface* s, Point pos, float r, Color c) {
  int xmin = pos.x - r;
  int ymin = pos.y - r;
  int xmax = pos.x + r + 1;
  int ymax = pos.y + r + 1;
  
  Color cc = c;
  cc.a = c.a /2;
  
  int x,y;
  for(x=xmin; x < xmax; x++){
    for(y=ymin; y < ymax; y++){
      if(sqrt(pow(x-pos.x,2) + pow(y-pos.y,2)) <= r)
	set_pixel_blending(s,x,y,c);
    }
  }
}
