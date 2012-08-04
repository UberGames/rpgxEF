// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=======================================================================

GAME OPTIONS MENU

=======================================================================
*/


#include "ui_local.h"


#define NUM_CROSSHAIRS		12
#define PIC_BUTTON2			"menu/common/full_button2.tga"

extern void BG_LoadItemNames(void);

// Precache stuff for Game Options Menu
static struct 
{
	menuframework_s		menu;

	qhandle_t	slant1;
	qhandle_t	slant2;
	qhandle_t	swooptop;
	qhandle_t	swoopbottom;
	qhandle_t	singraph;
	qhandle_t	graphbox;
	qhandle_t	lswoop;
	qhandle_t	lswoop2;
	qhandle_t	tallswoop;
	qhandle_t	tallswoop2;

} s_gameoptions;


extern int s_OffOnNone_Names[];

#define ID_LIGHTFLARES			129
#define ID_LIGHTFLARES_VALUE	130
#define ID_WALLMARKS_VALUE		133
#define ID_DYNAMICLIGHTS_VALUE	135
#define ID_IDENTIFYTARGET_VALUE	137
#define ID_SYNCEVERYFRAME_VALUE 139


#define PREFERENCES_X_POS		360

#define ID_TEXTLANGUAGE			126
#define ID_CROSSHAIR			127
#define ID_SIMPLEITEMS			128
#define ID_HIGHQUALITYSKY		129
#define ID_EJECTINGBRASS		130
#define ID_WALLMARKS			131
#define ID_DYNAMICLIGHTS		132
#define ID_IDENTIFYTARGET		133
#define ID_SYNCEVERYFRAME		134
#define ID_FORCEMODEL			135
#define ID_DRAWTEAMOVERLAY		136
#define ID_ALLOWDOWNLOAD		137
#define ID_BACK					138
#define ID_VOICELANGUAGE		139



typedef struct {
	menuframework_s		menu;

	//menubitmap_s		crosshair;
	menulist_s			crosshair;
	menulist_s			textlanguage;
	menulist_s			voicelanguage;
	menulist_s			simpleitems;
	menulist_s			wallmarks;
	menulist_s			dynamiclights;
	menulist_s			identifytarget;
	menulist_s			synceveryframe;
	menulist_s			forcemodel;
	menulist_s			drawteamoverlay;
	menulist_s			allowdownload;
	menulist_s			flares;

	int					currentcrosshair;
	//qhandle_t			crosshairShader[NUM_CROSSHAIRS];
} preferences_t;

static preferences_t s_preferences;

int s_textlanguage_Names[] =
{
	MNT_ENGLISH,
	MNT_GERMAN,
	MNT_DUTCH,
	MNT_NONE
};

int s_voicelanguage_Names[] =
{
	MNT_ENGLISH,
	MNT_GERMAN,
	MNT_NONE
};

/*static int teamoverlay_names[] =
{
	MNT_TO_OFF,
	MNT_TO_UPPER_RIGHT,
	MNT_TO_LOWER_RIGHT,
	MNT_TO_LOWER_LEFT,
	0
};*/

