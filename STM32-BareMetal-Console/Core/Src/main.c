// RCC
#define RCC_AHB1ENR *((volatile unsigned int *)0x40023830)
#define RCC_APB1ENR *((volatile unsigned int *)0x40023840)

// GPIO
#define GPIOA_MODER *((volatile unsigned int *)0x40020000)
#define GPIOD_MODER *((volatile unsigned int *)0x40020C00)
#define GPIOA_IDR *((volatile unsigned int *)0x40020010)
#define GPIOD_ODR *((volatile unsigned int *)0x40020C14)
#define GPIOA_AFRL *((volatile unsigned int *)0x40020020)

// USART2
#define USART2_SR *((volatile unsigned int *)0x40004400)
#define USART2_DR *((volatile unsigned int *)0x40004404)
#define USART2_BRR *((volatile unsigned int *)0x40004408)
#define USART2_CR1 *((volatile unsigned int *)0x4000440C)

#define PORTA_EN_BIT 0
#define PORTD_EN_BIT 3
#define USART2_EN_BIT 17

#define PIN_BUTTON 0
#define PIN_UART_TX 2
#define PIN_LED 12

#define MODER_BITS_PER_PIN 2
#define AFR_BITS_PER_PIN 4

#define MODE_INPUT 0  // 00
#define MODE_OUTPUT 1 // 01
#define MODE_AF 2     // 10
#define AF7_USART2 7

#define BRR_115200_16MHZ 0x008B // calculated

#define CR1_UE_BIT 13
#define CR1_TE_BIT 3
#define SR_TXE_BIT 7

void uart_send_string(const char *str)
{
  while (*str != '\0')
  {
    while (!(USART2_SR & (1 << SR_TXE_BIT)));

    USART2_DR = *str;
    str++;
  }
}

int main(void)
{
  // activate clock for port a (0) and port d (3)
  RCC_AHB1ENR |= (1 << PORTA_EN_BIT) | (1 << PORTD_EN_BIT);

  // activate clock for usart2 (17)
  RCC_APB1ENR |= (1 << USART2_EN_BIT);

  // led (pin 12) as output (01)
  GPIOD_MODER &= ~(3 << (PIN_LED * MODER_BITS_PER_PIN));
  GPIOD_MODER |= (MODE_OUTPUT << (PIN_LED * MODER_BITS_PER_PIN));

  // button (pin 0) as input (00)
  GPIOA_MODER &= ~(3 << (PIN_BUTTON * MODER_BITS_PER_PIN));

  // uart (pin 2) as af (10)
  GPIOA_MODER &= ~(3 << (PIN_UART_TX * MODER_BITS_PER_PIN));
  GPIOA_MODER |= (MODE_AF << (PIN_UART_TX * MODER_BITS_PER_PIN));

  // af7 for pin 2
  GPIOA_AFRL &= ~(0xF << (PIN_UART_TX * AFR_BITS_PER_PIN));
  GPIOA_AFRL |= (AF7_USART2 << (PIN_UART_TX * AFR_BITS_PER_PIN));

  // baudrate
  USART2_BRR = BRR_115200_16MHZ;

  // activate transmitter
  USART2_CR1 |= (1 << CR1_TE_BIT) | (1 << CR1_UE_BIT);

  int button_was_pressed = 0;

  while (1)
  {
    if (GPIOA_IDR & (1 << PIN_BUTTON))
    {
      GPIOD_ODR |= (1 << PIN_LED); 

      if (button_was_pressed == 0)
      {
        uart_send_string("Button Pressed!\r\n");
        button_was_pressed = 1; 
      }
    }
    else
    {
      GPIOD_ODR &= ~(1 << PIN_LED); 
      button_was_pressed = 0;       
    }
  }
}