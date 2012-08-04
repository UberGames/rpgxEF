/**********************************************************************
	UI_TURBOLIFT.C

	User interface trigger from within game
**********************************************************************/
#include "ui_local.h"

typedef struct
{
	int		deckNum;
	char	deckDesc[MAX_QPATH];
} deckData_t;

// Data for Closing Credits Menu
typedef struct //static
{
	menuframework_s menu;
	int				maxDecks;
	int				chosenDeck;
	int				highLightedDeck;
	sfxHandle_t		openingVoice;
	menubitmap_s	quitmenu;
	menubitmap_s	engage;
	menubitmap_s	deck1;
	menubitmap_s	deck2;
	menubitmap_s	deck3;
	menubitmap_s	deck4;
	menubitmap_s	deck5;
	menubitmap_s	deck6;
	menubitmap_s	deck7;
	menubitmap_s	deck8;
	menubitmap_s	deck9;
	menubitmap_s	deck10;
	menubitmap_s	deck11;
	menubitmap_s	deck12;
	menubitmap_s	deck13;
	menubitmap_s	deck14;
	menubitmap_s	deck15;
	menubitmap_s	deck16;

	deckData_t		deckData[MAX_DECKS];
	int				numDecks;

	int				liftNum;
} turbolift_t;

turbolift_t	s_turbolift;

void TurboliftMenu_LoadText (void);

#define ID_ARROW1UP			2
#define ID_ARROW1DOWN		3
#define ID_ARROW2UP			4
#define ID_ARROW2DOWN		5
#define ID_COMPUTERVOICE	6

#define ID_QUIT		10
#define ID_DECK1	11
#define ID_DECK2	12
#define ID_DECK3	13
#define ID_DECK4	14
#define ID_DECK5	15
#define ID_DECK6	16
#define ID_DECK7	17
#define ID_DECK8	18
#define ID_DECK9	19
#define ID_DECK10	20
#define ID_DECK11	21
#define ID_DECK12	22
#define ID_DECK13	23
#define ID_DECK14	24
#define ID_DECK15	25
#define ID_DECK16	26
#define ID_ENGAGE	100

void UI_HolodeckMenu_Cache (void);

/*
=================
M_Turbolift_Event
=================
*/
static void M_Turbolift_Event (void* ptr, int notification)
{
	int	id;
	menubitmap_s	*holdDeck;

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
		case ID_DECK1:
		case ID_DECK2:
		case ID_DECK3:
		case ID_DECK4:
		case ID_DECK5:
		case ID_DECK6:
		case ID_DECK7:
		case ID_DECK8:
		case ID_DECK9:
		case ID_DECK10:
		case ID_DECK11:
		case ID_DECK12:
		case ID_DECK13:
		case ID_DECK14:
		case ID_DECK15:
		case ID_DECK16:

			if ( notification == QM_ACTIVATED )
			{
				if (s_turbolift.chosenDeck >= 0)
				{
					holdDeck = &s_turbolift.deck1;
					holdDeck += s_turbolift.chosenDeck;
					holdDeck->textcolor	= CT_BLACK;
				}

				s_turbolift.chosenDeck	= id - ID_DECK1;
				s_turbolift.engage.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;

				holdDeck = &s_turbolift.deck1;
				holdDeck += s_turbolift.chosenDeck;
				//holdDeck->textcolor	= CT_WHITE;//CT_LTGOLD1;
			}
			else if ( notification == QM_GOTFOCUS )
			{
				s_turbolift.highLightedDeck = id - ID_DECK1;
			}
			break;
		case ID_ENGAGE:		// Active only if a deck has been chosen
			if ( notification == QM_ACTIVATED ) {
				UI_ForceMenuOff();
				trap_Cmd_ExecuteText( EXEC_APPEND, va( "deck %i %i", s_turbolift.liftNum, s_turbolift.deckData[s_turbolift.chosenDeck].deckNum ) );
			}
			break;
	}
}

/*
=================
TurboliftMenu_Key
=================
*/
sfxHandle_t TurboliftMenu_Key (int key)
{
	return ( Menu_DefaultKey( &s_turbolift.menu, key ) );
}

qhandle_t			leftRound;
qhandle_t			corner_ul_24_60;
qhandle_t			corner_ll_12_60;
qhandle_t			turbolift;

