// Copyright (C) 1999-2000 Id Software, Inc.
//
// bg_public.h -- definitions shared by both the server game and client game modules

// meh somehow preprocessor G_LUA won't work for me
#define G_LUA 1
#define CG_LUA 1

// because games can change separately from the main system version, we need a
// second version that must match between game and cgame
#define RPGX_VERSION		"RPG-X v2.2 wc18072012"
#define RPGX_COMPILEDATE	"20/05/11"
#define RPGX_COMPILEDBY		"GSIO01"
#define GAME_VERSION		"RPG-X v" RPGX_VERSION

#define INJURED_MODE_HEALTH		20	//!<For the injured mode health system.

#define	DEFAULT_GRAVITY		800
#define	GIB_HEALTH			-40//RPG-X: RedTechie - If rpg_medicsrevive is off use this
#define	GIB_HEALTH_IMPOSSIBLE			-100000//RPG-X: RedTechie - If rpg_medicsrevive is on use this
#define	ARMOR_PROTECTION	1.0//0.66
#define PIERCED_ARMOR_PROTECTION		0.50 //!< trek: shields only stop 50% of armor-piercing dmg

#define	MAX_ITEMS			256

#define	RANK_TIED_FLAG		0x4000

#define	ITEM_RADIUS			15		//!< item sizes are needed for client side pickup detection

#define	LIGHTNING_RANGE		768

#define	SCORE_NOT_PRESENT	-9999	//!< for the CS_SCORES[12] when only one player is present

#define	VOTE_TIME			30000

#define	MINS_Z				-24
#define	DEFAULT_VIEWHEIGHT	34 // TIM - 26//32
#define CROUCH_VIEWHEIGHT	12
#define	DEAD_VIEWHEIGHT		-16

//TiM : New Rank Definitions
#define	MAX_RANKS			128 //32 - see how this goes

#define	NUM_CVAR_STORES		16	//If we ever change the number of CVARS, we can change it here easily

//scannable entities
#define MAX_SCANNABLES		256
#define MAX_ENTSCANNABLES	256

/** \struct rankNames_t
*	Struct that hold rank names.
*/
typedef struct {
	char	consoleName[MAX_QPATH]; //!< short name for console
	char	formalName[MAX_QPATH];	//!< long name 
} rankNames_t;

#define	RANKSET_DEFAULT	"rpgx_default"	//!< name of the default rankset

//TiM : Class script info
#define MAX_CLASSES		32				//!< the maximum ammount of classes
#define	CLASS_DEFAULT	"rpgx_default"	//!< the default classset

//RPG-X: Marcin: I'm lazy sorry
#define WARNING(x) \
do { \
	G_LogPrintf x; \
	trap_SendServerCommand( -1, va("print \"%s\"", va x)); \
} while ( 0 )

//RPG-X: Marcin: DEBUG STUFF - 06/12/2008
#ifdef  MY_DEBUG
#define DPRINTF(x) WARNING x
#else
#define DPRINTF(x) // nothing
#endif

//TiM - Used globally for hitbox definition
#define		DEFAULT_MINS_0	-12
#define		DEFAULT_MINS_1	-12
#define		DEFAULT_MINS_2	-24

#define		DEFAULT_MAXS_0	12
#define		DEFAULT_MAXS_1	12
#define		DEFAULT_MAXS_2	32

#define	DEFAULT_CHAR		"munro"
#define DEFAULT_MODEL		"main"
#define DEFAULT_SKIN		"default"

#define DEFAULT_PLAYER		"munro/main/default"

//TiM - Turbolift Decks
#define MAX_DECKS	16

/**	\struct widescreen_t
*	variables used for widescreen drawing, used in both UI and CG
*
*	\author Ubergames - TiM
*/
typedef struct
{
	float				ratio;				//!<multiplied against x/w to decrease w/h ratio to 1:1
	int					state;				//!<specific rendering state (so we can disable the effect for certain elements)
	int					bias;				//!<shifts every element as if the entire screen was being shifted (for centering purposes)	
} widescreen_t;

/**	\enum widescreen_e
*	flags dictating what to do to an element that's being distorted by fullscreen mode
*
*	\author Ubergames - TiM
*/
typedef enum
{
	WIDESCREEN_NONE = 0,			//!<draw as normal
	WIDESCREEN_LEFT,				//!<Keeps it aligned to the left side of the screen
	WIDESCREEN_RIGHT,				//!<offsets it so it fits a 4:3 perspective in the middle of the screen
	WIDESCREEN_CENTER				//!<offsets it, so it's within a 4:3 perspective on the right side of the screen
} widescreen_e;

//
// config strings are a general means of communicating variable length strings
// from the server to all connected clients.
//

// CS_SERVERINFO and CS_SYSTEMINFO are defined in q_shared.h
#define	CS_MUSIC			2
#define	CS_MESSAGE			3		//!< from the map worldspawn's message field
#define	CS_MOTD				4		//!< g_motd string for server message of the day
#define	CS_WARMUP			5		//!< server time when the match will be restarted
#define	CS_SCORES1			6
#define	CS_SCORES2			7
#define CS_VOTE_TIME		8
#define CS_VOTE_STRING		9
#define	CS_VOTE_YES			10
#define	CS_VOTE_NO			11
#define	CS_GAME_VERSION		12
#define	CS_LEVEL_START_TIME	13		//!< so the timer only shows the current level
#define	CS_INTERMISSION		14		//!< when 1, fraglimit/timelimit has been hit and intermission will start in a second or two
#define CS_FLAGSTATUS		15		//!< string indicating flag status in CTF
#define CS_RED_GROUP		16		//!< used to send down what the group the red team is
#define CS_BLUE_GROUP		17		//!< used to send down what the group the blue team is

