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

#define MARKER_SIZE 14

#define GET_COUNTS(inp) counts[(uint8_t)(inp) - 'a']

uint8_t check_marker(const char *buff);

uint8_t counts[26] = {0};

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

	// are the current four chars considered a valid marker
	uint8_t valid = 1;

	// get the initial four chars
	for (uint8_t i = 0; i < MARKER_SIZE; ++i)
	{
		//  get char and update the list of char used
		fscanf(f, "%c", chars + i);

		++GET_COUNTS(chars[i]);
	}

	uint64_t nb_chars = MARKER_SIZE;

	valid = check_marker(chars);

	while (!valid)
	{
		--GET_COUNTS(chars[0]);
		memmove(chars, chars + 1, MARKER_SIZE - 1); // shift the chars buffer one to the left to make space for the new char

		fscanf(f, "%c", chars + MARKER_SIZE - 1);

		++GET_COUNTS(chars[MARKER_SIZE - 1]);

		valid = check_marker(chars);

		++nb_chars;
	}

	// printf("the marker is %c%c%c%c\n", chars[0], chars[1], chars[2], chars[3]);
	printf("the marker was found after %" PRIu64 " characters\n", nb_chars);

	fclose(f);

	return EXIT_SUCCESS;
}

// returns 1 if the marker is valid else, returns 0
uint8_t check_marker(const char *buff)
{
	for (uint8_t i = 0; i < MARKER_SIZE; ++i)
		if (GET_COUNTS(buff[i]) > 1)
			return 0;

	return 1;
}