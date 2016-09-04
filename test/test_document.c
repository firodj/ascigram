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

static const char *data_stickman = "    \n"
	"  o \n"
	" -|-\n"
	" / \\\n";

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

int
test_document_stickman(void)
{
	int results = 0;
	ascigram_cell *cell_p;
	int x, y;
	int expect_x,expect_y;
	
	ascigram_document *doc = ascigram_document_new(NULL);

	ascigram_patterns_register_all();

	scan_rows(doc, data_blank, strlen(data_blank));
	parse_rows(doc);

	ascigram_document_free(doc);

	return results;
}


