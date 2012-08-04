// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=============================================================================

SINGLE PLAYER POSTGAME MENU

=============================================================================
*/

#include "ui_local.h"

#define MAX_SCOREBOARD_CLIENTS		8

#define AWARD_PRESENTATION_TIME		2000


#define ID_AGAIN		10
#define ID_NEXT			11
#define ID_MENU			12

typedef struct {
	menuframework_s	menu;
	menubitmap_s	item_again;
	menubitmap_s	item_next;
	menubitmap_s	item_menu;

	int				phase;
	int				ignoreKeysTime;
	int				starttime;
	int				scoreboardtime;
	int				serverId;

	int				clientNums[MAX_SCOREBOARD_CLIENTS];
	int				ranks[MAX_SCOREBOARD_CLIENTS];
	int				scores[MAX_SCOREBOARD_CLIENTS];

	char			placeNames[3][64];

	int				level;
	int				numClients;
	int				won;
	int				numAwards;
	int				awardsEarned[6];
	int				awardsLevels[6];
	qboolean		playedSound[6];
	int				lastTier;
	sfxHandle_t		winnerSound;
} postgameMenuInfo_t;

static postgameMenuInfo_t	postgameMenuInfo;
static char					arenainfo[MAX_INFO_VALUE];

char	*ui_medalNames[] = 
{
	"Efficiency",		// AWARD_EFFICIENCY,		Accuracy	
	"Sharpshooter",		// AWARD_SHARPSHOOTER,		Most compression rifle frags
	"Untouchable",		// AWARD_UNTOUCHABLE,		Perfect (no deaths)
	"Logistics",		// AWARD_LOGISTICS,			Most pickups
	"Tactician",		// AWARD_TACTICIAN,			Kills with all weapons
	"Demolitionist",	// AWARD_DEMOLITIONIST,		Most explosive damage kills
	"Streak",			// AWARD_STREAK,			Ace/Expert/Master/Champion
	"Role",				// AWARD_TEAM,				MVP/Defender/Warrior/Carrier/Interceptor/Bravery
	"Section 31"		// AWARD_SECTION31			All-around god
};

char	*ui_medalPicNames[] = {
	"menu/medals/medal_efficiency",		// AWARD_EFFICIENCY,		
	"menu/medals/medal_sharpshooter",	// AWARD_SHARPSHOOTER,		
	"menu/medals/medal_untouchable",	// AWARD_UNTOUCHABLE,		
	"menu/medals/medal_logistics",		// AWARD_LOGISTICS,			
	"menu/medals/medal_tactician",		// AWARD_TACTICIAN,			
	"menu/medals/medal_demolitionist",	// AWARD_DEMOLITIONIST,		
	"menu/medals/medal_ace",			// AWARD_STREAK,			
	"menu/medals/medal_teammvp",		// AWARD_TEAM,				
	"menu/medals/medal_section31"		// AWARD_SECTION31			
};

char	*ui_medalSounds[] = {
	"sound/voice/computer/misc/effic.wav",		// AWARD_EFFICIENCY,		
	"sound/voice/computer/misc/sharp.wav",		// AWARD_SHARPSHOOTER,		
	"sound/voice/computer/misc/untouch.wav",	// AWARD_UNTOUCHABLE,		
	"sound/voice/computer/misc/log.wav",		// AWARD_LOGISTICS,			
	"sound/voice/computer/misc/tact.wav",		// AWARD_TACTICIAN,			
	"sound/voice/computer/misc/demo.wav",		// AWARD_DEMOLITIONIST,		
	"sound/voice/computer/misc/ace.wav",		// AWARD_STREAK,			
	"sound/voice/computer/misc/mvp.wav",		// AWARD_TEAM,				
	"sound/voice/computer/misc/sec31.wav",		// AWARD_SECTION31			
};


