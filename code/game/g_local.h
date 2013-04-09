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
*	Pointer to \link gentity_s \endlink
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

	struct gclient_s	*client;	// NULL if not a client

	qboolean	inuse;

	char*		classname;			//!< set in QuakeEd
	int			spawnflags;			//!< set in QuakeEd

	qboolean	neverFree;			//!< if true, FreeEntity will only unlink bodyque uses this

	int			flags;				//!< FL_* variables

	char*		model;				//!< the model or brushmodel the entities uses
	char*		model2;			//!< an alternate model
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
	gentity_t*	parent;			//!< parent of this entity
	gentity_t*	nextTrain;			//!< next path_corner of a func_train
	gentity_t*	prevTrain;			//!< previous path_corner of a func_train
	vec3_t		pos1;				//!< start position for binary movers
	vec3_t		pos2;				//!< end position for binary movers
	vec3_t		apos1;				//!< angular start position for movers
	vec3_t		apos2;				//!< angular end position for movers

	char*		message;			//!< message for target_print

	int			timestamp;			//!< body queue sinking, etc

	float		angle;				//!< set in editor, -1 = up, -2 = down
	char*		target;			//!< target of the entity
	char*		paintarget;		//!< target to use if entity takes damage
	char*		targetname;		//!< targetname that identyfies the entity
	char*		team;				//!< for teamchains
	gentity_t*	target_ent;		//!< target of turrets etc

	float		speed;				//!< moving speed etc
	vec3_t		movedir;			//!< moving direction

	int			nextthink;			//!< next level.time the entities think functions gets called
	void		(*think)(gentity_t *self);	//!< think function
	void		(*reached)(gentity_t *self);	//!< movers call this when hitting endpoint
	void		(*blocked)(gentity_t *self, gentity_t *other); //!< movers call this when blocked
	void		(*touch)(gentity_t *self, gentity_t *other, trace_t *trace); //!< touch function for triggers
	void		(*use)(gentity_t *self, gentity_t *other, gentity_t *activator); //!< function that gets called if the entity is used
	void		(*pain)(gentity_t *self, gentity_t *attacker, int damage); //!< function that gets called if entity gets damged
	void		(*die)(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod); //!< function that gets called if entity dies

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

	gentity_t*	lastEnemy;			//!< last enemy of the entity (turrets etc)
	gentity_t*	enemy;				//!< current enemy of the entity (turrets etc)
	gentity_t*	activator;			//!< entity that activated/used this entity
	gentity_t*	teamchain;			//!< next entity in team
	gentity_t*	teammaster;		//!< master of the team

	int			watertype;
	int			waterlevel;

	int			noise_index;

	int			voiceChatSquelch;
	int			voiceChatPreviousTime;

	// timing variables
	float		wait;				//!< how long to wait
	float		random;

	gitem_t*	item;				//!< for bonus items

	qboolean	botDelayBegin;

	char*		swapname;			//RPG-X Modification | Phenix | 13/06/2004
	char*		truename;
	char*		falsename;
	char*		truetarget;
	char*		falsetarget;
	qboolean	booleanstate;	

	float		distance;		// VALKYRIE: for rotating doors

	int			n00bCount;	//RPG-X | Phenix | 06/04/2004 | For when people kill

	//RPG-X: TiM : Additional Params for map ents
	char*		targetname2;	// GSIO01 | 08/05/2009

	gentity_t*	touched;		// GSIO01 | 08/05/2009 repairing breakables
	
	//GSIO01 | 10/05/2009 | ok these are for target_alert:
	char*		bluename;
	char*		greensound;
	char*		yellowsound;
	char*		redsound;
	char*		bluesound;

	char		*targetShaderName;		//!< shader to remap for shader remapping
	char		*targetShaderNewName;	//!< shader to remap to for shader remapping

	qboolean	tmpEntity;				//!< is this a temporal entity?

