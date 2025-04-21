#include <stm32f0xx_hal.h>
#include "hal_gpio.h"
#include <assert.h>
uint16_t incoming_char;
uint8_t new_data_flag = 0;

void USART3_4_IRQHandler(void);

//void uart_main();
void uart_final();
void usart_char_writer(char c);
void usart_string_writer(char* str);
void usart_reader();