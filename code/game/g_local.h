// Copyright (C) 1999-2000 Id Software, Inc.
//
// g_local.h -- local definitions for game module
#ifndef _G_LOCAL_H_
#define _G_LOCAL_H_

#include "../base_game/q_shared.h"
#include "../base_game/bg_public.h"
#include "g_public.h"
#include "../base_game/bg_list.h"

//==================================================================

// the "gameversion" client command will print this plus compile date
//#define	GAMEVERSION	"RPG-X v",RPGX_VERSION
//const char	GAMEVERSION[] = strcat("RPG-X v",RPGX_VERSION);
#define GAMEVERSION			"rpg-x2"

#ifndef BASEPATH
#define BASEPATH			"rpgxEF"
#endif

//#define RPGX_SERECT_PASS	"HRkq1yF22o06Zng9FZXH5sle"	//"��������ƫ�������ŵ���� ���"

#define BODY_QUEUE_SIZE		8

#define GINFINITE			1000000

#define	FRAMETIME			100					// msec
#define	EVENT_VALID_MSEC	300
#define	CARNAGE_REWARD_TIME	3000
#define REWARD_SPRITE_TIME	2000
#define REWARD_STREAK_SPRITE_TIME	5000

#define	INTERMISSION_DELAY_TIME	500

// gentity->flags
#define	FL_GODMODE				0x00000010
#define	FL_NOTARGET				0x00000020
#define	FL_TEAMSLAVE			0x00000400	// not the first on the team
#define FL_NO_KNOCKBACK			0x00000800
#define FL_DROPPED_ITEM			0x00001000
#define FL_NO_BOTS				0x00002000	// spawn point not for bot use
#define FL_NO_HUMANS			0x00004000	// spawn point just for bots
#define FL_CLOAK				0x00010000
#define FL_FLY					0x00020000
#define FL_EVOSUIT				0x00400000	//RPG-X | Phenix | 8/8/2004
#define FL_HOLSTER				0x00800000	//RPG-X | TiM	 | 6/9/2005
#define FL_CLAMPED				0x01000000  //RPG-X | TiM	 | 25/6/2006
#define FL_THROWN_ITEM          0x02000000  //RPG-X | Marcin | 03/12/2008
#define FL_LOCKED				0x04000000	//RPG-X | GSIO01 | 08/05/2009
// CCAM
#define FL_CCAM					0x08000000

//RPG-X Defines ==============================

//TiM : Rank Variables
extern rankNames_t g_rankNames[MAX_RANKS];

//TiM : Weapons table
extern stringID_table_t WeaponTable[];

//Quit, then reconnect system
#define	MAX_RECON_NAMES	32

/** \struct reconData_t
*	\brief Holds data of clients that where connected to the server.
*
*	Holds the IP and previous name of a clients that where connected to the server before.
*
*	\author Ubergames
*/
typedef struct {
	char	ipAddress[64]; //!< The IP adress
	char	previousName[MAX_TOKEN_CHARS]; //!< The previous name
} reconData_t;

/** \struct fxGunData_t
*	\brief Struct needed for FX gun arguments.
*
*	Holds the eventnum and multiple arguments for the fxGun.
*
*	\author Ubergames - TiM
*/
typedef struct {
	int		eventNum; //!< index of the event to play

	//different effects need different args, 
	//so here is a set of generic args
	int		arg_int1; //!< first arg - integer
	int		arg_int2; //!< second arg - integer

	float	arg_float1; //!< first arg - float
	float	arg_float2; //!< second arg - float

	vec3_t	arg_vec1;
} fxGunData_t;

//Struct for the dynamic class system
//Some of the data here is encoded into
//a config string and then sent to the clients
//so that they can render the classes client side
/**	\struct g_classData_t
*	\brief Struct for the dynamic class system
*
*	Some of the data here is encoded into
*	a config string and then sent to the clients
*	so that they can render the classes client side
*
*	\author Ubergames
*/
typedef struct {
	char		consoleName[15];	//!< name of class in console
	char		formalName[25];		//!< name of class in the scoreboard etc
	char		message[MAX_QPATH];	//!< msg when player enters class
	
	int			weaponsFlags;		//!< flags denoting default weapons of this class

	char		modelSkin[36];		//!< Denoting which skin this class is forced to

	int			isAdmin;			//!< class has admin access
	int			isMarine;			//!< class has marine
	int			isMedical;			//!< class has medical capacity
	int			isn00b;				//!< class is a dumbass
	int			isBorg;				//!< class is a borg
} g_classData_t;

//Class Data
extern g_classData_t	g_classData[MAX_CLASSES];

//===========================================

/**	\enum tpType_t
*	\brief Enumeration for diffrent transporter types.
*
*	Used to determine what transporter effect is used by various functions.
*
*	\author Raven Software
*	\author Ubergames - TiM
*	\author	Ubergames - J2J
*/
typedef enum
{
	TP_NORMAL,
	TP_BORG,
	TP_NUM_TP,
	TP_TRI_TP,			/*!< This transporter mode will give a federation transporter FX but not accelerate the player. */
	TP_TURBO			/*!< Used in turbolifts to let the teleporter know to maintain veloctiy. */
} tpType_t;

/**	\enum moverState_t
*	\brief Used to determine in which state a mover is.
*
*	Movers are things like doors, plats, buttons, etc.
*
*/
typedef enum {
	MOVER_POS1,
	MOVER_POS2,
	MOVER_1TO2,
	MOVER_2TO1,

	// VALKYRIE: angle movements
	ROTATOR_POS1,
	ROTATOR_POS2,
	ROTATOR_1TO2,
	ROTATOR_2TO1,

	//advanced mover
	ADV_POS1,
	ADV_POS2,
	ADV_1TO2,
	ADV_2TO1
#ifdef G_LUA
	,
	MOVER_LUA
#endif
} moverState_t;

#define SP_PODIUM_MODEL		"models/mapobjects/podium/podium4.md3"
#define TEAM_PODIUM_MODEL	"models/mapobjects/podium/podium_single.md3"

//============================================================================

typedef enum {
	ENT_FREE = 0,
	ENT_UNKNOWN,
	ENT_TEMPORAL,
	ENT_CUSTOM,
	ENT_LIGHT,
	ENT_PATH_CORNER,
	ENT_PATH_POINT,
	ENT_LASER_ARM,
	ENT_INFO_NULL = 100,
	ENT_INFO_NOTNULL,
	ENT_INFO_CAMP,
	ENT_INFO_PLAYER_START,
	ENT_INFO_PLAYER_INTERMISSION,
	ENT_INFO_FIRSTPLACE,
	ENT_INFO_SECONDPLACE,
	ENT_INFO_THIRDPLACE,
	ENT_INFO_PODIUM,
	ENT_FUNC_PLAT = 200,
	ENT_FUNC_FORCEFIELD,
	ENT_FUNC_STATIC,
	ENT_FUNC_ROTATING,
	ENT_FUNC_BOBBING,
	ENT_FUNC_PENDULUM,
	ENT_FUNC_BUTTON,
	ENT_FUNC_DOOR,
	ENT_FUNC_DOOR_ROTATING,
	ENT_FUNC_TRAIN,
	ENT_FUNC_TIMER,
	ENT_FUNC_USABLE,
	ENT_FUNC_BRUSHMODEL,
	ENT_FUNC_LIGHTCHANGE,
	ENT_FUNC_TARGETMOVER,
	ENT_FUNC_STASIS_DOOR,
	ENT_FUNC_MOVER,
	ENT_FUNC_BREAKABLE,
	ENT_TRIGGER_ALWAYS = 300,
	ENT_TRIGGER_MULTIPLE,
	ENT_TRIGGER_PUSH,
	ENT_TRIGGER_TELEPORT,
	ENT_TRIGGER_HURT,
	ENT_TRIGGER_TRANSPORTER,
	ENT_TRIGGGER_RADIATION,
	ENT_TARGET_REMOVE_POWERUPS = 400,
	ENT_TARGET_GIVE,
	ENT_TARGET_DELAY,
	ENT_TARGET_SPEAKER,
	ENT_TARGET_PRINT,
	ENT_TARGET_LASER,
	ENT_TARGET_CHARACTER,
	ENT_TARGET_TELEPORTER,
	ENT_TARGET_RELAY,
	ENT_TARGET_KILL,
	ENT_TARGET_LOCATION,
	ENT_TARGET_PUSH,
	ENT_TARGET_COUNTER,
	ENT_TARGET_OBJECTIVE,
	ENT_TARGET_BOOLEAN,
	ENT_TARGET_GRAVITY,
	ENT_TARGET_SHAKE,
	ENT_TARGET_EVOSUIT,
	ENT_TARGET_TURBOLIFT,
	ENT_TARGET_DOORLOCK,
	ENT_TARGET_REPAIR,
	ENT_TARGET_ALERT,
	ENT_TARGET_WARP,
	ENT_TARGET_DEACTIVATE,
	ENT_TARGET_SERVERCHANGE,
	ENT_TARGET_LEVELCHANGE,
	ENT_TARGET_SHADERREMAP,
	ENT_TARGET_SELFDESTRUCT,
	ENT_TARGET_ZONE,
	ENT_TARGET_SHIPHEALTH,
	ENT_TARGET_SEQUENCE,
	ENT_TARGET_HOLODECK,
	ENT_MISC_MODEL = 500,
	ENT_MISC_MODEL_BREAKABLE,
	ENT_MISC_PORTAL_CAMERA,
	ENT_MISC_PORTAL_SURFACE,
	ENT_MISC_TURRET,
	ENT_MISC_AMMOSTATION,
	ENT_SHOOTER_ROCKET = 600,
	ENT_SHOOTER_PLASMA,
	ENT_SHOOTER_GRENADE,
	ENT_SHOOTER_TORPEDO,
	ENT_FX_SPARK = 700,
	ENT_FX_STEAM,
	ENT_FX_BOLT,
	ENT_FX_TRANSPORTER,
	ENT_FX_DRIP,
	ENT_FX_FOUNTAIN,
	ENT_FX_SURFACE_EXPLOSION,
	ENT_FX_BLOW_CHUNKS,
	ENT_FX_SMOKE,
	ENT_FX_ELETRICAL_EXPLOSION,
	ENT_FX_PHASER,
	ENT_FX_TORPEDO,
	ENT_FX_PARTICLEFIRE,
	ENT_FX_FIRE,
	ENT_FX_COOKING_STEAM,
	ENT_FX_ELECTRICFIRE,
	ENT_FX_FORGE_BOLT,
	ENT_FX_PLASMA,
	ENT_FX_STREAM,
	ENT_FX_TRANSPORTER_STREAM,
	ENT_FX_EXPLOSION_TRAIL,
	ENT_FX_BORG_ENERGY_BEAM,
	ENT_FX_SHIMMERY_THING,
	ENT_FX_BORG_BOLT,
	ENT_UI_TRANSPORTER = 800,
	ENT_UI_HOLODECK,
	ENT_UI_MSD,
	ENT_CINEMATIC_CAMMERA = 900
} entityTypeNumber_t;

