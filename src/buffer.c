/* buffer.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "buffer.h"

void *
ascigram_malloc(size_t size)
{
	void *ret = malloc(size);
#ifdef _DEBUG
	fprintf(stderr, "malloc: 0x%x %d\n", ret, size);
#endif

	if (!ret) {
		fprintf(stderr, "Allocation failed.\n");
		abort();
	}

	return ret;
}

void *
ascigram_calloc(size_t nmemb, size_t size)
{
	void *ret = calloc(nmemb, size);
#ifdef _DEBUG
	fprintf(stderr, "calloc: 0x%x %d x %d\n", ret, nmemb, size);
#endif

	if (!ret) {
		fprintf(stderr, "Allocation failed.\n");
		abort();
	}

	return ret;
}

void *
ascigram_realloc(void *ptr, size_t size)
{
	void *ret = realloc(ptr, size);
#ifdef _DEBUG
	fprintf(stderr, "realloc: 0x%x %d\n", ret, size);
#endif

	if (!ret) {
		fprintf(stderr, "Allocation failed.\n");
		abort();
	}

	return ret;
}

void *
ascigram_memcpy(void *dest, const void *src, size_t size) {
	void *ret = memcpy(dest, src, size);
#ifdef _DEBUG
	fprintf(stderr, "memcpy: 0x%x <- 0x%x %d\n", dest, src, size);
#endif

	if (!ret) {
		fprintf(stderr, "Memcopy failed.\n");
		abort();
	}

	return ret;
}

void *
ascigram_memset(void *dest, unsigned char ch, size_t size)
{
	void *ret = memset(dest, ch, size);
#ifdef _DEBUG
	fprintf(stderr, "memset: 0x%x '%02X' %d", dest, ch, size);
#endif

	if (!ret) {
		fprintf(stderr, "Memset failed.\n");
	}

	return ret;
}

void
ascigram_buffer_init(
	ascigram_buffer *buf,
	size_t unit,
	ascigram_realloc_callback data_realloc,
	ascigram_free_callback data_free,
	ascigram_free_callback buffer_free)
{
	assert(buf);

	buf->data = NULL;
	buf->size = buf->asize = 0;
	buf->unit = unit;
	buf->data_realloc = data_realloc;
	buf->data_free = data_free;
	buf->buffer_free = buffer_free;
}

void
ascigram_buffer_uninit(ascigram_buffer *buf)
{
	assert(buf && buf->unit);
	buf->data_free(buf->data);
}

ascigram_buffer *
ascigram_buffer_new(size_t unit)
{
	ascigram_buffer *ret = ascigram_malloc(sizeof (ascigram_buffer));
	ascigram_buffer_init(ret, unit, ascigram_realloc, free, free);
	return ret;
}

void
ascigram_buffer_free(ascigram_buffer *buf)
{
	if (!buf) return;
	assert(buf && buf->unit);

	buf->data_free(buf->data);

	if (buf->buffer_free)
		buf->buffer_free(buf);
}

void
ascigram_buffer_reset(ascigram_buffer *buf)
{
	assert(buf && buf->unit);

	buf->data_free(buf->data);
	buf->data = NULL;
	buf->size = buf->asize = 0;
}

void
ascigram_buffer_grow(ascigram_buffer *buf, size_t neosz)
{
	size_t neoasz;
	assert(buf && buf->unit);

	if (buf->asize >= neosz)
		return;

	neoasz = buf->asize + buf->unit;
	while (neoasz < neosz)
		neoasz += buf->unit;

	buf->data = buf->data_realloc(buf->data, neoasz);
	buf->asize = neoasz;
}


void
ascigram_buffer_put(ascigram_buffer *buf, const uint8_t *data, size_t size)
{
	assert(buf && buf->unit);

	if (buf->size + size > buf->asize)
		ascigram_buffer_grow(buf, buf->size + size);

	memcpy(buf->data + buf->size, data, size);
	buf->size += size;
}

void
ascigram_buffer_puts(ascigram_buffer *buf, const char *str)
{
	ascigram_buffer_put(buf, (const uint8_t *)str, strlen(str));
}

void
ascigram_buffer_putc(ascigram_buffer *buf, uint8_t c)
{
	assert(buf && buf->unit);

	if (buf->size >= buf->asize)
		ascigram_buffer_grow(buf, buf->size + 1);

	buf->data[buf->size] = c;
	buf->size += 1;
}

int
ascigram_buffer_putf(ascigram_buffer *buf, FILE *file)
{
	assert(buf && buf->unit);

	while (!(feof(file) || ferror(file))) {
		ascigram_buffer_grow(buf, buf->size + buf->unit);
		buf->size += fread(buf->data + buf->size, 1, buf->unit, file);
	}

	return ferror(file);
}

void
ascigram_buffer_set(ascigram_buffer *buf, const uint8_t *data, size_t size)
{
	assert(buf && buf->unit);

	if (size > buf->asize)
		ascigram_buffer_grow(buf, size);

	memcpy(buf->data, data, size);
	buf->size = size;
}

void
ascigram_buffer_sets(ascigram_buffer *buf, const char *str)
{
	ascigram_buffer_set(buf, (const uint8_t *)str, strlen(str));
}

int
ascigram_buffer_eq(const ascigram_buffer *buf, const uint8_t *data, size_t size)
{
	if (buf->size != size) return 0;
	return memcmp(buf->data, data, size) == 0;
}

int
ascigram_buffer_eqs(const ascigram_buffer *buf, const char *str)
{
	return ascigram_buffer_eq(buf, (const uint8_t *)str, strlen(str));
}

int
ascigram_buffer_prefix(const ascigram_buffer *buf, const char *prefix)
{
	size_t i;

	for (i = 0; i < buf->size; ++i) {
		if (prefix[i] == 0)
			return 0;

		if (buf->data[i] != prefix[i])
			return buf->data[i] - prefix[i];
	}

	return 0;
}

void
ascigram_buffer_slurp(ascigram_buffer *buf, size_t size)
{
	assert(buf && buf->unit);

	if (size >= buf->size) {
		buf->size = 0;
		return;
	}

	buf->size -= size;
	memmove(buf->data, buf->data + size, buf->size);
}

const char *
ascigram_buffer_cstr(ascigram_buffer *buf)
{
	assert(buf && buf->unit);

	if (buf->size < buf->asize && buf->data[buf->size] == 0)
		return (char *)buf->data;

	ascigram_buffer_grow(buf, buf->size + 1);
	buf->data[buf->size] = 0;

	return (char *)buf->data;
}

void
ascigram_buffer_printf(ascigram_buffer *buf, const char *fmt, ...)
{
	va_list ap;
	int n;

	assert(buf && buf->unit);

	if (buf->size >= buf->asize)
		ascigram_buffer_grow(buf, buf->size + 1);

	va_start(ap, fmt);
	n = vsnprintf((char *)buf->data + buf->size, buf->asize - buf->size, fmt, ap);
	va_end(ap);

	if (n < 0) {
#ifndef _MSC_VER
		return;
#else
		va_start(ap, fmt);
		n = _vscprintf(fmt, ap);
		va_end(ap);
#endif
	}

	if ((size_t)n >= buf->asize - buf->size) {
		ascigram_buffer_grow(buf, buf->size + n + 1);

		va_start(ap, fmt);
		n = vsnprintf((char *)buf->data + buf->size, buf->asize - buf->size, fmt, ap);
		va_end(ap);
	}

	if (n < 0)
		return;

	buf->size += n;
}

void ascigram_buffer_put_utf8(ascigram_buffer *buf, unsigned int c) {
	unsigned char unichar[4];

	assert(buf && buf->unit);

	if (c < 0x80) {
		ascigram_buffer_putc(buf, c);
	}
	else if (c < 0x800) {
		unichar[0] = 192 + (c / 64);
		unichar[1] = 128 + (c % 64);
		ascigram_buffer_put(buf, unichar, 2);
	}
	else if (c - 0xd800u < 0x800) {
		ASCIGRAM_BUFPUTSL(buf, "\xef\xbf\xbd");
	}
	else if (c < 0x10000) {
		unichar[0] = 224 + (c / 4096);
		unichar[1] = 128 + (c / 64) % 64;
		unichar[2] = 128 + (c % 64);
		ascigram_buffer_put(buf, unichar, 3);
	}
	else if (c < 0x110000) {
		unichar[0] = 240 + (c / 262144);
		unichar[1] = 128 + (c / 4096) % 64;
		unichar[2] = 128 + (c / 64) % 64;
		unichar[3] = 128 + (c % 64);
		ascigram_buffer_put(buf, unichar, 4);
	}
	else {
		ASCIGRAM_BUFPUTSL(buf, "\xef\xbf\xbd");
	}
}
