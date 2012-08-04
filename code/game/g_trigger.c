/*
 * Copyright (C) 1999-2000 Id Software, Inc.
 */

#include "g_local.h"

#define SF_SPECTATOR	(1<<0)
#define SF_RANDOM	(1<<1)

/**
 * \brief Inits a trigger entity.
 *
 * Initializes trigger entities.
 *
 * @param self the trigger entity
 */
void InitTrigger( gentity_t *self ) {
	if (!VectorCompare (self->s.angles, vec3_origin))
		G_SetMovedir (self->s.angles, self->movedir);

	if(!self->tmpEntity) /* for spawnTent command */
		trap_SetBrushModel( self, self->model );
	self->r.contents = CONTENTS_TRIGGER;		/* replaces the -1 from trap_SetBrushModel */
	self->r.svFlags = SVF_NOCLIENT;
}


// the wait time has passed, so set back up for another activation
/**
 * \brief Reactivate a trigger_multiple after the wait time has passed.
 * 
 * Reactivates the trigger_multiple after the wait time has passed.
 *
 * @param ent the trigger
 */
void multi_wait( gentity_t *ent ) {
	ent->nextthink = 0;
}

/**
 * \brief Function that gets called when a trigger_multiple has been ... umm well triggered ;)
 *
 * The trigger_multiple was just activated. ent->activator should be set to the activator
 * so it can be held through a delay. May wait for the delay time until firing.
 *
 * @param ent the trigger
 * @param activator the activator
 */
void multi_trigger( gentity_t *ent, gentity_t *activator ) {
	ent->activator = activator;
	if ( ent->nextthink ) {
		return;		/* can't retrigger until the wait is over */
	}

	if ( activator->client && ((ent->spawnflags&4)||(ent->spawnflags&2)||(ent->spawnflags&1)) )
	{/* see if it's usable by this team */
		switch( activator->client->sess.sessionTeam )
		{
		case TEAM_RED:
			if ( !(ent->spawnflags&1) ) {
				return;/* red is not allowed */
			}
			break;
		case TEAM_BLUE:
			if ( !(ent->spawnflags&2) ) {
				return;/* blue is not allowed */
			}
			break;
		default:
			if ( (ent->spawnflags&4) ) {
				return;/* must be on a team */
			}
			break;
		}
	}

	G_UseTargets (ent, ent->activator);

	#ifdef G_LUA
	if(ent->luaTrigger && !(ent->nextthink))
	{
		if(activator)
		{
			LuaHook_G_EntityTrigger(ent->luaTrigger, ent->s.number, activator->s.number);
		}
		else
		{
			LuaHook_G_EntityTrigger(ent->luaTrigger, ent->s.number, ENTITYNUM_WORLD);
		}
	}
	#endif

	if ( ent->wait > 0 ) {
		ent->think = multi_wait;
		ent->nextthink = level.time + ( ent->wait + ent->random * crandom() ) * 1000;
	} else {
		/*
 		 *  we can't just remove (self) here, because this is a touch function
		 *  called while looping through area links...
		 */
		ent->touch = 0;
		ent->nextthink = level.time + FRAMETIME;
		ent->think = G_FreeEntity;
	}
}

/**
 * \brief Use function for trigger_multiple.
 *
 * Use function for trigger_multiple.
 *
 * @param ent the trigger
 * @param other another entity
 * @param activator the activator
 */
void Use_Multi( gentity_t *ent, gentity_t *other, gentity_t *activator ) {
	multi_trigger( ent, activator );
}

/**
 * \brief Touch function for trigger_multiple.
 *
 * Touch function for trigger_multiple.
 *
 * @param self the trigger
 * @param other touching entity
 * @param trace a trace
 */
void Touch_Multi( gentity_t *self, gentity_t *other, trace_t *trace ) {
	if( !other->client ) {
		return;
	}
	multi_trigger( self, other );
}

