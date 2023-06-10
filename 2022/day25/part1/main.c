#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <immintrin.h>

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
	UNREACH,
};

const int8_t digits_lut[32] __attribute__((aligned(16))) = {
		0, UNREACH, MINUS, UNREACH, ZERO, ONE, TWO, UNREACH,
		UNREACH, UNREACH, UNREACH, UNREACH, UNREACH, UNREACH, D_MINUS, UNREACH,
		0, UNREACH, MINUS, UNREACH, ZERO, ONE, TWO, UNREACH,
		UNREACH, UNREACH, UNREACH, UNREACH, UNREACH, UNREACH, D_MINUS, UNREACH};
__m256i digits_lutv;
#define GET_OFF_IDX(ch) (ch >> 2)
#define GET_IDX(ch) ((i - GET_OFF_IDX(ch)) & 0x0F)
#define GET_DIGIT(ch) digits_lut[GET_IDX(ch)] // (-GET_OFF_IDX('-') + '-') = 34

const char digits_str_[] = {'=', '-', '0', '1', '2'};
const char *const digits_str = &(digits_str_[0]) - D_MINUS;

const int8_t add_lut_low[33] __attribute__((aligned(16))) = {
		0, 1, 2, 11, 12, 13, 14, 15,
		16, 17, 18, 19, -1, 0, -2, -1,
		0, 1, 2, 11, 12, 13, 14, 15,
		16, 17, 18, 19, -1, 7, -2, -1, 20};
__m256i vadd_lut_low;
const int8_t add_lut_high[32] __attribute__((aligned(16))) = {
		0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 0, -1, -1, 0, 0,
		0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 0, -1, -1, 0, 0};
__m256i vadd_lut_high;

typedef struct SNAFU_s
{
	__m256i digits;
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

	digits_lutv = _mm256_loadu_si256((const __m256i *)digits_lut);
	vadd_lut_low = _mm256_loadu_si256((const __m256i *)add_lut_low);
	vadd_lut_high = _mm256_loadu_si256((const __m256i *)add_lut_high);

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
	snafu->digits = _mm256_setzero_si256();
	return;
}

void SNAFU_fread_line(FILE *f, SNAFU *ptr)
{
	char line_tmp[33] = {0};
	fscanf(f, "%32[^\n]", line_tmp);
	uint8_t len = strlen(line_tmp);

	char line[32] = {0};
	memcpy(line + 32 - len, line_tmp, len);

	const __m256i shift_mask = _mm256_set1_epi8(0x3F);

	ptr->digits = _mm256_loadu_si256((const __m256i *)line);
	const __m256i off = _mm256_and_si256(_mm256_srli_epi16(ptr->digits, 2), shift_mask); // >> 2
	const __m256i idx = _mm256_sub_epi8(ptr->digits, off);															 // - off - lut_const
	ptr->digits = _mm256_shuffle_epi8(digits_lutv, idx);

	return;
}

void SNAFU_add(SNAFU *rop, SNAFU *op1, SNAFU *op2)
{

	__m256i op1v = op1->digits;
	__m256i op2v = op2->digits;

	__m256i sum = _mm256_and_si256(_mm256_add_epi8(op1v, op2v), _mm256_set1_epi8(0x0F));

	__m256i low = _mm256_shuffle_epi8(vadd_lut_low, sum);
	__m256i high = _mm256_shuffle_epi8(vadd_lut_high, sum);
	rop->digits = _mm256_add_epi8(low, high);
	return;
}

void SNAFU_print(SNAFU *snafu)
{
	int8_t *digs = (int8_t *)&(snafu->digits);
	uint8_t is_still_zero = 1;
	for (uint8_t i = 0; i < MAX_DGT_NUM; ++i)
	{
		if (is_still_zero && digs[i] != 0)
			is_still_zero = 0;
		if (is_still_zero)
			continue;
		putchar(digits_str[digs[i]]);
	}
	return;
}