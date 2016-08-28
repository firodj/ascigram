/* version.h */

#ifndef _ASCIGRAM_VERSION_H_
#define _ASCIGRAM_VERSION_H_

#ifdef __cplusplus
extern "C" {
#endif


/*************
 * CONSTANTS *
 *************/

#define ASCIGRAM_VERSION "0.0.1"
#define ASCIGRAM_VERSION_MAJOR 0
#define ASCIGRAM_VERSION_MINOR 0
#define ASCIGRAM_VERSION_REVISION 1


/*************
 * FUNCTIONS *
 *************/

void ascigram_version(int *major, int *minor, int *revision);


#ifdef __cplusplus
}
#endif

#endif /** _ASCIGRAM_VERSION_H_ */
