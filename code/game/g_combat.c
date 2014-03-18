// Copyright (C) 1999-2000 Id Software, Inc.
//
// g_combat.c

#include "g_combat.h"
#include "g_local.h"
#include "g_breakable.h" //RPG-X | GSIO01 | 09/05/2009: needed by G_Repair
#include "g_main.h"
#include "g_cmds.h"
#include "g_client.h"
#include "g_items.h"
#include "g_lua.h"
#include "g_logger.h"
#include "g_missile.h"
#include "g_spawn.h"

void G_Combat_GibEntity(gentity_t* self, int32_t killer) {

	G_Assert(self, (void)0);

	// Start Disintegration
	G_AddEvent(self, EV_EXPLODESHELL, killer);
	self->takedamage = qfalse;
	self->s.eType = ET_INVISIBLE;
	self->r.contents = 0;
}

#define	BORG_ADAPT_NUM_HITS 10

/**
 * \brief Checks if borg have adapted to a specified damage type.
 * \param targ The target.
 * \param mod The damage type.
 */
static qboolean G_Combat_CheckBorgAdaptation(gentity_t* targ, int32_t mod) {
	int32_t	weapon = 0;

	G_Assert(targ, qfalse);
	G_Assert(targ->client, qfalse);

	switch (mod) {
		//other kinds of damage
		case MOD_UNKNOWN:
		case MOD_WATER:
		case MOD_SLIME:
		case MOD_LAVA:
		case MOD_CRUSH:
		case MOD_TELEFRAG:
		case MOD_FALLING:
		case MOD_SUICIDE:
		case MOD_RESPAWN:
		case MOD_TARGET_LASER:
		case MOD_TRIGGER_HURT:
		case MOD_DETPACK:
		case MOD_MAX:
		case MOD_KNOCKOUT:
		case MOD_EXPLOSION:
			return qfalse;
			break;
			// Trek weapons
		case MOD_PHASER:
		case MOD_PHASER_ALT:
			weapon = WP_5;
			break;
		case MOD_CRIFLE:
		case MOD_CRIFLE_SPLASH:
		case MOD_CRIFLE_ALT:
		case MOD_CRIFLE_ALT_SPLASH:
			weapon = WP_6;
			break;
		case MOD_SCAVENGER:
		case MOD_SCAVENGER_ALT:
		case MOD_SCAVENGER_ALT_SPLASH:
		case MOD_SEEKER:
			weapon = WP_4;
			break;
		case MOD_STASIS:
		case MOD_STASIS_ALT:
			weapon = WP_10;
			break;
		case MOD_GRENADE:
		case MOD_GRENADE_ALT:
		case MOD_GRENADE_SPLASH:
		case MOD_GRENADE_ALT_SPLASH:
			weapon = WP_8;
			break;
		case MOD_TETRION:
		case MOD_TETRION_ALT:
			weapon = WP_7;
			break;
		case MOD_DREADNOUGHT:
		case MOD_DREADNOUGHT_ALT:
			weapon = WP_13;
			break;
		case MOD_QUANTUM:
		case MOD_QUANTUM_SPLASH:
		case MOD_QUANTUM_ALT:
		case MOD_QUANTUM_ALT_SPLASH:
			weapon = WP_9;
			break;
		case MOD_IMOD:
		case MOD_IMOD_ALT:
			weapon = WP_3;
			break;
		case MOD_ASSIMILATE:
		case MOD_BORG:
		case MOD_BORG_ALT:
			return qtrue;
			break;
	}

	level.borgAdaptHits[weapon]++;
	switch (weapon) {
		case WP_5:
			if (level.borgAdaptHits[WP_5] > rpg_adaptPhaserHits.integer)
				return qtrue;
			break;
		case WP_6:
			if (level.borgAdaptHits[WP_6] > rpg_adaptCrifleHits.integer)
				return qtrue;
			break;
		case WP_10:
			if (level.borgAdaptHits[WP_10] > rpg_adaptDisruptorHits.integer)
				return qtrue;
			break;
		case WP_8:
			if (level.borgAdaptHits[WP_8] > rpg_adaptGrenadeLauncherHits.integer)
				return qtrue;
			break;
		case WP_7:
			if (level.borgAdaptHits[WP_7] > rpg_adaptTR116Hits.integer)
				return qtrue;
			break;
		case WP_9:
			if (level.borgAdaptHits[WP_9] > rpg_adaptPhotonHits.integer)
				return qtrue;
			break;
		default:
			return qfalse;
	}
	return qfalse;
}

