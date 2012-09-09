// Copyright (C) 1999-2000 Id Software, Inc.
//
// cg_info.c -- display information while data is being loading

#include "cg_local.h"
#include "cg_text.h"

#define MAX_LOADING_PLAYER_ICONS	16
#define MAX_LOADING_ITEM_ICONS		26

static qhandle_t	loadingPlayerIcon;
static qhandle_t	loadingItemIcon;

void CG_LoadBar(void);

/*
===================
CG_DrawLoadingIcons
===================
*/
static void CG_DrawLoadingIcons( void ) {
//	int		n;
	int		x, y;

	trap_R_SetColor( colorTable[CT_WHITE]);

	x= 500;
	y = 342;
	if (loadingPlayerIcon)
	{
		CG_DrawPic( x, y, 64, 64, loadingPlayerIcon );
	}
	else if (loadingItemIcon)
	{
		trap_R_SetColor(colorTable[CT_LTPURPLE1]);
		CG_DrawPic( x, y, 64, 64, cgs.media.weaponbox2 );
		trap_R_SetColor( NULL);
		CG_DrawPic( x, y, 64, 64, loadingItemIcon );
	}
}


/*
======================
CG_LoadingString

======================
*/
void CG_LoadingString( const char *s ) {
	Q_strncpyz( cg.infoScreenText, s, sizeof( cg.infoScreenText ) );

	trap_UpdateScreen();
}

/*
===================
CG_LoadingItem
===================
*/
void CG_LoadingItem( int itemNum ) {
	gitem_t		*item;

	item = &bg_itemlist[itemNum];

	if ( item->icon ) {
		loadingItemIcon = trap_R_RegisterShaderNoMip( item->icon );
	}

	CG_LoadingString( item->pickup_name );
}

/*
===================
CG_LoadingClient
===================
*/
void CG_LoadingClient( int clientNum ) {
	const char		*info;
	char			*skin;
	char			personality[MAX_QPATH];
	char			model[MAX_QPATH];
	char			iconName[MAX_QPATH];

	info = CG_ConfigString( CS_PLAYERS + clientNum );

	Q_strncpyz( model, Info_ValueForKey( info, "model" ), sizeof( model ) );
	skin = strchr( model, '/' );
	if ( skin ) {
		//*skin++ = '\0';
		//} else {
		//	skin = "default";
		model[strlen(model) - strlen(skin)] = '\0';
	}

	//RPG-X: MODEL SYSTEM CHANGE
	Com_sprintf( iconName, MAX_QPATH, "models/players_rpgx/%s/model_icon.jpg", model );

	//Com_sprintf( iconName, MAX_QPATH, "models/players/%s/icon_%s.jpg", model, skin );
	
	//CG_Printf( S_COLOR_RED "Loading %s\n", iconName );
	loadingPlayerIcon = trap_R_RegisterShaderNoMip( iconName ); //iconName;

	Q_strncpyz( personality, Info_ValueForKey( info, "n" ), sizeof(personality) );
	Q_CleanStr( personality );

	if( cgs.gametype == GT_SINGLE_PLAYER ) {
		trap_S_RegisterSound( va( "sound/voice/computer/misc/%s.wav", model ) );	//not exactly right since it'll miss subskins, but better than using personality
	}//precache sound played in g_bot.c, PlayerIntroSound

	CG_LoadingString( personality );
 	
}


