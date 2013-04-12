/* Copyright (C) 1999-2000 Id Software, Inc.
 *
 * g_weapon.c 
 * perform the server side effects of a weapon firing
 */

#include "g_local.h"
#include "g_weapon.h"

extern void G_MissileImpact( gentity_t *ent, trace_t *trace);

#define MAX_BEAM_HITS	4

#define DMG_VAR		(flrandom(0.8,1.2))


/* Weapon damages are located up here for easy access... */
extern vmCvar_t	rpg_rifleDamage;
extern vmCvar_t	rpg_rifleAltDamage;
extern vmCvar_t	rpg_phaserDamage;
extern vmCvar_t	rpg_disruptorDamage;
extern vmCvar_t	rpg_grenadeDamage;
extern vmCvar_t	rpg_grenadeAltDamage;
extern vmCvar_t	rpg_tr116Damage;
extern vmCvar_t	rpg_photonDamage;
extern vmCvar_t	rpg_photonAltDamage;

/* Phaser */
/* I'll keep this comment just because it's funny lol :D */
/* RPG-X: TiM - Increased to a standard 0.5 second 
 * burst - Phenix GOING DOWN - TiM GOING UP we had 
 * complaints when this was put down :P */
#define	PHASER_DAMAGE			rpg_phaserDamage.integer
#define PHASER_ALT_RADIUS		80 			 /* RPG-X: TiM - Increased to a near instant kill */

/* Compression Rifle */
#define	CRIFLE_DAMAGE			rpg_rifleDamage.integer
#define CRIFLE_MAIN_SPLASH_RADIUS	64
#define CRIFLE_MAIN_SPLASH_DMG		0
#define CRIFLE_ALTDAMAGE		rpg_rifleAltDamage.integer
#define CRIFLE_ALT_SPLASH_RADIUS	32
#define CRIFLE_ALT_SPLASH_DMG		0 

/* Stasis Weapon */
#define STASIS_DAMAGE			rpg_disruptorDamage.integer

/* Grenade Launcher */
#define GRENADE_DAMAGE			rpg_grenadeDamage.integer
#define GRENADE_SPLASH_RAD		190
#define GRENADE_SPLASH_DAM		100
#define GRENADE_ALT_DAMAGE		rpg_grenadeAltDamage.integer

/* Tetrion Disruptor */
#define TETRION_DAMAGE			rpg_tr116Damage.integer
		
/* Quantum Burst */
#define QUANTUM_DAMAGE			rpg_photonDamage.integer	
#define QUANTUM_SPLASH_DAM		rpg_photonDamage.integer
#define QUANTUM_SPLASH_RAD		160
#define QUANTUM_ALT_DAMAGE		rpg_photonAltDamage.integer	
#define QUANTUM_ALT_SPLASH_DAM		rpg_photonAltDamage.integer
#define QUANTUM_ALT_SPLASH_RAD		80

void G_Weapon_SnapVectorTowards( vec3_t v, vec3_t to ) {
	int i;

	for ( i = 0 ; i < 3 ; i++ ) {
		if ( to[i] <= v[i] ) {
			v[i] = (int)v[i];
		} else {
			v[i] = (int)v[i] + 1;
		}
	}
}




/*
----------------------------------------------
	PLAYER WEAPONS
----------------------------------------------
*/

/*
----------------------------------------------
	HYPERSPANNER
----------------------------------------------
*/


#define HYPERSPANNER_RATE		2
#define HYPERSPANNER_ALT_RATE		4

/**
 * \brief Handles weapon fire of the Hyperspanner.
 *
 * Handles weapon fire of the Hyperspanner.
 * 
 * @param ent the player
 * @param alt_fire was this alt fire mode?
 */
static void WP_FireHyperspanner(gentity_t *ent, qboolean alt_fire) {
	float		modifier;
	struct list validEnts;
	list_iter_p iter;
	container_p cont;
	gentity_t*	e = NULL;
	gentity_t*	nearest = NULL;
	int			count = 0;
	float		nearestd = 65000;
	vec3_t		dVec, end;
	vec3_t		mins = { -40, -40, 0 }, maxs = { 40, 40, 0 };
	struct list	classnames;

	/* prepare lists */
	list_init(&classnames, free);
	list_init(&validEnts, free);
	list_append(&classnames, "func_breakable", LT_STRING, strlen("func_breakable")+1);
	list_append(&classnames, "misc_model_breakable", LT_STRING, strlen("misc_model_breakable")+1);

	/* find all vlaid entities in range */
	count = G_RadiusListOfTypes(&classnames, ent->r.currentOrigin, 512, NULL, &validEnts);
	list_clear(&classnames);
	//G_Printf("Found %d possible candidates\n", count);
	if(count) {
		trace_t tr;

		iter = list_iterator(&validEnts, LIST_FRONT);
		for(cont = list_next(iter); cont != NULL; cont = list_next(iter)) {
			e = cont->data;

			// TODO: fix problems with small distance
			if(e->spawnflags & 512) {
				VectorSubtract(ent->r.currentOrigin, e->s.angles2, dVec);
				VectorMA(e->s.angles2, 1024, dVec, end);
				trap_Trace(&tr, e->s.angles2, mins, maxs, end, e->s.number, MASK_SHOT);
			} else {
				VectorSubtract(ent->r.currentOrigin, e->s.origin, dVec);
				VectorMA(e->s.origin, 1024, dVec, end);
				trap_Trace(&tr, e->s.origin, mins, maxs, end, e->s.number, MASK_SHOT);
			}
			//G_Printf("Checking entity: %d\n", i);
			if(tr.entityNum != ent->s.number) {
				continue;
			}
			//G_Printf("Nothing is blocking view ...\n");
			if(e->spawnflags & 512) {
				VectorSubtract(ent->r.currentOrigin, e->s.angles2, dVec);
			} else {
				VectorSubtract(ent->r.currentOrigin, e->s.origin, dVec);
			}
			if(VectorLength(dVec) < nearestd) {
				nearest = e;
				nearestd = VectorLength(dVec);
				//G_Printf("New nearest Entity is %d with a distance of %d\n", nearest, nearestd);
			}
		}
	} else {
		return;
	}

	if(nearest == NULL || nearest->inuse == qfalse) {
		list_clear(&validEnts);
		return;
	}

	/* determine the repair rate modifier */
	if(rpg_repairModifier.value < 0) {
		modifier = 1;
	} else {
		modifier = rpg_repairModifier.value;
	}

	/* call G_Repair */
	if(alt_fire) {
		G_Repair(ent, nearest, HYPERSPANNER_ALT_RATE * modifier);
	} else {
		G_Repair(ent, nearest, HYPERSPANNER_RATE * modifier);
	}

	list_clear(&validEnts);
}

/*
----------------------------------------------
	PHASER
----------------------------------------------
*/

#define MAXRANGE_PHASER			2048 /* This is the same as the range MAX_BEAM_RANGE	2048 */
#define NUM_PHASER_TRACES 		3
#define BEAM_VARIATION			6
#define PHASER_POINT_BLANK		96
#define PHASER_POINT_BLANK_FRAC		((float)PHASER_POINT_BLANK / (float)MAXRANGE_PHASER)

/**
 * \brief Handles weapon fire of the phaser.
 *
 * Handles weapon fire of the phaser.
 *
 * @param ent the player
 * @param alt_fire was this alt fire mode?
 */
