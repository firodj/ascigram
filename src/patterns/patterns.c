#include "../pattern.h"
#include "stickman.h"
#include "dbcylinder.h"
#include "documentbox.h"
#include "diamondbox.h"
#include "rectangularbox.h"

void
ascigram_patterns_register_all()
{
	stickman_pattern_register();
	dbcylinder_pattern_register();
	documentbox_pattern_register();
	diamondbox_pattern_register();
	rectangularbox_pattern_register();
}
