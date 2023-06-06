#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define ITER 10000
#define __HELPER_IMPLEMENTATION__
#include "../../../helper.h"

// #define _DEBUG
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
void SNAFU_print(SNAFU *snafu);
void SNAFU_set_zero(SNAFU *snafu);

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	timer sample, tot;
	double times[ITER] = {0};

	timer_start(&tot);

	FILE *f = fopen("../" INPUT, "r");
	if (f == NULL)
	{
		fprintf(stderr, "couln't open file %s : %s", INPUT, strerror(errno));
		return EXIT_FAILURE;
	}

	SNAFU sum;
	SNAFU_init(&sum);

	SNAFU curr;
	SNAFU_init(&curr);

	for (uint32_t i = 0; i < ITER; ++i)
	{
		timer_start(&sample);
		SNAFU_set_zero(&sum);
		char tmp;
		do
		{
			SNAFU_set_zero(&curr);
			SNAFU_fread_line(f, &curr);
			SNAFU_add(&sum, &sum, &curr);
		} while (fscanf(f, "%c", &tmp) != EOF);

		rewind(f);

		times[i] = timer_stop(&sample) * 1000;
	}

	fclose(f);

	SNAFU_print(&sum);
	putchar('\n');

	const double tot_time = timer_stop(&tot) * 1000;
	const double mean = mean_time(times);
	const double std_dev = std_dev_time(times, mean);

	printf("total execution was %f milliseconds\n", tot_time);
	printf("on average a single run of the algorithm took %f milliseconds, with a standart deviation of %f\n", mean, std_dev);
	printf("%f\n", mean * ITER);

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
#ifdef _DEBUG
		if (op1->digits[i] >= DGT_UNREACH || op2->digits[i] >= DGT_UNREACH)
		{
			fprintf(stderr, "[ERROR] unreachable !! (%u)\n", __LINE__);
			return;
		}
#endif

		int8_t res = op1->digits[i] + op2->digits[i];
		if (res >= D_MINUS)
		{
			uint8_t gt = (res >= BASE);
			res -= 5 * gt;
			rop->digits[i - 1] += gt;
			return;
		}
		else
		{
			res += 5;
			--rop->digits[i - 1];
		}

		rop->digits[i] = res;
	}
	return;
}

void SNAFU_print(SNAFU *snafu)
{
	uint8_t is_still_zero = 1;
	for (uint8_t i = 0; i < MAX_DGT_NUM; ++i)
	{
		if (is_still_zero && snafu->digits[i] != 0)
			is_still_zero = 0;
		if (is_still_zero)
			continue;
		putchar(digits_str[snafu->digits[i]]);
	}
	return;
}