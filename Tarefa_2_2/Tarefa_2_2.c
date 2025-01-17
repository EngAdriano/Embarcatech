//*******************************************************************************************************************
// Criado: 13/01/25
// Autor: José Adriano
// Descrição: Programa que pisca um LED a cada 5 vezes que um botão é pressionado. 
// O LED pisca a uma frequência de 10Hz e duração de 10s.
// O botão B alterna a frequência de 10Hz para 1Hz e vice versa
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
volatile bool flag_botao_B = true;
volatile uint16_t frequencia = 100; // Para frequência de 10Hz
volatile uint8_t tempo_relativo = 200; // No cálculo interno vai equivaler a 10s

//Protótipos de funções
void init_gpio();
bool monitora_botao_A_callback(struct repeating_timer *t);
bool pisca_led_callback(struct repeating_timer *t);
bool monitora_botao_B_callback(struct repeating_timer *t);


int main()
{
    stdio_init_all();
    init_gpio();

    struct repeating_timer timer;
    add_repeating_timer_ms(100, monitora_botao_A_callback, NULL, &timer);
    struct repeating_timer timer_bota_B;
    add_repeating_timer_ms(100, monitora_botao_B_callback, NULL, &timer_bota_B);

    printf("Tarefa 2.2 - Unidade 4\n");
    printf("Pressione o botão verde 5 vezes para iniciar os pulsos\n");
    printf("Pressione o botão vermelho para mudar a frequência\n");


    while (true) 
    {
        if(!flag_monitora_botao)
        {
            add_repeating_timer_ms(100, monitora_botao_A_callback, NULL, &timer);
            flag_monitora_botao = true;
            flag_botao_B = true;
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

}

// Função para monitorar o botão
bool monitora_botao_A_callback(struct repeating_timer *t) 
{
    static absolute_time_t Ultima_vez_pressionado = 0;
    bool botao_ultimo_estado = gpio_get(BOTAO_A_PIN);
    bool btn_estado = !gpio_get(BOTAO_A_PIN);

    if(btn_estado && !botao_ultimo_estado && absolute_time_diff_us(Ultima_vez_pressionado, get_absolute_time()) > 200000) // Debounce de 200ms
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
                flag_botao_B = false;
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

bool monitora_botao_B_callback(struct repeating_timer *t)
{
    static absolute_time_t Ultima_vez = 0;
    bool botao_ultimo_estado_B = gpio_get(BOTAO_B_PIN);
    bool btn_estado_B = !gpio_get(BOTAO_B_PIN);
    static bool flag_botao = true;

    if(flag_botao_B)
    {
        if(btn_estado_B && !botao_ultimo_estado_B && absolute_time_diff_us(Ultima_vez, get_absolute_time()) > 200000) // Debounce de 200ms
        {
            Ultima_vez = get_absolute_time();
          if(btn_estado_B && !led_piscando)
          {
            printf("Botão B pressionado\n");
            if(flag_botao)
            {
                frequencia = 1000;
                tempo_relativo = 20;
                flag_botao = false;
            }
            else
            {
                frequencia = 100;
                tempo_relativo = 200;
                flag_botao = true;
            }
          }
        }
    }
    return true;
}

// Função para piscar o LED
bool pisca_led_callback(struct repeating_timer *t)
{
    static int contador = 0;
    static bool led_ligado = false;
    static int tempoContado = 0;
    int resto = 0;
    float freq = 0.0;

    led_ligado = !led_ligado;
    gpio_put(LED_PIN, led_ligado);

    resto = contador % 2;
    
    if(resto == 0)
    {
        printf("Pulsos: %d\n", (tempoContado + 1));
        tempoContado++;
    }

    contador++;

    if(contador == tempo_relativo)
    {
        freq = ((float)contador/2) / 10;
        printf("Frequência: %.2f Hz\n", freq);
        printf("Pressione o botão verde 5 vezes para iniciar os pulsos\n");
        printf("Pressione o botão vermelho para mudar a frequência\n");
        cancel_repeating_timer(t);
        led_piscando = false;
        gpio_put(LED_PIN, false);
        contador = 0;
        tempoContado = 0;
        flag_monitora_botao = false;
        return false;
    }
    return true;
}
