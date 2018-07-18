// Copyright (C) 1999-2000 Id Software, Inc.
//

#include <sstream>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4127)
#endif
#include <nlohmann/json.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "g_main.h"
#include "g_local.h"
#include "g_groups.h"
#include "g_client.h"
#include "g_spawn.h"
#include "g_cmds.h"
#include "g_items.h"
#include "../base_game/bg_misc.h"
#include "g_logger.h"
#include "g_usable.h"
#include "g_lua.h"
#include "g_missile.h"
#include "g_utils.h"
#include "g_mover.h"
#include "g_weapon.h"
#include "g_syscalls.h"

// pre declare ai functions
luaAlertState_t* luaAlertState;
extern int32_t AI_main_BotAIStartFrame(int32_t time);
extern int32_t AI_main_BotAISetup(int32_t restart);
extern int32_t AI_main_BotAIShutdown(int32_t restart);
extern int32_t AI_main_BotAILoadMap(int32_t restart);

extern void BG_LoadItemNames(void);
extern qboolean BG_ParseRankNames(char* fileName, rankNames_t rankNames[], size_t size);

enum g_mainLimits_e
{
  MAX_GROUP_FILE_SIZE = 5000
};

//RPG-X: RedTechie
uint32_t RPGEntityCount; //Global entity count varible

level_locals_t level;
extern char	races[256];	//this is evil!

group_list_t group_list[MAX_GROUP_MEMBERS];
int32_t group_count;
int32_t numKilled;

typedef struct
{
  /*@shared@*/ /*@null@*/ vmCvar_t	*vmCvar;
  char*		 cvarName;
  char*		 defaultString;
  int			 cvarFlags;
  int			 modificationCount;  // for tracking changes
  qboolean	 trackChange;	// track this variable, and announce if changed
} cvarTable_t;

gentity_t g_entities[MAX_GENTITIES];
static gclient_t	g_clients[MAX_CLIENTS];

rankNames_t g_rankNames[MAX_RANKS];

g_classData_t	g_classData[MAX_CLASSES];

vmCvar_t	g_logLevel;
vmCvar_t	g_gametype;
vmCvar_t	g_dmflags;
vmCvar_t	g_password;
vmCvar_t	g_needpass;
vmCvar_t	g_maxclients;
vmCvar_t	g_maxGameClients;
vmCvar_t	g_dedicated;
vmCvar_t	g_speed;
vmCvar_t	g_gravity;
vmCvar_t	g_cheats;
vmCvar_t	g_knockback;
vmCvar_t	g_dmgmult;
vmCvar_t	g_forcerespawn;
vmCvar_t	g_inactivity;
vmCvar_t	g_debugMove;
vmCvar_t	g_debugDamage;
vmCvar_t	g_debugAlloc;
vmCvar_t	g_weaponRespawn;
vmCvar_t	g_adaptRespawn;
vmCvar_t	g_motd;
vmCvar_t	g_synchronousClients;
vmCvar_t	g_restarted;
static vmCvar_t	g_log;
static vmCvar_t	g_logSync;
vmCvar_t	g_allowVote;
vmCvar_t	g_banIPs;
vmCvar_t	g_filterBan;
vmCvar_t	g_banIDs;	//TiM - Security ban system
vmCvar_t	g_debugForward;
vmCvar_t	g_debugRight;
vmCvar_t	g_debugUp;
vmCvar_t	g_language;
vmCvar_t	g_holoIntro;
vmCvar_t	g_team_group_red;
vmCvar_t	g_team_group_blue;
vmCvar_t	g_random_skin_limit;
vmCvar_t	g_classChangeDebounceTime;

//RPG-X: - RedTechie More CVAR INFO
vmCvar_t	rpg_allowvote;
vmCvar_t	rpg_chatsallowed;
vmCvar_t	rpg_allowsuicide;
vmCvar_t	rpg_selfdamage;
vmCvar_t	rpg_rpg;
vmCvar_t	rpg_kickspammers;				//!< Specifies whether player that spam get kicked automatically.
vmCvar_t	rpg_kicksuiciders;				//!< Specifies whether player that sucide get kicked automatically.
vmCvar_t	rpg_allowspmaps;				//!< Specifies whether singleplayer maps can be loaded
vmCvar_t	rpg_rangetricorder;				//!< Maximum range the Tricorder can scan.
vmCvar_t	rpg_rangehypo;					//!< Maximum range of the Hypospray.
vmCvar_t	rpg_norpgclasses;
vmCvar_t	rpg_forceclasscolor;			//!< Specifies whether class colors specified in the *.class file are enforced.

/*
 * Ristrictions: nocloak = 1, noflight = 2
 * Default value: 0
 */
vmCvar_t	rpg_restrictions;

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
vmCvar_t	rpg_dmgFlags;
vmCvar_t	rpg_noweapons;						//!< Can be used to disable all weapons.
vmCvar_t	rpg_welcomemessage;					//!< Welcome message displayed when a player joins the server.
vmCvar_t	rpg_timedmessagetime;				//!< Delay between timed mesagges
vmCvar_t	rpg_forcekillradius;				//!< Specifies whether the forcekillradius command is avaible.
vmCvar_t	rpg_forcekillradiuswaittime;		//!< forcekillradius delay
vmCvar_t	rpg_noclipspectating;				//!< Specifies whether spectators uses clipping.
vmCvar_t	rpg_chatarearange;					//!< Maximumrange for area chat.
vmCvar_t	rpg_forcefielddamage;				//!< Damage a player takes when touching an admin force field
vmCvar_t	rpg_invisibletripmines;				//!< Specifies whether invisible tripmines are enabled.
vmCvar_t	rpg_medicsrevive;					//!< Are medics allowed to revive other players
vmCvar_t	rpg_effectsgun;						//!< Can be used to enable/disable the effects gun
vmCvar_t	rpg_phaserdisintegrates;			//!< If enabled phasers disintegrate players instead ob incapacitating them.
//! Kick player for n00bing after this ammount of kills
vmCvar_t	rpg_kickAfterXkills; //RPG-X | Phenix | 06/04/2005
vmCvar_t	rpg_rankSet;						//!< Rankset to use
vmCvar_t	rpg_passMessage;	 //RPG-X | TiM | 2/2/2006
//! If enabled the previous name of a player is displayed if the reconnects with a different one.
vmCvar_t	rpg_renamedPlayers;	 //RPG-X | TiM | For players that disconnect, reconnect with dif names to try and be sneaky...
//! If enabled only one player can have a name at the same time.
vmCvar_t	rpg_uniqueNames;	 //RPG-X | TiM | When active, only one player can have the same name on a server
//RPG-X | TiM | Cvars to make the rank system more controllable
vmCvar_t	rpg_startingRank;	 //!< The rank players will start as, regardless
vmCvar_t	rpg_maxRank;		 //!< Absolute rank players can set themselves to
vmCvar_t	rpg_changeRanks;	 //!< If players are allowed to change ranks themselves

//TiM - height paramters
vmCvar_t	rpg_maxHeight;
vmCvar_t	rpg_minHeight;
vmCvar_t	rpg_maxWeight;
vmCvar_t	rpg_minWeight;

//! Classet to use
vmCvar_t	rpg_classSet;		//TiM: current server class configuration

vmCvar_t	rpg_mapGiveFlags;

vmCvar_t	rpg_scannablePanels;	//!< Scan consoles and doors be scanned with the Tricorder

// Drop stuff
//! Enables weapon dropping
vmCvar_t    rpg_allowWeaponDrop; //RPG-X | Marcin | 03/12/2008
//! Do weapons stay in inventory when a player drops them
vmCvar_t    rpg_weaponsStay;     //RPG-X | Marcin | 04/12/2008
//! Does a player drop his weapons when he dies
vmCvar_t	rpg_dropOnDeath;	 //RPG-X | Marcin | 30/12/2008
//vmCvar_t	rpg_flushDroppedOnDisconnect; //RPG-X | GSIO01 | 08/05/2009

// Weapon speeds
//! Speed for compression riffle  projectiles
vmCvar_t    rpg_rifleSpeed;      //RPG-X | Marcin | 04/12/2008
//! Speed for disruptor projectiles
vmCvar_t    rpg_disruptorSpeed;  //RPG-X | Marcin | 04/12/2008
//! Speed for photon burst projectiles
vmCvar_t    rpg_photonSpeed;     //RPG-X | Marcin | 05/12/2008
//! Speed for altfire photon burst projectiles
vmCvar_t    rpg_altPhotonSpeed;  //RPG-X | Marcin | 06/12/2008

// Weapon delays
vmCvar_t    rpg_rifleDelay;      //RPG-X | Marcin | 06/12/2008
vmCvar_t    rpg_disruptorDelay;  //RPG-X | Marcin | 06/12/2008
vmCvar_t    rpg_photonDelay;     //RPG-X | Marcin | 06/12/2008
vmCvar_t    rpg_altPhotonDelay;  //RPG-X | Marcin | 06/12/2008
vmCvar_t	rpg_TR116Delay;		 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_altTricorderDelay;	 //RPG-X | GSIO01 | 14/05/2009

// Motd
//! Specifies the message of the day file
vmCvar_t	rpg_motdFile;        //RPG-X | Marcin | 23/12/2008

// Privacy
//! If enabled admins can the private chat messages
vmCvar_t	rpg_respectPrivacy;  //RPG-X | Marcin | 24/12/2008

// Weaps
vmCvar_t	rpg_maxTricorders;	 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_maxPADDs;		 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_maxCups;		 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_maxPhasers;		 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_maxRifles;		 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_maxTR116s;		 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_maxAdminguns;	 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_maxPhotonbursts; //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_maxDisruptors;	 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_maxMedkits;		 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_maxHyposprays;	 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_maxRegenerators; //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_maxToolkits;	 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_maxHyperSpanners;//RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_minTricorders;	 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_minPADDs;		 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_minCups;		 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_minPhasers;		 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_minRifles;		 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_minTR116s;		 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_minAdminguns;	 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_minPhotonbursts; //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_minDisruptors;	 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_minMedkits;		 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_minHyposprays;	 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_minRegenerators; //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_minToolkits;	 //RPG-X | Marcin | 30/12/2008
vmCvar_t	rpg_minHyperSpanners;//RPG-X | Marcin | 30/12/2008

// respawn delay
//! Delay for respawn function
vmCvar_t	rpg_fraggerSpawnDelay; //RPG-X | Marcin | 03/01/2009

// borg adaption
//! If enabled borg get immune to weapons after some time
vmCvar_t	rpg_borgAdapt;				//RPG-X | GSIO01 | 08/05/2009
//! Borg shield plays sound if hit?
vmCvar_t	rpg_adaptUseSound;			//RPG-X | GSIO01 | 08/05/2009
//! Number of hits  before borg adapt to this weapon
vmCvar_t	rpg_adaptPhaserHits;		//RPG-X | GSIO01 | 08/05/2009
//! Number of hits  before borg adapt to this weapon
vmCvar_t	rpg_adaptPhotonHits;		//RPG-X | GSIO01 | 08/05/2009
//! Number of hits  before borg adapt to this weapon
vmCvar_t	rpg_adaptCrifleHits;		//RPG-X | GSIO01 | 08/05/2009
//! Number of hits  before borg adapt to this weapon
vmCvar_t	rpg_adaptDisruptorHits;		//RPG-X | GSIO01 | 08/05/2009
//! Number of hits  before borg adapt to this weapon
vmCvar_t	rpg_adaptTR116Hits;			//RPG-X | GSIO01 | 08/05/2009
//! Number of hits  before borg adapt to this weapon
vmCvar_t	rpg_adaptGrenadeLauncherHits; //RPG-X | GSIO01 | 08/05/2009
//! Are player allowed to remodulate their weapons
vmCvar_t	rpg_allowRemodulation;		//RPG-X | GSIO01 | 10/05/2009
//! Can borg move through admin force fields
vmCvar_t	rpg_borgMoveThroughFields;	//RPG-X | GSIO01 | 19/05/2009
//! Delay for modulation
vmCvar_t	rpg_RemodulationDelay;

