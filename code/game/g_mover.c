// Copyright (C) 1999-2000 Id Software, Inc.
//

#include "g_local.h"

extern qboolean G_CallSpawn(gentity_t *ent);


/*
===============================================================================

PUSHMOVE

===============================================================================
*/

static void MatchTeam( gentity_t *teamLeader, int moverState, int time );

typedef struct {
	gentity_t	*ent;
	vec3_t	origin;
	vec3_t	angles;
	float	deltayaw;
} pushed_t;
pushed_t	pushed[MAX_GENTITIES], *pushed_p;


/*
============
G_TestEntityPosition

============
*/
gentity_t	*G_TestEntityPosition( gentity_t *ent ) {
	trace_t	tr;
	int		mask;

	if ( ent->clipmask ) {
		mask = ent->clipmask;
	} else {
		mask = MASK_SOLID;
	}
	if ( ent->client ) {
		trap_Trace( &tr, ent->client->ps.origin, ent->r.mins, ent->r.maxs, ent->client->ps.origin, ent->s.number, mask );
	} else {
		trap_Trace( &tr, ent->s.pos.trBase, ent->r.mins, ent->r.maxs, ent->s.pos.trBase, ent->s.number, mask );
	}
	
	if (tr.startsolid)
		return &g_entities[ tr.entityNum ];
		
	return NULL;
}


/*
==================
G_TryPushingEntity

Returns qfalse if the move is blocked
==================
*/
static qboolean	G_TryPushingEntity( gentity_t *check, gentity_t *pusher, vec3_t move, vec3_t amove ) {
	vec3_t		forward, right, up;
	vec3_t		org, org2, move2;
	gentity_t	*block;
	playerState_t *ps = &check->client->ps;

	// EF_MOVER_STOP will just stop when contacting another entity
	// instead of pushing it, but entities can still ride on top of it
	if ( ( pusher->s.eFlags & EF_MOVER_STOP ) && 
		check->s.groundEntityNum != pusher->s.number ) {
		return qfalse;
	}

	// save off the old position
	if (pushed_p > &pushed[MAX_GENTITIES]) {
		G_Error( "pushed_p > &pushed[MAX_GENTITIES]" );
	}
	pushed_p->ent = check;
	VectorCopy (check->s.pos.trBase, pushed_p->origin);
	VectorCopy (check->s.apos.trBase, pushed_p->angles);
	if ( check->client ) {
		pushed_p->deltayaw = ps->delta_angles[YAW];
		VectorCopy (ps->origin, pushed_p->origin);
	}
	pushed_p++;

	// we need this for pushing things later
	VectorSubtract (vec3_origin, amove, org);
	AngleVectors (org, forward, right, up);

	// try moving the contacted entity 
	VectorAdd (check->s.pos.trBase, move, check->s.pos.trBase);
	if (check->client) {
		// make sure the client's view rotates when on a rotating mover
		ps->delta_angles[YAW] += ANGLE2SHORT(amove[YAW]);
	}

	// figure movement due to the pusher's amove
	if ( check->client ) {
		VectorSubtract (ps->origin, pusher->r.currentOrigin, org);
	}
	else {
		VectorSubtract (check->s.pos.trBase, pusher->r.currentOrigin, org);
	}
	org2[0] = DotProduct (org, forward);
	org2[1] = -DotProduct (org, right);
	org2[2] = DotProduct (org, up);
	VectorSubtract (org2, org, move2);
	VectorAdd (check->s.pos.trBase, move2, check->s.pos.trBase);
	if ( check->client ) {
		VectorAdd (ps->origin, move, ps->origin);
		VectorAdd (ps->origin, move2, ps->origin);
	}

	// may have pushed them off an edge
	if ( check->s.groundEntityNum != pusher->s.number ) {
		check->s.groundEntityNum = -1;
	}

	block = G_TestEntityPosition( check );
	if (!block) {
		// pushed ok
		if ( check->client ) {
			VectorCopy( ps->origin, check->r.currentOrigin );
		} else {
			VectorCopy( check->s.pos.trBase, check->r.currentOrigin );
		}
		trap_LinkEntity (check);
		return qtrue;
	}

	// if it is ok to leave in the old position, do it
	// this is only relevent for riding entities, not pushed
	// Sliding trapdoors can cause this.
	VectorCopy( (pushed_p-1)->origin, check->s.pos.trBase);
	if ( check->client ) {
		VectorCopy( (pushed_p-1)->origin, ps->origin);
	}
	VectorCopy( (pushed_p-1)->angles, check->s.apos.trBase );
	block = G_TestEntityPosition (check);
	if ( !block ) {
		check->s.groundEntityNum = -1;
		pushed_p--;
		return qtrue;
	}

	//if(check->

	// blocked
	return qfalse;
}


/*
============
G_MoverPush

Objects need to be moved back on a failed push,
otherwise riders would continue to slide.
If qfalse is returned, *obstacle will be the blocking entity
============
*/
static qboolean G_MoverPush( gentity_t *pusher, vec3_t move, vec3_t amove, gentity_t **obstacle ) {
	int			i, e;
	gentity_t	*check;
	vec3_t		mins, maxs;
	pushed_t	*p;
	int			entityList[MAX_GENTITIES];
	int			listedEntities;
	vec3_t		totalMins, totalMaxs;
	qboolean	notMoving;

	*obstacle = NULL;


	// mins/maxs are the bounds at the destination
	// totalMins / totalMaxs are the bounds for the entire move
	if ( pusher->r.currentAngles[0] || pusher->r.currentAngles[1] || pusher->r.currentAngles[2]
		|| amove[0] || amove[1] || amove[2] ) {
		float		radius;

		radius = RadiusFromBounds( pusher->r.mins, pusher->r.maxs );
		for ( i = 0 ; i < 3 ; i++ ) {
			mins[i] = pusher->r.currentOrigin[i] + move[i] - radius;
			maxs[i] = pusher->r.currentOrigin[i] + move[i] + radius;
			totalMins[i] = mins[i] - move[i];
			totalMaxs[i] = maxs[i] - move[i];
		}
	} else {
		for (i=0 ; i<3 ; i++) {
			mins[i] = pusher->r.absmin[i] + move[i];
			maxs[i] = pusher->r.absmax[i] + move[i];
		}

		VectorCopy( pusher->r.absmin, totalMins );
		VectorCopy( pusher->r.absmax, totalMaxs );
		for (i=0 ; i<3 ; i++) {
			if ( move[i] > 0 ) {
				totalMaxs[i] += move[i];
			} else {
				totalMins[i] += move[i];
			}
		}
	}

	// unlink the pusher so we don't get it in the entityList
	trap_UnlinkEntity( pusher );

	listedEntities = trap_EntitiesInBox( totalMins, totalMaxs, entityList, MAX_GENTITIES );

	// move the pusher to it's final position
	VectorAdd( pusher->r.currentOrigin, move, pusher->r.currentOrigin );
	VectorAdd( pusher->r.currentAngles, amove, pusher->r.currentAngles );
	trap_LinkEntity( pusher );

	notMoving = (qboolean)(VectorCompare( vec3_origin, move )&&VectorCompare( vec3_origin, amove ));
	// see if any solid entities are inside the final position
	for ( e = 0 ; e < listedEntities ; e++ ) {
		check = &g_entities[ entityList[ e ] ];

		if ( notMoving && check->s.eFlags&EF_MISSILE_STICK )
		{//special case hack for sticky things
			//destroy it
			G_Damage( check, pusher, pusher, NULL, NULL, 99999, 0, MOD_CRUSH );
			continue;
		}

		// only push items and players
		if ( check->s.eType != ET_ITEM && check->s.eType != ET_PLAYER && !check->physicsObject ) {
			continue;
		}

		// if the entity is standing on the pusher, it will definitely be moved
		if ( check->s.groundEntityNum != pusher->s.number ) {
			// see if the ent needs to be tested
			if ( check->r.absmin[0] >= maxs[0]
			|| check->r.absmin[1] >= maxs[1]
			|| check->r.absmin[2] >= maxs[2]
			|| check->r.absmax[0] <= mins[0]
			|| check->r.absmax[1] <= mins[1]
			|| check->r.absmax[2] <= mins[2] ) {
				continue;
			}
			// see if the ent's bbox is inside the pusher's final position
			// this does allow a fast moving object to pass through a thin entity...
			if (!G_TestEntityPosition (check)) {
				continue;
			}
		}

		// the entity needs to be pushed
		if ( G_TryPushingEntity( check, pusher, move, amove ) ) {
			continue;
		}

		// the move was blocked an entity

		// bobbing entities are instant-kill and never get blocked
		if ( pusher->s.pos.trType == TR_SINE || pusher->s.apos.trType == TR_SINE ) {
			G_Damage( check, pusher, pusher, NULL, NULL, 99999, 0, MOD_CRUSH );
			continue;
		}

		
		// save off the obstacle so we can call the block function (crush, etc)
		*obstacle = check;

		// move back any entities we already moved
		// go backwards, so if the same entity was pushed
		// twice, it goes back to the original position
		for ( p=pushed_p-1 ; p>=pushed ; p-- ) {
			VectorCopy (p->origin, p->ent->s.pos.trBase);
			VectorCopy (p->angles, p->ent->s.apos.trBase);
			if ( p->ent->client ) {
				p->ent->client->ps.delta_angles[YAW] = p->deltayaw;
				VectorCopy (p->origin, p->ent->client->ps.origin);
			}
			trap_LinkEntity (p->ent);
		}
		return qfalse;
	}

	return qtrue;
}


/*
=================
G_MoverTeam
=================
*/
static void G_MoverTeam( gentity_t *ent ) {
	vec3_t		move, amove;
	gentity_t	*part, *obstacle;
	vec3_t		origin, angles;

	obstacle = NULL;

	// make sure all team slaves can move before commiting
	// any moves or calling any think functions
	// if the move is blocked, all moved objects will be backed out
	pushed_p = pushed;
	for (part = ent ; part ; part=part->teamchain) {
		// get current position
		BG_EvaluateTrajectory( &part->s.pos, level.time, origin );
		BG_EvaluateTrajectory( &part->s.apos, level.time, angles );
		VectorSubtract( origin, part->r.currentOrigin, move );
		VectorSubtract( angles, part->r.currentAngles, amove );
		if ( !G_MoverPush( part, move, amove, &obstacle ) ) {
			break;	// move was blocked
		}
	}

	if (part) {
		// go back to the previous position
		for ( part = ent ; part ; part = part->teamchain ) {
			part->s.pos.trTime += level.time - level.previousTime;
			part->s.apos.trTime += level.time - level.previousTime;
			BG_EvaluateTrajectory( &part->s.pos, level.time, part->r.currentOrigin );
			BG_EvaluateTrajectory( &part->s.apos, level.time, part->r.currentAngles );
			trap_LinkEntity( part );
		}

		// if the pusher has a "blocked" function, call it
		if (ent->blocked) {
			ent->blocked( ent, obstacle );
		}
		return;
	}

	// the move succeeded
	for ( part = ent ; part ; part = part->teamchain ) {
		// call the reached function if time is at or past end point
		if ( part->s.pos.trType == TR_LINEAR_STOP ) {
			if ( level.time >= part->s.pos.trTime + part->s.pos.trDuration ) {
				if ( part->reached ) {
					part->reached( part );
					#ifdef G_LUA
					if(part->luaReached)
						LuaHook_G_EntityReached(part->luaReached, part->s.number);
					#endif
				}
			}
		}
		if ( part->s.apos.trType == TR_LINEAR_STOP ) {
			if ( level.time >= part->s.apos.trTime + part->s.apos.trDuration ) {
				if ( part->reached ) {
					part->reached( part );
					#ifdef G_LUA
					if(part->luaReachedAngular)
						LuaHook_G_EntityReachedAngular(part->luaReachedAngular, part->s.number);
					#endif
				}
			}
		}
	}
}

/*
================
G_Mover_Run

================
*/
void G_Mover_Run( gentity_t *ent ) {
	// if not a team captain, don't do anything, because
	// the captain will handle everything
	if ( ent->flags & FL_TEAMSLAVE ) {
		return;
	}

	// if stationary at one of the positions, don't move anything
	if ( ent->s.pos.trType != TR_STATIONARY || ent->s.apos.trType != TR_STATIONARY ) {
		G_MoverTeam( ent );
	}

	// check think function
	G_RunThink( ent );
}

/*
============================================================================

GENERAL MOVERS

Doors, plats, and buttons are all binary (two position) movers
Pos1 is "at rest", pos2 is "activated"
============================================================================
*/

