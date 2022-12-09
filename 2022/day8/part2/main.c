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

	uint8_t north = 0, east = 0, south = 0, west = 0;
	uint64_t max = 0, nmax = 0;
	for (uint8_t x = 1; x < GRID_SIZE - 1; ++x)
	{
		for (uint8_t y = 1; y < GRID_SIZE - 1; ++y)
		{
			for (int8_t tx = x - 1; tx >= 0; --tx)
			{
				++west;
				if (grid[tx][y] >= grid[x][y])
					break;
			}

			for (uint8_t tx = x + 1; tx < GRID_SIZE; ++tx)
			{
				++east;
				if (grid[tx][y] >= grid[x][y])
					break;
			}
			for (int8_t ty = y - 1; ty >= 0; --ty)
			{
				++north;
				if (grid[x][ty] >= grid[x][y])
					break;
			}
			for (uint8_t ty = y + 1; ty < GRID_SIZE; ++ty)
			{
				++south;
				if (grid[x][ty] >= grid[x][y])
					break;
			}
			if ((nmax = (uint64_t)north * (uint64_t)east * (uint64_t)south * (uint64_t)west) > max)
			{
				max = nmax;
			}
			north = 0;
			east = 0;
			south = 0;
			west = 0;
		}
	}

	printf("the maximum scenic score is : %" PRIu64 "\n", max);

	fclose(f);

	return EXIT_SUCCESS;
}