/* stack.h - simple stacking */
#ifndef _ASCIGRAM_STACK_H_
#define _ASCIGRAM_STACK_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/*********
 * TYPES *
 *********/

struct ascigram_stack {
	void **item;
	size_t size;
	size_t asize;
};
typedef struct ascigram_stack ascigram_stack;


/*************
 * FUNCTIONS *
 *************/

/* ascigram_stack_init: initialize a stack */
void ascigram_stack_init(ascigram_stack *st, size_t initial_size);

/* ascigram_stack_uninit: free internal data of the stack */
void ascigram_stack_uninit(ascigram_stack *st);

/* ascigram_stack_grow: increase the allocated size to the given value */
void ascigram_stack_grow(ascigram_stack *st, size_t neosz);

/* ascigram_stack_push: push an item to the top of the stack */
void ascigram_stack_push(ascigram_stack *st, void *item);

/* ascigram_stack_pop: retrieve and remove the item at the top of the stack */
void *ascigram_stack_pop(ascigram_stack *st);

/* ascigram_stack_top: retrieve the item at the top of the stack */
void *ascigram_stack_top(const ascigram_stack *st);


#ifdef __cplusplus
}
#endif

#endif /** _ASCIGRAM_STACK_H_ **/
