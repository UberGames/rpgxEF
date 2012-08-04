// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=======================================================================

NETWORK OPTIONS MENU

=======================================================================
*/

#include "ui_local.h"


#define ID_GRAPHICS			10
#define ID_DISPLAY			11
#define ID_SOUND			12
#define ID_NETWORK			13
#define ID_RATE				14
#define ID_BACK				15

/*
static const char *rate_items[] = {
	"<= 28.8K",
	"33.6K",
	"56K",
	"ISDN",
	"LAN/Cable/xDSL",
	0
};
*/
int rate_items[] =
{
	MNT_NW_28,
	MNT_NW_33,
	MNT_NW_56,
	MNT_NW_ISDN,
	MNT_NW_CABLE,
	0
};


typedef struct 
{
	menuframework_s	menu;

	menulist_s		rate;
	qhandle_t	swooshTop;
	qhandle_t	swooshBottom;

} networkOptionsInfo_t;

static networkOptionsInfo_t	networkOptionsInfo;


/*
=================
UI_NetworkOptionsMenu_Event
=================
*/
static void UI_NetworkOptionsMenu_Event( void* ptr, int event ) 
{
	if( event != QM_ACTIVATED ) 
	{
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) 
	{
	case ID_GRAPHICS:
		UI_PopMenu();
		UI_VideoDataMenu();		// Move to the Video Menu
		break;

	case ID_DISPLAY:
		UI_PopMenu();
		UI_VideoDataMenu();		// Move to the Video Menu
		break;

	case ID_SOUND:
		UI_PopMenu();
//		UI_SoundOptionsMenu();
		break;

	case ID_NETWORK:
		break;

	case ID_RATE:
		if( networkOptionsInfo.rate.curvalue == 0 ) 
		{
			trap_Cvar_SetValue( "rate", 2500 );
		}
		else if( networkOptionsInfo.rate.curvalue == 1 ) 
		{
			trap_Cvar_SetValue( "rate", 3000 );
		}
		else if( networkOptionsInfo.rate.curvalue == 2 ) 
		{
			trap_Cvar_SetValue( "rate", 4000 );
		}
		else if( networkOptionsInfo.rate.curvalue == 3 ) 
		{
			trap_Cvar_SetValue( "rate", 5000 );
		}
		else if( networkOptionsInfo.rate.curvalue == 4 ) 
		{
			trap_Cvar_SetValue( "rate", 25000 );
		}
		break;

	}
}


/*
=================
M_NetworkMenu_Graphics
=================
*/
void M_NetworkMenu_Graphics (void)
{
	UI_MenuFrame(&networkOptionsInfo.menu);

	UI_Setup_MenuButtons();

	UI_DrawProportionalString(  74,  66, "925",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "88PK",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  188, "8125",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  206, "358677",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "3-679",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);


	// Rest of Bottom1_Graphics
	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(  30, 203, 47, 69, uis.whiteShader);	// Top Left column above 
	UI_DrawHandlePic(  30, 275, 47, 66, uis.whiteShader);	// Top Left column middle
	UI_DrawHandlePic(  30, 344, 47, 45, uis.whiteShader);	// Top Left column below 

	// Brackets around Video Data
	trap_R_SetColor( colorTable[CT_LTPURPLE1]);
	UI_DrawHandlePic(158,163, 16, 16, uis.graphicBracket1CornerLU);
	UI_DrawHandlePic(158,179,  8, 233, uis.whiteShader);
	UI_DrawHandlePic(158,412, 16, -16, uis.graphicBracket1CornerLU);	//LD

	UI_DrawHandlePic(174,163, 320, 8, uis.whiteShader);	// Top line

	UI_DrawHandlePic(494,163, 128, 128, networkOptionsInfo.swooshTop);			// Top swoosh

	UI_DrawHandlePic(501,188, 110, 54, uis.whiteShader);	// Top right column
	UI_DrawHandlePic(501,245, 110, 100, uis.whiteShader);	// Middle right column
	UI_DrawHandlePic(501,348, 110, 55, uis.whiteShader);	// Bottom right column

	UI_DrawHandlePic(494,406, 128, 128, networkOptionsInfo.swooshBottom);		// Bottom swoosh

	UI_DrawHandlePic(174,420, 320, 8, uis.whiteShader);	// Bottom line


}

