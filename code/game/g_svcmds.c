// Copyright (C) 1999-2000 Id Software, Inc.
//

// this file holds commands that can be executed by the server console, but not remote clients

#include "g_local.h"


/*
==============================================================================

PACKET FILTERING
 

You can add or remove addresses from the filter list with:

addip <ip>
removeip <ip>

The ip address is specified in dot format, and any unspecified digits will match any value, so you can specify an entire class C network with "addip 192.246.40".

Removeip will only remove an address specified exactly the same way.  You cannot addip a subnet, then removeip a single host.

listip
Prints the current list of filters.

g_filterban <0 or 1>

If 1 (the default), then ip addresses matching the current list will be prohibited from entering the game.  This is the default setting.

If 0, then only addresses matching the list will be allowed.  This lets you easily set up a private game, or a game that only allows players from your local network.


==============================================================================
*/

// extern	vmCvar_t	g_banIPs;
// extern	vmCvar_t	g_filterBan;


typedef struct ipFilter_s
{
	unsigned	mask;
	unsigned	compare;
} ipFilter_t;

typedef struct
{
	unsigned long	playerID;
	char			playerName[36];
	char			banReason[128];
} idFilter_t;

#define	MAX_IPFILTERS	1024
#define MAX_IDFILTERS	1024

static ipFilter_t	ipFilters[MAX_IPFILTERS];
static int			numIPFilters;

static idFilter_t	idFilters[MAX_IDFILTERS];
static int			numIDFilters;

/*
=================
StringToFilter
=================
*/
static qboolean StringToFilter (char *s, ipFilter_t *f)
{
	char	num[128];
	int		i, j;
	byte	b[4];
	byte	m[4];
	
	for (i=0 ; i<4 ; i++)
	{
		b[i] = 0;
		m[i] = 0;
	}
	
	for (i=0 ; i<4 ; i++)
	{
		if (*s < '0' || *s > '9')
		{
			G_Printf( "Bad filter address: %s\n", s );
			return qfalse;
		}
		
		j = 0;
		while (*s >= '0' && *s <= '9')
		{
			num[j++] = *s++;
		}
		num[j] = 0;
		b[i] = atoi(num);
		if (b[i] != 0)
			m[i] = 255;

		if (!*s)
			break;
		s++;
	}
	
	f->mask = *(unsigned *)m;
	f->compare = *(unsigned *)b;
	
	return qtrue;
}

/*
=================
UpdateIPBans
=================
*/
static void UpdateIPBans (void)
{
	byte	b[4];
	int		i;
	char	iplist[MAX_INFO_STRING];

	*iplist = 0;
	for (i = 0 ; i < numIPFilters ; i++)
	{
		if (ipFilters[i].compare == 0xffffffff)
			continue;

		*(unsigned *)b = ipFilters[i].compare;
		Com_sprintf( iplist + strlen(iplist), sizeof(iplist) - strlen(iplist), 
			"%i.%i.%i.%i ", b[0], b[1], b[2], b[3]);
	}

	trap_Cvar_Set( "g_banIPs", iplist );
}

/*
=================
G_FilterPacket
=================
*/
qboolean G_FilterPacket (char *from)
{
	int		i;
	unsigned	in;
	byte m[4];
	char *p;

	i = 0;
	p = from;
	while (*p && i < 4) {
		m[i] = 0;
		while (*p >= '0' && *p <= '9') {
			m[i] = m[i]*10 + (*p - '0');
			p++;
		}
		if (!*p || *p == ':')
			break;
		i++, p++;
	}
	
	in = *(unsigned *)m;

	for (i=0 ; i<numIPFilters ; i++)
	{
		if ( (in & ipFilters[i].mask) == ipFilters[i].compare)
		{
			return g_filterBan.integer != 0;
		}
	}

	return g_filterBan.integer == 0;
}

