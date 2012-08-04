// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=======================================================================

TEAM ORDERS MENU

=======================================================================
*/


#include "ui_local.h"


#define ID_LIST_BOTS		10
#define ID_LIST_CTF_ORDERS	11
#define ID_LIST_TEAM_ORDERS	12


typedef struct {
	menuframework_s	menu;

	menulist_s		list;

	menubitmap_s	back;

	int				gametype;
	int				numBots;
	int				selectedBot;
	char			*bots[9];
	char			botNames[9][16];
} teamOrdersMenuInfo_t;

static teamOrdersMenuInfo_t	teamOrdersMenuInfo;

#define NUM_CTF_ORDERS		7

//you can translate this list
/*
static const char *ctfOrders[] = 
{
"I Am the Leader",
"Defend the Base",
"Follow Me",
"Get Enemy Flag",
"Camp Here",
"Report",
"I Relinquish Command"
};
*/

//you can translate this list
static int ctfOrders[] = 
{
	MNT_ORDER_IMLEADER,
	MNT_ORDER_DEFEND,
	MNT_ORDER_FOLLOW,
	MNT_ORDER_GETFLAG,
	MNT_ORDER_CAMP,
	MNT_ORDER_REPORT,
	MNT_ORDER_IRELINQUISH,
	MNT_NONE
};

//!! Cannot translate these !!
static const char *ctfMessages[] = 
{
	"i am the leader",
	"%s defend the base",
	"%s follow me",
	"%s get enemy flag",
	"%s camp here",
	"%s report",
	"i stop being the leader",
	NULL
};

#define NUM_TEAM_ORDERS		6
//you can translate this list
/*
static const char *teamOrders[] = 
{
"I Am the Leader",
"Follow Me",
"Roam"
"Camp Here",
"Report",
"I Relinquish Command"
};
*/

int teamOrders[] = 
{
	MNT_ORDER_IMLEADER,
	MNT_ORDER_FOLLOW,
	MNT_ORDER_ROAM,
	MNT_ORDER_CAMP,
	MNT_ORDER_REPORT,
	MNT_ORDER_IRELINQUISH,
	MNT_NONE
};

//!! Cannot translate these !!
static const char *teamMessages[] = 
{
	"i am the leader",
	"%s follow me",
	"%s roam",
	"%s camp here",
	"%s report",
	"i stop being the leader",
	NULL
};


/*
===============
UI_TeamOrdersMenu_BackEvent
===============
*/
static void UI_TeamOrdersMenu_BackEvent( void *ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}
	UI_PopMenu();
}


/*
===============
UI_TeamOrdersMenu_SetList
===============
*/
static void UI_TeamOrdersMenu_SetList( int id ) {
	switch( id ) {
	default:
	case ID_LIST_BOTS:
		teamOrdersMenuInfo.list.generic.id = id;
		teamOrdersMenuInfo.list.numitems = teamOrdersMenuInfo.numBots;
		teamOrdersMenuInfo.list.itemnames = (const char **)teamOrdersMenuInfo.bots;
		teamOrdersMenuInfo.list.listnames = NULL;
		 break;

	case ID_LIST_CTF_ORDERS:
		teamOrdersMenuInfo.list.generic.id = id;
		teamOrdersMenuInfo.list.numitems = NUM_CTF_ORDERS;
		teamOrdersMenuInfo.list.itemnames = NULL;
		teamOrdersMenuInfo.list.listnames = ctfOrders;
		break;

	case ID_LIST_TEAM_ORDERS:
		teamOrdersMenuInfo.list.generic.id = id;
		teamOrdersMenuInfo.list.numitems = NUM_TEAM_ORDERS;
		teamOrdersMenuInfo.list.itemnames = NULL;
		teamOrdersMenuInfo.list.listnames = teamOrders;
		break;
	}

	teamOrdersMenuInfo.list.generic.bottom = teamOrdersMenuInfo.list.generic.top + teamOrdersMenuInfo.list.numitems * PROP_HEIGHT;
}