/*
=================
UI_SPPostgameMenu_AgainEvent
=================
*/
static void UI_SPPostgameMenu_AgainEvent( void* ptr, int event )
{
	if (event != QM_ACTIVATED) {
		return;
	}
	UI_PopMenu();
	trap_Cmd_ExecuteText( EXEC_APPEND, "map_restart 0\n" );
}


/*
=================
UI_SPPostgameMenu_NextEvent
=================
*/
static void UI_SPPostgameMenu_NextEvent( void* ptr, int event ) {
//	int			currentSet;
	int			levelSet;
	int			level;
//	int			currentLevel;
	const char	*arenaInfo;
	int			gametype;

	gametype = trap_Cvar_VariableValue("g_gametype" );
	//------------------------------------------- GAME TYPES
	//0	GT_FFA,				// free for all
	//1	GT_TOURNAMENT,		// one on one tournament
	//2	GT_SINGLE_PLAYER,	// single player tournament
	//3	GT_TEAM,			// team deathmatch
	//4	GT_CTF,				// capture the flag


	if (event != QM_ACTIVATED) {
		return;
	}
	UI_PopMenu();

//	if (gametype!=2)  // FOR ALL NON 'GT_SINGLE_PLAYER' GAMES
//	{	return;
//	}


	// handle specially if we just won the training map
	if( postgameMenuInfo.won == 0 ) {
		level = 0;
	}
	else {
		level = postgameMenuInfo.level + 1;
	}
	if (uis.demoversion) {
		levelSet = level / 2;
	} else {
		levelSet = level / ARENAS_PER_TIER;
	}
#if 0
	currentLevel = UI_GetCurrentGame();
	if( currentLevel == -1 ) {
		currentLevel = postgameMenuInfo.level;
	}
	currentSet = currentLevel / ARENAS_PER_TIER;

	if( levelSet > currentSet || levelSet == UI_GetNumSPTiers() ) {
		level = currentLevel;
	}
#else
	level = UI_GetCurrentGame(postgameMenuInfo.level);
	if (level == UI_GetNumSPArenas())
	{
		level = 0;
	}
#endif
	arenaInfo = UI_GetArenaInfoByNumber( level );
	if ( !arenaInfo ) {
		return;
	}

	UI_SPArena_Start( arenaInfo );
}


/*
=================
UI_SPPostgameMenu_MenuEvent
=================
*/
static void UI_SPPostgameMenu_MenuEvent( void* ptr, int event )
{
	if (event != QM_ACTIVATED) {
		return;
	}
	UI_PopMenu();
	trap_Cmd_ExecuteText( EXEC_APPEND, "disconnect; levelselect\n" );
}


/*
=================
UI_SPPostgameMenu_MenuKey
=================
*/
static sfxHandle_t UI_SPPostgameMenu_MenuKey( int key ) {
	if ( uis.realtime < postgameMenuInfo.ignoreKeysTime ) {
		return 0;
	}

	postgameMenuInfo.starttime = uis.realtime;
	postgameMenuInfo.ignoreKeysTime	= uis.realtime + 250;

	// NO ESCAPE FOR YOU!!!
	if( key == K_ESCAPE || key == K_MOUSE2 ) {
		return 0;
	}

	return Menu_DefaultKey( &postgameMenuInfo.menu, key );
}


// ____________________________________________________________________ FIX ME!!!!
/*
static int medalLocations[6] = {144, 448, 88, 504, 32, 560};

static void UI_SPPostgameMenu_DrawAwardsMedals( int max ) {
}


static void UI_SPPostgameMenu_DrawAwardsPresentation( int timer ) {
}

static void UI_SPPostgameMenu_MenuDrawScoreLine( int n, int y ) {
}
*/

