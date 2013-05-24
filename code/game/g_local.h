// Copyright (C) 1999-2000 Id Software, Inc.
//
// g_local.h -- local definitions for game module
#ifndef _G_LOCAL_H_
#define _G_LOCAL_H_

#include "q_shared.h"
#include "bg_public.h"
#include "g_public.h"
#include "list.h"

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
	/*@shared@*/ /*@null@*/ void		(*think)(gentity_t *self);	//!< think function
	/*@shared@*/ /*@null@*/ void		(*reached)(gentity_t *self);	//!< movers call this when hitting endpoint
	/*@shared@*/ /*@null@*/ void		(*blocked)(gentity_t *self, gentity_t *other); //!< movers call this when blocked
	/*@shared@*/ /*@null@*/ void		(*touch)(gentity_t *self, gentity_t *other, trace_t *trace); //!< touch function for triggers
	/*@shared@*/ /*@null@*/ void		(*use)(gentity_t *self, gentity_t *other, gentity_t *activator); //!< function that gets called if the entity is used
	/*@shared@*/ /*@null@*/ void		(*pain)(gentity_t *self, gentity_t *attacker, int damage); //!< function that gets called if entity gets damged
	/*@shared@*/ /*@null@*/ void		(*die)(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod); //!< function that gets called if entity dies

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
 * Set the score for a client.
 *
 * \param ent The client.
 * \param score New score for the client.
 */
void SetScore( gentity_t *ent, int score );

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
	gentity_t*			locationHead;			/*!< head of the location list */
	int					bodyQueIndex;			/*!< dead bodies */
	gentity_t*			bodyQue[BODY_QUEUE_SIZE]; /*!< body Que */

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
} level_locals_t;


//
// g_spawn.c
//
/**
 * \brief Get a string for a custom entity key.
 *
 * Spawn string returns a temporary reference, you must copy the string if you want to keep it.
 *
 * \param[in] key Key to get the value for.
 * \param[in] defaultString Default value for this key.
 * \param[out] out The result.
 * \return Success or fail.
 */
qboolean G_SpawnString( const char* key, const char* defaultString, char** out );

/**
 * \brief Get a float for a custom entity key.
 *
 * \param[in] key Key to get the value for.
 * \param[in] defaultString Default value for this key.
 * \param[out] out The result.
 * \return Success or fail.
 */
qboolean G_SpawnFloat( const char* key, const char* defaultString, float* out );

/**
 * \brief Get a int for a custom entity key.
 *
 * \param[in] key Key to get the value for.
 * \param[in] defaultString Default value for this key.
 * \param[out] out The result.
 * \return Success or fail.
 */
qboolean G_SpawnInt( const char* key, const char* defaultString, int* out );

/**
 * \brief Get a vector for a custom entity key.
 *
 * \param[in] key Key to get the value for.
 * \param[in] defaultString Default value for this key.
 * \param[out] out The result.
 * \return Success or fail.
 */
qboolean	G_SpawnVector( const char* key, const char* defaultString, float* out );

/**
 * \brief Spawn all entities from the entity string.
 */
void		G_SpawnEntitiesFromString( void );

/**
 * \brief Creates a copy of the given string. Allocated on the memory pool.
 *
 * \param string String to copy.
 * \return Copy of the string.
 */
/*@shared@*/ /*@null@*/ char* G_NewString( /*@null@*/ const char* string );

//
// g_cmds.c
//
/**
 * Concatenate all arguments for this string.
 *
 * \param start start from the given argument
 * \return String containing concatenated command arguments.
 */
char* ConcatArgs( int start );

/**
 * Request current scoreboard information.
 */
void Cmd_Score_f (gentity_t* ent);

/**
 * If the client being followed leaves the game, or you just want to drop
 * to free floating spectator mode
 */
void StopFollowing( gentity_t* ent );

/**
 * Let everyone know about a team change.
 *
 * \param client The client that changed team.
 * \param oldTeam The team the client was in.
 */
void BroadcastTeamChange( gclient_t* client, int oldTeam );

/**
 * Set the team for a player.
 *
 * \param ent A player.
 * \param s The new team.
 * \return Success or fail.
 */
qboolean SetTeam( gentity_t* ent, char* s );

/**
 * Cycle different players.
 */
void Cmd_FollowCycle_f( gentity_t* ent, int dir );

/**
 * Command signaling player readiness.
 *
 * \param ent A player.
 */
void Cmd_Ready_f (gentity_t* ent);

//
// g_items.c
//
 
/**
*	Data structures + functions for the PADD messaging system.
*
*	\author Ubergames - Marcin 
*	\date 06/12/2008
*/
typedef struct
{
	gentity_t*	key;		//!< PADD item entity pointer
	char		owner[64];
	char		value[256];
} paddData_t;

#define PADD_DATA_MAX	256 //!< max number of padds
#define MAX_DROPPED		255 //!< should be reasonable

extern paddData_t	paddData[PADD_DATA_MAX];
extern int			paddDataNum;
extern int			numTotalDropped;

/**
 * Add a new padd.
 *
 * \param key entity
 * \param who owner of the padd
 * \param txt text of the padd
 * \author Ubergames - Marcin
 * \date 06/12/2008
 */
void Padd_Add( gentity_t* key, gentity_t* who, char* txt);

/**
 * Pickup padd.
 *
 * \param key entity
 * \param who Who picked up the padd.
 * \return Text of the padd.
 * \author Ubergames - Marcin
 * \date 06/12/2008
 */
char* Padd_Get( gentity_t* key, gentity_t* who );

/**
 * Remove a padd.
 *
 * \param key entity
 * \author Ubergames - Marcin
 * \date 06/12/2008
 */
