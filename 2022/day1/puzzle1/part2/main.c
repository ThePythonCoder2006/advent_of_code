#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>

#define NB_ELFS 3
#define INPUT "input.txt"

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	printf("START PART2\n\n");

	FILE *f = fopen("../" INPUT, "r");
	if (f == NULL)
	{
		fprintf(stderr, "An error occured when openning the file %s : %s", INPUT, strerror(errno));
		return 1;
	}

	uint32_t max[NB_ELFS] = {0}; // the laderboard for the elfs with the most calories
	uint32_t acc = 0;						 // the accumulator to compute the amout of calories of a particular elf
	uint32_t int_read = 0;			 // a temporary value to read the number of calories of a particular type

	uint8_t run = 1;

	char tmp;

	while (run)
	{
		// read the calories of one elf
		do
		{
			// read a uint32_t and remove the line ending '\n' and exit if reading into EOF
			if (fscanf(f, "%u%*1[\n]", &int_read) == EOF)
			{
				run = 0;
				break;
			}
			acc += int_read;
		} while (fscanf(f, "%1[\n]", &tmp) < 1); // if there is no other '\n' htat would indicate an other elf : continue looping

		// update leaderboard
		for (uint8_t i = 0; i < NB_ELFS; ++i)
		{
			// if we have a new score for any of the places
			if (acc > max[i])
			{
				// update the leaderboard starting from the end
				for (uint8_t j = NB_ELFS - 2; j > i; --j)
					max[j + 1] = max[j];

				// put the new record in place
				max[i] = acc;
				break;
			}
		}

		acc = 0;
	}

	printf("max = %u, %u, %u\n", max[0], max[1], max[2]);
	printf("the three elfs that carry the most snacks are carrying a total of %u calories\n", max[0] + max[1] + max[2]);

	fclose(f);

	return 0;
}