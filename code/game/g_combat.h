#ifndef G_COMBAT_H_
#define G_COMBAT_H_

#include "g_local.h"

// damage flags
#define DAMAGE_RADIUS				0x00000001	//!< damage was indirect
#define DAMAGE_NO_ARMOR				0x00000002	//!< armor (shields) do not protect from this damage
#define DAMAGE_NO_KNOCKBACK			0x00000008	//!< do not affect velocity, just view angles
#define DAMAGE_NO_PROTECTION		0x00000020  //!< armor, shields, invulnerability, and godmode have no effect
#define DAMAGE_NOT_ARMOR_PIERCING	0x00000000	//!< trek: shields fully protect from this damage (for clarity)
#define DAMAGE_ARMOR_PIERCING		0x00000040	//!< trek: shields don't fully protect from this damage
#define DAMAGE_NO_INVULNERABILITY	0x00000080	//!< trek: invulnerability doesn't event protect from this damage
#define DAMAGE_HALF_NOTLOS			0x00000100	//!< trek: radius damage still does 1/2 damage to ents that do not have LOS to explosion org but are in radius
#define DAMAGE_ALL_TEAMS			0x00000200	//!< trek: damage ignores teamdamage settings

/**
* Damage all entities around an origin in a specified radius.
*
* \return Hit a client?
* \param origin Origin.
* \param attacker Attacker.
* \param damage Amount much damage.
* \param radius Radius.
* \param ignore Entity to ignore.
* \param dflags Damage flags.
* \param mod Means of death.
* \todo Replace ignore entity by list of entites.
*/
qboolean G_Combat_RadiusDamage(vec3_t origin, gentity_t* attacker, double damage, double radius, gentity_t* ignore, int32_t dflags, int32_t mod);

/**
* Repairs repairable entities.
*
* \param ent The player.
* \param tr_ent Entity to repair.
* \param rate Rate to repair with.
* \author Ubergames - GSIO01
* \date 09/05/2009
*/
void G_Combat_Repair(gentity_t* ent, gentity_t* tr_ent, double rate);

/**
* @brief Throws gibs and entity.
* @param self The entity.
* @param killer The killer.
*/
void G_Combat_GibEntity(gentity_t* self, int32_t killer);

/**
 * brief Damage an entity.
 *
 * \param targ entity that is being damaged
 * \param inflictor entity that is causing the damage
 * \param attacker entity that caused the inflictor to damage targ
 * \param dir direction of the attack for knockback
 * \param point point at which the damage is being inflicted, used for headshots
 * \param damage amount of damage being inflicted
 * \param knockback force to be applied against targ as a result of the damage
 * \param dflags these flags are used to control how G_Damage works
 * \param mod means of death
 *
 * Damage flags:
 * DAMAGE_RADIUS			damage was indirect (from a nearby explosion)
 * DAMAGE_NO_ARMOR			armor does not protect from this damage
 * DAMAGE_NO_KNOCKBACK		do not affect velocity, just view angles
 * DAMAGE_NO_PROTECTION		kills godmode, armor, everything
 */
void G_Combat_Damage(gentity_t* targ, /*@null@*/ gentity_t* inflictor, /*@null@*/ gentity_t* attacker, /*@null@*/ vec3_t dir, /*@null@*/ vec3_t point, int32_t damage, int32_t dflags, int32_t mod);

/**
 * \brief Returns qtrue if the inflictor can directly damage the target.  
 * Used for explosions and melee attacks.
 *
 * \param targ the target
 * \param origin the origin
 * \return qtrue if the inflictor can directly damage the target.  Used for explosions and melee attacks.
 */
qboolean G_Combat_CanDamage(gentity_t* targ, vec3_t origin);

#endif /* G_COMBAT_H_ */