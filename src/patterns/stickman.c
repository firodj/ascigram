#include "../buffer.h"
#include "../pattern.h"

int
stickman_pattern_match(ascigram_pattern_p, ascigram_cell*);

static
ascigram_factory stickman_pattern_factory = {
	"stickman",
	stickman_pattern_match,
	NULL,
	NULL};
	
int
stickman_pattern_match(ascigram_pattern_p pat, ascigram_cell* cell_p)
{
	int meta;
	switch(pat->state) {
	case 0: do {
		meta = ascigram_pattern_expect(pat, cell_p, "o", M_OCCUPIED);
		return meta;
	case 1:
		meta = ascigram_pattern_await(pat, cell_p, -1, 1);
		if (meta & M_OCCUPIED) {
			meta = ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED);
        }
        return meta; 
    case 2:
        meta = ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED);
        return meta;
    case 3:
        meta = ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED);
        return meta;
	case 4:
		meta = ascigram_pattern_await(pat, cell_p, -2, 1);
		if (meta & M_OCCUPIED) {
			meta = ascigram_pattern_expect(pat, cell_p, "/", M_OCCUPIED);
		}
		return meta;
	case 5:
        meta = ascigram_pattern_expect(pat, cell_p, " ", M_OCCUPIED);
        return meta;
	case 6:
        meta = ascigram_pattern_expect(pat, cell_p, "\\", P_ACCEPT);
        return meta;
	    } while(0);
	}
	
	return -1;
}

void stickman_pattern_register()
{
	ascigram_patterns_register(&stickman_pattern_factory);
}
