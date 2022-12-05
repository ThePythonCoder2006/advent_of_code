#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>

// #define TEST

#ifndef TEST
#define INPUT "input.txt"
#define COLS 9
#define MAX_INIT_HEIGHT 8
#else
#define INPUT "test_input.txt"
#define COLS 3
#define MAX_INIT_HEIGHT 3
#endif

#define GET_STACK_SIZE(n) (stack_ptr[(n)] - stack[(n)])

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	FILE *f = fopen("../" INPUT, "r");
	if (f == NULL)
	{
		fprintf(stderr, "couln't open file %s : %s", INPUT, strerror(errno));
		return EXIT_FAILURE;
	}

	/*
	MEMORY LAYOUT :
	*stack_ptr[0] = 'D'
	stack[0] : "XYZDJK...""
							|	^end of the stack ('D' not included) : next value to be written
							^top of the stack
	[X]
	[Y]
	[Z]
	 1

	stack[x][y] : the yth element of the xth stack
	*/
	char stack[COLS][64];
	char *stack_ptr[COLS];
	for (uint8_t i = 0; i < COLS; ++i)
		stack_ptr[i] = stack[i];

	// init : get the stacks
	for (uint8_t c = 0; c < MAX_INIT_HEIGHT; ++c)
	{
		fscanf(f, "%*c" // remove '['
							"%1c" // get the element of stack 1 = stack[0]
							"%*c" // remove ']'
					 ,
					 stack_ptr[0]);
		if (*(stack_ptr[0]) != ' ')
			++(stack_ptr[0]);
		for (uint8_t i = 1; i < COLS; ++i)
		{
			fscanf(f, "%*2c" // remove " ["
								"%1c"	 // get the element of stack i + 1 = stack[i]
								"%*c"	 // remove ']'
						 ,
						 stack_ptr[i]);
			if (*(stack_ptr[i]) != ' ')
				++(stack_ptr[i]);
		}

		fscanf(f, "%*1[\n]");
	}
	fscanf(f, "%*[^\n]%*1[\n]");

	uint32_t nb, src, dst;
	char tmp;
	do
	{
		if (fscanf(f, "%*5c" // remove "move "
									"%u"	 // get nb
									"%*5c" // remove " from"
									"%u"	 // get src
									"%*4c" // remove " to "
									"%u"	 // get dst
							 ,
							 &nb, &src, &dst) == EOF)
			break;

		/*
		moving the top of src to dst :
			- create space in dst
			- copy nb elements from src to dst
			- "colapse" src, remove the element that were moved to dst
		*/
		memmove(stack[dst - 1] + nb, stack[dst - 1], GET_STACK_SIZE(dst - 1)); // create space in dst
		// easy case, no need to worry about order of placmeent of crates
		memcpy(stack[dst - 1], stack[src - 1], nb);														 // copy elements from src to dst
		memmove(stack[src - 1], stack[src - 1] + nb, GET_STACK_SIZE(src - 1)); // "colapse" src, remove the element that were moved to dst

		// upadte the stack pointers, dst grew by nb and src was shorttened by nb
		stack_ptr[dst - 1] += (nb);
		stack_ptr[src - 1] -= (nb);
	} while (fscanf(f, "%1[\n]", &tmp) != EOF);

	printf("the final configuration of the top crates is : %c", stack[0][0]);
	for (uint8_t i = 1; i < COLS; ++i)
	{
		printf("%c", stack[i][0]);
	}

	fclose(f);

	return EXIT_SUCCESS;
}