/*
=================
M_TurboliftMenu_Graphics
=================
*/
static void M_TurboliftMenu_Graphics (void)
{
	menubitmap_s	*holdDeck;
	int		i,length,xTurboStart;
	int		numColor,roundColor;

	// Draw the basic screen frame

	// Upper corners
	trap_R_SetColor( colorTable[CT_DKPURPLE1]);
	UI_DrawHandlePic( 20,  24,  64,  32, corner_ul_24_60);		// Upper corner
	UI_DrawHandlePic( 20, 353,  64,  16, corner_ll_12_60);		// Lower Corner
	trap_R_SetColor( colorTable[CT_LTPURPLE1] ); //colorTable[CT_LTBLUE1] //[CT_DKGOLD1] //colorTable[CT_VDKPURPLE2]
	UI_DrawHandlePic( 100, 86, 128, 128, turbolift);			// Turbolift graphic

	// Lower corners
	trap_R_SetColor( colorTable[CT_DKPURPLE3]); //colorTable[CT_VDKPURPLE2]
	UI_DrawHandlePic( 20, 375,  64, -16, corner_ll_12_60);		// 
	UI_DrawHandlePic( 20, 440,  64,  16, corner_ll_12_60);		// 

	xTurboStart = 604;
	length = UI_ProportionalStringWidth( menu_normal_text[MNT_TURBOLIFT],UI_BIGFONT);
	length += 4;

	// Upper half
	trap_R_SetColor( colorTable[CT_DKPURPLE1]); //DKGOLD1
	UI_DrawHandlePic( 79,  24, xTurboStart - (79 + length),  PROP_BIG_HEIGHT, uis.whiteShader);	// Top left line
	UI_DrawHandlePic( 20,  60,  60,  40, uis.whiteShader);		// 
	trap_R_SetColor( colorTable[CT_DKPURPLE3]);
	UI_DrawHandlePic( 20, 106,  60,  11, uis.whiteShader);		// 
	trap_R_SetColor( colorTable[CT_DKPURPLE1]); //DKGOLD1
	UI_DrawHandlePic( 20, 123,  60, 240, uis.whiteShader);		//  Left hand column
	UI_DrawHandlePic( 69, 356, 245,  12, uis.whiteShader);		// 

	trap_R_SetColor( colorTable[CT_DKPURPLE3] ); //colorTable[CT_DKPURPLE1]
	UI_DrawHandlePic(319, 356, 93,   5, uis.whiteShader);		// 
	UI_DrawHandlePic(319, 360, 31,   8, uis.whiteShader);		// 
	UI_DrawHandlePic(381, 360, 31,   8, uis.whiteShader);		// 

	trap_R_SetColor( colorTable[CT_DKPURPLE1]);
	UI_DrawHandlePic(417, 356, 93,   12, uis.whiteShader);		// 

	UI_DrawHandlePic(510, 356,114,   12, uis.whiteShader);		// 

	// Lower half
	trap_R_SetColor( colorTable[CT_DKPURPLE3] ); //colorTable[CT_VDKPURPLE2]
	UI_DrawHandlePic( 20, 380,  60,  70, uis.whiteShader);		// Left Column

	trap_R_SetColor( colorTable[CT_DKPURPLE3] ); //colorTable[CT_VDKPURPLE2]
	UI_DrawHandlePic( 69, 376, 158,  12, uis.whiteShader);		// Top line
	UI_DrawHandlePic(232, 376,  82,  12, uis.whiteShader);		// Top line
	UI_DrawHandlePic(417, 376,  12,  12, uis.whiteShader);		// 
	UI_DrawHandlePic(434, 376, 190,  12, uis.whiteShader);		// 

	// Funky indent
	trap_R_SetColor( colorTable[CT_DKPURPLE1] ); //colorTable[CT_DKGOLD1]
	UI_DrawHandlePic(319, 383,  93,   5, uis.whiteShader);		// 
	UI_DrawHandlePic(319, 376,  31,   8, uis.whiteShader);		// 
	UI_DrawHandlePic(381, 376,  31,   8, uis.whiteShader);		// 

	// Bottom line
	trap_R_SetColor( colorTable[CT_DKPURPLE3] ); //colorTable[CT_VDKPURPLE2]
	UI_DrawHandlePic( 69, 443, 287,   12, uis.whiteShader);		// 
	trap_R_SetColor( colorTable[CT_DKPURPLE1]); //colorTable[CT_DKGOLD1]
	UI_DrawHandlePic(364, 443, 260,   12, uis.whiteShader);		// Bottom line


	UI_DrawProportionalString( xTurboStart, 24,
		menu_normal_text[MNT_TURBOLIFT],   UI_BIGFONT | UI_RIGHT, colorTable[CT_WHITE]);		

	trap_R_SetColor( colorTable[CT_DKPURPLE1]); //DKGOLD1
	UI_DrawHandlePic( 607,  24,-16,   32, leftRound);
		
	holdDeck = &s_turbolift.deck1;

	// Print deck buttons
	for (i=0;i<s_turbolift.maxDecks;i++)
	{
		if (s_turbolift.deckData[i].deckNum )
		{
			if (s_turbolift.chosenDeck == i)		// This deck was chosen
			{
				numColor = CT_WHITE;//CT_LTGOLD1;
				roundColor = CT_LTPURPLE1;//CT_LTGOLD1;
			}
			else								// This deck was not chosen
			{
				numColor = CT_WHITE;
				roundColor = CT_DKPURPLE1;//CT_DKGOLD1;
			}

			UI_DrawProportionalString( holdDeck->generic.x - 6,
				holdDeck->generic.y, 
				va("%d",s_turbolift.deckData[i].deckNum),   UI_BIGFONT|UI_RIGHT, colorTable[numColor]);		//i+1

			trap_R_SetColor( colorTable[roundColor]);
			UI_DrawHandlePic( holdDeck->generic.x - 45, 
				holdDeck->generic.y, 
				16,   32, leftRound);
		}

		holdDeck++;
	}

	
	if ( s_turbolift.highLightedDeck >= 0 )
	{
		UI_DrawProportionalString( 353, 409, 
				s_turbolift.deckData[s_turbolift.highLightedDeck].deckDesc, UI_SMALLFONT|UI_CENTER, colorTable[CT_WHITE]);	
	}
	else
	{
		UI_DrawProportionalString( 353, 409, menu_normal_text[MNT_SPECFICYDECK], UI_SMALLFONT|UI_CENTER, colorTable[CT_WHITE]);
	}

	// Round graphic on left of engage & quit button
	trap_R_SetColor( colorTable[s_turbolift.quitmenu.color]);
	UI_DrawHandlePic(s_turbolift.engage.generic.x - 14,
		s_turbolift.engage.generic.y, 
		MENU_BUTTON_MED_HEIGHT, s_turbolift.engage.height, uis.graphicButtonLeftEnd);

	UI_DrawHandlePic(s_turbolift.quitmenu.generic.x - 14,
		s_turbolift.quitmenu.generic.y, 
		MENU_BUTTON_MED_HEIGHT, s_turbolift.quitmenu.height, uis.graphicButtonLeftEnd);

}