#ifdef G_LUA
	// for lua hooks
	// pointers to lua functions
	char*		luaTouch;
	char*		luaUse;
	char*		luaThink;
	char*		luaHurt;
	char*		luaDie;
	char*		luaFree;
	char*		luaTrigger;
	char*		luaReached;
	char*		luaReachedAngular;
	char*		luaSpawn;
	char*		luaParm1;
	char*		luaParm2;
	char*		luaParm3;
	char*		luaParm4;
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
	CON_DISCONNECTED,
	CON_CONNECTING,
	CON_CONNECTED
} clientConnected_t;

/** \enum spectatorState_t
*
*	Diffrent spectator modes.
*/
typedef enum {
	SPECTATOR_NOT,
	SPECTATOR_FREE,
	SPECTATOR_FOLLOW,
	SPECTATOR_SCOREBOARD
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
*	For example number of flag captures.
*/
typedef struct {
	playerTeamStateState_t	state;

	int			location;

	int			captures;
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
	struct gclient_s*	clients;	/*!< level clients */

	struct gentity_s*	gentities;			/*! level gentities */
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
	char*				changemap;				/*!< ??? */
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

	list_p				safezones;								/*!< self destruct safezones list */
	list_p				locations;								/*!< level locations list */
	list_p				timedMessages;							/*!< timed messages list */

	// other stuff
	srvChangeData_t		srvChangeData;							/*!< Server change data */

	// override rpg_calcLiftTravelDuration
	int					overrideCalcLiftTravelDuration;			/*!< Indicated whether the level author wants to override rpg_calcLiftTravelDuration */
} level_locals_t;


//
// g_spawn.c
//
qboolean	G_SpawnString( const char* key, const char* defaultString, char** out );
// spawn string returns a temporary reference, you must CopyString() if you want to keep it
qboolean	G_SpawnFloat( const char* key, const char* defaultString, float* out );
qboolean	G_SpawnInt( const char* key, const char* defaultString, int* out );
qboolean	G_SpawnVector( const char* key, const char* defaultString, float* out );
void		G_SpawnEntitiesFromString( void );
char*		G_NewString( const char* string );

//
// Flash Light / Laser
//
void Laser_Gen (gentity_t* ent, int type);
void Laser_Think( gentity_t* self );


//
// g_cmds.c
//
char*		ConcatArgs( int start );
void		Cmd_Score_f (gentity_t* ent);
void		StopFollowing( gentity_t* ent );
void		BroadcastTeamChange( gclient_t* client, int oldTeam );
qboolean	SetTeam( gentity_t* ent, char* s );
void		Cmd_FollowCycle_f( gentity_t* ent, int dir );
void		Cmd_Ready_f (gentity_t* ent);

//
// g_roff.c
//
int		G_LoadRoff( const char* fileName );
void	G_Roff( gentity_t* ent );

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

#define PADD_DATA_MAX	256 // max number of padds
#define MAX_DROPPED		255 // should be reasonable

extern paddData_t	paddData[PADD_DATA_MAX];
extern int			paddDataNum;
extern int			numTotalDropped;

void	Padd_Add( gentity_t* key, gentity_t* who, char* txt);
char*	Padd_Get( gentity_t* key, gentity_t* who );
void	Padd_Remove( gentity_t* key );

void	G_RunItem( gentity_t* ent );
void	RespawnItem( gentity_t* ent );

void		UseHoldableItem( gentity_t* ent );
void		PrecacheItem (gitem_t* it);
gentity_t*	Drop_Item( gentity_t* ent, gitem_t* item, float angle );
void		SetRespawn (gentity_t* ent, float delay);
void		G_SpawnItem (gentity_t* ent, gitem_t* item);
void		FinishSpawningItem( gentity_t* ent );
void		Think_Weapon (gentity_t* ent);
int			ArmorIndex (gentity_t* ent);
void		Add_Ammo (gentity_t* ent, int weapon, int count);
void		Touch_Item (gentity_t* ent, gentity_t* other, trace_t* trace);

void	ClearRegisteredItems( void );
void	RegisterItem( gitem_t* item );
void	SaveRegisteredItems( void );

//
// g_utils.c
//
int		G_ModelIndex( char* name );
int		G_SoundIndex( char* name );
void	G_TeamCommand( team_t team, char* cmd );
int		G_TricStringIndex( char* name ); //TiM
void	G_KillBox (gentity_t* ent);
//RPG-X: J2J - Added to move enities out of a transporting player area.
qboolean	G_MoveBox (gentity_t* ent);
gentity_t*	G_Find (gentity_t* from, int fieldofs, const char* match);
gentity_t*	G_PickTarget (char* targetname);
//TiM
//gentity_t *G_PickEntity ( char* className, int fieldofs, const char *match );
void	G_UseTargets (gentity_t* ent, gentity_t* activator);
void	G_UseTargets2( gentity_t* ent, gentity_t* activator, char* target );
void	G_SetMovedir ( vec3_t angles, vec3_t movedir);

void		G_InitGentity( gentity_t* ent );
gentity_t*	G_Spawn (void);
gentity_t*	G_TempEntity( vec3_t origin, int event );
void		G_Sound( gentity_t* ent, int soundIndex );
void		G_FreeEntity( gentity_t* ent );

void		G_TouchTriggers (gentity_t* ent);
void		G_TouchSolids (gentity_t* ent);

float*		tv (float x, float y, float z);
char*		vtos( const vec3_t v );

float vectoyaw( const vec3_t vec );

void		G_AddPredictableEvent( gentity_t* ent, int event, int eventParm );
void		G_AddEvent( gentity_t* ent, int event, int eventParm );
void		G_SetOrigin( gentity_t* ent, vec3_t origin );
void		G_SetAngles( gentity_t* ent, vec3_t anlges ); //RPG-X | GSIO01 | 24.08.2009
int			G_RadiusList ( vec3_t origin, float radius,	gentity_t* ignore, qboolean takeDamage, gentity_t* ent_list[MAX_GENTITIES]);
int			G_RadiusListOfTypes(char* classname[], int count, vec3_t origin, float radius, gentity_t* ignore, list_p ent_list);
gentity_t*	G_GetNearestEnt(char* classname, vec3_t origin, float radius, gentity_t* ignore, qboolean takeDamage);
gentity_t*	G_GetNearestPlayer(vec3_t origin, float radius, gentity_t* ignore );

// GSIO - additional util funcs to make life easier with spawnfile
int G_GetEntityByTargetname(const char* targetname, gentity_t* entities[MAX_GENTITIES]);
int G_GetEntityByTarget(const char* target, gentity_t* entities[MAX_GENTITIES]);
int G_GetEntityByBmodel(char* bmodel, gentity_t* entities[MAX_GENTITIES]);

/* shader remapping */
void AddRemap(const char* oldShader, const char* newShader, float timeOffset);

//
// g_combat.c
//
qboolean	CanDamage (gentity_t* targ, vec3_t origin);
void		G_Damage (gentity_t* targ, gentity_t* inflictor, gentity_t* attacker, vec3_t dir, vec3_t point, int damage, int dflags, int mod);
qboolean	G_RadiusDamage (vec3_t origin, gentity_t* attacker, float damage, float radius, gentity_t* ignore, int dflags, int mod);
void		body_die( gentity_t* self, gentity_t* inflictor, gentity_t* attacker, int damage, int meansOfDeath );
void		TossClientItems( gentity_t* self, qboolean dis_con );
void		G_Repair(gentity_t* ent, gentity_t* tr_ent, float rate); //RPG-X | GSIO01 | 09/05/2009

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
void		G_Missile_Run( gentity_t* ent );
gentity_t*	fire_blaster (gentity_t* self, vec3_t start, vec3_t aimdir);
gentity_t*	fire_plasma (gentity_t* self, vec3_t start, vec3_t aimdir);
gentity_t*	fire_quantum (gentity_t* self, vec3_t start, vec3_t aimdir);
gentity_t*	fire_grenade (gentity_t* self, vec3_t start, vec3_t aimdir);
gentity_t*	fire_rocket (gentity_t* self, vec3_t start, vec3_t dir);
gentity_t*	fire_grapple (gentity_t* self, vec3_t start, vec3_t dir);
gentity_t*	fire_comprifle (gentity_t* self, vec3_t start, vec3_t dir);



//
// g_mover.c
//
void G_Mover_Run( gentity_t* ent );
void G_Mover_TouchDoorTrigger( gentity_t* ent, gentity_t* other, trace_t* trace );
void G_Mover_UseBinaryMover( gentity_t* ent, gentity_t* other, gentity_t* activator );

//
// g_trigger.c
//
void trigger_teleporter_touch (gentity_t* self, gentity_t* other, trace_t* trace );


//
// g_misc.c
//
void TeleportPlayer( gentity_t* player, vec3_t origin, vec3_t angles, tpType_t tyType );
void TransportPlayer( gentity_t* player, vec3_t origin, vec3_t angles, int speed );

//TiM - g_target.c
void target_turbolift_start( gentity_t* ent );

//
// g_weapon.c
//
qboolean	G_Weapon_LogAccuracyHit( gentity_t* target, gentity_t* attacker );
void		G_Weapon_CalcMuzzlePoint ( gentity_t* ent, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint, float projsize);
void		G_Weapon_SnapVectorTowards( vec3_t v, vec3_t to );

//
// g_client.c
//
/**
 * Get number of clients in team.
 */
team_t		G_Client_TeamCount( int ignoreClientNum, int team );
/**
 * Pick a random team.
 */
team_t		G_Client_PickTeam( int ignoreClientNum );
/**
 * Set the clients view angle.
 */
void		G_Client_SetViewAngle( gentity_t* ent, vec3_t angle );
/**
 * Select a spawnpoint.
 */
gentity_t*	G_Client_SelectSpawnPoint ( vec3_t avoidPoint, vec3_t origin, vec3_t angles );
/**
 * Respawn client.
 */
void		G_Client_Respawn(gentity_t *ent);
/**
 * Begin intermission.
 */
void		G_Client_BeginIntermission(void);
/**
 * Init the body que.
 */
void		G_Client_InitBodyQue(void);
/**
 * Spawn client.
 */
void		G_Client_Spawn( gentity_t* ent, int rpgx_spawn, qboolean fromDeath );
/**
 * Let the client die.
 */
void		G_Client_Die (gentity_t* self, gentity_t* inflictor, gentity_t* attacker, int damage, int mod);
/**
 * Add score for the client.
 */
void		G_Client_AddScore( gentity_t* ent, int score );
/**
 * Calculate clients ranks.
 */
void		G_Client_CalculateRanks( qboolean fromExit );
/**
 * Determine whether spot would telefrag.
 */
qboolean	G_Client_SpotWouldTelefrag( gentity_t* spot );
/**
 * Get weapons for a class.
 */
void		G_Client_WeaponsForClass( gclient_t* client, pclass_t pclass );
/**
 * Get holdable items for a class.
 */
void		G_Client_HoldablesForClass( gclient_t* client, pclass_t pclass );
/**
 * Store the clients initial status.
 */
void		G_Client_StoreClientInitialStatus( gentity_t* ent );
/**
 *	Get location message for a client.
 */
qboolean	G_Client_GetLocationMsg(gentity_t* ent, char* loc, int loclen);
/**
 * Check client statuses.
 */
void		G_Client_CheckClientStatus(void);
/**
 * Send client location information.
 */
void		G_Client_LocationsMessage( gentity_t *ent );
/**
 * Client connect.
 */
char* G_Client_Connect( int clientNum, qboolean firstTime, qboolean isBot );
/**
 * Clients user info changed.
 */
void G_Client_UserinfoChanged( int clientNum );
/**
 * Disconnect client.
 */
void G_Client_Disconnect( int clientNum );
/**
 * Initialize client.
 */
void G_Client_Begin( int clientNum, qboolean careAboutWarmup, qboolean isBot, qboolean first );
/**
 * Client command.
 */
void G_Client_Command( int clientNum );

//
// g_svcmds.c
//
qboolean	ConsoleCommand( void ); //void
void		G_ProcessIPBans(void);
qboolean	G_FilterPacket (char* from);
gclient_t*	ClientForString( const char* s ); //RPG-X: RedTechie - Added so g_cmds could read the function
//TiM
void		G_ProcessIDBans( void );
qboolean	CheckID( char*	id );

//
// g_weapon.c
//
void FireWeapon( gentity_t* ent, qboolean alt_fire );
void steam_think( gentity_t* ent );

//
// p_hud.c
//
void MoveClientToIntermission (gentity_t* client);
void G_SetStats (gentity_t* ent);
void DeathmatchScoreboardMessage (gentity_t* client);

//
// g_fx.c
//
void SP_fx_spark( gentity_t* ent );

//
// g_cmds.c
//
//static void G_SayTo( gentity_t *ent, gentity_t *other, int mode, int color, const char *name, const char *message );

//
// g_pweapon.c
//


//
// g_main.c
//
void FindIntermissionPoint( void );
void G_RunThink (gentity_t* ent);
void SendScoreboardMessageToAllClients( void );
void QDECL G_Printf( const char* fmt, ... ) __attribute__ ((format (printf, 1, 2)));
void QDECL G_Error( const char* fmt, ... ) __attribute__ ((format (printf, 1, 2)));
void QDECL G_PrintfClient( gentity_t* ent, const char* fmt, ...) __attribute__ ((format (printf, 2, 3)));
void QDECL G_PrintfClientAll(const char* fmt, ...) __attribute__ ((format (printf, 1, 2)));

//
// g_sql.c
//
extern		qboolean sql_ready;
qboolean	G_Sql_Init(void);
void		G_Sql_Shutdown(void);
qboolean	G_Sql_UserDB_Add(const char* uName, const char* password);
qboolean	G_Sql_UserDB_CheckRight(int uid, int right);
qboolean	G_Sql_UserDB_AddRight(int uid, int right);
qboolean	G_Sql_UserDB_RemoveRight(int uid, int right);
qboolean	G_Sql_UserDB_Del(const char* uName);
qboolean	G_Sql_UserDB_Login(const char* uName, const char* pwd, int clientnum);
int			G_Sql_UserDB_GetUID(const char* uName);

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
	int					beamTime;								//!< Time the beam was initialized. 4 seconds after this, the player will teleport, 8 seconds later the effect will reset

	transVecData_t		currentCoord;			//!< If they are beaming, have the active variables placed here
	transVecData_t		storedCoord[TPT_MAX];	//!< Any separate beam data (ie, tricorder, portable beam etc) is kept here till needed.

	int					LastClick;								//!< Last click (in level time) when feature was used (used to prevent accidents)
	qboolean			beamed;							//!< Once we've called the teleport, make this true.  Or else we'll teleport on every clock cycle >.<
} RPGX_SiteTOSiteData;

