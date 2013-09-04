// Copyright (C) 1999-2000 Id Software, Inc.
//
//=================================================
//
// TiM: Just a reference for my whacky jargon in here
// Character	= the player model as a whole group (ie kulhane )
// Model		= the .model file used to build the character ( ie admiral, cadet etc)
// Skin			= the skinset field in the .model file, dictating which .skins to use (ie red/teal/gold... )
//
//=================================================
#include "ui_local.h"
#include "ui_logger.h"

#define MODEL_SELECT		"menu/art/opponents_select"
#define MODEL_SELECTED		"menu/art/opponents_selected"
//#define PIC_ARROW_LEFT		"menu/common/arrow_left_16.tga"
//#define PIC_ARROW_RIGHT		"menu/common/arrow_right_16.tga"

#define	PIC_ARROW_UP		"menu/common/arrow_up_16.tga"
#define	PIC_ARROW_DOWN		"menu/common/arrow_dn_16.tga"

#define LOW_MEMORY			(5 * 1024 * 1024)

#define MAX_PLAYERNAMELENGTH	21


static void PlayerModel_BuildList( void );
static void PlayerModel_SetMenuItems( void );
static void PlayerModel_MenuInit(int32_t menuFrom);

static char* playermodel_artlist[] =
{
	MODEL_SELECT,
	MODEL_SELECTED,
	NULL
};

int32_t s_SkinFilter_Names[] =
{
	MNT_DEFAULT,
	MNT_BLUE,
	MNT_RED,
	MNT_NONE
};

//#define PLAYERGRID_COLS		4
//#define PLAYERGRID_ROWS		3
//#define MAX_MODELSPERPAGE	(PLAYERGRID_ROWS*PLAYERGRID_COLS)

#define MAX_PLAYERCHARS		256
#define	MAX_PLAYERMODELS	12
#define	MAX_PLAYERSKINS		24

#define	MAX_MENULISTITEMS	12

#define	ID_MENUCHAR0		0
#define	ID_MENUCHAR1		1
#define	ID_MENUCHAR2		2
#define ID_MENUCHAR3		3
#define	ID_MENUCHAR4		4
#define	ID_MENUCHAR5		5
#define ID_MENUCHAR6		6
#define	ID_MENUCHAR7		7
#define ID_MENUCHAR8		8
#define ID_MENUCHAR9		9
#define ID_MENUCHAR10		10
#define ID_MENUCHAR11		11

//#define ID_PLAYERPIC0		0
//#define ID_PLAYERPIC1		1
//#define ID_PLAYERPIC2		2
//#define ID_PLAYERPIC3		3
//#define ID_PLAYERPIC4		4
//#define ID_PLAYERPIC5		5
//#define ID_PLAYERPIC6		6
//#define ID_PLAYERPIC7		7
//#define ID_PLAYERPIC8		8
//#define ID_PLAYERPIC9		9
//#define ID_PLAYERPIC10		10
//#define ID_PLAYERPIC11		11
//#define ID_PLAYERPIC12		12
//#define ID_PLAYERPIC13		13
//#define ID_PLAYERPIC14		14
//#define ID_PLAYERPIC15		15
#define ID_UPARROW			100
#define ID_DNARROW			101
#define ID_BACK				102
#define ID_MAINMENU			103
#define ID_INGAMEMENU		104

#define	ID_APPLY			105

//#define ID_SKINFILTER		112

#define ID_SETTINGS			20

typedef struct
{
	menuframework_s	menu;
	int32_t				prevMenu;
	//menubitmap_s	pics[MAX_MODELSPERPAGE];
	//menubitmap_s	picbuttons[MAX_MODELSPERPAGE];
	//menubitmap_s	framel;
	//menubitmap_s	framer;
	//menubitmap_s	ports;
	menubitmap_s	mainmenu;
	menubitmap_s	back;
	menubitmap_s	player;
	//menubitmap_s	arrows;
	//menubitmap_s	left;
	//menubitmap_s	right;

	menubitmap_s	charMenu[MAX_MENULISTITEMS];

	menubitmap_s	upArrow;
	menubitmap_s	dnArrow;

	menulist_s		charModel;
	menulist_s		charSkin;

	menubitmap_s	apply;

	menubitmap_s	data;
	menubitmap_s	model;

	//menulist_s		skinfilter;

	qhandle_t		corner_ll_4_4;
	qhandle_t		corner_ll_4_18;
	qhandle_t		corner_lr_4_18;
	qhandle_t		corner_lr_18_4;
	qhandle_t		corner_ur_18_18;

	qhandle_t		playerIcon;

	menutext_s		modelname;
	menutext_s		skinname;
	menutext_s		skinnameviewed;
	menutext_s		playername;
	playerInfo_t	playerinfo;

	int32_t				numChars;
	char			charNames[MAX_PLAYERCHARS][128];
	char*			modelNames[MAX_PLAYERMODELS];
	char*			skinNames[MAX_PLAYERSKINS];

	//int32_t				modelpage;
	//int32_t				numpages;
	char			modelData[64];
	char			modelName[32];

	int32_t				selectedChar;

	int32_t				scrollOffset;
} playermodel_t;

static playermodel_t s_playermodel;

//#define FILTER_DEFAULT	0
//#define FILTER_RED		1
//#define FILTER_BLUE		2

