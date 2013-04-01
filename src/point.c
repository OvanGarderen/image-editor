#include <math.h>
#include "point.h"

Point min_Point(Point p){
  return (Point) {-p.x,-p.y};
}

Point subs_Point(Point a, Point b){
  return (Point) {a.x - b.x, a.y - b.y};
}

Point add_Point(Point a,Point b){
  return (Point) {a.x + b.x, a.y + b.y};
}

Point mult_Point(float l,Point a){
  return (Point) {a.x*l,a.y*l};
}

float norm_Point(Point p){
  return sqrt(pow(p.x,2) + pow(p.y,2));
}

float arg_Point(Point p){
  if(p.x == p.y && p.x == 0.0)
    return 0.0f;
  return atan2f(p.y,p.x);
}

Point turn_Point(float angle,Point a){
  return (Point) 
    { 
      a.x * cos(angle) - a.y * sin(angle),
      a.x * sin(angle) + a.y * cos(angle)
	};
}
