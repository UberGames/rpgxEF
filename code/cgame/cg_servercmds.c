// Copyright (C) 1999-2000 Id Software, Inc.
//
// cg_servercmds.c -- reliably sequenced text commands sent by the server
// these are processed at snapshot transition time, so there will definately
// be a valid snapshot this frame

#include "cg_local.h"

#define	MAX_LOCAL_ENTITIES	512
extern localEntity_t	cg_localEntities[MAX_LOCAL_ENTITIES];

/*
=================
CG_ParseScores

=================
*/
static void CG_ParseScores( void ) {
	int		i, powerups, eliminated;

	cg.numScores = atoi( CG_Argv( 1 ) );
	if ( cg.numScores > MAX_CLIENTS ) {
		cg.numScores = MAX_CLIENTS;
	}

	cg.teamScores[0] = atoi( CG_Argv( 2 ) );
	cg.teamScores[1] = atoi( CG_Argv( 3 ) );

	memset( cg.scores, 0, sizeof( cg.scores ) );
	for ( i = 0 ; i < cg.numScores ; i++ ) {
		cg.scores[i].client			= atoi( CG_Argv( i * 11+ 4 ) );
		cg.scores[i].score			= atoi( CG_Argv( i * 11+ 5 ) );
		cg.scores[i].ping			= atoi( CG_Argv( i * 11+ 6 ) );
		cg.scores[i].time			= atoi( CG_Argv( i * 11+ 7 ) );
		cg.scores[i].scoreFlags		= atoi( CG_Argv( i * 11+ 8 ) );
		powerups					= atoi( CG_Argv( i * 11+ 9 ) );
//		cg.scores[i].faveTarget		= atoi( CG_Argv( i * 12+ 10) );
//		cg.scores[i].faveTargetKills = atoi( CG_Argv( i * 12+ 11) );
		cg.scores[i].worstEnemy		= atoi( CG_Argv( i * 11+ 10) );
		cg.scores[i].worstEnemyKills= atoi( CG_Argv( i * 11+ 11) );
		cg.scores[i].faveWeapon		= atoi( CG_Argv( i * 11+ 12) );
		cg.scores[i].killedCnt		= atoi( CG_Argv( i * 11+ 13) );
		eliminated					= atoi( CG_Argv( i * 11+ 14) );

		if ( cg.scores[i].client < 0 || cg.scores[i].client >= MAX_CLIENTS ) {
			cg.scores[i].client = 0;
		}
		cgs.clientinfo[ cg.scores[i].client ].score = cg.scores[i].score;
		cgs.clientinfo[ cg.scores[i].client ].powerups = powerups;
		cgs.clientinfo[ cg.scores[i].client ].eliminated = eliminated;
	}

}

/*
=================
CG_ParseTeamInfo

=================
*/
static void CG_ParseTeamInfo( void ) {
	int		i;
	int		client;

	numSortedTeamPlayers = atoi( CG_Argv( 1 ) );

	for ( i = 0 ; i < numSortedTeamPlayers ; i++ ) {
		client = atoi( CG_Argv( i * 2 + 2 ) ); //6

		sortedTeamPlayers[i] = client;

		cgs.clientinfo[ client ].location = atoi( CG_Argv( i * 2 + 3 ) ); //6 + 
		/*cgs.clientinfo[ client ].health = atoi( CG_Argv( i * 6 + 4 ) );
		cgs.clientinfo[ client ].armor = atoi( CG_Argv( i * 6 + 5 ) );
		cgs.clientinfo[ client ].curWeapon = atoi( CG_Argv( i * 6 + 6 ) );
		cgs.clientinfo[ client ].powerups = atoi( CG_Argv( i * 6 + 7 ) );*/
	}
}

