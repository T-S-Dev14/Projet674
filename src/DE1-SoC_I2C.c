#include "i2c.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>

#define I2C_BUS "/dev/i2c-0"
#define ADXL345_ADDR 0x53

#define POWER_CTL 0x2D
#define DATAX0    0x32

static int file;

static float convert(int16_t raw){
    return raw / 256.0f;
}

int init_accel() {

    char config[2];

    if ((file = open(I2C_BUS, O_RDWR)) < 0) return -1;
    if (ioctl(file, I2C_SLAVE, ADXL345_ADDR) < 0) return -1;

    // Enable
    config[0] = POWER_CTL;
    config[1] = 0x08;
    write(file, config, 2);

    return 0;
}

void read_raw(int16_t *x, int16_t *y, int16_t *z) {
    char data[6];
    char reg = DATAX0;

    write(file, &reg, 1);
    read(file, data, 6);

    *x = (data[1] << 8) | data[0];
    *y = (data[3] << 8) | data[2];
    *z = (data[5] << 8) | data[4];
}

float read_accel_x() {
    int16_t x,y,z;
    read_raw(&x,&y,&z);
    return convert(x);
}

float read_accel_y() {
    int16_t x,y,z;
    read_raw(&x,&y,&z);
    return convert(y);
}

float read_accel_z() {
    int16_t x,y,z;
    read_raw(&x,&y,&z);
    return convert(z);
}
