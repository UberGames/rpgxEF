// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=======================================================================

CD KEY MENU

=======================================================================
*/


#include "ui_local.h"

#define ID_CDKEY		10
#define ID_ACCEPT		11
#define ID_ACCEPTLATER	12


typedef struct {
	menuframework_s	menu;

	menufield_s		cdkey;

	menubitmap_s	accept;
	menubitmap_s	acceptlater;
	int				keyResult;

	qhandle_t		halfround;
	int				fromMenu;
} cdkeyMenuInfo_t;

static cdkeyMenuInfo_t	cdkeyMenuInfo;

/*
=================
UI_CDKeyMenu_PreValidateKey
=================
*/
static int UI_CDKeyMenu_PreValidateKey( const char *key ) 
{
	int		cnt, i;

	// With dashes can't be more than 21 long
	if( strlen( key ) > 22 ) 
	{
		return -1;
	}

	cnt = 0;
	for(i = 0; i < strlen( key ) && key[i]; i++)
	{
		if ( ( key[i] >= '0' && key[i] <= '9' )
			|| ( key[i] >= 'a' && key[i] <= 'z' )
			|| ( key[i] >= 'A' && key[i] <= 'Z' )
			)
		{
			cnt++;
		}
	}

	// Count without dashes (Should be 18 but 0's at the end weren't printed on the documentation so we're allowing 16 and adding the 0's in the keycheck)
	if ((cnt < 16) || (cnt > 18))
//	if (cnt != 18)
	{
		return -1;
	}
	return 0;
}


/*
===============
UI_CDKeyMenu_Event
===============
*/
static void UI_CDKeyMenu_Event( void *ptr, int event ) 
{
	if( event != QM_ACTIVATED ) 
	{
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) 
	{
	case ID_ACCEPT:

		cdkeyMenuInfo.keyResult = UI_CDKeyMenu_PreValidateKey( cdkeyMenuInfo.cdkey.field.buffer);

		if (cdkeyMenuInfo.keyResult == 0)
		{//passed quick check, now check for real
			if(trap_SetCDKey( cdkeyMenuInfo.cdkey.field.buffer ))
			{//success
				trap_Cvar_Set( "ui_cdkeychecked", "1" );
				if (cdkeyMenuInfo.fromMenu)
				{
					UI_PopMenu();
				}
			}
			else
			{//failed real check
				cdkeyMenuInfo.keyResult = -1;
				trap_S_StartLocalSound( menu_buzz_sound, CHAN_MENU1);	// WRONG!!!! sound
			}
		}
		else if (cdkeyMenuInfo.keyResult < 0)
		{//failed ui quick check
			trap_S_StartLocalSound( menu_buzz_sound, CHAN_MENU1);	// WRONG!!!! sound
		}
		break;

	case ID_ACCEPTLATER:
		if (cdkeyMenuInfo.keyResult != 0) {	//not valid or not entered
			trap_Cvar_Set( "ui_cdkeychecked", "-1" );
		}
		UI_PopMenu();
		break;

	}
}


/*
=================
UI_CDKeyMenu_DrawKey
=================
*/
static void UI_CDKeyMenu_DrawKey( void *self ) 
{
	menufield_s		*f;
	qboolean		focus;
	int				style;
	char			c;
	float			*color;
	int				x, y;

	f = (menufield_s *)self;

	focus = (f->generic.parent->cursor == f->generic.menuPosition);

	style = UI_LEFT;
	if( focus ) 
	{
		color = colorTable[CT_LTGOLD1];
	}
	else 
	{
		color = colorTable[CT_DKGOLD1];
	}

	x = cdkeyMenuInfo.cdkey.generic.x;
	y = cdkeyMenuInfo.cdkey.generic.y;

	UI_FillRect( x, y, cdkeyMenuInfo.cdkey.field.widthInChars * SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, colorTable[CT_BLACK] );
	UI_FillRect( x, y, cdkeyMenuInfo.cdkey.field.widthInChars * SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, listbar_color );
	UI_DrawString( x, y, f->field.buffer, style, color, qtrue );

	// draw cursor if we have focus
	if( focus ) 
	{
		if ( trap_Key_GetOverstrikeMode() ) 
		{
			c = 11;
		} 
		else 
		{
			c = 10;
		}

		style &= ~UI_PULSE;
		style |= UI_BLINK;

		UI_DrawChar( x + f->field.cursor * SMALLCHAR_WIDTH, y, c, style, color_white );
	}

}