/**
 * \brief Determines body the location damage was dealt to.
 * \param point Hit point.
 * \param targ Target entity.
 * \param attacker The attacker.
 * \param take Determines whether the target entity can take damage.
 */
static int32_t G_Combat_LocationDamage(vec3_t point, gentity_t* targ, gentity_t* attacker, int32_t take) {
	vec3_t bulletPath = { 0, 0, 0 };
	vec3_t bulletAngle = { 0, 0, 0 };
	int32_t clientHeight = 0;
	int32_t clientFeetZ = 0;
	int32_t clientRotation = 0;
	int32_t bulletHeight = 0;
	int32_t bulletRotation = 0;
	int32_t impactRotation = 0;


	// First things first.  If we're not damaging them, why are we here? 
	if (take == 0) {
		return 0;
	}

	// Point[2] is the REAL world Z. We want Z relative to the clients feet

	// Where the feet are at [real Z]
	clientFeetZ = targ->r.currentOrigin[2] + targ->r.mins[2];
	// How tall the client is [Relative Z]
	clientHeight = targ->r.maxs[2] - targ->r.mins[2];
	// Where the bullet struck [Relative Z]
	bulletHeight = point[2] - clientFeetZ;

	// Get a vector aiming from the client to the bullet hit 
	VectorSubtract(targ->r.currentOrigin, point, bulletPath);
	// Convert it into PITCH, ROLL, YAW
	vectoangles(bulletPath, bulletAngle);

	clientRotation = targ->client->ps.viewangles[YAW];
	bulletRotation = bulletAngle[YAW];

	impactRotation = abs(clientRotation - bulletRotation);

	impactRotation += 45; // just to make it easier to work with
	impactRotation = impactRotation % 360; // Keep it in the 0-359 range

	if (impactRotation < 90) {
		targ->client->lasthurt_location = LOCATION_BACK;
	} else if (impactRotation < 180) {
		targ->client->lasthurt_location = LOCATION_RIGHT;
	} else if (impactRotation < 270) {
		targ->client->lasthurt_location = LOCATION_FRONT;
	} else if (impactRotation < 360) {
		targ->client->lasthurt_location = LOCATION_LEFT;
	} else {
		targ->client->lasthurt_location = LOCATION_NONE;
	}

	// The upper body never changes height, just distance from the feet
	if (bulletHeight > clientHeight - 2) {
		targ->client->lasthurt_location |= LOCATION_HEAD;
	} else if (bulletHeight > clientHeight - 8) {
		targ->client->lasthurt_location |= LOCATION_FACE;
	} else if (bulletHeight > clientHeight - 10) {
		targ->client->lasthurt_location |= LOCATION_SHOULDER;
	} else if (bulletHeight > clientHeight - 16) {
		targ->client->lasthurt_location |= LOCATION_CHEST;
	} else if (bulletHeight > clientHeight - 26) {
		targ->client->lasthurt_location |= LOCATION_STOMACH;
	} else if (bulletHeight > clientHeight - 29) {
		targ->client->lasthurt_location |= LOCATION_GROIN;
	} else if (bulletHeight < 4) {
		targ->client->lasthurt_location |= LOCATION_FOOT;
	} else {
		// The leg is the only thing that changes size when you duck,
		// so we check for every other parts RELATIVE location, and
		// whats left over must be the leg. 
		targ->client->lasthurt_location |= LOCATION_LEG;
	}

	// Check the location ignoring the rotation info
	switch ((targ->client->lasthurt_location & ~(LOCATION_BACK | LOCATION_LEFT | LOCATION_RIGHT | LOCATION_FRONT)) != 0) {
		case LOCATION_HEAD:
			take *= 1.8;
			break;
		case LOCATION_FACE:
			if ((targ->client->lasthurt_location & LOCATION_FRONT) != 0) {
				take *= 5.0; // Faceshots REALLY suck
			} else {
				take *= 1.8;
			}
			break;
		case LOCATION_SHOULDER:
			if ((targ->client->lasthurt_location & (LOCATION_FRONT | LOCATION_BACK)) != 0) {
				take *= 1.4; // Throat or nape of neck
			} else {
				take *= 1.1; // Shoulders
			}
			break;
		case LOCATION_CHEST:
			if ((targ->client->lasthurt_location & (LOCATION_FRONT | LOCATION_BACK)) != 0) {
				take *= 1.3; // Belly or back
			} else {
				take *= 0.8; // Arms
			}
			break;
		case LOCATION_STOMACH:
			take *= 1.2;
			break;
		case LOCATION_GROIN:
			if ((targ->client->lasthurt_location & LOCATION_FRONT) != 0) {
				take *= 1.3; // Groin shot
			}
			break;
		case LOCATION_LEG:
			take *= 0.7;
			break;
		case LOCATION_FOOT:
			take *= 0.5;
			break;

	}

	return take;
}

