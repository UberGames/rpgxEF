// Copyright (C) 1999-2000 Id Software, Inc.
//
// g_bot.c

#include "g_local.h"
#include "g_main.h"
#include "g_client.h"
#include "g_syscalls.h"
#include "ai_main.h"


static int32_t	g_numBots;
static char*	g_botInfos[MAX_BOTS];


int32_t			g_numArenas;
static char*	g_arenaInfos[MAX_ARENAS];


static const uint16_t BOT_BEGIN_DELAY_BASE = 2000;
static const uint16_t BOT_BEGIN_DELAY_INCREMENT = 2000;
static const uint8_t BOT_SPAWN_QUEUE_DEPTH = 16;

typedef struct {
	int32_t	clientNum;
	int32_t	spawnTime;
} botSpawnQueue_t;

static int32_t			botBeginDelay;
static botSpawnQueue_t	botSpawnQueue[BOT_SPAWN_QUEUE_DEPTH];

vmCvar_t bot_minplayers;

extern gentity_t* podium1;
extern gentity_t* podium2;
extern gentity_t* podium3;


/*
===============
G_ParseInfos
===============
*/
int32_t G_ParseInfos( char* buf, int32_t max, char* infos[] ) {
	char*	token = NULL;
	int32_t	count = 0;
	char	key[MAX_TOKEN_CHARS];
	char	info[MAX_INFO_STRING];

	count = 0;

	while ( qtrue ) {
		token = COM_Parse( &buf );
		if ( token == NULL || token[0] == 0 ) {
			break;
		}

		if ( strcmp( token, "{" ) != 0 ) {
			Com_Printf( "Missing { in info file\n" );
			break;
		}

		if ( count == max ) {
			Com_Printf( "Max infos exceeded\n" );
			break;
		}

		info[0] = '\0';
		while ( qtrue ) {
			token = COM_ParseExt( &buf, qtrue );
			if ( token == NULL || token[0] == 0 ) {
				Com_Printf( "Unexpected end of info file\n" );
				break;
			}

			if ( strcmp( token, "}" ) == 0 ) {
				break;
			}
			strncpy(key, token, sizeof(key));

			token = COM_ParseExt( &buf, qfalse );
			if ( token == NULL || token[0] == 0 ) {
				strcpy( token, "<NULL>" );
			}
			Info_SetValueForKey( info, key, token );
		}
		//NOTE: extra space for arena number
		infos[count] = G_Alloc(strlen(info) + strlen("\\num\\") + strlen(va("%d", MAX_ARENAS)) + 1);
		if (infos[count] != NULL) {
			strcpy(infos[count], info);
			count++;
		}
	}
	return count;
}

/*
===============
G_LoadArenasFromFile
===============
*/
static void G_LoadArenasFromFile( char* filename ) {
	int32_t			len = 0;
	fileHandle_t	f = 0;
	char			buf[MAX_ARENAS_TEXT];

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( f == 0 ) {
		trap_Printf( va( S_COLOR_RED "file not found: %s\n", filename ) );
		return;
	}

	if ( len >= MAX_ARENAS_TEXT ) {
		trap_Printf( va( S_COLOR_RED "file too large: %s is %i, max allowed is %i", filename, len, MAX_ARENAS_TEXT ) );
		trap_FS_FCloseFile( f );
		return;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );

	g_numArenas += G_ParseInfos( buf, MAX_ARENAS - g_numArenas, &g_arenaInfos[g_numArenas] );
}

