/* option.h */
#ifndef _ASCIGRAM_OPTION_H_
#define _ASCIGRAM_OPTION_H_

#define str(x) __str(x)
#define __str(x) #x

int parse_options(
    int argc, char **argv,
    int(*parse_short_option)(char opt, char *next, void *opaque),
    int(*parse_long_option)(char *opt, char *next, void *opaque),
    int(*parse_argument)(int argn, char *arg, int is_forced, void *opaque),
    void *opaque);

int parseint(const char *string, long *result);

#endif /** _ASCIGRAM_OPTION_H_ **/
