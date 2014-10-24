// Copyright (C) 1999-2000 Id Software, Inc.
//

/*****************************************************************************
 * name:		be_ai_move.h
 *
 * desc:		movement AI
 *
 * $Archive: /StarTrek/Code-DM/game/be_ai_move.h $
 * $Author: Jmonroe $
 * $Revision: 1 $
 * $Modtime: 1/21/00 10:12p $
 * $Date: 1/25/00 6:27p $
 *
 *****************************************************************************/

#ifndef BE_AI_MOVE_H
#define BE_AI_MOVE_H

//movement types
enum be_ai_movementType_e {
	MOVE_WALK = 1,
	MOVE_CROUCH = 2,
	MOVE_JUMP = 4,
	MOVE_GRAPPLE = 8,
	MOVE_ROCKETJUMP = 16,
	MOVE_BFGJUMP = 32
};

//move flags
enum be_ai_moveFlag_e {
	MFL_BARRIERJUMP = 1,		//!<bot is performing a barrier jump
	MFL_ONGROUND = 2,			//!<bot is in the ground
	MFL_SWIMMING = 4,			//!<bot is swimming
	MFL_AGAINSTLADDER = 8,		//!<bot is against a ladder
	MFL_WATERJUMP = 16,			//!<bot is waterjumping
	MFL_TELEPORTED = 32,		//!<bot is being teleported
	MFL_GRAPPLEPULL = 64,		//!<bot is being pulled by the grapple
	MFL_ACTIVEGRAPPLE = 128,	//!<bot is using the grapple hook
	MFL_GRAPPLERESET = 256,		//!<bot has reset the grapple
	MFL_WALK = 512				//!<bot should walk slowly
};

//move result flags
enum be_ai_moveResultFlags_e {
	MOVERESULT_MOVEMENTVIEW = 1,		//!<bot uses view for movement
	MOVERESULT_SWIMVIEW = 2,			//!<bot uses view for swimming
	MOVERESULT_WAITING = 4,				//!<bot is waiting for something
	MOVERESULT_MOVEMENTVIEWSET = 8,		//!<bot has set the view in movement code
	MOVERESULT_MOVEMENTWEAPON = 16,		//!<bot uses weapon for movement
	MOVERESULT_ONTOPOFOBSTACLE = 32,	//!<bot is ontop of obstacle
	MOVERESULT_ONTOPOF_FUNCBOB = 64,	//!<bot is ontop of a func_bobbing
	MOVERESULT_ONTOPOF_ELEVATOR = 128	//!<bot is ontop of an elevator (func_plat)
};

#define MAX_AVOIDREACH					1

enum be_ai_resultType_e {
	RESULTTYPE_ELEVATORUP = 1,			//!<elevator is up
	RESULTTYPE_WAITFORFUNCBOBBING = 2,	//!<waiting for func bobbing to arrive
	RESULTTYPE_BADGRAPPLEPATH = 4		//!<grapple path is obstructured
};

//structure used to initialize the movement state
//the or_moveflags MFL_ONGROUND, MFL_TELEPORTED and MFL_WATERJUMP come from the playerstate
typedef struct bot_initmove_s {
	vec3_t origin;				//!<origin of the bot
	vec3_t velocity;			//!<velocity of the bot
	vec3_t viewoffset;			//!<view offset
	int32_t entitynum;			//!<entity number of the bot
	int32_t client;				//!<client number of the bot
	double thinktime;			//!<time the bot thinks
	int32_t presencetype;		//!<presencetype of the bot
	vec3_t viewangles;			//!<view angles of the bot
	int32_t or_moveflags;		//!<values ored to the movement flags
} bot_initmove_t;

//NOTE: the ideal_viewangles are only valid if MFL_MOVEMENTVIEW is set
typedef struct bot_moveresult_s {
	int32_t failure;			//!<true if movement failed all together
	int32_t type;				//!<failure or blocked type
	int32_t blocked;			//!<true if blocked by an entity
	int32_t blockentity;		//!<entity blocking the bot
	int32_t traveltype;			//!<last executed travel type
	int32_t flags;				//!<result flags
	int32_t weapon;				//!<weapon used for movement
	vec3_t movedir;				//!<movement direction
	vec3_t ideal_viewangles;	//!<ideal viewangles for the movement
} bot_moveresult_t;

//!resets the whole movestate
void BotResetMoveState(int movestate);
//!moves the bot to the given goal
void BotMoveToGoal(bot_moveresult_t *result, int movestate, bot_goal_t *goal, int travelflags);
//!moves the bot in the specified direction
int BotMoveInDirection(int movestate, vec3_t dir, float speed, int type);
//!reset avoid reachability
void BotResetAvoidReach(int movestate);
//!resets the last avoid reachability
void BotResetLastAvoidReach(int movestate);
//!returns a reachability area if the origin is in one
int BotReachabilityArea(vec3_t origin, int client);
//!view target based on movement
int BotMovementViewTarget(int movestate, bot_goal_t *goal, int travelflags, float lookahead, vec3_t target);
//!predict the position of a player
int BotPredictVisiblePosition(vec3_t origin, int areanum, bot_goal_t *goal, int travelflags, vec3_t target);
//!returns the handle of a newly allocated movestate
int BotAllocMoveState(void);
//!frees the movestate with the given handle
void BotFreeMoveState(int handle);
//!initialize movement state
void BotInitMoveState(int handle, bot_initmove_t *initmove);
//!must be called every map change
void BotSetBrushModelTypes(void);
//!setup movement AI
int BotSetupMoveAI(void);
//!shutdown movement AI
void BotShutdownMoveAI(void);

#endif /* BE_AI_MOVE_H */
