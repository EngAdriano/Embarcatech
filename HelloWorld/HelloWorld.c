#include <stdio.h>
#include "pico/stdlib.h"

#define LED_PIN 11
#define BTN_PIN 5

int main()
{
    stdio_init_all();

    //Inicializa o pino do LED e comfigura como saída
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    //Inicializa o pino do botão e configura como entrada
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);

    //Loop infinito
    while (true) {
        printf("Alô Mundo!\n");
        gpio_put(LED_PIN, 1);
        sleep_ms(1000);
        gpio_put(LED_PIN, 0);
        sleep_ms(1000);
    }
}