#define	CS_ITEMS			27		//!< string of 0's and 1's that tell which items are present
//TiM: Ooooh! a gap between 27 + 32! Exploitationism!
#define CS_CAMERA_SHAKE		28		//!<holds the intensity + duration for camera shakes.
									//I'm using a CS string, so if a client joins mid-shake, they'll start shaking too
#define CS_CON_FAIL			29		//!<If a connection fails - Display this message
#define CS_CLASS_DATA		30		//!<Class data passed to the client from the server (to allow players not requiring class data locally)
#define CS_TURBOLIFT_DATA	31
#define CS_SHADERSTATE		32 		//!< used for shader remapping
#define	CS_MODELS			33
#define	CS_SOUNDS			(CS_MODELS+MAX_MODELS)
#define	CS_PLAYERS			(CS_SOUNDS+MAX_SOUNDS)
#define CS_LOCATIONS		(CS_PLAYERS+MAX_CLIENTS)

//TiM: RPG-X Tricorder scan-able func_usables
#define CS_TRIC_STRINGS		(CS_LOCATIONS+MAX_LOCATIONS)
//TiM: RPG-X Data for decoy spawnage
#define CS_DECOYS			(CS_TRIC_STRINGS+MAX_TRIC_STRINGS)

#define CS_MAX				(CS_DECOYS+MAX_DECOYS)
//#define CS_MAX				(CS_LOCATIONS+MAX_LOCATIONS) //TiM : Old

//TiM: Current String count is 896. 128 left O_o
//GSIO: Current String count is 897 for rpgxEF. 3199 left for rpgxEF O_o :D

#if (CS_MAX) > MAX_CONFIGSTRINGS
#error overflow: (CS_MAX) > MAX_CONFIGSTRINGS
#endif

typedef enum {
	GT_FFA,				//!< free for all
	GT_TOURNAMENT,		//!< one on one tournament
	GT_SINGLE_PLAYER,	//!< single player tournament

	//-- team games go after this --

	GT_TEAM,			//!< team deathmatch
	GT_CTF,				//!< capture the flag

	GT_MAX_GAME_TYPE
} gametype_t;

typedef enum { GENDER_MALE, GENDER_FEMALE, GENDER_NEUTER } gender_t;

//TiM - Global variables for the player weight/height system
#define BASE_HEIGHT		185.0
#define BASE_WEIGHT		90.0

#define FEMALE_OFFSET	0.73

#define MAX_HEIGHT		1.15
#define MIN_HEIGHT		0.9

#define MAX_WEIGHT		1.1
#define MIN_WEIGHT		0.9

#define HEIGHT_UNIT		"cm"
#define WEIGHT_UNIT		"kg"

/*
===================================================================================

PMOVE MODULE

The pmove code takes a player_state_t and a usercmd_t and generates a new player_state_t
and some other output data.  Used for local prediction on the client game and true
movement on the server game.
===================================================================================
*/

/**	\enum pmtype_t
*
*/
typedef enum {
	PM_NORMAL,		//!< can accelerate and turn
	PM_NOCLIP,		//!< noclip movement
	PM_SPECTATOR,	//!< still run into walls
	PM_DEAD,		//!< no acceleration or turning, but free falling
	PM_FREEZE,		//!< stuck in place with no control
	PM_INTERMISSION,//!< no movement or status bar
	PM_CCAM			//!< cinematic cam mode
} pmtype_t;

/** \enum weaponstate_t
*
*/
typedef enum {
	WEAPON_READY,
	WEAPON_RAISING,
	WEAPON_DROPPING,
	WEAPON_FIRING
} weaponstate_t;

// pmove->pm_flags

#define	PMF_DUCKED			1 //1 //2
#define	PMF_JUMP_HELD		2 //2 //4
#define	PMF_BACKWARDS_JUMP	8		//!< go into backwards land
#define	PMF_BACKWARDS_RUN	16		//!< coast down to backwards run
#define	PMF_TIME_LAND		32		//!< pm_time is time before rejump
#define	PMF_TIME_KNOCKBACK	64		//!< pm_time is an air-accelerate only time
#define	PMF_TIME_WATERJUMP	256		//!< pm_time is waterjump
#define	PMF_RESPAWNED		512		//!< clear after attack and jump buttons come up
#define	PMF_USE_ITEM_HELD	1024
#define PMF_FOLLOW			4096	//!< spectate following another player
#define PMF_SCOREBOARD		8192	//!< spectate as a scoreboard

#define ANIM_ONLADDER		16384
#define ANIM_DIDFLY			32768
#define	ANIM_ALERT			128		//TiM: How was this missed!?
#define ANIM_ALERT2			2048	//TiM: This makes it full I think

#define	PMF_ALL_TIMES	(PMF_TIME_WATERJUMP|PMF_TIME_LAND|PMF_TIME_KNOCKBACK)

#define	MAXTOUCH	32

