#include "../pattern.h"

int documentbox_pattern_match(ascigram_pattern_p, ascigram_cell*);
void documentbox_pattern_dump(ascigram_pattern_p pat);

typedef struct documentbox_opaque {
	uint16_t t;
	uint16_t l;
	uint16_t w;
	uint16_t h;
} documentbox_opaque;

static
ascigram_factory documentbox_pattern_factory = {
	"documentbox",
	documentbox_pattern_match,
	NULL,
	NULL,
	documentbox_pattern_dump,
	sizeof(documentbox_opaque)};

void documentbox_pattern_register()
{
	ascigram_patterns_register(&documentbox_pattern_factory);
}

/* IMPLEMENTATIOPN */

int
documentbox_pattern_match(ascigram_pattern_p pat, ascigram_cell* cell_p)
{
	int meta;
	documentbox_opaque *opaque = (documentbox_opaque*)pat->opaque;
	switch(pat->state) {
	case 0: do {
		meta = ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED|M_BOX_START_S|M_BOX_START_E);
		if (meta & M_OCCUPIED) {
			opaque->t = cell_p->attr.y;
			opaque->l = cell_p->attr.x;
		}
		return meta;
	case 1:
		meta = ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED|M_BOX_START_S);
		return meta;
	case 2:
		meta = ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED|M_BOX_START_S);
		if (meta & M_OCCUPIED) {
			opaque->w = cell_p->attr.x - opaque->l + 1;
		} else {
			meta = ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED|M_BOX_START_S);
			if (meta & M_OCCUPIED) {
				pat->state--;
			}
		}
		return meta;
	case 3:
		return M_BOX_AFTER_E;
	case 4:		
		meta = ascigram_pattern_await(pat, cell_p, opaque->l, opaque->t + 1);
		if (meta & M_OCCUPIED) {
			meta = ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_E);
        }
		return meta;
	case 5:
		meta = ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED);
		if (meta & M_OCCUPIED) {
			if ((cell_p->attr.x - opaque->l) < (opaque->w - 2)) {
				pat->state--;
			}
		}
		return meta;
	case 6:		
		meta = ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_E);
		if (meta & M_OCCUPIED) {
			opaque->h = cell_p->attr.y - opaque->t + 1;
		}
		return meta;
	case 7:		
		return M_BOX_AFTER_E;
	case 8:
		meta = ascigram_pattern_await(pat, cell_p, opaque->l, opaque->t + opaque->h);
		if (meta & M_OCCUPIED) {
			meta = ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_E);
			if (meta & M_OCCUPIED) {
				if (opaque->h > 2) {
					pat->state = 9;
				} else {
					meta = P_REJECT;
				}
			} else {
				meta = ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED|M_BOX_START_E);
			}
		}
		return meta;
	case 9:
		meta = ascigram_pattern_await(pat, cell_p, opaque->l + opaque->w - 1, pat->curr.y);
		if (meta & M_OCCUPIED) {
			meta = ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED);
			if (meta & M_OCCUPIED) {
				opaque->h++;
				pat->state = 6;
			}
		}
		return meta;
	case 10:
		meta = ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED);
		if (meta & M_OCCUPIED) {
			if ((cell_p->attr.x - opaque->l) < (opaque->w - 2)) {
				pat->state--;
			}
		}
		return meta;
	case 11:
		meta = ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED);
		if (meta & M_OCCUPIED) {
			opaque->h++;
			pat->finish = P_ACCEPT;
		}
		return meta;
	case 12:
		return M_BOX_AFTER_E;
	case 13:
		meta = ascigram_pattern_await(pat, cell_p, opaque->l, opaque->t + opaque->h);
		if (meta & M_OCCUPIED) {
			meta = M_BOX_AFTER_S;
		}
		return meta;
	case 14:
		meta = M_BOX_AFTER_S;
		if ((cell_p->attr.x - opaque->l) >= opaque->w-1) {
			pat->finish = P_FINISH;
		} else {
			pat->state--;
		}
		return meta;
		} while(0);
	}

	return P_REJECT;
}

void
documentbox_pattern_dump(ascigram_pattern_p pat)
{
	documentbox_opaque *opaque = (documentbox_opaque*)pat->opaque;
	fprintf(stdout, "\t- t: %d\n", opaque->t);
	fprintf(stdout, "\t- l: %d\n", opaque->l);
	fprintf(stdout, "\t- w: %d\n", opaque->w);
	fprintf(stdout, "\t- h: %d\n", opaque->h);
}
