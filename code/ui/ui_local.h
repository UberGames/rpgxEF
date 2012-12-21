// Copyright (C) 1999-2000 Id Software, Inc.
//
#ifndef __UI_LOCAL_H__
#define __UI_LOCAL_H__

#include "../game/q_shared.h"
#include "../cgame/tr_types.h"
#include "ui_public.h"
#include "keycodes.h"
#include "../game/bg_public.h"

//RPG-X : TiM
//Defines for animation code in UI module
// RPG-X
int UI_GetAnim ( int anim, int weapon, qboolean upper );

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
#define ANIM_WEAPON6	17
#define ANIM_WEAPON7	18
#define ANIM_WEAPON8	19
#define ANIM_WEAPON9	20
#define ANIM_WEAPON10	21
#define ANIM_ATTACK		22
#define ANIM_GESTURE	23
#define ANIM_DIE		24
#define ANIM_CHAT		25

//Ranks
#define MAX_RANKSETS	16
#define MAX_NAMELENGTH	24

//Classes
#define MAX_CLASSSETS	16

//TiM - Struct to hold all the data on rank sets
typedef struct {
	char	rankSetNames[MAX_RANKSETS][128]; //Character pointers piss me off sooooo bad. >:(

	char			rankSetName[MAX_NAMELENGTH];
	rankNames_t		rankNames[MAX_RANKS];
} rankSets_t;

//TiM - UI Struct to hold all the data on current
//class settings
typedef struct {
	char	classNameFull[25];
	char	classNameConsole[15];
} classData_t;

