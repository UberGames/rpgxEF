// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=======================================================================

REMOVE BOTS MENU

=======================================================================
*/


#include "ui_local.h"

#define ID_BACK				14
#define ID_UP				11
#define ID_DOWN				12
#define ID_DELETE			13
#define ID_BOTNAME0			20
#define ID_BOTNAME1			21
#define ID_BOTNAME2			22
#define ID_BOTNAME3			23
#define ID_BOTNAME4			24
#define ID_BOTNAME5			25
#define ID_BOTNAME6			26

#define BOTS_VIEWABLE		11

typedef struct {
	menuframework_s	menu;

	menubitmap_s	up;
	menubitmap_s	down;

	menutext_s		bots[BOTS_VIEWABLE];

	menubitmap_s	delete;
	menubitmap_s	back;

	int				numBots;
	int				baseBotNum;
	int				selectedBotNum;
	char			botnames[BOTS_VIEWABLE][32];
	int				botClientNums[MAX_BOTS];
} removeBotsMenuInfo_t;

static removeBotsMenuInfo_t	removeBotsMenuInfo;


/*
=================
UI_RemoveBotsMenu_SetBotNames
=================
*/
static void UI_RemoveBotsMenu_SetBotNames( void ) {
	int		n;
	char	info[MAX_INFO_STRING];

	for ( n = 0; (n < BOTS_VIEWABLE) && (removeBotsMenuInfo.baseBotNum + n < removeBotsMenuInfo.numBots); n++ ) {
		trap_GetConfigString( CS_PLAYERS + removeBotsMenuInfo.botClientNums[removeBotsMenuInfo.baseBotNum + n], info, MAX_INFO_STRING );
		Q_strncpyz( removeBotsMenuInfo.botnames[n], Info_ValueForKey( info, "n" ), sizeof(removeBotsMenuInfo.botnames[n]) );
		Q_CleanStr( removeBotsMenuInfo.botnames[n] );
	}

}


/*
=================
UI_RemoveBotsMenu_DeleteEvent
=================
*/
static void UI_RemoveBotsMenu_DeleteEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}

	trap_Cmd_ExecuteText( EXEC_APPEND, va("kick %i\n", removeBotsMenuInfo.botClientNums[removeBotsMenuInfo.baseBotNum + removeBotsMenuInfo.selectedBotNum]) );

	// Blank out name on screen so they know it's been kicked
	memset(removeBotsMenuInfo.botnames[removeBotsMenuInfo.baseBotNum + removeBotsMenuInfo.selectedBotNum],0,sizeof(removeBotsMenuInfo.botnames[removeBotsMenuInfo.baseBotNum + removeBotsMenuInfo.selectedBotNum]));
}


/*
=================
UI_RemoveBotsMenu_BotEvent
=================
*/
static void UI_RemoveBotsMenu_BotEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}

	removeBotsMenuInfo.bots[removeBotsMenuInfo.selectedBotNum].color = colorTable[CT_DKGOLD1];
	removeBotsMenuInfo.selectedBotNum = ((menucommon_s*)ptr)->id - ID_BOTNAME0;
	removeBotsMenuInfo.bots[removeBotsMenuInfo.selectedBotNum].color = colorTable[CT_YELLOW];
}


/*
=================
UI_RemoveAddBotsMenu_BackEvent
=================
*/
static void UI_RemoveBotsMenu_BackEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}
	UI_PopMenu();
}


/*
=================
UI_RemoveBotsMenu_UpEvent
=================
*/
static void UI_RemoveBotsMenu_UpEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}

	if( removeBotsMenuInfo.baseBotNum > 0 ) {
		removeBotsMenuInfo.baseBotNum--;
		UI_RemoveBotsMenu_SetBotNames();
	}
}


/*
=================
UI_RemoveBotsMenu_DownEvent
=================
*/
static void UI_RemoveBotsMenu_DownEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}

	if( removeBotsMenuInfo.baseBotNum + BOTS_VIEWABLE < removeBotsMenuInfo.numBots ) {
		removeBotsMenuInfo.baseBotNum++;
		UI_RemoveBotsMenu_SetBotNames();
	}
}


/*
=================
UI_RemoveBotsMenu_GetBots
=================
*/
static void UI_RemoveBotsMenu_GetBots( void ) {
	int		numPlayers;
	int		isBot;
	int		n;
	char	info[MAX_INFO_STRING];

	trap_GetConfigString( CS_SERVERINFO, info, sizeof(info) );
	numPlayers = atoi( Info_ValueForKey( info, "sv_maxclients" ) );
	removeBotsMenuInfo.numBots = 0;

	for( n = 0; n < numPlayers; n++ ) {
		trap_GetConfigString( CS_PLAYERS + n, info, MAX_INFO_STRING );

		isBot = atoi( Info_ValueForKey( info, "skill" ) );
		if( !isBot ) {
			continue;
		}

		removeBotsMenuInfo.botClientNums[removeBotsMenuInfo.numBots] = n;
		removeBotsMenuInfo.numBots++;
	}
}