void Padd_Remove( gentity_t* key );

/**
 * Run an item.
 *
 * \param ent The item.
 */
void G_RunItem( gentity_t* ent );

/**
 * Repsawn an item.
 *
 * \param ent The item.
 */
void RespawnItem( gentity_t* ent );

/**
 * Spawns an item and tosses it forward.
 *
 * \param ent An entity to toss from.
 * \param item The item.
 * \param angle Direction to toss to.
 * \return The entity for the item.
 */
/*@shared@*/ /*@null@*/  gentity_t* Drop_Item( gentity_t* ent, gitem_t* item, float angle );

/**
 * Sets the clipping size and plants the object on the floor.
 * Items can't be immediately dropped to floor, because they might
 * be on an entity that hasn't spawned yet.
 *
 * \param ent Entity for item.
 * \param item The item.
 */
void G_SpawnItem (gentity_t* ent, gitem_t* item);

/**
 * Traces down to find where an item should rest, instead of letting them
 * free fall from their spawn points
 *
 * \param ent Entity for the item.
 */
void FinishSpawningItem( gentity_t* ent );

/**
 * Add ammo for a weapon to a player.
 *
 * \param ent The player.
 * \param weapon For which weapon.
 * \param count Ammount of ammo.
 */
void Add_Ammo (gentity_t* ent, int weapon, int count);

/**
 * Touch function for items.
 *
 * \param ent The entity for the item.
 * \param other The touching entity.
 * \param trace A trace.
 */
void Touch_Item (gentity_t* ent, gentity_t* other, trace_t* trace);

/**
 * Clear all registered items.
 */
void ClearRegisteredItems( void );

/**
 * Register a new item. The item will be added to the precache list.
 *
 * \param item Item to register.
 */
void RegisterItem( gitem_t* item );

/**
 * Write the needed items to a config string so the client will know which ones to precache.
 */
void SaveRegisteredItems( void );

//
// g_utils.c
//
/**
 * \brief Get the model index for a model.
 *
 * Get the model index for a model.
 *
 * \param name the model name
 *
 * \return the models index
 */
int G_ModelIndex( char* name );

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
/*@shared@*/ /*@null@*/  gentity_t* G_PickTarget (char* targetname);

/**
 * \brief Use all of the given entity's targets.
 *
 * Use all of the given entity's targets.
 *
 * \param ent the entity
 * \param activator the initiator of the function call
 */
void G_UseTargets (gentity_t* ent, gentity_t* activator);

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
void G_UseTargets2( gentity_t* ent, gentity_t* activator, char* target );

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
int G_RadiusList ( vec3_t origin, float radius,	list_p ignore, qboolean takeDamage, list_p ent_list);

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
int G_RadiusListOfTypes(list_p classnames, vec3_t origin, float radius, list_p ignore, list_p ent_list);

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
/*@shared@*/ /*@null@*/  gentity_t* G_GetNearestEnt(char* classname, vec3_t origin, float radius, list_p ignore, qboolean takeDamage);

/**
 * Get the nearest player orund an origin.
 *
 * \param origin Origin to search around.
 * \param radius Radius to search in.
 * \param ignore List of entities to ignore.
 * \return Nearest player.
 */
/*@shared@*/ /*@null@*/  gentity_t* G_GetNearestPlayer(vec3_t origin, float radius, list_p ignore );

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
// g_combat.c
//
/**
 * Returns qtrue if the inflictor can directly damage the target.  Used for
 * explosions and melee attacks.
 *
 * \param targ the target
 * \param origin the origin
 * \return qtrue if the inflictor can directly damage the target.  Used for explosions and melee attacks.
 */
qboolean CanDamage (gentity_t* targ, vec3_t origin);

/**
 * Damage an entity.
 *
 * \param targ entity that is being damaged
 * \param inflictor entity that is causing the damage
 * \param attacker entity that caused the inflictor to damage targ
 * \param dir direction of the attack for knockback
 * \param point point at which the damage is being inflicted, used for headshots
 * \param damage amount of damage being inflicted
 * \param knockback force to be applied against targ as a result of the damage
 * \param dflags these flags are used to control how G_Damage works
 * \param mod means of death
 *
 * Damage flags:
 * DAMAGE_RADIUS			damage was indirect (from a nearby explosion)
 * DAMAGE_NO_ARMOR			armor does not protect from this damage
 * DAMAGE_NO_KNOCKBACK		do not affect velocity, just view angles
 * DAMAGE_NO_PROTECTION		kills godmode, armor, everything
 */
void G_Damage (gentity_t* targ, gentity_t* inflictor, gentity_t* attacker, vec3_t dir, vec3_t point, int damage, int dflags, int mod);

/**
 * Damage all entities around an origin in a specified radius.
 *
 * \return Hit a client?
 * \param origin Origin.
 * \param attacker Attacker.
 * \param damage Amount much damage.
 * \param radius Radius.
 * \param ignore Entity to ignore.
 * \param dflags Damage flags.
 * \param mod Means of death.
 * \todo Replace ignore entity by list of entites.
 */
qboolean G_RadiusDamage (vec3_t origin, gentity_t* attacker, float damage, float radius, gentity_t* ignore, int dflags, int mod);

/**
 * Let a body die.
 *
 * \param self Self.
 * \param inflictor entity that is causing the damage
 * \param attacker entity that caused the inflictor to damage targ
 * \param damage Amount of damage.
 * \param meansOfDeath Means of death.
 */
void body_die( gentity_t* self, gentity_t* inflictor, gentity_t* attacker, int damage, int meansOfDeath );