// Menu Normal Text enum
typedef enum
{
	MNT_NONE,

	// Main Screen Headings
	MNT_MAINMENU_TITLE,
	MNT_CDKEYMENU_TITLE,
	MNT_DEMOS_TITLE,
	MNT_CONTROLSMENU_TITLE,
	MNT_SINGLEPLAYER_TITLE,
	MNT_MULTIPLAYER_TITLE,
	MNT_MODS_TITLE,
	MNT_CREDITS_TITLE, //TiM:
	MNT_QUITMENU_TITLE,
	MNT_INGAMEMAIN_TITLE,
	MNT_TEAMMENU_TITLE,
	MNT_EMOTES_MENU,
	MNT_ADMIN_MENU,
	MNT_CLIENT_MENU,
	MNT_AUDIO_MENU,

	// Ingame Menu
	MNT_INGAME_MENU,

	// Team Menu
	MNT_TEAM_MENU,

	// Demos Screen
	MNT_DEMOS,
	MNT_CURRENTFILE,
	MNT_CURRENTDEMOSAVAILABLE,

	// Main Menu
	MNT_ALPHA,	
	MNT_STELLAR_CARTOGRAPHY,
	MNT_FEDERATION_TXT,
	MNT_CARDASSIA_TXT,
	MNT_FERENGI_TXT,
	MNT_ROMULAN_TXT,
	MNT_KLINGON_TXT,
	MNT_DOMINION_TXT,
	MNT_VOYAGER_TXT,
	MNT_BORG_TXT,
	MNT_WORMHOLE_TXT,
	MNT_GALACTICCORE_TXT,
	MNT_ALPHAQUAD_TXT,
	MNT_BETAQUAD_TXT,
	MNT_DELTAQUAD_TXT,
	MNT_GAMMAQUAD_TXT,

	// Video Setup Menu
	MNT_VIDEOSETUP,
	MNT_VIDEO_HIGH_QUALITY,
	MNT_VIDEO_NORMAL,
	MNT_VIDEO_FAST,
	MNT_VIDEO_FASTEST,
	MNT_VIDEO_CUSTOM,
	MNT_VIDEO_DRIVER_DEFAULT,
	MNT_VIDEO_DRIVER_VOODOO,
	MNT_4X3,
	MNT_16X9,
	MNT_16X10,
	MNT_OFF,
	MNT_ON,
	MNT_320X200,
	MNT_400X300,
	MNT_512X384,
	MNT_640X480,
	MNT_800X600,
	MNT_960X720,
	MNT_1024X768,
	MNT_1152X864,
	MNT_1280X960,
	MNT_1600X1200,
	MNT_2048X1536,
	MNT_856x480WIDE,
	MNT_DEFAULT,
	MNT_16BIT,
	MNT_32BIT,
	MNT_LIGHTMAP,
	MNT_VERTEX,
	MNT_LOW,
	MNT_MEDIUM,
	MNT_HIGH,
	MNT_VERY_HIGH,
	MNT_BILINEAR,
	MNT_TRILINEAR,

	// Player setup
	MNT_CHANGEPLAYER_TITLE,
	MNT_PLAYERSETUP,
	MNT_HANDICAP_NONE,
	MNT_HANDICAP_95,
	MNT_HANDICAP_90,
	MNT_HANDICAP_85,
	MNT_HANDICAP_80,
	MNT_HANDICAP_75,
	MNT_HANDICAP_70,
	MNT_HANDICAP_65,
	MNT_HANDICAP_60,
	MNT_HANDICAP_55,
	MNT_HANDICAP_50,
	MNT_HANDICAP_45,
	MNT_HANDICAP_40,
	MNT_HANDICAP_35,
	MNT_HANDICAP_30,
	MNT_HANDICAP_25,
	MNT_HANDICAP_20,
	MNT_HANDICAP_15,
	MNT_HANDICAP_10,
	MNT_HANDICAP_05,

	// Video Driver Menu
	MNT_VIDEODRIVER,
	MNT_VIDEODRIVERS,
	MNT_VENDOR,
	MNT_VERSION,
	MNT_RENDERER,
	MNT_PIXELFORMAT,
	MNT_EXTENSIONS,

	// Setup / Controls / Weapons
	MNT_WEAPONKEY_SETUP,

	// Sound Menu
	MNT_SOUND_SETUP,

	// Game Options
	MNT_CROSSHAIR_NONE,
	MNT_GAMEOPTION_LABEL,

	// Quit screen
	MNT_SHIP_SYSTEMS,
	MNT_QUIT_ELITE_FORCE,

	// Multiplayer Server
	MNT_MASTER_ITEMS_LOCAL,
	MNT_MASTER_ITEMS_MPLAYER,
	MNT_MASTER_ITEMS_INTERNET1,
	MNT_MASTER_ITEMS_FAVORITES,
	MNT_SERVERTYPE_ALL,
	MNT_SERVERTYPE_FREE_FOR_ALL,
	MNT_SERVERTYPE_TEAM_DEATHMATCH,
	MNT_SERVERTYPE_TOURNAMENT,
	MNT_SERVERTYPE_CAPTURETHEFLAG,
	MNT_SERVERTYPE_EXCESSIVE,
	MNT_SERVERTYPE_FEDERATION,
	MNT_SERVERTYPE_GLADIATOR,
	MNT_SERVERTYPE_IN2TAGIB,
	MNT_SERVERTYPE_PINBALL,
	MNT_SERVERTYPE_Q32EF,
	MNT_SERVERTYPE_RPG,
	MNT_SERVERTYPE_TEAMELITE,
	MNT_SERVERTYPE_RPGX, //RPG-X: TiM - Cycle RPG-X as a gtype option as well :)
	MNT_SORTKEY_SERVER_NAME,
	MNT_SORTKEY_MAP_NAME,
	MNT_SORTKEY_OPENPLAYER,
	MNT_SORTKEY_GAMETYPE,
	MNT_SORTKEY_PINGTIME,

	MNT_FINDSERVER,
	MNT_CREATESERVER,
	MNT_SPECIFYSERVER,

	//	Specify server
	MNT_SERVER_ADDRESS,
	MNT_SERVER_PORT,

	MNT_TYPE_FREEFORALL,
	MNT_TYPE_TEAMDEATHMATCH,
	MNT_TYPE_TOURNAMENT,
	MNT_TYPE_CAPTURETHEFLAG,

	MNT_NO,
	MNT_YES,

	// Mods Menu
	MNT_ELITEFORCE_HOLOMATCH,
	MNT_MODS,
	MNT_AVAILABLEMODS,

	// Single Player Menu
	MNT_SINGLEPLAYER,			
	MNT_TIER,
	MNT_ACCESSDENIED,
	MNT_OPPONENTS,
	MNT_PLAYER_DATA,

	// Change Player Menu
	MNT_CHANGEPLAYER,

	// Single Player Reset Menu
	MNT_RESETPLAYER,

	// Setup / Controls / Attack/Look
	MNT_ATTACKLOOKKEY_SETUP,

	// Setup / Controls / Movement
	MNT_MOVEMENTKEYS_SETUP,

	// Setup / Controls / Mouse/JoyStick
	MNT_MOUSEJOYSTICK_SETUP,
	MNT_MOUSE,
	MNT_JOYSTICK,

	// Skill levels
	MNT_BABYLEVEL,
	MNT_EASYLEVEL,
	MNT_MEDIUMLEVEL,
	MNT_HARDLEVEL,
	MNT_NIGHTMARELEVEL,

	// Dedicated server
	MNT_DEDICATED_NO,
	MNT_DEDICATED_LAN,
	MNT_DEDICATED_INTERNET,

	// Player type
	MNT_OPEN,
	MNT_BOT,
	MNT_CLOSED,

	// Player team
	MNT_TEAM_BLUE,
	MNT_TEAM_RED,

	// Single Player Reset
	MNT_PLAYER_RESET1,
	MNT_PLAYER_RESET2,
	MNT_PLAYER_RESET3,
	MNT_RESET_PLAYER,

	//Demo screen
	MNT_NO_DEMOS_FOUND,

	MNT_ENTER_CDKEY,
	MNT_VALID_CDKEY,
	MNT_CDKEY_INVALID,

	MNT_LOADING,
	MNT_CONNECTINGTO,
	MNT_PRESSESCAPETOABORT,
	
	MNT_WAITINGFORNEWKEY,
	MNT_CLICKTOCHANGE,
	MNT_BACKSPACETOCLEAR,
	MNT_USEARROWKEYS,
	MNT_RESET1,
	MNT_RESET2,
	MNT_QUIT1,
	MNT_QUIT2,

	MNT_NAME,
	MNT_EFFECTS,

	MNT_ENTERFILENAME,

	MNT_HOSTNAME,
	MNT_MAP,
	MNT_PLAYERS,
	MNT_TYPE,
	MNT_PORT,
	MNT_PING,

	MNT_DEFAULT1,
	MNT_DEFAULT2,

	MNT_ELIMINATED,
	MNT_TIE,


	MNT_COMPLETERESET1,
	MNT_COMPLETERESET2,
	MNT_COMPLETERESET3,
	MNT_COMPLETERESET4,

	MNT_NOLIMIT,

	MNT_SHORTCUT_KEY,

	MNT_SHORTCUT_SCORES,
	MNT_SHORTCUT_USE,
	MNT_SHORTCUT_RUNWALK,
	MNT_SHORTCUT_WALKFORWARD,
	MNT_SHORTCUT_BACKPEDAL,
	MNT_SHORTCUT_STEPLEFT,
	MNT_SHORTCUT_STEPRIGHT,
	MNT_SHORTCUT_UPJUMP,
	MNT_SHORTCUT_DOWNCROUCH,
	MNT_SHORTCUT_TURNLEFT,
	MNT_SHORTCUT_TURNRIGHT,
	MNT_SHORTCUT_SIDESTEPTURN,
	MNT_SHORTCUT_LOOKUP,
	MNT_SHORTCUT_LOOKDOWN,
	MNT_SHORTCUT_MOUSELOOK,
	MNT_SHORTCUT_CENTERVIEW,
	MNT_SHORTCUT_ZOOMVIEW,
	MNT_SHORTCUT_WEAPON0,
	MNT_SHORTCUT_WEAPON1,
	MNT_SHORTCUT_WEAPON2,
	MNT_SHORTCUT_WEAPON3,
	MNT_SHORTCUT_WEAPON4,
	MNT_SHORTCUT_WEAPON5,
	MNT_SHORTCUT_WEAPON6,
	MNT_SHORTCUT_WEAPON7,
	MNT_SHORTCUT_WEAPON8,
	MNT_SHORTCUT_WEAPON9,
	MNT_SHORTCUT_ATTACK,
	MNT_SHORTCUT_ALTATTCK,
	MNT_SHORTCUT_PREVWEAPON,
	MNT_SHORTCUT_NEXTWEAPON,
	MNT_SHORTCUT_GESTURE,

	MNT_SHORTCUT_EQUIP,
	MNT_SHORTCUT_DROP, // RPG-X | Marcin | 04/12/2008

	MNT_SHORTCUT_CHAT,
	MNT_SHORTCUT_CHATTEAM,
	MNT_SHORTCUT_CHATTARGET,
	MNT_SHORTCUT_CHATATTACKER,
	MNT_SHORTCUT_CHATCLASS,
	//TiM - 3rd Person view commands (Not necessarily modelview related )
	MNT_SHORTCUT_TOGGLE3RDPERSON,
	MNT_SHORTCUT_COMMITANGLES,
	MNT_SHORTCUT_REVERTANGLES,
	MNT_SHORTCUT_RESETANGLES,
	//RPG-X TiM - Modelview Buttonzors
	MNT_SHORTCUT_MODELVIEW_FORWARD,
	MNT_SHORTCUT_MODELVIEW_BACKWARD,
	MNT_SHORTCUT_MODELVIEW_LEFT,
	MNT_SHORTCUT_MODELVIEW_RIGHT,
	MNT_SHORTCUT_MODELVIEW_UP,
	MNT_SHORTCUT_MODELVIEW_DOWN,
	MNT_SHORTCUT_MODELVIEW_ROTLEFT,
	MNT_SHORTCUT_MODELVIEW_ROTRIGHT,
	MNT_SHORTCUT_MODELVIEW_PITCHUP,
	MNT_SHORTCUT_MODELVIEW_PITCHDOWN,

	MNT_MODELVIEW_SETUP,	//RPG-X TiM

	MNT_DEFAULT_WARNING1,
	MNT_DEFAULT_WARNING2,
	MNT_DEFAULT_WARNING3,

	MNT_CONTINUE_WARNING,
	MNT_DEFAULT_SETUP,
	MNT_OTHEROPTIONS_SETUP,

	MNT_COMMANDKEYS_SETUP,

	MNT_TO_OFF,
	MNT_TO_UPPER_RIGHT,
	MNT_TO_LOWER_RIGHT,
	MNT_TO_LOWER_LEFT,

	MNT_NW_28,
	MNT_NW_33,
	MNT_NW_56,
	MNT_NW_ISDN,
	MNT_NW_CABLE,

	MNT_NETWORKSETUP,
	MNT_SERVERINFO,
	MNT_SERVEROPTIONS,

	MNT_HCSELECT,
	MNT_SERVERINFO_TITLE,
	MNT_FREE,

	MNT_ADD_HCS_TITLE,
	MNT_ADD_HCS,

	MNT_HOLOGRAPHIC_CHARS,
	MNT_SETTINGS,

	MNT_REMOVE_HCS_TITLE,
	MNT_REMOVE_HCS,

	MNT_TEAM_ORDERS_TITLE,
	MNT_TEAM_ORDERS,

	MNT_ORDER_IMLEADER,
	MNT_ORDER_DEFEND,
	MNT_ORDER_FOLLOW,
	MNT_ORDER_GETFLAG,
	MNT_ORDER_CAMP,
	MNT_ORDER_REPORT,
	MNT_ORDER_IRELINQUISH,
	MNT_ORDER_ROAM,

	MNT_ORDER_NAME_DEFEND,
	MNT_ORDER_NAME_FOLLOW,
	MNT_ORDER_NAME_GETFLAG,
	MNT_ORDER_NAME_CAMP,
	MNT_ORDER_NAME_REPORT,
	MNT_ORDER_NAME_ROAM,

	MNT_NETWORK,
	MNT_SOUND,

	MNT_CNT_SERVERS,
	MNT_SPACETOSTOP,

	MNT_NORESPONSE_MASTER,
	MNT_NO_SERVERS,

	MNT_HITREFRESH,
	MNT_SCANNINGFORSERVERS,

	MNT_CURRENTMAP,
	MNT_SORTPARMS,

	MNT_SERVERINFORMATION,
	MNT_WINNERS1,
	MNT_WINNERS2,

	MNT_ENGLISH,
	MNT_GERMAN,

	MNT_BRIDGE_DESC1,
	MNT_PHASER_STRIP1_DESC1,
	MNT_TORPEDOS_DESC1,
	MNT_VENTRAL_DESC1,
	MNT_MIDHULL_DESC1,
	MNT_BUSSARD_DESC1,
	MNT_NACELLES_DESC1,
	MNT_THRUSTERS_DESC1,
	MNT_VOYAGER_DESC1,

	MNT_SOLOMATCHTYPES,
	MNT_LEVELS,
	MNT_SKILLLEVEL,
	MNT_POINTLIMIT,

	MNT_HUMAN,
	MNT_SERVERPLAYERS,
	MNT_ACCESSING,
	MNT_HOLOCHARSLINE,
	MNT_SAFE,
	MNT_BEST,
	MNT_MODELS,
	MNT_CDKEY,

	MNT_AWAITINGCHALLENGE,
	MNT_AWAITINGAUTHORIZATION,
	MNT_AWAITINGCONNECTION,
	MNT_AWAITINGGAMESTATE,

	MNT_DUTCH,
	MNT_NOMAPSFOUND,
	MNT_ADVANCEDSERVER,
	MNT_ADV_ERROR,
	MNT_ADV_STATUS1_RESPAWN,
	MNT_ADV_STATUS1_MAXCLIENTS,
	MNT_ADV_STATUS1_RUNSPEED,
	MNT_ADV_STATUS1_GRAVITY,
	MNT_ADV_STATUS1_KNOCKBACK,
	MNT_ADV_STATUS1_DMGMULT,
	MNT_ADV_STATUS1_BOT_MINPLAYERS,

	MNT_HOLODECK1,
	MNT_WELCOMETOHOLMATCH,

	MNT_HOLOTITLE1,
	MNT_HOLOTITLE2,
	MNT_HOLOTITLE3,
	MNT_HOLOTITLE4,
	MNT_HOLOTITLE5,
	MNT_HOLOTITLE6,
	MNT_HOLOTITLE7,
	MNT_HOLOTITLE8,
	MNT_HOLOTITLE9,
	MNT_HOLOTITLE10,
	MNT_HOLOTITLE11,
	MNT_HOLOTITLE12,
	MNT_HOLOTITLE13,
	MNT_HOLOTITLE14,
	MNT_HOLOTITLE15,
	MNT_HOLOTITLE16,
	MNT_HOLOTITLE17,
	MNT_HOLOTITLE18,

	MNT_HOLOMATCH,
	MNT_PARAMOUNT_LEGAL,
	MNT_ID_LEGAL,
	MNT_LEAVINGHOLMATCH,
	MNT_OR,
	MNT_CONFIRMATIONMENU_TITLE,
	MNT_CONFIRMATION,
	MNT_RESTART_MATCH,
	MNT_MASTER_ITEMS_INTERNET2,
	MNT_MASTER_ITEMS_INTERNET3,
	MNT_MASTER_ITEMS_INTERNET4,
	MNT_MASTER_ITEMS_INTERNET5,
	MNT_ADV_STATUS2_RESPAWN,
	MNT_ADV_STATUS2_MAXCLIENTS,
	MNT_ADV_STATUS2_RUNSPEED,
	MNT_ADV_STATUS2_GRAVITY,
	MNT_ADV_STATUS2_KNOCKBACK,
	MNT_ADV_STATUS2_DMGMULT,
	MNT_ADV_STATUS2_BOT_MINPLAYERS,
	MNT_STARTREK1,
	MNT_STARTREK2,
	MNT_FORMOREINFO,
	MNT_UBERGAMES,
	MNT_UBERGAMES2,
	MNT_UBERGAMESINFO,
	MNT_BACKSPACE,

	MNT_GAMMA_LINE1,
	MNT_GAMMA_LINE2,
	MNT_GAMMA_LINE3,

	MNT_INITIALSETUP_TITLE,
	MNT_INITIALSETUP,

	MNT_WEAPON1_DESC,
	MNT_WEAPON2_DESC,
	MNT_WEAPON3_DESC,
	MNT_WEAPON4_DESC,
	MNT_WEAPON5_DESC,
	MNT_WEAPON6_DESC,
	MNT_WEAPON7_DESC,
	MNT_WEAPON8_DESC,
	MNT_WEAPON9_DESC,
	MNT_ACCESSINGLCARS,
	MNT_CLOSINGLCARS,

	MNT_INSERTCD,
	MNT_BADCDKEY,

	MNT_ADV_STATUS1_FORCEPLAYERRESPAWN,
	MNT_ADV_STATUS2_FORCEPLAYERRESPAWN,
	MNT_ADV_STATUS1_RESPAWNINVULNERABILITY,
	MNT_ADV_STATUS2_RESPAWNINVULNERABILITY,
	MNT_ADV_STATUS1_DOWARMUP,
	MNT_ADV_STATUS2_DOWARMUP,
	MNT_HOLODECKSIMULATION,
	MNT_HOLOMATCHWEAPONRY,
	MNT_BLUE,
	MNT_RED,
	MNT_SCREEN,
	MNT_OF,

	MNT_DOWNLOADING,
	MNT_ESTIMATEDTIMELEFT,
	MNT_TRANSFERRATE,
	MNT_ESTIMATING,
	MNT_OFCOPIED,
	MNT_COPIED,
	MNT_SEC,
	MNT_ANVANCEDMENU_TITLE,
	MNT_LEAVE_MATCH,
	MNT_FONTS_DEVELOPER,
	MNT_THISWILLRESETMENU,
	MNT_GAMMA2_LINE1,
	MNT_GAMMA2_LINE2,
	MNT_RECOMMENDEDPLAYERS,
	MNT_LOOSEVIDSETTINGS,
	MNT_UNPURECLIENT,
	MNT_CANNOTVALIDATE,
	MNT_KICKED,
	MNT_TIMEDOUT,
	MNT_SERVERSHUTDOWN,
	MNT_DISCONNECTED,
	MNT_BROKENDOWNLOAD,
	MNT_SERVERCOMMANDOVERFLOW,
	MNT_LOSTRELIABLECOMMANDS,
	MNT_A3DNOTAVAILABLE,
	MNT_PC_NOCLASS,
	MNT_PC_INFILTRATOR,
	MNT_PC_SNIPER,
	MNT_PC_HEAVY,
	MNT_PC_DEMO,
	MNT_PC_MEDIC,
	MNT_PC_TECH,
	MNT_PC_BORG,
	MNT_SHORTCUT_USEINVENTORY,
	MNT_SHORTCUT_USEOBJECT,
	MNT_SHORTCUT_OBJECTIVES,
	MNT_PARAMETERS,
	MNT_ADV_STATUS1_NOJOINTIMEOUT,
	MNT_ADV_STATUS2_NOJOINTIMEOUT,
	MNT_ADV_STATUS1_CLASSCHANGETIMEOUT,
	MNT_ADV_STATUS2_CLASSCHANGETIMEOUT,
	MNT_ACTIONHERO,
	//RPG-X: Redtechie - For added class support in UI Menu
	
	// Credits Menu
	MNT_CREDITS,

	//Emotes,
	MNT_EMOTES,

	//ADmin
	MNT_ADMIN,
	
	MNT_PC_ALPHAOMEGA22,
	MNT_PC_N00B,
	MNT_CHANGEAPPLIED,
	MNT_CHARS,
	MNT_CHARDATA,
	
	MNT_CHOOSEEMOTE,
	MNT_MODELOFFSET,
	MNT_EMOTELIST,
	MNT_EMOTEPARAM,
	
	MNT_PLAYERSTATS,
	MNT_CLASS,
	MNT_RANK,
	MNT_AGE,
	MNT_RACE,
	MNT_HEIGHT,
	MNT_WEIGHT,
	MNT_MODEL,
	MNT_ID,
	MNT_PLAYERSTATUS,

	MNT_QUICKCONNECT,
	MNT_QUICKMESSAGE,
	MNT_QUICKMESSAGE2,

	MNT_MAIN_CMDS,
	MNT_GIVE_CMDS,
	MNT_REVIVE_CMDS,
	MNT_BROADCAST_CMDS,
	MNT_USEENT_CMDS,
	MNT_BEAM_CMDS,
	MNT_FXGUN_CMDS,
	MNT_SPAWN_CMDS,
	MNT_BIND_CMDS,

	MNT_MYSELF,
	MNT_EVERYONE,
	MNT_MINE,
	MNT_ALL,
	MNT_RADIUS,
	MNT_CHUNKTYPE,
	MNT_SPARKINTERVAL,
	MNT_TIMELENGTH,
	MNT_DRIPTYPE,
	MNT_DRIPINTENSITY,
	MNT_SHAKEINTENSITY,
	MNT_KEYBINDINGS,

	MNT_CLIENT_INFO_CMDS,
	MNT_KICK_CLIENT_CMDS,
	MNT_FORCE_PARAM_CMDS,

	MNT_PLAYMUSIC,
	MNT_PLAYSOUND,

	MNT_TURBOLIFT,
	MNT_SPECFICYDECK,

	MNT_TRANSPORTER,
	MNT_HOLODECK,

	MNT_ID_NOTTHERE,
	MNT_ID_WRONGSIZE,
	MNT_ID_INVALID,

	MNT_MAX
} menuNormalTextType_t;

