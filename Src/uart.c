#include "uart.h"
// #include "lab5.h"
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
void uart_main()
{

    uart_init();

    while (1)
        uart_final();
}

void uart_init()
{

    HAL_Init();
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODER10 | GPIO_MODER_MODER11)) | GPIO_MODER_MODER10_1 | GPIO_MODER_MODER11_1;

    uint16_t ledPins = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;

    GPIO_InitTypeDef initLED = {ledPins,
                                GPIO_MODE_OUTPUT_PP,
                                GPIO_SPEED_FREQ_LOW,
                                GPIO_NOPULL};

    HAL_GPIO_Init(GPIOC, &initLED);

    GPIO_InitTypeDef initClk = {GPIO_PIN_0,
                                GPIO_MODE_INPUT,
                                GPIO_SPEED_FREQ_LOW,
                                GPIO_PULLDOWN};

    HAL_GPIO_Init(GPIOA, &initClk);

    GPIOB->AFR[1] |= 0x04 << GPIO_AFRH_AFSEL10_Pos;
    GPIOB->AFR[1] |= 0x04 << GPIO_AFRH_AFSEL11_Pos;

    USART3->BRR = HAL_RCC_GetHCLKFreq() / 115200;

    NVIC_EnableIRQ(USART3_4_IRQn);
    NVIC_SetPriority(USART3_4_IRQn, 1);

    // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, 0);

    USART3->CR1 |= USART_CR1_RXNEIE;
    USART3->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
}

void USART3_4_IRQHandler(void)
{
    incoming_char = USART3->RDR;
    new_data_flag = 1;
}

void uart_final()
{

    char incoming_function;
    char incoming_num;
    char value;

    // usart_string_writer("CMD?:   ");
    // usart_char_writer(0x0D);
    // usart_char_writer(0x04);

    while (new_data_flag == 0)
        HAL_Delay(1);
    new_data_flag = 0;

    if ((incoming_char == 't') | (incoming_char == 'f'))
    {
        incoming_function = incoming_char;
    }
    else
    {
        usart_string_writer("Please enter a valid function, 1-3\n");
        usart_char_writer(0x00);
        // usart_char_writer(0x04);
        return;
    }

    while (new_data_flag == 0)
        HAL_Delay(1);
    new_data_flag = 0;
    if ((incoming_char == '0') | (incoming_char == '1'))
    {

        incoming_num = incoming_char;
        usart_string_writer("Command entered, conduction test\n");
    }
    else
    {
        usart_string_writer("Enterred int is not a valid command. Enter \'0\', \'1\', or \'2\'.\n");
        usart_char_writer(0x00);
        return;
    }
    usart_adc_probe(incoming_num == '1');
    // uint16_t targetted_GPIO;

    usart_string_writer("Command Executed: ");
    usart_char_writer(incoming_function);
    usart_char_writer(incoming_num);
    usart_string_writer(" has been toggled.");
    usart_char_writer(0x00);
    // usart_char_writer('\n');
    // usart_char_writer(0x0D);
    // usart_char_writer(0x04);
    return;
}

