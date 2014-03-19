// Copyright (C) 1999-2000 Id Software, Inc.
//
//
// g_arenas.c
//

#include "g_local.h"
#include "g_client.h"
#include "g_utils.h"

#ifndef min
#define min(a, b)	(a) < (b) ? a : b
#endif


gentity_t* podium1;
gentity_t* podium2;
gentity_t* podium3;

void UpdateTournamentInfo(void) {
	char		msg[AWARDS_MSG_LENGTH];
	char		msg2[AWARDS_MSG_LENGTH];
	char*		mvpName = "";
	int32_t		i = 0;
	int32_t		j = 0;
	int32_t		k = 0;
	int32_t		n = 0;
	int32_t		playerClientNum = 0;
	int32_t		playerRank = level.numNonSpectatorClients - 1;
	int32_t		highestTiedRank = 0;
	int32_t		mvpNum = -1;
	int32_t		mvpPoints = 0;
	int32_t		winningCaptures = 0;
	int32_t		winningPoints = 0;
	int32_t		winningTeam=0;
	int32_t		loseCaptures = 0;
	int32_t		losePoints = 0;
	int32_t		secondPlaceTied=0;
	gentity_t*	MVP = NULL;
	gentity_t*	player = NULL;
	gclient_t*	cl = NULL;
	gclient_t*	cl2= NULL;

	memset(msg, 0, AWARDS_MSG_LENGTH);
	memset(msg2, 0, AWARDS_MSG_LENGTH);

	// Was there a tie for second place on the podium?
	cl = &level.clients[level.sortedClients[1]];
	cl2= &level.clients[level.sortedClients[2]];
	if(cl->ps.persistant[PERS_SCORE] == cl2->ps.persistant[PERS_SCORE]) {
		secondPlaceTied = 1;
	}

	winningTeam = level.clients[0].ps.persistant[PERS_RANK]+1;
	if((winningTeam != TEAM_BLUE) && (winningTeam != TEAM_RED)) {
		//tie or not a team game
		winningTeam = 0;
	}

	if(mvpNum < 0) {
		//ah, crap no MVP, pick the first player on the winning team
		for(i = 0; i < level.maxclients; i++) {
			if((&g_entities[i]) == NULL) { continue; }
			if((&g_entities[i])->client == NULL) { continue; }
			if(g_entities[i].client->ps.persistant[PERS_TEAM] == winningTeam ) {
				mvpNum = i;
				break;
			}
		}
	}

	if(mvpNum >= 0) {
		//still no MVP, so skip it
		MVP = &g_entities[mvpNum];
		mvpName = MVP->client->pers.netname;
		mvpPoints = MVP->client->ps.persistant[PERS_SCORE];
		winningTeam = MVP->client->ps.persistant[PERS_TEAM];
	}

	if(winningTeam != 0)	{
		//one of the teams won
		winningCaptures = level.teamScores[winningTeam];

		if(winningTeam == TEAM_RED) {
			loseCaptures = level.teamScores[TEAM_BLUE];
		} else {
			loseCaptures = level.teamScores[TEAM_RED];
		}

		for(i = 0; i < level.maxclients; i++) {
			if((&g_entities[i]) == NULL) { continue; }
			if((&g_entities[i])->client == NULL) { continue; }
			if(g_entities[i].client->ps.persistant[PERS_TEAM] == winningTeam) {
				winningPoints += g_entities[i].client->ps.persistant[PERS_SCORE];
			} else {
				losePoints += g_entities[i].client->ps.persistant[PERS_SCORE];
			}
		}
	}

	for(i = 0; i < level.maxclients; i++) {
		player = &g_entities[i];

		if ((player == NULL) || (player->inuse == qfalse) || ((player->r.svFlags & SVF_BOT) != 0)) {
			continue;
		}
		playerClientNum = i;

		G_Client_CalculateRanks( qfalse );
		// put info for the top three players into the msg
		Com_sprintf(msg, AWARDS_MSG_LENGTH, "awards %d", level.numNonSpectatorClients);
		for(j = 0; j < level.numNonSpectatorClients; j++) {
			if(j > 2) {
				break;
			}
			n = level.sortedClients[j];
			strcpy(msg2, msg);
			Com_sprintf(msg, AWARDS_MSG_LENGTH, "%s %d", msg2, n);
		}

		// put this guy's awards into the msg
		if(level.clients[playerClientNum].sess.sessionTeam == TEAM_SPECTATOR) {
			strcpy(msg2, msg);
			Com_sprintf( msg, sizeof(msg), "%s 0", msg2);
		}

		// now supply...
		//
		// 1) winning team's MVP's name
		// 2) winning team's MVP's score
		// 3) winning team's total captures
		// 4) winning team's total points
		// 5) this player's rank
		// 6) the highest rank for which this player tied
		// 7) losing team's total captures
		// 8) losing team's total points
		// 9) if second place was tied
		// 10) intermission point
		// 11) intermission angles
		//
		for(k = 0; k < level.numNonSpectatorClients; k++) {
			if(level.sortedClients[k] == playerClientNum) {
				playerRank = k;
				break;
			}
		}

		highestTiedRank = 0;
		for(k = playerRank-1; k >= 0; k--) {
			cl = &level.clients[level.sortedClients[k]];
			if (cl->ps.persistant[PERS_SCORE] > level.clients[level.sortedClients[playerRank]].ps.persistant[PERS_SCORE]) {
				break;
			}
			highestTiedRank = k+1;
		}
		strcpy(msg2, msg);
		Com_sprintf(msg, AWARDS_MSG_LENGTH, "%s \"%s\" %d %d %d %d %d %d %d %d %f %f %f %f %f %f",
			msg2, mvpName, mvpPoints, winningCaptures, winningPoints, playerRank, highestTiedRank,
			loseCaptures, losePoints, secondPlaceTied, level.intermission_origin[0], level.intermission_origin[1],
			level.intermission_origin[2], level.intermission_angle[0], level.intermission_angle[1],
			level.intermission_angle[2]);

		trap_SendServerCommand(player-g_entities, msg);
	}

	if(g_gametype.integer == GT_SINGLE_PLAYER) {
		Com_sprintf( msg, sizeof(msg), "postgame %i", playerRank);
		trap_SendConsoleCommand( EXEC_APPEND, msg); 
	}
}


