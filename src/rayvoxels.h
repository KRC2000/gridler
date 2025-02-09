#ifndef RAYVOXELS_H
#define RAYVOXELS_H

#include <cglm/cglm.h>

float** getLineIntersectedVoxels(float vSize,
								 vec2 vecStart,
								 vec2 vecEnd,
								 size_t* outCount);

static void outExpand(float*** out, size_t currentCount, size_t newCount);
static void outShrink(float*** out, size_t currentCount, size_t newCount);

#endif