extern const char *menu_normal_text[MNT_MAX];


// Menu Button Text enum
typedef enum
{
	MBT_NONE,

	// Main Menu
	MBT_MULTIPLAYER,
	MBT_SETUP,
	MBT_DEMO,
	MBT_QUIT,
	MBT_MODS,
	MBT_CREDITS,

	// Generic Stuff
	MBT_GENERIC_NO,
	MBT_GENERIC_YES,

	// Demos Screen
	MBT_MAINMENU,
	MBT_ENGAGEDEMO,
	MBT_DEMOLINE,

	//Credits
	MBT_LEADS,
	MBT_CODERS,
	MBT_2D,
	MBT_3D,
	MBT_MAPPING,
	MBT_SOUNDS,
	MBT_EXTRAS,
	MBT_BETAS,
	MBT_THANKS,

	// Settings Top Buttons
	MBT_VIDEOSETTINGS,
	MBT_CONTROLS,
	MBT_SOUNDSETTINGS,
	MBT_GAMEOPTIONS,
	MBT_CDKEY,
	MBT_PLAYER,
	MBT_FONTS,

	// Video Data
	MBT_VIDEOOPTIONS,
	//MBT_VIDEODRIVER,
	MBT_ASPECTRATIO,
	MBT_VIDEOGLEXTENTIONS,
	MBT_VIDEOMODE,
	MBT_VIDEOCOLORDEPTH,
	MBT_VIDEOFULLSCREEN,
	MBT_VIDEOLIGHTING,
	MBT_VIDEOGEOMETRY,
	MBT_VIDEOTEXTUREDETAIL,
	MBT_VIDEOTEXTUREBITS,
	MBT_VIDEOTEXTUREFILTER,
	MBT_VIDEOAPPLYCHANGES,
	MBT_VIDEODATA,
	MBT_VIDEODATA2,
	MBT_VIDEODRIVERS,


	//	Video 2 Data
	MBT_BRIGHTNESS,
	MBT_SCREENSIZE,

	// Setup / Sound
	MBT_EFFECTSVOLUME,
	MBT_MUSICVOLUME,
	MBT_VOICEVOLUME,
	MBT_SOUNDQUALITY,
	MBT_A3D,

	MBT_WAITFORKEY,
	MBT_WEAPONKEYS,
	MBT_LOOKKEYS,
	MBT_MOVEMENTKEYS,
	MBT_COMMANDKEYS,
	MBT_MODELVIEWKEYS,
	MBT_MOUSECONFIG,
	MBT_OTHEROPTIONS,

	MBT_ON,
	MBT_OFF,

	// Player settings
	MBT_PLAYER_HANDICAP,
	MBT_PLAYER_NAME,

	// Game Options
	MBT_WALLMARKS1,
	MBT_WALLMARKS2,
	MBT_DYNAMICLIGHTS1,
	MBT_DYNAMICLIGHTS2,
	MBT_LIGHTFLARES1,
	MBT_LIGHTFLARES2,
	MBT_IDENTIFYTARGET1,
	MBT_IDENTIFYTARGET2,
	MBT_SYNCEVERYFRAME1,
	MBT_SYNCEVERYFRAME2,
	MBT_CROSSHAIR,

	// Quit Game
	MBT_QUITGAME_YES,
	MBT_QUITGAME_NO,

	// Single Player Server
	/*MBT_RESETSTANDINGS,
	MBT_CREATEMATCH,
	MBT_PRESETGAME,
	MBT_NEXTGRADE,
	MBT_PREVGRADE,
	MBT_PLAYERINFO,
	MBT_STARTMATCH1,
	MBT_STARTMATCH2,
	MBT_ABORT,*/
	MBT_ENGAGE,

	// Skill levels
	MBT_BABYLEVEL,
	MBT_EASYLEVEL,
	MBT_MEDIUMLEVEL,
	MBT_HARDLEVEL,
	MBT_NIGHTMARELEVEL,

	// Multiplayer Server
	MBT_SERVERS,
	MBT_SERVERTYPE_GAMETYPE,
	MBT_SORTTYPE,
	MBT_SHOWFULL,
	MBT_SHOWEMPTY,
	MBT_REFRESH,
	MBT_STOPREFRESH,

	MBT_GAMETYPE,

	MBT_SEARCHSERVER,
	MBT_SPECIFY,
	MBT_CREATE,

	MBT_ENGAGEMULTIPLAYER,

	MBT_LOADMOD,

	// Single Player reset
	MBT_YESRESET,
	MBT_NORESET,

	MBT_CHANGEMODEL,

	// Ingame menu
	MBT_INGAMETEAM,
	MBT_INGAMEADDSIMULANTS,
	MBT_INGAMEREMOVESIMULANTS,
	MBT_INGAMETEAMORDERS,
	MBT_INGAMESETUP,
	MBT_INGAMESERVERDATA,
	MBT_INGAMERESTART,
	MBT_INGAMERESUME,
	MBT_INGAMELEAVE,
	MBT_GAMEQUIT,

	// Team menu
	MBT_REDTEAM,
	MBT_BLUETEAM,
	MBT_JOINMATCH,
	MBT_OBSERVER,

	MBT_INGAMEMENU,

	// Setup/ Controls / Mouse Config
	MBT_MOUSEFREELOOK,
	MBT_MOUSESPEED,
	MBT_MOUSEINVERT,
	MBT_MOUSESMOOTH,
	MBT_JOYSTICKENABLE,
	MBT_JOYSTICKTHRESHOLD,
	MBT_FORCEFEEDBACK,

	MBT_BACK,
	MBT_PLAYERDATA,
	MBT_NEXTPAGE,
	MBT_PREVPAGE,

	// Ingame Quit Screen
	MBT_QUIT_NO,
	MBT_QUIT_YES,

	MBT_CDKEY2,

	MBT_MISSION,

	MBT_SETDEFAULT,

	MBT_DEFAULT_NO,
	MBT_DEFAULT_YES,

	MBT_KEYTURNSPEED,
	MBT_ALWAYSRUN,
	MBT_LOOKSPRING,
	MBT_AUTOSWITCHWEAPONS,

	MBT_FORCEMODEL,
	MBT_DRAWTEAMOVERLAY,
	MBT_ALLOWDOWNLOAD,
	MBT_SIMPLEITEMS,

	MBT_NETWORK,
	MBT_DATARATE,

	MBT_ADDTOFAVS,

	MBT_PREVMAPS,
	MBT_NEXTMAPS,
	MBT_SETPARAMETERS,
 
	MBT_POINTLIMIT,
	MBT_CAPTURELIMIT,
	MBT_TIMELIMIT,
	MBT_FRIENDLYFIRE,
	MBT_PURESERVER,
	MBT_DEDICATEDSERVER,
	MBT_HOSTNAME,
	MBT_HCSKILL,

	MBT_PLAYER1,
	MBT_PLAYER2,
	MBT_PLAYER3,
	MBT_PLAYER4,
	MBT_PLAYER5,
	MBT_PLAYER6,
	MBT_PLAYER7,
	MBT_PLAYER8,
	MBT_PLAYER9,
	MBT_PLAYER10,
	MBT_PLAYER11,
	MBT_PLAYER12,

	MBT_CHOOSEMAP,

	MBT_PREVHC,
	MBT_NEXTHC,
	MBT_ACCEPT,
	MBT_TEAM,
	MBT_SKILL,

	MBT_REMOVE_HOLO,

	MBT_ARROW_UP,
	MBT_ARROW_DOWN,

	MBT_REMOVE,

	MBT_ADDRESS,
	MBT_PORT,

	MBT_REPLAY,
	MBT_NEXTMATCH,

	MBT_RETURNMENU,

	MBT_LANGUAGE,

	MBT_FEDERATION_LABEL,
	MBT_CARDASSIA_LABEL,
	MBT_FERENGI_LABEL,
	MBT_ROMULAN_LABEL,
	MBT_KLINGON_LABEL,
	MBT_DOMINION_LABEL,
	MBT_VOYAGER_LABEL1,
	MBT_VOYAGER_LABEL2,
	MBT_BORG_LABEL1,
	MBT_BORG_LABEL2,
	MBT_WORMHOLE_LABEL1,
	MBT_WORMHOLE_LABEL2,
	MBT_GALACTICCORE_LABEL,

	MBT_ALPHA,
	MBT_BETA,
	MBT_DELTA,
	MBT_GAMMA,
	MBT_QUADRANT,

	MBT_V_PHASER_LABEL,
	MBT_V_TORPEDOS_LABEL,
	MBT_V_TORPEDOS_LABEL2,
	MBT_V_TORPEDOS_LABEL3,
	MBT_V_VENTRAL_LABEL,
	MBT_V_MIDHULL_LABEL,
	MBT_V_MIDHULL_LABEL2,
	MBT_V_BUSSARD_LABEL,
	MBT_V_NACELLES_LABEL,
	MBT_V_THRUSTERS_LABEL,
	MBT_V_VOYAGER_LABEL,
	MBT_V_BRIDGE_LABEL,
	MBT_SCREENSHOT,
	MBT_SKINDEFAULT,
	MBT_SKINRED,
	MBT_SKINBLUE,

	MBT_LIGHTFLARES,
	MBT_ADVANCEDSERVER,
	MBT_AUTOJOIN,
	MBT_AUTOBALANCE,
	MBT_MAXCLIENTS,
	MBT_FALLINGDAMAGE,
	MBT_RESPAWNTIME,
	MBT_RUNSPEED,
	MBT_GRAVITY,
	MBT_KNOCKBACK,
	MBT_DMGMULT,
	MBT_SIMPLESHADER,
	MBT_BOT_MINPLAYERS,
	MBT_WEAPON1,
	MBT_WEAPON2,
	MBT_WEAPON3,
	MBT_WEAPON4,
	MBT_WEAPON5,
	MBT_WEAPON6,
	MBT_WEAPON7,
	MBT_WEAPON8,
	MBT_WEAPON9,
	MBT_ANISOTROPICFILTERING,
	MBT_COMPRESSEDTEXTURES,
	MBT_ADAPTITEMRESPAWN,
	MBT_HOLODECKINTRO,
	MBT_FORCEPLAYERRESPAWN,
	MBT_RESPAWNINVULNERABILITY,
	MBT_WARMUP,
	MBT_BLUETEAMGROUP,
	MBT_REDTEAMGROUP,
	MBT_TEXTLANGUAGE,
	MBT_VOICELANGUAGE,
	MBT_RESETPROGRESS,
	MBT_GROUPFILTER,
	MBT_CHECKKEYLATER,
	MBT_FONTSSMALL,
	MBT_FONTSMEDIUM,
	MBT_FONTSLARGE,
	MBT_FONTSGRID,
	MBT_KEYBOARDLANGUAGE,
	MBT_X_AXIS,
	MBT_Y_AXIS,
	MBT_ASSIMILATION,
	MBT_SPECIALTIES,
	MBT_DISINTEGRATION,
	MBT_ACTIONHERO,
	MBT_ELIMINATION,
	MBT_PLAYERCLASS,
	MBT_TEAMCLASS,
	MBT_AUTOTEAM,
	MBT_NOJOINTIMEOUT,
	MBT_CLASSCHANGE,
	MBT_CLASS,
	MBT_RANK,
	MBT_OTHER,
	
	//Player Settings
	MBT_AGE,
	MBT_RACE,
	MBT_HEIGHT,
	MBT_WEIGHT,
	MBT_COMMIT,
	MBT_RANKSETS,
	MBT_CLASSSETS,
	MBT_CHARMODEL,
	MBT_CHARSKIN,
	MBT_RACEFILTER,
	MBT_GENDERFILTER,
	MBT_PROFILESCROLL,
	MBT_LOADPROFILE,
	MBT_PROFILENAME,
	MBT_SAVEPROFILE,

	//RPG-X Emotes
	MBT_EMOTES_MENU,
	MBT_RECENT_FILTER,
	MBT_FAV_FILTER,
	MBT_ALL_FILTER,
	MBT_SITTING_FILTER,
	MBT_CONSOLE_FILTER,
	MBT_GESTURE_FILTER,
	MBT_FULLMOTION_FILTER,
	MBT_INJURED_FILTER,
	MBT_MISC_FILTER,
	MBT_MODEL_OFFSET,
	MBT_KEY_BIND,
	MBT_PRESS_KEY,
	MBT_FAV_EMOTE,
	MBT_KILL_FAV_EMOTE,
	MBT_DO_EMOTE,
	MBT_CHANGE_OFFSET,

	MBT_ADMIN_MENU,
	MBT_ADMIN_MAIN,
	MBT_ADMIN_CLIENTS,
	MBT_ADMIN_AUDIO,

	MBT_ADMIN_EXECUTE,

	MBT_ADMIN_GOD,
	MBT_ADMIN_NOCLIP,
	MBT_ADMIN_CLOAK,
	MBT_ADMIN_FLIGHT,

	MBT_ADMIN_ITEM,

	MBT_ADMIN_REVIVE,

	MBT_ADMIN_MESSAGE,

	MBT_ADMIN_ENTITY,

	MBT_ADMIN_BEAMLOC,
	MBT_ADMIN_BEAMPLAYER,

	MBT_ADMIN_FX,
	MBT_ADMIN_PARM1,
	MBT_ADMIN_PARM2,
	MBT_ADMIN_PARM3,

	MBT_ADMIN_SPAWN,
	MBT_ADMIN_REMOVE,

	MBT_ADMIN_SPAWNB,
	MBT_ADMIN_USEENTB,
	MBT_ADMIN_KICKB,

	MBT_CLIENT_SELECT,

	MBT_CLIENT_KICK,
	MBT_CLIENT_FORCE,
	MBT_CLIENT_PARAM,

	MBT_AUDIO_PLAYSONG,
	MBT_AUDIO_STOPSONG,

	MBT_AUDIO_PLAYSND,

	MBT_DECK,

	MBT_CONTINUE,

	MBT_MOTD,	// RPG-X | Marcin | 03/01/2009
	MBT_RESPAWN,// RPG-X | Marcin | 03/01/2009

	MBT_TRANS_EXTERN,
	MBT_TRANS_DELAY,
	MBT_TRANS_DELAY_PARAM,
	
	MBT_HOLODECK_LIST,
	MBT_HOLODECK_START,
	MBT_HOLODECK_LEAVE,

	MBT_MAX

} menuButtonTextType_t;

  
#define GRAPHIC_BUTTONRIGHT		"menu/new/bar1.tga"
#define GRAPHIC_BUTTONLEFT		"menu/new/bar2.tga"
#define GRAPHIC_BUTTONSLIDER	"menu/common/slider.tga"
#define GRAPHIC_BUTTONROUND		"button_round.tga"

