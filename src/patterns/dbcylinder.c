#include "../pattern.h"

#ifdef _WITH_DBCYLINDER

int dbcylinder_pattern_match(ascigram_pattern_p, ascigram_cell*);
void dbcylinder_pattern_export(ascigram_pattern_p, ascigram_buffer*);

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
	dbcylinder_pattern_export,
	sizeof(dbcylinder_opaque)};

/* IMPLEMENTATIOPN */

int
dbcylinder_pattern_match(ascigram_pattern_p pat, ascigram_cell* cell_p)
{
	ccrBegin(dbcylinder);

	/* Top */
	opq->t = cell_p->attr.y;
	opq->l = cell_p->attr.x;

	ccrReturn(ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED|M_BOX_START_S|M_BOX_START_E));

	do {
		ccrReturn(ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED|M_BOX_START_S));
	} while (cell_p->ch != '.');

	opq->w = cell_p->attr.x - opq->l + 1;
	ccrReturn(ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED|M_BOX_START_S));

	ccrReturn(M_BOX_AFTER_E);
	
	while (ascigram_pattern_await(pat, cell_p, opq->l, opq->t + 1, &meta)) {
		ccrReturn(meta);
	}
	
	ccrReturn(ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|
		M_BOX_START_E));

	while (ascigram_pattern_await(pat, cell_p, opq->l + opq->w - 1, pat->curr.y, &meta)) {
		if (!meta) ccrReturn(ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED));
		else ccrReturn(meta);
	}

	opq->h = cell_p->attr.y - opq->t + 1;
	ccrReturn(ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_E));

	ccrReturn(M_BOX_AFTER_E);

	/* Body */
	
	do {
		while (ascigram_pattern_await(pat, cell_p, opq->l, opq->t + opq->h, &meta)) {
			ccrReturn(meta);
		}

		if (cell_p->ch == '\'') break;

		ccrReturn(ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED|M_BOX_START_E));
		
		while (ascigram_pattern_await(pat, cell_p, opq->l + opq->w - 1, pat->curr.y, &meta)) {
			ccrReturn(meta);
		}

		opq->h++;
		ccrReturn(ascigram_pattern_expect(pat, cell_p, "|", M_OCCUPIED));

		ccrReturn(M_BOX_AFTER_E);
	} while (1);

	/* Bottom */

	ccrReturn(ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_E));

	while (ascigram_pattern_await(pat, cell_p, opq->l + opq->w - 1, pat->curr.y, &meta))
	{
		if (!meta) ccrReturn(ascigram_pattern_expect(pat, cell_p, "-", M_OCCUPIED));
		else ccrReturn(meta);
	}

	opq->h++;
	ccrReturn( ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|P_ACCEPT) );

	ccrReturn(M_BOX_AFTER_E);
	
	/* Extras */
	while(ascigram_pattern_await(pat, cell_p, opq->l, opq->t + opq->h, &meta))
	{
		ccrReturn(meta);
	}

	while(ascigram_pattern_await(pat, cell_p, opq->l + opq->w - 1, opq->t + opq->h, &meta))
	{
		if (!meta) ccrReturn(M_BOX_AFTER_S);
		else ccrReturn(meta);
	}
	
	ccrReturn(M_BOX_AFTER_S | P_FINISH);

	ccrEnd();
}

void
dbcylinder_pattern_export(ascigram_pattern_p pat, ascigram_buffer *ob)
{
	dbcylinder_opaque *opq = (dbcylinder_opaque*)pat->opaque;
	ascigram_buffer_printf(ob, ",\"t\":%d", opq->t);
	ascigram_buffer_printf(ob, ",\"l\":%d", opq->l);
	ascigram_buffer_printf(ob, ",\"w\":%d", opq->w);
	ascigram_buffer_printf(ob, ",\"h\":%d", opq->h);
}

void dbcylinder_pattern_register()
{
	ascigram_patterns_register(&dbcylinder_pattern_factory);
}

#else

void dbcylinder_pattern_register()
{
}

#endif
