#include "../pattern.h"

int stickman_pattern_match(ascigram_pattern_p, ascigram_cell*);
void stickman_pattern_dump(ascigram_pattern_p pat);

typedef struct stickman_opaque {
	uint16_t x;
	uint16_t y;
} stickman_opaque;

static
ascigram_factory stickman_pattern_factory = {
	"stickman",
	stickman_pattern_match,
	NULL,
	NULL,
	stickman_pattern_dump,
	sizeof(stickman_opaque)};

void stickman_pattern_register()
{
	ascigram_patterns_register(&stickman_pattern_factory);
}

/* IMPLEMENTATION */

int
stickman_pattern_match(ascigram_pattern_p pat, ascigram_cell* cell_p)
{
	int meta;
	stickman_opaque *opaque = (stickman_opaque*)pat->opaque;
	switch(pat->state) {
	case 0: do {
		meta = ascigram_pattern_expect(pat, cell_p, "O", M_OCCUPIED);
		if (meta & M_OCCUPIED) {
			opaque->x = cell_p->attr.x;
			opaque->y = cell_p->attr.y;
		}
		return meta;
	case 1:
		meta = ascigram_pattern_await(pat, cell_p, pat->curr.x-1, pat->curr.y+1);
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
		meta = ascigram_pattern_await(pat, cell_p, pat->curr.x-2, pat->curr.y+1);
		if (meta & M_OCCUPIED) {
			meta = ascigram_pattern_expect(pat, cell_p, "/", M_OCCUPIED);
		}
		return meta;
	case 5:
        meta = ascigram_pattern_expect(pat, cell_p, " ", M_OCCUPIED);
        return meta;
	case 6:
        meta = ascigram_pattern_expect(pat, cell_p, "\\", M_OCCUPIED);
		if (meta & M_OCCUPIED) {
			pat->finish = P_FINISH;
		}
        return meta;
	    } while(0);
	}
	
	return P_REJECT;
}

void
stickman_pattern_dump(ascigram_pattern_p pat)
{
	stickman_opaque *opaque = (stickman_opaque*)pat->opaque;
	fprintf(stdout, "\t- x: %d\n", opaque->x);
	fprintf(stdout, "\t- y: %d\n", opaque->y);
}

