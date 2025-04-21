#include "uart.h"
//#include "lab5.h"

void uart_main()
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

   
    GPIOB->AFR[1] |= 0x04 <<  GPIO_AFRH_AFSEL10_Pos;
    GPIOB->AFR[1] |= 0x04 << GPIO_AFRH_AFSEL11_Pos;
    
    USART3->BRR = HAL_RCC_GetHCLKFreq() / 115200;
    
    
    NVIC_EnableIRQ(USART3_4_IRQn);
    NVIC_SetPriority(USART3_4_IRQn, 1);
    
    
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, 0);

    USART3->CR1 |= USART_CR1_RXNEIE;
    USART3->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;

    while(1)
    uart_final();
    
}

void USART3_4_IRQHandler(void){
    incoming_char = USART3->RDR;
    new_data_flag = 1;
}
void uart_final(){

    char incoming_function;
    char incoming_num;
    char value;

    //usart_string_writer("CMD?:   ");  
    //usart_char_writer(0x0D);
    //usart_char_writer(0x04);

    while(new_data_flag == 0) HAL_Delay(1);
    new_data_flag = 0;
    if((incoming_char == 't') | (incoming_char == 'f')){
        incoming_function = incoming_char;
    }
    else{
        usart_string_writer("Please enter a valid function, 1-3\n");
        usart_char_writer(0x00);
        //usart_char_writer(0x04);
        return;
    }
    
    while(new_data_flag == 0) HAL_Delay(1);
    new_data_flag = 0;
    if((incoming_char == '0') | (incoming_char == '1') | (incoming_char == '2'))
        incoming_num = incoming_char;
    else{
        usart_string_writer("Enterred int is not a valid command. Enter \'0\', \'1\', or \'2\'.\n");
        usart_char_writer(0x00);
        return;
    }
    
    uint16_t targetted_GPIO;

    switch(incoming_num){
        case '0':
           HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, incoming_function == 't');
        break;
        case '1':
             HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, incoming_function == 't');
        break;
        case '2':
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, incoming_function == 't');
        break;
        default:
        return;
    }
    /*
    switch(value){
        case '0':
        HAL_GPIO_WritePin(GPIOC, targetted_GPIO, 0);
        break;
        case '1':
        HAL_GPIO_WritePin(GPIOC, targetted_GPIO, 1);
        break;
        case '2':
        HAL_GPIO_TogglePin(GPIOC, targetted_GPIO);
        break;
        default:
        return;
    }
    */
    usart_string_writer("Command Executed: ");
    usart_char_writer(incoming_function);
    usart_char_writer(incoming_num);
    usart_string_writer(" has been toggled.");
    usart_char_writer(0x00);
    //usart_char_writer('\n');
    //usart_char_writer(0x0D);
   // usart_char_writer(0x04);
    return;
}
void usart_reader(){
    uint16_t readVal;
    while((USART3->ISR & USART_ISR_RXNE) != USART_ISR_RXNE);
    

    readVal = USART3->RDR;

    switch(readVal){
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
    //usart_char_writer(readVal);
}
void usart_char_writer(char c)
{
    //volatile uint8_t status;
    //volatile int hold;
    //volatile int hold2 = USART3->TDR; 
    while((USART3->ISR & USART_ISR_TXE) != USART_ISR_TXE)
    {
        //hold = USART3->ISR;
       
    } 
    //hold2 = USART3->TDR;
    USART3->TDR = c;


    // USART1->ICR |= USART_ICR_TCCF;
    // attempt to write a !
}
void usart_string_writer(char* str){
    int i = 0;
    while(1){
        if(str[i] == 0x00) return;
        else{
            usart_char_writer(str[i]);
            i++;
        }
    }
}