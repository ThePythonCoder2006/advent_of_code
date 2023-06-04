#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

// #define TEST

#ifndef TEST
#define INPUT "input.txt"
#define PACKET_NUM 300
#else
#define INPUT "test_input.txt"
#define PACKET_NUM 16
#endif

char SEPS[2][6] = {"[[2]]", "[[6]]"};

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

enum
{
	SEP_NONE,
	SEP_FIRST,
	SEP_SECOND,
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
	uint8_t is_sep;
} obj;

uint32_t max(uint32_t a, uint32_t b)
{
	if (a > b)
		return a;
	else
		return b;
}

void obj_init(obj *o);
uint32_t read_obj(char *str, obj *dst);
void print_obj(const obj o);
uint8_t cmp_obj(obj first, obj second);
void swap_obj(obj packets[PACKET_NUM + 2], uint32_t idx1, uint32_t idx2, uint32_t seps_idx[2]);
void packets_sort(obj packets[PACKET_NUM + 2], uint32_t seps_idx[2]);
void print_packets(obj packets[PACKET_NUM + 2]);

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

	uint32_t seps_idx[2] = {PACKET_NUM, PACKET_NUM + 1};

	obj packets[PACKET_NUM + 2];
	for (uint16_t i = 0; i < PACKET_NUM + 2; ++i)
		obj_init(&packets[i]);
	packets[seps_idx[0]].is_sep = SEP_FIRST;
	packets[seps_idx[1]].is_sep = SEP_SECOND;
	read_obj(SEPS[0], &packets[seps_idx[0]]);
	read_obj(SEPS[1], &packets[seps_idx[1]]);

	char line[256] = {0};

	for (uint32_t i = 0; i < PACKET_NUM;)
	{
		fscanf(f, "%256[^\n]%*c", line);
		read_obj(line, &packets[i]);
		++i;
		fscanf(f, "%256[^\n]%*c", line);
		read_obj(line, &packets[i]);
		fscanf(f, "%*c");
		++i;
	}

	// print_packets(packets);

	packets_sort(packets, seps_idx);

	// printf("\n\n");
	// print_packets(packets);

	++seps_idx[0], ++seps_idx[1]; // convert them from 0-counting to 1-counting

	printf("%u * %u = %u\n", seps_idx[0], seps_idx[1], seps_idx[0] * seps_idx[1]);

	fclose(f);

	return EXIT_SUCCESS;
}

void obj_init(obj *o)
{
	o->list_len = 0;
	o->type = TYPE_INT;
	o->val.integer_value = 0;
	o->val.list_content = NULL;
	o->is_sep = 0;

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

		char *buff = malloc(BUFF_LEN);
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
			if (str[prev_size] == ',')
				++prev_size;
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

void swap_obj(obj packets[PACKET_NUM + 2], uint32_t idx1, uint32_t idx2, uint32_t seps_idx[2])
{
	if (packets[idx1].is_sep == 0 && packets[idx2].is_sep == 0)
		goto swap;

	if (idx1 == seps_idx[0])
		seps_idx[0] = idx2;
	if (idx2 == seps_idx[0])
		seps_idx[0] = idx1;
	if (idx1 == seps_idx[1])
		seps_idx[1] = idx2;
	if (idx2 == seps_idx[1])
		seps_idx[1] = idx1;

	obj tmp;
swap:
	memcpy(&tmp, &packets[idx1], sizeof(*packets));
	memcpy(&packets[idx1], &packets[idx2], sizeof(*packets));
	memcpy(&packets[idx2], &tmp, sizeof(*packets));

	return;
}

void packets_sort(obj packets[PACKET_NUM + 2], uint32_t seps_idx[2])
{
	for (uint32_t i = 0; i < PACKET_NUM + 1; ++i)
		for (uint32_t j = 0; j < PACKET_NUM + 1 - i; ++j)
			if (cmp_obj(packets[j], packets[j + 1]) == ORDER_FALSE)
				swap_obj(packets, j, j + 1, seps_idx);
}

void print_packets(obj packets[PACKET_NUM + 2])
{
	for (uint32_t i = 0; i < PACKET_NUM + 2; ++i)
	{
		print_obj(packets[i]);
		putchar('\n');
	}

	return;
}