/*
===============
TurboliftMenu_Draw
===============
*/
static void TurboliftMenu_Draw(void)
{
	// Draw graphics particular to Main Menu
	M_TurboliftMenu_Graphics();
	
	Menu_Draw( &s_turbolift.menu );
}

/*
===============
UI_TurboliftMenu_Cache
===============
*/
void UI_TurboliftMenu_Cache (void)
{	
	leftRound = trap_R_RegisterShaderNoMip("menu/common/halfroundl_24.tga");
	corner_ul_24_60 = trap_R_RegisterShaderNoMip("menu/common/corner_ul_24_60.tga");
	corner_ll_12_60 = trap_R_RegisterShaderNoMip("menu/common/corner_ll_12_60.tga");
	turbolift = trap_R_RegisterShaderNoMip("menu/common/lift_button.tga");
}


//Sorts decks in ascending order
static int QDECL SortDecks( const void *arg1, const void *arg2 ) 
{
	int	deck1;
	int	deck2;

	deck1 = ( (deckData_t *)arg1)->deckNum;
	deck2 = ( (deckData_t *)arg2)->deckNum;

	if ( deck1 < 0 )
		deck1 = 0;

	if ( deck2 < 0 )
		deck2 = 0;

	if ( deck1 > deck2 )
		return 1;

	if ( deck1 == deck2 )
		return 0;
	
	return -1;
}

static void UI_TurboliftMenu_LoadDecks( void )
{
	char	buffer[MAX_TOKEN_CHARS];
	int		i;
	char	*temp;

	s_turbolift.numDecks = 0;

	//load the string
	trap_GetConfigString( CS_TURBOLIFT_DATA, buffer, sizeof( buffer ) );

	if ( !buffer[0] )
		return;

	memset( &s_turbolift.deckData, 0, sizeof( s_turbolift.deckData ) );

	for ( i=0; i < MAX_DECKS; i++ )
	{
		temp = Info_ValueForKey( buffer, va( "d%i", i ) );

		if ( !temp[0] )
			break;

		s_turbolift.deckData[ s_turbolift.numDecks ].deckNum = atoi( temp );

		temp = Info_ValueForKey( buffer, va( "n%i", i ) );

		Q_strncpyz( s_turbolift.deckData[ s_turbolift.numDecks ].deckDesc, temp, sizeof( s_turbolift.deckData[ s_turbolift.numDecks ].deckDesc ) );
		
		s_turbolift.numDecks++;
	}

	//TiM - sort the decks into their sequential order
	qsort( s_turbolift.deckData, s_turbolift.numDecks, sizeof( deckData_t ), SortDecks );
}