void G_Combat_Damage(gentity_t* targ, gentity_t* inflictor, gentity_t* attacker, vec3_t dir, vec3_t point, int32_t damage, int32_t dflags, int32_t mod) {
	int32_t take = 0;
	int32_t knockback = 0;
	qboolean bFriend = qfalse;
	gclient_t* client = NULL;

	G_Assert(targ, (void)0);

#ifdef G_LUA
	if (targ->luaHurt && targ->client == NULL) {
		LuaHook_G_EntityHurt(targ->luaHurt, targ->s.number, inflictor->s.number, attacker->s.number);
	}
#endif

	if (!targ->takedamage) {
		return;
	}

	// the intermission has allready been qualified for, so don't
	// allow any extra scoring
	if (level.intermissionQueued != 0) {
		return;
	}

	if (inflictor == NULL) {
		inflictor = &g_entities[ENTITYNUM_WORLD];
	}

	if (attacker == NULL) {
		attacker = &g_entities[ENTITYNUM_WORLD];
	}

	// shootable doors / buttons don't actually have any health
	if (((targ->s.eType == ET_MOVER) && (targ->type != ENT_FUNC_BREAKABLE) && (targ->type != ENT_MISC_MODEL_BREAKABLE)) ||
		((targ->s.eType == ET_MOVER_STR) && (targ->type != ENT_FUNC_BREAKABLE) && (targ->type != ENT_MISC_MODEL_BREAKABLE))) //RPG-X | GSIO01 | 13/05/2009 
	{
		if (targ->type == ENT_FUNC_FORCEFIELD) {
			if (targ->pain != NULL) {
				targ->pain(targ, inflictor, take);
			}
		} else if ((targ->use != NULL) && ((targ->moverState == MOVER_POS1) || (targ->moverState == ROTATOR_POS1)) && (targ->type != ENT_FUNC_DOOR) && (targ->type != ENT_FUNC_DOOR_ROTATING)) {
			targ->use(targ, inflictor, attacker);
		}
		return;
	}

	//RPG-X | GSIO01 | 08/05/2009: as we put borg adaption back in we need this again
	if (rpg_borgAdapt.integer > -1 && G_Combat_CheckBorgAdaptation(targ, mod) && G_Client_IsBorg(targ)) {
		//flag targ for adaptation effect
		targ->client->ps.powerups[PW_BORG_ADAPT] = level.time + 250;
		if (rpg_adaptUseSound.integer == 1) {
			G_AddEvent(targ, EV_ADAPT_SOUND, 0);
		}
		return;
	}

	// multiply damage times dmgmult
	damage *= g_dmgmult.value;

	// reduce damage by the attacker's handicap value
	// unless they are rocket jumping
	if (attacker->client != NULL && attacker != targ) {
		damage = damage * attacker->client->ps.stats[STAT_MAX_HEALTH] / 100;
	}

	client = targ->client;

	if (client != NULL) {
		if (client == NULL || client->noclip) {
			return;
		}
	}

	if (dir == NULL) {
		dflags |= DAMAGE_NO_KNOCKBACK;
	} else {
		VectorNormalize(dir);
	}

	knockback = damage;
	if (knockback > 200) {
		knockback = 200;
	}
	if (targ->flags & FL_NO_KNOCKBACK) {
		knockback = 0;
	}
	if (dflags & DAMAGE_NO_KNOCKBACK) {
		knockback = 0;
	}

	knockback = floor(knockback * g_dmgmult.value);

	// figure momentum add, even if the damage won't be taken
	if (knockback && targ->client != NULL) {
		//if it's non-radius damage knockback from a teammate, don't do it if the damage won't be taken
		if ((dflags & DAMAGE_ALL_TEAMS) != 0 || (dflags & DAMAGE_RADIUS) != 0 || attacker->client == NULL) {
			double mass = 200;
			vec3_t kvel = { 0, 0, 0 };

			if (targ->client->ps.powerups[PW_FLIGHT] != 0) {
				mass *= 0.375;
			}

			if (dir != NULL) {
				VectorScale(dir, g_knockback.value * (double)knockback / mass, kvel);
				VectorAdd(targ->client->ps.velocity, kvel, targ->client->ps.velocity);
			}

			// set the timer so that the other client can't cancel
			// out the movement immediately
			if (targ->client->ps.pm_time == 0) {
				int32_t	t = knockback * 2;

				if (t < 50) {
					t = 50;
				}
				if (t > 200) {
					t = 200;
				}
				targ->client->ps.pm_time = t;
				targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
			}
		}
	}

	// check for godmode
	if ((targ->flags & FL_GODMODE) != 0) {
		return;
	}

	// always give half damage if hurting self
	// calculated after knockback, so rocket jumping works
	if (rpg_selfdamage.integer != 0) {
		if (targ == attacker) {
			damage *= 0.5;
		}
		if (damage < 1) {
			damage = 1;
		}
	} else {
		if (targ == attacker) {
			damage *= 0.0;
		}
		if (damage < 1) {
			damage = 0;
		}
	}

	take = damage;

	// save some from armor
	//RPG-X: - RedTechie No armor in RPG
	//asave = CheckArmor (targ, take, dflags);
	//take -= asave;

	if (g_debugDamage.integer != 0) {
		G_Printf("%i: client:%i health:%i damage:%i armor:<n/a>\n", level.time, targ->s.number, targ->health, take);
	}

	// add to the damage inflicted on a player this frame
	// the total will be turned into screen blends and view angle kicks
	// at the end of the frame
	if (client != NULL) {
		if (attacker != NULL) {
			client->ps.persistant[PERS_ATTACKER] = attacker->s.number;
		} else {
			client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;
		}
		//RPG-X: - RedTechie no armor in RPG
		client->damage_blood += take;
		client->damage_knockback += knockback;
		if (dir != NULL) {
			VectorCopy(dir, client->damage_from);
			client->damage_fromWorld = qfalse;
		} else {
			VectorCopy(targ->r.currentOrigin, client->damage_from);
			client->damage_fromWorld = qtrue;
		}
	}

	if (targ->client != NULL) {
		// set the last client who damaged the target
		targ->client->lasthurt_client = attacker->s.number;
		targ->client->lasthurt_mod = mod;

		// Modify the damage for location damage
		if (point != NULL && targ != NULL && targ->health > 1 && attacker != NULL && take != 0) {
			take = G_Combat_LocationDamage(point, targ, attacker, take);
		} else {
			targ->client->lasthurt_location = LOCATION_NONE;
		}
	}

	// do the damage
	if (take > 0) {
		// add to the attacker's hit counter
		if ((MOD_TELEFRAG != mod) && attacker->client != NULL && targ != attacker && targ->health > 0) {//don't telefrag since damage would wrap when sent as a short and the client would think it's a team dmg.
			if (bFriend) {
				attacker->client->ps.persistant[PERS_HITS] -= damage;
			} else if (targ->classname != NULL && strcmp(targ->classname, "holdable_shield") == 0 && strcmp(targ->classname, "holdable_detpack") == 0) {
				attacker->client->ps.persistant[PERS_HITS] += damage;
			}
		}

		targ->health = targ->health - take;

		//RPG-X: RedTechie - If medicrevive is on then health only goes down to 1 so we can simulate fake death
		if ((rpg_medicsrevive.integer == 1) && (targ->type != ENT_FUNC_BREAKABLE) && (targ->type != ENT_MISC_MODEL_BREAKABLE)) {
			if (targ->health <= 0) {
				targ->health = 1;
			}
		} else {
			if (rpg_medicsrevive.integer != 1) {
				if (targ->health == 1) { //RPG-X: RedTechie: Ok regular die now kills the player at 1 health not 0
					targ->health = 0;
				}
			}
		}

		if (targ->client != NULL) {
			targ->client->ps.stats[STAT_HEALTH] = targ->health;
		}

		//RPG-X: RedTechie - Custum medicrevive code
		if (rpg_medicsrevive.integer == 1 && targ->s.eType == ET_PLAYER) {
			if (targ->health == 1) { //TiM : Added Client to try and fix this stupid crashy bug
				client->ps.stats[STAT_WEAPONS] = (1 << WP_0); //?!!!!!
				client->ps.stats[STAT_HOLDABLE_ITEM] = HI_NONE;
				targ->health = 1;
				G_Client_Die(targ, inflictor, attacker, take, mod);
			}
		} else {
			if (targ->health <= 0) {
				if (client)
					targ->flags |= FL_NO_KNOCKBACK;

				if (targ->health < -999)
					targ->health = -999;

#ifdef G_LUA
				if (targ->luaDie && targ->client == NULL) {
					LuaHook_G_EntityDie(targ->luaDie, targ->s.number, inflictor->s.number, attacker->s.number, take, mod);
				}
#endif

				targ->enemy = attacker;
				targ->die(targ, inflictor, attacker, take, mod);
				return;
			}

			if (targ->pain != NULL) {
				targ->pain(targ, attacker, take);
			}
		}
		G_LogWeaponDamage(attacker->s.number, mod, take);
	}

}

