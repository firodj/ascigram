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

int
test_pattern_stickman(void)
{
	int results = 0;

	int fact_i;
	ascigram_factory *fact;
	ascigram_pattern_p pat;
	ascigram_cell cell;
	int meta;

	// Get factory for stickman
	stickman_pattern_register();
	fact_i = 0;
	while (fact = ascigram_patterns_iter(&fact_i)) {
		if (strcmp("stickman", fact->name) == 0) break;
	}

	EXPECT(NULL != fact, "fact is NULL");
	
	// Check state for Stickman
	pat = ascigram_pattern_new(fact);

	EXPECT(0 == pat->state, "pat->state: %d", pat->state);
	EXPECT(0 == pat->finish, "pat->finish: %d", pat->finish);

	ascigram_memset(&cell, 0, sizeof(ascigram_cell));
	cell.ch = ' ';
	cell.attr.x = 0;
	cell.attr.y = 0;

	meta = ascigram_pattern_test(pat, &cell);
	EXPECT(P_REJECT == pat->finish, "pat->finish: 0x%x", pat->finish);
	EXPECT(M_NONE == meta, "meta: 0x%x", meta);

	ascigram_pattern_free(pat);

	//
	pat = ascigram_pattern_new(fact);

	ascigram_memset(&cell, 0, sizeof(ascigram_cell));
	cell.ch = 'o';
	cell.attr.x = 1;
	cell.attr.y = 0;

	meta = ascigram_pattern_test(pat, &cell);
	EXPECT(M_OCCUPIED == meta, "meta: 0x%x", meta);
	EXPECT(1 == pat->curr.x, "pat->curr.x: %d", pat->curr.x);

	//
	ascigram_memset(&cell, 0, sizeof(ascigram_cell));
	cell.ch = ' ';
	cell.attr.x = 2;
	cell.attr.y = 0;
	
	meta = ascigram_pattern_test(pat, &cell);
	EXPECT(M_NONE == meta, "meta: 0x%x", meta);
	EXPECT(1 == pat->curr.x, "pat->curr.x: %d", pat->curr.x);

	//
	ascigram_memset(&cell, 0, sizeof(ascigram_cell));
	cell.ch = '-';
	cell.attr.x = 0;
	cell.attr.y = 1;

	meta = ascigram_pattern_test(pat, &cell);
	EXPECT(M_OCCUPIED == meta, "meta: 0x%x", meta);
	EXPECT(1 == pat->curr.y, "pat->curr.y: %d", pat->curr.x);

	ascigram_pattern_free(pat);

	return results;
}