/*
===============
SetMoverState
===============
*/
void SetMoverState( gentity_t *ent, moverState_t moverState, int time ) {
	vec3_t			delta;
	float			f;
	gentity_t		*touched = NULL;

	if(ent->touched)
		touched = ent->touched;

	ent->moverState = moverState;

	ent->s.pos.trTime = time;
	ent->s.apos.trTime = time;
	switch( moverState ) {
	case MOVER_POS1:
		VectorCopy( ent->pos1, ent->s.pos.trBase );
		ent->s.pos.trType = TR_STATIONARY;
		if(touched && touched->angle) {
			VectorCopy(ent->apos1, ent->s.apos.trBase);
			ent->s.apos.trType = TR_STATIONARY;
		}
		break;
	case MOVER_POS2:
		VectorCopy( ent->pos2, ent->s.pos.trBase );
		ent->s.pos.trType = TR_STATIONARY;
		if(touched && touched->angle) {
			VectorCopy(ent->apos2, ent->s.apos.trBase);
			ent->s.apos.trType = TR_STATIONARY;
		}
		break;
	case MOVER_1TO2:
		VectorCopy( ent->pos1, ent->s.pos.trBase );
		VectorSubtract( ent->pos2, ent->pos1, delta );
		f = 1000.0 / ent->s.pos.trDuration;
		VectorScale( delta, f, ent->s.pos.trDelta );
		ent->s.pos.trType = TR_LINEAR_STOP;
		if(touched && touched->angle) {
			VectorCopy( ent->apos2, ent->s.apos.trBase );
			VectorSubtract( ent->apos2, ent->apos1, delta );
			f = 1000.0 / ent->s.apos.trDuration;
			VectorScale( delta, f, ent->s.apos.trDelta );
			ent->s.apos.trType = TR_LINEAR_STOP;
		}
		break;
	case MOVER_2TO1:
		VectorCopy( ent->pos2, ent->s.pos.trBase );
		VectorSubtract( ent->pos1, ent->pos2, delta );
		f = 1000.0 / ent->s.pos.trDuration;
		VectorScale( delta, f, ent->s.pos.trDelta );
		ent->s.pos.trType = TR_LINEAR_STOP;
		if(touched && touched->angle) {
			VectorCopy(ent->apos2, ent->s.apos.trBase );
			VectorSubtract( ent->apos1, ent->apos2, delta );
			f = 1000.0 / ent->s.apos.trDuration;
			VectorScale(delta, f, ent->s.apos.trDelta);
			ent->s.apos.trType = TR_LINEAR_STOP;
		}
		break;
	case ROTATOR_POS1:
		VectorCopy( ent->pos1, ent->s.apos.trBase );
		ent->s.apos.trType = TR_STATIONARY;
		break;
	case ROTATOR_POS2:
		VectorCopy( ent->pos2, ent->s.apos.trBase );
		ent->s.apos.trType = TR_STATIONARY;
		break;
	case ROTATOR_1TO2:
		VectorCopy( ent->pos1, ent->s.apos.trBase );
		VectorSubtract( ent->pos2, ent->pos1, delta );
		f = 1000.0 / ent->s.apos.trDuration;
		VectorScale( delta, f, ent->s.apos.trDelta );
		ent->s.apos.trType = TR_LINEAR_STOP;
		break;
	case ROTATOR_2TO1:
		VectorCopy( ent->pos2, ent->s.apos.trBase );
		VectorSubtract( ent->pos1, ent->pos2, delta );
		f = 1000.0 / ent->s.apos.trDuration;
		VectorScale( delta, f, ent->s.apos.trDelta );
		ent->s.apos.trType = TR_LINEAR_STOP;
		break;
	#ifdef G_LUA
	case MOVER_LUA:
		break;
	#endif
	default: // to make gcc happy
		break;
	}
	BG_EvaluateTrajectory( &ent->s.pos, level.time, ent->r.currentOrigin );
	BG_EvaluateTrajectory( &ent->s.apos, level.time, ent->r.currentAngles );
	trap_LinkEntity( ent );
}

/*
================
MatchTeam

All entities in a mover team will move from pos1 to pos2
in the same amount of time
================
*/
void MatchTeam( gentity_t *teamLeader, int moverState, int time ) {
	gentity_t		*slave = NULL;

	for ( slave = teamLeader ; slave ; slave = slave->teamchain ) {
		SetMoverState( slave, (moverState_t)moverState, time );
	}
}



/*
================
ReturnToPos1
================
*/
void ReturnToPos1( gentity_t *ent ) 
{
	//if it's a crushing door, make sure there are no ents in the way
	if ( !Q_stricmp( ent->classname, "func_door" ) && ent->targetname && !(ent->spawnflags&32) && ent->wait > 0 ) //OVERRIDE
	{
		gentity_t *t;
		trace_t tr;
		vec3_t	mins, maxs;

		//FIX: make sure it isn't a turbolift door either
		//A turbolift door should only be targetted by its turbolift parent
		t = G_Find(NULL, FOFS(target), ent->targetname);
		if ( t && Q_stricmp( t->classname, "target_turbolift" ) )
		{
			VectorCopy( ent->r.mins, mins );
			VectorCopy( ent->r.maxs, maxs );

			//expand out from the door
			mins[0] -= 40;
			mins[1] -= 40;

			maxs[0] += 40;
			maxs[1] += 40;

			trap_Trace( &tr, ent->s.origin, mins, maxs, ent->s.origin, ent-g_entities, CONTENTS_BODY );

			if ( tr.fraction != 1.0f )
			{
				ent->nextthink = level.time + ent->wait;
				return;
			}
		}
	}

	MatchTeam( ent, MOVER_2TO1, level.time );

	// looping sound
	ent->s.loopSound = ent->soundLoop;

	// starting sound
	if ( ent->sound2to1 ) {
		G_AddEvent( ent, EV_GENERAL_SOUND, ent->sound2to1 );
	}
}

/*
===============
ReturnToPos1_Use
TiM: To make toggle doors
===============
*/
void ReturnToPos1_Use( gentity_t* ent, gentity_t* other, gentity_t* activator)
{
	//if it's a crushing door, make sure there are no ents in the way
	if ( !Q_stricmp( ent->classname, "func_door" ) && ent->targetname && !(ent->spawnflags&32) && ent->wait > 0 ) //OVERRIDE
	{
		gentity_t *t;
		trace_t tr;
		vec3_t	mins, maxs;

		//FIX: make sure it isn't a turbolift door either
		//A turbolift door should only be targetted by its turbolift parent
		t = G_Find(NULL, FOFS(target), ent->targetname);
		if ( t && Q_stricmp( t->classname, "target_turbolift" ) )
		{
			VectorCopy( ent->r.mins, mins );
			VectorCopy( ent->r.maxs, maxs );

			//expand out from the door
			mins[0] -= 40;
			mins[1] -= 40;

			maxs[0] += 40;
			maxs[1] += 40;

			trap_Trace( &tr, ent->s.origin, mins, maxs, ent->s.origin, ent-g_entities, CONTENTS_BODY );

			if ( tr.fraction != 1.0f )
			{
				ent->nextthink = level.time + ent->wait;
				return;
			}
		}
	}

	MatchTeam( ent, MOVER_2TO1, level.time );

	// looping sound
	ent->s.loopSound = ent->soundLoop;

	// starting sound
	if ( ent->sound2to1 ) {
		G_AddEvent( ent, EV_GENERAL_SOUND, ent->sound2to1 );
	}
}

/*
================
ReturnToApos1
================
*/
void ReturnToApos1( gentity_t *ent ) {

	MatchTeam( ent, ROTATOR_2TO1, level.time );

	// looping sound
	ent->s.loopSound = ent->soundLoop;

	// starting sound
	if ( ent->sound2to1 ) {
		G_AddEvent( ent, EV_GENERAL_SOUND, ent->sound2to1 );
	}
}

/*
===============
ReturnToApos1_Use
TiM: To make toggle doors
===============
*/
void ReturnToApos1_Use( gentity_t* ent, gentity_t* other, gentity_t* activator)
{
	MatchTeam( ent, ROTATOR_2TO1, level.time );

	// looping sound
	ent->s.loopSound = ent->soundLoop;

	// starting sound
	if ( ent->sound2to1 ) {
		G_AddEvent( ent, EV_GENERAL_SOUND, ent->sound2to1 );
	}
}


/*
================
Reached_BinaryMover
================
*/
void Reached_BinaryMover( gentity_t *ent ) {

	// stop the looping sound
	ent->s.loopSound = ent->soundLoop;

	#ifdef G_LUA
	if(ent->luaTrigger)
	{
		if(ent->activator)
		{
			LuaHook_G_EntityTrigger(ent->luaTrigger, ent->s.number, ent->activator->s.number);
		}
		else
		{
			LuaHook_G_EntityTrigger(ent->luaTrigger, ent->s.number, ENTITYNUM_WORLD);
		}
	}
	#endif

	if ( ent->moverState == MOVER_1TO2 ) {
		// reached pos2
		SetMoverState( ent, MOVER_POS2, level.time );

		// play sound
		if ( ent->soundPos2 ) {
			G_AddEvent( ent, EV_GENERAL_SOUND, ent->soundPos2 );
		}

		if ( ent->wait < 0 )
		{//never go back
			ent->think = 0;
			ent->nextthink = -1;
			ent->use = ReturnToPos1_Use; //0 //TiM - allow toggle doors
		}
		else
		{
			// return to pos1 after a delay
			ent->think = ReturnToPos1;
			ent->nextthink = level.time + ent->wait;
		}

		// fire targets
		if ( !ent->activator ) {
			ent->activator = ent;
		}
		G_UseTargets( ent, ent->activator );
	} else if ( ent->moverState == MOVER_2TO1 ) {
		// reached pos1
		SetMoverState( ent, MOVER_POS1, level.time );

		// play sound
		if ( ent->soundPos1 ) {
			G_AddEvent( ent, EV_GENERAL_SOUND, ent->soundPos1 );
		}

		if ( ent->wait < 0 )
			ent->use = G_Mover_UseBinaryMover;

		// close areaportals
		if ( ent->teammaster == ent || !ent->teammaster ) {
			trap_AdjustAreaPortalState( ent, qfalse );
		}
	} else if ( ent->moverState == ROTATOR_1TO2 ) {
		// reached pos2
		SetMoverState( ent, ROTATOR_POS2, level.time );

		// play sound
		if ( ent->soundPos2 ) {
			G_AddEvent( ent, EV_GENERAL_SOUND, ent->soundPos2 );
		}


		if(ent->wait < 0) {
			ent->think = 0;
			ent->nextthink  = -1;
			ent->use = ReturnToApos1_Use;
		}
		else {
			// return to apos1 after a delay
			ent->think = ReturnToApos1;
			ent->nextthink = level.time + ent->wait;
		}

		// fire targets
		if ( !ent->activator ) {
			ent->activator = ent;
		}
		G_UseTargets( ent, ent->activator );
	} else if ( ent->moverState == ROTATOR_2TO1 ) {
		// reached pos1
		SetMoverState( ent, ROTATOR_POS1, level.time );

		// play sound
		if ( ent->soundPos1 ) {
			G_AddEvent( ent, EV_GENERAL_SOUND, ent->soundPos1 );
		}

		if(ent->wait < 0)
			ent->use = G_Mover_UseBinaryMover;

		// close areaportals
		if ( ent->teammaster == ent || !ent->teammaster ) {
			trap_AdjustAreaPortalState( ent, qfalse );
		}
	#ifdef G_LUA
	} else if (ent->moverState == MOVER_LUA) {
		// lua all is fine
		if(level.time >= ent->s.pos.trTime + ent->s.pos.trDuration) {
			BG_EvaluateTrajectory(&ent->s.pos, level.time, ent->r.currentOrigin);
			VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
			ent->s.pos.trType = TR_STATIONARY;
			ent->s.pos.trTime = level.time;
			trap_LinkEntity(ent);
		}
		if(level.time >= ent->s.apos.trTime + ent->s.apos.trDuration) {
			BG_EvaluateTrajectory(&ent->s.apos, level.time, ent->r.currentAngles);
			VectorCopy(ent->r.currentAngles, ent->s.apos.trBase);
			ent->s.apos.trType = TR_STATIONARY;
			ent->s.apos.trTime = level.time;
			trap_LinkEntity(ent);
		}
	#endif
	} else {
		G_Error( "Reached_BinaryMover: bad moverState" );
	}
}


/*
================
G_Mover_UseBinaryMover
================
*/
void G_Mover_UseBinaryMover( gentity_t *ent, gentity_t *other, gentity_t *activator ) {
	int		total;
	int		partial;

	//GSIO01 -> is this a train and is called by the swapname
	if(activator && activator->target && ent->swapname) {
		if(!Q_stricmp(ent->classname, "func_train") && !Q_stricmp(activator->target, ent->swapname)) {
			if(ent->count == 1) {
				ent->s.solid = 0;
				ent->r.contents = 0;
				ent->clipmask = 0;
				ent->r.svFlags |= SVF_NOCLIENT;
				ent->s.eFlags |= EF_NODRAW;
				ent->count = 0;
			} else {
				ent->clipmask = CONTENTS_BODY;
				trap_SetBrushModel( ent, ent->model );
				ent->r.svFlags &= ~SVF_NOCLIENT;
				ent->s.eFlags &= ~EF_NODRAW;
				ent->clipmask = 0;
				ent->count = 1;
			}
			return;
		}
	}

	//TiM -> Do not engage if clamped
	if ( ent->flags & FL_CLAMPED )
		return;

	//GSIO01 | 08/05/2009: do not engage if door is locked
	if(ent->flags & FL_LOCKED) {
		G_AddEvent(ent, EV_GENERAL_SOUND, ent->n00bCount /*ent->soundLocked*/);
		return;
	}

	//GSIO01 | 09/05/2009: do engage if door is admin only and player isn admin
	if((!Q_stricmp(ent->classname, "func_door") && (ent->spawnflags & 128)) 
		|| (!Q_stricmp(ent->classname, "func_door_rotating") && (ent->spawnflags & 64))) {
		if(activator && !IsAdmin(activator))
			return;
	}

	// only the master should be used
	if ( ent->flags & FL_TEAMSLAVE ) {
		G_Mover_UseBinaryMover( ent->teammaster, other, activator );
		return;
	}

	if(activator)
		ent->activator = activator;

	#ifdef G_LUA
	if(ent->luaTrigger)
	{
		if(activator)
			LuaHook_G_EntityTrigger(ent->luaTrigger, ent->s.number, activator->s.number);
		else
			LuaHook_G_EntityTrigger(ent->luaTrigger, ent->s.number, -1);
	}
	#endif

	if ( ent->moverState == MOVER_POS1 ) {
		// start moving 50 msec later, becase if this was player
		// triggered, level.time hasn't been advanced yet
		MatchTeam( ent, MOVER_1TO2, level.time + 50 );

		// starting sound
		if ( ent->sound1to2 ) {
			G_AddEvent( ent, EV_GENERAL_SOUND, ent->sound1to2 );
		}

		// looping sound
		ent->s.loopSound = ent->soundLoop;

		// open areaportal
		if ( !ent->teammaster || ent->teammaster == ent ) {
			trap_AdjustAreaPortalState( ent, qtrue );
		}
		return;
	}

	// if all the way up, just delay before coming down
	if ( ent->moverState == MOVER_POS2 ) {
		if ( ent->wait >= 0 ) {
			ent->nextthink = level.time + ent->wait;
		}
		return;
	}

	// only partway down before reversing
	if ( ent->moverState == MOVER_2TO1 ) {
		total = ent->s.pos.trDuration;
		partial = level.time - ent->s.time;
		if ( partial > total ) {
			partial = total;
		}

		MatchTeam( ent, MOVER_1TO2, level.time - ( total - partial ) );

		if ( ent->sound1to2 ) {
			G_AddEvent( ent, EV_GENERAL_SOUND, ent->sound1to2 );
		}
		return;
	}

	// only partway up before reversing
	if ( ent->moverState == MOVER_1TO2 ) {
		total = ent->s.pos.trDuration;
		partial = level.time - ent->s.time;
		if ( partial > total ) {
			partial = total;
		}

		MatchTeam( ent, MOVER_2TO1, level.time - ( total - partial ) );

		if ( ent->sound2to1 ) {
			G_AddEvent( ent, EV_GENERAL_SOUND, ent->sound2to1 );
		}
		return;
	}

	if ( ent->moverState == ROTATOR_POS1 ) {
		// start moving 50 msec later, becase if this was player
		// triggered, level.time hasn't been advanced yet
		MatchTeam( ent, ROTATOR_1TO2, level.time + 50 );

		// starting sound
		if ( ent->sound1to2 ) {
			G_AddEvent( ent, EV_GENERAL_SOUND, ent->sound1to2 );
		}

		// looping sound
		ent->s.loopSound = ent->soundLoop;

		// open areaportal
		if ( !ent->teammaster || ent->teammaster == ent ) {
			trap_AdjustAreaPortalState( ent, qtrue );
		}
		return;
	}

	// if all the way up, just delay before coming down
	if ( ent->moverState == ROTATOR_POS2 ) {
		ent->nextthink = level.time + ent->wait;
		return;
	}

	// only partway down before reversing
	if ( ent->moverState == ROTATOR_2TO1 ) {
		total = ent->s.apos.trDuration;
		partial = level.time - ent->s.time;
		if ( partial > total ) {
			partial = total;
		}

		MatchTeam( ent, ROTATOR_1TO2, level.time - ( total - partial ) );

		if ( ent->sound1to2 ) {
			G_AddEvent( ent, EV_GENERAL_SOUND, ent->sound1to2 );
		}
		return;
	}

	// only partway up before reversing
	if ( ent->moverState == ROTATOR_1TO2 ) {
		total = ent->s.apos.trDuration;
		partial = level.time - ent->s.time;
		if ( partial > total ) {
			partial = total;
		}

		MatchTeam( ent, ROTATOR_2TO1, level.time - ( total - partial ) );

		if ( ent->sound2to1 ) {
			G_AddEvent( ent, EV_GENERAL_SOUND, ent->sound2to1 );
		}
		return;
	}
}