/*
=================
UI_TeamOrdersMenu_Key
=================
*/
sfxHandle_t UI_TeamOrdersMenu_Key( int key ) {
	menulist_s	*l;
	int	x;
	int	y;
	int	index;

	l = (menulist_s	*)Menu_ItemAtCursor( &teamOrdersMenuInfo.menu );
	if( l != &teamOrdersMenuInfo.list ) {
		return Menu_DefaultKey( &teamOrdersMenuInfo.menu, key );
	}

	switch( key ) {
		case K_MOUSE1:
			x = l->generic.left;
			y = l->generic.top;
			if( UI_CursorInRect( x, y, l->generic.right - x, l->generic.bottom - y ) ) {
				index = (uis.cursory - y) / PROP_HEIGHT;
				l->oldvalue = l->curvalue;
				l->curvalue = index;

				if( l->generic.callback ) {
					l->generic.callback( l, QM_ACTIVATED );
					return menu_move_sound;
				}
			}
			return menu_null_sound;

		case K_KP_UPARROW:
		case K_UPARROW:
			l->oldvalue = l->curvalue;

			if( l->curvalue == 0 ) {
				l->curvalue = l->numitems - 1;
			}
			else {
				l->curvalue--;
			}
			return menu_move_sound;

		case K_KP_DOWNARROW:
		case K_DOWNARROW:
			l->oldvalue = l->curvalue;

			if( l->curvalue == l->numitems - 1 ) {
				l->curvalue = 0;;
			}
			else {
				l->curvalue++;
			}
			return menu_move_sound;
	}

	return Menu_DefaultKey( &teamOrdersMenuInfo.menu, key );
}


/*
=================
UI_TeamOrdersMenu_ListDraw
=================
*/
static void UI_TeamOrdersMenu_ListDraw( void *self ) {
	menulist_s	*l;
	int			x;
	int			y;
	int			i;
	float		*color;
	qboolean	hasfocus;
	int			style;

	l = (menulist_s *)self;

	hasfocus = (l->generic.parent->cursor == l->generic.menuPosition);

	x =	320;//l->generic.x;
	y =	l->generic.y;
	for( i = 0; i < l->numitems; i++ ) 
	{
		style = UI_LEFT|UI_SMALLFONT|UI_CENTER;
		if( i == l->curvalue ) 
		{
			color = colorTable[CT_DKGOLD1];
			if( hasfocus ) 
			{
				color = colorTable[CT_LTGOLD1];
			}
		}
		else 
		{
			color = colorTable[CT_DKGOLD1];
		}

		if (l->listnames)
		{
			UI_DrawProportionalString( x, y, menu_normal_text[l->listnames[i]], style, color );
		}
		else
		{
			UI_DrawProportionalString( x, y, l->itemnames[i], style, color );
		}

		y += PROP_HEIGHT;
	}
}


/*
===============
UI_TeamOrdersMenu_ListEvent
===============
*/
static void UI_TeamOrdersMenu_ListEvent( void *ptr, int event ) {
	int		id;
	int		selection;
	char	message[256];

	if (event != QM_ACTIVATED)
		return;

	id = ((menulist_s *)ptr)->generic.id;
	selection = ((menulist_s *)ptr)->curvalue;

	if( id == ID_LIST_BOTS ) 
	{
		teamOrdersMenuInfo.selectedBot = selection;
		if( teamOrdersMenuInfo.gametype == GT_CTF ) 
		{
			UI_TeamOrdersMenu_SetList( ID_LIST_CTF_ORDERS );
		}
		else 
		{
			UI_TeamOrdersMenu_SetList( ID_LIST_TEAM_ORDERS );
		}
		return;
	}

	if( id == ID_LIST_CTF_ORDERS ) 
	{
		Com_sprintf( message, sizeof(message), ctfMessages[selection], teamOrdersMenuInfo.botNames[teamOrdersMenuInfo.selectedBot] );
	}
	else 
	{
		Com_sprintf( message, sizeof(message), teamMessages[selection], teamOrdersMenuInfo.botNames[teamOrdersMenuInfo.selectedBot] );
	}

	trap_Cmd_ExecuteText( EXEC_APPEND, va( "say_team \"%s\"\n", message ) );
	UI_PopMenu();
}