/**
 * Toss the weapon and powerups for the killed player.
 *
 * \param self the client.
 * \param Caused by disconnect?
 */
void TossClientItems( gentity_t* self, qboolean dis_con );

/**
 * Repairs repairable entities.
 *
 * \param ent The player.
 * \param tr_ent Entity to repair.
 * \param rate Rate to repair with.
 * \author Ubergames - GSIO01
 * \date 09/05/2009
 */
void G_Repair(gentity_t* ent, gentity_t* tr_ent, float rate);

// damage flags
#define DAMAGE_RADIUS				0x00000001	// damage was indirect
#define DAMAGE_NO_ARMOR				0x00000002	// armor (shields) do not protect from this damage
#define DAMAGE_NO_KNOCKBACK			0x00000008	// do not affect velocity, just view angles
#define DAMAGE_NO_PROTECTION		0x00000020  // armor, shields, invulnerability, and godmode have no effect
#define DAMAGE_NOT_ARMOR_PIERCING	0x00000000	// trek: shields fully protect from this damage (for clarity)
#define DAMAGE_ARMOR_PIERCING		0x00000040	// trek: shields don't fully protect from this damage
#define DAMAGE_NO_INVULNERABILITY	0x00000080	// trek: invulnerability doesn't event protect from this damage
#define DAMAGE_HALF_NOTLOS			0x00000100	// trek: radius damage still does 1/2 damage to ents that do not have LOS to explosion org but are in radius
#define DAMAGE_ALL_TEAMS			0x00000200	// trek: damage ignores teamdamage settings

//
// g_missile.c
//
/**
 * Run a missile.
 *
 * \param ent the missile
 */
void G_Missile_Run( gentity_t* ent );

/**
 * Fire alient plasma projectile.
 *
 * \param The shooter.
 * \param start Start point.
 * \param aimdir Direction.
 */
/*@shared@*/ /*@null@*/  gentity_t* fire_plasma (gentity_t* self, vec3_t start, vec3_t aimdir);

/**
 * Fire torpedo projectile.
 *
 * \param The shooter.
 * \param start Start point.
 * \param aimdir Direction.
 */
/*@shared@*/ /*@null@*/  gentity_t* fire_quantum (gentity_t* self, vec3_t start, vec3_t aimdir);

/**
 * Fire grenade.
 *
 * \param The shooter.
 * \param start Start point.
 * \param aimdir Direction.
 */
/*@shared@*/ /*@null@*/  gentity_t* fire_grenade (gentity_t* self, vec3_t start, vec3_t aimdir);

/**
 * Fire a rocket.
 *
 * \param The shooter.
 * \param start Start point.
 * \param dir Direction.
 */
/*@shared@*/ /*@null@*/  gentity_t* fire_rocket (gentity_t* self, vec3_t start, vec3_t dir);

/**
 * Fire a compression rifle projectile.
 *
 * \param The shooter.
 * \param start Start point.
 * \param dir Direction.
 */
/*@shared@*/ /*@null@*/  gentity_t* fire_comprifle (gentity_t* self, vec3_t start, vec3_t dir);

//
// g_mover.c
//
/**
 * Run a mover.
 *
 * \param ent Mover to run.
 */
void G_Mover_Run( gentity_t* ent );

/**
 * Touch function for doors.
 *
 * \param ent The mover.
 * \param other The touching entity.
 * \param trace A trace.
 */
void G_Mover_TouchDoorTrigger( gentity_t* ent, gentity_t* other, trace_t* trace );

/**
 * Use function for binary movers.
 *
 * \param ent The mover.
 * \param other Other entity.
 * \param activator Activator.
 */
void G_Mover_UseBinaryMover( gentity_t* ent, gentity_t* other, gentity_t* activator );

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
void target_turbolift_start( gentity_t* ent );

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
void G_Weapon_CalcMuzzlePoint ( gentity_t* ent, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint, float projsize);

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
// g_client.c
//
/**
 * Get number of clients in team.
 *
 * \param ignoreClientNum Client to ignore.
 * \param team Team.
 * \reutrn Number of clients in team.
 */
team_t G_Client_TeamCount( int ignoreClientNum, int team );

/**
 * Pick a random team.
 *
 * \param ignoreClientNum Client to ignore.
 * \return Random team.
 */
team_t G_Client_PickTeam( int ignoreClientNum );

/**
 * Set the clients view angle.
 *
 * \param ent Entity for which to set view angle.
 * \param angle New view angle.
 */
void G_Client_SetViewAngle( gentity_t* ent, vec3_t angle );

/**
 * Select a spawnpoint.
 *
 * \param avoidPoint Point to avoid.
 * \param origin Origin.
 * \param angles Angles.
 */
/*@shared@*/ /*@null@*/  gentity_t* G_Client_SelectSpawnPoint ( vec3_t avoidPoint, vec3_t origin, vec3_t angles );

/**
 * Respawn client.
 *
 * \param ent Client to respawn.
 */
void G_Client_Respawn(gentity_t *ent);

/**
 * Begin intermission.
 */
void G_Client_BeginIntermission(void);

/**
 * Init the body que.
 */
void G_Client_InitBodyQue(void);

/**
 * Spawn client.
 *
 * \param ent Client to spawn
 * \param rpgx_spawn rpgx_spawn
 * \param fromDeath Is this a spawn from death?
 */
void G_Client_Spawn( gentity_t* ent, int rpgx_spawn, qboolean fromDeath );

/**
 * Let the client die.
 *
 * \param self Client.
 * \param inflictor Entity causing death.
 * \param attacker Entity that made inflicotr cause death.
 * \param damage ammount of demage
 * \param mod means of death
 */