/**	\typedef gentity_t
*
*	Type for \link gentity_s \endlink
*	@see gentity_s
*
*/
typedef struct gentity_s gentity_t;

/** \typedef gclient_t
*
*	Pointer to \link gclient_s \endlink
*
*/
typedef struct gclient_s gclient_t;

/** \struct gentity_s
*
*	The game side representation of entities.
*
*/
struct gentity_s {
	entityState_t	s;				//!< communicated by server to clients
	entityShared_t	r;				//!< shared by both the server system and game

	// DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
	// EXPECTS THE FIELDS IN THAT ORDER!
	//================================

	/*@shared@*/ /*@null@*/ struct gclient_s	*client;	// NULL if not a client

	qboolean	inuse;

	entityTypeNumber_t					type;				//!< Entity type
	/*@shared@*/ /*@null@*/ char*		classname;			//!< set in QuakeEd
	int			spawnflags;			//!< set in QuakeEd

	qboolean	neverFree;			//!< if true, FreeEntity will only unlink bodyque uses this

	int			flags;				//!< FL_* variables

	/*@shared@*/ /*@null@*/ char*		model;				//!< the model or brushmodel the entities uses
	/*@shared@*/ /*@null@*/ char*		model2;			//!< an alternate model
	int			freetime;			//!< level.time when the object was freed

	int			eventTime;			//!< events will be cleared EVENT_VALID_MSEC after set
	qboolean	freeAfterEvent;		//!< free the entity after the event?
	qboolean	unlinkAfterEvent;	//!< unlink the entity after the event?

	qboolean	physicsObject;		//!< if true, it can be pushed by movers and fall off edges
									//!< all game items are physicsObjects,
	float		physicsBounce;		//!< 1.0 = continuous bounce, 0.0 = no bounce
	int			clipmask;			//!< brushes with this content value will be collided again when moving. items and corpses do not collide against players, for instance

	// movers
	moverState_t moverState;		//!< current state of the mover
	int			soundPos1;			//!< soundindex of sound to play when reaching Pos1
	int			sound1to2;			//!< soundindex of sound to play when moving from Pos1 to Pos2
	int			sound2to1;			//!< soundindex of sound to play when moving from Pos2 to Pos1
	int			soundPos2;			//!< soundindex of sound to play when reaching Pos2
	int			soundLoop;			//!< soundindex for looped sounds
	/*@shared@*/ /*@null@*/ gentity_t*	parent;			//!< parent of this entity
	/*@shared@*/ /*@null@*/ gentity_t*	nextTrain;			//!< next path_corner of a func_train
	/*@shared@*/ /*@null@*/ gentity_t*	prevTrain;			//!< previous path_corner of a func_train
	vec3_t		pos1;				//!< start position for binary movers
	vec3_t		pos2;				//!< end position for binary movers
	vec3_t		apos1;				//!< angular start position for movers
	vec3_t		apos2;				//!< angular end position for movers

	/*@shared@*/ /*@null@*/ char*		message;			//!< message for target_print

	int			timestamp;			//!< body queue sinking, etc

	float		angle;				//!< set in editor, -1 = up, -2 = down
	/*@shared@*/ /*@null@*/ char*		target;			//!< target of the entity
	/*@shared@*/ /*@null@*/ char*		paintarget;		//!< target to use if entity takes damage
	/*@shared@*/ /*@null@*/ char*		targetname;		//!< targetname that identyfies the entity
	/*@shared@*/ /*@null@*/ char*		team;				//!< for teamchains
	/*@shared@*/ /*@null@*/ gentity_t*	target_ent;		//!< target of turrets etc

	float		speed;				//!< moving speed etc
	vec3_t		movedir;			//!< moving direction

	int			nextthink;			//!< next level.time the entities think functions gets called
	/*@shared@*/ /*@null@*/ void		(*think)(/*@shared@*/ gentity_t *self);	//!< think function
	/*@shared@*/ /*@null@*/ void		(*reached)(/*@shared@*/ gentity_t *self);	//!< movers call this when hitting endpoint
	/*@shared@*/ /*@null@*/ void		(*blocked)(/*@shared@*/ gentity_t *self, /*@shared@*/ gentity_t *other); //!< movers call this when blocked
	/*@shared@*/ /*@null@*/ void		(*touch)(/*@shared@*/ gentity_t *self, /*@shared@*/ gentity_t *other, /*@shared@*/ trace_t *trace); //!< touch function for triggers
	/*@shared@*/ /*@null@*/ void		(*use)(/*@shared@*/ gentity_t *self, /*@shared@*/ /*@null@*/ gentity_t *other, /*@shared@*/ /*@null@*/ gentity_t *activator); //!< function that gets called if the entity is used
	/*@shared@*/ /*@null@*/ void		(*pain)(/*@shared@*/ gentity_t *self, /*@shared@*/ gentity_t *attacker, int damage); //!< function that gets called if entity gets damged
	/*@shared@*/ /*@null@*/ void		(*die)(/*@shared@*/ gentity_t *self, /*@shared@*/ gentity_t *inflictor, /*@shared@*/ gentity_t *attacker, int damage, int mod); //!< function that gets called if entity dies

	int			pain_debounce_time;
	int			fly_sound_debounce_time;	// wind tunnel
	int			last_move_time;

	int			health;				//!< current health
	int			old_health;			//!< old/max value for health

	qboolean	takedamage;			//!< entity can take damage

	int			damage;				//!< ammount of damge to do
	int			splashDamage;		//!< do splash damage
									//!< quad will increase this without increasing radius
	int			splashRadius;		//!< radius for splash damage
	int			methodOfDeath;
	int			splashMethodOfDeath;//!< method of death to use for splash damage

	int			count;				//!< used for counting, check whether an entity is allowed to use
									//!< and other various things

	/*@shared@*/ /*@null@*/ gentity_t*	lastEnemy;			//!< last enemy of the entity (turrets etc)
	/*@shared@*/ /*@null@*/ gentity_t*	enemy;				//!< current enemy of the entity (turrets etc)
	/*@shared@*/ /*@null@*/ gentity_t*	activator;			//!< entity that activated/used this entity
	/*@shared@*/ /*@null@*/ gentity_t*	teamchain;			//!< next entity in team
	/*@shared@*/ /*@null@*/ gentity_t*	teammaster;		//!< master of the team

	int			watertype;
	int			waterlevel;

	int			noise_index;

	int			voiceChatSquelch;
	int			voiceChatPreviousTime;

	// timing variables
	float		wait;				//!< how long to wait
	float		random;

	/*@shared@*/ /*@null@*/ gitem_t*	item;				//!< for bonus items

	qboolean	botDelayBegin;

	/*@shared@*/ /*@null@*/ char*		swapname;			//RPG-X Modification | Phenix | 13/06/2004
	/*@shared@*/ /*@null@*/ char*		truename;
	/*@shared@*/ /*@null@*/ char*		falsename;
	/*@shared@*/ /*@null@*/ char*		truetarget;
	/*@shared@*/ /*@null@*/ char*		falsetarget;
	qboolean	booleanstate;	

	float		distance;		// VALKYRIE: for rotating doors

	int			n00bCount;	//RPG-X | Phenix | 06/04/2004 | For when people kill

	//RPG-X: TiM : Additional Params for map ents
	/*@shared@*/ /*@null@*/ char*		targetname2;	// GSIO01 | 08/05/2009

