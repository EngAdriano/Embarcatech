#include <stdio.h>
#include "pico/stdlib.h"

const uint led_pin_red = 12;
const uint button_b = 6;

static volatile uint a = 1;

//protótipo da função
static void gpio_irq_handler(uint gpio, uint32_t events);


int main()
{
    stdio_init_all();
    gpio_init(led_pin_red);
    gpio_set_dir(led_pin_red, GPIO_OUT);
    gpio_init(button_b);
    gpio_set_dir(button_b, GPIO_IN);
    gpio_pull_up(button_b);
    gpio_set_irq_enabled_with_callback(button_b, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}

//função de interrupção	
static void gpio_irq_handler(uint gpio, uint32_t events)
{
    a++;
    if(a % 2 == 0)
    {
        gpio_put(led_pin_red, 1);
    }
    else
    {
        gpio_put(led_pin_red, 0);
    }
}
