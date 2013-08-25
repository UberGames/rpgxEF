// Copyright (C) 1999-2000 Id Software, Inc.
//
#include "g_client.h"
#include "g_local.h"
#include "g_groups.h"
#include "g_main.h"
#include "g_cmds.h"
#include "g_main.h"
#include "g_spawn.h"

reconData_t	g_reconData[MAX_RECON_NAMES]; //!< recon data for a limited ammount of clients
int32_t		g_reconNum;

// g_client.c -- client functions that don't happen every frame

void G_Client_StoreClientInitialStatus( gentity_t *ent );

//! players mins
static vec3_t	playerMins = {-12, -12, -24}; //RPG-X : TiM - {-15, -15, -24}
//! players maxs
static vec3_t	playerMaxs = {12, 12, 32}; // {15, 15, 32}

clInitStatus_t clientInitialStatus[MAX_CLIENTS];

//TiM: For easier transport setup
/**
*	Function that makes transport setup easier
*	\author Ubergames - TiM
*/
void G_InitTransport( int32_t clientNum, vec3_t origin, vec3_t angles ) {
	gentity_t* tent = NULL;

	TransDat[clientNum].beamTime = level.time + 8000;
	g_entities[clientNum].client->ps.powerups[PW_BEAM_OUT] = level.time + 8000;

	//Transfer stored data to active beamer
	VectorCopy( origin, TransDat[clientNum].currentCoord.origin );
	VectorCopy( angles,	TransDat[clientNum].currentCoord.angles );

	tent = G_TempEntity( g_entities[clientNum].client->ps.origin, EV_PLAYER_TRANSPORT_OUT );
	tent->s.clientNum = clientNum;
}

/*QUAKED info_player_deathmatch (1 0 1) (-16 -16 -24) (16 16 32) INITIAL
-----DESCRIPTION-----
potential spawning position for deathmatch games.

-----SPAWNFLAGS-----
1: INITIAL - Preferred spawn for the first spawn of a client when entering a match.

-----KEYS-----
"target" - entities with matching targetname will be fired if someone spawns here.
"nobots" - if 1 will prevent bots from using this spot.
"nohumans" - if 1 will prevent non-bots from using this spot.
*/
/**
*	Spawn function for deathmatch spawnpoint
*/
void SP_info_player_deathmatch( gentity_t *ent ) {
	int32_t i = 0;

	ent->type = ENT_INFO_PLAYER_START;

	if(strcmp(ent->classname, "info_player_deathmatch") != 0) {
		ent->classname = "info_player_deathmatch";
	}

	G_SpawnInt( "nobots", "0", &i);
	if ( i != 0 ) {
		ent->flags |= FL_NO_BOTS;
	}
	G_SpawnInt( "nohumans", "0", &i );
	if ( i != 0 ) {
		ent->flags |= FL_NO_HUMANS;
	}

	trap_LinkEntity(ent);
}

/*QUAKED info_player_intermission (1 0 1) (-16 -16 -24) (16 16 32)
-----DESCRIPTION-----
The intermission will be viewed from this point.
It is also used to spawn spectators.
Target an info_notnull or similar for the view direction.

-----SPAWNFLAGS-----
none

-----KEYS-----
none
*/
/**
*	Spawn function for intermission entity.
*/
void SP_info_player_intermission( gentity_t *ent ) { 
	ent->type = ENT_INFO_PLAYER_INTERMISSION;
}

/**
 * Determine whether spot would telefrag.
 *
 * \param spot Spot to check.
 * \return Whether this spot would telefrag.
 */
static qboolean G_Client_SpotWouldTelefrag( gentity_t *spot ) {
	int32_t		i = 0;
	int32_t		num = 0;
	int			touch[MAX_GENTITIES];
	gentity_t*	hit = NULL;
	vec3_t		mins;
	vec3_t		maxs;

	memset(touch, 0, sizeof(touch));

	VectorAdd( spot->s.origin, playerMins, mins );
	VectorAdd( spot->s.origin, playerMaxs, maxs );
	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	for ( ; i<num ; i++) {
		hit = &g_entities[touch[i]];
		if ( hit && hit->client && hit->client->ps.stats[STAT_HEALTH] > 0 ) {
			return qtrue;
		}
		if (hit && hit->s.eType == ET_USEABLE && hit->s.modelindex == HI_SHIELD) {	//hit a portable force field
			return qtrue;
		}


	}

	return qfalse;
}

/*
================
SelectNearestDeathmatchSpawnPoint

Find the spot that we DON'T want to use
================
*/
#define	MAX_SPAWN_POINTS	256
/**
*	Find the spot that we DON'T want to use
*/
static gentity_t* SelectNearestDeathmatchSpawnPoint( vec3_t from ) {
	gentity_t*	spot = NULL;
	vec3_t		delta;
	double		dist = 0.0;
	double		nearestDist = 999999.0;
	gentity_t*	nearestSpot = NULL;

	while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL) {

		VectorSubtract( spot->s.origin, from, delta );
		dist = VectorLength( delta );
		if ( dist < nearestDist ) {
			nearestDist = dist;
			nearestSpot = spot;
		}
	}

	return nearestSpot;
}


/*
================
SelectRandomDeathmatchSpawnPoint

go to a random point that doesn't telefrag
================
*/
#define	MAX_SPAWN_POINTS	256
/**
*	go to a random point that doesn't telefrag
*/
static gentity_t* SelectRandomDeathmatchSpawnPoint( void ) {
	gentity_t*	spot = NULL;
	int32_t		count = 0;
	int32_t		selection = 0;
	gentity_t*	spots[MAX_SPAWN_POINTS];

	memset(spots, 0, sizeof(spots));

	while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL) {
		if ( G_Client_SpotWouldTelefrag( spot ) ) {
			continue;
		}
		spots[ count ] = spot;
		count++;
	}

	if ( count == 0 ) {	// no spots that won't telefrag
		return G_Find( NULL, FOFS(classname), "info_player_deathmatch");
	}

	selection = rand() % count;
	return spots[ selection ];
}


gentity_t* G_Client_SelectSpawnPoint ( vec3_t avoidPoint, vec3_t origin, vec3_t angles ) {
	gentity_t* spot = NULL;
	gentity_t* nearestSpot = SelectNearestDeathmatchSpawnPoint( avoidPoint );

	spot = SelectRandomDeathmatchSpawnPoint ( );
	if ( spot == nearestSpot ) {
		// roll again if it would be real close to point of death
		spot = SelectRandomDeathmatchSpawnPoint ( );
		if ( spot == nearestSpot ) {
			// last try
			spot = SelectRandomDeathmatchSpawnPoint ( );
		}
	}

	// find a single player start spot
	if (spot == NULL) {
		G_Error( "Couldn't find a spawn point" );
		return spot;
	}

	VectorCopy (spot->s.origin, origin);
	origin[2] += 9;
	VectorCopy (spot->s.angles, angles);

	return spot;
}

/*
===========
SelectInitialSpawnPoint

Try to find a spawn point marked 'initial', otherwise
use normal spawn selection.
============
*/
/**
*	Try to find a spawn point marked 'initial', otherwise
*	use normal spawn selection.
*/
static gentity_t* SelectInitialSpawnPoint( vec3_t origin, vec3_t angles ) {
	gentity_t* spot = NULL;

	spot = NULL;
	while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL) {
		if ( spot->spawnflags & 1 ) {
			break;
		}
	}

	if ( spot == NULL || G_Client_SpotWouldTelefrag( spot ) ) {
		return G_Client_SelectSpawnPoint( vec3_origin, origin, angles );
	}

	VectorCopy (spot->s.origin, origin);
	origin[2] += 9;
	VectorCopy (spot->s.angles, angles);

	return spot;
}

/*
===========
SelectSpectatorSpawnPoint

============
*/
static gentity_t* SelectSpectatorSpawnPoint( vec3_t origin, vec3_t angles ) {
	FindIntermissionPoint();

	VectorCopy( level.intermission_origin, origin );
	VectorCopy( level.intermission_angle, angles );

	return NULL;
}

/*
=======================================================================

BODYQUE

=======================================================================
*/

static int32_t	bodyFadeSound=0;


/*
===============
G_Client_InitBodyQue
===============
*/
void G_Client_InitBodyQue (void) {
	int32_t		i = 0;
	gentity_t*	ent = NULL;

	level.bodyQueIndex = 0;
	for ( ; i<BODY_QUEUE_SIZE ; i++) {
		ent = G_Spawn();

		if(ent == NULL) {
			// TODO print error?
			return;
		}
		
		ent->classname = "bodyque";
		ent->neverFree = qtrue;
		level.bodyQue[i] = ent;
	}

	if (bodyFadeSound == 0)
	{	// Initialize this sound.
		bodyFadeSound = G_SoundIndex("sound/enemies/borg/walkthroughfield.wav");
	}
}

/*
=============
BodyRezOut

After sitting around for five seconds, fade out.
=============
*/
/**
*	After sitting around for five seconds, fade out.
*/
static void BodyRezOut( gentity_t *ent ) {
	if ( level.time - ent->timestamp >= 7500 ) {
		// the body ques are never actually freed, they are just unlinked
		trap_UnlinkEntity( ent );
		ent->physicsObject = qfalse;
		return;
	}

	ent->nextthink = level.time + 2500;
	ent->s.time = level.time + 2500;

	G_AddEvent(ent, EV_GENERAL_SOUND, bodyFadeSound);
}

/*
=============
CopyToBodyQue

A player is respawning, so make an entity that looks
just like the existing corpse to leave behind.
=============
*/
/**
*	A player is respawning, so make an entity that looks
*	just like the existing corpse to leave behind.
*/
static void CopyToBodyQue( gentity_t *ent ) {
	gentity_t*		body = NULL;
	int32_t			contents = 0;
	entityState_t*	eState = NULL;

	trap_UnlinkEntity (ent);

	// if client is in a nodrop area, don't leave the body
	contents = trap_PointContents( ent->s.origin, -1 );
	if ( (contents & CONTENTS_NODROP) != 0 ) {
		ent->s.eFlags &= ~EF_NODRAW;	// Just in case we died from a bottomless pit, reset EF_NODRAW
		return;
	}

	// grab a body que and cycle to the next one
	body = level.bodyQue[ level.bodyQueIndex ];
	level.bodyQueIndex = (level.bodyQueIndex + 1) % BODY_QUEUE_SIZE;

	trap_UnlinkEntity (body);

	eState = &ent->s;
	eState->eFlags = EF_DEAD;		// clear EF_TALK, etc
	eState->powerups = 0;	// clear powerups
	eState->loopSound = 0;	// clear lava burning
	eState->number = body - g_entities;
	body->timestamp = level.time;
	body->physicsObject = qtrue;
	body->physicsBounce = 0;		// don't bounce
	if ( eState->groundEntityNum == ENTITYNUM_NONE ) {
		eState->pos.trType = TR_GRAVITY;
		eState->pos.trTime = level.time;
		VectorCopy( ent->client->ps.velocity, eState->pos.trDelta );
	} else {
		eState->pos.trType = TR_STATIONARY;
	}
	eState->event = 0;

	// change the animation to the last-frame only, so the sequence
	// doesn't repeat anew for the body
	switch ( eState->legsAnim & ~ANIM_TOGGLEBIT ) {
	case BOTH_DEATH1:
	case BOTH_DEAD1:
		eState->torsoAnim = eState->legsAnim = BOTH_DEAD1;
		break;
	case BOTH_DEATH2:
	case BOTH_DEAD2:
		eState->torsoAnim = eState->legsAnim = BOTH_DEAD2;
		break;
	default:
		eState->torsoAnim = eState->legsAnim = BOTH_DEAD1; //DEAD3
		break;
	}

	body->r.svFlags = ent->r.svFlags;
	VectorCopy (ent->r.mins, body->r.mins);
	VectorCopy (ent->r.maxs, body->r.maxs);
	VectorCopy (ent->r.absmin, body->r.absmin);
	VectorCopy (ent->r.absmax, body->r.absmax);

	body->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	body->r.contents = CONTENTS_CORPSE;
	body->r.ownerNum = ent->r.ownerNum;

	body->nextthink = level.time + 5000;
	body->think = BodyRezOut;

	body->die = body_die;

	// if there shouldn't be a body, don't show one.
	if (ent->client != NULL &&
			((level.time - ent->client->ps.powerups[PW_DISINTEGRATE]) < 10000 ||
			(level.time - ent->client->ps.powerups[PW_EXPLODE]) < 10000))
	{
		eState->eFlags |= EF_NODRAW;
	}

	// don't take more damage if already gibbed
	//RPG-X: RedTechie - Check for medicrevive
	if(rpg_medicsrevive.integer == 0){
		if ( ent->health <= GIB_HEALTH ) {
			body->takedamage = qfalse;
		} else {
			body->takedamage = qtrue;
		}
	} else {
		body->takedamage = qfalse;
	}

	VectorCopy ( eState->pos.trBase, body->r.currentOrigin );
	trap_LinkEntity (body);
}