#define MENU_BUTTON_MED_HEIGHT	18
#define MENU_BUTTON_MED_WIDTH	130
#define MENU_TITLE_X			611
#define MENU_TITLE_Y			24
#define MENU_DESC_X				100
#define MENU_DESC_Y				444
#define MENU_BUTTON_TEXT_X		5
#define MENU_BUTTON_TEXT_Y		1

#define MG_OFF		0
#define MG_STRING	1
#define MG_GRAPHIC	2
#define MG_NUMBER	3
#define MG_NONE		4
#define MG_VAR		5

extern const char *menu_button_text[MBT_MAX][2];


extern vmCvar_t	ui_ffa_fraglimit;
extern vmCvar_t	ui_ffa_timelimit;

extern vmCvar_t	ui_tourney_fraglimit;
extern vmCvar_t	ui_tourney_timelimit;

extern vmCvar_t	ui_team_fraglimit;
extern vmCvar_t	ui_team_timelimit;
extern vmCvar_t	ui_team_friendly;

extern vmCvar_t	ui_ctf_capturelimit;
extern vmCvar_t	ui_ctf_timelimit;
extern vmCvar_t	ui_ctf_friendly;

extern vmCvar_t	ui_arenasFile;
extern vmCvar_t	ui_botsFile;
extern vmCvar_t	ui_botminplayers;
extern vmCvar_t	ui_spScores1;
extern vmCvar_t	ui_spScores2;
extern vmCvar_t	ui_spScores3;
extern vmCvar_t	ui_spScores4;
extern vmCvar_t	ui_spScores5;
extern vmCvar_t	ui_spAwards;
extern vmCvar_t	ui_spVideos;
extern vmCvar_t	ui_spSkill;