/**
*	\struct pmove_t
*/
typedef struct {
	// state (in / out)
	playerState_t	*ps;

	// command (in)
	usercmd_t	cmd;
	int			tracemask;			//!< collide against these types of surfaces
	int			debugLevel;			//!< if set, diagnostic output will be printed
	qboolean	noFootsteps;		//!< if the game is setup for no footsteps by the server
	qboolean	pModDisintegration;		//!< true if the Disintegration playerMod is on

	// results (out)
	int			numtouch;
	int			touchents[MAXTOUCH];

	int			useEvent;

	vec3_t		mins, maxs;			//!< bounding box size

	int			watertype;
	int			waterlevel;

	float		xyspeed;
	float		xyzspeed; //TiM : in case body is falling as well

	qboolean	admin;
	qboolean	medic;
	qboolean	borg;

	// callbacks to test the world
	// these will be different functions during game and cgame
	void		(*trace)( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask );
	int			(*pointcontents)( const vec3_t point, int passEntityNum );

} pmove_t;

// if a full pmove isn't done on the client, you can just update the angles
void PM_UpdateViewAngles( playerState_t *ps, const usercmd_t *cmd );
void Pmove (pmove_t *pmove);

//===================================================================================


// player_state->stats[] indexes
//
// maximum of MAX_STATS...currently 16
//TiM: Ooohhh! Usable space!
/** \enum statIndex_t
*	Each of these array cells can store data up to 2^16 bits of data!
*	This is good in the fact that we are capable of using this place to store data
*	that is larger for its previous cells, for example, animation data.
*/
typedef enum {
	STAT_HEALTH,
	STAT_HOLDABLE_ITEM,
	STAT_WEAPONS,					//!< 16 bit fields
	STAT_ARMOR,
	STAT_DEAD_YAW,					//!< look this direction when dead (FIXME: get rid of?)
	STAT_CLIENTS_READY,				//!< bit mask of clients wishing to exit the intermission (FIXME: configstring?)
	STAT_MAX_HEALTH,				//!< health / armor limit, changable by handicap
	STAT_USEABLE_PLACED,			//!< have we placed the detpack yet?

	//TiM : Placeholder for emotes data and anim holding
	TORSOTIMER,						//!<Time index for torso model animations (usable for up to 65 seconds worth of anim data)
	LEGSTIMER,						//!<Time index for legs animations

	LEGSANIM,						//!<Index of current legs animation to play
	TORSOANIM,						//!<Index of current legs animation to play

	EMOTES							//!<Flags stored for emote conditions
} statIndex_t;

// player_state->persistant[] indexes
/** \enum persEnum_t
*	these fields are the only part of player_state that isn't
*	cleared on respawn
*/
typedef enum {
	PERS_SCORE,						//!< !!! MUST NOT CHANGE, SERVER AND GAME BOTH REFERENCE !!!
	PERS_HITS,						//!< total points damage inflicted so damage beeps can sound on change
	PERS_SHIELDS,					//!< total shield points damage inflicted so damage beeps can sound on change
	PERS_RANK,
	PERS_TEAM,
	PERS_SPAWN_COUNT,				//!< incremented every respawn
	PERS_REWARD_COUNT,				//!< incremented for each reward sound
	PERS_REWARD,					//!< a reward_t
	PERS_ATTACKER,					//!< clientnum of last damage inflicter
	PERS_KILLED,					//!< count of the number of times you died
	PERS_CLASS,
} persEnum_t;


// entityState_t->eFlags
// IMPORTANT:  You need to check entityStateFields in msg.c to be certain that enough bits are being sent!

//TiM : ioEF has defined it so 24 bits is the norm... but in reg EF, it's 27
//either way, confirm there's absolutely no more than 27 (24 if we can)

#define	EF_DEAD				0x00000001		//!< don't draw a foe marker over players with EF_DEAD
#define EF_ITEMPLACEHOLDER	0x00000002		//!< faded items
#define	EF_TELEPORT_BIT		0x00000004		//!< toggled every time the origin abruptly changes
#define	EF_BOUNCE			0x00000008		//!< for missiles
#define	EF_BOUNCE_HALF		0x00000010		//!< for missiles
#define EF_MISSILE_STICK	0x00000020		//!< missiles that stick to the wall.
#define	EF_NODRAW			0x00000040		//!< may have an event, but no model (unspawned items)
#define	EF_FIRING			0x00000080		//!< for lightning gun
#define EF_ALT_FIRING		0x00000100		//!< for alt-fires, mostly for lightning guns though
#define	EF_CONNECTION		0x00000200		//!< draw a connection trouble sprite
#define	EF_TALK				0x00000400		//!< draw a talk balloon
#define	EF_SHIELD_BOX_X		0x00000800		//!< tells the client to use special client-collision info, x-axis aligned
#define EF_VOTED			0x00001000		//!< already cast a vote
#define EF_ANIM_ALLFAST		0x00002000		//!< automatically cycle through all frames at 10hz
//RPG-X Flags
#define EF_TALKING			0x00004000		//!< Player is talking
#define	EF_CLAMP_BODY		0x00008000		//!< clamps player in place. moving cancels the emote, looking around rotates head (applies to legs only)
#define	EF_CLAMP_ALL		0x00010000		//!< clamp whole body
#define EF_EYES_SHUT		0x00020000		//!< player toggled their char's eye's shut.
#define EF_EYES_ANGRY		0x00040000		//!< player toggled pissed off char face.
#define EF_FULL_ROTATE		0x00080000		
#define EF_ANIM_ONCE		0x00100000		//!< cycle through all frames just once then stop
#define	EF_TURBOLIFT		0x00200000		//!< ent is in a turbolift now, so gets special rotation
#define EF_SHIELD_BOX_Y		0x00400000		//!< tells the client to use special client-collision info, y-axis aligned
#define	EF_MOVER_STOP		0x00800000		//!< will push otherwise - 67108864