//======================================================================


/*
==================
G_Client_SetViewAngle

==================
*/
void G_Client_SetViewAngle( gentity_t *ent, vec3_t angle ) {
	int			i;

	// set the delta angle
	for (i=0 ; i<3 ; i++) {
		int		cmdAngle;

		cmdAngle = ANGLE2SHORT(angle[i]);
		ent->client->ps.delta_angles[i] = cmdAngle - ent->client->pers.cmd.angles[i];
	}
	VectorCopy( angle, ent->s.angles );
	VectorCopy (ent->s.angles, ent->client->ps.viewangles);
}

/*
================
G_Client_Respawn
================
*/
extern char *ClassNameForValue( pclass_t pClass );
void G_Client_Respawn( gentity_t *ent ) {
	qboolean		borg = qfalse;
	gentity_t*		tent = NULL;
	playerState_t*	ps = NULL;

	CopyToBodyQue (ent);

	G_Client_Spawn(ent, 0, qfalse);//RPG-X: RedTechie - Modifyed

	ps = &ent->client->ps;

	// add a teleportation effect
	if ( borg ) {
		tent = G_TempEntity( ps->origin, EV_BORG_TELEPORT );
	} else {
		tent = G_TempEntity( ps->origin, EV_PLAYER_TRANSPORT_IN );
		ps->powerups[PW_QUAD] = level.time + 4000;
	}

	tent->s.clientNum = ent->s.clientNum;
}

/**
 * Get number of clients in team.
 *
 * \param ignoreClientNum Client to ignore.
 * \param team Team.
 * \reutrn Number of clients in team.
 */
static team_t G_Client_TeamCount( int ignoreClientNum, int team ) {
	int		i;
	int		count = 0;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( i == ignoreClientNum ) {
			continue;
		}
		if ( level.clients[i].pers.connected == CON_DISCONNECTED ) {
			continue;
		}
		if ( level.clients[i].sess.sessionTeam == team ) {
			count++;
		}
	}

	return (team_t)count;
}

team_t G_Client_PickTeam( int ignoreClientNum ) {
	int		counts[TEAM_NUM_TEAMS];

	counts[TEAM_BLUE] = G_Client_TeamCount( ignoreClientNum, TEAM_BLUE );
	counts[TEAM_RED] = G_Client_TeamCount( ignoreClientNum, TEAM_RED );

	if ( counts[TEAM_BLUE] > counts[TEAM_RED] ) {
		return TEAM_RED;
	}
	if ( counts[TEAM_RED] > counts[TEAM_BLUE] ) {
		return TEAM_BLUE;
	}
	// equal team count, so join the team with the lowest score
	if ( level.teamScores[TEAM_BLUE] > level.teamScores[TEAM_RED] ) {
		return TEAM_RED;
	}
	if ( level.teamScores[TEAM_BLUE] < level.teamScores[TEAM_RED] ) {
		return TEAM_BLUE;
	}
	return (team_t)irandom( TEAM_RED, TEAM_BLUE );
}

/*
===========
ForceClientSkin

Forces a client's skin (for teamplay)
HEAVILY modified for the RPG-X
Player Model system :P
===========
*/
/**
*	Forces a client's skin (for teamplay)
*	HEAVILY modified for the RPG-X
*	Player Model system
*/
static void ForceClientSkin(char* model, const char* skin ) {
	char* p = NULL;
	char* q = NULL;

	//we expect model to equal 'char/model/skin'

	p = strchr(model, '/');

	//if no slashes at all
	if ( p == NULL || p[0] == 0 || p[1] == 0 ) {
		//input everything
		strncat(model, "/", MAX_QPATH);
		strncat(model, "main", MAX_QPATH);
		strncat(model, "/", MAX_QPATH);
		strncat(model, skin, MAX_QPATH);
	} else { //ie we got a slash (which should be the first of two
		p++;
		q = strchr(p, '/'); //okay, we should get another one if one was already found
		if (q == NULL || q[0] == 0 || q[1] == 0 ) 
		{ //no slashes were found?? >.<
			//okay, let's assume they specified the .model file, no skin
			//so just add the skin to the end :P
			strncat(model, "/", MAX_QPATH);
			strncat(model, skin, MAX_QPATH);
		} else  {
			q++;
			*q= '\0';
			strncat(model, skin, MAX_QPATH);		
		}
	}
}

/*
===========
ClientCheckName
============
*/
static void ClientCleanName( const char* in, char* out, int outSize ) {
	int32_t	len = 0;
	int32_t colorlessLen = 0;
	char	ch = 0;
	char*	p = NULL;
	int32_t	spaces = 0;

	//save room for trailing null byte
	outSize--;

	p = out;
	*p = 0;
	spaces = 0;

	while( 1 ) {
		ch = *in++;
		if( ch == 0 ) {
			break;
		}

		// don't allow leading spaces
		if( !*p && ch == ' ' ) {
			continue;
		}

		// check colors
		if( ch == Q_COLOR_ESCAPE ) {
			// solo trailing carat is not a color prefix
			if( !*in ) {
				break;
			}

			// don't allow black in a name, period
			if( ColorIndex(*in) == 0 ) {
				in++;
				continue;
			}

			// make sure room in dest for both chars
			if( len > outSize - 2 ) {
				break;
			}

			*out++ = ch;
			*out++ = *in++;
			len += 2;
			continue;
		}

		// don't allow too many consecutive spaces
		if( ch == ' ' ) {
			spaces++;
			if( spaces > 3 ) {
				continue;
			}
		}
		else {
			spaces = 0;
		}

		if( len > outSize - 1 ) {
			break;
		}

		*out++ = ch;
		colorlessLen++;
		len++;
	}
	*out = 0;

	// don't allow empty names
	if( *p == 0 || colorlessLen == 0 )
	{
		Q_strncpyz( p, "RedShirt", outSize );
	}
}

/*
===========
legalSkin

Compare a list of races with an incoming race name.
Used to decide if in a CTF game where a race is specified for a given team if a skin is actually already legal.
===========
*/
/**
*	Compare a list of races with an incoming race name.
*	Used to decide if in a CTF game where a race is specified for a given team if a skin is actually already legal.
*/
static qboolean legalSkin(const char*race_list, const char* race)
{
	char current_race_name[125];
	const char *s = race_list;
	const char *max_place = race_list + strlen(race_list);
	const char *marker = NULL;

	memset(current_race_name, 0, sizeof(current_race_name));
	// look through the list till it's empty
	while (s < max_place)
	{
		marker = s;
		// figure out from where we are where the next ',' or 0 is
		while (*s != ',' && *s != 0)
		{
			s++;
		}

		// copy just that name
		strncpy(current_race_name, marker, (s-marker)+1);

		// avoid the comma or increment us past the end of the string so we fail the main while loop
		s++;

		// compare and see if this race is the same as the one we want
		if (Q_stricmp(current_race_name, race) == 0)
		{
			return qtrue;
		}
	}
	return qfalse;
}


/*
===========
randomSkin

given a race name, go find all the skins that use it, and randomly select one
===========
*/
/**
*	given a race name, go find all the skins that use it, and randomly select one
*/
static void randomSkin(const char* race, char* model, int32_t current_team, int32_t clientNum)
{
	char**		skinsForRace = NULL;
	int32_t		howManySkins = 0;
	int32_t		i = 0;
	int32_t		x = 0;
	int32_t		temp = 0;
	int32_t		skin_count_check = 0;
	char**		skinNamesAlreadyUsed = NULL;
	int32_t		current_skin_count = 0;
	gentity_t*	ent = NULL;
	char*		userinfo = NULL;
	char		temp_model[MAX_QPATH];

	memset(temp_model, 0, sizeof(temp_model));

	skinsForRace = malloc(MAX_SKINS_FOR_RACE * 128 * sizeof(char));
	if(skinsForRace == NULL) {
		G_Error("Was unable to allocate %i bytes.\n", MAX_SKINS_FOR_RACE * 128 * sizeof(char));
		return;
	}
	skinNamesAlreadyUsed = malloc(16 * 128 * sizeof(char));
	if(skinNamesAlreadyUsed == NULL) {
		G_Error("Was unable to allocate %i bytes.\n", 16 * 128 * sizeof(char));
		return;
	}

	memset(skinsForRace, 0, MAX_SKINS_FOR_RACE * 128 * sizeof(char));
	memset(skinNamesAlreadyUsed, 0, 16 * 128 * sizeof(char));

	// first up, check to see if we want to select a skin from someone that's already playing on this guys team
	skin_count_check = g_random_skin_limit.integer;
	if (skin_count_check != 0) {
		// sanity check the skins to compare against count
		if (skin_count_check > 16) {
			skin_count_check = 16;
		}

		// now construct an array of the names already used
		for ( ; i<g_maxclients.integer; i++) {
			// did we find enough skins to grab a random one from yet?
			if (current_skin_count == skin_count_check)
			{
				break;
			}

			ent = g_entities + i;
			if (ent == NULL || !ent->inuse || i == clientNum) {
				continue;
			}

			// no, so look at the next one, and see if it's in the list we are constructing
			// same team?
			if 	(ent->client != NULL && ent->client->sess.sessionTeam == current_team) {
				userinfo = malloc(MAX_INFO_STRING * sizeof(char));
				if(userinfo == NULL) {
					G_Error("Was unable to allocate %i bytes.\n", MAX_INFO_STRING * sizeof(char));
					return;
				}
				// so what's this clients model then?
				trap_GetUserinfo( i, userinfo, MAX_INFO_STRING * sizeof(char) );
				Q_strncpyz( temp_model, Info_ValueForKey (userinfo, "model"), sizeof( temp_model ) );

				free(userinfo);

				// check the name
				for (x = 0; x< current_skin_count; x++)	{
					// are we the same?
					if (Q_stricmp(skinNamesAlreadyUsed[x], temp_model) == 0) {
						// yeah - ok we already got this one
						break;
					}
				}

				// ok, did we match anything?
				if (x == current_skin_count) {
					// no - better add this name in
					Q_strncpyz(skinNamesAlreadyUsed[current_skin_count], temp_model, sizeof(skinNamesAlreadyUsed[current_skin_count]));
					current_skin_count++;
				}
			}
		}

		// ok, array constructed. Did we get enough?
		if (current_skin_count >= skin_count_check) {
			// yeah, we did - so select a skin from one of these then
			temp = rand() % current_skin_count;
			Q_strncpyz( model, skinNamesAlreadyUsed[temp], MAX_QPATH );
			ForceClientSkin(model, "");
			free(skinNamesAlreadyUsed);
			free(skinsForRace);
			return;
		}
	}

	// search through each and every skin we can find
	for (i=0; i<group_count && howManySkins < MAX_SKINS_FOR_RACE; i++){

		// if this models race list contains the race we want, then add it to the list
		if (legalSkin(group_list[i].text, race)) {
			Q_strncpyz( skinsForRace[howManySkins++], group_list[i].name , 128 );
		}
	}

	// set model to a random one
	if (howManySkins > 0) {
		temp = rand() % howManySkins;
		Q_strncpyz( model, skinsForRace[temp], MAX_QPATH );
	} else {
		model[0] = 0;
	}

	free(skinsForRace);
	free(skinNamesAlreadyUsed);
}

