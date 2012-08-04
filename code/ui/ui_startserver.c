// Copyright (C) 1999-2000 Raven Software
//
/*
=============================================================================

START SERVER MENU *****

=============================================================================
*/
#include "ui_local.h"

#define GAMESERVER_SELECT		"menu/art/maps_select"
#define GAMESERVER_SELECTED		"menu/art/maps_selected"
#define GAMESERVER_UNKNOWNMAP	"levelshots/unknownmap"

#define MAX_SKINS_FOR_RACE 128

#define MAX_MAPROWS		2
#define MAX_MAPCOLS		2
#define MAX_MAPSPERPAGE	6

#define	MAX_SERVERSTEXT	8192

#define MAX_SERVERMAPS	1024
//#define MAX_NAMELENGTH	24

#define ID_GAMETYPE				10
#define ID_PICTURES				11	// 12, 13, 14
#define ID_PREVPAGE				15
#define ID_NEXTPAGE				16
#define ID_STARTSERVERBACK		17
#define ID_STARTSERVERNEXT		18
#define ID_MAINMENU				100

#define ID_AUTOJOIN					5
#define ID_AUTOBALANCE				6
#define ID_FRIENDLYFIRE				7
#define ID_FALLINGDAMAGE			8
#define ID_RESPAWNTIME				9
#define ID_ADVMAXCLIENTS			10
#define ID_RUNSPEED					11
#define ID_GRAVITY					12
#define ID_KNOCKBACK				13
#define ID_DMGMULT					14
#define ID_BOT_MINPLAYERS			15
#define ID_ADAPTITEMRESPAWN			16
#define ID_HOLODECKINTRO			17
#define ID_FORCEPLAYERRESPAWN		18
#define ID_RESPAWNINVULNERABILITY	19
#define ID_DOWARMUP					20
#define ID_BLUETEAM					21
#define ID_REDTEAM					22
#define ID_PMOD_ASSIMILIATION		23
#define ID_PMOD_DISINTEGRATION		24
#define ID_PMOD_ACTIONHERO			25
#define ID_PMOD_SPECIALTIES			26
#define ID_PMOD_ELIMINATION			27
#define ID_PLAYER_TYPE			28
#define ID_MAXCLIENTS			29
#define ID_DEDICATED			30
#define ID_GO					31
#define ID_BACK					32
#define ID_PLAYER_TEAM			33
#define ID_ADVANCED				34
#define	ID_NOJOINTIMEOUT		35
#define	ID_CLASSCHANGETIMEOUT	36
#define ID_PLAYER_CLASS			37

#define ERR_RESPAWNTIME				1
#define ERR_MAXCLIENTS				2
#define ERR_RUNSPEED				3
#define ERR_GRAVITY					4
#define ERR_KNOCKBACK				5
#define ERR_DMGMULT					6
#define ERR_BOT_MINPLAYERS			7
#define ERR_FORCEPLAYERRESPAWN		8
#define ERR_RESPAWNINVULNERABILITY	9
#define ERR_DOWARMUP				10
#define	ERR_NOJOINTIMEOUT			11
#define	ERR_CLASSCHANGETIMEOUT		12

void UI_BuildGroupTable(void);
void SetPlayerMod(void);

extern int s_OffOnNone_Names[];

static const char  *s_skinsForRace[MAX_SKINS_FOR_RACE];

static char skinsForRace[MAX_SKINS_FOR_RACE][128];

typedef struct 
{
	menuframework_s	menu;
	menubitmap_s	mainmenu;
	menubitmap_s	back;

	menulist_s		autojoin;
	menulist_s		autobalance;
	menulist_s		friendlyfire;
	menulist_s		fallingdamage;

	menufield_s		repawntime;
	menufield_s		maxclients;
	menufield_s		runspeed;
	menufield_s		gravity;
	menufield_s		knockback;
	menufield_s		dmgmult;
	menufield_s		bot_minplayers;
	menufield_s		classchangetimeout;

	// Second column
	menulist_s		adaptitemrespawn;
	menulist_s		holodeckintro;
	menufield_s		forceplayerrespawn;
	menufield_s		respawninvulnerability;
	menufield_s		dowarmup;

	menulist_s		blueteam;
	menulist_s		redteam;


	menulist_s		assimilation;
	menulist_s		specialties;
	menulist_s		disintegration;

	menulist_s		actionhero;
	menulist_s		elimination;

	menutext_s		errorText;

	int				errorFlag;

} advancedserver_t;

static advancedserver_t s_advancedserver;

typedef struct 
{
	menuframework_s	menu;

	menubitmap_s	mainmenu;
	menulist_s		gametype;
	menubitmap_s	mappics[MAX_MAPSPERPAGE];
	menubitmap_s	mapbuttons[MAX_MAPSPERPAGE];
	menubitmap_s	arrows;
	menubitmap_s	prevpage;
	menubitmap_s	nextpage;
	menubitmap_s	back;
	menubitmap_s	next;

	menulist_s		assimilation;
	menulist_s		specialties;
	menulist_s		disintegration;

	menulist_s		actionhero;
	menulist_s		elimination;

	menutext_s		mapname;
	menubitmap_s	item_null;

	qhandle_t		corner_ll;
	qhandle_t		corner_lr;

	qboolean		multiplayer;
	int				currentmap;
	int				nummaps;
	int				page;
	int				maxpages;
	char			maplist[MAX_SERVERMAPS][MAX_NAMELENGTH];
	int				mapGamebits[MAX_SERVERMAPS];
	char			maplongname[MAX_SERVERMAPS][MAX_NAMELENGTH];
	int				maprecommended[MAX_SERVERMAPS];
} startserver_t;

static startserver_t s_startserver;
/*
static const char *gametype_items[] = {
	"Free For All",
	"Team Deathmatch",
	"Tournament",
	"Capture the Flag",
	0
};
*/

static int gametype_items[] =
{
	MNT_TYPE_FREEFORALL,
	MNT_TYPE_TEAMDEATHMATCH,
	MNT_TYPE_TOURNAMENT,
	MNT_TYPE_CAPTURETHEFLAG,
	0
};

static int gametype_remap[] = {GT_FFA, GT_TEAM, GT_TOURNAMENT, GT_CTF};
//static int gametype_remap2[] = {0, 2, 0, 1, 3};

static void UI_ServerOptionsMenu( qboolean multiplayer );
static int UI_SearchGroupTable(char *current_race);
static void UI_BlankGroupTable(void);

void StartServerSetModButtons(void);

/*
=================
GametypeBits
=================
*/
static int GametypeBits( char *string ) {
	int		bits;
	char	*p;
	char	*token;

	bits = 0;
	p = string;
	while( 1 ) {
		token = COM_ParseExt( &p, qfalse );
		if( token[0] == 0 )
		{
			break;
		}

		if( Q_stricmp( token, "ffa" ) == 0 )
		{
			bits |= 1 << GT_FFA;
			bits |= 1 << GT_TOURNAMENT;
			continue;
		}

		if( Q_stricmp( token, "tourney" ) == 0 )
		{
			bits |= 1 << GT_TOURNAMENT;
			continue;
		}

		if( Q_stricmp( token, "single" ) == 0 )
		{
			bits |= 1 << GT_SINGLE_PLAYER;
			bits |= 1 << GT_TOURNAMENT;
			continue;
		}

		if( Q_stricmp( token, "team" ) == 0 )
		{
			bits |= 1 << GT_TEAM;
			continue;
		}

		if( Q_stricmp( token, "ctf" ) == 0 )
		{
			bits |= 1 << GT_CTF;
			continue;
		}
	}

	return bits;
}


/*
=================
StartServer_Update
=================
*/
static void StartServer_Update( void ) {
	int				i;
	int				top;
	static	char	picname[MAX_MAPSPERPAGE][64];

	top = s_startserver.page*MAX_MAPSPERPAGE;

	for (i=0; i<MAX_MAPSPERPAGE; i++)
	{
		if (top+i >= s_startserver.nummaps)
			break;

		Com_sprintf( picname[i], sizeof(picname[i]), "levelshots/%s", s_startserver.maplist[top+i] );

		s_startserver.mappics[i].generic.flags &= ~QMF_HIGHLIGHT;
		s_startserver.mappics[i].generic.name   = picname[i];
		s_startserver.mappics[i].shader         = 0;

		// reset
		s_startserver.mapbuttons[i].generic.flags |= QMF_PULSEIFFOCUS;
		s_startserver.mapbuttons[i].generic.flags &= ~QMF_INACTIVE;
	}

	for (; i<MAX_MAPSPERPAGE; i++)
	{
		s_startserver.mappics[i].generic.flags &= ~QMF_HIGHLIGHT;
		s_startserver.mappics[i].generic.name   = NULL;
		s_startserver.mappics[i].shader         = 0;

		// disable
		s_startserver.mapbuttons[i].generic.flags &= ~QMF_PULSEIFFOCUS;
		s_startserver.mapbuttons[i].generic.flags |= QMF_INACTIVE;
	}

	// no servers to start
	if( !s_startserver.nummaps ) {
		s_startserver.next.generic.flags |= QMF_INACTIVE;

		// set the map name
		strcpy( s_startserver.mapname.string, menu_normal_text[MNT_NOMAPSFOUND] );
	}
	else {
		// set the highlight
		s_startserver.next.generic.flags &= ~QMF_INACTIVE;
		i = s_startserver.currentmap - top;
		if ( i >=0 && i < MAX_MAPSPERPAGE ) 
		{
			s_startserver.mappics[i].generic.flags    |= QMF_HIGHLIGHT;
			s_startserver.mapbuttons[i].generic.flags &= ~QMF_PULSEIFFOCUS;
		}

		// set the map name
		strcpy( s_startserver.mapname.string, s_startserver.maplist[s_startserver.currentmap] );
	}

	Q_strupr( s_startserver.mapname.string );
}


/*
=================
StartServer_MapEvent
=================
*/
static void StartServer_MapEvent( void* ptr, int event ) {
	if( event != QM_ACTIVATED) {
		return;
	}

	s_startserver.currentmap = (s_startserver.page*MAX_MAPSPERPAGE) + (((menucommon_s*)ptr)->id - ID_PICTURES);
	StartServer_Update();
}

static void StartServer_Settings(void)
{
	s_startserver.actionhero.curvalue= 0;
	s_startserver.assimilation.curvalue= 0;
	s_startserver.disintegration.curvalue= 0;
	s_startserver.elimination.curvalue= 0;
	s_startserver.specialties.curvalue= 0;

	s_startserver.actionhero.generic.flags &= ~QMF_GRAYED;
	s_startserver.assimilation.generic.flags &= ~QMF_GRAYED;
	s_startserver.disintegration.generic.flags &= ~QMF_GRAYED;
	s_startserver.elimination.generic.flags &= ~QMF_GRAYED;
	s_startserver.specialties.generic.flags &= ~QMF_GRAYED;

	// Set assimilation button
	if  (gametype_remap[s_startserver.gametype.curvalue]== GT_TEAM )
	{
		s_startserver.assimilation.generic.flags &= ~QMF_HIDDEN;
	}
	else
	{
		s_startserver.assimilation.generic.flags |= QMF_HIDDEN;
	}
	

	// Set specialties button
//	if  (gametype_remap[s_startserver.gametype.curvalue]>= GT_TEAM )
//	{
//		s_startserver.specialties.generic.flags &= ~QMF_HIDDEN;
//	}
//	else
//	{
//		s_startserver.specialties.generic.flags |= QMF_HIDDEN;
//	}

	// Set assimilation button
	s_startserver.disintegration.generic.flags &= ~QMF_HIDDEN;

	// Set elimination button
	if  (gametype_remap[s_startserver.gametype.curvalue]< GT_CTF )
	{
		s_startserver.elimination.generic.flags &= ~QMF_HIDDEN;
	}
	else
	{
		s_startserver.elimination.generic.flags |= QMF_HIDDEN;
	}

	// Set action hero button
	if  (gametype_remap[s_startserver.gametype.curvalue]< GT_TEAM )
	{
		s_startserver.actionhero.generic.flags &= ~QMF_HIDDEN;
	}
	else
	{
		s_startserver.actionhero.generic.flags |= QMF_HIDDEN;
	}
}

/*
=================
StartServer_GametypeEvent
=================
*/
extern vmCvar_t	ui_language;
static void StartServer_GametypeEvent( void* ptr, int event ) {
	int			i;
	int			count;
	int			gamebits;
	int			matchbits;
	const char	*info;

	if( event != QM_ACTIVATED) {
		return;
	}


	count = UI_GetNumArenas();
	s_startserver.nummaps = 0;
	matchbits = 1 << gametype_remap[s_startserver.gametype.curvalue];
	if( gametype_remap[s_startserver.gametype.curvalue] == GT_FFA ) {
		matchbits |= ( 1 << GT_SINGLE_PLAYER );
	}
	for( i = 0; i < count; i++ ) {
		info = UI_GetArenaInfoByNumber( i );

		gamebits = GametypeBits( Info_ValueForKey( info, "type") );
		if( !( gamebits & matchbits ) ) {
			continue;
		}

		Q_strncpyz( s_startserver.maplist[s_startserver.nummaps], Info_ValueForKey( info, "map"), MAX_NAMELENGTH );
		Q_strupr( s_startserver.maplist[s_startserver.nummaps] );

		if (ui_language.string[0] == 0 || Q_stricmp ("ENGLISH",ui_language.string)==0 ) {
			Q_strncpyz( s_startserver.maplongname[s_startserver.nummaps], Info_ValueForKey( info, "longname"), MAX_NAMELENGTH );
		} else {
			Q_strncpyz( s_startserver.maplongname[s_startserver.nummaps], Info_ValueForKey( info, va("longname_%s",ui_language.string) ), MAX_NAMELENGTH );
			if (!s_startserver.maplongname[s_startserver.nummaps][0]) {
				Q_strncpyz( s_startserver.maplongname[s_startserver.nummaps], Info_ValueForKey( info, "longname"), MAX_NAMELENGTH );
			}
		}
		Q_strupr( s_startserver.maplongname[s_startserver.nummaps] );

		s_startserver.maprecommended[s_startserver.nummaps] = atoi(Info_ValueForKey( info, "recommended"));

		s_startserver.mapGamebits[s_startserver.nummaps] = gamebits;
		s_startserver.nummaps++;
	}
	s_startserver.maxpages = (s_startserver.nummaps + MAX_MAPSPERPAGE-1)/MAX_MAPSPERPAGE;
	s_startserver.page = 0;
	s_startserver.currentmap = 0;

	StartServer_Settings();

	StartServer_Update();

	// Zero these bad boys out.
	trap_Cvar_SetValue( "g_pModAssimilation", 0);
	trap_Cvar_SetValue( "g_pModActionHero", 0);
	trap_Cvar_SetValue( "g_pModDisintegration", 0);
	trap_Cvar_SetValue( "g_pModElimination", 0);
	trap_Cvar_SetValue( "g_pModSpecialties", 0);

	StartServerSetModButtons();
}


/*
=================
GetStartServerMods
=================
*/
void GetStartServerMods(void)
{

	s_startserver.assimilation.curvalue = Com_Clamp( 0, 1, trap_Cvar_VariableValue( "g_pModAssimilation"));

	s_startserver.disintegration.curvalue = Com_Clamp( 0, 1, trap_Cvar_VariableValue( "g_pModDisintegration"));

	s_startserver.actionhero.curvalue = Com_Clamp( 0, 1, trap_Cvar_VariableValue( "g_pModActionHero"));

	s_startserver.specialties.curvalue = Com_Clamp( 0, 1, trap_Cvar_VariableValue( "g_pModSpecialties"));

	s_startserver.elimination.curvalue = Com_Clamp( 0, 1, trap_Cvar_VariableValue( "g_pModElimination"));

	s_startserver.assimilation.curvalue = Com_Clamp( 0, 1, trap_Cvar_VariableValue( "g_pModAssimilation"));


}


/*
=================
SetServerButtons - where buttons are turned on/off or activated/grayed
=================
*/
void SetServerButtons(int gameType,menulist_s *assimilation,menulist_s *specialties,menulist_s *disintegration,menulist_s *elimination,menulist_s *actionhero)
{

	//Lots of checks because NULL  could be sent for one of the buttons

	if  (gameType == GT_TEAM )
	{
		if (assimilation)
		{
			assimilation->generic.flags &= ~QMF_HIDDEN;
		}
	}
	else
	{
		if (assimilation)
		{
			assimilation->generic.flags |= QMF_HIDDEN;
		}
	}

	// Set elimination button
	if  (gameType < GT_CTF )
	{
		if (elimination)
		{
			elimination->generic.flags &= ~QMF_HIDDEN;
		}
	}
	else
	{
		if (elimination)
		{
			elimination->generic.flags |= QMF_HIDDEN;
		}
	}

	// Set action hero button
	if  (gameType < GT_TEAM )
	{
		if (actionhero)
		{
			actionhero->generic.flags &= ~QMF_HIDDEN;
		}
	}
	else
	{
		if (actionhero)
		{
			actionhero->generic.flags |= QMF_HIDDEN;
		}
	}

	// Ungray everything
	if (actionhero)
	{
		actionhero->generic.flags &= ~QMF_GRAYED;
	}
	if (assimilation)
	{
		assimilation->generic.flags &= ~QMF_GRAYED;
	}
	if (disintegration)
	{
		disintegration->generic.flags &= ~QMF_GRAYED;
	}
	if (elimination)
	{
		elimination->generic.flags &= ~QMF_GRAYED;
	}
	if (specialties)
	{
		specialties->generic.flags &= ~QMF_GRAYED;
	}

	// If action hero is ON
	if (actionhero)
	{
		if (actionhero->curvalue==1)
		{	// Gray these out
			if (assimilation)
			{
				assimilation->generic.flags |= QMF_GRAYED;
			}
			if (disintegration)
			{
				disintegration->generic.flags |= QMF_GRAYED;
			}
			if (elimination)
			{
				elimination->generic.flags |= QMF_GRAYED;
			}
			if (specialties)
			{
				specialties->generic.flags |= QMF_GRAYED;
			}
		}
	}

	// If assimilation is ON
	if (assimilation)
	{
		if (assimilation->curvalue==1)
		{
			if (actionhero)
			{
				actionhero->generic.flags |= QMF_GRAYED;
			}
			if (disintegration)
			{
				disintegration->generic.flags |= QMF_GRAYED;
			}
			if (elimination)
			{
				elimination->generic.flags |= QMF_GRAYED;
			}
			if (specialties)
			{
				specialties->generic.flags |= QMF_GRAYED;
			}
		}
	}

	// If disintegration is ON
	if (disintegration)
	{
		if (disintegration->curvalue==1)
		{
			if (actionhero)
			{
				actionhero->generic.flags |= QMF_GRAYED;
			}
			if (assimilation)
			{
				assimilation->generic.flags |= QMF_GRAYED;
			}
			if (specialties)
			{
				specialties->generic.flags |= QMF_GRAYED;
			}
		}
	}

	// If elimination is ON
	if (elimination)
	{
		if (elimination->curvalue==1)
		{
			if (actionhero)
			{
				actionhero->generic.flags |= QMF_GRAYED;
			}
			if (assimilation)
			{
				assimilation->generic.flags |= QMF_GRAYED;
			}
		}
	}

	// If specialties is ON
	if (specialties)
	{
		if (specialties->curvalue==1)
		{
			if (assimilation)
			{
				assimilation->generic.flags |= QMF_GRAYED;
			}
			if (actionhero)
			{
				actionhero->generic.flags |= QMF_GRAYED;
			}
			if (disintegration)
			{
				disintegration->generic.flags |= QMF_GRAYED;
			}
		}
	}

}


//
// Where all the mods are set up
//
/*
=================
ServerGameMods - where game mod rules are
=================
*/
void ServerGameMods(int gametype,int *pModAssimilation,int *pModSpecialties,int *pModDisintegration,int *pModElimination,int *pModActionHero)
{
//Check gametype first
	// Check assimilation
	if ( gametype != GT_TEAM )
	{
		trap_Cvar_SetValue( "g_pModAssimilation", 0);
	}

	// Check elimination
	if  ( gametype >= GT_CTF )
	{
		trap_Cvar_SetValue( "g_pModElimination", 0);
	}

	// Check action hero
	if ( gametype >= GT_TEAM )
	{
		trap_Cvar_SetValue( "g_pModActionHero", 0);
	}

//now get these game mod values again
	*pModAssimilation = Com_Clamp( 0, 1, trap_Cvar_VariableValue( "g_pModAssimilation"));
	*pModDisintegration = Com_Clamp( 0, 1, trap_Cvar_VariableValue( "g_pModDisintegration"));
	*pModActionHero = Com_Clamp( 0, 1, trap_Cvar_VariableValue( "g_pModActionHero"));
	*pModSpecialties = Com_Clamp( 0, 1, trap_Cvar_VariableValue( "g_pModSpecialties"));
	*pModElimination = Com_Clamp( 0, 1, trap_Cvar_VariableValue( "g_pModElimination"));


//now check gamemods

	// Check Assimilation
	if ( *pModAssimilation )
	{
		trap_Cvar_SetValue( "g_pModDisintegration", 0);
		trap_Cvar_SetValue( "g_pModElimination", 0);
		trap_Cvar_SetValue( "g_pModActionHero", 0);
		trap_Cvar_SetValue( "g_pModSpecialties", 0);
	}

	// Check Specialties
	if ( *pModSpecialties )
	{
		trap_Cvar_SetValue( "g_pModDisintegration", 0);
		trap_Cvar_SetValue( "g_pModActionHero", 0);
		trap_Cvar_SetValue( "g_pModAssimilation", 0);
	}

	// Check Disintegration
	if ( *pModDisintegration )
	{
		trap_Cvar_SetValue( "g_pModAssimilation", 0);
		trap_Cvar_SetValue( "g_pModActionHero", 0);
		trap_Cvar_SetValue( "g_pModSpecialties", 0);
	}

	// Check Elimination
	if ( *pModElimination )
	{
		trap_Cvar_SetValue( "g_pModAssimilation", 0);
		trap_Cvar_SetValue( "g_pModActionHero", 0);
	}

	// Check Action Hero
	if ( *pModActionHero )
	{
		trap_Cvar_SetValue( "g_pModAssimilation", 0);
		trap_Cvar_SetValue( "g_pModSpecialties", 0);
		trap_Cvar_SetValue( "g_pModElimination", 0);
		trap_Cvar_SetValue( "g_pModDisintegration", 0);
	}

}

// Calls for the StartServer screen 
void StartServerSetModButtons(void)
{
		ServerGameMods(gametype_remap[s_startserver.gametype.curvalue],
			&s_startserver.assimilation.curvalue,
			&s_startserver.specialties.curvalue,
			&s_startserver.disintegration.curvalue,
			&s_startserver.elimination.curvalue,
			&s_startserver.actionhero.curvalue);

		SetServerButtons(gametype_remap[s_startserver.gametype.curvalue],
			&s_startserver.assimilation,
			&s_startserver.specialties,
			&s_startserver.disintegration,
			&s_startserver.elimination,
			&s_startserver.actionhero);
}


/*
=================
StartServer_MenuEvent
=================
*/
static void StartServer_MenuEvent( void* ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_PMOD_ASSIMILIATION:
		trap_Cvar_SetValue( "g_pModAssimilation", s_startserver.assimilation.curvalue);

		StartServerSetModButtons();

		break;

	case ID_PMOD_DISINTEGRATION:
		trap_Cvar_SetValue( "g_pModDisintegration", s_startserver.disintegration.curvalue);

		StartServerSetModButtons();

		break;
	case ID_PMOD_ACTIONHERO:

		trap_Cvar_SetValue( "g_pModActionHero", s_startserver.actionhero.curvalue);

		StartServerSetModButtons();

		break;
	case ID_PMOD_SPECIALTIES:
		trap_Cvar_SetValue( "g_pModSpecialties", s_startserver.specialties.curvalue);

		StartServerSetModButtons();

		break;
	case ID_PMOD_ELIMINATION:
		trap_Cvar_SetValue( "g_pModElimination", s_startserver.elimination.curvalue);

		StartServerSetModButtons();
		break;
	case ID_PREVPAGE:
		if( s_startserver.page > 0 ) {
			s_startserver.page--;
			StartServer_Update();
		}
		break;

	case ID_NEXTPAGE:
		if( s_startserver.page < s_startserver.maxpages - 1 ) {
			s_startserver.page++;
			StartServer_Update();
		}
		break;

	case ID_STARTSERVERNEXT:
		SetPlayerMod();
		trap_Cvar_SetValue( "g_gameType", gametype_remap[s_startserver.gametype.curvalue] );
		UI_ServerOptionsMenu( s_startserver.multiplayer );
		break;

	case ID_STARTSERVERBACK:
		UI_PopMenu();
		break;

	case ID_MAINMENU:
		UI_PopMenu();
		UI_MainMenu();
		break;
	}
}


