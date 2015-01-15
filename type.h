#ifndef TYPE_H
#define TYPE_H

typedef unsigned addr_t;

typedef enum { C_VALID=1, C_ACCESS=2, C_DIRTY=4 } state_t;
typedef struct
{
  unsigned len:3;
  unsigned ext:1;
  unsigned rw:1;
} access_t;

#endif