/*QUAKED trigger_multiple (.5 .5 .5) ? RED_OK BLUE_OK TEAM_ONLY
RED_OK - People on the red team can fire this trigger
BLUE_OK - People on the blue team can fire this trigger
TEAM_ONLY - Only people on red or blue can fire this trigger (not TEAM_FREE like in straight holomatch or spectators)

"wait" : Seconds between triggerings, 0.5 default, -1 = one time only.
"random"	wait variance, default is 0
Variable sized repeatable trigger.  Must be targeted at one or more entities.
so, the basic time between firing is a random time between
(wait - random) and (wait + random)
*/
/**
 * \brief Spawn function of trigger_multiple.
 *
 * Spawn function of trigger_multiple.
 *
 * @param ent the trigger
 */
void SP_trigger_multiple( gentity_t *ent ) {
	G_SpawnFloat( "wait", "0.5", &ent->wait );
	G_SpawnFloat( "random", "0", &ent->random );

	G_SetOrigin(ent, ent->s.origin);

	if ( ent->random >= ent->wait && ent->wait >= 0 ) {
		ent->random = ent->wait - FRAMETIME;
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Warning] trigger_multiple has random >= wait\n" ););
	}

	ent->touch = Touch_Multi;
	ent->use = Use_Multi;

	InitTrigger( ent );

	VectorCopy(ent->r.maxs, ent->s.origin2);
	VectorCopy(ent->r.mins, ent->s.angles2);

	trap_LinkEntity (ent);

	level.numBrushEnts++;
}



/*
==============================================================================

trigger_always

==============================================================================
*/

/**
 * \brief Think function of trigger_always.
 *
 * Think function of trigger_always.
 *
 * @param ent the trigger
 */
void trigger_always_think( gentity_t *ent ) {
	G_UseTargets(ent, ent);

	#ifdef G_LUA
	if(ent->luaTrigger)
	{
		LuaHook_G_EntityTrigger(ent->luaTrigger, ent->s.number, ent->s.number);
	}
	#endif

	G_FreeEntity( ent );
}

/*QUAKED trigger_always (.5 .5 .5) (-8 -8 -8) (8 8 8)
This trigger will always fire.  It is activated by the world.
*/
/**
 * \brief Spawn function of trigger_multiple.
 *
 * Spawn function of trigger_multiple.
 *
 * @param ent the trigger
 */
void SP_trigger_always (gentity_t *ent) {
	/* we must have some delay to make sure our use targets are present */
	ent->nextthink = level.time + 300;
	ent->think = trigger_always_think;
}


/*
==============================================================================

trigger_push

==============================================================================
*/

/**
 * \brief Touch function of trigger_push.
 *
 * Touch function of trigger_push.
 *
 * @param self the trigger
 * @param other the touching entity
 * @param trace a trace
 */
void trigger_push_touch (gentity_t *self, gentity_t *other, trace_t *trace ) {
	if ( !other->client ) {
		return;
	}
/*
	if ( other->client->ps.pm_type != PM_NORMAL ) {
		return;
	}
	if ( other->client->ps.powerups[PW_FLIGHT] ) {
		return;
	}
*/
	if ( other->client->ps.velocity[2] < 100 ) {
		/* don't play the event sound again if we are in a fat trigger */
		G_AddPredictableEvent( other, EV_JUMP_PAD, 0 );
	}
	VectorCopy (self->s.origin2, other->client->ps.velocity);
}


/**
 * \brief Calculate origin2 so the target apogee will be hit.
 *
 * Calculate origin2 so the target apogee will be hit.
 *
 * @param the trigger
 */
