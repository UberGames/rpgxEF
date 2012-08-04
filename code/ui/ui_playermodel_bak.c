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
static void PlayerModel_MenuInit(int menuFrom);

static char* playermodel_artlist[] =
{
	MODEL_SELECT,
	MODEL_SELECTED,
	NULL
};

int s_SkinFilter_Names[] =
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
	int				prevMenu;
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

	int				numChars;
	char			charNames[MAX_PLAYERCHARS][128];
	char*			modelNames[MAX_PLAYERMODELS];
	char*			skinNames[MAX_PLAYERSKINS];

	//int				modelpage;
	//int				numpages;
	char			modelData[64];
	char			modelName[32];

	int				selectedChar;

	int				scrollOffset;
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
int PlayerModel_LoadAvailableModels( char* charModels[] ) {
	int		i;
	int		numFiles;
	char	fileList[2048]; //Hopefully, this will never be exceed ROFL
	char*	filePtr;
	int		fileLen;
	int		strLen;

	//Get our num files
	memset( &fileList, 0, sizeof ( fileList ) );
	numFiles = trap_FS_GetFileList( va( "models/players_rpgx/%s", s_playermodel.charNames[s_playermodel.selectedChar + s_playermodel.scrollOffset] ), ".model", fileList, sizeof(fileList) );

	if ( !numFiles )
		return 0;

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
int	PlayerModel_LoadAvailableSkins( char* charSkins[] ) {
	int				i=0;
	int				fileLen;
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
		return 0;
	}

	if ( fileLen > sizeof ( fileBuffer ) ) {
		Com_Printf( S_COLOR_RED "File exceeded maximum size: %s\n", filePath );
		return 0;
	}

	trap_FS_Read( fileBuffer, sizeof( fileBuffer ), f );

	if ( !fileBuffer[0] )
		return 0;

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

	return i;
}

/*
=================
PlayerModel_OffsetCharList

TiM: Called whenever we scroll the model list.
So it'll cycle the value of each one up and down.
==================
*/

void PlayerModel_OffsetCharList( int* offset ) {
	char*	buffer; //intermediate value so performing strupr won't pwn our case sensitive data
	int		i;

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
}

/*
=================
PlayerModel_UpdateGrid
=================
*/
/*static void PlayerModel_UpdateGrid( void )
{
	int	i;
    int	j;

	j = s_playermodel.modelpage * MAX_MODELSPERPAGE;
	for (i=0; i<PLAYERGRID_ROWS*PLAYERGRID_COLS; i++,j++)
	{
		if (j < s_playermodel.nummodels)
		{ 
			// model/skin portrait
 			s_playermodel.pics[i].generic.name         = s_playermodel.modelnames[j];
			s_playermodel.picbuttons[i].generic.flags &= ~QMF_INACTIVE;
		}
		else
		{
			// dead slot
 			s_playermodel.pics[i].generic.name         = NULL;
			s_playermodel.picbuttons[i].generic.flags |= QMF_INACTIVE;
		}

 		s_playermodel.pics[i].generic.flags       &= ~QMF_HIGHLIGHT;
 		s_playermodel.pics[i].shader               = 0;
 		s_playermodel.picbuttons[i].generic.flags |= QMF_PULSEIFFOCUS;
	}

	if (s_playermodel.selectedmodel/MAX_MODELSPERPAGE == s_playermodel.modelpage)
	{
		// set selected model
		i = s_playermodel.selectedmodel % MAX_MODELSPERPAGE;

		s_playermodel.pics[i].generic.flags       |= QMF_HIGHLIGHT;
		s_playermodel.picbuttons[i].generic.flags &= ~QMF_PULSEIFFOCUS;

	}

	if (s_playermodel.numpages > 1)
	{
		if (s_playermodel.modelpage > 0)
		{
			s_playermodel.left.generic.flags &= ~QMF_INACTIVE; 
			s_playermodel.left.generic.flags &= ~QMF_GRAYED;
		}
		else
		{
			s_playermodel.left.generic.flags |= QMF_INACTIVE | QMF_GRAYED;
		}

		if (s_playermodel.modelpage < s_playermodel.numpages-1)
		{
			s_playermodel.right.generic.flags &= ~QMF_INACTIVE;
			s_playermodel.right.generic.flags &= ~QMF_GRAYED;
		}
		else
		{
			s_playermodel.right.generic.flags |= QMF_INACTIVE | QMF_GRAYED;
		}
	}
	else
	{
		// hide left/right markers
		s_playermodel.left.generic.flags |= QMF_INACTIVE | QMF_GRAYED;
		s_playermodel.right.generic.flags |= QMF_INACTIVE | QMF_GRAYED;
	}
}*/