qboolean G_Combat_CanDamage(gentity_t* targ, vec3_t origin) {
	vec3_t dest = { 0, 0, 0 };
	vec3_t midpoint = { 0, 0, 0 };
	trace_t	tr;

	// use the midpoint of the bounds instead of the origin, because
	// bmodels may have their origin is 0,0,0
	VectorAdd(targ->r.absmin, targ->r.absmax, midpoint);
	VectorScale(midpoint, 0.5, midpoint);

	VectorCopy(midpoint, dest);
	memset(&tr, 0, sizeof(trace_t));
	trap_Trace(&tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0) {
		return qtrue;
	}

	// this should probably check in the plane of projection, 
	// rather than in world coordinate, and also include Z
	VectorCopy(midpoint, dest);
	dest[0] += 15.0;
	dest[1] += 15.0;
	memset(&tr, 0, sizeof(trace_t));
	trap_Trace(&tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0) {
		return qtrue;
	}

	VectorCopy(midpoint, dest);
	dest[0] += 15.0;
	dest[1] -= 15.0;
	memset(&tr, 0, sizeof(trace_t));
	trap_Trace(&tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0) {
		return qtrue;
	}

	VectorCopy(midpoint, dest);
	dest[0] -= 15.0;
	dest[1] += 15.0;
	memset(&tr, 0, sizeof(trace_t));
	trap_Trace(&tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0) {
		return qtrue;
	}

	VectorCopy(midpoint, dest);
	dest[0] -= 15.0;
	dest[1] -= 15.0;
	memset(&tr, 0, sizeof(trace_t));
	trap_Trace(&tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0) {
		return qtrue;
	}

	return qfalse;
}

