// Copyright (C) 1999-2000 Id Software, Inc.
//
#include "g_local.h"

#define	MISSILE_PRESTEP_TIME	50

// ick
extern void grenadeSpewShrapnel( gentity_t *ent );

/*
================
G_BounceMissile

  returns true if a bounce sound should be played
================
*/
qboolean G_BounceMissile( gentity_t *ent, trace_t *trace ) {
	vec3_t	velocity;
	float	dot;
	int		hitTime;

	// reflect the velocity on the trace plane
	hitTime = level.previousTime + ( level.time - level.previousTime ) * trace->fraction;
	BG_EvaluateTrajectoryDelta( &ent->s.pos, hitTime, velocity );
	dot = DotProduct( velocity, trace->plane.normal );
	VectorMA( velocity, -2*dot, trace->plane.normal, ent->s.pos.trDelta );

	if ( ent->s.eFlags & EF_BOUNCE_HALF ) {
		VectorScale( ent->s.pos.trDelta, 0.65, ent->s.pos.trDelta );
		ent->s.pos.trDelta[2]*=0.5f;
		// check for stop
		if ( trace->plane.normal[2] > 0.2 && VectorLength( ent->s.pos.trDelta ) < 40 ) {
			G_SetOrigin( ent, trace->endpos );
			return qfalse;
		}
	}

	VectorAdd( ent->r.currentOrigin, trace->plane.normal, ent->r.currentOrigin);
	VectorCopy( ent->r.currentOrigin, ent->s.pos.trBase );
	SnapVector( ent->s.pos.trBase );			// save net bandwidth
	ent->s.pos.trTime = level.time;
	return qtrue;
}

void TouchStickyGrenade(gentity_t *ent, gentity_t *other, trace_t *trace)
{
	// if the guy that touches this grenade can take damage, he's about to.
	//RPG-X: RedTechie - Fixed bug when other admins walk threw other admins grenades it kills them
	//if(other->client->sess.sessionClass != PC_ADMIN){
	if (IsAdmin( other ) == qfalse) {
		if (other->takedamage)
		{
			if (ent->parent != other)
			{
				ent->touch = 0;
				ent->nextthink = level.time + FRAMETIME;
				ent->think = grenadeSpewShrapnel;
			}
		}
	}
}

//---------------------------------------------------------
void tripwireThink ( gentity_t *ent )
//---------------------------------------------------------
{
	gentity_t	*traceEnt;
	vec3_t		start;
	vec3_t		end;
	trace_t		tr;

	ent->s.eFlags |= EF_FIRING;
	ent->r.contents = CONTENTS_TRIGGER; //RPG-X: - RedTechie added
	//FIXME: wait until we latch
	ent->think = tripwireThink;
	ent->nextthink = level.time + FRAMETIME;

	start[0] = 0.0f;
	start[1] = 0.0f;
	start[2] = -2.0f;

	VectorAdd(start, ent->s.pos.trBase, start);

	// Find the main impact point
	VectorMA ( ent->s.pos.trBase, 1024, ent->movedir, end );

//RPG-X: J2J - Replaced the NULL,NULL, with actual vectors contains origin info, to prevent game from crashing.
	trap_Trace ( &tr, start, NULL, NULL, end, ent->s.number, MASK_SHOT );


	traceEnt = &g_entities[ tr.entityNum ];


/*RPG-X: Redtechie - No idea what this code is for but i commented it out if
something is broke regarding tripmines uncomment

	if ( g_gametype.integer >= GT_TEAM )
	{
		if ( traceEnt->client && traceEnt->client->sess.sessionTeam != ent->s.otherEntityNum2 || tr.startsolid )
		{
			grenadeSpewShrapnel( ent );
		}
	}
	else
	{
		if ( traceEnt->client || tr.startsolid )
		{
			grenadeSpewShrapnel( ent );
		}
	}
//}*/

//RPG-X: J2J If statment fixed so that touching a mine won't crash the game.
	
	if ( traceEnt->client )
	{

//RPG-X: RedTechie - Admin Class go threw
//RPG-X: J2J - Mistake made here, was checking if the _owner_ was in admin at the time, not the player tripping the mine, FIXED.
		
		//if(traceEnt->client->sess.sessionClass != PC_ADMIN)
		if (IsAdmin( traceEnt ) == qfalse)
		{
				grenadeSpewShrapnel( ent );
		}	
	}

}

