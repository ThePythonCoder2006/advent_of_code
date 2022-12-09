#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>

// #define TEST

#ifndef TEST
#define INPUT "input.txt"
#define GRID_SIZE 99
#else
#define INPUT "test_input.txt"
#define GRID_SIZE 5
#endif

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

	uint16_t grid[GRID_SIZE][GRID_SIZE] = {0};

	for (uint8_t x = 0; x < GRID_SIZE; ++x)
		for (uint8_t y = 0; y < GRID_SIZE; ++y)
			fscanf(f, "%1hu", &(grid[x][y]));

	uint8_t north = 1, east = 1, south = 1, west = 1;
	uint64_t count = 0;
	for (uint8_t x = 1; x < GRID_SIZE - 1; ++x)
	{
		for (uint8_t y = 1; y < GRID_SIZE - 1; ++y)
		{
			for (uint8_t tx = 0; tx < x; ++tx)
				if (grid[tx][y] >= grid[x][y])
				{
					west = 0;
					break;
				}
			for (uint8_t tx = x + 1; tx < GRID_SIZE; ++tx)
				if (grid[tx][y] >= grid[x][y])
				{
					east = 0;
					break;
				}
			for (uint8_t ty = 0; ty < y; ++ty)
				if (grid[x][ty] >= grid[x][y])
				{
					north = 0;
					break;
				}
			for (uint8_t ty = y + 1; ty < GRID_SIZE; ++ty)
				if (grid[x][ty] >= grid[x][y])
				{
					south = 0;
					break;
				}
			if (north || east || south || west)
				++count;
			north = 1;
			east = 1;
			south = 1;
			west = 1;
		}
	}

	printf("the number of trees visible from the outside is %" PRIu64 "\n", 2 * GRID_SIZE + 2 * (GRID_SIZE - 2) + count);

	fclose(f);

	return EXIT_SUCCESS;
}