/*
=================
AddIP
=================
*/
static void AddIP( char *str )
{
	int		i;

	for (i = 0 ; i < numIPFilters ; i++)
		if (ipFilters[i].compare == 0xffffffff)
			break;		// free spot

	if (i == numIPFilters)
	{
		if (numIPFilters == MAX_IPFILTERS)
		{
			G_Printf ("IP filter list is full\n");
			return;
		}
		numIPFilters++;
	}
	
	if (!StringToFilter (str, &ipFilters[i]))
		ipFilters[i].compare = 0xffffffffu;

	UpdateIPBans();
}

/*
=================
G_ProcessIPBans
=================
*/
void G_ProcessIPBans(void) 
{
	char *s, *t;
	char		str[MAX_TOKEN_CHARS];

	Q_strncpyz( str, g_banIPs.string, sizeof(str) );

	for (t = s = g_banIPs.string; *t; /* */ ) {
		s = strchr(s, ' ');
		if (!s)
			break;
		while (*s == ' ')
			*s++ = 0;
		if (*t)
			AddIP( t );
		t = s;
	}
}

/*
============================================
TiM: Client side ID filter system
============================================
*/

qboolean CheckID( char *id )
{
	int i;

	//TiM - screw it if we haven't got a valid ID yet
	if ( atoul( id ) == SECURITY_PID )
		return qfalse;

	for( i = 0; i < numIDFilters; i++ )
	{
		if ( atoul(id) == idFilters[i].playerID )
			return qtrue;
	}

	return qfalse;
}

static void UpdateIDBans (void)
{
	fileHandle_t	f;
	int				i;
	char			buffer[1024];
	idFilter_t		*id;

	//TiM: Create and/reset the ban file
	trap_FS_FOpenFile( "RPG-X_Banned_Players.txt", &f, FS_WRITE );

	if ( !f )
	{
		G_Printf( "ERROR: Couldn't update the ban file.\n" );	
		return;
	}

	//file header
	memset( buffer, 0, sizeof( buffer ) );
	Q_strcat( buffer, sizeof(buffer), "//***************************************************\n" );
	Q_strcat( buffer, sizeof(buffer), "//RPG-X Banned Users ID List\n" );
	Q_strcat( buffer, sizeof(buffer), "//\n" );
	Q_strcat( buffer, sizeof(buffer), "//The formatting for each entry goes as such:\n" );
	Q_strcat( buffer, sizeof(buffer), "//{\n" );
	Q_strcat( buffer, sizeof(buffer), "//\t-ID : Unique Player ID\n" );
	Q_strcat( buffer, sizeof(buffer), "//\t-Name : User name of the banned player\n" );
	Q_strcat( buffer, sizeof(buffer), "//\t-Ban Reason : Reason for being banned.\n" );
	Q_strcat( buffer, sizeof(buffer), "//}\n" );
	Q_strcat( buffer, sizeof(buffer), "//\n" );
	Q_strcat( buffer, sizeof(buffer), "//***************************************************\n" );
	Q_strcat( buffer, sizeof(buffer), "\n" );

	//write the header to the file
	trap_FS_Write( buffer, strlen(buffer), f );

	//write out the data for each banned player
	for ( i = 0; i < numIDFilters; i++ )
	{
		id = &idFilters[i];

		if ( !id || id->playerID == 0 )
			continue;

		memset( buffer, 0, sizeof( buffer ) );

		//will produce this output:
		//i
		//{
		//	<ID>
		//	<Name>
		//	<Reason>
		//}
		//When parsed back in, the line breaks will be used to divide it up

		Com_sprintf( buffer, sizeof( buffer ), "%i\n{\n\t%ul\n\t\"%s\"\n\t\"%s\"\n}\n\n", i, id->playerID, id->playerName, id->banReason );

		trap_FS_Write( buffer, strlen(buffer), f );
	}

	trap_FS_FCloseFile(	f );
}

