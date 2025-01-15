
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Definições dos pinos do I2C
#define I2C_SDA_PIN 20  // SDA
#define I2C_SCL_PIN 21  // SCL

// Endereço I2C do DS1307
#define DS1307_ADDRESS 0x68

// Função para converter valores BCD para decimal
int bcd_to_decimal(uint8_t bcd) {
  return (bcd >> 4) * 10 + (bcd & 0x0F);
}

// Função para converter valores decimais para BCD
uint8_t decimal_to_bcd(int decimal) {
  return ((decimal / 10) << 4) | (decimal % 10);
}

// Função para configurar a data e hora no DS1307
void set_rtc_time(uint8_t second, uint8_t minute, uint8_t hour,
                  uint8_t day, uint8_t date, uint8_t month, uint8_t year) {
  uint8_t time_data[8] = {
    0x00,                     // Começa no registro 0x00
    decimal_to_bcd(second) & 0x7F,  // Segundos (ativa o oscilador com MSB = 0)
    decimal_to_bcd(minute),         // Minutos
    decimal_to_bcd(hour),           // Horas
    decimal_to_bcd(day),            // Dia da semana
    decimal_to_bcd(date),           // Dia do mês
    decimal_to_bcd(month),          // Mês
    decimal_to_bcd(year)            // Ano
  };

  // Escreve nos registros do RTC
  i2c_write_blocking(i2c0, DS1307_ADDRESS, time_data, 8, false);
}

// Função para ler a data e hora do DS1307
void get_rtc_time() {
  uint8_t time_data[7];

  // Solicita leitura a partir do endereço 0x00
  uint8_t start_register = 0x00;
  i2c_write_blocking(i2c0, DS1307_ADDRESS, &start_register, 1, true);

  // Lê os 7 registros de tempo
  i2c_read_blocking(i2c0, DS1307_ADDRESS, time_data, 7, false);

  // Converte os valores lidos de BCD para decimal
  int second = bcd_to_decimal(time_data[0] & 0x7F);  // Remove o bit MSB (oscilador)
  int minute = bcd_to_decimal(time_data[1]);
  int hour = bcd_to_decimal(time_data[2]);
  int day = bcd_to_decimal(time_data[3]);
  int date = bcd_to_decimal(time_data[4]);
  int month = bcd_to_decimal(time_data[5]);
  int year = bcd_to_decimal(time_data[6]);

  // Imprime os valores no console
  printf("Data: %02d/%02d/20%02d\n", date, month, year);
  printf("Hora: %02d:%02d:%02d\n", hour, minute, second);
}

// Inicializa o I2C no Raspberry Pi Pico
void init_i2c() {
  i2c_init(i2c0, 100 * 1000);  // Velocidade de 100kHz
  gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA_PIN);
  gpio_pull_up(I2C_SCL_PIN);
}

int main() {
  // Inicializa as funções padrão
  stdio_init_all();

  // Inicializa o I2C
  init_i2c();

  // Configura a data e hora inicial no RTC DS1307
  printf("Configurando o RTC DS1307...\n");
  set_rtc_time(0, 27, 13, 2, 24, 9, 24);  // 24/09/2024 - 13:27:00

  printf("Sistema inicializado! Lendo RTC a cada 5 segundos...\n");

  // Loop principal: lê e exibe a data/hora a cada 5 segundos
  while (true) {
    get_rtc_time();
    sleep_ms(5000);  // Atraso de 5 segundos
  }

  return 0;
}