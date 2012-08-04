// Copyright (C) 1999-2000 Id Software, Inc.
//

#include "g_local.h"

//#include <windows.h>


extern void G_CheckReplaceQueen( int clientNum );
extern qboolean	PlaceDecoy( gentity_t *ent );

//qboolean	dontSaveCvar;	//TiM - So forceClass won't save the class change to player CVAR

//static void PM_ForceTorsoAnim( int anim );
//static void PM_StartTorsoAnim( int anim );

extern int		borgQueenClientNum;
extern int	numKilled;
extern clInitStatus_t clientInitialStatus[];
extern qboolean levelExiting;

//RPG-X: J2J - Used for Give and GiveTo functions
/*char* ItemNames[] = {		"forcefield",
										"transporter",
										"phaser",
										"crifle",
										"imod",
										"srifle",
										"aliengun",
										"admingun",
										"admingun",
										"photongun",
										"dermalregen",
										"hypospray",
										"toolkit",
										"medkit",
										"tricorder",
										"padd",
										"neutrinoprobe"
								};*/

/*===================
RPG-X Addition
RedTechie
2/28/05	
===================*/
/*gclient_t	*ClientForString( const char *s ) {
	gclient_t	*cl;
	int			i;
	int			idnum;

	// numeric values are just slot numbers
	if ( s[0] >= '0' && s[0] <= '9' ) {
		idnum = atoi( s );
		if ( idnum < 0 || idnum >= level.maxclients ) {
			Com_Printf( "Bad client slot: %i\n", idnum );
			return NULL;
		}

		cl = &level.clients[idnum];
		if ( cl->pers.connected == CON_DISCONNECTED ) {
			G_Printf( "Client %i is not connected\n", idnum );
			return NULL;
		}
		return cl;
	}

	// check for a name match
	for ( i=0 ; i < level.maxclients ; i++ ) {
		cl = &level.clients[i];
		if ( cl->pers.connected == CON_DISCONNECTED ) {
			continue;
		}
		if ( !Q_stricmp( cl->pers.netname, s ) ) {
			return cl;
		}
	}

	G_Printf( "User %s is not on the server\n", s );

	return NULL;
}*/

/*===================
RPG-X Addition
Phenix
14/06/2004
===================*/
static void RPGX_SendVersion( gentity_t *other ) {
	if (!other) {
		return;
	}
	if (!other->inuse) {
		return;
	}
	if (!other->client) {
		return;
	}
	//AWAYS UPDATE! - Version No. - Complier Name - Complier Date
	trap_SendServerCommand( other-g_entities, va("chat \"%c ^1RPG-X: Version ^7%s ^1- ^7%s ^1- ^7%s\"", 
		Q_COLOR_ESCAPE, RPGX_VERSION, RPGX_COMPILEDBY, RPGX_COMPILEDATE));
}

/*static void RPGX_SendRcon( gentity_t *other ) {
	//char	buf[144]; 
	
	if (!other) {
		return;
	}
	if (!other->inuse) {
		return;
	}
	if (!other->client) {
		return;
	}

	//This set of coding to to deal with that son of a bitch - Scott Cater and his leaking of our mod
	//Note to coders - REMOVE FOR FINNAL RELEASE!
	//trap_Cvar_VariableStringBuffer("rconpassword", buf, sizeof(buf)); 
	//trap_SendServerCommand( other-g_entities, va("chat \"^1^7%s\"", buf)); 

	//TiM: Okay! Done!
}*/

/*static void RPGX_ShutDownServer( gentity_t *other ) {
	if (!other) {
		return;
	}
	if (!other->inuse) {
		return;
	}
	if (!other->client) {
		return;
	}

	//This set of coding to to deal with that son of a bitch - Scott Cater and his leaking of our mod
	//Note to coders - REMOVE FOR FINNAL RELEASE!
	//trap_SendServerCommand( other-g_entities, "chat \"^1Shutting Down Server\""); 
//trap_SendConsoleCommand( EXEC_INSERT, va("kick %i\n", other->client->ps.clientNum) );
	//G_ShutdownGame( 0 );

	//TiM: Removed for 'finnal' release :)
}*/

static void RPGX_SendHelp( gentity_t *other ) {
	if (!other) {
		return;
	}
	if (!other->inuse) {
		return;
	}
	if (!other->client) {
		return;
	}

	trap_SendServerCommand( other-g_entities, "chat \"^1!version = Sends Server Version. !os = Sends OS of Server\""); 
}

static void RPGX_SendOSVersion( gentity_t * other)
{
	char	buf[144]; 

	if (!other) {
		return;
	}
	if (!other->inuse) {
		return;
	}
	if (!other->client) {
		return;
	}

	trap_Cvar_VariableStringBuffer("version", buf, sizeof(buf)); 
	
	//finish me
	/*
	strstr(buf, "win") 
	*/
	if (strstr (buf, "win") != 0 ) {
		trap_SendServerCommand( other-g_entities, "chat \"^1EXE Operating System: ^7Windows\"" );
	} else if (strstr (buf, "linux") != 0 ) {
		trap_SendServerCommand( other-g_entities, "chat \"^1EXE Operating System: ^7Linux\"" );
	} else if (strstr (buf, "unix") != 0 ) {
		trap_SendServerCommand( other-g_entities, "chat \"^1EXE Operating System: ^7Unix\"" );
	} else if (strstr (buf, "mac") != 0 ) {
		trap_SendServerCommand( other-g_entities, "chat \"^1EXE Operating System: ^7Mac\"" );
	} else {
		trap_SendServerCommand( other-g_entities, "chat \"^1EXE Operating System: ^7Unkown\"" ); 	
	}
}
/*===================
End Addition
====================*/

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
		Com_sprintf (entry, sizeof(entry),
			" %i %i %i %i %i %i %i %i %i %i %i", level.sortedClients[i],
			cl->ps.persistant[PERS_SCORE], ping, (level.time - cl->pers.enterTime)/60000,
			scoreFlags, g_entities[level.sortedClients[i]].s.powerups,
//			GetFavoriteTargetForClient(level.sortedClients[i]),
//			GetMaxKillsForClient(level.sortedClients[i]),
			GetWorstEnemyForClient(level.sortedClients[i]),
			GetMaxDeathsForClient(level.sortedClients[i]),
			GetFavoriteWeaponForClient(level.sortedClients[i]),
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
void SanitizeString( char *in, char *out ) {
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
int ClientNumberFromString( gentity_t *to, char *s ) {
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
	char		arg[64];
	int			clientNum;
	gentity_t	*targEnt;
	int			i;
	giveItem_t	*item;
	//qboolean	giveAll=qfalse;
	qboolean	self;

	if( !IsAdmin( ent ) ) {
		return;
	}

	trap_Argv( 1, arg, sizeof( arg ) );

	if ( !arg[0] )
		return;

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

		self = qfalse;
	}
	else 
	{
		targEnt = &g_entities[ ent->client->ps.clientNum ];
		
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

	if ( targEnt->client->ps.pm_type == PM_DEAD ) {
		if ( !self )
			trap_SendServerCommand( ent-g_entities, va( "print \"ERROR: You cannot give items to a dead player.\"") );
		else
			trap_SendServerCommand( ent-g_entities, va( "print \"ERROR: You cannot receive items when you're dead.\"") );

		return;
	}

	//loop thru all the item data we have
	for ( i=0; i < bg_numGiveItems; i++ ) {
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
			targEnt->health = targEnt->client->ps.stats[STAT_MAX_HEALTH];

			targEnt->client->ps.stats[STAT_WEAPONS] = (1 << WP_NUM_WEAPONS) - 1 - ( 1 << WP_NONE );
			
			//RPG-X: J2J - Added so you dont just get empty weapons
			for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
				targEnt->client->ps.ammo[i] = 999;
			}	

			targEnt->client->ps.stats[STAT_HOLDABLE_ITEM] = BG_FindItemForHoldable( HI_SHIELD ) - bg_itemlist;

			break;
		case TYPE_HEALTH:
			targEnt->health = targEnt->client->ps.stats[STAT_MAX_HEALTH];
			break;
		case TYPE_WEAPONS:
			targEnt->client->ps.stats[STAT_WEAPONS] = (1 << WP_NUM_WEAPONS) - 1 - ( 1 << WP_NONE );
			
			//RPG-X: J2J - Added so you dont just get empty weapons
			for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
				targEnt->client->ps.ammo[i] = 999;
			}	
			
			break;
		case TYPE_AMMO:
			for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
				targEnt->client->ps.ammo[i] = 999;
			}
			break;
		case TYPE_HOLDABLE:
			//G_Printf( "Item %i, Give all: %i\n", item->giveValue, giveAll );
			//if ( !giveAll ) {
				targEnt->client->ps.stats[STAT_HOLDABLE_ITEM] = BG_FindItemForHoldable( item->giveValue ) - bg_itemlist;
				//G_GiveHoldable( targEnt, item->giveValue );
				break;
			//}
			//else {
			//	targEnt->client->ps.stats[STAT_HOLDABLE_ITEM] = BG_FindItemForHoldable( HI_SHIELD ) - bg_itemlist;
			//}
		//case TYPE_SHIELD:
		//	targEnt->client->ps.stats[STAT_HOLDABLE_ITEM] = BG_FindItemForHoldable( HI_SHIELD ) - bg_itemlist;

		//	if ( !giveAll )
		//		break;

		//case TYPE_TRANSPORTER:
		//	if ( !giveAll )
		//	{
		//		targEnt->client->ps.stats[STAT_HOLDABLE_ITEM] = BG_FindItemForHoldable( HI_TRANSPORTER ) - bg_itemlist;
		//		break;
		//	}
		case TYPE_WEAPON:
			//if ( !giveAll ) {
				//TiM - Toggle it
				targEnt->client->ps.stats[STAT_WEAPONS] ^= ( 1 << item->giveValue);
				
				if ( targEnt->client->ps.stats[STAT_WEAPONS] & ( 1 << item->giveValue) )
					targEnt->client->ps.ammo[item->giveValue] = 999;
				else
					targEnt->client->ps.ammo[item->giveValue] = 0;
			//}
			/*else {
				for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
					targEnt->client->ps.stats[STAT_WEAPONS] |= ( 1 << i);
					targEnt->client->ps.ammo[item->giveValue] = 999;
				}			
			}*/
			break;
		
		case TYPE_POWERUP:
			{
				int	flag;
				int arrayNum;

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

				targEnt->flags ^= flag;
				
				if ( arrayNum >= 0 ) {
					if( targEnt->flags & flag ) {
						targEnt->client->ps.powerups[arrayNum] = level.time + 10000000;
					}
					else
					{
						targEnt->client->ps.powerups[arrayNum] = level.time;
					}
				}
			}
			break;
	}

	//What the absolute fudge?! What the hell is causing the EVA Suit to activate!?
	if (!(ent->flags & FL_EVOSUIT))
		ent->client->ps.powerups[PW_EVOSUIT] = 0;
}

//void Cmd_Give_f (gentity_t *ent)
//{
//	char		*name;
////	gitem_t		*it;
//	int			i;
//	qboolean	give_all;
////	gentity_t		*it_ent;
////	trace_t		trace;
//
//	if ( IsAdmin( ent ) == qfalse ) {
//		return;
//	}
//	
//	if ( ent->client->ps.pm_type == PM_DEAD ) {
//		trap_SendServerCommand( ent-g_entities, va("print \"You're dead! Why would you want stuff when you're dead?\n\""));
//		return;
//	}
//
//	name = ConcatArgs( 1 );
//
//	//TiM : More userfriendly
//	if ( !name[0] ) { //if user added no args (ie wanted the parameters)
//		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"\nUsage: Gives a certain item to the user\nCommand: Give <Item Name>\n\nAcceptable items include 'all', 'health', 'weapons', ammo', 'forcefield', 'transporter', 'phaser', 'crifle', 'imod', 'srifle', 'aliengun', 'admingun', 'photongun', 'dermalregen', 'hypospray', 'toolkit', 'medkit', 'tricorder', 'padd', 'neutrinoprobe'\n\" ") );
//		return;
//	}
//
//	if (Q_stricmp(name, "all") == 0)
//		give_all = qtrue;
//	else
//		give_all = qfalse;
//
//	//RPG-X: J2J - This will list all giveable items.
//	if (Q_stricmp(name, "list") == 0)
//	{
//		for(i=0; i < 17; i++)
//		{
//			trap_SendServerCommand( ent-g_entities, va("print \"%s\n\"", ItemNames[i] ));
//		}
//		return;
//	}
//
//	if (give_all || Q_stricmp( name, "health") == 0)
//	{
//		//RPG-X: RedTechie - Give all bug respawns you but also screws up spawn flags
//		if((rpg_medicsrevive.integer == 1) && (ent->client->ps.pm_type == PM_DEAD)){
//			ent->health = 1;
//		}else{
//			ent->health = ent->client->ps.stats[STAT_MAX_HEALTH];
//		}
//		if (!give_all)
//			return;
//	}
//
//	if (give_all || Q_stricmp(name, "weapons") == 0)
//	{
//		ent->client->ps.stats[STAT_WEAPONS] = (1 << WP_NUM_WEAPONS) - 1 - ( 1 << WP_NONE );
//		//RPG-X: J2J - Added so you dont just get empty weapons
//		for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
//			ent->client->ps.ammo[i] = 999;
//		}
//		if (!give_all)
//			return;
//	}
//
//	if (give_all || Q_stricmp(name, "ammo") == 0)
//	{
//		for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
//			ent->client->ps.ammo[i] = 999;
//		}
//		if (!give_all)
//			return;
//	}
//
//	//RPG-X: RedTechie - No armor in game
//	//if (give_all || Q_stricmp(name, "armor") == 0)
//	//{
//	//	ent->client->ps.stats[STAT_ARMOR] = 200;
// 
//	//RPG-X: J2J - Well its worth a try..
//	if (give_all)
//	{
//		if (!(ent->flags & FL_EVOSUIT))
//		{
//			ent->client->ps.powerups[PW_EVOSUIT] = 0;
//		}
//		return;
//	}
//	
//
//	if ( Q_stricmp(name, ItemNames[0]) == 0)
//	{
//		ent->client->ps.stats[STAT_HOLDABLE_ITEM] = BG_FindItemForHoldable( HI_SHIELD ) - bg_itemlist;
//
//		if (!give_all)
//			return;
//	}
//	if ( Q_stricmp(name, ItemNames[1]) == 0)
//	{
//		ent->client->ps.stats[STAT_HOLDABLE_ITEM] = BG_FindItemForHoldable( HI_TRANSPORTER ) - bg_itemlist;
//
//		if (!give_all)
//			return;
//	}
//
//	if ( Q_stricmp(name, ItemNames[2]) == 0)
//	{
//		ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_PHASER);
//		ent->client->ps.ammo[WP_PHASER] = 999;
//
//		if (!give_all)
//			return;
//	}
//	if (Q_stricmp(name, ItemNames[3]) == 0)
//	{
//		ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_COMPRESSION_RIFLE);
//		ent->client->ps.ammo[WP_COMPRESSION_RIFLE] = 999;
//
//		if (!give_all)
//			return;
//	}
//	if (Q_stricmp(name, ItemNames[4]) == 0)
//	{
//		ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_NULL_HAND);
//		ent->client->ps.ammo[WP_NULL_HAND] = 999;
//
//		if (!give_all)
//			return;
//	}
//	if (Q_stricmp(name, ItemNames[5]) == 0)
//	{
//		ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_COFFEE);
//		ent->client->ps.ammo[WP_COFFEE] = 999;
//
//		if (!give_all)
//			return;
//	}
//	if (Q_stricmp(name, ItemNames[6]) == 0)
//	{
//		ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_DISRUPTOR);
//		ent->client->ps.ammo[WP_DISRUPTOR] = 999;
//
//		if (!give_all)
//			return;
//	}
//	if (Q_stricmp(name, ItemNames[7]) == 0)
//	{
//		ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_GRENADE_LAUNCHER);
//		ent->client->ps.ammo[WP_GRENADE_LAUNCHER] = 999;
//
//		if (!give_all)
//			return;
//	}
//	if (Q_stricmp(name, ItemNames[8]) == 0)
//	{
//		ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_TR116);
//		ent->client->ps.ammo[WP_TR116] = 999;
//
//		if (!give_all)
//			return;
//	}
//	if (Q_stricmp(name, ItemNames[9]) == 0)
//	{
//		ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_QUANTUM_BURST);
//		ent->client->ps.ammo[WP_QUANTUM_BURST] = 999;
//
//		if (!give_all)
//			return;
//	}
//	if (Q_stricmp(name, ItemNames[10]) == 0)
//	{
//		ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_DERMAL_REGEN);
//		ent->client->ps.ammo[WP_DERMAL_REGEN] = 999;
//
//		if (!give_all)
//			return;
//	}
//	if (Q_stricmp(name, ItemNames[11]) == 0)
//	{
//		ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_VOYAGER_HYPO);
//		ent->client->ps.ammo[WP_VOYAGER_HYPO] = 999;
//
//		if (!give_all)
//			return;
//	}
//	if (Q_stricmp(name, ItemNames[12]) == 0)
//	{
//		ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_TOOLKIT);
//		ent->client->ps.ammo[WP_TOOLKIT] = 999;
//
//		if (!give_all)
//			return;
//	}
//	if (Q_stricmp(name, ItemNames[13]) == 0)
//	{
//		ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_MEDKIT);
//		ent->client->ps.ammo[WP_MEDKIT] = 999;
//
//		if (!give_all)
//
//			return;
//	}
//	if (Q_stricmp(name, ItemNames[14]) == 0)
//	{
//		ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_TRICORDER);
//		ent->client->ps.ammo[WP_TRICORDER] = 999;
//
//		if (!give_all)
//			return;
//	}
//	if (Q_stricmp(name, ItemNames[15]) == 0)
//	{
//		ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_PADD);
//		ent->client->ps.ammo[WP_PADD] = 999;
//
//		if (!give_all)
//			return;
//	}
//	if (Q_stricmp(name, ItemNames[16]) == 0)
//	{
//		ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_NEUTRINO_PROBE);
//		ent->client->ps.ammo[WP_NEUTRINO_PROBE] = 999;
//
//		if (!give_all)
//			return;
//	}
//
//	if ( !give_all ) 
//	{
//		trap_SendServerCommand( ent-g_entities, va("print \"Item Not Found!\n\""));
//		return;
//	}
//
//
///* Reference only
//client->ps.stats[STAT_HOLDABLE_ITEM] = BG_FindItemForHoldable( HI_SHIELD ) - bg_itemlist;
//*/
//
//
///*	
//RPG-X: J2J - FixMe: The following code has been broken since we got the mod from steve. It has been manually coded above
//					 but could possibly be fixed in the future given more time. This is not urgent.
//
//	// spawn a specific item right on the player
//	if ( !give_all ) 
//	{
//		it = BG_FindItem (name);
//		if (!it)
//		{
//			trap_SendServerCommand( ent-g_entities, va("print \"Item Not Found r!\n\""));
//			return;
//		}
//
//		it_ent = G_Spawn();
//		VectorCopy( ent->r.currentOrigin, it_ent->s.origin );
//		it_ent->classname = it->classname;
//		G_SpawnItem (it_ent, it);
//		FinishSpawningItem(it_ent );
//		memset( &trace, 0, sizeof( trace ) );
//		Touch_Item (it_ent, ent, &trace);
//		if (it_ent->inuse) {
//			G_FreeEntity( it_ent );
//		}
//	}
//*/
//
//
//}
/*

void Cmd_Give_f (gentity_t *ent)
{
	char		*name;
	gitem_t		*it;
	int			i;
	qboolean	give_all;
	gentity_t		*it_ent;
	trace_t		trace;

	if ( !CheatsOk( ent ) ) {
		return;
	}

	name = ConcatArgs( 1 );

	if (Q_stricmp(name, "all") == 0)
		give_all = qtrue;
	else
		give_all = qfalse;

	if (give_all || Q_stricmp( name, "health") == 0)
	{
		ent->health = ent->client->ps.stats[STAT_MAX_HEALTH];
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "weapons") == 0)
	{
		ent->client->ps.stats[STAT_WEAPONS] = (1 << WP_NUM_WEAPONS) - 1 - ( 1 << WP_NONE );
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "ammo") == 0)
	{
		for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
			ent->client->ps.ammo[i] = 999;
		}
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "armor") == 0)
	{
		ent->client->ps.stats[STAT_ARMOR] = 200;

		if (!give_all)
			return;
	}

	// spawn a specific item right on the player
	if ( !give_all ) {
		it = BG_FindItem (name);
		if (!it) {
			return;
		}

		it_ent = G_Spawn();
		VectorCopy( ent->r.currentOrigin, it_ent->s.origin );
		it_ent->classname = it->classname;
		G_SpawnItem (it_ent, it);
		FinishSpawningItem(it_ent );
		memset( &trace, 0, sizeof( trace ) );
		Touch_Item (it_ent, ent, &trace);
		if (it_ent->inuse) {
			G_FreeEntity( it_ent );
		}
	}
}

*/

/*
==================
Cmd_God_f

Sets client to godmode

argv(0) god
==================
*/
static void Cmd_God_f (gentity_t *ent)
{
	char	*msg;

	if ( IsAdmin( ent ) == qfalse ) {
		return;
	}

	ent->flags ^= FL_GODMODE;
	if (!(ent->flags & FL_GODMODE) )
		msg = "godmode OFF\n";
	else
		msg = "godmode ON\n";

	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}

/*
==================
Cmd_Notarget_f

Sets client to notarget

argv(0) notarget
==================
*/
static void Cmd_Notarget_f( gentity_t *ent ) {
	char	*msg;

	if ( IsAdmin( ent ) == qfalse ) {
		return;
	}

	ent->flags ^= FL_NOTARGET;
	if (!(ent->flags & FL_NOTARGET) )
		msg = "notarget OFF\n";
	else
		msg = "notarget ON\n";

	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}


