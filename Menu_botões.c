#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>

// Definições do display
#define I2C_PORT i2c0
#define LCD_ADDR 0x27
#define LCD_ROWS 2
#define LCD_COLS 16

// Definições dos botões
#define BUTTON_NEXT 14
#define BUTTON_PREV 15
#define BUTTON_SELECT 16

// Funções do display LCD
void lcd_send_cmd(uint8_t cmd);
void lcd_send_data(uint8_t data);
void lcd_init();
void lcd_clear();
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print(const char* str);

// Funções do menu
void show_main_menu();
void show_sub_menu();
void update_menu();

// Variáveis do menu
enum Menu {MAIN_MENU, SUB_MENU} current_menu = MAIN_MENU;
uint8_t current_option = 0;

// Main
int main() {
    stdio_init_all();
    i2c_init(I2C_PORT, 100 * 1000);  // Inicializa I2C no Raspberry Pi Pico
    lcd_init();

    // Inicializa os botões
    gpio_init(BUTTON_NEXT);
    gpio_set_dir(BUTTON_NEXT, GPIO_IN);
    gpio_pull_up(BUTTON_NEXT);

    gpio_init(BUTTON_PREV);
    gpio_set_dir(BUTTON_PREV, GPIO_IN);
    gpio_pull_up(BUTTON_PREV);

    gpio_init(BUTTON_SELECT);
    gpio_set_dir(BUTTON_SELECT, GPIO_IN);
    gpio_pull_up(BUTTON_SELECT);

    while (true) {
        update_menu();
        sleep_ms(100);  // Aguarda um breve período entre as verificações dos botões
    }

    return 0;
}

// Implementação das funções do display LCD
void lcd_send_cmd(uint8_t cmd) {
    // Envia comando para o display LCD
}

void lcd_send_data(uint8_t data) {
    // Envia dados para o display LCD
}

void lcd_init() {
    // Inicializa o display LCD
    lcd_send_cmd(0x33);
    lcd_send_cmd(0x32);
    lcd_send_cmd(0x28);
    lcd_send_cmd(0x0C);
    lcd_send_cmd(0x06);
    lcd_clear();
}

void lcd_clear() {
    lcd_send_cmd(0x01);
    sleep_ms(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t pos = (row == 0) ? 0x80 + col : 0xC0 + col;
    lcd_send_cmd(pos);
}

void lcd_print(const char* str) {
    while (*str) {
        lcd_send_data((uint8_t)(*str));
        str++;
    }
}

// Implementação das funções do menu
void show_main_menu() {
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("1. Menu Principal");
    lcd_set_cursor(1, 0);
    lcd_print("2. Submenu");
}

void show_sub_menu() {
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Submenu 1");
    lcd_set_cursor(1, 0);
    lcd_print("Submenu 2");
}

void update_menu() {
    if (gpio_get(BUTTON_NEXT) == 0) {
        current_option = (current_option + 1) % 2;
        sleep_ms(200);  // Debounce
    } else if (gpio_get(BUTTON_PREV) == 0) {
        current_option = (current_option - 1 + 2) % 2;
        sleep_ms(200);  // Debounce
    } else if (gpio_get(BUTTON_SELECT) == 0) {
        if (current_option == 1) {
            current_menu = SUB_MENU;
            show_sub_menu();
        } else {
            current_menu = MAIN_MENU;
            show_main_menu();
        }
        sleep_ms(200);  // Debounce
    }

    if (current_menu == MAIN_MENU) {
        show_main_menu();
    } else {
        show_sub_menu();
    }
}
