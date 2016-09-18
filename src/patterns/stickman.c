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
	ccrBegin(stickman);

	opq->x = cell_p->attr.x;
	opq->y = cell_p->attr.y;

	ccrReturn(ascigram_pattern_expect(pat, cell_p, "O", M_OCCUPIED));
	
    while (ascigram_pattern_await(pat, cell_p, pat->curr.x-1, pat->curr.y+1, &meta)) {
      ccrReturn(meta);
    }
    
	ccrReturn(ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED));
    ccrReturn(ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED));
    ccrReturn(ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED));
	
	while (ascigram_pattern_await(pat, cell_p, pat->curr.x-2, pat->curr.y+1, &meta)) {
        ccrReturn(meta);
    }
    
	ccrReturn(ascigram_pattern_expect(pat, cell_p, "/", M_OCCUPIED));
    ccrReturn(ascigram_pattern_expect(pat, cell_p, " ", M_OCCUPIED));
    ccrReturn(ascigram_pattern_expect(pat, cell_p, "\\", M_OCCUPIED|P_FINISH));
    
    ccrEnd();
}

void
stickman_pattern_dump(ascigram_pattern_p pat)
{
	stickman_opaque *opaque = (stickman_opaque*)pat->opaque;
	fprintf(stdout, "\t- x: %d\n", opaque->x);
	fprintf(stdout, "\t- y: %d\n", opaque->y);
}