/*
===============
G_LoadArenas
===============
*/
static void G_LoadArenas( void ) {
	int32_t		numdirs = 0;
	vmCvar_t	arenasFile;
	char		filename[128];
	char		dirlist[4096];
	char*		dirptr = NULL;
	int32_t		i = 0;
	int32_t		n = 0;
	int32_t		dirlen = 0;

	g_numArenas = 0;

	trap_Cvar_Register( &arenasFile, "g_arenasFile", "", CVAR_INIT|CVAR_ROM );
	if( *arenasFile.string ) {
		G_LoadArenasFromFile(arenasFile.string);
	} else {
		G_LoadArenasFromFile("scripts/arenas.txt");
	}

	// get all arenas from .arena files
	numdirs = trap_FS_GetFileList("scripts", ".arena", dirlist, sizeof(dirlist) );
	dirptr  = dirlist;
	for (i = 0; i < numdirs; i++, dirptr += dirlen+1)
	{
		dirlen = strlen(dirptr);
		strcpy(filename, "scripts/");
		strcat(filename, dirptr);
		G_LoadArenasFromFile(filename);
	}
	trap_Printf( va( "%i arenas parsed\n", g_numArenas ) );
	
	for( n = 0; n < g_numArenas; n++ ) {
		Info_SetValueForKey( g_arenaInfos[n], "num", va( "%i", n ) );
	}
}


/*
===============
G_GetArenaInfoByNumber
===============
*/
const char* G_GetArenaInfoByMap( const char* map ) {
	int32_t	n = 0;

	for( n = 0; n < g_numArenas; n++ ) {
		if( Q_stricmp( Info_ValueForKey( g_arenaInfos[n], "map" ), map ) == 0 ) {
			return g_arenaInfos[n];
		}
	}

	return NULL;
}


/*
=================
PlayerIntroSound
=================
*/
static void PlayerIntroSound( const char* modelAndSkin ) {
	char	model[MAX_QPATH];
	char*	skin = NULL;

	strncpy(model, modelAndSkin, sizeof(model));
	skin = strchr(model, '/');
	if ( skin != NULL ) {
		*skin++ = '\0';
	} else {
		skin = model;
	}

	if( Q_stricmp( skin, "default" ) == 0 ) {
		skin = model;
	}
	if( Q_stricmp( skin, "red" ) == 0 ) {
		skin = model;
	}
	if( Q_stricmp( skin, "blue" ) == 0 ) {
		skin = model;
	}
	//precached in cg_info.c, CG_LoadingClient
	trap_SendConsoleCommand( EXEC_APPEND, va( "play sound/voice/computer/misc/%s.wav\n", skin ) );
}

/*
===============
G_AddRandomBot
===============
*/
void G_AddRandomBot( int32_t team ) {
	int32_t		i = 0;
	int32_t		n = 0;
	int32_t		num = 0;
	int32_t		skill = 0;
	char*		value = NULL;
	char		netname[36];
	char*		teamstr = NULL;
	gclient_t*	cl = NULL;

	num = 0;
	for ( n = 0; n < g_numBots ; n++ ) {
		value = Info_ValueForKey( g_botInfos[n], "name" );
		//
		for ( i=0 ; i< g_maxclients.integer ; i++ ) {
			cl = level.clients + i;
			if ( cl->pers.connected != CON_CONNECTED ) {
				continue;
			}
			if ( (g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT) == 0 ) {
				continue;
			}
			if ( team >= 0 && cl->sess.sessionTeam != team ) {
				continue;
			}
			if ( Q_stricmp( value, cl->pers.netname ) == 0 ) {
				break;
			}
		}
		if (i >= g_maxclients.integer) {
			num++;
		}
	}
	num = random() * num;
	for ( n = 0; n < g_numBots ; n++ ) {
		value = Info_ValueForKey( g_botInfos[n], "name" );
		//
		for ( i=0 ; i< g_maxclients.integer ; i++ ) {
			cl = level.clients + i;
			if ( cl->pers.connected != CON_CONNECTED ) {
				continue;
			}
			if ( (g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT) == 0 ) {
				continue;
			}
			if ( team >= 0 && cl->sess.sessionTeam != team ) {
				continue;
			}
			if ( Q_stricmp( value, cl->pers.netname ) == 0 ) {
				break;
			}
		}
		if (i >= g_maxclients.integer) {
			num--;
			if (num <= 0) {
				skill = trap_Cvar_VariableIntegerValue( "g_spSkill" );

				if (team == TEAM_RED) {
					teamstr = "red";
				} else if (team == TEAM_BLUE) { 
					teamstr = "blue";
				} else {
					teamstr = "";
				}

				strncpy(netname, value, sizeof(netname)-1);
				netname[sizeof(netname)-1] = '\0';
				Q_CleanStr(netname);
				trap_SendConsoleCommand( EXEC_INSERT, va("addbot %s %i %s %i\n", netname, skill, teamstr, 0) );

				return;
			}
		}
	}
}

