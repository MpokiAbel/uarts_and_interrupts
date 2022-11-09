#include "main.h"
#include "uart.h"

struct cb rxcb0, txcb0, rxcb1, txcb1, rxcb2, txcb2, rxcb3, txcb3;

/**
 * Receive a character from the given uart, this is a non-blocking call.
 * Returns 0 if there are no character available.
 * Returns 1 if a character was read.
 */

int uart_receive(int uart, unsigned char *s)
{
  // unsigned short *uart_fr = (unsigned short *)(uart + UART_FR);
  // unsigned short *uart_dr = (unsigned short *)(uart + UART_DR);
  // if (*uart_fr & UART_RXFE)
  //   return 0;
  // *s = (*uart_dr & 0xff);

  // return 1;

  int value;

  switch (uart)
  {
  case 0:
    value = cb_get(&rxcb0, s);
    break;
  case 1:
    value = cb_get(&rxcb1, s);
    break;
  case 2:
    value = cb_get(&rxcb2, s);
    break;
  case 3:
    value = cb_get(&rxcb3, s);
    break;
  }

  return value + 1;
}

/**
 * Sends a character through the given uart, this is a blocking call.
 * The code spins until there is room in the UART TX FIFO queue to send
 * the character.
 */
void uart_send(int uart, unsigned char s)
{
  // unsigned short *uart_fr = (unsigned short *)(uart + UART_FR);
  // unsigned short *uart_dr = (unsigned short *)(uart + UART_DR);
  // while (*uart_fr & UART_TXFF)
  //   ;
  // *uart_dr = s;

  switch (uart)
  {
  case 0:
    cb_put(&txcb0, s);
    break;
  case 1:
    cb_put(&txcb1, s);
    break;
  case 2:
    cb_put(&txcb2, s);
    break;
  case 3:
    cb_put(&txcb3, s);
    break;
  }
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

void uart_init()
{
  // Enable FIFO queues, both rx-queue and tx-queue.
  uint16_t lcr = *(uint16_t *)(UART0 + CUARTLCR_H);
  lcr |= CUARTLCR_H_FEN;
  *(uint16_t *)(UART0 + CUARTLCR_H) = lcr;

  uint16_t lcr = *(uint16_t *)(UART1 + CUARTLCR_H);
  lcr |= CUARTLCR_H_FEN;
  *(uint16_t *)(UART1 + CUARTLCR_H) = lcr;

  uint16_t lcr = *(uint16_t *)(UART2 + CUARTLCR_H);
  lcr |= CUARTLCR_H_FEN;
  *(uint16_t *)(UART2 + CUARTLCR_H) = lcr;

  uint16_t lcr = *(uint16_t *)(UART3 + CUARTLCR_H);
  lcr |= CUARTLCR_H_FEN;
  *(uint16_t *)(UART3 + CUARTLCR_H) = lcr;

  cb_init(&rxcb0);
  cb_init(&txcb0);
  cb_init(&rxcb1);
  cb_init(&txcb1);
  cb_init(&rxcb2);
  cb_init(&txcb2);
  cb_init(&rxcb3);
  cb_init(&txcb3);

  vic_setup();
  vic_enable();
}

void rx_handler()
{
  //fill with the available bytes from the RX FIFO. 
}
void tx_handler()
{
  //that the handler of the TX interrupt 
  //will empty, writing the bytes to the TX FIFO, when there is room to do so.
}

void uart_clear(int uart)
{
  // Clear screen
  uart_send(uart, 27);
  uart_send(uart, 91);
  uart_send(uart, 49);
  uart_send(uart, 74);

  // Move the cursor to top
  uart_send(uart, 27);
  uart_send(uart, 91);
  uart_send(uart, 72);
}
