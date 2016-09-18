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
	size_t total_size;

	assert(fact);

	total_size = sizeof(ascigram_pattern) + fact->opaque_size - sizeof(pat->opaque[1]);
	pat = (ascigram_pattern_p) ascigram_malloc(total_size);
	ascigram_memset(pat, 0x00, total_size);
	pat->factory = fact;
	
	ascigram_stack_init(&pat->attrs, sizeof(ascigram_attr));

	return pat;
}

void
ascigram_pattern_free(ascigram_pattern_p pat)
{
	ascigram_stack_uninit(&pat->attrs);

	free(pat);
}

uint32_t
ascigram_pattern_test(ascigram_pattern_p pat, ascigram_cell* cell_p)
{
	uint32_t meta;
	
	if (pat->factory->match) {
		meta = pat->factory->match(pat, cell_p);
		if (meta & E_RESULT) {
			/* if REJECTED but ACCEPTED then FINISH */
			if (pat->finish == P_ACCEPT) {
				pat->finish = P_FINISH;
			} else {
				pat->finish = (meta & E_RESULT);
			}

			if (pat->finish & E_FAIL) {
				meta = M_NONE;
			}
		} 
		if (meta & E_MASK) {
		    pat->curr = cell_p->attr;
		}
		return (meta & E_MASK);
	}
	
	pat->finish = P_REJECT;
	return M_NONE;
}

uint32_t
ascigram_pattern_expect(ascigram_pattern_p pat, ascigram_cell* cell_p, const char *expect, uint32_t default_meta)
{
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
ascigram_pattern_await(ascigram_pattern_p pat, ascigram_cell* cell_p, int16_t x, int16_t y, uint32_t *meta)
{
	if (x == cell_p->attr.x) {
        if (y == cell_p->attr.y) {
            *meta = M_OCCUPIED;
            return 0;
        }
        else if (x < cell_p->attr.x) {
            *meta = P_OUTPOS;
            return 1;
        }
	} else if (y < cell_p->attr.y) {
		*meta = P_OUTPOS;
		return 1;
    }

    *meta = M_NONE;
    return 1;
}

