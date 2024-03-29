#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define ITER 1000
#define BUFF_SIZE 64

#define __HELPER_IMPLEMENTATION__
#include "../../../helper.h"

#ifdef _WIN32
#include <windows.h>
#endif

// #define TEST

#ifndef TEST
#define INPUT "input.txt"
#else
#define INPUT "test_input.txt"
#endif

#define BUFF_LEN (64)

enum
{
	TYPE_INT,
	TYPE_LIST,
	TYPE_COUNT,
};

enum
{
	ORDER_RIGHT,
	ORDER_FALSE,
	ORDER_INIMPORTANT,
};

enum
{
	CMP_TYPE_INTS,
	CMP_TYPE_DIFF,
	CMP_TYPE_LISTS,
};

typedef struct obj_s obj;

typedef struct obj_s
{
	union obj_u
	{
		obj *list_content;
		int integer_value;
	} val;
	uint32_t list_len;
	uint8_t type;
} obj;

void obj_init(obj *o);
uint32_t read_obj(char *str, obj *dst);
void print_obj(const obj o);
uint8_t cmp_obj(obj first, obj second);

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	timer tot, sample;
	timer_start(&tot);

	FILE *f = fopen("../" INPUT, "r");
	if (f == NULL)
	{
		fprintf(stderr, "couln't open file %s : %s", INPUT, strerror(errno));
		return EXIT_FAILURE;
	}

	double times[ITER] = {0};

	uint32_t total;
	for (uint64_t sample_idx = 0; sample_idx < ITER; ++sample_idx)
	{
		timer_start(&sample);

		obj first, second;
		obj_init(&first);
		obj_init(&second);

		char *line = calloc(256, sizeof(char));

		char tmp;
		uint32_t i = 1;
		total = 0;
		do
		{
			fscanf(f, "%256[^\n]%*c", line);
			read_obj(line, &first);
			fscanf(f, "%256[^\n]%*c", line);
			read_obj(line, &second);

			uint8_t ans = cmp_obj(first, second);
			if (ans == 0)
				total += i;

			++i;
		} while ((tmp = fgetc(f)) == '\n');

		rewind(f);

		times[sample_idx] = timer_stop(&sample) * 1000;
	}

	printf("the sum of the indices already in the right order is : %u\n", total);

	fclose(f);

	const double tot_time = timer_stop(&tot) * 1000;
	const double mean = mean_time(times);
	const double std_dev = std_dev_time(times, mean);

	printf("total execution was %f milliseconds\n", tot_time);
	printf("on average a single run of the algorithm took %f milliseconds, with a standart deviation of %f milliseconds\n", mean, std_dev);

	return EXIT_SUCCESS;
}

void obj_init(obj *o)
{
	o->list_len = 0;
	o->type = TYPE_INT;
	o->val.integer_value = 0;
	o->val.list_content = NULL;

	return;
}

// returns the number of chars read
uint32_t read_obj(char *str, obj *dst)
{
	uint32_t lbracket = 1, rbracket = 0;
	uint32_t i = 0;
	char tmp = str[i++];
	if (tmp != '[')
	{
		dst->type = TYPE_INT;

		char *str_copy = calloc(strlen(str) + 1, sizeof(char));
		strcpy(str_copy, str);

		char buff_[64] = {0};
		char *buff = &buff_[0];
		buff = strtok(str_copy, ",]");

		dst->val.integer_value = atoi(buff);
		return strlen(buff);
	}

	dst->type = TYPE_LIST;

	uint32_t char_len = 0;
	uint32_t len = 0;

	while (rbracket != lbracket)
	{
		tmp = str[i++];
		if (tmp == '[')
			++lbracket;
		else if (tmp == ']')
			++rbracket;
		else if (tmp == ',')
			if (lbracket - rbracket == 1)
				++len;

		++char_len;
	}

	++char_len;

	const uint8_t is_empty = char_len <= 2;

	if (is_empty)
		dst->list_len = 0;
	else
	{
		++len;

		dst->list_len = len;
		dst->val.list_content = calloc(len, sizeof(*dst->val.list_content));
		uint32_t prev_size = 1;
		for (uint32_t i = 0; i < len; ++i)
		{
			prev_size += read_obj(str + prev_size, &(dst->val.list_content[i]));
			prev_size += str[prev_size] == ',';
		}
	}

	return char_len;
}

void print_obj(const obj o)
{
	if (o.type == TYPE_INT)
	{
		printf("%u", o.val.integer_value);
		return;
	}

	if (o.list_len == 0)
	{
		printf("[]");
		return;
	}

	putchar('[');
	for (uint32_t i = 0; i < o.list_len - 1; ++i)
	{
		print_obj(o.val.list_content[i]);
		putchar(',');
	}
	print_obj(o.val.list_content[o.list_len - 1]);
	putchar(']');

	return;
}

uint8_t cmp_obj(obj first, obj second)
{
	uint8_t type = first.type + second.type;

	switch (type)
	{
	case CMP_TYPE_INTS:
		if (first.val.integer_value < second.val.integer_value)
			return ORDER_RIGHT;
		else if (first.val.integer_value == second.val.integer_value)
			return ORDER_INIMPORTANT;
		else
			return ORDER_FALSE;
		break;

	case CMP_TYPE_DIFF:
		if (first.type == TYPE_INT)
			return cmp_obj((obj){
												 .type = TYPE_LIST,
												 .list_len = 1,
												 .val = {
														 .list_content = &first}},
										 second);
		else
			return cmp_obj(first, (obj){
																.type = TYPE_LIST,
																.list_len = 1,
																.val = {
																		.list_content = &second}});
		break;

	case CMP_TYPE_LISTS:
		if (first.list_len == 0 && second.list_len == 0)
			return ORDER_INIMPORTANT;
		if (first.list_len == 0)
			return ORDER_RIGHT;
		if (second.list_len == 0)
			return ORDER_FALSE;

		for (uint32_t i = 0; i < max(first.list_len, second.list_len); ++i)
		{
			uint8_t res = cmp_obj(first.val.list_content[i], second.val.list_content[i]);
			if (res != ORDER_INIMPORTANT)
				return res;

			if (first.list_len != second.list_len)
			{
				if (i >= first.list_len - 1)
					return ORDER_RIGHT;
				if (i >= second.list_len - 1)
					return ORDER_FALSE;
			}
		}
		break;

	default:
		fprintf(stderr, "[ERROR] Unreachable !!!");
		break;
	}

	return ORDER_INIMPORTANT;
}