/*
===============
StartServer_LevelshotDraw
===============
*/
static void StartServer_LevelshotDraw( void *self ) {
	menubitmap_s	*b;
	int				x;
	int				y;
	int				n;
	int				color = CT_DKGOLD1;

	b = (menubitmap_s *)self;

	if( !b->generic.name ) {
		return;
	}

	if( b->generic.name && !b->shader ) {
		b->shader = trap_R_RegisterShaderNoMip( b->generic.name );
		if( !b->shader && b->errorpic ) {
			b->shader = trap_R_RegisterShaderNoMip( b->errorpic );
		}
	}

	if( b->focuspic && !b->focusshader ) {
		b->focusshader = trap_R_RegisterShaderNoMip( b->focuspic );
	}

	x = b->generic.x;
	y = b->generic.y;
	if( b->shader ) {
		UI_DrawHandlePic( x, y, b->width, b->height, b->shader );
	}

	n = (s_startserver.page * MAX_MAPSPERPAGE) + (b->generic.id - ID_PICTURES);
	if (n == s_startserver.currentmap)
	{
		color = CT_LTGOLD1;
	}


	y += b->height;

	x += b->width / 2;
	y += 4;

	UI_DrawProportionalString( x, y, s_startserver.maplongname[n], UI_CENTER|UI_TINYFONT, colorTable[CT_WHITE] );
	UI_DrawProportionalString( x, y+12, s_startserver.maplist[n], UI_CENTER|UI_TINYFONT, colorTable[CT_LTGOLD1] );

	x = b->generic.x;
	y = b->generic.y;
	if( b->generic.flags & QMF_HIGHLIGHT ) {	
		UI_DrawHandlePic( x, y, b->width, b->height + 28, b->focusshader );
	}
}