/*
===============
G_RemoveRandomBot
===============
*/
qboolean G_RemoveRandomBot( int32_t team ) {
	int32_t		i = 0;
	char		netname[36];
	gclient_t*	cl = NULL;

	for ( i=0 ; i< g_maxclients.integer ; i++ ) {
		cl = level.clients + i;

		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}

		if ( !(g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT) ) {
			continue;
		}

		if ( team >= 0 && cl->sess.sessionTeam != team ) {
			continue;
		}

		strcpy(netname, cl->pers.netname);
		Q_CleanStr(netname);
		trap_SendConsoleCommand( EXEC_INSERT, va("kick \"%s\"\n", netname) );

		return qtrue;
	}
	return qfalse;
}

/*
===============
G_CountHumanPlayers
===============
*/
int32_t G_CountHumanPlayers( int32_t team ) {
	int32_t		i = 0; 
	int32_t		num = 0;
	gclient_t*	cl = NULL;

	num = 0;
	for ( i=0 ; i< g_maxclients.integer ; i++ ) {
		cl = level.clients + i;

		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}

		if ( g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT ) {
			continue;
		}

		if ( team >= 0 && cl->sess.sessionTeam != team ) {
			continue;
		}
		num++;
	}

	return num;
}

/*
===============
G_CountBotPlayers
===============
*/
int32_t G_CountBotPlayers( int32_t team ) {
	int32_t		i = 0;
	int32_t		n = 0;
	int32_t		num = 0;
	gclient_t*	cl = NULL;

	for ( ; i< g_maxclients.integer ; i++ ) {
		cl = level.clients + i;

		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}

		if ( (g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT) == 0 ) {
			continue;
		}

		if ( team >= 0 && cl->sess.sessionTeam != team ) {
			continue;
		}

		num++;
	}

	for( ; n < BOT_SPAWN_QUEUE_DEPTH; n++ ) {
		if( botSpawnQueue[n].spawnTime == 0 ) {
			continue;
		}
		if ( botSpawnQueue[n].spawnTime > level.time ) {
			continue;
		}
		num++;
	}
	return num;
}