void AimAtTarget( gentity_t *self ) {
	gentity_t	*ent;
	vec3_t		origin;
	float		height, gravity, time, forward;
	float		dist;

	VectorAdd( self->r.absmin, self->r.absmax, origin );
	VectorScale ( origin, 0.5, origin );

	ent = G_PickTarget( self->target );
	if ( !ent ) {
		G_FreeEntity( self );
		return;
	}

	height = ent->s.origin[2] - origin[2];
	gravity = g_gravity.value;
	time = sqrt( height / ( .5 * gravity ) );
	if ( !time ) {
		G_FreeEntity( self );
		return;
	}

	/* set s.origin2 to the push velocity */
	VectorSubtract ( ent->s.origin, origin, self->s.origin2 );
	self->s.origin2[2] = 0;
	dist = VectorNormalize( self->s.origin2);

	forward = dist / time;
	VectorScale( self->s.origin2, forward, self->s.origin2 );

	self->s.origin2[2] = time * gravity;
}


/*QUAKED trigger_push (.5 .5 .5) ?
Must point at a target_position, which will be the apex of the leap.
This will be client side predicted, unlike target_push
*/
/**
 * \brief Spawn function of trigger_push.
 *
 * Spawn function of trigger_push.
 *
 * @param self the trigger
 */
void SP_trigger_push( gentity_t *self ) {
	InitTrigger (self);

	/* unlike other triggers, we need to send this one to the client */
	self->r.svFlags &= ~SVF_NOCLIENT;

	self->s.eType = ET_PUSH_TRIGGER;
	self->touch = trigger_push_touch;
	self->think = AimAtTarget;
	self->nextthink = level.time + FRAMETIME;
	trap_LinkEntity (self);

	VectorCopy(self->r.maxs, self->s.apos.trBase);
	VectorCopy(self->r.mins, self->s.pos.trBase);

	level.numBrushEnts++;
}

/**
 * \brief Use function of target_push.
 *
 * Use function of target_push.
 *
 * @param self the entity
 * @param other another entity
 * @param activator the activator
 */
void Use_target_push( gentity_t *self, gentity_t *other, gentity_t *activator ) {
	if ( !activator->client ) {
		return;
	}

	/* RPG-X: J2J noclip use */
	if ( activator->client->ps.pm_type != PM_NORMAL || activator->client->ps.pm_type != PM_NOCLIP) {
		return;
	}

	if ( activator->client->ps.powerups[PW_FLIGHT] ) {
		return;
	}

	VectorCopy (self->s.origin2, activator->client->ps.velocity);

	/* play fly sound every 1.5 seconds */
	if ( activator->fly_sound_debounce_time < level.time ) {
		activator->fly_sound_debounce_time = level.time + 1500;
		G_Sound( activator, self->noise_index );
	}
}

/*QUAKED target_push (.5 .5 .5) (-8 -8 -8) (8 8 8) ENERGYNOISE
Pushes the activator in the direction.of angle, or towards a target apex.
"speed"		defaults to 1000
if "ENERGYNOISE", play energy noise instead of windfly
*/
/**
 * \brief Spawn function of target_push.
 *
 * Spawn function of target_push.
 *
 * @param self the entity
 */
void SP_target_push( gentity_t *self ) {
	if (!self->speed) {
		self->speed = 1000;
	}
	G_SetMovedir (self->s.angles, self->s.origin2);
	VectorScale (self->s.origin2, self->speed, self->s.origin2);

	if ( self->spawnflags & 1 ) {
		self->noise_index = G_SoundIndex("sound/ambience/forge/antigrav.wav");
	} else {
		self->noise_index = G_SoundIndex("sound/misc/windfly.wav");	/* fixme need sound! */
	}
	if ( self->target ) {
		VectorCopy( self->s.origin, self->r.absmin );
		VectorCopy( self->s.origin, self->r.absmax );
		self->think = AimAtTarget;
		self->nextthink = level.time + FRAMETIME;
	}
	self->use = Use_target_push;
}

/*
==============================================================================

trigger_teleport

==============================================================================
*/

#define MAX_TRANSPORTER_POINTS 16

/**
 * \brief Select a random spawn point.  
 *
 * Select a random spawn point.
 * 
 * @return a random spawn point
 */
