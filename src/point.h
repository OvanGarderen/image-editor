#pragma once

typedef struct{
  float x,y;
} Point;

Point min_Point(Point p);
Point add_Point(Point a,Point b);
Point mult_Point(float l,Point a);
float norm_Point(Point p);
float arg_Point(Point p);
Point subs_Point(Point a, Point b);
Point turn_Point(float angle,Point a);

