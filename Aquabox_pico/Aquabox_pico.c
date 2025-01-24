#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"

//#define I2C_PORT i2c0
#define I2C_SDA 20
#define I2C_SCL 21

// Endereço I2C do DS1307
#define DS1307_ADDRESS 0x68

// Defição dos pinos do LCD
#define LCD_RS 2
#define LCD_E  3
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7

// Comandos do LCD
#define LCD_CMD 0
#define LCD_DATA 1

// Definição do pinos do relés
#define BOMBA   22
#define SETOR_2 26
#define SETOR_1 27
#define CAIXA   28

//Definição dos pinos dos sensores
#define NIVEL_BAIXO 19
#define NIVEL_ALTO 18
#define DELAY_DEBOUNCE 50       //50 milissegundos

// Temporizadores de debounce
absolute_time_t ultima_vez_nivel_baixo;
absolute_time_t ultima_vez_nivel_alto;

// Definições gerais
#define OFFSET_ASCII 48
#define CUMBUCO "AQUABOX  CUMBUCO"
#define OFF     0
#define ON      1

//Estrutura para controle do relógio
struct tempo
{
    uint8_t segundos;
    uint8_t minutos;
    uint8_t horas;
    uint8_t diaSemana;
    uint8_t dia;
    uint8_t mes;
    uint8_t ano;
};

//Estrutura para controle da irrigação
struct irriga
{
    uint8_t hora;
    uint8_t minutos;
    bool dia_da_semana[7];
    uint8_t duracao;
};

// Variáveis
int8_t funcao_ativa = 0;
volatile bool nivel_baixo_flag = false;
volatile bool nivel_alto_flag = false;
bool enchendo_flag = false;
bool irrigando_flag = false;

// Variáveis a partir de estruturas
struct tempo relogio_rtc;
struct irriga hora_Irrigar;

// Protótipo das funções
//int64_t alarm_callback(alarm_id_t id, void *user_data);
void lcd_init();
void init_gpio();
void init_irriga(uint8_t hora, uint8_t minutos, uint8_t duracao);
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
void converte_para_caracteres(int num);
void relogio ();
static void lcd_send_nibble(uint8_t nibble, uint8_t mode);
static void lcd_send(uint8_t value, uint8_t mode);
bool nivel_timer_callback(struct repeating_timer *t);
void ativa_flag_irrigacao();
void estado_0();
void estado_1();
void estado_2();

int main()
{
    // Inicializa interfaces seriais para debug
    stdio_init_all();
    
    // Inicializa I2C para o RTC
    init_i2c();
    // Inicializa display LCD 16x2
    lcd_init();

    //Inicialização dos gpios digitais
    init_gpio();

    // Inicializa dados de horário para irrigação
    init_irriga(10, 31, 2);     //Ajustado para irrigar as 10:32 pelo tempo de 2 minutos

    // Inicializa os temporizadores de debounce
    ultima_vez_nivel_baixo = get_absolute_time();
    ultima_vez_nivel_alto = get_absolute_time();

    // Configurar o timer
    struct repeating_timer timer;
    add_repeating_timer_ms(100, nivel_timer_callback, NULL, &timer);

    // Seta o relógio RTC para 21/01/2021
    set_rtc_time(0, 30, 10, 2, 21, 1, 25);  // 21/01/2025 - 10:30:00
    
    lcd_escreve_string(CUMBUCO);
    lcd_set_cursor(2,1);
    lcd_escreve_string("Versao:  1.0");
    sleep_ms(2000);
    lcd_limpa();

    while (true) 
    {
        switch (funcao_ativa)
        {
        case 0:
            estado_0();
            break;
        
        case 1:
            estado_1();
            break;
        
        case 2:
            estado_2();
            break;
        
        default:
            break;
        }
        
    }
}

void estado_0()
{
    lcd_set_cursor(0,0);
    lcd_escreve_string(CUMBUCO);

    relogio();

    if((nivel_baixo_flag == true) && (irrigando_flag == false))
    {
        funcao_ativa = 1;
        enchendo_flag = true;
    }

    ativa_flag_irrigacao();

    if(irrigando_flag == true && enchendo_flag == false)
    {
        funcao_ativa = 2;
    }
}