/*
====================
CG_DrawInformation

Draw all the status / pacifier stuff during level loading
this overlays the ui version in ui_connect.c, UI_DrawConnectScreen
====================
*/
extern void CG_AddGameModNameToGameName( char *gamename );
void CG_DrawInformation( void ) {
	const char	*s;
	const char	*info;
	const char	*sysInfo;
	int			y,x;
//	int			value;
	qhandle_t	levelshot;
//	qhandle_t	detail;
	char		buf[1024];
	int			strlength,length;

	//trap_Cvar_Set ("rpg_playIntro", "1");

	info = CG_ConfigString( CS_SERVERINFO );
	sysInfo = CG_ConfigString( CS_SYSTEMINFO );

	s = Info_ValueForKey( info, "mapname" );
	levelshot = trap_R_RegisterShaderNoMip( va( "levelshots/%s.tga", s ) );
	if ( !levelshot ) {
		levelshot = trap_R_RegisterShaderNoMip( "levelshots/unknownmap" );
	}

	cgs.widescreen.state = WIDESCREEN_NONE;

	trap_R_SetColor( colorTable[CT_BLACK] );
	CG_DrawPic( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, cgs.media.whiteShader );
	
	cgs.widescreen.state = WIDESCREEN_CENTER;

	trap_R_SetColor( colorTable[CT_DKGREY] );
	CG_DrawPic( 11, 60, 260, 196, cgs.media.whiteShader );

	trap_R_SetColor( NULL );
	CG_DrawPic( 13, 62, 256, 192, levelshot );	//correct aspect

//	trap_R_SetColor( colorTable[CT_LTGREY] );
//	CG_DrawPic( 418, 82, 132, 132, cgs.media.whiteShader );

//	trap_R_SetColor( NULL );
//	CG_DrawPic( 420, 84, 128, 128, levelshot );

	// blend a detail texture over it
//	detail = trap_R_RegisterShader( "levelShotDetail" );
//	trap_R_DrawStretchPic( 0, 0, cgs.glconfig.vidWidth, cgs.glconfig.vidHeight, 0, 0, 1, 1, detail );

	UI_DrawProportionalString( 10, 10, ingame_text[IGT_HOLODECKSIMULATION], UI_BIGFONT, colorTable[CT_LTORANGE] );


	strlength = UI_ProportionalStringWidth(ingame_text[IGT_HOLODECKSIMULATION],UI_BIGFONT);
	length = 582 - (strlength + 6);

	trap_R_SetColor( colorTable[CT_DKORANGE]);
	CG_DrawPic( 10 + strlength + 6, 11, length,  22,cgs.media.whiteShader);
//	CG_DrawPic( 224, 11, 368,  22,cgs.media.whiteShader);
	CG_DrawPic( 595, 11,  32, 32,cgs.media.halfroundr_22);		// Right End
	
	trap_R_SetColor( colorTable[CT_DKPURPLE1]);

	CG_DrawPic(  274+333, 232, -32,  32,cgs.media.corner_12_18);		// LR
	CG_DrawPic(  274+333,  84, -32, -32,cgs.media.corner_12_18);		// UR

	CG_DrawPic(  274,		60,		314,	 18,cgs.media.whiteShader);	// Top
	CG_DrawPic(  274,		238,	314,	 18,cgs.media.whiteShader);	//Bottom
	CG_DrawPic(  274,		75,		10,		170,cgs.media.whiteShader);	// Left
	CG_DrawPic(  274+321,	78,		12,		162,cgs.media.whiteShader);	// Right

	CG_LoadBar();

	// draw the icons of thiings as they are loaded
	CG_DrawLoadingIcons();

	// the first 150 rows are reserved for the client connection
	// screen to write into
	if ( cg.infoScreenText[0] ) {
		UI_DrawProportionalString( 320, 442, va("%s ... %s", ingame_text[IGT_LOADING], cg.infoScreenText),
			UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, colorTable[CT_LTGOLD1]);
	} else {
		UI_DrawProportionalString( 320, 442, va("%s...", ingame_text[IGT_SNAPSHOT]),
			UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, colorTable[CT_LTGOLD1]  );
	}

	// draw info string information

	y = 107;
	x = 288;
	// don't print server lines if playing a local game
	trap_Cvar_VariableStringBuffer( "sv_running", buf, sizeof( buf ) );
	if ( !atoi( buf ) ) {
		// server hostname
		Q_strncpyz(buf, Info_ValueForKey( info, "sv_hostname" ), 1024);
		Q_CleanStr(buf);
		UI_DrawProportionalString( x, y, buf, UI_SMALLFONT|UI_DROPSHADOW, colorTable[CT_VLTGOLD1] );
		y += PROP_HEIGHT;

		// pure server
		s = Info_ValueForKey( sysInfo, "sv_pure" );
		if ( s[0] == '1' ) {
			UI_DrawProportionalString( x, y, ingame_text[IGT_PURESERVER],
				UI_SMALLFONT|UI_DROPSHADOW, colorTable[CT_VLTGOLD1] );
			y += PROP_HEIGHT;
		}

		// server-specific message of the day
		s = CG_ConfigString( CS_MOTD );
		if ( s[0] ) {
			UI_DrawProportionalString(320, y, s,
				UI_SMALLFONT|UI_DROPSHADOW, colorTable[CT_VLTGOLD1] );
			y += PROP_HEIGHT;
		}

		// some extra space after hostname and motd
		y += 10;
	}

	// map-specific message (long map name)
	s = CG_ConfigString( CS_MESSAGE );
	if ( s[0] ) {
		UI_DrawProportionalString( x, y, s,
			UI_SMALLFONT|UI_DROPSHADOW, colorTable[CT_VLTGOLD1] );
		y += PROP_HEIGHT;
	}

	// cheats warning
	s = Info_ValueForKey( sysInfo, "sv_cheats" );
	if ( s[0] == '1' ) {
		UI_DrawProportionalString( x, y, ingame_text[IGT_CHEATSAREENABLED],
			UI_SMALLFONT|UI_DROPSHADOW, colorTable[CT_VLTGOLD1] );
		y += PROP_HEIGHT;
	}

	// RPG-X | Marcin | 24/12/2008
	// privacy thing :p
	// translate later
	s = Info_ValueForKey( info, "rpg_respectPrivacy" );
	if ( atoi( s ) != 0 ) {
		UI_DrawProportionalString( x, y, "PRIVACY MODE ^5ON",
			UI_SMALLFONT|UI_DROPSHADOW, colorTable[CT_VLTGOLD1] );
		y += PROP_HEIGHT;
	} else {
		UI_DrawProportionalString( x, y, "PRIVACY MODE ^1OFF",
			UI_SMALLFONT|UI_DROPSHADOW, colorTable[CT_VLTGOLD1] );
		y += PROP_HEIGHT;
	}

	// game type
	switch ( cgs.gametype ) 
	{
	case GT_FFA:
		s = ingame_text[IGT_GAME_FREEFORALL];
		break;
	case GT_SINGLE_PLAYER:
		s = ingame_text[IGT_GAME_SINGLEPLAYER];
		break;
	case GT_TOURNAMENT:
		s = ingame_text[IGT_GAME_TOURNAMENT];
		break;
	case GT_TEAM:
		s = ingame_text[IGT_GAME_TEAMHOLOMATCH];
		break;
	case GT_CTF:
		s = ingame_text[IGT_GAME_CAPTUREFLAG];
		break;
	default:
		s = ingame_text[IGT_GAME_UNKNOWN];
		break;
	}

	{
		char	gamename[1024];

		Q_strncpyz( gamename, s, sizeof(gamename) );

		CG_AddGameModNameToGameName( gamename );

		UI_DrawProportionalString( x, y, gamename, UI_SMALLFONT|UI_DROPSHADOW, colorTable[CT_VLTGOLD1] );
	}

	y += PROP_HEIGHT;
		
/*	value = atoi( Info_ValueForKey( info, "timelimit" ) );
	if ( value ) {
		UI_DrawProportionalString( x, y, va( "%s %i",ingame_text[IGT_TIME_LIMIT], value ),
			UI_SMALLFONT|UI_DROPSHADOW, colorTable[CT_VLTGOLD1] );
		y += PROP_HEIGHT;
	}

	if (cgs.gametype != GT_CTF) {
		value = atoi( Info_ValueForKey( info, "fraglimit" ) );
		if ( value ) {
			UI_DrawProportionalString( x, y, va( "%s %i", ingame_text[IGT_POINT_LIMIT],value ),
				UI_SMALLFONT|UI_DROPSHADOW, colorTable[CT_VLTGOLD1] );
			y += PROP_HEIGHT;
		}
	}

	if (cgs.gametype == GT_CTF) {
		value = atoi( Info_ValueForKey( info, "capturelimit" ) );
		if ( value ) {
			UI_DrawProportionalString( x, y, va( "%s %i",ingame_text[IGT_CAPTURE_LIMIT], value ),
				UI_SMALLFONT|UI_DROPSHADOW, colorTable[CT_VLTGOLD1] );
			y += PROP_HEIGHT;
		}
	}*/

	cgs.widescreen.state = WIDESCREEN_NONE;
}

