#include <stdio.h>
#include "pico/stdlib.h"

// Definição dos pinos TX e RX para UART0 e UART1
#define UART0_TX_PIN 0  // UART0 TX
#define UART0_RX_PIN 1  // UART0 RX
#define UART1_TX_PIN 4  // UART1 TX
#define UART1_RX_PIN 5  // UART1 RX

void config_uart() {
  // Configura a UART0
  uart_init(uart0, 9600);  // Inicializa UART0 a 9600 baud
  gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART0_RX_PIN, GPIO_FUNC_UART);

  // Configura a UART1
  uart_init(uart1, 9600);  // Inicializa UART1 a 9600 baud
  gpio_set_function(UART1_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART1_RX_PIN, GPIO_FUNC_UART);
}

int main() {
  // Inicialização do sistema
  stdio_init_all();

  // Configura UART's
  config_uart();

  printf("Tarefa EMBARCATECH - Unidade 4.\n");
  printf("Sistema inicializado! Digite algo no console:\n");

  char input[100];  // Buffer para armazenar a entrada do console

  while (true) {
    // Substitui scanf para poder ler uma linha inteira do console (inclui espaços)
    if (fgets(input, sizeof(input), stdin) != NULL) {
      printf("Você digitou: %s", input);

      // Envia o dado via UART0 para UART1
      for (int i = 0; input[i] != '\0'; i++) {
        uart_putc(uart0, input[i]);  // Envia pela UART0
      }

      printf("Dados transmitidos via UART0 para UART1.\n");

      // Lê o dado de volta da UART1
      printf("Recebendo dados da UART1...\n");
      while (uart_is_readable(uart1)) {
        char c = uart_getc(uart1);  // Recebe da UART1
        printf("%c", c);           // Imprime no console
      }
      printf("\n");
    }

    sleep_ms(100);  // Para evitar loop muito rápido
  }

  return 0;
}