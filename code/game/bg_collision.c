#include "bg_collision.h"
#include "q_shared.h"

#define QUADRANT_LEFT 0
#define QUADRANT_RIGHT 1
#define QUADRANT_MIDDLE 2
qboolean BG_LineBoxIntersection(vec3_t mins, vec3_t maxs, vec3_t origin, vec3_t dir, vec_t* hit) {
	qboolean inside;
	char quadrant[3];
	int i;
	int plane;
	vec3_t maxT;
	vec3_t candidate;

	for(i = 0; i < 3; i++) {
		if(origin[i] < mins[i]) {
			quadrant[i] = QUADRANT_LEFT;
			candidate[i] = mins[i];
			inside = qfalse;
		} else if(origin[i] > maxs[i]) {
			quadrant[i] = QUADRANT_RIGHT;
			candidate[i] = maxs[i];
			inside = qfalse;
		} else {
			quadrant[i] = QUADRANT_MIDDLE;
		}
	}

	if(inside) {
		VectorCopy(origin, hit);
		return qtrue;
	}

	for(i = 0; i < 3; i++) {
		if(quadrant[i] != QUADRANT_MIDDLE && dir[0] != 0) {
			maxT[i] = (candidate[i] - origin[i]) / dir[i];
		} else {
			maxT[i] = -1;
		}
	}

	plane = 0;
	for(i = 0; i < 3; i++) {
		if(maxT[plane] < maxT[i]) {
			plane = i;
		}
	}

	if(maxT[plane] < 0) {
		return qfalse;
	}

	for(i = 0; i < 3; i++) {
		if(plane != i) {
			hit[i] = origin[i] + maxT[plane] * dir[i];
			
			if(hit[i] < mins[i] || hit[i] > maxs[i]) {
				return qfalse;
			} 
		} else {
			hit[i] = candidate[i];
		}
	}

	return qtrue;
}

qboolean BG_IsInBox(vec3_t pos, vec3_t mins, vec3_t maxs) {
	if( pos[0] <= maxs[0] && pos[0] >= mins[0] &&
		pos[1] <= maxs[1] && pos[1] >= mins[1] &&
		pos[2] <= maxs[2] && pos[2] >= mins[2]) {
			return qtrue;
	}

	return qfalse;
}