static void AddID( idFilter_t *id )
{
	int	i;

	for ( i = 0; i < numIDFilters; i++ )
		if ( idFilters[i].playerID == 0 ) break;

	if ( i == numIDFilters )
	{
		if ( i >= MAX_IDFILTERS )
		{
			G_Printf( "ID Filter list is full.\n" );
			return;
		}

		numIDFilters++;
	}

	//idFilters[i].playerID	=	id->playerID;
	//idFilters[i].playerName =	id->playerName;
	//idFilters[i].banReason	=	id->banReason;
	memcpy( &idFilters[i], id, sizeof( idFilter_t ) );

}

void G_ProcessIDBans( void )
{
	fileHandle_t	f;
	int				fileLen;
	char			buffer[16000];
	char			*token, *filePtr;
	idFilter_t		id;

	fileLen = trap_FS_FOpenFile( "RPG-X_Banned_Players.txt", &f, FS_READ );

	if ( !f || !fileLen )
		return;

	trap_FS_Read( buffer, fileLen, f );

	if ( !buffer[0] )
		return;

	buffer[fileLen] = '\0';

	trap_FS_FCloseFile( f );

	COM_BeginParseSession();

	filePtr = buffer;

	while ( 1 )
	{
		token = COM_Parse( &filePtr );
		if ( !token[0] ) break;

		if ( !Q_stricmp( token, "{" ) )
		{
			memset( &id, 0, sizeof( id ) );

			token = COM_ParseExt( &filePtr, qtrue );
			if ( !token[0] ) continue;
			
			//parse player id
			id.playerID = atoul( token );
			
			token = COM_ParseExt( &filePtr, qtrue );
			if ( !token[0] ) continue;

			//parse player name
			Q_strncpyz( id.playerName, token, sizeof( id.playerName ) );

			token = COM_ParseExt( &filePtr, qtrue );
			if ( !token[0] ) continue;

			//parse ban reason
			Q_strncpyz( id.banReason, token, sizeof( id.banReason ) );

			AddID( &id );
		}
	}

	G_Printf( "%i ban entries were successfully loaded.\n", numIDFilters );
}

/*
=================
Svcmd_BanUser_f
=================
*/
extern void ClientCleanName( const char *in, char *out, int outSize );

void Svcmd_BanUser_f( void )
{
	char		str[MAX_TOKEN_CHARS];
	char		userInfo[MAX_TOKEN_CHARS];
	idFilter_t	id;
	int			playerNum;
	char		*ip;

	if ( trap_Argc() < 2 )
	{
		G_Printf("Usage: banUser <client ID> <reason for banning>\n");
		return;		
	}

	trap_Argv( 1, str, sizeof( str ) );

	playerNum = atoi(str);
	if ( playerNum > MAX_CLIENTS || playerNum < 0 || !g_entities[playerNum].client )
	{
		G_Printf("Error: Player ID wasn't valid.\n");
		return;			
	}
	
	trap_GetUserinfo( playerNum, userInfo, sizeof( userInfo ) );
	if ( !userInfo[0] )
		return;

	//get unique Ban ID
	id.playerID = atoul( Info_ValueForKey( userInfo, "sv_securityCode" ) );
	
	//Get player name and clean it of color tags
	Q_strncpyz( id.playerName, Q_CleanStr(Info_ValueForKey( userInfo, "name" )), sizeof( id.playerName ) );
	//( Info_ValueForKey( userInfo, "name" ), id.playerName, sizeof( id.playerName ) );
	
	//get ban reason
	trap_Argv( 2, id.banReason, sizeof( id.banReason ) );

	if ( !id.banReason[0] )
		Q_strncpyz( id.banReason, "No reason given.", sizeof( id.banReason ) );

	AddID( &id );
	
	ip = g_entities[playerNum].client->pers.ip;

	UpdateIDBans();

	//G_Printf( S_COLOR_RED "%s\n", ip );

	//Scooter's filter list
	if( Q_stricmp( ip, "localhost" )		//localhost
		&& Q_strncmp( ip, "10.", 3 )		//class A
		&& Q_strncmp( ip, "172.16.", 7 )	//class B
		&& Q_strncmp( ip, "192.168.", 8 )	//class C
		&& Q_strncmp( ip, "127.", 4 )		//loopback
		&& Q_strncmp( ip, "169.254.", 8 )	//link-local
		)
	{
		AddIP( ip );
		G_Printf( "User: %s ( %i - %s ) ^7was successfully banned.\n", Info_ValueForKey( userInfo, "name" ), playerNum, ip );
	}

	trap_DropClient( playerNum, "Banned from the server" );
	G_Printf( "User: %s ( %i ) ^7was successfully banned.\n", id.playerName, playerNum );
}

