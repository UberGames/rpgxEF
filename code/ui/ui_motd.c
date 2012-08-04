/**********************************************************************
	UI_MOTD.C

	The MotD thing
    RPG-X - Marcin - 15/12/2008
**********************************************************************/

#include "../qcommon/stv_version.h"
#include "ui_local.h"

#define ID_CONTINUE			10
#define ID_DISCONNECT		11
#define ID_ARROWUP			100
#define ID_ARROWDOWN		101

#define MIN_MOTD_LINES 20
#define MAX_MOTD_LINES 256

extern void InGame_LeaveAction( qboolean result );

char motdtext[MAX_MOTD_LINES][256];
int motdtextnum;

typedef struct {
    menuframework_s	menu;
    menubitmap_s	quitmenu;
	menubitmap_s	disconnect;
	menubitmap_s	arrowdwn;
	menubitmap_s	arrowup;

	int				scrollnum;
	qhandle_t		halfroundl_22;
} motdstuff_t;

static motdstuff_t s_motdstuff;

/*
=================
M_Motd_Event
=================
*/
static void M_MotdMenu_Event( void *ptr, int notification )
{
	if( notification != QM_ACTIVATED )  {
		return;
	}

	switch ( ((menucommon_s*)ptr)->id ) {
	case ID_DISCONNECT:
		UI_ConfirmMenu( menu_normal_text[MNT_LEAVE_MATCH], 0, InGame_LeaveAction );
		break;
	case ID_CONTINUE:
		UI_PopMenu();
		break;
	case ID_ARROWUP:
		if ( s_motdstuff.scrollnum > 0 ) {
			s_motdstuff.scrollnum -= 3;
		}

		break;
	case ID_ARROWDOWN:
		if ( s_motdstuff.scrollnum + MIN_MOTD_LINES < motdtextnum ) {
			s_motdstuff.scrollnum += 3;
		}
	}	
}

/*
=================
MotdMenu_Key
=================
*/
sfxHandle_t MotdMenu_Key( int key )
{
    return ( Menu_DefaultKey( &s_motdstuff.menu, key ) );
}

/*
=================
M_MotdMenu_Graphics
=================
*/
static void M_MotdMenu_Graphics( void )
{
	int i;
    int x = 15;
    int y = 15;

	for (i = s_motdstuff.scrollnum; i < motdtextnum && i < (MIN_MOTD_LINES + s_motdstuff.scrollnum); ++i) {
        UI_DrawProportionalString( x, y, motdtext[i], UI_SMALLFONT | UI_LEFT, colorTable[CT_WHITE]);
        y += 21;
	}

	//UI_FrameBottom_Graphics();	// Bottom two thirds

	// Print version
	UI_DrawProportionalString(  371, 445, Q3_VERSION, UI_TINYFONT, colorTable[CT_BLACK]);

    trap_R_SetColor( colorTable[s_motdstuff.quitmenu.color] );
    UI_DrawHandlePic(s_motdstuff.quitmenu.generic.x - 14,
		s_motdstuff.quitmenu.generic.y, 
		MENU_BUTTON_MED_HEIGHT, s_motdstuff.quitmenu.height, uis.graphicButtonLeftEnd);
	UI_DrawHandlePic(s_motdstuff.disconnect.generic.x - 14,
		s_motdstuff.disconnect.generic.y, 
		MENU_BUTTON_MED_HEIGHT, s_motdstuff.disconnect.height, uis.graphicButtonLeftEnd);

	trap_R_SetColor( colorTable[CT_LTBLUE1]); //LTBROWN1]);
	UI_DrawHandlePic( 28,  440, 287,  17, uis.whiteShader);	// Bottom front Line

	UI_DrawHandlePic( 0,  440, 25, 25, s_motdstuff.halfroundl_22);
	UI_DrawHandlePic( 319,  440, 25, 25, uis.halfroundr_22);

	trap_R_SetColor(NULL);
}

/*
===============
MotdMenu_Draw
===============
*/
static void MotdMenu_Draw( void )
{
    M_MotdMenu_Graphics();
    Menu_Draw( &s_motdstuff.menu );
}

/*
===============
UI_MotdMenu_Cache
===============
*/
void UI_MotdMenu_Cache( void )
{
		s_motdstuff.halfroundl_22 = trap_R_RegisterShaderNoMip( "menu/common/halfroundl_22.tga" );
}

