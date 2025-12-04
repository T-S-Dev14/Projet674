#include "DE1-SoC_IO.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define LW_BRIDGE_BASE  0xFF200000
#define LW_BRIDGE_SPAN  0x00005000

#define SW_BASE   0x40 
#define KEY_BASE  0x50

static int mem_fd;
static void *lw_bridge_ptr;

static volatile uint32_t *sw_ptr;
static volatile uint32_t *key_ptr;

int de1_io_init() {
    mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd == -1) return -1;

    lw_bridge_ptr = mmap(NULL, LW_BRIDGE_SPAN, PROT_READ | PROT_WRITE,
                         MAP_SHARED, mem_fd, LW_BRIDGE_BASE);

    if (lw_bridge_ptr == MAP_FAILED) return -1;

    sw_ptr  = (uint32_t *)((char *)lw_bridge_ptr + SW_BASE);
    key_ptr = (uint32_t *)((char *)lw_bridge_ptr + KEY_BASE);

    return 0;
}

uint32_t read_switches() {
    return *sw_ptr;
}

uint32_t read_buttons() {
    return *key_ptr;
}

void de1_io_close() {
    munmap(lw_bridge_ptr, LW_BRIDGE_SPAN);
    close(mem_fd);
}
