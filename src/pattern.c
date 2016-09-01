#include <assert.h>
#include "pattern.h"
#include "patterns/stickman.h"
#include "document.h"

static
ascigram_stack ascigram_factories;

void
ascigram_patterns_initialize()
{
	ascigram_stack_init(&ascigram_factories, sizeof(ascigram_factory));
	
	stickman_pattern_register();
}

void
ascigram_patterns_register(ascigram_factory *fact)
{
	ascigram_stack_push(&ascigram_factories, fact);
}

ascigram_factory*
ascigram_patterns_iter(int *index) {
	return ascigram_stack_iter(&ascigram_factories, index);
}

ascigram_pattern_p
ascigram_pattern_new(ascigram_factory *fact)
{
	ascigram_pattern_p pattern = NULL;

	assert(fact);

	pattern = (ascigram_pattern_p) ascigram_malloc(sizeof(ascigram_pattern));
	ascigram_memset(pattern, 0, sizeof(ascigram_pattern));
	pattern->factory = fact;
	
	ascigram_stack_init(&pattern->attrs, sizeof(ascigram_attr));

	if (pattern->factory->init)
		pattern->factory->init(pattern);
	
	return pattern;
}

void
ascigram_pattern_free(ascigram_pattern *pattern)
{
	if (pattern->factory->uninit)
		pattern->factpry->uninit(pattern);

	ascigram_stack_uninit(&pattern->attrs);

	free(pattern);
}

int
ascigram_pattern_test(ascigram_pattern_p pattern, ascigram_cell_p cell)
{
	int meta;
	
	if (pattern->factory->match) {
		meta = pattern->factory->match(pattern, cell);
		if (meta < 0) {
			pattern->finish = meta;
		}
		return meta;
	}
	
	return P_REJECT;
}

int
ascigram_pattern_expect(ascigram_pattern_p pattern, const char *expect) {
}
