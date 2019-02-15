// Copyright (C) 1999-2000 Id Software, Inc.
//

/*****************************************************************************
 * name:		ai_main.c
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /StarTrek/Code-DM/game/ai_main.c $
 * $Author: Jmonroe $
 * $Revision: 4 $
 * $Modtime: 11/30/00 5:17p $
 * $Date: 11/30/00 5:19p $
 *
 *****************************************************************************/


#include "g_local.h"
#include "../base_game/q_shared.h"
#include "botlib.h"		//bot lib interface
#include "be_aas.h"
#include "be_ea.h"
#include "be_ai_char.h"
#include "be_ai_chat.h"
#include "be_ai_gen.h"
#include "be_ai_goal.h"
#include "be_ai_move.h"
#include "be_ai_weap.h"
//
#include "ai_main.h"
#include "ai_dmq3.h"
#include "ai_chat.h"
#include "ai_cmd.h"
#include "ai_dmnet.h"
//
#include "chars.h"
#include "inv.h"
#include "syn.h"
#include "../base_game/q_math.h"

#include "g_syscalls.h"

#define AI_MAX_PATH		144


//bot states
bot_state_t	*botstates[MAX_CLIENTS];
//number of bots
int32_t numbots;
//time to do a regular update
double regularupdate_time;
//
int32_t bot_interbreed;
int32_t bot_interbreedmatchcount;
//
vmCvar_t bot_thinktime;
vmCvar_t bot_memorydump;
vmCvar_t bot_pause;
vmCvar_t bot_report;
vmCvar_t bot_testsolid;
vmCvar_t bot_interbreedchar;
vmCvar_t bot_interbreedbots;
vmCvar_t bot_interbreedcycle;
vmCvar_t bot_interbreedwrite;

qboolean bot_setupComplete = qfalse;

void ExitLevel(void);


/*
==================
BotAI_Print
==================
*/
void QDECL AI_main_BotAIPrint(int32_t type, char* fmt, ...) {
	char str[2048];
	va_list ap;

	va_start(ap, fmt);
	vsprintf(str, fmt, ap);
	va_end(ap);

	switch (type) {
		case PRT_MESSAGE:
		{
							G_Printf("%s", str);
							break;
		}
		case PRT_WARNING:
		{
							G_Printf(S_COLOR_YELLOW "Warning: %s", str);
							break;
		}
		case PRT_ERROR:
		{
						  G_Printf(S_COLOR_RED "Error: %s", str);
						  break;
		}
		case PRT_FATAL:
		{
						  G_Printf(S_COLOR_RED "Fatal: %s", str);
						  break;
		}
		case PRT_EXIT:
		{
						 G_Error(S_COLOR_RED "Exit: %s", str);
						 break;
		}
		default:
		{
				   G_Printf("unknown print type\n");
				   break;
		}
	}
}


/*
==================
BotAI_Trace
==================
*/
void AI_main_BotAITrace(bsp_trace_t* bsptrace, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int32_t passent, int32_t contentmask) {
	trace_t trace;

	AI_Assert(bsptrace, (void)0);

	trap_Trace(&trace, start, mins, maxs, end, passent, contentmask);
	//copy the trace information
	bsptrace->allsolid = trace.allsolid;
	bsptrace->startsolid = trace.startsolid;
	bsptrace->fraction = trace.fraction;
	VectorCopy(trace.endpos, bsptrace->endpos);
	bsptrace->plane.dist = trace.plane.dist;
	VectorCopy(trace.plane.normal, bsptrace->plane.normal);
	bsptrace->plane.signbits = trace.plane.signbits;
	bsptrace->plane.type = trace.plane.type;
	bsptrace->surface.value = trace.surfaceFlags;
	bsptrace->ent = trace.entityNum;
	bsptrace->exp_dist = 0;
	bsptrace->sidenum = 0;
	bsptrace->contents = 0;
}

/*
==================
BotAI_GetClientState
==================
*/
int32_t AI_main_BotAIGetClientState(int32_t clientNum, playerState_t* state) {
	gentity_t* ent = NULL;

	AI_Assert(state, qfalse);

	ent = &g_entities[clientNum];
	AI_Assert(ent, qfalse);

	if (!ent->inuse) {
		return qfalse;
	}

	if (ent->client == NULL) {
		return qfalse;
	}

	memcpy(state, &ent->client->ps, sizeof(playerState_t));
	return qtrue;
}

/*
==================
BotAI_GetEntityState
==================
*/
int32_t AI_main_BotAIGetEntityState(int32_t entityNum, entityState_t* state) {
	gentity_t* ent = NULL;

	AI_Assert(state, qfalse);

	ent = &g_entities[entityNum];
	AI_Assert(ent, qfalse);

	memset(state, 0, sizeof(entityState_t));

	if (!ent->inuse) {
		return qfalse;
	}

	if (!ent->r.linked) {
		return qfalse;
	}

	if ((ent->r.svFlags & SVF_NOCLIENT) != 0) {
		return qfalse;
	}

	memcpy(state, &ent->s, sizeof(entityState_t));
	return qtrue;
}

/*
==================
BotAI_GetSnapshotEntity
==================
*/
int32_t AI_main_BotAIGetSnapshotEntity(int32_t clientNum, int32_t sequence, entityState_t* state) {
	int32_t	entNum = -1;

	AI_Assert(state, -1);

	entNum = trap_BotGetSnapshotEntity(clientNum, sequence);
	if (entNum == -1) {
		memset(state, 0, sizeof(entityState_t));
		return -1;
	}

	AI_main_BotAIGetEntityState(entNum, state);

	return sequence + 1;
}

/*
==================
BotAI_BotInitialChat
==================
*/
void QDECL AI_main_BotAIInitialChat(bot_state_t* bs, const char* type, ...) {
	int32_t	i = 0;
	int32_t mcontext = 0;
	va_list	ap;
	char* p = NULL;
	char* vars[MAX_MATCHVARIABLES];

	AI_Assert(bs, (void)0);
	AI_Assert(type, (void)0);

	memset(vars, 0, sizeof(vars));
	va_start(ap, type);
	p = va_arg(ap, char*);
	for (i = 0; i < MAX_MATCHVARIABLES; i++) {
		if (p == NULL) {
			break;
		}
		vars[i] = p;
		p = va_arg(ap, char *);
	}
	va_end(ap);

	mcontext = CONTEXT_NORMAL | CONTEXT_NEARBYITEM | CONTEXT_NAMES;
	if (BotCTFTeam(bs) == CTF_TEAM_RED) {
		mcontext |= CONTEXT_CTFREDTEAM;
	} else {
		mcontext |= CONTEXT_CTFBLUETEAM;
	}

	trap_BotInitialChat(bs->cs, type, mcontext, vars[0], vars[1], vars[2], vars[3], vars[4], vars[5], vars[6], vars[7]);
}


