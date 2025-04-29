#include "adc_in.h"

/*
void write_digits(uint32_t count)
{
    char *buffer[32];
    int i = 0;
    while (count > 0)
    {
        buffer[i] = 0x30 + (count % 10);
        i++;
        count /= 10;
    }
    for (int j = i; j >= 0; j--)
    {
        usart_char_writer(buffer[j]);
    }
    // usart_string_writer(buffer);
}
    */
void adc_in_main()
{
    HAL_Init();
    uart_init();
   
    GPIO_InitTypeDef initInput = {GPIO_PIN_2 | GPIO_PIN_3,
                                  GPIO_MODE_ANALOG,
                                  GPIO_SPEED_FREQ_LOW,
                                  GPIO_NOPULL};

    HAL_GPIO_Init(GPIOC, &initInput);
    

    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    ADC1->CFGR1 &= (~ADC_CFGR1_RES_Msk);
    ADC1->CFGR1 |= 0x10 << ADC_CFGR1_RES_Pos;
    // enable continuous mode
    // disable hardware triggers
    ADC1->CFGR1 &= ~ADC_CFGR1_EXTEN_Msk;

    ADC1->CFGR1 |= ADC_CFGR1_CONT_Msk;

    ADC1->CHSELR &= ~(ADC_CHSELR_CHSEL_Msk);
    ADC1->CHSELR |= ADC_CHSELR_CHSEL12_Msk;

    // Following code pulled from the STM Peripheral Manual
    //===================================================
    if ((ADC1->CR & ADC_CR_ADEN) != 0)
    {
        ADC1->CR |= ADC_CR_ADDIS;
    }
    while ((ADC1->CR & ADC_CR_ADEN) != 0)
        ;
    ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN;
    ADC1->CR |= ADC_CR_ADCAL;
    while ((ADC1->CR & ADC_CR_ADCAL) != 0)
        ;

    if ((ADC1->ISR & ADC_ISR_ADRDY) != 0)
    {
        ADC1->ISR |= ADC_ISR_ADRDY;
    }
    ADC1->CR |= ADC_CR_ADEN;
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0)
        ;
    //===================================================
    ADC1->CR |= ADC_CR_ADSTART_Msk;
    // uint8_t count = 0;
    while (1)
    {
        uint32_t volts = ADC1->DR;
        uint32_t holdForWrite = volts;

        usart_string_writer("Volts: ");
        write_digits(volts);
        usart_char_writer('\n');
        usart_char_writer(0x0D);
        
    }
    // NVIC_EnableIRQ(ADC1_IRQn);
}

void adc_init()
{

    uint16_t ledPins = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;

    GPIO_InitTypeDef initLED = {ledPins,
                                GPIO_MODE_OUTPUT_PP,
                                GPIO_SPEED_FREQ_LOW,
                                GPIO_NOPULL};

    // PA6 and PA7 for ADC_6 and ADC_7

    // enable analog mode for pins 6 and 7
    GPIOA->MODER |= (GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1);

    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR6_1 | GPIO_PUPDR_PUPDR7_0 | GPIO_PUPDR_PUPDR7_1);

    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    ADC1->CFGR1 &= (~ADC_CFGR1_RES_Msk);
    ADC1->CFGR1 |= 0x01 << ADC_CFGR1_RES_Pos;
    // enable continuous mode
    ADC1->CFGR1 |= ADC_CFGR1_CONT_Msk;
    // disable hardware triggers
    ADC1->CFGR1 &= ~ADC_CFGR1_EXTEN_Msk;

    // enable ADC_6
    ADC1->CFGR1 &= ~(ADC_CFGR1_AWD1CH_Msk);
    ADC1->CFGR1 |= (0x06 << ADC_CFGR1_AWD1CH_Pos);

    // set and then wait for calibration to be complete
    /*
    ADC1->CR |= ADC_CR_ADCAL;
    while(ADC1->CR & ADC_CR_ADCAL);

    //enable and then start the ADC
    ADC1->CR |= ADC_CR_ADEN;
    ADC1->CR |= ADC_CR_ADSTART_Msk;
        */

    // Following code pulled from the STM Peripheral Manual
    //===================================================
    if ((ADC1->CR & ADC_CR_ADEN) != 0)
    {
        ADC1->CR |= ADC_CR_ADDIS;
    }
    while ((ADC1->CR & ADC_CR_ADEN) != 0)
        ;
    ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN;
    ADC1->CR |= ADC_CR_ADCAL;
    while ((ADC1->CR & ADC_CR_ADCAL) != 0)
        ;

    if ((ADC1->ISR & ADC_ISR_ADRDY) != 0)
    {
        ADC1->ISR |= ADC_ISR_ADRDY;
    }
    ADC1->CR |= ADC_CR_ADEN;
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0)
        ;
    //===================================================

    ADC1->CR |= ADC_CR_ADSTART_Msk;


    /*
    while (1)
    {
        uint8_t volts = ADC1->DR;

        if (volts >= 1)
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, 1);
        else
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, 0);

        if (volts > 2)
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, 1);
        else
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, 0);

        if (volts > 3)
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 1);
        else
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 0);
    }
            */
    // NVIC_EnableIRQ(ADC1_IRQn);
}