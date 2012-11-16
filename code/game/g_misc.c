// Copyright (C) 1999-2000 Id Software, Inc.
//
// g_misc.c

#include "g_local.h"


/*QUAKED func_group (0 0 0) ?
Used to group brushes together just for editor convenience.  They are turned into normal brushes by the utilities.
*/


/*QUAKED info_camp (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for calculations in the utilities (spotlights, etc), but removed during gameplay.
*/
void SP_info_camp( gentity_t *self ) {
	G_SetOrigin( self, self->s.origin );
}


/*QUAKED info_null (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for calculations in the utilities (spotlights, etc), but removed during gameplay.
*/
void SP_info_null( gentity_t *self ) {
	G_FreeEntity( self );
}


/*QUAKED info_notnull (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for in-game calculation, like jumppad targets.
target_position does the same thing
*/
void SP_info_notnull( gentity_t *self ){
	if(!Q_stricmp(self->classname, "ref_tag") && !rpg_allowspmaps.integer)
		G_FreeEntity(self);
	G_SetOrigin( self, self->s.origin );
}


/*QUAKED light (0 1 0) (-8 -8 -8) (8 8 8) linear noIncidence
Non-displayed light.
"light" overrides the default 300 intensity.
'Linear' checkbox gives linear falloff instead of inverse square
'noIncidence' checkbox makes lighting smoother
Lights pointed at a target will be spotlights.
"radius" overrides the default 64 unit radius of a spotlight at the target point.
*/
void SP_light( gentity_t *self ) {
	G_FreeEntity( self );
}



/*
=================================================================================

TELEPORTERS

=================================================================================
*/

void TransportPlayer( gentity_t *player, vec3_t origin, vec3_t angles, int speed )
{
	gentity_t	*tent = NULL;
	playerState_t *ps = &player->client->ps;
	clientSession_t *sess = &player->client->sess;

	// use temp events at source and destination to prevent the effect
	// from getting dropped by a second player event
	if ( sess->sessionTeam != TEAM_SPECTATOR /*&& !(ps->eFlags&EF_ELIMINATED)*/ ) {
		vec3_t	org;

		VectorCopy( ps->origin, org );
		org[2] += (ps->viewheight >> 1);

		tent = G_TempEntity( ps->origin, EV_PLAYER_TELEPORT_OUT );
		tent->s.clientNum = player->s.clientNum;

		tent = G_TempEntity( origin, EV_PLAYER_TELEPORT_IN );
		tent->s.clientNum = player->s.clientNum;
	}

	// unlink to make sure it can't possibly interfere with G_KillBox
	trap_UnlinkEntity (player);

	VectorCopy ( origin, ps->origin );
	ps->origin[2] += 1;

	// spit the player out
	AngleVectors( angles, ps->velocity, NULL, NULL );
	VectorScale( ps->velocity, speed, ps->velocity );
	ps->pm_time = 160;		// hold time
	ps->pm_flags |= PMF_TIME_KNOCKBACK;

	// toggle the teleport bit so the client knows to not lerp
	ps->eFlags ^= EF_TELEPORT_BIT;

	// set angles
	G_Client_SetViewAngle( player, angles );

	// kill anything at the destination
	if ( sess->sessionTeam != TEAM_SPECTATOR /*&& !(ps->eFlags&EF_ELIMINATED)*/) {
		G_KillBox (player);
	}

	// save results of pmove
	BG_PlayerStateToEntityState( &player->client->ps, &player->s, qtrue );

	// use the precise origin for linking
	VectorCopy( ps->origin, player->r.currentOrigin );

	if ( sess->sessionTeam != TEAM_SPECTATOR /*&& !(ps->eFlags&EF_ELIMINATED)*/) {
		trap_LinkEntity (player);
	}
}


