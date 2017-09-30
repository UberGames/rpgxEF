// Copyright (C) 1999-2000 Id Software, Inc.
//
/*****************************************************************************
 * name:		botlib.h
 *
 * desc:		bot AI library
 *
 * $Archive: /StarTrek/Code-DM/game/botlib.h $
 * $Author: Mgummelt $
 * $Revision: 4 $
 * $Modtime: 3/12/01 3:08p $
 * $Date: 3/12/01 4:43p $
 *
 *****************************************************************************/

#ifndef BOTLIB_H_
#define BOTLIB_H_

#ifdef _MSC_VER
#include <cstddef>
#endif

#include <cstdint>

static const uint32_t BOTLIB_API_VERSION = 2;

struct aas_clientmove_s;
struct aas_entityinfo_s;
struct bot_consolemessage_s;
struct bot_match_s;
struct bot_goal_s;
struct bot_moveresult_s;
struct bot_initmove_s;
struct weaponinfo_s;

//debug line colors
static const int32_t  LINECOLOR_NONE	= -1;
static const uint32_t LINECOLOR_RED		= 1; //0xf2f2f0f0L
static const uint32_t LINECOLOR_GREEN	= 2; //0xd0d1d2d3L
static const uint32_t LINECOLOR_BLUE	= 3; //0xf3f3f1f1L
static const uint32_t LINECOLOR_YELLOW	= 4; //0xdcdddedfL
static const uint32_t LINECOLOR_ORANGE	= 5; //0xe0e1e2e3L

//Print types
typedef enum {
	PRT_MESSAGE = 1,
	PRT_WARNING,
	PRT_ERROR,
	PRT_FATAL,
	PRT_EXIT
} botlib_PrintType;

//console message types
typedef enum {
	CMS_NORMAL,
	CMS_CHAT
} botlib_ConsoleMessageTpye;

//botlib error codes
typedef enum {
	BLERR_NOERROR,					//!<no error
	BLERR_LIBRARYNOTSETUP,			//!<library not setup
	BLERR_INVALIDENTITYNUMBER,		//!<invalid entity number
	BLERR_NOAASFILE,				//!<no AAS file available
	BLERR_CANNOTOPENAASFILE,		//!<cannot open AAS file
	BLERR_WRONGAASFILEID,			//!<incorrect AAS file id
	BLERR_WRONGAASFILEVERSION,		//!<incorrect AAS file version
	BLERR_CANNOTREADAASLUMP,		//!<cannot read AAS file lump
	BLERR_CANNOTLOADICHAT,			//!<cannot load initial chats
	BLERR_CANNOTLOADITEMWEIGHTS,	//!<cannot load item weights
	BLERR_CANNOTLOADITEMCONFIG,		//!<cannot load item config
	BLERR_CANNOTLOADWEAPONWEIGHTS,	//!<cannot load weapon weights
	BLERR_CANNOTLOADWEAPONCONFIG,	//!<cannot load weapon config
} botlib_ErrorCode;

//action flags
typedef enum {
	ACTION_ATTACK = 1,
	ACTION_USE = 2,
	ACTION_RESPAWN = 4,
	ACTION_JUMP = 8,
	ACTION_MOVEUP = 8,
	ACTION_CROUCH = 16,
	ACTION_MOVEDOWN = 16,
	ACTION_MOVEFORWARD = 32,
	ACTION_MOVEBACK = 64,
	ACTION_MOVELEFT = 128,
	ACTION_MOVERIGHT = 256,
	ACTION_DELAYEDJUMP = 512,
	ACTION_TALK = 1024,
	ACTION_GESTURE = 2048,
	ACTION_WALK = 4096,
	ACTION_ALT_ATTACK = 8192
} botlib_ActionFlag;

/**
*	the bot input, will be converted to an usercmd_t
*/
typedef struct bot_input_s {
	double thinktime;		//!<time since last output (in seconds)
	vec3_t dir;				//!<movement direction
	double speed;			//!<speed in the range [0, 400]
	vec3_t viewangles;		//!<the view angles
	int32_t actionflags;		//!<one of the ACTION_? flags
	int32_t weapon;				//!<weapon to use
} bot_input_t;

#ifndef BSPTRACE

#define BSPTRACE

