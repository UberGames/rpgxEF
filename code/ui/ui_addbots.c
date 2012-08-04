/*
 * Copyright (C) 1999-2000 Id Software, Inc.
 */
/*
=======================================================================

ADD BOTS MENU

=======================================================================
*/
#include "ui_local.h"

#define BOTS_VIEWABLE		11

#define ID_BACK				10
#define ID_GO				11
#define ID_LIST				12
#define ID_UP				13
#define ID_DOWN				14
#define ID_SKILL			15
#define ID_TEAM				16
#define ID_BOTNAME0			20
#define ID_BOTNAME1			21
#define ID_BOTNAME2			22
#define ID_BOTNAME3			23
#define ID_BOTNAME4			24
#define ID_BOTNAME5			25
#define ID_BOTNAME6			26
#define ID_PCLASS			27


typedef struct {
	menuframework_s	menu;
	menubitmap_s	arrows;
	menubitmap_s	up;
	menubitmap_s	down;
	menutext_s		bots[BOTS_VIEWABLE];
	menulist_s		skill;
	menulist_s		team;
	menulist_s		pclass;
	menubitmap_s	go;
	menubitmap_s	back;

	int				numBots;
	int				delay;
	int				baseBotNum;
	int				selectedBotNum;
	int				sortedBotNums[MAX_BOTS];
	char			botnames[BOTS_VIEWABLE][32];
} addBotsMenuInfo_t;

static addBotsMenuInfo_t	addBotsMenuInfo;


/*
=================
UI_AddBotsMenu_BotEvent
=================
*/
static void UI_AddBotsMenu_BotEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}

	addBotsMenuInfo.bots[addBotsMenuInfo.selectedBotNum].color = colorTable[CT_DKGOLD1];
	addBotsMenuInfo.selectedBotNum = ((menucommon_s*)ptr)->id - ID_BOTNAME0;
	addBotsMenuInfo.bots[addBotsMenuInfo.selectedBotNum].color = colorTable[CT_YELLOW];
}


/*
=================
UI_AddBotsMenu_BackEvent
=================
*/
static void UI_AddBotsMenu_BackEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}
	UI_PopMenu();
}


/*
=================
UI_AddBotsMenu_SetBotNames
=================
*/
static void UI_AddBotsMenu_SetBotNames( void ) {
	int			n;
	const char	*info;

	for ( n = 0; n < BOTS_VIEWABLE; n++ ) 
	{
		info = UI_GetBotInfoByNumber( addBotsMenuInfo.sortedBotNums[addBotsMenuInfo.baseBotNum + n] );
		Q_strncpyz( addBotsMenuInfo.botnames[n], Info_ValueForKey( info, "name" ), sizeof(addBotsMenuInfo.botnames[n]) );
	}

}


/*
=================
UI_AddBotsMenu_UpEvent
=================
*/
static void UI_AddBotsMenu_UpEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}

	if( addBotsMenuInfo.baseBotNum > 0 ) {
		addBotsMenuInfo.baseBotNum--;
		UI_AddBotsMenu_SetBotNames();
	}
}


/*
=================
UI_AddBotsMenu_DownEvent
=================
*/
static void UI_AddBotsMenu_DownEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}

	if( addBotsMenuInfo.baseBotNum + BOTS_VIEWABLE < addBotsMenuInfo.numBots ) {
		addBotsMenuInfo.baseBotNum++;
		UI_AddBotsMenu_SetBotNames();
	}
}


/*
=================
UI_AddBotsMenu_GetSortedBotNums
=================
*/
static int QDECL UI_AddBotsMenu_SortCompare( const void *arg1, const void *arg2 ) {
	int			num1, num2;
	const char	*info1, *info2;
	const char	*name1, *name2;

	num1 = *(int *)arg1;
	num2 = *(int *)arg2;

	info1 = UI_GetBotInfoByNumber( num1 );
	info2 = UI_GetBotInfoByNumber( num2 );

	name1 = Info_ValueForKey( info1, "name" );
	name2 = Info_ValueForKey( info2, "name" );

	return Q_stricmp( name1, name2 );
}