/*
=================
CDKeyMenu_Graphics
=================
*/
void CDKeyMenu_Graphics (void)
{
	int x,y;

	// Draw the basic screen layout
	UI_MenuFrame(&cdkeyMenuInfo.menu);

	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30,203, 47, 186, uis.whiteShader);	// Middle left line of frame

	trap_R_SetColor( colorTable[CT_DKPURPLE1]);
	UI_DrawHandlePic( 246, 238, 197,  24, uis.whiteShader);	
	UI_DrawHandlePic( 227, 238, -16,  32, cdkeyMenuInfo.halfround);	// Left round
	UI_DrawHandlePic( 446, 238,  16,  32, cdkeyMenuInfo.halfround);	// Right round

	UI_DrawProportionalString(  74,  66, "557",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "2344",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  188, "89-35",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  206, "32906",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "30-1789",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	x = 344;
	y = 279;
	if ( cdkeyMenuInfo.keyResult == 0 ) 
	{
		UI_DrawProportionalString( x, y, menu_normal_text[MNT_VALID_CDKEY], UI_CENTER|UI_SMALLFONT, colorTable[CT_WHITE]  );
	}
	else if( cdkeyMenuInfo.keyResult == 1 ) 
	{
		UI_DrawProportionalString( x, y, menu_normal_text[MNT_ENTER_CDKEY], UI_CENTER|UI_SMALLFONT, colorTable[CT_LTGOLD1] );
	}
	else 
	{
		UI_DrawProportionalString( x, y, menu_normal_text[MNT_CDKEY_INVALID], UI_CENTER|UI_SMALLFONT, colorTable[CT_RED]  );
	}

	UI_Setup_MenuButtons();

}

/*
=================
CDKey_MenuDraw
=================
*/
static void CDKey_MenuDraw (void)
{
	CDKeyMenu_Graphics();
	Menu_Draw( &cdkeyMenuInfo.menu );
}

/*
===============
UI_CDKeyMenu_Init
===============
*/
static void UI_CDKeyMenu_Init( void ) 
{
	memset( &cdkeyMenuInfo, 0, sizeof(cdkeyMenuInfo) );

	UI_CDKeyMenu_Cache();

	cdkeyMenuInfo.menu.wrapAround					= qtrue;
	cdkeyMenuInfo.menu.fullscreen					= qtrue;
    cdkeyMenuInfo.menu.draw							= CDKey_MenuDraw;
	cdkeyMenuInfo.menu.descX						= MENU_DESC_X;
	cdkeyMenuInfo.menu.descY						= MENU_DESC_Y;
	cdkeyMenuInfo.menu.titleX						= MENU_TITLE_X;
	cdkeyMenuInfo.menu.titleY						= MENU_TITLE_Y;
	cdkeyMenuInfo.menu.titleI						= MNT_CDKEYMENU_TITLE;
	cdkeyMenuInfo.menu.footNoteEnum					= MNT_CDKEY;

	cdkeyMenuInfo.cdkey.generic.type				= MTYPE_FIELD;
	cdkeyMenuInfo.cdkey.generic.flags				= QMF_UPPERCASE;
	cdkeyMenuInfo.cdkey.generic.x					= 256;
	cdkeyMenuInfo.cdkey.generic.y					= 242;
	cdkeyMenuInfo.cdkey.field.style					= UI_SMALLFONT;
	cdkeyMenuInfo.cdkey.field.widthInChars			= 22;
	cdkeyMenuInfo.cdkey.field.maxchars				= 22;
	cdkeyMenuInfo.cdkey.generic.ownerdraw			= UI_CDKeyMenu_DrawKey;
	cdkeyMenuInfo.cdkey.field.titleEnum				= MNT_CDKEY;
	cdkeyMenuInfo.cdkey.field.titlecolor			= CT_LTGOLD1;
	cdkeyMenuInfo.cdkey.field.textcolor				= CT_DKGOLD1;
	cdkeyMenuInfo.cdkey.field.textcolor2			= CT_LTGOLD1;


	cdkeyMenuInfo.accept.generic.type				= MTYPE_BITMAP;
	cdkeyMenuInfo.accept.generic.name				= GRAPHIC_SQUARE;
	cdkeyMenuInfo.accept.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	cdkeyMenuInfo.accept.generic.id					= ID_ACCEPT;
	cdkeyMenuInfo.accept.generic.callback			= UI_CDKeyMenu_Event;
	cdkeyMenuInfo.accept.generic.x					= 279;
	cdkeyMenuInfo.accept.generic.y					= 391;
	cdkeyMenuInfo.accept.width						= MENU_BUTTON_MED_WIDTH;
	cdkeyMenuInfo.accept.height						= 42;
	cdkeyMenuInfo.accept.color						= CT_DKPURPLE1;
	cdkeyMenuInfo.accept.color2						= CT_LTPURPLE1;
	cdkeyMenuInfo.accept.textX						= MENU_BUTTON_TEXT_X;
	cdkeyMenuInfo.accept.textY						= MENU_BUTTON_TEXT_Y;
	cdkeyMenuInfo.accept.textEnum					= MBT_ACCEPT;
	cdkeyMenuInfo.accept.textcolor					= CT_BLACK;
	cdkeyMenuInfo.accept.textcolor2					= CT_WHITE;


	SetupMenu_TopButtons(&cdkeyMenuInfo.menu,MENU_CDKEY,NULL);

	Menu_AddItem( &cdkeyMenuInfo.menu, &cdkeyMenuInfo.cdkey );
	Menu_AddItem( &cdkeyMenuInfo.menu, &cdkeyMenuInfo.accept );

	if (ui_cdkeychecked.integer == 1) {
		cdkeyMenuInfo.keyResult = 0; //already got a good one once
	} else {
		cdkeyMenuInfo.keyResult = 1;
	}
}


