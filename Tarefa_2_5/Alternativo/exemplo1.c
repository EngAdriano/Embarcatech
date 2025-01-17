
/*
Modifique o programa para controlar os três LEDs RGB da placa BitDogLab usando o
módulo PWM e interrupções, seguindo as orientações a seguir:
O LED_R_PIN deve ser acionado com um PWM de 1kHz e o duty cycle deve ser iniciado em 5% e atualizado a cada 2 segundos
em incrementos de 5%. Quando atingir o valor máximo, deve retornar a 5%.
O LED_B_PIN deve ser acionado com um PWM de 10kHz.
O LED_G_PIN deve permanecer do mesmo jeito
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// Definições de constantes para controle do PWM
const uint LED_R_PIN = 13;                  // Pino do LED vermelho
const uint LED_G_PIN = 11;                  // Pino do LED verde
const uint LED_B_PIN = 12;                  // Pino do LED azul

// Configurações do PWM para cada LED
const uint16_t PERIOD_R = 1000;             // Período do PWM para 1 kHz
const uint16_t PERIOD_B = 100;              // Período do PWM para 10 kHz
const uint16_t PERIOD_G = 2000;             // Período do PWM para a configuração atual do verde

const float DIVIDER_PWM_RB = 16.0;          // Divisor fracional do clock para PWM
const uint16_t LED_R_STEP = 50;             // Incremento de 5% do duty cycle (50 de 1000)
const uint32_t PWM_REFRESH_INTERVAL_MS = 2000; // 2 segundos para atualizar o duty cycle do vermelho

uint16_t red_led_level = 50;                // Duty cycle inicial do LED vermelho (5% de 1000)
uint16_t blue_led_level = 100;              // Duty cycle inicial do LED azul (100%)

// Protótipos das funções
void setup_pwm(uint pin, uint16_t period, float divider, uint16_t initial_level);
void pwm_irq_handler();

// Configura PWM para um pino específico do LED com nível inicial e divisor de clock
void setup_pwm(uint pin, uint16_t period, float divider, uint16_t initial_level) {
    uint slice = pwm_gpio_to_slice_num(pin);
    gpio_set_function(pin, GPIO_FUNC_PWM);  
    pwm_set_clkdiv(slice, divider);    
    pwm_set_wrap(slice, period);           
    pwm_set_gpio_level(pin, initial_level);
    pwm_set_enabled(slice, true);           
    
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_irq_handler);
    pwm_clear_irq(slice);
    pwm_set_irq_enabled(slice, true);
    irq_set_enabled(PWM_IRQ_WRAP, true);
}

// Manipulador de interrupções PWM para atualizar o duty cycle
void pwm_irq_handler() {
    static uint32_t count = 0;
    uint32_t slice = pwm_get_irq_status_mask();
    pwm_clear_irq(slice);

    if (++count < PWM_REFRESH_INTERVAL_MS) return;

    count = 0;  // Reseta o contador para o próximo intervalo

    red_led_level += LED_R_STEP;
    if (red_led_level > PERIOD_R) {
        red_led_level = 50;  // Reseta para 5% de duty cycle ao passar de 100%
    }

    pwm_set_gpio_level(LED_R_PIN, red_led_level);
}

int main() {
    stdio_init_all();  // Inicializa I/O padrão
    setup_pwm(LED_R_PIN, PERIOD_R, DIVIDER_PWM_RB, red_led_level);  // Configura PWM para o LED vermelho
    setup_pwm(LED_G_PIN, PERIOD_G, DIVIDER_PWM_RB, 100);            // Configura PWM para o LED verde como antes
    setup_pwm(LED_B_PIN, PERIOD_B, DIVIDER_PWM_RB, blue_led_level); // Configura PWM para o LED azul, 10 kHz, inicial 100%

    while (true) {
        sleep_ms(1);  // Loop principal para evitar uso excessivo da CPU
    }
}
