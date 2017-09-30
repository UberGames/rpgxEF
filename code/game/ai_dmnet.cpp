// Copyright (C) 1999-2000 Id Software, Inc.
//

/*****************************************************************************
 * name:		ai_dmnet.c
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /StarTrek/Code-DM/game/ai_dmnet.c $
 * $Author: Mgummelt $ 
 * $Revision: 7 $
 * $Modtime: 3/28/01 11:12a $
 * $Date: 3/28/01 11:15a $
 *
 *****************************************************************************/

#include "g_local.h"
#include "botlib.h"
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
//data file headers
#include "chars.h"			//characteristics
#include "inv.h"			//indexes into the inventory
#include "syn.h"			//synonyms
#include "match.h"			//string matching types and vars

#include "g_syscalls.h"
#include "ai_common.h"

//goal flag, see be_ai_goal.h for the other GFL_*
static const int32_t GFL_AIR = 128;

int32_t numnodeswitches;
char nodeswitch[MAX_NODESWITCHES+1][144];

// pre declare functions
static void AI_dmnet_AIEnter_Seek_NBG(bot_state_t* bs);
static void AI_dmnet_AIEnter_Battle_SuicidalFight(bot_state_t* bs);
static int32_t AI_dmnet_AINode_Battle_Chase(bot_state_t* bs);
static int32_t AI_dmnet_AINode_Battle_Fight(bot_state_t* bs);
static void AI_dmnet_AIEnter_Battle_Retreat(bot_state_t* bs);
static int32_t AI_dmnet_AINode_Battle_Retreat(bot_state_t* bs);;
static void AI_dmnet_AIEnter_Battle_NBG(bot_state_t* bs);
static int32_t AI_dmnet_AINode_Battle_NBG(bot_state_t* bs);

/*
==================
BotResetNodeSwitches
==================
*/
void AI_dmnet_BotResetNodeSwitches(void) {
	numnodeswitches = 0;
}

/*
==================
BotDumpNodeSwitches
==================
*/
void AI_dmnet_BotDumpNodeSwitches(bot_state_t* bs) {
	int32_t i = 0;
	char netname[MAX_NETNAME];

	AI_Assert(bs, (void)0)

	ClientName(bs->client, netname, sizeof(netname));
	AI_main_BotAIPrint(PRT_MESSAGE, "%s at %1.1f switched more than %d AI nodes\n", netname, trap_AAS_Time(), MAX_NODESWITCHES);
	for (; i < numnodeswitches; i++) {
		AI_main_BotAIPrint(PRT_MESSAGE, "%s", nodeswitch[i]);
	}
	AI_main_BotAIPrint(PRT_FATAL, "");
}

/*
==================
BotRecordNodeSwitch
==================
*/
static void AI_dmnet_BotRecordNodeSwitch(bot_state_t* bs, char* node, char* str) {
	char netname[MAX_NETNAME];

	AI_Assert(bs, (void)0);
	AI_Assert(node, (void)0);
	AI_Assert(str, (void)0);

	ClientName(bs->client, netname, sizeof(netname));
	Com_sprintf(nodeswitch[numnodeswitches], 144, "%s at %2.1f entered %s: %s\n", netname, trap_AAS_Time(), node, str);
#ifdef DEBUG
	if (0) {
		AI_main_BotAIPrint(PRT_MESSAGE, nodeswitch[numnodeswitches]);
	}
#endif //DEBUG
	numnodeswitches++;
}

/*
==================
BotGetAirGoal
==================
*/
static int32_t AI_dmnet_BotGetAirGoal(bot_state_t* bs, bot_goal_t* goal) {
	bsp_trace_t bsptrace;
	vec3_t end;
	vec3_t mins = { -15, -15, -2 };
	vec3_t maxs = { 15, 15, 2 };

	AI_Assert(bs, qfalse);
	AI_Assert(goal, qfalse);

	//trace up until we hit solid
	VectorCopy(bs->origin, end);
	end[2] += 1000;
	AI_main_BotAITrace(&bsptrace, bs->origin, mins, maxs, end, bs->entitynum, CONTENTS_SOLID|CONTENTS_PLAYERCLIP);
	//trace down until we hit water
	VectorCopy(bsptrace.endpos, end);
	AI_main_BotAITrace(&bsptrace, end, mins, maxs, bs->origin, bs->entitynum, CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA);
	//if we found the water surface
	if (bsptrace.fraction > 0) {
		int32_t areanum = BotPointAreaNum(bsptrace.endpos);
		if (areanum != 0) {
			VectorCopy(bsptrace.endpos, goal->origin);
			goal->origin[2] -= 2;
			goal->areanum = areanum;
			goal->mins[0] = -15;
			goal->mins[1] = -15;
			goal->mins[2] = -1;
			goal->maxs[0] = 15;
			goal->maxs[1] = 15;
			goal->maxs[2] = 1;
			goal->flags = GFL_AIR;
			goal->number = 0;
			goal->iteminfo = 0;
			goal->entitynum = 0;
			return qtrue;
		}
	}
	return qfalse;
}

/*
==================
BotGoForAir
==================
*/
static int32_t AI_dmnet_BotGoForAir(bot_state_t* bs, int32_t tfl, bot_goal_t* ltg, double range) {
	bot_goal_t goal;

	AI_Assert(bs, qfalse);

	//if the bot needs air
	if (bs->lastair_time < trap_AAS_Time() - 6) {
		//
#ifdef DEBUG
		//BotAI_Print(PRT_MESSAGE, "going for air\n");
#endif //DEBUG
		//if we can find an air goal
		if (AI_dmnet_BotGetAirGoal(bs, &goal)) {
			trap_BotPushGoal(bs->gs, &goal);
			return qtrue;
		} else {
			qboolean botRoamsOnly = qtrue;
			/*if ( bs->cur_ps.persistant[PERS_CLASS]!=PC_NOCLASS && bs->cur_ps.persistant[PERS_CLASS]!=PC_ACTIONHERO )
			{
				botRoamsOnly = qtrue;
			}*/
			//get a nearby goal outside the water
			while( trap_BotChooseNBGItem( bs->gs, bs->origin, bs->inventory, tfl, ltg, range, botRoamsOnly ) ) {
				trap_BotGetTopGoal(bs->gs, &goal);
				//if the goal is not in water
				if (0 == (trap_AAS_PointContents(goal.origin) & (CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA))) {
					return qtrue;
				}
				trap_BotPopGoal(bs->gs);
			}
			trap_BotResetAvoidGoals(bs->gs);
		}
	}
	return qfalse;
}

/*
==================
BotNearbyGoal
==================
*/
static int32_t AI_dmnet_BotNearbyGoal(bot_state_t* bs, int32_t tfl, bot_goal_t* ltg, double range) {
	int32_t ret = 0;
	qboolean botRoamsOnly = qtrue;

	AI_Assert(bs, qfalse);

	//check if the bot should go for air
	if (AI_dmnet_BotGoForAir(bs, tfl, ltg, range) != 0) {
		return qtrue;
	}

	//if the bot is carrying the enemy flag
	if (BotCTFCarryingFlag(bs) != 0) {
		//if the bot is just a few secs away from the base 
		if (trap_AAS_AreaTravelTimeToGoalArea(bs->areanum, bs->origin, bs->teamgoal.areanum, TFL_DEFAULT) < 300) {
			//make the range really small
			range = 50;
		}
	}
	//
	/*if ( bs->cur_ps.persistant[PERS_CLASS]!=PC_NOCLASS && bs->cur_ps.persistant[PERS_CLASS]!=PC_ACTIONHERO )
	{
		botRoamsOnly = qtrue;
	}*/
	ret = trap_BotChooseNBGItem(bs->gs, bs->origin, bs->inventory, tfl, ltg, range, botRoamsOnly );
	/*
	if (ret)
	{
		char buf[128];
		//get the goal at the top of the stack
		trap_BotGetTopGoal(bs->gs, &goal);
		trap_BotGoalName(goal.number, buf, sizeof(buf));
		BotAI_Print(PRT_MESSAGE, "%1.1f: new nearby goal %s\n", trap_AAS_Time(), buf);
	}
	*/
	return ret;
}

/*
==================
BotReachedGoal
==================
*/
static int32_t AI_dmnet_BotReachedGoal(bot_state_t* bs, bot_goal_t* goal) {
	AI_Assert(bs, qfalse);
	AI_Assert(goal, qfalse);

	if ((goal->flags & GFL_ITEM) != 0) {
		//if touching the goal
		if (trap_BotTouchingGoal(bs->origin, goal) != 0) {
			return qtrue;
		}

		//if the goal isn't there
		if (trap_BotItemGoalInVisButNotVisible(bs->entitynum, bs->eye, bs->viewangles, goal) != 0) {
			return qtrue;
		}

		//if in the goal area and below or above the goal and not swimming
		if (bs->areanum == goal->areanum) {
			if (bs->origin[0] > goal->origin[0] + goal->mins[0] && bs->origin[0] < goal->origin[0] + goal->maxs[0]) {
				if (bs->origin[1] > goal->origin[1] + goal->mins[1] && bs->origin[1] < goal->origin[1] + goal->maxs[1]) {
					if (trap_AAS_Swimming(bs->origin) == 0) {
						return qtrue;
					}
				}
			}
		}
	} else if ((goal->flags & GFL_AIR) != 0) {
		//if touching the goal
		if (trap_BotTouchingGoal(bs->origin, goal) != 0) {
			return qtrue;
		}

		//if the bot got air
		if (bs->lastair_time > trap_AAS_Time() - 1) {
			return qtrue;
		}
	} else {
		//if touching the goal
		if (trap_BotTouchingGoal(bs->origin, goal) != 0) {
			return qtrue;
		}
	}

	return qfalse;
}

