
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "document.h"
#include "pattern.h"
#include "stack.h"

#ifndef _MSC_VER
#include <strings.h>
#else
#define strncasecmp	_strnicmp
#endif


/************
 * INTERNAL *
 ************/

void
scan_rows(ascigram_document *doc, const uint8_t *data, size_t size)
{
	size_t end = 0;
	int y = 0, x;

	while (end < size) {
		ascigram_row row, *prow;

		row.y = y;
		row.width = 0;
		ascigram_stack_init(&row.cells, sizeof(ascigram_cell));

		prow = ascigram_stack_push(&doc->rows, &row);
		
		x = 0;
		while (end < size && data[end] != '\n') {
			ascigram_cell cell, *pcell;
			cell.ch = data[end];
			cell.attr.meta = 0;
			cell.attr.y = prow->y;
			cell.attr.x = x;
			prow->width = x;
			ascigram_stack_init(&cell.pattern_refs, sizeof(ascigram_pattern_p));

			pcell = ascigram_stack_push(&prow->cells, &cell);

			end++;
			x++;
		}

		if (data[end] == '\n') end++;
		y++;
	}
}

void
free_rows(ascigram_document *doc)
{
	int irow, icell;
	ascigram_row *prow;
	ascigram_cell *pcell;

	irow = 0;
	while(prow = ascigram_stack_iter(&doc->rows, &irow)) {
		icell = 0;
		while(pcell = ascigram_stack_iter(&prow->cells, &icell)) {
			ascigram_stack_uninit(&pcell->pattern_refs);
		}
		ascigram_stack_uninit(&prow->cells);
	}
	doc->rows.size = 0;
}

ascigram_cell *
get_cell(ascigram_document *doc, uint16_t x, uint16_t y)
{
	ascigram_row *row;
	ascigrem_cell *cell;
	
	row = ascigram_stack_get(&doc->rows, y)
	if (!row) return NULL;
	
	cell = ascigram_stack_get(&row->cells, x);
	return cell;
}

void
add_pat(ascigram_document *doc, ascigram_pattern_p fact)
{
	ascigram_pattern_p pat_new = ascigram_pattern_new(fact);
    ascigram_stack_push(&doc->pattern_refs, &pat_new);
}

void
remove_pat(ascigram_document *doc, ascigram_pattern_p pat, int cooc)
{
	int iattr;
	ascigram_attr *attr;
	
	if (!pat->finish)
       pat->finish = P_REJECT;

	iattr = 0;
	while (attr = ascigram_stack_iter(&pat->attrs, &attr)) {
		if (attr->meta & M_OCCUPIED) {
			int iref = 0;
			ascigram_pattern **pat_ref;
			ascigram_cell *cell = get_cell(doc, attr->x, attr->y);
			if (!cell) continue;
			
			if (coop) {
				while (pat_ref = ascigram_stack_iter(&cell->pattern_refs, &iref)) {
					pat_ref = ascigram_stack_pick(&cell->pattern_refs, --iref);
					if (*pat_ref != pat) {
						remove_pat(doc, *pat_ref, 0);
					}
				}
				cell->pattern_refs.size = 0;
			} else {
				while (pat_ref = ascigram_stack_iter(&cell->pattern_refs, &iref)) {
					if (*pat_ref == pat) {
						ascigram_stack_pick(&cell->pattern_refs, --iref);
						break;
					}
				}
			}
		}
	}
	
    pat->attrs.size = 0;
}

void
add_meta(ascigram_document *doc, ascigram_pattern_p pat, ascigram_cell *cell, int32_t meta)
{
	ascigram_attr *attr;
	if (meta & M_OCCUPIED) {
		ascigram_stack_push(&cell->pattern_refs, &pat);
    }
    attr = ascigram_stack_push(pat->attrs, &cell->attr);
    attr->meta = meta;
}

void
apply_pat(ascigram_document *doc, ascigram_pattern_p pat)
{
    int iattr;
	ascigram_attr *attr;
	
	if (pat->finish != P_ACCEPT) return;

	iattr = 0;
	while (attr = ascigram_stack_iter(&pat->attrs, &attr)) {
		ascigram_cell *cell = get_cell(doc, attr->x, attr->y);
		if (!cell) continue;
		
		cell->meta |= attr->meta;
	}
}

void
reserve_patrefs(ascigram_document *doc)
{
	ascigram_pattern **pat_ref;
	int ipat = 0;
	while (pat_ref = ascigram_stack_iter(&doc->pattern_refs, &ipat)) {
		if (!(*pat_ref)->finish != P_ACCEPT) {
			ascigram_pattern_free(*pat_ref);
			ascigram_stack_pick(&doc->pattern_refs, --ipat);
		}
	}
}


/**********************
 * EXPORTED FUNCTIONS *
 **********************/

ascigram_document *
ascigram_document_new(
	const ascigram_renderer *renderer)
{
	ascigram_document *doc = NULL;

	assert(renderer);

	doc = ascigram_malloc(sizeof(ascigram_document));
	memcpy(&doc->renderer, renderer, sizeof(ascigram_renderer));

	ascigram_stack_init(&doc->rows, sizeof(ascigram_row));
	ascigram_stack_init(&doc->pattern_refs, sizeof(ascigram_pattern_p));

	return doc;
}

void
ascigram_document_render(ascigram_document *doc, ascigram_buffer *ob, const uint8_t *data, size_t size)
{
	ascigram_pattern *pfact;
	
	int ifact;
 
    assert(doc->pattern_refs.size == 0);
	
	scan_rows(doc, data, size);

	ifact = 0;
	while(pfact = ascigram_patterns_iter(&ifact)) {
		ascigram_pattern **pat_ref;
		ascigram_row *prow;
		int y = 0;

		while (prow = ascigram_stack_iter(&doc->rows, &y)) {
			ascigram_cell *pcell;
			int x = 0;

			while (pcell = ascigram_stack_iter(&prow->cells, &x)) {
				int ipat;
				add_pat(doc, pfact);
				
				ipat = 0;
				while (pat_ref = ascigram_stack_iter(&doc->pattern_refs, &ipat)) {
					if ((*pat_ref)->finished) continue;
					
					int meta = ascigram_pattern_test(*pat_ref, pcell);
					if (meta == P_REJECT) {
						remove_pat(doc, *pat_ref, 0);
					} else if (meta == P_ACCEPT) {
						apply_pat(doc, *pat_ref);
						remove_pat(doc, *pat_ref, 1);
					} else {
						add_meta(doc, *pat_ref, pcell, meta);
					}
				}
								
			}
		}

		reserve_patrefs(doc);

	}

	ascigram_stack_uninit(&pattern_refs);
}

void
ascigram_document_free(ascigram_document *doc)
{
	free_rows(doc);	
	ascigram_stack_uninit(&doc->rows);
	free(doc);
}