/**
*	bsp_trace_t hit surface
*/
typedef struct bsp_surface_s {
	char name[16];
	int32_t flags;
	int32_t value;
} bsp_surface_t;

//remove the bsp_trace_s structure definition l8r on
/**
*	a trace is returned when a box is swept through the world
*/
typedef struct bsp_trace_s {
	qboolean		allsolid;	//!< if true, plane is not valid
	qboolean		startsolid;	//!< if true, the initial point was in a solid area
	double			fraction;	//!< time completed, 1.0 = didn't hit anything
	vec3_t			endpos;		//!< final position
	cplane_t		plane;		//!< surface normal at impact
	double			exp_dist;	//!< expanded plane distance
	int32_t			sidenum;	//!< number of the brush side hit
	bsp_surface_t	surface;	//!< the hit point surface
	int32_t			contents;	//!< contents on other side of surface hit
	int32_t			ent;		//!< number of entity hit
} bsp_trace_t;

#endif	// BSPTRACE

/**
*	entitystate for bots
*/
typedef struct bot_entitystate_s {
	int32_t	type;			// entity type
	int32_t	flags;			// entity flags
	vec3_t	origin;			// origin of the entity
	vec3_t	angles;			// angles of the model
	vec3_t	old_origin;		// for lerping
	vec3_t	mins;			// bounding box minimums
	vec3_t	maxs;			// bounding box maximums
	int32_t	groundent;		// ground entity
	int32_t	solid;			// solid type
	int32_t	modelindex;		// model used
	int32_t	modelindex2;	// weapons, CTF flags, etc
	int32_t	frame;			// model frame number
	int32_t	event;			// impulse events -- muzzle flashes, footsteps, etc
	int32_t	eventParm;		// even parameter
	int32_t	powerups;		// bit flags
	int32_t	weapon;			// determines weapon and flash model, etc
	int32_t	legsAnim;		// mask off ANIM_TOGGLEBIT
	int32_t	torsoAnim;		// mask off ANIM_TOGGLEBIT
} bot_entitystate_t;

/**
*	bot AI library exported functions
*/
typedef struct botlib_import_s {
	//!print messages from the bot library
	void (QDECL *Print)(int32_t type, char *fmt, ...);

	//!trace a bbox through the world
	void(*Trace)(bsp_trace_t* trace, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int32_t passent, int32_t contentmask);

	//!trace a bbox against a specific entity
	void(*EntityTrace)(bsp_trace_t* trace, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int32_t entnum, int32_t contentmask);

	//!retrieve the contents at the given point
	int(*PointContents)(vec3_t point);

	//!check if the point is in potential visible sight
	int(*inPVS)(vec3_t p1, vec3_t p2);

	//!retrieve the BSP entity data lump
	char *(*BSPEntityData)(void);

	//
	void(*BSPModelMinsMaxsOrigin)(int32_t modelnum, vec3_t angles, vec3_t mins, vec3_t maxs, vec3_t origin);

	//!send a bot client command
	void(*BotClientCommand)(int client, char* command);

	//memory allocation
	void *(*GetMemory)(int32_t size);
	void(*FreeMemory)(void* ptr);
	void *(*HunkAlloc)(int32_t size);

	//file system access
	int32_t(*FS_FOpenFile)(const char* qpath, fileHandle_t* file, fsMode_t mode);
	int32_t(*FS_Read)(void* buffer, int32_t len, fileHandle_t f);
	int32_t(*FS_Write)(const void* buffer, int32_t len, fileHandle_t f);
	void(*FS_FCloseFile)(fileHandle_t f);
	int32_t(*FS_Seek)(fileHandle_t f, int64_t offset, int32_t origin);

	//debug visualisation stuff
	int32_t(*DebugLineCreate)(void);
	void(*DebugLineDelete)(int32_t line);
	void(*DebugLineShow)(int32_t line, vec3_t start, vec3_t end, int32_t color);

	int32_t(*DebugPolygonCreate)(int32_t color, int32_t numPoints, vec3_t* points);
	void(*DebugPolygonDelete)(int32_t id);
} botlib_import_t;