/*
==================
BotGetItemLongTermGoal
==================
*/
static int32_t AI_dmnet_BotGetItemLongTermGoal(bot_state_t* bs, int32_t tfl, bot_goal_t* goal) {
	AI_Assert(bs, qfalse);

	//if the bot has no goal
	if (trap_BotGetTopGoal(bs->gs, goal) == 0) {
		//BotAI_Print(PRT_MESSAGE, "no ltg on stack\n");
		bs->ltg_time = 0;
	} else if (AI_dmnet_BotReachedGoal(bs, goal) != 0) { //if the bot touches the current goal
		BotChooseWeapon(bs);
		bs->ltg_time = 0;
	}

	//if it is time to find a new long term goal
	if (bs->ltg_time < trap_AAS_Time()) {
		qboolean botRoamsOnly = qtrue;
		//pop the current goal from the stack
		trap_BotPopGoal(bs->gs);
		//BotAI_Print(PRT_MESSAGE, "%s: choosing new ltg\n", ClientName(bs->client, netname, sizeof(netname)));
		//choose a new goal
		//BotAI_Print(PRT_MESSAGE, "%6.1f client %d: BotChooseLTGItem\n", trap_AAS_Time(), bs->client);
		/*if ( bs->cur_ps.persistant[PERS_CLASS]!=PC_NOCLASS && bs->cur_ps.persistant[PERS_CLASS]!=PC_ACTIONHERO )
		{
			botRoamsOnly = qtrue;
		}*/
		if (trap_BotChooseLTGItem(bs->gs, bs->origin, bs->inventory, tfl, botRoamsOnly) != 0) {
			/*
			char buf[128];
			//get the goal at the top of the stack
			trap_BotGetTopGoal(bs->gs, goal);
			trap_BotGoalName(goal->number, buf, sizeof(buf));
			BotAI_Print(PRT_MESSAGE, "%1.1f: new long term goal %s\n", trap_AAS_Time(), buf);
			*/
			bs->ltg_time = trap_AAS_Time() + 20;
		} else {//the bot gets sorta stuck with all the avoid timings, shouldn't happen though
			//
#ifdef DEBUG
			//char netname[128];

			//BotAI_Print(PRT_MESSAGE, "%s: no valid ltg (probably stuck)\n", ClientName(bs->client, netname, sizeof(netname)));
#endif
			//trap_BotDumpAvoidGoals(bs->gs);
			//reset the avoid goals and the avoid reach
			trap_BotResetAvoidGoals(bs->gs);
			trap_BotResetAvoidReach(bs->ms);
		}
		//get the goal at the top of the stack
		return trap_BotGetTopGoal(bs->gs, goal);
	}
	return qtrue;
}

