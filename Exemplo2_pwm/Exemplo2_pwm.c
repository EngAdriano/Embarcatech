#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

const uint LED = 12;                        //Pino do led
const uint16_t PERIOD = 2000;               //Período do pwm (valor máximo do contador)
const float DIVIDER_PWM = 16.0;             //Divisor fracional do clock para PWM
const uint16_t LED_STEP = 100;              //Passo de incremento/decremento para o duty cycle do LED
const uint32_t PWM_REFRESH_LEVEL = 40000;   //Nível de contagem para suavizar o fade do LED
uint16_t led_level = 100;                   //Nível inicial do PWM (duty cycle)

//Protótipo das funções
void setup_pwm();
void pwm_irq_handler();

int main()
{
    stdio_init_all();                       //Iniciliza o sistema padrão de I/O
    setup_pwm();                            //Configura o PWM

    while (true) 
    {
        sleep_ms(1);    
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

    //Configura e habilita a interrupção do PWM
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_irq_handler);   //Define o handler da interrupção
    pwm_clear_irq(slice);                       //Limpa interrupções pendentes do slice
    pwm_set_irq_enabled(slice, true);           //Habilita interrupções para o slice do PWM
    irq_set_enabled(PWM_IRQ_WRAP, true);        //Habilita interrupções globais para o PWM

}

void pwm_irq_handler()
{
    static uint up_down = 1;                //Variável para controlar se o nível do LED aumenta ou diminui
    static uint32_t count = 0;              //Contador para controlar a frequência de atualização do
    uint32_t slice = pwm_get_irq_status_mask(); //Obtém o status da interrupção do PWM
    pwm_clear_irq(slice);                   //Limpa a interrupção do slice correspondente
    if(count++ < PWM_REFRESH_LEVEL)         //Verifica se o contador atingiu o nível
    {
        return;
    }

    count = 0;      //Ajusta o contador para iniciar a próxima verificação

    if(up_down)     //Ajusta o duty cycle do LED, alternando entre aumentar e diminuir
        {
            led_level += LED_STEP;          //Incrementa o nível do LED
            if(led_level >= PERIOD)
            {
                up_down = 0;                //Muda direção para diminuir quando atingir o período máximo
            }
        }
        else
        {
            led_level -= LED_STEP;          //Decrementa o nível do LED
            if(led_level <= LED_STEP)
            {
                up_down = 1;                //Muda direção para aumentar quando atingir o período mínimo
            }
        }

        pwm_set_gpio_level(LED, led_level);
}
