#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"

// I2C defines
// This example will use I2C0 on GPIO20 (SDA) and GPIO21(SCL) running at 100KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
//#define I2C_PORT i2c0
#define I2C_SDA 20
#define I2C_SCL 21

// Endereço I2C do DS1307
#define DS1307_ADDRESS 0x68

// Defina os pinos do LCD
#define LCD_RS 2
#define LCD_E  3
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7

// Comandos do LCD
#define LCD_CMD 0
#define LCD_DATA 1

// Variáveis
uint8_t dadosHora[7];
int segundos;


// Protótipo das funções
//int64_t alarm_callback(alarm_id_t id, void *user_data);
void lcd_init();
void lcd_limpa();
void lcd_home();
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_escreve_char(char c);
void lcd_escreve_string(const char *str);
int decimal_bcd(int valor);
int bcd_decimal(int valor);
void set_rtc_time(uint8_t segundo, uint8_t minuto, uint8_t hora, uint8_t dia, uint8_t diaMes, uint8_t mes, uint8_t ano);
void get_rtc_time();
void init_i2c();
void convert_to_characters(int num);
static void lcd_send_nibble(uint8_t nibble, uint8_t mode);
static void lcd_send(uint8_t value, uint8_t mode);

int main()
{
    // Inicializa interfaces seriais para debug
    stdio_init_all();
    
    // Inicializa I2C para o RTC
    init_i2c();
    // Inicializa display LCD 16x2
    lcd_init();

    // Seta o relógio RTC para 21/01/2021
    set_rtc_time(0, 30, 10, 2, 21, 1, 25);  // 21/01/2025 - 10:30:00

    
    lcd_escreve_string("AQUABOX  CUMBUCO");
    lcd_set_cursor(2,1);
    lcd_escreve_string("Versao:  1.0");
    sleep_ms(5000);
    lcd_limpa();
     lcd_escreve_char('A');

    while (true) 
    {
        int seg = (bcd_decimal(dadosHora[0]));
        convert_to_characters(seg);
        get_rtc_time();
        sleep_ms(1000);
    }
}

int64_t alarm_callback(alarm_id_t id, void *user_data) 
{
    // Put your timeout handler code in here
    return 0;
}

// Funções dos display 16x2
/************************************************************************************** */
// Inicialização do display
void lcd_init() {
    gpio_init(LCD_RS);
    gpio_set_dir(LCD_RS, GPIO_OUT);

    gpio_init(LCD_E);
    gpio_set_dir(LCD_E, GPIO_OUT);

    gpio_init(LCD_D4);
    gpio_set_dir(LCD_D4, GPIO_OUT);

    gpio_init(LCD_D5);
    gpio_set_dir(LCD_D5, GPIO_OUT);

    gpio_init(LCD_D6);
    gpio_set_dir(LCD_D6, GPIO_OUT);

    gpio_init(LCD_D7);
    gpio_set_dir(LCD_D7, GPIO_OUT);

    sleep_ms(50); // Espera para a inicialização

    lcd_send(0x03, LCD_CMD);  // Inicialização em 8 bits
    sleep_ms(5);
    lcd_send(0x03, LCD_CMD);  // Inicialização em 8 bits
    sleep_us(200);
    lcd_send(0x03, LCD_CMD);  // Inicialização em 8 bits
    lcd_send(0x02, LCD_CMD);  // Inicialização em 4 bits

    lcd_send(0x28, LCD_CMD); // Modo 4 bits, 2 linhas, 5x8 pontos
    lcd_send(0x0C, LCD_CMD); // Display ligado, cursor desligado
    lcd_send(0x06, LCD_CMD); // Movimento do cursor para a direita
    lcd_send(0x01, LCD_CMD); // Limpa o display
    sleep_ms(5); // Espera para a inicialização
}

void lcd_limpa() 
{
    lcd_send(0x01, LCD_CMD);
    sleep_ms(5);
}

void lcd_home() 
{
    lcd_send(0x02, LCD_CMD);
    sleep_ms(5);
}

void lcd_set_cursor(uint8_t col, uint8_t row) 
{
    uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    lcd_send(0x80 | (col + row_offsets[row]), LCD_CMD);
}

void lcd_escreve_char(char c) 
{
    lcd_send(c, LCD_DATA);
}

void lcd_escreve_string(const char *str) 
{
    while (*str) {
        lcd_escreve_char(*str++);
    }
}

static void lcd_send_nibble(uint8_t nibble, uint8_t mode) 
{
    gpio_put(LCD_RS, mode);

    gpio_put(LCD_D4, (nibble >> 0) & 0x01);
    gpio_put(LCD_D5, (nibble >> 1) & 0x01);
    gpio_put(LCD_D6, (nibble >> 2) & 0x01);
    gpio_put(LCD_D7, (nibble >> 3) & 0x01);

    gpio_put(LCD_E, 1);
    sleep_us(1);  // Pequena pausa para habilitar
    gpio_put(LCD_E, 0);
    sleep_us(100);  // Pequena pausa para desabilitar
}

static void lcd_send(uint8_t value, uint8_t mode) 
{
    lcd_send_nibble(value >> 4, mode);
    lcd_send_nibble(value & 0x0F, mode);
}

//Funções do relógio de tempo real - RTC
/************************************************************************************** */
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
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);
}


void convert_to_characters(int num) {
    if (num < 0 || num > 59) {
        printf("Número fora do intervalo\n");
        return;
    }

    // Converte o número em caracteres e imprime
    char result[3];
    snprintf(result, sizeof(result), "%02d", num);
    //printf("Número convertido em caracteres: %s\n", result);
    lcd_escreve_char(result[0]);
}