/*
==================
BotTestSolid
==================
*/
void AI_main_BotTestSolid(vec3_t origin) {

	if (!bot_setupComplete) {
		return;
	}

	trap_Cvar_Update(&bot_testsolid);
	if (bot_testsolid.integer != 0) {
		int areanum;

		if (trap_AAS_Initialized() == 0) {
			return;
		}

		areanum = BotPointAreaNum(origin);
		if (areanum != 0) {
			AI_main_BotAIPrint(PRT_MESSAGE, "\remtpy area");
		} else {
			AI_main_BotAIPrint(PRT_MESSAGE, "\r^1SOLID area");
		}
	}
}

/*
==================
BotReportStatus
==================
*/
static void AI_main_BotReportStatus(bot_state_t* bs) {
	char goalname[MAX_MESSAGE_SIZE];
	char netname[MAX_MESSAGE_SIZE];
	char* leader = NULL;
	char* flagstatus = NULL;

	if (bs == NULL) {
		return;
	}

	ClientName(bs->client, netname, sizeof(netname));
	if (Q_stricmp(netname, bs->teamleader) == 0) {
		leader = "L";
	} else {
		leader = " ";
	}

	if (BotCTFCarryingFlag(bs) != 0) {
		if (BotCTFTeam(bs) == TEAM_RED) {
			flagstatus = S_COLOR_RED"F";
		} else {
			flagstatus = S_COLOR_BLUE"F";
		}
	} else {
		flagstatus = " ";
	}

	switch (bs->ltgtype) {
		case LTG_TEAMHELP:
		{
							 EasyClientName(bs->teammate, goalname, sizeof(goalname));
							 AI_main_BotAIPrint(PRT_MESSAGE, "%-20s%s%s: helping %s\n", netname, leader, flagstatus, goalname);
							 break;
		}
		case LTG_TEAMACCOMPANY:
		{
								  EasyClientName(bs->teammate, goalname, sizeof(goalname));
								  AI_main_BotAIPrint(PRT_MESSAGE, "%-20s%s%s: accompanying %s\n", netname, leader, flagstatus, goalname);
								  break;
		}
		case LTG_DEFENDKEYAREA:
		{
								  trap_BotGoalName(bs->teamgoal.number, goalname, sizeof(goalname));
								  AI_main_BotAIPrint(PRT_MESSAGE, "%-20s%s%s: defending %s\n", netname, leader, flagstatus, goalname);
								  break;
		}
		case LTG_GETITEM:
		{
							trap_BotGoalName(bs->teamgoal.number, goalname, sizeof(goalname));
							AI_main_BotAIPrint(PRT_MESSAGE, "%-20s%s%s: getting item %s\n", netname, leader, flagstatus, goalname);
							break;
		}
		case LTG_KILL:
		{
						 ClientName(bs->teamgoal.entitynum, goalname, sizeof(goalname));
						 AI_main_BotAIPrint(PRT_MESSAGE, "%-20s%s%s: killing %s\n", netname, leader, flagstatus, goalname);
						 break;
		}
		case LTG_CAMP:
		case LTG_CAMPORDER:
		{
							  AI_main_BotAIPrint(PRT_MESSAGE, "%-20s%s%s: camping\n", netname, leader, flagstatus);
							  break;
		}
		case LTG_PATROL:
		{
						   AI_main_BotAIPrint(PRT_MESSAGE, "%-20s%s%s: patrolling\n", netname, leader, flagstatus);
						   break;
		}
		case LTG_GETFLAG:
		{
							AI_main_BotAIPrint(PRT_MESSAGE, "%-20s%s%s: capturing flag\n", netname, leader, flagstatus);
							break;
		}
		case LTG_RUSHBASE:
		{
							 AI_main_BotAIPrint(PRT_MESSAGE, "%-20s%s%s: rushing base\n", netname, leader, flagstatus);
							 break;
		}
		case LTG_RETURNFLAG:
		{
							   AI_main_BotAIPrint(PRT_MESSAGE, "%-20s%s%s: returning flag\n", netname, leader, flagstatus);
							   break;
		}
		default:
		{
				   AI_main_BotAIPrint(PRT_MESSAGE, "%-20s%s%s: roaming\n", netname, leader, flagstatus);
				   break;
		}
	}
}

/*
==================
BotTeamplayReport
==================
*/
static void AI_main_BotTeamplayReport(void) {
	int32_t i = 0;
	char buf[MAX_INFO_STRING];

	AI_main_BotAIPrint(PRT_MESSAGE, S_COLOR_RED"RED\n");
	for (i = 0; i < maxclients && i < MAX_CLIENTS; i++) {
		if (botstates[i] == NULL || !botstates[i]->inuse) {
			continue;
		}

		trap_GetConfigstring(CS_PLAYERS + i, buf, sizeof(buf));

		//if no config string or no name
		if (strlen(buf) == 0 || strlen(Info_ValueForKey(buf, "n")) == 0) {
			continue;
		}

		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_RED) {
			AI_main_BotReportStatus(botstates[i]);
		}
	}
	AI_main_BotAIPrint(PRT_MESSAGE, S_COLOR_BLUE"BLUE\n");
	for (i = 0; i < maxclients && i < MAX_CLIENTS; i++) {
		if (botstates[i] == NULL || !botstates[i]->inuse) {
			continue;
		}

		trap_GetConfigstring(CS_PLAYERS + i, buf, sizeof(buf));

		//if no config string or no name
		if (strlen(buf) == 0 || strlen(Info_ValueForKey(buf, "n")) == 0) {
			continue;
		}

		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_BLUE) {
			AI_main_BotReportStatus(botstates[i]);
		}
	}
}

