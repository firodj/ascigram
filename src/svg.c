#include "svg.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

struct ascigram_svg_renderer_state {
	void *opaque;
};
typedef struct ascigram_svg_renderer_state ascigram_svg_renderer_state;

ascigram_renderer *
ascigram_svg_renderer_new()
{
	static const ascigram_renderer cb_default = {
		NULL,
	};

	ascigram_svg_renderer_state *state;
	ascigram_renderer *renderer;

	/* Prepare the state pointer */
	state = ascigram_malloc(sizeof(ascigram_svg_renderer_state));
	memset(state, 0x0, sizeof(ascigram_svg_renderer_state));

	/* Prepare the renderer */
	renderer = ascigram_malloc(sizeof(ascigram_renderer));
	memcpy(renderer, &cb_default, sizeof(ascigram_renderer));

	renderer->opaque = state;
	return renderer;
}

void
ascigram_svg_renderer_free(ascigram_renderer *renderer)
{
	free(renderer->opaque);
	free(renderer);
}
