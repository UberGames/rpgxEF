#include "g_local.h"

#define	ARM_ANGLE_RANGE		60
#define	HEAD_ANGLE_RANGE	90
#define	TURR_FOFS		18.0f
#define	TURR_ROFS		0.0f
#define	TURR_UOFS		12.0f
#define	ARM_FOFS		0.0f
#define	ARM_ROFS		0.0f
#define	ARM_UOFS		0.0f
#define	FARM_FOFS		14.0f
#define	FARM_ROFS		0.0f
#define	FARM_UOFS		4.0f
#define	FTURR_FOFS		0.0f
#define	FTURR_ROFS		0.0f
#define	FTURR_UOFS		6.0f
#define	LARM_FOFS		2.0f
#define	LARM_ROFS		0.0f
#define	LARM_UOFS		-26.0f

/**
 * \brief Turret's die function.
 *
 * Function called when a turret dies.
 *
 * @param self the turret
 * @param inflictor the inflictor
 * @param attacker the attakcer
 * @param damage the ammount of damage
 * @param meansOfDeath the means ot death
 */
void turret_die ( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath )
{
	vec3_t	dir;
	gentity_t	*owner, *te, *activator = self->activator;

	/* Turn off the thinking of the base & use it's targets */
	activator->think = 0;
	activator->nextthink = -1;
	activator->use = 0;
	if ( self->activator->target )
	{
		G_UseTargets( activator, attacker );
	}

	/* Remove the arm */
	if ( self->r.ownerNum >= 0 && self->r.ownerNum < ENTITYNUM_WORLD )
	{
		owner = &g_entities[self->r.ownerNum];
		G_FreeEntity( owner );
	}

	/* clear my data */
	self->die  = 0;
	self->think = 0;
	self->nextthink = -1;
	self->takedamage = qfalse;
	self->health = 0;

	/* Throw some chunks */
	/*AngleVectors( activator->r.currentAngles, dir, NULL, NULL );
	  VectorNormalize( dir );
	  CG_Chunks( self->s.number, self->r.currentOrigin, dir, Q_flrand(150, 300), irandom(3, 7), self->material, -1, 1.0 );*/

	if ( self->splashDamage > 0 && self->splashRadius > 0 )
	{/* FIXME: specify type of explosion?  (barrel, electrical, etc.) */
		G_RadiusDamage( self->r.currentOrigin, attacker, self->splashDamage, self->splashRadius, activator, DAMAGE_RADIUS, MOD_EXPLOSION );

		te = G_TempEntity( self->r.currentOrigin, EV_MISSILE_MISS );
		VectorSet( dir, 0, 0, 1 );
		te->s.eventParm = DirToByte( dir );
		te->s.weapon = WP_8;

		/* G_Sound(activator, G_SoundIndex("sound/weapons/explosions/explode11.wav")); */
	}

	activator->s.modelindex = activator->s.modelindex2;

	G_FreeEntity( self );
}

#define FORGE_TURRET_DAMAGE		2
#define FORGE_TURRET_SPLASH_RAD		64
#define FORGE_TURRET_SPLASH_DAM		4
#define FORGE_TURRET_VELOCITY		500

/**
 * \brief Fire the turret.
 *
 * Creates a new projectile and set it up.
 *
 * @param ent the turret
 * @param start start point
 * @param dir the direction
 */
