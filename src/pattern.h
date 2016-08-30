/* pattern.h */
#ifndef _ASCIGRAM_PATTERN_H_
#define _ASCIGRAM_PATTERN_H_

#include "stack.h"

struct ascigram_pattern;
typedef struct ascigram_pattern * ascigram_pattern_p;

typedef int (*ascigram_pattern_match_t)(ascigram_pattern_p, char, int, int, int);
typedef void (*ascigram_pattern_init_t)(ascigram_pattern_p);
typedef void (*ascigram_pattern_uninit_t)(ascigram_pattern_p);

struct ascigram_pattern {
	const char *name;
	int state;
	ascigram_pattern_match_t match;
	ascigram_pattern_init_t init;
	ascigram_pattern_uninit_t uninit;
	ascigram_stack cell_refs;
};
typedef struct ascigram_pattern ascigram_pattern;

void ascigram_patterns_initialize();
void ascigram_patterns_register(ascigram_pattern_p pattern);
ascigram_pattern_p ascigram_pattern_new(ascigram_pattern_p fact);
void ascigram_pattern_free(ascigram_pattern_p pattern);
int ascigram_pattern_expect(ascigram_pattern_p pattern, const char *);

#endif /** _ASCIGRAM_PATTERN_H_ **/