/*
===========
getNewSkin

Go away and actually get a random new skin based on a group name
============
*/
/**
*	Go away and actually get a random new skin based on a group name	
*/
static qboolean getNewSkin(const char* group, char* model, const char* color, const gclient_t* client, int32_t clientNum)
{
	char* temp_string = NULL;

  	// go away and get what ever races this skin is attached to.
  	// remove blue or red name
  	ForceClientSkin(model, "");

  	temp_string = G_searchGroupList(model);

  	// are any of the races legal for this team race?
  	if (legalSkin(temp_string, group)) {
  		ForceClientSkin(model, color);
  		return qfalse;
  	}

  	//if we got this far, then we need to reset the skin to something appropriate
  	randomSkin(group, model, client->sess.sessionTeam, clientNum);
	return qtrue;
}

/*
===========
G_Client_UserinfoChanged
============
*/
/**
*	Called from G_Client_Connect when the player first connects and
*	directly by the server system when the player updates a userinfo variable.
*
*	The game can override any of the settings and call trap_SetUserinfo
*	if desired.
*/
void G_Client_UserinfoChanged( int32_t clientNum ) {
	gentity_t*			ent = NULL;
	int32_t				i = 0;
	int32_t				modelOffset = 0;
	char*				s = NULL;
	char				model[MAX_QPATH];
	char				oldname[MAX_STRING_CHARS];
	char				userinfo[MAX_INFO_STRING];
	char				age[MAX_NAME_LENGTH];
	char				race[MAX_NAME_LENGTH];
	char				sHeight[10];
	char				sWeight[10];
	double				weight;
	double				height;
	gclient_t*			client = NULL;
	qboolean			reset;
	qboolean			changeName = qtrue; //TiM : For the name filter
	clientPersistant_t*	pers = NULL;
	clientSession_t*	sess = NULL;

	ent = g_entities + clientNum;
	if(ent == NULL) {
		return;
	}

	memset(model, 0, sizeof(model));
	memset(oldname, 0, sizeof(oldname));
	memset(userinfo, 0, sizeof(userinfo));
	memset(age, 0, sizeof(age));
	memset(race, 0, sizeof(race));
	memset(sHeight, 0, sizeof(sHeight));
	memset(sWeight, 0, sizeof(sWeight));
	client = ent->client;
	pers = &client->pers;
	sess = &client->sess;

	//TiM - Exit if this user has had their info clamped
	if ( (ent->flags & FL_CLAMPED) != 0 ) {
		return;
	}

	trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

	// check for malformed or illegal info strings
	if ( !Info_Validate(userinfo) ) {
		strcpy (userinfo, "\\name\\badinfo");
	}

	// check for local client
	s = Info_ValueForKey( userinfo, "ip" );
	if ( strcmp( s, "localhost" ) == 0 ) {
		pers->localClient = qtrue;
	}

	// check the item prediction
	s = Info_ValueForKey( userinfo, "cg_predictItems" );
	if ( atoi( s ) == 0 ) {
		pers->predictItemPickup = qfalse;
	} else {
		pers->predictItemPickup = qtrue;
	}

	// set name
	//TiM: Filter for if a player is already on this server with that name.
	s = Info_ValueForKey (userinfo, "name");
	
	if ( (rpg_uniqueNames.integer != 0) && (( ent->r.svFlags & SVF_BOT ) == 0) ) {
		char newName[36];
		char activeName[36];

		memset(newName, 0, sizeof(newName));
		memset(activeName, 0, sizeof(activeName));
		ClientCleanName( s, newName, sizeof(newName) );
		Q_CleanStr( newName );

		//loop thru all the clients, and see if we have one that has the same name as our proposed one
		for ( i = 0; i < level.numConnectedClients; i++ ) {
			Q_strncpyz( activeName, g_entities[i].client->pers.netname, sizeof( activeName ) );
			Q_CleanStr( activeName );

			if ( (g_entities[i].client->ps.clientNum != client->ps.clientNum)
				&& (Q_stricmp( newName, activeName ) == 0) ) {
				trap_SendServerCommand( ent-g_entities, " print \"Unable to change name. A player already has that name on this server.\n\" ");
				changeName = qfalse;
				break;
			}
		}
	}

	if ( changeName ) {
		Q_strncpyz ( oldname, pers->netname, sizeof( oldname ) );
		ClientCleanName( s, pers->netname, sizeof(pers->netname) );

		if ( sess->sessionTeam == TEAM_SPECTATOR ) {
			if ( sess->spectatorState == SPECTATOR_SCOREBOARD ) {
				Q_strncpyz( pers->netname, "scoreboard", sizeof(pers->netname) );
			}
		}

		if ( pers->connected == CON_CONNECTED ) {
			if ( strcmp( oldname, pers->netname ) != 0) {
				if ( (!levelExiting) && (level.intermissiontime == 0) )	{
					//no need to do this during level changes
					trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " renamed to %s\n\"", oldname, pers->netname) );
				}
			}
		}
	}

	pers->pms_height = atof( Info_ValueForKey( userinfo, "height" ) );
	if ( pers->pms_height == 0 ) {
		pers->pms_height = 1.0f;
	}

	pers->maxHealth = atoi( Info_ValueForKey( userinfo, "handicap" ) );
	if ( pers->maxHealth < 1 || pers->maxHealth > 100 ) {
		pers->maxHealth = 100;
	}
	//if you have a class, ignores handicap and 100 limit, sorry
	client->ps.stats[STAT_MAX_HEALTH] = pers->maxHealth;

	Q_strncpyz( model, Info_ValueForKey (userinfo, "model"), sizeof( model ) );

	// team

	switch( sess->sessionTeam ) {
	case TEAM_RED: 
		{
			// decide if we are going to have to reset a skin cos it's not applicable to a race selected
			if ((g_gametype.integer < GT_TEAM) || (Q_stricmp("", g_team_group_red.string) == 0)) {
				ForceClientSkin(model, "red");
				break;
			} else {
				// at this point, we are playing CTF and there IS a race specified for this game
				reset = getNewSkin(g_team_group_red.string, model, "red", client, clientNum);

				// did we get a model name back?
				if (model[0] == 0) {
					// no - this almost certainly means we had a bogus race is the g_team_group_team cvar
					// so reset it to starfleet and try it again
					Com_Printf("WARNING! - Red Group %s is unknown - resetting Red Group to Allow Any Group\n", g_team_group_red.string);
					trap_Cvar_Set("g_team_group_red", "");
					trap_Cvar_Register( &g_team_group_red, "g_team_group_red",
						"", CVAR_LATCH);

					// Since we are allow any group now, just get his normal model and carry on
					Q_strncpyz( model, Info_ValueForKey (userinfo, "model"), sizeof( model ) );
					ForceClientSkin(model, "red");
					reset = qfalse;
				}

				if (reset) {
					if ( !levelExiting ) {
						//no need to do this during level changes
						trap_SendServerCommand( -1, va("print \"In-appropriate skin selected for %s on team %s\nSkin selection overridden from skin %s to skin %s\n\"",
							pers->netname, g_team_group_red.string, Info_ValueForKey (userinfo, "model"), model));
					}
					ForceClientSkin(model, "red");
					// change the value in out local copy, then update it on the server
					Info_SetValueForKey(userinfo, "model", model);
					trap_SetUserinfo(clientNum, userinfo);
				}
				break;
			}
		}
	case TEAM_BLUE: 
		{
			// decide if we are going to have to reset a skin cos it's not applicable to a race selected
			if ((g_gametype.integer < GT_TEAM) || (Q_stricmp("", g_team_group_blue.string) == 0)) {
				ForceClientSkin(model, "blue");
				break;
			} else {
				// at this point, we are playing CTF and there IS a race specified for this game
				// go away and get what ever races this skin is attached to.
				reset = getNewSkin(g_team_group_blue.string, model, "blue", client, clientNum);

				// did we get a model name back?
				if (model[0] == 0) {
					// no - this almost certainly means we had a bogus race is the g_team_group_team cvar
					// so reset it to klingon and try it again
					Com_Printf("WARNING! - Blue Group %s is unknown - resetting Blue Group to Allow Any Group\n", g_team_group_blue.string);
					trap_Cvar_Set("g_team_group_blue", "");
					trap_Cvar_Register( &g_team_group_blue, "g_team_group_blue",
						"", CVAR_LATCH );

					// Since we are allow any group now, just get his normal model and carry on
					Q_strncpyz( model, Info_ValueForKey (userinfo, "model"), sizeof( model ) );
					ForceClientSkin(model, "blue");
					reset = qfalse;
				}

				if (reset) {
					if ( !levelExiting ) {
						//no need to do this during level changes
						trap_SendServerCommand( -1, va("print \"In-appropriate skin selected for %s on team %s\nSkin selection overridden from skin %s to skin %s\n\"",
							pers->netname, g_team_group_blue.string, Info_ValueForKey (userinfo, "model"), model));
					}
					ForceClientSkin(model, "blue");
					// change the value in out local copy, then update it on the server
					Info_SetValueForKey(userinfo, "model", model);
					trap_SetUserinfo(clientNum, userinfo);
				}
				break;
			}
		}
	default:
		break;
	}

	if((g_gametype.integer >= GT_TEAM) && (sess->sessionTeam == TEAM_SPECTATOR)) {
		// don't ever use a default skin in teamplay, it would just waste memory
		ForceClientSkin(model, "red");
	}

	if((rpg_rpg.integer != 0) && (rpg_forceclasscolor.integer != 0) && (g_gametype.integer < GT_TEAM)) {
		ForceClientSkin( model, g_classData[sess->sessionClass].modelSkin );
	}

	//TiM : For when an admin chooses not to see admin messages
	//Marcin : and check for privacy mode - 24/12/2008
	s = Info_ValueForKey( userinfo, "noAdminChat" );
	if ( atoi( s ) > 0 ) {
		client->noAdminChat = qtrue;
	} else {
		client->noAdminChat = qfalse;
	}

	// teamInfo
	s = Info_ValueForKey( userinfo, "teamoverlay" );
	if ( ! *s || atoi( s ) != 0 ) {
		pers->teamInfo = qtrue;
	} else {
		pers->teamInfo = qfalse;
	}

	//PMS system - lock down the values
	s = Info_ValueForKey( userinfo, "height" );
	height = atof( s );
	if (height > (float)rpg_maxHeight.value ) {
		Q_strncpyz( sHeight, rpg_maxHeight.string, sizeof( sHeight ) );
	} else if (height < (float)rpg_minHeight.value ) {
		Q_strncpyz( sHeight, rpg_minHeight.string, sizeof( sHeight ) );
	} else {
		Q_strncpyz( sHeight, s, sizeof( sHeight ) );
	}

	//TiM - needed for height offset
	pers->pms_height = atof( sHeight );

	//PMS system - lock down the values
	s = Info_ValueForKey( userinfo, "weight" );
	weight = atof( s );
	if (weight > (float)rpg_maxWeight.value ) {
		Q_strncpyz( sWeight, rpg_maxWeight.string, sizeof( sWeight ) );
	} else if (weight < (float)rpg_minWeight.value ) {
		Q_strncpyz( sWeight, rpg_minWeight.string, sizeof( sWeight ) );
	} else {
		Q_strncpyz( sWeight, s, sizeof( sWeight ) );
	}

	s = Info_ValueForKey( userinfo, "age" );
	Q_strncpyz( age, s, sizeof(age) );

	s = Info_ValueForKey( userinfo, "race" );
	Q_strncpyz( race, s, sizeof( race ) );

	s = Info_ValueForKey( userinfo, "modelOffset" );
	modelOffset = atoi( s );

	// send over a subset of the userinfo keys so other clients can
	// print scoreboards, display models, and play custom sounds
	//FIXME: In future, we'll lock down these PMS values so we can't have overloaded transmission data
	if ( (ent->r.svFlags & SVF_BOT) != 0 ) {
		s = va("n\\%s\\t\\%i\\p\\%i\\model\\%s\\hc\\%i\\w\\%i\\l\\%i\\skill\\%s\\age\\25\\height\\%s\\weight\\%s\\race\\Bot\\of\\%i\\admin\\0",
			pers->netname, sess->sessionTeam, sess->sessionClass, model,
			pers->maxHealth, sess->wins, sess->losses,
			Info_ValueForKey( userinfo, "skill" ),
			sHeight, sWeight, modelOffset );
	} else {
		s = va("n\\%s\\t\\%i\\p\\%i\\model\\%s\\hc\\%i\\w\\%i\\l\\%i\\age\\%s\\height\\%s\\weight\\%s\\race\\%s\\of\\%i\\admin\\%i",
			pers->netname, sess->sessionTeam, sess->sessionClass, model,
			pers->maxHealth, sess->wins, sess->losses, age,
			sHeight, sWeight, race, modelOffset, ((int)IsAdmin(g_entities+clientNum)));
	}

	trap_SetConfigstring( CS_PLAYERS+clientNum, s );

	G_LogPrintf( "ClientUserinfoChanged: %i %s\n", clientNum, s ); // no ip logging here as string might get to long
}