void TeleportPlayer( gentity_t *player, vec3_t origin, vec3_t angles, tpType_t tpType ) {
	gentity_t	*tent;
	playerState_t *ps = &player->client->ps;
	clientSession_t *sess = &player->client->sess;

	// unlink to make sure it can't possibly interfere with G_KillBox
	trap_UnlinkEntity (player);

	VectorCopy ( origin, ps->origin );

	// use temp events at source and destination to prevent the effect
	// from getting dropped by a second player event
	if ( sess->sessionTeam != TEAM_SPECTATOR ) 
	{
		if ( tpType == TP_BORG )
		{
			// ...we are borg...prepare to be...
			tent = G_TempEntity( origin, EV_BORG_TELEPORT );
			tent->s.clientNum = player->s.clientNum;
		}
		//RPG-X: J2J Added to get fed trans effect without any traveling after beam in
		//TiM: Since the SP teleporter has been coded to only work with Jay's modification,
		//we'll add the second half of the client-side effects (ie materialization) here
		else if( tpType == TP_TRI_TP )
		{
			// probably isn't necessary, but just in case, end the beam out powerup
			ps->powerups[PW_BEAM_OUT] = 0;
			//and add the beam in one
			ps->powerups[PW_QUAD] = level.time + 4000;

			tent = G_TempEntity( ps->origin, EV_PLAYER_TRANSPORT_IN );
			tent->s.clientNum = player->s.clientNum;			
		}
	}

	// spit the player out
	//TiM - If in a turbolift and moving, get their velocity, perform the rotation
	//calc on it, and then reset it to their velocity.
	if ( tpType == TP_TURBO )
	{
		vec3_t	dir;
		vec3_t	velAngles;
		float length;

		VectorCopy( ps->velocity, dir );
		length = VectorLength( dir );
		VectorNormalize( dir );
		vectoangles( dir, velAngles );

		velAngles[YAW] = AngleNormalize360( velAngles[YAW]+ (angles[YAW] - ps->viewangles[YAW] ) );
		AngleVectors( velAngles, dir, NULL, NULL );

		VectorScale( dir, length, ps->velocity);
	}
	else {
		//TiM: Set the velocity to 0.  So if they were moving b4 the transport, they'll be stopped when they come out.
		//It's a little something called the Heisenberg compensators. ;)
		
		//bug-fix. if the velocity is killed in a spectator door teleporter, the player
		//gets wedged in the door. >_<
		if ( sess->sessionTeam != TEAM_SPECTATOR )
			VectorScale( ps->velocity, 0, ps->velocity );
		else
		{
			AngleVectors( angles, ps->velocity, NULL, NULL );
			VectorScale( ps->velocity, 400, ps->velocity );
			ps->pm_time = 160;		// hold time
			ps->pm_flags |= PMF_TIME_KNOCKBACK;
		}

		{//see if we can move the player up one
			vec3_t	newOrg;
			trace_t	tr;

			VectorCopy ( origin, newOrg );
			newOrg[2] += 1;
			trap_Trace( &tr, ps->origin, player->r.mins, player->r.maxs, newOrg, ps->clientNum, player->clipmask );
			if ( !tr.allsolid && !tr.startsolid && tr.fraction == 1.0 )
			{
				ps->origin[2] += 1;
			}
		}

	}

	// toggle the teleport bit so the client knows to not lerp
	ps->eFlags ^= EF_TELEPORT_BIT;

	// set angles
	G_Client_SetViewAngle( player, angles );

	// kill anything at the destination
	if ( sess->sessionTeam != TEAM_SPECTATOR ) {
		if ( G_MoveBox (player) )
			player->r.contents = CONTENTS_NONE;
	}

	// save results of pmove
	BG_PlayerStateToEntityState( &player->client->ps, &player->s, qtrue );

	// use the precise origin for linking
	VectorCopy( ps->origin, player->r.currentOrigin );

	if ( sess->sessionTeam != TEAM_SPECTATOR /*&& !(ps->eFlags&EF_ELIMINATED)*/) {
		trap_LinkEntity (player);
	}
}


/*QUAKED misc_teleporter_dest (1 0 0) (-32 -32 -24) (32 32 -16)
Point teleporters at these.
Now that we don't have teleport destination pads, this is just
an info_notnull
*/
void SP_misc_teleporter_dest( gentity_t *ent ) {
}


//===========================================================