static void Preferences_SetMenuItems( void ) 
{
	char buffer[32];
	int *language;

	s_preferences.currentcrosshair			= trap_Cvar_VariableValue( "cg_drawCrosshair" ) != 0 /*% NUM_CROSSHAIRS*/;
	s_preferences.simpleitems.curvalue		= trap_Cvar_VariableValue( "cg_simpleItems" ) != 0;
	s_preferences.wallmarks.curvalue		= trap_Cvar_VariableValue( "cg_marks" ) != 0;
	s_preferences.identifytarget.curvalue	= trap_Cvar_VariableValue( "cg_drawCrosshairNames" ) != 0;
	s_preferences.dynamiclights.curvalue	= trap_Cvar_VariableValue( "r_dynamiclight" ) != 0;
//	s_preferences.highqualitysky.curvalue	= trap_Cvar_VariableValue ( "r_fastsky" ) == 0;
	s_preferences.synceveryframe.curvalue	= trap_Cvar_VariableValue( "r_finish" ) != 0;
	s_preferences.forcemodel.curvalue		= trap_Cvar_VariableValue( "cg_forcemodel" ) != 0;
	s_preferences.drawteamoverlay.curvalue	= Com_Clamp( 0, 3, trap_Cvar_VariableValue( "cg_drawTeamOverlay" ) );
	s_preferences.allowdownload.curvalue	= trap_Cvar_VariableValue( "cl_allowDownload" ) != 0;

	s_preferences.flares.curvalue			= Com_Clamp( 0, 1, trap_Cvar_VariableValue( "r_flares" ));	

	trap_Cvar_VariableStringBuffer( "g_language", buffer, 32 );
	language = s_textlanguage_Names;

	s_preferences.textlanguage.curvalue = 0;
	if (buffer[0]) {
		while (*language)	//scan for a match
		{
			if (Q_stricmp(menu_normal_text[*language],buffer)==0)
			{
				break;
			}
			language++;
			s_preferences.textlanguage.curvalue++;
		}

		if (!*language)
		{
			s_preferences.textlanguage.curvalue = 0;
		}
	}

	trap_Cvar_VariableStringBuffer( "s_language", buffer, 32 );
	language = s_voicelanguage_Names;

	s_preferences.voicelanguage.curvalue = 0;
	if (buffer[0]) {
		while (*language)	//scan for a match
		{
			if (Q_stricmp(menu_normal_text[*language],buffer)==0)
			{
				break;
			}
			language++;
			s_preferences.voicelanguage.curvalue++;
		}

		if (!*language)
		{
			s_preferences.voicelanguage.curvalue = 0;
		}
	}
}


static void Preferences_Event( void* ptr, int notification ) 
{
	if( notification != QM_ACTIVATED ) 
	{
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) 
	{
	case ID_CROSSHAIR:
		/*s_preferences.currentcrosshair++;
		if( s_preferences.currentcrosshair == NUM_CROSSHAIRS ) 
		{
			s_preferences.currentcrosshair = 0;
		}*/
		//trap_Cvar_SetValue( "cg_drawCrosshair", s_preferences.currentcrosshair );
		trap_Cvar_SetValue( "cg_drawCrosshair", s_preferences.crosshair.curvalue );
		break;

	case ID_SIMPLEITEMS:
		trap_Cvar_SetValue( "cg_simpleItems", s_preferences.simpleitems.curvalue );
		break;

	case ID_LIGHTFLARES:
		trap_Cvar_SetValue( "r_flares", s_preferences.flares.curvalue );
		break;

	case ID_WALLMARKS:
		trap_Cvar_SetValue( "cg_marks", s_preferences.wallmarks.curvalue );
		break;

	case ID_DYNAMICLIGHTS:
		trap_Cvar_SetValue( "r_dynamiclight", s_preferences.dynamiclights.curvalue );
		break;		

	case ID_IDENTIFYTARGET:
		trap_Cvar_SetValue( "cg_drawCrosshairNames", s_preferences.identifytarget.curvalue );
		break;

	case ID_SYNCEVERYFRAME:
		trap_Cvar_SetValue( "r_finish", s_preferences.synceveryframe.curvalue );
		break;

	case ID_FORCEMODEL:
		trap_Cvar_SetValue( "cg_forcemodel", s_preferences.forcemodel.curvalue );
		break;

	case ID_DRAWTEAMOVERLAY:
		trap_Cvar_SetValue( "cg_drawTeamOverlay", s_preferences.drawteamoverlay.curvalue );
		break;

	case ID_ALLOWDOWNLOAD:
		trap_Cvar_SetValue( "cl_allowDownload", s_preferences.allowdownload.curvalue );
		break;

	case ID_TEXTLANGUAGE:
		trap_Cvar_Set( "g_language", menu_normal_text[s_textlanguage_Names[s_preferences.textlanguage.curvalue]] );
		UI_LoadButtonText();	//reload the menus
		UI_LoadMenuText();
		BG_LoadItemNames();
		break;

	case ID_VOICELANGUAGE:
		trap_Cvar_Set( "s_language", menu_normal_text[s_voicelanguage_Names[s_preferences.voicelanguage.curvalue]] );
		trap_Cmd_ExecuteText( EXEC_APPEND, "snd_restart\n" );
		break;

	case ID_BACK:
		UI_PopMenu();
		break;
	}
}