/*
===============
G_CheckMinimumPlayers
===============
*/
void G_CheckMinimumPlayers( void ) {
	int32_t		minplayers = 0;
	int32_t		humanplayers = 0;
	int32_t		botplayers = 0;
	static int32_t checkminimumplayers_time = 0;

	if(level.intermissiontime != 0) {
		return;
	}

	//only check once each 10 seconds
	if (checkminimumplayers_time > level.time - 10000) {
		return;
	}

	checkminimumplayers_time = level.time;
	trap_Cvar_Update(&bot_minplayers);
	minplayers = bot_minplayers.integer;
	if (minplayers <= 0) {
		return;
	}

	if (g_gametype.integer != GT_TOURNAMENT) {
		botplayers = G_CountBotPlayers( TEAM_SPECTATOR );
		if ( botplayers > 0 ) {
			G_RemoveRandomBot( TEAM_SPECTATOR );
		}
	}

	if (g_gametype.integer >= GT_TEAM) {
		if (minplayers >= g_maxclients.integer / 2) {
			minplayers = (g_maxclients.integer / 2) -1;
		}
		humanplayers = G_CountHumanPlayers( TEAM_RED );
		botplayers = G_CountBotPlayers( TEAM_RED );
		//
		if (humanplayers + botplayers < minplayers) {
			G_AddRandomBot( TEAM_RED );
		} else if (humanplayers + botplayers > minplayers && botplayers) {
			G_RemoveRandomBot( TEAM_RED );
		}
		//
		humanplayers = G_CountHumanPlayers( TEAM_BLUE );
		botplayers = G_CountBotPlayers( TEAM_BLUE );
		//
		if (humanplayers + botplayers < minplayers) {
			G_AddRandomBot( TEAM_BLUE );
		} else if (humanplayers + botplayers > minplayers && botplayers) {
			G_RemoveRandomBot( TEAM_BLUE );
		}
	} else if (g_gametype.integer == GT_TOURNAMENT) {
		if (minplayers >= g_maxclients.integer) {
			minplayers = g_maxclients.integer-1;
		}
		humanplayers = G_CountHumanPlayers( -1 );
		botplayers = G_CountBotPlayers( -1 );
		//
		if (humanplayers + botplayers < minplayers) {
			G_AddRandomBot( TEAM_FREE );
		} else if (humanplayers + botplayers > minplayers && botplayers) {
			// try to remove spectators first
			if (!G_RemoveRandomBot( TEAM_SPECTATOR )) {
				// just remove the bot that is playing
				G_RemoveRandomBot( -1 );
			}
		}
	} else if (g_gametype.integer == GT_FFA) {
		if (minplayers >= g_maxclients.integer) {
			minplayers = g_maxclients.integer-1;
		}
		humanplayers = G_CountHumanPlayers( TEAM_FREE );
		botplayers = G_CountBotPlayers( TEAM_FREE );
		//
		if (humanplayers + botplayers < minplayers) {
			G_AddRandomBot( TEAM_FREE );
		} else if (humanplayers + botplayers > minplayers && botplayers) {
			G_RemoveRandomBot( TEAM_FREE );
		}
	}
}

/*
===============
G_CheckBotSpawn
===============
*/
void G_CheckBotSpawn( void ) {
	int32_t	n = 0;
	char	userinfo[MAX_INFO_VALUE];

	G_CheckMinimumPlayers();

	for( ; n < BOT_SPAWN_QUEUE_DEPTH; n++ ) {
		if( botSpawnQueue[n].spawnTime == 0 ) {
			continue;
		}

		if ( botSpawnQueue[n].spawnTime > level.time ) {
			continue;
		}

		G_Client_Begin( botSpawnQueue[n].clientNum, qfalse, qtrue, qfalse );
		botSpawnQueue[n].spawnTime = 0;

		if( g_gametype.integer == GT_SINGLE_PLAYER ) {
			trap_GetUserinfo( botSpawnQueue[n].clientNum, userinfo, sizeof(userinfo) );
			PlayerIntroSound( Info_ValueForKey (userinfo, "model") );
		}
	}
}


/*
===============
AddBotToSpawnQueue
===============
*/
static void AddBotToSpawnQueue( int32_t clientNum, int32_t delay ) {
	int32_t	n = 0;

	for( ; n < BOT_SPAWN_QUEUE_DEPTH; n++ ) {
		if( botSpawnQueue[n].spawnTime == 0 ) {
			botSpawnQueue[n].spawnTime = level.time + delay;
			botSpawnQueue[n].clientNum = clientNum;
			return;
		}
	}

	G_Printf( S_COLOR_YELLOW "Unable to delay spawn\n" );
	G_Client_Begin( clientNum, qfalse, qtrue, qfalse );
}


/*
===============
G_QueueBotBegin
===============
*/
void G_QueueBotBegin( int32_t clientNum ) {
	AddBotToSpawnQueue( clientNum, botBeginDelay );
	botBeginDelay += BOT_BEGIN_DELAY_INCREMENT;
}