void Reached_AdvancedMover(gentity_t *ent);

/*
================
InitMover

"pos1", "pos2", and "speed" should be set before calling,
so the movement delta can be calculated
================
*/
void InitMover( gentity_t *ent ) {
	vec3_t		move;
	float		distance;
	float		light;
	vec3_t		color;
	qboolean	lightSet, colorSet;
	char		*sound;

	// if the "model2" key is set, use a seperate model
	// for drawing, but clip against the brushes
	if ( ent->model2 ) {
		ent->s.modelindex2 = G_ModelIndex( ent->model2 );
	}

	// if the "loopsound" key is set, use a constant looping sound when moving
	if ( G_SpawnString( "noise", "100", &sound ) ) {
		ent->s.loopSound = G_SoundIndex( sound );
	}

	// if the "color" or "light" keys are set, setup constantLight
	lightSet = G_SpawnFloat( "light", "100", &light );
	colorSet = G_SpawnVector( "color", "1 1 1", color );
	if ( lightSet || colorSet ) {
		int		r, g, b, i;

		r = color[0] * 255;
		if ( r > 255 ) {
			r = 255;
		}
		g = color[1] * 255;
		if ( g > 255 ) {
			g = 255;
		}
		b = color[2] * 255;
		if ( b > 255 ) {
			b = 255;
		}
		i = light / 4;
		if ( i > 255 ) {
			i = 255;
		}
		ent->s.constantLight = r | ( g << 8 ) | ( b << 16 ) | ( i << 24 );
	}


	ent->use = G_Mover_UseBinaryMover;
	if(Q_stricmp(ent->classname, "func_mover"))
		ent->reached = Reached_BinaryMover;
	else
		ent->reached = Reached_AdvancedMover;

	// if this is a func_mover we have to make sure it is a bit away from it's first target
	if(!Q_stricmp(ent->classname, "func_mover")) {
		VectorSubtract(ent->pos1, ent->pos2, move);
		distance = VectorLength(move);
		if(distance < 32) {
			ent->pos1[2] += 32;
			VectorCopy(ent->pos1, ent->s.origin);
		}
		VectorClear(move);
		distance = 0;
	}

	ent->moverState = MOVER_POS1;
	ent->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	ent->s.eType = ET_MOVER;
	VectorCopy (ent->pos1, ent->r.currentOrigin);
	// we also need support for angular movement (func_mover)
	VectorCopy (ent->apos1, ent->r.currentAngles);
	trap_LinkEntity (ent);

	ent->s.pos.trType = TR_STATIONARY;
	VectorCopy( ent->pos1, ent->s.pos.trBase );

	// we also need support for angular movement (func_mover)
	ent->s.pos.trType = TR_STATIONARY;
	VectorCopy( ent->apos1, ent->s.pos.trBase );

	// calculate time to reach second position from speed
	VectorSubtract( ent->pos2, ent->pos1, move );
	distance = VectorLength( move );
	if ( ! ent->speed ) {
		ent->speed = 100;
	}
	VectorScale( move, ent->speed, ent->s.pos.trDelta );
	ent->s.pos.trDuration = distance * 1000 / ent->speed;
	if ( ent->s.pos.trDuration <= 0 ) {
		ent->s.pos.trDuration = 1;
	}

	// we also need support for angular movement (func_mover)
	ent->s.apos.trDuration = distance * 1000 / ent->speed; // rotating should take as long as moving
	if( ent->s.apos.trDuration <= 0 ) {
		ent->s.apos.trDuration = 1;
	}

	if(!Q_stricmp(ent->classname, "func_rotating")) {
		ent->reached = 0;
	}
}

/*
================
InitRotator

"pos1", "pos2", and "speed" should be set before calling,
so the movement delta can be calculated
================
*/
void InitRotator( gentity_t *ent ) {
	vec3_t		move;
	float		angle;
	float		light;
	vec3_t		color;
	qboolean	lightSet, colorSet;
	char		*sound;

	// if the "model2" key is set, use a seperate model
	// for drawing, but clip against the brushes
	if ( ent->model2 ) {
		ent->s.modelindex2 = G_ModelIndex( ent->model2 );
	}

	// if the "loopsound" key is set, use a constant looping sound when moving
	if ( G_SpawnString( "noise", "100", &sound ) ) {
		ent->s.loopSound = G_SoundIndex( sound );
	}

	// if the "color" or "light" keys are set, setup constantLight
	lightSet = G_SpawnFloat( "light", "100", &light );
	colorSet = G_SpawnVector( "color", "1 1 1", color );
	if ( lightSet || colorSet ) {
		int		r, g, b, i;

		r = color[0] * 255;
		if ( r > 255 ) {
			r = 255;
		}
		g = color[1] * 255;
		if ( g > 255 ) {
			g = 255;
		}
		b = color[2] * 255;
		if ( b > 255 ) {
			b = 255;
		}
		i = light / 4;
		if ( i > 255 ) {
			i = 255;
		}
		ent->s.constantLight = r | ( g << 8 ) | ( b << 16 ) | ( i << 24 );
	}


	ent->use = G_Mover_UseBinaryMover;
	ent->reached = Reached_BinaryMover;

	ent->moverState = ROTATOR_POS1;
	ent->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	ent->s.eType = ET_MOVER;
	VectorCopy( ent->pos1, ent->r.currentAngles );
	trap_LinkEntity (ent);

	ent->s.apos.trType = TR_STATIONARY;
	VectorCopy( ent->pos1, ent->s.apos.trBase );

	// calculate time to reach second position from speed
	VectorSubtract( ent->pos2, ent->pos1, move );
	angle = VectorLength( move );
	if ( ! ent->speed ) {
		ent->speed = 120;
	}
	VectorScale( move, ent->speed, ent->s.apos.trDelta );
	ent->s.apos.trDuration = angle * 1000 / ent->speed;
	if ( ent->s.apos.trDuration <= 0 ) {
		ent->s.apos.trDuration = 1;
	}
}


/*
===============================================================================

DOOR

A use can be triggered either by a touch function, by being shot, or by being
targeted by another entity.

===============================================================================
*/

/*
================
Blocked_Door
================
*/
void Blocked_Door( gentity_t *ent, gentity_t *other ) {
	// remove anything other than a client
	if ( !other->client ) {
		G_TempEntity( other->s.origin, EV_ITEM_POP );
		G_FreeEntity( other );
		return;
	}

	if ( ent->damage ) {
		G_Damage( other, ent, ent, NULL, NULL, ent->damage, 0, MOD_CRUSH );
	}
	if ( ent->spawnflags & 4 || (!Q_stricmp(ent->classname, "func_door_rotating") && (ent->spawnflags & 2))) { // GSIO01: added support for fucn_door_roating
		return;		// crushers don't reverse
	}

	// reverse direction
	G_Mover_UseBinaryMover( ent, ent, other );
}

/*
================
Touch_DoorTriggerSpectator
================
*/
static void Touch_DoorTriggerSpectator( gentity_t *ent, gentity_t *other, trace_t *trace ) {
	int i, axis;
	vec3_t origin, dir, angles;

	axis = ent->count;
	VectorClear(dir);
	if (fabs(other->s.origin[axis] - ent->r.absmax[axis]) <
		fabs(other->s.origin[axis] - ent->r.absmin[axis])) {
		origin[axis] = ent->r.absmin[axis] - 10;
		dir[axis] = -1;
	}
	else {
		origin[axis] = ent->r.absmax[axis] + 10;
		dir[axis] = 1;
	}
	for (i = 0; i < 3; i++) {
		if (i == axis) continue;
		origin[i] = (ent->r.absmin[i] + ent->r.absmax[i]) * 0.5;
	}
	vectoangles(dir, angles);
	TeleportPlayer(other, origin, angles, TP_NORMAL );
}

void DoorTriggerReactivate(gentity_t *ent);

/*
================
G_Mover_TouchDoorTrigger
================
*/
void G_Mover_TouchDoorTrigger( gentity_t *ent, gentity_t *other, trace_t *trace ) {
#if 0
	vec3_t	vec, doorcenter, movedir;
	float	dot, dist ;

	// fuck this code - it will never work. Let this be a lesson to you from expecting code from single
	// player to work in multiplayer. 
	// should we do a face only door?
	if (ent->parent->spawnflags & 16)
	{
		//Dir from activator to door center
		VectorSubtract(ent->r.absmax , ent->r.absmin , vec );
		VectorMA(ent->r.absmin, 0.5, vec, doorcenter);
		
		VectorSubtract( doorcenter, other->client->ps.origin, vec );
		dist = VectorNormalize ( vec );
		//Activator's facing dir
		VectorCopy( other->client->ps.velocity, movedir );
		VectorNormalize( movedir );
		dot = DotProduct( vec, movedir );
		if ( dot < 0.7 )
		{
			return;
		}
	}
#endif

	//This door's been tempoarily disabled
	if ( ent->parent->flags & FL_CLAMPED )
		return;

	#ifdef G_LUA
	if(ent->luaTrigger)
		LuaHook_G_EntityTrigger(ent->luaTrigger, ent-g_entities, other-g_entities);
	#endif

	if ( other->client && (other->client->sess.sessionTeam == TEAM_SPECTATOR /*|| (other->client->ps.eFlags&EF_ELIMINATED)*/) ) {
		// if the door is not open and not opening
		if ( ent->parent->moverState != MOVER_1TO2 &&
			ent->parent->moverState != MOVER_POS2 &&
			ent->parent->moverState != ROTATOR_1TO2 &&
			ent->parent->moverState != ROTATOR_POS2 ) {
			Touch_DoorTriggerSpectator( ent, other, trace );
		}
	}
	else if ( ent->parent->moverState != MOVER_1TO2 &&
		ent->parent->moverState != ROTATOR_1TO2 ) 
	{
		G_Mover_UseBinaryMover( ent->parent, ent, other );
	}
	if(ent->parent->flags & FL_LOCKED) {
		ent->touch = 0;
		ent->nextthink = level.time + 3000;
	}
}

void DoorTriggerReactivate(gentity_t *ent) {
	ent->touch = G_Mover_TouchDoorTrigger;
	ent->nextthink = -1;
}


/*
======================
Think_SpawnNewDoorTrigger

All of the parts of a door have been spawned, so create
a trigger that encloses all of them
======================
*/
void Think_SpawnNewDoorTrigger( gentity_t *ent ) {
	gentity_t		*other;
	vec3_t		mins, maxs;
	int			i, best;

	if(!ent) return;

	// set all of the slaves as shootable
	for ( other = ent ; other ; other = other->teamchain ) {
		other->takedamage = qtrue;
	}

	// find the bounds of everything on the team
	VectorCopy (ent->r.absmin, mins);
	VectorCopy (ent->r.absmax, maxs);

	for (other = ent->teamchain ; other ; other=other->teamchain) {
		AddPointToBounds (other->r.absmin, mins, maxs);
		AddPointToBounds (other->r.absmax, mins, maxs);
	}

	// Copy maxs and mins to s.origin2 and s.angles2 for scanable door
	VectorCopy(maxs, ent->s.origin2);
	VectorCopy(mins, ent->s.angles2);

	// find the thinnest axis, which will be the one we expand
	best = 0;
	for ( i = 1 ; i < 3 ; i++ ) {
		if ( maxs[i] - mins[i] < maxs[best] - mins[best] ) {
			best = i;
		}
	}

	// should we have a big old trigger volume, or a small one?
	if(((ent->spawnflags & 256) && !Q_stricmp(ent->classname, "func_door")) ||
		((ent->spawnflags & 128) && !Q_stricmp(ent->classname, "func_door_rotating"))) {
		maxs[best] += 12;
		mins[best] -= 12;
	}
	else if (ent->spawnflags & 8 && !Q_stricmp(ent->classname, "func_door"))
	{
		maxs[best] += 48;
		mins[best] -= 48;
	}
	else
	{
		maxs[best] += 100;
		mins[best] -= 100;
	}

	// create a trigger with this size
	other = G_Spawn ();
	VectorCopy (mins, other->r.mins);
	VectorCopy (maxs, other->r.maxs);
	other->parent = ent;
	other->r.contents = CONTENTS_TRIGGER;
	other->touch = G_Mover_TouchDoorTrigger;
	// remember the thinnest axis
	other->count = best;
	//RPG-X | GSIO01 | 08/05/2009 | SOE = START OF EDIT ... lol
	other->think = DoorTriggerReactivate;
	other->nextthink = -1;

	trap_LinkEntity (other);

	MatchTeam( ent, ent->moverState, level.time );

}

