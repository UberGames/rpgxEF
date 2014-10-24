// Copyright (C) 1999-2000 Id Software, Inc.
//

/*****************************************************************************
 * name:		ai_main.h
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /StarTrek/Code-DM/game/ai_main.h $
 * $Author: Jmonroe $
 * $Revision: 1 $
 * $Modtime: 1/21/00 10:12p $
 * $Date: 1/25/00 6:26p $
 *
 *****************************************************************************/

#ifndef AI_MAIN_H_
#define AI_AMIN_H_

#include "ai_common.h"
#include "be_ai_goal.h"
#include "be_aas.h"
#include "botlib.h"

//#define DEBUG
#define CTF

//bot flags

enum ai_mainBFL_e {
	BFL_STRAFERIGHT = 1,	//!<strafe to the right
	BFL_ATTACKED = 2,		//!<bot has attacked last ai frame
	BFL_ATTACKJUMPED = 4,	//!<bot jumped during attack last frame
	BFL_AIMATENEMY = 8,		//!<bot aimed at the enemy this frame
	BFL_AVOIDRIGHT = 16,	//!<avoid obstacles by going to the right
	BFL_IDEALVIEWSET = 32,	//!<bot has ideal view angles set
	BFL_FIGHTSUICIDAL = 64	//!<bot is in a suicidal fight
};

//long term goal types
enum ai_mainLTG_e {
	LTG_TEAMHELP = 1,	//!<help a team mate
	LTG_TEAMACCOMPANY,	//!<accompany a team mate
	LTG_DEFENDKEYAREA,	//!<defend a key area
	LTG_GETFLAG,		//!<get the enemy flag
	LTG_RUSHBASE,		//!<rush to the base
	LTG_RETURNFLAG,		//!<return the flag
	LTG_CAMP,			//!<camp somewhere
	LTG_CAMPORDER,		//!<ordered to camp somewhere
	LTG_PATROL,			//!<patrol
	LTG_GETITEM,		//!<get an item
	LTG_KILL			//!<kill someone
};

//some goal dedication times
static const uint32_t TEAM_HELP_TIME = 10;				//!<1 minute teamplay help time
static const uint32_t TEAM_ACCOMPANY_TIME = 10;			//!<10 minutes teamplay accompany time
static const uint32_t TEAM_DEFENDKEYAREA_TIME = 600;	//!<5 minutes ctf defend base time
static const uint32_t TEAM_CAMP_TIME = 10;				//!<10 minutes camping time
static const uint32_t TEAM_PATROL_TIME = 600;			//!<10 minutes patrolling time
static const uint32_t TEAM_LEAD_TIME = 600;				//!<10 minutes taking the lead
static const uint32_t TEAM_GETITEM_TIME = 10;			//!<1 minute
static const uint32_t TEAM_KILL_SOMEONE = 10;			//!<3 minute to kill someone
static const uint32_t CTF_GETFLAG_TIME = 600;			//!<5 minutes ctf get flag time
static const uint32_t CTF_RUSHBASE_TIME = 600;			//!<2 minutes ctf rush base time
static const uint32_t CTF_RETURNFLAG_TIME = 600;		//!<3 minutes to return the flag
static const uint32_t CTF_ROAM_TIME = 600;				//!<1 minute ctf roam time

//patrol flags
static const uint32_t PATROL_LOOP = 1;
static const uint32_t PATROL_REVERSE = 2;
static const uint32_t PATROL_BACK = 4;

//CTF task preference
static const uint32_t CTFTP_DEFENDER = 1;
static const uint32_t CTFTP_ATTACKER = 2;

//CTF strategy
static const uint32_t CTFS_PASSIVE = 1;

//copied from the aas file header
static const uint32_t PRESENCE_NONE = 1;
static const uint32_t PRESENCE_NORMAL = 2;
static const uint32_t PRESENCE_CROUCH = 4;

//!check points
typedef struct bot_waypoint_s {
	int32_t		inuse;
	char		name[32];
	bot_goal_t	goal;
	struct		bot_waypoint_s *next, *prev;
} bot_waypoint_t;

