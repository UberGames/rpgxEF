/**********************************************************************
	UI_HOLODECK.C

	User interface trigger from within game
**********************************************************************/
#include "ui_local.h"

typedef struct {
	char	name[5][MAX_QPATH];
	char	desc[5][1024];
	char	image[5][MAX_QPATH];
	int		active;
	int		numProgs;
	int		currentProg;
	int		currentPart;
} holoData_t;

holoData_t holoData;

typedef struct //static
{
	menuframework_s menu;
	sfxHandle_t		openingVoice;
	menubitmap_s	quitmenu;
	menubitmap_s	engage;
	menulist_s		progButton;

	char			*prgListPtr[6];

	int				targetEntID;
} holodeck_t;

holodeck_t	s_holodeck;

void HoloDataReceived(const char *data) {
	char *temp;
	if(holoData.currentPart < 0) return;
	if(!holoData.numProgs) {
		holoData.numProgs = atoi(data);
		holoData.currentProg = 0;
		holoData.currentPart = 0;
		return;
	}
	switch(holoData.currentPart) {
		case 0: // name
			temp = Info_ValueForKey(data, va("n%i", holoData.currentProg));
			Q_strncpyz(holoData.name[holoData.currentProg], temp, sizeof(holoData.name[holoData.currentProg]));
			break;
		case 1: // desc1
			temp = Info_ValueForKey(data, va("d1%i", holoData.currentProg));
			Q_strcat(holoData.desc[holoData.currentProg], sizeof(holoData.desc[holoData.currentProg]), temp);
			break;
		case 2:
			temp = Info_ValueForKey(data, va("d2%i", holoData.currentProg));
			Q_strcat(holoData.desc[holoData.currentProg], sizeof(holoData.desc[holoData.currentProg]), temp);
			break;
		case 3:
			temp = Info_ValueForKey(data, va("i%i", holoData.currentProg));
			Q_strncpyz(holoData.image[holoData.currentProg], temp, sizeof(holoData.image[holoData.currentProg]));
			break;
	}
	holoData.currentProg++;
	if(holoData.currentProg == holoData.numProgs) {
		holoData.currentProg = 0;
		holoData.currentPart++;
	}
}

/*
===============
PrgmList_Init
===============
*/
void PrgmList_Init(void) {
	int i;

	for(i = 0; i < holoData.numProgs; i++) {
		s_holodeck.prgListPtr[i] = holoData.name[i];
	}
	s_holodeck.prgListPtr[holoData.numProgs] = 0;
}

void HolodeckMenu_LoadText (void);

#define ID_COMPUTERVOICE	6

#define ID_QUIT			10
#define ID_ENGAGE		100
#define ID_PRGM_BUTTON	101

void UI_HolodeckMenu_Cache (void);
void UI_HolodeckMenu(int trNum);

/*
=================
M_Holodeck_Event
=================
*/
static void M_Holodeck_Event (void* ptr, int notification)
{
	int	id;
	int i;

	id = ((menucommon_s*)ptr)->id;

	switch (id)
	{
		case ID_QUIT:
			if ( notification == QM_ACTIVATED )
				UI_PopMenu();
			break;
		case ID_ENGAGE:
			if ( notification == QM_ACTIVATED ) {
				// do somthing
				UI_ForceMenuOff();
			}
			break;
		case ID_PRGM_BUTTON:
			i = s_holodeck.targetEntID;
			UI_PopMenu();
			UI_HolodeckMenu(i);
			break;
	}
}

/*
=================
HolodeckMenu_Key
=================
*/
sfxHandle_t HolodeckMenu_Key (int key)
{
	return ( Menu_DefaultKey( &s_holodeck.menu, key ) );
}

extern qhandle_t			leftRound;
extern qhandle_t			corner_ul_24_60;
extern qhandle_t			corner_ll_12_60;
qhandle_t					prgImage1;
qhandle_t					prgImage2;
qhandle_t					prgImage3;
qhandle_t					prgImage4;
qhandle_t					prgImage5;

