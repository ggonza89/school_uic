#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int DEBUG = 0;
typedef struct Stack{

    char * stack_array;
    int stack_size;
    int stack_at;

}Stack;

int is_empty(Stack * stack) {

    if(stack->stack_at == 0)
        return 1;
    else
        return 0;

}

void grow(Stack * stack) {

    // dynamic allocation of the array
    if(DEBUG)
        printf("Grow: old size: %d new size: %d copying %d values to "
            "new stack.\n", stack->stack_size, stack->stack_size+2, stack->stack_at);

    char * temp;
    temp = (char *) malloc ( stack->stack_size + 2 * sizeof(char) );
    for (int i = 0 ; i < stack->stack_at ; ++i)
        temp[i] = stack->stack_array[i];
    free (stack->stack_array);
    stack->stack_array = temp;
    stack->stack_size += 2;

}

char top(Stack * stack) {

    return stack->stack_array[stack->stack_at-1];

}

int push(Stack * stack, char elem) {

    if(DEBUG)
        printf("Pushing element %c to stack.\n",
            elem);

    if(stack->stack_at == stack->stack_size)
        grow(stack);

    stack->stack_array[stack->stack_at] = elem;
    stack->stack_at += 1;

    return stack->stack_at;

}

int pop(Stack * stack) {

    if(DEBUG)
        printf("Popping element %c from stack.\n",
            top(stack));
    if(stack->stack_at == 0)
        return -1;
    else
        return (stack->stack_at -= 1);

}

void initialize(Stack * stack) {

    stack->stack_array = (char *)malloc(2*sizeof(char));
    stack->stack_size = 2;
    stack->stack_at = 0;

}

int reset(Stack * stack) {

    if(DEBUG)
        printf("Resetting stack...\n");

    free(stack->stack_array);
    initialize(stack);

}

char expected_symbol(char top_of_stack) {

    if(top_of_stack == '(')
        top_of_stack = ')';
    else if(top_of_stack == '{')
        top_of_stack = '}';
    else if(top_of_stack == '[')
        top_of_stack = ']';
    else if(top_of_stack == '<')
        top_of_stack = '>';

    return top_of_stack;

}

int is_balanced(Stack * stack, char input_sym, char opening_sym, int input_pos) {

    if(is_empty(stack)) {

        for(int i = 0; i < input_pos; ++i)
            printf(" ");
        printf("^ missing %c\n", opening_sym);

        return 0;

    }
    else {

        if(top(stack) == opening_sym) {

            pop(stack);
            return 1;

        }
        else {

            for(int i = 0; i < input_pos; ++i)
                printf(" ");

            printf("^ expecting %c\n", expected_symbol(top(stack)));
            return 0;

        }

    }

}

int main(int argc, char ** argv) {

    // command line arguments taken care of
    if(argc > 1) {

        if(!strncmp(argv[1], "-d", strlen(argv[1])))
            DEBUG = 1;

    }

    char input[300];

    Stack stack;

    initialize(&stack);

    printf("Enter a single expression that will be checked for balanced symbols.\n");

    while( fgets(input, 300, stdin) != NULL ) {

        if(strlen(input) == 2 && (input[0] == 'q' || input[0] == 'Q'))
            return 0;
        else {
            int i;
            for(i = 0; i < strlen(input); ++i) {

                // check for opening symbols
                if(input[i] == '(')
                    push(&stack, input[i]);
                else if(input[i] == '{')
                    push(&stack, input[i]);
                else if(input[i] == '[')
                    push(&stack, input[i]);
                else if(input[i] == '<')
                    push(&stack, input[i]);

                // check for closing symbols
                else if(input[i] == ')') {

                    if(!is_balanced(&stack, input[i], '(', i))
                        break;

                }
                else if(input[i] == '}') {

                    if(!is_balanced(&stack, input[i], '{', i))
                        break;

                }
                else if(input[i] == ']') {

                    if(!is_balanced(&stack, input[i], '[', i))
                        break;

                }
                else if(input[i] == '>') {

                    if(!is_balanced(&stack, input[i], '<', i))
                        break;

                }


            }

            if(i < (strlen(input)-1)) {

                reset(&stack);
                continue;

            }

            if(is_empty(&stack))
                printf("Expression is balanced.\n");
            else {

                for(int i = 0; i < strlen(input); ++i)
                    printf(" ");

                printf("^ missing %c\n", expected_symbol(top(&stack)));

            }

        }

        reset(&stack);

    }

}