//!bot state
typedef struct bot_state_s {
	int32_t inuse;										//!<true if this state is used by a bot client
	int32_t botthink_residual;							//!<residual for the bot thinks
	int32_t client;										//!<client number of the bot
	int32_t entitynum;									//!<entity number of the bot
	playerState_t cur_ps;							//!<current player state
	int32_t last_eFlags;								//!<last ps flags
	usercmd_t lastucmd;								//!<usercmd from last frame
	int32_t entityeventTime[1024];						//!<last entity event time
	//
	bot_settings_t settings;						//!<several bot settings
	int(*ainode)(struct bot_state_s *bs);			//!<current AI node
	double thinktime;								//!<time the bot thinks this frame
	vec3_t origin;									//!<origin of the bot
	vec3_t velocity;								//!<velocity of the bot
	int32_t presencetype;								//!<presence type of the bot
	vec3_t eye;										//!<eye coordinates of the bot
	int32_t areanum;									//!<the number of the area the bot is in
	int32_t inventory[MAX_ITEMS];						//!<string with items amounts the bot has
	int32_t tfl;										//!<the travel flags the bot uses
	int32_t flags;										//!<several flags
	int32_t respawn_wait;								//!<wait until respawned
	int32_t lasthealth;									//!<health value previous frame
	int32_t lastkilledplayer;							//!<last killed player
	int32_t lastkilledby;								//!<player that last killed this bot
	int32_t botdeathtype;								//!<the death type of the bot
	int32_t enemydeathtype;								//!<the death type of the enemy
	int32_t botsuicide;									//!<true when the bot suicides
	int32_t enemysuicide;								//!<true when the enemy of the bot suicides
	int32_t setupcount;									//!<true when the bot has just been setup
	int32_t entergamechat;								//!<true when the bot used an enter game chat
	int32_t num_deaths;									//!<number of time this bot died
	int32_t num_kills;									//!<number of kills of this bot
	int32_t revenge_enemy;								//!<the revenge enemy
	int32_t revenge_kills;								//!<number of kills the enemy made
	int32_t lastframe_health;							//!<health value the last frame
	int32_t lasthitcount;								//!<number of hits last frame
	int32_t chatto;										//!<chat to all or team
	double walker;									//!<walker charactertic
	double ltime;									//!<local bot time
	double entergame_time;							//!<time the bot entered the game
	double ltg_time;									//!<long term goal time
	double nbg_time;									//!<nearby goal time
	double respawn_time;								//!<time the bot takes to respawn
	double respawnchat_time;							//!<time the bot started a chat during respawn
	double chase_time;								//!<time the bot will chase the enemy
	double enemyvisible_time;						//!<time the enemy was last visible
	double check_time;								//!<time to check for nearby items
	double stand_time;								//!<time the bot is standing still
	double lastchat_time;							//!<time the bot last selected a chat
	double standfindenemy_time;						//!<time to find enemy while standing
	double attackstrafe_time;						//!<time the bot is strafing in one dir
	double attackcrouch_time;						//!<time the bot will stop crouching
	double attackchase_time;							//!<time the bot chases during actual attack
	double attackjump_time;							//!<time the bot jumped during attack
	double enemysight_time;							//!<time before reacting to enemy
	double enemydeath_time;							//!<time the enemy died
	double enemyposition_time;						//!<time the position and velocity of the enemy were stored
	double activate_time;							//!<time to activate something
	double activatemessage_time;						//!<time to show activate message
	double defendaway_time;							//!<time away while defending
	double defendaway_range;							//!<max travel time away from defend area
	double rushbaseaway_time;						//!<time away from rushing to the base
	double ctfroam_time;								//!<time the bot is roaming in ctf
	double killedenemy_time;							//!<time the bot killed the enemy
	double arrive_time;								//!<time arrived (at companion)
	double lastair_time;								//!<last time the bot had air
	double teleport_time;							//!<last time the bot teleported
	double camp_time;								//!<last time camped
	double camp_range;								//!<camp range
	double weaponchange_time;						//!<time the bot started changing weapons
	double firethrottlewait_time;					//!<amount of time to wait
	double firethrottleshoot_time;					//!<amount of time to shoot
	double notblocked_time;							//!<last time the bot was not blocked
	vec3_t aimtarget;
	vec3_t enemyvelocity;							//!<enemy velocity 0.5 secs ago during battle
	vec3_t enemyorigin;								//!<enemy origin 0.5 secs ago during battle
	//
	int32_t character;									//!<the bot character
	int32_t ms;											//!<move state of the bot
	int32_t gs;											//!<goal state of the bot
	int32_t cs;											//!<chat state of the bot
	int32_t ws;											//!<weapon state of the bot
	//
	int32_t enemy;										//!<enemy entity number
	int32_t lastenemyareanum;							//!<last reachability area the enemy was in
	vec3_t lastenemyorigin;							//!<last origin of the enemy in the reachability area
	int32_t weaponnum;									//!<current weapon number
	vec3_t viewangles;								//!<current view angles
	vec3_t ideal_viewangles;						//!<ideal view angles
	vec3_t viewanglespeed;
	//
	int32_t ltgtype;									//!<long term goal type
	//
	int32_t teammate;									//!<team mate
	bot_goal_t teamgoal;							//!<the team goal
	double teammessage_time;							//!<time to message team mates what the bot is doing
	double teamgoal_time;							//!<time to stop helping team mate
	double teammatevisible_time;						//!<last time the team mate was NOT visible
	//
	int32_t lead_teammate;								//!<team mate the bot is leading
	bot_goal_t lead_teamgoal;						//!<team goal while leading
	double lead_time;								//!<time leading someone
	double leadvisible_time;							//!<last time the team mate was visible
	double leadmessage_time;							//!<last time a messaged was sent to the team mate
	double leadbackup_time;							//!<time backing up towards team mate
	//
	char teamleader[32];							//!<netname of the team leader
	double askteamleader_time;						//!<time asked for team leader
	double becometeamleader_time;					//!<time the bot will become the team leader
	double teamgiveorders_time;						//!<time to give team orders
	double lastflagcapture_time;						//!<last time a flag was captured
	int32_t numteammates;								//!<number of team mates
	int32_t redflagstatus;								//!<0 = at base, 1 = not at base
	int32_t blueflagstatus;								//!<0 = at base, 1 = not at base
	int32_t flagstatuschanged;							//!<flag status changed
	int32_t forceorders;								//!<true if forced to give orders
	int32_t flagcarrier;								//!<team mate carrying the enemy flag
	int32_t ctfstrategy;								//!<ctf strategy
	char subteam[32];								//!<sub team name
	double formation_dist;							//!<formation team mate intervening space
	char formation_teammate[16];					//!<netname of the team mate the bot uses for relative positioning
	double formation_angle;							//!<angle relative to the formation team mate
	vec3_t formation_dir;							//!<the direction the formation is moving in
	vec3_t formation_origin;						//!<origin the bot uses for relative positioning
	bot_goal_t formation_goal;						//!<formation goal
	bot_goal_t activategoal;						//!<goal to activate (buttons etc.)
	bot_waypoint_t *checkpoints;					//!<check points
	bot_waypoint_t *patrolpoints;					//!<patrol points
	bot_waypoint_t *curpatrolpoint;					//!<current patrol point the bot is going for
	int32_t patrolflags;								//!<patrol flags
} bot_state_t;