/*
===========
G_Client_Connect
============
*/
/**
*	Called when a player begins connecting to the server.
*	Called again for every map change or tournement restart.
*
*	The session information will be valid after exit.
*
*	Return NULL if the client should be allowed, otherwise return
*	a string with the reason for denial.
*	
*	Otherwise, the client will be sent the current gamestate
*	and will eventually get to G_Client_Begin.
*
*	firstTime will be qtrue the very first time a client connects
*	to the server machine, but qfalse on map changes and tournement
*	restarts.
*/
char* G_Client_Connect( int32_t clientNum, qboolean firstTime, qboolean isBot ) {
	char*		value = NULL;
	char*		newClass = NULL;
	char*		newRank = NULL;
	char		ip[64]; //TiM : Saved the IP data for player recon feature
	char		userinfo[MAX_INFO_STRING];
	gclient_t*	client = NULL;
	gentity_t*	ent = NULL;
	vmCvar_t	mapname;
	vmCvar_t	sv_hostname;
	
	ent = &g_entities[ clientNum ];
	if(ent == NULL) {
		return "Critical Error: Client entity was NULL";
	}

	memset(ip, 0, sizeof(ip));
	memset(userinfo, 0, sizeof(userinfo));

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );
	trap_Cvar_Register( &sv_hostname, "sv_hostname", "", CVAR_SERVERINFO | CVAR_ROM );

	trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

	// check to see if they are on the banned IP list
	value = Info_ValueForKey (userinfo, "ip");

	Q_strncpyz( ip, value, sizeof(ip) );

	if ( G_FilterPacket( value ) || CheckID( Info_ValueForKey(userinfo, "sv_securityCode" ) ) ) {
			return "Banned from this server";
	}

	// check for a password
	if ( !isBot )
	{
 		value = Info_ValueForKey (userinfo, "password");
		if ( g_password.string[0] && Q_stricmp( g_password.string, "none" ) != 0 && strcmp( g_password.string, value) != 0)
		{
			return "Invalid password";
		}
	}

	//TiM: If need be, chack to see if any other players have the current name...
	//evil impersonators and the likes
	if ( rpg_uniqueNames.integer != 0 && !isBot ) {
		char name[36];
		char oldName[36];
		int32_t i = 0;

		memset(name, 0, sizeof(name));
		memset(oldName, 0, sizeof(oldName));

		//get the name
		value = Info_ValueForKey( userinfo, "name" );
		//Clean the data
		ClientCleanName( value, name, sizeof( name ) );
		//Now, do a compare with all clients in the server
		for (i = 0; i < MAX_CLIENTS; i++ ) {
			if ( (g_entities[i].client == NULL) || (g_entities[i].client->pers.connected != CON_CONNECTED) ) {
				continue;
			}

			if ( g_entities[i].client->pers.netname[0] ) {

				//local copy the string and work on that, else we risk wrecking other people's names
				Q_strncpyz( oldName, g_entities[i].client->pers.netname, sizeof( oldName ) );
				if ( (Q_stricmp( Q_CleanStr(name), Q_CleanStr(oldName) ) == 0) && !isBot ) {
					return "There is already a user with that name.";
				}
			}
		}
	}

	// they can connect
	ent->client = level.clients + clientNum;
	client = ent->client;

	memset( client, 0, sizeof(*client) );

	client->pers.connected = CON_CONNECTING;

	// read or initialize the session data
	if ( firstTime || level.newSession ) {
		G_InitSessionData( client, userinfo );
	}
	G_ReadSessionData( client );

	if( isBot ) {
		ent->r.svFlags |= SVF_BOT;
		ent->inuse = qtrue;
		if( !G_BotConnect( clientNum, !firstTime ) ) {
			return "BotConnectfailed";
		}
	}

	// get and distribute relevent paramters
	G_LogPrintf( "ClientConnect: %i (%s)\n", clientNum, g_entities[clientNum].client->pers.ip );
	if ( rpg_rpg.integer != 0 /*&& firstTime*/ )
	{
		//TiM: Code for automatic class + rank switching
		//========================================================
		if ( isBot ) {
			client->sess.sessionClass = 0;
			client->ps.persistant[PERS_SCORE] = 1;
		} else {
			int32_t tmpScore = 0;
			int32_t i = 0;
			qboolean changeRank = qfalse;

			newClass = Info_ValueForKey (userinfo, "ui_playerClass" );
			newRank	= Info_ValueForKey (userinfo, "ui_playerRank" );

			//Com_Printf( S_COLOR_RED "Data: %s %s\n", newClass, newRank );

			if ( newClass[0] != 0 ) {
				client->sess.sessionClass = ValueNameForClass ( newClass ); //TiM: BOOYEAH! :)
				//if class doesn't exist, default to 0
				if ( client->sess.sessionClass < 0 ) {
					client->sess.sessionClass = 0;
				}
			} else {
				client->sess.sessionClass = 0;
			}

			for (i = 0; i < MAX_RANKS; i++ ) {
				if ( rpg_startingRank.string[0] == 0 && newRank[0] != 0 ) {
					if ( Q_stricmp( newRank, g_rankNames[i].consoleName ) == 0 ) {
						tmpScore = i;//1 << i;

						if ( rpg_changeRanks.integer != 0 ) {
							changeRank = qtrue;
						}
						break;
					}
				} else {
					if ((rpg_startingRank.string[0] != 0) && (Q_stricmp( g_rankNames[i].consoleName, rpg_startingRank.string ) == 0)) {
						tmpScore = i;// 1 << i;
						changeRank = qtrue;
						break;
					}
				}
			}
					
			if ( changeRank ) {
				ent->client->UpdateScore = qtrue;
				SetScore( ent, tmpScore );
			}
		}

		//========================================================
	}
	G_Client_UserinfoChanged( clientNum );

	//RPG-X: Save the ip for later - has to be down here, since it gets flushed in the above function
	Q_strncpyz( ent->client->pers.ip, ip, sizeof( ent->client->pers.ip ) );

	// don't do the "xxx connected" messages if they were caried over from previous level
	if ( firstTime )
	{
		if ( !levelExiting )
		{//no need to do this during level changes
			qboolean nameFound = qfalse;
			
			//Check to see if this player already connected on this server
			if ( (rpg_renamedPlayers.integer != 0) && ((ent->r.svFlags & SVF_BOT) == 0) ) {
				int32_t i = 0;

				for ( i = 0; i < MAX_RECON_NAMES; i++ ) {
					if ( g_reconData[i].previousName[0] == 0 ) {
						continue;
					}

					if ( (Q_stricmp( client->pers.ip, g_reconData[i].ipAddress ) == 0) 
							&& (Q_stricmp( client->pers.netname, g_reconData[i].previousName ) != 0 )) {		
						trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " (With the previous name of %s" S_COLOR_WHITE ") connected\n\"", client->pers.netname, g_reconData[i].previousName) );
						nameFound = qtrue;
						break;
					}
				}
			}

			if ( !nameFound ) {
				trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " connected\n\"", client->pers.netname) );
			}

			//RPG-X | Phenix | 07/04/2005
			client->AdminFailed = 0;
			ent->n00bCount = 0;
			client->LoggedAsDeveloper = qfalse;
		}
	}

	if ( (g_gametype.integer >= GT_TEAM) && (client->sess.sessionTeam != TEAM_SPECTATOR )) {
		BroadcastTeamChange( client, -1 );
	}

	// count current clients and rank for scoreboard
	//G_Client_CalculateRanks( qfalse );

	//RPG-X: J2J - Reset Variables
	DragDat[clientNum].AdminId = -1;
	DragDat[clientNum].distance = 0;
	g_entities[clientNum].client->noclip = qfalse; 

	return NULL;
}

extern holoData_t holoData;

//! Think function for temporal entity that transmits the holodeck date to the client
static void holoTent_think(gentity_t* ent) {
	if(ent->count == 0) {
		trap_SendServerCommand(ent-g_entities, va("holo_data %i", holoData.numProgs));
		ent->count = 1;
		ent->health = 0;
		ent->nextthink = level.time + 250;
		return;
	}
	if(ent->health == holoData.numProgs) {
		ent->count++;
		ent->health = 0;
	}
	switch(ent->count) {
		case 1: // name
			trap_SendServerCommand(ent-g_entities, va("holo_data \"n%i\\%s\\\"", ent->health, holoData.name[ent->health]));
			break;
		case 2: // desc1
			trap_SendServerCommand(ent-g_entities, va("holo_data \"da%i\\%s\\\"", ent->health, holoData.desc1[ent->health]));
			break;
		case 3: // desc2
			trap_SendServerCommand(ent-g_entities, va("holo_data \"db%i\\%s\\\"", ent->health, holoData.desc2[ent->health]));
			break;
		case 4: // image
			trap_SendServerCommand(ent-g_entities, va("holo_data \"i%i\\%s\\\"", ent->health, holoData.image[ent->health]));
			break;
	}
	ent->health++;
	if(ent->count > 4) {
		G_PrintfClient(ent, "Received data of %i holodeck programs.\n", holoData.numProgs);
		G_FreeEntity(ent);
		return;
	}
	ent->nextthink = level.time + 250;
}