static void UI_AddBotsMenu_GetSortedBotNums( void ) {
	int		n;

	/* initialize the array */
	for( n = 0; n < addBotsMenuInfo.numBots; n++ ) {
		addBotsMenuInfo.sortedBotNums[n] = n;
	}

	qsort( addBotsMenuInfo.sortedBotNums, addBotsMenuInfo.numBots, sizeof(addBotsMenuInfo.sortedBotNums[0]), UI_AddBotsMenu_SortCompare );
}


/*
=================
UI_AddBotsMenu_Draw
=================
*/
static void UI_AddBotsMenu_Draw( void ) 
{
	UI_MenuFrame(&addBotsMenuInfo.menu);

	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(30,201,  47, 90, uis.whiteShader);	/* Left side of frame */
	UI_DrawHandlePic(30,293,  47, 97, uis.whiteShader);

	/* Bot name frame */
	trap_R_SetColor( colorTable[CT_DKBLUE1]);
	UI_DrawHandlePic( 81, 172,  225,  18, uis.whiteShader);	/* Top  */
	UI_DrawHandlePic(288, 189,   18, 246, uis.whiteShader);	/* Side */
	UI_DrawProportionalString( 90, 173, menu_normal_text[MNT_HOLOGRAPHIC_CHARS],UI_SMALLFONT,colorTable[CT_BLACK]);	/* Top */

	/* Bot settings frame */
	trap_R_SetColor( colorTable[CT_DKBLUE1]);
	UI_DrawHandlePic( 316, 172,  295,  18, uis.whiteShader);	/* Top    */
	UI_DrawHandlePic( 316, 185,   18,  94, uis.whiteShader);	/* Side   */
	UI_DrawHandlePic( 316, 282,  295,  18, uis.whiteShader);	/* Bottom */
	UI_DrawProportionalString( 340, 173, menu_normal_text[MNT_SETTINGS],UI_SMALLFONT,colorTable[CT_BLACK]);	/* Top */

	/* standard menu drawing */
	Menu_Draw( &addBotsMenuInfo.menu );
}

	
/*
=================
UI_AddBotsMenu_Init
=================
*/
static int skillNames[] =
{
MNT_BABYLEVEL,
MNT_EASYLEVEL,
MNT_MEDIUMLEVEL,
MNT_HARDLEVEL,
MNT_NIGHTMARELEVEL,
0
};

static int teamNames1[] = 
{
	MNT_FREE,
	0
};

static int teamNames2[] = 
{
	MNT_TEAM_RED,
	MNT_TEAM_BLUE,
	0
};

static char* Game_teamNames1[] = 
{
	"free",
};

static char* Game_teamNames2[] = 
{
	"red",
	"blue",
};

static int pClassNames[] = 
{
	MNT_PC_INFILTRATOR,
	MNT_PC_SNIPER,
	MNT_PC_HEAVY,
	MNT_PC_DEMO,
	MNT_PC_MEDIC,
	MNT_PC_TECH,
	0
};

static int pClassNames2[] = 
{
	MNT_PC_NOCLASS,
	0
};

static char* Game_pClassNames[] = 
{
	"infiltrator",
	"sniper",
	"heavy",
	"demo",
	"medic",
	"tech"
};

static char* Game_pClassNames2[] = 
{
	"noclass"
};

