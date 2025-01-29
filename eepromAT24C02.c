#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define EEPROM_ADDR 0x50

void i2c_write_byte(uint16_t mem_addr, uint8_t data) {
    uint8_t buffer[3] = {mem_addr >> 8, mem_addr & 0xFF, data};
    i2c_write_blocking(I2C_PORT, EEPROM_ADDR, buffer, 3, false);
}

uint8_t i2c_read_byte(uint16_t mem_addr) {
    uint8_t data;
    uint8_t mem_addr_buf[2] = {mem_addr >> 8, mem_addr & 0xFF};
    i2c_write_blocking(I2C_PORT, EEPROM_ADDR, mem_addr_buf, 2, true);
    i2c_read_blocking(I2C_PORT, EEPROM_ADDR, &data, 1, false);
    return data;
}

int main() {
    stdio_init_all();
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);

    printf("EEPROM AT24C02 Test\n");

    // Escrever um byte na EEPROM
    i2c_write_byte(0x00, 0xAB);
    sleep_ms(5); // Aguardar o ciclo de gravação

    // Ler o byte da EEPROM
    uint8_t data = i2c_read_byte(0x00);
    printf("Data read: 0x%X\n", data);

    while (1) {
        tight_loop_contents();
    }

    return 0;
}
