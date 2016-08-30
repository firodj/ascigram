#include <stdio.h>
#include "document.h"
#include "pattern.h"
#include "test.h"
#include "svg.h"
#include <string.h>

int
test_document_blank(void)
{
	int results = 0;
	int x, y;
	ascigram_row *prow;
	ascigram_cell *pcell;

	ascigram_patterns_initialize();

	const char *input = "    \n" 
		"    \n"
		"    \n"
		"    \n";

	ascigram_renderer *ren = ascigram_svg_renderer_new();
	ascigram_document *doc = ascigram_document_new(ren);
	ascigram_document_render(doc, NULL, input, strlen(input));

	EXPECT(4 == doc->rows.size, "rows.size: %d", doc->rows.size);
	
	y = 0;
	while (prow = ascigram_stack_iter(&doc->rows, &y)) {
		x = 0;
		while (pcell = ascigram_stack_iter(&prow->cells, &x)) {
			EXPECT(y-1 == pcell->attr.y || x-1 == pcell->attr.x, 
					"(%d %d) <> %d %d '%02X' |", y-1, x-1, 
					pcell->attr.y, pcell->attr.x, pcell->ch);
		}
	}

	ascigram_document_free(doc);

	return results;
}
