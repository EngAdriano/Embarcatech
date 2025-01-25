#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// Define os pinos dos botões
#define BUTTON1_GPIO 14
#define BUTTON2_GPIO 15
#define BUTTON3_GPIO 16
#define BUTTON4_GPIO 17
#define BUTTON5_GPIO 18
#define BUTTON6_GPIO 19

// Constante de debounce (em milissegundos)
#define DEBOUNCE_DELAY 50

// Variáveis de estado dos botões
volatile bool button1_pressed = false;
volatile bool button2_pressed = false;
volatile bool button3_pressed = false;
volatile bool button4_pressed = false;
volatile bool button5_pressed = false;
volatile bool button6_pressed = false;

// Temporizadores de debounce
absolute_time_t last_press_button1;
absolute_time_t last_press_button2;
absolute_time_t last_press_button3;
absolute_time_t last_press_button4;
absolute_time_t last_press_button5;
absolute_time_t last_press_button6;

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

    // Verifica o estado do botão 3
    if (!gpio_get(BUTTON3_GPIO) && absolute_time_diff_us(last_press_button3, get_absolute_time()) >= DEBOUNCE_DELAY * 1000) {
        button3_pressed = true;
        last_press_button3 = get_absolute_time();
    }

    // Verifica o estado do botão 4
    if (!gpio_get(BUTTON4_GPIO) && absolute_time_diff_us(last_press_button4, get_absolute_time()) >= DEBOUNCE_DELAY * 1000) {
        button4_pressed = true;
        last_press_button4 = get_absolute_time();
    }

    // Verifica o estado do botão 5
    if (!gpio_get(BUTTON5_GPIO) && absolute_time_diff_us(last_press_button5, get_absolute_time()) >= DEBOUNCE_DELAY * 1000) {
        button5_pressed = true;
        last_press_button5 = get_absolute_time();
    }

    // Verifica o estado do botão 6
    if (!gpio_get(BUTTON6_GPIO) && absolute_time_diff_us(last_press_button6, get_absolute_time()) >= DEBOUNCE_DELAY * 1000) {
        button6_pressed = true;
        last_press_button6 = get_absolute_time();
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

    gpio_init(BUTTON3_GPIO);
    gpio_set_dir(BUTTON3_GPIO, GPIO_IN);
    gpio_pull_up(BUTTON3_GPIO);

    gpio_init(BUTTON4_GPIO);
    gpio_set_dir(BUTTON4_GPIO, GPIO_IN);
    gpio_pull_up(BUTTON4_GPIO);

    gpio_init(BUTTON5_GPIO);
    gpio_set_dir(BUTTON5_GPIO, GPIO_IN);
    gpio_pull_up(BUTTON5_GPIO);

    gpio_init(BUTTON6_GPIO);
    gpio_set_dir(BUTTON6_GPIO, GPIO_IN);
    gpio_pull_up(BUTTON6_GPIO);

    // Inicializa os temporizadores de debounce
    last_press_button1 = get_absolute_time();
    last_press_button2 = get_absolute_time();
    last_press_button3 = get_absolute_time();
    last_press_button4 = get_absolute_time();
    last_press_button5 = get_absolute_time();
    last_press_button6 = get_absolute_time();

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

        if (button3_pressed) {
            printf("Botão 3 pressionado!\n");
            button3_pressed = false; // Reseta o estado do botão
        }

        if (button4_pressed) {
            printf("Botão 4 pressionado!\n");
            button4_pressed = false; // Reseta o estado do botão
        }

        if (button5_pressed) {
            printf("Botão 5 pressionado!\n");
            button5_pressed = false; // Reseta o estado do botão
        }

        if (button6_pressed) {
            printf("Botão 6 pressionado!\n");
            button6_pressed = false; // Reseta o estado do botão
        }

        sleep_ms(10); // Aguarda um breve momento
    }

    return 0;
}