void G_Client_Die (gentity_t* self, gentity_t* inflictor, gentity_t* attacker, int damage, int mod);

/**
 * Add score for the client.
 *
 * \param ent The client.
 * \param score Ammount of score to add.
 */
void G_Client_AddScore( gentity_t* ent, int score );

/**
 * Calculate clients ranks.
 *
 * \param fromExit From exit?
 */
void G_Client_CalculateRanks( qboolean fromExit );

/**
 * Determine whether spot would telefrag.
 *
 * \param spot Spot to check.
 * \return Whether this spot would telefrag.
 */
qboolean G_Client_SpotWouldTelefrag( gentity_t* spot );

/**
 * Get weapons for a class.
 *
 * \param client The client.
 * \param pclass Class to get weapons for.
 */
void G_Client_WeaponsForClass( gclient_t* client, pclass_t pclass );

/**
 * Get holdable items for a class.
 *
 * \param client The client.
 * \param pclass Class for which to get holdables.
 */
void G_Client_HoldablesForClass( gclient_t* client, pclass_t pclass );

/**
 * Store the clients initial status.
 *
 * \param ent The client.
 */
void G_Client_StoreClientInitialStatus( gentity_t* ent );

/**
 *	Get location message for a client.
 * 
 * \param ent The client.
 * \param loc Location char.
 * \param loclen Length of location char.
 * \return Indicates success or fail.
 */
qboolean G_Client_GetLocationMsg(gentity_t* ent, char* loc, int loclen);

/**
 * Check client statuses.
 */
void G_Client_CheckClientStatus(void);

/**
 * Send client location information.
 *
 * \param ent The client.
 */
void G_Client_LocationsMessage( gentity_t *ent );

/**
 * Client connect.
 *
 * \param clientNum Client num.
 * \param firstTime Indicate whether this was the first connect.
 * \param isBot Indicate if bot or player.
 */
char* G_Client_Connect( int clientNum, qboolean firstTime, qboolean isBot );

/**
 * Clients user info changed.
 *
 * \param clientNum Client num.
 */
void G_Client_UserinfoChanged( int clientNum );

/**
 * Disconnect client.
 *
 * \param clientNum Client num.
 */
void G_Client_Disconnect( int clientNum );

/**
 * Initialize client.
 *
 * \param clientNum Client num.
 * \param careAboutWarmup Indicate whether to care about warm up.
 * \param isBot Bot or client?
 * \param first First time?
 */
void G_Client_Begin( int clientNum, qboolean careAboutWarmup, qboolean isBot, qboolean first );

/**
 * Client command.
 *
 * \param clientNum Client num.
 */
void G_Client_Command( int clientNum );

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
// g_fx.c
//
void SP_fx_spark( gentity_t* ent );

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

//TiM - Delayed Transport Beam
void G_InitTransport( int clientNum, vec3_t origin, vec3_t angles );

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
void ThrowWeapon( gentity_t* ent, char* txt );

/**
 *	Drop a weapon.
 *
 *	\author Ubergames - Marcin 
 *	\date 03/12/2008
 *	\param ent The client.
 *	\param item The item.
 *	\param angle Angle to throw at.
 *	\param flags Entity flags to use.
 *	\param Optional message for PADD dropping.
 */
/*@shared@*/ /*@null@*/  gentity_t *DropWeapon( gentity_t* ent, gitem_t* item, float angle, int flags, char* txt );

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
char* G_GetBotInfoByNumber( int num );

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
qboolean G_BotConnect( int clientNum, qboolean restart );

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
void BotInterbreedEndMatch( void );

// ai_main.c

//some maxs
#define MAX_NETNAME		36
#define MAX_FILEPATH	144

//bot settings
/** \struct bot_settings_t
*	Containing settings for bots.
*/
typedef struct bot_settings_s
{
	char	characterfile[MAX_FILEPATH];	/*!< filename of character file */
	int		skill;							/*!< skill level */
	char	team[MAX_FILEPATH];				/*!< team */
	char	pclass[MAX_FILEPATH];			/*!< class */
} bot_settings_t;

/**
 * Setup bot AI.
 *
 * \param restart Determines if this is a map restart.
 */
int BotAISetup( int restart );

/**
 * Shutdown bot AI.
 *
 * \param restart Determines if this is a map restart.
 */
int BotAIShutdown( int restart );

/**
 * Load map in bot lib.
 *
 * \param restart Determines if this is a map restart.
 */
int BotAILoadMap( int restart );

/**
 * Setup bot AI for client.
 *
 * \param client client number
 * \param settings bot settings
 */
int BotAISetupClient( int client, bot_settings_t* settings );

/**
 * Shutdown bot client.
 *
 * \param client client number
 */
int BotAIShutdownClient( int client );

/**
 * Star frame.
 *
 * \param time current time
 */
int BotAIStartFrame( int time );


//
//	g_lua.c
//
#ifdef G_LUA
/**
 *	Lua hook for InitGame event.
 *
 *	\param leveltime level time the event occured
 *	\param radomseed a random seed
 *	\param restart is this a map restart?
 */
void LuaHook_G_InitGame(int leveltime, unsigned int randomseed, int restart);

/**
 *	Lua hook for Shutdown event.
 *
 *	\param restart is this a map restart?
 */
void LuaHook_G_Shutdown(int restart);

/**
 *	Lua hook for RunFrame event.
 *
 *	\param leveltime the level time
 */
void LuaHook_G_RunFrame(int leveltime);

/**
 * Lua hook for G_Print function.
 *
 *	\param text text to be printed
 */