/**
*	needed for bot navigation
*/
typedef struct aas_export_s {
	//-----------------------------------
	// be_aas_entity.h
	//-----------------------------------
	void(*AAS_EntityInfo)(int32_t entnum, struct aas_entityinfo_s* info);

	//-----------------------------------
	// be_aas_main.h
	//-----------------------------------
	int32_t(*AAS_Initialized)(void);
	void(*AAS_PresenceTypeBoundingBox)(int32_t presencetype, vec3_t mins, vec3_t maxs);
	double(*AAS_Time)(void);

	//--------------------------------------------
	// be_aas_sample.c
	//--------------------------------------------
	int32_t(*AAS_PointAreaNum)(vec3_t point);
	int32_t(*AAS_TraceAreas)(vec3_t start, vec3_t end, int32_t* areas, vec3_t* points, int32_t maxareas);

	//--------------------------------------------
	// be_aas_bspq3.c
	//--------------------------------------------
	int32_t(*AAS_PointContents)(vec3_t point);
	int32_t(*AAS_NextBSPEntity)(int32_t ent);
	int32_t(*AAS_ValueForBSPEpairKey)(int32_t ent, char* key, char* value, int32_t size);
	int32_t(*AAS_VectorForBSPEpairKey)(int32_t ent, char* key, vec3_t v);
	int32_t(*AAS_FloatForBSPEpairKey)(int32_t ent, char* key, double* value);
	int32_t(*AAS_IntForBSPEpairKey)(int32_t ent, char* key, int32_t* value);

	//--------------------------------------------
	// be_aas_reach.c
	//--------------------------------------------
	int32_t(*AAS_AreaReachability)(int32_t areanum);

	//--------------------------------------------
	// be_aas_route.c
	//--------------------------------------------
	int32_t(*AAS_AreaTravelTimeToGoalArea)(int32_t areanum, vec3_t origin, int32_t goalareanum, int32_t travelflags);

	//--------------------------------------------
	// be_aas_move.c
	//--------------------------------------------
	int32_t(*AAS_Swimming)(vec3_t origin);
	int32_t(*AAS_PredictClientMovement)(struct aas_clientmove_s* move,
										int32_t entnum, vec3_t origin,
										int32_t presencetype, int32_t onground,
										vec3_t velocity, vec3_t cmdmove,
										int32_t cmdframes,
										int32_t maxframes, double frametime,
										int32_t stopevent, int32_t stopareanum, int32_t visualize);
} aas_export_t;

/**
*
*/
typedef struct ea_export_s {
	//ClientCommand elementary actions
	void(*EA_Say)(int32_t client, char* str);
	void(*EA_SayTeam)(int32_t client, char* str);
	void(*EA_SayClass)(int32_t client, char* str);
	void(*EA_UseItem)(int32_t client, char* it);
	void(*EA_DropItem)(int32_t client, char *it);
	void(*EA_UseInv)(int32_t client, char* inv);
	void(*EA_DropInv)(int32_t client, char* inv);
	void(*EA_Gesture)(int32_t client);
	void(*EA_Command)(int32_t client, char* command);

	//regular elementary actions
	void(*EA_SelectWeapon)(int32_t client, int32_t weapon);
	void(*EA_Talk)(int32_t client);
	void(*EA_Attack)(int32_t client);
	void(*EA_Alt_Attack)(int32_t client);
	void(*EA_Use)(int32_t client);
	void(*EA_Respawn)(int32_t client);
	void(*EA_Jump)(int32_t client);
	void(*EA_DelayedJump)(int32_t client);
	void(*EA_Crouch)(int32_t client);
	void(*EA_MoveUp)(int32_t client);
	void(*EA_MoveDown)(int32_t client);
	void(*EA_MoveForward)(int32_t client);
	void(*EA_MoveBack)(int32_t client);
	void(*EA_MoveLeft)(int32_t client);
	void(*EA_MoveRight)(int32_t client);
	void(*EA_Move)(int32_t client, vec3_t dir, double speed);
	void(*EA_View)(int32_t client, vec3_t viewangles);
	
	//send regular input to the server
	void(*EA_EndRegular)(int32_t client, double thinktime);
	void(*EA_GetInput)(int32_t client, double thinktime, bot_input_t* input);
	void(*EA_ResetInput)(int32_t client);
} ea_export_t;

