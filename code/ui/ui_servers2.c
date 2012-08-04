// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=======================================================================

MULTIPLAYER MENU (SERVER BROWSER)

=======================================================================
*/


#include "ui_local.h"


static menubitmap_s			s_find_server;
static menubitmap_s			s_specific_server;
static menubitmap_s			s_create_server;

#define RPGX_SERVERNAME			"RPG-X2"

#define MAX_GLOBALSERVERS		256
#define MAX_PINGREQUESTS		16
#define MAX_ADDRESSLENGTH		64
#define MAX_HOSTNAMELENGTH		64
#define MAX_MAPNAMELENGTH		30
#define MAX_LISTBOXITEMS		256
#define MAX_LOCALSERVERS		256
#define MAX_STATUSLENGTH		64
#define MAX_LISTBOXWIDTH		67

#define ART_UNKNOWNMAP			"levelshots/unknownmap"

#define ID_MASTER			10
#define ID_GAMETYPE			11
#define ID_SORTKEY			12
#define ID_SHOW_FULL		13
#define ID_SHOW_EMPTY		14
#define ID_LIST				15
#define ID_SCROLL_UP		16
#define ID_SCROLL_DOWN		17
#define ID_BACK				18
#define ID_REFRESH			19
#define ID_SPECIFY			20
#define ID_CREATE			21
#define ID_CONNECT			22
#define ID_REMOVE			23
#define ID_STOPREFRESH		24
#define ID_FINDSERVER		25
#define ID_FAVORITE			26

#define	ID_MAINMENU			100

#define AS_LOCAL			0
//#define AS_MPLAYER			1
#define AS_GLOBAL1			1
#define AS_GLOBAL2			2
#define AS_GLOBAL3			3
#define AS_GLOBAL4			4
#define AS_GLOBAL5			5
#define AS_FAVORITES		6

#define SORT_HOST			0
#define SORT_MAP			1
#define SORT_CLIENTS		2
#define SORT_GAME			3
#define SORT_PING			4

/*#define GAMES_ALL			0
#define GAMES_FFA			1
#define GAMES_TEAMPLAY		2
#define GAMES_TOURNEY		3
#define GAMES_CTF			4
#define GAMES_EXCESSIVE			5
#define GAMES_FEDERATION			6
#define GAMES_GLADIATOR			7
#define GAMES_IN2TAGIB			8
#define GAMES_PINBALL			9
#define GAMES_Q32EF			10
#define GAMES_RPG			11
#define GAMES_TEAMELITE			12
#define GAMES_RPGX			13*/

#define GAMES_ALL			0
#define GAMES_RPGX			1
#define GAMES_FFA			2
#define GAMES_TEAMPLAY		3
#define GAMES_TOURNEY		4
#define GAMES_CTF			5

/*
static const char *master_items[] = {
	"Local",
	"Mplayer",
	"Internet",
	"Favorites",
	0
};

static const char *servertype_items[] = {
	"All",
	"Free For All",
	"Team Deathmatch",
	"Tournament",
	"Capture the Flag",
	0
};
static const char *sortkey_items[] = {
	"Server Name",
	"Map Name",
	"Open Player Spots",
	"Game Type",
	"Ping Time",
	0
};
*/

static int master_items[] = 
{
	MNT_MASTER_ITEMS_LOCAL,
//	MNT_MASTER_ITEMS_MPLAYER,
	MNT_MASTER_ITEMS_INTERNET1,
	MNT_MASTER_ITEMS_INTERNET2,
	MNT_MASTER_ITEMS_INTERNET3,
	MNT_MASTER_ITEMS_INTERNET4,
	MNT_MASTER_ITEMS_INTERNET5,
	MNT_MASTER_ITEMS_FAVORITES,
	0
};

static int servertype_items[] = 
{
	MNT_SERVERTYPE_ALL,
	MNT_SERVERTYPE_RPGX,
	MNT_SERVERTYPE_FREE_FOR_ALL,
	MNT_SERVERTYPE_TEAM_DEATHMATCH,
	MNT_SERVERTYPE_TOURNAMENT,
	MNT_SERVERTYPE_CAPTURETHEFLAG,
	/*MNT_SERVERTYPE_EXCESSIVE,
	MNT_SERVERTYPE_FEDERATION,
	MNT_SERVERTYPE_GLADIATOR,
	MNT_SERVERTYPE_IN2TAGIB,
	MNT_SERVERTYPE_PINBALL,
	MNT_SERVERTYPE_Q32EF,
	MNT_SERVERTYPE_RPG,
	MNT_SERVERTYPE_TEAMELITE,*/
	0
};

static int sortkey_items[] = 
{
	MNT_SORTKEY_SERVER_NAME,
	MNT_SORTKEY_MAP_NAME,
	MNT_SORTKEY_OPENPLAYER,
	MNT_SORTKEY_GAMETYPE,
	MNT_SORTKEY_PINGTIME,
	0
};

static int noyes_items[] = 
{
	MNT_NO,
	MNT_YES,
	0
};

static float server_buttons[5][2] = 
{
{274, 72},	//	Server type
{274, 92},	//	Sort by
{274,112},	//	Game Type
{274,132},	//	All
{274,152},	//	Empty
};



char* gamenames[] = {
	"DM ",	// deathmatch
	"1v1",	// tournament
	"SP ",	// single player
	"TDM",	// team deathmatch
	"CTF",	// capture the flag
	/*"EXCESSIVE",
	"FEDERATION",
	"GLADIATOR",
	"IN2TAGIB",
	"PINBALL",
	"Q32EF",
	"RPG",
	"TEAMELITE",
	"RPG-X", //The best gametype of them all ;)*/
	"???",
};

static char* netnames[] = {
	"???",
	"UDP",
	"IPX",
	NULL
};

static char quake3worldMessage[] = "For more information, please visit :   www.rpg-x.net   www.ravensoft.com/eliteforce   www.startrek.com";

typedef struct {
	char	adrstr[MAX_ADDRESSLENGTH];
	int		start;
} pinglist_t;

typedef struct servernode_s {
	char	adrstr[MAX_ADDRESSLENGTH];
	char	hostname[MAX_HOSTNAMELENGTH];
	char	mapname[MAX_MAPNAMELENGTH];
	int		numclients;
	int		maxclients;
	int		pingtime;
	int		gametype;
	char	gamename[12];
	int		nettype;
	int		minPing;
	int		maxPing;
	qboolean	isPure;
	qboolean	isExpansion;
} servernode_t; 

typedef struct {
	char			buff[MAX_LISTBOXWIDTH];
	servernode_t*	servernode;
} table_t;

typedef struct {
	menuframework_s		menu;

	menubitmap_s		mainmenu;
	menubitmap_s		back;
	menulist_s			master;
	menulist_s			gametype;
	menulist_s			sortkey;
	menulist_s			showfull;
	menulist_s			showempty;

	menulist_s			list;
	menubitmap_s		mappic;
	menubitmap_s		arrows;
	menubitmap_s		up;
	menubitmap_s		down;
	menutext_s			status;
	menutext_s			statusbar;

	menubitmap_s		remove;
	menubitmap_s		refresh;
	menubitmap_s		stoprefresh;
	menubitmap_s		go;
	menubitmap_s		favorite;

	pinglist_t			pinglist[MAX_PINGREQUESTS];
	table_t				table[MAX_LISTBOXITEMS];
	char*				items[MAX_LISTBOXITEMS];
	int					numqueriedservers;
	int					*numservers;
	servernode_t		*serverlist;	
	int					currentping;
	qboolean			refreshservers;
	int					nextpingtime;
	int					maxservers;
	int					refreshtime;
	char				favoriteaddresses[MAX_FAVORITESERVERS][MAX_ADDRESSLENGTH];
	int					numfavoriteaddresses;
} arenaservers_t;

static arenaservers_t	g_arenaservers;

typedef struct 
{
	menuframework_s		menu;
	menubitmap_s		main;

} chooseservertype_t;
static chooseservertype_t		g_chooseservertype;

static servernode_t		g_globalserverlist[MAX_GLOBALSERVERS];
static int				g_numglobalservers;
static servernode_t		g_localserverlist[MAX_LOCALSERVERS];
static int				g_numlocalservers;
static servernode_t		g_favoriteserverlist[MAX_FAVORITESERVERS];
static int				g_numfavoriteservers;
//static servernode_t		g_mplayerserverlist[MAX_GLOBALSERVERS];
//static int				g_nummplayerservers;
static int				g_servertype;
static int				g_gametype;
static int				g_sortkey;
static int				g_emptyservers;
static int				g_fullservers;


/*
=================
ArenaServers_MaxPing
=================
*/
int ArenaServers_MaxPing( void ) { /*static*/
	int		maxPing;

	maxPing = (int)trap_Cvar_VariableValue( "cl_maxPing" );
	if( maxPing < 100 ) {
		maxPing = 100;
	}
	return maxPing;
}