//! Create a temporal entity that sends over the holodata to the client
static void G_SendHoloData(int32_t clientNum) {
	gentity_t *holoTent;

	holoTent = G_Spawn();
	holoTent->classname = "holoTent";

	holoTent->target_ent = g_entities + clientNum;

	holoTent->think = holoTent_think;
	holoTent->nextthink = level.time + 2500;
}

//! Think function for temporal entity that transmits the server change data and map change data for transporter UI
static void transTent_think(gentity_t* ent) {
	char	temp[MAX_STRING_CHARS];
	int32_t i;

	memset(temp, 0, sizeof(temp));

	for(i = 0; i < level.srvChangeData.count; i++) {
		if(temp[0] == 0) {
			Com_sprintf(temp, sizeof(temp), "ui_trdata d%i\\%s\\", i, level.srvChangeData.name[i]);
		} else {
			Com_sprintf(temp, sizeof(temp), "%sd%i\\%s\\", temp, i, level.srvChangeData.name[i]);
		}
	}

	trap_SendServerCommand(ent->target_ent-g_entities, temp);
	G_FreeEntity(ent);
}

//! creates an entity that transmits the server change data to the client 
static void G_SendTransData(int32_t clientNum) {
	gentity_t *transTent;

	transTent = G_Spawn();
	transTent->classname = "transTent";

	transTent->target_ent = g_entities + clientNum;

	transTent->think = transTent_think;
	transTent->nextthink = level.time + 500;
}

/*
===========
G_Client_Begin
============
*/
/**
*	called when a client has finished connecting, and is ready
*	to be placed into the level.  This will happen every level load,
*	and on transition between teams, but doesn't happen on respawns
*/
void G_Client_Begin( int32_t clientNum, qboolean careAboutWarmup, qboolean isBot, qboolean first ) {
	gentity_t*	ent = NULL;
	gclient_t*	client = NULL;
	gentity_t*	tent = NULL;
	gentity_t*	selfdestruct = NULL;
	int32_t		flags = 0;
	qboolean	alreadyIn = qfalse;
	int32_t		score = 0;

	ent = g_entities + clientNum;
	if(ent == NULL) {
		return;
	}

	if( ent->botDelayBegin ) {
		G_QueueBotBegin( clientNum );
		ent->botDelayBegin = qfalse;
		return;
	}

	client = level.clients + clientNum;

	if ( ent->r.linked ) {
		trap_UnlinkEntity( ent );
	}
	G_InitGentity( ent );
	ent->touch = 0;
	ent->pain = 0;
	ent->client = client;

	if ( client->pers.connected == CON_CONNECTED ) {
		alreadyIn = qtrue;
	}
	client->pers.connected = CON_CONNECTED;
	client->pers.enterTime = level.time;
	client->pers.teamState.state = TEAM_BEGIN;

	// save eflags around this, because changing teams will
	// cause this to happen with a valid entity, and we
	// want to make sure the teleport bit is set right
	// so the viewpoint doesn't interpolate through the
	// world to the new position

	//TiM... I think this is why my damn RANK SYSTEM ENHANCEMENT HAS BEEN BUGGING OUT!!@!@!!
	//ARRRGRGRGRGRGRGRGRGRGRGRGHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH!!!!! D:<
	flags = client->ps.eFlags;
	score = client->ps.persistant[PERS_SCORE];
	memset( &client->ps, 0, sizeof( client->ps ) );
	client->ps.eFlags = flags;

	client->UpdateScore = qtrue;
	SetScore( ent, score );

	// locate ent at a spawn point
	G_Client_Spawn( ent, 0, qfalse );//RPG-X: RedTechie - Modifyed

	if ( client->sess.sessionTeam != TEAM_SPECTATOR) {	
		// Don't use transporter FX for spectators or those watching the holodoors.
		// send event

		ent->client->ps.powerups[PW_QUAD] = level.time + 4000;
		tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TRANSPORT_IN );
		tent->s.clientNum = ent->s.clientNum;
	}
	G_LogPrintf( "ClientBegin: %i (%s)\n", clientNum, g_entities[clientNum].client->pers.ip );

	// count current clients and rank for scoreboard
	G_Client_CalculateRanks( qfalse );
	
	//TiM - This appears to be a flaw in Raven's design
	//When a client connects, or if they enter admin or medics class
	//ensure the relevant health data is sent to them, or else they'll
	//see anomalies when scanning players
	if ( (client->sess.sessionTeam == TEAM_SPECTATOR) || 
			(g_classData[client->sess.sessionClass].isMedical) || 
			(g_classData[client->sess.sessionClass].isAdmin) )
	{
		int32_t		i = 0;
		char		entry[16];
		char		command[1024];
		int32_t		numPlayers = 0;
		gentity_t	*player;
		int32_t		len = 0;
		int32_t		cmdLen=0;

		memset(entry, 0, sizeof(entry));
		memset(command, 0, sizeof(command));

		for ( ; i < g_maxclients.integer; i++ )
		{
			player = g_entities + i;

			if ( (player == NULL) || (player == ent) || !player->inuse ) {
				continue;
			}
			
			Com_sprintf( entry, sizeof(entry), " %i %i", i, player->health >= 0 ? player->health : 0 );
			len = strlen( entry );
			if ( cmdLen + len > sizeof( command ) ) {
				break;
			}
			strcpy( command + cmdLen, entry );
			cmdLen += len;

			numPlayers++;
		}

		if ( numPlayers > 0 ) {
			trap_SendServerCommand( clientNum, va("hinfo %i%s", numPlayers, command) );
		}
	}

	//RPG-X: RedTechie - But we dont care about warmup!
	if ( careAboutWarmup ) {
		if (level.restarted || g_restarted.integer != 0) {
			trap_Cvar_Set( "g_restarted", "0" );
			level.restarted = qfalse;
		}
	}

	//RPG-X | Phenix | 21/11/2004
	//BOOKMARK FOR INIT
	if(!alreadyIn) {
		// RPG-X | Phenix | 06/04/2005
		ent->client->n00bTime = -1;
	}

	ent->client->fraggerTime = -1;

	// kef -- should reset all of our awards-related stuff
	G_ClearClientLog(clientNum);

	//TiM - if our user's security key was default, transmit the received IP bak to
	//the client and get it to encode it into our new key

	//Scooter's filter list
	if( Q_stricmp( ent->client->pers.ip, "localhost" ) != 0			//localhost
		&& Q_strncmp( ent->client->pers.ip, "10.", 3 )	!= 0		//class A
		&& Q_strncmp( ent->client->pers.ip, "172.16.", 7 ) != 0		//class B
		&& Q_strncmp( ent->client->pers.ip, "192.168.", 8 )	!= 0	//class C
		&& Q_strncmp( ent->client->pers.ip, "127.", 4 )	!= 0		//loopback
		&& Q_strncmp( ent->client->pers.ip, "169.254.", 8 )	!= 0	//link-local
		)
	{
		char		userInfo[MAX_TOKEN_CHARS];
		uint64_t	securityID = 0;

		memset(userInfo, 0, sizeof(userInfo));

		trap_GetUserinfo( clientNum, userInfo, sizeof( userInfo ) );
		if ( userInfo[0] == 0 ) {
			return;
		}

		securityID = (uint64_t)atoul( Info_ValueForKey( userInfo, "sv_securityCode" ) );

		if ( (securityID <= 0) || (securityID >= 0xffffffff) ) {
			trap_SendServerCommand( clientNum, va( "configID %s", ent->client->pers.ip ) );
		}
	}

	// send srv change data to ui
	if(!isBot && first) {
		if(level.srvChangeData.count > 0) {
			G_SendTransData(clientNum);
		}
	}

	// send holo data to ui
	if(!isBot && first) {
		if(holoData.numProgs) {
			G_SendHoloData(clientNum);
		}
	}

	//RPG-X: Marcin: show the server motd - 15/12/2008
	if ( !isBot && first ) {
		trap_SendServerCommand( ent->s.number, "motd" );
	}

    if ( !isBot ) {
		qboolean		last = qfalse;
        int32_t			len = 0;
        fileHandle_t	file = 0;
		char*			p = NULL;
		char*			q = NULL;
        char buf[16000]; // TODO move to heap ?

        len = trap_FS_FOpenFile( rpg_motdFile.string, &file, FS_READ );
        if (file == 0 || len == 0) {
            trap_SendServerCommand( ent->s.number, va("motd_line \"^1%s not found or empty^7\"", rpg_motdFile.string) );
			return;
        }

		memset(buf, 0, sizeof(buf));
        trap_FS_Read( buf, len, file );

		p = &buf[0];
		q = p;
        buf[len] = '\0';
		while ( !last ) {
			p = q;
			while ( *q != '\n' ) {
				if ( !*q ) {
					last = qtrue;
				}
				if ( ( *q == ' ' ) && ( EndWord( q ) - p ) > 78 ) {
					break;
				}
				q++;
			}
			*q = '\0';

			trap_SendServerCommand( ent->s.number, va( "motd_line \"%s\"", p ) );

			q++;
		}
      
    }
	//we may currently be selfdestructing, so send stuff for that case
	selfdestruct = G_Find(NULL, FOFS(classname), "target_selfdestruct");
	if((selfdestruct != NULL) && ((selfdestruct->spawnflags & 1) != 0)) {
		trap_SendServerCommand(ent->s.number, va("selfdestructupdate %i", selfdestruct->damage - level.time));
	} else {
		trap_SendServerCommand(ent->s.number, va("selfdestructupdate %i", -1));
	}
}

/**
 * Get weapons for a class.
 *
 * \param client The client.
 * \param pclass Class to get weapons for.
 * \author PHENIX1
 */
static void G_Client_WeaponsForClass ( gclient_t* client, pclass_t pclass )
{
	int32_t i = WP_1;
	int32_t	Bits = 0;

	Bits = ( 1 << WP_1);
	Bits |= g_classData[pclass].weaponsFlags;

	for ( ; i < MAX_WEAPONS; i++ ) {
		//if we want no weapons and aren't an admin, skip this particular weapon
		if ( (rpg_noweapons.integer != 0) && !g_classData[pclass].isAdmin ) {
			if ( (i >= WP_5) && (i <= WP_10) ) {
				continue;
			}
		}
		
		if ( (Bits & ( 1 << i )) != 0 ) {
			client->ps.stats[STAT_WEAPONS] |= ( 1 << i );
			client->ps.ammo[i] = Min_Weapon(i);
		}
	}
}

/**
 * Get holdable items for a class.
 *
 * \param client The client.
 * \param pclass Class for which to get holdables.
 */
static void G_Client_HoldablesForClass ( gclient_t* client, pclass_t pclass )
{
	if ( g_classData[pclass].isMarine ) {
		client->ps.stats[STAT_HOLDABLE_ITEM] = BG_FindItemForHoldable( HI_TRANSPORTER ) - bg_itemlist;
	} else if ( g_classData[pclass].isAdmin ) {
		client->ps.stats[STAT_HOLDABLE_ITEM] = BG_FindItemForHoldable( HI_SHIELD ) - bg_itemlist;
	}
}

