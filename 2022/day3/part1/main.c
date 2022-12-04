#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>

#define INPUT "input.txt"
#define BUFF_SIZE 64
#define NB_LETTERS (26 + 26)

#define GET_PRIORITY(ch) ((ch < 'a' ? 27 : 1) + ((ch) - (ch < 'a' ? 'A' : 'a')))

uint8_t used[128U] = {0};

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

	char inp[BUFF_SIZE + 1],				 // 64 chars plus a \0, to end the string
			first[(BUFF_SIZE >> 1) + 1], // 64 / 2 = 32 chars plus a \0 to end the string
			end[(BUFF_SIZE >> 1) + 1];	 // 64 / 2 = 32 chars plus a \0 to end the string

	uint8_t len;
	uint64_t sum_priorities = 0;

	char tmp;

	do
	{
		if (fscanf(f, "%64[^\n]", inp) == EOF)
			break;

		len = strnlen(inp, BUFF_SIZE);

		strncpy(first, inp, len >> 1);
		strncpy(end, inp + (len >> 1), len >> 1);

		for (uint8_t i = 0; i < len >> 1; ++i)
			++used[(uint8_t)first[i]];

		for (uint8_t i = 0; i < len >> 1; ++i)
			if (used[(uint8_t)end[i]])
			{
				// printf("the common objet is '%c'\n", end[i]);
				sum_priorities += GET_PRIORITY(end[i]);
				break;
			}

		memset(used, 0, sizeof(used));
	} while (fscanf(f, "%1[\n]", &tmp) != EOF);

	// printf("%s + %s = %s%s (%s)\n%s   %s\n", first, end, first, end, inp, end, first);

	printf("the sum of the priorities is %" PRIu64 "\n", sum_priorities);

	fclose(f);

	return EXIT_SUCCESS;
}