/*
=================
Crosshair_Draw
=================
*/
/*
static void Crosshair_Draw( void *self ) {
	menulist_s	*s;
	float		*color;
	int			x, y;
	int			style;
	qboolean	focus;

	s = (menulist_s *)self;
	x = s->generic.x;
	y =	s->generic.y;

	style = UI_SMALLFONT;
	focus = (s->generic.parent->cursor == s->generic.menuPosition);

	if ( s->generic.flags & QMF_GRAYED )
		color = text_color_disabled;
	else if ( focus )
	{
		color = text_color_highlight;
		style |= UI_PULSE;
	}
	else if ( s->generic.flags & QMF_BLINK )
	{
		color = text_color_highlight;
		style |= UI_BLINK;
	}
	else
		color = text_color_normal;

	if ( focus )
	{
		// draw cursor
		UI_FillRect( s->generic.left, s->generic.top, s->generic.right-s->generic.left+1, s->generic.bottom-s->generic.top+1, listbar_color ); 
		UI_DrawChar( x, y, 13, UI_CENTER|UI_BLINK|UI_SMALLFONT, color);
	}

	UI_DrawString( x - SMALLCHAR_WIDTH, y, s->generic.name, style|UI_RIGHT, color );
	if( !s->curvalue ) {
		return;
	}
	UI_DrawHandlePic( x + SMALLCHAR_WIDTH, y - 4, 24, 24, s_preferences.crosshairShader[s->curvalue] );
}
*/

