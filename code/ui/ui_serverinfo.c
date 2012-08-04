// Copyright (C) 1999-2000 Id Software, Inc.
//
#include "ui_local.h"

#define	MAX_VIDEODRIVER_LINES	20

#define ID_ADD			100
#define ID_MAINMENU		101
#define	ID_ARROWUP		102
#define	ID_ARROWDWN		103


typedef struct
{
	menuframework_s	menu;
	menubitmap_s	mainmenu;
	menubitmap_s	add;
	char			info[MAX_INFO_STRING];
	qhandle_t		cornerLower;
	int				lineCnt;	
	int				lineStartCnt;
	menubitmap_s	arrowdwn;
	menubitmap_s	arrowup;
	menutext_s		key1;
	menutext_s		key2;
	menutext_s		key3;
	menutext_s		key4;
	menutext_s		key5;
	menutext_s		key6;
	menutext_s		key7;
	menutext_s		key8;
	menutext_s		key9;
	menutext_s		key10;
	menutext_s		key11;
	menutext_s		key12;
	menutext_s		key13;
	menutext_s		key14;
	menutext_s		key15;
	menutext_s		key16;
	menutext_s		key17;
	menutext_s		key18;
	menutext_s		key19;
	menutext_s		key20;
	menutext_s		value1;
	menutext_s		value2;
	menutext_s		value3;
	menutext_s		value4;
	menutext_s		value5;
	menutext_s		value6;
	menutext_s		value7;
	menutext_s		value8;
	menutext_s		value9;
	menutext_s		value10;
	menutext_s		value11;
	menutext_s		value12;
	menutext_s		value13;
	menutext_s		value14;
	menutext_s		value15;
	menutext_s		value16;
	menutext_s		value17;
	menutext_s		value18;
	menutext_s		value19;
	menutext_s		value20;
	char			key[MAX_VIDEODRIVER_LINES][MAX_INFO_KEY];
	char			value[MAX_VIDEODRIVER_LINES][MAX_INFO_VALUE];
} serverinfo_t;

static serverinfo_t	s_serverinfo;

static void ServerInfo_LinePrep( void);

/*
=================
Favorites_Add

Add current server to favorites
=================
*/
void Favorites_Add( void )
{
	char	adrstr[128];
	char	serverbuff[128];
	int		i;
	int		best;

	trap_Cvar_VariableStringBuffer( "cl_currentServerAddress", serverbuff, sizeof(serverbuff) );
	if (!serverbuff[0])
		return;

	best = 0;
	for (i=0; i<MAX_FAVORITESERVERS; i++)
	{
		trap_Cvar_VariableStringBuffer( va("server%d",i+1), adrstr, sizeof(adrstr) );
		if (!Q_stricmp(serverbuff,adrstr))
		{
			// already in list
			return;
		}
		
		// use first empty or non-numeric available slot
		if ((adrstr[0]  < '0' || adrstr[0] > '9' ) && !best)
			best = i+1;
	}

	if (best)
		trap_Cvar_Set( va("server%d",best), serverbuff);
}


