#ifndef BG_COLLISION_H_
#define BG_COLLISION_H_

#include "q_shared.h"

qboolean BG_LineBoxIntersection(vec3_t mins, vec3_t maxs, vec3_t origin, vec3_t dir, vec_t* hit);
qboolean BG_IsInBox(vec3_t pos, vec3_t mins, vec3_t maxs);

#endif