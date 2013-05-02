#pragma once

typedef struct cbqueue Cbqueue;
typedef struct cbqueue_node Cbqueue_node;

struct cbqueue {
  Cbqueue_node* start;
  Cbqueue_node* end;
};

#include "modeswitch.h"

struct cbqueue_node {
  void* callback;
  Modespec* origin;
  Cbqueue_node* next;
};

#define cbqueue_draw(queue) {Cbqueue_node* _node = (queue)->start;	\
    for(; _node ;_node = _node->next){ ((void (*)(Modespec*)) _node->callback)(_node->origin); }}

void cbqueue_add(Cbqueue* queue, void* callback, Modespec* origin);