/*
=================
ArenaServers_Compare
=================
*/
static int QDECL ArenaServers_Compare( const void *arg1, const void *arg2 ) {
	float			f1;
	float			f2;
	servernode_t*	t1;
	servernode_t*	t2;

	t1 = (servernode_t *)arg1;
	t2 = (servernode_t *)arg2;

	switch( g_sortkey ) {
	case SORT_HOST:
		return Q_stricmp( t1->hostname, t2->hostname );

	case SORT_MAP:
		return Q_stricmp( t1->mapname, t2->mapname );

	case SORT_CLIENTS:
		f1 = t1->maxclients - t1->numclients;
		if( f1 < 0 ) {
			f1 = 0;
		}

		f2 = t2->maxclients - t2->numclients;
		if( f2 < 0 ) {
			f2 = 0;
		}

		if( f1 < f2 ) {
			return 1;
		}
		if( f1 == f2 ) {
			return 0;
		}
		return -1;

	case SORT_GAME:
		if( t1->gametype < t2->gametype ) {
			return -1;
		}
		if( t1->gametype == t2->gametype ) {
			return 0;
		}
		return 1;

	case SORT_PING:
		if( t1->pingtime < t2->pingtime ) {
			return -1;
		}
		if( t1->pingtime > t2->pingtime ) {
			return 1;
		}
		return Q_stricmp( t1->hostname, t2->hostname );
	}

	return 0;
}


/*
=================
ArenaServers_Go
=================
*/
static void ArenaServers_Go( void ) {
	servernode_t*	servernode;

	servernode = g_arenaservers.table[g_arenaservers.list.curvalue].servernode;
	if( servernode ) {
		trap_Cmd_ExecuteText( EXEC_APPEND, va( "connect %s\n", servernode->adrstr ) );
	}
}


/*
=================
ArenaServers_UpdatePicture
=================
*/
static void ArenaServers_UpdatePicture( void ) {
	static char		picname[64];
	servernode_t*	servernodeptr;
	qhandle_t hpic;

	if( !g_arenaservers.list.numitems ) {
		g_arenaservers.mappic.generic.name = NULL;
	}
	else {
		servernodeptr = g_arenaservers.table[g_arenaservers.list.curvalue].servernode;
		hpic = trap_R_RegisterShaderNoMip( va("levelshots/%s.tga", servernodeptr->mapname) );
		if (!hpic)
		{
			Com_sprintf( picname, sizeof(picname), "levelshots/unknownmap.tga", servernodeptr->mapname );
		}
		else
		{
			Com_sprintf( picname, sizeof(picname), "levelshots/%s.tga", servernodeptr->mapname );
		}
		g_arenaservers.mappic.generic.name = picname;
	}

	// force shader update during draw
	g_arenaservers.mappic.shader = 0;
}


/*
=================
ArenaServers_UpdateMenu
=================
*/
static void ArenaServers_UpdateMenu( void ) {
	int				i;
	int				j;
	int				count;
	char*			buff;
	servernode_t*	servernodeptr;
	table_t*		tableptr;
	char			*pingColor, *pongColor;

	if( g_arenaservers.numqueriedservers > 0 ) {
		// servers found
		if( g_arenaservers.refreshservers && ( g_arenaservers.currentping <= g_arenaservers.numqueriedservers ) ) {
			// show progress
			Com_sprintf( g_arenaservers.status.string, MAX_STATUSLENGTH, menu_normal_text[MNT_CNT_SERVERS], g_arenaservers.currentping, g_arenaservers.numqueriedservers);
			g_arenaservers.statusbar.string  = (char*)menu_normal_text[MNT_SPACETOSTOP];
			qsort( g_arenaservers.serverlist, *g_arenaservers.numservers, sizeof( servernode_t ), ArenaServers_Compare);
		}
		else {
			// all servers pinged - enable controls
			g_arenaservers.master.generic.flags		&= ~QMF_GRAYED;
			g_arenaservers.gametype.generic.flags	&= ~QMF_GRAYED;
			g_arenaservers.sortkey.generic.flags	&= ~QMF_GRAYED;
			g_arenaservers.showempty.generic.flags	&= ~QMF_GRAYED;
			g_arenaservers.showfull.generic.flags	&= ~QMF_GRAYED;
			g_arenaservers.list.generic.flags		&= ~QMF_GRAYED;
			g_arenaservers.refresh.generic.flags	&= ~QMF_GRAYED;
			g_arenaservers.go.generic.flags			&= ~QMF_GRAYED;
			
			// update status bar
			if(( g_servertype >= AS_GLOBAL1) && ( g_servertype <= AS_GLOBAL5)) 
			{
				g_arenaservers.statusbar.string = quake3worldMessage;
			}
			else 
			{
				g_arenaservers.statusbar.string = "";
			}
		}
	}
	else {
		// no servers found
		if( g_arenaservers.refreshservers ) 
		{
			strcpy( g_arenaservers.status.string,menu_normal_text[MNT_SCANNINGFORSERVERS] );
			g_arenaservers.statusbar.string = (char*)menu_normal_text[MNT_SPACETOSTOP];

			// disable controls during refresh
			g_arenaservers.master.generic.flags		|= QMF_GRAYED;
			g_arenaservers.gametype.generic.flags	|= QMF_GRAYED;
			g_arenaservers.sortkey.generic.flags	|= QMF_GRAYED;
			g_arenaservers.showempty.generic.flags	|= QMF_GRAYED;
			g_arenaservers.showfull.generic.flags	|= QMF_GRAYED;
			g_arenaservers.list.generic.flags		|= QMF_GRAYED;
			g_arenaservers.refresh.generic.flags	|= QMF_GRAYED;
			g_arenaservers.go.generic.flags			|= QMF_GRAYED;

			g_arenaservers.statusbar.string  = (char*)menu_normal_text[MNT_ACCESSING];
		}
		else {
			if( g_arenaservers.numqueriedservers < 0 ) {
				strcpy(g_arenaservers.status.string,menu_normal_text[MNT_NORESPONSE_MASTER] );
			}
			else {
				strcpy(g_arenaservers.status.string,menu_normal_text[MNT_NO_SERVERS] );
			}

			// update status bar
			if(( g_servertype >= AS_GLOBAL1) && ( g_servertype <= AS_GLOBAL5)) 
			{
				g_arenaservers.statusbar.string = quake3worldMessage;
			}
			else {
				g_arenaservers.statusbar.string = "";
			}

			// end of refresh - set control state
			g_arenaservers.master.generic.flags		&= ~QMF_GRAYED;
			g_arenaservers.gametype.generic.flags	&= ~QMF_GRAYED;
			g_arenaservers.sortkey.generic.flags	&= ~QMF_GRAYED;
			g_arenaservers.showempty.generic.flags	&= ~QMF_GRAYED;
			g_arenaservers.showfull.generic.flags	&= ~QMF_GRAYED;
			g_arenaservers.list.generic.flags		|= QMF_GRAYED;
			g_arenaservers.refresh.generic.flags	&= ~QMF_GRAYED;
			g_arenaservers.go.generic.flags			|= QMF_GRAYED;

		}

		// zero out list box
		g_arenaservers.list.numitems = 0;
		g_arenaservers.list.curvalue = 0;
		g_arenaservers.list.top      = 0;

		// update picture
		ArenaServers_UpdatePicture();
		return;
	}

	// build list box strings - apply culling filters
	servernodeptr = g_arenaservers.serverlist;
	count         = *g_arenaservers.numservers;
	for( i = 0, j = 0; i < count; i++, servernodeptr++ ) {
		tableptr = &g_arenaservers.table[j];
		tableptr->servernode = servernodeptr;
		buff = tableptr->buff;

		if( !g_emptyservers && !servernodeptr->numclients ) {
			continue;
		}

		if( !g_fullservers && ( servernodeptr->numclients == servernodeptr->maxclients ) ) {
			continue;
		}

		switch( g_gametype ) {

		case GAMES_ALL:
			break;

		case GAMES_FFA:
			if( servernodeptr->gametype != GT_FFA ) {
				continue;
			}
			break;

		case GAMES_TEAMPLAY:
			if( servernodeptr->gametype != GT_TEAM ) {
				continue;
			}
			break;

		case GAMES_TOURNEY:
			if( servernodeptr->gametype != GT_TOURNAMENT ) {
				continue;
			}
			break;

		case GAMES_CTF:
			if( servernodeptr->gametype != GT_CTF ) {
				continue;
			}
			break;

		/*case GAMES_EXCESSIVE:
			if( Q_stricmp(servernodeptr->gamename, "excessive") ) {
				continue;
			}
			break;
		case GAMES_FEDERATION:
			if( Q_stricmp(servernodeptr->gamename, "federation") ) {
				continue;
			}
			break;
		case GAMES_GLADIATOR:
			if( Q_stricmp(servernodeptr->gamename, "gladiator") ) {
				continue;
			}
			break;
		case GAMES_IN2TAGIB:
			if( Q_stricmp(servernodeptr->gamename, "in2tagib") ) {
				continue;
			}
			break;
		case GAMES_PINBALL:
			if( Q_stricmp(servernodeptr->gamename, "pinball") ) {
				continue;
			}
			break;
		case GAMES_Q32EF:
			if( Q_stricmp(servernodeptr->gamename, "q32ef") ) {
				continue;
			}
			break;
		case GAMES_RPG:
			if( Q_stricmp(servernodeptr->gamename, "rpg") ) {
				continue;
			}
			break;
		case GAMES_TEAMELITE:
			if( Q_stricmp(servernodeptr->gamename, "teamelite") ) {
				continue;
			}
			break;*/
		case GAMES_RPGX:
			if( Q_stricmp(servernodeptr->gamename, RPGX_SERVERNAME ) ) {
				continue;
			}
			break;

		}

		if( servernodeptr->pingtime < servernodeptr->minPing ) {
			pingColor = S_COLOR_BLUE;
		}
		else if( servernodeptr->maxPing && servernodeptr->pingtime > servernodeptr->maxPing ) {
			pingColor = S_COLOR_BLUE;
		}
		else if( servernodeptr->pingtime < 50 ) {
			pingColor = S_COLOR_WHITE;
		}
		else if( servernodeptr->pingtime < 200 ) {
			pingColor = S_COLOR_GREEN;
		}
		else if( servernodeptr->pingtime < 400 ) {
			pingColor = S_COLOR_YELLOW;
		}
		else {
			pingColor = S_COLOR_RED;
		}

		pongColor = S_COLOR_WHITE;
		Com_sprintf( buff, MAX_LISTBOXWIDTH, "%s%-31.31s %s%-11.11s %2d/%2d %-8.8s %3d", //31.31
			pongColor, servernodeptr->hostname, pingColor, servernodeptr->mapname, servernodeptr->numclients,
	 		servernodeptr->maxclients, servernodeptr->gamename, servernodeptr->pingtime);
		if (!servernodeptr->isPure) {	//prev length is 62, we can safely add 2 more chars.
			strcat(buff, "*");	//mark this unpure server!
		}
		j++;
	}

//	Com_sprintf( g_arenaservers.status.string, MAX_STATUSLENGTH, "%d of %d Arena Servers.", j, *g_arenaservers.numservers );

	g_arenaservers.list.numitems = j;
	g_arenaservers.list.curvalue = 0;
	g_arenaservers.list.top      = 0;

	if (!g_arenaservers.list.numitems)
	{
		g_arenaservers.go.generic.flags			|= QMF_GRAYED;
	}

	// update picture
	ArenaServers_UpdatePicture();
}