/*
==============
BotInterbreedBots
==============
*/
static void AI_main_BotInterbreedBots(void) {
	double ranks[MAX_CLIENTS];
	int32_t parent1 = 0;
	int32_t parent2 = 0;
	int32_t child = 0;;
	int32_t i = 0;

	// get rankings for all the bots
	for (; i < MAX_CLIENTS; i++) {
		if (botstates[i] != NULL && botstates[i]->inuse) {
			ranks[i] = botstates[i]->num_kills * 2 - botstates[i]->num_deaths;
		} else {
			ranks[i] = -1;
		}
	}

	if (trap_GeneticParentsAndChildSelection(MAX_CLIENTS, (float*)ranks, &parent1, &parent2, &child) != 0) {
		trap_BotInterbreedGoalFuzzyLogic(botstates[parent1]->gs, botstates[parent2]->gs, botstates[child]->gs);
		trap_BotMutateGoalFuzzyLogic(botstates[child]->gs, 1);
	}

	// reset the kills and deaths
	for (i = 0; i < MAX_CLIENTS; i++) {
		if (botstates[i] != NULL && botstates[i]->inuse) {
			botstates[i]->num_kills = 0;
			botstates[i]->num_deaths = 0;
		}
	}
}

/*
==============
BotWriteInterbreeded
==============
*/
static void AI_main_BotWriteInterbreeded(char* filename) {
	double rank = 0.0;
	double bestrank = 0.0;
	int32_t i = 0;
	int32_t bestbot = -1;

	AI_Assert(filename, (void)0);

	// get the best bot
	for (; i < MAX_CLIENTS; i++) {
		if (botstates[i] != NULL && botstates[i]->inuse) {
			rank = botstates[i]->num_kills * 2 - botstates[i]->num_deaths;
		} else {
			rank = -1;
		}
		if (rank > bestrank) {
			bestrank = rank;
			bestbot = i;
		}
	}

	if (bestbot >= 0) {
		//write out the new goal fuzzy logic
		trap_BotSaveGoalFuzzyLogic(botstates[bestbot]->gs, filename);
	}
}

/*
==============
BotInterbreedEndMatch

add link back into ExitLevel?
==============
*/
void AI_main_BotInterbreedEndMatch(void) {

	if (bot_interbreed == 0) {
		return;
	}

	bot_interbreedmatchcount++;
	if (bot_interbreedmatchcount >= bot_interbreedcycle.integer) {
		bot_interbreedmatchcount = 0;

		trap_Cvar_Update(&bot_interbreedwrite);
		if (strlen(bot_interbreedwrite.string) != 0) {
			AI_main_BotWriteInterbreeded(bot_interbreedwrite.string);
			trap_Cvar_Set("bot_interbreedwrite", "");
		}
		AI_main_BotInterbreedBots();
	}
}

/*
==============
BotInterbreeding
==============
*/
static void AI_main_BotInterbreeding(void) {
	int32_t i = 0;

	trap_Cvar_Update(&bot_interbreedchar);
	if (strlen(bot_interbreedchar.string) == 0) {
		return;
	}

	//make sure we are in tournament mode
	if (gametype != GT_TOURNAMENT) {
		trap_Cvar_Set("g_gametype", va("%d", GT_TOURNAMENT));
		ExitLevel();
		return;
	}

	//shutdown all the bots
	for (; i < MAX_CLIENTS; i++) {
		if (botstates[i] != NULL  && botstates[i]->inuse) {
			AI_main_BotAIShutdownClient(botstates[i]->client);
		}
	}

	//make sure all item weight configs are reloaded and Not shared
	trap_BotLibVarSet("bot_reloadcharacters", "1");

	//add a number of bots using the desired bot character
	for (i = 0; i < bot_interbreedbots.integer; i++) {
		trap_SendConsoleCommand(EXEC_INSERT, va("addbot %s 4 free %i %s%d\n", bot_interbreedchar.string, i * 50, bot_interbreedchar.string, i));
	}

	trap_Cvar_Set("bot_interbreedchar", "");
	bot_interbreed = qtrue;
}

/*
==============
BotEntityInfo
==============
*/
void AI_main_BotEntityInfo(int entnum, aas_entityinfo_t* info) {
	trap_AAS_EntityInfo(entnum, info);
}

/*
==============
NumBots
==============
*/
int32_t AI_main_NumBots(void) {
	return numbots;
}

/*
==============
BotTeamLeader
==============
*/
int32_t AI_main_BotTeamLeader(bot_state_t* bs) {
	int32_t leader = 0;

	AI_Assert(bs, qfalse);

	leader = ClientFromName(bs->teamleader);
	if (leader < 0) {
		return qfalse;
	}

	if (botstates[leader] == NULL || !botstates[leader]->inuse) {
		return qfalse;
	}

	return qtrue;
}

/*
==============
AngleDifference
==============
*/
static double AngleDifference(double ang1, double ang2) {
	double diff = ang1 - ang2;

	if (ang1 > ang2) {
		if (diff > 180.0) diff -= 360.0;
	} else {
		if (diff < -180.0) diff += 360.0;
	}
	return diff;
}

/*
==============
BotChangeViewAngle
==============
*/
static double AI_main_BotChangeViewAngle(double angle, double ideal_angle, double speed) {
	double move = 0.0;

	angle = AngleMod(angle);
	ideal_angle = AngleMod(ideal_angle);
	if (angle == ideal_angle) {
		return angle;
	}

	move = ideal_angle - angle;
	if (ideal_angle > angle) {
		if (move > 180.0) {
			move -= 360.0;
		}
	} else {
		if (move < -180.0) {
			move += 360.0;
		}
	}

	if (move > 0) {
		if (move > speed) {
			move = speed;
		}
	} else {
		if (move < -speed) {
			move = -speed;
		}
	}

	return AngleMod(angle + move);
}

/*
==============
BotChangeViewAngles
==============
*/
static void AI_main_BotChangeViewAngles(bot_state_t* bs, double thinktime) {
	double diff = 0.0;
	double factor = 1.0;
	double maxchange = 1800.0;
	double anglespeed = 0.0;
	int32_t i = 0;

	AI_Assert(bs, (void)0);

	if (bs->ideal_viewangles[PITCH] > 180) {
		bs->ideal_viewangles[PITCH] -= 360;
	}

	maxchange *= thinktime;
	for (; i < 2; i++) {
		//smooth slowdown view model
		diff = Q_fabs(AngleDifference(bs->viewangles[i], bs->ideal_viewangles[i]));
		anglespeed = diff * factor;
		if (anglespeed > maxchange) {
			anglespeed = maxchange;
		}
		bs->viewangles[i] = AI_main_BotChangeViewAngle(bs->viewangles[i], bs->ideal_viewangles[i], anglespeed);
	}

	if (bs->viewangles[PITCH] > 180) {
		bs->viewangles[PITCH] -= 360;
	}

	//elementary action: view
	trap_EA_View(bs->client, bs->viewangles);
}