/*
=================
Svcmd_FindID_f
=================
*/
void Svcmd_FindID_f ( void )
{
	char		str[MAX_TOKEN_CHARS];
	char		outputBuf[MAX_TOKEN_CHARS];
	char		searchLine[256];
	char		name[36]; //local stores to ensure pointer data isn't carried over.
	char		reason[64];
	int			i;
	idFilter_t	*id;
	int			resultsFound = 0;

	if ( trap_Argc() < 2 )
	{
		G_Printf("Usage: findUser <search string in name and/or reason>\n");
		return;		
	}

	trap_Argv( 1, str, sizeof( str ) );
	Q_strlwr( str );

	for ( i = 0; i < numIDFilters; i++ )
	{
		id = &idFilters[i];

		if ( !id || id->playerID <= 0 )
			continue;

		memset( name, 0, sizeof( name ) );
		Q_strncpyz( name, id->playerName, sizeof(name) );
		Q_strlwr( name );

		memset( reason, 0, sizeof( reason ) );
		Q_strncpyz( reason, id->banReason, sizeof( reason ) );
		Q_strlwr( reason );

		if ( strstr( name, str  ) != NULL || strstr( reason, str ) != NULL )
		{
			Com_sprintf( searchLine, sizeof( searchLine ), "%4i %-16.16s %-45.45s\n", i, id->playerName, id->banReason );
			
			if ( !resultsFound )
				Q_strncpyz( outputBuf, searchLine, sizeof(outputBuf) );
			else
				Q_strcat( outputBuf, sizeof(outputBuf), searchLine  );

			resultsFound++;
		}
	}

	G_Printf( "%i %s found.\n\n", resultsFound, resultsFound == 1 ? "result" : "results" );

	if ( resultsFound > 0 )
	{
		G_Printf( "%-4.4s %-16.16s %-45.45s\n", "ID:", "Name:", "Reason:" );
		G_Printf( "%-4.4s %-16.16s %-45.45s\n", "----", "-----------------", "-------------------------------------" );
		G_Printf( outputBuf );
	}
}

/*
=================
Svcmd_DeleteID_f
=================
*/
void Svcmd_RemoveID_f ( void )
{
	char str[MAX_TOKEN_CHARS];
	idFilter_t	*id;

	if ( trap_Argc() < 2 )
	{
		G_Printf( "Usage: removeID <Ban ID>\n" );
		return;
	}

	trap_Argv( 1, str, sizeof( str ) );

	id = &idFilters[atoi(str)];
	if ( !id )
	{
		G_Printf( "Specified ID not found.\n" );
		return;
	}

	memset( id, 0, sizeof( idFilter_t ) );

	UpdateIDBans();
}

/*
=================
Svcmd_AddIP_f
=================
*/
void Svcmd_AddIP_f (void)
{
	char		str[MAX_TOKEN_CHARS];

	if ( trap_Argc() < 2 ) {
		G_Printf("Usage:  addip <ip-mask>\n");
		return;
	}

	trap_Argv( 1, str, sizeof( str ) );

	AddIP( str );

}

