#include "../pattern.h"

#ifdef _WITH_RECTANGULARBOX

uint32_t rectangularbox_pattern_match(ascigram_pattern_p, ascigram_cell*);
void rectangularbox_pattern_export(ascigram_pattern_p, ascigram_buffer*);

typedef struct rectangularbox_opaque {
	uint16_t t;
	uint16_t l;
	uint16_t w;
	uint16_t h;
	uint8_t cner;
} rectangularbox_opaque;

static
ascigram_factory rectangularbox_pattern_factory = {
	"rectangularbox",
	rectangularbox_pattern_match,
	rectangularbox_pattern_export,
	sizeof(rectangularbox_opaque)};

/* IMPLEMENTATION */

uint32_t
rectangularbox_pattern_match(ascigram_pattern_p pat, ascigram_cell* cell_p)
{
	static const char *cnerchars[3][4] = {{"+", "+", "+", "+"},
	{".",".","'","'"},	{"/","\\","\\","/"}};

	ccrBegin(rectangularbox);

	/* Top left corner */

	opq->t = cell_p->attr.y;
	opq->l = cell_p->attr.x;

	for (opq->cner = 0; opq->cner < 3; opq->cner++) {
		meta = ascigram_pattern_expect(pat, cell_p, cnerchars[opq->cner][0], M_OCCUPIED|M_BOX_START_S|M_BOX_START_E);
		if (meta & M_OCCUPIED) break;
	}
	ccrReturn(meta);

	/* Top line */
	ccrReturn(ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED|M_BOX_START_S));
	
	while (cell_p->ch != cnerchars[opq->cner][1][0]) {
		ccrReturn(ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED|M_BOX_START_S));
	}
	opq->w = cell_p->attr.x - opq->l + 1;
			
	/* Top right corner */
	ccrReturn(ascigram_pattern_expect(pat, cell_p, cnerchars[opq->cner][1], M_OCCUPIED|M_BOX_START_S));
	ccrReturn(M_BOX_AFTER_E);

	/* Next line */
	opq->h++;
	while (ascigram_pattern_await(pat, cell_p, opq->l, opq->t + opq->h, &meta)) {
		ccrReturn(meta);
	}

	/* First left side */
	ccrReturn(ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED|M_BOX_START_E));

	/* First right side */
	while (ascigram_pattern_await(pat, cell_p, opq->l + opq->w - 1, opq->t + opq->h, &meta)) {
		ccrReturn(meta);
	}
	ccrReturn(ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED));
	ccrReturn(M_BOX_AFTER_E);

	/* Middle */
	opq->h++;
	while (ascigram_pattern_await(pat, cell_p, opq->l, opq->t + opq->h, &meta)) {
		ccrReturn(meta);
	}

	while (cell_p->ch != cnerchars[opq->cner][2][0]) {
		ccrReturn(ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED|M_BOX_START_E));	
		
		while (ascigram_pattern_await(pat, cell_p, opq->l + opq->w - 1, opq->t + opq->h, &meta)) {
			ccrReturn(meta);
		}

		ccrReturn(ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED));			
		ccrReturn(M_BOX_AFTER_E);

		opq->h++;
		while (ascigram_pattern_await(pat, cell_p, opq->l, opq->t + opq->h, &meta)) {
			ccrReturn(meta);
		}
	}

	/* Bottom left corner */
	ccrReturn(ascigram_pattern_expect(pat, cell_p, cnerchars[opq->cner][2], M_OCCUPIED|M_BOX_START_S));
	
	/* Bottom line */
	while (ascigram_pattern_await(pat, cell_p, opq->l + opq->w - 1, pat->curr.y, &meta)) {
		if (!meta) ccrReturn(ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED));
		else ccrReturn(meta);
	}
	
	/* Bottom right corner */
	ccrReturn(ascigram_pattern_expect(pat, cell_p, cnerchars[opq->cner][3], M_OCCUPIED|P_ACCEPT));
	ccrReturn(M_BOX_AFTER_E);
	
	/* Extra */
	while (ascigram_pattern_await(pat, cell_p, opq->l, opq->t + opq->h + 1, &meta)) {
		ccrReturn(meta);
	}
	while (ascigram_pattern_await(pat, cell_p, opq->l + opq->w - 1, opq->t + opq->h + 1, &meta)) {
		if (!meta) ccrReturn(M_BOX_AFTER_S);
		else ccrReturn(meta);
	}
	ccrReturn(M_BOX_AFTER_S | P_FINISH);

	ccrEnd();
}

void
rectangularbox_pattern_export(ascigram_pattern_p pat, ascigram_buffer *ob)
{
	rectangularbox_opaque *opq = (rectangularbox_opaque*)pat->opaque;
	ascigram_buffer_printf(ob, ",\"t\":%d", opq->t);
	ascigram_buffer_printf(ob, ",\"l\":%d", opq->l);
	ascigram_buffer_printf(ob, ",\"w\":%d", opq->w);
	ascigram_buffer_printf(ob, ",\"h\":%d", opq->h);
}

void rectangularbox_pattern_register()
{
	ascigram_patterns_register(&rectangularbox_pattern_factory);
}

#else

void rectangularbox_pattern_register()
{
}

#endif
