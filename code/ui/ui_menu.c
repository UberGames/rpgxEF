// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=======================================================================

MAIN MENU

=======================================================================
*/


#include "ui_local.h"

extern void UI_SetupWeaponsMenu( void );

void UI_QuitMenu(void);
static void QuitMenuInterrupt(int labelId);
//static void QuitMenu_ChangeAreaFocus(int newSystem);

static void UI_HolomatchIn_Menu(void);

#define ID_TEXTLANGUAGE		102
#define ID_VOICELANGUAGE	103
#define ID_KEYBOARDLANGUAGE	104
#define UI_NUM_WEAPONS		9

static const char *s_keyboardlanguage_Names[] =
{
	"AMERICAN",
	"DEUTSCH",
	"FRANCAIS",
	"ESPANOL",
	"ITALIANO",
	0
};

static struct 
{
	menuframework_s		menu;

	menulist_s			textlanguage;
	menulist_s			voicelanguage;
	menulist_s			keyboardlanguage;

	menuslider_s		gamma;
	qhandle_t			test;

	menuaction_s		apply;
} s_initialsetup;


#define PIC_PHASER_STRIP		"menu/voyager/phaser_strip.tga"
#define PIC_PHOTON_LAUNCHER		"menu/voyager/photon_launch.tga"
#define PIC_BOTTOM_STRIP		"menu/voyager/bottom_strip.tga"
#define PIC_MID_HULL			"menu/voyager/mid_hull.tga"
#define PIC_WARPNAC				"menu/voyager/warpnac.tga"
#define	PIC_BUSSARD				"menu/voyager/bussard.tga"
#define PIC_RCS					"menu/voyager/rcs.tga"
#define	PIC_BRIDGE				"menu/voyager/bridge.tga"
#define	PIC_VOYAGER				"menu/special/voy_1.tga"


#define ID_MULTIPLAYER			10
#define ID_SETUP				11
#define ID_DEMOS				12
#define ID_CINEMATICS			13
#define ID_MODS					14
#define ID_CREDITS				15
#define ID_QUIT					16

#define ID_FAV0					100
#define ID_FAV1					101
#define ID_FAV2					102
#define ID_FAV3					103
#define ID_FAV4					104
#define ID_FAV5					105
#define ID_FAV6					106
#define ID_FAV7					107
#define ID_FAV8					108

#define ID_PLAYER_SETTINGS		17
#define	ID_PLAYER_MODEL			18
#define ID_PLAYER_REGION		19

#define	ID_LIST_UP				20
#define ID_LIST_DOWN			21

#define ID_REFRESH				22
#define ID_CONNECT				23

#define	PIC_ARROW_UP		"menu/common/arrow_up_16.tga"
#define	PIC_ARROW_DOWN		"menu/common/arrow_dn_16.tga"

/*#define ID_FAVLIST				19
#define ID_FAVREFRESH			20
#define ID_FAVSTOP				21
#define ID_FAVGO				22*/


/*#define	ID_WEAPON1				215
#define	ID_WEAPON2				216
#define	ID_WEAPON3				217
#define	ID_WEAPON4				218
#define	ID_WEAPON5				219
#define	ID_WEAPON6				220
#define	ID_WEAPON7				221
#define	ID_WEAPON8				222
#define	ID_WEAPON9				223
#define	ID_WEAPON10				224*/


// Quit Menu
#define	ID_PHASER_LABEL			301
#define	ID_TORPEDO_LABEL		302
#define	ID_VENTRAL_LABEL		303
#define	ID_MIDHULL_LABEL		304
#define	ID_BUSSARD_LABEL		305
#define	ID_NACELLES_LABEL		306
#define	ID_THRUSTERS_LABEL		307
#define	ID_Q_VOYAGER_LABEL		308
#define	ID_BRIDGE_LABEL			309

#define WEAPON_WAIT 10000

#define MAIN_LOGO_MODEL		"models/mapobjects/rpg-x/logo.MD3" //TiM : Considering how big this is on-screen here, i think we can spare the processing power to render the high-res one :D

// END RPG-X

//ui_servers2.c struct data

extern char* gamenames[];

#define MAX_PINGREQUESTS 16	//max number of servers the engine can ping in one go - hard coded I think, altho TA's is 32
#define MAX_FAVBUTTONS	 8
#define	MAX_ADDRESSLENGTH 64
#define MAX_HOSTNAMELENGTH 64
#define	MAX_MAPNAMELENGTH 30
#define MAX_LISTBOXWIDTH 67

typedef struct {
	char	adrstr[MAX_ADDRESSLENGTH];
	int		start;
} pinglist_t;

typedef struct servernode_s {
	char	adrstr[MAX_ADDRESSLENGTH];
	char	hostname[MAX_HOSTNAMELENGTH];
	char	mapname[MAX_MAPNAMELENGTH];
	int		numclients;
	int		maxclients;
	int		pingtime;
	int		gametype;
	char	gamename[12];
	int		nettype;
	int		minPing;
	int		maxPing;
	qboolean	isPure;
	qboolean	isExpansion;
} servernode_t; 

typedef struct {
	//char			buff[MAX_LISTBOXWIDTH];
	servernode_t	servernode;
} table_t;

typedef struct { //Struct to hold all the network relevant parameters
	
	qboolean	refreshservers;
	int			currentping;
	int			nextpingtime;
	int			numservers;
	int			maxservers;
	
	int			refreshtime;

	pinglist_t		pinglist[MAX_PINGREQUESTS];
	table_t			table[MAX_FAVORITESERVERS];

	char		favBuffer[MAX_FAVORITESERVERS][MAX_ADDRESSLENGTH];

	qboolean	showNoServersAlert;
} network_t;

//--

typedef struct 
{
	menuframework_s	menu;

	qboolean		initialized;				

	menubitmap_s	demo;
	menubitmap_s	multiplayer;
	menubitmap_s	quit;
	menubitmap_s	setup;
	menubitmap_s	mods;
	menubitmap_s	mission;
	menubitmap_s	credits;
	menubitmap_s	player;

	menubitmap_s	playerSettings;
	menubitmap_s	playerModel;

	//server buttons
	menubitmap_s	upArrow;
	menubitmap_s	dnArrow;
	menubitmap_s	refresh;
	menubitmap_s	connect;

/*	menubitmap_s	favRefresh;
	menubitmap_s	favStop;
	menubitmap_s	favGo; */

/*	menubitmap_s	weapon1;
	menubitmap_s	weapon2;
	menubitmap_s	weapon3;
	menubitmap_s	weapon4;
	menubitmap_s	weapon5;
	menubitmap_s	weapon6;
	menubitmap_s	weapon7;
	menubitmap_s	weapon8;
	menubitmap_s	weapon9;*/


	/*sfxHandle_t			weaponnamesnd[UI_NUM_WEAPONS];*/
	int					timer;
	int					currentWeapon;
	playerInfo_t		playerinfo;

	vec3_t				playerViewangles;
	vec3_t				playerMoveangles;
	int					playerLegs;
	int					playerTorso;
	int					playerWeapon;
	qboolean			playerChat;
	//qhandel_t			rpgxlogo; //RPG-X HACK

	//RPG-X | Phenix | 18/11/2004 | Adding RPG-X logo
	qhandle_t		logoModel;
	int				startTime;

	qhandle_t		graphic_12_8_LU;
	qhandle_t		graphic_12_8_LL;
	qhandle_t		graphic_12_8_RU;
	qhandle_t		graphic_12_8_RL;

	qhandle_t		graphic_16_12_LU;
	qhandle_t		graphic_16_18_LL;

	qhandle_t		soundGrid;
	qhandle_t		soundWaveform;

	qhandle_t		activateSound;

	network_t		networkParms;				//struct to hold all the data we need when pinging for online fav servers
	int				favOffset;					//int offset when scrolling the list
	int				selectedFav;				//currently selected server ( as its index in the array )
	menubitmap_s	favMenu[MAX_FAVBUTTONS];	//buttons for slecting servers
	char*			favList[MAX_FAVBUTTONS];	//strings to link to the buffers

} mainmenu_t;

static mainmenu_t s_main;
static menubitmap_s			s_main_playermdl; //RPG-X HACK


//QVM - HACK!
/*const char* prank_items_formal2[] = 
{
	"Crewman",
	"Cadet 4th Class",
	"Cadet 3rd Class",
	"Cadet 2nd Class",
	"Cadet 1st Class",
	"Ensign",
	"Lieutenant J.G",
	"Lieutenant",
	"Lt. Commander",
	"Commander",
	"Captain",
	"Commodore",
	"Rear Admiral",
	"Vice Admiral",
	"Admiral",
	"Fleet Admiral",
	0
};

const char* prank_items_actual2[] =
{
	"crewman",
	"cadet1",
	"cadet2",
	"cadet3",
	"cadet4",
	"ensign",
	"ltjg",
	"lt",
	"ltcmdr",
	"cmdr",
	"capt",
	"cmmdr",
	"adm2",
	"adm3",
	"adm4",
	"adm5",
	0
};

static void* g_weapons[] =
{
	&s_main.weapon1,
	&s_main.weapon2,
	&s_main.weapon3,
	&s_main.weapon4,
	&s_main.weapon5,
	&s_main.weapon6,
	&s_main.weapon7,
	&s_main.weapon8,
	&s_main.weapon9,
	NULL,
};


static const char *weaponnamefiles [UI_NUM_WEAPONS] = 
{
	"sound/voice/computer/misc/phaser.wav",
	"sound/voice/computer/misc/comprifle.wav",
	"sound/voice/computer/misc/imod.wav",
	"sound/voice/computer/misc/scavrifle.wav",
	"sound/voice/computer/misc/stasisweap.wav",
	"sound/voice/computer/misc/cgl.wav",
	"sound/voice/computer/misc/tpd.wav",
	"sound/voice/computer/misc/pptl.wav",
	"sound/voice/computer/misc/arcwelder.wav"
};*/

// Data for LCARS out Menu
static struct 
{
	menuframework_s	menu;
	qhandle_t	logo;
	qhandle_t	halfround;
	sfxHandle_t	logoutsnd;
} s_leaving_menu;

#define ID_NO					109
#define ID_YES					110
#define ID_MAINMENU				121


typedef struct 
{
	menuframework_s	menu;
	qhandle_t	swoop1;
	qhandle_t	swoop2;
	qhandle_t	corner_ur;

	menubitmap_s	mainmenu;
	menubitmap_s	no;
	menubitmap_s	yes;

	qhandle_t		quitBanner;

	/*menutext_s		voyager_label;
	menubitmap_s	voyager_pic;
	menutext_s		thrusters_label;
	menubitmap_s	thrusters_pic;
	menutext_s		nacelles_label;
	menubitmap_s	nacelles_pic;
	menutext_s		bussard_label;
	menubitmap_s	bussard_pic;
	menutext_s		midhull_label;
	menubitmap_s	midhull_pic;
	menutext_s		ventral_label;
	menubitmap_s	ventral_pic;
	menutext_s		torpedo_label;
	menubitmap_s	torpedo_pic;
	menutext_s		phaser_label;
	menubitmap_s	phaser_pic;
	menutext_s		bridge_label;
	menubitmap_s	bridge_pic;*/

	sfxHandle_t	pingsound;
} quitmenu_t;

static quitmenu_t s_quit;


#define SYSTEM_MAXDESC 5
char systemDesc[SYSTEM_MAXDESC][512];

typedef enum 
{
	QMG_NUMBERS,
	QMG_COL1_NUM1,
	QMG_COL1_NUM2,
	QMG_COL1_NUM3,
	QMG_COL1_NUM4,
	QMG_COL1_NUM5,

	QMG_COL2_NUM1,
	QMG_COL2_NUM2,
	QMG_COL2_NUM3,
	QMG_COL2_NUM4,
	QMG_COL2_NUM5,

	QMG_COL3_NUM1,
	QMG_COL3_NUM2,
	QMG_COL3_NUM3,
	QMG_COL3_NUM4,
	QMG_COL3_NUM5,

	QMG_COL4_NUM1,
	QMG_COL4_NUM2,
	QMG_COL4_NUM3,
	QMG_COL4_NUM4,
	QMG_COL4_NUM5,

	QMG_COL5_NUM1,
	QMG_COL5_NUM2,
	QMG_COL5_NUM3,
	QMG_COL5_NUM4,
	QMG_COL5_NUM5,

	QMG_COL6_NUM1,
	QMG_COL6_NUM2,
	QMG_COL6_NUM3,
	QMG_COL6_NUM4,
	QMG_COL6_NUM5,

	QMG_ACTIVE_SYSTEM,

	QMG_DESC_START,
	QMG_BRIDGE_DESC1,
	QMG_PHASER_STRIP1_DESC1,
	QMG_TORPEDOS_DESC1,
	QMG_VENTRAL_DESC1,
	QMG_MIDHULL_DESC1,
	QMG_BUSSARD_DESC1,
	QMG_NACELLES_DESC1,
	QMG_THRUSTERS_DESC1,
	QMG_VOYAGER_DESC1,
	QMG_DESC_END,

	QMG_LABEL_START,
	QMG_BRIDGE_LABEL,
	QMG_PHASER_LABEL,
	QMG_TORPEDO_LABEL,
	QMG_VENTRAL_LABEL,
	QMG_MIDHULL_LABEL,
	QMG_BUSSARD_LABEL,
	QMG_NACELLES_LABEL,
	QMG_THRUSTERS_LABEL,
	QMG_VOYAGER_LABEL,
	QMG_LABEL_END,

	QMG_SWOOP_START,
	QMG_BRIDGE_SWOOP,
	QMG_PHASER_STRIP1_SWOOP,
	QMG_TORPEDO_SWOOP,
	QMG_VENTRAL_SWOOP,
	QMG_MIDHULL_SWOOP,
	QMG_BUSSARD_SWOOP,
	QMG_NACELLES_SWOOP,
	QMG_THRUSTERS_SWOOP,
	QMG_SWOOP_END,

	QMG_BOTTOM_BLIP,
	QMG_BOTTOM_BLIP2,
	QMG_MAX
} quitmenu_graphics_t;