/*
=================
PlayerModel_SpinPlayer
=================
*/
static void PlayerModel_SpinPlayer( void* ptr, int event)
{
	if ( event == QM_ACTIVATED ) 
	{
		uis.spinView = qtrue;
		uis.cursorpx = uis.cursorx;
	}
}

/*
=================
PlayerModel_UpdateModel
=================
*/
static void PlayerModel_UpdateModel( void )
{
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
static void PlayerModel_MenuEvent( void* ptr, int event )
{

	if (event != QM_ACTIVATED)
		return;

	switch (((menucommon_s*)ptr)->id)
	{
		/*case ID_PREVPAGE:
			if (s_playermodel.modelpage > 0)
			{
				s_playermodel.modelpage--;
				PlayerModel_UpdateGrid();
			}
			break;

		case ID_NEXTPAGE:
			if (s_playermodel.modelpage < s_playermodel.numpages-1)
			{
				s_playermodel.modelpage++;
				PlayerModel_UpdateGrid();
			}
			break;*/

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

		//case ID_SKINFILTER:
			//PlayerModel_BuildList();
			//PlayerModel_UpdateGrid();
			break;
	}
}

/*
=================
PlayerModel_MenuKey
=================
*/
static sfxHandle_t PlayerModel_MenuKey( int key )
{
	//menucommon_s*	m;
	//int				picnum;

	switch (key)
	{
		/*case K_KP_LEFTARROW:
		case K_LEFTARROW:
			m = Menu_ItemAtCursor(&s_playermodel.menu);
			picnum = m->id - ID_PLAYERPIC0;
			if (picnum >= 0 && picnum <= 15)
			{
				if (picnum > 0)
				{
					Menu_SetCursor(&s_playermodel.menu,s_playermodel.menu.cursor-1);
					return (menu_move_sound);
					
				}
				else if (s_playermodel.modelpage > 0)
				{
					s_playermodel.modelpage--;
					Menu_SetCursor(&s_playermodel.menu,s_playermodel.menu.cursor+15);
					PlayerModel_UpdateGrid();
					return (menu_move_sound);
				}
				else
					return (menu_buzz_sound);
			}
			break;

		case K_KP_RIGHTARROW:
		case K_RIGHTARROW:
			m = Menu_ItemAtCursor(&s_playermodel.menu);
			picnum = m->id - ID_PLAYERPIC0;
			if (picnum >= 0 && picnum <= 15)
			{
				if ((picnum < 15) && (s_playermodel.modelpage*MAX_MODELSPERPAGE + picnum+1 < s_playermodel.nummodels))
				{
					Menu_SetCursor(&s_playermodel.menu,s_playermodel.menu.cursor+1);
					return (menu_move_sound);
				}					
				else if ((picnum == 15) && (s_playermodel.modelpage < s_playermodel.numpages-1))
				{
					s_playermodel.modelpage++;
					Menu_SetCursor(&s_playermodel.menu,s_playermodel.menu.cursor-15);
					PlayerModel_UpdateGrid();
					return (menu_move_sound);
				}
				else
					return (menu_buzz_sound);
			}
			break;*/
			
		case K_MOUSE2:
		case K_ESCAPE:
			PlayerModel_SaveChanges();
			break;
	}

	return ( Menu_DefaultKey( &s_playermodel.menu, key ) );
}

/*
=================
PlayerModel_PicEvent
=================
*/
/*static void PlayerModel_PicEvent( void* ptr, int event )
{
	int				modelnum;
	int				maxlen;
	char*			buffptr;
	char*			pdest;
	int				i;

	if (event != QM_ACTIVATED)
		return;

	for (i=0; i<PLAYERGRID_ROWS*PLAYERGRID_COLS; i++)
	{
		// reset
 		s_playermodel.pics[i].generic.flags       &= ~QMF_HIGHLIGHT;
 		s_playermodel.picbuttons[i].generic.flags |= QMF_PULSEIFFOCUS;
	}

	// set selected
	i = ((menucommon_s*)ptr)->id - ID_PLAYERPIC0;
	s_playermodel.pics[i].generic.flags       |= QMF_HIGHLIGHT;
	s_playermodel.picbuttons[i].generic.flags &= ~QMF_PULSEIFFOCUS;

	// get model and strip icon_
	modelnum = s_playermodel.modelpage*MAX_MODELSPERPAGE + i;
	buffptr  = s_playermodel.modelnames[modelnum] + strlen("models/players2/");
	pdest    = strstr(buffptr,"icon_");
	if (pdest)
	{
		// track the whole model/skin name
		Q_strncpyz(s_playermodel.modelData,buffptr,pdest-buffptr+1);
		strcat(s_playermodel.modelData,pdest + 5);

		// seperate the model name
		maxlen = pdest-buffptr;
		if (maxlen > 16)
			maxlen = 16;
		Q_strncpyz( s_playermodel.modelname.string, buffptr, maxlen );
		Q_strupr( s_playermodel.modelname.string );

		// seperate the skin name
		maxlen = strlen(pdest+5)+1;
		if (maxlen > 16)
			maxlen = 16;
		Q_strncpyz( s_playermodel.skinname.string, pdest+5, maxlen );
		Q_strupr( s_playermodel.skinname.string );

		s_playermodel.selectedmodel = modelnum;

		// kef -- make sure something like "chell/nelson" doesn't occur
		if (Q_stricmp( s_playermodel.skinname.string, "red") &&
			Q_stricmp( s_playermodel.skinname.string, "blue") &&
			Q_stricmp( s_playermodel.skinname.string, "black") &&
			Q_stricmp( s_playermodel.skinname.string, "default"))
		{
			// assume something like "chell/nelson" has occurred
			Q_strncpyz( s_playermodel.modelname.string, s_playermodel.skinname.string, strlen(s_playermodel.skinname.string)+1 );
			Q_strncpyz( s_playermodel.skinname.string, "DEFAULT", 8 );
		}

		// Kind of a hack to display the externalized text
		if (!Q_stricmp( s_playermodel.skinname.string, "red"))
		{
			Q_strncpyz( s_playermodel.skinnameviewed.string, menu_normal_text[MNT_RED], strlen(menu_normal_text[MNT_RED])+1 );
		}	
		else if (!Q_stricmp( s_playermodel.skinname.string, "blue")) 
		{
			Q_strncpyz( s_playermodel.skinnameviewed.string, menu_normal_text[MNT_BLUE], strlen(menu_normal_text[MNT_BLUE])+1 );
		}
		else if (!Q_stricmp( s_playermodel.skinname.string, "default"))
		{
			Q_strncpyz( s_playermodel.skinnameviewed.string, menu_normal_text[MNT_DEFAULT], strlen(menu_normal_text[MNT_DEFAULT])+1 );
		}

		if( trap_MemoryRemaining() > LOW_MEMORY ) {
			PlayerModel_UpdateModel();
		}
	}
}*/

/*
=================
PlayerModel_DrawPlayer
=================
*/
static void PlayerModel_DrawPlayer( void *self )
{
	menubitmap_s*	b;
	vec3_t			origin = {-15, 0, 0 }; //-3.8

	b = (menubitmap_s*) self;

	if( trap_MemoryRemaining() <= LOW_MEMORY ) {
		UI_DrawProportionalString( b->generic.x, b->generic.y + b->height / 2, "LOW MEMORY", UI_LEFT, color_red );
		return;
	}

	UI_DrawPlayer( b->generic.x, b->generic.y, b->width, b->height, origin, &s_playermodel.playerinfo, uis.realtime/2 );
}

/*void precacheSpecificGroups(char *race_list)
{
	char current_race_name[125];
	char *s = race_list;
	char *max_place = race_list + strlen(race_list);
	char *marker;


	memset(current_race_name, 0, sizeof(current_race_name));
	// look through the list till it's empty
	while (s < max_place)
	{
		marker = s;
		// figure out from where we are where the next ',' or 0 is
		while (*s != ',' && *s != 0)
		{
			s++;
		}

		// copy just that name
		Q_strncpyz(current_race_name, marker, s-marker+1);

		// avoid the comma or increment us past the end of the string so we fail the main while loop
		s++;

		// register	the group wins announce sound
		trap_S_RegisterSound( va( "sound/voice/computer/misc/%s_wins.wav",current_race_name  ) );

		// register the blue and red flag images
		trap_R_RegisterShaderNoMip( va( "models/flags/%s_red", current_race_name));
		trap_R_RegisterShaderNoMip( va( "models/flags/%s_blue", current_race_name));
	}
}*/

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
	int		numdirs;
	int		numfiles;
	char	dirlist[8192];
	char	filelist[2048];
	//char	skinname[64];
	char*	dirptr;
	//char*	fileptr;
	int		i;
	//int		j;
	int		dirlen;
	//int		filelen;
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

		

		//fileptr  = filelist;
		//for (j=0; j<numfiles && s_playermodel.nummodels < MAX_PLAYERMODELS;j++,fileptr+=filelen+1)
		//{
		//	filelen = strlen(fileptr);

		//	COM_StripExtension(fileptr,skinname);

		//	// look for icon_????
		//	if (!Q_stricmpn(skinname,"icon_",5))
		//	{	//inside here skinname is always "icon_*"
		//		if (!precache) {
		//			if (s_playermodel.skinfilter.curvalue == 0)
		//			{
		//				// no nelson/kenn team skins
		//				if (Q_stricmp(skinname+5 ,"default"))
		//				{
		//					continue;
		//				}
		//				// No red team skins
		//				if (!Q_stricmp(skinname+5 ,"red"))
		//				{
		//					continue;
		//				}
		//				// No blue team skins
		//				if (!Q_stricmp(skinname+5 ,"blue"))
		//				{
		//					continue;
		//				}
		//			}
		//			
		//			if (s_playermodel.skinfilter.curvalue == 1)
		//			{
		//				// Only blue team skins
		//				if (Q_stricmp(skinname+5 ,"blue"))
		//				{
		//					continue;
		//				}
		//			}
		//			
		//			if (s_playermodel.skinfilter.curvalue == 2)
		//			{
		//				// Only blue team skins
		//				if (Q_stricmp(skinname+5 ,"red"))
		//				{
		//					continue;
		//				}
		//			}
		//		}
		//		//Com_sprintf( s_playermodel.modelnames[s_playermodel.nummodels++],
		//			//sizeof( s_playermodel.modelnames[s_playermodel.nummodels] ),
		//			//"models/players2/%s/%s", dirptr, skinname );
		//		
		//		/*if( precache ) {	//per skin type inside a dir
		//			if( Q_stricmp( skinname+5, "default" ) == 0 ) {	//+5 to skip past "icon_"
		//				continue;
		//			}
		//			if( Q_stricmp( skinname+5, "red" ) == 0 ) {
		//				continue;
		//			}
		//			if( Q_stricmp( skinname+5, "blue" ) == 0 ) {
		//				continue;
		//			}
		//			trap_S_RegisterSound( va( "sound/voice/computer/misc/%s_wins.wav",skinname+5 ) );
		//		}*/
		//	}
		//}

		/*if( precache ) {	//per modelname (dir)
			trap_S_RegisterSound( va( "sound/voice/computer/misc/%s_wins.wav",dirptr ) );
			trap_S_RegisterSound( va( "sound/voice/computer/misc/%s.wav", dirptr ) );
			precacheSpecificGroups( BG_RegisterRace(va("models/players2/%s/groups.cfg", dirptr)));
		}*/
	}	

	/*s_playermodel.numpages = s_playermodel.nummodels/MAX_MODELSPERPAGE;
	if (s_playermodel.nummodels % MAX_MODELSPERPAGE)
		s_playermodel.numpages++;*/
}

