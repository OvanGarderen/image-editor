#include <stdlib.h>
#include <SDL.h>

#include "colors.h"
//
Color c_red     = {0xFF,0   ,0   ,0xFF};
Color c_green   = {0   ,0xFF,0   ,0xFF};
Color c_blue    = {0   ,0   ,0xFF,0xFF};

Color c_yellow  = {0xFF,0xFF,0   ,0xFF};
Color c_cyan    = {0   ,0xFF,0xFF,0xFF};
Color c_fuchsia = {0xFF,0   ,0xFF,0xFF}; 

Color c_white   = {0xFF,0xFF,0xFF,0xFF};
Color c_ltgray  = {90  ,90  ,90  , 255};
Color c_gray    = {80  ,80  ,80  , 255};
Color c_dkgray  = {60  ,60  ,60  , 255};
Color c_black   = {0   ,0   ,0   ,0xFF};
Color c_orange  = {0xFF,0xA5,0   ,0xFF};

Color c_trans   = {0xFF,0xFF,0xFF,0   };


Uint32 intColor(Color c){
  return (Uint32)( c.r *rmult +
		   c.g *gmult +
		   c.b *bmult +
		   c.a *amult);
}

Uint32 intColor_fmt(Color c, SDL_PixelFormat* fmt){
  return (Uint32)((c.r << fmt->Rshift) +
		  (c.g << fmt->Gshift) +
		  (c.b << fmt->Bshift) +
		  (c.a << fmt->Ashift));
}

Color Colorint_fmt(Uint32 in, SDL_PixelFormat* fmt){
  return (Color){
    (in & fmt->Rmask) >> fmt->Rshift, 
      (in & fmt->Gmask) >> fmt->Gshift, 
      (in & fmt->Bmask) >> fmt->Bshift, 
      (in & fmt->Amask) >> fmt->Ashift};
}

float hsv_wave(float input){
  /* gives this wave pattern: _/--\_ input is a float between 0 and 6 
     output is a float between 0 and 1
   */
  return fmin( 1, fmax( 0, (input-1) - 2* fmax( 0, input-3 )));
}


float hsv_r_wave(float input){
  return 1.0 - fmin(1.0, 
		     fmax(0.0, 
			  input - 1.0 - 2*fmax(0.0,
					       input-3.0)));
}

float hsv_g_wave(float input){
  return fmin(1.0,
	      fmax(0.0,
		   input - 2* fmax(0.0, 
				   input-2.0 )));
}

float hsv_b_wave(float input){
  return hsv_g_wave(6.0 - input);
}


Color hsv2Color(Uint8 h, Uint8 s, Uint8 v, Uint8 a){
  Color ret;
  ret.a= a;

  float fv = (float) v;
  float fs = ((float) s) / 255.0;
  
  float mr = hsv_r_wave( h/42.5 );
  float mg = hsv_g_wave( h/42.5 );
  float mb = hsv_b_wave( h/42.5 );

  ret.r = fv*( (mr - 1.0) * fs + 1.0 );
  ret.g = fv*( (mg - 1.0) * fs + 1.0 );
  ret.b = fv*( (mb - 1.0) * fs + 1.0 );

  return ret;
}

Color color2hsv(Color in){
  Color out;
  // h->r ; s->g ; v->b; a->a
  out.r = 0; // fix this
  out.b = (int) fmax( in.r, fmax( in.g, in.b ));
  out.g = - (fmin( in.r, fmax( in.g, in.b )) - out.b ) / (float) out.b;

  return out;
}

Colordef* find_Colordef(Colorlist* cl, char* name){
  if(cl){
    Colordef* _node = cl->start;
    for(; _node!=NULL; _node = _node->next)
      if(!strcmp(name,_node->name))
	return _node;
  }

  return NULL;
}

void add_Colordef(Colorlist* cl, char* name, Color c){
  if(cl){
    Colordef* cd = malloc(sizeof(Colordef));
    cd->name = malloc(strlen(name));
    strcpy(cd->name,name);
    cd->c = c;
    cd->next = NULL;

    if(cl->end){
      cl->end->next = cd;
      cl->end = cd;
    } else {
      cl->start = cd;
      cl->end = cd;
    }

    cl->size++;
  }
}

void fill_Colorlist(Colorlist* color_list, Colordef_el* color_array, int num){
  int i=0;
  for(; i<num; i++)
    add_Colordef(color_list, color_array[i].name, color_array[i].c);
}

void add_standard_colors(Colorlist* cl){
  Colordef_el color_array[] = {
    {"black",c_black},
    {"white",c_white},
    {"light-gray",c_ltgray},
    {"gray",c_gray},
    {"dark-gray",c_dkgray},
    {"orange",c_orange},
    {"red",c_red},
    {"blue",c_blue},
    {"green",c_green},
    {"yellow",c_yellow},
    {"cyan",c_cyan},
    {"fuchsia",c_fuchsia},
    {"trans",c_trans}
  };
  fill_Colorlist(cl,color_array,sizeof(color_array)/sizeof(Colordef_el));
}

int parse_Color(Colorlist* collist, char* arg, Color* dest){
  if(arg){
    if(arg[0] == '#'){ /* color is in hex-format */
      char* test;
      int rgb = strtol(arg+1,&test,16);
      if(test != arg){
	Color c = {(rgb & 0xff0000) >> 16,
		   (rgb & 0x00ff00) >> 8,
		   (rgb & 0x0000ff),
		   dest->a};
	*dest = c;
	return 1;
      }
    } else { /* color is named */
      Colordef* cd;
      if( (cd = find_Colordef(collist,arg)) ){
	Uint8 a = dest->a;
	*dest = cd->c;
	dest->a = a;
	return 1;
      }
    }
  }
  return 0;
}