/*
=================
ServerInfo_Event
=================
*/
static void ServerInfo_Event( void* ptr, int event )
{
	switch (((menucommon_s*)ptr)->id)
	{
		case ID_ADD:
			if (event != QM_ACTIVATED)
				break;
		
			Favorites_Add();
			UI_PopMenu();
			break;

		case ID_MAINMENU:
			if (event != QM_ACTIVATED)
				break;

			UI_PopMenu();
			break;

		case ID_ARROWUP:
			if (event != QM_ACTIVATED)
				break;
			s_serverinfo.lineStartCnt--;
			if (s_serverinfo.lineStartCnt>=0)
			{
				ServerInfo_LinePrep();
				s_serverinfo.arrowdwn.generic.flags &= ~QMF_HIDDEN;
				s_serverinfo.arrowdwn.generic.flags &= ~QMF_INACTIVE;
			}
			else
			{
				s_serverinfo.arrowup.generic.flags |= QMF_HIDDEN|QMF_INACTIVE;
				s_serverinfo.lineStartCnt=0;
			}
			break;

		case ID_ARROWDWN:
			if (event != QM_ACTIVATED)
				break;
			s_serverinfo.lineStartCnt++;
			if ((s_serverinfo.lineStartCnt + MAX_VIDEODRIVER_LINES)  <=s_serverinfo.lineCnt)
			{
				ServerInfo_LinePrep();
				s_serverinfo.arrowup.generic.flags &= ~QMF_HIDDEN;
				s_serverinfo.arrowup.generic.flags &= ~QMF_INACTIVE;
			}
			else
			{
				s_serverinfo.arrowdwn.generic.flags |= QMF_HIDDEN|QMF_INACTIVE;

				s_serverinfo.lineStartCnt = s_serverinfo.lineCnt - MAX_VIDEODRIVER_LINES; 
			}
			break;

	}
}

/*
=================
ServerInfoMenu_Graphics
=================
*/
void ServerInfoMenu_Graphics (void)
{
	// Draw the basic screen layout
	UI_MenuFrame2(&s_serverinfo.menu);

	trap_R_SetColor( colorTable[CT_LTPURPLE1]);
	UI_DrawHandlePic(30,202,  47, 188, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_LTPURPLE1]);
	UI_DrawHandlePic(250,400,  180, 20, uis.whiteShader);

}

/*
=================
ServerInfo_MenuDraw
=================
*/
static void ServerInfo_MenuDraw( void )
{
	ServerInfoMenu_Graphics();

	Menu_Draw( &s_serverinfo.menu );
}


/*
=================
ServerInfo_LinePrep
=================
*/
static void ServerInfo_LinePrep( void)
{
	int				i;
	const char		*s;
	char			key[MAX_INFO_KEY];
	char			value[MAX_INFO_VALUE];
	menutext_s		*hold_key,*hold_value;

	s = s_serverinfo.info;
	hold_key = &s_serverinfo.key1;
	hold_value = &s_serverinfo.value1;
	s_serverinfo.lineCnt = 0;
	i=0;
	while ( s ) 
	{
		Info_NextPair( &s, key, value );
		if (!key[0])
		{
			break;
		}

		Q_strcat( key, MAX_INFO_KEY, ":" ); 

		if ((s_serverinfo.lineStartCnt <= s_serverinfo.lineCnt) && (i < MAX_VIDEODRIVER_LINES))
		{
			Q_strncpyz(s_serverinfo.key[i], key, sizeof(key));
			Q_strncpyz(s_serverinfo.value[i], value, sizeof(value));
			i++;
		}

		s_serverinfo.lineCnt++;
	}
}

/*
=================
ServerInfo_MenuKey
=================
*/
static sfxHandle_t ServerInfo_MenuKey( int key )
{
	return ( Menu_DefaultKey( &s_serverinfo.menu, key ) );
}

/*
=================
ServerInfo_Cache
=================
*/
void ServerInfo_Cache( void )
{
	s_serverinfo.cornerLower = trap_R_RegisterShaderNoMip("menu/common/corner_ll_47_18.tga");

}

