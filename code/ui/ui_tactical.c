/**********************************************************************
	UI_TACTICAL.C

	User interface trigger from within game
**********************************************************************/
#include "ui_local.h"

typedef struct {
	char	weaponName[MAX_QPATH];
	int		weaponMaxCount;
	int		weaponCurrentCount;
	char	weaponTarget[MAX_QPATH];
	int		weaponNum;
} weaponData_t;

typedef struct {
	menuframework_s menu;
	int				maxWeapons;
	int				choosenWeapon;
	int				highLightedWeapon;
	sfxHandle_t		openingVoice;
	sfxHandle_t		fireVoice;
	menubitmap_s	quitmenu;
	menubitmap_s	fire;
	menubitmap_s	weap1;
	menubitmap_s	weap2;
	menubitmap_s	weap3;
	menubitmap_s	weap4;
	menubitmap_s	weap5;
	menubitmap_s	weap6;

	weaponData_t	weaponData[MAX_TACTICAL_WEAPONS];
	int				numWeapons;
} tactical_t;

tactical_t s_tactical;

void TacticalMenu_LoadText (void);

#define ID_COMPUTERVOISE 6
#define ID_QUIT		10
#define ID_WEAP1	11
#define ID_WEAP2	12
#define ID_WEAP3	13
#define ID_WEAP4	14
#define ID_WEAP5	15
#define ID_WEAP6	16
#define ID_FIRE		100

void UI_TacticalMenu_Cache (void);

/*
=================
M_Tactical_Event
=================
*/
static void M_Tactical_Event (void* ptr, int notification) {
	int id;
	menubitmap_s	*holdWeapon;

	id = ((menucommon_s*)ptr)->id;

	switch(id)
	{
		case ID_QUIT:
			if(notification == QM_ACTIVATED)
				UI_PopMenu();
			trap_Cmd_ExecuteText(EXEC_APPEND, "ui_tactical_free");
			break;
		case ID_WEAP1:
		case ID_WEAP2:
		case ID_WEAP3:
		case ID_WEAP4:
		case ID_WEAP5:
		case ID_WEAP6:
			if(notification == QM_ACTIVATED) {
				if(s_tactical.choosenWeapon >= 0) {
					holdWeapon = &s_tactical.weap1;
					holdWeapon += s_tactical.choosenWeapon;
					holdWeapon->textcolor = CT_BLACK;
				}

				s_tactical.choosenWeapon = id - ID_WEAP1;
				s_tactical.fire.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;

				holdWeapon = &s_tactical.weap1;
				holdWeapon += s_tactical.choosenWeapon;
			} else if(notification == QM_GOTFOCUS) {
				s_tactical.highLightedWeapon = id - ID_WEAP1;
			}
			break;
		case ID_FIRE:
			if(notification == QM_ACTIVATED) {
				//UI_ForceMenuOff();
				trap_Cmd_ExecuteText( EXEC_APPEND, va( "tactical %s", s_tactical.weaponData[s_tactical.choosenWeapon].weaponTarget ));
			}
			break;
	}
}

/*
=================
TacticalMenu_Key
=================
*/
sfxHandle_t TacticalMenu_Key (int key) {
	return ( Menu_DefaultKey( &s_tactical.menu, key ) );
}

extern qhandle_t leftRound;
qhandle_t tactical;
qhandle_t square_rl;
qhandle_t square_rr;
qhandle_t corner_tact_ul;
qhandle_t corner_tact_ll;

/*
=================
M_TacticalMenu_Graphics
=================
*/
static void M_TacticalMenu_Graphics (void) {
	menubitmap_s *holdWeapon;
	int		i,length,xWeaponStart;
	int		numColor,roundColor;

	vec4_t	bgColor = { 0.1, 0.1, 0.1, .75 };

	// background
	//UI_DrawRect(75, 420, 490, 50, bgColor);
	trap_R_SetColor(bgColor);
	UI_DrawHandlePic(150, 375, 100, 100, square_rl);
	UI_DrawHandlePic(250, 375, 140, 100, uis.whiteShader);
	UI_DrawHandlePic(390, 375, 100, 100, square_rr);

	// upper left
	trap_R_SetColor(colorTable[CT_DKPURPLE1]);
	UI_DrawHandlePic(151, 376, 75, 50, corner_tact_ul);
	UI_DrawHandlePic(218, 376, 270, 28, uis.whiteShader);
}

/*
===============
TacticalMenu_Draw
===============
*/
static void TacticalMenu_Draw(void)
{
	// Draw graphics particular to Main Menu
	M_TacticalMenu_Graphics();
	
	Menu_Draw( &s_tactical.menu );
}