/*typedef struct
{
	int	beamTime;								//TiM - Time the beam was initialized. 4 seconds after this, the player will teleport, 8 seconds later the effect will reset

	vec3_t	currentCoord, TransCoordRot;		//If they are beaming, have the active variables placed here

	vec3_t TransCoord, TransCoordRot;			//Transporter Coordinate and rotation values
	int	beamer;									//TiM - Client num to reference the array where the necessary beam data is
	int LastClick;								//Last click (in level time) when feature was used (used to prevent accidents)
	qboolean Used;								//False for first use

	//same again but for personal transporter
	vec3_t pTransCoord, pTransCoordRot;
	qboolean pActivated;
	qboolean pUsed;

	qboolean	beamed;							//Once we've called the teleport, make this true.  Or else we'll teleport on every clock cycle >.<
}
RPGX_SiteTOSiteData;*/

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

gentity_t* SelectRandomSpawnPoint( void );		//Added so it can be used in main.

//RPG-X Misc Sounds:
//extern sfxHandle_t n00bsnd;

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
	char	model[MAX_QPATH];
	team_t	team;
	pclass_t pClass;
	qboolean initialized;
} clInitStatus_t;

//
// g_active.c
//

void ClientThink( int clientNum ); // TODO move me to g_client.c
void ClientEndFrame( gentity_t* ent ); // TODO move me to g_client.c
void G_RunClient( gentity_t* ent ); // TODO move me to g_client.c
void G_Active_ShieldRemove(gentity_t* self);

