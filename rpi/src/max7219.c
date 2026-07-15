#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "../inc/max7219.h"
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <fcntl.h>

#define SPI_DEVICE "/dev/spidev0.0"

#define DECODE_MODE 0x09
#define INTENSITY 0x0A
#define SCAN_LIMIT 0x0B
#define SHUTDOWN 0x0C
#define DISPLAY_TEST 0x0F

int fd = -1;
uint8_t mode = SPI_MODE_0;
uint8_t bits = 8;
uint32_t speed = 1000000;


void max7219_write_row(uint8_t reg, uint32_t data) {
    uint8_t tx_buf[8] = {0};
    uint8_t rx_buf[8] = {0};

    for (int k = 0; k < 4; k++) {
        tx_buf[k * 2] = reg;
        tx_buf[k * 2 + 1] = (uint8_t)(data >> ((3 - k) * 8));
    }

    struct spi_ioc_transfer tr = {0};
    tr.tx_buf = (unsigned long)tx_buf;
    tr.rx_buf = (unsigned long)rx_buf;
    tr.len = 8;
    tr.speed_hz = speed;
    tr.bits_per_word = bits;

    if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
        perror("Error: SPI transfer failed");
    }
}

void max7219_write_array(uint32_t *array) {
    for (int k = 0; k < 8; k++) {
        max7219_write_row(k + 1, array[k]);
    }
}

/*** CLEAN UP ***/

void max7219_cleanup(void) {
    uint32_t empty[8] = {0};
    max7219_write_array(empty);
    if (fd) {
        close(fd);
        fd = -1;
    }
}

/*** INIT ***/

int max7219_init(void) {
    fd = open(SPI_DEVICE, O_RDWR);
    if (fd == -1) {
        perror("Error: open spi device");
        return -1;
    }

    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) == -1) goto error;
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) goto error;
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) goto error;

    max7219_write_row(DISPLAY_TEST, 0x00000000);
    max7219_write_row(DECODE_MODE, 0x00000000);
    max7219_write_row(SCAN_LIMIT, 0x07070707);
    max7219_write_row(INTENSITY, 0x01010101);
    max7219_write_row(SHUTDOWN, 0x01010101);

    return 0;

    error:
    perror("Error: SPI configuration failed");
    return -1;

}