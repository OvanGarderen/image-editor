#pragma once

#include <SDL.h>
#include "colors.h"

int imax(int a,int b);
int imin(int a,int b);

void draw_B_spline(SDL_Surface* s, 
		   Point start, Point startfrom,
		   Point end  , Point endto, Color c);
void set_pixel_blending(SDL_Surface* surf, int x, int y, Color c);
void draw_thickline(SDL_Surface* s, Line* l, float thickness,Color c);
void draw_filledcircle(SDL_Surface* s, Point pos, float r, Color c);
void draw_test(SDL_Surface* s, Point mid);
void draw_ring(SDL_Surface* s, Point pos, float r, float rin, Color c);

float splinefunc0(float t);
float splinefunc1(float t);
float splinefunc2(float t);
float splinefunc3(float t);
