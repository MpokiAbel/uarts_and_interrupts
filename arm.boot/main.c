#include "main.h"
#include "kprintf.c"
#include <string.h>

#define HISCOUNT 20
#define MAXCOMANDCHARSIZE 255

unsigned char commandHistory[HISCOUNT][MAXCOMANDCHARSIZE];

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

void stringParse(int start, unsigned char *cmd, int *endIndex, int *commandLength, unsigned char *command)
{
  int i = start;
  kprintf("%d", i);
  while (i < *commandLength)
  {
    if (*(command + i) == ' ')
    {
      *(cmd + i) = '\0';
      *endIndex = i + 1;
      break;
    }
    *(cmd + (i - start)) = *(command + i);

    i++;
  }
}

void commandExecution(int uart, int *commandLength, int *commandCursor,
                      int *historyCounter, int *historyFull, unsigned char *command)
{
  unsigned char parsedString[*commandLength];
  int a = 0;
  int *index;
  index = &a;
  if (*commandLength > 0)
  {

    stringParse(0, parsedString, index, commandLength, command);

    if (stringCompare(parsedString, "reset") == 0)
    {
      uart_clear(uart);
    }
    else if (stringCompare(parsedString, "echo") == 0)
    {
      uart_send(uart, '\n');
      if (*index == 0)
        uart_send(uart, ' ');
      else
        uart_send_string(uart, command + *index);
      uart_send(uart, '\n');
    }
    else
    {
      unsigned char temp[] = ": command not found\n";
      uart_send(uart, '\n');
      uart_send_string(uart, command);
      uart_send_string(uart, temp);
    }

    kprintf("%d \n", *index);
    uart_send(1, 27);
    uart_send(1, 91);
    uart_send(1, 71);
    // store the command here+

    if (*historyCounter < HISCOUNT)
    {

      for (int i = 0; i < *commandLength; i++)
      {
        commandHistory[*historyCounter][i] = *(command + i);
      }
      commandHistory[*historyCounter][*commandLength] = '\0';
      (*historyCounter)++;
    }
    else
    {
      int i = 1, k = 0;
      while (i < HISCOUNT)
      {
        int j = 0;
        while (commandHistory[i][j] != '\0')
        {
          commandHistory[i - 1][j] = commandHistory[i][j];
          j++;
        }
        commandHistory[i - 1][j] = '\0';
        i++;
      }

      while (k < *commandLength)
      {
        commandHistory[*historyCounter - 1][k] = *(command + k);
        k++;
      }
      commandHistory[*historyCounter - 1][k] = '\0';
    }
  }
  else
    uart_send(uart, '\n');
  uart_send(uart, '>');

  *commandCursor = 0;
  *commandLength = 0;
}
void *commandLength_pos(int uart, int pos)
{

  int temp = pos + 2;
  int temp2 = pos + 2;
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
    kprintf("%d ---", array[i]);
  }
}

void takeHistory(int uart, int pos, int *commandLength, int *commandCursor, unsigned char *command)
{
  int i = 0;

  while (commandHistory[pos][i] != '\0')
  {
    *(command + i) = commandHistory[pos][i];
    uart_send(uart, *(command + i));
    i++;
  }
  *(command + i) = '\0';
  *commandCursor = i;
  *commandLength = i;
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

  uart_send(uart, '>');
}

void moveCursor(int pos)
{
  // Move cursor to the required position
  uart_send(0, 27);
  uart_send(0, 91);
  if (pos > 9)
  {
    kprintf("%d, ", pos);
    commandLength_pos(0, pos);
  }
  else
    uart_send(0, 50 + pos);
  uart_send(0, 71);
}

void moveCursorBegin()
{
  uart_send(0, 27);
  uart_send(0, 91);
  uart_send(0, 71);

  uart_send(0, '>');
}

