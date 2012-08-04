/**********************************************************************
	UI_TRANSPORTER.C

	User interface trigger from within game
**********************************************************************/
#include "ui_local.h"

char *delayList[20] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
						"11", "12", "13", "14", "15", "20", "30", "60", 0 };

char			srvList[6][MAX_QPATH];

typedef struct //static
{
	menuframework_s menu;
	sfxHandle_t		openingVoice;
	menubitmap_s	quitmenu;
	menubitmap_s	engage;
	menubitmap_s	engage2;
	menulist_s		locButton;
	menulist_s		srvButton;
	menulist_s		delButton;

	char			*srvListPtr[7];
	char			*mlist[MAX_QPATH];
	
	int				targetEntID;
} transporter_t;

transporter_t	s_transporter;

#define ID_COMPUTERVOICE	6

#define ID_QUIT		10
#define ID_ENGAGE	100
#define ID_ENGAGE2	101

extern adminGeneric_t s_adminGeneric;
extern void AdminGeneric_InitLists( void );

void UI_TransporterMenu_Cache (void);


/*static void TransCreateMaplist(void) {

}*/

/*
=================
TransDataReceived
=================
*/
void TransDataReceived(const char *data) {
	char *temp;
	int i;
	for(i = 0; i < 6; i++) {
		temp = Info_ValueForKey(data, va("d%i", i));
		if(!temp[0]) break;
		Q_strncpyz(srvList[i], temp, sizeof(srvList[i]));
	}
}

/*
=================
M_Transporter_Event
=================
*/
static void M_Transporter_Event (void* ptr, int notification)
{
	int	id;
	//menubitmap_s	*holdLocation;
	//menubitmap_s	*holdServer;

	id = ((menucommon_s*)ptr)->id;

	/*if ( notification != QM_ACTIVATED )
	{
		return;
	}*/

	switch (id)
	{
		case ID_QUIT:
			if ( notification == QM_ACTIVATED )
				UI_PopMenu();
			break;
		case ID_ENGAGE:		// Active only if a deck has been chosen
			if ( notification == QM_ACTIVATED ) {
				if(s_transporter.delButton.curvalue <= 16)
					trap_Cmd_ExecuteText( EXEC_APPEND, va("ui_transporterLoc %i %i %i", s_transporter.targetEntID, s_transporter.locButton.curvalue + 1, s_transporter.delButton.curvalue));
				else {
					switch(s_transporter.delButton.curvalue) {
						case 17:
							trap_Cmd_ExecuteText( EXEC_APPEND, va("ui_transporterLoc %i %i %i", s_transporter.targetEntID, s_transporter.locButton.curvalue + 1, 20));
							break;
						case 18:
							trap_Cmd_ExecuteText( EXEC_APPEND, va("ui_transporterLoc %i %i %i", s_transporter.targetEntID, s_transporter.locButton.curvalue + 1, 30));
							break;
						case 19:
							trap_Cmd_ExecuteText( EXEC_APPEND, va("ui_transporterLoc %i %i %i", s_transporter.targetEntID, s_transporter.locButton.curvalue + 1, 60));
							break;
					}
				}
				UI_ForceMenuOff();
			}
			break;
		case ID_ENGAGE2:		// Active only if a deck has been chosen
			if ( notification == QM_ACTIVATED ) {
				if(s_transporter.delButton.curvalue <= 16)
					trap_Cmd_ExecuteText( EXEC_APPEND, va("ui_transporterExt %i %i %i", s_transporter.targetEntID, s_transporter.srvButton.curvalue , s_transporter.delButton.curvalue));
				else {
					switch(s_transporter.delButton.curvalue) {
						case 17:
							trap_Cmd_ExecuteText( EXEC_APPEND, va("ui_transporterExt %i %i %i", s_transporter.targetEntID, s_transporter.srvButton.curvalue, 20));
							break;
						case 18:
							trap_Cmd_ExecuteText( EXEC_APPEND, va("ui_transporterExt %i %i %i", s_transporter.targetEntID, s_transporter.srvButton.curvalue, 30));
							break;
						case 19:
							trap_Cmd_ExecuteText( EXEC_APPEND, va("ui_transporterExt %i %i %i", s_transporter.targetEntID, s_transporter.srvButton.curvalue, 60));
							break;
					}
				}
				UI_ForceMenuOff();
			}
			break;
	}
}