//RPG-X | Marcin | 03/12/2008
void ThrowWeapon( gentity_t* ent, char* txt );
gentity_t *DropWeapon( gentity_t* ent, gitem_t* item, float angle, int flags, char* txt );

//
// g_mem.c
//
void*	G_Alloc( int size );
void	G_InitMemory( void );
void	Svcmd_GameMem_f( void );

//
// g_session.c
//
void G_ReadSessionData( gclient_t* client );
void G_InitSessionData( gclient_t* client, char* userinfo );

void G_InitWorldSession( void );
void G_WriteSessionData( void );

//
// g_arenas.c
//
void UpdateTournamentInfo( void );
void SpawnModelsOnVictoryPads( void );
void Svcmd_AbortPodium_f( void );

//
// g_bot.c
//
void		G_InitBots( qboolean restart );
char*		G_GetBotInfoByNumber( int num );
char*		G_GetBotInfoByName( const char* name );
void		G_CheckBotSpawn( void );
void		G_QueueBotBegin( int clientNum );
qboolean	G_BotConnect( int clientNum, qboolean restart );
void		Svcmd_AddBot_f( void );
void		Svcmd_BotList_f( void );
void		BotInterbreedEndMatch( void );

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
	char	characterfile[MAX_FILEPATH];
	int		skill;
	char	team[MAX_FILEPATH];
	char	pclass[MAX_FILEPATH];
} bot_settings_t;

