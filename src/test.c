#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>

#define LW_BRIDGE_BASE 0xFF200000     // Base LWHPS2FPGA
#define LW_BRIDGE_SPAN 0x00200000     // 2 Mo

#define BUTTONS_OFFSET 0x00000050     // Adresse des KEY sur DE1-SoC

int main() {
    int fd;
    void *lw_bridge_map;
    volatile uint32_t *buttons_ptr;

    // ouvrir /dev/mem
    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    lw_bridge_map = mmap(NULL, LW_BRIDGE_SPAN, PROT_READ | PROT_WRITE,
                         MAP_SHARED, fd, LW_BRIDGE_BASE);
    if (lw_bridge_map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return EXIT_FAILURE;
    }

    buttons_ptr = (uint32_t*)((uint8_t*)lw_bridge_map + BUTTONS_OFFSET);

    printf("Lecture des KEY (boutons) sur DE1-SoC...\n");

    while (1) {
        uint32_t v = *buttons_ptr;
        printf("KEY = 0x%08X\n", v);
        usleep(200000); // 200 ms
    }

    munmap(lw_bridge_map, LW_BRIDGE_SPAN);
    close(fd);
    return 0;
}