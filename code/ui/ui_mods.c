// Copyright (C) 1999-2000 Id Software, Inc.
//
#include "ui_local.h"

#define MAX_MODS			64
#define NAMEBUFSIZE			( MAX_MODS * 48 )
#define GAMEBUFSIZE			( MAX_MODS * 16 )

#define ID_BACK				10
#define ID_GO				11
#define ID_LIST				12
#define ID_MAINMENU			100


typedef struct 
{
	menuframework_s	menu;

	menulist_s		list;

	menubitmap_s	mainmenu;
	menubitmap_s	go;

	qhandle_t		corner_ul;
	qhandle_t		corner_ur;
	qhandle_t		corner_ll;
	qhandle_t		corner_lr;

	char			description[NAMEBUFSIZE];
	char			fs_game[GAMEBUFSIZE];

	char			*descriptionPtr;
	char			*fs_gamePtr;

	char			*descriptionList[MAX_MODS];
	char			*fs_gameList[MAX_MODS];
} mods_t;

static mods_t	s_mods;


/*
===============
Mods_MenuEvent
===============
*/
static void Mods_MenuEvent( void *ptr, int event ) 
{
	if( event != QM_ACTIVATED ) 
	{
		return;
	}

	switch ( ((menucommon_s*)ptr)->id ) 
	{
	case ID_GO:
		trap_Cvar_Set( "fs_game", s_mods.fs_gameList[s_mods.list.curvalue] );
		trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart;" );
		UI_PopMenu();
		break;

	case ID_MAINMENU:
		UI_PopMenu();
		break;
	}
}


/*
===============
UI_Mods_ParseInfos
===============
*/
static void UI_Mods_ParseInfos( char *modDir, char *modDesc ) 
{
	s_mods.fs_gameList[s_mods.list.numitems] = s_mods.fs_gamePtr;
	Q_strncpyz( s_mods.fs_gamePtr, modDir, 16 );

	s_mods.descriptionList[s_mods.list.numitems] = s_mods.descriptionPtr;
	Q_strncpyz( s_mods.descriptionPtr, modDesc, 48 );

	s_mods.list.itemnames[s_mods.list.numitems] = s_mods.descriptionPtr;
	s_mods.descriptionPtr += strlen( s_mods.descriptionPtr ) + 1;
	s_mods.fs_gamePtr += strlen( s_mods.fs_gamePtr ) + 1;
	s_mods.list.numitems++;
}


/*
===============
UI_Mods_LoadModsFromFile
===============
*/
/*
static void UI_Mods_LoadModsFromFile( char *filename ) 
{
	int				len;
	fileHandle_t	f;
	char			buf[1024];

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( !f ) 
	{
		trap_Print( va( S_COLOR_RED "File not found: %s\n", filename ) );
		return;
	}

	if ( len >= sizeof(buf) ) 
	{
		trap_Print( va( S_COLOR_RED "File too large: %s is %i, max allowed is %i", filename, len, sizeof(buf) ));
		trap_FS_FCloseFile( f );
		return;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );

	len = strlen( filename );
	if( !Q_stricmp(filename +  len - 4,".mod") ) 
	{
		filename[len-4] = '\0';
	}

	UI_Mods_ParseInfos( filename, buf );
}
*/

/*
===============
Mods_LoadMods
===============
*/
static void Mods_LoadMods( void ) 
{
	int		numdirs;
	char	dirlist[4096];
	char	*dirptr;
	char	*descptr;
	int		i;
	int		dirlen;

	s_mods.list.itemnames = (const char **)s_mods.descriptionList;
	s_mods.descriptionPtr = s_mods.description;
	s_mods.fs_gamePtr = s_mods.fs_game;

	// always start off with baseEF
	s_mods.list.numitems = 1;
	s_mods.list.itemnames[0] = s_mods.descriptionList[0] = (char*)menu_normal_text[MNT_ELITEFORCE_HOLOMATCH];
	s_mods.fs_gameList[0] = "";

	numdirs = trap_FS_GetFileList( "$modlist", "", dirlist, sizeof(dirlist) );
	dirptr  = dirlist;
	for( i = 0; i < numdirs && s_mods.list.numitems < MAX_MODS; i++ )
	{
		dirlen = strlen( dirptr ) + 1;
		descptr = dirptr + dirlen;
  		UI_Mods_ParseInfos( dirptr, descptr);
		dirptr += dirlen + strlen(descptr) + 1;
	}

	trap_Print(va("%i mods parsed\n", s_mods.list.numitems));
	if (s_mods.list.numitems > MAX_MODS) 
	{
		s_mods.list.numitems = MAX_MODS;
	}
}


