#include "../pattern.h"
#include "stickman.h"
#include "dbcylinder.h"

void
ascigram_patterns_register_all()
{
	stickman_pattern_register();
	dbcylinder_pattern_register();
}