int BotAISetup( int restart );
int BotAIShutdown( int restart );
int BotAILoadMap( int restart );
int BotAISetupClient( int client, bot_settings_t* settings );
int BotAIShutdownClient( int client );
int BotAIStartFrame( int time );


//
//	g_lua.c
//
#ifdef G_LUA
void		LuaHook_G_InitGame(int leveltime, int randomseed, int restart);
void		LuaHook_G_Shutdown(int restart);
void		LuaHook_G_RunFrame(int leveltime);
void		LuaHook_G_Print(char* text);
void		LuaHook_G_ClientPrint(char* text, int entnum);
qboolean	LuaHook_G_EntityThink(char* function, int entnum);
qboolean	LuaHook_G_EntityTouch(char* function, int entnum, int othernum);
qboolean	LuaHook_G_EntityUse(char* function, int entnum, int othernum, int activatornum);
qboolean	LuaHook_G_EntityHurt(char* function, int entnum, int inflictornum, int attackernum);
qboolean	LuaHook_G_EntityDie(char* function, int entnum, int inflictornum, int attackernum, int dmg, int mod);
qboolean	LuaHook_G_EntityFree(char* function, int entnum);
qboolean	LuaHook_G_EntityTrigger(char* function, int entnum, int othernum);
qboolean	LuaHook_G_EntitySpawn(char* function, int entnum);
qboolean	LuaHook_G_EntityReached(char* function, int entnum);
qboolean	LuaHook_G_EntityReachedAngular(char* function, int entnum);
void		G_LuaNumThreads(void);
void		G_LuaCollectGarbage(void);