#define	START_X_POS 150 //25
/*
=================
StartServer_Graphics
=================
*/
void StartServer_Graphics (void)
{
	int x;

	UI_MenuFrame2(&s_startserver.menu);

//	trap_R_SetColor( colorTable[CT_DKBROWN1]);
//	UI_DrawHandlePic(30,331, 47, 58, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30,203, 47, 186, uis.whiteShader);	// Middle left line
	UI_DrawProportionalString(  74,  150, "5164",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	//trap_R_SetColor( colorTable[CT_BLACK]);
	//UI_DrawHandlePic(30,55, 47, 280, uis.whiteShader);

	// Level pics frame
	trap_R_SetColor( colorTable[CT_DKBROWN1]);

	x = START_X_POS;

	UI_DrawProportionalString(  74,  30, "6801",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74, 207, "615222",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	// Left rounded ends for buttons
	trap_R_SetColor( colorTable[s_startserver.mainmenu.color]);
	UI_DrawHandlePic(s_startserver.mainmenu.generic.x - 14, s_startserver.mainmenu.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor( colorTable[s_startserver.back.color]);
	UI_DrawHandlePic(s_startserver.back.generic.x - 14, s_startserver.back.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	//UI_DrawHandlePic(s_startserver.assimilation.generic.x - 24, 56, 170, 18, uis.whiteShader);		// Top
	//UI_DrawHandlePic(s_startserver.assimilation.generic.x - 34, 46,32, -32,s_startserver.corner_ll); 
	//UI_DrawHandlePic(s_startserver.assimilation.generic.x - 34, 64, 18, 278, uis.whiteShader);		// Side
	//UI_DrawProportionalString(  s_startserver.assimilation.generic.x,  57, menu_normal_text[MNT_PARAMETERS],UI_SMALLFONT, colorTable[CT_BLACK]);

	trap_R_SetColor( colorTable[CT_DKPURPLE2]);

	//left bracket 'round server icons
	UI_DrawHandlePic(129 ,65, 16, 16, uis.graphicBracket1CornerLU);
	UI_DrawHandlePic(129, 81, 8, 276, uis.whiteShader);
	UI_DrawHandlePic(129,357, 16, -16, uis.graphicBracket1CornerLU);

	//right bracket
	UI_DrawHandlePic(565 ,65, -16, 16, uis.graphicBracket1CornerLU);
	UI_DrawHandlePic(573, 81, 8, 276, uis.whiteShader);
	UI_DrawHandlePic(565,357, -16, -16, uis.graphicBracket1CornerLU);
}

static void MapName_Draw( void *self ) 
{
	if (!Q_stricmp(s_startserver.mapname.string,menu_normal_text[MNT_NOMAPSFOUND]))
	{
		UI_DrawProportionalString(  s_startserver.mapname.generic.x,  s_startserver.mapname.generic.y,
			s_startserver.mapname.string,s_startserver.mapname.style, s_startserver.mapname.color);
	}
	else
	{
		UI_DrawProportionalString(  s_startserver.mapname.generic.x,  s_startserver.mapname.generic.y,
			s_startserver.maplongname[s_startserver.currentmap],s_startserver.mapname.style, s_startserver.mapname.color);
	}


}

/*
=================
StartServer_MenuDraw
=================
*/
static void StartServer_MenuDraw( void )
{
	StartServer_Graphics();

	Menu_Draw( &s_startserver.menu );
}

/*
=================
StartServer_MenuInit
=================
*/
static void StartServer_MenuInit(int multiplayer) {
	int	i;
	int	x;
	int	y,pad;
	static char mapnamebuffer[64];
	int	picWidth,picHeight;

	// zero set all our globals
	memset( &s_startserver, 0 ,sizeof(startserver_t) );

	s_startserver.multiplayer = multiplayer;

	StartServer_Cache();

	s_startserver.menu.wrapAround = qtrue;
	s_startserver.menu.fullscreen = qtrue;
    s_startserver.menu.draw							= StartServer_MenuDraw;
	s_startserver.menu.descX						= MENU_DESC_X;
	s_startserver.menu.descY						= MENU_DESC_Y;
	s_startserver.menu.titleX						= MENU_TITLE_X;
	s_startserver.menu.titleY						= MENU_TITLE_Y;

	if (s_startserver.multiplayer)
	{
		s_startserver.menu.titleI						= MNT_MULTIPLAYER_TITLE;
	}
	else
	{
		s_startserver.menu.titleI						= MNT_SINGLEPLAYER_TITLE;
	}
	s_startserver.menu.footNoteEnum					= MNT_CREATESERVER;

	s_startserver.mainmenu.generic.type				= MTYPE_BITMAP;      
	s_startserver.mainmenu.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_startserver.mainmenu.generic.x				= 110;
	s_startserver.mainmenu.generic.y				= 391;
	s_startserver.mainmenu.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_startserver.mainmenu.generic.id				= ID_MAINMENU;
	s_startserver.mainmenu.generic.callback			= StartServer_MenuEvent;
	s_startserver.mainmenu.width					= MENU_BUTTON_MED_WIDTH;
	s_startserver.mainmenu.height					= MENU_BUTTON_MED_HEIGHT;
	s_startserver.mainmenu.color					= CT_DKPURPLE1;
	s_startserver.mainmenu.color2					= CT_LTPURPLE1;
	s_startserver.mainmenu.textX					= 5;
	s_startserver.mainmenu.textY					= 2;
	s_startserver.mainmenu.textEnum					= MBT_MAINMENU;
	s_startserver.mainmenu.textcolor				= CT_BLACK;
	s_startserver.mainmenu.textcolor2				= CT_WHITE;

	s_startserver.back.generic.type				= MTYPE_BITMAP;      
	s_startserver.back.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_startserver.back.generic.x				= 110;
	s_startserver.back.generic.y				= 415;
	s_startserver.back.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_startserver.back.generic.id				= ID_STARTSERVERBACK;
	s_startserver.back.generic.callback			= StartServer_MenuEvent;
	s_startserver.back.width					= MENU_BUTTON_MED_WIDTH;
	s_startserver.back.height					= MENU_BUTTON_MED_HEIGHT;
	s_startserver.back.color					= CT_DKPURPLE1;
	s_startserver.back.color2					= CT_LTPURPLE1;
	s_startserver.back.textX					= 5;
	s_startserver.back.textY					= 2;
	s_startserver.back.textEnum					= MBT_BACK;
	s_startserver.back.textcolor				= CT_BLACK;
	s_startserver.back.textcolor2				= CT_WHITE;

	s_startserver.gametype.generic.type				= MTYPE_SPINCONTROL;
	s_startserver.gametype.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_startserver.gametype.generic.callback			= StartServer_GametypeEvent;
	s_startserver.gametype.generic.id				= ID_GAMETYPE;
	s_startserver.gametype.generic.x				= 249;
	s_startserver.gametype.generic.y				= 353;
	s_startserver.gametype.textEnum					= MBT_GAMETYPE;
	s_startserver.gametype.textcolor				= CT_BLACK;
	s_startserver.gametype.textcolor2				= CT_WHITE;
	s_startserver.gametype.color					= CT_DKPURPLE1;
	s_startserver.gametype.color2					= CT_LTPURPLE1;
	s_startserver.gametype.width					= 80;
	s_startserver.gametype.textX					= 5;
	s_startserver.gametype.textY					= 2;
	s_startserver.gametype.listnames				= gametype_items;

	x = 475;
	y = 90;
	pad = 30;
	s_startserver.assimilation.generic.type				= MTYPE_SPINCONTROL;
	s_startserver.assimilation.generic.name				= "menu/common/bar1.tga";
	s_startserver.assimilation.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_startserver.assimilation.generic.callback			= StartServer_MenuEvent;
	s_startserver.assimilation.generic.id				= ID_PMOD_ASSIMILIATION;
	s_startserver.assimilation.generic.x				= x;
	s_startserver.assimilation.generic.y				= y;
	s_startserver.assimilation.textEnum					= MBT_ASSIMILATION;
	s_startserver.assimilation.textcolor				= CT_BLACK;
	s_startserver.assimilation.textcolor2				= CT_WHITE;
	s_startserver.assimilation.color					= CT_DKPURPLE1;
	s_startserver.assimilation.color2					= CT_LTPURPLE1;
	s_startserver.assimilation.width					= 80;
	s_startserver.assimilation.textX					= 5;
	s_startserver.assimilation.textY					= 2;
	s_startserver.assimilation.listnames				= s_OffOnNone_Names;

	y += pad;
	s_startserver.specialties.generic.type				= MTYPE_SPINCONTROL;      
	s_startserver.specialties.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_startserver.specialties.generic.x					= x;
	s_startserver.specialties.generic.y					= y;
	s_startserver.specialties.generic.name				= "menu/common/bar1.tga";
	s_startserver.specialties.generic.id				= ID_PMOD_SPECIALTIES; 
	s_startserver.specialties.generic.callback			= StartServer_MenuEvent;
	s_startserver.specialties.color						= CT_DKPURPLE1;
	s_startserver.specialties.color2					= CT_LTPURPLE1;
	s_startserver.specialties.textX						= MENU_BUTTON_TEXT_X;
	s_startserver.specialties.textY						= MENU_BUTTON_TEXT_Y;
	s_startserver.specialties.textEnum					= MBT_SPECIALTIES;
	s_startserver.specialties.textcolor					= CT_BLACK;
	s_startserver.specialties.textcolor2				= CT_WHITE;	
	s_startserver.specialties.listnames					= s_OffOnNone_Names;
	s_startserver.specialties.width						= 80;

	y += pad;
	s_startserver.disintegration.generic.type				= MTYPE_SPINCONTROL;      
	s_startserver.disintegration.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_startserver.disintegration.generic.x					= x;
	s_startserver.disintegration.generic.y					= y;
	s_startserver.disintegration.generic.name				= "menu/common/bar1.tga";
	s_startserver.disintegration.generic.id					= ID_PMOD_DISINTEGRATION; 
	s_startserver.disintegration.generic.callback			= StartServer_MenuEvent;
	s_startserver.disintegration.color						= CT_DKPURPLE1;
	s_startserver.disintegration.color2						= CT_LTPURPLE1;
	s_startserver.disintegration.textX						= MENU_BUTTON_TEXT_X;
	s_startserver.disintegration.textY						= MENU_BUTTON_TEXT_Y;
	s_startserver.disintegration.textEnum					= MBT_DISINTEGRATION;
	s_startserver.disintegration.textcolor					= CT_BLACK;
	s_startserver.disintegration.textcolor2					= CT_WHITE;	
	s_startserver.disintegration.listnames					= s_OffOnNone_Names;
	s_startserver.disintegration.width						= 80;

	y += pad;
	s_startserver.elimination.generic.type					= MTYPE_SPINCONTROL;      
	s_startserver.elimination.generic.flags					= QMF_HIGHLIGHT_IF_FOCUS;
	s_startserver.elimination.generic.x						= x;
	s_startserver.elimination.generic.y						= y;
	s_startserver.elimination.generic.name					= "menu/common/bar1.tga";
	s_startserver.elimination.generic.id					= ID_PMOD_ELIMINATION; 
	s_startserver.elimination.generic.callback				= StartServer_MenuEvent;
	s_startserver.elimination.color							= CT_DKPURPLE1;
	s_startserver.elimination.color2						= CT_LTPURPLE1;
	s_startserver.elimination.textX							= MENU_BUTTON_TEXT_X;
	s_startserver.elimination.textY							= MENU_BUTTON_TEXT_Y;
	s_startserver.elimination.textEnum						= MBT_ELIMINATION;
	s_startserver.elimination.textcolor						= CT_BLACK;
	s_startserver.elimination.textcolor2					= CT_WHITE;	
	s_startserver.elimination.listnames						= s_OffOnNone_Names;
	s_startserver.elimination.width							= 80;

	y += pad;
	s_startserver.actionhero.generic.type				= MTYPE_SPINCONTROL;      
	s_startserver.actionhero.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_startserver.actionhero.generic.x					= x;
	s_startserver.actionhero.generic.y					= y;
	s_startserver.actionhero.generic.name				= "menu/common/bar1.tga";
	s_startserver.actionhero.generic.id					= ID_PMOD_ACTIONHERO; 
	s_startserver.actionhero.generic.callback			= StartServer_MenuEvent;
	s_startserver.actionhero.color						= CT_DKPURPLE1;
	s_startserver.actionhero.color2						= CT_LTPURPLE1;
	s_startserver.actionhero.textX						= MENU_BUTTON_TEXT_X;
	s_startserver.actionhero.textY						= MENU_BUTTON_TEXT_Y;
	s_startserver.actionhero.textEnum					= MBT_ACTIONHERO;
	s_startserver.actionhero.textcolor					= CT_BLACK;
	s_startserver.actionhero.textcolor2					= CT_WHITE;	
	s_startserver.actionhero.listnames					= s_OffOnNone_Names;
	s_startserver.actionhero.width							= 80;

	picWidth = 128;
	picHeight = 96;

	y =  83;
	x = START_X_POS + 5;
	for (i=0; i<MAX_MAPSPERPAGE; i++)
	{
		s_startserver.mappics[i].generic.type   = MTYPE_BITMAP;
		s_startserver.mappics[i].generic.flags  = QMF_LEFT_JUSTIFY|QMF_INACTIVE;
		s_startserver.mappics[i].generic.x	    = x;
		s_startserver.mappics[i].generic.y	    = y;
		s_startserver.mappics[i].generic.id		= ID_PICTURES+i;
		s_startserver.mappics[i].width  		= picWidth;
		s_startserver.mappics[i].height  	    = picHeight;
		s_startserver.mappics[i].focuspic       = GAMESERVER_SELECTED;
		s_startserver.mappics[i].errorpic       = GAMESERVER_UNKNOWNMAP;
		s_startserver.mappics[i].generic.ownerdraw = StartServer_LevelshotDraw;

		s_startserver.mapbuttons[i].generic.type     = MTYPE_BITMAP;
		s_startserver.mapbuttons[i].generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_NODEFAULTINIT;
		s_startserver.mapbuttons[i].generic.id       = ID_PICTURES+i;
		s_startserver.mapbuttons[i].generic.callback = StartServer_MapEvent;
		s_startserver.mapbuttons[i].generic.x	     = x - 30;
		s_startserver.mapbuttons[i].generic.y	     = y - 32;
		s_startserver.mapbuttons[i].width  		     = 253;
		s_startserver.mapbuttons[i].height  	     = 247;
		s_startserver.mapbuttons[i].generic.left     = x;
		s_startserver.mapbuttons[i].generic.top  	 = y;
		s_startserver.mapbuttons[i].generic.right    = x + 128;
		s_startserver.mapbuttons[i].generic.bottom   = y + 128;
		s_startserver.mapbuttons[i].focuspic         = GAMESERVER_SELECT;

		x += (picWidth+8);

		if (i==2)
		{
			y += 130;
			x = START_X_POS + 5;
		}
	}

	s_startserver.prevpage.generic.type	    = MTYPE_BITMAP;
	s_startserver.prevpage.generic.flags    = QMF_HIGHLIGHT_IF_FOCUS;
	s_startserver.prevpage.generic.callback = StartServer_MenuEvent;
	s_startserver.prevpage.generic.name		= "menu/common/arrow_left_16.tga";
	s_startserver.prevpage.generic.id	    = ID_PREVPAGE;
	s_startserver.prevpage.generic.x		= START_X_POS + 82; //114 //94
	s_startserver.prevpage.generic.y		= 347;
	s_startserver.prevpage.width  		    = 16;
	s_startserver.prevpage.height  		    = 16;
	s_startserver.prevpage.color			= CT_DKGOLD1;
	s_startserver.prevpage.color2			= CT_LTGOLD1;
	s_startserver.prevpage.textX			= MENU_BUTTON_TEXT_X;
	s_startserver.prevpage.textY			= MENU_BUTTON_TEXT_Y;
	s_startserver.prevpage.textEnum			= MBT_PREVMAPS;
	s_startserver.prevpage.textcolor		= CT_BLACK;
	s_startserver.prevpage.textcolor2		= CT_WHITE;

	s_startserver.nextpage.generic.type	    = MTYPE_BITMAP;
	s_startserver.nextpage.generic.flags    = QMF_HIGHLIGHT_IF_FOCUS;
	s_startserver.nextpage.generic.callback = StartServer_MenuEvent;
	s_startserver.nextpage.generic.name		= "menu/common/arrow_right_16.tga";
	s_startserver.nextpage.generic.id	    = ID_NEXTPAGE;
	s_startserver.nextpage.generic.x		= START_X_POS + 310; //342
	s_startserver.nextpage.generic.y		= 347;
	s_startserver.nextpage.width  		    = 16;
	s_startserver.nextpage.height  		    = 16;
	s_startserver.nextpage.color			= CT_DKGOLD1;
	s_startserver.nextpage.color2			= CT_LTGOLD1;
	s_startserver.nextpage.textX			= MENU_BUTTON_TEXT_X;
	s_startserver.nextpage.textY			= MENU_BUTTON_TEXT_Y;
	s_startserver.nextpage.textEnum			= MBT_NEXTMAPS;
	s_startserver.nextpage.textcolor		= CT_BLACK;
	s_startserver.nextpage.textcolor2		= CT_WHITE;

	s_startserver.mapname.generic.type  = MTYPE_PTEXT;
	s_startserver.mapname.generic.flags = QMF_INACTIVE;
	s_startserver.mapname.generic.x	    = START_X_POS + 205;
	s_startserver.mapname.generic.y	    = 347;
	s_startserver.mapname.string        = mapnamebuffer;
	s_startserver.mapname.style         = UI_SMALLFONT | UI_CENTER;
	s_startserver.mapname.color         = colorTable[CT_WHITE];
	s_startserver.mapname.generic.ownerdraw	= MapName_Draw;

	s_startserver.next.generic.type	    = MTYPE_BITMAP;
	s_startserver.next.generic.flags    = QMF_HIGHLIGHT_IF_FOCUS;
	s_startserver.next.generic.callback = StartServer_MenuEvent;
	s_startserver.next.generic.name		= "menu/common/square.tga";
	s_startserver.next.generic.id	    = ID_STARTSERVERNEXT;
	s_startserver.next.generic.x		= 462;
	s_startserver.next.generic.y		= 391;
	s_startserver.next.width  		    = 150;
	s_startserver.next.height  		    = 42;
	s_startserver.next.color			= CT_DKPURPLE1;
	s_startserver.next.color2			= CT_LTPURPLE1;
	s_startserver.next.textX			= MENU_BUTTON_TEXT_X + 5;
	s_startserver.next.textY			= MENU_BUTTON_TEXT_Y + 2;
	s_startserver.next.textEnum			= MBT_SETPARAMETERS;
	s_startserver.next.textcolor		= CT_BLACK;
	s_startserver.next.textcolor2		= CT_WHITE;

	s_startserver.item_null.generic.type	= MTYPE_BITMAP;
	s_startserver.item_null.generic.flags	= QMF_LEFT_JUSTIFY|QMF_MOUSEONLY|QMF_SILENT;
	s_startserver.item_null.generic.x		= 0;
	s_startserver.item_null.generic.y		= 0;
	s_startserver.item_null.width			= 640;
	s_startserver.item_null.height			= 480;


	Menu_AddItem( &s_startserver.menu, &s_startserver.mainmenu );
	Menu_AddItem( &s_startserver.menu, &s_startserver.back );
//	Menu_AddItem( &s_startserver.menu, &s_startserver.gametype );
	for (i=0; i<MAX_MAPSPERPAGE; i++)
	{
		Menu_AddItem( &s_startserver.menu, &s_startserver.mappics[i] );
		Menu_AddItem( &s_startserver.menu, &s_startserver.mapbuttons[i] );
	}

//	if (ui_cdkeychecked2.integer == 1) 
//	{
//		Menu_AddItem( &s_startserver.menu, &s_startserver.assimilation );
//		Menu_AddItem( &s_startserver.menu, &s_startserver.specialties );
//		Menu_AddItem( &s_startserver.menu, &s_startserver.disintegration );
//		Menu_AddItem( &s_startserver.menu, &s_startserver.elimination );
//		Menu_AddItem( &s_startserver.menu, &s_startserver.actionhero );
//	}

	Menu_AddItem( &s_startserver.menu, &s_startserver.prevpage );
	Menu_AddItem( &s_startserver.menu, &s_startserver.nextpage );
	Menu_AddItem( &s_startserver.menu, &s_startserver.next );
	Menu_AddItem( &s_startserver.menu, &s_startserver.mapname );
	Menu_AddItem( &s_startserver.menu, &s_startserver.item_null );

	GetStartServerMods();

	StartServer_Settings();

	StartServer_GametypeEvent( NULL, QM_ACTIVATED ); 
}


/*
=================
StartServer_Cache
=================
*/
void StartServer_Cache( void )
{
	int				i;
	const char		*info;
	qboolean		precache;
	char			picname[64];

	trap_R_RegisterShaderNoMip( GAMESERVER_SELECT );	
	trap_R_RegisterShaderNoMip( GAMESERVER_SELECTED );	
	trap_R_RegisterShaderNoMip( GAMESERVER_UNKNOWNMAP );

	s_startserver.corner_ll = trap_R_RegisterShaderNoMip("menu/common/corner_ll_18_18.tga");
	s_startserver.corner_lr = trap_R_RegisterShaderNoMip("menu/common/corner_lr_18_18.tga");

	precache = trap_Cvar_VariableValue("com_buildscript");

	s_startserver.nummaps = UI_GetNumArenas();

	for( i = 0; i < s_startserver.nummaps; i++ ) {
		info = UI_GetArenaInfoByNumber( i );

		Q_strncpyz( s_startserver.maplist[i], Info_ValueForKey( info, "map"), MAX_NAMELENGTH );
		Q_strupr( s_startserver.maplist[i] );

		s_startserver.mapGamebits[i] = GametypeBits( Info_ValueForKey( info, "type") );

		if (ui_language.string[0] == 0 || Q_stricmp ("ENGLISH",ui_language.string)==0 ) {
			Q_strncpyz( s_startserver.maplongname[i], Info_ValueForKey( info, "longname"), MAX_NAMELENGTH );
		} else {
			Q_strncpyz( s_startserver.maplongname[i], Info_ValueForKey( info, va("longname_%s",ui_language.string) ), MAX_NAMELENGTH );
			if (!s_startserver.maplongname[i][0]) {
				Q_strncpyz( s_startserver.maplongname[i], Info_ValueForKey( info, "longname"), MAX_NAMELENGTH );
			}
		}
		Q_strupr( s_startserver.maplongname[i] );

		s_startserver.maprecommended[i] = atoi(Info_ValueForKey( info, "recommended"));
		if( precache ) {
			Com_sprintf( picname, sizeof(picname), "levelshots/%s", s_startserver.maplist[i] );
			trap_R_RegisterShaderNoMip(picname);
		}
	}

	s_startserver.maxpages = (s_startserver.nummaps + MAX_MAPSPERPAGE-1)/MAX_MAPSPERPAGE;
}


/*
=================
UI_StartServerMenu
=================
*/
void UI_StartServerMenu( qboolean multiplayer ) 
{
	StartServer_MenuInit(multiplayer);
	UI_PushMenu( &s_startserver.menu );
	Menu_SetCursorToItem( &s_startserver.menu, &s_startserver.next);
}



/*
=============================================================================

SERVER OPTIONS MENU *****

=============================================================================
*/


#define PLAYER_SLOTS			12

typedef enum
{
	SO_TEAM_BLUE,
	SO_TEAM_RED,
	SO_NUM_TEAMS
} soTeamType_t;

typedef enum
{
	SO_PC_INFILTRATOR,
	SO_PC_SNIPER,
	SO_PC_HEAVY,
	SO_PC_DEMO,
	SO_PC_MEDIC,
	SO_PC_TECH,
	SO_NUM_PLAYER_CLASSES
} soClassType_t;

typedef struct {
	menuframework_s		menu;

	menutext_s			banner;

	menubitmap_s		mappic;

	menulist_s			dedicated;
	menufield_s			timelimit;
	menufield_s			fraglimit;
	menufield_s			flaglimit;
	menulist_s			friendlyfire;
	menufield_s			hostname;
	menubitmap_s		hostnamebackground1;
	menubitmap_s		hostnamebackground2;

	menulist_s			pure;
	menulist_s			botSkill;

	qhandle_t			corner_ur_12_18;
	qhandle_t			corner_lr_7_12;
	qhandle_t			teamShaders[SO_NUM_TEAMS];
	qhandle_t			pClassShaders[SO_NUM_PLAYER_CLASSES];

	menutext_s			player0;
	menulist_s			playerType[PLAYER_SLOTS];
	menutext_s			playerName[PLAYER_SLOTS];
	menulist_s			playerTeam[PLAYER_SLOTS];
	menulist_s			playerClass[PLAYER_SLOTS];

	menubitmap_s		go;
	menubitmap_s		back;
	menubitmap_s		mainmenu;
	menubitmap_s		advanced;

	qboolean			multiplayer;
	int					gametype;
	qboolean			specialties;
	char				mapnamebuffer[32];
	char				playerNameBuffers[PLAYER_SLOTS][16];
	qboolean			updatedPlayer;
	int					newUpdatedPlayer;

	qboolean			newBot;
	int					newBotIndex;
	char				newBotName[16];

} serveroptions_t;

static serveroptions_t s_serveroptions;
/*
static const char *dedicated_list[] = {
	"No",
	"LAN",
	"Internet",
	0
};
*/

static int dedicated_list[] = 
{
	MNT_DEDICATED_NO,
	MNT_DEDICATED_LAN,
	MNT_DEDICATED_INTERNET,
	0
};

/*
static const char *playerType_list[] = {
	"Open",
	"Bot",
	"----",
	0
};*/

static int playerType_list[] = 
{
	MNT_OPEN,
	MNT_BOT,
	MNT_CLOSED,
	0
};
/*
static const char *playerTeam_list[] = {
	"Blue",
	"Red",
	0
};
*/
static int playerTeam_list[] = 
{
	MNT_TEAM_BLUE,
	MNT_TEAM_RED,
	0
};

static int playerClass_list[] = 
{
	MNT_PC_INFILTRATOR,
	MNT_PC_SNIPER,
	MNT_PC_HEAVY,
	MNT_PC_DEMO,
	MNT_PC_MEDIC,
	MNT_PC_TECH,
	0
};

/*
static const char *botSkill_list[] = 
{
	"I Can Win",
	"Bring It On",
	"Hurt Me Plenty",
	"Hardcore",
	"Nightmare!",
	0
};
*/

static int botSkill_list[] =
{
	MNT_BABYLEVEL,
	MNT_EASYLEVEL,
	MNT_MEDIUMLEVEL,
	MNT_HARDLEVEL,
	MNT_NIGHTMARELEVEL,
	0
};


/*
=================
BotAlreadySelected
=================
*/
static qboolean BotAlreadySelected( const char *checkName ) {
	int		n;

	for( n = 1; n < PLAYER_SLOTS; n++ ) {
		if( s_serveroptions.playerType[n].curvalue != 1 ) {
			continue;
		}
		if( (s_serveroptions.gametype >= GT_TEAM) &&
			(s_serveroptions.playerTeam[n].curvalue != s_serveroptions.playerTeam[s_serveroptions.newBotIndex].curvalue ) ) {
			continue;
		}
		if( ( s_serveroptions.specialties ) &&
			(s_serveroptions.playerClass[n].curvalue != s_serveroptions.playerClass[s_serveroptions.newBotIndex].curvalue ) ) {
			continue;
		}
		if( Q_stricmp( checkName, s_serveroptions.playerNameBuffers[n] ) == 0 ) {
			return qtrue;
		}
	}

	return qfalse;
}


/*
=================
ServerOptions_Start
=================
*/
static void ServerOptions_Start( void ) {
	int		timelimit;
	int		fraglimit;
	int		maxclients,holdMaxClients;
	int		dedicated;
	int		friendlyfire;
	int		flaglimit;
	int		pure;
	int		skill;
	int		n;
	char	buf[64];

	char pClass[MAX_QPATH];
	char pRank[MAX_QPATH];

	timelimit	 = atoi( s_serveroptions.timelimit.field.buffer );
	fraglimit	 = atoi( s_serveroptions.fraglimit.field.buffer );
	flaglimit	 = atoi( s_serveroptions.flaglimit.field.buffer );
	dedicated	 = s_serveroptions.dedicated.curvalue;
	friendlyfire = s_serveroptions.friendlyfire.curvalue;
	pure		 = s_serveroptions.pure.curvalue;
	skill		 = s_serveroptions.botSkill.curvalue + 1;

	holdMaxClients = Com_Clamp( 0, 64, trap_Cvar_VariableValue( "sv_maxclients" ));

	//set maxclients
	for( n = 0, maxclients = 0; n < PLAYER_SLOTS; n++ ) {
		if( s_serveroptions.playerType[n].curvalue == 2 ) {
			continue;
		}
		if( (s_serveroptions.playerType[n].curvalue == 1) && (s_serveroptions.playerNameBuffers[n][0] == 0) ) {
			continue;
		}
		maxclients++;
	}

	if (holdMaxClients > maxclients)
	{
		maxclients = holdMaxClients;
	}

	switch( s_serveroptions.gametype ) {
	case GT_FFA:
	default:
		trap_Cvar_SetValue( "ui_ffa_fraglimit", fraglimit );
		trap_Cvar_SetValue( "ui_ffa_timelimit", timelimit );
		break;

	case GT_TOURNAMENT:
		trap_Cvar_SetValue( "ui_tourney_fraglimit", fraglimit );
		trap_Cvar_SetValue( "ui_tourney_timelimit", timelimit );
		break;

	case GT_TEAM:
		trap_Cvar_SetValue( "ui_team_fraglimit", fraglimit );
		trap_Cvar_SetValue( "ui_team_timelimit", timelimit );
		trap_Cvar_SetValue( "ui_team_friendly", friendlyfire );
		break;

	case GT_CTF:
		trap_Cvar_SetValue( "ui_ctf_fraglimit", fraglimit );
		trap_Cvar_SetValue( "ui_ctf_timelimit", timelimit );
		trap_Cvar_SetValue( "ui_ctf_friendly", friendlyfire );
		break;
	}

	trap_Cvar_SetValue( "sv_maxclients", Com_Clamp( 0, 64, maxclients ) );
	trap_Cvar_SetValue( "dedicated", Com_Clamp( 0, 2, dedicated ) );
	trap_Cvar_SetValue ("timelimit", Com_Clamp( 0, timelimit, timelimit ) );
	trap_Cvar_SetValue ("fraglimit", Com_Clamp( 0, fraglimit, fraglimit ) );
	trap_Cvar_SetValue ("capturelimit", Com_Clamp( 0, flaglimit, flaglimit ) );
	trap_Cvar_SetValue( "g_friendlyfire", friendlyfire );
	trap_Cvar_Set("sv_hostname", s_serveroptions.hostname.field.buffer );

	if ( dedicated == 0 )
	{
		trap_Cvar_SetValue( "sv_pure", 0 );
	}
	else
	{
		trap_Cvar_SetValue( "sv_pure", pure );
	}

	// the wait commands will allow the dedicated to take effect
	trap_Cmd_ExecuteText( EXEC_APPEND, va( "wait ; wait ; map %s\n", s_startserver.maplist[s_startserver.currentmap] ) );

	// add bots
	trap_Cmd_ExecuteText( EXEC_APPEND, "wait 3\n" );
	for( n = 1; n < PLAYER_SLOTS; n++ ) {
		if( s_serveroptions.playerType[n].curvalue != 1 ) {
			continue;
		}
		if( s_serveroptions.playerNameBuffers[n][0] == 0 ) {
			continue;
		}
		if( s_serveroptions.playerNameBuffers[n][0] == '-' ) {
			continue;
		}
		if( s_serveroptions.gametype >= GT_TEAM ) {
			Com_sprintf( buf, sizeof(buf), "addbot %s %i %s %s\n", s_serveroptions.playerNameBuffers[n], skill,
				menu_normal_text[playerTeam_list[s_serveroptions.playerTeam[n].curvalue]], menu_normal_text[playerClass_list[s_serveroptions.playerClass[n].curvalue]] );
		}
		else {
			Com_sprintf( buf, sizeof(buf), "addbot %s %i free %s\n", s_serveroptions.playerNameBuffers[n], skill, menu_normal_text[playerClass_list[s_serveroptions.playerClass[n].curvalue]] );
		}
		trap_Cmd_ExecuteText( EXEC_APPEND, buf );
	}

	trap_Cvar_VariableStringBuffer( "ui_playerClass", pClass, MAX_QPATH );
	trap_Cvar_VariableStringBuffer( "ui_playerRank", pRank, MAX_QPATH );

	// set player's team
	if( dedicated == 0 )
	{
		if ( s_serveroptions.gametype >= GT_TEAM ) {
			trap_Cmd_ExecuteText( EXEC_APPEND, va( "wait 5; team %s; class %s; rank %s\n", menu_normal_text[playerTeam_list[s_serveroptions.playerTeam[0].curvalue]], pClass, pRank ) );
		}
		else {
			trap_Cmd_ExecuteText( EXEC_APPEND, va( "wait 5; class %s; rank %s \n", pClass, pRank ) );
		}
	}
}


/*
=================
ServerOptions_InitPlayerItems
=================
*/
static void ServerOptions_InitPlayerItems( void ) {
	int		n;
	int		v;

	// init types
	if( s_serveroptions.multiplayer ) {
		v = 0;	// open
	}
	else {
		v = 1;	// bot
	}
	
	for( n = 0; n < PLAYER_SLOTS; n++ ) {
		s_serveroptions.playerType[n].curvalue = v;
	}

	if( s_serveroptions.multiplayer && (s_serveroptions.gametype < GT_TEAM) ) {
		for( n = 8; n < PLAYER_SLOTS; n++ ) {
			s_serveroptions.playerType[n].curvalue = 2;
		}
	}

	// if not a dedicated server, first slot is reserved for the human on the server
	if( s_serveroptions.dedicated.curvalue == 0 ) 
	{	// human
		char buffer[32];
		s_serveroptions.playerType[0].generic.flags |= QMF_INACTIVE;
		s_serveroptions.playerType[0].curvalue = 0;
		trap_Cvar_VariableStringBuffer( "name", buffer, sizeof(buffer) );
		Q_CleanStr( buffer );
		Q_strncpyz(s_serveroptions.playerNameBuffers[0], buffer, sizeof(s_serveroptions.playerNameBuffers[0]) );
	}

	// init teams
	if( s_serveroptions.gametype >= GT_TEAM ) 
	{
		for( n = 0; n < (PLAYER_SLOTS / 2); n++ ) 
		{
			s_serveroptions.playerTeam[n].curvalue = 0;
		}

		for( ; n < PLAYER_SLOTS; n++ ) 
		{
			s_serveroptions.playerTeam[n].curvalue = 1;
		}


	}
	else {
		for( n = 0; n < PLAYER_SLOTS; n++ ) {
			s_serveroptions.playerTeam[n].generic.flags |= (QMF_INACTIVE|QMF_HIDDEN);
		}
	}
	// init classes
	for( n = 0; n < PLAYER_SLOTS; n++ ) {
		s_serveroptions.playerClass[n].curvalue = 0;
		if ( !s_serveroptions.specialties )	{
			s_serveroptions.playerClass[n].generic.flags |= (QMF_INACTIVE|QMF_HIDDEN);
		}
	}
}


/*
=================
ServerOptions_SetPlayerItems
=================
*/
static int ServerOptions_SetPlayerItems( void ) {
	int		start;
	int		n,maxClients,holdmaxClients;

	// types
//	for( n = 0; n < PLAYER_SLOTS; n++ ) {
//		if( (!s_serveroptions.multiplayer) && (n > 0) && (s_serveroptions.playerType[n].curvalue == 0) ) {
//			s_serveroptions.playerType[n].curvalue = 1;
//		}
//	}
	holdmaxClients = Com_Clamp( 0, 64, trap_Cvar_VariableValue( "sv_maxclients" ));

	// names
	if( s_serveroptions.dedicated.curvalue == 0 ) 
	{
		s_serveroptions.player0.string = (char*)menu_normal_text[MNT_HUMAN];
		s_serveroptions.playerName[0].generic.flags &= ~QMF_HIDDEN;
		start = 1;
	}
	else 
	{
		s_serveroptions.player0.string = (char*)menu_normal_text[MNT_OPEN];
		start = 0;
	}

	maxClients = start;
	for( n = start; n < PLAYER_SLOTS; n++ )
	{
		if (s_serveroptions.playerType[n].curvalue < 2)
		{
			maxClients++;
		}

		if( s_serveroptions.playerType[n].curvalue == 1 ) 
		{
			s_serveroptions.playerName[n].generic.flags &= ~(QMF_INACTIVE|QMF_HIDDEN);
		}
		else {
			s_serveroptions.playerName[n].generic.flags |= (QMF_INACTIVE|QMF_HIDDEN);
		}
	}

	if (maxClients < holdmaxClients)
	{
		maxClients = holdmaxClients;
	}

	for( n = start; n < PLAYER_SLOTS; n++ ) {
		if( s_serveroptions.playerType[n].curvalue == 2 || !s_serveroptions.specialties ) {
			//specialties not on or... slot closed
			s_serveroptions.playerClass[n].generic.flags |= (QMF_INACTIVE|QMF_HIDDEN);
		}
		else {
			s_serveroptions.playerClass[n].generic.flags &= ~(QMF_INACTIVE|QMF_HIDDEN);
		}
	}

	// teams
	if( s_serveroptions.gametype < GT_TEAM ) {
		return(maxClients);
	}

	for( n = start; n < PLAYER_SLOTS; n++ ) {
		if( s_serveroptions.playerType[n].curvalue == 2 ) {
			s_serveroptions.playerTeam[n].generic.flags |= (QMF_INACTIVE|QMF_HIDDEN);
		}
		else {
			s_serveroptions.playerTeam[n].generic.flags &= ~(QMF_INACTIVE|QMF_HIDDEN);
		}
	}

	return(maxClients);
}


/*
=================
ServerOptions_Event
=================
*/
static void ServerOptions_Event( void* ptr, int event ) 
{
	int maxClients;

	switch( ((menucommon_s*)ptr)->id ) {

	case ID_PLAYER_TEAM:
		break;

	case ID_PLAYER_CLASS:
		break;

	case ID_PLAYER_TYPE:
		if( event != QM_ACTIVATED ) {
			break;
		}
		maxClients = ServerOptions_SetPlayerItems();
		trap_Cvar_SetValue( "sv_maxclients", maxClients );
		break;

	case ID_MAXCLIENTS:
	case ID_DEDICATED:
		ServerOptions_SetPlayerItems();
		break;

	case ID_GO:
		if( event != QM_ACTIVATED ) {
			break;
		}
		ServerOptions_Start();
		break;

	case ID_BACK:
		if( event != QM_ACTIVATED ) {
			break;
		}

		UI_PopMenu();
		break;

	case ID_ADVANCED:
		if( event != QM_ACTIVATED ) 
		{
			break;
		}
		UI_ServerAdvancedOptions(0);
		break;
	case ID_MAINMENU:
		if( event != QM_ACTIVATED ) {
			break;
		}
		UI_PopMenu();
		UI_MainMenu();
		break;
	}
}


static void ServerOptions_PlayerNameEvent( void* ptr, int event ) {
	int		n;

	if( event != QM_ACTIVATED ) 
	{
		return;
	}

	n = ((menutext_s*)ptr)->generic.id;
	s_serveroptions.newBotIndex = n;

	if (((n==0) && ( s_serveroptions.dedicated.curvalue == 0 )) || // Player who started server
		(s_serveroptions.playerType[n].curvalue == MNT_OPEN))		// Human player
	{
		s_serveroptions.updatedPlayer = qtrue;
		s_serveroptions.newUpdatedPlayer = n;
		UI_PlayerSettingsMenu(PS_MENU_SINGLEPLAYER);
	}
	else // Choosing a bot name
	{
		UI_BotSelectMenu( s_serveroptions.playerNameBuffers[n] );
	}
}


/*
=================
ServerOptions_StatusBar
=================
*/
static void ServerOptions_StatusBar( void* ptr ) 
{
	UI_DrawProportionalString( 320, 417, menu_normal_text[MNT_NOLIMIT], UI_CENTER|UI_SMALLFONT, colorTable[CT_WHITE]);
}


/*
===============
ServerOptions_LevelshotDraw
===============
*/
static void ServerOptions_LevelshotDraw( void *self ) {
	menubitmap_s	*b;
	int				x;
	int				y;

	// Map background
	trap_R_SetColor( colorTable[CT_DKGREY]);
	UI_DrawHandlePic(  (s_serveroptions.mappic.generic.x - 2), (s_serveroptions.mappic.generic.y - 2), (s_serveroptions.mappic.width + 4),  148, uis.whiteShader);

	// strange place for this, but it works
	if( s_serveroptions.newBot ) {
		Q_strncpyz( s_serveroptions.playerNameBuffers[s_serveroptions.newBotIndex], s_serveroptions.newBotName, 16 );
		s_serveroptions.newBot = qfalse;
	}

	if (s_serveroptions.updatedPlayer)
	{
		s_serveroptions.updatedPlayer = qfalse;
		trap_Cvar_VariableStringBuffer( "name", s_serveroptions.playerNameBuffers[s_serveroptions.newUpdatedPlayer], 16 );
	}

	trap_R_SetColor( colorTable[CT_WHITE]);
	b = (menubitmap_s *)self;

	Bitmap_Draw( b );

	x = b->generic.x;
	y = b->generic.y + b->height;
	UI_FillRect( x, y, b->width, 24, colorBlack ); //40

	x += b->width / 2;
	y += 4;

	UI_DrawProportionalString( x, y+3, s_startserver.maplongname[s_startserver.currentmap], UI_CENTER|UI_TINYFONT, colorTable[CT_WHITE] );

	y += SMALLCHAR_HEIGHT;
	//UI_DrawProportionalString( x, y, menu_normal_text[gametype_items[gametype_remap2[s_serveroptions.gametype]]], UI_CENTER|UI_SMALLFONT, colorTable[CT_DKBROWN1] );

}


static void ServerOptions_InitBotNames( void )
{
	int			count;
	int			n;
	const char	*arenaInfo;
	const char	*botInfo;
	char		*p;
	char		*bot;
	char		bots[MAX_INFO_STRING];
	int			max;

	if ( s_serveroptions.gametype >= GT_TEAM )
	{
		Q_strncpyz( s_serveroptions.playerNameBuffers[1], "Janeway", 16 );
		Q_strncpyz( s_serveroptions.playerNameBuffers[2], "Seven", 16 );
		Q_strncpyz( s_serveroptions.playerNameBuffers[3], "Chakotay", 16 );
		Q_strncpyz( s_serveroptions.playerNameBuffers[6], "Paris", 16 );
		Q_strncpyz( s_serveroptions.playerNameBuffers[7], "Kim", 16 );
		Q_strncpyz( s_serveroptions.playerNameBuffers[8], "Tuvok", 16 );
		Q_strncpyz( s_serveroptions.playerNameBuffers[9], "Doctor", 16 );
		s_serveroptions.playerType[4].curvalue = 2;
		s_serveroptions.playerType[5].curvalue = 2;
		s_serveroptions.playerType[10].curvalue = 2;
		s_serveroptions.playerType[11].curvalue = 2;
		return;
	}

	count = 1;	// skip the first slot, reserved for a human

	// get info for this map
	arenaInfo = UI_GetArenaInfoByMap( s_serveroptions.mapnamebuffer );

	// get the bot info - we'll seed with them if any are listed
	Q_strncpyz( bots, Info_ValueForKey( arenaInfo, "bots" ), sizeof(bots) );
	p = &bots[0];
	while( *p && count < PLAYER_SLOTS )
	{
		//skip spaces
		while( *p && *p == ' ' )
		{
			p++;
		}
		if( !p )
		{
			break;
		}

		// mark start of bot name
		bot = p;

		// skip until space of null
		while( *p && *p != ' ' )
		{
			p++;
		}
		if( *p )
		{
			*p++ = 0;
		}

		botInfo = UI_GetBotInfoByName( bot );
		bot = Info_ValueForKey( botInfo, "name" );

		Q_strncpyz( s_serveroptions.playerNameBuffers[count], bot, sizeof(s_serveroptions.playerNameBuffers[count]) );
		count++;
	}

	max = Com_Clamp( 0, 64, trap_Cvar_VariableValue( "sv_maxclients" ));

	if (max > PLAYER_SLOTS)
	{
		max = PLAYER_SLOTS;
	}

	// set the rest of the bot slots to "---"
	for( n = count; n < max; n++ )
	{
		strcpy( s_serveroptions.playerNameBuffers[n], "--------" );
	}

	// pad up to #8 as open slots
	for( ;count < max; count++ )
	{
		s_serveroptions.playerType[count].curvalue = 0;
	}

	// close off the rest by default
	for( ;count < PLAYER_SLOTS; count++ ) 
	{
//		if( s_serveroptions.playerType[count].curvalue == 1 ) {
			s_serveroptions.playerType[count].curvalue = 2;
//		}
	}
}


/*
=================
ServerOptions_SetMenuItems
=================
*/
static void ServerOptions_SetMenuItems( void )
{
	static char picname[64];

	switch( s_serveroptions.gametype )
	{
	case GT_FFA:
	default:
		Com_sprintf( s_serveroptions.fraglimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_ffa_fraglimit" ) ) );
		Com_sprintf( s_serveroptions.timelimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_ffa_timelimit" ) ) );
		break;

	case GT_TOURNAMENT:
		Com_sprintf( s_serveroptions.fraglimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_tourney_fraglimit" ) ) );
		Com_sprintf( s_serveroptions.timelimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_tourney_timelimit" ) ) );
		break;

	case GT_TEAM:
		Com_sprintf( s_serveroptions.fraglimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_team_fraglimit" ) ) );
		Com_sprintf( s_serveroptions.timelimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_team_timelimit" ) ) );
		s_serveroptions.friendlyfire.curvalue = (int)Com_Clamp( 0, 1, trap_Cvar_VariableValue( "ui_team_friendly" ) );
		break;

	case GT_CTF:
		Com_sprintf( s_serveroptions.flaglimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 100, trap_Cvar_VariableValue( "ui_ctf_capturelimit" ) ) );
		Com_sprintf( s_serveroptions.timelimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_ctf_timelimit" ) ) );
		s_serveroptions.friendlyfire.curvalue = (int)Com_Clamp( 0, 1, trap_Cvar_VariableValue( "ui_ctf_friendly" ) );
		break;
	}

	Q_strncpyz( s_serveroptions.hostname.field.buffer, UI_Cvar_VariableString( "sv_hostname" ), sizeof( s_serveroptions.hostname.field.buffer ) );
	s_serveroptions.pure.curvalue = Com_Clamp( 0, 1, trap_Cvar_VariableValue( "sv_pure" ) );


	// set the map pic
	Com_sprintf( picname, 64, "levelshots/%s", s_startserver.maplist[s_startserver.currentmap] );
	s_serveroptions.mappic.generic.name = picname;

	// set the map name
	strcpy( s_serveroptions.mapnamebuffer, s_startserver.mapname.string );
	Q_strupr( s_serveroptions.mapnamebuffer );

	// get the player selections initialized
	ServerOptions_InitPlayerItems();
	ServerOptions_SetPlayerItems();

	// seed bot names
	ServerOptions_InitBotNames();
	ServerOptions_SetPlayerItems();
}

/*
=================
PlayerName_Draw
=================
*/
static void PlayerName_Draw( void *item )
{
	menutext_s	*s;
	float		*color;
	int			x, y;
	int			style;
	qboolean	focus;

	s = (menutext_s *)item;

	x = s->generic.x;
	y =	s->generic.y;

	style = UI_SMALLFONT;
	focus = (s->generic.parent->cursor == s->generic.menuPosition);

	if ( s->generic.flags & QMF_GRAYED )
	{
		color = text_color_disabled;
	}
	else if ( focus )
	{
		color = colorTable[CT_LTGOLD1];
		style |= UI_PULSE;
	}
	else if ( s->generic.flags & QMF_BLINK )
	{
		color = colorTable[CT_LTGOLD1];
		style |= UI_BLINK;
	}
	else
	{
		color = colorTable[CT_LTGOLD1];
	}

	if ( focus )
	{
		// draw cursor
		UI_FillRect( s->generic.left, s->generic.top, s->generic.right-s->generic.left+1, s->generic.bottom-s->generic.top+1, listbar_color ); 
		UI_DrawChar( x, y, 13, UI_CENTER|UI_BLINK|UI_SMALLFONT, color);
	}

	UI_DrawString( x - SMALLCHAR_WIDTH, y, s->generic.name, style|UI_RIGHT, color, qtrue );
	UI_DrawString( x + SMALLCHAR_WIDTH, y, s->string, style|UI_LEFT, color, qtrue );
}

/*
=================
ServerOptionsMenu_Graphics
=================
*/
void ServerOptionsMenu_Graphics (void)
{
	UI_MenuFrame2(&s_serveroptions.menu);

	trap_R_SetColor( colorTable[CT_LTORANGE]);
	UI_DrawHandlePic(  30, 203, 47, 69, uis.whiteShader);	// Menu Frame
	UI_DrawHandlePic(  30, 275, 47, 66, uis.whiteShader);	// Menu Frame
	UI_DrawHandlePic(  30, 344, 47, 45, uis.whiteShader);	// Menu Frame

	// Players frame
	trap_R_SetColor( colorTable[CT_DKORANGE]);
	UI_DrawHandlePic( 354, 100,  16,  32, s_serveroptions.corner_ur_12_18);	// Corner at top
	UI_DrawHandlePic( 355, 368,  16,  16, s_serveroptions.corner_lr_7_12);	// Corner at bottom
	UI_DrawHandlePic(  80, 100, 280,  18, uis.whiteShader);	// Top
	UI_DrawHandlePic( 358, 117,  12, 253, uis.whiteShader);	// Right side
	UI_DrawHandlePic(  80, 371, 281,   7, uis.whiteShader);	// Bottom
	UI_DrawProportionalString(  93, 101, menu_normal_text[MNT_SERVERPLAYERS], UI_SMALLFONT, colorTable[CT_BLACK] );

	// Server frame
	trap_R_SetColor( colorTable[CT_DKORANGE]);
	UI_DrawHandlePic(  376,  60, -16,  32, s_serveroptions.corner_ur_12_18);	// Corner at top
	UI_DrawHandlePic(  375, 367, -16,  16, s_serveroptions.corner_lr_7_12);	// Corner at bottom
	UI_DrawHandlePic(  391,  60, 221,  18, uis.whiteShader);	// Top
	UI_DrawHandlePic(  376,  80,  12, 289, uis.whiteShader);	// Left side
	UI_DrawHandlePic(  391, 370, 221,   7, uis.whiteShader);	// Bottom
	UI_DrawProportionalString( 396,  61, menu_normal_text[MNT_SERVERINFO], UI_SMALLFONT, colorTable[CT_BLACK] );

	trap_R_SetColor( colorTable[s_serveroptions.mainmenu.color]);
	UI_DrawHandlePic(s_serveroptions.mainmenu.generic.x - 14, s_serveroptions.mainmenu.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor( colorTable[s_serveroptions.back.color]);
	UI_DrawHandlePic(s_serveroptions.back.generic.x - 14, s_serveroptions.back.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor( colorTable[s_serveroptions.back.color]);
	UI_DrawHandlePic(s_serveroptions.advanced.generic.x - 14, s_serveroptions.advanced.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	UI_DrawProportionalString(  74,   28, "6211",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  150, "6700",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  206, "1523",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  278, "23",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  347, "68311",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "34-5",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
}
/*
=================
ServerOptions_MenuDraw
=================
*/
static void ServerOptions_MenuDraw (void)
{
	ServerOptionsMenu_Graphics();

	Menu_Draw( &s_serveroptions.menu );
}

/*
=================
ServerOptions_MenuInit2
=================
*/
static void ServerOptions_MenuInit2( int specialties) 
{
	int	assim,n;

	assim = trap_Cvar_VariableValue( "g_pModAssimilation");

	s_serveroptions.specialties = specialties;

	if (!assim)	// No timelimit in assimilation
	{
		s_serveroptions.timelimit.generic.flags	&= ~(QMF_INACTIVE|QMF_HIDDEN);
	}
	else
	{
		s_serveroptions.timelimit.generic.flags	|= (QMF_INACTIVE|QMF_HIDDEN);
	}


	s_serveroptions.fraglimit.generic.flags	|= (QMF_INACTIVE|QMF_HIDDEN);
	if ( s_serveroptions.gametype != GT_CTF ) 
	{
		if (!assim)	// No points in assimilation
		{
			s_serveroptions.fraglimit.generic.flags	&= ~(QMF_INACTIVE|QMF_HIDDEN);
		}
	}


	for( n = 0; n < PLAYER_SLOTS; n++ ) 
	{
		if ( specialties && s_serveroptions.playerType[n].curvalue != 2 )
		{
			s_serveroptions.playerClass[n].generic.flags	&= ~(QMF_INACTIVE|QMF_HIDDEN);
		}
		else
		{
			s_serveroptions.playerClass[n].generic.flags	|= (QMF_INACTIVE|QMF_HIDDEN);
		}
	}
}

/*
=================
ServerOptions_MenuInit
=================
*/
//#define OPTIONS_X	456
#define OPTIONS_X	488

static void ServerOptions_MenuInit( qboolean multiplayer ) 
{
	int		x,y,yInc;
	int		n,assim;

	memset( &s_serveroptions, 0 ,sizeof(serveroptions_t) );
	s_serveroptions.multiplayer = multiplayer;
	s_serveroptions.gametype = (int)Com_Clamp( 0, 5, trap_Cvar_VariableValue( "g_gameType" ) );
	s_serveroptions.specialties = (qboolean)trap_Cvar_VariableValue( "g_pModSpecialties" );

	ServerOptions_Cache();

	s_serveroptions.menu.wrapAround = qtrue;
	s_serveroptions.menu.fullscreen = qtrue;
	s_serveroptions.menu.wrapAround = qtrue;
	s_serveroptions.menu.draw						= ServerOptions_MenuDraw;
	s_serveroptions.menu.descX						= MENU_DESC_X;
	s_serveroptions.menu.descY						= MENU_DESC_Y;
	s_serveroptions.menu.listX						= 230;
	s_serveroptions.menu.listY						= 188;
	s_serveroptions.menu.titleX						= MENU_TITLE_X;
	s_serveroptions.menu.titleY						= MENU_TITLE_Y;
	s_serveroptions.menu.titleI						= MNT_CONTROLSMENU_TITLE;
	s_serveroptions.menu.footNoteEnum				= MNT_SERVEROPTIONS;

	s_serveroptions.mappic.generic.type			= MTYPE_BITMAP;
	s_serveroptions.mappic.generic.flags		= QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	s_serveroptions.mappic.generic.x			= 420;
	s_serveroptions.mappic.generic.y			= 100;
	s_serveroptions.mappic.width				= 160;
	s_serveroptions.mappic.height				= 120;
	s_serveroptions.mappic.errorpic				= GAMESERVER_UNKNOWNMAP;
	s_serveroptions.mappic.generic.ownerdraw	= ServerOptions_LevelshotDraw;

	y = 248; //248  //254
	yInc = BIGCHAR_HEIGHT + 8; //4 //6
	if( s_serveroptions.gametype != GT_CTF ) 
	{	// Frag limit (Point limit)
		s_serveroptions.fraglimit.generic.type					= MTYPE_FIELD;
		s_serveroptions.fraglimit.generic.flags					= QMF_NUMBERSONLY|QMF_SMALLFONT;
		s_serveroptions.fraglimit.generic.x						= OPTIONS_X;
		s_serveroptions.fraglimit.generic.y						= y;
		s_serveroptions.fraglimit.generic.statusbar				= ServerOptions_StatusBar;
		s_serveroptions.fraglimit.field.widthInChars			= 3;
		s_serveroptions.fraglimit.field.maxchars				= 3;
		s_serveroptions.fraglimit.field.style					= UI_SMALLFONT;
		s_serveroptions.fraglimit.field.titleEnum				= MBT_POINTLIMIT;
		s_serveroptions.fraglimit.field.titlecolor				= CT_LTGOLD1;
		s_serveroptions.fraglimit.field.textcolor				= CT_DKGOLD1;
		s_serveroptions.fraglimit.field.textcolor2				= CT_LTGOLD1;
	}
	else 
	{
		// Capture Limit
		s_serveroptions.flaglimit.generic.type					= MTYPE_FIELD;
		s_serveroptions.flaglimit.generic.flags					= QMF_NUMBERSONLY|QMF_SMALLFONT;
		s_serveroptions.flaglimit.generic.x						= OPTIONS_X + 12;
		s_serveroptions.flaglimit.generic.y						= y;
		s_serveroptions.flaglimit.generic.statusbar				= ServerOptions_StatusBar;
		s_serveroptions.flaglimit.field.widthInChars			= 3;
		s_serveroptions.flaglimit.field.maxchars				= 3;
		s_serveroptions.flaglimit.field.style					= UI_SMALLFONT;
		s_serveroptions.flaglimit.field.titleEnum				= MBT_CAPTURELIMIT;
		s_serveroptions.flaglimit.field.titlecolor				= CT_LTGOLD1;
		s_serveroptions.flaglimit.field.textcolor				= CT_DKGOLD1;
		s_serveroptions.flaglimit.field.textcolor2				= CT_LTGOLD1;
	}

	//y += yInc;
	// Time Limit
	s_serveroptions.timelimit.generic.type					= MTYPE_FIELD;
	s_serveroptions.timelimit.generic.flags					= QMF_NUMBERSONLY|QMF_SMALLFONT;
	s_serveroptions.timelimit.generic.x						= OPTIONS_X;
	s_serveroptions.timelimit.generic.y						= y;
	s_serveroptions.timelimit.generic.statusbar				= ServerOptions_StatusBar;
	s_serveroptions.timelimit.field.widthInChars			= 3;
	s_serveroptions.timelimit.field.maxchars				= 3;
	s_serveroptions.timelimit.field.style					= UI_SMALLFONT;
	s_serveroptions.timelimit.field.titleEnum				= MBT_TIMELIMIT;
	s_serveroptions.timelimit.field.titlecolor				= CT_LTGOLD1;
	s_serveroptions.timelimit.field.textcolor				= CT_DKGOLD1;
	s_serveroptions.timelimit.field.textcolor2				= CT_LTGOLD1;

	if( s_serveroptions.gametype >= GT_TEAM ) 
	{
		y += BIGCHAR_HEIGHT+2;
		// Friendly Fire
		s_serveroptions.friendlyfire.generic.type		= MTYPE_SPINCONTROL;
		s_serveroptions.friendlyfire.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS|QMF_SMALLFONT;
		s_serveroptions.friendlyfire.generic.x			= OPTIONS_X-96;
		s_serveroptions.friendlyfire.generic.y			= y;
		s_serveroptions.friendlyfire.textEnum			= MBT_FRIENDLYFIRE;
		s_serveroptions.friendlyfire.textcolor			= CT_BLACK;
		s_serveroptions.friendlyfire.textcolor2			= CT_WHITE;
		s_serveroptions.friendlyfire.color				= CT_DKPURPLE1;
		s_serveroptions.friendlyfire.color2				= CT_LTPURPLE1;
		s_serveroptions.friendlyfire.textX				= 5;
		s_serveroptions.friendlyfire.textY				= 2;
		s_serveroptions.friendlyfire.listnames			= s_OffOnNone_Names;
	}

	y += yInc;
	// Pure Server
	s_serveroptions.pure.generic.type			= MTYPE_SPINCONTROL;
	s_serveroptions.pure.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS|QMF_SMALLFONT;
	s_serveroptions.pure.generic.x				= OPTIONS_X-96;
	s_serveroptions.pure.generic.y				= y;
	s_serveroptions.pure.textEnum				= MBT_PURESERVER;
	s_serveroptions.pure.textcolor				= CT_BLACK;
	s_serveroptions.pure.textcolor2				= CT_WHITE;
	s_serveroptions.pure.color					= CT_DKPURPLE1;
	s_serveroptions.pure.color2					= CT_LTPURPLE1;
	s_serveroptions.pure.textX					= 5;
	s_serveroptions.pure.textY					= 2;
	s_serveroptions.pure.listnames				= s_OffOnNone_Names;


	if( s_serveroptions.multiplayer ) 
	{
		y += yInc;
		s_serveroptions.dedicated.generic.type		= MTYPE_SPINCONTROL;
		s_serveroptions.dedicated.generic.id		= ID_DEDICATED;
		s_serveroptions.dedicated.generic.flags		= QMF_SMALLFONT;
		s_serveroptions.dedicated.generic.callback	= ServerOptions_Event;
		s_serveroptions.dedicated.generic.x			= OPTIONS_X-96;
		s_serveroptions.dedicated.generic.y			= y;
		s_serveroptions.dedicated.listnames			= dedicated_list;
		s_serveroptions.dedicated.textEnum			= MBT_DEDICATEDSERVER;
		s_serveroptions.dedicated.textcolor			= CT_BLACK;
		s_serveroptions.dedicated.textcolor2			= CT_WHITE;
		s_serveroptions.dedicated.color				= CT_DKPURPLE1;
		s_serveroptions.dedicated.color2				= CT_LTPURPLE1;
		s_serveroptions.dedicated.textX				= 5;
		s_serveroptions.dedicated.textY				= 2;

	}

	y += yInc;
	s_serveroptions.botSkill.generic.type			= MTYPE_SPINCONTROL;
	s_serveroptions.botSkill.generic.flags			= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_serveroptions.botSkill.textEnum				= MBT_HCSKILL;
	s_serveroptions.botSkill.generic.x				= OPTIONS_X-96;
	s_serveroptions.botSkill.generic.y				= y;
	s_serveroptions.botSkill.listnames				= botSkill_list;
	s_serveroptions.botSkill.curvalue				= 1;
	s_serveroptions.botSkill.textcolor			= CT_BLACK;
	s_serveroptions.botSkill.textcolor2			= CT_WHITE;
	s_serveroptions.botSkill.color				= CT_DKPURPLE1;
	s_serveroptions.botSkill.color2				= CT_LTPURPLE1;
	s_serveroptions.botSkill.textX				= 5;
	s_serveroptions.botSkill.textY				= 2;


	if( s_serveroptions.multiplayer ) 
	{
		s_serveroptions.hostname.generic.type       = MTYPE_FIELD;
		s_serveroptions.hostname.generic.flags      = QMF_SMALLFONT;
		s_serveroptions.hostname.generic.x          = 180;
		s_serveroptions.hostname.generic.y	        = 63;
		s_serveroptions.hostname.field.widthInChars = 18;
		s_serveroptions.hostname.field.maxchars     = 64;
		s_serveroptions.hostname.field.style		= UI_SMALLFONT;
		s_serveroptions.hostname.field.titleEnum	= MBT_HOSTNAME;
		s_serveroptions.hostname.field.titlecolor	= CT_BLACK;
		s_serveroptions.hostname.field.textcolor	= CT_DKGOLD1;
		s_serveroptions.hostname.field.textcolor2	= CT_LTGOLD1;

		s_serveroptions.hostnamebackground1.generic.type			= MTYPE_BITMAP;      
		s_serveroptions.hostnamebackground1.generic.flags		= QMF_INACTIVE;
		s_serveroptions.hostnamebackground1.generic.x			= 80;
		s_serveroptions.hostnamebackground1.generic.y			= 60;
		s_serveroptions.hostnamebackground1.generic.name			= GRAPHIC_SQUARE;
		s_serveroptions.hostnamebackground1.width				= 265;
		s_serveroptions.hostnamebackground1.height				= 22;
		s_serveroptions.hostnamebackground1.color				= CT_DKPURPLE1;
		s_serveroptions.hostnamebackground1.textEnum				= MBT_NONE;

		s_serveroptions.hostnamebackground2.generic.type		= MTYPE_BITMAP;      
		s_serveroptions.hostnamebackground2.generic.flags		= QMF_INACTIVE;
		s_serveroptions.hostnamebackground2.generic.x			= 180;
		s_serveroptions.hostnamebackground2.generic.y			= 63;
		s_serveroptions.hostnamebackground2.generic.name		= GRAPHIC_SQUARE;
		s_serveroptions.hostnamebackground2.width				= 153;
		s_serveroptions.hostnamebackground2.height				= 17;
		s_serveroptions.hostnamebackground2.color				= CT_BLACK;
		s_serveroptions.hostnamebackground2.textEnum			= MBT_NONE;

	}

	y = 123;
	x = 90;
	s_serveroptions.player0.generic.type			= MTYPE_TEXT;
	s_serveroptions.player0.generic.flags			= QMF_SMALLFONT | QMF_INACTIVE;
	s_serveroptions.player0.generic.x				= 90;
	s_serveroptions.player0.generic.y				= y;
	s_serveroptions.player0.color					= colorTable[CT_LTGOLD1];
	s_serveroptions.player0.style					= UI_LEFT|UI_SMALLFONT;

	y = 123;
	for( n = 0; n < PLAYER_SLOTS; n++ ) 
	{
		s_serveroptions.playerType[n].generic.type		= MTYPE_SPINCONTROL;
		s_serveroptions.playerType[n].generic.flags		= QMF_SMALLFONT;
		s_serveroptions.playerType[n].generic.id		= ID_PLAYER_TYPE;
		s_serveroptions.playerType[n].generic.callback	= ServerOptions_Event;
		s_serveroptions.playerType[n].generic.x			= x;
		s_serveroptions.playerType[n].generic.y			= y;
		s_serveroptions.playerType[n].listnames			= playerType_list;
		s_serveroptions.playerType[n].focusWidth		= 30;
		s_serveroptions.playerType[n].focusHeight		= 18;
		s_serveroptions.playerType[n].color				= CT_DKPURPLE1;
		s_serveroptions.playerType[n].color2			= CT_LTPURPLE1;
		s_serveroptions.playerType[n].textEnum			= MBT_PLAYER1 + n;
		s_serveroptions.playerType[n].textX				= 3;
		s_serveroptions.playerType[n].textY				= 2;
		s_serveroptions.playerType[n].textcolor			= CT_BLACK;
		s_serveroptions.playerType[n].textcolor2		= CT_WHITE;

		s_serveroptions.playerName[n].generic.type		= MTYPE_TEXT;
		s_serveroptions.playerName[n].generic.flags		= QMF_SMALLFONT;
		s_serveroptions.playerName[n].generic.x			= x + 46;
		s_serveroptions.playerName[n].generic.y			= y;
		s_serveroptions.playerName[n].generic.callback	= ServerOptions_PlayerNameEvent;
		s_serveroptions.playerName[n].generic.id		= n;
		s_serveroptions.playerName[n].generic.ownerdraw	= PlayerName_Draw;
		s_serveroptions.playerName[n].color				= colorTable[CT_LTGOLD1];
		s_serveroptions.playerName[n].style				= UI_SMALLFONT;
		s_serveroptions.playerName[n].string			= s_serveroptions.playerNameBuffers[n];
		s_serveroptions.playerName[n].focusX			= s_serveroptions.playerName[n].generic.x;
		s_serveroptions.playerName[n].focusY			= s_serveroptions.playerName[n].generic.y;
		s_serveroptions.playerName[n].focusHeight		= SMALLCHAR_HEIGHT;
		s_serveroptions.playerName[n].focusWidth		= 14 * SMALLCHAR_WIDTH;

		s_serveroptions.playerTeam[n].generic.type		= MTYPE_SPINCONTROL;
		//s_serveroptions.playerTeam[n].generic.flags		= QMF_GRAPHICLIST;
		s_serveroptions.playerTeam[n].generic.callback	= ServerOptions_Event;
		s_serveroptions.playerTeam[n].generic.id		= ID_PLAYER_TEAM;
		s_serveroptions.playerTeam[n].generic.x			= 296;
		s_serveroptions.playerTeam[n].generic.y			= y;
		s_serveroptions.playerTeam[n].listnames			= playerTeam_list;
		s_serveroptions.playerTeam[n].listshaders		= s_serveroptions.teamShaders;
		s_serveroptions.playerTeam[n].listX				= 1;
		s_serveroptions.playerTeam[n].listY				= 1;
		s_serveroptions.playerTeam[n].focusWidth		= 20;
		s_serveroptions.playerTeam[n].focusHeight		= 18;
		s_serveroptions.playerTeam[n].width				= 20;
		s_serveroptions.playerTeam[n].height			= 18;

//MCG ADD:
//		if ( s_serveroptions.specialties )
//		{
			s_serveroptions.playerClass[n].generic.type		= MTYPE_SPINCONTROL;
			//s_serveroptions.playerClass[n].generic.flags	= QMF_GRAPHICLIST;
			s_serveroptions.playerClass[n].generic.callback	= ServerOptions_Event;
			s_serveroptions.playerClass[n].generic.id		= ID_PLAYER_CLASS;
			s_serveroptions.playerClass[n].generic.x		= 322;
			s_serveroptions.playerClass[n].generic.y		= y;
			s_serveroptions.playerClass[n].listnames		= playerClass_list;
			s_serveroptions.playerClass[n].listshaders		= s_serveroptions.pClassShaders;
			s_serveroptions.playerClass[n].listX			= 1;
			s_serveroptions.playerClass[n].listY			= 1;
			s_serveroptions.playerClass[n].focusWidth		= 28;
			s_serveroptions.playerClass[n].focusHeight		= 18;
			s_serveroptions.playerClass[n].width			= 28;
			s_serveroptions.playerClass[n].height			= 18;
//		}
//MCG END
		y += ( SMALLCHAR_HEIGHT + 4 );
	}

	s_serveroptions.mainmenu.generic.type			= MTYPE_BITMAP;      
	s_serveroptions.mainmenu.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_serveroptions.mainmenu.generic.x				= 110;
	s_serveroptions.mainmenu.generic.y				= 391;
	s_serveroptions.mainmenu.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	s_serveroptions.mainmenu.generic.id				= ID_MAINMENU;
	s_serveroptions.mainmenu.generic.callback		= ServerOptions_Event;
	s_serveroptions.mainmenu.width					= MENU_BUTTON_MED_WIDTH;
	s_serveroptions.mainmenu.height					= MENU_BUTTON_MED_HEIGHT;
	s_serveroptions.mainmenu.color					= CT_DKPURPLE1;
	s_serveroptions.mainmenu.color2					= CT_LTPURPLE1;
	s_serveroptions.mainmenu.textX					= MENU_BUTTON_TEXT_X;
	s_serveroptions.mainmenu.textY					= MENU_BUTTON_TEXT_Y;
	s_serveroptions.mainmenu.textEnum				= MBT_MAINMENU;
	s_serveroptions.mainmenu.textcolor				= CT_BLACK;
	s_serveroptions.mainmenu.textcolor2				= CT_WHITE;

	s_serveroptions.back.generic.type				= MTYPE_BITMAP;
	s_serveroptions.back.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_serveroptions.back.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_serveroptions.back.generic.callback			= ServerOptions_Event;
	s_serveroptions.back.generic.id					= ID_BACK;
	s_serveroptions.back.generic.x					= 110;
	s_serveroptions.back.generic.y					= 415;
	s_serveroptions.back.width  					= MENU_BUTTON_MED_WIDTH;
	s_serveroptions.back.height  					= MENU_BUTTON_MED_HEIGHT;
	s_serveroptions.back.color						= CT_DKPURPLE1;
	s_serveroptions.back.color2						= CT_LTPURPLE1;
	s_serveroptions.back.textX						= 5;
	s_serveroptions.back.textY						= 2;
	s_serveroptions.back.textEnum					= MBT_BACK;
	s_serveroptions.back.textcolor					= CT_BLACK;
	s_serveroptions.back.textcolor2					= CT_WHITE;

	s_serveroptions.advanced.generic.type				= MTYPE_BITMAP;
	s_serveroptions.advanced.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_serveroptions.advanced.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_serveroptions.advanced.generic.callback			= ServerOptions_Event;
	s_serveroptions.advanced.generic.id					= ID_ADVANCED;
	s_serveroptions.advanced.generic.x					= 293;
	s_serveroptions.advanced.generic.y					= 391;
	s_serveroptions.advanced.width  					= MENU_BUTTON_MED_WIDTH;
	s_serveroptions.advanced.height  					= MENU_BUTTON_MED_HEIGHT;
	s_serveroptions.advanced.color						= CT_DKPURPLE1;
	s_serveroptions.advanced.color2						= CT_LTPURPLE1;
	s_serveroptions.advanced.textX						= 5;
	s_serveroptions.advanced.textY						= 2;
	s_serveroptions.advanced.textEnum					= MBT_ADVANCEDSERVER;
	s_serveroptions.advanced.textcolor					= CT_BLACK;
	s_serveroptions.advanced.textcolor2					= CT_WHITE;

	s_serveroptions.go.generic.type					= MTYPE_BITMAP;
	s_serveroptions.go.generic.name					= GRAPHIC_SQUARE;
	s_serveroptions.go.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_serveroptions.go.generic.callback				= ServerOptions_Event;
	s_serveroptions.go.generic.id					= ID_GO;
	s_serveroptions.go.generic.x					= 462;
	s_serveroptions.go.generic.y					= 391;
	s_serveroptions.go.width  						= 150;
	s_serveroptions.go.height  						= 42;
	s_serveroptions.go.color						= CT_DKPURPLE1;
	s_serveroptions.go.color2						= CT_LTPURPLE1;
	s_serveroptions.go.textX						= 5;
	s_serveroptions.go.textY						= 2;
	s_serveroptions.go.textEnum						= MBT_ENGAGE;
	s_serveroptions.go.textcolor					= CT_BLACK;
	s_serveroptions.go.textcolor2					= CT_WHITE;

	Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.mappic );
	Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.botSkill );
	Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.player0 );

	for( n = 0; n < PLAYER_SLOTS; n++ ) 
	{
		if( n != 0 ) 
		{
			Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.playerType[n] );
		}

		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.playerName[n] );

		if( s_serveroptions.gametype >= GT_TEAM ) 
		{
			Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.playerTeam[n] );
		}

		//if( s_serveroptions.specialties ) 
		{
			Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.playerClass[n] );
		}
	}

	assim = trap_Cvar_VariableValue( "g_pModAssimilation");

#if 0
	if ( s_serveroptions.gametype != GT_CTF ) 
	{
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.fraglimit );
	}
	else 
	{
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.flaglimit );
	}
#endif

//	if (!assim)	// No points in assimilation
//	{
//		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.timelimit );
//	}

	if ( s_serveroptions.gametype >= GT_TEAM ) 
	{
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.friendlyfire );
	}

	Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.pure );

	if ( s_serveroptions.multiplayer ) 
	{
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.dedicated );
	}

	if ( s_serveroptions.multiplayer ) 
	{
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.hostnamebackground1 );
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.hostnamebackground2 );
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.hostname );
	}

	Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.mainmenu );
	Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.back );
	Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.advanced );
	Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.go );

	ServerOptions_SetMenuItems();
	ServerOptions_MenuInit2(s_serveroptions.specialties);
}