gentity_t *SelectRandomSpawnPoint( void )
{
	gentity_t	*spot = NULL;
	int			count = 0;
	int			selection = 0;
	gentity_t	*spots[MAX_TRANSPORTER_POINTS];
	char		*classname;

	classname = "info_player_deathmatch";

	while ((spot = G_Find (spot, FOFS(classname), classname)) != NULL)
	{
		spots[ count++ ] = spot;
		if (count == MAX_TRANSPORTER_POINTS)
		{
			break;
		}
	}

	if ( !count )
	{	/* no spawn points !!??! */
		return NULL;
	}

	selection = rand() % count;
	return spots[ selection ];
}

/**
 * \brief Touch function of trigger_teleport.
 *
 * Touch function of trigger_teleport.
 *
 * @param self the trigger
 * @param other the touching entity
 * @param trace a trace
 */
void trigger_teleporter_touch (gentity_t *self, gentity_t *other, trace_t *trace )
{
	gentity_t	*dest;
	vec3_t		destPoint;
	
	trace_t		tr;
	vec3_t		tracePoint;

	int			clientNum;

	if(self->flags & FL_LOCKED)
		return;

	if ( !other->client ) {
		return;
	}
	if ( other->client->ps.pm_type == PM_DEAD ) {
		return;
	}
	/* Spectators only? */
	if ( ( self->spawnflags & SF_SPECTATOR ) && 
		other->client->sess.sessionTeam != TEAM_SPECTATOR )
	{
		return;
	}

	clientNum = other->client->ps.clientNum;

	/* BOOKMARK J2J */
	if (self->spawnflags & SF_RANDOM)
	{
		/* find a random spawn point */
		dest = SelectRandomSpawnPoint();
	}
	else
	{
		dest = 	G_PickTarget( self->target );
	}
	if (!dest)
	{
		DEVELOPER(G_Printf (S_COLOR_YELLOW "[Entity-Error] Couldn't find teleporter destination\n"););
		return;
	}

	/* suspended */
	if ( self->spawnflags & 8 )
	{
		/* put the bottom of the player's bbox at the bottom of the target's bbox */
		VectorCopy(dest->s.origin, destPoint);
		destPoint[2] += dest->r.mins[2]; 
		destPoint[2] -= other->r.mins[2];
		/* fudge it upwards just a bit */
		destPoint[2] += 1;
	}
	else
	{
		VectorCopy( dest->s.origin, tracePoint );
		tracePoint[2] -= 4096.0f;

		trap_Trace( &tr, dest->s.origin, dest->r.mins, dest->r.maxs, tracePoint, dest->s.number, MASK_PLAYERSOLID ); 
		VectorCopy( tr.endpos, destPoint );

		/* offset the player's bounding box. */
		destPoint[2] += dest->r.mins[2];
		destPoint[2] -= other->r.mins[2];

		/* add 1 to ensure non-direct collision */
		destPoint[2] += 1;
	}

	if (self->health)
	{
		/*
 		 * TiM - Transporter VFX
		 * Only if no spitting though. Having spitting transporter with these VFX would look freaky weird
		 */
		if ( self->health == -1 && ( self->spawnflags & 4 ) ) {
			if ( TransDat[clientNum].beamTime == 0 ) {
				G_InitTransport( clientNum, destPoint, dest->s.angles );
			}
		}
		else {
			TransportPlayer(other, destPoint, dest->s.angles, (self->health == -1)?0:self->health);
		}
	}
	else
	{
		/* Transporter VFX */
		if ( self->spawnflags & 4 ) {
			if ( TransDat[clientNum].beamTime == 0 ) {
				G_InitTransport( clientNum, destPoint, dest->s.angles );
			}
		}
		else {
			TeleportPlayer( other, destPoint, dest->s.angles, TP_NORMAL );
		}
	}

	if(self->sound1to2 && self->wait && !(self->flags & FL_CLAMPED)) {
		G_AddEvent(self, EV_GENERAL_SOUND, self->sound1to2);
		self->flags ^= FL_CLAMPED;
	}
}

/**
 * \brief Think function of trigger_teleport.
 *
 * Think function of trigger_teleport.
 *
 * @param ent the trigger
 */
