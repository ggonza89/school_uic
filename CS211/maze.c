#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef enum {FALSE = 0, TRUE, NO = 0, YES} boolean;

typedef enum {UNVISITED = 0, VISITED} visitType;

typedef struct mazeStruct
{
//    char arr[32][32];  /* allows for a maze of size 30x30 plus outer walls */
    char ** arr;
    int xsize, ysize;
    int xstart, ystart;
    int xend, yend;
} maze;

typedef struct mazeNode {

    int xpos;
    int ypos;
    visitType visited;
    struct mazeNode * next;

} mazeNode;

typedef mazeNode * mazePtr;

boolean DEBUG = FALSE;

/*
checks if top of stack is NULL, if it is it is empty
*/
boolean isEmpty(mazePtr stack) {

    if(stack != NULL)
        return FALSE;

    return TRUE;

}

/* returns the length of the list provided as a linkedPtr */
int listLength (mazePtr stack)
{

    int length = 0;

    while (stack != NULL) {

        length++;
        stack = stack->next;

    }

    return length;

}

/*
if stack is not empty, make the top of the stack be the new data token
make the data token the new top of the stack
*/
void push(mazePtr * stack, mazeNode data) {

    mazePtr dataToken = (mazePtr) malloc (sizeof(mazeNode));
    dataToken->visited = data.visited;
    dataToken->xpos = data.xpos;
    dataToken->ypos = data.ypos;
    dataToken->next = NULL;

    if(!isEmpty(*stack))
        dataToken->next = *stack;

    *stack = dataToken;

}

/*
displays the contents of the list
*/
// void displayListInformation (mazePtr stack) {

//     while (stack != NULL) {

//         if(stack->visited == VISITED)
//             printf("*", stack->val);
//         else
//             printf("%c", stack->op);

//         // printNodestack);
//         stack = stack->next;
//         if(stack != NULL)
//             printf(", ");

//     }

//     printf ("\n");

// }

/*
return the top of the stack, if stack empty return NULL
*/
mazePtr top(mazePtr stack) {

    if(!isEmpty(stack))
        return stack;

    return NULL;

}

/*
pop off the top of the stack
*/
void pop(mazePtr * stack) {

    if(!isEmpty(*stack)) {

        tokenPtr topOfStack = *stack;
        *stack = topOfStack->next;

        free(topOfStack);

    }
    else
        fprintf(stderr, "Stack is empty! Cannot pop...\n");

}

boolean validInput(int xpos, int ypos, int mazeXSize, int mazeYSize) {

    fprintf(stderr, "%d %d\t", xpos, ypos);

    if( xpos == 0 || ypos == 0) {

        fprintf (stderr, "=> Invalid: Maze sizes must be greater than 0");
        return NO;

    }
    else if(xpos > mazeXSize) {

        fprintf(stderr, "=> Invalid: row %d is outside range from 1 to %d", xpos, mazeXSize);
        return NO;

    }
    else if(ypos > mazeYSize) {

        fprintf(stderr, "=> Invalid: column %d is outside range from 1 to %d", ypos, mazeYSize);
        return NO;

    }
    else
        return YES;

}

int main (int argc, char **argv)
{
    maze m1;

    int xpos, ypos;
    int i,j;

    FILE *src;

    /* verify the proper number of command line arguments were given */
    if(argc != 2) {
        printf("Usage: %s <input file name>\n", argv[0]);
        exit(-1);
    }

    /* Try to open the input file. */
    if ( ( src = fopen( argv[1], "r" )) == NULL )
    {
        printf ( "Can't open input file: %s", argv[1] );
        exit(-1);
    }

    /* read in the size of maze */
    while(fscanf (src, "%d %d", &m1.xsize, &m1.ysize) != EOF) {

        if(validInput(m1.xsize, m1.ysize, INT_MAX, INT_MAX)) {

            fprintf(stderr, "=> Maze becomes size %d x %d\n", m1.xsize, m1.ysize);
            break;

        }

        fprintf(stderr, "\n");

    }

    /* read in starting position of maze */
    while(fscanf (src, "%d %d", &m1.xstart, &m1.ystart) != EOF) {

        if(validInput(m1.xstart, m1.ystart, m1.xsize, m1.ysize)) {

            fprintf(stderr, "=> Starting position is at position %d, %d\n", m1.xstart, m1.ystart);
            break;

        }

        fprintf(stderr, "\n");

    }

    /* read in ending position of maze */
    while(fscanf (src, "%d %d", &m1.xend, &m1.yend) != EOF) {

        // fscanf (src, "%d %d", &m1.xend, &m1.yend);
        if(validInput(m1.xend, m1.yend, m1.xsize, m1.ysize)) {

            fprintf(stderr, "=> Ending position is at position %d, %d\n", m1.xend, m1.yend);
            break;

        }

        fprintf(stderr, "\n");

    }

    /* allocate the maze */
    m1.arr = (char **) malloc (sizeof(char *) * (m1.xsize+2));
    for (i = 0; i < m1.xsize+2; i++)
        m1.arr[i] = (char *) malloc (sizeof(char ) * (m1.ysize+2));

    /* initialize the maze to empty */
    for (i = 0; i < m1.xsize+2; i++)
        for (j = 0; j < m1.ysize+2; j++)
            m1.arr[i][j] = '.';

    /* mark the borders of the maze with *'s */
    for (i=0; i < m1.xsize+2; i++)
    {
        m1.arr[i][0] = '*';
        m1.arr[i][m1.ysize+1] = '*';
    }
    for (i=0; i < m1.ysize+2; i++)
    {
        m1.arr[0][i] = '*';
        m1.arr[m1.xsize+1][i] = '*';
    }

    /* mark the starting and ending positions in the maze */
    m1.arr[m1.xstart][m1.ystart] = 's';
    m1.arr[m1.xend][m1.yend] = 'e';

    /* mark the blocked positions in the maze with *'s */
    while (fscanf (src, "%d %d", &xpos, &ypos) != EOF)
    {

        fprintf(stderr, "%d %d\t", xpos, ypos);
        // if xpos or ypos not in bounds, skip that input line
        if(xpos > m1.xsize)
            fprintf(stderr, "=> Invalid: row %d is outside range from 1 to %d", xpos, m1.xsize);
        else if(ypos > m1.ysize)
            fprintf(stderr, "=> Invalid: column %d is outside range from 1 to %d", ypos, m1.ysize);
        // if (xpos,ypos) is the starting point, skip input line
        else if(xpos == m1.xstart && ypos == m1.ystart)
            fprintf(stderr, "=> Invalid: attempting to block starting position");
        // default is to mark spot as blocked
        else
            m1.arr[xpos][ypos] = '*';

        fprintf(stderr, "\n");

    }

    /* print out the initial maze */
    for (i = 0; i < m1.xsize+2; i++)
    {
        for (j = 0; j < m1.ysize+2; j++)
            printf ("%c", m1.arr[i][j]);
        printf("\n");
    }



}
