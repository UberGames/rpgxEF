// Copyright (C) 1999-2000 Id Software, Inc.
//

#include "g_local.h"
#include "g_sql.h"

//#include <windows.h>


static void G_SayTo( gentity_t *ent, gentity_t *other, int mode, int color, const char *name, const char *message );

extern void G_CheckReplaceQueen( int clientNum );
extern qboolean	PlaceDecoy( gentity_t *ent );
extern qboolean G_CallSpawn(gentity_t *ent);

extern int	numKilled;
extern clInitStatus_t clientInitialStatus[];
extern qboolean levelExiting;

/*
==================
DeathmatchScoreboardMessage

==================
*/
void DeathmatchScoreboardMessage( gentity_t *ent ) {
	char		entry[1024];
	char		string[1400];
	int			stringlength;
	int			i, j;
	gclient_t	*cl;
	int			numSorted;
	int			scoreFlags;

	// send the latest information on all clients
	string[0] = 0;
	stringlength = 0;
	scoreFlags = 0;

	// don't send more than 32 scores (FIXME?)
	numSorted = level.numConnectedClients;
	if ( numSorted > 32 ) {
		numSorted = 32;
	}

	for (i=0 ; i < numSorted ; i++) {
		int		ping;

		cl = &level.clients[level.sortedClients[i]];

		if ( cl->pers.connected == CON_CONNECTING ) {
			ping = -1;
		} else {
			ping = cl->ps.ping < 999 ? cl->ps.ping : 999;
		}
		// TODO ajust me some things here might not be needed anymore
		Com_sprintf (entry, sizeof(entry),
			" %i %i %i %i %i %i %i %i %i %i %i", level.sortedClients[i],
			cl->ps.persistant[PERS_SCORE], ping, (level.time - cl->pers.enterTime)/60000,
			scoreFlags, g_entities[level.sortedClients[i]].s.powerups,
			0,
			0,
			0,
			cl->ps.persistant[PERS_KILLED],
			((g_entities[cl->ps.clientNum].r.svFlags&SVF_ELIMINATED)!=0) );
		j = strlen(entry);
		if (stringlength + j > 1024)
			break;
		strcpy (string + stringlength, entry);
		stringlength += j;
	}

	trap_SendServerCommand( ent-g_entities, va("scores %i %i %i%s", i, 
		level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE],
		string ) );
}


/*
==================
Cmd_Score_f

Request current scoreboard information
==================
*/
void Cmd_Score_f( gentity_t *ent ) {
	DeathmatchScoreboardMessage( ent );
}



/*
==================
CheatsOk
==================
*/
static qboolean	CheatsOk( gentity_t *ent ) {
	if ( !g_cheats.integer || IsAdmin( ent ) == qfalse) {
		trap_SendServerCommand( ent-g_entities, va("print \"You can't use cheats in a Roleplay Session!\n\""));
		return qfalse;
	}
	if ( ent->health <= 0 ) {
		trap_SendServerCommand( ent-g_entities, va("print \"You cheat! However you're dead - so haha...\n\""));
		return qfalse;
	}
	return qtrue;
}


/*
==================
ConcatArgs
==================
*/
char	*ConcatArgs( int start ) {
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
==================
SanitizeString

Remove case and control characters
==================
*/
static void SanitizeString( char *in, char *out ) {
	while ( *in ) {
		if ( *in == 27 ) {
			in += 2;		// skip color code
			continue;
		}
		if ( *in < 32 ) {
			in++;
			continue;
		}
		*out++ = tolower( *in++ );
	}

	*out = 0;
}

/*
==================
ClientNumberFromString

Returns a player number for either a number or name string
Returns -1 if invalid
==================
*/
static int ClientNumberFromString( gentity_t *to, char *s ) {
	gclient_t	*cl;
	int			idnum;
	char		s2[MAX_STRING_CHARS];
	char		n2[MAX_STRING_CHARS];

	// numeric values are just slot numbers
	if (s[0] >= '0' && s[0] <= '9') {
		idnum = atoi( s );
		if ( idnum < 0 || idnum >= level.maxclients ) {
			trap_SendServerCommand( to-g_entities, va("print \"Bad client slot: %i\n\"", idnum));
			return -1;
		}

		cl = &level.clients[idnum];
		if ( cl->pers.connected != CON_CONNECTED ) {
			trap_SendServerCommand( to-g_entities, va("print \"Client %i is not active\n\"", idnum));
			return -1;
		}
		return idnum;
	}

	// check for a name match
	SanitizeString( s, s2 );
	for ( idnum=0,cl=level.clients ; idnum < level.maxclients ; idnum++,cl++ ) {
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		SanitizeString( cl->pers.netname, n2 );
		if ( !strcmp( n2, s2 ) ) {
			return idnum;
		}
	}

	trap_SendServerCommand( to-g_entities, va("print \"User %s is not on the server\n\"", s));
	return -1;
}



/*
==================
Cmd_Give_f

Give items to a client

TiM: Rebuilt from scratch so
as to allow giving between yourself
and other clients. 
Also, incorporated a dynamic item list
to optimize the data between the game
and the UI module.
==================
*/

static void Cmd_Give_f( gentity_t *ent ) {
	char			arg[64];
	int				clientNum;
	gentity_t		*targEnt;
	int				i;
	giveItem_t		*item = NULL;
	qboolean		self;
	gclient_t		*client;
	playerState_t	*ps;

	if(!ent || !ent->client) return;

	client = ent->client;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_GIVE) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_Argv( 1, arg, sizeof( arg ) );

	if ( !arg[0] )
		return;

	//lets assume the target is the player for now
	clientNum = client->ps.clientNum;
	ps = &client->ps;

	//if first char is a num, assume we're directing to another client
	if ( arg[0] >='0' && arg[0] <= '9' ) {
		clientNum = atoi( arg );

		if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
			trap_SendServerCommand( ent-g_entities, va( "print \"Invalid client num specified.\n\" " ) );
			return;
		}

		targEnt = &g_entities[ clientNum ];

		if ( !targEnt || !targEnt->client ) {
			trap_SendServerCommand( ent-g_entities, va( "print \"Client was invalid.\n\" " ) );
			return;
		}

		Q_strncpyz( arg, ConcatArgs( 2 ), sizeof( arg ) );

		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, va( "print \"No Item Specified.\n\" " ) );
			return;
		}

		client = targEnt->client;
		ps = &client->ps;

		self = qfalse;
	}
	else 
	{
		targEnt = &g_entities[ clientNum ];

		if ( !targEnt || !targEnt->client ) {
			trap_SendServerCommand( ent-g_entities, va( "print \"Client was invalid.\n\" " ) );
			return;
		}

		trap_Argv( 1, arg, sizeof( arg ) );

		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, va( "print \"No Item Specified.\n\" " ) );
			return;
		}

		self = qtrue;
	}

	if ( ps->pm_type == PM_DEAD ) {
		if ( !self )
			trap_SendServerCommand( ent-g_entities, va( "print \"ERROR: You cannot give items to a dead player.\"") );
		else
			trap_SendServerCommand( ent-g_entities, va( "print \"ERROR: You cannot receive items when you're dead.\"") );

		return;
	}

	//loop thru all the item data we have - GSIO: changed from i++ to i-- 
	for ( i = bg_numGiveItems - 1; i > -1; i-- ) {
		item = &bg_giveItem[i];

		if ( !Q_stricmp( arg, item->consoleName ) ) {
			break;
		}
	}

	if ( i >= bg_numGiveItems ) {
		trap_SendServerCommand( ent-g_entities, va( "print \"ERROR: Specified item not found.\"") );
		return;
	}

	if ( !self ) {
		trap_SendServerCommand( ent-g_entities, va( "print \"Gave item %s to %s^7.\n\"", arg, targEnt->client->pers.netname) );
		trap_SendServerCommand( targEnt-g_entities, va( "print \"Received item: %s.\n\"", arg ) );
	}

	//Fuck this. Why does ioEF crash if you don't break a case statement with code in it? :S

	switch ( item->giveType ) {
	case TYPE_ALL:
		targEnt->health = ps->stats[STAT_MAX_HEALTH];

		ps->stats[STAT_WEAPONS] = (1 << WP_NUM_WEAPONS) - 1 - ( 1 << WP_0 );

		//RPG-X: J2J - Added so you dont just get empty weapons
		for ( i = MAX_WEAPONS - 1; i > -1; i-- ) { /* RPG-X | Marcin | 30/12/2008 */ // GSIO: Changed from i++ to i--
			targEnt->client->ps.ammo[i] += 1;
		}	

		ps->stats[STAT_HOLDABLE_ITEM] = BG_FindItemForHoldable( HI_SHIELD ) - bg_itemlist;

		break;
	case TYPE_HEALTH:
		targEnt->health = ps->stats[STAT_MAX_HEALTH];
		break;
	case TYPE_WEAPONS:
		ps->stats[STAT_WEAPONS] = (1 << WP_NUM_WEAPONS) - 1 - ( 1 << WP_0 );

		//RPG-X: J2J - Added so you dont just get empty weapons
		for ( i = MAX_WEAPONS -1; i > -1 ; i-- ) { // GSIO: changed from i++ to i--
			targEnt->client->ps.ammo[i] += 1;
		}	

		break;
	case TYPE_AMMO:
		for ( i = MAX_WEAPONS - 1; i > -1 ; i-- ) { // GSIO: changed from i++ to i--
			targEnt->client->ps.ammo[i] += 1;
		}
		break;
	case TYPE_HOLDABLE:
		ps->stats[STAT_HOLDABLE_ITEM] = BG_FindItemForHoldable( (holdable_t)item->giveValue ) - bg_itemlist;
		break;
	case TYPE_WEAPON:
		ps->stats[STAT_WEAPONS] ^= ( 1 << item->giveValue);

		if ( ps->stats[STAT_WEAPONS] & ( 1 << item->giveValue) )
			ps->ammo[item->giveValue] += 1;
		else
			ps->ammo[item->giveValue] = 0;
		break;

	case TYPE_POWERUP:
		{
			int	flag = 0;
			int arrayNum = 0;

			if ( !Q_stricmp( item->consoleName, "god" ) ) {
				flag = FL_GODMODE;
				arrayNum = -1;
			}
			else if ( !Q_stricmp( item->consoleName, "cloak" ) ) {
				flag = FL_CLOAK;
				arrayNum = PW_INVIS;
			}
			else if ( !Q_stricmp( item->consoleName, "flight" ) ) {
				flag = FL_FLY;
				arrayNum = PW_FLIGHT;					
			}
			else if ( !Q_stricmp( item->consoleName, "evasuit" ) ) {
				flag = FL_FLY;
				arrayNum = PW_FLIGHT;					
			}

			targEnt->flags ^= flag;

			if ( arrayNum >= 0 ) {
				if( targEnt->flags & flag ) {
					ps->powerups[arrayNum] = level.time + 10000000;
				}
				else
				{
					ps->powerups[arrayNum] = level.time;
				}
			}
		}
		break;
	}

	//What the absolute fudge?! What the hell is causing the EVA Suit to activate!?
	if (!(ent->flags & FL_EVOSUIT))
		ps->powerups[PW_EVOSUIT] = 0;
}

/*
==================
Cmd_God_f

Sets client to godmode

argv(0) god
==================
*/
static void Cmd_God_f (gentity_t *ent)
{

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_GOD) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	ent->flags ^= FL_GODMODE;
	if (!(ent->flags & FL_GODMODE) )
		G_PrintfClient(ent, "%s", "godmode OFF\n");
	else
		G_PrintfClient(ent, "%s", "godmode ON\n");
}

/*
==================
Cmd_Notarget_f

Sets client to notarget

argv(0) notarget
==================
*/
// Harry -- Do we need this? I believe this is for bots only.
static void Cmd_Notarget_f( gentity_t *ent ) {

	if ( IsAdmin( ent ) == qfalse ) {
		return;
	}

	ent->flags ^= FL_NOTARGET;
	if (!(ent->flags & FL_NOTARGET) )
		G_PrintfClient(ent, "%s", "notarget OFF\n");
	else
		G_PrintfClient(ent, "%s", "notarget ON\n");
}


/*
==================
Cmd_Noclip_f

argv(0) noclip
==================
*/
static void Cmd_Noclip_f( gentity_t *ent ) {
	gclient_t *client;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_NOCLIP) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif
	client = ent->client;
	if ( client->ps.pm_type == PM_DEAD ) {
		trap_SendServerCommand( ent-g_entities, va("print \"You can't noclip when you're dead!\n\""));
		return;
	}

	if ( client->noclip ) {
		G_PrintfClient(ent, "%s", "noclip OFF\n");
	} else {
		G_PrintfClient(ent, "%s", "noclip ON\n");
	}
	client->noclip = (qboolean)!client->noclip;

	return;
}


/*
==================
Cmd_LevelShot_f

This is just to help generate the level pictures
for the menus.  It goes to the intermission immediately
and sends over a command to the client to resize the view,
hide the scoreboard, and take a special screenshot
==================
*/
static void Cmd_LevelShot_f( gentity_t *ent ) {
	if ( !CheatsOk( ent ) ) {
		return;
	}

	// doesn't work in single player
	if ( g_gametype.integer != 0 ) {
		trap_SendServerCommand( ent-g_entities, 
			"print \"Must be in g_gametype 0 for levelshot\n\"" );
		return;
	}

	level.intermissiontime = -1;
	// Special 'level shot' setting -- Terrible ABUSE!!!  HORRIBLE NASTY HOBBITTESSSES

	G_Client_BeginIntermission();
	trap_SendServerCommand( ent-g_entities, "clientLevelShot" );
}


/*
=================
Cmd_Kill_f

TiM: Okay... I think
instead of making a new
func, we can just tweak this one a bit. :)
=================
*/
int lastKillTime[MAX_CLIENTS];
static void Cmd_Kill_f( gentity_t *ent ) 
{
	int		meansOfDeath = MOD_SUICIDE;		//Means of death set to suicide by default
	char	deathMsg[MAX_STRING_TOKENS];	//The death message will never be this long, but just to be sure....
	clientSession_t *sess = &ent->client->sess;
	int		clientNum = ent->client->ps.clientNum;
	playerState_t *ps = &ent->client->ps;
	clientPersistant_t *pers = &ent->client->pers;

	//RPG-X: Redtechie - haha too stupid to kill them selves!
	if ( g_classData[sess->sessionClass].isn00b )
	{
		trap_SendServerCommand( ent-g_entities, "print \"Sorry, you're too n00bish to handle this command.\n\"" );
		return;
	}

	if ( sess->sessionTeam == TEAM_SPECTATOR ) {
		return;
	}

	if ( rpg_allowsuicide.integer == 0 )
	{
		return;
	}

	if ( lastKillTime[clientNum] > level.time - 5000 ) // Marcin - changed
	{//can't flood-kill
		trap_SendServerCommand( clientNum, va("cp \"Cannot suicide for %d seconds", (lastKillTime[ent->client->ps.clientNum]-(level.time-60000))/1000 ) );
		return;
	}

	//TiM: Check for whether we have args or not
	if ( trap_Argc() > 1 ) {
		//Load the arguments
		Q_strncpyz( deathMsg, ConcatArgs( 1 ), sizeof(deathMsg) );

		//If the args are valid
		if ( deathMsg[0] ) {
			//switch the MOD
			meansOfDeath = MOD_CUSTOM_DIE;

			//broadcast the message
			trap_SendServerCommand( -1, va( "print \"%s" S_COLOR_WHITE " %s\n\" ", ent->client->pers.netname, deathMsg ) );
		}
	}

	lastKillTime[clientNum] = level.time;
	ent->flags &= ~FL_GODMODE;

	//RPG-X: Medics revive Support for suiciding
	if(rpg_medicsrevive.integer != 0 ){
		ps->stats[STAT_WEAPONS] = ( 1 << WP_0 );
		ps->stats[STAT_HOLDABLE_ITEM] = HI_NONE;
		ps->stats[STAT_HEALTH] = ent->health = 1;
		G_Client_Die (ent, ent, ent, 1, meansOfDeath ); //MOD_SUICIDE
	}else{
		ps->stats[STAT_HEALTH] = ent->health = 0;
		G_Client_Die (ent, ent, ent, 100000, meansOfDeath ); //MOD_SUICIDE
	}

	if ( rpg_kicksuiciders.integer > 0 )
	{
		pers->suicideCount++;
		if ( pers->suicideCount >= rpg_kicksuiciders.integer )
		{
			trap_DropClient( clientNum, "Kicked: Too many suicides");
		}
	}
}

/*
=================
BroadCastTeamChange

Let everyone know about a team change
=================
*/
void BroadcastTeamChange( gclient_t *client, int oldTeam )
{
	clientSession_t *sess = &client->sess;
	clientPersistant_t *pers = &client->pers;
	if ( levelExiting )
	{//no need to do this during level changes
		return;
	}
	if ( sess->sessionTeam == TEAM_RED ) {
		char	red_team[MAX_QPATH];
		trap_GetConfigstring( CS_RED_GROUP, red_team, sizeof( red_team ) );
		if (!red_team[0])	{
			Q_strncpyz( red_team, "red team", sizeof( red_team ) );
		}
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " joined the %s.\n\"", pers->netname, red_team ) );
	} else if ( sess->sessionTeam == TEAM_BLUE ) {
		char	blue_team[MAX_QPATH];
		trap_GetConfigstring( CS_BLUE_GROUP, blue_team, sizeof( blue_team ) );
		if (!blue_team[0]) {
			Q_strncpyz( blue_team, "blue team", sizeof( blue_team ) );
		}
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " joined the %s.\n\"", pers->netname, blue_team ) );
	} else if ( sess->sessionTeam == TEAM_SPECTATOR && oldTeam != TEAM_SPECTATOR ) {
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " is now spectating.\n\"",
			pers->netname));
	} else if ( sess->sessionTeam == TEAM_FREE ) {
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " joined the Roleplay Session.\n\"",
			pers->netname));
	}
}

/*
=================
BroadCastClassChange

Let everyone know about a team change
=================
*/
void BroadcastClassChange( gclient_t *client, pclass_t oldPClass )
{
	if ( levelExiting )
	{//no need to do this during level changes
		return;
	}

	if ( g_classData[client->sess.sessionClass].message[0] )
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE "%s\n\"", client->pers.netname, g_classData[client->sess.sessionClass].message) );
}

/*
=================
SetTeam
=================
*/
qboolean SetTeam( gentity_t *ent, char *s ) {
	int					team, oldTeam;
	gclient_t			*client;
	int					clientNum, clNum;
	spectatorState_t	specState;
	int					specClient;
	int					isBot;
	char				info[MAX_INFO_STRING];
	clientSession_t		*sess;
	isBot = atoi( Info_ValueForKey( info, "skill" ) );


	//
	// see what change is requested
	//
	client = ent->client;

	clientNum = client - level.clients;
	specClient = 0;

	specState = SPECTATOR_NOT;

	clNum = client->ps.clientNum;
	sess = &client->sess;

	if ( g_gametype.integer >= GT_TEAM && !isBot )
	{
		if ( !( !Q_stricmp( s, "spectator" ) || !Q_stricmp( s, "s" ) || !Q_stricmp( s, "red" ) || !Q_stricmp( s, "r" ) || !Q_stricmp( s, "blue" ) || !Q_stricmp( s, "b" ) ) )
		{
			return qfalse;
		}
	}

	if ( !Q_stricmp( s, "spectator" ) || !Q_stricmp( s, "s" ) ) {
		team = TEAM_SPECTATOR;
		specState = SPECTATOR_FREE;
		client->noclip = (qboolean)1;
	} else if ( g_gametype.integer >= GT_TEAM ) {
		// if running a team game, assign player to one of the teams
		specState = SPECTATOR_NOT;
		if ( !Q_stricmp( s, "red" ) || !Q_stricmp( s, "r" ) ) {
			team = TEAM_RED;
		} else if ( !Q_stricmp( s, "blue" ) || !Q_stricmp( s, "b" ) ) {
			team = TEAM_BLUE;
		} else {
			// pick the team with the least number of players
			if ( isBot )
			{
				team = G_Client_PickTeam( clientNum );
			}
			else
			{
				team = TEAM_SPECTATOR;
				specState = SPECTATOR_FREE;
			}
		}
	}
	else
	{
		team = TEAM_FREE;
	}

	// override decision if limiting the players
	if ( g_gametype.integer == GT_TOURNAMENT
		&& level.numNonSpectatorClients >= 2 ) {
			team = TEAM_SPECTATOR;
	} else if ( g_maxGameClients.integer > 0 && 
		level.numNonSpectatorClients >= g_maxGameClients.integer ) {
			team = TEAM_SPECTATOR;
	}

	//
	// decide if we will allow the change
	//
	oldTeam = sess->sessionTeam;
	if ( team == oldTeam && team != TEAM_SPECTATOR ) {
		return qfalse;
	}

	//
	// execute the team change
	//

	// he starts at 'base'
	client->pers.teamState.state = TEAM_BEGIN;

	if ( oldTeam != TEAM_SPECTATOR ) {
		// Kill him (makes sure he loses flags, etc)
		ent->flags &= ~FL_GODMODE;
		ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
		G_Client_Die (ent, NULL, NULL, 100000, MOD_RESPAWN);

	}
	// they go to the end of the line for tournements
	if ( team == TEAM_SPECTATOR ) {
		sess->spectatorTime = level.time;
	}

	sess->sessionTeam = (team_t)team;
	sess->spectatorState = specState;
	sess->spectatorClient = specClient;

	BroadcastTeamChange( client, oldTeam );

	// get and distribute relevent paramters
	G_Client_UserinfoChanged( clientNum );

	G_Client_Begin( clientNum, qfalse, qfalse, qfalse );

	return qtrue;
}

char *ClassNameForValue( pclass_t pClass ) 
{
	char buffer[MAX_QPATH];
	char *ptr;

	trap_Cvar_VariableStringBuffer( va( "rpg_%sPass", g_classData[pClass].consoleName ), buffer, sizeof(buffer) );

	ptr = buffer;

	return ptr;
}

/*
=================
ValueNameForClass

TiM: I removed this portion from the SetClass
function so I could use it in several other 
sections of the code.

More effective code recycling. :)
=================
*/

pclass_t ValueNameForClass ( char* s ) {

	pclass_t	pclass;
	char		buffer[MAX_QPATH];

	for ( pclass = 0; pclass < MAX_CLASSES; pclass++ )
	{
		if ( !g_classData[pclass].consoleName[0])
			break;

		trap_Cvar_VariableStringBuffer( va( "rpg_%sPass", g_classData[pclass].consoleName ), buffer, sizeof( buffer ));

		if ( !Q_stricmp( buffer, s ) )
			return pclass;
	}

	return -1;
}

/*
=================
ValueNameForClass

TiM: Created so we can get the class
name off of the actual class, and not user input

More effective code recycling. :)
=================
*/

static char* ClassForValueName ( pclass_t pclass ) {

	return g_classData[pclass].formalName;
}

/*
=================
SetClass
=================
*/
qboolean SetClass( gentity_t *ent, char *s, char *teamName, qboolean SaveToCvar ) {
	int					pclass, oldPClass;
	gclient_t			*client;
	int					clientNum;
	qboolean			wasAdmin = ent->client->LoggedAsAdmin;
	clientSession_t		*sess;
	playerState_t		*ps;
	//int OldType;

	//FIXME: check for appropriate game mod being on first

	//FIXME: can't change class while playing

	//
	// see what change is requested
	//
	client = ent->client;

	sess = &client->sess;
	ps = &client->ps;

	clientNum = client - level.clients;

	//RPG-X: TiM - Code Recyclables :)
	pclass = ValueNameForClass( s );

	if (pclass < 0 ) {
		trap_SendServerCommand( ps->clientNum, va("print \"Class doesn't exist!\n\""));
		return qfalse;
	}

	//
	// decide if we will allow the change
	//
	oldPClass = sess->sessionClass;


	if ( pclass == oldPClass )
	{
		return qfalse;
	}

	//
	// execute the class change
	//

	if ( g_classData[pclass].isAdmin ) {
		// RPG-X | Phenix | 21/11/2004 | If they go into the admin class remove other
		// admin settings
		client->AdminFailed = 0;
		client->LoggedAsAdmin = qfalse;
		client->LoggedAsDeveloper = qfalse;
	}

	sess->sessionClass = pclass;

	//SetPlayerClassCvar(ent);

	BroadcastClassChange( client, oldPClass );

	if ( teamName != NULL && SetTeam( ent, teamName ) )
	{
		return qtrue;
	}
	else
	{//not changing teams or couldn't change teams
		// get and distribute relevent paramters
		G_Client_UserinfoChanged( clientNum );

		//if in the game already, kill and respawn him, else just wait to join
		if ( sess->sessionTeam == TEAM_SPECTATOR )
		{// they go to the end of the line for tournaments
			sess->spectatorTime = level.time;
		}
		else
		{			
			//RPG-X: RedTechie - No respawn for n00bs set all info and frap that a n00b needs HERE this eliminates respawns for n00bs
			if(g_classData[pclass].isn00b/*pclass == PC_N00B*/){

				G_Client_Spawn(ent, 1, qfalse);
				ps->stats[STAT_WEAPONS] = ( 1 << WP_0 );
				ps->stats[STAT_HOLDABLE_ITEM] = HI_NONE;

				//Take some admin powerups away and give some n00b powerps ;)
				ent->flags &= ~FL_GODMODE;
				ps->powerups[PW_FLIGHT] = 0;
				ps->powerups[PW_INVIS] = 0;
				ps->powerups[PW_EVOSUIT] = 0;
				ps->powerups[PW_GHOST] = INT_MAX;

				//Turn no clip on for the N00B so he dosnt disturb rp's				
				client->noclip = qtrue;
				ent->r.contents = CONTENTS_CORPSE;


				trap_SendServerCommand(ps->clientNum, "playN00bInsult");
			}else{
				// he starts at 'base' - Removed phenix
				client->pers.teamState.state = TEAM_BEGIN;

				// Kill him (makes sure he loses flags, etc)
				ent->flags &= ~FL_GODMODE;
				ps->stats[STAT_HEALTH] = ent->health = 0;

				G_Client_Die (ent, NULL, NULL, 100000, MOD_RESPAWN);
				G_Client_Begin( clientNum, qfalse, qfalse, qfalse );
			}
		}
	}

	if ( wasAdmin ) { // RPG-X: Marcin: people requested this... - 03/01/2009
		client->LoggedAsAdmin = qtrue;
	}

	return qtrue;
}
/*
=================
StopFollowing

If the client being followed leaves the game, or you just want to drop
to free floating spectator mode
=================
*/
void StopFollowing( gentity_t *ent ) {
	playerState_t *ps = &ent->client->ps;
	clientSession_t *sess = &ent->client->sess;
	ps->persistant[ PERS_TEAM ] = TEAM_SPECTATOR;	
	sess->sessionTeam = TEAM_SPECTATOR;	
	sess->spectatorState = SPECTATOR_FREE;
	ps->pm_flags &= ~PMF_FOLLOW;
	ent->r.svFlags &= ~SVF_BOT;
	ps->clientNum = ent - g_entities;
	//don't be dead
	ps->stats[STAT_HEALTH] = ps->stats[STAT_MAX_HEALTH];
}

/*
=================
Cmd_Team_f
=================
*/
static void Cmd_Team_f( gentity_t *ent ) {
	int			oldTeam;
	char		s[MAX_TOKEN_CHARS];
	int			j;
	char		send[100];
	gentity_t	*other;
	clientSession_t *sess = &ent->client->sess;

	//RPG-X: Redtechie - haha to stupid to change teams!
	if ( g_classData[sess->sessionClass].isn00b )
	{
		Com_sprintf (send, sizeof(send), "%s ^7 doesn't know when to quit trying to change teams!", ent->client->pers.netname); 

		for (j = MAX_GENTITIES - 1; j > -1; j--) { // GSIO: change j++ to j--, also why the gell 1023 and not MAX_GENTITIES???
			if(g_entities[j].client){
				other = &g_entities[j];
				G_SayTo( ent, other, 5, COLOR_CYAN, "^7Server: ", send );
			}
		}

		trap_SendServerCommand( ent-g_entities, "print \"Sorry, you're too n00bish to handle this command.\n\"" );
		return;
	}

	oldTeam = sess->sessionTeam;

	if ( trap_Argc() != 2 ) {
		switch ( oldTeam ) {
		case TEAM_BLUE:
			trap_SendServerCommand( ent-g_entities, "print \"Blue team\n\"" );
			break;
		case TEAM_RED:
			trap_SendServerCommand( ent-g_entities, "print \"Red team\n\"" );
			break;
		case TEAM_FREE:
			trap_SendServerCommand( ent-g_entities, "print \"Free team\n\"" );
			break;
		case TEAM_SPECTATOR:
			trap_SendServerCommand( ent-g_entities, "print \"Spectator team\n\"" );
			break;
		}
		return;
	}

	trap_Argv( 1, s, sizeof( s ) );

	// if they are playing a tournement game, count as a loss
	if ( g_gametype.integer == GT_TOURNAMENT && sess->sessionTeam == TEAM_FREE )
	{
		sess->losses++;
	}

	//if this is a manual change, not an assimilation, uninitialize the clInitStatus data
	SetTeam( ent, s );

	ent->client->switchTeamTime = level.time + 2000;
}

/*
====================
Cmd_Ready_f
====================
*/
/**
*	This function is called from the ui_sp_postgame.c as a result of clicking on the
*	"next" button in non GT_TOURNAMENT games.  This replaces the old system of waiting
*	for the user to click an ATTACK or USE button to signal ready
*	(see ClientIntermissionThink())
*
*	when all clients have signaled ready, the game continues to the next match.
*/
void Cmd_Ready_f (gentity_t *ent)
{
	gclient_t *client;
	client = ent->client;

	client->readyToExit = qtrue;
}