void uart_commandline(unsigned char *s, int uart, int *UpDownMove, int *commandLength, int *commandCursor,
                      int *historyCounter, int *historyFull, int *escapeSeq, unsigned char *command)
{
  int options = *s;
  if (*escapeSeq == 1 && *(escapeSeq + 1) == 1)
  {

    // Read the other value
    switch (options)
    {
    case 65: // Up Arrow
      if (*historyCounter >= *UpDownMove + 1)
      {
        (*UpDownMove)++;
        moveCursorBeginErase(uart);
        takeHistory(uart, *historyCounter - *UpDownMove, commandLength, commandCursor, command);
      }
      kprintf("%d", *UpDownMove);
      break;
    case 66: // Down Arrow

      (*UpDownMove)--;
      if (*UpDownMove > 0)
      {
        moveCursorBeginErase(uart);
        takeHistory(uart, *historyCounter - *UpDownMove, commandLength, commandCursor, command);
      }
      else if (*UpDownMove == 0)
      {
        moveCursorBeginErase(uart);
        *commandCursor = 0;
        *commandLength = 0;
      }
      else
        *UpDownMove = 0;

      break;

    case 67:                               // Right Arrow
      if (*commandCursor < *commandLength) // if the cursor is not at the end move to right
      {
        uart_send(uart, 27);
        uart_send(uart, 91);
        uart_send(uart, 67);
        (*commandCursor)++;
      }

      break;

    case 68: // Left Arrow

      if (*commandCursor > 0)
      {
        uart_send(uart, 8); // backspace key
        (*commandCursor)--;
      }

      break;

    case 126: // Delete key

      if (*commandCursor < *commandLength)
      {

        int tempLegth = (*commandLength) - (*commandCursor) - 1;
        unsigned char tempArray[tempLegth];

        // kprintf("%d,%d,%d--- ", *commandCursor, *commandLength, tempLegth);

        for (int i = 0; i < tempLegth; i++)
        {
          tempArray[i] = *(command + (*commandCursor) + i + 1);
        }
        tempArray[tempLegth] = '\0';
        for (int j = *commandCursor; j < *commandLength - 1; j++)
        {
          *(command + j) = tempArray[j - *commandCursor];
        }
        (*commandLength)--;
        *(command + *commandLength) = '\0';

        // moveCursorBeginErase(uart);
        uart_send(uart, 27);
        uart_send(uart, 91);
        uart_send(uart, 75);

        // Print the command
        uart_send_string(0, tempArray);

        // Move cursor to the required position
        moveCursor(*commandCursor);
      }
      break;
    }
    // set the escapeSeq to 0;
    if (options >= 64 && options <= 126)
    {
      *escapeSeq = 0;
      *(escapeSeq + 1) = 0;
    }
  }
  else if (*escapeSeq == 1 && options == 91)
  {
    *(escapeSeq + 1) = 1;
  }
  else if (options == 27)
  {
    *escapeSeq = 1;
  }
  else if (options == 127) // backspace key
  {
    if (*commandCursor > 0)
      uart_send(uart, 8); // Backspace
    uart_send(uart, 27);
    uart_send(uart, 91);
    uart_send(uart, 75);

    uart_send(1, 27);
    uart_send(1, 91);
    uart_send(1, 49);
    uart_send(1, 71);

    if (*commandCursor < *commandLength)
    {
      unsigned char tempArray[*commandLength - *commandCursor];
      for (int i = 0; i < *commandLength - *commandCursor; i++)
      {
        tempArray[i] = *(command + *commandCursor + i);
      }

      for (int j = *commandCursor - 1; j < *commandLength - 1; j++)
      {
        *(command + j) = tempArray[j - (*commandCursor - 1)];
      }
      (*commandLength)--;
      (*commandCursor)--;
      *(command + *commandLength) = '\0';

      // Move Cursor to the beginning of a live
      moveCursorBegin();

      // Print the command
      uart_send_string(0, command);

      // Move cursor to the required position
      moveCursor(*commandCursor);

      *commandLength = (*commandLength) + 1;
      *commandCursor = (*commandCursor) + 1;
    }

    if (*commandCursor > 0)
    {
      (*commandLength)--;
      (*commandCursor)--;
    }
  }
  else if (options == 13)
  {
    commandExecution(uart, commandLength, commandCursor, historyCounter, historyFull, command);
  }
  else if (*commandLength == *commandCursor)
  {

    command[*commandLength] = options;
    if (*(command + *commandLength) == '\r')
    {
      uart_send(uart, '\n');
    }
    uart_send(uart, *(command + *commandLength));

    (*commandLength)++;
    (*commandCursor)++;

    *(command + *commandLength) = '\0';
  }
  else if (*commandLength > *commandCursor)
  {

    unsigned char tempArray[*commandLength - *commandCursor];
    for (int i = 0; i < *commandLength - *commandCursor; i++)
    {
      tempArray[i] = *(command + (*commandCursor) + i);
    }

    for (int j = (*commandCursor) + 1; j < (*commandLength) + 1; j++)
    {
      *(command + j) = tempArray[j - (*commandCursor + 1)];
    }

    *(command + *commandCursor) = options;
    (*commandLength)++;
    (*commandCursor)++;
    *(command + *commandLength) = '\0';

    // Move Cursor to the beginning of a live
    moveCursorBegin();

    // Print the command
    uart_send_string(0, command);

    // Move cursor to the required position
    moveCursor(*commandCursor);
  }
}

/**
 * This is the C entry point, upcalled once the hardware has been setup properly
 * in assembly language, see the reset.s file.
 */

void _start()
{

  uart_init();
  int a = 0, b = 0, c = 0, d = 0, e = 0;
  int count = 0;
  int *commandLength;
  int *commandCursor;
  int *historyCounter;
  int *historyFull;
  int *UpDownMove;
  int escapeSeq[2];
  unsigned char command[MAXCOMANDCHARSIZE];

  commandLength = &a;
  commandCursor = &b;
  UpDownMove = &c;
  historyCounter = &d;
  historyFull = &e;

  uart_clear(0);
  uart_send_string(0, "\nQuit with \"C-a c\" and then type in \"quit\".\n");

  uart_send(0, '>');

  while (1)
  {
    unsigned char c;
    while (1 == uart_receive(0, &c))
    {
      // friendly reminder that you are polling and therefore spinning...
      // not good for the planet! But until we introduce interrupts,
      // there is nothing you can do about it... except comment out
      // // this annoying code ;-)
      // count++;
      // if (count > 10000000) {
      //   uart_send_string(0, "\n\rZzzz....\n\r");
      //   count = 0;
      // }
      uart_commandline(&c, 0, UpDownMove,
                       commandLength, commandCursor, historyCounter,
                       historyFull, escapeSeq, command);

      kprintf("%x, %d \n \r", c, c);
    }

    wfi();
  }
}