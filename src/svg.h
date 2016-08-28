/* html.h - HTML renderer and utilities */
#ifndef _ASCIGRAM_SVG_H_
#define _ASCIGRAM_SVG_H_

#include "document.h"
#include "buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ascigram_svg_renderer_new: allocates a regular SVG renderer */
ascigram_renderer *ascigram_svg_renderer_new(
) __attribute__ ((malloc));

/* ascigram_svg_renderer_free: deallocate an SVG renderer */
void ascigram_svg_renderer_free(ascigram_renderer *renderer);

#ifdef __cplusplus
}
#endif

#endif /** _ASCIGRAM_SVG_H_ **/