void trigger_teleport_think(gentity_t *ent) {
	ent->nextthink = -1;
	if(!(ent->flags & FL_LOCKED))
		ent->flags ^= FL_LOCKED;
	if(ent->wait && (ent->flags & FL_CLAMPED))
		ent->flags ^= FL_CLAMPED;
}

/**
 * \brief Use function of trigger_teleport.
 *
 * Use function of trigger_teleport.
 *
 * @param ent the trigger
 * @param other another entity
 * @param activator the activator
 */
void trigger_teleport_use(gentity_t *ent, gentity_t *other, gentity_t *activator) {
	if(!Q_stricmp(ent->swapname, activator->target))
		if(ent->flags & FL_LOCKED)
			ent->nextthink = level.time + (ent->wait * 1000);
		ent->flags ^= FL_LOCKED;
}


/*QUAKED trigger_teleport (.5 .5 .5) ? SPECTATOR RANDOM VISUAL_FX SUSPENDED DEACTIVATED SND_ONCE
Allows client side prediction of teleportation events.
Must point at a target_position, which will be the teleport destination.

--------- spawnflags --------

If spectator is set, only spectators can use this teleport
Spectator teleporters are not normally placed in the editor, but are created
automatically near doors to allow spectators to move through them

  RANDOM -- send player to random info_player_deathmatch spawn point
  VISUAL_FX -- plays the Star Trek transporter FX and beams the player out slowly
  SUSPENDED -- player appears with the bounding box aligned to the bottom of the target
  If this isn't set, the player materializes at the first solid surface under it

----------- keys ------------

 "swapname"   - ACTIVATE/DEACTIVATE (Using entity needs SELF/NOACTIVATOR)
 "wait"		  - time before trigger deactivates itself automatically
 "soundstart" - sound to play if triggered

 health -- default is original behavior (speed of 400), any other value will be the
 speed at which the player is spewed forth from the tranpsorter destination. -1
 if you want no speed. The transporter VISUAL_FX flag will only work if the health
 is set to 0 or -1 as it cannot support 'spewing'.
*/
/**
 * \brief Spawn function of trigger_teleport.
 *
 * Spawn function of trigger_teleport.
 *
 * @param self the trigger
 */
void SP_trigger_teleport( gentity_t *self )
{
	char *temp;

	InitTrigger (self);

	/*
 	 *  unlike other triggers, we need to send this one to the client
	 *  unless is a spectator trigger
	 */
	if ( self->spawnflags & SF_SPECTATOR )
	{
		self->r.svFlags |= SVF_NOCLIENT;
	}
	else
	{
		self->r.svFlags &= ~SVF_NOCLIENT;
	}

	self->s.eType = ET_TELEPORT_TRIGGER;
	self->touch = trigger_teleporter_touch;

	if(self->spawnflags & 16)
		self->flags ^= FL_LOCKED;

	self->use = trigger_teleport_use;

	if(self->wait) {
		self->nextthink = -1;
		self->think = trigger_teleport_think;
	}

	if(G_SpawnString("soundstart", "", &temp))
		self->sound1to2 = G_SoundIndex(temp);

	VectorCopy(self->r.maxs, self->s.origin2);
	VectorCopy(self->r.mins, self->s.angles2);

	trap_LinkEntity (self);

	level.numBrushEnts++;
}


/*
==============================================================================

trigger_hurt

==============================================================================
*/

/*QUAKED trigger_hurt (.5 .5 .5) ? START_OFF TOGGLE SILENT NO_PROTECTION SLOW EVO_PROTECT NO_ADMIN
Any entity that touches this will be hurt.
It does dmg points of damage each server frame
Targeting the trigger will toggle its on / off state.

SILENT			supresses playing the sound
TOGGLE			can be toggled
SLOW			changes the damage rate to once per second
NO_PROTECTION	*nothing* stops the damage
EVO_PROTECT		Evosuit protects the client
NO_ADMIN		admins don't get hurt

"dmg"			default 5 (whole numbers only)

*/
/**
 * \brief Use function of trigger_hurt.
 *
 * Use function of trigger hurt.
 *
 * @param self the trigger
 * @param other another entity
 * @param activator the activator
 */
