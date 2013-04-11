#pragma once

#define MESS_LEN 200
#define MODE_LEN 30
#define MMODE_LEN 10

typedef struct modestack Modestack;

struct modestack {
  char mode [ MODE_LEN];
  char mmode[MMODE_LEN];
  int pos;
  Modestack* next;
};

Modestack* push_Modestack(Modestack* ms, char* mode, char* mmode);
Modestack* pop_Modestack(Modestack* ms);

void destruct_Modestack(Modestack* ms);
