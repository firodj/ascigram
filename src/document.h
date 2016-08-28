/* document.h - generic markdown parser */
#ifndef _ASCIGRAM_DOCUMENT_H_
#define _ASCIGRAM_DOCUMENT_H_

#include "buffer.h"

#ifdef __cplusplus
extern "C" {
#endif


/*************
 * CONSTANTS *
 *************/

/*********
 * TYPES *
 *********/

struct ascigram_document;
typedef struct ascigram_document ascigram_document;

struct ascigram_renderer_data {
	void *opaque;
};
typedef struct ascigram_renderer_data ascigram_renderer_data;

/* ascigram_renderer - functions for rendering parsed data */
struct ascigram_renderer {
	/* state object */
	void *opaque;

	/* block level callbacks - NULL skips the block */
};
typedef struct ascigram_renderer ascigram_renderer;


/*************
 * FUNCTIONS *
 *************/

/* ascigram_document_new: allocate a new document processor instance */
ascigram_document *ascigram_document_new(
	const ascigram_renderer *renderer
) __attribute__ ((malloc));

/* ascigram_document_render: render regular Markdown using the document processor */
void ascigram_document_render(ascigram_document *doc, ascigram_buffer *ob, const uint8_t *data, size_t size);

/* ascigram_document_free: deallocate a document processor instance */
void ascigram_document_free(ascigram_document *doc);


#ifdef __cplusplus
}
#endif

#endif /** _ASCIGRAM_DOCUMENT_H_ **/