void G_Client_StoreClientInitialStatus( gentity_t* ent )
{
	char userinfo[MAX_INFO_STRING];

	if ( clientInitialStatus[ent->s.number].initialized ) {
		//already set
		return;
	}

	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		//don't store their data if they're just a spectator
		return;
	}

	memset(userinfo, 0, sizeof(userinfo));
	trap_GetUserinfo( ent->s.number, userinfo, sizeof( userinfo ) );
	Q_strncpyz( clientInitialStatus[ent->s.number].model, Info_ValueForKey (userinfo, "model"), sizeof( clientInitialStatus[ent->s.number].model ) );
	clientInitialStatus[ent->s.number].pClass = ent->client->sess.sessionClass;
	clientInitialStatus[ent->s.number].team = ent->client->sess.sessionTeam;
	clientInitialStatus[ent->s.number].initialized = qtrue;
	ent->client->classChangeDebounceTime = 0;
}

/*
===========
G_Client_Spawn

Called every time a client is placed fresh in the world:
after the first G_Client_Begin, and after each respawn
Initializes all non-persistant parts of playerState
------------------------------------
Modifyed By: RedTechie
And also by Marcin - 30/12/2008
============
*/
void G_Client_Spawn(gentity_t* ent, int32_t rpgx_spawn, qboolean fromDeath ) {
	int32_t				index = 0;
	int32_t				i = 0;
	int32_t				persistant[MAX_PERSISTANT];
	int32_t				flags = 0;
	int32_t				savedPing = 0;
	int32_t				cCDT = 0;
	int32_t				clientNum = 0;
	vec3_t				spawn_origin;
	vec3_t				spawn_angles;
	gclient_t*			client = NULL;
	clientPersistant_t	saved;
	clientSession_t		savedSess;
	gentity_t*			spawnPoint = NULL;
	pclass_t			pClass = 0;
	pclass_t			oClass = 0;

	if(ent == NULL) {
		return;
	}

	memset(persistant, 0, sizeof(persistant));
	index = ent - g_entities;
	client = ent->client;
	clientNum = ent->client->ps.clientNum;

	// find a spawn point
	// do it before setting health back up, so farthest
	// ranging doesn't count this client
	if(rpgx_spawn != 1){//RPG-X: RedTechie - Make sure the spawn is regular spawn or spawn at current position (rpgx_spawn = current possition)
		if ( client->sess.sessionTeam == TEAM_SPECTATOR ) {
			spawnPoint = SelectSpectatorSpawnPoint (spawn_origin, spawn_angles);
		} else {
			do {
				// the first spawn should be at a good looking spot
				if ( !client->pers.initialSpawn && client->pers.localClient ) {
					client->pers.initialSpawn = qtrue;
					spawnPoint = SelectInitialSpawnPoint( spawn_origin, spawn_angles );
				} else {
					// don't spawn near existing origin if possible
					spawnPoint = G_Client_SelectSpawnPoint (client->ps.origin, spawn_origin, spawn_angles);
				}

				// Tim needs to prevent bots from spawning at the initial point
				// on q3dm0...
				if ( (( spawnPoint->flags & FL_NO_BOTS ) != 0) && (( ent->r.svFlags & SVF_BOT ) != 0) ) {
					continue;	// try again
				}
				// just to be symetric, we have a nohumans option...
				if ( (( spawnPoint->flags & FL_NO_HUMANS ) != 0) && (( ent->r.svFlags & SVF_BOT ) == 0) ) {
					continue;	// try again
				}

				break;

			} while ( 1 );
		}
	} //RPG-X: RedTechie - End rpgx_spawn check
	client->pers.teamState.state = TEAM_ACTIVE;

	// toggle the teleport bit so the client knows to not lerp
	if(rpgx_spawn != 1){
		flags = ent->client->ps.eFlags & EF_TELEPORT_BIT;
		flags ^= EF_TELEPORT_BIT;
	}

	// clear everything but the persistant data

	saved = client->pers;
	savedSess = client->sess;
	savedPing = client->ps.ping;
	for ( i = 0 ; i < MAX_PERSISTANT ; i++ ) {
		persistant[i] = client->ps.persistant[i];
	}
	//okay, this is hacky, but we need to keep track of this, even if uninitialized first time you spawn, it will be stomped anyway
	//RPG-X: RedTechie - Damn thing screwed my function up
	if(rpgx_spawn != 1){
		if ( client->classChangeDebounceTime ) {
			cCDT = client->classChangeDebounceTime;
		}
		memset (client, 0, sizeof(*client));
		client->classChangeDebounceTime = cCDT;
	}
	//
	client->pers = saved;
	client->sess = savedSess;
	client->ps.ping = savedPing;
	for ( i = 0 ; i < MAX_PERSISTANT ; i++ ) {
		client->ps.persistant[i] = persistant[i];
	}

	// increment the spawncount so the client will detect the respawn
	if(rpgx_spawn != 1) {
		client->ps.persistant[PERS_SPAWN_COUNT]++;
		client->airOutTime = level.time + 12000;
	}

	if(client->sess.sessionTeam != TEAM_SPECTATOR) {
		client->sess.sessionTeam = TEAM_FREE;
	}
	client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam;
	
	// clear entity values
	client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;
	client->ps.eFlags = flags;
	client->streakCount = 0;
	
	ent->client->ps.pm_type = PM_NORMAL;
	ent->s.groundEntityNum = ENTITYNUM_NONE;
	ent->client = &level.clients[index];
	ent->takedamage = qtrue;
	ent->inuse = qtrue;
	ent->classname = "player";
	
	ent->r.contents = CONTENTS_BODY;

	ent->clipmask = MASK_PLAYERSOLID;
	ent->die = G_Client_Die;
	ent->waterlevel = 0;
	ent->watertype = 0;
	ent->flags = 0;

	if(rpgx_spawn != 1) {
		VectorCopy (playerMins, ent->r.mins);
		VectorCopy (playerMaxs, ent->r.maxs);
	}

	client->ps.clientNum = index;

	// health will count down towards max_health
	ent->health = client->ps.stats[STAT_HEALTH] = client->ps.stats[STAT_MAX_HEALTH] * 1.25;

	oClass = client->sess.sessionClass;

	if ( oClass != client->sess.sessionClass ) {
		//need to send the class change
		G_Client_UserinfoChanged( client->ps.clientNum );
	}

	client->ps.persistant[PERS_CLASS] = client->sess.sessionClass;
	pClass = client->sess.sessionClass;

	if ( pClass != 0 ) {
		//no health boost on spawn for playerclasses
		ent->health = client->ps.stats[STAT_HEALTH] = client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;
	}
		
	if ( !fromDeath || (rpg_dropOnDeath.integer == 0) || (rpg_allowWeaponDrop.integer == 0) ) {
		G_Client_WeaponsForClass( client, pClass );
	} else { // Marcin: just a hand
		G_Client_WeaponsForClass( client, 0 );
	}
	G_Client_HoldablesForClass( client, pClass );
	
	if(rpgx_spawn != 1) {
		G_SetOrigin( ent, spawn_origin );
		VectorCopy( spawn_origin, client->ps.origin );
	}

	// the respawned flag will be cleared after the attack and jump keys come up
	if(rpgx_spawn != 1) {
		client->ps.pm_flags |= PMF_RESPAWNED;
	}

	trap_GetUsercmd( client - level.clients, &ent->client->pers.cmd );
	if(rpgx_spawn != 1){
		G_Client_SetViewAngle( ent, spawn_angles );
	}
	
	if(rpgx_spawn != 1) {
		if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {

		} else {
			G_MoveBox( ent );
			trap_LinkEntity (ent);

			// force the base weapon up
			client->ps.weapon = WP_1; //TiM: WP_5
			client->ps.weaponstate = WEAPON_READY;

		}
	}

	// don't allow full run speed for a bit
	client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
	client->ps.pm_time = 100;

	if(rpgx_spawn != 1) {
		client->respawnTime = level.time;
	}

	client->inactivityTime = level.time + g_inactivity.integer * 1000;
	client->latched_buttons = 0;

	// set default animations
	if (rpgx_spawn != 1 ) {
		client->ps.stats[TORSOANIM] = BOTH_STAND1;
		client->ps.stats[LEGSANIM] = BOTH_STAND1;
	}

	if ( level.intermissiontime ) {
		MoveClientToIntermission( ent );
	} else {
		// fire the targets of the spawn point
		if(rpgx_spawn != 1) {
			G_UseTargets( spawnPoint, ent );
		}

		// select the highest weapon number available, after any
		// spawn given items have fired
		client->ps.weapon = 1;

		//TiM - Always default to the null hand
		client->ps.weapon = WP_1;
	}

	// run a client frame to drop exactly to the floor,
	// initialize animations and other things
	client->ps.commandTime = level.time - 100;
	ent->client->pers.cmd.serverTime = level.time;
	ClientThink( ent-g_entities );

	// positively link the client, even if the command times are weird
	if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		BG_PlayerStateToEntityState( &client->ps, &ent->s, qtrue );
		if(rpgx_spawn != 1) {
			VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );
		}
		trap_LinkEntity( ent );
		
	}

	// run the presend to set anything else
	ClientEndFrame( ent );

	// clear entity state values
	BG_PlayerStateToEntityState( &client->ps, &ent->s, qtrue );

	//start-up messages
	//FIXME: externalize all this text!
	//FIXME: make the gametype titles be graphics!
	//FIXME: make it do this on a map_restart also
	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		//spectators just get the title of the game
		switch ( g_gametype.integer ) {
		case GT_FFA:				// free for all
			trap_SendServerCommand( ent-g_entities, va("cp \"%s\"", rpg_welcomemessage.string )  );
			break;
		case GT_TOURNAMENT:		// one on one tournament
			trap_SendServerCommand( ent-g_entities, va("cp \"%s\"", rpg_welcomemessage.string )  );
			break;
		case GT_SINGLE_PLAYER:	// single player tournament
			trap_SendServerCommand( ent-g_entities, va("cp \"%s\"", rpg_welcomemessage.string )  );
			break;
		case GT_TEAM:			// team deathmatch
			trap_SendServerCommand( ent-g_entities, va("cp \"%s\"", rpg_welcomemessage.string )  );
			break;
		case GT_CTF:				// capture the flag
			trap_SendServerCommand( ent-g_entities, va("cp \"%s\"", rpg_welcomemessage.string )  );
			break;
		}
	} else {
		if ( !clientInitialStatus[ent->s.number].initialized ) {
			//first time coming in
			switch ( g_gametype.integer ) {
			case GT_FFA:				// free for all
				trap_SendServerCommand( ent-g_entities, va("cp \"%s\"", rpg_welcomemessage.string ) );
				break;
			case GT_TOURNAMENT:		// one on one tournament
				trap_SendServerCommand( ent-g_entities, va("cp \"%s\"", rpg_welcomemessage.string )  );
				break;
			case GT_SINGLE_PLAYER:	// single player tournament
				trap_SendServerCommand( ent-g_entities, va("cp \"%s\"", rpg_welcomemessage.string )  );
				break;
			case GT_TEAM:			// team deathmatch
				trap_SendServerCommand( ent-g_entities, va("cp \"%s\"", rpg_welcomemessage.string )  );
				break;
			case GT_CTF:				// capture the flag
				trap_SendServerCommand( ent-g_entities, va("cp \"%s\"", rpg_welcomemessage.string )  );
				break;
			}
		}
	}

	if(rpgx_spawn != 0) {
		if ( (client->sess.sessionTeam == TEAM_SPECTATOR) || 
				g_classData[client->sess.sessionClass].isMedical || 
				g_classData[client->sess.sessionClass].isAdmin ) {
			int32_t		l = 0;
			int32_t		numPlayers = 0;
			int32_t		len = 0;
			int32_t		cmdLen=0;
			char		entry[16];
			char		command[1024];
			gentity_t*	player = NULL;

			memset(entry, 0, sizeof(entry));
			memset(command, 0, sizeof(entry));

			for ( ; l < g_maxclients.integer; l++ )
			{
				player = g_entities + l;

				if ( (player == NULL) || (player == ent) || !player->inuse ) {
					continue;
				}
				
				Com_sprintf( entry, sizeof(entry), " %i %i", l, player->health >= 0 ? player->health : 0 );
				len = strlen( entry );
				if ( cmdLen + len > sizeof( command ) ) {
					break;
				}
				strcpy( command + cmdLen, entry );
				cmdLen += len;

				numPlayers++;
			}

			if ( numPlayers > 0 ) {
				trap_SendServerCommand( clientNum, va("hinfo %i%s", numPlayers, command) );
			}
		}
	}

	//store intial client values
	//FIXME: when purposely change teams, this gets confused?

	G_Client_StoreClientInitialStatus( ent );

	//RPG-X: Marcin: stuff was here previously - 22/12/2008
}

