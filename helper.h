#ifndef __HELPER__
#define __HELPER__

#include <math.h>

#ifdef _WIN32
#include <windows.h>

typedef struct timer_s
{
	LARGE_INTEGER frequency;
	LARGE_INTEGER start;
} timer;

void timer_start(timer *timer);
double timer_stop(timer *timer);
#endif

FILE *init_run();
static uint64_t get_run_count();

double mean_time(double times[ITER]);
double std_dev_time(double times[ITER]);

#endif // __HELPER

#ifdef __HELPER_IMPLEMENTATION__

#ifdef _WIN32

void timer_start(timer *timer)
{
	QueryPerformanceFrequency(&timer->frequency);
	QueryPerformanceCounter(&timer->start);
	return;
}

double timer_stop(timer *timer)
{
	double interval;
	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);
	interval = (double)(end.QuadPart - timer->start.QuadPart) / timer->frequency.QuadPart;
	return interval;
}

#endif

FILE *init_run()
{
	// getting the run number to not overwrite the previous run
	const uint64_t run_count = get_run_count();

	// create a new dir to hold the log of the current run
	char dir_name[BUFF_SIZE], file_name[BUFF_SIZE];
	snprintf(dir_name, BUFF_SIZE, LOGFILE "/run %" PRIu64, run_count);
	mkdir(dir_name);

	strncpy(file_name, dir_name, BUFF_SIZE);
	strcat(file_name, "/runlog.txt");
	// file to save global data like time
	FILE *run_log = fopen(file_name, "w");

	fprintf(run_log, "####################################################\n"
									 "THE LOG FILE OF THE RUN %" PRIu64 "\n"
									 "####################################################\n"
									 "CONFIG OF THIS RUN :\n"
									 "ITER = %u\n"
									 "####################################################\n"
									 "SAMPLE TIMES :\n",
					run_count, ITER);

	return run_log;
}

static uint64_t get_run_count()
{
	uint64_t run_count;

	FILE *f = fopen("run_numb", "r");
	if (f != NULL)
		fread(&run_count, sizeof(run_count), 1, f);

	fclose(f);

	// update the run number
	f = fopen("run_numb", "w");

	++run_count;

	fwrite(&run_count, sizeof(run_count), 1, f);

	fclose(f);
	return run_count;
}

double mean_time(double times[ITER])
{
	double mean, sum = 0;

	for (uint64_t i = 0; i < ITER; ++i)
		sum += times[i];
	mean = sum / ITER;
	return mean;
}

double std_dev_time(double times[ITER])
{
	double numerator = 0;
	const double mean = mean_time(times);
	for (uint64_t i = 0; i < ITER; ++i)
	{
		const double dev = times[i] - mean;
		numerator += dev * dev;
	}
	return sqrt(numerator / ITER);
}

#endif // __HELPER_IMPLEMENTATION__