// hypo melee
//! Can the hypospray be used as weapons
vmCvar_t	rpg_hypoMelee;

// repairing breakables
//! Can be used to modify repairspeed
vmCvar_t	rpg_repairModifier;	//RPG-X | GSIO01 | 09/05/2009

// force field colors
//! Current force field color
vmCvar_t	rpg_forceFieldColor; //RPG-X | GSIO01 | 09/05/2009

// modulation thingies //RPG-X | GSIO01 | 12/05/2009
vmCvar_t	rpg_forceFieldFreq;

// calc lif travel duration by dec distance
//! Calculathe the travel durration of the turbolift by the difference between the deck numbers?
vmCvar_t	rpg_calcLiftTravelDuration;
//! Can be used to modify the lift speed
vmCvar_t	rpg_liftDurationModifier;

// admin vote override
//! Admins can override votes
vmCvar_t	rpg_adminVoteOverride;

// server change
//! Enables/disables target_serverchange
vmCvar_t	rpg_serverchange;

// SP level change
//! Allow target_levelchange to change the current level?
vmCvar_t	rpg_allowSPLevelChange;

/* SQL Database */
vmCvar_t	sql_use;		//!< Use SQL? 1 = mysql, 2 = sqlite

// developer tools
vmCvar_t	dev_showTriggers;

#ifdef G_LUA
// Print Lua debugging information into the game console?
vmCvar_t        g_debugLua;
vmCvar_t        lua_modules;
vmCvar_t        lua_allowedModules;
#endif

vmCvar_t	g_developer;

vmCvar_t	rpg_spEasterEggs;


