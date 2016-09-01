/* pattern.h */
#ifndef _ASCIGRAM_PATTERN_H_
#define _ASCIGRAM_PATTERN_H_

#include "stack.h"
#include "document.h"

#define M_NONE 0
#define M_OCCUPIED 0x000001
#define M_BOX_START_S 0x000002
#define M_BOX_AFTER_S 0x000004
#define M_BOX_START_E 0x000008
#define M_BOX_AFTER_E 0x000010
#define M_LINE_START_E 0x000020
#define M_DASH_START_E 0x000040
#define M_LINE_AFTER_E 0x000080
#define M_DASH_AFTER_E 0x000100
#define M_LINE_START_S 0x000200
#define M_DASH_START_S 0x000400
#define M_LINE_AFTER_S 0x000800
#define M_DASH_AFTER_S 0x001000
#define M_LINE_START_SE 0x002000
#define M_DASH_START_SE 0x004000
#define M_LINE_AFTER_SE 0x008000
#define M_DASH_AFTER_SE 0x010000
#define M_LINE_START_SW 0x020000
#define M_DASH_START_SW 0x040000
#define M_LINE_AFTER_SW 0x080000
#define M_DASH_AFTER_SW 0x100000
#define P_REJECT -1
#define P_ACCEPT -2

#ifdef __cplusplus
extern "C" {
#endif

struct ascigram_cell;
struct ascigram_pattern;
typedef struct ascigram_cell ascigram_cell;
typedef struct ascigram_pattern * ascigram_pattern_p;

typedef int (*ascigram_pattern_match_t)(ascigram_pattern_p, ascigram_cell*);
typedef void (*ascigram_pattern_init_t)(ascigram_pattern_p);
typedef void (*ascigram_pattern_uninit_t)(ascigram_pattern_p);

struct ascigram_factory {
	const char *name;
	ascigram_pattern_match_t match;
	ascigram_pattern_init_t init;
	ascigram_pattern_uninit_t uninit;
};
typedef struct ascigram_factory ascigram_factory;

struct ascigram_pattern {
	ascigram_factory *factory;
	int state;
	int finish;
	ascigram_stack attrs;
	ascigram_cell curr;
};
typedef struct ascigram_pattern ascigram_pattern;

void ascigram_patterns_initialize();
void ascigram_patterns_register(ascigram_factory *fact);
ascigram_factory* ascigram_patterns_iter(int *index);
ascigram_pattern_p ascigram_pattern_new(ascigram_factory* fact);
void ascigram_pattern_free(ascigram_pattern_p pat);
int ascigram_pattern_expect(ascigram_pattern_p pat, const char *);

#ifdef __cplusplus
}
#endif

#endif /** _ASCIGRAM_PATTERN_H_ **/