static void UI_ManageDeckLoading( void )
{
	char			fileRoute[MAX_QPATH];
	char			mapRoute[MAX_QPATH];
	char			info[MAX_TOKEN_CHARS];
	fileHandle_t	f;
	int				file_len;
	char			*textPtr;
	char			buffer[20000];
	char			*token;

	//get the map name
	trap_GetConfigString( CS_SERVERINFO, info, sizeof( info ) );
	Com_sprintf( mapRoute, sizeof( fileRoute ), "maps/%s", Info_ValueForKey( info, "mapname" ) );

	//check for language
	UI_LanguageFilename( mapRoute, "turbolift", fileRoute );

	file_len = trap_FS_FOpenFile( fileRoute, &f, FS_READ );

	if ( file_len <= 1 )
	{
		//Com_Printf( S_COLOR_YELLOW "WARNING: Attempted to load %s, but wasn't found.\n", fileRoute );
		UI_TurboliftMenu_LoadDecks();
		return;
	}

	trap_FS_Read( buffer, file_len, f );
	trap_FS_FCloseFile( f );

	if ( !buffer[0] )
	{
		Com_Printf( S_COLOR_RED "ERROR: Attempted to load %s, but no data was read.\n", fileRoute );
		UI_TurboliftMenu_LoadDecks();
		return;
	}

	s_turbolift.numDecks = 0;
	memset( &s_turbolift.deckData, 0, sizeof( s_turbolift.deckData ) );
	buffer[file_len] = '\0';

	COM_BeginParseSession();
	textPtr = buffer;

	//Com_Printf( S_COLOR_RED "Beginning Parse\n" );

	//expected format is 'decknum' <space> 'deck Desc'
	while( 1 )
	{
		token = COM_Parse( &textPtr );
		if ( !token[0] )
			break;

		//Com_Printf( S_COLOR_RED "First Token: %s\n", token );

		//in case of Scooter's EF SP style DAT files, which require 'DECK' in front of the number
		if ( !Q_strncmp( token, "DECK", 4 ) )
			token += 4;

		//grab the deck number
		s_turbolift.deckData[s_turbolift.numDecks].deckNum = atoi( token );

		token = COM_ParseExt( &textPtr, qfalse );
		if (!token[0])
			continue;

		//Com_Printf( S_COLOR_RED "Second Token: %s\n", token );

		Q_strncpyz( s_turbolift.deckData[s_turbolift.numDecks].deckDesc, 
					token,
					sizeof( s_turbolift.deckData[s_turbolift.numDecks].deckDesc ) );

		s_turbolift.numDecks++;

		//if this is an EF SP style script, there may be more data after these two. ignore them
		if ( COM_ParseExt( &textPtr, qfalse ) == NULL )
			SkipRestOfLine( &textPtr );
	}

	qsort( s_turbolift.deckData, s_turbolift.numDecks, sizeof( deckData_t ), SortDecks );
}