void hurt_use( gentity_t *self, gentity_t *other, gentity_t *activator ) {
	self->count = !self->count;
}

/**
 * \brief Touch function of trigger_hurt.
 *
 * Touch function of trigger_hurt.
 *
 * @param self the trigger
 * @param other the touching entity
 * @param trace a trace
 */
void hurt_touch( gentity_t *self, gentity_t *other, trace_t *trace ) {
	int		dflags;

	if(!self->count) return;

	/*
 	 * RPG-X | Phenix | 8/8/204
	 * (If the guy is wearning an evosuit)
	 */
	if ((self->spawnflags & 32) && (other->flags & FL_EVOSUIT)) {
		return;
	}

	if(self->spawnflags & 64) {
		if(IsAdmin(other))
			return;
	}

	if ( !other->takedamage ) {
		return;
	}

	if ( self->timestamp > level.time ) {
		return;
	}

	/* RPG-X | Phenix | 9/8/2004 */
	if (other->health <= 1) {
		return;
	}

	if ( self->spawnflags & 16 ) {
		self->timestamp = level.time + 1000;
	} else {
		self->timestamp = level.time + FRAMETIME;
	}

	/* play sound */
	if ( !(self->spawnflags & 4) ) {
		G_Sound( other, self->noise_index );
	}

	if (self->spawnflags & 8)
		dflags = DAMAGE_NO_PROTECTION;
	else
		dflags = 0;
	G_Damage (other, self, self, NULL, NULL, self->damage, dflags, MOD_TRIGGER_HURT);
}

/**
 * \brief Spawn function of trigger_hurt.
 *
 * Spawn function of trigger_hurt.
 *
 * @param self the trigger
 */
void SP_trigger_hurt( gentity_t *self ) {
	InitTrigger (self);

	/* TiM - gets very annoying after a while */
	self->noise_index = G_SoundIndex( "sound/world/electro.wav" );
	self->touch = hurt_touch;

	if ( !self->damage ) {
		self->damage = 5;
	}

	self->r.contents = CONTENTS_TRIGGER;

	if ( self->spawnflags & 2 ) {
		self->use = hurt_use;
	}

	self->count = !(self->spawnflags & 1);

	/* link in to the world if starting active | this won't work */
	/*if ( ! (self->spawnflags & 1) ) {
		trap_LinkEntity (self);
	}*/
	VectorCopy(self->r.maxs, self->s.origin2);
	VectorCopy(self->r.mins, self->s.angles2);

	trap_LinkEntity(self);

	level.numBrushEnts++;
}


/*
==============================================================================

timer

==============================================================================
*/


/*QUAKED func_timer (0.3 0.1 0.6) (-8 -8 -8) (8 8 8) START_ON
This should be renamed trigger_timer...
Repeatedly fires its targets.
Can be turned on or off by using.

"wait"			base time between triggering all targets, default is 1
"random"		wait variance, default is 0
so, the basic time between firing is a random time between
(wait - random) and (wait + random)

*/
/**
 * \brief Think function of func_timer.
 *
 * Think function of func_timer.
 *
 * @param self the entity
 */
void func_timer_think( gentity_t *self ) {
	G_UseTargets (self, self->activator);
	/* set time before next firing */
	self->nextthink = level.time + 1000 * ( self->wait + crandom() * self->random );
}

/**
 * \brief Use function of func_timer.
 *
 * Use function of func_timer.
 *
 * @param self the entity
 * @param other another entity
 * @param activator the activator
 */
void func_timer_use( gentity_t *self, gentity_t *other, gentity_t *activator ) {
	self->activator = activator;

	/* if on, turn it off */
	if ( self->nextthink ) {
		self->nextthink = 0;
		return;
	}

	/* turn it on */
	func_timer_think (self);
}

