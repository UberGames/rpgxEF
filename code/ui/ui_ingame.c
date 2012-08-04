// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=======================================================================

INGAME MENU

=======================================================================
*/

#include "ui_local.h"

void UI_SetupWeaponsMenu( void );

int		ingameFlag = qfalse;	// true when in game menu is in use

#define INGAME_MENU_VERTICAL_SPACING	24 // 28 // Marcin

//#define ID_TEAM					10
#define ID_ADDBOTS				11
#define ID_REMOVEBOTS			12
#define ID_SETUP				13
#define ID_SERVERINFO			14
#define ID_LEAVEARENA			15
#define ID_RESTART				16
#define ID_QUIT					17
#define ID_RESUME				18
#define ID_TEAMORDERS			19
#define ID_SCREENSHOT			20
#define ID_INGAMEMENU			125
#define ID_INGAME_QUIT_YES		131
#define ID_INGAME_QUIT_NO		132

#define ID_MODEL				21
#define ID_INMATCH				22
#define ID_SPECTATE				23
#define ID_JOIN					24

#define	ID_EMOTES				25
#define	ID_ADMIN				26

//#define MAX_NAMELENGTH			24

#define ID_PLAYER_SETTINGS		27
#define ID_PLAYER_MODEL			28

#define ID_MOTD					29
#define ID_RESPAWN				30

typedef struct 
{
	menuframework_s	menu;

	//menubitmap_s	team;
	menubitmap_s	setup;
	menubitmap_s	server;
	menubitmap_s	leave;
	menubitmap_s	restart;
	menubitmap_s	addbots;
	menubitmap_s	removebots;
	menubitmap_s	teamorders;
	menubitmap_s	screenshot;
	menubitmap_s	resume;

	qhandle_t			graphic_12_8_LU;
	qhandle_t			graphic_12_8_LL;
	qhandle_t			graphic_12_8_RU;
	qhandle_t			graphic_12_8_RL;

	/*menufield_s			playerName;
	menulist_s			pClass;
	menulist_s			pRank;*/
	int					pclass;

	menubitmap_s		psettings;
	menubitmap_s		pmodel;

	menubitmap_s		join;
	menubitmap_s		spectate;

	menubitmap_s		emotes;
	menubitmap_s		admin;

	menubitmap_s		motd;
	menubitmap_s		respawn;

	menubitmap_s		playermdl;

	char				playerModel[MAX_QPATH];
	playerInfo_t		playerinfo;

	int					prevMenu;

	qboolean			isAdmin;

} ingamemenu_t;

static ingamemenu_t	s_ingame;

/*static int ingame_buttons[10][2] =
{
{152,220},
{152,220 + INGAME_MENU_VERTICAL_SPACING},
{152,220 + (INGAME_MENU_VERTICAL_SPACING *2)},
{152,220 + (INGAME_MENU_VERTICAL_SPACING *3)},
{152,220 + (INGAME_MENU_VERTICAL_SPACING *4)},

{368,220},
{368,220 + INGAME_MENU_VERTICAL_SPACING},
{368,220 + (INGAME_MENU_VERTICAL_SPACING *2)},
{368,220 + (INGAME_MENU_VERTICAL_SPACING *3)},
{368,220 + (INGAME_MENU_VERTICAL_SPACING *4)},
};*/


typedef struct 
{
	menuframework_s	menu;

	menubitmap_s	ingamemenu;
	menubitmap_s	no;
	menubitmap_s	yes;
} ingamequitmenu_t;

static ingamequitmenu_t	s_ingamequit;

/*
=================
Player_DrawPlayer
RPG-X: TiM - Modified it so it actually fit in the box, 
and had an appropriately set FOV
=================
*/
static void IngamePlayer_DrawPlayer( void *self ) 
{
	menubitmap_s	*b;
	vec3_t			viewangles;
	vec3_t			origin = {-20, 5, -4 }; //-3.8
	char			buf[MAX_QPATH];

	trap_Cvar_VariableStringBuffer( "model", buf, MAX_QPATH );
	
	if ( Q_stricmp( buf, s_ingame.playerModel ) ) {
		
		UI_PlayerInfo_SetModel( &s_ingame.playerinfo, buf );
		strcpy( s_ingame.playerModel, buf );

		viewangles[YAW]   = uis.lastYaw;
		viewangles[PITCH] = 0;
		viewangles[ROLL]  = 0;

		s_ingame.playerinfo.randomEmote = qtrue;

		UI_PlayerInfo_SetInfo( &s_ingame.playerinfo, BOTH_STAND1, BOTH_STAND1, viewangles, origin, WP_0, trap_Cvar_VariableValue( "height" ), trap_Cvar_VariableValue( "weight" ), qfalse );
	}

	b = &s_ingame.playermdl;
	UI_DrawPlayer( b->generic.x, b->generic.y, b->width, b->height, origin, &s_ingame.playerinfo, uis.realtime );

}

/*
=================
Player_ModelEvent
=================
*/

static void Player_ModelEvent ( void *self, int result ) {
	
	if ( result == QM_ACTIVATED ) {
		uis.cursorpx = uis.cursorx;
		uis.spinView = qtrue;
	}
}

/*
=================
InGame_RestartAction
=================
*/
static void InGame_RestartAction( qboolean result ) 
{
	if( !result ) 
	{
		return;
	}

	UI_PopMenu();
	trap_Cmd_ExecuteText( EXEC_APPEND, "map_restart 0\n" );
}

/*
=================
InGame_LeaveAction
=================
*/
void InGame_LeaveAction( qboolean result ) 
{
	if( !result ) 
	{
		return;
	}

	//trap_Cvar_Set ("rpg_playIntro", "0");
	UI_PopMenu();
	trap_Cmd_ExecuteText( EXEC_APPEND, "disconnect\n" );
}


