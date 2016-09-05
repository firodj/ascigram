#include <stdio.h>
#include "ansicolor-w32.h"
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

void
dump_document_cells(ascigram_document *doc)
{
	int x, y, y_now, x_old;
	ascigram_cell *cell_p;
	x = 0; y = 0;
	fprintf(stdout, "\033[0;37m");
	while (cell_p = cells_iter(doc, &x, &y)) {
		if (x==1) {
			x = 0;
			y_now = y;
			fprintf(stdout, "\n\033[33m%d\033[0m:", y);		
			while (cell_p = cells_iter(doc, &x, &y)) {
				if (y != y_now) break;
				fprintf(stdout, " %06x", cell_p->attr.meta);
			}

			y = y_now;
			x = 0;

			fprintf(stdout, ": ");

			while (cell_p = cells_iter(doc, &x, &y)) {
				if (y != y_now) break;
				if (cell_p->attr.meta & M_OCCUPIED) {
					fprintf(stdout, "\033[41m");
				}
				fprintf(stdout, "%c\033[0m", cell_p->ch);				
				x_old = x;
			}

            y = y_now;
            x = x_old;
		}
	}
	fprintf(stdout, "\n");
}

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

	EXPECT(0 == expect_x && 4 == expect_y, "unsatisfy end, %d %d", expect_x, expect_y);

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

	scan_rows(doc, data_stickman, strlen(data_stickman));
	parse_rows(doc);

	dump_document_cells(doc);
	dump_document_patrefs(doc);

	ascigram_document_free(doc);

	return results;
}