void LuaHook_G_Print(char* text);
/**
 * Lua hook for G_ClientPrint function.
 *
 *	\param text text to be printed
 *	\param entnum entity index for client the text gets send to
 */
void LuaHook_G_ClientPrint(char* text, int entnum);

/**
 * Lua hook for entity think function function.
 *
 *	\param function name of function to call
 *	\param entnum entity index of entity the think function was called on
 *	\return success or fail
 */
void LuaHook_G_EntityThink(char* function, int entnum);

/**
 * Lua hook for entity touch function function.
 *
 *	\param function name of function to call
 *	\param entnum entity index of entity the touch function was called on
 *	\param othernum entiy index of touching entity
 *	\return success or fail
 */
void LuaHook_G_EntityTouch(char* function, int entnum, int othernum);

/**
 * Lua hook for entity use function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the use function was called on
 * \param othernum entity index of other entity
 * \param activatornum entity index of activating entity
 */
void LuaHook_G_EntityUse(char* function, int entnum, int othernum, int activatornum);

/**
 * Lua hook for entity hurt function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the hurt function was called on
 * \param inflictornum entity index of inflictor
 * \param attackernum entity index of attacker
 */
void LuaHook_G_EntityHurt(char* function, int entnum, int inflictornum, int attackernum);

/**
 * Lua hook for entity die function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the die function was called on
 * \param inflictornum entity index of inflictor
 * \param attackernum entity index of attacker
 * \param dmg ammount of damage
 * \param mod means of death
 */
void LuaHook_G_EntityDie(char* function, int entnum, int inflictornum, int attackernum, int dmg, int mod);

/**
 * Lua hook for entity free function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the free function was called on
 */
void LuaHook_G_EntityFree(char* function, int entnum);

/**
 * Lua hook for entity trigger function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the trigger function was called on
 * \param othernum entity index of triggering entity
 */
void LuaHook_G_EntityTrigger(char* function, int entnum, int othernum);

/**
 * Lua hook for entity spawn function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the spawn function was called on
 */
void LuaHook_G_EntitySpawn(char* function, int entnum);

/**
 * Lua hook for entity reached function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the reached function was called on
 */
void LuaHook_G_EntityReached(char* function, int entnum);

/**
 * Lua hook for entity reached angular function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the reached angular function was called on
 */
void LuaHook_G_EntityReachedAngular(char* function, int entnum);

/**
 * Output information about lua threads.
 */
void G_LuaNumThreads(void);

/**
 * Collect garbage in lua.
 */
void G_LuaCollectGarbage(void);

/**
 * Show lua status information.
 *
 * \param ent client
 */
void G_LuaStatus(gentity_t* ent);

/**
 * Initialize lua.
 */
qboolean G_LuaInit(void);

/**
 * Shutdown lua.
 */
void G_LuaShutdown(void);

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
extern int RPGEntityCount; //Global entity count varible

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

// Weapon Damage
extern  vmCvar_t	rpg_rifleDamage;
extern  vmCvar_t	rpg_rifleAltDamage;
extern  vmCvar_t	rpg_phaserDamage;
extern  vmCvar_t	rpg_disruptorDamage;
extern  vmCvar_t	rpg_grenadeDamage;
extern  vmCvar_t	rpg_grenadeAltDamage;
extern  vmCvar_t	rpg_tr116Damage;
extern  vmCvar_t	rpg_photonDamage;
extern  vmCvar_t	rpg_photonAltDamage;

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



//RPG-X - Stephen: Some global varibles for RPG-X
//extern int lastTimedMessage; //The last timed message that was displayed
//TiM - since we'll only use this in g_active, why not reduce its scope to make things easier. :)

/**
 * Call G_PRINT in the engine.
 * \param fmt format string.
 */
void trap_Printf( const char* fmt );

/**
*	Calls Com_error in the engine
*	\param fmt error desription
*/
void trap_Error( const char* fmt );

/**
*	Get milliseconds since engine start
*	\return milliseconds since engine start
*/
int trap_Milliseconds( void );

/**
*	Get count of arguments for the current client game command
*	\return count of arguments
*/
int trap_Argc( void );

/**
*	Get a n of the current client game command
*	\param n argument to get
*	\param buffer buffer to store the argument in
*	\param bufferLength size of the buffer
*/
void trap_Argv( int n, char* buffer, int bufferLength );

/**
*	Get all args of the current client game command
*	\param buffer buffer to store the arguments in
*	\param bufferLength size of the buffer
*/
void trap_Args( char* buffer, int bufferLength );

/**
*	Opens a file
*	\param qpath path and filename
*	\param f filehandle to use
*	\param mode mode to use
*/
int trap_FS_FOpenFile( const char* qpath, fileHandle_t* f, fsMode_t mode );

/**
*	Read a opened file
*	\param buffer buffer to read to
*	\param len buffer length or length to read (<sizeof(buffer))
*	\param f filehandle for the file to read from
*
*	You have to open the file first.
*/
void trap_FS_Read( void* buffer, int len, fileHandle_t f );

/**
*	Write to a file
*	\param buffer text to write
*	\param len length of buffer
*	\param f filehandle  for the file to write to
*	
*	You have to open the file first.
*/
void trap_FS_Write( const void* buffer, int len, fileHandle_t f );

/**
*	Close a file
*	\param f filehandle for file to close
*/
void trap_FS_FCloseFile( fileHandle_t f );

/**
*	Get a list of files in a path
*	\param path path to get the list for
*	\param extension filter to get only files with this extension
*	\param listbuf buffer to store the file list in
*	\param bufsize size of the buffer
*	\return number of files in the list
*/
int trap_FS_GetFileList( const char* path, const char* extension, char* listbuf, int bufsize );