/*
===============
UI_TacticalMenu_Cache
===============
*/
void UI_TacticalMenu_Cache (void)
{	
	leftRound = trap_R_RegisterShaderNoMip("menu/common/halfroundl_24.tga");
	tactical = trap_R_RegisterShaderNoMip("menu/common/lift_button.tga");
	square_rl = trap_R_RegisterShaderNoMip("menu/common/square_rounded_left.tga");
	square_rr = trap_R_RegisterShaderNoMip("menu/common/square_rounded_right.tga");
	corner_tact_ll = trap_R_RegisterShaderNoMip("menu/common/corner_tact_ll.tga");
	corner_tact_ul = trap_R_RegisterShaderNoMip("menu/common/corner_ul_18_50.tga");
}

static void UI_TacticalMenu_LoadWeapons( void )
{
	char	buffer[MAX_TOKEN_CHARS];
	int		i;
	char	*temp;

	s_tactical.numWeapons = 0;

	//load the string
	trap_GetConfigString( CS_TACTICAL_DATA, buffer, sizeof( buffer ) );

	if ( !buffer[0] )
		return;

	memset( &s_tactical.weaponData, 0, sizeof( s_tactical.weaponData ) );

	for ( i=0; i < MAX_TACTICAL_WEAPONS; i++ )
	{
		temp = Info_ValueForKey( buffer, va( "d%i", i ) );

		if ( !temp[0] )
			break;

		s_tactical.weaponData[ s_tactical.numWeapons ].weaponNum = atoi( temp );

		temp = Info_ValueForKey( buffer, va( "n%i", i ) );

		Q_strncpyz( s_tactical.weaponData[ s_tactical.numWeapons ].weaponName, temp, sizeof( s_tactical.weaponData[ s_tactical.numWeapons ].weaponName ) );
		
		s_tactical.numWeapons++;
	}
}

static void UI_ManageWeaponLoading( void )
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
	UI_LanguageFilename( mapRoute, "tactical", fileRoute );

	file_len = trap_FS_FOpenFile( fileRoute, &f, FS_READ );

	if ( file_len <= 1 )
	{
		//Com_Printf( S_COLOR_YELLOW "WARNING: Attempted to load %s, but wasn't found.\n", fileRoute );
		UI_TacticalMenu_LoadWeapons();
		return;
	}

	trap_FS_Read( buffer, file_len, f );
	trap_FS_FCloseFile( f );

	if ( !buffer[0] )
	{
		Com_Printf( S_COLOR_RED "ERROR: Attempted to load %s, but no data was read.\n", fileRoute );
		UI_TacticalMenu_LoadWeapons();
		return;
	}

	s_tactical.numWeapons = 0;
	memset( &s_tactical.weaponData, 0, sizeof( s_tactical.weaponData ) );
	buffer[file_len] = '\0';

	COM_BeginParseSession();
	textPtr = buffer;

	//Com_Printf( S_COLOR_RED "Beginning Parse\n" );

	//expected format is 'weapon num' <space> 'weapon desc' <space> 'weapon target'
	while( 1 )
	{
		token = COM_Parse( &textPtr );
		if ( !token[0] )
			break;

		//Com_Printf( S_COLOR_RED "First Token: %s\n", token );

		//grab the weapon number
		s_tactical.weaponData[s_tactical.numWeapons].weaponNum = atoi( token );

		token = COM_ParseExt( &textPtr, qfalse );
		if (!token[0])
			continue;

		//Com_Printf( S_COLOR_RED "Second Token: %s\n", token );

		Q_strncpyz( s_tactical.weaponData[s_tactical.numWeapons].weaponName, 
					token,
					sizeof( s_tactical.weaponData[s_tactical.numWeapons].weaponName ) );

		token = COM_ParseExt( &textPtr, qfalse );
		if(!token[0])
			continue;

		Q_strncpyz( s_tactical.weaponData[s_tactical.numWeapons].weaponTarget,
					token,
					sizeof(s_tactical.weaponData[s_tactical.numWeapons].weaponTarget) );

		s_tactical.numWeapons++;


	}
}