//!resets the whole bot state
void AI_main_BotResetState(bot_state_t *bs);

//!returns the number of bots in the game
int32_t AI_main_NumBots(void);

//!returns info about the entity
void AI_main_BotEntityInfo(int32_t entnum, aas_entityinfo_t *info);

// from the game source
void QDECL AI_main_BotAIPrint(int32_t type, char *fmt, ...) __attribute__((format(printf, 2, 3)));

void QDECL QDECL AI_main_BotAIInitialChat(bot_state_t *bs, char *type, ...);

void AI_main_BotAITrace(bsp_trace_t *bsptrace, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int32_t passent, int32_t contentmask);

int32_t	AI_main_BotAIGetClientState(int32_t clientNum, playerState_t *state);

int32_t AI_main_BotAIGetEntityState(int32_t entityNum, entityState_t *state);

int32_t	AI_main_BotAIGetSnapshotEntity(int32_t clientNum, int32_t sequence, entityState_t *state);

int32_t	AI_main_BotTeamLeader(bot_state_t *bs);

/**
* Setup bot AI.
*
* \param restart Determines if this is a map restart.
*/
int32_t AI_main_BotAISetup(int32_t restart);

/**
* Shutdown bot AI.
*
* \param restart Determines if this is a map restart.
*/
int32_t AI_main_BotAIShutdown(int32_t restart);

/**
* Load map in bot lib.
*
* \param restart Determines if this is a map restart.
*/
int32_t AI_main_BotAILoadMap(int32_t restart);

/**
* Setup bot AI for client.
*
* \param client client number
* \param settings bot settings
*/
int32_t AI_main_BotAISetupClient(int32_t client, bot_settings_t* settings);

/**
* Shutdown bot client.
*
* \param client client number
*/
int32_t AI_main_BotAIShutdownClient(int32_t client);

/**
* Star frame.
*
* \param time current time
*/
int32_t AI_main_BotAIStartFrame(int32_t time);


#endif /* AI_AMIN_H_ */


