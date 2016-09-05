#include "../pattern.h"

int dbcylinder_pattern_match(ascigram_pattern_p, ascigram_cell*);
void dbcylinder_pattern_dump(ascigram_pattern_p pat);

typedef struct dbcylinder_opaque {
	uint16_t t;
	uint16_t l;
	uint16_t w;
	uint16_t h;
} dbcylinder_opaque;

static
ascigram_factory dbcylinder_pattern_factory = {
	"dbcylinder",
	dbcylinder_pattern_match,
	NULL,
	NULL,
	dbcylinder_pattern_dump,
	sizeof(dbcylinder_opaque)};

void dbcylinder_pattern_register()
{
	ascigram_patterns_register(&dbcylinder_pattern_factory);
}

/* IMPLEMENTATIOPN */

int
dbcylinder_pattern_match(ascigram_pattern_p pat, ascigram_cell* cell_p)
{
	int meta;
	dbcylinder_opaque *opaque = (dbcylinder_opaque*)pat->opaque;
	switch(pat->state) {
	/* Top */
	case 0: do {
		opaque->t = cell_p->attr.y;
		opaque->l = cell_p->attr.x;

		return ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED|M_BOX_START_S|M_BOX_START_E);
	case 1:
		return ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED|M_BOX_START_S);
	case 2:
		meta = ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED|M_BOX_START_S);
		if (meta & M_OCCUPIED) {pat->state--; return meta;}

		opaque->w = cell_p->attr.x - opaque->l + 1;

		return ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED|M_BOX_START_S);
	case 3:
		return M_BOX_AFTER_E;
	case 4:
		meta = ascigram_pattern_await(pat, cell_p, opaque->l, opaque->t + 1);
		if (meta & M_OCCUPIED) pat->state++; else return meta;
	case 5:
		return ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_E);
	case 6:
		if (cell_p->attr.y != opaque->t + 1) return P_REJECT;
		if ((cell_p->attr.x - opaque->l) < (opaque->w - 2)) pat->state--;

		return ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED);
	case 7:
		opaque->h = cell_p->attr.y - opaque->t + 1;

		return ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_E);
	case 8:
		return M_BOX_AFTER_E;

	/* Body */
	case 9:
		meta = ascigram_pattern_await(pat, cell_p, opaque->l, opaque->t + opaque->h);
		if (meta & M_OCCUPIED) pat->state++; else return meta;
	case 10:
		meta = ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED|M_BOX_START_E);
		if (meta & M_OCCUPIED) return meta;

		if (opaque->h > 2) pat->state = 14 -1; /* Bottom */	else return P_REJECT;
		return ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_E);
	case 11:
		meta = ascigram_pattern_await(pat, cell_p, opaque->l + opaque->w - 1, pat->curr.y);
		if (meta & M_OCCUPIED) pat->state++; else return meta;
	case 12:
		opaque->h++;
		return ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED);
	case 13:
		pat->state = 9 -1; /* To: Body */
		return M_BOX_AFTER_E;

	/* Bottom */
	case 14: 
		if (cell_p->attr.y != (opaque->t + opaque->h)) return P_REJECT;
		
		if ((cell_p->attr.x - opaque->l) < (opaque->w - 2)) pat->state--;

		return ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED);
	case 15:
		opaque->h++;
		return ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|P_ACCEPT);
	case 16:
		return M_BOX_AFTER_E;
	
	/* Extras */
	case 17:
		meta = ascigram_pattern_await(pat, cell_p, opaque->l, opaque->t + opaque->h);
		if (meta & M_OCCUPIED) pat->state++; else return meta;
	case 18:
		if (cell_p->attr.y != (opaque->t + opaque->h)) return P_REJECT;

		meta = M_BOX_AFTER_S;
		if ((cell_p->attr.x - opaque->l) >= opaque->w-1)
			meta |= P_FINISH;
		else
			pat->state--;
		return meta;
		} while(0);
	}

	return P_REJECT;
}

void
dbcylinder_pattern_dump(ascigram_pattern_p pat)
{
	dbcylinder_opaque *opaque = (dbcylinder_opaque*)pat->opaque;
	fprintf(stdout, "\t- t: %d\n", opaque->t);
	fprintf(stdout, "\t- l: %d\n", opaque->l);
	fprintf(stdout, "\t- w: %d\n", opaque->w);
	fprintf(stdout, "\t- h: %d\n", opaque->h);
}