	/*@shared@*/ /*@null@*/ gentity_t*	touched;		// GSIO01 | 08/05/2009 repairing breakables
	
	//GSIO01 | 10/05/2009 | ok these are for target_alert:
	/*@shared@*/ /*@null@*/ char*		bluename;
	/*@shared@*/ /*@null@*/ char*		greensound;
	/*@shared@*/ /*@null@*/ char*		yellowsound;
	/*@shared@*/ /*@null@*/ char*		redsound;
	/*@shared@*/ /*@null@*/ char*		bluesound;

	char		*targetShaderName;		//!< shader to remap for shader remapping
	char		*targetShaderNewName;	//!< shader to remap to for shader remapping

	qboolean	tmpEntity;				//!< is this a temporal entity?

#ifdef G_LUA
	// for lua hooks
	// pointers to lua functions
	/*@shared@*/ /*@null@*/ char*		luaTouch;
	/*@shared@*/ /*@null@*/ char*		luaUse;
	/*@shared@*/ /*@null@*/ char*		luaThink;
	/*@shared@*/ /*@null@*/ char*		luaHurt;
	/*@shared@*/ /*@null@*/ char*		luaDie;
	/*@shared@*/ /*@null@*/ char*		luaFree;
	/*@shared@*/ /*@null@*/ char*		luaTrigger;
	/*@shared@*/ /*@null@*/ char*		luaReached;
	/*@shared@*/ /*@null@*/ char*		luaReachedAngular;
	/*@shared@*/ /*@null@*/ char*		luaSpawn;
	/*@shared@*/ /*@null@*/ char*		luaParm1;
	/*@shared@*/ /*@null@*/ char*		luaParm2;
	/*@shared@*/ /*@null@*/ char*		luaParm3;
	/*@shared@*/ /*@null@*/ char*		luaParm4;
	qboolean	luaEntity;
#endif

	vec4_t		startRGBA;
	vec4_t		finalRGBA;
};

/** \enum clientConnected_t
*
*	Various connection states a client can have.
*/
typedef enum {
	CON_DISCONNECTED,	/*!< client is disconnected */
	CON_CONNECTING,		/*!< client is connecting */
	CON_CONNECTED		/*!< client is connected */
} clientConnected_t;

/** \enum spectatorState_t
*
*	Diffrent spectator modes.
*/
typedef enum {
	SPECTATOR_NOT,			/*!< not spectating */
	SPECTATOR_FREE,			/*!< free spectator mode */
	SPECTATOR_FOLLOW,		/*!< follow spectator mode */
	SPECTATOR_SCOREBOARD	/*!< spectator scoreboard */
} spectatorState_t;

/** \enum playerTeamStateState_t
*
*/
typedef enum {
	TEAM_BEGIN,		/*!< Beginning a team game, spawn at base */
	TEAM_ACTIVE		/*!< Now actively playing */
} playerTeamStateState_t;

/**
*	\brief Contains statistic information about players for team games.
*
*	\todo remove me
*
*	For example number of flag captures.
*/
typedef struct {
	playerTeamStateState_t	state;	/*!< players team state */

	int			location;			/*!< current location */

	int			captures;			/*!< number of captured flags */
	int			basedefense;
	int			carrierdefense;
	int			flagrecovery;
	int			fragcarrier;
	int			assists;
	int			frags;
	int			suicides;

	float		lasthurtcarrier;
	float		lastreturnedflag;
	float		flagsince;
	float		lastfraggedcarrier;

	// kef -- oh boy am I abusing this system
	float		lastFireTime;
} playerTeamState_t;

// the auto following clients don't follow a specific client
// number, but instead follow the first two active players
#define	FOLLOW_ACTIVE1	-1
#define	FOLLOW_ACTIVE2	-2

/**
*	\brief client data that stays across multiple levels or tournament restarts
*
*	this is achieved by writing all the data to cvar strings at game shutdown
*	time and reading them back at connection time.  Anything added here
*	MUST be dealt with in G_InitSessionData() / G_ReadSessionData() / G_WriteSessionData()
*/
typedef struct {
	team_t				sessionTeam;		//!< team of the client
	int					sessionClass;		//!< class of the client is in
	int					spectatorTime;		//!< for determining next-in-line to play
	spectatorState_t	spectatorState;		//!< spectator state
	int					spectatorClient;	//!< for chasecam and follow mode
	int					wins;				//!< tournament stat wins
	int					losses;				//!< tournament stat losses
} clientSession_t;

#define	MAX_VOTE_COUNT		3

/**
*	\brief client data that stays across multiple respawns,
*
*	but is cleared on each level change or team change at G_Client_Begin()
*/
typedef struct {
	clientConnected_t	connected;
	usercmd_t			cmd;				// we would lose angles if not persistant
	qboolean			localClient;		// true if "ip" info key is "localhost"
	qboolean			initialSpawn;		// the first spawn should be at a cool location
	qboolean			predictItemPickup;	// based on cg_predictItems userinfo
	char				netname[36];
	int					maxHealth;			// for handicapping
	int					enterTime;			// level.time the client entered the game
	playerTeamState_t	teamState;	// status in teamplay games
	int					voteCount;			// to prevent people from constantly calling votes
	int					kickCount;
	int					suicideCount;
	qboolean			teamInfo;			// send team overlay updates?
	char				ip[64];				// RPG-X - Store this for login comparisons
	float				pms_height;			//player model system, height
} clientPersistant_t;

/** \struct gclient_s
*
*	this structure is cleared on each G_Client_Spawn(),
*	except for 'client->pers' and 'client->sess'
*/
struct gclient_s {
	// ps MUST be the first element, because the server expects it
	playerState_t		ps;				//!< communicated by server to clients

	// the rest of the structure is private to game
	clientPersistant_t	pers;
	clientSession_t		sess;

	qboolean			readyToExit;		//!< wishes to leave the intermission

	qboolean			noclip;

	int					lastCmdTime;		//!< level.time of last usercmd_t, for EF_CONNECTION we can't just use pers.lastCommand.time, because of the g_sycronousclients case
	int					buttons;
	int					oldbuttons;
	int					latched_buttons;

	// sum up damage over an entire frame, so
	// shotgun blasts give a single big kick
	int					damage_armor;		//!< damage absorbed by armor
	int					damage_blood;		//!< damage taken out of health
	int					damage_knockback;	//!< impact damage
	vec3_t				damage_from;		//!< origin for vector calculation
	qboolean			damage_fromWorld;	//!< if true, don't use the damage_from vector

	int					accurateCount;		//!< for "impressive" reward sound
	int					streakCount;		//!< To count up kills without dying.

	//
	int					lasthurt_client;	//!< last client that damaged this client
	int					lasthurt_mod;		//!< type of damage the client did
	int					lasthurt_location;	//!< Where the client was hit.

	// timers
	int					respawnTime;		//!< can respawn when time > this, force after g_forcerespwan
	int					inactivityTime;		//!< kick players when time > this
	qboolean			inactivityWarning;	//!< qtrue if the five seoond warning has been given
	int					rewardTime;			//!< clear the EF_AWARD_IMPRESSIVE, etc when time > this

	int					airOutTime;

	int					lastKillTime;		//!< for multiple kill rewards
	int					nextRespawn;		//!< level.time when player is allowed to use the respawn command/button again

	qboolean			fireHeld;			//!< used for hook
	gentity_t*			hook;				//!< grapple hook if out

	int					switchTeamTime;

	// timeResidual is used to handle events that happen every second
	// like health / armor countdowns and regeneration
	int					timeResidual;
	int					mod;				//!< means of death

	//added for new game mods
	int					teleportTime;				//!< Borg teleporting
	int					classChangeDebounceTime;	//!< next time you can change classes

	//RPG-X Specific
	qboolean			SpecialGrav;					//!<	Added for target_gravity ent
														//	RPG-X: J2J
	qboolean			UpdateScore;					//!< Check if we should update the score or not
														//	RPG-X: RedTechie
	int					AdminFailed;					//!< For failed admin logins.
														//	RPG-X | Phenix | 21/11/2004
	qboolean			LoggedAsAdmin;					//!< For AdminLogin command
														//	RPG-X | Phenix | 21/11/2004 
	qboolean			LoggedAsDeveloper;				//!< For Debuging Admin Commands
														//	RPG-X | Phenix | 21/11/2004
	int					n00bTime;						//!< For when to bring them out of n00b
														//	RPG-X | Phenix | 06/04/2004
	char				origClass[MAX_STRING_CHARS];	//!<	What class they came from
														//	RPG-X | Phenix | 06/04/2004
	qboolean			noAdminChat;					//!< If admins choose not to see admin messages
														//	RPG-X | TiM    | 02/04/2006

	fxGunData_t			fxGunData;						//!< Storage point for info needed for multiple admin gun FX				

    char				lastPaddMsg[256];               //!< Padd buffer
														//	RPG-X | Marcin | 08/12/2008
	qboolean			pressedUse;						//!< To prevent multiple pickups per press
														//	RPG-X | Marcin | 30/12/2008 
	int					fraggerTime;					//!< For rpg_fraggerSpawnDelay
														//	RPG-X | Marcin | 03/01/2009

	// for sql
	char*				userName;						//!< username (not player name) in the sql database
	int					uid;							//!< uiser id of the player in the sql database 

