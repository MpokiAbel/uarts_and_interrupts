#include "main.h"
#include "kprintf.c"

char command[255];
int cursor;

void cursor_pos(int uart)
{

  int temp = cursor + 1;
  int temp2 = cursor + 1;
  int count = 0;

  
  while (temp > 0)
  {
    temp = temp / 10;
    count++;
  }
  // count -= 1;
  int array[count];
  int i = 1;
  while (temp2 > 0)
  {
    array[count - i] = temp2 % 10;
    temp2 = temp2 / 10;
    i++;
  }

  for (int i = 0; i < count; i++)
  {
    uart_send(uart, 48 + array[i]);
    kprintf("%d", array[i]);
    
  }
}

void uart_commandline(unsigned char *s, int uart)
{
  int options = *s;
  
  switch (options)
  {
  case 27:
    uart_receive(UART0, s);
    if (*s == 91)
    {
      uart_receive(UART0, s);
      if (*s == 65) // UP arrow
      {
        // uart_send_string(UART1, " --arrows UP\n");
      }

      else if (*s == 66)
      {
        // uart_send_string(UART1, "\nDOWN\n");
      }

      else if (*s == 67) // RIGHT ARROW
      {
        if (cursor == 0)
        {
          uart_send(uart, 27);
          uart_send(uart, 91);
          uart_send(uart, 68);
        }
        else if (cursor > 0)
        {

          uart_send(uart, 27);
          uart_send(uart, 91);
          cursor_pos(uart);
          
          uart_send(uart, 71);
        }
      }

      if (*s == 68 && cursor > 0) // BACK ARROW
      {
        uart_send(uart, 8); // Backspace key
      }
      if (*s == 51)
      {
        uart_receive(UART0, s);
        if (*s == 126) // Delete key
        {
          // uart_send_string(UART1, "\nDELETE\n");
        }
      }
    }
    break;

  case 127:             // Backspace key
    uart_send(uart, 8); // Backspace

    uart_send(uart, 27);
    uart_send(uart, 91);
    uart_send(uart, 75);

    //    uart_send(uart, 8); //Backspace
    cursor--;
    break;

  default:
   
    command[cursor] = *s;

    if (command[cursor] == '\r')
    {
      uart_send(UART0, '\n');
    }
    uart_send(UART0, command[cursor]);

    cursor++;

    break;
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
  cursor = 0;

  uart_clear(UART0);
  uart_send_string(UART0, "\nQuit with \"C-a c\" and then type in \"quit\".\n");
  uart_send_string(UART0, "\nHello world!\n");
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

    // kprintf("%d", c);
    uart_commandline(&c, UART0);
  }
}