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
#define P_ACCEPT 0x80000000
#define P_FINISH 0xC0000000
#define P_REJECT 0xE0000000
#define P_OUTPOS 0xD0000000

#define E_STOP   0x40000000
#define E_FAIL   0x30000000
#define E_RESULT 0xF0000000
#define E_MASK   0x0FFFFFFF

#define ccrBegin(n)   \
        int meta; \
	    n ## _opaque *opq = (n ## _opaque*)pat->opaque; \
        switch(pat->state) { case 0:;

#define ccrEnd()     \
        } \
        return P_REJECT; 

#define ccrReturn(z)     \
        do {\
            pat->state=__LINE__;\
            return (z); case __LINE__:;\
        } while (0)

#ifdef __cplusplus
extern "C" {
#endif

struct ascigram_pattern;
typedef struct ascigram_pattern * ascigram_pattern_p;

typedef uint32_t (*ascigram_pattern_match_f)(ascigram_pattern_p, ascigram_cell*);
typedef void (*ascigram_pattern_f)(ascigram_pattern_p);

struct ascigram_factory {
	const char *name;
	ascigram_pattern_match_f match;
	ascigram_pattern_f init;
	ascigram_pattern_f uninit;
	ascigram_pattern_f dump;
	size_t opaque_size;
};
typedef struct ascigram_factory ascigram_factory;

struct ascigram_pattern {
	ascigram_factory *factory;
	int state;
	int finish;
	ascigram_stack attrs;
	ascigram_attr curr;
	char opaque[1];
};
typedef struct ascigram_pattern ascigram_pattern;

void ascigram_patterns_initze();
void ascigram_patterns_uninit();
void ascigram_patterns_register(ascigram_factory *fact);
void ascigram_patterns_register_all();
ascigram_factory* ascigram_patterns_iter(int *index);
ascigram_pattern_p ascigram_pattern_new(ascigram_factory* fact);
void ascigram_pattern_free(ascigram_pattern_p pat);
uint32_t ascigram_pattern_test(ascigram_pattern_p pat, ascigram_cell* cell_p);
int ascigram_pattern_expect(ascigram_pattern_p pat, ascigram_cell* cell_p, const char *expect, int32_t default_meta);
int ascigram_pattern_await(ascigram_pattern_p pat, ascigram_cell* cell_p, int16_t x, int16_t y, int *meta);

#ifdef __cplusplus
}
#endif

#endif /** _ASCIGRAM_PATTERN_H_ **/