/*
=================
UI_SPPostgameMenu_MenuDraw
=================
*/
/*
static void UI_SPPostgameMenu_MenuDraw( void ) {
	int		serverId;
	char	info[MAX_INFO_STRING];

	trap_GetConfigString( CS_SYSTEMINFO, info, sizeof(info) );
	serverId = atoi( Info_ValueForKey( info, "sv_serverid" ) );
	if( serverId != postgameMenuInfo.serverId ) {
		UI_PopMenu();
		return;
	}

	postgameMenuInfo.item_again.generic.flags &= ~QMF_INACTIVE;
	postgameMenuInfo.item_next.generic.flags &= ~QMF_INACTIVE;
	postgameMenuInfo.item_menu.generic.flags &= ~QMF_INACTIVE;

	Menu_Draw( &postgameMenuInfo.menu );
}
*/

/*
=================
UI_SPPostgameMenu_Cache
=================
*/
void UI_SPPostgameMenu_Cache( void ) {
	qboolean	buildscript;
	buildscript = trap_Cvar_VariableValue("com_buildscript");
	if( buildscript ) {	//cache these for the pack file!
		trap_Cmd_ExecuteText( EXEC_APPEND, "music music/win\n" );
		trap_Cmd_ExecuteText( EXEC_APPEND, "music music/loss\n" );
	}
}


/*
=================
UI_SPPostgameMenu_Init
=================
Sets up the exact look of the menu buttons at the bottom
*/
static void UI_SPPostgameMenu_Init( void ) {

	postgameMenuInfo.menu.wrapAround	= qtrue;
	postgameMenuInfo.menu.key			= UI_SPPostgameMenu_MenuKey;
//	postgameMenuInfo.menu.draw			= UI_SPPostgameMenu_MenuDraw;
	postgameMenuInfo.ignoreKeysTime		= uis.realtime + 1500;

	UI_SPPostgameMenu_Cache();

	postgameMenuInfo.item_menu.generic.type			= MTYPE_BITMAP;
	postgameMenuInfo.item_menu.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	postgameMenuInfo.item_menu.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	postgameMenuInfo.item_menu.generic.x			= 33;
	postgameMenuInfo.item_menu.generic.y			= 446;
	postgameMenuInfo.item_menu.generic.callback		= UI_SPPostgameMenu_MenuEvent;
	postgameMenuInfo.item_menu.generic.id			= ID_MENU;
	postgameMenuInfo.item_menu.width				= MENU_BUTTON_MED_WIDTH;
	postgameMenuInfo.item_menu.height				= MENU_BUTTON_MED_HEIGHT;
	postgameMenuInfo.item_menu.color				= CT_DKPURPLE1;
	postgameMenuInfo.item_menu.color2				= CT_LTPURPLE1;
	postgameMenuInfo.item_menu.textX				= 5;
	postgameMenuInfo.item_menu.textY				= 1;
	postgameMenuInfo.item_menu.textEnum				= MBT_RETURNMENU;
	postgameMenuInfo.item_menu.textcolor			= CT_BLACK;
	postgameMenuInfo.item_menu.textcolor2			= CT_WHITE;

	postgameMenuInfo.item_again.generic.type		= MTYPE_BITMAP;
	postgameMenuInfo.item_again.generic.name		= BUTTON_GRAPHIC_LONGRIGHT;
	postgameMenuInfo.item_again.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	postgameMenuInfo.item_again.generic.x			= 256;
	postgameMenuInfo.item_again.generic.y			= 446;
	postgameMenuInfo.item_again.generic.callback	= UI_SPPostgameMenu_AgainEvent;
	postgameMenuInfo.item_again.generic.id			= ID_AGAIN;
	postgameMenuInfo.item_again.width				= MENU_BUTTON_MED_WIDTH;
	postgameMenuInfo.item_again.height				= MENU_BUTTON_MED_HEIGHT;
	postgameMenuInfo.item_again.color				= CT_DKPURPLE1;
	postgameMenuInfo.item_again.color2				= CT_LTPURPLE1;
	postgameMenuInfo.item_again.textX				= 5;
	postgameMenuInfo.item_again.textY				= 1;
	postgameMenuInfo.item_again.textEnum			= MBT_REPLAY;
	postgameMenuInfo.item_again.textcolor			= CT_BLACK;
	postgameMenuInfo.item_again.textcolor2			= CT_WHITE;

	postgameMenuInfo.item_next.generic.type			= MTYPE_BITMAP;
	postgameMenuInfo.item_next.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	postgameMenuInfo.item_next.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	postgameMenuInfo.item_next.generic.x			= 477;
	postgameMenuInfo.item_next.generic.y			= 446;
	postgameMenuInfo.item_next.generic.callback		= UI_SPPostgameMenu_NextEvent;
	postgameMenuInfo.item_next.generic.id			= ID_NEXT;
	postgameMenuInfo.item_next.width				= MENU_BUTTON_MED_WIDTH;
	postgameMenuInfo.item_next.height				= MENU_BUTTON_MED_HEIGHT;
	postgameMenuInfo.item_next.color				= CT_DKPURPLE1;
	postgameMenuInfo.item_next.color2				= CT_LTPURPLE1;
	postgameMenuInfo.item_next.textX				= 5;
	postgameMenuInfo.item_next.textY				= 1;
	postgameMenuInfo.item_next.textEnum				= MBT_NEXTMATCH;
	postgameMenuInfo.item_next.textcolor			= CT_BLACK;
	postgameMenuInfo.item_next.textcolor2			= CT_WHITE;


	Menu_AddItem( &postgameMenuInfo.menu, ( void * )&postgameMenuInfo.item_menu );
	Menu_AddItem( &postgameMenuInfo.menu, ( void * )&postgameMenuInfo.item_next );
	Menu_AddItem( &postgameMenuInfo.menu, ( void * )&postgameMenuInfo.item_again );
}

