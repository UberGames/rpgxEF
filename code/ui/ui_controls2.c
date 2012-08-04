// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=======================================================================

CONTROLS MENU

=======================================================================
*/
#include "ui_local.h"

static void SetupMenu_SideButtons(menuframework_s *menu,int menuType);
static void Controls_UpdateNew( void );
static void Controls_MenuEvent (void* ptr, int event);
static void UI_ControlsMoveMenu( void );
static void UI_ControlsAttackLookMenu( void );
static void UI_ControlsMouseJoyStickMenu( void );
static void UI_ControlsDefaultMenu( void );
static void UI_ControlsOtherMenu( void );
static void UI_ControlsCommandMenu( void );
static void Controls_MenuEventVideo (void* ptr, int event);
static void UI_ControlsModelViewMenu( void ); //TiM

void UI_SetupWeaponsMenu( void );
static qhandle_t	swooshTop;
static qhandle_t	swooshBottom;
//static qboolean				g_waitingforkey;
static qboolean				g_changesmade;
static qboolean				g_prompttosave;
static menucommon_s	*chosenitem;


#define PIC_MONBAR2		"menu/common/monbar_2.tga"
#define PIC_MOUSE1		"menu/common/mouse1.tga"
#define PIC_MOUSE2		"menu/common/mouse2.tga"

void *holdControlPtr;
int holdControlEvent;
static menuaction_s *vid_apply_action;

float setup_menubuttons[8][2] = 
{
{125,62},
{125,86},
{125,109},
{300,62},
{300,86},
{300,109},
{482,62},
{482,86},
};

int s_OffOnNone_Names[] =
{
	MNT_OFF,
	MNT_ON,
	MNT_NONE
};

int s_Autoswitch_Names[] =
{
	MNT_OFF,
	MNT_SAFE,
	MNT_BEST,
	MNT_NONE
};

static menuframework_s		s_controlsdefault_menu;
static menubitmap_s			s_controls_default_yes;
static menubitmap_s			s_controls_default_no;

static menubitmap_s			s_controls_mainmenu;
static menubitmap_s			s_controls_controls;
static menubitmap_s			s_controls_video;
static menubitmap_s			s_controls_sound;
static menubitmap_s			s_controls_game;
static menubitmap_s			s_controls_cdkey;
static menubitmap_s			s_controls_player;
static menubitmap_s			s_controls_fonts;
static menubitmap_s			s_controls_network;

static menuframework_s		s_controlsother_menu;
//static menulist_s			s_lookspring_box;
static menuslider_s			s_keyturnspeed_slider;

static struct 
{
	qhandle_t mon_bar;
} s_controlsother;

static struct 
{
	qhandle_t mouse1;
	qhandle_t mouse2;
} s_joystick_mouse;

#define ID_MAINMENU		100
#define ID_CONTROLS		110
#define ID_VIDEO		102
#define ID_SOUND		103
#define ID_GAMEOPTIONS	104
#define ID_CDKEY		105
#define ID_PLAYER		106
#define ID_FONTS		107
#define ID_NETWORK		108
#define ID_INGAMEMENU	109

#define ID_KEYTURNPEED			45
#define ID_LOOKSPRING			44
#define ID_AUTOSWITCHWEAPONS	48

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
} attackmenu_graphics_t;