void usart_adc_probe(uint8_t which_pin)
{
    GPIO_InitTypeDef initInput = {GPIO_PIN_1 | GPIO_PIN_3,
                                  GPIO_MODE_ANALOG,
                                  GPIO_SPEED_FREQ_LOW,
                                  GPIO_NOPULL};

    HAL_GPIO_Init(GPIOC, &initInput);
    // GPIOA->MODER |= (GPIO_MODER_MODER6_Msk | GPIO_MODER_MODER7_Msk);

    // GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR6_1 | GPIO_PUPDR_PUPDR7_0 | GPIO_PUPDR_PUPDR7_1);

    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    ADC1->CFGR1 &= (~ADC_CFGR1_RES_Msk);
    ADC1->CFGR1 |= 0x10 << ADC_CFGR1_RES_Pos;
    // enable continuous mode
    // disable hardware triggers
    ADC1->CFGR1 &= ~ADC_CFGR1_EXTEN_Msk;

    ADC1->CFGR1 |= ADC_CFGR1_CONT_Msk;
    // ADC1->CFGR1 |= ADC_CFGR1_OVRMOD;
    //  enable ADC_6
    //  ADC1->CFGR1 &= ~(ADC_CFGR1_AWD1CH_Msk);
    //  ADC1->CFGR1 |= (0x06 << ADC_CFGR1_AWD1CH_Pos);
    ADC1->CHSELR &= ~(ADC_CHSELR_CHSEL_Msk);
    if (which_pin)
        ADC1->CHSELR |= ADC_CHSELR_CHSEL11_Msk; // | ADC_CHSELR_CHSEL13_Msk;
    else
        ADC1->CHSELR |= ADC_CHSELR_CHSEL13_Msk;
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
    uint32_t count = 0;
    uint32_t output = 0;
    while (1)
    {
        count++;
        // usart_string_writer("Count: ");
        // write_digits(count);
        // usart_char_writer('\n');
        // usart_char_writer(0x0D);

        // while ((ADC1->ISR & ADC_ISR_EOC_Msk))
        // ;

        // if (ADC1->ISR & ADC_ISR_OVR)
        //  ADC1->ISR |= ADC_ISR_OVR_Msk;

        // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, ADC1->CR & ADC_CR_ADSTART);
        // HAL_Delay(100);
        // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
        output = ADC1->DR;
        // break;
        if (count > 100)
            break;
        /*
        usart_string_writer("volts: ");
        write_digits(output);
        usart_char_writer('\n');
        usart_char_writer(0x0D);
        */
        // uint32_t holdForWrite = volts;
        // if(count > 10) break;

        // usart_string_writer("Volts: ");
        // write_digits(volts);
        // usart_char_writer('\n');
        // usart_char_writer(0x0D);
        //  count += 1;
        //  usart_char_writer(count);
        //   usart_char_writer(volts);
    }
    usart_string_writer("Volts: ");
    write_digits(output);
    usart_char_writer('\n');
    usart_char_writer(0x0D);
    if (output < 100 && (!which_pin))
    {
        usart_string_writer("Test Passed");
    }
    else if (output > 100 && (!which_pin))
        usart_string_writer("Test Failed");
    else if (output > 100 && (which_pin))
        usart_string_writer("Test Passed");
    else
        usart_string_writer("Test Failed");
    ADC1->CR |= ADC_CR_ADSTP_Msk;
    while (!(ADC1->CR & ADC_CR_ADSTP_Msk))
        ;

    ADC1->CR |= ADC_CR_ADDIS_Msk;
    while (!(ADC1->CR & ADC_CR_ADDIS_Msk))
        ;
    // PC3 is t0, and pulled up
    // PC1 is t1, and is pulled down
    //  NVIC_EnableIRQ(ADC1_IRQn);
}
void usart_reader()
{
    uint16_t readVal;
    while ((USART3->ISR & USART_ISR_RXNE) != USART_ISR_RXNE)
        ;

    readVal = USART3->RDR;

    switch (readVal)
    {
    case 'r':
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
        break;
    case 'b':
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
        break;
    case 'g':
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
        break;
    case 'o':
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
        break;
    default:
        usart_string_writer("Invalid Command\n");

        break;
    }
    // usart_char_writer(readVal);
}
void usart_char_writer(char c)
{
    // volatile uint8_t status;
    // volatile int hold;
    // volatile int hold2 = USART3->TDR;
    while ((USART3->ISR & USART_ISR_TXE) != USART_ISR_TXE)
    {
        // hold = USART3->ISR;
    }
    // hold2 = USART3->TDR;
    USART3->TDR = c;

    // USART1->ICR |= USART_ICR_TCCF;
    // attempt to write a !
}
void usart_string_writer(char *str)
{
    int i = 0;
    while (1)
    {
        if (str[i] == 0x00)
            return;
        else
        {
            usart_char_writer(str[i]);
            i++;
        }
    }
}