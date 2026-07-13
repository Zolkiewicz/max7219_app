#include <stdio.h>
#include <stdlib.h>
#include <gpiod.h>
#include <stdint.h>
#include <unistd.h>

#define PIN_DIN 10
#define PIN_CS  8
#define PIN_CLK 11

#define CHIP_PATH "/dev/gpiochip0"

#define DECODE_MODE 0x09
#define INTENSITY 0x0A
#define SCAN_LIMIT 0x0B
#define SHUTDOWN 0x0C
#define DISPLAY_TEST 0x0F

struct gpiod_chip *chip = NULL;
struct gpiod_line_request *request = NULL;
unsigned int out_offsets[] = {PIN_DIN, PIN_CS, PIN_CLK};

int chip_init(void);
int max7219_init(void);
void cleanup(void);

/*** SEND DATA ***/

void max7219_send_packet(uint8_t reg, uint8_t data) {
    uint16_t packet = ((uint16_t)reg << 8) | data;

    for (int i = 0; i < 16; i++) {

        gpiod_line_request_set_value(request, PIN_CLK, GPIOD_LINE_VALUE_INACTIVE);

        if (packet & (1 << (15 - i))) 
            gpiod_line_request_set_value(request, PIN_DIN, GPIOD_LINE_VALUE_ACTIVE);
        else
            gpiod_line_request_set_value(request, PIN_DIN, GPIOD_LINE_VALUE_INACTIVE);

        gpiod_line_request_set_value(request, PIN_CLK, GPIOD_LINE_VALUE_ACTIVE);
    }
    
}

void max7219_write_row(uint8_t reg, uint32_t data) {
    gpiod_line_request_set_value(request, PIN_CS, GPIOD_LINE_VALUE_INACTIVE);

    for (int k = 0; k < 4; k++) {
        uint8_t byte_to_send = (uint8_t)(data >> ((3 - k) * 8));
        max7219_send_packet(reg, byte_to_send);
    }

    gpiod_line_request_set_value(request, PIN_CLK, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(request, PIN_CS, GPIOD_LINE_VALUE_ACTIVE);
}

/*** MAIN ***/

int main(void) {
    atexit(cleanup);
    if (chip_init() == -1) exit(EXIT_FAILURE);
    if (max7219_init() == -1) exit(EXIT_FAILURE);

    for (int k = 0 ; k < 8; k++) {
        max7219_write_row(k + 1, 300);
    }

    sleep(5);
    
    return 0;
}


/*** INIT ***/

int chip_init(void) {
    chip = gpiod_chip_open(CHIP_PATH);
    if (!chip) {
        perror("Error: chip - gpiod_chip_open ");
        return -1;
    }
    return 0;
}

int max7219_init(void) {
    struct gpiod_line_settings *line_settings = NULL; 
    struct gpiod_line_config *line_config = NULL;
    struct gpiod_request_config *req_config = NULL;

    line_settings = gpiod_line_settings_new();
    if (!line_settings) {
        perror("Error: line_settings - gpiod_line_settings_new ");
        exit(EXIT_FAILURE);        
    }

    gpiod_line_settings_set_direction(line_settings, GPIOD_LINE_DIRECTION_OUTPUT);
    gpiod_line_settings_set_output_value(line_settings, GPIOD_LINE_VALUE_INACTIVE);

    line_config = gpiod_line_config_new();
    if (!line_config) {
        perror("Error: line_config - gpiod_line_config_new ");
        goto error;     
    }

    gpiod_line_config_add_line_settings(line_config, out_offsets, 3, line_settings);

    req_config = gpiod_request_config_new();
    if(!req_config) {
        perror("Error: req_config - gpiod_request_config_new ");
        goto error; 
    }

    gpiod_request_config_set_consumer(req_config, "matrix_led_app");

    request = gpiod_chip_request_lines(chip, req_config, line_config);
    if (!request) {
        perror("Error: request - gpiod_chip_request_lines ");
        goto error; 
    }

    gpiod_line_settings_free(line_settings);
    gpiod_line_config_free(line_config);
    gpiod_request_config_free(req_config);

    max7219_write_row(DISPLAY_TEST, 0x00000000);
    max7219_write_row(DECODE_MODE, 0x00000000);
    max7219_write_row(SCAN_LIMIT, 0x07070707);
    max7219_write_row(INTENSITY, 0x01010101);
    max7219_write_row(SHUTDOWN, 0x01010101);

    gpiod_line_request_set_value(request, PIN_CLK, GPIOD_LINE_VALUE_INACTIVE);

    return 0;

    error:
    if (line_settings) gpiod_line_settings_free(line_settings);
    if (line_config) gpiod_line_config_free(line_config);
    if (req_config) gpiod_request_config_free(req_config);
    if (request) {
        gpiod_line_request_release(request);
        request = NULL;
    }
    return -1;
}

/*** CLEAN UP ***/

void cleanup(void) {

    if (request) {
        for (int k = 0; k < 8; k++) {
            max7219_write_row(k + 1, 0x00);
        }
        gpiod_line_request_release(request);
        request = NULL;
    }

    if (chip) {
        gpiod_chip_close(chip);
        chip = NULL;
    }

}