/*
=================
ArenaServers_Remove
=================
*/
static void ArenaServers_Remove( void )
{
	int				i;
	servernode_t*	servernodeptr;
	table_t*		tableptr;

	if (!g_arenaservers.list.numitems)
		return;

	// remove selected item from display list
	// items are in scattered order due to sort and cull
	// perform delete on list box contents, resync all lists

	tableptr      = &g_arenaservers.table[g_arenaservers.list.curvalue];
	servernodeptr = tableptr->servernode;

	// find address in master list
	for (i=0; i<g_arenaservers.numfavoriteaddresses; i++)
		if (!Q_stricmp(g_arenaservers.favoriteaddresses[i],servernodeptr->adrstr))
				break;

	// delete address from master list
	if (i <= g_arenaservers.numfavoriteaddresses-1)
	{
		if (i < g_arenaservers.numfavoriteaddresses-1)
		{
			// shift items up
			memcpy( &g_arenaservers.favoriteaddresses[i], &g_arenaservers.favoriteaddresses[i+1], (g_arenaservers.numfavoriteaddresses - i - 1)*sizeof(MAX_ADDRESSLENGTH));
		}
		g_arenaservers.numfavoriteaddresses--;
	}	

	// find address in server list
	for (i=0; i<g_numfavoriteservers; i++)
		if (&g_favoriteserverlist[i] == servernodeptr)
				break;

	// delete address from server list
	if (i <= g_numfavoriteservers-1)
	{
		if (i < g_numfavoriteservers-1)
		{
			// shift items up
			memcpy( &g_favoriteserverlist[i], &g_favoriteserverlist[i+1], (g_numfavoriteservers - i - 1)*sizeof(servernode_t));
		}
		g_numfavoriteservers--;
	}	

	g_arenaservers.numqueriedservers = g_arenaservers.numfavoriteaddresses;
	g_arenaservers.currentping       = g_arenaservers.numfavoriteaddresses;
}


/*
=================
ArenaServers_Insert
=================
*/
static void ArenaServers_Insert( char* adrstr, char* info, int pingtime )
{
	servernode_t*	servernodeptr;
	char*			s;
	int				i;

	if ((pingtime >= ArenaServers_MaxPing()) && (g_servertype != AS_FAVORITES))
	{
		// slow global or local servers do not get entered
		return;
	}

	if (*g_arenaservers.numservers >= g_arenaservers.maxservers) {
		// list full;
		servernodeptr = g_arenaservers.serverlist+(*g_arenaservers.numservers)-1;
	} else {
		// next slot
		servernodeptr = g_arenaservers.serverlist+(*g_arenaservers.numservers);
		(*g_arenaservers.numservers)++;
	}

	Q_strncpyz( servernodeptr->adrstr, adrstr, MAX_ADDRESSLENGTH );

	Q_strncpyz( servernodeptr->hostname, Info_ValueForKey( info, "hostname"), MAX_HOSTNAMELENGTH );
	Q_CleanStr( servernodeptr->hostname );
	Q_CleanStr( servernodeptr->hostname );
	Q_CleanStr( servernodeptr->hostname );
	Q_CleanStr( servernodeptr->hostname );

	Q_strncpyz( servernodeptr->mapname, Info_ValueForKey( info, "mapname"), MAX_MAPNAMELENGTH );
	Q_CleanStr( servernodeptr->mapname );
	Q_strlwr( servernodeptr->mapname );

	servernodeptr->numclients = atoi( Info_ValueForKey( info, "clients") );
	servernodeptr->maxclients = atoi( Info_ValueForKey( info, "sv_maxclients") );
	servernodeptr->pingtime   = pingtime;
	servernodeptr->minPing    = atoi( Info_ValueForKey( info, "minPing") );
	servernodeptr->maxPing    = atoi( Info_ValueForKey( info, "maxPing") );

	servernodeptr->isPure	  = atoi( Info_ValueForKey( info, "pure") );
	servernodeptr->isExpansion= atoi( Info_ValueForKey( info, "vv") );

	s = Info_ValueForKey( info, "nettype" );
	for (i=0; ;i++)
	{
		if (!netnames[i])
		{
			servernodeptr->nettype = 0;
			break;
		}
		else if (!Q_stricmp( netnames[i], s ))
		{
			servernodeptr->nettype = i;
			break;
		}
	}

	s = Info_ValueForKey( info, "game");
	i = atoi( Info_ValueForKey( info, "gametype") );
	if( i < 0 ) {
		i = 0;
	}
	else if( i > 5 ) {
		i = 5;
	}
	if( *s ) {
		servernodeptr->gametype = -1;
		Q_strncpyz( servernodeptr->gamename, s, sizeof(servernodeptr->gamename) );
	}
	else {
		servernodeptr->gametype = i;
		if (servernodeptr->isExpansion)	{
			Com_sprintf( servernodeptr->gamename, sizeof(servernodeptr->gamename), "vv-%s",gamenames[i]);
		} else {
			Q_strncpyz( servernodeptr->gamename, gamenames[i], sizeof(servernodeptr->gamename) );
		}
	}
}


/*
=================
ArenaServers_InsertFavorites

Insert nonresponsive address book entries into display lists.
=================
*/
void ArenaServers_InsertFavorites( void )
{
	int		i;
	int		j;
	char	info[MAX_INFO_STRING];

	// resync existing results with new or deleted cvars
	info[0] = '\0';
	Info_SetValueForKey( info, "hostname", "No Response" );
	for (i=0; i<g_arenaservers.numfavoriteaddresses; i++)
	{
		// find favorite address in refresh list
		for (j=0; j<g_numfavoriteservers; j++)
			if (!Q_stricmp(g_arenaservers.favoriteaddresses[i],g_favoriteserverlist[j].adrstr))
				break;

		if ( j >= g_numfavoriteservers)
		{
			// not in list, add it
			ArenaServers_Insert( g_arenaservers.favoriteaddresses[i], info, ArenaServers_MaxPing() );
		}
	}
}


/*
=================
ArenaServers_LoadFavorites

Load cvar address book entries into local lists.
=================
*/
void ArenaServers_LoadFavorites( void )
{
	int				i;
	int				j;
	int				numtempitems;
	char			emptyinfo[MAX_INFO_STRING];
	char			adrstr[MAX_ADDRESSLENGTH];
	servernode_t	templist[MAX_FAVORITESERVERS];
	qboolean		found;

	found        = qfalse;
	emptyinfo[0] = '\0';

	// copy the old
	memcpy( templist, g_favoriteserverlist, sizeof(servernode_t)*MAX_FAVORITESERVERS );
	numtempitems = g_numfavoriteservers;

	// clear the current for sync
	memset( g_favoriteserverlist, 0, sizeof(servernode_t)*MAX_FAVORITESERVERS );
	g_numfavoriteservers = 0;

	// resync existing results with new or deleted cvars
	for (i=0; i<MAX_FAVORITESERVERS; i++)
	{
		trap_Cvar_VariableStringBuffer( va("server%d",i+1), adrstr, MAX_ADDRESSLENGTH );
		if (!adrstr[0])
			continue;

		// quick sanity check to avoid slow domain name resolving
		// first character must be numeric
		if (adrstr[0] < '0' || adrstr[0] > '9')
			continue;

		// favorite server addresses must be maintained outside refresh list
		// this mimics local and global netadr's stored in client
		// these can be fetched to fill ping list
		strcpy( g_arenaservers.favoriteaddresses[g_numfavoriteservers], adrstr );

		// find this server in the old list
		for (j=0; j<numtempitems; j++)
			if (!Q_stricmp( templist[j].adrstr, adrstr ))
				break;

		if (j < numtempitems)
		{
			// found server - add exisiting results
			memcpy( &g_favoriteserverlist[g_numfavoriteservers], &templist[j], sizeof(servernode_t) );
			found = qtrue;
		}
		else
		{
			// add new server
			Q_strncpyz( g_favoriteserverlist[g_numfavoriteservers].adrstr, adrstr, MAX_ADDRESSLENGTH );
			g_favoriteserverlist[g_numfavoriteservers].pingtime = ArenaServers_MaxPing();
		}

		g_numfavoriteservers++;
	}

	g_arenaservers.numfavoriteaddresses = g_numfavoriteservers;

	if (!found)
	{
		// no results were found, reset server list
		// list will be automatically refreshed when selected
		g_numfavoriteservers = 0;
	}
}


