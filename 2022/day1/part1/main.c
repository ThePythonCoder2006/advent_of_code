#include <stdio.h>
#include <stdint.h>

#define CHECK_EOF(r) \
	if (r == EOF)      \
	{                  \
		run = 0;         \
		break;           \
	}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	FILE *f = fopen("../input.txt", "r");
	if (f == NULL)
		return 1;

	uint32_t max = 0;
	uint32_t acc = 0;
	uint32_t int_read = 0;

	uint8_t run = 1;

	char tmp = 0;
	int c;
	while (run)
	{
		do
		{
			CHECK_EOF(c)

			c = fscanf(f, "%u%*1[\n]", &int_read);

			CHECK_EOF(c)

			acc += int_read;

		} while ((c = fscanf(f, "%1[\n]", &tmp)) < 1);

		if (acc > max)
			max = acc;

		acc = 0;
	}

	printf("max = %i", max);

	fclose(f);

	return 0;
}