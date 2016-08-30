/* buffer.h */
#ifndef _ASCIGRAM_BUFFER_H_
#define _ASCIGRAM_BUFFER_H_

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#define __attribute__(x)
#define inline __inline
#define __builtin_expect(x,n) x
#endif

/*********
 * TYPES *
 *********/

typedef void *(*ascigram_realloc_callback)(void *, size_t);
typedef void (*ascigram_free_callback)(void *);

struct ascigram_buffer {
	uint8_t *data;	/* actual character data */
	size_t size;	/* size of the string */
	size_t asize;	/* allocated size (0 = volatile buffer) */
	size_t unit;	/* reallocation unit size (0 = read-only buffer) */

	ascigram_realloc_callback data_realloc;
	ascigram_free_callback data_free;
	ascigram_free_callback buffer_free;
};

typedef struct ascigram_buffer ascigram_buffer;


/*************
 * FUNCTIONS *
 *************/

/* allocation wrappers */
void *ascigram_malloc(size_t size) __attribute__ ((malloc));
void *ascigram_calloc(size_t nmemb, size_t size) __attribute__ ((malloc));
void *ascigram_realloc(void *ptr, size_t size) __attribute__ ((malloc));
void *ascigram_memcpy(void *dest, const void *src, size_t size) __attribute__ ((memcpy));

/* ascigram_buffer_init: initialize a buffer with custom allocators */
void ascigram_buffer_init(
	ascigram_buffer *buffer,
	size_t unit,
	ascigram_realloc_callback data_realloc,
	ascigram_free_callback data_free,
	ascigram_free_callback buffer_free
);

/* ascigram_buffer_uninit: uninitialize an existing buffer */
void ascigram_buffer_uninit(ascigram_buffer *buf);

/* ascigram_buffer_new: allocate a new buffer */
ascigram_buffer *ascigram_buffer_new(size_t unit) __attribute__ ((malloc));

/* ascigram_buffer_free: free the buffer */
void ascigram_buffer_free(ascigram_buffer *buf);

/* ascigram_buffer_reset: free internal data of the buffer */
void ascigram_buffer_reset(ascigram_buffer *buf);

/* ascigram_buffer_grow: increase the allocated size to the given value */
void ascigram_buffer_grow(ascigram_buffer *buf, size_t neosz);

/* ascigram_buffer_put: append raw data to a buffer */
void ascigram_buffer_put(ascigram_buffer *buf, const uint8_t *data, size_t size);

/* ascigram_buffer_puts: append a NUL-terminated string to a buffer */
void ascigram_buffer_puts(ascigram_buffer *buf, const char *str);

/* ascigram_buffer_putc: append a single char to a buffer */
void ascigram_buffer_putc(ascigram_buffer *buf, uint8_t c);

/* ascigram_buffer_putf: read from a file and append to a buffer, until EOF or error */
int ascigram_buffer_putf(ascigram_buffer *buf, FILE* file);

/* ascigram_buffer_set: replace the buffer's contents with raw data */
void ascigram_buffer_set(ascigram_buffer *buf, const uint8_t *data, size_t size);

/* ascigram_buffer_sets: replace the buffer's contents with a NUL-terminated string */
void ascigram_buffer_sets(ascigram_buffer *buf, const char *str);

/* ascigram_buffer_eq: compare a buffer's data with other data for equality */
int ascigram_buffer_eq(const ascigram_buffer *buf, const uint8_t *data, size_t size);

/* ascigram_buffer_eq: compare a buffer's data with NUL-terminated string for equality */
int ascigram_buffer_eqs(const ascigram_buffer *buf, const char *str);

/* ascigram_buffer_prefix: compare the beginning of a buffer with a string */
int ascigram_buffer_prefix(const ascigram_buffer *buf, const char *prefix);

/* ascigram_buffer_slurp: remove a given number of bytes from the head of the buffer */
void ascigram_buffer_slurp(ascigram_buffer *buf, size_t size);

/* ascigram_buffer_cstr: NUL-termination of the string array (making a C-string) */
const char *ascigram_buffer_cstr(ascigram_buffer *buf);

/* ascigram_buffer_printf: formatted printing to a buffer */
void ascigram_buffer_printf(ascigram_buffer *buf, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

/* ascigram_buffer_put_utf8: put a Unicode character encoded as UTF-8 */
void ascigram_buffer_put_utf8(ascigram_buffer *buf, unsigned int codepoint);

/* ascigram_buffer_free: free the buffer */
void ascigram_buffer_free(ascigram_buffer *buf);


/* ASCIGRAM_BUFPUTSL: optimized ascigram_buffer_puts of a string literal */
#define ASCIGRAM_BUFPUTSL(output, literal) \
	ascigram_buffer_put(output, (const uint8_t *)literal, sizeof(literal) - 1)

/* ASCIGRAM_BUFSETSL: optimized ascigram_buffer_sets of a string literal */
#define ASCIGRAM_BUFSETSL(output, literal) \
	ascigram_buffer_set(output, (const uint8_t *)literal, sizeof(literal) - 1)

/* ASCIGRAM_BUFEQSL: optimized ascigram_buffer_eqs of a string literal */
#define ASCIGRAM_BUFEQSL(output, literal) \
	ascigram_buffer_eq(output, (const uint8_t *)literal, sizeof(literal) - 1)


#ifdef __cplusplus
}
#endif

#endif /** _ASCIGRAM_BUFFER_H_ **/
