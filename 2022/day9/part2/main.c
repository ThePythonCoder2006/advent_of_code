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

#define CELL_VISITED ((uint8_t)0x01U)

#define ROWS (500)
#define COLS (500)

#define TAILS_NB (9)
#define TAIL_END (TAILS_NB - 1)

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

typedef struct knot
{
	uint16_t x, y;
} knot_t;

const int8_t mvmt[DIR_COUNT][DIMENSIONS] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

uint8_t grid[ROWS][COLS] = {0};

uint64_t count = 0;

void update_head_pos(char c, knot_t *head);
void update_tail_pos(knot_t *head, knot_t *tail, uint8_t is_end);
void print_grid(knot_t *head, knot_t *tail);

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

	knot_t head = {COLS / 2, ROWS / 2};
	knot_t tails[TAILS_NB];

	for (uint8_t i = 0; i < TAILS_NB; ++i)
	{
		tails[i].x = COLS / 2;
		tails[i].y = ROWS / 2;
	}

	grid[tails[TAIL_END].y][tails[TAIL_END].x] = CELL_VISITED;
	++count;

	char dir;
	uint32_t nb;

	char tmp;
	while (fscanf(f, "%1[\n]", &tmp) != EOF)
	{

		fscanf(f, "%1c%*1[ ]%u", &dir, &nb);

		for (uint8_t _ = 0; _ < nb; ++_)
		{
			update_head_pos(dir, &head);
			update_tail_pos(&head, &(tails[0]), 0);
			for (uint8_t i = 1; i < TAILS_NB - 1; ++i)
				update_tail_pos(&(tails[i - 1]), &(tails[i]), 0);
			update_tail_pos(&(tails[TAIL_END - 1]), &(tails[TAIL_END]), 1);
		}
	}
	printf("the tail visited %" PRIu64 " cells\n", count);

	fclose(f);

	return EXIT_SUCCESS;
}

void update_head_pos(char c, knot_t *head)
{
	switch (c)
	{
	case 'U':
		head->x += mvmt[U][X];
		head->y += mvmt[U][Y];
		break;
	case 'R':
		head->x += mvmt[R][X];
		head->y += mvmt[R][Y];
		break;
	case 'D':
		head->x += mvmt[D][X];
		head->y += mvmt[D][Y];
		break;
	case 'L':
		head->x += mvmt[L][X];
		head->y += mvmt[L][Y];
		break;

	default:
		break;
	}

	return;
}

void update_tail_pos(knot_t *head, knot_t *tail, uint8_t is_end)
{
	int16_t dx = head->x - tail->x;
	int16_t dy = head->y - tail->y;

	// if the tail is not to far away, do not change anything
	if (dx <= 1 && dx >= -1 && dy <= 1 && dy >= -1)
		return;

	if (dx > 2 || dx < -2 || dy > 2 || dy < -2)
	{
		fprintf(stderr, "error, the head (%" PRIu16 ", %" PRIu16 ") and tail (%" PRIu16 ", %" PRIu16 ") provided were to fas away", head->x, head->y, tail->x, tail->y);
		return;
	}

	if (dy != 0 && !(dy != 0 && dx != 0))
		goto next;
	if (dx > 0)
		++(tail->x);
	else
		--(tail->x);
next:
	if (dx != 0 && !(dy != 0 && dx != 0))
		goto end;
	if (dy > 0)
		++(tail->y);
	else
		--(tail->y);

end:
	// if the cell was not yet visited, make it visited and update counter
	if (grid[tail->y][tail->x] != CELL_VISITED && is_end)
	{
		grid[tail->y][tail->x] = CELL_VISITED;
		++count;
	}

	return;
}

void print_grid(knot_t *head, knot_t *tail)
{
	for (uint16_t x = 0; x < COLS; ++x)
	{
		for (uint16_t y = 0; y < ROWS; ++y)
			if (x == head->x && y == head->y)
				putchar('H');
			else if (x == tail->x && y == tail->y)
				putchar('T');
			else
				printf("%c", grid[y][x] == CELL_VISITED ? '#' : '.');
		putchar('\n');
	}

	return;
}