	// CCAM
	vec3_t				origViewAngles;
	vec3_t				origOrigin;
	gentity_t*			cam;

	// for selfdestruct and shiphealth
	int					nokilli;						//!< if 1 this player won't be killed if the ship is killed
	int					myship;							//!< entnum of the shiphealth entity this client currently holds info from

	int					respawnDisabledTime;
};


//
// this structure is cleared as each map is entered
//
#define	MAX_SPAWN_VARS			64
#define	MAX_SPAWN_VARS_CHARS	2048

/** \typedef levelLocation
*	Type for level location.
*	@see levelLocation_s
*
*	\author Ubergames - GSIO01
*/
typedef struct levelLocation_s levelLocation;

/** \typedef levelLocation´_p
*	Pointer to a level location.
*	@see levelLocation
*	@see levelLocation_s
*
*	\author Ubergames - GSIO01
*/
typedef levelLocation levelLocation_p;

/** \struct levelLocation_s
*	Describes a level location.
*
*	Contains a list of origin and angles where the first is the primary location origin and angles.
*	A name which identifies the location like the targetname for entities.
*	The actual description displayed.
*
*	\author Ubergames - GSIO01
*/
struct levelLocation_s {
	list_p	origins;
	list_p	angles;
	char*	name;
	char*	description;
};

/** \typedef srvChangeData_t
*	Type for \link srvChangeData_s \endlink
*
*	\author Ubergames - GSIO01
*/
typedef struct srvChangeData_s srvChangeData_t;

/** \struct srvChangeData_s
*	Contains data needed by traporter UI and transportet entities for serverchage transports.
*
*	\author Ubergames - GSIO01
*/
struct srvChangeData_s {
	char	name[6][MAX_QPATH];	//!< name of the locations
	char	ip[6][MAX_QPATH];	//!< IPs of the servers to connect to
	int		count;				//!< Count of servers
};

/**
*	Contains important informations about the current leve and information needed for game logic.
*/
typedef struct {
	/*@shared@*/ struct gclient_s*	clients;	/*!< level clients */

	/*@shared@*/ struct gentity_s*	gentities;			/*! level gentities */
	int					gentitySize;		/*! size of gentity */
	int					num_entities;		/*!< current number of entities, <= MAX_GENTITIES */

	int					warmupTime;			/*!< restart match at this time */

	fileHandle_t		logFile;					/*! file handle for the log file */

	// store latched cvars here that we want to get at often
	int					maxclients;					/*! maximum number of clients allowed */

	int					framenum;					/*!< number of the frame */
	int					time;						/*!< time since level start in msec */
	int					previousTime;				/*!< so movers can back up when blocked */

	int					startTime;					/*!< level.time the map was started */

	int					message;					/*!< MESSAGE TIME */

	int					teamScores[TEAM_NUM_TEAMS];	/*!< scores for each team */
	int					lastTeamLocationTime;		/*!< last time of client team location update */

	qboolean			newSession;					/*!< don't use any old session data, because we changed gametype */

	qboolean			restarted;					/*!< waiting for a map_restart to fire */

	int					numConnectedClients;		/*!< number of connected clients */
	int					numNonSpectatorClients;		/*!< includes connecting clients */
	int					numPlayingClients;			/*!< connected, non-spectators */
	int					sortedClients[MAX_CLIENTS];	/*!< Clients sorted by score */
	int					follow1, follow2;			/*!< clientNums for auto-follow spectators */

	int					snd_fry;					/*!< sound index for standing in lava */

	qboolean			firstStrike;				/*!< Was the first strike done yet? */

	// voting state
	char				voteString[MAX_STRING_CHARS];
	int					voteTime;					/*!< level.time vote was called */
	int					voteYes;					/*!< Number of yes votes */
	int					voteNo;						/*!< Number of no votes */
	int					numVotingClients;			/*!< Set by CalculateRanks */

	// spawn variables
	qboolean			spawning;						/*!< the G_Spawn*() functions are valid */
	int					numSpawnVars;					/*!< Number of spawn vars */
	char*				spawnVars[MAX_SPAWN_VARS][2];	/*!< key / value pairs */
	int					numSpawnVarChars;					/*!< Number of spawn var chars */
	char				spawnVarChars[MAX_SPAWN_VARS_CHARS];/*!< The spawn var chars */

	// intermission state
	int					intermissionQueued;		/*!< Intermission was qualified, but wait INTERMISSION_DELAY_TIME before actually going there so the last frag can be watched.  Disable future kills during this delay */
	int					intermissiontime;		/*!< Time the intermission was started */
	qboolean			readyToExit;			/*!< Determines whether at least one client wants to exit */
	int					exitTime;				/*!< Exit time */
	vec3_t				intermission_origin;	/*!< Origin of the camera for intermission. Also used for spectator spawns. */
	vec3_t				intermission_angle;		/*!< Angle of the camera for intermission. Also used for spectator spawns. */

	qboolean			locationLinked;			/*!< target_locations get linked */
	/*@shared@*/ /*@null@*/ gentity_t*			locationHead;			/*!< head of the location list */
	int					bodyQueIndex;			/*!< dead bodies */
	/*@shared@*/ gentity_t*			bodyQue[BODY_QUEUE_SIZE]; /*!< body Que */

	int					numObjectives;			/*! Number of level objectives (unused) */

	//RPG-X - Decoy index
	int					decoyIndex;				/*!< 0-128. Counting decoys.  If we do hit the end (omfg), start from the beginning again lol */
	int					numDecks;				/*!< Counts the number of turbolift decks on the map */
	int					borgAdaptHits[WP_NUM_WEAPONS];	/*!< Counts of hits for each weapon for borg adaption */
	// usable things
	//TiM - usables client side text defines
	int					g_scannables[MAX_SCANNABLES];			/*!< the ID is stored here, but the index is what's referenced */
	int					g_entScannables[MAX_ENTSCANNABLES][2];	/*!< first cell is the entity ID, the second is the id of the object in question */
	qboolean			hasScannableFile;						/*!< Most maps probably won't even have one of these */
	qboolean			hasEntScannableFile;					/*!< Only older maps would probably have this */

	int					numBrushEnts;							/*!< number of entities in the level that use brushmodels */

	/*@shared@*/ /*@null@*/ list_p	safezones;								/*!< self destruct safezones list */
	/*@shared@*/ /*@null@*/ list_p	locations;								/*!< level locations list */
	/*@shared@*/ /*@null@*/ list_p	timedMessages;				/*!< timed messages list */
	/*@shared@*/ /*@null@*/ list_iter_p iterTimedMessages;

	// other stuff
	srvChangeData_t		srvChangeData;							/*!< Server change data */

	// override rpg_calcLiftTravelDuration
	int					overrideCalcLiftTravelDuration;			/*!< Indicated whether the level author wants to override rpg_calcLiftTravelDuration */

	qboolean			sqlReady;
} level_locals_t;


//
// g_spawn.c
//

/**
 * \brief Creates a copy of the given string. Allocated on the memory pool.
 *
 * \param string String to copy.
 * \return Copy of the string.
 */
/*@shared@*/ /*@null@*/ char* G_NewString( /*@null@*/ const char* string );

//
// g_utils.c
//
/**
 * \brief Get the sound index for a sound.
 *
 * Get the sound index for a sound.
 *
 * \param name the sound name
 *
 * \return the sounds index
 */
int G_SoundIndex( char* name );

/**
 * \brief Issue a team command.
 *
 * Issue a team command.
 *
 * \param team the team
 * \param cmd the command
 */
void G_TeamCommand( team_t team, char* cmd );

/**
 * \brief Get the tric string index.
 *
 * Get the tric string index.
 *
 * \param name the tric string name
 *
 * \return the tric strings index
 *
 * \author Ubergames - TiM
 */
int G_TricStringIndex( char* name );

/**
 * \brief Kill all that would be inside a new one.
 *
 * Kills all entities that would touch the proposed new positioning
 * of ent.  Ent should be unlinked before calling this!
 * 
 * \param ent the entity
 */
void G_KillBox (gentity_t* ent);

/**
 * \author Ubergames - J2J
 * \brief Push all entities away that are inside a new entity.
 *
 * Basically does teh same as G_KillBox except it will
 * push players and other entities away instead of killing them.
 *
 * \param ent the entity
 *
 * \return was an ent moved?
 */
qboolean G_MoveBox (gentity_t* ent);

/**
 *
 * \brief Finds an entity.
 *
 * Searches all active entities for the next one that holds
 * the matching string at fieldofs (use the FOFS() macro) in the structure.
 * Searches beginning at the entity after from, or the beginning if NULL
 * NULL will be returned if the end of the list is reached.
 *
 * \param from search from this entity on
 * \param fieldofs in which field to look
 * \param match string to match
 *
 * \return an matching entity or NULL
 */
/*@shared@*/ /*@null@*/  gentity_t* G_Find (/*@null@*/ gentity_t* from, size_t fieldofs, const char* match);

/**
 * \brief Pick a target.
 *
 * Selects a random entity from among the targets.
 *
 * \param targetname the targets targetname
 *
 * \return an entity or NULL
 */
/*@shared@*/ /*@null@*/  gentity_t* G_PickTarget (/*@null@*/ char* targetname);