/*
=================
M_HolodeckMenu_Graphics
=================
*/
static void M_HolodeckMenu_Graphics (void)
{
	int		i,length,xTurboStart;
	int		numColor,roundColor;

	// Draw the basic screen frame

	// Upper corners
	trap_R_SetColor( colorTable[CT_DKPURPLE1]);
	UI_DrawHandlePic( 20,  24,  64,  32, corner_ul_24_60);		// Upper corner
	trap_R_SetColor( colorTable[CT_LTPURPLE1] ); //colorTable[CT_LTBLUE1] //[CT_DKGOLD1] //colorTable[CT_VDKPURPLE2]
	//UI_DrawHandlePic( 70, 56, 128, 128, transporter_sym);			// Transporter graphic

	// Lower corners
	trap_R_SetColor( colorTable[CT_DKPURPLE3]); //colorTable[CT_VDKPURPLE2]
	UI_DrawHandlePic( 20, 440,  64,  16, corner_ll_12_60);		// 

	xTurboStart = 604;
	length = UI_ProportionalStringWidth( menu_normal_text[MNT_HOLODECK],UI_BIGFONT);
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
		menu_normal_text[MNT_HOLODECK],   UI_BIGFONT | UI_RIGHT, colorTable[CT_WHITE]);		

	trap_R_SetColor( colorTable[CT_DKPURPLE1]); //DKGOLD1
	UI_DrawHandlePic( 607,  24,-16,   32, leftRound);

	//UI_DrawHandlePic( 204, 106, 110, 24, buttonRight);

	trap_R_SetColor( colorTable[CT_WHITE] );
	switch(s_holodeck.progButton.curvalue) {
		case 0:
			UI_DrawHandlePic(346, 89, 257, 192, prgImage1);
			break;
		case 1:
			UI_DrawHandlePic(346, 89, 257, 192, prgImage2);
			break;
		case 2:
			UI_DrawHandlePic(346, 89, 257, 192, prgImage3);
			break;
		case 3:
			UI_DrawHandlePic(346, 89, 257, 192, prgImage4);
			break;
		case 4:
			UI_DrawHandlePic(346, 89, 257, 192, prgImage5);
			break;
	}

	trap_R_SetColor( colorTable[CT_DKPURPLE1]);

	UI_DrawHandlePic(322, 72, 24, 15, corner_ul_24_60);
	UI_DrawHandlePic(346, 72, 259, 11, uis.whiteShader);
	UI_DrawHandlePic(607, 72, -8, 14, leftRound);
	UI_DrawHandlePic(322, 89, 22.5, 192, uis.whiteShader);
	UI_DrawHandlePic(322, 283, 24, -15, corner_ul_24_60);
	UI_DrawHandlePic(346, 287, 243, 11, uis.whiteShader);
	UI_DrawHandlePic(591, 287, -24, 15, corner_ul_24_60);
	UI_DrawHandlePic(592.5, 301, 22.5, 137, uis.whiteShader);

	UI_DrawProportionalString(346, 289, va("%s", holoData.desc[0]), UI_TINYFONT, colorTable[CT_WHITE]);

}

/*
===============
HolodeckMenu_Draw
===============
*/
static void HolodeckMenu_Draw(void)
{
	// Draw graphics particular to Main Menu
	M_HolodeckMenu_Graphics();
	
	Menu_Draw( &s_holodeck.menu );
}

/*
===============
UI_HolodeckMenu_Cache
===============
*/
void UI_HolodeckMenu_Cache (void)
{	
	leftRound = trap_R_RegisterShaderNoMip("menu/common/halfroundl_24.tga");
	corner_ul_24_60 = trap_R_RegisterShaderNoMip("menu/common/corner_ul_24_60.tga");
	corner_ll_12_60 = trap_R_RegisterShaderNoMip("menu/common/corner_ll_12_60.tga");
	if(holoData.image[0][0])
		prgImage1 = trap_R_RegisterShaderNoMip(holoData.image[0]);
	if(holoData.image[1][0])
		prgImage2 = trap_R_RegisterShaderNoMip(holoData.image[1]);
	if(holoData.image[2][0])
		prgImage3 = trap_R_RegisterShaderNoMip(holoData.image[2]);
	if(holoData.image[3][0])
		prgImage4 = trap_R_RegisterShaderNoMip(holoData.image[3]);
	if(holoData.image[4][0])
		prgImage5 = trap_R_RegisterShaderNoMip(holoData.image[4]);
}