/*
=================
ServerOptions_Cache
=================
*/
void ServerOptions_Cache( void ) 
{
	trap_R_RegisterShaderNoMip( GAMESERVER_SELECT );
	trap_R_RegisterShaderNoMip( GAMESERVER_UNKNOWNMAP );

	s_serveroptions.corner_ur_12_18  = trap_R_RegisterShaderNoMip("menu/common/corner_ur_18_12");
	s_serveroptions.corner_lr_7_12  = trap_R_RegisterShaderNoMip("menu/common/corner_lr_7_12");

	//register the team and pClass shaders
	s_serveroptions.pClassShaders[SO_PC_INFILTRATOR] = trap_R_RegisterShaderNoMip( "menu/art/pc_infiltrator.tga" );//PC_INFILTRATOR,//fast, low attack
	s_serveroptions.pClassShaders[SO_PC_SNIPER] = trap_R_RegisterShaderNoMip( "menu/art/pc_sniper.tga" );//PC_SNIPER,//sneaky, snipe only
	s_serveroptions.pClassShaders[SO_PC_HEAVY] = trap_R_RegisterShaderNoMip( "menu/art/pc_heavy.tga" );//PC_HEAVY,//slow, heavy attack
	s_serveroptions.pClassShaders[SO_PC_DEMO] = trap_R_RegisterShaderNoMip( "menu/art/pc_demo.tga" );//PC_DEMO,//go boom
	s_serveroptions.pClassShaders[SO_PC_MEDIC] = trap_R_RegisterShaderNoMip( "menu/art/pc_medic.tga" );//PC_MEDIC,//heal
	s_serveroptions.pClassShaders[SO_PC_TECH] = trap_R_RegisterShaderNoMip( "menu/art/pc_tech.tga" );//PC_TECH,//operate

	s_serveroptions.teamShaders[SO_TEAM_BLUE] = trap_R_RegisterShaderNoMip( "sprites/team_blue" );
	s_serveroptions.teamShaders[SO_TEAM_RED] = trap_R_RegisterShaderNoMip( "sprites/team_red" );
}


