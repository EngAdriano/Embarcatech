//*******************************************************************************************************************
// Criado: 14/01/25
// Autor: José Adriano
// Descrição: Utilização de RTC DS1307 com RaspberryPi Pico W 
// Configurada para 24/09/2024 - 13:27:00
// Mostra data e hora no console a cada 5 segundos
//*******************************************************************************************************************
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"

// Definições dos pinos do I2C
#define I2C_SDA_PIN 16  // SDA
#define I2C_SCL_PIN 17  // SCL

//Definido a frequência dotimer (5 segundos = 5000 ms)
#define INTERVALO_TIMER_MS 5000



// Endereço I2C do DS1307
#define DS1307_ADDRESS 0x68

// Protótipo de funções
int decimal_bcd(int valor);
int bcd_decimal(int valor);
void set_rtc_time(uint8_t segundo, uint8_t minuto, uint8_t hora, uint8_t dia, uint8_t diaMes, uint8_t mes, uint8_t ano);
void get_rtc_time();
void init_i2c();
bool repeating_timer_callback(repeating_timer_t *rt);
void read_ram_data(uint8_t *data, size_t length);
void write_ram_data(uint8_t *data, size_t length);

int main() {
  // Inicializa as funções padrão
  stdio_init_all();

  // Inicializa o I2C
  init_i2c();

  printf("Tarefa 2.4 - Unidade 4\n");
  // Configura a data e hora inicial no RTC DS1307
  printf("Configurando o RTC DS1307 para 24/09/2024 as 15:27:00\n");
  set_rtc_time(0, 27, 13, 2, 25, 9, 24);  // 24/09/2024 - 13:27:00

  printf("RTC será lido a cada 5 segundos\n");
  repeating_timer_t timer;
  if (add_repeating_timer_ms(INTERVALO_TIMER_MS, repeating_timer_callback, NULL, &timer)) 
  { 
    printf("Timer inicializado com sucesso\n"); 
  } 
  else 
  { 
    printf("Falha ao inicializar o timer\n"); return true; 
  }

  // Dados a serem armazenados na RAM
    uint8_t data_to_write[12] = { 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'};
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

  // Loop principal: lê e exibe a data/hora a cada 5 segundos
  while (true) 
  {
    sleep_ms(1);
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

// Seta as configurações de data e hora no DS1307
void set_rtc_time(uint8_t segundo, uint8_t minuto, uint8_t hora, uint8_t dia, uint8_t diaMes, uint8_t mes, uint8_t ano) 
{
  uint8_t tempoCalendario[8] = {
    0x00,                         // Começa no registro 0x00
    decimal_bcd(segundo) & 0x7F,  // Segundos 
    decimal_bcd(minuto),          // Minutos
    decimal_bcd(hora),            // Horas
    decimal_bcd(dia),             // Dia da semana
    decimal_bcd(diaMes),          // Dia do mês
    decimal_bcd(mes),             // Mês
    decimal_bcd(ano)              // Ano
  };

  // Configura os registradores internos do DS1307
  i2c_write_blocking(i2c0, DS1307_ADDRESS, tempoCalendario, 8, false);
}

// Função para recuperar a data e hora do DS1307
void get_rtc_time() 
{
  uint8_t dadosHora[7];
  uint8_t registroInicial = 0x00;

  // Escreve o valor do registro inicial
  i2c_write_blocking(i2c0, DS1307_ADDRESS, &registroInicial, 1, true);

  // Lê as informações de data e hora e armazena em um vetor de 7 posições
  i2c_read_blocking(i2c0, DS1307_ADDRESS, dadosHora, 7, false);

  // Faz as conversões para facilitar a impressão no console
  int segundos = bcd_decimal(dadosHora[0] & 0x7F);  
  int minutos = bcd_decimal(dadosHora[1]);
  int hora = bcd_decimal(dadosHora[2]);
  int dia = bcd_decimal(dadosHora[3]);
  int diaMes = bcd_decimal(dadosHora[4]);
  int mes = bcd_decimal(dadosHora[5]);
  int ano = bcd_decimal(dadosHora[6]);

  // mostra os valores no console
  printf("Data: %02d/%02d/20%02d - ", diaMes, mes, ano);
  printf("Hora: %02d:%02d:%02d\n", hora, minutos, segundos);
}

// Inicializa o I2C no Raspberry Pi Pico W
void init_i2c() 
{
  i2c_init(i2c0, 100 * 1000);  // Velocidade de 100kHz
  gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA_PIN);
  gpio_pull_up(I2C_SCL_PIN);
}

// Função para ler dados da memória RAM do RTC
void read_ram_data(uint8_t *data, size_t length) 
{
    uint8_t reg = 0x08; // Endereço da RAM começa no 0x08
    i2c_write_blocking(i2c0, DS1307_ADDRESS, &reg, 1, true);
    i2c_read_blocking(i2c0, DS1307_ADDRESS, data, length, false);
}

// Função para escrever dados na memória RAM do RTC
void write_ram_data(uint8_t *data, size_t length) 
{
    uint8_t buffer[length + 1];
    buffer[0] = 0x08; // Endereço da RAM começa no 0x08
    for (size_t i = 0; i < length; i++) {
        buffer[i + 1] = data[i];
    }
    i2c_write_blocking(i2c0, DS1307_ADDRESS, buffer, length + 1, false);
}

//Handler da interrupção do timer
bool repeating_timer_callback(repeating_timer_t *rt)
{
  get_rtc_time();
  return true;
}