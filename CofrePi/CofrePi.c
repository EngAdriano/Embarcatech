//Práticas de GPIO com delay
//Unidade 4 | Capítulo 3
//Exercício do Cofre
//Nome: Cofre_embarcatech
//Autor: José Adriano Filho
// Abrir cofre: Digite a senha correta
// Simular fechar o cofre: pressione "#" no teclado quando o led verde estiver aceso
// Tempo de bloqueio: 20s
// Ao bloquear o cofre o led vermelho fica aceso direto para indicar esta situação
// O led azul é apenas para indicar que o sistema está ligado.

#include <stdio.h>
#include "pico/stdlib.h"

//Definição do número de linhas e colunas do teclado
#define LINHAS 4
#define COLUNAS 4

//Definição dos pinos dos LEDs
#define LED_VERMELHO_PIN 12
#define LED_VERDE_PIN 11
#define LED_AZUL_PIN 21

//Definição dos pinos dos segmentos e controles dos displays (Dig1, Dig2, Dig3, Dig4)
const uint8_t segmentos_pins[] = {0, 1, 2, 3, 4, 5, 6};
const uint8_t display_pins[] = {7, 8, 9,10};

//Mapeamento dos números de 0 a 9 para os segmentos dos displays (0 = off, 1 = on)
//Display Catodo comun
const uint8_t numeros[10][7] = {
    {1, 1, 1, 1, 1, 1, 0}, //0
    {0, 1, 1, 0, 0, 0, 0}, //1
    {1, 1, 0, 1, 1, 0, 1}, //2
    {1, 1, 1, 1, 0, 0, 1}, //3
    {0, 1, 1, 0, 0, 1, 1}, //4
    {1, 0, 1, 1, 0, 1, 1}, //5
    {1, 0, 1, 1, 1, 1, 1}, //6
    {1, 1, 1, 0, 0, 0, 0}, //7
    {1, 1, 1, 1, 1, 1, 1}, //8
    {1, 1, 1, 1, 0, 1, 1}  //9
};

//Definição dos pinos das linhas e colunas do teclado
const uint8_t teclado_linhas[] = {17, 18, 19, 20};  
const uint8_t teclado_colunas[] = {13, 14, 15, 16};

