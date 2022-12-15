#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <curses.h>

#define GRAPH_COLS 162
#define GRAPH_ROWS 41

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

const char wall[GRAPH_ROWS][GRAPH_COLS] = {
		"abdefghi",
		"acctuvwj",
		""};

void init_grid(graph *grid);
void print_grid(void);
node_pos get_min(void);

#define GET_GRID(pos) (grid[pos.x][pos.y])

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	init_grid(&grid);

	const node_pos start = {GRAPH_COLS - 1, 0},
								 end = {GRAPH_COLS - 3, GRAPH_ROWS - 3};

	GET_GRID(start).d = 0;

	node_pos u;
	while (1)
	{
		/*
		u : the current node, all of it's neighbours will be checked, it wil not be visited again
		*/
		u = get_min();
		if (u.x == end.x && u.y == end.y)
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
				if (v.x < 0 || v.x > GRAPH_COLS || v.y < 0 || v.y > GRAPH_ROWS)
					continue;

				// if the node was already visited or is a wall: next node
				if (IS_VISITED(GET_GRID(v)) || (wall[v.x][v.y] > wall[u.y][u.x] + 1))
					continue;

				if (GET_GRID(u).d + 1 < GET_GRID(v).d)
				{
					GET_GRID(v).d = GET_GRID(u).d + 1; // adding one because all of the nodes are at the same distance : 1
					GET_GRID(v).prev = u;
				}
			}
	}

	// print_grid();

	/* --- reconstructing the path --- */

	u = end;

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

	initscr();
	start_color(); /* Start color 			*/
	init_pair(1, COLOR_GREEN, COLOR_BLACK);

	printf("\n\nthe shortest ditance from start to end is %" PRIu64 "\n", GET_GRID(end).d);

	/* --- printing the grid --- */
	for (int32_t y = (GRAPH_ROWS - 1); y >= 0; --y)
	{
		for (int32_t x = 0; x < GRAPH_COLS; ++x)
		{
			if (IS_PATH(grid[x][y]))
				attron(COLOR_PAIR(1));

			if (start.x == x && start.y == y)
				addch('S');
			else if (end.x == x && end.y == y)
				addch('E');
			else if (wall[y][x])
				addch(wall[y][x]);
			else
				addch('.');

			attroff(COLOR_PAIR(1));
		}
		addch('\n');
	}

	scanw("%*c");

	return EXIT_SUCCESS;
}

void init_grid(graph *grid)
{
	const vertex basic = {INF, 0, {NODE_POS_UNDEF, NODE_POS_UNDEF}};

	for (uint32_t x = 0; x < GRAPH_ROWS; ++x)
		for (uint32_t y = 0; y < GRAPH_COLS; ++y)
			(*grid)[y][x] = basic;
}

void print_grid(void)
{
	for (uint32_t x = 0; x < GRAPH_COLS; ++x)
	{
		for (uint32_t y = 0; y < GRAPH_ROWS; ++y)
			if (grid[x][y].d < INF)
				printf("%" PRIu64 " ", grid[x][y].d);
			else
				printf(". ");
		putchar('\n');
	}
	return;
}

node_pos get_min(void)
{
	node_pos node;
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