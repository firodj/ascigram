#include <stdio.h>
#include "document.h"
#include "pattern.h"
#include "test.h"
#include "svg.h"
#include <string.h>

void scan_rows(ascigram_document *doc, const uint8_t *data, size_t size);
void parse_rows(ascigram_document *doc);
ascigram_cell* cells_iter(ascigram_document* doc, int* x, int* y);

static const char *data_blank = "    \n"
	"    \n"
	"    \n"
	"    \n";

int
test_document_blank(void)
{
	int results = 0;
	int x, y;
	ascigram_row *prow;
	ascigram_cell *pcell;

	ascigram_document *doc = ascigram_document_new(NULL);

	scan_rows(doc, data_blank, strlen(data_blank));

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

int
test_document_cells_iter(void)
{
	int results = 0;
	ascigram_cell *cell_p;
	int x, y;
	int expect_x,expect_y;
	
	ascigram_document *doc = ascigram_document_new(NULL);

	scan_rows(doc, data_blank, strlen(data_blank));

	x = 0; y = 0;
	expect_x = 0; expect_y = 0;
	while (cell_p = cells_iter(doc, &x, &y)) {
		EXPECT(expect_x == cell_p->attr.x, "x: %d", expect_x);
		EXPECT(expect_y == cell_p->attr.y, "y: %d", expect_y);

		expect_x++;
		if (expect_x >= 4) {
			expect_x = 0;
			expect_y++;
		}
	}
	EXPECT(0 == expect_x && 4 == expect_y, "unsatisfy send coordinate");

	ascigram_document_free(doc);

	return results;
}