/*
=================
ArenaServers_StopRefresh
=================
*/
static void ArenaServers_StopRefresh( void )
{
	if (!g_arenaservers.refreshservers)
		// not currently refreshing
		return;

	g_arenaservers.refreshservers = qfalse;

	if (g_servertype == AS_FAVORITES)
	{
		// nonresponsive favorites must be shown
		ArenaServers_InsertFavorites();
	}

	// final tally
	if (g_arenaservers.numqueriedservers >= 0)
	{
		g_arenaservers.currentping       = *g_arenaservers.numservers;
		g_arenaservers.numqueriedservers = *g_arenaservers.numservers; 
	}
	
	// sort
	qsort( g_arenaservers.serverlist, *g_arenaservers.numservers, sizeof( servernode_t ), ArenaServers_Compare);

	ArenaServers_UpdateMenu();
}


/*
=================
ArenaServers_DoRefresh
=================
*/
static void ArenaServers_DoRefresh( void )
{
	int		i;
	int		j;
	int		time;
	int		maxPing;
	char	adrstr[MAX_ADDRESSLENGTH];
	char	info[MAX_INFO_STRING];

	if (uis.realtime < g_arenaservers.refreshtime)
	{
		// wait for timeout or results
		if (g_servertype == AS_LOCAL)
		{
			if (!trap_LAN_GetLocalServerCount())
			{
				// still waiting for response
				return;
			}
		}
		else if(( g_servertype >= AS_GLOBAL1) && ( g_servertype <= AS_GLOBAL5)) 
		{
			if (trap_LAN_GetGlobalServerCount() < 0)
			{
				// still waiting for master server response
				return;
			}
		}
	}

	if (uis.realtime < g_arenaservers.nextpingtime)
	{
		// wait for time trigger
		return;
	}

	// trigger at 10Hz intervals
	g_arenaservers.nextpingtime = uis.realtime + 50;

	// process ping results
	maxPing = ArenaServers_MaxPing();
	for (i=0; i<MAX_PINGREQUESTS; i++)
	{
		trap_LAN_GetPing( i, adrstr, MAX_ADDRESSLENGTH, &time );
		if (!adrstr[0])
		{
			// ignore empty or pending pings
			continue;
		}

		// find ping result in our local list
		for (j=0; j<MAX_PINGREQUESTS; j++)
			if (!Q_stricmp( adrstr, g_arenaservers.pinglist[j].adrstr ))
				break;

		if (j < MAX_PINGREQUESTS)
		{
			// found it
			if (!time)
			{
				time = uis.realtime - g_arenaservers.pinglist[j].start;
				if (time < maxPing)
				{
					// still waiting
					continue;
				}
			}

			if (time > maxPing)
			{
				// stale it out
				info[0] = '\0';
				time    = maxPing;
			}
			else
			{
				trap_LAN_GetPingInfo( i, info, MAX_INFO_STRING ); //bookmark - TiM: This is where the info is received!
				
				//TiM - testing the data output from this baby
				//if ( !Q_stricmp( Info_ValueForKey( info, "game" ), "rpg-x" ) )
					//Com_Printf( "%s", info );
			}

			// insert ping results
			ArenaServers_Insert( adrstr, info, time );

			// clear this query from internal list
			g_arenaservers.pinglist[j].adrstr[0] = '\0';
   		}

		// clear this query from external list
		trap_LAN_ClearPing( i );
	}

	// get results of servers query
	// counts can increase as servers respond
	switch (g_servertype)
	{
		case AS_LOCAL:
			g_arenaservers.numqueriedservers = trap_LAN_GetLocalServerCount();
			break;

		case AS_GLOBAL1:
		case AS_GLOBAL2:
		case AS_GLOBAL3:
		case AS_GLOBAL4:
		case AS_GLOBAL5:
			g_arenaservers.numqueriedservers = trap_LAN_GetGlobalServerCount();
			break;

		case AS_FAVORITES:
			g_arenaservers.numqueriedservers = g_arenaservers.numfavoriteaddresses;
			break;
	}

//	if (g_arenaservers.numqueriedservers > g_arenaservers.maxservers)
//		g_arenaservers.numqueriedservers = g_arenaservers.maxservers;

	// send ping requests in reasonable bursts
	// iterate ping through all found servers
	for (i=0; i<MAX_PINGREQUESTS && g_arenaservers.currentping < g_arenaservers.numqueriedservers; i++)
	{
		if (trap_LAN_GetPingQueueCount() >= MAX_PINGREQUESTS)
		{
			// ping queue is full
			break;
		}

		// find empty slot
		for (j=0; j<MAX_PINGREQUESTS; j++)
			if (!g_arenaservers.pinglist[j].adrstr[0])
				break;

		if (j >= MAX_PINGREQUESTS)
			// no empty slots available yet - wait for timeout
			break;

		// get an address to ping
		switch (g_servertype)
		{
			case AS_LOCAL:	
				trap_LAN_GetLocalServerAddressString( g_arenaservers.currentping, adrstr, MAX_ADDRESSLENGTH );
				break;

			case AS_GLOBAL1:
			case AS_GLOBAL2:
			case AS_GLOBAL3:
			case AS_GLOBAL4:
			case AS_GLOBAL5:
				trap_LAN_GetGlobalServerAddressString( g_arenaservers.currentping, adrstr, MAX_ADDRESSLENGTH );
				break;

			case AS_FAVORITES:
				strcpy( adrstr, g_arenaservers.favoriteaddresses[g_arenaservers.currentping] ); 		
				break;

		}

		strcpy( g_arenaservers.pinglist[j].adrstr, adrstr );
		g_arenaservers.pinglist[j].start = uis.realtime;

		trap_Cmd_ExecuteText( EXEC_NOW, va( "ping %s\n", adrstr )  );
		
		// advance to next server
		g_arenaservers.currentping++;
	}

	if (!trap_LAN_GetPingQueueCount())
	{
		// all pings completed
		ArenaServers_StopRefresh();
		return;
	}

	// update the user interface with ping status
	ArenaServers_UpdateMenu();
}


/*
=================
ArenaServers_StartRefresh
=================
*/
static void ArenaServers_StartRefresh( void )
{
	int		i;
	char	myargs[32];

	memset( g_arenaservers.serverlist, 0, g_arenaservers.maxservers*sizeof(table_t) );

	for (i=0; i<MAX_PINGREQUESTS; i++)
	{
		g_arenaservers.pinglist[i].adrstr[0] = '\0';
		trap_LAN_ClearPing( i );
	}

	g_arenaservers.refreshservers    = qtrue;
	g_arenaservers.currentping       = 0;
	g_arenaservers.nextpingtime      = 0;
	*g_arenaservers.numservers       = 0;
	g_arenaservers.numqueriedservers = 0;

	// allow max 5 seconds for responses
	g_arenaservers.refreshtime = uis.realtime + 5000;

	// place menu in zeroed state
	ArenaServers_UpdateMenu();

	if( g_servertype == AS_LOCAL ) {
		trap_Cmd_ExecuteText( EXEC_APPEND, "localservers\n" );
		return;
	}

	if(( g_servertype >= AS_GLOBAL1) && ( g_servertype <= AS_GLOBAL5)) 
	{
		i = g_servertype - AS_GLOBAL1;

//		if( g_servertype == AS_GLOBAL1 ) {
//			i = 0;
//		}
//		else {
//			i = 1;
//		}

		switch( g_arenaservers.gametype.curvalue ) {
		default:
		/*case GAMES_EXCESSIVE:
		case GAMES_FEDERATION:
		case GAMES_GLADIATOR:
		case GAMES_IN2TAGIB:
		case GAMES_PINBALL:
		case GAMES_Q32EF:
		case GAMES_RPG:
		case GAMES_TEAMELITE:*/
		case GAMES_RPGX:
		case GAMES_ALL:
			myargs[0] = 0;
			break;

		case GAMES_FFA:
			strcpy( myargs, " ffa" );
			break;

		case GAMES_TEAMPLAY:
			strcpy( myargs, " team" );
			break;

		case GAMES_TOURNEY:
			strcpy( myargs, " tourney" );
			break;

		case GAMES_CTF:
			strcpy( myargs, " ctf" );
			break;
		}


		if (g_emptyservers) {
			strcat(myargs, " empty");
		}

		if (g_fullservers) {
			strcat(myargs, " full");
		}

		trap_Cmd_ExecuteText( EXEC_APPEND, va( "globalservers %d %d%s\n", i, (int)trap_Cvar_VariableValue( "protocol" ), myargs ) );
	}
}


/*
=================
ArenaServers_SaveChanges
=================
*/
void ArenaServers_SaveChanges( void )
{
	int	i;

	for (i=0; i<g_arenaservers.numfavoriteaddresses; i++)
		trap_Cvar_Set( va("server%d",i+1), g_arenaservers.favoriteaddresses[i] );

	for (; i<MAX_FAVORITESERVERS; i++)
		trap_Cvar_Set( va("server%d",i+1), "" );
}


/*
=================
ArenaServers_Sort
=================
*/
void ArenaServers_Sort( int type ) {
	if( g_sortkey == type ) {
		return;
	}

	g_sortkey = type;
	qsort( g_arenaservers.serverlist, *g_arenaservers.numservers, sizeof( servernode_t ), ArenaServers_Compare);
}


