//*******************************************************************************************************************
// Tarefa 2.3 - Unidade 4 - Criado: 12/01/25
// Autor: José Adriano
// Descrição: Programa para enviar dados entre UART0 e UART1 
// Envio de caracteres entre uart0 e uart1 mostrando no console
//*******************************************************************************************************************
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

// Definições dos pinos de UART0 e UART1
#define UART0_TX_PIN 0
#define UART0_RX_PIN 1
#define UART1_TX_PIN 4
#define UART1_RX_PIN 5

#define UART_0 uart0
#define UART_1 uart1

void uartTeste(void);

int main() 
{
    // Inicializa a UART padrão e as outras UARTs
    stdio_init_all();

    //Inicializa a velocidade de comunicação das duas UARTs
    uart_init(UART_0, 115200);
    uart_init(UART_1, 115200);

    // Configura os pinos para UART0 e UART1
    gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART1_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART1_RX_PIN, GPIO_FUNC_UART);

  while(true)
  {
    //funçao para executar a tarefa
    uartTeste();
  }
    
    return 0;
}

void uartTeste(void)
{
  //Caracter de entrada
  char input;
  printf("Digite um caractere: ");

  // Lê o dado do console
  scanf(" %c", &input);

  // Limpa o buffer da UART1
  while (uart_is_readable(UART_1)) 
  {
      uart_getc(UART_1);
  }

  // Transmite o dado da UART0 para a UART1
  //uart_putc_raw(UART_0, input);
  printf("%c", input);

  // Adiciona um pequeno delay para garantir que o dado seja transmitido
  sleep_ms(100); 

  // Recebe o dado da UART1
  char received = uart_getc(UART_1);

  // Envia o dado recebido para o console
  printf("\nDado recebido na UART1: %c\n", received);
}