/*
=================
Svcmd_RemoveIP_f
=================
*/
void Svcmd_RemoveIP_f (void)
{
	ipFilter_t	f;
	int			i;
	char		str[MAX_TOKEN_CHARS];

	if ( trap_Argc() < 2 ) {
		G_Printf("Usage:  sv removeip <ip-mask>\n");
		return;
	}

	trap_Argv( 1, str, sizeof( str ) );

	if (!StringToFilter (str, &f))
		return;

	for (i=0 ; i<numIPFilters ; i++) {
		if (ipFilters[i].mask == f.mask	&&
			ipFilters[i].compare == f.compare) {
			ipFilters[i].compare = 0xffffffffu;
			G_Printf ("Removed.\n");

			UpdateIPBans();
			return;
		}
	}

	G_Printf ( "Didn't find %s.\n", str );
}

/*
===================
Svcmd_EntityList_f
===================
*/
void	Svcmd_EntityList_f (void) {
	int			e;
	gentity_t	*check;
	char		arg[MAX_QPATH*4];
	int			length = 0;
	//int			numArgs;

	if(trap_Argc() > 1) {
		trap_Argv(1, arg, sizeof(arg));
		length = strlen(arg);
	}

	check = g_entities+1;
	for (e = 1; e < level.num_entities ; e++, check++) {
		if ( !check->inuse ) {
			continue;
		}
		if(!arg[0]) {
			if ( check->classname && Q_stricmpn(check->classname, "noclass", 7) && Q_stricmpn(check->classname, "bodyque", 7) ) {
				G_Printf("%3i:", e);
				switch ( check->s.eType ) {
				case ET_GENERAL:
					G_Printf("ET_GENERAL          ");
					break;
				case ET_PLAYER:
					G_Printf("ET_PLAYER           ");
					break;
				case ET_ITEM:
					G_Printf("ET_ITEM             ");
					break;
				case ET_MISSILE:
					G_Printf("ET_MISSILE          ");
					break;
				case ET_MOVER:
				case ET_MOVER_STR: //RPG-X | GSIO01 | 13/05/2009
					G_Printf("ET_MOVER            ");
					break;
				case ET_BEAM:
					G_Printf("ET_BEAM             ");
					break;
				case ET_PORTAL:
					G_Printf("ET_PORTAL           ");
					break;
				case ET_SPEAKER:
					G_Printf("ET_SPEAKER          ");
					break;
				case ET_PUSH_TRIGGER:
					G_Printf("ET_PUSH_TRIGGER     ");
					break;
				case ET_TELEPORT_TRIGGER:
					G_Printf("ET_TELEPORT_TRIGGER ");
					break;
				case ET_INVISIBLE:
					G_Printf("ET_INVISIBLE        ");
					break;
				default:
					G_Printf("%3i                 ", check->s.eType);
					break;
				}
				G_Printf("%s", check->classname);
				G_Printf("\n");
			}
		} else {
			if ( check->classname && Q_stricmpn(check->classname, "noclass", 7) && Q_stricmpn(check->classname, "bodyque", 7) && !Q_stricmpn(check->classname, arg, length)) {
				G_Printf("%3i:", e);
				switch ( check->s.eType ) {
				case ET_GENERAL:
					G_Printf("ET_GENERAL          ");
					break;
				case ET_PLAYER:
					G_Printf("ET_PLAYER           ");
					break;
				case ET_ITEM:
					G_Printf("ET_ITEM             ");
					break;
				case ET_MISSILE:
					G_Printf("ET_MISSILE          ");
					break;
				case ET_MOVER:
				case ET_MOVER_STR: //RPG-X | GSIO01 | 13/05/2009
					G_Printf("ET_MOVER            ");
					break;
				case ET_BEAM:
					G_Printf("ET_BEAM             ");
					break;
				case ET_PORTAL:
					G_Printf("ET_PORTAL           ");
					break;
				case ET_SPEAKER:
					G_Printf("ET_SPEAKER          ");
					break;
				case ET_PUSH_TRIGGER:
					G_Printf("ET_PUSH_TRIGGER     ");
					break;
				case ET_TELEPORT_TRIGGER:
					G_Printf("ET_TELEPORT_TRIGGER ");
					break;
				case ET_INVISIBLE:
					G_Printf("ET_INVISIBLE        ");
					break;
				default:
					G_Printf("%3i                 ", check->s.eType);
					break;
				}
				G_Printf("%s", check->classname);
				G_Printf("\n");
			}
		}
	}
}