/*
=================
PlayerModel_SetMenuItems
=================
*/
static void PlayerModel_SetMenuItems( void )
{
	int				i;
	//int				maxlen;
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
		int len;
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

		// strip icon_
		/*buffptr  = s_playermodel.charnames[i] + strlen("models/players2/");
		pdest    = strstr(buffptr,"icon_");
		if (pdest)
		{
			Q_strncpyz(modelskin,buffptr,pdest-buffptr+1);
			strcat(modelskin,pdest + 5);
		}
		else
			continue;

		if (!Q_stricmp( s_playermodel.modelData, modelskin ))
		{
			// found pic, set selection here		
			s_playermodel.selectedmodel = i;
			s_playermodel.modelpage     = i/MAX_MODELSPERPAGE;

			// seperate the model name
			maxlen = pdest-buffptr;
			if (maxlen > 16)
				maxlen = 16;
			Q_strncpyz( s_playermodel.modelname.string, buffptr, maxlen );
			Q_strupr( s_playermodel.modelname.string );

			// seperate the skin name
			maxlen = strlen(pdest+5)+1;
			if (maxlen > 16)
				maxlen = 16;
			Q_strncpyz( s_playermodel.skinname.string, pdest+5, maxlen );
			Q_strupr( s_playermodel.skinname.string );

			// kef -- make sure something like "chell/nelson" doesn't occur
			if (Q_stricmp( s_playermodel.skinname.string, "red") &&
				Q_stricmp( s_playermodel.skinname.string, "blue") &&
				Q_stricmp( s_playermodel.skinname.string, "default"))
			{
				// assume something like "chell/nelson" has occurred
				Q_strncpyz( s_playermodel.modelname.string, s_playermodel.skinname.string, strlen(s_playermodel.skinname.string)+1 );
				Q_strncpyz( s_playermodel.skinname.string, "DEFAULT", 8 );
			}

			break;
		}*/
	}

	//try to register the current shader icon
	s_playermodel.playerIcon = trap_R_RegisterShaderNoMip( va( "models/players_rpgx/%s/model_icon", s_playermodel.modelName ) );

	//Huh... guess whatever the hell's in the console, we don't have it on our list
	if ( s_playermodel.selectedChar == -1 ) {	
		return;
	}

	//If we're exceeded the list, update it so we're at the top
	if ( s_playermodel.selectedChar > MAX_MENULISTITEMS ) {
		s_playermodel.scrollOffset = s_playermodel.selectedChar - MAX_MENULISTITEMS;
		PlayerModel_OffsetCharList( &s_playermodel.scrollOffset );
	}

	//Set the model and skin data
	/*{
		char* buf[24];

		s_playermodel.charModel.numitems = PlayerModel_LoadAvailableModels( buf );
		s_playermodel.charModel.itemnames = buf;

		s_playermodel.charSkin.numitems = PlayerModel_LoadAvailableSkins( buf );
		s_playermodel.charSkin.itemnames = buf;
	}*/
}