/*
===============
TurboliftMenu_Init
===============
*/
void TurboliftMenu_Init(void)
{
	int	y,pad,x;
	menubitmap_s	*holdDeck;
	int		i,width;

	UI_ManageDeckLoading();

	s_turbolift.menu.nitems						= 0;
	s_turbolift.menu.draw						= TurboliftMenu_Draw;
	s_turbolift.menu.key						= TurboliftMenu_Key;
	s_turbolift.menu.wrapAround					= qtrue;
	s_turbolift.menu.descX						= MENU_DESC_X;
	s_turbolift.menu.descY						= MENU_DESC_Y;
	s_turbolift.menu.titleX						= MENU_TITLE_X;
	s_turbolift.menu.titleY						= MENU_TITLE_Y;

	s_turbolift.chosenDeck = -1;
	s_turbolift.highLightedDeck = -1;

	pad = PROP_BIG_HEIGHT + 10;
	y =  72;
	x = 319;
	width = MENU_BUTTON_MED_WIDTH-20;

	s_turbolift.maxDecks = MAX_DECKS;
	holdDeck = &s_turbolift.deck1;

	for (i=0;i<s_turbolift.maxDecks;i++)
	{
		holdDeck->generic.type				= MTYPE_BITMAP;      
		holdDeck->generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
		holdDeck->generic.x					= x;
		holdDeck->generic.y					= y;
		holdDeck->generic.name				= GRAPHIC_BUTTONRIGHT;
		holdDeck->generic.id				= ID_DECK1 + i;
		holdDeck->generic.callback			= M_Turbolift_Event; 
		holdDeck->width						= width;
		holdDeck->height					= PROP_BIG_HEIGHT;
		holdDeck->color						= CT_DKPURPLE1;//CT_VDKRED1;//CT_DKGOLD1;
		holdDeck->color2					= CT_LTPURPLE1;//CT_DKRED1;//CT_LTGOLD1;
		holdDeck->textX						= MENU_BUTTON_TEXT_X;
		holdDeck->textY						= 12;
		holdDeck->textEnum					= MBT_DECK;
		holdDeck->textcolor					= CT_BLACK;
		holdDeck->textcolor2				= CT_WHITE;
		holdDeck->textStyle					= UI_TINYFONT;

		holdDeck++;
		y += pad;

		// Start the next column
		if (i == ((s_turbolift.maxDecks-1)/2))
		{
			x += width + 90; 
			y = 80;
		}
	}

	s_turbolift.engage.generic.type				= MTYPE_BITMAP;      
	s_turbolift.engage.generic.flags			= QMF_GRAYED;
	s_turbolift.engage.generic.x				= 110;
	s_turbolift.engage.generic.y				= 72 + (pad * 5);
	s_turbolift.engage.generic.name				= GRAPHIC_BUTTONRIGHT;
	s_turbolift.engage.generic.id				= ID_ENGAGE;
	s_turbolift.engage.generic.callback			= M_Turbolift_Event; 
	s_turbolift.engage.width					= width;
	s_turbolift.engage.height					= PROP_BIG_HEIGHT;
	s_turbolift.engage.color					= CT_DKORANGE;
	s_turbolift.engage.color2					= CT_LTORANGE;
	s_turbolift.engage.textX					= MENU_BUTTON_TEXT_X;
	s_turbolift.engage.textY					= 6;
	s_turbolift.engage.textEnum					= MBT_ENGAGE;
	s_turbolift.engage.textcolor				= CT_BLACK;
	s_turbolift.engage.textcolor2				= CT_WHITE;
	//s_turbolift.engage.generic.statusbarfunc	= Turbolift_StatusBar;

	s_turbolift.quitmenu.generic.type			= MTYPE_BITMAP;      
	s_turbolift.quitmenu.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_turbolift.quitmenu.generic.x				= 110;
	s_turbolift.quitmenu.generic.y				= 72 + (pad * 7);
	s_turbolift.quitmenu.generic.name			= GRAPHIC_BUTTONRIGHT;
	s_turbolift.quitmenu.generic.id				= ID_QUIT;
	s_turbolift.quitmenu.generic.callback		= M_Turbolift_Event; 
	s_turbolift.quitmenu.width					= width;
	s_turbolift.quitmenu.height					= PROP_BIG_HEIGHT;
	s_turbolift.quitmenu.color					= CT_DKORANGE;
	s_turbolift.quitmenu.color2					= CT_LTORANGE;
	s_turbolift.quitmenu.textX					= MENU_BUTTON_TEXT_X;
	s_turbolift.quitmenu.textY					= 6;
	s_turbolift.quitmenu.textEnum				= MBT_RETURNMENU;//MBT_RETURN;
	s_turbolift.quitmenu.textcolor				= CT_BLACK;
	s_turbolift.quitmenu.textcolor2				= CT_WHITE;
	//s_turbolift.quitmenu.generic.statusbarfunc	= Turbolift_StatusBar;

	Menu_AddItem( &s_turbolift.menu,	&s_turbolift.engage );
	Menu_AddItem( &s_turbolift.menu,	&s_turbolift.quitmenu );

	holdDeck = &s_turbolift.deck1;
	for (i=0;i<s_turbolift.maxDecks;i++)
	{
		if (s_turbolift.deckData[i].deckDesc[0])
		{
			Menu_AddItem( &s_turbolift.menu,	holdDeck );
		}
		holdDeck++;
	}
}

/*
===============
UI_TurboliftMenu
===============
*/
void UI_TurboliftMenu ( int liftNum )
{
	if ( !liftNum )
		return;

	memset( &s_turbolift, 0, sizeof( s_turbolift ) );

	s_turbolift.liftNum = liftNum;

	uis.menusp = 0;

	ingameFlag = qtrue;	// true when in game menu is in use

	Mouse_Show();

	UI_TurboliftMenu_Cache();

	TurboliftMenu_Init(); 

	UI_PushMenu( &s_turbolift.menu );

	Menu_AdjustCursor( &s_turbolift.menu, 1 );	
}

