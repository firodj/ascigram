#include <stdio.h>
#include "test.h"
#include "pattern.h"

int test_stack_init(void);
int test_stack_push(void);
int test_pattern_factory(void);
int test_document_cells_iter(void);
int test_pattern_stickman(void);
int test_document_stickman(void);
int test_document_dbcylinder(void);
int test_document_documentbox(void);

static 
test_case_item_t test_case_items[] = {
	{"Stack Init", test_stack_init},
	{"Stack Push", test_stack_push},
	{"Pattern Factory", test_pattern_factory},
	{"Document Cells Iter", test_document_cells_iter},
	{"Pattern Stickman", test_pattern_stickman},
	{"Document Stickman", test_document_stickman},
	{"Document DbCylinder", test_document_dbcylinder},
	{"Document DocumentBox", test_document_documentbox}
};

int
main(int argc, char** argv)
{
	int results = 0;
	size_t i;

	int max = sizeof(test_case_items)/sizeof(test_case_items[0]);

	for (i = 0; i < max; i++) {
		int result_case;
		
		fprintf(stdout, "TEST: %s ...", test_case_items[i].name);
		/* setUp */
		ascigram_patterns_init();
		/* run */
		result_case = test_case_items[i].test_case();
		/* tearDown */
		ascigram_patterns_uninit();
		
		if (result_case == 0) {
			fprintf(stdout, "PASS\n");
		} else {
			fprintf(stdout, "FAIL\n");
			results++;
		}
	}

	return results;
}