/*
=================
CG_ParseHealthInfo

=================
*/
static void CG_ParseHealthInfo( void ) {
	int		i;
	int		client;
	int		numHealthInfoClients = 0;

	numHealthInfoClients = atoi( CG_Argv( 1 ) );

	for ( i = 0 ; i < numHealthInfoClients ; i++ ) {
		client = atoi( CG_Argv( i * 2 + 2 ) );

		cgs.clientinfo[ client ].health = atoi( CG_Argv( i * 2 + 3 ) );
	}
}
/*
================
CG_ParseServerinfo

This is called explicitly when the gamestate is first received,
and whenever the server updates any serverinfo flagged cvars
================
*/
void CG_ParseServerinfo( void ) {
	const char	*info;
	char	*mapname;

	info = CG_ConfigString( CS_SERVERINFO );
	cgs.gametype = atoi( Info_ValueForKey( info, "g_gametype" ) );
	cgs.pModAssimilation = atoi( Info_ValueForKey( info, "g_pModAssimilation" ) );
	cgs.pModDisintegration = atoi( Info_ValueForKey( info, "g_pModDisintegration" ) );
	cgs.pModActionHero = atoi( Info_ValueForKey( info, "g_pModActionHero" ) );
	cgs.pModSpecialties = atoi( Info_ValueForKey( info, "g_pModSpecialties" ) );
	cgs.pModElimination = atoi( Info_ValueForKey( info, "g_pModElimination" ) );
	cgs.dmflags = atoi( Info_ValueForKey( info, "dmflags" ) );
	cgs.teamflags = atoi( Info_ValueForKey( info, "teamflags" ) );
	cgs.fraglimit = atoi( Info_ValueForKey( info, "fraglimit" ) );
	cgs.capturelimit = atoi( Info_ValueForKey( info, "capturelimit" ) );
	cgs.timelimit = atoi( Info_ValueForKey( info, "timelimit" ) );
	cgs.maxclients = atoi( Info_ValueForKey( info, "sv_maxclients" ) );
	cgs.ForceClassColor = atoi( Info_ValueForKey( info, "rpg_forceClassColor" ) );
	mapname = Info_ValueForKey( info, "mapname" );
	Com_sprintf( cgs.mapname, sizeof( cgs.mapname ), "maps/%s.bsp", mapname );

	//RPG-X: TiM - new Rankset
	Q_strncpyz( cgs.rankSet, Info_ValueForKey( info, "rpg_rankSet"), sizeof(cgs.rankSet) );
	//RPG-X: TiM - new Class set
	Q_strncpyz( cgs.classSet, Info_ValueForKey( info, "rpg_classSet" ), sizeof( cgs.classSet ) );

	//scannable panels
	cgs.scannablePanels = atoi( Info_ValueForKey( info, "rpg_scannablePanels" ) );
}

/*
==================
CG_ParseWarmup
==================
*/
static void CG_ParseWarmup( void ) {
	const char	*info;
	int			warmup;

	info = CG_ConfigString( CS_WARMUP );

	warmup = atoi( info );
	cg.warmupCount = -1;

	if ( warmup == 0 && cg.warmup ) {

	} else if ( warmup > 0 && cg.warmup <= 0 ) {
		trap_S_StartLocalSound( cgs.media.countPrepareSound, CHAN_ANNOUNCER );
	}

	cg.warmup = warmup;
}

/*
================
CG_SetConfigValues

Called on load to set the initial values from configure strings
================
*/
void CG_SetConfigValues( void ) {
	const char *s;

	cgs.scores1 = atoi( CG_ConfigString( CS_SCORES1 ) );
	cgs.scores2 = atoi( CG_ConfigString( CS_SCORES2 ) );
	cgs.levelStartTime = atoi( CG_ConfigString( CS_LEVEL_START_TIME ) );
	s = CG_ConfigString( CS_FLAGSTATUS );
	cgs.redflag = s[0] - '0';
	cgs.blueflag = s[1] - '0';
	cg.warmup = atoi( CG_ConfigString( CS_WARMUP ) );
}