void Think_MatchTeam( gentity_t *ent ) {
	gentity_t* lift=NULL;
	gentity_t	*other;
	vec3_t		mins, maxs;

	//TiM - check if this is a turbolift door.  if it is, spawn a trigger
	while ( (lift = G_Find( lift, FOFS(classname), "target_turbolift" ) ) != NULL )
	{
		if ( !Q_stricmp( lift->target, ent->targetname ) ) {
			ent->think = Think_SpawnNewDoorTrigger;
			ent->nextthink = level.time + FRAMETIME;
			return;
		}
	}

	// scanable doors:
	VectorCopy(ent->r.absmax, maxs);
	VectorCopy(ent->r.absmin, mins);

	for(other = ent->teamchain; other; other = other->teamchain) {
		AddPointToBounds(other->r.absmax, mins, maxs);
		AddPointToBounds(other->r.absmin, mins, maxs);
	}

	VectorCopy(maxs, ent->s.origin2);
	VectorCopy(mins, ent->s.angles2);
	

	MatchTeam( ent, ent->moverState, level.time );
}


/*====================
DoorGetMessage

Check whether the door has a messageNum and
if yes set it's message to that

RPG-X | GSIO01 | 13/05/2009
====================*/
void DoorGetMessage(gentity_t *ent) {
	int i, j, messageNum, entityNum;

	if(rpg_scannablePanels.integer) {
		if(level.hasScannableFile) {
			i = 0;
			j = 0;
			messageNum = 0;
			entityNum  = ent-g_entities;
			ent->s.weapon = 0;
			G_SpawnInt("messageNum", "0", &messageNum);
			if(messageNum > 0) {
				while(i < MAX_SCANNABLES) {
					if(level.g_scannables[i] == 0)
						break;
					if(level.g_scannables[i] == messageNum) {
						ent->s.weapon = i + 1;
						break;
					}
					i++;
				}
			}
			if(ent->s.weapon == 0 && level.hasEntScannableFile) {
				i = 0;
				while(i < MAX_SCANNABLES) {
					if(!level.g_entScannables[i][0] && !level.g_entScannables[i][1])
						break;
					if(level.g_entScannables[i][0] == entityNum) {
						j = 0;
						while(j < MAX_SCANNABLES) {
							if(level.g_scannables[j] == 0)
								break;
							if(level.g_scannables[j] == level.g_entScannables[i][1]) {
								ent->s.weapon = j + 1;
								i = MAX_ENTSCANNABLES + 1;
								break;
							}
							j++;
						}
					}
					i++;
				}
			}
			if(ent->s.weapon > 0) {
				ent->s.eType = ET_MOVER_STR;

				VectorCopy(ent->r.absmin, ent->s.origin2);
				VectorCopy(ent->r.absmax, ent->s.angles2);
			}

		}
	}
	trap_LinkEntity(ent);
}


/*QUAKED func_door (0 .5 .8) ? START_OPEN X CRUSHER	TREK_DOOR FACE OVERRIDE LOCKED ADMIN_ONLY CORRIDOR 
-----DESCRIPTION-----
A door that moves between its two positions. Can only translate around one axis at a time.
Can be teamed with multiple doors to trigger simultaniously.

-----SPAWNFLAGS-----
1: START_OPEN : the door to moves to its destination when spawned, and operate in reverse.  It is used to temporarily or permanently close off an area when triggered (not useful for touch or takedamage doors).
2: X : Unknown, do not use.
4: CRUSHER : door will be stuck on it's open position
8: TREK_DOOR : if set this door will have a reduced auto trigger volume
16: FACE : if set, this door requires you to be facing it before the trigger will fire
32: OVERRIDE : if set, targetted doors won't wait until they're clear before closing
64: LOCKED : if set, door is locked at spawn
128: ADMIN_ONLY : if set, door only opens for admins
256: CORRIDOR : if set, door will have en even more reduced auto trigger volume

-----KEYS-----
"model2" - .md3 model to also draw
"angle" - determines the opening direction
"speed" - movement speed (100 default)
"lip" - lip remaining at end of move (8 default)
"dmg" - damage to inflict when blocked (2 default)
"color" - constantLight color
"light" - constantLight radius
"health" - if set, the door must be shot open
"soundstart" - sound to play at start of moving
"soundstop" - sound to play at stop of moving
"soundlocked" - sound to play when locked
"team" - all doors that have the same team will move simultaniously

Ther following keys are only required on one in a team of doors (master door)
"targetname" - if set, no touch field will be spawned and a remote button or trigger field activates the door.
"targetname2" - for target_doorlock.
"wait" - wait before returning (3 default, -1 = never return)

q3map2:
"_clone" - _clonename of entity to clone brushes from. Note: this entity still needs at least one brush which gets replaced.
"_clonename" - see _clone
"_castShadows" OR "_cs" - sets whether the entity casts shadows
"_receiveShadows" OR "_rs" - sets whether the entity receives shadows
*/
*/
void SP_func_door (gentity_t *ent) {
	vec3_t	abs_movedir;
	float	distance;
	vec3_t	size;
	float	lip;
	char	*sound;

	if(!ent->tmpEntity) { // not modified by spawnfile
		G_SpawnString("soundstart", "sound/movers/doors/largedoorstart.wav", &sound);
		ent->sound1to2 = ent->sound2to1 = G_SoundIndex(sound);
		G_SpawnString("soundstop", "sound/movers/doors/largedoorstop.wav", &sound);
		ent->soundPos1 = ent->soundPos2 = G_SoundIndex(sound);
		G_SpawnString("soundlocked", "sound/ambience/voyager/doorchime.mp3", &sound);
		ent->n00bCount = G_SoundIndex(sound);
	}

	ent->blocked = Blocked_Door;

	if(ent->spawnflags & 64)
		ent->flags ^= FL_LOCKED;

	// default speed of 400
	if (!ent->speed)
		ent->speed = 400;

	// default wait of 2 seconds
	if (!ent->wait)
		ent->wait = 2;
	ent->wait *= 1000;

	// default lip of 8 units
	G_SpawnFloat( "lip", "8", &lip );

	// default damage of 2 points
	G_SpawnInt( "dmg", "2", &ent->damage );

	// first position at start
	VectorCopy( ent->s.origin, ent->pos1 );

	// calculate second position
	trap_SetBrushModel( ent, ent->model );
	G_SetMovedir (ent->s.angles, ent->movedir);
	abs_movedir[0] = Q_fabs(ent->movedir[0]);
	abs_movedir[1] = Q_fabs(ent->movedir[1]);
	abs_movedir[2] = Q_fabs(ent->movedir[2]);
	VectorSubtract( ent->r.maxs, ent->r.mins, size );
	if(ent->count) // for SP map support
		distance = DotProduct( abs_movedir, size ) - ent->count;
	else 
		distance = DotProduct( abs_movedir, size ) - lip;
	VectorMA( ent->pos1, distance, ent->movedir, ent->pos2 );

	// if "start_open", reverse position 1 and 2
	if ( ent->spawnflags & 1 ) {
		vec3_t	temp;

		VectorCopy( ent->pos2, temp );
		VectorCopy( ent->s.origin, ent->pos2 );
		VectorCopy( temp, ent->pos1 );
	}

	InitMover( ent );

	ent->nextthink = level.time + FRAMETIME;

	if ( ! (ent->flags & FL_TEAMSLAVE ) ) {
		int health;

		G_SpawnInt( "health", "0", &health );
		if ( health ) {
			ent->takedamage = qtrue;
		}

		if ( ent->targetname || health  ) {
			// non touch/shoot doors
			ent->think = Think_MatchTeam;
		} else {
			ent->think = Think_SpawnNewDoorTrigger;
		}
	}
	
	if(!(ent->flags & FL_TEAMSLAVE)) {
		DoorGetMessage(ent);
	}

	level.numBrushEnts++;
}

/*
===============================================================================

PLAT

===============================================================================
*/

/*
==============
Touch_Plat

Don't allow decent if a living player is on it
===============
*/
void Touch_Plat( gentity_t *ent, gentity_t *other, trace_t *trace ) {
	if ( !other->client || other->client->ps.stats[STAT_HEALTH] <= 0 ) {
		return;
	}

	#ifdef G_LUA
	if(ent->luaTrigger)
		LuaHook_G_EntityTrigger(ent->luaTrigger, ent-g_entities, other-g_entities);
	#endif

	// delay return-to-pos1 by one second
	if ( ent->moverState == MOVER_POS2 ) {
		ent->nextthink = level.time + 1000;
	}
}

/*
==============
Touch_PlatCenterTrigger

If the plat is at the bottom position, start it going up
===============
*/
void Touch_PlatCenterTrigger(gentity_t *ent, gentity_t *other, trace_t *trace ) {
	if ( !other->client ) {
		return;
	}

	#ifdef G_LUA
	if(ent->luaTrigger)
		LuaHook_G_EntityTrigger(ent->luaTrigger, ent-g_entities, other-g_entities);
	#endif

	if ( ent->parent->moverState == MOVER_POS1 ) {
		G_Mover_UseBinaryMover( ent->parent, ent, other );
	}
}

void func_plat_use(gentity_t *ent, gentity_t *other, gentity_t *activator) {
	if (ent->parent->moverState == MOVER_POS1 ) {
		G_Mover_UseBinaryMover(ent->parent, other, activator);
	}
}


/*
================
SpawnPlatTrigger

Spawn a trigger in the middle of the plat's low position
Elevator cars require that the trigger extend through the entire low position,
not just sit on top of it.
================
*/
void SpawnPlatTrigger( gentity_t *ent ) {
	gentity_t	*trigger;
	vec3_t	tmin, tmax;

	// the middle trigger will be a thin trigger just
	// above the starting position
	trigger = G_Spawn();
	trigger->touch = Touch_PlatCenterTrigger;
	trigger->r.contents = CONTENTS_TRIGGER;
	trigger->parent = ent;
	
	tmin[0] = ent->pos1[0] + ent->r.mins[0] + 33;
	tmin[1] = ent->pos1[1] + ent->r.mins[1] + 33;
	tmin[2] = ent->pos1[2] + ent->r.mins[2];

	tmax[0] = ent->pos1[0] + ent->r.maxs[0] - 33;
	tmax[1] = ent->pos1[1] + ent->r.maxs[1] - 33;
	tmax[2] = ent->pos1[2] + ent->r.maxs[2] + 8;

	if ( tmax[0] <= tmin[0] ) {
		tmin[0] = ent->pos1[0] + (ent->r.mins[0] + ent->r.maxs[0]) *0.5;
		tmax[0] = tmin[0] + 1;
	}
	if ( tmax[1] <= tmin[1] ) {
		tmin[1] = ent->pos1[1] + (ent->r.mins[1] + ent->r.maxs[1]) *0.5;
		tmax[1] = tmin[1] + 1;
	}
	
	VectorCopy (tmin, trigger->r.mins);
	VectorCopy (tmax, trigger->r.maxs);

	trap_LinkEntity (trigger);
}


/*QUAKED func_plat (0 .5 .8) NO_TOUCH
-----DESCRIPTION-----
Binary mover...I don't know much more until I have tested it...
Plats are always drawn in the extended position so they will light correctly.

-----SPAWNFLAGS-----
NO_TOUCH - instead of staying up as long as someone touches it, it will wait "wait" seconds and return

-----KEYS-----
"targetname" - if targeted will only move when used
"lip" - default 8, protrusion above rest position
"height" - total height of movement, defaults to model height
"speed" - overrides default 200.
"dmg" - overrides default 2
"model2" - .md3 model to also draw
"color" - constantLight color
"light" - constantLight radius
"wait" - how many seconds to wait before returning

q3map2:
"_clone" - _clonename of entity to clone brushes from. Note: this entity still needs at least one brush which gets replaced.
"_clonename" - see _clone
"_castShadows" OR "_cs" - sets whether the entity casts shadows
"_receiveShadows" OR "_rs" - sets whether the entity receives shadows
*/
void SP_func_plat (gentity_t *ent) {
	float		lip, height;
	char		*sound;

	G_SpawnString("soundstart", "sound/movers/plats/largeplatstart.wav", &sound);
	ent->sound1to2 = ent->sound2to1 = G_SoundIndex(sound);
	G_SpawnString("soundstop", "sound/movers/plats/largeplatstop.wav", &sound);
	ent->soundPos1 = ent->soundPos2 = G_SoundIndex(sound);

	VectorClear (ent->s.angles);

	G_SpawnFloat( "speed", "200", &ent->speed );
	G_SpawnInt( "dmg", "2", &ent->damage );
	G_SpawnFloat( "wait", "1", &ent->wait );
	G_SpawnFloat( "lip", "8", &lip );

	ent->wait *= 1000;

	// create second position
	trap_SetBrushModel( ent, ent->model );

	if ( !G_SpawnFloat( "height", "0", &height ) ) {
		height = (ent->r.maxs[2] - ent->r.mins[2]) - lip;
	}

	// pos1 is the rest (bottom) position, pos2 is the top
	VectorCopy( ent->s.origin, ent->pos2 );
	VectorCopy( ent->pos2, ent->pos1 );

	if(ent->count) // for SP support
		ent->pos1[2] -= ent->count;
	else
		ent->pos1[2] -= height;

	InitMover( ent );

	// touch function keeps the plat from returning while
	// a live player is standing on it
	if ( !(ent->spawnflags&1) )
	{//we don't want the touch-holding
		ent->touch = Touch_Plat;
	}

	ent->blocked = Blocked_Door;

	ent->parent = ent;	// so it can be treated as a door

	// spawn the trigger if one hasn't been custom made
	if ( !ent->targetname || ent->count ) {
		SpawnPlatTrigger(ent);
	} 

	level.numBrushEnts++;
}


/*
===============================================================================

BUTTON

===============================================================================
*/

/*
==============
Touch_Button

===============
*/
void Touch_Button(gentity_t *ent, gentity_t *other, trace_t *trace ) {
	if ( !other->client ) {
		return;
	}

	#ifdef G_LUA
	if(ent->luaTrigger)
		LuaHook_G_EntityTrigger(ent->luaTrigger, ent-g_entities, other-g_entities);
	#endif

	if ( ent->moverState == MOVER_POS1 ) {
		G_Mover_UseBinaryMover( ent, other, other );
	}
}


