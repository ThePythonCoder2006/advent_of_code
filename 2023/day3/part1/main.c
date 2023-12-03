#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>

#define ITER 1
#define LOGFILE "logs"
#define BUFF_SIZE 64

#define __HELPER_IMPLEMENTATION__
#include "../../../helper.h"

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

  timer tot;
  timer sample;

  timer_start(&tot);

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

    rewind(f);

    times[sample_idx] = timer_stop(&sample) * 1000;
  }

  fclose(f);

  const double tot_time = timer_stop(&tot) * 1000;
  const double mean = mean_time(times);
  const double std_dev = std_dev_time(times, mean);

  printf("total execution was %f milliseconds\n", tot_time);
  printf("on average a single run of the algorithm took %f milliseconds, with a standart deviation of %f milliseconds\n", mean, std_dev);

  return EXIT_SUCCESS;
}