/*
=====================
CG_ClientShakeCamera

TiM: Parses the cam shake
config string, and inputs the useful
data.
=====================
*/
void CG_ClientShakeCamera ( void ) {
	float	intensity;
	int		duration;
	char	*str;

	str = (char *)CG_ConfigString( CS_CAMERA_SHAKE );
	intensity = (float)atoi( COM_Parse( &str ) )/10.0f;
	duration = atoi( COM_Parse( &str ) ); //This is an offset so if a player somehow received
																			//the string halfway thru the cycle (ie just connected etc)
																			//This way, it'll only do it as much as is left for the rest of the players
	cg.shake_serverIndex = duration;	//Back up the index for later
	duration -= ( cg.time - cgs.levelStartTime ); //This is the actual duration, based off of length, and the time the command was received

	CG_CameraShake( intensity, duration, qtrue );
}

/*
==================
CG_ParseClassData
==================
*/
/*void CG_ParseClassData( void )
{
	char	*str;
	int		i;
	char	*val;
	char	*lineChar;
	char	*lineCharEnd;
	int		colorBits;
	int		classBits;

	str = (char *)CG_ConfigString( CS_CLASS_DATA );

	if ( !str[0] )
		return;

	memset( &cgs.classData, 0, sizeof( cgs.classData ) );

	for ( i = 0; i < MAX_CLASSES; i++ ) {
		val = Info_ValueForKey( str, va( "c%i", i ) );

		if (!val[0])
			break;

		//First slash = consoleName, so skip that
		lineChar = strstr( val, "|");
		lineChar++;

		//next line should be formal name
		lineCharEnd = strstr( lineChar, "|" );
		lineCharEnd--;

		val = lineChar;
		val[ strlen(lineChar) - strlen(lineCharEnd) + 1] = '\0';

		Q_strncpyz( cgs.classData[i].formalName, val, sizeof( cgs.classData[i].formalName ) );

		//CG_Printf( S_COLOR_RED "%s\n", cgs.classData[i].formalName );

		//--Next is color

		lineChar = lineChar + (strlen(lineChar) - strlen(lineCharEnd))+2;

		lineCharEnd = strstr( lineChar, "|" );
		lineCharEnd--;

		val = lineChar;
		val[ strlen(lineChar) - strlen(lineCharEnd)+1] = '\0';

		colorBits=atoi( val );
		cgs.classData[i].radarColor[0] = colorBits & 255;
		cgs.classData[i].radarColor[1] = (colorBits >> 8) & 255;
		cgs.classData[i].radarColor[2] = (colorBits >> 16) & 255;
	
		//CG_Printf( S_COLOR_RED "%i\n", colorBits );

		//cgs.classData[i].showRanks = (colorBits >> 25) & 1;

		//--Next is Rank Icon Color
		lineChar = lineCharEnd+2;

		classBits = atoi( lineChar );
		
		cgs.classData[i].isMedic = ( classBits >> 1 ) & 1;
		cgs.classData[i].showRanks = ( classBits >> 2 ) & 1;
		cgs.classData[i].iconColor = ( classBits >> 4 ) & 15;

		//CG_Printf( S_COLOR_RED "%i\n", classBits );
	}
}*/

