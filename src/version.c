#include "version.h"

void
asciigram_version(int *major, int *minor, int *revision)
	{
	*major = ASCIGRAM_VERSION_MAJOR;
	*minor = ASCIGRAM_VERSION_MINOR;
	*revision = ASCIGRAM_VERSION_REVISION;
}