/*
=================
Network_MenuDraw
=================
*/
static void Network_MenuDraw (void)
{

	M_NetworkMenu_Graphics();

	Menu_Draw( &networkOptionsInfo.menu );
}

/*
===============
UI_NetworkOptionsMenu_Init
===============
*/
static void UI_NetworkOptionsMenu_Init( void ) 
{
	int		rate;

	memset( &networkOptionsInfo, 0, sizeof(networkOptionsInfo) );

	UI_NetworkOptionsMenu_Cache();

	networkOptionsInfo.menu.nitems						= 0;
	networkOptionsInfo.menu.wrapAround					= qtrue;
	networkOptionsInfo.menu.draw						= Network_MenuDraw;
	networkOptionsInfo.menu.fullscreen					= qtrue;
	networkOptionsInfo.menu.descX						= MENU_DESC_X;
	networkOptionsInfo.menu.descY						= MENU_DESC_Y;
	networkOptionsInfo.menu.listX						= 230;
	networkOptionsInfo.menu.listY						= 188;
	networkOptionsInfo.menu.titleX						= MENU_TITLE_X;
	networkOptionsInfo.menu.titleY						= MENU_TITLE_Y;
	networkOptionsInfo.menu.titleI						= MNT_CONTROLSMENU_TITLE;
	networkOptionsInfo.menu.footNoteEnum				= MNT_NETWORKSETUP;


	SetupMenu_TopButtons(&networkOptionsInfo.menu,MENU_NETWORK,NULL);

	networkOptionsInfo.rate.generic.type				= MTYPE_SPINCONTROL;
	networkOptionsInfo.rate.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	networkOptionsInfo.rate.generic.x					= 212;
	networkOptionsInfo.rate.generic.y					= 262;
	networkOptionsInfo.rate.textEnum					= MBT_DATARATE;
	networkOptionsInfo.rate.generic.callback			= UI_NetworkOptionsMenu_Event;
	networkOptionsInfo.rate.generic.id					= ID_RATE;
	networkOptionsInfo.rate.textcolor					= CT_BLACK;
	networkOptionsInfo.rate.textcolor2					= CT_WHITE;
	networkOptionsInfo.rate.color						= CT_DKPURPLE1;
	networkOptionsInfo.rate.color2						= CT_LTPURPLE1;
	networkOptionsInfo.rate.textX						= 5;
	networkOptionsInfo.rate.textY						= 2;
	networkOptionsInfo.rate.listnames					= rate_items;



	Menu_AddItem( &networkOptionsInfo.menu, ( void * ) &networkOptionsInfo.rate );

	rate = trap_Cvar_VariableValue( "rate" );
	if( rate <= 2500 ) 
	{
		networkOptionsInfo.rate.curvalue = 0;
	}
	else if( rate <= 3000 ) 
	{
		networkOptionsInfo.rate.curvalue = 1;
	}
	else if( rate <= 4000 ) 
	{
		networkOptionsInfo.rate.curvalue = 2;
	}
	else if( rate <= 5000 ) 
	{
		networkOptionsInfo.rate.curvalue = 3;
	}
	else 
	{
		networkOptionsInfo.rate.curvalue = 4;
	}
}


/*
===============
UI_NetworkOptionsMenu_Cache
===============
*/
void UI_NetworkOptionsMenu_Cache( void ) 
{
	networkOptionsInfo.swooshTop = trap_R_RegisterShaderNoMip("menu/common/swoosh_top.tga");
	networkOptionsInfo.swooshBottom= trap_R_RegisterShaderNoMip("menu/common/swoosh_bottom.tga");
}


/*
===============
UI_NetworkOptionsMenu
===============
*/
void UI_NetworkOptionsMenu( void ) 
{

	UI_NetworkOptionsMenu_Init();

	UI_PushMenu( &networkOptionsInfo.menu );

}