/*
===========
G_Client_Disconnect

Called when a player drops from the server.
Will not be called between levels.

This should NOT be called directly by any game logic,
call trap_DropClient(), which will call this and do
server system housekeeping.
============
*/
void G_Client_Disconnect( int32_t clientNum ) {
	gentity_t*	ent = NULL;
	gentity_t*	tent = NULL;
	int32_t		i = 0;

	ent = &g_entities[clientNum];
	if ( (ent == NULL) || (ent->client == NULL) ) {
		return;
	}

	// stop any following clients
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].sess.sessionTeam == TEAM_SPECTATOR
			&& level.clients[i].sess.spectatorState == SPECTATOR_FOLLOW
			&& level.clients[i].sess.spectatorClient == clientNum ) {
			StopFollowing( &g_entities[i] );
		}
	}

	//RPG-X: J2J - Stop any dragging.
	DragDat[clientNum].AdminId = -1;
	DragDat[clientNum].distance = 0;
	g_entities[clientNum].client->noclip = qfalse;  

	//TiM: Log the player's IP and name.  If they reconnect again, it'll announce their deceipt >:)
	if ( (rpg_renamedPlayers.integer != 0) && ((ent->r.svFlags & SVF_BOT) == 0) ) {
		int32_t		l = 0;
		qboolean	foundName = qfalse;

		//Do a chek to see if this player has disconnected b4.  else we'll be wasting a slot.
		for ( ; l < MAX_RECON_NAMES; l++ ) {
			if ( g_reconData[l].ipAddress[0] == 0 ) {
				continue;
			}

			if ( Q_stricmp( ent->client->pers.ip, g_reconData[l].ipAddress ) == 0 ) {
				foundName=qtrue;
				break;
			}
		}

		if ( foundName ) {
			memset( &g_reconData[l], 0, sizeof( g_reconData[l] ) );
		
			//IP Address
			Q_strncpyz( g_reconData[l].ipAddress, ent->client->pers.ip, sizeof( g_reconData[l].ipAddress ) );
			//Player Name
			Q_strncpyz( g_reconData[l].previousName, ent->client->pers.netname, sizeof( g_reconData[l].previousName ) );
		} else {
			memset( &g_reconData[g_reconNum], 0, sizeof( g_reconData[g_reconNum] ) );

			//IP Address
			Q_strncpyz( g_reconData[g_reconNum].ipAddress, ent->client->pers.ip, sizeof( g_reconData[g_reconNum].ipAddress ) );
			//Player Name
			Q_strncpyz( g_reconData[g_reconNum].previousName, ent->client->pers.netname, sizeof( g_reconData[g_reconNum].previousName ) );
				
			g_reconNum++;
			//cap reconNum just in case.
			
			if ( g_reconNum >= MAX_RECON_NAMES ) {
				g_reconNum = 0;
			}
		}
	}

	// send effect if they were completely connected
	if ( (ent->client->pers.connected == CON_CONNECTED)
		&& (ent->client->sess.sessionTeam != TEAM_SPECTATOR) ) {
		vec3_t	org;

		VectorCopy( ent->client->ps.origin, org );
		org[2] += (ent->client->ps.viewheight >> 1);

		tent = G_TempEntity( org, EV_PLAYER_TELEPORT_OUT );
		tent->s.clientNum = ent->s.clientNum;

		// They don't get to take powerups with them!
		// Especially important for stuff like CTF flags
		TossClientItems ( ent, qtrue );
	}
	G_LogPrintf( "ClientDisconnect: %i (%s)\n", clientNum, g_entities[clientNum].client->pers.ip );

	// if we are playing in tourney mode and losing, give a win to the other player
	if ( (g_gametype.integer == GT_TOURNAMENT) && (level.intermissiontime == 0)
		&& (level.warmupTime == 0) && (level.sortedClients[1] == clientNum) ) {
		level.clients[ level.sortedClients[0] ].sess.wins++;
		G_Client_UserinfoChanged( level.sortedClients[0] );
	}

	if ( (g_gametype.integer == GT_TOURNAMENT) && (ent->client->sess.sessionTeam == TEAM_FREE) && (level.intermissiontime != 0) ) {
		trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
		level.restarted = qtrue;
		level.intermissiontime = 0;
	}

	trap_UnlinkEntity (ent);
	memset( ent, 0, sizeof( ent ) );
	ent->s.modelindex = 0;
	ent->inuse = qfalse;
	ent->classname = "disconnected";
	ent->client->pers.connected = CON_DISCONNECTED;
	ent->client->ps.persistant[PERS_TEAM] = TEAM_FREE;
	ent->client->ps.persistant[PERS_CLASS] = 0;//PC_NOCLASS;
	ent->client->sess.sessionTeam = TEAM_FREE;
	ent->client->sess.sessionClass = 0;//PC_NOCLASS;

	trap_SetConfigstring( CS_PLAYERS + clientNum, "");

	G_Client_CalculateRanks( qfalse );

	if ( (ent->r.svFlags & SVF_BOT) != 0 ) {
		BotAIShutdownClient( clientNum );
	}

	// kef -- if this guy contributed to any of our kills/deaths/weapons logs, clean 'em out
	G_ClearClientLog(clientNum);

	//also remove any initial data
	clientInitialStatus[clientNum].initialized = qfalse;
}

/*
================
IsAdmin
RPG-X | Phenix | 21/11/2004
================
*/
/**
*	Checks if player is an admin.
*	\param ent the player
*
*	\author Ubergames - Phenix
*	\date 21/11/2004
*/
qboolean IsAdmin( gentity_t *ent)
{
	if ( ent == NULL ) {
		return qfalse;
	}

	if ( ent->client == NULL ) {
		return qfalse;
	}

	if (( g_classData[ent->client->sess.sessionClass].isAdmin ) ||
		( ent->client->LoggedAsAdmin == qtrue ) ||
		( ent->client->LoggedAsDeveloper == qtrue )) {
			return qtrue;
	} else {
		return qfalse;
	}
}

/*
===========
G_Client_GetLocation

Report a location for the player. Uses placed nearby target_location entities
============
*/
static gentity_t* G_Client_GetLocation(gentity_t* ent) {
	gentity_t*		eloc = NULL;
	gentity_t*		best = NULL;
	double			bestlen = 0.0;
	double			len = 0.0;
	vec3_t			origin;

	bestlen = 3*8192.0*8192.0;

	VectorCopy( ent->r.currentOrigin, origin );

	for (eloc = level.locationHead; eloc; eloc = eloc->nextTrain) {
		len = ( origin[0] - eloc->r.currentOrigin[0] ) * ( origin[0] - eloc->r.currentOrigin[0] )
			+ ( origin[1] - eloc->r.currentOrigin[1] ) * ( origin[1] - eloc->r.currentOrigin[1] )
			+ ( origin[2] - eloc->r.currentOrigin[2] ) * ( origin[2] - eloc->r.currentOrigin[2] );

		if ( len > bestlen ) {
			continue;
		}

		if ( !trap_InPVS( origin, eloc->r.currentOrigin ) ) {
			continue;
		}

		bestlen = len;
		best = eloc;
	}

	return best;
}

/*
===========
G_Client_GetLocationMsg

Report a location for the player. Uses placed nearby target_location entities
============
*/
qboolean G_Client_GetLocationMsg(gentity_t *ent, char *loc, int32_t loclen)
{
	gentity_t* best = NULL;

	best = G_Client_GetLocation( ent );
	
	if (best == NULL) {
		return qfalse;
	}

	if (best->count != 0) {
		if (best->count < 0)
			best->count = 0;
		if (best->count > 7)
			best->count = 7;
		Com_sprintf(loc, loclen, "%c%c%s" S_COLOR_WHITE, Q_COLOR_ESCAPE, best->count + '0', best->message );
	} else {
		Com_sprintf(loc, loclen, "%s", best->message);
	}

	return qtrue;
}

/*
==================
G_Client_CheckHealthInfoMessage

Sends Health Changes to proper clients

Format:
	clientNum health

==================
*/
static void G_Client_CheckHealthInfoMessage( void ) {
	char		entry[1024];
	char		string[1400];
	int32_t		stringlength = 0;
	int32_t		i = 0;
	int32_t		j = 0;
	int32_t		t = 0;
	int32_t		sendToCnt = 0;
	int32_t		cnt = 0;
	int32_t		sentCnt = 0;
	int32_t		h = 0;
	int32_t		clients[MAX_CLIENTS];
	int32_t		sendToClients[MAX_CLIENTS];
	gentity_t*	player = NULL;
	gentity_t*	ent = NULL;

	memset(entry, 0, sizeof(entry));
	memset(string, 0, sizeof(string));
	memset(clients, 0, sizeof(clients));
	memset(sendToClients, 0, sizeof(sendToClients));

	//only send this to medics or spectators or adminz
	for ( ; i < g_maxclients.integer; i++)  {

		if ( (level.clients[i].pers.connected == CON_CONNECTED) && (level.clients[i].ps.stats[STAT_HEALTH] > 0) && //make sure they've actually spawned in already
			((level.clients[i].sess.sessionTeam == TEAM_SPECTATOR) || (g_classData[level.clients[i].sess.sessionClass].isMedical) || (g_classData[level.clients[i].sess.sessionClass].isAdmin) ) ) {
			sendToClients[sendToCnt++] = i;
		}
	}

	if ( sendToCnt == 0 ) {
		//no-one to send to
		return;
	}

	//only send those clients whose health has changed this cycle
	//NB: there's a prob with client 0 in here....
	for (i = 0, cnt = 0; i < g_maxclients.integer; i++)  {
		player = g_entities + i;
		if ( (player != NULL) && player->inuse && (player->old_health != player->health) && ( (player->health > 0) || (player->old_health > 0) )) {
			clients[cnt++] = i;
			player->old_health = player->health;
		}
	}

	if ( cnt == 0 ) {
		//no-one relevant changed health
		return;
	}

	for ( t = 0; t < sendToCnt; t++ ) {
		ent = g_entities + sendToClients[t];
		sentCnt = 0;

		// send the latest information on all clients
		string[0] = 0;
		stringlength = 0;

		for (i = 0; i < cnt; i++) {
			player = g_entities + clients[i];

			if ( ent == player ) {//don't send the ent his own health
				continue;
			}

			//send this one
			sentCnt++;

			h = player->health;
			if (h < 0) h = 0;

			Com_sprintf (entry, sizeof(entry), " %i %i", clients[i], h);
			j = strlen(entry);
			if (stringlength + j > sizeof(string))
				break;
			strcpy (string + stringlength, entry);
			stringlength += j;
		}

		if ( sentCnt != 0 ) {
			trap_SendServerCommand( sendToClients[t], va("hinfo %i%s", sentCnt, string) );
		}
	}
}