void tripmine_explode( gentity_t *self )
{
		G_TempEntity ( self->s.origin, EV_GRENADE_EXPLODE );
		G_RadiusDamage( self->s.origin, self->enemy?self->enemy:self->parent, self->splashDamage, self->splashRadius, self, DAMAGE_ALL_TEAMS, MOD_GRENADE_ALT_SPLASH );
		//FIXME: clear me from owner's list of tripmines
		G_FreeEntity(self);
}

void tripmine_delayed_explode( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath )
{
	self->enemy = attacker;
	self->think = grenadeSpewShrapnel;
	self->nextthink = level.time + FRAMETIME;
	self->takedamage = qfalse;
}

/*
================
G_MissileStick

================
*/
#define GRENADE_ALT_STICK_TIME		2500
void G_MissileStick( gentity_t *ent, trace_t *trace )
{
	vec3_t	org, dir;
	gentity_t	*tent = NULL;
	gentity_t		*other = NULL;
	gclient_t	*client;

	client = ent->client;

	other = &g_entities[trace->entityNum];

	if (other->takedamage)
	{
		// using grenade as a direct fire weapon. hit someone. explode.
		ent->splashMethodOfDeath = MOD_GRENADE_ALT;
		grenadeSpewShrapnel(ent);
		return;
	}

	// send client a message to start the grenade a-beepin'
	tent = G_TempEntity( ent->r.currentOrigin, EV_GRENADE_SHRAPNEL );

	// Back away from the wall
	//VectorMA( trace->endpos, 3.5, trace->plane.normal, org );
	VectorMA( trace->endpos, -2.5, trace->plane.normal, org );
	G_SetOrigin( ent, org );
	VectorCopy( trace->plane.normal, ent->pos1 );

	VectorClear( ent->s.apos.trDelta );
	// This will orient the object to face in the direction of the normal
	/*
	VectorScale( trace->plane.normal, -1, ent->s.pos.trDelta );
	ent->s.pos.trTime = level.time;
	SnapVector( ent->s.pos.trDelta );			// save net bandwidth
	*/

	ent->s.pos.trTime = level.time;
	VectorClear( ent->s.pos.trDelta );
	VectorClear( ent->s.apos.trBase );
	ent->s.pos.trType = TR_STATIONARY;
	VectorScale ( trace->plane.normal, -1, dir );
	vectoangles( dir, ent->s.angles );
	SnapVector( ent->s.angles );			// save net bandwidth

	// check FX_GrenadeShrapnelBits() to make sure this nextthink coincides with that killtime
	if ( ent->count )
	{//a tripwire
		//add draw line flag
		//RPG-X: Redtechie - hidden grenades
		//ent->flags = FL_CLOAK;
		//ent->client->ps.powerups[PW_INVIS] = level.time + 1000000000;
		VectorCopy( trace->plane.normal, ent->movedir );
		ent->think = tripwireThink;
		ent->nextthink = level.time + 300;//delay the activation
		//make it shootable
		VectorSet( ent->r.mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );
		VectorSet( ent->r.maxs, ITEM_RADIUS, ITEM_RADIUS, ITEM_RADIUS );
		ent->r.contents = CONTENTS_CORPSE;//|CONTENTS_TRIGGER;
		//RPG-X|FIXME-NOT FIXED: RedTechie - Maker can shoot down others cant
		//if( ent->enemy->client->sess.sessionClass != PC_MAKER )
		//{
			ent->takedamage = 0;
			ent->health = 999;
		//}
		//else
		//{
		//	ent->takedamage = 1;
		//	ent->health = 1;
		//}
		ent->die = tripmine_delayed_explode;
		ent->touch = TouchStickyGrenade;//?
	}
	else
	{
		ent->touch = TouchStickyGrenade;
		ent->r.contents = CONTENTS_TRIGGER;
		ent->nextthink = level.time + GRENADE_ALT_STICK_TIME;
		ent->think = grenadeSpewShrapnel;
	}

	ent->s.groundEntityNum = trace->entityNum;
}


