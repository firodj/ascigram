#include "../pattern.h"

#ifdef _WITH_DOCUMENTBOX

uint32_t documentbox_pattern_match(ascigram_pattern_p, ascigram_cell*);
void documentbox_pattern_export(ascigram_pattern_p, ascigram_buffer*);

typedef struct documentbox_opaque {
	uint16_t t;
	uint16_t l;
	uint16_t w;
	uint16_t h;
	uint8_t fold, wave;
} documentbox_opaque;

static
ascigram_factory documentbox_pattern_factory = {
	"documentbox",
	documentbox_pattern_match,
	documentbox_pattern_export,
	sizeof(documentbox_opaque)};

/* IMPLEMENTATION */

uint32_t
documentbox_pattern_match(ascigram_pattern_p pat, ascigram_cell* cell_p)
{
	ccrBegin(documentbox);

	/* Top part with(out) fold */

	opq->t = cell_p->attr.y;
	opq->l = cell_p->attr.x;

	ccrReturn(ascigram_pattern_expect(pat, cell_p, "+", M_OCCUPIED|M_BOX_START_S|M_BOX_START_E));

	do {
		ccrReturn(ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED|M_BOX_START_S));
	} while (cell_p->ch == '-');


	opq->h = 1;
	if (cell_p->ch == '+') {
		opq->fold = 0;
		opq->w = cell_p->attr.x - opq->l + 1;
		ccrReturn(ascigram_pattern_expect(pat, cell_p, "+", M_OCCUPIED|M_BOX_START_S));
	} else  {
		opq->fold = 1;
		opq->w = cell_p->attr.x - opq->l + 1 + 2;	
		ccrReturn(ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED|M_BOX_START_S));
	}
	
	ccrReturn(M_BOX_AFTER_E);

	while (ascigram_pattern_await(pat, cell_p, opq->l, opq->t + opq->h, &meta)) {
		ccrReturn(meta);
	}

	/* Fold */

	if (opq->fold) {
		ccrReturn(ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED|M_BOX_START_E));

		while (ascigram_pattern_await(pat, cell_p, opq->l + opq->w-4, opq->t + opq->h, &meta)) {
			ccrReturn(meta);
		}

		ccrReturn(ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED));
		ccrReturn(ascigram_pattern_expect(pat, cell_p, "_", M_OCCUPIED));
		
		opq->h++;
		ccrReturn(ascigram_pattern_expect(pat, cell_p, "\\", M_OCCUPIED|M_BOX_START_S));
		
		ccrReturn(M_BOX_AFTER_E);


		while (ascigram_pattern_await(pat, cell_p, opq->l, opq->t + opq->h, &meta)) {
			ccrReturn(meta);
		}
	}


	/* Body */

	do {
		ccrReturn(ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED|M_BOX_START_E));

		while (ascigram_pattern_await(pat, cell_p, opq->l + opq->w - 1, pat->curr.y, &meta)) {
			ccrReturn(meta);
		}
	
		opq->h++;
		ccrReturn(ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED));

		ccrReturn(M_BOX_AFTER_E);

		while (ascigram_pattern_await(pat, cell_p, opq->l, opq->t + opq->h, &meta)) {
			ccrReturn(meta);
		}
	} while (cell_p->ch == '|');

	/* Bottom-Wave: */
	if (cell_p->ch == '\'' && !opq->fold) {
		opq->wave = 1;
		ccrReturn(ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_E));

		while (ascigram_pattern_await(pat, cell_p, opq->l + opq->w - 2, pat->curr.y, &meta)) {
			if (meta & E_FAIL) ccrReturn(meta);
			else ccrReturn(ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED));

			while (ascigram_pattern_await(pat, cell_p, opq->l + opq->w - 4, pat->curr.y, &meta)) {
				if (meta & E_FAIL) ccrReturn(meta);
				else ccrReturn(ascigram_pattern_expect(pat, cell_p, "_", M_OCCUPIED));

				if (cell_p->ch == '.') break;
			}
			ccrReturn(ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED));
			while (ascigram_pattern_await(pat, cell_p, opq->l + opq->w - 2, pat->curr.y, &meta)) {
				if (meta & E_FAIL) ccrReturn(meta);
				else ccrReturn(ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED));

				if (cell_p->ch == '.') break;
			}
		}

		ccrReturn(ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED));

		opq->h++;
		ccrReturn(ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED | P_ACCEPT));

		ccrReturn(M_BOX_AFTER_E);
	}

	/* Bottom-Flat */   
	else {
		opq->wave = 0;

		ccrReturn(ascigram_pattern_expect(pat, cell_p, "+", M_OCCUPIED|M_BOX_START_E));

		while (ascigram_pattern_await(pat, cell_p, opq->l + opq->w - 1, pat->curr.y, &meta)) {
			if (meta & E_FAIL) ccrReturn(meta);
			else ccrReturn(ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED));
		}
		
		opq->h++;
		ccrReturn(ascigram_pattern_expect(pat, cell_p, "+", M_OCCUPIED|P_ACCEPT));

		ccrReturn(M_BOX_AFTER_E);
	}
	
	/* Extras */

	while (ascigram_pattern_await(pat, cell_p, opq->l, opq->t + opq->h, &meta)) {
		ccrReturn(meta);
	}

	while (ascigram_pattern_await(pat, cell_p, opq->l + opq->w - 1, opq->t + opq->h, &meta)) {
		if (meta & E_FAIL) ccrReturn(meta);
		else ccrReturn(M_BOX_AFTER_S);
	}
	
	ccrReturn(M_BOX_AFTER_S | P_FINISH);

	ccrEnd();
}

void
documentbox_pattern_export(ascigram_pattern_p pat, ascigram_buffer *ob)
{
	documentbox_opaque *opq = (documentbox_opaque*)pat->opaque;
	ascigram_buffer_printf(ob, ",\"t\":%d", opq->t);
	ascigram_buffer_printf(ob, ",\"l\":%d", opq->l);
	ascigram_buffer_printf(ob, ",\"w\":%d", opq->w);
	ascigram_buffer_printf(ob, ",\"h\":%d", opq->h);
}

void documentbox_pattern_register()
{
	ascigram_patterns_register(&documentbox_pattern_factory);
}

#else

void documentbox_pattern_register()
{
}

#endif