//===================================================================================
//TiM : Emote system definitions
//===================================================================================

//anim list
#include "../cgame/cg_anims.h"

//body flags
#define	EMOTE_UPPER				1								//!<Emote affects torso model only
#define EMOTE_LOWER				2								//!<Emote affects	legs model only
#define	EMOTE_BOTH				( EMOTE_UPPER | EMOTE_LOWER )	//!<Emote affects both parts

//Emote specific flags
#define	EMOTE_CLAMP_BODY		4								//!<Emote clamps player angles, but leaves head free swinging
#define	EMOTE_CLAMP_ALL			8								//!<Emote clamps the whole player. period.

//Infinite loop this anim
#define	EMOTE_LOOP_LOWER		16								//!<Lower emote will loop continuously until manually stopped
#define EMOTE_LOOP_UPPER		32								//!<Upper emote will loop continuosly until stopped.
#define EMOTE_LOOP_BOTH			( EMOTE_LOOP_LOWER | EMOTE_LOOP_UPPER )

//Eye emotes
#define EMOTE_EYES_SHUT			64								//!<Emote plays with player's eyes shut
#define EMOTE_EYES_PISSED		128								//!<Emote plays with player looking not happy.

//Override previous emotes flags
#define EMOTE_OVERRIDE_UPPER	256								//!<If another emote is already playing, this one WILL override the upper section
#define EMOTE_OVERRIDE_LOWER	512								//!<If another emote is already playing, this one WILL override the lower section
#define EMOTE_OVERRIDE_BOTH		( EMOTE_OVERRIDE_UPPER | EMOTE_OVERRIDE_LOWER ) //!<Overrides both parts for the new emote

//Revert to a loop anim when this is finished playing
#define EMOTE_REVERTLOOP_UPPER	1024							//!<If already in a looping anim, when this new one finishes playing, the player will revert back to that anim (ie if u gestured whilst sitting down etc)
#define EMOTE_REVERTLOOP_LOWER	2048
#define EMOTE_REVERTLOOP_BOTH	( EMOTE_REVERTLOOP_UPPER | EMOTE_REVERTLOOP_LOWER )

//Used to clear all emote params except for blinking.
//Kind of hacky, but better than diverting the blinking flag resources elsewhere.  
//We only have so many array cells available
#define EMOTE_MASK_LOWER		( EMOTE_LOWER | EMOTE_CLAMP_BODY | EMOTE_CLAMP_ALL | EMOTE_LOOP_LOWER | EMOTE_OVERRIDE_LOWER | EMOTE_REVERTLOOP_LOWER )
#define EMOTE_MASK_UPPER		( EMOTE_UPPER | EMOTE_LOOP_UPPER | EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER )
#define EMOTE_MASK_BOTH			( EMOTE_MASK_LOWER | EMOTE_MASK_UPPER )

/** \struct emoteList_t
*	Emote data structure
*
*	\author Ubergames
*/
typedef struct {
	char		*name;							//!<name of emote player enters into console
	int			emoteType;						//!<Type of emote (for menu display)
	int			enumName;						//!<eNum value of the emote
	int			enumLoop;						//!<eNum value for loop anim

	int			viewHeight;						//!<Specific viewheight the player will be looking at when this emote is active. (ie, sitting down, you'd be lower than standing up)
	int			hitBoxHeight;

	int			bodyFlags;						//!<flags relating to whether this emote is upper or lower or both
	int			animFlags;						//!<Specific flags related to emote handling
} emoteList_t;

/**	\enum emoteTypes_t
*	Different type numbers for sorting in the UI etc
*
*	\author Ubergames
*/
typedef enum {
	TYPE_NONE = -1,
	TYPE_SITTING = 0,
	TYPE_CONSOLE = 1,
	TYPE_GESTURE = 2,
	TYPE_FULLBODY = 3,
	TYPE_INJURED = 4,
	TYPE_MISC = 5

} emoteTypes_t;

extern emoteList_t	bg_emoteList[];

//=====================================================================================

/**
* The different powerups	
*/
typedef enum {
	PW_NONE,

	PW_QUAD,
	PW_BOLTON, //TiM - Used to be PW_BOLTON
	PW_HASTE, //Used to be PW_HASTE
	PW_INVIS,
	PW_LASER, //TiM - Beam Out Powerup used to be PW_LASER
	PW_FLIGHT,
	PW_FLASHLIGHT,

 	PW_BORG_ADAPT,
	PW_OUCH,

	PW_DISINTEGRATE,
	PW_GHOST,

	PW_EXPLODE,
	PW_ARCWELD_DISINT,
	PW_BEAM_OUT, //TiM - Beam in used to be PW_BEAMING

	PW_EVOSUIT, //RPG-X | Phenix | 8/8/2004
	
	PW_NUM_POWERUPS
} powerup_t;

//=============================
//TiM - Struct to hold data about all the items 
//admins can give.  Point being so the data can be shared
//in the UI and actual command

/** \enum giveType_t
*	Enum depciting what type it is
*/
typedef enum {
	TYPE_ALL,
	TYPE_HEALTH,
	TYPE_WEAPONS,
	TYPE_AMMO,
	TYPE_HOLDABLE,
	TYPE_SHIELD,
	TYPE_TRANSPORTER,
	TYPE_WEAPON,
	TYPE_POWERUP,

	TYPE_MAX
} giveType_t;