/*
================
CG_ConfigStringModified

================
*/
static void CG_ConfigStringModified( void ) {
	const char	*str;
	int		num;

	num = atoi( CG_Argv( 1 ) );

	// get the gamestate from the client system, which will have the
	// new configstring already integrated
	trap_GetGameState( &cgs.gameState );

	// look up the individual string that was modified
	str = CG_ConfigString( num );

	// do something with it if necessary
	if ( num == CS_MUSIC ) {
		CG_StartMusic();
	} else if ( num == CS_CAMERA_SHAKE ) { //RPG-X : TiM - Camera Shake
		CG_ClientShakeCamera();
	} else if ( num == CS_SERVERINFO ) {
		CG_ParseServerinfo();
	} else if ( num == CS_WARMUP ) {
		CG_ParseWarmup();
	} else if ( num == CS_SCORES1 ) {
		cgs.scores1 = atoi( str );
	} else if ( num == CS_SCORES2 ) {
		cgs.scores2 = atoi( str );
	} else if ( num == CS_WARMUP ) {
		CG_ParseWarmup();
	} else if ( num == CS_LEVEL_START_TIME ) {
		cgs.levelStartTime = atoi( str );
	} else if ( num == CS_VOTE_TIME ) {
		cgs.voteTime = atoi( str );
		cgs.voteModified = qtrue;
	} else if ( num == CS_VOTE_YES ) {
		cgs.voteYes = atoi( str );
		cgs.voteModified = qtrue;
	} else if ( num == CS_VOTE_NO ) {
		cgs.voteNo = atoi( str );
		cgs.voteModified = qtrue;
	} else if ( num == CS_VOTE_STRING ) {
		Q_strncpyz( cgs.voteString, str, sizeof( cgs.voteString ) );
	} else if ( num == CS_INTERMISSION ) {
		cg.intermissionStarted = atoi( str );
	} else if ( num >= CS_MODELS && num < CS_MODELS+MAX_MODELS ) {
		cgs.gameModels[ num-CS_MODELS ] = trap_R_RegisterModel( str );
	} else if ( num >= CS_SOUNDS && num < CS_SOUNDS+MAX_SOUNDS ) {
		if ( str[0] != '*' ) {	// player specific sounds don't register here
			cgs.gameSounds[ num-CS_SOUNDS] = trap_S_RegisterSound( str );
		}
	} else if ( num >= CS_PLAYERS && num < CS_PLAYERS+MAX_CLIENTS ) {
		CG_NewClientInfo( num - CS_PLAYERS );
	} else if ( num >= CS_DECOYS && num < CS_DECOYS+MAX_DECOYS ) {
		CG_NewDecoyInfo( num - CS_DECOYS );
	} else if ( num == CS_FLAGSTATUS ) {
		// format is rb where its red/blue, 0 is at base, 1 is taken, 2 is dropped
		cgs.redflag = str[0] - '0';
		cgs.blueflag = str[1] - '0';
	}
	else if(num == CS_SHADERSTATE) {
		CG_ShaderStateChanged();
	}
		
}


/*
=======================
CG_AddToTeamChat

=======================
*/
static void CG_AddToTeamChat( const char *str ) {
	int len;
	char *p, *ls;
	int lastcolor;
	int chatHeight;

	if (cg_teamChatHeight.integer < TEAMCHAT_HEIGHT) {
		chatHeight = cg_teamChatHeight.integer;
	} else {
		chatHeight = TEAMCHAT_HEIGHT;
	}

	if (chatHeight <= 0 || cg_teamChatTime.integer <= 0) {
		// team chat disabled, dump into normal chat
		cgs.teamChatPos = cgs.teamLastChatPos = 0;
		return;
	}

	len = 0;

	p = cgs.teamChatMsgs[cgs.teamChatPos % chatHeight];
	*p = 0;

	lastcolor = '7';

	ls = NULL;
	while (*str) {
		if (len > TEAMCHAT_WIDTH - 1) {
			if (ls) {
				str -= (p - ls);
				str++;
				p -= (p - ls);
			}
			*p = 0;

			cgs.teamChatMsgTimes[cgs.teamChatPos % chatHeight] = cg.time;

			cgs.teamChatPos++;
			p = cgs.teamChatMsgs[cgs.teamChatPos % chatHeight];
			*p = 0;
			*p++ = Q_COLOR_ESCAPE;
			*p++ = lastcolor;
			len = 0;
			ls = NULL;
		}

		if ( Q_IsColorString( str ) ) {
			*p++ = *str++;
			lastcolor = *str;
			*p++ = *str++;
			continue;
		}
		if (*str == ' ') {
			ls = p;
		}
		*p++ = *str++;
		len++;
	}
	*p = 0;

	cgs.teamChatMsgTimes[cgs.teamChatPos % chatHeight] = cg.time;
	cgs.teamChatPos++;

	if (cgs.teamChatPos - cgs.teamLastChatPos > chatHeight)
		cgs.teamLastChatPos = cgs.teamChatPos - chatHeight;
}



