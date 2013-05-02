#pragma once

typedef struct{
  float x,y;
} Point;

typedef struct{
  Point start;
  Point end;
  Point dir;
  float length;
  float arg;
} Line;

Point min_Point(Point p);
Point add_Point(Point a,Point b);
Point mult_Point(float l,Point a);
float norm_Point(Point p);
float arg_Point(Point p);
Point line_Point(float perc, Point from, Point to);
float angle_Point(Point st, Point mid, Point end);
float dist_Point(Point a, Point b);
Point subs_Point(Point a, Point b);
Point turn_Point(float angle,Point a);

Line make_Line(Point a, Point b);
Point get_x_Line(float x, Line l);
Point get_y_Line(float y, Line l);
Point dilarot_Point(float rx,float ry,float alpha,Point a);
Point dilarot_inv_Point(float rx,float ry,float alpha,Point a);