/*
==============
BotInputToUserCommand
==============
*/
static void AI_main_BotInputToUserCommand(bot_input_t* bi, usercmd_t* ucmd, int32_t delta_angles[3], int32_t time) {
	vec3_t angles;
	vec3_t forward;
	vec3_t right;
	int16_t temp = 0;
	int32_t j = 0;

	AI_Assert(ucmd, (void)0)
	AI_Assert(bi, (void)0)

	//clear the whole structure
	memset(ucmd, 0, sizeof(usercmd_t));

	//Com_Printf("dir = %f %f %f speed = %f\n", bi->dir[0], bi->dir[1], bi->dir[2], bi->speed);
	//the duration for the user command in milli seconds
	ucmd->serverTime = time;

	if ((bi->actionflags & ACTION_DELAYEDJUMP) != 0) {
		bi->actionflags |= ACTION_JUMP;
		bi->actionflags &= ~ACTION_DELAYEDJUMP;
	}

	//set the buttons
	if ((bi->actionflags & ACTION_RESPAWN) != 0) {
		ucmd->buttons = BUTTON_ATTACK;
	}

	if ((bi->actionflags & ACTION_ATTACK) != 0) {
		ucmd->buttons |= BUTTON_ATTACK;
	}

	if ((bi->actionflags & ACTION_TALK) != 0) {
		ucmd->buttons |= BUTTON_TALK;
	}

	if ((bi->actionflags & ACTION_GESTURE) != 0) {
		ucmd->buttons |= BUTTON_GESTURE;
	}

	if ((bi->actionflags & ACTION_USE) != 0) {
		ucmd->buttons |= BUTTON_USE_HOLDABLE;
	}

	if ((bi->actionflags & ACTION_WALK) != 0) {
		ucmd->buttons |= BUTTON_WALKING;
	}

	if ((bi->actionflags & ACTION_ALT_ATTACK) != 0) {
		ucmd->buttons |= BUTTON_ALT_ATTACK;
	}

	ucmd->weapon = bi->weapon;
	//set the view angles
	//NOTE: the ucmd->angles are the angles WITHOUT the delta angles
	ucmd->angles[PITCH] = ANGLE2SHORT(bi->viewangles[PITCH]);
	ucmd->angles[YAW] = ANGLE2SHORT(bi->viewangles[YAW]);
	ucmd->angles[ROLL] = ANGLE2SHORT(bi->viewangles[ROLL]);

	//subtract the delta angles
	for (; j < 3; j++) {
		temp = ucmd->angles[j] - delta_angles[j];
		/*NOTE: disabled because temp should be mod first
		if ( j == PITCH ) {
		// don't let the player look up or down more than 90 degrees
		if ( temp > 16000 ) temp = 16000;
		else if ( temp < -16000 ) temp = -16000;
		}
		*/
		ucmd->angles[j] = temp;
	}

	//NOTE: movement is relative to the REAL view angles
	//get the horizontal forward and right vector
	//get the pitch in the range [-180, 180]
	if (bi->dir[2]) {
		angles[PITCH] = bi->viewangles[PITCH];
	} else {
		angles[PITCH] = 0;
	}
	angles[YAW] = bi->viewangles[YAW];
	angles[ROLL] = 0;
	AngleVectors(angles, forward, right, NULL);

	//bot input speed is in the range [0, 400]
	bi->speed = bi->speed * 127 / 400;

	//set the view independent movement
	ucmd->forwardmove = DotProduct(forward, bi->dir) * bi->speed;
	ucmd->rightmove = DotProduct(right, bi->dir) * bi->speed;
	ucmd->upmove = Q_fabs(forward[2]) * bi->dir[2] * bi->speed;

	//normal keyboard movement
	if ((bi->actionflags & ACTION_MOVEFORWARD) != 0) {
		ucmd->forwardmove += 127;
	}

	if ((bi->actionflags & ACTION_MOVEBACK) != 0) {
		ucmd->forwardmove -= 127;
	}

	if ((bi->actionflags & ACTION_MOVELEFT) != 0) {
		ucmd->rightmove -= 127;
	}

	if ((bi->actionflags & ACTION_MOVERIGHT) != 0) {
		ucmd->rightmove += 127;
	}

	//jump/moveup
	if ((bi->actionflags & ACTION_JUMP) != 0) {
		ucmd->upmove += 127;
	}

	//crouch/movedown
	if ((bi->actionflags & ACTION_CROUCH) != 0) {
		ucmd->upmove -= 127;
	}
	
	//Com_Printf("forward = %d right = %d up = %d\n", ucmd.forwardmove, ucmd.rightmove, ucmd.upmove);
	//Com_Printf("ucmd->serverTime = %d\n", ucmd->serverTime);
}

/*
==============
BotUpdateInput
==============
*/
	bot_input_t bi;
static void AI_main_BotUpdateInput(bot_state_t* bs, int32_t time, int32_t elapsed_time) {
	int32_t j = 0;

	AI_Assert(bs, (void)0)

	//add the delta angles to the bot's current view angles
	for (; j < 3; j++) {
		bs->viewangles[j] = AngleMod(bs->viewangles[j] + SHORT2ANGLE(bs->cur_ps.delta_angles[j]));
	}

	//change the bot view angles
	AI_main_BotChangeViewAngles(bs, (float)elapsed_time / 1000);

	//retrieve the bot input
	trap_EA_GetInput(bs->client, (float)time / 1000, &bi);

	//respawn hack
	if ((bi.actionflags & ACTION_RESPAWN) != 0) {
		if ((bs->lastucmd.buttons & BUTTON_ATTACK) != 0) {
			bi.actionflags &= ~(ACTION_RESPAWN | ACTION_ATTACK);
		}
	}

	//convert the bot input to a usercmd
	AI_main_BotInputToUserCommand(&bi, &bs->lastucmd, bs->cur_ps.delta_angles, time);
	//subtract the delta angles

	for (j = 0; j < 3; j++) {
		bs->viewangles[j] = AngleMod(bs->viewangles[j] - SHORT2ANGLE(bs->cur_ps.delta_angles[j]));
	}
}