/*
===============
G_BotConnect
===============
*/
qboolean G_BotConnect( int32_t clientNum, qboolean restart ) {
	bot_settings_t	settings;
	char			userinfo[MAX_INFO_STRING];

	trap_GetUserinfo( clientNum, userinfo, sizeof(userinfo) );

	strncpy(settings.characterfile, Info_ValueForKey(userinfo, "characterfile"), sizeof(settings.characterfile));
	settings.skill = atoi( Info_ValueForKey( userinfo, "skill" ) );
	strncpy( settings.team, Info_ValueForKey( userinfo, "team" ), sizeof(settings.team) );
	strncpy( settings.pclass, Info_ValueForKey( userinfo, "class" ), sizeof(settings.pclass) );

	if (AI_main_BotAISetupClient( clientNum, &settings ) == 0) {
		trap_DropClient( clientNum, "BotAISetupClient failed" );
		return qfalse;
	}

	if( restart && (g_gametype.integer == GT_SINGLE_PLAYER) ) {
		g_entities[clientNum].botDelayBegin = qtrue;
	} else {
		g_entities[clientNum].botDelayBegin = qfalse;
	}

	return qtrue;
}


/*
===============
G_AddBot
===============
*/
static void G_AddBot( const char* name, double skill, const char* team, const char* pclass, int32_t delay, char* altname) {
	int32_t			clientNum = 0;
	char*			botinfo = NULL;
	gentity_t*		bot = NULL;
	char*			key = NULL;
	char*			s = NULL;
	char*			botname = NULL;
	char*			model = NULL;
	char			userinfo[MAX_INFO_STRING];
	int32_t			preTeam = 0;

	// get the botinfo from bots.txt
	botinfo = G_GetBotInfoByName( name );
	if ( botinfo == NULL || botinfo[0] == 0 ) {
		G_Printf( S_COLOR_RED "Error: Bot '%s' not defined\n", name );
		return;
	}

	// create the bot's userinfo
	userinfo[0] = '\0';

	botname = Info_ValueForKey( botinfo, "funname" );
	if( botname == NULL || botname[0] == 0 ) {
		botname = Info_ValueForKey( botinfo, "name" );
	}
	// check for an alternative name
	if ((altname != NULL) && (altname[0] != 0)) {
		botname = altname;
	}
	Info_SetValueForKey( userinfo, "name", botname );
	Info_SetValueForKey( userinfo, "rate", "25000" );
	Info_SetValueForKey( userinfo, "snaps", "20" );
	Info_SetValueForKey( userinfo, "skill", va("%1.2f", skill) );

	key = "model";
	model = Info_ValueForKey( botinfo, key );
	if ( *model ) {
		model = "munro/main/default"; //RPG-X MODEL SYSTEM
	}
	Info_SetValueForKey( userinfo, key, model );

	key = "gender";
	s = Info_ValueForKey( botinfo, key );
	if ( !*s ) {
		s = "male";
	}
	Info_SetValueForKey( userinfo, "sex", s );

	key = "color";
	s = Info_ValueForKey( botinfo, key );
	if ( !*s ) {
		s = "4";
	}
	Info_SetValueForKey( userinfo, key, s );

	s = Info_ValueForKey(botinfo, "aifile");
	if (!*s ) {
		trap_Printf( S_COLOR_RED "Error: bot has no aifile specified\n" );
		return;
	}

	// have the server allocate a client slot
	clientNum = trap_BotAllocateClient();
	if ( clientNum == -1 ) {
		G_Printf( S_COLOR_RED "Unable to add bot.  All player slots are in use.\n" );
		G_Printf( S_COLOR_RED "Start server with more 'open' slots (or check setting of sv_maxclients cvar).\n" );
		return;
	}

	// initialize the bot settings
	if( team == NULL || !*team ) {
		if( g_gametype.integer >= GT_TEAM ) {
			if( G_Client_PickTeam(clientNum) == TEAM_RED) {
				team = "red";
			}
			else {
				team = "blue";
			}
		}
		else {
			team = "red";
		}
	}
	Info_SetValueForKey( userinfo, "characterfile", Info_ValueForKey( botinfo, "aifile" ) );
	Info_SetValueForKey( userinfo, "skill", va( "%5.2f", skill ) );
	Info_SetValueForKey( userinfo, "team", team );

	bot = &g_entities[ clientNum ];
	bot->r.svFlags |= SVF_BOT;
	bot->inuse = qtrue;

	// register the userinfo
	trap_SetUserinfo( clientNum, userinfo );

	if (g_gametype.integer >= GT_TEAM)
	{
		if (team && Q_stricmp(team, "red") == 0)
		{
			bot->client->sess.sessionTeam = TEAM_RED;
		}
		else if (team && Q_stricmp(team, "blue") == 0)
		{
			bot->client->sess.sessionTeam = TEAM_BLUE;
		}
		else
		{
			bot->client->sess.sessionTeam = G_Client_PickTeam( -1 );
		}
	}

	preTeam = bot->client->sess.sessionTeam;

	// have it connect to the game as a normal client
	if ( G_Client_Connect( clientNum, qtrue, qtrue ) ) {
		return;
	}

	if (bot->client->sess.sessionTeam != preTeam)
	{
		trap_GetUserinfo(clientNum, userinfo, MAX_INFO_STRING);

		if (bot->client->sess.sessionTeam == TEAM_SPECTATOR)
		{
			bot->client->sess.sessionTeam = preTeam;
		}

		if (bot->client->sess.sessionTeam == TEAM_RED)
		{
			team = "Red";
		}
		else
		{
			team = "Blue";
		}

		Info_SetValueForKey( userinfo, "team", team );

		trap_SetUserinfo( clientNum, userinfo );

		bot->client->ps.persistant[ PERS_TEAM ] = bot->client->sess.sessionTeam;

		G_ReadSessionData( bot->client );
		G_Client_UserinfoChanged( clientNum );
	}

	if( delay == 0 ) {
		G_Client_Begin( clientNum, qfalse, qfalse, qfalse );
		return;
	}

	AddBotToSpawnQueue( clientNum, delay );
}


