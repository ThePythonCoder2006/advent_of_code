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
#define MKY_NB 8
#else
#define INPUT "test_input.txt"
#define MKY_NB 4
#endif

#define PLUS '+'
#define TIMES '*'

#define ROUNDS 20

#define RELIEF 3

#define MKY_OBJ_LST_SIZE 100

typedef struct monkey
{
	uint32_t obj[MKY_OBJ_LST_SIZE];
	uint16_t obj_count;
	char op;
	uint8_t op_val;
	uint8_t test;
	uint8_t test_true;
	uint8_t test_false;
	uint64_t count;
} mky_t;

mky_t *init_mky(FILE *f);
void update_mky(mky_t *mky);
void print_mkys(void);

mky_t *mkys[MKY_NB];

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

	for (uint8_t i = 0; i < MKY_NB; ++i)
		mkys[i] = init_mky(f);

	for (uint8_t _ = 1; _ <= ROUNDS; ++_)
	{
		for (uint8_t i = 0; i < MKY_NB; ++i)
		{
			update_mky(mkys[i]);
		}
		printf("after round %" PRIu8 " :\n", _);
		print_mkys();
	}

	uint64_t max1 = 0,
					 max2 = 0;

	for (uint8_t i = 0; i < MKY_NB; ++i)
	{
		if (mkys[i]->count > max1)
		{
			max2 = max1;
			max1 = mkys[i]->count;
		}
		else if (mkys[i]->count > max2)
			max2 = mkys[i]->count;
	}

	fclose(f);

	printf("the two most active monkeys have inspected respectively %" PRIu64 " and %" PRIu64 " item for a total monkey business of %" PRIu64 "\n", max1, max2, max1 * max2);

	return EXIT_SUCCESS;
}

mky_t *init_mky(FILE *f)
{
	mky_t *mky = calloc(1, sizeof(*mky));

	fscanf(f, "%*[^\n]%*1[\n]" // remove first line ("Monkey [number]:\n")
						"%*18c"					 // remove "  starting items: "
	);

	// get the objects
	char tmp;
	mky->obj_count = 0;

	fscanf(f, "%u%c", mky->obj + mky->obj_count++, &tmp);
	if (tmp == '\n')
		goto next;

	do
	{
		fscanf(f, "%*c" // remove " "
							"%u"	// get the worryness level of item i
					 ,
					 mky->obj + mky->obj_count++);
	} while (fscanf(f, "%1c", &tmp), tmp != '\n');

next:
	// get operation
	fscanf(f, "%*23c"		// remove "  Operation: new = old "
						"%1c"			// get the operator ('+' or '*')
						"%*1[ ]"	// remove ' '
						"%" SCNu8 // get the operand
				 ,
				 &(mky->op), &(mky->op_val));
	fscanf(f, "%*[^\n]%*1[\n]"); // finish the line

	// get test
	fscanf(f, "%*21c"		// remove "  Test: divisible by "
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

	fscanf(f, "%*1[\n]");

	mky->count = 0;

	return mky;
}

void update_mky(mky_t *mky)
{
	if (mky->obj_count == 0)
	{
		fprintf(stderr, "the monkey had no item...\n");
		return;
	}

	// const uint64_t pcount = mky->count;
	// printf("the monkey had made %" PRIu64 " inspections and has %" PRIu16 " objects on him", pcount, mky->obj_count);

	for (uint8_t i = 0; i < mky->obj_count; ++i)
	{
		++mky->count;

		if (mky->op_val == 0)
			mky->obj[i] *= mky->obj[i];
		else
		{
			if (mky->op == TIMES)
				mky->obj[i] *= mky->op_val;
			else if (mky->op == PLUS)
				mky->obj[i] += mky->op_val;
			else
			{
				fprintf(stderr, "error, operand was neither '+' nor '*', it was %c", mky->op);
				return;
			}
		}

		mky->obj[i] /= RELIEF;

		uint8_t dst_mky;
		if (mky->obj[i] % mky->test == 0)
			dst_mky = mky->test_true;
		else
			dst_mky = mky->test_false;

		mky_t *dst = mkys[dst_mky];
		dst->obj[dst->obj_count++] = mky->obj[i];

		mky->obj[i] = 0;
	}

	mky->obj_count = 0; // all the objects hav been thrown

	// printf(" and has now made %" PRIu64 " inspections for a total of %" PRIu64 " inspections\n", mky->count - pcount, mky->count);

	return;
}

void print_mkys(void)
{
	for (uint8_t i = 0; i < MKY_NB; ++i)
	{
		printf("monkey %" PRIu8 " : ", i);
		for (uint8_t j = 0; j < mkys[i]->obj_count; ++j)
		{
			printf("%u, ", mkys[i]->obj[j]);
		}
		putchar('\n');
	}

	return;
}