/**
 * \brief Use all of the given entity's targets.
 *
 * Use all of the given entity's targets.
 *
 * \param ent the entity
 * \param activator the initiator of the function call
 */
void G_UseTargets (gentity_t* ent, /*@null@*/ gentity_t* activator);

/**
 * \brief Use all targets of the given entity.
 *
 * Goes through all entities and calls ther use function if their
 * targetname, swapname, truename, falsename, bluename are matching
 * the target. activator should be set the the inflictor of this function
 * call.
 *
 * \param ent the entity
 * \param activator the activator
 * \param target target to match
 */
void G_UseTargets2( /*@shared@*/ gentity_t* ent, /*@shared@*/ /*@null@*/ gentity_t* activator, /*@null@*/ char* target );

/**
 * \brief Converts angles to move directions.
 *
 * The editor only specifies a single value for angles (yaw),
 * but we have special constants to generate an up or down direction.
 * Angles will be cleared, because it is being used to represent a direction
 * instead of an orientation.
 *
 * \param angles the angles
 * \param movedir the movedir
 */
void G_SetMovedir ( vec3_t angles, vec3_t movedir);

/**
 * \brief Init the entity.
 *
 * Inits a given game entity.
 *
 * \param e the entity
 */
void G_InitGentity( gentity_t* ent );
/**
 * \brief Spawns a new entity.
 *
 * Either finds a free entity, or allocates a new one.
 * The slots from 0 to MAX_CLIENTS-1 are always reserved for clients, 
 * and will never be used by anything else.
 * Try to avoid reusing an entity that was recently freed, because it
 * can cause the client to think the entity morphed into something else
 * instead of being removed and recreated, which can cause interpolated
 * angles and bad trails.
 *
 * \return a new entity or NULL
 */
/*@shared@*/ /*@null@*/  gentity_t* G_Spawn (void);

/**
 * \brief Spawn an temporary entity.
 *
 * Spawns an event entity that will be auto-removed
 * The origin will be snapped to save net bandwidth, so care
 * must be taken if the origin is right on a surface (snap towards start vector first)
 *
 * \param origin the origin
 * \param event the event to use for this entity
 *
 * \return the temporary entity
 */
/*@shared@*/ /*@null@*/  gentity_t* G_TempEntity( vec3_t origin, int event );

/**
 * \brief Makes an entity to play a non looping sound.
 *
 * Makes an entity to play a non looping sound.
 *
 * \param ent the entity
 * \param soundIndex the sounds index
 */
void G_Sound( gentity_t* ent, int soundIndex );

/**
 * \brief Free an entity.
 *
 * Marks the entity as free.
 *
 * \param ed entity to free
 */
void G_FreeEntity( gentity_t* ent );

/**
*	Find all trigger entities that ent's current position touches.
*	Spectators will only interact with teleporters.
*/
void G_TouchTriggers (gentity_t* ent);

/**
 * \brief Create a temporary vector.
 *
 * This is just a convenience function
 * for making temporary vectors for function calls
 *
 * \param x x-value
 * \param y y-value
 * \param z z-value
 *
 * \return temporary vector
 */
float* tv(float x, float y, float z);

/**
 * \brief Converts a vector to a string to be printed.
 *
 * This is just a convenience function
 * for printing vectors
 *
 * \param v the vector
 *
 * \return string representation of the vector
 */
char* vtos( const vec3_t v );

/**
 * \brief Get the yaw from a vector.
 *
 * Get the yaw from a vector.
 *
 * \param vec the vector
 *
 * \return the yaw
 */
float vectoyaw( const vec3_t vec );

/**
 * \brief Adds a new Predictable event.
 *
 * Use for non-pmove events that would also be predicted on the
 * client side: jumppads and item pickups
 * Adds an event+parm and twiddles the event counter
 *
 * \param ent the entity
 * \param event the event
 * \param eventParm any parameters for the event
 */
void G_AddPredictableEvent( gentity_t* ent, int event, int eventParm );

/**
 * \brief Add a new event.
 * 
 * Adds an event+parm and twiddles the event counter
 *
 * \param ent the entity
 * \param event the event
 * \param eventParm parameter for the event
 */
void G_AddEvent( gentity_t* ent, int event, int eventParm );

/**
 * \brief Set the Origin of an entity.
 *
 * Sets the pos trajectory for a fixed position
 *
 * \param ent the entity
 * \param origin the new origin
 */
void G_SetOrigin( gentity_t* ent, vec3_t origin );

/**
 * \brief Set the angles of an entity.
 *
 * Sets the pos trajectory for a fixed angular position
 *
 * \param ent the entity
 * \param angles the new angles
 * \author Ubergames - GSIO01
 */
void G_SetAngles( gentity_t* ent, vec3_t anlges ); //RPG-X | GSIO01 | 24.08.2009

/**
 * Get a list of entities in a specified radous around an origin.
 *
 * \param origin Origin to search around.
 * \param radius Radius to serach in.
 * \param ignore List of entities to ignore.
 * \param takeDamage Only return entities matching this value for takeDamage.
 * \param ent_list List to store found entities in.
 * \return Count of entities found.
 */
int32_t G_RadiusList ( vec3_t origin, double radius, list_p ignore, qboolean takeDamage, list_p ent_list);

/**
 *	Get a list of specified entity classes in a specified radius.
 *
 *	\author Ubergames - GSIO01
 *
 *	\param classname class names for searched entity classes
 *	\param count cout of class names in classname
 *	\param origin origin around which entities are searched
 *	\param radius radius to search in
 *	\param ignore entity to ignore
 *	\param ent_list list to store the results
 *	\return count of found entities
 */
int32_t G_RadiusListOfTypes(list_p classnames, vec3_t origin, double radius, list_p ignore, list_p ent_list);

/**
 * Get the neares entity to an origin.
 *
 * \param classname Filter by this classname.
 * \param origin Origin to search around.
 * \param radius Radius to search in.
 * \param ignore List of entities to ignore.
 * \param takeDamage Only return entities that match this value for takeDamage.
 * \return Nearest entity found.
 */
/*@shared@*/ /*@null@*/  gentity_t* G_GetNearestEnt(char* classname, vec3_t origin, double radius, list_p ignore, qboolean takeDamage);

/**
 * Get the nearest player orund an origin.
 *
 * \param origin Origin to search around.
 * \param radius Radius to search in.
 * \param ignore List of entities to ignore.
 * \return Nearest player.
 */
/*@shared@*/ /*@null@*/  gentity_t* G_GetNearestPlayer(vec3_t origin, double radius, list_p ignore );

/**
 * \author Ubergames - GSIO01
 * \brief Get all entities with the specified targetname.
 *
 * Get all entities with the specified targetname.
 *
 * \param targetname the targetname
 * \param entities the result
 *
 * \return number of entities found
 */
int G_GetEntityByTargetname(const char* targetname, list_p entities);

/**
 * \author Ubergames - GSIO01
 * \brief Get all entities with specified target.
 * 
 *  Get all entities matching the specifie target.
 *
 *  \param target target the entities should have
 *  \param entities the result
 *
 *  \return number of matches found
 */
int G_GetEntityByTarget(const char* target, list_p entities);

/**
 * \author Ubergames - GSIO01
 * \brief Get all entities with specified brush model
 *
 * Get all entities matching the specified brush model.
 * Normally this only shoud be one entity.
 *
 * \param bmodel brush model to match
 * \param entities the result
 *
 * \return number of matches found
 */
int G_GetEntityByBmodel(char* bmodel,list_p entities);

/**
 * \brief Add a new shader remap.
 *
 * Remaps oldShader with newShader.
 *
 * \param oldShader shader to be remapped
 * \param newShader replacement shader
 * \param timeOffset time offset
 */
void AddRemap(const char* oldShader, const char* newShader, float timeOffset);

//
// g_misc.c
//
/**
 * Teleport a player to an origin.
 *
 * \param player The player.
 * \param origin Where to teleport.
 * \param angles Angles of the player at destination.
 * \param tyTpye Teleport type.
 */
void TeleportPlayer( gentity_t* player, vec3_t origin, vec3_t angles, tpType_t tyType );

/**
 * Transport a player to an origin.
 *
 * \param player The player.
 * \param origin Destination.
 * \param angles Angles at destination.
 * \param speed Spit out speed at destination.
 */
void TransportPlayer( gentity_t* player, vec3_t origin, vec3_t angles, int speed );

/**
 * Start turbolist travel.
 *
 * \param ent The turbolift.
 * \author Ubergames - TiM
 */
void target_turbolift_start( /*@shared@*/ gentity_t* ent );

//
// g_weapon.c
//
/**
 * \brief Checks wether accuray for this hit should be logged.
 *
 * \param target the target entity
 * \param attacker the attacker entity
 */
qboolean G_Weapon_LogAccuracyHit( gentity_t* target, gentity_t* attacker );

/**
 * \brief Calculates the muzzle point.
 *
 * Calculates the muzzle point.
 *
 * \param ent the player
 * \param fwd the forward vector
 * \param rt the right vector
 * \param vup the up vector
 * \param muzzlePoint the muzzle point
 * \param projsize projsize
 */
void G_Weapon_CalcMuzzlePoint ( gentity_t* ent, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint, double projsize);

