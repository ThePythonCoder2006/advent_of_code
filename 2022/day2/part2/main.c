#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>

#define INPUT "input.txt"

// only works for 0 <= val <= 5
#define MOD_3(val) ((val) - ((val) >= 3) * 3)

typedef enum opp
{
	A, // rock
	B, // paper
	C	 // cisors
} opp_t;

typedef enum curr
{
	X, // loose
	Y, // draw
	Z	 // win
} curr_t;

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

	uint64_t score = 0;

	uint8_t opp = 0;
	uint8_t obj = 0;

	char opp_ch = 0;
	char obj_ch = 0;
	char tmp = 0;

	do
	{
		if (fscanf(f, "%c%*1[ ]%c", &opp_ch, &obj_ch) == EOF)
			break;

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
			fprintf(stderr, "error, invalid value : %u", obj);
			return EXIT_FAILURE;
			break;
		}

		switch (obj_ch)
		{
		case 'X':
			obj = X;
			score += MOD_3(3 + (opp - 1)) + 1; // opp - 1 : what you need to do to loose
			score += 0;
			break;
		case 'Y':
			obj = Y;
			score += opp + 1;
			score += 3;
			break;
		case 'Z':
			obj = Z;
			score += MOD_3(opp + 1) + 1;
			score += 6;
			break;
		default:
			fprintf(stderr, "error, invalid value : %u", obj);
			return EXIT_FAILURE;
			break;
		}

	} while (fscanf(f, "%1[\n]", &tmp) != EOF);

	printf("the total score is : %" PRIu64 "\n", score);

	fclose(f);

	return EXIT_SUCCESS;
}