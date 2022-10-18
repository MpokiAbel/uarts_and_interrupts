#include "main.h"



/**
 * Receive a character from the given uart, this is a non-blocking call.
 * Returns 0 if there are no character available.
 * Returns 1 if a character was read.
 */
int uart_receive(int uart, unsigned char *s)
{
  unsigned short *uart_fr = (unsigned short *)(uart + UART_FR);
  unsigned short *uart_dr = (unsigned short *)(uart + UART_DR);
  if (*uart_fr & UART_RXFE)
    return 0;
  *s = (*uart_dr & 0xff);
  
  return 1;
}

/**
 * Sends a character through the given uart, this is a blocking call.
 * The code spins until there is room in the UART TX FIFO queue to send
 * the character.
 */
void uart_send(int uart, unsigned char s)
{
  unsigned short *uart_fr = (unsigned short *)(uart + UART_FR);
  unsigned short *uart_dr = (unsigned short *)(uart + UART_DR);
  while (*uart_fr & UART_TXFF)
    ;
  *uart_dr = s;
}

/**
 * This is a wrapper function, provided for simplicity,
 * it sends a C string through the given uart.
 */
void uart_send_string(int uart, const unsigned char *s)
{
  while (*s != '\0')
  {
    uart_send(uart, *s);
    s++;
  }
}

void uart_clear(int uart)
{
  //Clear screen
    uart_send(uart, 27);
    uart_send(uart, 91);
    uart_send(uart,49);
    uart_send(uart, 74);

  //Move the cursor to top
    uart_send(uart, 27);
    uart_send(uart, 91);
    uart_send(uart,48);
    uart_send(uart,59);
    uart_send(uart,48);
    uart_send(uart, 72);

}