/*
===============
UI_TeamOrdersMenu_BuildBotList
===============
*/
static void UI_TeamOrdersMenu_BuildBotList( void ) {
	uiClientState_t	cs;
	int		numPlayers;
	int		isBot;
	int		n;
	char	playerTeam=0;
	char	botTeam;
	char	info[MAX_INFO_STRING];

	for( n = 0; n < 9; n++ ) {
		teamOrdersMenuInfo.bots[n] = teamOrdersMenuInfo.botNames[n];
	}

	trap_GetClientState( &cs );

	Q_strncpyz( teamOrdersMenuInfo.botNames[0], "Everyone", 16 );
	teamOrdersMenuInfo.numBots = 1;

	trap_GetConfigString( CS_SERVERINFO, info, sizeof(info) );
	numPlayers = atoi( Info_ValueForKey( info, "sv_maxclients" ) );
	teamOrdersMenuInfo.gametype = atoi( Info_ValueForKey( info, "g_gametype" ) );

	for( n = 0; n < numPlayers && teamOrdersMenuInfo.numBots < 9; n++ ) {
		trap_GetConfigString( CS_PLAYERS + n, info, MAX_INFO_STRING );

		if( n == cs.clientNum ) {
			playerTeam = *Info_ValueForKey( info, "t" );
			continue;
		}

		isBot = atoi( Info_ValueForKey( info, "skill" ) );
		if( !isBot ) {
			continue;
		}

		botTeam = *Info_ValueForKey( info, "t" );
		if( botTeam != playerTeam ) {
			continue;
		}

		Q_strncpyz( teamOrdersMenuInfo.botNames[teamOrdersMenuInfo.numBots], Info_ValueForKey( info, "n" ), 16 );
		Q_CleanStr( teamOrdersMenuInfo.botNames[teamOrdersMenuInfo.numBots] );
		teamOrdersMenuInfo.numBots++;
	}
}