/**
 * Round a vector to integers for more efficient network
 * transmission, but make sure that it rounds towards a given point
 * rather than blindly truncating.  This prevents it from truncating 
 * into a wall.
 *
 * \param v vector to round
 * \param to rounded vector
 */
void G_Weapon_SnapVectorTowards( vec3_t v, vec3_t to );

//
// g_svcmds.c
//
/**
 * Check for server console command and run if it is.
 *
 * \return whether this was a server command.
 */
qboolean ConsoleCommand( void );

/**
 * Process IP bans.
 */
void G_ProcessIPBans(void);

/**
 * Filter a packet.
 */
qboolean G_FilterPacket (char* from);

/**
 * Get a client for a string.
 */
gclient_t* ClientForString( const char* s ); //RPG-X: RedTechie - Added so g_cmds could read the function

/**
 * Process ID bans.
 */
void G_ProcessIDBans( void );

/**
 * Check ID.
 */
qboolean CheckID( char*	id );

//
// g_weapon.c
//
/**
 * \brief Fire weapons.
 *
 * Handles weapon firing.
 *
 * \param ent the player
 * \param alt_fire was this alt fire mode?
 */
void FireWeapon( gentity_t* ent, qboolean alt_fire );

//
// p_hud.c
//
/**
 * When the intermission starts, this will be called for all players.
 * If a new client connects, this will be called after the spawn function.
 *
 * \param client A client.
 */
void MoveClientToIntermission (gentity_t* client);

/**
 * Send deathmatch scorebard message.
 *
 * \param client A client.
 */
void DeathmatchScoreboardMessage (gentity_t* client);

//
// g_main.c
//
/**
 * Find the intermission point. This is also used for spectator spawns.
 */
void FindIntermissionPoint( void );

/**
 * Run think functions for entity.
 */
void G_RunThink (gentity_t* ent);

/**
 * Send scorebard message to all clients.
 */
void SendScoreboardMessageToAllClients( void );

/**
 * Print function that prints to the server console.
 */
void QDECL G_Printf( const char* fmt, ... ) __attribute__ ((format (printf, 1, 2)));

/**
 * Error function. Will result in any running map to be stoped and this way somewhat stops the server.
 */
void QDECL G_Error( const char* fmt, ... ) __attribute__ ((format (printf, 1, 2)));

/**
 * Print a message in a specified clients console.
 */
void QDECL G_PrintfClient( gentity_t* ent, const char* fmt, ...) __attribute__ ((format (printf, 2, 3)));

/**
 * Print a message in all clients consoles.
 */
void QDECL G_PrintfClientAll(const char* fmt, ...) __attribute__ ((format (printf, 1, 2)));

//
// g_sql.c
//
extern qboolean sql_ready; /*<! Indicates whether sql is ready. */

/**
 * Initialize SQL.
 *
 * \return Success or fail.
 */
qboolean G_Sql_Init(void);

/**
 * Shutdown SQL.
 */
void G_Sql_Shutdown(void);

/**
 * Add a new user to the user database.
 *
 * \param uName Username.
 * \param password User's password.
 * \return Success or fail.
 */
qboolean G_Sql_UserDB_Add(const char* uName, const char* password);

/**
 * Check if a user has a specific right.
 *
 * \param uid User's id.
 * \param right Right's id.
 * \return Access or not.
 */
qboolean G_Sql_UserDB_CheckRight(int uid, int right);

/**
 * Grant a user a specific right.
 *
 * \param uid User's id.
 * \param right Right's id.
 * \return Success or fail.
 */
qboolean G_Sql_UserDB_AddRight(int uid, int right);

/**
 * Revoe a user a specific right.
 *
 * \param uid User's id.
 * \param right Right's id.
 * \return Success or fail.
 */
qboolean G_Sql_UserDB_RemoveRight(int uid, int right);

/**
 * Delete a user.
 *
 * \param uName Username.
 * \return Success or fail.
 */
qboolean G_Sql_UserDB_Del(const char* uName);

/**
 * User login.
 *
 * \param uName Username.
 * \param pwd Password.
 * \param clientnum User's clientnum.
 * \return Success or fail.
 */
qboolean G_Sql_UserDB_Login(const char* uName, const char* pwd, int clientnum);

/**
 * Get user id for a user.
 *
 * \param uName Username.
 * \return UID
 */
int G_Sql_UserDB_GetUID(const char* uName);

//RPG-X: J2J - Nice neat struct to hold info for admin tricorder transport..//////
//TiM: I may be a complete nutter here, but I'm gonna try and mod this to see if we can mosey SP transportery FX in here
//It's only really a matter of delaying the teleport a few seconds, and displaying some fancy stuff, like lens flares in the CG ^_^

//TiM: Addendum - Turns out I'll be referring to this more heavily than I thought, so I'm going to optimize it
//a bit more so it can store and then access data in a more easy way.

/**	\enum transportTypes_t
*	Enumeration used to indentify different transporter types.
*	
*	\author Ubergames
*/
typedef enum {
	TPT_TRICORDER,
	TPT_PORTABLE,
	//TPT_LOCATION, //Copy direct to current, so don't need this

	TPT_MAX
} transportTypes_t;

//TiM: In each transport case, we need these two variables,
//so let's make them structs for easier declaration.

/** \struct transVecData_t
*	Contains data needed for each transport.
*
*	\author Ubergames
*/
typedef struct {
	vec3_t	origin;
	vec3_t	angles;
} transVecData_t;

/** \struct RPGX_SiteTOSiteData
*	Contains data needed for Site to Site transport
*
*	\author Ubergames
*/
typedef struct
{
												//TiM :
	int					beamTime;				//!< Time the beam was initialized. 4 seconds after this, the player will teleport, 8 seconds later the effect will reset

	transVecData_t		currentCoord;			//!< If they are beaming, have the active variables placed here
	transVecData_t		storedCoord[TPT_MAX];	//!< Any separate beam data (ie, tricorder, portable beam etc) is kept here till needed.

	int					LastClick;				//!< Last click (in level time) when feature was used (used to prevent accidents)
	qboolean			beamed;					//!< Once we've called the teleport, make this true.  Or else we'll teleport on every clock cycle >.<
} RPGX_SiteTOSiteData;

extern RPGX_SiteTOSiteData TransDat[];

/** \struct RPGX_DragData
*	Another Neat Struct (tm) to hold data for the toggle drag admin feature.
*
*	/author Ubergames - J2J
*/
typedef struct
{
	int AdminId;			//!< -1 when not beting grabed
	float distance;			//!< Length of Leash

} RPGX_DragData;

extern RPGX_DragData DragDat[];

//////////////

/**
 * \brief Select a random spawn point.  
 *
 * Select a random spawn point.
 * 
 * \return a random spawn point
 */
/*@shared@*/ /*@null@*/  gentity_t* SelectRandomSpawnPoint( void );		//Added so it can be used in main.

//RPG-X Misc Sounds:
//extern sfxHandle_t n00bsnd;

/**
 * Shutdown game.
 *
 * \param restart For map restart?
 */
void G_ShutdownGame( int restart );

//RPG-X END///////////////////////////////////////////////////////////////////////


//
// g_log.c
//

//
// synchronized with cg_local.h via mental telepathy
//
/** \enum awardType_t
*	Enumeration containing the different award types.
*/
typedef enum
{
	AWARD_EFFICIENCY = 0,	//!< Accuracy
	AWARD_SHARPSHOOTER,		//!< Most compression rifle frags
	AWARD_UNTOUCHABLE,		//!< Perfect (no deaths)
	AWARD_LOGISTICS,		//!< Most pickups
	AWARD_TACTICIAN,		//!< Kills with all weapons
	AWARD_DEMOLITIONIST,	//!< Most explosive damage kills
	AWARD_STREAK,			//!< Ace/Expert/Master/Champion
	AWARD_TEAM,				//!< MVP/Defender/Warrior/Carrier/Interceptor/Bravery
	AWARD_SECTION31,		//!< All-around god
	AWARD_MAX
} awardType_t;

/** \enum teamAward_e
*	Enumeration containing the different award types for team games.
*/
typedef enum
{
	TEAM_NONE = 0,			//!< ha ha! you suck!
	TEAM_MVP,				//!< most overall points
	TEAM_DEFENDER,			//!< killed the most baddies near your flag
	TEAM_WARRIOR,			//!< most frags
	TEAM_CARRIER,			//!< infected the most people with plague
	TEAM_INTERCEPTOR,		//!< returned your own flag the most
	TEAM_BRAVERY,			//!< Red Shirt Award (tm). you died more than anybody.
	TEAM_MAX
} teamAward_e;

//
// the above lists are synchronized with cg_local.h via mental telepathy
//

#define AWARDS_MSG_LENGTH 256

/**
 * Print message to log.
 */
void QDECL G_LogPrintf( const char* fmt, ... ) __attribute__ ((format (printf, 1, 2)));
/**
 * Log weapon pickup.
 */
void QDECL G_LogWeaponPickup(int client, int weaponid);
/**
 * Log weapon fire.
 */
void QDECL G_LogWeaponFire(int client, int weaponid);
/**
 * Log weapon damage.
 */