/*
===============
MotdMenu_Init
===============
*/
void MotdMenu_Init( void )
{
    s_motdstuff.menu.nitems         = 0;
    s_motdstuff.menu.draw           = MotdMenu_Draw;
    s_motdstuff.menu.key            = MotdMenu_Key;
    s_motdstuff.menu.wrapAround     = qtrue;
    s_motdstuff.menu.descX          = MENU_DESC_X;
    s_motdstuff.menu.descY          = MENU_DESC_Y;
    s_motdstuff.menu.titleX         = MENU_TITLE_X;
    s_motdstuff.menu.titleY         = MENU_TITLE_Y;

	s_motdstuff.quitmenu.generic.type			= MTYPE_BITMAP;      
	s_motdstuff.quitmenu.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_motdstuff.quitmenu.generic.x				= 500;
	s_motdstuff.quitmenu.generic.y				= 440;
	s_motdstuff.quitmenu.generic.name			= GRAPHIC_BUTTONRIGHT;
	s_motdstuff.quitmenu.generic.id				= ID_CONTINUE;
	s_motdstuff.quitmenu.generic.callback		= M_MotdMenu_Event; 
	s_motdstuff.quitmenu.width					= MENU_BUTTON_MED_WIDTH;
	s_motdstuff.quitmenu.height					= MENU_BUTTON_MED_HEIGHT;
	s_motdstuff.quitmenu.color					= CT_DKORANGE;
	s_motdstuff.quitmenu.color2					= CT_LTORANGE;
	s_motdstuff.quitmenu.textX					= MENU_BUTTON_TEXT_X;
	s_motdstuff.quitmenu.textY					= MENU_BUTTON_TEXT_Y;
	s_motdstuff.quitmenu.textEnum				= MBT_CONTINUE;
	s_motdstuff.quitmenu.textcolor				= CT_BLACK;
	s_motdstuff.quitmenu.textcolor2				= CT_WHITE;

	s_motdstuff.disconnect.generic.type			= MTYPE_BITMAP;      
	s_motdstuff.disconnect.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_motdstuff.disconnect.generic.x			= 350;
	s_motdstuff.disconnect.generic.y			= 440;
	s_motdstuff.disconnect.generic.name			= GRAPHIC_BUTTONRIGHT;
	s_motdstuff.disconnect.generic.id			= ID_DISCONNECT;
	s_motdstuff.disconnect.generic.callback		= M_MotdMenu_Event; 
	s_motdstuff.disconnect.width				= MENU_BUTTON_MED_WIDTH;
	s_motdstuff.disconnect.height				= MENU_BUTTON_MED_HEIGHT;
	s_motdstuff.disconnect.color				= CT_DKORANGE;
	s_motdstuff.disconnect.color2				= CT_LTORANGE;
	s_motdstuff.disconnect.textX				= MENU_BUTTON_TEXT_X;
	s_motdstuff.disconnect.textY				= MENU_BUTTON_TEXT_Y;
	s_motdstuff.disconnect.textEnum				= MBT_INGAMELEAVE;
	s_motdstuff.disconnect.textcolor			= CT_BLACK;
	s_motdstuff.disconnect.textcolor2			= CT_WHITE;

	s_motdstuff.arrowup.generic.type			= MTYPE_BITMAP;      
	s_motdstuff.arrowup.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_motdstuff.arrowup.generic.x				= 610;
	s_motdstuff.arrowup.generic.y				= 10;
	s_motdstuff.arrowup.generic.name			= "menu/common/arrow_up_16.tga";
	s_motdstuff.arrowup.generic.id				= ID_ARROWUP;
	s_motdstuff.arrowup.generic.callback		= M_MotdMenu_Event;
	s_motdstuff.arrowup.width					= 16;
	s_motdstuff.arrowup.height					= 16;
	s_motdstuff.arrowup.color					= CT_DKBLUE1;
	s_motdstuff.arrowup.color2					= CT_LTBLUE1;
	s_motdstuff.arrowup.textX					= 0;
	s_motdstuff.arrowup.textY					= 0;
	s_motdstuff.arrowup.textEnum				= MBT_NONE;
	s_motdstuff.arrowup.textcolor				= CT_BLACK;
	s_motdstuff.arrowup.textcolor2				= CT_WHITE;

	s_motdstuff.arrowdwn.generic.type			= MTYPE_BITMAP;      
	s_motdstuff.arrowdwn.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_motdstuff.arrowdwn.generic.x				= 610;
	s_motdstuff.arrowdwn.generic.y				= 420;
	s_motdstuff.arrowdwn.generic.name			= "menu/common/arrow_dn_16.tga";
	s_motdstuff.arrowdwn.generic.id			= ID_ARROWDOWN;
	s_motdstuff.arrowdwn.generic.callback		= M_MotdMenu_Event;
	s_motdstuff.arrowdwn.width					= 16;
	s_motdstuff.arrowdwn.height				= 16;
	s_motdstuff.arrowdwn.color					= CT_DKBLUE1;
	s_motdstuff.arrowdwn.color2				= CT_LTBLUE1;
	s_motdstuff.arrowdwn.textX					= 0;
	s_motdstuff.arrowdwn.textY					= 0;
	s_motdstuff.arrowdwn.textEnum				= MBT_NONE;
	s_motdstuff.arrowdwn.textcolor				= CT_BLACK;
	s_motdstuff.arrowdwn.textcolor2			= CT_WHITE;

    Menu_AddItem( &s_motdstuff.menu, &s_motdstuff.quitmenu );
	Menu_AddItem( &s_motdstuff.menu, &s_motdstuff.disconnect );
	Menu_AddItem( &s_motdstuff.menu, &s_motdstuff.arrowup );
	Menu_AddItem( &s_motdstuff.menu, &s_motdstuff.arrowdwn );
}

/*
===============
MotdReset
===============
*/
void MotdReset( void )
{
	int i;

	motdtextnum = 0;
	for ( i = 0; i < MAX_MOTD_LINES; ++i ) {
		*motdtext[i] = '\0';
	}
}

/*
===============
MotdReceiveLine
===============
*/
void MotdReceiveLine( const char *txt )
{
	Q_strncpyz(motdtext[motdtextnum], txt, sizeof (motdtext[motdtextnum]));
	motdtextnum++;
}

/*
===============
UI_MotdMenu
===============
*/
void UI_MotdMenu( void )
{
    memset( &s_motdstuff, 0, sizeof( s_motdstuff ) );

    uis.menusp = 0;

    ingameFlag = qtrue;

    Mouse_Show();

    UI_MotdMenu_Cache();

    MotdMenu_Init();

    UI_PushMenu( &s_motdstuff.menu );

    Menu_AdjustCursor( &s_motdstuff.menu, 1 );
}
