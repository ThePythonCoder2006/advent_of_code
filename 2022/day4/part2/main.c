#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>

#define INPUT "input.txt"

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	FILE *f = fopen("../" INPUT, "r");
	if (f == NULL)
	{
		fprintf(stderr, "couln't open file %s : %s", INPUT, strerror(errno));
		return EXIT_FAILURE;
	}

	uint32_t start1, end1; // start and end for the first range
	uint32_t start2, end2; // start and end for the second range

	uint64_t count = 0;

	char tmp;

	do
	{
		if (fscanf(f, "%u"	// read start1
									"%*c" // remove '-'
									"%u"	// read end1
									"%*c" // remove ','
									"%u"	// read start2
									"%*c" // remove '-'
									"%u"	// read end2
							 ,
							 &start1, &end1, &start2, &end2) == EOF)
			break;

		if (start1 >= start2 && start1 <= end2)
			++count;
		else if (start2 >= start1 && start2 <= end1)
			++count;
	} while (fscanf(f, "%1[\n]", &tmp) != EOF);

	printf("the total amout of ranges that contain each other is : %" PRIu64 "\n", count);

	fclose(f);

	return EXIT_SUCCESS;
}