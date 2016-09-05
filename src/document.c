#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "ansicolor-w32.h"

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
	doc->width = 0;

	while (end < size) {
		ascigram_row row, *row_p;

		row.y = y;
		row.width = 0;
		ascigram_stack_init(&row.cells, sizeof(ascigram_cell));

		row_p = ascigram_stack_push(&doc->rows, &row);

		x = 0;
		while (end < size) {
			ascigram_cell cell, *cell_p;
			cell.ch = data[end];
			cell.attr.meta = M_NONE;
			cell.attr.y = row_p->y;
			cell.attr.x = x;
			row_p->width = x;
			ascigram_stack_init(&cell.pattern_refs, sizeof(ascigram_pattern_p));

			cell_p = ascigram_stack_push(&row_p->cells, &cell);
			if (!cell_p) ;

			end++;
			x++;

			if (doc->width < x) doc->width = x;

			if (cell.ch == '\n') break;
		}

		y++;

		doc->height = y;
	}
}

void
free_rows(ascigram_document *doc)
{
	int irow, icell;
	ascigram_row *row_p;
	ascigram_cell *cell_p;

	irow = 0;
	while(row_p = ascigram_stack_iter(&doc->rows, &irow)) {
		icell = 0;
		while(cell_p = ascigram_stack_iter(&row_p->cells, &icell)) {
			ascigram_stack_uninit(&cell_p->pattern_refs);
		}
		ascigram_stack_uninit(&row_p->cells);
	}
	doc->rows.size = 0;
}

ascigram_cell *
get_cell(ascigram_document *doc, uint16_t x, uint16_t y)
{
	ascigram_row *row;
	ascigram_cell *cell;

	row = ascigram_stack_get(&doc->rows, y);
	if (!row) return NULL;

	cell = ascigram_stack_get(&row->cells, x);
	return cell;
}

void
add_pat(ascigram_document *doc, ascigram_factory* fact)
{
	ascigram_pattern_p pat_new = ascigram_pattern_new(fact);
    ascigram_stack_push(&doc->pattern_refs, &pat_new);
}

void
remove_pat(ascigram_document *doc, ascigram_pattern_p pat, int cooc)
{
	int attr_i;
	ascigram_attr *attr_p;

	if (!pat->finish)
       pat->finish = P_REJECT;

	attr_i = 0;
	while (attr_p = ascigram_stack_iter(&pat->attrs, &attr_i)) {
		if (attr_p->meta & M_OCCUPIED) {
			int ref_i = 0;
			ascigram_pattern **pat_ref;
			ascigram_cell *cell_p = get_cell(doc, attr_p->x, attr_p->y);
			if (!cell_p) continue;

			if (cooc) {
				while (pat_ref = ascigram_stack_iter(&cell_p->pattern_refs, &ref_i)) {
					pat_ref = ascigram_stack_pick(&cell_p->pattern_refs, --ref_i);
					if (*pat_ref != pat) {
						remove_pat(doc, *pat_ref, 0);
					}
				}
				cell_p->pattern_refs.size = 0;
			} else {
				while (pat_ref = ascigram_stack_iter(&cell_p->pattern_refs, &ref_i)) {
					if (*pat_ref == pat) {
						ascigram_stack_pick(&cell_p->pattern_refs, --ref_i);
						break;
					}
				}
			}
		}
	}

    pat->attrs.size = 0;
}

void
add_meta(ascigram_document *doc, ascigram_pattern_p pat, ascigram_cell *cell_p, int32_t meta)
{
	ascigram_attr *attr_p;
	if (meta & M_OCCUPIED) {
		ascigram_stack_push(&cell_p->pattern_refs, &pat);
    }
    attr_p = ascigram_stack_push(&pat->attrs, &cell_p->attr);
    attr_p->meta = meta;
}

void
apply_pat(ascigram_document *doc, ascigram_pattern_p pat)
{
    int attr_i;
	ascigram_attr *attr_p;

	if (pat->finish != P_FINISH) return;

	attr_i = 0;
	while (attr_p = ascigram_stack_iter(&pat->attrs, &attr_i)) {
		ascigram_cell *cell_p = get_cell(doc, attr_p->x, attr_p->y);
		if (!cell_p) continue;

		cell_p->attr.meta |= attr_p->meta;
	}
}

void
reserve_patrefs(ascigram_document *doc)
{
	ascigram_pattern **pat_ref;
	int pat_i = 0;
	while (pat_ref = ascigram_stack_iter(&doc->pattern_refs, &pat_i)) {
		if ((*pat_ref)->finish == P_ACCEPT) {
			/* ACCEPT but uncomplete */
			(*pat_ref)->finish = P_FINISH;
			apply_pat(doc, *pat_ref);
			remove_pat(doc, *pat_ref, 1);
		}
		if ((*pat_ref)->finish != P_FINISH) {
			ascigram_pattern_free(*pat_ref);
			ascigram_stack_pick(&doc->pattern_refs, --pat_i);
		}
	}
}