/*
=================
TransporterMenu_Key
=================
*/
sfxHandle_t TransporterMenu_Key (int key)
{
	return ( Menu_DefaultKey( &s_transporter.menu, key ) );
}

extern qhandle_t			leftRound;
extern qhandle_t			corner_ul_24_60;
extern qhandle_t			corner_ll_12_60;
qhandle_t					transporter_sym;
qhandle_t					loading1;
qhandle_t					loading2;
qhandle_t					loading3;
qhandle_t					loading4;
qhandle_t					loading5;

/*
=================
M_TransporterMenu_Graphics
=================
*/
static void M_TransporterMenu_Graphics (void)
{
	//menubitmap_s	*holdDeck;
	//int		i;
	int length,xTurboStart;
	//int		numColor, roundColor;

	// Draw the basic screen frame

	// Upper corners
	trap_R_SetColor( colorTable[CT_DKPURPLE1]);
	UI_DrawHandlePic( 20,  24,  64,  32, corner_ul_24_60);		// Upper corner
	trap_R_SetColor( colorTable[CT_LTPURPLE1] ); //colorTable[CT_LTBLUE1] //[CT_DKGOLD1] //colorTable[CT_VDKPURPLE2]
	UI_DrawHandlePic( 70, 56, 128, 128, transporter_sym);			// Transporter graphic

	// Lower corners
	trap_R_SetColor( colorTable[CT_DKPURPLE3]); //colorTable[CT_VDKPURPLE2]
	UI_DrawHandlePic( 20, 440,  64,  16, corner_ll_12_60);		// 

	xTurboStart = 604;
	length = UI_ProportionalStringWidth( menu_normal_text[MNT_TRANSPORTER],UI_BIGFONT);
	length += 4;

	// Upper half
	trap_R_SetColor( colorTable[CT_DKPURPLE1]); //DKGOLD1
	UI_DrawHandlePic( 79,  24, xTurboStart - (79 + length),  PROP_BIG_HEIGHT, uis.whiteShader);	// Top left line
	UI_DrawHandlePic( 20,  60,  60,  40, uis.whiteShader);		// 
	trap_R_SetColor( colorTable[CT_DKPURPLE3]);
	UI_DrawHandlePic( 20, 106,  60,  11, uis.whiteShader);		// 
	trap_R_SetColor( colorTable[CT_DKPURPLE1]); //DKGOLD1
	UI_DrawHandlePic( 20, 123,  60, 250, uis.whiteShader);		//  Left hand column

	// Lower half
	trap_R_SetColor( colorTable[CT_DKPURPLE3] ); //colorTable[CT_VDKPURPLE2]
	UI_DrawHandlePic( 20, 380,  60,  70, uis.whiteShader);		// Left Column

	// Bottom line
	trap_R_SetColor( colorTable[CT_DKPURPLE3] ); //colorTable[CT_VDKPURPLE2]
	UI_DrawHandlePic( 69, 443, 287,   12, uis.whiteShader);		// 
	trap_R_SetColor( colorTable[CT_DKPURPLE1]); //colorTable[CT_DKGOLD1]
	UI_DrawHandlePic(364, 443, 260,   12, uis.whiteShader);		// Bottom line


	UI_DrawProportionalString( xTurboStart, 24,
		menu_normal_text[MNT_TRANSPORTER],   UI_BIGFONT | UI_RIGHT, colorTable[CT_WHITE]);		

	trap_R_SetColor( colorTable[CT_DKPURPLE1]); //DKGOLD1
	UI_DrawHandlePic( 607,  24,-16,   32, leftRound);

	trap_R_SetColor( colorTable[CT_LTGOLD1] );
	UI_DrawHandlePic( 86, 240, 74, 46, uis.whiteShader);
	UI_DrawHandlePic( 86, 290, 74, 46, uis.whiteShader);
	UI_DrawHandlePic( 86, 340, 74, 46, uis.whiteShader);
	UI_DrawHandlePic( 86, 390, 74, 46, uis.whiteShader);
	UI_DrawHandlePic( 164, 240, 74, 46, uis.whiteShader);
	UI_DrawHandlePic( 164, 290, 74, 46, uis.whiteShader);
	UI_DrawHandlePic( 164, 340, 74, 46, uis.whiteShader);
	UI_DrawHandlePic( 164, 390, 74, 46, uis.whiteShader);
	UI_DrawHandlePic( 242, 240, 74, 46, uis.whiteShader);
	UI_DrawHandlePic( 242, 290, 76, 46, uis.whiteShader);
	UI_DrawHandlePic( 242, 340, 76, 46, uis.whiteShader);
	UI_DrawHandlePic( 242, 390, 76, 46, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_LTPURPLE1] );
	UI_DrawHandlePic( 84, 192, 2, 243, uis.whiteShader);
	UI_DrawHandlePic( 158, 192, 2, 243, uis.whiteShader);
	UI_DrawHandlePic( 162, 192, 2, 243, uis.whiteShader);
	UI_DrawHandlePic( 236, 192, 2, 243, uis.whiteShader);
	UI_DrawHandlePic( 240, 192, 2, 243, uis.whiteShader);
	UI_DrawHandlePic( 316, 192, -2, 243, uis.whiteShader);
	UI_DrawHandlePic( 84, 192, 74, 2, uis.whiteShader);
	UI_DrawHandlePic( 162, 192, 74, 2, uis.whiteShader);
	UI_DrawHandlePic( 240, 192, 76, 2, uis.whiteShader);
	UI_DrawHandlePic( 84, 435, 76, -2, uis.whiteShader);
	UI_DrawHandlePic( 162, 435, 76, -2, uis.whiteShader);
	UI_DrawHandlePic( 240, 435, 78, -2, uis.whiteShader);

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
TransporterMenu_Draw
===============
*/
static void TransporterMenu_Draw(void)
{
	// Draw graphics particular to Main Menu
	M_TransporterMenu_Graphics();
	
	Menu_Draw( &s_transporter.menu );
}

