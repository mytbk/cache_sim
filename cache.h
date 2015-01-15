#ifndef CACHE_H
#define CACHE_H

#include "type.h"

typedef struct
{
  addr_t tag;
  state_t state;
  void *data;
  unsigned lastvisit;
} block_t;

typedef struct
{
  unsigned bSet, nWay, bLine;
  block_t **blocks;
  unsigned acctime;
} cache_t;

unsigned mem_read(cache_t *c, addr_t addr, access_t op);
void mem_write(cache_t *c, addr_t addr, unsigned data, access_t op);
void invalid_entire_cache(cache_t *c);
int cache_init(cache_t *cache, unsigned s, unsigned w, unsigned b);

#endif
