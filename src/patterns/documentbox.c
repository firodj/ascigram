#include "../pattern.h"

int documentbox_pattern_match(ascigram_pattern_p, ascigram_cell*);
void documentbox_pattern_dump(ascigram_pattern_p pat);

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
	/* Top part with(out) fold */
	case 0: do {
		opaque->t = cell_p->attr.y;
		opaque->l = cell_p->attr.x;

		return ascigram_pattern_expect(pat, cell_p, "+", M_OCCUPIED|M_BOX_START_S|M_BOX_START_E);
	case 1:
		return ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED|M_BOX_START_S);
	case 2:
		opaque->h = 1;
		meta = ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED|M_BOX_START_S);
		if (meta & M_OCCUPIED) {
			opaque->fold = 1;
			opaque->w = cell_p->attr.x - opaque->l + 1 + 2;
			return meta;
		}

		meta = ascigram_pattern_expect(pat, cell_p, "+", M_OCCUPIED|M_BOX_START_S);
		if (meta & M_OCCUPIED) {
			opaque->fold = 0;
			opaque->w = cell_p->attr.x - opaque->l + 1;
			return meta;
		}

		pat->state--;
		return ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED|M_BOX_START_S);
	case 3:
		if (!opaque->fold) pat->state = 11 -1; /* To: Body */
		return M_BOX_AFTER_E;

	/* Fold */
	case 4:
		meta = ascigram_pattern_await(pat, cell_p, opaque->l, opaque->t + opaque->h);
		if (meta & M_OCCUPIED) pat->state++; else return meta;
	case 5:
		return ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED|M_BOX_START_E);
	case 6:
		meta = ascigram_pattern_await(pat, cell_p, opaque->l + opaque->w-4, opaque->t + opaque->h);
		if (meta & M_OCCUPIED) pat->state++; else return meta;
	case 7:
		return ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED);
	case 8:
		return ascigram_pattern_expect(pat, cell_p, "_", M_OCCUPIED);
	case 9:
		opaque->h = cell_p->attr.y - opaque->t + 1;
		return ascigram_pattern_expect(pat, cell_p, "\\", M_OCCUPIED|M_BOX_START_S);
	case 10:
		return M_BOX_AFTER_E;

	/* Body */
	case 11:
		meta = ascigram_pattern_await(pat, cell_p, opaque->l, opaque->t + opaque->h);
		if (meta & M_OCCUPIED) pat->state++; else return meta;
	case 12:
		meta = ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED|M_BOX_START_E);
		if (meta & M_OCCUPIED) return meta;

		if (opaque->h < opaque->fold + 2) return P_REJECT;
		
		pat->state = 16 -1; /* To: Bottom-Wave */
		opaque->wave = 1;
		meta = ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_E);
		if ((meta & M_OCCUPIED) || !opaque->fold) return meta;
		
		pat->state = 21 -1; /* To: Bottom-Flat */
		return ascigram_pattern_expect(pat, cell_p, "+", M_OCCUPIED|M_BOX_START_E);
	case 13:
		meta = ascigram_pattern_await(pat, cell_p, opaque->l + opaque->w - 1, pat->curr.y);
		if (meta & M_OCCUPIED) pat->state++; else return meta;
	case 14:
		opaque->h++;
		return ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED);
	case 15:
		pat->state = 11 -1; /* To: Body */
		return M_BOX_AFTER_E;

	/* Bottom-Wave */
	case 16:
		return ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED);
	
	/* Wave */
	case 17:
		meta = ascigram_pattern_await(pat, cell_p, opaque->l + opaque->w - 1, opaque->t + opaque->h);
		if (meta & E_FAIL) return meta;
		if (meta & M_OCCUPIED) {
			opaque->h++;
			pat->state = 23 -1; /* To: Corner */
			return ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED | P_ACCEPT);
		}

		return ascigram_pattern_expect(pat, cell_p, "_", M_OCCUPIED);
	case 18:
		meta = ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED);
		if (meta & M_OCCUPIED) return meta;
		
		pat->state--;
		return ascigram_pattern_expect(pat, cell_p, "_", M_OCCUPIED);
	case 19:
		return ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED);
	case 20:
		meta = ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED);
		if (meta & M_OCCUPIED) {
			pat->state = 17 -1; /* To: Wave */
			return meta;
		}
		
		pat->state--;
		return ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED);

	/* Bottom-Flat */
	case 21:
		meta = ascigram_pattern_await(pat, cell_p, opaque->l + opaque->w - 2, opaque->t + opaque->h);
		if (!(meta & M_OCCUPIED)) pat->state--;
		return ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED);
	case 22:
		opaque->h++;
		return ascigram_pattern_expect(pat, cell_p, "+", M_OCCUPIED|P_ACCEPT);

	/* Corner */
	case 23:
		return M_BOX_AFTER_E;

	/* Extras */
	case 24:
		meta = ascigram_pattern_await(pat, cell_p, opaque->l, opaque->t + opaque->h);
		if (meta & M_OCCUPIED) pat->state++; else return meta;
	case 25:
		meta = ascigram_pattern_await(pat, cell_p, opaque->l + opaque->w - 1, opaque->t + opaque->h);
		if (meta & M_OCCUPIED) return M_BOX_AFTER_S | P_FINISH;
		pat->state--;
		return M_BOX_AFTER_S;
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