/*
==================
BotGetLongTermGoal

we could also create a seperate AI node for every long term goal type
however this saves us a lot of code
==================
*/
static int32_t AI_dmnet_BotGetLongTermGoal(bot_state_t* bs, int32_t tfl, int32_t retreat, bot_goal_t* goal) {
	vec3_t target;
	vec3_t dir;
	char netname[MAX_NETNAME];
	char buf[MAX_MESSAGE_SIZE];
	int32_t areanum;
	double croucher;
	aas_entityinfo_t entinfo;
	bot_waypoint_t* wp;

	AI_Assert(bs, qfalse);

	if (bs->ltgtype == LTG_TEAMHELP && retreat == 0) {
		//check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < trap_AAS_Time()) {
			AI_main_BotAIInitialChat(bs, "help_start", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			bs->teammessage_time = 0;
		}
		
		//if trying to help the team mate for more than a minute
		if (bs->teamgoal_time < trap_AAS_Time()) {
			bs->ltgtype = 0;
		}

		//if the team mate IS visible for quite some time
		if (bs->teammatevisible_time < trap_AAS_Time() - 10) {
			bs->ltgtype = 0;
		}

		//get entity information of the companion
		AI_main_BotEntityInfo(bs->teammate, &entinfo);
		
		//if the team mate is visible
		if (BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, bs->teammate) != 0.0) {
			//if close just stand still there
			VectorSubtract(entinfo.origin, bs->origin, dir);
			if (VectorLength(dir) < 100) {
				trap_BotResetAvoidReach(bs->ms);
				return qfalse;
			}
		} else {
			//last time the bot was NOT visible
			bs->teammatevisible_time = trap_AAS_Time();
		}

		//if the entity information is valid (entity in PVS)
		if (entinfo.valid != 0) {
			areanum = BotPointAreaNum(entinfo.origin);
			if (areanum != 0 && trap_AAS_AreaReachability(areanum) != 0) {
				//update team goal
				bs->teamgoal.entitynum = bs->teammate;
				bs->teamgoal.areanum = areanum;
				VectorCopy(entinfo.origin, bs->teamgoal.origin);
				VectorSet(bs->teamgoal.mins, -8, -8, -8);
				VectorSet(bs->teamgoal.maxs, 8, 8, 8);
			}
		}
		memcpy(goal, &bs->teamgoal, sizeof(bot_goal_t));
		return qtrue;
	}

	//if the bot accompanies someone
	if (bs->ltgtype == LTG_TEAMACCOMPANY && retreat == 0) {
		//check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < trap_AAS_Time()) {
			AI_main_BotAIInitialChat(bs, "accompany_start", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			bs->teammessage_time = 0;
		}

		//if accompanying the companion for 3 minutes
		if (bs->teamgoal_time < trap_AAS_Time()) {
			AI_main_BotAIInitialChat(bs, "accompany_stop", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			bs->ltgtype = 0;
		}

		//get entity information of the companion
		AI_main_BotEntityInfo(bs->teammate, &entinfo);
		
		//if the companion is visible
		if (BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, bs->teammate)) {
			//update visible time
			bs->teammatevisible_time = trap_AAS_Time();
			VectorSubtract(entinfo.origin, bs->origin, dir);
			if (VectorLength(dir) < bs->formation_dist) {
				//check if the bot wants to crouch
				//don't crouch if crouched less than 5 seconds ago
				if (bs->attackcrouch_time < trap_AAS_Time() - 5) {
					croucher = 1;
					if (random() < bs->thinktime * croucher) {
						bs->attackcrouch_time = trap_AAS_Time() + 5 + croucher * 15;
					}
				}
				
				//don't crouch when swimming
				if (trap_AAS_Swimming(bs->origin) != 0) {
					bs->attackcrouch_time = trap_AAS_Time() - 1;
				}
				
				//if not arrived yet or arived some time ago
				if (bs->arrive_time < trap_AAS_Time() - 2) {
					//if not arrived yet
					if (!bs->arrive_time) {
						trap_EA_Gesture(bs->client);
						AI_main_BotAIInitialChat(bs, "accompany_arrive", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
						trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
						bs->arrive_time = trap_AAS_Time();
					} else if (bs->attackcrouch_time > trap_AAS_Time()) { //if the bot wants to crouch
						trap_EA_Crouch(bs->client);
					} else if (random() < bs->thinktime * 0.3) { //else do some model taunts
						//do a gesture :)
						trap_EA_Gesture(bs->client);
					}
				}

				//if just arrived look at the companion
				if (bs->arrive_time > trap_AAS_Time() - 2) {
					VectorSubtract(entinfo.origin, bs->origin, dir);
					vectoangles(dir, bs->ideal_viewangles);
					bs->ideal_viewangles[2] *= 0.5;
				} else if (random() < bs->thinktime * 0.8) { //else look strategically around for enemies
					BotRoamGoal(bs, target);
					VectorSubtract(target, bs->origin, dir);
					vectoangles(dir, bs->ideal_viewangles);
					bs->ideal_viewangles[2] *= 0.5;
				}

				//check if the bot wants to go for air
				if (AI_dmnet_BotGoForAir(bs, bs->tfl, &bs->teamgoal, 400)) {
					trap_BotResetLastAvoidReach(bs->ms);
					//get the goal at the top of the stack
					//trap_BotGetTopGoal(bs->gs, &tmpgoal);
					//trap_BotGoalName(tmpgoal.number, buf, 144);
					//BotAI_Print(PRT_MESSAGE, "new nearby goal %s\n", buf);
					//time the bot gets to pick up the nearby goal item
					bs->nbg_time = trap_AAS_Time() + 8;
					AI_dmnet_AIEnter_Seek_NBG(bs);
					return qfalse;
				}
				
				trap_BotResetAvoidReach(bs->ms);
				return qfalse;
			}
		}

		//if the entity information is valid (entity in PVS)
		if (entinfo.valid != 0) {
			areanum = BotPointAreaNum(entinfo.origin);
			if (areanum != 0 && trap_AAS_AreaReachability(areanum) != 0) {
				//update team goal so bot will accompany 
				bs->teamgoal.entitynum = bs->teammate;
				bs->teamgoal.areanum = areanum;
				VectorCopy(entinfo.origin, bs->teamgoal.origin);
				VectorSet(bs->teamgoal.mins, -8, -8, -8);
				VectorSet(bs->teamgoal.maxs, 8, 8, 8);
			}
		}

		//the goal the bot should go for
		memcpy(goal, &bs->teamgoal, sizeof(bot_goal_t));
		
		//if the companion is NOT visible for too long
		if (bs->teammatevisible_time < trap_AAS_Time() - 60) {
			AI_main_BotAIInitialChat(bs, "accompany_cannotfind", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			bs->ltgtype = 0;
		}
		return qtrue;
	}
	
	if (bs->ltgtype == LTG_DEFENDKEYAREA) {
		if (trap_AAS_AreaTravelTimeToGoalArea(bs->areanum, bs->origin, bs->teamgoal.areanum, TFL_DEFAULT) > bs->defendaway_range) {
			bs->defendaway_time = 0;
		}
	}

	//if defending a key area
	if (bs->ltgtype == LTG_DEFENDKEYAREA && !retreat && bs->defendaway_time < trap_AAS_Time()) {
		//check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < trap_AAS_Time()) {
			trap_BotGoalName(bs->teamgoal.number, buf, sizeof(buf));
			AI_main_BotAIInitialChat(bs, "defend_start", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			bs->teammessage_time = 0;
		}

		//set the bot goal
		memcpy(goal, &bs->teamgoal, sizeof(bot_goal_t));
		
		//stop after 2 minutes
		if (bs->teamgoal_time < trap_AAS_Time()) {
			trap_BotGoalName(bs->teamgoal.number, buf, sizeof(buf));
			AI_main_BotAIInitialChat(bs, "defend_stop", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			bs->ltgtype = 0;
		}

		//if very close... go away for some time
		VectorSubtract(goal->origin, bs->origin, dir);
		if (VectorLength(dir) < 70) {
			trap_BotResetAvoidReach(bs->ms);
			bs->defendaway_time = trap_AAS_Time() + 2 + 5 * random();
			bs->defendaway_range = 250;
		}
		return qtrue;
	}

	//going to kill someone
	if (bs->ltgtype == LTG_KILL && !retreat) {
		//check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < trap_AAS_Time()) {
			EasyClientName(bs->teamgoal.entitynum, buf, sizeof(buf));
			AI_main_BotAIInitialChat(bs, "kill_start", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			bs->teammessage_time = 0;
		}
		
		if (bs->lastkilledplayer == bs->teamgoal.entitynum) {
			EasyClientName(bs->teamgoal.entitynum, buf, sizeof(buf));
			AI_main_BotAIInitialChat(bs, "kill_done", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			bs->lastkilledplayer = -1;
			bs->ltgtype = 0;
		}
		
		if (bs->teamgoal_time < trap_AAS_Time()) {
			bs->ltgtype = 0;
		}

		//just roam around
		return AI_dmnet_BotGetItemLongTermGoal(bs, tfl, goal);
	}

	//get an item
	if (bs->ltgtype == LTG_GETITEM && !retreat) {
		//check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < trap_AAS_Time()) {
			trap_BotGoalName(bs->teamgoal.number, buf, sizeof(buf));
			AI_main_BotAIInitialChat(bs, "getitem_start", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			bs->teammessage_time = 0;
		}

		//set the bot goal
		memcpy(goal, &bs->teamgoal, sizeof(bot_goal_t));

		//stop after some time
		if (bs->teamgoal_time < trap_AAS_Time()) {
			bs->ltgtype = 0;
		}
		
		if (trap_BotItemGoalInVisButNotVisible(bs->entitynum, bs->eye, bs->viewangles, goal) != 0) {
			trap_BotGoalName(bs->teamgoal.number, buf, sizeof(buf));
			AI_main_BotAIInitialChat(bs, "getitem_notthere", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			bs->ltgtype = 0;
		} else if (AI_dmnet_BotReachedGoal(bs, goal) != 0) {
			trap_BotGoalName(bs->teamgoal.number, buf, sizeof(buf));
			AI_main_BotAIInitialChat(bs, "getitem_gotit", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			bs->ltgtype = 0;
		}
		return qtrue;
	}

	//if camping somewhere
	if ((bs->ltgtype == LTG_CAMP || bs->ltgtype == LTG_CAMPORDER) && !retreat) {
		//check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < trap_AAS_Time()) {
			if (bs->ltgtype == LTG_CAMPORDER) {
				AI_main_BotAIInitialChat(bs, "camp_start", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
				trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			}
			bs->teammessage_time = 0;
		}

		//set the bot goal
		memcpy(goal, &bs->teamgoal, sizeof(bot_goal_t));
		
		if (bs->teamgoal_time < trap_AAS_Time()) {
			if (bs->ltgtype == LTG_CAMPORDER) {
				AI_main_BotAIInitialChat(bs, "camp_stop", NULL);
				trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			}
			bs->ltgtype = 0;
		}

		//if really near the camp spot
		VectorSubtract(goal->origin, bs->origin, dir);
		if (VectorLength(dir) < 60)
		{
			//if not arrived yet
			if (!bs->arrive_time) {
				if (bs->ltgtype == LTG_CAMPORDER) {
					AI_main_BotAIInitialChat(bs, "camp_arrive", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
					trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
				}
				bs->arrive_time = trap_AAS_Time();
			}

			//look strategically around for enemies
			if (random() < bs->thinktime * 0.8) {
				BotRoamGoal(bs, target);
				VectorSubtract(target, bs->origin, dir);
				vectoangles(dir, bs->ideal_viewangles);
				bs->ideal_viewangles[2] *= 0.5;
			}

			//check if the bot wants to crouch
			//don't crouch if crouched less than 5 seconds ago
			if (bs->attackcrouch_time < trap_AAS_Time() - 5) {
				croucher = 1;
				if (random() < bs->thinktime * croucher) {
					bs->attackcrouch_time = trap_AAS_Time() + 5 + croucher * 15;
				}
			}

			//if the bot wants to crouch
			if (bs->attackcrouch_time > trap_AAS_Time()) {
				trap_EA_Crouch(bs->client);
			}

			//don't crouch when swimming
			if (trap_AAS_Swimming(bs->origin)) bs->attackcrouch_time = trap_AAS_Time() - 1;
			
			//make sure the bot is not gonna drown
			if ((trap_PointContents(bs->eye,bs->entitynum) & (CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA)) != 0) {
				if (bs->ltgtype == LTG_CAMPORDER) {
					AI_main_BotAIInitialChat(bs, "camp_stop", NULL);
					trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
				}
				bs->ltgtype = 0;
			}
			
			if (bs->camp_range > 0) {
				//FIXME: move around a bit
			}
			
			trap_BotResetAvoidReach(bs->ms);
			return qfalse;
		}
		return qtrue;
	}

	//patrolling along several waypoints
	if (bs->ltgtype == LTG_PATROL && !retreat) {
		//check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < trap_AAS_Time()) {
			strcpy(buf, "");
			for (wp = bs->patrolpoints; wp; wp = wp->next) {
				strcat(buf, wp->name);
				if (wp->next) strcat(buf, " to ");
			}
			AI_main_BotAIInitialChat(bs, "patrol_start", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			bs->teammessage_time = 0;
		}
		
		if (bs->curpatrolpoint != NULL) {
			bs->ltgtype = 0;
			return qfalse;
		}

		//if the bot touches the current goal
		if (trap_BotTouchingGoal(bs->origin, &bs->curpatrolpoint->goal) != 0) {
			if ((bs->patrolflags & PATROL_BACK) != 0) {
				if (bs->curpatrolpoint->prev != NULL) {
					bs->curpatrolpoint = bs->curpatrolpoint->prev;
				} else {
					bs->curpatrolpoint = bs->curpatrolpoint->next;
					bs->patrolflags &= ~PATROL_BACK;
				}
			} else {
				if (bs->curpatrolpoint->next != NULL) {
					bs->curpatrolpoint = bs->curpatrolpoint->next;
				} else {
					bs->curpatrolpoint = bs->curpatrolpoint->prev;
					bs->patrolflags |= PATROL_BACK;
				}
			}
		}

		//stop after 5 minutes
		if (bs->teamgoal_time < trap_AAS_Time()) {
			AI_main_BotAIInitialChat(bs, "patrol_stop", NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			bs->ltgtype = 0;
		}

		if (bs->curpatrolpoint != NULL) {
			bs->ltgtype = 0;
			return qfalse;
		}
		memcpy(goal, &bs->curpatrolpoint->goal, sizeof(bot_goal_t));
		return qtrue;
	}

#ifdef CTF
	//if going for enemy flag
	if (bs->ltgtype == LTG_GETFLAG) {
		//check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < trap_AAS_Time()) {
			AI_main_BotAIInitialChat(bs, "captureflag_start", NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			bs->teammessage_time = 0;
		}
		//
		switch(BotCTFTeam(bs)) {
			case CTF_TEAM_RED:
			{
				*goal = ctf_blueflag;
				break;
			}
			case CTF_TEAM_BLUE:
			{
				*goal = ctf_redflag; 
				break;
			}
			default:
			{
				bs->ltgtype = 0; 
				return qfalse;
			}
		}

		//if touching the flag
		if (trap_BotTouchingGoal(bs->origin, goal) != 0) {
			bs->ltgtype = 0;
		}

		//stop after 3 minutes
		if (bs->teamgoal_time < trap_AAS_Time()) {
#ifdef DEBUG
			AI_main_BotAIPrint(PRT_MESSAGE, "%s: I quit getting the flag\n", ClientName(bs->client, netname, sizeof(netname)));
#endif //DEBUG
			bs->ltgtype = 0;
		}
		return qtrue;
	}

	//if rushing to the base
	if (bs->ltgtype == LTG_RUSHBASE && bs->rushbaseaway_time < trap_AAS_Time()) {
		switch(BotCTFTeam(bs)) {
			case CTF_TEAM_RED: 
			{
				*goal = ctf_redflag;
				break; 
			}
			case CTF_TEAM_BLUE:
			{
				*goal = ctf_blueflag; 
				break;
			}
			default: bs->ltgtype = 0; return qfalse;
		}

		//if not carrying the flag anymore
		if (BotCTFCarryingFlag(bs) == 0) {
			bs->ltgtype = 0;
		}

		//quit rushing after 2 minutes
		if (bs->teamgoal_time < trap_AAS_Time()) {
			bs->ltgtype = 0;
		}

		//if touching the base flag the bot should loose the enemy flag
		if (trap_BotTouchingGoal(bs->origin, goal) != 0) {
			//if the bot is still carrying the enemy flag then the
			//base flag is gone, now just walk near the base a bit
			if (BotCTFCarryingFlag(bs) != 0) {
				trap_BotResetAvoidReach(bs->ms);
				bs->rushbaseaway_time = trap_AAS_Time() + 5 + 10 * random();
				//FIXME: add chat to tell the others to get back the flag
				//FIXME: Make them camp?  Get health?  Preserve themselves?
			} else {
				bs->ltgtype = 0;
			}
		}

		return qtrue;
	}

	//returning flag
	if (bs->ltgtype == LTG_RETURNFLAG) {
		//check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < trap_AAS_Time()) {
			EasyClientName(bs->teamgoal.entitynum, buf, sizeof(buf));
			AI_main_BotAIInitialChat(bs, "returnflag_start", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			bs->teammessage_time = 0;
		}
		
		if (bs->teamgoal_time < trap_AAS_Time()) {
			bs->ltgtype = 0;
		}
		//FIXME:  Uh.... we're trying to retrieve our flag, shouldn't
		//		  we set that as our goal somewhere?
		//ALSO: Can't we also easily implement the ability to pick up
		//		the enemy flag if it's dropped in the field by a teammate?

		//just roam around
		return AI_dmnet_BotGetItemLongTermGoal(bs, tfl, goal);
	}
#endif //CTF

	//normal goal stuff
	return AI_dmnet_BotGetItemLongTermGoal(bs, tfl, goal);
}

/*
==================
BotLongTermGoal
==================
*/
static int32_t AI_dmnet_BotLongTermGoal(bot_state_t* bs, int32_t tfl, int32_t retreat, bot_goal_t* goal) {
	aas_entityinfo_t entinfo;
	char teammate[MAX_MESSAGE_SIZE];
	vec3_t dir;

	AI_Assert(bs, qfalse);

	//FIXME: also have air long term goals?
	//
	//if the bot is leading someone and not retreating
	if (bs->lead_time > 0 && retreat == 0) {
		double dist;

		if (bs->lead_time < trap_AAS_Time()) {
			//FIXME: add chat to tell the team mate that he/she's on his/her own
			bs->lead_time = 0;
			return AI_dmnet_BotGetLongTermGoal(bs, tfl, retreat, goal);
		}
		
		if (bs->leadmessage_time < 0 && -bs->leadmessage_time < trap_AAS_Time()) {
			AI_main_BotAIInitialChat(bs, "followme", EasyClientName(bs->lead_teammate, teammate, sizeof(teammate)), NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			bs->leadmessage_time = trap_AAS_Time();
		}

		//get entity information of the companion
		AI_main_BotEntityInfo(bs->lead_teammate, &entinfo);
		
		if (entinfo.valid != 0) {
			int32_t areanum = BotPointAreaNum(entinfo.origin);
			if (areanum != 0 && trap_AAS_AreaReachability(areanum) != 0) {
				//update team goal
				bs->lead_teamgoal.entitynum = bs->lead_teammate;
				bs->lead_teamgoal.areanum = areanum;
				VectorCopy(entinfo.origin, bs->lead_teamgoal.origin);
				VectorSet(bs->lead_teamgoal.mins, -8, -8, -8);
				VectorSet(bs->lead_teamgoal.maxs, 8, 8, 8);
			}
		}

		//if the team mate is visible
		if (BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, bs->lead_teammate)) {
			bs->leadvisible_time = trap_AAS_Time();
		}

		//if the team mate is not visible for 1 seconds
		if (bs->leadvisible_time < trap_AAS_Time() - 1) {
			bs->leadbackup_time = trap_AAS_Time() + 2;
		}

		//distance towards the team mate
		VectorSubtract(bs->origin, bs->lead_teamgoal.origin, dir);
		dist = VectorLength(dir);
		
		//if backing up towards the team mate
		if (bs->leadbackup_time > trap_AAS_Time()) {
			if (bs->leadmessage_time < trap_AAS_Time() - 20) {
				AI_main_BotAIInitialChat(bs, "followme", EasyClientName(bs->lead_teammate, teammate, sizeof(teammate)), NULL);
				trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
				bs->leadmessage_time = trap_AAS_Time();
			}

			//if very close to the team mate
			if (dist < 100) {
				bs->leadbackup_time = 0;
			}

			//the bot should go back to the team mate
			memcpy(goal, &bs->lead_teamgoal, sizeof(bot_goal_t));
			return qtrue;
		} else {
			//if quite distant from the team mate
			if (dist > 500) {
				if (bs->leadmessage_time < trap_AAS_Time() - 20) {
					AI_main_BotAIInitialChat(bs, "followme", EasyClientName(bs->lead_teammate, teammate, sizeof(teammate)), NULL);
					trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
					bs->leadmessage_time = trap_AAS_Time();
				}
				
				//look at the team mate
				VectorSubtract(entinfo.origin, bs->origin, dir);
				vectoangles(dir, bs->ideal_viewangles);
				bs->ideal_viewangles[2] *= 0.5;
				
				//just wait for the team mate
				return qfalse;
			}
		}
	}
	return AI_dmnet_BotGetLongTermGoal(bs, tfl, retreat, goal);
}

/*
==================
AINode_Intermission
==================
*/
static int32_t AI_dmnet_AINode_Intermission(bot_state_t* bs) {
	AI_Assert(bs, qfalse);

	//if the intermission ended
	if (!BotIntermission(bs)) {
		if (BotChat_StartLevel(bs) != 0) {
			bs->stand_time = trap_AAS_Time() + BotChatTime(bs);
		} else {
			bs->stand_time = trap_AAS_Time() + 2;
		}
		AI_dmnet_AIEnter_Stand(bs);
	}
	return qtrue;
}

/*
==================
AIEnter_Intermission
==================
*/
static void AI_dmnet_AIEnter_Intermission(bot_state_t* bs) {
	AI_Assert(bs, (void)0);

	AI_dmnet_BotRecordNodeSwitch(bs, "intermission", "");
	
	//reset the bot state
	AI_main_BotResetState(bs);
	
	//check for end level chat
	if (BotChat_EndLevel(bs) != 0) {
		trap_BotEnterChat(bs->cs, bs->client, bs->chatto);
	}
	bs->ainode = AI_dmnet_AINode_Intermission;
}

/*
==================
AINode_Observer
==================
*/
static int32_t AI_dmnet_AINode_Observer(bot_state_t* bs) {
	AI_Assert(bs, qfalse);

	//if the bot left observer mode
	if (!BotIsObserver(bs)) {
		AI_dmnet_AIEnter_Stand(bs);
	}

	return qtrue;
}

/*
==================
AIEnter_Observer
==================
*/
static void AI_dmnet_AIEnter_Observer(bot_state_t* bs) {
	AI_Assert(bs, (void)0);

	AI_dmnet_BotRecordNodeSwitch(bs, "observer", "");

	//reset the bot state
	AI_main_BotResetState(bs);
	bs->ainode = AI_dmnet_AINode_Observer;
}

/*
==================
AIEnter_Stand
==================
*/
void AI_dmnet_AIEnter_Stand(bot_state_t* bs) {
	AI_Assert(bs, (void)0);

	AI_dmnet_BotRecordNodeSwitch(bs, "stand", "");
	bs->standfindenemy_time = trap_AAS_Time() + 1;
	bs->ainode = AI_dmnet_AINode_Stand;
}

/*
==================
AIEnter_Battle_Fight
==================
*/
static void AI_dmnet_AIEnter_Battle_Fight(bot_state_t* bs) {

	AI_Assert(bs, (void)0);

	AI_dmnet_BotRecordNodeSwitch(bs, "battle fight", "");
	trap_BotResetLastAvoidReach(bs->ms);
	bs->ainode = AI_dmnet_AINode_Battle_Fight;
}

/*
==================
AINode_Stand
==================
*/
int32_t AI_dmnet_AINode_Stand(bot_state_t* bs) {
	AI_Assert(bs, qfalse);

	//if the bot's health decreased
	if (bs->lastframe_health > bs->inventory[INVENTORY_HEALTH]) {
		if (BotChat_HitTalking(bs) != 0) {
			bs->standfindenemy_time = trap_AAS_Time() + BotChatTime(bs) + 0.1;
			bs->stand_time = trap_AAS_Time() + BotChatTime(bs) + 0.1;
		}
	}
	if (bs->standfindenemy_time < trap_AAS_Time()) {
		if (BotFindEnemy(bs, -1)) {
			AI_dmnet_AIEnter_Battle_Fight(bs);
			return qfalse;
		}
		bs->standfindenemy_time = trap_AAS_Time() + 1;
	}

	trap_EA_Talk(bs->client);
	if (bs->stand_time < trap_AAS_Time()) {
		trap_BotEnterChat(bs->cs, bs->client, bs->chatto);
		AI_dmnet_AIEnter_Seek_LTG(bs);
		return qfalse;
	}
	
	return qtrue;
}

/*
==================
AINode_Respawn
==================
*/
int32_t AI_dmnet_AINode_Respawn(bot_state_t* bs) {
	AI_Assert(bs, qfalse);

	if (bs->respawn_wait != 0) {
		if (!BotIsDead(bs)) {
			AI_dmnet_AIEnter_Seek_LTG(bs);
		} else {
			trap_EA_Respawn(bs->client);
		}
	} else if (bs->respawn_time < trap_AAS_Time()) {
		//wait until respawned
		bs->respawn_wait = qtrue;

		//elementary action respawn
		trap_EA_Respawn(bs->client);

		if (bs->respawnchat_time) {
			trap_BotEnterChat(bs->cs, bs->client, bs->chatto);
			bs->enemy = -1;
		}
	}

	if (bs->respawnchat_time && bs->respawnchat_time < trap_AAS_Time() - 0.5) {
		trap_EA_Talk(bs->client);
	}

	return qtrue;
}

/*
==================
AIEnter_Respawn
==================
*/
static void AI_dmnet_AIEnter_Respawn(bot_state_t* bs) {
	AI_Assert(bs, (void)0);
	
	AI_dmnet_BotRecordNodeSwitch(bs, "respawn", "");
	
	//reset some states
	trap_BotResetMoveState(bs->ms);
	trap_BotResetGoalState(bs->gs);
	trap_BotResetAvoidGoals(bs->gs);
	trap_BotResetAvoidReach(bs->ms);
	
	//if the bot wants to chat
	if (BotChat_Death(bs) != 0) {
		bs->respawn_time = trap_AAS_Time() + BotChatTime(bs);
		bs->respawnchat_time = trap_AAS_Time();
	} else {
		bs->respawn_time = trap_AAS_Time() + 1 + random();
		bs->respawnchat_time = 0;
	}

	//set respawn state
	bs->respawn_wait = qfalse;
	bs->ainode = AI_dmnet_AINode_Respawn;
}

/*
==================
AINode_Seek_Activate_Entity
==================
*/
static int32_t AI_dmnet_AINode_Seek_ActivateEntity(bot_state_t* bs) {
	bot_goal_t* goal = NULL;
	vec3_t target;
	vec3_t dir;
	bot_moveresult_t moveresult;

	AI_Assert(bs, qfalse);

	if (BotIsObserver(bs)) {
		AI_dmnet_AIEnter_Observer(bs);
		return qfalse;
	}

	//if in the intermission
	if (BotIntermission(bs)) {
		AI_dmnet_AIEnter_Intermission(bs);
		return qfalse;
	}

	//respawn if dead
	if (BotIsDead(bs)) {
		AI_dmnet_AIEnter_Respawn(bs);
		return qfalse;
	}

	bs->tfl = TFL_DEFAULT;
	if (bot_grapple.integer != 0) {
		bs->tfl |= TFL_GRAPPLEHOOK;
	}

	//if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) {
		bs->tfl |= TFL_LAVA | TFL_SLIME;
	}

	//map specific code
	BotMapScripts(bs);

	//no enemy
	bs->enemy = -1;

	goal = &bs->activategoal;

	//if the bot has no goal
	if (goal == NULL) {
		bs->activate_time = 0;
	} else if (trap_BotTouchingGoal(bs->origin, goal)) { //if the bot touches the current goal
		BotChooseWeapon(bs);
#ifdef DEBUG
		AI_main_BotAIPrint(PRT_MESSAGE, "touched button or trigger\n");
#endif //DEBUG
		bs->activate_time = 0;
	}

	if (bs->activate_time < trap_AAS_Time()) {
		AI_dmnet_AIEnter_Seek_NBG(bs);
		return qfalse;
	}

	//initialize the movement state
	BotSetupForMovement(bs);

	//move towards the goal
	trap_BotMoveToGoal(&moveresult, bs->ms, goal, bs->tfl);

	//if the movement failed
	if (moveresult.failure != 0) {
		//reset the avoid reach, otherwise bot is stuck in current area
		trap_BotResetAvoidReach(bs->ms);
		bs->nbg_time = 0;
	}

	//check if the bot is blocked
	BotAIBlocked(bs, &moveresult, qtrue);

	if ((moveresult.flags & (MOVERESULT_MOVEMENTVIEWSET | MOVERESULT_MOVEMENTVIEW | MOVERESULT_SWIMVIEW)) != 0) {
		VectorCopy(moveresult.ideal_viewangles, bs->ideal_viewangles);
	} else if (moveresult.flags & MOVERESULT_WAITING) { //if waiting for something
		if (random() < bs->thinktime * 0.8) {
			BotRoamGoal(bs, target);
			VectorSubtract(target, bs->origin, dir);
			vectoangles(dir, bs->ideal_viewangles);
			bs->ideal_viewangles[2] *= 0.5;
		}
	} else if ((bs->flags & BFL_IDEALVIEWSET) == 0) {
		if (trap_BotMovementViewTarget(bs->ms, goal, bs->tfl, 300, target)) {
			VectorSubtract(target, bs->origin, dir);
			vectoangles(dir, bs->ideal_viewangles);
		} /*else {
		  //vectoangles(moveresult.movedir, bs->ideal_viewangles);
		  }*/
		bs->ideal_viewangles[2] *= 0.5;
	}

	//if the weapon is used for the bot movement
	if ((moveresult.flags & MOVERESULT_MOVEMENTWEAPON) != 0) {
		bs->weaponnum = moveresult.weapon;
	}

	//if there is an enemy
	if (BotFindEnemy(bs, -1) != 0) {
		if (BotWantsToRetreat(bs) != 0) {
			//keep the current long term goal and retreat
			AI_dmnet_AIEnter_Battle_NBG(bs);
		} else {
			trap_BotResetLastAvoidReach(bs->ms);
			//empty the goal stack
			trap_BotEmptyGoalStack(bs->gs);
			//go fight
			AI_dmnet_AIEnter_Battle_Fight(bs);
		}
	}

	return qtrue;
}

/*
==================
AIEnter_Seek_ActivateEntity
==================
*/
void AI_dmnet_AIEnter_Seek_ActivateEntity(bot_state_t* bs) {
	AI_Assert(bs, (void)0);

	AI_dmnet_BotRecordNodeSwitch(bs, "activate entity", "");
	bs->ainode = AI_dmnet_AINode_Seek_ActivateEntity;
}

/*
==================
AIEnter_Seek_NBG
==================
*/
static void AI_dmnet_AIEnter_Seek_NBG(bot_state_t* bs) {
	bot_goal_t goal;
	char buf[144];

	AI_Assert(bs, (void)0);

	if (trap_BotGetTopGoal(bs->gs, &goal) != 0) {
		trap_BotGoalName(goal.number, buf, 144);
		AI_dmnet_BotRecordNodeSwitch(bs, "seek NBG", buf);
	} else {
		AI_dmnet_BotRecordNodeSwitch(bs, "seek NBG", "no goal");
	}
	bs->ainode = AI_dmnat_AINode_Seek_NBG;
}

/*
==================
AINode_Seek_NBG
==================
*/
int32_t AI_dmnat_AINode_Seek_NBG(bot_state_t* bs) {
	bot_goal_t goal;
	vec3_t target, dir;
	bot_moveresult_t moveresult;

	AI_Assert(bs, qfalse);

	if (BotIsObserver(bs)) {
		AI_dmnet_AIEnter_Observer(bs);
		return qfalse;
	}

	//if in the intermission
	if (BotIntermission(bs)) {
		AI_dmnet_AIEnter_Intermission(bs);
		return qfalse;
	}

	//respawn if dead
	if (BotIsDead(bs)) {
		AI_dmnet_AIEnter_Respawn(bs);
		return qfalse;
	}
	
	bs->tfl = TFL_DEFAULT;
	if (bot_grapple.integer != 0) {
		bs->tfl |= TFL_GRAPPLEHOOK;
	}

	//if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) {
		bs->tfl |= TFL_LAVA | TFL_SLIME;
	}
	
	if (BotCanAndWantsToRocketJump(bs) != 0) {
		bs->tfl |= TFL_ROCKETJUMP;
	}

	//map specific code
	BotMapScripts(bs);

	//no enemy
	bs->enemy = -1;

	//if the bot has no goal
	if (trap_BotGetTopGoal(bs->gs, &goal) == 0) {
		bs->nbg_time = 0;
	} else if (AI_dmnet_BotReachedGoal(bs, &goal)) { //if the bot touches the current goal
		BotChooseWeapon(bs);
		bs->nbg_time = 0;
	}
	
	if (bs->nbg_time < trap_AAS_Time()) {
		//pop the current goal from the stack
		trap_BotPopGoal(bs->gs);
		//check for new nearby items right away
		//NOTE: we canNOT reset the check_time to zero because it would create an endless loop of node switches
		bs->check_time = trap_AAS_Time() + 0.05;
		//go back to seek ltg
		AI_dmnet_AIEnter_Seek_LTG(bs);
		return qfalse;
	}

	//initialize the movement state
	BotSetupForMovement(bs);

	//move towards the goal
	trap_BotMoveToGoal(&moveresult, bs->ms, &goal, bs->tfl);

	//if the movement failed
	if (moveresult.failure != 0) {
		//reset the avoid reach, otherwise bot is stuck in current area
		trap_BotResetAvoidReach(bs->ms);
		bs->nbg_time = 0;
	}

	//check if the bot is blocked
	BotAIBlocked(bs, &moveresult, qtrue);

	//if the viewangles are used for the movement
	if ((moveresult.flags & (MOVERESULT_MOVEMENTVIEWSET|MOVERESULT_MOVEMENTVIEW|MOVERESULT_SWIMVIEW)) != 0) {
		VectorCopy(moveresult.ideal_viewangles, bs->ideal_viewangles);
	} else if ((moveresult.flags & MOVERESULT_WAITING) != 0) { //if waiting for something
		if (random() < bs->thinktime * 0.8) {
			BotRoamGoal(bs, target);
			VectorSubtract(target, bs->origin, dir);
			vectoangles(dir, bs->ideal_viewangles);
			bs->ideal_viewangles[2] *= 0.5;
		}
	} else if ((bs->flags & BFL_IDEALVIEWSET) == 0) {
		if (trap_BotGetSecondGoal(bs->gs, &goal) == 0) {
			trap_BotGetTopGoal(bs->gs, &goal);
		}

		if (trap_BotMovementViewTarget(bs->ms, &goal, bs->tfl, 300, target) != 0) {
			VectorSubtract(target, bs->origin, dir);
			vectoangles(dir, bs->ideal_viewangles);
		} else { //FIXME: look at cluster portals?
			vectoangles(moveresult.movedir, bs->ideal_viewangles);
		}
		bs->ideal_viewangles[2] *= 0.5;
	}

	//if the weapon is used for the bot movement
	if ((moveresult.flags & MOVERESULT_MOVEMENTWEAPON) != 0) {
		bs->weaponnum = moveresult.weapon;
	}
	
	//if there is an enemy
	if (BotFindEnemy(bs, -1) != 0) {
		if (BotWantsToRetreat(bs) != 0) {
			//keep the current long term goal and retreat
			AI_dmnet_AIEnter_Battle_NBG(bs);
		} else {
			trap_BotResetLastAvoidReach(bs->ms);
			//empty the goal stack
			trap_BotEmptyGoalStack(bs->gs);
			//go fight
			AI_dmnet_AIEnter_Battle_Fight(bs);
		}
	}

	return qtrue;
}

/*
==================
AINode_Seek_LTG
==================
*/
int32_t AI_dmnet_AINode_Seek_LTG(bot_state_t* bs) {
	bot_goal_t goal;
	vec3_t target;
	vec3_t dir;
	bot_moveresult_t moveresult;
	//char buf[128];
	//bot_goal_t tmpgoal;

	AI_Assert(bs, qfalse);

	if (BotIsObserver(bs)) {
		AI_dmnet_AIEnter_Observer(bs);
		return qfalse;
	}

	//if in the intermission
	if (BotIntermission(bs)) {
		AI_dmnet_AIEnter_Intermission(bs);
		return qfalse;
	}

	//respawn if dead
	if (BotIsDead(bs)) {
		AI_dmnet_AIEnter_Respawn(bs);
		return qfalse;
	}

	if (BotChat_Random(bs) != 0) {
		bs->stand_time = trap_AAS_Time() + BotChatTime(bs);
		AI_dmnet_AIEnter_Stand(bs);
		return qfalse;
	}

	bs->tfl = TFL_DEFAULT;
	if (bot_grapple.integer != 0) {
		bs->tfl |= TFL_GRAPPLEHOOK;
	}

	//if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) {
		bs->tfl |= TFL_LAVA | TFL_SLIME;
	}

	if (BotCanAndWantsToRocketJump(bs) != 0) {
		bs->tfl |= TFL_ROCKETJUMP;
	}

	//map specific code
	BotMapScripts(bs);

	//no enemy
	bs->enemy = -1;

	if (bs->killedenemy_time > trap_AAS_Time() - 2) {
		if (random() < bs->thinktime * 1) {
			trap_EA_Gesture(bs->client);
		}
	}

	//if there is an enemy
	if (BotFindEnemy(bs, -1) != 0) {
		if (BotWantsToRetreat(bs)) {
			//keep the current long term goal and retreat
			AI_dmnet_AIEnter_Battle_Retreat(bs);
			return qfalse;
		} else {
			trap_BotResetLastAvoidReach(bs->ms);
			//empty the goal stack
			trap_BotEmptyGoalStack(bs->gs);
			//go fight
			AI_dmnet_AIEnter_Battle_Fight(bs);
			return qfalse;
		}
	}

#ifdef CTF
	if (gametype == GT_CTF) {
		//decide what to do in CTF mode
		BotCTFSeekGoals(bs);
	}
#endif //CTF

	//get the current long term goal
	if (AI_dmnet_BotLongTermGoal(bs, bs->tfl, qfalse, &goal) == 0) {
		return qtrue;
	}

	//check for nearby goals periodicly
	if (bs->check_time < trap_AAS_Time()) {
		double range;

		bs->check_time = trap_AAS_Time() + 0.5;
		//check if the bot wants to camp
		BotWantsToCamp(bs);
		//
		if (bs->ltgtype == LTG_DEFENDKEYAREA) {
			range = 400;
		} else {
			range = 150;
		}

#ifdef CTF
		//if carrying a flag the bot shouldn't be distracted too much
		if (BotCTFCarryingFlag(bs) != 0) {
			range = 50;
		}
#endif //CTF

		if (AI_dmnet_BotNearbyGoal(bs, bs->tfl, &goal, range) != 0) {
			trap_BotResetLastAvoidReach(bs->ms);
			//get the goal at the top of the stack
			//trap_BotGetTopGoal(bs->gs, &tmpgoal);
			//trap_BotGoalName(tmpgoal.number, buf, 144);
			//BotAI_Print(PRT_MESSAGE, "new nearby goal %s\n", buf);
			//time the bot gets to pick up the nearby goal item
			bs->nbg_time = trap_AAS_Time() + 4 + range * 0.01;
			AI_dmnet_AIEnter_Seek_NBG(bs);
			return qfalse;
		}
	}

	//initialize the movement state
	BotSetupForMovement(bs);

	//move towards the goal
	trap_BotMoveToGoal(&moveresult, bs->ms, &goal, bs->tfl);

	//if the movement failed
	if (moveresult.failure != 0) {
		//reset the avoid reach, otherwise bot is stuck in current area
		trap_BotResetAvoidReach(bs->ms);
		//BotAI_Print(PRT_MESSAGE, "movement failure %d\n", moveresult.traveltype);
		bs->ltg_time = 0;
	}

	BotAIBlocked(bs, &moveresult, qtrue);

	//if the viewangles are used for the movement
	if ((moveresult.flags & (MOVERESULT_MOVEMENTVIEWSET | MOVERESULT_MOVEMENTVIEW | MOVERESULT_SWIMVIEW)) != 0) {
		VectorCopy(moveresult.ideal_viewangles, bs->ideal_viewangles);
	} else if ((moveresult.flags & MOVERESULT_WAITING) != 0) { //if waiting for something
		if (random() < bs->thinktime * 0.8) {
			BotRoamGoal(bs, target);
			VectorSubtract(target, bs->origin, dir);
			vectoangles(dir, bs->ideal_viewangles);
			bs->ideal_viewangles[2] *= 0.5;
		}
	} else if ((bs->flags & BFL_IDEALVIEWSET) == 0) {
		if (trap_BotMovementViewTarget(bs->ms, &goal, bs->tfl, 300, target)) {
			VectorSubtract(target, bs->origin, dir);
			vectoangles(dir, bs->ideal_viewangles);
		} else if (VectorLength(moveresult.movedir)) { //FIXME: look at cluster portals?
			vectoangles(moveresult.movedir, bs->ideal_viewangles);
		} else if (random() < bs->thinktime * 0.8) {
			BotRoamGoal(bs, target);
			VectorSubtract(target, bs->origin, dir);
			vectoangles(dir, bs->ideal_viewangles);
			bs->ideal_viewangles[2] *= 0.5;
		}
		bs->ideal_viewangles[2] *= 0.5;
	}

	//if the weapon is used for the bot movement
	if ((moveresult.flags & MOVERESULT_MOVEMENTWEAPON) != 0) {
		bs->weaponnum = moveresult.weapon;
	}

	return qtrue;
}

/*
==================
AIEnter_Seek_LTG
==================
*/
void AI_dmnet_AIEnter_Seek_LTG(bot_state_t* bs) {
	bot_goal_t goal;
	char buf[144];

	AI_Assert(bs, (void)0);

	if (trap_BotGetTopGoal(bs->gs, &goal) != 0) {
		trap_BotGoalName(goal.number, buf, 144);
		AI_dmnet_BotRecordNodeSwitch(bs, "seek LTG", buf);
	} else {
		AI_dmnet_BotRecordNodeSwitch(bs, "seek LTG", "no goal");
	}
	bs->ainode = AI_dmnet_AINode_Seek_LTG;
}

/*
==================
AIEnter_Battle_Fight
==================
*/
static void AI_dmnet_AIEnter_Battle_SuicidalFight(bot_state_t* bs) {

	AI_Assert(bs, (void)0);

	AI_dmnet_BotRecordNodeSwitch(bs, "battle fight", "");
	trap_BotResetLastAvoidReach(bs->ms);
	bs->ainode = AI_dmnet_AINode_Battle_Fight;
	bs->flags |= BFL_FIGHTSUICIDAL;
}

/*
==================
AIEnter_Battle_Chase
==================
*/
static void AI_dmnet_AIEnter_Battle_Chase(bot_state_t* bs) {

	AI_Assert(bs, (void)0);

	AI_dmnet_BotRecordNodeSwitch(bs, "battle chase", "");
	bs->chase_time = trap_AAS_Time();
	bs->ainode = AI_dmnet_AINode_Battle_Chase;
}

/*
==================
AINode_Battle_Fight
==================
*/
static int32_t AI_dmnet_AINode_Battle_Fight(bot_state_t* bs) {
	int32_t areanum = 0;
	aas_entityinfo_t entinfo;
	bot_moveresult_t moveresult;

	AI_Assert(bs, qfalse);

	if (BotIsObserver(bs)) {
		AI_dmnet_AIEnter_Observer(bs);
		return qfalse;
	}

	//if in the intermission
	if (BotIntermission(bs)) {
		AI_dmnet_AIEnter_Intermission(bs);
		return qfalse;
	}

	//respawn if dead
	if (BotIsDead(bs)) {
		AI_dmnet_AIEnter_Respawn(bs);
		return qfalse;
	}

	//if there is another better enemy
	if (BotFindEnemy(bs, bs->enemy) != 0) {
#ifdef DEBUG
//		BotAI_Print(PRT_MESSAGE, "found new better enemy\n");
#endif
	}

	//if no enemy
	if (bs->enemy < 0) {
		AI_dmnet_AIEnter_Seek_LTG(bs);
		return qfalse;
	}
	
	AI_main_BotEntityInfo(bs->enemy, &entinfo);

	//if the enemy is dead
	if (bs->enemydeath_time) {
		if (bs->enemydeath_time < trap_AAS_Time() - 1.0) {
			bs->enemydeath_time = 0;

			if (bs->enemysuicide != 0) {
				BotChat_EnemySuicide(bs);
			}

			if (bs->lastkilledplayer == bs->enemy && BotChat_Kill(bs)) {
				bs->stand_time = trap_AAS_Time() + BotChatTime(bs);
				AI_dmnet_AIEnter_Stand(bs);
			} else {
				bs->ltg_time = 0;
				AI_dmnet_AIEnter_Seek_LTG(bs);
			}

			return qfalse;
		}
	} else {
		if (EntityIsDead(&entinfo)) {
			bs->enemydeath_time = trap_AAS_Time();
		}
	}

	//if the enemy is invisible and not shooting the bot looses track easily
	if (EntityIsInvisible(&entinfo) && !EntityIsShooting(&entinfo)) {
		if (random() < 0.2) {
			AI_dmnet_AIEnter_Seek_LTG(bs);
			return qfalse;
		}
	}

	//update the reachability area and origin if possible
	areanum = BotPointAreaNum(entinfo.origin);
	if (areanum != 0 && trap_AAS_AreaReachability(areanum) != 0) {
		VectorCopy(entinfo.origin, bs->lastenemyorigin);
		bs->lastenemyareanum = areanum;
	}

	//update the attack inventory values
	BotUpdateBattleInventory(bs, bs->enemy);

	//if the bot's health decreased
	if (bs->lastframe_health > bs->inventory[INVENTORY_HEALTH]) {
		if (BotChat_HitNoDeath(bs) != 0) {
			bs->stand_time = trap_AAS_Time() + BotChatTime(bs);
			AI_dmnet_AIEnter_Stand(bs);
			return qfalse;
		}
	}

	//if the bot hit someone
	if (bs->cur_ps.persistant[PERS_HITS] > bs->lasthitcount)
	{
		if (BotChat_HitNoKill(bs) != 0)
		{
			bs->stand_time = trap_AAS_Time() + BotChatTime(bs);
			AI_dmnet_AIEnter_Stand(bs);
			return qfalse;
		}
	}

	//if the enemy is not visible
	if (!BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, bs->enemy)) {
		if (BotWantsToChase(bs) != 0) {
			AI_dmnet_AIEnter_Battle_Chase(bs);
			return qfalse;
		} else {
			AI_dmnet_AIEnter_Seek_LTG(bs);
			return qfalse;
		}
	}

	//use holdable items
	BotBattleUseItems(bs);
	
	bs->tfl = TFL_DEFAULT;
	if (bot_grapple.integer != 0) {
		bs->tfl |= TFL_GRAPPLEHOOK;
	}

	//if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) {
		bs->tfl |= TFL_LAVA | TFL_SLIME;
	}
	
	if (BotCanAndWantsToRocketJump(bs) != 0) {
		bs->tfl |= TFL_ROCKETJUMP;
	}

	//choose the best weapon to fight with
	BotChooseWeapon(bs);
	
	//do attack movements
	moveresult = BotAttackMove(bs, bs->tfl);
	
	//if the movement failed
	if (moveresult.failure != 0) {
		//reset the avoid reach, otherwise bot is stuck in current area
		trap_BotResetAvoidReach(bs->ms);
		//BotAI_Print(PRT_MESSAGE, "movement failure %d\n", moveresult.traveltype);
		bs->ltg_time = 0;
	}
	
	BotAIBlocked(bs, &moveresult, qfalse);
	
	//aim at the enemy
	BotAimAtEnemy(bs);
	
	//attack the enemy if possible
	BotCheckAttack(bs);
	
	//if the bot wants to retreat
	if ((bs->flags & BFL_FIGHTSUICIDAL) == 0) {
		if (BotWantsToRetreat(bs) != 0) {
			AI_dmnet_AIEnter_Battle_Retreat(bs);
			return qtrue;
		}
	}
	return qtrue;
}

/*
==================
AINode_Battle_Chase
==================
*/
static int32_t AI_dmnet_AINode_Battle_Chase(bot_state_t* bs)
{
	bot_goal_t goal;
	vec3_t target;
	vec3_t dir;
	bot_moveresult_t moveresult;

	AI_Assert(bs, qfalse);

	if (BotIsObserver(bs)) {
		AI_dmnet_AIEnter_Observer(bs);
		return qfalse;
	}

	//if in the intermission
	if (BotIntermission(bs)) {
		AI_dmnet_AIEnter_Intermission(bs);
		return qfalse;
	}

	//respawn if dead
	if (BotIsDead(bs)) {
		AI_dmnet_AIEnter_Respawn(bs);
		return qfalse;
	}

	//if no enemy
	if (bs->enemy < 0) {
		AI_dmnet_AIEnter_Seek_LTG(bs);
		return qfalse;
	}

	//if the enemy is visible
	if (BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, bs->enemy)) {
		AI_dmnet_AIEnter_Battle_Fight(bs);
		return qfalse;
	}

	//if there is another enemy
	if (BotFindEnemy(bs, -1) != 0) {
		AI_dmnet_AIEnter_Battle_Fight(bs);
		return qfalse;
	}

	//there is no last enemy area
	if (bs->lastenemyareanum == 0) {
		AI_dmnet_AIEnter_Seek_LTG(bs);
		return qfalse;
	}
	
	bs->tfl = TFL_DEFAULT;
	if (bot_grapple.integer != 0) {
		bs->tfl |= TFL_GRAPPLEHOOK;
	}

	//if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) {
		bs->tfl |= TFL_LAVA | TFL_SLIME;
	}
	
	if (BotCanAndWantsToRocketJump(bs) != 0) {
		bs->tfl |= TFL_ROCKETJUMP;
	}

	//map specific code
	BotMapScripts(bs);

	//create the chase goal
	goal.entitynum = bs->enemy;
	goal.areanum = bs->lastenemyareanum;
	VectorCopy(bs->lastenemyorigin, goal.origin);
	VectorSet(goal.mins, -8, -8, -8);
	VectorSet(goal.maxs, 8, 8, 8);

	//if the last seen enemy spot is reached the enemy could not be found
	if (trap_BotTouchingGoal(bs->origin, &goal) != 0) {
		bs->chase_time = 0;
	}

	//if there's no chase time left
	if (!bs->chase_time || bs->chase_time < trap_AAS_Time() - 10) {
		AI_dmnet_AIEnter_Seek_LTG(bs);
		return qfalse;
	}

	//check for nearby goals periodicly
	if (bs->check_time < trap_AAS_Time()) {
		double range = 150;
		bs->check_time = trap_AAS_Time() + 1;

		if (AI_dmnet_BotNearbyGoal(bs, bs->tfl, &goal, range) != 0) {
			//the bot gets 5 seconds to pick up the nearby goal item
			bs->nbg_time = trap_AAS_Time() + 0.1 * range + 1;
			trap_BotResetLastAvoidReach(bs->ms);
			AI_dmnet_AIEnter_Battle_NBG(bs);

			return qfalse;
		}
	}
	
	BotUpdateBattleInventory(bs, bs->enemy);

	//initialize the movement state
	BotSetupForMovement(bs);

	//move towards the goal
	trap_BotMoveToGoal(&moveresult, bs->ms, &goal, bs->tfl);

	//if the movement failed
	if (moveresult.failure != 0) {
		//reset the avoid reach, otherwise bot is stuck in current area
		trap_BotResetAvoidReach(bs->ms);
		//BotAI_Print(PRT_MESSAGE, "movement failure %d\n", moveresult.traveltype);
		bs->ltg_time = 0;
	}
	
	BotAIBlocked(bs, &moveresult, qfalse);
	
	if ((moveresult.flags & (MOVERESULT_MOVEMENTVIEWSET|MOVERESULT_MOVEMENTVIEW|MOVERESULT_SWIMVIEW)) != 0) {
		VectorCopy(moveresult.ideal_viewangles, bs->ideal_viewangles);
	} else if ((bs->flags & BFL_IDEALVIEWSET) == 0) {
		if (bs->chase_time > trap_AAS_Time() - 2) {
			BotAimAtEnemy(bs);
		} else {
			if (trap_BotMovementViewTarget(bs->ms, &goal, bs->tfl, 300, target) != 0) {
				VectorSubtract(target, bs->origin, dir);
				vectoangles(dir, bs->ideal_viewangles);
			} else {
				vectoangles(moveresult.movedir, bs->ideal_viewangles);
			}
		}
		bs->ideal_viewangles[2] *= 0.5;
	}

	//if the weapon is used for the bot movement
	if ((moveresult.flags & MOVERESULT_MOVEMENTWEAPON) != 0) {
		bs->weaponnum = moveresult.weapon;
	}

	//if the bot is in the area the enemy was last seen in
	if (bs->areanum == bs->lastenemyareanum) {
		bs->chase_time = 0;
	}

	//if the bot wants to retreat (the bot could have been damage during the chase)
	if (BotWantsToRetreat(bs) != 0) {
		AI_dmnet_AIEnter_Battle_Retreat(bs);
		return qtrue;
	}
	return qtrue;
}

