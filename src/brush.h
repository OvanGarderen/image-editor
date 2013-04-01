#pragma once

typedef enum {BShape_Round} BrushShape;

#include "brush.h"
#include "point.h"
#include "colors.h"
#include "picture.h"

typedef struct brush{
  int size;
  BrushShape shape;
} Brush;

#define brush_std ((Brush) {10,BShape_Round})

void init_brush(Brush* b);
void apply_brush(SDL_Surface* surf, Brush* b, Point p, Color c);
void stroke_brush_straight(Picture* pic, Brush* b, Point fro, Point to, Color c);
void brush_change_size(Brush* b, int change);

