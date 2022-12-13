#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define GRAPH_COLS 10
#define GRAPH_ROWS 10

#define INF UINT64_MAX

typedef struct node
{
	uint64_t d;
	uint8_t wall;
	bool visited;
} vertex;

typedef vertex graph[GRAPH_COLS][GRAPH_ROWS];

graph grid;

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	return EXIT_SUCCESS;
}

void init_grid(graph *grid)
{
	const vertex basic = {INF, 0, false};

	for (uint32_t x = 0; x < GRAPH_COLS; ++x)
		for (uint32_t y = 0; y < GRAPH_ROWS; ++y)
			(*grid)[y][x] = basic;
}