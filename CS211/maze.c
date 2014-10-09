#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef enum {FALSE = 0, TRUE, NO = 0, YES} boolean;

typedef enum {UNVISITED = 0, VISITED} visitType;

typedef enum {BLOCKED = 0, UNBLOCKED, START, END} mazeType;

typedef struct mazeNode {

    mazeType type;
    int xpos;
    int ypos;
    visitType visited;
    struct mazeNode * next;

} mazeNode;

typedef struct mazeStruct {

    mazeNode ** arr;
    int xsize, ysize;
    int xstart, ystart;
    int xend, yend;

} maze;

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
    dataToken->type = data.type;
    dataToken->visited = data.visited;
    dataToken->xpos = data.xpos;
    dataToken->ypos = data.ypos;
    dataToken->next = NULL;

    if(!isEmpty(*stack))
        dataToken->next = *stack;

    *stack = dataToken;

}

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

        mazePtr topOfStack = *stack;
        *stack = topOfStack->next;

        free(topOfStack);

    }
    else
        fprintf(stderr, "Stack is empty! Cannot pop...\n");

}

/*
Prints contents of the stack in reverse order
*/
void printSolution(mazePtr stack) {

    if(isEmpty(stack))
        return;

    printSolution(stack->next);
    printf("(%d, %d)\n", stack->xpos, stack->ypos);

}

/*
Implements Depth First Search to find the solution of the maze
*/
void findSolution(maze * m1) {

    // Mark all unblocked positions in the maze as "UNVISITED"
    for(int i = 0; i < m1->xsize+2; ++i)
        for(int j = 0; j < m1->ysize+2; ++j)
            if(m1->arr[i][j].type == UNBLOCKED)
                m1->arr[i][j].visited = UNVISITED;

    // push the start position's coordinates on the stack, mark visited
    mazePtr stack;
    push(&stack, m1->arr[m1->xstart][m1->ystart]);
    stack->visited = VISITED;

    while(!isEmpty(stack)) {

        // end has been found
        mazePtr topOfStack = top(stack);
        if(topOfStack->type == END) {

            printSolution(stack);
            while(!isEmpty(stack))
                pop(&stack);
            return;

        }

        // coordinate at top of stack has an unvisited and unblocked neighbor
        // if(!neighborVisited(m1, &stack))
        //     pop(&stack);
        int x = topOfStack->xpos, y = topOfStack->ypos;
        mazeNode neighbor;

        boolean noNeighbor = NO;

        // first checks below
        if(x != m1->xsize && noNeighbor == NO) {

            neighbor = m1->arr[x+1][y];
            if((neighbor.type == UNBLOCKED || neighbor.type == END) && neighbor.visited == UNVISITED) {

                m1->arr[x+1][y].visited = VISITED;
                push(&stack, m1->arr[x+1][y]);
                noNeighbor = YES;

            }

        }

        // checks top
        if(x != 1 && noNeighbor == NO) {

            neighbor = m1->arr[x-1][y];
            if((neighbor.type == UNBLOCKED || neighbor.type == END) && neighbor.visited == UNVISITED) {

                m1->arr[x-1][y].visited = VISITED;
                push(&stack, m1->arr[x-1][y]);
                noNeighbor = YES;

            }

        }

        // checks to the right
        if(y != m1->ysize && noNeighbor == NO) {

            neighbor = m1->arr[x][y+1];
            if((neighbor.type == UNBLOCKED || neighbor.type == END) && neighbor.visited == UNVISITED) {

                m1->arr[x][y+1].visited = VISITED;
                push(&stack, m1->arr[x][y+1]);
                noNeighbor = YES;

            }

        }

        // checks to the left
        if(y != 1 && noNeighbor == NO) {

            neighbor = m1->arr[x][y-1];
            if((neighbor.type == UNBLOCKED || neighbor.type == END) && neighbor.visited == UNVISITED) {

                m1->arr[x][y-1].visited = VISITED;
                push(&stack, m1->arr[x][y-1]);
                noNeighbor = YES;

            }

        }

        if(noNeighbor == NO) {

            if(DEBUG)
                printf("[DEBUG] coordinate (%d,%d) does not have an unvisited (and unblocked) neighbor\n", x, y);
            pop(&stack);

        }


    }

    printf("No solution was found...\n");

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
    if(argc < 2) {
        printf("Usage: %s [-d] <input file name>\n", argv[0]);
        exit(-1);
    }

    char * input_file = argv[1];
    if(argc == 3) {

        DEBUG = TRUE;
        printf("strcmp(input_file, -d) = %d\n", strcmp(input_file, "-d"));
        if(strcmp(input_file, "-d") == 0)
            input_file = argv[2];

    }
    // printf("%d\n", argc);
    // return 0;
    /* Try to open the input file. */
    if ( ( src = fopen( input_file, "r" )) == NULL )
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
    m1.arr = (mazeNode **) malloc (sizeof(mazeNode *) * (m1.xsize+2));
    for (i = 0; i < m1.xsize+2; i++)
        m1.arr[i] = (mazeNode *) malloc (sizeof(mazeNode) * (m1.ysize+2));

    /* initialize the maze to empty */
    for (i = 0; i < m1.xsize+2; i++)
        for (j = 0; j < m1.ysize+2; j++) {

            m1.arr[i][j].type = UNBLOCKED;
            m1.arr[i][j].xpos = i;
            m1.arr[i][j].ypos = j;

        }

    /* mark the borders of the maze with *'s */
    for (i=0; i < m1.xsize+2; i++)
    {
        m1.arr[i][0].type = BLOCKED;
        m1.arr[i][m1.ysize+1].type = BLOCKED;
    }
    for (i=0; i < m1.ysize+2; i++)
    {
        m1.arr[0][i].type = BLOCKED;
        m1.arr[m1.xsize+1][i].type = BLOCKED;
    }

    /* mark the starting and ending positions in the maze */
    m1.arr[m1.xstart][m1.ystart].type = START;
    m1.arr[m1.xend][m1.yend].type = END;

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
            m1.arr[xpos][ypos].type = BLOCKED;

        fprintf(stderr, "\n");

    }

    /* print out the initial maze */
    for (i = 0; i < m1.xsize+2; i++)
    {
        for (j = 0; j < m1.ysize+2; j++) {

            if(m1.arr[i][j].type == BLOCKED)
                printf("*");
            else if(m1.arr[i][j].type == UNBLOCKED)
                printf(".");
            else if(m1.arr[i][j].type == START)
                printf("s");
            else
                printf("e");

        }
        printf("\n");
    }

    // return 0;

    findSolution(&m1);

}