/*
==================
AIEnter_Battle_Retreat
==================
*/
static void AI_dmnet_AIEnter_Battle_Retreat(bot_state_t* bs) {

	AI_Assert(bs, (void)0);

	AI_dmnet_BotRecordNodeSwitch(bs, "battle retreat", "");
	bs->ainode = AI_dmnet_AINode_Battle_Retreat;
}

/*
==================
AI_dmnet_AINode_Battle_Retreat
==================
*/
static int32_t AI_dmnet_AINode_Battle_Retreat(bot_state_t* bs) {
	bot_goal_t goal;
	aas_entityinfo_t entinfo;
	bot_moveresult_t moveresult;
	vec3_t target;
	vec3_t dir;

	AI_Assert(bs, qfalse);

	if (BotIsObserver(bs)) {
		AI_dmnet_AIEnter_Observer(bs);
		return qfalse;
	}

	//if in the intermission
	if (BotIntermission(bs)) {
		AI_dmnet_AIEnter_Intermission(bs);
		return qfalse;
	}

	//respawn if dead
	if (BotIsDead(bs)) {
		AI_dmnet_AIEnter_Respawn(bs);
		return qfalse;
	}

	//if no enemy
	if (bs->enemy < 0) {
		AI_dmnet_AIEnter_Seek_LTG(bs);
		return qfalse;
	}
	
	AI_main_BotEntityInfo(bs->enemy, &entinfo);
	if (EntityIsDead(&entinfo)) {
		AI_dmnet_AIEnter_Seek_LTG(bs);
		return qfalse;
	}
	
	bs->tfl = TFL_DEFAULT;
	if (bot_grapple.integer != 0) {
		bs->tfl |= TFL_GRAPPLEHOOK;
	}

	//if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) {
		bs->tfl |= TFL_LAVA | TFL_SLIME;
	}

	//map specific code
	BotMapScripts(bs);

	//update the attack inventory values
	BotUpdateBattleInventory(bs, bs->enemy);

	//if the bot doesn't want to retreat anymore... probably picked up some nice items
	if (BotWantsToChase(bs) != 0) {
		//empty the goal stack, when chasing, only the enemy is the goal
		trap_BotEmptyGoalStack(bs->gs);
		//go chase the enemy
		AI_dmnet_AIEnter_Battle_Chase(bs);
		return qfalse;
	}

	//update the last time the enemy was visible
	if (BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, bs->enemy)) {
		bs->enemyvisible_time = trap_AAS_Time();
		//update the reachability area and origin if possible
		int32_t areanum = BotPointAreaNum(entinfo.origin);
		if (areanum != 0 && trap_AAS_AreaReachability(areanum) != 0) {
			VectorCopy(entinfo.origin, bs->lastenemyorigin);
			bs->lastenemyareanum = areanum;
		}
	}

	//if the enemy is NOT visible for 4 seconds
	if (bs->enemyvisible_time < trap_AAS_Time() - 4) {
		AI_dmnet_AIEnter_Seek_LTG(bs);
		return qfalse;
	} else if (bs->enemyvisible_time < trap_AAS_Time()) { //else if the enemy is NOT visible
		//if there is another enemy
		if (BotFindEnemy(bs, -1) != 0) {
			AI_dmnet_AIEnter_Battle_Fight(bs);
			return qfalse;
		}
	}
	
