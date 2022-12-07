#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>

#define NAME_MAX_SIZE 64

typedef struct dir
{
	char name[NAME_MAX_SIZE];
	uint8_t dir_numb;
	struct dir **dirs;
	struct dir *back;
	int64_t size;
} dir_t;

// #define TEST

#ifndef TEST
#define INPUT "input.txt"
#else
#define INPUT "test_input.txt"
#endif

#define DIR_INIT_LIST_SIZE 20

dir_t *dir_init(const char *name, dir_t *back);

dir_t *add_dir(dir_t *rop, const char *name);

int add_file(dir_t *rop, uint64_t size);

dir_t *get_dir(dir_t *curr, const char *name);

dir_t *get_back(dir_t *curr);

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

	dir_t *curr;

	dir_t *root = dir_init("/", NULL);

	char tmp, command, name[64 + 1];

	if (fscanf(f, "%1[$]", &tmp) == 1)
	{
		fscanf(f, "%*1[ ]" // remove the ' ' following the '$' sign that indicates the commande
							"%c"		 // read first char of command
							"%*1c"	 // remove the second char of the command
					 ,
					 &command);

		if (command == 'c') // $ cd
		{
			fscanf(f, "%64[^\n]", name);
			get_dir(curr, name);
		}
		else if (command == 'l') // $ ls
		{
		}
		else
		{
		}
	}

	fclose(f);

	return EXIT_SUCCESS;
}

dir_t *dir_init(const char *name, dir_t *back)
{
	dir_t *dir;

	dir = malloc(sizeof(*dir));

	dir->dirs = malloc(DIR_INIT_LIST_SIZE * sizeof(dir_t *));
	dir->size = -1;
	dir->dir_numb = 0;
	dir->back = back;
	strcpy(dir->name, name);

	return dir;
}

dir_t *add_dir(dir_t *rop, const char *name)
{
	if (rop == NULL)
	{
		fprintf(stderr, "errro, pointer provided was NULL, no dir was added");
		return NULL;
	}

	return rop->dirs[rop->dir_numb++] = dir_init(name, rop);
}

int add_file(dir_t *rop, uint64_t size)
{
	if (rop == NULL)
	{
		fprintf(stderr, "errro, pointer provided was NULL, no file was added");
		return -1;
	}

	if (rop->size == 0)
	{
		rop->size = size;
		return 0;
	}

	rop->size += size;

	return 0;
}

dir_t *get_dir(dir_t *curr, const char *name)
{
	dir_t *dir;

	for (uint8_t i = 0; i < curr->dir_numb; ++i)
	{
		if (strcmp(curr->dirs[i]->name, name) == 0) // if the name of the dir is the same
			return dir;
	}
}

dir_t *get_back(dir_t *curr)
{
	return curr->back;
}