/**
*	Sends a console command to execute to the client console
*	\param exec_when when to exec (e.g. EXEC_APPEND)
*	\param text the command to execute
*/
void trap_SendConsoleCommand( int exec_when, const char *text );

/**
*	Register a cvar
*	\param cvar representation of the cvar in the vm
*	\param var_name name of the cvar
*	\param value default value for the cvar
*	\param flags additional options for the cvar (e.g. CVAR_ARCHIVE)
*/
void trap_Cvar_Register( vmCvar_t* cvar, const char* var_name, const char* value, int flags );

/**
*	\brief Update a cvar.
*
*	Tells the server/engine that a cvar in the vm has changed.
*	\param cvar cvar to update
*/
void trap_Cvar_Update( vmCvar_t* cvar );

/**
*	Set the cvar to a value.
*	\param var_name name of the cvar to set
*	\param value new value for the cvar
*/
void trap_Cvar_Set( const char* var_name, const char* value );

/**
*	Get the integer value for an cvar
*	\param var_name name of the cvar
*/
int trap_Cvar_VariableIntegerValue( const char* var_name );

/**
*	Get the value of the cvar as string
*	\param var_name name of the cvar
*	\param buffer to store the value
*	\param bufsize size of the buffer
*/
void trap_Cvar_VariableStringBuffer( const char* var_name, char* buffer, int bufsize );

/**
*	Send some information of the current game/map to the server
*	\param pointer to level.entities which is g_entities
*	\param numGEntities number of game entities (level.num_entities)
*	\param sizeofGEntity_t size of gentity_t
*	\param gameClients level.clients[0].ps
*	\param sizeOfGameClient size of level.clients[0]
*/
void trap_LocateGameData( gentity_t* gEnts, int numGEntities, int sizeofGEntity_t, playerState_t* gameClients, int sizeofGameClient );

/**
*	Drop a client from server.
*	\param clientNum client number of client to drop
*	\param test reason for client drop
*/
void trap_DropClient( int clientNum, const char* reason );

/**
*	\brief Send a server command to the client
*	\param clientNum client number of client
*	\param server command to execute
*
*	A value of -1 for clientNum will send the command to all clients.
*/
void trap_SendServerCommand( int clientNum, const char* text );

/**
*	Set a configstring
*	\param num CS_...
*	\param string set cofig string to this
*/
void trap_SetConfigstring( int num, const char* string );

/**
*	Get a configstring
*	\param num CS_...
*	\param buffer buffer to store config string in
*	\param bufferSize size of buffer
*/
void trap_GetConfigstring( int num, char* buffer, int bufferSize );

/**
*	Get the userinfo for a client
*	\param num client number
*	\param buffer buffer to store config string in
*	\param size of buffer
*/
void trap_GetUserinfo( int num, char* buffer, int bufferSize );

/**
*	Set the userinfo for a client
*	\param num client number
*	\param buffer string the contains new userinfo
*/
void trap_SetUserinfo( int num, const char* buffer );

/**
*	Get server info.
*	\param buffer buffer to store the info in
*	\param bufferSize size of buffer
*/
void trap_GetServerinfo( char* buffer, size_t bufferSize );

/**
*	\brief Set the brush model for a entity.
*	\param ent entity to the the model on
*	\param name the model name
*
*	The normal case is trap_SetBrushModel(ent, ent->model).
*	Brush models always have names of *<int>.
*/
void trap_SetBrushModel( gentity_t* ent, const char* name );

/**
*	\brief Do a trace on the server
*	\param results trace_t to store the results in
*	\param start startpoint
*	\param end endpoint
*	\param mins used to define the volume size
*	\param maxs uses to define the volume size
*	\param passEntityNum entity/entities to ignore
*	\param contentmask only check for this contents (CONTENT_...)
*
*	This actually moves a box with the size defined by mins and maxs through the world
*	from start to end and checks whether it colides with anything that matches the contentmask.
*	The entities that math the passEntityNum will be ingnored.
*/
void trap_Trace( trace_t* results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );

/**
*	\param point the point
*	\param passEntityNum ingore this
*
*	Works similar to a trace but only check a single point.
*/
int trap_PointContents( const vec3_t point, int passEntityNum );

qboolean trap_InPVS( const vec3_t p1, const vec3_t p2 );

qboolean trap_InPVSIgnorePortals( const vec3_t p1, const vec3_t p2 );

/**
*	Adjust the state of a area portal used for doors etc
*	\param ent entity that effects the areaportal (area portal is inide the entities bounds)
*	\param open open or close it?
*/
void trap_AdjustAreaPortalState( gentity_t* ent, qboolean open );

/**
*	Checks if two areas are connected.
*/
qboolean trap_AreasConnected( int area1, int area2 );

/**
*	Link an entity.
*	This results in shared values beeing avaible on both game and client side.
*/
void trap_LinkEntity( gentity_t* ent );

/**
*	Unlinks an entity.
*/
void trap_UnlinkEntity( gentity_t* ent );

/**
*	\brief Get a list of all entities in a box.
*	\param entityList list where entitynums will be stored
*	\param maxcount limits the number of listed entities
*
*	The size of the box is defined by mins and maxs.
*/
int trap_EntitiesInBox( const vec3_t mins, const vec3_t maxs, int* entityList, int maxcount );

/**
*	Checks if a entity is in contact with a defined box.
*/
qboolean trap_EntityContact( const vec3_t mins, const vec3_t maxs, const gentity_t* ent );

/**
*	Allocates a free client for a bot.
*/
int trap_BotAllocateClient( void );