ascigram_cell*
cells_iter(ascigram_document* doc, int* x, int* y)
{
	ascigram_cell *cell_p = NULL;
	ascigram_row *row_p;

	while (row_p = ascigram_stack_iter(&doc->rows, y)) {
		while (cell_p = ascigram_stack_iter(&row_p->cells, x)) {
			(*y)--;
			return cell_p;
		}
		*x = 0;
	}

	return cell_p;
}

void
test_cell(ascigram_document* doc, ascigram_pattern_p pat, ascigram_cell *cell_p)
{
	uint32_t meta;

	if (pat->finish & E_STOP) return;

    meta = ascigram_pattern_test(pat, cell_p);

    if (pat->finish && (pat->finish != P_ACCEPT)) {
		if (pat->finish == P_FINISH) {
			add_meta(doc, pat, cell_p, meta);
			apply_pat(doc, pat);
			remove_pat(doc, pat, 1);
		} else {
			remove_pat(doc, pat, 0);
		}
	} else {
		add_meta(doc, pat, cell_p, meta);
	}
}

void
parse_rows(ascigram_document *doc)
{
	ascigram_factory *fact;
	int fact_i, x, y;

    if (doc->pattern_refs.size != 0) return;
	if (doc->rows.size == 0) return;

	fact_i = 0;
	while(fact = ascigram_patterns_iter(&fact_i)) {
		ascigram_pattern **pat_ref;
		ascigram_cell *cell_p;
		int pat_i_hold = doc->pattern_refs.size;
		y = 0;
		x = 0;

		while (cell_p = cells_iter(doc, &x, &y)) {
			int pat_i;

			add_pat(doc, fact);

			pat_i = pat_i_hold;
			while (pat_ref = ascigram_stack_iter(&doc->pattern_refs, &pat_i)) {
				test_cell(doc, *pat_ref, cell_p);
			}
		}

		reserve_patrefs(doc);
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

	doc = ascigram_malloc(sizeof(ascigram_document));
	ascigram_memset(doc, 0, sizeof(ascigram_document));

	if (renderer)
		memcpy(&doc->renderer, renderer, sizeof(ascigram_renderer));

	ascigram_stack_init(&doc->rows, sizeof(ascigram_row));
	ascigram_stack_init(&doc->pattern_refs, sizeof(ascigram_pattern_p));

	return doc;
}

void
ascigram_document_render(ascigram_document *doc, ascigram_buffer *ob, const uint8_t *data, size_t size)
{
	scan_rows(doc, data, size);
	parse_rows(doc);
	/* TODO: something to ob */
}

void
ascigram_document_free(ascigram_document *doc)
{
	ascigram_pattern **pat_ref;
	int pat_i = 0;
	while (pat_ref = ascigram_stack_iter(&doc->pattern_refs, &pat_i)) {
		ascigram_pattern_free(*pat_ref);
	}
	ascigram_stack_uninit(&doc->pattern_refs);

	free_rows(doc);
	ascigram_stack_uninit(&doc->rows);
	free(doc);
}

void
dump_document_cells(ascigram_document *doc)
{
	int x, y, y_now, x_old;
	ascigram_cell *cell_p;
	x = 0; y = 0;
	fprintf(stdout, "\033[0;37m");
	fprintf(stdout, "\nwidth: %d, height: %d", doc->width, doc->height);
	while (cell_p = cells_iter(doc, &x, &y)) {
		if (x==1) {
			x = 0;
			y_now = y;
			fprintf(stdout, "\n\033[33m%d\033[0m:", y);
			while (cell_p = cells_iter(doc, &x, &y)) {
				if (y != y_now) break;
				fprintf(stdout, " %06x", cell_p->attr.meta);

				x_old = x;
			}
			if (doc->width > x_old) {
				for(x=x_old; x < doc->width; x++) {
					fprintf(stdout, " ......");
				}
			}

			y = y_now;
			x = 0;

			fprintf(stdout, ": ");

			while (cell_p = cells_iter(doc, &x, &y)) {
				uint8_t safe_ch = cell_p->ch;
				if (safe_ch < 0x20 || safe_ch > 126) safe_ch = ' ';
				if (y != y_now) break;
				if (cell_p->attr.meta & M_OCCUPIED) {
					fprintf(stdout, "\033[41m");
				} else if (cell_p->attr.meta & ~M_OCCUPIED) {
					fprintf(stdout, "\033[42m");
				}
				fprintf(stdout, "%c\033[0m", safe_ch);
				x_old = x;
			}

            y = y_now;
            x = x_old;
		}
	}
	fprintf(stdout, "\n");
}

void
dump_document_patrefs(ascigram_document *doc)
{
	ascigram_pattern_p *pat_ref;
	int pat_i = 0;
	fprintf(stdout, "\033[0;37m");
	while (pat_ref = ascigram_stack_iter(&doc->pattern_refs, &pat_i)) {
		fprintf(stdout, "\n\033[32m%d\033[0m:", pat_i-1);
		fprintf(stdout, "\t- name: %s\n", (*pat_ref)->factory->name);
		fprintf(stdout, "\t- state: %d\n", (*pat_ref)->state);
		if ((*pat_ref)->factory->dump) {
			(*pat_ref)->factory->dump(*pat_ref);
		}
	}
	fprintf(stdout, "\n");
}
