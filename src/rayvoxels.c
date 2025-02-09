#include "rayvoxels.h"

#include <stdlib.h>
#include <cglm/cglm.h>

float** getLineIntersectedVoxels(float vSize, vec2 vecStart, vec2 vecEnd, size_t* outCount) {
	size_t maxOutCount = 10;
	*outCount = 0;
	float** out = malloc(maxOutCount * sizeof(float*));
	for (size_t i = 0; i < maxOutCount; ++i)
		out[i] = calloc(2, sizeof(float));

	// Get vector representation of the ray
	vec2 vec;
	glm_vec2_sub(vecEnd, vecStart, vec);

	/* Algorithm steps:
	 * 1. Figure out which vertical and horizontal grid lines will be intersected with ray first.
	 *    Particularly we need to get equations of these grid lines, so that later we can put
	 *    them in one system with a ray equation to figure out their intersetion points. Luckily
	 *    these grid lines equations are very simple, for X-axis parallel grid lines it will be
	 *    y = a and for Y-axis parallel grid lines it will be x = b.
	 * 2. Calculate ray line slope and intercept values. They will be used to form a ray line
	 *    equation and then to find intersection points of ray with grid lines by solving a 
	 *    system of ray line equation and grid line equation. (y=mx+b x-slope b-intercept).
	 * 3. Calculate points of intersection by solving a system of ray line equation and grid line
	 *    equations.
	 * 4. Figure out which intersection point(calculated in the previous step) is closer, and
	 *    use it to determine the position of voxel.
	*/

	/* Step 1.
	First - figure out on which side OX and OY parallel grid lines will be hit first by ray.
	Depending on the direction of the ray, X-axis parallel grid line can be hit on the north
	or on the south side, Y-axis parallel grid line can be hit on the west or on the east side.

	On this example ray direction X is negative(-) and direction Y is negative(-) - this means
	that first X-axis parallel grid line will be intersected on the south(A is under S), and 
	first Y-axis parallel grid line will be intersected on the west(B is on the left from S).
	

        ^ OY
        |
        +-------+-------+-------+  S - ray start.
        |       |   S   |       |
        |       |  /    |       |  A - first intersection with X-axis parallel grid line
        +-------+-A-----+-------+  B - first intersection with Y-axis parallel grid line
        |       |/      |       |
        |       B       |       |
        +------/+-------+-------+
        |     / |       |       |
        |    /  |       |       |
        +---/---+-------+-------+---> OX

	*/
	vec2 side;
	side[0] = glm_signf(vec[0]);
	side[1] = glm_signf(vec[1]);
	// Side value == 0 means that ray is parallel to the one of the axis and never intersects
	// parallel grid lines

	// Get the grid position where ray starts
	vec2 head;
	glm_vec2_copy(vecStart, head);
	vec2 startCell;
	startCell[0] = floor(head[0] / vSize);
	startCell[1] = floor(head[1] / vSize);
	vec2 endCell;
	endCell[0] = floor(vecEnd[0] / vSize);
	endCell[1] = floor(vecEnd[1] / vSize);

	glm_vec2_copy(startCell, out[*outCount]);
	++(*outCount);

	// TODO fix endless loop when startCell == endCell by returning early.
	if (glm_vec2_eqv_eps(startCell, endCell)) {
		outShrink(&out, *outCount, maxOutCount);
		return out;
	}

	do {

		// Calculate grid line equations. Represented by just one value.(x = a for Y-axis parallel lines,
		// y = b for X-axis parallel lines)
		float oxY = startCell[1] * vSize + (side[1] < 0 ? 0 : side[1]) * vSize; // Here we calculate b from y = b equation
		float oyX = startCell[0] * vSize + (side[0] < 0 ? 0 : side[0]) * vSize; // Here we calculate a from x = a equation

		/* Step 2.
		 * To calculate slope and intercept values of the ray line we form a ray line equation 2 times -
		 * using ray start point and ray end point. We put both equations in the system and then we solve
		 * it for a slope.
		*/

		// System of slope-intercept line equations of a ray
		// We will be solving for slope

		// System start {
		// Equation 1:      head[1] = head[0] * slope + intercept;
		// Equation 2:      vecEnd[1] = vecEnd[0] * slope + intercept;
		// System end   }

		// Substract second equation from the first to eliminate intercept
		// head[1] - vecEnd[1] = (vecStar[0] * slope + intercept) - (vecEnd[0] * slope + intercept)
		// We can drop parentheses (signs flip if - before parentheses)
		// head[1] - vecEnd[1] = vecStar[0] * slope + intercept - vecEnd[0] * slope - intercept
		// head[1] - vecEnd[1] = vecStar[0] * slope - vecEnd[0] * slope
		// head[1] - vecEnd[1] = slope * (head[0] - vecEnd[0])
		float slope = (head[1] - vecEnd[1]) / (head[0] - vecEnd[0]);
		// Now calculate intercept
		// head[1] = head[0] * slope + intercept;
		float intercept = head[1] - (head[0] * slope);

		// Step 3.
		// Now we need to calculate intersection points of ray and closest voxel grid lines that we
		// described in step 1.

		// y = x * slope + intercept;

		// oxY = oxX * slope + intercept;
		float oxX = (oxY - intercept) / slope;

		// oyY = oyX * slope + intercept;
		float oyY = oyX * slope + intercept;


		// Step 4.
		// Which point is closer?

		float oyDist2 = glm_vec2_distance2(head, (vec2){oyX, oyY});
		float oxDist2 = glm_vec2_distance2(head, (vec2){oxX, oxY});

		if (oyDist2 < oxDist2) {
			glm_vec2_copy((vec2){oyX, oyY}, head);
			startCell[0] += side[0];
		}
		else {
			glm_vec2_copy((vec2){oxX, oxY}, head);
			startCell[1] += side[1];
		}


		glm_vec2_copy(startCell, out[*outCount]);
		++(*outCount);

		if (*outCount == maxOutCount-1) {
			maxOutCount += 10;
			outExpand(&out, *outCount, maxOutCount);
		}

	} while (!glm_vec2_eqv(startCell, endCell));

	outShrink(&out, *outCount, maxOutCount);
	return out;
}

static void outExpand(float*** out, size_t currentCount, size_t newCount) {
	*out = realloc(*out, newCount * sizeof(float*));
	for (size_t i = currentCount + 1; i < newCount; ++i)
		(*out)[i] = calloc(2, sizeof(float));
}

static void outShrink(float*** out, size_t currentCount, size_t newCount) {
	for (size_t i = currentCount-1; i > newCount-1; --i)
		free(*(out)[i]);
	*out = realloc(*out, newCount * sizeof(float*));
}