/**
*	Free the client that was used for a bot.
*/
void trap_BotFreeClient( int clientNum );

/**
*	Get the last command a user did.
*/
void trap_GetUsercmd( int clientNum, usercmd_t* cmd );

/**
 * Get entity token.
 */
qboolean trap_GetEntityToken( char* buffer, int bufferSize );

/**
 * Create a debug polygon.
 *
 * \param color color of the polygon
 * \param numPoints number of points the polygon has
 * \param points points of the polygon
 * \return Polgon id
 */
int trap_DebugPolygonCreate(int color, int numPoints, vec3_t* points);

/**
 * Deletes a debug polygon.
 *
 * \param id id of polygon to delete
 */
void trap_DebugPolygonDelete(int id);

int		trap_BotLibSetup( void );
int		trap_BotLibShutdown( void );
int		trap_BotLibVarSet(char* var_name, char* value);
int		trap_BotLibVarGet(char* var_name, char* value, int size);
int		trap_BotLibDefine(char* string);
int		trap_BotLibStartFrame(float time);
int		trap_BotLibLoadMap(const char* mapname);
int		trap_BotLibUpdateEntity(int ent, void* bue);
int		trap_BotLibTest(int parm0, char* parm1, vec3_t parm2, vec3_t parm3);

int		trap_BotGetSnapshotEntity( int clientNum, int sequence );
int		trap_BotGetConsoleMessage(int clientNum, char* message, int size);
void	trap_BotUserCommand(int client, usercmd_t* ucmd);

void		trap_AAS_EntityInfo(int entnum, void* info);

int			trap_AAS_Initialized(void);
void		trap_AAS_PresenceTypeBoundingBox(int presencetype, vec3_t mins, vec3_t maxs);
float		trap_AAS_Time(void);

int			trap_AAS_PointAreaNum(vec3_t point);
int			trap_AAS_TraceAreas(vec3_t start, vec3_t end, int* areas, vec3_t* points, int maxareas);

int			trap_AAS_PointContents(vec3_t point);
int			trap_AAS_NextBSPEntity(int ent);
int			trap_AAS_ValueForBSPEpairKey(int ent, char* key, char* value, int size);
int			trap_AAS_VectorForBSPEpairKey(int ent, char* key, vec3_t v);
int			trap_AAS_FloatForBSPEpairKey(int ent, char* key, float* value);
int			trap_AAS_IntForBSPEpairKey(int ent, char* key, int* value);

int			trap_AAS_AreaReachability(int areanum);

int			trap_AAS_AreaTravelTimeToGoalArea(int areanum, vec3_t origin, int goalareanum, int travelflags);

int			trap_AAS_Swimming(vec3_t origin);
int			trap_AAS_PredictClientMovement(void* move, int entnum, vec3_t origin, int presencetype, int onground, vec3_t velocity, vec3_t cmdmove, int cmdframes, int maxframes, float frametime, int stopevent, int stopareanum, int visualize);


void	trap_EA_Say(int client, char* str);
void	trap_EA_SayTeam(int client, char* str);
void	trap_EA_SayClass(int client, char* str);
void	trap_EA_UseItem(int client, char* it);
void	trap_EA_DropItem(int client, char* it);
void	trap_EA_UseInv(int client, char* inv);
void	trap_EA_DropInv(int client, char* inv);
void	trap_EA_Gesture(int client);
void	trap_EA_Command(int client, char* command);

void	trap_EA_SelectWeapon(int client, int weapon);
void	trap_EA_Talk(int client);
void	trap_EA_Attack(int client);
void	trap_EA_Alt_Attack(int client);
void	trap_EA_Use(int client);
void	trap_EA_Respawn(int client);
void	trap_EA_Jump(int client);
void	trap_EA_DelayedJump(int client);
void	trap_EA_Crouch(int client);
void	trap_EA_MoveUp(int client);
void	trap_EA_MoveDown(int client);
void	trap_EA_MoveForward(int client);
void	trap_EA_MoveBack(int client);
void	trap_EA_MoveLeft(int client);
void	trap_EA_MoveRight(int client);
void	trap_EA_Move(int client, vec3_t dir, float speed);
void	trap_EA_View(int client, vec3_t viewangles);

void	trap_EA_EndRegular(int client, float thinktime);
void	trap_EA_GetInput(int client, float thinktime, void* input);
void	trap_EA_ResetInput(int client);


int		trap_BotLoadCharacter(char* charfile, int skill);
void	trap_BotFreeCharacter(int character);
float	trap_Characteristic_Float(int character, int index);
float	trap_Characteristic_BFloat(int character, int index, float min, float max);
int		trap_Characteristic_Integer(int character, int index);
int		trap_Characteristic_BInteger(int character, int index, int min, int max);
void	trap_Characteristic_String(int character, int index, char *buf, int size);