/*
=================
Cmd_Class_f
=================
*/
static void Cmd_Class_f( gentity_t *ent ) {
	int			OldScoreclass;
	char		s[MAX_TOKEN_CHARS];
	int			j;
	char		send[100];
	gentity_t	*other;
	char		*className;
	gclient_t	*client;
	clientSession_t *sess;
	playerState_t *ps;

	if(!ent || !ent->client)
		return;

	if ( ent->flags & FL_CLAMPED )
		return;

	client = ent->client;
	sess = &client->sess;
	ps = &client->ps;

	//RPG-X: Marcin - fragger respawn prevention - 03/01/2009
	if ( (ent->s.eFlags & EF_DEAD) && (client->fraggerTime != -1) && (client->fraggerTime > level.time)) {
		trap_SendServerCommand( ent-g_entities, "print \"Sorry, you can't directly respawn after having been caught fragging.\n\"");
		return;
	}

	//RPG-X: Redtechie - haha to stupid to change classes!
	if ( g_classData[sess->sessionClass].isn00b )
	{
		Com_sprintf (send, sizeof(send), "%s ^7 doesn't know when to quit trying to change classes from n00bie", ent->client->pers.netname); 

		for (j = 0; j < MAX_CLIENTS - 1; j++) { 
			if(g_entities[j].client){
				other = &g_entities[j];
				G_SayTo( ent, other, 5, COLOR_CYAN, "^7Server: ", send );
			}
		}

		trap_SendServerCommand( ent-g_entities, "print \"Sorry, you're too n00bish to handle this command.\n\"" );
		return;
	}

	OldScoreclass = ps->persistant[PERS_SCORE];

	if ( trap_Argc() == 1 ) 
	{
		className = g_classData[sess->sessionClass].formalName;

		trap_SendServerCommand( ent-g_entities, va( "print \"\nCurrent Class: %s\nUsage: Changes the user to a different class\nCommand: Class <Class Name>\n\nType '/classlist' into the console for a more complete list\n\"", className ) );
		return;
	}

	//trying to set your class
	trap_Argv( 1, s, sizeof( s ) );

	//if this is a manual change, not an assimilation, uninitialize the clInitStatus data
	clientInitialStatus[ent->s.number].initialized = qfalse;
	if ( SetClass( ent, s, NULL, qtrue ) )
	{
		ent->client->classChangeDebounceTime = level.time + (g_classChangeDebounceTime.integer*1000);

		trap_SendServerCommand( ent-g_entities, va ( "pc %s", s ) );
	}
	ps->persistant[PERS_SCORE] = OldScoreclass;
}

/*
=================
Cmd_Cloak_f
=================
*/
static void Cmd_Cloak_f( gentity_t *ent )
{
	char			arg[16];
	char			*msg;
	playerState_t	*ps;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_CLOAK) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	ps = &ent->client->ps;

	trap_Argv(1, arg, sizeof(arg));
	if(!arg[0]) {
		G_PrintfClient(ent, "Usage: cloak 1 = cloak silent, cloak 0 = qflash\n");
		return;
	}

	G_AddEvent(ent, EV_SET_CLOAK, atoi(arg)); //GSIO01 I know this sucks but at least it works

	ent->flags ^= FL_CLOAK;
	if (!(ent->flags & FL_CLOAK))
	{
		msg = "Cloak Is Off\n";
		ps->powerups[PW_INVIS] = level.time;
	}        
	else
	{
		msg = "Cloak Is On\n";
		ps->powerups[PW_INVIS] = INT_MAX;
	}

	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));

}

/*
=================
RPG-X (Admin Feature)
Cmd_EvoSuit_f
Phenix - 8/8/2004
=================
*/
// Harry -- Can not call this from ingame...
static void Cmd_EvoSuit_f( gentity_t *ent )
{
	char		*msg;
	playerState_t *ps;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_EVOSUIT) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	ps = &ent->client->ps;

	ent->flags ^= FL_EVOSUIT;
	if (!(ent->flags & FL_EVOSUIT))
	{
		msg = "You have taken an EVA Suit off\n";
		ps->powerups[PW_EVOSUIT] = 0; //level.time;   //eh? who put this here? -J2J
	}        
	else
	{
		msg = "You have put an EVA Suit on\n";
		ps->powerups[PW_EVOSUIT] = INT_MAX;
	}
	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));

}



/*
=================
Cmd_Flight_f
=================
*/
static void Cmd_Flight_f( gentity_t *ent )
{
	char		*msg;
	playerState_t *ps;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_FLIGHT) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	ps = &ent->client->ps;

	ent->flags ^= FL_FLY;
	if (!(ent->flags & FL_FLY))
	{
		msg = "Flight Is Off\n";
		ps->powerups[PW_FLIGHT] = level.time;
	}        
	else
	{
		msg = "Flight Is On\n";
		ps->powerups[PW_FLIGHT] = INT_MAX;
	}
	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}

/*
=================
Cmd_FollowCycle_f
=================
*/
void Cmd_FollowCycle_f( gentity_t *ent, int dir ) {
	int		clientnum;
	int		original;
	clientSession_t *sess;

	if ( ent->r.svFlags&SVF_BOT )
	{//bots can't follow!
		return;
	}

	sess = &ent->client->sess;

	// if they are playing a tournement game, count as a loss
	if ( g_gametype.integer == GT_TOURNAMENT && sess->sessionTeam == TEAM_FREE ) {
		sess->losses++;
	}
	// first set them to spectator
	if ( sess->spectatorState == SPECTATOR_NOT ) {
		SetTeam( ent, "spectator" );
	}

	if ( dir != 1 && dir != -1 ) {
		G_Error( "Cmd_FollowCycle_f: bad dir %i", dir );
	}

	clientnum = sess->spectatorClient;
	original = clientnum;
	do {
		clientnum += dir;
		if ( clientnum >= level.maxclients ) {
			clientnum = 0;
		}
		if ( clientnum < 0 ) {
			clientnum = level.maxclients - 1;
		}

		// can only follow connected clients
		if ( level.clients[ clientnum ].pers.connected != CON_CONNECTED ) {
			continue;
		}

		// can't follow another spectator, including myself
		if ( level.clients[ clientnum ].sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}

		//TiM: Fixed this code so it runs like b4.
		// this is good, we can use it
		sess->spectatorClient = clientnum;
		sess->spectatorState = SPECTATOR_FOLLOW;
		return;
	} while ( clientnum != original );

	// leave it where it was

}

/*
==================
Same Class
==================
*/
static qboolean OnSameClass( gentity_t *ent1, gentity_t *ent2 ) {
	gclient_t *client1 = ent1->client;
	gclient_t *client2 = ent2->client;
	clientSession_t *sess1 = &client1->sess;
	clientSession_t *sess2 = &client2->sess;

	if ( !client1 || !client2 ) {
		if ( !client1 && !client2 )
		{
			if ( ent1->classname && ent2->classname && atoi( ent1->classname ) == atoi( ent2->classname ) )
			{
				return qtrue;
			}
		}
		else if ( !client1 )
		{
			if ( ent1->classname && atoi( ent1->classname ) == sess2->sessionClass)
			{
				return qtrue;
			}
		}
		else
		{
			if ( ent2->classname && sess1->sessionClass == atoi( ent2->classname ) )
			{
				return qtrue;
			}
		}

		return qfalse;
	}

	if ( sess1->sessionClass == sess2->sessionClass ) {
		return qtrue;
	}

	return qfalse;
}

/*
==================
G_Say
==================
*/

#define SAY_ALL		0
#define SAY_TEAM	1
#define SAY_TELL	2
#define SAY_INVAL	3
#define SAY_CLASS	4
#define SAY_ADMIN	5
#define SAY_TELL2	6
#define SAY_AREA	7

static void G_SayTo( gentity_t *ent, gentity_t *other, int mode, int color, const char *name, const char *message ) {
	clientSession_t *sess;

	if (!other) {
		return;
	}
	if (!other->inuse) {
		return;
	}
	if (!other->client) {
		return;
	}

	sess = &ent->client->sess;

	if ( mode == SAY_CLASS && !OnSameClass(ent, other) && (IsAdmin( other ) == qfalse) ) {
		return;
	}
	if ( mode == SAY_ADMIN && (IsAdmin( other ) == qfalse)) {
		return;
	}
	// no chatting to players in tournements
	if ( g_gametype.integer == GT_TOURNAMENT
		&& other->client->sess.sessionTeam == TEAM_FREE
		&& sess->sessionTeam != TEAM_FREE ) {
			return;
	}

	//RPG-X: RedTechie - N00B's cant chat at all
	if ( g_classData[sess->sessionClass].isn00b/*ent->client->sess.sessionClass == PC_N00B*/ ) {
		trap_SendServerCommand( ent-g_entities, "print \"[You're too stupid to use this command]\n\"");
		return;
	}

	//TiM : If admins want to not see broadcast messages, here's the check.
	//trap_GetUserinfo( other->client->ps.clientNum, userInfo, sizeof( userInfo ) );
	if ( ( mode == SAY_CLASS || mode == SAY_ADMIN ) && (IsAdmin(other) && (other->client->noAdminChat == qtrue || rpg_respectPrivacy.integer > 0))) {
		return;
	}	

	trap_SendServerCommand( other-g_entities, va("%s \"%s%c%c%s\"", 
		mode == SAY_TEAM ? "tchat" : "chat",
		name, Q_COLOR_ESCAPE, color, message));
}

static void G_Say( gentity_t *ent, gentity_t *target, int mode, const char *chatText ) {
	int			j;
	gentity_t	*other;
	int			color;
	char		name[80];
	char		text[150];
	char		location[64];
	char		*className;
	gclient_t	*entClient = ent->client;
	gclient_t	*tarClient = NULL;
	clientPersistant_t *entPers = &entClient->pers;
	clientPersistant_t *tarPers = NULL;
	clientSession_t *entSess = &entClient->sess;

	if(target && target->client) {
		tarClient = target->client;
		tarPers = &tarClient->pers;
	}

	if ( g_gametype.integer < GT_TEAM && mode == SAY_TEAM ) {
		mode = SAY_ALL;
	}

	// DHM - Nerve :: Don't allow excessive spamming of voice chats
	ent->voiceChatSquelch -= (level.time - ent->voiceChatPreviousTime);
	ent->voiceChatPreviousTime = level.time;

	if ( ent->voiceChatSquelch < 0 )
	{
		ent->voiceChatSquelch = 0;
	}

	if ( ent->voiceChatSquelch >= 30000 )
	{
		trap_SendServerCommand( ent-g_entities, "print \"^1Spam Protection^7: Chat Ignored\n\"" );
		if ( rpg_kickspammers.integer > 0 )
		{
			entPers->kickCount++;
			if ( entPers->kickCount >= rpg_kickspammers.integer )
			{
				trap_SendConsoleCommand( EXEC_APPEND, va("kick \"%i\"\n", entClient->ps.clientNum ) );
			}
		}
		return;
	}

	if ( rpg_chatsallowed.integer > 0)
	{
		ent->voiceChatSquelch += (34000 / rpg_chatsallowed.integer );
	}
	else
	{
		return;
	}
	// dhm

	switch ( mode ) {
	default:
	case SAY_ALL:
		G_LogPrintf( "say: %s: %s (%s)\n", entPers->netname, chatText, entPers->ip );
		Com_sprintf (name, sizeof(name), "^7 %s%c%c ^7: ", entPers->netname, Q_COLOR_ESCAPE, COLOR_WHITE );
		color = COLOR_WHITE;
		break;
	case SAY_TEAM:
		// Team Say has become say to all for RPG-X
		G_LogPrintf( "say: %s: %s (%s)\n", entPers->netname, chatText, entPers->ip );
		Com_sprintf (name, sizeof(name), "^7 %s%c%c^7: ", entPers->netname, Q_COLOR_ESCAPE, COLOR_WHITE );
		color = COLOR_WHITE;
		break;
	case SAY_TELL:
		if(tarPers)
			Com_sprintf (name, sizeof(name), "^7%s ^7from %s%c%c: ", tarPers->netname, entPers->netname, Q_COLOR_ESCAPE, COLOR_WHITE );
		else return;
		color = COLOR_MAGENTA;
		break;
	case SAY_TELL2:
		if(tarPers)
			Com_sprintf (name, sizeof(name), "^7%s ^7from %s%c%c: ", tarPers->netname, entPers->netname, Q_COLOR_ESCAPE, COLOR_WHITE );
		else return;
		color = COLOR_MAGENTA;
		target = ent;
		break;
		// CLASS SAY
	case SAY_CLASS:

		className = g_classData[entSess->sessionClass].formalName;

		G_LogPrintf( "sayclass: %s: %s (%s)\n", entPers->netname, chatText, entPers->ip);
		Com_sprintf (name, sizeof(name), "^7To all %s^7's from %s%c%c: ", className, entPers->netname, Q_COLOR_ESCAPE, COLOR_WHITE );
		color = COLOR_YELLOW;
		break;
	case SAY_ADMIN:
		if (G_Client_GetLocationMsg(ent, location, sizeof(location)) && tarPers)
			Com_sprintf (name, sizeof(name), "[%s%c%c] [%s] (%s): ", entPers->netname, Q_COLOR_ESCAPE, COLOR_WHITE, tarPers->netname, location );
		else if(tarPers)
			Com_sprintf (name, sizeof(name), "[%s%c%c ^7To %s^7]: ", entPers->netname, Q_COLOR_ESCAPE, COLOR_WHITE, tarPers->netname );
		else return;
		color = COLOR_CYAN;
		target = NULL;
		break;
	case SAY_INVAL:
		G_LogPrintf( "Invalid During Intermission: %s: %s (%s)\n", entPers->netname, chatText, entPers->ip );
		Com_sprintf (name, sizeof(name), "[Invalid During Intermission%c%c]: ", Q_COLOR_ESCAPE, COLOR_WHITE );
		color = COLOR_GREEN;
		target = ent;
		break;
	}

	Q_strncpyz( text, chatText, sizeof(text) );

	if ( target ) {
		G_SayTo( ent, target, mode, color, name, text );
		return;
	}

	// echo the text to the console
	if ( g_dedicated.integer ) {
		G_Printf( "%s%s\n", name, text);
	}

	// send it to all the apropriate clients
	for (j = level.maxclients - 1; j > -1; j--) { //GSIO changed from i++ to i--
		other = &g_entities[j];
		if(!other)
			continue;
		if(!other->client)
			continue;

		G_SayTo( ent, other, mode, color, name, text );
	}
}

/*
==================
Cmd_SayArea
==================
*/
//RPG-X: J2J - Wrote the say area function becuase the Say function is bloated and wierd memory errors appear.
static void Cmd_SayArea( gentity_t *ent, char* text)
{

	gentity_t*			OtherPlayer;			//Entity pointers to other players in game (used in loop)
	int					i;						//Loop Counter
	gclient_t*			client =  ent->client;
	clientPersistant_t* pers = &client->pers;

	if ( g_dedicated.integer ) {
		G_Printf( "%s said to area: %s\n", pers->netname, text);
	}


	//Loop Through Clients on the server
	//RPG-X: J2J - BugFix: used to be connected clients which meant most clients were missed out and other bugs.
	//					   Change to max clients and the loop ingores invalid players.
	for(i = level.maxclients - 1; i > -1; i--) //GSIO i-- now instead of i++
	{

		OtherPlayer = &g_entities[i];			//Point OtherPlayer to next player

		//Send message to admins warning about command being used.
		//TiM - since double spamming is annoying, ensure that the target admin wants this alert
		if ( !OtherPlayer->client->noAdminChat )
			G_SayTo( ent, OtherPlayer, SAY_ADMIN, COLOR_CYAN, va("%s ^7said to area: ", pers->netname ), text  ); //^2%s

		//Check is OtherPlayer is valid
		if ( !OtherPlayer || !OtherPlayer->inuse || !OtherPlayer->client ) 
		{
			continue;
		}

		//TiM - I have a better solution. the trap_inPVS function lets u see if two points are within the same Vis cluster
		//in the BSP tree. That should mean as long as they're in the same room, regardless if they can see each other or not,
		//they'll get the message

		if ( trap_InPVS( client->ps.origin, OtherPlayer->client->ps.origin ) )
		{
			//Communicate to the player	
			G_SayTo( ent, OtherPlayer, SAY_TELL, COLOR_GREEN, va("%s^2: ", pers->netname), text );
		}

		OtherPlayer = NULL;					//Reset pointer ready for next iteration.
	}

}

/*
==================
Cmd_Say_f
==================
*/
static void Cmd_Say_f( gentity_t *ent, int mode, qboolean arg0 ) {
	char		*p;

	if ( trap_Argc () < 2 && !arg0 ) {
		return;
	}

	if (arg0)
	{
		p = ConcatArgs( 0 );
	}
	else
	{
		p = ConcatArgs( 1 );
	}

	if(mode == SAY_ALL)
		Cmd_SayArea(ent, p);
	else
		G_Say( ent, NULL, mode, p );
}

/*
==================
Cmd_Tell_f
==================
*/
static void Cmd_Tell_f( gentity_t *ent ) {
	int			targetNum;
	gentity_t	*target;
	char		*p;
	char		arg[MAX_TOKEN_CHARS];

	if ( trap_Argc () < 2 ) {
		return;
	}

	trap_Argv( 1, arg, sizeof( arg ) );
	targetNum = atoi( arg );
	if ( targetNum < 0 || targetNum >= level.maxclients ) {
		return;
	}

	target = &g_entities[targetNum];
	if ( !target || !target->inuse || !target->client ) {
		return;
	}

	p = ConcatArgs( 2 );

	G_LogPrintf( "tell: %s to %s: %s (%s)\n", ent->client->pers.netname, target->client->pers.netname, p, ent->client->pers.ip );
	G_Say( ent, target, SAY_TELL, p );
	G_Say( ent, target, SAY_TELL2, p );
	G_Say( ent, target, SAY_ADMIN, p);
}


static char	*gc_orders[] = {
	"hold your position",
	"hold this position",
	"come here",
	"cover me",
	"guard location",
	"search and destroy",
	"report"
};

static void Cmd_GameCommand_f( gentity_t *ent ) {
	int		player;
	int		order;
	char	str[MAX_TOKEN_CHARS];

	trap_Argv( 1, str, sizeof( str ) );
	player = atoi( str );
	trap_Argv( 2, str, sizeof( str ) );
	order = atoi( str );

	if ( player < 0 || player >= MAX_CLIENTS ) {
		return;
	}
	if ( order < 0 || order > sizeof(gc_orders)/sizeof(char *) ) {
		return;
	}
	G_Say( ent, &g_entities[player], SAY_TELL, gc_orders[order] );
	G_Say( ent, ent, SAY_TELL, gc_orders[order] );
}

/*
==================
Cmd_Where_f
==================
*/
static void Cmd_Where_f( gentity_t *ent ) {
	trap_SendServerCommand( ent-g_entities, va("print \"%s\n\"", vtos( ent->s.origin ) ) );
}


/*
==================
Cmd_CallVote_f
==================
*/
static void Cmd_CallVote_f( gentity_t *ent ) {
	int		i;
	char	arg1[MAX_STRING_TOKENS];
	char	arg2[MAX_STRING_TOKENS];

	if ( !g_allowVote.integer ) {
		trap_SendServerCommand( ent-g_entities, "print \"Voting not allowed here.\n\"" );
		return;
	}

	if ( level.voteTime ) {
		trap_SendServerCommand( ent-g_entities, "print \"A vote is already in progress.\n\"" );
		return;
	}

	// make sure it is a valid command to vote on
	trap_Argv( 1, arg1, sizeof( arg1 ) );
	trap_Argv( 2, arg2, sizeof( arg2 ) );

	if( strchr( arg1, ';' ) || strchr( arg2, ';' ) ) {
		trap_SendServerCommand( ent-g_entities, "print \"Invalid Vote.\n\"" );
		return;
	}

	if ( !Q_stricmp( arg1, "map_restart" ) ) {
	} else if ( !Q_stricmp( arg1, "map" ) ) {
	} else if ( !Q_stricmp( arg1, "kick" ) && rpg_allowvote.integer > 0 ) {
	} else {
		trap_SendServerCommand( ent-g_entities, "print \"Invalid Vote Command.\n\"" );
		return;
	}

	if ( rpg_allowspmaps.integer != 1 )
	{
		if ( !Q_stricmp( arg1, "map" ) &&
			( !Q_stricmp( arg2, "_brig" )
			|| !Q_stricmp( arg2, "_holodeck_camelot" ) || !Q_stricmp( arg2, "_holodeck_firingrange" ) || !Q_stricmp( arg2, "_holodeck_garden" ) || !Q_stricmp( arg2, "_holodeck_highnoon" ) || !Q_stricmp( arg2, "_holodeck_minigame" ) || !Q_stricmp( arg2, "_holodeck_proton" ) || !Q_stricmp( arg2, "_holodeck_proton2" ) || !Q_stricmp( arg2, "_holodeck_temple" ) || !Q_stricmp( arg2, "_holodeck_warlord" )
			|| !Q_stricmp( arg2, "borg1" ) || !Q_stricmp( arg2, "borg2" ) || !Q_stricmp( arg2, "borg3" ) || !Q_stricmp( arg2, "borg4" ) || !Q_stricmp( arg2, "borg5" ) || !Q_stricmp( arg2, "borg6" )
			|| !Q_stricmp( arg2, "dn1" ) || !Q_stricmp( arg2, "dn2" ) || !Q_stricmp( arg2, "dn3" ) || !Q_stricmp( arg2, "dn4" ) || !Q_stricmp( arg2, "dn5" ) || !Q_stricmp( arg2, "dn6" ) || !Q_stricmp( arg2, "dn8" )
			|| !Q_stricmp( arg2, "forge1" ) || !Q_stricmp( arg2, "forge2" ) || !Q_stricmp( arg2, "forge3" ) || !Q_stricmp( arg2, "forge4" ) || !Q_stricmp( arg2, "forge5" ) || !Q_stricmp( arg2, "forgeboss" )
			|| !Q_stricmp( arg2, "holodeck" )
			|| !Q_stricmp( arg2, "scav1" ) || !Q_stricmp( arg2, "scav2" ) || !Q_stricmp( arg2, "scav3" ) || !Q_stricmp( arg2, "scav3b" ) || !Q_stricmp( arg2, "scav4" ) || !Q_stricmp( arg2, "scav5" ) || !Q_stricmp( arg2, "scavboss" )
			|| !Q_stricmp( arg2, "stasis1" ) || !Q_stricmp( arg2, "stasis2" ) || !Q_stricmp( arg2, "stasis3" )
			|| !Q_stricmp( arg2, "tour/deck01" ) || !Q_stricmp( arg2, "tour/deck02" ) || !Q_stricmp( arg2, "tour/deck03" ) || !Q_stricmp( arg2, "tour/deck04" ) || !Q_stricmp( arg2, "tour/deck05" ) || !Q_stricmp( arg2, "tour/deck08" ) || !Q_stricmp( arg2, "tour/deck09" ) || !Q_stricmp( arg2, "tour/deck10" ) || !Q_stricmp( arg2, "tour/deck11" ) || !Q_stricmp( arg2, "tour/deck15" )
			|| !Q_stricmp( arg2, "tutorial" )
			|| !Q_stricmp( arg2, "voy1" ) || !Q_stricmp( arg2, "voy13" ) || !Q_stricmp( arg2, "voy14" ) || !Q_stricmp( arg2, "voy15" ) || !Q_stricmp( arg2, "voy16" ) || !Q_stricmp( arg2, "voy17" ) || !Q_stricmp( arg2, "voy2" ) || !Q_stricmp( arg2, "voy20" ) || !Q_stricmp( arg2, "voy3" ) || !Q_stricmp( arg2, "voy4" ) || !Q_stricmp( arg2, "voy5" ) || !Q_stricmp( arg2, "voy6" ) || !Q_stricmp( arg2, "voy7" ) || !Q_stricmp( arg2, "voy8" ) || !Q_stricmp( arg2, "voy9" ) ) )
		{
			trap_SendServerCommand( ent-g_entities, "print \"Invalid Map.\n\"" );
			return;
		}
	}

	if ( !Q_stricmp( arg1, "kick" ) ) {
		//TiM - if we're callvoting to kick an admin, deny it
		int id = ClientNumberFromString( ent, arg2 );

		//Harry Young - Check if clientnum is valid, else we get a crash
		if ( id == -1 ) {
			trap_SendServerCommand( ent-g_entities, "print \"Error: no such client.\n\"" );
			return;
		}

		//TiM - if we're callvoting to kick an admin, deny it
		//only publicly broadcasted admins are checked
		//the hidden admin login people are not to protect their nature
		if ( g_classData[g_entities[id].client->ps.persistant[PERS_CLASS]].isAdmin ) {
			trap_SendServerCommand( ent-g_entities, "print \"Error: You are not allowed to kick admins.\n\"" );
			return;
		}
	}

	ent->client->pers.voteCount++;

	if ( !Q_stricmp( arg1, "map" ) )
	{
		char	s[MAX_STRING_CHARS];
		trap_Cvar_VariableStringBuffer( "nextmap", s, sizeof(s) );
		if (*s)
		{
			Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %s; set nextmap \"%s\n\"", arg1, arg2, s );
		}
		else
		{
			Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %s", arg1, arg2 );
		}
	}
	else if ( !Q_stricmp( arg1, "map_restart" ) ) 
	{
		char	serverInfo[MAX_TOKEN_CHARS];
		trap_GetServerinfo( serverInfo, sizeof( serverInfo ) );		
		Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %s", "map", Info_ValueForKey( serverInfo, "mapname" ) );
	}
	else
	{
		Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %s", arg1, arg2 );
	}

	trap_SendServerCommand( -1, va("print \"%s called a vote.\n\"", ent->client->pers.netname ) );

	// start the voting, the caller autoamtically votes yes
	level.voteTime = level.time;
	level.voteYes = 1;
	level.voteNo = 0;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		level.clients[i].ps.eFlags &= ~EF_VOTED;
	}
	ent->client->ps.eFlags |= EF_VOTED;

	trap_SetConfigstring( CS_VOTE_TIME, va("%i", level.voteTime ) );
	trap_SetConfigstring( CS_VOTE_STRING, level.voteString );	
	trap_SetConfigstring( CS_VOTE_YES, va("%i", level.voteYes ) );
	trap_SetConfigstring( CS_VOTE_NO, va("%i", level.voteNo ) );	
}

/*
==================
Cmd_Vote_f
==================
*/
static void Cmd_Vote_f( gentity_t *ent ) {
	char		msg[64];

	if ( !level.voteTime ) {
		trap_SendServerCommand( ent-g_entities, "print \"No vote in progress.\n\"" );
		return;
	}
	if ( ent->client->ps.eFlags & EF_VOTED ) {
		trap_SendServerCommand( ent-g_entities, "print \"Vote already cast.\n\"" );
		return;
	}
	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		trap_SendServerCommand( ent-g_entities, "print \"Spectators cannot vote.\n\"" );
		return;
	}

	trap_SendServerCommand( ent-g_entities, "print \"Vote cast.\n\"" );

	ent->client->ps.eFlags |= EF_VOTED;

	trap_Argv( 1, msg, sizeof( msg ) );

	if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
		level.voteYes++;
		trap_SetConfigstring( CS_VOTE_YES, va("%i", level.voteYes ) );
	} else {
		level.voteNo++;
		trap_SetConfigstring( CS_VOTE_NO, va("%i", level.voteNo ) );	
	}
	// a majority will be determined in G_CheckVote, which will also account
	// for players entering or leaving
}

/*
==================
Cmd_ForceVote_f
==================
*/
static void Cmd_ForceVote_f( gentity_t *ent ) {
	char		msg[64];
	gclient_t	*client = ent->client;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_FORCEVOTE) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	if(!rpg_adminVoteOverride.integer) { // vote override?
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: Overwrite is not allowed on this server.\n\" ") );
		return;
	}

	if ( !level.voteTime ) {
		trap_SendServerCommand( ent-g_entities, "print \"No vote in progress.\n\"" );
		return;
	}
	if ( client->ps.eFlags & EF_VOTED ) {
		trap_SendServerCommand( ent-g_entities, "print \"Vote already cast.\n\"" );
		return;
	}
	if ( client->sess.sessionTeam == TEAM_SPECTATOR ) {
		trap_SendServerCommand( ent-g_entities, "print \"Spectators cannot vote.\n\"" );
		return;
	}

	trap_SendServerCommand( ent-g_entities, "print \"Vote force cast.\n\"" );

	client->ps.eFlags |= EF_VOTED;

	trap_Argv( 1, msg, sizeof( msg ) );

	if(msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1') {
		level.voteYes += 999;
		trap_SetConfigstring(CS_VOTE_YES, va("%i", level.voteYes));
		trap_SendServerCommand( ent-g_entities, "print \"You have overridden the vote with yes.\n\"");
	} else {
		level.voteNo += 999;
		trap_SetConfigstring(CS_VOTE_NO, va("%i", level.voteNo));
		trap_SendServerCommand( ent-g_entities, "print \"You have overridden the vote with no.\n\"");
	}

}


/*
=================
Cmd_SetViewpos_f
=================
*/
static void Cmd_SetViewpos_f( gentity_t *ent ) {
	vec3_t		origin, angles;
	char		buffer[MAX_TOKEN_CHARS];
	int			i;

	if ( !g_cheats.integer && !IsAdmin( ent )) {
		trap_SendServerCommand( ent-g_entities, va("print \"Cheats are not enabled on this server.\n\""));
		return;
	}
	if ( trap_Argc() != 5 ) {
		trap_SendServerCommand( ent-g_entities, va("print \"usage: setviewpos x y z yaw\n\""));
		return;
	}

	VectorClear( angles );
	for ( i = 0 ; i < 3 ; i++ ) {
		trap_Argv( i + 1, buffer, sizeof( buffer ) );
		origin[i] = atof( buffer );
	}

	trap_Argv( 4, buffer, sizeof( buffer ) );
	angles[YAW] = atof( buffer );

	TeleportPlayer( ent, origin, angles, TP_NORMAL );
}

