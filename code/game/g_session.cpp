// Copyright (C) 1999-2000 Id Software, Inc.
//
#include "g_local.h"
#include "g_main.h"
#include "g_syscalls.h"


/*
=======================================================================

  SESSION DATA

Session data is the only data that stays persistant across level loads
and tournament restarts.
=======================================================================
*/

/*
================
G_WriteClientSessionData

Called on game shutdown
================
*/
static void G_WriteClientSessionData( gclient_t *client ) {
	const char	*s;
	const char	*var;

	s = va("%i %i %i %i %i %i %i", 
		client->sess.sessionTeam,
		client->sess.sessionClass,
		client->sess.spectatorTime,
		client->sess.spectatorState,
		client->sess.spectatorClient,
		client->sess.wins,
		client->sess.losses
		);

	var = va( "session%i", client - level.clients );

	trap_Cvar_Set( var, s );
}

void G_ReadSessionData( gclient_t *client ) {
	char	s[MAX_STRING_CHARS];
	const char	*var;
	int team, spec;

	var = va( "session%i", client - level.clients );
	trap_Cvar_VariableStringBuffer( var, s, sizeof(s) );

	sscanf( s, "%i %i %i %i %i %i %i", 
		&team,
		&client->sess.sessionClass,
		&client->sess.spectatorTime,
		&spec,
		&client->sess.spectatorClient,
		&client->sess.wins,
		&client->sess.losses
		);
	client->sess.sessionTeam = (team_t)team;
	client->sess.spectatorState = (spectatorState_t)spec;
}

void G_InitSessionData( gclient_t *client, char *userinfo ) {
	clientSession_t	*sess;
	const char		*value;

	sess = &client->sess;

	value = Info_ValueForKey( userinfo, "team" );
	if ( value[0] == 's' ) {
		// a willing spectator, not a waiting-in-line
		sess->sessionTeam = TEAM_SPECTATOR;
	} else {
		switch ( g_gametype.integer ) {
		default:
		case GT_FFA:
			sess->sessionTeam = TEAM_FREE;
			break;
		case GT_SINGLE_PLAYER:
			sess->sessionTeam = TEAM_FREE;
			break;
		case GT_TOURNAMENT:
			// if the game is full, go into a waiting mode
			if ( level.numNonSpectatorClients >= 2 ) {
				sess->sessionTeam = TEAM_SPECTATOR;
			} else {
				sess->sessionTeam = TEAM_FREE;
			}
			break;
		}
	}

	sess->sessionClass = 0; //PC_NOCLASS; //TiM: Default Class
	sess->spectatorState = SPECTATOR_FREE;
	sess->spectatorTime = level.time;

	G_WriteClientSessionData( client );
}

void G_InitWorldSession( void ) {
	char	s[MAX_STRING_CHARS];
	int			gt;

	trap_Cvar_VariableStringBuffer( "session", s, sizeof(s) );
	gt = atoi( s );
	
	// if the gametype changed since the last session, don't use any
	// client sessions
	if ( g_gametype.integer != gt ) {
		level.newSession = qtrue;
		G_Printf( "Gametype changed, clearing session data.\n" );
	}
}

void G_WriteSessionData( void ) {
	int		i;

	trap_Cvar_Set( "session", va("%i", g_gametype.integer) );

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected == CON_CONNECTED ) {
			G_WriteClientSessionData( &level.clients[i] );
		}
	}
}
