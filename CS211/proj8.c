/*  Code for the user interface for Lab 4 for CS 211 Fall 2014
 *
 *  Author: Pat Troy
 *  Date: 10/6/2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {FALSE = 0, TRUE, NO = 0, YES} boolean;

/* forward definition of functions */
void clearToEoln();

/* Read in until the first Non-White-Space character is Read */
/* The white space characters are:
 *      space, tab \t, newline \n, vertical tab \v,
 *      form feed \f, and carriage return \r
 */
int getNextNWSChar ()
{
 int ch;

 ch = getc(stdin);
 if (ch == EOF)
   return ch;
 while (isspace (ch))
   {
    ch = getc(stdin);
    if (ch == EOF)
      return ch;
   }
 return ch;
}

/* read in the next Positive Integer or error:    */
/* This is based on the Mathematical definition of a Postive Integer */
/*    zero is not counted as a positive number */
int getPosInt ()
{
 int value = 0;

 /* clear white space characters */
 int ch;
 ch = getc(stdin);
 while (!isdigit(ch))
   {
    if ('\n' == ch)  /* error \n ==> no integer given */
       return 0;
    if (!isspace(ch)) /* error non white space ==> integer not given next */
      {
       clearToEoln();
       return 0;
      }
    ch = getc(stdin);
   }

 value = ch - '0';
 ch = getc(stdin);
 while (isdigit(ch))
   {
    value = value * 10 + ch - '0';
    ch = getc(stdin);
   }

 ungetc (ch, stdin);  /* put the last read character back in input stream */

 /* Integer value of 0 is an error in this program */
 if (0 == value)
    clearToEoln();

 return value;
}

/* Clear input until next End of Line Character - \n */
void clearToEoln()
{
 int ch;

 do {
     ch = getc(stdin);
    }
 while ((ch != '\n') && (ch != EOF));
}

/* Print out a list of the commands for this program */
void printCommands() {

  printf ("The commands for this program are:\n\n");
  printf ("q - to quit the program\n");
  printf ("? - to list the accepted commands\n");
  printf ("i <int> - insert the integer value into the hash table.\n");
  printf ("d <int> - delete the integer value from the hash table.\n");
  printf ("c <int> - display a message stating whether the given integer value is contained in the hash table.\n");
  printf ("r <int> - resize the array to contain the number of positions indicated by the given integer value.\n");
  printf ("l - list all the items contained in the hash table and include which array position they are located.\n");
  printf ("e - erase all of the values currently stored in the hash table.\n");

  /* clear input to End of Line */
  clearToEoln();

}

void doInsert (HashTable * hashTable) {

  /* get group size from input */
  int value = getPosInt();
  if (size < 1) {

    printf ("Error: insert command requires an integer value of at least 1\n");
    printf ("insert command is of form: i <int>\n");
    printf ("  where: <int> is the value to be inserted into the hash table.\n");

    return;

  }

  int pos = value % hashTable->size;

  printf ("Inserting value %d to position %d\n", value, pos);

  // add code to perform this operation here
}


void doCallAhead ()
{
 /* get group size from input */
 int size = getPosInt();
 if (size < 1)
   {
    printf ("Error: Call-ahead command requires an integer value of at least 1\n");
    printf ("Call-ahead command is of form: c <size> <name>\n");
    printf ("  where: <size> is the size of the group making the reservation\n");
    printf ("         <name> is the name of the group making the reservation\n");
    return;
   }

 /* get group name from input */
 char *name = getName();
 if (NULL == name)
   {
    printf ("Error: Call-ahead command requires a name to be given\n");
    printf ("Call-ahead command is of form: c <size> <name>\n");
    printf ("  where: <size> is the size of the group making the reservation\n");
    printf ("         <name> is the name of the group making the reservation\n");
    return;
   }

 printf ("Call-ahead group \"%s\" of size %d\n", name, size);

 // add code to perform this operation here
}

void doWaiting ()
{
 /* get group name from input */
 char *name = getName();
 if (NULL == name)
   {
    printf ("Error: Waiting command requires a name to be given\n");
    printf ("Waiting command is of form: w <name>\n");
    printf ("  where: <name> is the name of the group that is now waiting\n");
    return;
   }

 printf ("Waiting group \"%s\" is now in the restaurant\n", name);

 // add code to perform this operation here
}

void doRetrieve ()
{
 /* get table size from input */
 int size = getPosInt();
 if (size < 1)
   {
    printf ("Error: Retrieve command requires an integer value of at least 1\n");
    printf ("Retrieve command is of form: r <size>\n");
    printf ("  where: <size> is the size of the group making the reservation\n");
    return;
   }
 clearToEoln();
 printf ("Retrieve (and remove) the first group that can fit at a tabel of size %d\n", size);

 // add code to perform this operation here
}

void doList ()
{
 /* get group name from input */
 char *name = getName();
 if (NULL == name)
   {
    printf ("Error: List command requires a name to be given\n");
    printf ("List command is of form: l <name>\n");
    printf ("  where: <name> is the name of the group to inquire about\n");
    return;
   }

 printf ("Group \"%s\" is behind the following groups\n", name);

 // add code to perform this operation here
}

void doDisplay ()
{
 clearToEoln();
 printf ("Display information about all groups\n");

 // add code to perform this operation here
}


int main (int argc, char **argv)
{

 char *input;
 int ch;

 printf ("Starting Restaurant Wait List Program\n\n");
 printf ("Enter command: ");

 while ((ch = getNextNWSChar ()) != EOF)
   {
    /* check for quit */
    if ('q' == ch)
      {
       printf ("Quitting Program\n");
       return (0);
      }
    else if ('?' == ch)
      {
       printCommands();
      }
    else if('i' == ch)
      {
       doInsert();
      }
    else if('c' == ch)
      {
       doCallAhead();
      }
    else if('w' == ch)
      {
       doWaiting();
      }
    else if('r' == ch)
      {
       doRetrieve();
      }
    else if('l' == ch)
      {
       doList();
      }
    else if('d' == ch)
      {
       doDisplay();
      }
    else
      {
       printf ("%c - in not a valid command\n", ch);
       printf ("For a list of valid commands, type ?\n");
       clearToEoln();
      }

    printf ("\nEnter command: ");
   }

 printf ("Quiting Program - EOF reached\n");
 return 1;
}