/*
=================
UI_CDKeyMenu_Cache
=================
*/
void UI_CDKeyMenu_Cache( void ) 
{
	cdkeyMenuInfo.halfround = trap_R_RegisterShaderNoMip( "menu/common/halfround_r_24.tga" );
}


/*
===============
UI_CDKeyMenu
===============
*/
void UI_CDKeyMenu( void ) 
{
	UI_CDKeyMenu_Init();
	UI_PushMenu( &cdkeyMenuInfo.menu );
}


/*
=================
M_CDKey2Menu_Key
=================
*/
sfxHandle_t M_CDKey2Menu_Key (int key)
{
	if (key == K_ESCAPE)
	{
		return(0);
	}

	return ( Menu_DefaultKey( &cdkeyMenuInfo.menu, key ) );
}

/*
=================
CDKeyMenu2_Graphics
=================
*/
void CDKeyMenu2_Graphics (void)
{
	int x,y;

	// Draw the basic screen layout
	UI_MenuFrame2(&cdkeyMenuInfo.menu);

	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30,203, 47, 186, uis.whiteShader);	// Middle left line of frame

	// Background for CD Key data
	trap_R_SetColor( colorTable[CT_DKPURPLE1]);
	UI_DrawHandlePic( 246, 168, 197,  24, uis.whiteShader);	
	UI_DrawHandlePic( 227, 168, -16,  32, cdkeyMenuInfo.halfround);	// Left round
	UI_DrawHandlePic( 446, 168,  16,  32, cdkeyMenuInfo.halfround);	// Right round

	
	UI_DrawProportionalString(  74,  36, "755",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  132, "4423",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  188, "35-89",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  206, "60932",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "1789-30",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	x = 344;
	y = 228;
	if ( cdkeyMenuInfo.keyResult == 0 ) 
	{
		UI_DrawProportionalString( x, y, menu_normal_text[MNT_VALID_CDKEY], UI_CENTER|UI_SMALLFONT, colorTable[CT_WHITE]  );
	}
	else if( cdkeyMenuInfo.keyResult == 1 ) 
	{
		UI_DrawProportionalString( x, y, menu_normal_text[MNT_ENTER_CDKEY], UI_CENTER|UI_SMALLFONT, colorTable[CT_LTGOLD1] );
	}
	else 
	{
		UI_DrawProportionalString( x, y, menu_normal_text[MNT_CDKEY_INVALID], UI_CENTER|UI_SMALLFONT, colorTable[CT_RED]  );
	}
}

/*
=================
CDKey2_MenuDraw
=================
*/
static void CDKey2_MenuDraw (void)
{
	CDKeyMenu2_Graphics();
	Menu_Draw( &cdkeyMenuInfo.menu );
}