/*
=================
M_MainMenu_Graphics
=================
*/
void ModsMenu_Graphics (void)
{
	// Draw the basic screen layout
	UI_MenuFrame(&s_mods.menu);

	UI_DrawProportionalString(  74,  66, "66",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "122855",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  188, "81",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  206, "4561",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "2000",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30,203,  47, 123, uis.whiteShader);	// Top left column square on bottom 3rd 
	UI_DrawHandlePic(30,328,  47, 61, uis.whiteShader);	// Bottom left column square on bottom 3rd

	UI_DrawProportionalString(  74,  206, "38",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  332, "28",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);


	trap_R_SetColor( colorTable[CT_DKPURPLE3]);
	UI_DrawHandlePic(127, 167,  32,  32, s_mods.corner_ul);
	UI_DrawHandlePic(550, 167,  32,  32, s_mods.corner_ur);

	UI_DrawHandlePic(127, 410,  32,  32, s_mods.corner_ll);
	UI_DrawHandlePic(550, 410,  32,  32, s_mods.corner_lr);

	UI_DrawHandlePic(149, 167, 402,  18, uis.whiteShader);	//	Top line
	UI_DrawHandlePic(127, 192,  18, 215, uis.whiteShader);	//	Left side
	UI_DrawHandlePic(554, 192,  18, 215, uis.whiteShader);	//	Right side

	UI_DrawHandlePic(149, 414, 133,  18, uis.whiteShader);	//	Bottom line to left of button
	UI_DrawHandlePic(418, 414, 133,  18, uis.whiteShader);	//	Bottom line to right of button

	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawProportionalString(  152, 168, menu_normal_text[MNT_AVAILABLEMODS], UI_SMALLFONT, colorTable[CT_BLACK]); //colorTable[CT_LTPURPLE3] //210
	UI_DrawProportionalString(  568, 170, "38-56",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
}

/*
=================
ModsMenu_Draw
=================
*/
void ModsMenu_Draw (void)
{
	// Draw graphics particular to Mods Menu
	ModsMenu_Graphics();

	Menu_Draw( &s_mods.menu );
}

/*
=================
UI_Mods_Cache
=================
*/
void UI_ModsMenu_Cache( void ) 
{
	s_mods.corner_ul	= trap_R_RegisterShaderNoMip("menu/common/corner_ul_18_18.tga");
	s_mods.corner_ur	= trap_R_RegisterShaderNoMip("menu/common/corner_ur_18_18.tga");
	s_mods.corner_ll	= trap_R_RegisterShaderNoMip("menu/common/corner_ll_18_18.tga");
	s_mods.corner_lr	= trap_R_RegisterShaderNoMip("menu/common/corner_lr_18_18.tga");
}

/*
===============
UI_Mods_MenuInit
===============
*/
static void UI_Mods_MenuInit( void ) 
{
	// Menu Data
	memset( &s_mods, 0 ,sizeof(mods_t) );
	s_mods.menu.wrapAround					= qtrue;
	s_mods.menu.fullscreen					= qtrue;
	s_mods.menu.draw						= ModsMenu_Draw;
	s_mods.menu.descX						= MENU_DESC_X;
	s_mods.menu.descY						= MENU_DESC_Y;	
	s_mods.menu.titleX						= MENU_TITLE_X;
	s_mods.menu.titleY						= MENU_TITLE_Y;
	s_mods.menu.titleI						= MNT_MODS_TITLE;
	s_mods.menu.footNoteEnum				= MNT_MODS;

	UI_ModsMenu_Cache();

	// Button Data
	s_mods.mainmenu.generic.type			= MTYPE_BITMAP;      
	s_mods.mainmenu.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_mods.mainmenu.generic.x				= 482;
	s_mods.mainmenu.generic.y				= 136;
	s_mods.mainmenu.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	s_mods.mainmenu.generic.id				= ID_MAINMENU;
	s_mods.mainmenu.generic.callback		= Mods_MenuEvent;
	s_mods.mainmenu.width					= MENU_BUTTON_MED_WIDTH;
	s_mods.mainmenu.height					= MENU_BUTTON_MED_HEIGHT;
	s_mods.mainmenu.color					= CT_DKPURPLE1;
	s_mods.mainmenu.color2					= CT_LTPURPLE1;
	s_mods.mainmenu.textX					= MENU_BUTTON_TEXT_X;
	s_mods.mainmenu.textY					= MENU_BUTTON_TEXT_Y;
	s_mods.mainmenu.textEnum				= MBT_MAINMENU;
	s_mods.mainmenu.textcolor				= CT_BLACK;
	s_mods.mainmenu.textcolor2				= CT_WHITE;

	s_mods.go.generic.type					= MTYPE_BITMAP;
	s_mods.go.generic.flags					= QMF_HIGHLIGHT_IF_FOCUS;
	s_mods.go.generic.x						= 285;
	s_mods.go.generic.y						= 414;
	s_mods.go.generic.name					= GRAPHIC_SQUARE;
	s_mods.go.generic.id					= ID_GO;
	s_mods.go.generic.callback				= Mods_MenuEvent;
	s_mods.go.width							= MENU_BUTTON_MED_WIDTH;
	s_mods.go.height						= MENU_BUTTON_MED_HEIGHT;
	s_mods.go.color							= CT_DKPURPLE1;
	s_mods.go.color2						= CT_LTPURPLE1;
	s_mods.go.textX							= MENU_BUTTON_TEXT_X;
	s_mods.go.textY							= MENU_BUTTON_TEXT_Y;
	s_mods.go.textEnum						= MBT_LOADMOD;
	s_mods.go.textcolor						= CT_BLACK;
	s_mods.go.textcolor2					= CT_WHITE;

	// scan for mods
	s_mods.list.generic.type				= MTYPE_SCROLLLIST;
	s_mods.list.generic.flags				= QMF_PULSEIFFOCUS;
	s_mods.list.generic.callback			= Mods_MenuEvent;
	s_mods.list.generic.id					= ID_LIST;
	s_mods.list.generic.x					= 159;
	s_mods.list.generic.y					= 195;
	s_mods.list.width						= 48;
	s_mods.list.height						= 14;

	Mods_LoadMods();

	Menu_AddItem( &s_mods.menu, &s_mods.mainmenu );
	Menu_AddItem( &s_mods.menu, &s_mods.list );
	Menu_AddItem( &s_mods.menu, &s_mods.go );
}


/*
===============
UI_ModsMenu
===============
*/
void UI_ModsMenu( void ) 
{
	UI_Mods_MenuInit();
	UI_PushMenu( &s_mods.menu );
}