static gentity_t* SpawnModelOnVictoryPad(gentity_t* pad, vec3_t offset, gentity_t* ent, int32_t place) {
	gentity_t*		body;
	vec3_t			vec;
	vec3_t			f;
	vec3_t			r;
	vec3_t			u;
	entityState_t*	eState;
	entityShared_t*	eShared;

	body = G_Spawn();
	if(body == NULL) {
		G_Printf( S_COLOR_RED "ERROR: out of gentities\n" );
		return NULL;
	}

	eState = &body->s;
	eShared = &body->r;

	body->classname = ent->client->pers.netname;
	body->client = ent->client;
	eState = &ent->s;
	eState->eType = ET_PLAYER;		// could be ET_INVISIBLE
	eState->eFlags = 0;				// clear EF_TALK, etc
	eState->powerups = 0;			// clear powerups
	eState->loopSound = 0;			// clear lava burning
	eState->number = body - g_entities;
	body->timestamp = level.time;
	body->physicsObject = qtrue;
	body->physicsBounce = 0;		// don't bounce
	eState->event = 0;
	eState->pos.trType = TR_STATIONARY;
	eState->groundEntityNum = ENTITYNUM_WORLD;
	eState->legsAnim = BOTH_STAND1; //TORSO_STAND
	eState->torsoAnim = BOTH_STAND1;

	// fix up some weapon holding / shooting issues
	if (eState->weapon==WP_5 || eState->weapon==WP_13 || eState->weapon == WP_0 ) {
		eState->weapon = WP_6;
	}

	eState->event = 0;
	eShared->svFlags = ent->r.svFlags;
	VectorCopy (ent->r.mins, eShared->mins);
	VectorCopy (ent->r.maxs, eShared->maxs);
	VectorCopy (ent->r.absmin, eShared->absmin);
	VectorCopy (ent->r.absmax, eShared->absmax);
	body->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	eShared->contents = CONTENTS_BODY;
	eShared->ownerNum = ent->r.ownerNum;
	body->takedamage = qfalse;
	VectorSubtract( level.intermission_origin, pad->r.currentOrigin, vec );
	vectoangles( vec, eState->apos.trBase );
	eState->apos.trBase[PITCH] = 0;
	eState->apos.trBase[ROLL] = 0;

	AngleVectors( eState->apos.trBase, f, r, u );
	VectorMA( pad->r.currentOrigin, offset[0], f, vec );
	VectorMA( vec, offset[1], r, vec );
	VectorMA( vec, offset[2], u, vec );

	G_SetOrigin( body, vec );

	trap_LinkEntity (body);

	body->count = place;

	return body;
}