/*
=================
PlayerModel_LoadAvailableModels
TiM: Loads a list of all the .model
files there are in a character's directory.

NB: In Spin Control menu types, the number 
of elements is calced on init only.  Each
time we refresh this, we'll need to update ourselves.
=================
*/
int32_t PlayerModel_LoadAvailableModels( char* charModels[] ) {
	UI_LogFuncBegin();
	int32_t		i;
	int32_t		numFiles;
	char	fileList[2048]; //Hopefully, this will never be exceed ROFL
	char*	filePtr;
	int32_t		fileLen;
	int32_t		strLen;

	//Get our num files
	memset( &fileList, 0, sizeof ( fileList ) );
	numFiles = trap_FS_GetFileList( va( "models/players_rpgx/%s", s_playermodel.charNames[s_playermodel.selectedChar + s_playermodel.scrollOffset] ), ".model", fileList, sizeof(fileList) );

	if ( !numFiles ){
		UI_LogFuncEnd();
		return 0;
	}

	//Convert to ptr for easier manip
	filePtr = fileList;

	//iterate thru all the null terminations in this thing
	for ( i = 0; i < numFiles && i < MAX_PLAYERMODELS; i++, filePtr += fileLen+1 ) {
		fileLen = strlen( filePtr );

		if ( !fileLen || !filePtr )
			break;

		charModels[i] = filePtr;

		//TiM: Strip extension
		strLen = strlen( charModels[i] );
		if ( strLen > 6 && !Q_stricmp( charModels[i] + strLen - 6, ".model" ) ) {
			charModels[i][strLen-6] = '\0';
		}
	}
	UI_LogFuncEnd();
	return i;
}

/*
================
PlayerModel_LoadAvailableSkins
TiM: Access our selected .model file and 
fill the skin array with all the skins we found.

Hoi... this could get complicated... O_o
================
*/
int32_t	PlayerModel_LoadAvailableSkins( char* charSkins[] ) {
	UI_LogFuncBegin();
	int32_t				i=0;
	int32_t				fileLen;
	char			fileBuffer[2048];
	char*			filePtr;
	fileHandle_t	f;
	char*			filePath;
	char*			token;

	filePath = va( "models/players_rpgx/%s/%s.model", 
							s_playermodel.charNames[s_playermodel.selectedChar + s_playermodel.scrollOffset],
							s_playermodel.modelNames[s_playermodel.charModel.curvalue] );

	//load the .model data into our active buffer
	memset( &fileBuffer, 0, sizeof( fileBuffer ) );
	fileLen = trap_FS_FOpenFile( filePath, &f, FS_READ);
	
	if ( fileLen <= 0 ) {
		Com_Printf( S_COLOR_RED "File not found: %s\n", filePath );
		UI_LogFuncEnd();
		return 0;
	}

	if ( fileLen > sizeof ( fileBuffer ) ) {
		Com_Printf( S_COLOR_RED "File exceeded maximum size: %s\n", filePath );
		UI_LogFuncEnd();
		return 0;
	}

	trap_FS_Read( fileBuffer, sizeof( fileBuffer ), f );

	if ( !fileBuffer[0] ){
		UI_LogFuncEnd();
		return 0;
	}

	filePtr = fileBuffer;

	//Loop thru our buffer 'til we find ourselves the right skin data
	while ( i < MAX_PLAYERSKINS ) {

		token = COM_Parse( &filePtr );
		if ( !token || !filePtr )
			break;

		if ( !Q_stricmp( token, "skinName" ) ) {
			if ( COM_ParseString( &filePtr, &token ) ) {
				continue;
			} 

			charSkins[i] = token;
			i++;
			continue;
		}
	}

	UI_LogFuncEnd();
	return i;
}

/*
=================
PlayerModel_OffsetCharList

TiM: Called whenever we scroll the model list.
So it'll cycle the value of each one up and down.
==================
*/

