#include "../pattern.h"

int stickman_pattern_match(ascigram_pattern_p, ascigram_cell*);
void stickman_pattern_export(ascigram_pattern_p, ascigram_buffer*);

typedef struct stickman_opaque {
	uint16_t x;
	uint16_t y;
} stickman_opaque;

static
ascigram_factory stickman_pattern_factory = {
	"stickman",
	stickman_pattern_match,
	stickman_pattern_export,
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
stickman_pattern_export(ascigram_pattern_p pat, ascigram_buffer *ob)
{
	stickman_opaque *opq = (stickman_opaque*)pat->opaque;
	ascigram_buffer_printf(ob, ",\"x\":%d", opq->x);
	ascigram_buffer_printf(ob, ",\"y\":%d", opq->y);
}