/*
===============
Svcmd_AddBot_f
===============
*/
void Svcmd_AddBot_f( void ) {
	int32_t			skill = 0;
	int32_t			delay = 0;
	char			name[MAX_TOKEN_CHARS];
	char			altname[MAX_TOKEN_CHARS];
	char			string[MAX_TOKEN_CHARS];
	char			team[MAX_TOKEN_CHARS];
	char			pclass[MAX_TOKEN_CHARS];

	// are bots enabled?
	if ( trap_Cvar_VariableIntegerValue( "bot_enable" ) == 0 ) {
		return;
	}

	memset(name, 0, sizeof(name));
	memset(altname, 0, sizeof(altname));
	memset(string, 0, sizeof(string));
	memset(team, 0, sizeof(team));
	memset(pclass, 0, sizeof(pclass));

	// name
	trap_Argv( 1, name, sizeof( name ) );
	if ( name[0] == 0 ) {
		trap_Printf( "Usage: Addbot <botname> [skill 1-4] [team] [class] [msec delay] [altname]\n" );
		return;
	}

	// skill
	trap_Argv( 2, string, sizeof( string ) );
	if ( string[0] == 0 ) {
		skill = 4;
	} else {
		skill = atoi( string );
	}

	// team
	trap_Argv( 3, team, sizeof( team ) );

	// class
	trap_Argv( 4, pclass, sizeof( pclass ) );

	// delay
	trap_Argv( 5, string, sizeof( string ) );
	if ( string[0] == 0 ) {
		delay = 0;
	} else {
		delay = atoi( string );
	}

	// alternative name
	trap_Argv( 6, altname, sizeof( altname ) );

	G_AddBot( name, skill, team, pclass, delay, altname );

	// if this was issued during gameplay and we are playing locally,
	// go ahead and load the bot's media immediately
	if ( (level.time - level.startTime > 1000) &&
		trap_Cvar_VariableIntegerValue( "cl_running" ) ) {
		trap_SendServerCommand( -1, "loaddeferred\n" );	// FIXME: spelled wrong, but not changing for demo
	}
}