menugraphics_s quitmenu_graphics[QMG_MAX] = 
{
//	type		timer	x		y		width	height	file/text		graphic,	min		max		target	inc		style						color

	{ MG_VAR,		0.0,	0,		0,		0,		0,		NULL,	0,		0,		0,		0,		0,		0,		0,							0,				NULL },	// Q{ MG_NUMBERS
	{ MG_NUMBER,	0.0,	368,	54,		16,		10,		NULL,	0,		0,		0,		9,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL1_NUM1
	{ MG_NUMBER,	0.0,	368,	68,		16,		10,		NULL,	0,		0,		0,		9,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL1_NUM2
	{ MG_NUMBER,	0.0,	368,	82,		16,		10,		NULL,	0,		0,		0,		9,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL1_NUM3
	{ MG_NUMBER,	0.0,	368,	96,		16,		10,		NULL,	0,		0,		0,		9,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL1_NUM4
	{ MG_NUMBER,	0.0,	368,	110,	16,		10,		NULL,	0,		0,		0,		9,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL1_NUM5

	{ MG_NUMBER,	0.0,	423,	54,		16,		10,		NULL,	0,		0,		0,		6,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL2_NUM1
	{ MG_NUMBER,	0.0,	423,	68,		16,		10,		NULL,	0,		0,		0,		6,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL2_NUM2
	{ MG_NUMBER,	0.0,	423,	82,		16,		10,		NULL,	0,		0,		0,		6,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL2_NUM3
	{ MG_NUMBER,	0.0,	423,	96,		16,		10,		NULL,	0,		0,		0,		6,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL2_NUM4
	{ MG_NUMBER,	0.0,	423,	110,	16,		10,		NULL,	0,		0,		0,		6,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL2_NUM5

	{ MG_NUMBER,	0.0,	463,	54,		16,		10,		NULL,	0,		0,		0,		9,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL3_NUM1
	{ MG_NUMBER,	0.0,	463,	68,		16,		10,		NULL,	0,		0,		0,		9,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL3_NUM2
	{ MG_NUMBER,	0.0,	463,	82,		16,		10,		NULL,	0,		0,		0,		9,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL3_NUM3
	{ MG_NUMBER,	0.0,	463,	96,		16,		10,		NULL,	0,		0,		0,		9,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL3_NUM4
	{ MG_NUMBER,	0.0,	463,	110,	16,		10,		NULL,	0,		0,		0,		9,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL3_NUM5

	{ MG_NUMBER,	0.0,	526,	54,		16,		10,		NULL,	0,		0,		0,		9,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL4_NUM1
	{ MG_NUMBER,	0.0,	526,	68,		16,		10,		NULL,	0,		0,		0,		9,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL4_NUM2
	{ MG_NUMBER,	0.0,	526,	82,		16,		10,		NULL,	0,		0,		0,		9,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL4_NUM3
	{ MG_NUMBER,	0.0,	526,	96,		16,		10,		NULL,	0,		0,		0,		9,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL4_NUM4
	{ MG_NUMBER,	0.0,	526,	110,	16,		10,		NULL,	0,		0,		0,		9,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL4_NUM5

	{ MG_NUMBER,	0.0,	581,	54,		16,		10,		NULL,	0,		0,		0,		2,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL5_NUM1
	{ MG_NUMBER,	0.0,	581,	68,		16,		10,		NULL,	0,		0,		0,		2,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL5_NUM2
	{ MG_NUMBER,	0.0,	581,	82,		16,		10,		NULL,	0,		0,		0,		2,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL5_NUM3
	{ MG_NUMBER,	0.0,	581,	96,		16,		10,		NULL,	0,		0,		0,		2,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL5_NUM4
	{ MG_NUMBER,	0.0,	581,	110,	16,		10,		NULL,	0,		0,		0,		2,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL5_NUM5

	{ MG_NUMBER,	0.0,	601,	54,		16,		10,		NULL,	0,		0,		0,		2,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL6_NUM1
	{ MG_NUMBER,	0.0,	601,	68,		16,		10,		NULL,	0,		0,		0,		2,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL6_NUM2
	{ MG_NUMBER,	0.0,	601,	82,		16,		10,		NULL,	0,		0,		0,		2,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL6_NUM3
	{ MG_NUMBER,	0.0,	601,	96,		16,		10,		NULL,	0,		0,		0,		2,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL6_NUM4
	{ MG_NUMBER,	0.0,	601,	110,	16,		10,		NULL,	0,		0,		0,		2,		0,		0,		UI_TINYFONT,				CT_DKPURPLE2,	NULL },	// Q{ MG_COL6_NUM5


//	type		timer	x		y		width	height	file/text						graphic,	min		max	target							inc		style						color
	{ MG_VAR,		0.0,	0,		0,		0,		0,		NULL,				0,			0,		0,		0,	0,								0,		0,							0,				NULL },	// Q{ MG_ACTIVE_SYSTEM

//	type		timer	x		y		width	height	file/text										graphic,	min		max	target							inc		style						color
	{ MG_VAR,		0.0,	0,		0,		0,		0,		NULL,				0,					0,			0,		0,	0,								0,		0,							0,				NULL },	// Q{ MG_DESC_START
	{ MG_STRING,	0.0,	385,	378,	0,		0,		NULL,		MNT_BRIDGE_DESC1,			0,			0,		0,		0,		0,		UI_TINYFONT,				CT_LTGOLD1,		NULL },	// Q{ MG_BRIDGE_DESC1
	{ MG_STRING,	0.0,	385,	378,	0,		0,		NULL,		MNT_PHASER_STRIP1_DESC1,	0,			0,		0,		0,		0,		UI_TINYFONT,				CT_LTGOLD1,		NULL },	// Q{ MG_PHASER_STRIP1_DESC1
	{ MG_STRING,	0.0,	385,	378,	0,		0,		NULL,		MNT_TORPEDOS_DESC1,			0,			0,		0,		0,		0,		UI_TINYFONT,				CT_LTGOLD1,		NULL },	// Q{ MG_TORPEDOS_DESC1
	{ MG_STRING,	0.0,	385,	378,	0,		0,		NULL,		MNT_VENTRAL_DESC1,			0,			0,		0,		0,		0,		UI_TINYFONT,				CT_LTGOLD1,		NULL },	// Q{ MG_VENTRAL_DESC1
	{ MG_STRING,	0.0,	385,	378,	0,		0,		NULL,		MNT_MIDHULL_DESC1,			0,			0,		0,		0,		0,		UI_TINYFONT,				CT_LTGOLD1,		NULL },	// Q{ MG_MIDHULL_DESC1
	{ MG_STRING,	0.0,	385,	378,	0,		0,		NULL,		MNT_BUSSARD_DESC1,			0,			0,		0,		0,		0,		UI_TINYFONT,				CT_LTGOLD1,		NULL },	// Q{ MG_BUSSARD_DESC1
	{ MG_STRING,	0.0,	385,	378,	0,		0,		NULL,		MNT_NACELLES_DESC1,			0,			0,		0,		0,		0,		UI_TINYFONT,				CT_LTGOLD1,		NULL },	// Q{ MG_NACELLES_DESC1
	{ MG_STRING,	0.0,	385,	378,	0,		0,		NULL,		MNT_THRUSTERS_DESC1,		0,			0,		0,		0,		0,		UI_TINYFONT,				CT_LTGOLD1,		NULL },	// Q{ MG_THRUSTERS_DESC1
	{ MG_STRING,	0.0,	385,	378,	0,		0,		NULL,		MNT_VOYAGER_DESC1,			0,			0,		0,		0,		0,		UI_TINYFONT,				CT_LTGOLD1,		NULL },	// Q{ MG_VOYAGER_DESC1

	{ MG_VAR,		0.0,	0,		0,		0,		0,		NULL,									0,			0,		0,		0,	0,								0,		0,							0,				NULL },	// Q{ MG_DESC_END

//	type		timer	x		y		width	height	file/text		graphic,	min	(desc)						max		target	(swoop)					inc		style						color
	{ MG_VAR,		0.0,	0,		0,		0,		0,		NULL,		0,	0,		0,								0,		0,								0,		0,							0,				NULL },								// Q{ MG_LABEL_START
//	{ MG_VAR,		0.0,	0,		0,		0,		0,		NULL,		0,	0,		Q{ MG_BRIDGE_DESC1,				0,		Q{ MG_BRIDGE_SWOOP,				0,		0,							CT_DKBROWN1,	(void *) &s_quit.bridge_pic },	// Q{ MG_BRIDGE_LABEL
//	{ MG_VAR,		0.0,	0,		0,		0,		0,		NULL,		0,	0,		Q{ MG_PHASER_STRIP1_DESC1,		0,		Q{ MG_PHASER_STRIP1_SWOOP,		0,		0,							CT_DKBROWN1,	(void *) &s_quit.phaser_pic },	// Q{ MG_PHASER_LABEL
//	{ MG_VAR,		0.0,	0,		0,		0,		0,		NULL,		0,	0,		Q{ MG_TORPEDOS_DESC1,				0,		Q{ MG_TORPEDO_SWOOP,				0,		0,							CT_DKBROWN1,	(void *) &s_quit.torpedo_pic },	// Q{ MG_TORPEDOS_LABEL
//	{ MG_VAR,		0.0,	0,		0,		0,		0,		NULL,		0,	0,		Q{ MG_VENTRAL_DESC1,				0,		Q{ MG_VENTRAL_SWOOP,				0,		0,							CT_DKBROWN1,	(void *) &s_quit.ventral_pic },	// Q{ MG_VENTRAL_LABEL
//	{ MG_VAR,		0.0,	0,		0,		0,		0,		NULL,		0,	0,		Q{ MG_MIDHULL_DESC1,				0,		Q{ MG_MIDHULL_SWOOP,				0,		0,							CT_DKBROWN1,	(void *) &s_quit.midhull_pic },	// Q{ MG_MIDHULL_LABEL
//	{ MG_VAR,		0.0,	0,		0,		0,		0,		NULL,		0,	0,		Q{ MG_BUSSARD_DESC1,				0,		Q{ MG_BUSSARD_SWOOP,				0,		0,							CT_DKBROWN1,	(void *) &s_quit.bussard_pic },	// Q{ MG_BUSSARD_LABEL
//	{ MG_VAR,		0.0,	0,		0,		0,		0,		NULL,		0,	0,		Q{ MG_NACELLES_DESC1,				0,		Q{ MG_NACELLES_SWOOP,				0,		0,							CT_DKBROWN1,	(void *) &s_quit.nacelles_pic },	// Q{ MG_NACELLES_LABEL
//	{ MG_VAR,		0.0,	0,		0,		0,		0,		NULL,		0,	0,		Q{ MG_THRUSTERS_DESC1,			0,		Q{ MG_THRUSTERS_SWOOP,			0,		0,							CT_DKBROWN1,	(void *) &s_quit.thrusters_pic },	// Q{ MG_THRUSTERS_LABEL
//	{ MG_VAR,		0.0,	0,		0,		0,		0,		NULL,		0,	0,		Q{ MG_VOYAGER_DESC1,				0,		0,								0,		0,							CT_DKBROWN1,	(void *) &s_quit.voyager_pic },	// Q{ MG_VOYAGER_LABEL
	{ MG_VAR,		0.0,	0,		0,		0,		0,		NULL,		0,	0,		0,								0,		0,								0,		0,							0,				NULL },								// Q{ MG_LABEL_END

//	{ MG_VAR,		0.0,	0,		0,		0,		0,		NULL,						0,	0,		0,		0,	0,								0,		0,							0,				NULL },	// Q{ MG_SWOOP_START
//	{ MG_GRAPHIC,	0.0,	156,	198,	93,		1,		"menu/common/square.tga",	0,	0,		0,		0,	Q{ MG_BRIDGE_DESC1,				1,		0,							CT_WHITE,		NULL },	// Q{ MG_BRIDGE_SWOOP
//	{ MG_GRAPHIC,	0.0,	155,	229,	16,		64,		"menu/voyager/swoop1.tga",	0,	0,		0,		0,	Q{ MG_PHASER_STRIP1_DESC1,		2,		0,							CT_WHITE,		NULL },	// Q{ MG_PHASER_STRIP1_SWOOP
//	{ MG_GRAPHIC,	0.0,	252,	272,	32,		32,		"menu/voyager/swoop2.tga",	0,	0,		0,		0,	Q{ MG_TORPEDOS_DESC1,				1,		0,							CT_WHITE,		NULL },	// Q{ MG_TORPEDOS_SWOOP
//	{ MG_GRAPHIC,	0.0,	318,	310,	32,		32,		"menu/voyager/swoop3.tga",	0,	0,		0,		0,	Q{ MG_VENTRAL_DESC1,				1,		0,							CT_WHITE,		NULL },	// Q{ MG_VENTRAL_SWOOP
//	{ MG_GRAPHIC,	0.0,	366,	247,	16,		128,	"menu/voyager/swoop4.tga",	0,	0,		0,		0,	Q{ MG_MIDHULL_DESC1,				1,		0,							CT_WHITE,		NULL },	// Q{ MG_MIDHULL_SWOOP
//	{ MG_GRAPHIC,	0.0,	457,	281,	64,		64,		"menu/voyager/swoop5.tga",	0,	0,		0,		0,	Q{ MG_BUSSARD_DESC1,				3,		0,							CT_WHITE,		NULL },	// Q{ MG_BUSSARD_SWOOP
//	{ MG_GRAPHIC,	0.0,	563,	181,	64,		128,	"menu/voyager/swoop6.tga",	0,	0,		0,		0,	Q{ MG_NACELLES_DESC1,				2,		0,							CT_WHITE,		NULL },	// Q{ MG_NACELLES_SWOOP
//	{ MG_GRAPHIC,	0.0,	288,	183,	64,		128,	"menu/voyager/swoop7.tga",	0,	0,		0,		0,	Q{ MG_THRUSTERS_DESC1,			1,		0,							CT_WHITE,		NULL },	// Q{ MG_THRUSTERS_SWOOP
//	{ MG_VAR,		0.0,	0,		0,		0,		0,		NULL,						0,	0,		0,		0,	0,								0,		0,							0,				NULL },	// Q{ MG_SWOOP_END

//	type		timer	x		y		width	height	file/text							graphic,	min	(desc)						max		target	(swoop)					inc		style						color
//	{ MG_GRAPHIC,	0.0,	  0,	353,	8,		8,		"menu/common/square.tga",	0,		0,		77,		550,0,								12,		UI_TINYFONT,				CT_LTPURPLE1,	NULL },	// Q{ MG_BOTTOM_BLIP
//	{ MG_GRAPHIC,	0.0,	  0,	167,	8,		8,		"menu/common/square.tga",	0,		0,		77,		550,0,								12,		UI_TINYFONT,				CT_LTPURPLE1,	NULL },	// Q{ MG_BOTTOM_BLIP2
};


/*
===================
RPG-X
Phenix
1/6/2004
===================
*/
typedef enum 
{
	AMG_MIDLEFT,
	AMG_UPPERLEFT,
	AMG_LOWERLEFT,
	AMG_UPPERCORNER,
	AMG_LOWERCORNER,
	AMG_UPPERTOP1ST,
	AMG_LOWERTOP1ST,
	AMG_UPPERTOP2ND,
	AMG_LOWERTOP2ND,
	AMG_UPPERSWOOP,
	AMG_LOWERSWOOP,
	AMG_TOPRIGHT,
	AMG_BOTTOMRIGHT,
	AMG_PLAYERBKGRND,
	AMG_MAX
} attackmenu_graphics1_t;

menugraphics_s attackmenu_graphics1[AMG_MAX] = 
{
	//X - 58
//	type		timer	x		y		width	height	file/text						graphic,	min		max	target	inc		style	color
	{ MG_GRAPHIC,	0.0,	100,	280,	4,		32,		"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_MIDLEFT
	{ MG_GRAPHIC,	0.0,	100,	180,	8,		97,		"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_UPPERLEFT
	{ MG_GRAPHIC,	0.0,	100,	315,	8,		100,	"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_LOWERLEFT
	{ MG_GRAPHIC,	0.0,	100,	164,	16,		 16,	"menu/common/corner_lu.tga",0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_UPPERCORNER
	{ MG_GRAPHIC,	0.0,	100,	406,	32,		 32,	"menu/common/newswoosh.tga",0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_LOWERCORNER

	{ MG_GRAPHIC,	0.0,	119,	164,	338,	  8,	"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_UPPERTOP1ST
	{ MG_GRAPHIC,	0.0,	117,	410,	340,	 18,	"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_LOWERTOP1ST

	{ MG_GRAPHIC,	0.0,	457,	164,	 34,	  8,	"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_UPPERTOP2ND
	{ MG_GRAPHIC,	0.0,	457,	410,	 34,	 18,	"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_LOWERTOP2ND

	{ MG_GRAPHIC,	0.0,	494,	164,	128,	128,	"menu/common/swoosh_top.tga",0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_UPPERSWOOP
	{ MG_GRAPHIC,	0.0,	483,	403,	128,	32,	"menu/common/newswoosh_long.tga",0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_LOWERSWOOP

	{ MG_GRAPHIC,	0.0,	501,	189,	110,	17,		"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_TOPRIGHT
	{ MG_GRAPHIC,	0.0,	501,	383,	110,	17,		"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_BOTTOMRIGHT

	{ MG_GRAPHIC,	0.0,	501,	206,	110,	177,	"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_PLAYERBKGRND
};
/*
===================
END MOD
===================
*/



static float mm_buttons[6][2] = 
{
{129,62},
{129,86},
{129,109},
{305,62},
{305,86},
{305,109}
};

static float federationTimer;

// Data for Welcome In Menu
static struct 
{
	menuframework_s	menu;
	sfxHandle_t		inSound;
	qhandle_t		logo;
	qhandle_t		halfround;

} s_HolomatchInmenu;

/*
=================
InitialSetup_Event
=================
*/
static void InitialSetup_Event( void* ptr, int notification )
{
	if (notification != QM_ACTIVATED)
	{
		return;
	}

	switch (((menucommon_s*)ptr)->id)
	{
		case ID_TEXTLANGUAGE:
			trap_Cvar_Set( "g_language", menu_normal_text[s_textlanguage_Names[s_initialsetup.textlanguage.curvalue]] );
			UI_LoadButtonText();
			UI_LoadMenuText();
			break;
		case ID_VOICELANGUAGE:
			trap_Cvar_Set( "s_language", menu_normal_text[s_voicelanguage_Names[s_initialsetup.voicelanguage.curvalue]] );
			break;
		case ID_KEYBOARDLANGUAGE:
			trap_Cvar_Set( "k_language", s_keyboardlanguage_Names[s_initialsetup.keyboardlanguage.curvalue] );
			break;
	}
}

/*
=================
InitialSetupApplyChanges - 
=================
*/
static void InitialSetupApplyChanges( void *unused, int notification )
{
	if (notification != QM_ACTIVATED)
	{
		return;
	}

	if (!uis.glconfig.deviceSupportsGamma)
	{
		trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart\n" );
	}

	trap_Cvar_Set("ui_initialsetup", "1");	// so this won't come up again
	UI_MainMenu();
}

/*
=================
M_InitialSetupMenu_Key
=================
*/
static sfxHandle_t M_InitialSetupMenu_Key (int key)
{
	if (key == K_ESCAPE)
	{
		return(0);
	}

	return ( Menu_DefaultKey( &s_initialsetup.menu, key ) );
}

/*
=================
M_InitialSetupMenu_Graphics
=================
*/
static void M_InitialSetupMenu_Graphics (void)
{
	int y;

	UI_MenuFrame2(&s_initialsetup.menu);

	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30,203, 47, 186, uis.whiteShader);	// Middle left line

	y = 97;

	if (uis.glconfig.deviceSupportsGamma)
	{
		trap_R_SetColor( colorTable[CT_DKGREY]);
		UI_DrawHandlePic(  178, y, 68, 68, uis.whiteShader);	// 
		trap_R_SetColor( colorTable[CT_WHITE]);
		UI_DrawHandlePic(  180, y+2, 64, 64, s_initialsetup.test);	// Starfleet graphic

		UI_DrawProportionalString( 256,  y + 5, menu_normal_text[MNT_GAMMA_LINE1],UI_SMALLFONT, colorTable[CT_LTGOLD1]);
		UI_DrawProportionalString( 256,  y + 25, menu_normal_text[MNT_GAMMA_LINE2],UI_SMALLFONT, colorTable[CT_LTGOLD1]);
		UI_DrawProportionalString( 256,  y + 45, menu_normal_text[MNT_GAMMA_LINE3],UI_SMALLFONT,colorTable[CT_LTGOLD1]);
	}
	else
	{
		UI_DrawProportionalString( 178,  y + 5, menu_normal_text[MNT_GAMMA2_LINE1],UI_SMALLFONT, colorTable[CT_LTGOLD1]);
		UI_DrawProportionalString( 178,  y + 25,menu_normal_text[MNT_GAMMA2_LINE2],UI_SMALLFONT, colorTable[CT_LTGOLD1]);
	}

}

/*
=================
M_InitialSetupMenu_Draw
=================
*/
static void M_InitialSetupMenu_Draw (void)
{	
	// Draw graphics particular to Main Menu
	M_InitialSetupMenu_Graphics();

	Menu_Draw( &s_initialsetup.menu );
}

/*
===============
InitialSetup_SetValues
===============
*/
static void InitialSetup_SetValues(void)
{
	char buffer[32];
	int *language;

	trap_Cvar_VariableStringBuffer( "g_language", buffer, 32 );
	language = s_textlanguage_Names;
	
	s_initialsetup.textlanguage.curvalue=0;
	if (buffer[0]) 
	{
		while (*language)
		{
			if (Q_stricmp(menu_normal_text[*language],buffer)==0)
			{
				break;
			}
			language++;
			s_initialsetup.textlanguage.curvalue++;
		}

		if (!*language)
		{
			s_initialsetup.textlanguage.curvalue = 0;
		}
	}

	trap_Cvar_VariableStringBuffer( "s_language", buffer, 32 );
	language = s_voicelanguage_Names;
	s_initialsetup.voicelanguage.curvalue=0;
	if (buffer[0]) 
	{
		while (*language)
		{
			if (Q_stricmp(menu_normal_text[*language],buffer)==0)
			{
				break;
			}
			language++;
			s_initialsetup.voicelanguage.curvalue++;
		}

		if (!*language)
		{
			s_initialsetup.voicelanguage.curvalue = 0;
		}
	}

	trap_Cvar_VariableStringBuffer( "k_language", buffer, 32 );

	s_initialsetup.keyboardlanguage.curvalue=0;
	if (buffer[0]) 
	{
		while (s_keyboardlanguage_Names[s_initialsetup.keyboardlanguage.curvalue])
		{
			if (Q_stricmp(s_keyboardlanguage_Names[s_initialsetup.keyboardlanguage.curvalue],buffer)==0)
			{
				break;
			}
			s_initialsetup.keyboardlanguage.curvalue++;
		}

		if (!s_keyboardlanguage_Names[s_initialsetup.keyboardlanguage.curvalue])
		{
			s_initialsetup.keyboardlanguage.curvalue = 0;
		}
	}

	s_initialsetup.gamma.curvalue = trap_Cvar_VariableValue( "r_gamma" ) *  10.0f;
}

/*
===============
InitialSetupMenu_Cache
===============
*/
static void InitialSetupMenu_Cache( void ) 
{

	s_initialsetup.test = trap_R_RegisterShaderNoMip("menu/special/gamma_test.tga");

}

/*
===============
InitialSetupMenu_Init
===============
*/
static void InitialSetupMenu_Init( void ) 
{
	int x = 179;
	int y = 260;

	InitialSetupMenu_Cache();

	s_initialsetup.menu.nitems					= 0;
	s_initialsetup.menu.wrapAround				= qtrue;
	s_initialsetup.menu.draw					= M_InitialSetupMenu_Draw;
	s_initialsetup.menu.key						= M_InitialSetupMenu_Key;
	s_initialsetup.menu.fullscreen				= qtrue;
	s_initialsetup.menu.descX					= MENU_DESC_X;
	s_initialsetup.menu.descY					= MENU_DESC_Y;
	s_initialsetup.menu.titleX					= MENU_TITLE_X;
	s_initialsetup.menu.titleY					= MENU_TITLE_Y;
	s_initialsetup.menu.titleI					= MNT_INITIALSETUP_TITLE;
	s_initialsetup.menu.footNoteEnum			= MNT_INITIALSETUP;


	s_initialsetup.textlanguage.generic.type		= MTYPE_SPINCONTROL;      
	s_initialsetup.textlanguage.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_initialsetup.textlanguage.generic.x			= x;
	s_initialsetup.textlanguage.generic.y			= y;
	s_initialsetup.textlanguage.generic.name		= GRAPHIC_BUTTONRIGHT;
	s_initialsetup.textlanguage.generic.callback	= InitialSetup_Event; 
	s_initialsetup.textlanguage.generic.id			= ID_TEXTLANGUAGE; 
	s_initialsetup.textlanguage.color				= CT_DKPURPLE1;
	s_initialsetup.textlanguage.color2				= CT_LTPURPLE1;
	s_initialsetup.textlanguage.textX				= MENU_BUTTON_TEXT_X;
	s_initialsetup.textlanguage.textY				= MENU_BUTTON_TEXT_Y;
	s_initialsetup.textlanguage.width				= 80;
	s_initialsetup.textlanguage.textEnum			= MBT_TEXTLANGUAGE;
	s_initialsetup.textlanguage.textcolor			= CT_BLACK;
	s_initialsetup.textlanguage.textcolor2		= CT_BLACK; //CT_WHITE;	
	s_initialsetup.textlanguage.listnames			= s_textlanguage_Names;
	y += 32;

	s_initialsetup.voicelanguage.generic.type		= MTYPE_SPINCONTROL;      
	s_initialsetup.voicelanguage.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_initialsetup.voicelanguage.generic.x			= x;
	s_initialsetup.voicelanguage.generic.y			= y;
	s_initialsetup.voicelanguage.generic.name		= GRAPHIC_BUTTONRIGHT;
	s_initialsetup.voicelanguage.generic.callback	= InitialSetup_Event; 
	s_initialsetup.voicelanguage.generic.id			= ID_VOICELANGUAGE; 
	s_initialsetup.voicelanguage.color				= CT_DKPURPLE1;
	s_initialsetup.voicelanguage.color2				= CT_LTPURPLE1;
	s_initialsetup.voicelanguage.textX				= MENU_BUTTON_TEXT_X;
	s_initialsetup.voicelanguage.textY				= MENU_BUTTON_TEXT_Y;
	s_initialsetup.voicelanguage.width				= 80;
	s_initialsetup.voicelanguage.textEnum			= MBT_VOICELANGUAGE;
	s_initialsetup.voicelanguage.textcolor			= CT_BLACK;
	s_initialsetup.voicelanguage.textcolor2			= CT_BLACK; //CT_WHITE;	
	s_initialsetup.voicelanguage.listnames			= s_voicelanguage_Names;
	y += 32;

	s_initialsetup.keyboardlanguage.generic.type		= MTYPE_SPINCONTROL;      
	s_initialsetup.keyboardlanguage.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_initialsetup.keyboardlanguage.generic.x			= x;
	s_initialsetup.keyboardlanguage.generic.y			= y;
	s_initialsetup.keyboardlanguage.generic.name		= GRAPHIC_BUTTONRIGHT;
	s_initialsetup.keyboardlanguage.generic.callback	= InitialSetup_Event; 
	s_initialsetup.keyboardlanguage.generic.id			= ID_KEYBOARDLANGUAGE; 
	s_initialsetup.keyboardlanguage.color				= CT_DKPURPLE1;
	s_initialsetup.keyboardlanguage.color2				= CT_LTPURPLE1;
	s_initialsetup.keyboardlanguage.textX				= MENU_BUTTON_TEXT_X;
	s_initialsetup.keyboardlanguage.textY				= MENU_BUTTON_TEXT_Y;
	s_initialsetup.keyboardlanguage.width				= 80;
	s_initialsetup.keyboardlanguage.textEnum			= MBT_KEYBOARDLANGUAGE;
	s_initialsetup.keyboardlanguage.textcolor			= CT_BLACK;
	s_initialsetup.keyboardlanguage.textcolor2		= CT_BLACK; //CT_WHITE;	
	s_initialsetup.keyboardlanguage.itemnames			= s_keyboardlanguage_Names;

	x = 180;
	y = 182;
	s_initialsetup.gamma.generic.type			= MTYPE_SLIDER;
	s_initialsetup.gamma.generic.x				= x + 162;
	s_initialsetup.gamma.generic.y				= y;
	s_initialsetup.gamma.generic.flags			= QMF_SMALLFONT;
	s_initialsetup.gamma.generic.callback		= GammaCallback;
	s_initialsetup.gamma.minvalue				= 5;
	s_initialsetup.gamma.maxvalue				= 30;
	s_initialsetup.gamma.color					= CT_DKPURPLE1;
	s_initialsetup.gamma.color2					= CT_LTPURPLE1;
	s_initialsetup.gamma.generic.name			= "menu/common/monbar_2.tga";
	s_initialsetup.gamma.width					= 256;
	s_initialsetup.gamma.height					= 32;
	s_initialsetup.gamma.focusWidth				= 145;
	s_initialsetup.gamma.focusHeight			= 18;
	s_initialsetup.gamma.picName				= "menu/common/square.tga";
	s_initialsetup.gamma.picX					= x;
	s_initialsetup.gamma.picY					= y;
	s_initialsetup.gamma.picWidth				= MENU_BUTTON_MED_WIDTH + 21;
	s_initialsetup.gamma.picHeight				= MENU_BUTTON_MED_HEIGHT;
	s_initialsetup.gamma.textX					= MENU_BUTTON_TEXT_X;
	s_initialsetup.gamma.textY					= MENU_BUTTON_TEXT_Y;
	s_initialsetup.gamma.textEnum				= MBT_BRIGHTNESS;
	s_initialsetup.gamma.textcolor				= CT_BLACK;
	s_initialsetup.gamma.textcolor2		= CT_BLACK; //CT_WHITE;
	s_initialsetup.gamma.thumbName				= GRAPHIC_BUTTONSLIDER;
	s_initialsetup.gamma.thumbHeight			= 32;
	s_initialsetup.gamma.thumbWidth				= 16;
	s_initialsetup.gamma.thumbGraphicWidth		= 9;
	s_initialsetup.gamma.thumbColor				= CT_DKBLUE1;
	s_initialsetup.gamma.thumbColor2			= CT_LTBLUE1;


	s_initialsetup.apply.generic.type				= MTYPE_ACTION;
	s_initialsetup.apply.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS|QMF_BLINK;
	s_initialsetup.apply.generic.x					= 501;
	s_initialsetup.apply.generic.y					= 357;
	s_initialsetup.apply.generic.callback			= InitialSetupApplyChanges;
	s_initialsetup.apply.textEnum					= MBT_ACCEPT;
	s_initialsetup.apply.textcolor					= CT_BLACK;
	s_initialsetup.apply.textcolor2					= CT_WHITE;
	s_initialsetup.apply.textcolor3					= CT_LTGREY;
	s_initialsetup.apply.color						= CT_DKPURPLE1;
	s_initialsetup.apply.color2						= CT_LTPURPLE1;
	s_initialsetup.apply.color3						= CT_DKGREY;
	s_initialsetup.apply.textX						= 5;
	s_initialsetup.apply.textY						= 47;
	s_initialsetup.apply.width						= 110;
	s_initialsetup.apply.height						= 65;


	Menu_AddItem( &s_initialsetup.menu, &s_initialsetup.gamma );
	Menu_AddItem( &s_initialsetup.menu, &s_initialsetup.textlanguage );
	Menu_AddItem( &s_initialsetup.menu, &s_initialsetup.voicelanguage );
	Menu_AddItem( &s_initialsetup.menu, &s_initialsetup.keyboardlanguage );
	Menu_AddItem( &s_initialsetup.menu, &s_initialsetup.apply );

	InitialSetup_SetValues();
}

static void UI_InitialSetupMenu(void)
{
	InitialSetupMenu_Init(); 

	UI_PushMenu ( &s_initialsetup.menu );

	trap_Key_SetCatcher( KEYCATCH_UI );
	uis.menusp = 0;
}

//TiM - Functions needed for the quick connect stuff

int ArenaServers_MaxPing( void );

/*
=================
MainArenaServers_Compare
=================
*/
static int QDECL MainArenaServers_Compare( const void *arg1, const void *arg2 ) {
	float			f1;
	float			f2;
	table_t*		t1;
	table_t*		t2;

	//t1 = (servernode_t *)arg1;
	//t2 = (servernode_t *)arg2;

	t1 = (table_t *)arg1;
	t2 = (table_t *)arg2;

	//hard-coded for client sorting
	//f1 = t1->servernode.maxclients - t1->servernode.numclients;
	f1 = t1->servernode.numclients;
	if( f1 < 0 ) {
		f1 = 0;
	}

	//f2 = t2->servernode.maxclients - t2->servernode.numclients;
	f2 = t2->servernode.numclients;
	if( f2 < 0 ) {
		f2 = 0;
	}

	if( f1 < f2 ) {
		return 1;
	}
	if( f1 == f2 ) {
		return 0;
	}
	return -1;
}

/*
=================
MainArenaServers_UpdateMenu
=================
*/
static void MainArenaServers_UpdateMenu( void ) {
	int				i;
	//int				j;
	//int				count;
	//char*			buff;
	//servernode_t*	servernodeptr;
	//table_t*		tableptr;
	//char			*pingColor, *pongColor;

	if( s_main.networkParms.numservers > 0 ) {
		// servers found
		if( s_main.networkParms.refreshservers && ( s_main.networkParms.currentping <= s_main.networkParms.numservers ) ) {
			qsort( s_main.networkParms.table, s_main.networkParms.numservers, sizeof( table_t ), MainArenaServers_Compare);
		}
		else {
			// all servers pinged - enable controls
			s_main.refresh.generic.flags	&= ~( QMF_GRAYED | QMF_INACTIVE );
			//s_main.connect.generic.flags	&= ~( QMF_GRAYED | QMF_INACTIVE );

			//if required, enable the arrow buttons
			if ( s_main.networkParms.numservers > MAX_FAVBUTTONS ) {
				s_main.upArrow.generic.flags &= ~( QMF_GRAYED | QMF_INACTIVE );
				s_main.dnArrow.generic.flags &= ~( QMF_GRAYED | QMF_INACTIVE );

				s_main.upArrow.generic.flags |= QMF_HIGHLIGHT_IF_FOCUS;
				s_main.dnArrow.generic.flags |= QMF_HIGHLIGHT_IF_FOCUS;
			}

			//enable selection buttons
			for ( i = 0; i < MAX_FAVBUTTONS; i++ ) {
				if ( s_main.networkParms.numservers > i ) {
					//TiM : Unhide the necessary menu buttons as we go along
					s_main.favMenu[i].generic.flags = QMF_SILENT;
				}
			}
		}
	}
	else {
		// no servers found
		if( s_main.networkParms.refreshservers ) 
		{
			// disable controls during refresh
			s_main.refresh.generic.flags	|= ( QMF_GRAYED | QMF_INACTIVE );
			//s_main.connect.generic.flags	|= ( QMF_GRAYED | QMF_INACTIVE );

			s_main.upArrow.generic.flags	|= ( QMF_GRAYED | QMF_INACTIVE );
			s_main.dnArrow.generic.flags	|= ( QMF_GRAYED | QMF_INACTIVE );
		}
		else {
			s_main.refresh.generic.flags	&= ~( QMF_GRAYED | QMF_INACTIVE );
			//s_main.connect.generic.flags	|= ( QMF_GRAYED | QMF_INACTIVE );

			s_main.upArrow.generic.flags	|= ( QMF_GRAYED | QMF_INACTIVE );
			s_main.dnArrow.generic.flags	|= ( QMF_GRAYED | QMF_INACTIVE );

		}

		for ( i = 0; i < MAX_FAVBUTTONS; i++ )
			s_main.favMenu[i].generic.flags |= (QMF_HIDDEN | QMF_INACTIVE);
		return;
	}

	// build list box strings - apply culling filters
	/*count         = s_main.networkParms.numservers;
	for( i = 0, j = 0; i < count; i++ ) {
		tableptr = &s_main.networkParms.table[j];
		servernodeptr = &tableptr->servernode;
		buff = tableptr->buff;

		if( servernodeptr->pingtime < servernodeptr->minPing ) {
			pingColor = S_COLOR_BLUE;
		}
		else if( servernodeptr->maxPing && servernodeptr->pingtime > servernodeptr->maxPing ) {
			pingColor = S_COLOR_BLUE;
		}
		else if( servernodeptr->pingtime < 50 ) {
			pingColor = S_COLOR_WHITE;
		}
		else if( servernodeptr->pingtime < 200 ) {
			pingColor = S_COLOR_GREEN;
		}
		else if( servernodeptr->pingtime < 400 ) {
			pingColor = S_COLOR_YELLOW;
		}
		else {
			pingColor = S_COLOR_RED;
		}

		pongColor = S_COLOR_WHITE;
		Com_sprintf( buff, MAX_LISTBOXWIDTH, "%s%-31.31s %s%-11.11s %2d/%2d %-8.8s %3d", //31.31
			pongColor, servernodeptr->hostname, pingColor, servernodeptr->mapname, servernodeptr->numclients,
	 		servernodeptr->maxclients, servernodeptr->gamename, servernodeptr->pingtime);
		if (!servernodeptr->isPure) {	//prev length is 62, we can safely add 2 more chars.
			strcat(buff, "*");	//mark this unpure server!
		}
		j++;
	}*/

//	Com_sprintf( g_arenaservers.status.string, MAX_STATUSLENGTH, "%d of %d Arena Servers.", j, *g_arenaservers.numservers );

	if (!s_main.networkParms.numservers )
	{
		s_main.connect.generic.flags			|= ( QMF_GRAYED | QMF_INACTIVE );
	}
}

/*
=================
MainArenaServers_Insert
=================
*/
static void MainArenaServers_Insert( char* adrstr, char* info, int pingtime )
{
	servernode_t*	servernodeptr;
	char*			s;
	int				i;

	if (s_main.networkParms.numservers >= s_main.networkParms.maxservers ) {
		// list full;
		servernodeptr = &s_main.networkParms.table[s_main.networkParms.numservers-1].servernode;
	} else {
		// next slot

		servernodeptr = &s_main.networkParms.table[s_main.networkParms.numservers].servernode;
		s_main.networkParms.numservers++;
	}

	Q_strncpyz( servernodeptr->adrstr, adrstr, MAX_ADDRESSLENGTH );

	Q_strncpyz( servernodeptr->hostname, Info_ValueForKey( info, "hostname"), MAX_HOSTNAMELENGTH );
	Q_CleanStr( servernodeptr->hostname );
	Q_CleanStr( servernodeptr->hostname );
	Q_CleanStr( servernodeptr->hostname );
	Q_CleanStr( servernodeptr->hostname );

	Q_strncpyz( servernodeptr->mapname, Info_ValueForKey( info, "mapname"), MAX_MAPNAMELENGTH );
	Q_CleanStr( servernodeptr->mapname );
	Q_strlwr( servernodeptr->mapname );

	servernodeptr->numclients = atoi( Info_ValueForKey( info, "clients") );
	servernodeptr->maxclients = atoi( Info_ValueForKey( info, "sv_maxclients") );
	servernodeptr->pingtime   = pingtime;
	servernodeptr->minPing    = atoi( Info_ValueForKey( info, "minPing") );
	servernodeptr->maxPing    = atoi( Info_ValueForKey( info, "maxPing") );

	servernodeptr->isPure	  = atoi( Info_ValueForKey( info, "pure") );
	servernodeptr->isExpansion= atoi( Info_ValueForKey( info, "vv") );

	s = Info_ValueForKey( info, "game");
	i = atoi( Info_ValueForKey( info, "gametype") );
	if( i < 0 ) {
		i = 0;
	}
	else if( i > 5 ) {
		i = 5;
	}
	if( *s ) {
		servernodeptr->gametype = -1;
		Q_strncpyz( servernodeptr->gamename, s, sizeof(servernodeptr->gamename) );
	}
	else {
		servernodeptr->gametype = i;
		if (servernodeptr->isExpansion)	{
			Com_sprintf( servernodeptr->gamename, sizeof(servernodeptr->gamename), "vv-%s",gamenames[i]);
		} else {
			Q_strncpyz( servernodeptr->gamename, gamenames[i], sizeof(servernodeptr->gamename) );
		}
	}
}

/*
=================
MainArenaServers_StopRefresh
=================
*/
static void MainArenaServers_StopRefresh( void )
{
	if (!s_main.networkParms.refreshservers)
		// not currently refreshing
		return;

	s_main.networkParms.refreshservers = qfalse;

	// nonresponsive favorites must be shown
	//MainArenaServers_InsertFavorites(); -TIMMARK
	if ( s_main.networkParms.numservers < s_main.networkParms.maxservers )
	{
		char infoString[MAX_INFO_STRING];
		int	i;
		int j;

		infoString[0]='\0';
		//write into the host name the IP that didn't work			
		Info_SetValueForKey( infoString, "hostname", "No Response" );

		for ( i = 0; i < s_main.networkParms.maxservers; i++ ) {
			
			//loop thru all the successfully pinged servers, and check that this one from the main list was found
			for ( j = 0; j < s_main.networkParms.numservers; j++ ) {
				if ( !Q_stricmp( s_main.networkParms.favBuffer[i], s_main.networkParms.table[j].servernode.adrstr ) )
					break;
			}

			//whup, I guess we couldn't see it
			if ( j >= s_main.networkParms.numservers ) {
				//Insert into the list as a normal entry.  Who knows, the server might be up and Q3 is dumb
				MainArenaServers_Insert( s_main.networkParms.favBuffer[i], infoString, ArenaServers_MaxPing() );
			}
		}
	}
	
	// sort
	qsort( s_main.networkParms.table, s_main.networkParms.numservers, sizeof( table_t ), MainArenaServers_Compare);

	if ( !s_main.networkParms.numservers )
		s_main.networkParms.showNoServersAlert = qtrue;

	//Com_Printf( S_COLOR_RED "OUTPUT: Name: %s, Map: %s\n", s_main.networkParms.table[1].servernode.hostname, s_main.networkParms.table[1].servernode.mapname );

	MainArenaServers_UpdateMenu();
}


/*
=================
MainArenaServers_DoRefresh
=================
*/

static void MainArenaServers_DoRefresh( void )
{
	int		i;
	int		j;
	int		time;
	int		maxPing;
	char	adrstr[MAX_ADDRESSLENGTH];
	char	info[MAX_INFO_STRING];

	if (uis.realtime < s_main.networkParms.nextpingtime)
	{
		// wait for time trigger
		return;
	}

	// trigger at 10Hz intervals
	s_main.networkParms.nextpingtime = uis.realtime + 50;

	// process ping results
	maxPing = ArenaServers_MaxPing();
	for (i=0; i<MAX_PINGREQUESTS; i++)
	{
		trap_LAN_GetPing( i, adrstr, MAX_ADDRESSLENGTH, &time );
		if (!adrstr[0])
		{
			// ignore empty or pending pings
			continue;
		}

		// find ping result in our local list
		for (j=0; j<MAX_PINGREQUESTS; j++)
			if (!Q_stricmp( adrstr, s_main.networkParms.pinglist[j].adrstr ))
				break;

		if (j < MAX_PINGREQUESTS)
		{
			// found it
			if (!time)
			{
				time = uis.realtime - s_main.networkParms.pinglist[j].start;
				if (time < maxPing)
				{
					// still waiting
					continue;
				}
			}

			if (time > maxPing)
			{
				// stale it out
				info[0] = '\0';
				time    = maxPing;
			}
			else
			{
				trap_LAN_GetPingInfo( i, info, MAX_INFO_STRING ); //bookmark - TiM: This is where the info is received!
				
				//TiM - testing the data output from this baby
				//if ( !Q_stricmp( Info_ValueForKey( info, "game" ), "rpg-x" ) )
					//Com_Printf( "%s", info );
			}

			// insert ping results
			MainArenaServers_Insert( adrstr, info, time );

			// clear this query from internal list
			s_main.networkParms.pinglist[j].adrstr[0] = '\0';
   		}

		// clear this query from external list
		trap_LAN_ClearPing( i );
	}

	// send ping requests in reasonable bursts
	// iterate ping through all found servers
	for (i=0; i<MAX_PINGREQUESTS && s_main.networkParms.currentping < s_main.networkParms.maxservers; i++)
	{
		if (trap_LAN_GetPingQueueCount() >= MAX_PINGREQUESTS)
		{
			// ping queue is full
			break;
		}

		// find empty slot
		for (j=0; j<MAX_PINGREQUESTS; j++)
			if (!s_main.networkParms.pinglist[j].adrstr[0])
				break;

		if (j >= MAX_PINGREQUESTS)
			// no empty slots available yet - wait for timeout
			break;		

		strcpy( s_main.networkParms.pinglist[j].adrstr, s_main.networkParms.favBuffer[s_main.networkParms.currentping] );
		s_main.networkParms.pinglist[j].start = uis.realtime;

		trap_Cmd_ExecuteText( EXEC_NOW, va( "ping %s\n", s_main.networkParms.pinglist[j].adrstr )  );
		
		// advance to next server
		s_main.networkParms.currentping++;
	}

	if (!trap_LAN_GetPingQueueCount())
	{
		// all pings completed
		MainArenaServers_StopRefresh();
		return;
	}

	// update the user interface with ping status
	MainArenaServers_UpdateMenu();
}


/*
=================
MainArenaServers_StartRefresh
=================
*/
static void MainArenaServers_StartRefresh( void )
{
	int		i;
	char	address[MAX_ADDRESSLENGTH];

	memset( &s_main.networkParms, 0, sizeof( s_main.networkParms ) );

	//fill the main server list with all the faved addresses we have
	for ( i=0; i < MAX_FAVORITESERVERS; i++ ) {
		trap_Cvar_VariableStringBuffer( va( "server%i", i ), address, MAX_ADDRESSLENGTH );

		if ( !address[0] || !strlen( address ) )
			continue;

		Q_strncpyz( s_main.networkParms.favBuffer[s_main.networkParms.maxservers], address, MAX_ADDRESSLENGTH );
		s_main.networkParms.maxservers++;
	}

	for (i=0; i<MAX_PINGREQUESTS; i++)
	{
		s_main.networkParms.pinglist[i].adrstr[0] = '\0';
		trap_LAN_ClearPing( i );
	}

	s_main.networkParms.refreshservers    = qtrue;
	s_main.networkParms.currentping       = 0;
	s_main.networkParms.nextpingtime      = 0;
	s_main.networkParms.numservers       = 0;

	//unselect 
	s_main.selectedFav					= -1;

	// allow max 5 seconds for responses
	s_main.networkParms.refreshtime = uis.realtime + 5000;

	// place menu in zeroed state
	MainArenaServers_UpdateMenu();
}

/*
=================
Main_MenuEvent
=================
*/
static void Main_MenuEvent (void* ptr, int event) 
{
	//void**		weaponptr;

	if( event != QM_ACTIVATED ) 
	{
		return;
	}

	uis.spinView = qfalse;

	switch( ((menucommon_s*)ptr)->id ) 
	{
		case ID_MULTIPLAYER:
			UI_ChooseServerTypeMenu();
			break;

		case ID_SETUP:
			UI_SetupWeaponsMenu();
			break;

		case ID_DEMOS:
			UI_DemosMenu();
			break;

	//	case ID_CINEMATICS:
	//		UI_CinematicsMenu();
	//		break;

		case ID_MODS:
			UI_ModsMenu();
			break;

		case ID_CREDITS:
			UI_CreditsMenu();
			break;

		case ID_QUIT:
			UI_PopMenu();
			UI_QuitMenu();

	//	case ID_FAVREFRESH:
	//		Main_StartRefresh();

	//		UI_ConfirmMenu( "EXIT GAME?", NULL, MainMenu_ExitAction );
			break;

		case ID_PLAYER_SETTINGS:
			UI_PlayerSettingsMenu(PS_MENU_CONTROLS);
			break;

		case ID_PLAYER_MODEL:
			UI_PlayerModelMenu( PS_MENU_CONTROLS );
			break;

		case ID_LIST_UP:
			s_main.favOffset--;

			if ( s_main.favOffset < 0 )
				s_main.favOffset = 0;
			break;
		case ID_LIST_DOWN:
			s_main.favOffset++;

			if ( s_main.networkParms.numservers > MAX_FAVBUTTONS && s_main.favOffset > ( s_main.networkParms.numservers - MAX_FAVBUTTONS ) )
				s_main.favOffset = ( s_main.networkParms.numservers - MAX_FAVBUTTONS );

			break;
		case ID_FAV0:
		case ID_FAV1:
		case ID_FAV2:
		case ID_FAV3:
		case ID_FAV4:
		case ID_FAV5:
		case ID_FAV6:
		case ID_FAV7:
			trap_S_StartLocalSound( uis.menu_choice1_snd, CHAN_MENU1 );
			s_main.selectedFav = ((menucommon_s *)ptr)->id - 100 + s_main.favOffset;

			if ( s_main.connect.generic.flags & ( QMF_INACTIVE | QMF_GRAYED ) )
				s_main.connect.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;

			break;

		case ID_REFRESH:
			MainArenaServers_StartRefresh();
			break;

		case ID_CONNECT:
			if ( s_main.selectedFav >= 0 )
				trap_Cmd_ExecuteText( EXEC_APPEND, va( "connect %s\n", s_main.networkParms.table[s_main.selectedFav].servernode.adrstr) );
			break;

			case ID_Q_VOYAGER_LABEL:
			case ID_PHASER_LABEL:
			case ID_TORPEDO_LABEL:
			case ID_VENTRAL_LABEL:
			case ID_MIDHULL_LABEL:
			case ID_BUSSARD_LABEL:
			case ID_NACELLES_LABEL:
			case ID_THRUSTERS_LABEL:
			case ID_BRIDGE_LABEL:
				QuitMenuInterrupt(((menucommon_s*)ptr)->id);
				break;
			/*case ID_WEAPON1 :
			case ID_WEAPON2 :
			case ID_WEAPON3 :
			case ID_WEAPON4 :
			case ID_WEAPON5 :
			case ID_WEAPON6 :
			case ID_WEAPON7 :
			case ID_WEAPON8 :
			case ID_WEAPON9 :

				trap_S_StartLocalSound( s_main.weaponnamesnd[((menucommon_s*)ptr)->id - ID_WEAPON1],CHAN_MENU1);	// Weapon name

				trap_S_StartLocalSound( uis.menu_choice1_snd, CHAN_LOCAL_SOUND );

				weaponptr = g_weapons[s_main.currentWeapon];	
				((menubitmap_s*)weaponptr)->textcolor = CT_BLACK;

				s_main.timer = uis.realtime + (WEAPON_WAIT * 2);
				s_main.currentWeapon = (((menucommon_s*)ptr)->id - ID_WEAPON1);

				weaponptr = g_weapons[s_main.currentWeapon];	
				((menubitmap_s*)weaponptr)->textcolor = CT_LTGOLD1;

				break;*/

	//	case ID_EXIT:
	//		UI_ConfirmMenu( "EXIT GAME?", NULL, MainMenu_ExitAction );
	//		break;
	}
}


/*
=================
UI_MainMenuButtons
=================
*/
static void UI_MainMenuButtons(int count)
{
	int i, j;

	trap_R_SetColor( colorTable[CT_DKPURPLE1]);
	
	if (count==99) {	//This how you spell hack?
		j = 6;
	} else {
		j = count;
	}
	for (i=0;i<j;++i)
	{
		if ((count==99) && (i==5)) {	//SOMEBODY SPELL RPG-X HACK!
			trap_R_SetColor( colorTable[CT_VDKRED1]);
		}
		UI_DrawHandlePic(mm_buttons[i][0] - 14,mm_buttons[i][1], MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
	}
}

typedef struct
{
	qhandle_t	modelhandle;		// model Handle
	int			pitch;
	int			roll;
	vec3_t		mid;
	char*		modelname;
} weapongraphics_s;

/*
==========================
RPG-X Modification
Phenix
1/6/2004
==========================
*/

/*static void UI_drawRPGXLogo ( void )
{
	refdef_t		refdef;
	refEntity_t		ent;
	vec3_t			origin = {0.0, 0.0, 0.0};
	vec3_t			angles;
	float			adjust;
	float			x, y, w, h;
	//vec4_t			color = {0.5, 0, 0, 1};

	// setup the refdef

	memset( &refdef, 0, sizeof( refdef ) );

	refdef.rdflags = RDF_NOWORLDMODEL;

	AxisClear( refdef.viewaxis );

	x = 465; //440 // controls location of top left corner of ent
	y = 309 - 146; //0

	w = 139; //400; //640 //150 //TiM: I don't know exactly, but w and h seem to control the size of the region the model will be drawn in
	h = 139; //480; //120 //150 //good going brainiac! ;P - TiM several months later ;)
	UI_AdjustFrom640( &x, &y, &w, &h );
	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	adjust = 0; // JDC: Kenneth asked me to stop this 1.0 * sin( (float)uis.realtime / 1000 );
	refdef.fov_x = 10 + adjust; //60 //TiM: Controls width scale // yeah.... coz it's the FOV lol //17
																//that being the case, we have to aim for a value of 10
																//(10's what I use in MD3View... anything higher and it starts to distort)
	refdef.fov_y = 10 + adjust; //19.6875 // like i said, 10 ;)

	refdef.time = uis.realtime;

	//TiM: Okay... several months later and I actually understand what this whacky array called 
	//a 'vector' actually does. ;P
	origin[0] = 930; //300 //TiM: Controls Overall Size //controls Z position (and hence, the scale coz it's either moving towards, or away from the camera)
	origin[1] = 0; //TiM: Controls Horizontal Position //controls X
	origin[2] = -27; //-32 //TiM: Controls vertical Position //controls Y

	trap_R_ClearScene();

	// add the model

	memset( &ent, 0, sizeof(ent) );

	adjust = 20.0/300.0 * uis.realtime; //5.0 * sin( (float)uis.realtime / 5000 );
	VectorSet( angles, 0, 180 + adjust, 0 );
	AnglesToAxis( angles, ent.axis );
	ent.hModel = s_main.logoModel;

	
	if (trap_Cvar_VariableValue("r_dynamiclight") ) {
		ent.renderfx = RF_LOWLIGHT|RF_NOSHADOW;		// keep it dark, and no stencil shadows //RF_LOWBRIGHT
	} else {
		ent.renderfx = RF_NOSHADOW;		// no stencil shadows
	}
	

	VectorCopy( origin, ent.origin );
	VectorCopy( origin, ent.lightingOrigin );
	VectorCopy( ent.origin, ent.oldorigin );

	origin[0] -= 50;	// + = behind, - = in front
	origin[1] += 50;	// + = left, - = right
	origin[2] += 50;	// + = above, - = below
	trap_R_AddLightToScene( origin, 500, 1.0, 1.0, 1.0 );

	trap_R_AddRefEntityToScene( &ent );

	trap_R_RenderScene( &refdef );
}

void P_WeaponsMenu_Blinkies (void)
{
	int i;

	for (i=0;i<AMG_MAX;++i)
	{
		attackmenu_graphics1[i].color = CT_VDKPURPLE1;
	}

	if (attackmenu_graphics1[AMG_MIDLEFT].timer < uis.realtime)
	{
		attackmenu_graphics1[AMG_MIDLEFT].timer = uis.realtime + 500;
		++attackmenu_graphics1[AMG_MIDLEFT].target;
		if (attackmenu_graphics1[AMG_MIDLEFT].target > 7)
		{
			attackmenu_graphics1[AMG_MIDLEFT].target = 0;
		}
	}

	switch (attackmenu_graphics1[AMG_MIDLEFT].target)
	{
		case 0:
			attackmenu_graphics1[AMG_MIDLEFT].color = CT_LTPURPLE1;
			break;
		case 1:
			attackmenu_graphics1[AMG_UPPERLEFT].color = CT_LTPURPLE1;
			attackmenu_graphics1[AMG_LOWERLEFT].color = CT_LTPURPLE1;
			break;
		case 2:
			attackmenu_graphics1[AMG_UPPERCORNER].color = CT_LTPURPLE1;
			attackmenu_graphics1[AMG_LOWERCORNER].color = CT_LTPURPLE1;
			break;
		case 3:
			attackmenu_graphics1[AMG_UPPERTOP1ST].color = CT_LTPURPLE1;
			attackmenu_graphics1[AMG_LOWERTOP1ST].color = CT_LTPURPLE1;
			break;
		case 4:
			attackmenu_graphics1[AMG_UPPERTOP2ND].color = CT_LTPURPLE1;
			attackmenu_graphics1[AMG_LOWERTOP2ND].color = CT_LTPURPLE1;
			break;
		case 5:
			attackmenu_graphics1[AMG_UPPERSWOOP].color = CT_LTPURPLE1;
			attackmenu_graphics1[AMG_LOWERSWOOP].color = CT_LTPURPLE1;
			break;
		case 6:
			attackmenu_graphics1[AMG_TOPRIGHT].color = CT_LTPURPLE1;
			attackmenu_graphics1[AMG_BOTTOMRIGHT].color = CT_LTPURPLE1;
			break;
		case 7:
			attackmenu_graphics1[AMG_PLAYERBKGRND].color = CT_LTPURPLE1;
			break;
	}

}*/

static void Player_DrawPlayer( void ) //*self ) 
{
	menubitmap_s	*b;
	vec3_t			viewangles;
	vec3_t			origin = {-3, 5, -3 };//{ 0, 3.8, 0};
	char			buf[MAX_QPATH];
//	float				yaw;

	trap_Cvar_VariableStringBuffer( "model", buf, sizeof( buf ) );
	

	if ( Q_stricmp( buf, s_main.playerinfo.modelName ) 
		|| trap_Cvar_VariableValue( "height" ) != s_main.playerinfo.height
		|| trap_Cvar_VariableValue( "weight" ) != s_main.playerinfo.weight ) 
	{
		UI_PlayerInfo_SetModel( &s_main.playerinfo, buf);

		viewangles[YAW] = uis.lastYaw; //yaw
		viewangles[PITCH] = 0;
		viewangles[ROLL]  = 0;

		UI_PlayerInfo_SetInfo( &s_main.playerinfo, BOTH_STAND1, BOTH_STAND1, viewangles, vec3_origin, WP_1, trap_Cvar_VariableValue( "height" ), trap_Cvar_VariableValue( "weight" ), qfalse );

		//Player_UpdateModel( ANIM_IDLE ); //ADDDED 
		//UI_PlayerInfo_SetInfo( &s_main.playerinfo, LEGS_IDLE, TORSO_STAND2, viewangles, vec3_origin, s_main.playerWeapon, qfalse );
	}

	b = &s_main_playermdl; //(menubitmap_s*) self;
	UI_DrawPlayer( b->generic.x, b->generic.y, b->width, b->height, origin, &s_main.playerinfo, uis.realtime );
	//UI_DrawPlayer( s_main_playermdl.generic.x, s_main_playermdl.generic.y, s_main_playermdl.width, s_main_playermdl.height, origin, &s_main.playerinfo, uis.realtime/2 );

}

/*
=================
Player_SpinPlayer
=================
*/
static void Player_SpinPlayer( void* ptr, int event)
{
	if ( event == QM_ACTIVATED ) 
	{
		uis.spinView = qtrue;
		uis.cursorpx = uis.cursorx;
	}
}

static void Player_MenuInit( void )
{
	s_main_playermdl.generic.type			= MTYPE_BITMAP;
	s_main_playermdl.generic.flags			= QMF_SILENT; //INACTIVE
	s_main_playermdl.generic.callback		= Player_SpinPlayer;
//	s_main_playermdl.generic.ownerdraw		= Player_DrawPlayer;
	s_main_playermdl.generic.x				= 82; //440 //25
	s_main_playermdl.generic.y				= 158; //95
	s_main_playermdl.width					= 164; //32*6.6 //211.2 //246.2
	s_main_playermdl.height					= 276; //56*6.6 //369.6 //404.6
	s_main_playermdl.generic.id				= ID_PLAYER_REGION;
}

static void Player_InitModel( void )
{
	memset( &s_main.playerinfo, 0, sizeof(playerInfo_t) );

	UI_PlayerInfo_SetModel( &s_main.playerinfo, UI_Cvar_VariableString( "model" ) );

	//Player_UpdateModel( ANIM_IDLE );
	VectorClear( s_main.playerViewangles );
	VectorClear( s_main.playerMoveangles );
	s_main.playerViewangles[YAW] = 200;
	s_main.playerViewangles[PITCH] = 0;
	s_main.playerViewangles[ROLL] = 0;
	s_main.playerMoveangles[YAW] = 0; //s_main.playerViewangles[YAW];
	s_main.playerLegs		     = BOTH_STAND1;
	s_main.playerWeapon			 = WP_0;
	s_main.playerTorso			 = BOTH_STAND1; //TORSO_STAND2

	s_main.playerinfo.randomEmote = qtrue;	//play some random anims hehe

	UI_PlayerInfo_SetInfo( &s_main.playerinfo, 
							s_main.playerLegs, 
							s_main.playerTorso, 
							s_main.playerViewangles,
							s_main.playerMoveangles,
							s_main.playerWeapon,
							trap_Cvar_VariableValue( "height" ), 
							trap_Cvar_VariableValue( "weight" ), 
							qfalse );
}

/*
==========================
END MOFICIATION
==========================
*/

static weapongraphics_s weapon_graphics[UI_NUM_WEAPONS] =	//was WP_NUM_WEAPONS, but we added some more for vv
{
	{ 0, -15, -25 },		// WP_5
	{ 0,  7,  -20 },		// WP_6
	{ 0,  15, -10 },		// WP_1
	{ 0,   8, -15 },		// WP_4
	{ 0,  25,   0 },		// WP_10
	{ 0,  18, -17 },		// WP_8
	{ 0,   5, -25 },		// WP_TETRYON_DISRUPTOR
	{ 0,  20, -10 },		// WP_PHOTON_BURST
	{ 0,   5, -19 }			// WP_ARCWELDER
};


/*static void UI_Draw3DModel( float x, float y, float w, float h, qhandle_t model, vec3_t origin, vec3_t angles, vec3_t weaponMidpoint ) {
	refdef_t		refdef;
	refEntity_t		ent;

	UI_AdjustFrom640( &x, &y, &w, &h );

	memset( &refdef, 0, sizeof( refdef ) );

	memset( &ent, 0, sizeof( ent ) );
	AnglesToAxis( angles, ent.axis );

	origin[0] -= 
		weaponMidpoint[0] * ent.axis[0][0] +
		weaponMidpoint[1] * ent.axis[1][0] +
		weaponMidpoint[2] * ent.axis[2][0];
	origin[1] -= 
		weaponMidpoint[0] * ent.axis[0][1] +
		weaponMidpoint[1] * ent.axis[1][1] +
		weaponMidpoint[2] * ent.axis[2][1];
	origin[2] -= 
		weaponMidpoint[0] * ent.axis[0][2] +
		weaponMidpoint[1] * ent.axis[1][2] +
		weaponMidpoint[2] * ent.axis[2][2];
	
	VectorCopy( origin, ent.origin );
	
	ent.hModel = model;
	if (trap_Cvar_VariableValue("r_dynamiclight") ) {
		ent.renderfx = RF_LOWLIGHT|RF_NOSHADOW;		// keep it dark, and no stencil shadows
	} else {
		ent.renderfx = RF_NOSHADOW;		// no stencil shadows
	}
	refdef.rdflags = RDF_NOWORLDMODEL;

	AxisClear( refdef.viewaxis );

	refdef.fov_x = 40;
	refdef.fov_y = 30;

	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	refdef.time = uis.realtime;

	trap_R_ClearScene();
	trap_R_AddRefEntityToScene( &ent );

	origin[0] -= 50;	// + = behind, - = in front
	origin[1] += 50;	// + = left, - = right
	origin[2] += 50;	// + = above, - = below
	trap_R_AddLightToScene( origin, 500, 1.0, 1.0, 1.0 );

	trap_R_RenderScene( &refdef );
}*/

/*
================
UI_DrawWeapon

================
*/
static void CacheWeaponModel(int i);
/*static void UI_DrawWeapon(void)
{
	vec3_t	origin = {50,0,2};
	vec3_t	angles;

	origin[2] = 2; //+ = up, - = down
	origin[1] = 0;
	origin[0] = 60;

	angles[PITCH] = weapon_graphics[s_main.currentWeapon].pitch;
	angles[YAW]   = 20.0/300.0 * uis.realtime; //Rotates object
	angles[ROLL]  = weapon_graphics[s_main.currentWeapon].roll;

	if (!weapon_graphics[s_main.currentWeapon].modelhandle) 
	{
		CacheWeaponModel(s_main.currentWeapon);
	}
	UI_Draw3DModel( 113, 178, 358, 244, weapon_graphics[s_main.currentWeapon].modelhandle, origin, angles, weapon_graphics[s_main.currentWeapon].mid );

	angles[PITCH] = 0;
	angles[ROLL]  = 0;
	UI_Draw3DModel( 113, 178, 358, 244, weapon_graphics[s_main.currentWeapon].modelhandle, origin, angles, weapon_graphics[s_main.currentWeapon].mid );

	//BOOKMARK - 163
	UI_DrawProportionalString( 289, 180, bg_itemlist[s_main.currentWeapon + 1].pickup_name,UI_CENTER|UI_SMALLFONT, colorTable[CT_YELLOW]);
}*/

/*
=================
M_MainMenu_Graphics
=================
*/
//BOOKMARK

//static 	qboolean hasSlashed = qfalse;

static void M_MainMenu_Graphics (void)
{
	int y,pad; //i removed
//	void**		weaponptr;
	char string[256];
	char temp[128];
	int i;
	float	scale;
	servernode_t*	node;
	int		style;

	trap_R_ClearScene();

	// Draw the basic screen layout
	UI_MenuFrame(&s_main.menu);

	UI_MainMenuButtons(99); //Ammount of left butty things //TiM heh heh.... butty ROFL

	trap_R_SetColor( colorTable[CT_LTPURPLE1] ); // END LEFT (2112)
	UI_DrawHandlePic( 460 + MENU_BUTTON_MED_WIDTH + 3 , 136,  -19,  MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);	//right

	trap_R_SetColor( colorTable[CT_DKPURPLE1]); // END LEFT (2112)
	UI_DrawHandlePic( 482, 136,  MENU_BUTTON_MED_WIDTH - 13, MENU_BUTTON_MED_HEIGHT, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_LTBROWN1]); //LEFT BAR (93433)
	UI_DrawHandlePic(30,203,  47, 88, uis.whiteShader);	// Long left column square on bottom 3rd //H: 168

	trap_R_SetColor( colorTable[CT_LTGOLD1]); //LEFT BAR (93433)
	UI_DrawHandlePic(30,295,  47, 65, uis.whiteShader);	// Long left column square on bottom 3rd

	trap_R_SetColor( colorTable[CT_DKRED1]); //LEFT BAR (93433)
	UI_DrawHandlePic(30,363,  47, 25, uis.whiteShader);	// Long left column square on bottom 3rd

	UI_DrawProportionalString(  74,  66, "7617",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "4396",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  188, "81453",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	UI_DrawProportionalString(  74,  206, "93433",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  298, "16594",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  366, "44205",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	UI_DrawProportionalString(  74,  395, "431108",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	UI_DrawProportionalString( 584, 142, "2112",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	pad = 4;
	y = 176 + pad;

	//The favorite servers bar
	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic( 596, 159,  -16, 15, uis.graphicButtonLeftEnd);	//end curve
	UI_DrawHandlePic( 252, 159, 348, 15, uis.whiteShader ); //Top Bar
	UI_DrawHandlePic( 252, 177, 8, 32, uis.whiteShader ); //Bit directly beneath
	UI_DrawHandlePic( 252, 245, 8, 32, uis.whiteShader ); //Bit beneath the dark blue bit
	
	UI_DrawHandlePic( 252, 269, 16, 32, s_main.graphic_12_8_LL ); //Curve
	UI_DrawProportionalString(  271,  284, menu_normal_text[MNT_QUICKCONNECT], UI_SMALLFONT, colorTable[CT_LTGOLD1]);
	
	//block next to the name
	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic( 271 + UI_ProportionalStringWidth( menu_normal_text[MNT_QUICKCONNECT], UI_SMALLFONT ) + 3, 283, 157 - UI_ProportionalStringWidth( menu_normal_text[MNT_QUICKCONNECT], UI_SMALLFONT ) - 3, 18, uis.whiteShader );
	//Up Arrow Box
	UI_DrawHandlePic( 431, 283, 18, 18, uis.whiteShader );
	//Dn Arrow Box
	UI_DrawHandlePic( 452, 283, 18, 18, uis.whiteShader );
	UI_DrawHandlePic( 612-19, 283,  -19,  MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	//Side Box
	UI_DrawHandlePic( 252, 212, 8, 30, uis.whiteShader );

	//Relevant text
	UI_DrawProportionalString(  265, 161, menu_normal_text[MNT_HOSTNAME],UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  400, 161, menu_normal_text[MNT_MAP],UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  485, 161, menu_normal_text[MNT_PLAYERS],UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  531, 161, menu_normal_text[MNT_TYPE],UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  573, 161, menu_normal_text[MNT_PING],UI_TINYFONT, colorTable[CT_BLACK]);

	//draw the data we have for the fave servers
	//done separately to guarantee proper formatting

	y = 177;
	pad = 13;
	style = UI_TINYFONT;
	for (i = 0; i < MAX_FAVBUTTONS; i++ ) {
		node = &s_main.networkParms.table[i+s_main.favOffset].servernode;

		if ( ( i + s_main.favOffset ) == s_main.selectedFav ) {
			//selection rectangle
			UI_FillRect( 263, y+( pad*i)-1, 332, 12, listbar_color );
		}	

		if ( node->adrstr[0] ) {
			//host name
			//Com_sprintf( temp, sizeof( temp ), S_COLOR_WHITE "%-25.25s", node->hostname );
			Com_sprintf( temp, sizeof( temp ), S_COLOR_WHITE "%s", node->hostname );
			
			//cap the end of the string if it's too long
			if ( strlen(temp) > 26 )
			{
				temp[26]='\0';
				Q_strcat( temp, 30, "..." );
			}
			UI_DrawProportionalString( 265, y + (pad*i), temp, style, colorTable[CT_WHITE] );
		
			//map name
			Com_sprintf( temp, sizeof( temp ), S_COLOR_WHITE "%s", node->mapname );

			//cap the end of the string if it's too long
			if ( strlen(temp) > 17 )
			{
				temp[17]='\0';
				Q_strcat( temp, 30, "..." );
			}
			UI_DrawProportionalString( 400, y + (pad*i), temp, style, colorTable[CT_WHITE] );

			//players
			Com_sprintf( temp, sizeof( temp ), S_COLOR_WHITE "%3i/%3i", node->numclients, node->maxclients );
			UI_DrawProportionalString( 485, y + (pad*i), temp, style, colorTable[CT_WHITE] );

			//server type
			Com_sprintf( temp, sizeof( temp ), S_COLOR_WHITE "%-8.8s", node->gamename );
			UI_DrawProportionalString( 531, y + (pad*i), temp, style, colorTable[CT_WHITE] );

			//pingzor
			Com_sprintf( temp, sizeof( temp ), S_COLOR_WHITE "%3i", node->pingtime );
			if ( !node->isPure )
				strcat( temp, "*" );
			UI_DrawProportionalString( 573, y + (pad*i), temp, style, colorTable[CT_WHITE] );
		}
	}

	//Text
	y = 304;
	pad = 23;
	UI_DrawProportionalString(  271,  y+1, menu_normal_text[MNT_PLAYERSTATS],UI_SMALLFONT, colorTable[CT_LTGOLD1]);
	
	//player name
	y += pad;
	Com_sprintf( string, sizeof( string ), "%s: %s", menu_normal_text[MNT_NAME], UI_Cvar_VariableString("name") );
	if ( strlen( string ) > 45 )
	{
		string[45] = '\0';
		strcat( string, "..." );
	}
	UI_DrawProportionalString(	270,  y, string, UI_LEFT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);
	
	//TiM - no way of knowing which class they're in coz of the password system
	Q_strncpyz( temp, UI_Cvar_VariableString("ui_playerClass"), sizeof( temp ) );
	//uppercase the first letter
	if ( temp[0] >= 'a' && temp[0] <= 'z') //if the first character is lower case 
		temp[0] -= 32; // make it capital

	y+=pad;
	Com_sprintf( string, sizeof( string ), "%s: %s", menu_normal_text[MNT_CLASS], temp );
	if ( strlen( string ) > 20 )
	{
		string[20] = '\0';
		strcat( string, "..." );
	}
	UI_DrawProportionalString(  270,  y, string, UI_LEFT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);

	scale = trap_Cvar_VariableValue( "height" ) * (float)BASE_HEIGHT;

	Com_sprintf( string, sizeof( string ), "%s: %3.2f%s", menu_normal_text[MNT_HEIGHT], scale, HEIGHT_UNIT );
	if ( strlen( string ) > 20 )
	{
		string[20] = '\0';
		strcat( string, "..." );
	}
	UI_DrawProportionalString(  432,  y, string, UI_LEFT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);

	//rank
	y+=pad;
	//retrieve the rank from our loaded rank data
	if ( uis.rankSet.rankNames[uis.currentRank].formalName[0] )
	{
		Com_sprintf( string, sizeof( string ), "%s: %s", menu_normal_text[MNT_RANK], uis.rankSet.rankNames[uis.currentRank].formalName );
	}
	else //failing that... load the playerrank CVAR
	{
		char rank[36];
		Q_strncpyz( rank, UI_Cvar_VariableString( "ui_playerrank" ), 36 );

		//validate we got a value
		if ( !rank[0] )
			Q_strncpyz( rank, "Unknown", 36 );
		else if ( rank[0] >= 'a' && rank[0] <= 'z' )
			rank[0] -= 32; //make first char upper case if need be

		Com_sprintf( string, sizeof( string ), "%s: %s", menu_normal_text[MNT_RANK], rank );
	}
	
	//TiM - Clip it to prevent overlapping
	if ( strlen( string ) > 20 )
	{
		string[20] = '\0';
		strcat( string, "..." );
	}
	UI_DrawProportionalString(  270,  y, string, UI_LEFT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);

	scale = trap_Cvar_VariableValue( "height" ) * trap_Cvar_VariableValue( "weight" ) * (float)BASE_WEIGHT;
	if ( s_main.playerinfo.gender == GENDER_FEMALE )
		scale *= (float)FEMALE_OFFSET;

	Com_sprintf( string, sizeof( string ), "%s: %3.2f%s", menu_normal_text[MNT_WEIGHT], scale, WEIGHT_UNIT );
	if ( strlen( string ) > 20 )
	{
		string[20] = '\0';
		strcat( string, "..." );
	}
	UI_DrawProportionalString(  432,  y, string, UI_LEFT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);

	y+=pad;
	Com_sprintf( string, sizeof(string), "%s: %s", menu_normal_text[MNT_AGE], UI_Cvar_VariableString("age") );
	if ( strlen( string ) > 20 )
	{
		string[20] = '\0';
		strcat( string, "..." );
	}
	UI_DrawProportionalString(  270,  y, string, UI_LEFT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);

	Com_sprintf( string, sizeof(string), "%s: %s", menu_normal_text[MNT_RACE], UI_Cvar_VariableString("race") );
	//TiM - Clip it to prevent overlapping
	if ( strlen( string ) > 20 )
	{
		string[20] = '\0';
		strcat( string, "..." );
	}
	UI_DrawProportionalString(  432,  y, string, UI_LEFT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);

	// Grid over top of space map
	trap_R_SetColor( colorTable[CT_LTBLUE1]);
	UI_DrawHandlePic(  81, 228, 165,   1, uis.whiteShader); //296
	UI_DrawHandlePic(  83, 293, 161,   1, uis.whiteShader);
	UI_DrawHandlePic(  81, 365, 165,   1, uis.whiteShader);
	//UI_DrawHandlePic( 83, 169,   1, 256, uis.whiteShader);
	UI_DrawHandlePic( 136, 162,   1, 266, uis.whiteShader); //132
	UI_DrawHandlePic( 186, 162,   1, 266, uis.whiteShader); //181

	Player_DrawPlayer(); //RPG-X HACK - Draw the model now, so it's over the grid, yet under the brackets :)

	// bracket around the buttons
	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic( 252, 304, 16, 32, s_main.graphic_12_8_LU );
	UI_DrawHandlePic( 252, 402, 16, 32, s_main.graphic_12_8_LL );
	UI_DrawHandlePic( 595, 304, 16, 32, s_main.graphic_12_8_RU );
	UI_DrawHandlePic( 595, 402, 16, 32, s_main.graphic_12_8_RL );

	UI_DrawHandlePic( 252, 328, 8, 20, uis.whiteShader ); //25 //Left upper  rect
	UI_DrawHandlePic( 252, 388, 8, 22, uis.whiteShader ); //Left lower rect

	UI_DrawHandlePic( 603, 328, 8, 20, uis.whiteShader ); //right up
	UI_DrawHandlePic( 603, 388, 8, 22, uis.whiteShader ); //right down

	UI_DrawHandlePic( (3 + 271 + (UI_ProportionalStringWidth(menu_normal_text[MNT_PLAYERSTATS],UI_SMALLFONT))), 304, ((321 - (UI_ProportionalStringWidth(menu_normal_text[MNT_PLAYERSTATS],UI_SMALLFONT))) - 3), 18, uis.whiteShader );

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic( 406, 416, 51, 18, uis.whiteShader ); //main bars along the bottom //257

	UI_DrawHandlePic( 252, 351, 8, 34, uis.whiteShader ); //dark blue blocks
	UI_DrawHandlePic( 603, 351, 8, 34, uis.whiteShader );

	//Left Bracket around model picture
	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(81,158, 16, 16, uis.graphicBracket1CornerLU);
	UI_DrawHandlePic(81,174,  8, 94, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(81,271,  8, 11, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_LTORANGE]);
	UI_DrawHandlePic(83,285,  6, 21, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(81,310,  8, 11, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(81,324,  8, 94, uis.whiteShader);
	UI_DrawHandlePic(81,418, 16, -16, uis.graphicBracket1CornerLU);	//LD

	//Right Bracket around model picture
	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(230,158, -16, 16, uis.graphicBracket1CornerLU); //375	//RU -200
	UI_DrawHandlePic(238,174,  8, 94, uis.whiteShader); //383

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(238,271,  8, 11, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_LTORANGE]);
	UI_DrawHandlePic(238,285,  6, 21, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(238,310,  8, 11, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(238,324,  8, 94, uis.whiteShader);
	UI_DrawHandlePic(230,418, -16, -16, uis.graphicBracket1CornerLU); //375	//RD

	if ( s_main.networkParms.numservers == 0 && !s_main.networkParms.refreshservers ) {
		UI_DrawProportionalString( 431, 199, menu_normal_text[MNT_QUICKMESSAGE], UI_TINYFONT|UI_CENTER, colorTable[CT_RED] );
		UI_DrawProportionalString( 431, 215, menu_normal_text[MNT_QUICKMESSAGE2], UI_TINYFONT|UI_CENTER, colorTable[CT_RED] );
	}

	if ( s_main.networkParms.showNoServersAlert )
		UI_DrawProportionalString( 431, 247, menu_normal_text[MNT_NO_SERVERS], UI_TINYFONT|UI_CENTER, colorTable[CT_RED] );

	// Frame around model pictures
	trap_R_SetColor( colorTable[CT_LTORANGE]);
}


/*
=================
Main_MenuDraw
=================
*/
static void Main_MenuDraw (void)
{
	if ( s_main.networkParms.refreshservers == qtrue ) {
		MainArenaServers_DoRefresh();
	}

	// Draw graphics particular to Main Menu
	M_MainMenu_Graphics();

	Menu_Draw( &s_main.menu );

	//UI_drawRPGXLogo();
}

static void CacheWeaponModel(int i)
{
//	vec3_t			mins, maxs;
	//weapon_graphics[i].modelhandle = trap_R_RegisterModel( weapon_graphics[i].modelname);
	
	// offset the origin y and z to center the model
	//trap_R_ModelBounds( weapon_graphics[i].modelhandle, mins, maxs );
	//weapon_graphics[i].mid[0] = mins[0] + 0.5 * ( maxs[0] - mins[0]);
	//weapon_graphics[i].mid[1] = mins[1] + 0.5 * ( maxs[1] - mins[1]);
	//weapon_graphics[i].mid[2] = mins[2] + 0.5 * ( maxs[2] - mins[2]);
}
/*
===============
MainMenu_Cache
===============
*/
//extern vmCvar_t	ui_precacheweapons;
void MainMenu_Cache( void ) 
{
	int i;
	gitem_t *	item;

	i = 0;
	for ( item = bg_itemlist + 1 ; item->classname && (i < UI_NUM_WEAPONS) ; item++ ) 
	{
		if ( item->giType != IT_WEAPON ) 
		{
			continue;
		}

		weapon_graphics[i].modelname = item->world_model;
		CacheWeaponModel(i);
		i++;
	}

	/*for (i=0;i<UI_NUM_WEAPONS;i++)
	{
		s_main.weaponnamesnd[i] = trap_S_RegisterSound(weaponnamefiles[i]);
	}*/

	// Precache all menu graphics in array - Makes the graphics stay
	for (i=0;i<AMG_MAX;++i)
	{
		if (attackmenu_graphics1[i].type == MG_GRAPHIC)
		{
			attackmenu_graphics1[i].graphic = trap_R_RegisterShaderNoMip(attackmenu_graphics1[i].file);
		}
	}

	s_main.logoModel = trap_R_RegisterModel( MAIN_LOGO_MODEL );

	//RPG-X - TiM: main menu additional lcars shapes cache
	s_main.graphic_12_8_LU = trap_R_RegisterShaderNoMip( "menu/common/corner_ul_8_12.tga" );
	s_main.graphic_12_8_LL	= trap_R_RegisterShaderNoMip( "menu/common/corner_ll_8_12.tga" );
	s_main.graphic_12_8_RU = trap_R_RegisterShaderNoMip( "menu/common/corner_ur_8_12.tga" );
	s_main.graphic_12_8_RL	= trap_R_RegisterShaderNoMip( "menu/common/corner_lr_8_12.tga" );

	s_main.graphic_16_12_LU	= trap_R_RegisterShaderNoMip( "menu/common/corner_ul_16_12.tga" );
	s_main.graphic_16_18_LL	= trap_R_RegisterShaderNoMip( "menu/common/corner_ll_16_18.tga" );

	//s_main.soundGrid		= trap_R_RegisterShaderNoMip("menu/special/grid.tga");
	//s_main.soundWaveform	= trap_R_RegisterShaderNoMip("menu/special/wave3.tga");

	s_main.activateSound	= trap_S_RegisterSound( "sound/interface/button2.wav" );

}

/*
===============
UI_MainMenu_Init
===============
*/
static void UI_MainMenu_Init(void)
{
	int i;

	memset( &s_main, 0, sizeof(mainmenu_t) );

	MainMenu_Cache();
	Player_MenuInit();

	//Spin Model init
	uis.spinView = qfalse;
	uis.lastYaw = 200;

	//Set selected favs to NULL by default
	s_main.selectedFav = -1;

	//RPG-X Phenix/J2J
	// BOOKMARK s_main.rpgxlogo = trap_R_RegisterModel( //
	
	s_main.menu.draw					= Main_MenuDraw;
	s_main.menu.fullscreen				= qtrue;
	s_main.menu.wrapAround				= qtrue;
	s_main.menu.descX					= MENU_DESC_X;
	s_main.menu.descY					= MENU_DESC_Y;	
	s_main.menu.titleX					= MENU_TITLE_X;
	s_main.menu.titleY					= MENU_TITLE_Y;
	s_main.menu.titleI					= MNT_MAINMENU_TITLE;
	s_main.menu.footNoteEnum			= MNT_HOLOMATCHWEAPONRY;


	s_main.multiplayer.generic.type		= MTYPE_BITMAP;      
	s_main.multiplayer.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
	s_main.multiplayer.generic.x		= mm_buttons[0][0];
	s_main.multiplayer.generic.y		= mm_buttons[0][1];
	s_main.multiplayer.generic.name		= BUTTON_GRAPHIC_LONGRIGHT;
	s_main.multiplayer.generic.id		= ID_MULTIPLAYER;
	s_main.multiplayer.generic.callback	= Main_MenuEvent; 
	s_main.multiplayer.width			= MENU_BUTTON_MED_WIDTH;
	s_main.multiplayer.height			= MENU_BUTTON_MED_HEIGHT;
	// COLOUR
	s_main.multiplayer.color			= CT_DKPURPLE1;
	s_main.multiplayer.color2			= CT_LTPURPLE1;
	s_main.multiplayer.textX			= MENU_BUTTON_TEXT_X;
	s_main.multiplayer.textY			= MENU_BUTTON_TEXT_Y;
	s_main.multiplayer.textEnum			= MBT_MULTIPLAYER;
	s_main.multiplayer.textcolor		= CT_BLACK;
	s_main.multiplayer.textcolor2		= CT_BLACK; //CT_WHITE;

	s_main.setup.generic.type		= MTYPE_BITMAP;      
	s_main.setup.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
	s_main.setup.generic.x		= mm_buttons[1][0];
	s_main.setup.generic.y		= mm_buttons[1][1];
	s_main.setup.generic.name		= BUTTON_GRAPHIC_LONGRIGHT;
	s_main.setup.generic.id		= ID_SETUP;
	s_main.setup.generic.callback	= Main_MenuEvent; 
	s_main.setup.width			= MENU_BUTTON_MED_WIDTH;
	s_main.setup.height			= MENU_BUTTON_MED_HEIGHT;
	// COLOUR
	s_main.setup.color			= CT_DKPURPLE1;
	s_main.setup.color2			= CT_LTPURPLE1;
	s_main.setup.textX			= MENU_BUTTON_TEXT_X;
	s_main.setup.textY			= MENU_BUTTON_TEXT_Y;
	s_main.setup.textEnum			= MBT_SETUP;
	s_main.setup.textcolor		= CT_BLACK;
	s_main.setup.textcolor2		= CT_BLACK; //CT_WHITE;

	s_main.demo.generic.type			= MTYPE_BITMAP;      
	s_main.demo.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_main.demo.generic.x				= mm_buttons[2][0];
	s_main.demo.generic.y				= mm_buttons[2][1];
	s_main.demo.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	s_main.demo.generic.id				= ID_DEMOS;
	s_main.demo.generic.callback		= Main_MenuEvent; 
	s_main.demo.width					= MENU_BUTTON_MED_WIDTH;
	s_main.demo.height					= MENU_BUTTON_MED_HEIGHT;
	// COLOUR
	s_main.demo.color					= CT_DKPURPLE1;
	s_main.demo.color2					= CT_LTPURPLE1;
	s_main.demo.textX					= MENU_BUTTON_TEXT_X;
	s_main.demo.textY					= MENU_BUTTON_TEXT_Y;
	s_main.demo.textEnum				= MBT_DEMO;
	s_main.demo.textcolor				= CT_BLACK;
	s_main.demo.textcolor2		= CT_BLACK; //CT_WHITE;
		if (uis.demoversion)
	{
		s_main.demo.generic.flags			|= QMF_GRAYED;
	}

	s_main.mods.generic.type			= MTYPE_BITMAP;      
	s_main.mods.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_main.mods.generic.x				= mm_buttons[3][0];
	s_main.mods.generic.y				= mm_buttons[3][1];
	s_main.mods.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	s_main.mods.generic.id				= ID_MODS;
	s_main.mods.generic.callback		= Main_MenuEvent; 
	s_main.mods.width					= MENU_BUTTON_MED_WIDTH;
	s_main.mods.height					= MENU_BUTTON_MED_HEIGHT;
	// COLOUR
	s_main.mods.color					= CT_DKPURPLE1;
	s_main.mods.color2					= CT_LTPURPLE1;
	s_main.mods.textX					= MENU_BUTTON_TEXT_X;
	s_main.mods.textY					= MENU_BUTTON_TEXT_Y;
	s_main.mods.textEnum				= MBT_MODS;
	s_main.mods.textcolor				= CT_BLACK;
	s_main.mods.textcolor2		= CT_BLACK; //CT_WHITE;
	if (uis.demoversion)
	{
		s_main.mods.generic.flags			|= QMF_GRAYED;
	}

	s_main.credits.generic.type			= MTYPE_BITMAP;
	s_main.credits.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_main.credits.generic.x				= mm_buttons[4][0];
	s_main.credits.generic.y				= mm_buttons[4][1];
	s_main.credits.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	s_main.credits.generic.id				= ID_CREDITS;
	s_main.credits.generic.callback		= Main_MenuEvent; 
	s_main.credits.width					= MENU_BUTTON_MED_WIDTH;
	s_main.credits.height					= MENU_BUTTON_MED_HEIGHT;
	// COLOUR
	s_main.credits.color					= CT_DKPURPLE1;
	s_main.credits.color2					= CT_LTPURPLE1;
	s_main.credits.textX					= MENU_BUTTON_TEXT_X;
	s_main.credits.textY					= MENU_BUTTON_TEXT_Y;
	s_main.credits.textEnum				= MBT_CREDITS;
	s_main.credits.textcolor				= CT_BLACK;
	s_main.credits.textcolor2		= CT_BLACK; //CT_WHITE;


	s_main.quit.generic.type			= MTYPE_BITMAP;      
	s_main.quit.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_main.quit.generic.x				= mm_buttons[5][0];
	s_main.quit.generic.y				= mm_buttons[5][1];
	s_main.quit.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	s_main.quit.generic.id				= ID_QUIT;
	s_main.quit.generic.callback		= Main_MenuEvent; 
	s_main.quit.width					= MENU_BUTTON_MED_WIDTH;
	s_main.quit.height					= MENU_BUTTON_MED_HEIGHT;
	// COLOUR
	s_main.quit.color					= CT_VDKRED1; //CT_DKPURPLE1;
	s_main.quit.color2					= CT_DKRED1; //CT_LTPURPLE1;
	s_main.quit.textX					= MENU_BUTTON_TEXT_X;
	s_main.quit.textY					= MENU_BUTTON_TEXT_Y;
	s_main.quit.textEnum				= MBT_GAMEQUIT;
	s_main.quit.textcolor				= CT_BLACK;
	s_main.quit.textcolor2		= CT_BLACK; //CT_WHITE;

	s_main.mission.generic.type			= MTYPE_BITMAP;      
	s_main.mission.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_main.mission.generic.x				= 30;
	s_main.mission.generic.y				= 161;
	s_main.mission.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	s_main.mission.generic.id				= ID_DEMOS;
	s_main.mission.generic.callback		= Main_MenuEvent; 
	s_main.mission.width					= MENU_BUTTON_MED_WIDTH;
	s_main.mission.height					= MENU_BUTTON_MED_HEIGHT;
	// COLOUR
	s_main.mission.color					= CT_DKPURPLE1;
	s_main.mission.color2					= CT_LTPURPLE1;
	s_main.mission.textX					= MENU_BUTTON_TEXT_X;
	s_main.mission.textY					= MENU_BUTTON_TEXT_Y;
	s_main.mission.textEnum				= MBT_MISSION;
	s_main.mission.textcolor				= CT_BLACK;
	s_main.mission.textcolor2		= CT_BLACK; //CT_WHITE;

	s_main.playerSettings.generic.type			= MTYPE_BITMAP;      
	s_main.playerSettings.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS; 
	s_main.playerSettings.generic.x				= 271;
	s_main.playerSettings.generic.y				= 416;
	s_main.playerSettings.generic.callback		= Main_MenuEvent;
	s_main.playerSettings.generic.id			= ID_PLAYER_SETTINGS;
	s_main.playerSettings.generic.name			= GRAPHIC_SQUARE;
	s_main.playerSettings.width					= 132;
	s_main.playerSettings.height				= MENU_BUTTON_MED_HEIGHT;
	s_main.playerSettings.color					= CT_DKPURPLE1;
	s_main.playerSettings.color2				= CT_LTPURPLE1;
	s_main.playerSettings.textX					= MENU_BUTTON_TEXT_X;
	s_main.playerSettings.textY					= MENU_BUTTON_TEXT_Y;
	s_main.playerSettings.textEnum				= MBT_PLAYERDATA;
	s_main.playerSettings.textcolor				= CT_BLACK;
	s_main.playerSettings.textcolor2			= CT_WHITE;

	s_main.playerModel.generic.type				= MTYPE_BITMAP;      
	s_main.playerModel.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS; 
	s_main.playerModel.generic.x				= 460;
	s_main.playerModel.generic.y				= 416;
	s_main.playerModel.generic.callback			= Main_MenuEvent;
	s_main.playerModel.generic.id				= ID_PLAYER_MODEL;
	s_main.playerModel.generic.name				= GRAPHIC_SQUARE;
	s_main.playerModel.width					= 132;
	s_main.playerModel.height					= MENU_BUTTON_MED_HEIGHT;
	s_main.playerModel.color					= CT_DKPURPLE1;
	s_main.playerModel.color2					= CT_LTPURPLE1;
	s_main.playerModel.textX					= MENU_BUTTON_TEXT_X;
	s_main.playerModel.textY					= MENU_BUTTON_TEXT_Y;
	s_main.playerModel.textEnum					= MBT_CHANGEMODEL;
	s_main.playerModel.textcolor				= CT_BLACK;
	s_main.playerModel.textcolor2				= CT_WHITE;

	s_main.upArrow.generic.type					= MTYPE_BITMAP;
	s_main.upArrow.generic.flags				= QMF_INACTIVE | QMF_GRAYED;
	s_main.upArrow.generic.x					= 432;
	s_main.upArrow.generic.y					= 286;
	s_main.upArrow.generic.name					= PIC_ARROW_UP;
	s_main.upArrow.generic.id					= ID_LIST_UP;
	s_main.upArrow.generic.callback				= Main_MenuEvent;
	s_main.upArrow.width  						= 16;
	s_main.upArrow.height  						= 16;
	s_main.upArrow.color						= CT_DKPURPLE1;
	s_main.upArrow.color2						= CT_LTPURPLE1;
	s_main.upArrow.textcolor					= CT_BLACK;
	s_main.upArrow.textcolor2					= CT_WHITE;

	s_main.dnArrow.generic.type					= MTYPE_BITMAP;
	s_main.dnArrow.generic.flags				= QMF_INACTIVE | QMF_GRAYED;
	s_main.dnArrow.generic.x					= 453;
	s_main.dnArrow.generic.y					= 286;
	s_main.dnArrow.generic.name					= PIC_ARROW_DOWN;
	s_main.dnArrow.generic.id					= ID_LIST_DOWN;
	s_main.dnArrow.generic.callback				= Main_MenuEvent;
	s_main.dnArrow.width  						= 16;
	s_main.dnArrow.height  						= 16;
	s_main.dnArrow.color						= CT_DKPURPLE1;
	s_main.dnArrow.color2						= CT_LTPURPLE1;
	s_main.dnArrow.textcolor					= CT_BLACK;
	s_main.dnArrow.textcolor2					= CT_WHITE;

	s_main.refresh.generic.type					= MTYPE_BITMAP;      
	s_main.refresh.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS; 
	s_main.refresh.generic.x					= 473;
	s_main.refresh.generic.y					= 283;
	s_main.refresh.generic.callback				= Main_MenuEvent;
	s_main.refresh.generic.id					= ID_REFRESH;
	s_main.refresh.generic.name					= GRAPHIC_SQUARE;
	s_main.refresh.width						= 63;
	s_main.refresh.height						= MENU_BUTTON_MED_HEIGHT;
	s_main.refresh.color						= CT_DKPURPLE1;
	s_main.refresh.color2						= CT_LTPURPLE1;
	s_main.refresh.textX						= MENU_BUTTON_TEXT_X;
	s_main.refresh.textY						= MENU_BUTTON_TEXT_Y;
	s_main.refresh.textEnum						= MBT_REFRESH;
	s_main.refresh.textcolor					= CT_BLACK;
	s_main.refresh.textcolor2					= CT_WHITE;

	s_main.connect.generic.type					= MTYPE_BITMAP;      
	s_main.connect.generic.flags				= QMF_GRAYED | QMF_INACTIVE; 
	s_main.connect.generic.x					= 539;
	s_main.connect.generic.y					= 283;
	s_main.connect.generic.callback				= Main_MenuEvent;
	s_main.connect.generic.id					= ID_CONNECT;
	s_main.connect.generic.name					= GRAPHIC_SQUARE;
	s_main.connect.width						= 58;
	s_main.connect.height						= MENU_BUTTON_MED_HEIGHT;
	s_main.connect.color						= CT_DKPURPLE1;
	s_main.connect.color2						= CT_LTPURPLE1;
	s_main.connect.textX						= MENU_BUTTON_TEXT_X;
	s_main.connect.textY						= MENU_BUTTON_TEXT_Y;
	s_main.connect.textEnum						= MBT_ENGAGEMULTIPLAYER;
	s_main.connect.textcolor					= CT_BLACK;
	s_main.connect.textcolor2					= CT_WHITE;

	for ( i = 0; i < MAX_FAVBUTTONS; i++ ) {
		s_main.favMenu[i].generic.type			= MTYPE_BITMAP;
		s_main.favMenu[i].generic.flags			= QMF_SILENT | QMF_HIDDEN | QMF_INACTIVE;
		s_main.favMenu[i].generic.x				= 263;
		s_main.favMenu[i].generic.y				= 176 + ( 13 * i );
		s_main.favMenu[i].generic.callback		= Main_MenuEvent;
		s_main.favMenu[i].generic.id			= ID_FAV0+i;
		s_main.favMenu[i].width					= 332;
		s_main.favMenu[i].height				= 12;
	}

	Menu_AddItem( &s_main.menu,	&s_main.multiplayer );
	Menu_AddItem( &s_main.menu,	&s_main.setup );
	Menu_AddItem( &s_main.menu,	&s_main.demo );
	Menu_AddItem( &s_main.menu,	&s_main.mods ); 
	Menu_AddItem( &s_main.menu,	&s_main.credits );
	Menu_AddItem( &s_main.menu,	&s_main.quit );

	for ( i = 0; i < MAX_FAVBUTTONS; i++ )
		Menu_AddItem( &s_main.menu, &s_main.favMenu[i] );

	Menu_AddItem( &s_main.menu, &s_main.upArrow );
	Menu_AddItem( &s_main.menu, &s_main.dnArrow );
	Menu_AddItem( &s_main.menu, &s_main.refresh );
	Menu_AddItem( &s_main.menu, &s_main.connect );

	Menu_AddItem( &s_main.menu, &s_main.playerSettings);  
	Menu_AddItem( &s_main.menu, &s_main.playerModel );
            
	s_main.currentWeapon = 0;

	Menu_AddItem( &s_main.menu, ( void * )&s_main_playermdl);

	// intialize the model
	Player_InitModel();

	uis.menusp = 0;
	UI_PushMenu ( &s_main.menu );

	if ( s_HolomatchInmenu.menu.initialized )
		trap_S_StartLocalSound( s_main.activateSound, CHAN_LOCAL );

}


/*
===============
UI_MainMenu

The main menu only comes up when not in a game,
so make sure that the attract loop server is down
and that local cinematics are killed
===============
*/
void UI_MainMenu( void ) 
{

	trap_Cvar_Set( "sv_killserver", "1" );
	ingameFlag = qfalse;	// true when ingame menu is in use

//#ifdef NDEBUG
	if (!s_HolomatchInmenu.menu.initialized)	// Haven't played Holomatch In menu yet
	{
		UI_HolomatchIn_Menu();
		return;
	}
//#endif

	Mouse_Show();

	// Get CD Key if it's never been checked before
	if (!uis.demoversion && !trap_Cvar_VariableValue( "ui_cdkeychecked" ))
	{
		UI_CDKeyMenu2();
		return;
	}

	if (!trap_Cvar_VariableValue( "ui_initialsetup" ))
	{
		UI_InitialSetupMenu();
		return;
	}


	UI_MainMenu_Init();
	//trap_Cvar_Set ("rpg_playIntro", "1"); Didnt work
	

	trap_Key_SetCatcher( KEYCATCH_UI );

}

/*
=======================================================================

LCARS IN MENU

=======================================================================
*/

/*
=================
M_HolomatchIn_Key
=================
*/
static sfxHandle_t M_HolomatchIn_Key (int key)
{
	// Advance to Main Menu
	if ((key == K_SPACE) || (key == K_ENTER) || (key == K_ESCAPE) || (key == K_MOUSE1))
	{
		UI_PopMenu();	// Get rid of whatever is ontop
		UI_MainMenu();
	}
	return ( menu_out_sound );
}

/*
===============
HolomatchInMenu_Draw
===============
*/
static void HolomatchInMenu_Draw(void)
{
	int len,x,y;

	x = 30;
	y = 15;

	// Top line
	UI_DrawProportionalString(x, y, menu_normal_text[MNT_HOLOMATCH], UI_BIGFONT, colorTable[CT_LTGOLD1]);
	trap_R_SetColor( colorTable[CT_LTORANGE]);
	len = UI_ProportionalStringWidth( menu_normal_text[MNT_HOLOMATCH],UI_BIGFONT);
	UI_DrawHandlePic( x + len + 6, y, 580 - (x + len + 6),  22, uis.whiteShader);
	UI_DrawHandlePic( 584 , y,  32,  32, s_HolomatchInmenu.halfround);


	trap_R_SetColor( colorTable[CT_WHITE]);
	UI_DrawHandlePic( 175, 63, 316, 316, s_HolomatchInmenu.logo );
	//UI_DrawHandlePic( 160, 62,  420,  420, s_HolomatchInmenu.logo);

	UI_DrawProportionalString(320, 380, menu_normal_text[MNT_WELCOMETOHOLMATCH], UI_BIGFONT | UI_CENTER, colorTable[CT_LTGOLD1]);
//	UI_DrawProportionalString(320, 360, menu_normal_text[MNT_ACCESSINGLCARS], UI_BIGFONT | UI_CENTER, colorTable[CT_WHITE]);
	UI_DrawProportionalString(320, 450, menu_normal_text[MNT_PARAMOUNT_LEGAL], UI_TINYFONT | UI_CENTER, colorTable[CT_MDGREY]);
	UI_DrawProportionalString(320, 465, menu_normal_text[MNT_ID_LEGAL], UI_TINYFONT | UI_CENTER, colorTable[CT_MDGREY]);

	// Bottom line
	UI_DrawProportionalString(610, 422, menu_normal_text[MNT_HOLODECK1], UI_SMALLFONT | UI_RIGHT, colorTable[CT_LTGOLD1]);
	trap_R_SetColor( colorTable[CT_LTORANGE]);
	len = UI_ProportionalStringWidth( menu_normal_text[MNT_HOLODECK1],UI_SMALLFONT);
	UI_DrawHandlePic( x, 420, (610 - (len + 4)) - x,  18, uis.whiteShader);

	UI_DrawProportionalString(320, 450, menu_normal_text[MNT_PARAMOUNT_LEGAL], UI_TINYFONT | UI_CENTER, colorTable[CT_MDGREY]);
	UI_DrawProportionalString(320, 465, menu_normal_text[MNT_ID_LEGAL], UI_TINYFONT | UI_CENTER, colorTable[CT_MDGREY]);

	if (federationTimer < uis.realtime)
	{
		
		UI_PopMenu();	// Get rid of whatever is ontop
		UI_MainMenu();
	}
}

/*
===============
HolomatchInMenu_Cache
===============
*/
void UI_HolomatchInMenu_Cache (void)
{
	s_HolomatchInmenu.inSound = trap_S_RegisterSound( "sound/voice/computer/rpg-x/welcome.wav" );
	s_HolomatchInmenu.logo = trap_R_RegisterShaderNoMip( "menu/special/rpgx_logo.tga" ); //menu/special/holo_logo.tga
	s_HolomatchInmenu.halfround = trap_R_RegisterShaderNoMip( "menu/common/halfroundr_22.tga" );
}

void UI_PlayIntroCinematic( void )
{
	if ( !uis.playCinematic ) { //&& ( trap_Milliseconds() <=5000 ) 
			uis.playCinematic = qtrue;	//FIXME: Duhhh I'm a stupid function that completely ignores the value of uis.playCinematic O_o						  
			//Com_Printf( "qtrue" );								  
			trap_Cmd_ExecuteText( EXEC_APPEND, "wait 5; wait 5; cinematic rpgx_intro.roq \n" ); 
	}	
}

/*
===============
HolomatchInMenu_Init
===============
*/
static void HolomatchInMenu_Init(void) 
{
	UI_HolomatchInMenu_Cache();

	s_HolomatchInmenu.menu.nitems					= 0;
	s_HolomatchInmenu.menu.wrapAround				= qtrue;
	s_HolomatchInmenu.menu.draw						= HolomatchInMenu_Draw;
	s_HolomatchInmenu.menu.key						= M_HolomatchIn_Key;
	s_HolomatchInmenu.menu.fullscreen				= qtrue;
	s_HolomatchInmenu.menu.descX					= 0;
	s_HolomatchInmenu.menu.descY					= 0;
	s_HolomatchInmenu.menu.titleX					= 0;
	s_HolomatchInmenu.menu.titleY					= 0;
	s_HolomatchInmenu.menu.titleI					= MNT_NONE;

	s_HolomatchInmenu.menu.initialized = qtrue;

	//trap_S_StartLocalSound( s_HolomatchInmenu.inSound,CHAN_MENU1);

	federationTimer = uis.realtime + 12000; //12000

	//if (trap_Cvar_VariableValue ("rpg_playIntro") == 1) { trap_Cvar_Set ("rpg_playIntro", "0"); UI_PlayIntroCinematic(); }


}

/*
===============
UI_HolomatchIn_Menu
===============
*/
static void UI_HolomatchIn_Menu(void)
{

	Mouse_Hide();

	HolomatchInMenu_Init(); 

	UI_PushMenu ( &s_HolomatchInmenu.menu );

	trap_Key_SetCatcher( KEYCATCH_UI );

}

/*
=======================================================================

LCARS OUT MENU

=======================================================================
*/

/*
=================
M_LCARSOut_Key
=================
*/
static sfxHandle_t M_Leaving_Key (int key)
{
	// Get out now!!
	if ((key == K_SPACE) || (key == K_ENTER) || (key == K_ESCAPE) || (key == K_MOUSE1))
	{
		federationTimer = 0;	// Get out of game now!!!
		return(0);
	}

	return ( Menu_DefaultKey( &s_leaving_menu.menu, key ) );
}

/*
===============
LeavingMenu_Draw
===============
*/
static void LeavingMenu_Draw(void)
{
	int len,x,y;

	x = 30;
	y = 15;

	// Top line
	UI_DrawProportionalString(x, y, menu_normal_text[MNT_HOLOMATCH], UI_BIGFONT, colorTable[CT_LTGOLD1]);
	trap_R_SetColor( colorTable[CT_LTORANGE]);
	len = UI_ProportionalStringWidth( menu_normal_text[MNT_HOLOMATCH],UI_BIGFONT);
	UI_DrawHandlePic( x + len + 6, y, 580 - (x + len + 6),  22, uis.whiteShader);
	UI_DrawHandlePic( 584 , y,  32,  32, s_leaving_menu.halfround);


	trap_R_SetColor( colorTable[CT_WHITE]);
	UI_DrawHandlePic( 170, 80,  280,  280, s_leaving_menu.logo);

	UI_DrawProportionalString(320, 360, menu_normal_text[MNT_CLOSINGLCARS], UI_BIGFONT | UI_CENTER, colorTable[CT_WHITE]);

	// Bottom line
	UI_DrawProportionalString(610, 422, menu_normal_text[MNT_HOLODECK1], UI_SMALLFONT | UI_RIGHT, colorTable[CT_LTGOLD1]);
	trap_R_SetColor( colorTable[CT_LTORANGE]);
	len = UI_ProportionalStringWidth( menu_normal_text[MNT_HOLODECK1],UI_SMALLFONT);
	UI_DrawHandlePic( x, 420, (610 - (len + 4)) - x,  18, uis.whiteShader);

	UI_DrawProportionalString(320, 450, menu_normal_text[MNT_PARAMOUNT_LEGAL], UI_TINYFONT | UI_CENTER, colorTable[CT_MDGREY]);
	UI_DrawProportionalString(320, 465, menu_normal_text[MNT_ID_LEGAL], UI_TINYFONT | UI_CENTER, colorTable[CT_MDGREY]);

	// Leave game 
	if (federationTimer < uis.realtime)
	{
		//trap_Cvar_Set ("rpg_playIntro", "1");
		trap_Cmd_ExecuteText( EXEC_NOW, "quit\n" );
	}
}

/*
===============
LeavingMenu_Cache
===============
*/
void LeavingMenu_Cache (void)
{
	s_leaving_menu.logo = trap_R_RegisterShaderNoMip("menu/special/rpgx_logo.tga"); 
	s_leaving_menu.halfround = trap_R_RegisterShaderNoMip( "menu/common/halfroundr_22.tga" );
	s_leaving_menu.logoutsnd = trap_S_RegisterSound( "sound/voice/computer/menu/logout.wav" );

}

/*
===============
LCARSOutMenu_Init
===============
*/
static void LeavingMenu_Init(void) 
{
	LeavingMenu_Cache();

	s_leaving_menu.menu.nitems					= 0;
	s_leaving_menu.menu.wrapAround				= qtrue;
	s_leaving_menu.menu.draw						= LeavingMenu_Draw;
	s_leaving_menu.menu.key						= M_Leaving_Key;
	s_leaving_menu.menu.fullscreen				= qtrue;
	s_leaving_menu.menu.descX					= 0;
	s_leaving_menu.menu.descY					= 0;
	s_leaving_menu.menu.titleX					= 0;
	s_leaving_menu.menu.titleY					= 0;
	s_leaving_menu.menu.titleI					= MNT_NONE;

	federationTimer = uis.realtime + 3000;
	//trap_Cvar_Set ("rpg_playIntro", "1");
	trap_S_StartLocalSound( s_leaving_menu.logoutsnd, CHAN_MENU1 );
}

/*
===============
UI_Leaving_Menu
===============
*/
static void UI_Leaving_Menu(void)
{

	LeavingMenu_Init(); 

	UI_PushMenu ( &s_leaving_menu.menu );

	trap_Key_SetCatcher( KEYCATCH_UI );
}

/*
=================
UI_Setup_MenuButtons
=================
*/
void UI_Setup_MenuButtons(void)
{
	int i,max;

	trap_R_SetColor( colorTable[CT_DKPURPLE1]);

	max=7;

	for (i=0;i<max;++i)
	{
		UI_DrawHandlePic(setup_menubuttons[i][0] - 14,setup_menubuttons[i][1], MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
	}

}

/*
=================
M_Quit_SplitDesc
=================
*/
/*static void M_Quit_SplitDesc(int descI)
{
	int	lineWidth,currentWidth,charCnt,currentLineI;
	const char *s, *holds;
	char holdChar[2];

	// Clean out any old data
	memset(systemDesc,0,sizeof(systemDesc));

	// Break into individual lines
	s = menu_normal_text[quitmenu_graphics[descI].normaltextEnum];
	holds = s;


	lineWidth = 228;	// How long (in pixels) a line can be
	currentWidth = 0;
	holdChar[1] = '\0';
	charCnt= 0;
	currentLineI = 0;

	while( *s ) 
	{
		charCnt++;
		holdChar[0] = *s;
		currentWidth += UI_ProportionalStringWidth(holdChar,UI_TINYFONT);
		currentWidth +=1; // The space between characters

		if ( currentWidth >= lineWidth )
		{//Reached max length of this line
			//step back until we find a space

			while((currentWidth) && (*s != ' '))
			{
				holdChar[0] = *s;
				currentWidth -= UI_ProportionalStringWidth(holdChar,UI_TINYFONT);
				s--;
				charCnt--;
			}

			Q_strncpyz( systemDesc[currentLineI], holds, charCnt);
			systemDesc[currentLineI][charCnt] = 0;

			currentLineI++;
			currentWidth = 0;
			charCnt = 0;

			holds = s;
			holds++;

			if (currentLineI > SYSTEM_MAXDESC)
			{
				currentLineI = (SYSTEM_MAXDESC -1);
				break;
			}
		}	
		++s;
	}

	++charCnt;  // So the NULL will be properly placed at the end of the string of Q_strncpyz
	Q_strncpyz( systemDesc[currentLineI], holds, charCnt);
	systemDesc[currentLineI][charCnt] = 0;

}*/

/*
=================
QuitMenu_Label
=================
*/
/*menutext_s *QuitMenu_Label(int labelId)
{
	menutext_s *label;

	switch(labelId)
	{
		case QMG_PHASER_LABEL :
			label = &s_quit.phaser_label;
			break;

		case QMG_TORPEDO_LABEL :
			label = &s_quit.torpedo_label;
			break;

		case QMG_VENTRAL_LABEL :
			label = &s_quit.ventral_label;
			break;

		case QMG_MIDHULL_LABEL :
			label = &s_quit.midhull_label;
			break;

		case QMG_BUSSARD_LABEL :
			label = &s_quit.bussard_label;
			break;

		case QMG_NACELLES_LABEL :
			label = &s_quit.nacelles_label;
			break;

		case QMG_THRUSTERS_LABEL :
			label = &s_quit.thrusters_label;
			break;

		case QMG_BRIDGE_LABEL :
			label = &s_quit.bridge_label;
			break;

		case QMG_VOYAGER_LABEL :
			label = &s_quit.voyager_label;
			break;

		default:
			label = &s_quit.bridge_label;
			break;
	}

	return(label);
}*/


/*
=================
QuitMenuInterrupt
=================
*/
static void QuitMenuInterrupt(int labelId)
{
	// ten seconds from now, start the auto animation again
	quitmenu_graphics[QMG_ACTIVE_SYSTEM].timer = uis.realtime + 10000;
}

/*
=================
QuitMenu_ChangeAreaFocus
=================
*/
/*static void QuitMenu_ChangeAreaFocus(int newSystem)
{
	int swoopI;
	int descI;
	int oldSystem;
	menubitmap_s *bitmap;
	menutext_s *oldLabel,*newLabel;


	// Turn off current system info
	oldSystem = quitmenu_graphics[QMG_ACTIVE_SYSTEM].target;


	// Turn old label brown
	oldLabel = QuitMenu_Label(oldSystem);
	oldLabel->color = colorTable[CT_DKBROWN1];

	// Turn off swoop
	swoopI = quitmenu_graphics[oldSystem].target;
	if (swoopI)
	{
		quitmenu_graphics[swoopI].type = MG_OFF;
	}

	// Turn pic off
	if (quitmenu_graphics[oldSystem].pointer)
	{
		bitmap = (menubitmap_s *) quitmenu_graphics[oldSystem].pointer;

		if (oldSystem == QMG_VOYAGER_LABEL)	//Voyager graphic can't be hidden
		{
			bitmap->color = CT_LTBLUE1;	
		}
		else
		{
			bitmap->generic.flags |= QMF_HIDDEN;	
		}
	}

	// Turning on the new system graphics
	quitmenu_graphics[QMG_ACTIVE_SYSTEM].target = newSystem;

	// Turn on new label
	newLabel = QuitMenu_Label(newSystem);
	newLabel->color = colorTable[CT_YELLOW];

	// Turn on system description
	descI = quitmenu_graphics[newSystem].min;
	if (descI)
	{
		M_Quit_SplitDesc(descI);
	}

	// Turn on pic
	if (quitmenu_graphics[newSystem].pointer)
	{
		bitmap = (menubitmap_s *) quitmenu_graphics[newSystem].pointer;
		bitmap->generic.flags &= ~QMF_HIDDEN;	

		if (newSystem == QMG_VOYAGER_LABEL)	//Voyager graphic can't be hidden
		{
			bitmap->color = CT_LTGOLD1;	
		}
	}

	// Turn onswoop
	swoopI = quitmenu_graphics[newSystem].target;
	if (swoopI)
	{
		quitmenu_graphics[swoopI].type = MG_GRAPHIC;
	}

}*/


/*
===============
M_Quit_Event
===============
*/
static void M_Quit_Event (void* ptr, int notification)
{
	if (notification != QM_ACTIVATED)
		return;

	switch (((menucommon_s*)ptr)->id)
	{
		case ID_MAINMENU:
			UI_PopMenu ();
			break;

		case ID_NO:
			UI_PopMenu ();
			break;

		case ID_YES:
			UI_Leaving_Menu();
//			trap_Cmd_ExecuteText( EXEC_NOW, "quit\n" );
			break;
	}
}

/*
===============
Quit_MenuKey
===============
*/
static sfxHandle_t Quit_MenuKey( int key )
{
	switch ( key )
	{
		case K_KP_LEFTARROW:
		case K_LEFTARROW:
		case K_KP_RIGHTARROW:
		case K_RIGHTARROW:
			key = K_TAB;
			break;

		case 'n':
		case 'N':
			M_Quit_Event( &s_quit.no, QM_ACTIVATED );
			break;

		case 'y':
		case 'Y':
			M_Quit_Event( &s_quit.yes, QM_ACTIVATED );
			break;
	}

	return ( Menu_DefaultKey( &s_quit.menu, key ) );
}

/*
===============
Quit_MenuBlinkies
===============
*/
static void Quit_MenuBlinkies( void )
{
	int /*activeSystem,*/i;

	// Move bottom blip
	if ( quitmenu_graphics[QMG_BOTTOM_BLIP].timer < uis.realtime )
	{
		quitmenu_graphics[QMG_BOTTOM_BLIP].x += 7;
		quitmenu_graphics[QMG_BOTTOM_BLIP2].x = quitmenu_graphics[QMG_BOTTOM_BLIP].x;

		/*if (quitmenu_graphics[QMG_BOTTOM_BLIP].x > quitmenu_graphics[QMG_BOTTOM_BLIP].max)
		{
			trap_S_StartLocalSound(s_quit.pingsound, CHAN_MENU1);

			quitmenu_graphics[QMG_BOTTOM_BLIP].x = quitmenu_graphics[QMG_BOTTOM_BLIP].min;
			quitmenu_graphics[QMG_BOTTOM_BLIP2].x = quitmenu_graphics[QMG_BOTTOM_BLIP].min;
		}*/

		// Middle ping?
		/*if ((quitmenu_graphics[QMG_BOTTOM_BLIP].x > 304) && 
			(quitmenu_graphics[QMG_BOTTOM_BLIP].x < 314))
		{
			trap_S_StartLocalSound(s_quit.pingsound, CHAN_LOCAL);
		}*/

		quitmenu_graphics[QMG_BOTTOM_BLIP].timer = uis.realtime + 75;
	}

	// Time to change systems???
	/*if ( quitmenu_graphics[QMG_ACTIVE_SYSTEM].timer < uis.realtime )
	{
		activeSystem = quitmenu_graphics[QMG_ACTIVE_SYSTEM].target;

		// Change again in five seconds
		quitmenu_graphics[QMG_ACTIVE_SYSTEM].timer = uis.realtime + 5000;

		// Advance to next system
		++activeSystem;
		if (activeSystem >= QMG_LABEL_END)	// Past max strings
		{
			activeSystem = QMG_LABEL_START +1;	// Reset
		}

		QuitMenu_ChangeAreaFocus(activeSystem);

		trap_S_StartLocalSound( uis.menu_choice1_snd, CHAN_MENU1 );	// Ping!

	}*/

	// Generate new numbers
	if ((quitmenu_graphics[QMG_NUMBERS].timer < uis.realtime) && (quitmenu_graphics[QMG_NUMBERS].target==5))
	{

		for (i=0;i<5;++i)
		{
			quitmenu_graphics[i + QMG_COL1_NUM1].target = (random() * (900000000)) + 99999999;
			quitmenu_graphics[i + QMG_COL3_NUM1].target = (random() * (900000000)) + 99999999;
			quitmenu_graphics[i + QMG_COL4_NUM1].target = (random() * (900000000)) + 99999999;
		}

		for (i=0;i<5;++i)
		{
			quitmenu_graphics[i + QMG_COL2_NUM1].target = (random() * (900000)) + 99999;
		}

		for (i=0;i<5;++i)
		{
			quitmenu_graphics[i + QMG_COL5_NUM1].target = (random() * (90)) + 9;
			quitmenu_graphics[i + QMG_COL6_NUM1].target = (random() * (90)) + 9;
		}

		quitmenu_graphics[QMG_NUMBERS].target=1;

		// Turn off all but the first row
		for (i=0;i<4;++i)
		{
			quitmenu_graphics[i + QMG_COL1_NUM2].type = MG_OFF;
			quitmenu_graphics[i + QMG_COL2_NUM2].type = MG_OFF;
			quitmenu_graphics[i + QMG_COL3_NUM2].type = MG_OFF;
			quitmenu_graphics[i + QMG_COL4_NUM2].type = MG_OFF;
			quitmenu_graphics[i + QMG_COL5_NUM2].type = MG_OFF;
			quitmenu_graphics[i + QMG_COL6_NUM2].type = MG_OFF;
		}
	}
	// Activate the next row of numbers.  When at the last row wait and then generate new numbers
	else if ((quitmenu_graphics[QMG_NUMBERS].timer < uis.realtime) && (quitmenu_graphics[QMG_NUMBERS].target<5))
	{
		trap_S_StartLocalSound( uis.menu_datadisp1_snd, CHAN_LOCAL );	// 

		quitmenu_graphics[QMG_NUMBERS].timer = uis.realtime + 50;

		i = quitmenu_graphics[QMG_NUMBERS].target;
		quitmenu_graphics[i + QMG_COL1_NUM1].type = MG_NUMBER;
		quitmenu_graphics[i + QMG_COL2_NUM1].type = MG_NUMBER;
		quitmenu_graphics[i + QMG_COL3_NUM1].type = MG_NUMBER;
		quitmenu_graphics[i + QMG_COL4_NUM1].type = MG_NUMBER;
		quitmenu_graphics[i + QMG_COL5_NUM1].type = MG_NUMBER;
		quitmenu_graphics[i + QMG_COL6_NUM1].type = MG_NUMBER;

		++quitmenu_graphics[QMG_NUMBERS].target;

		if (quitmenu_graphics[QMG_NUMBERS].target == 5)
		{
			quitmenu_graphics[QMG_NUMBERS].timer = uis.realtime + 6000;
		}
	}
}

/*
===============
Quit_MenuDraw
===============
*/
static void Quit_MenuDraw( void )
{
	int i,y;

	// Draw the basic screen layout
	UI_MenuFrame(&s_quit.menu);

	UI_DrawProportionalString(  74,  66, "6881",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "2-099",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "45677",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(30,364,  47, 25, uis.whiteShader);	// Left hand column
	UI_DrawHandlePic(80,364, 247, 7, uis.whiteShader);	// Top line above CONFIDENTIAL
	UI_DrawHandlePic(334,386, 30, 50, uis.whiteShader);	// LH Middle column
	UI_DrawHandlePic(327,364,  64, 64, s_quit.swoop1);	// 

	UI_DrawHandlePic(334,386, 30, 50, uis.whiteShader);	// LH Middle column

	UI_DrawHandlePic(367,379, 15, 57, uis.whiteShader);	// RH Middle column
	UI_DrawHandlePic(387,364, 225, 7, uis.whiteShader);	// Top line above systems description
	UI_DrawHandlePic(367,364,  32, 32, s_quit.swoop2);	// 

	UI_DrawProportionalString(  74,  368, "2166",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	// Leave Voyager box
	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(301,  57,  64,  32, s_quit.corner_ur);		// Corner
	UI_DrawHandlePic( 81,  57, 224,  18, uis.whiteShader);	// Top
	UI_DrawHandlePic(305,  74,  60,  62, uis.whiteShader);	// Right side
	UI_DrawProportionalString(157,58,menu_normal_text[MNT_QUIT_ELITE_FORCE], UI_BLINK | UI_SMALLFONT, colorTable[CT_LTGOLD1] );

	//block under the picture
	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic( 30,  353, 47,	8, uis.whiteShader);

	// Litte squares off to the side of the LEAVE VOYAGER Yes/No buttons
	trap_R_SetColor( colorTable[CT_DKPURPLE1]);
	UI_DrawHandlePic(140,81, 8, MENU_BUTTON_MED_HEIGHT, uis.whiteShader);	// LEAVE VOYAGER?
	UI_DrawHandlePic(140,81 + MENU_BUTTON_MED_HEIGHT + 6, 8, MENU_BUTTON_MED_HEIGHT, uis.whiteShader);	// LEAVE VOYAGER?


	trap_R_SetColor( colorTable[CT_BLACK]);
	UI_DrawHandlePic(30, 164, 47, 3, uis.whiteShader);	// Upper left hand blip columns
	UI_DrawHandlePic(30, 175, 47, 25, uis.whiteShader);	// Upper left hand blip columns

	/*trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(30, 167, 47, 8, uis.whiteShader);	// Blip columns
	UI_DrawHandlePic(565, 167, 47, 8, uis.whiteShader);	// Blip columns


	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(30, 353, 47, 8, uis.whiteShader);	// Blip columns
	UI_DrawHandlePic(565,353, 47, 8, uis.whiteShader);	// Blip columns*/

	//RPG-X Fling
	UI_DrawProportionalString(97, 380, menu_normal_text[MNT_STARTREK1], UI_SMALLFONT, colorTable[CT_LTBROWN1] );
	UI_DrawProportionalString(97, 400, menu_normal_text[MNT_STARTREK2], UI_SMALLFONT, colorTable[CT_LTBROWN1] );
	UI_DrawProportionalString(97, 426, menu_normal_text[MNT_FORMOREINFO], UI_TINYFONT, colorTable[CT_LTBROWN1] );

	//UberGames Fling
	UI_DrawProportionalString(610, 380, menu_normal_text[MNT_UBERGAMES], UI_SMALLFONT|UI_RIGHT, colorTable[CT_LTBROWN1] );
	UI_DrawProportionalString(610, 398, menu_normal_text[MNT_UBERGAMES2], UI_TINYFONT|UI_RIGHT, colorTable[CT_LTBROWN1] );
	UI_DrawProportionalString(610, 426, menu_normal_text[MNT_UBERGAMESINFO], UI_TINYFONT|UI_RIGHT, colorTable[CT_LTBROWN1] );

	//draw the giant banner
	UI_DrawHandleStretchPic( 30, 178, 582, 172, 0, 0, 0.97412, 1.0, s_quit.quitBanner );

	trap_R_SetColor( colorTable[CT_NONE]);

	Menu_Draw( &s_quit.menu );

	Quit_MenuBlinkies();

	UI_PrintMenuGraphics(quitmenu_graphics,QMG_MAX);

	y = 374;
	for (i=0;i<SYSTEM_MAXDESC;++i)
	{
		UI_DrawProportionalString(384, y, systemDesc[i],UI_TINYFONT,colorTable[CT_LTGOLD1]);
		y += 12;
	}
}


/*
===============
UI_QuitMenu_Cache
===============
*/
void UI_QuitMenu_Cache( void ) 
{
	s_quit.corner_ur	= trap_R_RegisterShaderNoMip("menu/common/corner_ur_18_60.tga");
	s_quit.swoop1		= trap_R_RegisterShaderNoMip("menu/common/corner_ur_7_30.tga");
	s_quit.swoop2		= trap_R_RegisterShaderNoMip("menu/common/corner_ul_7_15.tga");
	s_quit.quitBanner	= trap_R_RegisterShaderNoMip("gfx/2d/quit_banner.tga");
	//s_quit.pingsound	= trap_S_RegisterSound( "sound/interface/sensorping.wav" );

	/*trap_R_RegisterShaderNoMip(PIC_PHASER_STRIP);
	trap_R_RegisterShaderNoMip(PIC_PHOTON_LAUNCHER);
	trap_R_RegisterShaderNoMip(PIC_BOTTOM_STRIP);
	trap_R_RegisterShaderNoMip(PIC_MID_HULL);
	trap_R_RegisterShaderNoMip(PIC_WARPNAC);
	trap_R_RegisterShaderNoMip(PIC_BUSSARD);
	trap_R_RegisterShaderNoMip(PIC_RCS);
	trap_R_RegisterShaderNoMip(PIC_BRIDGE);
	trap_R_RegisterShaderNoMip(PIC_VOYAGER);*/

	// Precache all menu graphics in array
	UI_PrecacheMenuGraphics(quitmenu_graphics,QMG_MAX);
}

/*
=================
UI_QuitMenu
=================
*/
static void Quit_MenuInit(void)
{
	int y,x,i;

	UI_QuitMenu_Cache(); 

	s_quit.menu.nitems					= 0;
	s_quit.menu.draw					= Quit_MenuDraw;
	s_quit.menu.key						= Quit_MenuKey;
	s_quit.menu.wrapAround				= qtrue;
	s_quit.menu.descX					= MENU_DESC_X;
	s_quit.menu.descY					= MENU_DESC_Y;
	s_quit.menu.titleX					= MENU_TITLE_X;
	s_quit.menu.titleY					= MENU_TITLE_Y;
	s_quit.menu.titleI					= MNT_QUITMENU_TITLE;
	s_quit.menu.footNoteEnum			= MNT_SHIP_SYSTEMS;

	s_quit.menu.fullscreen = qtrue;

	s_quit.mainmenu.generic.type		= MTYPE_BITMAP;      
	s_quit.mainmenu.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_quit.mainmenu.generic.x			= 482;
	s_quit.mainmenu.generic.y			= 136;
	s_quit.mainmenu.generic.name		= BUTTON_GRAPHIC_LONGRIGHT;
	s_quit.mainmenu.generic.id			= ID_MAINMENU;
	s_quit.mainmenu.generic.callback	= M_Quit_Event;
	s_quit.mainmenu.width				= MENU_BUTTON_MED_WIDTH;
	s_quit.mainmenu.height				= MENU_BUTTON_MED_HEIGHT;
	s_quit.mainmenu.color				= CT_DKPURPLE1;
	s_quit.mainmenu.color2				= CT_LTPURPLE1;
	s_quit.mainmenu.textX				= MENU_BUTTON_TEXT_X;
	s_quit.mainmenu.textY				= MENU_BUTTON_TEXT_Y;
	s_quit.mainmenu.textEnum			= MBT_MAINMENU;
	s_quit.mainmenu.textcolor			= CT_BLACK;
	s_quit.mainmenu.textcolor2		= CT_BLACK; //CT_WHITE;
	

	y = 81;
	x = 152;
	s_quit.yes.generic.type				= MTYPE_BITMAP;      
	s_quit.yes.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS; 
	s_quit.yes.generic.x				= x;                 
	s_quit.yes.generic.y				= y;
	s_quit.yes.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_quit.yes.generic.id				= ID_YES;
	s_quit.yes.generic.callback			= M_Quit_Event;
	s_quit.yes.width					= MENU_BUTTON_MED_WIDTH;
	s_quit.yes.height					= MENU_BUTTON_MED_HEIGHT;
	s_quit.yes.color					= CT_DKPURPLE1;
	s_quit.yes.color2					= CT_LTPURPLE1;
	s_quit.yes.textX					= MENU_BUTTON_TEXT_X;
	s_quit.yes.textY					= MENU_BUTTON_TEXT_Y;
	s_quit.yes.textEnum					= MBT_QUITGAME_YES;
	s_quit.yes.textcolor				= CT_BLACK;
	s_quit.yes.textcolor2		= CT_BLACK; //CT_WHITE;

	s_quit.no.generic.type				= MTYPE_BITMAP;      
	s_quit.no.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_quit.no.generic.x					= x;
	s_quit.no.generic.y					= y + MENU_BUTTON_MED_HEIGHT + 6;
	s_quit.no.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_quit.no.generic.id				= ID_NO;
	s_quit.no.generic.callback			= M_Quit_Event; 
	s_quit.no.width						= MENU_BUTTON_MED_WIDTH;
	s_quit.no.height					= MENU_BUTTON_MED_HEIGHT;
	s_quit.no.color						= CT_DKPURPLE1;
	s_quit.no.color2					= CT_LTPURPLE1;
	s_quit.no.textX						= MENU_BUTTON_TEXT_X;
	s_quit.no.textY						= MENU_BUTTON_TEXT_Y;
	s_quit.no.textEnum					= MBT_QUITGAME_NO;
	s_quit.no.textcolor					= CT_BLACK;
	s_quit.no.textcolor2		= CT_BLACK; //CT_WHITE;

	Menu_AddItem( &s_quit.menu,	&s_quit.mainmenu );
	Menu_AddItem( &s_quit.menu,	&s_quit.no );
	Menu_AddItem( &s_quit.menu,	&s_quit.yes );             

	s_quit.menu.initialized = qtrue;

	quitmenu_graphics[QMG_BOTTOM_BLIP].x = quitmenu_graphics[QMG_BOTTOM_BLIP].min;
	quitmenu_graphics[QMG_BOTTOM_BLIP2].x = quitmenu_graphics[QMG_BOTTOM_BLIP].x;
	quitmenu_graphics[QMG_BOTTOM_BLIP].timer = uis.realtime + 50;

    // Turn off swoops
	for (i=QMG_SWOOP_START+1;i<QMG_SWOOP_END;++i)
	{
		quitmenu_graphics[i].type = CT_LTGREY;
	}

    // Turn off descriptions
	for (i=QMG_DESC_START+1;i<QMG_DESC_END;++i)
	{
		quitmenu_graphics[i].type = MG_OFF;
	}

	// Set labels to brown
	for (i=(QMG_LABEL_START+1);i<QMG_LABEL_END;++i)	
	{
		quitmenu_graphics[i].color = CT_DKBROWN1;
	}

	// Turn on active system info
	quitmenu_graphics[QMG_ACTIVE_SYSTEM].timer = uis.realtime + 100;	// When to change to next system
	quitmenu_graphics[QMG_ACTIVE_SYSTEM].target = QMG_LABEL_END - 1;	// Give it an old system to turn off

	// Force numbers to change
	quitmenu_graphics[QMG_NUMBERS].timer = 0;	// To get numbers right away
	quitmenu_graphics[QMG_NUMBERS].target=5;

}

/*
=================
UI_QuitMenu
=================
*/
void UI_QuitMenu( void )
{

//f (!s_quit.menu.initialized)
//
		Quit_MenuInit();
//

	UI_PushMenu( &s_quit.menu );
}

