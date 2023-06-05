#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>

#define ITER 100
#define LOGFILE "logs"
#define BUFF_SIZE 64

#define __HELPER_IMPLEMENTATION__
#include "../../helper.h"

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
	timer tot;
	timer sample;

	timer_start(&tot);

	FILE *run_log = init_run();

	FILE *f = fopen("../" INPUT, "r");
	if (f == NULL)
	{
		fprintf(stderr, "couln't open file %s : %s", INPUT, strerror(errno));
		return EXIT_FAILURE;
	}

	double times[ITER] = {0};

	for (uint64_t sample_idx = 0; sample_idx < ITER; ++sample_idx)
	{
		timer_start(&sample);

		/*
		 code goes here
		*/

		times[sample_idx] = timer_stop(&sample);
	}

	fclose(f);

	double tot_time = timer_end(&tot);

	return EXIT_SUCCESS;
}