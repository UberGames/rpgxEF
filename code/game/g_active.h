#ifndef _G_ACTIVE_H
#define _G_ACTIVE_H
#include "g_local.h"

void detpack_shot( gentity_t* self, gentity_t* inflictor, gentity_t* attacker, int32_t damage, int32_t meansOfDeath );

qboolean PlaceDecoy(gentity_t *ent);

#endif /* _G_ACTIVE_H */