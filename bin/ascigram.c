#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "option.h"

/* OPTION PARSING */

struct option_data {
    char *base_name;
    int done;

    /* I/O */
    const char *filename;
};

int
parse_short_option(char opt, char *next, void *opaque)
{
	struct option_data *data = opaque;
	long int num;
	int isNum = next ? parseint(next, &num) : 0;

	return 0;
}

int
parse_long_option(char *opt, char *next, void *opaque)
{
	struct option_data *data = opaque;
	long int num;
	int isNum = next ? parseint(next, &num) : 0;

	return 0;
}

int
parse_argument(int argn, char *arg, int is_forced, void *opaque)
{
	struct option_data *data = opaque;

	if (argn == 0) {
		/* Input file */
		if (strcmp(arg, "-")!=0 || is_forced) data->filename = arg;
		return 1;
	}

	fprintf(stderr, "Too many arguments.\n");
	return 0;
}


int
main(int argc, char** argv[])
{
	struct option_data data;

	argc = parse_options(argc, argv, parse_short_option, parse_long_option, parse_argument, &data);
	if (data.done) return 0;
	if (!argc) return 1;

	return 0;
}