/*
=================
PlayerSettingsMenu_Graphics
=================
*/
void PlayerModelMenu_Graphics (void)
{
	// Draw the basic screen layout
	UI_MenuFrame2(&s_playermodel.menu);

	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30,203, 47, 186, uis.whiteShader);	// Middle left line

	// Frame around model pictures
	trap_R_SetColor( colorTable[CT_LTORANGE]);
	//UI_DrawHandlePic(  114,  50,   8,  -32, s_playermodel.corner_ll_4_18);	// UL Corner
	//UI_DrawHandlePic(  114, 355,   8,  32, s_playermodel.corner_ll_4_18);	// LL Corner
	//UI_DrawHandlePic(  411,  50,   8,  -32, s_playermodel.corner_lr_4_18);	// UR Corner
	//UI_DrawHandlePic(  411, 355,   8,  32, s_playermodel.corner_lr_4_18);	// LR Corner
	//UI_DrawHandlePic(  114,  81,   4, 284, uis.whiteShader);	// Left side
	//UI_DrawHandlePic(  414,  81,   4, 284, uis.whiteShader);	// Right side
	//UI_DrawHandlePic(  120,  62, 293,  18, uis.whiteShader);	// Top
	//UI_DrawHandlePic(  120, 357, 293,  18, uis.whiteShader);	// Bottom

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


}