static cvarTable_t		gameCvarTable[] = {
  // don't override the cheat state set by the system
  { &g_cheats, "sv_cheats", "", 0, 0, qfalse },
  // noset vars
  { NULL, "gamename", GAMEVERSION, CVAR_SERVERINFO | CVAR_ROM, 0, qfalse },
  { NULL, "gamedate", __DATE__, CVAR_ROM, 0, qfalse },
  { &g_restarted, "g_restarted", "0", CVAR_ROM, 0, qfalse },
  { NULL, "sv_mapname", "", CVAR_SERVERINFO | CVAR_ROM, 0, qfalse },
  // latched vars
  { &g_gametype, "g_gametype", "0", CVAR_SERVERINFO | CVAR_LATCH | CVAR_INIT | CVAR_ROM, 0, qfalse },
  { &g_maxclients, "sv_maxclients", "8", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse },
  { &g_maxGameClients, "g_maxGameClients", "0", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse },
  // change anytime vars
  { &g_dmflags, "dmflags", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qtrue },
  { &g_synchronousClients, "g_synchronousClients", "0", CVAR_SYSTEMINFO, 0, qfalse },
  { &g_log, "g_log", "", CVAR_ARCHIVE, 0, qfalse },
  { &g_logSync, "g_logSync", "0", CVAR_ARCHIVE, 0, qfalse },
  { &g_password, "g_password", "", CVAR_USERINFO, 0, qfalse },
  { &g_banIPs, "g_banIPs", "", CVAR_ARCHIVE, 0, qfalse },
  { &g_filterBan, "g_filterBan", "1", CVAR_ARCHIVE, 0, qfalse },
  { &g_banIDs, "g_banIDs", "", CVAR_ARCHIVE, 0, qfalse },
  { &g_needpass, "g_needpass", "0", CVAR_SERVERINFO | CVAR_ROM, 0, qfalse },
  { &g_dedicated, "dedicated", "0", 0, 0, qfalse },
  { &g_speed, "g_speed", "250", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qtrue }, // Quake 3 default was 320.
  { &g_gravity, "g_gravity", "800", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qtrue },
  { &g_knockback, "g_knockback", "500", 0, 0, qtrue },
  { &g_dmgmult, "g_dmgmult", "1", 0, 0, qtrue },
  { &g_weaponRespawn, "g_weaponrespawn", "5", 0, 0, qtrue }, // Quake 3 default (with 1 ammo weapons) was 5.
  { &g_adaptRespawn, "g_adaptrespawn", "1", 0, 0, qtrue }, // Make weapons respawn faster with a lot of players.
  { &g_forcerespawn, "g_forcerespawn", "0", 0, 0, qtrue }, // Quake 3 default was 20.  This is more "user friendly".
  { &g_inactivity, "g_inactivity", "0", 0, 0, qtrue },
  { &g_debugMove, "g_debugMove", "0", 0, 0, qfalse },
  { &g_debugDamage, "g_debugDamage", "0", 0, 0, qfalse },
  { &g_debugAlloc, "g_debugAlloc", "0", 0, 0, qfalse },
  { &g_motd, "g_motd", "", 0, 0, qfalse },
  { &g_allowVote, "g_allowVote", "1", CVAR_SERVERINFO, 0, qfalse },
#if 0
  { &g_debugForward,				"g_debugForward",				"0",						0,														0, qfalse },
  { &g_debugRight,				"g_debugRight",					"0",						0,														0, qfalse },
  { &g_debugUp,					"g_debugUp",					"0",						0,														0, qfalse },
#endif
  { &g_language, "g_language", "", CVAR_ARCHIVE, 0, qfalse },
  { &g_holoIntro, "g_holoIntro", "1", CVAR_ARCHIVE, 0, qfalse },
  { &g_team_group_red, "g_team_group_red", "", CVAR_LATCH, 0, qfalse }, // Used to have CVAR_ARCHIVE
  { &g_team_group_blue, "g_team_group_blue", "", CVAR_LATCH, 0, qfalse }, // Used to have CVAR_ARCHIVE
  { &g_random_skin_limit, "g_random_skin_limit", "4", CVAR_ARCHIVE, 0, qfalse },
  { &g_classChangeDebounceTime, "g_classChangeDebounceTime", "180", CVAR_ARCHIVE, 0, qfalse },
  //RPG-X: RedTechie - RPG-X CVARS....duh....just for the slow ones
  { &rpg_allowvote, "rpg_allowVote", "1", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_chatsallowed, "rpg_chatsAllowed", "10", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_allowsuicide, "rpg_allowSuicide", "1", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_selfdamage, "rpg_selfDamage", "1", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_rpg, "rpg_rpg", "1", CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse },
  { &rpg_kickspammers, "rpg_kickSpammers", "0", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_kicksuiciders, "rpg_kickSuiciders", "0", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_allowspmaps, "rpg_allowSPMaps", "0", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_rangetricorder, "rpg_rangeTricorder", "128", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_rangehypo, "rpg_rangeHypo", "32", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_norpgclasses, "rpg_noRPGClasses", "0", CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse },
  { &rpg_forceclasscolor, "rpg_forceClassColor", "0", CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse },
  { &rpg_restrictions, "rpg_restrictions", "0", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_dmgFlags, "rpg_dmgFlags", "63", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_noweapons, "rpg_noWeapons", "0", CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse },
  { &rpg_welcomemessage, "rpg_welcomeMessage", "Welcome to the RPG-X Mod", CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse },
  { &rpg_timedmessagetime, "rpg_timedMessageTime", "0", CVAR_ARCHIVE, 0, qfalse }, //TiM : LATCH Not necessary here.
  { &rpg_forcekillradius, "rpg_forceKillRadius", "0", CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
  { &rpg_forcekillradiuswaittime, "rpg_forceKillRadiusWaitTime", "45000", CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse },
  { &rpg_chatarearange, "rpg_chatAreaRange", "200", CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },	//Not latched (ie doesnt need server restart)
  { &rpg_forcefielddamage, "rpg_forcefieldDamage", "0", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_invisibletripmines, "rpg_invisibleTripmines", "1", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse },
  { &rpg_medicsrevive, "rpg_medicsRevive", "1", CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse },
  { &rpg_effectsgun, "rpg_effectsGun", "1", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse },
  { &rpg_phaserdisintegrates, "rpg_phaserDisintegrates", "1", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_kickAfterXkills, "rpg_kickAfterNumkills", "2", CVAR_ARCHIVE, 0, qfalse }, //RPG-X | Phenix | 06/04/2005
  { &rpg_rankSet, "rpg_rankSet", (char*)RANKSET_DEFAULT, CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_LATCH, 0, qfalse },
  { &rpg_passMessage, "rpg_passMessage", "", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_renamedPlayers, "rpg_renamedPlayers", "1", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_uniqueNames, "rpg_uniqueNames", "1", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_startingRank, "rpg_startingRank", "", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_maxRank, "rpg_maxRank", "", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse },
  { &rpg_changeRanks, "rpg_changeRanks", "1", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_classSet, "rpg_classSet", "rpgx_default", CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_LATCH, 0, qfalse },
  { &rpg_maxHeight, "rpg_maxHeight", "1.15", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_minHeight, "rpg_minHeight", "0.90", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_maxWeight, "rpg_maxWeight", "1.10", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_minWeight, "rpg_minWeight", "0.90", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_mapGiveFlags, "rpg_mapGiveFlags", "0", CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse },
  //TiM - maybe we can fix it later, but for now, disable it
  { &rpg_scannablePanels, "rpg_scannablePanels", "1", CVAR_ARCHIVE | CVAR_LATCH | CVAR_SERVERINFO, 0, qfalse },
  { &rpg_allowWeaponDrop, "rpg_allowWeaponDrop", "1", CVAR_ARCHIVE, 0, qfalse }, // RPG-X | Marcin | 03/12/2008
  { &rpg_weaponsStay, "rpg_weaponsStay", "0", CVAR_ARCHIVE, 0, qfalse },         // RPG-X | Marcin | 04/12/2008
  { &rpg_rifleSpeed, "rpg_rifleSpeed", "2700", 0, 0, qtrue },        // RPG-X | Marcin | 04/12/2008
  { &rpg_disruptorSpeed, "rpg_disruptorSpeed", "3000", 0, 0, qtrue },// RPG-X | Marcin | 04/12/2008
  { &rpg_photonSpeed, "rpg_photonSpeed", "1300", 0, 0, qtrue },      // RPG-X | Marcin | 05/12/2008
  { &rpg_altPhotonSpeed, "rpg_altPhotonSpeed", "650", 0, 0, qtrue }, // RPG-X | Marcin | 06/12/2008
  { &rpg_rifleDelay, "rpg_rifleDelay", "250", 0, 0, qtrue },         // RPG-X | Marcin | 06/12/2008
  { &rpg_disruptorDelay, "rpg_disruptorDelay", "700", 0, 0, qtrue }, // RPG-X | Marcin | 06/12/2008
  { &rpg_photonDelay, "rpg_photonDelay", "1200", 0, 0, qtrue },      // RPG-X | Marcin | 06/12/2008
  { &rpg_altPhotonDelay, "rpg_altPhotonDelay", "1600", 0, 0, qtrue },// RPG-X | Marcin | 06/12/2008
  { &rpg_TR116Delay, "rpg_TR116Delay", "500", 0, 0, qtrue },		   // RPG-X | Marcin | 30/12/2008
  { &rpg_motdFile, "rpg_motdFile", "RPG-X_Motd.txt", CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse },		// RPG-X | Marcin | 23/12/2008
  { &rpg_respectPrivacy, "rpg_respectPrivacy", "0", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse },	// RPG-X | Marcin | 24/12/2008
  { &rpg_maxTricorders, "rpg_maxTricorders", "1", CVAR_ARCHIVE, 0, qtrue },						// RPG-X | Marcin | 30/12/2008
  { &rpg_maxPADDs, "rpg_maxPADDs", "10", CVAR_ARCHIVE, 0, qtrue },								// RPG-X | Marcin | 30/12/2008
  { &rpg_maxCups, "rpg_maxCups", "2", CVAR_ARCHIVE, 0, qtrue },									// RPG-X | Marcin | 30/12/2008
  { &rpg_maxPhasers, "rpg_maxPhasers", "1", CVAR_ARCHIVE, 0, qtrue },								// RPG-X | Marcin | 30/12/2008
  { &rpg_maxRifles, "rpg_maxRifles", "1", CVAR_ARCHIVE, 0, qtrue },								// RPG-X | Marcin | 30/12/2008
  { &rpg_maxTR116s, "rpg_maxTR116s", "1", CVAR_ARCHIVE, 0, qtrue },								// RPG-X | Marcin | 30/12/2008
  { &rpg_maxAdminguns, "rpg_maxAdminguns", "1", CVAR_ARCHIVE, 0, qtrue },							// RPG-X | Marcin | 30/12/2008
  { &rpg_maxPhotonbursts, "rpg_maxPhotonbursts", "1", CVAR_ARCHIVE, 0, qtrue },					// RPG-X | Marcin | 30/12/2008
  { &rpg_maxDisruptors, "rpg_maxDisruptors", "1", CVAR_ARCHIVE, 0, qtrue },						// RPG-X | Marcin | 30/12/2008
  { &rpg_maxMedkits, "rpg_maxMedkits", "1", CVAR_ARCHIVE, 0, qtrue },								// RPG-X | Marcin | 30/12/2008
  { &rpg_maxHyposprays, "rpg_maxHyposprays", "2", CVAR_ARCHIVE, 0, qtrue },						// RPG-X | Marcin | 30/12/2008
  { &rpg_maxRegenerators, "rpg_maxRegenerators", "1", CVAR_ARCHIVE, 0, qtrue },					// RPG-X | Marcin | 30/12/2008
  { &rpg_maxToolkits, "rpg_maxToolkits", "1", CVAR_ARCHIVE, 0, qtrue },							// RPG-X | Marcin | 30/12/2008
  { &rpg_maxHyperSpanners, "rpg_maxHyperSpanners", "1", CVAR_ARCHIVE, 0, qtrue },					// RPG-X | Marcin | 30/12/2008
  { &rpg_minTricorders, "rpg_minTricorders", "1", CVAR_ARCHIVE, 0, qtrue },						// RPG-X | Marcin | 30/12/2008
  { &rpg_minPADDs, "rpg_minPADDs", "5", CVAR_ARCHIVE, 0, qtrue },									// RPG-X | Marcin | 30/12/2008
  { &rpg_minCups, "rpg_minCups", "1", CVAR_ARCHIVE, 0, qtrue },									// RPG-X | Marcin | 30/12/2008
  { &rpg_minPhasers, "rpg_minPhasers", "1", CVAR_ARCHIVE, 0, qtrue },								// RPG-X | Marcin | 30/12/2008
  { &rpg_minRifles, "rpg_minRifles", "1", CVAR_ARCHIVE, 0, qtrue },								// RPG-X | Marcin | 30/12/2008
  { &rpg_minTR116s, "rpg_minTR116s", "1", CVAR_ARCHIVE, 0, qtrue },								// RPG-X | Marcin | 30/12/2008
  { &rpg_minAdminguns, "rpg_minAdminguns", "1", CVAR_ARCHIVE, 0, qtrue },							// RPG-X | Marcin | 30/12/2008
  { &rpg_minPhotonbursts, "rpg_minPhotonbursts", "1", CVAR_ARCHIVE, 0, qtrue },					// RPG-X | Marcin | 30/12/2008
  { &rpg_minDisruptors, "rpg_minDisruptors", "1", CVAR_ARCHIVE, 0, qtrue },						// RPG-X | Marcin | 30/12/2008
  { &rpg_minMedkits, "rpg_minMedkits", "1", CVAR_ARCHIVE, 0, qtrue },								// RPG-X | Marcin | 30/12/2008
  { &rpg_minHyposprays, "rpg_minHyposprays", "1", CVAR_ARCHIVE, 0, qtrue },						// RPG-X | Marcin | 30/12/2008
  { &rpg_minRegenerators, "rpg_minRegenerators", "1", CVAR_ARCHIVE, 0, qtrue },					// RPG-X | Marcin | 30/12/2008
  { &rpg_minToolkits, "rpg_minToolkits", "1", CVAR_ARCHIVE, 0, qtrue },							// RPG-X | Marcin | 30/12/2008
  { &rpg_minHyperSpanners, "rpg_minHyperSpanners", "1", CVAR_ARCHIVE, 0, qtrue },					// RPG-X | Marcin | 30/12/2008
  { &rpg_dropOnDeath, "rpg_dropItemsOnDeath", "1", CVAR_ARCHIVE, 0, qfalse },						// RPG-X | Marcin | 30/12/2008
  { &rpg_fraggerSpawnDelay, "rpg_fraggerSpawnDelay", "100", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_borgAdapt, "rpg_borgAdapt", "0", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_hypoMelee, "rpg_hypoMelee", "0", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_adaptUseSound, "rpg_adaptUseSound", "0", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_adaptCrifleHits, "rpg_adaptCrifleHits", "6", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_adaptDisruptorHits, "rpg_adaptDisruptorHits", "6", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_adaptPhaserHits, "rpg_adaptPhaserHits", "6", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_adaptPhotonHits, "rpg_adaptPhotonHits", "6", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_adaptTR116Hits, "rpg_adaptTR116Hits", "6", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_adaptGrenadeLauncherHits, "rpg_adaptGrenadeLauncherHits", "6", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_repairModifier, "rpg_repairModifier", "1", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_forceFieldColor, "rpg_forceFieldColor", "0", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_allowRemodulation, "rpg_allowRemodulation", "1", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_forceFieldFreq, "rpg_forceFieldFreq", "0", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_calcLiftTravelDuration, "rpg_calcLiftTravelDuration", "1", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_liftDurationModifier, "rpg_liftDurationModifier", "0.5", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_altTricorderDelay, "rpg_altTricorderDelay", "1000", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_borgMoveThroughFields, "rpg_borgMoveThroughFields", "0", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_RemodulationDelay, "rpg_RemodulationDelay", "5000", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_adminVoteOverride, "rpg_adminVoteOverride", "1", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_serverchange, "rpg_serverchange", "0", CVAR_ARCHIVE, 0, qfalse },
  { &rpg_allowSPLevelChange, "rpg_allowSPLevelChange", "1", CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse },
  { &rpg_spEasterEggs, "rpg_spEasterEggs", "0", CVAR_ARCHIVE, 0, qfalse },
  { &dev_showTriggers, "dev_showTriggers", "0", CVAR_ARCHIVE, 0, qfalse },
  { &sql_use, "sql_use", "0", CVAR_ARCHIVE, 0, qfalse },
#ifdef G_LUA
  { &g_debugLua, "g_debugLua", "0", 0, 0, qfalse },
  { &lua_allowedModules, "lua_allowedModules", "", 0, 0, qfalse },
  { &lua_modules, "lua_modules", "", 0, 0, qfalse },
#endif
  { &g_developer, "g_developer", "0", CVAR_ARCHIVE, 0, qfalse },
  { &g_logLevel, "g_loglevel", "2", CVAR_ARCHIVE, 0, qfalse }
};

static int	gameCvarTableSize = (int)(sizeof(gameCvarTable) / sizeof(gameCvarTable[0]));

static void G_InitGame(int levelTime, unsigned int randomSeed, int restart);
static void G_RunFrame(int levelTime);
void G_ShutdownGame(int restart);

//=============================
// begin code

/*
================
vmMain

This is the only way control passes into the module.
This MUST be the very first function compiled into the .q3vm file
================
*/
Q_EXPORT intptr_t vmMain(int command, int arg0, int arg1, int arg2, /*@unused@*/ int arg3, /*@unused@*/ int arg4, /*@unused@*/ int arg5, /*@unused@*/ int arg6)
{
  switch(command)
  {
  case GAME_INIT:
    G_InitGame(arg0, (unsigned)arg1, arg2);
    return 0;
  case GAME_SHUTDOWN:
    G_ShutdownGame(arg0);
    return 0;
  case GAME_CLIENT_CONNECT:
    return (intptr_t)G_Client_Connect(arg0, (qboolean)arg1, (qboolean)arg2);
  case GAME_CLIENT_THINK:
    ClientThink(arg0);
    return 0;
  case GAME_CLIENT_USERINFO_CHANGED:
    G_Client_UserinfoChanged(arg0); //TiM - this means a user just tried to change it
    return 0;
  case GAME_CLIENT_DISCONNECT:
    G_Client_Disconnect(arg0);
    return 0;
  case GAME_CLIENT_BEGIN:
    G_Client_Begin(arg0, qtrue, qfalse, qtrue);
    return 0;
  case GAME_CLIENT_COMMAND:
    G_Client_Command(arg0);
    return 0;
  case GAME_RUN_FRAME:
    G_RunFrame(arg0);
    return 0;
  case GAME_CONSOLE_COMMAND:
    //RPG-X : TiM - plagiarised Red's logic from SFEFMOD here lol
    return (intptr_t)ConsoleCommand();
  case BOTAI_START_FRAME:
    return (intptr_t)AI_main_BotAIStartFrame(arg0);
  }

  return -1;
}

void QDECL G_PrintfClientAll(const char *fmt, ...)
{
  va_list argptr;
  char	text[1024];

  va_start(argptr, fmt);
  vsnprintf(text, sizeof(text), fmt, argptr);
  va_end(argptr);

  trap_SendServerCommand(-1, va("print \"%s\n\"", text));
}

void QDECL G_PrintfClient(gentity_t *ent, const char *fmt, ...)
{
  va_list		argptr;
  char		text[1024];

  /* only makes sense for players */
  G_Assert(ent->client, (void)0);

  va_start(argptr, fmt);
  vsnprintf(text, sizeof(text), fmt, argptr);
  va_end(argptr);

#ifdef G_LUA
  LuaHook_G_ClientPrint(text, ent - g_entities);
#endif

  trap_SendServerCommand(ent - g_entities, va("print \"%s\n\"", text));
}

void QDECL G_Printf(const char *fmt, ...)
{
  va_list		argptr;
  char		text[1024];

  va_start(argptr, fmt);
  vsnprintf(text, sizeof(text), fmt, argptr);
  va_end(argptr);

  if(trap_Cvar_VariableIntegerValue("developer") != 0)
  {

  }

#ifdef G_LUA
  // Lua API callbacks
  LuaHook_G_Print(text);
#endif

  trap_Printf(text);
}

void QDECL G_Error(const char *fmt, ...)
{
  va_list		argptr;
  char		text[1024];

  va_start(argptr, fmt);
  vsnprintf(text, sizeof(text), fmt, argptr);
  va_end(argptr);

#ifdef G_LUA
  G_Lua_Shutdown();
#endif

  trap_Error(text);
}

stringID_table_t WeaponTable[] = {
  { ENUM2STRING(WP_2) },
  { ENUM2STRING(WP_3) },
  { ENUM2STRING(WP_4) },
  { ENUM2STRING(WP_5) },
  { ENUM2STRING(WP_6) },
  { ENUM2STRING(WP_7) },
  { ENUM2STRING(WP_8) },
  { ENUM2STRING(WP_9) },
  { ENUM2STRING(WP_10) },
  { ENUM2STRING(WP_11) },
  { ENUM2STRING(WP_12) },
  { ENUM2STRING(WP_13) },
  { ENUM2STRING(WP_14) },
  { ENUM2STRING(WP_15) },

  { "WP_TRICORDER", WP_2 },
  { "WP_PADD", WP_3 },
  { "WP_COFFEE", WP_4 },
  { "WP_PHASER", WP_5 },
  { "WP_COMPRESSION_RIFLE", WP_6 },
  { "WP_TR116", WP_7 },
  { "WP_GRENADE_LAUNCHER", WP_8 },
  { "WP_QUANTUM_BURST", WP_9 },
  { "WP_DISRUPTOR", WP_10 },
  { "WP_MEDKIT", WP_11 },
  { "WP_VOYAGER_HYPO", WP_12 },
  { "WP_DERMAL_REGEN", WP_13 },
  { "WP_TOOLKIT", WP_14 },
  { "WP_HYPERSPANNER", WP_15 },

  { NULL, -1 }
};

/**************************
G_LoadClassData

TiM: Loads a ".class" file
and parses the class data
for utilization on the server
and transfer to clients
**************************/
static qboolean G_LoadClassData(char* fileName)
{
  char*			buffer = NULL;
  char*			textPtr = NULL;
  char*			token = NULL;
  int				fileLen;
  fileHandle_t	f = 0;
  qboolean		classValid = qfalse;
  int				classIndex = 0;
  unsigned		weapon;
  int				i;

  //Init the storage place
  memset(&g_classData, 0, sizeof(g_classData));

  fileLen = trap_FS_FOpenFile(fileName, &f, FS_READ);

  if(f == 0)
  {
    G_Printf(S_COLOR_RED "ERROR: File %s not found.\n", fileName);
    return qfalse;
  }

  buffer = (char *)malloc((fileLen + 1) * sizeof(char));
  if(buffer == NULL)
  {
    G_Printf(S_COLOR_RED "ERROR: Was unable to allocate %i bytes.\n", (fileLen + 1) * sizeof(char));
    trap_FS_FCloseFile(f);
    return qfalse;
  }
  memset(buffer, 0, sizeof(buffer));

  trap_FS_Read(buffer, fileLen, f);
  buffer[fileLen] = 0;
  trap_FS_FCloseFile(f);

  COM_BeginParseSession();

  textPtr = buffer;

  token = COM_Parse(&textPtr);

  if(token == NULL || token[0] == 0)
  {
    G_Printf(S_COLOR_RED "ERROR: No data was found when going to parse the file!\n");
    free(buffer);
    return qfalse;
  }

  if(Q_stricmpn(token, "{", 1) != 0)
  {
    G_Printf(S_COLOR_RED "ERROR: File did not start with a '{' symbol!\n");
    free(buffer);
    return qfalse;
  }

  while(qtrue)
  {
    if(classIndex >= MAX_CLASSES)
    {
      break;
    }

    if(Q_stricmpn(token, "{", 1) == 0)
    {
      while(qtrue)
      {
        token = COM_Parse(&textPtr);
        if(token == NULL || token[0] == 0)
        {
          break;
        }

        if(Q_stricmpn(token, "consoleName", 11) == 0)
        {
          if(COM_ParseString(&textPtr, &token))
          {
            G_Printf(S_COLOR_RED "ERROR: Invalid class console name in class index: %i.\n", classIndex);
            SkipBracedSection(&textPtr);
            continue;
          }

          Q_strncpyz(g_classData[classIndex].consoleName, token, sizeof(g_classData[classIndex].consoleName));
          classValid = qtrue;

          continue;
        }

        if(Q_stricmpn(token, "formalName", 11) == 0)
        {
          if(COM_ParseString(&textPtr, &token))
          {
            G_Printf(S_COLOR_RED "ERROR: Invalid class formal name in class index: %i.\n", classIndex);
            SkipBracedSection(&textPtr);
            continue;
          }

          Q_strncpyz(g_classData[classIndex].formalName, token, sizeof(g_classData[classIndex].formalName));
          classValid = qtrue;

          continue;
        }

        if(Q_stricmpn(token, "message", 7) == 0)
        {
          if(COM_ParseString(&textPtr, &token))
          {
            G_Printf(S_COLOR_RED "ERROR: Invalid class message in class index: %i.\n", classIndex);
            continue;
          }

          Q_strncpyz(g_classData[classIndex].message, token, sizeof(g_classData[classIndex].message));
          continue;
        }

        if(Q_stricmpn(token, "modelSkin", 9) == 0)
        {
          if(COM_ParseString(&textPtr, &token))
          {
            G_Printf(S_COLOR_RED "ERROR: Invalid class skin color in class index: %i.\n", classIndex);
            continue;
          }

          Q_strncpyz(g_classData[classIndex].modelSkin, token, sizeof(g_classData[classIndex].modelSkin));
          continue;
        }

        if(Q_stricmpn(token, "weapons", 7) == 0)
        {
          token = COM_Parse(&textPtr);

          if(Q_stricmpn(token, "{", 1) != 0)
          {
            G_Printf(S_COLOR_RED "No opening bracket found for weapons field in class: %i.\n", classIndex);
            SkipRestOfLine(&textPtr);
            continue;
          }

          //sub loop
          while(qtrue)
          {
            token = COM_Parse(&textPtr);

            if(token == NULL || token[0] == 0)
            {
              break;
            }

            if(Q_stricmpn(token, "|", 1) == 0)
            {
              continue;
            }

            if(Q_stricmpn(token, "}", 1) == 0)
            {
              break;
            }

            if(Q_stricmpn(token, "WP_", 3) == 0)
            {
              int t = GetIDForString(WeaponTable, token);

              if(t >= 0)
              {
                weapon = (unsigned)t;
                g_classData[classIndex].weaponsFlags |= (1 << weapon);
                continue;
              }
            }
          }

          continue;
        }

        if(Q_stricmpn(token, "admin", 5) == 0)
        {
          if(COM_ParseInt(&textPtr, &g_classData[classIndex].isAdmin))
          {
            G_Printf(S_COLOR_RED "ERROR: Class admin check for class %i was invalid.\n", classIndex);
            continue;
          }

          continue;
        }

        if(Q_stricmpn(token, "marine", 6) == 0)
        {
          if(COM_ParseInt(&textPtr, &g_classData[classIndex].isMarine))
          {
            G_Printf(S_COLOR_RED "ERROR: Class marine check for class %i was invalid.\n", classIndex);
            continue;
          }

          continue;
        }

        if(Q_stricmpn(token, "medical", 7) == 0)
        {
          if(COM_ParseInt(&textPtr, &g_classData[classIndex].isMedical))
          {
            G_Printf(S_COLOR_RED "ERROR: Class medic check for class %i was invalid.\n", classIndex);
            continue;
          }

          continue;
        }

        if(Q_stricmpn(token, "isBorg", 6) == 0)
        {
          if(COM_ParseInt(&textPtr, &g_classData[classIndex].isBorg))
          {
            G_Printf(S_COLOR_RED "ERROR: Class borg check for class %i was invalid.\n", classIndex);
            continue;
          }
          continue;
        }

        if(Q_stricmpn(token, "n00b", 4) == 0)
        {
          if(COM_ParseInt(&textPtr, &g_classData[classIndex].isn00b))
          {
            G_Printf(S_COLOR_RED "ERROR: Class n00b check for class %i was invalid.\n", classIndex);
            continue;
          }

          continue;
        }

        //skip the client-side specific entries since they interfere with the parsing
        if((Q_stricmpn(token, "radarColor", 10) == 0)
          || (Q_stricmpn(token, "iconColor", 9) == 0)
          || (Q_stricmpn(token, "hasRanks", 8) == 0)
          || (Q_stricmpn(token, "noShow", 6) == 0)
          )
        {
          SkipRestOfLine(&textPtr);
          continue;
        }

        if(Q_stricmpn(token, "}", 1) == 0)
        {
          break;
        }
      }


      if(classValid)
      {
        classIndex++;
        classValid = qfalse;
      }
    }

    token = COM_Parse(&textPtr);
    if(token == NULL || token[0] == 0)
    {
      break;
    }
  }

  free(buffer);

  //build ourselves custom CVARs for each class
  for(i = 0; (g_classData[i].consoleName[0] != 0) && (i < MAX_CLASSES); i++)
  {
    trap_Cvar_Register(NULL, va("rpg_%sPass", g_classData[i].consoleName), g_classData[i].consoleName, CVAR_ARCHIVE);
    trap_Cvar_Register(NULL, va("rpg_%sFlags", g_classData[i].consoleName), va("%i", g_classData[i].weaponsFlags), CVAR_ARCHIVE);
  }

  if(classIndex > 0)
  {
    return qtrue;
  }
  else
  {
    G_Printf(S_COLOR_RED "ERROR: No valid classes were found.\n");
    return qfalse;
  }
}

void SP_target_location(gentity_t *ent);

/**
 * @brief Load timed messages.
 * Each line in the cfg file is a single message.
 */
static void G_LoadTimedMessages(void)
{
  fileHandle_t	f = 0;

  const auto len = trap_FS_FOpenFile("timedmessages.cfg", &f, FS_READ);
  if(f == 0)
  {
    return;
  }

  std::string data(len, '\0');
  trap_FS_Read(data.data(), data.size(), f);
  trap_FS_FCloseFile(f);

  std::stringstream stream{ data };

  std::string buffer;
  while(std::getline(stream, buffer))
  {
    level.timedMessages.push_back(buffer);
  }
}

holoData_t holoData;
static void G_LoadHolodeckFile(void)
{
  char			fileRoute[MAX_QPATH];
  char			mapRoute[MAX_QPATH];
  char*			info = NULL;
  fileHandle_t	f = 0;
  char*			buffer = NULL;
  int				file_len;
  char*			txtPtr = NULL;
  char*			token = NULL;
  int				numProgs = 0;

  memset(fileRoute, 0, sizeof(fileRoute));
  memset(mapRoute, 0, sizeof(mapRoute));

  info = (char *)malloc(MAX_INFO_STRING * sizeof(char));
  if(!info)
  {
    G_Printf(S_COLOR_RED "ERROR: Was unable to allocate %i byte.\n", MAX_INFO_STRING * sizeof(char));
    return;
  }
  memset(info, 0, sizeof(info));

  //get the map name out of the server data
  trap_GetServerinfo(info, MAX_INFO_STRING * sizeof(char));

  //setup the file route
  Com_sprintf(mapRoute, sizeof(mapRoute), "maps/%s", Info_ValueForKey(info, "mapname"));

  BG_LanguageFilename(mapRoute, "holodeck", fileRoute);

  file_len = trap_FS_FOpenFile(fileRoute, &f, FS_READ);

  free(info);

  if(f == 0)
  {
    return;
  }

  buffer = (char *)malloc(32000 * sizeof(char));
  if(buffer == NULL)
  {
    G_Printf(S_COLOR_RED "ERROR: Was unable to allocate %i bytes.\n", 32000 * sizeof(char));
    trap_FS_FCloseFile(f);
    return;
  }
  memset(buffer, 0, sizeof(buffer));

  trap_FS_Read(buffer, file_len, f);
  if(buffer[0] == 0)
  {
    G_Printf(S_COLOR_RED "ERROR: Couldn't read in file: %s!\n", fileRoute);
    trap_FS_FCloseFile(f);
    free(buffer);
    return;
  }

  buffer[file_len] = '\0';
  trap_FS_FCloseFile(f);

  memset(&holoData, 0, sizeof(holoData));

  COM_BeginParseSession();
  txtPtr = buffer;

  while(qtrue)
  {
    token = COM_Parse(&txtPtr);
    if(token == NULL || token[0] == 0)
    {
      break;
    }

    if(Q_stricmpn(token, "HolodeckData", 12) == 0)
    {
      token = COM_Parse(&txtPtr);
      if(Q_stricmpn(token, "{", 1) != 0)
      {
        G_Printf(S_COLOR_RED "ERROR: HolodeckData had no opening brace ( { )!\n");
        continue;
      }
      while(Q_stricmpn(token, "}", 1) != 0)
      {
        token = COM_Parse(&txtPtr);
        if(token == NULL || token[0] == 0)
        {
          break;
        }

        if(Q_stricmpn(token, "Program", 7) == 0)
        {
          token = COM_Parse(&txtPtr);
          if(Q_stricmpn(token, "[", 1) != 0)
          {
            G_Printf(S_COLOR_RED "ERROR: Program had no opening brace ( [ )!\n");
            continue;
          }

          // expected format:
          // <string> - target notnull
          // <string> - name
          // <string> - desc1
          // <string> - desc2
          // <string> - image
          while(Q_stricmpn(token, "]", 1) != 0)
          {
            if(token == NULL || token[0] == 0)
            {
              break;
            }

            if(numProgs >= 5)
            {
              free(buffer);
              return;
            }

            // targetname of info_notnull
            token = COM_Parse(&txtPtr);
            Q_strncpyz(holoData.target[numProgs], token, sizeof(holoData.target[numProgs]));

            // parse name
            token = COM_Parse(&txtPtr);
            Q_strncpyz(holoData.name[numProgs], token, sizeof(holoData.name[numProgs]));

            // parse desc1
            token = COM_Parse(&txtPtr);
            Q_strncpyz(holoData.desc1[numProgs], token, sizeof(holoData.desc1[numProgs]));

            // parse desc2
            token = COM_Parse(&txtPtr);
            Q_strncpyz(holoData.desc2[numProgs], token, sizeof(holoData.desc2[numProgs]));

            // parse image
            token = COM_Parse(&txtPtr);
            Q_strncpyz(holoData.image[numProgs], token, sizeof(holoData.image[numProgs]));

            holoData.active = -1;

            numProgs++;

            token = COM_Parse(&txtPtr);
          }
        }
      }
    }
  }

  holoData.numProgs = numProgs;

  free(buffer);
}

/*
File Format is json. Example:

{
  "ServerChangeConfig" :
  [
    { "Server" : { "ip" : "1.1.1.1", "name" : "Server Name" } },
    { "Server" : { "ip" : "1.1.1.2", "name" : "Server Name 2" } }
  ]
}

*/
static void G_LoadServerChangeFile(void)
{
  char fileRoute[MAX_QPATH];
  fileHandle_t	f = 0;

  memset(fileRoute, 0, sizeof(fileRoute));
  BG_LanguageFilename("serverchange", "cfg", fileRoute);

  auto file_len = trap_FS_FOpenFile(fileRoute, &f, FS_READ);

  if(f == 0)
  {
    return;
  }

  std::string buffer(file_len, '\0');
  trap_FS_Read(buffer.data(), buffer.size(), f);
  if(buffer.empty())
  {
    G_LocLogger(LL_ERROR, "Couldn't read in file: %s!\n", fileRoute);
    trap_FS_FCloseFile(f);
    return;
  }

  trap_FS_FCloseFile(f);

  G_Logger(LL_INFO, "Loading ServerChangeConfig '%s'.\n", fileRoute);

  std::stringstream stream{ buffer };

  try
  {
    nlohmann::json j;
    stream >> j;

    auto scc = j.find("ServerChangeConfig");
    if(scc == j.end())
    {
      throw std::invalid_argument("Could not find element: ServerChangeConfig");
    }

    for(const auto& server : scc.value())
    {
      auto srv = server.find("Server");
      if(srv == server.end())
      {
        throw std::invalid_argument("Could not find element: Server");
      }

      auto ip = srv.value().find("ip");
      if(ip == srv.value().end())
      {
        throw std::invalid_argument("Could not find element: ip");
      }

      auto name = srv.value().find("name");
      if(name == srv.value().end())
      {
        throw std::invalid_argument("Could not find element: name");
      }

      level.srvChangeData.push_back({ ip.value().get<std::string>(), name.value().get<std::string>() });
    }
  }
  catch(std::exception& ex)
  {
    G_Logger(LL_ERROR, "An error occured while loading the ServerChangeConfig '%s': %s\n", fileRoute, ex.what());
  }
}


mapChangeData_t mapChangeData;
static void G_LoadMapChangeFile(void)
{
  char			fileRoute[MAX_QPATH];
  fileHandle_t	f = 0;
  char*			buffer;
  int				file_len;
  char*			txtPtr, *token;
  int				cnt = 0;
  int				i = 0;

  memset(fileRoute, 0, sizeof(fileRoute));
  BG_LanguageFilename("mapchange", "cfg", fileRoute);

  file_len = trap_FS_FOpenFile(fileRoute, &f, FS_READ);

  if(f == 0)
  {
    return;
  }

  buffer = (char *)malloc(32000 * sizeof(char));
  if(buffer == NULL)
  {
    G_Printf(S_COLOR_RED "ERROR: Was unable to allocate %i bytes.\n", 32000 * sizeof(char));
    trap_FS_FCloseFile(f);
    return;
  }
  memset(buffer, 0, sizeof(buffer));

  trap_FS_Read(buffer, file_len, f);
  if(buffer[0] == 0)
  {
    G_Printf(S_COLOR_RED "ERROR: Couldn't read in file: %s!\n", fileRoute);
    trap_FS_FCloseFile(f);
    free(buffer);
    return;
  }

  buffer[file_len] = '\0';
  trap_FS_FCloseFile(f);

  memset(&mapChangeData, 0, sizeof(mapChangeData));

  COM_BeginParseSession();
  txtPtr = buffer;

  while(qtrue)
  {
    token = COM_Parse(&txtPtr);
    if(token == NULL || token[0] == 0)
    {
      break;
    }

    if(Q_stricmp(token, "MapChangeConfig") == 0)
    {
      token = COM_Parse(&txtPtr);
      if(Q_strncmp(token, "{", 1) != 0)
      {
        G_Printf(S_COLOR_RED "ERROR: MapChangeConfig had no opening brace ( { )!\n");
        continue;
      }

      while(Q_strncmp(token, "}", 1) != 0)
      {
        token = COM_Parse(&txtPtr);
        if(token == NULL || token[0] == 0)
        {
          break;
        }

        if(Q_stricmp(token, "Map") == 0)
        {
          token = COM_Parse(&txtPtr);
          if(Q_strncmp(token, "[", 1) != 0)
          {
            G_Printf(S_COLOR_RED "ERROR: Server had no opening brace ( [ )!\n");
            continue;
          }

          token = COM_Parse(&txtPtr);
          while(Q_strncmp(token, "]", 1) != 0)
          {
            if(token == NULL || token[0] == 0)
            {
              break;
            }

            if(cnt > 12) break;

            if(cnt % 2 == 0)
              Q_strncpyz(mapChangeData.name[i], token, sizeof(mapChangeData.name[i]));
            else
              Q_strncpyz(mapChangeData.bspname[i], token, sizeof(mapChangeData.bspname[i]));

            cnt++;
            if(cnt % 2 == 0)
              i++;

            token = COM_Parse(&txtPtr);
          }
        }
      }
    }
  }

  free(buffer);
}

/*
File format is json. Example:
{
  "LocationsList" :
  [
    { "position" : [ 100.0, 200.0, 300.0 ], "angles" : [ 0.0, 42.0, -24 ], "name" : "A Name" },
    { "position" : [ 100.0, 500.0, 300.0 ], "angles" : [ 0.0, 42.0, 0 ], "name" : "Another Name" }
  ]
}
*/
static void G_LoadLocationsFile(void)
{
  char			fileRoute[MAX_QPATH];
  char			mapRoute[MAX_QPATH];
  fileHandle_t	f = 0;
  int				file_len;

  memset(fileRoute, 0, sizeof(fileRoute));
  memset(mapRoute, 0, sizeof(mapRoute));

  std::string serverInfo(MAX_INFO_STRING, '\0');

  //get the map name out of the server data
  trap_GetServerinfo(serverInfo.data(), serverInfo.size());

  //setup the file route
  Com_sprintf(mapRoute, sizeof(mapRoute), "maps/%s", Info_ValueForKey(serverInfo.data(), "mapname"));

  BG_LanguageFilename(mapRoute, "locations", fileRoute);

  file_len = trap_FS_FOpenFile(fileRoute, &f, FS_READ);

  if(f <= 0)
  {
    return;
  }

  std::string buffer(file_len, '\0');
  trap_FS_Read(buffer.data(), buffer.size(), f);
  if(buffer.empty())
  {
    G_LocLogger(LL_ERROR, "Couldn't read in file: %s!\n", fileRoute);
    trap_FS_FCloseFile(f);
    return;
  }

  trap_FS_FCloseFile(f);

  G_Logger(LL_INFO, "Locations file %s located. Proceeding to load scan data.\n", fileRoute); //GSIO01: why did this say "Usables file ..."? lol ;)

  try
  {
    std::stringstream stream{ buffer };
    nlohmann::json j;
    stream >> j;

    auto locList = j.find("LocationsList");
    if(locList == j.end())
    {
      throw std::invalid_argument("Could not find LocationsList element");
    }

    for(const auto& loc : locList.value())
    {
      auto pos = loc.find("position");
      if(pos == loc.end())
      {
        throw std::invalid_argument("Could not find position element");
      }

      auto angles = loc.find("angles");
      if(angles == loc.end())
      {
        throw std::invalid_argument("Could not find angles element");
      }

      auto name = loc.find("name");
      if(name == loc.end())
      {
        throw std::invalid_argument("Could not find name element");
      }

      auto restricted = 0;
      auto rest = loc.find("restricted");
      if(rest != loc.end())
      {
        restricted = rest.value().get<bool>() ? 1 : 0;
      }

      auto ent = G_Spawn();
      if(ent == nullptr)
      {
        throw std::runtime_error("failed to spawn new entity");
      }

      ent->classname = "target_location";

      //copy position data
      ent->s.origin[0] = pos.value().at(0).get<float>();
      ent->s.origin[1] = pos.value().at(1).get<float>();
      ent->s.origin[2] = pos.value().at(2).get<float>();
      ent->s.angles[0] = angles.value().at(0).get<float>();
      ent->s.angles[1] = angles.value().at(1).get<float>();
      ent->s.angles[2] = angles.value().at(2).get<float>();

      //copy string
      ent->message = G_NewString(name.value().get<std::string>().data());

      //copy desc into target as well
      ent->target = ent->targetname = G_NewString(name.value().get<std::string>().data());

      // copy restriction value
      ent->sound1to2 = restricted;

      //initiate it as a location ent
      SP_target_location(ent);

      //reset the ent
      ent = nullptr;
    }
  }
  catch(std::exception& ex)
  {
    G_Logger(LL_ERROR, "Error loading locations file '%s': %s\n", fileRoute, ex.what());
  }
}


char *G_searchGroupList(const char *name)
{
  char	*text_p = NULL, *slash = NULL;
  char	text[MAX_GROUP_FILE_SIZE];
  int		i;
  char	mod_name[200];

  memset(mod_name, 0, sizeof(mod_name));
  memset(races, 0, sizeof(races));
  memset(text, 0, sizeof(text));

  // check to see if there is a '/' in the name
  Q_strncpyz(mod_name, name, sizeof(mod_name));
  slash = strstr(mod_name, "/");
  if(slash != NULL)
  {//drop the slash and everything after it for the purpose of finding the model name in th group
    *slash = 0;
  }

  // find the name in the group list
  for(i = 0; i < group_count; i++)
  {
    if(Q_stricmp(mod_name, group_list[i].name) == 0)
    {
      text_p = group_list[i].text;
      break;
    }
  }

  // did we find this group in the list?
  if(i == group_count || text_p == NULL)
  {
    Com_sprintf(races, sizeof(races), "unknown");
  }
  else
  {
    Com_sprintf(races, sizeof(races), "%s", text_p);
  }
  return races;

}


/*
================
G_FindTeams

Chain together all entities with a matching team field.
Entity teams are used for item groups and multi-entity mover groups.

All but the first will have the FL_TEAMSLAVE flag set and teammaster field set
All but the last will have the teamchain field set to the next one
================
*/
static void G_FindTeams(void)
{
  /*@shared@*/ /*@null@*/ gentity_t* e;
  /*@shared@*/ /*@null@*/ gentity_t* e2;
  int		i, j;
  int		c, c2;

  c = 0;
  c2 = 0;
  for(i = 1, e = g_entities + i; i < level.num_entities; i++, e++)
  {
    if(!e->inuse)
      continue;
    if(!e->team)
      continue;
    if((e->flags & FL_TEAMSLAVE) != 0)
      continue;
    if((e->classname != NULL) && (e->type != EntityType::ENT_FUNC_DOOR))
    {//not a door
      if(Q_stricmp("1", e->team) == 0 || Q_stricmp("2", e->team) == 0)
      {//is trying to tell us it belongs to the TEAM_RED or TEAM_BLUE
        continue;
      }
    }
    e->teammaster = e;
    c++;
    c2++;
    for(j = i + 1, e2 = e + 1; j < level.num_entities; j++, e2++)
    {
      if(!e2->inuse)
        continue;
      if(!e2->team)
        continue;
      if((e2->flags & FL_TEAMSLAVE) != 0)
        continue;
      if(strcmp(e->team, e2->team) == 0)
      {
        c2++;
        e2->teamchain = e->teamchain;
        e->teamchain = e2;
        e2->teammaster = e;
        e2->flags |= FL_TEAMSLAVE;

        // make sure that targets only point at the master
        if(e2->targetname)
        {
          e->targetname = e2->targetname;
          e2->targetname = NULL;
        }
      }
    }
  }

  G_Printf("%i teams with %i entities\n", c, c2);
}

/*
=================
G_RegisterCvars
=================
*/
static void G_RegisterCvars(void)
{
  int			i;
  cvarTable_t	*cv;

  for(i = 0, cv = gameCvarTable; i < gameCvarTableSize; i++, cv++)
  {

    if(rpg_rpg.integer != 0)
    {
      trap_Cvar_Set("g_gametype", "0");//RPG-X: RedTechie - Make sure we keep gametype at 0
    }

    trap_Cvar_Register(cv->vmCvar, cv->cvarName, cv->defaultString, cv->cvarFlags);
    if(cv->vmCvar)
    {
      cv->modificationCount = cv->vmCvar->modificationCount;
    }
  }

  // check some things

  if(g_gametype.integer < 0 || g_gametype.integer >= GT_MAX_GAME_TYPE)
  {
    G_Printf("g_gametype %i is out of range, defaulting to 0\n", g_gametype.integer);
    trap_Cvar_Set("g_gametype", "0");
  }
}

/*
=================
G_UpdateCvars
=================
*/
static void G_UpdateCvars(void)
{
  int			i;
  cvarTable_t	*cv;

  for(i = 0, cv = gameCvarTable; i < gameCvarTableSize; i++, cv++)
  {
    if(cv->vmCvar)
    {
      trap_Cvar_Update(cv->vmCvar);

      if(cv->modificationCount != cv->vmCvar->modificationCount)
      {
        cv->modificationCount = cv->vmCvar->modificationCount;

        if(cv->trackChange)
        {
          if(!levelExiting)
          {//no need to do this during level changes
//RPG-X: J2J - Don't show gravity changed messages any more (for the gravity target ents)
            if(Q_stricmp(cv->cvarName, "g_gravity") != 0)
            {
              trap_SendServerCommand(-1, va("print \"Server: %s changed to %s\n\"",
                cv->cvarName, cv->vmCvar->string));
            }
          }
        }
      }
    }
  }
}

extern int altAmmoUsage[];
static void G_InitModRules(void)
{
  numKilled = 0;
}

static void Dev_ShowTriggers(gentity_t *ent)
{
  int i;
  gentity_t *tar;

  ent->nextthink = level.time + 9500;

  for(i = 0; i < MAX_GENTITIES; i++)
  {
    if((tar = &g_entities[i]) == NULL) continue;
    if(tar->type >= EntityType::ENT_TRIGGER_ALWAYS && tar->type < EntityType::ENT_TARGET_REMOVE_POWERUPS)
    {
      if(tar->type == EntityType::ENT_TRIGGER_ALWAYS)
      {
        continue;
      }

      if((tar->r.svFlags & SVF_NOCLIENT) != 0)
      {
        tar->r.svFlags ^= SVF_NOCLIENT;
      }

      trap_LinkEntity(ent);
      if(tar->type == EntityType::ENT_TRIGGER_PUSH)
      {
        G_AddEvent(tar, EV_TRIGGER_SHOW, 1);
      }
      else
      {
        G_AddEvent(tar, EV_TRIGGER_SHOW, 0);
      }
    }
  }
}


/*
============
G_InitGame
============
*/
extern int lastKillTime[];
extern int LastFKRadius[];					//(RPG-X J2J) added so array can be initialised to 0 first.
extern RPGX_SiteTOSiteData TransDat[];		//(RPG-X J2J) added for tricorder transporter
extern RPGX_DragData DragDat[];
void G_InitGame(int levelTime, unsigned int randomSeed, int restart)
{
  int					i;
  char				fileName[MAX_QPATH];
  float				messageTime;

  G_Printf("------- Game Initialization -------\n");
  G_Printf("gamename: %s\n", GAMEVERSION);
  G_Printf("gamedate: %s\n", __DATE__);
  G_Printf("This is RPG-X version %s compiled by %s on %s.\n", RPGX_VERSION, RPGX_COMPILEDBY, RPGX_COMPILEDATE);

  level.overrideCalcLiftTravelDuration = 0;

  init_tonextint(qtrue);
  srand(randomSeed);

  numKilled = 0;
  level.numObjectives = 0;//no objectives by default

  G_RegisterCvars();

  G_ProcessIPBans();
  G_ProcessIDBans();

  G_InitMemory();

  //G_initGroupsList();

  BG_LoadItemNames();

  // set some level globals
  memset(&level, 0, sizeof(level));

  //Class loader
  memset(fileName, 0, sizeof(fileName));
  Com_sprintf(fileName, sizeof(fileName), "ext_data/classes/%s.classes", rpg_classSet.string);
  if(!G_LoadClassData(fileName))
  {
    G_Printf(S_COLOR_RED "ERROR: Could not load class set %s. Reverting to default.\n", fileName);
    trap_Cvar_Set("rpg_classSet", CLASS_DEFAULT);

    if(!G_LoadClassData(va("ext_data/classes/%s.classes", CLASS_DEFAULT)))
    {
      G_Error("Could not load default class set: %s", CLASS_DEFAULT);
    }
  }

  //Rank Loader
  memset(fileName, 0, sizeof(fileName));
  Com_sprintf(fileName, sizeof(fileName), "ext_data/ranksets/%s.ranks", rpg_rankSet.string);
  if(!BG_ParseRankNames(fileName, g_rankNames, sizeof(rankNames_t)* MAX_RANKS))
  {
    G_Printf(S_COLOR_RED "ERROR: Could not load rankset %s. Reverting to default.\n", fileName);
    trap_Cvar_Set("rpg_rankSet", RANKSET_DEFAULT);

    if(!BG_ParseRankNames(va("ext_data/ranksets/%s.ranks", RANKSET_DEFAULT), g_rankNames, sizeof(rankNames_t)* MAX_RANKS))
    {
      G_Error("Could not load default rankset: %s", RANKSET_DEFAULT);
    }
  }

  level.time = levelTime;
  level.startTime = levelTime;
  level.restarted = (qboolean)restart;

  //level.message = levelTime - (int)(rpg_timedmessagetime.value * 60000) + 30000;
  if(rpg_timedmessagetime.value < 0.2)
  {
    messageTime = 0.2;
  }
  else
  {
    messageTime = rpg_timedmessagetime.value;
  }

  level.message = levelTime + (messageTime * 60000);

  level.snd_fry = G_SoundIndex("sound/player/fry.wav");	// FIXME standing in lava / slime

  if(g_gametype.integer != GT_SINGLE_PLAYER && g_log.string[0] != 0)
  {
    if(g_logSync.integer != 0)
    {
      trap_FS_FOpenFile(g_log.string, &level.logFile, FS_APPEND_SYNC);
    }
    else
    {
      trap_FS_FOpenFile(g_log.string, &level.logFile, FS_APPEND);
    }
    if(level.logFile == 0)
    {
      G_Printf("WARNING: Couldn't open logfile: %s\n", g_log.string);
    }
    else
    {
      char	serverinfo[MAX_INFO_STRING];

      memset(serverinfo, 0, sizeof(serverinfo));
      trap_GetServerinfo(serverinfo, sizeof(serverinfo));

      G_LogPrintf("------------------------------------------------------------\n");
      G_LogPrintf("InitGame: %s\n", serverinfo);
    }
  }
  else
  {
    G_Printf("Not logging to disk.\n");
  }

#ifdef G_LUA
  G_Lua_Init();
#endif

  G_Weapon_LoadConfig();

  G_LogWeaponInit();

  G_InitWorldSession();

  // initialize all entities for this game
  memset(g_entities, 0, MAX_GENTITIES * sizeof(g_entities[0]));
  level.gentities = g_entities;

  // initialize all clients for this game
  level.maxclients = g_maxclients.integer;
  memset(g_clients, 0, MAX_CLIENTS * sizeof(g_clients[0]));
  level.clients = g_clients;

  // set client fields on player ents
  for(i = 0; i < level.maxclients; i++)
  {
    g_entities[i].client = level.clients + i;
  }

  // always leave room for the max number of clients,
  // even if they aren't all used, so numbers inside that
  // range are NEVER anything but clients
  level.num_entities = MAX_CLIENTS;

  // let the server system know where the entites are
  trap_LocateGameData(level.gentities, level.num_entities, (int)sizeof(gentity_t),
    &level.clients[0].ps, (int)sizeof(level.clients[0]));

  // reserve some spots for dead player bodies
  G_Client_InitBodyQue();

  ClearRegisteredItems();

  // parse the map usables file
  G_Usable_SetupUsablesStrings();

  // parse the key/value pairs and spawn gentities
  G_SpawnEntitiesFromString();

  //TiM: load a locations file, whereever possible
  G_LoadLocationsFile();

  //GSIO: load server change file if avaible
  G_LoadServerChangeFile();

  //GSIO: load map change file if avaible
  G_LoadMapChangeFile();

  //GSIO: load holodeck file if avaible
  G_LoadHolodeckFile();

  //GSIO: load timed message
  G_LoadTimedMessages();

  // general initialization
  G_FindTeams();

  SaveRegisteredItems();

  G_Printf("-----------------------------------\n");

  if(g_gametype.integer == GT_SINGLE_PLAYER || trap_Cvar_VariableIntegerValue("com_buildScript") != 0)
  {
    G_ModelIndex(SP_PODIUM_MODEL);
    G_SoundIndex("sound/player/gurp1.wav");
    G_SoundIndex("sound/player/gurp2.wav");
  }
  if(g_gametype.integer >= GT_TEAM || trap_Cvar_VariableIntegerValue("com_buildScript") != 0)
  {
    G_ModelIndex(TEAM_PODIUM_MODEL);
  }

  if(trap_Cvar_VariableIntegerValue("bot_enable") != 0)
  {
    AI_main_BotAISetup(restart);
    AI_main_BotAILoadMap(restart);
    G_InitBots((qboolean)restart);
  }

  G_InitModRules();

  levelExiting = qfalse;

  /*RPG-X J2J************************************************************************************/
  G_Printf("Initializing RPG-X Globals...");

  //WARNING - used hard coded number to shut up compiler, 1023 is MAX_ENTITIES (which apperently cant be increased without substansial exe recoding)
  //TiM : NB Ents 0 -> 128 are clients only. cyclicng thru all ents here is not needed.
  for(i = 0; i < MAX_CLIENTS; i++)
  {
    //RPG-X: Redtechie - Make sure score cant be chnaged unless other wise told to
    if(g_entities[i].client)
    {
      g_entities[i].client->UpdateScore = qfalse;
      g_entities[i].client->AdminFailed = 0; // RPG-X | Phenix | 21/11/2004 | Init Admin Failed Integer
      g_entities[i].client->LoggedAsAdmin = qfalse;
      g_entities[i].client->LoggedAsDeveloper = qfalse;
    }
  }

  for(i = 0; i < MAX_CLIENTS; i++)
  {
    lastKillTime[i] = level.time - 30000;

    //FKR
    LastFKRadius[i] = level.time - rpg_forcekillradiuswaittime.integer;

    //Transporter
    memset(&TransDat[i], 0, sizeof(TransDat[i]));
    TransDat[i].LastClick = level.time - 5000;				//Make last click current time less 5 seconds.

    //Drag Data
    DragDat[i].AdminId = -1;
  }

  G_Printf("%i entities loaded.\n", level.num_entities);

  G_Printf("COMPLETED\n");

#ifdef G_LUA
  LuaHook_G_InitGame(levelTime, randomSeed, restart);
#endif

  if(dev_showTriggers.integer != 0 && restart == 0)
  {
    gentity_t *t;
    t = G_Spawn();
    if(t)
    {
      t->think = Dev_ShowTriggers;
      t->nextthink = level.time + 1000;
    }
  }


#ifdef SQL
  if(G_Sql_Init())
  {
    G_Printf("SQL initialization successful.\n");
  }
  else
  {
    G_Printf(S_COLOR_RED "Error: SQL initialization failed!\n");
  }
#endif

  /*************************************************************************************************/

  G_Printf("                       ,.                      \n");	  G_Printf("          ..:,        :Xt.       ,:.            \n");
  G_Printf("         ,=+t:       .IRX=       :++=.         \n");	  G_Printf("        .=iVt:.      :RYYI.      .itt+         \n");
  G_Printf("       .:tXI=;.      tRtiV;       ,IYY:.       \n");	  G_Printf("      .+;ii=;.      ,XVi+Vt.       :tIi+      \n");
  G_Printf("     .;ti;;:.       +RI++IY,        ,+tt=.     \n");	  G_Printf("    ,++YY;.        ,XXi+++X=         ;IYI=.    \n");
  G_Printf("    ;ttY+;.    .,=iVRI++++YX+;.       ;VYt;    \n");	  G_Printf("   .;ii+=,   .;IXRRXVi++++iVRXVi:.    ,=iii.   \n");
  G_Printf("  .==;ti,  .;YRRVVXYii+++++IVIVRXt,   ,+=tI=   \n");	  G_Printf("  .iitY=, .tRRVXXVRV+++ii++YRXVIYXV;   :tYti,  \n");
  G_Printf("  .+iii=,,IBVVXYiiXViiiiiiitVtIXViVR=  ,+t+I:  \n");	  G_Printf("   =+=I:.tBVXVt=;tRIiiiiiiiiXi:=YXiIX; :+=It;  \n");
  G_Printf(" .;;tYt:;RVVV+=:,YRiiiiiiiiiYI,.:IXiVY..+IYi=  \n");	  G_Printf(" .ti=t+;tRIXi;, :XViiiiiiiiiIV:  ,YViX=.:titt. \n");
  G_Printf("  iY++I;YVYY=:  +BIiiiiiiiiiiX=   +XiVi;i++Vi, \n");    G_Printf(" ,+YYYI:VYYY;. .YRiiiiiiiiiiiVt.  ;RIYt:IIVVi: \n");
  G_Printf(" ,+tYXi;YVIX;  ;RVtiiiiIXXtiiVI,  iRIVt,=XVit: \n");    G_Printf(" .+iiti++XiXI. iBIiiiiYXIIXtiIV: :XXIV++;i+iI;.\n");
  G_Printf("  ;Ii=ii:VYtRi,VRtiiiVVi=;IXitX=;VBYXI=i+;iV+;.\n");    G_Printf("  ;tYtVt;;XYIRXBVttiVV+;:.:VYiXVRBVXY+;+IYVt+, \n");
  G_Printf("  =iiItii,=XVIRRIttXV+=:..,tRtVBXVRI+=i:iIit+. \n");    G_Printf("  :t==++I:.=YXYIIiYBXYIttIVRBYtVXXI+;;t+;;+Y=, \n");
  G_Printf("   +I=;+Y= .:IRItYIVXRRRBBRXXVIRY+=;.:i=;iVi;. \n");    G_Printf("   .+IYVV+:  +BYXXVXXXXXXXXXVRVVi;:.:;tVYY+=:  \n");
  G_Printf("    .+ttii+ .IBXY++ittIIIti++tXXi, .++=tI+;:   \n");    G_Printf("     ;YYtIY;;VBI+;:,::;;;;;:,:IBt,::tItYV=.    \n");
  G_Printf("      =IYYI++ti+;,   .......  :Xt;i=iYYI+;.    \n");    G_Printf("      .:+i++ii;;.             .=i=+i=t+;;:.    \n");
  G_Printf("        ,tYIVI==:,..       ..,;=+iYIVt:..      \n");    G_Printf("         ,itt+iIYYti;.   ,;itYIIt:iIi=;.       \n");
  G_Printf("          .:;;:+tIIVIi:.;iYYIii+=:,;;:.        \n");    G_Printf("            .  ,:=itIXi.tXYit=;::,  .          \n");
  G_Printf("                 .+tti=,,iIt+;.                \n");    G_Printf("                  .:;;:. ,;;;:.                \n");

    }

void G_ShutdownGame(int restart)
{
  G_Printf("==== ShutdownGame ====\n");

#ifdef G_LUA
  LuaHook_G_Shutdown(restart);
  G_Lua_Shutdown();
#endif

#if 0	// kef -- Pat sez this is causing some trouble these days
  G_LogWeaponOutput();
#endif
  if(level.logFile != 0)
  {
    G_LogPrintf("ShutdownGame:\n");
    G_LogPrintf("------------------------------------------------------------\n");
    trap_FS_FCloseFile(level.logFile);
  }

  // write all the client session data so we can get it back
  G_WriteSessionData();

  if(trap_Cvar_VariableIntegerValue("bot_enable") != 0)
  {
    AI_main_BotAIShutdown(restart);
  }

  level.timedMessages.clear();
  level.timedMessageIndex = 0;

#ifdef SQL
  G_Sql_Shutdown();
#endif
}



//===================================================================

#ifndef GAME_HARD_LINKED
// this is only here so the functions in q_shared.c and bg_*.c can link

void QDECL Com_Error( /*@unused@*/ int32_t errlevel, const char *error, ...)
{
  va_list		argptr;
  char		text[1024];

  va_start(argptr, error);
  vsnprintf(text, sizeof(text), error, argptr);
  va_end(argptr);

  G_Error("%s", text);
}

void QDECL Com_Printf(const char *msg, ...)
{
  va_list		argptr;
  char		text[1024];

  va_start(argptr, msg);
  vsnprintf(text, sizeof(text), msg, argptr);
  va_end(argptr);

  G_Printf("%s", text);
}

#endif

/*
========================================================================

PLAYER COUNTING / SCORE SORTING

========================================================================
*/

/*
=======================
RemoveTournamentLoser

Make the loser a spectator at the back of the line
=======================
*/
static void RemoveTournamentLoser(void)
{
  int			clientNum;

  if(level.numPlayingClients != 2)
  {
    return;
  }

  clientNum = level.sortedClients[1];

  if(level.clients[clientNum].pers.connected != CON_CONNECTED)
  {
    return;
  }

  // make them a spectator
  SetTeam(&g_entities[clientNum], "s");
}


/*
=======================
AdjustTournamentScores

=======================
*/
static void AdjustTournamentScores(void)
{
  int			clientNum;

  clientNum = level.sortedClients[0];
  if(level.clients[clientNum].pers.connected == CON_CONNECTED)
  {
    level.clients[clientNum].sess.wins++;
    G_Client_UserinfoChanged(clientNum);
  }

  clientNum = level.sortedClients[1];
  if(level.clients[clientNum].pers.connected == CON_CONNECTED)
  {
    level.clients[clientNum].sess.losses++;
    G_Client_UserinfoChanged(clientNum);
  }

}


/*
========================================================================

MAP CHANGING

========================================================================
*/

/*
========================
SendScoreboardMessageToAllClients

Do this at G_Client_BeginIntermission time and whenever ranks are recalculated
due to enters/exits/forced team changes
========================
*/
void SendScoreboardMessageToAllClients(void)
{
  int		i;

  for(i = 0; i < level.maxclients; i++)
  {
    if(level.clients[i].pers.connected == CON_CONNECTED)
    {
      DeathmatchScoreboardMessage(g_entities + i);
    }
  }
}

void MoveClientToIntermission(gentity_t *ent)
{
  entityState_t* es;
  playerState_t* ps;

  if(ent == NULL || ent->client == NULL)
  {
    return;
  }

  ps = &ent->client->ps;
  es = &ent->s;

  // take out of follow mode if needed
  if(ent->client->sess.spectatorState == SPECTATOR_FOLLOW)
  {
    StopFollowing(ent);
  }


  // move to the spot
  VectorCopy(level.intermission_origin, es->origin);
  VectorCopy(level.intermission_origin, ps->origin);
  VectorCopy(level.intermission_angle, ps->viewangles);
  ps->pm_type = PM_INTERMISSION;
  ps->eFlags ^= EF_TELEPORT_BIT;

  // clean up powerup info
  memset(ps->powerups, 0, sizeof(ps->powerups));

  ps->eFlags = 0;
  es->eFlags = 0;
  es->eType = ET_GENERAL;
  es->modelindex = 0;
  es->loopSound = 0;
  es->event = 0;
  ent->r.contents = 0;
}

void FindIntermissionPoint(void)
{
  gentity_t	*ent, *target;
  vec3_t		dir;

  // find the intermission spot
  ent = G_Find(NULL, FOFS(classname), "info_player_intermission");
  if(ent == NULL)
  {	// the map creator forgot to put in an intermission point...
    G_Client_SelectSpawnPoint(vec3_origin, level.intermission_origin, level.intermission_angle);
  }
  else
  {
    VectorCopy(ent->s.origin, level.intermission_origin);
    VectorCopy(ent->s.angles, level.intermission_angle);
    // if it has a target, look towards it
    if(ent->target)
    {
      target = G_PickTarget(ent->target);
      if(target)
      {
        VectorSubtract(target->s.origin, level.intermission_origin, dir);
        vectoangles(dir, level.intermission_angle);
      }
    }
  }

}

/*
==================
ClearFiringFlags
==================
*/
static void ClearFiringFlags(void)
{
  int i = 0;
  gentity_t	*ent = NULL;

  for(i = 0; i < level.maxclients; i++)
  {
    ent = g_entities + i;
    if(!ent->inuse)
      continue;
    // clear the firing flag
    if(ent->client)
    {
      ent->client->ps.eFlags &= ~EF_FIRING;
    }
  }
}

/*
==================
G_Client_BeginIntermission
==================
*/
void G_Client_BeginIntermission(void)
{
  int			i;
  gentity_t	*client;
  qboolean	doingLevelshot;

  if(level.intermissiontime == -1)
    doingLevelshot = qtrue;
  else
    doingLevelshot = qfalse;

  if(level.intermissiontime != 0 && level.intermissiontime != -1)
  {
    return;		// already active
  }

  // if in tournament mode, change the wins / losses
  if(g_gametype.integer == GT_TOURNAMENT)
  {
    AdjustTournamentScores();
  }

  level.intermissiontime = level.time;
  FindIntermissionPoint();

  // kef -- make sure none of the players are still firing (cuz we don't want weapons fx going off while
  //they're on the podium)
  ClearFiringFlags();


  // cdr - Want to generate victory pads for all game types  - except level shots (gametype 10)
  UpdateTournamentInfo();
  if(!doingLevelshot)
    SpawnModelsOnVictoryPads();


  // move all clients to the intermission point
  for(i = 0; i < level.maxclients; i++)
  {
    client = g_entities + i;
    if(!client->inuse)
      continue;
    if(client->health <= 0)
    {
      G_Client_Respawn(client);
    }
    MoveClientToIntermission(client);

  }

  // send the current scoring to all clients
  SendScoreboardMessageToAllClients();
}


static void G_ClearObjectives(void)
{
  gentity_t *tent;

  tent = G_TempEntity(vec3_origin, EV_OBJECTIVE_COMPLETE);

  if(tent == NULL)
  {
    return;
  }

  //Be sure to send the event to everyone
  tent->r.svFlags |= SVF_BROADCAST;
  tent->s.eventParm = 0;//tells it to clear all
}
/*
=============
ExitLevel

When the intermission has been exited, the server is either killed
or moved to a new level based on the "nextmap" cvar

=============
*/
qboolean levelExiting = qfalse;
void ExitLevel(void)
{
  int		i;

  levelExiting = qtrue;

  //bot interbreeding
  AI_main_BotInterbreedEndMatch();

  G_ClearObjectives();
  // if we are running a tournement map, kick the loser to spectator status,
  // which will automatically grab the next spectator and restart
  if(g_gametype.integer == GT_TOURNAMENT)
  {
    if(!level.restarted)
    {
      RemoveTournamentLoser();
      trap_SendConsoleCommand(EXEC_APPEND, "map_restart 0\n");
      level.restarted = qtrue;
      level.intermissiontime = 0;
    }
    return;
  }

  trap_SendConsoleCommand(EXEC_APPEND, "vstr nextmap\n");
  level.intermissiontime = 0;

  // we need to do this here before chaning to CON_CONNECTING
  G_WriteSessionData();

  // change all client states to connecting, so the early players into the
  // next level will know the others aren't done reconnecting
  for(i = 0; i < g_maxclients.integer; i++)
  {
    if(level.clients[i].pers.connected == CON_CONNECTED)
    {
      level.clients[i].pers.connected = CON_CONNECTING;
    }
  }

}

/*
========================================================================

FUNCTIONS CALLED EVERY FRAME

========================================================================
*/


/*
=============
CheckTournement

Once a frame, check for changes in tournement player state
=============
*/
static void CheckTournement(void)
{
  if(level.numPlayingClients == 0)
  {
    return;
  }

  if(g_gametype.integer == GT_TOURNAMENT)
  {
    if(level.warmupTime == 0 || level.warmupTime != 0)
    {//RPG-X: RedTechie - No warmup Fail safe
      return;
    }
  }
  else if(g_gametype.integer != GT_SINGLE_PLAYER)
  {
    if(level.warmupTime == 0)
    {
      return;
    }
  }
}


/*
==================
CheckVote
==================
*/
static void CheckVote(void)
{
  if(level.voteTime == 0)
  {
    return;
  }
  if(level.time - level.voteTime >= VOTE_TIME)
  {
    trap_SendServerCommand(-1, "print \"Vote failed.\n\"");
  }
  else
  {
    if(level.voteYes > level.numVotingClients / 2)
    {
      // execute the command, then remove the vote
      char message[1024] = "";
      trap_SendServerCommand(-1, "print \"Vote passed.\n\"");
      Com_sprintf(message, 1024, "%s\n", level.voteString);
      trap_SendConsoleCommand(EXEC_APPEND, message);
    }
    else if(level.voteNo >= level.numVotingClients / 2)
    {
      // same behavior as a timeout
      trap_SendServerCommand(-1, "print \"Vote failed.\n\"");
    }
    else
    {
      // still waiting for a majority
      return;
    }
  }
  level.voteTime = 0;
  trap_SetConfigstring(CS_VOTE_TIME, "");

}


/*
==================
CheckCvars
==================
*/
static void CheckCvars(void)
{
  static int lastMod = -1;

  if(g_password.modificationCount != lastMod)
  {
    lastMod = g_password.modificationCount;
    if(g_password.string[0] != 0 && Q_stricmp(g_password.string, "none") != 0)
    {
      trap_Cvar_Set("g_needpass", "1");
    }
    else
    {
      trap_Cvar_Set("g_needpass", "0");
    }
  }
}

/*
=============
G_RunThink

Runs thinking code for this frame if necessary
=============
*/
void G_RunThink(gentity_t *ent)
{
  int thinktime;

  if(ent == NULL)
  {
    return;
  }

  thinktime = ent->nextthink;
  if(thinktime <= 0)
  {
    return;
  }
  if(thinktime > level.time)
  {
    return;
  }

  ent->nextthink = 0;
  if(ent->think == NULL)
  {
    G_Error("NULL ent->think");
    return;
  }

#ifdef G_LUA
  if(ent->luaThink != NULL && ent->client == NULL)
  {
    LuaHook_G_EntityThink(ent->luaThink, ent->s.number);
  }
#endif

  ent->think(ent);
}

/*
================
G_RunFrame

Advances the non-player objects in the world
================
*/

void CheckHealthInfoMessage(void);
void G_RunFrame(int32_t levelTime)
{
  int32_t			i;
  gentity_t*		ent;
  gclient_t*		client;
  playerState_t*	ps;
  entityState_t*	es;

  // if we are waiting for the level to restart, do nothing
  if(level.restarted)
  {
    return;
  }

  level.framenum++;
  level.previousTime = level.time;
  level.time = levelTime;

  // get any cvar changes
  G_UpdateCvars();

  //
  // go through all allocated objects
  //
  ent = &g_entities[0];
  for(i = 0; i < level.num_entities; i++, ent++)
  {
    if(ent == NULL || ent->inuse == qfalse)
    {
      continue;
    }

    es = &ent->s;
    ps = &ent->client->ps;

    // clear events that are too old
    if(level.time - ent->eventTime > EVENT_VALID_MSEC)
    {
      if(es->event != 0)
      {
        es->event = 0;
        if(ent->client)
        {
          ps->externalEvent = 0;
          ps->events[0] = 0;
          ps->events[1] = 0;
          ps->events[2] = 0;
          ps->events[3] = 0;
        }
      }
      if(ent->freeAfterEvent)
      {
        // tempEntities or dropped items completely go away after their event
        G_FreeEntity(ent);
        continue;
      }
      else if(ent->unlinkAfterEvent)
      {
        // items that will respawn will hide themselves after their pickup event
        ent->unlinkAfterEvent = qfalse;
        trap_UnlinkEntity(ent);
      }
    }

    // temporary entities don't think
    if(ent->freeAfterEvent)
    {
      continue;
    }

    if(!ent->r.linked && ent->neverFree)
    {
      continue;
    }

    if(!ent->client)
    {
      if((es->eFlags & EF_ANIM_ONCE) == EF_ANIM_ONCE)
      {//this must be capped render-side
        es->frame++;
      }
    }

    if((es->eType == ET_MISSILE) || (es->eType == ET_ALT_MISSILE))
    {
      G_Missile_Run(ent);
      continue;
    }

    if(es->eType == ET_ITEM || ent->physicsObject)
    {
      G_RunItem(ent);
      continue;
    }

    if(es->eType == ET_MOVER || es->eType == ET_MOVER_STR)
    { //RPG-X | GSIO01 | 13/05/2009
      G_Mover_Run(ent);
      continue;
    }

    if(i < MAX_CLIENTS)
    {
      G_RunClient(ent);
      continue;
    }

    G_RunThink(ent);
  }

  // perform final fixups on the players
  ent = &g_entities[0];
  for(i = 0; i < level.maxclients; i++, ent++)
  {
    if(ent->inuse)
    {
      ClientEndFrame(ent);
    }
  }

  // see if it is time to do a tournement restart
  CheckTournement();

  // update to client status?
  G_Client_CheckClientStatus();

  // cancel vote if timed out
  CheckVote();

  // for tracking changes
  CheckCvars();

  //RPG-X: J2J - This will check for drag movements that need to be calculated.
  DragCheck();

  for(i = 0; i < MAX_CLIENTS; i++)
  {
    ent = &g_entities[i];

    if(!ent || !ent->client)
      continue;

    client = ent->client;

    if((client->pers.cmd.buttons & BUTTON_USE) == 0)
    {
      client->pressedUse = qfalse;
    }

    if(g_classData[client->sess.sessionClass].isn00b != 0)
    {
      if((client->n00bTime != -1) && (client->n00bTime <= level.time) && client->origClass[0] != 0)
      {
        if(SetClass(ent, client->origClass, NULL, qtrue) == qfalse)
        {
          DEVELOPER(G_Printf(S_COLOR_YELLOW "G_RunFrame - Warning: SetClass failed!\n"););
        }
      }
    }
  }

  //RPG-X: Marcin: To clear pressedUse. - 30/12/2008

#ifdef G_LUA
  LuaHook_G_RunFrame(levelTime);
#endif

}
