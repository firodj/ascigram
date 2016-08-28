#include "stack.h"

#include "buffer.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

void
ascigram_stack_init(ascigram_stack *st, size_t initial_size)
{
	assert(st);

	st->item = NULL;
	st->size = st->asize = 0;

	if (!initial_size)
		initial_size = 8;

	ascigram_stack_grow(st, initial_size);
}

void
ascigram_stack_uninit(ascigram_stack *st)
{
	assert(st);

	free(st->item);
}

void
ascigram_stack_grow(ascigram_stack *st, size_t neosz)
{
	assert(st);

	if (st->asize >= neosz)
		return;

	st->item = ascigram_realloc(st->item, neosz * sizeof(void *));
	memset(st->item + st->asize, 0x0, (neosz - st->asize) * sizeof(void *));

	st->asize = neosz;

	if (st->size > neosz)
		st->size = neosz;
}

void
ascigram_stack_push(ascigram_stack *st, void *item)
{
	assert(st);

	if (st->size >= st->asize)
		ascigram_stack_grow(st, st->size * 2);

	st->item[st->size++] = item;
}

void *
ascigram_stack_pop(ascigram_stack *st)
{
	assert(st);

	if (!st->size)
		return NULL;

	return st->item[--st->size];
}

void *
ascigram_stack_top(const ascigram_stack *st)
{
	assert(st);

	if (!st->size)
		return NULL;

	return st->item[st->size - 1];
}