/*
=================
PlayerSettings_MenuInit
=================
*/
static void UI_ServerInfoMenu_Init(void) 
{
	int	i,x,y;
	menutext_s	*hold_key,*hold_value;

	// zero set all our globals
	memset( &s_serverinfo, 0 ,sizeof(serverinfo_t) );

	ServerInfo_Cache();

	s_serverinfo.menu.draw						= ServerInfo_MenuDraw;
	s_serverinfo.menu.key						= ServerInfo_MenuKey;
	s_serverinfo.menu.wrapAround				= qtrue;
	s_serverinfo.menu.fullscreen				= qtrue;
	s_serverinfo.menu.nitems					= 0;
	s_serverinfo.menu.descX						= MENU_DESC_X;
	s_serverinfo.menu.descY						= MENU_DESC_Y;
	s_serverinfo.menu.listX						= 230;
	s_serverinfo.menu.listY						= 188;
	s_serverinfo.menu.titleX					= MENU_TITLE_X;
	s_serverinfo.menu.titleY					= MENU_TITLE_Y;
	s_serverinfo.menu.titleI					= MNT_SERVERINFO_TITLE;
	s_serverinfo.menu.footNoteEnum				= MNT_SERVERINFO;

	y = 400;
	// Button Data
	s_serverinfo.mainmenu.generic.type			= MTYPE_BITMAP;      
	s_serverinfo.mainmenu.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_serverinfo.mainmenu.generic.x				= 90;
	s_serverinfo.mainmenu.generic.y				= 400;
	s_serverinfo.mainmenu.generic.name			= GRAPHIC_SQUARE;
	s_serverinfo.mainmenu.generic.id			= ID_MAINMENU;
	s_serverinfo.mainmenu.generic.callback		= ServerInfo_Event;
	s_serverinfo.mainmenu.width					= MENU_BUTTON_MED_WIDTH;
	s_serverinfo.mainmenu.height				= MENU_BUTTON_MED_HEIGHT;
	s_serverinfo.mainmenu.color					= CT_DKPURPLE1;
	s_serverinfo.mainmenu.color2				= CT_LTPURPLE1;
	s_serverinfo.mainmenu.textX					= MENU_BUTTON_TEXT_X;
	s_serverinfo.mainmenu.textY					= MENU_BUTTON_TEXT_Y;
	s_serverinfo.mainmenu.textEnum				= MBT_INGAMEMENU;
	s_serverinfo.mainmenu.textcolor				= CT_BLACK;
	s_serverinfo.mainmenu.textcolor2			= CT_WHITE;

	s_serverinfo.add.generic.type				= MTYPE_BITMAP;      
	s_serverinfo.add.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_serverinfo.add.generic.x					= 460;
	s_serverinfo.add.generic.y					= y;
	s_serverinfo.add.generic.name				= GRAPHIC_SQUARE;
	s_serverinfo.add.generic.id					= ID_ADD;
	s_serverinfo.add.generic.callback			= ServerInfo_Event;
	s_serverinfo.add.width						= MENU_BUTTON_MED_WIDTH;
	s_serverinfo.add.height						= MENU_BUTTON_MED_HEIGHT;
	s_serverinfo.add.color						= CT_DKPURPLE1;
	s_serverinfo.add.color2						= CT_LTPURPLE1;
	s_serverinfo.add.textX						= MENU_BUTTON_TEXT_X;
	s_serverinfo.add.textY						= MENU_BUTTON_TEXT_Y;
	s_serverinfo.add.textEnum					= MBT_ADDTOFAVS;
	s_serverinfo.add.textcolor					= CT_BLACK;
	s_serverinfo.add.textcolor2					= CT_WHITE;
	
	s_serverinfo.arrowup.generic.type			= MTYPE_BITMAP;      
	s_serverinfo.arrowup.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_serverinfo.arrowup.generic.x				= 270;
	s_serverinfo.arrowup.generic.y				= y+2;
	s_serverinfo.arrowup.generic.name			= "menu/common/arrow_up_16.tga";
	s_serverinfo.arrowup.generic.id				= ID_ARROWUP;
	s_serverinfo.arrowup.generic.callback		= ServerInfo_Event;
	s_serverinfo.arrowup.width					= 16;
	s_serverinfo.arrowup.height					= 16;
	s_serverinfo.arrowup.color					= CT_DKBLUE1;
	s_serverinfo.arrowup.color2					= CT_LTBLUE1;
	s_serverinfo.arrowup.textX					= 0;
	s_serverinfo.arrowup.textY					= 0;
	s_serverinfo.arrowup.textEnum				= MBT_NONE;
	s_serverinfo.arrowup.textcolor				= CT_BLACK;
	s_serverinfo.arrowup.textcolor2				= CT_WHITE;

	s_serverinfo.arrowdwn.generic.type			= MTYPE_BITMAP;      
	s_serverinfo.arrowdwn.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_serverinfo.arrowdwn.generic.x				= 394;
	s_serverinfo.arrowdwn.generic.y				= y+2;
	s_serverinfo.arrowdwn.generic.name			= "menu/common/arrow_dn_16.tga";
	s_serverinfo.arrowdwn.generic.id			= ID_ARROWDWN;
	s_serverinfo.arrowdwn.generic.callback		= ServerInfo_Event;
	s_serverinfo.arrowdwn.width					= 16;
	s_serverinfo.arrowdwn.height				= 16;
	s_serverinfo.arrowdwn.color					= CT_DKBLUE1;
	s_serverinfo.arrowdwn.color2				= CT_LTBLUE1;
	s_serverinfo.arrowdwn.textX					= 0;
	s_serverinfo.arrowdwn.textY					= 0;
	s_serverinfo.arrowdwn.textEnum				= MBT_NONE;
	s_serverinfo.arrowdwn.textcolor				= CT_BLACK;
	s_serverinfo.arrowdwn.textcolor2			= CT_WHITE;

	Menu_AddItem( &s_serverinfo.menu, (void*) &s_serverinfo.mainmenu );
	Menu_AddItem( &s_serverinfo.menu, (void*) &s_serverinfo.add );
	Menu_AddItem( &s_serverinfo.menu,( void * ) &s_serverinfo.arrowup);
	Menu_AddItem( &s_serverinfo.menu, ( void * ) &s_serverinfo.arrowdwn);

	s_serverinfo.lineStartCnt = 0;

	hold_key = &s_serverinfo.key1;
	hold_value = &s_serverinfo.value1;
	y = 62;
	x = SCREEN_WIDTH*0.50;
	for (i=0;i<MAX_VIDEODRIVER_LINES;i++)
	{
		hold_key->generic.x				= x-8;
		hold_key->generic.y				= y;
		hold_key->generic.type			= MTYPE_TEXT;      
		hold_key->generic.flags			= UI_RIGHT|UI_SMALLFONT;
		hold_key->color					= colorTable[CT_LTBLUE2];
		hold_key->string				= s_serverinfo.key[i];		
		hold_key->style					= UI_RIGHT|UI_SMALLFONT;	

		hold_value->generic.x			= x+8;
		hold_value->generic.y			= y;
		hold_value->generic.type		= MTYPE_TEXT;      
		hold_value->generic.flags		= UI_LEFT|UI_SMALLFONT;
		hold_value->color				= colorTable[CT_LTGOLD1];
		hold_value->string				= s_serverinfo.value[i];
		hold_value->style				= UI_LEFT|UI_SMALLFONT;	

		Menu_AddItem( &s_serverinfo.menu, ( void * ) hold_key);
		Menu_AddItem( &s_serverinfo.menu, ( void * ) hold_value);

		y += SMALLCHAR_HEIGHT;
		++hold_key;
		++hold_value;

	}


	if( trap_Cvar_VariableValue( "sv_running" ) ) 
	{
		s_serverinfo.add.generic.flags |= QMF_GRAYED;
	}

	trap_GetConfigString( CS_SERVERINFO, s_serverinfo.info, MAX_INFO_STRING );

	ServerInfo_LinePrep();

	s_serverinfo.arrowup.generic.flags |= QMF_HIDDEN|QMF_INACTIVE;

}

/*
=================
UI_ServerInfoMenu
=================
*/
void UI_ServerInfoMenu( void )
{

	UI_ServerInfoMenu_Init();

	UI_PushMenu( &s_serverinfo.menu );
}