/**
*
*/
typedef struct ai_export_s {
	//-----------------------------------
	// be_ai_char.h
	//-----------------------------------
	int32_t(*BotLoadCharacter)(char* charfile, int32_t skill);
	void(*BotFreeCharacter)(int32_t character);
	double(*Characteristic_Float)(int32_t character, int32_t index);
	double(*Characteristic_BFloat)(int32_t character, int32_t index, double min, double max);
	int32_t(*Characteristic_Integer)(int32_t character, int32_t index);
	int32_t(*Characteristic_BInteger)(int32_t character, int32_t index, int32_t min, int32_t max);
	void(*Characteristic_String)(int32_t character, int32_t index, char* buf, int32_t size);
	
	//-----------------------------------
	// be_ai_chat.h
	//-----------------------------------
	int32_t(*BotAllocChatState)(void);
	void(*BotFreeChatState)(int32_t handle);
	void(*BotQueueConsoleMessage)(int32_t chatstate, int32_t type, char* message);
	void(*BotRemoveConsoleMessage)(int32_t chatstate, int32_t handle);
	int32_t(*BotNextConsoleMessage)(int32_t chatstate, struct bot_consolemessage_s* cm);
	int32_t(*BotNumConsoleMessages)(int32_t chatstate);
	void(*BotInitialChat)(int32_t chatstate, char* type, int32_t mcontext, char* var0, char* var1, char* var2, char* var3, char* var4, char* var5, char* var6, char* var7);
	int32_t(*BotNumInitialChats)(int32_t chatstate, char* type);
	int32_t(*BotReplyChat)(int32_t chatstate, char* message, int32_t mcontext, int32_t vcontext, char* var0, char* var1, char* var2, char* var3, char* var4, char* var5, char* var6, char* var7);
	int32_t(*BotChatLength)(int32_t chatstate);
	void(*BotEnterChat)(int32_t chatstate, int32_t client, int32_t sendto);
	void(*BotGetChatMessage)(int32_t chatstate, char* buf, int32_t size);
	int32_t(*StringContains)(char* str1, char* str2, int32_t casesensitive);
	int32_t(*BotFindMatch)(char* str, struct bot_match_s* match, uint64_t context);
	void(*BotMatchVariable)(struct bot_match_s* match, int32_t variable, char* buf, int32_t size);
	void(*UnifyWhiteSpaces)(char* string);
	void(*BotReplaceSynonyms)(char* string, uint64_t context);
	int32_t(*BotLoadChatFile)(int32_t chatstate, char* chatfile, char* chatname);
	void(*BotSetChatGender)(int32_t chatstate, int32_t gender);
	void(*BotSetChatName)(int32_t chatstate, char* name);
	
	//-----------------------------------
	// be_ai_goal.h
	//-----------------------------------
	void(*BotResetGoalState)(int32_t goalstate);
	void(*BotResetAvoidGoals)(int32_t goalstate);
	void(*BotRemoveFromAvoidGoals)(int32_t goalstate, int32_t number);
	void(*BotPushGoal)(int32_t goalstate, struct bot_goal_s* goal);
	void(*BotPopGoal)(int32_t goalstate);
	void(*BotEmptyGoalStack)(int32_t goalstate);
	void(*BotDumpAvoidGoals)(int32_t goalstate);
	void(*BotDumpGoalStack)(int32_t goalstate);
	void(*BotGoalName)(int32_t number, char* name, int32_t size);
	int32_t(*BotGetTopGoal)(int32_t goalstate, struct bot_goal_s* goal);
	int32_t(*BotGetSecondGoal)(int32_t goalstate, struct bot_goal_s* goal);
	int32_t(*BotChooseLTGItem)(int32_t goalstate, vec3_t origin, int32_t* inventory, int32_t travelflags, qboolean botRoamsOnly);
	int32_t(*BotChooseNBGItem)(int32_t goalstate, vec3_t origin, int32_t* inventory, int32_t travelflags, struct bot_goal_s* ltg, double maxtime, qboolean botRoamsOnly);
	int32_t(*BotTouchingGoal)(vec3_t origin, struct bot_goal_s* goal);
	int32_t(*BotItemGoalInVisButNotVisible)(int32_t viewer, vec3_t eye, vec3_t viewangles, struct bot_goal_s* goal);
	int32_t(*BotGetLevelItemGoal)(int32_t index, char* classname, struct bot_goal_s* goal);
	int32_t(*BotGetNextCampSpotGoal)(int32_t num, struct bot_goal_s* goal);
	int32_t(*BotGetMapLocationGoal)(char* name, struct bot_goal_s* goal);
	double(*BotAvoidGoalTime)(int32_t goalstate, int32_t number);
	void(*BotInitLevelItems)(void);
	void(*BotUpdateEntityItems)(void);
	int32_t(*BotLoadItemWeights)(int32_t goalstate, char* filename);
	void(*BotFreeItemWeights)(int32_t goalstate);
	void(*BotInterbreedGoalFuzzyLogic)(int32_t parent1, int32_t parent2, int32_t child);
	void(*BotSaveGoalFuzzyLogic)(int32_t goalstate, char* filename);
	void(*BotMutateGoalFuzzyLogic)(int32_t goalstate, double range);
	int32_t(*BotAllocGoalState)(int32_t client);
	void(*BotFreeGoalState)(int32_t handle);

	//-----------------------------------
	// be_ai_move.h
	//-----------------------------------
	void(*BotResetMoveState)(int32_t movestate);
	void(*BotMoveToGoal)(struct bot_moveresult_s* result, int32_t movestate, struct bot_goal_s* goal, int32_t travelflags);
	int32_t(*BotMoveInDirection)(int32_t movestate, vec3_t dir, double speed, int32_t type);
	void(*BotResetAvoidReach)(int32_t movestate);
	void(*BotResetLastAvoidReach)(int32_t movestate);
	int32_t(*BotReachabilityArea)(vec3_t origin, int32_t testground);
	int32_t(*BotMovementViewTarget)(int32_t movestate, struct bot_goal_s* goal, int32_t travelflags, double lookahead, vec3_t target);
	int32_t(*BotPredictVisiblePosition)(vec3_t origin, int32_t areanum, struct bot_goal_s* goal, int32_t travelflags, vec3_t target);
	int32_t(*BotAllocMoveState)(void);
	void(*BotFreeMoveState)(int32_t handle);
	void(*BotInitMoveState)(int32_t handle, struct bot_initmove_s* initmove);
	
	//-----------------------------------
	// be_ai_weap.h
	//-----------------------------------
	int32_t(*BotChooseBestFightWeapon)(int32_t weaponstate, int32_t* inventory, qboolean meleeRange);
	void(*BotGetWeaponInfo)(int32_t weaponstate, int32_t weapon, struct weaponinfo_s* weaponinfo);
	int32_t(*BotLoadWeaponWeights)(int32_t weaponstate, char* filename);
	int32_t(*BotAllocWeaponState)(void);
	void(*BotFreeWeaponState)(int32_t weaponstate);
	void(*BotResetWeaponState)(int32_t weaponstate);
	
	//-----------------------------------
	// be_ai_gen.h
	//-----------------------------------
	int32_t(*GeneticParentsAndChildSelection)(int32_t numranks, double* ranks, int32_t* parent1, int32_t* parent2, int32_t* child);
} ai_export_t;