extern vmCvar_t	ui_weaponrespawn;
extern vmCvar_t	ui_speed;
extern vmCvar_t	ui_gravity;
extern vmCvar_t	ui_knockback;
extern vmCvar_t	ui_dmgmult;
extern vmCvar_t	ui_adaptRespawn;
extern vmCvar_t	ui_holoIntro;
extern vmCvar_t	ui_forcerespawn;
extern vmCvar_t	ui_respawnGhostTime;
extern vmCvar_t	ui_warmup;
extern vmCvar_t	ui_dowarmup;
extern vmCvar_t	ui_team_race_blue;
extern vmCvar_t	ui_team_race_red;

extern vmCvar_t	ui_spSelection;

extern vmCvar_t	ui_browserMaster;
extern vmCvar_t	ui_browserGameType;
extern vmCvar_t	ui_browserSortKey;
extern vmCvar_t	ui_browserShowFull;
extern vmCvar_t	ui_browserShowEmpty;

extern vmCvar_t	ui_brassTime;
extern vmCvar_t	ui_drawCrosshair;
extern vmCvar_t	ui_drawCrosshairNames;
extern vmCvar_t	ui_marks;

extern vmCvar_t	ui_defaultChar;

extern vmCvar_t	ui_server1;
extern vmCvar_t	ui_server2;
extern vmCvar_t	ui_server3;
extern vmCvar_t	ui_server4;
extern vmCvar_t	ui_server5;
extern vmCvar_t	ui_server6;
extern vmCvar_t	ui_server7;
extern vmCvar_t	ui_server8;
extern vmCvar_t	ui_server9;
extern vmCvar_t	ui_server10;
extern vmCvar_t	ui_server11;
extern vmCvar_t	ui_server12;
extern vmCvar_t	ui_server13;
extern vmCvar_t	ui_server14;
extern vmCvar_t	ui_server15;
extern vmCvar_t	ui_server16;
extern vmCvar_t	ui_server17;
extern vmCvar_t	ui_server18;
extern vmCvar_t	ui_server19;
extern vmCvar_t	ui_server20;
extern vmCvar_t	ui_server21;
extern vmCvar_t	ui_server22;
extern vmCvar_t	ui_server23;
extern vmCvar_t	ui_server24;
extern vmCvar_t	ui_server25;
extern vmCvar_t	ui_server26;
extern vmCvar_t	ui_server27;
extern vmCvar_t	ui_server28;
extern vmCvar_t	ui_server29;
extern vmCvar_t	ui_server30;
extern vmCvar_t	ui_server31;
extern vmCvar_t	ui_server32;

extern vmCvar_t	ui_cdkeychecked;
extern vmCvar_t	ui_cdkeychecked2;
extern vmCvar_t	ui_playerClass;
extern vmCvar_t	ui_playerRank;

extern	vmCvar_t	ui_allowvote;
extern	vmCvar_t	ui_chatsallowed;
extern	vmCvar_t	ui_allowsuicide;
extern	vmCvar_t	ui_selfdamage;
extern	vmCvar_t	ui_rpg;
extern	vmCvar_t	ui_kickspammers;
extern	vmCvar_t	ui_kicksuiciders;
extern	vmCvar_t	ui_allowspmaps;
extern	vmCvar_t	ui_rangetricorder;
extern	vmCvar_t	ui_rangehypo;
extern	vmCvar_t	ui_norpgclasses;
extern	vmCvar_t	ui_forceclasscolor;
extern	vmCvar_t	ui_nosecurity;
extern	vmCvar_t	ui_nomarine;
extern	vmCvar_t	ui_nomedical;
extern	vmCvar_t	ui_noscience;
extern	vmCvar_t	ui_nocommand;
extern	vmCvar_t	ui_noengineer;
extern	vmCvar_t	ui_noalien;
extern	vmCvar_t	ui_nomaker;
extern	vmCvar_t	ui_nocloak;
extern	vmCvar_t	ui_noflight;
extern	vmCvar_t	ui_phaserdmg;
extern	vmCvar_t	ui_rifledmg;
extern	vmCvar_t	ui_stasisdmg;
extern	vmCvar_t	ui_imoddmg;
extern	vmCvar_t	ui_noweapons;
extern	vmCvar_t	ui_alienflags;
extern	vmCvar_t	ui_marineflags;
extern	vmCvar_t	ui_securityflags;
extern	vmCvar_t	ui_makerflags;
extern	vmCvar_t	ui_medicalflags;
extern	vmCvar_t	ui_scienceflags;
extern	vmCvar_t	ui_commandflags;
extern	vmCvar_t	ui_engineerflags;
extern	vmCvar_t	ui_dynamicCrosshair;
extern	vmCvar_t	ui_dynamicLensFlares;
extern	vmCvar_t	ui_currentRankSet;
extern	vmCvar_t	ui_currentClassSet;

extern	vmCvar_t	ui_age;
extern	vmCvar_t	ui_height;
extern	vmCvar_t	ui_weight;
extern	vmCvar_t	ui_race;

//Emote related CVARs
extern	vmCvar_t	ui_recentEmote1;
extern	vmCvar_t	ui_recentEmote2;
extern	vmCvar_t	ui_recentEmote3;
extern	vmCvar_t	ui_recentEmote4;
extern	vmCvar_t	ui_recentEmote5;
extern	vmCvar_t	ui_recentEmote6;
extern	vmCvar_t	ui_recentEmote7;
extern	vmCvar_t	ui_recentEmote8;
extern	vmCvar_t	ui_recentEmote9;
extern	vmCvar_t	ui_recentEmote10;
extern	vmCvar_t	ui_recentEmote11;
extern	vmCvar_t	ui_recentEmote12;
extern	vmCvar_t	ui_recentEmote13;
extern	vmCvar_t	ui_recentEmote14;
extern	vmCvar_t	ui_recentEmote15;
extern	vmCvar_t	ui_recentEmote16;

//-Users can have up to 16 favorite emotes
extern	vmCvar_t	ui_favoriteEmote1;
extern	vmCvar_t	ui_favoriteEmote2;
extern	vmCvar_t	ui_favoriteEmote3;
extern	vmCvar_t	ui_favoriteEmote4;
extern	vmCvar_t	ui_favoriteEmote5;
extern	vmCvar_t	ui_favoriteEmote6;
extern	vmCvar_t	ui_favoriteEmote7;
extern	vmCvar_t	ui_favoriteEmote8;
extern	vmCvar_t	ui_favoriteEmote9;
extern	vmCvar_t	ui_favoriteEmote10;
extern	vmCvar_t	ui_favoriteEmote11;
extern	vmCvar_t	ui_favoriteEmote12;
extern	vmCvar_t	ui_favoriteEmote13;
extern	vmCvar_t	ui_favoriteEmote14;
extern	vmCvar_t	ui_favoriteEmote15;
extern	vmCvar_t	ui_favoriteEmote16;

//TiM - Ban system
extern	vmCvar_t		sv_securityHash;
extern	vmCvar_t		sv_securityCode;

extern	vmCvar_t		ui_handleWidescreen;

#define GRAPHIC_SQUARE "menu/common/square.tga"
#define BUTTON_GRAPHIC_LONGRIGHT "menu/common/bar1.tga"

//
// ui_qmenu.c
//

#define RCOLUMN_OFFSET			( BIGCHAR_WIDTH )
#define LCOLUMN_OFFSET			(-BIGCHAR_WIDTH )

#define SLIDER_RANGE			10
#define	MAX_EDIT_LINE			256

#define MAX_MENUDEPTH			8
#define MAX_MENUITEMS			64

#define MTYPE_NULL				0
#define MTYPE_SLIDER			1	
#define MTYPE_ACTION			2
#define MTYPE_SPINCONTROL		3
#define MTYPE_FIELD				4
#define MTYPE_RADIOBUTTON		5
#define MTYPE_BITMAP			6	
#define MTYPE_TEXT				7
#define MTYPE_SCROLLLIST		8
#define MTYPE_PTEXT				9
#define MTYPE_BTEXT				10

#define QMF_BLINK				0x00000001
#define QMF_SMALLFONT			0x00000002
#define QMF_LEFT_JUSTIFY		0x00000004
#define QMF_CENTER_JUSTIFY		0x00000008
#define QMF_RIGHT_JUSTIFY		0x00000010
#define QMF_NUMBERSONLY			0x00000020	// edit field is only numbers
#define QMF_HIGHLIGHT			0x00000040
#define QMF_HIGHLIGHT_IF_FOCUS	0x00000080	// steady focus
#define QMF_PULSEIFFOCUS		0x00000100	// pulse if focus
#define QMF_HASMOUSEFOCUS		0x00000200
#define QMF_NOONOFFTEXT			0x00000400
#define QMF_MOUSEONLY			0x00000800	// only mouse input allowed
#define QMF_HIDDEN				0x00001000	// skips drawing
#define QMF_GRAYED				0x00002000	// grays and disables
#define QMF_INACTIVE			0x00004000	// disables any input
#define QMF_NODEFAULTINIT		0x00008000	// skip default initialization
#define QMF_OWNERDRAW			0x00010000
#define QMF_PULSE				0x00020000
#define QMF_LOWERCASE			0x00040000	// edit field is all lower case
#define QMF_UPPERCASE			0x00080000	// edit field is all upper case
#define QMF_SILENT				0x00100000
#define QMF_ALTERNATE			0x00200000  // use sqare buttion instead of round one 
#define QMF_ALTERNATE2			0x00400000

// callback notifications
#define QM_GOTFOCUS				1
#define QM_LOSTFOCUS			2
#define QM_ACTIVATED			3

typedef struct _tag_menuframework
{
	int	cursor;
	int cursor_prev;

	int	nitems;
	void *items[MAX_MENUITEMS];

	void (*draw) (void);
	sfxHandle_t (*key) (int key);

	qboolean	wrapAround;
	qboolean	fullscreen;
	qboolean	nobackground;
	qboolean	initialized;		// Have the structures for this menu been initialized?
	int			descX;				// Description x pos
	int			descY;				// Description y pos
	int			listX;				// Beginning X position of list
	int			listY;				// Beginning Y position of list
	int			titleX;				// Title x pos
	int			titleY;				// Title y pos
	int			titleI;				// The title
	int			footNoteEnum;		// Footnote text

	void		*displaySpinList;	//if not NULL, display the list from this one (must be typecast as menulist_s when used)
	qboolean	noNewSelecting;		//used when we want to stop other buttons getting selected. mainly for the spin list, and slider dragging
} menuframework_s;

