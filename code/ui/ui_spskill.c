// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=============================================================================

SINGLE PLAYER SKILL MENU

=============================================================================
*/

#include "ui_local.h"


#define ART_MAP_COMPLETE1			"menu/art/level_complete1"
#define ART_MAP_COMPLETE2			"menu/art/level_complete2"
#define ART_MAP_COMPLETE3			"menu/art/level_complete3"
#define ART_MAP_COMPLETE4			"menu/art/level_complete4"
#define ART_MAP_COMPLETE5			"menu/art/level_complete5"

#define ID_BABY						10
#define ID_EASY						11
#define ID_MEDIUM					12
#define ID_HARD						13
#define ID_NIGHTMARE				14
#define ID_BACK						15
#define ID_FIGHT					16
#define ID_MAINMENU					17


typedef struct 
{
	menuframework_s	menu;

	menubitmap_s	item_baby;
	menubitmap_s	item_easy;
	menubitmap_s	item_medium;
	menubitmap_s	item_hard;
	menubitmap_s	item_nightmare;
	menubitmap_s	item_dotl;
	menubitmap_s	item_dotr;

	menubitmap_s	mainmenu;
	menubitmap_s	art_skillPic;
	menubitmap_s	item_back;
	menubitmap_s	item_fight;
	qhandle_t		corner_18_50;

	const char		*arenaInfo;
	qhandle_t		skillpics[5];
	sfxHandle_t		nightmareSound;	//	Do we need this for Voyager??? RWL
	sfxHandle_t		silenceSound;
} skillMenuInfo_t;

static skillMenuInfo_t	skillMenuInfo;

int skillButtonY[5] = 
{
	131,
	161,
	191,
	221,
	251
};

/*
=================
SetSkillColor
=================
*/
static void SetSkillColor( int skill, int color ) 
{
	switch( skill ) 
	{
	case 1:
		skillMenuInfo.item_baby.color = color;
		break;
	case 2:
		skillMenuInfo.item_easy.color = color;
		break;
	case 3:
		skillMenuInfo.item_medium.color = color;
		break;
	case 4:
		skillMenuInfo.item_hard.color = color;
		break;
	case 5:
		skillMenuInfo.item_nightmare.color = color;
		break;
	default:
		break;
	}

}


/*
=================
UI_SPSkillMenu_SkillEvent
=================
*/
static void UI_SPSkillMenu_SkillEvent( void *ptr, int notification ) 
{
	int		id;
	int		skill;

	if (notification != QM_ACTIVATED)
	{
		return;
	}

//	SetSkillColor( (int)trap_Cvar_VariableValue( "g_spSkill" ), CT_RED );

	id = ((menucommon_s*)ptr)->id;
	skill = id - ID_BABY + 1;
	trap_Cvar_SetValue( "g_spSkill", skill );

	skillMenuInfo.item_dotl.generic.y= skillButtonY[skill-1];
	skillMenuInfo.item_dotr.generic.y= skillButtonY[skill-1];

	skillMenuInfo.art_skillPic.shader = skillMenuInfo.skillpics[skill - 1];

//	if( id == ID_NIGHTMARE ) 
//	{
//		trap_S_StartLocalSound( skillMenuInfo.nightmareSound, CHAN_ANNOUNCER );
//	}
//	else 
//	{
		trap_S_StartLocalSound( skillMenuInfo.silenceSound, CHAN_ANNOUNCER );
//	}
}


/*
=================
UI_SPSkillMenu_FightEvent
=================
*/
static void UI_SPSkillMenu_FightEvent( void *ptr, int notification ) 
{
	if (notification != QM_ACTIVATED)
	{
		return;
	}

	UI_SPArena_Start( skillMenuInfo.arenaInfo );
}


/*
=================
UI_SPSkillMenu_BackEvent
=================
*/
static void UI_SPSkillMenu_BackEvent( void* ptr, int notification ) 
{
	if (notification != QM_ACTIVATED) 
	{
		return;
	}

	trap_S_StartLocalSound( skillMenuInfo.silenceSound, CHAN_ANNOUNCER );
	UI_PopMenu();
}