/*
=================
UI_TeamOrdersMenu_Draw
=================
*/
static void UI_TeamOrdersMenu_Draw( void ) 
{
	UI_MenuFrame(&teamOrdersMenuInfo.menu);

	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(30,201,  47, 90, uis.whiteShader);	//Left side of frame
	UI_DrawHandlePic(30,293,  47, 97, uis.whiteShader);

	// standard menu drawing
	Menu_Draw( &teamOrdersMenuInfo.menu );
}
/*
===============
UI_TeamOrdersMenu_Init
===============
*/
static void UI_TeamOrdersMenu_Init(int fromMenu ) 
{
	UI_TeamOrdersMenu_Cache();

	memset( &teamOrdersMenuInfo, 0, sizeof(teamOrdersMenuInfo) );
	teamOrdersMenuInfo.menu.fullscreen					= qtrue;
	teamOrdersMenuInfo.menu.wrapAround					= qtrue;
	teamOrdersMenuInfo.menu.key							= UI_TeamOrdersMenu_Key;
	teamOrdersMenuInfo.menu.draw						= UI_TeamOrdersMenu_Draw;
	teamOrdersMenuInfo.menu.descX						= MENU_DESC_X;
	teamOrdersMenuInfo.menu.descY						= MENU_DESC_Y;
	teamOrdersMenuInfo.menu.titleX						= MENU_TITLE_X;
	teamOrdersMenuInfo.menu.titleY						= MENU_TITLE_Y;
	teamOrdersMenuInfo.menu.titleI						= MNT_TEAM_ORDERS_TITLE;
	teamOrdersMenuInfo.menu.footNoteEnum				= MNT_TEAM_ORDERS;

	UI_TeamOrdersMenu_BuildBotList();

	teamOrdersMenuInfo.list.generic.type		= MTYPE_SCROLLLIST;
	teamOrdersMenuInfo.list.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	teamOrdersMenuInfo.list.generic.ownerdraw	= UI_TeamOrdersMenu_ListDraw;
	teamOrdersMenuInfo.list.generic.callback	= UI_TeamOrdersMenu_ListEvent;
	teamOrdersMenuInfo.list.generic.x			= 320-64;
	teamOrdersMenuInfo.list.generic.y			= 170;
	teamOrdersMenuInfo.list.color				= CT_DKGOLD1;
	teamOrdersMenuInfo.list.color2				= CT_LTGOLD1;

	teamOrdersMenuInfo.back.generic.type		= MTYPE_BITMAP;
	teamOrdersMenuInfo.back.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	teamOrdersMenuInfo.back.generic.name		= BUTTON_GRAPHIC_LONGRIGHT;
	teamOrdersMenuInfo.back.generic.callback	= UI_TeamOrdersMenu_BackEvent;
	teamOrdersMenuInfo.back.generic.x			= 482;
	teamOrdersMenuInfo.back.generic.y			= 136;
	teamOrdersMenuInfo.back.width  				= MENU_BUTTON_MED_WIDTH;
	teamOrdersMenuInfo.back.height  			= MENU_BUTTON_MED_HEIGHT;
	teamOrdersMenuInfo.back.color				= CT_DKPURPLE1;
	teamOrdersMenuInfo.back.color2				= CT_LTPURPLE1;
	teamOrdersMenuInfo.back.textX				= MENU_BUTTON_TEXT_X;
	teamOrdersMenuInfo.back.textY				= MENU_BUTTON_TEXT_Y;

	if (!fromMenu)
	{
		teamOrdersMenuInfo.back.textEnum			= MBT_INGAMEMENU;
	}
	else
	{
		teamOrdersMenuInfo.back.textEnum			= MBT_INGAMERESUME;
	}
	teamOrdersMenuInfo.back.textcolor			= CT_BLACK;
	teamOrdersMenuInfo.back.textcolor2			= CT_WHITE;


	Menu_AddItem( &teamOrdersMenuInfo.menu, &teamOrdersMenuInfo.list );
	Menu_AddItem( &teamOrdersMenuInfo.menu, &teamOrdersMenuInfo.back );

	teamOrdersMenuInfo.list.generic.left = 220;
	teamOrdersMenuInfo.list.generic.top = teamOrdersMenuInfo.list.generic.y;
	teamOrdersMenuInfo.list.generic.right = 420;
	UI_TeamOrdersMenu_SetList( ID_LIST_BOTS );
}


/*
=================
UI_TeamOrdersMenu_Cache
=================
*/
void UI_TeamOrdersMenu_Cache( void ) 
{
}


/*
===============
UI_TeamOrdersMenu
===============
*/
void UI_TeamOrdersMenu(int fromMenu ) 
{
	UI_TeamOrdersMenu_Init(fromMenu);
	UI_PushMenu( &teamOrdersMenuInfo.menu );
}


/*
===============
UI_TeamOrdersMenu_f
===============
*/
void UI_TeamOrdersMenu_f( void ) 
{
	uiClientState_t	cs;
	char	info[MAX_INFO_STRING];
	int		team;

	// make sure it's a team game
	trap_GetConfigString( CS_SERVERINFO, info, sizeof(info) );
	teamOrdersMenuInfo.gametype = atoi( Info_ValueForKey( info, "g_gametype" ) );
	if( teamOrdersMenuInfo.gametype < GT_TEAM ) 
	{
		return;
	}

	// not available to spectators
	trap_GetClientState( &cs );
	trap_GetConfigString( CS_PLAYERS + cs.clientNum, info, MAX_INFO_STRING );
	team = atoi( Info_ValueForKey( info, "t" ) );
	if( team == TEAM_SPECTATOR ) 
	{
		return;
	}

	Mouse_Show();

	UI_TeamOrdersMenu(1);
}