/*
===============
TacticalMenu_Init
===============
*/
void TacticalMenu_Init(void)
{
	int	y,pad,x;
	menubitmap_s	*holdWeapon;
	int		i,width;

	UI_ManageWeaponLoading();

	s_tactical.menu.fullscreen					= qfalse;
	s_tactical.menu.nobackground				= qtrue;

	s_tactical.menu.nitems						= 0;
	s_tactical.menu.draw						= TacticalMenu_Draw;
	s_tactical.menu.key							= TacticalMenu_Key;
	s_tactical.menu.wrapAround					= qtrue;
	s_tactical.menu.descX						= MENU_DESC_X;
	s_tactical.menu.descY						= MENU_DESC_Y;
	s_tactical.menu.titleX						= MENU_TITLE_X;
	s_tactical.menu.titleY						= MENU_TITLE_Y;

	s_tactical.choosenWeapon = -1;
	s_tactical.highLightedWeapon = -1;

	pad = PROP_BIG_HEIGHT + 10;
	y =  72;
	x = 319;
	width = MENU_BUTTON_MED_WIDTH-20;

	s_tactical.maxWeapons = MAX_TACTICAL_WEAPONS;
	holdWeapon = &s_tactical.weap1;

	for (i=0;i<s_tactical.maxWeapons;i++)
	{
		holdWeapon->generic.type				= MTYPE_BITMAP;      
		holdWeapon->generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
		holdWeapon->generic.x					= x;
		holdWeapon->generic.y					= y;
		holdWeapon->generic.name				= GRAPHIC_BUTTONRIGHT;
		holdWeapon->generic.id					= ID_WEAP1 + i;
		holdWeapon->generic.callback			= M_Tactical_Event; 
		holdWeapon->width						= width;
		holdWeapon->height						= PROP_BIG_HEIGHT;
		holdWeapon->color						= CT_DKPURPLE1;//CT_VDKRED1;//CT_DKGOLD1;
		holdWeapon->color2						= CT_LTPURPLE1;//CT_DKRED1;//CT_LTGOLD1;
		holdWeapon->textX						= MENU_BUTTON_TEXT_X;
		holdWeapon->textY						= 12;
		holdWeapon->textEnum					= MBT_WEAPON;
		holdWeapon->textcolor					= CT_BLACK;
		holdWeapon->textcolor2					= CT_WHITE;
		holdWeapon->textStyle					= UI_TINYFONT;

		holdWeapon++;
		y += pad;

		// Start the next column
		if (i == ((s_tactical.maxWeapons-1)/2))
		{
			x += width + 90; 
			y = 80;
		}
	}

	s_tactical.fire.generic.type				= MTYPE_BITMAP;      
	s_tactical.fire.generic.flags				= QMF_GRAYED;
	s_tactical.fire.generic.x					= 110;
	s_tactical.fire.generic.y					= 72 + (pad * 5);
	s_tactical.fire.generic.name				= GRAPHIC_BUTTONRIGHT;
	s_tactical.fire.generic.id				= ID_FIRE;
	s_tactical.fire.generic.callback			= M_Tactical_Event; 
	s_tactical.fire.width						= width;
	s_tactical.fire.height					= PROP_BIG_HEIGHT;
	s_tactical.fire.color						= CT_DKORANGE;
	s_tactical.fire.color2					= CT_LTORANGE;
	s_tactical.fire.textX						= MENU_BUTTON_TEXT_X;
	s_tactical.fire.textY						= 6;
	s_tactical.fire.textEnum					= MBT_FIRE;
	s_tactical.fire.textcolor					= CT_BLACK;
	s_tactical.fire.textcolor2				= CT_WHITE;

	s_tactical.quitmenu.generic.type			= MTYPE_BITMAP;      
	s_tactical.quitmenu.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_tactical.quitmenu.generic.x				= 110;
	s_tactical.quitmenu.generic.y				= 72 + (pad * 7);
	s_tactical.quitmenu.generic.name			= GRAPHIC_BUTTONRIGHT;
	s_tactical.quitmenu.generic.id				= ID_QUIT;
	s_tactical.quitmenu.generic.callback		= M_Tactical_Event; 
	s_tactical.quitmenu.width					= width;
	s_tactical.quitmenu.height					= PROP_BIG_HEIGHT;
	s_tactical.quitmenu.color					= CT_DKORANGE;
	s_tactical.quitmenu.color2					= CT_LTORANGE;
	s_tactical.quitmenu.textX					= MENU_BUTTON_TEXT_X;
	s_tactical.quitmenu.textY					= 6;
	s_tactical.quitmenu.textEnum				= MBT_RETURNMENU;//MBT_RETURN;
	s_tactical.quitmenu.textcolor				= CT_BLACK;
	s_tactical.quitmenu.textcolor2				= CT_WHITE;

	Menu_AddItem( &s_tactical.menu,	&s_tactical.fire );
	Menu_AddItem( &s_tactical.menu,	&s_tactical.quitmenu );

	holdWeapon = &s_tactical.weap1;
	for (i=0;i<s_tactical.maxWeapons;i++)
	{
		if (s_tactical.weaponData[i].weaponName[0])
		{
			Menu_AddItem( &s_tactical.menu,	holdWeapon );
		}
		holdWeapon++;
	}
}

/*
===============
UI_TacticalMenu
===============
*/
void UI_TacticalMenu ( void )
{
	memset( &s_tactical, 0, sizeof( s_tactical ) );

	uis.menusp = 0;

	ingameFlag = qtrue;	// true when in game menu is in use

	Mouse_Show();

	UI_TacticalMenu_Cache();

	TacticalMenu_Init(); 

	UI_PushMenu( &s_tactical.menu );

	Menu_AdjustCursor( &s_tactical.menu, 1 );	
}