/*
================
G_MissileImpact

================
*/
void G_MissileImpact( gentity_t *ent, trace_t *trace ) {
	gentity_t		*other = NULL, *tent = NULL;
	//qboolean		hitClient = qfalse;

	other = &g_entities[trace->entityNum];

	// check for bounce
	if (	!other->takedamage &&
			(ent->s.eFlags & ( EF_BOUNCE | EF_BOUNCE_HALF )) )
	{
		// Check to see if there is a bounce count
		if ( ent->count ) {
			// decrement number of bounces and then see if it should be done bouncing
			if ( !(--ent->count) ) {
				// He (or she) will bounce no more (after this current bounce, that is).
				ent->s.eFlags &= ~( EF_BOUNCE | EF_BOUNCE_HALF );
			}
		}

		if (G_BounceMissile( ent, trace ))
		{
			// fixme. shouldn't the normal of the trace be passed to cgame?
			//G_AddEvent( ent, EV_GRENADE_BOUNCE, 0 );
			tent = G_TempEntity( trace->endpos, EV_GRENADE_BOUNCE );
			tent->s.weapon = ent->s.weapon;
			VectorCopy(trace->plane.normal, tent->s.angles2);
			VectorShort(tent->s.angles2);
		}
		return;
	}

	// check for sticking
	if ( ( ent->s.eFlags & EF_MISSILE_STICK ) ) 
	{
		// kinda cheap, but if a sticky grenade hits a forcefield or a moving, explode
		if ( other && ( (other->classname && !Q_stricmp(other->classname, "holdable_shield")) || (!VectorCompare( vec3_origin, other->s.pos.trDelta ) && other->s.pos.trType!=TR_STATIONARY) || (!VectorCompare( vec3_origin, other->s.apos.trDelta ) && other->s.apos.trType!=TR_STATIONARY) ) )
		{
			//RPG-X: - Our forcefield killer :D
			if(other && ((other->classname && !Q_stricmp(other->classname, "holdable_shield")))){
				if(IsAdmin(ent->parent)){
					G_FreeEntity(ent);
					ShieldRemove(other);
					return;
				}else{
					// can't call grenadeSpewShrapnel right here or G_RunMissile will puke
					ent->think = grenadeSpewShrapnel;
					// set our next think to right now. our think fn will get called this frame.
					ent->nextthink = level.time;
					trap_LinkEntity( ent );
					return;
				}
			}else{
				// can't call grenadeSpewShrapnel right here or G_RunMissile will puke
				ent->think = grenadeSpewShrapnel;
				// set our next think to right now. our think fn will get called this frame.
				ent->nextthink = level.time;
				trap_LinkEntity( ent );
				return;
			}
		}

		G_MissileStick( ent, trace );
		G_AddEvent( ent, EV_MISSILE_STICK, 0 );
		return;
	}

	// impact damage
	if (other->takedamage) {
		// FIXME: wrong damage direction?
		if ( ent->damage ) {
			vec3_t	velocity;
			int		flags = 0;

			/*if( LogAccuracyHit( other, &g_entities[ent->r.ownerNum] ) ) {
				g_entities[ent->r.ownerNum].client->ps.persistant[PERS_ACCURACY_HITS]++;
				hitClient = qtrue;
			}*/
			BG_EvaluateTrajectoryDelta( &ent->s.pos, level.time, velocity );
			if ( VectorLength( velocity ) == 0 ) {
				velocity[2] = 1;	// stepped on a grenade
			}
			if (MOD_STASIS == ent->methodOfDeath)
			{
				flags |= DAMAGE_ARMOR_PIERCING;
			}
			G_Damage (other, ent, &g_entities[ent->r.ownerNum], velocity,
				ent->s.origin, ent->damage, 
				flags, ent->methodOfDeath);
		}
	}

	// is it cheaper in bandwidth to just remove this ent and create a new
	// one, rather than changing the missile into the explosion?

	if ( other->takedamage && other->client )
	{
		G_AddEvent( ent, EV_MISSILE_HIT, DirToByte( trace->plane.normal ) );
		ent->s.otherEntityNum = other->s.number;
	}
	else
	{
		G_AddEvent( ent, EV_MISSILE_MISS, DirToByte( trace->plane.normal ) );
	}

	ent->freeAfterEvent = qtrue;

	// change over to a normal entity right at the point of impact
	ent->s.eType = ET_GENERAL;

	SnapVectorTowards( trace->endpos, ent->s.pos.trBase );	// save net bandwidth

	G_SetOrigin( ent, trace->endpos );

	// splash damage (doesn't apply to person directly hit)
	if ( ent->splashDamage ) {
		if( G_RadiusDamage( trace->endpos, ent->parent, ent->splashDamage, ent->splashRadius, 
			other, 0, ent->splashMethodOfDeath ) ) {
			/*if( !hitClient ) {
				g_entities[ent->r.ownerNum].client->ps.persistant[PERS_ACCURACY_HITS]++;
			}*/
		}
	}

	trap_LinkEntity( ent );
}

