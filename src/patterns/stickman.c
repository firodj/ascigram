#include "..\buffer.h"
#include "..\pattern.h"

int
stickman_pattern_match(ascigram_pattern_p, ascigram_cell*);

static
ascigram_factory stickman_pattern_factory = {
	"stickman",
	stickman_pattern_match,
	NULL,
	NULL};
	
int
stickman_pattern_match(ascigram_pattern_p pat, ascigram_cell* cell)
{
	switch(pat->state) {
	case 0: do {
		pat->state = 1;
		return ascigram_pattern_expect(pat, "o");
	case 1:
		;
	    } while(0);
	}
	
	return -1;
}

void stickman_pattern_register()
{
	ascigram_patterns_register(&stickman_pattern_factory);
}
