#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// Define os pinos dos botões
#define BUTTON1_GPIO 14
#define BUTTON2_GPIO 15

// Constante de debounce (em milissegundos)
#define DEBOUNCE_DELAY 50

// Variáveis de estado dos botões
volatile bool button1_pressed = false;
volatile bool button2_pressed = false;

// Temporizadores de debounce
absolute_time_t last_press_button1;
absolute_time_t last_press_button2;

// Função de callback para o timer
bool repeating_timer_callback(struct repeating_timer *t) {
    // Verifica o estado do botão 1
    if (!gpio_get(BUTTON1_GPIO) && absolute_time_diff_us(last_press_button1, get_absolute_time()) >= DEBOUNCE_DELAY * 1000) {
        button1_pressed = true;
        last_press_button1 = get_absolute_time();
    }

    // Verifica o estado do botão 2
    if (!gpio_get(BUTTON2_GPIO) && absolute_time_diff_us(last_press_button2, get_absolute_time()) >= DEBOUNCE_DELAY * 1000) {
        button2_pressed = true;
        last_press_button2 = get_absolute_time();
    }

    return true; // Continue chamando o timer
}

int main() {
    stdio_init_all();
    
    // Configura os pinos dos botões como entrada com pull-up
    gpio_init(BUTTON1_GPIO);
    gpio_set_dir(BUTTON1_GPIO, GPIO_IN);
    gpio_pull_up(BUTTON1_GPIO);

    gpio_init(BUTTON2_GPIO);
    gpio_set_dir(BUTTON2_GPIO, GPIO_IN);
    gpio_pull_up(BUTTON2_GPIO);

    // Inicializa os temporizadores de debounce
    last_press_button1 = get_absolute_time();
    last_press_button2 = get_absolute_time();

    // Configura o timer
    struct repeating_timer timer;
    add_repeating_timer_ms(100, repeating_timer_callback, NULL, &timer); // Varre a cada 100 ms

    // Loop principal
    while (1) {
        if (button1_pressed) {
            printf("Botão 1 pressionado!\n");
            button1_pressed = false; // Reseta o estado do botão
        }

        if (button2_pressed) {
            printf("Botão 2 pressionado!\n");
            button2_pressed = false; // Reseta o estado do botão
        }

        sleep_ms(10); // Aguarda um breve momento
    }

    return 0;
}