/*
=================
Cmd_ForceName_f
=================
*/
static void Cmd_ForceName_f( gentity_t *ent ) {
	gclient_t	*cl;
	gentity_t *other;
	gentity_t *sayA;
	int j;
	char send[100];
	char  str[MAX_TOKEN_CHARS];
	char  *str2;
	char clientCmd[MAX_INFO_STRING];

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_FORCEPARM) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	// find the player
	trap_Argv( 1, str, sizeof( str ) );

	if ( !str[0] ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"\nUsage: User forces another player's name to what they specify\nCommand: ForceName <Player's ID Number> \"[New Name]\"\n\" ") );
		return;
	}

	cl = ClientForString( str );
	if ( !cl ) { 
		return;
	}
	other = g_entities + cl->ps.clientNum;

	//Get there new name
	str2 = ConcatArgs( 2 );

	//Print out some chat text
	G_LogPrintf( "%s renamed %s to %s (%s)\n", ent->client->pers.netname, other->client->pers.netname, str2, ent->client->pers.ip );

	Com_sprintf (send, sizeof(send), "%s ^7renamed %s ^7to %s", ent->client->pers.netname, other->client->pers.netname, str2);

	for (j = MAX_GENTITIES - 1; j > -1; j--) { // GSIO change from i++ for to i--, uses MAX_GENTITIES now
		if(g_entities[j].client){
			sayA = &g_entities[j];
			G_SayTo( ent, sayA, SAY_ADMIN, COLOR_CYAN, "^7Server: ", send );
		}
	}

	//Set the name
	//TiM: Remember to actually load the User's Info into the variable before modifying it
	//The server was parsing a NULL string. O_o

	//send a command to the client and the client will do all this automatically
	Com_sprintf( clientCmd, sizeof(clientCmd), "changeClientInfo name \"%s\"", str2 );
	trap_SendServerCommand( cl->ps.clientNum, clientCmd );		
}

/*
=================
Cmd_ShakeCamera_f

TiM's "How inefficient this is" rant:
Oh jeez! Cut off my hands and gouge out my eyes!!!!
This has to be the most in-efficient
function I've ever seen! >.<
You don't need passworded checksum 
validation type thingys to protect this from n00b players!
If you just use an InfoString instead, that's all the security
you need! Clients can't access that API!
All this is doing is contributing to un-necessary procs
and potentially adding more lag to the game O_o
=================
*/
static void Cmd_ShakeCamera_f( gentity_t *ent ) {
	char arg_intensity[5];
	char arg_duration[5];
	int duration;
	int intensity;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_FX) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_Argv( 1, arg_intensity, sizeof( arg_intensity ) );

	//TiM : More userfriendly
	if ( !arg_intensity[0] ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"\nUsage: User makes every player's screen shake for X seconds\nCommand: Shake [Intensity] [Duration]\n\" ") );
		return;
	}

	//TiM: Intensity can be a float
	intensity = atoi( arg_intensity );

	if(intensity <= 0 ){
		return;
	}
	//I'm putting much faith in admins here
	else if(intensity > 9999 ){
		intensity = 9999;
	}

	trap_Argv( 2, arg_duration, sizeof( arg_duration ) );
	duration = atoi( arg_duration );

	if(duration < 1){
		duration = 1;
	}
	if(duration > 10000){
		duration = 10000;
	}

	//More or less, this will be the time the effect ends.  As long as cg.time is 
	//in sync more or less this should be better.  We can't use a raw value, or else
	//joining clients will be out of sync
	duration = ( duration * 1000 ) + (level.time - level.startTime );

	trap_SetConfigstring( CS_CAMERA_SHAKE, va( "%i %i", intensity, duration ) );
}

/*
=================
Cmd_ForceClass_f
=================
*/
// Harry -- How to call this one?
static void Cmd_ForceClass_f( gentity_t *ent ) {
	int			targetNum;
	int			j;
	gentity_t	*target;
	gentity_t	*other;
	char		send[80];
	char		arg[MAX_TOKEN_CHARS];
	char		s[MAX_TOKEN_CHARS];
	int			OldScoreclass;

	if ( trap_Argc () < 2 ) {
		return;
	}

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_FORCEPARM) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_Argv( 1, arg, sizeof( arg ) );

	targetNum = atoi( arg );
	if ( targetNum < 0 || targetNum >= level.maxclients ) {
		return;
	}

	target = &g_entities[targetNum];
	if ( !target || !target->inuse || !target->client ) {
		return;
	}

	//RPG-X: RedTechie - Save old rank
	OldScoreclass = target->client->ps.persistant[PERS_SCORE];

	if ( trap_Argc() != 3 ) 
	{//Just asking what class they're on
		char	*className;

		className = g_classData[ent->client->sess.sessionClass].formalName;

		trap_SendServerCommand( ent-g_entities, va( "print \"\nCurrent Class: %s\nUsage: Changes the user to a different class\nCommand: Class <Class Name>\n\nType '/classlist' into the console for a more complete list\n\"", className ) );
		return;
	}

	//trying to set there class
	trap_Argv( 2, s, sizeof( s ) );

	//if this is a manual change, not an assimilation, uninitialize the clInitStatus data
	clientInitialStatus[target->s.number].initialized = qfalse;
	if ( !SetClass( target, s, NULL, qfalse ) ) {
		trap_SendServerCommand( ent-g_entities, "print \"ERROR: Was unable to change class\n\" " );
		return;
	}

	//RPG-X: RedTechie - Update rank to old
	target->client->ps.persistant[PERS_SCORE] = OldScoreclass;

	Com_sprintf (send, sizeof(send), "%s ^7put %s into the ^7%s class", ent->client->pers.netname, target->client->pers.netname, ClassForValueName( target->client->sess.sessionClass ) );

	for (j = 0; j < MAX_CLIENTS; j++) {
		if(g_entities[j].client){
			other = &g_entities[j];
			G_SayTo( ent, other, SAY_ADMIN, COLOR_CYAN, "^7Server: ", send );
		}
	}

	G_LogPrintf( "%s made %s a %s (%s)\n", ent->client->pers.netname, target->client->pers.netname, s, ent->client->pers.ip);
}

/*
=================
Cmd_ForceKill_f					//J2J to others: PLEASE CAN  WE COMMENT THE CODE!!!! :P
=================
*/
static void Cmd_ForceKill_f( gentity_t *ent ) {
	int			targetNum;
	int			j, p;
	char		send[80];
	gentity_t	*target = NULL;
	gentity_t	*other;
	char		arg[MAX_TOKEN_CHARS];
	playerState_t *ps;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_FORCEKILL) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	if ( trap_Argc () < 2 ) {
		return;
	}

	trap_Argv( 1, arg, sizeof( arg ) );
	targetNum = atoi( arg );
	if ( targetNum >= level.maxclients ) {
		return;
	}

	if ( targetNum == -1 ) {
		for(p = 0; p < level.maxclients; p++)
		{
			target = &g_entities[p];
			if ( !target || !target->inuse || !target->client ) {
				return;
			}

			ps = &target->client->ps;

			lastKillTime[ps->clientNum] = level.time;
			target->flags &= ~FL_GODMODE;										//Bypass godmode (?)

			//RPG-X: Medics revive Support for suiciding
			if(rpg_medicsrevive.integer == 1){
				ps->stats[STAT_WEAPONS] = ( 1 << WP_0 );
				ps->stats[STAT_HOLDABLE_ITEM] = HI_NONE;
				ps->stats[STAT_HEALTH] = target->health = 1;
				G_Client_Die (target, target, target, 100000, MOD_FORCEDSUICIDE);
			}else{
				ps->stats[STAT_HEALTH] = target->health = 0;
				G_Client_Die (target, target, target, 100000, MOD_FORCEDSUICIDE);
			}
		} // end iterations

		Com_sprintf (send, sizeof(send), "%s ^7forced %s^7's death", ent->client->pers.netname, target->client->pers.netname);

		for (j = 0; j < MAX_CLIENTS - 1; j++) {
			if(g_entities[j].client){
				other = &g_entities[j];
				G_SayTo( ent, other, SAY_ADMIN, COLOR_CYAN, "^7Server: ", send );
			}
		}

		G_LogPrintf( "%s forced everyones death\n", ent->client->pers.netname );
	}

	else //one vs all

	{
		target = &g_entities[targetNum];
		if ( !target || !target->inuse || !target->client ) {
			return;
		}

		ps = &target->client->ps;

		lastKillTime[ps->clientNum] = level.time;
		target->flags &= ~FL_GODMODE;										//Bypass godmode (?)

		//RPG-X: Medics revive Support for suiciding
		if(rpg_medicsrevive.integer == 1){
			ps->stats[STAT_WEAPONS] = ( 1 << WP_0 );
			ps->stats[STAT_HOLDABLE_ITEM] = HI_NONE;
			ps->stats[STAT_HEALTH] = target->health = 1;
			G_Client_Die (target, target, target, 100000, MOD_FORCEDSUICIDE);
		}else{
			ps->stats[STAT_HEALTH] = target->health = 0;
			G_Client_Die (target, target, target, 100000, MOD_FORCEDSUICIDE);
		}

		Com_sprintf (send, sizeof(send), "%s ^7forced %s^7's death", ent->client->pers.netname, target->client->pers.netname);

		for (j = 0; j < MAX_CLIENTS - 1; j++) {
			if(g_entities[j].client){
				other = &g_entities[j];
				G_SayTo( ent, other, SAY_ADMIN, COLOR_CYAN, "^7Server: ", send );
			}
		}

		G_LogPrintf( "%s forced %s's death (%s)\n", ent->client->pers.netname, target->client->pers.netname, ent->client->pers.ip );
	}
}

/*
=================
ForceKillRadius			(RPG-X: J2J)
=================
*/
//Array used to store each players last use of the FKR command. (though only applies to admins)
int LastFKRadius[MAX_CLIENTS];

static void Cmd_ForceKillRadius_f( gentity_t *ent)
{
	gentity_t* OtherPlayer;					//Entity pointers to other players in game (used in loop)
	char		arg[MAX_TOKEN_CHARS];		//Arguments
	int i;									//Loop Counter
	float DistanceVector[2];				//Distance Vector from client caller to victim
	float Distance;							//Real Distance from client caller to victim
	float range;
	int kill_self;
	int clientNum = ent->client->ps.clientNum;
	playerState_t *ePs = &ent->client->ps;
	playerState_t *oPs;

	memset(arg,0,MAX_TOKEN_CHARS);			//Clear aray.

	//Grab range from command
	trap_Argv( 1, arg, sizeof( arg ) );			//Gets 1st
	range = atoi(arg);

	memset(arg,0,MAX_TOKEN_CHARS);			//Clear aray.

	//Grab kill_self from command
	trap_Argv( 2, arg, sizeof( arg ) );			//Gets 2nd	
	kill_self = atoi(arg);

	//If client isn't in admin class, exit and disallow command.

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_FORCEKILL) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	//If forcekillradius is not enabled in cvar do not allow it to continue.
	if(rpg_forcekillradius.integer != 1)				
	{
		return;											
	}
	//If last time FKR was used before the wait time is up..
	if( ( rpg_forcekillradiuswaittime.integer - (level.time - LastFKRadius[clientNum]) > 0) )
	{
		//Send message to client informing them so they can't flood.
		trap_SendServerCommand( clientNum, va("cp \"Cannot use Force Kill Radius Command for %d seconds", (int)(( rpg_forcekillradiuswaittime.integer - (level.time - LastFKRadius[clientNum]) ) * 0.001 ))); // GSIO was / 1000
		return;
	}

	//Loop Through Clients on the server
	for(i = 0; i < level.numConnectedClients; i++)
	{
		//If current client == caller client, skip this iteration.
		if(i == clientNum)
		{
			continue;
		}

		OtherPlayer = &g_entities[i];			//Point OtherPlayer to next player

		//Send message to admins warning about command being used.
		G_SayTo( ent, OtherPlayer, SAY_ADMIN, COLOR_CYAN, "^7Server: ", va("%s has triggred a force kill radius command", ent->client->pers.netname) );

		//Check is OtherPlayer is valid
		if ( !OtherPlayer || !OtherPlayer->inuse || !OtherPlayer->client ) 
		{
			continue;
		}


		//If player is allready dead or in spectator, skip this iteration
		if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR /*|| (ent->client->ps.eFlags&EF_ELIMINATED)*/ )
		{
			OtherPlayer = NULL;					//Reset pointer ready for next iteration.
			continue;
		}

		oPs = &OtherPlayer->client->ps;


		//If current player is higer or lower by 1.5 units, do not kill them (probably on another deck), continue to next loop.
		if( (oPs->origin[2] > ePs->origin[2]+15.0f) || (oPs->origin[2] < ePs->origin[2]-15.0f) )
		{
			OtherPlayer = NULL;					//Reset pointer ready for next iteration.
			continue;
		}

		//Vector subtraction, to get distance vector (using player positions as vectors
		DistanceVector[0] =  ePs->origin[0] - oPs->origin[0];
		DistanceVector[1] =  ePs->origin[1] - oPs->origin[1];
		//Get Length of Distance Vector
		Distance = sqrt( (DistanceVector[0] * DistanceVector[0]) + (DistanceVector[1] * DistanceVector[1]) );

		//If distance is within the radius given...
		if( Distance <= range)
		{
			//Kill the player.	
			lastKillTime[oPs->clientNum] = level.time;
			OtherPlayer->flags &= ~FL_GODMODE;							//Bypass godmode (?)
			//RPG-X: Medics revive Support for suiciding
			if(rpg_medicsrevive.integer == 1){
				oPs->stats[STAT_WEAPONS] = ( 1 << WP_0 );
				oPs->stats[STAT_HOLDABLE_ITEM] = HI_NONE;
				oPs->stats[STAT_HEALTH] = OtherPlayer->health = 1;
				G_Client_Die(OtherPlayer,OtherPlayer,OtherPlayer,100000, MOD_FORCEDSUICIDE);
			}else{
				oPs->stats[STAT_HEALTH] = OtherPlayer->health = 0;
				G_Client_Die(OtherPlayer,OtherPlayer,OtherPlayer,100000, MOD_FORCEDSUICIDE);
			}
		}

		OtherPlayer = NULL;					//Reset pointer ready for next iteration.
	}

	//If kill_self != 0 (they want to kill them selves too)
	if(kill_self != 0)										
	{
		lastKillTime[clientNum] = level.time;
		ent->flags &= ~FL_GODMODE;							//Bypass godmode (?)
		//RPG-X: Medics revive Support for suiciding
		if(rpg_medicsrevive.integer == 1){
			ePs->stats[STAT_WEAPONS] = ( 1 << WP_0 );
			ePs->stats[STAT_HOLDABLE_ITEM] = HI_NONE;
			ePs->stats[STAT_HEALTH] = ent->health = 1;
			G_Client_Die(ent,ent,ent,100000, MOD_FORCEDSUICIDE);
		}else{
			ePs->stats[STAT_HEALTH] = ent->health = 0;
			G_Client_Die(ent,ent,ent,100000, MOD_FORCEDSUICIDE);
		}
	}

	LastFKRadius[clientNum] = level.time; // - rpg_forcekillradiuswaittime.integer;
}

/*
=================
TargetKick			(RPG-X: J2J)
=================
*/
static void Cmd_TargetKick_f( gentity_t *ent)
{
	gentity_t	*target, *other;		//Target entity, and other used to tell the
	trace_t		trace;					//Used to trace target
	vec3_t		src, dest, vf;			//Used to find target
	int i;								//Loop counter

	//Disallow if not in admin class

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_KICK) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	//////////////////////////////////////
	//All this code below finds the target entity

	VectorCopy( ent->r.currentOrigin, src );
	src[2] += ent->client->ps.viewheight;

	AngleVectors( ent->client->ps.viewangles, vf, NULL, NULL );

	//extend to find end of use trace
	VectorMA( src, -6, vf, src );//in case we're inside something?
	VectorMA( src, 1340, vf, dest );//128+6

	//Trace ahead to find a valid target
	trap_Trace( &trace, src, vec3_origin, vec3_origin, dest, ent->s.number, CONTENTS_BODY );

	if ( trace.fraction == 1.0f || trace.entityNum < 0 )
	{
		trap_SendConsoleCommand( EXEC_APPEND, va("echo No target in range to kick.") );
		return;
	}

	target = &g_entities[trace.entityNum];

	if ( !target->client || trace.entityNum > MAX_CLIENTS )
	{
		trap_SendConsoleCommand( EXEC_APPEND, va("echo That target cannot be kicked.") );
		return;
	}

	////////////////////////////////

	//Send a kick command to kick the target.
	trap_SendConsoleCommand( EXEC_APPEND, va("kick \"%i\"\n", target->client->ps.clientNum ) );

	for (i = 0; i < level.maxclients; i++)
	{
		other = &g_entities[i];
		G_SayTo( ent, other, SAY_ADMIN, COLOR_CYAN, "^7Server: ", va("%s has kicked %s", ent->client->pers.netname, target->client->pers.netname) );
	}
}

/*
=================
Drag			(RPG-X: J2J)
=================
*/

RPGX_DragData DragDat[MAX_CLIENTS];
// /DragPlayer <ID> <range>
// This will start a drag on a player or adjust the range of a drag if its allready started
static void Cmd_Drag_f( gentity_t *ent)
{
	char		arg[MAX_TOKEN_CHARS];		//Arguments
	int			ID, i;
	float		range;
	int			clientNum;

	if(!ent || !ent->client)
		return;

	//If client isn't in admin class, exit and disallow command.

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_DRAG) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	clientNum = ent->client->ps.clientNum;

	memset(arg,0,MAX_TOKEN_CHARS);			//Clear aray.
	//Grab range from command
	trap_Argv( 1, arg, sizeof( arg ) );			//Gets 1st

	//TiM : Make this a bit more user friendly
	if ( !arg[0] ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( clientNum, va("print \"\nUsage: User can forcefully drag another player\nCommand: Drag [Player ID] [Distance between both Players]\n\" ") );
		return;
	}

	ID = atoi(arg);

	if ( ID < 0 || ID >= level.maxclients )
	{
		return;
	}

	//This prevents a interesting form of no clip where you could move at a speed as range
	if(ent->client->ps.clientNum == ID)
	{
		trap_SendServerCommand( clientNum, "print \"You cannot drag yourself!\n\"" );
		return;
	}

	memset(arg,0,MAX_TOKEN_CHARS);			//Clear aray.
	//Grab range from command
	trap_Argv( 2, arg, sizeof( arg ) );			//Gets 1st
	range = atof(arg);

	//If target is being dragged.
	if(DragDat[ID].AdminId != -1)
	{
		//This undrag is left here, but can also be done using the undrag command.
		if(DragDat[ID].AdminId == clientNum)
		{
			//Adjust Range if range given
			//QVM HACK!
			if( (Q_stricmp(arg,"")==0) && (range >= 0))
			{
				DragDat[ID].distance = range;
				trap_SendServerCommand( clientNum, va("print \"Adjusted Drag Range of Player %i\n\"", ID) );
			}
			//Else end drag.
			else
			{
				if(!g_entities[ID].client)
					return;
				DragDat[ID].AdminId = -1;
				DragDat[ID].distance = 0;
				g_entities[ID].client->noclip = qfalse;   
			}
		}
		else
		{
			trap_SendServerCommand( clientNum, "print \"Cannot Drag, Someone else is already dragging that player!\n\"" );
		}
		return;
	}

	for(i = 0; i < MAX_CLIENTS; i++)
	{
		if(DragDat[i].AdminId == ID)
		{
			trap_SendServerCommand( clientNum, "print \"Cannot Drag, player is currently dragging.\n\"" );
			return;
		}
	}


	if(!g_entities[ID].client)
		return;
	//Set the struct data for this victim..
	DragDat[ID].AdminId = clientNum;
	DragDat[ID].distance = range;
	//Prevent wierd errors in transit
	g_entities[ID].client->noclip = qtrue;   

	trap_SendServerCommand( clientNum, va("print \"Dragging Client %i\n\"", ID) );

	//Leave the actaual position calculations to the loop function. (See below)
}

/*
=================
UnDrag			(RPG-X: J2J)
=================
*/
// /DragPlayer <ID>
//
// This is used to stop draging
// If the argument 'all' is specified then all drags will be haulted.
// If a client ID is supplied then that play will be droped (by anyone)
// If no parameters are given, the calling player will be undraged.
// 
static void Cmd_UnDrag_f( gentity_t *ent)
{
	char		arg[MAX_TOKEN_CHARS];		//Arguments
	int			ID, i;
	int			clientNum;

	if(!ent || !ent->client)
		return;

	//If client isn't in admin class, exit and disallow command.

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_DRAG) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	clientNum = ent->client->ps.clientNum;

	memset(arg,0,MAX_TOKEN_CHARS);			//Clear aray.
	//Grab range from command
	trap_Argv( 1, arg, sizeof( arg ) );			//Gets 1st

	if(Q_strncmp(arg,"all", 3) == 0)
	{
		for(i = 0; i < MAX_CLIENTS; i++)
		{
			DragDat[i].AdminId = -1;
			DragDat[i].distance = 0;
			if(!g_entities[i].client)
				continue;
			g_entities[i].client->noclip = qfalse;  
		}
		trap_SendServerCommand( clientNum, va("print \"Stopped Dragging all Clients.\n\"") );
		return;
	}

	if(Q_strncmp(arg,"self", 4) == 0)
	{
		DragDat[clientNum].AdminId = -1;
		DragDat[clientNum].distance = 0;
		ent->client->noclip = qfalse;  

		trap_SendServerCommand( clientNum, va("print \"You are UnDragged.\n\"") );
		return;
	}

	if(arg == NULL)
	{
		for(i = 0; i < MAX_CLIENTS; i++)
		{
			if( DragDat[i].AdminId == clientNum )
			{
				DragDat[i].AdminId = -1;
				DragDat[i].distance = 0;
				if(!g_entities[i].client)
					continue;
				g_entities[i].client->noclip = qfalse;  
			}
		}
		trap_SendServerCommand( clientNum, va("print \"Stopped Dragging your Clients\n\"") );
		return;
	}
	else
	{
		ID = atoi(arg);

		if ( ID < 0 || ID >= level.maxclients )
		{
			return;
		}
	}

	if(DragDat[ID].AdminId == -1)
	{
		return;
	}

	if(!g_entities[ID].client)
		return;

	//Set the struct data for this victim..
	DragDat[ID].AdminId = -1;
	DragDat[ID].distance = 0;
	g_entities[ID].client->noclip = qfalse;   
	trap_SendServerCommand( clientNum, va("print \"Stopped Dragging Client %i\n\"", ID) );
}


/*
=================
DragCheck			(RPG-X: J2J)
=================
*/
//This is used internally and run every frame to check for clients that need to be draged by someone.
void DragCheck(void)
{
	gentity_t	*ent = NULL;
	gentity_t	*target = NULL;
	int i;
	playerState_t *ePs;
	playerState_t *tPs;

	for ( i = 0; i < MAX_CLIENTS; i++ )
	{
		//If the admin id is < 0 then they are not being draged.
		if(DragDat[i].AdminId < 0)
			continue;

		//If the starting admin is no longer in admin class:
		if( IsAdmin(&g_entities[DragDat[i].AdminId]) == qfalse)
		{
			//Mark as non-dragging.
			DragDat[i].AdminId = -1;
			if(target && target->client)
				target->client->noclip = qfalse; 
			continue;								                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
		}

		//Get victim and admin entities
		target = &g_entities[i];
		ent = &g_entities[DragDat[i].AdminId];

		//Error checking

		if(!target || ! target->client)
			continue;

		if(!ent || !ent->client)
			continue;

		//If the target has gone to spec, stop dragging.
		if( target->client->sess.sessionTeam == TEAM_SPECTATOR )
		{
			//Mark as non-dragging.
			DragDat[i].AdminId = -1;
			target->client->noclip = qfalse; 
			continue;		
		}

		ePs = &ent->client->ps;
		tPs = &target->client->ps;

		//Vector math!
		VectorCopy(ePs->origin, tPs->origin);
		tPs->origin[0] +=  ( DragDat[i].distance * cos(DEG2RAD(ePs->viewangles[1])));//(ent->client->ps.viewangles[1] * 0.017453292222222222222222222222222 ) );
		tPs->origin[1] +=  ( DragDat[i].distance * sin(DEG2RAD(ePs->viewangles[1])));//(ent->client->ps.viewangles[1] * 0.017453292222222222222222222222222) );
		tPs->origin[2] +=  ( DragDat[i].distance * -tan(DEG2RAD(ePs->viewangles[0]))) + ePs->viewheight;
	}
}


/*
=================
Disarm Tripmines			(RPG-X: RedTechie)
=================
*/
// Harry -- What's the command...
static void Cmd_disarm_f( gentity_t *ent)
{
	// /disarm_tripmines <0 or 1 (yours or all)>
	gentity_t	*tripwire = NULL;
	int			foundTripWires[MAX_GENTITIES] = {ENTITYNUM_NONE};
	int			tripcount = 0;
	int			i;
	char		arg[MAX_TOKEN_CHARS];

	//If client isn't in admin class, exit and disallow command.

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_DISARM) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_Argv( 1, arg, sizeof( arg ) );

	if(arg == NULL){ //J2J
		//Just mine
		while ( (tripwire = G_Find( tripwire, FOFS(classname), "tripwire" )) != NULL )
		{
			if ( tripwire->parent != ent )
			{
				continue;
			}
			foundTripWires[tripcount++] = tripwire->s.number;
		}
	}else if(Q_strncmp(arg, "all",3)){ //J2J
		//All Mines
		while ( (tripwire = G_Find( tripwire, FOFS(classname), "tripwire" )) != NULL )
		{
			foundTripWires[tripcount++] = tripwire->s.number;
		}
	}
	else //J2J
	{
		return;
	}

	if(tripcount != 0){
		for ( i = 0; i < tripcount; i++ )
		{
			//remove it... or blow it?
			if ( &g_entities[foundTripWires[i]] == NULL )
			{
				return;
			}else{
				G_FreeEntity( &g_entities[foundTripWires[i]] );
				foundTripWires[i] = ENTITYNUM_NONE;
			}
		}	
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"Disarmed %i tripmines\n\"", tripcount ) );
	}else{
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"No tripmines to disarm\n\"" ) );
	}
}


static void Cmd_Rank_f( gentity_t *ent)
{
	//	const char *info;
	int OldScore;	
	char ArgStr[50]; //Argument String
	int i;
	int newScore = -1;

	char tmpScore[MAX_QPATH]; // TiM
	qboolean	MaxRankHit=qfalse;

	//Still not sure how the hell this condition would ever get tripped O_o			
	if(!ent || !ent->client)
		return;

	if ( ent->flags & FL_CLAMPED )
		return;

	//TiM | Okay. Ranks are enabled, but only admins can change them
	if ( !rpg_changeRanks.integer )
	{
		trap_SendServerCommand( ent-g_entities, "print \"You cannot change your rank yourself on this server.\n\"");
		return;
	}

	//Lets get old score first just incase
	OldScore = ent->client->ps.persistant[PERS_SCORE];

	trap_Argv(1, ArgStr, sizeof(ArgStr));					//Get the raw arguments

	if ( !ArgStr[0] ) { //If no arguments (ie player wants to see current rank)

		Com_sprintf(tmpScore, sizeof(tmpScore), "%s - %s", g_rankNames[OldScore].formalName, g_rankNames[OldScore].consoleName ); //Put OldScore in a string variable

		trap_SendServerCommand( ent-g_entities, va("print \"\nCurrent Rank: %s\nUsage: Changes user to the specified rank\nCommand: Rank <New rank>\n\nType '/ranklist' into the console for a full list of ranks\" ", tmpScore ) );
		return;
	}

	//TiM: Lemme butt my coding hackiness in here. :P
	//I just realised, that regardless of rank they currently are (even if they're the same), 
	//this code always switches rank, which is causing heck in some other bits of the code. :P

	//RPG-X: TiM - Check if they're already that rank, and return if they are. :P
	//RPG-X: TiM (1.5 years later) - Holy crap man! That really is hacky!!!! >.<

	//RPG-X: RedTechie - Lets enable score updating without this scores will not be updated
	ent->client->UpdateScore = qtrue;

	for ( i=0; (g_rankNames[i].consoleName[0] && i < MAX_RANKS); i++ ) {
		if ( !Q_stricmp( ArgStr, g_rankNames[i].consoleName ) ) {
			newScore = i;

			if ( newScore == OldScore )
				return;

			if( !MaxRankHit )
			{
				SetScore( ent,  newScore );
				trap_SendServerCommand(ent-g_entities, va( "prank %s", g_rankNames[i].consoleName ) );
				break;
			}
			else
			{
				if ( !MaxRankHit )
					trap_SendServerCommand( ent-g_entities, va("print \"This rank is disabled\n\""));
				else
					trap_SendServerCommand( ent-g_entities, va("print \"You cannot set your rank that high on this server.\n\""));

				return;
			}
		}

		//Okay... we've hit the highest rank we're allowed to go.  If the player tries to change their rank to above this, they'll be pwned lol
		if ( rpg_maxRank.string[0] && !Q_stricmp( g_rankNames[i].consoleName, rpg_maxRank.string ) && IsAdmin(ent) ) {
			MaxRankHit=qtrue;
		}
	}

	//if we didn't get find a matching name. >.<
	if ( newScore < 0 ) {
		trap_SendServerCommand( ent-g_entities, va("print \"This rank doesn't exist on this server!\n\"\n"));
		SetScore( ent, OldScore);
		return;
	}

	if ( OldScore > ent->client->ps.persistant[PERS_SCORE] ) {
		trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " was demoted to %s\n\"", ent->client->pers.netname, g_rankNames[i].formalName ) );
	}
	else {
		trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " was promoted to %s\n\"", ent->client->pers.netname, g_rankNames[i].formalName ) );
	}
}