static void WP_FirePhaser( gentity_t *ent, qboolean alt_fire )
{
	trace_t		tr;
	vec3_t		end;
	gentity_t	*traceEnt;
	int		trEnts[NUM_PHASER_TRACES], i = 0;
	float		trEntFraction[NUM_PHASER_TRACES];
	int		damage = 0;

	VectorMA (muzzle, MAXRANGE_PHASER, forward, end);
	/* Add a subtle variation to the beam weapon's endpoint */
	for (i = 0; i < 3; i ++ )
	{
		end[i] += crandom() * BEAM_VARIATION;
	}

	for (i = 0; i < NUM_PHASER_TRACES; i++)
	{
		trEnts[i] = -1;
		trEntFraction[i] = 0.0;
	}
	/* Find out who we've hit */
	trap_Trace (&tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );
	if (tr.entityNum != (MAX_GENTITIES-1))
	{
		trEnts[0] = tr.entityNum;
		trEntFraction[0] = tr.fraction;
	}
	if ( alt_fire && ent->client->ps.ammo[WP_5])
	{	/* 
		 * Use the ending point of the thin trace to do two more traces, 
		 * one on either side, for actual damaging effect.
		 */
		vec3_t	vUp = {0,0,1}, vRight;

		CrossProduct(forward, vUp, vRight);
		VectorNormalize(vRight);
		VectorCopy(tr.endpos, end);
		trap_Trace (&tr, muzzle, NULL, NULL, end, ent->s.number, (CONTENTS_PLAYERCLIP|CONTENTS_BODY) );
		if (	(tr.entityNum != (MAX_GENTITIES-1)) &&
				(tr.entityNum != trEnts[0]) )
		{
			trEnts[1] = tr.entityNum;
			trEntFraction[1] = tr.fraction;
		}
		trap_Trace (&tr, muzzle, NULL, NULL, end, ent->s.number, (CONTENTS_PLAYERCLIP|CONTENTS_BODY) );
		if (	(tr.entityNum != (MAX_GENTITIES-1)) &&
				(tr.entityNum != trEnts[0]) &&
				(tr.entityNum != trEnts[1]))
		{
			trEnts[2] = tr.entityNum;
			trEntFraction[2] = tr.fraction;
		}
	}

	for (i = 0; i < NUM_PHASER_TRACES; i++)
	{
		if (-1 == trEnts[i])
		{
			continue;
		}
		traceEnt = &g_entities[ trEnts[i] ];

		if ( traceEnt->takedamage && (rpg_dmgFlags.integer & 1) ) 
		{
			/*damage = (float)PHASER_DAMAGE*DMG_VAR*s_quadFactor;*/ /* No variance on phaser */
			damage = (float)PHASER_DAMAGE;

			if (trEntFraction[i] <= PHASER_POINT_BLANK_FRAC)
			{	/* Point blank!  Do up to double damage. */
				damage += damage * (1.0 - (trEntFraction[i]/PHASER_POINT_BLANK_FRAC));
			}
			else
			{	/* Normal range */
				damage -= (int)(trEntFraction[i]*5.0);
			}

			if (!ent->client->ps.ammo[WP_5])
			{
				damage *= .35; /* weak out-of-ammo phaser */
			}
			
			if (damage > 0)
			{
				if ( alt_fire ) 
				{
					G_Damage( traceEnt, ent, ent, forward, tr.endpos, damage, 
								DAMAGE_NO_KNOCKBACK | DAMAGE_NOT_ARMOR_PIERCING, MOD_PHASER_ALT );
				}
				else
				{
					G_Damage( traceEnt, ent, ent, forward, tr.endpos, damage, 
								DAMAGE_NO_KNOCKBACK | DAMAGE_ARMOR_PIERCING, MOD_PHASER );
				}
			}
		}
	}
}


/*
----------------------------------------------
	COMPRESSION RIFLE
----------------------------------------------
*/

#define COMPRESSION_SPREAD	100
#define MAXRANGE_CRIFLE		8192
#define CRIFLE_SIZE		1  /* RPG-X | Marcin | 04/12/2008 */

/**
 * \brief Fires a new compression rifle bullet.
 *
 * Creates a new compression rifle bullet entity.
 *
 * @param ent the player
 * @param start start point
 * @param end end point
 */
