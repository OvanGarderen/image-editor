#include <math.h>
#include "point.h"

Point min_Point(Point p) {
  return (Point) {-p.x,-p.y};
}

Point subs_Point(Point a, Point b) {
  return (Point) {a.x - b.x, a.y - b.y};
}

Point add_Point(Point a,Point b) {
  return (Point) {a.x + b.x, a.y + b.y};
}

Point mult_Point(float l,Point a) {
  return (Point) {a.x*l,a.y*l};
}

Point line_Point(float perc, Point a, Point b) {
  return (Point) {(1-perc) * a.x + perc* b.x, (1-perc) * a.y + perc*b.y};
}

float angle_Point(Point st, Point mid, Point end) {
  float a1 = arg_Point(subs_Point(st,mid));
  float a2 = arg_Point(subs_Point(end,mid));
  return  fmin( fabs(a1-a2), fabs(a1 - a2 - M_PI*2.0));
}

float dist_Point(Point a, Point b) {
  return norm_Point(subs_Point(a,b));
}

float norm_Point(Point p) {
  return sqrt(pow(p.x,2) + pow(p.y,2));
}

float arg_Point(Point p) {
  if(p.x == p.y && p.x == 0.0)
    return 0.0f;
  return atan2f(p.y,p.x);
}

Point turn_Point(float angle,Point a) {
  return (Point) 
    { 
      a.x * cos(angle) - a.y * sin(angle),
      a.x * sin(angle) + a.y * cos(angle)
	};
}

Line make_Line(Point a, Point b) {
  return (Line) {a,b,subs_Point(b,a),
      dist_Point(b,a),arg_Point(subs_Point(b,a))};
}

Point get_x_Line(float x, Line l) {
  if(l.dir.x == 0.0)
    return l.start;
  return add_Point(l.start,mult_Point((x-l.start.x)/l.dir.x,l.dir));
}

Point get_y_Line(float y, Line l) {
  if(l.dir.y == 0.0)
    return l.start;
  return add_Point(l.start,mult_Point((y-l.start.y)/l.dir.y,l.dir));
}

Point dilarot_Point(float rx, float ry,float alpha,Point a) {
  return (Point) {rx*cos(alpha)*a.x - ry*sin(alpha)*a.y, rx*sin(alpha)*a.x + ry*cos(alpha)*a.y};
}

Point dilarot_inv_Point(float rx, float ry,float alpha,Point a) { 
  if(rx == 0.0 || ry == 0.0)
    return (Point) {0,0};
  return (Point) {(1.0/rx)*(cos(alpha)*a.x + sin(alpha)*a.y),(1.0/ry)*(-sin(alpha)*a.x + cos(alpha)*a.y)};
}