/*
=================
InGame_Event
=================
*/
void InGame_Event( void *ptr, int notification ) 
{
	if( notification != QM_ACTIVATED ) 
	{
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) 
	{
//	case ID_TEAM:
////		InGame_SaveChanges();
//		UI_TeamMainMenu();
//		break;

	case ID_SETUP:
//		InGame_SaveChanges();
		UI_SetupWeaponsMenu();
		break;

	case ID_SCREENSHOT:
//		InGame_SaveChanges();
		UI_ForceMenuOff();
		trap_Cmd_ExecuteText( EXEC_APPEND, "wait; wait; wait; wait; screenshot\n" );
		break;

	case ID_LEAVEARENA:
//		InGame_SaveChanges();
		UI_ConfirmMenu( menu_normal_text[MNT_LEAVE_MATCH], 0, InGame_LeaveAction );
//		trap_Cmd_ExecuteText( EXEC_APPEND, "disconnect\n" );
		break;

	case ID_RESTART:
//		InGame_SaveChanges();
		UI_ConfirmMenu( menu_normal_text[MNT_RESTART_MATCH], 0, InGame_RestartAction );
		break;

	case ID_QUIT:
//		InGame_SaveChanges();
		UI_QuitMenu();
//		UI_ConfirmMenu( "EXIT GAME?", NULL, InGame_QuitAction );
		break;

	case ID_SERVERINFO:
//		InGame_SaveChanges();
		UI_ServerInfoMenu();
		break;

	case ID_ADDBOTS:
//		InGame_SaveChanges();
		UI_AddBotsMenu();
		break;

	case ID_REMOVEBOTS:
//		InGame_SaveChanges();
		UI_RemoveBotsMenu();
		break;

	case ID_TEAMORDERS:
//		InGame_SaveChanges();
		UI_TeamOrdersMenu(0);
		break;

	case ID_RESUME:
//		InGame_SaveChanges();
		UI_PopMenu();
		break;
	
	case ID_PLAYER_SETTINGS:
		UI_PlayerSettingsMenu( s_ingame.prevMenu );
		break;

	case ID_PLAYER_MODEL:
//		UI_PopMenu();
//		PlayerSettings_SaveChanges();
		UI_PlayerModelMenu( s_ingame.prevMenu );
		break;

	case ID_JOIN:
//		InGame_SaveChanges();
		trap_Cmd_ExecuteText( EXEC_APPEND, "cmd team free\n" );
		UI_ForceMenuOff();
		break;

	case ID_SPECTATE:
//		InGame_SaveChanges();
		trap_Cmd_ExecuteText( EXEC_APPEND, "cmd team spectator\n" );
		UI_ForceMenuOff();
		break;

	case ID_EMOTES:
		UI_EmotesMenu( qfalse );
		break;

	case ID_ADMIN:
		UI_AdminMenu(qfalse);
		break;

	case ID_MOTD: // RPG-X | Marcin | 03/01/2008
		UI_ForceMenuOff();
		UI_MotdMenu();
		break;

	case ID_RESPAWN: // RPG-X | Marcin | 03/01/2008
		//trap_Cmd_ExecuteText( EXEC_APPEND, "cmd team spectator\n" );
		//trap_Cmd_ExecuteText( EXEC_APPEND, "cmd team free\n" );
		trap_Cmd_ExecuteText( EXEC_APPEND, "respawn\n" );
		UI_ForceMenuOff();
		break;
	}
}

