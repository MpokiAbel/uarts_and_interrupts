#include "main.h"
#include "kprintf.c"

char command[255];
int commandLength;
int commandCursor;
int escapeSeq[2];

void commandLength_pos(int uart, int length)
{

  int temp = length + 1;
  int temp2 = length + 1;
  int count = 0;

  while (temp > 0)
  {
    temp = temp / 10;
    count++;
  }
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
  if (escapeSeq[0] == 1 && escapeSeq[1] == 1)
  {
    // Read the other value
    switch (options)
    {
    case 65:
      /* code */
      break;
    case 66:
      /* code */
      break;

    case 67: // Right Arrow
      if (commandLength == 0)
      {
        uart_send(uart, 27);
        uart_send(uart, 91);
        uart_send(uart, 68);
      }
      else if (commandLength > 0)
      {

        if (commandCursor == commandLength)
        {
          uart_send(uart, 27);
          uart_send(uart, 91);
          commandLength_pos(uart, commandLength);
          uart_send(uart, 71);
        }

        else if (commandCursor < commandLength)
        {
          uart_send(uart, 27);
          uart_send(uart, 91);
          uart_send(uart, 67);
          commandCursor++;
        }
      }

      break;

    case 68:              // Left Arrow
      uart_send(uart, 8); // backspace key
      if (commandCursor > 0)
      {
        commandCursor--;
      }

      break;

    case 126:

      break;
    }
    // set the escapeSeq to 0;
    if (options >= 64 && options <= 126)
    {
      escapeSeq[0] = 0;
      escapeSeq[1] = 0;
    }
  }
  else if (escapeSeq[0] == 1 && options == 91)
  {
    escapeSeq[1] = 1;
  }
  else if (options == 27)
  {
    escapeSeq[0] = 1;
  }
  else if (options == 127) // backspace key
  {
    uart_send(uart, 8); // Backspace
    uart_send(uart, 27);
    uart_send(uart, 91);
    uart_send(uart, 75);

    if (commandCursor < commandLength)
    {
      int temp = commandCursor + 1;
      while (temp != commandLength)
      {
        uart_send(uart, command[temp]);
        temp++;
      }

      commandLength_pos(uart, commandCursor);
    }

    if (commandCursor > 0)
    {
      commandLength--;
      commandCursor--;
    }
  }
  else
  {

    command[commandLength] = options;
    if (command[commandLength] == '\r')
    {
      uart_send(uart, '\n');
    }
    uart_send(uart, command[commandLength]);

    commandLength++;
    commandCursor++;
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
  commandLength = 0;
  commandCursor = 0;
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
    uart_commandline(&c, UART0);
  }
}