/**
*	bot AI library imported functions
*/
typedef struct botlib_export_s {
	//!Area Awareness System functions
	aas_export_t aas;
	
	//!Elementary Action functions
	ea_export_t ea;
	
	//!AI functions
	ai_export_t ai;
	
	//!setup the bot library, returns BLERR_
	int32_t(*BotLibSetup)(void);
	
	//!shutdown the bot library, returns BLERR_
	int32_t(*BotLibShutdown)(void);
	
	//!sets a library variable returns BLERR_
	int32_t(*BotLibVarSet)(char* var_name, char* value);
	
	//!gets a library variable returns BLERR_
	int32_t(*BotLibVarGet)(char* var_name, char* value, int32_t size);

	//!sets a C-like define returns BLERR_
	int32_t(*BotLibDefine)(char* string);
	
	//!start a frame in the bot library
	int32_t(*BotLibStartFrame)(double time);
	
	//!load a new map in the bot library
	int32_t(*BotLibLoadMap)(const char* mapname);
	
	//!entity updates
	int32_t(*BotLibUpdateEntity)(int32_t ent, bot_entitystate_t* state);
	
	//!just for testing
	int32_t(*Test)(int32_t parm0, char* parm1, vec3_t parm2, vec3_t parm3);
} botlib_export_t;

