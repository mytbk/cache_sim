#include "type.h"
#include <stdlib.h>

#define NPAGES 1024
#define PGSIZE (1<<22)
#define PGSHIFT 22

static void* pagetable[NPAGES];

void* addr_translate(addr_t addr)
{
  size_t idx = addr>>PGSHIFT;
  if (pagetable[idx]==NULL) {
    pagetable[idx] = malloc(PGSIZE);
    if (pagetable[idx]==NULL) {
      return NULL;
    }
  }
  return pagetable[idx]+addr%PGSIZE;
}