/**
 * Send client location information.
 *
 * \param ent The client.
 */
static void G_Client_LocationsMessage( gentity_t *ent ) {
	char		entry[1024];
	char		string[1400];
	int32_t		stringlength = 0;
	int32_t		i = 0;
	int32_t		j = 0;
	int32_t		cnt = 0;
	gentity_t*	player = NULL;
	
	//don't bother sending during intermission?
	if ( level.intermissiontime != 0 ) {
		return;
	}

	memset(entry, 0, sizeof(entry));
	memset(string, 0, sizeof(string));

	// figure out what client should be on the display
	// we are limited to 8, but we want to use the top eight players
	// but in client order (so they don't keep changing position on the overlay)
	for ( ; i < g_maxclients.integer && cnt < TEAM_MAXOVERLAY; i++) {
		player = g_entities + level.sortedClients[i];
		if ((player != NULL) && (player->inuse) && (player->client->sess.sessionTeam == ent->client->sess.sessionTeam) ) { 
			// TODO huh? remove?
		}
	}

	// send the latest information on all clients
	for (i = 0, cnt = 0; i < g_maxclients.integer && cnt < TEAM_MAXOVERLAY; i++) {
		player = g_entities + i;
		//RPG-X | Phenix | 05/03/2005
		if ((player != NULL) && player->inuse) {
			//to counter for the fact we could pwn the server doing this, remove all superfluous data

			Com_sprintf (entry, sizeof(entry), " %i %i ", i, player->client->pers.teamState.location);
			j = strlen(entry);
			if (stringlength + j > sizeof(string)) {
				break;
			}
			strcpy (string + stringlength, entry);
			stringlength += j;
			cnt++;
		}
	}

	trap_SendServerCommand( ent-g_entities, va("tinfo %i%s", cnt, string) );
}

//TiM - Modified to work with RPG-X
void G_Client_CheckClientStatus(void) {
	gentity_t*	loc = NULL;
	gentity_t*	ent = NULL;

	if (level.time - level.lastTeamLocationTime > TEAM_LOCATION_UPDATE_TIME) {
		int32_t i = 0;

		level.lastTeamLocationTime = level.time;

		for ( ; i < g_maxclients.integer; i++) {
			ent = g_entities + i;
			if ((ent != NULL) && (ent->inuse)) {
				loc = G_Client_GetLocation( ent );
				if (loc != NULL) {
					ent->client->pers.teamState.location = loc->health;
				} else {
					ent->client->pers.teamState.location = 0;
				}
			}
		}

		for (i = 0; i < g_maxclients.integer; i++) {
			ent = g_entities + i;
			if ((ent != NULL) && (ent->inuse)) {
				G_Client_LocationsMessage( ent );
			}
		}

		G_Client_CheckHealthInfoMessage();
	}
}

/*
============
G_Client_AddScore

Adds score to both the client and his team
============
*/
void G_Client_AddScore( gentity_t *ent, int score ) {
	if ( !ent )
	{
		return;
	}
	if ( !ent->client ) {
		return;
	}

	if(!ent->client->UpdateScore)
	{
		return;
	}

	ent->client->ps.persistant[PERS_SCORE] += score;
	//don't add score to team score during elimination
	if (g_gametype.integer == GT_TEAM)
	{//this isn't capture score
		level.teamScores[ ent->client->ps.persistant[PERS_TEAM] ] += score;
	}
	G_Client_CalculateRanks( qfalse );

	//RPG-X: RedTechie - Lets enable score updating without this scores will not be updated
	ent->client->UpdateScore = qfalse;
}

/*
=============
SortRanks

=============
*/
static int QDECL SortRanks( const void *a, const void *b ) {
	gclient_t	*ca, *cb;

	ca = &level.clients[*(int *)a];
	cb = &level.clients[*(int *)b];

	// sort special clients last
	if ( ca->sess.spectatorState == SPECTATOR_SCOREBOARD || ca->sess.spectatorClient < 0 ) {
		return 1;
	}
	if ( cb->sess.spectatorState == SPECTATOR_SCOREBOARD || cb->sess.spectatorClient < 0  ) {
		return -1;
	}

	// then connecting clients
	if ( ca->pers.connected == CON_CONNECTING ) {
		return 1;
	}
	if ( cb->pers.connected == CON_CONNECTING ) {
		return -1;
	}


	// then spectators
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR && cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		if ( ca->sess.spectatorTime < cb->sess.spectatorTime ) {
			return -1;
		}
		if ( ca->sess.spectatorTime > cb->sess.spectatorTime ) {
			return 1;
		}
		return 0;
	}
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR ) {
		return 1;
	}
	if ( cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		return -1;
	}

	// then sort by score & number of times killed
	if ( ca->ps.persistant[PERS_SCORE]
		> cb->ps.persistant[PERS_SCORE] ) {
		return -1;
	}
	if ((ca->ps.persistant[PERS_SCORE] == cb->ps.persistant[PERS_SCORE]) &&
		(ca->ps.persistant[PERS_KILLED] < cb->ps.persistant[PERS_KILLED])   )
	{	return -1;}

	if ( ca->ps.persistant[PERS_SCORE]
		< cb->ps.persistant[PERS_SCORE] ) {
		return 1;
	}
	if ((ca->ps.persistant[PERS_SCORE] == cb->ps.persistant[PERS_SCORE]) &&
		(ca->ps.persistant[PERS_KILLED] > cb->ps.persistant[PERS_KILLED])   )
	{	return 1;}

	return 0;
}

/*
============
G_Client_CalculateRanks

Recalculates the score ranks of all players
This will be called on every client connect, begin, disconnect, death,
and team change.

FIXME: for elimination, the last man standing must be ranked first
============
*/
void G_Client_CalculateRanks( qboolean fromExit ) {
	int32_t	i = 0;

	gclient_t	*cl;

	level.follow1 = -1;
	level.follow2 = -1;
	level.numConnectedClients = 0;
	level.numNonSpectatorClients = 0;
	level.numPlayingClients = 0;
	level.numVotingClients = 0;		// don't count bots
	for ( ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected != CON_DISCONNECTED ) {
			level.sortedClients[level.numConnectedClients] = i;
			level.numConnectedClients++;

			if ( level.clients[i].sess.sessionTeam != TEAM_SPECTATOR ) {
				level.numNonSpectatorClients++;

				// decide if this should be auto-followed
				if ( level.clients[i].pers.connected == CON_CONNECTED ) {
					level.numPlayingClients++;
					if ( (g_entities[i].r.svFlags & SVF_BOT) == 0 ) {
						level.numVotingClients++;
					}
					if ( level.follow1 == -1 ) {
						level.follow1 = i;
					} else if ( level.follow2 == -1 ) {
						level.follow2 = i;
					}
				}
			}
		}
	}

	qsort( level.sortedClients, (size_t)level.numConnectedClients,
		sizeof(level.sortedClients[0]), SortRanks );

	// set the rank value for all clients that are connected and not spectators
	if ( g_gametype.integer >= GT_TEAM ) {
		// in team games, rank is just the order of the teams, 0=red, 1=blue, 2=tied
		for ( i = 0;  i < level.numConnectedClients; i++ ) {
			cl = &level.clients[ level.sortedClients[i] ];
			if ( level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE] ) {
				cl->ps.persistant[PERS_RANK] = 2;
			} else if ( level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE] ) {
				cl->ps.persistant[PERS_RANK] = 0;
			} else {
				cl->ps.persistant[PERS_RANK] = 1;
			}
		}
	} else {
		int32_t rank = -1;
		int32_t score = 0;
		for ( i = 0;  i < level.numPlayingClients; i++ ) {
			int32_t newScore = 0;
			
			cl = &level.clients[ level.sortedClients[i] ];
			newScore = cl->ps.persistant[PERS_SCORE];
			if ( i == 0 || newScore != score ) {
				rank = i;
				// assume we aren't tied until the next client is checked
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank;
			} else {
				// we are tied with the previous client
				level.clients[ level.sortedClients[i-1] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
			}
			score = newScore;
			if ( g_gametype.integer == GT_SINGLE_PLAYER && level.numPlayingClients == 1 ) {
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
			}
		}
	}

	// set the CS_SCORES1/2 configstrings, which will be visible to everyone
	if ( g_gametype.integer >= GT_TEAM ) {
		trap_SetConfigstring( CS_SCORES1, va("%i", level.teamScores[TEAM_RED] ) );
		trap_SetConfigstring( CS_SCORES2, va("%i", level.teamScores[TEAM_BLUE] ) );
	} else {
		if ( level.numConnectedClients == 0 ) {
			trap_SetConfigstring( CS_SCORES1, va("%i", SCORE_NOT_PRESENT) );
			trap_SetConfigstring( CS_SCORES2, va("%i", SCORE_NOT_PRESENT) );
		} else if ( level.numConnectedClients == 1 ) {
			trap_SetConfigstring( CS_SCORES1, va("%i", level.clients[ level.sortedClients[0] ].ps.persistant[PERS_SCORE] ) );
			trap_SetConfigstring( CS_SCORES2, va("%i", SCORE_NOT_PRESENT) );
		} else {
			trap_SetConfigstring( CS_SCORES1, va("%i", level.clients[ level.sortedClients[0] ].ps.persistant[PERS_SCORE] ) );
			trap_SetConfigstring( CS_SCORES2, va("%i", level.clients[ level.sortedClients[1] ].ps.persistant[PERS_SCORE] ) );
		}
	}

	// if we are at the intermission, send the new info to everyone
	if ( level.intermissiontime != 0 ) {
		SendScoreboardMessageToAllClients();
	}
}

void G_Client_UpdateSoundZones(void) {
	int32_t i = 0;
	int32_t b = 0;
	int32_t entlist[MAX_GENTITIES];
	int32_t zones[MAX_CLIENTS];
	int32_t count = 0;
	char supdate[MAX_STRING_CHARS];
	
	memset(&zones, 0, sizeof(zones));
	memset(&supdate, 0, sizeof(supdate));

	for( ; i < MAX_GENTITIES; i++) {
		if((g_entities[i].type == ENT_TARGET_ZONE) && (g_entities[i].count == 3)) {
			memset(&entlist, 0, sizeof(entlist));
			count = trap_EntitiesInBox(g_entities[i].r.mins, g_entities[i].r.maxs, &entlist, MAX_GENTITIES);

			for(b = 0; b < count; b++) {
				if(g_entities[entlist[b]].client != NULL) {
					zones[g_entities[entlist[b]].client->ps.clientNum] = g_entities[i].s.number;
				}
			}
		}
	}

	for(i = 0; i < MAX_CLIENTS; i++) {
		if(strlen(supdate) != 0) {
			Com_sprintf(supdate, sizeof(supdate), "%s\\c%d\\%d", supdate, i, zones[i]);
		} else {
			Com_sprintf(supdate, sizeof(supdate), "cd%\\%d", i, zones[i]);
		}
	}

	for(i = 0; i < MAX_GENTITIES; i++) {
		if(g_entities[i].client != NULL) {
			trap_SendServerCommand(i, va("slup %s", supdate));
		}
	}
}
