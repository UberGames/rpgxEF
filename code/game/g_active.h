#ifndef _G_ACTIVE_H
#define _G_ACTIVE_H
#include "g_local.h"

/**
 * @brief The detonation pack has been activated.
 * @param self The detonation pack.
 * @param inflictor The inflicting entity.
 * @param attacker The attacking entity.
 * @param damage Amount of damage.
 * @param meansOfDeath The means of death.
 */
void detpack_shot( gentity_t* self, gentity_t* inflictor, gentity_t* attacker, int32_t damage, int32_t meansOfDeath );

/**
 * @brief Places a decoy of the player.
 * @param ent The player.
 */
qboolean PlaceDecoy(gentity_t *ent);

/**
 * @brief Detonate a detonation pack.
 * @param ent The detonation pack.
 */
void DetonateDetpack(gentity_t* ent);

#endif /* _G_ACTIVE_H */