/*
=================
GameOptions_MenuDraw
=================
*/
static void GameOptions_MenuDraw( void )
{
	//int x;

	UI_MenuFrame(&s_gameoptions.menu);

	UI_Setup_MenuButtons();

	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(30,203,  47, 186, uis.whiteShader);	// Long left hand column square
/*
	UI_DrawHandlePic( 543, 171, 128,	32,	s_gameoptions.swooptop);
	UI_DrawHandlePic( 543, 396, 128,	32,	s_gameoptions.swoopbottom);

	UI_DrawHandlePic(548, 195,  64,	8,		uis.whiteShader);	// Top of right hand column
	UI_DrawHandlePic(548, 389,  64,	7,		uis.whiteShader);	// Bottom of right hand column
	UI_DrawHandlePic( 548, 206, 64, 180,    uis.whiteShader);	// 

	UI_DrawHandlePic( 104, 171, 436, 12,    uis.whiteShader);	// Top
	UI_DrawHandlePic( 104, 182, 16,  227,    uis.whiteShader);	// Side
	UI_DrawHandlePic( 104, 408, 436, 12,    uis.whiteShader);	// Bottom
*/
	//RPG-X TiM : We can't customize our crosshairs no more. :'(

//	trap_R_SetColor( colorTable[CT_DKBLUE1]);
//	UI_DrawHandlePic(387, 245, 8,   87,	uis.whiteShader);		// Lefthand side of CROSSHAIR box
//	UI_DrawHandlePic(513, 245, 8,   87,	uis.whiteShader);		// Righthand side of CROSSHAIR box
//	UI_DrawHandlePic(395, 335, 116, 3,	uis.whiteShader);		// Bottom of CROSSHAIR box
//	UI_DrawHandlePic( 387, 332, 16,  16,s_gameoptions.lswoop);	// lower left hand swoop
//	UI_DrawHandlePic( 510, 332, 16,  16,s_gameoptions.lswoop2);	// lower right hand swoop
//	
//	UI_DrawHandlePic( 387, 224, 16,  32,  s_gameoptions.tallswoop);		// upper left hand swoop
//	UI_DrawHandlePic( 507, 224, 16,  32,  s_gameoptions.tallswoop2);	// upper right hand swoop
//	
//	trap_R_SetColor( colorTable[CT_YELLOW]);
//	x = 438;
//	if (s_preferences.currentcrosshair)
//	{
//		UI_DrawHandlePic(x,270,  32, 32, s_preferences.crosshairShader[s_preferences.currentcrosshair]);	// Draw crosshair
//	}
//	else
//	{
//		UI_DrawProportionalString( 454, 275, menu_normal_text[MNT_CROSSHAIR_NONE],UI_CENTER|UI_SMALLFONT, colorTable[CT_LTGOLD1]);	// No crosshair
//	}

	// Menu frame numbers
	UI_DrawProportionalString(  74,  66, "1776",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "9214",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  188, "2510-81",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  206, "644",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "1001001",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	Menu_Draw( &s_gameoptions.menu );
}

/*
=================
GameOptions_MenuKey
=================
*/
static sfxHandle_t GameOptions_MenuKey( int key )
{
	return Menu_DefaultKey( &s_gameoptions.menu, key );
}
/*
===============
UI_GameOptionsMenu_Cache
===============
*/
void UI_GameOptionsMenu_Cache( void ) 
{
	//int i;

	s_gameoptions.slant1 = trap_R_RegisterShaderNoMip("menu/lcarscontrols/slant1.tga");
	s_gameoptions.slant2 = trap_R_RegisterShaderNoMip("menu/lcarscontrols/slant2.tga");

	s_gameoptions.swooptop = trap_R_RegisterShaderNoMip("menu/lcarscontrols/bigswooptop.tga");
	s_gameoptions.swoopbottom = trap_R_RegisterShaderNoMip("menu/lcarscontrols/bigswoopbottom.tga");

	s_gameoptions.singraph = trap_R_RegisterShaderNoMip("menu/lcarscontrols/singraph.tga");
	s_gameoptions.graphbox = trap_R_RegisterShaderNoMip("menu/lcarscontrols/graphbox.tga");

	s_gameoptions.lswoop = trap_R_RegisterShaderNoMip("menu/lcarscontrols/lswoop.tga");
	s_gameoptions.lswoop2 = trap_R_RegisterShaderNoMip("menu/lcarscontrols/lswoop2.tga");
	s_gameoptions.tallswoop = trap_R_RegisterShaderNoMip("menu/lcarscontrols/tallswoop.tga");
	s_gameoptions.tallswoop2 = trap_R_RegisterShaderNoMip("menu/lcarscontrols/tallswoop2.tga");

	trap_R_RegisterShaderNoMip(PIC_BUTTON2);

	// Precache crosshairs
	/*for( i = 0; i < NUM_CROSSHAIRS; i++ ) 
	{
		s_preferences.crosshairShader[i] = trap_R_RegisterShaderNoMip( va("gfx/2d/crosshair%c", 'a' + i ) );
	}*/

}

/*
=================
GameOptions_MenuInit
=================
*/
static void GameOptions_MenuInit( void )
{
	int x,y,inc,width;

	UI_GameOptionsMenu_Cache();

	s_gameoptions.menu.nitems				= 0;
	s_gameoptions.menu.wrapAround			= qtrue;
	s_gameoptions.menu.draw					= GameOptions_MenuDraw;
	s_gameoptions.menu.key					= GameOptions_MenuKey;
	s_gameoptions.menu.fullscreen			= qtrue;
	s_gameoptions.menu.descX				= MENU_DESC_X;
	s_gameoptions.menu.descY				= MENU_DESC_Y;
	s_gameoptions.menu.titleX				= MENU_TITLE_X;
	s_gameoptions.menu.titleY				= MENU_TITLE_Y;
	s_gameoptions.menu.titleI				= MNT_CONTROLSMENU_TITLE;
	s_gameoptions.menu.footNoteEnum			= MNT_GAMEOPTION_LABEL;

	SetupMenu_TopButtons(&s_gameoptions.menu,MENU_GAME,NULL);

	inc = 24;
	x = 100;
	y = 170;
	width = 160; //170

	s_preferences.flares.generic.type			= MTYPE_SPINCONTROL;
	s_preferences.flares.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_preferences.flares.generic.x				= x;
	s_preferences.flares.generic.y				= y;
	s_preferences.flares.generic.callback		= Preferences_Event;
	s_preferences.flares.generic.id				= ID_LIGHTFLARES;
	s_preferences.flares.textEnum				= MBT_LIGHTFLARES;
	s_preferences.flares.textcolor				= CT_BLACK;
	s_preferences.flares.textcolor2				= CT_WHITE;
	s_preferences.flares.color					= CT_DKPURPLE1;
	s_preferences.flares.color2					= CT_LTPURPLE1;
	s_preferences.flares.textX					= MENU_BUTTON_TEXT_X;
	s_preferences.flares.textY					= MENU_BUTTON_TEXT_Y;
	s_preferences.flares.listnames				= s_OffOnNone_Names;
	s_preferences.flares.width					= width;

	y += inc;
	s_preferences.wallmarks.generic.type			= MTYPE_SPINCONTROL;
	s_preferences.wallmarks.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_preferences.wallmarks.generic.x				= x;
	s_preferences.wallmarks.generic.y				= y;
	s_preferences.wallmarks.generic.callback		= Preferences_Event;
	s_preferences.wallmarks.generic.id				= ID_WALLMARKS;
	s_preferences.wallmarks.textEnum				= MBT_WALLMARKS1;
	s_preferences.wallmarks.textcolor				= CT_BLACK;
	s_preferences.wallmarks.textcolor2				= CT_WHITE;
	s_preferences.wallmarks.color					= CT_DKPURPLE1;
	s_preferences.wallmarks.color2					= CT_LTPURPLE1;
	s_preferences.wallmarks.textX					= MENU_BUTTON_TEXT_X;
	s_preferences.wallmarks.textY					= MENU_BUTTON_TEXT_Y;
	s_preferences.wallmarks.listnames				= s_OffOnNone_Names;
	s_preferences.wallmarks.width					= width;

	y += inc;
	s_preferences.dynamiclights.generic.type			= MTYPE_SPINCONTROL;
	s_preferences.dynamiclights.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_preferences.dynamiclights.generic.x				= x;
	s_preferences.dynamiclights.generic.y				= y;
	s_preferences.dynamiclights.generic.callback		= Preferences_Event;
	s_preferences.dynamiclights.generic.id				= ID_DYNAMICLIGHTS;
	s_preferences.dynamiclights.textEnum				= MBT_DYNAMICLIGHTS1;
	s_preferences.dynamiclights.textcolor				= CT_BLACK;
	s_preferences.dynamiclights.textcolor2				= CT_WHITE;
	s_preferences.dynamiclights.color					= CT_DKPURPLE1;
	s_preferences.dynamiclights.color2					= CT_LTPURPLE1;
	s_preferences.dynamiclights.textX					= MENU_BUTTON_TEXT_X;
	s_preferences.dynamiclights.textY					= MENU_BUTTON_TEXT_Y;
	s_preferences.dynamiclights.listnames				= s_OffOnNone_Names;
	s_preferences.dynamiclights.width					= width;

	y += inc;
	s_preferences.identifytarget.generic.type			= MTYPE_SPINCONTROL;
	s_preferences.identifytarget.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_preferences.identifytarget.generic.x				= x;
	s_preferences.identifytarget.generic.y				= y;
	s_preferences.identifytarget.generic.callback		= Preferences_Event;
	s_preferences.identifytarget.generic.id				= ID_IDENTIFYTARGET;
	s_preferences.identifytarget.textEnum				= MBT_IDENTIFYTARGET1;
	s_preferences.identifytarget.textcolor				= CT_BLACK;
	s_preferences.identifytarget.textcolor2				= CT_WHITE;
	s_preferences.identifytarget.color					= CT_DKPURPLE1;
	s_preferences.identifytarget.color2					= CT_LTPURPLE1;
	s_preferences.identifytarget.textX					= MENU_BUTTON_TEXT_X;
	s_preferences.identifytarget.textY					= MENU_BUTTON_TEXT_Y;
	s_preferences.identifytarget.listnames				= s_OffOnNone_Names;
	s_preferences.identifytarget.width					= width;

	y += inc;
	s_preferences.synceveryframe.generic.type			= MTYPE_SPINCONTROL;
	s_preferences.synceveryframe.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_preferences.synceveryframe.generic.x				= x;
	s_preferences.synceveryframe.generic.y				= y;
	s_preferences.synceveryframe.generic.callback		= Preferences_Event;
	s_preferences.synceveryframe.generic.id				= ID_SYNCEVERYFRAME;
	s_preferences.synceveryframe.textEnum				= MBT_SYNCEVERYFRAME1;
	s_preferences.synceveryframe.textcolor				= CT_BLACK;
	s_preferences.synceveryframe.textcolor2				= CT_WHITE;
	s_preferences.synceveryframe.color					= CT_DKPURPLE1;
	s_preferences.synceveryframe.color2					= CT_LTPURPLE1;
	s_preferences.synceveryframe.textX					= MENU_BUTTON_TEXT_X;
	s_preferences.synceveryframe.textY					= MENU_BUTTON_TEXT_Y;
	s_preferences.synceveryframe.listnames				= s_OffOnNone_Names;
	s_preferences.synceveryframe.width					= width;

	y += inc;
	s_preferences.forcemodel.generic.type			= MTYPE_SPINCONTROL;
	s_preferences.forcemodel.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_preferences.forcemodel.generic.x				= x;
	s_preferences.forcemodel.generic.y				= y;
	s_preferences.forcemodel.generic.callback		= Preferences_Event;
	s_preferences.forcemodel.generic.id				= ID_FORCEMODEL;
	s_preferences.forcemodel.textEnum				= MBT_FORCEMODEL;
	s_preferences.forcemodel.textcolor				= CT_BLACK;
	s_preferences.forcemodel.textcolor2				= CT_WHITE;
	s_preferences.forcemodel.color					= CT_DKPURPLE1;
	s_preferences.forcemodel.color2					= CT_LTPURPLE1;
	s_preferences.forcemodel.textX					= MENU_BUTTON_TEXT_X;
	s_preferences.forcemodel.textY					= MENU_BUTTON_TEXT_Y;
	s_preferences.forcemodel.listnames				= s_OffOnNone_Names;
	s_preferences.forcemodel.width					= width;

	//TiM - Replace overlay with crosshairs 
	y += inc;
	s_preferences.crosshair.generic.type				= MTYPE_SPINCONTROL;      
	s_preferences.crosshair.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_preferences.crosshair.generic.x					= x;
	s_preferences.crosshair.generic.y					= y;
	//s_preferences.crosshair.generic.name				= PIC_BUTTON2;
	s_preferences.crosshair.generic.id					= ID_CROSSHAIR;
	s_preferences.crosshair.generic.callback			= Preferences_Event;
	s_preferences.crosshair.width						= width;
	//s_preferences.crosshair.height						= 32;
	s_preferences.crosshair.color						= CT_DKPURPLE1;
	s_preferences.crosshair.color2						= CT_LTPURPLE1;
	s_preferences.crosshair.textX						= MENU_BUTTON_TEXT_X;
	s_preferences.crosshair.textY						= MENU_BUTTON_TEXT_Y;
	s_preferences.crosshair.textEnum					= MBT_CROSSHAIR;
	s_preferences.crosshair.textcolor					= CT_BLACK;
	s_preferences.crosshair.textcolor2					= CT_WHITE;
	s_preferences.crosshair.listnames					= s_OffOnNone_Names;;

	/*s_preferences.drawteamoverlay.generic.type		= MTYPE_SPINCONTROL;
	s_preferences.drawteamoverlay.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_preferences.drawteamoverlay.generic.x				= x;
	s_preferences.drawteamoverlay.generic.y				= y;
	s_preferences.drawteamoverlay.generic.callback		= Preferences_Event;
	s_preferences.drawteamoverlay.generic.id				= ID_DRAWTEAMOVERLAY;
	s_preferences.drawteamoverlay.textEnum				= MBT_DRAWTEAMOVERLAY;
	s_preferences.drawteamoverlay.textcolor				= CT_BLACK;
	s_preferences.drawteamoverlay.textcolor2				= CT_WHITE;
	s_preferences.drawteamoverlay.color					= CT_DKPURPLE1;
	s_preferences.drawteamoverlay.color2					= CT_LTPURPLE1;
	s_preferences.drawteamoverlay.textX					= MENU_BUTTON_TEXT_X;
	s_preferences.drawteamoverlay.textY					= MENU_BUTTON_TEXT_Y;
	s_preferences.drawteamoverlay.listnames				= teamoverlay_names;
	s_preferences.drawteamoverlay.width					= width;*/

	y += inc;
	s_preferences.allowdownload.generic.type			= MTYPE_SPINCONTROL;
	s_preferences.allowdownload.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_preferences.allowdownload.generic.x				= x;
	s_preferences.allowdownload.generic.y				= y;
	s_preferences.allowdownload.generic.callback		= Preferences_Event;
	s_preferences.allowdownload.generic.id				= ID_ALLOWDOWNLOAD;
	s_preferences.allowdownload.textEnum				= MBT_ALLOWDOWNLOAD;
	s_preferences.allowdownload.textcolor				= CT_BLACK;
	s_preferences.allowdownload.textcolor2				= CT_WHITE;
	s_preferences.allowdownload.color					= CT_DKPURPLE1;
	s_preferences.allowdownload.color2					= CT_LTPURPLE1;
	s_preferences.allowdownload.textX					= MENU_BUTTON_TEXT_X;
	s_preferences.allowdownload.textY					= MENU_BUTTON_TEXT_Y;
	s_preferences.allowdownload.listnames				= s_OffOnNone_Names;
	s_preferences.allowdownload.width					= width;

	y += inc;
	s_preferences.simpleitems.generic.type			= MTYPE_SPINCONTROL;
	s_preferences.simpleitems.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_preferences.simpleitems.generic.x				= x;
	s_preferences.simpleitems.generic.y				= y;
	s_preferences.simpleitems.generic.callback		= Preferences_Event;
	s_preferences.simpleitems.generic.id			= ID_SIMPLEITEMS;
	s_preferences.simpleitems.textEnum				= MBT_SIMPLEITEMS;
	s_preferences.simpleitems.textcolor				= CT_BLACK;
	s_preferences.simpleitems.textcolor2			= CT_WHITE;
	s_preferences.simpleitems.color					= CT_DKPURPLE1;
	s_preferences.simpleitems.color2				= CT_LTPURPLE1;
	s_preferences.simpleitems.textX					= MENU_BUTTON_TEXT_X;
	s_preferences.simpleitems.textY					= MENU_BUTTON_TEXT_Y;
	s_preferences.simpleitems.listnames				= s_OffOnNone_Names;
	s_preferences.simpleitems.width					= width;

	y += inc;
	s_preferences.textlanguage.generic.type			= MTYPE_SPINCONTROL;      
	s_preferences.textlanguage.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_preferences.textlanguage.generic.x			= x;
	s_preferences.textlanguage.generic.y			= y;
	s_preferences.textlanguage.generic.name			= "menu/common/bar1.tga";
	s_preferences.textlanguage.generic.callback		= Preferences_Event; 
	s_preferences.textlanguage.generic.id			= ID_TEXTLANGUAGE; 
	s_preferences.textlanguage.color				= CT_DKPURPLE1;
	s_preferences.textlanguage.color2				= CT_LTPURPLE1;
	s_preferences.textlanguage.textX				= MENU_BUTTON_TEXT_X;
	s_preferences.textlanguage.textY				= MENU_BUTTON_TEXT_Y;
	s_preferences.textlanguage.textEnum				= MBT_TEXTLANGUAGE;
	s_preferences.textlanguage.textcolor			= CT_BLACK;
	s_preferences.textlanguage.textcolor2			= CT_WHITE;	
	s_preferences.textlanguage.listnames			= s_textlanguage_Names;
	s_preferences.textlanguage.width				= width;

	y += inc;
	s_preferences.voicelanguage.generic.type					= MTYPE_SPINCONTROL;      
	s_preferences.voicelanguage.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_preferences.voicelanguage.generic.x					= x;
	s_preferences.voicelanguage.generic.y					= y;
	s_preferences.voicelanguage.generic.name					= "menu/common/bar1.tga";
	s_preferences.voicelanguage.generic.callback				= Preferences_Event; 
	s_preferences.voicelanguage.generic.id					= ID_VOICELANGUAGE; 
	s_preferences.voicelanguage.color						= CT_DKPURPLE1;
	s_preferences.voicelanguage.color2						= CT_LTPURPLE1;
	s_preferences.voicelanguage.textX						= MENU_BUTTON_TEXT_X;
	s_preferences.voicelanguage.textY						= MENU_BUTTON_TEXT_Y;
	s_preferences.voicelanguage.textEnum						= MBT_VOICELANGUAGE;
	s_preferences.voicelanguage.textcolor					= CT_BLACK;
	s_preferences.voicelanguage.textcolor2					= CT_WHITE;	
	s_preferences.voicelanguage.listnames					= s_voicelanguage_Names;
	s_preferences.voicelanguage.width						= width;

/*	s_preferences.crosshair.generic.type				= MTYPE_BITMAP;      
	s_preferences.crosshair.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_preferences.crosshair.generic.x					= 404;
	s_preferences.crosshair.generic.y					= 224;
	s_preferences.crosshair.generic.name				= PIC_BUTTON2;
	s_preferences.crosshair.generic.id					= ID_CROSSHAIR;
	s_preferences.crosshair.generic.callback			= Preferences_Event;
	s_preferences.crosshair.width						= 100;
	s_preferences.crosshair.height						= 32;
	s_preferences.crosshair.color						= CT_DKBLUE1;
	s_preferences.crosshair.color2						= CT_LTBLUE1;
	s_preferences.crosshair.textX						= 20;
	s_preferences.crosshair.textY						= 1;
	s_preferences.crosshair.textEnum					= MBT_CROSSHAIR;
	s_preferences.crosshair.textcolor					= CT_BLACK;
	s_preferences.crosshair.textcolor2					= CT_WHITE;*/

	Menu_AddItem( &s_gameoptions.menu, &s_preferences.flares );
	Menu_AddItem( &s_gameoptions.menu, &s_preferences.wallmarks );
	Menu_AddItem( &s_gameoptions.menu, &s_preferences.dynamiclights );
	Menu_AddItem( &s_gameoptions.menu, &s_preferences.identifytarget );
	Menu_AddItem( &s_gameoptions.menu, &s_preferences.synceveryframe );
	Menu_AddItem( &s_gameoptions.menu, &s_preferences.forcemodel );
	//Menu_AddItem( &s_gameoptions.menu, &s_preferences.drawteamoverlay );
	Menu_AddItem( &s_gameoptions.menu, &s_preferences.allowdownload );
	Menu_AddItem( &s_gameoptions.menu, &s_preferences.simpleitems );
	Menu_AddItem( &s_gameoptions.menu, &s_preferences.textlanguage );
	Menu_AddItem( &s_gameoptions.menu, &s_preferences.voicelanguage );
	Menu_AddItem( &s_gameoptions.menu, &s_preferences.crosshair);

	s_gameoptions.menu.initialized = qtrue;		// Show we've been here

	Preferences_SetMenuItems();
}

/*
=================
UI_GameOptionsMenu
=================
*/
void UI_GameOptionsMenu( void )
{

	GameOptions_MenuInit();

	UI_PushMenu( &s_gameoptions.menu );
}