static void CelebrateStop(gentity_t* player) {
	int32_t	anim;

	anim = BOTH_STAND1; //TORSO_STAND
	player->s.torsoAnim = ( ( player->s.torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;
}


#define	TIMER_GESTURE	(34*66+50)
extern void	BG_AddPredictableEventToPlayerstate(int32_t newEvent, int32_t eventParm, playerState_t* ps);
static void CelebrateStart(gentity_t* player) 
{
	/*player->s.torsoAnim = ( ( player->s.torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | TORSO_GESTURE;
	player->nextthink = level.time + TIMER_GESTURE;
	player->think = CelebrateStop;*/

	//	We don't want the taunt sound effect because it interfears with the computer voice giving awards
	//	FIXME: just get timing right?
	//FIXME: why does this get lost now?
	BG_AddPredictableEventToPlayerstate( EV_TAUNT, 0, &player->client->ps );
}


static vec3_t	offsetFirst  = {0, 0, 64};
static vec3_t	offsetSecond = {-10, 60, 44};
static vec3_t	offsetThird  = {-19, -60, 35};

static void PodiumPlacementThink(gentity_t* podium) {
	vec3_t		vec;
	vec3_t		origin;
	vec3_t		f;
	vec3_t		r;
	vec3_t		u;

	podium->nextthink = level.time + 100;

	AngleVectors( level.intermission_angle, vec, NULL, NULL );
	VectorMA( level.intermission_origin, trap_Cvar_VariableIntegerValue( "g_podiumDist" ), vec, origin );
	origin[2] -= trap_Cvar_VariableIntegerValue( "g_podiumDrop" );
	G_SetOrigin( podium, origin );

	if(podium1 != NULL) {
		VectorSubtract( level.intermission_origin, podium->r.currentOrigin, vec );
		vectoangles( vec, podium1->s.apos.trBase );
		podium1->s.apos.trBase[PITCH] = 0;
		podium1->s.apos.trBase[ROLL] = 0;

		AngleVectors( podium1->s.apos.trBase, f, r, u );
		VectorMA( podium->r.currentOrigin, offsetFirst[0], f, vec );
		VectorMA( vec, offsetFirst[1], r, vec );
		VectorMA( vec, offsetFirst[2], u, vec );

		G_SetOrigin( podium1, vec );
	}

	if(podium2 != NULL) {
		VectorSubtract( level.intermission_origin, podium->r.currentOrigin, vec );
		vectoangles( vec, podium2->s.apos.trBase );
		podium2->s.apos.trBase[PITCH] = 0;
		podium2->s.apos.trBase[ROLL] = 0;

		AngleVectors( podium2->s.apos.trBase, f, r, u );
		VectorMA( podium->r.currentOrigin, offsetSecond[0], f, vec );
		VectorMA( vec, offsetSecond[1], r, vec );
		VectorMA( vec, offsetSecond[2], u, vec );

		G_SetOrigin( podium2, vec );
	}

	if(podium3 != NULL) {
		VectorSubtract( level.intermission_origin, podium->r.currentOrigin, vec );
		vectoangles( vec, podium3->s.apos.trBase );
		podium3->s.apos.trBase[PITCH] = 0;
		podium3->s.apos.trBase[ROLL] = 0;

		AngleVectors( podium3->s.apos.trBase, f, r, u );
		VectorMA( podium->r.currentOrigin, offsetThird[0], f, vec );
		VectorMA( vec, offsetThird[1], r, vec );
		VectorMA( vec, offsetThird[2], u, vec );

		G_SetOrigin( podium3, vec );
	}
}


static gentity_t* SpawnPodium( void ) {
	gentity_t*	podium;
	vec3_t		vec;
	vec3_t		origin;

	podium = G_Spawn();
	if(podium == NULL) {
		return NULL;
	}

	podium->classname = "podium";
	podium->s.eType = ET_GENERAL;
	podium->s.number = podium - g_entities;
	podium->clipmask = CONTENTS_SOLID;
	podium->r.contents = CONTENTS_SOLID;
	if(g_gametype.integer > GT_SINGLE_PLAYER) {
		podium->s.modelindex = G_ModelIndex( TEAM_PODIUM_MODEL );
	} else {
		podium->s.modelindex = G_ModelIndex( SP_PODIUM_MODEL );
	}

	AngleVectors( level.intermission_angle, vec, NULL, NULL );
	VectorMA( level.intermission_origin, trap_Cvar_VariableIntegerValue( "g_podiumDist" ), vec, origin );
	origin[2] -= trap_Cvar_VariableIntegerValue( "g_podiumDrop" );
	G_SetOrigin( podium, origin );

	VectorSubtract( level.intermission_origin, podium->r.currentOrigin, vec );
	podium->s.apos.trBase[YAW] = vectoyaw( vec );
	trap_LinkEntity (podium);

	podium->think = PodiumPlacementThink;
	podium->nextthink = level.time + 100;
	return podium;
}

void SpawnModelsOnVictoryPads( void ) {
	gentity_t* player;
	gentity_t* podium;

	podium1 = NULL;
	podium2 = NULL;
	podium3 = NULL;

	podium = SpawnPodium();


	player = SpawnModelOnVictoryPad( podium, offsetFirst, &g_entities[level.sortedClients[0]],
		level.clients[ level.sortedClients[0] ].ps.persistant[PERS_RANK] &~ RANK_TIED_FLAG );

	if (player != NULL) {
		player->nextthink = level.time + 2000;
		player->think = CelebrateStart;
		podium1 = player;
	}

	// For non team game types, we want to spawn 3 characters on the victory pad
	// For team games (GT_TEAM, GT_CTF) we want to have only a single player on the pad
	if(( g_gametype.integer == GT_FFA ) || (g_gametype.integer == GT_TOURNAMENT) || (g_gametype.integer == GT_SINGLE_PLAYER)) {
		if( level.numNonSpectatorClients > 1 ) {
			player = SpawnModelOnVictoryPad( podium, offsetSecond, &g_entities[level.sortedClients[1]],
				level.clients[ level.sortedClients[1] ].ps.persistant[PERS_RANK] &~ RANK_TIED_FLAG );
			if(player != NULL) {
				podium2 = player;
			}
		}

		if( level.numNonSpectatorClients > 2 ) {
			player = SpawnModelOnVictoryPad( podium, offsetThird, &g_entities[level.sortedClients[2]],
				level.clients[ level.sortedClients[2] ].ps.persistant[PERS_RANK] &~ RANK_TIED_FLAG );
			if(player  != NULL) {
				podium3 = player;
			}
		}
	}
}

void Svcmd_AbortPodium_f( void ) {
	if( g_gametype.integer != GT_SINGLE_PLAYER ) {
		return;
	}

	if( podium1 != NULL ) {
		podium1->nextthink = level.time;
		podium1->think = CelebrateStop;
	}
}
