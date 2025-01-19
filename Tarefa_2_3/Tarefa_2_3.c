#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

// Definições dos pinos de UART0 e UART1
#define UART0_TX_PIN 0
#define UART0_RX_PIN 1
#define UART1_TX_PIN 4
#define UART1_RX_PIN 5

void uartTeste(void);

int main() 
{
    // Inicializa a UART padrão e as outras UARTs
    stdio_init_all();
    
    uart_init(uart0, 115200);
    uart_init(uart1, 115200);

    // Configura os pinos para UART0 e UART1
    gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART1_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART1_RX_PIN, GPIO_FUNC_UART);

  while(true)
  {
    uartTeste();
  }
    
    return 0;
}

void uartTeste(void)
{
  char input;
  printf("Digite um caractere: ");

  // Lê o dado do console
  scanf(" %c", &input);

  // Limpa o buffer da UART1
  while (uart_is_readable(uart1)) 
  {
      uart_getc(uart1);
  }

  // Transmite o dado da UART0 para a UART1
  //uart_putc_raw(uart0, input);
  printf("%c", input);

  sleep_ms(100); // Adiciona um pequeno delay para garantir que o dado seja transmitido

  // Recebe o dado da UART1
  char received = uart_getc(uart1);

  // Envia o dado recebido para o console
  printf("\nDado recebido na UART1: %c\n", received);
}