/*QUAKED func_button (0 .5 .8) ?
-----DESCRIPTION-----
When a button is touched, it moves some distance in the direction of it's angle, triggers all of it's targets, waits some time, then returns to it's original position where it can be triggered again.

-----SPAWNFLAGS-----
none

-----KEYS-----
"model2" - .md3 model to also draw
"angle" - determines the opening direction
"target" - all entities with a matching targetname will be used
"speed" - override the default 40 speed
"wait" - override the default 1 second wait (-1 = never return)
"lip" - override the default 4 pixel lip remaining at end of move
"health" - if set, the button must be killed instead of touched
"color" - constantLight color
"light" - constantLight radius

q3map2:
"_clone" - _clonename of entity to clone brushes from. Note: this entity still needs at least one brush which gets replaced.
"_clonename" - see _clone
"_castShadows" OR "_cs" - sets whether the entity casts shadows
"_receiveShadows" OR "_rs" - sets whether the entity receives shadows
*/
void SP_func_button( gentity_t *ent ) {
	vec3_t		abs_movedir;
	float		distance;
	vec3_t		size;
	float		lip;
	char		*sound;

	if(!ent->tmpEntity) { // not modified by spawn file
		G_SpawnString("sounduse", "sound/movers/switches/forgepos.wav", &sound);
		ent->sound1to2 = G_SoundIndex(sound);
	} else { // modified by spawnfile
		if(!ent->sound1to2)
			ent->sound1to2 = G_SoundIndex("sound/movers/switches/forgepos.wav");
	}

	if ( !ent->speed ) {
		ent->speed = 40;
	}

	if ( !ent->wait ) {
		ent->wait = 1;
	}
	ent->wait *= 1000;

	// first position
	VectorCopy( ent->s.origin, ent->pos1 );

	// calculate second position
	trap_SetBrushModel( ent, ent->model );

	G_SpawnFloat( "lip", "4", &lip );

	G_SetMovedir( ent->s.angles, ent->movedir );
	abs_movedir[0] = fabs(ent->movedir[0]);
	abs_movedir[1] = fabs(ent->movedir[1]);
	abs_movedir[2] = fabs(ent->movedir[2]);
	VectorSubtract( ent->r.maxs, ent->r.mins, size );
	distance = abs_movedir[0] * size[0] + abs_movedir[1] * size[1] + abs_movedir[2] * size[2] - lip;
	VectorMA (ent->pos1, distance, ent->movedir, ent->pos2);

	if (ent->health) {
		// shootable button
		ent->takedamage = qtrue;
	} else {
		// touchable button
		ent->touch = Touch_Button;
	}

	InitMover( ent );

	level.numBrushEnts++;
}



/*
===============================================================================

TRAIN

===============================================================================
*/


#define TRAIN_START_ON		1
#define TRAIN_TOGGLE		2
#define TRAIN_BLOCK_STOPS	4

/*
===============
Think_BeginMoving

The wait time at a corner has completed, so start moving again
===============
*/
void Think_BeginMoving( gentity_t *ent ) {
	ent->s.pos.trTime = level.time;
	ent->s.pos.trType = TR_LINEAR_STOP;
}

/*
===============
Reached_Train
===============
*/
void Reached_Train( gentity_t *ent ) {
	gentity_t		*next = NULL;
	float			speed;
	vec3_t			move;
	float			length;

	// copy the apropriate values
	if(ent->nextTrain)
		next = ent->nextTrain;
	if ( !next || !next->nextTrain ) {
		return;		// just stop
	}

	#ifdef G_LUA
	if(ent->luaTrigger)
	{
		LuaHook_G_EntityTrigger(ent->luaTrigger, ent->s.number, -1);
	}
	if(next->luaTrigger)
	{
		LuaHook_G_EntityTrigger(next->luaTrigger, next->s.number, ent->s.number);
	}
	#endif

	// fire all other targets
	G_UseTargets( next, NULL );

	// set the new trajectory
	ent->nextTrain = next->nextTrain;
	VectorCopy( next->s.origin, ent->pos1 );
	VectorCopy( next->nextTrain->s.origin, ent->pos2 );

	// if the path_corner has a speed, use that
	if ( next->speed ) {
		speed = next->speed;
	} else {
		// otherwise use the train's speed
		speed = ent->speed;
	}
	if ( speed < 1 ) {
		speed = 1;
	}

	// calculate duration
	VectorSubtract( ent->pos2, ent->pos1, move );
	length = VectorLength( move );

	ent->s.pos.trDuration = length * 1000 / speed;

	// looping sound
	ent->s.loopSound = next->soundLoop;

	// start it going
	SetMoverState( ent, MOVER_1TO2, level.time );

	// if there is a "wait" value on the target, don't start moving yet
	if ( next->wait ) {
		ent->nextthink = level.time + next->wait * 1000;
		ent->think = Think_BeginMoving;
		ent->s.pos.trType = TR_STATIONARY;
	}
}


/*
===============
Think_SetupTrainTargets

Link all the corners together
===============
*/
void Think_SetupTrainTargets( gentity_t *ent ) {
	gentity_t		*path = NULL, *next = NULL, *start = NULL;

	ent->nextTrain = G_Find( NULL, FOFS(targetname), ent->target );
	if ( !ent->nextTrain ) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] func_train at %s with an unfound target\n",
			vtos(ent->r.absmin) ););
		return;
	}

	start = NULL;
	for ( path = ent->nextTrain ; path != start ; path = next ) {
		if ( !start ) {
			start = path;
		}

		if ( !path->target ) {
			DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] Train corner at %s without a target\n",
				vtos(path->s.origin) ););
			return;
		}

		// find a path_corner among the targets
		// there may also be other targets that get fired when the corner
		// is reached
		next = NULL;
		do {
			next = G_Find( next, FOFS(targetname), path->target );
			if ( !next ) {
				DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] Train corner at %s without a target path_corner\n",
					vtos(path->s.origin) ););
				return;
			}
		} while ( strcmp( next->classname, "path_corner" ) );

		path->nextTrain = next;
	}

	// start the train moving from the first corner
	Reached_Train( ent );
}



/*QUAKED path_corner (.5 .3 0) (-8 -8 -8) (8 8 8)
-----DESCRIPTION-----
func_train path corners.

-----SPAWNFLAGS-----
none

-----KEYS-----
"target" - next path corner and other targets to fire
"speed" - speed to move to the next corner
"wait" - seconds to wait before behining move to next corner
*/
void SP_path_corner( gentity_t *self ) {
	if ( !self->targetname ) {
		DEVELOPER(G_Printf (S_COLOR_YELLOW "[Entity-Error] path_corner with no targetname at %s\n", vtos(self->s.origin)););
		G_FreeEntity( self );
		return;
	}
	// path corners don't need to be linked in
}



/*QUAKED func_train (0 .5 .8) ? START_ON TOGGLE BLOCK_STOPS START_INVISIBLE
-----DESCRIPTION-----
A train is a mover that moves between path_corner target points.
Can be turned invisible.
Trains MUST HAVE AN ORIGIN BRUSH.
The train spawns at the first target it is pointing at.

-----SPAWNFLAGS-----
1: START_ON - the train will begin to move after spawn
2: TOGGLE - the train can be toggled
4: BLOCK_STOPS - the train will stop if blocked
8: START_INVISIBLE - the train will be invisible at spawn

-----KEYS-----
"model2" - .md3 model to also draw
"speed" - default 100
"dmg" - default 2
"noise" - looping sound to play when the train is in motion
"target" - next path corner
"color" - constantLight color
"light" - constantLight radius
"swapname" - targetname for visiblility change

q3map2:
"_clone" - _clonename of entity to clone brushes from. Note: this entity still needs at least one brush which gets replaced.
"_clonename" - see _clone
"_castShadows" OR "_cs" - sets whether the entity casts shadows
"_receiveShadows" OR "_rs" - sets whether the entity receives shadows
*/
void SP_func_train (gentity_t *self) {
	VectorClear (self->s.angles);

	if (self->spawnflags & TRAIN_BLOCK_STOPS) {
		self->damage = 0;
	} else {
		if (!self->damage) {
			self->damage = 2;
		}
	}

	if ( !self->speed ) {
		self->speed = 100;
	}

	if ( !self->target ) {
		DEVELOPER(G_Printf (S_COLOR_YELLOW "[Entity-Error] func_train without a target at %s\n", vtos(self->r.absmin)););
		G_FreeEntity( self );
		return;
	}

	trap_SetBrushModel( self, self->model );
	InitMover( self );

	if(self->spawnflags & 8) {
		self->s.solid = 0;
		self->r.contents = 0;
		self->clipmask = 0;
		self->r.svFlags |= SVF_NOCLIENT;
		self->s.eFlags |= EF_NODRAW;
		self->count = 0;
	} else 
		self->count = 1;

	self->reached = Reached_Train;

	// start trains on the second frame, to make sure their targets have had
	// a chance to spawn
	self->nextthink = level.time + FRAMETIME;
	self->think = Think_SetupTrainTargets;

	level.numBrushEnts++;
}

/*
===============================================================================

STATIC

===============================================================================
*/


/*QUAKED func_static (0 .5 .8) ?
-----DESCRIPTION-----
A bmodel that just sits there, doing nothing.
Can be used for conditional walls and models.
If it has an origin-Brush it can be moved using Lua.

-----SPAWNFLAGS-----
none

-----KEYS-----
"model2" - .md3 model to also draw
"color" - constantLight color
"light" - constantLight radius

q3map2:
"_clone" - _clonename of entity to clone brushes from. Note: this entity still needs at least one brush which gets replaced.
"_clonename" - see _clone
"_castShadows" OR "_cs" - sets whether the entity casts shadows
"_receiveShadows" OR "_rs" - sets whether the entity receives shadows
*/
void SP_func_static( gentity_t *ent ) {
	trap_SetBrushModel( ent, ent->model );
	G_SetOrigin(ent, ent->s.origin);
	G_SetAngles(ent, ent->s.angles);
	VectorCopy( ent->s.origin, ent->pos1);
	InitMover( ent );
	VectorCopy( ent->s.origin, ent->s.pos.trBase );
	VectorCopy( ent->s.origin, ent->r.currentOrigin );

	level.numBrushEnts++;
}


/*
===============================================================================

FORCE FIELD

===============================================================================
*/

extern gentity_t	*G_TestEntityPosition( gentity_t *ent );
void forcefield_use( gentity_t *ent, gentity_t *other, gentity_t *activator );
void forcefield_think(gentity_t *ent);

void FieldReturnSolid( gentity_t *ent )
{
	//once a frame, see if it's clear.
	ent->clipmask = CONTENTS_BODY;
	if ( G_TestEntityPosition( ent ) == NULL )
	{
		trap_SetBrushModel( ent, ent->model );
		InitMover( ent );
		VectorCopy( ent->s.origin, ent->s.pos.trBase );
		VectorCopy( ent->s.origin, ent->r.currentOrigin );
		ent->r.svFlags &= ~SVF_NOCLIENT;
		ent->s.eFlags &= ~EF_NODRAW;
		ent->use = forcefield_use;
		ent->clipmask = 0;
		ent->r.contents = CONTENTS_SOLID;
		ent->nextthink=level.time + ent->waterlevel;
		ent->think = forcefield_think;
		G_AddEvent(ent, EV_GENERAL_SOUND, ent->sound1to2);

		if ( !(ent->spawnflags&1) )
		{//START_OFF doesn't effect area portals
			trap_AdjustAreaPortalState( ent, qfalse );
		}
	}
	else
	{
		ent->clipmask = 0;
		ent->think = FieldReturnSolid;
		ent->nextthink = level.time + FRAMETIME;
	}
}

// Turn the shield off to allow a friend to pass through.
static void FieldGoNotSolid(gentity_t *self)
{
	// make the shield non-solid very briefly
	self->r.contents = CONTENTS_NONE;
	self->s.eFlags &= ~EF_NODRAW;					//Commenting this should make it look like the player passes through..
	// nextthink needs to have a large enough interval to avoid excess accumulation of Activate messages
	self->nextthink = level.time + 200;
	self->think = FieldReturnSolid;

	//TiM - Make the field visible
	self->r.svFlags &= ~SVF_NOCLIENT;

	trap_LinkEntity(self);

	// Play raising sound...
	G_AddEvent(self, EV_GENERAL_SOUND, self->soundPos1);
}

void forcefield_think(gentity_t *ent)
{
	ent->r.svFlags |= SVF_NOCLIENT;
	ent->s.eFlags |= EF_NODRAW;
	ent->nextthink = 0;

	if( !ent->count )
	{
		G_AddEvent(ent, EV_GENERAL_SOUND, ent->soundLoop);
	}
}

static void forcefield_pain(gentity_t *ent, gentity_t *attacker, int damage)
{
	int i;
	if ( !ent->timestamp )
		return;

	#ifdef G_LUA
	if(ent->luaHurt)
		LuaHook_G_EntityHurt(ent->luaHurt, ent-g_entities, attacker-g_entities, attacker-g_entities);
	#endif
	
	ent->r.svFlags &= ~SVF_NOCLIENT;
	ent->s.eFlags &= ~EF_NODRAW;
	ent->nextthink = level.time + ent->old_health;
	ent->think = forcefield_think;

	if(ent->yellowsound) { // fire hittargets
		for(i = 0; i < MAX_GENTITIES; i++) {
			if(!Q_stricmp(ent->yellowsound, g_entities[i].targetname))
				g_entities[i].use(&g_entities[i], NULL, ent);
		}
	}

	G_AddEvent(ent, EV_GENERAL_SOUND, ent->sound2to1);
}

static void forcefield_deactivate( gentity_t* ent )
{
	ent->s.solid	= 0;
	ent->r.contents = 0;
	ent->clipmask	= 0;
	ent->r.svFlags	|= SVF_NOCLIENT;
	ent->s.eFlags	|= EF_NODRAW;

	ent->think = 0;
	ent->nextthink = -1;
	
	if ( !(ent->spawnflags&1) )
	{//START_OFF doesn't effect area portals
		trap_AdjustAreaPortalState( ent, qtrue );
	}

}

