#include "../pattern.h"

#ifdef _WITH_DIAMONDBOX

uint32_t diamondbox_pattern_match(ascigram_pattern_p, ascigram_cell*);
void diamondbox_pattern_export(ascigram_pattern_p, ascigram_buffer*);

typedef struct diamondbox_opaque {
	uint16_t t;
	uint16_t c;
	uint16_t l;
	uint16_t w;
	uint16_t h;
	uint16_t cnt;
	uint8_t apeak:1;
} diamondbox_opaque;

static
ascigram_factory diamondbox_pattern_factory = {
	"diamondbox",
	diamondbox_pattern_match,
	diamondbox_pattern_export,
	sizeof(diamondbox_opaque)};

/* IMPLEMENTATION */

uint32_t
diamondbox_pattern_match(ascigram_pattern_p pat, ascigram_cell* cell_p)
{
	ccrBegin(diamondbox)

	/* Top peak */
	opq->t = cell_p->attr.y;
	opq->c = cell_p->attr.x;

	ccrReturn(ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED|M_BOX_START_E|M_BOX_START_S));

	opq->apeak = cell_p->ch == '\'';
	if (opq->apeak) {
		opq->c = cell_p->attr.x;
		ccrReturn(ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_S));
		ccrReturn(ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED|M_BOX_START_S));
	}
    ccrReturn(M_BOX_AFTER_E);

	/* Top Slope */
	opq->cnt = 1;
	do {
        while (ascigram_pattern_await(pat, cell_p, opq->c - opq->cnt*2 - opq->apeak, opq->t + opq->cnt, &meta)) {
			if (meta) { skip = 1; break; }
			ccrReturn(meta);
		}
		if (skip) { skip = 0; break; }

        meta = ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED|M_BOX_START_E|M_BOX_START_S);
		if (meta & E_FAIL) { ccrReturn(M_NONE);	break; }
		ccrReturn(meta);

        ccrReturn(ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_S));

        while (ascigram_pattern_await(pat, cell_p, opq->c + (opq->cnt-1)*2 +1+ opq->apeak, opq->t + opq->cnt, &meta)) {
			ccrReturn(meta);
		}

        ccrReturn(ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_S));
        ccrReturn(ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED|M_BOX_START_S));

		ccrReturn(M_BOX_AFTER_E);
		opq->cnt += 1;
	} while (1);

	/* Middle */
	while (ascigram_pattern_await(pat, cell_p, opq->c - opq->cnt*2 - opq->apeak+1, opq->t + opq->cnt, &meta)) {
		ccrReturn(meta);
	}

    opq->w = 4*(opq->cnt-1) + 3 + 2*opq->apeak;
	opq->l = cell_p->attr.x;

    ccrReturn(ascigram_pattern_expect(pat, cell_p, "<", M_OCCUPIED|M_BOX_START_S|M_BOX_START_E));
	while (ascigram_pattern_await(pat, cell_p, opq->c + (opq->cnt-1)*2 +1+ opq->apeak, opq->t + opq->cnt, &meta)) {
		ccrReturn(meta);
	}
    ccrReturn(ascigram_pattern_expect(pat, cell_p, ">", M_OCCUPIED|M_BOX_START_S));
	ccrReturn(M_BOX_AFTER_E);

	/* Bottom Slope */
	opq->h = opq->cnt;
	for (opq->cnt=1; opq->cnt < opq->h; opq->cnt++) {
		if (opq->cnt > 1) {
			while (ascigram_pattern_await(pat, cell_p, opq->c - (opq->h-opq->cnt)*2 - opq->apeak -2, opq->t + opq->h+opq->cnt, &meta)) {
				ccrReturn(meta);
			}
			ccrReturn(M_BOX_AFTER_S);
		}

		while (ascigram_pattern_await(pat, cell_p, opq->c - (opq->h-opq->cnt)*2 - opq->apeak -1, opq->t + opq->h+opq->cnt, &meta)) {
			ccrReturn(meta);
		}
		ccrReturn(M_BOX_AFTER_S);

		while (ascigram_pattern_await(pat, cell_p, opq->c - (opq->h-opq->cnt)*2 - opq->apeak, opq->t + opq->h+opq->cnt, &meta)) {
			ccrReturn(meta);
		}
        ccrReturn(ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_E));
        ccrReturn(ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED));

		while (ascigram_pattern_await(pat, cell_p, opq->c + (opq->h-opq->cnt-1)*2 +1+ opq->apeak, opq->t + opq->h+opq->cnt, &meta)) {
			ccrReturn(meta);
		}
        ccrReturn(ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED));
        ccrReturn(ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED));

		while (ascigram_pattern_await(pat, cell_p, opq->c + (opq->h-opq->cnt-1)*2 +1+ opq->apeak +2, opq->t + opq->h+opq->cnt, &meta)) {
			ccrReturn(meta);
		}
		ccrReturn(M_BOX_AFTER_E|M_BOX_AFTER_S);

		if (opq->cnt > 1) {
			while (ascigram_pattern_await(pat, cell_p, opq->c + (opq->h-opq->cnt-1)*2 +1+ opq->apeak +3, opq->t + opq->h+opq->cnt, &meta)) {
				if (meta) { skip=1; break; }
				ccrReturn(meta);
			}
			if (!skip) {
				ccrReturn(M_BOX_AFTER_S);
			}
		}
	}
	opq->cnt = opq->h;

	/* Bottom Peak */
	if (opq->cnt > 1) {
		while (ascigram_pattern_await(pat, cell_p, opq->c - opq->apeak -2, opq->t + opq->h *2, &meta)) {
			ccrReturn(meta);
		}
		ccrReturn(M_BOX_AFTER_S);
	}
	while (ascigram_pattern_await(pat, cell_p, opq->c - opq->apeak -1, opq->t + opq->h *2, &meta)) {
		ccrReturn(meta);
	}
	ccrReturn(M_BOX_AFTER_S);

	while (ascigram_pattern_await(pat, cell_p, opq->c - opq->apeak, opq->t + opq->h *2, &meta)) {
		ccrReturn(meta);
	}

	if (opq->apeak) {
		ccrReturn(ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_E));
        ccrReturn(ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED));

		opq->h *= 2;
        ccrReturn(ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|P_ACCEPT));
	} else {
		opq->h *= 2;
		ccrReturn(ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_E|P_ACCEPT));
	}

	while (ascigram_pattern_await(pat, cell_p, opq->c + opq->apeak +1, opq->t + opq->h, &meta)) {
		ccrReturn(meta);
	}
	ccrReturn(M_BOX_AFTER_E|M_BOX_AFTER_S);

	if (opq->cnt > 1) {
		while (ascigram_pattern_await(pat, cell_p, opq->c + opq->apeak +2, opq->t + opq->h, &meta)) {
			if (meta) { skip=1; break; }
			ccrReturn(meta);
		}
		if (!skip) {
			ccrReturn(M_BOX_AFTER_S);
		}
	}

	/* Extra */
	if (opq->apeak) {
		for(opq->cnt=0; opq->cnt<2; opq->cnt++) {
			while (ascigram_pattern_await(pat, cell_p, opq->c - 1+opq->cnt, opq->t + opq->h +1, &meta)) {
				ccrReturn(meta);
			}
			ccrReturn(M_BOX_AFTER_S);
		}

		while (ascigram_pattern_await(pat, cell_p, opq->c - 1+opq->cnt, opq->t + opq->h +1, &meta)) {
			ccrReturn(meta);
		}
		ccrReturn(M_BOX_AFTER_S|P_FINISH);
	} else {
		while (ascigram_pattern_await(pat, cell_p, opq->c, opq->t + opq->h +1, &meta)) {
			ccrReturn(meta);
		}
		ccrReturn(M_BOX_AFTER_S|P_FINISH);
	}

	ccrEnd();
}

void
diamondbox_pattern_export(ascigram_pattern_p pat, ascigram_buffer *ob)
{
	diamondbox_opaque *opq = (diamondbox_opaque*)pat->opaque;
	ascigram_buffer_printf(ob, ",\"t\":%d", opq->t);
	ascigram_buffer_printf(ob, ",\"c\":%d", opq->c);
	ascigram_buffer_printf(ob, ",\"l\":%d", opq->l);
	ascigram_buffer_printf(ob, ",\"w\":%d", opq->w);
	ascigram_buffer_printf(ob, ",\"h\":%d", opq->h);
}

void diamondbox_pattern_register()
{
	ascigram_patterns_register(&diamondbox_pattern_factory);
}

#else

void diamondbox_pattern_register()
{
}

#endif