/*
=================
Force Rank			(RPG-X: J2J & RedTechie)
=================
*/
static void Cmd_ForceRank_f( gentity_t *ent)
{
	int OldScore;	
	char ArgStr[MAX_TOKEN_CHARS];							//Argument String
	int targetNum;
	gentity_t *other;
	char send[100];
	int i;
	gentity_t *sayA;
	int newScore = -1;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_FORCEPARM) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	if(!ent || !ent->client)
		return;

	// find the player
	trap_Argv( 1, ArgStr, sizeof( ArgStr ) );

	if ( !ArgStr[0] ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( ent-g_entities, va("print \"\nUsage: User forces another player into a specific rank\nCommand: ForceRank [Player ID] <Rank Name>\n\nType 'rankList' into the console for a full list of ranks\" ") );
		return;
	}

	targetNum = atoi( ArgStr );
	if ( targetNum < 0 || targetNum >= level.maxclients ) {
		return;
	}

	other = &g_entities[targetNum];

	//Lets get old score first just incase
	OldScore = other->client->ps.persistant[PERS_SCORE]; //ent

	if ( !other || !other->inuse || !other->client ) {
		return;
	}

	//Get the raw rank value
	trap_Argv(2, ArgStr, sizeof(ArgStr));

	other->client->UpdateScore = qtrue;

	for ( i=0; (g_rankNames[i].consoleName && i < MAX_RANKS); i++ ) {
		if ( !Q_stricmp( ArgStr, g_rankNames[i].consoleName ) ) {
			newScore = i;//1 << i;

			SetScore( other,  newScore );
			trap_SendServerCommand(other-g_entities, va( "prank %s", g_rankNames[i].consoleName ) );
			break;
		}
	}

	//if we didn't get find a matching name. >.<
	if ( newScore < 0 ) {
		trap_SendServerCommand( ent-g_entities, va("print \"This rank doesn't exist on this server!\n\"\n"));
		SetScore( other, OldScore);
		return;
	}

	if ( OldScore > ent->client->ps.persistant[PERS_SCORE] ) {
		trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " was demoted to %s\n\"", other->client->pers.netname, g_rankNames[i].formalName ) );
	}
	else {
		trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " was promoted to %s\n\"", other->client->pers.netname, g_rankNames[i].formalName ) );
	}

	G_LogPrintf( "%s changed %s's rank to %s (%s)\n", ent->client->pers.netname, other->client->pers.netname, ArgStr, ent->client->pers.ip );

	for (i = 0; i < MAX_CLIENTS - 1; i++) {
		if(g_entities[i].client){
			sayA = &g_entities[i];
			Com_sprintf (send, sizeof(send), "%s changed %s's rank to %s\n", ent->client->pers.netname, other->client->pers.netname, ArgStr);
			G_SayTo( ent, sayA, SAY_ADMIN, COLOR_CYAN, "^7Server: ", send );
		}
	}
}

/*
=================
Admins
RPG-X | Phenix | 21/11/2004
=================
*/

static void Cmd_Admins_f( gentity_t *ent)
{
	char		send[MAX_TOKEN_CHARS];
	int			j;
	gentity_t	*target;

	Q_strncpyz( send, "The following players are logged in as admins: \n", sizeof(send));
	for (j = 0; j < level.maxclients; j++) {
		target = &g_entities[j];
		if (g_classData[target->client->sess.sessionClass].isAdmin)
			Q_strncpyz( send, va("%s %s \n", send, target->client->pers.netname), sizeof(send));

		if ((target->client->LoggedAsAdmin == qtrue) && (IsAdmin( ent ) == qtrue)) 
			Q_strncpyz( send, va("%s %s (hidden) \n", send, target->client->pers.netname), sizeof(send));
	}

	trap_SendServerCommand( ent->client->ps.clientNum, va("print \"%s\n\"", send ) );
}

/*
=================
AdminLogin
=================
*/
static void Cmd_AdminLogin_f( gentity_t *ent)
{
	char		arg[MAX_TOKEN_CHARS];
	char		send[MAX_TOKEN_CHARS];
	char		classPass[MAX_TOKEN_CHARS];
	int			j;
	gentity_t	*sayA;

	trap_Argv( 1, arg, sizeof( arg ) );

	//logout if no argument and player is admin
	if(!arg[0] && ent->client->LoggedAsAdmin) {
		ent->client->LoggedAsAdmin = qfalse;
		trap_SendServerCommand( ent-g_entities, va("print \"You are now logged out.\n\"") );
		G_Client_UserinfoChanged( ent-g_entities );
		return;
	}
	else if ( !arg[0] ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( ent-g_entities, va("print \"\nUsage: Allows a player to login as an admin\nCommand: AdminLogin <Admin Password>\nWARNING: Entering an incorrect password 3 times will automatically kick you from the server!\nEntering AdminLogin without password will log you out if you are locked in\n\" ") );
		return;
	}

	//Admin class login thingy
	for ( j=0; g_classData[j].consoleName[0] && j < MAX_CLASSES; j++ ) 
	{
		trap_Cvar_VariableStringBuffer( va( "rpg_%sPass", g_classData[j].consoleName ), classPass, sizeof(classPass) );

		if ( g_classData[j].isAdmin && !Q_stricmp( classPass, arg ) )
		{
			if ( IsAdmin( ent ) == qfalse ) {
				ent->client->LoggedAsAdmin = qtrue;
				trap_SendServerCommand( ent-g_entities, va("print \"You are logged in as an admin.\n\"") );
				G_Client_UserinfoChanged( ent-g_entities );
				return;
			} else {
				trap_SendServerCommand( ent-g_entities, va("print \"You are already logged in as an admin or in the admin class.\n\"") );			
				return;
			}
			break;
		}
	}

	ent->client->AdminFailed++;
	if ( ent->client->AdminFailed == 3 )
	{
		//Tell admins why he was kicked
		Com_sprintf (send, sizeof(send), "%s tried to login as an admin, failed 3 times and so was kicked.\n", ent->client->pers.netname);
		for (j = level.maxclients - 1; j > -1; j--) { // changed j++ for into j--
			sayA = &g_entities[j];
			G_SayTo( ent, sayA, SAY_ADMIN, COLOR_CYAN, "^7Server: ", send );
		}

		//Kick Client because client has entered the wrong password 3 times
		trap_DropClient( ent->s.number, "Kicked: Too many bad passwords!" );
	} else {
		//Give the client another warning
		trap_SendServerCommand( ent-g_entities, va("print \"You have entered an incorrect password, if you enter a wrong password %i more times you will be kicked.\n\"", (3 - ent->client->AdminFailed) ) );

		//Send warning to other admins
		Com_sprintf (send, sizeof(send), "%s tried to login as an admin and got an incorrect password, %i tries left.\n", ent->client->pers.netname, (3 - ent->client->AdminFailed));
		for (j = level.maxclients - 1; j > -1; j--) { // changed j++ for into j--
			sayA = &g_entities[j];
			G_SayTo( ent, sayA, SAY_ADMIN, COLOR_CYAN, "^7Server: ", send );
		}
	}
}