typedef struct
{
	int type;
	const char *name;
	int	id;
	int x, y;
	int left;
	int	top;
	int	right;
	int	bottom;
	menuframework_s *parent;
	int menuPosition;
	unsigned flags;

	void (*callback)( void *self, int event );
	void (*statusbar)( void *self );
	void (*ownerdraw)( void *self );
} menucommon_s;

typedef struct {
	int		cursor;
	int		scroll;
	int		widthInChars;
	char	buffer[MAX_EDIT_LINE];
	int		maxchars;
	int		style;
	int		titleEnum;		// Label
	int		titlecolor;		// Normal color
	int		textcolor;		// Normal color
	int		textcolor2;		// Highlight color
} mfield_t;

typedef struct
{
	menucommon_s	generic;
	mfield_t		field;
} menufield_s;

typedef struct 
{
	menucommon_s generic;

	float minvalue;
	float maxvalue;
	float curvalue;
	float defvalue; //RPG-X: TiM | I'm going to make it if u click the button part, it'll reset to this default value

	int				focusWidth;		// For graph
	int				focusHeight;	// For graph
	int				color;			// Normal color
	int				color2;			// Highlight color
	int				shader;			// Graph shader
	int				width;			// Graph bitmap width
	int				height;			// Graph bitmap height
	char			*thumbName;		// Thumb file name
	int				thumbShader;	// THumb shader
	int				thumbWidth;		// Width of thumb graphic
	int				thumbHeight;	// Height of thumb graphic
	int				thumbColor;		// Normal color
	int				thumbColor2;	// Highlight color
	int				thumbGraphicWidth;	// Width of visible part of graphic
	char			*picName;
	int				picShader;
	int				picWidth;
	int				picHeight;
	int				picX;
	int				picY;
	int				textX;
	int				textY;
	int				textEnum;		// Label on pic
	int				textcolor;		// Normal color
	int				textcolor2;		// Highlight color

	float range;

	qboolean		mouseDown;
} menuslider_s;

typedef	struct {
	int		left;
	int		right;
	int		up;
	int		down;

	int		xOffset; //offsets in case the calculated box will exceed the darn screen lol
	int		yOffset; //Stored here, so they may be applied to the text as well
} drawList_t;

#define MAX_LIST 30

typedef struct
{
	menucommon_s generic;

	int	oldvalue;
	int curvalue;
	int	numitems;
	int	top;
		
	const char		**itemnames;
	int				*listnames;
	int				*listshaders;

	int width;
	int height;
	int	columns;
	int	seperation;
	int				color;			// Normal color
	int				color2;			// Highlight color
	int				textEnum;
	int				textX;
	int				textY;
	int				textFlags;
	int				textcolor;		// Normal color
	int				textcolor2;		// Highlight color
	byte			updated;		// 1 if changed
	int				focusWidth;		// 
	int				focusHeight;	// 
	int				listX;
	int				listY;
	int				listFlags;
	int				listcolor;
	int				maxChars;		//TiM - max number of characters it can display

	//TiM - draw list params
	drawList_t		drawList;
	qboolean		ignoreList;		//maybe don't show the list :P
} menulist_s;

typedef struct
{
	menucommon_s generic;
	int				color;	// Normal color
	int				color2;	// Highlight color
	int				color3;	// Greyed color
	int				textEnum;
	int				textEnum2;	// If there's a second line of text
	int				textX;
	int				textY;
	int				textcolor;	// Normal color
	int				textcolor2;	// Highlight color
	int				textcolor3;	// Grayed color
	int				width;
	int				height;
	byte			updated;	// 1 if changed
} menuaction_s;

typedef struct
{
	menucommon_s generic;
	int curvalue;
} menuradiobutton_s;

typedef struct
{
	menucommon_s	generic;
	char*			focuspic;	
	char*			errorpic;
	qhandle_t		shader;
	qhandle_t		focusshader;

	int				focusX;
	int				focusY;
	int				focusWidth;
	int				focusHeight;

	int				width;
	int				height;
	float*			focuscolor;

	int				color;	// Normal color
	int				color2;	// Highlight color

	char			*textPtr;	//	In case an enum doesn't work
	int				textEnum;
	int				textEnum2;	// If there's a second line of text
	int				textX;
	int				textY;
	int				textcolor;	// Normal color
	int				textcolor2;	// Highlight color
	int				textStyle;	// Style of text (UI_SMALLFONT)

} menubitmap_s;

typedef struct
{
	menucommon_s	generic;
	char*			string;			// A normal string
	int				normaltextEnum;	// Enum from normal menu text
	int				buttontextEnum;	// Enum from button text
	int				normaltextEnum2; // Enum from normal menu text
	int				buttontextEnum2; // Enum from button text
	int				normaltextEnum3; // Enum from normal menu text
	int				buttontextEnum3; // Enum from button text
	int				style;
	float			*color;	// Normal color
	float			*color2;	// Highlight color
	int				focusX;
	int				focusY;
	int				focusWidth;
	int				focusHeight;
} menutext_s;

typedef struct
{
	int				type;		// STRING or GRAPHIC
	float			timer;		// When it changes
	int				x;			// X position
	int				y;			// Y positon
	int				width;		// Graphic width
	int				height;		// Graphic height
	const char		*file;		// File name of graphic/ text if STRING
	int				normaltextEnum;	//	Text comes from Menu_Normal_text
	qhandle_t		graphic;	// Handle of graphic if GRAPHIC
	int				min;		// 
	int				max;
	int				target;		// Final value
	int				inc;
	int				style;
	int				color;		// Normal color
	void			*pointer;		// To an address
} menugraphics_s;

extern void Mouse_Hide(void);
extern void Mouse_Show(void);

extern void			UI_HolomatchInMenu_Cache (void);
extern void			Menu_Cache( void );
extern void			Menu_Focus( menucommon_s *m );
extern void			Menu_AddItem( menuframework_s *menu, void *item );
extern void			Menu_AdjustCursor( menuframework_s *menu, int dir );
extern void			Menu_Draw( menuframework_s *menu );
extern void			*Menu_ItemAtCursor( menuframework_s *m );
extern sfxHandle_t	Menu_ActivateItem( menuframework_s *s, menucommon_s* item );
extern void			Menu_SetCursor( menuframework_s *s, int cursor );
extern void			Menu_SetCursorToItem( menuframework_s *m, void* ptr );
extern sfxHandle_t	Menu_DefaultKey( menuframework_s *s, int key );
extern void			Bitmap_Init( menubitmap_s *b );
extern void			Bitmap_Draw( menubitmap_s *b );
extern void			ScrollList_Draw( menulist_s *l );
extern sfxHandle_t	ScrollList_Key( menulist_s *l, int key );
extern sfxHandle_t	menu_in_sound;
extern sfxHandle_t	menu_move_sound;
extern sfxHandle_t	menu_out_sound;
extern sfxHandle_t	menu_buzz_sound;
extern sfxHandle_t	menu_null_sound;
extern sfxHandle_t	menu_rpgx_theme;				//J2J
extern vec4_t		menu_text_color;
extern vec4_t		menu_grayed_color;
extern vec4_t		menu_dark_color;
extern vec4_t		menu_highlight_color;
extern vec4_t		menu_red_color;
extern vec4_t		menu_black_color;
extern vec4_t		menu_dim_color;
extern vec4_t		color_white;
extern vec4_t		color_yellow;
extern vec4_t		color_blue;
extern vec4_t		color_orange;
extern vec4_t		color_red;
extern vec4_t		color_dim;
extern vec4_t		name_color;
extern vec4_t		list_color;
extern vec4_t		listbar_color;
extern vec4_t		text_color_disabled; 
extern vec4_t		text_color_normal;
extern vec4_t		text_color_highlight;

extern char	*ui_medalNames[];
extern char	*ui_medalPicNames[];
extern char	*ui_medalSounds[];

//
// ui_mfield.c
//
extern void			MField_Clear( mfield_t *edit );
extern void			MField_KeyDownEvent( mfield_t *edit, int key );
extern void			MField_CharEvent( mfield_t *edit, int ch );
extern void			MField_Draw( mfield_t *edit, int x, int y, int style, vec4_t color,int cursor );
extern void			MenuField_Init( menufield_s* m );
extern void			MenuField_Draw( menufield_s *f );
extern sfxHandle_t	MenuField_Key( menufield_s* m, int* key );

//
// ui_menu.c
//
extern void MainMenu_Cache( void );
extern void UI_MainMenu(void);
extern void UI_RegisterCvars( void );
extern void UI_UpdateCvars( void );
extern void UI_Setup_MenuButtons(void);
extern void UI_QuitMenu( void );
extern void UI_ChooseServerTypeMenu( void ); 
extern void UI_QuitMenu_Cache(void); 

//
// ui_demo2.c
//
extern void UI_DemosMenu_Cache(void);

//
// ui_ingame.c
//
extern int			ingameFlag;	// true when ingame menu is in use

extern void InGame_Cache( void );
extern void UI_InGameMenu(void);

//
// ui_confirm.c
//
extern void ConfirmMenu_Cache( void );
extern void UI_ConfirmMenu( const char *question, void (*draw)( void ), void (*action)( qboolean result ) );

//
// ui_team.c
//
//extern void UI_TeamMainMenu( void );
//extern void TeamMain_Cache( void );

//
// ui_connect.c
//
extern void UI_DrawConnectScreen( qboolean overlay );

//
// ui_controls2.c
//
extern void Controls_Cache( void );
extern void UI_ControlsMouseJoyStickMenu_Cache(void);

void SetupMenu_TopButtons(menuframework_s *menu,int menuType,menuaction_s *s_video_apply_action);
#define MENU_CONTROLS	1
#define MENU_VIDEO		2
#define MENU_SOUND		3
#define MENU_GAME		4
#define MENU_CDKEY		5
#define MENU_PLAYER		6
#define MENU_FONTS		7
#define MENU_DEFAULT				8
#define MENU_CONTROLS_WEAPON		9
#define MENU_CONTROLS_LOOK			10
#define MENU_CONTROLS_MOVEMENT		11
#define MENU_CONTROLS_COMMAND		12
#define	MENU_CONTROLS_MODELVIEW		17 //RPG-X TiM
#define MENU_CONTROLS_MOUSE			13
#define MENU_CONTROLS_OTHER			14
#define MENU_NETWORK				15
#define MENU_VIDEODATA				16

//
// ui_motd.c
//
extern void UI_MotdMenu( void );
extern void UI_MotdMenu_Cache( void );
extern void MotdReceiveLine( const char *txt );
extern void MotdReset( void );

//
// ui_holodeck.c
//
extern void HoloDataReceived(const char *data);