void turret_fire ( gentity_t *ent, vec3_t start, vec3_t dir )
{
	gentity_t	*bolt;

	bolt = G_Spawn();
	bolt->classname = "red turret shot";

	bolt->nextthink = level.time + 10000;
	bolt->think = G_FreeEntity;

	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_10;
	bolt->r.ownerNum = ent->s.number;
	bolt->parent = ent;
	bolt->damage = ent->damage;
	bolt->splashDamage = 0;
	bolt->splashRadius = 0;
	bolt->methodOfDeath = MOD_STASIS;
	bolt->clipmask = MASK_SHOT;

	/*
 	 * There are going to be a couple of different sized projectiles, so store 'em here
	 * kef -- need to keep the size in something that'll reach the cgame side
	 */
	bolt->count = bolt->s.time2 = 2;

	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time;		/* move a bit on the very first frame */
	VectorCopy( start, bolt->s.pos.trBase );
	SnapVector( bolt->s.pos.trBase );			/* save net bandwidth */
	VectorScale( dir, 1100, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			/* save net bandwidth */
	VectorCopy( start, bolt->r.currentOrigin );

	VectorCopy (start, bolt->r.currentOrigin);
	/* Used by trails */
	VectorCopy (start, bolt->pos1 );
	VectorCopy (start, bolt->pos2 );
	/* kef -- need to keep the origin in something that'll reach the cgame side */
	VectorCopy(start, bolt->s.angles2);
	SnapVector( bolt->s.angles2 );			/* save net bandwidth */
}

/**
 * \Fires an fturret.
 *
 * Creates and sets up a new projectile.
 *
 * @param ent the turret
 * @param start the start point
 * @param dir the direction
 */
void fturret_fire ( gentity_t *ent, vec3_t start, vec3_t dir )
{
	gentity_t	*bolt;

	bolt = G_Spawn();
	
	bolt->classname = "red turret shot";
	bolt->nextthink = level.time + 10000;
	bolt->think = G_FreeEntity;
	bolt->s.eType = ET_MISSILE;
	bolt->s.weapon = WP_4;
	bolt->r.ownerNum = ent->s.number;
	bolt->damage = ent->damage;
	bolt->methodOfDeath = MOD_SCAVENGER;	/* ? */
	bolt->clipmask = MASK_SHOT;

	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time;		/* move a bit on the very first frame */
	VectorCopy( start, bolt->s.pos.trBase );
	VectorScale( dir, 1100, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );		/* save net bandwidth */
	VectorCopy( start, bolt->r.currentOrigin);
}

/**
 * \brief Think function of the turrets head.
 *
 * Think function of the turrets head.
 *
 * @param self the turrets head
 */
void turret_head_think (gentity_t *self)
{
	qboolean	fire_now = qfalse;

	if ( !(self->activator->spawnflags & 2) )
	{/* because forge turret heads have no anims... sigh... */
		/* animate */
		if ( self->activator->enemy || self->pain_debounce_time > level.time || self->s.frame )
		{
			self->s.frame++;
			if ( self->s.frame > 10 )
			{
				self->s.frame = 0;
			}

			if ( self->s.frame == 0 || self->s.frame == 4 )
			{
				fire_now = qtrue;
			}
		}
	}
	else
	{
		if ( self->fly_sound_debounce_time < level.time )
		{
			self->fly_sound_debounce_time = level.time + self->wait * 10;
			fire_now = qtrue;
		}
	}

	/* Fire */
	if ( fire_now && self->activator->enemy && self->last_move_time < level.time )
	{/* Only fire if ready to */
		vec3_t	forward, right, up, muzzleSpot;
		float	rOfs = 0;

		AngleVectors(self->r.currentAngles, forward, right, up);
		VectorMA( self->r.currentOrigin, 16, forward, muzzleSpot );
		VectorMA( self->r.currentOrigin, 8, up, muzzleSpot );
		if ( !(self->activator->spawnflags & 2) )
		{/* turrets have offsets */
			if ( self->s.frame == 0 )
			{/* Fire left barrel */
				rOfs = -6;
			}
			else if ( self->s.frame == 4 )
			{/* Fire right barrel */
				rOfs = 6;
			}
		}

		VectorMA( self->r.currentOrigin, rOfs, right, muzzleSpot );

		
		if ( atoi(self->team) == TEAM_RED )
		{
			/*G_Sound(self, G_SoundIndex("sound/enemies/turret/ffire.wav"));*/
			fturret_fire( self, muzzleSpot, forward );
		}
		else
		{
			/*G_Sound(self, G_SoundIndex("sound/enemies/turret/fire.wav"));*/
			turret_fire( self, muzzleSpot, forward );
		}
	}

	/*next think*/
	self->nextthink = level.time + self->wait;
}

/**
 * \brief Puts the head and arm of a turret together.
 *
 * Puts the head and arm of an turret together.
 *
 * @param arm the arm
 * @param head the head
 * @param fwdOfs forward offset
 * @param rtOfs right offset
 * @param upOfs up offset
 */
void bolt_head_to_arm( gentity_t *arm, gentity_t *head, float fwdOfs, float rtOfs, float upOfs )
{
	vec3_t	headOrg, forward, right, up;

	AngleVectors( arm->r.currentAngles, forward, right, up );
	VectorMA( arm->r.currentOrigin, fwdOfs, forward, headOrg );
	VectorMA( headOrg, rtOfs, right, headOrg );
	VectorMA( headOrg, upOfs, up, headOrg );
	G_SetOrigin( head, headOrg );
	head->r.currentAngles[1] = head->s.apos.trBase[1] = head->s.angles[1] = arm->r.currentAngles[1];
	trap_LinkEntity( head );
}

/**
 * \brief Puts the base and arm of a turret together.
 *
 * Puts the base and arm of a turret together.
 *
 * @param base the base
 * @param arm the arm
 * @param fwdOfs forward offset
 * @param rtOfs right offset
 * @param upOfs up offset
 */
void bolt_arm_to_base( gentity_t *base, gentity_t *arm, float fwdOfs, float rtOfs, float upOfs )
{
	vec3_t	headOrg, forward, right, up;

	AngleVectors( base->r.currentAngles, forward, right, up );
	VectorMA( base->r.currentOrigin, fwdOfs, forward, headOrg );
	VectorMA( headOrg, rtOfs, right, headOrg );
	VectorMA( headOrg, upOfs, up, headOrg );
	G_SetOrigin( arm, headOrg );
	trap_LinkEntity( arm );
	VectorCopy( base->r.currentAngles, arm->s.apos.trBase );
}

/**
 * \brief Put the turret together again.
 *
 * If the turret has moved this function is used to update all of the turrets parts.
 *
 * @param the turrets base
 */
void rebolt_turret( gentity_t *base )
{
	vec3_t	headOrg, forward, right, up;
	gentity_t *lastEnemy = base->lastEnemy;

	if ( !lastEnemy )
	{/* no arm */
		return;
	}

	if ( !lastEnemy->lastEnemy )
	{/* no head */
		return;
	}

	if ( base->spawnflags&2 )
	{
		bolt_arm_to_base( base, lastEnemy, FARM_FOFS, FARM_ROFS, FARM_UOFS );
		bolt_head_to_arm( lastEnemy, lastEnemy->lastEnemy, FTURR_FOFS, FTURR_ROFS, FTURR_UOFS );
	}
	else
	{
		/* FIXME: maybe move these seperately so they interpolate? */
		G_SetOrigin( lastEnemy, base->s.pos.trBase );
		trap_LinkEntity(lastEnemy);
		/*VectorCopy( base->r.currentAngles, lastEnemy->s.apos.trBase );*/
		AngleVectors( lastEnemy->r.currentAngles, forward, right, up );
		VectorMA( lastEnemy->r.currentOrigin, TURR_FOFS, forward, headOrg );
		VectorMA( headOrg, TURR_ROFS, right, headOrg );
		VectorMA( headOrg, TURR_UOFS, up, headOrg );
		G_SetOrigin( lastEnemy->lastEnemy, headOrg );
		/*lastEnemy->lastEnemy->r.currentAngles[1] = lastEnemy->lastEnemy->s.apos.trBase[1] = lastEnemy->lastEnemy->s.angles[1] = lastEnemy->r.currentAngles[1];*/
		trap_LinkEntity( lastEnemy->lastEnemy );
	}
}


/**
 * \brief Aims arm and head at enemy or neutral position.
 *
 * Aims arm and head at enemy or neutral position.
 *
 * @param self the turret
 */
void turret_aim( gentity_t *self )
{
	vec3_t	enemyDir;
	vec3_t	desiredAngles;
	float	diffAngle, armAngleDiff, headAngleDiff;
	int		yawTurn = 0;
	gentity_t *lastEnemy = self->lastEnemy;

	if ( self->enemy )
	{/* Aim at enemy */
		VectorSubtract( self->enemy->r.currentOrigin, self->r.currentOrigin, enemyDir );
		vectoangles( enemyDir, desiredAngles );
	}
	else
	{/* Return to front */
		VectorCopy( self->r.currentAngles, desiredAngles );
	}

	/*
 	 * yaw-aim arm at enemy at speed
	 * FIXME: noise when turning?
	 */
	diffAngle = AngleSubtract(desiredAngles[1], lastEnemy->r.currentAngles[1]);
	if ( diffAngle )
	{
		if ( fabs(diffAngle) < self->speed )
		{/* Just set the angle */
			lastEnemy->r.currentAngles[1] = desiredAngles[1];
		}
		else
		{/* Add the increment */
			lastEnemy->r.currentAngles[1] += (diffAngle < 0) ? -self->speed : self->speed;
		}
		yawTurn = (diffAngle > 0) ? 1 : -1;
	}
	/* Cap the range */
	armAngleDiff = AngleSubtract(self->r.currentAngles[1], lastEnemy->r.currentAngles[1]);
	if ( armAngleDiff > ARM_ANGLE_RANGE )
	{
		lastEnemy->r.currentAngles[1] = AngleNormalize360(self->r.currentAngles[1] - ARM_ANGLE_RANGE);
	}
	else if ( armAngleDiff < -ARM_ANGLE_RANGE )
	{
		lastEnemy->r.currentAngles[1] = AngleNormalize360(self->r.currentAngles[1] + ARM_ANGLE_RANGE);
	}
	VectorCopy( lastEnemy->r.currentAngles, lastEnemy->s.apos.trBase );

	/* Now put the turret at the tip of the arm */
	if ( self->spawnflags&2 )
	{
		bolt_head_to_arm( lastEnemy, lastEnemy->lastEnemy, FTURR_FOFS, FTURR_ROFS, FTURR_UOFS );
	}
	else
	{
		bolt_head_to_arm( lastEnemy, lastEnemy->lastEnemy, TURR_FOFS, TURR_ROFS, TURR_UOFS );
	}

	/* 
 	 * pitch-aim head at enemy at speed
	 * FIXME: noise when turning?
	 */
	if ( self->enemy )
	{
		VectorSubtract( self->enemy->r.currentOrigin, lastEnemy->lastEnemy->r.currentOrigin, enemyDir );
		vectoangles( enemyDir, desiredAngles );
	}
	/*//Not necc
	else
	{
		VectorCopy(self->r.currentAngles, desiredAngles);
	}
	*/
	diffAngle = AngleSubtract( desiredAngles[0], lastEnemy->lastEnemy->r.currentAngles[0] );
	if ( diffAngle )
	{
		if ( fabs(diffAngle) < self->speed )
		{/* Just set the angle */
			lastEnemy->lastEnemy->r.currentAngles[0] = desiredAngles[0];
		}
		else
		{/* Add the increment */
			lastEnemy->lastEnemy->r.currentAngles[0] += (diffAngle < 0) ? -self->speed : self->speed;
		}
	}
	/* Cap the range */
	headAngleDiff = AngleSubtract(self->r.currentAngles[0], lastEnemy->lastEnemy->r.currentAngles[0]);
	if ( headAngleDiff > HEAD_ANGLE_RANGE )
	{
		lastEnemy->lastEnemy->r.currentAngles[0] = AngleNormalize360(self->r.currentAngles[0] - HEAD_ANGLE_RANGE);
	}
	else if ( headAngleDiff < -HEAD_ANGLE_RANGE )
	{
		lastEnemy->lastEnemy->r.currentAngles[0] = AngleNormalize360(lastEnemy->r.currentAngles[0] + HEAD_ANGLE_RANGE);
	}
	VectorCopy( lastEnemy->lastEnemy->r.currentAngles, lastEnemy->lastEnemy->s.apos.trBase );

	/* Play sound if turret changes direction */
	/* Pitch: */
	/*
	if ( upTurn && upTurn != self->count )
	{//changed dir
		G_Sound(lastEnemy->lastEnemy, G_SoundIndex("sound/enemies/turret/move.wav"));
	}
	else if ( !upTurn && self->count )
	{//Just stopped
		G_Sound(lastEnemy->lastEnemy, G_SoundIndex("sound/enemies/turret/stop.wav"));
	}
	self->count = upTurn;
	*/
	/* Yaw: */
	if ( yawTurn && yawTurn != self->soundPos2 )
	{/* changed dir */
		G_Sound(lastEnemy, G_SoundIndex("sound/enemies/turret/move.wav"));
	}
	else if ( !yawTurn && self->soundPos2 )
	{/* Just stopped */
		G_Sound(lastEnemy, G_SoundIndex("sound/enemies/turret/stop.wav"));
	}
	self->soundPos2 = yawTurn;

	/*
	if ( turned )
	{
		G_Sound(lastEnemy, G_SoundIndex("sound/enemies/turret/move.wav"));
	}
	*/
}

/**
 * \brief Turn the turret off.
 *
 * Turns the turret off.
 *
 * @param self the turret
 */
void turret_turnoff (gentity_t *self)
{
	if ( self->enemy == NULL )
	{
		return;
	}
	/* shut-down sound */
	G_Sound(self, G_SoundIndex("sound/enemies/turret/shutdown.wav"));
	
	/* make turret keep animating for 3 secs */
	self->lastEnemy->lastEnemy->pain_debounce_time = level.time + 3000;

	/* Clear enemy */
	self->enemy = NULL;
}

/**
 * \brief Think function of the turrets base.
 *
 * Think function of the turrets base.
 *
 * @param self the turret
 */
void turret_base_think (gentity_t *self)
{
	vec3_t		enemyDir;
	float		enemyDist;
	gentity_t *lastEnemy;
	
	if(!self) return;

	lastEnemy = self->lastEnemy;

	self->nextthink = level.time + FRAMETIME;

	if ( self->spawnflags & 1 )
	{/* not turned on */
		turret_turnoff( self );
		turret_aim( self );
		/* No target */
		if ( lastEnemy && lastEnemy->lastEnemy )
		{
			lastEnemy->lastEnemy->flags |= FL_NOTARGET;
		}
		return;
	}
	else
	{/* I'm all hot and bothered */
		if ( lastEnemy && lastEnemy->lastEnemy )
		{
			lastEnemy->lastEnemy->flags &= ~FL_NOTARGET;
		}
	}

	if ( !self->enemy )
	{/* Find one */
		gentity_t	*target;
		float		bestDist = self->random * self->random;
		struct list entity_list;
		struct list ignore;
		list_iter_p iter;
		container_p c;

		if ( self->last_move_time > level.time )
		{/* We're active and alert, had an enemy in the last 5 secs */
			if ( self->pain_debounce_time < level.time )
			{
				G_Sound(self, G_SoundIndex("sound/enemies/turret/ping.wav"));
				self->pain_debounce_time = level.time + 1000;
			}
		}

		list_init(&entity_list, free);
		list_init(&ignore, free);

		if(lastEnemy && lastEnemy->lastEnemy) {
			ignore.append_ptr(&ignore, lastEnemy->lastEnemy, LT_DATA);
			G_RadiusList( self->r.currentOrigin, self->random, &ignore, qtrue, &entity_list );
		} else {
			G_RadiusList( self->r.currentOrigin, self->random, NULL, qtrue, &entity_list );
		}
		ignore.clear(&ignore);

		iter = entity_list.iterator(&entity_list, LIST_FRONT);
		for(c = entity_list.next(iter); c != NULL; c = entity_list.next(iter)) {
			target = c->data;

			if(target == NULL) {
				continue;
			}

			if ( target == self )
			{
				continue;
			}

			if ( target->takedamage && target->health > 0 && !(target->flags & FL_NOTARGET) )
			{
				if ( !target->client && target->team && atoi(target->team) == atoi(self->team) )
				{/* Something of ours we don't want to destroy */
					continue;
				}
				if ( target->client && target->client->sess.sessionTeam == atoi(self->team) )
				{/* A bot we don't want to shoot */
					continue;
				}

				if ( trap_InPVS( lastEnemy->lastEnemy->r.currentOrigin, target->r.currentOrigin ) )
				{
					trace_t	tr;

					trap_Trace( &tr, lastEnemy->lastEnemy->r.currentOrigin, NULL, NULL, target->r.currentOrigin, lastEnemy->lastEnemy->s.number, MASK_SHOT );

					if ( !tr.allsolid && !tr.startsolid && (tr.fraction == 1.0 || tr.entityNum == target->s.number) )
					{/* Only acquire if have a clear shot */
						/* Is it in range and closer than our best? */
						VectorSubtract( target->r.currentOrigin, self->r.currentOrigin, enemyDir );
						enemyDist = VectorLengthSquared( enemyDir );
						if ( enemyDist < bestDist )/* all things equal, keep current */
						{
							if ( self->last_move_time < level.time )
							{/* We haven't fired or acquired an enemy in the last 5 seconds */
								/* start-up sound */
								G_Sound(self, G_SoundIndex("sound/enemies/turret/startup.wav"));
								/* Wind up turrets for a second */
								lastEnemy->lastEnemy->last_move_time = level.time + 1000;
							}
							self->enemy = target;
							bestDist = enemyDist;
						}
					}
				}
			}
		}
		destroy_iterator(iter);
		entity_list.clear(&entity_list);
	}

	if ( self->enemy )
	{/* Check if still in random */
		if ( self->enemy->health <= 0 )
		{
			turret_turnoff( self );
			return;
		}

		VectorSubtract( self->enemy->r.currentOrigin, self->r.currentOrigin, enemyDir );
		enemyDist = VectorLengthSquared( enemyDir );
		if ( enemyDist > self->random*self->random )
		{
			turret_turnoff( self );
			return;
		}

		if(lastEnemy) {
			if(lastEnemy->lastEnemy) {
				if ( !trap_InPVS( lastEnemy->lastEnemy->r.currentOrigin, self->enemy->r.currentOrigin ) )
				{
					turret_turnoff( self );
					return;
				}
			}
		}

		/* Every now and again, check to see if we can even trace to the enemy */
		if ( irandom( 0, 16 ) > 15 )
		{
			trace_t tr;

			if(lastEnemy) {
				if(lastEnemy->lastEnemy) {
					trap_Trace( &tr, lastEnemy->lastEnemy->r.currentOrigin, NULL, NULL, self->enemy->r.currentOrigin, lastEnemy->lastEnemy->s.number, MASK_SHOT );
					if ( tr.allsolid || tr.startsolid || tr.fraction != 1.0 )
					{
						/* Couldn't see our enemy */
						turret_turnoff( self );
					}
				}
			}
		}
	}

	if ( self->enemy )
	{/* Aim */
		/* Won't need to wind up turrets for a while */
		self->last_move_time = level.time + 5000;
		turret_aim( self );
	}
	else if ( self->last_move_time < level.time )
	{
		/* Move arm and head back to neutral angles */
		turret_aim( self );
	}
}

/**
 * \brief Use function of the turrets base.
 *
 * Use function of the turrets base.
 *
 * @param self the turrets base
 * @param other another entity
 * @param the activator
 */
void turret_base_use (gentity_t *self, gentity_t *other, gentity_t *activator)
{/* Toggle on and off */
	self->spawnflags = (self->spawnflags ^ 1);
}

/*QUAKED misc_turret (1 0 0) (-8 -8 -8) (8 8 8) START_OFF
-----DESCRIPTION-----
Will aim and shoot at enemies

-----SPAWNFLAGS-----
1: START_OFF - Starts off

-----KEYS-----
random - How far away an enemy can be for it to pick it up (default 512)
speed - How fast it turns (degrees per second, default 30)
wait	- How fast it shoots (shots per second, default 4, can't be less)
dmg	- How much damage each shot does (default 5)
health - How much damage it can take before exploding (default 100)
  
splashDamage - How much damage the explosion does
splashRadius - The random of the explosion
NOTE: If either of the above two are 0, it will not make an explosion
  
targetname - Toggles it on/off
target - What to use when destroyed

"team" - This cannot take damage from members of this team and will not target members of this team (2 = blue, 1 = red) 2 will exclude players in RPG-X
*/

/**
 * \brief Spawn a turret.
 *
 * The spawn function for turrets.
 *
 * @param base the turrets base
 */
void SP_misc_turret (gentity_t *base)
{
	/* We're the base, spawn the arm and head */
	gentity_t *arm = G_Spawn();
	gentity_t *head = G_Spawn();
	vec3_t		fwd;

	/* Base */
	/* Base does the looking for enemies and pointing the arm and head */
	VectorCopy( base->s.angles, base->s.apos.trBase );
	VectorCopy( base->s.angles, base->r.currentAngles );
	AngleVectors( base->r.currentAngles, fwd, NULL, NULL );
	VectorMA( base->s.origin, -8, fwd, base->s.origin );
	G_SetOrigin(base, base->s.origin);
	trap_LinkEntity(base);
	if ( atoi( base->team ) == TEAM_RED )
	{/* red model */
		base->s.modelindex = G_ModelIndex("models/mapobjects/forge/turret.md3");
		base->s.modelindex2 = G_ModelIndex("models/mapobjects/forge/turret_d1.md3");
	}
	else
	{/* blue model */
		base->s.modelindex = G_ModelIndex("models/mapobjects/dn/gunturret_base.md3");
	}
	base->s.eType = ET_GENERAL;
	/* anglespeed - how fast it can track the player, entered in degrees per second, so we divide by FRAMETIME/1000 */
	if ( !base->speed )
	{
		base->speed = 3.0f;
	}
	else
	{
		base->speed /= FRAMETIME/1000.0f;
	}
	/* range */
	if ( !base->random )
	{
		base->random = 512;
	}
	base->use = turret_base_use;
	base->think = turret_base_think;
	base->nextthink = level.time + FRAMETIME;

	/* Arm */
	/* Does nothing, not solid, gets removed when head explodes */
	if ( atoi( base->team ) == TEAM_RED )
	{
		bolt_arm_to_base( base, arm, FARM_FOFS, FARM_ROFS, FARM_UOFS );
		bolt_head_to_arm( arm, head, FTURR_FOFS, FTURR_ROFS, FTURR_UOFS );
	}
	else
	{
		bolt_arm_to_base( base, arm, ARM_FOFS, ARM_ROFS, ARM_UOFS );
		/*G_SetOrigin( arm, base->s.origin );*/
		/*trap_LinkEntity(arm);*/
		/*VectorCopy( base->r.currentAngles, arm->s.apos.trBase );*/
		bolt_head_to_arm( arm, head, TURR_FOFS, TURR_ROFS, TURR_UOFS );
	}
	if ( atoi( base->team ) == TEAM_RED )
	{
		arm->s.modelindex = G_ModelIndex("models/mapobjects/forge/turret_neck.md3");
	}
	else
	{
		arm->s.modelindex = G_ModelIndex("models/mapobjects/dn/gunturret_arm.md3");
	}
	arm->team = base->team;

	/* Head */
	/* Fires when enemy detected, animates, can be blown up */
	VectorCopy( base->r.currentAngles, head->s.apos.trBase );
	if ( atoi( base->team ) == TEAM_RED )
	{
		head->s.modelindex = G_ModelIndex("models/mapobjects/forge/turret_head.md3");
	}
	else
	{
		head->s.modelindex = G_ModelIndex("models/mapobjects/dn/gunturret_head.md3");
	}
	head->team = base->team;
	head->s.eType = ET_GENERAL;
	VectorSet( head->r.mins, -8, -8, -16 );
	VectorSet( head->r.maxs, 8, 8, 16 );
	/* FIXME: make an index into an external string table for localization */
	/*head->fullName = "Turret";*/
	trap_LinkEntity(head);

	/* How much health head takes to explode */
	if ( !base->health )
	{
		head->health = 100;
	}
	else
	{
		head->health = base->health;
	}
	base->health = 0;
	/* How quickly to fire */
	if ( !base->wait )
	{
		head->wait = 50;
	}
	else
	{
		head->wait = 100/(base->wait/2);
	}
	base->wait = 0;
	/* splashDamage */
	if ( !base->splashDamage )
	{
		head->splashDamage = 10;
	}
	else
	{
		head->splashDamage = base->splashDamage;
	}
	base->splashDamage = 0;
	/* splashRadius */
	if ( !base->splashRadius )
	{
		head->splashRadius = 25;
	}
	else
	{
		head->splashRadius = base->splashRadius;
	}
	base->splashRadius = 0;
	/* dmg */
	if ( !base->damage )
	{
		head->damage = 5;
	}
	else
	{
		head->damage = base->damage;
	}
	base->damage = 0;

	/* Precache firing and explode sounds */
	G_SoundIndex("sound/weapons/explosions/explode11.wav");
	G_SoundIndex("sound/enemies/turret/startup.wav");
	G_SoundIndex("sound/enemies/turret/shutdown.wav");
	G_SoundIndex("sound/enemies/turret/move.wav");
	G_SoundIndex("sound/enemies/turret/stop.wav");
	G_SoundIndex("sound/enemies/turret/ping.wav");
	if ( atoi( base->team ) == TEAM_RED )
	{
		G_SoundIndex("sound/enemies/turret/ffire.wav");
	}
	else
	{
		G_SoundIndex("sound/enemies/turret/fire.wav");
	}

	head->r.contents = CONTENTS_BODY;
	/* head->max_health = head->health; */
	head->takedamage = qtrue;
	head->die = turret_die;

	head->think = turret_head_think;
	head->nextthink = level.time + FRAMETIME;

	/*head->material = MAT_METAL;*/
	/*head->r.svFlags |= SVF_NO_TELEPORT|SVF_NONNPC_ENEMY|SVF_SELF_ANIMATING;*/

	/* Link them up */
	base->lastEnemy = arm;
	arm->lastEnemy = head;
	head->r.ownerNum = arm->s.number;
	arm->activator = head->activator = base;

	/* FIXME: register the weapons whose effects are being used */
	if ( base->team )
	{
		if ( atoi( base->team ) == TEAM_BLUE )
		{
			/* temp gfx and sounds */
			RegisterItem( BG_FindItemForWeapon( WP_10 ) );	/* precache the weapon */
		}
		else
		{
			/* temp gfx and sounds */
			RegisterItem( BG_FindItemForWeapon( WP_4 ) );	/* precache the weapon */
		}
	}
	else
	{
		RegisterItem( BG_FindItemForWeapon( WP_4 ) );	/* precache the weapon */
	}
}

/**
 * \brief Fires the laser of a laser arm.
 * 
 * Fires the laser of a  laser arm.
 *
 * @param ent the laser arm
 */
void laser_arm_fire (gentity_t *ent)
{
	vec3_t	start, end, fwd, rt, up;
	trace_t	trace;

	if ( ent->health < level.time && ent->booleanstate )
	{
		/* If I'm firing the laser and it's time to quit....then quit! */
		ent->booleanstate = qfalse;
		/*ent->e_ThinkFunc = thinkF_NULL;*/
		/*return;*/
	}

	ent->nextthink = ent->health = level.time + FRAMETIME;

	/* If a fool gets in the laser path, fry 'em */
	AngleVectors( ent->r.currentAngles, fwd, rt, up );

	VectorMA( ent->r.currentOrigin, 20, fwd, start );	
	/*VectorMA( start, -6, rt, start );*/
	/*VectorMA( start, -3, up, start );*/
	VectorMA( start, 4096, fwd, end );
	
	trap_Trace( &trace, start, NULL, NULL, end, -1, MASK_SHOT ); /* ignore */
	
	/* Only deal damage when in alt-fire mode */
	if ( trace.fraction < 1.0 && ent->booleanstate )
	{
		if ( trace.entityNum < ENTITYNUM_WORLD )
		{
			gentity_t *hapless_victim = &g_entities[trace.entityNum];
			if ( hapless_victim && hapless_victim->takedamage && ent->damage )
			{
				G_Damage( hapless_victim, ent, ent->nextTrain->activator, fwd, trace.endpos, ent->damage, DAMAGE_ALL_TEAMS, 1 );
			}
		}
	}
	
	if ( ent->booleanstate )
	{
		ent->s.origin2[0] = trace.endpos[0];
		ent->s.origin2[1] = trace.endpos[1];
		ent->s.origin2[2] = trace.endpos[2];

		ent->s.angles[0] = trace.plane.normal[0];
		ent->s.angles[1] = trace.plane.normal[1];
		ent->s.angles[2] = trace.plane.normal[2];

		ent->s.angles2[0] = ent->nextTrain->startRGBA[0];
		ent->s.angles2[1] = ent->nextTrain->startRGBA[1];
		ent->s.angles2[2] = ent->nextTrain->startRGBA[2];
		/*ent->s.scale = ent->nextTrain->startRGBA[3];*/

		G_AddEvent( ent, EV_LASERTURRET_FIRE, 0 );
	}
	else
	{
		ent->s.origin2[0] = trace.endpos[0];
		ent->s.origin2[1] = trace.endpos[1];
		ent->s.origin2[2] = trace.endpos[2];

		ent->s.angles[0] = trace.plane.normal[0];
		ent->s.angles[1] = trace.plane.normal[1];
		ent->s.angles[2] = trace.plane.normal[2];

		G_AddEvent( ent, EV_LASERTURRET_AIM, 0 );
	}
}

/**
 * \brief Use a laser arm.
 *
 * Use function of the laser arm entity.
 *
 * @param self the laser arm
 * @param other another entity
 * @param activator the activator
 */
void laser_arm_use (gentity_t *self, gentity_t *other, gentity_t *activator)
{
	vec3_t	newAngles;

	self->activator = activator;
	switch( self->count )
	{
	case 0:
	default:
		/* Fire */
		/* trap_Printf("FIRE!\n"); */
		self->lastEnemy->lastEnemy->think = laser_arm_fire;
		self->lastEnemy->lastEnemy->nextthink = level.time + FRAMETIME;
		/* For 3 seconds */
		self->lastEnemy->lastEnemy->booleanstate = qtrue; /* Let 'er rip! */
		self->lastEnemy->lastEnemy->last_move_time = level.time + self->lastEnemy->lastEnemy->wait;
		G_Sound(self->lastEnemy->lastEnemy, G_SoundIndex("sound/enemies/l_arm/fire.wav"));
		break;
	case 1:
		/* Yaw left */
		/*trap_Printf("LEFT...\n");*/
		VectorCopy( self->lastEnemy->r.currentAngles, newAngles );
		newAngles[1] += self->speed;
		VectorCopy( newAngles, self->lastEnemy->s.apos.trBase );
		bolt_head_to_arm( self->lastEnemy, self->lastEnemy->lastEnemy, LARM_FOFS, LARM_ROFS, LARM_UOFS );
		G_Sound( self->lastEnemy, G_SoundIndex( "sound/enemies/l_arm/move.wav" ) );
		break;
	case 2:
		/* Yaw right */
		/*trap_Printf("RIGHT...\n");*/
		VectorCopy( self->lastEnemy->r.currentAngles, newAngles );
		newAngles[1] -= self->speed;
		VectorCopy( newAngles, self->lastEnemy->s.apos.trBase );
		bolt_head_to_arm( self->lastEnemy, self->lastEnemy->lastEnemy, LARM_FOFS, LARM_ROFS, LARM_UOFS );
		G_Sound( self->lastEnemy, G_SoundIndex( "sound/enemies/l_arm/move.wav" ) );
		break;
	case 3:
		/* pitch up */
		/*trap_Printf("UP...\n");*/
		/* FIXME: Clamp */
		VectorCopy( self->lastEnemy->lastEnemy->r.currentAngles, newAngles );
		newAngles[0] -= self->speed;
		if ( newAngles[0] < -45 )
		{
			newAngles[0] = -45;
		}
		VectorCopy( newAngles, self->lastEnemy->lastEnemy->s.apos.trBase );
		G_Sound( self->lastEnemy->lastEnemy, G_SoundIndex( "sound/enemies/l_arm/move.wav" ) );
		break;
	case 4:
		/* pitch down */
		/*trap_Printf("DOWN...\n");*/
		/* FIXME: Clamp */
		VectorCopy( self->lastEnemy->lastEnemy->r.currentAngles, newAngles );
		newAngles[0] += self->speed;
		if ( newAngles[0] > 90 )
		{
			newAngles[0] = 90;
		}
		VectorCopy( newAngles, self->lastEnemy->lastEnemy->s.apos.trBase );
		G_Sound( self->lastEnemy->lastEnemy, G_SoundIndex( "sound/enemies/l_arm/move.wav" ) );
		break;
	}
}

/*QUAKED misc_laser_arm (1 0 0) (-8 -8 -8) (8 8 8) 
-----DESCRIPTION-----
What it does when used depends on it's "count" (can be set by a lua-script)

-----SPAWNFLAGS-----
none

-----KEYS-----
	count:
		0 (default) - Fire in direction facing
		1 turn left
		2 turn right
		3 aim up
		4 aim down

  speed - How fast it turns (degrees per second, default 30)
  dmg	- How much damage the laser does 10 times a second (default 5 = 50 points per second)
  wait  - How long the beam lasts, in seconds (default is 3)
  
  targetname - to use it
  target - What thing for it to be pointing at to start with

  "startRGBA" - laser color, Red Green Blue Alpha, range 0 to 1 (default  1.0 0.85 0.15 0.75 = Yellow-Orange)
*/

/**
 * \brief Start the laser arm.
 *
 * Start the laser arm.
 *
 * @param base the laser arm's base
 */
void laser_arm_start (gentity_t *base)
{
	vec3_t	armAngles;
	vec3_t	headAngles;
	gentity_t *arm;
	gentity_t *head;

	base->think = 0;
	/* We're the base, spawn the arm and head */
	arm = G_Spawn();
	head = G_Spawn();

	VectorCopy( base->s.angles, armAngles );
	VectorCopy( base->s.angles, headAngles );
	if ( base->target && base->target[0] )
	{/* Start out pointing at something */
		gentity_t *targ = G_Find( NULL, FOFS(targetname), base->target );
		if ( !targ )
		{/* couldn't find it! */
			Com_Printf(S_COLOR_RED "ERROR : laser_arm can't find target %s!\n", base->target);
		}
		else
		{/* point at it */
			vec3_t	dir, angles;

			VectorSubtract(targ->r.currentOrigin, base->s.origin, dir );
			vectoangles( dir, angles );
			armAngles[1] = angles[1];
			headAngles[0] = angles[0];
			headAngles[1] = angles[1];
		}
	}

	/* Base */
	/* Base does the looking for enemies and pointing the arm and head */
	VectorCopy( base->s.angles, base->s.apos.trBase );
	/*base->s.origin[2] += 4;*/
	G_SetOrigin(base, base->s.origin);
	trap_LinkEntity(base);
	/* FIXME: need an actual model */
	base->s.modelindex = G_ModelIndex("models/mapobjects/dn/laser_base.md3");
	base->s.eType = ET_GENERAL;

	if ( !base->startRGBA )
	{	base->startRGBA[0] = 1.0;
		base->startRGBA[1] = 0.85;
		base->startRGBA[2] = 0.15 ;
		base->startRGBA[3] = 0.75;
	}
	/* anglespeed - how fast it can track the player, entered in degrees per second, so we divide by FRAMETIME/1000 */
	if ( !base->speed )
	{
		base->speed = 3.0f;
	}
	else
	{
		base->speed *= FRAMETIME/1000.0f;
	}
	base->use = laser_arm_use;
	base->nextthink = level.time + FRAMETIME;

	/* Arm */
	/* Does nothing, not solid, gets removed when head explodes */
	G_SetOrigin( arm, base->s.origin );
	trap_LinkEntity(arm);
	VectorCopy( armAngles, arm->s.apos.trBase );
	bolt_head_to_arm( arm, head, LARM_FOFS, LARM_ROFS, LARM_UOFS );
	arm->s.modelindex = G_ModelIndex("models/mapobjects/dn/laser_arm.md3");

	/* Head */
	/* Fires when enemy detected, animates, can be blown up */
	/* Need to normalize the headAngles pitch for the clamping later */
	if ( headAngles[0] < -180 )
	{
		headAngles[0] += 360;
	}
	else if ( headAngles[0] > 180 )
	{
		headAngles[0] -= 360;
	}
	VectorCopy( headAngles, head->s.apos.trBase );
	head->s.modelindex = G_ModelIndex("models/mapobjects/dn/laser_head.md3");
	head->s.eType = ET_GENERAL;
	/*head->r.svFlags |= SVF_BROADCAST;*/ /* Broadcast to all clients */
	VectorSet( head->r.mins, -8, -8, -8 );
	VectorSet( head->r.maxs, 8, 8, 8 );
	head->r.contents = CONTENTS_BODY;
	/* FIXME: make an index into an external string table for localization */
	head->message = "Surgical Laser";
	trap_LinkEntity(head);

	/* dmg */
	if ( !base->damage )
	{
		head->damage = 5;
	}
	else
	{
		head->damage = base->damage;
	}
	base->damage = 0;
	/* lifespan of beam */
	if ( !base->wait )
	{
		head->wait = 3000;
	}
	else
	{
		head->wait = base->wait * 1000;
	}
	base->wait = 0;

	/* Precache firing and explode sounds */
	G_SoundIndex("sound/weapons/explosions/cargoexplode.wav");
	G_SoundIndex("sound/enemies/l_arm/fire.wav");
	G_SoundIndex("sound/enemies/l_arm/move.wav");

	/* Link them up */
	base->lastEnemy = arm;
	arm->lastEnemy = head;
	head->parent = arm;
	arm->nextTrain = head->nextTrain = base;

	/* The head should always think, since it will be either firing a damage laser or just a target laser */
	head->think = laser_arm_fire;
	head->nextthink = level.time + FRAMETIME;
	head->booleanstate = qfalse; /* Don't do damage until told to */
}

/**
 * \brief Spawn a laser arm.
 *
 * Spawn function of the laser arm entity.
 *
 * @param base the laser arms base
 */
void SP_laser_arm (gentity_t *base)
{
	base->think = laser_arm_start;
	base->nextthink = level.time + FRAMETIME;
}