/*
=================
Revive			(RPG-X: RedTechie)
=================
*/
static void Cmd_Revive_f( gentity_t *ent)
{
	char pla_str[50];	//Argument String
	int targetNum;
	gentity_t *other;
	int i;
	playerState_t *ps;
	clientPersistant_t *pers;

#ifndef SQL
	if ( !IsAdmin( ent ) || !rpg_medicsrevive.integer ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_REVIVE) || !rpg_medicsrevive.integer ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	if(!ent || !ent->client)
		return;

	//If for some strange reason medic revive is turned off
	if(rpg_medicsrevive.integer <= 0)
	{
		trap_SendServerCommand( ent-g_entities, va("print \"Medics' Revive is disabled on this server!\n\""));
		return;
	}

	// find the player
	trap_Argv( 1, pla_str, sizeof( pla_str ) );

	//If player entered no args AND is alive (So as to not screw up reviving yourself ;) )
	if ( !pla_str[0] && (ent && ent->client) && !(ent->health <= 1) && !(ent->client->ps.pm_type == PM_DEAD) ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( ent-g_entities, va("print \"\nUsage: User revives incapacitated players\nCommand: Revive {Nothing = Revive Self} OR [Player ID] OR 'all'\n\" ") );
		return;
	}

	pers = &ent->client->pers;

	if(!Q_strncmp(pla_str, "all", 3))
	{
		//Loop threw all clients
		int j;
		gentity_t *sayA;
		char send[100];

		for(i = MAX_CLIENTS - 1; i > -1; i--) //eherm MAX_CLIENTS is 128 no 1023, changed from i++ to i--
		{
			if( (g_entities[i].client) && (g_entities[i].health == 1) && (g_entities[i].client->ps.pm_type == PM_DEAD))
			{
				G_Client_Spawn(&g_entities[i], 1, qtrue);

				ps = &g_entities[i].client->ps;

				//TiM: Push any users out of the way, and then play a 'getup' emote
				g_entities[i].r.contents = CONTENTS_NONE;
				ps->stats[LEGSANIM] = ( ( ps->stats[LEGSANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | BOTH_GET_UP1;
				ps->stats[TORSOANIM] = ( ( ps->stats[TORSOANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | BOTH_GET_UP1;
				ps->stats[TORSOTIMER] = 1700;
				ps->stats[LEGSTIMER] = 1700;
				ps->stats[EMOTES] = EMOTE_BOTH | EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH;
				ps->legsAnim = 0;
				ps->torsoAnim = 0;
				ps->legsTimer = 0;
				ps->torsoTimer = 0;
			}
		}

		G_LogPrintf( "%s revived everyone (%s)\n", pers->netname, pers->ip);

		Com_sprintf (send, sizeof(send), "%s revived everyone\n", pers->netname);

		for (j = MAX_CLIENTS - 1; j > -1; j--) { // again j++ to j-- and 1023 to MAX_CLIENTS
			if(g_entities[j].client){
				sayA = &g_entities[j];
				G_SayTo( ent, sayA, SAY_ADMIN, COLOR_CYAN, "^7Server: ", send );
			}
		}
	}
	else
	{
		if (!pla_str[0])
		{
			//Just me
			if( (ent && ent->client) && (ent->health <= 1) && (ent->client->ps.pm_type == PM_DEAD))
			{
				G_Client_Spawn(ent, 1, qtrue);

				ps = &ent->client->ps;

				ent->r.contents = CONTENTS_NONE;
				ps->stats[LEGSANIM] = ( ( ps->stats[LEGSANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | BOTH_GET_UP1;
				ps->stats[TORSOANIM] = ( ( ps->stats[TORSOANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | BOTH_GET_UP1;
				ps->stats[TORSOTIMER] = 1700;
				ps->stats[LEGSTIMER] = 1700;
				ps->stats[EMOTES] = EMOTE_BOTH | EMOTE_OVERRIDE_BOTH | EMOTE_CLAMP_BODY;
				ps->legsAnim = 0;
				ps->torsoAnim = 0;
				ps->legsTimer = 0;
				ps->torsoTimer = 0;
			}
		}
		else
		{
			//Specific user
			int j;
			gentity_t *sayA;
			char send[100];

			targetNum = atoi( pla_str );
			if ( targetNum < 0 || targetNum >= level.maxclients ) {
				return;
			}

			other = &g_entities[targetNum];
			if ( !other || !other->inuse || !other->client ) {
				return;
			}

			if( (other && other->client) && (other->health == 1) && (other->client->ps.pm_type == PM_DEAD))
			{
				G_Client_Spawn(other, 1, qtrue);

				ps = &other->client->ps;

				other->r.contents = CONTENTS_NONE;
				ps->stats[LEGSANIM] = ( ( ps->stats[LEGSANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | BOTH_GET_UP1;
				ps->stats[TORSOANIM] = ( ( ps->stats[TORSOANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | BOTH_GET_UP1;
				ps->stats[TORSOTIMER] = 1700;
				ps->stats[LEGSTIMER] = 1700;
				ps->stats[EMOTES] |= EMOTE_BOTH | EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH;
				ps->legsAnim = 0;
				ps->torsoAnim = 0;
				ps->legsTimer = 0;
				ps->torsoTimer = 0;

				G_LogPrintf( "%s revived %s (%s)\n", pers->netname, other->client->pers.netname, pers->ip);

				Com_sprintf (send, sizeof(send), "%s revived %s\n", pers->netname, other->client->pers.netname);

				for (j = MAX_CLIENTS - 1; j > -1; j--) // j++ to j-- and 1023 to MAX_CLIENTS
				{
					if(g_entities[j].client)
					{
						sayA = &g_entities[j];
						G_SayTo( ent, sayA, SAY_ADMIN, COLOR_CYAN, "^7Server: ", send );
					}
				}
			}
		}
	}
}

/*
=================
N00b			(RPG-X: Phenix)
=================
*/
static void Cmd_n00b_f( gentity_t *ent)
{
	char arg[MAX_TOKEN_CHARS];
	int targetNum;
	int timeToBe;
	gentity_t *target;
	int i;

	//If client isn'tin admin class, exit and disallow command.

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_NOOB) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	if ( !ent || !ent->client ) {
		return;		// not fully in game yet
	}

	memset(arg,0,MAX_TOKEN_CHARS);
	trap_Argv( 1, arg, sizeof( arg ) );

	//User Friendly
	if ( !arg[0] ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( ent-g_entities, va("print \"\nUsage: User places another player in n00b class for X seconds\nCommand: n00b [Player ID] [Seconds to be in n00b class]\n\" ") );
		return;
	}

	targetNum = atoi(arg);

	memset(arg,0,MAX_TOKEN_CHARS);
	trap_Argv( 2, arg, sizeof( arg ) );


	timeToBe = atoi(arg);

	target = g_entities + targetNum;

	for ( i = 0; g_classData[i].consoleName[0] && i < MAX_CLASSES; i++ ) {
		if ( g_classData[i].isn00b ) {
			char conName[15];
			trap_Cvar_VariableStringBuffer( va( "rpg_%sPass", g_classData[i].consoleName ), conName, 15);

			Q_strncpyz(target->client->origClass, ClassNameForValue( target->client->sess.sessionClass ), sizeof(target->client->origClass));
			target->client->n00bTime = level.time + (1000 * timeToBe);
			SetClass( target, conName, NULL, qfalse );
			break;
		}
	}
}

/*
=================
Admin Message
RPG-X | Phenix | 08/06/2005
=================
*/
static void Cmd_admin_message( gentity_t *ent)
{
	char *arg;

	if ( trap_Argc () < 1 ) {
		return;
	}

	//If client isn't in admin class, exit and disallow command.

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_MESSAGE) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	if ( !ent || !ent->client ) {
		return;		// not fully in game yet
	}

	arg = ConcatArgs( 1 );

	if ( !arg[0] ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( ent-g_entities, va("print \"\nUsage: Admin broadcasts a message to all users on the server that stays for several seconds\nCommand: Msg \"<Message>\"\n\" ") );
		return;
	}

	if ( arg[0] == '\0' )
	{
		return;
	}

	//RPG-X: Marcin: changed to -1
	trap_SendServerCommand( -1, va("print \"%s\n\" ", arg));

	trap_SendServerCommand( -1, va("servermsg \"%s\"", arg));

}

/*
=================
Cmd_ForceModel_f
=================
*/
static void Cmd_ForceModel_f( gentity_t *ent ) {
	gclient_t	*cl;
	gentity_t *other;
	gentity_t *sayA;
	int j;
	char send[100];
	char  str[MAX_TOKEN_CHARS];
	char  *str2;
	char clientCmd[64];
	clientPersistant_t *pers;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_FORCEPARM) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	//get the first arg
	trap_Argv( 1, str, sizeof( str ) );

	if ( !str[0] ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( ent-g_entities, va("print \"\nUsage: User forces another player into a specific character model\n\nCommand: ForceModel [Player ID] <Model Name>/<Model Skin Name>\n\" ") );
		return;
	}

	// find the player
	cl = ClientForString( str );
	if ( !cl ) { 
		return;
	}
	other = g_entities + cl->ps.clientNum;

	//Get the new model
	str2 = ConcatArgs( 2 );

	pers = &ent->client->pers;

	//Print out some chat text
	G_LogPrintf( "%s changed %s's model to %s (%s)\n", pers->netname, other->client->pers.netname, str2, pers->ip );

	Com_sprintf (send, sizeof(send), "%s ^7changed %s^7's model to %s", pers->netname, other->client->pers.netname, str2);

	for (j = MAX_CLIENTS; j > -1; j--) { // j++ to j--, 1024 to MAX_CLIENTS
		if(g_entities[j].client){
			sayA = &g_entities[j];
			G_SayTo( ent, sayA, SAY_ADMIN, COLOR_CYAN, "^7Server: ", send );
		}
	}

	//send a command to the client and the client will do all this automatically
	Com_sprintf( clientCmd, sizeof(clientCmd), "changeClientInfo model %s", str2 );
	trap_SendServerCommand( cl->ps.clientNum, clientCmd );

}

/*
=================
Cmd_PlayMusic_f
=================
*/
static void Cmd_PlayMusic_f( gentity_t *ent )
{
	char songIntro[MAX_TOKEN_CHARS];
	char songLoop[MAX_TOKEN_CHARS];

	//standard checks

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_MUSIC) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	if(!ent || !ent->client)
		return;

	//Load the arguments
	trap_Argv( 1, songIntro, sizeof( songIntro ) );
	trap_Argv( 2, songLoop, sizeof( songLoop ) );

	//Output the isntructions if no arguments
	if ( !songIntro[0] ) {
		trap_SendServerCommand( ent-g_entities, va("print \"\nUsage: Plays music throughout the level\n\nCommand: playMusic [songIntro] <songLoop>\n\" ") );
		return;	
	}

	//Broadcast the command
	trap_SetConfigstring( CS_MUSIC, va("%s %s", songIntro, songLoop ) );

}

/*
=================
Cmd_StopMusic_f
=================
*/
static void Cmd_StopMusic_f( gentity_t *ent )
{
	//standard checks

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_MUSIC) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	if(!ent || !ent->client)
		return;

	trap_SetConfigstring( CS_MUSIC, "" );
}

/*
=================
Cmd_PlaySound_f
=================
*/
static void Cmd_PlaySound_f( gentity_t *ent )
{
	char soundAddress[MAX_TOKEN_CHARS];

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_MUSIC) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	if(!ent || !ent->client)
		return;

	//Load the arguments
	trap_Argv( 1, soundAddress, sizeof( soundAddress ) );

	//Output the isntructions if no arguments
	if ( !soundAddress[0] ) {
		trap_SendServerCommand( ent-g_entities, va("print \"\nUsage: Plays a sound file throughout the level\n\nCommand: playSound [File Address]\n\" ") );
		return;	
	}

	//Broadcast the command
	trap_SendServerCommand( -1, va("playSnd %s\n", soundAddress ) );

}

/*
=================
Cmd_Bolton_f
=================
*/
static void Cmd_Bolton_f ( gentity_t *ent ) 
{
	gclient_t	*client;
	char		*msg;

	client = ent->client;

	ent->flags ^= FL_HOLSTER;
	if (!(ent->flags & FL_HOLSTER))
	{
		msg = "You took your equipment off.\n";		
		client->ps.powerups[PW_BOLTON] = level.time;
	}        
	else
	{
		msg = "You put your equipment on.\n";
		client->ps.powerups[PW_BOLTON] = INT_MAX;
	}

	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}	

/*
=================
Cmd_UseEnt_f
=================
*/
//TiM: If called with an int argument, finds that ent and tries to use it.
//if no argument, it'll 'use' the entity the player's looking at
//Addendum: if player enters targetname, (checked to make sure it's no valid int in the ent list)
//activate that one.
static void Cmd_UseEnt_f ( gentity_t *ent ) {
	char		entArg[MAX_TOKEN_CHARS];
	gentity_t	*targetEnt;
	int			index;
	playerState_t *ps;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_USEENT) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_Argv( 1, entArg, sizeof( entArg ) );

	ps = &ent->client->ps;

	//No arguments - Do a trace 
	if ( !entArg[0] ) {
		vec3_t start, forward, end;
		trace_t tr;

		//calc start
		VectorCopy( ent->r.currentOrigin, start );
		start[2] += ps->viewheight;

		//calc end
		AngleVectors( ps->viewangles, forward, NULL, NULL );
		VectorMA( start, 8192, forward, end );

		//trace-er-roo
		trap_Trace( &tr, start, NULL, NULL, end, ps->clientNum, MASK_SHOT );

		index = tr.entityNum;
	}
	else { //We gotz an arg, so put it in
		index = atoi( entArg );
	}

	//invalid value ... so I guess it could be a targetname
	if ( index < MAX_CLIENTS || index >= ENTITYNUM_WORLD ) {
		targetEnt = G_Find (NULL, FOFS(targetname), entArg );

		//STILL no dice?? Gah... just eff it then lol.
		if ( !targetEnt ) {	
			return;
		}
	}
	else {
		targetEnt = &g_entities[index]; //get the ent
	}

	//find out the ent is valid, and it is useable
	//TiM: Hack. Well... since we can use usables anyway, I'ma gonna disable them here.
	//Doing it this way can screw up maps bigtime. >.<
	//RPG_Chambers may never be the same again.... :S
	if ( ( targetEnt && targetEnt->use) ) {
		if ( !Q_stricmp( targetEnt->classname, "func_usable" ) )
			return;

		targetEnt->use( targetEnt, ent, ent ); //Activate the Ent
	}
	else { //otherwise berrate the user for being n00bish
		trap_SendServerCommand( ent-g_entities, va( " print \"Entity %i cannot be activated in that way.\n\" ", index ) );
	}
}

/*
=================
Cmd_EntList_f
=================
TiM: Displays as many ents it can in the console without pwning the network code in the process
*/
static void Cmd_EntList_f ( gentity_t *ent ) {
	int			i;
	gentity_t	*mapEnt;
	char		entBuffer[128];
	char		mainBuffer[1024]; //16384

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_USEENT) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	//initialise the data holders
	memset( &entBuffer, 0, sizeof( entBuffer ) );
	memset( &mainBuffer, 0, sizeof( mainBuffer ) );

	for (i = 0, mapEnt = g_entities;
		i < level.num_entities;
		i++, mapEnt++) {

			if (  ( !Q_stricmpn( mapEnt->classname, "fx_", 3) ) || ( !Q_stricmp( mapEnt->classname, "func_usable" ) && ent->targetname ) ) {
				if ( mapEnt->use ) {
					memset( &entBuffer, 0, sizeof( entBuffer ) );

					if ( mapEnt->targetname ) {
						Com_sprintf( entBuffer, sizeof( entBuffer ), "ClassName: '%s', TargetName: '%s', ID: %i\n", mapEnt->classname, mapEnt->targetname, i);
					}
					else {
						Com_sprintf( entBuffer, sizeof( entBuffer ), "ClassName: '%s', ID: %i\n", mapEnt->classname, i);
					}

					if ( strlen(mainBuffer) + strlen(entBuffer) > sizeof( mainBuffer ) ) {
						break;
					}
					else {
						Q_strcat( mainBuffer, sizeof( mainBuffer ), entBuffer );
					}
				}
			}
	}

	if ( strlen( mainBuffer ) > 0 ) {
		trap_SendServerCommand( ent-g_entities, va("print \"%s\" ", mainBuffer) );
	}
	else {
		trap_SendServerCommand( ent-g_entities, va("print \"No activatable entities detected.\n\" " ) );
	}
}

/*
=================
Cmd_BeamTo_f
=================
TiM: Allows people to transport themselves to any target_location entities
in the map :)
Syntax:
beamTo: <location index>
beamTo: <clientID> <location index>
*/
// Harry -- Get the command...
static void Cmd_BeamToLoc_f( gentity_t *ent ) {
	char		argStr[MAX_TOKEN_CHARS];
	gentity_t	*locEnt;
	gentity_t	*targEnt;
	gentity_t	*beamTarget;
	int			i;
	int			clientNum = 0, locIndex = 0;
	char		*strLoc=NULL;
	qboolean	all = qfalse;

	//Has to be an admin.. if anyone had it, the brig would become useless.

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_BEAM) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_Argv( 1, argStr, sizeof( argStr ) );
	if ( !argStr[0] ) {
		trap_SendServerCommand( ent-g_entities, va( "print \"Usage: Allows you to beam yourself to any 'target_location' entity that has a compatible beam-in point\nCommand: (For yourself) - beamToLocation <Index or name of target_location ent> \n(For other players) - beamTo <player ID> <Index or name of target_location ent>\nType '/beamList' into the console for a list of the 'target_location' indexes.\n\" " ) );
		return;
	}

	//if more than one arg, assume we're beaming a separate client
	//first arg MUST BE AN INTEGER CHIKUSHOYO OR THE THING BREAKS!!!!!! (Japanese expletive)
	//must be an int coz it's the clientNum, anything else is not valid.
	if ( trap_Argc() > 2 && (unsigned char)argStr[0] >= '0' && (unsigned char)argStr[0] <= '9' ) {
		//beam all?
		if(!Q_stricmp(argStr, "all")) {
			all = qtrue;
		} else {
			//Get Client ID
			clientNum = atoi( argStr );
		}

		if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
			trap_SendServerCommand( ent-g_entities, va("chat \"Invalid Client ID Number.\n\" " ) );
			return;
		}

		beamTarget = &g_entities[clientNum];
		if ( !beamTarget || !beamTarget->client ) {
			trap_SendServerCommand( ent-g_entities, va("chat \"No valid client found.\n\" " ) );
			return;
		}

		//Get beam location index.
		trap_Argv( 2, argStr, sizeof( argStr ) );

		//If arg is a string of chars or an integer
		if ( (unsigned char)argStr[0] < '0' || (unsigned char)argStr[0] > '9' ) {
			strLoc = ConcatArgs( 2 );
		}
		else {
			locIndex = atoi( argStr );
		}
	}
	else { //else 1 arg was specified - the index to beam ourselves.
		//If arg is a string of chars or an integer
		if ( (unsigned char)argStr[0] < '0' || (unsigned char)argStr[0] > '9' ) {
			strLoc = ConcatArgs( 1 );
		}
		else {
			locIndex = atoi( argStr );
		}

		//The target is us!
		beamTarget = ent;
	}

	//if it's an invalid index, then I'm guessing the player specified location by name then
	if ( locIndex < 1 || locIndex >= MAX_LOCATIONS ) {
		if ( !strLoc ) {
			trap_SendServerCommand( ent-g_entities, va("chat \"Invalid Location Index.\n\" " ) );
			return;
		}
	}

	//locEnt = &g_entities[iArg];
	//Scan for the right entity
	for ( i=0, locEnt=g_entities; i < level.num_entities; locEnt++, i++ ) 
	{

		if ( !Q_stricmp( locEnt->classname, "target_location" ) ) {
			//if we have a health index (which will always be above 0 coz 0 is a default 'unknown' value)
			if ( locEnt->health == locIndex && locEnt->health >= 1 && locIndex >= 1 ) {
				break;
			}

			//Failing that, compare string values. If all went well you cannot
			//have a string value or an int value valid at the same time in these checks.
			if ( !locIndex && !Q_stricmp( locEnt->message, strLoc ) ) {
				break;
			}
		}
	}

	if ( !locEnt || Q_stricmp( locEnt->classname, "target_location" ) ) {
		trap_SendServerCommand( ent-g_entities, va("chat \"Invalid Beam Entity.\n\" " ) );
		return;
	}

	//locEnt
	if ( locEnt->target ) {
		targEnt = G_PickTarget( locEnt->target );
	}
	else {
		targEnt = locEnt;
	}

	if (targEnt /*&& TransDat[beamTarget->client->ps.clientNum].beamTime == 0*/ ) {
		vec3_t	destPoint;
		//TiM : Leeched this code off elsewhere.  instead of hard-coding a value, align it to the ent's bbox.
		//In case they don't use notnulls but something else.
		VectorCopy(targEnt->s.origin, destPoint);
		destPoint[2] += targEnt->r.mins[2]; 
		destPoint[2] -= beamTarget->r.mins[2];
		destPoint[2] += 1;

		if ( TransDat[beamTarget->client->ps.clientNum].beamTime == 0 ) {
			trap_SendServerCommand( ent-g_entities, va( "chat \"Initiating transport to location: %s\n\" ", locEnt->message ) ); 
			if(!all) {
				G_InitTransport( beamTarget->client->ps.clientNum, destPoint, targEnt->s.angles );
			} else {
				for(i = 0; i < MAX_CLIENTS && i < g_maxclients.integer; i++) {
					if(!&g_entities[i] || !&g_entities[i].client) continue;
					G_InitTransport(g_entities[i].client->ps.clientNum, destPoint, g_entities[i].s.angles );
					targEnt = G_PickTarget(locEnt->target);
				}
			}
		}
		else {
			if ( beamTarget->client->ps.clientNum == ent->client->ps.clientNum ) {
				trap_SendServerCommand( ent-g_entities, va( "chat \"Unable to comply. You are already within a transport cycle.\n\" " ) );
			}
			else {
				trap_SendServerCommand( ent-g_entities, va( "chat \"Unable to comply. Subject is already within a transport cycle.\n\" " ) );
			}
		}
	}
	else 
	{
		trap_SendServerCommand( ent-g_entities, va( "chat \"Location entity does not have a valid beam location.\n\" " ) ); 
	}
}

/*==============
Cmd_ForcePlayer_cmd
===============
TiM : Lets admins change any generic values
within a client's userinfo settings.
Things like age, race, height etc.  Paramaters
that are too simple to require their own specific
function.
Although, name and model could be changed here too...
*/
static void Cmd_ForcePlayer_f ( gentity_t *ent ) {
	gclient_t	*cl;
	gentity_t *other;
	gentity_t *sayA;
	int j;
	char send[100];
	char  str[MAX_TOKEN_CHARS];
	char  *str2;
	char userinfo[MAX_INFO_STRING];
	char clientCmd[MAX_INFO_STRING];
	clientPersistant_t *pers;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_FORCEPARM) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	// find the player
	trap_Argv( 1, str, sizeof( str ) );

	if ( !str[0] ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( ent-g_entities, va("print \"\nUsage: User force changes a parameter in another player's configuration settings on the server.\nCommand: ForcePlayer <Player's ID Number> \"[Setting to be changed]\" \"[New Value]\"\n\n\" ") );
		return;
	}

	cl = ClientForString( str );
	if ( !cl ) { 
		return;
	}
	other = g_entities + cl->ps.clientNum;

	//Get the key
	trap_Argv( 2, str, sizeof( str ) );
	if ( !str[0] ) {
		return;
	}

	//get client's data
	trap_GetUserinfo( cl->ps.clientNum, userinfo, sizeof( userinfo ) );
	if ( !strstr( userinfo, str ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: Invalid setting specified.\n\" ") );
		return;
	}

	str2 = ConcatArgs( 3 );
	if ( !str2[0] ) {
		return;
	}

	pers = &ent->client->pers;

	//Print out some chat text
	G_LogPrintf( "%s changed %s's %s setting to %s (%s)\n", pers->netname, other->client->pers.netname, str, str2, pers->ip );

	Com_sprintf (send, sizeof(send), "%s ^7changed %s's %s setting to ^7to %s", pers->netname, other->client->pers.netname, str, str2);

	for (j = 0; j < MAX_CLIENTS; j++) {
		if(g_entities[j].client){
			sayA = &g_entities[j];
			G_SayTo( ent, sayA, SAY_ADMIN, COLOR_CYAN, "^7Server: ", send );
		}
	}

	Com_sprintf( clientCmd, sizeof(clientCmd), "changeClientInfo %s %s", str, str2 );

	trap_SendServerCommand( cl->ps.clientNum, clientCmd );
}

/*
==============
Cmd_BeamToPlayer_f
===============
TiM : Lets players beam to
other player locations.

Marcin : Implemented an 'all' option. (11/12/2008)
*/
#define PLAYER_BEAM_DIST	50

static void Cmd_BeamToPlayer_f( gentity_t	*ent ) {
	char		argStr[MAX_TOKEN_CHARS];
	gentity_t	*target;
	gentity_t	*beamee;
	int			clientNum = 0;
	int			bClientNum = 0;
	vec3_t		mins = { -12, -12, -24 }; //for the volume trace -//12
	vec3_t		maxs = { 12, 12, 56 }; //44
	int			i, j = 0;
	vec3_t		origin, angles, zOrigin;
	trace_t		tr;
	qboolean	validTraceFound = qfalse;
	int			startPoint;
	int			totalCount;
	int			offsetRA[8][2] = {	{  1,  0 }, //offsets for each beam test location
	{  1, -1 },
	{  0, -1 },
	{ -1, -1 },
	{ -1,  0 },
	{ -1,  1 }, 
	{  0,  1 }, 
	{  1,  1 } 
	};
	int			viewAngleHeading[8] = { 180, 135, 90, 45, 0, -45, -90, -135 };
	qboolean    everyone = qfalse;


	//Has to be an admin.. if anyone had it, the brig would become useless.

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_BEAM) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_Argv( 1, argStr, sizeof( argStr ) );
	if ( !argStr[0] ) {
		trap_SendServerCommand( ent-g_entities, va( "print \"Usage: Allows you to beam yourself or another player to the location of a separate player.\nCommand: (For yourself) - beamToPlayer <ID of player to beam to> \n(For other players) - beamToPlayer <ID of player to beam> <ID of location player>\n\" " ) );
		return;
	}

	if ( trap_Argc() == 2 ) {
		clientNum = atoi( argStr );
		bClientNum = ent->client->ps.clientNum;
		everyone = qfalse;
	}
	else if ( trap_Argc() >= 3 ) {
		if ( Q_stricmp( argStr, "all" ) == 0 ) {
			bClientNum = -1;
			everyone = qtrue;
		} else {
			bClientNum = atoi( argStr );
			everyone = qfalse;
		}
		trap_Argv( 2, argStr, sizeof( argStr ) );
		if ( Q_stricmp( argStr, "all" ) == 0 ) {
			trap_SendServerCommand( ent-g_entities, "print \"ERROR. You can not beam a player to everyone.\nPerhaps you meant /beamToPlayer all <number>.\n\" " );
			return;
		}

		clientNum = atoi( argStr );
	}

	if ( clientNum == bClientNum ) {
		trap_SendServerCommand( ent-g_entities, va( "print \"ERROR. Target location player and target beaming player cannot be the same.\n\" " ) );
		return;
	}

	if ( ( clientNum < 0 || clientNum >= MAX_CLIENTS )  || ( ( bClientNum < 0 || bClientNum >= MAX_CLIENTS ) && !everyone ) ) {
		trap_SendServerCommand( ent-g_entities, va( "print \"Invalid client specified.\n\" " ) );
		return;
	}

	do {
		if ( 1 ) { // sorry
			//get our intended beam subject
			if ( !everyone ) {
				beamee = &g_entities[bClientNum];
				j = bClientNum;
			} else {
				if ( g_entities[j].client ) {
					beamee = &g_entities[j];
				} else {
					continue;
				}               
			}

			if ( beamee == NULL ) {
				continue;
			}

			//get our intended target
			target = &g_entities[clientNum];
			if ( target == NULL || !target->client ) {
				continue;
			}

			if ( g_entities[j].client->ps.clientNum == target->client->ps.clientNum ) {
				continue;
			}

			startPoint = irandom( 1, 7 ); //we'll randomize where it checks, so players won't potentially beam to the same spot each time.

			//okay, we're going to test 8 locations around our target.  First one we find that's available, we'll take. 
			for ( i = startPoint, totalCount = 0; totalCount < 8; i++, totalCount++ ) {

				//Reset the counter if it exceeds 7
				if ( i >= 8 ) {
					i = 0;
				}

				//target origin is old origin offsetted in a different direction each loop 
				origin[0] = target->r.currentOrigin[0] + PLAYER_BEAM_DIST * offsetRA[i][0]; //set X offset
				origin[1] = target->r.currentOrigin[1] + PLAYER_BEAM_DIST * offsetRA[i][1]; //set Y offset
				origin[2] = target->r.currentOrigin[2];

				//do a volume trace from our old org to new org
				//This vol trace is set to standard EF bounding box size, so if ANY geometry gets inside, it returns false;
				trap_Trace( &tr, target->r.currentOrigin, mins, maxs, origin, target->client->ps.clientNum, MASK_ALL ); //CONTENTS_SOLID

				//We didn't hit anything solid
				if ( tr.fraction == 1.0 && !tr.allsolid ) {
					//trace straight down to see if there's some floor immeadiately below us we can use
					VectorCopy( origin, zOrigin );
					zOrigin[2] -= 32;

					trap_Trace( &tr, origin, NULL, NULL, zOrigin, target->client->ps.clientNum, CONTENTS_SOLID );

					//ew... seems to be a chasm or something below us... don't wanna beam there
					if ( tr.fraction == 1.0 ) {
						continue;
					}
					else {
						validTraceFound = qtrue;
						break;
					}
				}
			}

			if ( !validTraceFound ) {
				trap_SendServerCommand( ent-g_entities, va( "print \"No valid beam points next to player found.\n\" " ) );
				continue;
			}

			VectorCopy( ent->client->ps.viewangles, angles );
			angles[YAW] = (float)viewAngleHeading[i];

			//Com_Printf( "OldAngle = %f, NewAngle = %f, index = %i\n", ent->client->ps.viewangles[YAW], angles[YAW], i );

			if ( TransDat[ beamee->client->ps.clientNum ].beamTime == 0 ) {
				if ( j == ent->client->ps.clientNum ) {
					trap_SendServerCommand( ent-g_entities, va( "chat \"Initiating transport to player %s^7's co-ordinates.\n\" ", target->client->pers.netname ) );
				}
				else {
					trap_SendServerCommand( ent-g_entities, va( "chat \"Transporting %s^7 to player %s^7's co-ordinates.\n\" ", beamee->client->pers.netname, target->client->pers.netname ) );
				}
				//commence beaming
				G_InitTransport( beamee->client->ps.clientNum, origin, angles );
			}
			else if ( !everyone ) {
				if ( j == ent->client->ps.clientNum ) {
					trap_SendServerCommand( ent-g_entities, va( "chat \"Unable to comply.  You are already within a transport cycle.\n\" " ) );

				}
				else {
					trap_SendServerCommand( ent-g_entities, va( "chat \"Unable to comply.  Subject is already within a transport cycle.\n\" " ) );
				}
			}
		}
	} while ( ( (++j) < g_maxclients.integer ) && everyone );

}

/*==============
Cmd_DoEmote_f
===============
TiM : T3h emote-zor code
Phase two.  The emote index and
time length has been received from
client.  Any specific flags the
emote may have will be checked here. 
*/
static void Cmd_DoEmote_f ( gentity_t	*ent ) {
	char			argStr[MAX_QPATH];
	emoteList_t		*emote;
	int				animLength;
	int				i;
	int				emoteInt;
	int				flagHolder=0;
	qboolean		doUpper = qfalse;
	qboolean		doLower = qfalse;
	qboolean		alreadyEmoting;
	playerState_t	*ps;

	trap_Argv( 1, argStr, sizeof( argStr ) );

	//RPG-X | Marcin | 24/12/2008
	//n00b check
	if ( g_classData[ent->client->sess.sessionClass].isn00b ) {
		trap_SendServerCommand( ent-g_entities, "print \"[You're too stupid to use this command]\n\"");
		return;
	}

	ps = &ent->client->ps;

	//Small override for the specific eyes animation emotes.
	//And also the alert mode now
	if ( !Q_stricmp( argStr, "eyes_shut" ) || !Q_stricmp( argStr, "eyes_frown" ) || !Q_stricmpn( argStr, "alert2", 6 ) || !Q_stricmpn( argStr, "alert", 5 ) ) {
		if ( !Q_stricmp( argStr, "eyes_shut" ) ) {
			ps->stats[EMOTES] ^= EMOTE_EYES_SHUT;
		}

		else if ( !Q_stricmp( argStr, "eyes_frown" ) )
			ps->stats[EMOTES] ^= EMOTE_EYES_PISSED;
		else if ( !Q_stricmpn( argStr, "alert2", 6 ) )
		{
			ps->pm_flags &= ~ANIM_ALERT;
			ps->pm_flags ^= ANIM_ALERT2;
		}
		else if ( !Q_stricmpn( argStr, "alert", 5 ) )
		{
			ps->pm_flags &= ~ANIM_ALERT2;
			ps->pm_flags ^= ANIM_ALERT;
		}

		return;
	}

	if ( trap_Argc() != 3 ) {
		trap_SendServerCommand( ent-g_entities, va( "print \"Invalid arguments listed.  Please use the '/emote' command to perform emotes.\n\" " ) );
		return;
	}

	emoteInt = atoi( argStr );
	emote = &bg_emoteList[ emoteInt ];

	if ( !emote ) {
		trap_SendServerCommand( ent-g_entities, va( "print \"Invalid emote specified.\n\" " ) );
		return;
	}

	trap_Argv( 2, argStr,  sizeof( argStr ) );
	animLength = atoi( argStr );

	if ( animLength > 65000 ) {
		trap_SendServerCommand( ent-g_entities, va( "print \"Invalid emote time length specified.\n\" " ) );
		return;
	}

	//Before we flush the emotes, perform a check to see if we're loopin any animations
	//If we are, set this one to revert
	ps->torsoAnim = 0;
	ps->legsAnim = 0;

	if ( emote->bodyFlags & EMOTE_UPPER ) {
		doUpper = qtrue;
	}

	if ( emote->bodyFlags & EMOTE_LOWER ) {
		doLower = qtrue;
	}

	if ( ( ps->stats[EMOTES] & EMOTE_UPPER ) && 
		( ps->stats[EMOTES] & EMOTE_LOOP_UPPER ) ) 
	{
		if ( emote->animFlags & EMOTE_REVERTLOOP_UPPER ) {
			int anim = ps->stats[TORSOANIM];
			anim &= ~ANIM_TOGGLEBIT; //remove the toggle msk

			for (i=0;i<bg_numEmotes;i++ ) {
				if ( bg_emoteList[i].enumName == anim ) {
					ps->torsoAnim = i;
					break;
				}
			}
		}
	}

	if ( ( ps->stats[EMOTES] & EMOTE_LOWER ) && 
		( ps->stats[EMOTES] & EMOTE_LOOP_LOWER ) ) 
	{
		if ( emote->animFlags & EMOTE_REVERTLOOP_LOWER ) {
			int anim = ps->stats[LEGSANIM];
			anim &= ~ANIM_TOGGLEBIT;

			for (i=0;i<bg_numEmotes;i++ ) {
				if ( bg_emoteList[i].enumName == anim ) {
					ps->legsAnim = i;
					break;
				}
			}
		}
	}

	alreadyEmoting = (qboolean)( ( ps->stats[EMOTES] & EMOTE_UPPER ) || ( ps->stats[EMOTES] & EMOTE_LOWER ) );

	if ( alreadyEmoting ) {
		doUpper = (qboolean)( ( emote->animFlags & EMOTE_OVERRIDE_UPPER ) );
		doLower = (qboolean)( ( emote->animFlags & EMOTE_OVERRIDE_LOWER ) );
	}
	else {
		if ( doLower && !( emote->animFlags & EMOTE_OVERRIDE_LOWER ) && ps->powerups[PW_FLIGHT] ) {
			doLower = qfalse;
		}
	}

	//flush out any previous emotes
	if ( doUpper ) {
		ps->stats[EMOTES] &= ~EMOTE_MASK_UPPER;	//Remove all emotes but eye flags

		flagHolder = ( emote->animFlags | emote->bodyFlags );
		flagHolder &= ~(EMOTE_MASK_LOWER | EMOTE_EYES_SHUT | EMOTE_EYES_PISSED);

		ps->stats[EMOTES] |= flagHolder;
	}

	if ( doLower ) {
		ps->stats[EMOTES] &= ~EMOTE_MASK_LOWER;	//Remove all emotes but eye flags

		flagHolder = ( emote->animFlags | emote->bodyFlags );
		flagHolder &= ~(EMOTE_MASK_UPPER | EMOTE_EYES_SHUT | EMOTE_EYES_PISSED);

		ps->stats[EMOTES] |= flagHolder;		
	}

	//The clamp flag is set here, but is then moved to cState->eFlags on the CG 
	//during the next snapshot. A tad more resource savvy, but for some weird reason,
	//it wasn't passing directly from here... might be a scope issue.
	//Huh... turns out the Ravensoft d00ds were having the same trouble too.
	//My guess it's an engine problem when the vars from pState are converted to eState
	//and then sent over the network.  It all seems cool game-side, but screwy CG side O_o

	//TiM : Since we're no longer using legsTimer and torsoTimer,
	//These are great to use for additional args

	//======================================

	if ( doUpper ) {
		if ( animLength > 0 ) {

			//check to see if we don't already have a set loop anim
			if ( ps->torsoAnim <= 0 && emote->enumLoop >= 0) {
				for (i=0; i<bg_numEmotes; i++ ) {
					if ( bg_emoteList[i].enumName == emote->enumLoop ) {
						ps->torsoAnim = i;
						break;
					}
				}
			}

			ps->stats[TORSOTIMER] = animLength; 
		}															 
		else {
			ps->stats[TORSOTIMER] = 0;						//Infinite animations (ie sitting/typing )
		}

		ps->torsoTimer = emoteInt;

		ps->stats[TORSOANIM] =
			( ( ps->stats[TORSOANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | emote->enumName;
	}

	//=====================================

	if ( doLower ) {

		//if we had a pre-defined anim length
		if ( animLength > 0 ) {
			ps->stats[LEGSTIMER] = animLength;

			//check to see if we don't already have a set loop anim
			if ( ps->legsAnim == 0 && emote->enumLoop >= 0) {
				for (i=0; i<bg_numEmotes; i++ ) {
					if ( bg_emoteList[i].enumName == emote->enumLoop ) {
						ps->legsAnim = i;
						break;
					}
				}
			}
		}
		else {
			ps->stats[LEGSTIMER] = 0;
		}

		//set emote num into emote timer so we can use it in pmove
		ps->legsTimer = emoteInt;

		//ent->client->ps.legsAnim = 
		//( ( ent->client->ps.legsAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;
		ps->stats[LEGSANIM] =
			( ( ps->stats[LEGSANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | emote->enumName;
	}
}

/*==============
Cmd_EndEmote_f
Ends all active emotes.
I originally had it so holding the
walk button did this, but then found out
that it's permanently held down in other instances :S
================*/
static void Cmd_EndEmote_f( gentity_t *ent ) {
	playerState_t *ps;

	if( !ent->client )
		return;

	ps = &ent->client->ps;

	ps->stats[EMOTES] &= ~EMOTE_MASK_BOTH;

	ps->legsTimer = 0;
	ps->legsAnim = 0;
	ps->stats[LEGSTIMER] = 0;


	ps->torsoTimer = 0;
	ps->torsoAnim = 0;
	ps->stats[TORSOTIMER] = 0;
}

/*==============
Cmd_Laser_f
Activates the Laser the same way 
as cloaking or flying.  The previous
code was bad in the fact that it used
up entity slots, had to constantly
call think funcs to update, and then
transmit the origins over the network, 
resulting in sluggish movement for high
ping d00ds.
===============*/
static void Cmd_Laser_f ( gentity_t *ent ) {
	char* message;
	playerState_t *ps = &ent->client->ps;

	//TiM - Say... what was the max length of time a Q3 server was capable of running
	//nonstop again lol? :)
	if ( !ps->powerups[PW_LASER] ) {
		ps->powerups[PW_LASER] = level.time + 10000000;
		message = "Activated Laser";
	}
	else {
		ps->powerups[PW_LASER] = level.time;
		message = "Deactivated Laser";
	}

	trap_SendServerCommand( ent-g_entities, va( " print \"%s\n\" ", message ) );
}

/*==============
Cmd_FlashLight_f
==============*/
static void Cmd_FlashLight_f ( gentity_t *ent ) {
	char* message;
	playerState_t *ps = &ent->client->ps;

	//TiM - Say... what was the max length of time a Q3 server was capable of running
	//nonstop again lol? :)
	if ( !ps->powerups[PW_FLASHLIGHT] ) {
		ps->powerups[PW_FLASHLIGHT] = level.time + 10000000;
		message = "Activated Flashlight";
	}
	else {
		ps->powerups[PW_FLASHLIGHT] = level.time;
		message = "Deactivated Flashlight";
	}

	trap_SendServerCommand( ent-g_entities, va( " print \"%s\n\" ", message ) );
}

/*==============
Cmd_fxGun_f
TiM: Allows admins
to configure their
FX guns to emit different FX
==============*/
static void Cmd_fxGun_f ( gentity_t *ent ) {
	char	arg[MAX_TOKEN_CHARS];
	char	fxName[36];
	const int FX_DEFAULT_TIME = 900000;
	fxGunData_t *fxGunData;

	if ( !ent->client ) 
		return;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_FX) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_Argv( 1, arg, sizeof( arg ) );

	//save the name for the end
	Q_strncpyz( fxName, arg, sizeof( fxName ) );

	fxGunData = &ent->client->fxGunData;

	if ( !Q_stricmp ( arg, "default" ) ) {
		memset( fxGunData, 0, sizeof( fxGunData ) );
	}
	else if ( !Q_stricmp ( arg, "detpack" ) ) {
		memset( fxGunData, 0, sizeof( fxGunData ) );
		fxGunData->eventNum = EV_DETPACK;
	}
	else if ( !Q_stricmp( arg, "chunks" ) ) {
		memset( fxGunData, 0, sizeof( fxGunData ) );
		//radius
		trap_Argv( 2, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun chunks <radius> <chunk type: 1-5>\n\" " );
			return;
		}

		fxGunData->arg_int1 = atoi( arg );
		fxGunData->eventNum = EV_FX_CHUNKS;

		//radius
		trap_Argv( 3, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun chunks <radius> <chunk type: 1-5>\n\" " );
			memset( fxGunData, 0, sizeof( fxGunData ) );
			return;
		}

		fxGunData->arg_int2 = atoi( arg );
	}
	else if ( !Q_stricmp ( arg, "sparks" ) ) {
		memset( fxGunData, 0, sizeof( fxGunData ) );
		trap_Argv( 2, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun sparks <spark time interval> <time length of effect> | in milliseconds\n\" " );
			return;
		}

		fxGunData->eventNum = EV_FX_SPARK;
		fxGunData->arg_float1 = atof( arg );

		//optional arg for timelength
		trap_Argv( 3, arg, sizeof( arg ) );
		if ( arg[0] && atoi( arg ) ) {
			fxGunData->arg_int2 = atoi(arg);
		}
		else {
			fxGunData->arg_int2 = FX_DEFAULT_TIME;
		}
	}
	else if ( !Q_stricmp ( arg, "steam" ) ) {
		memset( fxGunData, 0, sizeof( fxGunData ) );
		fxGunData->eventNum = EV_FX_STEAM;

		//optional arg for timelength
		trap_Argv( 2, arg, sizeof( arg ) );
		if ( arg[0] && atoi( arg ) ) {
			fxGunData->arg_int2 = atoi(arg);
		}
		else {
			fxGunData->arg_int2 = FX_DEFAULT_TIME;
		}
	}
	else if ( !Q_stricmp( arg, "drips" ) ) {
		memset( fxGunData, 0, sizeof( fxGunData ) );
		//type of drips
		trap_Argv( 2, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun drips <type of drips: 0,1,2> <drips intensity: 0.0->1.0> <time length of effect: 1000 = 1 second> \n\" " );
			return;
		}

		fxGunData->eventNum = EV_FX_DRIP;
		fxGunData->arg_int1 = atoi( arg );

		//intensity of drips
		trap_Argv( 3, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun drips <type of drips: 0,1,2> <drips intensity: 0.0->1.0> <time length of effect: 1000 = 1 second> \n\" " );
			fxGunData->eventNum = 0;
			fxGunData->arg_int1 = 0;
			return;
		}

		fxGunData->arg_float1 = atof( arg );

		//optional time length
		trap_Argv( 4, arg, sizeof( arg ) );
		if ( arg[0] && atoi( arg ) ) {
			fxGunData->arg_int2 = atoi(arg);
		}
		else {
			fxGunData->arg_int2 = FX_DEFAULT_TIME;
		}		
	}
	else if ( !Q_stricmp( arg, "smoke" ) ) {
		memset( fxGunData, 0, sizeof( fxGunData ) );
		//smoke radius
		trap_Argv( 2, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun smoke <smoke radius size> <time length of effect: 1000 = 1 second> \n\" " );
			return;
		}

		fxGunData->eventNum = EV_FX_SMOKE;
		fxGunData->arg_int1 = atoi( arg );

		//optional time length
		trap_Argv( 3, arg, sizeof( arg ) );
		if ( arg[0] && atoi( arg )) {
			fxGunData->arg_int2 = atoi(arg);
		}
		else {
			fxGunData->arg_int2 = FX_DEFAULT_TIME;
		}		

	}
	else if ( !Q_stricmp( arg, "surf_explosion" ) ) {
		memset( fxGunData, 0, sizeof( fxGunData ) );
		//explosion radius
		trap_Argv( 2, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun surf_explosion <explosion radius> <camera shake intensity> \n\" " );
			return;
		}

		fxGunData->eventNum = EV_FX_SURFACE_EXPLOSION;
		fxGunData->arg_float1 = atof( arg );

		//explosion cam shake
		trap_Argv( 3, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun surf_explosion <explosion radius> <camera shake intensity> \n\" " );

			fxGunData->eventNum = 0;
			fxGunData->arg_float1 = 0;			
			return;
		}

		fxGunData->arg_float2 = atof( arg );		
	}
	else if ( !Q_stricmp( arg, "elec_explosion" ) ) {
		memset( fxGunData, 0, sizeof( fxGunData ) );
		//explosion radius
		trap_Argv( 2, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun elec_explosion <explosion radius> \n\" " );
			return;
		}

		fxGunData->eventNum = EV_FX_ELECTRICAL_EXPLOSION;
		fxGunData->arg_float1 = atof( arg );

	}
	else if ( !Q_stricmp( arg, "fire" ) ) {
		memset( fxGunData, 0, sizeof( fxGunData ) );
		//time
		trap_Argv( 2, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun fire <radius> <time> \n\" " );
			return;
		}

		fxGunData->eventNum = EV_FX_FIRE;
		fxGunData->arg_int1 = atoi( arg );

		//optional time length
		trap_Argv( 3, arg, sizeof( arg ) );
		if ( arg[0] && atoi( arg ) ) {
			fxGunData->arg_int2 = atoi( arg );
		}
		else {
			fxGunData->arg_int2 = FX_DEFAULT_TIME;
		}

	}
	else if ( !Q_stricmp( arg, "shake" ) ) {
		memset( fxGunData, 0, sizeof( fxGunData ) );
		//time
		trap_Argv( 2, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun shake <radius> <intensity> \n\" " );
			return;
		}

		fxGunData->eventNum = EV_FX_SHAKE;
		fxGunData->arg_int1 = atoi( arg );

		//optional intensity
		trap_Argv( 3, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun shake <radius> <intensity> \n\" " );
			return;
		}

		fxGunData->arg_int2 = atoi( arg );

	}
	else {
		trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun <FX_Name>\nValid Effects:\n  default\n  chunks\n  detpack\n  sparks\n  steam\n  drips\n  smoke\n  surf_explosion\n  elec_explosion \n\" " );
		memset( fxGunData, 0, sizeof( fxGunData ) );
		return;
	}

	trap_SendServerCommand( ent-g_entities, va( "print \"Effect successfully reconfigured to %s.\n\" ", fxName ) );
}

/*==============
Cmd_flushFX_f
TiM: Systematically
purges ALL ACTIVE
CLIENTSIDE EFFECTS!
This is a safety feature
if an admin was a little too 
happy with the FX gun
==============*/
static void Cmd_flushFX_f( gentity_t	*ent ) {
	if ( !ent->client ) 
		return;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_FX) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_SendServerCommand( -1, "cg_flushFX" );
}

/*=================
Cmd_spawnChar_f
TiM: Takes ur current data, and 
spawns a player model that looks like you with it.
==================*/
static void Cmd_spawnChar_f( gentity_t	*ent ) {
	if ( !ent->client ) 
		return;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_CHARS) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	if ( !PlaceDecoy(ent) )
	{
		trap_SendServerCommand( ent-g_entities, "cp \"NO ROOM TO PLACE CHARACTER\"" );
	}
	else
	{
		trap_SendServerCommand( ent-g_entities, "cp \"CHARACTER PLACED\"" );
	}
}

/*====================
Cmd_flushChars_f
TiM: Purges all the decoys on the server
====================*/
static void Cmd_fluchChars_f( gentity_t *ent ) //GSIO01: fluch Chars ehhh? you know fluch means curse in german? :D
{
	char	arg[16];
	int		i;
	gentity_t	*locEnt;

	if ( !ent->client ) 
		return;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_CHARS) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_Argv( 1, arg, sizeof(arg) );
	if ( !arg[0] ) {
		trap_SendServerCommand( ent-g_entities,"print \"Usage: Removes spawn characters from the level.\nSyntax: /flushChars <number> | 0 = Just your spawned characters, 1 = All characters\n\" " );
		return;
	}

	for ( i=0, locEnt=g_entities; i < level.num_entities; locEnt++, i++ ) {
		if ( !Q_stricmp( locEnt->classname, "decoy" ) ) {
			if ( atoi( arg ) == 0 && locEnt->parent != ent )
				continue;
			else
				G_FreeEntity( locEnt );
		}
	}
}

/*====================
Cmd_flushDropped_f
RPG-X: Marcin: Removes all dropped weapons - 04/12/2008
code stolen from above
====================*/
static void Cmd_flushDropped_f( gentity_t *ent )
{
	char	arg[16];
	int		i;
	qboolean	ans;
	gentity_t	*locEnt;

	if ( !ent->client ) 
		return;

	trap_Argv( 1, arg, sizeof(arg) );
	if ( !arg[0] ) {
		ans = (qboolean)0;
	} else {
		ans = (qboolean)atoi( arg );
	}

	for ( i=0, locEnt=g_entities; i < level.num_entities; locEnt++, i++ ) {
		if (  locEnt->classname && !Q_strncmp( locEnt->classname, "weapon", 6) ) { // everything that begins with weapon_
			if ( ( ans == qfalse || !IsAdmin( ent ) ) && locEnt->parent != ent )
				continue;
			else
				G_FreeEntity( locEnt );
		}
	}
}
/*
===================
Cmd_Kick2_f

kick2 <player> <reason>

Boots the user off the server while
giving them a reason why they deserved it so

Credit: Scooter, TiM
===================
*/
static void Cmd_Kick2_f( gentity_t *ent ) {
	gclient_t *cl;
	char  str[MAX_TOKEN_CHARS];
	char  *str2;
	char  reason[MAX_TOKEN_CHARS];
	int i;

	//standard checks

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_KICK) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	if(!ent || !ent->client)
		return;

	// find the player
	trap_Argv( 1, str, sizeof( str ) );
	//If player adds no args, write a friendly msg saying how it works
	if ( !str[0] ) {
		trap_SendServerCommand( ent-g_entities, "print \"Usage: Kicks specified user with a reason\n\nCommand: kick [User ID] <Reason>\n\"" );
		return;	
	}

	//if we were told to kick all
	if ( !Q_stricmp( str, "all" ) ) {
		//loop thru everyone
		for (i = 0; i < MAX_CLIENTS; i++ ) {
			//check if they are a client and NOT US LOL
			if ( g_entities[i].client &&
				g_entities[i].client->ps.clientNum != ent->client->ps.clientNum ) {
					//if there was only one arg, just kick em
					if ( trap_Argc() <= 2 ) {
						trap_SendConsoleCommand( EXEC_APPEND, va("kick \"%i\"\n", g_entities[i].client->ps.clientNum ) );
					}
					else { //else give em a reason I guess
						str2 = ConcatArgs( 2 );
						Com_sprintf(reason, MAX_TOKEN_CHARS, "Kicked: %s", str2 );

						trap_DropClient( g_entities[i].client-level.clients, reason );
					}
			}
		}
	}

	//also, if we wanna destroy all bots
	else if ( !Q_stricmp( str, "allbots" ) ) {
		for (i = 0; i < MAX_CLIENTS; i++ ) {
			if ( (g_entities[i].client &&  g_entities[i].r.svFlags & SVF_BOT) ) {
				trap_SendConsoleCommand( EXEC_APPEND, va("kick \"%i\"\n", g_entities[i].client->ps.clientNum ) );
			}
		}
	}
	//The original kick2 code - crafted by Scooter
	else {
		cl = ClientForString( str );
		if ( !cl ) {
			return;
		}

		//TiM: trap_Argc works along the principle that the command name itself (ie in this case, "kick2") is an argument too
		if ( trap_Argc() <= 2 ) {
			trap_SendConsoleCommand( EXEC_APPEND, va("kick \"%i\"\n", cl->ps.clientNum ) );
		}
		else {
			// show him the exit
			str2 = ConcatArgs( 2 );
			Com_sprintf(reason, MAX_TOKEN_CHARS, "Kicked: %s", str2 );

			trap_DropClient( cl - level.clients, reason );
		}
	}
}