/*
=================
ArenaServers_SetType
=================
*/
void ArenaServers_SetType( int type )
{
	char IP[128];

	if (g_servertype == type)
		return;

	if (type == AS_GLOBAL1)
	{
		trap_Cvar_VariableStringBuffer( "sv_master1", IP, 128);
		if (!IP[0])
		{
			type= AS_GLOBAL2;
		}
		if (g_arenaservers.numservers)
		{
			*g_arenaservers.numservers       = 0;
		}
	}
	if (type == AS_GLOBAL2)
	{
		trap_Cvar_VariableStringBuffer( "sv_master2", IP, 128);
		if (!IP[0])
		{
			type= AS_GLOBAL3;
		}
		if (g_arenaservers.numservers)
		{
			*g_arenaservers.numservers       = 0;
		}
	}
	if (type == AS_GLOBAL3)
	{
		trap_Cvar_VariableStringBuffer( "sv_master3", IP, 128);
		if (!IP[0])
		{
			type= AS_GLOBAL4;
		}
		if (g_arenaservers.numservers)
		{
			*g_arenaservers.numservers       = 0;
		}
	}
	if (type == AS_GLOBAL4)
	{
		trap_Cvar_VariableStringBuffer( "sv_master4", IP, 128);
		if (!IP[0])
		{
			type= AS_GLOBAL5;
		}
		if (g_arenaservers.numservers)
		{
			*g_arenaservers.numservers       = 0;
		}
	}
	if (type == AS_GLOBAL5)
	{
		trap_Cvar_VariableStringBuffer( "sv_master5", IP, 128);
		if (!IP[0])
		{
			type= AS_FAVORITES;
		}
		if (g_arenaservers.numservers)
		{
			*g_arenaservers.numservers       = 0;
		}
	}

	g_arenaservers.master.curvalue = type;
	g_servertype = type;

	switch( type ) {
	default:
	case AS_LOCAL:
		g_arenaservers.remove.generic.flags |= (QMF_INACTIVE|QMF_HIDDEN);
		g_arenaservers.serverlist = g_localserverlist;
		g_arenaservers.numservers = &g_numlocalservers;
		g_arenaservers.maxservers = MAX_LOCALSERVERS;
		break;

	case AS_GLOBAL1:
	case AS_GLOBAL2:
	case AS_GLOBAL3:
	case AS_GLOBAL4:
	case AS_GLOBAL5:
		g_arenaservers.remove.generic.flags |= (QMF_INACTIVE|QMF_HIDDEN);
		g_arenaservers.serverlist = g_globalserverlist;
		g_arenaservers.numservers = &g_numglobalservers;
		g_arenaservers.maxservers = MAX_GLOBALSERVERS;
		break;

	case AS_FAVORITES:
		g_arenaservers.remove.generic.flags &= ~(QMF_INACTIVE|QMF_HIDDEN);
		g_arenaservers.serverlist = g_favoriteserverlist;
		g_arenaservers.numservers = &g_numfavoriteservers;
		g_arenaservers.maxservers = MAX_FAVORITESERVERS;
		break;

//	case AS_MPLAYER:
//		g_arenaservers.remove.generic.flags |= (QMF_INACTIVE|QMF_HIDDEN);
//		g_arenaservers.serverlist = g_mplayerserverlist;
//		g_arenaservers.numservers = &g_nummplayerservers;
//		g_arenaservers.maxservers = MAX_GLOBALSERVERS;
//		break;
	}

	if( !*g_arenaservers.numservers ) {
		ArenaServers_StartRefresh();
	}
	else {
		// avoid slow operation, use existing results
		g_arenaservers.currentping       = *g_arenaservers.numservers;
		g_arenaservers.numqueriedservers = *g_arenaservers.numservers; 
		ArenaServers_UpdateMenu();
	}
	strcpy(g_arenaservers.status.string,menu_normal_text[MNT_HITREFRESH]);
}


/*
=================
ArenaServers_Favorites_Add

Add current server to favorites
=================
*/
void ArenaServers_Favorites_Add( void )
{
	char	adrstr[128];
	int		i;
	int		best;

	if (!g_arenaservers.table[g_arenaservers.list.curvalue].servernode->adrstr)
		return;

	best = 0;
	for (i=0; i<MAX_FAVORITESERVERS; i++)
	{
		trap_Cvar_VariableStringBuffer( va("server%d",i+1), adrstr,	sizeof(adrstr) );

		if (!Q_stricmp(g_arenaservers.table[g_arenaservers.list.curvalue].servernode->adrstr,adrstr))
		{
			// already in list
			return;
		}
		
		// use first empty or non-numeric available slot
		if ((adrstr[0]  < '0' || adrstr[0] > '9' ) && !best)
			best = i+1;
	}

	if (best)
		trap_Cvar_Set( va("server%d",best), g_arenaservers.table[g_arenaservers.list.curvalue].servernode->adrstr);
}



/*
=================
ArenaServers_Event
=================
*/
static void ArenaServers_Event( void* ptr, int event ) {
	int		id;
	menuframework_s*	m;


	id = ((menucommon_s*)ptr)->id;

	if( event != QM_ACTIVATED && id != ID_LIST ) 
	{
		return;
	}

	m = ((menucommon_s*)ptr)->parent;

	switch( id ) 
	{
	case ID_MAINMENU:
		UI_PopMenu();
		UI_MainMenu();
		break;

	case ID_MASTER:

		trap_Cvar_SetValue( "ui_browserMaster", g_arenaservers.master.curvalue );

		ArenaServers_SetType( g_arenaservers.master.curvalue );

		if (master_items[g_arenaservers.master.curvalue] == MNT_MASTER_ITEMS_FAVORITES)
		{
			g_arenaservers.favorite.generic.flags			|= QMF_GRAYED;
		}
		else
		{
			g_arenaservers.favorite.generic.flags			&= ~QMF_GRAYED;
		}

		break;

	case ID_GAMETYPE:
		trap_Cvar_SetValue( "ui_browserGameType", g_arenaservers.gametype.curvalue );
		g_gametype = g_arenaservers.gametype.curvalue;
		ArenaServers_UpdateMenu();
		break;

	case ID_SORTKEY:
		trap_Cvar_SetValue( "ui_browserSortKey", g_arenaservers.sortkey.curvalue );
		ArenaServers_Sort( g_arenaservers.sortkey.curvalue );
		ArenaServers_UpdateMenu();
		break;

	case ID_SHOW_FULL:
		trap_Cvar_SetValue( "ui_browserShowFull", g_arenaservers.showfull.curvalue );
		g_fullservers = g_arenaservers.showfull.curvalue;
		ArenaServers_UpdateMenu();
		break;

	case ID_SHOW_EMPTY:
		trap_Cvar_SetValue( "ui_browserShowEmpty", g_arenaservers.showempty.curvalue );
		g_emptyservers = g_arenaservers.showempty.curvalue;
		ArenaServers_UpdateMenu();
		break;

	case ID_LIST:
		if( event == QM_GOTFOCUS ) {
			ArenaServers_UpdatePicture();
		}
		break;

	case ID_SCROLL_UP:
		ScrollList_Key( &g_arenaservers.list, K_UPARROW );
		break;

	case ID_SCROLL_DOWN:
		ScrollList_Key( &g_arenaservers.list, K_DOWNARROW );
		break;

	case ID_FAVORITE:
		ArenaServers_Favorites_Add();
		ArenaServers_LoadFavorites();

		break;

	case ID_BACK:
		ArenaServers_StopRefresh();
		ArenaServers_SaveChanges();
		UI_PopMenu();
		break;

	case ID_STOPREFRESH:
		ArenaServers_StopRefresh();
		break;

	case ID_REFRESH:
		ArenaServers_StartRefresh();
		break;

	case ID_SPECIFY:
			UI_SpecifyServerMenu();
		break;

	case ID_CREATE:
		UI_StartServerMenu( qtrue );
		break;

	case ID_FINDSERVER:
		UI_ArenaServersMenu();
		break;

	case ID_CONNECT:
		ArenaServers_Go();
		break;

	case ID_REMOVE:
		ArenaServers_Remove();
		ArenaServers_UpdateMenu();
		break;
	}
}