void estado_1()
{
    lcd_limpa();
    lcd_set_cursor(0,0);
    lcd_escreve_string(CUMBUCO);
    lcd_set_cursor(1,1);
    lcd_escreve_string("Enchendo caixa");

    while(true)
    {
        ativa_flag_irrigacao();
        nivel_baixo_flag = false;
        gpio_put(CAIXA, ON);            // Liga a vávula da caixa d'água
        sleep_ms(1000);                 // Espera um tempo
        gpio_put(BOMBA, ON);            // Liga a bomba d'água

        if(nivel_alto_flag == true)
        {
            gpio_put(BOMBA, OFF);            // Desliga a bomba d'água
            sleep_ms(1000);                 // Espera um tempo
            gpio_put(CAIXA, OFF);            // desliga a vávula da caixa d'água
            funcao_ativa = 0;
            nivel_alto_flag = false;
            enchendo_flag = false;
            return;
        }
    }
}

void estado_2()
{
    bool s1 = true;         // Habilita setor 1
    bool s2 = false;        // Habilita setor 2
    static int contador = 0;

    lcd_limpa();
    lcd_set_cursor(0,0);
    lcd_escreve_string(CUMBUCO);
    lcd_set_cursor(2,1);
    lcd_escreve_string("Irrigando...");

    while(true)
    {
        if ((s1 == true) && (irrigando_flag == true))
        {
            lcd_set_cursor(0,1);
            lcd_escreve_string("Irrigando setor1");
            gpio_put(SETOR_1, ON);
            sleep_ms(1000);
            gpio_put(BOMBA, ON);
            if(contador == hora_Irrigar.duracao)
            {
                s2 = true;
                s1 = false;
                contador =0;
                gpio_put(BOMBA, OFF);
                sleep_ms(1000);
                gpio_put(SETOR_1, OFF);
            }
            contador++;
        } 

        if ((s2 == true) && (irrigando_flag == true))
        {
            lcd_set_cursor(0,1);
            lcd_escreve_string("Irrigando setor2");
            gpio_put(SETOR_2, ON);
            sleep_ms(1000);
            gpio_put(BOMBA, ON);
            if(contador == hora_Irrigar.duracao)
            {
                s2 = false;
                s1 = true;
                gpio_put(BOMBA, OFF);
                sleep_ms(1000);
                gpio_put(SETOR_2, OFF);
                funcao_ativa = 0;
                irrigando_flag = false;
                return;
            }
            contador++;
        }      

    }
}

void ativa_flag_irrigacao()
{
    if((hora_Irrigar.hora == relogio_rtc.horas) && (hora_Irrigar.minutos == relogio_rtc.minutos))
    {
        irrigando_flag = true;
    }
}

void relogio ()
{
    get_rtc_time();

    char dia_dezena = (relogio_rtc.dia/10) + OFFSET_ASCII;
    char dia_unidade = (relogio_rtc.dia%10) + OFFSET_ASCII;
    char mes_dezena = (relogio_rtc.mes/10) + OFFSET_ASCII;
    char mes_unidade = (relogio_rtc.mes%10) + OFFSET_ASCII;
    char segundo_dezena = (relogio_rtc.segundos/10) + OFFSET_ASCII;
    char segundo_unidade = (relogio_rtc.segundos%10) + OFFSET_ASCII;
    char minuto_dezena = (relogio_rtc.minutos/10) + OFFSET_ASCII;
    char minuto_unidade = (relogio_rtc.minutos%10) + OFFSET_ASCII;
    char hora_dezena = (relogio_rtc.horas/10) + OFFSET_ASCII;
    char hora_unidade = (relogio_rtc.horas%10) + OFFSET_ASCII;

    lcd_set_cursor(0,1);
    lcd_escreve_char(dia_dezena);
    lcd_escreve_char(dia_unidade);
    lcd_escreve_char('/');
    lcd_escreve_char(mes_dezena);
    lcd_escreve_char(mes_unidade);
    lcd_escreve_char(' ');
    lcd_escreve_char('-');
    lcd_escreve_char(' ');
    lcd_escreve_char(hora_dezena);
    lcd_escreve_char(hora_unidade);
    lcd_escreve_char(':');
    lcd_escreve_char(minuto_dezena);
    lcd_escreve_char(minuto_unidade);
    lcd_escreve_char(':');
    lcd_escreve_char(segundo_dezena);
    lcd_escreve_char(segundo_unidade);


}

