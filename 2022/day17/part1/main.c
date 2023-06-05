#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <curses.h>
#include <windows.h>

// #define TEST

#ifndef TEST
#define INPUT "input.txt"
#define MAX_JET_PAT_LEN (10091)
#else
#define INPUT "test_input.txt"
#endif

#define JET_RIGHT 1
#define JET_LEFT 0

enum ROCK_TYPES
{
	ROCK_MINUS,
	ROCK_PLUS,
	ROCK_L,
	ROCK_I,
	ROCK_SQ,
	NB_ROCKS
};

const char *rocks[NB_ROCKS] = {"####",
															 ".#.\n###\n.#.",
															 "..#\n..#\n###",
															 "#\n#\n#\n#",
															 "##\n##"};

const uint8_t rocks_height[NB_ROCKS] = {1, 3, 3, 4, 2};

#define JET_PAT_BIT_LEN ((MAX_JET_PAT_LEN + ((8 - (MAX_JET_PAT_LEN % 8)) % 8)) / 8)
uint8_t jet_pat[(JET_PAT_BIT_LEN)];
#define SET_JET_PATH_RIGHT(i) jet_pat[(i) / 8] |= 1 << ((i) % 8)
#define SET_JET_PATH_LEFT(i) jet_pat[(i) / 8] &= ~(1 << ((i) % 8))
#define GET_JET_PATH(i) ((jet_pat[(i) / 8] >> ((i) % 8)) & 1)

#define GRID_AIR 0
#define GRID_ROCK 1

#define MAX_GRID_HEIGHT 4096U // should be a power of two
#define MAX_GRID_BIT_HEIGHT ((MAX_GRID_HEIGHT) / 8U)
#define GRID_WIDTH 7
uint8_t grid[GRID_WIDTH][MAX_GRID_BIT_HEIGHT];
#define BIT_SET_GRID(x, y) (grid[(x)][(y) / 8] |= (1 << ((y) % 8)))
#define BIT_CLR_GRID(x, y) (grid[(x)][(y) / 8] &= ~(1 << ((y) % 8)))
#define SET_GRID(x, y, type) \
	if (type == GRID_AIR)      \
		BIT_CLR_GRID(x, y);      \
	else                       \
		BIT_SET_GRID(x, y)
#define GET_GRID(x, y) ((grid[(x)][(y) / 8] >> ((y) % 8)) & 1)

uint16_t rock_tower = 0;
uint16_t cave_height = 0;
uint32_t curr_jet = 0;

typedef struct
{
	uint32_t y;
	uint8_t x, type, push, move;
} piece;

void add_piece(const piece piece);
void print_cave(void);
void update_piece(piece *const piece);

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

	/* parse file */

	char tmp;
	uint32_t i = 0;
	do
	{
		if (fgetc(f) == '>')
			SET_JET_PATH_RIGHT(i);
		else
			SET_JET_PATH_LEFT(i);
		++i; // cannot put the ++i into the macro call because it is used multiple times in the macro
	} while (i - 1 != MAX_JET_PAT_LEN);

	/* first piece */

	piece curr = {.x = 2, .y = 3, .type = 0, .push = 1, .move = 0}; // the first piece

	initscr();

	add_piece(curr);

	print_cave();

	getch();

	update_piece(&curr);

	print_cave();

	getch();

	fclose(f);

	return EXIT_SUCCESS;
}

void add_piece(const piece piece)
{
	uint8_t i = 0,
					x = piece.x,
					y = piece.y + rocks_height[piece.type];

	cave_height = y - 1;
	const char *const rock = rocks[piece.type];
	do
	{
		if (rock[i] == '\n')
		{
			--y;
			x = piece.x;
			continue;
		}

		if (rock[i] == '#') // there is a rock there
		{
			SET_GRID(x, y, GRID_ROCK);
			++x;
		}
	} while (rock[i++] != 0);

	return;
}

void rem_piece(const piece piece)
{
	uint8_t i = 0,
					x = piece.x,
					y = piece.y + rocks_height[piece.type];

	const char *const rock = rocks[piece.type];
	do
	{
		if (rock[i] == '\n')
		{
			--y;
			x = piece.x;
			continue;
		}

		if (rock[i] == '#') // there is a rock there
		{
			BIT_CLR_GRID(x, y);
			++x;
		}
	} while (rock[i++] != 0);

	return;
}

/* return the new value of piece.push */
void update_piece(piece *const piece)
{
	if (piece->push)
	{
		if (GET_JET_PATH(curr_jet) == JET_RIGHT)
		{
			rem_piece(*piece);
			++piece->x;
			add_piece(*piece);
		}
		else // moving left
		{
			rem_piece(*piece);
			--piece->x;
			add_piece(*piece);
		}

		++curr_jet;
	}
	else
	{
		rem_piece(*piece);
		if (piece->y <= rock_tower)
			add_piece(*piece);
	}

	return;
}

void print_cave(void)
{
	for (uint8_t y = cave_height + 1; y > 0; --y)
	{
		addch('|');
		for (uint8_t x = 0; x < GRID_WIDTH; ++x)
			addch(GET_GRID(x, y) ? '#' : '.');
		addch('|');
		addch('\n');
	}
	refresh();
}