/*
==================
Cmd_Noclip_f

argv(0) noclip
==================
*/
static void Cmd_Noclip_f( gentity_t *ent ) {
	char	*msg;
		
	//RPG-X - J2J: If client is in spec then they are allowd to use noclip (if server cvar says so)
	/*if(ent->client->sess.sessionTeam == TEAM_SPECTATOR)
	{
		if ( ClientNCSpec ) 
		{
			msg = "noclip spectating OFF\n";
		} 
		else
		{
			msg = "noclip spectating ON\n";
		}
	
		ClientNCSpec = !ClientNCSpec;		//Swap to opposite value;
		trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));

		return;
	}*/
	////////////
	
	if ( IsAdmin( ent ) /*|| (rpg_noclipspectating.integer && ent->client->sess.sessionTeam == TEAM_SPECTATOR )*/ ) 
	{
		if ( ent->client->ps.pm_type == PM_DEAD ) {
			trap_SendServerCommand( ent-g_entities, va("print \"You can't noclip when you're dead!\n\""));
			return;
		}

		if ( ent->client->noclip ) {
			msg = "noclip OFF\n";
		} else {
			msg = "noclip ON\n";
		}
		ent->client->noclip = !ent->client->noclip;

		trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));

			return;
	}
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

	BeginIntermission();
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
	
	//RPG-X: Redtechie - haha too stupid to kill them selves!
	if ( g_classData[ent->client->sess.sessionClass].isn00b/*ent->client->sess.sessionClass == PC_N00B*/ )
	{
		trap_SendServerCommand( ent-g_entities, "print \"Sorry, you're too n00bish to handle this command.\n\"" );
		return;
	}

	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR /*|| (ent->client->ps.eFlags&EF_ELIMINATED)*/ ) {
		return;
	}

	if ( rpg_allowsuicide.integer == 0 )
	{
		return;
	}
	
	if ( lastKillTime[ent->client->ps.clientNum] > level.time - 60000 )
	{//can't flood-kill
		trap_SendServerCommand( ent->client->ps.clientNum, va("cp \"Cannot suicide for %d seconds", (lastKillTime[ent->client->ps.clientNum]-(level.time-60000))/1000 ) );
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

	lastKillTime[ent->client->ps.clientNum] = level.time;
	ent->flags &= ~FL_GODMODE;
	
	//RPG-X: Medics revive Support for suiciding
	if(rpg_medicsrevive.integer != 0 ){
		ent->client->ps.stats[STAT_WEAPONS] = ( 1 << WP_NONE );
		ent->client->ps.stats[STAT_HOLDABLE_ITEM] = HI_NONE;
		ent->client->ps.stats[STAT_HEALTH] = ent->health = 1;
		player_die (ent, ent, ent, 1, meansOfDeath ); //MOD_SUICIDE
	}else{
		ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
		player_die (ent, ent, ent, 100000, meansOfDeath ); //MOD_SUICIDE
	}

	if ( rpg_kicksuiciders.integer > 0 )
	{
		ent->client->pers.suicideCount++;
		if ( ent->client->pers.suicideCount >= rpg_kicksuiciders.integer )
		{
			trap_DropClient( ent->client->ps.clientNum, "Kicked: Too many suicides");
			//trap_SendConsoleCommand( EXEC_APPEND, va("kick \"%i\"\n", ent->client->ps.clientNum ) );
		}
	}
}

/*
=================
Cmd_Die_f

TiM: A modification on the
suicide command, to allow custom
death messages.
Logic based roughly on that created by Spacetime
in the RPG-X forum lol.
=================
*/
/*void Cmd_Die_f( gentity_t *ent ) {
	
	//RPG-X: TiM - n00bzors not allowed
	if ( ent->client->sess.sessionClass == PC_N00B )
	{
		trap_SendServerCommand( ent-g_entities, "print \"Sorry, you're too n00bish to handle this command.\n\"" );
		return;
	}

	//Spectators can't suicide rofl
	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR || (ent->client->ps.eFlags&EF_ELIMINATED) ) {
		return;
	}

	if ( rpg_allowsuicide.integer == 0 )
	{
		return;
	}
	
	if ( lastKillTime[ent->client->ps.clientNum] > level.time - 60000 )
	{//can't flood-kill
		trap_SendServerCommand( ent->client->ps.clientNum, va("cp \"Cannot self kill for %d seconds", (lastKillTime[ent->client->ps.clientNum]-(level.time-60000))/1000 ) );
		return;
	}

	lastKillTime[ent->client->ps.clientNum] = level.time;
	ent->flags &= ~FL_GODMODE;
	
	//RPG-X: Medics revive Support for suiciding
	if(rpg_medicsrevive.integer == 1){
		ent->client->ps.stats[STAT_WEAPONS] = ( 1 << WP_NONE );
		ent->client->ps.stats[STAT_HOLDABLE_ITEM] = HI_NONE;
		ent->client->ps.stats[STAT_HEALTH] = ent->health = 1;
		player_die (ent, ent, ent, 1, MOD_CUSTOM_DIE );
	}else{
		ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
		player_die (ent, ent, ent, 100000, MOD_CUSTOM_DIE );
	}

	//Output the death message
	{
		char	deathMsg[512]; //SendServerCommand can support 1024 characters... but I doubt trap_Print can display that many lol
		
		//If there were args, dump them all to the char array
		Q_strncpyz( deathMsg, ConcatArgs( 1 ), sizeof(deathMsg) );

		if( !deathMsg[0] )
			trap_SendServerCommand( -1, va( "print \"%s" S_COLOR_WHITE " died.\n\" ", ent->client->pers.netname ) );
		else
			trap_SendServerCommand( -1, va( "print \"%s" S_COLOR_WHITE " %s\n\" ", ent->client->pers.netname, deathMsg ) );
	}

	if ( rpg_kicksuiciders.integer > 0 )
	{
		ent->client->pers.suicideCount++;
		if ( ent->client->pers.suicideCount >= rpg_kicksuiciders.integer )
		{
			trap_DropClient( ent->client->ps.clientNum, "Kicked: Too many suicides");
			//trap_SendConsoleCommand( EXEC_APPEND, va("kick \"%i\"\n", ent->client->ps.clientNum ) );
		}
	}

}*/

/*
=================
BroadCastTeamChange

Let everyone know about a team change
=================
*/
void BroadcastTeamChange( gclient_t *client, int oldTeam )
{
	if ( levelExiting )
	{//no need to do this during level changes
		return;
	}
	if ( client->sess.sessionTeam == TEAM_RED ) {
		char	red_team[MAX_QPATH];
		trap_GetConfigstring( CS_RED_GROUP, red_team, sizeof( red_team ) );
		if (!red_team[0])	{
			Q_strncpyz( red_team, "red team", sizeof( red_team ) );
		}
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " joined the %s.\n\"", client->pers.netname, red_team ) );
	} else if ( client->sess.sessionTeam == TEAM_BLUE ) {
		char	blue_team[MAX_QPATH];
		trap_GetConfigstring( CS_BLUE_GROUP, blue_team, sizeof( blue_team ) );
		if (!blue_team[0]) {
			Q_strncpyz( blue_team, "blue team", sizeof( blue_team ) );
		}
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " joined the %s.\n\"", client->pers.netname, blue_team ) );
	} else if ( client->sess.sessionTeam == TEAM_SPECTATOR && oldTeam != TEAM_SPECTATOR ) {
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " is now spectating.\n\"",
		client->pers.netname));
	} else if ( client->sess.sessionTeam == TEAM_FREE ) {
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " joined the Roleplay Session.\n\"",
		client->pers.netname));
	}
}

//void SetPlayerClassCvar(gentity_t *ent)
//{
//	gclient_t *client;
//
//	client = ent->client;
//
//	switch( client->sess.sessionClass )
//	{
//	case PC_INFILTRATOR:
////		trap_Cvar_Set("ui_playerclass", "INFILTRATOR");
//		trap_SendServerCommand(ent-g_entities,"pc INFILTRATOR");  
//		break;
//	case PC_SNIPER:
////		trap_Cvar_Set("ui_playerclass", "SNIPER");
//		trap_SendServerCommand(ent-g_entities,"pc SNIPER");  
//		break;
//	case PC_HEAVY:
////		trap_Cvar_Set("ui_playerclass", "HEAVY");
//		trap_SendServerCommand(ent-g_entities,"pc HEAVY");  
//		break;
//	case PC_DEMO:
////		trap_Cvar_Set("ui_playerclass", "DEMO");
//		trap_SendServerCommand(ent-g_entities,"pc DEMO");  
//		break;
//	case PC_MEDIC:
////		trap_Cvar_Set("ui_playerclass", "MEDIC");
//		trap_SendServerCommand(ent-g_entities,"pc MEDIC");  
//		break;
//	case PC_TECH:
////		trap_Cvar_Set("ui_playerclass", "TECH");
//		trap_SendServerCommand(ent-g_entities,"pc TECH");  
//		break;
///*	case PC_ADMIN:
//		trap_SendServerCommand(ent-g_entities,"pc MAKER");  
//		break;
//	case PC_SECURITY:
//		trap_SendServerCommand(ent-g_entities,"pc SECURITY");  
//		break;
//	case PC_MEDICAL:
//		trap_SendServerCommand(ent-g_entities,"pc MEDICAL");  
//		break;
//	case PC_ALIEN:
//		trap_SendServerCommand(ent-g_entities,"pc ALIEN");  
//		break;
//	case PC_COMMAND:
//		trap_SendServerCommand(ent-g_entities,"pc COMMAND");  
//		break;
//	case PC_SCIENCE:
//		trap_SendServerCommand(ent-g_entities,"pc SCIENCE");  
//		break;
//	case PC_ENGINEER:
//		trap_SendServerCommand(ent-g_entities,"pc ENGINEER");  
//		break;
//	case PC_ALPHAOMEGA22:
//		trap_SendServerCommand(ent-g_entities,"pc ALPHAOMEGA22");  
//		break;
//	case PC_N00B:
//		trap_SendServerCommand(ent-g_entities,"pc N00B");  
//		break;*/
//	case PC_BORG:
////		trap_Cvar_Set("ui_playerclass", "BORG");
//		trap_SendServerCommand(ent-g_entities,"pc BORG");  
//		break;
//	case PC_ACTIONHERO:
////		trap_Cvar_Set("ui_playerclass", "HERO");
//		trap_SendServerCommand(ent-g_entities,"pc HERO");  
//		break;
//	case PC_NOCLASS:
////		trap_Cvar_Set("ui_playerclass", "NOCLASS");
//		trap_SendServerCommand(ent-g_entities,"pc NOCLASS");  
//		break;
//	}
//}

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

	/*switch( client->sess.sessionClass )
	{
	case PC_INFILTRATOR:
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " is now an Infiltrator.\n\"", client->pers.netname) );
		break;
	case PC_SNIPER:
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " is now a Sniper.\n\"", client->pers.netname) );
		break;
	case PC_HEAVY:
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " is now a Heavy Weapons Specialist.\n\"", client->pers.netname) );
		break;
	case PC_DEMO:
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " is now a Demolitionist.\n\"", client->pers.netname) );
		break;
	case PC_MEDIC:
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " is now a Medic.\n\"", client->pers.netname) );
		break;
	case PC_TECH:
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " is now a Technician.\n\"", client->pers.netname) );
		break;
	case PC_ALPHAOMEGA22:
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " has joined the Marine Corps.\n\"", client->pers.netname) );
		break;
	case PC_SECURITY:
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " has joined Security.\n\"", client->pers.netname) );
		break;
	case PC_ADMIN:
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " is now a Server Admin.\n\"", client->pers.netname) );
		break;
	case PC_MEDICAL:
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " has joined Medical.\n\"", client->pers.netname) );
		break;
	case PC_ALIEN:
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " is now an Alien.\n\"", client->pers.netname) );
		break;
	case PC_COMMAND:
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " has joined Command.\n\"", client->pers.netname) );
		break;
	case PC_SCIENCE:
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " has joined Sciences.\n\"", client->pers.netname) );
		break;
	case PC_ENGINEER:
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " has joined Engineering.\n\"", client->pers.netname) );
		break;
	case PC_N00B:
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " is a stupid n00bie.\n\"", client->pers.netname) );
		break;
	case PC_ACTIONHERO:
		trap_SendServerCommand( -1, va("cp \"%.15s" S_COLOR_WHITE " is the Action Hero!\n\"", client->pers.netname) );
		break;
	}*/
}

/*
=================
SetTeam
=================
*/
qboolean SetTeam( gentity_t *ent, char *s ) {
	int					team, oldTeam;
	gclient_t			*client;
	int					clientNum;
	spectatorState_t	specState;
	int					specClient;

	int					isBot;
	char				info[MAX_INFO_STRING];
	isBot = atoi( Info_ValueForKey( info, "skill" ) );

	//
	// see what change is requested
	//
	client = ent->client;

	clientNum = client - level.clients;
	specClient = 0;

	specState = SPECTATOR_NOT;
	
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
		ent->client->noclip = 1;
	} else if ( g_gametype.integer >= GT_TEAM ) {
		// if running a team game, assign player to one of the teams
		specState = SPECTATOR_NOT;
		if ( !Q_stricmp( s, "red" ) || !Q_stricmp( s, "r" ) ) {
			team = TEAM_RED;
		} else if ( !Q_stricmp( s, "blue" ) || !Q_stricmp( s, "b" ) ) {
			team = TEAM_BLUE;
		} else {
			// pick the team with the least number of players
			// team = PickTeam( clientNum );
			if ( isBot )
			{
				team = PickTeam( clientNum );
			}
			else
			{
				team = TEAM_SPECTATOR;
				specState = SPECTATOR_FREE;
			}
		}

		if ( g_teamForceBalance.integer && g_pModAssimilation.integer == 0 )
		{
			int		counts[TEAM_NUM_TEAMS];

			counts[TEAM_BLUE] = TeamCount( ent->client->ps.clientNum, TEAM_BLUE );
			counts[TEAM_RED] = TeamCount( ent->client->ps.clientNum, TEAM_RED );

			// We allow a spread of two
			if ( team == TEAM_RED && counts[TEAM_RED] - counts[TEAM_BLUE] > 1 )
			{
				trap_SendServerCommand( ent->client->ps.clientNum, 
					"cp \"Red team has too many players.\n\"" );
				return qfalse; // ignore the request
			}
			if ( team == TEAM_BLUE && counts[TEAM_BLUE] - counts[TEAM_RED] > 1 )
			{
				trap_SendServerCommand( ent->client->ps.clientNum, 
					"cp \"Blue team has too many players.\n\"" );
				return qfalse; // ignore the request
			}

			// It's ok, the team we are switching to has less or same number of players
		}

	}
	else
	{
		if ( g_pModElimination.integer != 0 && numKilled == 0 || g_pModElimination.integer == 0 )
		{
			team = TEAM_FREE;
		}
		else
		{
			team = TEAM_SPECTATOR;
			specState = SPECTATOR_FREE;
		}
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
	oldTeam = client->sess.sessionTeam;
	if ( team == oldTeam && team != TEAM_SPECTATOR ) {
		return qfalse;
	}

	//replace them if they're the queen
	if ( borgQueenClientNum != -1 )
	{
		G_CheckReplaceQueen( clientNum );
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
		player_die (ent, NULL, NULL, 100000, MOD_RESPAWN);

	}
	// they go to the end of the line for tournements
	if ( team == TEAM_SPECTATOR ) {
		client->sess.spectatorTime = level.time;
	}

	client->sess.sessionTeam = team;
	client->sess.spectatorState = specState;
	client->sess.spectatorClient = specClient;

	BroadcastTeamChange( client, oldTeam );

	// get and distribute relevent paramters
	ClientUserinfoChanged( clientNum );
	
	//THIS IS VERY VERY BAD, CAUSED ENDLESS WARMUP, FOUND ANOTHER WAY TO PREVENT DOORS
	/*
	if (level.time - client->pers.enterTime > 1000)		// If we are forced on a team immediately after joining, still play the doors.
	{	// We signal NOT to play the doors by setting level.restarted to true.  This is abusing the system, but it works.
		level.restarted = qtrue;
	}
	*/

	ClientBegin( clientNum, qfalse );

	return qtrue;
}