/*
=================
UI_AddBotsMenu_FightEvent
=================
*/
static void UI_AddBotsMenu_FightEvent( void* ptr, int event ) {
	const char	*team;
	const char	*pclass;
	int			skill;

	if (event != QM_ACTIVATED) {
		return;
	}

	/*team = menu_normal_text[addBotsMenuInfo.team.listnames[addBotsMenuInfo.team.curvalue]];
	pclass = menu_normal_text[addBotsMenuInfo.pclass.listnames[addBotsMenuInfo.pclass.curvalue]];*/
	/* can't translate these, game code is in english only */
	if (addBotsMenuInfo.team.listnames == teamNames1) {
		team = Game_teamNames1[addBotsMenuInfo.team.curvalue];
	} else {
		team = Game_teamNames2[addBotsMenuInfo.team.curvalue];
	}
	if (addBotsMenuInfo.pclass.listnames == pClassNames) {
		pclass = Game_pClassNames[addBotsMenuInfo.pclass.curvalue];
	} else {
		pclass = Game_pClassNames2[addBotsMenuInfo.pclass.curvalue];
	}

	skill = addBotsMenuInfo.skill.curvalue + 1;

	trap_Cmd_ExecuteText( EXEC_APPEND, va("addbot %s %i %s %s %i\n",
		addBotsMenuInfo.botnames[addBotsMenuInfo.selectedBotNum], skill, team, pclass, addBotsMenuInfo.delay) );

	addBotsMenuInfo.delay += 1500;
}


