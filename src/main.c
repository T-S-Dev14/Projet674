#include <stdio.h>
#include <unistd.h>

#include "DE1-SoC_I2C.h"
#include "DE1-SoC_IO.h"

int main() {

    if (init_accel() != 0) {
        printf("Erreur accel\n");
        return 1;
    }

    if (de1_io_init() != 0) {
        printf("Erreur FPGA IO\n");
        return 1;
    }

    printf("DONE. Reading inputs...\n");

    while(1) {

        float ax = read_accel_x();
        float ay = read_accel_y();
        float az = read_accel_z();

        uint32_t sw  = read_switches();
        uint32_t key = read_buttons();

        printf("\rAX:%5.2f AY:%5.2f AZ:%5.2f | SW:%08X | KEY:%08X",
                ax, ay, az, sw, key);

        fflush(stdout);
        usleep(80000);
    }

    de1_io_close();
    return 0;
}
