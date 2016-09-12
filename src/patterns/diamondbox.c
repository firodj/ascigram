#include "../pattern.h"

uint32_t diamondbox_pattern_match(ascigram_pattern_p, ascigram_cell*);
void diamondbox_pattern_dump(ascigram_pattern_p pat);

typedef struct diamondbox_opaque {
	uint16_t t;
	uint16_t l;
	uint16_t w;
	uint16_t h;
	uint8_t fold, wave;
} diamondbox_opaque;

static
ascigram_factory diamondbox_pattern_factory = {
	"diamondbox",
	diamondbox_pattern_match,
	NULL,
	NULL,
	diamondbox_pattern_dump,
	sizeof(diamondbox_opaque)};

void diamondbox_pattern_register()
{
	ascigram_patterns_register(&diamondbox_pattern_factory);
}

/* IMPLEMENTATION */

uint32_t
diamondbox_pattern_match(ascigram_pattern_p pat, ascigram_cell* cell_p)
{
	int meta;
	documentbox_opaque *opaque = (documentbox_opaque*)pat->opaque;
	switch(pat->state) {
	/* Top part */
	case 0: do {
		opaque->t = cell_p->attr.y;
		opaque->l = cell_p->attr.x;

		return ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED|M_BOX_START_E|M_BOX_START_S);
	case 1:
	    opaque->h = 1;
	    opaque->cnt = 1;
	    meta = ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_S);
        if (meta & M_OCCUPIED) {
            pat->apeak = 1;
            pat->state = 2 -1; /* To: Apeak */
            
            opaque->t = cell_p->attr.y;
		    opaque->l = cell_p->attr.x;
            return meta;
        }

        pat->state = 4 -1; /* To: Body */
        return M_BOX_AFTER_E;
    /* Apeak */
    case 2:
        return ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED|M_BOX_START_S);
    case 3:
        return M_BOX_AFTER_E;
    /* Top Slope */
    case 4:
        meta = ascigram_pattern_await(pat, cell_p, opaque->l - opaque->cnt*2 - opaque->apeak, opaque->t + opaque->cnt);
        if (meta & M_OCCUPIED) pat->state++; else return meta;
    case 5:
        meta = ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED|M_BOX_START_E|M_BOX_START_S);
        if (meta & E_FAIL) {
            pat->state = -1; /* To: Middle */
            return M_NONE;
        }
        return meta;
    case 6:
        return ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_S);
    case 7:
        meta = ascigram_pattern_await(pat, cell_p, opaque->l + (opaque->cnt-1)*2 +1 + opaque->apeak, opaque->t + opaque->cnt);
        if (meta & M_OCCUPIED) pat->state++; else return meta;
    case 8:
        return ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_S);
    case 9:
        opaque->cnt++;
        return ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED|M_BOX_START_S);
    case 10:
        pat->state = 4 -1; /* To: Top Slope */
        return M_BOX_AFTER_E;
    /* Middle */
    case 11:
        meta = ascigram_pattern_await(pat, cell_p, opaque->l - opaque->cnt*2 - opaque->apeak+1, opaque->t + opaque->cnt);
        if (meta & M_OCCUPIED) pat->state++; else return meta;
    case 12:
        opaque->w = 4*(opaque->cnt-1) +3 +2*opaque->apeak;
        return ascigram_pattern_expect(pat, cell_p, "<", M_OCCUPIED|M_BOX_START_S|M_BOX_START_E);
    case 13:
        meta = ascigram_pattern_await(pat, cell_p, opaque->l + (opaque->cnt-1)*2 +1 + opaque->apeak+1, opaque->t + opaque->cnt);
        if (meta & M_OCCUPIED) pat->state++; else return meta;
    case 14:
        opaque->w = 4*(opaque->cnt-1) +3 +2*opaque->apeak;
        return ascigram_pattern_expect(pat, cell_p, ">", M_OCCUPIED|M_BOX_START_S|M_BOX_START_E);
    case 15:
        opaque->sz = opaque->cnt;
        opaque->cnt = 1;
        return M_BOX_AFTER_E;
    /* Bottom Slope */
    case 16:
        if (opaque->cnt > 1) {
            meta = ascigram_pattern_await(pat, cell_p, opaque->l - (opaque->sz-opaque->cnt)*2 -opaque->apeak -2, opaque->t + opaque->cnt + opaque->sz);
            if (!(meta & M_OCCUPIED)) return meta;
        }
        pat->state++;
    case 17:
        if (opaque->cnt > 1) {
            return M_BOX_AFTER_S;
        }
        pat->state++;
    case 18:
        meta = ascigram_pattern_await(pat, cell_p, 
            opaque->l - (opaque->sz-opaque->cnt)*2 -opaque->apeak -1,
            opaque->t + opaque->cnt + opaque->sz);
        if (meta & M_OCCUPIED) pat->state++; else return meta;
    case 19:     
        return M_BOX_AFTER_S;
    case 20:
        meta = ascigram_pattern_await(pat, cell_p, 
            opaque->l - (opaque->sz-opaque->cnt)*2 -opaque->apeak,
            opaque->t + opaque->cnt + opaque->sz);
        if (meta & M_OCCUPIED) pat->state++; else return meta;
    case 21:
        return ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED|M_BOX_START_E);
    case 22:
        return ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED);
    case 23:
        meta = ascigram_pattern_await(pat, cell_p, 
            opaque->l + (opaque->sz-opaque->cnt-1)*2 +opaque->apeak +1,
            opaque->t + opaque->cnt + opaque->sz);
        if (meta & M_OCCUPIED) pat->state++; else return meta;
    case 24:
        return ascigram_pattern_expect(pat, cell_p, ".", M_OCCUPIED);
    case 25:
        return ascigram_pattern_expect(pat, cell_p, "'", M_OCCUPIED);
    /* Bottom Slope */
        ;
        } while(0);
    }
    
    return P_REJECT;
}