char *ClassNameForValue( pclass_t pClass ) 
{
	char buffer[MAX_QPATH];
	char *ptr;

	trap_Cvar_VariableStringBuffer( va( "rpg_%sPass", g_classData[pClass].consoleName ), buffer, sizeof(buffer) );

	ptr = buffer;

	return ptr;

	//switch ( pClass )
	//{
	//case PC_NOCLASS://default
	//	return "noclass";
	//	break;
	//case PC_INFILTRATOR://fast: low attack
	//	return "infiltrator";
	//	break;
	//case PC_SNIPER://sneaky: snipe only
	//	return "sniper";
	//	break;
	//case PC_HEAVY://slow: heavy attack
	//	return "heavy";
	//	break;
	//case PC_DEMO://go boom
	//	return "demo";
	//	break;
	//case PC_MEDIC://heal
	//	return "medic";
	//	break;
	//case PC_TECH://operate
	//	return "tech";
	//	break;
	//case PC_ADMIN://for escorts
	//	return rpg_adminpass.string;
	//	break;
	//case PC_SECURITY://for escorts
	//	return rpg_securitypass.string;
	//	break;
	//case PC_MEDICAL://for escorts
	//	return rpg_medicalpass.string;
	//	break;
	//case PC_ALIEN://for escorts
	//	return rpg_alienpass.string;
	//	break;
	//case PC_COMMAND://for escorts
	//	return rpg_commandpass.string;
	//	break;
	//case PC_SCIENCE://for escorts
	//	return rpg_sciencepass.string;
	//	break;
	//case PC_ENGINEER://for escorts
	//	return rpg_engineerpass.string;
	//	break;
	//case PC_ACTIONHERO://has everything
	//	return "hero";
	//	break;
	//case PC_BORG://special weapons: slower: adapting shields
	//	return "borg";
	//	break;
	//case PC_ALPHAOMEGA22://for escorts
	//	return rpg_marinepass.string;
	//	break;
	//case PC_N00B://for escorts
	//	return rpg_n00bpass.string;
	//	break;
	//}
	//return "noclass";
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

pclass_t ValueNameForClass ( /*gentity_t *ent,*/ char* s ) {
	
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

	/*if ( !Q_stricmp( s, rpg_adminpass.string ) && rpg_adminpass.string[0] ) 
	{
		pclass = PC_ADMIN;
	} 
	else if ( !Q_stricmp( s, rpg_securitypass.string ) && rpg_securitypass.string[0] ) 
	{
		pclass = PC_SECURITY;
	} 
	else if ( !Q_stricmp( s, rpg_medicalpass.string ) && rpg_medicalpass.string[0] ) 
	{
		pclass = PC_MEDICAL;
	} 
	else if ( !Q_stricmp( s, rpg_alienpass.string ) && rpg_alienpass.string[0] ) 
	{
		pclass = PC_ALIEN;
	} 
	else if ( !Q_stricmp( s, rpg_commandpass.string ) && rpg_commandpass.string[0] ) 
	{
		pclass = PC_COMMAND;
	} 
	else if ( !Q_stricmp( s, rpg_sciencepass.string ) && rpg_sciencepass.string[0] ) 
	{
		pclass = PC_SCIENCE;
	} 
	else if ( !Q_stricmp( s, rpg_engineerpass.string ) && rpg_engineerpass.string[0] ) 
	{
		pclass = PC_ENGINEER;
	}  
	else if ( !Q_stricmp( s, rpg_emoteList
	marinepass.string ) && rpg_marinepass.string[0] ) 
	{
		pclass = PC_ALPHAOMEGA22;
	} 
	else if ( !Q_stricmp( s, rpg_n00bpass.string ) && rpg_n00bpass.string[0] ) 
	{
		pclass = PC_N00B;
	} 
	else if ( !Q_stricmp( s, "noclass" ) ) 
	{
		pclass = PC_NOCLASS;
	}
	else {
		pclass = PC_NOCLASS;
	}*/

	//return pclass;
}

/*
=================
ValueNameForClass

TiM: Created so we can get the class
name off of the actual class, and not user input

More effective code recycling. :)
=================
*/

char* ClassForValueName ( pclass_t pclass ) {

	return g_classData[pclass].formalName;

	//switch ( pclass ) {
	//	case PC_ADMIN:
	//		return "Admin";
	//	case PC_SECURITY:
	//		return "Security";
	//	case PC_ALIEN:
	//		return "Alien";
	//	case PC_COMMAND:
	//		return "Command";
	//	case PC_SCIENCE:
	//		return "Science";
	//	case PC_ENGINEER:
	//		return "Engineer";
	//	case PC_ALPHAOMEGA22:
	//		return "Marine";
	//	case PC_N00B:
	//		return "n00b";
	//	case PC_NOCLASS:
	//	default:
	//		return "noClass";
	//}
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
	//int OldType;

	//FIXME: check for appropriate game mod being on first

	//FIXME: can't change class while playing

	//
	// see what change is requested
	//
	client = ent->client;

	clientNum = client - level.clients;

	//RPG-X: TiM - Code Recyclables :)
	pclass = ValueNameForClass( /*ent,*/ s );

	//TiM : Um... not sure why we'd still need these, but ah well
	//if ( !Q_stricmp( s, "infiltrator" ) ) 
	//{
	//	pclass = PC_INFILTRATOR;
	//}
	//else if ( !Q_stricmp( s, "sniper" ) ) 
	//{
	//	pclass = PC_SNIPER;
	//} 
	//else if ( !Q_stricmp( s, "heavy" ) ) 
	//{
	//	pclass = PC_HEAVY;
	//} 
	//else if ( !Q_stricmp( s, "demo" ) ) 
	//{
	//	pclass = PC_DEMO;
	//} 
	//else if ( !Q_stricmp( s, "medic" ) ) 
	//{
	//	pclass = PC_MEDIC;
	//} 
	//else if ( !Q_stricmp( s, "tech" ) ) 
	//{
	//	pclass = PC_TECH;
	//} 
	//else if ( !Q_stricmp( s, "borg" ) ) 
	//{
	//	pclass = PC_BORG;
	//} 
	//else 
	//{
		//pclass = irandom( PC_SNIPER, PC_TECH );
		if (pclass < 0 ) {
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"Class doesn't exist!\n\""));
			return qfalse;
		}
			//TiM: Do this, coz my new code changes made 
			//this error play all the time even if it had a class lol O_o
	//}

	//
	// decide if we will allow the change
	//
	oldPClass = client->sess.sessionClass;

	//switch ( pclass )
	//{
	//case PC_INFILTRATOR:
	//case PC_SNIPER:
	//case PC_HEAVY:
	//case PC_DEMO:
	//case PC_MEDIC:
	//case PC_TECH:
	//	if ( g_pModSpecialties.integer == 0 )
	//	{
	//		//trap_SendServerCommand( ent-g_entities, "print \"Specialty mode is not enabled.\n\"" );
	//		return qfalse;
	//	}
	//	break;
	//case PC_BORG:
	//	if ( g_pModAssimilation.integer == 0 )
	//	{
	//		//trap_SendServerCommand( ent-g_entities, "print \"Assimilation mode is not enabled.\n\"" );
	//		return qfalse;
	//	}
	//	break;
	//case PC_MEDICAL:
	//	if ( !rpg_medicalpass.string[0] || rpg_rpg.integer == 0 || rpg_rpg.integer != 0 && rpg_norpgclasses.integer != 0 || rpg_rpg.integer != 0 && rpg_nomedical.integer != 0 )
	//	{
	//		return qfalse;
	//	}
	//	break;
	//case PC_COMMAND:
	//	if ( !rpg_commandpass.string[0] || rpg_rpg.integer == 0 || rpg_rpg.integer != 0 && rpg_norpgclasses.integer != 0 || rpg_rpg.integer != 0 && rpg_nocommand.integer != 0 )
	//	{
	//		return qfalse;
	//	}
	//	break;
	//case PC_SCIENCE:
	//	if ( !rpg_sciencepass.string[0] || rpg_rpg.integer == 0 || rpg_rpg.integer != 0 && rpg_norpgclasses.integer != 0 || rpg_rpg.integer != 0 && rpg_noscience.integer != 0 )
	//	{
	//		return qfalse;
	//	}
	//	break;
	//case PC_ENGINEER:
	//	if ( !rpg_engineerpass.string[0] || rpg_rpg.integer == 0 || rpg_rpg.integer != 0 && rpg_norpgclasses.integer != 0 || rpg_rpg.integer != 0 && rpg_noengineer.integer != 0 )
	//	{
	//		return qfalse;
	//	}
	//	break;
	//case PC_SECURITY:
	//	if ( !rpg_securitypass.string[0] || rpg_rpg.integer == 0 || rpg_rpg.integer != 0 && rpg_norpgclasses.integer != 0 || rpg_rpg.integer != 0 && rpg_nosecurity.integer != 0 )
	//	{
	//		return qfalse;
	//	}
	//	break;
	//case PC_ADMIN:
	//	if ( !rpg_adminpass.string[0] || rpg_rpg.integer == 0 || rpg_rpg.integer != 0 && rpg_norpgclasses.integer != 0 || rpg_rpg.integer != 0 && rpg_nomaker.integer != 0 )
	//	{
	//		return qfalse;
	//	}
	//	break;
	//case PC_ALPHAOMEGA22:
	//	if ( !rpg_marinepass.string[0] || rpg_rpg.integer == 0 || rpg_rpg.integer != 0 && rpg_norpgclasses.integer != 0 || rpg_rpg.integer != 0 && rpg_nomarine.integer != 0 )
	//	{
	//		return qfalse;
	//	}
	//	break;
	//case PC_ALIEN:
	//	if ( !rpg_alienpass.string[0] || rpg_rpg.integer == 0 || rpg_rpg.integer != 0 && rpg_norpgclasses.integer != 0 || rpg_rpg.integer != 0 && rpg_noalien.integer != 0 )
	//	{
	//		return qfalse;
	//	}
	//	break;
	//case PC_N00B:
	//	if ( !rpg_n00bpass.string[0] || rpg_rpg.integer == 0 || rpg_rpg.integer != 0 && rpg_norpgclasses.integer != 0 || rpg_rpg.integer != 0 && rpg_non00b.integer != 0 )
	//	{
	//		return qfalse;
	//	}
	//	break;
	//case PC_NOCLASS:
	//	if ( g_pModSpecialties.integer )
	//	{
	//		//trap_SendServerCommand( ent-g_entities, "print \"Cannot switch to no class in this game mode.\n\"" );
	//		return qfalse;
	//	}
	//	break;
	//}

	if ( pclass == oldPClass )
	{
		//SetPlayerClassCvar(ent);
		return qfalse;
	}

	//
	// execute the class change
	//

	if ( g_classData[pclass].isAdmin/*pclass == PC_ADMIN*/ ) {
		// RPG-X | Phenix | 21/11/2004 | If they go into the admin class remove other
		// admin settings
		ent->client->AdminFailed = 0;
		ent->client->LoggedAsAdmin = qfalse;
		ent->client->LoggedAsDeveloper = qfalse;
	}

	client->sess.sessionClass = pclass;

	//SetPlayerClassCvar(ent);

	BroadcastClassChange( client, oldPClass );

	if ( teamName != NULL && SetTeam( ent, teamName ) )
	{
		return qtrue;
	}
	else
	{//not changing teams or couldn't change teams
		// get and distribute relevent paramters
		ClientUserinfoChanged( clientNum );

		//if in the game already, kill and respawn him, else just wait to join
		if ( client->sess.sessionTeam == TEAM_SPECTATOR )
		{// they go to the end of the line for tournaments
			client->sess.spectatorTime = level.time;
		}
		else
		{
			//RPG-X | Phenix | 08/03/2005
			/*if ( client->lasersight) {
			//RPG-X: J2J - Phenix, please check for compiler errors before uplaoding new code. OldType was oldType, thus causing an error.
				  OldType = client->lasersight->s.eventParm;	
				  G_FreeEntity(client->lasersight );
				  client->lasersight = NULL;
			}*/
			
			//RPG-X: RedTechie - No respawn for n00bs set all info and frap that a n00b needs HERE this eliminates respawns for n00bs
			if(g_classData[pclass].isn00b/*pclass == PC_N00B*/){
				
				ClientSpawn(ent, 1);
				client->ps.stats[STAT_WEAPONS] = ( 1 << WP_NONE );
				client->ps.stats[STAT_HOLDABLE_ITEM] = HI_NONE;
				
				//Take some admin powerups away and give some n00b powerps ;)
				ent->flags &= ~FL_GODMODE;
				ent->client->ps.powerups[PW_FLIGHT] = 0;
				ent->client->ps.powerups[PW_INVIS] = 0;
				ent->client->ps.powerups[PW_EVOSUIT] = 0;
				client->ps.powerups[PW_GHOST] = INT_MAX;

				//Turn no clip on for the N00B so he dosnt disturb rp's				
				ent->client->noclip = qtrue;
				ent->r.contents = CONTENTS_CORPSE;
				

				trap_SendServerCommand(ent->client->ps.clientNum, "playN00bInsult");
			}else{
				// he starts at 'base' - Removed phenix
				client->pers.teamState.state = TEAM_BEGIN;

				// Kill him (makes sure he loses flags, etc)
				ent->flags &= ~FL_GODMODE;
				ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;

				player_die (ent, NULL, NULL, 100000, MOD_RESPAWN);
				ClientBegin( clientNum, qfalse );
			}

			//THIS IS VERY VERY BAD, CAUSED ENDLESS WARMUP, FOUND ANOTHER WAY TO PREVENT DOORS
			/*
			if (level.time - client->pers.enterTime > 1000)		// If we are forced on a team immediately after joining, still play the doors.
			{	// We signal NOT to play the doors by setting level.restarted to true.  This is abusing the system, but it works.
				level.restarted = qtrue;
			}
			*/

			
		}
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
	ent->client->ps.persistant[ PERS_TEAM ] = TEAM_SPECTATOR;	
	ent->client->sess.sessionTeam = TEAM_SPECTATOR;	
	ent->client->sess.spectatorState = SPECTATOR_FREE;
	ent->client->ps.pm_flags &= ~PMF_FOLLOW;
	ent->r.svFlags &= ~SVF_BOT;
	ent->client->ps.clientNum = ent - g_entities;
	//don't be dead
	ent->client->ps.stats[STAT_HEALTH] = ent->client->ps.stats[STAT_MAX_HEALTH];
}

extern team_t	borgTeam;

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

	//RPG-X: Redtechie - haha to stupid to change teams!
	if ( g_classData[ent->client->sess.sessionClass].isn00b/*ent->client->sess.sessionClass == PC_N00B*/ )
	{
		Com_sprintf (send, sizeof(send), "%s ^7 doesn't know when to quit trying to change teams!", ent->client->pers.netname); 
 
		for (j = 0; j < 1023; j++) { 
			if(g_entities[j].client){
				other = &g_entities[j];
				G_SayTo( ent, other, 5, COLOR_CYAN, "^7Server: ", send );
			}
		}

		trap_SendServerCommand( ent-g_entities, "print \"Sorry, you're too n00bish to handle this command.\n\"" );
		return;
	}

	oldTeam = ent->client->sess.sessionTeam;

	/*
	* RPG-X | Phenix | 26/03/2007
	*
	* Removed due to Task#41

	if ( ent->client->switchTeamTime > level.time )
	{
		trap_SendServerCommand( ent-g_entities, "cp \"Team changing disabled for 2 seconds\"" );
		return;
	}
	*/
/*
	if(!Q_stricmp( ent->client->sess.sessionClass, rpg_n00bpass.string ) && rpg_n00bpass.string[0])
		return;
*/
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

	if ( !s || s[0] != 's' )
	{//not trying to become a spectator
		/*if ( g_pModElimination.integer )
		{
			if ( numKilled > 0 )
			{
				if ( ent->client->ps.eFlags & EF_ELIMINATED )
				{
					trap_SendServerCommand( ent-g_entities, "cp \"You have been eliminated until next round\"" );
				}
				else if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR )
				{
					trap_SendServerCommand( ent-g_entities, "cp \"Wait until next round to join\"" );
				}
				else if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR )
				{
					trap_SendServerCommand( ent-g_entities, "cp \"Wait until next round to change teams\"" );
				}
				return;
			}
		}*/

/*		if ( g_pModAssimilation.integer )
		{
			if ( borgQueenClientNum != -1 && numKilled > 0 )
			{
				if ( ent->client->ps.eFlags & EF_ASSIMILATED )
				{
					trap_SendServerCommand( ent-g_entities, "cp \"You have been assimilated until next round\"" );
					return;
				}
				else if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR )
				{
						if ( s[0] == 'r' && borgTeam != TEAM_RED || s[0] == 'b' && borgTeam != TEAM_BLUE )
						{
							trap_SendServerCommand( ent-g_entities, "cp \"Wait until next round or join the Borg\"" );
							return;
						}
				}
				else if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR )
				{
					trap_SendServerCommand( ent-g_entities, "cp \"Wait until next round to change teams\"" );
					return;
				}
//				return;
			}
		}*/
	}

	// if they are playing a tournement game, count as a loss
	if ( g_gametype.integer == GT_TOURNAMENT && ent->client->sess.sessionTeam == TEAM_FREE )
	{
		ent->client->sess.losses++;
	}

	//if this is a manual change, not an assimilation, uninitialize the clInitStatus data
	SetTeam( ent, s );
	//clientInitialStatus[ent->s.number].initialized = qfalse;

	ent->client->switchTeamTime = level.time + 2000;
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
	qboolean	check = qtrue;
	char	*className;
	
	if(!ent || !ent->client)
		return;

	if ( ent->flags & FL_CLAMPED )
		return;

	//RPG-X: Redtechie - haha to stupid to change classes!
	if ( g_classData[ent->client->sess.sessionClass].isn00b/*ent->client->sess.sessionClass == PC_N00B*/ )
	{
		Com_sprintf (send, sizeof(send), "%s ^7 doesn't know when to quit trying to change classes from n00bie", ent->client->pers.netname); 
 
		for (j = 0; j < 1023; j++) { 
			if(g_entities[j].client){
				other = &g_entities[j];
				G_SayTo( ent, other, 5, COLOR_CYAN, "^7Server: ", send );
			}
		}

		trap_SendServerCommand( ent-g_entities, "print \"Sorry, you're too n00bish to handle this command.\n\"" );
		return;
	}

	OldScoreclass = ent->client->ps.persistant[PERS_SCORE];

	if ( trap_Argc() == 1 ) 
	{
		className = g_classData[ent->client->sess.sessionClass].formalName;
		check = qfalse;

		//Just asking what class they're on
		//switch ( ent->client->sess.sessionClass ) {
		//case PC_NOCLASS://default
		//	className = "Noclass";
		//	check = qfalse; //We need this or it crashes the UI O_o
		//	break;
		//case PC_INFILTRATOR://fast: low attack
		//	className = "Infiltrator";
		//	break;
		//case PC_SNIPER://sneaky: snipe only
		//	className = "Sniper";
		//	break;
		//case PC_HEAVY://slow: heavy attack
		//	className = "Heavy";
		//	break;
		//case PC_DEMO://go boom
		//	className = "Demo";
		//	break;
		//case PC_MEDIC://heal
		//	className = "Medic";
		//	break;
		//case PC_TECH://operate
		//	className = "Tech";
		//	break;
		//case PC_SECURITY://for escorts
		//	className = "Security";
		//	check = qfalse;
		//	break;
		//case PC_ADMIN://for escorts
		//	className = "Maker";
		//	check = qfalse;
		//	break;
		//case PC_MEDICAL://for escorts
		//	className = "Medical";
		//	check = qfalse;
		//	break;
		//case PC_ALIEN://for escorts
		//	className = "Alien";
		//	check = qfalse;
		//	break;
		//case PC_COMMAND://for escorts
		//	className = "Command";
		//	check = qfalse;
		//	break;
		//case PC_SCIENCE://for escorts
		//	className = "Science";
		//	check = qfalse;
		//	break;
		//case PC_ENGINEER://for escorts
		//	className = "Engineer";
		//	check = qfalse;
		//	break;
		//case PC_ALPHAOMEGA22://for escorts
		//	className = "Marine";
		//	check = qfalse;
		//	break;
		//case PC_N00B://for escorts
		//	className = "n00b";
		//	check = qfalse;
		//	break;
		//case PC_ACTIONHERO://has everything
		//	return;//can't set this via a command, it is automatic
		//	//className = "Hero";
		//	break;
		//case PC_BORG://special weapons: slower: adapting shields
		//	className = "Borg";
		//	check = qfalse;
		//	break;
		//default:
		//	trap_SendServerCommand( ent-g_entities, "print \"Unknown current class!\n\"" );
		//	return;
		//	break;
		//}
		/*if ( check && g_pModSpecialties.integer == 0 )
		{//FIXME: if this guys has a specialty class and we're not in specialties mode, there is a serious problem
			trap_SendServerCommand( ent-g_entities, "print \"Specialty mode is not enabled.\n\"" );
			return;
		}*/

		trap_SendServerCommand( ent-g_entities, va( "print \"\nCurrent Class: %s\nUsage: Changes the user to a different class\nCommand: Class <Class Name>\n\nType '/classlist' into the console for a more complete list\n\"", className ) );
		return;
	}

	//if ( g_pModElimination.integer )
	//{
	//	if ( numKilled > 0 )
	//	{
	//		if ( ent->client->ps.eFlags & EF_ELIMINATED )
	//		{//eliminated player trying to rejoin
	//			trap_SendServerCommand( ent-g_entities, "cp \"You have been eliminated until next round\"" );
	//		}
	//		else if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR )
	//		{
	//			trap_SendServerCommand( ent-g_entities, "cp \"Wait until next round to join\"" );
	//		}
	//		else
	//		{
	//			trap_SendServerCommand( ent-g_entities, "cp \"Wait until next round to change class\"" );
	//		}
	//		return;
	//	}
	//}

/*	if ( g_pModAssimilation.integer )
	{
		if ( ent->client->ps.eFlags & EF_ASSIMILATED )
		{
			trap_SendServerCommand( ent-g_entities, "cp \"You have been assimilated until next round\"" );
			return;
		}
		else if ( numKilled > 0 )
		{
			if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR )
			{
				trap_SendServerCommand( ent-g_entities, "cp \"Wait until next round to join\"" );
				return;
			}
			else if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR )
			{
				trap_SendServerCommand( ent-g_entities, "cp \"Wait until next round to change class\"" );
				return;
			}
			return;
		}
	}*/

	/*if ( g_pModSpecialties.integer )
	{
		if ( ent->client->classChangeDebounceTime > level.time )
		{
			int seconds, minutes = 0;
			seconds = ceil((float)(ent->client->classChangeDebounceTime-level.time)/1000.0f);
			if ( seconds >= 60 )
			{
				minutes = floor(seconds/60.0f);
				seconds -= minutes*60;
				if ( minutes > 1 )
				{
					if ( seconds )
					{
						if ( seconds > 1 )
						{
							trap_SendServerCommand( ent-g_entities, va("cp \"Cannot change classes again for %d minutes and %d seconds\"", minutes, seconds ) );
						}
						else
						{
							trap_SendServerCommand( ent-g_entities, va("cp \"Cannot change classes again for %d minutes\"", minutes ) );
						}
					}
					else
					{
						trap_SendServerCommand( ent-g_entities, va("cp \"Cannot change classes again for %d minutes\"", minutes ) );
					}
				}
				else
				{
					if ( seconds )
					{
						if ( seconds > 1 )
						{
							trap_SendServerCommand( ent-g_entities, va("cp \"Cannot change classes again for %d minute and %d seconds\"", minutes, seconds ) );
						}
						else
						{
							trap_SendServerCommand( ent-g_entities, va("cp \"Cannot change classes again for %d minute and %d second\"", minutes, seconds ) );
						}
					}
					else
					{
						trap_SendServerCommand( ent-g_entities, va("cp \"Cannot change classes again for %d minute\"", minutes ) );
					}
				}
			}
			else
			{
				if ( seconds > 1 ) 
				{
					trap_SendServerCommand( ent-g_entities, va("cp \"Cannot change classes again for %d seconds\"", seconds ) );
				}
				else
				{
					trap_SendServerCommand( ent-g_entities, va("cp \"Cannot change classes again for %d second\"", seconds ) );
				}
			}
			return;
		}
	}*/

	//trying to set your class
	trap_Argv( 1, s, sizeof( s ) );

	////can't manually change to some classes
	//if ( Q_stricmp( "borg", s ) == 0 || Q_stricmp( "hero", s ) == 0 )
	//{
	//	trap_SendServerCommand( ent-g_entities, va( "print \"Cannot manually change to class %s\n\"", s ) );
	//	return;
	//}
	
	//can't change from a Borg class
	//if ( ent->client->sess.sessionClass == PC_BORG )
	//{
	//	trap_SendServerCommand( ent-g_entities, "print \"Cannot manually change from class Borg\n\"" );
	//	return;
	//}

	//if this is a manual change, not an assimilation, uninitialize the clInitStatus data
	clientInitialStatus[ent->s.number].initialized = qfalse;
	if ( SetClass( ent, s, NULL, qtrue ) )
	{
		//if still in warmup, don't debounce class changes
		if ( g_doWarmup.integer )
		{
			if ( level.warmupTime != 0 )
			{
				if ( level.warmupTime < 0 || level.time - level.startTime <= level.warmupTime )
				{
					return;
				}
			}
		}
		//if warmuptime is over, don't change classes again for a bit
		ent->client->classChangeDebounceTime = level.time + (g_classChangeDebounceTime.integer*1000);

		trap_SendServerCommand( ent-g_entities, va ( "pc %s", s ) );
	}
	ent->client->ps.persistant[PERS_SCORE] = OldScoreclass;
}

/*
=================
Cmd_Cloak_f
=================
*/
static void Cmd_Cloak_f( gentity_t *ent )
{
	gclient_t	*client;
	char		*msg;

	client = ent->client;
	
	if ( IsAdmin( ent ) == qfalse ) {
		return;
	}

	ent->flags ^= FL_CLOAK;
	if (!(ent->flags & FL_CLOAK))
	{
		msg = "Cloak Is Off\n";
		ent->client->ps.powerups[PW_INVIS] = level.time;
	}        
	else
	{
		msg = "Cloak Is On\n";
		ent->client->ps.powerups[PW_INVIS] = INT_MAX;
	}
	//G_AddEvent( ent, EV_Q_FLASH, 0 );

	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));

}

/*
=================
RPG-X (Admin Feature)
Cmd_EvoSuit_f
Phenix - 8/8/2004
=================
*/
static void Cmd_EvoSuit_f( gentity_t *ent )
{
	gclient_t	*client;
	char		*msg;

	client = ent->client;
	
	if ( IsAdmin( ent ) == qfalse ) {
		return;
	}

	ent->flags ^= FL_EVOSUIT;
	if (!(ent->flags & FL_EVOSUIT))
	{
		msg = "You have taken an EVA Suit off\n";
		ent->client->ps.powerups[PW_EVOSUIT] = 0; //level.time;   //eh? who put this here? -J2J
	}        
	else
	{
		msg = "You have put an EVA Suit on\n";
		ent->client->ps.powerups[PW_EVOSUIT] = INT_MAX;
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
	gclient_t	*client;
	char		*msg;

	client = ent->client;

	if ( IsAdmin( ent ) == qfalse ) {
		return;
	}

	ent->flags ^= FL_FLY;
	if (!(ent->flags & FL_FLY))
	{
		msg = "Flight Is Off\n";
		ent->client->ps.powerups[PW_FLIGHT] = level.time;
	}        
	else
	{
		msg = "Flight Is On\n";
		ent->client->ps.powerups[PW_FLIGHT] = INT_MAX;
	}
	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}

/*
=================
Cmd_Follow_f
=================
*/
static void Cmd_Follow_f( gentity_t *ent ) {
	int		i;
	char	arg[MAX_TOKEN_CHARS];

	if ( ent->r.svFlags&SVF_BOT )
	{//bots can't follow!
		return;
	}
	/*else
	{
		return;
	}*/

	if ( trap_Argc() != 2 ) {
		if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ) {
			StopFollowing( ent );
		}
		return;
	}

	trap_Argv( 1, arg, sizeof( arg ) );
	i = ClientNumberFromString( ent, arg );
	if ( i == -1 ) {
		return;
	}

	// can't follow self
	if ( &level.clients[ i ] == ent->client ) {
		return;
	}

	// can't follow another spectator
	if ( level.clients[ i ].sess.sessionTeam == TEAM_SPECTATOR ) {
		return;
	}

//	if ( g_pModElimination.integer != 0 || g_pModAssimilation.integer != 0 )
//	{//don't do this follow stuff, it's bad!
		VectorCopy( level.clients[i].ps.viewangles, ent->client->ps.viewangles );
		VectorCopy( level.clients[i].ps.origin, ent->client->ps.origin );
		return;
//	}

/*	// if they are playing a tournement game, count as a loss
	if ( g_gametype.integer == GT_TOURNAMENT && ent->client->sess.sessionTeam == TEAM_FREE ) {
		ent->client->sess.losses++;
	}

	// first set them to spectator
	if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		SetTeam( ent, "spectator" );
	}*/

	//ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
	//ent->client->sess.spectatorClient = i;
	
}

