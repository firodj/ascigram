#include "pattern.h"
#include "patterns/stickman.h"

static
ascigram_stack ascigram_patterns;

void
ascigram_patterns_initialize()
{
	ascigram_stack_init(&ascigram_patterns);
	
	stickman_pattern_register();
}

void
ascigram_patterns_register(ascigram_pattern *pattern)
{
	ascigram_stack_push(ascigram_patterns, pattern);
}

int ascigram_pattern_expect(const char *) {
}