//Mapeamento dos caracteres do teclado
const char teclado[LINHAS][COLUNAS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

//Definição da senha do cofre
int senha = 1324;

//Protótipo das funções
void init_leds();
void init_display();
void mostra_digito(uint8_t display, uint8_t digito);
void mostra_numero(int numero);
void init_teclado();
char le_teclado();
int le_senha();
void estadoZero(int *numero, int *estado);
void estadoUm(int *estado);
void estadoDois(int *tentativas, int *estado);

//Função principal
int main()
{
    stdio_init_all();
    init_display();
    init_teclado();
    init_leds();

    int numero = 0;
    int estado = 0;
    static int tentativas = 0;

    while (true) {

        switch (estado)
        {
        case 0:
            estadoZero(&numero, &estado);
            break;
        case 1:
            estadoUm(&estado);
            break;
        case 2:
            tentativas++;
            estadoDois(&tentativas, &estado);
            break;
        default:
            break;
        }
    }
}

//Função para o estado 0
void estadoZero(int *numero, int *estado)
{
    *numero = le_senha();
    if (*numero == 1234) 
    {
        *estado = 1;
    } 
    else
    {
        if(*numero == 'A')
        {
            *estado = 3; 
        }
        else
        {
            *estado = 2;
        }
    }
    
}

//Função para o estado 1 
void estadoUm(int *estado)
{
    bool aberto = true;
    while(aberto)
    {
        gpio_put(LED_VERDE_PIN, 1);

        char tecla = le_teclado();
        if (tecla != '\0') 
        {
            if (tecla == '#') 
            {
                aberto = false;
                gpio_put(LED_VERDE_PIN, 0);
                *estado = 0;
                break;
            }
        }
    }
}

void estadoDois(int *tentativas, int *estado)
{
    if (*tentativas == 3) 
    {
        gpio_put(LED_VERMELHO_PIN, 1);
        sleep_ms(10000);
        gpio_put(LED_VERMELHO_PIN, 0);
        *tentativas = 0;
        *estado = 0;
    }
    else
    {
        for(int i = 0; i < 3; i++)
        {
            gpio_put(LED_VERMELHO_PIN, 1);
            sleep_ms(500);
            gpio_put(LED_VERMELHO_PIN, 0);
            sleep_ms(500);
            *estado = 0;
        }
    }
}

//Função para inicializar os LEDs
void init_leds()
{
    gpio_init(LED_VERMELHO_PIN);
    gpio_set_dir(LED_VERMELHO_PIN, GPIO_OUT);
    gpio_put(LED_VERMELHO_PIN, 0);

    gpio_init(LED_VERDE_PIN);
    gpio_set_dir(LED_VERDE_PIN, GPIO_OUT);
    gpio_put(LED_VERDE_PIN, 0);

    gpio_init(LED_AZUL_PIN);
    gpio_set_dir(LED_AZUL_PIN, GPIO_OUT);
    gpio_put(LED_AZUL_PIN, 1);
}

//Função para inicializar os displays
void init_display()
{
    //Configura os pinos dos segmentos como saída
    for (int i = 0; i < 7; i++) {
        gpio_init(segmentos_pins[i]);
        gpio_set_dir(segmentos_pins[i], GPIO_OUT);
    }

    //Configura os pinos dos displays como saída
    for (int i = 0; i < 4; i++) {
        gpio_init(display_pins[i]);
        gpio_set_dir(display_pins[i], GPIO_OUT);
        gpio_put(display_pins[i], 0);    //Desliga todos os displays
    }
}

//Função para mostrar um dígito em um display
void mostra_digito(uint8_t display, uint8_t digito)
{
    //Desliga todos os displays
    //for (int i = 0; i < 4; i++) {
        //gpio_put(display_pins[i], 0);
    //}

    //Configura os segmentos de acordo com o número
    for (int i = 0; i < 7; i++) {
        gpio_put(segmentos_pins[i], numeros[digito][i]);
    }

    //Liga o display
    gpio_put(display_pins[display], 0);
    sleep_ms(5);

    //Desliga o display
    gpio_put(display_pins[display], 1);
}

//Função para mostrar um número nos displays 
void mostra_numero(int numero)
{
    int digitos[4];

    digitos[0] = numero / 1000;
    digitos[1] = (numero % 1000) / 100;
    digitos[2] = (numero % 100) / 10;
    digitos[3] = numero % 10;

    for (int i = 0; i < 4; i++) {
        mostra_digito(i, digitos[i]);
        sleep_ms(5);
    }
}

void init_teclado()
{
    //Configura os pinos das linhas como saída
    for (int i = 0; i < LINHAS; i++) {
        gpio_init(teclado_linhas[i]);
        gpio_set_dir(teclado_linhas[i], GPIO_OUT);
        gpio_put(teclado_linhas[i], 1);
    }

    //Configura os pinos das colunas como entrada
    for (int i = 0; i < COLUNAS; i++) {
        gpio_init(teclado_colunas[i]);
        gpio_set_dir(teclado_colunas[i], GPIO_IN);
        gpio_pull_up(teclado_colunas[i]);
    }
}

char le_teclado()
{
    char tecla = '\0';
    for (int i = 0; i < LINHAS; i++) {
        gpio_put(teclado_linhas[i], 0);
        for (int j = 0; j < COLUNAS; j++) {
            if (gpio_get(teclado_colunas[j]) == 0) {
                tecla = teclado[i][j];
                while (gpio_get(teclado_colunas[j]) == 0);
            }
        }
        gpio_put(teclado_linhas[i], 1);
    }

    return tecla;
}

int le_senha()
{
    int senhaInt = 0;
    char tecla;
    int i = 0;

    while (i < 4) {
        tecla = le_teclado();
        if (tecla != '\0') {
            if (tecla >= '0' && tecla <= '9') {
                senhaInt = senhaInt * 10 + (tecla - '0');
                i++;
            }
        }
        if(i > 0)
        {
            mostra_numero(senhaInt);
        }
        //mostra_numero(senhaInt);
    }
    
    return senhaInt;
}
