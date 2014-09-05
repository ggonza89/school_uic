#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void printArray(int array[], int length) {

    for( int i = 0; i < length; ++i ) {

        printf("%d\n", array[i]);
        // if( i != (length-1) )
        //     printf("%d, ", array[i]);
        // else
        //     printf("%d\n", array[i]);

    }

    printf("-999\n");

}

int main() {

    srand (time(NULL));

    // printf("My name is Giancarlo Gonzalez.  My UIN is 665799950.\n");

    int unsortedArray[20000], sortedArray[20000], array_length = 20000;

    for(int i = 0; i < array_length; ++i)
        unsortedArray[i] = rand() % array_length;

    printArray(unsortedArray, array_length);
    printf("0\n");
    printf("4998\n");
    printf("9999\n");
    printf("10000\n");
    printf("19999\n");
    printf("20000\n");
    printf("-999\n");

    return 0;

}
