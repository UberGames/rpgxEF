/**********************************************************************
	UI_TRANSPORTER.C

	User interface trigger from within game
**********************************************************************/
#include "ui_local.h"

typedef struct //static
{
	menuframework_s menu;
	menubitmap_s	quitmenu;

	int				maxhull;
	int				currhull;
	int				maxshield;
	int				currshield;
	int				shieldstate;
	int				warpstate;
	int				turbostate;
	int				transstate;
	int				alertstate;
} msd_t;

	msd_t	s_msd;

//s_msd.maxhull

#define ID_QUIT		10

void UI_msdMenu_Cache (void);

/*
=================
M_msd_Event
=================
*/
static void M_msd_Event (void* ptr, int notification)
{
	int	id;
	//menubitmap_s	*holdLocation;
	//menubitmap_s	*holdServer;

	id = ((menucommon_s*)ptr)->id;

	/*if ( notification != QM_ACTIVATED )
	{
		return;
	}*/

	if ((id == ID_QUIT) && (notification == QM_ACTIVATED))
		UI_PopMenu();
}

/*
=================
msdMenu_Key
=================
*/
sfxHandle_t msdMenu_Key (int key)
{
	return ( Menu_DefaultKey( &s_msd.menu, key ) );
}

extern qhandle_t			leftRound;
extern qhandle_t			corner_ul_24_60;
extern qhandle_t			corner_ll_12_60;
qhandle_t					loading1;
qhandle_t					loading2;
qhandle_t					loading3;
qhandle_t					loading4;
qhandle_t					loading5;

