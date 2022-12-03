#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>

#define INPUT "input.txt"

enum opp
{
	A, // rock
	B, // paper
	C	 // cisors
};

enum curr
{
	X, // rock
	Y, // paper
	Z	 // cisors
};

int main(int argc, char **argv)
{
	(void)argc, (void)argv;

	FILE *f = fopen("../" INPUT, "r");
	if (f == NULL)
	{
		fprintf(stderr, "couln't open file %s : %s", INPUT, strerror(errno));
		return EXIT_FAILURE;
	}

	uint64_t score = 0;

	uint8_t opp = 0;
	uint8_t curr = 0;

	char opp_ch = 0;
	char curr_ch = 0;
	char tmp = 0;

	do
	{
		if (fscanf(f, "%c%*1[ ]%c", &opp_ch, &curr_ch) == EOF)
			break;

		// adding score for our choice
		switch (curr_ch)
		{
		case 'X':
			curr = X;
			break;
		case 'Y':
			curr = Y;
			break;
		case 'Z':
			curr = Z;
			break;
		default:
			fprintf(stderr, "error, invalid value : %u", curr);
			return EXIT_FAILURE;
			break;
		}

		switch (opp_ch)
		{
		case 'A':
			opp = A;
			break;
		case 'B':
			opp = B;
			break;
		case 'C':
			opp = C;
			break;
		default:
			fprintf(stderr, "error, invalid value : %u", curr);
			return EXIT_FAILURE;
			break;
		}

		score += curr + 1;

		if ((opp + 1) % 3 == curr)
			score += 6;
		else if (opp == curr)
			score += 3;

	} while (fscanf(f, "%1[\n]", &tmp) != EOF);

	printf("the total score is : %" PRIu64 "\n", score);

	fclose(f);

	return EXIT_SUCCESS;
}