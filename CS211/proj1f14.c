#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void max(int x, int y) {

    if( x > y )
        printf("Max value is %d.\n", x);
    else
        printf("Max value is %d.\n", y);

}

void swap(int x, int y) {

    printf("Before swap %d, %d.\n", x, y);

    int temp = x;
    x = y;
    y = temp;

    printf("After swap %d, %d.\n", x, y);

}

void sum(int array[], int length) {

    int arraySum = 0;

    for( int i = 0; i < length; ++i )
        arraySum += array[i];

    printf("Sum of %d values in array is %d.\n", length, arraySum);

}

void printArray(int array[], int length) {

    for( int i = 0; i < length; ++i ) {

        if( i != (length-1) )
            printf("%d,", array[i]);
        else
            printf("%d\n", array[i]);

    }

}

void adjust(int array[], int length) {


    printArray(array, length);

    srand (time(NULL));

    int randTemp = rand();

    for( int i = 0; i < length; ++i ) {

        // printf("%d\n", randTemp);
        array[i] += randTemp;
        randTemp = rand();

    }

    printArray(array, length);

}

int main() {

    printf("My name is Giancarlo Gonzalez. My UIN is 665799950.\n");

    int x, y;
    printf("Please enter first integer value: ");
    scanf("%d", &x);
    printf("Please enter second integer value: ");
    scanf("%d", &y);

    printf("You have entered %d and %d respectively.\n", x, y);

    max(x, y);
    swap(x, y);

    int array[10];

    for( int i = 0; i < 10; ++i ) {

        printf("Please enter integer %d of array: ", i+1);
        scanf("%d", &array[i]);
        // printf("Array[%d] = %d.\n", i, array[i]);

    }

    sum(array, 10);
    adjust(array, 10);

    return 0;

}