/*
===============
UI_CDKeyMenu2_Init
===============
*/
static void UI_CDKeyMenu2_Init( void ) 
{//initial first time menu
	memset( &cdkeyMenuInfo, 0, sizeof(cdkeyMenuInfo) );

	cdkeyMenuInfo.fromMenu = 1;

	UI_CDKeyMenu_Cache();

	cdkeyMenuInfo.menu.wrapAround					= qtrue;
	cdkeyMenuInfo.menu.fullscreen					= qtrue;
    cdkeyMenuInfo.menu.draw							= CDKey2_MenuDraw;
    cdkeyMenuInfo.menu.key							= M_CDKey2Menu_Key;
	cdkeyMenuInfo.menu.descX						= MENU_DESC_X;
	cdkeyMenuInfo.menu.descY						= MENU_DESC_Y;
	cdkeyMenuInfo.menu.titleX						= MENU_TITLE_X;
	cdkeyMenuInfo.menu.titleY						= MENU_TITLE_Y;
	cdkeyMenuInfo.menu.titleI						= MNT_CDKEYMENU_TITLE;
	cdkeyMenuInfo.menu.footNoteEnum					= MNT_CDKEY;

	cdkeyMenuInfo.cdkey.generic.type				= MTYPE_FIELD;
	cdkeyMenuInfo.cdkey.generic.flags				= QMF_UPPERCASE;
	cdkeyMenuInfo.cdkey.generic.x					= 260;
	cdkeyMenuInfo.cdkey.generic.y					= 172;
	cdkeyMenuInfo.cdkey.field.style					= UI_SMALLFONT;
	cdkeyMenuInfo.cdkey.field.widthInChars			= 22;
	cdkeyMenuInfo.cdkey.field.maxchars				= 22;
	cdkeyMenuInfo.cdkey.generic.ownerdraw			= UI_CDKeyMenu_DrawKey;
	cdkeyMenuInfo.cdkey.field.titleEnum				= MNT_CDKEY;
	cdkeyMenuInfo.cdkey.field.titlecolor			= CT_LTGOLD1;
	cdkeyMenuInfo.cdkey.field.textcolor				= CT_DKGOLD1;
	cdkeyMenuInfo.cdkey.field.textcolor2			= CT_LTGOLD1;

	cdkeyMenuInfo.accept.generic.type					= MTYPE_BITMAP;
	cdkeyMenuInfo.accept.generic.name					= GRAPHIC_SQUARE;
	cdkeyMenuInfo.accept.generic.flags					= QMF_HIGHLIGHT_IF_FOCUS;
	cdkeyMenuInfo.accept.generic.id						= ID_ACCEPT;
	cdkeyMenuInfo.accept.generic.callback				= UI_CDKeyMenu_Event;
	cdkeyMenuInfo.accept.generic.x						= 96;
	cdkeyMenuInfo.accept.generic.y						= 347;
	cdkeyMenuInfo.accept.width							= MENU_BUTTON_MED_WIDTH;
	cdkeyMenuInfo.accept.height							= 42;
	cdkeyMenuInfo.accept.color							= CT_DKPURPLE1;
	cdkeyMenuInfo.accept.color2							= CT_LTPURPLE1;
	cdkeyMenuInfo.accept.textX							= MENU_BUTTON_TEXT_X;
	cdkeyMenuInfo.accept.textY							= MENU_BUTTON_TEXT_Y;
	cdkeyMenuInfo.accept.textEnum						= MBT_ACCEPT;
	cdkeyMenuInfo.accept.textcolor						= CT_BLACK;
	cdkeyMenuInfo.accept.textcolor2						= CT_WHITE;

	cdkeyMenuInfo.acceptlater.generic.type				= MTYPE_BITMAP;
	cdkeyMenuInfo.acceptlater.generic.name				= GRAPHIC_SQUARE;
	cdkeyMenuInfo.acceptlater.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	cdkeyMenuInfo.acceptlater.generic.id				= ID_ACCEPTLATER;
	cdkeyMenuInfo.acceptlater.generic.callback			= UI_CDKeyMenu_Event;
	cdkeyMenuInfo.acceptlater.generic.x					= 482;
	cdkeyMenuInfo.acceptlater.generic.y					= 347;
	cdkeyMenuInfo.acceptlater.width						= MENU_BUTTON_MED_WIDTH + 20;
	cdkeyMenuInfo.acceptlater.height					= 42;
	cdkeyMenuInfo.acceptlater.color						= CT_DKPURPLE1;
	cdkeyMenuInfo.acceptlater.color2					= CT_LTPURPLE1;
	cdkeyMenuInfo.acceptlater.textX						= MENU_BUTTON_TEXT_X;
	cdkeyMenuInfo.acceptlater.textY						= MENU_BUTTON_TEXT_Y;
	cdkeyMenuInfo.acceptlater.textEnum					= MBT_CHECKKEYLATER;
	cdkeyMenuInfo.acceptlater.textcolor					= CT_BLACK;
	cdkeyMenuInfo.acceptlater.textcolor2				= CT_WHITE;

	Menu_AddItem( &cdkeyMenuInfo.menu, &cdkeyMenuInfo.cdkey );
	Menu_AddItem( &cdkeyMenuInfo.menu, &cdkeyMenuInfo.accept );
	Menu_AddItem( &cdkeyMenuInfo.menu, &cdkeyMenuInfo.acceptlater );

	if (ui_cdkeychecked.integer == 1) {
		cdkeyMenuInfo.keyResult = 0; //already got a good one once
	} else {
		cdkeyMenuInfo.keyResult = 1;
	}
}

/*
===============
UI_CDKeyMenu
===============
*/
void UI_CDKeyMenu2( void ) 
{
	UI_CDKeyMenu2_Init();
	UI_PushMenu( &cdkeyMenuInfo.menu );
}

/*
===============
UI_CDKeyMenu_f
===============
*/
void UI_CDKeyMenu_f( void ) 
{
	UI_CDKeyMenu2();
}
