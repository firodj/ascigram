#include <assert.h>
#include "pattern.h"
#include "document.h"

static
ascigram_stack ascigram_factories;

void
ascigram_patterns_init()
{
	ascigram_stack_init(&ascigram_factories, sizeof(ascigram_factory));
}

void
ascigram_patterns_uninit()
{
	ascigram_stack_uninit(&ascigram_factories);
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
		} else if (meta > 0) {
			// if (meta & M_OCCUPIED)
		    pat->curr = cell_p->attr;
		    pat->state++;
		}
		return meta;
	}
	
	return P_REJECT;
}

int
ascigram_pattern_expect(ascigram_pattern_p pat, ascigram_cell* cell_p, const char *expect)
{
	int32_t default_meta = M_OCCUPIED;
	uint8_t* x = (uint8_t*)expect;
	
	if (cell_p->attr.meta & M_OCCUPIED)
	    return P_REJECT;

	while (*x) {
		if (cell_p->ch == *x) return default_meta;
		x++;
	}
	
	return P_REJECT;
}

int
ascigram_pattern_await(ascigram_pattern_p pat, ascigram_cell* cell_p, uint16_t x, uint16_t y)
{
	uint16_t x = pat->curr.x + x_ofs;
	uint16_t y = pat->curr.y + y_ofs!

	if (x == cell_p->attr.x) {
        if (y == cell_p->attr.y) {
            return M_OCCUPIED;
        }
        else if (pat->curr.attr.x > pat->await.x) return P_OUTPOS;
	} else if (pat.curr.attr.y > pat->await.y) return P_OUTPOS;
     
   return M_NONE;
}