/** \struct giveItem_t
*	Struct to hold data about all the items 
*	admins can give.  Point being so the data can be shared
*	in the UI and actual command
*/
typedef struct {
	char	*consoleName;		//!<item you gotz to plug into the console for this shizzie
	int		giveType;			//!<eNum depicting which type this val is
	int		giveValue;			//!<items such as weapons+holdables reference to here
} giveItem_t;

extern giveItem_t	bg_giveItem[];

//=============================

typedef enum {
	HI_NONE=0,		 //!< keep this first enum entry equal to 0 so cgs.useableModels will work properly

	HI_TRANSPORTER=1,
	HI_MEDKIT,
	HI_DETPACK,
	HI_SHIELD,
	HI_DECOY,		// cdr

	HI_NUM_HOLDABLE
} holdable_t;

//TiM: GAH! I've had it!  I've gone through and renamed the weapons to what they're
//called in RPG-X.  Having to remember which is what is starting to wear out my sanity lol. :P
typedef enum {
	WP_0, // WP_NULL				//	1

	WP_1, // WP_NULL_HAND			//	2	// 3/10/00 kef -- used to be WP_RAILGUN						// 1/1/06 TiM: used to be WP_1 

	WP_2, // WP_TRICORDER			//	4
	WP_3, // WP_PADD 				//	8
	WP_4, // WP_COFFEE			//	16	// 3/13/00 kef -- used to be WP_PLASMAGUN					// 1/1/06 TiM: used to be WP_4 

	WP_5, // WP_PHASER				//	32	// 3/13/00 kef -- used to be WP_LIGHTNING
	WP_6, // WP_COMPRESSION_RIFLE	//	64	// 3/13/00 kef -- added
	WP_7, // WP_TR116				//	128	// 3/13/00 kef -- added										// 1/1/06 TiM: used to be WP_7

	WP_8, // WP_GRENADE_LAUNCHER	//	256	// 3/10/00 kef -- used to be...heh...WP_8	// TiM: Hahaha I get it! hehe ;P
	WP_9, // WP_QUANTUM_BURST		//	512	// 3/13/00 kef -- added
	WP_10, // WP_DISRUPTOR			//	1024	// 3/13/00 kef -- used to be WP_ROCKET_LAUNCHER			// 1/1/06 TiM: used to be WP_10 

	WP_11, // WP_MEDKIT				//	2048	// 10/12/00 jtd -- added for MP patch/gold - merges single player projectile and taser		// 1/1/06 TiM: used to be WP_11
	WP_12, // WP_VOYAGER_HYPO		//	4096	// 10/6/00 mcg -- added for MP patch/gold
	WP_13, // WP_DERMAL_REGEN		//	8192	// 3/13/00 kef -- added									// 1/1/06 TiM: used to be WP_13
	
	WP_14, // WP_TOOLKIT				//	16384	// 10/12/00 jtd -- added for MP patch/gold					// 1/1/06 TiM: used to be WP_14
	WP_15, // WP_HYPERSPANNER				//	32768														// 1/1/06 TiM: used to be WP_NEUTRINO_PROBE

	WP_NUM_WEAPONS
} weapon_t;

// reward sounds
typedef enum {
	REWARD_BAD,

	REWARD_IMPRESSIVE,		//!< One shot kill with Compression
	REWARD_EXCELLENT,		//!< Two frags in a row
	REWARD_DENIED,			//!< Near powerup, stolen
	REWARD_FIRST_STRIKE,	//!< First blood in a level
	REWARD_STREAK			//!< Ace/Expert/Master/Champion
} reward_t;

#define STREAK_ACE			5
#define STREAK_EXPERT		10
#define STREAK_MASTER		15
#define STREAK_CHAMPION		20


// entityState_t->event values
// entity events are for effects that take place reletive
// to an existing entities origin.  Very network efficient.

// two bits at the top of the entityState->event field
// will be incremented with each change in the event so
// that an identical event started twice in a row can
// be distinguished.  And off the value with ~EV_EVENT_BITS
// to retrieve the actual event number
#define	EV_EVENT_BIT1		0x00000100
#define	EV_EVENT_BIT2		0x00000200
#define	EV_EVENT_BITS		(EV_EVENT_BIT1|EV_EVENT_BIT2)

