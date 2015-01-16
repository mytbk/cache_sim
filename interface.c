#include "cache.h"
#include <stdio.h>

cache_t my_cache;

void sim_cache_init()
{
  cache_init(&my_cache, 128, 2, 5);
}

void
read_data(addr_t addr, int len, int ext)
{
  access_t ac;
  unsigned data;
  printf("reading data from address 0x%x, len=%d\n", addr, len);
  ac.rw = 0;
  ac.len = len;
  ac.ext = ext;
  if (addr%len!=0) {
    if (len==2) {
      unsigned lb,hb;
      ac.len = 1;
      lb = mem_read(&my_cache, addr, ac);
      printf("byte 0x%x fetched.\n", lb&0xff);
      hb = mem_read(&my_cache, addr+1, ac);
      printf("byte 0x%x fetched.\n", hb&0xff);
      int r = (hb<<8)|lb;
      if (ext) {
        r = (r<<16)>>16;
      }
      data = r;
    } else if (len==4) {
      if (addr%2==0) {
        unsigned lb,hb;
        ac.len = 2;
        lb = mem_read(&my_cache, addr, ac);
        hb = mem_read(&my_cache, addr+2, ac);
        data = (hb<<16)|lb;
      } else {
        unsigned lb,mb,hb;
        ac.len = 1;
        lb = mem_read(&my_cache, addr, ac);
        hb = mem_read(&my_cache, addr+3, ac);
        ac.len = 2;
        mb = mem_read(&my_cache, addr+1, ac);
        data = (hb<<24)|(mb<<8)|lb;
      }
    }
  } else {
    data = mem_read(&my_cache, addr, ac);
  }
  printf("word %x read from address %x.\n", data, addr);
}

void
write_data(addr_t addr, int len, unsigned data)
{
  access_t ac;
  printf("writing data 0x%x to address 0x%x, len=%d\n", data, addr, len);
  ac.rw = 1;
  ac.len = len;
  if (addr%len==0) {
    mem_write(&my_cache, addr, data, ac);
  } else {
    if (len==2) {
      ac.len = 1;
      mem_write(&my_cache, addr, data, ac);
      mem_write(&my_cache, addr+1, data>>8, ac);
    } else if (len==4) {
      if (addr%2==0) {
        ac.len = 2;
        mem_write(&my_cache, addr, data, ac);
        mem_write(&my_cache, addr+2, data>>16, ac);
      } else {
        ac.len = 1;
        mem_write(&my_cache, addr, data, ac);
        mem_write(&my_cache, addr+3, data>>24, ac);
        ac.len = 2;
        mem_write(&my_cache, addr+1, data>>8, ac);
      }
    }
  }
  printf("word %x written to %x.\n", data, addr);
}

void inval()
{
  invalid_entire_cache(&my_cache);
}