menugraphics_s attackmenu_graphics[AMG_MAX] = 
{
//	type		timer	x		y		width	height	file/text						graphic,	min		max	target	inc		style	color
	{ MG_GRAPHIC,	0.0,	158,	280,	4,		32,		"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_MIDLEFT
	{ MG_GRAPHIC,	0.0,	158,	180,	8,		97,		"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_UPPERLEFT
	{ MG_GRAPHIC,	0.0,	158,	315,	8,		100,	"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_LOWERLEFT
	{ MG_GRAPHIC,	0.0,	158,	164,	16,		 16,	"menu/common/corner_lu.tga",0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_UPPERCORNER
	{ MG_GRAPHIC,	0.0,	158,	406,	32,		 32,	"menu/common/newswoosh.tga",0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_LOWERCORNER

	{ MG_GRAPHIC,	0.0,	177,	164,	280,	  8,	"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_UPPERTOP1ST
	{ MG_GRAPHIC,	0.0,	175,	410,	282,	 18,	"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_LOWERTOP1ST

	{ MG_GRAPHIC,	0.0,	457,	164,	 34,	  8,	"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_UPPERTOP2ND
	{ MG_GRAPHIC,	0.0,	457,	410,	 34,	 18,	"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_LOWERTOP2ND

	{ MG_GRAPHIC,	0.0,	494,	164,	128,	128,	"menu/common/swoosh_top.tga",0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_UPPERSWOOP
	{ MG_GRAPHIC,	0.0,	483,	403,	128,	32,	"menu/common/newswoosh_long.tga",0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_LOWERSWOOP

	{ MG_GRAPHIC,	0.0,	501,	189,	110,	17,		"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_TOPRIGHT
	{ MG_GRAPHIC,	0.0,	501,	383,	110,	17,		"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_BOTTOMRIGHT

	{ MG_GRAPHIC,	0.0,	501,	206,	110,	177,	"menu/common/square.tga",	0,	0,		0,		0,	0,		0,		0,		CT_VDKPURPLE1,	NULL },	// AMG_PLAYERBKGRND
};
static int					g_section;

static menuframework_s		s_weapons_menu;
static menubitmap_s			s_controls_playermdl;
static menubitmap_s			s_controls_weapon;
static menuaction_s			s_attack_weapon0_action;
static menuaction_s			s_attack_weapon1_action;
static menuaction_s			s_attack_weapon2_action;
static menuaction_s			s_attack_weapon3_action;
static menuaction_s			s_attack_weapon4_action;
static menuaction_s			s_attack_weapon5_action;
//static menuaction_s			s_attack_weapon6_action;
//static menuaction_s			s_attack_weapon7_action;
//static menuaction_s			s_attack_weapon8_action;
//static menuaction_s			s_attack_weapon9_action;
static menuaction_s			s_attack_weapon_next_action;
static menuaction_s			s_attack_weapon_prev_action;
static menuaction_s			s_attack_waiting_action;

static menubitmap_s			s_controls_default;

static menubitmap_s			s_controls_look;

static menuframework_s		s_controlscommand_menu;
static menuframework_s		s_controlslook_menu;
static menuaction_s			s_attack_attack_action;
static menuaction_s			s_attack_alt_attack_action;
static menuaction_s			s_attack_objectives;
static menuaction_s			s_attack_use_action;
static menuaction_s			s_attack_use_inv_action;
static menuaction_s			s_look_lookup_action;
static menuaction_s			s_look_lookdown_action;
static menuaction_s			s_look_mouselook_action;
static menuaction_s			s_look_centerview_action;
static menuaction_s			s_zoomview_action;

static menuaction_s			s_equip_action;
static menuaction_s         s_drop_action; // RPG-X | Marcin | 04/12/2008

static menuframework_s		s_controlsmove_menu;
static menuframework_s		s_controlscommand_menu;

static menuframework_s		s_controlsmouse_menu;
//atic menulist_s			s_joyenable_box;
//static menuslider_s			s_joythreshold_slider;
//static menulist_s			s_forcefeedback_box;
static menulist_s			s_joyxbutton_box;
static menulist_s			s_joyybutton_box;

static menubitmap_s			s_controls_movement;
static menuaction_s			s_move_walkforward_action;
static menuaction_s			s_move_backpedal_action;
static menuaction_s			s_move_turnleft_action;
static menuaction_s			s_move_turnright_action;
static menuaction_s			s_move_run_action;
static menuaction_s			s_move_stepleft_action;
static menuaction_s			s_move_stepright_action;
static menuaction_s			s_move_sidestep_action;
static menuaction_s			s_move_moveup_action;
static menuaction_s			s_move_movedown_action;

//RPG-X - Modelview Menu
static menuframework_s		s_controlsmodelview_menu;	//Modelview menu
static menubitmap_s			s_controls_modelview;		//Main model view button
static menubitmap_s			s_thirdperson_action;
static menubitmap_s			s_thirdpersoncommit_action;
static menubitmap_s			s_thirdpersonrevert_action;
static menubitmap_s			s_thirdpersonreset_action;
static menuaction_s			s_zoomforward_action;
static menuaction_s			s_zoombackward_action;
static menuaction_s			s_panleft_action;
static menuaction_s			s_panright_action;
static menuaction_s			s_panup_action;
static menuaction_s			s_pandown_action;
static menuaction_s			s_rotateleft_action;
static menuaction_s			s_rotateright_action;
static menuaction_s			s_pitchup_action;
static menuaction_s			s_pitchdown_action;

static menubitmap_s			s_controls_command;
static menubitmap_s			s_controls_mouse;
static menubitmap_s			s_controls_other;

#define ID_CONTROLSCOMMAND	200
#define ID_CONTROLSLOOK		201
#define ID_CONTROLSMOVE		202
#define ID_CONTROLSMOUSE	203
#define ID_CONTROLSOTHER	204
#define ID_CONTROLSDEFAULT	205
//RPG-X : TiM
#define	ID_CONTROLSMODELVIEW	206

#define ID_DEFAULT_YES		300
#define ID_DEFAULT_NO		301

#define C_WEAPONS		0
#define C_COMMAND		1
#define C_LOOK			2
#define C_MOVE			3
#define	C_MODELVIEW		4
#define C_MOUSE			5
#define C_OTHER			6
#define C_MAX			7 //6 - TiM

// control sections
#define C_MOVEMENT		0
#define C_LOOKING		1
//#define C_WEAPONS		2
#define C_MISC			3
//#define C_MAX			4

typedef struct {
	char	*command;
	int		label;
	int		id;
	int		anim;
	int		defaultbind1;
//	int		defaultbind2;
	int		bind1;
	int		bind2;
	int		bind3;
	int		desc;
} bind_t;

typedef struct
{
	char*	name;
	float	defaultvalue;
	float	value;	
} configcvar_t;

#define SAVE_NOOP		0
#define SAVE_YES		1
#define SAVE_NO			2
#define SAVE_CANCEL		3

#define ID_MOVEMENT		100
#define ID_LOOKING		101
#define ID_WEAPONS		102
//#define ID_MISC			103
//#define ID_DEFAULTS		104
//#define ID_BACK			105
#define ID_SAVEANDEXIT	106
#define ID_EXIT			107

// bindable actions
#define ID_SHOWSCORES	0
#define ID_USEITEM		1	
#define ID_SPEED		2	
#define ID_FORWARD		3	
#define ID_BACKPEDAL	4
#define ID_MOVELEFT		5
#define ID_MOVERIGHT	6
#define ID_MOVEUP		7	
#define ID_MOVEDOWN		8
#define ID_LEFT			9	
#define ID_RIGHT		10	
#define ID_STRAFE		11	
#define ID_LOOKUP		12	
#define ID_LOOKDOWN		13
#define ID_MOUSELOOK	14
#define ID_CENTERVIEW	15
#define ID_ZOOMVIEW		16
#define ID_WEAPON1		17	
#define ID_WEAPON2		18	
#define ID_WEAPON3		19	
#define ID_WEAPON4		20	
#define ID_WEAPON5		21	
#define ID_WEAPON0		22
//#define ID_WEAPON6		22	
//#define ID_WEAPON7		23	
//#define ID_WEAPON8		24	
//#define ID_WEAPON9		25	
#define ID_ATTACK		23
#define ID_ALT_ATTACK	24	
#define ID_WEAPPREV		25
#define ID_WEAPNEXT		26
#define ID_GESTURE		27
#define ID_EQUIP		28
#define ID_DROP			29 // RPG-X | Marcin | 04/12/2008
#define ID_CHAT			30
#define ID_CHAT2		31
#define ID_CHAT3		32
#define ID_CHAT4		33
//#define ID_CHAT5		34


// START MOD
//#define ID_CHAT5		900
// END MOD
#define ID_USEINVENTORY	34
#define ID_OBJECTIVES	35

#define ID_TOGGLE3DP	36
#define ID_COMMITANGLES	37
#define	ID_REVERTANGLES	38
#define	ID_RESETANGLES	39

#define	ID_ZOOMFORWARD	40
#define	ID_ZOOMBACKWARD	41
#define	ID_PANLEFT		42
#define	ID_PANRIGHT		43
#define	ID_PANUP		44
#define	ID_PANDOWN		45
#define	ID_ROTATELEFT	46
#define	ID_ROTATERIGHT	47
#define ID_PITCHUP		48
#define ID_PITCHDOWN	49

#define ID_USE			1	

// all others
#define ID_FREELOOK		50
#define ID_INVERTMOUSE	51
#define ID_ALWAYSRUN	52
#define ID_AUTOSWITCH	53
#define ID_MOUSESPEED	54
#define ID_JOYENABLE	55
#define ID_JOYTHRESHOLD	56
#define ID_SMOOTHMOUSE	57

#define ANIM_IDLE		0
#define ANIM_RUN		1
#define ANIM_WALK		2
#define ANIM_BACK		3
#define ANIM_JUMP		4
#define ANIM_CROUCH		5
#define ANIM_STEPLEFT	6
#define ANIM_STEPRIGHT	7
#define ANIM_TURNLEFT	8
#define ANIM_TURNRIGHT	9
#define ANIM_LOOKUP		10
#define ANIM_LOOKDOWN	11
#define ANIM_WEAPON1	12
#define ANIM_WEAPON2	13
#define ANIM_WEAPON3	14
#define ANIM_WEAPON4	15
#define ANIM_WEAPON5	16
/*#define ANIM_WEAPON6	17
#define ANIM_WEAPON7	18
#define ANIM_WEAPON8	19
#define ANIM_WEAPON9	20
#define ANIM_WEAPON10	21*/
#define ANIM_ATTACK		22
#define ANIM_GESTURE	23
#define ANIM_DIE		24
#define ANIM_CHAT		25

typedef struct
{
	menuframework_s		menu;

	menutext_s			banner;
	menubitmap_s		framel;
	menubitmap_s		framer;
	menubitmap_s		player;

	menutext_s			movement;
	menutext_s			looking;
	menutext_s			weapons;
	menutext_s			misc;

	menuaction_s		walkforward;
	menuaction_s		backpedal;
	menuaction_s		stepleft;
	menuaction_s		stepright;
	menuaction_s		moveup;
	menuaction_s		movedown;
	menuaction_s		turnleft;
	menuaction_s		turnright;
	menuaction_s		sidestep;
	menuaction_s		run;
	menuaction_s		machinegun;
	menuaction_s		attack;
	menuaction_s		prevweapon;
	menuaction_s		nextweapon;
	menuaction_s		lookup;
	menuaction_s		lookdown;
	menuaction_s		mouselook;
	menulist_s			freelook;
	menuaction_s		centerview;
	menuaction_s		zoomview;
	menuaction_s		gesture;
	menulist_s			invertmouse;
	menuslider_s		sensitivity;
	menulist_s			smoothmouse;
	menulist_s			alwaysrun;
	menuaction_s		showscores;
	menulist_s			autoswitch;
	menuaction_s		useitem;
	playerInfo_t		playerinfo;
	qboolean			changesmade;
	menuaction_s		chat;
	menuaction_s		chat2;
	menuaction_s		chat3;
	menuaction_s		chat4;
	menuaction_s		chat5;
	menulist_s			joyenable;
	menuslider_s		joythreshold;
	int					section;
	qboolean			waitingforkey;
	char				playerModel[64];
	vec3_t				playerViewangles;
	vec3_t				playerMoveangles;
	int					playerLegs;
	int					playerTorso;
	int					playerWeapon;
	qboolean			playerChat;

	menubitmap_s		back;
	menutext_s			name;
} controls_t; 	

static controls_t s_controls;

//static vec4_t controls_binding_color  = {1.00, 0.43, 0.00, 1.00};

static bind_t g_bindings[] = 
{
	{"+info",					MNT_SHORTCUT_SCORES,			ID_SHOWSCORES,	ANIM_IDLE,		K_TAB,			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"+use",					MNT_SHORTCUT_USEOBJECT,			ID_USEITEM,		ANIM_IDLE,		K_ENTER,		-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"+speed", 					MNT_SHORTCUT_RUNWALK,			ID_SPEED,		ANIM_RUN,		K_SHIFT,		-1,		-1,	-1,MNT_SHORTCUT_KEY},
	{"+forward", 				MNT_SHORTCUT_WALKFORWARD,		ID_FORWARD,		ANIM_WALK,		K_UPARROW,		-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"+back", 					MNT_SHORTCUT_BACKPEDAL,			ID_BACKPEDAL,	ANIM_BACK,		K_DOWNARROW,	-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"+moveleft",				MNT_SHORTCUT_STEPLEFT,			ID_MOVELEFT,	ANIM_STEPLEFT,	',',			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"+moveright", 				MNT_SHORTCUT_STEPRIGHT,			ID_MOVERIGHT,	ANIM_STEPRIGHT,	'.',			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"+moveup",					MNT_SHORTCUT_UPJUMP,			ID_MOVEUP,		ANIM_JUMP,		'd',		-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"+movedown",				MNT_SHORTCUT_DOWNCROUCH,		ID_MOVEDOWN,	ANIM_CROUCH,	'c',			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"+left", 					MNT_SHORTCUT_TURNLEFT,			ID_LEFT,		ANIM_TURNLEFT,	K_LEFTARROW,	-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"+right", 					MNT_SHORTCUT_TURNRIGHT,			ID_RIGHT,		ANIM_TURNRIGHT,	K_RIGHTARROW,	-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"+strafe", 				MNT_SHORTCUT_SIDESTEPTURN,		ID_STRAFE,		ANIM_IDLE,		K_ALT,			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"+lookup", 				MNT_SHORTCUT_LOOKUP,			ID_LOOKUP,		ANIM_LOOKUP,	K_PGDN,			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"+lookdown", 				MNT_SHORTCUT_LOOKDOWN,			ID_LOOKDOWN,	ANIM_LOOKDOWN,	K_DEL,			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"laser", 					MNT_SHORTCUT_MOUSELOOK,			ID_MOUSELOOK,	ANIM_IDLE,		'r',			-1,		-1, -1,MNT_SHORTCUT_KEY},
//	{"+mlook", 					MNT_SHORTCUT_MOUSELOOK,			ID_MOUSELOOK,	ANIM_IDLE,		'/',			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"centerview", 				MNT_SHORTCUT_CENTERVIEW,		ID_CENTERVIEW,	ANIM_IDLE,		K_END,			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"+zoom", 					MNT_SHORTCUT_ZOOMVIEW,			ID_ZOOMVIEW,	ANIM_IDLE,		-1,				-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"weapon 1",				MNT_SHORTCUT_WEAPON1,			ID_WEAPON1,		ANIM_WEAPON1,	'1',			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"weapon 2",				MNT_SHORTCUT_WEAPON2,			ID_WEAPON2,		ANIM_WEAPON2,	'2',			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"weapon 3",				MNT_SHORTCUT_WEAPON3,			ID_WEAPON3,		ANIM_WEAPON3,	'3',			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"weapon 4",				MNT_SHORTCUT_WEAPON4,			ID_WEAPON4,		ANIM_WEAPON4,	'4',			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"weapon 5",				MNT_SHORTCUT_WEAPON5,			ID_WEAPON5,		ANIM_WEAPON5,	'5',			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"weapon 0",				MNT_SHORTCUT_WEAPON0,			ID_WEAPON0,		ANIM_WEAPON10,	'0',			-1,		-1, -1,MNT_SHORTCUT_KEY},
	/*{"weapon 6",				MNT_SHORTCUT_WEAPON6,			ID_WEAPON6,		ANIM_WEAPON6,	'6',			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"weapon 7",				MNT_SHORTCUT_WEAPON7,			ID_WEAPON7,		ANIM_WEAPON7,	'7',			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"weapon 8",				MNT_SHORTCUT_WEAPON8,			ID_WEAPON8,		ANIM_WEAPON8,	'8',			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"weapon 9",				MNT_SHORTCUT_WEAPON9,			ID_WEAPON9,		ANIM_WEAPON9,	'9',			-1,		-1, -1,MNT_SHORTCUT_KEY},*/
	{"+attack", 				MNT_SHORTCUT_ATTACK,			ID_ATTACK,		ANIM_ATTACK,	K_CTRL,			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"+altattack", 				MNT_SHORTCUT_ALTATTCK,			ID_ALT_ATTACK,	ANIM_ATTACK,	K_MOUSE2,		-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"weapprev",				MNT_SHORTCUT_PREVWEAPON,		ID_WEAPPREV,	ANIM_IDLE,		'[',			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"weapnext", 				MNT_SHORTCUT_NEXTWEAPON,		ID_WEAPNEXT,	ANIM_IDLE,		']',			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"+button3", 				MNT_SHORTCUT_GESTURE,			ID_GESTURE,		ANIM_GESTURE,	K_MOUSE3,		-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"equip",					MNT_SHORTCUT_EQUIP,				ID_EQUIP,		ANIM_IDLE,		'p',			-1,		-1,	-1,MNT_SHORTCUT_KEY},
    // RPG-X | Marcin | 04/12/2008
    {"drop",					MNT_SHORTCUT_DROP,				ID_DROP,		ANIM_IDLE,		'g',			-1,		-1,	-1,MNT_SHORTCUT_KEY},
    {"messagemode",				MNT_SHORTCUT_CHAT,				ID_CHAT,		ANIM_CHAT,		'y',			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"messagemode2",		 	MNT_SHORTCUT_CHATTEAM,			ID_CHAT2,		ANIM_CHAT,		't',			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"messagemode3",		 	MNT_SHORTCUT_CHATTARGET,		ID_CHAT3,		ANIM_CHAT,		-1,				-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"messagemode4",		 	MNT_SHORTCUT_CHATATTACKER,		ID_CHAT4,		ANIM_CHAT,		-1,				-1,		-1, -1,MNT_SHORTCUT_KEY},
//	{"messagemode5",		 	MNT_SHORTCUT_CHATCLASS,			ID_CHAT5,		ANIM_CHAT,		-1,				-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"+button2",				MNT_SHORTCUT_USEINVENTORY,		ID_USEINVENTORY,ANIM_IDLE,		K_SPACE,		-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"flashlight",				MNT_SHORTCUT_OBJECTIVES,		ID_OBJECTIVES,	ANIM_IDLE,		'q',			-1,		-1, -1,MNT_SHORTCUT_KEY},
	{"thirdPerson",				MNT_SHORTCUT_TOGGLE3RDPERSON,	ID_TOGGLE3DP,	ANIM_IDLE,		K_KP_5,			-1,		-1,	-1,MNT_SHORTCUT_KEY},	
	{"thirdPersonCommit",		MNT_SHORTCUT_COMMITANGLES,		ID_COMMITANGLES,ANIM_IDLE,		K_KP_ENTER,		-1,		-1,	-1,MNT_SHORTCUT_KEY},
	{"thirdPersonRevert",		MNT_SHORTCUT_REVERTANGLES,		ID_REVERTANGLES,ANIM_IDLE,		K_KP_INS,		-1,		-1,	-1,MNT_SHORTCUT_KEY},
	{"thirdPersonReset",		MNT_SHORTCUT_RESETANGLES,		ID_RESETANGLES,	ANIM_IDLE,		K_KP_DEL,		-1,		-1,	-1,MNT_SHORTCUT_KEY},
	{"+thirdPersonForward",		MNT_SHORTCUT_MODELVIEW_FORWARD,	ID_ZOOMFORWARD,	ANIM_IDLE,		K_KP_UPARROW,	-1,		-1,	-1,MNT_SHORTCUT_KEY},
	{"+thirdPersonBackward",	MNT_SHORTCUT_MODELVIEW_BACKWARD,ID_ZOOMBACKWARD,ANIM_IDLE,		K_KP_DOWNARROW,	-1,		-1,	-1,MNT_SHORTCUT_KEY},
	{"+thirdPersonLeft",		MNT_SHORTCUT_MODELVIEW_LEFT,	ID_PANLEFT,		ANIM_IDLE,		K_KP_STAR,		-1,		-1,	-1,MNT_SHORTCUT_KEY},
	{"+thirdPersonRight",		MNT_SHORTCUT_MODELVIEW_RIGHT,	ID_PANRIGHT,	ANIM_IDLE,		K_KP_MINUS,		-1,		-1,	-1,MNT_SHORTCUT_KEY},
	{"+thirdPersonUp",			MNT_SHORTCUT_MODELVIEW_UP,		ID_PANUP,		ANIM_IDLE,		K_KP_PGUP,		-1,		-1,	-1,MNT_SHORTCUT_KEY},			
	{"+thirdPersonDown",		MNT_SHORTCUT_MODELVIEW_DOWN,	ID_PANDOWN,		ANIM_IDLE,		K_KP_DEL,		-1,		-1,	-1,MNT_SHORTCUT_KEY},
	{"+thirdPersonAngleLeft",	MNT_SHORTCUT_MODELVIEW_ROTLEFT,	ID_ROTATELEFT,	ANIM_IDLE,		K_KP_LEFTARROW, -1,		-1,	-1,MNT_SHORTCUT_KEY},
	{"+thirdPersonAngleRight",	MNT_SHORTCUT_MODELVIEW_ROTRIGHT,ID_ROTATERIGHT,	ANIM_IDLE,		K_KP_RIGHTARROW,-1,		-1,	-1,MNT_SHORTCUT_KEY},
	{"+thirdPersonPitchUp",		MNT_SHORTCUT_MODELVIEW_PITCHUP,	ID_PITCHUP,		ANIM_IDLE,		K_KP_HOME,		-1,		-1,	-1,MNT_SHORTCUT_KEY},
	{"+thirdPersonPitchDown",	MNT_SHORTCUT_MODELVIEW_PITCHDOWN,ID_PITCHDOWN,	ANIM_IDLE,		K_KP_END,		-1,		-1,	-1,MNT_SHORTCUT_KEY},
//	{"+analysis",				MNT_SHORTCUT_OBJECTIVES,		ID_OBJECTIVES,	ANIM_IDLE,		-1,				-1,		-1, -1,MNT_SHORTCUT_KEY},
	{(char*)NULL,				0,								0,				0,				-1,				-1,		-1,	-1,0},
};

static configcvar_t g_configcvars[] =
{
	{"cl_run",			0,					0},
	{"m_pitch",			0,					0},
	{"cg_autoswitch",	0,					0},
	{"sensitivity",		0,					0},
	{"in_joystick",		0,					0},
	{"joy_threshold",	0,					0},
	{"m_filter",		0,					0},
	{"cl_freelook",		0,					0},
	{"cl_anglespeedkey",0,					0},
	{"joy_xbutton",		0,					0},
	{"joy_ybutton",		0,					0},
	{NULL,				0,					0}
};

static void* g_attack_controls[] =
{
	&s_attack_weapon1_action,
	&s_attack_weapon2_action,
	&s_attack_weapon3_action,
	&s_attack_weapon4_action,
	&s_attack_weapon5_action,
	&s_attack_weapon0_action,
	/*&s_attack_weapon6_action,
	&s_attack_weapon7_action,
	&s_attack_weapon8_action,
	&s_attack_weapon9_action,*/
	&s_attack_weapon_next_action, 
	&s_attack_weapon_prev_action, 
	NULL,
};


static void* g_move_controls[] =
{
	&s_move_walkforward_action,
	&s_move_backpedal_action,
	&s_move_turnleft_action,      
	&s_move_turnright_action,     
	&s_move_run_action,            
	&s_move_stepleft_action,      
	&s_move_stepright_action,     
	&s_move_sidestep_action,
	&s_move_moveup_action,        
	&s_move_movedown_action,      
//	&s_alwaysrun_box,     
	NULL
};


static void* g_command_controls[] =
{
	&s_controls.chat2,
	&s_controls.chat,
	&s_controls.chat3,
	//&s_controls.chat4,
	//&s_controls.chat5,
	&s_controls.gesture,
	&s_equip_action,
    &s_drop_action, // RPG-X | Marcin | 04/12/2008
	&s_thirdperson_action,
	&s_thirdpersonrevert_action,
	&s_thirdpersoncommit_action,
	&s_thirdpersonreset_action,
	NULL,
};


static void* g_attacklook_controls[] =
{
	&s_attack_attack_action,
	&s_attack_alt_attack_action,
	&s_attack_use_action,
	&s_attack_use_inv_action,
	&s_attack_objectives,
	&s_look_mouselook_action,
	&s_look_lookup_action, 
	&s_look_lookdown_action, 
// 
	&s_look_centerview_action, 
	&s_zoomview_action,
	&s_controls.showscores,
	NULL,
};

static void* g_mouse_controls[] =
{
	&s_controls.freelook, 
	&s_controls.sensitivity,
	&s_controls.invertmouse, 
	&s_controls.smoothmouse,
	NULL,
};

static void* g_modelview_controls[] = 
{
	&s_zoomforward_action,
	&s_zoombackward_action,
	&s_panleft_action,
	&s_panright_action,
	&s_panup_action,
	&s_pandown_action,
	&s_rotateleft_action,
	&s_rotateright_action,
	&s_pitchup_action,
	&s_pitchdown_action,
	NULL,
};

static void** g_controls[] =
{
	g_attack_controls,
	g_command_controls,
	g_attacklook_controls,
	g_move_controls,
	g_modelview_controls,
	g_mouse_controls,
	g_command_controls
};

/*static menucommon_s *g_movement_controls[] =
{
	(menucommon_s *)&s_controls.alwaysrun,     
	(menucommon_s *)&s_controls.run,            
	(menucommon_s *)&s_controls.walkforward,
	(menucommon_s *)&s_controls.backpedal,
	(menucommon_s *)&s_controls.stepleft,      
	(menucommon_s *)&s_controls.stepright,     
	(menucommon_s *)&s_controls.moveup,        
	(menucommon_s *)&s_controls.movedown,      
	(menucommon_s *)&s_controls.turnleft,      
	(menucommon_s *)&s_controls.turnright,     
	(menucommon_s *)&s_controls.sidestep,
	NULL
};*/


/*static menucommon_s *g_looking_controls[] = 
{
	(menucommon_s *)&s_controls.sensitivity,
	(menucommon_s *)&s_controls.smoothmouse,
	(menucommon_s *)&s_controls.invertmouse,
	(menucommon_s *)&s_controls.lookup,
	(menucommon_s *)&s_controls.lookdown,
	(menucommon_s *)&s_controls.mouselook,
	(menucommon_s *)&s_controls.freelook,
	(menucommon_s *)&s_controls.centerview,
	(menucommon_s *)&s_controls.zoomview,
	(menucommon_s *)&s_controls.joyenable,
	(menucommon_s *)&s_controls.joythreshold,
	NULL,
};*/

/*static menucommon_s *g_misc_controls[] = 
{
	(menucommon_s *)&s_controls.showscores, 
	(menucommon_s *)&s_controls.useitem,
	(menucommon_s *)&s_controls.gesture,
	(menucommon_s *)&s_controls.chat,
	(menucommon_s *)&s_controls.chat2,
	(menucommon_s *)&s_controls.chat3,
	(menucommon_s *)&s_controls.chat4,
	(menucommon_s *)&s_controls.chat5,
	(menucommon_s *)&s_controls.gesture,
	(menucommon_s *)&s_controls.showscores,
	NULL,
};*/

/*
=================
Setup_ResetDefaults_Action
=================
*/
void Setup_ResetDefaults_Action( qboolean result ) {
	if( !result ) {
		return;
	}
	trap_Cmd_ExecuteText( EXEC_APPEND, "exec default.cfg\n");
	trap_Cmd_ExecuteText( EXEC_APPEND, "cvar_restart\n");
	trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart\n" );
}

/*
=================
Controls_InitCvars
=================
*/
static void Controls_InitCvars( void )
{
	int				i;
	configcvar_t*	cvarptr;

	cvarptr = g_configcvars;
	for (i=0; ;i++,cvarptr++)
	{
		if (!cvarptr->name)
			break;

		// get current value
		cvarptr->value = trap_Cvar_VariableValue( cvarptr->name );

		// get default value
		trap_Cvar_Reset( cvarptr->name );
		cvarptr->defaultvalue = trap_Cvar_VariableValue( cvarptr->name );

		// restore current value
		trap_Cvar_SetValue( cvarptr->name, cvarptr->value );
	}
}

/*
=================
Controls_GetCvarDefault
=================
*/
/*
static float Controls_GetCvarDefault( char* name )
{
	configcvar_t*	cvarptr;
	int				i;

	cvarptr = g_configcvars;
	for (i=0; ;i++,cvarptr++)
	{
		if (!cvarptr->name)
			return (0);

		if (!strcmp(cvarptr->name,name))
			break;
	}

	return (cvarptr->defaultvalue);
}
*/
/*
=================
Controls_GetCvarValue
=================
*/
static float Controls_GetCvarValue( char* name )
{
	configcvar_t*	cvarptr;
	int				i;

	cvarptr = g_configcvars;
	for (i=0; ;i++,cvarptr++)
	{
		if (!cvarptr->name)
			return (0);

		if (!strcmp(cvarptr->name,name))
			break;
	}

	return (cvarptr->value);
}


/*
=================
Controls_UpdateModel
=================
*/

static void Controls_UpdateModel( int anim ) 
{
	VectorClear( s_controls.playerViewangles );
	VectorClear( s_controls.playerMoveangles );
	s_controls.playerViewangles[YAW]	= uis.lastYaw; //180 + 10
	s_controls.playerMoveangles[YAW]	= s_controls.playerViewangles[YAW];
	s_controls.playerLegs				= BOTH_STAND1; //LEGS_IDLE
	s_controls.playerWeapon				= -1;
	s_controls.playerChat				= qfalse;
	s_controls.playerTorso				= BOTH_STAND1;

	switch( anim ) 
	{
		//TiM : the anim selection code from PMove was adapted to this so it'll play the same animations
		//per weapon from ingame :)
		case ANIM_RUN:
			s_controls.playerTorso	= UI_GetAnim( ANIM_RUN, s_controls.playerWeapon, qtrue );
			s_controls.playerLegs	= UI_GetAnim( ANIM_RUN, s_controls.playerWeapon, qfalse ); //LEGS_RUN
			break;

		case ANIM_WALK:	
			s_controls.playerTorso	= UI_GetAnim( ANIM_WALK, s_controls.playerWeapon, qtrue );
			s_controls.playerLegs	= UI_GetAnim( ANIM_WALK, s_controls.playerWeapon, qfalse );//LEGS_WALK
			break;

		case ANIM_BACK:	
			s_controls.playerTorso	= UI_GetAnim( ANIM_BACK, s_controls.playerWeapon, qtrue );
			s_controls.playerLegs	= UI_GetAnim( ANIM_BACK, s_controls.playerWeapon, qfalse ); //LEGS_BACK
			break;

		case ANIM_JUMP:	
			s_controls.playerTorso	= UI_GetAnim( ANIM_JUMP, s_controls.playerWeapon, qtrue );
			s_controls.playerLegs	= UI_GetAnim( ANIM_JUMP, s_controls.playerWeapon, qfalse ); //LEGS_JUMP
			break;

		case ANIM_CROUCH:	
			s_controls.playerTorso = UI_GetAnim( ANIM_CROUCH, s_controls.playerWeapon, qtrue );
			s_controls.playerLegs	= UI_GetAnim( ANIM_CROUCH, s_controls.playerWeapon, qfalse ); //LEGS_IDLECR
			break;

		case ANIM_TURNLEFT:
			s_controls.playerViewangles[YAW] += 90;
			break;

		case ANIM_TURNRIGHT:
			s_controls.playerViewangles[YAW] -= 90;
			break;

		case ANIM_STEPLEFT:
			s_controls.playerTorso = UI_GetAnim( ANIM_WALK, s_controls.playerWeapon, qtrue );
			s_controls.playerLegs = UI_GetAnim( ANIM_WALK, s_controls.playerWeapon, qfalse ); //LEGS_WALK
			s_controls.playerMoveangles[YAW] = s_controls.playerViewangles[YAW] + 90;
			break;

		case ANIM_STEPRIGHT:
			s_controls.playerTorso = UI_GetAnim( ANIM_WALK, s_controls.playerWeapon, qtrue );
			s_controls.playerLegs = UI_GetAnim( ANIM_WALK, s_controls.playerWeapon, qfalse );
			s_controls.playerMoveangles[YAW] = s_controls.playerViewangles[YAW] - 90;
			break;

		case ANIM_LOOKUP:
			s_controls.playerViewangles[PITCH] = -45;
			break;

		case ANIM_LOOKDOWN:
			s_controls.playerViewangles[PITCH] = 45;
			break;

		case ANIM_WEAPON1:
			s_controls.playerWeapon = WP_2;
			break;

		case ANIM_WEAPON2:
			s_controls.playerWeapon = WP_5;
			break;

		case ANIM_WEAPON3:
			s_controls.playerWeapon = WP_8;
			break;

		case ANIM_WEAPON4:
			s_controls.playerWeapon = WP_12;
			break;

		case ANIM_WEAPON5:
			s_controls.playerWeapon = WP_15;
			break;

		/*case ANIM_WEAPON6:
			s_controls.playerWeapon = WP_8;
			break;

		case ANIM_WEAPON7:
			s_controls.playerWeapon = WP_7;
			break;

		case ANIM_WEAPON8:
			s_controls.playerWeapon = WP_12;
			break;

		case ANIM_WEAPON9:
			s_controls.playerWeapon = WP_13;
			break;*/

		case ANIM_WEAPON10:
			s_controls.playerWeapon = WP_1;
			break;

		case ANIM_ATTACK:
			s_controls.playerTorso = UI_GetAnim( ANIM_ATTACK, s_controls.playerWeapon, qtrue );
			break;

		case ANIM_GESTURE:
			//s_controls.playerTorso = TORSO_GESTURE;
			//FIXME: qboolean talking?
			break;

		case ANIM_DIE:
			s_controls.playerLegs = BOTH_DEATH1;
			s_controls.playerTorso = BOTH_DEATH1;
			s_controls.playerWeapon = WP_0;
			break;

		case ANIM_CHAT:
			s_controls.playerChat = qtrue;
			break;

		default:
			break;
	}

	UI_PlayerInfo_SetInfo( &s_controls.playerinfo, s_controls.playerLegs, s_controls.playerTorso, s_controls.playerViewangles, s_controls.playerMoveangles, s_controls.playerWeapon, trap_Cvar_VariableValue( "height" ), trap_Cvar_VariableValue( "weight" ), s_controls.playerChat );
}


/*
=================
Controls_DrawKeyBinding
=================
*/
static void Controls_DrawKeyBinding( void *self )
{
	menuaction_s*	a;
	int				x,bindingX;
	int				y;
	int				b1;
	int				b2;
	qboolean		c;
	char			name[32];
	char			name2[32];
	int				color,bindingtextcolor, buttontextcolor;
	int				width;

	a = (menuaction_s*) self;

	if (a->generic.flags & QMF_HIDDEN)	// It's bloody invisible
	{
		return;
	}

	x =	a->generic.x;
	y = a->generic.y;

	c = (Menu_ItemAtCursor( a->generic.parent ) == a);

	// Set up bindings
	b1 = g_bindings[a->generic.id].bind1;
	if (b1 == -1)
		strcpy(name,"???");
	else
	{
		trap_Key_KeynumToStringBuf( b1, name, 32 );
		Q_strupr(name);

		b2 = g_bindings[a->generic.id].bind2;
		if (b2 != -1)
		{
			trap_Key_KeynumToStringBuf( b2, name2, 32 );
			Q_strupr(name2);
			strcat( name, va(" %s ",menu_normal_text[MNT_OR]));
			strcat( name, name2 );
			if (g_bindings[a->generic.id].bind3>=0)
			{
				strcat( name, " ...");
			}
		}
	}

	// Waiting for key input, and this isn't the chosen key so gray it out
	if ((s_controls.waitingforkey) && (a != (menuaction_s*) chosenitem))
	{
		color = CT_MDGREY;
		bindingtextcolor = CT_MDGREY;
		buttontextcolor = CT_BLACK;
		a->generic.flags |= QMF_INACTIVE;
	}
	else	// Normal key colors
	{
		a->generic.flags &= ~QMF_INACTIVE;
		bindingtextcolor = CT_WHITE;

		// Keep the chosenitem lit up even when mouse isn't over it
		// or if mouse is over it
		if (((s_controls.waitingforkey) && (a == (menuaction_s*) chosenitem)) || (c))
		{
			buttontextcolor = CT_WHITE;
			color = CT_LTORANGE;
		}
		else
		{
			buttontextcolor = CT_BLACK;
			color = CT_DKORANGE;
		}
	}	

	if ((c) && (g_bindings[a->generic.id].desc))
	{
		if (menu_normal_text[g_bindings[a->generic.id].desc])
		{
			UI_DrawProportionalString( a->generic.parent->descX, a->generic.parent->descY, menu_normal_text[g_bindings[a->generic.id].desc], UI_LEFT|UI_TINYFONT, colorTable[CT_BLACK]);
		}
	}


	width = a->width;

	if (!width)
	{
		// Print button
		trap_R_SetColor( colorTable[color]);
		UI_DrawHandlePic( x , y,  19,  MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
		UI_DrawHandlePic( x + (SMALLCHAR_WIDTH * 11), y,  -19,  MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);	//right
		UI_DrawHandlePic( x + 8,  y,  (SMALLCHAR_WIDTH * 11),  MENU_BUTTON_MED_HEIGHT, uis.whiteShader);
		trap_R_SetColor( NULL );
		bindingX =x + (SMALLCHAR_WIDTH * 12);
	}
	else
	{
		width -= 8 + 8;

		// Print button
		trap_R_SetColor( colorTable[color]);
		// Left end
		UI_DrawHandlePic( x , y,  16,  MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
		// Right end
		UI_DrawHandlePic( x + width, y,  -16,  MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);	//right
		// Middle
		UI_DrawHandlePic( x + 8,  y,  width,  MENU_BUTTON_MED_HEIGHT, uis.whiteShader);

		bindingX = x + 8 + width + 8 + 6;

		trap_R_SetColor( NULL );
	}

	// Button text
	if (g_bindings[a->generic.id].label)
	{
		UI_DrawProportionalString( x + a->textX, y + a->textY, menu_normal_text[g_bindings[a->generic.id].label], UI_LEFT|UI_SMALLFONT, colorTable[buttontextcolor] );
	}

	// Binding text
	UI_DrawProportionalString(  bindingX, y, name, UI_SMALLFONT, colorTable[bindingtextcolor] );

}

/*
=================
Controls_DrawPlayer
=================
*/
//BOOKMARK
static void Controls_DrawPlayer( void *self )
{
	menubitmap_s	*b;
	char			buf[MAX_QPATH];
	vec3_t			origin={ -22, 2, -3 };

	trap_Cvar_VariableStringBuffer( "model", buf, sizeof( buf ) );
	if ( strcmp( buf, s_controls.playerModel ) != 0 ) {

		UI_PlayerInfo_SetModel( &s_controls.playerinfo, buf );
		strcpy( s_controls.playerModel, buf );
		Controls_UpdateModel( ANIM_IDLE );
	}

	b = (menubitmap_s*) self;
	UI_DrawPlayer( b->generic.x, b->generic.y, b->width, b->height, origin, &s_controls.playerinfo, uis.realtime/2 );
}


/*
=================
Controls_GetKeyAssignment
=================
*/
static void Controls_GetKeyAssignment (char *command, int *twokeys)
{
	int		count;
	int		j;
	char	b[256];

	twokeys[0] = twokeys[1] = twokeys[2] = -1;
	count = 0;

	for ( j = 0; j < 256; j++ )
	{
		trap_Key_GetBindingBuf( j, b, 256 );
		if ( *b == 0 ) {
			continue;
		}
		if ( !Q_stricmp( b, command ) ) {
			twokeys[count] = j;
			count++;
			if (count == 3)
				break;
		}
	}
}

/*
=================
Controls_ClearKeyAssignment
=================
*/
static void Controls_ClearKeyAssignment (char *command)
{
	int		i;
	char	b[256];

	for ( i = 0; i < 256; i++ )
	{
		trap_Key_GetBindingBuf( i, b, 256 );
		if ( *b == 0 ) 
		{
			continue;
		}
		if ( !Q_stricmp( b, command ) ) 
		{
			trap_Key_SetBinding( i, "" );
		}
	}


}

/*
=================
Controls_GetConfig
=================
*/
static void Controls_GetConfig( void )
{
	int		i;
	int		twokeys[3];
	bind_t*	bindptr;

	// put the bindings into a local store
	bindptr = g_bindings;

	// iterate each command, get its numeric binding
	for (i=0; ;i++,bindptr++)
	{
		if (!bindptr->label)
			break;

		Controls_GetKeyAssignment(bindptr->command, twokeys);

		bindptr->bind1 = twokeys[0];
		bindptr->bind2 = twokeys[1];
		bindptr->bind3 = twokeys[2];
	}

	s_controls.invertmouse.curvalue  = Controls_GetCvarValue( "m_pitch" ) < 0;
	s_controls.smoothmouse.curvalue  = UI_ClampCvar( 0, 1, Controls_GetCvarValue( "m_filter" ) );
	s_controls.alwaysrun.curvalue    = UI_ClampCvar( 0, 1, Controls_GetCvarValue( "cl_run" ) );
	s_controls.autoswitch.curvalue   = UI_ClampCvar( 0, 2, Controls_GetCvarValue( "cg_autoswitch" ) );
	s_controls.sensitivity.curvalue  = UI_ClampCvar( 2, 30, Controls_GetCvarValue( "sensitivity" ) );
	s_controls.joyenable.curvalue    = UI_ClampCvar( 0, 1, Controls_GetCvarValue( "in_joystick" ) );
	s_controls.joythreshold.curvalue = UI_ClampCvar( 0.05, 0.75, Controls_GetCvarValue( "joy_threshold" ) );
	s_controls.freelook.curvalue     = UI_ClampCvar( 0, 1, Controls_GetCvarValue( "cl_freelook" ) );
	s_keyturnspeed_slider.curvalue   = UI_ClampCvar( 1, 5, Controls_GetCvarValue( "cl_anglespeedkey" ) );
	s_joyxbutton_box.curvalue		 = UI_ClampCvar( 0, 1, Controls_GetCvarValue( "joy_xbutton" ) );
	s_joyybutton_box.curvalue        = UI_ClampCvar( 0, 1, Controls_GetCvarValue( "joy_ybutton" ) );
}

/*
=================
Controls_SetJoystick
=================
*/
static void Controls_SetJoystick( void )
{

	trap_Cvar_SetValue( "in_joystick", s_controls.joyenable.curvalue );
	trap_Cmd_ExecuteText( EXEC_APPEND, "in_restart\n" );
}

/*
=================
Controls_SetConfig
=================
*/
static void Controls_SetConfig( void )
{
	int		i;
	bind_t*	bindptr;

	// unbind the command from all keys
//	for (i=0; i<256; i++)
//	{
//		trap_Key_SetBinding( i, "" );
//	}

	// set the bindings from the local store
	bindptr = g_bindings;

	// iterate each command, get its numeric binding
	for (i=0; ;i++,bindptr++)
	{
		if (!bindptr->label)
			break;

		if (bindptr->bind1 != -1)
		{	
			trap_Key_SetBinding( bindptr->bind1, bindptr->command );

			if (bindptr->bind2 != -1)
				trap_Key_SetBinding( bindptr->bind2, bindptr->command );
		}
	}


	if ( s_controls.invertmouse.curvalue )
		trap_Cvar_SetValue( "m_pitch", -fabs( trap_Cvar_VariableValue( "m_pitch" ) ) );
	else
		trap_Cvar_SetValue( "m_pitch", fabs( trap_Cvar_VariableValue( "m_pitch" ) ) );

	trap_Cvar_SetValue( "m_filter", s_controls.smoothmouse.curvalue );
	trap_Cvar_SetValue( "cl_run", s_controls.alwaysrun.curvalue );
	trap_Cvar_SetValue( "cg_autoswitch", s_controls.autoswitch.curvalue );
	trap_Cvar_SetValue( "sensitivity", s_controls.sensitivity.curvalue );
	trap_Cvar_SetValue( "joy_threshold", s_controls.joythreshold.curvalue );
	trap_Cvar_SetValue( "cl_freelook", s_controls.freelook.curvalue );
	trap_Cvar_SetValue( "cl_anglespeedkey", s_keyturnspeed_slider.curvalue );
	trap_Cvar_SetValue( "joy_xbutton", s_joyxbutton_box.curvalue );
	trap_Cvar_SetValue( "joy_ybutton", s_joyybutton_box.curvalue );
}

/*
=================
Controls_SetDefaults
=================
*/
/*
static void Controls_SetDefaults( void )
{
	int	i;
	bind_t*	bindptr;

	// set the bindings from the local store
	bindptr = g_bindings;

	// iterate each command, set its default binding
	for (i=0; ;i++,bindptr++)
	{
		if (!bindptr->label)
			break;

		bindptr->bind1 = bindptr->defaultbind1;
		bindptr->bind2 = bindptr->defaultbind2;
	}

	s_controls.invertmouse.curvalue  = Controls_GetCvarDefault( "m_pitch" ) < 0;
	s_controls.smoothmouse.curvalue  = Controls_GetCvarDefault( "m_filter" );
	s_controls.alwaysrun.curvalue    = Controls_GetCvarDefault( "cl_run" );
	s_controls.autoswitch.curvalue   = Controls_GetCvarDefault( "cg_autoswitch" );
	s_controls.sensitivity.curvalue  = Controls_GetCvarDefault( "sensitivity" );
	s_controls.joyenable.curvalue    = Controls_GetCvarDefault( "in_joystick" );
	s_controls.joythreshold.curvalue = Controls_GetCvarDefault( "joy_threshold" );
	s_controls.freelook.curvalue     = Controls_GetCvarDefault( "cl_freelook" );
}
*/
/*
=================
Controls_MenuKey
=================
*/
static sfxHandle_t Controls_MenuKey( int key )
{
	int			id;
	int			i;
	qboolean	found;
	bind_t*		bindptr;
	menuframework_s* current_menu;

	found = qfalse;

	switch (g_section)
	{
		default:
		case C_WEAPONS:
			current_menu = &s_weapons_menu;
			break;
		case C_COMMAND:
			current_menu = &s_controlscommand_menu;
			break;
		case C_LOOK:
			current_menu = &s_controlslook_menu;
			break;
		case C_MOVE:
			current_menu = &s_controlsmove_menu;
			break;
		case C_MOUSE:
			current_menu = &s_controlsmouse_menu;
			break;
		case C_MODELVIEW:
			current_menu = &s_controlsmodelview_menu;
			break;
	}

	if (!s_controls.waitingforkey)
	{
		switch (key)
		{
			case K_BACKSPACE:
			case K_DEL:
			case K_KP_DEL:
				key = -1;
				break;
		
			case K_MOUSE2:
			case K_ESCAPE:
				if (s_controls.changesmade)
					Controls_SetConfig();
				goto ignorekey;	

			default:
				goto ignorekey;
		}
	}
	else
	{
		if (key & K_CHAR_FLAG)
			goto ignorekey;

		if ((key<1) || (key>256))	// Ignore high ascii keys
		{
			return (menu_null_sound);
		}

		switch (key)
		{
			case K_ESCAPE:
				s_controls.waitingforkey = qfalse;
				// Turn off the waiting for key message
				s_attack_waiting_action.generic.flags			= QMF_HIDDEN;
				Controls_UpdateNew();
				return (menu_out_sound);
	
			case '`':
				goto ignorekey;
		}
	}

	s_controls.changesmade = qtrue;
	
	if (key != -1)
	{
		// remove from any other bind
		bindptr = g_bindings;
		for (i=0; ;i++,bindptr++)
		{
			if (!bindptr->label)	
				break;

			if (bindptr->bind3 == key)
			{
				bindptr->bind3 = -1;
			}
			else if (bindptr->bind2 == key)
			{
				bindptr->bind2 = bindptr->bind3;
				bindptr->bind3 = -1;
			}
			else if (bindptr->bind1 == key)
			{
				bindptr->bind1 = bindptr->bind2;	
				bindptr->bind2 = bindptr->bind3;	
				bindptr->bind3 = -1;
			}

		}
	}

	// Turn off the waiting for key message
	s_attack_waiting_action.generic.flags			= QMF_HIDDEN;

	// assign key to local store
//	id      = ((menucommon_s*)(s_controls.menu.items[s_controls.menu.cursor]))->id;
	id      = ((menucommon_s*)(current_menu->items[current_menu->cursor]))->id;
	bindptr = g_bindings;
	for (i=0; ;i++,bindptr++)
	{
		if (!bindptr->label)	
			break;
		
		if (bindptr->id == id)
		{
			found = qtrue;
			if (key == -1)
			{

				Controls_ClearKeyAssignment (bindptr->command);
				bindptr->bind1 = -1;
				bindptr->bind2 = -1;
				bindptr->bind3 = -1;
/*
				if( bindptr->bind1 != -1 ) 
				{
					trap_Key_SetBinding( bindptr->bind1, "" );
					bindptr->bind1 = -1;
				}
				if( bindptr->bind2 != -1 ) 
				{
					trap_Key_SetBinding( bindptr->bind2, "" );
					bindptr->bind2 = -1;
				}*/
			}
			else if (bindptr->bind1 == -1) 
			{
				bindptr->bind1 = key;
			}
			else if (bindptr->bind1 != key && bindptr->bind2 == -1) 
			{
				bindptr->bind2 = key;
			}
			else
			{
				bindptr->bind3 = bindptr->bind2;
				bindptr->bind2 = bindptr->bind1;
				bindptr->bind1 = key;
			}						
			break;
		}
	}				
		
	s_controls.waitingforkey = qfalse;
	s_controls.changesmade = qfalse;

	if (found)
	{	
		Controls_SetConfig();
		Controls_UpdateNew();
		return (menu_out_sound);
	}

ignorekey:
	return Menu_DefaultKey( current_menu, key );
}

/*
=================
Controls_ResetDefaults_Action
=================
*/
/*
static void Controls_ResetDefaults_Action( qboolean result ) 
{
	if( !result ) {
		return;
	}

	s_controls.changesmade = qtrue;
	Controls_SetDefaults();
	Controls_UpdateNew();
}
*/
/*
=================
Controls_ActionEvent
=================
*/
static void Controls_ActionEvent( void* ptr, int event )
{
	menuframework_s*	menu;

	if (event == QM_LOSTFOCUS)
	{
		Controls_UpdateModel( ANIM_IDLE );
	}
	else if (event == QM_GOTFOCUS)
	{
		Controls_UpdateModel( g_bindings[((menucommon_s*)ptr)->id].anim );
	}
	else if ((event == QM_ACTIVATED) && !s_controls.waitingforkey)
	{
		s_controls.waitingforkey = 1;
		// Show your waiting for data
		s_attack_waiting_action.generic.flags &= ~QMF_HIDDEN;
		s_attack_waiting_action.generic.flags |= QMF_BLINK;
		// Turn off 'apply action' so it won't flash
//		s_attack_apply_action.generic.flags = QMF_GRAYED;

		// Show this it the chosen item
		menu = ((menucommon_s*)ptr)->parent;
		chosenitem = (menucommon_s *) Menu_ItemAtCursor(((menuframework_s*)menu));

		Controls_UpdateNew();
	}
}

/*
=================
Controls_InitModel
=================
*/
static void Controls_InitModel( void )
{
	memset( &s_controls.playerinfo, 0, sizeof(playerInfo_t) );

	UI_PlayerInfo_SetModel( &s_controls.playerinfo, UI_Cvar_VariableString( "model" ) );

	Controls_UpdateModel( ANIM_IDLE );
}

/*
=================
SetupMenu_TopButtons
=================
*/
void SetupMenu_TopButtons(menuframework_s *menu,int menuType,menuaction_s *s_video_apply_action)
{
	vid_apply_action = s_video_apply_action;

	s_controls_mainmenu.generic.type			= MTYPE_BITMAP;      
	s_controls_mainmenu.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_mainmenu.generic.x				= 482;
	s_controls_mainmenu.generic.y				= 136;
	s_controls_mainmenu.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	if (menuType != MENU_VIDEODATA)
	{
		s_controls_mainmenu.generic.callback	= Controls_MenuEvent;
	}
	else	// How do you spell HACK?
	{
		s_controls_mainmenu.generic.callback	= Controls_MenuEventVideo;
	}
	s_controls_mainmenu.width					= MENU_BUTTON_MED_WIDTH;
	s_controls_mainmenu.height					= MENU_BUTTON_MED_HEIGHT;
	s_controls_mainmenu.color					= CT_DKPURPLE1;
	s_controls_mainmenu.color2					= CT_LTPURPLE1;
	s_controls_mainmenu.textX					= 5;
	s_controls_mainmenu.textY					= 2;
	if (!ingameFlag)
	{
		s_controls_mainmenu.textEnum			= MBT_MAINMENU;
		s_controls_mainmenu.generic.id			= ID_MAINMENU;
	}
	else	// In game menu
	{
		s_controls_mainmenu.textEnum			= MBT_INGAMEMENU;
		s_controls_mainmenu.generic.id			= ID_INGAMEMENU;
	}
	s_controls_mainmenu.textcolor				= CT_BLACK;
	s_controls_mainmenu.textcolor2				= CT_WHITE;

	s_controls_controls.generic.type		= MTYPE_BITMAP;      
	s_controls_controls.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_controls.generic.x		= setup_menubuttons[0][0];
	s_controls_controls.generic.y		= setup_menubuttons[0][1];
	s_controls_controls.generic.name	= BUTTON_GRAPHIC_LONGRIGHT;
	s_controls_controls.generic.id		= ID_CONTROLS;
	if (menuType != MENU_VIDEODATA)
	{
		s_controls_controls.generic.callback	= Controls_MenuEvent;
	}
	else	// How do you spell HACK?
	{
		s_controls_controls.generic.callback	= Controls_MenuEventVideo;
	}
	s_controls_controls.width			= MENU_BUTTON_MED_WIDTH;
	s_controls_controls.height			= MENU_BUTTON_MED_HEIGHT;
	s_controls_controls.color			= CT_DKPURPLE1;
	s_controls_controls.color2			= CT_LTPURPLE1;
	s_controls_controls.textX			= 5;
	s_controls_controls.textY			= 2;
	s_controls_controls.textEnum		= MBT_CONTROLS;
	s_controls_controls.textcolor		= CT_BLACK;
	s_controls_controls.textcolor2		= CT_WHITE;

	s_controls_video.generic.type		= MTYPE_BITMAP;      
	s_controls_video.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_video.generic.x		= setup_menubuttons[1][0];
	s_controls_video.generic.y		= setup_menubuttons[1][1];
	s_controls_video.generic.name	= BUTTON_GRAPHIC_LONGRIGHT;
	s_controls_video.generic.id		= ID_VIDEO;
	if (menuType != MENU_VIDEODATA)
	{
		s_controls_video.generic.callback	= Controls_MenuEvent;
	}
	else	// How do you spell HACK?
	{
		s_controls_video.generic.callback	= Controls_MenuEventVideo;
	}
	s_controls_video.width			= MENU_BUTTON_MED_WIDTH;
	s_controls_video.height			= MENU_BUTTON_MED_HEIGHT;
	s_controls_video.color			= CT_DKPURPLE1;
	s_controls_video.color2			= CT_LTPURPLE1;
	s_controls_video.textX			= 5;
	s_controls_video.textY			= 2;
	s_controls_video.textEnum		= MBT_VIDEOSETTINGS;
	s_controls_video.textcolor		= CT_BLACK;
	s_controls_video.textcolor2		= CT_WHITE;

	s_controls_sound.generic.type		= MTYPE_BITMAP;      
	s_controls_sound.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_sound.generic.x		= setup_menubuttons[2][0];
	s_controls_sound.generic.y		= setup_menubuttons[2][1];
	s_controls_sound.generic.name	= BUTTON_GRAPHIC_LONGRIGHT;
	s_controls_sound.generic.id		= ID_SOUND;
	if (menuType != MENU_VIDEODATA)
	{
		s_controls_sound.generic.callback	= Controls_MenuEvent;
	}
	else	// How do you spell HACK?
	{
		s_controls_sound.generic.callback	= Controls_MenuEventVideo;
	}
	s_controls_sound.width			= MENU_BUTTON_MED_WIDTH;
	s_controls_sound.height			= MENU_BUTTON_MED_HEIGHT;
	s_controls_sound.color			= CT_DKPURPLE1;
	s_controls_sound.color2			= CT_LTPURPLE1;
	s_controls_sound.textX			= 5;
	s_controls_sound.textY			= 2;
	s_controls_sound.textEnum		= MBT_SOUNDSETTINGS;
	s_controls_sound.textcolor		= CT_BLACK;
	s_controls_sound.textcolor2		= CT_WHITE;

	s_controls_game.generic.type		= MTYPE_BITMAP;      
	s_controls_game.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_game.generic.x		= setup_menubuttons[3][0];
	s_controls_game.generic.y		= setup_menubuttons[3][1];
	s_controls_game.generic.name	=BUTTON_GRAPHIC_LONGRIGHT;
	s_controls_game.generic.id		= ID_GAMEOPTIONS;
	if (menuType != MENU_VIDEODATA)
	{
		s_controls_game.generic.callback	= Controls_MenuEvent;
	}
	else	// How do you spell HACK?
	{
		s_controls_game.generic.callback	= Controls_MenuEventVideo;
	}
	s_controls_game.width			= MENU_BUTTON_MED_WIDTH;
	s_controls_game.height			= MENU_BUTTON_MED_HEIGHT;
	s_controls_game.color			= CT_DKPURPLE1;
	s_controls_game.color2			= CT_LTPURPLE1;
	s_controls_game.textX			= 5;
	s_controls_game.textY			= 2;
	s_controls_game.textEnum		= MBT_GAMEOPTIONS;
	s_controls_game.textcolor		= CT_BLACK;
	s_controls_game.textcolor2		= CT_WHITE;

	s_controls_player.generic.type		= MTYPE_BITMAP;      
	s_controls_player.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_player.generic.x		= setup_menubuttons[4][0];
	s_controls_player.generic.y		= setup_menubuttons[4][1];
	s_controls_player.generic.name	= BUTTON_GRAPHIC_LONGRIGHT;
	s_controls_player.generic.id		= ID_PLAYER;
	if (menuType != MENU_VIDEODATA)
	{
		s_controls_player.generic.callback	= Controls_MenuEvent;
	}
	else	// How do you spell HACK?
	{
		s_controls_player.generic.callback	= Controls_MenuEventVideo;
	}
	s_controls_player.width			= MENU_BUTTON_MED_WIDTH;
	s_controls_player.height			= MENU_BUTTON_MED_HEIGHT;
	s_controls_player.color			= CT_DKPURPLE1;
	s_controls_player.color2			= CT_LTPURPLE1;
	s_controls_player.textX			= 5;
	s_controls_player.textY			= 2;
	s_controls_player.textEnum		= MBT_PLAYER;
	s_controls_player.textcolor		= CT_BLACK;
	s_controls_player.textcolor2		= CT_WHITE;

	s_controls_default.generic.type		= MTYPE_BITMAP;      
	s_controls_default.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_default.generic.x		= setup_menubuttons[5][0];
	s_controls_default.generic.y		= setup_menubuttons[5][1];
	s_controls_default.generic.name		= "menu/common/bar1.tga";
	s_controls_default.generic.id		= ID_CONTROLSDEFAULT;
	if (menuType != MENU_VIDEODATA)
	{
		s_controls_default.generic.callback	= Controls_MenuEvent;
	}
	else	// How do you spell HACK?
	{
		s_controls_default.generic.callback	= Controls_MenuEventVideo;
	}
	s_controls_default.width			= MENU_BUTTON_MED_WIDTH;
	s_controls_default.height			= MENU_BUTTON_MED_HEIGHT;
	s_controls_default.color			= CT_DKPURPLE1;
	s_controls_default.color2			= CT_LTPURPLE1;
	s_controls_default.textX			= 5;
	s_controls_default.textY			= 2;
	s_controls_default.textEnum			= MBT_SETDEFAULT;
	s_controls_default.textcolor		= CT_BLACK;
	s_controls_default.textcolor2		= CT_WHITE;

	s_controls_cdkey.generic.type		= MTYPE_BITMAP;      
	s_controls_cdkey.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_cdkey.generic.x		= setup_menubuttons[6][0];
	s_controls_cdkey.generic.y		= setup_menubuttons[6][1];
	s_controls_cdkey.generic.name	= BUTTON_GRAPHIC_LONGRIGHT;
	s_controls_cdkey.generic.id		= ID_CDKEY;
	if (menuType != MENU_VIDEODATA)
	{
		s_controls_cdkey.generic.callback	= Controls_MenuEvent;
	}
	else	// How do you spell HACK?
	{
		s_controls_cdkey.generic.callback	= Controls_MenuEventVideo;
	}
	s_controls_cdkey.width			= MENU_BUTTON_MED_WIDTH;
	s_controls_cdkey.height			= MENU_BUTTON_MED_HEIGHT;
	s_controls_cdkey.color			= CT_DKPURPLE1;
	s_controls_cdkey.color2			= CT_LTPURPLE1;
	s_controls_cdkey.textX			= 5;
	s_controls_cdkey.textY			= 2;
	s_controls_cdkey.textEnum		= MBT_CDKEY;
	s_controls_cdkey.textcolor		= CT_BLACK;
	s_controls_cdkey.textcolor2		= CT_WHITE;

	if (uis.demoversion)
	{
		s_controls_cdkey.generic.flags			|= QMF_GRAYED;
	}
/*
	s_controls_network.generic.type		= MTYPE_BITMAP;      
	s_controls_network.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_network.generic.x		= setup_menubuttons[6][0];
	s_controls_network.generic.y		= setup_menubuttons[6][1];
	s_controls_network.generic.name		= "menu/common/bar1.tga";
	s_controls_network.generic.id		= ID_NETWORK;
	s_controls_network.generic.callback	= Controls_MenuEvent;
	s_controls_network.width			= MENU_BUTTON_MED_WIDTH;
	s_controls_network.height			= MENU_BUTTON_MED_HEIGHT;
	s_controls_network.color			= CT_DKPURPLE1;
	s_controls_network.color2			= CT_LTPURPLE1;
	s_controls_network.textX			= 5;
	s_controls_network.textY			= 2;
	s_controls_network.textEnum			= MBT_NETWORK;
	s_controls_network.textcolor		= CT_BLACK;
	s_controls_network.textcolor2		= CT_WHITE;
*/
	s_controls_fonts.generic.type		= MTYPE_BITMAP;      
	s_controls_fonts.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_fonts.generic.x		= setup_menubuttons[7][0];
	s_controls_fonts.generic.y		= setup_menubuttons[7][1];
	s_controls_fonts.generic.name	= BUTTON_GRAPHIC_LONGRIGHT;
	s_controls_fonts.generic.id		= ID_FONTS;
	if (menuType != MENU_VIDEODATA)
	{
		s_controls_fonts.generic.callback	= Controls_MenuEvent;
	}
	else	// How do you spell HACK?
	{
		s_controls_fonts.generic.callback	= Controls_MenuEventVideo;
	}
	s_controls_fonts.width			= 100;
	s_controls_fonts.height			= MENU_BUTTON_MED_HEIGHT;
	s_controls_fonts.color			= CT_DKPURPLE1;
	s_controls_fonts.color2			= CT_LTPURPLE1;
	s_controls_fonts.textX			= 5;
	s_controls_fonts.textY			= 2;
	s_controls_fonts.textEnum		= MBT_FONTS;
	s_controls_fonts.textcolor		= CT_BLACK;
	s_controls_fonts.textcolor2		= CT_WHITE;

	if (menuType!=MENU_PLAYER)
	{
		Menu_AddItem( menu, ( void * )&s_controls_mainmenu);
	}
	Menu_AddItem( menu, ( void * )&s_controls_controls);
	Menu_AddItem( menu, ( void * )&s_controls_video);
	Menu_AddItem( menu, ( void * )&s_controls_sound);
	Menu_AddItem( menu, ( void * )&s_controls_game);
	Menu_AddItem( menu, ( void * )&s_controls_player);
	Menu_AddItem( menu, ( void * )&s_controls_default);
	Menu_AddItem( menu, ( void * )&s_controls_cdkey);

	switch (menuType)
	{
	case MENU_CONTROLS :
		s_controls_controls.textcolor		= CT_LTGOLD1;	
		s_controls_controls.textcolor2		= CT_LTGOLD1;	
		s_controls_controls.generic.flags	= QMF_GRAYED;
		break;
	case MENU_VIDEO :
	case MENU_VIDEODATA :
		s_controls_video.textcolor		= CT_LTGOLD1;	
		s_controls_video.textcolor2		= CT_LTGOLD1;	
		s_controls_video.generic.flags	= QMF_GRAYED;
		break;
	case MENU_SOUND :
		s_controls_sound.textcolor		= CT_LTGOLD1;	
		s_controls_sound.textcolor2		= CT_LTGOLD1;	
		s_controls_sound.generic.flags	= QMF_GRAYED;
		break;
	case MENU_GAME :
		s_controls_game.textcolor		= CT_LTGOLD1;	
		s_controls_game.textcolor2		= CT_LTGOLD1;	
		s_controls_game.generic.flags	= QMF_GRAYED;
		break;
	case MENU_DEFAULT :
		s_controls_default.textcolor		= CT_LTGOLD1;	
		s_controls_default.textcolor2		= CT_LTGOLD1;	
		s_controls_default.generic.flags	= QMF_GRAYED;
		break;
	case MENU_CDKEY :
		s_controls_cdkey.textcolor		= CT_LTGOLD1;	
		s_controls_cdkey.textcolor2		= CT_LTGOLD1;	
		s_controls_cdkey.generic.flags	= QMF_GRAYED;
		break;
	case MENU_PLAYER :
		s_controls_player.textcolor		= CT_LTGOLD1;	
		s_controls_player.textcolor2		= CT_LTGOLD1;	
		s_controls_player.generic.flags	= QMF_GRAYED;
		break;
	case MENU_NETWORK :
		s_controls_network.textcolor		= CT_LTGOLD1;	
		s_controls_network.textcolor2		= CT_LTGOLD1;	
		s_controls_network.generic.flags	= QMF_GRAYED;
		break;
	case MENU_FONTS :
		s_controls_fonts.textcolor		= CT_LTGOLD1;	
		s_controls_fonts.textcolor2		= CT_LTGOLD1;	
		s_controls_fonts.generic.flags	= QMF_GRAYED;
		break;
	}

	if (trap_Cvar_VariableValue("developer"))
		Menu_AddItem( menu, ( void * )&s_controls_fonts);

}

/*
=================
M_WeaponsMenu_Blinkies
=================
*/
void M_WeaponsMenu_Blinkies (void)
{
	int i;

	for (i=0;i<AMG_MAX;++i)
	{
		attackmenu_graphics[i].color = CT_VDKPURPLE1;
	}

	// Don't flash frame unless waiting for input
	if (!s_controls.waitingforkey)
	{
		return;
	}

	if (attackmenu_graphics[AMG_MIDLEFT].timer < uis.realtime)
	{
		attackmenu_graphics[AMG_MIDLEFT].timer = uis.realtime + 500;
		++attackmenu_graphics[AMG_MIDLEFT].target;
		if (attackmenu_graphics[AMG_MIDLEFT].target > 7)
		{
			attackmenu_graphics[AMG_MIDLEFT].target = 0;
		}
	}

	switch (attackmenu_graphics[AMG_MIDLEFT].target)
	{
		case 0:
			attackmenu_graphics[AMG_MIDLEFT].color = CT_LTPURPLE1;
			break;
		case 1:
			attackmenu_graphics[AMG_UPPERLEFT].color = CT_LTPURPLE1;
			attackmenu_graphics[AMG_LOWERLEFT].color = CT_LTPURPLE1;
			break;
		case 2:
			attackmenu_graphics[AMG_UPPERCORNER].color = CT_LTPURPLE1;
			attackmenu_graphics[AMG_LOWERCORNER].color = CT_LTPURPLE1;
			break;
		case 3:
			attackmenu_graphics[AMG_UPPERTOP1ST].color = CT_LTPURPLE1;
			attackmenu_graphics[AMG_LOWERTOP1ST].color = CT_LTPURPLE1;
			break;
		case 4:
			attackmenu_graphics[AMG_UPPERTOP2ND].color = CT_LTPURPLE1;
			attackmenu_graphics[AMG_LOWERTOP2ND].color = CT_LTPURPLE1;
			break;
		case 5:
			attackmenu_graphics[AMG_UPPERSWOOP].color = CT_LTPURPLE1;
			attackmenu_graphics[AMG_LOWERSWOOP].color = CT_LTPURPLE1;
			break;
		case 6:
			attackmenu_graphics[AMG_TOPRIGHT].color = CT_LTPURPLE1;
			attackmenu_graphics[AMG_BOTTOMRIGHT].color = CT_LTPURPLE1;
			break;
		case 7:
			attackmenu_graphics[AMG_PLAYERBKGRND].color = CT_LTPURPLE1;
			break;
	}

}

/*
=================
M_WeaponsMenu_Graphics
=================
*/
void M_WeaponsMenu_Graphics (void)
{

	UI_MenuFrame(&s_weapons_menu);

	UI_DrawProportionalString(  74,  66, "56-129",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "33",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  188, "9893",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "12799",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	M_WeaponsMenu_Blinkies();

	UI_PrintMenuGraphics(attackmenu_graphics,AMG_MAX);

	UI_DrawProportionalString(  607,  174, "981235",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  607,  406, "5672141",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	if (s_attack_waiting_action.generic.flags & QMF_HIDDEN)
	{
		UI_DrawProportionalString(  207,  411, menu_normal_text[MNT_BACKSPACE],UI_SMALLFONT, colorTable[CT_WHITE]);
	}
}

/*
=================
Weapons_MenuDraw
=================
*/
static void Weapons_MenuDraw (void)
{
	M_WeaponsMenu_Graphics();

	UI_Setup_MenuButtons();

	Menu_Draw( &s_weapons_menu );
}
/*
=================
WeaponsMenu_Precache
=================
*/
static void WeaponsMenu_Precache( void )
{
	int i;

	swooshTop = trap_R_RegisterShaderNoMip("menu/common/swoosh_top.tga");
	swooshBottom= trap_R_RegisterShaderNoMip("menu/common/swoosh_bottom.tga");

	// Precache all menu graphics in array
	for (i=0;i<AMG_MAX;++i)
	{
		if (attackmenu_graphics[i].type == MG_GRAPHIC)
		{
			attackmenu_graphics[i].graphic = trap_R_RegisterShaderNoMip(attackmenu_graphics[i].file);
		}
	}

}

/*
=================
Controls_ModelEvent
=================
*/

static void Controls_ModelEvent( void* ptr, int event )
{
	/*menucommon_s*	menu = (menucommon_s*)ptr;
	if (event == QM_LOSTFOCUS)
	{
		g_playerinfo.looking = qfalse;
	}
	else if (event == QM_GOTFOCUS)
	{
		g_playerinfo.looking = menu->x;
	}
	else*/ if ((event == QM_ACTIVATED) )
	{
		uis.spinView = qtrue;
		uis.cursorpx = uis.cursorx;
	}
}

/*
=================
Playermodel_MenuInit
=================
*/
static void Playermodel_MenuInit( void )
{
	s_controls_playermdl.generic.type			= MTYPE_BITMAP;
	s_controls_playermdl.generic.flags			= QMF_SILENT; //INACTIVE
	s_controls_playermdl.generic.callback		= Controls_ModelEvent;
	s_controls_playermdl.generic.ownerdraw		= Controls_DrawPlayer;
	s_controls_playermdl.generic.x				= 501; //455
	s_controls_playermdl.generic.y				= 189; //145
	s_controls_playermdl.width					= 110;//32*5.6
	s_controls_playermdl.height					= 211;//56*5.6

	//Model spin data
	uis.spinView = qfalse;
	uis.lastYaw = 180 - 20;
}

/*
=================
SetupActionButtons_Init
=================
*/
static void SetupActionButtons_Init(int section)
{
	int i,y;
	void**		controlptr;
	menuframework_s *current_menu;


	controlptr = g_controls[g_section];

	switch (g_section)
	{
		default:
		case C_WEAPONS:
			current_menu = &s_weapons_menu;
			break;
		case C_COMMAND:
			current_menu = &s_controlscommand_menu;
			break;
		case C_LOOK:
			current_menu = &s_controlslook_menu;
			break;
		case C_MOVE:
			current_menu = &s_controlsmove_menu;
			break;
		case C_MOUSE:
			current_menu = &s_controlsmouse_menu;
			break;
		case C_MODELVIEW:
			current_menu = &s_controlsmodelview_menu;
			break;
	}


	y = current_menu->listY;

	for (i=0; i<99; i++)	// The 99 is to avoid runaway loops
	{
		if (!controlptr[i])
		{
			break;
		}

		((menuaction_s*)controlptr[i])->generic.x	= current_menu->listX;
		((menuaction_s*)controlptr[i])->generic.y	= y;
		((menuaction_s*)controlptr[i])->textX		= 5;
		((menuaction_s*)controlptr[i])->textY		= 1;
		((menuaction_s*)controlptr[i])->height		= 18;
		((menuaction_s*)controlptr[i])->width		= 150;

		//if ( ((menuaction_s*)controlptr[i])->generic.id == ID_EQUIP || ((menuaction_s*)controlptr[i])->generic.id == ID_WEAPON0 )
		//	y += 40;
		//else
		y += 20;

	}
}

/*
=================
Weapons_MenuInit
=================
*/
static void Weapons_MenuInit( void )
{
	WeaponsMenu_Precache();

	attackmenu_graphics[AMG_MIDLEFT].timer = uis.realtime + 500;

	s_weapons_menu.nitems						= 0;
	s_weapons_menu.wrapAround					= qtrue;
//	s_weapons_menu.opening						= NULL;
//	s_weapons_menu.closing						= NULL;
	s_weapons_menu.draw							= Weapons_MenuDraw;
	s_weapons_menu.key							= Controls_MenuKey;
	s_weapons_menu.fullscreen					= qtrue;
	s_weapons_menu.descX						= MENU_DESC_X;
	s_weapons_menu.descY						= MENU_DESC_Y;
	s_weapons_menu.listX						= 170;
	s_weapons_menu.listY						= 184;
	s_weapons_menu.titleX						= MENU_TITLE_X;
	s_weapons_menu.titleY						= MENU_TITLE_Y;
	s_weapons_menu.titleI						= MNT_CONTROLSMENU_TITLE;
	s_weapons_menu.footNoteEnum					= MNT_WEAPONKEY_SETUP;

	Playermodel_MenuInit();

	SetupMenu_TopButtons(&s_weapons_menu,MENU_CONTROLS,NULL);

	SetupMenu_SideButtons(&s_weapons_menu,MENU_CONTROLS_WEAPON);

	s_controls_weapon.textcolor			= CT_LTGOLD1;
	s_controls_weapon.textcolor2		= CT_LTGOLD1;

	s_attack_weapon0_action.generic.type		= MTYPE_ACTION;
	s_attack_weapon0_action.generic.flags		= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_attack_weapon0_action.generic.callback	= Controls_ActionEvent;
	s_attack_weapon0_action.generic.ownerdraw	= Controls_DrawKeyBinding;
	s_attack_weapon0_action.generic.id			= ID_WEAPON0;	

	s_attack_weapon1_action.generic.type		= MTYPE_ACTION;
	s_attack_weapon1_action.generic.flags		= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_attack_weapon1_action.generic.callback	= Controls_ActionEvent;
	s_attack_weapon1_action.generic.ownerdraw	= Controls_DrawKeyBinding;
	s_attack_weapon1_action.generic.id			= ID_WEAPON1;

	s_attack_weapon2_action.generic.type		= MTYPE_ACTION;
	s_attack_weapon2_action.generic.flags		= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_attack_weapon2_action.generic.callback	= Controls_ActionEvent;
	s_attack_weapon2_action.generic.ownerdraw	= Controls_DrawKeyBinding;
	s_attack_weapon2_action.generic.id			= ID_WEAPON2;

	s_attack_weapon3_action.generic.type		= MTYPE_ACTION;
	s_attack_weapon3_action.generic.flags		= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_attack_weapon3_action.generic.callback	= Controls_ActionEvent;
	s_attack_weapon3_action.generic.ownerdraw	= Controls_DrawKeyBinding;
	s_attack_weapon3_action.generic.id			= ID_WEAPON3;

	s_attack_weapon3_action.generic.type		= MTYPE_ACTION;
	s_attack_weapon3_action.generic.flags		= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_attack_weapon3_action.generic.callback	= Controls_ActionEvent;
	s_attack_weapon3_action.generic.ownerdraw	= Controls_DrawKeyBinding;
	s_attack_weapon3_action.generic.id			= ID_WEAPON3;

	s_attack_weapon4_action.generic.type		= MTYPE_ACTION;
	s_attack_weapon4_action.generic.flags		= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_attack_weapon4_action.generic.callback	= Controls_ActionEvent;
	s_attack_weapon4_action.generic.ownerdraw	= Controls_DrawKeyBinding;
	s_attack_weapon4_action.generic.id			= ID_WEAPON4;

	s_attack_weapon5_action.generic.type	   = MTYPE_ACTION;
	s_attack_weapon5_action.generic.flags     = QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_attack_weapon5_action.generic.callback  = Controls_ActionEvent;
	s_attack_weapon5_action.generic.ownerdraw = Controls_DrawKeyBinding;
	s_attack_weapon5_action.generic.id        = ID_WEAPON5;

	/*s_attack_weapon6_action.generic.type	   = MTYPE_ACTION;
	s_attack_weapon6_action.generic.flags     = QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_attack_weapon6_action.generic.callback  = Controls_ActionEvent;
	s_attack_weapon6_action.generic.ownerdraw = Controls_DrawKeyBinding;
	s_attack_weapon6_action.generic.id        = ID_WEAPON6;

	s_attack_weapon6_action.generic.type	   = MTYPE_ACTION;
	s_attack_weapon6_action.generic.flags     = QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_attack_weapon6_action.generic.callback  = Controls_ActionEvent;
	s_attack_weapon6_action.generic.ownerdraw = Controls_DrawKeyBinding;
	s_attack_weapon6_action.generic.id        = ID_WEAPON6;

	s_attack_weapon7_action.generic.type	   = MTYPE_ACTION;
	s_attack_weapon7_action.generic.flags     = QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_attack_weapon7_action.generic.callback  = Controls_ActionEvent;
	s_attack_weapon7_action.generic.ownerdraw = Controls_DrawKeyBinding;
	s_attack_weapon7_action.generic.id        = ID_WEAPON7;

	s_attack_weapon8_action.generic.type	   = MTYPE_ACTION;
	s_attack_weapon8_action.generic.flags     = QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_attack_weapon8_action.generic.callback  = Controls_ActionEvent;
	s_attack_weapon8_action.generic.ownerdraw = Controls_DrawKeyBinding;
	s_attack_weapon8_action.generic.id        = ID_WEAPON8;

	s_attack_weapon9_action.generic.type	   = MTYPE_ACTION;
	s_attack_weapon9_action.generic.flags     = QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_attack_weapon9_action.generic.callback  = Controls_ActionEvent;
	s_attack_weapon9_action.generic.ownerdraw = Controls_DrawKeyBinding;
	s_attack_weapon9_action.generic.id        = ID_WEAPON9;*/

	s_attack_weapon_next_action.generic.type	   = MTYPE_ACTION;
	s_attack_weapon_next_action.generic.flags     = QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_attack_weapon_next_action.generic.callback  = Controls_ActionEvent;
	s_attack_weapon_next_action.generic.ownerdraw = Controls_DrawKeyBinding;
	s_attack_weapon_next_action.generic.id        = ID_WEAPNEXT;

	s_attack_weapon_prev_action.generic.type	   = MTYPE_ACTION;
	s_attack_weapon_prev_action.generic.flags     = QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_attack_weapon_prev_action.generic.callback  = Controls_ActionEvent;
	s_attack_weapon_prev_action.generic.ownerdraw = Controls_DrawKeyBinding;
	s_attack_weapon_prev_action.generic.id        = ID_WEAPPREV;

	s_attack_waiting_action.generic.type			= MTYPE_ACTION;
	s_attack_waiting_action.generic.flags			= QMF_HIDDEN;
	s_attack_waiting_action.generic.x				= 202;
	s_attack_waiting_action.generic.y				= 410;
	s_attack_waiting_action.textEnum				= MBT_WAITFORKEY;
	s_attack_waiting_action.textcolor				= CT_BLACK;
	s_attack_waiting_action.textcolor2				= CT_WHITE;
	s_attack_waiting_action.textcolor3				= CT_LTGREY;
	s_attack_waiting_action.color					= CT_VDKPURPLE1;//CT_DKPURPLE1;
	s_attack_waiting_action.color2					= CT_VDKPURPLE1;//CT_LTPURPLE1;
	s_attack_waiting_action.color3					= CT_DKGREY;
	s_attack_waiting_action.textX					= 5;
	s_attack_waiting_action.textY					= 1;
	s_attack_waiting_action.width					= 255;
	s_attack_waiting_action.height					= 18;

	g_section        = C_WEAPONS;

	SetupActionButtons_Init(g_section);		// Set up standard values

	Menu_AddItem( &s_weapons_menu, ( void * )&s_controls_playermdl);
	Menu_AddItem( &s_weapons_menu, ( void * )&s_attack_weapon0_action);
	Menu_AddItem( &s_weapons_menu, ( void * )&s_attack_weapon1_action);
	Menu_AddItem( &s_weapons_menu, ( void * )&s_attack_weapon2_action);
	Menu_AddItem( &s_weapons_menu, ( void * )&s_attack_weapon3_action);
	Menu_AddItem( &s_weapons_menu, ( void * )&s_attack_weapon4_action);
	Menu_AddItem( &s_weapons_menu, ( void * )&s_attack_weapon5_action);
	/*Menu_AddItem( &s_weapons_menu, ( void * )&s_attack_weapon6_action);
	Menu_AddItem( &s_weapons_menu, ( void * )&s_attack_weapon7_action);
	Menu_AddItem( &s_weapons_menu, ( void * )&s_attack_weapon8_action);
	Menu_AddItem( &s_weapons_menu, ( void * )&s_attack_weapon9_action);*/
	Menu_AddItem( &s_weapons_menu, ( void * )&s_attack_weapon_next_action);
	Menu_AddItem( &s_weapons_menu, ( void * )&s_attack_weapon_prev_action);
	Menu_AddItem( &s_weapons_menu, ( void * )&s_attack_waiting_action);


	// initialize the configurable cvars
	Controls_InitCvars();

	// initialize the current config
	Controls_GetConfig();

	// initial default section
	s_controls.waitingforkey  = qfalse;
	g_changesmade    = qfalse;
	g_prompttosave   = qfalse;

	// update the ui
	Controls_UpdateNew();
}


/*
=================
UI_SetupWeaponsMenu
=================
*/
void UI_SetupWeaponsMenu( void )
{
//	if (!s_weapons_menu.initialized)
//	{
		Weapons_MenuInit();
//	}

	UI_PushMenu( &s_weapons_menu );
}
/*
===============
SetupMenu_SideButtons
===============
*/
static void SetupMenu_SideButtons(menuframework_s *menu,int menuType)
{
	int x,y,inc;

	y = 204;
	inc = 6;
	x = 30;

	s_controls_weapon.generic.type			= MTYPE_BITMAP;      
	s_controls_weapon.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_weapon.generic.x				= x;
	s_controls_weapon.generic.y				= y;
	s_controls_weapon.generic.name			= GRAPHIC_SQUARE;
	s_controls_weapon.generic.id			= ID_ATTACK;
	s_controls_weapon.generic.callback		= Controls_MenuEvent;
	s_controls_weapon.width					= MENU_BUTTON_MED_WIDTH - 10;
	s_controls_weapon.height				= MENU_BUTTON_MED_HEIGHT;
	s_controls_weapon.color					= CT_DKPURPLE1;
	s_controls_weapon.color2				= CT_LTPURPLE1;
	s_controls_weapon.textX					= 5;
	s_controls_weapon.textY					= 1;
	s_controls_weapon.textEnum				= MBT_WEAPONKEYS;
	s_controls_weapon.textcolor				= CT_BLACK;
	s_controls_weapon.textcolor2			= CT_WHITE;

	y += inc+MENU_BUTTON_MED_HEIGHT;
	s_controls_look.generic.type			= MTYPE_BITMAP;      
	s_controls_look.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_look.generic.x				= x;
	s_controls_look.generic.y				= y;
	s_controls_look.generic.name			= GRAPHIC_SQUARE;
	s_controls_look.generic.id				= ID_CONTROLSLOOK;
	s_controls_look.generic.callback		= Controls_MenuEvent;
	s_controls_look.width					= MENU_BUTTON_MED_WIDTH - 10;
	s_controls_look.height					= MENU_BUTTON_MED_HEIGHT;
	s_controls_look.color					= CT_DKPURPLE1;
	s_controls_look.color2					= CT_LTPURPLE1;
	s_controls_look.textX					= 5;
	s_controls_look.textY					= 1;
	s_controls_look.textEnum				= MBT_LOOKKEYS;
	s_controls_look.textcolor				= CT_BLACK;
	s_controls_look.textcolor2				= CT_WHITE;

	y += inc+MENU_BUTTON_MED_HEIGHT;
	s_controls_movement.generic.type		= MTYPE_BITMAP;      
	s_controls_movement.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_movement.generic.x			= x;
	s_controls_movement.generic.y			= y;
	s_controls_movement.generic.name		= GRAPHIC_SQUARE;
	s_controls_movement.generic.id			= ID_CONTROLSMOVE;
	s_controls_movement.generic.callback	= Controls_MenuEvent;
	s_controls_movement.width				= MENU_BUTTON_MED_WIDTH - 10;
	s_controls_movement.height				= MENU_BUTTON_MED_HEIGHT;
	s_controls_movement.color				= CT_DKPURPLE1;
	s_controls_movement.color2				= CT_LTPURPLE1;
	s_controls_movement.textX				= 5;
	s_controls_movement.textY				= 1;
	s_controls_movement.textEnum			= MBT_MOVEMENTKEYS;
	s_controls_movement.textcolor			= CT_BLACK;
	s_controls_movement.textcolor2			= CT_WHITE;

	y += inc+MENU_BUTTON_MED_HEIGHT;
	s_controls_command.generic.type			= MTYPE_BITMAP;      
	s_controls_command.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_command.generic.x			= x;
	s_controls_command.generic.y			= y;
	s_controls_command.generic.name			= GRAPHIC_SQUARE;
	s_controls_command.generic.id			= ID_CONTROLSCOMMAND;
	s_controls_command.generic.callback		= Controls_MenuEvent;
	s_controls_command.width				= MENU_BUTTON_MED_WIDTH - 10;
	s_controls_command.height				= MENU_BUTTON_MED_HEIGHT;
	s_controls_command.color				= CT_DKPURPLE1;
	s_controls_command.color2				= CT_LTPURPLE1;
	s_controls_command.textX				= 5;
	s_controls_command.textY				= 1;
	s_controls_command.textEnum				= MBT_COMMANDKEYS;
	s_controls_command.textcolor			= CT_BLACK;
	s_controls_command.textcolor2			= CT_WHITE;

	y += inc+MENU_BUTTON_MED_HEIGHT;
	s_controls_modelview.generic.type		= MTYPE_BITMAP;      
	s_controls_modelview.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_modelview.generic.x			= x;
	s_controls_modelview.generic.y			= y;
	s_controls_modelview.generic.name		= GRAPHIC_SQUARE;
	s_controls_modelview.generic.id			= ID_CONTROLSMODELVIEW; 
	s_controls_modelview.generic.callback	= Controls_MenuEvent;
	s_controls_modelview.width				= MENU_BUTTON_MED_WIDTH - 10;
	s_controls_modelview.height				= MENU_BUTTON_MED_HEIGHT;
	s_controls_modelview.color				= CT_DKPURPLE1;
	s_controls_modelview.color2				= CT_LTPURPLE1;
	s_controls_modelview.textX				= 5;
	s_controls_modelview.textY				= 1;
	s_controls_modelview.textEnum			= MBT_MODELVIEWKEYS;
	s_controls_modelview.textcolor			= CT_BLACK;
	s_controls_modelview.textcolor2			= CT_WHITE;

	y += inc+MENU_BUTTON_MED_HEIGHT;
	y += inc+MENU_BUTTON_MED_HEIGHT-3;
	s_controls_mouse.generic.type			= MTYPE_BITMAP;      
	s_controls_mouse.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_mouse.generic.x				= x;
	s_controls_mouse.generic.y				= y;
	s_controls_mouse.generic.name			= GRAPHIC_SQUARE;
	s_controls_mouse.generic.id				= ID_CONTROLSMOUSE;
	s_controls_mouse.generic.callback		= Controls_MenuEvent;
	s_controls_mouse.width					= MENU_BUTTON_MED_WIDTH - 10;
	s_controls_mouse.height					= MENU_BUTTON_MED_HEIGHT;
	s_controls_mouse.color					= CT_DKPURPLE1;
	s_controls_mouse.color2					= CT_LTPURPLE1;
	s_controls_mouse.textX					= 5;
	s_controls_mouse.textY					= 1;
	s_controls_mouse.textEnum				= MBT_MOUSECONFIG;
	s_controls_mouse.textcolor				= CT_BLACK;
	s_controls_mouse.textcolor2				= CT_WHITE;

	y += inc+MENU_BUTTON_MED_HEIGHT;
	s_controls_other.generic.type			= MTYPE_BITMAP;      
	s_controls_other.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_other.generic.x				= x;
	s_controls_other.generic.y				= y;
	s_controls_other.generic.name			= GRAPHIC_SQUARE;
	s_controls_other.generic.id				= ID_CONTROLSOTHER;
	s_controls_other.generic.callback		= Controls_MenuEvent;
	s_controls_other.width					= MENU_BUTTON_MED_WIDTH - 10;
	s_controls_other.height					= MENU_BUTTON_MED_HEIGHT;
	s_controls_other.color					= CT_DKPURPLE1;
	s_controls_other.color2					= CT_LTPURPLE1;
	s_controls_other.textX					= 5;
	s_controls_other.textY					= 1;
	s_controls_other.textEnum				= MBT_OTHEROPTIONS;
	s_controls_other.textcolor				= CT_BLACK;
	s_controls_other.textcolor2				= CT_WHITE;

/*	y += inc+MENU_BUTTON_MED_HEIGHT;
	s_controls_loadconfig.generic.type		= MTYPE_BITMAP;      
	s_controls_loadconfig.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_loadconfig.generic.x			= x;
	s_controls_loadconfig.generic.y			= y;
	s_controls_loadconfig.generic.name		= "menu/common/square.tga";
	s_controls_loadconfig.generic.id		= ID_MAINMENU;
	s_controls_loadconfig.generic.callback	= Controls_MenuEvent;
	s_controls_loadconfig.width				= MENU_BUTTON_MED_WIDTH - 10;
	s_controls_loadconfig.height			= MENU_BUTTON_MED_HEIGHT;
	s_controls_loadconfig.color				= CT_DKPURPLE1;
	s_controls_loadconfig.color2			= CT_LTPURPLE1;
	s_controls_loadconfig.textX				= 5;
	s_controls_loadconfig.textY				= 2;
	s_controls_loadconfig.textEnum			= MBT_LOADCONFIG;
	s_controls_loadconfig.textcolor			= CT_BLACK;
	s_controls_loadconfig.textcolor2		= CT_WHITE;

	y += inc+MENU_BUTTON_MED_HEIGHT;
	s_controls_saveconfig.generic.type		= MTYPE_BITMAP;      
	s_controls_saveconfig.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_saveconfig.generic.x			= x;
	s_controls_saveconfig.generic.y			= y;
	s_controls_saveconfig.generic.name		= "menu/common/square.tga";
	s_controls_saveconfig.generic.id		= ID_MAINMENU;
	s_controls_saveconfig.generic.callback	= Controls_MenuEvent;
	s_controls_saveconfig.width				= MENU_BUTTON_MED_WIDTH - 10;
	s_controls_saveconfig.height			= MENU_BUTTON_MED_HEIGHT;
	s_controls_saveconfig.color				= CT_DKPURPLE1;
	s_controls_saveconfig.color2			= CT_LTPURPLE1;
	s_controls_saveconfig.textX				= 5;
	s_controls_saveconfig.textY				= 2;
	s_controls_saveconfig.textEnum			= MBT_SAVECONFIG;
	s_controls_saveconfig.textcolor			= CT_BLACK;
	s_controls_saveconfig.textcolor2		= CT_WHITE;

*/
	switch (menuType)
	{
	case MENU_CONTROLS_WEAPON :
		s_controls_weapon.generic.flags			= QMF_GRAYED;
		break;
	case MENU_CONTROLS_LOOK :
		s_controls_look.generic.flags			= QMF_GRAYED;
		break;
	case MENU_CONTROLS_MOVEMENT :
		s_controls_movement.generic.flags		= QMF_GRAYED;
		break;
	case MENU_CONTROLS_COMMAND :
		s_controls_command.generic.flags		= QMF_GRAYED;
		break;
	case MENU_CONTROLS_MODELVIEW:
		s_controls_modelview.generic.flags		= QMF_GRAYED;
		break;
	case MENU_CONTROLS_MOUSE :
		s_controls_mouse.generic.flags			= QMF_GRAYED;
		break;
	case MENU_CONTROLS_OTHER :
		s_controls_other.generic.flags			= QMF_GRAYED;
		break;
	}

	Menu_AddItem( menu, ( void * )&s_controls_weapon);
	Menu_AddItem( menu, ( void * )&s_controls_look);
	Menu_AddItem( menu, ( void * )&s_controls_movement);
	Menu_AddItem( menu, ( void * )&s_controls_command);
	Menu_AddItem( menu, ( void * )&s_controls_modelview);
	
	Menu_AddItem( menu, ( void * )&s_controls_mouse);
	Menu_AddItem( menu, ( void * )&s_controls_other);
//	Menu_AddItem( menu, ( void * )&s_controls_loadconfig);
//	Menu_AddItem( menu, ( void * )&s_controls_saveconfig);

}
/*
=================
Controls_Update
=================
*/
static void Controls_UpdateNew( void )
{
	int			i;
	int			j;
	void**		controlptr;
	menuframework_s *current_menu;


	switch (g_section)
	{
		default:
		case C_WEAPONS:
			current_menu = &s_weapons_menu;
			break;
		case C_COMMAND:
			current_menu = &s_controlscommand_menu;
			break;
		case C_LOOK:
			current_menu = &s_controlslook_menu;
			break;
		case C_MOVE:
			current_menu = &s_controlsmove_menu;
			break;
		case C_MOUSE:
			current_menu = &s_controlsmouse_menu;
			break;
		case C_MODELVIEW:
			current_menu = &s_controlsmodelview_menu;
			break;
	}


	// enable specified controls
	controlptr = g_controls[g_section];
	for (i=0,j=0; ;i++,j++,controlptr++)
	{
		if (!controlptr[0])
		{
			// end of list
			break;
		}
	
		((menucommon_s*)controlptr[0])->flags &= ~(QMF_GRAYED);
	}


	if (s_controls.waitingforkey)
	{
		((menucommon_s*)(current_menu->items[current_menu->cursor]))->flags &= ~QMF_HIGHLIGHT;
		return;
	}

}

void ControlsVideoDataAction( qboolean result ) 
{
	if ( result )	// Yes - do it
	{
		Controls_MenuEvent (holdControlPtr, holdControlEvent);
	}
}
/*
=================
Controls_MenuEventVideo
=================
*/
static void Controls_MenuEventVideo (void* ptr, int event)
{

	if (event != QM_ACTIVATED)
		return;

	holdControlPtr = ptr;
	holdControlEvent = event;

	if (vid_apply_action->generic.flags & QMF_BLINK)	// Video apply changes button is flashing
	{
		UI_ConfirmMenu(menu_normal_text[MNT_LOOSEVIDSETTINGS], 0, ControlsVideoDataAction);	
	}
	else	// Go ahead, act normal
	{
		Controls_MenuEvent (holdControlPtr, holdControlEvent);
	}
}


/*
=================
Controls_MenuEvent
=================
*/
static void Controls_MenuEvent (void* ptr, int event)
{
	menuframework_s*	m;

	if (event != QM_ACTIVATED)
		return;


	m = ((menucommon_s*)ptr)->parent;

	switch (((menucommon_s*)ptr)->id)
	{
		case ID_CONTROLS:
			UI_PopMenu();			// Get rid of whatever is ontop
			UI_SetupWeaponsMenu();			// Move to the Controls Menu
			break;

		case ID_ATTACK:
			UI_PopMenu();			// Get rid of whatever is ontop
			UI_SetupWeaponsMenu();	// Move to the Command Menu
			break;

		case ID_VIDEO:
			UI_PopMenu();			// Get rid of whatever is ontop
			UI_VideoDataMenu();		// Move to the Video Menu
			break;

		case ID_SOUND:
			UI_PopMenu();			// Get rid of whatever is ontop
			UI_SoundMenu();			// Move to the Sound Menu
			break;

		case ID_GAMEOPTIONS:
			UI_PopMenu();			// Get rid of whatever is ontop
			UI_GameOptionsMenu();	// Move to the Game Options Menu
			break;

		case ID_CDKEY:
			UI_PopMenu();			// Get rid of whatever is ontop
			UI_CDKeyMenu();			// Move to the CD Key Menu
			break;

		case ID_NETWORK:
			UI_PopMenu();			// Get rid of whatever is ontop
			UI_NetworkOptionsMenu();			// Move to the CD Key Menu
			break;

		case ID_PLAYER:
//			UI_PopMenu();				// Get rid of whatever is ontop
			UI_PlayerSettingsMenu(PS_MENU_CONTROLS);	// Move to the Player Settings Menu
			break;

		case ID_MAINMENU:
			UI_PopMenu();
			break;

		case ID_INGAMEMENU:
			UI_PopMenu();
			break;

		case ID_CONTROLSCOMMAND:
			UI_PopMenu();				// Get rid of whatever is ontop
			UI_ControlsCommandMenu();	// Move to the Command Menu
			break;

		case ID_CONTROLSMODELVIEW:		//TiM - Modelview menu
			UI_PopMenu();				//Uh... what the others said lol
			UI_ControlsModelViewMenu();	//Yeah... Move to the Modelview menu
			break;						//Crazy copy and paste crazy Raven coder rofl...

		case ID_CONTROLSLOOK:
			UI_PopMenu();				// Get rid of whatever is ontop
			UI_ControlsAttackLookMenu();		// Move to the Look Menu
			break;

		case ID_CONTROLSMOVE:
			UI_PopMenu();				// Get rid of whatever is ontop
			UI_ControlsMoveMenu();		// Move to the Move Menu
			break;

		case ID_CONTROLSMOUSE:
			UI_PopMenu();				// Get rid of whatever is ontop
			UI_ControlsMouseJoyStickMenu();		// Move to the Mouse Menu
			break;

		case ID_CONTROLSOTHER:
			UI_PopMenu();				// Get rid of whatever is ontop
			UI_ControlsOtherMenu();		// Move to the Other Menu
			break;

		case ID_CONTROLSDEFAULT:
			UI_PopMenu();				// Get rid of whatever is ontop
			UI_ControlsDefaultMenu();	// Move to the Default Menu
			break;

		case ID_FONTS:
			UI_PopMenu();				// Get rid of whatever is ontop
			UI_FontsMenu();				// Move to the Font Menu
			break;


		case ID_JOYENABLE:
			Controls_SetJoystick();
			break;

		case ID_FREELOOK:
		case ID_MOUSESPEED:
		case ID_INVERTMOUSE:
		case ID_SMOOTHMOUSE:
		case ID_ALWAYSRUN:
		case ID_AUTOSWITCH:
		case ID_JOYTHRESHOLD:
		case ID_KEYTURNPEED:
		case ID_LOOKSPRING:
		case ID_AUTOSWITCHWEAPONS:

			Controls_SetConfig();
//			if (event == QM_ACTIVATED)
//			{
//				s_controls.changesmade = qtrue;
//			}
			break;		
	}
}
/*
=================
M_ControlsMoveMenu_Graphics
=================
*/
void M_ControlsMoveMenu_Graphics (void)
{
	UI_MenuFrame(&s_controlsmove_menu);

	UI_DrawProportionalString(  74,  66, "7-2345",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "803",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  188, "32811",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "560-99",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	M_WeaponsMenu_Blinkies();

	UI_PrintMenuGraphics(attackmenu_graphics,AMG_MAX);

	UI_DrawProportionalString(  607,  174, "634579",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  607,  406, "2815689",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	if (s_attack_waiting_action.generic.flags & QMF_HIDDEN)
	{
		UI_DrawProportionalString(  207,  411, menu_normal_text[MNT_BACKSPACE],UI_SMALLFONT, colorTable[CT_WHITE]);
	}
}

/*
=================
ControlsMove_MenuDraw
=================
*/
static void ControlsMove_MenuDraw (void)
{
	M_ControlsMoveMenu_Graphics();

	UI_Setup_MenuButtons();

	Menu_Draw( &s_controlsmove_menu );
}

/*
=================
ControlsMove_MenuInit
=================
*/
static void ControlsMove_MenuInit( void )
{
	s_controlsmove_menu.nitems				= 0;
	s_controlsmove_menu.wrapAround			= qtrue;
	s_controlsmove_menu.draw				= ControlsMove_MenuDraw;
	s_controlsmove_menu.key					= Controls_MenuKey;
	s_controlsmove_menu.fullscreen			= qtrue;
	s_controlsmove_menu.descX				= MENU_DESC_X;
	s_controlsmove_menu.descY				= MENU_DESC_Y;
	s_controlsmove_menu.listX				= 170;
	s_controlsmove_menu.listY				= 188;
	s_controlsmove_menu.titleX				= MENU_TITLE_X;
	s_controlsmove_menu.titleY				= MENU_TITLE_Y;
	s_controlsmove_menu.titleI				= MNT_CONTROLSMENU_TITLE;
	s_controlsmove_menu.footNoteEnum		= MNT_MOVEMENTKEYS_SETUP;

	Playermodel_MenuInit();

	SetupMenu_TopButtons(&s_controlsmove_menu,MENU_CONTROLS,NULL);
	SetupMenu_SideButtons(&s_controlsmove_menu,MENU_CONTROLS_MOVEMENT);

	s_controls_movement.textcolor					= CT_LTGOLD1;
	s_controls_movement.textcolor2					= CT_LTGOLD1;

	s_move_walkforward_action.generic.type			= MTYPE_ACTION;
	s_move_walkforward_action.generic.flags			= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_move_walkforward_action.generic.callback		= Controls_ActionEvent;
	s_move_walkforward_action.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_move_walkforward_action.generic.id 			= ID_FORWARD;

	s_move_backpedal_action.generic.type			= MTYPE_ACTION;
	s_move_backpedal_action.generic.flags			= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_move_backpedal_action.generic.callback		= Controls_ActionEvent;
	s_move_backpedal_action.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_move_backpedal_action.generic.id 				= ID_BACKPEDAL;

	s_move_turnleft_action.generic.type				= MTYPE_ACTION;
	s_move_turnleft_action.generic.flags			= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_move_turnleft_action.generic.callback			= Controls_ActionEvent;
	s_move_turnleft_action.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_move_turnleft_action.generic.id				= ID_LEFT;

	s_move_turnright_action.generic.type			= MTYPE_ACTION;
	s_move_turnright_action.generic.flags			= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_move_turnright_action.generic.callback		= Controls_ActionEvent;
	s_move_turnright_action.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_move_turnright_action.generic.id				= ID_RIGHT;

	s_move_run_action.generic.type					= MTYPE_ACTION;
	s_move_run_action.generic.flags					= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_move_run_action.generic.callback				= Controls_ActionEvent;
	s_move_run_action.generic.ownerdraw				= Controls_DrawKeyBinding;
	s_move_run_action.generic.id					= ID_SPEED;

	s_move_stepleft_action.generic.type				= MTYPE_ACTION;
	s_move_stepleft_action.generic.flags			= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_move_stepleft_action.generic.callback			= Controls_ActionEvent;
	s_move_stepleft_action.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_move_stepleft_action.generic.id 				= ID_MOVELEFT;

	s_move_stepright_action.generic.type			= MTYPE_ACTION;
	s_move_stepright_action.generic.flags			= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_move_stepright_action.generic.callback		= Controls_ActionEvent;
	s_move_stepright_action.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_move_stepright_action.generic.id				= ID_MOVERIGHT;

	s_move_sidestep_action.generic.type				= MTYPE_ACTION;
	s_move_sidestep_action.generic.flags			= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_move_sidestep_action.generic.callback			= Controls_ActionEvent;
	s_move_sidestep_action.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_move_sidestep_action.generic.id				= ID_STRAFE;

	s_move_moveup_action.generic.type				= MTYPE_ACTION;
	s_move_moveup_action.generic.flags				= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_move_moveup_action.generic.callback			= Controls_ActionEvent;
	s_move_moveup_action.generic.ownerdraw			= Controls_DrawKeyBinding;
	s_move_moveup_action.generic.id					= ID_MOVEUP;

	s_move_movedown_action.generic.type				= MTYPE_ACTION;
	s_move_movedown_action.generic.flags			= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_move_movedown_action.generic.callback			= Controls_ActionEvent;
	s_move_movedown_action.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_move_movedown_action.generic.id				= ID_MOVEDOWN;

	s_attack_waiting_action.generic.type			= MTYPE_ACTION;
	s_attack_waiting_action.generic.flags			= QMF_HIDDEN;
	s_attack_waiting_action.generic.x				= 202;
	s_attack_waiting_action.generic.y				= 410;
	s_attack_waiting_action.textEnum				= MBT_WAITFORKEY;
	s_attack_waiting_action.textcolor				= CT_BLACK;
	s_attack_waiting_action.textcolor2				= CT_WHITE;
	s_attack_waiting_action.textcolor3				= CT_LTGREY;
	s_attack_waiting_action.color					= CT_VDKPURPLE1;//CT_DKPURPLE1;
	s_attack_waiting_action.color2					= CT_VDKPURPLE1;//CT_LTPURPLE1;
	s_attack_waiting_action.color3					= CT_DKGREY;
	s_attack_waiting_action.textX					= 5;
	s_attack_waiting_action.textY					= 1;
	s_attack_waiting_action.width					= 255;
	s_attack_waiting_action.height					= 18;

	g_section        = C_MOVE;
	SetupActionButtons_Init(g_section);		// Set up standard values

	Menu_AddItem( &s_controlsmove_menu, ( void * )&s_controls_playermdl);
	Menu_AddItem( &s_controlsmove_menu, ( void * )&s_move_walkforward_action);
	Menu_AddItem( &s_controlsmove_menu, ( void * )&s_move_backpedal_action);
	Menu_AddItem( &s_controlsmove_menu, ( void * )&s_move_turnleft_action);
	Menu_AddItem( &s_controlsmove_menu, ( void * )&s_move_turnright_action);
	Menu_AddItem( &s_controlsmove_menu, ( void * )&s_move_run_action);
	Menu_AddItem( &s_controlsmove_menu, ( void * )&s_move_stepleft_action);
	Menu_AddItem( &s_controlsmove_menu, ( void * )&s_move_stepright_action);
	Menu_AddItem( &s_controlsmove_menu, ( void * )&s_move_sidestep_action);
	Menu_AddItem( &s_controlsmove_menu, ( void * )&s_move_moveup_action);
	Menu_AddItem( &s_controlsmove_menu, ( void * )&s_move_movedown_action);
	Menu_AddItem( &s_controlsmove_menu, ( void * )&s_attack_waiting_action);

	// initialize the configurable cvars
	Controls_InitCvars();

	// initialize the current config
	Controls_GetConfig();

	// intialize the model
//	Controls_InitModel();

	// initial default section
	s_controls.waitingforkey  = qfalse;
	g_changesmade    = qfalse;
//	g_prompttosave   = qfalse;

	// update the ui
	Controls_UpdateNew();
}


/*
=================
UI_ControlsMoveMenu
=================
*/
static void UI_ControlsMoveMenu( void )
{
	ControlsMove_MenuInit();

	UI_PushMenu( &s_controlsmove_menu );
}

/*
=================
M_ControlsLookMenu_Graphics
=================
*/
void M_ControlsLookMenu_Graphics (void)
{
	UI_MenuFrame(&s_controlslook_menu);

	UI_DrawProportionalString(  74,  66, "3567",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "9003",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  188, "1425",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "6780-1",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	M_WeaponsMenu_Blinkies();

	UI_PrintMenuGraphics(attackmenu_graphics,AMG_MAX);

	UI_DrawProportionalString(  607,  174, "556541",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  607,  406, "909090",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	if (s_attack_waiting_action.generic.flags & QMF_HIDDEN)
	{
		UI_DrawProportionalString(  207,  411, menu_normal_text[MNT_BACKSPACE],UI_SMALLFONT, colorTable[CT_WHITE]);
	}
}

/*
=================
ControlsAttackLook_MenuDraw
=================
*/
static void ControlsAttackLook_MenuDraw (void)
{
	M_ControlsLookMenu_Graphics();

	UI_Setup_MenuButtons();

	Menu_Draw( &s_controlslook_menu );
}

/*
=================
ControlsAttackLook_MenuInit
=================
*/
static void ControlsAttackLook_MenuInit( void )
{
	s_controlslook_menu.nitems					= 0;
	s_controlslook_menu.wrapAround				= qtrue;
	s_controlslook_menu.draw					= ControlsAttackLook_MenuDraw;
	s_controlslook_menu.key						= Controls_MenuKey;
	s_controlslook_menu.fullscreen				= qtrue;
	s_controlslook_menu.descX					= MENU_DESC_X;
	s_controlslook_menu.descY					= MENU_DESC_Y;
	s_controlslook_menu.listX					= 170;
	s_controlslook_menu.listY					= 188;
	s_controlslook_menu.titleX					= MENU_TITLE_X;
	s_controlslook_menu.titleY					= MENU_TITLE_Y;
	s_controlslook_menu.titleI					= MNT_CONTROLSMENU_TITLE;
	s_controlslook_menu.footNoteEnum			= MNT_ATTACKLOOKKEY_SETUP;

	Playermodel_MenuInit();

	SetupMenu_TopButtons(&s_controlslook_menu,MENU_CONTROLS,NULL);
	SetupMenu_SideButtons(&s_controlslook_menu,MENU_CONTROLS_LOOK);

	s_controls_look.textcolor					= CT_LTGOLD1;
	s_controls_look.textcolor2					= CT_LTGOLD1;

	s_attack_attack_action.generic.type			= MTYPE_ACTION;
	s_attack_attack_action.generic.flags		= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_attack_attack_action.generic.callback		= Controls_ActionEvent;
	s_attack_attack_action.generic.ownerdraw	= Controls_DrawKeyBinding;
	s_attack_attack_action.generic.id			= ID_ATTACK;

	s_attack_alt_attack_action.generic.type			= MTYPE_ACTION;
	s_attack_alt_attack_action.generic.flags		= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_attack_alt_attack_action.generic.callback		= Controls_ActionEvent;
	s_attack_alt_attack_action.generic.ownerdraw	= Controls_DrawKeyBinding;
	s_attack_alt_attack_action.generic.id			= ID_ALT_ATTACK;

	s_attack_use_action.generic.type			= MTYPE_ACTION;
	s_attack_use_action.generic.flags			= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_attack_use_action.generic.callback		= Controls_ActionEvent;
	s_attack_use_action.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_attack_use_action.generic.id				= ID_USEITEM;

	s_attack_use_inv_action.generic.type		= MTYPE_ACTION;
	s_attack_use_inv_action.generic.flags		= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_attack_use_inv_action.generic.callback	= Controls_ActionEvent;
	s_attack_use_inv_action.generic.ownerdraw	= Controls_DrawKeyBinding;
	s_attack_use_inv_action.generic.id			= ID_USEINVENTORY;

	s_attack_objectives.generic.type			= MTYPE_ACTION;
	s_attack_objectives.generic.flags			= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_attack_objectives.generic.callback		= Controls_ActionEvent;
	s_attack_objectives.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_attack_objectives.generic.id				= ID_OBJECTIVES;

	s_look_lookup_action.generic.type			= MTYPE_ACTION;
	s_look_lookup_action.generic.flags			= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_look_lookup_action.generic.callback		= Controls_ActionEvent;
	s_look_lookup_action.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_look_lookup_action.generic.id				= ID_LOOKUP;

	s_look_lookdown_action.generic.type	    = MTYPE_ACTION;
	s_look_lookdown_action.generic.flags     = QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_look_lookdown_action.generic.callback  = Controls_ActionEvent;
	s_look_lookdown_action.generic.ownerdraw = Controls_DrawKeyBinding;
	s_look_lookdown_action.generic.id        = ID_LOOKDOWN;

	s_look_mouselook_action.generic.type	     = MTYPE_ACTION;
	s_look_mouselook_action.generic.flags     = QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_look_mouselook_action.generic.callback  = Controls_ActionEvent;
	s_look_mouselook_action.generic.ownerdraw = Controls_DrawKeyBinding;
	s_look_mouselook_action.generic.id        = ID_MOUSELOOK;

	s_look_centerview_action.generic.type	  = MTYPE_ACTION;
	s_look_centerview_action.generic.flags     = QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_look_centerview_action.generic.callback  = Controls_ActionEvent;
	s_look_centerview_action.generic.ownerdraw = Controls_DrawKeyBinding;
	s_look_centerview_action.generic.id        = ID_CENTERVIEW;

	s_zoomview_action.generic.type	    = MTYPE_ACTION;
	s_zoomview_action.generic.flags     = QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_zoomview_action.generic.callback  = Controls_ActionEvent;
	s_zoomview_action.generic.ownerdraw = Controls_DrawKeyBinding;
	s_zoomview_action.generic.id        = ID_ZOOMVIEW;

	s_controls.showscores.generic.type				= MTYPE_ACTION;
	s_controls.showscores.generic.flags				= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_controls.showscores.generic.callback			= Controls_ActionEvent;
	s_controls.showscores.generic.ownerdraw			= Controls_DrawKeyBinding;
	s_controls.showscores.generic.id 				= ID_SHOWSCORES;

	s_attack_waiting_action.generic.type			= MTYPE_ACTION;
	s_attack_waiting_action.generic.flags			= QMF_HIDDEN;
	s_attack_waiting_action.generic.x				= 202;
	s_attack_waiting_action.generic.y				= 410;
	s_attack_waiting_action.textEnum				= MBT_WAITFORKEY;
	s_attack_waiting_action.textcolor				= CT_BLACK;
	s_attack_waiting_action.textcolor2				= CT_WHITE;
	s_attack_waiting_action.textcolor3				= CT_LTGREY;
	s_attack_waiting_action.color					= CT_VDKPURPLE1;//CT_DKPURPLE1;
	s_attack_waiting_action.color2					= CT_VDKPURPLE1;//CT_LTPURPLE1;
	s_attack_waiting_action.color3					= CT_DKGREY;
	s_attack_waiting_action.textX					= 5;
	s_attack_waiting_action.textY					= 1;
	s_attack_waiting_action.width					= 255;
	s_attack_waiting_action.height					= 18;

	g_section        = C_LOOK;
	SetupActionButtons_Init(g_section);		// Set up standard values

	Menu_AddItem( &s_controlslook_menu, ( void * )&s_controls_playermdl);

	Menu_AddItem( &s_controlslook_menu, ( void * )&s_attack_attack_action);
	Menu_AddItem( &s_controlslook_menu, ( void * )&s_attack_alt_attack_action);
	Menu_AddItem( &s_controlslook_menu, ( void * )&s_attack_use_action);
	Menu_AddItem( &s_controlslook_menu, ( void * )&s_attack_use_inv_action);
	Menu_AddItem( &s_controlslook_menu, ( void * )&s_attack_objectives);

	Menu_AddItem( &s_controlslook_menu, ( void * )&s_look_lookup_action);
	Menu_AddItem( &s_controlslook_menu, ( void * )&s_look_lookdown_action);
	Menu_AddItem( &s_controlslook_menu, ( void * )&s_look_mouselook_action);
	Menu_AddItem( &s_controlslook_menu, ( void * )&s_look_centerview_action);
	Menu_AddItem( &s_controlslook_menu, ( void * )&s_zoomview_action);
	Menu_AddItem( &s_controlslook_menu, ( void * )&s_attack_waiting_action);
	Menu_AddItem( &s_controlslook_menu, ( void * )&s_controls.showscores);

	// initialize the configurable cvars
	Controls_InitCvars();

	// initialize the current config
	Controls_GetConfig();

	// intialize the model
	Controls_InitModel();

	// initial default section
	s_controls.waitingforkey  = qfalse;
	g_changesmade    = qfalse;
	g_prompttosave   = qfalse;

	// update the ui
	Controls_UpdateNew();
}


/*
=================
UI_ControlsAttackLookMenu
=================
*/
static void UI_ControlsAttackLookMenu( void )
{
	ControlsAttackLook_MenuInit();

	UI_PushMenu( &s_controlslook_menu );
}


/*
=================
M_ControlsMouseJoyStickMenu_Graphics
=================
*/
void M_ControlsMouseJoyStickMenu_Graphics (void)
{
	UI_MenuFrame(&s_controlsmouse_menu);

	trap_R_SetColor( colorTable[CT_LTORANGE]);
	UI_DrawProportionalString(  MENU_TITLE_X, 440, menu_normal_text[MNT_MOUSEJOYSTICK_SETUP],UI_RIGHT | UI_SMALLFONT, colorTable[CT_LTORANGE]);
	UI_MenuBottomLineEnd_Graphics (menu_normal_text[MNT_MOUSEJOYSTICK_SETUP],CT_LTBROWN1, NULL);

	trap_R_SetColor( colorTable[CT_DKBLUE2]);
	UI_DrawHandlePic( 235,  166,  377,  20, uis.whiteShader);	// Long thick line above mouse stuff
	UI_DrawHandlePic( 185,  184,  50,  100, uis.whiteShader);	// Thick column by mouse stuff
	UI_DrawHandlePic( 185,  284,  427,  8, uis.whiteShader);	// Long thin line below mouse stuff
	UI_DrawHandlePic( 185,  166,  64,  32, s_joystick_mouse.mouse1);	// Rounded corner


	UI_DrawHandlePic( 185,  300,  427,  8, uis.whiteShader);	// Long thin line below joystick stuff
	UI_DrawHandlePic( 185,  305,  50,   100, uis.whiteShader);	// Thick column by joystick stuff
	UI_DrawHandlePic( 235,  406,  377,  20, uis.whiteShader);	// Long thick line above joystick stuff
	UI_DrawHandlePic( 185,  401,  64,  32, s_joystick_mouse.mouse2);	// Rounded corner

	UI_DrawProportionalString(  608,  169, menu_normal_text[MNT_MOUSE],UI_RIGHT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);
	UI_DrawProportionalString(  608,  409, menu_normal_text[MNT_JOYSTICK],UI_RIGHT|UI_SMALLFONT, colorTable[CT_LTGOLD1]);

	UI_DrawProportionalString(  74,  66, "4568",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "71-49",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  188, "67014",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "356-905",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

}

/*
=================
ControlsMouseJoyStick_MenuDraw
=================
*/
static void ControlsMouseJoyStick_MenuDraw (void)
{
	M_ControlsMouseJoyStickMenu_Graphics();

	UI_Setup_MenuButtons();

	Menu_Draw( &s_controlsmouse_menu );
}

/*
=================
ControlsMouseJoyStick_MenuKey
=================
*/
static sfxHandle_t ControlsMouseJoyStick_MenuKey( int key )
{
	return Menu_DefaultKey( &s_controlsmouse_menu, key );
}

/*
=================
ControlsMouseJoyStick_GetCvars
=================
*/
void ControlsMouseJoyStick_GetCvars(void)
{

}

void UI_ControlsMouseJoyStickMenu_Cache(void)
{
	trap_R_RegisterShaderNoMip(PIC_MONBAR2);
	s_joystick_mouse.mouse1 = trap_R_RegisterShaderNoMip(PIC_MOUSE1);
	s_joystick_mouse.mouse2 = trap_R_RegisterShaderNoMip(PIC_MOUSE2);
}

/*
=================
ControlsMouseJoyStick_MenuInit
=================
*/
static void ControlsMouseJoyStick_MenuInit( void )
{
	int x,y;

	UI_ControlsMouseJoyStickMenu_Cache();

	s_controlsmouse_menu.nitems					= 0;
	s_controlsmouse_menu.wrapAround				= qtrue;
	s_controlsmouse_menu.draw					= ControlsMouseJoyStick_MenuDraw;
	s_controlsmouse_menu.key					= ControlsMouseJoyStick_MenuKey;
	s_controlsmouse_menu.fullscreen				= qtrue;
	s_controlsmouse_menu.descX					= MENU_DESC_X;
	s_controlsmouse_menu.descY					= MENU_DESC_Y;
	s_controlsmouse_menu.listX					= 230;
	s_controlsmouse_menu.listY					= 188;
	s_controlsmouse_menu.titleX					= MENU_TITLE_X;
	s_controlsmouse_menu.titleY					= MENU_TITLE_Y;
	s_controlsmouse_menu.titleI					= MNT_CONTROLSMENU_TITLE;
	s_controlsmouse_menu.footNoteEnum			= MNT_MOUSEJOYSTICK_SETUP;

	SetupMenu_TopButtons(&s_controlsmouse_menu,MENU_CONTROLS,NULL);
	SetupMenu_SideButtons(&s_controlsmouse_menu,MENU_CONTROLS_MOUSE);

	s_controls_mouse.textcolor					= CT_LTGOLD1;
	s_controls_mouse.textcolor2					= CT_LTGOLD1;

	x = 250;
	y = 193;

	s_controls.freelook.generic.type			= MTYPE_SPINCONTROL;
	s_controls.freelook.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls.freelook.generic.x				= x;
	s_controls.freelook.generic.y				= y;
	s_controls.freelook.generic.id 				= ID_FREELOOK;
	s_controls.freelook.generic.callback		= Controls_MenuEvent;
	s_controls.freelook.textEnum				= MBT_MOUSEFREELOOK;
	s_controls.freelook.textcolor				= CT_BLACK;
	s_controls.freelook.textcolor2				= CT_WHITE;
	s_controls.freelook.color					= CT_DKPURPLE1;
	s_controls.freelook.color2					= CT_LTPURPLE1;
	s_controls.freelook.textX					= MENU_BUTTON_TEXT_X;
	s_controls.freelook.textY					= MENU_BUTTON_TEXT_Y;
	s_controls.freelook.listnames				= s_OffOnNone_Names;

	y += 22;
	s_controls.sensitivity.generic.type			= MTYPE_SLIDER;
	s_controls.sensitivity.generic.x			= x + 162;
	s_controls.sensitivity.generic.y			= y;
	s_controls.sensitivity.generic.flags		= QMF_SMALLFONT;
	s_controls.sensitivity.generic.id 			= ID_MOUSESPEED;
	s_controls.sensitivity.generic.callback		= Controls_MenuEvent;
	s_controls.sensitivity.minvalue				= 2;
	s_controls.sensitivity.maxvalue				= 30;
	s_controls.sensitivity.color				= CT_DKPURPLE1;
	s_controls.sensitivity.color2				= CT_LTPURPLE1;
	s_controls.sensitivity.generic.name			= PIC_MONBAR2;
	s_controls.sensitivity.width				= 256;
	s_controls.sensitivity.height				= 32;
	s_controls.sensitivity.focusWidth			= 145;
	s_controls.sensitivity.focusHeight			= 18;
	s_controls.sensitivity.picName				= GRAPHIC_SQUARE;
	s_controls.sensitivity.picX					= x;
	s_controls.sensitivity.picY					= y;
	s_controls.sensitivity.picWidth				= MENU_BUTTON_MED_WIDTH + 21;
	s_controls.sensitivity.picHeight			= MENU_BUTTON_MED_HEIGHT;
	s_controls.sensitivity.textX				= MENU_BUTTON_TEXT_X;
	s_controls.sensitivity.textY				= MENU_BUTTON_TEXT_Y;
	s_controls.sensitivity.textEnum				= MBT_MOUSESPEED;
	s_controls.sensitivity.textcolor			= CT_BLACK;
	s_controls.sensitivity.textcolor2			= CT_WHITE;
	s_controls.sensitivity.thumbName			= "menu/common/slider.tga";
	s_controls.sensitivity.thumbHeight			= 32;
	s_controls.sensitivity.thumbWidth			= 16;
	s_controls.sensitivity.thumbGraphicWidth	= 9;
	s_controls.sensitivity.thumbColor			= CT_DKBLUE1;
	s_controls.sensitivity.thumbColor2			= CT_LTBLUE1;

	y += 22;
	s_controls.invertmouse.generic.type         = MTYPE_SPINCONTROL;
	s_controls.invertmouse.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls.invertmouse.generic.id 			= ID_INVERTMOUSE;
	s_controls.invertmouse.generic.x			= x;
	s_controls.invertmouse.generic.y			= y;
	s_controls.invertmouse.generic.callback		= Controls_MenuEvent;
	s_controls.invertmouse.textEnum				= MBT_MOUSEINVERT;
	s_controls.invertmouse.textcolor			= CT_BLACK;
	s_controls.invertmouse.textcolor2			= CT_WHITE;
	s_controls.invertmouse.color				= CT_DKPURPLE1;
	s_controls.invertmouse.color2				= CT_LTPURPLE1;
	s_controls.invertmouse.textX				= MENU_BUTTON_TEXT_X;
	s_controls.invertmouse.textY				= MENU_BUTTON_TEXT_Y;
	s_controls.invertmouse.listnames			= s_OffOnNone_Names;


	y += 22;
	s_controls.smoothmouse.generic.type			    = MTYPE_SPINCONTROL;
	s_controls.smoothmouse.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls.smoothmouse.generic.x					= x;
	s_controls.smoothmouse.generic.y					= y;
	s_controls.smoothmouse.generic.callback			= SmoothMouseCallback;
	s_controls.smoothmouse.textEnum					= MBT_MOUSESMOOTH;
	s_controls.smoothmouse.textcolor					= CT_BLACK;
	s_controls.smoothmouse.textcolor2				= CT_WHITE;
	s_controls.smoothmouse.color						= CT_DKPURPLE1;
	s_controls.smoothmouse.color2					= CT_LTPURPLE1;
	s_controls.smoothmouse.textX						= MENU_BUTTON_TEXT_X;
	s_controls.smoothmouse.textY						= MENU_BUTTON_TEXT_Y;
	s_controls.smoothmouse.listnames					= s_OffOnNone_Names;



	y = 315;
	s_controls.joyenable.generic.type			= MTYPE_SPINCONTROL;
	s_controls.joyenable.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls.joyenable.generic.x				= x;
	s_controls.joyenable.generic.y				= y;
	s_controls.joyenable.generic.id				= ID_JOYENABLE;
	s_controls.joyenable.generic.callback		= Controls_MenuEvent;
	s_controls.joyenable.textEnum				= MBT_JOYSTICKENABLE;
	s_controls.joyenable.textcolor				= CT_BLACK;
	s_controls.joyenable.textcolor2				= CT_WHITE;
	s_controls.joyenable.color					= CT_DKPURPLE1;
	s_controls.joyenable.color2					= CT_LTPURPLE1;
	s_controls.joyenable.textX					= MENU_BUTTON_TEXT_X;
	s_controls.joyenable.textY					= MENU_BUTTON_TEXT_Y;
	s_controls.joyenable.listnames				= s_OffOnNone_Names;

	y += 22;
	s_controls.joythreshold.generic.type		= MTYPE_SLIDER;
	s_controls.joythreshold.generic.x			= x + 162;
	s_controls.joythreshold.generic.y			= y;
	s_controls.joythreshold.generic.flags		= QMF_SMALLFONT;
	s_controls.joythreshold.generic.id		= ID_MOUSESPEED;
	s_controls.joythreshold.generic.callback	= Controls_MenuEvent;
	s_controls.joythreshold.minvalue			= .5;
	s_controls.joythreshold.maxvalue			= .75;
	s_controls.joythreshold.color				= CT_DKPURPLE1;
	s_controls.joythreshold.color2			= CT_LTPURPLE1;
	s_controls.joythreshold.generic.name		= PIC_MONBAR2;
	s_controls.joythreshold.width				= 256;
	s_controls.joythreshold.height			= 32;
	s_controls.joythreshold.focusWidth		= 145;
	s_controls.joythreshold.focusHeight		= 18;
	s_controls.joythreshold.picName			= GRAPHIC_SQUARE;
	s_controls.joythreshold.picX				= x;
	s_controls.joythreshold.picY				= y;
	s_controls.joythreshold.picWidth			= MENU_BUTTON_MED_WIDTH + 21;
	s_controls.joythreshold.picHeight			= MENU_BUTTON_MED_HEIGHT;
	s_controls.joythreshold.textX				= MENU_BUTTON_TEXT_X;
	s_controls.joythreshold.textY				= MENU_BUTTON_TEXT_Y;
	s_controls.joythreshold.textEnum			= MBT_JOYSTICKTHRESHOLD;
	s_controls.joythreshold.textcolor			= CT_BLACK;
	s_controls.joythreshold.textcolor2		= CT_WHITE;
	s_controls.joythreshold.thumbName			= "menu/common/slider.tga";
	s_controls.joythreshold.thumbHeight		= 32;
	s_controls.joythreshold.thumbWidth		= 16;
	s_controls.joythreshold.thumbGraphicWidth	= 9;
	s_controls.joythreshold.thumbColor		= CT_DKBLUE1;
	s_controls.joythreshold.thumbColor2		= CT_LTBLUE1;
/*
	y += 22;
	s_forcefeedback_box.generic.type		= MTYPE_SPINCONTROL;
	s_forcefeedback_box.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_forcefeedback_box.generic.x			= x;
	s_forcefeedback_box.generic.y			= y;
	s_forcefeedback_box.generic.callback	= Controls_MenuEvent;
	s_forcefeedback_box.textEnum			= MBT_FORCEFEEDBACK;
	s_forcefeedback_box.textcolor			= CT_BLACK;
	s_forcefeedback_box.textcolor2			= CT_WHITE;
	s_forcefeedback_box.color				= CT_DKPURPLE1;
	s_forcefeedback_box.color2				= CT_LTPURPLE1;
	s_forcefeedback_box.textX				= MENU_BUTTON_TEXT_X;
	s_forcefeedback_box.textY				= MENU_BUTTON_TEXT_Y;
	s_forcefeedback_box.listnames			= s_OffOnNone_Names;
*/



	y += 22;
	s_joyxbutton_box.generic.type		= MTYPE_SPINCONTROL;
	s_joyxbutton_box.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_joyxbutton_box.generic.x			= x;
	s_joyxbutton_box.generic.y			= y;
	s_joyxbutton_box.generic.callback	= JoyXButtonCallback;
	s_joyxbutton_box.textEnum			= MBT_X_AXIS;
	s_joyxbutton_box.textcolor			= CT_BLACK;
	s_joyxbutton_box.textcolor2			= CT_WHITE;
	s_joyxbutton_box.color				= CT_DKPURPLE1;
	s_joyxbutton_box.color2				= CT_LTPURPLE1;
	s_joyxbutton_box.textX				= MENU_BUTTON_TEXT_X;
	s_joyxbutton_box.textY				= MENU_BUTTON_TEXT_Y;
	s_joyxbutton_box.listnames			= s_OffOnNone_Names;

	y += 22;
	s_joyybutton_box.generic.type		= MTYPE_SPINCONTROL;
	s_joyybutton_box.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_joyybutton_box.generic.x			= x;
	s_joyybutton_box.generic.y			= y;
	s_joyybutton_box.generic.callback	= JoyYButtonCallback;
	s_joyybutton_box.textEnum			= MBT_Y_AXIS;
	s_joyybutton_box.textcolor			= CT_BLACK;
	s_joyybutton_box.textcolor2			= CT_WHITE;
	s_joyybutton_box.color				= CT_DKPURPLE1;
	s_joyybutton_box.color2				= CT_LTPURPLE1;
	s_joyybutton_box.textX				= MENU_BUTTON_TEXT_X;
	s_joyybutton_box.textY				= MENU_BUTTON_TEXT_Y;
	s_joyybutton_box.listnames			= s_OffOnNone_Names;


	Menu_AddItem( &s_controlsmouse_menu, ( void * )&s_controls.freelook);
	Menu_AddItem( &s_controlsmouse_menu, ( void * )&s_controls.sensitivity);
	Menu_AddItem( &s_controlsmouse_menu, ( void * )&s_controls.invertmouse);
	Menu_AddItem( &s_controlsmouse_menu, ( void * )&s_controls.smoothmouse);
	Menu_AddItem( &s_controlsmouse_menu, ( void * )&s_controls.joyenable);
	Menu_AddItem( &s_controlsmouse_menu, ( void * )&s_controls.joythreshold);
//	Menu_AddItem( &s_controlsmouse_menu, ( void * )&s_forcefeedback_box);	// Commented out until needed
	Menu_AddItem( &s_controlsmouse_menu, ( void * )&s_joyxbutton_box);	// Commented out until needed
	Menu_AddItem( &s_controlsmouse_menu, ( void * )&s_joyybutton_box);	// Commented out until needed

	// initialize the configurable cvars
	Controls_InitCvars();

	// initialize the current config
	Controls_GetConfig();

	// initial default section
	s_controls.waitingforkey  = qfalse;
	g_section        = C_MOUSE;
	g_changesmade    = qfalse;
	g_prompttosave   = qfalse;

	// update the ui
	Controls_UpdateNew();
}

/*
=================
UI_ControlsMouseJoyStickMenu
=================
*/
static void UI_ControlsMouseJoyStickMenu( void )
{
	ControlsMouseJoyStick_MenuInit();

	UI_PushMenu( &s_controlsmouse_menu );
}


/*
=================
M_ControlsOtherMenu_Graphics
=================
*/
void M_ControlsDefaultMenu_Graphics (void)
{
	// Draw the basic screen layout
	UI_MenuFrame(&s_controlsdefault_menu);

	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30,203,  47, 186, uis.whiteShader);		// Long left column square on bottom 3rd

	trap_R_SetColor( colorTable[CT_RED]);
	UI_DrawHandlePic(132,175,  425, 18, uis.whiteShader);		// Top
	UI_DrawHandlePic(132,193,  47, 175, uis.whiteShader);		// Left side
	UI_DrawHandlePic(510,193,  47, 175, uis.whiteShader);		// Right side
	UI_DrawHandlePic(132,365,  80, 18, uis.whiteShader);		// Bottom Left
	UI_DrawHandlePic(477,365,  80, 18, uis.whiteShader);		// Bottom Right


	UI_DrawProportionalString(345,210,menu_normal_text[MNT_DEFAULT_WARNING1],UI_BIGFONT | UI_BLINK | UI_CENTER,colorTable[CT_RED]);
	UI_DrawProportionalString(345,263,menu_normal_text[MNT_DEFAULT_WARNING2],UI_SMALLFONT | UI_CENTER,colorTable[CT_RED]);
	UI_DrawProportionalString(345,283,menu_normal_text[MNT_DEFAULT_WARNING3],UI_SMALLFONT | UI_CENTER,colorTable[CT_RED]);
	UI_DrawProportionalString(345,321,menu_normal_text[MNT_CONTINUE_WARNING],UI_SMALLFONT | UI_CENTER,colorTable[CT_RED]);

}

/*
=================
ControlsDefault_MenuDraw
=================
*/
static void ControlsDefault_MenuDraw (void)
{
	M_ControlsDefaultMenu_Graphics();

	UI_Setup_MenuButtons();

	Menu_Draw( &s_controlsdefault_menu );
}


/*
=================
M_Default_Event
=================
*/
void M_Default_Event (void* ptr, int event)
{
	menuframework_s*	m;

	if (event != QM_ACTIVATED)
	{
		return;
	}

	m = ((menucommon_s*)ptr)->parent;

	switch (((menucommon_s*)ptr)->id)
	{

		// Get default config
		case ID_DEFAULT_YES:
			Setup_ResetDefaults_Action(qtrue);
			break;

		// No, just back up
		case ID_DEFAULT_NO:
			UI_PopMenu();
			break;

	}
}

/*
=================
ControlsDefault_MenuKey
=================
*/
static sfxHandle_t ControlsDefault_MenuKey( int key )
{
	return Menu_DefaultKey( &s_controlsdefault_menu, key );
}

/*
=================
ControlsOther_MenuInit
=================
*/
static void ControlsDefault_MenuInit( void )
{

	s_controlsdefault_menu.nitems					= 0;
	s_controlsdefault_menu.wrapAround				= qtrue;
	s_controlsdefault_menu.draw						= ControlsDefault_MenuDraw;
	s_controlsdefault_menu.key						= ControlsDefault_MenuKey;
	s_controlsdefault_menu.fullscreen				= qtrue;
	s_controlsdefault_menu.descX					= MENU_DESC_X;
	s_controlsdefault_menu.descY					= MENU_DESC_Y;
	s_controlsdefault_menu.listX					= 230;
	s_controlsdefault_menu.listY					= 188;
	s_controlsdefault_menu.titleX					= MENU_TITLE_X;
	s_controlsdefault_menu.titleY					= MENU_TITLE_Y;
	s_controlsdefault_menu.titleI					= MNT_CONTROLSMENU_TITLE;
	s_controlsdefault_menu.footNoteEnum				= MNT_DEFAULT_SETUP;

	SetupMenu_TopButtons(&s_controlsdefault_menu,MENU_DEFAULT,NULL);

	s_controls_other.textcolor						= CT_LTGOLD1;
	s_controls_other.textcolor2						= CT_LTGOLD1;

	s_controls_default_yes.generic.type				= MTYPE_BITMAP;      
	s_controls_default_yes.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_default_yes.generic.x				= 215;
	s_controls_default_yes.generic.y				= 365;
	s_controls_default_yes.generic.name				= GRAPHIC_SQUARE;
	s_controls_default_yes.generic.id				= ID_DEFAULT_YES;
	s_controls_default_yes.generic.callback			= M_Default_Event; 
	s_controls_default_yes.width					= 103;
	s_controls_default_yes.height					= MENU_BUTTON_MED_HEIGHT;
	s_controls_default_yes.color					= CT_DKPURPLE1;
	s_controls_default_yes.color2					= CT_LTPURPLE1;
	s_controls_default_yes.textX					= MENU_BUTTON_TEXT_X;
	s_controls_default_yes.textY					= MENU_BUTTON_TEXT_Y;
	s_controls_default_yes.textEnum					= MBT_DEFAULT_YES;
	s_controls_default_yes.textcolor				= CT_BLACK;
	s_controls_default_yes.textcolor2				= CT_WHITE;
	s_controls_default_yes.textStyle				= UI_SMALLFONT;

	s_controls_default_no.generic.type				= MTYPE_BITMAP;      
	s_controls_default_no.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls_default_no.generic.x					= 371;
	s_controls_default_no.generic.y					= 365;
	s_controls_default_no.generic.name				= GRAPHIC_SQUARE;
	s_controls_default_no.generic.id				= ID_DEFAULT_NO;
	s_controls_default_no.generic.callback			= M_Default_Event; 
	s_controls_default_no.width						= 103;
	s_controls_default_no.height					= MENU_BUTTON_MED_HEIGHT;
	s_controls_default_no.color						= CT_DKPURPLE1;
	s_controls_default_no.color2					= CT_LTPURPLE1;
	s_controls_default_no.textX						= MENU_BUTTON_TEXT_X;
	s_controls_default_no.textY						= MENU_BUTTON_TEXT_Y;
	s_controls_default_no.textEnum					= MBT_DEFAULT_NO;
	s_controls_default_no.textcolor					= CT_BLACK;
	s_controls_default_no.textcolor2				= CT_WHITE;
	s_controls_default_no.textStyle					= UI_SMALLFONT;

	Menu_AddItem( &s_controlsdefault_menu, ( void * )&s_controls_default_yes);
	Menu_AddItem( &s_controlsdefault_menu, ( void * )&s_controls_default_no);

}

/*
=================
UI_ControlsDefaultMenu
=================
*/
static void UI_ControlsDefaultMenu( void )
{
	ControlsDefault_MenuInit();

	UI_PushMenu( &s_controlsdefault_menu );
}
/*
=================
M_ControlsOtherMenu_Graphics
=================
*/
void M_ControlsOtherMenu_Graphics (void)
{
	// Draw the basic screen layout
	UI_MenuFrame(&s_controlsother_menu);

	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic( 210,  204,  402,  8, uis.whiteShader);	// Long thin line below turn speed
	UI_DrawHandlePic( 545,  212,  67,  140, uis.whiteShader);	// Thick right column
	UI_DrawHandlePic( 210,  344,  402,  8, uis.whiteShader);	// Long thin line below autoswitch
	UI_DrawHandlePic( 210,  352,  10,  83, uis.whiteShader);	// Thin left column

	UI_DrawProportionalString(  608,  340, "1701 - B",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	UI_DrawProportionalString(  280,  419, "500987",UI_RIGHT|UI_TINYFONT, colorTable[CT_DKGOLD1]);
	UI_DrawProportionalString(  358,  419, "456732",UI_RIGHT|UI_TINYFONT, colorTable[CT_DKGOLD1]);
	UI_DrawProportionalString(  436,  419, "67024",UI_RIGHT|UI_TINYFONT, colorTable[CT_DKGOLD1]);
	UI_DrawProportionalString(  514,  419, "166110",UI_RIGHT|UI_TINYFONT, colorTable[CT_DKGOLD1]);
	UI_DrawProportionalString(  592,  419, "575",UI_RIGHT|UI_TINYFONT, colorTable[CT_DKGOLD1]);

	trap_R_SetColor( colorTable[CT_DKPURPLE3]);
	UI_DrawHandlePic( 288,  364,  32,  64, s_controlsother.mon_bar);	// 
	UI_DrawHandlePic( 366,  364,  32,  64, s_controlsother.mon_bar);	// 
	UI_DrawHandlePic( 444,  364,  32,  64, s_controlsother.mon_bar);	// 
	UI_DrawHandlePic( 522,  364,  32,  64, s_controlsother.mon_bar);	// 
	UI_DrawHandlePic( 600,  364,  32,  64, s_controlsother.mon_bar);	// 


	UI_DrawProportionalString(  74,  66, "5-0987",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "16116",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  188, "28430",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "6900",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

}

/*
=================
ControlsOther_MenuDraw
=================
*/
static void ControlsOther_MenuDraw (void)
{
	M_ControlsOtherMenu_Graphics();

	UI_Setup_MenuButtons();

	Menu_Draw( &s_controlsother_menu );
}

/*
=================
ControlsOther_MenuKey
=================
*/
static sfxHandle_t ControlsOther_MenuKey( int key )
{
	return Menu_DefaultKey( &s_controlsother_menu, key );
}

/*
=================
ControlsOther_Cache
=================
*/
void ControlsOther_Cache(void)
{
	s_controlsother.mon_bar = trap_R_RegisterShaderNoMip("menu/common/mon_bar.tga");
}


/*
=================
ControlsOther_MenuInit
=================
*/
static void ControlsOther_MenuInit( void )
{
	int x,y;

	ControlsOther_Cache();

	s_controlsother_menu.nitems					= 0;
	s_controlsother_menu.wrapAround				= qtrue;
	s_controlsother_menu.draw					= ControlsOther_MenuDraw;
	s_controlsother_menu.key					= ControlsOther_MenuKey;
	s_controlsother_menu.fullscreen				= qtrue;
	s_controlsother_menu.descX					= MENU_DESC_X;
	s_controlsother_menu.descY					= MENU_DESC_Y;
	s_controlsother_menu.listX					= 230;
	s_controlsother_menu.listY					= 188;
	s_controlsother_menu.titleX					= MENU_TITLE_X;
	s_controlsother_menu.titleY					= MENU_TITLE_Y;
	s_controlsother_menu.titleI					= MNT_CONTROLSMENU_TITLE;
	s_controlsother_menu.footNoteEnum			= MNT_OTHEROPTIONS_SETUP;

	SetupMenu_TopButtons(&s_controlsother_menu,MENU_CONTROLS,NULL);
	SetupMenu_SideButtons(&s_controlsother_menu,MENU_CONTROLS_OTHER);

	s_controls_other.textcolor					= CT_LTGOLD1;
	s_controls_other.textcolor2					= CT_LTGOLD1;

	x = 210;
	y = 172;

	s_keyturnspeed_slider.generic.type		= MTYPE_SLIDER;
	s_keyturnspeed_slider.generic.x			= x + 162;
	s_keyturnspeed_slider.generic.y			= y;
	s_keyturnspeed_slider.generic.flags		= QMF_SMALLFONT;
	s_keyturnspeed_slider.generic.id		= ID_KEYTURNPEED;
	s_keyturnspeed_slider.generic.callback	= Controls_MenuEvent;
	s_keyturnspeed_slider.minvalue			= 1;
	s_keyturnspeed_slider.maxvalue			= 5;
	s_keyturnspeed_slider.color				= CT_DKPURPLE1;
	s_keyturnspeed_slider.color2			= CT_LTPURPLE1;
	s_keyturnspeed_slider.generic.name		= PIC_MONBAR2;
	s_keyturnspeed_slider.width				= 256;
	s_keyturnspeed_slider.height			= 32;
	s_keyturnspeed_slider.focusWidth		= 145;
	s_keyturnspeed_slider.focusHeight		= 18;
	s_keyturnspeed_slider.picName			= GRAPHIC_SQUARE;
	s_keyturnspeed_slider.picX				= x;
	s_keyturnspeed_slider.picY				= y;
	s_keyturnspeed_slider.picWidth			= MENU_BUTTON_MED_WIDTH + 21;
	s_keyturnspeed_slider.picHeight			= MENU_BUTTON_MED_HEIGHT;
	s_keyturnspeed_slider.textX				= 5;
	s_keyturnspeed_slider.textY				= 1;
	s_keyturnspeed_slider.textEnum			= MBT_KEYTURNSPEED;
	s_keyturnspeed_slider.textcolor			= CT_BLACK;
	s_keyturnspeed_slider.textcolor2		= CT_WHITE;
	s_keyturnspeed_slider.thumbName			= "menu/common/slider.tga";
	s_keyturnspeed_slider.thumbHeight		= 32;
	s_keyturnspeed_slider.thumbWidth		= 16;
	s_keyturnspeed_slider.thumbGraphicWidth	= 9;
	s_keyturnspeed_slider.thumbColor		= CT_DKBLUE1;
	s_keyturnspeed_slider.thumbColor2		= CT_LTBLUE1;


	y =225;
	s_controls.alwaysrun.generic.type			= MTYPE_SPINCONTROL;
	s_controls.alwaysrun.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls.alwaysrun.generic.x				= x;
	s_controls.alwaysrun.generic.y				= y;
	s_controls.alwaysrun.generic.callback		= Controls_MenuEvent;
	s_controls.alwaysrun.generic.id			    = ID_ALWAYSRUN;
	s_controls.alwaysrun.textEnum				= MBT_ALWAYSRUN;
	s_controls.alwaysrun.textcolor				= CT_BLACK;
	s_controls.alwaysrun.textcolor2				= CT_WHITE;
	s_controls.alwaysrun.color					= CT_DKPURPLE1;
	s_controls.alwaysrun.color2					= CT_LTPURPLE1;
	s_controls.alwaysrun.textX					= 5;
	s_controls.alwaysrun.textY					= 2;
	s_controls.alwaysrun.listnames				= s_OffOnNone_Names;

/*	y +=22;
	s_lookspring_box.generic.type			= MTYPE_SPINCONTROL;
	s_lookspring_box.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_lookspring_box.generic.x				= x;
	s_lookspring_box.generic.y				= y;
	s_lookspring_box.generic.callback		= Controls_MenuEvent;
	s_lookspring_box.generic.id			    = ID_LOOKSPRING;
	s_lookspring_box.textEnum				= MBT_LOOKSPRING;
	s_lookspring_box.textcolor				= CT_BLACK;
	s_lookspring_box.textcolor2				= CT_WHITE;
	s_lookspring_box.color					= CT_DKPURPLE1;
	s_lookspring_box.color2					= CT_LTPURPLE1;
	s_lookspring_box.textX					= 5;
	s_lookspring_box.textY					= 2;
	s_lookspring_box.listnames				= s_OffOnNone_Names;
*/
	y +=22;
	y +=22;

	s_controls.autoswitch.generic.type			= MTYPE_SPINCONTROL;
	s_controls.autoswitch.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_controls.autoswitch.generic.x				= x;
	s_controls.autoswitch.generic.y				= y;
	s_controls.autoswitch.generic.callback		= Controls_MenuEvent;
	s_controls.autoswitch.generic.id		    = ID_AUTOSWITCHWEAPONS;
	s_controls.autoswitch.textEnum				= MBT_AUTOSWITCHWEAPONS;
	s_controls.autoswitch.textcolor				= CT_BLACK;
	s_controls.autoswitch.textcolor2			= CT_WHITE;
	s_controls.autoswitch.color					= CT_DKPURPLE1;
	s_controls.autoswitch.color2				= CT_LTPURPLE1;
	s_controls.autoswitch.textX					= 5;
	s_controls.autoswitch.textY					= 2;
	s_controls.autoswitch.listnames				= s_Autoswitch_Names;


	Menu_AddItem( &s_controlsother_menu, ( void * )&s_controls.alwaysrun);
//	Menu_AddItem( &s_controlsother_menu, ( void * )&s_lookspring_box);
	Menu_AddItem( &s_controlsother_menu, ( void * )&s_keyturnspeed_slider);
	Menu_AddItem( &s_controlsother_menu, ( void * )&s_controls.autoswitch);

	// initialize the configurable cvars
	Controls_InitCvars();

	// initialize the current config
	Controls_GetConfig();

	// initial default section
	g_section        = C_OTHER;
	g_changesmade    = qfalse;
	g_prompttosave   = qfalse;

	// update the ui
	Controls_UpdateNew();
}

/*
=================
UI_ControlsOtherMenu
=================
*/
static void UI_ControlsOtherMenu( void )
{
	ControlsOther_MenuInit();
	UI_PushMenu( &s_controlsother_menu );
}


/*
=================
M_ControlsCommandMenu_Graphics
=================
*/
void M_ControlsCommandMenu_Graphics (void)
{
	UI_MenuFrame(&s_controlscommand_menu);

	UI_DrawProportionalString(  74,  66, "6-7",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "8252",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  188, "21231",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "96709",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	M_WeaponsMenu_Blinkies();

	UI_PrintMenuGraphics(attackmenu_graphics,AMG_MAX);


	UI_DrawProportionalString(  607,  174, "423",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  607,  406, "2-2334",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	if (s_attack_waiting_action.generic.flags & QMF_HIDDEN)
	{
		UI_DrawProportionalString(  207,  411, menu_normal_text[MNT_BACKSPACE],UI_SMALLFONT, colorTable[CT_WHITE]);
	}
}

/*
=================
ControlsCommand_MenuDraw
=================
*/
static void ControlsCommand_MenuDraw (void)
{
	M_ControlsCommandMenu_Graphics();

	UI_Setup_MenuButtons();

	Menu_Draw( &s_controlscommand_menu );
}

/*
=================
ControlsCommand_MenuInit
=================
*/
static void ControlsCommand_MenuInit( void )
{
	s_controlscommand_menu.nitems				= 0;
	s_controlscommand_menu.wrapAround			= qtrue;
	s_controlscommand_menu.draw				= ControlsCommand_MenuDraw;
	s_controlscommand_menu.key					= Controls_MenuKey;
	s_controlscommand_menu.fullscreen			= qtrue;
	s_controlscommand_menu.descX				= MENU_DESC_X;
	s_controlscommand_menu.descY				= MENU_DESC_Y;
	s_controlscommand_menu.listX				= 170;
	s_controlscommand_menu.listY				= 188;
	s_controlscommand_menu.titleX				= MENU_TITLE_X;
	s_controlscommand_menu.titleY				= MENU_TITLE_Y;
	s_controlscommand_menu.titleI				= MNT_CONTROLSMENU_TITLE;
	s_controlscommand_menu.footNoteEnum			= MNT_COMMANDKEYS_SETUP;

	Playermodel_MenuInit();

	SetupMenu_TopButtons(&s_controlscommand_menu,MENU_CONTROLS,NULL);
	SetupMenu_SideButtons(&s_controlscommand_menu,MENU_CONTROLS_COMMAND);

	s_controls_command.textcolor					= CT_LTGOLD1;
	s_controls_command.textcolor2					= CT_LTGOLD1;

	s_controls.chat.generic.type			= MTYPE_ACTION;
	s_controls.chat.generic.flags			= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_controls.chat.generic.callback		= Controls_ActionEvent;
	s_controls.chat.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_controls.chat.generic.id 				= ID_CHAT;

	s_controls.chat2.generic.type			= MTYPE_ACTION;
	s_controls.chat2.generic.flags			= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_controls.chat2.generic.callback		= Controls_ActionEvent;
	s_controls.chat2.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_controls.chat2.generic.id 			= ID_CHAT2;

	s_controls.chat3.generic.type			= MTYPE_ACTION;
	s_controls.chat3.generic.flags			= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_controls.chat3.generic.callback		= Controls_ActionEvent;
	s_controls.chat3.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_controls.chat3.generic.id 			= ID_CHAT3;

/*	s_controls.chat4.generic.type			= MTYPE_ACTION;
	s_controls.chat4.generic.flags			= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_controls.chat4.generic.callback		= Controls_ActionEvent;
	s_controls.chat4.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_controls.chat4.generic.id 			= ID_CHAT4;*/

/*	s_controls.chat5.generic.type			= MTYPE_ACTION;
	s_controls.chat5.generic.flags			= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_controls.chat5.generic.callback		= Controls_ActionEvent;
	s_controls.chat5.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_controls.chat5.generic.id 			= ID_CHAT5;*/

	s_controls.gesture.generic.type			= MTYPE_ACTION;
	s_controls.gesture.generic.flags		= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_controls.gesture.generic.callback		= Controls_ActionEvent;
	s_controls.gesture.generic.ownerdraw	= Controls_DrawKeyBinding;
	s_controls.gesture.generic.id 			= ID_GESTURE;

	s_equip_action.generic.type				= MTYPE_ACTION;
	s_equip_action.generic.flags			= QMF_LEFT_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;
	s_equip_action.generic.callback			= Controls_ActionEvent;
	s_equip_action.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_equip_action.generic.id				= ID_EQUIP;

    // RPG-X | Marcin | 04/12/2008
	s_drop_action.generic.type				= MTYPE_ACTION;
	s_drop_action.generic.flags				= QMF_LEFT_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;
	s_drop_action.generic.callback			= Controls_ActionEvent;
	s_drop_action.generic.ownerdraw			= Controls_DrawKeyBinding;
	s_drop_action.generic.id				= ID_DROP;

	s_thirdperson_action.generic.type		= MTYPE_ACTION;
	s_thirdperson_action.generic.flags		= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_thirdperson_action.generic.callback	= Controls_ActionEvent;
	s_thirdperson_action.generic.ownerdraw	= Controls_DrawKeyBinding;
	s_thirdperson_action.generic.id 		= ID_TOGGLE3DP;

	s_thirdpersoncommit_action.generic.type			= MTYPE_ACTION;
	s_thirdpersoncommit_action.generic.flags		= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_thirdpersoncommit_action.generic.callback		= Controls_ActionEvent;
	s_thirdpersoncommit_action.generic.ownerdraw	= Controls_DrawKeyBinding;
	s_thirdpersoncommit_action.generic.id 			= ID_COMMITANGLES;

	s_thirdpersonrevert_action.generic.type			= MTYPE_ACTION;
	s_thirdpersonrevert_action.generic.flags		= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_thirdpersonrevert_action.generic.callback		= Controls_ActionEvent;
	s_thirdpersonrevert_action.generic.ownerdraw	= Controls_DrawKeyBinding;
	s_thirdpersonrevert_action.generic.id 			= ID_REVERTANGLES;	

	s_thirdpersonreset_action.generic.type			= MTYPE_ACTION;
	s_thirdpersonreset_action.generic.flags			= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_thirdpersonreset_action.generic.callback		= Controls_ActionEvent;
	s_thirdpersonreset_action.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_thirdpersonreset_action.generic.id 			= ID_RESETANGLES;

	s_attack_waiting_action.generic.type			= MTYPE_ACTION;
	s_attack_waiting_action.generic.flags			= QMF_HIDDEN;
	s_attack_waiting_action.generic.x				= 202;
	s_attack_waiting_action.generic.y				= 410;
	s_attack_waiting_action.textEnum				= MBT_WAITFORKEY;
	s_attack_waiting_action.textcolor				= CT_BLACK;
	s_attack_waiting_action.textcolor2				= CT_WHITE;
	s_attack_waiting_action.textcolor3				= CT_LTGREY;
	s_attack_waiting_action.color					= CT_VDKPURPLE1;//CT_DKPURPLE1;
	s_attack_waiting_action.color2					= CT_VDKPURPLE1;//CT_LTPURPLE1;
	s_attack_waiting_action.color3					= CT_DKGREY;
	s_attack_waiting_action.textX					= 5;
	s_attack_waiting_action.textY					= 1;
	s_attack_waiting_action.width					= 255;
	s_attack_waiting_action.height					= 18;

	g_section        = C_COMMAND;
	SetupActionButtons_Init(g_section);		// Set up standard values

	Menu_AddItem( &s_controlscommand_menu, ( void * )&s_controls_playermdl);
	Menu_AddItem( &s_controlscommand_menu, ( void * )&s_controls.chat);
	Menu_AddItem( &s_controlscommand_menu, ( void * )&s_controls.chat2);
	Menu_AddItem( &s_controlscommand_menu, ( void * )&s_controls.chat3);
	//Menu_AddItem( &s_controlscommand_menu, ( void * )&s_controls.chat4);
	//Menu_AddItem( &s_controlscommand_menu, ( void * )&s_controls.chat5);
	Menu_AddItem( &s_controlscommand_menu, ( void * )&s_controls.gesture);
	Menu_AddItem( &s_controlscommand_menu, ( void * )&s_attack_waiting_action);
	Menu_AddItem( &s_controlscommand_menu, ( void *)&s_equip_action );
    Menu_AddItem( &s_controlscommand_menu, ( void *)&s_drop_action ); // RPG-X | Marcin | 04/12/2008

	Menu_AddItem( &s_controlscommand_menu, ( void * )&s_thirdperson_action );
	Menu_AddItem( &s_controlscommand_menu, ( void * )&s_thirdpersoncommit_action);
	Menu_AddItem( &s_controlscommand_menu, ( void * )&s_thirdpersonrevert_action);
	Menu_AddItem( &s_controlscommand_menu, ( void * )&s_thirdpersonreset_action);

	// initialize the configurable cvars
	Controls_InitCvars();

	// initialize the current config
	Controls_GetConfig();

	// intialize the model
	Controls_InitModel();

	// initial default section
	s_controls.waitingforkey  = qfalse;

	// update the ui
	Controls_UpdateNew();
}


/*
=================
UI_ControlsCommandMenu
=================
*/
static void UI_ControlsCommandMenu( void )
{
	ControlsCommand_MenuInit();

	UI_PushMenu( &s_controlscommand_menu );
}


/*
=================
M_ControlsModelView_Graphics
=================
*/
void M_ControlsModelView_Graphics (void)
{
	UI_MenuFrame(&s_controlsmodelview_menu);

	UI_DrawProportionalString(  74,  66, "2-8",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "6154",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  188, "604",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "3699",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	M_WeaponsMenu_Blinkies();

	UI_PrintMenuGraphics(attackmenu_graphics,AMG_MAX);


	UI_DrawProportionalString(  607,  174, "867",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  607,  406, "2-2135",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	if (s_attack_waiting_action.generic.flags & QMF_HIDDEN)
	{
		UI_DrawProportionalString(  207,  411, menu_normal_text[MNT_BACKSPACE],UI_SMALLFONT, colorTable[CT_WHITE]);
	}
}

/*
=================
ControlsModelView_MenuDraw
=================
*/
static void ControlsModelView_MenuDraw (void)
{
	M_ControlsModelView_Graphics();

	UI_Setup_MenuButtons();

	Menu_Draw( &s_controlsmodelview_menu );
}

/*
=================
ControlsModelView_MenuInit
=================
*/
static void ControlsModelView_MenuInit( void )
{
	s_controlsmodelview_menu.nitems				= 0;
	s_controlsmodelview_menu.wrapAround			= qtrue;
	s_controlsmodelview_menu.draw				= ControlsModelView_MenuDraw;
	s_controlsmodelview_menu.key				= Controls_MenuKey;
	s_controlsmodelview_menu.fullscreen			= qtrue;
	s_controlsmodelview_menu.descX				= MENU_DESC_X;
	s_controlsmodelview_menu.descY				= MENU_DESC_Y;
	s_controlsmodelview_menu.listX				= 170;
	s_controlsmodelview_menu.listY				= 188;
	s_controlsmodelview_menu.titleX				= MENU_TITLE_X;
	s_controlsmodelview_menu.titleY				= MENU_TITLE_Y;
	s_controlsmodelview_menu.titleI				= MNT_CONTROLSMENU_TITLE;
	s_controlsmodelview_menu.footNoteEnum		= MNT_MODELVIEW_SETUP;

	Playermodel_MenuInit();

	SetupMenu_TopButtons(&s_controlsmodelview_menu,MENU_CONTROLS,NULL);
	SetupMenu_SideButtons(&s_controlsmodelview_menu,MENU_CONTROLS_MODELVIEW);

	s_controls_modelview.textcolor				= CT_LTGOLD1;
	s_controls_modelview.textcolor2				= CT_LTGOLD1;

	s_zoomforward_action.generic.type			= MTYPE_ACTION;
	s_zoomforward_action.generic.flags			= QMF_LEFT_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;
	s_zoomforward_action.generic.callback		= Controls_ActionEvent;
	s_zoomforward_action.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_zoomforward_action.generic.id				= ID_ZOOMFORWARD;
	
	s_zoombackward_action.generic.type			= MTYPE_ACTION;
	s_zoombackward_action.generic.flags			= QMF_LEFT_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;
	s_zoombackward_action.generic.callback		= Controls_ActionEvent;
	s_zoombackward_action.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_zoombackward_action.generic.id			= ID_ZOOMBACKWARD;

	s_panleft_action.generic.type				= MTYPE_ACTION;
	s_panleft_action.generic.flags				= QMF_LEFT_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;
	s_panleft_action.generic.callback			= Controls_ActionEvent;
	s_panleft_action.generic.ownerdraw			= Controls_DrawKeyBinding;
	s_panleft_action.generic.id					= ID_PANLEFT;

	s_panright_action.generic.type				= MTYPE_ACTION;
	s_panright_action.generic.flags				= QMF_LEFT_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;
	s_panright_action.generic.callback			= Controls_ActionEvent;
	s_panright_action.generic.ownerdraw			= Controls_DrawKeyBinding;
	s_panright_action.generic.id				= ID_PANRIGHT;
	
	s_panup_action.generic.type					= MTYPE_ACTION;
	s_panup_action.generic.flags				= QMF_LEFT_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;
	s_panup_action.generic.callback				= Controls_ActionEvent;
	s_panup_action.generic.ownerdraw			= Controls_DrawKeyBinding;
	s_panup_action.generic.id					= ID_PANUP;
	
	s_pandown_action.generic.type				= MTYPE_ACTION;
	s_pandown_action.generic.flags				= QMF_LEFT_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;
	s_pandown_action.generic.callback			= Controls_ActionEvent;
	s_pandown_action.generic.ownerdraw			= Controls_DrawKeyBinding;
	s_pandown_action.generic.id					= ID_PANDOWN;

	s_rotateleft_action.generic.type			= MTYPE_ACTION;
	s_rotateleft_action.generic.flags			= QMF_LEFT_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;
	s_rotateleft_action.generic.callback		= Controls_ActionEvent;
	s_rotateleft_action.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_rotateleft_action.generic.id				= ID_ROTATELEFT;

	s_rotateright_action.generic.type			= MTYPE_ACTION;
	s_rotateright_action.generic.flags			= QMF_LEFT_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;
	s_rotateright_action.generic.callback		= Controls_ActionEvent;
	s_rotateright_action.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_rotateright_action.generic.id				= ID_ROTATERIGHT;

	s_pitchup_action.generic.type				= MTYPE_ACTION;
	s_pitchup_action.generic.flags				= QMF_LEFT_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;
	s_pitchup_action.generic.callback			= Controls_ActionEvent;
	s_pitchup_action.generic.ownerdraw			= Controls_DrawKeyBinding;
	s_pitchup_action.generic.id					= ID_PITCHUP;

	s_pitchdown_action.generic.type				= MTYPE_ACTION;
	s_pitchdown_action.generic.flags			= QMF_LEFT_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;
	s_pitchdown_action.generic.callback			= Controls_ActionEvent;
	s_pitchdown_action.generic.ownerdraw		= Controls_DrawKeyBinding;
	s_pitchdown_action.generic.id				= ID_PITCHDOWN;

	s_attack_waiting_action.generic.type		= MTYPE_ACTION;
	s_attack_waiting_action.generic.flags		= QMF_HIDDEN;
	s_attack_waiting_action.generic.x			= 202;
	s_attack_waiting_action.generic.y			= 410;
	s_attack_waiting_action.textEnum			= MBT_WAITFORKEY;
	s_attack_waiting_action.textcolor			= CT_BLACK;
	s_attack_waiting_action.textcolor2			= CT_WHITE;
	s_attack_waiting_action.textcolor3			= CT_LTGREY;
	s_attack_waiting_action.color				= CT_VDKPURPLE1;//CT_DKPURPLE1;
	s_attack_waiting_action.color2				= CT_VDKPURPLE1;//CT_LTPURPLE1;
	s_attack_waiting_action.color3				= CT_DKGREY;
	s_attack_waiting_action.textX				= 5;
	s_attack_waiting_action.textY				= 1;
	s_attack_waiting_action.width				= 255;
	s_attack_waiting_action.height				= 18;

	g_section        = C_MODELVIEW;
	SetupActionButtons_Init(g_section);		// Set up standard values

	Menu_AddItem( &s_controlsmodelview_menu, ( void* )&s_zoomforward_action );
	Menu_AddItem( &s_controlsmodelview_menu, ( void* )&s_zoombackward_action );
	Menu_AddItem( &s_controlsmodelview_menu, ( void* )&s_panleft_action );
	Menu_AddItem( &s_controlsmodelview_menu, ( void* )&s_panright_action );
	Menu_AddItem( &s_controlsmodelview_menu, ( void* )&s_panup_action );
	Menu_AddItem( &s_controlsmodelview_menu, ( void* )&s_pandown_action );
	Menu_AddItem( &s_controlsmodelview_menu, ( void* )&s_rotateleft_action );
	Menu_AddItem( &s_controlsmodelview_menu, ( void* )&s_rotateright_action );
	Menu_AddItem( &s_controlsmodelview_menu, ( void* )&s_pitchup_action );
	Menu_AddItem( &s_controlsmodelview_menu, ( void* )&s_pitchdown_action );
	Menu_AddItem( &s_controlsmodelview_menu, ( void* )&s_attack_waiting_action );

	Menu_AddItem( &s_controlsmodelview_menu, ( void * )&s_controls_playermdl);

	// initialize the configurable cvars
	Controls_InitCvars();

	// initialize the current config
	Controls_GetConfig();

	// intialize the model
	Controls_InitModel();

	// initial default section
	s_controls.waitingforkey  = qfalse;

	// update the ui
	Controls_UpdateNew();
}


/*
=================
UI_ControlsModelViewMenu
=================
*/
static void UI_ControlsModelViewMenu( void )
{
	ControlsModelView_MenuInit();

	UI_PushMenu( &s_controlsmodelview_menu );
}