/*
=================
M_msdMenu_Graphics
=================
*/
static void M_msdMenu_Graphics (void)
{
	/*Notes:
	Color-Types in Frame:
	trap_R_SetColor( colorTable[CT_DKPURPLE3]);
	Blue Alert:
	trap_R_SetColor( colorTable[CT_BLUE] );
	Yellow Alert:
	trap_R_SetColor( colorTable[CT_YELLOW] );
	Red Alert:
	trap_R_SetColor( colorTable[CT_RED] );

	*/
	//menubitmap_s	*holdDeck;
	//int		i;
	int length,xTurboStart;
	//int		numColor, roundColor;

	// Draw the basic screen frame

	// Upper corners
	trap_R_SetColor( colorTable[CT_DKPURPLE1]);
	UI_DrawHandlePic( 20,  24,  64,  32, corner_ul_24_60);		// Upper corner

	// Lower corners
	if(s_msd.alertstate == 2)
		trap_R_SetColor( colorTable[CT_RED]);
	else if(s_msd.alertstate == 1)
		trap_R_SetColor( colorTable[CT_YELLOW]);
	else if(s_msd.alertstate == 3)
		trap_R_SetColor( colorTable[CT_BLUE] );
	else 
		trap_R_SetColor( colorTable[CT_DKPURPLE3]); //colorTable[CT_VDKPURPLE2]
	UI_DrawHandlePic( 20, 440,  64,  16, corner_ll_12_60);		// 

	xTurboStart = 604;
	length = UI_ProportionalStringWidth( menu_normal_text[MNT_TRANSPORTER],UI_BIGFONT);
	length += 4;

	// Upper half
	trap_R_SetColor( colorTable[CT_DKPURPLE1]); //DKGOLD1
	UI_DrawHandlePic( 79,  24, xTurboStart - (79 + length),  PROP_BIG_HEIGHT, uis.whiteShader);	// Top left line
	UI_DrawHandlePic( 20,  60,  60,  40, uis.whiteShader);		// 
	if(s_msd.alertstate == 2)
		trap_R_SetColor( colorTable[CT_RED]);
	else if(s_msd.alertstate == 1)
		trap_R_SetColor( colorTable[CT_YELLOW]);
	else if(s_msd.alertstate == 3)
		trap_R_SetColor( colorTable[CT_BLUE] );
	else 
		trap_R_SetColor( colorTable[CT_DKPURPLE3]);
	UI_DrawHandlePic( 20, 106,  60,  11, uis.whiteShader);		// 
	trap_R_SetColor( colorTable[CT_DKPURPLE1]);  //DKGOLD1
	UI_DrawHandlePic( 20, 123,  60, 250, uis.whiteShader);		//  Left hand column

	// Lower half
	if(s_msd.alertstate == 2)
		trap_R_SetColor( colorTable[CT_RED] );
	else if(s_msd.alertstate == 1)
		trap_R_SetColor( colorTable[CT_YELLOW] );
	else if(s_msd.alertstate == 3)
		trap_R_SetColor( colorTable[CT_BLUE] );
	else 
		trap_R_SetColor( colorTable[CT_DKPURPLE3] ); //colorTable[CT_VDKPURPLE2]
	UI_DrawHandlePic( 20, 380,  60,  70, uis.whiteShader);		// Left Column

	// Bottom line
	if(s_msd.alertstate == 2)
		trap_R_SetColor( colorTable[CT_RED] );
	else if(s_msd.alertstate == 1)
		trap_R_SetColor( colorTable[CT_YELLOW] );
	else if(s_msd.alertstate == 3)
		trap_R_SetColor( colorTable[CT_BLUE] );
	else 
		trap_R_SetColor( colorTable[CT_DKPURPLE3] ); //colorTable[CT_VDKPURPLE2]
	UI_DrawHandlePic( 69, 443, 287,   12, uis.whiteShader);		// 
	trap_R_SetColor( colorTable[CT_DKPURPLE1]); //colorTable[CT_DKGOLD1]
	UI_DrawHandlePic(364, 443, 260,   12, uis.whiteShader);		// Bottom line

	if(s_msd.alertstate == 2)
	UI_DrawProportionalString( xTurboStart, 24,
		"MASTER SYSTEMS DISPLAY",   UI_BIGFONT | UI_RIGHT, colorTable[CT_RED]);	
	else if(s_msd.alertstate == 1)
	UI_DrawProportionalString( xTurboStart, 24,
		"MASTER SYSTEMS DISPLAY",   UI_BIGFONT | UI_RIGHT, colorTable[CT_YELLOW]);	
	else if(s_msd.alertstate == 3)
	UI_DrawProportionalString( xTurboStart, 24,
		"MASTER SYSTEMS DISPLAY",   UI_BIGFONT | UI_RIGHT, colorTable[CT_BLUE]);
	else 
	UI_DrawProportionalString( xTurboStart, 24,
		"MASTER SYSTEMS DISPLAY",   UI_BIGFONT | UI_RIGHT, colorTable[CT_WHITE]);		

	trap_R_SetColor( colorTable[CT_DKPURPLE1]); //DKGOLD1
	UI_DrawHandlePic( 607,  24,-16,   32, leftRound);

	trap_R_SetColor( colorTable[CT_LTBLUE1] );
	UI_DrawHandlePic( 366, 208, 243, 212, loading2 );
	trap_R_SetColor( colorTable[CT_LTGOLD1] );
	UI_DrawHandlePic( 366, 208, 243, 212, loading3 );
	UI_DrawHandlePic( 366, 208, 243, 212, loading1 );
	trap_R_SetColor( colorTable[CT_DKBLUE1] );
	UI_DrawHandlePic( 366, 208, 243, 212, loading4 );
	trap_R_SetColor( colorTable[CT_DKORANGE] );
	UI_DrawHandlePic( 366, 208, 243, 212, loading5 );

	UI_DrawProportionalString(487, 297, "1", UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(508, 297, "2", UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(487, 322, "3", UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(508, 322, "4", UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(395, 405, "22", UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(370, 327, "45", UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(399, 236, "7", UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(487, 215, "35", UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(502, 215, "2", UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(596, 318, "67", UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(502, 405, "27", UI_TINYFONT, colorTable[CT_BLACK]);

}

/*
===============
msdMenu_Draw
===============
*/
static void msdMenu_Draw(void)
{
	// Draw graphics particular to Main Menu
	M_msdMenu_Graphics();
	
	Menu_Draw( &s_msd.menu );
}

/*
===============
UI_msdMenu_Cache
===============
*/
void UI_msdMenu_Cache (void)
{	
	leftRound = trap_R_RegisterShaderNoMip("menu/common/halfroundl_24.tga");
	corner_ul_24_60 = trap_R_RegisterShaderNoMip("menu/common/corner_ul_24_60.tga");
	corner_ll_12_60 = trap_R_RegisterShaderNoMip("menu/common/corner_ll_12_60.tga");
	loading1 = trap_R_RegisterShaderNoMip("menu/new/nav_y.tga");
	loading2 = trap_R_RegisterShaderNoMip("menu/new/nav_mb.tga");
	loading3 = trap_R_RegisterShaderNoMip("menu/new/nav_lb.tga");
	loading4 = trap_R_RegisterShaderNoMip("menu/new/nav_db.tga");
	loading5 = trap_R_RegisterShaderNoMip("menu/new/nab_o.tga");
}

/*
===============
msdMenu_Init
===============
*/
void msdMenu_Init(void)
{
	int y,pad,x;
	//int i;
	int width;

	s_msd.menu.nitems				= 0;
	s_msd.menu.draw					= msdMenu_Draw;
	s_msd.menu.key					= msdMenu_Key;
	s_msd.menu.wrapAround			= qtrue;
	s_msd.menu.descX				= MENU_DESC_X;
	s_msd.menu.descY				= MENU_DESC_Y;
	s_msd.menu.titleX				= MENU_TITLE_X;
	s_msd.menu.titleY				= MENU_TITLE_Y;

	pad = PROP_BIG_HEIGHT + 10;
	width = MENU_BUTTON_MED_WIDTH-20;
	y = 72;
	x = 208;

	s_msd.quitmenu.generic.type		= MTYPE_BITMAP;
	s_msd.quitmenu.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
	s_msd.quitmenu.generic.x		= x;
	s_msd.quitmenu.generic.y		= y;
	s_msd.quitmenu.generic.name		= GRAPHIC_BUTTONLEFT;
	s_msd.quitmenu.generic.id		= ID_QUIT;
	s_msd.quitmenu.generic.callback	= M_msd_Event;
	s_msd.quitmenu.width			= width;
	s_msd.quitmenu.height			= PROP_BIG_HEIGHT;
	s_msd.quitmenu.color			= CT_DKPURPLE1;
	s_msd.quitmenu.color2			= CT_LTPURPLE1;
	s_msd.quitmenu.textX			= MENU_BUTTON_TEXT_X;
	s_msd.quitmenu.textY			= 12;
	s_msd.quitmenu.textEnum			= MBT_RETURNMENU;
	s_msd.quitmenu.textcolor		= CT_BLACK;
	s_msd.quitmenu.textcolor2		= CT_WHITE;
	s_msd.quitmenu.textStyle		= UI_TINYFONT;

	y -= (2*pad);
	x += width + 8;

	Menu_AddItem( &s_msd.menu, &s_msd.quitmenu );
}

/*
===============
UI_msdMenu
===============
*/
void UI_msdMenu(int maxhull, int currhull, int maxshield, int currshield, int shieldstate, int warpstate, int turbostate, int transstate, int alertstate)
{

	memset( &s_msd, 0, sizeof( s_msd ) );

	s_msd.maxhull = maxhull;
	s_msd.currhull = currhull;
	s_msd.maxshield = maxshield;
	s_msd.currshield = currshield;
	s_msd.shieldstate = shieldstate;
	s_msd.warpstate = warpstate;
	s_msd.turbostate = turbostate;
	s_msd.transstate = transstate;
	s_msd.alertstate = alertstate;

	uis.menusp = 0;

	ingameFlag = qtrue;	// true when in game menu is in use

	Mouse_Show();

	UI_msdMenu_Cache();

	msdMenu_Init(); 

	UI_PushMenu( &s_msd.menu );

	Menu_AdjustCursor( &s_msd.menu, 1 );	
}
