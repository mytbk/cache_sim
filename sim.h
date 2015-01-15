#ifndef SIM_H
#define SIM_H

#include "type.h"

void sim_cache_init();
void read_data(addr_t addr, int len, int ext);
void write_data(addr_t addr, int len, unsigned data);
void inval();

#endif
