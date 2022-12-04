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
uint8_t used_cpy[128U] = {0};

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

	char inp[BUFF_SIZE + 1]; // 64 chars plus a \0, to end the string

	uint64_t sum_priorities = 0;

	char tmp;

	do
	{
		if (fscanf(f, "%64[^\n]", inp) == EOF)
			break;

		for (uint8_t i = 0; i < strlen(inp); ++i)
			++used[(uint8_t)inp[i]];

		if (fscanf(f, "%*1[\n]" // remove the backspace of the precedent line
									"%64[^\n]",
							 inp) == EOF)
			break;

		for (uint8_t i = 0; i < strlen(inp); ++i)
			used_cpy[(uint8_t)inp[i]] = used[(uint8_t)inp[i]];

		if (fscanf(f, "%*1[\n]" // remove the backspace of the precedent line
									"%64[^\n]",
							 inp) == EOF)
			break;

		for (uint8_t i = 0; i < strlen(inp); ++i)
			if (used_cpy[(uint8_t)inp[i]])
			{
				sum_priorities += GET_PRIORITY(inp[i]);
				break;
			}

		memset(used, 0, sizeof(used));
		memset(used_cpy, 0, sizeof(used_cpy));

		// scanf("%*c");
	} while (fscanf(f, "%1[\n]", &tmp) != EOF);

	printf("the sum of the priorities is %" PRIu64 "\n", sum_priorities);

	fclose(f);

	return EXIT_SUCCESS;
}