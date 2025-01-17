#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"

// Pinos dos LEDs
#define LED_RED_PIN 16
#define LED_BLUE_PIN 17

// Duty Cycle inicial do LED vermelho
volatile uint16_t duty_cycle_red = 5; 

void pwm_irq_handler() {
    uint slice_num = pwm_gpio_to_slice_num(LED_RED_PIN);
    pwm_clear_irq(slice_num);

    duty_cycle_red += 5;
    if (duty_cycle_red > 100) {
        duty_cycle_red = 5;
    }

    pwm_set_gpio_level(LED_RED_PIN, (duty_cycle_red * 24999) / 100);
}

void init_pwm_red() {
    gpio_set_function(LED_RED_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(LED_RED_PIN);
    pwm_set_wrap(slice_num, 24999); // 1kHz com clock de 125MHz (125MHz/1kHz - 1)
    pwm_set_irq_enabled(slice_num, true);
    irq_set_enabled(PWM_IRQ_WRAP, true);
    pwm_set_enabled(slice_num, true);
}

void init_pwm_blue() {
    gpio_set_function(LED_BLUE_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(LED_BLUE_PIN);
    pwm_set_wrap(slice_num, 1249); // 10kHz com clock de 125MHz (125MHz/10kHz - 1)
    pwm_set_enabled(slice_num, true);
}

int main() {
    stdio_init_all();

    // Inicia os PWM dos LEDs
    init_pwm_red();
    init_pwm_blue();

    // Define o duty cycle inicial do LED azul
    uint slice_num_blue = pwm_gpio_to_slice_num(LED_BLUE_PIN);
    pwm_set_gpio_level(LED_BLUE_PIN, (50 * 1249) / 100); // Duty cycle de 50%

    // Cria um alarme que chama pwm_irq_handler a cada 2 segundos
    add_repeating_timer_ms(2000, pwm_irq_handler, NULL, NULL);

    while (1) {
        tight_loop_contents();
    }

    return 0;
}