/*
=================
ArenaServers_Graphics
=================
*/
void ArenaServers_Graphics (void)
{
	UI_MenuFrame2(&g_arenaservers.menu);
	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(  30, 203,  47, 186, uis.whiteShader);	// Left side of frame

	// Frame map picture
	trap_R_SetColor( colorTable[CT_DKPURPLE3]);
	UI_DrawHandlePic(  87,  53, 158,  16, uis.whiteShader);	// Top line
	UI_DrawHandlePic( 232,  69,  13, 103, uis.whiteShader);	// Side line
	UI_DrawProportionalString(  90, 53, menu_normal_text[MNT_CURRENTMAP],UI_SMALLFONT, colorTable[CT_BLACK]);

	// Frame sort options
	trap_R_SetColor( colorTable[CT_DKPURPLE3]);
	UI_DrawHandlePic( 252,  53, 359,  16, uis.whiteShader);	// Top line
	UI_DrawHandlePic( 252,  69,  13, 103, uis.whiteShader);	// Side line
	UI_DrawProportionalString(  272, 53, menu_normal_text[MNT_SORTPARMS],UI_SMALLFONT, colorTable[CT_BLACK]);

	// Frame servers
	trap_R_SetColor( colorTable[CT_DKORANGE]);
	UI_DrawHandlePic(  87, 175, 523,  13, uis.whiteShader);	// Top line
	UI_DrawHandlePic(  87, 188,  16,  11, uis.whiteShader);	// Side line
	UI_DrawHandlePic(  87, 202,  16,  17, uis.whiteShader);	// Side line
	UI_DrawHandlePic(  87, 222,  16, 110, uis.whiteShader);	// Side line
	UI_DrawHandlePic(  87, 335,  16,  17, uis.whiteShader);	// Side line
	UI_DrawHandlePic(  87, 355,  16,  11, uis.whiteShader);	// Side line
	UI_DrawHandlePic(  87, 366, 268,  18, uis.whiteShader);	// Bottom line

	UI_DrawProportionalString(  105, 177, menu_normal_text[MNT_HOSTNAME],UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  360, 177, menu_normal_text[MNT_MAP],UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  466, 177, menu_normal_text[MNT_PLAYERS],UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  504, 177, menu_normal_text[MNT_TYPE],UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  583, 177, menu_normal_text[MNT_PING],UI_TINYFONT, colorTable[CT_BLACK]);

	UI_DrawProportionalString(  74,  27, "819",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74, 149, "42",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74, 205, "4363",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74, 395, "6700",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	// Left rounded ends for buttons
	trap_R_SetColor( colorTable[g_arenaservers.mainmenu.color]);
	UI_DrawHandlePic(g_arenaservers.mainmenu.generic.x - 14, g_arenaservers.mainmenu.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor( colorTable[g_arenaservers.back.color]);
	UI_DrawHandlePic(g_arenaservers.back.generic.x - 14, g_arenaservers.back.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

}

/*
=================
ArenaServers_MenuDraw
=================
*/
static void ArenaServers_MenuDraw( void )
{
	ArenaServers_Graphics();

	if (g_arenaservers.refreshservers)
	{
		ArenaServers_DoRefresh();
	}

	trap_R_SetColor( colorTable[CT_WHITE]);
	Menu_Draw( &g_arenaservers.menu );
}


/*
=================
ArenaServers_MenuKey
=================
*/
static sfxHandle_t ArenaServers_MenuKey( int key ) {
	if( key == K_SPACE  && g_arenaservers.refreshservers ) {
		ArenaServers_StopRefresh();	
		return menu_move_sound;
	}

	if( ( key == K_DEL || key == K_KP_DEL ) && ( g_servertype == AS_FAVORITES ) &&
		( Menu_ItemAtCursor( &g_arenaservers.menu) == &g_arenaservers.list ) ) {
		ArenaServers_Remove();
		ArenaServers_UpdateMenu();
		return menu_move_sound;
	}

	if( key == K_MOUSE2 || key == K_ESCAPE ) {
		ArenaServers_StopRefresh();
		ArenaServers_SaveChanges();
	}


	return Menu_DefaultKey( &g_arenaservers.menu, key );
}


/*
=================
ArenaServers_MenuInit
=================
*/
static void ArenaServers_MenuInit( void ) 
{
	int			i;
	int			type;
	static char	statusbuffer[MAX_STATUSLENGTH];

	// zero set all our globals
	memset( &g_arenaservers, 0 ,sizeof(arenaservers_t) );

	ArenaServers_Cache();

	g_arenaservers.menu.fullscreen					= qtrue;
	g_arenaservers.menu.wrapAround					= qtrue;
    g_arenaservers.menu.draw						= ArenaServers_MenuDraw;
	g_arenaservers.menu.key							= ArenaServers_MenuKey;
	g_arenaservers.menu.descX						= MENU_DESC_X;
	g_arenaservers.menu.descY						= MENU_DESC_Y;
	g_arenaservers.menu.titleX						= MENU_TITLE_X;
	g_arenaservers.menu.titleY						= MENU_TITLE_Y;
	g_arenaservers.menu.titleI						= MNT_MULTIPLAYER_TITLE;
	g_arenaservers.menu.footNoteEnum				= MNT_FINDSERVER;


	g_arenaservers.mainmenu.generic.type			= MTYPE_BITMAP;      
	g_arenaservers.mainmenu.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	g_arenaservers.mainmenu.generic.x				= 110;
	g_arenaservers.mainmenu.generic.y				= 391;
	g_arenaservers.mainmenu.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	g_arenaservers.mainmenu.generic.id				= ID_MAINMENU;
	g_arenaservers.mainmenu.generic.callback		= ArenaServers_Event;
	g_arenaservers.mainmenu.width					= MENU_BUTTON_MED_WIDTH;
	g_arenaservers.mainmenu.height					= MENU_BUTTON_MED_HEIGHT;
	g_arenaservers.mainmenu.color					= CT_DKPURPLE1;
	g_arenaservers.mainmenu.color2					= CT_LTPURPLE1;
	g_arenaservers.mainmenu.textX					= 5;
	g_arenaservers.mainmenu.textY					= 2;
	g_arenaservers.mainmenu.textEnum				= MBT_MAINMENU;
	g_arenaservers.mainmenu.textcolor				= CT_BLACK;
	g_arenaservers.mainmenu.textcolor2				= CT_WHITE;

	g_arenaservers.back.generic.type				= MTYPE_BITMAP;      
	g_arenaservers.back.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	g_arenaservers.back.generic.x					= 110;
	g_arenaservers.back.generic.y					= 415;
	g_arenaservers.back.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	g_arenaservers.back.generic.id					= ID_BACK;
	g_arenaservers.back.generic.callback			= ArenaServers_Event;
	g_arenaservers.back.width						= MENU_BUTTON_MED_WIDTH;
	g_arenaservers.back.height						= MENU_BUTTON_MED_HEIGHT;
	g_arenaservers.back.color						= CT_DKPURPLE1;
	g_arenaservers.back.color2						= CT_LTPURPLE1;
	g_arenaservers.back.textX						= 5;
	g_arenaservers.back.textY						= 2;
	g_arenaservers.back.textEnum					= MBT_BACK;
	g_arenaservers.back.textcolor					= CT_BLACK;
	g_arenaservers.back.textcolor2					= CT_WHITE;

	g_arenaservers.master.generic.type				= MTYPE_SPINCONTROL;
	g_arenaservers.master.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	g_arenaservers.master.generic.callback			= ArenaServers_Event;
	g_arenaservers.master.generic.id				= ID_MASTER;
	g_arenaservers.master.generic.x					= server_buttons[0][0];
	g_arenaservers.master.generic.y					= server_buttons[0][1];
	g_arenaservers.master.textEnum					= MBT_SERVERS;
	g_arenaservers.master.textcolor					= CT_BLACK;
	g_arenaservers.master.textcolor2				= CT_WHITE;
	g_arenaservers.master.color						= CT_DKPURPLE1;
	g_arenaservers.master.color2					= CT_LTPURPLE1;
	g_arenaservers.master.width						= 80;
	g_arenaservers.master.textX						= 5;
	g_arenaservers.master.textY						= 2;
	g_arenaservers.master.listnames					= master_items;

	g_arenaservers.gametype.generic.type			= MTYPE_SPINCONTROL;
	g_arenaservers.gametype.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	g_arenaservers.gametype.generic.callback		= ArenaServers_Event;
	g_arenaservers.gametype.generic.id				= ID_GAMETYPE;
	g_arenaservers.gametype.generic.x				= server_buttons[1][0];
	g_arenaservers.gametype.generic.y				= server_buttons[1][1];
	g_arenaservers.gametype.textEnum				= MBT_SERVERTYPE_GAMETYPE;
	g_arenaservers.gametype.textcolor				= CT_BLACK;
	g_arenaservers.gametype.textcolor2				= CT_WHITE;
	g_arenaservers.gametype.color					= CT_DKPURPLE1;
	g_arenaservers.gametype.color2					= CT_LTPURPLE1;
	g_arenaservers.gametype.width					= 80;
	g_arenaservers.gametype.textX					= 5;
	g_arenaservers.gametype.textY					= 2;
	g_arenaservers.gametype.listnames				= servertype_items;

	g_arenaservers.sortkey.generic.type				= MTYPE_SPINCONTROL;
	g_arenaservers.sortkey.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	g_arenaservers.sortkey.generic.callback			= ArenaServers_Event;
	g_arenaservers.sortkey.generic.id				= ID_SORTKEY;
	g_arenaservers.sortkey.generic.x				= server_buttons[2][0];
	g_arenaservers.sortkey.generic.y				= server_buttons[2][1];
	g_arenaservers.sortkey.textEnum					= MBT_SORTTYPE;
	g_arenaservers.sortkey.textcolor				= CT_BLACK;
	g_arenaservers.sortkey.textcolor2				= CT_WHITE;
	g_arenaservers.sortkey.color					= CT_DKPURPLE1;
	g_arenaservers.sortkey.color2					= CT_LTPURPLE1;
	g_arenaservers.sortkey.width					= 80;
	g_arenaservers.sortkey.textX					= 5;
	g_arenaservers.sortkey.textY					= 2;
	g_arenaservers.sortkey.listnames				= sortkey_items;


	g_arenaservers.showfull.generic.type			= MTYPE_SPINCONTROL;
	g_arenaservers.showfull.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	g_arenaservers.showfull.generic.callback		= ArenaServers_Event;
	g_arenaservers.showfull.generic.id				= ID_SHOW_FULL;
	g_arenaservers.showfull.generic.x				= server_buttons[3][0];
	g_arenaservers.showfull.generic.y				= server_buttons[3][1];
	g_arenaservers.showfull.textEnum				= MBT_SHOWFULL;
	g_arenaservers.showfull.textcolor				= CT_BLACK;
	g_arenaservers.showfull.textcolor2				= CT_WHITE;
	g_arenaservers.showfull.color					= CT_DKPURPLE1;
	g_arenaservers.showfull.color2					= CT_LTPURPLE1;
	g_arenaservers.showfull.width					= 80;
	g_arenaservers.showfull.textX					= 5;
	g_arenaservers.showfull.textY					= 2;
	g_arenaservers.showfull.listnames				= noyes_items;

	g_arenaservers.showempty.generic.type			= MTYPE_SPINCONTROL;
	g_arenaservers.showempty.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	g_arenaservers.showempty.generic.callback		= ArenaServers_Event;
	g_arenaservers.showempty.generic.id				= ID_SHOW_EMPTY;
	g_arenaservers.showempty.generic.x				= server_buttons[4][0];
	g_arenaservers.showempty.generic.y				= server_buttons[4][1];
	g_arenaservers.showempty.textEnum				= MBT_SHOWEMPTY;
	g_arenaservers.showempty.textcolor				= CT_BLACK;
	g_arenaservers.showempty.textcolor2				= CT_WHITE;
	g_arenaservers.showempty.color					= CT_DKPURPLE1;
	g_arenaservers.showempty.color2					= CT_LTPURPLE1;
	g_arenaservers.showempty.width					= 80;
	g_arenaservers.showempty.textX					= 5;
	g_arenaservers.showempty.textY					= 2;
	g_arenaservers.showempty.listnames				= noyes_items;

	g_arenaservers.list.generic.type			= MTYPE_SCROLLLIST;
	g_arenaservers.list.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	g_arenaservers.list.generic.id				= ID_LIST;
	g_arenaservers.list.generic.callback		= ArenaServers_Event;
	g_arenaservers.list.generic.x				= 103;
	g_arenaservers.list.generic.y				= 188;
	g_arenaservers.list.width					= MAX_LISTBOXWIDTH - 3;
	g_arenaservers.list.height					= 11;
	g_arenaservers.list.itemnames				= (const char **)g_arenaservers.items;
	for( i = 0; i < MAX_LISTBOXITEMS; i++ ) 
	{
		g_arenaservers.items[i] = g_arenaservers.table[i].buff;
	}
	
	g_arenaservers.mappic.generic.type			= MTYPE_BITMAP;
	g_arenaservers.mappic.generic.flags			= QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	g_arenaservers.mappic.generic.x				= 95;
	g_arenaservers.mappic.generic.y				= 71;
	g_arenaservers.mappic.width					= 128;
	g_arenaservers.mappic.height				= 96;
	g_arenaservers.mappic.errorpic				= ART_UNKNOWNMAP;

	g_arenaservers.up.generic.type				= MTYPE_BITMAP;
	g_arenaservers.up.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS|QMF_MOUSEONLY;
	g_arenaservers.up.generic.callback			= ArenaServers_Event;
	g_arenaservers.up.generic.id				= ID_SCROLL_UP;
	g_arenaservers.up.generic.x					= 87;
	g_arenaservers.up.generic.y					= 204;
	g_arenaservers.up.width						= 16;
	g_arenaservers.up.height					= 16;
	g_arenaservers.up.color  					= CT_DKGOLD1;
	g_arenaservers.up.color2  					= CT_LTGOLD1;
	g_arenaservers.up.generic.name				= "menu/common/arrow_up_16.tga";
	g_arenaservers.up.textEnum					= MBT_ARROW_UP;

	g_arenaservers.down.generic.type			= MTYPE_BITMAP;
	g_arenaservers.down.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS|QMF_MOUSEONLY;
	g_arenaservers.down.generic.callback		= ArenaServers_Event;
	g_arenaservers.down.generic.id				= ID_SCROLL_DOWN;
	g_arenaservers.down.generic.x				= 87;
	g_arenaservers.down.generic.y				= 337;
	g_arenaservers.down.width					= 16;
	g_arenaservers.down.height					= 16;
	g_arenaservers.down.color  					= CT_DKGOLD1;
	g_arenaservers.down.color2  				= CT_LTGOLD1;
	g_arenaservers.down.generic.name			= "menu/common/arrow_dn_16.tga";
	g_arenaservers.down.textEnum				= MBT_ARROW_DOWN;

	g_arenaservers.favorite.generic.type			= MTYPE_BITMAP;
	g_arenaservers.favorite.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS|QMF_MOUSEONLY;
	g_arenaservers.favorite.generic.callback		= ArenaServers_Event;
	g_arenaservers.favorite.generic.id				= ID_FAVORITE;
	g_arenaservers.favorite.generic.x				= 285;
	g_arenaservers.favorite.generic.y				= 415;
	g_arenaservers.favorite.width					= MENU_BUTTON_MED_WIDTH + 15;
	g_arenaservers.favorite.height					= MENU_BUTTON_MED_HEIGHT;
	g_arenaservers.favorite.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	g_arenaservers.favorite.textEnum				= MBT_ADDTOFAVS;
	g_arenaservers.favorite.color					= CT_DKPURPLE1;
	g_arenaservers.favorite.color2					= CT_LTPURPLE1;
	g_arenaservers.favorite.textX					= 5;
	g_arenaservers.favorite.textY					= 2;
	g_arenaservers.favorite.textcolor				= CT_BLACK;
	g_arenaservers.favorite.textcolor2				= CT_WHITE;


	g_arenaservers.status.generic.type		= MTYPE_TEXT;
	g_arenaservers.status.generic.flags		= QMF_INACTIVE;
	g_arenaservers.status.generic.x			= 129;
	g_arenaservers.status.generic.y			= 368;
	g_arenaservers.status.string			= statusbuffer;
	g_arenaservers.status.style				= UI_SMALLFONT;
	g_arenaservers.status.color				= menu_text_color;

	g_arenaservers.refresh.generic.type			= MTYPE_BITMAP;
	g_arenaservers.refresh.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	g_arenaservers.refresh.generic.callback		= ArenaServers_Event;
	g_arenaservers.refresh.generic.id			= ID_REFRESH;
	g_arenaservers.refresh.generic.x			= 358;
	g_arenaservers.refresh.generic.y			= 366;
	g_arenaservers.refresh.generic.name			= "menu/common/square.tga";
	g_arenaservers.refresh.generic.callback		= ArenaServers_Event;
	g_arenaservers.refresh.width				= 85;
	g_arenaservers.refresh.height				= MENU_BUTTON_MED_HEIGHT;
	g_arenaservers.refresh.color				= CT_DKPURPLE1;
	g_arenaservers.refresh.color2				= CT_LTPURPLE1;
	g_arenaservers.refresh.textX				= 5;
	g_arenaservers.refresh.textY				= 2;
	g_arenaservers.refresh.textEnum				= MBT_REFRESH;
	g_arenaservers.refresh.textcolor			= CT_BLACK;
	g_arenaservers.refresh.textcolor2			= CT_WHITE;

	g_arenaservers.stoprefresh.generic.type			= MTYPE_BITMAP;
	g_arenaservers.stoprefresh.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	g_arenaservers.stoprefresh.generic.callback		= ArenaServers_Event;
	g_arenaservers.stoprefresh.generic.id			= ID_STOPREFRESH;
	g_arenaservers.stoprefresh.generic.x			= 446;
	g_arenaservers.stoprefresh.generic.y			= 366;
	g_arenaservers.stoprefresh.generic.name			= "menu/common/square.tga";
	g_arenaservers.stoprefresh.width				= 85;
	g_arenaservers.stoprefresh.height				= MENU_BUTTON_MED_HEIGHT;
	g_arenaservers.stoprefresh.color				= CT_DKPURPLE1;
	g_arenaservers.stoprefresh.color2				= CT_LTPURPLE1;
	g_arenaservers.stoprefresh.textX				= 5;
	g_arenaservers.stoprefresh.textY				= 2;
	g_arenaservers.stoprefresh.textEnum				= MBT_STOPREFRESH;
	g_arenaservers.stoprefresh.textcolor			= CT_BLACK;
	g_arenaservers.stoprefresh.textcolor2			= CT_WHITE;

	g_arenaservers.remove.generic.type				= MTYPE_BITMAP;
	g_arenaservers.remove.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	g_arenaservers.remove.generic.callback			= ArenaServers_Event;
	g_arenaservers.remove.generic.id				= ID_REMOVE;
	g_arenaservers.remove.generic.x					= 534;
	g_arenaservers.remove.generic.y					= 366;
	g_arenaservers.remove.generic.name				= "menu/common/square.tga";
	g_arenaservers.remove.width						= 85;
	g_arenaservers.remove.height					= MENU_BUTTON_MED_HEIGHT;
	g_arenaservers.remove.color						= CT_DKPURPLE1;
	g_arenaservers.remove.color2					= CT_LTPURPLE1;
	g_arenaservers.remove.textX						= 5;
	g_arenaservers.remove.textY						= 2;
	g_arenaservers.remove.textEnum					= MBT_REMOVE;
	g_arenaservers.remove.textcolor					= CT_BLACK;
	g_arenaservers.remove.textcolor2				= CT_WHITE;

	g_arenaservers.go.generic.type			= MTYPE_BITMAP;
	g_arenaservers.go.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	g_arenaservers.go.generic.x				= 462;
	g_arenaservers.go.generic.y				= 391;
	g_arenaservers.go.generic.name			= "menu/common/square.tga";
	g_arenaservers.go.generic.id			= ID_CONNECT;
	g_arenaservers.go.generic.callback		= ArenaServers_Event;
	g_arenaservers.go.width					= 150;
	g_arenaservers.go.height				= 42;
	g_arenaservers.go.color					= CT_DKPURPLE1;
	g_arenaservers.go.color2				= CT_LTPURPLE1;
	g_arenaservers.go.textX					= 5;
	g_arenaservers.go.textY					= 2;
	g_arenaservers.go.textEnum				= MBT_ENGAGEMULTIPLAYER;
	g_arenaservers.go.textcolor				= CT_BLACK;
	g_arenaservers.go.textcolor2			= CT_WHITE;

	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.mainmenu );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.back );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.master );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.gametype );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.sortkey );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.showfull);
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.showempty );

	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.mappic );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.list );	
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.status );