/*
===============
HolodeckMenu_Init
===============
*/
void HolodeckMenu_Init()
{
	int y,pad,x;
	int i,width;

	PrgmList_Init();

	s_holodeck.menu.nitems				= 0;
	s_holodeck.menu.draw				= HolodeckMenu_Draw;
	s_holodeck.menu.key					= HolodeckMenu_Key;
	s_holodeck.menu.wrapAround			= qtrue;
	s_holodeck.menu.descX				= MENU_DESC_X;
	s_holodeck.menu.descY				= MENU_DESC_Y;
	s_holodeck.menu.titleX				= MENU_TITLE_X;
	s_holodeck.menu.titleY				= MENU_TITLE_Y;

	pad = PROP_BIG_HEIGHT + 10;
	width = MENU_BUTTON_MED_WIDTH-20;
	y = 72;
	x = 86;

	s_holodeck.engage.generic.type		= MTYPE_BITMAP;
	s_holodeck.engage.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_holodeck.engage.generic.x			= x;
	s_holodeck.engage.generic.y			= y;
	s_holodeck.engage.generic.name		= GRAPHIC_BUTTONLEFT;
	s_holodeck.engage.generic.id		= ID_ENGAGE;
	s_holodeck.engage.generic.callback	= M_Holodeck_Event;
	s_holodeck.engage.width				= width;
	s_holodeck.engage.height			= PROP_BIG_HEIGHT;
	s_holodeck.engage.color				= CT_DKPURPLE1;
	s_holodeck.engage.color2			= CT_LTPURPLE1;
	s_holodeck.engage.textX				= MENU_BUTTON_TEXT_X;
	s_holodeck.engage.textY				= 12;
	s_holodeck.engage.textEnum			= MBT_HOLODECK_START;
	s_holodeck.engage.textcolor			= CT_BLACK;
	s_holodeck.engage.textcolor2		= CT_WHITE;
	s_holodeck.engage.textStyle			= UI_TINYFONT;

	x += width + 8;

	s_holodeck.quitmenu.generic.type	= MTYPE_BITMAP;
	s_holodeck.quitmenu.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
	s_holodeck.quitmenu.generic.x		= x;
	s_holodeck.quitmenu.generic.y		= y;
	s_holodeck.quitmenu.generic.name	= GRAPHIC_BUTTONRIGHT;
	s_holodeck.quitmenu.generic.id		= ID_QUIT;
	s_holodeck.quitmenu.generic.callback= M_Holodeck_Event;
	s_holodeck.quitmenu.width			= width;
	s_holodeck.quitmenu.height			= PROP_BIG_HEIGHT;
	s_holodeck.quitmenu.color			= CT_DKPURPLE1;
	s_holodeck.quitmenu.color2			= CT_LTPURPLE1;
	s_holodeck.quitmenu.textX			= MENU_BUTTON_TEXT_X;
	s_holodeck.quitmenu.textY			= 12;
	s_holodeck.quitmenu.textEnum		= MBT_RETURNMENU;
	s_holodeck.quitmenu.textcolor		= CT_BLACK;
	s_holodeck.quitmenu.textcolor2		= CT_WHITE;
	s_holodeck.quitmenu.textStyle		= UI_TINYFONT;

	x -= width + 8;
	y += pad;

	s_holodeck.progButton.generic.type	= MTYPE_SPINCONTROL;
	s_holodeck.progButton.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS | QMF_ALTERNATE2;
	s_holodeck.progButton.generic.x		= x;
	s_holodeck.progButton.generic.y		= y;
	s_holodeck.progButton.generic.id	= ID_PRGM_BUTTON;
	s_holodeck.progButton.width			= width;
	s_holodeck.progButton.height		= PROP_BIG_HEIGHT;
	s_holodeck.progButton.itemnames		= (const char**)s_holodeck.prgListPtr;
	s_holodeck.progButton.color			= CT_DKPURPLE1;
	s_holodeck.progButton.color2		= CT_LTPURPLE1;
	s_holodeck.progButton.textX			= MENU_BUTTON_TEXT_X;
	s_holodeck.progButton.textY			= 12;
	s_holodeck.progButton.textEnum		= MBT_HOLODECK_LIST;
	s_holodeck.progButton.textcolor		= CT_BLACK;
	s_holodeck.progButton.textcolor2	= CT_WHITE;

	Menu_AddItem( &s_holodeck.menu, &s_holodeck.progButton );
	Menu_AddItem( &s_holodeck.menu, &s_holodeck.engage );
	Menu_AddItem( &s_holodeck.menu, &s_holodeck.quitmenu );
}

/*
===============
UI_HolodeckMenu
===============
*/
void UI_HolodeckMenu (int trNum)
{
	if ( !trNum )
		return;

	memset( &s_holodeck, 0, sizeof( s_holodeck ) );

	s_holodeck.targetEntID = trNum;

	uis.menusp = 0;

	ingameFlag = qtrue;	// true when in game menu is in use

	Mouse_Show();

	UI_HolodeckMenu_Cache();

	HolodeckMenu_Init(); 

	UI_PushMenu( &s_holodeck.menu );

	Menu_AdjustCursor( &s_holodeck.menu, 1 );	
}

