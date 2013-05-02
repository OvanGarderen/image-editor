#include "cbqueue.h"

void cbqueue_add(Cbqueue* queue, void* callback, Modespec* origin) {
  Cbqueue_node* node = malloc(sizeof(Cbqueue_node));
  node->callback = callback;
  node->origin = origin;
  node->next = NULL;
  
  if(!queue->start)
    queue->start = node;
  if(queue->end)
    queue->end->next = node;
  queue->end = node;
}		   