/*
=================
UI_ServerOptionsMenu
=================
*/
static void UI_ServerOptionsMenu( qboolean multiplayer ) 
{
	ServerOptions_MenuInit( multiplayer );
	UI_PushMenu( &s_serveroptions.menu );
}



/*
=============================================================================

BOT SELECT MENU *****

=============================================================================
*/


#define BOTSELECT_SELECT		"menu/art/opponents_select"
#define BOTSELECT_SELECTED		"menu/art/opponents_selected"

#define PLAYERGRID_COLS			5
#define PLAYERGRID_ROWS			3
#define MAX_MODELSPERPAGE		(PLAYERGRID_ROWS * PLAYERGRID_COLS)


typedef struct {
	menuframework_s	menu;

	menutext_s		banner;

	menubitmap_s	pics[MAX_MODELSPERPAGE];
	menubitmap_s	picbuttons[MAX_MODELSPERPAGE];
	menutext_s		picnames[MAX_MODELSPERPAGE];

	menubitmap_s	arrows;
	menubitmap_s	left;
	menubitmap_s	right;
	menutext_s		chosenname;

	menubitmap_s	go;
	menubitmap_s	back;
	menubitmap_s	mainmenu;
	qhandle_t		corner_ll_18_18;

	int				numBots;
	int				modelpage;
	int				numpages;
	int				selectedmodel;
	int				sortedBotNums[MAX_BOTS];
	char			boticons[MAX_MODELSPERPAGE][MAX_QPATH];
	char			botnames[MAX_MODELSPERPAGE][16];
} botSelectInfo_t;

static botSelectInfo_t	botSelectInfo;


/*
=================
UI_BotSelectMenu_SortCompare
=================
*/
static int QDECL UI_BotSelectMenu_SortCompare( const void *arg1, const void *arg2 ) {
	int			num1, num2;
	const char	*info1, *info2;
	const char	*name1, *name2;

	num1 = *(int *)arg1;
	num2 = *(int *)arg2;

	info1 = UI_GetBotInfoByNumber( num1 );
	info2 = UI_GetBotInfoByNumber( num2 );

	name1 = Info_ValueForKey( info1, "name" );
	name2 = Info_ValueForKey( info2, "name" );

	return Q_stricmp( name1, name2 );
}


/*
=================
UI_BotSelectMenu_BuildList
=================
*/
static void UI_BotSelectMenu_BuildList( void ) {
	int		n;

	botSelectInfo.modelpage = 0;
	botSelectInfo.numBots = UI_GetNumBots();
	botSelectInfo.numpages = botSelectInfo.numBots / MAX_MODELSPERPAGE;
	if( botSelectInfo.numBots % MAX_MODELSPERPAGE ) {
		botSelectInfo.numpages++;
	}

	// initialize the array
	for( n = 0; n < botSelectInfo.numBots; n++ ) {
		botSelectInfo.sortedBotNums[n] = n;
	}

	// now sort it
	qsort( botSelectInfo.sortedBotNums, botSelectInfo.numBots, sizeof(botSelectInfo.sortedBotNums[0]), UI_BotSelectMenu_SortCompare );
}

extern void PlayerIcon( const char *modelAndSkin, char *iconName, int iconNameMaxSize );	//from ui_splevel

/*
=================
UI_BotSelectMenu_UpdateGrid
=================
*/
static void UI_BotSelectMenu_UpdateGrid( void ) {
	const char	*info;
	int			i;
    int			j,len;

	j = botSelectInfo.modelpage * MAX_MODELSPERPAGE;
	for( i = 0; i < (PLAYERGRID_ROWS * PLAYERGRID_COLS); i++, j++) {
		if( j < botSelectInfo.numBots ) { 
			info = UI_GetBotInfoByNumber( botSelectInfo.sortedBotNums[j] );
			PlayerIcon( Info_ValueForKey( info, "model" ), botSelectInfo.boticons[i], MAX_QPATH );
			Q_strncpyz( botSelectInfo.botnames[i], Info_ValueForKey( info, "name" ), 16 );
			Q_CleanStr( botSelectInfo.botnames[i] );

			len = UI_ProportionalStringWidth(botSelectInfo.picnames[i].string,UI_SMALLFONT);
			if (len > 1)
			{
				if (botSelectInfo.picnames[i].style	& UI_SMALLFONT)
				{
					botSelectInfo.picnames[i].generic.y +=5;
					botSelectInfo.picnames[i].style		&= ~UI_SMALLFONT;
				}
				botSelectInfo.picnames[i].style		|= UI_TINYFONT;
			}
			else
			{
				if (botSelectInfo.picnames[i].style	& UI_TINYFONT)
				{
					botSelectInfo.picnames[i].generic.y -=5;
					botSelectInfo.picnames[i].style		&= ~UI_TINYFONT;
				}
				botSelectInfo.picnames[i].style	|= UI_SMALLFONT;
			}


 			botSelectInfo.pics[i].generic.name = botSelectInfo.boticons[i];
			if( BotAlreadySelected( botSelectInfo.botnames[i] ) ) {
				botSelectInfo.picnames[i].color = colorTable[CT_LTGOLD1];
			}
			else {
				botSelectInfo.picnames[i].color =  colorTable[CT_DKGOLD1];
			}
			botSelectInfo.picbuttons[i].generic.flags &= ~QMF_INACTIVE;
		}
		else {
			// dead slot
 			botSelectInfo.pics[i].generic.name         = NULL;
			botSelectInfo.picbuttons[i].generic.flags |= QMF_INACTIVE;
			botSelectInfo.botnames[i][0] = 0;
		}

 		botSelectInfo.pics[i].generic.flags       &= ~QMF_HIGHLIGHT;
 		botSelectInfo.pics[i].shader               = 0;
 		botSelectInfo.picbuttons[i].generic.flags |= QMF_PULSEIFFOCUS;
	}

	// set selected model
	i = botSelectInfo.selectedmodel % MAX_MODELSPERPAGE;
	botSelectInfo.pics[i].generic.flags |= QMF_HIGHLIGHT;
	botSelectInfo.picbuttons[i].generic.flags &= ~QMF_PULSEIFFOCUS;

	if( botSelectInfo.numpages > 1 ) {
		if( botSelectInfo.modelpage > 0 ) {
			botSelectInfo.left.generic.flags &= ~QMF_INACTIVE;
		}
		else {
			botSelectInfo.left.generic.flags |= QMF_INACTIVE;
		}

		if( botSelectInfo.modelpage < (botSelectInfo.numpages - 1) ) {
			botSelectInfo.right.generic.flags &= ~QMF_INACTIVE;
		}
		else {
			botSelectInfo.right.generic.flags |= QMF_INACTIVE;
		}
	}
	else {
		// hide left/right markers
		botSelectInfo.left.generic.flags |= QMF_INACTIVE;
		botSelectInfo.right.generic.flags |= QMF_INACTIVE;
	}
}


/*
=================
UI_BotSelectMenu_Default
=================
*/
static void UI_BotSelectMenu_Default( char *bot ) {
	const char	*botInfo;
	const char	*test;
	int			n;
	int			i;

	for( n = 0; n < botSelectInfo.numBots; n++ ) {
		botInfo = UI_GetBotInfoByNumber( n );
		test = Info_ValueForKey( botInfo, "name" );
		if( Q_stricmp( bot, test ) == 0 ) {
			break;
		}
	}
	if( n == botSelectInfo.numBots ) {
		botSelectInfo.selectedmodel = 0;
		return;
	}

	for( i = 0; i < botSelectInfo.numBots; i++ ) {
		if( botSelectInfo.sortedBotNums[i] == n ) {
			break;
		}
	}
	if( i == botSelectInfo.numBots ) {
		botSelectInfo.selectedmodel = 0;
		return;
	}

	botSelectInfo.selectedmodel = i;
}


/*
=================
UI_BotSelectMenu_LeftEvent
=================
*/
static void UI_BotSelectMenu_LeftEvent( void* ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}
	if( botSelectInfo.modelpage > 0 ) {
		botSelectInfo.modelpage--;
		botSelectInfo.selectedmodel = botSelectInfo.modelpage * MAX_MODELSPERPAGE;
		UI_BotSelectMenu_UpdateGrid();
	}
}


/*
=================
UI_BotSelectMenu_RightEvent
=================
*/
static void UI_BotSelectMenu_RightEvent( void* ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}
	if( botSelectInfo.modelpage < botSelectInfo.numpages - 1 ) {
		botSelectInfo.modelpage++;
		botSelectInfo.selectedmodel = botSelectInfo.modelpage * MAX_MODELSPERPAGE;
		UI_BotSelectMenu_UpdateGrid();
	}
}


/*
=================
UI_BotSelectMenu_BotEvent
=================
*/
static void UI_BotSelectMenu_BotEvent( void* ptr, int event ) {
	int		i;

	if( event != QM_ACTIVATED ) {
		return;
	}

	for( i = 0; i < (PLAYERGRID_ROWS * PLAYERGRID_COLS); i++ ) {
 		botSelectInfo.pics[i].generic.flags &= ~QMF_HIGHLIGHT;
 		botSelectInfo.picbuttons[i].generic.flags |= QMF_PULSEIFFOCUS;
	}

	// set selected
	i = ((menucommon_s*)ptr)->id;
	botSelectInfo.pics[i].generic.flags |= QMF_HIGHLIGHT;
	botSelectInfo.picbuttons[i].generic.flags &= ~QMF_PULSEIFFOCUS;
	botSelectInfo.selectedmodel = botSelectInfo.modelpage * MAX_MODELSPERPAGE + i;

	strcpy( botSelectInfo.chosenname.string, botSelectInfo.picnames[i].string);

}


/*
=================
UI_BotSelectMenu_BackEvent
=================
*/
static void UI_BotSelectMenu_BackEvent( void* ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}
	UI_PopMenu();
}

/*
=================
UI_BotSelectMenu_MainMenuEvent
=================
*/
static void UI_BotSelectMenu_MainMenuEvent( void* ptr, int event ) 
{
	if( event != QM_ACTIVATED ) 
	{
		return;
	}

	UI_MainMenu();
}

/*
=================
UI_BotSelectMenu_SelectEvent
=================
*/
static void UI_BotSelectMenu_SelectEvent( void* ptr, int event ) 
{
	if( event != QM_ACTIVATED ) 
	{
		return;
	}
	UI_PopMenu();

	s_serveroptions.newBot = qtrue;
	Q_strncpyz( s_serveroptions.newBotName, botSelectInfo.botnames[botSelectInfo.selectedmodel % MAX_MODELSPERPAGE], 16 );
}