/*
==============
BotAIRegularUpdate
==============
*/
static void AI_main_BotAIRegularUpdate(void) {
	if (regularupdate_time < trap_AAS_Time()) {
		trap_BotUpdateEntityItems();
		regularupdate_time = trap_AAS_Time() + 0.3;
	}
}

/*
==============
BotAI
==============
*/
static int32_t AI_main_BotAI(int32_t client, double thinktime) {
	bot_state_t* bs = NULL;
	char buf[1024];
	char* args = NULL;
	int32_t j = 0;

	trap_EA_ResetInput(client);
	
	bs = botstates[client];
	if (bs == NULL || !bs->inuse) {
		AI_main_BotAIPrint(PRT_FATAL, "BotAI: client %d is not setup\n", client);
		return qfalse;
	}

	//retrieve the current client state
	AI_main_BotAIGetClientState(client, &bs->cur_ps);

	//retrieve any waiting console messages
	while (trap_BotGetConsoleMessage(client, buf, sizeof(buf))) {
		//have buf point to the command and args to the command arguments
		args = strchr(buf, ' ');
		if (args == NULL) {
			continue;
		}
		*args++ = '\0';

		//remove color espace sequences from the arguments
		Q_CleanStr(args);

		if (Q_stricmp(buf, "cp ") == 0) { 
			/*CenterPrintf*/
		} else if (Q_stricmp(buf, "cs") == 0) { 
			/*ConfigStringModified*/
		} else if (Q_stricmp(buf, "print") == 0) {
			//remove first and last quote from the chat message
			memmove(args, args + 1, strlen(args));
			args[strlen(args) - 1] = '\0';
			trap_BotQueueConsoleMessage(bs->cs, CMS_NORMAL, args);
		} else if (Q_stricmp(buf, "chat") == 0) {
			//remove first and last quote from the chat message
			memmove(args, args + 1, strlen(args));
			args[strlen(args) - 1] = '\0';
			trap_BotQueueConsoleMessage(bs->cs, CMS_CHAT, args);
		} else if (Q_stricmp(buf, "tchat") == 0) {
			//remove first and last quote from the chat message
			memmove(args, args + 1, strlen(args));
			args[strlen(args) - 1] = '\0';
			trap_BotQueueConsoleMessage(bs->cs, CMS_CHAT, args);
		} else if (Q_stricmp(buf, "scores") == 0) { 
			/*FIXME: parse scores?*/
		} else if (Q_stricmp(buf, "clientLevelShot") == 0) { 
			/*ignore*/
		}
	}
	//add the delta angles to the bot's current view angles
	for (j = 0; j < 3; j++) {
		bs->viewangles[j] = AngleMod(bs->viewangles[j] + SHORT2ANGLE(bs->cur_ps.delta_angles[j]));
	}
	//increase the local time of the bot
	bs->ltime += thinktime;
	
	bs->thinktime = thinktime;

	//origin of the bot
	VectorCopy(bs->cur_ps.origin, bs->origin);

	//eye coordinates of the bot
	VectorCopy(bs->cur_ps.origin, bs->eye);
	bs->eye[2] += bs->cur_ps.viewheight;

	//get the area the bot is in
	bs->areanum = BotPointAreaNum(bs->origin);

	//the real AI
	BotDeathmatchAI(bs, thinktime);

	//set the weapon selection every AI frame
	trap_EA_SelectWeapon(bs->client, bs->weaponnum);

	//subtract the delta angles
	for (j = 0; j < 3; j++) {
		bs->viewangles[j] = AngleMod(bs->viewangles[j] - SHORT2ANGLE(bs->cur_ps.delta_angles[j]));
	}

	//everything was ok
	return qtrue;
}

/*
==================
BotScheduleBotThink
==================
*/
static void AI_main_BotScheduleBotThink(void) {
	int32_t i = 0;
	int32_t botnum = 0;

	for (; i < MAX_CLIENTS; i++) {
		if (botstates[i] == NULL || !botstates[i]->inuse) {
			continue;
		}

		//initialize the bot think residual time
		botstates[i]->botthink_residual = bot_thinktime.integer * botnum / numbots;
		botnum++;
	}
}