/*
===============
Svcmd_BotList_f
===============
*/
void Svcmd_BotList_f( void ) {
	int32_t	i = 0;
	char	name[MAX_TOKEN_CHARS];
	char	funname[MAX_TOKEN_CHARS];
	char	model[MAX_TOKEN_CHARS];
	char	aifile[MAX_TOKEN_CHARS];

	memset(name, 0, sizeof(name));
	memset(funname, 0, sizeof(funname));
	memset(model, 0, sizeof(model));
	memset(aifile, 0, sizeof(aifile));

	trap_Printf("^1name             model            aifile              funname\n");
	for (i = 0; i < g_numBots; i++) {
		strcpy(name, Info_ValueForKey( g_botInfos[i], "name" ));
		if ( !*name ) {
			strcpy(name, "RedShirt");
		}
		strcpy(funname, Info_ValueForKey( g_botInfos[i], "funname" ));
		if ( !*funname ) {
			strcpy(funname, "");
		}
		strcpy(model, Info_ValueForKey( g_botInfos[i], "model" ));
		if ( !*model ) {
			strcpy(model, "munro/main/default"); //RPG-X MODEL SYSTEM
		}
		strcpy(aifile, Info_ValueForKey( g_botInfos[i], "aifile"));
		if (!*aifile ) {
			strcpy(aifile, "bots/default_c.c");
		}
		trap_Printf(va("%-16s %-16s %-20s %-20s\n", name, model, aifile, funname));
	}
}


/*
===============
G_SpawnBots
===============
*/
static void G_SpawnBots( char* botList, int32_t baseDelay ) {
	char*		bot;
	char*		p;
	int32_t		skill = 0;
	int32_t		delay = 0;
	char		bots[MAX_INFO_VALUE];

	podium1 = NULL;
	podium2 = NULL;
	podium3 = NULL;
	memset(bots, 0, sizeof(bots));

	skill = trap_Cvar_VariableIntegerValue( "g_spSkill" );
	if( skill < 1 || skill > 5 ) {
		trap_Cvar_Set( "g_spSkill", "2" );
		skill = 2;
	}

	strncpy(bots, botList, sizeof(bots));
	p = &bots[0];
	delay = baseDelay;
	while( *p ) {
		//skip spaces
		while( *p && *p == ' ' ) {
			p++;
		}
		if( !p ) {
			break;
		}

		// mark start of bot name
		bot = p;

		// skip until space of null
		while( *p && *p != ' ' ) {
			p++;
		}
		if( *p ) {
			*p++ = 0;
		}

		// we must add the bot this way, calling G_AddBot directly at this stage
		// does "Bad Things"
		trap_SendConsoleCommand( EXEC_INSERT, va("addbot %s %i free %i\n", bot, skill, delay) );

		delay += BOT_BEGIN_DELAY_INCREMENT;
	}
}


/*
===============
G_LoadBotsFromFile
===============
*/
static void G_LoadBotsFromFile( char* filename ) {
	int32_t			len = 0;
	fileHandle_t	f = 0;
	char			buf[MAX_BOTS_TEXT];

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( f == 0 ) {
		trap_Printf( va( S_COLOR_RED "file not found: %s\n", filename ) );
		return;
	}
	if ( len >= MAX_BOTS_TEXT ) {
		trap_Printf( va( S_COLOR_RED "file too large: %s is %i, max allowed is %i", filename, len, MAX_BOTS_TEXT ) );
		trap_FS_FCloseFile( f );
		return;
	}

	memset(buf, 0, sizeof(buf));
	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );

	g_numBots += G_ParseInfos( buf, MAX_BOTS - g_numBots, &g_botInfos[g_numBots] );
}