/*
=================
M_BotSelectMenu_Graphics
=================
*/
void M_BotSelectMenu_Graphics (void)
{
	UI_MenuFrame2(&botSelectInfo.menu);

	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30,203, 47, 186, uis.whiteShader);	// Middle left line

	// Frame
	trap_R_SetColor( colorTable[CT_DKPURPLE2]);

	UI_DrawHandlePic( 141, 345,  32,  32, botSelectInfo.corner_ll_18_18);	// LL 
	UI_DrawHandlePic( 530, 345, -32,  32, botSelectInfo.corner_ll_18_18);	// LR 
	UI_DrawHandlePic( 141,  49,  32, -32, botSelectInfo.corner_ll_18_18);	// UL 
	UI_DrawHandlePic( 530,  49, -32, -32, botSelectInfo.corner_ll_18_18);	// UR 

	UI_DrawHandlePic( 158,  59, 387,  18, uis.whiteShader);	// Top
	UI_DrawHandlePic( 141,  78,  18, 273, uis.whiteShader);	// Left side
	UI_DrawHandlePic( 544,  78,  18, 273, uis.whiteShader);	// Right side
	UI_DrawHandlePic( 163, 349,   8,  18, uis.whiteShader);	// Left Bottom
	UI_DrawHandlePic( 174, 349,  24,  18, uis.whiteShader);	// Left Bottom Arrow
	UI_DrawHandlePic( 201, 349, 300,  18, uis.whiteShader);	// Bottom
	UI_DrawHandlePic( 504, 349,  24,  18, uis.whiteShader);	// Right Bottom Arrow
	UI_DrawHandlePic( 531, 349,  13,  18, uis.whiteShader);	// Right Bottom
	UI_DrawProportionalString( 177,  60, menu_normal_text[MNT_HOLOGRAPHIC_CHARS],UI_SMALLFONT,colorTable[CT_BLACK]);	// Top

	// Left rounded ends for buttons
	trap_R_SetColor( colorTable[botSelectInfo.mainmenu.color]);
	UI_DrawHandlePic(botSelectInfo.mainmenu.generic.x - 14, botSelectInfo.mainmenu.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor( colorTable[botSelectInfo.back.color]);
	UI_DrawHandlePic(botSelectInfo.back.generic.x - 14, botSelectInfo.back.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	UI_DrawProportionalString(  74,   28, "79088",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  150, "4545",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  206, "3624",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "34",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

}


/*
=================
BotSelect_MenuDraw
=================
*/
static void BotSelect_MenuDraw (void)
{
	M_BotSelectMenu_Graphics();

	Menu_Draw( &botSelectInfo.menu );
}

/*
=================
UI_BotSelectMenu_Cache
=================
*/
void UI_BotSelectMenu_Cache( void ) 
{
	trap_R_RegisterShaderNoMip( BOTSELECT_SELECT );
	trap_R_RegisterShaderNoMip( BOTSELECT_SELECTED );

	botSelectInfo.corner_ll_18_18 = trap_R_RegisterShaderNoMip("menu/common/corner_ll_18_18.tga");
}


static void UI_BotSelectMenu_Init( char *bot ) 
{
	int		i, j, k;
	int		x, y,len;
	static char chosennamebuffer[64];

	memset( &botSelectInfo, 0 ,sizeof(botSelectInfo) );
	botSelectInfo.menu.nitems = 0;
	botSelectInfo.menu.wrapAround = qtrue;
	botSelectInfo.menu.fullscreen = qtrue;
	botSelectInfo.menu.draw							= BotSelect_MenuDraw;
	botSelectInfo.menu.fullscreen					= qtrue;
	botSelectInfo.menu.descX						= MENU_DESC_X;
	botSelectInfo.menu.descY						= MENU_DESC_Y;
	botSelectInfo.menu.listX						= 230;
	botSelectInfo.menu.listY						= 188;
	botSelectInfo.menu.titleX						= MENU_TITLE_X;
	botSelectInfo.menu.titleY						= MENU_TITLE_Y;
	botSelectInfo.menu.titleI						= MNT_CONTROLSMENU_TITLE;
	botSelectInfo.menu.footNoteEnum					= MNT_HCSELECT;

	UI_BotSelectMenu_Cache();

	y =	84;
	for( i = 0, k = 0; i < PLAYERGRID_ROWS; i++) 
	{
		x =	180;
		for( j = 0; j < PLAYERGRID_COLS; j++, k++ ) 
		{
			botSelectInfo.pics[k].generic.type				= MTYPE_BITMAP;
			botSelectInfo.pics[k].generic.flags				= QMF_LEFT_JUSTIFY|QMF_INACTIVE;
			botSelectInfo.pics[k].generic.x					= x;
			botSelectInfo.pics[k].generic.y					= y;
 			botSelectInfo.pics[k].generic.name				= botSelectInfo.boticons[k];
			botSelectInfo.pics[k].width						= 64;
			botSelectInfo.pics[k].height					= 64;
			botSelectInfo.pics[k].focuspic					= BOTSELECT_SELECTED;
			botSelectInfo.pics[k].focuscolor				= colorTable[CT_WHITE];

			botSelectInfo.picbuttons[k].generic.type		= MTYPE_BITMAP;
			botSelectInfo.picbuttons[k].generic.flags		= QMF_LEFT_JUSTIFY|QMF_NODEFAULTINIT|QMF_PULSEIFFOCUS;
			botSelectInfo.picbuttons[k].generic.callback	= UI_BotSelectMenu_BotEvent;
			botSelectInfo.picbuttons[k].generic.id			= k;
			botSelectInfo.picbuttons[k].generic.x			= x - 16;
			botSelectInfo.picbuttons[k].generic.y			= y - 16;
			botSelectInfo.picbuttons[k].generic.left		= x;
			botSelectInfo.picbuttons[k].generic.top			= y;
			botSelectInfo.picbuttons[k].generic.right		= x + 64;
			botSelectInfo.picbuttons[k].generic.bottom		= y + 64;
			botSelectInfo.picbuttons[k].width				= 128;
			botSelectInfo.picbuttons[k].height				= 128;
			botSelectInfo.picbuttons[k].focuspic			= BOTSELECT_SELECT;
			botSelectInfo.picbuttons[k].focuscolor			= colorTable[CT_WHITE];

			botSelectInfo.picnames[k].generic.type			= MTYPE_TEXT;
			botSelectInfo.picnames[k].generic.flags			= QMF_SMALLFONT;
			botSelectInfo.picnames[k].generic.x				= x + 32;
			botSelectInfo.picnames[k].generic.y				= y + 64;
			botSelectInfo.picnames[k].string				= botSelectInfo.botnames[k];
			botSelectInfo.picnames[k].color					= colorTable[CT_DKGOLD1];
			botSelectInfo.picnames[k].style					= UI_CENTER;

			len = UI_ProportionalStringWidth(botSelectInfo.picnames[k].string,UI_SMALLFONT);
			if (len > 70)
			{
				botSelectInfo.picnames[k].style		|= UI_TINYFONT;
				botSelectInfo.picnames[k].generic.y +=5;
			}
			else
			{
				botSelectInfo.picnames[k].style		|= UI_SMALLFONT;
			}

			x += (64 + 6);
		}
		y += (64 + SMALLCHAR_HEIGHT + 6);
	}

	botSelectInfo.left.generic.type			= MTYPE_BITMAP;
	botSelectInfo.left.generic.name			= "menu/common/arrow_left_16.tga";
	botSelectInfo.left.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	botSelectInfo.left.generic.callback		= UI_BotSelectMenu_LeftEvent;
	botSelectInfo.left.generic.x			= 178;
	botSelectInfo.left.generic.y			= 350;
	botSelectInfo.left.width  				= 16;
	botSelectInfo.left.height  				= 16;
	botSelectInfo.left.color				= CT_DKGOLD1;
	botSelectInfo.left.color2				= CT_LTGOLD1;
	botSelectInfo.left.textX				= MENU_BUTTON_TEXT_X;
	botSelectInfo.left.textY				= MENU_BUTTON_TEXT_Y;
	botSelectInfo.left.textEnum				= MBT_PREVHC;
	botSelectInfo.left.textcolor			= CT_BLACK;
	botSelectInfo.left.textcolor2			= CT_WHITE;

	botSelectInfo.right.generic.type	    = MTYPE_BITMAP;
	botSelectInfo.right.generic.name		= "menu/common/arrow_right_16.tga";
	botSelectInfo.right.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	botSelectInfo.right.generic.callback	= UI_BotSelectMenu_RightEvent;
	botSelectInfo.right.generic.x			= 508;
	botSelectInfo.right.generic.y			= 350;
	botSelectInfo.right.width  				= 16;
	botSelectInfo.right.height  		    = 16;
	botSelectInfo.right.color				= CT_DKGOLD1;
	botSelectInfo.right.color2				= CT_LTGOLD1;
	botSelectInfo.right.textX				= MENU_BUTTON_TEXT_X;
	botSelectInfo.right.textY				= MENU_BUTTON_TEXT_Y;
	botSelectInfo.right.textEnum			= MBT_NEXTHC;
	botSelectInfo.right.textcolor			= CT_BLACK;
	botSelectInfo.right.textcolor2			= CT_WHITE;

	botSelectInfo.mainmenu.generic.type			= MTYPE_BITMAP;      
	botSelectInfo.mainmenu.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	botSelectInfo.mainmenu.generic.x				= 110;
	botSelectInfo.mainmenu.generic.y				= 391;
	botSelectInfo.mainmenu.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	botSelectInfo.mainmenu.generic.id				= ID_MAINMENU;
	botSelectInfo.mainmenu.generic.callback		= UI_BotSelectMenu_MainMenuEvent;
	botSelectInfo.mainmenu.width					= MENU_BUTTON_MED_WIDTH;
	botSelectInfo.mainmenu.height					= MENU_BUTTON_MED_HEIGHT;
	botSelectInfo.mainmenu.color					= CT_DKPURPLE1;
	botSelectInfo.mainmenu.color2					= CT_LTPURPLE1;
	botSelectInfo.mainmenu.textX					= 5;
	botSelectInfo.mainmenu.textY					= 2;
	botSelectInfo.mainmenu.textEnum				= MBT_MAINMENU;
	botSelectInfo.mainmenu.textcolor				= CT_BLACK;
	botSelectInfo.mainmenu.textcolor2				= CT_WHITE;

	botSelectInfo.back.generic.type		= MTYPE_BITMAP;
	botSelectInfo.back.generic.name		= BUTTON_GRAPHIC_LONGRIGHT;
	botSelectInfo.back.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
	botSelectInfo.back.generic.callback	= UI_BotSelectMenu_BackEvent;
	botSelectInfo.back.generic.x		= 110;
	botSelectInfo.back.generic.y		= 415;
	botSelectInfo.back.width			= MENU_BUTTON_MED_WIDTH;
	botSelectInfo.back.height			= MENU_BUTTON_MED_HEIGHT;
	botSelectInfo.back.color			= CT_DKPURPLE1;
	botSelectInfo.back.color2			= CT_LTPURPLE1;
	botSelectInfo.back.textX			= MENU_BUTTON_TEXT_X;
	botSelectInfo.back.textY			= MENU_BUTTON_TEXT_Y;
	botSelectInfo.back.textEnum			= MBT_BACK;
	botSelectInfo.back.textcolor		= CT_BLACK;
	botSelectInfo.back.textcolor2		= CT_WHITE;


	botSelectInfo.go.generic.type		= MTYPE_BITMAP;
	botSelectInfo.go.generic.name		= "menu/common/square.tga";
	botSelectInfo.go.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	botSelectInfo.go.generic.callback	= UI_BotSelectMenu_SelectEvent;
	botSelectInfo.go.generic.x			= 462;
	botSelectInfo.go.generic.y			= 391;
	botSelectInfo.go.width				= 150;
	botSelectInfo.go.height				= 42;
	botSelectInfo.go.color				= CT_DKPURPLE1;
	botSelectInfo.go.color2				= CT_LTPURPLE1;
	botSelectInfo.go.textX				= MENU_BUTTON_TEXT_X;
	botSelectInfo.go.textY				= MENU_BUTTON_TEXT_Y;
	botSelectInfo.go.textEnum			= MBT_ACCEPT;
	botSelectInfo.go.textcolor			= CT_BLACK;
	botSelectInfo.go.textcolor2			= CT_WHITE;

	botSelectInfo.chosenname.generic.type  = MTYPE_PTEXT;
	botSelectInfo.chosenname.generic.flags = QMF_INACTIVE;
	botSelectInfo.chosenname.generic.x	    = 212;
	botSelectInfo.chosenname.generic.y	    = 351;
	botSelectInfo.chosenname.string        = chosennamebuffer;
	botSelectInfo.chosenname.style         = UI_SMALLFONT;
	botSelectInfo.chosenname.color         = colorTable[CT_LTBLUE2];

	for( i = 0; i < MAX_MODELSPERPAGE; i++ ) 
	{
		Menu_AddItem( &botSelectInfo.menu,	&botSelectInfo.pics[i] );
		Menu_AddItem( &botSelectInfo.menu,	&botSelectInfo.picbuttons[i] );
		Menu_AddItem( &botSelectInfo.menu,	&botSelectInfo.picnames[i] );
	}
	Menu_AddItem( &botSelectInfo.menu, &botSelectInfo.chosenname );
	Menu_AddItem( &botSelectInfo.menu, &botSelectInfo.left );
	Menu_AddItem( &botSelectInfo.menu, &botSelectInfo.right );
	Menu_AddItem( &botSelectInfo.menu, &botSelectInfo.mainmenu );
	Menu_AddItem( &botSelectInfo.menu, &botSelectInfo.back );
	Menu_AddItem( &botSelectInfo.menu, &botSelectInfo.go );

	UI_BotSelectMenu_BuildList();
	UI_BotSelectMenu_Default( bot );
	botSelectInfo.modelpage = botSelectInfo.selectedmodel / MAX_MODELSPERPAGE;
	UI_BotSelectMenu_UpdateGrid();
}


/*
=================
UI_BotSelectMenu
=================
*/
void UI_BotSelectMenu( char *bot ) 
{
	UI_BotSelectMenu_Init( bot );
	UI_PushMenu( &botSelectInfo.menu );
}


/*
=================
SetPlayerMod
=================
*/
void SetPlayerMod(void)
{
	trap_Cvar_SetValue( "g_pModAssimilation", s_startserver.assimilation.curvalue);
	trap_Cvar_SetValue( "g_pModDisintegration", s_startserver.disintegration.curvalue);
	trap_Cvar_SetValue( "g_pModActionHero", s_startserver.actionhero.curvalue);
	trap_Cvar_SetValue( "g_pModSpecialties", s_startserver.specialties.curvalue);
	trap_Cvar_SetValue( "g_pModElimination", s_startserver.elimination.curvalue);
}

/*
=================
AdvancedServer_Update
=================
*/
static void AdvancedServer_Update( void) 
{
	int holdInt;
	float holdFloat;

	// Set colors to normal
	s_advancedserver.repawntime.field.titlecolor				= CT_DKGOLD1;
	s_advancedserver.maxclients.field.titlecolor				= CT_DKGOLD1;
	s_advancedserver.runspeed.field.titlecolor					= CT_DKGOLD1;
	s_advancedserver.gravity.field.titlecolor					= CT_DKGOLD1;
	s_advancedserver.knockback.field.titlecolor					= CT_DKGOLD1;
	s_advancedserver.dmgmult.field.titlecolor					= CT_DKGOLD1;
	s_advancedserver.bot_minplayers.field.titlecolor			= CT_DKGOLD1;
	s_advancedserver.forceplayerrespawn.field.titlecolor		= CT_DKGOLD1;
	s_advancedserver.respawninvulnerability.field.titlecolor	= CT_DKGOLD1;
	s_advancedserver.dowarmup.field.titlecolor					= CT_DKGOLD1;
	s_advancedserver.classchangetimeout.field.titlecolor		= CT_DKGOLD1;



	holdInt	 = atoi( s_advancedserver.repawntime.field.buffer );
	if ((holdInt < 1) || (holdInt > 60))
	{
		s_advancedserver.errorFlag = ERR_RESPAWNTIME;
		s_advancedserver.repawntime.field.titlecolor	= CT_LTRED1;
		Menu_SetCursorToItem(&s_advancedserver.menu,&s_advancedserver.repawntime);
		return;
	}
	trap_Cvar_SetValue( "g_weaponrespawn", holdInt );


	holdInt	 = atoi( s_advancedserver.maxclients.field.buffer );
	if ((holdInt < 0) || (holdInt > 64))
	{
		s_advancedserver.errorFlag = ERR_MAXCLIENTS;
		s_advancedserver.maxclients.field.titlecolor	= CT_LTRED1;
		Menu_SetCursorToItem(&s_advancedserver.menu,&s_advancedserver.maxclients);
		return;
	}
	trap_Cvar_SetValue( "sv_maxclients", holdInt );


	holdInt	 = atoi( s_advancedserver.runspeed.field.buffer );
	if ((holdInt < 50) || (holdInt > 1000))
	{
		s_advancedserver.errorFlag = ERR_RUNSPEED;
		s_advancedserver.runspeed.field.titlecolor	= CT_LTRED1;
		Menu_SetCursorToItem(&s_advancedserver.menu,&s_advancedserver.runspeed);
		return;
	}
	trap_Cvar_SetValue( "g_speed", holdInt );


	holdInt	 = atoi( s_advancedserver.gravity.field.buffer );
	if ((holdInt < 0) || (holdInt > 5000))
	{
		s_advancedserver.errorFlag = ERR_GRAVITY;
		s_advancedserver.gravity.field.titlecolor	= CT_LTRED1;
		Menu_SetCursorToItem(&s_advancedserver.menu,&s_advancedserver.gravity);
		return;
	}
	trap_Cvar_SetValue( "g_gravity", holdInt );


	holdInt	 = atoi( s_advancedserver.knockback.field.buffer );
	if ((holdInt < 0) || (holdInt > 10000))
	{
		s_advancedserver.errorFlag = ERR_KNOCKBACK;
		s_advancedserver.knockback.field.titlecolor	= CT_LTRED1;
		Menu_SetCursorToItem(&s_advancedserver.menu,&s_advancedserver.knockback);
		return;
	}
	trap_Cvar_SetValue( "g_knockback", holdInt );


	holdFloat	 = atof( s_advancedserver.dmgmult.field.buffer );
	if ((holdFloat < 0) || (holdFloat > 100))
	{
		s_advancedserver.errorFlag = ERR_DMGMULT;
		s_advancedserver.dmgmult.field.titlecolor	= CT_LTRED1;
		Menu_SetCursorToItem(&s_advancedserver.menu,&s_advancedserver.dmgmult);
		return;
	}
	trap_Cvar_SetValue( "g_dmgmult", holdFloat );


	holdInt	 = atoi( s_advancedserver.bot_minplayers.field.buffer );
	if ((holdInt < 0) || (holdInt > 64))
	{
		s_advancedserver.errorFlag = ERR_BOT_MINPLAYERS;
		s_advancedserver.bot_minplayers.field.titlecolor	= CT_LTRED1;
		Menu_SetCursorToItem(&s_advancedserver.menu,&s_advancedserver.bot_minplayers);
		return;
	}
	trap_Cvar_SetValue( "bot_minplayers", holdInt );


	// Range check on forceplayerrespawn
	holdInt	 = atoi( s_advancedserver.forceplayerrespawn.field.buffer );
	if ((holdInt < 0) || (holdInt > 120))
	{
		s_advancedserver.errorFlag = ERR_FORCEPLAYERRESPAWN;
		s_advancedserver.forceplayerrespawn.field.titlecolor	= CT_LTRED1;
		Menu_SetCursorToItem(&s_advancedserver.menu,&s_advancedserver.forceplayerrespawn);
		return;
	}
	trap_Cvar_SetValue( "g_forcerespawn", holdInt );


	// Range check on respawnGhostTime
	holdInt	 = atoi( s_advancedserver.respawninvulnerability.field.buffer );
	if ((holdInt < 0) || (holdInt > 30))
	{
		s_advancedserver.errorFlag = ERR_RESPAWNINVULNERABILITY;
		s_advancedserver.respawninvulnerability.field.titlecolor	= CT_LTRED1;
		Menu_SetCursorToItem(&s_advancedserver.menu,&s_advancedserver.respawninvulnerability);
		return;
	}
	trap_Cvar_SetValue( "g_ghostRespawn", holdInt );

	// Range check on dowarmup
	holdInt	 = atoi( s_advancedserver.dowarmup.field.buffer );
	if ((holdInt < 0) || (holdInt > 120))
	{
		s_advancedserver.errorFlag = ERR_DOWARMUP;
		s_advancedserver.dowarmup.field.titlecolor	= CT_LTRED1;
		Menu_SetCursorToItem(&s_advancedserver.menu,&s_advancedserver.dowarmup);
		return;
	}

	if (holdInt == 0)
	{
		trap_Cvar_SetValue( "g_dowarmup", holdInt );
	}
	else
	{
		trap_Cvar_SetValue( "g_dowarmup", 1 );
		trap_Cvar_SetValue( "g_warmup", holdInt );
	}

	s_advancedserver.errorFlag = 0;

	holdInt	 = atoi( s_advancedserver.classchangetimeout.field.buffer );
	if (holdInt < 0)
	{
		s_advancedserver.errorFlag = ERR_CLASSCHANGETIMEOUT;
		s_advancedserver.classchangetimeout.field.titlecolor	= CT_LTRED1;
		Menu_SetCursorToItem(&s_advancedserver.menu,&s_advancedserver.classchangetimeout);
		return;
	}
	trap_Cvar_SetValue( "g_classChangeDebounceTime", holdInt );

	ServerOptions_MenuInit2(s_advancedserver.specialties.curvalue);

}


// Calls for the AdvancedServer screen 
void AdvancedServerSetModButtons(void)
{
		ServerGameMods(gametype_remap[s_startserver.gametype.curvalue],
			&s_advancedserver.assimilation.curvalue,
			&s_advancedserver.specialties.curvalue,
			&s_advancedserver.disintegration.curvalue,
			&s_advancedserver.elimination.curvalue,
			&s_advancedserver.actionhero.curvalue);

		SetServerButtons(gametype_remap[s_startserver.gametype.curvalue],
			&s_advancedserver.assimilation,
			&s_advancedserver.specialties,
			&s_advancedserver.disintegration,
			&s_advancedserver.elimination,
			&s_advancedserver.actionhero);
}

/*
=================
AdvancedServer_Event
=================
*/
static void AdvancedServer_Event( void* ptr, int notification ) 
{
	int dmflags;

	if( notification != QM_ACTIVATED ) 
	{
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) 
	{
	case ID_PMOD_ASSIMILIATION:
		trap_Cvar_SetValue( "g_pModAssimilation", s_advancedserver.assimilation.curvalue);
		AdvancedServerSetModButtons();
		break;
	case ID_PMOD_DISINTEGRATION:
		trap_Cvar_SetValue( "g_pModDisintegration", s_advancedserver.disintegration.curvalue);
		AdvancedServerSetModButtons();
		break;
	case ID_PMOD_ACTIONHERO:
		trap_Cvar_SetValue( "g_pModActionHero", s_advancedserver.actionhero.curvalue);
		AdvancedServerSetModButtons();
		break;
	case ID_PMOD_SPECIALTIES:
		trap_Cvar_SetValue( "g_pModSpecialties", s_advancedserver.specialties.curvalue);
		AdvancedServerSetModButtons();
		break;
	case ID_PMOD_ELIMINATION:
		trap_Cvar_SetValue( "g_pModElimination", s_advancedserver.elimination.curvalue);
		AdvancedServerSetModButtons();
		break;
	case ID_BLUETEAM:
		if (!Q_stricmp(s_skinsForRace[s_advancedserver.blueteam.curvalue],"NONE"))
		{
			trap_Cvar_Set( "g_team_group_blue", "" );
		}
		else
		{
			trap_Cvar_Set( "g_team_group_blue", s_skinsForRace[s_advancedserver.blueteam.curvalue] );
		}
		break;
	case ID_REDTEAM:
		if (!Q_stricmp(s_skinsForRace[s_advancedserver.redteam.curvalue],"NONE"))
		{
			trap_Cvar_Set( "g_team_group_red", "" );
		}
		else
		{
			trap_Cvar_Set( "g_team_group_red", s_skinsForRace[s_advancedserver.redteam.curvalue] );
		}

		break;
	case ID_AUTOJOIN:
		trap_Cvar_SetValue( "g_teamAutoJoin", s_advancedserver.autojoin.curvalue );
		break;
	case ID_AUTOBALANCE:
		trap_Cvar_SetValue( "g_teamForceBalance", s_advancedserver.autobalance.curvalue );
		break;
	case ID_FRIENDLYFIRE:
//		trap_Cvar_SetValue( "g_friendlyFire", s_advancedserver.friendlyfire.curvalue );
		break;
	case ID_FALLINGDAMAGE:
		dmflags	= trap_Cvar_VariableValue( "g_dmflags" );

		if (s_advancedserver.fallingdamage.curvalue)
		{
			dmflags &= ~DF_NO_FALLING;
		}
		else
		{
			dmflags |= DF_NO_FALLING;
		}

		trap_Cvar_SetValue( "g_dmflags", dmflags );
		break;

	case ID_ADAPTITEMRESPAWN:
		trap_Cvar_SetValue( "g_adaptRespawn", s_advancedserver.adaptitemrespawn.curvalue );
		break;

	case ID_HOLODECKINTRO:
		trap_Cvar_SetValue( "g_holointro", s_advancedserver.holodeckintro.curvalue );
		break;


	case ID_MAINMENU:
		AdvancedServer_Update();
		if (!s_advancedserver.errorFlag)	// No error with data
		{
			StartServerSetModButtons();
			UI_MainMenu();
		}
		break;

	case ID_BACK:
		AdvancedServer_Update();
		if (!s_advancedserver.errorFlag)	// No error with data
		{
			StartServerSetModButtons();
			UI_PopMenu();
		}
		break;
	}
}

#define ERROR_X_OFFSET	80
/*
=================
M_AdvancedServerMenu_Graphics
=================
*/
void M_AdvancedServerMenu_Graphics(void)
{
	UI_MenuFrame2(&s_advancedserver.menu);

	trap_R_SetColor( colorTable[CT_LTORANGE]);
	UI_DrawHandlePic( 30,  203,  47,  186, uis.whiteShader);

	switch(s_advancedserver.errorFlag)
	{
	case ERR_RESPAWNTIME :
		s_advancedserver.errorText.generic.flags &= ~QMF_HIDDEN;
		s_advancedserver.errorText.generic.y = s_advancedserver.repawntime.generic.y;
		s_advancedserver.errorText.generic.x = s_advancedserver.repawntime.generic.x + ERROR_X_OFFSET;
		break;
	case ERR_MAXCLIENTS :
		s_advancedserver.errorText.generic.flags &= ~QMF_HIDDEN;
		s_advancedserver.errorText.generic.y = s_advancedserver.maxclients.generic.y;
		s_advancedserver.errorText.generic.x = s_advancedserver.maxclients.generic.x + ERROR_X_OFFSET;
		break;
	case ERR_RUNSPEED :
		s_advancedserver.errorText.generic.flags &= ~QMF_HIDDEN;
		s_advancedserver.errorText.generic.y = s_advancedserver.runspeed.generic.y;
		s_advancedserver.errorText.generic.x = s_advancedserver.runspeed.generic.x + ERROR_X_OFFSET;
		break;
	case ERR_GRAVITY :
		s_advancedserver.errorText.generic.flags &= ~QMF_HIDDEN;
		s_advancedserver.errorText.generic.y = s_advancedserver.gravity.generic.y;
		s_advancedserver.errorText.generic.x = s_advancedserver.gravity.generic.x + ERROR_X_OFFSET;
		break;
	case ERR_KNOCKBACK :
		s_advancedserver.errorText.generic.flags &= ~QMF_HIDDEN;
		s_advancedserver.errorText.generic.y = s_advancedserver.knockback.generic.y;
		s_advancedserver.errorText.generic.x = s_advancedserver.knockback.generic.x + ERROR_X_OFFSET;
		break;
	case ERR_DMGMULT :
		s_advancedserver.errorText.generic.flags &= ~QMF_HIDDEN;
		s_advancedserver.errorText.generic.y = s_advancedserver.dmgmult.generic.y;
		s_advancedserver.errorText.generic.x = s_advancedserver.dmgmult.generic.x + ERROR_X_OFFSET;
		break;
	case ERR_BOT_MINPLAYERS :
		s_advancedserver.errorText.generic.flags &= ~QMF_HIDDEN;
		s_advancedserver.errorText.generic.y = s_advancedserver.bot_minplayers.generic.y;
		s_advancedserver.errorText.generic.x = s_advancedserver.bot_minplayers.generic.x + ERROR_X_OFFSET;
		break;
	case ERR_FORCEPLAYERRESPAWN :
		s_advancedserver.errorText.generic.flags &= ~QMF_HIDDEN;
		s_advancedserver.errorText.generic.y = s_advancedserver.forceplayerrespawn.generic.y;
		s_advancedserver.errorText.generic.x = s_advancedserver.forceplayerrespawn.generic.x + ERROR_X_OFFSET;
		break;
	case ERR_RESPAWNINVULNERABILITY :
		s_advancedserver.errorText.generic.flags &= ~QMF_HIDDEN;
		s_advancedserver.errorText.generic.y = s_advancedserver.respawninvulnerability.generic.y;
		s_advancedserver.errorText.generic.x = s_advancedserver.respawninvulnerability.generic.x + ERROR_X_OFFSET;
		break;
	case ERR_DOWARMUP :
		s_advancedserver.errorText.generic.flags &= ~QMF_HIDDEN;
		s_advancedserver.errorText.generic.y = s_advancedserver.dowarmup.generic.y;
		s_advancedserver.errorText.generic.x = s_advancedserver.dowarmup.generic.x + ERROR_X_OFFSET;
		break;
	case ERR_CLASSCHANGETIMEOUT :
		s_advancedserver.errorText.generic.flags &= ~QMF_HIDDEN;
		s_advancedserver.errorText.generic.y = s_advancedserver.classchangetimeout.generic.y;
		s_advancedserver.errorText.generic.x = s_advancedserver.classchangetimeout.generic.x + ERROR_X_OFFSET;
		break;
	}

	// Left rounded ends for buttons
	trap_R_SetColor( colorTable[s_advancedserver.mainmenu.color]);
	UI_DrawHandlePic(s_advancedserver.mainmenu.generic.x - 14, s_advancedserver.mainmenu.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor( colorTable[s_advancedserver.back.color]);
	UI_DrawHandlePic(s_advancedserver.back.generic.x - 14, s_advancedserver.back.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

}

/*
=================
ServerAdvanced_StatusBar
=================
*/
static void ServerAdvanced_StatusBar( void* ptr ) 
{
	int x,y;

	x = 265;
	y = 400;
	switch( ((menucommon_s*)ptr)->id ) 
	{
	case ID_RESPAWNTIME:
		UI_DrawProportionalString( x, y, menu_normal_text[MNT_ADV_STATUS1_RESPAWN], UI_SMALLFONT, colorTable[CT_WHITE]);
		UI_DrawProportionalString( x, (y+ PROP_HEIGHT + 2), menu_normal_text[MNT_ADV_STATUS2_RESPAWN], UI_SMALLFONT, colorTable[CT_WHITE]);
		break;
	case ID_ADVMAXCLIENTS:
		UI_DrawProportionalString( x, y, menu_normal_text[MNT_ADV_STATUS1_MAXCLIENTS], UI_SMALLFONT, colorTable[CT_WHITE]);
		UI_DrawProportionalString( x, (y+ PROP_HEIGHT + 2), menu_normal_text[MNT_ADV_STATUS2_MAXCLIENTS], UI_SMALLFONT, colorTable[CT_WHITE]);
		break;
	case ID_RUNSPEED:
		UI_DrawProportionalString( x, y, menu_normal_text[MNT_ADV_STATUS1_RUNSPEED], UI_SMALLFONT, colorTable[CT_WHITE]);
		UI_DrawProportionalString( x, (y+ PROP_HEIGHT + 2), menu_normal_text[MNT_ADV_STATUS2_RUNSPEED], UI_SMALLFONT, colorTable[CT_WHITE]);
		break;
	case ID_GRAVITY:
		UI_DrawProportionalString( x, y, menu_normal_text[MNT_ADV_STATUS1_GRAVITY], UI_SMALLFONT, colorTable[CT_WHITE]);
		UI_DrawProportionalString( x, (y+ PROP_HEIGHT + 2), menu_normal_text[MNT_ADV_STATUS2_GRAVITY], UI_SMALLFONT, colorTable[CT_WHITE]);
		break;
	case ID_KNOCKBACK:
		UI_DrawProportionalString( x, y, menu_normal_text[MNT_ADV_STATUS1_KNOCKBACK], UI_SMALLFONT, colorTable[CT_WHITE]);
		UI_DrawProportionalString( x, (y+ PROP_HEIGHT + 2), menu_normal_text[MNT_ADV_STATUS2_KNOCKBACK], UI_SMALLFONT, colorTable[CT_WHITE]);
		break;
	case ID_DMGMULT:
		UI_DrawProportionalString( x, y, menu_normal_text[MNT_ADV_STATUS1_DMGMULT], UI_SMALLFONT, colorTable[CT_WHITE]);
		UI_DrawProportionalString( x, (y+ PROP_HEIGHT + 2), menu_normal_text[MNT_ADV_STATUS2_DMGMULT], UI_SMALLFONT, colorTable[CT_WHITE]);
		break;
	case ID_BOT_MINPLAYERS:
		UI_DrawProportionalString( x, y, menu_normal_text[MNT_ADV_STATUS1_BOT_MINPLAYERS], UI_SMALLFONT, colorTable[CT_WHITE]);
		UI_DrawProportionalString( x, (y+ PROP_HEIGHT + 2), menu_normal_text[MNT_ADV_STATUS2_BOT_MINPLAYERS], UI_SMALLFONT, colorTable[CT_WHITE]);
		break;
	case ID_FORCEPLAYERRESPAWN :
		UI_DrawProportionalString( x, y, menu_normal_text[MNT_ADV_STATUS1_FORCEPLAYERRESPAWN], UI_SMALLFONT, colorTable[CT_WHITE]);
		UI_DrawProportionalString( x, (y+ PROP_HEIGHT + 2), menu_normal_text[MNT_ADV_STATUS2_FORCEPLAYERRESPAWN], UI_SMALLFONT, colorTable[CT_WHITE]);
		break;
	case ID_RESPAWNINVULNERABILITY :
		UI_DrawProportionalString( x, y, menu_normal_text[MNT_ADV_STATUS1_RESPAWNINVULNERABILITY], UI_SMALLFONT, colorTable[CT_WHITE]);
		UI_DrawProportionalString( x, (y+ PROP_HEIGHT + 2), menu_normal_text[MNT_ADV_STATUS2_RESPAWNINVULNERABILITY], UI_SMALLFONT, colorTable[CT_WHITE]);
		break;
	case ID_DOWARMUP :
		UI_DrawProportionalString( x, y, menu_normal_text[MNT_ADV_STATUS1_DOWARMUP], UI_SMALLFONT, colorTable[CT_WHITE]);
		UI_DrawProportionalString( x, (y+ PROP_HEIGHT + 2), menu_normal_text[MNT_ADV_STATUS2_DOWARMUP], UI_SMALLFONT, colorTable[CT_WHITE]);
		break;
	case ID_CLASSCHANGETIMEOUT:
		UI_DrawProportionalString( x, y, menu_normal_text[MNT_ADV_STATUS1_CLASSCHANGETIMEOUT], UI_SMALLFONT, colorTable[CT_WHITE]);
		UI_DrawProportionalString( x, (y+ PROP_HEIGHT + 2), menu_normal_text[MNT_ADV_STATUS2_CLASSCHANGETIMEOUT], UI_SMALLFONT, colorTable[CT_WHITE]);
		break;

	}
}

/*
=================
AdvanceServer_MenuDraw
=================
*/
static void AdvanceServer_MenuDraw (void)
{
	M_AdvancedServerMenu_Graphics();

	Menu_Draw( &s_advancedserver.menu );
}

/*
=================
UI_AdvancedServerMenu_Init
=================
*/
static void UI_AdvancedServerMenu_Init(int fromMenu)
{
	int x,y,pad,dmflags,holdInt;

	memset( &s_advancedserver, 0 ,sizeof(s_advancedserver) );

	s_advancedserver.menu.nitems					= 0;
	s_advancedserver.menu.wrapAround				= qtrue;
	s_advancedserver.menu.fullscreen				= qtrue;
	s_advancedserver.menu.draw						= AdvanceServer_MenuDraw;
	s_advancedserver.menu.fullscreen				= qtrue;
	s_advancedserver.menu.descX						= MENU_DESC_X;
	s_advancedserver.menu.descY						= MENU_DESC_Y;
	s_advancedserver.menu.titleX					= MENU_TITLE_X;
	s_advancedserver.menu.titleY					= MENU_TITLE_Y;
	s_advancedserver.menu.titleI					= MNT_ANVANCEDMENU_TITLE;
	s_advancedserver.menu.footNoteEnum				= MNT_ADVANCEDSERVER;

	s_advancedserver.mainmenu.generic.type			= MTYPE_BITMAP;      
	s_advancedserver.mainmenu.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_advancedserver.mainmenu.generic.x				= 110;
	s_advancedserver.mainmenu.generic.y				= 391;
	s_advancedserver.mainmenu.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	s_advancedserver.mainmenu.generic.id				= ID_MAINMENU;
	s_advancedserver.mainmenu.generic.callback		= AdvancedServer_Event;
	s_advancedserver.mainmenu.width					= MENU_BUTTON_MED_WIDTH;
	s_advancedserver.mainmenu.height					= MENU_BUTTON_MED_HEIGHT;
	s_advancedserver.mainmenu.color					= CT_DKPURPLE1;
	s_advancedserver.mainmenu.color2					= CT_LTPURPLE1;
	s_advancedserver.mainmenu.textX					= MENU_BUTTON_TEXT_X;
	s_advancedserver.mainmenu.textY					= MENU_BUTTON_TEXT_Y;
	s_advancedserver.mainmenu.textEnum				= MBT_MAINMENU;
	s_advancedserver.mainmenu.textcolor				= CT_BLACK;
	s_advancedserver.mainmenu.textcolor2				= CT_WHITE;

	s_advancedserver.back.generic.type				= MTYPE_BITMAP;
	s_advancedserver.back.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_advancedserver.back.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_advancedserver.back.generic.callback			= AdvancedServer_Event;
	s_advancedserver.back.generic.id					= ID_BACK;
	s_advancedserver.back.generic.x					= 110;
	s_advancedserver.back.generic.y					= 415;
	s_advancedserver.back.width  					= MENU_BUTTON_MED_WIDTH;
	s_advancedserver.back.height  					= MENU_BUTTON_MED_HEIGHT;
	s_advancedserver.back.color						= CT_DKPURPLE1;
	s_advancedserver.back.color2						= CT_LTPURPLE1;
	s_advancedserver.back.textX						= 5;
	s_advancedserver.back.textY						= 2;
	s_advancedserver.back.textEnum					= MBT_BACK;
	s_advancedserver.back.textcolor					= CT_BLACK;
	s_advancedserver.back.textcolor2					= CT_WHITE;

	pad = 22;
	x = 100;
	y = 80;
	s_advancedserver.autojoin.generic.type				= MTYPE_SPINCONTROL;      
	s_advancedserver.autojoin.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_advancedserver.autojoin.generic.x					= x;
	s_advancedserver.autojoin.generic.y					= y;
	s_advancedserver.autojoin.generic.name				= "menu/common/bar1.tga";
	s_advancedserver.autojoin.generic.id				= ID_AUTOJOIN; 
	s_advancedserver.autojoin.generic.callback			= AdvancedServer_Event;
	s_advancedserver.autojoin.color						= CT_DKPURPLE1;
	s_advancedserver.autojoin.color2					= CT_LTPURPLE1;
	s_advancedserver.autojoin.textX						= MENU_BUTTON_TEXT_X;
	s_advancedserver.autojoin.textY						= MENU_BUTTON_TEXT_Y;
	s_advancedserver.autojoin.textEnum					= MBT_AUTOJOIN;
	s_advancedserver.autojoin.textcolor					= CT_BLACK;
	s_advancedserver.autojoin.textcolor2				= CT_WHITE;	
	s_advancedserver.autojoin.listnames					= s_OffOnNone_Names;

	y += pad;
	s_advancedserver.autobalance.generic.type				= MTYPE_SPINCONTROL;      
	s_advancedserver.autobalance.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_advancedserver.autobalance.generic.x					= x;
	s_advancedserver.autobalance.generic.y					= y;
	s_advancedserver.autobalance.generic.name				= "menu/common/bar1.tga";
	s_advancedserver.autobalance.generic.id					= ID_AUTOBALANCE; 
	s_advancedserver.autobalance.generic.callback			= AdvancedServer_Event;
	s_advancedserver.autobalance.color						= CT_DKPURPLE1;
	s_advancedserver.autobalance.color2					= CT_LTPURPLE1;
	s_advancedserver.autobalance.textX						= MENU_BUTTON_TEXT_X;
	s_advancedserver.autobalance.textY						= MENU_BUTTON_TEXT_Y;
	s_advancedserver.autobalance.textEnum					= MBT_AUTOBALANCE;
	s_advancedserver.autobalance.textcolor					= CT_BLACK;
	s_advancedserver.autobalance.textcolor2				= CT_WHITE;	
	s_advancedserver.autobalance.listnames					= s_OffOnNone_Names;

	y += pad;
	/*
	s_advancedserver.friendlyfire.generic.type				= MTYPE_SPINCONTROL;      
	s_advancedserver.friendlyfire.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_advancedserver.friendlyfire.generic.x					= x;
	s_advancedserver.friendlyfire.generic.y					= y;
	s_advancedserver.friendlyfire.generic.name				= "menu/common/bar1.tga";
	s_advancedserver.friendlyfire.generic.id				= ID_FRIENDLYFIRE; 
	s_advancedserver.friendlyfire.generic.callback			= AdvancedServer_Event;
	s_advancedserver.friendlyfire.color						= CT_DKPURPLE1;
	s_advancedserver.friendlyfire.color2					= CT_LTPURPLE1;
	s_advancedserver.friendlyfire.textX						= MENU_BUTTON_TEXT_X;
	s_advancedserver.friendlyfire.textY						= MENU_BUTTON_TEXT_Y;
	s_advancedserver.friendlyfire.textEnum					= MBT_FRIENDLYFIRE;
	s_advancedserver.friendlyfire.textcolor					= CT_BLACK;
	s_advancedserver.friendlyfire.textcolor2				= CT_WHITE;	
	s_advancedserver.friendlyfire.listnames					= s_OffOnNone_Names;
*/
	y += pad;
	s_advancedserver.fallingdamage.generic.type				= MTYPE_SPINCONTROL;      
	s_advancedserver.fallingdamage.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_advancedserver.fallingdamage.generic.x				= x;
	s_advancedserver.fallingdamage.generic.y				= y;
	s_advancedserver.fallingdamage.generic.name				= "menu/common/bar1.tga";
	s_advancedserver.fallingdamage.generic.id				= ID_FRIENDLYFIRE; 
	s_advancedserver.fallingdamage.generic.callback			= AdvancedServer_Event;
	s_advancedserver.fallingdamage.color					= CT_DKPURPLE1;
	s_advancedserver.fallingdamage.color2					= CT_LTPURPLE1;
	s_advancedserver.fallingdamage.textX					= MENU_BUTTON_TEXT_X;
	s_advancedserver.fallingdamage.textY					= MENU_BUTTON_TEXT_Y;
	s_advancedserver.fallingdamage.textEnum					= MBT_FALLINGDAMAGE;
	s_advancedserver.fallingdamage.textcolor				= CT_BLACK;
	s_advancedserver.fallingdamage.textcolor2				= CT_WHITE;	
	s_advancedserver.fallingdamage.listnames				= s_OffOnNone_Names;


	y += pad;
	x += 160;
	s_advancedserver.repawntime.generic.type				= MTYPE_FIELD;
	s_advancedserver.repawntime.generic.flags				= QMF_NUMBERSONLY|QMF_SMALLFONT;
	s_advancedserver.repawntime.generic.x					= x;
	s_advancedserver.repawntime.generic.y					= y;
	s_advancedserver.repawntime.generic.id					= ID_RESPAWNTIME; 
	s_advancedserver.repawntime.generic.callback			= AdvancedServer_Event;
	s_advancedserver.repawntime.generic.statusbar			= ServerAdvanced_StatusBar;
	s_advancedserver.repawntime.field.widthInChars			= 2;
	s_advancedserver.repawntime.field.maxchars				= 2;
	s_advancedserver.repawntime.field.style					= UI_SMALLFONT;
	s_advancedserver.repawntime.field.titleEnum				= MBT_RESPAWNTIME;
	s_advancedserver.repawntime.field.titlecolor			= CT_LTGOLD1;
	s_advancedserver.repawntime.field.textcolor				= CT_DKGOLD1;
	s_advancedserver.repawntime.field.textcolor2			= CT_LTGOLD1;

	y += pad;
	s_advancedserver.maxclients.generic.type				= MTYPE_FIELD;
	s_advancedserver.maxclients.generic.flags				= QMF_NUMBERSONLY|QMF_SMALLFONT;
	s_advancedserver.maxclients.generic.x					= x;
	s_advancedserver.maxclients.generic.y					= y;
	s_advancedserver.maxclients.generic.id					= ID_ADVMAXCLIENTS; 
	s_advancedserver.maxclients.generic.callback			= AdvancedServer_Event;
	s_advancedserver.maxclients.generic.statusbar			= ServerAdvanced_StatusBar;
	s_advancedserver.maxclients.field.widthInChars			= 2;
	s_advancedserver.maxclients.field.maxchars				= 2;
	s_advancedserver.maxclients.field.style					= UI_SMALLFONT;
	s_advancedserver.maxclients.field.titleEnum				= MBT_MAXCLIENTS;
	s_advancedserver.maxclients.field.titlecolor			= CT_LTGOLD1;
	s_advancedserver.maxclients.field.textcolor				= CT_DKGOLD1;
	s_advancedserver.maxclients.field.textcolor2			= CT_LTGOLD1;

	y += pad;
	s_advancedserver.runspeed.generic.type					= MTYPE_FIELD;
	s_advancedserver.runspeed.generic.flags					= QMF_NUMBERSONLY|QMF_SMALLFONT;
	s_advancedserver.runspeed.generic.x						= x;
	s_advancedserver.runspeed.generic.y						= y;
	s_advancedserver.runspeed.generic.id					= ID_RUNSPEED; 
	s_advancedserver.runspeed.generic.callback				= AdvancedServer_Event;
	s_advancedserver.runspeed.generic.statusbar				= ServerAdvanced_StatusBar;
	s_advancedserver.runspeed.field.widthInChars			= 4;
	s_advancedserver.runspeed.field.maxchars				= 4;
	s_advancedserver.runspeed.field.style					= UI_SMALLFONT;
	s_advancedserver.runspeed.field.titleEnum				= MBT_RUNSPEED;
	s_advancedserver.runspeed.field.titlecolor				= CT_LTGOLD1;
	s_advancedserver.runspeed.field.textcolor				= CT_DKGOLD1;
	s_advancedserver.runspeed.field.textcolor2				= CT_LTGOLD1;

	y += pad;
	s_advancedserver.gravity.generic.type					= MTYPE_FIELD;
	s_advancedserver.gravity.generic.flags					= QMF_NUMBERSONLY|QMF_SMALLFONT;
	s_advancedserver.gravity.generic.x						= x;
	s_advancedserver.gravity.generic.y						= y;
	s_advancedserver.gravity.generic.id						= ID_GRAVITY; 
	s_advancedserver.gravity.generic.callback				= AdvancedServer_Event;
	s_advancedserver.gravity.generic.statusbar				= ServerAdvanced_StatusBar;
	s_advancedserver.gravity.field.widthInChars				= 4;
	s_advancedserver.gravity.field.maxchars					= 4;
	s_advancedserver.gravity.field.style					= UI_SMALLFONT;
	s_advancedserver.gravity.field.titleEnum				= MBT_GRAVITY;
	s_advancedserver.gravity.field.titlecolor				= CT_LTGOLD1;
	s_advancedserver.gravity.field.textcolor				= CT_DKGOLD1;
	s_advancedserver.gravity.field.textcolor2				= CT_LTGOLD1;

	y += pad;
	s_advancedserver.knockback.generic.type					= MTYPE_FIELD;
	s_advancedserver.knockback.generic.flags				= QMF_NUMBERSONLY|QMF_SMALLFONT;
	s_advancedserver.knockback.generic.x					= x;
	s_advancedserver.knockback.generic.y					= y;
	s_advancedserver.knockback.generic.id					= ID_KNOCKBACK; 
	s_advancedserver.knockback.generic.callback				= AdvancedServer_Event;
	s_advancedserver.knockback.generic.statusbar			= ServerAdvanced_StatusBar;
	s_advancedserver.knockback.field.widthInChars			= 5;
	s_advancedserver.knockback.field.maxchars				= 5;
	s_advancedserver.knockback.field.style					= UI_SMALLFONT;
	s_advancedserver.knockback.field.titleEnum				= MBT_KNOCKBACK;
	s_advancedserver.knockback.field.titlecolor				= CT_LTGOLD1;
	s_advancedserver.knockback.field.textcolor				= CT_DKGOLD1;
	s_advancedserver.knockback.field.textcolor2				= CT_LTGOLD1;

	y += pad;
	s_advancedserver.dmgmult.generic.type					= MTYPE_FIELD;
	s_advancedserver.dmgmult.generic.flags					= QMF_NUMBERSONLY|QMF_SMALLFONT;
	s_advancedserver.dmgmult.generic.x						= x;
	s_advancedserver.dmgmult.generic.y						= y;
	s_advancedserver.dmgmult.generic.id						= ID_DMGMULT; 
	s_advancedserver.dmgmult.generic.callback				= AdvancedServer_Event;
	s_advancedserver.dmgmult.generic.statusbar				= ServerAdvanced_StatusBar;
	s_advancedserver.dmgmult.field.widthInChars				= 5;
	s_advancedserver.dmgmult.field.maxchars					= 5;
	s_advancedserver.dmgmult.field.style					= UI_SMALLFONT;
	s_advancedserver.dmgmult.field.titleEnum				= MBT_DMGMULT;
	s_advancedserver.dmgmult.field.titlecolor				= CT_LTGOLD1;
	s_advancedserver.dmgmult.field.textcolor				= CT_DKGOLD1;
	s_advancedserver.dmgmult.field.textcolor2				= CT_LTGOLD1;

	y += pad;
	s_advancedserver.bot_minplayers.generic.type				= MTYPE_FIELD;
	s_advancedserver.bot_minplayers.generic.flags				= QMF_NUMBERSONLY|QMF_SMALLFONT;
	s_advancedserver.bot_minplayers.generic.x					= x;
	s_advancedserver.bot_minplayers.generic.y					= y;
	s_advancedserver.bot_minplayers.generic.id					= ID_BOT_MINPLAYERS; 
	s_advancedserver.bot_minplayers.generic.callback			= AdvancedServer_Event;
	s_advancedserver.bot_minplayers.generic.statusbar			= ServerAdvanced_StatusBar;
	s_advancedserver.bot_minplayers.field.widthInChars			= 5;
	s_advancedserver.bot_minplayers.field.maxchars				= 5;
	s_advancedserver.bot_minplayers.field.style					= UI_SMALLFONT;
	s_advancedserver.bot_minplayers.field.titleEnum				= MBT_BOT_MINPLAYERS;
	s_advancedserver.bot_minplayers.field.titlecolor			= CT_LTGOLD1;
	s_advancedserver.bot_minplayers.field.textcolor				= CT_DKGOLD1;
	s_advancedserver.bot_minplayers.field.textcolor2			= CT_LTGOLD1;

	y += pad;
	s_advancedserver.classchangetimeout.generic.type					= MTYPE_FIELD;
	s_advancedserver.classchangetimeout.generic.flags				= QMF_NUMBERSONLY|QMF_SMALLFONT;
	s_advancedserver.classchangetimeout.generic.x					= x;
	s_advancedserver.classchangetimeout.generic.y					= y;
	s_advancedserver.classchangetimeout.generic.id					= ID_CLASSCHANGETIMEOUT; 
	s_advancedserver.classchangetimeout.generic.callback				= AdvancedServer_Event;
	s_advancedserver.classchangetimeout.generic.statusbar			= ServerAdvanced_StatusBar;
	s_advancedserver.classchangetimeout.field.widthInChars			= 7;
	s_advancedserver.classchangetimeout.field.maxchars				= 7;
	s_advancedserver.classchangetimeout.field.style					= UI_SMALLFONT;
	s_advancedserver.classchangetimeout.field.titleEnum				= MBT_CLASSCHANGE;
	s_advancedserver.classchangetimeout.field.titlecolor				= CT_LTGOLD1;
	s_advancedserver.classchangetimeout.field.textcolor				= CT_DKGOLD1;
	s_advancedserver.classchangetimeout.field.textcolor2				= CT_LTGOLD1;

	// Second column
	x = 370;
	y = 80;
	s_advancedserver.adaptitemrespawn.generic.type				= MTYPE_SPINCONTROL;      
	s_advancedserver.adaptitemrespawn.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_advancedserver.adaptitemrespawn.generic.x					= x;
	s_advancedserver.adaptitemrespawn.generic.y					= y;
	s_advancedserver.adaptitemrespawn.generic.name				= "menu/common/bar1.tga";
	s_advancedserver.adaptitemrespawn.generic.id				= ID_ADAPTITEMRESPAWN; 
	s_advancedserver.adaptitemrespawn.generic.callback			= AdvancedServer_Event;
	s_advancedserver.adaptitemrespawn.color						= CT_DKPURPLE1;
	s_advancedserver.adaptitemrespawn.color2					= CT_LTPURPLE1;
	s_advancedserver.adaptitemrespawn.textX						= MENU_BUTTON_TEXT_X;
	s_advancedserver.adaptitemrespawn.textY						= MENU_BUTTON_TEXT_Y;
	s_advancedserver.adaptitemrespawn.textEnum					= MBT_ADAPTITEMRESPAWN;
	s_advancedserver.adaptitemrespawn.textcolor					= CT_BLACK;
	s_advancedserver.adaptitemrespawn.textcolor2				= CT_WHITE;	
	s_advancedserver.adaptitemrespawn.listnames					= s_OffOnNone_Names;

	y += pad;
	s_advancedserver.holodeckintro.generic.type					= MTYPE_SPINCONTROL;      
	s_advancedserver.holodeckintro.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_advancedserver.holodeckintro.generic.x					= x;
	s_advancedserver.holodeckintro.generic.y					= y;
	s_advancedserver.holodeckintro.generic.name					= "menu/common/bar1.tga";
	s_advancedserver.holodeckintro.generic.id					= ID_HOLODECKINTRO; 
	s_advancedserver.holodeckintro.generic.callback				= AdvancedServer_Event;
	s_advancedserver.holodeckintro.color						= CT_DKPURPLE1;
	s_advancedserver.holodeckintro.color2						= CT_LTPURPLE1;
	s_advancedserver.holodeckintro.textX						= MENU_BUTTON_TEXT_X;
	s_advancedserver.holodeckintro.textY						= MENU_BUTTON_TEXT_Y;
	s_advancedserver.holodeckintro.textEnum						= MBT_HOLODECKINTRO;
	s_advancedserver.holodeckintro.textcolor					= CT_BLACK;
	s_advancedserver.holodeckintro.textcolor2					= CT_WHITE;	
	s_advancedserver.holodeckintro.listnames					= s_OffOnNone_Names;

	y += pad;
	x += 160;
	s_advancedserver.forceplayerrespawn.generic.type				= MTYPE_FIELD;
	s_advancedserver.forceplayerrespawn.generic.flags				= QMF_NUMBERSONLY|QMF_SMALLFONT;
	s_advancedserver.forceplayerrespawn.generic.x					= x;
	s_advancedserver.forceplayerrespawn.generic.y					= y;
	s_advancedserver.forceplayerrespawn.generic.id					= ID_FORCEPLAYERRESPAWN; 
	s_advancedserver.forceplayerrespawn.generic.callback			= AdvancedServer_Event;
	s_advancedserver.forceplayerrespawn.generic.statusbar			= ServerAdvanced_StatusBar;
	s_advancedserver.forceplayerrespawn.field.widthInChars			= 5;
	s_advancedserver.forceplayerrespawn.field.maxchars				= 5;
	s_advancedserver.forceplayerrespawn.field.style					= UI_SMALLFONT;
	s_advancedserver.forceplayerrespawn.field.titleEnum				= MBT_FORCEPLAYERRESPAWN;
	s_advancedserver.forceplayerrespawn.field.titlecolor			= CT_LTGOLD1;
	s_advancedserver.forceplayerrespawn.field.textcolor				= CT_DKGOLD1;
	s_advancedserver.forceplayerrespawn.field.textcolor2			= CT_LTGOLD1;

	y += pad;
	s_advancedserver.respawninvulnerability.generic.type				= MTYPE_FIELD;
	s_advancedserver.respawninvulnerability.generic.flags				= QMF_NUMBERSONLY|QMF_SMALLFONT;
	s_advancedserver.respawninvulnerability.generic.x					= x;
	s_advancedserver.respawninvulnerability.generic.y					= y;
	s_advancedserver.respawninvulnerability.generic.id					= ID_RESPAWNINVULNERABILITY; 
	s_advancedserver.respawninvulnerability.generic.callback			= AdvancedServer_Event;
	s_advancedserver.respawninvulnerability.generic.statusbar			= ServerAdvanced_StatusBar;
	s_advancedserver.respawninvulnerability.field.widthInChars			= 5;
	s_advancedserver.respawninvulnerability.field.maxchars				= 5;
	s_advancedserver.respawninvulnerability.field.style					= UI_SMALLFONT;
	s_advancedserver.respawninvulnerability.field.titleEnum				= MBT_RESPAWNINVULNERABILITY;
	s_advancedserver.respawninvulnerability.field.titlecolor			= CT_LTGOLD1;
	s_advancedserver.respawninvulnerability.field.textcolor				= CT_DKGOLD1;
	s_advancedserver.respawninvulnerability.field.textcolor2			= CT_LTGOLD1;

	y += pad;
	s_advancedserver.dowarmup.generic.type								= MTYPE_FIELD;
	s_advancedserver.dowarmup.generic.flags								= QMF_NUMBERSONLY|QMF_SMALLFONT;
	s_advancedserver.dowarmup.generic.x									= x;
	s_advancedserver.dowarmup.generic.y									= y;
	s_advancedserver.dowarmup.generic.id								= ID_DOWARMUP; 
	s_advancedserver.dowarmup.generic.callback							= AdvancedServer_Event;
	s_advancedserver.dowarmup.generic.statusbar							= ServerAdvanced_StatusBar;
	s_advancedserver.dowarmup.field.widthInChars						= 5;
	s_advancedserver.dowarmup.field.maxchars							= 5;
	s_advancedserver.dowarmup.field.style								= UI_SMALLFONT;
	s_advancedserver.dowarmup.field.titleEnum							= MBT_WARMUP;
	s_advancedserver.dowarmup.field.titlecolor							= CT_LTGOLD1;
	s_advancedserver.dowarmup.field.textcolor							= CT_DKGOLD1;
	s_advancedserver.dowarmup.field.textcolor2							= CT_LTGOLD1;


	y += pad;
	x = 370;
	s_advancedserver.blueteam.generic.type					= MTYPE_SPINCONTROL;      
	s_advancedserver.blueteam.generic.flags					= QMF_HIGHLIGHT_IF_FOCUS;
	s_advancedserver.blueteam.generic.x						= x;
	s_advancedserver.blueteam.generic.y						= y;
	s_advancedserver.blueteam.generic.name					= "menu/common/bar1.tga";
	s_advancedserver.blueteam.generic.id					= ID_BLUETEAM; 
	s_advancedserver.blueteam.generic.callback				= AdvancedServer_Event;
	s_advancedserver.blueteam.color							= CT_DKPURPLE1;
	s_advancedserver.blueteam.color2						= CT_LTPURPLE1;
	s_advancedserver.blueteam.textX							= MENU_BUTTON_TEXT_X;
	s_advancedserver.blueteam.textY							= MENU_BUTTON_TEXT_Y;
	s_advancedserver.blueteam.textEnum						= MBT_BLUETEAMGROUP;
	s_advancedserver.blueteam.textcolor						= CT_BLACK;
	s_advancedserver.blueteam.textcolor2					= CT_WHITE;	
	s_advancedserver.blueteam.itemnames						= s_skinsForRace;

	y += pad;
	s_advancedserver.redteam.generic.type					= MTYPE_SPINCONTROL;      
	s_advancedserver.redteam.generic.flags					= QMF_HIGHLIGHT_IF_FOCUS;
	s_advancedserver.redteam.generic.x						= x;
	s_advancedserver.redteam.generic.y						= y;
	s_advancedserver.redteam.generic.name					= "menu/common/bar1.tga";
	s_advancedserver.redteam.generic.id						= ID_REDTEAM; 
	s_advancedserver.redteam.generic.callback				= AdvancedServer_Event;
	s_advancedserver.redteam.color							= CT_DKPURPLE1;
	s_advancedserver.redteam.color2							= CT_LTPURPLE1;
	s_advancedserver.redteam.textX							= MENU_BUTTON_TEXT_X;
	s_advancedserver.redteam.textY							= MENU_BUTTON_TEXT_Y;
	s_advancedserver.redteam.textEnum						= MBT_REDTEAMGROUP;
	s_advancedserver.redteam.textcolor						= CT_BLACK;
	s_advancedserver.redteam.textcolor2						= CT_WHITE;	
	s_advancedserver.redteam.itemnames						= s_skinsForRace;

	y += 33;
	s_advancedserver.assimilation.generic.type				= MTYPE_SPINCONTROL;      
	s_advancedserver.assimilation.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_advancedserver.assimilation.generic.x					= x;
	s_advancedserver.assimilation.generic.y					= y;
	s_advancedserver.assimilation.generic.name				= "menu/common/bar1.tga";
	s_advancedserver.assimilation.generic.id				= ID_PMOD_ASSIMILIATION; 
	s_advancedserver.assimilation.generic.callback			= AdvancedServer_Event;
	s_advancedserver.assimilation.color						= CT_DKPURPLE1;
	s_advancedserver.assimilation.color2					= CT_LTPURPLE1;
	s_advancedserver.assimilation.textX						= MENU_BUTTON_TEXT_X;
	s_advancedserver.assimilation.textY						= MENU_BUTTON_TEXT_Y;
	s_advancedserver.assimilation.textEnum					= MBT_ASSIMILATION;
	s_advancedserver.assimilation.textcolor					= CT_BLACK;
	s_advancedserver.assimilation.textcolor2				= CT_WHITE;	
	s_advancedserver.assimilation.listnames					= s_OffOnNone_Names;

	y += pad;
	s_advancedserver.specialties.generic.type				= MTYPE_SPINCONTROL;      
	s_advancedserver.specialties.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_advancedserver.specialties.generic.x					= x;
	s_advancedserver.specialties.generic.y					= y;
	s_advancedserver.specialties.generic.name				= "menu/common/bar1.tga";
	s_advancedserver.specialties.generic.id					= ID_PMOD_SPECIALTIES; 
	s_advancedserver.specialties.generic.callback			= AdvancedServer_Event;
	s_advancedserver.specialties.color						= CT_DKPURPLE1;
	s_advancedserver.specialties.color2						= CT_LTPURPLE1;
	s_advancedserver.specialties.textX						= MENU_BUTTON_TEXT_X;
	s_advancedserver.specialties.textY						= MENU_BUTTON_TEXT_Y;
	s_advancedserver.specialties.textEnum					= MBT_SPECIALTIES;
	s_advancedserver.specialties.textcolor					= CT_BLACK;
	s_advancedserver.specialties.textcolor2					= CT_WHITE;	
	s_advancedserver.specialties.listnames					= s_OffOnNone_Names;

	y += pad;
	s_advancedserver.disintegration.generic.type				= MTYPE_SPINCONTROL;      
	s_advancedserver.disintegration.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_advancedserver.disintegration.generic.x					= x;
	s_advancedserver.disintegration.generic.y					= y;
	s_advancedserver.disintegration.generic.name				= "menu/common/bar1.tga";
	s_advancedserver.disintegration.generic.id					= ID_PMOD_DISINTEGRATION; 
	s_advancedserver.disintegration.generic.callback			= AdvancedServer_Event;
	s_advancedserver.disintegration.color						= CT_DKPURPLE1;
	s_advancedserver.disintegration.color2						= CT_LTPURPLE1;
	s_advancedserver.disintegration.textX						= MENU_BUTTON_TEXT_X;
	s_advancedserver.disintegration.textY						= MENU_BUTTON_TEXT_Y;
	s_advancedserver.disintegration.textEnum					= MBT_DISINTEGRATION;
	s_advancedserver.disintegration.textcolor					= CT_BLACK;
	s_advancedserver.disintegration.textcolor2					= CT_WHITE;	
	s_advancedserver.disintegration.listnames					= s_OffOnNone_Names;

	y += pad;
	s_advancedserver.elimination.generic.type				= MTYPE_SPINCONTROL;      
	s_advancedserver.elimination.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_advancedserver.elimination.generic.x					= x;
	s_advancedserver.elimination.generic.y					= y;
	s_advancedserver.elimination.generic.name				= "menu/common/bar1.tga";
	s_advancedserver.elimination.generic.id					= ID_PMOD_ELIMINATION; 
	s_advancedserver.elimination.generic.callback			= AdvancedServer_Event;
	s_advancedserver.elimination.color						= CT_DKPURPLE1;
	s_advancedserver.elimination.color2						= CT_LTPURPLE1;
	s_advancedserver.elimination.textX						= MENU_BUTTON_TEXT_X;
	s_advancedserver.elimination.textY						= MENU_BUTTON_TEXT_Y;
	s_advancedserver.elimination.textEnum					= MBT_ELIMINATION;
	s_advancedserver.elimination.textcolor					= CT_BLACK;
	s_advancedserver.elimination.textcolor2					= CT_WHITE;	
	s_advancedserver.elimination.listnames					= s_OffOnNone_Names;

	y += pad;
	s_advancedserver.actionhero.generic.type				= MTYPE_SPINCONTROL;      
	s_advancedserver.actionhero.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_advancedserver.actionhero.generic.x					= x;
	s_advancedserver.actionhero.generic.y					= y;
	s_advancedserver.actionhero.generic.name				= "menu/common/bar1.tga";
	s_advancedserver.actionhero.generic.id					= ID_PMOD_ACTIONHERO; 
	s_advancedserver.actionhero.generic.callback			= AdvancedServer_Event;
	s_advancedserver.actionhero.color						= CT_DKPURPLE1;
	s_advancedserver.actionhero.color2						= CT_LTPURPLE1;
	s_advancedserver.actionhero.textX						= MENU_BUTTON_TEXT_X;
	s_advancedserver.actionhero.textY						= MENU_BUTTON_TEXT_Y;
	s_advancedserver.actionhero.textEnum					= MBT_ACTIONHERO;
	s_advancedserver.actionhero.textcolor					= CT_BLACK;
	s_advancedserver.actionhero.textcolor2					= CT_WHITE;	
	s_advancedserver.actionhero.listnames					= s_OffOnNone_Names;

	if  (s_serveroptions.gametype < GT_TEAM )
	{
		s_advancedserver.blueteam.generic.flags |= QMF_GRAYED;
		s_advancedserver.redteam.generic.flags |= QMF_GRAYED;
		s_advancedserver.assimilation.generic.flags |= QMF_GRAYED;
		s_advancedserver.specialties.generic.flags |= QMF_GRAYED;

		s_advancedserver.blueteam.curvalue = 0;
		s_advancedserver.redteam.curvalue = 0;

		UI_BlankGroupTable();
	}
	else	// A team game
	{
		char holdRace[256];
		// Build group table
		UI_BuildGroupTable();

		trap_Cvar_VariableStringBuffer( "g_team_group_blue", holdRace, sizeof(holdRace) );
		if (!holdRace[0])
		{
			Q_strncpyz(holdRace, "NONE", sizeof(holdRace));
		}
		s_advancedserver.blueteam.curvalue = UI_SearchGroupTable(holdRace);

		trap_Cvar_VariableStringBuffer( "g_team_group_red", holdRace, sizeof(holdRace) );
		if (!holdRace[0])
		{
			Q_strncpyz(holdRace, "NONE", sizeof(holdRace));
		}
		s_advancedserver.redteam.curvalue = UI_SearchGroupTable(holdRace);

		s_advancedserver.actionhero.generic.flags |= QMF_GRAYED;
//		s_advancedserver.elimination.generic.flags |= QMF_GRAYED;

	}

	s_advancedserver.errorText.generic.type					= MTYPE_TEXT;
	s_advancedserver.errorText.generic.flags				= QMF_HIDDEN | QMF_INACTIVE;
	s_advancedserver.errorText.generic.x					= 400;
	s_advancedserver.errorText.generic.y					= 380;
	s_advancedserver.errorText.style						= UI_SMALLFONT | UI_CENTER;	
	s_advancedserver.errorText.color						= colorTable[CT_WHITE];
	s_advancedserver.errorText.string 						= (char *) menu_normal_text[MNT_ADV_ERROR];


	Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.mainmenu );
	Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.back );
	Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.autojoin );
	Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.autobalance );
//	Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.friendlyfire );
	Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.fallingdamage );
	Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.repawntime );
	Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.maxclients );
	Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.runspeed );


	Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.classchangetimeout);