static void UI_AddBotsMenu_Init( void ) 
{
	int		n;
	int		y,x;
	int		gametype, specialties;
	int		count;
	char	info[MAX_INFO_STRING];

	trap_GetConfigString(CS_SERVERINFO, info, MAX_INFO_STRING);   
	gametype = atoi( Info_ValueForKey( info,"g_gametype" ) );
	specialties = atoi( Info_ValueForKey( info,"g_pModSpecialties" ) );

	memset( &addBotsMenuInfo, 0 ,sizeof(addBotsMenuInfo) );
	addBotsMenuInfo.menu.draw = UI_AddBotsMenu_Draw;
	addBotsMenuInfo.menu.fullscreen = qtrue;
	addBotsMenuInfo.menu.wrapAround = qtrue;
	addBotsMenuInfo.delay = 1000;
	addBotsMenuInfo.menu.descX				= MENU_DESC_X;
	addBotsMenuInfo.menu.descY				= MENU_DESC_Y;
	addBotsMenuInfo.menu.titleX				= MENU_TITLE_X;
	addBotsMenuInfo.menu.titleY				= MENU_TITLE_Y;
	addBotsMenuInfo.menu.titleI				= MNT_ADD_HCS_TITLE;
	addBotsMenuInfo.menu.footNoteEnum			= MNT_ADD_HCS;

	UI_AddBots_Cache();

	addBotsMenuInfo.numBots = UI_GetNumBots();
	count = addBotsMenuInfo.numBots < BOTS_VIEWABLE ? addBotsMenuInfo.numBots : BOTS_VIEWABLE;

	addBotsMenuInfo.up.generic.type				= MTYPE_BITMAP;
	addBotsMenuInfo.up.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	addBotsMenuInfo.up.generic.x				= 289;
	addBotsMenuInfo.up.generic.y				= 196;
	addBotsMenuInfo.up.generic.id				= ID_UP;
	addBotsMenuInfo.up.generic.callback			= UI_AddBotsMenu_UpEvent;
	addBotsMenuInfo.up.width  				= 16;
	addBotsMenuInfo.up.height  				= 16;
	addBotsMenuInfo.up.color  				= CT_DKGOLD1;
	addBotsMenuInfo.up.color2  				= CT_LTGOLD1;
	addBotsMenuInfo.up.generic.name				= "menu/common/arrow_up_16.tga";


	addBotsMenuInfo.down.generic.type			= MTYPE_BITMAP;
	addBotsMenuInfo.down.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	addBotsMenuInfo.down.generic.x				= 289;
	addBotsMenuInfo.down.generic.y				= 407;
	addBotsMenuInfo.down.generic.id				= ID_DOWN;
	addBotsMenuInfo.down.generic.callback			= UI_AddBotsMenu_DownEvent;
	addBotsMenuInfo.down.width  				= 16;
	addBotsMenuInfo.down.height  				= 16;
	addBotsMenuInfo.down.color  				= CT_DKGOLD1;
	addBotsMenuInfo.down.color2  				= CT_LTGOLD1;
	addBotsMenuInfo.down.generic.name			= "menu/common/arrow_dn_16.tga";

	for( n = 0, y = 194; n < count; n++, y += 20 ) 
	{
		addBotsMenuInfo.bots[n].generic.type		= MTYPE_PTEXT;
		addBotsMenuInfo.bots[n].generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
		addBotsMenuInfo.bots[n].generic.id		= ID_BOTNAME0 + n;
		addBotsMenuInfo.bots[n].generic.x		= 108;
		addBotsMenuInfo.bots[n].generic.y		= y;
		addBotsMenuInfo.bots[n].generic.callback	= UI_AddBotsMenu_BotEvent;
		addBotsMenuInfo.bots[n].string			= addBotsMenuInfo.botnames[n];
		addBotsMenuInfo.bots[n].color			= colorTable[CT_DKGOLD1];
		addBotsMenuInfo.bots[n].color2			= colorTable[CT_LTGOLD1];
		addBotsMenuInfo.bots[n].style			= UI_LEFT|UI_SMALLFONT;
	}

	y = 204;
	x = 348;

/* team button */
	UI_InitSpinControl(&addBotsMenuInfo.team);
	addBotsMenuInfo.team.generic.x			= x;
	addBotsMenuInfo.team.generic.y			= y;
	addBotsMenuInfo.team.textEnum			= MBT_TEAM;
	addBotsMenuInfo.team.generic.id			= ID_TEAM;
	if( gametype >= GT_TEAM ) 
	{
		addBotsMenuInfo.team.listnames		= teamNames2;
	}
	else 
	{
		addBotsMenuInfo.team.listnames		= teamNames1;
		addBotsMenuInfo.team.generic.flags	= QMF_GRAYED;
	}

/* MCG ADDED:   */
/* class button */
	y += SMALLCHAR_HEIGHT + 8;
	UI_InitSpinControl(&addBotsMenuInfo.pclass);
	addBotsMenuInfo.pclass.generic.x		= x;
	addBotsMenuInfo.pclass.generic.y		= y;
	addBotsMenuInfo.pclass.textEnum			= MBT_PLAYERCLASS;
	addBotsMenuInfo.pclass.generic.id		= ID_PCLASS;
	if ( specialties )
	{
		addBotsMenuInfo.pclass.listnames	= pClassNames;
	}
	else
	{
		addBotsMenuInfo.pclass.listnames	= pClassNames2;
		addBotsMenuInfo.pclass.generic.flags	= QMF_GRAYED;
	}
/* MCG END */

/* skill button */
	y += SMALLCHAR_HEIGHT + 8;
	UI_InitSpinControl(&addBotsMenuInfo.skill);
	addBotsMenuInfo.skill.generic.x			= x;
	addBotsMenuInfo.skill.generic.y			= y;
	addBotsMenuInfo.skill.generic.id		= ID_SKILL;
	addBotsMenuInfo.skill.curvalue			= Com_Clamp( 0, 4, (int)trap_Cvar_VariableValue( "g_spSkill" ) - 1 );
	addBotsMenuInfo.skill.textEnum			= MBT_SKILL;
	/*addBotsMenuInfo.skill.generic.id		= ID_TEAM;*/
	addBotsMenuInfo.skill.listnames			= skillNames;

/* go button */
	addBotsMenuInfo.go.generic.type			= MTYPE_BITMAP;
	addBotsMenuInfo.go.generic.name			= GRAPHIC_SQUARE;
	addBotsMenuInfo.go.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	addBotsMenuInfo.go.generic.id			= ID_GO;
	addBotsMenuInfo.go.generic.callback		= UI_AddBotsMenu_FightEvent;
	addBotsMenuInfo.go.generic.x			= 356;
	addBotsMenuInfo.go.generic.y			= 342;
	addBotsMenuInfo.go.width  			= MENU_BUTTON_MED_WIDTH;
	addBotsMenuInfo.go.height  			= MENU_BUTTON_MED_HEIGHT * 2;
	addBotsMenuInfo.go.color			= CT_DKPURPLE1;
	addBotsMenuInfo.go.color2			= CT_LTPURPLE1;
	addBotsMenuInfo.go.textX			= MENU_BUTTON_TEXT_X;
	addBotsMenuInfo.go.textY			= MENU_BUTTON_TEXT_Y;
	addBotsMenuInfo.go.textEnum			= MBT_ACCEPT;
	addBotsMenuInfo.go.textcolor			= CT_BLACK;
	addBotsMenuInfo.go.textcolor2			= CT_WHITE;

/* back button */
	addBotsMenuInfo.back.generic.type		= MTYPE_BITMAP;
	addBotsMenuInfo.back.generic.name		= BUTTON_GRAPHIC_LONGRIGHT;
	addBotsMenuInfo.back.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	addBotsMenuInfo.back.generic.id			= ID_BACK;
	addBotsMenuInfo.back.generic.callback		= UI_AddBotsMenu_BackEvent;
	addBotsMenuInfo.back.generic.x			= 482;
	addBotsMenuInfo.back.generic.y			= 136;
	addBotsMenuInfo.back.width  			= MENU_BUTTON_MED_WIDTH;
	addBotsMenuInfo.back.height  			= MENU_BUTTON_MED_HEIGHT;
	addBotsMenuInfo.back.color			= CT_DKPURPLE1;
	addBotsMenuInfo.back.color2			= CT_LTPURPLE1;
	addBotsMenuInfo.back.textX			= MENU_BUTTON_TEXT_X;
	addBotsMenuInfo.back.textY			= MENU_BUTTON_TEXT_Y;
	addBotsMenuInfo.back.textEnum			= MBT_INGAMEMENU;
	addBotsMenuInfo.back.textcolor			= CT_BLACK;
	addBotsMenuInfo.back.textcolor2			= CT_WHITE;


	addBotsMenuInfo.baseBotNum = 0;
	addBotsMenuInfo.selectedBotNum = 0;
	/*addBotsMenuInfo.bots[0].color = color_white;*/

	UI_AddBotsMenu_GetSortedBotNums();
	UI_AddBotsMenu_SetBotNames();

	Menu_AddItem( &addBotsMenuInfo.menu, &addBotsMenuInfo.up );
	Menu_AddItem( &addBotsMenuInfo.menu, &addBotsMenuInfo.down );
	for( n = 0; n < count; n++ ) 
	{
		Menu_AddItem( &addBotsMenuInfo.menu, &addBotsMenuInfo.bots[n] );
	}
	Menu_AddItem( &addBotsMenuInfo.menu, &addBotsMenuInfo.skill );
	Menu_AddItem( &addBotsMenuInfo.menu, &addBotsMenuInfo.team );
	Menu_AddItem( &addBotsMenuInfo.menu, &addBotsMenuInfo.pclass );
	Menu_AddItem( &addBotsMenuInfo.menu, &addBotsMenuInfo.go );
	Menu_AddItem( &addBotsMenuInfo.menu, &addBotsMenuInfo.back );
}


/*
=================
UI_AddBots_Cache
=================
*/
void UI_AddBots_Cache( void ) 
{
	trap_R_RegisterShaderNoMip( "menu/common/arrow_up_16.tga" );
	trap_R_RegisterShaderNoMip( "menu/common/arrow_dn_16.tga" );
}


/*
=================
UI_AddBotsMenu
=================
*/
void UI_AddBotsMenu( void ) 
{
	UI_AddBotsMenu_Init();
	UI_PushMenu( &addBotsMenuInfo.menu );
}