/*
================
G_ExplodeMissile

Explode a missile without an impact
================
*/
void G_ExplodeMissile( gentity_t *ent ) {
	vec3_t		dir;
	vec3_t		origin;

	BG_EvaluateTrajectory( &ent->s.pos, level.time, origin );
	SnapVector( origin );
	G_SetOrigin( ent, origin );

	// we don't have a valid direction, so just point straight up
	dir[0] = dir[1] = 0;
	dir[2] = 1;

	ent->s.eType = ET_GENERAL;
	G_AddEvent( ent, EV_MISSILE_MISS, DirToByte( dir ) );

	ent->freeAfterEvent = qtrue;

	// splash damage
	if ( ent->splashDamage ) {
		if( G_RadiusDamage( ent->r.currentOrigin, ent->parent, ent->splashDamage, ent->splashRadius, NULL
			, 0, ent->splashMethodOfDeath ) ) {
			//g_entities[ent->r.ownerNum].client->ps.persistant[PERS_ACCURACY_HITS]++;
		}
	}

	trap_LinkEntity( ent );
}

void G_RunStuckMissile( gentity_t *ent )
{
	if ( ent->takedamage )
	{
		if ( ent->s.groundEntityNum >= 0 && ent->s.groundEntityNum < ENTITYNUM_WORLD )
		{
			gentity_t *other = &g_entities[ent->s.groundEntityNum];

			if ( (!VectorCompare( vec3_origin, other->s.pos.trDelta ) && other->s.pos.trType != TR_STATIONARY) || 
				(!VectorCompare( vec3_origin, other->s.apos.trDelta ) && other->s.apos.trType != TR_STATIONARY) )
			{//thing I stuck to is moving or rotating now, kill me
				G_Damage( ent, other, other, NULL, NULL, 99999, 0, MOD_CRUSH );
				return;
			}
		}
	}
	// check think function
	G_RunThink( ent );
}

/*
================
G_RunMissile

================
*/
void G_RunMissile( gentity_t *ent ) {
	vec3_t		origin;
	trace_t		tr;

	// get current position
	BG_EvaluateTrajectory( &ent->s.pos, level.time, origin );

	// trace a line from the previous position to the current position,
	// ignoring interactions with the missile owner
	trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin, 
		ent->r.ownerNum, ent->clipmask );

	VectorCopy( tr.endpos, ent->r.currentOrigin );

	if ( tr.startsolid ) {
		tr.fraction = 0;
	}

	trap_LinkEntity( ent );

	if ( tr.fraction != 1 && !tr.allsolid) {
		// never explode or bounce on sky
		if ( tr.surfaceFlags & SURF_NOIMPACT ) {
			G_FreeEntity( ent );
			return;
		}

		G_MissileImpact( ent, &tr );
		if ( (ent->s.eType != ET_MISSILE) && (ent->s.eType != ET_ALT_MISSILE) ) {
			return;		// exploded
		}
	}

	if ( ent->s.pos.trType == TR_STATIONARY && (ent->s.eFlags&EF_MISSILE_STICK) )
	{//stuck missiles should check some special stuff
		G_RunStuckMissile( ent );
		return;
	}
	// check think function after bouncing
	G_RunThink( ent );
}


//=============================================================================