void QDECL G_LogWeaponDamage(int client, int mod, int amount);
/**
 * Log weapon kill.
 */
void QDECL G_LogWeaponKill(int client, int mod);
/**
 * Log weapon death.
 */
void QDECL G_LogWeaponDeath(int client, int weaponid);
/**
 * Log weapon frag.
 */
void QDECL G_LogWeaponFrag(int attacker, int deadguy);
/**
 * Log weapon powerup.
 */
void QDECL G_LogWeaponPowerup(int client, int powerupid);
/**
 * Log weapon item.
 */
void QDECL G_LogWeaponItem(int client, int itemid);
/**
 * Log weapon init.
 */
void QDECL G_LogWeaponInit(void);
/**
 * Log weapon output.
 */
void QDECL G_LogWeaponOutput(void);
/**
 * Log exit.
 */
void QDECL G_LogExit( const char *string );
/**
 * Clear client log.
 */
void QDECL G_ClearClientLog(int client);

/*----------------------------------------------------------------------------------------*/

/** \struct clInitStatus_t
 *
 */
typedef struct
{
	char	model[MAX_QPATH];	/*!< player model */
	team_t	team;				/*!< player team  */
	pclass_t pClass;			/*!< player class */
	qboolean initialized;		/*!< initialized? */
} clInitStatus_t;

//
// g_active.c
//

/**
 *	A new command has arrived from the client
 *
 * \param clientNum Cleintnum.
 */
void ClientThink( int clientNum ); // TODO move me to g_client.c

/**
 *	Called at the end of each server frame for each connected client
 *	A fast client will have multiple ClientThink for each ClientEdFrame,
 *	while a slow client may have multiple ClientEndFrame between ClientThink.
 *
 * \param ent Client.
 */
void ClientEndFrame( gentity_t* ent ); // TODO move me to g_client.c

/**
 * Run client.
 *
 * \param ent Client.
 */
void G_RunClient( gentity_t* ent ); // TODO move me to g_client.c

/**
 * Remove a forcefield
 *
 * \param self The forcefield.
 */
void G_Active_ShieldRemove(gentity_t* self);

/**
*	Throw a weapon away.
*
*	\author Ubergames - Marcin
*	\date 03/12/2008
*	\param ent The client.
*	\param txt Optional message for PADD dropping.
*/
void G_ThrowWeapon(gentity_t* ent, char* txt);

//
// g_mem.c
//
/**
 * Allocate memory from the memory pool.
 *
 * \param size Amount of memory to allocate.
 */
/*@shared@*/ /*@null@*/ void* G_Alloc( int size );

/**
 * Init memory pool.
 */
void G_InitMemory( void );

/**
 * Print memory pool information.
 */
void Svcmd_GameMem_f( void );

//
// g_session.c
//

/**
 *	Read session data for a client.
 *
 *	Called on a reconnect
 *
 *	\param client The client.
 */
void G_ReadSessionData( gclient_t* client );

/**
 *	Initialize session data for a client.
 *
 *	Called on a first-time connect
 *
 *	\param client The client.
 *	\param userinfo String containing user info.
 */
void G_InitSessionData( gclient_t* client, char* userinfo );

/**
 *	Initialize world session.
 */
void G_InitWorldSession( void );

/**
 *	Write session data.
 */
void G_WriteSessionData( void );

//
// g_arenas.c
//

/**
 *	Update the tournament information.
 *
 *	\todo Remove function and tournament related code.
 */
void UpdateTournamentInfo( void );

/**
 *	Spawn models on the victory pads.
 *
 *	\todo Remove function and related code.
 */
void SpawnModelsOnVictoryPads( void );

/**
 *	Abort end of match podium display.
 *
 *	\todo Remove function and related code.
 */
void Svcmd_AbortPodium_f( void );

//
// g_bot.c
//

/**
 *	Initialize bots.
 *
 *	\param restart True if map restart.
 *
 *	\todo Remove? We don't support bots.
 */
void G_InitBots( qboolean restart );

/**
 *	Get bot info by number.
 *
 * \param num number
 *
 * \todo Remove? We don't support bots.
 */
char* G_GetBotInfoByNumber( int32_t num );

/**
 *	Get bot info by name.
 *
 *	\param name Bot name.
 *
 *	\todo Remove? We don't support bots.
 */
char* G_GetBotInfoByName( const char* name );

/**
 *	Check bot spawn queue and spawn bots if they are allowed.
 *
 *	\todo Remove? We don't support bots.
 */
void G_CheckBotSpawn( void );

/**
 *	Add a bot to the bot spawn queue.
 *
 *	\param clientNum Client number for this bot.
 *
 *	\todo Remove? We don't support bots.
 */
void G_QueueBotBegin( int clientNum );

/**
 *	Try to connect a bot to the game.
 *
 *	\param clientNum Client number for the bot.
 *	\param restart Is this a restart?
 *
 *	\todo Remove? We don't support bots.
 */
qboolean G_BotConnect( int32_t clientNum, qboolean restart );

/**
 *	Server command. Add bot.
 *
 *	\todo Remove? We don't support bots.
 */
void Svcmd_AddBot_f( void );

/**
 *	Server command. Bot list.
 *
 *	\todo Remove? We don't support bots.
 */
void Svcmd_BotList_f( void );

/**
 *	???
 *
 *	\todo Remove? We don't support bots.
 */
void AI_main_BotInterbreedEndMatch( void );

//
//	g_lua.c
//
#ifdef G_LUA
extern vmCvar_t g_debugLua;
extern vmCvar_t lua_allowedModules;
extern vmCvar_t lua_modules;
#endif

extern vmCvar_t g_developer;
// developer tools:
// Prevents a single statement from beeing executed if developer isn't set
#define DEVELOPER(X) if(g_developer.integer != 0) { \
						X\
					 }


#include "g_team.h" // teamplay specific stuff


extern	level_locals_t	level;
extern  gentity_t		g_entities[MAX_GENTITIES];

#define FOFS(x) ((size_t)&(((gentity_t *)0)->x))

//RPG-X: RedTechie
//extern int shaketimer; //Global shake timer varible //TiM: NOT NECESSARY!
extern uint32_t RPGEntityCount; //Global entity count varible

extern	vmCvar_t	g_logLevel;
extern	vmCvar_t	g_gametype;
extern	vmCvar_t	g_dedicated;
extern	vmCvar_t	g_cheats;
extern	vmCvar_t	g_maxclients;			// allow this many total, including spectators
extern	vmCvar_t	g_maxGameClients;		// allow this many active
extern	vmCvar_t	g_restarted;
extern	vmCvar_t	g_language;

extern	vmCvar_t	g_dmflags;
extern	vmCvar_t	g_password;
extern	vmCvar_t	g_needpass;
extern	vmCvar_t	g_gravity;
extern	vmCvar_t	g_speed;
extern	vmCvar_t	g_knockback;
extern	vmCvar_t	g_dmgmult;
extern	vmCvar_t	g_forcerespawn;
extern	vmCvar_t	g_inactivity;
extern	vmCvar_t	g_debugMove;
extern	vmCvar_t	g_debugAlloc;
extern	vmCvar_t	g_debugDamage;
extern	vmCvar_t	g_weaponRespawn;
extern	vmCvar_t	g_adaptRespawn;
extern	vmCvar_t	g_synchronousClients;
extern	vmCvar_t	g_motd;
extern	vmCvar_t	g_allowVote;
extern	vmCvar_t	g_banIPs;
extern	vmCvar_t	g_banIDs;	//TiM - Security ban system
extern	vmCvar_t	g_filterBan;
extern	vmCvar_t	g_debugForward;
extern	vmCvar_t	g_debugRight;
extern	vmCvar_t	g_debugUp;
extern	vmCvar_t	g_holoIntro;
extern	vmCvar_t	g_team_group_red;
extern	vmCvar_t	g_team_group_blue;
extern	vmCvar_t	g_random_skin_limit;
extern	vmCvar_t	g_classChangeDebounceTime;

//RPG-X: - RedTechie Even more CVAR Info :|
extern	vmCvar_t	rpg_allowvote;
extern	vmCvar_t	rpg_chatsallowed;
extern	vmCvar_t	rpg_allowsuicide;
extern	vmCvar_t	rpg_selfdamage;
extern	vmCvar_t	rpg_rpg;
extern	vmCvar_t	rpg_kickspammers;
extern	vmCvar_t	rpg_kicksuiciders;
extern	vmCvar_t	rpg_allowspmaps;
extern	vmCvar_t	rpg_rangetricorder;
extern	vmCvar_t	rpg_rangehypo;
extern	vmCvar_t	rpg_norpgclasses;
extern	vmCvar_t	rpg_forceclasscolor;

/*
 * Ristrictions: nocloak = 1, noflight = 2
 * Default value: 0
 */
extern  vmCvar_t	rpg_restrictions;

/*
 * Which weapons do damage?
 * Phaser			= 1
 * Crifle			= 2
 * TR116			= 4
 * Grenade Laucher	= 8
 * Quantum Burst	= 16
 * Disruptor		= 32
 * Default value: 63
 */
extern  vmCvar_t	rpg_dmgFlags;
extern	vmCvar_t	rpg_noweapons;
extern	vmCvar_t	rpg_welcomemessage;
extern	vmCvar_t	rpg_timedmessagetime;
extern	vmCvar_t	rpg_forcekillradius;
extern	vmCvar_t	rpg_forcekillradiuswaittime;

