/*  Code for the user interface for Lab 4 for CS 211 Fall 2014
 *
 *  Author: Pat Troy
 *  Date: 10/6/2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {FALSE = 0, TRUE, NO = 0, YES} boolean;
struct linkedStruct {

    char name[30];
    int size;
    boolean waiting;
    struct linkedStruct * next;

};

typedef struct linkedStruct linked;
typedef linked* linkedPtr;

boolean DEBUG = FALSE;

/* prints the contents of the node given */
void printNode(linkedPtr node) {

    if(node->waiting == YES)
            printf("(%s, %d, waiting)", node->name, node->size);
        else
            printf("(%s, %d, call-ahead)", node->name, node->size);

}

/* prints debugging info, in this case the node while it traverses a */
/* list */
void debugInfo(linkedPtr node) {

    // printf("***DEBUG: ");
    if(DEBUG) {

        // switch(func_in) {

        //     case 0:

        // }
        printNode(node);
        printf("\n");

    }

}

/* checks if the group (name) is waiting, if it is not waiting change */
/* change the status from call-ahead to waiting and return YES or NO */
boolean updateStatus(linkedPtr * hd, char * name) {

    linkedPtr curr = *hd;
    while(curr != NULL) {

        debugInfo(curr);

        if(strncmp(name, curr->name, 30) == 0) {

            if(curr->waiting == YES)
                return NO;

            curr->waiting = YES;
            return YES;

        }
        else
            curr = curr->next;

    }

    return NO;

}

/* checks to see if a group represented as a name is in the linked */
/* list hd returns a YES or NO boolean type */
boolean doesNameExist(linkedPtr hd, char * name) {

    while(hd != NULL) {

        debugInfo(hd);

        if(strncmp(name, hd->name, 30) == 0)
            return YES;

        hd = hd->next;

    }

    return NO;

}

/* adds a new node to the end of the list hd with the info being */
/* passed as parameters size, name, and waiting */
void addToList (linkedPtr* hd, int size, char * name, boolean waiting) {

    linkedPtr curr = *hd;
    linkedPtr prev = NULL;

    while (curr != NULL)
    {

        debugInfo(curr);
        prev = curr;
        curr = curr->next;

    }

    linkedPtr ptr = (linkedPtr) malloc (sizeof(linked));
    ptr->size = size;
    strncpy(ptr->name, name, 30);
    ptr->waiting = waiting;
    ptr->next = curr;

    if (prev == NULL)
    {
        *hd = ptr;
    }
    else
    {
        prev->next = ptr;
    }

}

/* displays the contents of the list */
/* node is printed out as (name, size, waiting or call-ahead) */
void displayListInformation (linkedPtr hd) {

    while (hd != NULL) {

        printNode(hd);
        hd = hd->next;
        if(hd != NULL)
            printf("->");

    }

    printf ("\n");

}

/* returns the length of the list provided as a linkedPtr */
int listLength (linkedPtr hd)
{

    int length = 0;

    while (hd != NULL) {

        debugInfo(hd);
        length++;
        hd = hd->next;

    }

    return length;

}


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

/* read in the name until the end of the input */
char *getName()
{
    /* skip over the white space characters */
    int ch;
    ch = getc(stdin);
    if ('\n' == ch)  /* error \n ==> no integer given */
            return NULL;
    ch = getc(stdin);

    char *word;
    int size;
    size = 10;
    word = (char *) malloc (sizeof(char) * size);

    // read in character-by-character until the newline is encountered
    int count = 0;

    while (ch != '\n')
    {
        if (count+1 >= size)
        {
            // to grow an array to make it "dynamically sized" using malloc
            char* temp;
            int i;
            size = size * 2;
            temp = (char *) malloc (sizeof(char) * size);

            // printf ("Growing array from size %d to size %d\n", count, size);
            // copy the characters to the new array
            for (i = 0 ; i < count ; i++)
                temp[i] = word[i];

            free (word);
            word = temp;
        }

        word[count] = ch;
        count++;
        word[count] = '\0';

        // read next character
        ch = getc(stdin);
    }

    if (count > 30)
    {
        count = 30;
        word[count] = '\0';
    }

    /* clear ending white space characters */
    while (isspace (word[count-1]))
    {
        count--;
        word[count] = '\0';
    }

    return word;
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
void printCommands()
{
    printf ("The commands for this program are:\n\n");
    printf ("q - to quit the program\n");
    printf ("? - to list the accepted commands\n");
    printf ("a <size> <name> - to add a group to the wait list\n");
    printf ("c <size> <name> - to add a call-ahead group to the wait list\n");
    printf ("w <name> - to specify a call-ahead group is now waiting in the restaurant\n");
    printf ("r <table-size> - to retrieve the first waiting group that can fit at the available table size\n");
    printf ("l <name> - list how many groups are ahead of the named group\n");
    printf ("d - display the wait list information\n");

    /* clear input to End of Line */
    clearToEoln();
}

void doAdd (linkedPtr* hd)
{
    /* get group size from input */
    int size = getPosInt();
    if (size < 1)
    {
        printf ("Error: Add command requires an integer value of at least 1\n");
        printf ("Add command is of form: a <size> <name>\n");
        printf ("  where: <size> is the size of the group making the reservation\n");
        printf ("         <name> is the name of the group making the reservation\n");
        return;
    }

    /* get group name from input */
    char *name = getName();
    if (NULL == name)
    {
        printf ("Error: Add command requires a name to be given\n");
        printf ("Add command is of form: a <size> <name>\n");
        printf ("  where: <size> is the size of the group making the reservation\n");
        printf ("         <name> is the name of the group making the reservation\n");
        return;
    }

    if(doesNameExist(*hd, name))
        printf("Group \"%s\" is already on the waiting list\n", name);
    else {

        printf ("Adding group \"%s\" of size %d\n", name, size);
        addToList(hd, size, name, YES);

    }

}


void doCallAhead (linkedPtr * hd)
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

    addToList(hd, size, name, 0);

}

