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
#include "../../../helper.h"

// #define TEST

#ifndef TEST
#define INPUT "input.txt"
#else
#define INPUT "test_input.txt"
#endif

#define STR_SIZE (256)

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

    uint64_t acc = 0;

    int tmp = '\0';
    do
    {
      char line[STR_SIZE];
      fscanf(f, "%s", line);
      // printf("%s\n", line);

      char first = '\0', last = '\0';

      size_t len = strlen(line);

      for (size_t i = 0; i < len; ++i)
      {
        if (isdigit(line[i]))
        {
          if (first == '\0')
          {
            first = line[i];
            last = first;
          }
          else
            last = line[i];
        }
      }

      char num[3] = {first, last, '\0'};
      uint8_t val = atoi(num);
      // printf("%u\n", val);
      acc += val;
      tmp = fgetc(f);
    } while (tmp == '\n');

    rewind(f);

    times[sample_idx] = timer_stop(&sample) * 1000;
  }

  fclose(f);

  const double tot_time = timer_stop(&tot) * 1000;
  const double mean = mean_time(times);
  const double std_dev = std_dev_time(times, mean);

  printf("total execution was %f milliseconds\n", tot_time);
  printf("on average a single run of the algorithm took %f milliseconds, with a standart deviation of %f\n", mean, std_dev);

  return EXIT_SUCCESS;
}