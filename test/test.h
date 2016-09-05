/* test.h */
#ifndef _TEST_H_
#define _TEST_H_

typedef int (*test_case_t)(void);

struct test_case_item_t {
	const char *name;
	test_case_t test_case;
};

typedef struct test_case_item_t test_case_item_t;

#define EXPECT(spec, msg, ...) \
	if (!(spec)) { \
		fprintf(stderr, "\n  FAIL: " #msg "\n", ##__VA_ARGS__); \
		results++; \
	}

#endif /** _TEST_H_ **/
