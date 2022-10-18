#include "main.h"
#include "kprintf.c"


void uart_commandline(unsigned char *s, int uart)
{
  if (uart == UART0)
  {
    switch (*s)
    {
    case 27:
      break;
    case 1:
      uart_send_string(UART0, "\nArrows\n");
      break;

    case 127: //Backspace key
    uart_send(uart, 8); //Backspace
    uart_send(uart, 32); //Put space
    uart_send(uart, 8); //Backspace
      break;

    case 3: //Delete key
      uart_send(uart, 32);
      break;
    default:
      break;
    }
  }
}

/**
 * This is the C entry point, upcalled once the hardware has been setup properly
 * in assembly language, see the reset.s file.
 */

void _start()
{
  int i = 0;
  int count = 0;
  uart_clear(UART0);
  // uart_send_string(UART0, "\nQuit with \"C-a c\" and then type in \"quit\".\n");
  // uart_send_string(UART0, "\nHello world!\n");
  unsigned char s = 'a';

  while (1)
  {
    unsigned char c;
    while (0 == uart_receive(UART0, &c))
    {
      // friendly reminder that you are polling and therefore spinning...
      // not good for the planet! But until we introduce interrupts,
      // there is nothing you can do about it... except comment out
      // // this annoying code ;-)
      // count++;
      // if (count > 10000000) {
      //   uart_send_string(UART0, "\n\rZzzz....\n\r");
      //   count = 0;
      // }
    }
    
    if (c == '\r')
    {
      uart_send(UART0, '\n');
    }
    uart_send(UART0, c);

    // kprintf("%d", c);
    uart_commandline(&c,UART0);

    
    
  }
}

