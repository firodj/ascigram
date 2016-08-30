#include "stack.h"

#include "buffer.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

void
ascigram_stack_init(ascigram_stack *st, size_t element_size)
{
	assert(st);

	st->item = NULL;
	st->element_size = element_size;
	st->size = st->asize = 0;

	ascigram_stack_grow(st, 4);
}

void
ascigram_stack_uninit(ascigram_stack *st)
{
	assert(st);

	free(st->item);
	st->item = NULL;
}

void
ascigram_stack_grow(ascigram_stack *st, size_t neosz)
{
	assert(st);

	if (st->asize >= neosz)
		return;

	st->item = ascigram_realloc(st->item, neosz * st->element_size);
	ascigram_memset((char*)st->item + (st->asize * st->element_size), 0x0, (neosz - st->asize) * st->element_size);

	st->asize = neosz;

	if (st->size > neosz)
		st->size = neosz;
}

void*
ascigram_stack_push(ascigram_stack *st, void *item)
{
	assert(st);

	if (st->size >= st->asize)
		ascigram_stack_grow(st, st->size + 8);

	ascigram_memcpy((char*)st->item + (st->size++ * st->element_size),
	    item, st->element_size);
	return ascigram_stack_top(st);
}

void *
ascigram_stack_pop(ascigram_stack *st)
{
	assert(st);

	if (!st->size)
		return NULL;

	return (char*)st->item + (--st->size * st->element_size);
}

void *
ascigram_stack_top(const ascigram_stack *st)
{
	return ascigram_stack_get(st, st->size - 1);
}

void *
ascigram_stack_get(const ascigram_stack *st, int index)
{
	assert(st);

	if (index < 0 || index >= st->size)
		return NULL;

    return (char*)st->item + (index * st->element_size);
}

void *
ascigram_stack_pick(ascigram_stack *st, int index)
{
	void *item;
	
	if (index == st->size - 1)
	    return ascigram_stack_pop(st);
	
    item = ascigram_stack_get(st, index);
	if (item) {
		if (st->size >= st->asize)
			ascigram_stack_grow(st, st->size + 2);

	    item = ascigram_stack_push(st, item);

	    ascigram_memcpy((char*)st->item + (index * st->element_size),
			(char*)st->item + ((index + 1) * st->element_size),
			(--st->size - index) * st->element_size);
		st->size--;
	}
	
	return item;
}

void *
ascigram_stack_iter(const ascigram_stack *st, int *index)
{
	if (*index < st->size)
		return ascigram_stack_get(st, (*index)++);
	
	return NULL;
}
