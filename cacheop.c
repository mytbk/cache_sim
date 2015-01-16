#include "cache.h"
#include "mem.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

static unsigned
get8(char *data, int ext)
{
  int r=*data;
  if (ext) {
    r = (r<<24)>>24;
    return r;
  } else {
    r = r&0xff;
    return r;
  }
}

static unsigned
get16(short *data, int ext)
{
  int r = *data;
  if (ext) {
    r = (r<<16)>>16;
    return r;
  } else {
    return (r&0xffff);
  }
}

/* cache_data_ptr: get the cache data pointer of addr
   and save the block to blk_save
*/

static void*
cache_data_ptr(cache_t *c, addr_t addr, block_t **blk_save)
{
  unsigned offset = addr&((1<<c->bLine)-1);
  addr >>= c->bLine;
  unsigned setidx = addr&((1<<c->bSet)-1);
  addr_t tag = addr>>c->bSet;
  block_t *blk=NULL;
  for (int i=0; i<c->nWay; i++) {
    if ((c->blocks[setidx][i].state&C_VALID)
        && (c->blocks[setidx][i].tag==tag)) {
      blk = &c->blocks[setidx][i];
    }
  }
  if (blk_save) {
    *blk_save = blk;
  }
  if (blk==NULL) {
    return NULL;
  }
  return blk->data+offset;
}

static void
write_back(cache_t *ch, unsigned idx, int way)
{
  block_t *blk = &ch->blocks[idx][way];
  addr_t addr = (blk->tag<<(ch->bSet+ch->bLine))|(idx<<ch->bLine);
  printf("writing back to main memory, addr=0x%x.\n", addr);
  void *mem = addr_translate(addr);
  memcpy(mem, blk->data, 1<<ch->bLine);
}

/* linefill: find a cache line to place the bytes at addr

   first, checkout for an empty line, if not, use LRU
   then, write back the line if valid
   at last, read the line to cache
*/

static void
linefill(cache_t *ch, addr_t addr)
{
  size_t idx = (addr>>ch->bLine)&((1<<ch->bSet)-1);
  block_t *myset = ch->blocks[idx];
  unsigned lru,ltime=ch->acctime;
  for (int i=0; i<ch->nWay; i++) {
    if ((myset[i].state&C_VALID)==0) {
      lru = i;
      break;
    } else if (myset[i].lastvisit<ltime) {
      lru = i;
    }
  }
  // fill this line
  if ((myset[lru].state&C_VALID) && (myset[lru].state&C_DIRTY)) {
    write_back(ch, idx, lru);
  }
  myset[lru].state = C_VALID;
  myset[lru].tag = addr>>(ch->bSet+ch->bLine);
  void *memaddr = addr_translate((addr>>ch->bLine)<<ch->bLine);
  memcpy(myset[lru].data, memaddr, 1<<ch->bLine);
}

/* mem_read: first read cache memory, if hit
   return the word read from cache, otherwise
   read main memory
*/

unsigned
mem_read(cache_t *c, addr_t addr, access_t op)
{
  assert(op.rw==0);
  assert(addr%op.len==0);
  block_t *blk;
  void *ptr = cache_data_ptr(c, addr, &blk);
  printf("trying to read 0x%x from cache...", addr);
  if (ptr==NULL) {
    printf("MISS\n");
    linefill(c, addr);
    ptr = cache_data_ptr(c, addr, &blk);
  } else {
    printf("HIT\n");
  }
  c->acctime++;
  blk->lastvisit = c->acctime;
  blk->state |= C_ACCESS;
  switch (op.len) {
  case 1:
    return get8(ptr, op.ext);
    break;
  case 2:
    return get16(ptr, op.ext);
    break;
  case 4:
    return *(unsigned*)ptr;
    break;
  default:
    assert(0);
  }
  return 0;
}

// mem_write: write data to addr

void
mem_write(cache_t *c, addr_t addr, unsigned data, access_t op)
{
  assert(op.rw==1);
  assert(addr%op.len==0);
  block_t *blk;
  void *ptr = cache_data_ptr(c, addr, &blk);
  printf("trying to write to cache, memory address=0x%x...", addr);
  if (ptr==NULL) {
    printf("MISS\n");
    linefill(c, addr);
    ptr = cache_data_ptr(c, addr, &blk);
  } else {
    printf("HIT\n");
  }
  c->acctime++;
  blk->lastvisit = c->acctime;
  blk->state |= C_ACCESS|C_DIRTY;
  switch (op.len) {
  case 1:
    *(char*)ptr = data;
    break;
  case 2:
    *(short*)ptr = data;
    break;
  case 4:
    *(int*)ptr = data;
    break;
  default:
    assert(0);
  }
}

void
invalid_entire_cache(cache_t *c)
{
  int nSets = 1<<c->bSet;
  for (int i=0; i<nSets; i++) {
    for (int j=0; j<c->nWay; j++) {
      if (c->blocks[i][j].state&C_VALID) {
        if (c->blocks[i][j].state&C_DIRTY) {
          write_back(c, i, j);
        }
        c->blocks[i][j].state = 0;
      }
    }
  }
}