void doWaiting (linkedPtr * hd)
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

    // add code to perform this operation here
    // linkedPtr curr = *hd;

    if(doesNameExist(*hd, name)) {

        if(updateStatus(hd, name))
            printf ("Group \"%s\" is now in the restaurant\n", name);
        else
            printf("Group \"%s\" is already in the restaurant\n", name);

    }
    else
        printf("Group \"%s\" is not on the waiting list\n", name);

}

char * retreiveAndRemove(linkedPtr * hd, int size) {

    linkedPtr curr = *hd;
    linkedPtr prev = NULL;

    while(curr != NULL) {

        debugInfo(curr);

        if(curr->size <= size && curr->waiting) {

            //retreive group from list
            char * name;
            name = (char*)malloc(sizeof(char) * strlen(curr->name));
            strncpy(name, curr->name, 30);

            //remove group from list
            if(prev == NULL)
                *hd = curr->next;
            else
                prev->next = curr->next;

            free(curr);
            return name;

        }

        prev = curr;
        curr = curr->next;

    }

    return NULL;

}

void doRetrieve (linkedPtr * hd)
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
    printf ("Retrieve (and remove) the first group that can fit at a table of size %d\n", size);

    // add code to perform this operation here

    char * name = retreiveAndRemove(hd, size);
    if(name == NULL)
        printf("No group can fit at table of size %d\n", size);
    else {

        printf("Group \"%s\" can fit at the table with size %d\n", name, size);
        free(name);

    }

}

int countGroupsAhead(linkedPtr hd, char * name) {

    int num_groups_before = 0;

    while(hd != NULL) {

        debugInfo(hd);

        if(strncmp(hd->name, name, 30) == 0)
            return num_groups_before;

        ++num_groups_before;
        hd = hd->next;

    }

    return -1;

}

void displayGroupSizeAhead(linkedPtr hd, char * name) {

    while(strncmp(hd->name, name, 30) != 0) {

        debugInfo(hd);

        printf("Size of group: %d\n", hd->size);

        hd = hd->next;

    }

}

void doList (linkedPtr hd)
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

    // add code to perform this operation here

    int num_groups_before = countGroupsAhead(hd, name);
    if(num_groups_before > 0) {

        printf ("Group \"%s\" is behind %d groups\n", name, num_groups_before);
        displayGroupSizeAhead(hd, name);

    }
    else if(num_groups_before == 0)
        printf("Group \"%s\" is in the head of the list\n", name);
    else
        printf("Group \"%s\" does not exist\n", name);

}

void doDisplay (linkedPtr hd)
{

    clearToEoln();
    int list_length = listLength(hd);

    if(list_length == 0)
        printf("Waiting list is empty\n");
    else {

        printf ("Display information about all groups\n");

        // add code to perform this operation here
        printf("Number of groups on the waiting list is %d\n", list_length);

        displayListInformation(hd);

    }

}

void clean(linkedPtr * hd) {

    if(hd == NULL)
        return;

    linkedPtr curr = *hd;
    linkedPtr prev = NULL;

    do {

        debugInfo(curr);

        prev = curr;
        curr = curr->next;
        free(prev);

    }while(curr != NULL);

}


int main (int argc, char **argv)
{

    if(argc == 2) {

        DEBUG = TRUE;

    }
    else if (argc > 2){

        printf("Usage: %s [-d]\n", argv[0]);
        exit(0);

    }

    linkedPtr head = NULL;

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
        else if('a' == ch)
        {
            doAdd(&head);
        }
        else if('c' == ch)
        {
            doCallAhead(&head);
        }
        else if('w' == ch)
        {
            doWaiting(&head);
        }
        else if('r' == ch)
        {
            doRetrieve(&head);
        }
        else if('l' == ch)
        {
            doList(head);
        }
        else if('d' == ch)
        {
            doDisplay(head);
        }
        else
        {
            printf ("%c - is not a valid command\n", ch);
            printf ("For a list of valid commands, type ?\n");
            clearToEoln();
        }

        // displayListInformation(head);
        // printf("Length of list: %d\n", listLength(head));

        printf ("\nEnter command: ");
    }

    printf ("Quiting Program - EOF reached\n");
    clean(&head);

    return 1;

}
