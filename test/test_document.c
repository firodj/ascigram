#include <stdio.h>
#include "document.h"
#include "pattern.h"
#include "test.h"
#include "svg.h"
#include <string.h>
#include "ansicolor-w32.h"

#ifndef _MSC_VER
#include <errno.h>
#endif

void scan_rows(ascigram_document *doc, const uint8_t *data, size_t size);
void parse_rows(ascigram_document *doc);
ascigram_cell* cells_iter(ascigram_document* doc, int* x, int* y);

static const char *data_blank = "a   \n"
	" b  \n"
	"  c \n"
	"   d\n";

#define DEF_IUNIT 1024
#define DEF_OUNIT 64

static const char* documents[] = {
	"stickman",
	"dbcylinder",
	"documentbox",
	"diamondbox"
};

ascigram_buffer *
read_file(const char *filename) {
	FILE *file = fopen(filename, "r");
	ascigram_buffer *ib;

	if (!file) {
		fprintf(stderr, "Unable to open input file \"%s\": %s\n", filename, strerror(errno));
		return NULL;
	}

	ib = ascigram_buffer_new(DEF_IUNIT);

	if (!ascigram_buffer_putf(ib, file)) goto read_file_done;

	fprintf(stderr, "I/O errors found while reading input.\n");
	ascigram_buffer_free(ib);

read_file_done:
	fclose(file);
	return ib;
}

int
match_document(const char *name) {
	ascigram_buffer *ib, *xb, *ob;
	ascigram_document *doc = NULL;
	int res = -1;
	char inputname[256];
	char expectname[256];

	sprintf(inputname, "test/%s.txt", name);
	sprintf(expectname, "test/%s.expect", name);

	ib = read_file(inputname);
	if (ib) {
		doc = ascigram_document_new(NULL);

		ascigram_patterns_register_all();

		scan_rows(doc, ib->data, ib->size);
		ascigram_buffer_free(ib);
	} else {
		return res;
	}

	parse_rows(doc);
	dump_document_cells(doc);

	ob = ascigram_buffer_new(DEF_OUNIT);
	if (ob) {
		export_document_patrefs(doc, ob);

		xb = read_file(expectname);

		res = print_diff(ob, xb);

		if (xb) {
			ascigram_buffer_free(xb);
		}

		ascigram_buffer_free(ob);
	}

	ascigram_document_free(doc);
	return res;
}

int
print_diff(ascigram_buffer *ob, ascigram_buffer *xb)
{
	int res = 0;
	size_t i, j;
	int i_stop, j_stop;

	i_stop = 0;
	j_stop = 0;

	for (i = 0, j = 0; i < ob->size; ++i) {
		for (;i < ob->size; ++i) {
			if (ob->data[i] == 0) { i_stop = 1; break; }
			if (ob->data[i] == '\n') continue;
			break;
		}
		if (i >= ob->size) i_stop = 1;

		if (xb && res == 0) {
			for (;j < xb->size; ++j) {
				if (xb->data[j] == 0) { j_stop = 1; break; }
				if (xb->data[j] == '\r'|| xb->data[j] == '\n') continue;
				break;
			}
			if (j >= xb->size) j_stop = 1;

			if (i_stop && j_stop) break;
			else if (!i_stop && !j_stop) {
				if (ob->data[i] != xb->data[j]) {
					fputs("\033[31m", stdout);
					fputc(ob->data[i], stdout);
					fputs("\033[0m", stdout);
					fputs("\033[32m", stdout);
					fputc(xb->data[j], stdout);
					fputs("\033[0m", stdout);

					res = 1;
				} else {
					fputc(ob->data[i], stdout);
				}
			} else res = 1;

			j++;
		} else {
			if (!i_stop)
				fputc(ob->data[i], stdout);
		}
	}

	if (!xb) return -1;
	return res;
}

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
		if (expect_x >= doc->width) {
			expect_x = 0;
			expect_y++;
		}
	}

	EXPECT(0 == expect_x && doc->height == expect_y, "unsatisfy end, %d %d", expect_x, expect_y);

	ascigram_document_free(doc);

	return results;
}

int
test_documents(void)
{
	size_t i = 0;
	size_t max = sizeof(documents)/sizeof(documents[0]);
	int results = 0;

	const char *test_only = 0;	// eg. "diamondbox";

	fprintf(stdout, "\n");
	for(i =0; i < max; ++i) {
		int result_case = 0;
		const char *name = documents[i];

		fprintf(stdout, "PATTERN: %s ...", name);
		if (test_only && (strcmp(name, test_only) != 0)) {
			fputs("\033[33mPATTERN SKIP\033[0m\n", stdout);
			continue;
		}
		result_case = match_document(name);

		if (result_case == 0) {
			fputs("\033[32mPATTERN PASS\033[0m\n", stdout);
		} else {
			fputs("\033[31mPATTERN FAIL\033[0m\n", stdout);
			results++;
		}
	}

	return results;
}

