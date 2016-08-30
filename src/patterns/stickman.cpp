#include "stickman.h"
#include "buffer.h"
#include "pattern.h"

int
stickman_pattern_match(ascigram_pattern *pattern, char c, int x, int y, int m);

static const
ascigram_pattern stickman_pattern = {
	"stickman",
	0,
	stickman_pattern_match,
	NULL,
	NULL,
	NULL
	};
	
int
stickman_pattern_match(ascigram_pattern *pattern, char c, int x, int y, int m)
{
	switch(pattern->state) {
	case 0: do {
		pattern->state = 1;
		return ascigram_pattern_expect("o");
	case 1:
	    } while(0);
	}
	
	return -1;
}

void stickman_pattern_register()
{
	ascigram_patterns_register(&stickman_pattern);
}