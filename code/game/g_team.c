// Copyright (C) 1999-2000 Id Software, Inc.
//

#include "g_local.h"

const char *TeamName(int team)  {
	if (team==TEAM_SPECTATOR)
		return "SPECTATOR";
	return "FREE";
}

const char *OtherTeamName(int team) {
	if (team==TEAM_SPECTATOR)
		return "SPECTATOR";
	return "FREE";
}

const char *TeamColorString(int team) {
	if (team==TEAM_SPECTATOR)
		return S_COLOR_YELLOW;
	return S_COLOR_WHITE;
}

/*
===========
Team_GetLocation

Report a location for the player. Uses placed nearby target_location entities
============
*/
gentity_t *Team_GetLocation(gentity_t *ent)
{
	gentity_t		*eloc, *best;
	float			bestlen, len;
	vec3_t			origin;

	best = NULL;
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
Team_GetLocation

Report a location for the player. Uses placed nearby target_location entities
============
*/
qboolean Team_GetLocationMsg(gentity_t *ent, char *loc, int loclen)
{
	gentity_t *best;

	best = Team_GetLocation( ent );
	
	if (!best)
		return qfalse;

	if (best->count) {
		if (best->count < 0)
			best->count = 0;
		if (best->count > 7)
			best->count = 7;
		Com_sprintf(loc, loclen, "%c%c%s" S_COLOR_WHITE, Q_COLOR_ESCAPE, best->count + '0', best->message );
	} else
		Com_sprintf(loc, loclen, "%s", best->message);

	return qtrue;
}

/*---------------------------------------------------------------------------*/
/*
==================
CheckHealthInfoMessage

Sends Health Changes to proper clients

Format:
	clientNum health

==================
*/
void CheckHealthInfoMessage( void ) 
{
	char		entry[1024];
	char		string[1400];
	int			stringlength;
	int			i, j, t;
	gentity_t	*player, *ent;
	int			sendToCnt, cnt, sentCnt;
	int			h;
	int			clients[MAX_CLIENTS];
	int			sendToClients[MAX_CLIENTS];

	//only send this to medics or spectators or adminz
	for (i = 0, sendToCnt = 0; i < g_maxclients.integer; i++) 
	{

		if ( level.clients[i].pers.connected == CON_CONNECTED && level.clients[i].ps.stats[STAT_HEALTH] > 0 &&//make sure they've actually spawned in already
			(level.clients[i].sess.sessionTeam == TEAM_SPECTATOR || g_classData[level.clients[i].sess.sessionClass].isMedical || g_classData[level.clients[i].sess.sessionClass].isAdmin ) )
		{
			sendToClients[sendToCnt++] = i;
		}
	}

	if ( !sendToCnt )
	{//no-one to send to
		return;
	}

	//only send those clients whose health has changed this cycle
	//NB: there's a prob with client 0 in here....
	for (i = 0, cnt = 0; i < g_maxclients.integer; i++) 
	{
		player = g_entities + i;
		if ( player->inuse && player->old_health != player->health && ( player->health > 0 || player->old_health > 0 )) 
		{
			clients[cnt++] = i;
			player->old_health = player->health;
		}
	}

	if ( !cnt )
	{//no-one relevant changed health
		return;
	}

	for ( t = 0; t < sendToCnt; t++ )
	{
		ent = g_entities + sendToClients[t];
		sentCnt = 0;

		// send the latest information on all clients
		string[0] = 0;
		stringlength = 0;

		for (i = 0; i < cnt; i++) 
		{
			player = g_entities + clients[i];
			//RPG-X | Phenix | 05/03/2005
			/*if ( ent->client->sess.sessionTeam == player->client->sess.sessionTeam && ent->client->pers.teamInfo )
			{//already sent this player's health to this client because teaminfo is on
				continue;
			}*/

			if ( ent == player )
			{//don't send the ent his own health
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

		if ( sentCnt )
		{
			trap_SendServerCommand( sendToClients[t], va("hinfo %i%s", sentCnt, string) );
			//trap_SendServerCommand( -1, va( "print \"^1hinfo %i%s\n\"", sentCnt, string ));
		}
	}
}

/*
==================
TeamplayLocationsMessage

Format:
	clientNum location health armor weapon powerups

==================
*/
void TeamplayInfoMessage( gentity_t *ent ) {
	char		entry[1024];
	char		string[1400];
	int			stringlength;
	int			i, j;
	gentity_t	*player;
	int			cnt;
	
	//don't bother sending during intermission?
	if ( level.intermissiontime )
		return;

	// figure out what client should be on the display
	// we are limited to 8, but we want to use the top eight players
	// but in client order (so they don't keep changing position on the overlay)
	for (i = 0, cnt = 0; i < g_maxclients.integer && cnt < TEAM_MAXOVERLAY; i++) {
		player = g_entities + level.sortedClients[i];
		if (player->inuse && player->client->sess.sessionTeam == 
			ent->client->sess.sessionTeam ) {
		}
	}

	// send the latest information on all clients
	string[0] = 0;
	stringlength = 0;

	for (i = 0, cnt = 0; i < g_maxclients.integer && cnt < TEAM_MAXOVERLAY; i++) {
		player = g_entities + i;
//RPG-X | Phenix | 05/03/2005
		if (player->inuse) {
			//to counter for the fact we could pwn the server doing this, remove all superfluous data

			Com_sprintf (entry, sizeof(entry), " %i %i ", i, player->client->pers.teamState.location);
			j = strlen(entry);
			if (stringlength + j > sizeof(string))
				break;
			strcpy (string + stringlength, entry);
			stringlength += j;
			cnt++;
		}
	}

	trap_SendServerCommand( ent-g_entities, va("tinfo %i%s", cnt, string) );
}

//TiM - Modified to work with RPG-X
void CheckTeamStatus(void)
{
	int i;
	gentity_t *loc, *ent;

	if (level.time - level.lastTeamLocationTime > TEAM_LOCATION_UPDATE_TIME) {

		level.lastTeamLocationTime = level.time;

		for (i = 0; i < g_maxclients.integer; i++) {
			ent = g_entities + i;
			if (ent->inuse) {
				loc = Team_GetLocation( ent );
				if (loc)
					ent->client->pers.teamState.location = loc->health;
				else
					ent->client->pers.teamState.location = 0;
			}
		}

		for (i = 0; i < g_maxclients.integer; i++) {
			ent = g_entities + i;
			if (ent->inuse) {
				TeamplayInfoMessage( ent );
			}
		}

		CheckHealthInfoMessage();
	}
}