/*
=================
UI_InGameMenu_Draw
=================
*/
static void UI_InGameMenu_Draw( void ) 
{
	char*	playerClass;
	char	string[256];
	float	scale;
	int		y=191;
	int		yOffset = 23;
//	char* team;

	playerClass = uis.classData[s_ingame.pclass].classNameFull;

	/*switch ( s_ingame.pclass ) {
		case PC_ADMIN:
			playerClass = "Admin";
			break;
		case PC_SECURITY:
			playerClass = "Security";
			break;
		case PC_ALIEN:
			playerClass = "Alien";
			break;
		case PC_COMMAND:
			playerClass = "Command";
			break;
		case PC_SCIENCE:
			playerClass = "Science";
			break;
		case PC_ENGINEER:
			playerClass = "Engineer";
			break;
		case PC_ALPHAOMEGA22:
			playerClass = "Marine";
			break;
		case PC_N00B:
			playerClass = "n00b";
			break;
		case PC_NOCLASS:
		default:
			playerClass = "Unknown";
			break;
	}*/

	UI_MenuFrame(&s_ingame.menu);

	// Rounded button that takes place of INGAME MENU button
	trap_R_SetColor( colorTable[CT_DKPURPLE1]);
	//UI_DrawHandlePic( 482, 136,  MENU_BUTTON_MED_WIDTH - 22, MENU_BUTTON_MED_HEIGHT, uis.whiteShader);
	UI_DrawHandlePic( 460 + MENU_BUTTON_MED_WIDTH - 6, 136,  -19,  MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);	//right

	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30,203,  47, 186, uis.whiteShader);	// Long left column square on bottom 3rd

	// Left rounded ends for buttons
	trap_R_SetColor( colorTable[CT_DKPURPLE1]);

	/*UI_DrawHandlePic(s_ingame.team.generic.x - 14, s_ingame.team.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);*/

	UI_DrawHandlePic(s_ingame.addbots.generic.x - 14, s_ingame.addbots.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	UI_DrawHandlePic(s_ingame.removebots.generic.x - 14, s_ingame.removebots.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

//	UI_DrawHandlePic(s_ingame.teamorders.generic.x - 14, s_ingame.teamorders.generic.y, 
//		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	UI_DrawHandlePic(s_ingame.setup.generic.x - 14, s_ingame.setup.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	UI_DrawHandlePic(s_ingame.server.generic.x - 14, s_ingame.server.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	UI_DrawHandlePic(s_ingame.restart.generic.x - 14, s_ingame.restart.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT + 6, uis.graphicButtonLeftEnd);

	UI_DrawHandlePic(s_ingame.resume.generic.x - 14, s_ingame.resume.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT + 6, uis.graphicButtonLeftEnd);

	UI_DrawHandlePic( s_ingame.emotes.generic.x - 14, s_ingame.emotes.generic.y,
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd );

	UI_DrawHandlePic( s_ingame.motd.generic.x - 14, s_ingame.motd.generic.y,
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd );

	UI_DrawHandlePic( s_ingame.respawn.generic.x - 14, s_ingame.respawn.generic.y,
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd );


	if ( s_ingame.isAdmin/*s_ingame.pclass == PC_ADMIN*/ ) {
		UI_DrawHandlePic( s_ingame.admin.generic.x - 14, s_ingame.admin.generic.y,
			MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd );
	}

	trap_R_SetColor( colorTable[CT_VDKRED1] );
	UI_DrawHandlePic(s_ingame.leave.generic.x - 14, s_ingame.leave.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT + 6, uis.graphicButtonLeftEnd);

	trap_R_SetColor( colorTable[CT_DKPURPLE1]);
	UI_DrawHandlePic(s_ingame.spectate.generic.x - 14, s_ingame.spectate.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT + 6, uis.graphicButtonLeftEnd);

	UI_DrawHandlePic(s_ingame.join.generic.x - 14, s_ingame.join.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT + 6, uis.graphicButtonLeftEnd);

	UI_DrawProportionalString(  74,  66, "15567",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "2439",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  188, "3814",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  206, "4800",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "5671-1",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	//UI_DrawProportionalString( 584, 142, "1219",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	trap_R_SetColor( colorTable[CT_LTBLUE1]);
	UI_DrawHandlePic(  81, 228, 165,   1, uis.whiteShader); //296
	UI_DrawHandlePic(  83, 293, 161,   1, uis.whiteShader);
	UI_DrawHandlePic(  81, 365, 165,   1, uis.whiteShader);
	//UI_DrawHandlePic( 83, 169,   1, 256, uis.whiteShader);
	UI_DrawHandlePic( 136, 162,   1, 266, uis.whiteShader); //132
	UI_DrawHandlePic( 186, 162,   1, 266, uis.whiteShader); //181

	//Left Bracket around model picture
	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(81,158, 16, 16, uis.graphicBracket1CornerLU);
	UI_DrawHandlePic(81,174,  8, 94, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(81,271,  8, 11, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_LTORANGE]);
	UI_DrawHandlePic(83,285,  6, 21, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(81,310,  8, 11, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(81,324,  8, 94, uis.whiteShader);
	UI_DrawHandlePic(81,418, 16, -16, uis.graphicBracket1CornerLU);	//LD

	//Right Bracket around model picture
	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(230,158, -16, 16, uis.graphicBracket1CornerLU); //375	//RU -200
	UI_DrawHandlePic(238,174,  8, 94, uis.whiteShader); //383

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(238,271,  8, 11, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_LTORANGE]);
	UI_DrawHandlePic(238,285,  6, 21, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(238,310,  8, 11, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(238,324,  8, 94, uis.whiteShader);
	UI_DrawHandlePic(230,418, -16, -16, uis.graphicBracket1CornerLU); //375	//RD

	UI_DrawProportionalString(  271,  165, menu_normal_text[MNT_PLAYERSTATS], UI_SMALLFONT, colorTable[CT_LTGOLD1]);

	//ghey hack to stop Com_sprintf whining if we overflow the buffer lol
	Com_sprintf( string, sizeof(string), "%s: %-25.25s", menu_normal_text[MNT_NAME], UI_Cvar_VariableString("name") );
	//Q_strncpyz( string, string, 25 ); 
	UI_DrawProportionalString(	270,  y, string, UI_LEFT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);

	y+=yOffset;
	Com_sprintf( string, sizeof(string), "%s: %-25.25s", menu_normal_text[MNT_CLASS], playerClass );
	//Q_strncpyz( string, string, 25 ); 
	UI_DrawProportionalString(  270,  y, string, UI_LEFT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);

	scale = trap_Cvar_VariableValue( "height" ) * (float)BASE_HEIGHT;

	Com_sprintf( string, sizeof(string), "%s: %3.2f%s", menu_normal_text[MNT_HEIGHT], scale, HEIGHT_UNIT );
	//Q_strncpyz( string, string, 25 ); 
	UI_DrawProportionalString(  432,  y, string, UI_LEFT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);

	y+=yOffset;
	Com_sprintf( string, sizeof(string), "%s: %-25.25s", menu_normal_text[MNT_RANK], uis.rankSet.rankNames[uis.currentRank].formalName );
	//Q_strncpyz( string, string, 25 ); 
	UI_DrawProportionalString(  270,  y, string, UI_LEFT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);
	
	scale = trap_Cvar_VariableValue( "height" ) * trap_Cvar_VariableValue( "weight" ) * (float)BASE_WEIGHT;
	if ( s_ingame.playerinfo.gender == GENDER_FEMALE )
		scale *= (float)FEMALE_OFFSET;

	Com_sprintf( string, sizeof(string), "%s: %3.2f%s", menu_normal_text[MNT_WEIGHT], scale, WEIGHT_UNIT );
	//Q_strncpyz( string, string, 25 ); 
	UI_DrawProportionalString(  432,  y, string, UI_LEFT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);

	y+=yOffset;
	Com_sprintf( string, sizeof(string), "%s: %-25.25s", menu_normal_text[MNT_AGE], UI_Cvar_VariableString("age") );
	//Q_strncpyz( string, string, 25 ); 
	UI_DrawProportionalString(  270,  y, string, UI_LEFT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);

	Com_sprintf( string, sizeof(string), "%s: %-25.25s", menu_normal_text[MNT_RACE], UI_Cvar_VariableString("race") );
	//Q_strncpyz( string, string, 25 ); 
	UI_DrawProportionalString(  432,  y, string, UI_LEFT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);

//	UI_DrawProportionalString(	319,  193, UI_Cvar_VariableString("name"), UI_LEFT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);
//	UI_DrawProportionalString(  319,  215, pClass, UI_LEFT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);
//	UI_DrawProportionalString(  319,  236, pRank, UI_LEFT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);
//	UI_DrawProportionalString(  319,  257, pModel, UI_LEFT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);

	UI_DrawProportionalString(  271,  309, menu_normal_text[MNT_PLAYERSTATUS],UI_SMALLFONT, colorTable[CT_LTGOLD1]);

	// bracket around the buttons
	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic( 252, 165, 16, 32, s_ingame.graphic_12_8_LU );
	UI_DrawHandlePic( 252, 270, 16, 32, s_ingame.graphic_12_8_LL );
	UI_DrawHandlePic( 587, 165, 16, 32, s_ingame.graphic_12_8_RU );
	UI_DrawHandlePic( 587, 270, 16, 32, s_ingame.graphic_12_8_RL );

	UI_DrawHandlePic( 252, 189, 8, 20, uis.whiteShader ); //25 //Left upper  rect
	UI_DrawHandlePic( 252, 256, 8, 22, uis.whiteShader ); //Left lower rect

	UI_DrawHandlePic( 595, 189, 8, 20, uis.whiteShader ); //right up
	UI_DrawHandlePic( 595, 256, 8, 22, uis.whiteShader ); //right down

	UI_DrawHandlePic( (3 + 271 + (UI_ProportionalStringWidth(menu_normal_text[MNT_PLAYERSTATS],UI_SMALLFONT))), 165, ((313 - (UI_ProportionalStringWidth(menu_normal_text[MNT_PLAYERSTATS],UI_SMALLFONT))) - 3), 18, uis.whiteShader );

	//bracket around the main buttons
	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic( 440, 309, 16, 32, s_ingame.graphic_12_8_LU );
	UI_DrawHandlePic( 440, 332, 8, 25, uis.whiteShader );
	UI_DrawHandlePic( 440, 408, 8, 26, uis.whiteShader );

	//brackets around spectate/rejoin

	UI_DrawHandlePic( 
	(3 + 271 + (UI_ProportionalStringWidth(menu_normal_text[MNT_PLAYERSTATUS],UI_SMALLFONT))), 
	309, 
	((144 - (UI_ProportionalStringWidth(menu_normal_text[MNT_PLAYERSTATUS],UI_SMALLFONT))) - 3), 
	18, 
	uis.whiteShader );

	UI_DrawHandlePic( 252, 309, 16, 32, s_ingame.graphic_12_8_LU ); //corners
	UI_DrawHandlePic( 252, 402, 16, 32, s_ingame.graphic_12_8_LL );
	UI_DrawHandlePic( 418, 309, 16, 32, s_ingame.graphic_12_8_RU );
	UI_DrawHandlePic( 418, 402, 16, 32, s_ingame.graphic_12_8_RL );

	UI_DrawHandlePic( 252, 333, 8, 20, uis.whiteShader ); //topside
	UI_DrawHandlePic( 426, 333, 8, 20, uis.whiteShader );

	UI_DrawHandlePic( 252, 390, 8, 20, uis.whiteShader ); //underside
	UI_DrawHandlePic( 426, 390, 8, 20, uis.whiteShader );

	// dark blue
	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic( 404, 284, 47, 18, uis.whiteShader ); //main bars along the bottom //257
//	UI_DrawHandlePic( 496, 284, 88, 18, uis.whiteShader );

	UI_DrawHandlePic( 252, 212, 8, 41, uis.whiteShader ); //dark blue blocks
	UI_DrawHandlePic( 595, 212, 8, 41, uis.whiteShader );

	//main buttons
	UI_DrawHandlePic( 440, 360, 8, 45, uis.whiteShader );
	UI_DrawHandlePic( 459, 309, 132, 18, uis.whiteShader );
	UI_DrawHandlePic( 460 + MENU_BUTTON_MED_WIDTH - 6, 309,  -19,  18, uis.graphicButtonLeftEnd);

	//team buttons brackets
	UI_DrawHandlePic( 252, 356, 8, 31, uis.whiteShader );
	UI_DrawHandlePic( 426, 356, 8, 31, uis.whiteShader );

	UI_DrawHandlePic( 271, 416, 144, 18, uis.whiteShader );

	//text
	UI_DrawProportionalString( 570, 314, "4516", UI_TINYFONT, colorTable[CT_BLACK]);

	// standard menu drawing
	Menu_Draw( &s_ingame.menu );
}

/*
=================
InGame_MenuInit
=================
*/
void InGame_MenuInit( void ) 
{
	int		y,x;
	uiClientState_t	cs;
	char	info[MAX_INFO_STRING];
	char	info_server[MAX_INFO_STRING];
	int		team;

	trap_GetClientState( &cs );
	trap_GetConfigString( CS_PLAYERS + cs.clientNum, info, MAX_INFO_STRING );

	memset( &s_ingame, 0 ,sizeof(ingamemenu_t) );

	InGame_Cache();

	uis.lastYaw = 200;

	//TiM - Store current class
	s_ingame.pclass = atoi( Info_ValueForKey( info, "p" ) );
	s_ingame.isAdmin = atoi( Info_ValueForKey( info, "admin" ));

	//TiM: flush the ranks data
	trap_GetConfigString( CS_SERVERINFO, info_server, MAX_INFO_STRING );
	UI_InitRanksData( Info_ValueForKey( info_server, "rpg_rankSet" ) );
	UI_InitClassData( Info_ValueForKey( info_server, "rpg_classSet" ) );

	//TiM: Reset Class Data
	//UI_LoadClassString();

	//	Player_MenuInit();

	s_ingame.menu.wrapAround			= qtrue;
	s_ingame.menu.fullscreen			= qtrue;
	s_ingame.menu.descX					= MENU_DESC_X;
	s_ingame.menu.descY					= MENU_DESC_Y;
	s_ingame.menu.draw					= UI_InGameMenu_Draw;
	s_ingame.menu.titleX				= MENU_TITLE_X;
	s_ingame.menu.titleY				= MENU_TITLE_Y;
	s_ingame.menu.titleI				= MNT_INGAMEMAIN_TITLE;
	s_ingame.menu.footNoteEnum			= MNT_INGAME_MENU;

	x = 284; //305
	y = 62; //196
	/*s_ingame.team.generic.type			= MTYPE_BITMAP;
	s_ingame.team.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingame.team.generic.x				= x;
	s_ingame.team.generic.y				= y;
	s_ingame.team.generic.id			= ID_TEAM;
	s_ingame.team.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	s_ingame.team.generic.callback		= InGame_Event; 
	s_ingame.team.width					= MENU_BUTTON_MED_WIDTH;
	s_ingame.team.height				= MENU_BUTTON_MED_HEIGHT;
	s_ingame.team.color					= CT_DKPURPLE1;
	s_ingame.team.color2				= CT_LTPURPLE1;
	s_ingame.team.textX					= MENU_BUTTON_TEXT_X;
	s_ingame.team.textY					= MENU_BUTTON_TEXT_Y;
	s_ingame.team.textEnum				= MBT_TEAMCLASS;
	s_ingame.team.textcolor				= CT_BLACK;
	s_ingame.team.textcolor2			= CT_WHITE;*/


//	y += INGAME_MENU_VERTICAL_SPACING;
	s_ingame.addbots.generic.type		= MTYPE_BITMAP;
	s_ingame.addbots.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingame.addbots.generic.x			= x;
	s_ingame.addbots.generic.y			= y;
	s_ingame.addbots.generic.id			= ID_ADDBOTS;
	s_ingame.addbots.generic.name		= BUTTON_GRAPHIC_LONGRIGHT;
	s_ingame.addbots.generic.callback	= InGame_Event; 
	s_ingame.addbots.width				= MENU_BUTTON_MED_WIDTH;
	s_ingame.addbots.height				= MENU_BUTTON_MED_HEIGHT;
	s_ingame.addbots.color				= CT_DKPURPLE1;
	s_ingame.addbots.color2				= CT_LTPURPLE1;
	s_ingame.addbots.textX				= MENU_BUTTON_TEXT_X;
	s_ingame.addbots.textY				= MENU_BUTTON_TEXT_Y;
	s_ingame.addbots.textEnum			= MBT_INGAMEADDSIMULANTS;
	s_ingame.addbots.textcolor			= CT_BLACK;
	s_ingame.addbots.textcolor2			= CT_WHITE;
	if( !trap_Cvar_VariableValue( "sv_running" ) || !trap_Cvar_VariableValue( "bot_enable" ) || (trap_Cvar_VariableValue( "g_gametype" ) == GT_SINGLE_PLAYER)) 
	{
		s_ingame.addbots.generic.flags |= QMF_GRAYED;
	}

	y += INGAME_MENU_VERTICAL_SPACING;
	s_ingame.removebots.generic.type		= MTYPE_BITMAP;
	s_ingame.removebots.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingame.removebots.generic.x			= x;
	s_ingame.removebots.generic.y			= y;
	s_ingame.removebots.generic.id			= ID_REMOVEBOTS;
	s_ingame.removebots.generic.name		= BUTTON_GRAPHIC_LONGRIGHT;
	s_ingame.removebots.generic.callback	= InGame_Event; 
	s_ingame.removebots.width				= MENU_BUTTON_MED_WIDTH;
	s_ingame.removebots.height				= MENU_BUTTON_MED_HEIGHT;
	s_ingame.removebots.color				= CT_DKPURPLE1;
	s_ingame.removebots.color2				= CT_LTPURPLE1;
	s_ingame.removebots.textX				= MENU_BUTTON_TEXT_X;
	s_ingame.removebots.textY				= MENU_BUTTON_TEXT_Y;
	s_ingame.removebots.textEnum			= MBT_INGAMEREMOVESIMULANTS;
	s_ingame.removebots.textcolor			= CT_BLACK;
	s_ingame.removebots.textcolor2			= CT_WHITE;
	if( !trap_Cvar_VariableValue( "sv_running" ) || !trap_Cvar_VariableValue( "bot_enable" ) || (trap_Cvar_VariableValue( "g_gametype" ) == GT_SINGLE_PLAYER)) {
		s_ingame.removebots.generic.flags |= QMF_GRAYED;
	}

	y += INGAME_MENU_VERTICAL_SPACING;
	s_ingame.respawn.generic.type			= MTYPE_BITMAP;
	s_ingame.respawn.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingame.respawn.generic.x				= x;
	s_ingame.respawn.generic.y				= y;
	s_ingame.respawn.generic.id				= ID_RESPAWN;
	s_ingame.respawn.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	s_ingame.respawn.generic.callback		= InGame_Event;
	s_ingame.respawn.width					= MENU_BUTTON_MED_WIDTH;
	s_ingame.respawn.height					= MENU_BUTTON_MED_HEIGHT;
	s_ingame.respawn.color					= CT_DKPURPLE1;
	s_ingame.respawn.color2					= CT_LTPURPLE1;
	s_ingame.respawn.textX					= MENU_BUTTON_TEXT_X;
	s_ingame.respawn.textY					= MENU_BUTTON_TEXT_Y;
	s_ingame.respawn.textEnum				= MBT_RESPAWN;
	s_ingame.respawn.textcolor				= CT_BLACK;
	s_ingame.respawn.textcolor2				= CT_WHITE;

	x = 448;
	y = 62;
	s_ingame.emotes.generic.type			= MTYPE_BITMAP;
	s_ingame.emotes.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingame.emotes.generic.x				= x;
	s_ingame.emotes.generic.y				= y;
	s_ingame.emotes.generic.id				= ID_EMOTES;
	s_ingame.emotes.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	s_ingame.emotes.generic.callback		= InGame_Event;
	s_ingame.emotes.width					= MENU_BUTTON_MED_WIDTH;
	s_ingame.emotes.height					= MENU_BUTTON_MED_HEIGHT;
	s_ingame.emotes.color					= CT_DKPURPLE1;
	s_ingame.emotes.color2					= CT_LTPURPLE1;
	s_ingame.emotes.textX					= MENU_BUTTON_TEXT_X;
	s_ingame.emotes.textY					= MENU_BUTTON_TEXT_Y;
	s_ingame.emotes.textEnum				= MBT_EMOTES_MENU;
	s_ingame.emotes.textcolor				= CT_BLACK;
	s_ingame.emotes.textcolor2				= CT_WHITE;

	y += INGAME_MENU_VERTICAL_SPACING;
	s_ingame.admin.generic.type				= MTYPE_BITMAP;
	s_ingame.admin.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingame.admin.generic.x				= x;
	s_ingame.admin.generic.y				= y;
	s_ingame.admin.generic.id				= ID_ADMIN;
	s_ingame.admin.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_ingame.admin.generic.callback			= InGame_Event;
	s_ingame.admin.width					= MENU_BUTTON_MED_WIDTH;
	s_ingame.admin.height					= MENU_BUTTON_MED_HEIGHT;
	s_ingame.admin.color					= CT_DKPURPLE1;
	s_ingame.admin.color2					= CT_LTPURPLE1;
	s_ingame.admin.textX					= MENU_BUTTON_TEXT_X;
	s_ingame.admin.textY					= MENU_BUTTON_TEXT_Y;
	s_ingame.admin.textEnum					= MBT_ADMIN_MENU;
	s_ingame.admin.textcolor				= CT_BLACK;
	s_ingame.admin.textcolor2				= CT_WHITE;

	if ( !s_ingame.isAdmin/*s_ingame.pclass != PC_ADMIN*/ ) {
		s_ingame.admin.generic.flags		|= (QMF_HIDDEN|QMF_INACTIVE|QMF_GRAYED);
	}

	/*y += INGAME_MENU_VERTICAL_SPACING;
	s_ingame.teamorders.generic.type		= MTYPE_BITMAP;
	s_ingame.teamorders.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingame.teamorders.generic.x			= x;
	s_ingame.teamorders.generic.y			= y;
	s_ingame.teamorders.generic.id			= ID_TEAMORDERS;
	s_ingame.teamorders.generic.name		= BUTTON_GRAPHIC_LONGRIGHT;
	s_ingame.teamorders.generic.callback	= InGame_Event; 
	s_ingame.teamorders.width				= MENU_BUTTON_MED_WIDTH;
	s_ingame.teamorders.height				= MENU_BUTTON_MED_HEIGHT;
	s_ingame.teamorders.color				= CT_DKPURPLE1;
	s_ingame.teamorders.color2				= CT_LTPURPLE1;
	s_ingame.teamorders.textX				= MENU_BUTTON_TEXT_X;
	s_ingame.teamorders.textY				= MENU_BUTTON_TEXT_Y;
	s_ingame.teamorders.textEnum			= MBT_INGAMETEAMORDERS;
	s_ingame.teamorders.textcolor			= CT_BLACK;
	s_ingame.teamorders.textcolor2			= CT_WHITE;

	// make sure it's a team game
	trap_GetConfigString( CS_SERVERINFO, info, sizeof(info) );
	if( (atoi( Info_ValueForKey( info, "g_gametype" ) )) < GT_TEAM)
	{
		s_ingame.teamorders.generic.flags |= QMF_GRAYED;
	}
	else 
	{
		trap_GetClientState( &cs );
		trap_GetConfigString( CS_PLAYERS + cs.clientNum, info, MAX_INFO_STRING );
		team = atoi( Info_ValueForKey( info, "t" ) );
		if( team == TEAM_SPECTATOR ) 
		{
			s_ingame.teamorders.generic.flags |= QMF_GRAYED;
		}
	}*/

	x = 121;
	y = 62;
	s_ingame.setup.generic.type			= MTYPE_BITMAP;
	s_ingame.setup.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingame.setup.generic.x			= x;
	s_ingame.setup.generic.y			= y;
	s_ingame.setup.generic.id			= ID_SETUP;
	s_ingame.setup.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	s_ingame.setup.generic.callback		= InGame_Event; 
	s_ingame.setup.width				= MENU_BUTTON_MED_WIDTH;
	s_ingame.setup.height				= MENU_BUTTON_MED_HEIGHT;
	s_ingame.setup.color				= CT_DKPURPLE1;
	s_ingame.setup.color2				= CT_LTPURPLE1;
	s_ingame.setup.textX				= MENU_BUTTON_TEXT_X;
	s_ingame.setup.textY				= MENU_BUTTON_TEXT_Y;
	s_ingame.setup.textEnum				= MBT_INGAMESETUP;
	s_ingame.setup.textcolor			= CT_BLACK;
	s_ingame.setup.textcolor2			= CT_WHITE;

	y += INGAME_MENU_VERTICAL_SPACING;
	s_ingame.server.generic.type		= MTYPE_BITMAP;
	s_ingame.server.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingame.server.generic.x			= x;
	s_ingame.server.generic.y			= y;
	s_ingame.server.generic.id			= ID_SERVERINFO;
	s_ingame.server.generic.name		= BUTTON_GRAPHIC_LONGRIGHT;
	s_ingame.server.generic.callback	= InGame_Event; 
	s_ingame.server.width				= MENU_BUTTON_MED_WIDTH;
	s_ingame.server.height				= MENU_BUTTON_MED_HEIGHT;
	s_ingame.server.color				= CT_DKPURPLE1;
	s_ingame.server.color2				= CT_LTPURPLE1;
	s_ingame.server.textX				= MENU_BUTTON_TEXT_X;
	s_ingame.server.textY				= MENU_BUTTON_TEXT_Y;
	s_ingame.server.textEnum			= MBT_INGAMESERVERDATA;
	s_ingame.server.textcolor			= CT_BLACK;
	s_ingame.server.textcolor2			= CT_WHITE;

	// RPG-X | Marcin | 03/01/2009
	y += INGAME_MENU_VERTICAL_SPACING;
	s_ingame.motd.generic.type			= MTYPE_BITMAP;
	s_ingame.motd.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingame.motd.generic.x				= x;
	s_ingame.motd.generic.y				= y;
	s_ingame.motd.generic.id			= ID_MOTD;
	s_ingame.motd.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	s_ingame.motd.generic.callback		= InGame_Event; 
	s_ingame.motd.width					= MENU_BUTTON_MED_WIDTH;
	s_ingame.motd.height				= MENU_BUTTON_MED_HEIGHT;
	s_ingame.motd.color					= CT_DKPURPLE1;
	s_ingame.motd.color2				= CT_LTPURPLE1;
	s_ingame.motd.textX					= MENU_BUTTON_TEXT_X;
	s_ingame.motd.textY					= MENU_BUTTON_TEXT_Y;
	s_ingame.motd.textEnum				= MBT_MOTD;
	s_ingame.motd.textcolor				= CT_BLACK;
	s_ingame.motd.textcolor2			= CT_WHITE;

	y = 339; //365 //390
	x = 469;

	s_ingame.resume.generic.type			= MTYPE_BITMAP;
	s_ingame.resume.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingame.resume.generic.x				= x;
	s_ingame.resume.generic.y				= y; //y
	s_ingame.resume.generic.id				= ID_RESUME;
	s_ingame.resume.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	s_ingame.resume.generic.callback		= InGame_Event; 
	s_ingame.resume.width					= MENU_BUTTON_MED_WIDTH;
	s_ingame.resume.height					= MENU_BUTTON_MED_HEIGHT + 6;
	s_ingame.resume.color					= CT_DKPURPLE1;
	s_ingame.resume.color2					= CT_LTPURPLE1;
	s_ingame.resume.textX					= MENU_BUTTON_TEXT_X;
	s_ingame.resume.textY					= MENU_BUTTON_TEXT_Y;
	s_ingame.resume.textEnum				= MBT_INGAMERESUME;
	s_ingame.resume.textcolor				= CT_BLACK;
	s_ingame.resume.textcolor2				= CT_WHITE;

	y += 31;
	s_ingame.restart.generic.type		= MTYPE_BITMAP;
	s_ingame.restart.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingame.restart.generic.x			= x; //305
	s_ingame.restart.generic.y			= y; //y
	s_ingame.restart.generic.id			= ID_RESTART;
	s_ingame.restart.generic.name		= BUTTON_GRAPHIC_LONGRIGHT;
	s_ingame.restart.generic.callback	= InGame_Event; 
	s_ingame.restart.width				= MENU_BUTTON_MED_WIDTH;
	s_ingame.restart.height				= MENU_BUTTON_MED_HEIGHT + 6;
	s_ingame.restart.color				= CT_DKPURPLE1;
	s_ingame.restart.color2				= CT_LTPURPLE1;
	s_ingame.restart.textX				= MENU_BUTTON_TEXT_X;
	s_ingame.restart.textY				= MENU_BUTTON_TEXT_Y;
	s_ingame.restart.textEnum			= MBT_INGAMERESTART;
	s_ingame.restart.textcolor			= CT_BLACK;
	s_ingame.restart.textcolor2			= CT_WHITE;
	if( !trap_Cvar_VariableValue( "sv_running" ) ) 
	{
		s_ingame.restart.generic.flags |= QMF_GRAYED;
	}

	y += 31;
	s_ingame.leave.generic.type				= MTYPE_BITMAP;
	s_ingame.leave.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingame.leave.generic.x				= x; //126 //306
	s_ingame.leave.generic.y				= y;
	s_ingame.leave.generic.id				= ID_LEAVEARENA;
	s_ingame.leave.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_ingame.leave.generic.callback			= InGame_Event; 
	s_ingame.leave.width					= MENU_BUTTON_MED_WIDTH;
	s_ingame.leave.height					= MENU_BUTTON_MED_HEIGHT + 6;
	s_ingame.leave.color					= CT_VDKRED1;
	s_ingame.leave.color2					= CT_DKRED1;
	s_ingame.leave.textX					= MENU_BUTTON_TEXT_X;
	s_ingame.leave.textY					= MENU_BUTTON_TEXT_Y;
	s_ingame.leave.textEnum					= MBT_INGAMELEAVE;
	s_ingame.leave.textcolor				= CT_BLACK;
	s_ingame.leave.textcolor2				= CT_WHITE;

	s_ingame.screenshot.generic.type				= MTYPE_BITMAP;      
	s_ingame.screenshot.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	//s_ingame.screenshot.generic.x					= 477;
	//s_ingame.screenshot.generic.y					= 82;
	s_ingame.screenshot.generic.x					= 482;
	s_ingame.screenshot.generic.y					= 136;
	s_ingame.screenshot.generic.name				= GRAPHIC_SQUARE;
	s_ingame.screenshot.generic.id					= ID_SCREENSHOT;
	s_ingame.screenshot.generic.callback			= InGame_Event; 
	//s_ingame.screenshot.width						= MENU_BUTTON_MED_WIDTH;
	s_ingame.screenshot.width						= 107;
	//s_ingame.screenshot.height						= 36;
	s_ingame.screenshot.height						= 18;
	s_ingame.screenshot.color						= CT_DKPURPLE1;
	s_ingame.screenshot.color2						= CT_LTPURPLE1;
	s_ingame.screenshot.textX						= MENU_BUTTON_TEXT_X;
	s_ingame.screenshot.textY						= MENU_BUTTON_TEXT_Y;
	s_ingame.screenshot.textEnum					= MBT_SCREENSHOT;
	s_ingame.screenshot.textcolor					= CT_BLACK;
	s_ingame.screenshot.textcolor2					= CT_WHITE;

	s_ingame.psettings.generic.type					= MTYPE_BITMAP;
	s_ingame.psettings.generic.name					= GRAPHIC_SQUARE;
	s_ingame.psettings.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingame.psettings.generic.id					= ID_PLAYER_SETTINGS;
	s_ingame.psettings.generic.callback				= InGame_Event;
	s_ingame.psettings.generic.x					= 271;
	s_ingame.psettings.generic.y					= 284;
	s_ingame.psettings.width						= MENU_BUTTON_MED_WIDTH;
	s_ingame.psettings.height						= MENU_BUTTON_MED_HEIGHT;
	s_ingame.psettings.textcolor					= CT_BLACK;
	s_ingame.psettings.textcolor2					= CT_WHITE;
	s_ingame.psettings.color						= CT_DKPURPLE1;
	s_ingame.psettings.color2						= CT_LTPURPLE1;
	s_ingame.psettings.textX						= 5;
	s_ingame.psettings.textY						= 2;
	s_ingame.psettings.textEnum						= MBT_PLAYERDATA;

	s_ingame.pmodel.generic.type					= MTYPE_BITMAP;
	s_ingame.pmodel.generic.name					= GRAPHIC_SQUARE;
	s_ingame.pmodel.generic.flags					= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingame.pmodel.generic.id						= ID_PLAYER_MODEL;
	s_ingame.pmodel.generic.callback				= InGame_Event;
	s_ingame.pmodel.generic.x						= 454;
	s_ingame.pmodel.generic.y						= 284;
	s_ingame.pmodel.width							= MENU_BUTTON_MED_WIDTH;
	s_ingame.pmodel.height							= MENU_BUTTON_MED_HEIGHT;
	s_ingame.pmodel.color							= CT_DKPURPLE1;
	s_ingame.pmodel.color2							= CT_LTPURPLE1;
	s_ingame.pmodel.textX							= 5;
	s_ingame.pmodel.textY							= 2;
	s_ingame.pmodel.textEnum						= MBT_CHANGEMODEL;
	s_ingame.pmodel.textcolor						= CT_BLACK;
	s_ingame.pmodel.textcolor2						= CT_WHITE;

/*	s_ingame.playerName.generic.type				= MTYPE_FIELD;
	s_ingame.playerName.field.widthInChars			= MAX_NAMELENGTH;
	s_ingame.playerName.field.maxchars				= MAX_NAMELENGTH;
	s_ingame.playerName.generic.x					= 319;
	s_ingame.playerName.generic.y					= 193;
	s_ingame.playerName.field.style					= UI_SMALLFONT;
//	s_ingame.playerName.field.titleEnum				= MBT_PLAYER_NAME;
	s_ingame.playerName.field.titlecolor			= CT_LTGOLD1;
	s_ingame.playerName.field.textcolor				= CT_DKGOLD1;
	s_ingame.playerName.field.textcolor2			= CT_LTGOLD1;

	s_ingame.pClass.generic.type					= MTYPE_SPINCONTROL;
	s_ingame.pClass.generic.flags					= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingame.pClass.generic.callback				= InGame_Event;
	s_ingame.pClass.generic.id						= ID_CLASS;
	s_ingame.pClass.generic.x						= 265;
	s_ingame.pClass.generic.y						= 213;
	s_ingame.pClass.textEnum						= MBT_CLASS;
	s_ingame.pClass.textcolor						= CT_BLACK;
	s_ingame.pClass.textcolor2						= CT_WHITE;
	s_ingame.pClass.color							= CT_DKPURPLE1;
	s_ingame.pClass.color2							= CT_LTPURPLE1;
	s_ingame.pClass.listcolor						= CT_LTGOLD1;
	s_ingame.pClass.width							= 40; //80
	s_ingame.pClass.textX							= 5;
	s_ingame.pClass.textY							= 2;
	s_ingame.pClass.itemnames						= pclass_items;

	s_ingame.pRank.generic.type						= MTYPE_SPINCONTROL;
	s_ingame.pRank.generic.flags					= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingame.pRank.generic.callback					= InGame_Event;
	s_ingame.pRank.generic.id						= ID_RANK;
	s_ingame.pRank.generic.x						= 265;
	s_ingame.pRank.generic.y						= 235;
	s_ingame.pRank.textEnum							= MBT_RANK;
	s_ingame.pRank.textcolor						= CT_BLACK;
	s_ingame.pRank.textcolor2						= CT_WHITE;
	s_ingame.pRank.color							= CT_DKPURPLE1;
	s_ingame.pRank.color2							= CT_LTPURPLE1;
	s_ingame.pRank.listcolor						= CT_LTGOLD1;
	s_ingame.pRank.width							= 40;
	s_ingame.pRank.textX							= 5;
	s_ingame.pRank.textY							= 2;
	s_ingame.pRank.itemnames						= prank_items_formal;*/

	s_ingame.join.generic.type						= MTYPE_BITMAP;
	s_ingame.join.generic.flags						= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingame.join.generic.x							= 285; //126 //306
	s_ingame.join.generic.y							= 376;
	s_ingame.join.generic.id						= ID_JOIN;
	s_ingame.join.generic.name						= BUTTON_GRAPHIC_LONGRIGHT;
	s_ingame.join.generic.callback					= InGame_Event; 
	s_ingame.join.width								= MENU_BUTTON_MED_WIDTH;
	s_ingame.join.height							= MENU_BUTTON_MED_HEIGHT + 6;
	s_ingame.join.color								= CT_DKPURPLE1;
	s_ingame.join.color2							= CT_LTPURPLE1;
	s_ingame.join.textX								= MENU_BUTTON_TEXT_X;
	s_ingame.join.textY								= MENU_BUTTON_TEXT_Y;
	s_ingame.join.textEnum							= MBT_JOINMATCH;
	s_ingame.join.textcolor							= CT_BLACK;
	s_ingame.join.textcolor2						= CT_WHITE;

	s_ingame.spectate.generic.type					= MTYPE_BITMAP;
	s_ingame.spectate.generic.flags					= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingame.spectate.generic.x						= 285; //126 //306
	s_ingame.spectate.generic.y						= 342;
	s_ingame.spectate.generic.id					= ID_SPECTATE;
	s_ingame.spectate.generic.name					= BUTTON_GRAPHIC_LONGRIGHT;
	s_ingame.spectate.generic.callback				= InGame_Event; 
	s_ingame.spectate.width							= MENU_BUTTON_MED_WIDTH;
	s_ingame.spectate.height						= MENU_BUTTON_MED_HEIGHT + 6;
	s_ingame.spectate.color							= CT_DKPURPLE1;
	s_ingame.spectate.color2						= CT_LTPURPLE1;
	s_ingame.spectate.textX							= MENU_BUTTON_TEXT_X;
	s_ingame.spectate.textY							= MENU_BUTTON_TEXT_Y;
	s_ingame.spectate.textEnum						= MBT_OBSERVER;
	s_ingame.spectate.textcolor						= CT_BLACK;
	s_ingame.spectate.textcolor2					= CT_WHITE;

	s_ingame.playermdl.generic.type					= MTYPE_BITMAP;
	s_ingame.playermdl.generic.flags				= QMF_SILENT;
	s_ingame.playermdl.generic.callback				= Player_ModelEvent;
	s_ingame.playermdl.generic.ownerdraw			= IngamePlayer_DrawPlayer;
	s_ingame.playermdl.generic.x					= 86; //440
	s_ingame.playermdl.generic.y					= 158;
	s_ingame.playermdl.width						= 155; //32*6.6 //211.2
	s_ingame.playermdl.height						= 276; //56*6.6 //369.6

	team = atoi( Info_ValueForKey( info, "t" ) );
	if( team > TEAM_FREE ) {
		s_ingame.spectate.generic.flags |= QMF_GRAYED;
		s_ingame.join.generic.flags &= ~QMF_GRAYED;
	}
	else
	{
		s_ingame.spectate.generic.flags &= ~QMF_GRAYED;
		s_ingame.join.generic.flags |= QMF_GRAYED;
	}

//	Menu_AddItem( &s_ingame.menu, &s_ingame.team );
	Menu_AddItem( &s_ingame.menu, &s_ingame.addbots );
	Menu_AddItem( &s_ingame.menu, &s_ingame.removebots );

	Menu_AddItem( &s_ingame.menu, &s_ingame.emotes );
	Menu_AddItem( &s_ingame.menu, &s_ingame.admin );

//	Menu_AddItem( &s_ingame.menu, &s_ingame.teamorders );
	Menu_AddItem( &s_ingame.menu, &s_ingame.setup );
	Menu_AddItem( &s_ingame.menu, &s_ingame.server );
	Menu_AddItem( &s_ingame.menu, &s_ingame.leave );
	Menu_AddItem( &s_ingame.menu, &s_ingame.restart );
	Menu_AddItem( &s_ingame.menu, &s_ingame.resume );
//	Menu_AddItem( &s_ingame.menu, &s_ingame.quit );
	Menu_AddItem( &s_ingame.menu, &s_ingame.screenshot );

	Menu_AddItem( &s_ingame.menu, &s_ingame.psettings );
	Menu_AddItem( &s_ingame.menu, &s_ingame.pmodel );

	Menu_AddItem( &s_ingame.menu, &s_ingame.join );
	Menu_AddItem( &s_ingame.menu, &s_ingame.spectate );

	Menu_AddItem( &s_ingame.menu, &s_ingame.playermdl);

	Menu_AddItem( &s_ingame.menu, &s_ingame.motd );
	Menu_AddItem( &s_ingame.menu, &s_ingame.respawn );

//	Player_InitModel();
}

/*
=================
InGameQuit_Cache
=================
*/
void InGame_Cache( void ) 
{
	s_ingame.graphic_12_8_LU = trap_R_RegisterShaderNoMip( "menu/common/corner_ul_8_12.tga" );
	s_ingame.graphic_12_8_LL	= trap_R_RegisterShaderNoMip( "menu/common/corner_ll_8_12.tga" );
	s_ingame.graphic_12_8_RU = trap_R_RegisterShaderNoMip( "menu/common/corner_ur_8_12.tga" );
	s_ingame.graphic_12_8_RL	= trap_R_RegisterShaderNoMip( "menu/common/corner_lr_8_12.tga" );
}

/*
=================
UI_InGameMenu
=================
*/
void UI_InGameMenu( void ) 
{
	// force as top level menu
	uis.menusp = 0;  

	// set menu cursor to a nice location
	uis.cursorx = 319;
	uis.cursory = 80;

	ingameFlag = qtrue;	// true when ingame menu is in use
	Mouse_Show();

	InGame_MenuInit();
	UI_PushMenu( &s_ingame.menu );
}



/*
=================
InGameQuitMenu_Event
=================
*/
void InGameQuitMenu_Event( void *ptr, int notification ) 
{
	if( notification != QM_ACTIVATED ) 
	{
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) 
	{
	case ID_INGAME_QUIT_NO:
	case ID_INGAMEMENU:
		UI_PopMenu();
		return;
	case ID_INGAME_QUIT_YES:
		UI_PopMenu();
		UI_QuitMenu();
		return;
	}
}

/*
=================
UI_InGameQuitMenu_Draw
=================
*/
static void UI_InGameQuitMenu_Draw( void ) 
{
	UI_MenuFrame(&s_ingamequit.menu);

	// Rounded button that takes place of INGAME MENU button
	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic( 482, 136,  MENU_BUTTON_MED_WIDTH - 22, MENU_BUTTON_MED_HEIGHT, uis.whiteShader);
	UI_DrawHandlePic( 460 + MENU_BUTTON_MED_WIDTH -6 , 136,  -19,  MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);	//right

	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30,203,  47, 186, uis.whiteShader);	// Long left column square on bottom 3rd

	// standard menu drawing
	Menu_Draw( &s_ingamequit.menu );

}

/*
=================
InGameQuitMenu_Init
=================
*/
void InGameQuitMenu_Init( void ) 
{

	memset( &s_ingame, 0 ,sizeof(ingamemenu_t) );

	InGame_Cache();

	s_ingamequit.menu.wrapAround				= qtrue;
	s_ingamequit.menu.fullscreen				= qtrue;
	s_ingamequit.menu.descX						= MENU_DESC_X;
	s_ingamequit.menu.descY						= MENU_DESC_Y;
	s_ingamequit.menu.draw						= UI_InGameQuitMenu_Draw;
	s_ingamequit.menu.titleX					= MENU_TITLE_X;
	s_ingamequit.menu.titleY					= MENU_TITLE_Y;
	s_ingamequit.menu.titleI					= MNT_INGAMEMAIN_TITLE;
	s_ingamequit.menu.footNoteEnum				= MNT_INGAME_MENU;

	s_ingamequit.ingamemenu.generic.type		= MTYPE_BITMAP;
	s_ingamequit.ingamemenu.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingamequit.ingamemenu.generic.x			= 482;
	s_ingamequit.ingamemenu.generic.y			= 136;
	s_ingamequit.ingamemenu.generic.id			= ID_INGAMEMENU;
	s_ingamequit.ingamemenu.generic.name		= GRAPHIC_SQUARE;
	s_ingamequit.ingamemenu.generic.callback	= InGameQuitMenu_Event; 
	s_ingamequit.ingamemenu.width				= MENU_BUTTON_MED_WIDTH;
	s_ingamequit.ingamemenu.height				= MENU_BUTTON_MED_HEIGHT;
	s_ingamequit.ingamemenu.color				= CT_DKPURPLE1;
	s_ingamequit.ingamemenu.color2				= CT_LTPURPLE1;
	s_ingamequit.ingamemenu.textX				= MENU_BUTTON_TEXT_X;
	s_ingamequit.ingamemenu.textY				= MENU_BUTTON_TEXT_Y;
	s_ingamequit.ingamemenu.textEnum			= MBT_INGAMEMENU;
	s_ingamequit.ingamemenu.textcolor			= CT_BLACK;
	s_ingamequit.ingamemenu.textcolor2			= CT_WHITE;

	s_ingamequit.no.generic.type				= MTYPE_BITMAP;
	s_ingamequit.no.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingamequit.no.generic.x					= 100;
	s_ingamequit.no.generic.y					= 180;
	s_ingamequit.no.generic.id					= ID_INGAME_QUIT_NO;
	s_ingamequit.no.generic.name				= GRAPHIC_SQUARE;
	s_ingamequit.no.generic.callback			= InGameQuitMenu_Event; 
	s_ingamequit.no.width						= MENU_BUTTON_MED_WIDTH;
	s_ingamequit.no.height						= MENU_BUTTON_MED_HEIGHT;
	s_ingamequit.no.color						= CT_DKPURPLE1;
	s_ingamequit.no.color2						= CT_LTPURPLE1;
	s_ingamequit.no.textX						= MENU_BUTTON_TEXT_X;
	s_ingamequit.no.textY						= MENU_BUTTON_TEXT_Y;
	s_ingamequit.no.textEnum					= MBT_QUIT_NO;
	s_ingamequit.no.textcolor					= CT_BLACK;
	s_ingamequit.no.textcolor2					= CT_WHITE;	

	s_ingamequit.yes.generic.type				= MTYPE_BITMAP;
	s_ingamequit.yes.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_ingamequit.yes.generic.x					= 100;
	s_ingamequit.yes.generic.y					= 180;
	s_ingamequit.yes.generic.id					= ID_INGAME_QUIT_YES;
	s_ingamequit.yes.generic.name				= GRAPHIC_SQUARE;
	s_ingamequit.yes.generic.callback			= InGameQuitMenu_Event; 
	s_ingamequit.yes.width						= MENU_BUTTON_MED_WIDTH;
	s_ingamequit.yes.height						= MENU_BUTTON_MED_HEIGHT;
	s_ingamequit.yes.color						= CT_DKPURPLE1;
	s_ingamequit.yes.color2						= CT_LTPURPLE1;
	s_ingamequit.yes.textX						= MENU_BUTTON_TEXT_X;
	s_ingamequit.yes.textY						= MENU_BUTTON_TEXT_Y;
	s_ingamequit.yes.textEnum					= MBT_QUIT_YES;
	s_ingamequit.yes.textcolor					= CT_BLACK;
	s_ingamequit.yes.textcolor2					= CT_WHITE;

	Menu_AddItem( &s_ingamequit.menu, &s_ingamequit.ingamemenu );
	Menu_AddItem( &s_ingamequit.menu, &s_ingamequit.no );
	Menu_AddItem( &s_ingamequit.menu, &s_ingamequit.yes );
}

/*
=================
UI_InGameQuitMenu
=================
*/
void UI_InGameQuitMenu( void ) 
{
	InGameQuitMenu_Init();

	UI_PushMenu( &s_ingamequit.menu );
}