/**
 * \brief Spawn function of func_timer.
 *
 * Spawn function of func_timer.
 *
 * @param self the entity
 */
void SP_func_timer( gentity_t *self ) {
	G_SpawnFloat( "random", "1", &self->random);
	G_SpawnFloat( "wait", "1", &self->wait );

	self->use = func_timer_use;
	self->think = func_timer_think;

	if ( self->random >= self->wait ) {
		self->random = self->wait - FRAMETIME;
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Warning] func_timer at %s has random >= wait\n", vtos( self->s.origin ) ););
	}

	if ( self->spawnflags & 1 ) {
		self->nextthink = level.time + FRAMETIME;
		self->activator = self;
	}

	self->r.svFlags = SVF_NOCLIENT;
}

/*QUAKED trigger_transporter (0.5 0.5 0.5) ?
This is used in combination with ui_transporter.

"wait"			time to wait before trigger gets deactivated again(in seconds, default 5)
"soundstart"	transport sound;
*/
/**
 * \brief Think function of trigger_transporter.
 *
 * Think function of trigger_transporter.
 *
 * @param ent the trigger
 */
void trigger_transporter_think(gentity_t *ent) {
	if(Q_stricmp(ent->classname, "tent")) {
		ent->nextthink = -1;
		ent->flags ^= FL_LOCKED;
		ent->flags ^= FL_CLAMPED;
	} else {
		ent->target_ent->flags ^= FL_LOCKED;
		ent->target_ent->flags ^= FL_CLAMPED;
		G_FreeEntity(ent);
	}
}

/**
 * \brief Think function of trigger_transporter for server change.
 *
 * Think function of trigger_transporter for server change.
 *
 * @param ent the trigger
 */
void trigger_transporter_serverchange(gentity_t *ent) {
	trap_SendServerCommand(ent->touched-g_entities, va("cg_connect \"%s\"\n", ent->targetname2));
	G_FreeEntity(ent);
}

extern srvChangeData_t srvChangeData;

/**
 * \brief Touch function of trigger_transporter.
 *
 * Touch function of trigger_transporter.
 *
 * @param ent the trigger
 * @param other the touching entity
 * @param trace a trace
 */
void trigger_transporter_touch(gentity_t *ent, gentity_t *other, trace_t *trace) {
	char 		*srv;
	vec3_t 		offset = { 0, 0, 12 };
	vec3_t 		target;
	gentity_t 	*targetEnt;
	gentity_t 	*tent;
	int		clientNum;

	if((ent->flags & FL_LOCKED) || (!other->client) || (other->flags & FL_CLAMPED)) return;

	clientNum = other->client->ps.clientNum;

	if(ent->sound1to2 && !(ent->flags & FL_CLAMPED)) {
		G_AddEvent(other, EV_GENERAL_SOUND, ent->sound1to2);
		ent->flags ^= FL_CLAMPED;
	}

	other->flags ^= FL_CLAMPED;

	if(!ent->count)
		if(ent->target_ent->target) {
			targetEnt = G_PickTarget(ent->target_ent->target);
			VectorAdd(targetEnt->s.origin, offset, target);
			G_InitTransport(clientNum, target, targetEnt->s.angles);
		} else {
			G_InitTransport(clientNum, ent->target_ent->s.origin, ent->target_ent->s.angles);
		}
	else {

		srv = srvChangeData.ip[ent->health];

		tent = G_Spawn();
		tent->think = trigger_transporter_serverchange;
		tent->nextthink = level.time + 3000;
		TransDat[clientNum].beamTime = level.time + 8000;
		other->client->ps.powerups[PW_BEAM_OUT] = level.time + 8000;
		tent->touched = other;
		tent->targetname2 = G_NewString(srv);


		tent = G_Spawn();
		tent->classname = G_NewString("tent");
		tent->target_ent = ent;
		tent->think = trigger_transporter_think;
		tent->nextthink = level.time + ent->wait;
	}
}

