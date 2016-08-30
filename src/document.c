
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

	return doc;
}

void
ascigram_document_render(ascigram_document *doc, ascigram_buffer *ob, const uint8_t *data, size_t size)
{
	ascigram_pattern *pfact;
	ascigram_stack pattern_refs;
	int ifact;

	ascigram_stack_init(&pattern_refs, sizeof(ascigram_pattern_p));

	scan_rows(doc, data, size);

	ifact = 0;
	while(pfact = ascigram_patterns_iter(&ifact)) {
		ascigram_pattern **pat_ref;
		ascigram_row *prow;
		int y = 0;

		assert(pattern_refs.size == 0);

		while (prow = ascigram_stack_iter(&doc->rows, &y)) {
			ascigram_cell *pcell;
			int x = 0;

			while (pcell = ascigram_stack_iter(&prow->cells, &x)) {
				int ipat;
				ascigram_pattern *pat_new = ascigram_pattern_new(pfact);
				ascigram_stack_push(&pattern_refs, &pat_new);

				ipat = 0;
				while (pat_ref = ascigram_stack_iter(&pattern_refs, &ipat)) {
					int meta = (*pat_ref)->match(*pat_ref, pcell);
					

				}
								
			}
		}

		while (pat_ref = ascigram_stack_pop(&pattern_refs)) {
			ascigram_pattern_free(*pat_ref);
		}

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
