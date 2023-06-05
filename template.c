#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#endif

// #define TEST

#ifndef TEST
#define INPUT "input.txt"
#else
#define INPUT "test_input.txt"
#endif

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

#ifdef _WIN32
	QueryPerformanceCounter(&end);
	interval = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart * 1000000;

	printf("%f microseconds\n", interval);
#endif

	fclose(f);

	return EXIT_SUCCESS;
}