void forcefield_use( gentity_t *ent, gentity_t *other, gentity_t *activator )
{
	if ( !ent->timestamp )
	{//become solid again
		ent->timestamp = 1;
		FieldReturnSolid( ent );

		//use targets
		if(ent->target && ent->target[0])
		{
			G_UseTargets(ent, activator);
		}
	}
	else //deactivate field
	{
		G_AddEvent( ent, EV_GENERAL_SOUND, ent->soundPos1 );

		//use targets
		if(ent->target && ent->target[0])
		{
			G_UseTargets(ent, activator);
		}

		ent->timestamp	= 0;
		ent->r.svFlags &= ~SVF_NOCLIENT;
		ent->s.eFlags &= ~EF_NODRAW;
		ent->think = forcefield_deactivate;
		ent->nextthink = level.time + ent->waterlevel;
	}
}

static void forcefield_touch( gentity_t *ent, gentity_t *other, trace_t *trace )
{
	if ( !other->client || !ent->timestamp )
		return;

	#ifdef G_LUA
	if(ent->luaTrigger)
		LuaHook_G_EntityTrigger(ent->luaTrigger, ent-g_entities, other-g_entities);
	#endif

	ent->r.svFlags &= ~SVF_NOCLIENT;
	ent->s.eFlags &= ~EF_NODRAW;

	if ( ((ent->spawnflags & 4) && IsAdmin(other)) || 
		((rpg_borgAdapt.integer && rpg_borgMoveThroughFields.integer && IsBorg(other) && !(ent->spawnflags & 256))) )
	{
		FieldGoNotSolid( ent );
	}
	else
	{ //throw the player back
		if ( other->client->ps.pm_type != PM_NOCLIP && ent->count )
		{
			vec3_t	org;
			vec3_t dir;

			VectorAverage( ent->r.mins, ent->r.maxs, org); 

			trap_Trace( trace, other->client->ps.origin, NULL, NULL, org, other->client->ps.clientNum, MASK_SOLID );
			VectorCopy( trace->plane.normal, dir );

			VectorScale( dir, ent->count, dir);
			//Copy it straight to our velocity (this will mean the player will literally be thrown back)
			VectorCopy( dir, other->client->ps.velocity );
			other->client->ps.pm_time = 160;		// hold time
			other->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;

			G_AddEvent(ent, EV_GENERAL_SOUND, ent->soundLoop);
		}

		if ( ent->damage )
		{
			G_Damage (other, ent, ent, NULL, NULL, ent->damage, 0, MOD_FORCEFIELD);
		}

		ent->nextthink = level.time+ent->soundPos2;
		ent->think = forcefield_think;
	}
}

#define SF_USEABLE			(1<<0)

/*QUAKED func_forcefield (0 .5 .8) ? STARTOFF DONTTOGGLE ADMINS AUTOANIM X X X X NO_BORG
-----DESCRIPTION-----
A brush that remains invisible until it is contacted by a player, where it temporarily
becomes visible.

-----SPAWNFLAGS-----
1: STARTOFF - Spawns in an off state, and must be used to be activated
2: DONTTOGGLE - This entity cannot be used to be switched on and off (ie always on)
4: ADMINS - Players in admin classes can move through the field
8: AUTOANIM - If a model is spawned with it, it will animate at 10FPS repeatedly
16: X - Unknown, do not use
32: X - Unknown, do not use
64: X - Unknown, do not use
128: X - Unknown, do not use
256: NO_BORG - If set, borg can't move through

-----KEYS-----
"flareWait" - How long the forcefield remains visible after the player contacts it (milliseconds)(default 150)
"activateWait" - How long the forcefield remains visible after activation/deactivation(default 500)
"damageWait" - How long the forecefield remains visible after it has been shot (default 400)
"kickback" - How far the player gets pushed back when they touch the forcefield (default 50)
"damage" - Damage sustained when the player touches the field (default 0 )
"target" - Will fire this target every time it is toggled
"model2" - .md3 model to also draw
"color" - constantLight color
"light" - constantLight radius
"activateSnd" - sound file to play when the field is activated
"damageSnd" - sound to play when the field is shot
"touchSnd" - sound to play if the field is contacted by a player.
"deactivateSnd" - sound to play when the field is turned off
"hittarget" - target to fire when hit

q3map2:
"_clone" - _clonename of entity to clone brushes from. Note: this entity still needs at least one brush which gets replaced.
"_clonename" - see _clone
"_castShadows" OR "_cs" - sets whether the entity casts shadows
"_receiveShadows" OR "_rs" - sets whether the entity receives shadows
*/
void SP_func_forcefield( gentity_t *ent )
{
	char *activate, *damage, *touch, *deactivate, *temp;
	// timestamp keeps track of whether the field is on or off
	ent->timestamp = 1;

	trap_SetBrushModel( ent, ent->model );
	InitMover( ent );
	VectorCopy( ent->s.origin, ent->s.pos.trBase );
	VectorCopy( ent->s.origin, ent->r.currentOrigin );

	G_SpawnString( "activateSnd", "sound/movers/forcefield/forcefieldon.wav", &activate );
	G_SpawnString( "damageSnd", "sound/ambience/spark5.wav", &damage );
	G_SpawnString( "touchSnd", "sound/movers/forcefield/forcefieldtouch.wav", &touch );
	G_SpawnString( "deactivateSnd", "sound/movers/forcefield/forcefieldoff.wav", &deactivate );

	if(G_SpawnString( "hittarget", "", &temp))
		ent->yellowsound = G_NewString(temp);


	G_SpawnInt( "kickback", "50", &ent->count );

	G_SpawnInt( "damage", "0", &ent->damage );

	ent->sound1to2 = G_SoundIndex( activate );
	ent->sound2to1 = G_SoundIndex( damage );
	ent->soundLoop = G_SoundIndex( touch );
	ent->soundPos1 = G_SoundIndex( deactivate );

	G_SpawnInt( "flareWait", "150", &ent->soundPos2 );
	G_SpawnInt( "activateWait", "500", &ent->waterlevel );
	G_SpawnInt( "damageWait", "400", &ent->old_health );

	// remember our initial position
	VectorCopy(ent->s.origin, ent->pos1);

	ent->r.svFlags |= SVF_NOCLIENT;
	ent->s.eFlags |= EF_NODRAW;

	if (ent->spawnflags & 1 && !(ent->spawnflags & 2 ) )
	{
		ent->s.solid = 0;
		ent->r.contents = 0;
		ent->clipmask = 0;
		ent->timestamp = 0;
	}

	if (ent->spawnflags & 8)
	{
		ent->s.eFlags |= EF_ANIM_ALLFAST;
	}

	if ( !(ent->spawnflags & 2 ) )
		ent->use = forcefield_use;

	ent->pain = forcefield_pain;
	ent->touch = forcefield_touch;
	ent->takedamage = qtrue;
	ent->health = -1;

	ent->think = forcefield_think;
	ent->nextthink = level.time + 1000;

	trap_LinkEntity( ent );

	level.numBrushEnts++;
}


/*
===============================================================================

ROTATING

===============================================================================
*/


/*QUAKED func_rotating (0 .5 .8) ? START_ON X X_AXIS Y_AXIS x INVISIBLE NO_TOGGLE
-----DESCRIPTION-----
You need to have an origin brush as part of this entity. The center of that brush will be
the point around which it is rotated. It will rotate around the Z axis by default.  You can
check either the X_AXIS or Y_AXIS box to change that.

-----SPAWNFLAGS-----
1: START_ON - entity will move at spawn
2: X - Unknown, do not use
4: X-AXIS - rotate around the X-Axis
8: Y-AXIS - rotate around the Y-Axis
16: X - Unknown, do not use
32: INVISIBLE - will be invisible at spawn
64: NO_TOGGLE - will not be toggable

-----KEYS-----
"model2"	.md3 model to also draw
"speed"		determines how fast it moves; default value is 100.
"dmg"		damage to inflict when blocked (2 default)
"color"		constantLight color
"light"		constantLight radius
"swapname"  visibility swap (activator needs NO_ACTIVATOR/SELF)

"pos1"		Angles to end up at in addition to current angles- pitch yaw and roll.  Eg: 0 90 45 

q3map2:
"_clone" - _clonename of entity to clone brushes from. Note: this entity still needs at least one brush which gets replaced.
"_clonename" - see _clone
"_castShadows" OR "_cs" - sets whether the entity casts shadows
"_receiveShadows" OR "_rs" - sets whether the entity receives shadows
*/

//RPG-X Use Function
void func_rotating_use (gentity_t *ent, gentity_t *other, gentity_t *activator)
{
	if(!Q_stricmp(ent->swapname, activator->target)) {
		if(ent->count == 1) {
			ent->s.apos.trType = TR_LINEAR_STOP;
			ent->s.solid = 0;
			ent->r.contents = 0;
			ent->clipmask = 0;
			ent->r.svFlags |= SVF_NOCLIENT;
			ent->s.eFlags |= EF_NODRAW;
			ent->count = 0;
		} else {
			ent->clipmask = CONTENTS_BODY;
			trap_SetBrushModel( ent, ent->model );
			ent->r.svFlags &= ~SVF_NOCLIENT;
			ent->s.eFlags &= ~EF_NODRAW;
			ent->clipmask = 0;
			ent->count = 1;
			ent->s.apos.trType = TR_LINEAR;
		}
	} else {
		ent->distance = level.time + (ent->wait * 1000);
	}
}

void SP_func_rotating (gentity_t *ent) {
	float	speed;

	if ( !ent->speed ) {
		ent->speed = 100;
	}

	if ( !ent->distance ) {
		ent->distance = 0;
	}

	//ent->use = func_rotating_use; //RPG-X | GSIO01 --- weren't you aware that InitMover sets the use func to G_Mover_UseBinaryMover Phenix??

	if (!ent->booleanstate)
	{
		ent->booleanstate = qfalse;
	}

	//RPG-X: Phenix - If this is not toggled (as in no target name set) then just do orignal crap
	if ( !ent->targetname || (ent->spawnflags & 64) ) {

		// set the axis of rotation
		ent->s.apos.trType = TR_LINEAR;
		if ( ent->spawnflags & 4 ) {
			ent->s.apos.trDelta[2] = ent->speed;
		} else if ( ent->spawnflags & 8 ) {
			ent->s.apos.trDelta[0] = ent->speed;
		} else {
			ent->s.apos.trDelta[1] = ent->speed;
		}

		if (!ent->damage) {
			ent->damage = 2;
		}

		trap_SetBrushModel( ent, ent->model );
		InitMover( ent );

		VectorCopy( ent->s.origin, ent->s.pos.trBase );
		VectorCopy( ent->s.pos.trBase, ent->r.currentOrigin );
		VectorCopy( ent->s.apos.trBase, ent->r.currentAngles );

	} else {
		//Its a toggle rotating thingy
		if ( !ent->wait ) {
			Com_Printf("Toggle Func_Rotating without wait\n");
			return;
		}

		if (( ent->distance > level.time ) && ( ent->distance != 0 )) {

			//Are we going back?
			if (ent->booleanstate == qtrue)				//J2J to Phenix: you were seeing if ent->booleanstate = qtrue (which would allways be true), you need two equals to compare (==), corrrected for you.
			{
				speed = -(ent->speed);
			} else {
				speed = ent->speed;
			}

			// set the axis of rotation
			ent->s.apos.trType = TR_LINEAR;
			if ( ent->spawnflags & 4 ) {
				ent->s.apos.trDelta[2] = speed;
			} else if ( ent->spawnflags & 8 ) {
				ent->s.apos.trDelta[0] = speed;
			} else {
				ent->s.apos.trDelta[1] = speed;
			}

			if (!ent->damage) {
				ent->damage = 2;
			}

			trap_SetBrushModel( ent, ent->model );
			InitMover( ent );

			VectorCopy( ent->s.origin, ent->s.pos.trBase );
			VectorCopy( ent->s.pos.trBase, ent->r.currentOrigin );
			VectorCopy( ent->s.apos.trBase, ent->r.currentAngles );
		} else if (ent->distance != 0) {
			if (ent->booleanstate == qtrue) {
				ent->booleanstate = qfalse;
			} else {
				ent->booleanstate = qtrue;
			}
			ent->distance = 0;
		}
	}

	trap_LinkEntity( ent );

	if(ent->spawnflags & 32) {
		ent->s.solid = 0;
		ent->r.contents = 0;
		ent->clipmask = 0;
		ent->r.svFlags |= SVF_NOCLIENT;
		ent->s.eFlags |= EF_NODRAW;
		ent->count = 0;
	} else 
		ent->count = 1;

	ent->use = func_rotating_use;

	level.numBrushEnts++;
}


/*QUAKED func_door_rotating (0 .5 .8) ? START_OPEN CRUSHER REVERSE X_AXIS Y_AXIS LOCKED ADMIN_ONLY CORRIDOR
-----DESCRIPTION-----
This is the rotating door... just as the name suggests it's a door that rotates around it's origin Brush.
The Axis to rotate around is determined by spawnflag.

-----SPAWNFLAGS-----
1: START_OPEN - the door to moves to its destination when spawned, and operate in reverse.
2: CRUSHER - The door will stick in it's end position
4: REVERSE - if you want the door to open in the other direction, use this switch.
8: X_AXIS - rotate around the X-axis instead of the Z-axis
16: Y_AXIS - rotate around the Y-axis instead of the Z-axis
32: LOCKED - Door is locked at spawn
64: ADMIN_ONLY - only admins can use this door
128: CORRIDOR - will have a reduced autotrigger volume.
  
You need to have an origin brush as part of this entity.  The center of that brush will be
the point around which it is rotated. It will rotate around the Z axis by default.  You can
check either the X_AXIS or Y_AXIS box to change that.

"model2" - .md3 model to also draw
"distance" - how many degrees the door will open
"speed" - how fast the door will open (degrees/second)
"wait" - time to wait before returning, default = 2, -1 = move on toggle
"color" - constantLight color
"light" - constantLight radius
"targetname" - door can only open/close when used by anonther entity
"targetname2" - for target_doorlock
"soundstart" - sound played when start moving
"soundstop" - sound played when stop moving
"soundlocked" - sound played when locked

q3map2:
"_clone" - _clonename of entity to clone brushes from. Note: this entity still needs at least one brush which gets replaced.
"_clonename" - see _clone
"_castShadows" OR "_cs" - sets whether the entity casts shadows
"_receiveShadows" OR "_rs" - sets whether the entity receives shadows
*/

