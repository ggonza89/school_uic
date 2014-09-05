#include <stdio.h>
#include <stdlib.h>
// #include <time.h>

void swap(int *a, int *b)
{

  int t=*a; *a=*b; *b=t;

}

void printArray(int array[], int length) {

    for( int i = 0; i < length; ++i ) {

        printf("%d ", array[i]);
        // if( i != (length-1) )
        //     printf("%d, ", array[i]);
        // else
        //     printf("%d\n", array[i]);

    }

    printf("-999\n");

}

void copyArray(int fromArray[], int toArray[], int length) {

    for(int i = 0; i < length; ++i)
        toArray[i] = fromArray[i];

}

void sort(int array[], int beg, int end) {

    if( end > beg+1 ) {

        int pivot = array[beg], left = beg+1, right = end;

        while( left < right ) {

            if( array[left] <= pivot )
                ++left;
            else
                swap(&array[left], &array[--right]);

        }

        swap(&array[--left], &array[beg]);
        sort(array, beg, left);
        sort(array, right, end);

    }

}

int linearSearch(int unsortedArray[], int value, int length, int *numComparisons) {

    for(int i = 0; i < length; ++i) {

        if(unsortedArray[i] == value) {

            ++i;
            *numComparisons = i;
            return value;

        }

    }

    // printf("numComparisons = %d\n", length);
    *numComparisons = length;
    return -1;

}

int binarySearch(int sortedArray[], int value, int start, int length, int *numComparisons) {

    if(start > length)
        return -1;

    int midpoint = start + ((length - start) / 2);
    *numComparisons += 1;
    // printf("Midpoint %d start %d length %d\n", midpoint, start, length);

    if(sortedArray[midpoint] > value)
        return binarySearch(sortedArray, value, start, midpoint - 1, numComparisons);
    else if(sortedArray[midpoint] < value)
        return binarySearch(sortedArray, value, midpoint + 1, length, numComparisons);
    else
        return value;

}

int main() {

    // srand (time(NULL));

    printf("My name is Giancarlo Gonzalez.  My UIN is 665799950.\n");

    int *unsortedArray;
    int array_length = 100, inputNum = 0, array_input = 0, quit = -999;
    unsortedArray = (int *)malloc(array_length * sizeof(int));

    printf("Please enter array inputs followed by return. Enter -999 to end the array input.\n");
    scanf("%d", &inputNum);

    while(inputNum != quit) {

        if(array_input == array_length) {

            int *temp;
            temp = (int *) malloc ( array_length * 2 * sizeof(int) );
            for (int i = 0 ; i < array_length ; ++i)
                temp[i] = unsortedArray[i];
            free (unsortedArray);
            unsortedArray = temp;
            array_length = array_length * 2;

        }

        unsortedArray[array_input] = inputNum;
        ++array_input;
        scanf("%d", &inputNum);

    }

    // printf("Size of array %d array filled to %d\n", array_length, array_input);

    array_length = array_input;

    int sortedArray[array_length];

    // printArray(unsortedArray, array_length);
    copyArray(unsortedArray, sortedArray, array_length);
    sort(sortedArray, 0, array_length);
    // printArray(sortedArray, array_length);
    int numComparisons = 0, value = 0;

    printf("Enter a value to search in the array. Enter -999 to quit.\n");
    scanf("%d", &value);

    while(value != quit) {

        if(linearSearch(unsortedArray, value, array_length, &numComparisons) < 0)
            printf("Value %d was not found by Linear Search and it took %d comparisons.\n", value, numComparisons);
        else
            printf("Number of comparisons performed in Linear Search to find value %d was %d\n", value, numComparisons);

        numComparisons = 0;

        if(binarySearch(sortedArray, value, 0, array_length, &numComparisons) < 0)
            printf("Value %d was not found by Binary Search and it took %d comparisons.\n", value, numComparisons);
        else
            printf("Number of comparisons performed in Binary Search to find value %d was %d\n", value, numComparisons);

        scanf("%d", &value);

    }

    free(unsortedArray);

}