/*
====================
CG_LoadBar
====================
*/
void CG_LoadBar(void)
{
	int			x,y,pad;

	// Round LCARS buttons
	y = 309;
	x = 10;
	pad = 22;
	// First Bit (0987)
	if (cg.loadLCARSStage < 1)
	{
		trap_R_SetColor( colorTable[CT_VDKBROWN1]); //PURPLE3]);
	}
	else
	{
		trap_R_SetColor( colorTable[CT_VLTGOLD1]);
		CG_DrawPic( x + 222 - 20,y + 14, 16,  16,cgs.media.circle2);
		UI_DrawProportionalString( x + 222, y + 14, ingame_text[IGT_REPLICATION_MATRIX],UI_SMALLFONT, colorTable[CT_VLTGOLD1]);
		trap_R_SetColor( colorTable[CT_LTBROWN1]); //VLTPURPLE3]);
	}
	CG_DrawPic( x + 18,   y +102, 128,  64,cgs.media.loading1);


	if (cg.loadLCARSStage < 2)
	{
		trap_R_SetColor( colorTable[CT_VDKBLUE1]);
	}
	else
	{
		trap_R_SetColor( colorTable[CT_VLTGOLD1]);
		CG_DrawPic( x + 222 - 20,y + 14, 16,  16,cgs.media.circle);
		trap_R_SetColor( colorTable[CT_LTBLUE1]);
	}
	CG_DrawPic(      x,   y + 37,  64,  64,cgs.media.loading2);


	if (cg.loadLCARSStage < 3)
	{
		trap_R_SetColor( colorTable[CT_DKGOLD1]); //VDKPURPLE1]);
	}
	else
	{
		trap_R_SetColor( colorTable[CT_VLTGOLD1]);
		CG_DrawPic( x + 222 - 20,y + 14+pad, 16,  16,cgs.media.circle2);
		UI_DrawProportionalString( x + 222, y + 14 + pad, ingame_text[IGT_HOLOGRAPHIC_PROJECTORS],UI_SMALLFONT, colorTable[CT_VLTGOLD1]);
		trap_R_SetColor( colorTable[CT_LTGOLD1]); //LTPURPLE1]);
	}
	CG_DrawPic( x + 17,        y, 128,  64,cgs.media.loading3);


	if (cg.loadLCARSStage < 4)
	{
		trap_R_SetColor( colorTable[CT_VDKBLUE1]); //VDKPURPLE2]);
	}
	else
	{
		trap_R_SetColor( colorTable[CT_VLTGOLD1]);
		CG_DrawPic( x + 222 - 20,y + 14+pad, 16,  16,cgs.media.circle);
		trap_R_SetColor( colorTable[CT_LTBLUE1]); //PURPLE2]);
	}
	CG_DrawPic( x + 99,        y, 128, 128,cgs.media.loading4);


	if (cg.loadLCARSStage < 5)
	{
		trap_R_SetColor( colorTable[CT_VDKBROWN1]); //BLUE2]);
	}
	else
	{
		trap_R_SetColor( colorTable[CT_VLTGOLD1]);
		CG_DrawPic( x + 222 - 20,y + 14+pad+pad, 16,  16,cgs.media.circle2);
		UI_DrawProportionalString( x + 222, y + 14 + pad + pad, ingame_text[IGT_SIMULATION_DATA_BASE],UI_SMALLFONT, colorTable[CT_VLTGOLD1]);
		trap_R_SetColor( colorTable[CT_LTBROWN1]); //VLTBLUE2]);
	}
	CG_DrawPic( x +137,   y + 81,  64,  64,cgs.media.loading5);


	if (cg.loadLCARSStage < 6)
	{
		trap_R_SetColor( colorTable[CT_VDKRED1]); //ORANGE]);
	}
	else
	{
		trap_R_SetColor( colorTable[CT_VLTGOLD1]);
		CG_DrawPic( x + 222 - 20,y + 14+pad+pad, 16,  16,cgs.media.circle);
		trap_R_SetColor( colorTable[CT_DKRED1]); //LTORANGE]);
	}
	CG_DrawPic( x + 45,   y + 99, 128,  64,cgs.media.loading6);


	if (cg.loadLCARSStage < 7)
	{
		trap_R_SetColor( colorTable[CT_VDKRED1]); //BLUE2]);
	}
	else
	{
		trap_R_SetColor( colorTable[CT_VLTGOLD1]);
		CG_DrawPic( x + 222 - 20,y + 14+pad+pad+pad, 16,  16,cgs.media.circle2);
		UI_DrawProportionalString( x + 222, y + 14 + pad + pad + pad, ingame_text[IGT_SAFETY_LOCKS],UI_SMALLFONT, colorTable[CT_VLTGOLD1]);
		trap_R_SetColor( colorTable[CT_DKRED1]); //LTBLUE2]);
	}
	CG_DrawPic( x + 38,   y + 24,  64, 128,cgs.media.loading7);

	if (cg.loadLCARSStage < 8)
	{
		trap_R_SetColor( colorTable[CT_VDKBROWN1]); //PURPLE1]);
	}
	else
	{
		trap_R_SetColor( colorTable[CT_VLTGOLD1]);
		CG_DrawPic( x + 222 - 20,y + 14+pad+pad+pad, 16,  16,cgs.media.circle);
		trap_R_SetColor( colorTable[CT_LTBROWN1]); //PURPLE1]);
	}
	CG_DrawPic( x + 78,   y + 20, 128,  64,cgs.media.loading8);

	if (cg.loadLCARSStage < 9)
	{
		trap_R_SetColor( colorTable[CT_VDKBLUE2]); //VDKBROWN1]);
	}
	else
	{
		trap_R_SetColor( colorTable[CT_DKBLUE2]); //VLTBROWN1]);
	}
	CG_DrawPic( x +112,   y + 66,  64, 128,cgs.media.loading9);


	if (cg.loadLCARSStage < 9)
	{
		trap_R_SetColor( colorTable[CT_VDKORANGE]); //DKBLUE2]);
	}
	else
	{
		trap_R_SetColor( colorTable[CT_LTORANGE]); //LTBLUE2]);
	}
	CG_DrawPic( x + 62,   y + 44, 128, 128,cgs.media.loadingcircle);	// Center arrows

	cg.loadLCARScnt++;
	if (cg.loadLCARScnt > 3)
	{
		cg.loadLCARScnt = 0;
	}

	trap_R_SetColor( colorTable[CT_VDKBLUE1]); //DKPURPLE2]);
	CG_DrawPic( x +  61,   y + 43,  32,  32,cgs.media.loadingquarter);	// Quad UL
	CG_DrawPic( x + 135,   y + 43, -32,  32,cgs.media.loadingquarter);	// Quad UR
	CG_DrawPic( x + 135,   y +117, -32, -32,cgs.media.loadingquarter);	// Quad LR
	CG_DrawPic( x +  61,   y +117,  32, -32,cgs.media.loadingquarter);	// Quad LL

	trap_R_SetColor( colorTable[CT_LTBLUE1]); //LTPURPLE2]);
	switch (cg.loadLCARScnt)
	{
	case 0 :
		CG_DrawPic( x +  61,   y + 43,  32,  32,cgs.media.loadingquarter);	// Quad UL
		break;
	case 1 :
		CG_DrawPic( x + 135,   y + 43, -32,  32,cgs.media.loadingquarter);	// Quad UR
		break;
	case 2 :
		CG_DrawPic( x + 135,   y +117, -32, -32,cgs.media.loadingquarter);	// Quad LR
		break;
	case 3 :
		CG_DrawPic( x +  61,   y +117,  32, -32,cgs.media.loadingquarter);	// Quad LL
		break;
	} 

	UI_DrawProportionalString( x +  21, y + 150, "0987",UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString( x +   3, y +  90,   "18",UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString( x +  24, y +  20,    "7",UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString( x +  93, y +   5,   "51",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString( x + 103, y +   5,   "35",UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString( x + 165, y +  83,   "21",UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString( x + 101, y + 149,   "67",UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString( x + 123, y +  36,   "8",UI_TINYFONT, colorTable[CT_BLACK]);

	UI_DrawProportionalString( x +  90, y +  65, "1",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString( x + 105, y +  65, "2",UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString( x + 105, y +  87, "3",UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString( x +  91, y +  87, "4",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	trap_R_SetColor( colorTable[CT_DKGOLD1]); //DKBROWN1]);
	y +=10;
	CG_DrawPic( x + 130, y - 10 ,  64, 16,cgs.media.loadingtrim);	
	CG_DrawPic( x +  130, y + 150,  64, -16,cgs.media.loadingtrim);	

	CG_DrawPic( x +  150, y - 10,   432,  8, cgs.media.whiteShader);		// Top line
	CG_DrawPic( x +  150, y + 142, 432,  8, cgs.media.whiteShader);		// Bottom line
	CG_DrawPic( x +  583, y - 7,      16, 151, cgs.media.whiteShader);	// Side line

	CG_DrawPic( x +  580, y + 1,      32, -16,cgs.media.loadingcorner);	
	CG_DrawPic( x +  580, y + 139,    32, 16,cgs.media.loadingcorner);	

}

static int missionYcnt;
static int missionYpos;
static int	missionInfoScreenY;

#define	OBJ_HORIZONTAL_BORDER_X		15	// Where graphic starts
#define	OBJ_HORIZONTAL_BORDER_WIDTH	30	// Thickness of graphic
#define	OBJ_TEXT_X_BORDER_LEFT		10	// Distance from right edge of graphic to circle graphic
#define	OBJ_TEXT_X_BORDER_RIGHT		10	// Distance from right edge of text to right edge of screen
#define	OBJ_CIRCLE_SIZE				16	// Size of circle graphic
#define	OBJ_CIRCLE_TEXT_MARGIN		4	// Distance between circle and text
#define	OBJ_TEXT_XPOS			(OBJ_HORIZONTAL_BORDER_X + OBJ_HORIZONTAL_BORDER_WIDTH + OBJ_TEXT_X_BORDER_LEFT)
#define	OBJ_SCREEN_HEIGHT		428
#define	OBJ_SCREEN_YMARGIN		8
#define	OBJ_SCREEN_Y2MARGIN		4
#define	OBJ_SCREEN_YBORDERTOP	20
#define	OBJ_SCREEN_YBORDERBOT	8
#define	OBJ_NORMAL_LINE_HEIGHT	(PROP_HEIGHT * 1.15)
#define	OBJ_ADDITIONAL_LINE_HEIGHT	.20



/*
====================
ObjectivePrint_Line
====================
*/
static void ObjectivePrint_Line(int strIndex,int color,centity_t *cent)
{
	char *str,*strBegin;
	int y,pixelLen,charLen;
	char holdText[1024], holdText2[2];
	char finalText[MAX_OBJ_LENGTH];
	int len, maxPixLength, charHeight;

	assert(cgs.objectives[strIndex].text);

	str =  cgs.objectives[strIndex].text;

	len = strlen(str);
	len++;
	Q_strncpyz(finalText,str,len);

	pixelLen = UI_ProportionalStringWidth(finalText,UI_SMALLFONT);

	str = finalText;
	maxPixLength = SCREEN_WIDTH - (OBJ_TEXT_XPOS + OBJ_TEXT_X_BORDER_RIGHT + OBJ_CIRCLE_SIZE + OBJ_CIRCLE_TEXT_MARGIN);
	charHeight = OBJ_NORMAL_LINE_HEIGHT;

	if (missionYcnt)	// Not the very first objective to be printed?
	{
		missionYpos += (PROP_HEIGHT * 	OBJ_ADDITIONAL_LINE_HEIGHT); // Add a little space between objective lines
	}

	y =missionYpos + (charHeight * (missionYcnt));
	trap_R_SetColor( colorTable[color]);

	if (cgs.objectives[strIndex].complete)
	{
		CG_DrawPic( OBJ_TEXT_XPOS,y, OBJ_CIRCLE_SIZE,  OBJ_CIRCLE_SIZE,cgs.media.circle);
	}
	else
	{
		CG_DrawPic( OBJ_TEXT_XPOS,y, OBJ_CIRCLE_SIZE,  OBJ_CIRCLE_SIZE,cgs.media.circle2);
	}

	if (pixelLen < maxPixLength)	// One shot - small enough to print entirely on one line
	{
		UI_DrawProportionalString(OBJ_TEXT_XPOS + OBJ_CIRCLE_SIZE + OBJ_CIRCLE_TEXT_MARGIN, y,str, UI_SMALLFONT, colorTable[color] );
		++missionYcnt;
	}
	// Text is too long, break into lines.
	else
	{
		pixelLen = 0;
		charLen = 0;
		holdText2[1] = '\0';
		strBegin = str;

		while( *str ) 
		{
			holdText2[0] = *str;
			pixelLen += UI_ProportionalStringWidth(holdText2,UI_SMALLFONT);
			pixelLen += 2; // For kerning
			++charLen;

			if (pixelLen > maxPixLength ) 
			{	//Reached max length of this line
				//step back until we find a space
				while ((charLen) && (*str != ' ' ))
				{
					--str;
					--charLen;
				}

				if (*str==' ')
				{
					++str;	// To get past space
				}

				Q_strncpyz( holdText, strBegin, charLen);
				holdText[charLen] = '\0';
				strBegin = str;
				pixelLen = 0;
				charLen = 1;

				y = missionYpos + (charHeight * missionYcnt);

				UI_DrawProportionalString(OBJ_TEXT_XPOS + OBJ_CIRCLE_SIZE + OBJ_CIRCLE_TEXT_MARGIN, y, holdText, UI_SMALLFONT, colorTable[color] );
				++missionYcnt;
			} 
			else if (*(str+1) == '\0')
			{
				++charLen;

				y = missionYpos + (charHeight * missionYcnt);
				Q_strncpyz( holdText, strBegin, charLen);
				UI_DrawProportionalString(OBJ_TEXT_XPOS + OBJ_CIRCLE_SIZE + OBJ_CIRCLE_TEXT_MARGIN, y, holdText, UI_SMALLFONT, colorTable[color] );
				++missionYcnt;
				break;
			}
			++str; 
		} 
	}
}

static int Objective_LineCnt(int strIndex,centity_t *cent)
{
	char *str,*strBegin;
	int pixelLen,charLen;
	char holdText[1024], holdText2[2];
	char finalText[MAX_OBJ_LENGTH];
	int len, maxPixLength;
	int	lineCnt;

	assert(cgs.objectives[strIndex].text);
	str =  cgs.objectives[strIndex].text;

	len = strlen(str);
	len++;
	Q_strncpyz(finalText,str,len);

	pixelLen = UI_ProportionalStringWidth(finalText,UI_SMALLFONT);
	lineCnt = 0;

	maxPixLength = SCREEN_WIDTH - (OBJ_TEXT_XPOS + OBJ_TEXT_X_BORDER_RIGHT + OBJ_CIRCLE_SIZE + OBJ_CIRCLE_TEXT_MARGIN);

	str = finalText;

	if (pixelLen < maxPixLength)	// One shot - small enough to print entirely on one line
	{
		lineCnt = 1;
	}
	// Text is too long, break into lines.
	else
	{
		pixelLen = 0;
		charLen = 0;
		holdText2[1] = '\0';
		strBegin = str;

		while( *str ) 
		{
			holdText2[0] = *str;
			pixelLen += UI_ProportionalStringWidth(holdText2,UI_SMALLFONT);
			pixelLen += 2; // For kerning
			++charLen;

			if (pixelLen > maxPixLength ) 
			{	//Reached max length of this line
				//step back until we find a space
				while ((charLen) && (*str != ' ' ))
				{
					--str;
					--charLen;
				}

				if (*str==' ')
				{
					++str;	// To get past space
				}

				Q_strncpyz( holdText, strBegin, charLen);
				holdText[charLen] = '\0';
				strBegin = str;
				pixelLen = 0;
				charLen = 1;

				lineCnt++;
			} 
			else if (*(str+1) == '\0')
			{
				++charLen;
				lineCnt++;
				break;
			}
			++str; 
		} 
	}
	return (lineCnt);
}


/*
====================
Objectives_Draw
====================
*/
static void Objectives_Draw( centity_t *cent )
{
	int objCnt,i,lineCnt,maxLines;
	int	total,textYCnt,length,color;
	vec4_t newColor; 

	objCnt=0;
	for (i=0;i<MAX_OBJECTIVES;i++)
	{
		if (cgs.objectives[i].text[0])
		{
			objCnt++;
		}
	}

	lineCnt = 0;
	// Count the number of lines.
	for (i=0;i<objCnt;i++)
	{
		if (cgs.objectives[i].text[0])
		{
			lineCnt += Objective_LineCnt(i,cent);
		}
	}

	maxLines = OBJ_SCREEN_HEIGHT /OBJ_NORMAL_LINE_HEIGHT;
	if (lineCnt > maxLines)	// Too many lines?
	{
		lineCnt = maxLines;
	}

	if (lineCnt==0)	// Show there are no objectives
	{
		Q_strncpyz(cgs.objectives[0].text,ingame_text[IGT_NONETEXT],sizeof(cgs.objectives[0].text));
	}

	textYCnt = lineCnt * OBJ_NORMAL_LINE_HEIGHT;

	// For the space between objectives
	textYCnt +=	(objCnt-1) * (OBJ_ADDITIONAL_LINE_HEIGHT * PROP_HEIGHT);

	// Calc starting Y of text
	total = OBJ_SCREEN_YMARGIN + OBJ_SCREEN_YMARGIN + OBJ_SCREEN_Y2MARGIN + OBJ_SCREEN_Y2MARGIN +
		OBJ_SCREEN_YBORDERTOP + OBJ_SCREEN_YBORDERBOT + textYCnt;

	if (OBJ_SCREEN_HEIGHT < total)	// This should never happen (but just in case)
	{
		total = OBJ_SCREEN_HEIGHT;
	}

	missionInfoScreenY = ((OBJ_SCREEN_HEIGHT - total) /2) + (OBJ_SCREEN_YMARGIN + OBJ_SCREEN_YBORDERTOP);

	missionYcnt = 0;
	missionYpos = missionInfoScreenY;

	// Print top of frame
	trap_R_SetColor( colorTable[CT_DKPURPLE3]);
	CG_DrawPic( OBJ_HORIZONTAL_BORDER_X + 10,   missionInfoScreenY - (OBJ_SCREEN_YMARGIN + OBJ_SCREEN_YBORDERTOP) ,   
		SCREEN_WIDTH - (OBJ_HORIZONTAL_BORDER_X +  OBJ_TEXT_X_BORDER_RIGHT + 10), OBJ_SCREEN_YBORDERTOP, cgs.media.whiteShader);	// Middle column

	// Print bottom of frame
	CG_DrawPic( OBJ_HORIZONTAL_BORDER_X + 10,   missionInfoScreenY - OBJ_SCREEN_YMARGIN + textYCnt + (2 * OBJ_SCREEN_YMARGIN),   
		SCREEN_WIDTH - (OBJ_HORIZONTAL_BORDER_X +  OBJ_TEXT_X_BORDER_RIGHT + 10), OBJ_SCREEN_YBORDERBOT, cgs.media.whiteShader);	// Middle column

	length = (missionInfoScreenY - OBJ_SCREEN_YMARGIN + textYCnt + (2 * OBJ_SCREEN_YMARGIN)) - (missionInfoScreenY - (OBJ_SCREEN_YMARGIN + OBJ_SCREEN_YBORDERTOP)) - 15; 

	// Print left hand column of frame
	CG_DrawPic( OBJ_HORIZONTAL_BORDER_X,  (missionInfoScreenY - (OBJ_SCREEN_YMARGIN + OBJ_SCREEN_YBORDERTOP)) + 10,   
		OBJ_HORIZONTAL_BORDER_WIDTH, length, cgs.media.whiteShader);	// Middle column

	// Top corner
	trap_R_SetColor( colorTable[CT_DKPURPLE3]);
	CG_DrawPic( OBJ_HORIZONTAL_BORDER_X,  
		missionInfoScreenY - (OBJ_SCREEN_YMARGIN + OBJ_SCREEN_YBORDERTOP),   
		32, 32, cgs.media.corner_ul_20_30);	// Top corner

	// Bottom corner
	CG_DrawPic( OBJ_HORIZONTAL_BORDER_X,  
		(missionInfoScreenY - OBJ_SCREEN_YMARGIN + textYCnt + (2 * OBJ_SCREEN_YMARGIN))-5,   
		32, 32, cgs.media.corner_ll_8_30);	// Bottom corner

	UI_DrawProportionalString( OBJ_HORIZONTAL_BORDER_X + 30, missionInfoScreenY - (OBJ_SCREEN_YMARGIN + OBJ_SCREEN_YBORDERTOP) + 2, ingame_text[IGT_OBJECTIVES],UI_SMALLFONT, colorTable[CT_BLACK]);


	// Print the background
	newColor[0] = colorTable[CT_BLACK][0];
	newColor[1] = colorTable[CT_BLACK][1];
	newColor[2] = colorTable[CT_BLACK][2];
	newColor[3] = 0.5;
	trap_R_SetColor(newColor);
	CG_DrawPic( (OBJ_TEXT_XPOS - OBJ_TEXT_X_BORDER_LEFT), missionInfoScreenY - OBJ_SCREEN_YMARGIN, SCREEN_WIDTH - ((OBJ_TEXT_XPOS - OBJ_TEXT_X_BORDER_LEFT)+OBJ_TEXT_X_BORDER_RIGHT)  , textYCnt + (2 * OBJ_SCREEN_YMARGIN), cgs.media.whiteShader);	

	// Print the lines
	for (i=0;i<objCnt;i++)
	{
		if (cgs.objectives[i].text[0])
		{
			if (cgs.objectives[i].complete)
			{
				color = CT_DKGOLD1;
			}
			else
			{
				color = CT_VLTGOLD1;
			}

			ObjectivePrint_Line(i,color,cent);
		}
	}
}

/*
====================
CG_DrawObjectiveInformation
====================
*/
void CG_DrawObjectiveInformation( void ) 
{
	centity_t *cent;

	// Don't show if dead
	if (cg.predictedPlayerState.pm_type == PM_DEAD)
	{
		return;
	}

	cent = &cg_entities[cg.snap->ps.clientNum];

	Objectives_Draw(cent);
}

