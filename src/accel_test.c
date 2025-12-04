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

int main()
{
    int file;
    char config[2];
    char data[6];

    // Open I2C
    if ((file = open(I2C_BUS, O_RDWR)) < 0) {
        printf("Failed to open i2c bus\n");
        return 1;
    }

    // Talk to ADXL345
    if (ioctl(file, I2C_SLAVE, ADXL345_ADDR) < 0) {
        printf("Failed to connect to the sensor\n");
        return 1;
    }

    // Enable measurement mode
    config[0] = POWER_CTL;
    config[1] = 0x08;
    write(file, config, 2);

    printf("Accelerometer ready\n");

    while(1) {

        // Select DATAX0
        config[0] = DATAX0;
        write(file, config, 1);

        // Read 6 bytes: X,Y,Z
        read(file, data, 6);

        int16_t x = (data[1] << 8) | data[0];
        int16_t y = (data[3] << 8) | data[2];
        int16_t z = (data[5] << 8) | data[4];

        float ax = x / 256.0f;
        float ay = y / 256.0f;
        float az = z / 256.0f;

        printf("AX: %6.2f | AY: %6.2f | AZ: %6.2f\n", ax, ay, az);

        usleep(100000);
    }

    close(file);
    return 0;
}
