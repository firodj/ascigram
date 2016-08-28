#ifndef _OPTION_H_
#define _OPTION_H_

int parse_options(
    int argc, char **argv,
    int(*parse_short_option)(char opt, char *next, void *opaque),
    int(*parse_long_option)(char *opt, char *next, void *opaque),
    int(*parse_argument)(int argn, char *arg, int is_forced, void *opaque),
    void *opaque);

int parseint(const char *string, long *result);

#endif