/*
===============
G_LoadBots
===============
*/
static void G_LoadBots( void ) {
	vmCvar_t	botsFile;
	int32_t		numdirs = 0;
	char		filename[128];
	char		dirlist[4096];
	char*		dirptr = NULL;
	int32_t		i = 0;
	int32_t		dirlen = 0;

	if ( trap_Cvar_VariableIntegerValue( "bot_enable" ) == 0 ) {
		return;
	}

	memset(filename, 0, sizeof(filename));
	memset(dirlist, 0, sizeof(dirlist));
	g_numBots = 0;

	trap_Cvar_Register( &botsFile, "g_botsFile", "", CVAR_INIT|CVAR_ROM );
	if( *botsFile.string ) {
		G_LoadBotsFromFile(botsFile.string);
	}
	else {
		G_LoadBotsFromFile("scripts/bots.txt");
	}

	// get all bots from .bot files
	numdirs = trap_FS_GetFileList("scripts", ".bot", dirlist, sizeof(dirlist) );
	dirptr  = dirlist;
	for (i = 0; i < numdirs; i++, dirptr += dirlen+1)
	{
		dirlen = strlen(dirptr);
		strcpy(filename, "scripts/");
		strcat(filename, dirptr);
		G_LoadBotsFromFile(filename);
	}
	trap_Printf( va( "%i bots parsed\n", g_numBots ) );
}



/*
===============
G_GetBotInfoByNumber
===============
*/
char* G_GetBotInfoByNumber( int32_t num ) {
	if( num < 0 || num >= g_numBots ) {
		trap_Printf( va( S_COLOR_RED "Invalid bot number: %i\n", num ) );
		return NULL;
	}
	return g_botInfos[num];
}


/*
===============
G_GetBotInfoByName
===============
*/
char* G_GetBotInfoByName( const char* name ) {
	int32_t	n = 0;
	char*	value = NULL;

	for ( n = 0; n < g_numBots ; n++ ) {
		value = Info_ValueForKey( g_botInfos[n], "name" );
		if ( Q_stricmp( value, name ) == 0 ) {
			return g_botInfos[n];
		}
	}

	return NULL;
}

void G_InitBots( qboolean restart ) {
	const char* arenainfo = NULL;
	char*		strValue = NULL;
	char		map[MAX_QPATH];
	char		serverinfo[MAX_INFO_STRING];

	memset(map, 0, sizeof(map));
	memset(serverinfo, 0, sizeof(serverinfo));

	G_LoadBots();
	G_LoadArenas();

	trap_Cvar_Register( &bot_minplayers, "bot_minplayers", "0", CVAR_SERVERINFO );

	if( g_gametype.integer == GT_SINGLE_PLAYER ) {
		int32_t fragLimit = 0;
		int32_t timeLimit = 0;
		int32_t basedelay = 0;

		trap_GetServerinfo( serverinfo, sizeof(serverinfo) );
		Q_strncpyz( map, Info_ValueForKey( serverinfo, "mapname" ), sizeof(map) );
		arenainfo = G_GetArenaInfoByMap( map );
		if ( arenainfo == NULL ) {
			return;
		}

		strValue = Info_ValueForKey( arenainfo, "fraglimit" );
		fragLimit = atoi( strValue );
		if ( fragLimit != 0 ) {
			trap_Cvar_Set( "fraglimit", strValue );
		} else {
			trap_Cvar_Set( "fraglimit", "0" );
		}

		strValue = Info_ValueForKey( arenainfo, "timelimit" );
		timeLimit = atoi( strValue );
		if ( timeLimit != 0 ) {
			trap_Cvar_Set( "timelimit", strValue );
		} else {
			trap_Cvar_Set( "timelimit", "0" );
		}

		if ( fragLimit == 0 && timeLimit == 0 ) {
			trap_Cvar_Set( "fraglimit", "10" );
			trap_Cvar_Set( "timelimit", "0" );
		}

		if (g_holoIntro.integer != 0)
		{	// The player will be looking at the holodeck doors for the first five seconds, so take that into account.
			basedelay = BOT_BEGIN_DELAY_BASE + TIME_INTRO;
		} else {
			basedelay = BOT_BEGIN_DELAY_BASE;
		}
		strValue = Info_ValueForKey( arenainfo, "special" );
		if( Q_stricmp( strValue, "training" ) == 0 ) {
			basedelay += 10000;
		}

		if( !restart ) {
			G_SpawnBots( Info_ValueForKey( arenainfo, "bots" ), basedelay );
		}
	}
}
