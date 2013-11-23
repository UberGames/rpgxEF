#ifndef _BG_MISC_H
#define _BG_MISC_H

#include "q_shared.h"

void BG_LanguageFilename(char *baseName,char *baseExtension,char *finalName);
char* BG_RegisterRace( const char *name );

typedef struct bgRay_s bgRay_t;
struct bgRay_s {
	vec3_t origin;
	vec3_t direction;
	vec3_t inverse_direction;
	int sign[3];
};

/**
 * @brief Prepares a ray with a given origin and direction.
 * @param ray The ray.
 * @param origin The origin.
 * @param dir The direction.
 */
void BG_PrepareRay(bgRay_t* ray, vec3_t origin, vec3_t dir);

typedef struct bgBBox_s bgBBox_t;
struct bgBBox_s {
	vec3_t bounds[2];
};

/**
 * @brief Checks is a given ray intersects with a given bounding box.
 * @param ray The ray.
 * @param bbox The bounding box.
 */
int BG_RayIntersect(bgRay_t* ray, bgBBox_t* bbox);

#endif /* _BG_MISC_H */