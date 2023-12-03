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

#define COLOR_CNT (3)
char colors[][6] = {"red", "green", "blue"};

enum col
{
  RED,
  GREEN,
  BLUE
};

#define LINE_SIZE (256)

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

    char line_[LINE_SIZE] = {0};
    char *line = line_;
    char tmp[LINE_SIZE] = {0};
    uint64_t game_idx = 1;

    uint64_t acc = 0;

    do
    {
      line = line_;
      fscanf(f, "%[^\n]", line);

      while (*line != ':')
        ++line;

      line += 2; // skip the ": "
      uint32_t cnt = 0;
      int shift, val;
      char sep = 0;

      uint64_t cnt_color[3] = {0};

      do
      {
        cnt = val = sep = 0;
        val = sscanf(line, "%u %[^;^,] %c %n", &cnt, tmp, &sep, &shift);

        for (uint8_t i = 0; i < COLOR_CNT; ++i)
          if (strcmp(tmp, colors[i]) == 0)
          {
            if (cnt > cnt_color[i])
              cnt_color[i] = cnt;
            break;
          }

        line += shift;
      } while (val >= 3);

      acc += cnt_color[RED] * cnt_color[GREEN] * cnt_color[BLUE];

      ++game_idx;
    } while (fgetc(f) == '\n');

    printf("sum of power of all the games : %llu\n", acc);

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