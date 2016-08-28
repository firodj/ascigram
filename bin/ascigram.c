#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "document.h"
#include "svg.h"
#include "option.h"

#define DEF_IUNIT 1024
#define DEF_OUNIT 64

/* PRINT HELP */

void
print_option(char short_opt, const char *long_opt, const char *description)
{
	if (short_opt)
		printf("  -%c, ", short_opt);
	else
		printf("      ");

	printf("--%-13s  %s\n", long_opt, description);
}

void
print_help(const char *basename)
{
	/* usage */
	printf("Usage: %s [OPTION]... [FILE]\n\n", basename);

	/* description */
	printf("Process the ASCII Art Diagram in FILE (or standard input) and render it to standard output, using the Ascigram library. ");
	
	/* main options */
	printf("Main options:\n");
	print_option('h', "help", "Show this help usage.");
	print_option('T', "time", "Show time spent in rendering.");

}

/* OPTION PARSING */

struct option_data {
    char *basename;
    int done;

	/* time reporting */
	int show_time;
	
    /* I/O */

	size_t iunit;
	size_t ounit;
    const char *filename;
};

int
parse_short_option(char opt, char *next, void *opaque)
{
	struct option_data *data = opaque;
	long int num;
	int isNum = next ? parseint(next, &num) : 0;

	if (opt == 'h') {
		print_help(data->basename);
		data->done = 1;
		return 0;
	}

	if (opt == 'T') {
		data->show_time = 1;
		return 1;
	}

	return 0;
}

int
parse_long_option(char *opt, char *next, void *opaque)
{
	struct option_data *data = opaque;
	long int num;
	int isNum = next ? parseint(next, &num) : 0;

	if (strcmp(opt, "help")==0) {
		print_help(data->basename);
		data->done = 1;
		return 0;
	}

	if (strcmp(opt, "time")==0) {
		data->show_time = 1;
		return 1;
	}

	return 0;
}

int
parse_argument(int argn, char *arg, int is_forced, void *opaque)
{
	struct option_data *data = opaque;

	if (argn == 0) {
		/* Input file */
		if (strcmp(arg, "-")!=0 || is_forced) data->filename = arg;
		return 1;
	}

	fprintf(stderr, "Too many arguments.\n");
	return 0;
}


int
main(int argc, char** argv)
{
	struct option_data data;
	clock_t t1, t2;	
	FILE *file = stdin;
	ascigram_buffer *ib, *ob;
	ascigram_renderer *renderer = NULL;
	void (*renderer_free)(ascigram_renderer *) = NULL;
	ascigram_document *document;

	/* Parse options */
	data.basename = argv[0];
	data.done = 0;
	data.show_time = 0;
	data.iunit = DEF_IUNIT;
	data.ounit = DEF_OUNIT;
	data.filename = NULL;

	argc = parse_options(argc, argv, parse_short_option, parse_long_option, parse_argument, &data);
	if (data.done) return 0;
	if (!argc) return 1;

	/* Open input file, if needed */
	if (data.filename) {
		file = fopen(data.filename, "r");
		if (!file) {
			fprintf(stderr, "Unable to open input file \"%s\": %s\n", data.filename, strerror(errno));
			return 5;
		}
	}

	/* Read everything */
	ib = ascigram_buffer_new(data.iunit);

	if (ascigram_buffer_putf(ib, file)) {
		fprintf(stderr, "I/O errors found while reading input.\n");
		return 5;
	}

	/* Close input file */
	if (file != stdin) fclose(file);

	/* Create the renderer */
	renderer = ascigram_svg_renderer_new();
	renderer_free = ascigram_svg_renderer_free;

	/* Perform ASCII-Art Diagram rendering */
	ob = ascigram_buffer_new(data.ounit);
	document = ascigram_document_new(renderer);

	t1 = clock();
	ascigram_document_render(document, ob, ib->data, ib->size);
	t2 = clock();

	/* Cleanup */
	ascigram_buffer_free(ib);
	ascigram_document_free(document);
	renderer_free(renderer);

	/* Write the result to stdout */
	(void)fwrite(ob->data, 1, ob->size, stdout);
	ascigram_buffer_free(ob);

	if (ferror(stdout)) {
		fprintf(stderr, "I/O errors found while writing output.\n");
		return 5;
	}

	/* Show rendering time */
	if (data.show_time) {
		double elapsed;

		if (t1 == ((clock_t) -1) || t2 == ((clock_t) -1)) {
			fprintf(stderr, "Failed to get the time.\n");
			return 1;
		}

		elapsed = (double)(t2 - t1) / CLOCKS_PER_SEC;
		if (elapsed < 1)
			fprintf(stderr, "Time spent on rendering: %7.2f ms.\n", elapsed*1e3);
		else
			fprintf(stderr, "Time spent on rendering: %6.3f s.\n", elapsed);
	}


	return 0;
}