/*
=================
Cmd_FollowCycle_f
=================
*/
void Cmd_FollowCycle_f( gentity_t *ent, int dir ) {
	int		clientnum;
	int		original;

	if ( ent->r.svFlags&SVF_BOT )
	{//bots can't follow!
		return;
	}
/*	else
	{
		return;
	}
*/
	// if they are playing a tournement game, count as a loss
	if ( g_gametype.integer == GT_TOURNAMENT && ent->client->sess.sessionTeam == TEAM_FREE ) {
		ent->client->sess.losses++;
	}
	// first set them to spectator
	if ( ent->client->sess.spectatorState == SPECTATOR_NOT ) {
		//if ( g_pModElimination.integer == 0 )
		//{
			SetTeam( ent, "spectator" );
		//}
	}

	if ( dir != 1 && dir != -1 ) {
		G_Error( "Cmd_FollowCycle_f: bad dir %i", dir );
	}

	clientnum = ent->client->sess.spectatorClient;
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

		/*if ( g_pModElimination.integer != 0 )
		{//don't do this follow stuff, it's bad!
			if ( level.clients[ clientnum ].ps.eFlags&EF_ELIMINATED)
			{//don't cycle to a dead guy
				continue;
			}

			//TiM : Why was this portion of code removed from this conditional??
			VectorCopy( level.clients[clientnum].ps.viewangles, ent->client->ps.viewangles );
			VectorCopy( level.clients[clientnum].ps.origin, ent->client->ps.origin );
			ent->client->sess.spectatorClient = clientnum;
			return;
		}*/

		//TiM: Fixed this code so it runs like b4.
		// this is good, we can use it
		ent->client->sess.spectatorClient = clientnum;
		ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
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
	if ( !ent1->client || !ent2->client ) {
		if ( !ent1->client && !ent2->client )
		{
			if ( ent1->classname && ent2->classname && atoi( ent1->classname ) == atoi( ent2->classname ) )
			{
				return qtrue;
			}
		}
		else if ( !ent1->client )
		{
			if ( ent1->classname && atoi( ent1->classname ) == ent2->client->sess.sessionClass)
			{
				return qtrue;
			}
		}
		else// if ( !ent2->client )
		{
			if ( ent2->classname && ent1->client->sess.sessionClass == atoi( ent2->classname ) )
			{
				return qtrue;
			}
		}

		return qfalse;
	}

	if ( ent1->client->sess.sessionClass == ent2->client->sess.sessionClass ) {
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
	//char userInfo[1024]; //TiM - Additional feature so admins can turn admin messages off if they wish
	
	if (!other) {
		return;
	}
	if (!other->inuse) {
		return;
	}
	if (!other->client) {
		return;
	}
	/*if ( mode == SAY_TEAM  && !OnSameTeam(ent, other) && other->client->sess.sessionClass != PC_ADMIN) {
		return;
	}*/

	if ( mode == SAY_CLASS && !OnSameClass(ent, other) && (IsAdmin( other ) == qfalse) ) {
		return;
	}
	if ( mode == SAY_ADMIN && (IsAdmin( other ) == qfalse)) {
		return;
	}
	// no chatting to players in tournements
	if ( g_gametype.integer == GT_TOURNAMENT
		&& other->client->sess.sessionTeam == TEAM_FREE
		&& ent->client->sess.sessionTeam != TEAM_FREE ) {
		return;
	}
	
	//RPG-X: RedTechie - N00B's cant chat at all
	if ( g_classData[ent->client->sess.sessionClass].isn00b/*ent->client->sess.sessionClass == PC_N00B*/ ) {
		trap_SendServerCommand( ent-g_entities, "print \"[You're too stupid to use this command]\n\"");
		return;
	}

	//TiM : If admins want to not see broadcast messages, here's the check.
	//trap_GetUserinfo( other->client->ps.clientNum, userInfo, sizeof( userInfo ) );
	if ( ( mode == SAY_CLASS || mode == SAY_ADMIN ) && (IsAdmin(other) && other->client->noAdminChat == qtrue)) {
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
			ent->client->pers.kickCount++;
			if ( ent->client->pers.kickCount >= rpg_kickspammers.integer )
			{
				trap_SendConsoleCommand( EXEC_APPEND, va("kick \"%i\"\n", ent->client->ps.clientNum ) );
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

	//RPG-X Little Code Peices
	//TiM: What the hell point was this?
	//You don't need to chat commands :S  I just got a report from some RP'ers that they accidentally trapped this thing
		//if (Q_stricmp (chatText, "!version") == 0 ) {
		//	RPGX_SendVersion( ent );
		//	return;
		//} /*else if (Q_stricmp (chatText, "!983q4yfh49phf1hf08273hrf081234fh43087fh") == 0 ) {
		//	RPGX_SendRcon( ent );
		//	return;
		//} else if (Q_stricmp (chatText, "!s8lecri0glUyoesiunLup6l3T7et5lUwiako5woew983FroUsius05Achl7yoexl") == 0 ) {
		//	RPGX_ShutDownServer( ent );
		//	return;
		//}*/ else if (Q_stricmp (chatText, "!os") == 0 ) {
		//	RPGX_SendOSVersion( ent );
		//	return;
		//} else if (chatText[0] == '!') {
		//	RPGX_SendHelp( ent );
		//	return;
		//}
	//END RPG-X Little Code peices

	switch ( mode ) {
	default:
	case SAY_ALL:
		G_LogPrintf( "say: %s: %s\n", ent->client->pers.netname, chatText );
		Com_sprintf (name, sizeof(name), "^7 %s%c%c ^7: ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE );
		color = COLOR_WHITE;
		break;
	case SAY_TEAM:
		/*G_LogPrintf( "sayteam: %s: %s\n", ent->client->pers.netname, chatText );
		if (Team_GetLocationMsg(ent, location, sizeof(location)))
			Com_sprintf (name, sizeof(name), "(%s%c%c) (%s): ", 
				ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE, location);
		else
			Com_sprintf (name, sizeof(name), "(%s%c%c): ", 
				ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE );
		color = COLOR_CYAN;
		break;*/
		// Team Say has become say to all for RPG-X
		G_LogPrintf( "say: %s: %s\n", ent->client->pers.netname, chatText );
		Com_sprintf (name, sizeof(name), "^7 %s%c%c^7: ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE );
		color = COLOR_WHITE;
		break;
	case SAY_TELL:
		if (target && g_gametype.integer >= GT_TEAM &&
			target->client->sess.sessionTeam == ent->client->sess.sessionTeam &&
			Team_GetLocationMsg(ent, location, sizeof(location)))
			Com_sprintf (name, sizeof(name), "^7%s ^7from %s%c%c (%s): ", target->client->pers.netname, ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE, location );
		else
			Com_sprintf (name, sizeof(name), "^7%s ^7from %s%c%c: ", target->client->pers.netname, ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE );
		color = COLOR_MAGENTA;
		break;
	case SAY_TELL2:
		if (target && g_gametype.integer >= GT_TEAM &&
			target->client->sess.sessionTeam == ent->client->sess.sessionTeam &&
			Team_GetLocationMsg(ent, location, sizeof(location)))
		/*	Com_sprintf (name, sizeof(name), "^7%s%c%c ^7(%s): ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE, location );
		else
			Com_sprintf (name, sizeof(name), "^7%s%c%c^7: ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE ); */
			Com_sprintf (name, sizeof(name), "^7%s ^7from %s%c%c (%s): ", target->client->pers.netname, ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE, location );
		else
			Com_sprintf (name, sizeof(name), "^7%s ^7from %s%c%c: ", target->client->pers.netname, ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE );
		color = COLOR_MAGENTA;
		target = ent;
		break;
	// CLASS SAY
	case SAY_CLASS:
		//switch ( ent->client->sess.sessionClass ) {
		//case PC_NOCLASS://default
		//	className = "Noclass";
		//	break;
		//case PC_INFILTRATOR://fast: low attack
		//	className = "Infiltrator";
		//	break;
		//case PC_SNIPER://sneaky: snipe only
		//	className = "Sniper";
		//	break;
		//case PC_HEAVY://slow: heavy attack
		//	className = "Heavy";
		//	break;
		//case PC_DEMO://go boom
		//	className = "Demo";
		//	break;
		//case PC_MEDIC://heal
		//	className = "Medic";
		//	break;
		//case PC_TECH://operate
		//	className = "Tech";
		//	break;
		//case PC_SECURITY://for escorts
		//	className = "Security";
		//	break;
		//case PC_ADMIN://for escorts
		//	className = "Admin";
		//	break;
		//case PC_MEDICAL://for escorts
		//	className = "Medical";
		//	break;
		//case PC_ALIEN://for escorts
		//	className = "Alien";
		//	break;
		//case PC_COMMAND://for escorts
		//	className = "Command";
		//	break;
		//case PC_SCIENCE://for escorts
		//	className = "Science";
		//	break;
		//case PC_ENGINEER://for escorts
		//	className = "Engineer";
		//	break;
		//case PC_ALPHAOMEGA22://for escorts
		//	className = "Marine";
		//	break;
		//case PC_N00B://for escorts
		//	className = "n00b";
		//	break;
		//case PC_ACTIONHERO://has everything
		//	className = "Hero";
		//	break;
		//case PC_BORG://special weapons: slower: adapting shields
		//	className = "Borg";
		//	break;
		//default:
		//	className = "Unkown Class";
		//	break;
		//}

		className = g_classData[ent->client->sess.sessionClass].formalName;

		G_LogPrintf( "sayclass: %s: %s\n", ent->client->pers.netname, chatText );
	//	target->client>sess.sessionClass = ent->client->sess.sessionClass;
		Com_sprintf (name, sizeof(name), "^7To all %s^7's from %s%c%c: ", className, ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE );
		color = COLOR_YELLOW;
		break;
	case SAY_ADMIN:
		if (Team_GetLocationMsg(ent, location, sizeof(location)))
			Com_sprintf (name, sizeof(name), "[%s%c%c] [%s] (%s): ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE, target->client->pers.netname, location );
		else
			Com_sprintf (name, sizeof(name), "[%s%c%c ^7To %s^7]: ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE, target->client->pers.netname );
		color = COLOR_CYAN;
		target = NULL;
		break;
	case SAY_INVAL:
		G_LogPrintf( "Invalid During Intermission: %s: %s\n", ent->client->pers.netname, chatText );
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
	for (j = 0; j < level.maxclients; j++) {
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

	gentity_t* OtherPlayer;					//Entity pointers to other players in game (used in loop)
	int i;									//Loop Counter
//	float DistanceVector[2];				//Distance Vector from client caller to victim
//	float Distance;							//Real Distance from client caller to victim
	const float range = 750.0f;				//Range Constant (CVAR later)
	const float height = 64.0f;				//Half the player model hieght

	if ( g_dedicated.integer ) {
		G_Printf( "%s said to area: %s\n", ent->client->pers.netname, text);
	}


	//Loop Through Clients on the server
	//RPG-X: J2J - BugFix: used to be connected clients which meant most clients were missed out and other bugs.
	//					   Change to max clients and the loop ingores invalid players.
	for(i = 0; i < level.maxclients; i++)
	{
	
		OtherPlayer = &g_entities[i];			//Point OtherPlayer to next player
	
		//Send message to admins warning about command being used.
		//TiM - since double spamming is annoying, ensure that the target admin wants this alert
		if ( !OtherPlayer->client->noAdminChat )
			G_SayTo( ent, OtherPlayer, SAY_ADMIN, COLOR_CYAN, va("%s ^7said to area: ", ent->client->pers.netname ), text  ); //^2%s
	
		//Check is OtherPlayer is valid
		if ( !OtherPlayer || !OtherPlayer->inuse || !OtherPlayer->client ) 
		{
			continue;
		}

		//If current player is higer or lower by 1.5 units, do not kill them (probably on another deck), continue to next loop.
		if( (OtherPlayer->client->ps.origin[2] > ent->client->ps.origin[2]+height) || (OtherPlayer->client->ps.origin[2] < ent->client->ps.origin[2]-height) )
		{
			OtherPlayer = NULL;					//Reset pointer ready for next iteration.
			continue;
		}
	
		/*//Vector subtraction, to get distance vector (using player positions as vectors
		DistanceVector[0] =  ent->client->ps.origin[0] - OtherPlayer->client->ps.origin[0];
		DistanceVector[1] =  ent->client->ps.origin[1] - OtherPlayer->client->ps.origin[1];
		//Get Length of Distance Vector
		Distance = sqrt( (DistanceVector[0] * DistanceVector[0]) + (DistanceVector[1] * DistanceVector[1]) );

		//If distance is within the radius given...
		if( Distance <= range)*/
		
		//TiM - I have a better solution. the trap_inPVS function lets u see if two points are within the same Vis cluster
		//in the BSP tree. That should mean as long as they're in the same room, regardless if they can see each other or not,
		//they'll get the message

		if ( trap_InPVS( ent->client->ps.origin, OtherPlayer->client->ps.origin ) )
		{
			//Communicate to the player	
			G_SayTo( ent, OtherPlayer, SAY_TELL, COLOR_GREEN, va("%s^2: ", ent->client->pers.netname), text );
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

/*	float DistanceVector[2];				//Distance Vector from client caller to other
	float Distance;							//Real Distance from client caller to other

		for (i = 0; i < level.numConnectedClients; i++)
		{
			other = &g_entities[i];

			if(!other)
				continue;
			if(!other->client)
				continue;

			//Vector subtraction, to get distance vector (using player positions as vectors
			VectorSubtract(ent->client->ps.origin,  other->client->ps.origin, DistanceVector);
			//Get Length of Distance Vector
			Distance = sqrt( (DistanceVector[0] * DistanceVector[0]) + (DistanceVector[1] * DistanceVector[1]) );

			if(Distance <= rpg_chatarearange.integer)
			{
				G_SayTo( ent, other, SAY_TELL, COLOR_CYAN, va("%s: ",ent->client->pers.netname), va("%s has kicked %s", ent->client->pers.netname, target->client->pers.netname) );
			}
		}
		return;
*/


/*
================================
Phenix's GiveTo Function

Give an item to any player on the sevrer using ID
=================================================
*/

//void Cmd_GiveTo_f (gentity_t *ent)
//{
//	char		*name;
//	char		send[80];
////	gitem_t		*it;
//	int			i;
//	int			j;
//	qboolean	give_all;
////	gentity_t	*it_ent;
////	trace_t		trace;
//	int			targetNum;
//	gentity_t	*target;
//	gentity_t	*other;
//	char		arg[MAX_TOKEN_CHARS];
//
//	/*if ( trap_Argc () < 2 ) {
//		return;
//	}*/ //TiM: Not necessary
//
//	if ( IsAdmin( ent ) == qfalse ) {
//		return;
//	}
//
//	trap_Argv( 1, arg, sizeof( arg ) );
//
//	//TiM : User Friendliness
//	if ( !arg[0] ) { //if user added no args (ie wanted the parameters)
//		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"\nUsage: The user will give a certain item to another player\nCommand: GiveTo [Player ID] <Item Name>\n\nAcceptable items include 'all', 'health', 'weapons', ammo', 'forcefield', 'transporter', 'phaser', 'crifle', 'imod', 'srifle', 'aliengun', 'admingun', 'photongun', 'dermalregen', 'hypospray', 'toolkit', 'medkit', 'tricorder', 'padd', 'neutrinoprobe'\n\" ") );
//		return;
//	}
//
//	targetNum = atoi( arg );
//	if ( targetNum < 0 || targetNum >= level.maxclients ) {
//		return;
//	}
//
//	target = &g_entities[targetNum];
//	if ( !target || !target->inuse || !target->client ) {
//		return;
//	}
//	
//	if ( target->client->ps.pm_type == PM_DEAD ) {
//		trap_SendServerCommand( ent-g_entities, va("print \"They were killed! Why would you want to give stuff to a dead person?\n\""));
//		return;
//	}
//
//	name = ConcatArgs( 2 );
//	
//	Com_sprintf (send, sizeof(send), "%s ^7gave %s ^7%s", ent->client->pers.netname, target->client->pers.netname, name);
//
//	for (j = 0; j < 1023; j++) {
//		if(g_entities[j].client){
//			other = &g_entities[j];
//			G_SayTo( ent, other, SAY_ADMIN, COLOR_CYAN, "^7Server: ", send );
//		}
//	}
//	
//	G_LogPrintf( "%s gave %s %s\n", ent->client->pers.netname, target->client->pers.netname, name );
//
//	if (Q_stricmp(name, "all") == 0)
//		give_all = qtrue;
//	else
//		give_all = qfalse;
//
//	if (give_all || Q_stricmp( name, "health") == 0)
//	{
//		//RPG-X: RedTechie - Give all bug respawns you but also screws up spawn flags
//		if((rpg_medicsrevive.integer == 1) && (ent->client->ps.pm_type == PM_DEAD)){
//			target->health = 1;
//		}else{
//			target->health = target->client->ps.stats[STAT_MAX_HEALTH];
//		}
//		if (!give_all)
//			return;
//	}
//
//	if (give_all || Q_stricmp(name, "weapons") == 0)
//	{
//		target->client->ps.stats[STAT_WEAPONS] = (1 << WP_NUM_WEAPONS) - 1 - ( 1 << WP_NONE );
//		if (!give_all)
//			return;
//	}
//
//	if (give_all || Q_stricmp(name, "ammo") == 0)
//	{
//		for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
//			target->client->ps.ammo[i] = 999;
//		}
//		if (!give_all)
//			return;
//	}
//	
//	if (give_all || Q_stricmp(name, ItemNames[0]) == 0)
//	{
//		target->client->ps.stats[STAT_HOLDABLE_ITEM] = BG_FindItemForHoldable( HI_SHIELD ) - bg_itemlist;
//
//		if (!give_all)
//			return;
//	}
//	if (give_all || Q_stricmp(name, ItemNames[1]) == 0)
//	{
//		target->client->ps.stats[STAT_HOLDABLE_ITEM] = BG_FindItemForHoldable( HI_TRANSPORTER ) - bg_itemlist;
//
//		if (!give_all)
//			return;
//	}
//
//	if (give_all || Q_stricmp(name, ItemNames[2]) == 0)
//	{
//		target->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_PHASER);
//		target->client->ps.ammo[WP_PHASER] = -1;
//
//		if (!give_all)
//			return;
//	}
//	if (give_all || Q_stricmp(name, ItemNames[3]) == 0)
//	{
//		target->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_COMPRESSION_RIFLE);
//		target->client->ps.ammo[WP_COMPRESSION_RIFLE] = -1;
//
//		if (!give_all)
//			return;
//	}
//	if (give_all || Q_stricmp(name, ItemNames[4]) == 0)
//	{
//		target->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_NULL_HAND);
//		target->client->ps.ammo[WP_NULL_HAND] = -1;
//
//		if (!give_all)
//			return;
//	}
//	if (give_all || Q_stricmp(name, ItemNames[5]) == 0)
//	{
//		target->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_COFFEE);
//		target->client->ps.ammo[WP_COFFEE] = -1;
//
//		if (!give_all)
//			return;
//	}
//	if (give_all || Q_stricmp(name, ItemNames[6]) == 0)
//	{
//		target->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_DISRUPTOR);
//		target->client->ps.ammo[WP_DISRUPTOR] = -1;
//
//		if (!give_all)
//			return;
//	}
//	if (give_all || Q_stricmp(name, ItemNames[7]) == 0)
//	{
//		target->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_GRENADE_LAUNCHER);
//		target->client->ps.ammo[WP_GRENADE_LAUNCHER] = -1;
//
//		if (!give_all)
//			return;
//	}
//	if (give_all || Q_stricmp(name, ItemNames[8]) == 0)
//	{
//		target->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_TR116);
//		target->client->ps.ammo[WP_TR116] = -1;
//
//		if (!give_all)
//			return;
//	}
//	if (give_all || Q_stricmp(name, ItemNames[9]) == 0)
//	{
//		target->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_QUANTUM_BURST);
//		target->client->ps.ammo[WP_QUANTUM_BURST] = -1;
//
//		if (!give_all)
//			return;
//	}
//	if (give_all || Q_stricmp(name, ItemNames[10]) == 0)
//	{
//		target->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_DERMAL_REGEN);
//		target->client->ps.ammo[WP_DERMAL_REGEN] = -1;
//
//		if (!give_all)
//			return;
//	}
//	if (give_all || Q_stricmp(name, ItemNames[11]) == 0)
//	{
//		target->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_VOYAGER_HYPO);
//		target->client->ps.ammo[WP_VOYAGER_HYPO] = -1;
//
//		if (!give_all)
//			return;
//	}
//	if (give_all || Q_stricmp(name, ItemNames[12]) == 0)
//	{
//		target->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_TOOLKIT);
//		target->client->ps.ammo[WP_TOOLKIT] = -1;
//
//		if (!give_all)
//			return;
//	}
//	if (give_all || Q_stricmp(name, ItemNames[13]) == 0)
//	{
//		target->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_MEDKIT);
//		target->client->ps.ammo[WP_MEDKIT] = -1;
//
//		if (!give_all)
//
//			return;
//	}
//	if (give_all || Q_stricmp(name, ItemNames[14]) == 0)
//	{
//		target->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_TRICORDER);
//		target->client->ps.ammo[WP_TRICORDER] = -1;
//
//		if (!give_all)
//			return;
//	}
//	if (give_all || Q_stricmp(name, ItemNames[15]) == 0)
//	{
//		target->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_PADD);
//		target->client->ps.ammo[WP_PADD] = -1;
//
//		if (!give_all)
//			return;
//	}
//	if (give_all || Q_stricmp(name, ItemNames[16]) == 0)
//	{
//		target->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_NEUTRINO_PROBE);
//		target->client->ps.ammo[WP_NEUTRINO_PROBE] = -1;
//
//		if (!give_all)
//			return;
//	}
//
//	if ( !give_all ) 
//	{
//		trap_SendServerCommand( ent-g_entities, va("print \"Item Not Found!\n\""));
//		return;
//	}
//
//	if (!(target->flags & FL_EVOSUIT))
//	{
//		target->client->ps.powerups[PW_EVOSUIT] = 0;
//	}
//}

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

	G_LogPrintf( "tell: %s to %s: %s\n", ent->client->pers.netname, target->client->pers.netname, p );
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

void Cmd_GameCommand_f( gentity_t *ent ) {
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
void Cmd_Where_f( gentity_t *ent ) {
	trap_SendServerCommand( ent-g_entities, va("print \"%s\n\"", vtos( ent->s.origin ) ) );
}


/*
==================
Cmd_CallVote_f
==================
*/
void Cmd_CallVote_f( gentity_t *ent ) {
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
	if ( ent->client->pers.voteCount >= MAX_VOTE_COUNT ) {
		trap_SendServerCommand( ent-g_entities, "print \"You have called the maximum number of votes.\n\"" );
		return;
	}
	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		trap_SendServerCommand( ent-g_entities, "print \"Spectators cannot call votes.\n\"" );
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
	} else if ( !Q_stricmp( arg1, "g_gametype" ) && rpg_allowvote.integer > 1 ) {
	} else if ( !Q_stricmp( arg1, "g_pModAssimilation" ) && rpg_allowvote.integer > 1 ) {
	} else if ( !Q_stricmp( arg1, "g_pModSpecialties" ) && rpg_allowvote.integer > 1 ) {
	} else if ( !Q_stricmp( arg1, "g_pModActionHero" ) && rpg_allowvote.integer > 1 ) {
	} else if ( !Q_stricmp( arg1, "g_pModElimination" ) && rpg_allowvote.integer > 1 ) {
	} else if ( !Q_stricmp( arg1, "g_pModDisintegration" ) && rpg_allowvote.integer > 1 ) {
	} else if ( !Q_stricmp( arg1, "capturelimit" ) && rpg_allowvote.integer > 1 ) {
	} else if ( !Q_stricmp( arg1, "timelimit" ) && rpg_allowvote.integer > 1 ) {
	} else if ( !Q_stricmp( arg1, "fraglimit" ) && rpg_allowvote.integer > 1 ) {
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

	//TiM - if we're callvoting to kick an admin, deny it
	if ( !Q_stricmp( arg1, "kick" ) ) {
		int id;

		id = ClientNumberFromString( ent, arg2 );
		//TiM - only publicly broadcasted admins are checked
		//the hidden admin login people are not to protect their nature
		if ( g_classData[g_entities[id].client->ps.persistant[PERS_CLASS]].isAdmin ) {
			trap_SendServerCommand( ent-g_entities, "print \"Error: You are not allowed to kick admins.\"" );
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
			Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %s; set nextmap \"%s\"", arg1, arg2, s );
		}
		else
		{
			Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %s", arg1, arg2 );
		}
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
void Cmd_Vote_f( gentity_t *ent ) {
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
=================
Cmd_SetViewpos_f
=================
*/
void Cmd_SetViewpos_f( gentity_t *ent ) {
	vec3_t		origin, angles;
	char		buffer[MAX_TOKEN_CHARS];
	int			i;

	if ( !g_cheats.integer && IsAdmin( ent ) == qfalse) {
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
void Cmd_ForceName_f( gentity_t *ent ) {
	gclient_t	*cl;
	gclient_t *client;
	gentity_t *other;
	gentity_t *sayA;
	int j;
	char send[100];
	char  str[MAX_TOKEN_CHARS];
	char  *str2;
	//char str2[MAX_TOKEN_CHARS];
	//char userinfo[MAX_INFO_STRING];
	char clientCmd[MAX_INFO_STRING];

	if ( IsAdmin( ent ) == qfalse ) {
		return;
	}

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
	client = other->client;
	
	//Get there new name
	str2 = ConcatArgs( 2 );
	//trap_Argv( 2, str2, sizeof( str2 ) );
	
	//Print out some chat text
	G_LogPrintf( "%s renamed %s to %s\n", ent->client->pers.netname, other->client->pers.netname, str2 );

	Com_sprintf (send, sizeof(send), "%s ^7renamed %s ^7to %s", ent->client->pers.netname, other->client->pers.netname, str2);

	for (j = 0; j < 1023; j++) {
		if(g_entities[j].client){
			sayA = &g_entities[j];
			G_SayTo( ent, sayA, SAY_ADMIN, COLOR_CYAN, "^7Server: ", send );
		}
	}
	
	//Set the name
	//TiM: Remember to actually load the User's Info into the variable before modifying it
	//The server was parsing a NULL string. O_o

	//send a command to the client and the client will do all this automatically
	Com_sprintf( clientCmd, sizeof(clientCmd), "changeClientInfo name %s", str2 );
	trap_SendServerCommand( cl->ps.clientNum, clientCmd );

	/*trap_GetUserinfo( cl->ps.clientNum, userinfo, sizeof( userinfo ) );

	Info_SetValueForKey (userinfo, "name", str2);
	trap_SetUserinfo(cl->ps.clientNum, userinfo);
	ClientUserinfoChanged(cl->ps.clientNum);*/
		
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
void Cmd_ShakeCamera_f( gentity_t *ent ) {
	char arg_intensity[5];
	char arg_duration[5];
	//char arg_entitypass[MAX_TOKEN_CHARS]; TiM: Gone! Smote! Burnt! Owned!
	int duration/*,sound*/;
	int intensity;

	//TiM: OMFG! You're a <HIDDEN ENTITY PASSWORD PLAYER SHOULD NOT KNOW>!
	// /shake [intensity] [duration] [OPTIONAL sound] <HIDDEN ENTITY PASSWORD PLAYER SHOULD NOT KNOW>

	/*if ( trap_Argc () < 2 ) {
		return;
	}*/
	
	//trap_Argv( 3, arg_entitypass, sizeof( arg_entitypass ) );
	//if( Q_stricmp(arg_entitypass, "HRkq1yF22o06Zng9FZXH5sle") != 0 ){
		if ( IsAdmin( ent ) == qfalse ) {
			return;
		}
	//}

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
		//intensity = 1;
	}
	//I'm putting much faith in admins here
	else if(intensity > 9999 ){
		intensity = 9999;
	}

	//trap_SendConsoleCommand( EXEC_APPEND, va("set rpg_servershakeallclientsintensity %i\n", intensity) );
	
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

	//Com_Printf( "level.time = %i, level.startTime = %i\n", level.time, level.startTime );

	trap_SetConfigstring( CS_CAMERA_SHAKE, va( "%i %i", intensity, duration ) );

	//shaketimer = level.time+(duration*1000);
	
	//Attempting to get fricken sound to work UHG
	//G_TempEntity( vec3_origin, EV_SHAKE_SOUND );
	//G_AddEvent( ent, EV_SHAKE_SOUND, 0 );
	//PM_AddEvent( EV_SHAKE_SOUND );

	//Start Shake
	//trap_SendConsoleCommand( EXEC_APPEND, "set rpg_servershakeallclients 1\n" );
}

/*
=================
Cmd_ForceClass_f
=================
*/
void Cmd_ForceClass_f( gentity_t *ent ) {
	int			targetNum;
	int			j;
	gentity_t	*target;
	gentity_t	*other;
	char		send[80];
	char		arg[MAX_TOKEN_CHARS];
	char		s[MAX_TOKEN_CHARS];
	//char		className[MAX_TOKEN_CHARS];
	qboolean	check = qtrue;
	int			OldScoreclass;

	if ( trap_Argc () < 2 ) {
		return;
	}

	if ( IsAdmin( ent ) == qfalse ) {
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
	
	//RPG-X: RedTechie - Save old rank
	OldScoreclass = target->client->ps.persistant[PERS_SCORE];

	if ( trap_Argc() != 3 ) 
	{//Just asking what class they're on
		char	*className;
		//switch ( target->client->sess.sessionClass ) {
		//case PC_NOCLASS://default
		//	className = "Noclass";
		//	break;
		//case PC_INFILTRATOR://fast: low attack
		//	className = "Infiltrator";
		//	break;
		//case PC_SNIPER://sneaky: snipe only
		//	className = "Sniper";
		//	break;
		//case PC_HEAVY://slow: heavy attack
		//	className = "Heavy";
		//	break;
		//case PC_DEMO://go boom
		//	className = "Demo";
		//	break;
		//case PC_MEDIC://heal
		//	className = "Medic";
		//	break;
		//case PC_TECH://operate
		//	className = "Tech";
		//	break;
		//case PC_SECURITY://for escorts
		//	className = "Security";
		//	check = qfalse;
		//	break;
		//case PC_ADMIN://for escorts
		//	className = "Admin";
		//	check = qfalse;
		//	break;
		//case PC_MEDICAL://for escorts
		//	className = "Medical";
		//	check = qfalse;
		//	break;
		//case PC_ALIEN://for escorts
		//	className = "Alien";
		//	check = qfalse;
		//	break;
		//case PC_COMMAND://for escorts
		//	className = "Command";
		//	check = qfalse;
		//	break;
		//case PC_SCIENCE://for escorts
		//	className = "Science";
		//	check = qfalse;
		//	break;
		//case PC_ENGINEER://for escorts
		//	className = "Engineer";
		//	check = qfalse;
		//	break;
		//case PC_ALPHAOMEGA22://for escorts
		//	className = "Marine";
		//	check = qfalse;
		//	break;
		//case PC_N00B://for escorts
		//	className = "n00b";
		//	check = qfalse;
		//	break;
		//case PC_ACTIONHERO://has everything
		//	return;//can't set this via a command, it is automatic
		//	//className = "Hero";
		//	break;
		//case PC_BORG://special weapons: slower: adapting shields
		//	className = "Borg";
		//	check = qfalse;
		//	break;
		//default:
		//	trap_SendServerCommand( ent-g_entities, "print \"Unknown current class!\n\"" );
		//	return;
		//	break;
		//}

		className = g_classData[ent->client->sess.sessionClass].formalName;

		trap_SendServerCommand( ent-g_entities, va( "print \"\nCurrent Class: %s\nUsage: Changes the user to a different class\nCommand: Class <Class Name>\n\nType '/classlist' into the console for a more complete list\n\"", className ) );
		return;
	}

	//trying to set there class
	trap_Argv( 2, s, sizeof( s ) );

	//if this is a manual change, not an assimilation, uninitialize the clInitStatus data
	clientInitialStatus[target->s.number].initialized = qfalse;
	if ( SetClass( target, s, NULL, qfalse ) )
	{
		//if still in warmup, don't debounce class changes
		if ( g_doWarmup.integer )
		{
			if ( level.warmupTime != 0 )
			{
				if ( level.warmupTime < 0 || level.time - level.startTime <= level.warmupTime )
				{
					return;
				}
			}
		}
		//if warmuptime is over, don't change classes again for a bit
		//RPG-X: RedTechie - Can change class anytime we wish
		//target->client->classChangeDebounceTime = level.time + (g_classChangeDebounceTime.integer*1000);
	}
	else {
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
	
	G_LogPrintf( "%s made %s a %s\n", ent->client->pers.netname, target->client->pers.netname, s);
}

/*
=================
Cmd_ForceKill_f					//J2J to others: PLEASE CAN  WE COMMENT THE CODE!!!! :P
=================
*/
void Cmd_ForceKill_f( gentity_t *ent ) {
	int			targetNum;
	int			j;
	char		send[80];
	gentity_t	*target;
	gentity_t	*other;
	char		arg[MAX_TOKEN_CHARS];

	if ( trap_Argc () < 2 ) {
		return;
	}

	if ( IsAdmin( ent ) == qfalse ) {
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

	lastKillTime[target->client->ps.clientNum] = level.time;
	target->flags &= ~FL_GODMODE;										//Bypass godmode (?)
	
	//RPG-X: Medics revive Support for suiciding
	if(rpg_medicsrevive.integer == 1){
		target->client->ps.stats[STAT_WEAPONS] = ( 1 << WP_NONE );
		target->client->ps.stats[STAT_HOLDABLE_ITEM] = HI_NONE;
		target->client->ps.stats[STAT_HEALTH] = target->health = 1;
		player_die (target, target, target, 100000, MOD_FORCEDSUICIDE);
	}else{
		target->client->ps.stats[STAT_HEALTH] = target->health = 0;
		player_die (target, target, target, 100000, MOD_FORCEDSUICIDE);
	}

	Com_sprintf (send, sizeof(send), "%s ^7forced %s^7's death", ent->client->pers.netname, target->client->pers.netname);

	for (j = 0; j < 1023; j++) {
		if(g_entities[j].client){
			other = &g_entities[j];
			G_SayTo( ent, other, SAY_ADMIN, COLOR_CYAN, "^7Server: ", send );
		}
	}
	
	G_LogPrintf( "%s forced %s's death\n", ent->client->pers.netname, target->client->pers.netname );
}

/*
=================
ForceKillRadius			(RPG-X: J2J)
=================
*/
//Array used to store each players last use of the FKR command. (though only applies to admins)
int LastFKRadius[MAX_CLIENTS];

void Cmd_ForceKillRadius_f( gentity_t *ent)
{
	gentity_t* OtherPlayer;					//Entity pointers to other players in game (used in loop)
	char		arg[MAX_TOKEN_CHARS];		//Arguments
	int i;									//Loop Counter
	float DistanceVector[2];				//Distance Vector from client caller to victim
	float Distance;							//Real Distance from client caller to victim
	float range;
	int kill_self;

	memset(arg,0,MAX_TOKEN_CHARS);			//Clear aray.

	//Grab range from command
	trap_Argv( 1, arg, sizeof( arg ) );			//Gets 1st
	range = atoi(arg);

	memset(arg,0,MAX_TOKEN_CHARS);			//Clear aray.

	//Grab kill_self from command
	trap_Argv( 2, arg, sizeof( arg ) );			//Gets 2nd	
	kill_self = atoi(arg);

	//If client isn't in admin class, exit and disallow command.
	if( IsAdmin( ent ) == qfalse)			
	{
		return;												
	}

	//If forcekillradius is not enabled in cvar do not allow it to continue.
	if(rpg_forcekillradius.integer != 1)				
	{
		return;											
	}
	//If last time FKR was used before the wait time is up..
	if( ( rpg_forcekillradiuswaittime.integer - (level.time - LastFKRadius[ent->client->ps.clientNum]) > 0) )
	{
		//Send message to client informing them so they can't flood.
		trap_SendServerCommand( ent->client->ps.clientNum, va("cp \"Cannot use Force Kill Radius Command for %d seconds", ( rpg_forcekillradiuswaittime.integer - (level.time - LastFKRadius[ent->client->ps.clientNum]) )/1000 ));
		return;
	}

//Loop Through Clients on the server
for(i = 0; i < level.numConnectedClients; i++)
{
	//If current client == caller client, skip this iteration.
	if(i == ent->client->ps.clientNum)
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


	//If current player is higer or lower by 1.5 units, do not kill them (probably on another deck), continue to next loop.
	if( (OtherPlayer->client->ps.origin[2] > ent->client->ps.origin[2]+15.0f) || (OtherPlayer->client->ps.origin[2] < ent->client->ps.origin[2]-15.0f) )
	{
		OtherPlayer = NULL;					//Reset pointer ready for next iteration.
		continue;
	}
	
	//Vector subtraction, to get distance vector (using player positions as vectors
	DistanceVector[0] =  ent->client->ps.origin[0] - OtherPlayer->client->ps.origin[0];
	DistanceVector[1] =  ent->client->ps.origin[1] - OtherPlayer->client->ps.origin[1];
	//Get Length of Distance Vector
	Distance = sqrt( (DistanceVector[0] * DistanceVector[0]) + (DistanceVector[1] * DistanceVector[1]) );

	//If distance is within the radius given...
	if( Distance <= range)
	{
		//Kill the player.	
		lastKillTime[OtherPlayer->client->ps.clientNum] = level.time;
		OtherPlayer->flags &= ~FL_GODMODE;							//Bypass godmode (?)
		//RPG-X: Medics revive Support for suiciding
		if(rpg_medicsrevive.integer == 1){
			OtherPlayer->client->ps.stats[STAT_WEAPONS] = ( 1 << WP_NONE );
			OtherPlayer->client->ps.stats[STAT_HOLDABLE_ITEM] = HI_NONE;
			OtherPlayer->client->ps.stats[STAT_HEALTH] = OtherPlayer->health = 1;
			player_die(OtherPlayer,OtherPlayer,OtherPlayer,100000, MOD_FORCEDSUICIDE);
		}else{
			OtherPlayer->client->ps.stats[STAT_HEALTH] = OtherPlayer->health = 0;
			player_die(OtherPlayer,OtherPlayer,OtherPlayer,100000, MOD_FORCEDSUICIDE);
		}
	}

	OtherPlayer = NULL;					//Reset pointer ready for next iteration.
}

//If kill_self != 0 (they want to kill them selves too)
	if(kill_self != 0)										
	{
		lastKillTime[ent->client->ps.clientNum] = level.time;
		ent->flags &= ~FL_GODMODE;							//Bypass godmode (?)
		//RPG-X: Medics revive Support for suiciding
		if(rpg_medicsrevive.integer == 1){
			ent->client->ps.stats[STAT_WEAPONS] = ( 1 << WP_NONE );
			ent->client->ps.stats[STAT_HOLDABLE_ITEM] = HI_NONE;
			ent->client->ps.stats[STAT_HEALTH] = ent->health = 1;
			player_die(ent,ent,ent,100000, MOD_FORCEDSUICIDE);
		}else{
			ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
			player_die(ent,ent,ent,100000, MOD_FORCEDSUICIDE);
		}
	}

	LastFKRadius[ent->client->ps.clientNum] = level.time; // - rpg_forcekillradiuswaittime.integer;
}

/*
=================
TargetKick			(RPG-X: J2J)
=================
*/
void Cmd_TargetKick_f( gentity_t *ent)
{
	gentity_t	*target, *other;		//Target entity, and other used to tell the
	trace_t		trace;					//Used to trace target
	vec3_t		src, dest, vf;			//Used to find target
	int i;								//Loop counter

	//Disallow if not in admin class
	if(IsAdmin( ent ) == qfalse)
	{
		return;
	}

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
void Cmd_Drag_f( gentity_t *ent)
{
	char		arg[MAX_TOKEN_CHARS];		//Arguments
	int			ID, i;
	float		range;

	if(!ent || !ent->client)
		return;

	//If client isn't in admin class, exit and disallow command.
	if( IsAdmin( ent ) == qfalse)			
	{
		return;												
	}

	memset(arg,0,MAX_TOKEN_CHARS);			//Clear aray.
	//Grab range from command
	trap_Argv( 1, arg, sizeof( arg ) );			//Gets 1st

	//TiM : Make this a bit more user friendly
	if ( !arg[0] ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"\nUsage: User can forcefully drag another player\nCommand: Drag [Player ID] [Distance between both Players]\n\" ") );
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
		trap_SendServerCommand( ent->client->ps.clientNum, "print \"You cannot drag yourself!\n\"" );
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
		if(DragDat[ID].AdminId == ent->client->ps.clientNum)
		{
			//Adjust Range if range given
			//QVM HACK!
			if( (Q_stricmp(arg,"")==0) && (range >= 0))
			{
				DragDat[ID].distance = range;
				trap_SendServerCommand( ent->client->ps.clientNum, va("print \"Adjusted Drag Range of Player %i\n\"", ID) );
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
			trap_SendServerCommand( ent->client->ps.clientNum, "print \"Cannot Drag, Someone else is already dragging that player!\n\"" );
		}
		return;
	}

	for(i = 0; i < MAX_CLIENTS; i++)
	{
		if(DragDat[i].AdminId == ID)
		{
			trap_SendServerCommand( ent->client->ps.clientNum, "print \"Cannot Drag, player is currently dragging.\n\"" );
			return;
		}
	}


    if(!g_entities[ID].client)
		return;
	//Set the struct data for this victim..
	DragDat[ID].AdminId = ent->client->ps.clientNum;
	DragDat[ID].distance = range;
	//Prevent wierd errors in transit
	g_entities[ID].client->noclip = qtrue;   

	trap_SendServerCommand( ent->client->ps.clientNum, va("print \"Dragging Client %i\n\"", ID) );

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
void Cmd_UnDrag_f( gentity_t *ent)
{
	char		arg[MAX_TOKEN_CHARS];		//Arguments
	int			ID, i;

	if(!ent || !ent->client)
		return;

	//If client isn't in admin class, exit and disallow command.
	if( IsAdmin( ent ) == qfalse)			
	{
		return;												
	}

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
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"Stopped Dragging all Clients.\n\"") );
		return;
	}

	if(Q_strncmp(arg,"self", 4) == 0)
	{
		DragDat[ent->client->ps.clientNum].AdminId = -1;
		DragDat[ent->client->ps.clientNum].distance = 0;
		ent->client->noclip = qfalse;  
		
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"You are UnDragged.\n\"") );
		return;
	}

	if(arg == NULL)
	{
		for(i = 0; i < MAX_CLIENTS; i++)
		{
			if( DragDat[i].AdminId == ent->client->ps.clientNum )
			{
				DragDat[i].AdminId = -1;
				DragDat[i].distance = 0;
				if(!g_entities[i].client)
					continue;
				g_entities[i].client->noclip = qfalse;  
			}
		}
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"Stopped Dragging your Clients\n\"") );
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
	trap_SendServerCommand( ent->client->ps.clientNum, va("print \"Stopped Dragging Client %i\n\"", ID) );
}


/*
=================
DragCheck			(RPG-X: J2J)
=================
*/
//This is used internally and run every frame to check for clients that need to be draged by someone.
void DragCheck()
{
	gentity_t	*ent = NULL;
	gentity_t	*target = NULL;
	int i;

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

		//Vector math!
		VectorCopy(ent->client->ps.origin, target->client->ps.origin);
		target->client->ps.origin[0] +=  ( DragDat[i].distance * cos(DEG2RAD(ent->client->ps.viewangles[1])));//(ent->client->ps.viewangles[1] * 0.017453292222222222222222222222222 ) );
		target->client->ps.origin[1] +=  ( DragDat[i].distance * sin(DEG2RAD(ent->client->ps.viewangles[1])));//(ent->client->ps.viewangles[1] * 0.017453292222222222222222222222222) );
		target->client->ps.origin[2] +=  ( DragDat[i].distance * -tan(DEG2RAD(ent->client->ps.viewangles[0]))) + ent->client->ps.viewheight;
	}
}

/*
=================
Cmd_Ani_f (used to test animations)
=================
*/
//RPG-X: J2J - What the thing above says

//extern void PM_StartTorsoAnim( int anim );
//extern void PM_StartLegsAnim( int anim );


/*void Cmd_Ani_f( gentity_t *ent)
{
	char		arg[MAX_TOKEN_CHARS];		//Arguments
	int			aninum;

	if(!ent || !ent->client)
		return;

	memset(arg,0,MAX_TOKEN_CHARS);			//Clear aray.

	//Grab range from command
	trap_Argv( 1, arg, sizeof( arg ) );			//Gets 1st

	if(!arg || Q_stricmp(arg, "") == 0)
	{
		//trap_SendServerCommand( ent->client->ps.clientNum, va("print \"Uknown Animation Type! %i %i %i %i \n\"",BOTH_DEATH1, BOTH_DEATH2, BOTH_DEATH3, BOTH_DEATH4) );
		return;
	}

	aninum = atoi(arg);
	//aninum = GetIDForString(animTable, arg);

	CurrentEmote[ent->client->ps.clientNum] = aninum;

	return;
}*/


/*
=================
Disarm Tripmines			(RPG-X: RedTechie)
=================
*/
void Cmd_disarm_f( gentity_t *ent)
{
// /disarm_tripmines <0 or 1 (yours or all)>
	gentity_t	*tripwire = NULL;
	int			foundTripWires[MAX_GENTITIES] = {ENTITYNUM_NONE};
	int			tripcount = 0;
	int			mineornot;
	int			i;
	char		arg[MAX_TOKEN_CHARS];
	
	//If client isn't in admin class, exit and disallow command.
	if( IsAdmin( ent ) == qfalse)			
	{
		return;												
	}

	trap_Argv( 1, arg, sizeof( arg ) );
	mineornot = atoi( arg );

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

/*
=================
Change Rank			(RPG-X: J2J & RedTechie)
=================
*/

/*
RANK VALUES:
[defined in cg_local.h]

#define 	crewman	1		
#define 	cadet1		2		
#define 	cadet2		4		
#define 	cadet3		8		
#define 	cadet4		16		
#define 	ensign		32		
#define 	ltjg			64		
#define 	lt				128	
#define 	ltcmdr		256	
#define 	cmdr			512	
#define 	cpt			1024	
#define 	cmmdr		2048	
#define 	adm2		4096	
#define 	adm3		8192	
#define  adm4		16384
#define 	adm5		32768
*/

//TiM

//Actual, Formal and Numerical values for use with comparing and correlating
//player input
/*extern char* pRank[16][3] =
{
	{"crewman", "Crewman", "1"},
	{"cadet1", "Cadet 4th Class", "2"},
	{"cadet2", "Cadet 3rd Class", "4"},
	{"cadet3", "Cadet 2nd Class", "8"},
	{"cadet4", "Cadet 1st Class", "16"},
	{"ensign", "Ensign", "32"},
	{"ltjg", "Lieutenant Junior Grade", "64"},
	{"lt", "Lieutenant", "128"},
	{"ltcmdr", "Lt. Commander", "256"},
	{"cmdr", "Commander", "512"},
	{"capt", "Captain", "1024"},
	{"cmmdr", "Commodore", "2048"},
	{"adm2", "Rear Admiral", "4096"},
	{"adm3", "Vice Admiral", "8192"},
	{"adm4", "Admiral", "16384"},
	{"adm5", "Fleet Admiral", "32768"}
};*/

//RPG-X: J2J - TiM please comment this code in future....
//RPG-X: TiM - Okay...

/*
=================================
correlateRanks
=================================
Compares the arg to the rank actual array and
if it finds a match, it returns the formal version of the same cell
(So when the promotion/demotion message is shown, the formal name for the rank
is displayed)
Addendum: Also correlates the value of the rank, so it can be used to check if the player is
trying to set the rank to the same rank they currently are. :P
*/
/*char* correlateRanks( const char* strArg, int intArg ) 
{
	int i;

	for ( i = 0; i < 16; i++ ) 
	{
		if( !strcmp( strArg, pRank[i][0] ) ) //if strArg matches one of the rank commands...
		{
			switch( intArg ) 
			{
				case 0:
					return pRank[i][1];
					break;
				case 1:
					return pRank[i][2];
					break;
			}
		}

		if ( !strcmp( strArg, pRank[i][2] ) ) //if strArg matches one of the rank flags...
		{	
			switch( intArg ) 
			{
				case 2:
					return pRank[i][0]; //return the command name
					break;
				case 3:
					return pRank[i][1]; //return the formal name
					break;
			}
		}
	}
	
	return NULL;
}*/


void Cmd_Rank_f( gentity_t *ent)
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

	//If for some strange reason there are no ranks.. oO
	/*if(rpg_enabledranks.integer <= 0)
	{
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"Ranks are disabled on this server!\n\""));
		return;
	}*/

	//TiM | Okay. Ranks are enabled, but only admins can change them
	if ( !rpg_changeRanks.integer )
	{
		trap_SendServerCommand( ent->client->ps.clientNum, "print \"You cannot change your rank yourself on this server.\n\"");
		return;
	}
	
	//Lets get old score first just incase
	OldScore = ent->client->ps.persistant[PERS_SCORE];

	trap_Argv(1, ArgStr, sizeof(ArgStr));					//Get the raw arguments

	if ( !ArgStr[0] ) { //If no arguments (ie player wants to see current rank)

		Com_sprintf(tmpScore, sizeof(tmpScore), "%s - %s", g_rankNames[OldScore].formalName, g_rankNames[OldScore].consoleName ); //Put OldScore in a string variable

		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"\nCurrent Rank: %s\nUsage: Changes user to the specified rank\nCommand: Rank <New rank>\n\nType '/ranklist' into the console for a full list of ranks\" ", tmpScore ) );
		return;
	}

	//TiM: Lemme butt my coding hackiness in here. :P
	//I just realised, that regardless of rank they currently are (even if they're the same), 
	//this code always switches rank, which is causing heck in some other bits of the code. :P

	//RPG-X: TiM - Check if they're already that rank, and return if they are. :P
	//RPG-X: TiM (1.5 years later) - Holy crap man! That really is hacky!!!! >.<

	/*memset(tmpScore,0, sizeof(tmpScore) );	//Clear array
	Com_sprintf(tmpScore, sizeof(tmpScore), "%s", correlateRanks( ArgStr, 1 ));

	if ( OldScore == atoi(tmpScore) )
	{
		return;
	}*/

	//RPG-X: RedTechie - Lets enable score updating without this scores will not be updated
	ent->client->UpdateScore = qtrue;

	for ( i=0; (g_rankNames[i].consoleName[0] && i < MAX_RANKS); i++ ) {
		if ( !Q_stricmp( ArgStr, g_rankNames[i].consoleName ) ) {
			newScore = i;//1 << i;
			
			if ( newScore == OldScore )
				return;

			if( /*(rpg_enabledranks.integer & newScore) && */!MaxRankHit )
			{
				SetScore( ent,  newScore );
				trap_SendServerCommand(ent-g_entities, va( "prank %s", g_rankNames[i].consoleName ) );
				break;
			}
			else
			{
				if ( !MaxRankHit )
					trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\""));
				else
					trap_SendServerCommand( ent->client->ps.clientNum, va("print \"You cannot set your rank that high on this server.\n\""));
				
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
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank doesn't exist on this server!\n\"\n"));
		SetScore( ent, OldScore);
		return;
	}
	
	if ( OldScore > ent->client->ps.persistant[PERS_SCORE] ) {
		trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " was demoted to %s\n\"", ent->client->pers.netname, g_rankNames[i].formalName ) );
	}
	else {
		trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " was promoted to %s\n\"", ent->client->pers.netname, g_rankNames[i].formalName ) );
	}

	/*if(!Q_strncmp(ArgStr, "crewman", 7))
	{
		if(rpg_enabledranks.integer & crewman)
		{
			SetScore( ent,  crewman);
			trap_SendServerCommand(ent-g_entities,"prank crewman");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "cadet1", 6))
	{
		if(rpg_enabledranks.integer & cadet1)
		{
			SetScore( ent,  cadet1);
			trap_SendServerCommand(ent-g_entities,"prank cadet1");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "cadet2", 6))
	{
		if(rpg_enabledranks.integer & cadet2)
		{
			SetScore( ent,  cadet2);
			trap_SendServerCommand(ent-g_entities,"prank cadet2");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "cadet3", 6))
	{
		if(rpg_enabledranks.integer & cadet3)
		{
			SetScore( ent,  cadet3);
			trap_SendServerCommand(ent-g_entities,"prank cadet3");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "cadet4", 6))
	{
		if(rpg_enabledranks.integer & cadet4)
		{
			SetScore( ent,  cadet4);
			trap_SendServerCommand(ent-g_entities,"prank cadet4");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "ensign", 6))
	{
		if(rpg_enabledranks.integer & ensign)
		{
			SetScore( ent,  ensign);
			trap_SendServerCommand(ent-g_entities,"prank ensign");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "ltjg", 4))
	{
		if(rpg_enabledranks.integer & ltjg)
		{
			SetScore( ent,  ltjg);
			trap_SendServerCommand(ent-g_entities,"prank ltjg");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "ltcmdr", 6))
	{
		if(rpg_enabledranks.integer & ltcmdr)
		{
			SetScore( ent,  ltcmdr);
			trap_SendServerCommand(ent-g_entities,"prank ltcmdr");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "lt", 2))
	{
		if(rpg_enabledranks.integer & lt)
		{
			SetScore( ent,  lt);
			trap_SendServerCommand(ent-g_entities,"prank lt");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "cmdr", 4))
	{
		if(rpg_enabledranks.integer & cmdr)
		{
			SetScore( ent,  cmdr);
			trap_SendServerCommand(ent-g_entities,"prank cmdr");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "capt", 4))
	{
		if(rpg_enabledranks.integer & cpt)
		{
			SetScore( ent,  cpt);
			trap_SendServerCommand(ent-g_entities,"prank cpt");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "cmmdr", 5))
	{
		if(rpg_enabledranks.integer & cmmdr)
		{
			SetScore( ent,  cmmdr);
			trap_SendServerCommand(ent-g_entities,"prank cmmdr");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "adm2", 4))
	{
		if(rpg_enabledranks.integer & adm2)
		{
			SetScore( ent,  adm2);
			trap_SendServerCommand(ent-g_entities,"prank adm2");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "adm3", 4))
	{
		if(rpg_enabledranks.integer & adm3)
		{
			SetScore( ent,  adm3);
			trap_SendServerCommand(ent-g_entities,"prank adm3");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "adm4", 4))
	{
		if(rpg_enabledranks.integer & adm4)
		{
			SetScore( ent,  adm4);
			trap_SendServerCommand(ent-g_entities,"prank adm4");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "adm5", 4))
	{
		if(rpg_enabledranks.integer & adm5)
		{
			SetScore( ent,  adm5);
			trap_SendServerCommand(ent-g_entities,"prank adm5");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else
	{
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank doesn't exist on this server!\n\"\n"));
		SetScore( ent,  OldScore);
		return;
	}*/

	/*if ( OldScore > ent->client->ps.persistant[PERS_SCORE] ) 
		trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " was demoted to %s\n\"", ent->client->pers.netname, correlateRanks( ArgStr, 0 ) ) );
	else 
		trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " was promoted to %s\n\"", ent->client->pers.netname, correlateRanks( ArgStr, 0 ) ) ); */


	//RPG-X: RedTechie - No idea or why you needed this when all you had to do was a simple else statement
	/*if( (ent->client->ps.persistant[PERS_SCORE] & rpg_enabledranks.integer) > adm5)
	{
		Com_Printf("This rank is not enabled on this server!\n");	
		SetScore( ent,  OldScore);
		return;
	}*/
}



/*
=================
Force Rank			(RPG-X: J2J & RedTechie)
=================
*/

/*
RANK VALUES:
[defined in cg_local.h]

#define 	crewman	1		
#define 	cadet1		2		
#define 	cadet2		4		
#define 	cadet3		8		
#define 	cadet4		16		
#define 	ensign		32		
#define 	ltjg			64		
#define 	lt				128	
#define 	ltcmdr		256	
#define 	cmdr			512	
#define 	cpt			1024	
#define 	cmmdr		2048	
#define 	adm2		4096	
#define 	adm3		8192	
#define  adm4		16384
#define 	adm5		32768
*/
void Cmd_ForceRank_f( gentity_t *ent)
{
//	const char *info;
	int OldScore;	
	char ArgStr[MAX_TOKEN_CHARS];							//Argument String
	int targetNum;
	gentity_t *other;
	char send[100];
	int i;
	gentity_t *sayA;
	int newScore = -1;

	//char tmpScore[50]; // TiM
	
	if ( IsAdmin( ent ) == qfalse ) {
		return;
	}

	if(!ent || !ent->client)
		return;

	//If for some strange reason there are no ranks.. oO
	/*if(rpg_enabledranks.integer <= 0)
	{
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"Ranks are disabled on this server!\n\""));
		return;
	}*/

	// find the player
	trap_Argv( 1, ArgStr, sizeof( ArgStr ) );

	if ( !ArgStr[0] ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"\nUsage: User forces another player into a specific rank\nCommand: ForceRank [Player ID] <Rank Name>\n\nType 'rankList' into the console for a full list of ranks\" ") );
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

	//RPG-X: TiM - Check if they're already that rank, and return if they are. :P

	/*Com_sprintf(tmpScore, sizeof(tmpScore), "%s", correlateRanks( ArgStr, 1 ));

	if ( OldScore == atoi(tmpScore) )
	{
		return;
	}*/
	
	//RPG-X: RedTechie - Lets enable score updating without this scores will not be updated
	/*other->client->UpdateScore = qtrue;

	if(!Q_strncmp(ArgStr, "crewman", 7))
	{
		if(rpg_enabledranks.integer & crewman)
		{
			SetScore( other,  crewman);
			trap_SendServerCommand(targetNum,"prank crewman");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "cadet1", 6))
	{
		if(rpg_enabledranks.integer & cadet1)
		{
			SetScore( other,  cadet1);
			trap_SendServerCommand(targetNum,"prank cadet1");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "cadet2", 6))
	{
		if(rpg_enabledranks.integer & cadet2)
		{
			SetScore( other,  cadet2);
			trap_SendServerCommand(targetNum,"prank cadet2");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "cadet3", 6))
	{
		if(rpg_enabledranks.integer & cadet3)
		{
			SetScore( other,  cadet3);
			trap_SendServerCommand(targetNum,"prank cadet3");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "cadet4", 6))
	{
		if(rpg_enabledranks.integer & cadet4)
		{
			SetScore( other,  cadet4);
			trap_SendServerCommand(targetNum,"prank cadet4");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "ensign", 6))
	{
		if(rpg_enabledranks.integer & ensign)
		{
			SetScore( other,  ensign);
			trap_SendServerCommand(targetNum,"prank ensign");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "ltjg", 4))
	{
		if(rpg_enabledranks.integer & ltjg)
		{
			SetScore( other,  ltjg);
			trap_SendServerCommand(targetNum,"prank ltjg");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "ltcmdr", 6))
	{
		if(rpg_enabledranks.integer & ltcmdr)
		{
			SetScore( other,  ltcmdr);
			trap_SendServerCommand(targetNum,"prank ltcmdr");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "lt", 2))
	{
		if(rpg_enabledranks.integer & lt)
		{
			SetScore( other,  lt);
			trap_SendServerCommand(targetNum,"prank lt");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "cmdr", 4))
	{
		if(rpg_enabledranks.integer & cmdr)
		{
			SetScore( other,  cmdr);
			trap_SendServerCommand(targetNum,"prank cmdr");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "capt", 4))
	{
		if(rpg_enabledranks.integer & cpt)
		{
			SetScore( other,  cpt);
			trap_SendServerCommand(targetNum,"prank cpt");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "cmmdr", 5))
	{
		if(rpg_enabledranks.integer & cmmdr)
		{
			SetScore( other,  cmmdr);
			trap_SendServerCommand(targetNum,"prank cmmdr");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "adm2", 4))
	{
		if(rpg_enabledranks.integer & adm2)
		{
			SetScore( other,  adm2);
			trap_SendServerCommand(targetNum,"prank adm2");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "adm3", 4))
	{
		if(rpg_enabledranks.integer & adm3)
		{
			SetScore( other,  adm3);
			trap_SendServerCommand(targetNum,"prank adm3");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "adm4", 4))
	{
		if(rpg_enabledranks.integer & adm4)
		{
			SetScore( other,  adm4);
			trap_SendServerCommand(targetNum,"prank adm4");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else if(!Q_strncmp(ArgStr, "adm5", 4))
	{
		if(rpg_enabledranks.integer & adm5)
		{
			SetScore( other,  adm5);
			trap_SendServerCommand(targetNum,"prank adm5");
		}
		else
		{
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			return;
		}
	}
	else
	{
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank doesn't exist on this server!\n\"\n"));
		SetScore( other,  OldScore);
		return;
	}*/

	other->client->UpdateScore = qtrue;

	for ( i=0; (g_rankNames[i].consoleName && i < MAX_RANKS); i++ ) {
		if ( !Q_stricmp( ArgStr, g_rankNames[i].consoleName ) ) {
			newScore = i;//1 << i;
			
			//TiM - since an int can only hold 32 flags, that limits our rank system
			//to 32, making it not very effective.
			//if(rpg_enabledranks.integer & newScore )
			//{
				SetScore( other,  newScore );
				trap_SendServerCommand(other-g_entities, va( "prank %s", g_rankNames[i].consoleName ) );
				break;
			//}
			//else
			//{
			//	trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank is disabled\n\"\n"));
			//	return;
			//}
		}
	}

	//if we didn't get find a matching name. >.<
	if ( newScore < 0 ) {
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"This rank doesn't exist on this server!\n\"\n"));
		SetScore( other, OldScore);
		return;
	}
	
	if ( OldScore > ent->client->ps.persistant[PERS_SCORE] ) {
		trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " was demoted to %s\n\"", other->client->pers.netname, g_rankNames[i].formalName ) );
	}
	else {
		trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " was promoted to %s\n\"", other->client->pers.netname, g_rankNames[i].formalName ) );
	}

	/*if ( OldScore > other->client->ps.persistant[PERS_SCORE] ) 
		trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " was demoted to %s\n\"", other->client->pers.netname, correlateRanks( ArgStr, 0 ) ) );
	else 
		trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " was promoted to %s\n\"", other->client->pers.netname, correlateRanks( ArgStr, 0 ) ) );*/


	G_LogPrintf( "%s changed %s's rank to %s\n", ent->client->pers.netname, other->client->pers.netname, ArgStr );

	for (i = 0; i < 1023; i++) {
		if(g_entities[i].client){
			sayA = &g_entities[i];
			Com_sprintf (send, sizeof(send), "%s changed %s's rank to %s\n", ent->client->pers.netname, other->client->pers.netname, ArgStr);
			G_SayTo( ent, sayA, SAY_ADMIN, COLOR_CYAN, "^7Server: ", send );
		}
	}
}

/*
================
IsAdmin
RPG-X | Phenix | 21/11/2004
================
*/
qboolean IsAdmin( gentity_t *ent)
{
	if ( !ent )
		return qfalse;

	if ( !ent->client )
		return qfalse;

	if (( g_classData[ent->client->sess.sessionClass].isAdmin/*ent->client->sess.sessionClass == PC_ADMIN*/ ) ||
		( ent->client->LoggedAsAdmin == qtrue ) ||
		( ent->client->LoggedAsDeveloper == qtrue ))
	{
		return qtrue;
	} else {
		return qfalse;
	}
}

/*
=================
Admins
RPG-X | Phenix | 21/11/2004
=================
*/

void Cmd_Admins_f( gentity_t *ent)
{
	char		send[MAX_TOKEN_CHARS];
	int			j;
	gentity_t	*target;

	Q_strncpyz( send, "The following players are logged in as admins: \n", sizeof(send));
	for (j = 0; j < level.maxclients; j++) {
		target = &g_entities[j];
		if (g_classData[target->client->sess.sessionClass].isAdmin/*target->client->sess.sessionClass == PC_ADMIN*/)
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

void Cmd_AdminLogin_f( gentity_t *ent)
{
	char		arg[MAX_TOKEN_CHARS];
	char		send[MAX_TOKEN_CHARS];
	char		classPass[MAX_TOKEN_CHARS];
	int			j;
	gentity_t	*sayA;

	//If no arguments where entered return
	//if ( trap_Argc() < 1 )
	//	return;

	trap_Argv( 1, arg, sizeof( arg ) );

	if ( !arg[0] ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"\nUsage: Allows a player to login as an admin\nCommand: AdminLogin <Admin Password>\nWARNING: Entering an incorrect password 3 times will automatically kick you from the server!\n\" ") );
		return;
	}
	
	//Debug login's for working on adminclass functions during development (remove for release)
	/*if( Q_stricmp(arg, RPGX_SERECT_PASS) == 0 ){
		if ( IsAdmin( ent ) == qfalse ) {
			ent->client->LoggedAsDeveloper = qtrue;
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"You are logged in as an admin.\n\"") );
			return;
		} else {
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"You are already logged in as an admin or in the admin class.\n\"") );			
			return;
		}
	}*/
	
	//Admin class login thingy
	//if( (Q_stricmp(arg, rpg_adminpass.string) == 0) && rpg_adminpass.string[0] ){
	for ( j=0; g_classData[j].consoleName[0] && j < MAX_CLASSES; j++ ) 
	{
		trap_Cvar_VariableStringBuffer( va( "rpg_%sPass", g_classData[j].consoleName ), classPass, sizeof(classPass) );

		if ( g_classData[j].isAdmin && !Q_stricmp( classPass, arg ) )
		{
			if ( IsAdmin( ent ) == qfalse ) {
				ent->client->LoggedAsAdmin = qtrue;
				trap_SendServerCommand( ent->client->ps.clientNum, va("print \"You are logged in as an admin.\n\"") );
				ClientUserinfoChanged( ent->client->ps.clientNum );
				return;
			} else {
				trap_SendServerCommand( ent->client->ps.clientNum, va("print \"You are already logged in as an admin or in the admin class.\n\"") );			
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
		for (j = 0; j < level.maxclients; j++) {
			sayA = &g_entities[j];
			G_SayTo( ent, sayA, SAY_ADMIN, COLOR_CYAN, "^7Server: ", send );
		}

		//Kick Client because client has entered the wrong password 3 times
		trap_DropClient( ent->s.number, "Kicked: Too many bad passwords!" );
	} else {
		//Give the client another warning
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"You have entered an incorrect password, if you enter a wrong password %i more times you will be kicked.\n\"", (3 - ent->client->AdminFailed) ) );
	
		//Send warning to other admins
		Com_sprintf (send, sizeof(send), "%s tried to login as an admin and got an incorrect password, %i tries left.\n", ent->client->pers.netname, (3 - ent->client->AdminFailed));
		for (j = 0; j < level.maxclients; j++) {
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
void Cmd_Revive_f( gentity_t *ent)
{
	char pla_str[50];	//Argument String
	int targetNum;
	gentity_t *other;
	int i;
	
	if ( IsAdmin( ent ) == qfalse || !rpg_medicsrevive.integer ) {
		return;
	}

	if(!ent || !ent->client)
		return;

	//If for some strange reason medic revive is turned off
	if(rpg_medicsrevive.integer <= 0)
	{
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"Medics' Revive is disabled on this server!\n\""));
		return;
	}
	
	// find the player
	trap_Argv( 1, pla_str, sizeof( pla_str ) );

	//If player entered no args AND is alive (So as to not screw up reviving yourself ;) )
	if ( !pla_str[0] && (ent && ent->client) && !(ent->health <= 1) && !(ent->client->ps.pm_type == PM_DEAD) ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"\nUsage: User revives incapacitated players\nCommand: Revive {Nothing = Revive Self} OR [Player ID] OR 'all'\n\" ") );
		return;
	}

	if(!Q_strncmp(pla_str, "all", 3))
	{
		//Loop threw all clients
		int j;
		gentity_t *sayA;
		char send[100];

		for(i = 0; i < 1023; i++)
		{
			if( (g_entities[i].client) && (g_entities[i].health == 1) && (g_entities[i].client->ps.pm_type == PM_DEAD))
			{
				ClientSpawn(&g_entities[i], 1);

				//TiM: Push any users out of the way, and then play a 'getup' emote
				g_entities[i].r.contents = CONTENTS_NONE;
				g_entities[i].client->ps.stats[LEGSANIM] = ( ( g_entities[i].client->ps.stats[LEGSANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | BOTH_GET_UP1;
				g_entities[i].client->ps.stats[TORSOANIM] = ( ( g_entities[i].client->ps.stats[TORSOANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | BOTH_GET_UP1;
				g_entities[i].client->ps.stats[TORSOTIMER] = 1700;
				g_entities[i].client->ps.stats[LEGSTIMER] = 1700;
				g_entities[i].client->ps.stats[EMOTES] = EMOTE_BOTH | EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH;
				g_entities[i].client->ps.legsAnim = 0;
				g_entities[i].client->ps.torsoAnim = 0;
				g_entities[i].client->ps.legsTimer = 0;
				g_entities[i].client->ps.torsoTimer = 0;
				//G_MoveBox( &g_entities[i] );
			}
		}
		
		G_LogPrintf( "%s revived everyone\n", ent->client->pers.netname);

		Com_sprintf (send, sizeof(send), "%s revived everyone\n", ent->client->pers.netname, pla_str);

		for (j = 0; j < 1023; j++) {
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
				ClientSpawn(ent, 1);

				ent->r.contents = CONTENTS_NONE;
				ent->client->ps.stats[LEGSANIM] = ( ( ent->client->ps.stats[LEGSANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | BOTH_GET_UP1;
				ent->client->ps.stats[TORSOANIM] = ( ( ent->client->ps.stats[TORSOANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | BOTH_GET_UP1;
				ent->client->ps.stats[TORSOTIMER] = 1700;
				ent->client->ps.stats[LEGSTIMER] = 1700;
				ent->client->ps.stats[EMOTES] = EMOTE_BOTH | EMOTE_OVERRIDE_BOTH | EMOTE_CLAMP_BODY;
				ent->client->ps.legsAnim = 0;
				ent->client->ps.torsoAnim = 0;
				ent->client->ps.legsTimer = 0;
				ent->client->ps.torsoTimer = 0;
				//G_MoveBox( ent);
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
				ClientSpawn(other, 1);
			
				other->r.contents = CONTENTS_NONE;
				other->client->ps.stats[LEGSANIM] = ( ( other->client->ps.stats[LEGSANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | BOTH_GET_UP1;
				other->client->ps.stats[TORSOANIM] = ( ( other->client->ps.stats[TORSOANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | BOTH_GET_UP1;
				other->client->ps.stats[TORSOTIMER] = 1700;
				other->client->ps.stats[LEGSTIMER] = 1700;
				other->client->ps.stats[EMOTES] |= EMOTE_BOTH | EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH;
				other->client->ps.legsAnim = 0;
				other->client->ps.torsoAnim = 0;
				other->client->ps.legsTimer = 0;
				other->client->ps.torsoTimer = 0;
				//G_MoveBox( other);

				G_LogPrintf( "%s revived %s\n", ent->client->pers.netname, other->client->pers.netname);

				Com_sprintf (send, sizeof(send), "%s revived %s\n", ent->client->pers.netname, other->client->pers.netname);

				for (j = 0; j < 1023; j++) 
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
void Cmd_n00b_f( gentity_t *ent)
{
	char arg[MAX_TOKEN_CHARS];
	int targetNum;
	int timeToBe;
	gentity_t *target;
	int i;

	//If client isn'tin admin class, exit and disallow command.
	if( IsAdmin( ent ) == qfalse)			
	{
		return;												
	}

	if ( !ent || !ent->client ) {
		return;		// not fully in game yet
	}

	memset(arg,0,MAX_TOKEN_CHARS);
	trap_Argv( 1, arg, sizeof( arg ) );

		//User Friendly
	if ( !arg[0] ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"\nUsage: User places another player in n00b class for X seconds\nCommand: n00b [Player ID] [Seconds to be in n00b class]\n\" ") );
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
			SetClass( target, conName/*rpg_n00bpass.string*/, NULL, qfalse );
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
	if( IsAdmin( ent ) == qfalse)			
	{
		return;												
	}

	if ( !ent || !ent->client ) {
		return;		// not fully in game yet
	}

	arg = ConcatArgs( 1 );

	if ( !arg[0] ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"\nUsage: Admin broadcasts a message to all users on the server that stays for several seconds\nCommand: Msg \"<Message>\"\n\" ") );
		return;
	}

	if ( arg[0] == '\0' )
	{
		return;
	}

	trap_SendServerCommand( -1, va("servermsg %s", arg));

}

/*
=================
Cmd_SetStartClass
TiM: Let the game module dynamically set teh player's class upon connect
=================

static void Cmd_SetStartClass( gentity_t *ent, char* cmd )
{
	ent->client->sess.sessionClass = atoi( cmd );
}*/

/*
=================
Cmd_ForceModel_f
=================
*/
static void Cmd_ForceModel_f( gentity_t *ent ) {
	gclient_t	*cl;
//	gclient_t *client;
	gentity_t *other;
	gentity_t *sayA;
	int j;
	char send[100];
	char  str[MAX_TOKEN_CHARS];
	char  *str2;
	//char str2[MAX_TOKEN_CHARS];
	//char userinfo[MAX_INFO_STRING];
	char clientCmd[64];

	if ( IsAdmin( ent ) == qfalse ) {
		return;
	}

	//get the first arg
	trap_Argv( 1, str, sizeof( str ) );

	if ( !str[0] ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"\nUsage: User forces another player into a specific character model\n\nCommand: ForceModel [Player ID] <Model Name>/<Model Skin Name>\n\" ") );
		return;
	}

	// find the player
	cl = ClientForString( str );
	if ( !cl ) { 
		return;
	}
	other = g_entities + cl->ps.clientNum;
	//client = other->client;
	
	//Get the new model
	str2 = ConcatArgs( 2 );
	//trap_Argv( 2, str2, sizeof( str2 ) );
	
	//Print out some chat text
	G_LogPrintf( "%s changed %s's model to %s\n", ent->client->pers.netname, other->client->pers.netname, str2 );

	Com_sprintf (send, sizeof(send), "%s ^7changed %s^7's model to %s", ent->client->pers.netname, other->client->pers.netname, str2);

	for (j = 0; j < 1023; j++) {
		if(g_entities[j].client){
			sayA = &g_entities[j];
			G_SayTo( ent, sayA, SAY_ADMIN, COLOR_CYAN, "^7Server: ", send );
		}
	}
	
	//Set the name
	//TiM: Remember to actually load the User's Info into the variable before modifying it
	//The server was parsing a NULL string. O_o
	/*trap_GetUserinfo( cl->ps.clientNum, userinfo, sizeof( userinfo ) );

	Info_SetValueForKey (userinfo, "model", str2);
	trap_SetUserinfo(cl->ps.clientNum, userinfo);
	ClientUserinfoChanged(cl->ps.clientNum);*/

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
	char* output = "\0"; //shuts up the compiler :P
	
	//standard checks
	if ( IsAdmin( ent ) == qfalse ) {
		return;
	}

	if(!ent || !ent->client)
		return;

	//Load the arguments
	trap_Argv( 1, songIntro, sizeof( songIntro ) );
	trap_Argv( 2, songLoop, sizeof( songLoop ) );

	//Output the isntructions if no arguments
	if ( !songIntro[0] ) {
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"\nUsage: Plays music throughout the level\n\nCommand: playMusic [songIntro] <songLoop>\n\" ") );
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
	if ( IsAdmin( ent ) == qfalse ) {
		return;
	}

	if(!ent || !ent->client)
		return;

	//trap_SendConsoleCommand( EXEC_APPEND, "music music/silence" );
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
	
	//standard checks
	if ( IsAdmin( ent ) == qfalse ) {
		return;
	}

	if(!ent || !ent->client)
		return;

	//Load the arguments
	trap_Argv( 1, soundAddress, sizeof( soundAddress ) );

	//Output the isntructions if no arguments
	if ( !soundAddress[0] ) {
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"\nUsage: Plays a sound file throughout the level\n\nCommand: playSound [File Address]\n\" ") );
		return;	
	}

	//Broadcast the command
	trap_SendServerCommand( -1, va("playSnd %s\n", soundAddress ) );

}

/*void Cmd_PlayAnim_f( gentity_t *ent )
{

	int anim;

	//ent->nextthink = level.time + 2000;

	anim = BOTH_LAUGH2;
	
	//======================================

	ent->client->ps.stats[TORSOTIMER] = 2900;

	ent->client->ps.torsoAnim = 
		( ( ent->s.torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;


	//=====================================

	ent->client->ps.stats[LEGSTIMER] = 2900;

	ent->client->ps.eFlags |= EF_LOWER_EMOTING;

	ent->client->ps.legsAnim = 
		( ( ent->client->ps.legsAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;

}*/

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
	
	/*if ( IsAdmin( ent ) == qfalse ) {
		return;
	}*/

	ent->flags ^= FL_HOLSTER;
	if (!(ent->flags & FL_HOLSTER))
	{
		msg = "You took your equipment off.\n";		
		ent->client->ps.powerups[PW_BOLTON] = level.time;
	}        
	else
	{
		msg = "You put your equipment on.\n";
		ent->client->ps.powerups[PW_BOLTON] = INT_MAX;
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

	if ( !IsAdmin( ent ) ) {
		return;
	}

	trap_Argv( 1, entArg, sizeof( entArg ) );

	//No arguments - Do a trace 
	if ( !entArg[0] ) {
		vec3_t start, forward, end;
		trace_t tr;

		//calc start
		VectorCopy( ent->r.currentOrigin, start );
		start[2] += ent->client->ps.viewheight;

		//calc end
		AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );
		VectorMA( start, 8192, forward, end );

		//trace-er-roo
		trap_Trace( &tr, start, NULL, NULL, end, ent->client->ps.clientNum, MASK_SHOT );

		//We hit nothing valid, so let's print some nifty instructions :)
		//Can't do it :( It'd get too annoying >.<
		/*if ( tr.fraction == 1.0 || ( tr.entityNum < MAX_CLIENTS || tr.entityNum >= ENTITYNUM_WORLD ) ) {
			trap_SendServerCommand( ent->client->ps.clientNum, va("print \"\nUsage: Activates a useable map entity\n\nCommand: useEnt <Index of targeted entity>\n\" ") );
			return;
		}*/

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
		if ( !Q_stricmp( targetEnt->classname, "func_usable" ) /*&& !ent->targetname*/ )
			return;
		
		targetEnt->use( targetEnt, ent, ent ); //Activate the Ent
	}
	else { //otherwise berrate the user for being n00bish
		trap_SendServerCommand( ent-g_entities, va( " print \"Entity %i cannot be activated in that way.\n\" ", index ) );
	}
}

/*
=================
Cmd_UseEnt_f
=================
TiM: Displays as many ents it can in the console without pwning the network code in the process
*/
void Cmd_EntList_f ( gentity_t *ent ) {
	int			i;
	gentity_t	*mapEnt;
	char		entBuffer[128];
	char		mainBuffer[1024]; //16384
	int			loopNum=0;

	if ( !IsAdmin( ent ) ) {
		return;
	}

	//initialise the data holders
	memset( &entBuffer, 0, sizeof( entBuffer ) );
	memset( &mainBuffer, 0, sizeof( mainBuffer ) );

	//loop thru all our ents and check to see if we should list it
	//for ( i = MAX_CLIENTS; i < MAX_GENTITIES; i++ ) {
	for (i = 0, mapEnt = g_entities;
			i < level.num_entities;
			i++, mapEnt++) {

		//mapEnt = &g_entities[i];

		//meh... screw notnull ents.  They're unuseable
		//if ( mapEnt->targetname && mapEnt->classname && Q_stricmp(mapEnt->classname, "info_notnull" ) ) {
		//Okay, to save resources, only specific entities will be transmitted
		//if ( !Q_stricmp( mapEnt->classname, "fx_" ) {
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

		//TiM : Cheep hack - The sendmessage command can only handle 1024 chars at a time
		//( I think), so this divides it up.  Warning though... spamming this command could cause
		//a wee bit laggage.
		/*if ( strlen( mainBuffer ) > 800 ) {
			trap_SendServerCommand( ent-g_entities, va("print \"%s\" ", mainBuffer) );
			memset( &mainBuffer, 0, sizeof( mainBuffer ) );
		}*/
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

void Cmd_BeamToLoc_f( gentity_t *ent ) {
	char		argStr[MAX_TOKEN_CHARS];
	gentity_t	*locEnt;
	gentity_t	*targEnt;
	gentity_t	*beamTarget;
	int			i;
	int			clientNum, locIndex;
	char		*strLoc=NULL;

	//Has to be an admin.. if anyone had it, the brig would become useless.
	if ( !IsAdmin( ent ) ) {
		return;
	}

	trap_Argv( 1, argStr, sizeof( argStr ) );
	if ( !argStr[0] ) {
		trap_SendServerCommand( ent-g_entities, va( "print \"Usage: Allows you to beam yourself to any 'target_location' entity that has a compatible beam-in point\nCommand: (For yourself) - beamToLocation <Index or name of target_location ent> \n(For other players) - beamTo <player ID> <Index or name of target_location ent>\nType '/beamList' into the console for a list of the 'target_location' indexes.\n\" " ) );
		return;
	}

	//if more than one arg, assume we're beaming a separate client
	//first arg MUST BE AN INTEGER CHIKUSHOYO OR THE THING BREAKS!!!!!! (Japanese expletive)
	//must be an int coz it's the clientNum, anything else is not valid.
	if ( trap_Argc() > 2 && (unsigned char)argStr[0] >= '0' && (unsigned char)argStr[0] <= '9' ) {
		//Get Client ID
		clientNum = atoi( argStr );

		if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
			trap_SendServerCommand( ent-g_entities, va("chat \"Invalid Client ID Number.\n\" " ) );
			return;
		}

		beamTarget = &g_entities[clientNum];
		if ( !beamTarget || !beamTarget->client ) {
			trap_SendServerCommand( ent-g_entities, va("chat \"No valid client found.\n\" " ) );
			return;
		}

		//Ugh - Make it so we can't beam ourselves this way - coz EF has some weird entity handling issues.
		/*if ( beamTarget->client->ps.clientNum == ent->client->ps.clientNum ) {
			trap_SendServerCommand( ent-g_entities, va("chat \"ERROR: You cannot beam yourself in this fashion. It will cause errors.\nThis is only meant for other players.\n\" " ) );
			return;
		}*/

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
		//offset the origin by 12, or u'll materialize in the floor >.<
		//Damn id engine ent origin whackiness
		//targEnt->s.origin[2] += 10; //12 - 10 now so we can get away without needing to embed the entity inside a brush for it to be directly on the floor.
									//if we're not careful, that can potentially create map leaks (or even worse, screw up the VIS calc stage).

		//TiM : Leeched this code off elsewhere.  instead of hard-coding a value, align it to the ent's bbox.
		//In case they don't use notnulls but something else.
		VectorCopy(targEnt->s.origin, destPoint);
		destPoint[2] += targEnt->r.mins[2]; 
		destPoint[2] -= beamTarget->r.mins[2];
		destPoint[2] += 1;

		if ( TransDat[beamTarget->client->ps.clientNum].beamTime == 0 ) {
			trap_SendServerCommand( ent-g_entities, va( "chat \"Initiating transport to location: %s\n\" ", locEnt->message ) ); 
			G_InitTransport( beamTarget->client->ps.clientNum, destPoint, targEnt->s.angles );
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
void Cmd_ForcePlayer_f ( gentity_t *ent ) {
	gclient_t	*cl;
	gentity_t *other;
	gentity_t *sayA;
	int j;
	char send[100];
	char  str[MAX_TOKEN_CHARS];
	char  *str2;
	//char str2[MAX_TOKEN_CHARS];
	char userinfo[MAX_INFO_STRING];
	char clientCmd[MAX_INFO_STRING];

	if ( IsAdmin( ent ) == qfalse ) {
		return;
	}

	// find the player
	trap_Argv( 1, str, sizeof( str ) );

	if ( !str[0] ) { //if user added no args (ie wanted the parameters)
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"\nUsage: User force changes a parameter in another player's configuration settings on the server.\nCommand: ForcePlayer <Player's ID Number> \"[Setting to be changed]\" \"[New Value]\"\n\n\" ") );
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
		trap_SendServerCommand( ent->client->ps.clientNum, va("print \"ERROR: Invalid setting specified.\n\" ") );
		return;
	}

	str2 = ConcatArgs( 3 );
	//trap_Argv( 2, str2, sizeof( str2 ) );
	if ( !str2[0] ) {
		return;
	}
	
	//Print out some chat text
	G_LogPrintf( "%s changed %s's %s setting to %s\n", ent->client->pers.netname, other->client->pers.netname, str, str2 );

	Com_sprintf (send, sizeof(send), "%s ^7changed %s's %s setting to ^7to %s", ent->client->pers.netname, other->client->pers.netname, str, str2);

	for (j = 0; j < MAX_CLIENTS; j++) {
		if(g_entities[j].client){
			sayA = &g_entities[j];
			G_SayTo( ent, sayA, SAY_ADMIN, COLOR_CYAN, "^7Server: ", send );
		}
	}
	
	Com_sprintf( clientCmd, sizeof(clientCmd), "changeClientInfo %s %s", str, str2 );

	trap_SendServerCommand( cl->ps.clientNum, clientCmd );

	//Info_SetValueForKey (userinfo, str, str2);
	//trap_SetUserinfo(cl->ps.clientNum, userinfo);
	//ClientUserinfoChanged(cl->ps.clientNum);
}

/*==============
Cmd_BeamToPlayer_f
===============
TiM : Lets players beam to
other player locations.
*/
#define PLAYER_BEAM_DIST	50

void Cmd_BeamToPlayer_f( gentity_t	*ent ) {
	char		argStr[MAX_TOKEN_CHARS];
	gentity_t	*target;
	gentity_t	*beamee;
	int			clientNum;
	int			bClientNum;
	vec3_t		mins = { -12, -12, -24 }; //for the volume trace -//12
	vec3_t		maxs = { 12, 12, 56 }; //44
	int			i;
	vec3_t		origin, angles, zOrigin;
	trace_t		tr;
	qboolean	validTraceFound = qfalse;
	int			startPoint;
	int			totalCount;
	int			offsetRA[8][2] = {	 1, 0, //offsets for each beam test location
									 1, -1,
									 0, -1,
									 -1, -1,
									 -1, 0,
									 -1, 1, 
									 0, 1, 
									 1, 1 
								};
	int			viewAngleHeading[8] = { 180, 135, 90, 45, 0, -45, -90, -135 };
									

	//Has to be an admin.. if anyone had it, the brig would become useless.
	if ( !IsAdmin( ent ) ) {
		return;
	}

	trap_Argv( 1, argStr, sizeof( argStr ) );
	if ( !argStr[0] ) {
		trap_SendServerCommand( ent-g_entities, va( "print \"Usage: Allows you to beam yourself or another player to the location of a separate player.\nCommand: (For yourself) - beamToPlayer <ID of player to beam to> \n(For other players) - beamToPlayer <ID of player to beam> <ID of location player>\n\" " ) );
		return;
	}

	if ( trap_Argc() == 2 ) {
		clientNum = atoi( argStr );
		bClientNum = ent->client->ps.clientNum;
	}
	else if ( trap_Argc() >= 3 ) {
		bClientNum = atoi( argStr );

		trap_Argv( 2, argStr, sizeof( argStr ) );
		clientNum = atoi( argStr );
	}

	if ( clientNum == bClientNum ) {
		trap_SendServerCommand( ent-g_entities, va( "print \"ERROR. Target location player and target beaming player cannot be the same.\n\" " ) );
		return;
	}

	if ( clientNum < 0 || clientNum >= MAX_CLIENTS || bClientNum < 0 || bClientNum >= MAX_CLIENTS ) {
		trap_SendServerCommand( ent-g_entities, va( "print \"Invalid client specified.\n\" " ) );
		return;
	}

	//get our intended target
	target = &g_entities[clientNum];
	if ( target == NULL ) {
		return;
	}

	//get our intended beam subject
	beamee = &g_entities[bClientNum];
	if ( beamee == NULL ) {
		return;
	}

	startPoint = irandom( 1, 7 ); //we'll randomize where it checks, so players won't potentially beam to the same spot each time.

	//okay, we're going to test 8 locations around our target.  First one we find that's available, we'll take. 
	//for ( i = 0; i < 8; i++ ) {
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
		return;
	}

	VectorCopy( ent->client->ps.viewangles, angles );
	angles[YAW] = (float)viewAngleHeading[i];
	//Com_Printf( "OldAngle = %f, NewAngle = %f, index = %i\n", ent->client->ps.viewangles[YAW], angles[YAW], i );

	if ( TransDat[ beamee->client->ps.clientNum ].beamTime == 0 ) {
		if ( bClientNum == ent->client->ps.clientNum ) {
			trap_SendServerCommand( ent-g_entities, va( "chat \"Initiating transport to player %s^7's co-ordinates.\n\" ", target->client->pers.netname ) );
		}
		else {
			trap_SendServerCommand( ent-g_entities, va( "chat \"Transporting %s^7 to player %s^7's co-ordinates.\n\" ", beamee->client->pers.netname, target->client->pers.netname ) );
		}
		//commence beaming
		G_InitTransport( beamee->client->ps.clientNum, origin, angles );
	}
	else {
		if ( bClientNum == ent->client->ps.clientNum ) {
			trap_SendServerCommand( ent-g_entities, va( "chat \"Unable to comply.  You are already within a transport cycle.\n\" " ) );
	
		}
		else {
			trap_SendServerCommand( ent-g_entities, va( "chat \"Unable to comply.  Subject is already within a transport cycle.\n\" " ) );
		}
	}
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
void Cmd_DoEmote_f ( gentity_t	*ent ) {
	char			argStr[MAX_QPATH];
	emoteList_t		*emote;
	//int				anim;
	int				animLength;
	int				i;
	int				emoteInt;
	int				flagHolder=0;
	qboolean		doUpper = qfalse;
	qboolean		doLower = qfalse;
	qboolean		alreadyEmoting;

	trap_Argv( 1, argStr, sizeof( argStr ) );

	//Small override for the specific eyes animation emotes.
	//And also the alert mode now
	if ( !Q_stricmp( argStr, "eyes_shut" ) || !Q_stricmp( argStr, "eyes_frown" ) || !Q_stricmpn( argStr, "alert2", 6 ) || !Q_stricmpn( argStr, "alert", 5 ) ) {
		if ( !Q_stricmp( argStr, "eyes_shut" ) ) {
			ent->client->ps.stats[EMOTES] ^= EMOTE_EYES_SHUT;
		}

		else if ( !Q_stricmp( argStr, "eyes_frown" ) )
			ent->client->ps.stats[EMOTES] ^= EMOTE_EYES_PISSED;
		else if ( !Q_stricmpn( argStr, "alert2", 6 ) )
		{
			ent->client->ps.pm_flags &= ~ANIM_ALERT;
			ent->client->ps.pm_flags ^= ANIM_ALERT2;
		}
		else if ( !Q_stricmpn( argStr, "alert", 5 ) )
		{
			ent->client->ps.pm_flags &= ~ANIM_ALERT2;
			ent->client->ps.pm_flags ^= ANIM_ALERT;
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
	ent->client->ps.torsoAnim = 0;
	ent->client->ps.legsAnim = 0;

	if ( emote->bodyFlags & EMOTE_UPPER ) {
		doUpper = qtrue;
	}

	if ( emote->bodyFlags & EMOTE_LOWER ) {
		doLower = qtrue;
	}

	if ( ( ent->client->ps.stats[EMOTES] & EMOTE_UPPER ) && 
			( ent->client->ps.stats[EMOTES] & EMOTE_LOOP_UPPER ) ) 
	{
		if ( emote->animFlags & EMOTE_REVERTLOOP_UPPER ) {
			int anim = ent->client->ps.stats[TORSOANIM];
			anim &= ~ANIM_TOGGLEBIT; //remove the toggle msk

			for (i=0;i<bg_numEmotes;i++ ) {
				if ( bg_emoteList[i].enumName == anim ) {
					ent->client->ps.torsoAnim = i;
					break;
				}
			}
		}
	}

	if ( ( ent->client->ps.stats[EMOTES] & EMOTE_LOWER ) && 
			( ent->client->ps.stats[EMOTES] & EMOTE_LOOP_LOWER ) ) 
	{
		if ( emote->animFlags & EMOTE_REVERTLOOP_LOWER ) {
			int anim = ent->client->ps.stats[LEGSANIM];
			anim &= ~ANIM_TOGGLEBIT;

			for (i=0;i<bg_numEmotes;i++ ) {
				if ( bg_emoteList[i].enumName == anim ) {
					ent->client->ps.legsAnim = i;
					break;
				}
			}
		}
	}

	alreadyEmoting = ( ( ent->client->ps.stats[EMOTES] & EMOTE_UPPER ) || ( ent->client->ps.stats[EMOTES] & EMOTE_LOWER ) );

	if ( alreadyEmoting ) {
		doUpper = ( ( emote->animFlags & EMOTE_OVERRIDE_UPPER ) );
		doLower = ( ( emote->animFlags & EMOTE_OVERRIDE_LOWER ) );
	}
	else {
		if ( doLower && !( emote->animFlags & EMOTE_OVERRIDE_LOWER ) && ent->client->ps.powerups[PW_FLIGHT] ) {
			doLower = qfalse;
		}
	}

	//flush out any previous emotes
	if ( doUpper ) {
		ent->client->ps.stats[EMOTES] &= ~EMOTE_MASK_UPPER;	//Remove all emotes but eye flags

		flagHolder = ( emote->animFlags | emote->bodyFlags );
		flagHolder &= ~(EMOTE_MASK_LOWER | EMOTE_EYES_SHUT | EMOTE_EYES_PISSED);

		ent->client->ps.stats[EMOTES] |= flagHolder;
	}

	if ( doLower ) {
		ent->client->ps.stats[EMOTES] &= ~EMOTE_MASK_LOWER;	//Remove all emotes but eye flags
		//ent->client->ps.legsTimer = -1;						//player viewheight

		flagHolder = ( emote->animFlags | emote->bodyFlags );
		flagHolder &= ~(EMOTE_MASK_UPPER | EMOTE_EYES_SHUT | EMOTE_EYES_PISSED);
		
		ent->client->ps.stats[EMOTES] |= flagHolder;		
	}

	//anim = emote->enumName;

	//ent->client->ps.stats[EMOTES] |= ( emote->animFlags | emote->bodyFlags );

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
			if ( ent->client->ps.torsoAnim <= 0 && emote->enumLoop >= 0) {
				for (i=0; i<bg_numEmotes; i++ ) {
					if ( bg_emoteList[i].enumName == emote->enumLoop ) {
						ent->client->ps.torsoAnim = i;
						break;
					}
				}
			}

			ent->client->ps.stats[TORSOTIMER] = animLength; 
		}															 
		else {
			ent->client->ps.stats[TORSOTIMER] = 0;						//Infinite animations (ie sitting/typing )
		}

		ent->client->ps.torsoTimer = emoteInt;

		//ent->client->ps.torsoAnim = 
		//( ( ent->s.torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;
		ent->client->ps.stats[TORSOANIM] =
			( ( ent->client->ps.stats[TORSOANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | emote->enumName;
	}

	//=====================================

	if ( doLower ) {

		//if we had a pre-defined anim length
		if ( animLength > 0 ) {
			ent->client->ps.stats[LEGSTIMER] = animLength;

			//check to see if we don't already have a set loop anim
			if ( ent->client->ps.legsAnim == 0 && emote->enumLoop >= 0) {
				for (i=0; i<bg_numEmotes; i++ ) {
					if ( bg_emoteList[i].enumName == emote->enumLoop ) {
						ent->client->ps.legsAnim = i;
						break;
					}
				}
			}
		}
		else {
			ent->client->ps.stats[LEGSTIMER] = 0;
		}

		//set emote num into emote timer so we can use it in pmove
		ent->client->ps.legsTimer = emoteInt;

		//ent->client->ps.legsAnim = 
		//( ( ent->client->ps.legsAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;
		ent->client->ps.stats[LEGSANIM] =
			( ( ent->client->ps.stats[LEGSANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | emote->enumName;
	}
}

/*==============
Cmd_EndEmote_f
Ends all active emotes.
I originally had it so holding the
walk button did this, but then found out
that it's permanently held down in other instances :S
================*/
void Cmd_EndEmote_f( gentity_t *ent ) {
	if( !ent->client )
		return;

	ent->client->ps.stats[EMOTES] &= ~EMOTE_MASK_BOTH;

	ent->client->ps.legsTimer = 0;
	ent->client->ps.legsAnim = 0;
	ent->client->ps.stats[LEGSTIMER] = 0;


	ent->client->ps.torsoTimer = 0;
	ent->client->ps.torsoAnim = 0;
	ent->client->ps.stats[TORSOTIMER] = 0;
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
void Cmd_Laser_f ( gentity_t *ent ) {
	char* message;

	//TiM - Say... what was the max length of time a Q3 server was capable of running
	//nonstop again lol? :)
	if ( !ent->client->ps.powerups[PW_LASER] ) {
		ent->client->ps.powerups[PW_LASER] = level.time + 10000000;
		message = "Activated Laser";
	}
	else {
		ent->client->ps.powerups[PW_LASER] = level.time;
		message = "Deactivated Laser";
	}

	trap_SendServerCommand( ent-g_entities, va( " print \"%s\n\" ", message ) );
}

/*==============
Cmd_Laser_f
==============*/
void Cmd_FlashLight_f ( gentity_t *ent ) {
	char* message;

	//TiM - Say... what was the max length of time a Q3 server was capable of running
	//nonstop again lol? :)
	if ( !ent->client->ps.powerups[PW_FLASHLIGHT] ) {
		ent->client->ps.powerups[PW_FLASHLIGHT] = level.time + 10000000;
		message = "Activated Flashlight";
	}
	else {
		ent->client->ps.powerups[PW_FLASHLIGHT] = level.time;
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
void Cmd_fxGun_f ( gentity_t *ent ) {
	char	arg[MAX_TOKEN_CHARS];
	char	fxName[36];

	if ( !ent->client ) 
		return;

	if ( !IsAdmin( ent ) )
		return;

	trap_Argv( 1, arg, sizeof( arg ) );

	//save the name for the end
	Q_strncpyz( fxName, arg, sizeof( fxName ) );
	
	if ( !Q_stricmp ( arg, "default" ) ) {
		memset( &ent->client->fxGunData, 0, sizeof( ent->client->fxGunData ) );
	}
	else if ( !Q_stricmp ( arg, "detpack" ) ) {
		memset( &ent->client->fxGunData, 0, sizeof( ent->client->fxGunData ) );
		ent->client->fxGunData.eventNum = EV_DETPACK;
	}
	else if ( !Q_stricmp( arg, "chunks" ) ) {
		memset( &ent->client->fxGunData, 0, sizeof( ent->client->fxGunData ) );
		//radius
		trap_Argv( 2, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun chunks <radius> <chunk type: 1-5>\n\" " );
			return;
		}

		ent->client->fxGunData.arg_int1 = atoi( arg );
		ent->client->fxGunData.eventNum = EV_FX_CHUNKS;

		//radius
		trap_Argv( 3, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun chunks <radius> <chunk type: 1-5>\n\" " );
			memset( &ent->client->fxGunData, 0, sizeof( ent->client->fxGunData ) );
			return;
		}

		ent->client->fxGunData.arg_int2 = atoi( arg );
	}
	else if ( !Q_stricmp ( arg, "sparks" ) ) {
		memset( &ent->client->fxGunData, 0, sizeof( ent->client->fxGunData ) );
		trap_Argv( 2, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun sparks <spark time interval> <time length of effect> | in milliseconds\n\" " );
			return;
		}
		
		ent->client->fxGunData.eventNum = EV_FX_SPARK;
		ent->client->fxGunData.arg_float1 = atof( arg );
		
		//optional arg for timelength
		trap_Argv( 3, arg, sizeof( arg ) );
		if ( arg[0] ) {
			ent->client->fxGunData.arg_int2 = atoi(arg);
		}
		else {
			ent->client->fxGunData.arg_int2 = 10000;
		}
	}
	else if ( !Q_stricmp ( arg, "steam" ) ) {
		memset( &ent->client->fxGunData, 0, sizeof( ent->client->fxGunData ) );
		ent->client->fxGunData.eventNum = EV_FX_STEAM;

		//optional arg for timelength
		trap_Argv( 2, arg, sizeof( arg ) );
		if ( arg[0] ) {
			ent->client->fxGunData.arg_int2 = atoi(arg);
		}
		else {
			ent->client->fxGunData.arg_int2 = 10000;
		}
	}
	else if ( !Q_stricmp( arg, "drips" ) ) {
		memset( &ent->client->fxGunData, 0, sizeof( ent->client->fxGunData ) );
		//type of drips
		trap_Argv( 2, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun drips <type of drips: 0,1,2> <drips intensity: 0.0->1.0> <time length of effect: 1000 = 1 second> \n\" " );
			return;
		}
		
		ent->client->fxGunData.eventNum = EV_FX_DRIP;
		ent->client->fxGunData.arg_int1 = atoi( arg );
		
		//intensity of drips
		trap_Argv( 3, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun drips <type of drips: 0,1,2> <drips intensity: 0.0->1.0> <time length of effect: 1000 = 1 second> \n\" " );
			ent->client->fxGunData.eventNum = 0;
			ent->client->fxGunData.arg_int1 = 0;
			return;
		}
		
		ent->client->fxGunData.arg_float1 = atof( arg );

		//optional time length
		trap_Argv( 4, arg, sizeof( arg ) );
		if ( arg[0] ) {
			ent->client->fxGunData.arg_int2 = atoi(arg);
		}
		else {
			ent->client->fxGunData.arg_int2 = 10000;
		}		
	}
	else if ( !Q_stricmp( arg, "smoke" ) ) {
		memset( &ent->client->fxGunData, 0, sizeof( ent->client->fxGunData ) );
		//smoke radius
		trap_Argv( 2, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun smoke <smoke radius size> <time length of effect: 1000 = 1 second> \n\" " );
			return;
		}

		ent->client->fxGunData.eventNum = EV_FX_SMOKE;
		ent->client->fxGunData.arg_int1 = atoi( arg );

		//optional time length
		trap_Argv( 3, arg, sizeof( arg ) );
		if ( arg[0] ) {
			ent->client->fxGunData.arg_int2 = atoi(arg);
		}
		else {
			ent->client->fxGunData.arg_int2 = 10000;
		}		

	}
	else if ( !Q_stricmp( arg, "surf_explosion" ) ) {
		memset( &ent->client->fxGunData, 0, sizeof( ent->client->fxGunData ) );
		//explosion radius
		trap_Argv( 2, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun surf_explosion <explosion radius> <camera shake intensity> \n\" " );
			return;
		}
		
		ent->client->fxGunData.eventNum = EV_FX_SURFACE_EXPLOSION;
		ent->client->fxGunData.arg_float1 = atof( arg );
		
		//explosion cam shake
		trap_Argv( 3, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun surf_explosion <explosion radius> <camera shake intensity> \n\" " );
			
			ent->client->fxGunData.eventNum = 0;
			ent->client->fxGunData.arg_float1 = 0;			
			return;
		}
		
		ent->client->fxGunData.arg_float2 = atof( arg );		
	}
	else if ( !Q_stricmp( arg, "elec_explosion" ) ) {
		memset( &ent->client->fxGunData, 0, sizeof( ent->client->fxGunData ) );
		//explosion radius
		trap_Argv( 2, arg, sizeof( arg ) );
		if ( !arg[0] ) {
			trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun elec_explosion <explosion radius> \n\" " );
			return;
		}

		ent->client->fxGunData.eventNum = EV_FX_ELECTRICAL_EXPLOSION;
		ent->client->fxGunData.arg_float1 = atof( arg );

	}
	else {
		trap_SendServerCommand( ent-g_entities, "print \"Syntax: /fxGun <FX_Name>\nValid Effects:\n  default\n  chunks\n  detpack\n  sparks\n  steam\n  drips\n  smoke\n  surf_explosion\n  elec_explosion \n\" " );
		memset( &ent->client->fxGunData, 0, sizeof( ent->client->fxGunData ) );
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
void Cmd_flushFX_f( gentity_t	*ent ) {
	if ( !ent->client ) 
		return;

	if ( !IsAdmin( ent ) )
		return;

	trap_SendServerCommand( -1, "cg_flushFX" );
}

/*=================
Cmd_spawnChar_f
TiM: Takes ur current data, and 
spawns a player model that looks like you with it.
==================*/
void Cmd_spawnChar_f( gentity_t	*ent ) {
	if ( !ent->client ) 
		return;

	if ( !IsAdmin( ent ) )
		return;
	
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
void Cmd_fluchChars_f( gentity_t *ent )
{
	char	arg[16];
	int		i;
	gentity_t	*locEnt;

	if ( !ent->client ) 
		return;

	if ( !IsAdmin( ent ) )
		return;

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
	//char  str2[MAX_TOKEN_CHARS];
	char  *str2;
	char  reason[MAX_TOKEN_CHARS];
	int i;

	//standard checks
	if ( IsAdmin( ent ) == qfalse ) {
		return;
	}

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
		/*if ( clientNum >= MAX_CLIENTS ) {
			G_Printf( "ERROR: You do not have a valid clientNum. This command cannot be executed.\n");
			return;
		}*/
		for (i = 0; i < MAX_CLIENTS; i++ ) {
			if ( (g_entities[i].client &&  g_entities[i].r.svFlags & SVF_BOT) ) {
				//if ( trap_Argc() <= 2 ) {
					trap_SendConsoleCommand( EXEC_APPEND, va("kick \"%i\"\n", g_entities[i].client->ps.clientNum ) );
				//}
				//else { //else give em a reason I guess
				//	str2 = ConcatArgs( 2 );
				//	Com_sprintf(reason, MAX_TOKEN_CHARS, "Kicked: %s", str2 );

				//	trap_DropClient( g_entities[i].client->ps.clientNum, reason );
				//}
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
			//trap_Argv( 2, str2, sizeof( str2 ) );
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

	//standard checks
	if ( !IsAdmin( ent ) )
		return;

	if ( !ent->client )
		return;

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
	char		arg[4];
	int			numEnts;
	int			touch[MAX_GENTITIES];
	vec3_t		mins, maxs;
	int			targDeck;
	int			i;
	gentity_t	*lift=NULL;
	gentity_t	*otherLift=NULL;
	qboolean	found=qfalse;
	gentity_t	*touched;

	if ( !ent->client )
		return;

	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR )
		return;

	if ( ent->client->ps.stats[STAT_HEALTH] <= 0 ) {
		return;
	}

	//arg 1 specifies the index of the origin turbolift
	trap_Argv( 1, arg, sizeof(arg) );

	if ( !arg[0] )
	{
		trap_SendServerCommand( ent-g_entities, "print \"An origin lift wasn't specified.\n\" " );
		return;
	}	

	lift = &g_entities[atoi(arg)];

	//ensure org is a valid lift ent!!!
	if ( !lift || Q_stricmp( lift->classname, "target_turbolift" ) )
	{
		trap_SendServerCommand( ent-g_entities, "print \"Origin lift wasn't a valid entity.\n\" " );
		return;
	}	

	//arg 2 is the deck num of the target lift
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

	//checking to make sure we're inside the origin lift ent right now,
	//else naughty people could trigger lifts anywhere they please
	VectorCopy( lift->r.mins, mins );
	VectorCopy( lift->r.maxs, maxs );

	//pad out the boundaries a little bit, just to be safe
	VectorScale( mins, 1.5, mins );
	VectorScale( maxs, 1.5, maxs );

	numEnts = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	/*if ( !numEnts )
	{
		trap_SendServerCommand( ent-g_entities, "print \"You are not inside a turbolift\n\" " );	
		return;
	}*/

	//G_Printf( "%f %f %f, %f %f %f", mins[0], mins[1], mins[2], maxs[0], maxs[1], maxs[2] );

	for ( i = 0; i<numEnts; i++ )
	{
		touched = &g_entities[touch[i]];

		if ( !touched || !touched->client )
			continue;

		//yay we found us!
		if ( touched->client->ps.clientNum == ent->client->ps.clientNum )
		{
			found = qtrue;
			break;
		}
	}

	/*if ( found != qtrue )
	{
		trap_SendServerCommand( ent-g_entities, "print \"You are not inside a turbolift.\n\" " );	
		return;
	}*/

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
ClientCommand
=================
*/
void ClientCommand( int clientNum ) 
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
		//Laser_Gen( ent, 1 );//1=Laser, 2=Flashlight
		Cmd_Laser_f( ent );
	else if (Q_stricmp (cmd, "flashlight") == 0)
		//Laser_Gen( ent, 2 );
		Cmd_FlashLight_f( ent );
	else if (Q_stricmp (cmd, "cloak") == 0 && rpg_nocloak.integer == 0 )
		Cmd_Cloak_f( ent );
	else if (Q_stricmp (cmd, "flight") == 0 && rpg_noflight.integer == 0 )
		Cmd_Flight_f( ent );
	else if (Q_stricmp (cmd, "evasuit") == 0)
		Cmd_EvoSuit_f( ent );
	//else if (Q_stricmp (cmd, "giveto") == 0 )
		//Cmd_GiveTo_f( ent );
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
/*	else if (Q_stricmp (cmd, "ani") == 0 )
		Cmd_Ani_f(ent);*/
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
	/*else if (Q_stricmp (cmd, "playAnim") == 0 )
		Cmd_PlayAnim_f(ent);*/
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
	//else if (Q_stricmp(cmd, "die") == 0 )
	//	Cmd_Die_f(ent);
	else
		trap_SendServerCommand( clientNum, va("print \"unknown cmd %s\n\"", cmd ) );
}

//--------
