//// Copyright (C) 1999-2000 Id Software, Inc.
////
////
//// ui_team.c
////
//
#include "ui_local.h"
//
//#define ID_INGAMEMENU	50
//#define ID_JOINRED		100
//#define ID_JOINBLUE		101
//#define ID_JOINAUTO		102
//#define ID_JOINGAME		103
//#define ID_SPECTATE		104
//#define ID_PLAYERCLASS	105
//
////RPG-X: RedTechie - RPG-X Enhanced
//static int player_classes[] =
//{
//	MNT_PC_INFILTRATOR,//RPG-X: RedTechie - Security
//	MNT_PC_SNIPER,//RPG-X: RedTechie - Medical
//	MNT_PC_HEAVY,//RPG-X: RedTechie - ALIEN
//	MNT_PC_DEMO,//RPG-X: RedTechie - Command
//	MNT_PC_MEDIC,//RPG-X: RedTechie - Science
//	MNT_PC_TECH,//RPG-X: RedTechie - Engineer
//	MNT_PC_NOCLASS,
//	MNT_PC_ALPHAOMEGA22,
//	MNT_PC_N00B,
//	0
//};
//
////RPG-X: RedTechie - Not needed anymore
///*static int player_classes2[] =
//{
//	MNT_PC_INFILTRATOR,//RPG-X: RedTechie - Security
//	MNT_PC_SNIPER,//RPG-X: RedTechie - Medical
//	MNT_PC_HEAVY,//RPG-X: RedTechie - ALIEN
//	MNT_PC_DEMO,//RPG-X: RedTechie - Command
//	MNT_PC_MEDIC,//RPG-X: RedTechie - Science
//	MNT_PC_TECH,//RPG-X: RedTechie - Engineer
//	MNT_ACTIONHERO,
//	MNT_PC_BORG,
//	MNT_PC_NOCLASS,
//	MNT_PC_ALPHAOMEGA22,
//	MNT_PC_N00B,
//	0
//};*/
//
////RPG-X: RedTechie - RPG-X Enhanced
//// Class Text enum
//typedef enum
//{
//	TM_INFILTRATOR,//RPG-X: RedTechie - Security
//	TM_SNIPER,//RPG-X: RedTechie - Medical
//	TM_HEAVY,//RPG-X: RedTechie - ALIEN
//	TM_DEMO,//RPG-X: RedTechie - Command
//	TM_MEDIC,//RPG-X: RedTechie - Science
//	TM_TECH,//RPG-X: RedTechie - Engineer
//	//TM_ACTIONHERO,
//	//TM_BORG,
//	TM_NOCLASS,
//	TM_ALPHAOMEGA22,
//	TM_N00B
//} teamClassType_t;
//
//
//
//
//typedef struct
//{
//	menuframework_s	menu;
//	menubitmap_s	ingamemenu;
//	menubitmap_s	joinred;
//	menubitmap_s	joinblue;
//	menubitmap_s	joinauto;
//	menubitmap_s	joingame;
//	menubitmap_s	spectate;
//	menulist_s		playerClass;
//	int				holdClass;
//	qhandle_t		pClassShaders[NUM_PLAYER_CLASSES];
//} teammain_t;
//
//static teammain_t	s_teammain;
//
//static void UpdatePlayerClass(void)
//{
//	// Hasn't changed so don't do anything
//	if (s_teammain.holdClass == s_teammain.playerClass.curvalue)
//	{
//		return;
//	}
//
//
//	switch( s_teammain.playerClass.curvalue ) 
//	{
//	case TM_NOCLASS:
//		trap_Cmd_ExecuteText( EXEC_APPEND, "cmd class noclass\n" );
//		break;
//	case TM_INFILTRATOR:
//		trap_Cmd_ExecuteText( EXEC_APPEND, "cmd class security\n" );
//		break;
//	case TM_SNIPER:
//		trap_Cmd_ExecuteText( EXEC_APPEND, "cmd class medical\n" );
//		break;
//	case TM_HEAVY:
//		trap_Cmd_ExecuteText( EXEC_APPEND, "cmd class alien\n" );
//		break;
//	case TM_DEMO:
//		trap_Cmd_ExecuteText( EXEC_APPEND, "cmd class command\n" );
//		break;
//	case TM_MEDIC:
//		trap_Cmd_ExecuteText( EXEC_APPEND, "cmd class science\n" );
//		break;
//	case TM_TECH:
//		trap_Cmd_ExecuteText( EXEC_APPEND, "cmd class engineer\n" );
//		break;
//	case TM_ALPHAOMEGA22:
//		trap_Cmd_ExecuteText( EXEC_APPEND, "cmd class marine\n" );
//		break;
//	case TM_N00B:
//		trap_Cmd_ExecuteText( EXEC_APPEND, "cmd class n00b\n" );
//		break;
//		/*case TM_BORG:
//		trap_Cmd_ExecuteText( EXEC_APPEND, "cmd class Borg\n" );
//		break;
//	case TM_ACTIONHERO:
//		trap_Cmd_ExecuteText( EXEC_APPEND, "cmd class Hero\n" );
//		break;*/
//	//break;
//	}
//
//
//}
//
///*
//===============
//TeamMain_MenuEvent
//===============
//*/
//static void TeamMain_MenuEvent( void* ptr, int event )
//{
//	if( event != QM_ACTIVATED ) 
//	{
//		return;
//	}
//
//	switch( ((menucommon_s*)ptr)->id ) 
//	{
//	case ID_INGAMEMENU:
//		UpdatePlayerClass();
//		UI_PopMenu();
//		break;
//	case ID_JOINRED:
//		UpdatePlayerClass();
//		trap_Cmd_ExecuteText( EXEC_APPEND, "cmd team red\n" );
//		UI_ForceMenuOff();
//		break;
//
//	case ID_JOINBLUE:
//		UpdatePlayerClass();
//		trap_Cmd_ExecuteText( EXEC_APPEND, "cmd team blue\n" );
//		UI_ForceMenuOff();
//		break;
//
//	case ID_JOINAUTO:
//		UpdatePlayerClass();
//		trap_Cmd_ExecuteText( EXEC_APPEND, "cmd team auto\n" );
//		UI_ForceMenuOff();
//		break;
//
//	case ID_PLAYERCLASS:
//		break;
//
//	case ID_JOINGAME:
//		UpdatePlayerClass();
//		trap_Cmd_ExecuteText( EXEC_APPEND, "cmd team free\n" );
//		UI_ForceMenuOff();
//		break;
//
//	case ID_SPECTATE:
//		trap_Cmd_ExecuteText( EXEC_APPEND, "cmd team spectator\n" );
//		UI_ForceMenuOff();
//		break;
//
//	}
//}
//
///*
//=================
//UI_TeamMainMenu_Draw
//=================
//*/
//static void UI_TeamMainMenu_Draw( void ) 
//{
//	UI_MenuFrame(&s_teammain.menu);
//
//	trap_R_SetColor( colorTable[CT_LTBROWN1]);
//	UI_DrawHandlePic(30,203,  47, 186, uis.whiteShader);	// Long left column square on bottom 3rd
//
//
//	trap_R_SetColor( colorTable[CT_WHITE]);
//	UI_DrawHandlePic(s_teammain.playerClass.generic.x + 160, s_teammain.playerClass.generic.y, 
//		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, s_teammain.pClassShaders[s_teammain.playerClass.curvalue]);
//
//	// Left rounded ends for buttons
//	trap_R_SetColor( colorTable[CT_DKPURPLE1]);
//	UI_DrawHandlePic(s_teammain.joinred.generic.x - 14, s_teammain.joinred.generic.y, 
//		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
//
//	UI_DrawHandlePic(s_teammain.joinblue.generic.x - 14, s_teammain.joinblue.generic.y, 
//		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
//
//	UI_DrawHandlePic(s_teammain.joinauto.generic.x - 14, s_teammain.joinauto.generic.y, 
//		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
//
//	UI_DrawHandlePic(s_teammain.joingame.generic.x - 14, s_teammain.joingame.generic.y, 
//		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
//
//	UI_DrawHandlePic(s_teammain.spectate.generic.x - 14, s_teammain.spectate.generic.y, 
//		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
//
//	// standard menu drawing
//	Menu_Draw( &s_teammain.menu );
//
//}
//
//static sfxHandle_t TeamMenu_MenuKey( int key )
//{
//		switch (key)
//		{		
//			case K_ESCAPE:
//				UpdatePlayerClass();
//				break;
//			default :
//				goto ignorekey;
//		}
//
//ignorekey:
//	return Menu_DefaultKey( &s_teammain.menu, key );
//}
//
//void SetServerButtons(int gameType,menulist_s *assimilation,menulist_s *specialties,menulist_s *disintegration,menulist_s *elimination,menulist_s *actionhero);
//
///*
//===============
//TeamMain_MenuInit
//===============
//*/
//void TeamMain_MenuInit( void ) 
//{
//	int		y,pad,x;
//	int		gametype;
//	char	info[MAX_INFO_STRING];
//
//	memset( &s_teammain, 0, sizeof(s_teammain) );
//
//	TeamMain_Cache();
//
//	trap_GetConfigString(CS_SERVERINFO, info, MAX_INFO_STRING);   
//
//	s_teammain.menu.wrapAround					= qtrue;
//	s_teammain.menu.fullscreen					= qtrue;
//	s_teammain.menu.descX						= MENU_DESC_X;
//	s_teammain.menu.descY						= MENU_DESC_Y;
//	s_teammain.menu.draw						= UI_TeamMainMenu_Draw;
//	s_teammain.menu.key							= TeamMenu_MenuKey;
//	s_teammain.menu.titleX						= MENU_TITLE_X;
//	s_teammain.menu.titleY						= MENU_TITLE_Y;
//	s_teammain.menu.titleI						= MNT_TEAMMENU_TITLE;
//	s_teammain.menu.footNoteEnum				= MNT_TEAM_MENU;
//
//	s_teammain.ingamemenu.generic.type			= MTYPE_BITMAP;      
//	s_teammain.ingamemenu.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
//	s_teammain.ingamemenu.generic.x				= 482;
//	s_teammain.ingamemenu.generic.y				= 136;
//	s_teammain.ingamemenu.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
//	s_teammain.ingamemenu.generic.id			= ID_INGAMEMENU;
//	s_teammain.ingamemenu.generic.callback		= TeamMain_MenuEvent;
//	s_teammain.ingamemenu.width					= MENU_BUTTON_MED_WIDTH;
//	s_teammain.ingamemenu.height				= MENU_BUTTON_MED_HEIGHT;
//	s_teammain.ingamemenu.color					= CT_DKPURPLE1;
//	s_teammain.ingamemenu.color2				= CT_LTPURPLE1;
//	s_teammain.ingamemenu.textX					= 5;
//	s_teammain.ingamemenu.textY					= 2;
//	s_teammain.ingamemenu.textEnum				= MBT_INGAMEMENU;
//	s_teammain.ingamemenu.textcolor				= CT_BLACK;
//	s_teammain.ingamemenu.textcolor2			= CT_WHITE;
//
//	y = 200;
//	x = 290;
//	s_teammain.joinred.generic.type			    = MTYPE_BITMAP;
//	s_teammain.joinred.generic.flags		    = QMF_HIGHLIGHT_IF_FOCUS;
//	s_teammain.joinred.generic.x				= x;
//	s_teammain.joinred.generic.y				= y;
//	s_teammain.joinred.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
//	s_teammain.joinred.generic.id				= ID_JOINRED;
//	s_teammain.joinred.generic.callback			= TeamMain_MenuEvent;
//	s_teammain.joinred.width					= MENU_BUTTON_MED_WIDTH;
//	s_teammain.joinred.height					= MENU_BUTTON_MED_HEIGHT;
//	s_teammain.joinred.color					= CT_DKPURPLE1;
//	s_teammain.joinred.color2					= CT_LTPURPLE1;
//	s_teammain.joinred.textX					= MENU_BUTTON_TEXT_X;
//	s_teammain.joinred.textY					= MENU_BUTTON_TEXT_Y;
//	s_teammain.joinred.textEnum					= MBT_REDTEAM;
//	s_teammain.joinred.textcolor				= CT_BLACK;
//	s_teammain.joinred.textcolor2				= CT_WHITE;
//
//	pad = 36;
//	y += pad;
//	s_teammain.joinblue.generic.type			= MTYPE_BITMAP;
//	s_teammain.joinblue.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
//	s_teammain.joinblue.generic.x				= x;
//	s_teammain.joinblue.generic.y				= y;
//	s_teammain.joinblue.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
//	s_teammain.joinblue.generic.id				= ID_JOINBLUE;
//	s_teammain.joinblue.generic.callback		= TeamMain_MenuEvent;
//	s_teammain.joinblue.width					= MENU_BUTTON_MED_WIDTH;
//	s_teammain.joinblue.height					= MENU_BUTTON_MED_HEIGHT;
//	s_teammain.joinblue.color					= CT_DKPURPLE1;
//	s_teammain.joinblue.color2					= CT_LTPURPLE1;
//	s_teammain.joinblue.textX					= MENU_BUTTON_TEXT_X;
//	s_teammain.joinblue.textY					= MENU_BUTTON_TEXT_Y;
//	s_teammain.joinblue.textEnum				= MBT_BLUETEAM;
//	s_teammain.joinblue.textcolor				= CT_BLACK;
//	s_teammain.joinblue.textcolor2				= CT_WHITE;
//
//	y += pad;
//	s_teammain.joinauto.generic.type			= MTYPE_BITMAP;
//	s_teammain.joinauto.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
//	s_teammain.joinauto.generic.x				= x;
//	s_teammain.joinauto.generic.y				= y;
//	s_teammain.joinauto.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
//	s_teammain.joinauto.generic.id				= ID_JOINAUTO;
//	s_teammain.joinauto.generic.callback		= TeamMain_MenuEvent;
//	s_teammain.joinauto.width					= MENU_BUTTON_MED_WIDTH;
//	s_teammain.joinauto.height					= MENU_BUTTON_MED_HEIGHT;
//	s_teammain.joinauto.color					= CT_DKPURPLE1;
//	s_teammain.joinauto.color2					= CT_LTPURPLE1;
//	s_teammain.joinauto.textX					= MENU_BUTTON_TEXT_X;
//	s_teammain.joinauto.textY					= MENU_BUTTON_TEXT_Y;
//	s_teammain.joinauto.textEnum				= MBT_AUTOTEAM;
//	s_teammain.joinauto.textcolor				= CT_BLACK;
//	s_teammain.joinauto.textcolor2				= CT_WHITE;
//
//	y += pad;
//	s_teammain.playerClass.generic.type				= MTYPE_SPINCONTROL;      
//	s_teammain.playerClass.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
//	s_teammain.playerClass.generic.x				= x - 14;
//	s_teammain.playerClass.generic.y				= y;
//	s_teammain.playerClass.generic.name				= GRAPHIC_SQUARE;
//	s_teammain.playerClass.generic.id				= ID_PLAYERCLASS;
//	s_teammain.playerClass.generic.callback			= TeamMain_MenuEvent; 
//	s_teammain.playerClass.color					= CT_DKPURPLE1;
//	s_teammain.playerClass.color2					= CT_LTPURPLE1;
//	s_teammain.playerClass.textX					= MENU_BUTTON_TEXT_X;
//	s_teammain.playerClass.textY					= MENU_BUTTON_TEXT_Y;
//	s_teammain.playerClass.textEnum					= MBT_PLAYERCLASS;
//	s_teammain.playerClass.textcolor				= CT_BLACK;
//	s_teammain.playerClass.textcolor2				= CT_WHITE;	
//
//	// If assimilation is on then use player_class2 which includes Borg as a class
///*	if (atoi( Info_ValueForKey( info, "g_pModAssimilation" )) == 0)
//	{
//		s_teammain.playerClass.listnames				= player_classes;
//	}
//	else
//	{
//		s_teammain.playerClass.listnames				= player_classes2;
//	}
//*/
//
//	/*if (atoi( Info_ValueForKey( info, "g_pModSpecialties" )) == 0)
//	{
//		s_teammain.playerClass.generic.flags |= QMF_GRAYED;
//		s_teammain.playerClass.listnames				= player_classes2;
//	}
//	else
//	{
//		s_teammain.playerClass.listnames				= player_classes;
//	}*/
//	
//	//RPG-X: RedTechie - Originaly was trying to grey out menu if n00b but this just remberes so nexts time they connect its still greyed out ROFL
//	/*if (!Q_stricmp( ui_playerClass.string, "N00B" ))
//	{
//		s_teammain.playerClass.generic.flags |= QMF_GRAYED;
//		s_teammain.playerClass.listnames				= player_classes;
//	}
//	else
//	{*/
//		s_teammain.playerClass.listnames				= player_classes;
//	/*}*/
//
//	s_teammain.playerClass.listX					= 188;
//
//	y += pad;
//	s_teammain.joingame.generic.type			= MTYPE_BITMAP;
//	s_teammain.joingame.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
//	s_teammain.joingame.generic.x				= x;
//	s_teammain.joingame.generic.y				= y;
//	s_teammain.joingame.generic.id				= ID_JOINGAME;
//	s_teammain.joingame.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
//	s_teammain.joingame.generic.callback		= TeamMain_MenuEvent;
//	s_teammain.joingame.width					= MENU_BUTTON_MED_WIDTH;
//	s_teammain.joingame.height					= MENU_BUTTON_MED_HEIGHT;
//	s_teammain.joingame.color					= CT_DKPURPLE1;
//	s_teammain.joingame.color2					= CT_LTPURPLE1;
//	s_teammain.joingame.textX					= MENU_BUTTON_TEXT_X;
//	s_teammain.joingame.textY					= MENU_BUTTON_TEXT_Y;
//	s_teammain.joingame.textEnum				= MBT_JOINMATCH;
//	s_teammain.joingame.textcolor				= CT_BLACK;
//	s_teammain.joingame.textcolor2				= CT_WHITE;
//
//	y += pad;
//	s_teammain.spectate.generic.type			= MTYPE_BITMAP;
//	s_teammain.spectate.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
//	s_teammain.spectate.generic.x				= x;
//	s_teammain.spectate.generic.y				= y;
//	s_teammain.spectate.generic.id				= ID_SPECTATE;
//	s_teammain.spectate.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
//	s_teammain.spectate.generic.callback		= TeamMain_MenuEvent;
//	s_teammain.spectate.width					= MENU_BUTTON_MED_WIDTH;
//	s_teammain.spectate.height					= MENU_BUTTON_MED_HEIGHT;
//	s_teammain.spectate.color					= CT_DKPURPLE1;
//	s_teammain.spectate.color2					= CT_LTPURPLE1;
//	s_teammain.spectate.textX					= MENU_BUTTON_TEXT_X;
//	s_teammain.spectate.textY					= MENU_BUTTON_TEXT_Y;
//	s_teammain.spectate.textEnum				= MBT_OBSERVER;
//	s_teammain.spectate.textcolor				= CT_BLACK;
//	s_teammain.spectate.textcolor2				= CT_WHITE;
//
//
//	gametype = atoi( Info_ValueForKey( info,"g_gametype" ) );
//			      
//	// set initial states
//	switch( gametype ) 
//	{
//	case GT_SINGLE_PLAYER:
//	case GT_FFA:
//	case GT_TOURNAMENT:
//		s_teammain.joinred.generic.flags  |= QMF_GRAYED;
//		s_teammain.joinblue.generic.flags |= QMF_GRAYED;
//		s_teammain.joinauto.generic.flags |= QMF_GRAYED;
////		s_teammain.playerClass.generic.flags |= QMF_GRAYED;
//		break;
//
//	default:
//	case GT_TEAM:
//	case GT_CTF:
//		s_teammain.joingame.generic.flags |= QMF_GRAYED;
//		break;
//	}
//
//
//	Menu_AddItem( &s_teammain.menu, (void*) &s_teammain.ingamemenu );
//	Menu_AddItem( &s_teammain.menu, (void*) &s_teammain.joinred );
//	Menu_AddItem( &s_teammain.menu, (void*) &s_teammain.joinblue );
//	Menu_AddItem( &s_teammain.menu, (void*) &s_teammain.joinauto );
//	Menu_AddItem( &s_teammain.menu, (void*) &s_teammain.playerClass );
//	Menu_AddItem( &s_teammain.menu, (void*) &s_teammain.joingame );
//	Menu_AddItem( &s_teammain.menu, (void*) &s_teammain.spectate );
//
//	// Set up current value based on class
//	if ( !Q_stricmp( ui_playerClass.string, "NOCLASS" ) ) 
//	{
//		s_teammain.playerClass.curvalue = TM_NOCLASS;
//	}
//	else if ( !Q_stricmp( ui_playerClass.string, "SECURITY" ) ) 
//	{
//		s_teammain.playerClass.curvalue = TM_INFILTRATOR;
//	}
//	else if ( !Q_stricmp( ui_playerClass.string, "MEDICAL" ) ) 
//	{
//		s_teammain.playerClass.curvalue = TM_SNIPER;
//	}
//	else if ( !Q_stricmp( ui_playerClass.string, "ALIEN" ) ) 
//	{
//		s_teammain.playerClass.curvalue = TM_HEAVY;
//	}
//	else if ( !Q_stricmp( ui_playerClass.string, "COMMAND" ) ) 
//	{
//		s_teammain.playerClass.curvalue = TM_DEMO;
//	}
//	else if ( !Q_stricmp( ui_playerClass.string, "SCIENCE" ) ) 
//	{
//		s_teammain.playerClass.curvalue = TM_MEDIC;
//	}
//	else if ( !Q_stricmp( ui_playerClass.string, "ENGINEER" ) ) 
//	{
//		s_teammain.playerClass.curvalue = TM_TECH;
//	}
//	else if ( !Q_stricmp( ui_playerClass.string, "MARINE" ) ) 
//	{
//		s_teammain.playerClass.curvalue = TM_ALPHAOMEGA22;
//	}
//	else if ( !Q_stricmp( ui_playerClass.string, "N00B" ) ) 
//	{
//		s_teammain.playerClass.curvalue = TM_N00B;
//	}
//	else
//	{
//	/*else if ( !Q_stricmp( ui_playerClass.string, "BORG" ) ) 
//	{
//		s_teammain.playerClass.curvalue = TM_BORG;
//	}
//	else if ( !Q_stricmp( ui_playerClass.string, "HERO" ) ) 
//	{
//		s_teammain.playerClass.curvalue = TM_ACTIONHERO;
//	}
//	else
//	{*/
//		s_teammain.playerClass.curvalue = TM_NOCLASS;
//	}
//
//	s_teammain.holdClass = s_teammain.playerClass.curvalue;
//
//}
//
//
///*
//===============
//TeamMain_Cache
//===============
//*/
//void TeamMain_Cache( void ) 
//{
//	s_teammain.pClassShaders[TM_NOCLASS] = trap_R_RegisterShaderNoMip( "menu/art/pc_noclass.tga" );//PC_NOCLASS,
//	s_teammain.pClassShaders[TM_INFILTRATOR] = trap_R_RegisterShaderNoMip( "menu/art/pc_security.tga" );//PC_INFILTRATOR,//fast, low attack
//	s_teammain.pClassShaders[TM_SNIPER] = trap_R_RegisterShaderNoMip( "menu/art/pc_medical.tga" );//PC_SNIPER,//sneaky, snipe only
//	s_teammain.pClassShaders[TM_HEAVY] = trap_R_RegisterShaderNoMip( "menu/art/pc_alien.tga" );//PC_HEAVY,//slow, heavy attack
//	s_teammain.pClassShaders[TM_DEMO] = trap_R_RegisterShaderNoMip( "menu/art/pc_command.tga" );//PC_DEMO,//go boom
//	s_teammain.pClassShaders[TM_MEDIC] = trap_R_RegisterShaderNoMip( "menu/art/pc_science.tga" );//PC_MEDIC,//heal
//	s_teammain.pClassShaders[TM_TECH] = trap_R_RegisterShaderNoMip( "menu/art/pc_engineer.tga" );//PC_TECH,//operate
//	s_teammain.pClassShaders[TM_ALPHAOMEGA22] = trap_R_RegisterShaderNoMip( "menu/art/pc_marine.tga" );//PC_MEDIC,//heal
//	s_teammain.pClassShaders[TM_N00B] = trap_R_RegisterShaderNoMip( "menu/art/pc_n00b.tga" );//PC_TECH,//operate
//	/*s_teammain.pClassShaders[TM_BORG] = trap_R_RegisterShaderNoMip( "menu/art/pc_borg.tga" );//PC_BORG,//special weapons, slower, adapting shields
//	s_teammain.pClassShaders[TM_ACTIONHERO] = trap_R_RegisterShaderNoMip( "menu/art/pc_hero.tga" );//PC_ACTIONHERO,//has everything*/
//}
//
//
///*
//===============
//UI_TeamMainMenu
//===============
//*/
//void UI_TeamMainMenu( void )
//{
//	TeamMain_MenuInit();
//	UI_PushMenu ( &s_teammain.menu );
//}
