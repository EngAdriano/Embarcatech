#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"


//Definições de variáveis
const uint LED_PIN = 13;
const uint BOTAO_PIN = 5;
volatile bool botao_pressionado = false;
volatile bool led_piscando = false;
volatile int btn_contador = 0;
volatile bool flag_monitora_botao = true;
volatile uint16_t frequencia = 100;
volatile uint8_t tempo = 200;

//Protótipos de funções
void init_gpio();
int64_t alarme_callback(alarm_id_t id, void *user_data);
bool monitora_botao_callback(struct repeating_timer *t);
bool pisca_led_callback(struct repeating_timer *t);


int main()
{
    stdio_init_all();
    init_gpio();

    struct repeating_timer timer;
    add_repeating_timer_ms(100, monitora_botao_callback, NULL, &timer);

    while (true) 
    {
        sleep_ms(1);

        if(!flag_monitora_botao)
        {
            add_repeating_timer_ms(100, monitora_botao_callback, NULL, &timer);
            flag_monitora_botao = true;
        }
    }
}

//Área de funções

void init_gpio()
{
    // Configuração do LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Configuração do botão
    gpio_init(BOTAO_PIN);
    gpio_set_dir(BOTAO_PIN, GPIO_IN);
    gpio_pull_up(BOTAO_PIN);
}

int64_t alarme_callback(alarm_id_t id, void *user_data) 
{
    printf("Alarme disparado!\n");
    return 0;
}

bool monitora_botao_callback(struct repeating_timer *t) 
{
    static absolute_time_t Ultima_vez_pressionado = 0;
    bool botao_ultimo_estado = gpio_get(BOTAO_PIN);

    bool btn_estado = !gpio_get(BOTAO_PIN);

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

bool pisca_led_callback(struct repeating_timer *t)
{
    static int contador = 0;
    static bool led_ligado = false;
    led_ligado = !led_ligado;
    gpio_put(LED_PIN, led_ligado);

    printf("LED %s\n", led_ligado ? "ligado" : "desligado");
    contador++;

    if(contador == tempo)
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