void SP_func_door_rotating ( gentity_t *ent ) {
	char *sound;
	G_SpawnString("soundstart", "sound/movers/doors/largedoorstart.wav", &sound);
	ent->sound1to2 = ent->sound2to1 = G_SoundIndex(sound);
	G_SpawnString("soundstop", "sound/movers/doors/largedoorstop.wav", &sound);
	ent->soundPos1 = ent->soundPos2 = G_SoundIndex(sound);
	G_SpawnString("soundlocked", "sound/ambience/voyager/doorchime.mp3", &sound);
	ent->n00bCount = G_SoundIndex(sound);

	ent->blocked = Blocked_Door;

	//RPG-X | GSIO01 | 08/05/2009:
	if(ent->spawnflags & 32)
		ent->flags ^= FL_LOCKED;

	// default speed of 120
	if (!ent->speed)
		ent->speed = 120;

	// if speed is negative, positize it and add reverse flag
	if ( ent->speed < 0 ) {
		ent->speed *= -1;
		ent->spawnflags |= 8;
	}

	// default of 2 seconds
	if (!ent->wait)
		ent->wait = 2;
	ent->wait *= 1000;
	
	// set the axis of rotation
	VectorClear( ent->movedir );
	VectorClear( ent->s.angles );
	
	if ( ent->spawnflags & 8 ) {
		ent->movedir[2] = 1.0;
	} else if ( ent->spawnflags & 16 ) {
		ent->movedir[0] = 1.0;
	} else {
		ent->movedir[1] = 1.0;
	}

	// reverse direction if necessary
	if ( ent->spawnflags & 4 )
		VectorNegate ( ent->movedir, ent->movedir );

	// default distance of 90 degrees. This is something the mapper should not
	// leave out, so we'll tell him if he does.
	if ( !ent->distance ) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] %s at %s with no distance set.\n",
		ent->classname, vtos(ent->s.origin)););
		ent->distance = 90.0;
	}
	
	VectorCopy( ent->s.angles, ent->pos1 );
	trap_SetBrushModel( ent, ent->model );
	VectorMA ( ent->pos1, ent->distance, ent->movedir, ent->pos2 );

	// if "start_open", reverse position 1 and 2
	if ( ent->spawnflags & 1 ) {
		vec3_t	temp;

		VectorCopy( ent->pos2, temp );
		VectorCopy( ent->s.angles, ent->pos2 );
		VectorCopy( temp, ent->pos1 );
		VectorNegate ( ent->movedir, ent->movedir );
	}
	
	// set origin
	VectorCopy( ent->s.origin, ent->s.pos.trBase );
	VectorCopy( ent->s.pos.trBase, ent->r.currentOrigin );

	InitRotator( ent );

	ent->nextthink = level.time + FRAMETIME;

	if ( ! (ent->flags & FL_TEAMSLAVE ) ) {
		int health;

		G_SpawnInt( "health", "0", &health );
		if ( health ) {
			ent->takedamage = qtrue;
		}
		if ( ent->targetname || health ) {
			// non touch/shoot doors
			ent->think = Think_MatchTeam;
		} else {
			ent->think = Think_SpawnNewDoorTrigger;
		}
	}
	
	if(! (ent->flags & FL_TEAMSLAVE))
		DoorGetMessage(ent);

	level.numBrushEnts++;
}

/*
===============================================================================

BOBBING

===============================================================================
*/


/*QUAKED func_bobbing (0 .5 .8) ? X_AXIS Y_AXIS
-----DESCRIPTION-----
Don't now what this looks like.
Normally bobs on the Z axis

-----SPAWNFLAGS-----
1: X-AXIS - bobs on the X-Axis
2: Y-AXIS - bobs on the Y-Axis

-----KEYS-----
"model2" - .md3 model to also draw
"height" - amplitude of bob (32 default)
"speed" - seconds to complete a bob cycle (4 default)
"phase" - the 0.0 to 1.0 offset in the cycle to start at
"dmg" - damage to inflict when blocked (2 default)
"color" - constantLight color
"light" - constantLight radius

q3map2:
"_clone" - _clonename of entity to clone brushes from. Note: this entity still needs at least one brush which gets replaced.
"_clonename" - see _clone
"_castShadows" OR "_cs" - sets whether the entity casts shadows
"_receiveShadows" OR "_rs" - sets whether the entity receives shadows
*/
void SP_func_bobbing (gentity_t *ent) {
	float		height;
	float		phase;

	if(!ent->tmpEntity) { // only do this if this is not done by spawn file
		G_SpawnFloat( "height", "32", &height );
		G_SpawnFloat( "phase", "0", &phase );
	} else { // spawn file!
		height = ent->distance;
		ent->distance = 0;
		phase = ent->wait;
		ent->wait = 0;
	}

	trap_SetBrushModel( ent, ent->model );
	InitMover( ent );

	VectorCopy( ent->s.origin, ent->s.pos.trBase );
	VectorCopy( ent->s.origin, ent->r.currentOrigin );

	ent->s.pos.trDuration = ent->speed * 1000;
	ent->s.pos.trTime = ent->s.pos.trDuration * phase;
	ent->s.pos.trType = TR_SINE;

	// set the axis of bobbing
	if ( ent->spawnflags & 1 ) {
		ent->s.pos.trDelta[0] = height;
	} else if ( ent->spawnflags & 2 ) {
		ent->s.pos.trDelta[1] = height;
	} else {
		ent->s.pos.trDelta[2] = height;
	}

	level.numBrushEnts++;
}

/*
===============================================================================

PENDULUM

===============================================================================
*/


/*QUAKED func_pendulum (0 .5 .8) ?
-----DESCRIPTION-----
You need to have an origin brush as part of this entity.
Pendulums always swing north / south on unrotated models.  Add an angles field to the model to allow rotation in other directions.
Pendulum frequency is a physical constant based on the length of the beam and gravity.

-----SPAWNFLAGS-----
none

-----KEYS-----
"model2" - .md3 model to also draw
"speed" - the number of degrees each way the pendulum swings, (30 default)
"phase" - the 0.0 to 1.0 offset in the cycle to start at
"dmg" - damage to inflict when blocked (2 default)
"color" - constantLight color
"light" - constantLight radius

q3map2:
"_clone" - _clonename of entity to clone brushes from. Note: this entity still needs at least one brush which gets replaced.
"_clonename" - see _clone
"_castShadows" OR "_cs" - sets whether the entity casts shadows
"_receiveShadows" OR "_rs" - sets whether the entity receives shadows
*/
void SP_func_pendulum(gentity_t *ent) {
	float		freq;
	float		length;
	float		phase;
	float		speed;

	G_SpawnFloat( "speed", "30", &speed );
	G_SpawnInt( "dmg", "2", &ent->damage );
	G_SpawnFloat( "phase", "0", &phase );

	trap_SetBrushModel( ent, ent->model );

	// find pendulum length
	length = fabs( ent->r.mins[2] );
	if ( length < 8 ) {
		length = 8;
	}

	freq = 1 / ( M_PI * 2 ) * sqrt( g_gravity.value / ( 3 * length ) );

	ent->s.pos.trDuration = ( 1000 / freq );

	InitMover( ent );

	VectorCopy( ent->s.origin, ent->s.pos.trBase );
	VectorCopy( ent->s.origin, ent->r.currentOrigin );

	VectorCopy( ent->s.angles, ent->s.apos.trBase );

	ent->s.apos.trDuration = 1000 / freq;
	ent->s.apos.trTime = ent->s.apos.trDuration * phase;
	ent->s.apos.trType = TR_SINE;
	ent->s.apos.trDelta[2] = speed;

	level.numBrushEnts++;
}

/*
===============================================================================

LIGHTING CHANGE --- sort of :D

===============================================================================
*/

/*QUAKED func_brushmodel (0 .5 .8) ?
-----DESCRIPTION-----
A brushmodel. 
For use with func_lightchange.
Must have an origin brush.

-----SPAWNFLAGS-----
none

-----KEYS-----
"targetname" - have this be the target of a func_lightchange

q3map2:
"_clone" - _clonename of entity to clone brushes from. Note: this entity still needs at least one brush which gets replaced.
"_clonename" - see _clone
"_castShadows" OR "_cs" - sets whether the entity casts shadows
"_receiveShadows" OR "_rs" - sets whether the entity receives shadows
*/
void SP_func_brushmodel(gentity_t *ent) {
	trap_SetBrushModel(ent, ent->model);
	ent->s.eType = ET_MOVER;
	ent->s.pos.trType = TR_STATIONARY;
	trap_LinkEntity(ent);

	level.numBrushEnts++;
}

/*QUAKED func_lightchange (0 .5 .8) ?
-----DESCRIPTION-----
Can be used for "toggling" light on/off.
Must target a func_brushmodel.
Must have an origin brush.

-----SPAWNFLAGS-----
none

-----KEYS-----
"target" - func_brushmodel to swap positions with

q3map2:
"_clone" - _clonename of entity to clone brushes from. Note: this entity still needs at least one brush which gets replaced.
"_clonename" - see _clone
"_castShadows" OR "_cs" - sets whether the entity casts shadows
"_receiveShadows" OR "_rs" - sets whether the entity receives shadows
*/
void func_lightchange_use(gentity_t *ent, gentity_t *other, gentity_t *activator) {
	char *oldModel;

	ent->activator = activator;
	oldModel = ent->model;
	ent->model =  ent->model2;
	ent->model2 = oldModel;
	trap_SetBrushModel(ent, ent->model);
}

void func_lightchange_setup(gentity_t *ent) {
	gentity_t *bmodel;
	bmodel = G_Find(NULL, FOFS(targetname), ent->target);
	if(!bmodel) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] Could not find target %s for func_lightchange at %s!\n", ent->target, vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}
	if(Q_stricmp(bmodel->classname, "func_brushmodel")) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] func_lightchange with invalid target entity of class %s at %s!\n", bmodel->classname, vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}
	G_SetOrigin(bmodel, ent->s.origin);
	bmodel->r.svFlags = SVF_NOCLIENT;
	bmodel->s.eFlags = EF_NODRAW;
	trap_LinkEntity(bmodel);
	ent->model2 = bmodel->model;
	G_FreeEntity(bmodel);
	trap_LinkEntity(ent);
	ent->think = 0;
	ent->nextthink = -1;
	ent->use = func_lightchange_use;
}

void SP_func_lightchange(gentity_t *ent) {
	if(!ent->target) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] func_lightchange without target at %s!\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}
	
	trap_SetBrushModel(ent, ent->model);

	ent->s.eType = ET_MOVER;
	ent->s.pos.trType = TR_STATIONARY;

	trap_LinkEntity(ent);

	ent->think = func_lightchange_setup;
	ent->nextthink = level.time + FRAMETIME;

	level.numBrushEnts++;
}

/*
===============================================================================

TARGET MOVER

===============================================================================
*/

/*QUAKED func_targetmover (0 .5 .8) ? START_OPEN
-----DESCRIPTION-----
This work similar to an func_door but will move between the entities origin (requires origin brush) and an target origin.
Added for enhanced SP level support, that's why it is a quite basic entity.

-----SPAWNFLAGS-----
1: START_OPEN - will spawn at target position

-----KEYS-----
"target" - info_notnull, where to move
"wait" - time beforce returning, -1 = toggle
"speed" - speed to move with (default: 200)

q3map2:
"_clone" - _clonename of entity to clone brushes from. Note: this entity still needs at least one brush which gets replaced.
"_clonename" - see _clone
"_castShadows" OR "_cs" - sets whether the entity casts shadows
"_receiveShadows" OR "_rs" - sets whether the entity receives shadows
*/
void func_targetmover_link(gentity_t *ent) {
	gentity_t *target;

	target = G_Find(NULL, FOFS(targetname), ent->target);
	if(!target) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] func_targetmover with unfound target %s at %s!\n", ent->target, vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}

	VectorCopy(target->s.origin, ent->pos2);

	// if "start_open", reverse position 1 and 2
	if ( ent->spawnflags & 1 ) {
		vec3_t	temp;

		VectorCopy( ent->pos2, temp );
		VectorCopy( ent->s.origin, ent->pos2 );
		VectorCopy( temp, ent->pos1 );
	}

	InitMover( ent );

	ent->nextthink = level.time + FRAMETIME;

	if ( ! (ent->flags & FL_TEAMSLAVE ) ) {
		int health;

		G_SpawnInt( "health", "0", &health );
		if ( health ) {
			ent->takedamage = qtrue;
		}

		if ( ent->targetname || health  ) {
			// non touch/shoot doors
			ent->think = Think_MatchTeam;
		} else {
			ent->think = Think_SpawnNewDoorTrigger;
		}
	}
}

void SP_func_targetmover(gentity_t *ent) {
	char	*sound;

	if(!ent->target) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] func_targetmover without target at %s!\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}

	G_SpawnString("soundstart", "sound/movers/doors/largedoorstart.wav", &sound);
	ent->sound1to2 = ent->sound2to1 = G_SoundIndex(sound);
	G_SpawnString("soundstop", "sound/movers/doors/largedoorstop.wav", &sound);
	ent->soundPos1 = ent->soundPos2 = G_SoundIndex(sound);
	G_SpawnString("soundlocked", "sound/ambience/voyager/doorchime.mp3", &sound);
	ent->n00bCount = G_SoundIndex(sound);

	ent->blocked = Blocked_Door;

	// default speed of 400
	if (!ent->speed)
		ent->speed = 200;

	// default wait of 2 seconds
	if (!ent->wait)
		ent->wait = 2;
	ent->wait *= 1000;

	// first position at start
	VectorCopy( ent->s.origin, ent->pos1 );

	// calculate second position
	trap_SetBrushModel( ent, ent->model );
	
	ent->think = func_targetmover_link;
	ent->nextthink = level.time + 500;

	level.numBrushEnts++;
}

/*
===============================================================================

ADVANCED MOVER

===============================================================================
*/

void Move_AdvancedMover(gentity_t *ent) {
	G_Mover_UseBinaryMover(ent, NULL, ent->activator);
}


