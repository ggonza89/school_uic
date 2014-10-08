#include <stdio.h>
#include <stdlib.h>

typedef struct mazeStruct
{
//    char arr[32][32];  /* allows for a maze of size 30x30 plus outer walls */
    char ** arr;
    int xsize, ysize;
    int xstart, ystart;
    int xend, yend;
} maze;

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
    fscanf (src, "%d %d", &m1.xsize, &m1.ysize);
    fprintf(stderr, "%d %d\t", m1.xsize, m1.ysize);
    
    while(m1.xsize < 3 || m1.ysize < 3) {
        
        fprintf (stderr, "=> Invalid: Maze sizes must be greater than 0\n");
        fscanf(src, "%d %d", &m1.xsize, &m1.ysize);
        fprintf(stderr, "%d %d\t", m1.xsize, m1.ysize);
        
    }
    
    fprintf(stderr, "=> Maze becomes size %d x %d\n", m1.xsize, m1.ysize);
    
    /* read in starting position of maze */
    while(1) {
        
        fscanf (src, "%d %d", &m1.xstart, &m1.ystart);
        fprintf(stderr, "%d %d\t", m1.xstart, m1.ystart);
        
        if(m1.xstart > m1.xsize)
            fprintf(stderr, "=> Invalid: row %d is outside range from 1 to %d", m1.xstart, m1.xsize);
        else if(m1.ystart > m1.ysize)
            fprintf(stderr, "=> Invalid: column %d is outside range from 1 to %d", m1.ystart, m1.ysize);
        else {
            fprintf(stderr, "=> Starting position is at position %d, %d\n", m1.xstart, m1.ystart);
            break;
        }
        
        fprintf(stderr, "\n");
        
    }

    /* read in ending position of maze */
    while(1) {
        
        fscanf (src, "%d %d", &m1.xend, &m1.yend);
        fprintf(stderr, "%d %d\t", m1.xend, m1.yend);
        
        if(m1.xend > m1.xsize)
            fprintf(stderr, "=> Invalid: row %d is outside range from 1 to %d", m1.xend, m1.xsize);
        else if(m1.yend > m1.ysize)
            fprintf(stderr, "=> Invalid: column %d is outside range from 1 to %d", m1.yend, m1.ysize);
        else {
            fprintf(stderr, "=> Ending position is at position %d, %d\n", m1.xend, m1.yend);
            break;
        }
        
        fprintf(stderr, "\n");
        
    }
    
    /* print them out to verify the input */
//    printf ("size: %d, %d\n", m1.xsize, m1.ysize);
//    printf ("start: %d, %d\n", m1.xstart, m1.ystart);
//    printf ("end: %d, %d\n", m1.xend, m1.yend);
    
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