typedef enum {
	EV_NONE,

	EV_FOOTSTEP,
	EV_FOOTSTEP_METAL,
	EV_FOOTSPLASH,
	EV_FOOTWADE,
	EV_SWIM,

	EV_STEP_4,
	EV_STEP_8,
	EV_STEP_12,
	EV_STEP_16,

	EV_FALL_SHORT,
	EV_FALL_MEDIUM,
	EV_FALL_FAR,

	EV_JUMP_PAD,			//!< boing sound at origin, jump sound on player

	EV_JUMP,
	EV_WATER_TOUCH,	//!< foot touches
	EV_WATER_LEAVE,	//!< foot leaves
	EV_WATER_UNDER,	//!< head touches
	EV_WATER_CLEAR,	//!< head leaves

	EV_ITEM_PICKUP,			//!< normal item pickups are predictable
	EV_GLOBAL_ITEM_PICKUP,	//!< powerup / team sounds are broadcast to everyone

	EV_NOAMMO,
	EV_NOAMMO_ALT,
	EV_CHANGE_WEAPON,
	EV_FIRE_WEAPON,
	EV_ALT_FIRE,
	EV_FIRE_EMPTY_PHASER,

//BOOKMARK (J2J)
	EV_USE_ITEM0,
	EV_USE_ITEM1,
	EV_USE_ITEM2,
	EV_USE_ITEM3,
	EV_USE_ITEM4,
	EV_USE_ITEM5,
	EV_USE_ITEM6,
	EV_USE_ITEM7,
	EV_USE_ITEM8,
	EV_USE_ITEM9,
	EV_USE_ITEM10,
	EV_USE_ITEM11,
	EV_USE_ITEM12,
	EV_USE_ITEM13,
	EV_USE_ITEM14,
	EV_USE_ITEM15,

	EV_ITEM_RESPAWN,
	EV_ITEM_POP,
	EV_PLAYER_TELEPORT_IN,
	EV_PLAYER_TELEPORT_OUT,

	//RPG-X: TiM
	EV_PLAYER_TRANSPORT_IN,
	EV_PLAYER_TRANSPORT_OUT,

	EV_GRENADE_BOUNCE,		//!< eventParm will be the soundindex
	EV_MISSILE_STICK,		//!< eventParm will be the soundindex

	EV_GENERAL_SOUND,
	EV_GLOBAL_SOUND,		//!< no attenuation
	EV_TEAM_SOUND,

	EV_MISSILE_HIT,
	EV_MISSILE_MISS,

	EV_PAIN,
	EV_DEATH1,
	EV_DEATH2,
	EV_DEATH3,
	EV_OBITUARY,

	EV_POWERUP_BATTLESUIT,
	EV_POWERUP_REGEN,
	EV_POWERUP_SEEKER_FIRE,

	EV_DEBUG_LINE,
	EV_TAUNT,

	//
	// kef -- begin Trek stuff
	//

	// kef -- taken directly from Trek code
	EV_COMPRESSION_RIFLE,
	EV_COMPRESSION_RIFLE_ALT,

	EV_IMOD,
	EV_IMOD_HIT,
	EV_IMOD_ALTFIRE,
	EV_IMOD_ALTFIRE_HIT,

	EV_STASIS,

	EV_GRENADE_EXPLODE,
	EV_GRENADE_SHRAPNEL_EXPLODE,
	EV_GRENADE_SHRAPNEL,

	EV_DREADNOUGHT_MISS,

	EV_TETRION,

	EV_SHIELD_HIT,

	EV_FX_SPARK,
	EV_FX_STEAM,
	EV_FX_BOLT,
	EV_FX_TRANSPORTER_PAD,
	EV_FX_DRIP,

	EV_FX_GARDEN_FOUNTAIN_SPURT, //RPG-X - TiM
	EV_FX_SURFACE_EXPLOSION,	 //RPG-X - TiM
	EV_FX_SMOKE,				 //RPG-X - TiM
	EV_FX_ELECTRICAL_EXPLOSION,	 //RPG-X - TiM
	EV_FX_FIRE,					 //RPG-X - Marcin			
	EV_FX_SHAKE,				 //RPG-X - Marcin
	
	// Additional ports from SP by Harry Young
	// You forgot the , ... I added them for you - GSIO

	EV_FX_COOKING_STEAM,
	EV_FX_ELECFIRE,
	EV_FX_FORGE_BOLT,
	EV_FX_PLASMA,
	EV_FX_STREAM,
	EV_FX_TRANSPORTER_STREAM,
	EV_FX_EXPLOSION_TRAIL,
	EV_FX_BORG_ENERGY_BEAM,
	EV_FX_SHIMMERY_THING,
	EV_FX_BORG_BOLT,

	EV_SCREENFX_TRANSPORTER,

	EV_DISINTEGRATION,
	EV_EXPLODESHELL,
	EV_ARCWELD_DISINT,

	EV_DETPACK,

	EV_DISINTEGRATION2,

	//
	// expansion pack
	//
	EV_OBJECTIVE_COMPLETE,
	EV_USE,
	EV_BORG_ALT_WEAPON,// TASER
	EV_BORG_TELEPORT,
	EV_FX_CHUNKS,
	//RPG-X: RedTechie - Hypo puff and fx gun explo and shake sound and tr-116 tris
	EV_HYPO_PUFF,
	EV_EFFECTGUN_SHOOT,
	EV_SHAKE_SOUND,
	EV_TR116_TRIS,
	//RPG-X: TiM - Lensflare stuff
	EV_Q_FLASH,
	//SplatFX,
	EV_SPLAT,
	//RPG-X | GSIO01 | 08/95/2009 adaptsound
	EV_ADAPT_SOUND,
	EV_FX_PHASER,
	EV_FX_DISRUPTOR,
	EV_SET_CLOAK,
	EV_FX_TORPEDO,
	EV_FOOTSTEP_GRASS,
	EV_FOOTSTEP_GRAVEL,
	EV_FOOTSTEP_SNOW,
	EV_FOOTSTEP_WOOD,
	EV_FALL_SHORT_GRASS,
	EV_FALL_SHORT_GRAVEL,
	EV_FALL_SHORT_SNOW,
	EV_FALL_SHORT_WOOD,
	EV_FALL_MEDIUM_GRASS,
	EV_FALL_MEDIUM_GRAVEL,
	EV_FALL_MEDIUM_SNOW,
	EV_FALL_MEDIUM_WOOD,
	EV_FALL_FAR_GRASS,
	EV_FALL_FAR_GRAVEL,
	EV_FALL_FAR_SNOW,
	EV_FALL_FAR_WOOD,
	EV_FX_PARTICLEFIRE,
	EV_SHOOTER_SOUND,
	EV_TRIGGER_SHOW,
	EV_SCRIPT_SOUND,
	EV_CAMMODE_TOGGLE,

	EV_LASERTURRET_AIM,
	EV_LASERTURRET_FIRE,

	EV_STASIS_DOOR_OPENING,
	EV_STASIS_DOOR_CLOSING,

	EV_DEBUG_TRACE,

	EV_SELFDESTRUCT_SETTER,
	EV_HULLHEALTH_SETTER,
	EV_SHIELDHEALTH_SETTER
} entity_event_t;