static void FirePrifleBullet( gentity_t *ent, vec3_t start, vec3_t dir )
{
	gentity_t	*bolt;

	bolt = G_Spawn();
	
	bolt->classname = "prifle_proj";
	bolt->nextthink = level.time + 10000;
	bolt->think = G_FreeEntity;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_6;
	bolt->r.ownerNum = ent->s.number;
	bolt->parent = ent;

	/* fixme  - remove */
	{
		/* Flags effect as being the full beefy version for the player */
		bolt->count = 0;
	}

	if( rpg_dmgFlags.integer & 2) 
		bolt->damage = CRIFLE_DAMAGE*DMG_VAR;
	else
		bolt->damage = 0;

	bolt->splashDamage = 0;
	bolt->splashRadius = 0;
	bolt->methodOfDeath = MOD_CRIFLE;
	bolt->clipmask = MASK_SHOT;

	/* Set the size of the missile up */
	VectorSet(bolt->r.maxs, CRIFLE_SIZE>>1, CRIFLE_SIZE, CRIFLE_SIZE>>1);
	VectorSet(bolt->r.mins, -CRIFLE_SIZE>>1, -CRIFLE_SIZE, -CRIFLE_SIZE>>1);

	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - 10; 			/* move a bit on the very first frame */
	VectorCopy( start, bolt->s.pos.trBase );
	SnapVector( bolt->s.pos.trBase );			/* save net bandwidth */
	VectorScale( dir, rpg_rifleSpeed.integer, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			/* save net bandwidth */
	VectorCopy( start, bolt->r.currentOrigin);
}

/**
 * \brief Handles weapon fire of the compression rifle.
 *
 * Handles weapon fire of the compression rifle.
 *
 * @param ent the player
 * @param alt_fire was this alt fire mode?
 */
static void WP_FireCompressionRifle ( gentity_t *ent, qboolean alt_fire )
{
	if ( !alt_fire )
	{
		vec3_t	dir, angles, temp_ang, temp_org;
		vec3_t	start;

		VectorCopy( forward, dir );
		VectorCopy( muzzle, start );


		vectoangles( dir, angles );
		VectorSet( temp_ang, angles[0], angles[1], angles[2] );
		AngleVectors( temp_ang, dir, NULL, NULL );

		/* FIXME:  These offsets really don't work like they should */
		VectorMA( start, 0, right, temp_org );
		VectorMA( temp_org, 0, up, temp_org );
		FirePrifleBullet( ent, temp_org, dir ); /* temp_org */

		G_LogWeaponFire(ent->s.number, WP_6);
	}
	else
	{
		trace_t		tr;
		vec3_t		end;
		gentity_t	*traceEnt;
		int		damage = 0;

		VectorMA (muzzle, MAXRANGE_PHASER, forward, end);

		/* Find out who we've hit */
		trap_Trace (&tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );
		
		if (tr.entityNum == (MAX_GENTITIES-1))
		{
			return;
		}

		traceEnt = &g_entities[ tr.entityNum ];

		if ( traceEnt->takedamage && (rpg_dmgFlags.integer & 2) ) 
		{
			damage = (float)PHASER_DAMAGE;

			if (tr.fraction <= PHASER_POINT_BLANK_FRAC)
			{	/* Point blank!  Do up to double damage. */
				damage += damage * (1.0 - (tr.fraction/PHASER_POINT_BLANK_FRAC));
			}
			else
			{	/* Normal range */
				damage -= (int)(tr.fraction*5.0);
			}
			
			if (damage > 0)
			{
				G_Damage( traceEnt, ent, ent, forward, tr.endpos, damage, 
					DAMAGE_NO_KNOCKBACK | DAMAGE_ARMOR_PIERCING, MOD_CRIFLE_ALT ); /* GSIO01: was MOD_PHASER */
			}
		}
	}
}

/*
----------------------------------------------
	SCAVENGER
----------------------------------------------
*/
#define SCAV_SIZE		3
#define SCAV_ALT_SIZE		6

/*
----------------------------------------------
	STASIS
----------------------------------------------
*/
	
#define STASIS_SPREAD			0.085f	/* Roughly equivalent to sin(5 deg).*/
#define STASIS_MAIN_MISSILE_BIG		1 
#define STASIS_MAIN_MISSILE_SMALL	1 
#define STASIS_ALT_RIGHT_OFS		0.10
#define STASIS_ALT_UP_OFS		0.02
#define STASIS_ALT_MUZZLE_OFS		1
#define MAXRANGE_ALT_STASIS		4096

/**
 * \brief Fires a disruptor missile.
 *
 * Creates a disruptor bullet entity and sets it up.
 *
 * @param the player
 * @param origin the start point
 * @param dir the direction
 * @param size the size
 */
static void FireDisruptorMissile( gentity_t *ent, vec3_t origin, vec3_t dir, int size )
{
	gentity_t *bolt;
	int	  boltsize;

	bolt = G_Spawn();
	bolt->classname = "disruptor_projectile";

	
	bolt->nextthink = level.time + 10000;
	bolt->think = G_FreeEntity;

	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_10;
	bolt->r.ownerNum = ent->s.number;
	bolt->parent = ent;
	if ( rpg_dmgFlags.integer & 32 )
	{
		bolt->damage = STASIS_DAMAGE*DMG_VAR;
	}
	else
	{
		bolt->damage = 0;
	}
	bolt->splashDamage = 0;
	bolt->splashRadius = 0;
	bolt->methodOfDeath = MOD_STASIS_ALT; /* GSIO01: was MOD_TETRION_ALT */
	bolt->clipmask = MASK_SHOT;

	/* Set the size of the missile up */
	boltsize=3*size;
	VectorSet(bolt->r.maxs, boltsize>>1, boltsize, boltsize>>1);
	boltsize=-boltsize;
	VectorSet(bolt->r.mins, boltsize>>1, boltsize, boltsize>>1);

	/* There are going to be a couple of different sized projectiles, so store 'em here */
	bolt->count = size;
	/* kef -- need to keep the size in something that'll reach the cgame side */
	bolt->s.time2 = size;

	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time;
	VectorCopy( origin, bolt->s.pos.trBase );
	SnapVector( bolt->s.pos.trBase );			/* save net bandwidth */
	
	VectorScale( dir, rpg_disruptorSpeed.integer + ( 50 * size ), bolt->s.pos.trDelta ); /* RPG-X | Marcin | 05/12/2008 */
	
	SnapVector( bolt->s.pos.trDelta );			/* save net bandwidth */
	VectorCopy (origin, bolt->r.currentOrigin);
	/* Used by trails */
	VectorCopy (origin, bolt->pos1 );
	VectorCopy (origin, bolt->pos2 );
	/* kef -- need to keep the origin in something that'll reach the cgame side */
	VectorCopy(origin, bolt->s.angles2);
	SnapVector( bolt->s.angles2 );			/* save net bandwidth */
}

/**
 * \brief Handles firing of the dirsuptor.
 *
 * Handles firing of the disruptor.
 *
 * @ent the player
 * @alt_fire was this alt fire mode?
 */
static void WP_FireDisruptor( gentity_t *ent, qboolean alt_fire )
{
	/* This was moved out of the FireWeapon switch statement below to keep things more consistent */
	if ( !alt_fire )
	{
		trace_t		tr;
		vec3_t		end;
		gentity_t	*traceEnt;
		//int			i = 0;
		int			damage = 0;

		VectorMA (muzzle, MAXRANGE_PHASER, forward, end);

		/* Find out who we've hit */
		trap_Trace (&tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );
		
		if (tr.entityNum == (MAX_GENTITIES-1))
		{
			return;
		}

		traceEnt = &g_entities[ tr.entityNum ];

		if ( traceEnt->takedamage && (rpg_dmgFlags.integer & 32) ) 
		{
			damage = (float)PHASER_DAMAGE;

			if (tr.fraction <= PHASER_POINT_BLANK_FRAC)
			{	/* Point blank!  Do up to double damage. */
				damage += damage * (1.0 - (tr.fraction/PHASER_POINT_BLANK_FRAC));
			}
			else
			{	/* Normal range */
				damage -= (int)(tr.fraction*5.0);
			}
			
			if (damage > 0)
			{
				G_Damage( traceEnt, ent, ent, forward, tr.endpos, damage, 
					DAMAGE_NO_KNOCKBACK | DAMAGE_ARMOR_PIERCING, MOD_STASIS ); /* GSIO01: was MOD_TETRION_ALT */
			}
		}
	}
	else
	{
		FireDisruptorMissile(ent, muzzle, forward, STASIS_MAIN_MISSILE_BIG);
	}

	G_LogWeaponFire(ent->s.number, WP_10);
}

/*
----------------------------------------------
	GRENADE LAUNCHER
----------------------------------------------
*/

#define GRENADE_VELOCITY		1000
#define GRENADE_TIME			2000
#define GRENADE_SIZE			4
#define GRENADE_ALT_VELOCITY	1200
#define GRENADE_ALT_TIME		2500

#define SHRAPNEL_DAMAGE			30
#define SHRAPNEL_DISTANCE		4096
#define SHRAPNEL_BITS			6
#define SHRAPNEL_RANDOM			3
#define SHRAPNEL_SPREAD			0.75

/**
 * \brief Exploding a grenade.
 *
 * Handles all damage and visual effects for a exploding grenade.
 *
 * @param ent the grenade
 */
static void grenadeExplode( gentity_t *ent )
{
	vec3_t		pos;

	VectorSet( pos, ent->r.currentOrigin[0], ent->r.currentOrigin[1], ent->r.currentOrigin[2] + 8 );

	G_TempEntity( pos, EV_GRENADE_EXPLODE );

	/* splash damage (doesn't apply to person directly hit) */
	if ( ent->splashDamage ) {
		G_RadiusDamage( pos, ent->parent, ent->splashDamage, ent->splashRadius, 
			NULL, 0, ent->splashMethodOfDeath ); 
	}
	G_FreeEntity( ent );
}

/**
 * \brief Handles grenade shrapnels.
 *
 * Handles grenade shrapnels.
 *
 * @param ent the grenade
 */
void grenadeSpewShrapnel( gentity_t *ent )
{
	gentity_t	*tent = NULL;

	tent = G_TempEntity( ent->r.currentOrigin, EV_GRENADE_SHRAPNEL_EXPLODE );
	tent->s.eventParm = DirToByte(ent->pos1);

	// just do radius dmg for altfire
	G_RadiusDamage( ent->r.currentOrigin, ent->parent, ent->splashDamage, ent->splashRadius, 
		ent, 0, ent->splashMethodOfDeath );

	G_FreeEntity(ent);
}

/**
 * \brief Handles firing the grenade launcher.
 *
 * Handles firing the grenade launcher.
 *
 * @param ent the player
 * @param alt_fire was this alt fire mode?
 */
static void WP_FireGrenade( gentity_t *ent, qboolean alt_fire )
{
	gentity_t	*grenade;
	gentity_t	*tripwire = NULL;
	gentity_t	*tent = 0;
	vec3_t		dir, start;
	int		tripcount = 0;
	int		foundTripWires[MAX_GENTITIES] = {ENTITYNUM_NONE};
	int		tripcount_org;
	int		lowestTimeStamp;
	int		removeMe;
	int		i;
	trace_t		tr;
	vec3_t		end;

	VectorCopy( forward, dir );
	VectorCopy( muzzle, start );

	if(RPGEntityCount != ENTITYNUM_MAX_NORMAL-20){
		if ( alt_fire )
		{
			/* RPG-X: RedTechie - Moved here to stop entities from being sucked up */
			grenade = G_Spawn();
			
			/* kef -- make sure count is 0 so it won't get its bounciness removed like the tetrion projectile */
			grenade->count = 0;

			/* RPG-X: RedTechie - Forced Tripwires */
			if ( rpg_invisibletripmines.integer == 1 )
			{
				/* 
 				 * limit to 10 placed at any one time
				 * see how many there are now
				 */
				while ( (tripwire = G_Find( tripwire, FOFS(classname), "tripwire" )) != NULL )
				{
					if ( tripwire->parent != ent )
					{
						continue;
					}
					foundTripWires[tripcount++] = tripwire->s.number;
				}
				/* now remove first ones we find until there are only 9 left */
				tripwire = NULL;
				tripcount_org = tripcount;
				lowestTimeStamp = level.time;
				/* RPG-X: RedTechie - Added 51 tripwires for each person */
				while ( tripcount > 50 ) /* 9 */
				{
					removeMe = -1;
					for ( i = 0; i < tripcount_org; i++ )
					{
						if ( foundTripWires[i] == ENTITYNUM_NONE )
						{
							continue;
						}
						tripwire = &g_entities[foundTripWires[i]];
						if ( tripwire && tripwire->timestamp < lowestTimeStamp )
						{
							removeMe = i;
							lowestTimeStamp = tripwire->timestamp;
						}
					}
					if ( removeMe != -1 )
					{
						/* remove it... or blow it? */
						if ( &g_entities[foundTripWires[removeMe]] == NULL )
						{
							break;
						}
						else
						{
							G_FreeEntity( &g_entities[foundTripWires[removeMe]] );
						}
						foundTripWires[removeMe] = ENTITYNUM_NONE;
						tripcount--;
					}
					else
					{
						break;
					}
				}
				/* now make the new one */
				grenade->classname = "tripwire";
				if(rpg_dmgFlags.integer & 8) { 
					grenade->splashDamage = GRENADE_SPLASH_DAM*2;
					grenade->splashRadius = GRENADE_SPLASH_RAD*2;
				} else {
					grenade->splashDamage = 0;
					grenade->splashRadius = 0;

				}
				grenade->s.pos.trType = TR_LINEAR;
				grenade->nextthink = level.time + 1000; /* How long 'til she blows */
				grenade->count = 1; 			/* tell it it's a tripwire for when it sticks */
				grenade->timestamp = level.time; 	/* remember when we placed it */
				grenade->s.otherEntityNum2 = ent->client->sess.sessionTeam;
			}
			else
			{
				grenade->classname = "grenade_alt_projectile";
				if(rpg_dmgFlags.integer & 8) {
					grenade->splashDamage = GRENADE_SPLASH_DAM;
					grenade->splashRadius = GRENADE_SPLASH_RAD;
				} else {
					grenade->splashDamage = 0;
					grenade->splashRadius = 0;
				}
				grenade->s.pos.trType = TR_GRAVITY;
				grenade->nextthink = level.time + GRENADE_ALT_TIME; /* How long 'til she blows */
			}
			grenade->think = grenadeSpewShrapnel;
			grenade->s.eFlags |= EF_MISSILE_STICK;
			VectorScale( dir, 1000, grenade->s.pos.trDelta );

			grenade->damage = (rpg_dmgFlags.integer & 8) ? (GRENADE_ALT_DAMAGE*DMG_VAR) : (grenade->damage = 0);
			grenade->methodOfDeath = MOD_GRENADE_ALT;
			grenade->splashMethodOfDeath = MOD_GRENADE_ALT_SPLASH;
			grenade->s.eType = ET_ALT_MISSILE;

			/* RPG-X: RedTechie - Moved here to stop entities from being sucked up */
			grenade->r.svFlags = SVF_USE_CURRENT_ORIGIN;
			grenade->s.weapon = WP_8;
			grenade->r.ownerNum = ent->s.number;
			grenade->parent = ent;

			VectorSet(grenade->r.mins, -GRENADE_SIZE, -GRENADE_SIZE, -GRENADE_SIZE);
			VectorSet(grenade->r.maxs, GRENADE_SIZE, GRENADE_SIZE, GRENADE_SIZE);

			grenade->clipmask = MASK_SHOT;

			grenade->s.pos.trTime = level.time;		/* move a bit on the very first frame */
			VectorCopy( start, grenade->s.pos.trBase );
			SnapVector( grenade->s.pos.trBase );		/* save net bandwidth */
			
			SnapVector( grenade->s.pos.trDelta );		/* save net bandwidth */
			VectorCopy (start, grenade->r.currentOrigin);

			VectorCopy( start, grenade->pos2 );
		}
		else
		{
			/* RPG-X: RedTechie - Check to see if there admin if so grant them effects gun */
			if( IsAdmin(ent) && (rpg_effectsgun.integer == 1))
			{
				VectorMA (muzzle, MAXRANGE_CRIFLE, forward, end);
				trap_Trace (&tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );

				/*
 				 * TiM : FX Gun additional effects.
				 * Okay... screw the generic args. it's giving me a headache
				 * Case in this case... harhar is teh solution
				 */
				if ( ent->client->fxGunData.eventNum > 0 ) 
				{
					fxGunData_t *fxGunData = &ent->client->fxGunData;

					/* set the entity event */
					tent = G_TempEntity( tr.endpos, fxGunData->eventNum );
					
					/* based on the event, add additional args */
					switch ( fxGunData->eventNum ) {
						/* sparks */
						case EV_FX_SPARK:
							/* Direction vector based off of trace normal */
							VectorCopy( tr.plane.normal, tent->s.angles2 );
							VectorShort( tent->s.angles2 );

							/* spark interval */
							tent->s.time2 = fxGunData->arg_float1;
							/* spark time length */
							tent->s.time = fxGunData->arg_int2;
							break;
						case EV_FX_STEAM:
							/* Direction vector based off of trace normal */
							VectorCopy( tr.plane.normal, tent->s.angles2 );
							VectorShort( tent->s.angles2 );
							/* time length */
							tent->s.time = fxGunData->arg_int2;
							break;
						case EV_FX_FIRE:
							VectorCopy( tr.plane.normal, tent->s.angles2 );
							VectorShort( tent->s.angles2 );
							tent->s.time = fxGunData->arg_int1;
							tent->s.time2 = fxGunData->arg_int2;
							break;
						case EV_FX_SHAKE:
							VectorCopy( tr.plane.normal, tent->s.angles2 );
							VectorShort( tent->s.angles2 );
							tent->s.time = fxGunData->arg_int1;
							tent->s.time2 = fxGunData->arg_int2;
							break;
						case EV_FX_CHUNKS:
							/* normal direction */
							VectorCopy( tr.plane.normal, tent->s.angles2 );
							VectorShort( tent->s.angles2 );	

							/* scale/radius */
							tent->s.time2 = fxGunData->arg_int1;
							/* material type */
							tent->s.powerups = fxGunData->arg_int2;
							break;
						case EV_FX_DRIP:
							/* type of drip */
							tent->s.time2 = fxGunData->arg_int1;
							/* degree of drippiness */
							tent->s.angles2[0] = fxGunData->arg_float1;
							/* length of effect */
							tent->s.powerups = fxGunData->arg_int2;
							break;
						case EV_FX_SMOKE:
							/* Direction vector based off of trace normal */
							VectorCopy( tr.plane.normal, tent->s.angles2 );
							VectorShort( tent->s.angles2 );
							/* smoke radius */
							tent->s.time = fxGunData->arg_int1;
							/* killtime  */
							tent->s.time2 = fxGunData->arg_int2;

							/* set ent origin for dir calcs */
							VectorCopy( tent->s.origin, tent->s.origin2 );
							/* VectorMA( tent->s.origin2, 6, tr.plane.normal, tent->s.origin2 ); */
							tent->s.origin2[2] += 6;
							break;
						case EV_FX_SURFACE_EXPLOSION:
							/* radius */
							tent->s.angles2[0] = fxGunData->arg_float1;
							/* camera shake */
							tent->s.angles2[1] = fxGunData->arg_float2;
							/* orient the dir to the plane we shot at */
							VectorCopy( tr.plane.normal, tent->s.origin2 );
							/* Meh... generic hardcoded data for the rest lol */
							tent->s.time2 = 0;
							break;
						case EV_FX_ELECTRICAL_EXPLOSION:
							/* Set direction */
							VectorCopy( tr.plane.normal, tent->s.origin2 );
							/* Set Radius */
							tent->s.angles2[0] = fxGunData->arg_float1;
							break;
					}

					/* Little hack to make the Detpack sound global */
					if ( fxGunData->eventNum == EV_DETPACK ) {
						gentity_t	*te;
						te = G_TempEntity( tr.endpos, EV_GLOBAL_SOUND );
						te->s.eventParm = G_SoundIndex( "sound/weapons/explosions/detpakexplode.wav" );
						te->r.svFlags |= SVF_BROADCAST;
					}
				}
				else {
					tent = G_TempEntity( tr.endpos, EV_EFFECTGUN_SHOOT );
				}
				
				tent->s.eFlags |= EF_FIRING;
				
			}else{
				/* RPG-X: RedTechie - Moved here to stop entities from being sucked up */
				grenade = G_Spawn();
			
				/* kef -- make sure count is 0 so it won't get its bounciness removed like the tetrion projectile */
				grenade->count = 0;


				grenade->classname = "grenade_projectile";
				grenade->nextthink = level.time + GRENADE_TIME; /* How long 'til she blows */
				grenade->think = grenadeExplode;
				grenade->s.eFlags |= EF_BOUNCE_HALF;
				VectorScale( dir, GRENADE_VELOCITY, grenade->s.pos.trDelta );
				grenade->s.pos.trType = TR_GRAVITY;

				if(rpg_dmgFlags.integer & 8) {
					grenade->damage = GRENADE_DAMAGE*DMG_VAR;
					grenade->splashDamage = GRENADE_SPLASH_DAM;
					grenade->splashRadius = GRENADE_SPLASH_RAD;
				} else {
					grenade->damage = 0;
					grenade->splashDamage = 0;
					grenade->splashRadius = 0;
				}
				grenade->methodOfDeath = MOD_GRENADE;
				grenade->splashMethodOfDeath = MOD_GRENADE_SPLASH;
				grenade->s.eType = ET_MISSILE;

				/* RPG-X: RedTechie - Moved here to stop entities from being sucked up */
				grenade->r.svFlags = SVF_USE_CURRENT_ORIGIN;
				grenade->s.weapon = WP_8;
				grenade->r.ownerNum = ent->s.number;
				grenade->parent = ent;

				VectorSet(grenade->r.mins, -GRENADE_SIZE, -GRENADE_SIZE, -GRENADE_SIZE);
				VectorSet(grenade->r.maxs, GRENADE_SIZE, GRENADE_SIZE, GRENADE_SIZE);

				grenade->clipmask = MASK_SHOT;

				grenade->s.pos.trTime = level.time;		/* move a bit on the very first frame */
				VectorCopy( start, grenade->s.pos.trBase );
				SnapVector( grenade->s.pos.trBase );		/* save net bandwidth */
				
				SnapVector( grenade->s.pos.trDelta );		/* save net bandwidth */
				VectorCopy (start, grenade->r.currentOrigin);

				VectorCopy( start, grenade->pos2 );
			}
		}

		G_LogWeaponFire(ent->s.number, WP_8);
	}else{
		G_LogPrintf("RPG-X WARNING: Max entities about to be hit! Restart the server ASAP or suffer a server crash!\n");
		trap_SendServerCommand( -1, va("print \"^1RPG-X WARNING: Max entities about to be hit! Restart the server ASAP or suffer a server crash!\n\""));
	}
}

/*
----------------------------------------------
	TETRION
----------------------------------------------
*/

#define TETRION_ALT_SIZE		6

#define MAX_TR_116_DIST			8192
#define MAX_TRACES			24 /* Number of traces thru walls we'll do before we give up lol */

/**
 * \brief Fire a TR116 bullet.
 *
 * Creates and sets up an TR116 bullet entity.
 *
 * @param ent the player
 * @param start the start point
 * @dir the direction
 */
static void WP_FireTR116Bullet( gentity_t *ent, vec3_t start, vec3_t dir ) {
	gentity_t	*traceEnt;
	vec3_t 		end; 		/* end-point in trace */
	vec3_t 		traceFrom;
	trace_t	tr;

	VectorCopy( start, traceFrom );
	VectorMA( traceFrom, MAX_TR_116_DIST, dir, end ); /* set trace end point */

	trap_Trace( &tr, traceFrom, NULL, NULL, end, ent->s.number, CONTENTS_BODY ); /* MASK_SHOT - TiM - Goes thru everything but players */

	if ( tr.entityNum < ENTITYNUM_MAX_NORMAL ) {

		traceEnt = &g_entities[ tr.entityNum ];

		if ( traceEnt->takedamage && (rpg_dmgFlags.integer & 4)) {
			G_Damage( traceEnt, ent, ent, dir, tr.endpos, TETRION_DAMAGE, 0, MOD_TETRION_ALT );
		}
	}

}

/**
 * \brief Handles firing of the TR116 rifle.
 *
 * Handles firing of the TR116 rigle.
 *
 * @param ent the player
 * @param alt_fire was this alt fire mode?
 * TODO rename me?
 */
static void WP_FireTetrionDisruptor( gentity_t *ent, qboolean alt_fire )
/* (RPG-X: J2J MOdified to make it look and feel like tr116 */
/* RPG-X: TiM - Modified even furthur */
{
	vec3_t	dir;
	vec3_t	start;

	VectorCopy( forward, dir );
	VectorCopy( muzzle, start );

	WP_FireTR116Bullet( ent, start, dir );

	G_LogWeaponFire(ent->s.number, WP_7);
}


/*
----------------------------------------------
	QUANTUM BURST
----------------------------------------------
*/

#define QUANTUM_SIZE		1 
#define QUANTUM_ALT_THINK_TIME	300
#define QUANTUM_ALT_SEARCH_TIME	100
#define QUANTUM_ALT_SEARCH_DIST	4096

/**
 * \brief Fires a Quantum Burst.
 *
 * Creates and sets up an Quantum Burst projectile.
 *
 * @param ent the player
 * @param start the start point
 * @param dir the direction
 */
static void FireQuantumBurst( gentity_t *ent, vec3_t start, vec3_t dir )
{
	gentity_t	*bolt;

	bolt = G_Spawn();
	bolt->classname = "quantum_projectile";
	
	bolt->nextthink = level.time + 6000;
	bolt->think = G_FreeEntity;

	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_9;
	bolt->r.ownerNum = ent->s.number;
	bolt->parent = ent;

	if(rpg_dmgFlags.integer & 16) {
		bolt->damage = QUANTUM_DAMAGE*DMG_VAR;
		bolt->splashDamage = QUANTUM_SPLASH_DAM;
		bolt->splashRadius = QUANTUM_SPLASH_RAD;
	} else {
		bolt->damage = 0;
		bolt->splashDamage = 0;
		bolt->splashRadius = 0;
	}

	bolt->methodOfDeath = MOD_QUANTUM;
	bolt->splashMethodOfDeath = MOD_QUANTUM_SPLASH;
	bolt->clipmask = MASK_SHOT;

	VectorSet(bolt->r.mins, -QUANTUM_SIZE, -QUANTUM_SIZE, -QUANTUM_SIZE);
	VectorSet(bolt->r.maxs, QUANTUM_SIZE, QUANTUM_SIZE, QUANTUM_SIZE);

	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time;		/* move a bit on the very first frame */
	VectorCopy( start, bolt->s.pos.trBase );
	SnapVector( bolt->s.pos.trBase );		/* save net bandwidth */
	
	VectorScale( dir, rpg_photonSpeed.integer, bolt->s.pos.trDelta );
	
	SnapVector( bolt->s.pos.trDelta );		/* save net bandwidth */
	VectorCopy (start, bolt->r.currentOrigin);
	VectorCopy (start, bolt->pos1);
}

/**
 * \brief Search a target for quantum burst alt fire projectile.
 *
 * Search a target for the quantum burst alt fire mode projectile.
 *
 * @param ent the projectile
 * @param start start point
 * @param end end point
 */
static qboolean SearchTarget(gentity_t *ent, vec3_t start, vec3_t end)
{
	trace_t tr;
	gentity_t *traceEnt;
	vec3_t fwd;

	trap_Trace (&tr, start, NULL, NULL, end, ent->s.number, MASK_SHOT );
	traceEnt = &g_entities[ tr.entityNum ];

	if (traceEnt->takedamage && traceEnt->client) 
	{
		ent->target_ent = traceEnt;
		VectorSubtract(ent->target_ent->r.currentOrigin, ent->r.currentOrigin, fwd);
		VectorNormalize(fwd);
		VectorScale(fwd, rpg_altPhotonSpeed.integer, ent->s.pos.trDelta);
		VectorCopy(fwd, ent->movedir);
		SnapVector(ent->s.pos.trDelta);	/* save net bandwidth */
		VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
		ent->s.pos.trTime = level.time;
		ent->nextthink = level.time + QUANTUM_ALT_THINK_TIME;
		return qtrue;
	}
	return qfalse; 
}

/**
 * \brief Alt quantum burst projectile think functiom.
 *
 * Alt quantum burst projectile think function.
 *
 * @param ent the projectile
 */
static void WP_QuantumAltThink(gentity_t *ent)
{
	vec3_t start, newdir, targetdir, lup={0,0,1}, lright, search; 
	float dot, dot2;

	ent->health--;
	if (ent->health<=0)
	{
		G_FreeEntity(ent);
		return;
	}

	if (ent->target_ent)
	{	/* Already have a target, start homing. */
		if (ent->health <= 0 || !ent->inuse)
		{	/* No longer target this */
			ent->target_ent = NULL;
			ent->nextthink = level.time + 1000;
			ent->health -= 5;
			return;
		}
		VectorSubtract(ent->target_ent->r.currentOrigin, ent->r.currentOrigin, targetdir);
		VectorNormalize(targetdir);

		/* Now the rocket can't do a 180 in space, so we'll limit the turn to about 45 degrees. */
		dot = DotProduct(targetdir, ent->movedir);
		/* a dot of 1.0 means right-on-target. */
		if (dot < 0.0)
		{	/* Go in the direction opposite, start a 180. */
			CrossProduct(ent->movedir, lup, lright);
			dot2 = DotProduct(targetdir, lright);
			if (dot2 > 0)
			{	/* Turn 45 degrees right. */
				VectorAdd(ent->movedir, lright, newdir);
			}
			else
			{	/* Turn 45 degrees left. */
				VectorSubtract(ent->movedir, lright, newdir);
			}
			/* Yeah we've adjusted horizontally, but let's split the difference vertically, so we kinda try to move towards it. */
			newdir[2] = (targetdir[2] + ent->movedir[2]) * 0.5;
			VectorNormalize(newdir);
		}
		else if (dot < 0.7)
		{	/* Need about one correcting turn.  Generate by meeting the target direction "halfway". */
			/* Note, this is less than a 45 degree turn, but it is sufficient.  We do this because the rocket may have to go UP. */
			VectorAdd(ent->movedir, targetdir, newdir);
			VectorNormalize(newdir);
		}
		else
		{	/* else adjust to right on target. */
			VectorCopy(targetdir, newdir);
		}

		VectorScale(newdir, rpg_altPhotonSpeed.integer, ent->s.pos.trDelta);
		VectorCopy(newdir, ent->movedir);
		SnapVector(ent->s.pos.trDelta);			/* save net bandwidth */
		VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
		SnapVector(ent->s.pos.trBase);
		ent->s.pos.trTime = level.time;

		/* Home at a reduced frequency. */
		ent->nextthink = level.time + QUANTUM_ALT_THINK_TIME;	/* Nothing at all spectacular happened, continue. */
	}
	else
	{	/* Search in front of the missile for targets. */
		VectorCopy(ent->r.currentOrigin, start);
		CrossProduct(ent->movedir, lup, lright);

		/* Search straight ahead. */
		VectorMA(start, QUANTUM_ALT_SEARCH_DIST, ent->movedir, search);

		/* Add some small randomness to the search Z height, to give a bit of variation to where we are searching. */
		search[2] += flrandom(-QUANTUM_ALT_SEARCH_DIST*0.075, QUANTUM_ALT_SEARCH_DIST*0.075);

		if (SearchTarget(ent, start, search))
			return;

		/* Search to the right. */
		VectorMA(search, QUANTUM_ALT_SEARCH_DIST*0.1, lright, search);
		if (SearchTarget(ent, start, search))
			return;
		
		/* Search to the left. */
		VectorMA(search, -QUANTUM_ALT_SEARCH_DIST*0.2, lright, search);
		if (SearchTarget(ent, start, search))
			return;

		/* Search at a higher rate than correction. */
		ent->nextthink = level.time + QUANTUM_ALT_SEARCH_TIME;	/* Nothing at all spectacular happened, continue. */

	}
	return;
}

/**
 * \brief Fire quantum burst alt fire mode.
 *
 * Fire quantum burst alt fire mode.
 *
 * @param ent the player
 * @param start start point
 * @param dir the direction
 */
static void FireQuantumBurstAlt( gentity_t *ent, vec3_t start, vec3_t dir )
{
	gentity_t	*bolt;

	bolt = G_Spawn();
	bolt->classname = "quantum_alt_projectile";
	
	bolt->nextthink = level.time + 100;
	bolt->think = WP_QuantumAltThink;
	bolt->health = 25;		/* 10 seconds. */

	bolt->s.eType = ET_ALT_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_9;
	bolt->r.ownerNum = ent->s.number;
	bolt->parent = ent;
	bolt->s.eFlags |= EF_ALT_FIRING;

	if(rpg_dmgFlags.integer & 16) {
		bolt->damage = QUANTUM_ALT_DAMAGE*DMG_VAR;
		bolt->splashDamage = QUANTUM_ALT_SPLASH_DAM;
		bolt->splashRadius = QUANTUM_ALT_SPLASH_RAD;
	} else {
		bolt->damage = 0;
		bolt->splashDamage = 0;
		bolt->splashRadius = 0;
	}

	bolt->methodOfDeath = MOD_QUANTUM_ALT;
	bolt->splashMethodOfDeath = MOD_QUANTUM_ALT_SPLASH;
	bolt->clipmask = MASK_SHOT;

	VectorSet(bolt->r.mins, -QUANTUM_SIZE, -QUANTUM_SIZE, -QUANTUM_SIZE);
	VectorSet(bolt->r.maxs, QUANTUM_SIZE, QUANTUM_SIZE, QUANTUM_SIZE);

	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time;		/* move a bit on the very first frame */
	VectorCopy( start, bolt->s.pos.trBase );
	SnapVector(bolt->s.pos.trBase);
	
	VectorScale( dir, rpg_altPhotonSpeed.integer, bolt->s.pos.trDelta );
	VectorCopy(dir, bolt->movedir);
	
	SnapVector( bolt->s.pos.trDelta );			/* save net bandwidth */
	VectorCopy (start, bolt->r.currentOrigin);
}

/**
 * \brief Handles firing of the quatum burst.
 * 
 * Handles firing of the quantum burst.
 *
 * @param ent the player
 * @param alt_fire was this alt fire mode?
 */
static void WP_FireQuantumBurst( gentity_t *ent, qboolean alt_fire )
{
	vec3_t	dir, start;

	VectorCopy( forward, dir );
	VectorCopy( muzzle, start );

	if ( alt_fire )
	{
		FireQuantumBurstAlt( ent, start, dir );
	}
	else
	{
		FireQuantumBurst( ent, start, dir );
	}

	G_LogWeaponFire(ent->s.number, WP_9);
}

qboolean G_Weapon_LogAccuracyHit( gentity_t *target, gentity_t *attacker ) {
	if( !target->takedamage ) {
		return qfalse;
	}

	if ( target == attacker ) {
		return qfalse;
	}

	if( !target->client ) {
		return qfalse;
	}

	if( !attacker->client ) {
		return qfalse;
	}

	if( target->client->ps.stats[STAT_HEALTH] <= 0 ) {
		return qfalse;
	}

	return qtrue;
}

#define MAX_FORWARD_TRACE	8192

/**
 * \brief Corrects the forward vector.
 *
 * @param ent entity
 * @param fwd the forward vector
 * @param muzzlePoint the muzzle point
 * @param projsize projsize
 */
static void CorrectForwardVector(gentity_t *ent, vec3_t fwd, vec3_t muzzlePoint, float projsize)
{
	trace_t		tr;
	vec3_t		end;
	vec3_t		eyepoint;
	vec3_t		mins, maxs;

	/* Find the eyepoint. */
	VectorCopy(ent->client->ps.origin, eyepoint);
	eyepoint[2] += ent->client->ps.viewheight;

	/* First we must trace from the eyepoint to the muzzle point, to make sure that we have a legal muzzle point. */
	if (projsize>0)
	{
		VectorSet(mins, -projsize, -projsize, -projsize);
		VectorSet(maxs, projsize, projsize, projsize);
		trap_Trace(&tr, eyepoint, mins, maxs, muzzlePoint, ent->s.number, MASK_SHOT);
	}
	else
	{
		trap_Trace(&tr, eyepoint, NULL, NULL, muzzlePoint, ent->s.number, MASK_SHOT);
	}

	if (tr.fraction < 1.0)
	{	/* We hit something here...  Stomp the muzzlePoint back to the eye... */
		VectorCopy(eyepoint, muzzlePoint);
		/* Keep the forward vector where it is, 'cause straight forward from the eyeball is right where we want to be. */
	}
	else
	{
		/* figure out what our crosshairs are on... */
		VectorMA(eyepoint, MAX_FORWARD_TRACE, forward, end);
		trap_Trace (&tr, eyepoint, NULL, NULL, end, ent->s.number, MASK_SHOT );

		/* ...and have our new forward vector point at it */
		VectorSubtract(tr.endpos, muzzlePoint, fwd);
		VectorNormalize(fwd);
	}
}

/*
===============
G_Weapon_CalcMuzzlePoint

set muzzle location relative to pivoting eye
===============
*/

/**
 * \brief Muzzle point table...
 *
 * Table containing the muzzle points for all weapons.
 */
static vec3_t WP_MuzzlePoint[WP_NUM_WEAPONS] = 
{/*	Fwd,	right,		up. 		*/
	{0,	0,		0	},	/* WP_0, */
	{29,	2,		-4	},	/* WP_5, */			
	{25,	7,		-10	},	/* WP_6, */
	{25,	4,		-5	},	/* WP_1, */				
	{10,	14,		-8	},	/* WP_4, */	
	{25,	5,		-8	},	/* WP_10, */			
	{25,	5,		-10	},	/* WP_8, */	
	{0,	0,		0	},	/* WP_7, */ /*{22,	4.5,	-8	}, //TiM : Visual FX aren't necessary now, so just screw it */
	{5,	6,		-6	},	/* WP_9, */	
	{29,	2,		-4	},	/* WP_13, */		
	{29,	2,		-4	},	/* WP_12, */	
	{29,	2,		-4	},	/* WP_14 */
	{27,	8,		-10	},	/* WP_11 */
	{29,	2,		-4	},	/* WP_2, */	
	{29,	2,		-4	},	/* WP_3, */	
	{29,	2,		-4	},	/* WP_15, */
/*	{25,	7,		-10	},*/	/* WP_7 */
};


/**
 * \brief Shot size table.
 * 
 * Table containing the size of each weapons projectiles.
 */
static float WP_ShotSize[WP_NUM_WEAPONS] = 
{
	0,				/* WP_0, */
	0,				/* WP_5, */			
	0,				/* WP_6, */
	0,				/* WP_1, */				
	SCAV_SIZE,			/* WP_4, */	
	STASIS_MAIN_MISSILE_BIG*3,	/* WP_10, */			
	GRENADE_SIZE,			/* WP_8, */	
	6,				/* WP_7, */
	QUANTUM_SIZE,			/* WP_9, */	
	0,				/* WP_13, */		
	0,				/* WP_12, */
	0,				/* WP_14 */
	0,				/* WP_11 */
	0,				/* WP_2, */
	0,				/* WP_3, */
	0,				/* WP_15, */
/*	0, */				/* WP_7 */
};

/**
 * \brief Alt shot size table.
 *
 * Table containing the size of each weapons alt projectiles.
 */
static float WP_ShotAltSize[WP_NUM_WEAPONS] = 
{
	0,				/* WP_0, */
	PHASER_ALT_RADIUS,		/* WP_5, */			
	0,				/* WP_6, */
	0,				/* WP_1, */				
	SCAV_ALT_SIZE,			/* WP_4, */	
	STASIS_MAIN_MISSILE_BIG*3,	/* WP_10, */			
	GRENADE_SIZE,			/* WP_8, */	
	TETRION_ALT_SIZE,		/* WP_7, */
	QUANTUM_SIZE,			/* WP_9, */	
	0,				/* WP_13, */		
	0,				/* WP_12, */		
	0,				/* WP_14 */
	0,				/* WP_11 */
	0,				/* WP_2 */
	0,				/* WP_3, */
	0,				/* WP_15, */
/*	0,*/				/* WP_7 */
};

void G_Weapon_CalcMuzzlePoint ( gentity_t *ent, vec3_t fwd, vec3_t rt, vec3_t vup, vec3_t muzzlePoint, float projsize) 
{
	int weapontype;

	weapontype = ent->s.weapon;
	VectorCopy( ent->s.pos.trBase, muzzlePoint );

#if 1
	if (weapontype > WP_0 && weapontype < WP_NUM_WEAPONS)
	{	/* Use the table to generate the muzzlepoint; */
		{	/* Crouching.  Use the add-to-Z method to adjust vertically. */
			VectorMA(muzzlePoint, WP_MuzzlePoint[weapontype][0], fwd, muzzlePoint);
			VectorMA(muzzlePoint, WP_MuzzlePoint[weapontype][1], rt, muzzlePoint);
			if ( ent->client->ps.eFlags & EF_FULL_ROTATE && Q_fabs( ent->client->ps.viewangles[PITCH] > 89.0f ) ) {
				muzzlePoint[2] -= 20 + WP_MuzzlePoint[weapontype][2];
			}
			else
				muzzlePoint[2] += ent->client->ps.viewheight + WP_MuzzlePoint[weapontype][2];
			/* VectorMA(muzzlePoint, ent->client->ps.viewheight + WP_MuzzlePoint[weapontype][2], vup, muzzlePoint);*/
		}
	}
#else	/* Test code */
	muzzlePoint[2] += ent->client->ps.viewheight;/* By eyes */
	muzzlePoint[2] += g_debugUp.value;
	VectorMA( muzzlePoint, g_debugForward.value, fwd, muzzlePoint);
	VectorMA( muzzlePoint, g_debugRight.value, rt, muzzlePoint);
#endif

	CorrectForwardVector(ent, fwd, muzzlePoint, projsize);
	SnapVector( muzzlePoint );
}


RPGX_SiteTOSiteData TransDat[MAX_CLIENTS];

/**
 * \brief Handles firing of the Tricorder.
 *
 * Handles firing of the Tricorder.
 *
 * @param ent the player
 * @param alt_fire was this alt fire mode?
 */
static void WP_TricorderScan (gentity_t *ent, qboolean alt_fire)
{
	gentity_t	*tr_ent;
	trace_t		tr;
	vec3_t		mins, maxs, end;
	int clientNum = ent->client->ps.clientNum;

	if ( rpg_rangetricorder.integer < 32 )
	{
		return;
	}

	/* Fix - Changed || to && in the below if statement! */
	if ( IsAdmin( ent ) == qfalse )
	{
		return;
	}

	VectorMA( muzzle, rpg_rangetricorder.integer, forward, end );

	VectorSet( maxs, 6, 6, 6 );
	VectorScale( maxs, -1, mins );

	/* 
 	 * TiM: I don't think performing a volume trace here is really needed.
 	 * It is after all based on the player's current view.
	 * TiM: No, I was wrong! They're better coz it means errant n00bs or bots can't dodge them as easily!
	 */
	trap_Trace ( &tr, muzzle, mins, maxs, end, ent->s.number, MASK_SHOT );
	/*trap_Trace ( &tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );*/
	tr_ent = &g_entities[tr.entityNum];

	/* BOOKMARK J2J */
	if ( alt_fire )
	{
		/* 
 		 * RPG-X: J2J - New Transporter Tricorder Code (custom spawn points)
		 */
		/* if( TransDat[clientNum].Used == qfalse )*/
		if ( VectorCompare( vec3_origin, TransDat[clientNum].storedCoord[TPT_TRICORDER].origin ) &&
			VectorCompare( vec3_origin, TransDat[clientNum].storedCoord[TPT_TRICORDER].angles ) )
		{
			/*VectorCopy(ent->client->ps.origin, TransDat[clientNum].TransCoord);*/
			/*VectorCopy(ent->client->ps.viewangles, TransDat[clientNum].TransCoordRot);*/
			VectorCopy( ent->client->ps.origin, TransDat[clientNum].storedCoord[TPT_TRICORDER].origin );
			VectorCopy( ent->client->ps.viewangles, TransDat[clientNum].storedCoord[TPT_TRICORDER].angles );
			/*TransDat[clientNum].Used = qtrue;*/
		}

		if ( tr_ent && tr_ent->client && tr_ent->health > 0 )
		{
			/*gentity_t	*tent;*/
			/*
 			 * TiM: If we're already in a transport sequence, don't try another one.
			 * For starters, this screws up the visual FX, and secondly, I'm betting
			 * if u actually tried this, you'd atomically disperse the transportee in a very painful way O_o
			 */
			if ( TransDat[tr_ent->client->ps.clientNum].beamTime > level.time ) {
				trap_SendServerCommand( ent-g_entities, "chat \"Unable to comply. Subject is already within a transport cycle.\"");
				return;
			}

			trap_SendServerCommand( ent-g_entities, "chat \"Energizing.\"");

			G_InitTransport( tr_ent->client->ps.clientNum, TransDat[clientNum].storedCoord[TPT_TRICORDER].origin,
							TransDat[clientNum].storedCoord[TPT_TRICORDER].angles );				return;
		}
		/* If they clicked within 5 seconds ago */
		if((level.time - TransDat[clientNum].LastClick) <= 5000)
		{
			VectorCopy( ent->client->ps.origin, TransDat[clientNum].storedCoord[TPT_TRICORDER].origin );
			VectorCopy( ent->client->ps.viewangles, TransDat[clientNum].storedCoord[TPT_TRICORDER].angles );

			/*VectorCopy(ent->client->ps.origin, TransDat[clientNum].TransCoord);*/
			/*VectorCopy(ent->client->ps.viewangles, TransDat[clientNum].TransCoordRot);*/
			TransDat[clientNum].LastClick = level.time-5000;
			trap_SendServerCommand( ent-g_entities, "chat \"Location Confirmed.\"");
			/*trap_SendConsoleCommand( EXEC_APPEND, va("echo Location Confirmed.") );*/
		}
		else
		{
			trap_SendServerCommand( ent-g_entities, "chat \"Click again to confirm Transporter Location.\"");
			/*trap_SendConsoleCommand( EXEC_APPEND, va("echo Click again to confirm Transporter Location.") );*/
			TransDat[clientNum].LastClick = level.time;
		}
	}
}

/**
 * \brief Handles firing of the hypospray.
 *
 * Handles firing of the hypospray.
 *
 * @param ent the player
 * @param alt_fire was this alt fire mode?
 */
static void WP_SprayVoyagerHypo( gentity_t *ent, qboolean alt_fire )
{
	gentity_t	*tr_ent;
	trace_t		tr;
	vec3_t		mins, maxs, end;
	gentity_t	*t_ent;
	playerState_t 	*tr_entPs;

	if ( rpg_rangehypo.integer < 8 ) /*32*/
	{
		return;
	}

	VectorMA( muzzle, rpg_rangehypo.integer, forward, end );

	VectorSet( maxs, 6, 6, 6 );
	VectorScale( maxs, -1, mins );

	trap_Trace ( &tr, muzzle, mins, maxs, end, ent->s.number, MASK_OPAQUE|CONTENTS_BODY|CONTENTS_ITEM|CONTENTS_CORPSE ); /*MASK_SHOT*/
	
	if(rpg_effectsgun.integer == 1 && IsAdmin(ent) && alt_fire == qtrue && ent->s.weapon == WP_12){
		if(RPGEntityCount != ENTITYNUM_MAX_NORMAL-20){
			t_ent = G_TempEntity( muzzle, EV_HYPO_PUFF );
			t_ent->s.eventParm = qfalse; /* TiM: Event parm is holding a qboolean value for color of spray */
				VectorCopy( forward, t_ent->s.angles2 ); /* TiM: Holds the directional vector.  This is passed to CG so it can be rendered right */
			return;
		}else{
			G_LogPrintf("RPG-X WARNING: Max entities about to be hit! Restart the server ASAP or suffer a server crash!\n");
			trap_SendServerCommand( -1, va("print \"^1RPG-X WARNING: Max entities about to be hit! Restart the server ASAP or suffer a server crash!\n\""));
		}
	}

	tr_ent = &g_entities[tr.entityNum];
	/* RPG-X: RedTechie - Medics can revive dead people */
	if( (tr_ent && tr_ent->client) && (tr_ent->health == 1) && (tr_ent->client->ps.pm_type == PM_DEAD)){ 		
		tr_entPs = &tr_ent->client->ps;
		if(rpg_medicsrevive.integer == 1){
			G_Client_Spawn(tr_ent, 1, qtrue);

			/* TiM : Hard coded emote.  Makes the player play a 'get up' animation :) */
			/* G_MoveBox( tr_ent ); */
			tr_ent->r.contents = CONTENTS_NONE;
			tr_entPs->stats[LEGSANIM] = ((tr_entPs->stats[LEGSANIM] & ANIM_TOGGLEBIT) ^ ANIM_TOGGLEBIT ) | BOTH_GET_UP1;
			tr_entPs->stats[TORSOANIM] = ((tr_entPs->stats[LEGSANIM] & ANIM_TOGGLEBIT) ^ ANIM_TOGGLEBIT ) | BOTH_GET_UP1;
			tr_entPs->stats[EMOTES] |= EMOTE_BOTH | EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH;
			tr_entPs->stats[TORSOTIMER] = 1700;
			tr_entPs->stats[LEGSTIMER] = 1700;
			tr_entPs->legsAnim = 0;
			tr_entPs->torsoAnim = 0;
			tr_entPs->torsoTimer = 0;
			tr_entPs->legsTimer = 0;

			/*tr_entPs->stats[STAT_WEAPONS] = ( 1 << WP_0 );*/
			/*tr_entPs->stats[STAT_HOLDABLE_ITEM] = HI_NONE;*/
		}
	/* RPG-X: RedTechie - Regular functions still work */
	} else if ( tr_ent && tr_ent->client && tr_ent->health > 0 ) {
		tr_entPs = &tr_ent->client->ps;
		if(alt_fire && rpg_hypoMelee.integer) { /* alt fire and hypo melee enabled */
			tr_ent->health = 0;
			G_Client_Die( tr_ent, ent, ent, 100, MOD_KNOCKOUT );
			G_LogWeaponFire( ent->s.number, WP_12 );
		} else { /* else just heal */
			if ( tr_ent->health < tr_entPs->stats[STAT_MAX_HEALTH] )
			{
				tr_ent->health = tr_entPs->stats[STAT_MAX_HEALTH];
			}
		}
	}
	/* TiM- else, use it on yourself */
	else
	{
		ent->health = ent->client->ps.stats[STAT_MAX_HEALTH];
	}
}

/*
===============
FireWeapon
===============
*/

#define ACCURACY_TRACKING_DELAY			100 /* in ms */
#define NUM_FAST_WEAPONS			3

void FireWeapon( gentity_t *ent, qboolean alt_fire ) 
{
	float			projsize;

	ent->client->pers.teamState.lastFireTime = level.time;

	/* set aiming directions */
	AngleVectors (ent->client->ps.viewangles, forward, right, up);

	if (alt_fire)
	{
		projsize = WP_ShotAltSize[ent->s.weapon];
	}
	else
	{
		projsize = WP_ShotSize[ent->s.weapon];
	}
	G_Weapon_CalcMuzzlePoint ( ent, forward, right, up, muzzle, projsize);

	/* fire the specific weapon */
	switch( ent->s.weapon )
	{
	/* Player weapons */
	case WP_5:
		WP_FirePhaser( ent, alt_fire );
		break;
	case WP_6:
		WP_FireCompressionRifle( ent, alt_fire );
		break;
	case WP_1:
		if ( IsAdmin( ent ) && alt_fire )
			WP_FireGrenade( ent, qfalse );
		break;
	case WP_4:
		break;
	case WP_10:
		WP_FireDisruptor( ent, alt_fire );
		break;
	case WP_8:
		WP_FireGrenade( ent, alt_fire );
		break;
	case WP_7:
		WP_FireTetrionDisruptor( ent, alt_fire );
		break;
	case WP_13:
		WP_SprayVoyagerHypo( ent, alt_fire );
		break;
	case WP_9:
		WP_FireQuantumBurst( ent, alt_fire );
		break;
	case WP_2:
		WP_TricorderScan( ent, alt_fire );
		break;
	case WP_3:
		break;
	case WP_15:
		WP_FireHyperspanner(ent, alt_fire);
		break;
	case WP_12:
		WP_SprayVoyagerHypo( ent, alt_fire );
		break;
	case WP_14:
		break;
	case WP_11:
		break;
	default:
		break;
	}
}

