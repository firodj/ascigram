#include "..\buffer.h"
#include "..\pattern.h"

int
stickman_pattern_match(ascigram_pattern *pattern, char c, int x, int y, int m);

static
ascigram_pattern stickman_pattern = {
	"stickman",
	0,
	stickman_pattern_match,
	NULL,
	NULL,
	{0,0,0,0}
	};
	
int
stickman_pattern_match(ascigram_pattern_p pattern, char c, int x, int y, int m)
{
	switch(pattern->state) {
	case 0: do {
		pattern->state = 1;
		return ascigram_pattern_expect(pattern, "o");
	case 1:
		;
	    } while(0);
	}
	
	return -1;
}

void stickman_pattern_register()
{
	ascigram_patterns_register(&stickman_pattern);
}