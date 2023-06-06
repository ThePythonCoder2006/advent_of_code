#ifndef __HELPER__
#define __HELPER__

#include <math.h>

#ifndef __HELPER_BUFF_SIZE__
#define __HELPER_BUFF_SIZE__ 64
#endif

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

double mean_time(double times[ITER]);
double std_dev_time(double times[ITER], double mean);

#endif // __HELPER

#ifdef __HELPER_IMPLEMENTATION__

#ifdef _WIN32

void timer_start(timer *timer)
{
	QueryPerformanceFrequency(&(timer->frequency));
	QueryPerformanceCounter(&(timer->start));
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

double mean_time(double times[ITER])
{
	double mean, sum = 0;

	for (uint64_t i = 0; i < ITER; ++i)
		sum += times[i];
	mean = sum / ITER;
	return mean;
}

double std_dev_time(double times[ITER], double mean)
{
	double numerator = 0;
	if (mean == 0)
		mean = mean_time(times);
	for (uint64_t i = 0; i < ITER; ++i)
	{
		const double dev = times[i] - mean;
		numerator += dev * dev;
	}
	return sqrt(numerator / ITER);
}

#endif // __HELPER_IMPLEMENTATION__