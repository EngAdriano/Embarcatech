#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

const uint LED = 12;            //Pino do led
const uint16_t PERIOD = 2000;   //Período do pwm (valor máximo do contador)
const float DIVIDER_PWM = 16.0; //Divisor fracional do clock para PWM
const uint16_t LED_STEP = 100;  //Passo de incremento/decremento para o duty cycle do LED
uint16_t led_level = 100;       //Nível inicial do PWM (duty cycle)

//Protótipo das funções
void setup_pwm();

int main()
{
    uint up_down = 1;   //Variável para controlar se o nível do LED aumenta ou diminui
    stdio_init_all();   //Iniciliza o sistema padrão de I/O
    setup_pwm();    //Configura o PWM

    while (true) {
        pwm_set_gpio_level(LED, led_level);     //Define o nível atual do PWM
        sleep_ms(100);     //Atraso de 1 segundo

        if(up_down)
        {
            led_level += LED_STEP;      //Incrementa o nível do LED
            if(led_level >= PERIOD)
            {
                up_down = 0;    //Muda direção para diminuir quando atingir o período máximo
                sleep_ms(100);     //Atraso de 100 ms
            }
        }
        else
        {
            led_level -= LED_STEP;      //Decrementa o nível do LED
            if(led_level <= LED_STEP)
            {
                up_down = 1;    //Muda direção para aumentar quando atingir o período mínimo
                sleep_ms(100);     //Atraso de 100 ms
            }
        }
    }
}

void setup_pwm()
{
    uint slice;
    gpio_set_function(LED, GPIO_FUNC_PWM);      //Configura o pino do LED para a função PWM
    slice = pwm_gpio_to_slice_num(LED);         //Obtém o slice do PWM associado aopino do LED
    pwm_set_clkdiv(slice, DIVIDER_PWM);         //Define o divisor do clock do PWM
    pwm_set_wrap(slice, PERIOD);                //Configura o valor máximo do contador
    pwm_set_gpio_level(LED, led_level);         //Define o nível inicial do PWM para o pino do LED
    pwm_set_enabled(slice, true);               //Habilita o PWM no slice correspondente
}
