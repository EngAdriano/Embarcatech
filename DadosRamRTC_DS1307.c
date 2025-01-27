#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Endereço do módulo RTC DS1307
#define RTC_ADDR 0x68

// Função para inicializar o I2C
void i2c_init_custom() {
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);
}

// Função para ler dados da memória RAM do RTC
void read_ram_data(uint8_t *data, size_t length) {
    uint8_t reg = 0x08; // Endereço da RAM começa no 0x08
    i2c_write_blocking(i2c0, RTC_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c0, RTC_ADDR, data, length, false);
}

// Função para escrever dados na memória RAM do RTC
void write_ram_data(uint8_t *data, size_t length) {
    uint8_t buffer[length + 1];
    buffer[0] = 0x08; // Endereço da RAM começa no 0x08
    for (size_t i = 0; i < length; i++) {
        buffer[i + 1] = data[i];
    }
    i2c_write_blocking(i2c0, RTC_ADDR, buffer, length + 1, false);
}

int main() {
    stdio_init_all();
    i2c_init_custom();

    // Dados a serem armazenados na RAM
    uint8_t data_to_write[] = { 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd' };
    size_t data_length = sizeof(data_to_write) / sizeof(data_to_write[0]);

    // Escrevendo dados na RAM
    write_ram_data(data_to_write, data_length);

    // Lendo dados da RAM
    uint8_t data_read[data_length];
    read_ram_data(data_read, data_length);

    // Imprimindo dados lidos
    printf("Dados lidos da RAM: ");
    for (size_t i = 0; i < data_length; i++) {
        putchar(data_read[i]);
    }
    printf("\n");

    return 0;
}
