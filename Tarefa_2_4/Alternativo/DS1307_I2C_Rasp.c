
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Definições dos pinos do I2C
#define I2C_SDA_PIN 20  // SDA
#define I2C_SCL_PIN 21  // SCL

// Endereço I2C do DS1307
#define DS1307_ADDRESS 0x68

// Protótipo de funções
int decimal_bcd(int valor);
int bcd_decimal(int valor);
void set_rtc_time(uint8_t segundo, uint8_t minuto, uint8_t hora, uint8_t dia, uint8_t diaMes, uint8_t mes, uint8_t ano);
void get_rtc_time();
void init_i2c();

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


// Área de funções

// Converte decimal para BCD
int decimal_bcd(int valor)
{
  return ((valor / 10 * 16) + (valor % 10));
}

//Converte BCD para decimal
int bcd_decimal(int valor)
{
  return ((valor / 16 * 10) + (valor % 16));
}

// Configura a data e hora no DS1307
void set_rtc_time(uint8_t segundo, uint8_t minuto, uint8_t hora, uint8_t dia, uint8_t diaMes, uint8_t mes, uint8_t ano) 
{
  uint8_t tempoCalendario[8] = {
    0x00,                         // Começa no registro 0x00
    decimal_bcd(segundo) & 0x7F,  // Segundos (ativa o oscilador com MSB = 0)
    decimal_bcd(minuto),          // Minutos
    decimal_bcd(hora),            // Horas
    decimal_bcd(dia),             // Dia da semana
    decimal_bcd(diaMes),          // Dia do mês
    decimal_bcd(mes),             // Mês
    decimal_bcd(ano)              // Ano
  };

  // Escreve nos registros do RTC
  i2c_write_blocking(i2c0, DS1307_ADDRESS, tempoCalendario, 8, false);
}

// Função para ler a data e hora do DS1307
void get_rtc_time() 
{
  uint8_t dadosHora[7];

  // Solicita leitura a partir do endereço 0x00
  uint8_t registroInicial = 0x00;
  i2c_write_blocking(i2c0, DS1307_ADDRESS, &registroInicial, 1, true);

  // Lê os 7 registros de tempo
  i2c_read_blocking(i2c0, DS1307_ADDRESS, dadosHora, 7, false);

  // Converte os valores lidos de BCD para decimal
  int segundos = bcd_decimal(dadosHora[0] & 0x7F);  // Remove o bit MSB (oscilador)
  int minutos = bcd_decimal(dadosHora[1]);
  int hora = bcd_decimal(dadosHora[2]);
  int dia = bcd_decimal(dadosHora[3]);
  int diaMes = bcd_decimal(dadosHora[4]);
  int mes = bcd_decimal(dadosHora[5]);
  int ano = bcd_decimal(dadosHora[6]);

  // Imprime os valores no console
  printf("Data: %02d/%02d/20%02d\n", diaMes, mes, ano);
  printf("Hora: %02d:%02d:%02d\n", hora, minutos, segundos);
}

// Inicializa o I2C no Raspberry Pi Pico
void init_i2c() 
{
  i2c_init(i2c0, 100 * 1000);  // Velocidade de 100kHz
  gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA_PIN);
  gpio_pull_up(I2C_SCL_PIN);
}