/*
=================
Cmd_ClampInfo_f

TiM: If an admin does to a player, they'll
become unable to change any of their
server side parameters (ie name, model, etc)
=================
*/

static void Cmd_ClampInfo_f( gentity_t* ent ) {
	char		buffer[5];
	char		*message;
	gentity_t	*targ;

	if ( !ent->client )
		return;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_CLAMP) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	//get the arg
	trap_Argv( 1, buffer, sizeof( buffer ) );

	//no arg
	if ( !buffer[0] ) {
		trap_SendServerCommand( ent-g_entities, "print \"Usage: Force stops players from changing their user info\n\nCommand: clampInfo <client ID>\n\"" );
		return;
	}

	targ = &g_entities[atoi(buffer)];

	if( !targ || !targ->client )
		return;

	targ->flags ^= FL_CLAMPED;

	if ( targ->flags & FL_CLAMPED )
		message = va( "%s ^7has now had their info clamped.", targ->client->pers.netname);
	else
		message = va( "%s ^7has now had their info un-clamped.", targ->client->pers.netname);

	trap_SendServerCommand( ent-g_entities, va( "print \"%s\n\"", message ) );

}

/*
=================
Cmd_Turbolift_f

TiM: Someone entered a turbolift command
sets up the needed elements
=================
*/

static void Cmd_Turbolift_f( gentity_t* ent )
{
	char arg[4];
	int numEnts;
	int	touch[MAX_GENTITIES];
	vec3_t mins, maxs;
	int	targDeck;
	static vec3_t	range = { 40, 40, 52 };
	int i;
	gentity_t *lift=NULL;
	gentity_t *otherLift=NULL;
	playerState_t *ps;

	if ( !ent->client )
		return;

	ps = &ent->client->ps;

	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR )
		return;

	if ( ps->stats[STAT_HEALTH] <= 0 ) {
		return;
	}

	trap_Argv( 2, arg, sizeof(arg) );

	if ( !arg[0] )
	{
		trap_SendServerCommand( ent-g_entities, "print \"You must specify a deck\n\" " );
		return;
	}

	targDeck = atoi( arg );

	if ( targDeck <= 0 )
	{
		trap_SendServerCommand( ent-g_entities, "print \"Error: Deck was not recognized\n\" " );
		return;
	}

	//checking to make sure we're inside a lift ent right now
	VectorSubtract( ps->origin, range, mins );
	VectorAdd( ps->origin, range, maxs );

	numEnts = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	for ( i = 0; i<numEnts; i++ )
	{
		lift = &g_entities[touch[i]];

		if ( !lift )
			continue;

		//found our ent!
		if ( !Q_stricmp( lift->classname, "target_turbolift" ) )
		{
			break;
		}
		//reset it
		lift = NULL;
	}

	if ( !lift  )
	{
		trap_SendServerCommand( ent-g_entities, "print \"You are not inside a turbolift\n\" " );	
		return;
	}

	if ( lift->flags & FL_LOCKED && (!IsAdmin(ent)) ) {
		trap_SendServerCommand( ent-g_entities, "print \"Turbolift is offline.\n\"" );
		return;
	}

	if ( lift->count > 0 )
	{
		trap_SendServerCommand( ent-g_entities, "print \"Turbolift is currently in use.\n\" " );	
		return;		
	}

	if ( lift->health == targDeck )
	{
		trap_SendServerCommand( ent-g_entities, "print \"You cannot specify your current turbolift\n\" " );	
		return;		
	}

	//FIXME: random deck->turbolift search
	//Fixed
	{
		int			numLifts=0;
		gentity_t*	lifts[12];

		while ( ( otherLift=G_Find( otherLift, FOFS( classname ), "target_turbolift" )) != NULL )
		{
			if ( otherLift->health == targDeck )
			{
				if ( numLifts >= 12 )
					break;

				lifts[numLifts] = otherLift;
				numLifts++;
			}
		}

		if ( !numLifts )
		{
			otherLift = NULL;
		}
		else
		{
			otherLift = lifts[rand() % numLifts];
		}
	}

	if ( !otherLift )
	{
		trap_SendServerCommand( ent-g_entities, "print \"Could not find desired deck number.\n\" " );	
		return;	
	}

	lift->count = otherLift - g_entities;
	otherLift->count = lift - g_entities;

	lift->nextthink = level.time + FRAMETIME;
	lift->think = target_turbolift_start;
}

/*
=================
Cmd_Me_f
=================
*/
static void Cmd_MeAction_f( gentity_t* ent )
{
	char message[512];

	if ( !ent->client )
		return;

	if ( trap_Argc() < 2 )
	{
		trap_SendServerCommand( ent-g_entities, "print \"Chat Emote Command.\nUsage: type a 3rd person action sentence.\nExample: 'sits down'.\n\"" );
		return;
	}

	//RPG-X | Marcin | 24/12/2008
	//n00b check
	if ( g_classData[ent->client->sess.sessionClass].isn00b ) {
		trap_SendServerCommand( ent-g_entities, "print \"[You're too stupid to use this command]\n\"");
		return;
	}

	Q_strncpyz( message, ConcatArgs( 1 ), sizeof( message ) );

	trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " %s\n\"", ent->client->pers.netname, message ) );
}

/*
=================
Cmd_MeLocal_f
Based of J's code
=================
*/
static void Cmd_MeActionLocal_f( gentity_t* ent )
{
	char message[512];
	gentity_t* OtherPlayer;					//Entity pointers to other players in game (used in loop)
	int i;									//Loop Counter

	if ( !ent->client )
		return;

	if ( trap_Argc() < 2 )
	{
		trap_SendServerCommand( ent-g_entities, "print \"Chat Emote Command in your Immediate Area.\nUsage: type a 3rd person action sentence.\nExample: 'sits down'.\n\"" );
		return;
	}

	//RPG-X | Marcin | 24/12/2008
	//n00b check
	if ( g_classData[ent->client->sess.sessionClass].isn00b ) {
		trap_SendServerCommand( ent-g_entities, "print \"[You're too stupid to use this command]\n\"");
		return;
	}

	Q_strncpyz( message, ConcatArgs( 1 ), sizeof( message ) );

	if ( g_dedicated.integer ) {
		G_Printf( "%s emoted to area: %s\n", ent->client->pers.netname, message);
	}


	//Loop Through Clients on the server
	//RPG-X: J2J - BugFix: used to be connected clients which meant most clients were missed out and other bugs.
	//					   Change to max clients and the loop ingores invalid players.
	for(i = 0; i < level.maxclients; i++)
	{

		OtherPlayer = &g_entities[i];			//Point OtherPlayer to next player

		//Send message to admins warning about command being used.
		//TiM - since double spamming is annoying, ensure that the target admin wants this alert
		if ( !OtherPlayer->client->noAdminChat && IsAdmin(OtherPlayer) )
			trap_SendServerCommand( -1, va("print \"%s" S_COLOR_CYAN" (locally) " S_COLOR_WHITE "%s\n\"", ent->client->pers.netname, message ) );

		//Check is OtherPlayer is valid
		if ( !OtherPlayer || !OtherPlayer->inuse || !OtherPlayer->client ) 
		{
			continue;
		}

		if ( trap_InPVS( ent->client->ps.origin, OtherPlayer->client->ps.origin ) )
		{
			//Communicate to the player	
			trap_SendServerCommand( i, va("print \"%s" S_COLOR_WHITE " %s\n\"", ent->client->pers.netname, message ) );
		}

		OtherPlayer = NULL;					//Reset pointer ready for next iteration.
	}
}

static void Cmd_MapsList_f( gentity_t *ent )
{
	char	mapList[1024];
	char	fileList[4096];
	int		numFiles;
	char	*filePtr;
	int		i;
	int		len;

	if ( !ent->client )
		return;

	numFiles = trap_FS_GetFileList( "maps", "bsp", fileList, sizeof(fileList) );

	if ( numFiles <= 0 )
		return;

	filePtr = fileList;
	memset( mapList, 0, sizeof(mapList) );

	for ( i = 0; i < numFiles; i++, filePtr += (len+1) )
	{
		len = strlen( filePtr );

		if ( len <= 0 || !filePtr )
			return;

		if ( strchr(filePtr,'/') || strchr(filePtr,'\\') )
		{
			continue;
		}

		if ( strlen(mapList) + len + 20 > sizeof( mapList ) )
			break;

		Q_strcat( mapList, sizeof( mapList ), filePtr );
		Q_strcat( mapList, sizeof( mapList ), "\n" );
	}

	trap_SendServerCommand( ent-g_entities, va( "print \"%s\" ", mapList ) );
}


/*
=================
Cmd_Drop_f
RPG-X
Marcin 03/12/2008
=================
*/
static void Cmd_Drop_f( gentity_t *ent ) {
	char txt[512];

	if ( trap_Argc() < 2 ) {
		txt[0] = '\0';
	}

	Q_strncpyz( txt, ConcatArgs( 1 ), sizeof( txt ) );
	ThrowWeapon( ent, txt );
}

/*
=================
Cmd_lockDoor_f
GSIO01 | 08/05/2009
=================
*/
static void Cmd_lockDoor_f(gentity_t *ent) {
	//RPG-X | GSIO01 | 08/05/2009: I use the useent code as base
	char		entArg[MAX_TOKEN_CHARS];
	gentity_t	*targetEnt;
	int			index;
	playerState_t *ps;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_LOCK) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_Argv( 1, entArg, sizeof( entArg ) );

	//No arguments - Do a trace 
	if ( !entArg[0] ) {
		vec3_t start, forward, end;
		trace_t tr;

		ps = &ent->client->ps;

		//calc start
		VectorCopy( ent->r.currentOrigin, start );
		start[2] += ps->viewheight;

		//calc end
		AngleVectors( ps->viewangles, forward, NULL, NULL );
		VectorMA( start, 8192, forward, end );

		//trace-er-roo
		trap_Trace( &tr, start, NULL, NULL, end, ps->clientNum, MASK_SHOT );

		index = tr.entityNum;
	}
	else { //We gotz an arg, so put it in
		index = atoi( entArg );
	}

	//invalid value ... so I guess it could be a targetname
	if ( index < MAX_CLIENTS || index >= ENTITYNUM_WORLD ) {
		targetEnt = G_Find (NULL, FOFS(targetname), entArg );

		//STILL no dice?? Gah... just eff it then lol.
		if ( !targetEnt ) {	
			return;
		}
	}
	else {
		targetEnt = &g_entities[index]; //get the ent
	}

	//find out the ent is valid, and it is useable
	//TiM: Hack. Well... since we can use usables anyway, I'ma gonna disable them here.
	//Doing it this way can screw up maps bigtime. >.<
	//RPG_Chambers may never be the same again.... :S
	if ( ( targetEnt ) ) {
		if ( Q_stricmp( targetEnt->classname, "func_door" ) ) {
			//GSIO01 not a func_door?? well then check wheter its a rotating door
			if( Q_stricmp(targetEnt->classname, "func_door_rotating") ) {
				trap_SendServerCommand( ent-g_entities, va( " print \"Entity %i isn't a door.\n\" ", index ) );
				return;
			}
		}

		targetEnt->flags ^= FL_LOCKED;
	}
}

/*
=================
Cmd_ffColor_f
GSIO01 | 09/05/2009
=================
*/
static void Cmd_ffColor_f(gentity_t *ent) {
	char	arg[16];
	int		i;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_FFSTUFF) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_Argv(1, arg, sizeof(arg));
	i = atoi(arg);
	if(i < 0 || i > 3)
		trap_Cvar_Set("rpg_forceFieldColor", "0");
	else
		trap_Cvar_Set("rpg_forceFieldColor", va("%i", i));
}

static int lastRemodulation[3] = { 0, 0 , 0 };
/*
=================
Cmd_remodulate_f
GSIO01 | 09/05/2009
=================
*/
static void Cmd_remodulate_f(gentity_t *ent) {
	char	arg[16];
	int		i;
	if(!rpg_allowRemodulation.integer)
		return;
	if(trap_Argc() < 2) {
		G_PrintfClient(ent, "Usage: remodulate i, where i: 1 = phaser, 2 = phaser rifle and 3 = disruptor\n");
		G_PrintfClient(ent, "Usage: phaser, prifle, disruptor\n");
		return;
	}
	trap_Argv(1, arg, sizeof(arg));
	if(!Q_strncmp(arg, "phaser", 6))
		i = 1;
	else if(!Q_strncmp(arg, "prifle", 6))
		i = 2;
	else if(!Q_strncmp(arg, "disruptor", 9))
		i = 3;
	else
		i = atoi(arg);

	if(i < 1 || i > 3) return; 

	if(lastRemodulation[i-1] && (level.time - lastRemodulation[i-1] > rpg_RemodulationDelay.integer)) {
		G_PrintfClient(ent, "You have to wait some time until you can remodulate again.\n");
		return;
	}

	switch(i) {
	case 1:
		level.borgAdaptHits[WP_5] = 0;
		trap_SendServerCommand(-1, va("print \"Phasers have been remodulated.\n\""));
		break;
	case 2:
		level.borgAdaptHits[WP_6] = 0;
		trap_SendServerCommand(-1, va("print \"Compression Rifles have been remodulated.\n\""));
		break;
	case 3:
		level.borgAdaptHits[WP_10] = 0;
		trap_SendServerCommand(-1, va("print \"Disruptors have been remodulated.\n\""));
		break;
	default:
		G_PrintfClient(ent, "Usage: remodulate i, where i: 1 = phaser, 2 = phaser rifle and 3 = disruptor\n");
		break;
	}
}

/*
=================
Cmd_unlockAll_f
GSIO01 | 11/05/2009
=================
*/
static void Cmd_unlockAll_f(gentity_t *ent) {
	int i;
	if(!ent || !ent->client)
		return;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_LOCK) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	for(i = g_maxclients.integer; i < MAX_GENTITIES ; i++) {
		if(Q_stricmp(g_entities[i].classname, "func_door") == 0 && (g_entities[i].flags & FL_LOCKED))
			g_entities[i].flags ^= FL_LOCKED;
		else if(Q_stricmp(g_entities[i].classname, "func_door_rotating") == 0 && (g_entities[i].flags & FL_LOCKED))
			g_entities[i].flags ^= FL_LOCKED;
	}
	G_PrintfClient(ent, "All doors unlocked.\n");
}

/*
=================
Cmd_Respawn_f
GSIO01 | 12/05/2009
=================
*/
static void Cmd_Respawn_f(gentity_t *ent) {
	gentity_t *tent;
	if(!ent)
		return;
	if(!ent->client)
		return;

	G_Client_Spawn(ent, 0, qfalse);
	if(ent->client->sess.sessionTeam != TEAM_SPECTATOR) {
		ent->client->ps.powerups[PW_QUAD] = level.time + 4000;
		tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TRANSPORT_IN );
		tent->s.clientNum = ent->s.clientNum;
	}
}

/*
=================
Cmd_lockAll_f
GSIO01 | 12/05/2009
=================
*/
static void Cmd_lockAll_f(gentity_t *ent) {
	int i;
	if(!ent || !ent->client)
		return;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_LOCK) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	for(i = g_maxclients.integer; i < MAX_GENTITIES; i++) {
		if(Q_stricmp(g_entities[i].classname, "func_door") == 0 && !(g_entities[i].flags & FL_LOCKED))
			g_entities[i].flags ^= FL_LOCKED;
		else if(Q_stricmp(g_entities[i].classname, "func_door_rotating") == 0 && !(g_entities[i].flags & FL_LOCKED))
			g_entities[i].flags ^= FL_LOCKED;
	}
	G_PrintfClient(ent, "All doors locked.\n");
}

/*
=================
Cmd_changeFreq
GSIO01 | 12/05/2009
=================
*/
static void Cmd_changeFreq(gentity_t *ent) {
	float i;
	char  arg[16];
	if(!ent || !ent->client)
		return;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_FFSTUFF) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	if(trap_Argc() < 2) {
		G_PrintfClient(ent, "Usage: changeFreq value, -1 = level 10 forcefield, 0 = random or any value between 0 and 20\n");
		return;
	}
	trap_Argv(1, arg, sizeof(arg));
	i = atof(arg);
	switch(atoi(arg)) {
	case -1:
		i = -1;
		break;
	case 0:
		i = flrandom(0.1, 20);
		break;
	default:
		if(i < 0 || i > 20) {
			i = flrandom(0.1, 20);
		}
		break;
	}
	trap_Cvar_Set("rpg_forceFieldFreq", va("%f", i));
	G_Printf("%f\n", rpg_forceFieldFreq.value);
}

/*
=================
Cmd_alert_f
GSIO01 | 12/05/2009
=================
*/
static void Cmd_alert_f(gentity_t *ent) {
	gentity_t	*alertEnt, *te;
	char		arg[10], arg2[16];
	int			i = 0;
	if(!ent || !ent->client)
		return;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_ALERT) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	alertEnt = G_Find(NULL, FOFS(classname), "target_alert");
	if(!alertEnt) {
		G_PrintfClient(ent, "This map doesn't seem to support the alert command.\n");
		return;
	}
	if(trap_Argc() < 2) {
		G_PrintfClient(ent, "Usage: alert condition, where condition can be yellow, red, green, or blue.\nUsage: alert condition sound, where for condition see above and sound can be 0 = don't toggle and 1 = toggle.\nUsage: alert toggle, toggles the sound.\n");
		return;
	}
	te = G_Spawn();
	trap_Argv(1, arg, sizeof(arg));
	if(trap_Argc() > 2) {
		trap_Argv(2, arg2, sizeof(arg2));
		i = atoi(arg2);
	}
	if(!Q_stricmp(arg, "red"))
		te->target = alertEnt->falsename;
	else if(!Q_stricmp(arg, "yellow"))
		te->target = alertEnt->truename;
	else if(!Q_stricmp(arg, "blue"))
		te->target = alertEnt->bluename;
	else if(!Q_stricmp(arg, "green"))
		te->target = alertEnt->swapname;
	else if(!Q_stricmp(arg, "toggle")) {
		switch(alertEnt->damage) {
		case 0: // green
			te->target = alertEnt->swapname;
			break;
		case 1: // yellow
			te->target = alertEnt->truename;
			break;
		case 2: // red
			te->target = alertEnt->falsename;
			break;
		case 3: // blue
			te->target = alertEnt->bluename;
			break;
		}
		if(i == 1)
			i = 0;
	}
	else {
		G_PrintfClient(ent, "Invalid alert condition \'%s\'. Valid conditions: red, blue, yellow, green.\n", arg);
		return;
	}
	if(i == 1) {
		te->classname = "dummy";
		alertEnt->use(alertEnt, NULL, te);
		te->classname = NULL;
	}
	alertEnt->use(alertEnt, NULL, te);
	if(!te)
		G_Printf("te got removed\n");
}

/*
=================
Cmd_zonelist_f
Harry Young | 02/11/2012
=================
*/
static void Cmd_zonelist_f(gentity_t *ent) {
	gentity_t	*zone=NULL;
	char		arg[16];
	int			type;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_SMS ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_Argv(1, arg, sizeof(arg));

	type = atoi(arg);

	if(type == 0 || type > 2){
		trap_SendServerCommand( ent-g_entities, va("print \"Usage: zonelist <type> where type is one of the following types of zones:\n--1 = safezones for Selfdestruct\n--2 = MSD-HUD zones\n\" ") );
		return;
	}

	if(type == 1)
		trap_SendServerCommand( ent-g_entities, va("print \"\nList of safezones on this map: \n\n\""));
	if(type == 2)
		trap_SendServerCommand( ent-g_entities, va("print \"\nList of MSD-HUD-zones on this map: \n\n\""));

	while((zone = G_Find(zone, FOFS(classname), "target_zone")) != NULL){
		if(zone->count == type){
			trap_SendServerCommand( ent-g_entities, va("print \"Name of zone: %s \n\"", zone->targetname ));
			if(zone->n00bCount == 1)
				trap_SendServerCommand( ent-g_entities, va("print \"Status of safezone: ^2safe \n\""));
			else
				trap_SendServerCommand( ent-g_entities, va("print \"Status of safezone: ^1unsafe \n\""));
			if(zone->spawnflags & 2)
				trap_SendServerCommand( ent-g_entities, va("print \"Flagges as ship: yes \n\n\""));
			else
				trap_SendServerCommand( ent-g_entities, va("print \"Flagged as ship: no \n\n\""));
		}
	}
	trap_SendServerCommand( ent-g_entities, va("print \"End of list \n\n\""));
}

/*
=================
Cmd_selfdestruct_f
Harry Young | 25/07/2012
=================
*/
static void Cmd_selfdestruct_f(gentity_t *ent) {
	gentity_t	*destructEnt, *safezone=NULL;
	char		arg[16], arg2[16], arg6[16], arg7[16], arg8[16];
	double		ETAmin, ETAsec;
	if(!ent || !ent->client)
		return;

	//Trapping all potential args here.
	trap_Argv(1, arg, sizeof(arg));
	trap_Argv(2, arg2, sizeof(arg2));
	trap_Argv(3, arg6, sizeof(arg6));
	trap_Argv(4, arg7, sizeof(arg7));
	trap_Argv(5, arg8, sizeof(arg8));

#ifndef SQL
		if ( !IsAdmin( ent ) ) {
			trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
			return;
		}
#else
		if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_SMS ) ) {
			trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
			return;
		}
#endif


	// Setup command-Execution

	if (!Q_stricmp(arg, "start")) {
		//Is there sth running alrerady?
		destructEnt = G_Find(NULL, FOFS(classname), "target_selfdestruct");
		if(destructEnt) {
			G_PrintfClient(ent, "^1ERROR: There's already a self destruct in progress, aborting setup.");
			return;
		}

		//There is not so let's set this up.
		destructEnt = G_Spawn();
		destructEnt->classname = "target_selfdestruct";
		destructEnt->wait = atoi(arg2);
		destructEnt->spawnflags = atoi(arg6);
		destructEnt->bluename = G_NewString(arg7);
		destructEnt->target = G_NewString(arg8);

		//we need to check a few things here to make sure the entity works properly. Else we free it.
		if ( destructEnt->wait <= 0 )
			G_PrintfClient(ent, "^1ERROR: duration must not be 0. Removing entity."); 

		while((safezone = G_Find(safezone, FOFS(classname), "target_zone")) != NULL){
			if(!destructEnt->bluename && safezone->count == 1 && safezone->spawnflags & 2){
				G_PrintfClient(ent, "^1ERROR: safezone must be given for maps consisting of multiple ships/stations (like rpg_runabout). For a list of safezones use this command: zonelist 1 . Removing entity.");
				destructEnt->wait = 0; //we'll use this next to free the ent
				break;
			}
		}
		if(destructEnt->wait <= 0)
			G_FreeEntity(destructEnt);
		else
			G_CallSpawn(destructEnt);
		return;
		
	} else if (!Q_stricmp(arg, "remaining")) {
		//Is there sth running alrerady?
		destructEnt = G_Find(NULL, FOFS(classname), "target_selfdestruct");
		if(!destructEnt) {
			G_PrintfClient(ent, "^1ERROR: There's no self destruct in progress, aborting call.");
			return;
		}

		if(destructEnt->spawnflags == 1)
			return; //we simply don't need this while there is a visible countdown. 

		//we need the remaining time in minutes and seconds from that entity. Just ask them off and have the command do the math.
		ETAsec = floor(modf((( floor(destructEnt->damage / 1000) - floor(level.time / 1000) ) / 60), &ETAmin)*60); //break it apart, put off the minutes and return the floored secs
		if(ETAsec / 10 < 1) //get leading 0 for secs
			trap_SendServerCommand( -1, va("servermsg \"^1Self Destruct in %.0f:0%.0f\"", ETAmin, ETAsec ));
		else
			trap_SendServerCommand( -1, va("servermsg \"^1Self Destruct in %.0f:%.0f\"", ETAmin, ETAsec ));
	} else if (!Q_stricmp(arg, "abort")) {
		//Is there sth running alrerady?
		destructEnt = G_Find(NULL, FOFS(classname), "target_selfdestruct");
		if(!destructEnt) {
			G_PrintfClient(ent, "^1ERROR: There's no self destruct in progress, aborting call.");
			return;
		}
		destructEnt->use(destructEnt, NULL, NULL); // Use-Function will simply manage the abort
	} else {
		//maybe hook up a setup UI here later.
		G_PrintfClient(ent,		"^1ERROR: Invalid or no command-Argument. Arguments are start, remaining and abort");
		G_PrintfClient(ent,		"^3Usage: selfdestruct start duration audio [safezone] [target]");
		G_PrintfClient(ent,		"duration: total countdown-duration in seconds. Must not be 0.");
		G_PrintfClient(ent,		"audio: set this 0 if you do not want to display the countdown-clock in the top center of your screen, else set this 1.");
		G_PrintfClient(ent,		"safezone: Only for maps with multiple ships (like rpg_runabout). it will not be used for processing. Set NULL to skip.");
		G_PrintfClient(ent,		"target: Optional Argument for Effects to fire once the countdown hist 0.");
		G_PrintfClient(ent,		"The entity will automatically shake everyones screen and kill all clients outside an active target_zone configured as safezone.");
		G_PrintfClient(ent,		"\n^3Usage: selfdestruct remaining");
		G_PrintfClient(ent,		"This will give out the remaining countdown-time to all clients if the count is muted.");
		G_PrintfClient(ent,		"\n^3Usage: selfdestruct abort");
		G_PrintfClient(ent,		"This will abort any self destruct running");
		return;
	}
}

/*
=================
Cmd_shipdamage_f
Harry Young | 02/08/2012
=================
*/
static void Cmd_shipdamage_f(gentity_t *ent) {
	gentity_t	*healthEnt=NULL;
	char		arg[16];
	char		target[512];

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_SMS ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	if(trap_Argc() == 0){
		G_PrintfClient(ent,	"Usage: shipdamage damage [target] where damage is the total amount dealt and target is the target_shiphealth to recieve it. It is case-dependent and required on maps with 2 or more of such (like rpg_runabout).\n");
		G_PrintfClient(ent,	"It will be rendered to shields and hull respectively by the entity. Must be positive. You can not heal with this command.\n");
		return;
	}

	if(trap_Argc() > 1){
		Q_strncpyz( target, ConcatArgs( 2 ), sizeof( target ) );

		while((healthEnt = G_Find(healthEnt, FOFS(classname), "target_shiphealth")) != NULL){
			if(!Q_stricmp(healthEnt->targetname, target))//find the right entity if we have a target
				break;
		}
	}

	if(!healthEnt){//specific search did not turn up any results so fall back to the first entity
		trap_SendServerCommand( ent-g_entities, va("print \"^3 Warning: no healthEnt with targetname '%s' found, falling back to first entity found.\n\"", target ) );
		healthEnt = G_Find(NULL, FOFS(classname), "target_shiphealth");
		if(G_Find(healthEnt, FOFS(classname), "target_shiphealth")){//abort if we do have more than one healthEnt
			trap_SendServerCommand( ent-g_entities, va("print \"^1 ERROR: This map has more than one target_shiphealth, therefore a target needs to be specified. Aborting call.\n\"" ) );
			return;
		}
	}

	if(!healthEnt){
		trap_SendServerCommand( ent-g_entities, "print \"^4This map does not support the shiphealth system.\n\"" );
		return;
	}

	trap_Argv(1, arg, sizeof(arg));

	if(atoi(arg) > 0){
		if(healthEnt->count > 0){
			healthEnt->damage = atoi(arg);
		}else{
			G_PrintfClient(ent,	"^1ERROR: The ship is destroyed.");
			return;
		}
	} else {
		G_PrintfClient(ent,	"^1ERROR: Damage must be a positive value. You can not heal with this command.");
		return;
	}
	healthEnt->use(healthEnt, NULL, NULL);
	return;
}

/*
=================
Cmd_shiphealth_f
Harry Young | 02/08/2012
=================
*/
static void Cmd_shiphealth_f(gentity_t *ent) {
	gentity_t	*healthEnt=NULL;
	int			THS, CHS, HCI, TSS, CSS, SCI, SI;
	float		RHS, RSS;

	if(!G_Find(NULL, FOFS(classname), "target_shiphealth")){
		trap_SendServerCommand( ent-g_entities, "print \"^3This map does not support the shiphealth system.\n\"" );
		return;
	}

	while((healthEnt = G_Find(healthEnt, FOFS(classname), "target_shiphealth")) != NULL){ //do this for every healthEnt you find

		THS = healthEnt->health;
		CHS = healthEnt->count;
		TSS = healthEnt->splashRadius;
		CSS = healthEnt->n00bCount;
		SI = healthEnt->splashDamage;

		//Restructured the next 2 segments to fix a compiler-warning
		RHS = ((CHS * pow(THS, -1)) * 100);
		if(RHS <= 25)//Hull Color Indicators
			HCI = 1;
		else if(RHS <= 50)
			HCI = 3;
		else if(RHS <= 100)
			HCI = 2;
		else
			HCI = 7;

		RSS = ((CSS * pow(TSS, -1)) * 100);
		if(RSS <= 25)//Shield Color Indicators
			SCI = 1;
		else if(RSS <= 50)
			SCI = 3;
		else if(RSS <= 100)
			SCI = 2;
		else
			SCI = 7;

		if(CHS == 0){
			trap_SendServerCommand( ent-g_entities, va("print \"\n^1 %s is destroyed.\n\n\"", healthEnt->targetname ) );
		} else {
			trap_SendServerCommand( ent-g_entities, va("print \"\n^3 %s : Tactical Master Systems Display\n\"", healthEnt->targetname ) );
			switch(SI){
			case -2:
				trap_SendServerCommand( ent-g_entities, "print \"^1 Shields are offline\n\"" );
				break;
			case -1:
				trap_SendServerCommand( ent-g_entities, "print \"^1 Shields are inoperable\n\"" );
				break;
			case 0:
				trap_SendServerCommand( ent-g_entities, "print \"^3 Shields are standing by\n\"" );
				break;
			case 1:
				trap_SendServerCommand( ent-g_entities, "print \"^2 Shields are online\n\"" );
				break;
			}
			if(CSS>0)
				trap_SendServerCommand( ent-g_entities, va("print \"^%i Shield Capactiy at %.0f Percent (%i of %i Points)\n\"", SCI, RSS, CSS, TSS) );
			trap_SendServerCommand( ent-g_entities, va("print \"^%i Structual Integrity at %.0f Percent (%i of %i Points)\n\n\"", HCI, RHS, CHS, THS) );
		}
	}
	return;
}