//	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.statusbar );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.up );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.down );

	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.refresh );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.stoprefresh );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.remove );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.favorite );
	Menu_AddItem( &g_arenaservers.menu, (void*) &g_arenaservers.go );

	ArenaServers_LoadFavorites();

	g_servertype = Com_Clamp( 0, 3, ui_browserMaster.integer );
	g_arenaservers.master.curvalue = g_servertype;

	g_gametype = Com_Clamp( 0, 13, ui_browserGameType.integer );
	g_arenaservers.gametype.curvalue = g_gametype;


	g_sortkey = Com_Clamp( 0, 4, ui_browserSortKey.integer );
	g_arenaservers.sortkey.curvalue = g_sortkey;

	g_fullservers = Com_Clamp( 0, 1, ui_browserShowFull.integer );
	g_arenaservers.showfull.curvalue = g_fullservers;

	g_emptyservers = Com_Clamp( 0, 1, ui_browserShowEmpty.integer );
	g_arenaservers.showempty.curvalue = g_emptyservers;

	// force to initial state and refresh
	type = g_servertype;
	g_servertype = -1;
	ArenaServers_SetType( type );

	
	if (master_items[g_arenaservers.master.curvalue] == MNT_MASTER_ITEMS_FAVORITES)
	{
		g_arenaservers.favorite.generic.flags			|= QMF_GRAYED;
	}
	else
	{
		g_arenaservers.favorite.generic.flags			&= ~QMF_GRAYED;
	}

}