void Reached_AdvancedMover(gentity_t *ent) {
	gentity_t *temp, *old;
	vec3_t t, move;
	float distance;
	qboolean bypass = qfalse;
	gentity_t *touched = ent->touched;

	#ifdef G_LUA
	if(ent->luaTrigger)
	{
		LuaHook_G_EntityTrigger(ent->luaTrigger, ent->s.number, -1);
	}
	#endif

	ent->s.loopSound = ent->soundLoop;

	G_Printf(S_COLOR_RED "DEBUG: ent->apos1: %s, ent->apos2: %s\n", vtos(ent->apos1), vtos(ent->apos2));

	if(ent->moverState == MOVER_1TO2) {
		SetMoverState(ent, MOVER_POS2, level.time);

		if(ent->soundPos2)
			G_AddEvent(ent, EV_GENERAL_SOUND, ent->soundPos2);

		if(!touched->target)
			ent->damage = 0;
		if(!touched->targetname2 && !(ent->spawnflags & 1))
			ent->damage = 1;
		if((!touched->target && touched->angle) || (!touched->targetname2 && !(ent->spawnflags & 1) && touched->angle))
			bypass = qtrue;

		if( touched->wait < 0) {
			ent->nextthink = -1; // wait here until used again
			ent->use = G_Mover_UseBinaryMover;
			if(ent->damage) {
				temp = G_Find(NULL, FOFS(targetname), touched->target);
				if(!temp) {
					G_FreeEntity(ent);
					return;
				}
				VectorCopy(touched->s.origin, ent->pos1);
				VectorCopy(temp->s.origin, ent->pos2);
				old = touched;
				touched = temp;
				ent->moverState = MOVER_POS1;
				// if new target has angles copy them for angular movement
				if(touched->angle && !bypass) {
					VectorCopy(ent->apos2, ent->apos1);
					VectorCopy(touched->s.angles, ent->apos2);
				} else if(old->angle && bypass){
					VectorCopy(ent->apos2, ent->apos1);
					VectorCopy(old->s.angles, ent->apos2);
				}
			} else {
				temp = G_Find(NULL, FOFS(targetname), touched->targetname2);
				if(!temp) {
					G_FreeEntity(ent);
					return;
				}
				VectorCopy(touched->s.origin, ent->pos2);
				VectorCopy(temp->s.origin, ent->pos1);
				old = touched;
				touched = temp;
				ent->moverState = MOVER_POS1;
				if(touched->angle && !bypass) {
					VectorCopy(ent->apos2, ent->apos1);
					VectorCopy(touched->s.angles, ent->apos2);
				} else if(old->angle && bypass){
					VectorCopy(ent->apos2, ent->apos1);
					VectorCopy(old->s.angles, ent->apos2);
				}
			}
		} else {
			ent->think = Move_AdvancedMover;
			ent->nextthink = level.time + ent->wait;
			if(ent->damage) {
				temp = G_Find(NULL, FOFS(targetname), touched->target);
				if(!temp) {
					G_FreeEntity(ent);
					return;
				}
				VectorCopy(touched->s.origin, ent->pos1);
				VectorCopy(temp->s.origin, ent->pos2);
				old = touched;
				touched = temp;
				ent->moverState = MOVER_POS1;
				if(touched->angle && !bypass) {
					VectorCopy(ent->apos2, ent->apos1);
					VectorCopy(touched->s.angles, ent->apos2);
				} else if(old->angle && bypass){
					VectorCopy(ent->apos2, ent->apos1);
					VectorCopy(old->s.angles, ent->apos2);
				}
			} else {
				temp = G_Find(NULL, FOFS(targetname), touched->targetname2);
				if(!temp) {
					G_FreeEntity(ent);
					return;
				}
				VectorCopy(touched->s.origin, ent->pos2);
				VectorCopy(temp->s.origin, ent->pos1);
				old = touched;
				touched = temp;
				ent->moverState = MOVER_POS1;
				if(touched->angle && !bypass) {
					VectorCopy(ent->apos2, ent->apos1);
					VectorCopy(touched->s.angles, ent->apos2);
				} else if(old->angle && bypass){
					VectorCopy(ent->apos2, ent->apos1);
					VectorCopy(old->s.angles, ent->apos2);
				}
			}
		}

		if(!ent->damage) {
			VectorCopy(ent->pos1, t);
			VectorCopy(ent->pos2, ent->pos1);
			VectorCopy(t, ent->pos2);
			VectorCopy(ent->apos1, t);
			VectorCopy(ent->apos2, ent->apos1);
			VectorCopy(t, ent->apos2);
		}

		// recalc speed if path_point has speed
		if(touched->speed) {
			// calculate time to reach second position from speed
			VectorSubtract( ent->pos2, ent->pos1, move );
			distance = VectorLength( move );
			VectorScale( move, touched->speed, ent->s.pos.trDelta );
			ent->s.pos.trDuration = distance * 1000 / touched->speed;
			if ( ent->s.pos.trDuration <= 0 ) {
				ent->s.pos.trDuration = 1;
			}
			if(touched->angle)
				ent->s.apos.trDuration = ent->s.pos.trDuration;
		}

		if(!ent->activator)
			ent->activator = ent;
		// To do: only use targets it path_point has a specific value set?
		G_UseTargets(ent, ent->activator);
	}
}

/*QUAKED path_point (.5 .3 0) (-8 8 8) (8 8 8) START_POINT
-----DESCRIPTION-----
Target position for the discontinued func_mover

-----SPAWNFLAGS-----
1: START_POINT - this is the first path_point for the train

-----KEYS-----
"target" - next path_point
"wait" - time beforce moving on, -1 wait until used
"damage" - used to tell the func_mover it should fire it's targets here
"angles" - to rotate to
*/
void SP_path_point(gentity_t *ent) {
	// check if angles are set
	if(ent->angle)
		ent->angle = 0;
	if(ent->s.angles[0] || ent->s.angles[1] || ent->s.angles[2])
		ent->angle = 1;
}

/*QUAKED func_mover (0 .5 .8) ?
-----DESCRIPTION-----
Discontinued enhanced mover requiring an origin brush.
Use Lua for this now.

-----SPAWNFLAGS-----
none

-----KEYS-----
"target" - path_point to start at
"speed" - speed to move with (default: 10)
"aspeed" - angular speed to rotate with (default: 10)

q3map2:
"_clone" - _clonename of entity to clone brushes from. Note: this entity still needs at least one brush which gets replaced.
"_clonename" - see _clone
"_castShadows" OR "_cs" - sets whether the entity casts shadows
"_receiveShadows" OR "_rs" - sets whether the entity receives shadows
*/

void SP_func_mover(gentity_t *ent) {
	gentity_t  *target;
	float aspeed;
	
	if(!ent->target) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] func_mover without target at %s!\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}
	
	target = G_Find(NULL, FOFS(targetname), ent->target);
	ent->touched = target;

	ent->damage = 1;
	
	VectorCopy(ent->s.origin, ent->pos1);
	VectorCopy(target->s.origin, ent->pos2);
	VectorCopy(ent->s.angles, ent->apos1);
	VectorCopy(target->s.angles, ent->apos2);

	if(!ent->speed)
		ent->speed = 100;
	G_SpawnFloat("aspeed", "100", &aspeed);
	ent->count = aspeed;

	trap_SetBrushModel(ent, ent->model);
	InitMover(ent);

	level.numBrushEnts++;
}

/*
-------------------------------------------

func_stasis_door rewrite
by Ubergames Harry Young
based upon SP-code

-------------------------------------------
*/

/*
Presets
ent->count state of the door where
1 = closing
2 = closed
3 = opening
4 = opened

ent->n00bCount locked indicator
1 = locked
2 = unlocked
*/
#define STASIS_DOOR_CLOSING_PHASE1 1
#define STASIS_DOOR_CLOSING_PHASE2 2
#define STASIS_DOOR_CLOSED	3
#define STASIS_DOOR_OPENING_PHASE1 4
#define STASIS_DOOR_OPENING_PHASE2 5
#define STASIS_DOOR_OPENED	6


/*
-------------------------------------------

toggle_stasis_door 
will manage the door-toggeling

-------------------------------------------
*/
void touch_stasis_door( gentity_t *ent, gentity_t *other, trace_t *trace );

void toggle_stasis_door( gentity_t *ent )
{
	gentity_t *parent = ent->parent;
	ent->nextthink = -1; // prevent thinking again until this think is finished
	switch(parent->count) {
		case STASIS_DOOR_CLOSED: // then go to opening state
			G_AddEvent(parent, EV_STASIS_DOOR_OPENING, 0); // send event to client
			parent->count = STASIS_DOOR_OPENING_PHASE1;
			ent->nextthink = level.time + FRAMETIME;
			break;
		case STASIS_DOOR_OPENING_PHASE1:
			parent->r.svFlags |= SVF_NOCLIENT;
			trap_AdjustAreaPortalState(parent, qtrue); // open AP
			parent->s.eFlags |= EF_NODRAW;
			parent->count = STASIS_DOOR_OPENING_PHASE2;
			ent->nextthink = level.time + 1000;
			trap_LinkEntity(parent);
			break;
		case STASIS_DOOR_CLOSING_PHASE2: // then go to closed state
			trap_SetBrushModel(parent, parent->model);
			InitMover( parent );
			VectorCopy( parent->s.origin, parent->s.pos.trBase );
			VectorCopy( parent->s.origin, parent->r.currentOrigin );
			parent->r.contents &= ~SVF_NOCLIENT;
			parent->s.eFlags &= ~EF_NODRAW;
			trap_AdjustAreaPortalState(parent, qfalse); // close AP
			trap_LinkEntity(parent);
			parent->count = STASIS_DOOR_CLOSED;
			ent->touch = touch_stasis_door;
			break;
		case STASIS_DOOR_OPENED: // then go to closing 
			parent->r.contents &= ~SVF_NOCLIENT;
			trap_LinkEntity(parent);
			parent->count = STASIS_DOOR_CLOSING_PHASE1;
			ent->nextthink = level.time + FRAMETIME;
			break;
		case STASIS_DOOR_CLOSING_PHASE1:
			G_AddEvent(ent, EV_STASIS_DOOR_CLOSING, parent-g_entities); // send event to client
			parent->r.contents = CONTENTS_SOLID;
			parent->count = STASIS_DOOR_CLOSING_PHASE2;
			ent->nextthink = level.time + 1000;
			trap_LinkEntity(parent);
			break;
		case STASIS_DOOR_OPENING_PHASE2: // then go to opened state
			parent->r.contents = CONTENTS_NONE;
			trap_LinkEntity(parent);
			parent->count = STASIS_DOOR_OPENED;
			ent->nextthink = level.time + 3000;
			break;
	}
}
/*
-------------------------------------------

use_stasis_door 
will be called when the entity is used

-------------------------------------------
*/

void use_stasis_door(gentity_t *ent, gentity_t *other, gentity_t *activator)
{
	return;
	if(!Q_stricmp(activator->target, ent->targetname))
	{
		ent->think = toggle_stasis_door;
		ent->nextthink = level.time + 50;
	}
	else if(!Q_stricmp(activator->target, ent->swapname))
	{
		ent->nextthink = level.time + 100;
	}
}
/*
-------------------------------------------

touch_stasis_door 
triggers the door on touch

-------------------------------------------
*/

void touch_stasis_door( gentity_t *ent, gentity_t *other, trace_t *trace )
{
	// The door is solid so it's ok to open it, otherwise,
	//	the door is already open and we don't need to bother with the state change
	if (ent->parent->count == STASIS_DOOR_CLOSED)
	{
		ent->touch = 0;
		ent->think = toggle_stasis_door;
		ent->nextthink	 = level.time + FRAMETIME;
	}
}

/*
-------------------------------------------

spawn_trigger_stasis_door 
spawns the door-trigger

-------------------------------------------
*/

void spawn_trigger_stasis_door( gentity_t *ent ) {
	gentity_t		*other;
	vec3_t		mins, maxs;
	int			i, best;

	if(!ent) return;

	// set all of the slaves as shootable
	for ( other = ent ; other ; other = other->teamchain ) {
		other->takedamage = qtrue;
	}

	// find the bounds of everything on the team
	VectorCopy (ent->r.absmin, mins);
	VectorCopy (ent->r.absmax, maxs);

	// Copy maxs and mins to s.origin2 and s.angles2 for scanable door
	VectorCopy(maxs, ent->s.origin2);
	VectorCopy(mins, ent->s.angles2);

	// find the thinnest axis, which will be the one we expand
	best = 0;
	for ( i = 1 ; i < 3 ; i++ ) {
		if ( maxs[i] - mins[i] < maxs[best] - mins[best] ) {
			best = i;
		}
	}

	maxs[best] += 128;
	mins[best] -= 128;

	// create a trigger with this size
	other = G_Spawn ();
	G_SetOrigin(other, ent->s.origin);
	VectorCopy (mins, other->r.mins);
	VectorCopy (maxs, other->r.maxs);
	other->parent = ent;
	other->r.contents = CONTENTS_TRIGGER;
	other->touch = touch_stasis_door;

	trap_LinkEntity (other);

	ent->count = STASIS_DOOR_CLOSED;
}

//-------------------------------------------
/*QUAKED func_stasis_door (0 .5 .8) START_LOCKED
-----DESCRIPTION-----
A bmodel that just sits there and opens when a player gets close to it.

-----SPAWNFLAGS-----
1: START_LOCKED - door is locked at spawn

-----KEYS-----
"targetname" - will open the door
"swapname" - will lock the door (SELF/NO_ACTIVATOR needed)
"wait" - time to wait before closing, -1 for manual trigger, default is 5 seconds
*/
void SP_func_stasis_door( gentity_t *ent ) 
{
	/* set the brush model */
	trap_SetBrushModel( ent, ent->model );

	/* set the orgin */
	G_SetOrigin(ent, ent->s.origin);

	VectorCopy(ent->s.origin, ent->pos1);

	InitMover(ent);

	// Auto create a door trigger so the designers don't have to
	ent->think = spawn_trigger_stasis_door;
	ent->nextthink = level.time + 500; // give the target a chance to spawn in

	ent->use = use_stasis_door;
	ent->count = -1;
	if (!ent->wait)
	{
		ent->wait = 5;
	}

	if (ent->spawnflags & 1)
	{
		ent->n00bCount = 1;
		//show darker model
	}

	// copy mins and max for client side model
	VectorCopy(ent->r.maxs, ent->s.origin2);
	VectorCopy(ent->r.mins, ent->s.angles2);

	trap_LinkEntity (ent);
}
