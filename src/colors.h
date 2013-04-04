#pragma once

#include <math.h>
#include <SDL.h>

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define  rmult 0x01000000
#define  gmult 0x00010000
#define  bmult 0x00000100
#define  amult 0x00000001
#else
#define  rmult 0x00000001
#define  gmult 0x00000100
#define  bmult 0x00010000
#define  amult 0x01000000
#endif

typedef struct{
  Uint8 r,g,b,a;
} Color;

Uint32 intColor(Color c);
Uint32 intColor_fmt(Color c, SDL_PixelFormat* fmt);
Color Colorint_fmt(Uint32 in,SDL_PixelFormat* fmt);
float hsv_wave(float input);
float hsv_r_wave(float input);
float hsv_g_wave(float input);
float hsv_b_wave(float input);
Color hsv2Color(Uint8 h, Uint8 s, Uint8 v, Uint8 a);
Color color2hsv(Color in);

Color c_red;
Color c_green;
Color c_blue;

Color c_yellow;
Color c_cyan;
Color c_fuchsia;

Color c_white;
Color c_ltgray;
Color c_gray;
Color c_dkgray;
Color c_black;
Color c_orange;

Color c_trans;

typedef struct colordef_el Colordef_el;
typedef struct colordef Colordef;
typedef struct colorlist Colorlist;

struct colorlist{
  int size;
  Colordef* start, *end;
};

struct colordef{
  char* name;
  Color c;
  Colordef* next;
};

struct colordef_el{
  char* name;
  Color c;
};

Colordef* find_Colordef(Colorlist* cl, char* name);
void add_Colordef(Colorlist* cl, char* name, Color c);
void fill_Colorlist(Colorlist* color_list, Colordef_el* color_array, int num);
void add_standard_colors(Colorlist* cl);
int parse_Color(Colorlist* collist, char* arg, Color* dest);