#ifdef CTF
	if (gametype == GT_CTF) {
		BotCTFRetreatGoals(bs);
	}
#endif //CTF

	//use holdable items
	BotBattleUseItems(bs);

	//get the current long term goal while retreating
	if (AI_dmnet_BotLongTermGoal(bs, bs->tfl, qtrue, &goal) == 0) {
		AI_dmnet_AIEnter_Battle_SuicidalFight(bs);
		return qfalse;
	}

	//check for nearby goals periodicly
	if (bs->check_time < trap_AAS_Time()) {
		double range = 150;
		bs->check_time = trap_AAS_Time() + 1;
#ifdef CTF

		//if carrying a flag the bot shouldn't be distracted too much
		if (BotCTFCarryingFlag(bs) != 0) {
			range = 100;
		}
#endif //CTF
		
		if (AI_dmnet_BotNearbyGoal(bs, bs->tfl, &goal, range) != 0) {
			trap_BotResetLastAvoidReach(bs->ms);
			//time the bot gets to pick up the nearby goal item
			bs->nbg_time = trap_AAS_Time() + range / 100 + 1;
			AI_dmnet_AIEnter_Battle_NBG(bs);
			return qfalse;
		}
	}

	//initialize the movement state
	BotSetupForMovement(bs);

	//move towards the goal
	trap_BotMoveToGoal(&moveresult, bs->ms, &goal, bs->tfl);

	//if the movement failed
	if (moveresult.failure != 0) {
		//reset the avoid reach, otherwise bot is stuck in current area
		trap_BotResetAvoidReach(bs->ms);
		//BotAI_Print(PRT_MESSAGE, "movement failure %d\n", moveresult.traveltype);
		bs->ltg_time = 0;
	}
	
	BotAIBlocked(bs, &moveresult, qfalse);

	//choose the best weapon to fight with
	BotChooseWeapon(bs);

	//if the view is fixed for the movement
	if ((moveresult.flags & (MOVERESULT_MOVEMENTVIEW|MOVERESULT_SWIMVIEW)) != 0) {
		VectorCopy(moveresult.ideal_viewangles, bs->ideal_viewangles);
	} else if ((moveresult.flags & MOVERESULT_MOVEMENTVIEWSET) == 0 && (bs->flags & BFL_IDEALVIEWSET) == 0)	{
		double attack_skill = 1;

		//if the bot is skilled anough
		if (attack_skill > 0.3) {
			BotAimAtEnemy(bs);
		} else {
			if (trap_BotMovementViewTarget(bs->ms, &goal, bs->tfl, 300, target) != 0) {
				VectorSubtract(target, bs->origin, dir);
				vectoangles(dir, bs->ideal_viewangles);
			} else {
				vectoangles(moveresult.movedir, bs->ideal_viewangles);
			}
			bs->ideal_viewangles[2] *= 0.5;
		}
	}

	//if the weapon is used for the bot movement
	if ((moveresult.flags & MOVERESULT_MOVEMENTWEAPON) != 0) {
		bs->weaponnum = moveresult.weapon;
	}

	//attack the enemy if possible
	BotCheckAttack(bs);
	
	return qtrue;
}

