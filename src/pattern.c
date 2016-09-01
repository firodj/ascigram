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
	ascigram_pattern_p pat = NULL;

	assert(fact_p);

	pat = (ascigram_pattern_p) ascigram_malloc(sizeof(ascigram_pattern));
	ascigram_memset(pat, 0, sizeof(ascigram_pattern));
	pat->factory = fact;
	
	ascigram_stack_init(&pat->attrs, sizeof(ascigram_attr));

	if (pat->factory->init)
		pat->factory->init(pat);
	
	return pat;
}

void
ascigram_pattern_free(ascigram_pattern_p pat)
{
	if (pat->factory->uninit)
		pat->factory->uninit(pat);

	ascigram_stack_uninit(&pat->attrs);

	free(pat);
}

int
ascigram_pattern_test(ascigram_pattern_p pat, ascigram_cell* cell_p)
{
	int meta;
	
	if (pat->factory->match) {
		meta = pat->factory->match(pat, cell_p);
		if (meta < 0) {
			pat->finish = meta;
		}
		return meta;
	}
	
	return P_REJECT;
}

int
ascigram_pattern_expect(ascigram_pattern_p pattern, const char *expect) {
}
