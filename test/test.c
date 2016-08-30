#include <stdio.h>

#include "test.h"

int test_stack_init(void);
int test_stack_push(void);

static 
test_case_item_t test_case_items[] = {
	{"Stack Init", test_stack_init},
	{"Stack Push", test_stack_push}
};

int
main(int argc, char** argv)
{
	int results = 0;

	int max = sizeof(test_case_items)/sizeof(test_case_items[0]);

	for (size_t i = 0; i < max; i++) {
		int result_case;
		
		fprintf(stdout, "TEST: %s ...", test_case_items[i].name);
		result_case = test_case_items[i].test_case();
		if (result_case == 0) {
			fprintf(stdout, "PASS\n");
		} else {
			fprintf(stdout, "FAIL\n");
			results++;
		}
	}

	return results;
}
