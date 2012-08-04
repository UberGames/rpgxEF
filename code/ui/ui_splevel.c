// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=============================================================================

SINGLE PLAYER LEVEL SELECT MENU

=============================================================================
*/

//RPG-X: TiM - Removed for RPG-X as serves no purpose, and is wasting resources

#include "ui_local.h"
//
//typedef struct 
//{
//	menuframework_s	menu;
//	menubitmap_s	mainmenu;
//	menubitmap_s	preset;
//	menubitmap_s	create;
//
//} spchoose_t;
//static spchoose_t s_spchoose;
//
//
//#define ART_LEVELFRAME_FOCUS		"menu/art/maps_select"
//#define ART_LEVELFRAME_SELECTED		"menu/art/maps_selected"
//#define ART_MAP_UNKNOWN				"levelshots/unknownmap"
//#define ART_MAP_COMPLETE1			"menu/art/level_complete1"
//#define ART_MAP_COMPLETE2			"menu/art/level_complete2"
//#define ART_MAP_COMPLETE3			"menu/art/level_complete3"
//#define ART_MAP_COMPLETE4			"menu/art/level_complete4"
//#define ART_MAP_COMPLETE5			"menu/art/level_complete5"
//
//#define ID_LEFTARROW		10
//#define ID_PICTURE0			11
//#define ID_PICTURE1			12
//#define ID_PICTURE2			13
//#define ID_PICTURE3			14
//#define ID_RIGHTARROW		15
//#define ID_PLAYERPIC		16
//#define ID_AWARD1			17
//#define ID_AWARD2			18
//#define ID_AWARD3			19
//#define ID_AWARD4			20
//#define ID_AWARD5			21
//#define ID_AWARD6			22
//#define ID_AWARD7			23
//#define ID_AWARD8			24
//#define ID_AWARD9			25
//#define ID_RESET			26
//#define ID_CUSTOM			27
//#define ID_NEXT				28
//#define ID_MAINMENU			100
//#define ID_PLAYERINFO		112
//#define ID_NO				120
//#define ID_YES				121
//#define ID_PRESET			122
//#define ID_BACK				123
//#define ID_ADVANCED			124
//
//#define AWARDS_Y			(315)
//
//#define MAX_ITEM_MAPS 4
//
//#define MAX_LONGNAME		24
//
//typedef struct 
//{
//	menuframework_s	menu;
//
//	menubitmap_s	mainmenu;
//	menubitmap_s	back;
//	menubitmap_s	create;
//	menubitmap_s	fight;
//	menubitmap_s	item_leftarrow;
//	menubitmap_s	item_maps[MAX_ITEM_MAPS];
//	menubitmap_s	item_rightarrow;
//	menubitmap_s	item_player;
//	menubitmap_s	item_awards[6];
//	menubitmap_s	item_null;
//	menubitmap_s	item_reset;
//	menubitmap_s	playerinfo;
//	menubitmap_s	advanced;
//
//	qboolean		reinit;
//
//	const char *	selectedArenaInfo;
//	int				numMaps;
//	char			levelPicNames[4][MAX_QPATH];
//	char			levelNames[4][16];
//	char			levelLongNames[4][MAX_LONGNAME];
//	int				levelScores[MAX_ITEM_MAPS];
//	int				levelScoresSkill[4];
//	qhandle_t		levelSelectedPic;
//	qhandle_t		levelFocusPic;
//	qhandle_t		levelCompletePic[5];
//
//	qhandle_t		corner_ul_18_12;
//	qhandle_t		corner_ur_18_12;
//	qhandle_t		corner_ll_18_34;
//	qhandle_t		corner_lr_7_12;
//
//	char			playerModel[MAX_QPATH];
//	char			playerPicName[MAX_QPATH];
//	int				awardLevels[6];
//	sfxHandle_t		awardSounds[6];
//
//	int				numBots;
//	qhandle_t		botPics[7];
//	char			botNames[7][10];
//} levelMenuInfo_t;
//
//static levelMenuInfo_t	levelMenuInfo;
//
////=========================================================================================
//// Reset Menu Struct
////=========================================================================================
//typedef struct 
//{
//	menuframework_s	menu;
//	menubitmap_s	mainmenu;
//	menubitmap_s	back;
//	menubitmap_s	yes;
//
//	qhandle_t		corner_ul;
//	qhandle_t		corner_ur;
//	qhandle_t		corner_ll;
//	qhandle_t		corner_lr;
//
//} resetGameMenuInfo_t;
//
//static resetGameMenuInfo_t	resetGameMenuInfo;
//
//
//void UI_ResetGameMenu( void );
//
//static int	selectedArenaSet;
//static int	selectedArena;
//static int	currentSet;
//static int	currentGame;
//static int	trainingTier;
//static int	finalTier;
//static int	minTier;
//static int	maxTier;
//
//
///*
//=================
//PlayerIcon
//=================
//*/
void PlayerIcon( const char *modelAndSkin, char *iconName, int iconNameMaxSize ) {
	char	*skin;
	char	model[MAX_QPATH];

	Q_strncpyz( model, modelAndSkin, sizeof(model));
	skin = Q_strrchr( model, '/' );
	if ( skin ) {
		*skin++ = '\0';
	}
	else {
		skin = "default";
	}

	//TiM - Removed skin and added RPG-X parms
	Com_sprintf(iconName, iconNameMaxSize, "models/players_rpgx/%s/model_icon.jpg", model );

	if( !trap_R_RegisterShaderNoMip( iconName ) && Q_stricmp( skin, "default" ) != 0 ) {
		Com_sprintf(iconName, iconNameMaxSize, "models/players_rpgx/%s/model_icon.jpg", model );
	}
}
//
//
///*
//=================
//PlayerIconhandle
//=================
//*/
//static qhandle_t PlayerIconHandle( const char *modelAndSkin ) 
//{
//	char	iconName[MAX_QPATH];
//
//	PlayerIcon( modelAndSkin, iconName, sizeof(iconName) );
//	return trap_R_RegisterShaderNoMip( iconName );
//}
//
//
///*
//=================
//UI_SPLevelMenu_SetBots
//=================
//*/
//static void UI_SPLevelMenu_SetBots( void ) 
//{
//	char	*p;
//	char	*bot;
//	char	*botInfo;
//	char	bots[MAX_INFO_STRING];
//
//	levelMenuInfo.numBots = 0;
//
//	Q_strncpyz( bots, Info_ValueForKey( levelMenuInfo.selectedArenaInfo, "bots" ), sizeof(bots) );
//
//	p = &bots[0];
//	while( *p && levelMenuInfo.numBots < 7 ) 
//	{
//		//skip spaces
//		while( *p && *p == ' ' ) {
//			p++;
//		}
//		if( !p ) {
//			break;
//		}
//
//		// mark start of bot name
//		bot = p;
//
//		// skip until space of null
//		while( *p && *p != ' ' ) 
//		{
//			p++;
//		}
//		if( *p ) {
//			*p++ = 0;
//		}
//
//		botInfo = UI_GetBotInfoByName( bot );
//		if( botInfo ) 
//		{
//			levelMenuInfo.botPics[levelMenuInfo.numBots] = PlayerIconHandle( Info_ValueForKey( botInfo, "model" ) );
//			Q_strncpyz( levelMenuInfo.botNames[levelMenuInfo.numBots], Info_ValueForKey( botInfo, "name" ), 10 );
//		}
//		else 
//		{
//			levelMenuInfo.botPics[levelMenuInfo.numBots] = 0;
//			Q_strncpyz( levelMenuInfo.botNames[levelMenuInfo.numBots], bot, 10 );
//		}
//		Q_CleanStr( levelMenuInfo.botNames[levelMenuInfo.numBots] );
//		levelMenuInfo.numBots++;
//	}
//}
//
//
///*
//=================
//UI_SPLevelMenu_SetMenuArena
//=================
//*/
//extern vmCvar_t	ui_language;
//static void UI_SPLevelMenu_SetMenuArena( int n, int level, const char *arenaInfo ) 
//{
//	char		map[MAX_QPATH];
//	char			string[MAX_LONGNAME];
//
//	Q_strncpyz( map, Info_ValueForKey( arenaInfo, "map" ), sizeof(map) );
//
//	Q_strncpyz( levelMenuInfo.levelNames[n], map, sizeof(levelMenuInfo.levelNames[n]) );
//	Q_strupr( levelMenuInfo.levelNames[n] );
//
//	if (ui_language.string[0] == 0 || Q_stricmp ("ENGLISH",ui_language.string)==0 ) {
//		Q_strncpyz( string, Info_ValueForKey( arenaInfo, "longname" ), sizeof(string) );
//	} else {
//		Q_strncpyz( string, Info_ValueForKey( arenaInfo, va("longname_%s",ui_language.string) ), sizeof(string) );
//		if (!string[0]) {
//			Q_strncpyz( string, Info_ValueForKey( arenaInfo, "longname" ), sizeof(string) );
//		}
//	}
//	Q_strncpyz( levelMenuInfo.levelLongNames[n], string, sizeof(levelMenuInfo.levelLongNames[n]) );
//	Q_strupr( levelMenuInfo.levelLongNames[n] );
//
//
//	UI_GetBestScore( level, &levelMenuInfo.levelScores[n], &levelMenuInfo.levelScoresSkill[n] );
//	if( levelMenuInfo.levelScores[n] > 8 ) 
//	{
//		levelMenuInfo.levelScores[n] = 8;
//	}
//
//	strcpy( levelMenuInfo.levelPicNames[n], va( "levelshots/%s.tga", map ) );
//	if( !trap_R_RegisterShaderNoMip( levelMenuInfo.levelPicNames[n] ) ) 
//	{
//		strcpy( levelMenuInfo.levelPicNames[n], ART_MAP_UNKNOWN );
//	}
//
//	levelMenuInfo.item_maps[n].shader = 0;
//	levelMenuInfo.item_maps[n].generic.flags &= ~QMF_HIDDEN;
//	levelMenuInfo.item_maps[n].generic.flags &= ~QMF_INACTIVE;
//}
//
///*
//=================
//UI_SPLevelMenu_SetMenuItems
//=================
//*/
//static void UI_SPLevelMenu_SetMenuItems( void ) 
//{
//	int			n,i;
//	int			level;
//	const char	*arenaInfo;
//	char		*type;
//
//	if ( selectedArena == -1 ) 
//	{
//		selectedArena = 0;
//	}
//
////	if( selectedArenaSet == trainingTier || selectedArenaSet == finalTier ) 
////	{
////		selectedArena = 0;
////	}
//
//	if( selectedArena != -1 ) 
//	{
//		trap_Cvar_SetValue( "ui_spSelection", selectedArenaSet * ARENAS_PER_TIER + selectedArena );
//	}
//
//	if( selectedArenaSet == trainingTier ) 
//	{
//		arenaInfo = UI_GetSpecialArenaInfo( "training" );
//		level = atoi( Info_ValueForKey( arenaInfo, "num" ) );
//		UI_SPLevelMenu_SetMenuArena( 0, level, arenaInfo );
//		levelMenuInfo.selectedArenaInfo = arenaInfo;
//
////		levelMenuInfo.item_maps[0].generic.x = 256;
//		Bitmap_Init( &levelMenuInfo.item_maps[0] );
//		levelMenuInfo.item_maps[0].generic.bottom += 32;
//		levelMenuInfo.numMaps = 1;
//
//		levelMenuInfo.item_maps[1].generic.flags |= QMF_INACTIVE;
//		levelMenuInfo.item_maps[2].generic.flags |= QMF_INACTIVE;
//		levelMenuInfo.item_maps[3].generic.flags |= QMF_INACTIVE;
//		levelMenuInfo.levelPicNames[1][0] = 0;
//		levelMenuInfo.levelPicNames[2][0] = 0;
//		levelMenuInfo.levelPicNames[3][0] = 0;
//		levelMenuInfo.item_maps[1].shader = 0;
//		levelMenuInfo.item_maps[2].shader = 0;
//		levelMenuInfo.item_maps[3].shader = 0;
//	}
//	else if( selectedArenaSet == finalTier ) 
//	{
//		arenaInfo = UI_GetSpecialArenaInfo( "final" );
//		level = atoi( Info_ValueForKey( arenaInfo, "num" ) );
//		UI_SPLevelMenu_SetMenuArena( 0, level, arenaInfo );
//		levelMenuInfo.selectedArenaInfo = arenaInfo;
//
////		levelMenuInfo.item_maps[0].generic.x = 256;
//		Bitmap_Init( &levelMenuInfo.item_maps[0] );
//		levelMenuInfo.item_maps[0].generic.bottom += 32;
//		levelMenuInfo.numMaps = 1;
//
//		levelMenuInfo.item_maps[1].generic.flags |= QMF_INACTIVE;
//		levelMenuInfo.item_maps[2].generic.flags |= QMF_INACTIVE;
//		levelMenuInfo.item_maps[3].generic.flags |= QMF_INACTIVE;
//		levelMenuInfo.levelPicNames[1][0] = 0;
//		levelMenuInfo.levelPicNames[2][0] = 0;
//		levelMenuInfo.levelPicNames[3][0] = 0;
//		levelMenuInfo.item_maps[1].shader = 0;
//		levelMenuInfo.item_maps[2].shader = 0;
//		levelMenuInfo.item_maps[3].shader = 0;
//	}
//	else 
//	{
//		Bitmap_Init( &levelMenuInfo.item_maps[0] );
//		levelMenuInfo.item_maps[0].generic.bottom += 18;
//
//		memset( levelMenuInfo.levelNames,0, sizeof(levelMenuInfo.levelNames));
//
//		// Zero out info
//		for ( n = 0; n < MAX_ITEM_MAPS; n++ ) 
//		{
//			levelMenuInfo.item_maps[n].generic.flags |= QMF_HIDDEN | QMF_INACTIVE;
//			levelMenuInfo.levelScores[n] = 0;
//		}
//
//		i = 0;
//		for ( n = 0; n < MAX_ITEM_MAPS; n++ ) 
//		{
//			level = selectedArenaSet * ARENAS_PER_TIER + n;
//			arenaInfo = UI_GetArenaInfoByNumber( level );
//			type = Info_ValueForKey( arenaInfo, "type" );
//			if( strstr( type, "single" ) ) 
//			{
//				UI_SPLevelMenu_SetMenuArena( n, level, arenaInfo );
//				i++;
//			}
//		}
//
//		levelMenuInfo.numMaps = i;
//
//		if( selectedArena != -1 ) 
//		{
//			levelMenuInfo.selectedArenaInfo = UI_GetArenaInfoByNumber( selectedArenaSet * ARENAS_PER_TIER + selectedArena );
//		}
//	}
//
//	// enable/disable arrows when they are valid/invalid
//	if ( selectedArenaSet == minTier ) 
//	{
//		levelMenuInfo.item_leftarrow.generic.flags |= ( QMF_INACTIVE | QMF_GRAYED );
//	}
//	else 
//	{
//		levelMenuInfo.item_leftarrow.generic.flags &= ~( QMF_INACTIVE | QMF_GRAYED );
//	}
//
//	if ( selectedArenaSet >= maxTier ) 
//	{
//		levelMenuInfo.item_rightarrow.generic.flags |= ( QMF_INACTIVE | QMF_GRAYED );
//	}
//	else 
//	{
//		levelMenuInfo.item_rightarrow.generic.flags &= ~( QMF_INACTIVE | QMF_GRAYED );
//	}
//
//
//	UI_SPLevelMenu_SetBots();
//}
//
//
///*
//=================
//UI_SPLevelMenu_ResetAction
//=================
//*/
//static void UI_SPLevelMenu_ResetAction( qboolean result ) 
//{
//	if( !result ) 
//	{
//		return;
//	}
//
//	// clear game variables
//	UI_NewGame();
//	trap_Cvar_SetValue( "ui_spSelection", 0 );
//
//	// make the level select menu re-initialize
//	UI_PopMenu();
//	UI_SPLevelMenu();
//}
//
///*
//=================
//UI_SPLevelMenu_ResetEvent
//=================
//*/
//static void UI_SPLevelMenu_ResetEvent( void* ptr, int event )
//{
//	if (event != QM_ACTIVATED) 
//	{
//		return;
//	}
//
//	UI_ResetGameMenu();
//
//}
//
//
///*
//=================
//UI_SPLevelMenu_LevelEvent
//=================
//*/
//static void UI_SPLevelMenu_LevelEvent( void* ptr, int notification ) 
//{
//	if (notification != QM_ACTIVATED) 
//	{
//		return;
//	}
//
//	selectedArena = ((menucommon_s*)ptr)->id - ID_PICTURE0;
//	levelMenuInfo.selectedArenaInfo = UI_GetArenaInfoByNumber( selectedArenaSet * ARENAS_PER_TIER + selectedArena );
//	UI_SPLevelMenu_SetBots();
//
//	trap_Cvar_SetValue( "ui_spSelection", selectedArenaSet * ARENAS_PER_TIER + selectedArena );
//}
//
//
///*
//=================
//UI_SPLevelMenu_LeftArrowEvent
//=================
//*/
//static void UI_SPLevelMenu_LeftArrowEvent( void* ptr, int notification ) 
//{
//	if (notification != QM_ACTIVATED) 
//	{
//		return;
//	}
//
//	if ( selectedArenaSet == minTier ) 
//	{
//		return;
//	}
//
//	selectedArena = 0;
//	selectedArenaSet--;
//	UI_SPLevelMenu_SetMenuItems();
//}
//
//
///*
//=================
//UI_SPLevelMenu_RightArrowEvent
//=================
//*/
//static void UI_SPLevelMenu_RightArrowEvent( void* ptr, int notification ) 
//{
//	if (notification != QM_ACTIVATED) 
//	{
//		return;
//	}
//
//	if ( selectedArenaSet == maxTier ) 
//	{
//		return;
//	}
//
//	selectedArena = 0;
//	selectedArenaSet++;
//	UI_SPLevelMenu_SetMenuItems();
//}
//
//
///*
//=================
//UI_SPLevelMenu_PlayerEvent
//=================
//*/
//static void UI_SPLevelMenu_PlayerEvent( void* ptr, int notification ) 
//{
//	if (notification != QM_ACTIVATED) 
//	{
//		return;
//	}
//
//	UI_PlayerSettingsMenu(PS_MENU_SINGLEPLAYER);
//}
//
///*
//=================
//UI_SPLevelMenu_CreateEvent
//=================
//*/
//static void UI_SPLevelMenu_CreateEvent( void* ptr, int notification ) 
//{
//	if (notification != QM_ACTIVATED) 
//	{
//		return;
//	}
//	UI_StartServerMenu( qfalse );
//}
//
//
///*
//=================
//UI_SPLevelMenu_AdvancedEvent
//=================
//*/
//static void UI_SPLevelMenu_AdvancedEvent( void* ptr, int notification ) 
//{
//	if (notification != QM_ACTIVATED) 
//	{
//		return;
//	}
//
//	switch (((menucommon_s*)ptr)->id)
//	{
//		case ID_ADVANCED:
//			UI_ServerAdvancedOptions(1);
//			break;
//	}
//}
//
//
///*
//=================
//UI_SPLevelMenu_AwardEvent
//=================
//*/
//static void UI_SPLevelMenu_AwardEvent( void* ptr, int notification ) 
//{
//	int		n;
//
//	if (notification != QM_ACTIVATED) 
//	{
//		return;
//	}
//
//	n = ((menucommon_s*)ptr)->id - ID_AWARD1;
//	trap_S_StartLocalSound( levelMenuInfo.awardSounds[n], CHAN_ANNOUNCER );
//}
//
//
///*
//=================
//UI_SPLevelMenu_NextEvent
//=================
//*/
//static void UI_SPLevelMenu_NextEvent( void* ptr, int notification ) 
//{
//	if (notification != QM_ACTIVATED) 
//	{
//		return;
//	}
//
//	if ( selectedArena == -1 ) 
//	{
//		selectedArena = 0;
//	}
//
//	UI_SPSkillMenu( levelMenuInfo.selectedArenaInfo );
//}
//
//
///*
//=================
//UI_SPLevelMenu_BackEvent
//=================
//*/
//static void UI_SPLevelMenu_BackEvent( void* ptr, int notification ) 
//{
//	if (notification != QM_ACTIVATED) 
//	{
//		return;
//	}
//
//	if ( selectedArena == -1 ) 
//	{
//		selectedArena = 0;
//	}
//
//	switch (((menucommon_s*)ptr)->id)
//	{
//		case ID_MAINMENU:
//			UI_PopMenu ();
//			UI_MainMenu();
//			break;
//
//		case ID_BACK:
//			UI_PopMenu ();
//			break;
//	}
//}
//
///*
//=================
//UI_SPLevelMenu_MenuDraw
//=================
//*/
//#define LEVEL_DESC_LEFT_MARGIN		332
//#define MAP_WIDTH 118
//
//static void UI_SPLevelMenu_MenuDraw( void ) 
//{
//	int				n;//, i;
//	int				x, y;
//	vec4_t			color;
////	int				level;
//	int				fraglimit;
//	int				pad;
//	char			buf[MAX_INFO_VALUE];
//	char			string[64];
//
//	if(	levelMenuInfo.reinit ) 
//	{
//		UI_PopMenu();
//		UI_SPLevelMenu();
//		return;
//	}
//
//	UI_MenuFrame2(&levelMenuInfo.menu);
//
//	trap_R_SetColor( colorTable[CT_LTBROWN1]);
//	UI_DrawHandlePic(  30, 203,  47,  186, uis.whiteShader);	// Left side of frame
//
//	// Map pics frame
//	trap_R_SetColor( colorTable[CT_LTPURPLE3]);
//	UI_DrawHandlePic( 84,  50,  64,  -32, levelMenuInfo.corner_ll_18_34);	// Corner at bottom
//	UI_DrawHandlePic( 84, 190,  64,  32, levelMenuInfo.corner_ll_18_34);	// Corner at bottom
//	UI_DrawHandlePic( 108, 60, 506,  18, uis.whiteShader);	// Top line 
//	UI_DrawHandlePic( 84,  85,  34, 102, uis.whiteShader);	// Left side of frame
//
//	UI_DrawHandlePic(  93, 194,  34,  18, uis.whiteShader);	// Bottom line after corner
//	UI_DrawHandlePic( 130, 194,  26,  18, uis.whiteShader);	// Line under left arrow 
//	UI_DrawHandlePic( 159, 194, 391,  18, uis.whiteShader);	// Line between next and previous buttons
//	UI_DrawHandlePic( 553, 194,  26,  18, uis.whiteShader);	// Line under right arrow 
//	UI_DrawHandlePic( 582, 194,  30,  18, uis.whiteShader);	// Line after right arrow 
//
//	UI_DrawProportionalString(  133,  61, va( (char*)menu_normal_text[MNT_LEVELS],
//		((selectedArenaSet*ARENAS_PER_TIER)+1),
//		((selectedArenaSet*ARENAS_PER_TIER)+levelMenuInfo.numMaps)),
//		UI_SMALLFONT, colorTable[CT_BLACK]);
//
//	UI_DrawProportionalString(  74,  27, "111611",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
//	UI_DrawProportionalString(  74,  188, "81454",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
//	UI_DrawProportionalString(  74,  206, "71",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
//	UI_DrawProportionalString(  74,  395, "345-5",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
//
//	// Opponents frame
//	trap_R_SetColor( colorTable[CT_DKBROWN1]);
//	UI_DrawHandlePic( 348, 221,  16,  32, levelMenuInfo.corner_ur_18_12);	// Corner at top
//	UI_DrawHandlePic( 349, 373,  16,  16, levelMenuInfo.corner_lr_7_12);	// Corner at bottom
//	UI_DrawHandlePic(  84, 221, 270,  18, uis.whiteShader);	// Top of level info frame
//	UI_DrawHandlePic( 352, 245,  12, 125, uis.whiteShader);	// Right side of level info frame
//	UI_DrawHandlePic(  83, 376, 272,   7, uis.whiteShader);	// Bottom of frame
//	UI_DrawProportionalString( 88, 223, menu_normal_text[MNT_OPPONENTS], UI_SMALLFONT, colorTable[CT_BLACK] );
//
//	// Player info frame
//	trap_R_SetColor( colorTable[CT_DKBROWN1]);
//	UI_DrawHandlePic( 367, 221,  16,  32, levelMenuInfo.corner_ul_18_12);	// Corner at top
//	UI_DrawHandlePic( 366, 373,  -16,  16, levelMenuInfo.corner_lr_7_12);	// Corner at bottom
//	UI_DrawHandlePic( 377, 221, 235,  18, uis.whiteShader);	// Top of player info frame
//	UI_DrawHandlePic( 367, 245,  12, 125, uis.whiteShader);	// Left side of frame
//	UI_DrawHandlePic( 379, 376, 233,   7, uis.whiteShader);	// Bottom of frame
//
//
//	// Draw player name
//	trap_Cvar_VariableStringBuffer( "name", string, 32 );
//	Q_CleanStr( string );
//	UI_DrawProportionalString( 390, 223, string, UI_SMALLFONT, colorTable[CT_BLACK] );
//
//	// check for model changes
//	trap_Cvar_VariableStringBuffer( "model", buf, sizeof(buf) );
//	if( Q_stricmp( buf, levelMenuInfo.playerModel ) != 0 ) 
//	{
//		Q_strncpyz( levelMenuInfo.playerModel, buf, sizeof(levelMenuInfo.playerModel) );
//		PlayerIcon( levelMenuInfo.playerModel, levelMenuInfo.playerPicName, sizeof(levelMenuInfo.playerPicName) );
//		levelMenuInfo.item_player.shader = 0;
//	}
//
//	// standard menu drawing
//	Menu_Draw( &levelMenuInfo.menu );
//
//	// draw player award levels
///*	y = AWARDS_Y;
//	i = 0;
//	for( n = 0; n < AWARD_MAX; n++ ) 
//	{
//		level = levelMenuInfo.awardLevels[n];
//		if( level > 0 ) 
//		{
//			x = 385 + (i * 38);
//			i++;
//
//			if( level == 1 ) 
//			{
//				continue;
//			}
//
//			if( level >= 1000000 ) 
//			{
//				Com_sprintf( string, sizeof(string), "%im", level / 1000000 );
//			}
//			else if( level >= 1000 ) {
//				Com_sprintf( string, sizeof(string), "%ik", level / 1000 );
//			}
//			else 
//			{
//				Com_sprintf( string, sizeof(string), "%i", level );
//			}
//
//			UI_DrawString( x + 19, y + 38, string, UI_CENTER, colorTable[CT_LTGOLD1] );
//		}
//	}
//*/
//	// Print black rectangle below map picture
//	for ( n = 0; n < levelMenuInfo.numMaps; n++ ) 
//	{
//		x = levelMenuInfo.item_maps[n].generic.x;
//		y = levelMenuInfo.item_maps[n].generic.y;
//		UI_FillRect( x, y + 87, 116, 18, colorTable[CT_BLACK] );
//	}
//
//	// show levelshots for levels of current tier
//	Vector4Copy( color_white, color );
//	color[3] = 0.5+0.5*sin(uis.realtime/PULSE_DIVISOR);
//	for ( n = 0; n < levelMenuInfo.numMaps; n++ ) 
//	{
//		x = levelMenuInfo.item_maps[n].generic.x;
//		y = levelMenuInfo.item_maps[n].generic.y;
//
//		// Map name below picture
//		UI_DrawProportionalString( x + (MAP_WIDTH/2), y + 90, levelMenuInfo.levelLongNames[n], UI_CENTER|UI_TINYFONT, colorTable[CT_WHITE] );
//
//		if( levelMenuInfo.levelScores[n] == 1 ) 
//		{
//			UI_DrawHandlePic( x, y, 48, 48, levelMenuInfo.levelCompletePic[levelMenuInfo.levelScoresSkill[n] - 1] ); 
//		}
//
//		if ( n == selectedArena ) 
//		{
//			if( Menu_ItemAtCursor( &levelMenuInfo.menu ) == &levelMenuInfo.item_maps[n] ) 
//			{
//				trap_R_SetColor( color );
//			}
//
//			UI_DrawHandlePic( x-1, y-1, MAP_WIDTH, 118 - 14, levelMenuInfo.levelSelectedPic ); 
//
//			trap_R_SetColor( NULL );
//		}
//		else if( Menu_ItemAtCursor( &levelMenuInfo.menu ) == &levelMenuInfo.item_maps[n] ) 
//		{
//			trap_R_SetColor( color );
//			UI_DrawHandlePic( x-1, y-1, MAP_WIDTH, 118 - 14, levelMenuInfo.levelSelectedPic ); 
//			trap_R_SetColor( NULL );
//		}
//	}
//
//	// show map name and long name of selected level
//	Q_strncpyz( buf, Info_ValueForKey( levelMenuInfo.selectedArenaInfo, "map" ), 20 );
//	Q_strupr( buf );
//	Com_sprintf(string, sizeof(string),"%s",buf);
//	UI_DrawProportionalString( 169, 197, string, UI_TINYFONT,  colorTable[CT_BLACK] );
//
//	fraglimit = atoi( Info_ValueForKey( levelMenuInfo.selectedArenaInfo, "fraglimit" ) );
//	UI_DrawProportionalString( 372, 197, va("%s %i", menu_normal_text[MNT_POINTLIMIT],fraglimit) , UI_TINYFONT, colorTable[CT_BLACK] );
//
//	// draw bot opponents
//	y = 242;
//	pad = 64 + 3;
//	x = 84 - pad;
//	for( n = 0; n < levelMenuInfo.numBots; n++ ) 
//	{
//		if (n == 4)	//	Next line of bad guys
//		{
//			y +=68;
//			x = 84 - pad;
//		}
//
//		x += pad;
//		if( levelMenuInfo.botPics[n] ) 
//		{
//			UI_DrawHandlePic( x, y, 64, 64, levelMenuInfo.botPics[n]);
//		}
//		else 
//		{
//			UI_FillRect( x, y, 64, 64, colorTable[CT_BLACK] );
//			UI_DrawProportionalString( x+22, y+18, "?", UI_BIGFONT, color_orange );
//		}
//
//		UI_DrawProportionalString( x, y + 54, levelMenuInfo.botNames[n], UI_TINYFONT, colorTable[CT_LTORANGE] );
//
//	}
//
//
//	// Left rounded ends for buttons
//	trap_R_SetColor( colorTable[levelMenuInfo.mainmenu.color]);
//	UI_DrawHandlePic(110 - 14, levelMenuInfo.mainmenu.generic.y, 
//		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
//
//	trap_R_SetColor( colorTable[levelMenuInfo.back.color]);
//	UI_DrawHandlePic(110 - 14, levelMenuInfo.back.generic.y, 
//		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
//
//	trap_R_SetColor( colorTable[levelMenuInfo.item_reset.color]);
//	UI_DrawHandlePic(levelMenuInfo.item_reset.generic.x - 14, levelMenuInfo.item_reset.generic.y, 
//		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
//
//	trap_R_SetColor( colorTable[levelMenuInfo.playerinfo.color]);
//	UI_DrawHandlePic(levelMenuInfo.playerinfo.generic.x - 14, levelMenuInfo.playerinfo.generic.y, 
//		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
//
//	trap_R_SetColor( colorTable[levelMenuInfo.create.color]);
//	UI_DrawHandlePic(levelMenuInfo.create.generic.x - 14, levelMenuInfo.create.generic.y, 
//		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
//
//	trap_R_SetColor( colorTable[levelMenuInfo.advanced.color]);
//	UI_DrawHandlePic(levelMenuInfo.advanced.generic.x - 14, levelMenuInfo.advanced.generic.y, 
//		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
//
//
//}
//
//
///*
//=================
//UI_SPLevelMenu_Cache
//=================
//*/
//void UI_SPLevelMenu_Cache( void ) 
//{
//	int				n;
//
//	trap_R_RegisterShaderNoMip( ART_MAP_UNKNOWN );
//
//	for( n = 0; n < AWARD_MAX; n++ ) 
//	{
//		trap_R_RegisterShaderNoMip( ui_medalPicNames[n] );
//		levelMenuInfo.awardSounds[n] = trap_S_RegisterSound( ui_medalSounds[n] );
//	}
//
//	levelMenuInfo.corner_ul_18_12 = trap_R_RegisterShaderNoMip("menu/common/corner_ul_18_12");
//	levelMenuInfo.corner_ur_18_12 = trap_R_RegisterShaderNoMip("menu/common/corner_ur_18_12");
//	levelMenuInfo.corner_lr_7_12  = trap_R_RegisterShaderNoMip("menu/common/corner_lr_7_12");
//	levelMenuInfo.corner_ll_18_34 = trap_R_RegisterShaderNoMip("menu/common/corner_ll_18_34");
//
//	levelMenuInfo.levelSelectedPic = trap_R_RegisterShaderNoMip( ART_LEVELFRAME_SELECTED );
//	levelMenuInfo.levelFocusPic = trap_R_RegisterShaderNoMip( ART_LEVELFRAME_FOCUS );
//	levelMenuInfo.levelCompletePic[0] = trap_R_RegisterShaderNoMip( ART_MAP_COMPLETE1 );
//	levelMenuInfo.levelCompletePic[1] = trap_R_RegisterShaderNoMip( ART_MAP_COMPLETE2 );
//	levelMenuInfo.levelCompletePic[2] = trap_R_RegisterShaderNoMip( ART_MAP_COMPLETE3 );
//	levelMenuInfo.levelCompletePic[3] = trap_R_RegisterShaderNoMip( ART_MAP_COMPLETE4 );
//	levelMenuInfo.levelCompletePic[4] = trap_R_RegisterShaderNoMip( ART_MAP_COMPLETE5 );
//
//}
//
//
///*
//=================
//UI_SPLevelMenu_Init
//=================
//*/
//static void UI_SPLevelMenu_Init( void ) 
//{
//	int		skill;
//	int		n;
//	int		x, y;
//	int		count;
//	char	buf[MAX_QPATH];
//	int mapHeight,mapWidth;
//
//
//	// What skill level are we playing at??
//	skill = (int)trap_Cvar_VariableValue( "g_spSkill" );
//	if( skill < 1 || skill > 5 ) 
//	{
//		trap_Cvar_Set( "g_spSkill", "2" );
//		skill = 2;
//	}
//
//	memset( &levelMenuInfo, 0, sizeof(levelMenuInfo) );
//	levelMenuInfo.menu.fullscreen					= qtrue;
//	levelMenuInfo.menu.wrapAround					= qtrue;
//	levelMenuInfo.menu.draw							= UI_SPLevelMenu_MenuDraw;
//	levelMenuInfo.menu.descX						= MENU_DESC_X;
//	levelMenuInfo.menu.descY						= MENU_DESC_Y;
//	levelMenuInfo.menu.titleX						= MENU_TITLE_X;
//	levelMenuInfo.menu.titleY						= MENU_TITLE_Y;
//	levelMenuInfo.menu.titleI						= MNT_SINGLEPLAYER_TITLE;
//	levelMenuInfo.menu.footNoteEnum					= MNT_SINGLEPLAYER;
//
//	UI_SPLevelMenu_Cache();
//
//	levelMenuInfo.mainmenu.generic.type				= MTYPE_BITMAP;      
//	levelMenuInfo.mainmenu.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
//	levelMenuInfo.mainmenu.generic.x				= 110;
//	levelMenuInfo.mainmenu.generic.y				= 391;
//	levelMenuInfo.mainmenu.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
//	levelMenuInfo.mainmenu.generic.id				= ID_MAINMENU;
//	levelMenuInfo.mainmenu.generic.callback			= UI_SPLevelMenu_BackEvent;
//	levelMenuInfo.mainmenu.width					= MENU_BUTTON_MED_WIDTH;
//	levelMenuInfo.mainmenu.height					= MENU_BUTTON_MED_HEIGHT;
//	levelMenuInfo.mainmenu.color					= CT_DKPURPLE1;
//	levelMenuInfo.mainmenu.color2					= CT_LTPURPLE1;
//	levelMenuInfo.mainmenu.textX					= MENU_BUTTON_TEXT_X;
//	levelMenuInfo.mainmenu.textY					= MENU_BUTTON_TEXT_Y;
//	levelMenuInfo.mainmenu.textEnum					= MBT_MAINMENU;
//	levelMenuInfo.mainmenu.textcolor				= CT_BLACK;
//	levelMenuInfo.mainmenu.textcolor2				= CT_WHITE;
//
//	levelMenuInfo.back.generic.type				= MTYPE_BITMAP;      
//	levelMenuInfo.back.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
//	levelMenuInfo.back.generic.x				= 110;
//	levelMenuInfo.back.generic.y				= 415;
//	levelMenuInfo.back.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
//	levelMenuInfo.back.generic.id				= ID_BACK;
//	levelMenuInfo.back.generic.callback			= UI_SPLevelMenu_BackEvent;
//	levelMenuInfo.back.width					= MENU_BUTTON_MED_WIDTH;
//	levelMenuInfo.back.height					= MENU_BUTTON_MED_HEIGHT;
//	levelMenuInfo.back.color					= CT_DKPURPLE1;
//	levelMenuInfo.back.color2					= CT_LTPURPLE1;
//	levelMenuInfo.back.textX					= MENU_BUTTON_TEXT_X;
//	levelMenuInfo.back.textY					= MENU_BUTTON_TEXT_Y;
//	levelMenuInfo.back.textEnum					= MBT_BACK;
//	levelMenuInfo.back.textcolor				= CT_BLACK;
//	levelMenuInfo.back.textcolor2				= CT_WHITE;
//
//	levelMenuInfo.item_leftarrow.generic.type		= MTYPE_BITMAP;
//	levelMenuInfo.item_leftarrow.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
//	levelMenuInfo.item_leftarrow.generic.name		= "menu/common/arrow_left_16.tga";
//	levelMenuInfo.item_leftarrow.generic.x			= 135;
//	levelMenuInfo.item_leftarrow.generic.y			= 195;
//	levelMenuInfo.item_leftarrow.generic.callback	= UI_SPLevelMenu_LeftArrowEvent;
//	levelMenuInfo.item_leftarrow.generic.id			= ID_LEFTARROW;
//	levelMenuInfo.item_leftarrow.width				= 16;
//	levelMenuInfo.item_leftarrow.height				= 16;
//	levelMenuInfo.item_leftarrow.color				= CT_DKGOLD1;
//	levelMenuInfo.item_leftarrow.color2				= CT_LTGOLD1;
//	levelMenuInfo.item_leftarrow.textX				= MENU_BUTTON_TEXT_X;
//	levelMenuInfo.item_leftarrow.textY				= MENU_BUTTON_TEXT_Y;
//	levelMenuInfo.item_leftarrow.textEnum			= MBT_PREVMAPS;
//	levelMenuInfo.item_leftarrow.textcolor			= CT_BLACK;
//	levelMenuInfo.item_leftarrow.textcolor2			= CT_WHITE;
//
//	levelMenuInfo.item_rightarrow.generic.type		= MTYPE_BITMAP;
//	levelMenuInfo.item_rightarrow.generic.name		= "menu/common/arrow_right_16.tga";
//	levelMenuInfo.item_rightarrow.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
//	levelMenuInfo.item_rightarrow.generic.x			= 558;
//	levelMenuInfo.item_rightarrow.generic.y			= 195;
//	levelMenuInfo.item_rightarrow.generic.callback	= UI_SPLevelMenu_RightArrowEvent;
//	levelMenuInfo.item_rightarrow.generic.id		= ID_RIGHTARROW;
//	levelMenuInfo.item_rightarrow.width				= 16;
//	levelMenuInfo.item_rightarrow.height			= 16;
//	levelMenuInfo.item_rightarrow.color				= CT_DKGOLD1;
//	levelMenuInfo.item_rightarrow.color2			= CT_LTGOLD1;
//	levelMenuInfo.item_rightarrow.textX				= MENU_BUTTON_TEXT_X;
//	levelMenuInfo.item_rightarrow.textY				= MENU_BUTTON_TEXT_Y;
//	levelMenuInfo.item_rightarrow.textEnum			= MBT_PREVMAPS;
//	levelMenuInfo.item_rightarrow.textcolor			= CT_BLACK;
//	levelMenuInfo.item_rightarrow.textcolor2		= CT_WHITE;
//
//
//
//	// Map pics
//	y = 84;
//	x = 130;
//	mapHeight = 87;
//	mapWidth = 116;
//	levelMenuInfo.item_maps[0].generic.type			= MTYPE_BITMAP;
//	levelMenuInfo.item_maps[0].generic.name			= levelMenuInfo.levelPicNames[0];
//	levelMenuInfo.item_maps[0].generic.flags		= QMF_LEFT_JUSTIFY;
//	levelMenuInfo.item_maps[0].generic.x			= x;
//	levelMenuInfo.item_maps[0].generic.y			= y;
//	levelMenuInfo.item_maps[0].generic.id			= ID_PICTURE0;
//	levelMenuInfo.item_maps[0].generic.callback		= UI_SPLevelMenu_LevelEvent;
//	levelMenuInfo.item_maps[0].width				= mapWidth;
//	levelMenuInfo.item_maps[0].height				= mapHeight;
//
//	x += mapWidth + 6;
//	levelMenuInfo.item_maps[1].generic.type			= MTYPE_BITMAP;
//	levelMenuInfo.item_maps[1].generic.name			= levelMenuInfo.levelPicNames[1];
//	levelMenuInfo.item_maps[1].generic.flags		= QMF_LEFT_JUSTIFY;
//	levelMenuInfo.item_maps[1].generic.x			= x;
//	levelMenuInfo.item_maps[1].generic.y			= y;
//	levelMenuInfo.item_maps[1].generic.id			= ID_PICTURE1;
//	levelMenuInfo.item_maps[1].generic.callback		= UI_SPLevelMenu_LevelEvent;
//	levelMenuInfo.item_maps[1].width				= mapWidth;
//	levelMenuInfo.item_maps[1].height				= mapHeight;
//
//	x += mapWidth + 6;
//	levelMenuInfo.item_maps[2].generic.type			= MTYPE_BITMAP;
//	levelMenuInfo.item_maps[2].generic.name			= levelMenuInfo.levelPicNames[2];
//	levelMenuInfo.item_maps[2].generic.flags		= QMF_LEFT_JUSTIFY;
//	levelMenuInfo.item_maps[2].generic.x			= x;
//	levelMenuInfo.item_maps[2].generic.y			= y;
//	levelMenuInfo.item_maps[2].generic.id			= ID_PICTURE2;
//	levelMenuInfo.item_maps[2].generic.callback		= UI_SPLevelMenu_LevelEvent;
//	levelMenuInfo.item_maps[2].width				= mapWidth;
//	levelMenuInfo.item_maps[2].height				= mapHeight;
//
//	x += mapWidth + 6;
//	levelMenuInfo.item_maps[3].generic.type			= MTYPE_BITMAP;
//	levelMenuInfo.item_maps[3].generic.name			= levelMenuInfo.levelPicNames[3];
//	levelMenuInfo.item_maps[3].generic.flags		= QMF_LEFT_JUSTIFY;
//	levelMenuInfo.item_maps[3].generic.x			= x;
//	levelMenuInfo.item_maps[3].generic.y			= y;
//	levelMenuInfo.item_maps[3].generic.id			= ID_PICTURE3;
//	levelMenuInfo.item_maps[3].generic.callback		= UI_SPLevelMenu_LevelEvent;
//	levelMenuInfo.item_maps[3].width				= mapWidth;
//	levelMenuInfo.item_maps[3].height				= mapHeight;
//
//	levelMenuInfo.item_reset.generic.type			= MTYPE_BITMAP;
//	levelMenuInfo.item_reset.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
//	levelMenuInfo.item_reset.generic.x				= 432;
//	levelMenuInfo.item_reset.generic.y				= 350;
//	levelMenuInfo.item_reset.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
//	levelMenuInfo.item_reset.generic.id				= ID_RESET;
//	levelMenuInfo.item_reset.generic.callback		= UI_SPLevelMenu_ResetEvent;
//	levelMenuInfo.item_reset.width					= MENU_BUTTON_MED_WIDTH;
//	levelMenuInfo.item_reset.height					= MENU_BUTTON_MED_HEIGHT;
//	levelMenuInfo.item_reset.color					= CT_DKPURPLE1;
//	levelMenuInfo.item_reset.color2					= CT_LTPURPLE1;
//	levelMenuInfo.item_reset.textX					= MENU_BUTTON_TEXT_X;
//	levelMenuInfo.item_reset.textY					= MENU_BUTTON_TEXT_Y;
//	levelMenuInfo.item_reset.textEnum				= MBT_RESETPROGRESS;
//	levelMenuInfo.item_reset.textcolor				= CT_BLACK;
//	levelMenuInfo.item_reset.textcolor2				= CT_WHITE;
//
//	// Player pic
//	trap_Cvar_VariableStringBuffer( "model", levelMenuInfo.playerModel, sizeof(levelMenuInfo.playerModel) );
//	PlayerIcon( levelMenuInfo.playerModel, levelMenuInfo.playerPicName, sizeof(levelMenuInfo.playerPicName) );
//	levelMenuInfo.item_player.generic.type			= MTYPE_BITMAP;
//	levelMenuInfo.item_player.generic.flags			= QMF_INACTIVE;
//	levelMenuInfo.item_player.generic.name			= levelMenuInfo.playerPicName;
//	levelMenuInfo.item_player.generic.x				= 461;
//	levelMenuInfo.item_player.generic.y				= 247;
//	levelMenuInfo.item_player.generic.id			= ID_PLAYERPIC;
//	levelMenuInfo.item_player.width					= 64;
//	levelMenuInfo.item_player.height				= 64;
//
//	levelMenuInfo.playerinfo.generic.type			= MTYPE_BITMAP;
//	levelMenuInfo.playerinfo.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
//	levelMenuInfo.playerinfo.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
//	levelMenuInfo.playerinfo.generic.x				= 432;
//	levelMenuInfo.playerinfo.generic.y				= 325;
//	levelMenuInfo.playerinfo.generic.callback		= UI_SPLevelMenu_PlayerEvent;
//	levelMenuInfo.playerinfo.generic.id				= ID_PLAYERINFO;
//	levelMenuInfo.playerinfo.width					= MENU_BUTTON_MED_WIDTH;
//	levelMenuInfo.playerinfo.height					= MENU_BUTTON_MED_HEIGHT;
//	levelMenuInfo.playerinfo.color					= CT_DKPURPLE1;
//	levelMenuInfo.playerinfo.color2					= CT_LTPURPLE1;
//	levelMenuInfo.playerinfo.textX					= MENU_BUTTON_TEXT_X;
//	levelMenuInfo.playerinfo.textY					= MENU_BUTTON_TEXT_Y;
//	levelMenuInfo.playerinfo.textEnum				= MBT_PLAYERINFO;
//	levelMenuInfo.playerinfo.textcolor				= CT_BLACK;
//	levelMenuInfo.playerinfo.textcolor2				= CT_WHITE;
//
//
//	for( n = 0; n < AWARD_MAX; n++ ) 
//	{
//		levelMenuInfo.awardLevels[n] = UI_GetAwardLevel( n );
//	}
////	levelMenuInfo.awardLevels[AWARD_FRAGS] = 100 * (levelMenuInfo.awardLevels[AWARD_FRAGS] / 100);
//
//	y = AWARDS_Y;
//	count = 0;
//	for( n = 0; n < AWARD_MAX; n++ ) 
//	{
//		if( levelMenuInfo.awardLevels[n] ) 
//		{
//			x = 385 + (count * 38);
//
//			levelMenuInfo.item_awards[count].generic.type		= MTYPE_BITMAP;
//			levelMenuInfo.item_awards[count].generic.name		= ui_medalPicNames[n];
//			levelMenuInfo.item_awards[count].generic.flags		= QMF_LEFT_JUSTIFY|QMF_SILENT|QMF_MOUSEONLY;
//			levelMenuInfo.item_awards[count].generic.x			= x;
//			levelMenuInfo.item_awards[count].generic.y			= y;
//			levelMenuInfo.item_awards[count].generic.id			= ID_AWARD1 + n;
//			levelMenuInfo.item_awards[count].generic.callback	= UI_SPLevelMenu_AwardEvent;
//			levelMenuInfo.item_awards[count].width				= 34;
//			levelMenuInfo.item_awards[count].height				= 34;
//			count++;
//		}
//	}
//
//	levelMenuInfo.create.generic.type			= MTYPE_BITMAP;
//	levelMenuInfo.create.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
//	levelMenuInfo.create.generic.x			= 293;
//	levelMenuInfo.create.generic.y			= 391;
//	levelMenuInfo.create.generic.name		= BUTTON_GRAPHIC_LONGRIGHT;
//	levelMenuInfo.create.generic.id			= ID_CUSTOM;
//	levelMenuInfo.create.generic.callback	= UI_SPLevelMenu_CreateEvent;
//	levelMenuInfo.create.width				= MENU_BUTTON_MED_WIDTH;
//	levelMenuInfo.create.height				= MENU_BUTTON_MED_HEIGHT;
//	levelMenuInfo.create.color				= CT_DKPURPLE1;
//	levelMenuInfo.create.color2				= CT_LTPURPLE1;
//	levelMenuInfo.create.textX				= MENU_BUTTON_TEXT_X;
//	levelMenuInfo.create.textY				= MENU_BUTTON_TEXT_Y;
//	levelMenuInfo.create.textEnum				= MBT_CREATEMATCH;
//	levelMenuInfo.create.textcolor			= CT_BLACK;
//	levelMenuInfo.create.textcolor2			= CT_WHITE;
//
//	levelMenuInfo.advanced.generic.type			= MTYPE_BITMAP;
//	levelMenuInfo.advanced.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
//	levelMenuInfo.advanced.generic.x			= 293;
//	levelMenuInfo.advanced.generic.y			= 415;
//	levelMenuInfo.advanced.generic.name		= BUTTON_GRAPHIC_LONGRIGHT;
//	levelMenuInfo.advanced.generic.id			= ID_ADVANCED;
//	levelMenuInfo.advanced.generic.callback	= UI_SPLevelMenu_AdvancedEvent;
//	levelMenuInfo.advanced.width				= MENU_BUTTON_MED_WIDTH;
//	levelMenuInfo.advanced.height				= MENU_BUTTON_MED_HEIGHT;
//	levelMenuInfo.advanced.color				= CT_DKPURPLE1;
//	levelMenuInfo.advanced.color2				= CT_LTPURPLE1;
//	levelMenuInfo.advanced.textX				= MENU_BUTTON_TEXT_X;
//	levelMenuInfo.advanced.textY				= MENU_BUTTON_TEXT_Y;
//	levelMenuInfo.advanced.textEnum				= MBT_ADVANCEDSERVER;
//	levelMenuInfo.advanced.textcolor			= CT_BLACK;
//	levelMenuInfo.advanced.textcolor2			= CT_WHITE;
//
//	levelMenuInfo.fight.generic.type				= MTYPE_BITMAP;
//	levelMenuInfo.fight.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
//	levelMenuInfo.fight.generic.x					= 462;
//	levelMenuInfo.fight.generic.y					= 391;
//	levelMenuInfo.fight.generic.name				= "menu/common/square.tga";
//	levelMenuInfo.fight.generic.id					= ID_NEXT;
//	levelMenuInfo.fight.generic.callback			= UI_SPLevelMenu_NextEvent;
//	levelMenuInfo.fight.width						= 150;
//	levelMenuInfo.fight.height						= 42;
//	levelMenuInfo.fight.color						= CT_DKPURPLE1;
//	levelMenuInfo.fight.color2						= CT_LTPURPLE1;
//	levelMenuInfo.fight.textX						= 2;
//	levelMenuInfo.fight.textY						= 2;
//	levelMenuInfo.fight.textEnum					= MBT_ENGAGE;
//	levelMenuInfo.fight.textcolor					= CT_BLACK;
//	levelMenuInfo.fight.textcolor2					= CT_WHITE;
//
//	levelMenuInfo.item_null.generic.type			= MTYPE_BITMAP;
//	levelMenuInfo.item_null.generic.flags			= QMF_LEFT_JUSTIFY|QMF_MOUSEONLY|QMF_SILENT;
//	levelMenuInfo.item_null.generic.x				= 0;
//	levelMenuInfo.item_null.generic.y				= 0;
//	levelMenuInfo.item_null.width					= 640;
//	levelMenuInfo.item_null.height					= 480;
//
//	Menu_AddItem( &levelMenuInfo.menu, &levelMenuInfo.mainmenu );
//	Menu_AddItem( &levelMenuInfo.menu, &levelMenuInfo.back );
//	Menu_AddItem( &levelMenuInfo.menu, &levelMenuInfo.item_leftarrow );
//	Menu_AddItem( &levelMenuInfo.menu, &levelMenuInfo.item_rightarrow );
//	Menu_AddItem( &levelMenuInfo.menu, &levelMenuInfo.item_maps[0] );
//	Menu_AddItem( &levelMenuInfo.menu, &levelMenuInfo.item_maps[1] );
//	Menu_AddItem( &levelMenuInfo.menu, &levelMenuInfo.item_maps[2] );
//	Menu_AddItem( &levelMenuInfo.menu, &levelMenuInfo.item_maps[3] );
//	// Extend to pick up names at bottom of pictures
//	levelMenuInfo.item_maps[0].generic.bottom += 18;
//	levelMenuInfo.item_maps[1].generic.bottom += 18;
//	levelMenuInfo.item_maps[2].generic.bottom += 18;
//	levelMenuInfo.item_maps[3].generic.bottom += 18;
//
//	Menu_AddItem( &levelMenuInfo.menu, &levelMenuInfo.item_player );
//
////	for( n = 0; n < count; n++ ) 
////	{
////		Menu_AddItem( &levelMenuInfo.menu, &levelMenuInfo.item_awards[n] );
////	}
//
//	Menu_AddItem( &levelMenuInfo.menu, &levelMenuInfo.item_reset );
//	Menu_AddItem( &levelMenuInfo.menu, &levelMenuInfo.playerinfo );
//	Menu_AddItem( &levelMenuInfo.menu, &levelMenuInfo.create );
//	Menu_AddItem( &levelMenuInfo.menu, &levelMenuInfo.advanced );
//	Menu_AddItem( &levelMenuInfo.menu, &levelMenuInfo.fight );
//	Menu_AddItem( &levelMenuInfo.menu, &levelMenuInfo.item_null );
//
//	trap_Cvar_VariableStringBuffer( "ui_spSelection", buf, sizeof(buf) );
//	if( *buf ) 
//	{
//		n = atoi( buf );
//		selectedArenaSet = n / ARENAS_PER_TIER;
//		selectedArena = n % ARENAS_PER_TIER;
//	}
//	else 
//	{
//		selectedArenaSet = currentSet;
//		selectedArena = currentGame;
//	}
//
//	UI_SPLevelMenu_SetMenuItems();
//}
//
//
///*
//=================
//UI_SPLevelMenu
//=================
//*/
//void UI_SPLevelMenu( void ) 
//{
//	int			level;
//	int			trainingLevel;
//	const char	*arenaInfo;
//
//	trainingTier = -1;
//	arenaInfo = UI_GetSpecialArenaInfo( "training" );
//	if( arenaInfo ) 
//	{
//		minTier = trainingTier;
//		trainingLevel = atoi( Info_ValueForKey( arenaInfo, "num" ) );
//	}
//	else 
//	{
//		minTier = 0;
//		trainingLevel = -2;
//	}
//
//	finalTier = UI_GetNumSPTiers();
//	arenaInfo = UI_GetSpecialArenaInfo( "final" );
//	if( arenaInfo ) {
//		maxTier = finalTier;
//	}
//	else 
//	{
//		maxTier = finalTier - 1;
//		if( maxTier < minTier ) 
//		{
//			maxTier = minTier;
//		}
//	}
//
//	level = UI_GetCurrentGame(-1);
//
//	if ( level == -1 ) {
//		level = UI_GetNumSPArenas() - 1;
//		if( maxTier == finalTier ) 
//		{
//			level++;
//		}
//	}
//
//	if( level == trainingLevel ) 
//	{
//		currentSet = -1;
//		currentGame = 0;
//	}
//	else {
//		currentSet = level / ARENAS_PER_TIER;
//		currentGame = level % ARENAS_PER_TIER;
//	}
//
//	UI_SPLevelMenu_Init();
//	UI_PushMenu( &levelMenuInfo.menu );
//	Menu_SetCursorToItem( &levelMenuInfo.menu, &levelMenuInfo.fight );
//
//}
//
//
///*
//=================
//UI_SPLevelMenu_f
//=================
//*/
//void UI_SPLevelMenu_f( void ) 
//{
//	trap_Key_SetCatcher( KEYCATCH_UI );
//	uis.menusp = 0;
//	Mouse_Show();
//	UI_SPLevelMenu();
//}
//
//
///*
//=================
//UI_SPLevelMenu_ReInit
//=================
//*/
//void UI_SPLevelMenu_ReInit( void ) 
//{
//	levelMenuInfo.reinit = qtrue;
//}
//
//
///*
//===============
//ResetGameMenu_Event
//===============
//*/
//static void ResetGameMenu_Event (void* ptr, int notification)
//{
//	if (notification != QM_ACTIVATED)
//		return;
//
//	switch (((menucommon_s*)ptr)->id)
//	{
//		case ID_MAINMENU:
//			UI_PopMenu ();
//			UI_MainMenu();
//			break;
//
//		case ID_BACK:
//		case ID_NO:
//			UI_PopMenu ();
//			break;
//
//		case ID_YES:
//			UI_SPLevelMenu_ResetAction(qtrue);
//			break;
//	}
//}
//
///*
//===============
//UI_resetGameMenu_Draw
//===============
//*/
//static void UI_resetGameMenu_Draw( void ) 
//{
//	int y;
//
//	// Draw the basic screen layout
//	UI_MenuFrame2(&resetGameMenuInfo.menu);
//
//	trap_R_SetColor( colorTable[CT_LTBROWN1]);
//	UI_DrawHandlePic(30,203,  47, 186, uis.whiteShader);	// Long left column square on bottom 3rd
//
//	// Box around text
//	trap_R_SetColor( colorTable[CT_LTPURPLE3]);
//	UI_DrawHandlePic(122, 103,  32, 64,resetGameMenuInfo.corner_ul);	// Upper left corner
//	UI_DrawHandlePic(122, 284,  32, 32,resetGameMenuInfo.corner_ll);	// Lower left corner
//	UI_DrawHandlePic(544, 103,  32, 64,resetGameMenuInfo.corner_ur);	// Upper right corner
//	UI_DrawHandlePic(545, 285,  32, 32,resetGameMenuInfo.corner_lr);	// Lower right corner
//
//	UI_DrawHandlePic(132,103, 426,  31, uis.whiteShader);	// Top of box
//	UI_DrawHandlePic(122,143,  19, 138, uis.whiteShader);	// Left side of box
//	UI_DrawHandlePic(549,143,  19, 138, uis.whiteShader);	// Right side of box
//
//	UI_DrawHandlePic(132,288,  32, 18, uis.whiteShader);	// Bottom Left line
//	UI_DrawHandlePic(526,288,  32, 18, uis.whiteShader);	// Bottom Right line
//	UI_DrawHandlePic(167,288, 356, 18, uis.whiteShader);	// Bottom Middle line
//
//	// Numbers on frame
//	UI_DrawProportionalString(  74,  66, "66917",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
//	UI_DrawProportionalString(  74,  188, "098-145",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
//	UI_DrawProportionalString(  74,  206, "324-76",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
//	UI_DrawProportionalString(  74,  395, "774570",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
//
//	UI_DrawProportionalString(  150,  107, "63-0999712",UI_TINYFONT, colorTable[CT_BLACK]);
//
//	y = 172;
//	UI_DrawProportionalString( 176, y, menu_normal_text[MNT_PLAYER_RESET1], UI_SMALLFONT, colorTable[CT_RED] );
//	y+=24;
//	UI_DrawProportionalString( 176, y, menu_normal_text[MNT_PLAYER_RESET2], UI_SMALLFONT, colorTable[CT_RED] );
//	y+=24;
//	UI_DrawProportionalString( 176, y, menu_normal_text[MNT_PLAYER_RESET3], UI_SMALLFONT, colorTable[CT_RED] );
//	y+=24*2;
//	UI_DrawProportionalString( 262, y, menu_normal_text[MNT_RESET_PLAYER],UI_BLINK | UI_SMALLFONT, colorTable[CT_RED] );
//
//	trap_R_SetColor( colorTable[resetGameMenuInfo.mainmenu.color]);
//	UI_DrawHandlePic(110 - 14, resetGameMenuInfo.mainmenu.generic.y, 
//		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
//
//	trap_R_SetColor( colorTable[resetGameMenuInfo.back.color]);
//	UI_DrawHandlePic(110 - 14, resetGameMenuInfo.back.generic.y, 
//		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
//
//	// standard menu drawing
//	Menu_Draw( &resetGameMenuInfo.menu );
//}
//
///*
//=================
//UI_ResetGameMenu_Cache
//=================
//*/
//void UI_ResetGameMenu_Cache(void)
//{
//	resetGameMenuInfo.corner_ul = trap_R_RegisterShaderNoMip("menu/common/corner_ul_19_31.tga");
//	resetGameMenuInfo.corner_ur = trap_R_RegisterShaderNoMip("menu/common/corner_ur_19_31.tga");
//	resetGameMenuInfo.corner_ll = trap_R_RegisterShaderNoMip("menu/common/corner_ll_18_19.tga");
//	resetGameMenuInfo.corner_lr = trap_R_RegisterShaderNoMip("menu/common/corner_lr_18_19.tga");
//}
//
///*
//=================
//ResetGameMenu_Init
//=================
//*/
//void ResetGameMenu_Init( void )
//{
//
//	memset( &resetGameMenuInfo, 0, sizeof(resetGameMenuInfo) );
//
//	UI_ResetGameMenu_Cache();
//
//	resetGameMenuInfo.menu.fullscreen					= qtrue;
//	resetGameMenuInfo.menu.wrapAround					= qtrue;
//	resetGameMenuInfo.menu.draw							= UI_resetGameMenu_Draw;
//	resetGameMenuInfo.menu.descX						= MENU_DESC_X;
//	resetGameMenuInfo.menu.descY						= MENU_DESC_Y;
//	resetGameMenuInfo.menu.titleX						= MENU_TITLE_X;
//	resetGameMenuInfo.menu.titleY						= MENU_TITLE_Y;
//	resetGameMenuInfo.menu.titleI						= MNT_CONTROLSMENU_TITLE;
//	resetGameMenuInfo.menu.footNoteEnum					= MNT_RESETPLAYER;
//
//	resetGameMenuInfo.mainmenu.generic.type				= MTYPE_BITMAP;      
//	resetGameMenuInfo.mainmenu.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
//	resetGameMenuInfo.mainmenu.generic.x				= 110;
//	resetGameMenuInfo.mainmenu.generic.y				= 391;
//	resetGameMenuInfo.mainmenu.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
//	resetGameMenuInfo.mainmenu.generic.id				= ID_MAINMENU;
//	resetGameMenuInfo.mainmenu.generic.callback			= ResetGameMenu_Event;
//	resetGameMenuInfo.mainmenu.width					= MENU_BUTTON_MED_WIDTH;
//	resetGameMenuInfo.mainmenu.height					= MENU_BUTTON_MED_HEIGHT;
//	resetGameMenuInfo.mainmenu.color					= CT_DKPURPLE1;
//	resetGameMenuInfo.mainmenu.color2					= CT_LTPURPLE1;
//	resetGameMenuInfo.mainmenu.textX					= MENU_BUTTON_TEXT_X;
//	resetGameMenuInfo.mainmenu.textY					= MENU_BUTTON_TEXT_Y;
//	resetGameMenuInfo.mainmenu.textEnum					= MBT_MAINMENU;
//	resetGameMenuInfo.mainmenu.textcolor				= CT_BLACK;
//	resetGameMenuInfo.mainmenu.textcolor2				= CT_WHITE;
//
//	resetGameMenuInfo.back.generic.type				= MTYPE_BITMAP;      
//	resetGameMenuInfo.back.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
//	resetGameMenuInfo.back.generic.x				= 110;
//	resetGameMenuInfo.back.generic.y				= 415;
//	resetGameMenuInfo.back.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
//	resetGameMenuInfo.back.generic.id				= ID_BACK;
//	resetGameMenuInfo.back.generic.callback			= ResetGameMenu_Event;
//	resetGameMenuInfo.back.width					= MENU_BUTTON_MED_WIDTH;
//	resetGameMenuInfo.back.height					= MENU_BUTTON_MED_HEIGHT;
//	resetGameMenuInfo.back.color					= CT_DKPURPLE1;
//	resetGameMenuInfo.back.color2					= CT_LTPURPLE1;
//	resetGameMenuInfo.back.textX					= MENU_BUTTON_TEXT_X;
//	resetGameMenuInfo.back.textY					= MENU_BUTTON_TEXT_Y;
//	resetGameMenuInfo.back.textEnum					= MBT_BACK;
//	resetGameMenuInfo.back.textcolor				= CT_BLACK;
//	resetGameMenuInfo.back.textcolor2				= CT_WHITE;
//
//	resetGameMenuInfo.yes.generic.type					= MTYPE_BITMAP;      
//	resetGameMenuInfo.yes.generic.flags					= QMF_HIGHLIGHT_IF_FOCUS;
//	resetGameMenuInfo.yes.generic.x						= 462;
//	resetGameMenuInfo.yes.generic.y						= 391;
//	resetGameMenuInfo.yes.generic.name					= GRAPHIC_SQUARE;
//	resetGameMenuInfo.yes.generic.id					= ID_YES;
//	resetGameMenuInfo.yes.generic.callback				= ResetGameMenu_Event;
//	resetGameMenuInfo.yes.width							= 150;
//	resetGameMenuInfo.yes.height						= 42;
//	resetGameMenuInfo.yes.color							= CT_DKPURPLE1;
//	resetGameMenuInfo.yes.color2						= CT_LTPURPLE1;
//	resetGameMenuInfo.yes.textX							= MENU_BUTTON_TEXT_X;
//	resetGameMenuInfo.yes.textY							= MENU_BUTTON_TEXT_Y;
//	resetGameMenuInfo.yes.textEnum						= MBT_YESRESET;
//	resetGameMenuInfo.yes.textcolor						= CT_BLACK;
//	resetGameMenuInfo.yes.textcolor2					= CT_WHITE;
//
//
//	Menu_AddItem( &resetGameMenuInfo.menu, &resetGameMenuInfo.mainmenu );
//	Menu_AddItem( &resetGameMenuInfo.menu, &resetGameMenuInfo.back );
//	Menu_AddItem( &resetGameMenuInfo.menu, &resetGameMenuInfo.yes );
//
//}
//
///*
//=================
//UI_ResetGameMenu
//=================
//*/
//void UI_ResetGameMenu( void )
//{
//	ResetGameMenu_Init();
//
//	UI_PushMenu( &resetGameMenuInfo.menu );
//	Menu_SetCursorToItem( &resetGameMenuInfo.menu, &resetGameMenuInfo.back );
//}
//
//
///*
//===============
//M_SPChoose_Event
//===============
//*/
//void M_SPChoose_Event (void* ptr, int notification)
//{
//	if (notification != QM_ACTIVATED)
//		return;
//
//	switch (((menucommon_s*)ptr)->id)
//	{
//		case ID_MAINMENU:
//			UI_PopMenu ();
//			break;
//
//		case ID_CUSTOM:
//			UI_StartServerMenu( qfalse );
//			break;
//
//		case ID_PRESET:
//			UI_SPLevelMenu();
//			break;
//	}
//}
///*
//===============
//SPChoose_MenuKey
//===============
//*/
//static sfxHandle_t SPChoose_MenuKey( int key )
//{
//	return ( Menu_DefaultKey( &s_spchoose.menu, key ) );
//}
//
///*
//===============
//SPChoose_MenuDraw
//===============
//*/
//static void SPChoose_MenuDraw( void )
//{
//	// Draw the basic screen layout
//	UI_MenuFrame(&s_spchoose.menu);
//
//	trap_R_SetColor( colorTable[CT_LTBROWN1]);
//	UI_DrawHandlePic(30,203,  47, 186, uis.whiteShader);	// Long left column square on bottom 3rd
//
//	UI_DrawProportionalString(  74,  66, "45",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
//	UI_DrawProportionalString(  74,  84, "8341",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
//	UI_DrawProportionalString(  74,  188, "90902",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
//	UI_DrawProportionalString(  74,  206, "44",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
//	UI_DrawProportionalString(  74,  395, "6801",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
//
//	trap_R_SetColor( colorTable[CT_DKPURPLE1]);
//
//	// Left rounded ends
//	UI_DrawHandlePic(290 - 14, 240, MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
//	UI_DrawHandlePic(290 - 14, 310, MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
//
//	Menu_Draw( &s_spchoose.menu );
//}
//
///*
//===============
//SPChooseMenu_Cache
//===============
//*/
//void SPChooseMenu_Cache( void ) 
//{
//}
//
///*
//=================
//SPChoose_MenuInit
//=================
//*/
//static void SPChoose_MenuInit(void)
//{
//	int x,y;
//
//	SPChooseMenu_Cache(); 
//
//	s_spchoose.menu.nitems					= 0;
//	s_spchoose.menu.draw					= SPChoose_MenuDraw;
//	s_spchoose.menu.key						= SPChoose_MenuKey;
//	s_spchoose.menu.wrapAround				= qtrue;
//	s_spchoose.menu.descX					= MENU_DESC_X;
//	s_spchoose.menu.descY					= MENU_DESC_Y;
//	s_spchoose.menu.titleX					= MENU_TITLE_X;
//	s_spchoose.menu.titleY					= MENU_TITLE_Y;
//	s_spchoose.menu.titleI					= MNT_SINGLEPLAYER_TITLE;
//	s_spchoose.menu.footNoteEnum			= MNT_SOLOMATCHTYPES;
//	s_spchoose.menu.fullscreen				= qtrue;
//
//	s_spchoose.mainmenu.generic.type		= MTYPE_BITMAP;      
//	s_spchoose.mainmenu.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
//	s_spchoose.mainmenu.generic.x			= 482;
//	s_spchoose.mainmenu.generic.y			= 136;
//	s_spchoose.mainmenu.generic.name		= BUTTON_GRAPHIC_LONGRIGHT;
//	s_spchoose.mainmenu.generic.id			= ID_MAINMENU;
//	s_spchoose.mainmenu.generic.callback	= M_SPChoose_Event;
//	s_spchoose.mainmenu.width				= MENU_BUTTON_MED_WIDTH;
//	s_spchoose.mainmenu.height				= MENU_BUTTON_MED_HEIGHT;
//	s_spchoose.mainmenu.color				= CT_DKPURPLE1;
//	s_spchoose.mainmenu.color2				= CT_LTPURPLE1;
//	s_spchoose.mainmenu.textX				= MENU_BUTTON_TEXT_X;
//	s_spchoose.mainmenu.textY				= MENU_BUTTON_TEXT_Y;
//	s_spchoose.mainmenu.textEnum			= MBT_MAINMENU;
//	s_spchoose.mainmenu.textcolor			= CT_BLACK;
//	s_spchoose.mainmenu.textcolor2			= CT_WHITE;
//
//	x = 290;
//	y = 240;
//	s_spchoose.preset.generic.type			= MTYPE_BITMAP;
//	s_spchoose.preset.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
//	s_spchoose.preset.generic.x				= x;
//	s_spchoose.preset.generic.y				= y;
//	s_spchoose.preset.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
//	s_spchoose.preset.generic.id				= ID_PRESET;
//	s_spchoose.preset.generic.callback		= M_SPChoose_Event;
//	s_spchoose.preset.width					= MENU_BUTTON_MED_WIDTH;
//	s_spchoose.preset.height				= MENU_BUTTON_MED_HEIGHT;
//	s_spchoose.preset.color					= CT_DKPURPLE1;
//	s_spchoose.preset.color2				= CT_LTPURPLE1;
//	s_spchoose.preset.textX					= MENU_BUTTON_TEXT_X;
//	s_spchoose.preset.textY					= MENU_BUTTON_TEXT_Y;
//	s_spchoose.preset.textEnum				= MBT_PRESETGAME;
//	s_spchoose.preset.textcolor				= CT_BLACK;
//	s_spchoose.preset.textcolor2			= CT_WHITE;
//
//	y += 70;
//	s_spchoose.create.generic.type			= MTYPE_BITMAP;
//	s_spchoose.create.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
//	s_spchoose.create.generic.x			= x;
//	s_spchoose.create.generic.y			= y;
//	s_spchoose.create.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
//	s_spchoose.create.generic.id			= ID_CUSTOM;
//	s_spchoose.create.generic.callback		= M_SPChoose_Event;
//	s_spchoose.create.width				= MENU_BUTTON_MED_WIDTH;
//	s_spchoose.create.height				= MENU_BUTTON_MED_HEIGHT;
//	s_spchoose.create.color				= CT_DKPURPLE1;
//	s_spchoose.create.color2				= CT_LTPURPLE1;
//	s_spchoose.create.textX				= MENU_BUTTON_TEXT_X;
//	s_spchoose.create.textY				= MENU_BUTTON_TEXT_Y;
//	s_spchoose.create.textEnum				= MBT_CREATEMATCH;
//	s_spchoose.create.textcolor			= CT_BLACK;
//	s_spchoose.create.textcolor2			= CT_WHITE;
//
//	Menu_AddItem( &s_spchoose.menu,	&s_spchoose.mainmenu );
//	Menu_AddItem( &s_spchoose.menu,	&s_spchoose.preset );
//	Menu_AddItem( &s_spchoose.menu,	&s_spchoose.create );
//
//}
//	
///*
//=================
//UI_SPChooseMenu
//=================
//*/
//void UI_SPChooseMenu( void )
//{
//
//	SPChoose_MenuInit();
//
//	UI_PushMenu( &s_spchoose.menu );
//	Menu_SetCursorToItem( &s_spchoose.menu, &s_spchoose.preset );
//}
