#include <assert.h>
#include "pattern.h"
#include "patterns/stickman.h"
#include "document.h"

static
ascigram_stack ascigram_patterns;

void
ascigram_patterns_initialize()
{
	ascigram_stack_init(&ascigram_patterns, sizeof(ascigram_pattern));
	
	stickman_pattern_register();
}

void
ascigram_patterns_register(ascigram_pattern_p pattern)
{
	ascigram_stack_push(&ascigram_patterns, pattern);
}

ascigram_pattern_p
ascigram_patterns_iter(int *index) {
	return ascigram_stack_iter(&ascigram_patterns, index);
}

ascigram_pattern_p
ascigram_pattern_new(ascigram_pattern_p fact)
{
	ascigram_pattern_p pattern = NULL;

	assert(template);

	pattern = (ascigram_pattern_p) ascigram_malloc(sizeof(ascigram_pattern));
	memcpy(pattern, fact, sizeof(ascigram_pattern));
	
	ascigram_stack_init(&pattern->attrs, sizeof(ascigram_attr));

	if (pattern->init)
		pattern->init(pattern);
	
	return pattern;
}

void
ascigram_pattern_free(ascigram_pattern *pattern)
{
	if (pattern->uninit)
		pattern->uninit(pattern);

	ascigram_stack_uninit(&pattern->attrs);

	free(pattern);
}

int
ascigram_pattern_expect(ascigram_pattern_p pattern, const char *expect) {
}
