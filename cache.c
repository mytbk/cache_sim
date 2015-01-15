#include "cache.h"
#include <stdlib.h>

/* block_init:
   initialize a block with block size n
*/
static void
block_init(block_t *blk, unsigned n)
{
  blk->state = 0;
  blk->data = malloc(n);
}

/* cache_init:
   initialize a cache with s sets, w ways, 2^b block size
*/

int
cache_init(cache_t *cache, unsigned s, unsigned w, unsigned b)
{
  cache->bSet = 0;
  int i;
  for (i=1; i<s; i<<=1) {
    cache->bSet++;
  }
  if (i>s) {
    return -1;
  }
  cache->nWay = w;
  cache->bLine = b;
  cache->acctime = 0;

  cache->blocks = (block_t**)malloc(sizeof(block_t*)*s);
  for (int i=0; i<s; i++) {
    cache->blocks[i] = (block_t*)malloc(sizeof(block_t)*w);
    for (int j=0; j<w; j++) {
      block_init(&cache->blocks[i][j], (1<<b));
    }
  }
  return 0;
}