/*
=================
UI_RemoveMenu_Draw
=================
*/
static void UI_RemoveMenu_Draw( void ) 
{
	UI_MenuFrame(&removeBotsMenuInfo.menu);

	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(30,201,  47, 90, uis.whiteShader);	//Left side of frame
	UI_DrawHandlePic(30,293,  47, 97, uis.whiteShader);

	// Bot name frame
	trap_R_SetColor( colorTable[CT_DKBLUE1]);
	UI_DrawHandlePic( 81, 172,  225,  18, uis.whiteShader);	// Top
	UI_DrawHandlePic(288, 189,   18, 246, uis.whiteShader);	// Side
	UI_DrawProportionalString( 90, 173, menu_normal_text[MNT_HOLOGRAPHIC_CHARS],UI_SMALLFONT,colorTable[CT_BLACK]);	// Top

	// standard menu drawing
	Menu_Draw( &removeBotsMenuInfo.menu );
}

/*
=================
UI_RemoveBots_Cache
=================
*/
void UI_RemoveBots_Cache( void ) 
{
	trap_R_RegisterShaderNoMip( "menu/common/arrow_up_16.tga" );
	trap_R_RegisterShaderNoMip( "menu/common/arrow_dn_16.tga" );
}


/*
=================
UI_RemoveBotsMenu_Init
=================
*/
static void UI_RemoveBotsMenu_Init( void ) {
	int		n;
	int		count;
	int		y;

	memset( &removeBotsMenuInfo, 0 ,sizeof(removeBotsMenuInfo) );
	removeBotsMenuInfo.menu.draw					= UI_RemoveMenu_Draw;
	removeBotsMenuInfo.menu.fullscreen				= qtrue;
	removeBotsMenuInfo.menu.wrapAround				= qtrue;
	removeBotsMenuInfo.menu.descX					= MENU_DESC_X;
	removeBotsMenuInfo.menu.descY					= MENU_DESC_Y;
	removeBotsMenuInfo.menu.titleX					= MENU_TITLE_X;
	removeBotsMenuInfo.menu.titleY					= MENU_TITLE_Y;
	removeBotsMenuInfo.menu.titleI					= MNT_REMOVE_HCS_TITLE;
	removeBotsMenuInfo.menu.footNoteEnum			= MNT_REMOVE_HCS;

	UI_RemoveBots_Cache();

	UI_RemoveBotsMenu_GetBots();
	UI_RemoveBotsMenu_SetBotNames();
	count = removeBotsMenuInfo.numBots < BOTS_VIEWABLE ? removeBotsMenuInfo.numBots : BOTS_VIEWABLE;

	removeBotsMenuInfo.up.generic.type					= MTYPE_BITMAP;
	removeBotsMenuInfo.up.generic.flags					= QMF_HIGHLIGHT_IF_FOCUS;
	removeBotsMenuInfo.up.generic.x						= 289;
	removeBotsMenuInfo.up.generic.y						= 196;
	removeBotsMenuInfo.up.generic.id					= ID_UP;
	removeBotsMenuInfo.up.generic.callback				= UI_RemoveBotsMenu_UpEvent;
	removeBotsMenuInfo.up.width  						= 16;
	removeBotsMenuInfo.up.height  						= 16;
	removeBotsMenuInfo.up.color  						= CT_DKGOLD1;
	removeBotsMenuInfo.up.color2  						= CT_LTGOLD1;
	removeBotsMenuInfo.up.generic.name					= "menu/common/arrow_up_16.tga";

	removeBotsMenuInfo.down.generic.type				= MTYPE_BITMAP;
	removeBotsMenuInfo.down.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	removeBotsMenuInfo.down.generic.x					= 289;
	removeBotsMenuInfo.down.generic.y					= 407;
	removeBotsMenuInfo.down.generic.id					= ID_DOWN;
	removeBotsMenuInfo.down.generic.callback			= UI_RemoveBotsMenu_DownEvent;
	removeBotsMenuInfo.down.width  						= 16;
	removeBotsMenuInfo.down.height  					= 16;
	removeBotsMenuInfo.down.color  						= CT_DKGOLD1;
	removeBotsMenuInfo.down.color2  					= CT_LTGOLD1;
	removeBotsMenuInfo.down.generic.name				= "menu/common/arrow_dn_16.tga";


	for( n = 0, y = 194; n < count; n++, y += 20 ) 
	{
		removeBotsMenuInfo.bots[n].generic.type			= MTYPE_PTEXT;
		removeBotsMenuInfo.bots[n].generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
		removeBotsMenuInfo.bots[n].generic.id			= ID_BOTNAME0 + n;
		removeBotsMenuInfo.bots[n].generic.x			= 108;
		removeBotsMenuInfo.bots[n].generic.y			= y;
		removeBotsMenuInfo.bots[n].generic.callback		= UI_RemoveBotsMenu_BotEvent;
		removeBotsMenuInfo.bots[n].string				= removeBotsMenuInfo.botnames[n];
		removeBotsMenuInfo.bots[n].color				= colorTable[CT_DKGOLD1];
		removeBotsMenuInfo.bots[n].color2				= colorTable[CT_LTGOLD1];
		removeBotsMenuInfo.bots[n].style				= UI_LEFT|UI_SMALLFONT;
	}

	removeBotsMenuInfo.delete.generic.type			= MTYPE_BITMAP;
	removeBotsMenuInfo.delete.generic.name			= GRAPHIC_SQUARE;
	removeBotsMenuInfo.delete.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	removeBotsMenuInfo.delete.generic.id			= ID_DELETE;
	removeBotsMenuInfo.delete.generic.callback		= UI_RemoveBotsMenu_DeleteEvent;
	removeBotsMenuInfo.delete.generic.x				= 356;
	removeBotsMenuInfo.delete.generic.y				= 342;
	removeBotsMenuInfo.delete.width  				= MENU_BUTTON_MED_WIDTH;
	removeBotsMenuInfo.delete.height  				= MENU_BUTTON_MED_HEIGHT * 2;
	removeBotsMenuInfo.delete.color					= CT_DKPURPLE1;
	removeBotsMenuInfo.delete.color2				= CT_LTPURPLE1;
	removeBotsMenuInfo.delete.textX					= MENU_BUTTON_TEXT_X;
	removeBotsMenuInfo.delete.textY					= MENU_BUTTON_TEXT_Y;
	removeBotsMenuInfo.delete.textEnum				= MBT_REMOVE_HOLO;
	removeBotsMenuInfo.delete.textcolor				= CT_BLACK;
	removeBotsMenuInfo.delete.textcolor2			= CT_WHITE;

	removeBotsMenuInfo.back.generic.type			= MTYPE_BITMAP;
	removeBotsMenuInfo.back.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	removeBotsMenuInfo.back.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	removeBotsMenuInfo.back.generic.id				= ID_BACK;
	removeBotsMenuInfo.back.generic.callback		= UI_RemoveBotsMenu_BackEvent;
	removeBotsMenuInfo.back.generic.x				= 482;
	removeBotsMenuInfo.back.generic.y				= 136;
	removeBotsMenuInfo.back.width  					= MENU_BUTTON_MED_WIDTH;
	removeBotsMenuInfo.back.height  				= MENU_BUTTON_MED_HEIGHT;
	removeBotsMenuInfo.back.color					= CT_DKPURPLE1;
	removeBotsMenuInfo.back.color2					= CT_LTPURPLE1;
	removeBotsMenuInfo.back.textX					= MENU_BUTTON_TEXT_X;
	removeBotsMenuInfo.back.textY					= MENU_BUTTON_TEXT_Y;
	removeBotsMenuInfo.back.textEnum				= MBT_INGAMEMENU;
	removeBotsMenuInfo.back.textcolor				= CT_BLACK;
	removeBotsMenuInfo.back.textcolor2				= CT_WHITE;


	Menu_AddItem( &removeBotsMenuInfo.menu, &removeBotsMenuInfo.up );
	Menu_AddItem( &removeBotsMenuInfo.menu, &removeBotsMenuInfo.down );
	for( n = 0; n < count; n++ ) 
	{
		Menu_AddItem( &removeBotsMenuInfo.menu, &removeBotsMenuInfo.bots[n] );
	}
	Menu_AddItem( &removeBotsMenuInfo.menu, &removeBotsMenuInfo.delete );
	Menu_AddItem( &removeBotsMenuInfo.menu, &removeBotsMenuInfo.back );

	removeBotsMenuInfo.baseBotNum = 0;
	removeBotsMenuInfo.selectedBotNum = 0;
	removeBotsMenuInfo.bots[0].color = color_white;
}


/*
=================
UI_RemoveBotsMenu
=================
*/
void UI_RemoveBotsMenu( void ) 
{
	UI_RemoveBotsMenu_Init();
	UI_PushMenu( &removeBotsMenuInfo.menu );
}