qboolean G_Combat_RadiusDamage(vec3_t origin, gentity_t* attacker, double damage, double radius, gentity_t* ignore, int32_t dflags, int32_t mod) {
	double points = 0;
	double dist = 0;
	qboolean hitClient = qfalse;
	vec3_t mins = { 0, 0, 0 };
	vec3_t maxs = { 0, 0, 0 };
	vec3_t dir = { 0, 0, 0 };
	vec3_t v = { 0, 0, 0 };
	int32_t entityList[MAX_GENTITIES];
	int32_t numListedEntities = 0;
	int32_t i = 0;
	int32_t e = 0;
	gentity_t* ent = NULL;

	if (radius < 1) {
		radius = 1;
	}

	for (i = 0; i < 3; i++) {
		mins[i] = origin[i] - radius;
		maxs[i] = origin[i] + radius;
	}

	numListedEntities = trap_EntitiesInBox(mins, maxs, entityList, MAX_GENTITIES);

	for (e = 0; e < numListedEntities; e++) {
		ent = &g_entities[entityList[e]];

		if (ent == NULL) {
			continue;
		}

		if (ent == ignore) {
			continue;
		}

		if (!ent->takedamage) {
			continue;
		}

		if (ignore != NULL && ignore->parent != NULL && ent->parent == ignore->parent) {
			if (ignore->think == tripwireThink && ent->think == tripwireThink) {//your own tripwires do not fire off other tripwires of yours.
				continue;
			}
		}

		// find the distance from the edge of the bounding box
		for (i = 0; i < 3; i++) {
			if (origin[i] < ent->r.absmin[i]) {
				v[i] = ent->r.absmin[i] - origin[i];
			} else if (origin[i] > ent->r.absmax[i]) {
				v[i] = origin[i] - ent->r.absmax[i];
			} else {
				v[i] = 0;
			}
		}

		dist = VectorLength(v);
		if (dist >= radius) {
			continue;
		}

		points = damage * (1.0 - dist / radius);

		if (!G_Combat_CanDamage(ent, origin)) {
			//no LOS to ent
			if ((dflags & DAMAGE_HALF_NOTLOS) == 0) {
				//not allowed to do damage without LOS
				continue;
			} else {
				//do 1/2 damage if no LOS but within rad
				points *= 0.5;
			}
		}

		if (G_Weapon_LogAccuracyHit(ent, attacker)) {
			hitClient = qtrue;
		}
		VectorSubtract(ent->r.currentOrigin, origin, dir);
		// push the center of mass higher than the origin so players
		// get knocked into the air more
		dir[2] += 24;
		G_Combat_Damage(ent, NULL, attacker, dir, origin, (int)points, dflags | DAMAGE_RADIUS, mod);
	}

	return hitClient;
}

