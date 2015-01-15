/* sim.c: cache simulator
   operations:
     readb: read a byte(8 bit)
     readw: read a word(16 bit)
     readd: read a dword(32 bit)
     readsb, readsw, readsd: read a byte, word, or dword, signed
     usage: read[s][bwd] <addr>
     writeb, writew, writed: write a byte, word, or dword
     usage: write[bwd] <addr> <data>
     inval: invalidate all cache lines
*/

#include "sim.h"
#include <stdio.h>
#include <string.h>
#ifndef BUFSIZ
#define BUFSIZ 1024
#endif

static const char *e_opcode = "error while reading opcode %s.\n";

int main()
{
  char cmd[BUFSIZ];
  char opcode[BUFSIZ];
  addr_t address;
  unsigned data;
  int len, ext;

  sim_cache_init();

  while (fgets(cmd, BUFSIZ, stdin)) {
    printf("command: %s", cmd);
    sscanf(cmd, "%s%x%x", opcode, &address, &data);
    if (strcmp(opcode, "inval")==0) {
      inval();
    } else if (strncmp(opcode, "read", 4)==0) {
      if (opcode[4]=='s') {
        ext = 1;
        opcode[4] = opcode[5];
      } else {
        ext = 0;
      }
      switch (opcode[4]) {
      case 'b':
        len = 1;
        break;
      case 'w':
        len = 2;
        break;
      case 'd':
        len = 4;
        break;
      default:
        fprintf(stderr, e_opcode, opcode);
      }
      read_data(address, len, ext);
    } else if (strncmp(opcode, "write", 5)==0) {
      switch (opcode[5]) {
      case 'b':
        len = 1;
        break;
      case 'w':
        len = 2;
        break;
      case 'd':
        len = 4;
        break;
      default:
        fprintf(stderr, e_opcode, opcode);
      }
      write_data(address, len, data);
    } else {
      fprintf(stderr, e_opcode, opcode);
    }
    puts("");
  }
  return 0;
}
