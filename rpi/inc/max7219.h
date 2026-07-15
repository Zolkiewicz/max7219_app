#ifndef MAX7219_H
#define MAX7219_H

int chip_init(void);
int max7219_init(void);
void max7219_cleanup(void);

void max7219_write_array(uint32_t *array);

#endif