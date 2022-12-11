#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>

// #define TEST

#ifndef TEST
#define INPUT "input.txt"
#else
#define INPUT "test_input.txt"
#endif

#define PLUS '+'
#define TIMES '*'

#define RELIEF 3

#define MKY_OBJ_LST_SIZE 100

typedef struct monkey
{
	uint32_t obj[MKY_OBJ_LST_SIZE];
	char op;
	uint8_t op_val;
	uint8_t test;
	uint8_t test_true;
	uint8_t test_false;
} mky_t;

mky_t *init_mky(FILE *f);

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

	mky_t *mky = init_mky(f);

	(void)mky;

	fclose(f);

	return EXIT_SUCCESS;
}

mky_t *init_mky(FILE *f)
{
	mky_t *mky = malloc(sizeof(*mky));

	fscanf(f, "%*[^\n]%*1[\n]" // remove first line ("Monkey [number]:\n")
						"%*18c"					 // remove "  starting items: "
	);

	// get the objects
	{
		char tmp;
		uint8_t i = 0;
		do
		{
			fscanf(f, "%u" // get the worryness level of item i
						 ,
						 &(mky->obj[i]));
			++i;
		} while (fscanf(f, "%1[\n]", &tmp) == 0);
	}

	// get operation
	fscanf(f, "%*23c"		// remove "  Operation: new = old "
						"%1c"			// get the operator ('+' or '*')
						"%*1[ ]"	// remove ' '
						"%" SCNu8 // get the operand
						"%*1[\n]",
				 &(mky->op), &(mky->op_val));

	// get test
	fscanf(f, "%*21c"		// remove ""  Test: divisible by "
						"%" SCNu8 // get divisibility test
						"%*1[\n]",
				 &(mky->test));

	// get if true
	fscanf(f, "%*29c"		// remove ""    If true: throw to monkey"
						"%" SCNu8 // get the monkey to which it will get thrown
						"%*1[\n]",
				 &(mky->test_true));

	// get if false
	fscanf(f, "%*30c"		// remove ""    If false: throw to monkey"
						"%" SCNu8 // get the monkey to which it will get thrown
						"%*1[\n]",
				 &(mky->test_false));

	return mky;
}