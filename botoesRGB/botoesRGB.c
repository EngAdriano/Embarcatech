#include <stdio.h>
#include "pico/stdlib.h"

#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12
#define BTN_A_PIN 5
#define BTN_B_PIN 6

//Protótipo da função
void set_leds(bool red, bool green, bool blue);

int main()
{
    stdio_init_all();

    //Inicializa os pinos dos LEDs
    gpio_init(LED_R_PIN);
    gpio_init(LED_G_PIN);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);

    //Inicializa os pinos dos botões
    gpio_init(BTN_A_PIN);
    gpio_init(BTN_B_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_set_dir(BTN_B_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);
    gpio_pull_up(BTN_B_PIN);

    while (true) 
    {
        //printf("Hello, world!\n");
        //sleep_ms(1000);
        if ((!gpio_get(BTN_A_PIN)) && (!gpio_get(BTN_B_PIN))) 
        {
            set_leds(true, true, true);     //Todos os LEDs acesos

        } else if (!gpio_get(BTN_A_PIN)) 
        {
            set_leds(true, false, false);   //Apenas o LED vermelho aceso

        } else if(!gpio_get(BTN_B_PIN)) 
        {
            set_leds(false, true, false);   //Apenas o LED verde aceso

        } else
        {
            set_leds(false, false, true);   //Apenas o LED azul aceso
        }
    }
}

void set_leds(bool red, bool green, bool blue)
{
    gpio_put(LED_R_PIN, red);
    gpio_put(LED_G_PIN, green);
    gpio_put(LED_B_PIN, blue);
}