extern	vmCvar_t	rpg_chatarearange;
extern	vmCvar_t	rpg_forcefielddamage;
extern	vmCvar_t	rpg_invisibletripmines;
extern	vmCvar_t	rpg_medicsrevive;
extern	vmCvar_t	rpg_effectsgun;
extern	vmCvar_t	rpg_phaserdisintegrates;
extern	vmCvar_t	rpg_servershakeallclients;
extern	vmCvar_t	rpg_kickAfterXkills; //RPG-X | Phenix | 06/04/2005
extern	vmCvar_t	rpg_rankSet;
extern	vmCvar_t	rpg_passMessage;
extern	vmCvar_t	rpg_renamedPlayers;
extern	vmCvar_t	rpg_uniqueNames;
extern	vmCvar_t	rpg_startingRank;
extern	vmCvar_t	rpg_maxRank;
extern	vmCvar_t	rpg_changeRanks;
extern	vmCvar_t	rpg_classSet;

extern	vmCvar_t	rpg_scannablePanels;

/* 
 * TODO put maxs and mins into a single cvar to save cvars?
 * e.g. rpg_heightLimits
 * value would be a range. for example: "0.9-1.15"
 */
extern	vmCvar_t	rpg_maxHeight;
extern	vmCvar_t	rpg_minHeight;
extern	vmCvar_t	rpg_maxWeight;
extern	vmCvar_t	rpg_minWeight;

extern	vmCvar_t	rpg_mapGiveFlags;

//RPG-X | Marcin | 03/12/2008
//moar vars...
extern  vmCvar_t    rpg_allowWeaponDrop;
extern  vmCvar_t    rpg_weaponsStay;
extern	vmCvar_t	rpg_dropOnDeath;

// Weapon speeds
extern  vmCvar_t    rpg_rifleSpeed;      //RPG-X | Marcin | 04/12/2008
extern  vmCvar_t    rpg_disruptorSpeed;  //RPG-X | Marcin | 04/12/2008
extern  vmCvar_t    rpg_photonSpeed;     //RPG-X | Marcin | 05/12/2008
extern  vmCvar_t    rpg_altPhotonSpeed;  //RPG-X | Marcin | 06/12/2008

// Weapon Delays
//! Fire delay for Compression Rifle
extern  vmCvar_t    rpg_rifleDelay;      //RPG-X | Marcin | 06/12/2008
//! Fire delay for Disruptor
extern  vmCvar_t    rpg_disruptorDelay;  //RPG-X | Marcin | 06/12/2008
//! Fire delay for photon burst primary fire
extern  vmCvar_t    rpg_photonDelay;     //RPG-X | Marcin | 06/12/2008
//! Fire delay for photon burst secondary fire
extern  vmCvar_t    rpg_altPhotonDelay;  //RPG-X | Marcin | 06/12/2008
//! Fire delay for TR116
extern  vmCvar_t	rpg_TR116Delay;		 //RPG-X | Marcin | 30/12/2008
//! Fire delay for Tricorder alt fire
extern  vmCvar_t	rpg_altTricorderDelay;	 //RPG-X | GSIO01 | 14/05/2009

// Motd
extern	vmCvar_t	rpg_motdFile;		 //RPG-X | Marcin | 23/12/2008

// Privacy
extern	vmCvar_t	rpg_respectPrivacy;  //RPG-X | Marcin | 24/12/2008

// Weaps
extern	vmCvar_t	rpg_maxTricorders;	 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_maxPADDs;		 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_maxCups;		 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_maxPhasers;		 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_maxRifles;		 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_maxTR116s;		 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_maxAdminguns;	 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_maxPhotonbursts; //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_maxDisruptors;	 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_maxMedkits;		 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_maxHyposprays;	 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_maxRegenerators; //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_maxToolkits;	 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_maxHyperSpanners;//RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_minTricorders;	 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_minPADDs;		 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_minCups;		 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_minPhasers;		 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_minRifles;		 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_minTR116s;		 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_minAdminguns;	 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_minPhotonbursts; //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_minDisruptors;	 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_minMedkits;		 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_minHyposprays;	 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_minRegenerators; //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_minToolkits;	 //RPG-X | Marcin | 30/12/2008
extern	vmCvar_t	rpg_minHyperSpanners;//RPG-X | Marcin | 30/12/2008

// respawn delay
extern  vmCvar_t		rpg_fraggerSpawnDelay;	 //RPG-X | Marcin | 03/01/2009

// borg adaption
extern vmCvar_t rpg_borgAdapt;				//RPG-X | GSIO01 | 08/05/2009
extern vmCvar_t rpg_adaptUseSound;			//RPG-X | GSIO01 | 08/05/2009
extern vmCvar_t rpg_adaptPhaserHits;		//RPG-X | GSIO01 | 08/05/2009
extern vmCvar_t rpg_adaptCrifleHits;		//RPG-X | GSIO01 | 08/05/2009
extern vmCvar_t rpg_adaptTR116Hits;			//RPG-X | GSIO01 | 08/05/2009
extern vmCvar_t rpg_adaptDisruptorHits;		//RPG-X | GSIO01 | 08/05/2009
extern vmCvar_t rpg_adaptPhotonHits;		//RPG-X | GSIO01 | 08/05/2009
extern vmCvar_t rpg_adaptGrenadeLauncherHits;	//RPG-X | GSIO01 | 08/05/2009
extern vmCvar_t rpg_allowRemodulation;		//RPG-X | GSIO01 | 10/05/2009
extern vmCvar_t rpg_borgMoveThroughFields;	//RPG-X | GSIO01 | 19/05/2009
extern vmCvar_t rpg_RemodulationDelay;

// hypo melee
extern vmCvar_t rpg_hypoMelee;	//RPG-X | GSIO01 | 08/05/2009

// repairing breakables
extern vmCvar_t rpg_repairModifier;		 //RPG-X | GSIO01 | 09/05/2009

// forcefield colors
extern vmCvar_t rpg_forceFieldColor; //RPG-X | GSIO01 | 09/05/2009

// modulation thingies //RPG-X | GSIO01 | 12/05/2009
extern vmCvar_t	rpg_forceFieldFreq;

// turbolif travel time based on deck distance
extern vmCvar_t rpg_calcLiftTravelDuration;
extern vmCvar_t rpg_liftDurationModifier;

// admin vote override
extern vmCvar_t rpg_adminVoteOverride;

// server change entity
extern vmCvar_t rpg_serverchange;

// SP level change
extern vmCvar_t rpg_allowSPLevelChange;

// SQL Database
extern vmCvar_t sql_use;

// developer tools
extern vmCvar_t	dev_showTriggers;

extern vmCvar_t rpg_spEasterEggs;

//OUMS

/**	\typedef holoData_t
*	Pointer to \link holoData_s \endlink
*
*	\author Ubergames - GSIO01
*/
typedef struct holoData_s holoData_t;

/** \struct holoData_s
*	Contains data for holodeck Programms used by the holodeck UI and holodeck entities.
*
*	\author Ubergames - GSIO01
*/
struct holoData_s {
	char		target[5][MAX_QPATH];	//!< to what entity the player shoud be transported for each program
	char		name[5][MAX_QPATH];		//!< names of the holodeck programs
	char		desc1[5][512];			//!< first descriptions
	char		desc2[5][512];			//!< second descriptions
	char		image[5][MAX_QPATH];	//!< images to display for each program
	char		iTrigger[6][MAX_QPATH];
	char		dTrigger[6][MAX_QPATH];
	int			active;					//!< active holodeck program
	int			numProgs;				//!< number of programs <= 5
};

 typedef struct mapChangeData_s mapChangeData_t;
 struct mapChangeData_s {
	 char name[16][MAX_QPATH];
	 char bspname[16][MAX_QPATH];
 };

/**
*	Builds the config string for shader remapping.
*/
const /*@shared@*/ char* BuildShaderStateConfig(void);

typedef struct luaAlertState_s luaAlertState_t;
struct luaAlertState_s {
	int cond;
	int mode;
	char* shaders[4];
	char* sounds[4];
	char* targets[4];
};

extern luaAlertState_t* luaAlertState;

/* alert shaders */
typedef struct {
	/*@null@*/ char*	greenShaders[10];
	/*@null@*/ char*	redShaders[10];
	/*@null@*/ char*	yellowShaders[10];
	/*@null@*/ char*	blueShaders[10];
	int		numShaders;
} target_alert_Shaders_s;

/*
* @brief Checks if a pointer is NULL. If it is NULL it exits the function with the given return value.
* Prints an error message if ptr is NULL and prints function exit message on LL_TRACE. For void return use "(void)0"
* @param ptr The pointer to check.
* @param ret The return value.
* @return Whether the pointer is NULL or not.
*/
#define G_Assert(ptr, ret) 	if (ptr == NULL) { _G_LocLogger(__FILE__, __LINE__, LL_ERROR, "%s == NULL!\n", #ptr); _G_LocLogger(__FILE__, __LINE__, LL_TRACE, "%s - End\n", __FUNCTION__); return ret; }

#endif //_G_LOCAL_H_