//
// ui_admin.c
//
extern void UI_AdminMenu( qboolean fromConsole );
extern void UI_AdminMenu_Cache( void );

//
// ui_msd.c
//
extern void UI_msdMenu(int maxhull, int currhull, int maxshield, int currshield, int shieldstate, int warpstate, int turbostate, int transstate, int alertstate, char *model);

//
// ui_transporter.c
//
extern void UI_TransporterMenu(int trNum);
extern void TransDataReceived(const char *data);

//
// ui_login.c
//
/* nothing for now */

//
// ui_demo2.c
//
extern void UI_DemosMenu( void );

//
// ui_mods.c
//
extern void UI_ModsMenu( void );
extern void UI_ModsMenu_Cache( void );

//
// ui_credits.c
//
extern void UI_CreditsMenu( void );
extern void UI_CreditsMenu_Cache( void );

//
//ui_library.c
//
//extern void UI_LibraryMenu( void );
//extern void UI_LibraryMenu_Cache( void );
//extern void UI_AccessingMenu( void );
//extern void UI_AccessingMenu_Cache (void);

//
// ui_mods.c
//
extern void UI_ModsMenu( void );
extern void UI_ModsMenu_Cache( void );

// ui_emotes.c
extern void UI_EmotesMenu ( qboolean fromConsole );
extern void UI_PlayerEmotes_Cache( void );

//
// ui_cdkey.c
//
extern void UI_CDKeyMenu( void );
extern void UI_CDKeyMenu_Cache( void );
extern void UI_CDKeyMenu_f( void );
extern void UI_CDKeyMenu2( void );

//
// ui_playermodel.c
//
extern void UI_PlayerModelMenu(int menuFrom);
extern void PlayerModel_Cache( void );

//
// ui_playersettings.c
//
extern void UI_PlayerSettingsMenu(int menuFrom);
extern void PlayerSettings_Cache( void );

#define PS_MENU_CONTROLS		0
#define PS_MENU_SINGLEPLAYER	1
#define PS_MENU_SERVER			2

//
// ui_preferences.c
//
//extern void UI_PreferencesMenu( void );
//extern void Preferences_Cache( void );
extern void UI_GameOptionsMenu( void );
extern void UI_GameOptionsMenu_Cache(void);
extern int s_textlanguage_Names[];
extern int s_voicelanguage_Names[];

//
// ui_specifyserver.c
//
extern void UI_SpecifyServerMenu( void );
extern void SpecifyServer_Cache( void );

//
// ui_servers2.c
//
#define MAX_FAVORITESERVERS 32

extern void UI_ArenaServersMenu( void );
extern void ArenaServers_Cache( void );
extern void UI_ChooseServerTypeMenu_Cache( void );

//
// ui_startserver.c
//
extern void UI_StartServerMenu( qboolean multiplayer );
extern void StartServer_Cache( void );
extern void ServerOptions_Cache( void );
extern void UI_BotSelectMenu( char *bot );
extern void UI_BotSelectMenu_Cache( void );

//
// ui_serverinfo.c
//
extern void UI_ServerInfoMenu( void );
extern void ServerInfo_Cache( void );

// ui_turbolift.c

extern void UI_TurboliftMenu ( int liftNum );
extern void UI_TurboliftMenu_Cache (void);

//
// ui_video.c
//
//extern void GraphicsOptions_Cache( void );
extern void DriverInfo_Cache( void );
extern void UI_VideoDataMenu( void );
extern void UI_VideoDataMenu_Cache(void);
extern void UI_VideoData2Menu_Cache(void);
extern void UI_VideoDriverMenu_Cache(void);

//
// ui_sql.c
//
extern void UI_sqlMenu(void);

//
// ui_players.c
//

//FIXME ripped from cg_local.h
typedef struct {
	int			oldFrame;
	int			oldFrameTime;		// time when ->oldFrame was exactly on

	int			frame;
	int			frameTime;			// time when ->frame will be exactly on

	float		backlerp;

	float		yawAngle;
	qboolean	yawing;
	float		pitchAngle;
	qboolean	pitching;

	int			animationNumber;	// may include ANIM_TOGGLEBIT
	animation_t	*animation;
	int			animationTime;		// time when the first frame of the animation will be exact
} lerpFrame_t;

//TiM : Model system relevent parameters
#define MAX_BOLTONS		10
#define MAX_TALK_SKINS	4

typedef enum {
	BOLTON_HEAD = 0,
	BOLTON_TORSO,
	BOLTON_LEGS,
	BOLTON_MAX
} boltonLoc_t;

//min and max value for any timed events
typedef struct {
	int nextTime;

	int minSeconds;
	int maxSeconds;
} charSecs_t;

//bolton info
typedef struct {
	int			modelBase;
	char		tagName[MAX_QPATH];
	qhandle_t	tagModel;
} boltonTags_t;

typedef struct {
	// model info
	char			modelName[MAX_QPATH];

	gender_t		gender;

	float			height;
	float			weight;

	qhandle_t		legsModel;
	qhandle_t		legsSkin;
	lerpFrame_t		legs;

	qhandle_t		torsoModel;
	qhandle_t		torsoSkin;
	lerpFrame_t		torso;

	qhandle_t		headModel;
	qhandle_t		headSkin;
	
	//TiM : model system data
	qhandle_t		headSkinBlink;
	qhandle_t		headSkinTalk[MAX_TALK_SKINS];
	boltonTags_t	boltonTags[MAX_BOLTONS];
	qboolean		hasRanks;
	charSecs_t		headBlinkTime;
	int				nextTalkTime;
	qhandle_t		currentTalkSkin;

	qboolean		upperEmoting;
	qboolean		lowerEmoting; //set to qtrue when performing a non-loop emote. 
	//so as to let the code know to cancel it when the timer ends

	char			race[256];

	animation_t		animations[MAX_ANIMATIONS];

	qhandle_t		weaponModel;
	qhandle_t		barrelModel;
	qhandle_t		flashModel;
	vec3_t			flashDlightColor;
	int				muzzleFlashTime;

	// currently in use drawing parms
	vec3_t			viewAngles;
	vec3_t			moveAngles;
	weapon_t		currentWeapon;
	int				legsAnim;
	int				torsoAnim;

	// animation vars
	weapon_t		weapon;
	weapon_t		lastWeapon;
	weapon_t		pendingWeapon;
	int				weaponTimer;
	int				pendingLegsAnim;
	int				torsoAnimationTimer;

	int				pendingTorsoAnim;
	int				legsAnimationTimer;

	qboolean		chat;
	qboolean		newModel;

	qboolean		barrelSpinning;
	float			barrelAngle;
	int				barrelTime;

	//TiM - Just to give this thing a little spice. :)
	qboolean		randomEmote;	//if qtrue, player will play STAND1_* random gestures every minute or so
	int				nextEmoteTime;

	int				upperLoopEmote;	//Model just finished emoting, so put them into the new loop animation
	int				lowerLoopEmote;

	int				realWeapon;
} playerInfo_t;

void UI_DrawPlayer( float x, float y, float w, float h, vec3_t pOrigin, playerInfo_t *pi, int time ); //RPG-X : TiM-  origin
void UI_PlayerInfo_SetModel( playerInfo_t *pi, const char *model );
void UI_PlayerInfo_SetInfo( playerInfo_t *pi, int legsAnim, int torsoAnim, vec3_t viewAngles, vec3_t moveAngles, weapon_t weaponNum, float height, float weight, qboolean chat );
qboolean UI_RegisterClientModelname( playerInfo_t *pi, const char *modelSkinName );

//
// ui_atoms.c
//
void UI_LanguageFilename(char *baseName,char *baseExtension,char *finalName);

//typedef char string[128];

typedef struct {
	int					frametime;
	int					realtime;

	//RPG-X : TiM
	int					cursorpx;
	qboolean			spinView;
	int					lastYaw;

	qboolean			playCinematic;

	//Ranks Data
	rankSets_t			rankSet;					//Main Storage for all the current rankset data
	int					currentRank;	
	// /RPG-X

	//RPG-X Class Data
	classData_t			classData[MAX_CLASSES];
	char				classList[MAX_CLASSSETS][MAX_NAMELENGTH];
	char				classSetName[36];

	int					cursorx;
	int					cursory;
	int					menusp;
	menuframework_s*	activemenu;
	menuframework_s*	stack[MAX_MENUDEPTH];
	glconfig_t			glconfig;
	qboolean			debug;
	qhandle_t			whiteShader;
	qhandle_t			menuBackShader;
	qhandle_t			charset;
	qhandle_t			charsetPropTiny;
	qhandle_t			charsetPropBig;
	qhandle_t			charsetProp;
//	qhandle_t			charsetPropGlow;
	qboolean			cursorDraw;
	qhandle_t			charsetPropB;
	qhandle_t			cursor;
	qhandle_t			rb_on;
	qhandle_t			rb_off;
	float				scalex;
	float				scaley;
//	float				bias;
	qboolean			demoversion;
	qboolean			firstdraw;

	//widescreen data
	widescreen_t		widescreen;

	// Common Menu Graphics
	qhandle_t			smallNumbers[10];
	qhandle_t			graphicButtonLeftEnd;			// Rounded left button end
	qhandle_t			graphicCircle;					// Solid circle
	qhandle_t			graphicBracket1CornerLU;		// Thin Left side upper corner
	qhandle_t			corner_12_18;
	qhandle_t			halfroundr_22;
	qhandle_t			graphicButtonLeft;

	sfxHandle_t			menu_choice1_snd;
	sfxHandle_t			menu_datadisp1_snd;
	sfxHandle_t			menu_datadisp2_snd;
	sfxHandle_t			menu_datadisp3_snd;

} uiStatic_t;

extern void			UI_LoadMenuText(void);
extern void			UI_LoadButtonText(void);
extern void			UI_InitSpinControl(menulist_s *spincontrol);
extern void			UI_Init( void );
extern void			UI_Shutdown( void );
extern void			UI_KeyEvent( int key );
extern void			UI_MouseEvent( int dx, int dy );
extern void			UI_Refresh( int realtime );
extern qboolean		UI_ConsoleCommand( void );
extern float		UI_ClampCvar( float min, float max, float value );
extern void			UI_DrawNamedPic( float x, float y, float width, float height, const char *picname );
extern void			UI_DrawHandlePic( float x, float y, float w, float h, qhandle_t hShader ); 
extern void UI_DrawHandleStretchPic( float x, float y, float w, float h, float s0, float t0, float s1, float t1, qhandle_t hShader );
extern void			UI_FillRect( float x, float y, float width, float height, const float *color );
extern void			UI_DrawRect( float x, float y, float width, float height, const float *color );
//extern void			UI_LerpColor(vec4_t a, vec4_t b, vec4_t c, float t);
extern void			UI_DrawBannerString( int x, int y, const char* str, int style, vec4_t color );
extern float		UI_ProportionalSizeScale( int style );
extern void			UI_DrawProportionalString( int x, int y, const char* str, int style, vec4_t color );
extern int			UI_ProportionalStringWidth( const char* str,int style );
extern void			UI_DrawString( int x, int y, const char* str, int style, vec4_t color, qboolean highRes );
extern void			UI_DrawChar( int x, int y, int ch, int style, vec4_t color );
extern qboolean 	UI_CursorInRect (int x, int y, int width, int height);
extern void			UI_AdjustFrom640( float *x, float *y, float *w, float *h );