void		G_LuaStatus(gentity_t* ent);
qboolean	G_LuaInit(void);
void		G_LuaShutdown(void);

extern vmCvar_t g_debugLua;
extern vmCvar_t lua_allowedModules;
extern vmCvar_t lua_modules;
#endif

extern vmCvar_t g_developer;
// developer tools:
// Prevents a single statement from beeing executed if developer isn't set
#define DEVELOPER(X) if(g_developer.integer != 0) { \
						X\
					 } else


#include "g_team.h" // teamplay specific stuff


extern	level_locals_t	level;
extern	gentity_t		g_entities[MAX_GENTITIES];

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

void	trap_Printf( const char* fmt );
/**
*	Calls Com_error in the engine
*	\param fmt error desription
*/
void	trap_Error( const char* fmt );
/**
*	Get milliseconds since engine start
*	\return milliseconds since engine start
*/
int		trap_Milliseconds( void );
/**
*	Get count of arguments for the current client game command
*	\return count of arguments
*/
int		trap_Argc( void );
/**
*	Get a n of the current client game command
*	\param n argument to get
*	\param buffer buffer to store the argument in
*	\param bufferLength size of the buffer
*/
void	trap_Argv( int n, char* buffer, int bufferLength );
/**
*	Get all args of the current client game command
*	\param buffer buffer to store the arguments in
*	\param bufferLength size of the buffer
*/
void	trap_Args( char* buffer, int bufferLength );
/**
*	Opens a file
*	\param qpath path and filename
*	\param f filehandle to use
*	\param mode mode to use
*/
int		trap_FS_FOpenFile( const char* qpath, fileHandle_t* f, fsMode_t mode );
/**
*	Read a opened file
*	\param buffer buffer to read to
*	\param len buffer length or length to read (<sizeof(buffer))
*	\param f filehandle for the file to read from
*
*	You have to open the file first.
*/
void	trap_FS_Read( void* buffer, int len, fileHandle_t f );
/**
*	Write to a file
*	\param buffer text to write
*	\param len length of buffer
*	\param f filehandle  for the file to write to
*	
*	You have to open the file first.
*/
void	trap_FS_Write( const void* buffer, int len, fileHandle_t f );
/**
*	Close a file
*	\param f filehandle for file to close
*/
void	trap_FS_FCloseFile( fileHandle_t f );
/**
*	Get a list of files in a path
*	\param path path to get the list for
*	\param extension filter to get only files with this extension
*	\param listbuf buffer to store the file list in
*	\param bufsize size of the buffer
*	\return number of files in the list
*/
int		trap_FS_GetFileList( const char* path, const char* extension, char* listbuf, int bufsize );
/**
*	Sends a console command to execute to the client console
*	\param exec_when when to exec (e.g. EXEC_APPEND)
*	\param text the command to execute
*/
void	trap_SendConsoleCommand( int exec_when, const char *text );
/**
*	Register a cvar
*	\param cvar representation of the cvar in the vm
*	\param var_name name of the cvar
*	\param value default value for the cvar
*	\param flags additional options for the cvar (e.g. CVAR_ARCHIVE)
*/
void	trap_Cvar_Register( vmCvar_t* cvar, const char* var_name, const char* value, int flags );
/**
*	\brief Update a cvar.
*
*	Tells the server/engine that a cvar in the vm has changed.
*	\param cvar cvar to update
*/
void	trap_Cvar_Update( vmCvar_t* cvar );
/**
*	Set the cvar to a value.
*	\param var_name name of the cvar to set
*	\param value new value for the cvar
*/
void	trap_Cvar_Set( const char* var_name, const char* value );
/**
*	Get the integer value for an cvar
*	\param var_name name of the cvar
*/
int		trap_Cvar_VariableIntegerValue( const char* var_name );
/**
*	Get the value of the cvar as string
*	\param var_name name of the cvar
*	\param buffer to store the value
*	\param bufsize size of the buffer
*/
void	trap_Cvar_VariableStringBuffer( const char* var_name, char* buffer, int bufsize );
/**
*	Send some information of the current game/map to the server
*	\param pointer to level.entities which is g_entities
*	\param numGEntities number of game entities (level.num_entities)
*	\param sizeofGEntity_t size of gentity_t
*	\param gameClients level.clients[0].ps
*	\param sizeOfGameClient size of level.clients[0]
*/
void	trap_LocateGameData( gentity_t* gEnts, int numGEntities, int sizeofGEntity_t, playerState_t* gameClients, int sizeofGameClient );
/**
*	Drop a client from server.
*	\param clientNum client number of client to drop
*	\param test reason for client drop
*/
void	trap_DropClient( int clientNum, const char* reason );
/**
*	\brief Send a server command to the client
*	\param clientNum client number of client
*	\param server command to execute
*
*	A value of -1 for clientNum will send the command to all clients.
*/
void	trap_SendServerCommand( int clientNum, const char* text );
/**
*	Set a configstring
*	\param num CS_...
*	\param string set cofig string to this
*/
void	trap_SetConfigstring( int num, const char* string );
/**
*	Get a configstring
*	\param num CS_...
*	\param buffer buffer to store config string in
*	\param bufferSize size of buffer
*/
void	trap_GetConfigstring( int num, char* buffer, int bufferSize );
/**
*	Get the userinfo for a client
*	\param num client number
*	\param buffer buffer to store config string in
*	\param size of buffer
*/
void	trap_GetUserinfo( int num, char* buffer, int bufferSize );
/**
*	Set the userinfo for a client
*	\param num client number
*	\param buffer string the contains new userinfo
*/
void	trap_SetUserinfo( int num, const char* buffer );
/**
*	Get server info.
*	\param buffer buffer to store the info in
*	\param bufferSize size of buffer
*/
void	trap_GetServerinfo( char* buffer, int bufferSize );
/**
*	\brief Set the brush model for a entity.
*	\param ent entity to the the model on
*	\param name the model name
*
*	The normal case is trap_SetBrushModel(ent, ent->model).
*	Brush models always have names of *<int>.
*/
void	trap_SetBrushModel( gentity_t* ent, const char* name );
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
void	trap_Trace( trace_t* results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
/**
*	\param point the point
*	\param passEntityNum ingore this
*
*	Works similar to a trace but only check a single point.
*/
int			trap_PointContents( const vec3_t point, int passEntityNum );
qboolean	trap_InPVS( const vec3_t p1, const vec3_t p2 );
qboolean	trap_InPVSIgnorePortals( const vec3_t p1, const vec3_t p2 );
/**
*	Adjust the state of a area portal used for doors etc
*	\param ent entity that effects the areaportal (area portal is inide the entities bounds)
*	\param open open or close it?
*/
void	trap_AdjustAreaPortalState( gentity_t* ent, qboolean open );
/**
*	Checks if two areas are connected.
*/
qboolean trap_AreasConnected( int area1, int area2 );
/**
*	Link an entity.
*	This results in shared values beeing avaible on both game and client side.
*/
void	trap_LinkEntity( gentity_t* ent );
/**
*	Unlinks an entity.
*/
void	trap_UnlinkEntity( gentity_t* ent );
/**
*	\brief Get a list of all entities in a box.
*	\param entityList list where entitynums will be stored
*	\param maxcount limits the number of listed entities
*
*	The size of the box is defined by mins and maxs.
*/
int		trap_EntitiesInBox( const vec3_t mins, const vec3_t maxs, int* entityList, int maxcount );
/**
*	Checks if a entity is in contact with a defined box.
*/
qboolean trap_EntityContact( const vec3_t mins, const vec3_t maxs, const gentity_t* ent );
/**
*	Allocates a free client for a bot.
*/
int		trap_BotAllocateClient( void );
/**
*	Free the client that was used for a bot.
*/
void	trap_BotFreeClient( int clientNum );
/**
*	Get the last command a user did.
*/
void		trap_GetUsercmd( int clientNum, usercmd_t* cmd );
qboolean	trap_GetEntityToken( char* buffer, int bufferSize );

int		trap_DebugPolygonCreate(int color, int numPoints, vec3_t* points);
void	trap_DebugPolygonDelete(int id);

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