//RPG-X: J2J - Moved animation enum list so that the string list can see it, Note special case for cg_players.c.
//#include "../cgame/cg_anims.h"

typedef struct animation_s {
	int		firstFrame;
	int		numFrames;
	int		loopFrames;			//!< 0 to numFrames
	int		frameLerp;			//!< msec between frames
	int		initialLerp;		//!< msec to get to first frame
} animation_t;

// flip the togglebit every time an animation
// changes so a restart of the same anim can be detected

//RPG-X: Jason - Last bit in a 2 byte integer is now the flag. (essentailly all we need as 2 byes allows values up to 65536)
#define		ANIM_TOGGLEBIT		1024

typedef enum {
	TEAM_FREE,
	TEAM_RED,
	TEAM_BLUE,
	TEAM_SPECTATOR,

	TEAM_NUM_TEAMS
} team_t;

enum {
	RETURN_FLAG_SOUND,
	SCORED_FLAG_SOUND,
	DROPPED_FLAG_SOUND,
	SCORED_FLAG_NO_VOICE_SOUND,
	MAX_TEAM_SOUNDS
};

// Time between location updates
#define TEAM_LOCATION_UPDATE_TIME		1000

// How many players on the overlay
// TiM - Increasing this greatly may not be good...
#define TEAM_MAXOVERLAY		MAX_CLIENTS//8

#define LOCATION_NONE		0x00000000

// Height layers
#define LOCATION_HEAD		0x00000001 // [F,B,L,R] Top of head
#define LOCATION_FACE		0x00000002 // [F] Face [B,L,R] Head
#define LOCATION_SHOULDER	0x00000004 // [L,R] Shoulder [F] Throat, [B] Neck
#define LOCATION_CHEST		0x00000008 // [F] Chest [B] Back [L,R] Arm
#define LOCATION_STOMACH	0x00000010 // [L,R] Sides [F] Stomach [B] Lower Back
#define LOCATION_GROIN		0x00000020 // [F] Groin [B] Butt [L,R] Hip
#define LOCATION_LEG		0x00000040 // [F,B,L,R] Legs
#define LOCATION_FOOT		0x00000080 // [F,B,L,R] Bottom of Feet

// Relative direction strike came from
#define LOCATION_LEFT		0x00000100
#define LOCATION_RIGHT		0x00000200
#define LOCATION_FRONT		0x00000400
#define LOCATION_BACK		0x00000800

// means of death
typedef enum {
	MOD_UNKNOWN,

	MOD_WATER,
	MOD_SLIME,
	MOD_LAVA,
	MOD_CRUSH,
	MOD_TELEFRAG,
	MOD_FALLING,
	MOD_SUICIDE,
	MOD_TARGET_LASER,
	MOD_TRIGGER_HURT,

// Trek weapons
	MOD_PHASER,
	MOD_PHASER_ALT,
	MOD_CRIFLE,
	MOD_CRIFLE_SPLASH,
	MOD_CRIFLE_ALT,
	MOD_CRIFLE_ALT_SPLASH,
	MOD_IMOD,
	MOD_IMOD_ALT,
	MOD_SCAVENGER,
	MOD_SCAVENGER_ALT,
	MOD_SCAVENGER_ALT_SPLASH,
	MOD_STASIS,
	MOD_STASIS_ALT,
	MOD_GRENADE,
	MOD_GRENADE_ALT,
	MOD_GRENADE_SPLASH,
	MOD_GRENADE_ALT_SPLASH,
	MOD_TETRION,
	MOD_TETRION_ALT,
	MOD_DREADNOUGHT,
	MOD_DREADNOUGHT_ALT,
	MOD_QUANTUM,
	MOD_QUANTUM_SPLASH,
	MOD_QUANTUM_ALT,
	MOD_QUANTUM_ALT_SPLASH,

	MOD_DETPACK,
	MOD_SEEKER,

//expansion pack
	MOD_KNOCKOUT,
	MOD_ASSIMILATE,
	MOD_BORG,
	MOD_BORG_ALT,

	MOD_RESPAWN,
	MOD_EXPLOSION,
	MOD_FORCEFIELD,		//RPG-X: RedTechie - Added for forcefield kill
	MOD_FORCEDSUICIDE,	//RPG-X: RedTechie - Added for force kill
	MOD_CUSTOM_DIE,		//RPG-X: TiM - Added for custom die messages

	MOD_MAX
} meansOfDeath_t;

//---------------------------------------------------------

// gitem_t->type
typedef enum {
	IT_BAD,
	IT_WEAPON,				// EFX: rotate + upscale + minlight
	IT_AMMO,				// EFX: rotate
	IT_ARMOR,				// EFX: rotate + minlight
	IT_HEALTH,				// EFX: static external sphere + rotating internal
	IT_POWERUP,				// instant on, timer based
							// EFX: rotate + external ring that rotates
	IT_HOLDABLE,			// single use, holdable item
							// EFX: rotate + bob
	IT_TEAM
} itemType_t;