/*
=================
PlayerSettings_MenuDraw
=================
*/
static void PlayerModel_MenuDraw (void)
{
	PlayerModelMenu_Graphics();

	Menu_Draw( &s_playermodel.menu );
}
/*
=================
PlayerModel_MenuInit
=================
*/
static void PlayerModel_MenuInit(int menuFrom)
{
	int			i;
	//int			j;
	//int			k;
	int			x;
	int			y;
	static char	playername[32];
	static char	modelname[32];
	static char	skinname[32];
	static char	skinnameviewed[32];

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

	/*for (i=0,k=0; i<PLAYERGRID_ROWS; i++)
	{
		x =	129;
		for (j=0; j<PLAYERGRID_COLS; j++,k++)
		{
			s_playermodel.pics[k].generic.type	   = MTYPE_BITMAP;
			s_playermodel.pics[k].generic.flags    = QMF_LEFT_JUSTIFY|QMF_INACTIVE;
			s_playermodel.pics[k].generic.x		   = x;
			s_playermodel.pics[k].generic.y		   = y;
			s_playermodel.pics[k].width  		   = 66;
			s_playermodel.pics[k].height  		   = 66;
			s_playermodel.pics[k].focuspic         = MODEL_SELECTED;
			s_playermodel.pics[k].focuscolor       = colorTable[CT_WHITE];

			s_playermodel.picbuttons[k].generic.type	 = MTYPE_BITMAP;
			s_playermodel.picbuttons[k].generic.flags    = QMF_LEFT_JUSTIFY|QMF_NODEFAULTINIT|QMF_PULSEIFFOCUS;
			s_playermodel.picbuttons[k].generic.id	     = ID_PLAYERPIC0+k;
			s_playermodel.picbuttons[k].generic.callback = PlayerModel_PicEvent;
			s_playermodel.picbuttons[k].generic.x    	 = x - 16;
			s_playermodel.picbuttons[k].generic.y		 = y - 16;
			s_playermodel.picbuttons[k].generic.left	 = x;
			s_playermodel.picbuttons[k].generic.top		 = y;
			s_playermodel.picbuttons[k].generic.right	 = x + 64;
			s_playermodel.picbuttons[k].generic.bottom   = y + 64;
			s_playermodel.picbuttons[k].width  		     = 128;
			s_playermodel.picbuttons[k].height  		 = 128;
			s_playermodel.picbuttons[k].focuspic  		 = MODEL_SELECT;
			s_playermodel.picbuttons[k].focuscolor  	 = colorTable[CT_WHITE];

			x += 64+6;
		}
		y += 64+8;
	}*/

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

	/*s_playermodel.skinnameviewed.generic.type			= MTYPE_PTEXT;
	s_playermodel.skinnameviewed.generic.flags			= QMF_INACTIVE;
	s_playermodel.skinnameviewed.generic.x				= 323;
	s_playermodel.skinnameviewed.generic.y				= 338;
	s_playermodel.skinnameviewed.string					= skinnameviewed;
	s_playermodel.skinnameviewed.style					= UI_RIGHT;
	s_playermodel.skinnameviewed.color					= colorTable[CT_LTBLUE1];*/

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

	/*s_playermodel.skinfilter.generic.type			= MTYPE_SPINCONTROL;
	s_playermodel.skinfilter.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_playermodel.skinfilter.generic.x				= 190;
	s_playermodel.skinfilter.generic.y				= 311;
	s_playermodel.skinfilter.generic.id 			= ID_SKINFILTER;
	s_playermodel.skinfilter.generic.callback		= PlayerModel_MenuEvent;
	s_playermodel.skinfilter.textEnum				= MBT_GROUPFILTER;
	s_playermodel.skinfilter.textcolor				= CT_BLACK;
	s_playermodel.skinfilter.textcolor2				= CT_WHITE;
	s_playermodel.skinfilter.color					= CT_DKPURPLE1;
	s_playermodel.skinfilter.color2					= CT_LTPURPLE1;
	s_playermodel.skinfilter.textX					= MENU_BUTTON_TEXT_X;
	s_playermodel.skinfilter.textY					= MENU_BUTTON_TEXT_Y;
	s_playermodel.skinfilter.listnames				= s_SkinFilter_Names;*/

	Menu_AddItem( &s_playermodel.menu,	&s_playermodel.model );
	Menu_AddItem( &s_playermodel.menu,	&s_playermodel.data );
	Menu_AddItem( &s_playermodel.menu,	&s_playermodel.player );
	Menu_AddItem( &s_playermodel.menu,	&s_playermodel.playername );
	Menu_AddItem( &s_playermodel.menu,	&s_playermodel.modelname );
	//Menu_AddItem( &s_playermodel.menu,	&s_playermodel.skinfilter );

	/*for (i=0; i<MAX_MODELSPERPAGE; i++)
	{
		Menu_AddItem( &s_playermodel.menu,	&s_playermodel.pics[i] );
		Menu_AddItem( &s_playermodel.menu,	&s_playermodel.picbuttons[i] );
	}*/

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
	//PlayerModel_UpdateGrid();
	PlayerModel_UpdateModel();
}

