#include <stdio.h>
#include <stdint.h>

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	const int grid[3][2] = {{0, 1},
													{2, 3},
													{3, 4}};

	printf("%i\n", grid[2][0]);

	return 0;
}