typedef struct gitem_s {
	char		*classname;	//!< spawning name
	char		*pickup_sound;
	char		*world_model;
	char		*view_model;

	char		*icon;
	char		*pickup_name;	//!< for printing on pickup

	int			quantity;		//!< for ammo how much, or duration of powerup
	itemType_t  giType;			//!< IT_* flags

	int			giTag;

	char		*precaches;		//!< string of all models and images this item will use
	char		*sounds;		//!< string of all sounds this item will use
    //char        paddMsg[512];   // RPG-X: Marcin: I hope this works. - 06/12/2008 - no it does not i'm an idiot
} gitem_t;

// included in both the game dll and the client
extern	gitem_t	bg_itemlist[];
extern	int		bg_numItems;

extern	int		bg_numEmotes; //TiM - added a counter variable since it'd be easier to store then :P
extern	int		bg_numGiveItems;

gitem_t *BG_FindItemWithClassname(const char *name);
char *BG_FindClassnameForHoldable(holdable_t pw);
gitem_t	*BG_FindItem( const char *pickupName/*const char *classname*/ );//RPG-X: RedTechie - Attempting to fix give cmd
gitem_t	*BG_FindItemForWeapon( weapon_t weapon );
gitem_t	*BG_FindItemForAmmo( weapon_t weapon );
gitem_t	*BG_FindItemForPowerup( powerup_t pw );
gitem_t	*BG_FindItemForHoldable( holdable_t pw );
#define	ITEM_INDEX(x) ((x)-bg_itemlist)

qboolean	BG_CanItemBeGrabbed( const entityState_t *ent, const playerState_t *ps, int maxWeap );


// g_dmflags->integer flags
#define	DF_NO_FALLING			8
#define DF_FIXED_FOV			16
#define	DF_NO_FOOTSTEPS			32

// content masks
#define	MASK_ALL				(-1)
#define	MASK_SOLID				(CONTENTS_SOLID)
#define	MASK_PLAYERSOLID		(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_BODY)
#define	MASK_DEADSOLID			(CONTENTS_SOLID|CONTENTS_PLAYERCLIP)
#define	MASK_WATER				(CONTENTS_WATER|CONTENTS_LAVA|CONTENTS_SLIME)
#define	MASK_OPAQUE				(CONTENTS_SOLID|CONTENTS_SLIME|CONTENTS_LAVA)
#define	MASK_SHOT				(CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_CORPSE|CONTENTS_SHOTCLIP)
#define MASK_ONLYPLAYER			(CONTENTS_PLAYERCLIP | CONTENTS_BODY)					//RPG-X: - J2J Added for that transporter effect thingy where it appears next to the target (well not really, but dont tell)
#define MASK_BRUSHES			(CONTENTS_STRUCTURAL|CONTENTS_DETAIL|CONTENTS_TRANSLUCENT) //RPG-X: RedTechie - Added for FX gun for "anal probe" -jason

//
// entityState_t->eType
//

//	VERY IMPORTANT, if you change this enum, you MUST also change the enum in be_aas_entity.c!  THANKS ID!!!!!  WTG! GG!

typedef enum {
	ET_GENERAL,
	ET_PLAYER,
	ET_ITEM,
	ET_MISSILE,
	ET_ALT_MISSILE,
	ET_MOVER,
	ET_BEAM,
	ET_PORTAL,
	ET_SPEAKER,
	ET_PUSH_TRIGGER,
	ET_TELEPORT_TRIGGER,
	ET_INVISIBLE,
	ET_LASER,				//!< lasersight entity type
	ET_USEABLE,
	ET_TRIC_STRING,			//RPG-X: TiM - Tricorder string var
	ET_TURBOLIFT,			//RPG-X: TIM - Spawns a looping turbolift sound
	ET_MOVER_STR,			//RPG-X: GSIO01 - Mover entity that has Tricorder string var

	ET_EVENTS				// any of the EV_* events can be added freestanding
							// by setting eType to ET_EVENTS + eventNum
							// this avoids having to set eFlags and eventNum
} entityType_t;

/*====================
SecurityCode
TiM: Defines and security
structs for the local security
ID system.
=====================*/
#define SECURITY_ID		('7'<<24|'X'<<16|'G'<<8|'R') //RGX7 - Unique ID for the file
#define SECURITY_HASH	0xffffffff	//!<Identifier to validate the hash file against this copy of EF
#define SECURITY_PID	0xffffffff	//!<Main player ID that is sent to the server and checked for validation

//#define SECURITY_SIZE	4+2+4+2+4+2
#define SECURITY_SIZE	4+4+4
#define SECURITY_FILE	"rpgxid.dat"

typedef struct
{
	unsigned long		ID;
	//short				padding;
	unsigned long		hash;
	//short				padding2;
	unsigned long		playerID;
	//short				padding3;
} rpgxSecurityFile_t;

void	BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result );
void	BG_EvaluateTrajectoryDelta( const trajectory_t *tr, int atTime, vec3_t result );

void	BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps );

void	BG_PlayerStateToEntityState( playerState_t *ps, entityState_t *s, qboolean snap );

qboolean	BG_PlayerTouchesItem( playerState_t *ps, entityState_t *item, int atTime );

char	*NextWordEndsHere(char *pos);
char	*EndWord(char *pos);

#define ARENAS_PER_TIER		4
#define MAX_ARENAS			1024
#define	MAX_ARENAS_TEXT		8192

#define MAX_BOTS			1024
#define MAX_BOTS_TEXT		8192

//make this match Max_Ammo in g_items please;
#define PHASER_AMMO_MAX		50

extern int Max_Ammo[];