/*
=================
Cmd_reloadtorpedos_f
Harry Young | 12/01/2013
=================
*/
static void Cmd_reloadtorpedos_f(gentity_t *ent) {
	gentity_t	*torpedo=NULL;
	char		arg[16];
	char		target[512];

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_SMS ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_Argv(1, arg, sizeof(arg));
	trap_Argv(2, target, sizeof(target));

	if(atoi(arg) == 0 || !Q_stricmp(target, "")){
		G_PrintfClient(ent,	"Usage: reloadtorpedos [amount] [target] where amount is the number of torpedos the launcher will have after the command executed."); 
		G_PrintfClient(ent,	"Set -2 to depleet, set -1 to max out. You can not load more torpedos than the entity was designed for. If you do it will just Max out."); 
		G_PrintfClient(ent,	"target is the specific targetname of the fx_torpedo that shall be reloaded.");
		G_PrintfClient(ent,	"If set to all every limited launcher on the map will be set to amount or maxed out accordingly.");
		G_PrintfClient(ent,	"The targetnames can be extracted trough the command torpedolist.");
		return;
	}

	if(atoi(arg) < -2){
		G_PrintfClient(ent,	"^1ERROR: amount must not be less than -2, aborting call.\n"); 
		return;
	}
	
	if(!G_Find(NULL, FOFS(classname), "fx_torpedo")){
		trap_SendServerCommand( ent-g_entities, "print \"^4This map does not support the fx_torpedo system.\n\"" );
		return;
	}

	while((torpedo = G_Find(torpedo, FOFS(classname), "fx_torpedo")) != NULL){
		if(!Q_stricmp(torpedo->targetname, target) || !Q_stricmp(target, "all")){ //this entity is generally affectable
			if(torpedo->damage > 0){ //we only need to consider this for restock if it is restockable
				if(atoi(arg) == -1 || atoi(arg) > torpedo->damage)
					torpedo->count = torpedo->damage;
				else if(atoi(arg) == -2)
					torpedo->count = 0;
				else 
					torpedo->count = atoi(arg);
			}
		}
		return;
	}
}

/*
=================
Cmd_torpedolist_f
Harry Young | 02/11/2012
=================
*/
static void Cmd_torpedolist_f(gentity_t *ent) {
	gentity_t	*torpedo=NULL;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_SMS ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_SendServerCommand( ent-g_entities, va("print \"\nList of fx_torpedo-entities on this map: \n\n\""));
	while((torpedo = G_Find(torpedo, FOFS(classname), "fx_torpedo")) != NULL){
		trap_SendServerCommand( ent-g_entities, va("print \"Name of fx_torpedo: %s \n\"", torpedo->targetname ));
		if(torpedo->spawnflags & 1)
			trap_SendServerCommand( ent-g_entities, va("print \"Type: Quantum \n\""));
		else
			trap_SendServerCommand( ent-g_entities, va("print \"Type: Photon \n\""));
		if(torpedo->count == -1)
			trap_SendServerCommand( ent-g_entities, va("print \"Amount: Unlimited\n\n\""));
		else{
			trap_SendServerCommand( ent-g_entities, va("print \"Current Amount: %i \n\"", torpedo->count));
			trap_SendServerCommand( ent-g_entities, va("print \"Maximum Amount: %i \n\n\"", torpedo->damage));
		}
	}
	trap_SendServerCommand( ent-g_entities, va("print \"End of list \n\n\""));
}

/*
=================
Cmd_admin_centerprint_f
GSIO01 | 12/05/2009
=================
*/
static void Cmd_admin_centerprint_f(gentity_t *ent) {
	char *arg;

	if ( trap_Argc () < 1 ) {
		return;
	}

	//If client isn't in admin class, exit and disallow command.

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_MESSAGE) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	if ( !ent || !ent->client ) {
		return;		// not fully in game yet
	}

	arg = ConcatArgs( 1 );

	if ( !arg[0] ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( ent-g_entities, va("print \"\nUsage: Admin broadcasts a message to all users on the server that stays for several seconds\nCommand: Msg2 \"<Message>\"\n\" ") );
		return;
	}

	if ( arg[0] == '\0' )
	{
		return;
	}

	//RPG-X: Marcin: changed to -1
	trap_SendServerCommand( -1, va("print \"%s\n\" ", arg));

	trap_SendServerCommand( -1, va("servercprint \"%s\"", arg));
}

/*
=================
Cmd_getBrushEntCount_f
=================
*/
static void Cmd_getBrushEntCount_f(gentity_t *ent) {

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_DEBUG ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	G_PrintfClient(ent, "Brush Entity Count: %i\n", level.numBrushEnts);
	if(level.numBrushEnts > (MAX_MODELS - 1))
		G_PrintfClient(ent, S_COLOR_YELLOW "Nelson sees this number and says: Ha Ha!\n");
}


// Dev commands:

// not in release
static void Cmd_listSPs(gentity_t *ent) {
	int i;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_DEBUG ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	G_Printf("Spawnpoint list: \n");
	for(i = 0; i < MAX_GENTITIES; i++) {
		if(!Q_stricmp(g_entities[i].classname, "info_player_start"))
			G_Printf("Spawnpoint type: info_player_start Origin: %s\n", vtos(ent->s.origin));
		if(!Q_stricmp(g_entities[i].classname, "info_player_deathmatch")) 
			G_Printf("Spawnpoint type: info_player_deathmatch Origin: %s\n", vtos(ent->s.origin));
	}
}

/*
=================
Cmd_getEntInfo_f
=================
*/
static void Cmd_getEntInfo_f(gentity_t *ent) {
	int i;
	char arg[10];
	playerState_t *ps;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_DEBUG ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_Argv(1, arg, sizeof(arg));

	if ( !arg[0] ) {
		vec3_t start, forward, end;
		trace_t tr;
		ps = &ent->client->ps;
		VectorCopy( ent->r.currentOrigin, start );
		start[2] += ps->viewheight;
		AngleVectors( ps->viewangles, forward, NULL, NULL );
		VectorMA( start, 8192, forward, end );
		trap_Trace( &tr, start, NULL, NULL, end, ps->clientNum, MASK_SHOT );
		i = tr.entityNum;
	}
	else {
		i = atoi( arg );
	}

	if(i > MAX_GENTITIES - 1)
		return;

	G_PrintfClient(ent, "Classname: %s\n Targetname: %s\n Target: %s\n Spawnflags: %i\n Bmodel: %s\n\"", g_entities[i].classname, g_entities[i].targetname, g_entities[i].target, g_entities[i].spawnflags, g_entities[i].model);
}

/*
=================
Cmd_getOrigin_f
=================
*/
static void Cmd_getOrigin_f(gentity_t *ent) {
	int i;
	char arg[10];
	playerState_t *ps;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_DEBUG ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_Argv(1, arg, sizeof(arg));

	if ( !arg[0] ) {
		vec3_t start, forward, end;
		trace_t tr;
		ps = &ent->client->ps;
		VectorCopy( ent->r.currentOrigin, start );
		start[2] += ps->viewheight;
		AngleVectors( ps->viewangles, forward, NULL, NULL );
		VectorMA( start, 8192, forward, end );
		trap_Trace( &tr, start, NULL, NULL, end, ps->clientNum, MASK_SHOT );
		i = tr.entityNum;
	}
	else {
		i = atoi( arg );
	}

	if( i > MAX_GENTITIES - 1)
		return;

	G_PrintfClient(ent, "ent->s.origin        = %s", vtos(g_entities[i].s.origin));
	G_PrintfClient(ent, "ent->r.currentOrigin = %s", vtos(g_entities[i].r.currentOrigin));
	G_PrintfClient(ent, "ent->pos1            = %s", vtos(g_entities[i].pos1));
	G_PrintfClient(ent, "ent->pos2            = %s", vtos(g_entities[i].pos2));
	G_PrintfClient(ent, "ent->s.pos.trBase    = %s", vtos(g_entities[i].s.pos.trBase));
}

/*
=================
Cmd_getEntByTargetname_f
=================
*/
static void Cmd_getEntByTargetname_f(gentity_t *ent) {
	int i;
	char arg[MAX_STRING_TOKENS];
	gentity_t *t;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_DEBUG ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_Argv(1, arg, sizeof(arg));

	for(i = 0; i < MAX_GENTITIES; i++) {
		if(!&g_entities[i]) continue;
		t = &g_entities[i];
		if(t->targetname && !Q_stricmpn(t->targetname, arg, strlen(arg)))
			G_PrintfClient(ent, "ENT %i: %s", i, t->classname);
		if(t->targetname2 && !Q_stricmpn(t->targetname2, arg, strlen(arg)))
			G_PrintfClient(ent, "ENT %i: %s (targetname2)", i, t->classname);
	}
}

/*
=================
Cmd_getEntByTarget_f
=================
*/
static void Cmd_getEntByTarget_f(gentity_t *ent) {
	char arg[MAX_STRING_TOKENS];
	int cnt;
	int i;
	gentity_t *entities[MAX_GENTITIES];

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_DEBUG ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_Argv(1, arg, sizeof(arg));

	cnt = G_GetEntityByTarget(arg, entities);

	for(i = 0; i < cnt; i++) {
		if(!entities[i] || !entities[i]->classname) continue;
		G_PrintfClient(ent, "ENT %i: %s\n\"", entities[i]->s.number, entities[i]->classname);
	}
}

/*
=================
Cmd_getEntByBmodel_f
=================
*/
static void Cmd_getEntByBmodel_f(gentity_t *ent) {
	char arg[MAX_STRING_TOKENS];
	gentity_t *entity;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_DEBUG ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_Argv(1, arg, sizeof(arg));

	entity = G_Find(NULL, FOFS(model), arg);

	if(!entity) return;

	G_PrintfClient(ent, "ENT %i: %s\n\"", entity->s.number, entity->classname);
}

// not in release
static void Cmd_setOrigin(gentity_t *ent) {
	int i;
	vec3_t origin;
	gentity_t *ent2;
	char arg[10];

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_DEBUG ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	trap_Argv(1, arg, sizeof(arg));
	i = atoi(arg);
	trap_Argv(2, arg, sizeof(arg));
	origin[0] = atof(arg);
	trap_Argv(3, arg, sizeof(arg));
	origin[1] = atof(arg);
	trap_Argv(4, arg, sizeof(arg));
	origin[2] = atof(arg);

	if(i > MAX_GENTITIES - 1)
		return;

	ent2 = &g_entities[i];

	if(!ent2)
		return;

	VectorCopy(origin, ent2->s.origin);
	VectorCopy(origin, ent2->r.currentOrigin);
	trap_LinkEntity(ent);
	G_SetOrigin(ent2, origin);
}

/*
=================
Cmd_flushTentities_f
=================
*/
static void Cmd_flushTentities_f(gentity_t *ent) {
	int i;
	int cnt = 0;
	char arg[MAX_QPATH];

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_TESS) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	if(!trap_Argc()) {


	} else {
		trap_Argv(1, arg, sizeof(arg));
		if(!Q_stricmpn(arg, "all", 3)) {
			for(i = 0; i < MAX_GENTITIES; i++) {
				if(!Q_stricmpn(g_entities[i].classname, "tmp_", 4)) {
					G_FreeEntity(&g_entities[i]);
					cnt++;
				}
			}

			G_PrintfClient(ent, "Freed %i temporal entities.\n", cnt);
		} else {
			i = atoi(arg);
			if(i < 0 || i > MAX_GENTITIES - 1) {
				G_PrintfClient(ent, "Invalid entity num %i.\n\"", i);
				return;
			}
			if(!Q_stricmpn(g_entities[i].classname, "tmp_", 4)) {
				G_FreeEntity(&g_entities[i]);
			} else {
				G_PrintfClient(ent, "Entity %i is not a temporal entity or does not exist!\n\"", i);
			}
		}
	}
}

/*
=================
Cmd_spawnTEntity_f
=================
*/
static void Cmd_spawnTentity_f(gentity_t *ent) {
	char		arg[MAX_STRING_TOKENS], *arg2;
	vec3_t		origin, start, end, forward, angles;
	float		sizeX, sizeY, sizeZ;
	int			numArgs;
	int			clientNum;
	gentity_t	*newEnt;
	trace_t		tr;
	char		tmp[MAX_STRING_TOKENS];

	//return; // we don't want this to be avaible in the current release as it's not finised

	memset(arg, 0, sizeof(arg));
	memset(tmp, 0, sizeof(tmp));

	numArgs = trap_Argc();
	clientNum = ent->client->ps.clientNum;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_TESS) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	if(!numArgs) {
		trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for spawnTEnt!\n\"");
		return;
	}

	//get origin and angles
	//calc start
	VectorCopy( ent->r.currentOrigin, start );
	start[2] += ent->client->ps.viewheight;

	//calc end
	AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );
	VectorMA( start, 8192, forward, end );

	//trace-er-roo
	trap_Trace( &tr, start, NULL, NULL, end, clientNum, MASK_SHOT );

	VectorCopy( tr.endpos, origin );
	VectorCopy( tr.plane.normal, angles);

	newEnt = G_Spawn();

	if(!newEnt) {
		trap_SendServerCommand(clientNum, "print \"Could not spawn new entity. Most likely there are no free entities left.\n\"");
		return;
	}

	newEnt->tmpEntity = qtrue;

	G_SetOrigin(newEnt, origin);
	VectorCopy(angles, newEnt->s.angles);
	VectorCopy(angles, newEnt->r.currentAngles);

	trap_Argv(1, arg, sizeof(arg));

	//check wether this is a valid entity
	if(!Q_stricmpn(arg, "trigger_", 8)) {
		if(!Q_stricmp(arg, "trigger_teleport")) {
			newEnt->classname = G_NewString("trigger_teleport");
			if(numArgs < 5) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for trigger_teleport!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt trigger_teleport sizeX sizeY sizeZ target (spawnflags swapname speed)\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			arg2 = arg;
			sizeX = atof(arg2);
			trap_Argv(3, arg, sizeof(arg));
			arg2 = arg;
			sizeY = atof(arg2);
			trap_Argv(4, arg, sizeof(arg));
			arg2 = arg;
			sizeZ = atof(arg2);
			newEnt->r.mins[0] = newEnt->r.maxs[0] = sizeX;
			newEnt->r.mins[1] = newEnt->r.maxs[1] = sizeY;
			newEnt->r.mins[2] = newEnt->r.maxs[2] = sizeZ;
			VectorScale(newEnt->r.mins, -.5, newEnt->r.mins);
			VectorScale(newEnt->r.maxs, .5, newEnt->r.maxs);
			VectorCopy(newEnt->r.maxs, newEnt->r.absmax);
			VectorCopy(newEnt->r.mins, newEnt->r.absmin);
			trap_Argv(5, arg, sizeof(arg));
			newEnt->target = G_NewString(arg);
			if(numArgs >= 6) {
				trap_Argv(6, arg, sizeof(arg));
				newEnt->spawnflags = atoi(arg);
			}
			if(numArgs >= 7) {
				trap_Argv(7, arg, sizeof(arg));
				newEnt->swapname = G_NewString(arg);
			}
			if(numArgs >= 8) {
				trap_Argv(8, arg, sizeof(arg));
				newEnt->health = atof(arg);
			}
			G_CallSpawn(newEnt);
		}else if(!Q_stricmp(arg, "trigger_once")) { //actually trigger_multiple with wait of -1
			newEnt->classname = G_NewString("trigger_multiple");
			if(numArgs < 5) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of argmuments for trigger_once!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt trigger_once sizeX sizeY sizeZ target\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			arg2 = arg;
			sizeX = atof(arg2);
			trap_Argv(3, arg, sizeof(arg));
			arg2 = arg;
			sizeY = atof(arg2);
			trap_Argv(4, arg, sizeof(arg));
			arg2 = arg;
			sizeZ = atof(arg2);
			newEnt->r.mins[0] = newEnt->r.maxs[0] = sizeX;
			newEnt->r.mins[1] = newEnt->r.maxs[1] = sizeY;
			newEnt->r.mins[2] = newEnt->r.maxs[2] = sizeZ;
			VectorScale(newEnt->r.mins, -.5, newEnt->r.mins);
			VectorScale(newEnt->r.maxs, .5, newEnt->r.maxs);
			VectorCopy(newEnt->r.maxs, newEnt->r.absmax);
			VectorCopy(newEnt->r.mins, newEnt->r.absmin);
			trap_Argv(5, arg, sizeof(arg));
			newEnt->target = G_NewString(arg);
			G_CallSpawn(newEnt);
			newEnt->wait = -1;
		} else if(!Q_stricmp(arg, "trigger_multiple")) {
			newEnt->classname = G_NewString("trigger_multiple");
			if(numArgs < 6) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of argmuments for trigger_multple!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt trigger_multiple sizeX sizeY sizeZ target wait\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			arg2 = arg;
			sizeX = atof(arg2);
			trap_Argv(3, arg, sizeof(arg));
			arg2 = arg;
			sizeY = atof(arg2);
			trap_Argv(4, arg, sizeof(arg));
			arg2 = arg;
			sizeZ = atof(arg2);
			newEnt->r.mins[0] = newEnt->r.maxs[0] = sizeX;
			newEnt->r.mins[1] = newEnt->r.maxs[1] = sizeY;
			newEnt->r.mins[2] = newEnt->r.maxs[2] = sizeZ;
			VectorScale(newEnt->r.mins, -.5, newEnt->r.mins);
			VectorScale(newEnt->r.maxs, .5, newEnt->r.maxs);
			VectorCopy(newEnt->r.maxs, newEnt->r.absmax);
			VectorCopy(newEnt->r.mins, newEnt->r.absmin);
			trap_Argv(5, arg, sizeof(arg));
			newEnt->target = G_NewString(arg);
			G_CallSpawn(newEnt);
			trap_Argv(6, arg, sizeof(arg));
			newEnt->wait = atof(arg);
		} else if(!Q_stricmp(arg, "trigger_hurt")) {
			//newEnt = G_Spawn();
			newEnt->classname = G_NewString("trigger_hurt");
			if(numArgs < 4) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for trigger_hurt!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt trigger_hurt sizeX sizeY sizeZ dmg (targetname)");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			arg2 = arg;
			sizeX = atof(arg2);
			trap_Argv(3, arg, sizeof(arg));
			arg2 = arg;
			sizeY = atof(arg2);
			trap_Argv(4, arg, sizeof(arg));
			arg2 = arg;
			sizeZ = atof(arg2);
			newEnt->r.mins[0] = newEnt->r.maxs[0] = sizeX;
			newEnt->r.mins[1] = newEnt->r.maxs[1] = sizeY;
			newEnt->r.mins[2] = newEnt->r.maxs[2] = sizeZ;
			VectorScale(newEnt->r.mins, -.5, newEnt->r.mins);
			VectorScale(newEnt->r.maxs, .5, newEnt->r.maxs);
			VectorCopy(newEnt->r.maxs, newEnt->r.absmax);
			VectorCopy(newEnt->r.mins, newEnt->r.absmin);
			G_CallSpawn(newEnt);
			trap_Argv(5, arg, sizeof(arg));
			newEnt->damage = atoi(arg);
			if(numArgs >= 5) {
				trap_Argv(5, arg, sizeof(arg));
				newEnt->targetname = G_NewString(arg);
			}
		}
	} else if (!Q_stricmpn(arg, "fx_", 3)) {
		if(!Q_stricmp(arg, "fx_spark")) {
			//newEnt = G_Spawn();
			newEnt->classname = G_NewString("fx_spark");
			if(numArgs < 3) {
				G_FreeEntity(ent);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for fx_spark!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt fx_spark wait target (targetname spawnflags)\n\"");
				return;
			}
			trap_Argv(3, arg, sizeof(arg));
			newEnt->target = G_NewString(arg);
			if(numArgs >= 4) {
				trap_Argv(4, arg, sizeof(arg));
				newEnt->targetname = G_NewString(arg);
			}
			if(numArgs >= 5) {
				trap_Argv(5, arg, sizeof(arg));
				newEnt->spawnflags = atoi(arg);
			}
			G_CallSpawn(newEnt);
			trap_Argv(2, arg, sizeof(arg));
			newEnt->wait = atof(arg);
		} else if(!Q_stricmp(arg, "fx_surface_explosion")) {
			newEnt->classname = G_NewString("fx_surface_explosion");
			if(numArgs < 2) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for fx_surface_explosion!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt fx_surface_explosion targetname (spawnflags target damage radius speed)\"\n");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			newEnt->targetname = G_NewString(arg);
			if(numArgs >= 3) {
				trap_Argv(3, arg, sizeof(arg));
				newEnt->spawnflags = atoi(arg);
			}
			if(numArgs >= 4) {
				trap_Argv(4, arg, sizeof(arg));
				newEnt->target = G_NewString(arg);
			}
			if(numArgs >= 5) {
				trap_Argv(5, arg, sizeof(arg));
				newEnt->damage = atoi(arg);
			}
			if(numArgs >= 6) {
				trap_Argv(6, arg, sizeof(arg));
				newEnt->distance = atof(arg);
			}
			if(numArgs >= 7) {
				trap_Argv(7, arg, sizeof(arg));
				newEnt->speed = atof(arg);
			}
			G_CallSpawn(newEnt);
		} else if(!Q_stricmp(arg, "fx_blow_chunks")) {
			newEnt->classname = G_NewString("fx_blow_chunks");
			if(numArgs < 3) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for fx_blow_chunks!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt fx_blow_chunks targetname target (radius material)\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			newEnt->targetname = G_NewString(arg);
			trap_Argv(3, arg, sizeof(arg));
			newEnt->target = G_NewString(arg);
			if(numArgs>= 4) {
				trap_Argv(4, arg, sizeof(arg));
				newEnt->distance = atof(arg);
			}
			if(numArgs >= 5) {
				trap_Argv(5, arg, sizeof(arg));
				newEnt->s.powerups = atoi(arg);
			}
			G_CallSpawn(newEnt);
		} else if(!Q_stricmp(arg, "fx_electrical_explosion")) {
			newEnt->classname =  G_NewString("fx_electrical_explosion");
			if(numArgs < 2) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for fx_electrical_explosion!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt fx_electrical_explosion targetname (target radius damage)\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			newEnt->targetname = G_NewString(arg);
			if(numArgs >= 3) {
				trap_Argv(3, arg, sizeof(arg));
				newEnt->target = G_NewString(arg);
			}
			if(numArgs >= 4) {
				trap_Argv(4, arg, sizeof(arg));
				newEnt->distance = atof(arg);
			}
			if(numArgs >= 5) {
				trap_Argv(5, arg, sizeof(arg));
				newEnt->damage = atoi(arg);
			}
			G_CallSpawn(newEnt);
		}
	} else if(!Q_stricmpn(arg, "info_", 5)) {
		if(!Q_stricmp(arg, "info_notnull")) {
			newEnt->classname = G_NewString("info_notnull");
			if(numArgs < 2) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for info_notnull!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt info_notnull targetname\"\n");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			newEnt->targetname = G_NewString(arg);
			G_CallSpawn(newEnt);
		} else if(!Q_stricmp(arg, "info_player_deathmatch") || !Q_stricmp(arg, "info_player_start")) {
			newEnt->classname = G_NewString("info_player_deathmatch");
			G_CallSpawn(newEnt);
		}
	} else if(!Q_stricmpn(arg, "target_", 7)) {
		if(!Q_stricmp(arg, "target_boolean")) {
			newEnt->classname = G_NewString("target_boolean");
			if(numArgs < 5) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for target_boolean!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt target_boolean targetname truetarget \"");
				trap_SendServerCommand(clientNum, "print \"falsetarget swapname (spawnflags truename falsename)\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			newEnt->targetname = G_NewString(arg);
			trap_Argv(3, arg, sizeof(arg));
			newEnt->truetarget = G_NewString(arg);
			trap_Argv(4, arg, sizeof(arg));
			newEnt->falsetarget = G_NewString(arg);
			trap_Argv(5, arg, sizeof(arg));
			newEnt->swapname = G_NewString(arg);
			if(numArgs >= 6) {
				trap_Argv(6, arg, sizeof(arg));
				newEnt->spawnflags = atoi(arg);
			}
			if(numArgs >= 7) {
				trap_Argv(7, arg, sizeof(arg));
				newEnt->truename = G_NewString(arg);
			}
			if(numArgs >= 8) {
				trap_Argv(8, arg, sizeof(arg));
				newEnt->falsename = G_NewString(arg);
			}
			G_CallSpawn(newEnt);
		} else if(!Q_stricmp(arg, "target_counter")) {
			newEnt->classname = G_NewString("target_counter");
			if(numArgs < 4) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for target_count!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt target_counter targetname target count\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			newEnt->targetname = G_NewString(arg);
			trap_Argv(3, arg, sizeof(arg));
			newEnt->target = G_NewString(arg);
			trap_Argv(4, arg, sizeof(arg));
			newEnt->count = atoi(arg);
			G_CallSpawn(newEnt);
		} else if(!Q_stricmp(arg, "target_deactivate")) {
			newEnt->classname = G_NewString("target_deactivate");
			if(numArgs < 3) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for target_deactivate!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage spawnTEnt target_deactivate targetname target\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			newEnt->targetname = G_NewString(arg);
			trap_Argv(3, arg, sizeof(arg));
			newEnt->target = G_NewString(arg);
			G_CallSpawn(newEnt);
		} else if(!Q_stricmp(arg, "target_doorlock")) {
			newEnt->classname = G_NewString("target_doorlock");
			if(numArgs < 3) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of  arguments for target_doorlock!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage spawnTEnt target_doorlock target targetname (spawnflags lockMsg unlockMsg)\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			newEnt->target = G_NewString(arg);
			trap_Argv(3, arg, sizeof(arg));
			newEnt->targetname = G_NewString(arg);
			if(numArgs >= 4) {
				trap_Argv(4, arg, sizeof(arg));
				newEnt->spawnflags = atoi(arg);
			}
			if(numArgs >= 5) {
				trap_Argv(5, arg, sizeof(arg));
				newEnt->swapname = G_NewString(arg);
			}
			if(numArgs >= 6) {
				trap_Argv(5, arg, sizeof(arg));
				newEnt->truename = G_NewString(arg);
			}
			G_CallSpawn(newEnt);
		} else if(!Q_stricmp(arg, "target_relay")) {
			newEnt->classname = G_NewString("target_relay");
			if(numArgs < 3) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for target_relay!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt target_relay targetname target (spawnflags)\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			newEnt->targetname = G_NewString(arg);
			trap_Argv(3, arg, sizeof(arg));
			newEnt->target = G_NewString(arg);
			if(numArgs >= 4) {
				trap_Argv(4, arg, sizeof(arg));
				newEnt->spawnflags = atoi(arg);
			}
			G_CallSpawn(newEnt);
		} else if(!Q_stricmp(arg, "target_delay")) {
			newEnt->classname = G_NewString("target_delay");
			if(numArgs < 3) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for target_delay!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt target_delay targetname target delay (spawnflags random)\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			newEnt->targetname = G_NewString(arg);
			trap_Argv(3, arg, sizeof(arg));
			newEnt->target = G_NewString(arg);
			if(numArgs >= 4) {
				trap_Argv(5, arg, sizeof(arg));
				newEnt->spawnflags = atoi(arg);
			}
			if(numArgs >= 5) {
				trap_Argv(6, arg, sizeof(arg));
				newEnt->random = atof(arg);
			}
			G_CallSpawn(newEnt);
			trap_Argv(4, arg, sizeof(arg));
			newEnt->wait = atoi(arg);
		} else if(!Q_stricmp(arg, "target_evosuit")) {
			newEnt->classname = G_NewString("target_evosuit");
			if(numArgs < 2) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for target_evosuit!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt target_evosuit targetname\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			newEnt->targetname = G_NewString(arg);
			G_CallSpawn(newEnt);
		} else if(!Q_stricmp(arg, "target_give")) {
			newEnt->classname = G_NewString("target_give");
			if(numArgs < 3) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for target_give!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt target_give targetname items (example for items: WP_5 | WP_14)\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			newEnt->targetname = G_NewString(arg);
			trap_Argv(3, arg, sizeof(arg));
			newEnt->target = G_NewString(arg);
			G_CallSpawn(newEnt);
		} else if(!Q_stricmp(arg, "target_gravity")) {
			newEnt->classname = G_NewString("target_gravity");
			if(numArgs < 3) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for target_gravity!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt target_gravity targetname gravity (spawnflags)\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			newEnt->targetname = G_NewString(arg);
			trap_Argv(3, arg, sizeof(arg));
			newEnt->targetname2 = G_NewString(arg);
			if(numArgs >= 4) {
				trap_Argv(4, arg, sizeof(arg));
				newEnt->spawnflags = atoi(arg);
			}
			G_CallSpawn(newEnt);
		} else if(!Q_stricmp(arg, "target_kill")) {
			newEnt->classname = G_NewString("target_kill");
			if(numArgs < 2) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for target_kill!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt target_kill targetname\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			newEnt->targetname = G_NewString(arg);
			G_CallSpawn(newEnt);
		} else if(!Q_stricmp(arg, "target_print")) {
			newEnt->classname = G_NewString("target_print");
			if(numArgs < 3) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for target_print!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt target_print targetname message (spawnflags)\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			newEnt->targetname = G_NewString(arg);
			trap_Argv(3, arg, sizeof(arg));
			newEnt->message = G_NewString(arg);
			if(numArgs >= 4) {
				trap_Argv(4, arg, sizeof(arg));
				newEnt->spawnflags = atoi(arg);
			}
			G_CallSpawn(newEnt);
		} else if(!Q_stricmp(arg, "target_repair")) {
			newEnt->classname = G_NewString("target_repair");
			if(numArgs < 3) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for target_repair!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt target_repair targetname target\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			newEnt->targetname = G_NewString(arg);
			trap_Argv(3, arg, sizeof(arg));
			newEnt->target = G_NewString(arg);
			G_CallSpawn(newEnt);
		} else if(!Q_stricmp(arg, "target_shake")) {
			newEnt->classname = G_NewString("target_shake");
			if(numArgs < 4) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for target_shake!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt target_shake targetname intensity length\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			newEnt->targetname = G_NewString(arg);
			trap_Argv(3, arg, sizeof(arg));
			newEnt->distance = atof(arg);
			G_CallSpawn(newEnt);
			trap_Argv(4, arg, sizeof(arg));
			newEnt->wait = atof(arg);
		} else if(!Q_stricmp(arg, "target_teleporter")) {
			newEnt->classname = G_NewString("target_teleporter");
			if(numArgs < 3) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for target_teleporter!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt target_teleporter targetname target (spawnflags swapname)\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			newEnt->targetname = G_NewString(arg);
			trap_Argv(3, arg, sizeof(arg));
			newEnt->target = G_NewString(arg);
			if(numArgs >= 4) {
				trap_Argv(4, arg, sizeof(arg));
				newEnt->spawnflags = atoi(arg);
			}
			if(numArgs >= 5) {
				trap_Argv(5, arg, sizeof(arg));
				newEnt->swapname = G_NewString(arg);
			}
			G_CallSpawn(newEnt);
		} else if(!Q_stricmp(arg, "target_speaker")) {
			newEnt->classname = G_NewString("target_speaker");
			if(numArgs < 3) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, "print \"Insufficent number of arguments for target_speaker!\n\"");
				trap_SendServerCommand(clientNum, "print \"Usage: spawnTEnt target_speaker targetname sound (spawnflags wait random)\n\"");
				return;
			}
			trap_Argv(2, arg, sizeof(arg));
			newEnt->targetname = G_NewString(arg);
			trap_Argv(3, arg, sizeof(arg));
			if(strlen(arg) > MAX_QPATH - 1) {
				G_FreeEntity(newEnt);
				trap_SendServerCommand(clientNum, va("print \"Length of sound exceeds MAX_QPATH (%i)!\n\"", MAX_QPATH));
				return;
			}
			newEnt->count = G_SoundIndex(arg);
			if(numArgs >= 4) {
				trap_Argv(4, arg, sizeof(arg));
				newEnt->spawnflags = atoi(arg);
			}
			if(numArgs >= 5) {
				trap_Argv(5, arg, sizeof(arg));
				newEnt->wait = atof(arg);
			}
			if(numArgs >= 6 ) {
				trap_Argv(6, arg, sizeof(arg));
				newEnt->random = atof(arg);
			}
			G_CallSpawn(newEnt);
		}
	}
	if(newEnt) {
		Com_sprintf(tmp, sizeof(tmp), "%s%s", "tmp_", newEnt->classname);
		newEnt->classname = G_NewString(tmp);
		if(!Q_stricmp(newEnt->classname, "tmp_trigger_multiple") && ent->wait == -1)
			newEnt->classname = G_NewString("tmp_trigger_once");
		trap_SendServerCommand(clientNum, va("print \"Spawned entity of type %s with entity number %i.\"", newEnt->classname, newEnt->s.number));
	}
}

