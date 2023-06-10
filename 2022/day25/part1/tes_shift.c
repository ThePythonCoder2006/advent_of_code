#include <immintrin.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#define _mm256_shift_left(res, a, n)                                       \
	do                                                                       \
	{                                                                        \
		__m256i __b__[2] = {a};                                                \
		res = _mm256_loadu_si256((const __m256i *)((uint8_t *)(__b__) + (n))); \
	} while (0);

int main(int argc, char **argv)
{
	(void)argc, (void)argv;

	int val, n;
	scanf("%d%d", &val, &n);

	__m256i a = _mm256_set1_epi8(val);
	__m256i b;

	_mm256_shift_left(b, a, n);
	for (int i = 0; i < 32; ++i)
		printf(" %u", ((char *)&b)[i]);
	putchar('\n');
	return 0;
}