//linking of bot library
botlib_export_t* GetBotLibAPI(int32_t apiVersion, botlib_import_t* import);

/* Library variables:

name:						default:			module(s):			description:

"basedir"					""					l_utils.c			base directory
"gamedir"					""					l_utils.c			game directory
"cddir"						""					l_utils.c			CD directory

"log"						"0"					l_log.c				enable/disable creating a log file
"maxclients"				"4"					be_interface.c		maximum number of clients
"maxentities"				"1024"				be_interface.c		maximum number of entities
"bot_developer"				"0"					be_interface.c		bot developer mode

"sv_friction"				"6"					be_aas_move.c		ground friction
"sv_stopspeed"				"100"				be_aas_move.c		stop speed
"sv_gravity"				"800"				be_aas_move.c		gravity value
"sv_waterfriction"			"1"					be_aas_move.c		water friction
"sv_watergravity"			"400"				be_aas_move.c		gravity in water
"sv_maxvelocity"			"320"				be_aas_move.c		maximum velocity
"sv_maxwalkvelocity"		"320"				be_aas_move.c		maximum walk velocity
"sv_maxcrouchvelocity"		"100"				be_aas_move.c		maximum crouch velocity
"sv_maxswimvelocity"		"150"				be_aas_move.c		maximum swim velocity
"sv_walkaccelerate"			"10"				be_aas_move.c		walk acceleration
"sv_airaccelerate"			"1"					be_aas_move.c		air acceleration
"sv_swimaccelerate"			"4"					be_aas_move.c		swim acceleration
"sv_maxstep"				"18"				be_aas_move.c		maximum step height
"sv_maxsteepness"			"0.7"				be_aas_move.c		maximum floor steepness
"sv_maxbarrier"				"32"				be_aas_move.c		maximum barrier height
"sv_maxwaterjump"			"19"				be_aas_move.c		maximum waterjump height
"sv_jumpvel"				"270"				be_aas_move.c		jump z velocity

"max_aaslinks"				"4096"				be_aas_sample.c		maximum links in the AAS
"max_routingcache"			"4096"				be_aas_route.c		maximum routing cache size in KB
"forceclustering"			"0"					be_aas_main.c		force recalculation of clusters
"forcereachability"			"0"					be_aas_main.c		force recalculation of reachabilities
"forcewrite"				"0"					be_aas_main.c		force writing of aas file
"aasoptimize"				"0"					be_aas_main.c		enable aas optimization
"sv_mapChecksum"			"0"					be_aas_main.c		BSP file checksum
"bot_visualizejumppads"		"0"					be_aas_reach.c		visualize jump pads

"bot_reloadcharacters"		"0"					-					reload bot character files
"ai_gametype"				"0"					be_ai_goal.c		game type
"weapindex_rocketlauncher"	"5"					be_ai_move.c		rl weapon index for rocket jumping
"weapindex_bfg10k"			"9"					be_ai_move.c		bfg weapon index for bfg jumping
"weapindex_grapple"			"10"				be_ai_move.c		grapple weapon index for grappling
"entitytypemissile"			"3"					be_ai_move.c		ET_MISSILE
"offhandgrapple"			"0"					be_ai_move.c		enable off hand grapple hook
"cmd_grappleon"				"grappleon"			be_ai_move.c		command to activate off hand grapple
"cmd_grappleoff"			"grappleoff"		be_ai_move.c		command to deactivate off hand grapple
"itemconfig"				"items.c"			be_ai_goal.c		item configuration file
"weaponconfig"				"weapons.c"			be_ai_weap.c		weapon configuration file
"synfile"					"syn.c"				be_ai_chat.c		file with synonyms
"rndfile"					"rnd.c"				be_ai_chat.c		file with random strings
"matchfile"					"match.c"			be_ai_chat.c		file with match strings
"nochat"					"0"					be_ai_chat.c		disable chats
"max_messages"				"1024"				be_ai_chat.c		console message heap size
"max_weaponinfo"			"32"				be_ai_weap.c		maximum number of weapon info
"max_projectileinfo"		"32"				be_ai_weap.c		maximum number of projectile info
"max_iteminfo"				"256"				be_ai_goal.c		maximum number of item info
"max_levelitems"			"256"				be_ai_goal.c		maximum number of level items

*/

#endif /* BOTLIB_H_ */
