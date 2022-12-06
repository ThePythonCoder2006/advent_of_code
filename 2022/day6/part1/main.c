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

#define MARKER_SIZE 4

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

	char chars[MARKER_SIZE];
	uint8_t offset = 0;

	uint8_t counts[26] = {0};

	// are the current four chars considered a valid marker
	uint8_t valid = 1;

	// get the initial four chars
	for (uint8_t i = 0; i < MARKER_SIZE; ++i)
	{
		//  get char and update the list of char used
		fscanf(f, "%c", chars + offset);
		if (counts[(uint8_t)chars[offset++] - 'a']++ > 0)
			valid = 0;
		/*
		offset >= 4 : 0 or 1
		0 << 2 = 0
		1 << 2 = 4
		*/
		offset -= (offset >= 4) << 2; // working mod 4
	}

	while (!valid)
	{
		// remove the precedent char's presence from the counts and if ther is only one of it's kind left, set valid to 1
		if (--counts[(uint8_t)chars[offset] - 'a'] == 1)
			valid = 1;

		// get next char
		if (fscanf(f, "%c", chars + offset) == EOF)
		{
			fprintf(stderr, "reached end of file and did not find a marker ...");
			exit(EXIT_FAILURE);
		}
		if (counts[(uint8_t)chars[offset++] - 'a']++ > 0) // if already present, set valid = 0
			valid = 0;
		offset -= (offset >= 4) << 2; // working mod 4
	}

	printf("the marker is %c%c%c%c\n", chars[0], chars[1], chars[2], chars[3]);

	fclose(f);

	return EXIT_SUCCESS;
}