/*
==================
AI_dmnet_AIEnter_Battle_NBG
==================
*/
static void AI_dmnet_AIEnter_Battle_NBG(bot_state_t* bs) {

	AI_Assert(bs, (void)0);

	AI_dmnet_BotRecordNodeSwitch(bs, "battle NBG", "");
	bs->ainode = AI_dmnet_AINode_Battle_NBG;
}

/*
==================
AI_dmnet_AINode_Battle_NBG
==================
*/
static int32_t AI_dmnet_AINode_Battle_NBG(bot_state_t* bs) {
	bot_goal_t goal;
	aas_entityinfo_t entinfo;
	bot_moveresult_t moveresult;
	vec3_t target;
	vec3_t dir;

	AI_Assert(bs, qfalse);

	if (BotIsObserver(bs)) {
		AI_dmnet_AIEnter_Observer(bs);
		return qfalse;
	}

	//if in the intermission
	if (BotIntermission(bs)) {
		AI_dmnet_AIEnter_Intermission(bs);
		return qfalse;
	}

	//respawn if dead
	if (BotIsDead(bs)) {
		AI_dmnet_AIEnter_Respawn(bs);
		return qfalse;
	}

	//if no enemy
	if (bs->enemy < 0) {
		AI_dmnet_AIEnter_Seek_NBG(bs);
		return qfalse;
	}
	
	AI_main_BotEntityInfo(bs->enemy, &entinfo);
	if (EntityIsDead(&entinfo)) {
		AI_dmnet_AIEnter_Seek_NBG(bs);
		return qfalse;
	}

	bs->tfl = TFL_DEFAULT;
	if (bot_grapple.integer != 0) {
		bs->tfl |= TFL_GRAPPLEHOOK;
	}

	//if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) {
		bs->tfl |= TFL_LAVA | TFL_SLIME;
	}
	
	if (BotCanAndWantsToRocketJump(bs) != 0) {
		bs->tfl |= TFL_ROCKETJUMP;
	}

	//map specific code
	BotMapScripts(bs);

	//update the last time the enemy was visible
	if (BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, bs->enemy)) {
		bs->enemyvisible_time = trap_AAS_Time();

		//update the reachability area and origin if possible
		int32_t areanum = BotPointAreaNum(entinfo.origin);
		if (areanum != 0 && trap_AAS_AreaReachability(areanum) != 0) {
			VectorCopy(entinfo.origin, bs->lastenemyorigin);
			bs->lastenemyareanum = areanum;
		}
	}

	//if the bot has no goal or touches the current goal
	if (trap_BotGetTopGoal(bs->gs, &goal) == 0) {
		bs->nbg_time = 0;
	} else if (trap_BotTouchingGoal(bs->origin, &goal) != 0) {
		bs->nbg_time = 0;
	}
	
	if (bs->nbg_time < trap_AAS_Time()) {
		//pop the current goal from the stack
		trap_BotPopGoal(bs->gs);

		//if the bot still has a goal
		if (trap_BotGetTopGoal(bs->gs, &goal) != 0) {
			AI_dmnet_AIEnter_Battle_Retreat(bs);
		} else {
			AI_dmnet_AIEnter_Battle_Fight(bs);
		}

		return qfalse;
	}

	//initialize the movement state
	BotSetupForMovement(bs);

	//move towards the goal
	trap_BotMoveToGoal(&moveresult, bs->ms, &goal, bs->tfl);

	//if the movement failed
	if (moveresult.failure != 0) {
		//reset the avoid reach, otherwise bot is stuck in current area
		trap_BotResetAvoidReach(bs->ms);
		//BotAI_Print(PRT_MESSAGE, "movement failure %d\n", moveresult.traveltype);
		bs->nbg_time = 0;
	}
	
	BotAIBlocked(bs, &moveresult, qfalse);

	//update the attack inventory values
	BotUpdateBattleInventory(bs, bs->enemy);

	//choose the best weapon to fight with
	BotChooseWeapon(bs);

	//if the view is fixed for the movement
	if ((moveresult.flags & (MOVERESULT_MOVEMENTVIEW|MOVERESULT_SWIMVIEW)) != 0) {
		VectorCopy(moveresult.ideal_viewangles, bs->ideal_viewangles);
	} else if ((moveresult.flags & MOVERESULT_MOVEMENTVIEWSET) == 0 && (bs->flags & BFL_IDEALVIEWSET) == 0) {
		double attack_skill = 1;

		//if the bot is skilled anough and the enemy is visible
		if (attack_skill > 0.3) {
			//&& BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, bs->enemy)
			BotAimAtEnemy(bs);
		} else {
			if (trap_BotMovementViewTarget(bs->ms, &goal, bs->tfl, 300, target) != 0) {
				VectorSubtract(target, bs->origin, dir);
				vectoangles(dir, bs->ideal_viewangles);
			} else {
				vectoangles(moveresult.movedir, bs->ideal_viewangles);
			}
			bs->ideal_viewangles[2] *= 0.5;
		}
	}

	//if the weapon is used for the bot movement
	if ((moveresult.flags & MOVERESULT_MOVEMENTWEAPON) != 0) {
		bs->weaponnum = moveresult.weapon;
	}

	//attack the enemy if possible
	BotCheckAttack(bs);
	
	return qtrue;
}