/*QUAKED misc_model (1 0 0) (-16 -16 -16) (16 16 16) CAST_SHADOWS CLIP_MODEL FORCE_META 
"model"		arbitrary .md3 file to display

spawnflags only work when compiled with q3map2
*/
void SP_misc_model( gentity_t *ent ) {

#if 0
	ent->s.modelindex = G_ModelIndex( ent->model );
	VectorSet (ent->mins, -16, -16, -16);
	VectorSet (ent->maxs, 16, 16, 16);
	trap_LinkEntity (ent);

	G_SetOrigin( ent, ent->s.origin );
	VectorCopy( ent->s.angles, ent->s.apos.trBase );
#else
	G_FreeEntity( ent );
#endif
}


//===========================================================
static void setCamera ( gentity_t *ent, int ownernum )
{
	vec3_t		dir;
	gentity_t	*target = NULL;
	gentity_t	*owner = NULL;

	ent->r.ownerNum = ownernum;

	owner = &g_entities[ownernum];

	 //frame holds the rotate speed
	ent->s.frame = 0; //TiM: 0

	// clientNum holds the rotate offset
	ent->s.clientNum = owner->s.clientNum;

	VectorCopy( owner->s.origin, ent->s.origin2 );

	// see if the portal_camera has a target
	target = G_PickTarget( owner->target );
	if ( target ) {
		VectorSubtract( target->s.origin, owner->s.origin, dir );
		VectorNormalize( dir );
	} else {
		G_SetMovedir( owner->s.angles, dir );
	}

	ent->s.eventParm = DirToByte( dir );
}

void cycleCamera( gentity_t *self )
{
	gentity_t *orgOwner = NULL;
	gentity_t *owner = NULL;

	if ( self->r.ownerNum >= 0 && self->r.ownerNum < ENTITYNUM_WORLD )
	{
		orgOwner = &g_entities[self->r.ownerNum];
	}

	owner = G_Find( orgOwner, FOFS(targetname), self->target );

	if  ( owner == NULL )
	{
		//Uh oh! Not targeted at any ents!  Or reached end of list?  Which is it?
		//for now assume reached end of list and are cycling
		owner = G_Find( owner, FOFS(targetname), self->target );
		if  ( owner == NULL )
		{//still didn't find one
			DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] Couldn't find target for misc_portal_surface\n" ););
			G_FreeEntity( self );
			return;
		}
	}

	setCamera( self, owner->s.number );

	if ( self->think == cycleCamera )
	{
		if ( owner->wait > 0 )
		{
			self->nextthink = level.time + owner->wait;
		}
		else
		{
			self->nextthink = level.time + self->wait;
		}
	}
}

void misc_portal_use( gentity_t *self, gentity_t *other, gentity_t *activator )
{
	cycleCamera( self );
}

void locateCamera( gentity_t *ent ) {

	gentity_t *owner = NULL;
	owner = G_Find( NULL, FOFS(targetname), ent->target );
	if  ( owner == NULL )
	{
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] Couldn't find target for misc_partal_surface\n" ););
		G_FreeEntity( ent );
		return;
	}
	
	setCamera( ent, owner->s.number );

	if ( G_Find( owner, FOFS(targetname), ent->target) != NULL  )
	{//targeted at more than one thing
		ent->think = cycleCamera;
		if ( owner->wait > 0 )
		{
			ent->nextthink = level.time + owner->wait;
		}
		else
		{
			ent->nextthink = level.time + ent->wait;
		}
	}
}


/*QUAKED misc_portal_surface (0 0 1) (-8 -8 -8) (8 8 8)
The portal surface nearest this entity will show a view from the targeted misc_portal_camera, or a mirror view if untargeted.
This must be within 64 world units of the surface!

targetname - When used, cycles to the next misc_portal_camera it's targeted
wait - makes it auto-cycle between all cameras it's pointed at at intevervals of specified number of seconds.

  cameras will be cycled through in the order they were created on the map.
*/
void SP_misc_portal_surface(gentity_t *ent) {
	VectorClear( ent->r.mins );
	VectorClear( ent->r.maxs );
	trap_LinkEntity (ent);

	ent->r.svFlags = SVF_PORTAL;
	ent->s.eType = ET_PORTAL;
	ent->wait *= 1000;

	if ( !ent->target ) {
		VectorCopy( ent->s.origin, ent->s.origin2 );
	} else {
		ent->think = locateCamera;
		ent->nextthink = level.time + 100;
		if ( ent->targetname )
		{
			ent->use = misc_portal_use;
		}
	}
}