/*
=================
ArenaServers_Cache
=================
*/
void ArenaServers_Cache( void ) 
{
	trap_R_RegisterShaderNoMip( ART_UNKNOWNMAP );
}


/*
=================
UI_ArenaServersMenu
=================
*/
void UI_ArenaServersMenu( void ) 
{
	ArenaServers_MenuInit();
	UI_PushMenu( &g_arenaservers.menu );
}						  


/*
=================
ChooseServerType_MenuKey
=================
*/
static sfxHandle_t ChooseServerType_MenuKey( int key ) 
{
	return Menu_DefaultKey( &g_chooseservertype.menu, key );
}

/*
=================
ChooseServerType_Graphics
=================
*/
void ChooseServerType_Graphics (void)
{
	UI_MenuFrame(&g_chooseservertype.menu);

	trap_R_SetColor( colorTable[CT_LTPURPLE2]);
	UI_DrawHandlePic(  30, 203,  47, 186, uis.whiteShader);	// Frame Left side

	UI_DrawProportionalString(  74,  27, "66809",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "809",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74, 188, "991",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74, 206, "6-513",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74, 395, "7814",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	// Left rounded ends for buttons
	trap_R_SetColor( colorTable[s_find_server.color]);
	UI_DrawHandlePic(s_find_server.generic.x - 14, s_find_server.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor( colorTable[s_create_server.color]);
	UI_DrawHandlePic(s_create_server.generic.x - 14, s_create_server.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor( colorTable[s_specific_server.color]);
	UI_DrawHandlePic(s_specific_server.generic.x - 14, s_specific_server.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

}

/*
=================
ChooseServerType_MenuDraw
=================
*/
static void ChooseServerType_MenuDraw( void )
{
	ChooseServerType_Graphics();

	Menu_Draw( &g_chooseservertype.menu );
}

/*
=================
UI_ChooseServerTypeMenu_Cache
=================
*/
void UI_ChooseServerTypeMenu_Cache( void ) 
{
}
/*
=================
ChooseServerType_MenuInit
=================
*/
static void ChooseServerType_MenuInit( void ) 
{
	// zero set all our globals
	memset( &g_chooseservertype, 0 ,sizeof(chooseservertype_t) );

	g_chooseservertype.menu.fullscreen					= qtrue;
	g_chooseservertype.menu.wrapAround					= qtrue;
    g_chooseservertype.menu.draw						= ChooseServerType_MenuDraw;
	g_chooseservertype.menu.key							= ChooseServerType_MenuKey;
	g_chooseservertype.menu.descX						= MENU_DESC_X;
	g_chooseservertype.menu.descY						= MENU_DESC_Y;
	g_chooseservertype.menu.titleX						= MENU_TITLE_X;
	g_chooseservertype.menu.titleY						= MENU_TITLE_Y;
	g_chooseservertype.menu.titleI						= MNT_MULTIPLAYER_TITLE;
	g_chooseservertype.menu.footNoteEnum				= MNT_FINDSERVER;

	g_chooseservertype.main.generic.type			= MTYPE_BITMAP;
	g_chooseservertype.main.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	g_chooseservertype.main.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	g_chooseservertype.main.generic.id				= ID_BACK;
	g_chooseservertype.main.generic.callback		= ArenaServers_Event;
	g_chooseservertype.main.generic.x				= 482;
	g_chooseservertype.main.generic.y				= 136;
	g_chooseservertype.main.width  					= MENU_BUTTON_MED_WIDTH;
	g_chooseservertype.main.height  				= MENU_BUTTON_MED_HEIGHT;
	g_chooseservertype.main.color					= CT_DKPURPLE1;
	g_chooseservertype.main.color2					= CT_LTPURPLE1;
	g_chooseservertype.main.textX					= MENU_BUTTON_TEXT_X;
	g_chooseservertype.main.textY					= MENU_BUTTON_TEXT_Y;
	g_chooseservertype.main.textEnum				= MBT_MAINMENU;
	g_chooseservertype.main.textcolor				= CT_BLACK;
	g_chooseservertype.main.textcolor2				= CT_WHITE;

	s_find_server.generic.type					= MTYPE_BITMAP;      
	s_find_server.generic.flags					= QMF_HIGHLIGHT_IF_FOCUS;
	s_find_server.generic.x						= 282;
	s_find_server.generic.y						= 229;
	s_find_server.generic.name					= BUTTON_GRAPHIC_LONGRIGHT;
	s_find_server.generic.id					= ID_FINDSERVER;
	s_find_server.generic.callback				= ArenaServers_Event;
	s_find_server.width							= MENU_BUTTON_MED_WIDTH+16;
	s_find_server.height						= MENU_BUTTON_MED_HEIGHT;
	s_find_server.color							= CT_DKPURPLE1;
	s_find_server.color2						= CT_LTPURPLE1;
	s_find_server.textX							= 5;
	s_find_server.textY							= 2;
	s_find_server.textEnum						= MBT_SEARCHSERVER;
	s_find_server.textcolor						= CT_BLACK;
	s_find_server.textcolor2					= CT_WHITE;

	s_create_server.generic.type				= MTYPE_BITMAP;      
	s_create_server.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_create_server.generic.x					= 282;
	s_create_server.generic.y					= 279;
	s_create_server.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_create_server.generic.id					= ID_CREATE;
	s_create_server.generic.callback			= ArenaServers_Event;
	s_create_server.width						= MENU_BUTTON_MED_WIDTH+16;
	s_create_server.height						= MENU_BUTTON_MED_HEIGHT;
	s_create_server.color						= CT_DKPURPLE1;
	s_create_server.color2						= CT_LTPURPLE1;
	s_create_server.textX						= 5;
	s_create_server.textY						= 2;
	s_create_server.textEnum					= MBT_CREATE;
	s_create_server.textcolor					= CT_BLACK;
	s_create_server.textcolor2					= CT_WHITE;

	s_specific_server.generic.type				= MTYPE_BITMAP;      
	s_specific_server.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_specific_server.generic.x					= 282;
	s_specific_server.generic.y					= 329;
	s_specific_server.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_specific_server.generic.id				= ID_SPECIFY;
	s_specific_server.generic.callback			= ArenaServers_Event;
	s_specific_server.width						= MENU_BUTTON_MED_WIDTH+16;
	s_specific_server.height					= MENU_BUTTON_MED_HEIGHT;
	s_specific_server.color						= CT_DKPURPLE1;
	s_specific_server.color2					= CT_LTPURPLE1;
	s_specific_server.textX						= 5;
	s_specific_server.textY						= 2;
	s_specific_server.textEnum					= MBT_SPECIFY;
	s_specific_server.textcolor					= CT_BLACK;
	s_specific_server.textcolor2				= CT_WHITE;

	Menu_AddItem(&g_chooseservertype.menu, ( void * )&g_chooseservertype.main);
	Menu_AddItem(&g_chooseservertype.menu, ( void * )&s_find_server);
	Menu_AddItem(&g_chooseservertype.menu, ( void * )&s_create_server);
	Menu_AddItem(&g_chooseservertype.menu, ( void * )&s_specific_server);

	UI_ChooseServerTypeMenu_Cache();
}

/*
=================
UI_ChooseServerTypeMenu
=================
*/
void UI_ChooseServerTypeMenu( void ) 
{
	ChooseServerType_MenuInit();
	UI_PushMenu( &g_chooseservertype.menu );

}						  