/*
==============
BotAISetupClient
==============
*/
int32_t AI_main_BotAISetupClient(int32_t client, struct bot_settings_s* settings) {
	char filename[AI_MAX_PATH];
	char name[AI_MAX_PATH];
	char gender[AI_MAX_PATH];
	bot_state_t* bs = NULL;
	int32_t errnum = 0;

	if (botstates[client] == NULL) {
		botstates[client] = static_cast<bot_state_t*>(G_Alloc(sizeof(bot_state_t)));
	}
	bs = botstates[client];

	AI_Assert(bs, qfalse)

	if (bs->inuse) {
		AI_main_BotAIPrint(PRT_FATAL, "BotAISetupClient: client %d already setup\n", client);
		return qfalse;
	}

	if (trap_AAS_Initialized() == 0) {
		AI_main_BotAIPrint(PRT_FATAL, "AAS not initialized\n");
		return qfalse;
	}

	//load the bot character
	bs->character = trap_BotLoadCharacter(settings->characterfile, settings->skill);
	if (bs->character == 0) {
		AI_main_BotAIPrint(PRT_FATAL, "couldn't load skill %d from %s\n", settings->skill, settings->characterfile);
		return qfalse;
	}

	//copy the settings
	memcpy(&bs->settings, settings, sizeof(bot_settings_t));

	//allocate a goal state
	bs->gs = trap_BotAllocGoalState(client);

	//load the item weights
	trap_Characteristic_String(bs->character, CHARACTERISTIC_ITEMWEIGHTS, filename, AI_MAX_PATH);
	errnum = trap_BotLoadItemWeights(bs->gs, filename);
	if (errnum != BLERR_NOERROR) {
		trap_BotFreeGoalState(bs->gs);
		return qfalse;
	}

	//allocate a weapon state
	bs->ws = trap_BotAllocWeaponState();

	//load the weapon weights
	trap_Characteristic_String(bs->character, CHARACTERISTIC_WEAPONWEIGHTS, filename, AI_MAX_PATH);
	errnum = trap_BotLoadWeaponWeights(bs->ws, filename);
	if (errnum != BLERR_NOERROR) {
		trap_BotFreeGoalState(bs->gs);
		trap_BotFreeWeaponState(bs->ws);
		return qfalse;
	}

	//allocate a chat state
	bs->cs = trap_BotAllocChatState();

	//load the chat file
	trap_Characteristic_String(bs->character, CHARACTERISTIC_CHAT_FILE, filename, AI_MAX_PATH);
	trap_Characteristic_String(bs->character, CHARACTERISTIC_CHAT_NAME, name, AI_MAX_PATH);
	errnum = trap_BotLoadChatFile(bs->cs, filename, name);
	if (errnum != BLERR_NOERROR) {
		trap_BotFreeChatState(bs->cs);
		trap_BotFreeGoalState(bs->gs);
		trap_BotFreeWeaponState(bs->ws);
		return qfalse;
	}

	//get the gender characteristic
	trap_Characteristic_String(bs->character, CHARACTERISTIC_GENDER, gender, AI_MAX_PATH);
	
	//set the chat gender
	if (*gender == 'f' || *gender == 'F') {
		trap_BotSetChatGender(bs->cs, CHAT_GENDERFEMALE);
	} else if (*gender == 'm' || *gender == 'M') {
		trap_BotSetChatGender(bs->cs, CHAT_GENDERMALE);
	} else {
		trap_BotSetChatGender(bs->cs, CHAT_GENDERLESS);
	}

	bs->inuse = qtrue;
	bs->client = client;
	bs->entitynum = client;
	bs->setupcount = 4;
	bs->entergame_time = trap_AAS_Time();
	bs->ms = trap_BotAllocMoveState();
	bs->walker = 0;
	numbots++;

	if (trap_Cvar_VariableIntegerValue("bot_testichat")) {
		trap_BotLibVarSet("bot_testichat", "1");
		BotChatTest(bs);
	}

	//NOTE: reschedule the bot thinking
	AI_main_BotScheduleBotThink();

	//if interbreeding start with a mutation
	if (bot_interbreed != 0) {
		trap_BotMutateGoalFuzzyLogic(bs->gs, 1);
	}

	//bot has been setup succesfully
	return qtrue;
}