/*
=================
UI_SPSkillMenu_MainEvent
=================
*/
static void UI_SPSkillMenu_MainEvent( void* ptr, int notification ) 
{
	if (notification != QM_ACTIVATED) 
	{
		return;
	}

	UI_PopMenu();
	UI_MainMenu();
}

/*
=================
UI_SPSkillMenu_Key
=================
*/
static sfxHandle_t UI_SPSkillMenu_Key( int key ) 
{
	if( key == K_MOUSE2 || key == K_ESCAPE ) 
	{
		trap_S_StartLocalSound( skillMenuInfo.silenceSound, CHAN_ANNOUNCER );
	}
	return Menu_DefaultKey( &skillMenuInfo.menu, key );
}

/*
=================
UI_SPSkillMenu_MenuDraw
=================
*/
static void UI_SPSkillMenu_MenuDraw( void ) 
{
	UI_MenuFrame2(&skillMenuInfo.menu);

	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30,203,  47, 186, uis.whiteShader);	// Long left column square on bottom 3rd

	trap_R_SetColor( colorTable[CT_DKPURPLE2]);

	UI_DrawHandlePic( 146,  83,  64,  32, skillMenuInfo.corner_18_50);	// UL corner
	UI_DrawHandlePic( 479,  83, -64,  32, skillMenuInfo.corner_18_50);	// UR corner
	UI_DrawHandlePic( 146, 273,  64, -32, skillMenuInfo.corner_18_50);	// LL corner
	UI_DrawHandlePic( 479, 273, -64, -32, skillMenuInfo.corner_18_50);	// LR corner

	UI_DrawHandlePic( 156,  83, 377,  18, uis.whiteShader);	// Top
	UI_DrawHandlePic( 146, 101,  50,  51, uis.whiteShader);	// Left top
	UI_DrawHandlePic( 146, 155,  50,  90, uis.whiteShader);	// Left middle
	UI_DrawHandlePic( 146, 248,  50,  39, uis.whiteShader);	// Left bottom

	UI_DrawHandlePic( 156, 287, 377,  18, uis.whiteShader);	// Bottom
	UI_DrawHandlePic( 493, 101,  50,  51, uis.whiteShader);	// Right top
	UI_DrawHandlePic( 493, 155,  50,  90, uis.whiteShader);	// Right middle
	UI_DrawHandlePic( 493, 248,  50,  39, uis.whiteShader);	// Right bottom

	UI_DrawProportionalString( 208,  84, menu_normal_text[MNT_SKILLLEVEL],UI_SMALLFONT, colorTable[CT_BLACK]);

	UI_DrawProportionalString( 192, 138, "876-0",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString( 192, 158, "876-1",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString( 192, 251, "876-2",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	UI_DrawProportionalString( 496, 138, "878-0",UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString( 496, 158, "878-1",UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString( 496, 251, "878-2",UI_TINYFONT, colorTable[CT_BLACK]);

	UI_DrawProportionalString(  73, 130, "323",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  73, 150, "7240",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  73, 206, "1155",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  73, 375, "4550",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  73, 395, "533",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	// Left rounded ends for mainmenu and back buttons
	trap_R_SetColor( colorTable[skillMenuInfo.mainmenu.color]);
	UI_DrawHandlePic(110 - 14, skillMenuInfo.mainmenu.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor( colorTable[ skillMenuInfo.item_back.color]);
	UI_DrawHandlePic(110 - 14, skillMenuInfo.item_back.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	// standard menu drawing
	Menu_Draw( &skillMenuInfo.menu );
}


/*
=================
UI_SPSkillMenu_Cache
=================
*/
void UI_SPSkillMenu_Cache( void ) 
{
	skillMenuInfo.skillpics[0] = trap_R_RegisterShaderNoMip( ART_MAP_COMPLETE1 );
	skillMenuInfo.skillpics[1] = trap_R_RegisterShaderNoMip( ART_MAP_COMPLETE2 );
	skillMenuInfo.skillpics[2] = trap_R_RegisterShaderNoMip( ART_MAP_COMPLETE3 );
	skillMenuInfo.skillpics[3] = trap_R_RegisterShaderNoMip( ART_MAP_COMPLETE4 );
	skillMenuInfo.skillpics[4] = trap_R_RegisterShaderNoMip( ART_MAP_COMPLETE5 );

	skillMenuInfo.corner_18_50 = trap_R_RegisterShaderNoMip("menu/common/corner_ul_18_50" );

//	skillMenuInfo.nightmareSound = trap_S_RegisterSound( "sound/misc/nightmare.wav" );
	skillMenuInfo.silenceSound = trap_S_RegisterSound( "sound/silence.wav" );
}


/*
=================
UI_SPSkillMenu_Init
=================
*/
static void UI_SPSkillMenu_Init( void ) 
{
	int		skill,x;

	memset( &skillMenuInfo, 0, sizeof(skillMenuInfo) );
	skillMenuInfo.menu.fullscreen = qtrue;
	skillMenuInfo.menu.key = UI_SPSkillMenu_Key;
	skillMenuInfo.menu.draw							= UI_SPSkillMenu_MenuDraw;
	skillMenuInfo.menu.descX						= MENU_DESC_X;
	skillMenuInfo.menu.descY						= MENU_DESC_Y;
	skillMenuInfo.menu.titleX						= MENU_TITLE_X;
	skillMenuInfo.menu.titleY						= MENU_TITLE_Y;
	skillMenuInfo.menu.titleI						= MNT_SINGLEPLAYER_TITLE;
	skillMenuInfo.menu.footNoteEnum					= MNT_SINGLEPLAYER;

	UI_SPSkillMenu_Cache();

	x = 280;
	skillMenuInfo.item_dotl.generic.type				= MTYPE_BITMAP;
	skillMenuInfo.item_dotl.generic.name				= "menu/common/barbuttonleft.tga";
	skillMenuInfo.item_dotl.generic.x				= x - 20;
	skillMenuInfo.item_dotl.generic.y				= skillButtonY[0];
	skillMenuInfo.item_dotl.width					= MENU_BUTTON_MED_HEIGHT;
	skillMenuInfo.item_dotl.height					= MENU_BUTTON_MED_HEIGHT;
	skillMenuInfo.item_dotl.color					= CT_LTGOLD1;

	skillMenuInfo.item_dotr.generic.type			= MTYPE_BITMAP;
	skillMenuInfo.item_dotr.generic.name			= "menu/common/barbuttonleft.tga";
	skillMenuInfo.item_dotr.generic.x				= x + MENU_BUTTON_MED_WIDTH + 2;
	skillMenuInfo.item_dotr.generic.y				= skillButtonY[0];
	skillMenuInfo.item_dotr.width					= -MENU_BUTTON_MED_HEIGHT;
	skillMenuInfo.item_dotr.height					= MENU_BUTTON_MED_HEIGHT;
	skillMenuInfo.item_dotr.color					= CT_LTGOLD1;

	skillMenuInfo.item_baby.generic.type			= MTYPE_BITMAP;
	skillMenuInfo.item_baby.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	skillMenuInfo.item_baby.generic.name			= "menu/common/square.tga";
	skillMenuInfo.item_baby.generic.x				= x;
	skillMenuInfo.item_baby.generic.y				= skillButtonY[0];
	skillMenuInfo.item_baby.generic.callback		= UI_SPSkillMenu_SkillEvent;
	skillMenuInfo.item_baby.generic.id				= ID_BABY;
	skillMenuInfo.item_baby.width					= MENU_BUTTON_MED_WIDTH;
	skillMenuInfo.item_baby.height					= MENU_BUTTON_MED_HEIGHT;
	skillMenuInfo.item_baby.color					= CT_DKPURPLE1;
	skillMenuInfo.item_baby.color2					= CT_LTPURPLE1;
	skillMenuInfo.item_baby.textX					= MENU_BUTTON_TEXT_X;
	skillMenuInfo.item_baby.textY					= MENU_BUTTON_TEXT_Y;
	skillMenuInfo.item_baby.textEnum				= MBT_BABYLEVEL;
	skillMenuInfo.item_baby.textcolor				= CT_BLACK;
	skillMenuInfo.item_baby.textcolor2				= CT_WHITE;

	skillMenuInfo.item_easy.generic.type			= MTYPE_BITMAP;
	skillMenuInfo.item_easy.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	skillMenuInfo.item_easy.generic.name			= "menu/common/square.tga";
	skillMenuInfo.item_easy.generic.x				= x;
	skillMenuInfo.item_easy.generic.y				= skillButtonY[1];
	skillMenuInfo.item_easy.generic.callback		= UI_SPSkillMenu_SkillEvent;
	skillMenuInfo.item_easy.generic.id				= ID_EASY;
	skillMenuInfo.item_easy.width					= MENU_BUTTON_MED_WIDTH;
	skillMenuInfo.item_easy.height					= MENU_BUTTON_MED_HEIGHT;
	skillMenuInfo.item_easy.color					= CT_DKPURPLE1;
	skillMenuInfo.item_easy.color2					= CT_LTPURPLE1;
	skillMenuInfo.item_easy.textX					= MENU_BUTTON_TEXT_X;
	skillMenuInfo.item_easy.textY					= MENU_BUTTON_TEXT_Y;
	skillMenuInfo.item_easy.textEnum				= MBT_EASYLEVEL;
	skillMenuInfo.item_easy.textcolor				= CT_BLACK;
	skillMenuInfo.item_easy.textcolor2				= CT_WHITE;

	skillMenuInfo.item_medium.generic.type			= MTYPE_BITMAP;
	skillMenuInfo.item_medium.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	skillMenuInfo.item_medium.generic.name			= "menu/common/square.tga";
	skillMenuInfo.item_medium.generic.x				= x;
	skillMenuInfo.item_medium.generic.y				= skillButtonY[2];
	skillMenuInfo.item_medium.generic.callback		= UI_SPSkillMenu_SkillEvent;
	skillMenuInfo.item_medium.generic.id			= ID_MEDIUM;
	skillMenuInfo.item_medium.width					= MENU_BUTTON_MED_WIDTH;
	skillMenuInfo.item_medium.height				= MENU_BUTTON_MED_HEIGHT;
	skillMenuInfo.item_medium.color					= CT_DKPURPLE1;
	skillMenuInfo.item_medium.color2				= CT_LTPURPLE1;
	skillMenuInfo.item_medium.textX					= MENU_BUTTON_TEXT_X;
	skillMenuInfo.item_medium.textY					= MENU_BUTTON_TEXT_Y;
	skillMenuInfo.item_medium.textEnum				= MBT_MEDIUMLEVEL;
	skillMenuInfo.item_medium.textcolor				= CT_BLACK;
	skillMenuInfo.item_medium.textcolor2			= CT_WHITE;

	skillMenuInfo.item_hard.generic.type			= MTYPE_BITMAP;
	skillMenuInfo.item_hard.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	skillMenuInfo.item_hard.generic.name			= "menu/common/square.tga";
	skillMenuInfo.item_hard.generic.x				= x;
	skillMenuInfo.item_hard.generic.y				= skillButtonY[3];
	skillMenuInfo.item_hard.generic.callback		= UI_SPSkillMenu_SkillEvent;
	skillMenuInfo.item_hard.generic.id				= ID_HARD;
	skillMenuInfo.item_hard.width					= MENU_BUTTON_MED_WIDTH;
	skillMenuInfo.item_hard.height					= MENU_BUTTON_MED_HEIGHT;
	skillMenuInfo.item_hard.color					= CT_DKPURPLE1;
	skillMenuInfo.item_hard.color2					= CT_LTPURPLE1;
	skillMenuInfo.item_hard.textX					= MENU_BUTTON_TEXT_X;
	skillMenuInfo.item_hard.textY					= MENU_BUTTON_TEXT_Y;
	skillMenuInfo.item_hard.textEnum				= MBT_HARDLEVEL;
	skillMenuInfo.item_hard.textcolor				= CT_BLACK;
	skillMenuInfo.item_hard.textcolor2				= CT_WHITE;

	skillMenuInfo.item_nightmare.generic.type		= MTYPE_BITMAP;
	skillMenuInfo.item_nightmare.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	skillMenuInfo.item_nightmare.generic.name		= "menu/common/square.tga";
	skillMenuInfo.item_nightmare.generic.x			= x;
	skillMenuInfo.item_nightmare.generic.y			= skillButtonY[4];
	skillMenuInfo.item_nightmare.generic.callback	= UI_SPSkillMenu_SkillEvent;
	skillMenuInfo.item_nightmare.generic.id			= ID_NIGHTMARE;
	skillMenuInfo.item_nightmare.width				= MENU_BUTTON_MED_WIDTH;
	skillMenuInfo.item_nightmare.height				= MENU_BUTTON_MED_HEIGHT;
	skillMenuInfo.item_nightmare.color				= CT_DKPURPLE1;
	skillMenuInfo.item_nightmare.color2				= CT_LTPURPLE1;
	skillMenuInfo.item_nightmare.textX				= MENU_BUTTON_TEXT_X;
	skillMenuInfo.item_nightmare.textY				= MENU_BUTTON_TEXT_Y;
	skillMenuInfo.item_nightmare.textEnum			= MBT_NIGHTMARELEVEL;
	skillMenuInfo.item_nightmare.textcolor			= CT_BLACK;
	skillMenuInfo.item_nightmare.textcolor2			= CT_WHITE;

	skillMenuInfo.mainmenu.generic.type				= MTYPE_BITMAP;      
	skillMenuInfo.mainmenu.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	skillMenuInfo.mainmenu.generic.x				= 110;
	skillMenuInfo.mainmenu.generic.y				= 391;
	skillMenuInfo.mainmenu.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	skillMenuInfo.mainmenu.generic.id				= ID_MAINMENU;
	skillMenuInfo.mainmenu.generic.callback			= UI_SPSkillMenu_MainEvent;
	skillMenuInfo.mainmenu.width					= MENU_BUTTON_MED_WIDTH;
	skillMenuInfo.mainmenu.height					= MENU_BUTTON_MED_HEIGHT;
	skillMenuInfo.mainmenu.color					= CT_DKPURPLE1;
	skillMenuInfo.mainmenu.color2					= CT_LTPURPLE1;
	skillMenuInfo.mainmenu.textX					= MENU_BUTTON_TEXT_X;
	skillMenuInfo.mainmenu.textY					= MENU_BUTTON_TEXT_Y;
	skillMenuInfo.mainmenu.textEnum					= MBT_MAINMENU;
	skillMenuInfo.mainmenu.textcolor				= CT_BLACK;
	skillMenuInfo.mainmenu.textcolor2				= CT_WHITE;

	skillMenuInfo.item_back.generic.type			= MTYPE_BITMAP;
	skillMenuInfo.item_back.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	skillMenuInfo.item_back.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	skillMenuInfo.item_back.generic.x				= 110;
	skillMenuInfo.item_back.generic.y				= 415;
	skillMenuInfo.item_back.generic.callback		= UI_SPSkillMenu_BackEvent;
	skillMenuInfo.item_back.generic.id				= ID_BACK;
	skillMenuInfo.item_back.width					= MENU_BUTTON_MED_WIDTH;
	skillMenuInfo.item_back.height					= MENU_BUTTON_MED_HEIGHT;
	skillMenuInfo.item_back.color					= CT_DKPURPLE1;
	skillMenuInfo.item_back.color2					= CT_LTPURPLE1;
	skillMenuInfo.item_back.textX					= MENU_BUTTON_TEXT_X;
	skillMenuInfo.item_back.textY					= MENU_BUTTON_TEXT_Y;
	skillMenuInfo.item_back.textEnum				= MBT_BACK;
	skillMenuInfo.item_back.textcolor				= CT_BLACK;
	skillMenuInfo.item_back.textcolor2				= CT_WHITE;

	skillMenuInfo.item_fight.generic.type			= MTYPE_BITMAP;
	skillMenuInfo.item_fight.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	skillMenuInfo.item_fight.generic.name			= "menu/common/square.tga";
	skillMenuInfo.item_fight.generic.x				= 462;
	skillMenuInfo.item_fight.generic.y				= 391;
	skillMenuInfo.item_fight.generic.callback		= UI_SPSkillMenu_FightEvent;
	skillMenuInfo.item_fight.generic.id				= ID_FIGHT;
	skillMenuInfo.item_fight.width					= 150;
	skillMenuInfo.item_fight.height					= 42;
	skillMenuInfo.item_fight.color					= CT_DKPURPLE1;
	skillMenuInfo.item_fight.color2					= CT_LTPURPLE1;
	skillMenuInfo.item_fight.textX					= MENU_BUTTON_TEXT_X;
	skillMenuInfo.item_fight.textY					= MENU_BUTTON_TEXT_Y;
	skillMenuInfo.item_fight.textEnum				= MBT_ENGAGE;
	skillMenuInfo.item_fight.textcolor				= CT_BLACK;
	skillMenuInfo.item_fight.textcolor2				= CT_WHITE;

	skillMenuInfo.art_skillPic.generic.type		= MTYPE_BITMAP;
	skillMenuInfo.art_skillPic.generic.flags	= QMF_CENTER_JUSTIFY|QMF_INACTIVE;
	skillMenuInfo.art_skillPic.generic.x		= 354;
	skillMenuInfo.art_skillPic.generic.y		= 324;
	skillMenuInfo.art_skillPic.width			= 128;
	skillMenuInfo.art_skillPic.height			= 96; 

	Menu_AddItem( &skillMenuInfo.menu, ( void * )&skillMenuInfo.item_dotl );
	Menu_AddItem( &skillMenuInfo.menu, ( void * )&skillMenuInfo.item_dotr );
	Menu_AddItem( &skillMenuInfo.menu, ( void * )&skillMenuInfo.item_baby );
	Menu_AddItem( &skillMenuInfo.menu, ( void * )&skillMenuInfo.item_easy );
	Menu_AddItem( &skillMenuInfo.menu, ( void * )&skillMenuInfo.item_medium );
	Menu_AddItem( &skillMenuInfo.menu, ( void * )&skillMenuInfo.item_hard );
	Menu_AddItem( &skillMenuInfo.menu, ( void * )&skillMenuInfo.item_nightmare );
	Menu_AddItem( &skillMenuInfo.menu, ( void * )&skillMenuInfo.art_skillPic );
	Menu_AddItem( &skillMenuInfo.menu, ( void * )&skillMenuInfo.mainmenu );
	Menu_AddItem( &skillMenuInfo.menu, ( void * )&skillMenuInfo.item_back );
	Menu_AddItem( &skillMenuInfo.menu, ( void * )&skillMenuInfo.item_fight );

	skill = (int)Com_Clamp( 1, 5, trap_Cvar_VariableValue( "g_spSkill" ) );
	SetSkillColor( skill, CT_DKPURPLE1 );
	skillMenuInfo.art_skillPic.shader = skillMenuInfo.skillpics[skill - 1];
	skillMenuInfo.item_dotl.generic.y= skillButtonY[skill-1];
	skillMenuInfo.item_dotr.generic.y= skillButtonY[skill-1];
}


/*
=================
UI_SPSkillMenu
=================
*/
void UI_SPSkillMenu( const char *arenaInfo ) 
{
	UI_SPSkillMenu_Init();
	skillMenuInfo.arenaInfo = arenaInfo;
	UI_PushMenu( &skillMenuInfo.menu );
	Menu_SetCursorToItem( &skillMenuInfo.menu, &skillMenuInfo.item_fight );
}
