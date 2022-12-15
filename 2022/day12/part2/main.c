#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <curses.h>
#include <time.h>
#include <windows.h>

// #define TEST
// #define DEBUG

#ifndef TEST
#define INPUT "input.txt"
#define GRAPH_COLS (162)
#define GRAPH_ROWS (41)
#else
#define INPUT "test_input.txt"
#define GRAPH_COLS (8)
#define GRAPH_ROWS (5)
#endif

#define PATH_MAX_SIZE 1024

#define INF UINT64_MAX

#define VISITED 0x1
#define PATH 0x2
#define IS_VISITED(v) ((v.bools | VISITED) == v.bools)
#define IS_PATH(v) ((v.bools | PATH) == v.bools)

#define NODE_POS_UNDEF (-1)
typedef struct
{
	int32_t x, y;
} node_pos;

typedef struct node
{
	uint64_t d;
	uint8_t bools;
	node_pos prev;
} vertex;

typedef vertex graph[GRAPH_COLS][GRAPH_ROWS];

graph grid;

char wall[GRAPH_COLS][GRAPH_ROWS];

void init_grid(graph *grid);
node_pos get_min(void);

#define GET_GRID(pos) (grid[pos.x][pos.y])

#define PAIR_GREEN 1
#define PAIR_RED 2
#define PAIR_BLUE 3
#define PAIR_YELLOW 4

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	init_grid(&grid);

	FILE *f = fopen("../" INPUT, "r");
	if (f == NULL)
	{
		fprintf(stderr, "couln't open file %s : %s", INPUT, strerror(errno));
		return EXIT_FAILURE;
	}

	node_pos start;
#ifndef DEBUG
	initscr();
	start_color(); /* Start color 			*/
	init_pair(PAIR_GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(PAIR_RED, COLOR_RED, COLOR_BLACK);
	init_pair(PAIR_BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(PAIR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
#endif

	/* reading the height data from file */
	char tmp;
	for (int32_t y = (GRAPH_ROWS - 1); y >= 0; --y)
	{
		for (int32_t x = 0; x < GRAPH_COLS; ++x)
		{
			tmp = fgetc(f);

#ifndef DEBUG
			addch(tmp);
#endif

			if (tmp == 'S')
			{
				wall[x][y] = 'a';
				continue;
			}
			if (tmp == 'E')
			{
				start.x = x, start.y = y;
				wall[x][y] = 'z';
				continue;
			}

			wall[x][y] = tmp;
		}
		tmp = fgetc(f);
#ifndef DEBUG
		addch('\n');
#endif
	}

	fclose(f);

	/* dijkstra's algorithm */

	GET_GRID(start).d = 0;

	uint64_t min = INF;
	node_pos min_pos = {NODE_POS_UNDEF, NODE_POS_UNDEF};

	node_pos u = {0, 0};
	while (1)
	{
		u = get_min();
		if (u.x == NODE_POS_UNDEF && u.y == NODE_POS_UNDEF)
			break;

		GET_GRID(u).bools |= VISITED;

		// for all neighbors
		for (int8_t i = -1; i <= 1; ++i)
			for (int8_t j = -1; j <= 1; ++j)
			{
				// do not allow diagonal movement
				if (abs(i) == abs(j))
					continue;

				// v : the node that is being examined
				const node_pos v = {u.x + i, u.y + j};

				// if the position is outside of the grid : next node
				if (v.x < 0 || v.x >= GRAPH_COLS || v.y < 0 || v.y >= GRAPH_ROWS)
					continue;

				// if the node was already visited or is a wall: next node
				if (IS_VISITED(GET_GRID(v)) || (wall[v.x][v.y] + 1 < wall[u.x][u.y]))
					continue;

				if (GET_GRID(u).d + 1 < GET_GRID(v).d)
				{
					GET_GRID(v).d = GET_GRID(u).d + 1; // adding one because all of the nodes are at the same distance : 1
					GET_GRID(v).prev = u;
				}

				/* finding starting point */

				// the starting point has to be at level 'a'
				if (wall[u.x][u.y] != 'a')
					continue;

				if (GET_GRID(u).d < min)
				{
					min_pos.x = u.x, min_pos.y = u.y;
					min = GET_GRID(u).d;
				}

				// Sleep(1);
			}
	}

	/* --- reconstructing the path --- */
	u = min_pos;

	// if there is no previous node before end => end was not reach => it is unreachable => exit
	if (GET_GRID(u).prev.x == NODE_POS_UNDEF || GET_GRID(u).prev.y == NODE_POS_UNDEF)
	{
		fprintf(stderr, "there is no path beaween start and end...");
		return 1;
	}

	while (u.x != NODE_POS_UNDEF && u.y != NODE_POS_UNDEF)
	{
		GET_GRID(u).bools |= PATH;
		u = GET_GRID(u).prev;
	}

#ifndef DEBUG
	mvprintw(GRAPH_ROWS, GRAPH_COLS, "\n\nthe shortest ditance from start to end is %" PRIu64 "\n\n", min);
#endif

	/* --- printing the grid --- */

#ifndef DEBUG
	for (int32_t y = (GRAPH_ROWS - 1); y >= 0; --y)
	{
		for (int32_t x = 0; x < GRAPH_COLS; ++x)
		{
			if (IS_PATH(grid[x][y]))
				mvchgat(GRAPH_ROWS - 1 - y, x, 1, 0, 1, NULL);
		}
	}

	move(GRAPH_ROWS + 10, 0);

	getch();
#endif

	return EXIT_SUCCESS;
}

void init_grid(graph *grid)
{
	const vertex basic = {INF, 0, {NODE_POS_UNDEF, NODE_POS_UNDEF}};

	for (uint32_t x = 0; x < GRAPH_ROWS; ++x)
		for (uint32_t y = 0; y < GRAPH_COLS; ++y)
			(*grid)[y][x] = basic;
}

node_pos get_min(void)
{
	node_pos node = {NODE_POS_UNDEF, NODE_POS_UNDEF};
	uint64_t min = INF;

	for (uint32_t x = 0; x < GRAPH_COLS; ++x)
		for (uint32_t y = 0; y < GRAPH_ROWS; ++y)
		{
			if (grid[x][y].d < min && !IS_VISITED(grid[x][y]))
			{
				min = grid[x][y].d;
				node.x = x, node.y = y;
				continue;
			}
		}

	return node;
}
