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

#define NOOP 'n'
#define ADDX 'a'

#define X_START_VALUE 1

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

	char tmp;
	char op;
	char next_op = 0;
	int val = 0;
	int64_t x = X_START_VALUE;

	uint64_t cycles = 0;
	int64_t sum = 0;

	do
	{
		if (next_op != ADDX)
		{
			val = 0;
			fscanf(f, "%1c%*3c", &op);
			switch (op)
			{
			case NOOP:
				++cycles;
				break;
			case ADDX:
				next_op = ADDX;
				++cycles;
				break;
			default:
				printf("error, the operation '%c' is not supported, the two operations dupported are %c and %c\n", op, ADDX, NOOP);
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			++cycles;
			next_op = 0;
			fscanf(f, "%i", &val);
		}

		if (cycles > 220)
			break;
		if ((cycles + 20) % 40 == 0)
			sum += x * cycles;

		/*
		very end of loop : change value of x
		val will be 0 if noop is happening or if it is the first cycle of addx
		*/
		x += val;
	} while (fscanf(f, "%c", &tmp) != EOF);

	printf("the sum of those signal strenths is : %" PRId64 "\n", sum);

	fclose(f);

	return EXIT_SUCCESS;
}