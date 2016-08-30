#include <stdio.h>
#include "stack.h"
#include "test.h"
#include <stdint.h>

int
test_stack_init(void)
{
	int results = 0;
	ascigram_stack st;

	ascigram_stack_init(&st, sizeof(void*));
	EXPECT(NULL != st.item, "st.item: 0x%x", st.item);
	EXPECT(4 == st.element_size, "st.elemen_size: %d", st.element_size);
	EXPECT(4 == st.asize, "st.asize: %d", st.asize);

	ascigram_stack_uninit(&st);
	EXPECT(NULL == st.item, "st.item: 0x%x", st.item);

	return results;
}

int
test_stack_push(void)
{
	int results = 0;
	ascigram_stack st;
	uint16_t val, *pval;
	int index;

	ascigram_stack_init(&st, sizeof(val));
	EXPECT(2 == st.element_size, "st.elemen_size: %d\n", st.element_size);

	for (val = 10; val <= 40; val += 10) {
		ascigram_stack_push(&st, &val);
	}
	EXPECT(4 == st.size, "st.size: %d", st.size);

	pval = ascigram_stack_top(&st);
	EXPECT(40 == *pval, "1 *pval: %d", *pval);

	pval = ascigram_stack_get(&st, 0);
	EXPECT(10 == *pval, "2 *pval: %d", *pval);

	pval = ascigram_stack_get(&st, 1);
	EXPECT(20 == *pval, "3 *pval: %d", *pval);

	pval = ascigram_stack_pick(&st, 1);
	EXPECT(3 == st.size, "st.size: %d", st.size);
	EXPECT(20 == *pval, "4 *pval: %d", *pval);

	pval = ascigram_stack_get(&st, 1);
	EXPECT(30 == *pval, "5 *pval: %d", *pval);

	pval = ascigram_stack_top(&st);
	EXPECT(40 == *pval, "6 *pval: %d", *pval);

	pval = ascigram_stack_pop(&st);
	EXPECT(40 == *pval, "7 *pval: %d", *pval);

	pval = ascigram_stack_top(&st);
	EXPECT(30 == *pval, "8 *pval: %d", *pval);

	index = 0;
	pval = ascigram_stack_iter(&st, &index);
	EXPECT(1 == index, "1 index: %d", index);
	EXPECT(10 == *pval, "9 *pval: %d", *pval);

	pval = ascigram_stack_iter(&st, &index);
	EXPECT(2 == index, "2 index: %d", index);
	EXPECT(30 == *pval, "10 *pval: %d", *pval);

	pval = ascigram_stack_iter(&st, &index);
	EXPECT(NULL == pval, "pval: 0x%x", pval);

	ascigram_stack_uninit(&st);

	return results;
}


