#ifndef G_MISSILE_H_
#define G_MISSILE_H_

#include "g_local.h"

/**
 * \brief Missile impact function.
 *
 * \param ent the missile.
 * \param trace A trace.
 */
void G_Missile_Impact(gentity_t* ent, trace_t* trace);

/**
 * \brief Tripwire think function.
 * 
 * \param ent The tripwire.
 * \todo This does not seem to fit to g_missile
 */
void tripwireThink(gentity_t* ent);

/**
 * \brief Run a missile.
 *
 * \param ent the missile
 */
void G_Missile_Run(gentity_t* ent);

/**
 * \brief Fire alient plasma projectile.
 *
 * \param The shooter.
 * \param start Start point.
 * \param aimdir Direction.
 */
/*@shared@*/ /*@null@*/  gentity_t* G_Missile_FirePlasma(gentity_t* self, vec3_t start, vec3_t aimdir);

/**
 * \brief Fire torpedo projectile.
 *
 * \param The shooter.
 * \param start Start point.
 * \param aimdir Direction.
 */
/*@shared@*/ /*@null@*/  gentity_t* G_Missile_FireQuantum(gentity_t* self, vec3_t start, vec3_t aimdir);

/**
 * \brief Fire grenade.
 *
 * \param The shooter.
 * \param start Start point.
 * \param aimdir Direction.
 */
/*@shared@*/ /*@null@*/  gentity_t* G_Missile_FireGrenade(gentity_t* self, vec3_t start, vec3_t aimdir);

/**
 * \brief Fire a rocket.
 *
 * \param The shooter.
 * \param start Start point.
 * \param dir Direction.
 */
/*@shared@*/ /*@null@*/  gentity_t* G_Missile_FirerRocket(gentity_t* self, vec3_t start, vec3_t dir);

/**
 * \brief Fire a compression rifle projectile.
 *
 * \param The shooter.
 * \param start Start point.
 * \param dir Direction.
 */
/*@shared@*/ /*@null@*/  gentity_t* G_Missile_FireComprifle(gentity_t* self, vec3_t start, vec3_t dir);

#endif /* G_MISSILE_H_ */