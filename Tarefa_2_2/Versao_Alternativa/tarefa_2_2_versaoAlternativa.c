//*******************************************************************************************************************
// Criado: 13/01/25
// Autor: José Adriano
// Descrição: Programa que pisca um LED a cada 5 vezes que um botão é pressionado. 
// O LED pisca a uma frequência de 10Hz e duração de 10s.
//*******************************************************************************************************************

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"


//Definições de variáveis
const uint LED_PIN = 13;
const uint BOTAO_A_PIN = 5;
const uint BOTAO_B_PIN = 6;
volatile bool botao_pressionado = false;
volatile bool led_piscando = false;
volatile int btn_contador = 0;
volatile bool flag_monitora_botao = true;
volatile uint16_t frequencia = 100; //frequência de 10Hz
volatile uint8_t tempo_relativo = 200; // No cálculo interno va equivaler a 10s
bool ultimo_estado = true;

//Protótipos de funções
void init_gpio();
bool monitora_botao_callback(struct repeating_timer *t);
bool pisca_led_callback(struct repeating_timer *t);
void monitora_botao_B(void);

int main()
{
    stdio_init_all();
    init_gpio();

    struct repeating_timer timer;
    add_repeating_timer_ms(100, monitora_botao_callback, NULL, &timer);

    while (true) 
    {
        if(!led_piscando)
        {
            monitora_botao_B();
        }

        if(!flag_monitora_botao)
        {
            add_repeating_timer_ms(100, monitora_botao_callback, NULL, &timer);
            flag_monitora_botao = true;
        }

        sleep_ms(1);
    }
}


//********************************************************************************************************************
// Funções
//********************************************************************************************************************

// Função para inicializar os pinos
void init_gpio()
{
    // Configuração do LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Configuração do botão
    gpio_init(BOTAO_A_PIN);
    gpio_set_dir(BOTAO_A_PIN, GPIO_IN);
    gpio_pull_up(BOTAO_A_PIN);

    gpio_init(BOTAO_B_PIN);
    gpio_set_dir(BOTAO_B_PIN, GPIO_IN);
    gpio_pull_up(BOTAO_B_PIN);
    ultimo_estado = gpio_get(BOTAO_B_PIN);
}

// Função para monitorar o botão
bool monitora_botao_callback(struct repeating_timer *t) 
{
    static absolute_time_t Ultima_vez_pressionado = 0;
    bool botao_ultimo_estado = gpio_get(BOTAO_A_PIN);
    bool btn_estado = !gpio_get(BOTAO_A_PIN);

    if(btn_estado && !botao_ultimo_estado && absolute_time_diff_us(Ultima_vez_pressionado, get_absolute_time()) > 200000) 
    {
        botao_pressionado = true;
        btn_contador++;
        printf("Botão pressionado %d vezes\n", btn_contador);
       
        Ultima_vez_pressionado = get_absolute_time();

        if(btn_contador == 5)
        {
            printf("pressionado 5 vezes\n");
            btn_contador = 0;
            if(led_piscando)
            {
                cancel_repeating_timer(t);
                led_piscando = false;
                gpio_put(LED_PIN, false);
            }
            else
            {
                add_repeating_timer_ms(frequencia, pisca_led_callback, NULL, t);
                led_piscando = true;
            }
        }
    }
    else
    {
        if(!botao_pressionado)
        {
            botao_pressionado = false;
        }
    }

    return true;
}

void monitora_botao_B(void)
{
    static absolute_time_t Ultima_vez = 0;
    static bool flag_B = true;
    bool btn_estado = gpio_get(BOTAO_B_PIN);
    int t;

    if(btn_estado != ultimo_estado) 
    {
        ultimo_estado = btn_estado;
         for(t = 0; t < 100; t++)
         {
            if(ultimo_estado != gpio_get(BOTAO_B_PIN))
            {
                t = 0;
            }
         }

        if(ultimo_estado)
        {
            printf("Botão B pressionado\n");

            if(flag_B)
            {
                flag_B = false;
                frequencia = 100; //frequência de 1Hz
                tempo_relativo = 200; // No cálculo interno va equivaler a 50s
            }
            else
            {
                flag_B = true;
                frequencia = 1000; //frequência de 10Hz
                tempo_relativo = 20; // No cálculo interno va equivaler a 10s
            } 
        }
        
    }
}

// Função para piscar o LED
bool pisca_led_callback(struct repeating_timer *t)
{
    static int contador = 0;
    static bool led_ligado = false;
    led_ligado = !led_ligado;
    gpio_put(LED_PIN, led_ligado);

    printf("LED %s\n", led_ligado ? "ligado" : "desligado");
    contador++;

    if(contador == tempo_relativo)
    {
        cancel_repeating_timer(t);
        led_piscando = false;
        gpio_put(LED_PIN, false);
        contador = 0;
        flag_monitora_botao = false;
        return false;
    }
    return true;
}
