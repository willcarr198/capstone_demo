#include "main.h"
#include <stm32f0xx_hal.h>

int main(void)
{
  #if defined(UART)
  uart_main();
  #elif defined(ADC)
  adc_in_main();
  #elif defined(LAB3)
  lab3_main();
  #elif defined(LAB4)
  lab4_main();
  #elif defined(LAB5)
  lab5_main();
  #elif defined(LAB6)
  lab6_main();
  #elif defined(LAB7)
  lab7_main();
  #else
  #error No valid target specified
  #endif

}
