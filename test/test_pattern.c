#include <stdio.h>
#include "document.h"
#include "pattern.h"
#include "test.h"
#include <string.h>
#include "patterns/stickman.h"

int
test_pattern_factory(void)
{
	int results = 0;

	int fact_i;
	ascigram_factory *fact;
	ascigram_pattern_p pat;

	fact_i = 0;
	fact = ascigram_patterns_iter(&fact_i);

	EXPECT(NULL == fact, "fact = 0x%x, fact_i = %d", fact, fact_i);
	
	stickman_pattern_register();
	fact_i = 0;
	fact = ascigram_patterns_iter(&fact_i);

	EXPECT(strcmp("stickman", fact->name) == 0, "fact->name: %s", fact->name);
	
	pat = ascigram_pattern_new(fact);
	EXPECT(fact == pat->factory, "pat->factory: 0x%x != fact: 0x%x", pat->factory, fact);

	EXPECT(0 == pat->state, "pat->state: %d", pat->state);

	while(fact = ascigram_patterns_iter(&fact_i));
	EXPECT(1 == fact_i, "fact_i: %d", fact_i);

	ascigram_pattern_free(pat);

	return results;
}