//	Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.gravity );
	Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.knockback );
	Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.dmgmult );
	if (!fromMenu)
	{
		Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.bot_minplayers);
	}

	Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.adaptitemrespawn);
	Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.holodeckintro);
	Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.forceplayerrespawn);
	Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.respawninvulnerability);
	if (!fromMenu)
	{
		Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.dowarmup);
		Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.blueteam);
		Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.redteam);
//		if (ui_cdkeychecked2.integer == 1) 
//		{
//			Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.assimilation);
//			Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.specialties);
//			Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.disintegration);
//			Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.elimination);
//			Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.actionhero);
//		}
	}

	Menu_AddItem( &s_advancedserver.menu, &s_advancedserver.errorText );

	// Get values
	s_advancedserver.autojoin.curvalue		= Com_Clamp( 0, 1, trap_Cvar_VariableValue( "g_teamAutoJoin" ) );
	s_advancedserver.autobalance.curvalue	= Com_Clamp( 0, 1, trap_Cvar_VariableValue( "g_teamForceBalance" ) );
//	s_advancedserver.friendlyfire.curvalue	= Com_Clamp( 0, 1, trap_Cvar_VariableValue( "g_friendlyFire" ) );
	dmflags	= trap_Cvar_VariableValue( "g_dmflags" );
	if (dmflags & DF_NO_FALLING)
	{
		s_advancedserver.fallingdamage.curvalue	= qfalse;	
	}
	else
	{
		s_advancedserver.fallingdamage.curvalue	= qtrue;	
	}
	s_advancedserver.adaptitemrespawn.curvalue	= Com_Clamp( 0, 1, trap_Cvar_VariableValue( "g_adaptrespawn" ) );
	s_advancedserver.holodeckintro.curvalue	= Com_Clamp( 0, 1, trap_Cvar_VariableValue( "g_holointro" ) );

	Com_sprintf( s_advancedserver.repawntime.field.buffer, 3, "%i", (int)Com_Clamp( 1, 60, trap_Cvar_VariableValue( "g_weaponrespawn" ) ) );
	Com_sprintf( s_advancedserver.maxclients.field.buffer, 3, "%i", (int)Com_Clamp( 0, 64, trap_Cvar_VariableValue( "sv_maxclients" ) ) );
	Com_sprintf( s_advancedserver.runspeed.field.buffer, 5, "%i", (int)Com_Clamp( 150, 1000, trap_Cvar_VariableValue( "g_speed" ) ) );
	Com_sprintf( s_advancedserver.gravity.field.buffer, 5, "%i", (int)Com_Clamp( 0, 5000, trap_Cvar_VariableValue( "g_gravity" ) ) );
	Com_sprintf( s_advancedserver.knockback.field.buffer, 6, "%i", (int)Com_Clamp( 0, 10000, trap_Cvar_VariableValue( "g_knockback" ) ) );
	Com_sprintf( s_advancedserver.dmgmult.field.buffer, 12, "%f", (float)Com_Clamp( 0.0, 100.0, trap_Cvar_VariableValue( "g_dmgmult" ) ) );
	Com_sprintf( s_advancedserver.bot_minplayers.field.buffer, 3, "%i", (int)Com_Clamp( 0, 64, trap_Cvar_VariableValue( "bot_minplayers" ) ) );
	Com_sprintf( s_advancedserver.forceplayerrespawn.field.buffer, 3, "%i", (int)Com_Clamp( 0, 120, trap_Cvar_VariableValue( "g_forcerespawn" ) ) );
	Com_sprintf( s_advancedserver.respawninvulnerability.field.buffer, 3, "%i", (int)Com_Clamp( 0, 30, trap_Cvar_VariableValue( "g_ghostRespawn" ) ) );
	Com_sprintf( s_advancedserver.classchangetimeout.field.buffer, 8, "%i", (int)Com_Clamp( 0, 999999, trap_Cvar_VariableValue( "g_classChangeDebounceTime" ) ) );

	holdInt = trap_Cvar_VariableValue( "g_dowarmup" );
	if (holdInt == 1)
	{
		Com_sprintf( s_advancedserver.dowarmup.field.buffer, 3, "%i", (int)Com_Clamp( 0, 120, trap_Cvar_VariableValue( "g_warmup" ) ) );
	}
	else
	{
		Com_sprintf( s_advancedserver.dowarmup.field.buffer, 3, "%i", 0 );
	}

	AdvancedServerSetModButtons();

}