void G_Combat_Repair(gentity_t* ent, gentity_t* tr_ent, double rate) {
	double distance = 0;
	double max = 0;
	vec3_t help = { 0, 0, 0 };
	vec3_t forward = { 0, 0, 0 };
	int32_t i = 0;

	// if count isn't 0 the breakable is not damaged and if target is no breakable it does not make sense to go on
	if ((tr_ent->count != 0) || strstr(tr_ent->classname, "breakable") == 0) {
		return;
	}

	if ((tr_ent->spawnflags & 256) == 0) { // no REPAIRABLE flag set
		return;
	}

	// check if player is near the breakable
	if ((tr_ent->spawnflags & 512) != 0) {
		VectorSubtract(tr_ent->s.angles2, ent->r.currentOrigin, help);
		max = tr_ent->n00bCount;
	} else {
		VectorSubtract(tr_ent->s.origin, ent->r.currentOrigin, help);
		for (i = 0; i < 3; i++) {
			if (tr_ent->r.maxs[i] > max) {
				max = tr_ent->r.maxs[i];
			}
		}
	}
	distance = VectorLength(help);

	//G_Printf("goodDst=%f, curDst=%f\n", 80 + max, distance);
	if (distance > 80 + max) {
		return;
	}

	// check if the player is facing it
	AngleVectors(ent->client->ps.viewangles, forward, NULL, NULL);
	if (DotProduct(help, forward) < 0.4) {
		return;
	}

	// check wheter the breakable still needs to be repaired
	if (tr_ent->health < tr_ent->damage) {
		// still not repaired of let's go on
		tr_ent->health += rate;
		if (tr_ent->health >= tr_ent->damage) {//we're maxed out after this cycle, reenstate
			tr_ent->health = tr_ent->damage;
			if (tr_ent->target) {
				G_UseTargets2(tr_ent, tr_ent, tr_ent->target);
			}
			if (tr_ent->type == ENT_FUNC_BREAKABLE) {
				tr_ent->s.solid = CONTENTS_BODY;
				trap_SetBrushModel(tr_ent, tr_ent->model);
				tr_ent->r.svFlags &= ~SVF_NOCLIENT;
				tr_ent->s.eFlags &= ~EF_NODRAW;
				InitBBrush(tr_ent);

				if (tr_ent->health) {
					tr_ent->takedamage = qtrue;
				}

				tr_ent->use = breakable_use;

				if (tr_ent->paintarget) {
					tr_ent->pain = breakable_pain;
				}

				tr_ent->clipmask = 0;
				tr_ent->count = 1;
			} else if (tr_ent->type == ENT_MISC_MODEL_BREAKABLE) {
				SP_misc_model_breakable(tr_ent);
			}
		}
	}
}
