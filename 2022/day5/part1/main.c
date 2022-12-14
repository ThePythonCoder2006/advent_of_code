#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <curses.h>
#include <windows.h>

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

void print_crates(char stack[COLS][64], char *stack_ptr[COLS]);

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

	initscr();

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
		// fscanf(f, "%c", &tmp), printf("%i = '%c'\n", tmp, tmp);
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

		clear();

		print_crates(stack, stack_ptr);

		refresh();

		Sleep(10);

		/*
		moving the top of src to dst :
			- create space in dst
			- copy nb elements from src to dst
			- "colapse" src, remove the element that were moved to dst
		*/
		memmove(stack[(dst - 1)] + nb, stack[(dst - 1)], GET_STACK_SIZE((dst - 1)));
		stack_ptr[(dst)-1] += (nb);
		for (uint8_t i = 0; i < nb; ++i)
			stack[dst - 1][nb - i - 1] = stack[src - 1][i];
		memmove(stack[(src - 1)], stack[(src - 1)] + nb, GET_STACK_SIZE(((src - 1))));
		stack_ptr[(src)-1] -= (nb);
	} while (fscanf(f, "%1[\n]", &tmp) != EOF);

	print_crates(stack, stack_ptr);

	mvprintw(50, 0, "The final configuration of the top crates is : %c", stack[0][0]);
	for (uint8_t i = 1; i < COLS; ++i)
		addch(stack[i][0]);

	addch('\n');

	getch();

	fclose(f);

	return EXIT_SUCCESS;
}

void print_crates(char stack[COLS][64], char *stack_ptr[COLS])
{
	uint8_t max = 0;
	for (uint8_t i = 0; i < COLS; ++i)
		if (GET_STACK_SIZE(i) > max)
			max = GET_STACK_SIZE(i);

	for (uint8_t i = 0; i < COLS; ++i)
	{
		for (uint8_t j = 0; j < GET_STACK_SIZE(i); ++j)
		{
			mvprintw(max - GET_STACK_SIZE(i) + j, (4 * i), "[%c] ", stack[i][j]);
		}
		mvaddch(max, (4 * i) + 1, i + '1');
	}
	return;
}