/*
=================
UI_ServerAdvancedOptions
=================
*/
void UI_ServerAdvancedOptions(int fromMenu)
{
	UI_AdvancedServerMenu_Init(fromMenu);
	UI_PushMenu( &s_advancedserver.menu );
}


// Giving credit where credit is due - I stole most of this from Jake's code.
extern char* BG_RegisterRace( const char *name );
void UI_BuildGroupTable(void)
{
	int		howManySkins = 0;
	char	filename[128];
	char	dirlist[8192];
	int		i;
	char*	dirptr;
	char*	race_list;
	int		numdirs;
	int		dirlen;
	char current_race_name[125];
	char *max_place;
	char *s;
	char *marker;

	memset(skinsForRace,0,sizeof(skinsForRace));

	Q_strncpyz(skinsForRace[0],"",sizeof(skinsForRace[0]));
	s_skinsForRace[0] = skinsForRace[0];

	// search through each and every skin we can find
	numdirs = trap_FS_GetFileList("models/players_rpgx", "/", dirlist, sizeof(dirlist) );
	dirptr  = dirlist;
	for (i=0; i<numdirs && howManySkins < MAX_SKINS_FOR_RACE; i++,dirptr+=dirlen+1)
	{
		dirlen = strlen(dirptr);
		
		if (!dirlen) {
			continue;
		}

		if (dirptr[dirlen-1]=='/') {
			dirptr[dirlen-1]='\0';
		}

		if (!strcmp(dirptr,".") || !strcmp(dirptr,"..")) {
			continue;
		}

		// Get group names for this model
		Com_sprintf(filename, sizeof(filename), "models/players_rpgx/%s/groups.cfg", dirptr);
		race_list = BG_RegisterRace(filename);
		max_place = race_list + strlen(race_list);
		s = race_list;

		// look through the list till it's empty
		while (s < max_place)
		{
			marker = s;
			// figure out from where we are where the next ',' or 0 is
			while (*s != ',' && *s != 0)
			{
				s++;
			}

			// copy just that name
			Q_strncpyz(current_race_name, marker, (s-marker)+1);

			//Search table for a matching entry
			UI_SearchGroupTable(current_race_name);

			// avoid the comma or increment us past the end of the string so we fail the main while loop
			s++;
		}
	}

	UI_SearchGroupTable("NONE");	// Add this

}

static int UI_SearchGroupTable(char *current_race)
{
	int i;

	i=0;
	while (skinsForRace[i][0])
	{
		if (!strcmp(current_race,skinsForRace[i]))
		{
			return i;	
		}
		i++;
	}

	Q_strncpyz(skinsForRace[i],current_race,strlen(current_race)+1);
	s_skinsForRace[i] = skinsForRace[i];

	return i;
}

static void UI_BlankGroupTable(void)
{
	skinsForRace[0][0] = '\0';
	s_skinsForRace[0] = skinsForRace[0];
}