int		trap_BotAllocChatState(void);
void	trap_BotFreeChatState(int handle);
void	trap_BotQueueConsoleMessage(int chatstate, int type, char* message);
void	trap_BotRemoveConsoleMessage(int chatstate, int handle);
int		trap_BotNextConsoleMessage(int chatstate, void* cm);
int		trap_BotNumConsoleMessages(int chatstate);
void	trap_BotInitialChat(int chatstate, char* type, int mcontext, char* var0, char* var1, char* var2, char* var3, char* var4, char* var5, char* var6, char* var7 );
int		trap_BotNumInitialChats(int chatstate, char* type);
int		trap_BotReplyChat(int chatstate, char* message, int mcontext, int vcontext, char* var0, char* var1, char* var2, char* var3, char* var4, char* var5, char* var6, char* var7 );
int		trap_BotChatLength(int chatstate);
void	trap_BotEnterChat(int chatstate, int client, int sendto);
void	trap_BotGetChatMessage(int chatstate, char* buf, int size);
int		trap_StringContains(char* str1, char* str2, int casesensitive);
int		trap_BotFindMatch(char* str, void* match, unsigned long int context);
void	trap_BotMatchVariable(void* match, int variable, char* buf, int size);
void	trap_UnifyWhiteSpaces(char* string);
void	trap_BotReplaceSynonyms(char* string, unsigned long int context);
int		trap_BotLoadChatFile(int chatstate, char* chatfile, char* chatname);
void	trap_BotSetChatGender(int chatstate, int gender);
void	trap_BotSetChatName(int chatstate, char* name);
void	trap_BotResetGoalState(int goalstate);
void	trap_BotRemoveFromAvoidGoals(int goalstate, int number);
void	trap_BotResetAvoidGoals(int goalstate);
void	trap_BotPushGoal(int goalstate, void* goal);
void	trap_BotPopGoal(int goalstate);
void	trap_BotEmptyGoalStack(int goalstate);
void	trap_BotDumpAvoidGoals(int goalstate);
void	trap_BotDumpGoalStack(int goalstate);
void	trap_BotGoalName(int number, char* name, int size);
int		trap_BotGetTopGoal(int goalstate, void* goal);
int		trap_BotGetSecondGoal(int goalstate, void* goal);
int		trap_BotChooseLTGItem(int goalstate, vec3_t origin, int* inventory, int travelflags, qboolean botRoamsOnly );
int		trap_BotChooseNBGItem(int goalstate, vec3_t origin, int* inventory, int travelflags, void* ltg, float maxtime, qboolean botRoamsOnly);
int		trap_BotTouchingGoal(vec3_t origin, void* goal);
int		trap_BotItemGoalInVisButNotVisible(int viewer, vec3_t eye, vec3_t viewangles, void* goal);
int		trap_BotGetNextCampSpotGoal(int num, void* goal);
int		trap_BotGetMapLocationGoal(char* name, void* goal);
int		trap_BotGetLevelItemGoal(int index, char* classname, void* goal);
float	trap_BotAvoidGoalTime(int goalstate, int number);
void	trap_BotInitLevelItems(void);
void	trap_BotUpdateEntityItems(void);
int		trap_BotLoadItemWeights(int goalstate, char* filename);
void	trap_BotFreeItemWeights(int goalstate);
void	trap_BotInterbreedGoalFuzzyLogic(int parent1, int parent2, int child);
void	trap_BotSaveGoalFuzzyLogic(int goalstate, char* filename);
void	trap_BotMutateGoalFuzzyLogic(int goalstate, float range);
int		trap_BotAllocGoalState(int state);
void	trap_BotFreeGoalState(int handle);

void	trap_BotResetMoveState(int movestate);
void	trap_BotMoveToGoal(void* result, int movestate, void* goal, int travelflags);
int		trap_BotMoveInDirection(int movestate, vec3_t dir, float speed, int type);
void	trap_BotResetAvoidReach(int movestate);
void	trap_BotResetLastAvoidReach(int movestate);
int		trap_BotReachabilityArea(vec3_t origin, int testground);
int		trap_BotMovementViewTarget(int movestate, void* goal, int travelflags, float lookahead, vec3_t target);
int		trap_BotPredictVisiblePosition(vec3_t origin, int areanum, void* goal, int travelflags, vec3_t target);
int		trap_BotAllocMoveState(void);
void	trap_BotFreeMoveState(int handle);
void	trap_BotInitMoveState(int handle, void* initmove);

int		trap_BotChooseBestFightWeapon(int weaponstate, int* inventory, qboolean meleeRange);
void	trap_BotGetWeaponInfo(int weaponstate, int weapon, void* weaponinfo);
int		trap_BotLoadWeaponWeights(int weaponstate, char* filename);
int		trap_BotAllocWeaponState(void);
void	trap_BotFreeWeaponState(int weaponstate);
void	trap_BotResetWeaponState(int weaponstate);

int		trap_GeneticParentsAndChildSelection(int numranks, float* ranks, int* parent1, int* parent2, int* child);

/*=================*\
|| RPG-X Functions ||
\*=================*/

qboolean LineOfSight( gentity_t* ent1, gentity_t* ent2 ); //Phenix
qboolean IsAdmin( gentity_t* ent );
qboolean IsBorg( gentity_t* ent ); //GSIO01 | 08/05/2009

//TiM - load usables scan text
qboolean G_SetupUsablesStrings( void );

//RPG-X: Marcin: for easier lookup - 30/12/2008
int Max_Weapon(int num);
int Min_Weapon(int num);
//! Max ammount of weapons a player can have for each weapons
extern vmCvar_t* Max_Weapons[];
//! Min ammount of weapons a player can have for each weapons
extern vmCvar_t* Min_Weapons[];

// SP support
void Touch_Multi(gentity_t* self, gentity_t* other, trace_t* trace);
void Use_Multi(gentity_t* ent, gentity_t* other, gentity_t* activator);

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
const char* BuildShaderStateConfig(void);

typedef struct luaAlertState_s luaAlertState_t;
struct luaAlertState_s {
	int cond;
	int mode;
	char* shaders[4];
	char* sounds[4];
	char* targets[4];
};

luaAlertState_t* luaAlertState;

/* alert shaders */
typedef struct {
	char*	greenShaders[10];
	char*	redShaders[10];
	char*	yellowShaders[10];
	char*	blueShaders[10];
	int		numShaders;
} target_alert_Shaders_s;

#endif //_G_LOCAL_H_