/**
 * \brief Delay think function of trigger_transporter.
 *
 * Delay think function of trigger_transporter.
 *
 * @param ent the trigger
 */
void trigger_transporter_delay(gentity_t *ent) {
	ent->think = trigger_teleport_think;
	ent->nextthink = level.time + ent->wait;
	ent->flags ^= FL_LOCKED;
}

/**
 * \brief Spawn function of trigger_transporter.
 *
 * Spawn function of trigger_transporter.
 *
 * @param ent the trigger
 */
void SP_trigger_transporter(gentity_t *ent) {
	char		*temp;
	InitTrigger(ent);
	
	if(!ent->wait) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] trigger_transporter without wait at %s!\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}

	if(G_SpawnString("soundstart", "", &temp))
		ent->sound1to2 = G_SoundIndex(temp);

	if(!ent->wait)
		ent->wait = 5;
	ent->wait *= 1000;

	ent->touch = trigger_transporter_touch;

	ent->flags ^= FL_LOCKED;

	VectorCopy(ent->r.maxs, ent->s.apos.trBase);
	VectorCopy(ent->r.mins, ent->s.pos.trBase);

	trap_LinkEntity(ent);

	level.numBrushEnts++;
}

/*QUAKED trigger_radiation (0.5 0.5 0.5) ? START_OFF MAP_WIDE
This can be used in three ways:
- as radiation volume trigger
- as mapwide radiation

START_OFF	ent is off at spawn
MAP_WIDE	mapwide radiation

The damage the radiation does is calculated from these two values:
"dmg"			damage(default 1)
"wait"			wait(seconds, default 10)

Forumla is: dps = dmg / wait
*/
/**
 * \brief Touch function of trigger_radiation.
 *
 * Touch function of trigger_radiation.
 *
 * @param ent the trigger
 * @param other the touching entity
 * @param trace a trace
 */
void trigger_radiation_touch(gentity_t *ent, gentity_t *other, trace_t *trace) {
	if(!other || !other->client || !ent->count) return;

	if(!(ent->flags & FL_LOCKED)) {
		if(other->health - ent->damage < 0)
			other->health = 0;
		else
			other->health -= ent->damage;

		other->client->ps.stats[STAT_HEALTH] = other->health;

		ent->flags ^= FL_LOCKED;
	}

	/* TODO: display radiation symbol? */
}

/**
 * \brief Think function of trigger_radiation.
 *
 * Think function of trigger_radiation.
 *
 * @param ent the trigger
 */
void trigger_radiation_think(gentity_t *ent) {
	if(ent->flags & FL_LOCKED)
		ent->flags ^= FL_LOCKED;
}

/**
 * \brief Use function of trigger_radiation.
 *
 * Use function of trigger_radiation.
 *
 * @param ent the trigger
 * @param other another entity
 * @param activator the activator
 */
void trigger_radiation_use(gentity_t *ent, gentity_t *other, gentity_t *activator) {
	ent->count = !ent->count;
}

/**
 * \brief Spawn function of trigger_radiation.
 *
 * Spawn function of trigger_radiation.
 *
 * @param ent the trigger
 */
void SP_trigger_radiation(gentity_t *ent) {
	
	if(!ent->damage)
		ent->damage = 1;
	if(!ent->wait)
		ent->wait = 10000;
	else
		ent->wait *= 1000;

	InitTrigger(ent);

	ent->count = !(ent->spawnflags & 1);

	if(!(ent->spawnflags & 2))
		ent->touch = trigger_radiation_touch;
	
	ent->think = trigger_radiation_think;
	ent->nextthink = level.time + ent->wait;
	ent->flags ^= FL_LOCKED;
	ent->use = trigger_radiation_use;

	VectorCopy(ent->r.maxs, ent->s.apos.trBase);
	VectorCopy(ent->r.mins, ent->s.pos.trBase);

	trap_LinkEntity(ent);

	if(!ent->tmpEntity)
		level.numBrushEnts++;
}