/*
=================
PlayerModel_Cache
=================
*/
void PlayerModel_Cache( void )
{
	//int	i;

	s_playermodel.corner_ll_4_4		= trap_R_RegisterShaderNoMip("menu/common/corner_ll_4_4");
	s_playermodel.corner_ll_4_18	= trap_R_RegisterShaderNoMip("menu/common/corner_ll_4_18");
	s_playermodel.corner_lr_4_18	= trap_R_RegisterShaderNoMip("menu/common/corner_lr_4_18");
	s_playermodel.corner_lr_18_4	= trap_R_RegisterShaderNoMip("menu/common/corner_lr_18_4");
	s_playermodel.corner_ur_18_18	= trap_R_RegisterShaderNoMip("menu/common/corner_ur_18_18");

	trap_R_RegisterShaderNoMip(PIC_ARROW_UP);
	trap_R_RegisterShaderNoMip(PIC_ARROW_DOWN);

	/*for( i = 0; playermodel_artlist[i]; i++ ) 
	{
		trap_R_RegisterShaderNoMip( playermodel_artlist[i] );
	}*/
	
	//PlayerModel_BuildList();

	/*for( i = 0; i < s_playermodel.nummodels; i++ ) 
	{
		trap_R_RegisterShaderNoMip( s_playermodel.modelnames[i] );
	}*/
}

/*
=================
PlayerModel_Cache
=================
*/
void UI_PlayerModelMenu(int menuFrom)
{
	PlayerModel_MenuInit(menuFrom);

	UI_PushMenu( &s_playermodel.menu );

	//Menu_SetCursorToItem( &s_playermodel.menu, &s_playermodel.pics[s_playermodel.selectedmodel % MAX_MODELSPERPAGE] );
}