void PlayerModel_OffsetCharList( int32_t* offset ) {
	UI_LogFuncBegin();
	char*	buffer; //intermediate value so performing strupr won't pwn our case sensitive data
	int32_t		i;

	if ( *offset < 0 ) {
		*offset = 0;
	}

	if ( ( s_playermodel.numChars > MAX_MENULISTITEMS) && (*offset > (s_playermodel.numChars - MAX_MENULISTITEMS ) ) ) {
		*offset = (s_playermodel.numChars - MAX_MENULISTITEMS );
	}

	for ( i = 0; i < MAX_MENULISTITEMS; i++ ) {
		buffer = s_playermodel.charNames[i + *offset];

		//Com_Printf( "Buffer - %s\n", buffer );

		if ( !buffer )
			break;

		s_playermodel.charMenu[i].generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
		s_playermodel.charMenu[i].textPtr = Q_strupr( buffer );
	}
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_SpinPlayer
=================
*/
static void PlayerModel_SpinPlayer( void* ptr, int32_t event)
{
	UI_LogFuncBegin();
	if ( event == QM_ACTIVATED ) 
	{
		uis.spinView = qtrue;
		uis.cursorpx = uis.cursorx;
	}
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_UpdateModel
=================
*/
static void PlayerModel_UpdateModel( void )
{
	UI_LogFuncBegin();
	vec3_t	viewangles;
	vec3_t	moveangles;

	memset( &s_playermodel.playerinfo, 0, sizeof(playerInfo_t) );
	
	/*if (uis.spinView) {
//		yawDelta = ( uis.cursorx - uis.cursorpx ) + placeHolder / ( uis.frametime / 1000.0f ) ) / 20.0f*/; //5.0f

//		yaw = AngleNormalize360 ( yawDelta );
//	} 

	viewangles[YAW]   = uis.lastYaw;
	viewangles[PITCH] = 0;
	viewangles[ROLL]  = 0;
	VectorClear( moveangles );

	UI_PlayerInfo_SetModel( &s_playermodel.playerinfo, s_playermodel.modelData );
	UI_PlayerInfo_SetInfo( &s_playermodel.playerinfo, BOTH_STAND4, BOTH_STAND4, viewangles, moveangles, WP_COMPRESSION_RIFLE, qfalse );
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_SaveChanges
=================
*/
static void PlayerModel_SaveChanges( void )
{
	trap_Cvar_Set( "model", s_playermodel.modelData );
}

/*
=================
PlayerModel_MenuEvent
=================
*/
static void PlayerModel_MenuEvent( void* ptr, int32_t event )
{
	UI_LogFuncBegin();
	if (event != QM_ACTIVATED){
		UI_LogFuncEnd();
		return;
	}

	switch (((menucommon_s*)ptr)->id)
	{
		case ID_BACK:
			PlayerModel_SaveChanges();
			UI_PopMenu();
			break;

		case ID_MAINMENU:
			PlayerModel_SaveChanges();
			UI_MainMenu();
			break;

		case ID_INGAMEMENU:
			PlayerModel_SaveChanges();
			UI_InGameMenu();
			break;

		case ID_SETTINGS:
			UI_PopMenu();
			PlayerModel_SaveChanges();
			UI_PlayerSettingsMenu(s_playermodel.prevMenu); 
			break;

		case ID_DNARROW:
			s_playermodel.scrollOffset++;
			PlayerModel_OffsetCharList( &s_playermodel.scrollOffset );
			break;

		case ID_UPARROW:
			s_playermodel.scrollOffset--;
			PlayerModel_OffsetCharList( &s_playermodel.scrollOffset );
			break;

		case ID_MENUCHAR0:
		case ID_MENUCHAR1:
		case ID_MENUCHAR2:
		case ID_MENUCHAR3:
		case ID_MENUCHAR4:
		case ID_MENUCHAR5:
		case ID_MENUCHAR6:
		case ID_MENUCHAR7:
		case ID_MENUCHAR8:
		case ID_MENUCHAR9:
		case ID_MENUCHAR10:
		case ID_MENUCHAR11:
			s_playermodel.selectedChar = ( ((menucommon_s*)ptr)->id - ID_MENUCHAR0 ) + s_playermodel.scrollOffset;

			s_playermodel.playerIcon = trap_R_RegisterShaderNoMip( va( "models/players_rpgx/%s/model_icon.jpg", s_playermodel.charNames[s_playermodel.selectedChar] ) );
			break;
	}
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_MenuKey
=================
*/
static sfxHandle_t PlayerModel_MenuKey( int32_t key )
{
	UI_LogFuncBegin();
	switch (key)
	{		
		case K_MOUSE2:
		case K_ESCAPE:
			PlayerModel_SaveChanges();
			break;
	}
	UI_LogFuncEnd();
	return ( Menu_DefaultKey( &s_playermodel.menu, key ) );
}

/*
=================
PlayerModel_DrawPlayer
=================
*/
static void PlayerModel_DrawPlayer( void *self )
{
	UI_LogFuncBegin();
	menubitmap_s*	b;
	vec3_t			origin = {-15, 0, 0 }; //-3.8

	b = (menubitmap_s*) self;

	if( trap_MemoryRemaining() <= LOW_MEMORY ) {
		UI_DrawProportionalString( b->generic.x, b->generic.y + b->height / 2, "LOW MEMORY", UI_LEFT, color_red );
		UI_LogFuncEnd();
		return;
	}

	UI_DrawPlayer( b->generic.x, b->generic.y, b->width, b->height, origin, &s_playermodel.playerinfo, uis.realtime/2 );
	UI_LogFuncEnd();
}

extern char* BG_RegisterRace( const char *name );


/*
=================
PlayerModel_BuildList

Heavily modifed by TiM
All we'll take into account now
is a valid directory name,
and that it contains a .model file
We'll work the rest out later
=================
*/
static void PlayerModel_BuildList( void )
{
	UI_LogFuncBegin();
	int32_t		numdirs;
	int32_t		numfiles;
	char	dirlist[8192];
	char	filelist[2048];
	//char	skinname[64];
	char*	dirptr;
	//char*	fileptr;
	int32_t		i;
	//int32_t		j;
	int32_t		dirlen;
	//int32_t		filelen;
	//qboolean precache;

	//precache = trap_Cvar_VariableValue("com_buildscript");

	s_playermodel.selectedChar = 0;
	//s_playermodel.modelpage = 0;
	s_playermodel.numChars = 0;

	// iterate directory of all player models
	numdirs = trap_FS_GetFileList("models/players_rpgx", "/", dirlist, sizeof(dirlist) );
	dirptr  = dirlist;
	
	///Com_Printf("%i folders found\n", numdirs );

	for (i=0; i<numdirs && s_playermodel.numChars < MAX_PLAYERCHARS; i++,dirptr+=dirlen+1)
	{
		numfiles = 0;
		
		dirlen = strlen(dirptr);

		if ( !dirptr ) {
			break;
		}

		if (dirlen && dirptr[dirlen-1]=='/') 
			dirptr[dirlen-1]='\0';

		//I'm guessing this is for non-PK3'd files
		if (!strcmp(dirptr,".") || !strcmp(dirptr,".."))
			continue;
			
		// TiM : Check for .model files.  That's all we need
		numfiles = trap_FS_GetFileList( va("models/players_rpgx/%s",dirptr), ".model", filelist, sizeof(filelist) );

		if ( numfiles > 0 && dirptr[0] ) {
			//Com_Printf( "Hashed %s with %i model files\n", dirptr, numfiles);

			Q_strncpyz( s_playermodel.charNames[s_playermodel.numChars], dirptr, sizeof( s_playermodel.charNames[s_playermodel.numChars] ) );
			s_playermodel.numChars++;
		}
	}
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_SetMenuItems
=================
*/
static void PlayerModel_SetMenuItems( void )
{
	UI_LogFuncBegin();
	int32_t				i;
	//int32_t				maxlen;
	//char			modelskin[64];
	//char*			buffptr;
	//char*			pdest;
	char*			temp;
	char			model[64];

	// name
	trap_Cvar_VariableStringBuffer( "name", s_playermodel.playername.string, MAX_PLAYERNAMELENGTH );

	// model
	trap_Cvar_VariableStringBuffer( "model", s_playermodel.modelData, 64 );

	if ( ( temp = strchr( UI_Cvar_VariableString("model"), '/' ) ) == NULL ) {
		Q_strncpyz( s_playermodel.modelName, UI_Cvar_VariableString("model"), sizeof( s_playermodel.modelName ) );
	} else { 
		int32_t len;
//
		len = strlen( temp );
		Q_strncpyz( s_playermodel.modelName, UI_Cvar_VariableString("model"), (len - strlen(temp)) + 1 );
	}
	
	// find model in our list
	for (i=0; i<s_playermodel.numChars; i++)
	{
		if ( !Q_stricmp( model, s_playermodel.charNames[i] ) ) {
			s_playermodel.selectedChar = i;
			break;
		}
	}

	//try to register the current shader icon
	s_playermodel.playerIcon = trap_R_RegisterShaderNoMip( va( "models/players_rpgx/%s/model_icon", s_playermodel.modelName ) );

	//Huh... guess whatever the hell's in the console, we don't have it on our list
	if ( s_playermodel.selectedChar == -1 ) {
		UI_LogFuncEnd();
		return;
	}

	//If we're exceeded the list, update it so we're at the top
	if ( s_playermodel.selectedChar > MAX_MENULISTITEMS ) {
		s_playermodel.scrollOffset = s_playermodel.selectedChar - MAX_MENULISTITEMS;
		PlayerModel_OffsetCharList( &s_playermodel.scrollOffset );
	}

	UI_LogFuncEnd();
}

/*
=================
PlayerSettingsMenu_Graphics
=================
*/
void PlayerModelMenu_Graphics (void)
{
	UI_LogFuncBegin();
	// Draw the basic screen layout
	UI_MenuFrame2(&s_playermodel.menu);

	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30,203, 47, 186, uis.whiteShader);	// Middle left line

	// Frame around model pictures
	trap_R_SetColor( colorTable[CT_LTORANGE]);

	//TiM - Frame around the models selection list
	UI_DrawHandlePic(  96,  50,   8,  -32, s_playermodel.corner_ll_4_18);	// UL Corner
	UI_DrawHandlePic(  96, 369,   8,  8, s_playermodel.corner_ll_4_4);	// LL Corner
	UI_DrawHandlePic(  238,  62,   32,  32, s_playermodel.corner_ur_18_18);	// UR Corner
	UI_DrawHandlePic(  240, 368,   32,  8, s_playermodel.corner_lr_18_4);	// LR Corner
	UI_DrawHandlePic(  96,  81,   4, 290, uis.whiteShader);	// Left side
	UI_DrawHandlePic(  242,	87,	18, 18, uis.whiteShader ); //Right Side Up Arrow Button
	UI_DrawHandlePic(  242,  108,   18, 236, uis.whiteShader);	// Right side
	UI_DrawHandlePic(  242, 347, 18, 18, uis.whiteShader );		//Right Side Down Button
	UI_DrawHandlePic(  100,  62, 141,  18, uis.whiteShader);	// Top
	UI_DrawHandlePic(  101, 371, 140,  4, uis.whiteShader);	// Bottom

	//TiM - Fram around the model specific data window
	UI_DrawHandlePic(  265,  50,   8,  -32, s_playermodel.corner_ll_4_18);	// UL Corner
	UI_DrawHandlePic(  265, 369,   8,  8, s_playermodel.corner_ll_4_4);	// LL Corner
	UI_DrawHandlePic(  422,  50,   -8,  -32, s_playermodel.corner_ll_4_18);	// UR Corner
	UI_DrawHandlePic(  422, 369,   -8,  8, s_playermodel.corner_ll_4_4);	// LR Corner
	UI_DrawHandlePic(  265,  81,   4, 290, uis.whiteShader);	// Left side
	UI_DrawHandlePic(  426,  81,   4, 290, uis.whiteShader);	// Right side
	UI_DrawHandlePic(  269,  62, 157,  18, uis.whiteShader);	// Top
	UI_DrawHandlePic(  269, 371, 157,  4, uis.whiteShader);		// Bottom

	//TiM - Draw the stunningly awesome icon of the character
	UI_DrawHandlePic( 306, 114, 82, 82, uis.whiteShader);
	if ( !s_playermodel.playerIcon ) {
		trap_R_SetColor( colorTable[CT_BLACK] );
		UI_DrawHandlePic( 307, 115, 80, 80, uis.whiteShader );
		UI_DrawProportionalString( 347, 145, "?", UI_BIGFONT|UI_CENTER, colorTable[CT_LTORANGE] );
	}
	else {
		trap_R_SetColor( colorTable[CT_WHITE]);
		UI_DrawHandlePic( 307, 115, 80, 80, s_playermodel.playerIcon );
	}

	//Model Name along the top
	if ( s_playermodel.modelName[0] )
	{
		char* buf = s_playermodel.modelName;

		UI_DrawProportionalString( 347,  89, Q_strupr( buf ), UI_SMALLFONT|UI_CENTER,colorTable[CT_DKPURPLE1]);
	}

	//UI_DrawProportionalString(  220,  362, va("%s %d %s %d",menu_normal_text[MNT_SCREEN],(s_playermodel.modelpage + 1),menu_normal_text[MNT_OF],s_playermodel.numpages),UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	UI_DrawProportionalString( 108,  64, menu_normal_text[MNT_MODELS],UI_SMALLFONT,colorTable[CT_BLACK]);	// Top

	trap_R_SetColor( colorTable[CT_DKGREY2]);
	UI_DrawHandlePic(  439, 79, 151,   295, uis.whiteShader);	// Background

	// Frame around player model
	trap_R_SetColor( colorTable[CT_LTORANGE]);
	UI_DrawHandlePic( 435,  50,   8,  -32, s_playermodel.corner_ll_4_18);	// UL Corner
	UI_DrawHandlePic( 435, 369,   8,   8, s_playermodel.corner_ll_4_4);	// LL Corner
	UI_DrawHandlePic( 440,  62, 150,  18, uis.whiteShader);	// Top
	UI_DrawHandlePic( 435,  79,   4, 295, uis.whiteShader);	// Left side
	UI_DrawHandlePic( 440, 371, 150,   4, uis.whiteShader);	// Bottom
	
	// Left rounded ends for buttons
	trap_R_SetColor( colorTable[s_playermodel.mainmenu.color]);
	UI_DrawHandlePic(s_playermodel.mainmenu.generic.x - 14, s_playermodel.mainmenu.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor( colorTable[s_playermodel.back.color]);
	UI_DrawHandlePic(s_playermodel.back.generic.x - 14, s_playermodel.back.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor( colorTable[s_playermodel.data.color]);
	UI_DrawHandlePic(s_playermodel.data.generic.x - 14, s_playermodel.data.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor( colorTable[s_playermodel.model.color]);
	UI_DrawHandlePic(s_playermodel.model.generic.x - 14, s_playermodel.model.generic.y, 
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);


	UI_DrawProportionalString(  74,   28, "881",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  150, "2445",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  206, "600",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "3-44",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	UI_LogFuncEnd();
}

/*
=================
PlayerSettings_MenuDraw
=================
*/
static void PlayerModel_MenuDraw (void)
{
	UI_LogFuncBegin();
	PlayerModelMenu_Graphics();

	Menu_Draw( &s_playermodel.menu );
	UI_LogFuncEnd();
}
/*
=================
PlayerModel_MenuInit
=================
*/
static void PlayerModel_MenuInit(int32_t menuFrom)
{
	UI_LogFuncBegin();
	int32_t			i;
	//int32_t			j;
	//int32_t			k;
	int32_t			x;
	int32_t			y;
	static char	playername[32];
	static char	modelname[32];
	static char	skinname[32];

	// zero set all our globals
	memset( &s_playermodel, 0 ,sizeof(playermodel_t) );

	s_playermodel.prevMenu = menuFrom;

	//TiM : Model Spin view
	uis.spinView = qfalse;
	uis.lastYaw = 150;

	PlayerModel_Cache();

	// set initial states
	PlayerModel_BuildList();

	PlayerModel_SetMenuItems();

	s_playermodel.menu.key							= PlayerModel_MenuKey;
	s_playermodel.menu.wrapAround					= qtrue;
	s_playermodel.menu.fullscreen					= qtrue;
    s_playermodel.menu.draw							= PlayerModel_MenuDraw;
	s_playermodel.menu.descX						= MENU_DESC_X;
	s_playermodel.menu.descY						= MENU_DESC_Y;
	s_playermodel.menu.titleX						= MENU_TITLE_X;
	s_playermodel.menu.titleY						= MENU_TITLE_Y;
	s_playermodel.menu.titleI						= MNT_CHANGEPLAYER_TITLE;
	s_playermodel.menu.footNoteEnum					= MNT_CHANGEPLAYER;

	s_playermodel.mainmenu.generic.type				= MTYPE_BITMAP;      
	s_playermodel.mainmenu.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_playermodel.mainmenu.generic.x					= 110;
	s_playermodel.mainmenu.generic.y					= 391;
	s_playermodel.mainmenu.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_playermodel.mainmenu.generic.callback			= PlayerModel_MenuEvent;
	s_playermodel.mainmenu.width						= MENU_BUTTON_MED_WIDTH;
	s_playermodel.mainmenu.height					= MENU_BUTTON_MED_HEIGHT;
	s_playermodel.mainmenu.color						= CT_DKPURPLE1;
	s_playermodel.mainmenu.color2					= CT_LTPURPLE1;

	if (!ingameFlag)
	{
		s_playermodel.mainmenu.textEnum					= MBT_MAINMENU;
		s_playermodel.mainmenu.generic.id				= ID_MAINMENU;
	}
	else 
	{
		s_playermodel.mainmenu.textEnum					= MBT_INGAMEMENU;
		s_playermodel.mainmenu.generic.id				= ID_INGAMEMENU;
	}

	s_playermodel.mainmenu.textX						= MENU_BUTTON_TEXT_X;
	s_playermodel.mainmenu.textY						= MENU_BUTTON_TEXT_Y;
	s_playermodel.mainmenu.textcolor					= CT_BLACK;
	s_playermodel.mainmenu.textcolor2				= CT_WHITE;

	s_playermodel.back.generic.type					= MTYPE_BITMAP;
	s_playermodel.back.generic.name					= BUTTON_GRAPHIC_LONGRIGHT;
	s_playermodel.back.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_playermodel.back.generic.callback				= PlayerModel_MenuEvent;
	s_playermodel.back.generic.id					= ID_BACK;
	s_playermodel.back.generic.x					= 110;
	s_playermodel.back.generic.y					= 415;
	s_playermodel.back.width  						= MENU_BUTTON_MED_WIDTH;
	s_playermodel.back.height  						= MENU_BUTTON_MED_HEIGHT;
	s_playermodel.back.color						= CT_DKPURPLE1;
	s_playermodel.back.color2						= CT_LTPURPLE1;
	s_playermodel.back.textX						= MENU_BUTTON_TEXT_X;
	s_playermodel.back.textY						= MENU_BUTTON_TEXT_Y;
	s_playermodel.back.textEnum						= MBT_BACK;
	s_playermodel.back.generic.id					= ID_BACK;
	s_playermodel.back.textcolor					= CT_BLACK;
	s_playermodel.back.textcolor2					= CT_WHITE;

	s_playermodel.data.generic.type					= MTYPE_BITMAP;
	s_playermodel.data.generic.name					= BUTTON_GRAPHIC_LONGRIGHT;
	s_playermodel.data.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_playermodel.data.generic.id					= ID_SETTINGS;
	s_playermodel.data.generic.callback				= PlayerModel_MenuEvent;
	s_playermodel.data.generic.x					= 482;
	s_playermodel.data.generic.y					= 391;
	s_playermodel.data.width						= MENU_BUTTON_MED_WIDTH;
	s_playermodel.data.height						= MENU_BUTTON_MED_HEIGHT;
	s_playermodel.data.color						= CT_DKPURPLE1;
	s_playermodel.data.color2						= CT_LTPURPLE1;
	s_playermodel.data.textX						= 5;
	s_playermodel.data.textY						= 2;
	s_playermodel.data.textEnum						= MBT_PLAYERDATA;
	s_playermodel.data.textcolor					= CT_BLACK;
	s_playermodel.data.textcolor2					= CT_WHITE;

	s_playermodel.model.generic.type				= MTYPE_BITMAP;
	s_playermodel.model.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_playermodel.model.generic.flags				= QMF_GRAYED;
	s_playermodel.model.generic.x					= 482;
	s_playermodel.model.generic.y					= 415;
	s_playermodel.model.width						= MENU_BUTTON_MED_WIDTH;
	s_playermodel.model.height						= MENU_BUTTON_MED_HEIGHT;
	s_playermodel.model.color						= CT_DKPURPLE1;
	s_playermodel.model.color2						= CT_LTPURPLE1;
	s_playermodel.model.textX						= 5;
	s_playermodel.model.textY						= 2;
	s_playermodel.model.textEnum					= MBT_CHANGEMODEL;
	s_playermodel.model.textcolor					= CT_BLACK;
	s_playermodel.model.textcolor2					= CT_WHITE;


	//y =	88;
	x = 107;
	y = 85;

	for (i=0; i < MAX_MENULISTITEMS; i++ ) {
		s_playermodel.charMenu[i].generic.type			= MTYPE_BITMAP;
		s_playermodel.charMenu[i].generic.flags			= QMF_INACTIVE | QMF_HIDDEN;
		s_playermodel.charMenu[i].generic.x				= x;
		s_playermodel.charMenu[i].generic.y				= y;
		s_playermodel.charMenu[i].generic.callback		= PlayerModel_MenuEvent;
		s_playermodel.charMenu[i].generic.id			= ID_MENUCHAR0+i;
		s_playermodel.charMenu[i].width					= 129;
		s_playermodel.charMenu[i].height				= 16;
		s_playermodel.charMenu[i].color					= CT_DKPURPLE1;
		s_playermodel.charMenu[i].color2				= CT_LTPURPLE1;
		s_playermodel.charMenu[i].textPtr				= NULL;
		s_playermodel.charMenu[i].textX					= 4;
		s_playermodel.charMenu[i].textY					= 1;
		s_playermodel.charMenu[i].textcolor				= CT_DKGOLD1;
		s_playermodel.charMenu[i].textcolor2			= CT_LTGOLD1;
		s_playermodel.charMenu[i].textStyle				= UI_SMALLFONT;

		y += 24;
	}

	s_playermodel.playername.generic.type			= MTYPE_PTEXT;
	s_playermodel.playername.generic.flags			= QMF_INACTIVE;
	s_playermodel.playername.generic.x				= 444;
	s_playermodel.playername.generic.y				= 63;
	s_playermodel.playername.string					= playername;
	s_playermodel.playername.style					= UI_SMALLFONT;
	s_playermodel.playername.color					= colorTable[CT_BLACK];

	s_playermodel.modelname.generic.type			= MTYPE_PTEXT;
	s_playermodel.modelname.generic.flags			= QMF_INACTIVE;
	s_playermodel.modelname.generic.x				= 121;
	s_playermodel.modelname.generic.y				= 338;
	s_playermodel.modelname.string					= modelname;
	s_playermodel.modelname.style					= UI_LEFT;
	s_playermodel.modelname.color					= colorTable[CT_LTBLUE1];

	s_playermodel.skinname.generic.type				= MTYPE_PTEXT;
	s_playermodel.skinname.generic.flags			= QMF_INACTIVE;
	s_playermodel.skinname.generic.x				= 323;
	s_playermodel.skinname.generic.y				= 338;
	s_playermodel.skinname.string					= skinname;
	s_playermodel.skinname.style					= UI_RIGHT;
	s_playermodel.skinname.color					= colorTable[CT_LTBLUE1];

	s_playermodel.player.generic.type				= MTYPE_BITMAP;
	s_playermodel.player.generic.flags				= QMF_SILENT;
	s_playermodel.player.generic.ownerdraw			= PlayerModel_DrawPlayer;
	s_playermodel.player.generic.callback			= PlayerModel_SpinPlayer;
	s_playermodel.player.generic.x					= 439; //400
	s_playermodel.player.generic.y					= 80; //20
	s_playermodel.player.width						= 151; //32*7.3
	s_playermodel.player.height						= 291; //56*7.3

	s_playermodel.upArrow.generic.type				= MTYPE_BITMAP;
	s_playermodel.upArrow.generic.name				= PIC_ARROW_UP;
	s_playermodel.upArrow.generic.flags				= QMF_GRAYED | QMF_INACTIVE;
	s_playermodel.upArrow.generic.callback			= PlayerModel_MenuEvent;
	s_playermodel.upArrow.generic.id				= ID_UPARROW;
	s_playermodel.upArrow.generic.x					= 243;
	s_playermodel.upArrow.generic.y					= 89;
	s_playermodel.upArrow.width  					= 16;
	s_playermodel.upArrow.height  					= 16;
	s_playermodel.upArrow.color						= CT_DKPURPLE1;
	s_playermodel.upArrow.color2					= CT_LTPURPLE1;
	s_playermodel.upArrow.textX						= MENU_BUTTON_TEXT_X;
	s_playermodel.upArrow.textY						= MENU_BUTTON_TEXT_Y;
	//s_playermodel.upArrow.textEnum					= MBT_PREVPAGE;
	s_playermodel.upArrow.textcolor					= CT_BLACK;
	s_playermodel.upArrow.textcolor2				= CT_WHITE;

	s_playermodel.dnArrow.generic.type				= MTYPE_BITMAP;
	s_playermodel.dnArrow.generic.name				= PIC_ARROW_DOWN;
	s_playermodel.dnArrow.generic.flags				= QMF_GRAYED | QMF_INACTIVE;
	s_playermodel.dnArrow.generic.callback			= PlayerModel_MenuEvent;
	s_playermodel.dnArrow.generic.id				= ID_DNARROW;
	s_playermodel.dnArrow.generic.x					= 243;
	s_playermodel.dnArrow.generic.y					= 349;
	s_playermodel.dnArrow.width  					= 16;
	s_playermodel.dnArrow.height  					= 16;
	s_playermodel.dnArrow.color						= CT_DKPURPLE1;
	s_playermodel.dnArrow.color2					= CT_LTPURPLE1;
	s_playermodel.dnArrow.textX						= MENU_BUTTON_TEXT_X;
	s_playermodel.dnArrow.textY						= MENU_BUTTON_TEXT_Y;
	//s_playermodel.dnArrow.textEnum					= MBT_NEXTPAGE;
	s_playermodel.dnArrow.textcolor					= CT_BLACK;
	s_playermodel.dnArrow.textcolor2				= CT_WHITE;

	s_playermodel.charModel.generic.type				= MTYPE_SPINCONTROL;
	s_playermodel.charModel.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	//s_playermodel.charModel.generic.id					= ID_CLASS;
	//s_playermodel.charModel.generic.callback			= PlayerModel_MenuEvent;
	s_playermodel.charModel.generic.x					= 291;//134;
	s_playermodel.charModel.generic.y					= 209;//207;
	s_playermodel.charModel.textEnum					= MBT_CHARMODEL;
	s_playermodel.charModel.textcolor					= CT_BLACK;
	s_playermodel.charModel.textcolor2					= CT_WHITE;
	s_playermodel.charModel.color						= CT_DKPURPLE1;
	s_playermodel.charModel.color2						= CT_LTPURPLE1;
	s_playermodel.charModel.width						= 60;//80;
	s_playermodel.charModel.textX						= 56;
	s_playermodel.charModel.textY						= 2;
	s_playermodel.charModel.textFlags					= UI_CENTER;
	s_playermodel.charModel.listX						= 347;
	s_playermodel.charModel.listY						= 233;
	s_playermodel.charModel.listFlags					= UI_CENTER;
	//s_playermodel.charModel.itemnames					= s_playermodel.modelNames;

	s_playermodel.charSkin.generic.type				= MTYPE_SPINCONTROL;
	s_playermodel.charSkin.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	//s_playermodel.charSkin.generic.id					= ID_CLASS;
	//s_playermodel.charSkin.generic.callback			= PlayerModel_MenuEvent;
	s_playermodel.charSkin.generic.x				= 291;//134;
	s_playermodel.charSkin.generic.y				= 266;//207;
	s_playermodel.charSkin.textEnum					= MBT_CHARSKIN;
	s_playermodel.charSkin.textcolor				= CT_BLACK;
	s_playermodel.charSkin.textcolor2				= CT_WHITE;
	s_playermodel.charSkin.color					= CT_DKPURPLE1;
	s_playermodel.charSkin.color2					= CT_LTPURPLE1;
	s_playermodel.charSkin.width					= 60;//80;
	s_playermodel.charSkin.textX					= 56;
	s_playermodel.charSkin.textY					= 2;
	s_playermodel.charSkin.textFlags				= UI_CENTER;
	s_playermodel.charSkin.listX					= 347;
	s_playermodel.charSkin.listY					= 291;
	s_playermodel.charSkin.listFlags				= UI_CENTER;
	//s_playermodel.charSkin.itemnames				= s_playermodel.skinNames;

	s_playermodel.apply.generic.type				= MTYPE_BITMAP;
	s_playermodel.apply.generic.name				= GRAPHIC_SQUARE;
	s_playermodel.apply.generic.flags				= QMF_GRAYED | QMF_INACTIVE;
	s_playermodel.apply.generic.callback			= PlayerModel_MenuEvent;
	s_playermodel.apply.generic.id					= ID_APPLY;
	s_playermodel.apply.generic.x					= 281;
	s_playermodel.apply.generic.y					= 321;
	s_playermodel.apply.width						= 132;
	s_playermodel.apply.height						= 39;
	s_playermodel.apply.color						= CT_DKPURPLE1;
	s_playermodel.apply.color2						= CT_LTPURPLE1;
	s_playermodel.apply.textX						= MENU_BUTTON_TEXT_X;
	s_playermodel.apply.textY						= MENU_BUTTON_TEXT_Y;
	s_playermodel.apply.textEnum					= MBT_VIDEOAPPLYCHANGES;
	s_playermodel.apply.textcolor					= CT_BLACK;
	s_playermodel.apply.textcolor2					= CT_WHITE;

	Menu_AddItem( &s_playermodel.menu,	&s_playermodel.model );
	Menu_AddItem( &s_playermodel.menu,	&s_playermodel.data );
	Menu_AddItem( &s_playermodel.menu,	&s_playermodel.player );
	Menu_AddItem( &s_playermodel.menu,	&s_playermodel.playername );
	Menu_AddItem( &s_playermodel.menu,	&s_playermodel.modelname );

	Menu_AddItem( &s_playermodel.menu,	&s_playermodel.upArrow );
	Menu_AddItem( &s_playermodel.menu,	&s_playermodel.dnArrow );
	Menu_AddItem( &s_playermodel.menu,	&s_playermodel.charModel );
	Menu_AddItem( &s_playermodel.menu,	&s_playermodel.charSkin );
	Menu_AddItem( &s_playermodel.menu,	&s_playermodel.apply );

	for (i=0; i < MAX_MENULISTITEMS; i++ ) {
		Menu_AddItem( &s_playermodel.menu, &s_playermodel.charMenu[i] );
	}

	Menu_AddItem( &s_playermodel.menu,	&s_playermodel.back );
	Menu_AddItem( &s_playermodel.menu,	&s_playermodel.mainmenu );

	PlayerModel_OffsetCharList( &s_playermodel.scrollOffset );

	if ( s_playermodel.numChars > MAX_MENULISTITEMS ) {
		s_playermodel.upArrow.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
		s_playermodel.dnArrow.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	}

	// update user interface
	PlayerModel_UpdateModel();
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_Cache
=================
*/
void PlayerModel_Cache( void )
{
	UI_LogFuncBegin();

	s_playermodel.corner_ll_4_4		= trap_R_RegisterShaderNoMip("menu/common/corner_ll_4_4");
	s_playermodel.corner_ll_4_18	= trap_R_RegisterShaderNoMip("menu/common/corner_ll_4_18");
	s_playermodel.corner_lr_4_18	= trap_R_RegisterShaderNoMip("menu/common/corner_lr_4_18");
	s_playermodel.corner_lr_18_4	= trap_R_RegisterShaderNoMip("menu/common/corner_lr_18_4");
	s_playermodel.corner_ur_18_18	= trap_R_RegisterShaderNoMip("menu/common/corner_ur_18_18");

	trap_R_RegisterShaderNoMip(PIC_ARROW_UP);
	trap_R_RegisterShaderNoMip(PIC_ARROW_DOWN);
	UI_LogFuncEnd();
}

/*
=================
PlayerModel_Cache
=================
*/
void UI_PlayerModelMenu(int32_t menuFrom)
{
	UI_LogFuncBegin();

	PlayerModel_MenuInit(menuFrom);

	UI_PushMenu( &s_playermodel.menu );

	UI_LogFuncEnd();
}


