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

#define CELL_VISITED (uint8_t)0x01U

#define ROWS 500
#define COLS 500

enum dir
{
	U,
	R,
	D,
	L,
	DIR_COUNT
};

enum pos
{
	X,
	Y,
	DIMENSIONS
};

const int8_t mvmt[DIR_COUNT][DIMENSIONS] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

uint8_t grid[ROWS][COLS] = {0};

uint64_t count = 0;

void update_head_pos(char c, uint16_t *hx, uint16_t *hy);
void update_tail_pos(uint16_t hx, uint16_t hy, uint16_t *tx, uint16_t *ty);
void print_grid(uint16_t hx, uint16_t hy, uint16_t tx, uint16_t ty);

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

	uint16_t head_x = COLS / 2, head_y = ROWS / 2;
	uint16_t tail_x = COLS / 2, tail_y = ROWS / 2;

	grid[tail_y][tail_x] = CELL_VISITED;
	++count;

	char dir;
	uint32_t nb;

	char tmp;
	while (fscanf(f, "%1[\n]", &tmp) != EOF)
	{

		fscanf(f, "%1c%*1[ ]%u", &dir, &nb);

		for (uint8_t _ = 0; _ < nb; ++_)
		{
			update_head_pos(dir, &head_x, &head_y);
			update_tail_pos(head_x, head_y, &tail_x, &tail_y);

			// print_grid(head_x, head_y, tail_x, tail_y);
			// scanf("%*c");
			// fflush(stdin);
		}
	}
	printf("the tail visited %" PRIu64 " cells\n", count);

	fclose(f);

	return EXIT_SUCCESS;
}

void update_head_pos(char c, uint16_t *hx, uint16_t *hy)
{
	switch (c)
	{
	case 'U':
		*hx += mvmt[U][X];
		*hy += mvmt[U][Y];
		break;
	case 'R':
		*hx += mvmt[R][X];
		*hy += mvmt[R][Y];
		break;
	case 'D':
		*hx += mvmt[D][X];
		*hy += mvmt[D][Y];
		break;
	case 'L':
		*hx += mvmt[L][X];
		*hy += mvmt[L][Y];
		break;

	default:
		break;
	}

	return;
}

void update_tail_pos(uint16_t hx, uint16_t hy, uint16_t *tx, uint16_t *ty)
{
	int16_t dx = hx - *tx;
	int16_t dy = hy - *ty;

	// if the tail is not to far away, do not change anything
	if (dx <= 1 && dx >= -1 && dy <= 1 && dy >= -1)
		return;

	if (dx > 2 || dx < -2 || dy > 2 || dy < -2)
	{
		fprintf(stderr, "error, the head (%" PRIu16 ", %" PRIu16 ") and tail (%" PRIu16 ", %" PRIu16 ") provided were to fas away", hx, hy, *tx, *ty);
		return;
	}

	if (dy != 0 && !(dy != 0 && dx != 0))
		goto next;
	if (dx > 0)
		++(*tx);
	else
		--(*tx);
next:
	if (dx != 0 && !(dy != 0 && dx != 0))
		goto end;
	if (dy > 0)
		++(*ty);
	else
		--(*ty);

end:
	// if the cell was not yet visited, make it visited and update counter
	if (grid[*ty][*tx] != CELL_VISITED)
	{
		grid[*ty][*tx] = CELL_VISITED;
		++count;
	}

	return;
}

void print_grid(uint16_t hx, uint16_t hy, uint16_t tx, uint16_t ty)
{
	for (uint16_t x = 0; x < COLS; ++x)
	{
		for (uint16_t y = 0; y < ROWS; ++y)
			if (x == hx && y == hy)
				putchar('H');
			else if (x == tx && y == ty)
				putchar('T');
			else
				printf("%c", grid[y][x] == CELL_VISITED ? '#' : '.');
		putchar('\n');
	}

	return;
}