void converte_para_caracteres(int num) {
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

void init_gpio()
{
    gpio_init(BOMBA);
    gpio_set_dir(BOMBA, GPIO_OUT);
    gpio_put(BOMBA, OFF);

    gpio_init(SETOR_1);
    gpio_set_dir(SETOR_1, GPIO_OUT);
    gpio_put(SETOR_1, OFF);

    gpio_init(SETOR_2);
    gpio_set_dir(SETOR_2, GPIO_OUT);
    gpio_put(SETOR_2, OFF);

    gpio_init(CAIXA);
    gpio_set_dir(CAIXA, GPIO_OUT);
    gpio_put(CAIXA, OFF);

    gpio_init(NIVEL_ALTO);
    gpio_set_dir(NIVEL_ALTO, GPIO_IN);
    gpio_pull_up(NIVEL_ALTO);

    gpio_init(NIVEL_BAIXO);
    gpio_set_dir(NIVEL_BAIXO, GPIO_IN);
    gpio_pull_up(NIVEL_BAIXO);
}

void init_irriga(uint8_t hora, uint8_t minutos, uint8_t duracao)
{
    hora_Irrigar.hora = hora;
    hora_Irrigar.minutos = minutos;
    hora_Irrigar.duracao = duracao*60;
    
    for(int i = 0; i < 7; i++)
    {
        hora_Irrigar.dia_da_semana[i] = 1;
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
  uint8_t dadosHora[7];
  uint8_t registroInicial = 0x00;

  // Escreve o valor do registro inicial
  i2c_write_blocking(i2c0, DS1307_ADDRESS, &registroInicial, 1, true);

  // Lê as informações de data e hora e armazena em um vetor de 7 posições
  i2c_read_blocking(i2c0, DS1307_ADDRESS, dadosHora, 7, false);

  // Faz as conversões para facilitar a impressão no console
  relogio_rtc.segundos = bcd_decimal(dadosHora[0] & 0x7F);  
  relogio_rtc.minutos = bcd_decimal(dadosHora[1]);
  relogio_rtc.horas = bcd_decimal(dadosHora[2]);
  relogio_rtc.diaSemana = bcd_decimal(dadosHora[3]);
  relogio_rtc.dia = bcd_decimal(dadosHora[4]);
  relogio_rtc.mes = bcd_decimal(dadosHora[5]);
  relogio_rtc.ano = bcd_decimal(dadosHora[6]);

  //printf("Data: %02d/%02d/20%02d - ", relogio_rtc.dia, relogio_rtc.mes, relogio_rtc.ano);
  //printf("Hora: %02d:%02d:%02d\n", relogio_rtc.horas, relogio_rtc.minutos, relogio_rtc.segundos);
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

// Funções Callback
// Função de callback para o timer
bool nivel_timer_callback(struct repeating_timer *t)
{
    // Verifica o estado do sensor de nível baixo
    if(!gpio_get(NIVEL_BAIXO) && absolute_time_diff_us(ultima_vez_nivel_baixo, get_absolute_time()) >= DELAY_DEBOUNCE * 1000)
    {
        nivel_baixo_flag = true;
        ultima_vez_nivel_baixo = get_absolute_time();
    }

    // Verifica o estado do sensor de nível baixo
    if(!gpio_get(NIVEL_ALTO) && absolute_time_diff_us(ultima_vez_nivel_alto, get_absolute_time()) >= DELAY_DEBOUNCE * 1000)
    {
        nivel_alto_flag = true;
        ultima_vez_nivel_alto = get_absolute_time();
    }

    return true;
}
