#ifndef _BG_MISC_H
#define _BG_MISC_H

#include "q_shared.h"
#include "bg_public.h"

void BG_LanguageFilename(char *baseName,char *baseExtension,char *finalName);
char* BG_RegisterRace( const char *name );

/**
 * @brief Calculates an approximate origin from a bounding box.
 * Calculates an approximate origin from a bounding box. The bounding box is
 * specified by it's minimal and maximal point.
 * @param mins [in] Minimal point of the bounding box.
 * @param maxs [in] Maxmimal point of the bounding box.
 * @param origin [out] Three dimensional vector the origin should be stored into.
 */
void BG_OriginFromBoundingBox(vec3_t mins, vec3_t maxs, vec_t* origin);

qboolean BG_ParseRankNames(char* fileName, rankNames_t rankNames[], size_t size);

void BG_LoadItemNames();

#endif /* _BG_MISC_H */