/*
===============
CG_MapRestart

The server has issued a map_restart, so the next snapshot
is completely new and should not be interpolated to.

A tournement restart will clear everything, but doesn't
require a reload of all the media
===============
*/
static void CG_MapRestart( void ) {
	if ( cg_showmiss.integer ) {
		CG_Printf( "CG_MapRestart\n" );
	}

	CG_InitLocalEntities();
	CG_InitMarkPolys();

	// make sure the "3 frags left" warnings play again
	cg.fraglimitWarnings = 0;

	cg.timelimitWarnings = 0;

	cg.intermissionStarted = qfalse;

	cgs.voteTime = 0;

	CG_StartMusic();

	// we really should clear more parts of cg here and stop sounds

	// play the "fight" sound if this is a restart without warmup
	if ( cg.warmup == 0 /* && cgs.gametype == GT_TOURNAMENT */) 
	{
		trap_S_StartLocalSound( cgs.media.countFightSound, CHAN_ANNOUNCER );
	}
}

/***********************
CG_EncodeIDFile

The server detected that
we have a pure copy of
the ID, so it's sent us
the IP it received so we
can byte encrypt it into
an ID and save it to file
***********************/

static void CG_EncodeIDFile( void )
{
	unsigned int	playerID;
	char			*IP;
	char			strSubnet[3];
	int				intSubnet[4];
	int				i, j;
	
	memset(strSubnet, 0, sizeof(strSubnet));

	IP = (char *)CG_Argv( 1 );
	//IP = "143.163.12.2";

	//TiM - Scooter's IP List
	//Double-check we're not spawning an ID off of these
	if( Q_stricmp( IP, "localhost" )		//localhost
		&& Q_strncmp( IP, "10.", 3 )		//class A
		&& Q_strncmp( IP, "172.16.", 7 )	//class B
		&& Q_strncmp( IP, "192.168.", 8 )	//class C
		&& Q_strncmp( IP, "127.", 4 )		//loopback
		&& Q_strncmp( IP, "169.254.", 8 )	//link-local
		)
	{
		return;
	}

	//check we don't already have an ID
	if ( (unsigned)atoul( sv_securityCode.string ) != SECURITY_PID )
		return;

	i = 0;
	j = 0;
	while ( *IP )
	{
		if( *IP != '.' )
		{
			if ( i < 3 )
				strSubnet[i++] = *IP;
		}
		else
		{
			if ( j < 4 )
				intSubnet[j++] = atoi( strSubnet );
			
			i=0;
			memset( strSubnet, 0, 3 );
		}

		IP++;
	}
	
	//the final cell
	intSubnet[j++] = atoi( strSubnet );

	//calculate the key
	playerID = ( intSubnet[3] << 24 ) | ( intSubnet[2] << 16 ) | ( intSubnet[1] << 8 ) | intSubnet[0];

	//CG_Printf( "%i %i %i %i - %u\n", intSubnet[0], intSubnet[1], intSubnet[2], intSubnet[3], playerID );
	
	//encode the information into the id key file
	{
		fileHandle_t		f;
		//unsigned char		buffer[SECURITY_SIZE];
		int					fileLen;
		rpgxSecurityFile_t	sF;

		fileLen = trap_FS_FOpenFile( SECURITY_FILE, &f, FS_READ );

		if ( !f || fileLen != SECURITY_SIZE )
		{
			CG_Error( "ERROR: Could not validate %s file.\n", SECURITY_FILE );
			return;
		}

		trap_FS_Read( &sF, SECURITY_SIZE, f );

		trap_FS_FCloseFile( f );

		if ( !sF.ID || sF.ID != SECURITY_ID )
		{
			CG_Error( "ERROR: %s was loaded, but it wasn't valid.\n", SECURITY_FILE );
			return;
		}

		//ensure the hash is valid
		if ( sF.hash != atoul( sv_securityHash.string ) )
		{
			CG_Error( "ERROR: %s was loaded, but the hash wasn't valid.\n", SECURITY_FILE );
			return;
		}

		//okay, reopen the file for writing, and input the new ID
		f = 0;

		fileLen = trap_FS_FOpenFile( SECURITY_FILE, &f, FS_WRITE );
		if ( !f )
		{
			CG_Error( "ERROR: Could not validate %s file for writing.\n", SECURITY_FILE );
			return;
		}

		//copy over the new key
		sF.playerID = playerID;

		trap_FS_Write( &sF, SECURITY_SIZE, f );
		trap_FS_FCloseFile( f );
	}

	trap_Cvar_Set( "sv_SecurityCode", va( "%u", playerID ) );
}


