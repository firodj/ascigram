/* pattern.h */
#ifndef _ASCIGRAM_PATTERN_H_
#define _ASCIGRAM_PATTERN_H_

struct ascigram_pattern;
typedef ascigram_pattern ascigram_pattern;

typedef int (*ascigram_pattern_match_t)(ascigram_pattern*, char, int, int, int);
typedef void (*ascigram_pattern_init_t)(ascigram_pattern*);
typedef void (*ascigram_pattern_uninit_t)(ascigram_pattern*);

struct ascigram_pattern {
	const char *name;
	int state;
	ascigram_pattern_match_t match;
	ascigram_pattern_init_t init;
	ascigram_pattern_uninit_t uninit;
	ascigram_stack *matches;
};

#endif /** _ASCIGRAM_PATTERN_H_ **/