/*
==============
BotAIShutdownClient
==============
*/
int32_t AI_main_BotAIShutdownClient(int32_t client) {
	bot_state_t* bs = NULL;

	bs = botstates[client];
	if (bs == NULL || !bs->inuse) {
		//BotAI_Print(PRT_ERROR, "BotAIShutdownClient: client %d already shutdown\n", client);
		return qfalse;
	}

	if (BotChat_ExitGame(bs)) {
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	trap_BotFreeMoveState(bs->ms);

	//free the goal state	
	trap_BotFreeGoalState(bs->gs);

	//free the chat file
	trap_BotFreeChatState(bs->cs);

	//free the weapon weights
	trap_BotFreeWeaponState(bs->ws);

	//free the bot character
	trap_BotFreeCharacter(bs->character);
	
	BotFreeWaypoints(bs->checkpoints);
	BotFreeWaypoints(bs->patrolpoints);

	//clear the bot state
	memset(bs, 0, sizeof(bot_state_t));

	//set the inuse flag to qfalse
	bs->inuse = qfalse;

	//there's one bot less
	numbots--;

	//everything went ok
	return qtrue;
}

/*
==============
BotResetState

called when a bot enters the intermission or observer mode and
when the level is changed
==============
*/
void AI_main_BotResetState(bot_state_t* bs) {
	int32_t client = 0;
	int32_t entitynum = 0;
	int32_t inuse = 0;
	int32_t movestate = 0;
	int32_t goalstate = 0;
	int32_t chatstate = 0;
	int32_t weaponstate = 0;
	int32_t character;
	bot_settings_t settings;
	playerState_t ps; //current player state
	double entergame_time = 0.0;

	AI_Assert(bs, (void)0)

	//save some things that should not be reset here
	memcpy(&settings, &bs->settings, sizeof(bot_settings_t));
	memcpy(&ps, &bs->cur_ps, sizeof(playerState_t));
	inuse = bs->inuse;
	client = bs->client;
	entitynum = bs->entitynum;
	character = bs->character;
	movestate = bs->ms;
	goalstate = bs->gs;
	chatstate = bs->cs;
	weaponstate = bs->ws;
	entergame_time = bs->entergame_time;

	//free checkpoints and patrol points
	BotFreeWaypoints(bs->checkpoints);
	BotFreeWaypoints(bs->patrolpoints);

	//reset the whole state
	memset(bs, 0, sizeof(bot_state_t));

	//copy back some state stuff that should not be reset
	bs->ms = movestate;
	bs->gs = goalstate;
	bs->cs = chatstate;
	bs->ws = weaponstate;
	memcpy(&bs->cur_ps, &ps, sizeof(playerState_t));
	memcpy(&bs->settings, &settings, sizeof(bot_settings_t));
	bs->inuse = inuse;
	bs->client = client;
	bs->entitynum = entitynum;
	bs->character = character;
	bs->entergame_time = entergame_time;

	//reset several states
	if (bs->ms) trap_BotResetMoveState(bs->ms);
	if (bs->gs) trap_BotResetGoalState(bs->gs);
	if (bs->ws) trap_BotResetWeaponState(bs->ws);
	if (bs->gs) trap_BotResetAvoidGoals(bs->gs);
	if (bs->ms) trap_BotResetAvoidReach(bs->ms);
}

/*
==============
BotAILoadMap
==============
*/
int32_t AI_main_BotAILoadMap(int32_t restart) {
	int32_t i = 0;
	vmCvar_t mapname;

	if (restart == 0) {
		trap_Cvar_Register(&mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM);
		trap_BotLibLoadMap(mapname.string);
	}

	for (; i < MAX_CLIENTS; i++) {
		if (botstates[i] && botstates[i]->inuse) {
			AI_main_BotResetState(botstates[i]);
			botstates[i]->setupcount = 4;
		}
	}

	BotSetupDeathmatchAI();

	return qtrue;
}

/*
==================
BotAIStartFrame
==================
*/
int32_t AI_main_BotAIStartFrame(int32_t time) {
	int32_t i = 0;
	gentity_t* ent = NULL;
	bot_entitystate_t state;
	int32_t elapsed_time = 0;
	int32_t thinktime = 0;
	static int32_t local_time;
	static int32_t botlib_residual;
	static int32_t lastbotthink_time;

	G_CheckBotSpawn();

	trap_Cvar_Update(&bot_rocketjump);
	trap_Cvar_Update(&bot_grapple);
	trap_Cvar_Update(&bot_fastchat);
	trap_Cvar_Update(&bot_nochat);
	trap_Cvar_Update(&bot_testrchat);
	trap_Cvar_Update(&bot_thinktime);
	trap_Cvar_Update(&bot_memorydump);
	trap_Cvar_Update(&bot_pause);
	trap_Cvar_Update(&bot_report);

	if (bot_report.integer != 0) {
		AI_main_BotTeamplayReport();
		trap_Cvar_Set("bot_report", "0");
	}

	if (bot_pause.integer != 0) {
		// execute bot user commands every frame
		for (; i < MAX_CLIENTS; i++) {
			if (botstates[i] == NULL || !botstates[i]->inuse) {
				continue;
			}

			if (g_entities[i].client->pers.connected != CON_CONNECTED) {
				continue;
			}

			botstates[i]->lastucmd.forwardmove = 0;
			botstates[i]->lastucmd.rightmove = 0;
			botstates[i]->lastucmd.upmove = 0;
			botstates[i]->lastucmd.buttons = 0;
			botstates[i]->lastucmd.serverTime = time;
			trap_BotUserCommand(botstates[i]->client, &botstates[i]->lastucmd);
		}
		return qtrue;
	}

	if (bot_memorydump.integer != 0) {
		trap_BotLibVarSet("memorydump", "1");
		trap_Cvar_Set("bot_memorydump", "0");
	}

	//check if bot interbreeding is activated
	AI_main_BotInterbreeding();

	//cap the bot think time
	if (bot_thinktime.integer > 200) {
		trap_Cvar_Set("bot_thinktime", "200");
	}

	//if the bot think time changed we should reschedule the bots
	if (bot_thinktime.integer != lastbotthink_time) {
		lastbotthink_time = bot_thinktime.integer;
		AI_main_BotScheduleBotThink();
	}

	elapsed_time = time - local_time;
	local_time = time;

	botlib_residual += elapsed_time;

	if (elapsed_time > bot_thinktime.integer) {
		thinktime = elapsed_time;
	} else {
		thinktime = bot_thinktime.integer;
	}

	// update the bot library
	if (botlib_residual >= thinktime) {
		botlib_residual -= thinktime;

		trap_BotLibStartFrame((float)time / 1000);

		if (trap_AAS_Initialized() == 0) {
			return qfalse;
		}

		//update entities in the botlib
		for (i = 0; i < MAX_GENTITIES; i++) {
			ent = &g_entities[i];

			if (ent == NULL) {
				trap_BotLibUpdateEntity(i, NULL);
				continue;
			}

			if (!ent->inuse) {
				trap_BotLibUpdateEntity(i, NULL);
				continue;
			}

			if (!ent->r.linked) {
				trap_BotLibUpdateEntity(i, NULL);
				continue;
			}

			if (ent->r.svFlags & SVF_NOCLIENT) {
				trap_BotLibUpdateEntity(i, NULL);
				continue;
			}

			// do not update missiles
			if (ent->s.eType == ET_MISSILE) {
				trap_BotLibUpdateEntity(i, NULL);
				continue;
			}

			// do not update event only entities
			if (ent->s.eType > ET_EVENTS) {
				trap_BotLibUpdateEntity(i, NULL);
				continue;
			}
			
			memset(&state, 0, sizeof(bot_entitystate_t));
			
			VectorCopy(ent->r.currentOrigin, state.origin);
			if (i < MAX_CLIENTS) {
				VectorCopy(ent->s.apos.trBase, state.angles);
			} else {
				VectorCopy(ent->r.currentAngles, state.angles);
			}
			VectorCopy(ent->s.origin2, state.old_origin);
			VectorCopy(ent->r.mins, state.mins);
			VectorCopy(ent->r.maxs, state.maxs);
			state.type = ent->s.eType;
			state.flags = ent->s.eFlags;
			state.solid = ent->r.bmodel ? SOLID_BSP : SOLID_BBOX;
			state.groundent = ent->s.groundEntityNum;
			state.modelindex = ent->s.modelindex;
			state.modelindex2 = ent->s.modelindex2;
			state.frame = ent->s.frame;
			state.event = ent->s.event;
			state.eventParm = ent->s.eventParm;
			state.powerups = ent->s.powerups;
			state.legsAnim = ent->s.legsAnim;
			state.torsoAnim = ent->s.torsoAnim;
			state.weapon = ent->s.weapon;
			
			trap_BotLibUpdateEntity(i, &state);
		}

		AI_main_BotAIRegularUpdate();
	}

	// execute scheduled bot AI
	for (i = 0; i < MAX_CLIENTS; i++) {
		if (botstates[i] == NULL || !botstates[i]->inuse) {
			continue;
		}
		
		botstates[i]->botthink_residual += elapsed_time;
		
		if (botstates[i]->botthink_residual >= thinktime) {
			botstates[i]->botthink_residual -= thinktime;

			if (trap_AAS_Initialized() == 0) {
				return qfalse;
			}

			if (g_entities[i].client->pers.connected == CON_CONNECTED) {
				AI_main_BotAI(i, (float)thinktime / 1000);
			}
		}
	}


	// execute bot user commands every frame
	for (i = 0; i < MAX_CLIENTS; i++) {
		if (botstates[i] == NULL || !botstates[i]->inuse) {
			continue;
		}

		if (g_entities[i].client->pers.connected != CON_CONNECTED) {
			continue;
		}

		AI_main_BotUpdateInput(botstates[i], time, elapsed_time);
		trap_BotUserCommand(botstates[i]->client, &botstates[i]->lastucmd);
	}

	return qtrue;
}

/*
==============
BotInitLibrary
==============
*/
static int32_t AI_main_BotInitLibrary(void) {
	int32_t gt = 0;
	char buf[144];

	//set the maxclients and maxentities library variables before calling BotSetupLibrary
	trap_Cvar_VariableStringBuffer("sv_maxclients", buf, sizeof(buf));
	if (strlen(buf) == 0) {
		strcpy(buf, "8");
	}
	trap_BotLibVarSet("maxclients", buf);
	Com_sprintf(buf, sizeof(buf), "%d", MAX_GENTITIES);
	trap_BotLibVarSet("maxentities", buf);

	//bsp checksum
	trap_Cvar_VariableStringBuffer("sv_mapChecksum", buf, sizeof(buf));
	if (strlen(buf) != 0) {
		trap_BotLibVarSet("sv_mapChecksum", buf);
	}

	//maximum number of aas links
	trap_Cvar_VariableStringBuffer("max_aaslinks", buf, sizeof(buf));
	if (strlen(buf) != 0) {
		trap_BotLibVarSet("max_aaslinks", buf);
	}

	//maximum number of items in a level
	trap_Cvar_VariableStringBuffer("max_levelitems", buf, sizeof(buf));
	if (strlen(buf) != 0) {
		trap_BotLibVarSet("max_levelitems", buf);
	}

	//game type
	gt = trap_Cvar_VariableIntegerValue("g_gametype");
	if (gt == GT_SINGLE_PLAYER) {
		gt = AIGT_SINGLE_PLAYER;
	} else if (gt >= GT_TEAM) {
		gt = AIGT_TEAM;
	} else {
		gt = AIGT_OTHER;
	}
	trap_BotLibVarSet("ai_gametype", va("%i", gt));

	//bot developer mode and log file
	trap_Cvar_VariableStringBuffer("bot_developer", buf, sizeof(buf));
	if (strlen(buf) == 0) {
		strcpy(buf, "0");
	}
	trap_BotLibVarSet("bot_developer", buf);
	trap_BotLibVarSet("log", buf);
	
	//no chatting
	trap_Cvar_VariableStringBuffer("bot_nochat", buf, sizeof(buf));
	if (strlen(buf) != 0) {
		trap_BotLibVarSet("nochat", "0");
	}

	//visualize jump pads
	trap_Cvar_VariableStringBuffer("bot_visualizejumppads", buf, sizeof(buf));
	if (strlen(buf) != 0) {
		trap_BotLibVarSet("bot_visualizejumppads", buf);
	}

	//forced clustering calculations
	trap_Cvar_VariableStringBuffer("bot_forceclustering", buf, sizeof(buf));
	if (strlen(buf) != 0) {
		trap_BotLibVarSet("forceclustering", buf);
	}

	//forced reachability calculations
	trap_Cvar_VariableStringBuffer("bot_forcereachability", buf, sizeof(buf));
	if (strlen(buf) != 0) {
		trap_BotLibVarSet("forcereachability", buf);
	}

	//force writing of AAS to file
	trap_Cvar_VariableStringBuffer("bot_forcewrite", buf, sizeof(buf));
	if (strlen(buf) != 0) {
		trap_BotLibVarSet("forcewrite", buf);
	}

	//no AAS optimization
	trap_Cvar_VariableStringBuffer("bot_aasoptimize", buf, sizeof(buf));
	if (strlen(buf) != 0) {
		trap_BotLibVarSet("aasoptimize", buf);
	}

	//reload instead of cache bot character files
	trap_Cvar_VariableStringBuffer("bot_reloadcharacters", buf, sizeof(buf));
	if (strlen(buf) == 0) {
		strcpy(buf, "0");
	}
	trap_BotLibVarSet("bot_reloadcharacters", buf);
	
	//base directory
	trap_Cvar_VariableStringBuffer("fs_basepath", buf, sizeof(buf));
	if (strlen(buf) != 0) {
		trap_BotLibVarSet("basedir", buf);
	}
	
	//game directory
	trap_Cvar_VariableStringBuffer("fs_game", buf, sizeof(buf));
	if (strlen(buf) != 0) {
		trap_BotLibVarSet("gamedir", buf);
	}

	//cd directory
	trap_Cvar_VariableStringBuffer("fs_cdpath", buf, sizeof(buf));
	if (strlen(buf) != 0) {
		trap_BotLibVarSet("cddir", buf);
	}

	//setup the bot library
	return trap_BotLibSetup();
}

/*
==============
BotAISetup
==============
*/
int32_t AI_main_BotAISetup(int32_t restart) {
	int32_t	errnum = 0;

#ifdef RANDOMIZE
	srand((unsigned)time(NULL));
#endif //RANDOMIZE

	bot_setupComplete = qtrue;

	trap_Cvar_Register(&bot_thinktime, "bot_thinktime", "100", CVAR_CHEAT);
	trap_Cvar_Register(&bot_memorydump, "bot_memorydump", "0", CVAR_CHEAT);
	trap_Cvar_Register(&bot_pause, "bot_pause", "0", CVAR_CHEAT);
	trap_Cvar_Register(&bot_report, "bot_report", "0", CVAR_CHEAT);
	trap_Cvar_Register(&bot_testsolid, "bot_testsolid", "0", CVAR_CHEAT);
	trap_Cvar_Register(&bot_interbreedchar, "bot_interbreedchar", "", 0);
	trap_Cvar_Register(&bot_interbreedbots, "bot_interbreedbots", "10", 0);
	trap_Cvar_Register(&bot_interbreedcycle, "bot_interbreedcycle", "20", 0);
	trap_Cvar_Register(&bot_interbreedwrite, "bot_interbreedwrite", "", 0);

	//if the game is restarted for a tournament
	if (restart != 0) {
		return qtrue;
	}

	//initialize the bot states
	memset(botstates, 0, sizeof(botstates));

	errnum = AI_main_BotInitLibrary();
	if (errnum != BLERR_NOERROR) {
		return qfalse;
	}

	return qtrue;
}

/*
==============
BotAIShutdown
==============
*/
int32_t AI_main_BotAIShutdown(int32_t restart) {
	//if the game is restarted for a tournament
	if (restart != 0) {
		int32_t i = 0;

		//shutdown all the bots in the botlib
		for (; i < MAX_CLIENTS; i++) {
			if (botstates[i] != NULL && botstates[i]->inuse) {
				AI_main_BotAIShutdownClient(botstates[i]->client);
			}
		}
		//don't shutdown the bot library
	} else {
		trap_BotLibShutdown();
	}
	return qtrue;
}

