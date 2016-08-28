#include "document.h"
#include "document_priv.h"

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

	return doc;
}

void
ascigram_document_render(ascigram_document *doc, ascigram_buffer *ob, const uint8_t *data, size_t size)
{
}

void
ascigram_document_free(ascigram_document *doc)
{

	free(doc);
}
