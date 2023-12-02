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

#define STR_SIZE (256)

char nums_in_letters[][6] =
    {
        "unset",
        "one",
        "two",
        "three",
        "four",
        "five",
        "six",
        "seven",
        "eight",
        "nine"};

size_t str_num_size[] = {0, 3, 3, 5, 4, 4, 3, 5, 5, 4};

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
          last = line[i];
          if (first == '\0')
            first = last;
          continue;
        }

        // if it is not a digit we still need to check if it is a number spelled out:
        for (uint8_t j = 1; j < 10; ++j)
        {
          // printf("%s, %s, %llu\n", line + i, nums_in_letters[j], str_num_size[j]);
          if (strncmp(line + i, nums_in_letters[j], str_num_size[j]) == 0)
          {
            // we have a match for number j
            // printf("valid: %s, %s\n", line + i, nums_in_letters[j]);
            last = j + '0';
            if (first == '\0')
              first = last;
          }
        }
      }

      char num[3] = {first, last, '\0'};
      // printf("%s\n", num);
      uint8_t val = atoi(num);
      // printf("%u\n", val);
      acc += val;
      tmp = fgetc(f);
      // putchar('\n');
    } while (tmp == '\n');

    printf("%llu\n", acc);

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