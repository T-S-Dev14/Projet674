#ifndef DE1_IO_H
#define DE1_IO_H

#include <stdint.h>

int de1_io_init();
uint32_t read_switches();
uint32_t read_buttons();
void de1_io_close();

#endif