gclient_t	*ClientForString( const char *s ) {
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
}

/*
===================
Svcmd_ForceTeam_f

forceteam <player> <team>
===================
*/
void	Svcmd_ForceTeam_f( void ) {
	gclient_t	*cl;
	char		str[MAX_TOKEN_CHARS];

	// find the player
	trap_Argv( 1, str, sizeof( str ) );
	cl = ClientForString( str );
	if ( !cl ) {
		return;
	}

	// set the team
	trap_Argv( 2, str, sizeof( str ) );
	SetTeam( &g_entities[cl - level.clients], str );
}

/*
=================
Svcmd_LuaRestart_f
=================
*/
#ifdef G_LUA
static void Svcmd_LuaRestart_f(void)
{
	G_LuaShutdown();
	G_LuaInit();
}
#endif

/*
=================
ConsoleCommand

=================
*/
qboolean	ConsoleCommand( void ) { //void
	char	cmd[MAX_TOKEN_CHARS];
	//gentity_t	*ent;

	trap_Argv( 0, cmd, sizeof( cmd ) );

	/*if ( Q_stricmp (cmd, "kick2") == 0 ) {
		Svcmd_Kick2_f();
		return qtrue;
	}*/

	#ifdef G_LUA
	if(Q_stricmp(cmd, "lua_status") == 0)
	{
		G_LuaStatus(NULL);
		return qtrue;
	}

	if(Q_stricmp(cmd, "lua_restart") == 0)
	{
		Svcmd_LuaRestart_f();
		return qtrue;
	}
	#endif

	if ( Q_stricmp (cmd, "entitylist") == 0 ) {
		Svcmd_EntityList_f();
		return qtrue;
	}

	if ( Q_stricmp (cmd, "forceteam") == 0 ) {
		Svcmd_ForceTeam_f();
		return qtrue;
	}

	if (Q_stricmp (cmd, "game_memory") == 0) {
		Svcmd_GameMem_f();
		return qtrue;
	}

	if (Q_stricmp (cmd, "addbot") == 0) {
		Svcmd_AddBot_f();
		return qtrue;
	}

	if (Q_stricmp (cmd, "botlist") == 0) {
		Svcmd_BotList_f();
		return qtrue;
	}

	if (Q_stricmp (cmd, "abort_podium") == 0) {
		Svcmd_AbortPodium_f();
		return qtrue;
	}

	if (Q_stricmp (cmd, "addip") == 0) {
		Svcmd_AddIP_f();
		return qtrue;
	}

	if (Q_stricmp (cmd, "removeip") == 0) {
		Svcmd_RemoveIP_f();
		return qtrue;
	}

	if (Q_stricmp (cmd, "listip") == 0) {
		trap_SendConsoleCommand( EXEC_INSERT, "g_banIPs\n" );
		return qtrue;
	}

	if (Q_stricmp (cmd, "banUser") == 0) {
		Svcmd_BanUser_f();
		return qtrue;
	}

	if ( Q_stricmp (cmd, "findID") == 0 ) {
		Svcmd_FindID_f();
		return qtrue;
	}

	if ( Q_stricmp (cmd, "removeID") == 0 ) {
		Svcmd_RemoveID_f();
		return qtrue;
	}

	return qfalse;
}