/*QUAKED misc_portal_camera (0 0 1) (-8 -8 -8) (8 8 8) slowrotate fastrotate
The target for a misc_portal_surface.  You can set either angles or target another entity (NOT an info_null) to determine the direction of view.
"roll" an angle modifier to orient the camera around the target vector;
*/
void SP_misc_portal_camera(gentity_t *ent) {
	float	roll;

	VectorClear( ent->r.mins );
	VectorClear( ent->r.maxs );
	trap_LinkEntity( ent );

	G_SpawnFloat( "roll", "0", &roll );

	ent->s.clientNum = roll/360.0 * 256;
	ent->wait *= 1000;
}

/*
======================================================================

  SHOOTERS

======================================================================
*/

void Use_Shooter( gentity_t *ent, gentity_t *other, gentity_t *activator ) {
	vec3_t		dir;
	float		deg;
	vec3_t		up, right;

	// see if we have a target
	if ( ent->enemy ) {
		VectorSubtract( ent->enemy->r.currentOrigin, ent->s.origin, dir );
		VectorNormalize( dir );
	} else {
		VectorCopy( ent->movedir, dir );
	}

	// randomize a bit
	PerpendicularVector( up, dir );
	CrossProduct( up, dir, right );

	deg = crandom() * ent->random;
	VectorMA( dir, deg, up, dir );

	deg = crandom() * ent->random;
	VectorMA( dir, deg, right, dir );

	VectorNormalize( dir );

	switch ( ent->s.weapon ) {
	case WP_8:
		fire_grenade( ent, ent->s.origin, dir );
		break;
	case WP_10:
		fire_rocket( ent, ent->s.origin, dir );
		break;
	case WP_4:
		fire_plasma( ent, ent->s.origin, dir );
		break;
	case WP_9:
		fire_quantum( ent, ent->s.origin, dir );
		break;
	case WP_6:
		fire_comprifle( ent, ent->s.origin, dir );
		break;
	}
}


static void InitShooter_Finish( gentity_t *ent ) {
	ent->enemy = G_PickTarget( ent->target );
	ent->think = 0;
	ent->nextthink = 0;
}

void InitShooter( gentity_t *ent, int weapon ) {
	ent->use = Use_Shooter;
	ent->s.weapon = weapon;

	RegisterItem( BG_FindItemForWeapon( (weapon_t)weapon ) );

	G_SetMovedir( ent->s.angles, ent->movedir );

	if ( !ent->random ) {
		ent->random = 1.0;
	}
	ent->random = sin( M_PI * ent->random / 180 );
	// target might be a moving object, so we can't set movedir for it
	if ( ent->target ) {
		ent->think = InitShooter_Finish;
		ent->nextthink = level.time + 500;
	}
	trap_LinkEntity( ent );
}

/*QUAKED shooter_rocket (1 0 0) (-16 -16 -16) (16 16 16)
Fires at either the target or the current direction.
"random" the number of degrees of deviance from the taget. (1.0 default)
*/
void SP_shooter_rocket( gentity_t *ent ) {
	InitShooter( ent, WP_10 );
}

/*QUAKED shooter_plasma (1 0 0) (-16 -16 -16) (16 16 16)
Fires at either the target or the current direction.
"random" is the number of degrees of deviance from the taget. (1.0 default)
*/
void SP_shooter_plasma( gentity_t *ent ) {
	InitShooter( ent, WP_6 ); //TiM : WP_4
}

/*QUAKED shooter_grenade (1 0 0) (-16 -16 -16) (16 16 16)
Fires at either the target or the current direction.
"random" is the number of degrees of deviance from the taget. (1.0 default)
*/
void SP_shooter_grenade( gentity_t *ent ) {
	InitShooter( ent, WP_8);
}

/*QUAKED shooter_torpedo (1 0 0) (-16 -16 -16) (16 16 16)
Fires at either the target or the current direction.
"random" is the number of degrees of deviance from the taget. (1.0 default)
*/
void SP_shooter_torpedo( gentity_t *ent ) {
	InitShooter( ent, WP_9 );
}