/*
===============
UI_TransporterMenu_Cache
===============
*/
void UI_TransporterMenu_Cache (void)
{	
	leftRound = trap_R_RegisterShaderNoMip("menu/common/halfroundl_24.tga");
	corner_ul_24_60 = trap_R_RegisterShaderNoMip("menu/common/corner_ul_24_60.tga");
	corner_ll_12_60 = trap_R_RegisterShaderNoMip("menu/common/corner_ll_12_60.tga");
	transporter_sym = trap_R_RegisterShaderNoMip("menu/special/transporter_symbol.tga");
	loading1 = trap_R_RegisterShaderNoMip("menu/new/nav_y.tga");
	loading2 = trap_R_RegisterShaderNoMip("menu/new/nav_mb.tga");
	loading3 = trap_R_RegisterShaderNoMip("menu/new/nav_lb.tga");
	loading4 = trap_R_RegisterShaderNoMip("menu/new/nav_db.tga");
	loading5 = trap_R_RegisterShaderNoMip("menu/new/nab_o.tga");
}

/*
===============
TransporterMenu_Init
===============
*/
void TransporterMenu_Init(void)
{
	int y,pad,x;
	//int i;
	int width;

	AdminGeneric_InitLists();

	s_transporter.menu.nitems				= 0;
	s_transporter.menu.draw					= TransporterMenu_Draw;
	s_transporter.menu.key					= TransporterMenu_Key;
	s_transporter.menu.wrapAround			= qtrue;
	s_transporter.menu.descX				= MENU_DESC_X;
	s_transporter.menu.descY				= MENU_DESC_Y;
	s_transporter.menu.titleX				= MENU_TITLE_X;
	s_transporter.menu.titleY				= MENU_TITLE_Y;

	pad = PROP_BIG_HEIGHT + 10;
	width = MENU_BUTTON_MED_WIDTH-20;
	y = 72;
	x = 208;

	s_transporter.engage.generic.type		= MTYPE_BITMAP;
	s_transporter.engage.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_transporter.engage.generic.x			= x;
	s_transporter.engage.generic.y			= y;
	s_transporter.engage.generic.name		= GRAPHIC_BUTTONLEFT;
	s_transporter.engage.generic.id			= ID_ENGAGE;
	s_transporter.engage.generic.callback	= M_Transporter_Event;
	s_transporter.engage.width				= width;
	s_transporter.engage.height				= PROP_BIG_HEIGHT;
	s_transporter.engage.color				= CT_DKPURPLE1;
	s_transporter.engage.color2				= CT_LTPURPLE1;
	s_transporter.engage.textX				= MENU_BUTTON_TEXT_X;
	s_transporter.engage.textY				= 12;
	s_transporter.engage.textEnum			= MBT_ENGAGE;
	s_transporter.engage.textcolor			= CT_BLACK;
	s_transporter.engage.textcolor2			= CT_WHITE;
	s_transporter.engage.textStyle			= UI_TINYFONT;

	y += pad;

	s_transporter.engage2.generic.type		= MTYPE_BITMAP;
	s_transporter.engage2.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_transporter.engage2.generic.x			= x;
	s_transporter.engage2.generic.y			= y;
	s_transporter.engage2.generic.name		= GRAPHIC_BUTTONLEFT;
	s_transporter.engage2.generic.id		= ID_ENGAGE2;
	s_transporter.engage2.generic.callback	= M_Transporter_Event;
	s_transporter.engage2.width				= width;
	s_transporter.engage2.height			= PROP_BIG_HEIGHT;
	s_transporter.engage2.color				= CT_DKPURPLE1;
	s_transporter.engage2.color2			= CT_LTPURPLE1;
	s_transporter.engage2.textX				= MENU_BUTTON_TEXT_X;
	s_transporter.engage2.textY				= 12;
	s_transporter.engage2.textEnum			= MBT_ENGAGE;
	s_transporter.engage2.textcolor			= CT_BLACK;
	s_transporter.engage2.textcolor2		= CT_WHITE;
	s_transporter.engage2.textStyle			= UI_TINYFONT;

	y += pad;

	s_transporter.quitmenu.generic.type		= MTYPE_BITMAP;
	s_transporter.quitmenu.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
	s_transporter.quitmenu.generic.x		= x;
	s_transporter.quitmenu.generic.y		= y;
	s_transporter.quitmenu.generic.name		= GRAPHIC_BUTTONLEFT;
	s_transporter.quitmenu.generic.id		= ID_QUIT;
	s_transporter.quitmenu.generic.callback	= M_Transporter_Event;
	s_transporter.quitmenu.width			= width;
	s_transporter.quitmenu.height			= PROP_BIG_HEIGHT;
	s_transporter.quitmenu.color			= CT_DKPURPLE1;
	s_transporter.quitmenu.color2			= CT_LTPURPLE1;
	s_transporter.quitmenu.textX			= MENU_BUTTON_TEXT_X;
	s_transporter.quitmenu.textY			= 12;
	s_transporter.quitmenu.textEnum			= MBT_RETURNMENU;
	s_transporter.quitmenu.textcolor		= CT_BLACK;
	s_transporter.quitmenu.textcolor2		= CT_WHITE;
	s_transporter.quitmenu.textStyle		= UI_TINYFONT;

	y -= (2*pad);
	x += width + 8;

	s_transporter.locButton.generic.type	= MTYPE_SPINCONTROL;
	s_transporter.locButton.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS | QMF_ALTERNATE;
	s_transporter.locButton.generic.x		= x;
	s_transporter.locButton.generic.y		= y;
	s_transporter.locButton.width			= width;
	s_transporter.locButton.height			= PROP_BIG_HEIGHT;
	s_transporter.locButton.itemnames		= (const char**)s_adminGeneric.locListPtr;
	s_transporter.locButton.color			= CT_DKPURPLE1;
	s_transporter.locButton.color2			= CT_LTPURPLE1;
	s_transporter.locButton.textX			= MENU_BUTTON_TEXT_X;
	s_transporter.locButton.textY			= 12;
	s_transporter.locButton.textEnum		= MBT_ADMIN_BEAMLOC;
	s_transporter.locButton.textcolor		= CT_BLACK;
	s_transporter.locButton.textcolor2		= CT_WHITE;

	y += pad;

	s_transporter.srvButton.generic.type	= MTYPE_SPINCONTROL;
	s_transporter.srvButton.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS | QMF_ALTERNATE;
	s_transporter.srvButton.generic.x		= x;
	s_transporter.srvButton.generic.y		= y;
	s_transporter.srvButton.width			= width;
	s_transporter.srvButton.height			= PROP_BIG_HEIGHT;
	s_transporter.srvButton.itemnames		= (const char**)s_transporter.srvListPtr;
	s_transporter.srvButton.color			= CT_DKPURPLE1;
	s_transporter.srvButton.color2			= CT_LTPURPLE1;
	s_transporter.srvButton.textX			= MENU_BUTTON_TEXT_X;
	s_transporter.srvButton.textY			= 12;
	s_transporter.srvButton.textEnum		= MBT_TRANS_EXTERN;
	s_transporter.srvButton.textcolor		= CT_BLACK;
	s_transporter.srvButton.textcolor2		= CT_WHITE;

	y += pad;

	s_transporter.delButton.generic.type	= MTYPE_SPINCONTROL;
	s_transporter.delButton.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS | QMF_ALTERNATE;
	s_transporter.delButton.generic.x		= x;
	s_transporter.delButton.generic.y		= y;
	s_transporter.delButton.width			= width;
	s_transporter.delButton.height			= PROP_BIG_HEIGHT;
	s_transporter.delButton.itemnames		= (const char**)delayList;
	s_transporter.delButton.color			= CT_DKPURPLE1;
	s_transporter.delButton.color2			= CT_LTPURPLE1;
	s_transporter.delButton.textX			= MENU_BUTTON_TEXT_X;
	s_transporter.delButton.textY			= 12;
	s_transporter.delButton.textEnum		= MBT_TRANS_DELAY;
	s_transporter.delButton.textcolor		= CT_BLACK;
	s_transporter.delButton.textcolor2		= CT_WHITE;

	Menu_AddItem( &s_transporter.menu, &s_transporter.delButton );
	Menu_AddItem( &s_transporter.menu, &s_transporter.srvButton );
	Menu_AddItem( &s_transporter.menu, &s_transporter.locButton );
	Menu_AddItem( &s_transporter.menu, &s_transporter.engage );
	Menu_AddItem( &s_transporter.menu, &s_transporter.engage2 );
	Menu_AddItem( &s_transporter.menu, &s_transporter.quitmenu );
}

/*
=================
SrvData_Init
=================
*/
void SrvData_Init(void) {
	int		i;

	for(i = 0; i < 6; i++) {
		if(!srvList[i][0]) break;
		s_transporter.srvListPtr[i] = srvList[i];
	}
	s_transporter.srvListPtr[i+1] = 0;
}

/*
===============
UI_TransporterMenu
===============
*/
void UI_TransporterMenu (int trNum)
{
	if ( !trNum )
		return;

	memset( &s_transporter, 0, sizeof( s_transporter ) );

	s_transporter.targetEntID = trNum;

	uis.menusp = 0;

	ingameFlag = qtrue;	// true when in game menu is in use

	Mouse_Show();

	UI_TransporterMenu_Cache();

	SrvData_Init();

	TransporterMenu_Init(); 

	UI_PushMenu( &s_transporter.menu );

	Menu_AdjustCursor( &s_transporter.menu, 1 );	
}
