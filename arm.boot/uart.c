#include "main.h"
#include "uart.h"

struct cb rxcb[UART_COUNT];
struct cb txcb[UART_COUNT];

int uart_get_bar(int uart_id)
{
  switch (uart_id)
  {
  case 0:
    return UART0;
    break;
  case 1:
    return UART1;
    break;
  case 2:
    return UART2;
    break;
  // case 3:
  //   return UART3;
  //   break;
  default:
    return -1;
    break;
  }
}

void rx_handler(int uart_id)
{
  // fill with the available bytes from the RX FIFO.

  int uart = uart_get_bar(uart_id);
  unsigned short *uart_fr = (unsigned short *)(uart + UART_FR);
  unsigned short *uart_dr = (unsigned short *)(uart + UART_DR);
  while (1)
  {
    if ((*uart_fr & UART_RXFE) || cb_put(&rxcb[uart_id], (*uart_dr & 0xff)) != -1)
      break;
  }
}
void tx_handler(int uart_id)
{
  // that the handler of the TX interrupt will empty, writing the bytes to the TX FIFO, when there is room to do so.
  int uart = uart_get_bar(uart_id);
  uint8_t byte;
  unsigned short *uart_fr = (unsigned short *)(uart + UART_FR);
  unsigned short *uart_dr = (unsigned short *)(uart + UART_DR);
  while (1)
  {
    if ((*uart_fr & UART_TXFF) || cb_get(&txcb[uart_id], &byte) != -1)
      break;
    *uart_dr = byte;
  }

  uint16_t imsc = *(uint16_t *)(uart + UART_IMSC);
  imsc = imsc | UART_IMSC_TXIM;
  *(uint16_t *)(uart + UART_IMSC) = imsc;
}

/**
 * Receive a character from the given uart, this is a non-blocking call.
 * Returns 0 if there are no character available.
 * Returns 1 if a character was read.
 */

int uart_receive(int uart, unsigned char *s)
{
  rx_handler(uart);
  int value = cb_get(&rxcb[0], s);
  return value + 1;
}

/**
 * Sends a character through the given uart, this is a blocking call.
 * The code spins until there is room in the UART TX FIFO queue to send
 * the character.
 */
void uart_send(int uart, unsigned char s)
{
  cb_put(&txcb[uart], s);
  tx_handler(uart);
  // Trigger an interrupt here !!!
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

int uart_get_IRQ_rn(int uart_id)
{
  switch (uart_id)
  {
  case 0:
    return UART0_IRQ;
    break;
  case 1:
    return UART1_IRQ;
    break;
  case 2:
    return UART2_IRQ;
    break;
  // case 3:
  //   return UART3_IRQ;
  //   break;
  default:
    return -1;
    break;
  }
}

void handler(void *cookie)
{

  int uart_bar = uart_get_bar((int)cookie);
  if (*(uint16_t *)(UART_MIS + uart_bar) & UART_MIS_RXMIS)
  {
    rx_handler((int)cookie);
  }
  else if (*(uint16_t *)(UART_MIS + uart_bar) & UART_MIS_TXMIS)
  {
    tx_handler((int)cookie);
  }
  else if (*(uint16_t *)(UART_MIS + uart_bar) & UART_MIS_RTMIS)
  {
    rx_handler((int)cookie);
  }
}

void uart_init()
{
  vic_setup();
  // Enable FIFO queues, both rx-queue and tx-queue.

  for (int uart_id = 0; uart_id < UART_COUNT; uart_id++)
  {
    int uart_bar = uart_get_bar(uart_id);

    uint16_t lcr = *(uint16_t *)(uart_bar + CUARTLCR_H);
    lcr |= CUARTLCR_H_FEN;
    *(uint16_t *)(uart_bar + CUARTLCR_H) = lcr;

    cb_init(&rxcb[uart_id]);
    cb_init(&txcb[uart_id]);

    // Provide the handler for the interrupt at a specific handler position
    int irq_rn = uart_get_IRQ_rn(uart_id);
    vic_irq_enable(irq_rn, handler, (void *)uart_id);

    // set the Interrupt Mask Set/Clear Register to 1 to allow interrupt
    uint16_t imsc = *(uint16_t *)(uart_bar + UART_IMSC);
    imsc = imsc | UART_IMSC_RXIM | UART_IMSC_TXIM | UART_IMSC_RTIM;
    *(uint16_t *)(uart_bar + UART_IMSC) = imsc;
  }
  vic_enable();
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