/*
==================
ConcatArgs
==================
*/
char	*ConcatArgs2( int start ) {
	int		i, c, tlen;
	static char	line[MAX_STRING_CHARS];
	int		len;
	char	arg[MAX_STRING_CHARS];

	len = 0;
	c = trap_Argc();
	for ( i = start ; i < c ; i++ ) {
		trap_Argv( i, arg, sizeof( arg ) );
		tlen = strlen( arg );
		if ( len + tlen >= MAX_STRING_CHARS - 1 ) {
			break;
		}
		memcpy( line + len, arg, tlen );
		len += tlen;
		if ( i != c - 1 ) {
			line[len] = ' ';
			len++;
		}
	}

	line[len] = 0;

	return line;
}

/*
=====================
CG_ShaderStateChanged
=====================
*/
void CG_ShaderStateChanged(void) {
	char originalShader[MAX_QPATH];
	char newShader[MAX_QPATH];
	char timeOffset[16];
	const char *o;
	char *n,*t;

	o = CG_ConfigString( CS_SHADERSTATE );

	if(!o) return;

	while (o && *o) {
		n = strstr(o, "=");
		if (n && *n) {
			strncpy(originalShader, o, n-o);
			originalShader[n-o] = 0;
			n++;
			t = strstr(n, ":");
			if (t && *t) {
				strncpy(newShader, n, t-n);
				newShader[t-n] = 0;
			} else {
				break;
			}
			t++;
			o = strstr(t, "@");
			if (o) {
				strncpy(timeOffset, t, o-t);
				timeOffset[o-t] = 0;
				o++;
				trap_R_RemapShader( originalShader, newShader, timeOffset );
			}
		} else {
			break;
		}
	}
}