/*
=================
UI_SPPostgameMenu_f
=================
*/
void UI_SPPostgameMenu_f( void ) {
	int			playerGameRank;
//	int			oldFrags, newFrags;
	const char	*arena;
	char		map[MAX_QPATH];
	char		info[MAX_INFO_STRING];

	Mouse_Show();

	memset( &postgameMenuInfo, 0, sizeof(postgameMenuInfo) );

	trap_GetConfigString( CS_SYSTEMINFO, info, sizeof(info) );
	postgameMenuInfo.serverId = atoi( Info_ValueForKey( info, "sv_serverid" ) );

	postgameMenuInfo.menu.nobackground = qtrue;

	trap_GetConfigString( CS_SERVERINFO, info, sizeof(info) );
	Q_strncpyz( map, Info_ValueForKey( info, "mapname" ), sizeof(map) );
	arena = UI_GetArenaInfoByMap( map );
	if ( !arena ) {
		return;
	}
	Q_strncpyz( arenainfo, arena, sizeof(arenainfo) );

	postgameMenuInfo.level = atoi( Info_ValueForKey( arenainfo, "num" ) );

	playerGameRank = 8;		// in case they ended game as a spectator
	playerGameRank = atoi( UI_Argv(1));

	UI_SetBestScore( postgameMenuInfo.level, playerGameRank+1 );

	postgameMenuInfo.starttime = uis.realtime;
	postgameMenuInfo.scoreboardtime = uis.realtime;

	trap_Key_SetCatcher( KEYCATCH_UI );
	uis.menusp = 0;

	UI_SPPostgameMenu_Init();
	UI_PushMenu( &postgameMenuInfo.menu );

	if (playerGameRank == 0) 
	{
		postgameMenuInfo.won = 1;
		Menu_SetCursorToItem( &postgameMenuInfo.menu, &postgameMenuInfo.item_next );
	}
	else {
		Menu_SetCursorToItem( &postgameMenuInfo.menu, &postgameMenuInfo.item_menu );
	}

//	trap_Cmd_ExecuteText( EXEC_APPEND, "music music/win\n" );	//?? always win?  should this be deleted and playing cg_scoreboard now?

	postgameMenuInfo.lastTier = UI_GetNumSPTiers();
	if ( UI_GetSpecialArenaInfo( "final" ) ) {
		postgameMenuInfo.lastTier++;
	}
}