/*
=================
fire_plasma

=================
*/
gentity_t *fire_plasma (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "plasma";
	bolt->nextthink = level.time + 10000;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_4;
	bolt->r.ownerNum = self->s.number;
	bolt->parent = self;
	bolt->damage = 20;
	bolt->splashDamage = 15;
	bolt->splashRadius = 20;
	bolt->methodOfDeath = MOD_SCAVENGER_ALT;
	bolt->splashMethodOfDeath = MOD_SCAVENGER_ALT_SPLASH;
	bolt->clipmask = MASK_SHOT;

	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	SnapVector( bolt->s.pos.trBase );			// save net bandwidth
	VectorScale( dir, 2000, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth

	VectorCopy (start, bolt->r.currentOrigin);

	return bolt;
}

gentity_t *fire_comprifle(gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t *bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "plasma";
	bolt->nextthink = level.time + 10000;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_6;
	bolt->r.ownerNum = self->s.number;
	bolt->parent = self;
	bolt->damage = 20;
	bolt->splashDamage = 15;
	bolt->splashRadius = 20;
	bolt->methodOfDeath = MOD_CRIFLE;
	bolt->splashMethodOfDeath = MOD_CRIFLE_SPLASH;
	bolt->clipmask = MASK_SHOT;

	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	SnapVector( bolt->s.pos.trBase );			// save net bandwidth
	VectorScale( dir, 2000, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth

	VectorCopy (start, bolt->r.currentOrigin);

	G_AddEvent(self, EV_SHOOTER_SOUND, WP_6);

	return bolt;
}

//=============================================================================


/*
=================
fire_quantum

=================
*/
gentity_t *fire_quantum (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "plasma";
	bolt->nextthink = level.time + 10000;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_9;
	bolt->r.ownerNum = self->s.number;
	bolt->parent = self;
	bolt->damage = 20;
	bolt->splashDamage = 15;
	bolt->splashRadius = 20;
	bolt->methodOfDeath = MOD_QUANTUM;
	bolt->splashMethodOfDeath = MOD_QUANTUM_SPLASH;
	bolt->clipmask = MASK_SHOT;

	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	SnapVector( bolt->s.pos.trBase );			// save net bandwidth
	VectorScale( dir, 2000, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth

	VectorCopy (start, bolt->r.currentOrigin);

	G_AddEvent(self, EV_SHOOTER_SOUND, WP_9);

	return bolt;
}	

//=============================================================================

/*
=================
fire_grenade
=================
*/
gentity_t *fire_grenade (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "grenade";
	bolt->nextthink = level.time + 2500;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_8;
	bolt->s.eFlags = EF_BOUNCE_HALF;
	bolt->r.ownerNum = self->s.number;
	bolt->parent = self;
	bolt->damage = 100;
	bolt->splashDamage = 100;
	bolt->splashRadius = 150;
	bolt->methodOfDeath = MOD_GRENADE;
	bolt->splashMethodOfDeath = MOD_GRENADE_SPLASH;
	bolt->clipmask = MASK_SHOT;

	bolt->s.pos.trType = TR_GRAVITY;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	SnapVector( bolt->s.pos.trBase );			// save net bandwidth
	VectorScale( dir, 700, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth

	VectorCopy (start, bolt->r.currentOrigin);

	G_AddEvent(self, EV_SHOOTER_SOUND, WP_8);

	return bolt;
}


//=============================================================================


/*
=================
fire_rocket
=================
*/
gentity_t *fire_rocket (gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t	*bolt;

	VectorNormalize (dir);

	bolt = G_Spawn();
	bolt->classname = "rocket";
	bolt->nextthink = level.time + 10000;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_10;
	bolt->r.ownerNum = self->s.number;
	bolt->parent = self;
	bolt->damage = 100;
	bolt->splashDamage = 100;
	bolt->splashRadius = 120;
	bolt->methodOfDeath = MOD_QUANTUM;
	bolt->splashMethodOfDeath = MOD_QUANTUM_SPLASH;
	bolt->clipmask = MASK_SHOT;

	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	SnapVector( bolt->s.pos.trBase );			// save net bandwidth
	VectorScale( dir, 900, bolt->s.pos.trDelta );
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, bolt->r.currentOrigin);

	G_AddEvent(self, EV_SHOOTER_SOUND, WP_10);

	return bolt;
}



