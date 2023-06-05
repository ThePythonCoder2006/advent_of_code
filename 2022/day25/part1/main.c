#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#endif

// #define TEST

#ifndef TEST
#define INPUT "input.txt"
#else
#define INPUT "test_input.txt"
#endif

#define MAX_DGT_NUM 32

enum
{
	D_MINUS = -2,
	MINUS,
	ZERO,
	ONE,
	TWO,
	BASE,
	DGT_UNREACH,
};

const int8_t digits[17] = {MINUS, DGT_UNREACH, DGT_UNREACH, ZERO, ONE, TWO,
													 DGT_UNREACH, DGT_UNREACH, DGT_UNREACH, DGT_UNREACH, DGT_UNREACH, DGT_UNREACH, DGT_UNREACH, DGT_UNREACH, DGT_UNREACH, DGT_UNREACH, D_MINUS};
#define GET_DIGIT(ch) digits[ch - 45]

const char digits_str_[5] = "=-012";
const char *const digits_str = &(digits_str_[0]) + 2;

typedef struct SNAFU_s
{
	int8_t digits[MAX_DGT_NUM];
} SNAFU;

void SNAFU_init(SNAFU *snafu);
void SNAFU_fread_line(FILE *f, SNAFU *ptr);
void SNAFU_add(SNAFU *rop, SNAFU *op1, SNAFU *op2);
uint32_t SNAFU_print(SNAFU *snafu);
void SNAFU_set_zero(SNAFU *snafu);

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

#ifdef _WIN32
	LARGE_INTEGER frequency;
	LARGE_INTEGER start;
	LARGE_INTEGER end;
	double interval;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);
#endif

	FILE *f = fopen("../" INPUT, "r");
	if (f == NULL)
	{
		fprintf(stderr, "couln't open file %s : %s", INPUT, strerror(errno));
		return EXIT_FAILURE;
	}

	uint32_t int_tot;
	SNAFU tot;
	SNAFU_init(&tot);

	SNAFU curr;
	SNAFU_init(&curr);

	char tmp;
	do
	{
		SNAFU_set_zero(&curr);
		SNAFU_fread_line(f, &curr);
		SNAFU_add(&tot, &tot, &curr);
		int_tot += SNAFU_print(&curr);
		printf("\t\t");
		SNAFU_print(&tot);
		printf("\t\t%u", int_tot);
		putchar('\n');
	} while (fscanf(f, "%c", &tmp) != EOF);

	SNAFU_print(&tot);
	putchar('\n');

#ifdef _WIN32
	QueryPerformanceCounter(&end);
	interval = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart * 1000;

	printf("%f milliseconds\n", interval);
#endif

	fclose(f);

	return EXIT_SUCCESS;
}

void SNAFU_init(SNAFU *snafu)
{
	SNAFU_set_zero(snafu);
	return;
}

void SNAFU_set_zero(SNAFU *snafu)
{
	memset(&(snafu->digits[0]), ZERO, MAX_DGT_NUM);
	return;
}

void SNAFU_fread_line(FILE *f, SNAFU *ptr)
{
	char line[33] = {0};
	fscanf(f, "%32[^\n]", line);

	uint8_t len = strlen(line);

	for (uint8_t i = 0; i < len; ++i)
		ptr->digits[MAX_DGT_NUM - i - 1] = GET_DIGIT(line[len - i - 1]);

	return;
}

void SNAFU_add(SNAFU *rop, SNAFU *op1, SNAFU *op2)
{
	for (uint8_t i = MAX_DGT_NUM - 1; i != 0; --i)
	{
		if (op1->digits[i] >= DGT_UNREACH || op2->digits[i] >= DGT_UNREACH)
		{
			fprintf(stderr, "[ERROR] unreachable !! (%u)\n", __LINE__);
			return;
		}
		int8_t res = op1->digits[i] + op2->digits[i];
		if (res >= D_MINUS)
		{
			uint8_t gt = (res >= BASE);
			if (gt)
				res -= 5, ++rop->digits[i - 1];
			rop->digits[i] = res;
		}
		else
		{
			res += 5;
			rop->digits[i] = res;
			--rop->digits[i - 1];
		}
	}
	return;
}

uint32_t SNAFU_print(SNAFU *snafu)
{
	uint8_t is_still_zero = 1;
	uint32_t tot = 0;
	uint32_t pow = 1;
	uint8_t change_idx = 0;
	for (uint8_t i = 0; i < MAX_DGT_NUM; ++i)
	{
		if (is_still_zero && snafu->digits[i] != 0)
			is_still_zero = 0, change_idx = i;
		if (is_still_zero)
			continue;
		putchar(digits_str[snafu->digits[i]]);
		tot += snafu->digits[MAX_DGT_NUM + change_idx - i - 1] * pow;
		pow *= 5;
	}
	printf("\t  %9u", tot);
	return tot;
}