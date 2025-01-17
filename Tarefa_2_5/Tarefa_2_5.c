//*******************************************************************************************************************
// Tarefa 2.5 - Unidade 4 - Criado: 17/01/25
// Autor: José Adriano
// Descrição: Controle de led RGB com pwm 
// Led vermelho operando com pwm de 1kHz
// Led azul operando com pwm de 10kHz
// led verde operando com pwm de 4kHz (não foi indicado na questão frequência deste led)
// Duty cycle iniciando em 5% e sendo incrementado de 5%. No valor máximo retorna ao valor inicial
//*******************************************************************************************************************
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// Definição dos pinos dos LEDs
const uint LED_GREEN = 11;                  // Pino do LED verde
const uint LED_RED = 13;                    // Pino do LED vermelho
const uint LED_BLUE = 12;                   // Pino do LED azul

const uint16_t PERIOD_1KHZ = 10000;         // Período do PWM 1kHz (clock base de 10MHz, divisor 10)
const uint16_t PERIOD_4KHZ = 2500;          // Período do PWM 4kHz (clock base de 10MHz, divisor 10)
const uint16_t PERIOD_10KHZ = 1000;         // Período do PWM 10kHz (clock base de 10MHz, divisor 10)
const float DIVIDER_PWM = 10.0;             // Divisor fracional do clock para PWM
const uint16_t DUTY_CYCLE_STEP = 5;         // Passo de incremento/decremento para o duty cycle (5%)
const uint32_t TIME_INTERVAL = 2000;        // Intervalo de tempo em ms para atualização do duty cycle (2 segundos)
const uint16_t MAX_DUTY_CYCLE = 100;        // Duty cycle máximo em % (100%)
volatile uint16_t led_duty_cycle = DUTY_CYCLE_STEP;  // Nível inicial do PWM (duty cycle)

// Protótipo das funções
void setup_pwm();
void pwm_irq_handler();

int main()
{
    stdio_init_all();                       // Inicializa o sistema padrão de I/O
    setup_pwm();                            // Configura o PWM

    while (true)
    {
        sleep_ms(1);                        // Mantém o loop principal ativo
    }
}

void setup_pwm()
{
    uint slice;
    
    // Configura o PWM para cada LED
    // LED verde
    gpio_set_function(LED_GREEN, GPIO_FUNC_PWM);      // Configura o pino do LED verde para a função PWM
    slice = pwm_gpio_to_slice_num(LED_GREEN);         // Obtém o slice do PWM associado ao pino do LED verde
    pwm_set_clkdiv(slice, DIVIDER_PWM);               // Define o divisor do clock do PWM
    pwm_set_wrap(slice, PERIOD_4KHZ);                 // Configura o valor máximo do contador para 4kHz
    pwm_set_gpio_level(LED_GREEN, led_duty_cycle * PERIOD_4KHZ / MAX_DUTY_CYCLE);  // Define o nível inicial do PWM para o pino do LED verde
    pwm_set_enabled(slice, true);                     // Habilita o PWM no slice correspondente

    // LED vermelho
    gpio_set_function(LED_RED, GPIO_FUNC_PWM);        // Configura o pino do LED vermelho para a função PWM
    slice = pwm_gpio_to_slice_num(LED_RED);           // Obtém o slice do PWM associado ao pino do LED vermelho
    pwm_set_clkdiv(slice, DIVIDER_PWM);               // Define o divisor do clock do PWM
    pwm_set_wrap(slice, PERIOD_1KHZ);                 // Configura o valor máximo do contador para 1kHz
    pwm_set_gpio_level(LED_RED, led_duty_cycle * PERIOD_1KHZ / MAX_DUTY_CYCLE);   // Define o nível inicial do PWM para o pino do LED vermelho
    pwm_set_enabled(slice, true);                     // Habilita o PWM no slice correspondente

    // LED azul
    gpio_set_function(LED_BLUE, GPIO_FUNC_PWM);       // Configura o pino do LED azul para a função PWM
    slice = pwm_gpio_to_slice_num(LED_BLUE);          // Obtém o slice do PWM associado ao pino do LED azul
    pwm_set_clkdiv(slice, DIVIDER_PWM);               // Define o divisor do clock do PWM
    pwm_set_wrap(slice, PERIOD_10KHZ);                // Configura o valor máximo do contador para 10kHz
    pwm_set_gpio_level(LED_BLUE, led_duty_cycle * PERIOD_10KHZ / MAX_DUTY_CYCLE); // Define o nível inicial do PWM para o pino do LED azul
    pwm_set_enabled(slice, true);                     // Habilita o PWM no slice correspondente

    // Configura e habilita a interrupção do PWM
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_irq_handler);   // Define o handler da interrupção
    pwm_clear_irq(slice);                              // Limpa interrupções pendentes do slice
    pwm_set_irq_enabled(slice, true);                  // Habilita interrupções para o slice do PWM
    irq_set_enabled(PWM_IRQ_WRAP, true);               // Habilita interrupções globais para o PWM
}

void pwm_irq_handler()
{
    static uint32_t ms_counter = 0;                     // Nova variável para contar os milissegundos
    uint32_t irq_status = pwm_get_irq_status_mask();    // Obtém o status da interrupção do PWM
    pwm_clear_irq(irq_status);                          // Limpa a interrupção do slice correspondente

    if (ms_counter < TIME_INTERVAL) {
        ms_counter++;
        return;
    }
    ms_counter = 0;                                 // Reinicia o contador de milissegundos

    led_duty_cycle += DUTY_CYCLE_STEP;              // Incremento de 5%
    if (led_duty_cycle > MAX_DUTY_CYCLE) {
        led_duty_cycle = DUTY_CYCLE_STEP;           // Volta a 5%
    }

    // Atualiza o nível do PWM para os LEDs
    pwm_set_gpio_level(LED_GREEN, led_duty_cycle * PERIOD_4KHZ / MAX_DUTY_CYCLE);
    pwm_set_gpio_level(LED_RED, led_duty_cycle * PERIOD_1KHZ / MAX_DUTY_CYCLE);
    pwm_set_gpio_level(LED_BLUE, led_duty_cycle * PERIOD_10KHZ / MAX_DUTY_CYCLE);
}
