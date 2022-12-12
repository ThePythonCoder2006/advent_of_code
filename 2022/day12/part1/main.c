#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

// #define TEST

#ifndef TEST
#define INPUT "input.txt"
#define COLS 162
#define ROWS 41
#else
#define INPUT "test_input.txt"
#endif

#define PATH_MAX_SIZE 1024

typedef enum PATH_DIRS
{
	PATH_UP,
	PATH_RT,
	PATH_DW,
	PATH_LT,
	DIR_COUNT
} path_dir_t;

typedef struct pos
{
	uint16_t x, y;
	char h;
} pos_t;

typedef struct node
{
	char h;
	uint64_t dist
} node;

node grid[ROWS][COLS];

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

	pos_t curr = {0, 0, 0};
	pos_t end = {0, 0, 0};

	for (uint16_t y = 0; y < ROWS; ++y)
	{
		for (uint16_t x = 0; x < COLS; ++x)
		{
			fscanf(f, "%1c", &(grid[y][x].h));
			if (grid[y][x].h == 'S')
			{
				curr.h = 'a';
				curr.x = x;
				curr.y = y;
			}
			if (grid[y][x].h == 'E')
			{
				end.h = 'z';
				end.x = x;
				end.y = y;
			}
		}
		fscanf(f, "%*c");
		putchar('\n');
	}

	fclose(f);

	return EXIT_SUCCESS;
}

path_dir_t *get_path(pos_t start, pos_t end)
{
	path_dir_t *path = calloc(PATH_MAX_SIZE, sizeof(path_dir_t));

	bool grid_visited[ROWS][COLS] = {false};

	if (start.x == end.x && start.y == end.y)
		return path;

	for (uint32_t i = 0; i < (ROWS * COLS); ++i)
		(**(grid + i)).dist = UINT64_MAX;

	grid[start.y][start.x].dist = 0;

	pos_t curr = start;

	if (grid[curr.y][curr.x].h <= (grid[curr.y][curr.x + 1].h + 1))
	{
	}

	return path;
}