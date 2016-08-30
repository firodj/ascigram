#include "document.h"

#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "stack.h"

#ifndef _MSC_VER
#include <strings.h>
#else
#define strncasecmp	_strnicmp
#endif

struct ascigram_symbol {
	wchar_t ch;
	uint16_t x, y;
    uint32_t meta;
	ascigram_stack occupants;
}

struct ascigram_board {
	ascigram_stack rows;
}

struct ascigram_document {
	ascigram_renderer renderer;
	ascigram_renderer_data data;

	ascigram_stack rows;
};

/************
 * INTERNAL *
 ************/

void
scan_rows(ascigram_document *doc, const uint8_t *data, size_t size)
{
	size_t end = 0;
	
	while (end < size) {
		ascigram_stack_push(&doc->rows, (void*)end);
		
		fprintf(stderr, "TEST:: rows: %d\n", end);
		
		while (end < size && data[end] != '\n') end++;
		if (data[end] == '\n') end++;
	}
}

/**********************
 * EXPORTED FUNCTIONS *
 **********************/

ascigram_document *
ascigram_document_new(
	const ascigram_renderer *renderer)
{
	ascigram_document *doc = NULL;

	assert(renderer);

	doc = ascigram_malloc(sizeof(ascigram_document));
	memcpy(&doc->renderer, renderer, sizeof(ascigram_renderer));

	ascigram_stack_init(&doc->rows, 0);

	return doc;
}

void
ascigram_document_render(ascigram_document *doc, ascigram_buffer *ob, const uint8_t *data, size_t size)
{
	scan_rows(doc, data, size);

	size_t beg, end;
}

void
ascigram_document_free(ascigram_document *doc)
{
	ascigram_stack_uninit(&doc->rows);
	free(doc);
}
