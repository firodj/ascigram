#include "option.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
parseint(const char *string, long *result)
{
	char *end;
	errno = 0;
	*result = strtol(string, &end, 10);
	return !(*end || errno);
}

int
parse_options(
	int argc, char **argv,
	int(*parse_short_option)(char opt, char *next, void *opaque),
	int(*parse_long_option)(char *opt, char *next, void *opaque),
	int(*parse_argument)(int argn, char *arg, int is_forced, void *opaque),
	void *opaque)
{
	int result;
	int i = 1, regular_args = 0;

	/* Parse options mixed with arguments */
	while (i < argc) {
		char *arg = argv[i];

		if (arg[0] == '-' && arg[1]) {
			char *next_arg = (i+1 < argc) ? argv[i+1] : NULL;

			if (arg[1] == '-' && !arg[2]) {
				/* '--' signals end of options */
				i++;
				break;
			}

			if (arg[1] == '-') {
				/* Long option */
				result = parse_long_option(arg + 2, next_arg, opaque);
				if (!result) return 0;
				i += result;
			} else {
				/* Sequence of short options */
				size_t pos;
				for (pos = 1; arg[pos]; pos++) {
					char *next = (arg[pos+1]) ? arg + pos+1 : next_arg;
					result = parse_short_option(arg[pos], next, opaque);
					if (!result) return 0;
					if (result == 2) {
						if (next == next_arg) i++;
						break;
					}
				}
				i++;
			}
		} else {
			/* Argument */
			result = parse_argument(regular_args++, arg, 0, opaque);
			if (!result) return 0;
			i++;
		}
	}

	/* Parse rest as forced arguments */
	while (i < argc) {
		result = parse_argument(regular_args++, argv[i], 1, opaque);
		if (!result) return 0;
		i++;
	}

	return 1;
}
