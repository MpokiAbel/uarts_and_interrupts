#include "main.h"
#include "kprintf.c"
#include <string.h>

unsigned char command[255];
int commandLength;
int commandCursor;
int escapeSeq[2];
unsigned char commandHistory[20][255];
int historyCounter;
int historyFull;

int stringCompare(char a[], char b[])
{
  int flag = 0, i = 0;
  while (a[i] != '\0' && b[i] != '\0')
  {
    if (a[i] != b[i])
    {
      flag = 1;
      break;
    }
    i++;
  }
  if (flag == 0)
    return 0;
  else
    return 1;
}

void stringParse(int start, unsigned char *cmd, int *endIndex)
{
  int i = start;
  kprintf("%d", i);
  while (i < commandLength)
  {
    if (command[i] == ' ')
    {
      *(cmd + i) = '\0';
      *endIndex = i + 1;
      break;
    }
    *(cmd + (i - start)) = command[i];

    i++;
  }
}

void commandExecution(int uart)
{
  unsigned char parsedString[commandLength];
  int *index;

  stringParse(0, parsedString, index);

  if (stringCompare(parsedString, "reset") == 0)
  {
    uart_clear(uart);
  }
  else if (stringCompare(parsedString, "echo") == 0)
  {
    stringParse(*index, parsedString, index);
    uart_send(uart, '\n');
    uart_send_string(uart, parsedString);
    uart_send(uart, '\n');
  }
  else
  {
    unsigned char temp[] = ": command not found\n";
    uart_send(uart, '\n');
    uart_send_string(uart, command);
    uart_send_string(uart, temp);
  }
  // store the command here

  // if (historyCounter < 20)
  // {
  //   for (int i = 0; i < commandLength; i++)
  //   {
  //     commandHistory[historyCounter][i] = command[i];
  //   }
  //   commandHistory[historyCounter][commandLength] = '\0';
  //   historyCounter++;
  // }
  // else
  // {
  //   int i = 18;
  //   while (i > 0)
  //   {
  //     int j = 0;
  //     while (commandHistory[i][j] != '\0')
  //     {
  //       commandHistory[i + 1][j] = commandHistory[i][j];
  //       j++;
  //     }
  //     commandHistory[i + 1][j] = '\0';
  //     i--;
  //   }
  //   for (int i = 0; i < commandLength; i++)
  //   {
  //     commandHistory[0][i] = command[i];
  //   }
  //   commandHistory[0][i] = '\0';
  // }

  commandCursor = 0;
  commandLength = 0;
}
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

void moveCursorBeginErase(int uart)
{
  uart_send(uart, 27);
  uart_send(uart, 91);
  uart_send(uart, 71);

  // Erase Everything
  uart_send(uart, 27);
  uart_send(uart, 91);
  uart_send(uart, 75);
}

void takeHistory(int uart, int pos)
{
  int i = 0;
  moveCursorBeginErase(uart);
  while (commandHistory[pos][i] != '\0')
  {
    command[i] = commandHistory[pos][i];
    uart_send(uart, command[i]);
    i++;
  }
  command[i] = '\0';
  commandCursor = i;
  commandLength = i;
}

void moveCursor(int pos)
{
  // Move cursor to the required position
  uart_send(UART0, 27);
  uart_send(UART0, 91);
  uart_send(UART0, 49 + pos);
  uart_send(UART0, 71);
}

void moveCursorBegin()
{
  uart_send(UART0, 27);
  uart_send(UART0, 91);
  uart_send(UART0, 71);
}

void uart_commandline(unsigned char *s, int uart, int *UpDownMove)
{
  int options = *s;
  if (escapeSeq[0] == 1 && escapeSeq[1] == 1)
  {
    // Read the other value
    switch (options)
    {
      // case 65: // Up Arrow
      //   *UpDownMove++;
      //   if (*UpDownMove < 20)
      //   {
      //     takeHistory(uart, *UpDownMove);
      //   }
      //   else
      //     *UpDownMove = 19;
      //   break;
      // case 66: // Down Arrow
      //   *UpDownMove--;
      //   if (*UpDownMove > -1)
      //   {
      //     takeHistory(uart, *UpDownMove);
      //   }
      //   else
      //     *UpDownMove = -1;
      //   break;

    case 67:                  // Right Arrow
      if (commandLength == 0) // if there is no command typed dont go right
      {
        uart_send(uart, 27);
        uart_send(uart, 91);
        uart_send(uart, 68);
      }
      else if (commandLength > 0) // if there is command written do something
      {

        if (commandCursor == commandLength) // if the cursor is at the end of the command do nothing
        {
          uart_send(uart, 27);
          uart_send(uart, 91);
          commandLength_pos(uart, commandLength);
          uart_send(uart, 71);
        }

        else if (commandCursor < commandLength) // if the cursor is not at the end move to right
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

    case 126: // Delete key

      if (commandCursor < commandLength)
      {

        unsigned char tempArray[commandLength - commandCursor];
        for (int i = 0; i < commandLength - commandCursor; i++)
        {
          tempArray[i] = command[commandCursor + i + 1];
        }

        for (int j = commandCursor; j < commandLength - 1; j++)
        {
          command[j] = tempArray[j - commandCursor];
        }
        commandLength--;
        command[commandLength] = '\0';

        moveCursorBeginErase(uart);

        // Print the command
        uart_send_string(UART0, command);

        // Move cursor to the required position
        moveCursor(commandCursor);
      }
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

    uart_send(UART1, 27);
    uart_send(UART1, 91);
    uart_send(UART1, 49);
    uart_send(UART1, 71);

    if (commandCursor < commandLength)
    {
      unsigned char tempArray[commandLength - commandCursor];
      for (int i = 0; i < commandLength - commandCursor; i++)
      {
        tempArray[i] = command[commandCursor + i];
      }

      for (int j = commandCursor - 1; j < commandLength - 1; j++)
      {
        command[j] = tempArray[j - (commandCursor - 1)];
      }
      commandLength--;
      commandCursor--;
      command[commandLength] = '\0';

      // Move Cursor to the beginning of a live
      moveCursorBegin();

      // Print the command
      uart_send_string(UART0, command);

      // Move cursor to the required position
      moveCursor(commandCursor);

      commandLength++;
      commandCursor++;
    }

    if (commandCursor > 0)
    {
      commandLength--;
      commandCursor--;
    }
  }
  else if (options == 13)
  {
    commandExecution(uart);
  }
  else if (commandLength == commandCursor)
  {

    command[commandLength] = options;
    if (command[commandLength] == '\r')
    {
      uart_send(uart, '\n');
    }
    uart_send(uart, command[commandLength]);
    commandLength++;
    commandCursor++;
    command[commandLength] = '\0';
  }
  else if (commandLength > commandCursor)
  {

    unsigned char tempArray[commandLength - commandCursor];
    for (int i = 0; i < commandLength - commandCursor; i++)
    {
      tempArray[i] = command[commandCursor + i];
    }

    for (int j = commandCursor + 1; j < commandLength + 1; j++)
    {
      command[j] = tempArray[j - (commandCursor + 1)];
    }

    command[commandCursor] = options;
    commandLength++;
    commandCursor++;
    command[commandLength] = '\0';

    // Move Cursor to the beginning of a live
    moveCursorBegin();

    // Print the command
    uart_send_string(UART0, command);

    // Move cursor to the required position
    moveCursor(commandCursor);
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
  historyCounter = 0;
  historyFull = 0;
  int *UpDownMove;
  *UpDownMove = -1;
  // uart_clear(UART0);
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
    uart_commandline(&c, UART0, UpDownMove);
  }
}