//extern void			UI_DrawTextBox (int x, int y, int width, int lines);
extern qboolean		UI_IsFullscreen( void );
extern void			UI_SetActiveMenu( uiMenuCommand_t menu );
extern void			UI_PushMenu ( menuframework_s *menu );
extern void			UI_PopMenu (void);
extern void			UI_ForceMenuOff (void);
extern char			*UI_Argv( int arg );
extern char			*UI_Cvar_VariableString( const char *var_name );
extern void			UI_Refresh( int time );
extern void			UI_KeyEvent( int key );
//extern int			UI_RandomNumbers(int max);
extern void			UI_PrecacheMenuGraphics(menugraphics_s *menuGraphics,int maxI);
extern void			UI_MenuFrame(menuframework_s *menu);
extern void			UI_MenuFrame2(menuframework_s *menu);

extern int			UI_PopulateRanksArray( char* ranks[] ); //RPG-X: TiM
extern int			UI_PopulateRankSetArray( char *rankSets[] );
extern void			UI_InitRanksData( char* ranksName );
extern int			UI_LoadClassString( void );
extern int			UI_PopulateClassSetArray( char *classSets[] );
extern int			UI_PopulateClassArray( char *classes[] );
extern int 			UI_InitClassData( char* fileName );

extern qboolean		m_entersound;
extern uiStatic_t	uis;

//
// ui_spLevel.c
//
void UI_SPLevelMenu_Cache( void );
void UI_SPLevelMenu( void );
void UI_SPLevelMenu_f( void );
void UI_SPLevelMenu_ReInit( void );
extern void UI_ResetGameMenu_Cache(void);

//
// ui_spArena.c
//
void UI_SPArena_Start( const char *arenaInfo );

//
// ui_spPostgame.c
//
void UI_SPPostgameMenu_Cache( void );
void UI_SPPostgameMenu_f( void );

//
// ui_spSkill.c
//
void UI_SPSkillMenu( const char *arenaInfo );
void UI_SPSkillMenu_Cache( void );

//
// ui_syscalls.c
//
void			trap_Print( const char *string );
void			trap_Error( const char *string );
int				trap_Milliseconds( void );
void			trap_Cvar_Register( vmCvar_t *vmCvar, const char *varName, const char *defaultValue, int flags );
void			trap_Cvar_Update( vmCvar_t *vmCvar );
void			trap_Cvar_Set( const char *var_name, const char *value );
float			trap_Cvar_VariableValue( const char *var_name );
void			trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void			trap_Cvar_SetValue( const char *var_name, float value );
void			trap_Cvar_Reset( const char *name );
void			trap_Cvar_Create( const char *var_name, const char *var_value, int flags );
void			trap_Cvar_InfoStringBuffer( int bit, char *buffer, int bufsize );
int				trap_Argc( void );
void			trap_Argv( int n, char *buffer, int bufferLength );
void			trap_Cmd_ExecuteText( int exec_when, const char *text );	// don't use EXEC_NOW!
int				trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode );
void			trap_FS_Read( void *buffer, int len, fileHandle_t f );
void			trap_FS_Write( const void *buffer, int len, fileHandle_t f );
void			trap_FS_FCloseFile( fileHandle_t f );
int				trap_FS_GetFileList(  const char *path, const char *extension, char *listbuf, int bufsize );
qhandle_t		trap_R_RegisterModel( const char *name );
qhandle_t		trap_R_RegisterSkin( const char *name );
qhandle_t		trap_R_RegisterShaderNoMip( const char *name );
void			trap_R_ClearScene( void );
void			trap_R_AddRefEntityToScene( const refEntity_t *re );
void			trap_R_AddPolyToScene( qhandle_t hShader , int numVerts, const polyVert_t *verts );
void			trap_R_AddLightToScene( const vec3_t org, float intensity, float r, float g, float b );
void			trap_R_RenderScene( const refdef_t *fd );
void			trap_R_SetColor( const float *rgba );
void			trap_R_DrawStretchPic( float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader );
void			trap_UpdateScreen( void );
void			trap_CM_LerpTag( orientation_t *tag, clipHandle_t mod, int startFrame, int endFrame, float frac, const char *tagName );
void			trap_S_StartLocalSound( sfxHandle_t sfx, int channelNum );
sfxHandle_t		trap_S_RegisterSound( const char *sample );
void			trap_Key_KeynumToStringBuf( int keynum, char *buf, int buflen );
void			trap_Key_GetBindingBuf( int keynum, char *buf, int buflen );
void			trap_Key_SetBinding( int keynum, const char *binding );
qboolean		trap_Key_IsDown( int keynum );
qboolean		trap_Key_GetOverstrikeMode( void );
void			trap_Key_SetOverstrikeMode( qboolean state );
void			trap_Key_ClearStates( void );
int				trap_Key_GetCatcher( void );
void			trap_Key_SetCatcher( int catcher );
void			trap_GetClipboardData( char *buf, int bufsize );
void			trap_GetClientState( uiClientState_t *state );
void			trap_GetGlconfig( glconfig_t *glconfig );
int				trap_GetConfigString( int index, char* buff, int buffsize );
int				trap_LAN_GetLocalServerCount( void );
void			trap_LAN_GetLocalServerAddressString( int n, char *buf, int buflen );
int				trap_LAN_GetGlobalServerCount( void );
void			trap_LAN_GetGlobalServerAddressString( int n, char *buf, int buflen );
int				trap_LAN_GetPingQueueCount( void );
void			trap_LAN_ClearPing( int n );
void			trap_LAN_GetPing( int n, char *buf, int buflen, int *pingtime );
void			trap_LAN_GetPingInfo( int n, char *buf, int buflen );
int				trap_MemoryRemaining( void );
qboolean		trap_SetCDKey( char *buf );
void trap_R_ModelBounds( clipHandle_t model, vec3_t mins, vec3_t maxs );

//
// ui_addbots.c
//
void UI_AddBots_Cache( void );
void UI_AddBotsMenu( void );

//
// ui_removebots.c
//
void UI_RemoveBots_Cache( void );
void UI_RemoveBotsMenu( void );

//
// ui_teamorders.c
//
extern void UI_TeamOrdersMenu( int fromMenu );
extern void UI_TeamOrdersMenu_f( void );
extern void UI_TeamOrdersMenu_Cache( void );

//
// ui_display.c
//
//void UI_DisplayOptionsMenu_Cache( void );
void UI_DisplayOptionsMenu( void );

//
// ui_sound.c
//
//void UI_SoundOptionsMenu_Cache( void );
//void UI_SoundOptionsMenu( void );
extern float setup_menubuttons[8][2];
void UI_SoundMenu( void);
void UI_SoundMenu_Cache(void); 

//
// ui_fonts.c
//
void UI_FontsMenu( void); 

//
// ui_cvars.c
//
extern void GammaCallback( void *s, int notification );
extern void ScreensizeCallback( void *s, int notification );

//
// ui_network.c
//
void UI_NetworkOptionsMenu_Cache( void );
void UI_NetworkOptionsMenu( void );

//
// ui_gameinfo.c
//
typedef enum {
	AWARD_EFFICIENCY,		// Accuracy
	AWARD_SHARPSHOOTER,		// Most compression rifle frags
	AWARD_UNTOUCHABLE,		// Perfect (no deaths)
	AWARD_LOGISTICS,		// Most pickups
	AWARD_TACTICIAN,		// Kills with all weapons
	AWARD_DEMOLITIONIST,	// Most explosive damage kills
	AWARD_STREAK,			// Ace/Expert/Master/Champion
	AWARD_TEAM,				// MVP/Defender/Warrior/Carrier/Interceptor/Bravery
	AWARD_SECTION31,		// All-around god
	AWARD_MAX
} awardType_t;

const char *UI_GetArenaInfoByNumber( int num );
const char *UI_GetArenaInfoByMap( const char *map );
const char *UI_GetSpecialArenaInfo( const char *tag );
int UI_GetNumArenas( void );
int UI_GetNumSPArenas( void );
int UI_GetNumSPTiers( void );

void UI_ServerAdvancedOptions(int fromMenu);

char *UI_GetBotInfoByNumber( int num );
char *UI_GetBotInfoByName( const char *name );
int UI_GetNumBots( void );

void UI_GetBestScore( int level, int *score, int *skill );
void UI_SetBestScore( int level, int score );
int UI_TierCompleted( int levelWon );
qboolean UI_ShowTierVideo( int tier );
qboolean UI_CanShowTierVideo( int tier );
int  UI_GetCurrentGame( int curLevel );
void UI_NewGame( void );
void UI_LogAwardData( int award, int data );
int UI_GetAwardLevel( int award );

void UI_InitGameinfo( void );


void UI_MenuBottomLineEnd_Graphics (const char *string,int color, qboolean *space );
void UI_PrintMenuGraphics(menugraphics_s *menuGraphics,int maxI);

//
// ui_cvars.c
//
extern void InvertMouseCallback(void *s, int notification);
extern void MouseSpeedCallback( void *s, int notification );
extern void SmoothMouseCallback( void *s, int notification );
extern void AnisotropicFilteringCallback( void *s, int notification );
extern void JoyXButtonCallback( void *s, int notification );
extern void JoyYButtonCallback( void *s, int notification );

//
// ui_admin.c -- additional
//
typedef struct {
	int		id;
	char	name[36];
} clientData_t;

//Generic data, used in more than one menu
typedef struct {
	char				locList[MAX_LOCATIONS][32];
	char				*locListPtr[MAX_LOCATIONS+1]; //ghey....
	int					numLocs;	

	clientData_t		charList[MAX_CLIENTS];
	char				*charListPtr[MAX_CLIENTS+1]; //really ghey
	int					numChars;

	char*				giveItems[25];

} adminGeneric_t;

#endif