/*
=================
CG_ServerCommand

The string has been tokenized and can be retrieved with
Cmd_Argc() / Cmd_Argv()
=================
*/
static void CG_ServerCommand( void ) {
	const char	*cmd;

	cmd = CG_Argv(0);

	if ( !cmd[0] ) {
		// server claimed the command
		return;
	}

	//RPG-X | Phenix | 13/02/2005
	// Play a insult to the n00b when moved into n00b class
	if ( !strcmp( cmd, "playN00bInsult") ) {
		trap_S_StartLocalSound( cgs.media.N00bSound[(rand()%N00bSoundCount)], CHAN_LOCAL_SOUND );
		CG_CenterPrint( "Welcome to the n00b Class", SCREEN_HEIGHT * 0.25, BIGCHAR_WIDTH );
		return;
	}
	
	// RPG-X | Phenix | 08/06/05
	if ( !strcmp( cmd, "servermsg") ) {
		trap_S_StartLocalSound( cgs.media.AdminMsgSound, CHAN_LOCAL_SOUND );
		cg.adminMsgTime = cg.time + 10000;
		Q_strncpyz( cg.adminMsgMsg, ConcatArgs2(1), sizeof( cg.adminMsgMsg ) );
		//CG_CenterPrint( cg.adminMsgMsg, SCREEN_HEIGHT * 0.25, BIGCHAR_WIDTH );
		return;
	}

	if( !strcmp( cmd, "servercprint") ) {
		trap_S_StartLocalSound( cgs.media.AdminMsgSound, CHAN_LOCAL_SOUND );
		//cg.adminMsgTime = cg.time + 10000;
		Q_strncpyz( cg.adminMsgMsg, ConcatArgs2(1), sizeof( cg.adminMsgMsg ) );
		CG_CenterPrint( cg.adminMsgMsg, SCREEN_HEIGHT * 0.25, BIGCHAR_WIDTH );
		return;
	}

	if ( !strcmp( cmd, "cp" ) ) {
		CG_CenterPrint( CG_Argv(1), SCREEN_HEIGHT * 0.25, BIGCHAR_WIDTH );
		return;
	}

	if ( !strcmp( cmd, "cs" ) ) {
		CG_ConfigStringModified();
		return;
	}

	if ( !strcmp( cmd, "print" ) ) {
		CG_Printf( "%s", CG_Argv(1) );
		return;
	}

	if ( !strcmp( cmd, "chat" ) ) {
		trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
		CG_Printf( "%s\n", CG_Argv(1) );
		return;
	}

	if ( !strcmp( cmd, "pc" ) ) {
		trap_Cvar_Set("ui_playerClass", CG_Argv(1));
		return;
	}

	if ( !strcmp( cmd, "prank" ) ) {
		trap_Cvar_Set("ui_playerRank", CG_Argv(1));
		return;
	}

	/*if ( !strcmp( cmd, "cr" ) ) {
		trap_Cvar_VariableStringBuffer( "ui_playerclass", pClass, sizeof(pClass) );
		trap_Cvar_VariableStringBuffer( "ui_playerrank", pRank, sizeof(pRank) );
		
		if ( !strcmp( pClass, "maker" ) || !strcmp( pClass, "alphaomega22" ) ) {
			trap_SendClientCommand( "class command" );
			trap_SendClientCommand( va( "rank %s", pRank)  );
		}

		trap_SendClientCommand( va( "class %s", pClass)  );
		trap_SendClientCommand( va( "rank %s", pRank)  );
		return;
	}*/

	if ( !strcmp( cmd, "tchat" ) ) {
		trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
		CG_AddToTeamChat( CG_Argv(1) );
		CG_Printf( "%s\n", CG_Argv(1) );
		return;
	}

	if ( !strcmp( cmd, "scores" ) ) {
		CG_ParseScores();
		return;
	}

	if ( !strcmp( cmd, "awards" ) ) {
		AW_SPPostgameMenu_f();
		return;
	}

	if ( !strcmp( cmd, "tinfo" ) ) {
		CG_ParseTeamInfo();
		return;
	}

	if ( !strcmp( cmd, "hinfo" ) ) {
		CG_ParseHealthInfo();
		return;
	}

	if ( !strcmp( cmd, "map_restart" ) ) {
		CG_MapRestart();
		return;
	}

	//TiM: Purge all active effects
	if ( !strcmp( cmd, "cg_flushFX" ) ) {
		int i;

		for ( i = 0; i < MAX_LOCAL_ENTITIES; i ++ ) {
			cg_localEntities[i].endTime = cg.time;
		}
		return;
	}

	/*if ( !strcmp( cmd, "cg_flushAngles" ) ) {
		//CG_ResetPlayerEntity( &cg.predictedPlayerEntity ); //&cg_entities[ cg.predictedPlayerState.clientNum ]
		cg_entities[cg.predictedPlayerState.clientNum].pe.torso.yawAngle = cg_entities[cg.predictedPlayerState.clientNum].lerpAngles[YAW];
		cg_entities[cg.predictedPlayerState.clientNum].pe.legs.yawAngle = cg_entities[cg.predictedPlayerState.clientNum].lerpAngles[YAW];
		return;
	}*/

	// loaddeferred can be both a servercmd and a consolecmd
	if ( !strcmp( cmd, "loaddeferred" ) ) {	// FIXME: spelled wrong, but not changing for demo
		CG_LoadDeferredPlayers();
		return;
	}

	// clientLevelShot is sent before taking a special screenshot for
	// the menu system during development
	if ( !strcmp( cmd, "clientLevelShot" ) ) {
		cg.levelShot = qtrue;
		return;
	}

	//TiM - client received a command from a turbolift ent
	//Show the decks UI
	if ( !strcmp( cmd, "lift" ) ) {
		trap_SendConsoleCommand( va( "ui_turbolift %s", CG_Argv( 1 ) ) );
		return;
	}

	//The server motd thingzor
    //RPG-X | Marcin | 15/12/2008
	if ( !strcmp( cmd, "motd" ) ) {
		trap_SendConsoleCommand( "ui_motd_reset\n" );
		trap_SendConsoleCommand( "ui_motd\n" );
		return;
	}

	//RPG-X | Marcin | 15/12/2008
	if ( !strcmp( cmd, "motd_line" ) ) {
		trap_SendConsoleCommand( va( "ui_motd_line \"%s\"\n", CG_Argv( 1 ) ) );
		return;
	}

	if ( !strcmp( cmd, "configID" ) )
	{
		CG_EncodeIDFile();
		return;
	}

	if ( !strcmp( cmd, "changeClientInfo" ) )
	{
		//create local copy of the args
		//due to the way CG_Argv works
		char arg1[64];
		char arg2[64];

		Q_strncpyz( arg1, CG_Argv(1), sizeof(arg1) );
		Q_strncpyz( arg2, CG_Argv(2), sizeof(arg2) );

		trap_Cvar_Set( arg1, arg2 );
		return;
	}

	if ( !strcmp( cmd, "playSnd" ) )
	{
		trap_SendConsoleCommand( va( "play %s", CG_Argv(1) ) );
		return;
	}

	if ( !strcmp( cmd, "cg_connect" ) ) {
		trap_SendConsoleCommand( va( "connect %s", CG_Argv(1) ) );
		return;
	}

	if ( Q_stricmp (cmd, "remapShader") == 0 )
	{
		if (trap_Argc() == 4)
		{
			char shader1[MAX_QPATH];
			char shader2[MAX_QPATH];
			char shader3[MAX_QPATH];

			Q_strncpyz(shader1, CG_Argv(1), sizeof(shader1));
			Q_strncpyz(shader2, CG_Argv(2), sizeof(shader2));
			Q_strncpyz(shader3, CG_Argv(3), sizeof(shader3));

			trap_R_RemapShader(shader1, shader2, shader3);
		}
		
		return;
	}

	if(!strcmp(cmd, "ui_transporter")) {
		trap_SendConsoleCommand(va("ui_transporter %s", CG_Argv(1)));
		return;
	}

	if(!strcmp(cmd, "ui_trdata")) {
		trap_SendConsoleCommand(va("ui_trdata \"%s\"", CG_Argv(1)));
		return;
	}

	if(!strcmp(cmd, "holo_data")) {
		trap_SendConsoleCommand(va("holo_data \"%s\"", CG_Argv(1)));
		return;
	}

	if(!strcmp(cmd, "ui_holodeck")) {
		trap_SendClientCommand(va("ui_holodeck %s", CG_Argv(1)));
		return;
	}

	CG_Printf( "Unknown client game command: %s\n", cmd );
}


/*
====================
CG_ExecuteNewServerCommands

Execute all of the server commands that were received along
with this this snapshot.
====================
*/
void CG_ExecuteNewServerCommands( int latestSequence ) {
	while ( cgs.serverCommandSequence < latestSequence ) {
		if ( trap_GetServerCommand( ++cgs.serverCommandSequence ) ) {
			CG_ServerCommand();
		}
	}
}