static void Cmd_UiHolodeck_f(gentity_t *ent) {
	trap_SendServerCommand(ent-g_entities, "ui_holodeck 1");
}

extern void trigger_transporter_delay(gentity_t *ent);
extern void trigger_transporter_think(gentity_t *ent);

/*
=================
Cmd_UiTransporterLoc_f
=================
*/
static void Cmd_UiTransporterLoc_f(gentity_t *ent) {
	int entNum, targetLoc, i, delay;
	gentity_t *locTarget = NULL;
	gentity_t *trTrigger;
	char arg[MAX_QPATH];

	if(trap_Argc() < 2) return;

	trap_Argv(1, arg, sizeof(arg));
	entNum = atoi(arg);

	trTrigger = &g_entities[entNum];

	if(!trTrigger) return;

	trap_Argv(2, arg, sizeof(arg));
	targetLoc = atoi(arg);

	trap_Argv(3, arg, sizeof(arg));
	delay = atoi(arg);
	delay *= 1000;

	for(i = 0; i < MAX_GENTITIES; i++) {
		if(!Q_stricmp(g_entities[i].classname, "target_location")) {
			if(g_entities[i].health == targetLoc && g_entities[i].health >= 1 && targetLoc >= 1) {
				locTarget = &g_entities[i];
				break;
			}
		}
	}

	if(locTarget) {
		if(locTarget->sound1to2) {
#ifndef SQL
			if ( !IsAdmin( ent ) ) {
				G_PrintfClient(ent, "Destination is a restricted location.\n");
				return;
			}
#else
			if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_BEAM) ) {
				G_PrintfClient(ent, "Destination is a restricted location.\n");
				return;
			}
#endif
		}
		trTrigger->target_ent = locTarget;
		trTrigger->count = 0;
		if(trTrigger->flags & FL_LOCKED) { // not in use
			if(delay) {
				trTrigger->think = trigger_transporter_delay;
				trTrigger->nextthink = level.time + delay;
			} else {
				trTrigger->flags ^= FL_LOCKED;
				trTrigger->think = trigger_transporter_think;
				trTrigger->nextthink = level.time + trTrigger->wait;
			}
		} else {
			trap_SendServerCommand(trTrigger->target_ent-g_entities, "Transporter is already in a Transport Cycle.\n");
			return;
		}
	}
}

/*
=================
Cmd_UiTransporterExt_f
=================
*/
static void Cmd_UiTransporterExt_f(gentity_t *ent) {
	int entNum, srvNum, delay;
	gentity_t *trTrigger;
	char arg[MAX_QPATH];

	if(!rpg_serverchange.integer) {
		trap_SendServerCommand(ent-g_entities, "print \"Serverchange is disabled.\n\"");	
		return;
	}

	if(trap_Argc() < 2) return;

	trap_Argv(1, arg, sizeof(arg));
	entNum = atoi(arg);

	trap_Argv(2, arg, sizeof(arg));
	srvNum = atoi(arg);

	trap_Argv(3, arg, sizeof(arg));
	delay = atoi(arg);
	delay *= 1000;

	trTrigger = &g_entities[entNum];

	if(!trTrigger) return;

	trTrigger->count = 1;
	trTrigger->health = srvNum;
	if(trTrigger->flags & FL_LOCKED) { // not in use
		if(delay) {
			trTrigger->think = trigger_transporter_delay;
			trTrigger->nextthink = level.time + delay;
		} else {
			trTrigger->flags ^= FL_LOCKED;
			trTrigger->think = trigger_transporter_think;
			trTrigger->nextthink = level.time + trTrigger->wait;
		}
	} else {
		trap_SendServerCommand(trTrigger->target_ent-g_entities, "Transporter is already in a Transport Cycle.\n");
		return;
	}
}

static void UI_DEBUG(gentity_t *ent) {
	char arg[MAX_TOKEN_CHARS];

	trap_Argv(1, arg, sizeof(arg));
	G_Printf(S_COLOR_GREEN "%s\n", arg);
}

#ifdef SQL
/*
=================
Cmd_SqlLogin_f
=================
*/
static void Cmd_SqlLogin_f(gentity_t *ent) {
	char uName[MAX_TOKEN_CHARS];
	char pwd[MAX_TOKEN_CHARS];
	qboolean res;
	int clientNum;
	gclient_t *client;

	if(!sql_use.integer || !ent || !ent->client) return;

	//logout if no argument and player is loged in
	if(!uName[0] && !pwd[0] && ent->client->uid) {
		ent->client->uid = 0;
		trap_SendServerCommand( ent-g_entities, va("print \"You are now logged out.\n\"") );
		G_Client_UserinfoChanged( ent-g_entities );
		return;
	}
	else if ( !uName[0] && !pwd[0] ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( ent-g_entities, va("print \"\nUsage: Allows a player to login via the User-DB\nCommand: userlogin <User Name> <User Password>\nEntering userlogin without password will log you out if you are loged in\n\" ") );
		return;
	}

	clientNum = ent->client->ps.clientNum;
	client = ent->client;

	trap_Argv(1, uName, sizeof(uName));
	trap_Argv(2, pwd, sizeof(pwd));

	res = G_Sql_UserDB_Login(uName, pwd, clientNum);

	if(res) {
		trap_SendServerCommand(clientNum, "print \"Login Successful.\n\"");
	} else {
		trap_SendServerCommand(clientNum, "print \"Login failed. Wrong password or username?\n\"");
	}
}

/*
=================
Cmd_SqlUserAdd_f
=================
*/
static void Cmd_SqlUserAdd_f(gentity_t *ent) {
	char uName[MAX_TOKEN_CHARS];
	char password[MAX_TOKEN_CHARS];
	qboolean res;

	if(!sql_use.integer || !ent || !ent->client) return;

	if(!IsAdmin(ent)) {
		return;
	}

	if(trap_Argc() < 2) {
		return;
	}

	trap_Argv(1, uName, sizeof(uName));
	trap_Argv(2, password, sizeof(password));

	res = G_Sql_UserDB_Add(uName, password);

	if(!res) {
		trap_SendServerCommand(ent->client->ps.clientNum, "print \"User could not be added.\n\"");
	} else {
		trap_SendServerCommand(ent->client->ps.clientNum, va("print \"User was added.\n\""));
	}
}

/*
=================
Cmd_SqlUserMod_f
=================
*/
static void Cmd_SqlUserMod_f(gentity_t *ent) {
	char uName[MAX_TOKEN_CHARS];
	char right[MAX_TOKEN_CHARS];
	long lright;
	int value;
	qboolean res;
	int clientNum;

	if(!sql_use.integer || !ent || !ent->client) return;

	if(!IsAdmin(ent)) {
		return;
	}	

	if(trap_Argc() < 3) {
		return;
	}

	clientNum = ent->client->ps.clientNum;

	trap_Argv(3, uName, sizeof(uName));
	value = atoi(uName);
	trap_Argv(1, uName, sizeof(uName));
	trap_Argv(2, right, sizeof(right));

	/* TODO finish me */
	res = qfalse;

	switch(res) {
	case 1:
		trap_SendServerCommand(clientNum, "print \"Seems to have worked.\n\"");
		break;
	case 2:
		trap_SendServerCommand(clientNum, "print \"User does not exist.\n\"");
		break;
	case 3:
		trap_SendServerCommand(clientNum, "print \"No entry for user found. Created one.\n\"");
		break;
	case 0:
		trap_SendServerCommand(clientNum, "print \"An SQL Error occured, check server log for more information.\n\"");
		break;
	}
}
#endif

/*
=================
Cmd_findEntitiesInRadius
=================
*/
static void Cmd_findEntitiesInRadius(gentity_t *ent) {
	char		arg[MAX_QPATH];
	int			radius;
	char		*classname = NULL;
	qboolean	all = qfalse;
	qboolean	takeDamage = qfalse;
	gentity_t	*entities[MAX_GENTITIES];
	int			numEntities;

#ifndef SQL
	if ( !IsAdmin( ent ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as an admin.\n\" ") );
		return;
	}
#else
	if ( !IsAdmin( ent ) || !G_Sql_UserDB_CheckRight(ent->client->uid, SQLF_DEBUG ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"ERROR: You are not logged in as a user with the appropiate rights.\n\" ") );
		return;
	}
#endif

	if(trap_Argc() < 3) return;

	trap_Argv(1, arg, sizeof(arg));

	if(!Q_stricmp(Q_strlwr(arg), "all"))
		all = qtrue;
	else
		classname = G_NewString(Q_strlwr(arg));

	trap_Argv(2, arg, sizeof(arg));
	radius = atoi(arg);
	if(radius < 0) return;

	trap_Argv(3, arg, sizeof(arg));
	takeDamage = (qboolean)atoi(arg);

	numEntities = G_RadiusList(ent->r.currentOrigin, radius, ent, takeDamage, entities);

	for(radius = 0; radius < numEntities; radius++) {
		if(all)
			G_PrintfClient(ent, "Entity: %i, Classname: %s", entities[radius]-g_entities, entities[radius]->classname);
		else {
			if(!Q_stricmpn(entities[radius]->classname, classname, strlen(classname)))
				G_PrintfClient(ent, "Entity: %i Classname: %s", entities[radius]-g_entities, classname);
		}
	}
}

// CCAM
extern void Cinematic_ActivateCameraMode(gentity_t *ent, gentity_t *target);
extern void Cinematic_DeactivateCameraMode(gentity_t *ent);
static void Cmd_Camtest_f(gentity_t *ent) {
	gentity_t *targ;
	char tname[256];

	if(trap_Argc() < 1) return;

	G_Printf("activate cam\n");

	if(ent->flags & FL_CCAM) return;

	trap_Argv(1, tname, sizeof(tname));

	targ = G_Find(NULL, FOFS(targetname), tname);
	if(!targ) return;

	Cinematic_ActivateCameraMode(ent, targ);
}

void Cmd_CamtestEnd_f(gentity_t *ent) {
	//Cinematic_DeactivateCameraMode(ent);
	G_LuaNumThreads();
}
// END CCAM

typedef struct rShader_s {
	char *s;
} rShader_s;
void addShaderToList(list_p list, char *shader) {
	rShader_s* s = (rShader_s *)malloc(sizeof(rShader_s));
	rShader_s* t;
	list_iter_p i;

	if(s == NULL) return;
	if(shader[0] == 0) return;
	if(list == NULL) return;

	s->s = strdup(shader);
	if(s->s == NULL) {
		free(s);
		return;
	}

	i = list_iterator(list, FRONT);
	if(i == NULL) {
		free(s->s);
		free(s);
		return;
	}

	for(t = (rShader_s *)list_next(i); t != NULL; t = (rShader_s *)list_next(i)) {
		if(!strcmp(shader, t->s)) {
			return;
		}
	}

	list_add(list, s, sizeof(rShader_s));
}

extern target_alert_Shaders_s alertShaders;
void Cmd_GeneratePrecacheFile(gentity_t *ent) {
	int i;
	char info[MAX_INFO_STRING];
	char file[MAX_QPATH];
	list_p shaders;
	list_iter_p iter;
	qboolean first = qtrue;
	fileHandle_t f;
	rShader_s* s;

	trap_GetServerinfo(info, MAX_INFO_STRING);
	Com_sprintf(file, MAX_QPATH, "maps/%s.precache", Info_ValueForKey(info, "mapname"));
	trap_FS_FOpenFile(file, &f, FS_APPEND);
	if(!f) {
		G_Printf(S_COLOR_RED "[Error] - Could not create/open 'maps/%s.precache'\n", file);
		return;
	}

	shaders = create_list();
	if(!shaders) {
		G_Printf(S_COLOR_RED "[Error] - Could not create shader list.\n");
		trap_FS_FCloseFile(f);
		return;
	}

	G_Printf("Generating precache file '%s' ...\n", file);

	for(i = 0; i < alertShaders.numShaders; i++) {
		addShaderToList(shaders, alertShaders.blueShaders[i]);
		addShaderToList(shaders, alertShaders.greenShaders[i]);
		addShaderToList(shaders, alertShaders.redShaders[i]);
		addShaderToList(shaders, alertShaders.yellowShaders[i]);
	}

	for(i = 0; i < MAX_GENTITIES; i++) {
		if(!g_entities[i].inuse) continue;

		if(g_entities[i].classname != NULL && !strcmp(g_entities[i].classname, "target_turbolift")) {
			if(g_entities[i].falsename != NULL && g_entities[i].falsename[0] != 0) {
				addShaderToList(shaders, g_entities[i].falsename);
			}
			if(g_entities[i].truename != NULL && g_entities[i].truename[0] != 0) {
				addShaderToList(shaders, g_entities[i].truename);
			}
			continue;
		}

		if(g_entities[i].targetShaderNewName != NULL && g_entities[i].targetShaderNewName[0] != 0) {
			addShaderToList(shaders, g_entities[i].targetShaderNewName);
		}
	}

	iter = list_iterator(shaders, FRONT);
	if(iter == NULL) {
		trap_FS_FCloseFile(f);
		destroy_list(shaders);
		return;
	}

	for(s = (rShader_s *)list_next(iter); s != NULL; s = (rShader_s *)list_next(iter)) {
		G_Printf("\t%s\n", s->s);
		if(first) {
			trap_FS_Write("\"", 1, f);
			trap_FS_Write(s->s, strlen(s->s), f);
			trap_FS_Write("\"", 1, f);
			first = qfalse;
		} else {
			trap_FS_Write("\n\"", 2, f);
			trap_FS_Write(s->s, strlen(s->s), f);
			trap_FS_Write("\"", 1, f);
		}
	}
	trap_FS_Write("\n\"END\"", 6, f);

	G_Printf("Done.\n");

	if(shaders != NULL) {
		destroy_list(shaders);
	}
	trap_FS_FCloseFile(f);
}

/*
=================
G_Client_Command
=================
*/
void G_Client_Command( int clientNum ) 
{
	gentity_t *ent;
	char	cmd[MAX_TOKEN_CHARS];

	//ent = g_entities + clientNum;
	ent = &g_entities[clientNum];
	if ( !ent->client ) {
		return;		// not fully in game yet
	}

	trap_Argv( 0, cmd, sizeof( cmd ) );

	if (Q_stricmp (cmd, "say") == 0) {
		Cmd_Say_f (ent, SAY_ALL, qfalse);
		return;
	}
	if (Q_stricmp (cmd, "say_team") == 0) {
		Cmd_Say_f (ent, SAY_TEAM, qfalse);
		return;
	}
	if (Q_stricmp (cmd, "say_class") == 0) {
		Cmd_Say_f (ent, SAY_CLASS, qfalse);
		return;
	}
	if (Q_stricmp (cmd, "say_area") == 0) {
		Cmd_Say_f (ent, SAY_AREA, qfalse);
		return;
	}
	if (Q_stricmp (cmd, "tell") == 0) {
		Cmd_Tell_f ( ent );
		return;
	}
	if (Q_stricmp (cmd, "score") == 0) {
		Cmd_Score_f (ent);
		return;
	}
	if (Q_stricmp (cmd, "ready") == 0)
	{
		Cmd_Ready_f(ent);
		return;
	}

#ifdef G_LUA
	if(Q_stricmp(cmd, "lua_status") == 0)
	{
		G_LuaStatus(ent);
		return;
	}
#endif

	// ignore all other commands when at intermission
	if (level.intermissiontime)
	{
		Cmd_Say_f (ent, SAY_INVAL, qtrue);
		return;
	}

	if (Q_stricmp (cmd, "give") == 0)
		Cmd_Give_f (ent);
	else if (Q_stricmp (cmd, "callvote") == 0)
		Cmd_CallVote_f (ent);
	else if (Q_stricmp (cmd, "vote") == 0)
		Cmd_Vote_f (ent);
	else if (Q_stricmp (cmd, "god") == 0)
		Cmd_God_f (ent);
	else if (Q_stricmp (cmd, "notarget") == 0)
		Cmd_Notarget_f (ent);
	else if (Q_stricmp (cmd, "noclip") == 0)
		Cmd_Noclip_f (ent);
	else if (Q_stricmp (cmd, "kill") == 0)
		Cmd_Kill_f (ent);
	else if (Q_stricmp (cmd, "levelshot") == 0)
		Cmd_LevelShot_f (ent);
	else if (Q_stricmp (cmd, "team") == 0)
		Cmd_Team_f (ent);
	else if (Q_stricmp (cmd, "class") == 0)
		Cmd_Class_f (ent);
	else if (Q_stricmp (cmd, "where") == 0)
		Cmd_Where_f (ent);
	else if (Q_stricmp (cmd, "gc") == 0)
		Cmd_GameCommand_f( ent );
	else if (Q_stricmp (cmd, "setviewpos") == 0)
		Cmd_SetViewpos_f( ent );
	//LASER
	else if (Q_stricmp (cmd, "laser") == 0)
		Cmd_Laser_f( ent );
	else if (Q_stricmp (cmd, "flashlight") == 0)
		Cmd_FlashLight_f( ent );
	else if (Q_stricmp (cmd, "cloak") == 0 && !(rpg_restrictions.integer & 1) )
		Cmd_Cloak_f( ent );
	else if (Q_stricmp (cmd, "flight") == 0 && !(rpg_restrictions.integer & 2) )
		Cmd_Flight_f( ent );
	else if (Q_stricmp (cmd, "evasuit") == 0)
		Cmd_EvoSuit_f( ent );
	else if (Q_stricmp (cmd, "forcename") == 0 )
		Cmd_ForceName_f( ent );
	else if (Q_stricmp (cmd, "forcekill") == 0 )
		Cmd_ForceKill_f( ent );
	else if (Q_stricmp (cmd, "forcekillradius") == 0 )
		Cmd_ForceKillRadius_f( ent );
	else if (Q_stricmp (cmd, "forceclass") == 0 )
		Cmd_ForceClass_f( ent );
	else if (Q_stricmp (cmd, "kicktarget") == 0 )
		Cmd_TargetKick_f(ent);
	else if (Q_stricmp (cmd, "shake") == 0 )
		Cmd_ShakeCamera_f( ent );
	else if (Q_stricmp (cmd, "drag") == 0 )
		Cmd_Drag_f(ent);
	else if (Q_stricmp (cmd, "undrag") == 0 )
		Cmd_UnDrag_f( ent );
	else if (Q_stricmp (cmd, "flushTripmines") == 0 ) //disarm_tripmines
		Cmd_disarm_f(ent);
	else if (Q_stricmp (cmd, "rank") == 0 )
		Cmd_Rank_f(ent);
	else if (Q_stricmp (cmd, "forcerank") == 0 )
		Cmd_ForceRank_f(ent);
	else if (Q_stricmp (cmd, "forcemodel") == 0 )
		Cmd_ForceModel_f(ent);
	else if (Q_stricmp (cmd, "adminlogin") == 0 )
		Cmd_AdminLogin_f(ent);
	else if (Q_stricmp (cmd, "adminList") == 0 )
		Cmd_Admins_f(ent);
	else if (Q_stricmp (cmd, "revive") == 0 )
		Cmd_Revive_f(ent);
	else if (Q_stricmp (cmd, "n00b") == 0 )
		Cmd_n00b_f(ent);
	else if (Q_stricmp (cmd, "msg") == 0 )
		Cmd_admin_message(ent);
	else if (Q_stricmp (cmd, "playMusic") == 0 )
		Cmd_PlayMusic_f(ent);
	else if (Q_stricmp (cmd, "stopMusic") == 0 )
		Cmd_StopMusic_f(ent);
	else if (Q_stricmp (cmd, "playSound") == 0 )
		Cmd_PlaySound_f(ent);
	else if (Q_stricmp(cmd, "equip") == 0 )
		Cmd_Bolton_f(ent);
	else if (Q_stricmp(cmd, "useEnt") == 0 )
		Cmd_UseEnt_f(ent);
	else if (Q_stricmp(cmd, "entList") == 0 )
		Cmd_EntList_f(ent);
	else if (Q_stricmp(cmd, "beamToLocation") == 0 )
		Cmd_BeamToLoc_f(ent);
	else if (Q_stricmp(cmd, "beamToPlayer") == 0 )
		Cmd_BeamToPlayer_f(ent);
	else if (Q_stricmp(cmd, "forcePlayer") == 0 )
		Cmd_ForcePlayer_f(ent);
	else if (Q_stricmp(cmd, "doEmote") == 0 )
		Cmd_DoEmote_f(ent);
	else if (Q_stricmp(cmd, "flushEmote") == 0 )
		Cmd_EndEmote_f(ent);
	else if (Q_stricmp(cmd, "fxGun") == 0 )
		Cmd_fxGun_f(ent);
	else if (Q_stricmp(cmd, "flushFX") == 0 ) 
		Cmd_flushFX_f(ent);
	else if (Q_stricmp(cmd, "spawnChar") == 0 )
		Cmd_spawnChar_f( ent );
	else if (Q_stricmp(cmd, "flushChars") == 0 )
		Cmd_fluchChars_f(ent);
	else if (Q_stricmp( cmd, "kick2") == 0 )
		Cmd_Kick2_f(ent);
	else if (Q_stricmp( cmd, "clampInfo") == 0 )
		Cmd_ClampInfo_f(ent);
	else if (Q_stricmp( cmd, "deck" ) == 0 )
		Cmd_Turbolift_f(ent);
	else if (Q_stricmp( cmd, "me" ) == 0 )
		Cmd_MeAction_f(ent);
	else if (Q_stricmp( cmd, "meLocal" ) == 0 )
		Cmd_MeActionLocal_f(ent);
	else if (Q_stricmp( cmd, "mapsList" ) == 0 )
		Cmd_MapsList_f(ent);
	else if (Q_stricmp (cmd, "drop") == 0)  // RPG-X | Marcin | 03/12/2008
		Cmd_Drop_f(ent);
	else if (Q_stricmp (cmd, "flushDropped") == 0) // RPG-X | Marcin | 04/12/2008
		Cmd_flushDropped_f(ent);
	else if (Q_stricmp (cmd, "lock") == 0) //RPG-X | GSIO01 | 08/05/2009
		Cmd_lockDoor_f(ent);
	else if (Q_stricmp (cmd, "ffColor") == 0) //RPG-X | GSIO01 | 09/05/2009
		Cmd_ffColor_f(ent);
	else if (Q_stricmp (cmd, "remodulate") == 0) //RPG-X | GSIO01 | 10/05/2009
		Cmd_remodulate_f(ent);
	else if (Q_stricmp (cmd, "unlockAll") == 0)
		Cmd_unlockAll_f(ent);
	else if (Q_stricmp (cmd, "respawn") == 0)
		Cmd_Respawn_f(ent);
	else if (Q_stricmp (cmd, "lockAll") == 0)
		Cmd_lockAll_f(ent);
	else if (Q_stricmp (cmd, "changeFreq") == 0)
		Cmd_changeFreq(ent);
	else if (Q_stricmp(cmd, "alert") == 0)
		Cmd_alert_f(ent);
	else if (Q_stricmp(cmd, "zonelist") == 0)
		Cmd_zonelist_f(ent);
	else if (Q_stricmp(cmd, "selfdestruct") == 0)
		Cmd_selfdestruct_f(ent);
	else if (Q_stricmp(cmd, "shipdamage") == 0)
		Cmd_shipdamage_f(ent);
	else if (Q_stricmp(cmd, "shiphealth") == 0)
		Cmd_shiphealth_f(ent);
	else if (Q_stricmp(cmd, "reloadtorpedos") == 0)
		Cmd_reloadtorpedos_f(ent);
	else if (Q_stricmp(cmd, "torpedolist") == 0)
		Cmd_torpedolist_f(ent);
	else if (Q_stricmp(cmd, "msg2") == 0)
		Cmd_admin_centerprint_f(ent);
	else if (Q_stricmp(cmd, "forcevote") == 0)
		Cmd_ForceVote_f(ent);
	else if (Q_stricmp(cmd, "listSPs") == 0)
		Cmd_listSPs(ent);
	else if (Q_stricmp(cmd, "getEntInfo") == 0)
		Cmd_getEntInfo_f(ent);
	else if (Q_stricmp(cmd, "getOrigin") == 0)
		Cmd_getOrigin_f(ent);
	else if (Q_stricmp(cmd, "getEntByTargetname") == 0)
		Cmd_getEntByTargetname_f(ent);
	else if (Q_stricmp(cmd, "getEntByTarget") == 0)
		Cmd_getEntByTarget_f(ent);
	else if (Q_stricmp(cmd, "getEntByBmodel") == 0)
		Cmd_getEntByBmodel_f(ent);
	else if (Q_stricmp(cmd, "setOrigin") == 0)
		Cmd_setOrigin(ent);
	else if (Q_stricmp(cmd, "getBrushEntCount") == 0)
		Cmd_getBrushEntCount_f(ent);
	else if (Q_stricmp(cmd, "findEntitiesInRadius") == 0)
		Cmd_findEntitiesInRadius(ent);
	else if (Q_stricmp(cmd, "spawnTEnt") == 0)
		Cmd_spawnTentity_f(ent);
	else if (Q_stricmp(cmd, "flushTEnts") == 0)
		Cmd_flushTentities_f(ent);
	else if (Q_stricmp(cmd, "ui_transporterLoc") == 0)
		Cmd_UiTransporterLoc_f(ent);
	else if (Q_stricmp(cmd, "ui_transporterExt") == 0)
		Cmd_UiTransporterExt_f(ent);
	else if (Q_stricmp(cmd, "ui_holodeck") == 0)
		Cmd_UiHolodeck_f(ent);
	else if (Q_stricmp(cmd, "ui_debug") == 0)
		UI_DEBUG(ent);
#ifdef SQL
	else if (Q_stricmp(cmd, "userlogin") == 0)
		Cmd_SqlLogin_f(ent);
	else if (Q_stricmp(cmd, "userAdd") == 0)
		Cmd_SqlUserAdd_f(ent);
	else if (Q_stricmp(cmd, "userMod") == 0)
		Cmd_SqlUserMod_f(ent);
#endif
#ifdef OLDSQL // SQL
	else if (Q_stricmp(cmd, "sql_setup") == 0)
		Cmd_SqlSetup_f(ent);
	else if (Q_stricmp(cmd, "userAdd") == 0)
		Cmd_SqlUserAdd_f(ent);
	else if (Q_stricmp(cmd, "userDel") == 0)
		Cmd_SqlUserDel_f(ent);
#endif
	// CCAM
	else if (Q_stricmp(cmd, "camtest") == 0)
		Cmd_Camtest_f(ent);
	else if (Q_stricmp(cmd, "camtestend") == 0)
		Cmd_CamtestEnd_f(ent);
	// END CCAM
	else if (Q_stricmp (cmd, "generatePrecacheFile") == 0) {
		Cmd_GeneratePrecacheFile(ent);
	}
	else if (Q_strncmp (cmd, "\n", 1) == 0 || Q_strncmp (cmd, " ", 1) == 0 || Q_strncmp (cmd, "\0", 1) == 0) // sorry
		(void)(0);
	else
		trap_SendServerCommand( clientNum, va("print \"unknown cmd %s\n\"", cmd ) );
}

//--------
