// Copyright (C) 1999-2000 Id Software, Inc.
//

/*****************************************************************************
 * name:		ai_team.c
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /StarTrek/Code-DM/game/ai_team.c $
 * $Author: Jmonroe $ 
 * $Revision: 1 $
 * $Modtime: 1/21/00 10:12p $
 * $Date: 1/25/00 6:26p $
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

//ctf task preferences for a client
typedef struct bot_ctftaskpreference_s
{
	char		name[36];
	int			preference;
} bot_ctftaskpreference_t;

bot_ctftaskpreference_t ctftaskpreferences[MAX_CLIENTS];


/*
==================
BotValidTeamLeader
==================
*/
int BotValidTeamLeader(bot_state_t *bs) {
	if (!strlen(bs->teamleader)) return qfalse;
	if (ClientFromName(bs->teamleader) == -1) return qfalse;
	return qtrue;
}

/*
==================
BotNumTeamMates
==================
*/
int BotNumTeamMates(bot_state_t *bs) {
	int i, numplayers;
	char buf[MAX_INFO_STRING];
	static int maxclis;

	if (!maxclis)
		maxclis = trap_Cvar_VariableIntegerValue("sv_maxclients");

	numplayers = 0;
	for (i = 0; i < maxclis && i < MAX_CLIENTS; i++) {
		trap_GetConfigstring(CS_PLAYERS+i, buf, sizeof(buf));
		//if no config string or no name
		if (!strlen(buf) || !strlen(Info_ValueForKey(buf, "n"))) continue;
		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) continue;
		//
		if (BotSameTeam(bs, i)) {
			numplayers++;
		}
	}
	return numplayers;
}

/*
==================
BotClientTravelTimeToGoal
==================
*/
int BotClientTravelTimeToGoal(int client, bot_goal_t *goal) {
	playerState_t ps;
	int areanum;

	BotAI_GetClientState(client, &ps);
	areanum = BotPointAreaNum(ps.origin);
	if (!areanum) return 1;
	return trap_AAS_AreaTravelTimeToGoalArea(areanum, ps.origin, goal->areanum, TFL_DEFAULT);
}

/*
==================
BotSortTeamMatesByBaseTravelTime
==================
*/
int BotSortTeamMatesByBaseTravelTime(bot_state_t *bs, int *teammates, int maxteammates) {

	int i, j, k, numteammates, traveltime;
	char buf[MAX_INFO_STRING];
	static int maxclients;
	int traveltimes[MAX_CLIENTS];
	bot_goal_t *goal;

	if (BotCTFTeam(bs) == CTF_TEAM_RED) goal = &ctf_redflag;
	else goal = &ctf_blueflag;

	if (!maxclients)
		maxclients = trap_Cvar_VariableIntegerValue("sv_maxclients");

	numteammates = 0;
	for (i = 0; i < maxclients && i < MAX_CLIENTS; i++) {
		trap_GetConfigstring(CS_PLAYERS+i, buf, sizeof(buf));
		//if no config string or no name
		if (!strlen(buf) || !strlen(Info_ValueForKey(buf, "n"))) continue;
		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) continue;
		//
		if (BotSameTeam(bs, i)) {
			//
			traveltime = BotClientTravelTimeToGoal(i, goal);
			//
			for (j = 0; j < numteammates; j++) {
				if (traveltime < traveltimes[j]) {
					for (k = numteammates; k > j; k--) {
						traveltimes[k] = traveltimes[k-1];
						teammates[k] = teammates[k-1];
					}
					traveltimes[j] = traveltime;
					teammates[j] = i;
					break;
				}
			}
			if (j >= numteammates) {
				traveltimes[j] = traveltime;
				teammates[j] = i;
			}
			numteammates++;
			if (numteammates >= maxteammates) break;
		}
	}
	return numteammates;
}

/*
==================
BotGetTeamMateCTFPreference
==================
*/
void BotSetTeamMateCTFPreference(bot_state_t *bs, int teammate, int preference) {
	char teammatename[MAX_NETNAME];

	ctftaskpreferences[teammate].preference = preference;
	ClientName(teammate, teammatename, sizeof(teammatename));
	strcpy(ctftaskpreferences[teammate].name, teammatename);
}

/*
==================
BotGetTeamMateCTFPreference
==================
*/
int BotGetTeamMateCTFPreference(bot_state_t *bs, int teammate) {
	char teammatename[MAX_NETNAME];

	if (!ctftaskpreferences[teammate].preference) return 0;
	ClientName(teammate, teammatename, sizeof(teammatename));
	if (Q_stricmp(teammatename, ctftaskpreferences[teammate].name)) return 0;
	return ctftaskpreferences[teammate].preference;
}

/*
==================
BotSortTeamMatesByCTFPreference
==================
*/
int BotSortTeamMatesByCTFPreference(bot_state_t *bs, int *teammates, int numteammates) {
	int defenders[MAX_CLIENTS], numdefenders;
	int attackers[MAX_CLIENTS], numattackers;
	int roamers[MAX_CLIENTS], numroamers;
	int i, preference;

	numdefenders = numattackers = numroamers = 0;
	for (i = 0; i < numteammates; i++) {
		preference = BotGetTeamMateCTFPreference(bs, teammates[i]);
		if (preference & CTFTP_DEFENDER) {
			defenders[numdefenders++] = teammates[i];
		}
		else if (preference & CTFTP_ATTACKER) {
			attackers[numattackers++] = teammates[i];
		}
		else {
			roamers[numroamers++] = teammates[i];
		}
	}
	numteammates = 0;
	//defenders at the front of the list
	memcpy(&teammates[numteammates], defenders, numdefenders);
	numteammates += numdefenders;
	//roamers in the middle
	memcpy(&teammates[numteammates], roamers, numroamers);
	numteammates += numroamers;
	//attacker in the back of the list
	memcpy(&teammates[numteammates], attackers, numattackers);
	numteammates += numattackers;

	return numteammates;
}

/*
==================
BotSayTeamOrders
==================
*/
void BotSayTeamOrder(bot_state_t *bs, int toclient) {
	char teamchat[MAX_MESSAGE_SIZE];
	char buf[MAX_MESSAGE_SIZE];
	char name[MAX_NETNAME];

	//if the bot is talking to itself
	if (bs->client == toclient) {
		//don't show the message just put it in the console message queue
		trap_BotGetChatMessage(bs->cs, buf, sizeof(buf));
		ClientName(bs->client, name, sizeof(name));
		Com_sprintf(teamchat, sizeof(teamchat), "(%s): %s", name, buf);
		trap_BotQueueConsoleMessage(bs->cs, CMS_CHAT, teamchat);
	}
	else {
		trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
	}
}

/*
==================
BotCTFOrders
==================
*/
void BotCTFOrders_BothFlagsNotAtBase(bot_state_t *bs) {
	int numteammates, defenders, attackers, i, other;
	int teammates[MAX_CLIENTS];
	char name[MAX_NETNAME], carriername[MAX_NETNAME];

	numteammates = BotSortTeamMatesByBaseTravelTime(bs, teammates, sizeof(teammates));
	BotSortTeamMatesByCTFPreference(bs, teammates, numteammates);
	//different orders based on the number of team mates
	switch(bs->numteammates) {
		case 1: break;
		case 2:
		{
			//tell the one not carrying the flag to attack the enemy base
			if (teammates[0] != bs->flagcarrier) other = teammates[0];
			else other = teammates[1];
			ClientName(other, name, sizeof(name));
			BotAI_BotInitialChat(bs, "cmd_getflag", name, NULL);
			BotSayTeamOrder(bs, other);
			break;
		}
		case 3:
		{
			//tell the one closest to the base not carrying the flag to accompany the flag carrier
			if (teammates[0] != bs->flagcarrier) other = teammates[0];
			else other = teammates[1];
			ClientName(other, name, sizeof(name));
			ClientName(bs->flagcarrier, carriername, sizeof(carriername));
			if (bs->flagcarrier == bs->client) {
				BotAI_BotInitialChat(bs, "cmd_accompanyme", name, NULL);
			}
			else {
				BotAI_BotInitialChat(bs, "cmd_accompany", name, carriername, NULL);
			}
			BotSayTeamOrder(bs, other);
			//tell the one furthest from the the base not carrying the flag to get the enemy flag
			if (teammates[2] != bs->flagcarrier) other = teammates[2];
			else other = teammates[1];
			ClientName(other, name, sizeof(name));
			BotAI_BotInitialChat(bs, "cmd_getflag", name, NULL);
			BotSayTeamOrder(bs, other);
			break;
		}
		default:
		{
			defenders = (int) (float) numteammates * 0.4 + 0.5;
			if (defenders > 1) defenders = 1;
			attackers = (int) (float) numteammates * 0.5 + 0.5;
			ClientName(bs->flagcarrier, carriername, sizeof(carriername));
			for (i = 0; i < defenders; i++) {
				//
				if (teammates[i] == bs->flagcarrier) {
					continue;
				}
				//
				ClientName(teammates[i], name, sizeof(name));
				if (bs->flagcarrier == bs->client) {
					BotAI_BotInitialChat(bs, "cmd_accompanyme", name, NULL);
				}
				else {
					BotAI_BotInitialChat(bs, "cmd_accompany", name, carriername, NULL);
				}
				BotSayTeamOrder(bs, teammates[i]);
			}
			for (i = 0; i < attackers; i++) {
				//
				if (teammates[numteammates - i - 1] == bs->flagcarrier) {
					continue;
				}
				//
				ClientName(teammates[numteammates - i - 1], name, sizeof(name));
				BotAI_BotInitialChat(bs, "cmd_getflag", name, NULL);
				BotSayTeamOrder(bs, teammates[numteammates - i - 1]);
			}
			//
			break;
		}
	}
}

/*
==================
BotCTFOrders
==================
*/
void BotCTFOrders_FlagNotAtBase(bot_state_t *bs) {
	int numteammates, defenders, attackers, i;
	int teammates[MAX_CLIENTS];
	char name[MAX_NETNAME];

	numteammates = BotSortTeamMatesByBaseTravelTime(bs, teammates, sizeof(teammates));
	BotSortTeamMatesByCTFPreference(bs, teammates, numteammates);
	//agressive
	//different orders based on the number of team mates
	switch(bs->numteammates)
	{
		case 1: break;
		case 2:
		{
			//both will go for the enemy flag
			ClientName(teammates[0], name, sizeof(name));
			BotAI_BotInitialChat(bs, "cmd_getflag", name, NULL);
			BotSayTeamOrder(bs, teammates[0]);
			//
			ClientName(teammates[1], name, sizeof(name));
			BotAI_BotInitialChat(bs, "cmd_getflag", name, NULL);
			BotSayTeamOrder(bs, teammates[1]);
			break;
		}
		case 3:
		{
			//everyone go for the flag
			ClientName(teammates[0], name, sizeof(name));
			BotAI_BotInitialChat(bs, "cmd_getflag", name, NULL);
			BotSayTeamOrder(bs, teammates[0]);
			//
			ClientName(teammates[1], name, sizeof(name));
			BotAI_BotInitialChat(bs, "cmd_getflag", name, NULL);
			BotSayTeamOrder(bs, teammates[1]);
			//
			ClientName(teammates[2], name, sizeof(name));
			BotAI_BotInitialChat(bs, "cmd_getflag", name, NULL);
			BotSayTeamOrder(bs, teammates[2]);
			break;
		}
		default:
		{
			//keep some people near the base for when the flag is returned
			defenders = (int) (float) numteammates * 0.2 + 0.5;
			if (defenders > 1) defenders = 1;
			attackers = (int) (float) numteammates * 0.7 + 0.5;
			for (i = 0; i < defenders; i++) {
				//
				ClientName(teammates[i], name, sizeof(name));
				BotAI_BotInitialChat(bs, "cmd_defendbase", name, NULL);
				BotSayTeamOrder(bs, teammates[i]);
			}
			for (i = 0; i < attackers; i++) {
				//
				ClientName(teammates[numteammates - i - 1], name, sizeof(name));
				BotAI_BotInitialChat(bs, "cmd_getflag", name, NULL);
				BotSayTeamOrder(bs, teammates[numteammates - i - 1]);
			}
			//
			break;
		}
	}
}

/*
==================
BotCTFOrders
==================
*/
void BotCTFOrders_EnemyFlagNotAtBase(bot_state_t *bs) {
	int numteammates, defenders, attackers, i, other;
	int teammates[MAX_CLIENTS];
	char name[MAX_NETNAME], carriername[MAX_NETNAME];

	numteammates = BotSortTeamMatesByBaseTravelTime(bs, teammates, sizeof(teammates));
	BotSortTeamMatesByCTFPreference(bs, teammates, numteammates);
	//different orders based on the number of team mates
	switch(numteammates) {
		case 1: break;
		case 2:
		{
			//tell the one not carrying the flag to defend the base
			if (teammates[0] == bs->flagcarrier) other = teammates[1];
			else other = teammates[0];
			ClientName(other, name, sizeof(name));
			BotAI_BotInitialChat(bs, "cmd_defendbase", name, NULL);
			BotSayTeamOrder(bs, other);
			break;
		}
		case 3:
		{
			//tell the one closest to the base not carrying the flag to defend the base
			if (teammates[0] != bs->flagcarrier) other = teammates[0];
			else other = teammates[1];
			ClientName(other, name, sizeof(name));
			BotAI_BotInitialChat(bs, "cmd_defendbase", name, NULL);
			BotSayTeamOrder(bs, other);
			//tell the one furthest from the base not carrying the flag to accompany the flag carrier
			if (teammates[2] != bs->flagcarrier) other = teammates[2];
			else other = teammates[1];
			ClientName(other, name, sizeof(name));
			ClientName(bs->flagcarrier, carriername, sizeof(carriername));
			if (bs->flagcarrier == bs->client) {
				BotAI_BotInitialChat(bs, "cmd_accompanyme", name, NULL);
			}
			else {
				BotAI_BotInitialChat(bs, "cmd_accompany", name, carriername, NULL);
			}
			BotSayTeamOrder(bs, other);
			break;
		}
		default:
		{
			//40% will defend the base
			defenders = (int) (float) numteammates * 0.4 + 0.5;
			if (defenders > 1) defenders = 1;
			//50% accompanies the flag carrier
			attackers = (int) (float) numteammates * 0.5 + 0.5;
			for (i = 0; i < defenders; i++) {
				//
				if (teammates[i] == bs->flagcarrier) {
					continue;
				}
				ClientName(teammates[i], name, sizeof(name));
				BotAI_BotInitialChat(bs, "cmd_defendbase", name, NULL);
				BotSayTeamOrder(bs, teammates[i]);
			}
			ClientName(bs->flagcarrier, carriername, sizeof(carriername));
			for (i = 0; i < attackers; i++) {
				//
				if (teammates[numteammates - i - 1] == bs->flagcarrier) {
					continue;
				}
				//
				ClientName(teammates[numteammates - i - 1], name, sizeof(name));
				if (bs->flagcarrier == bs->client) {
					BotAI_BotInitialChat(bs, "cmd_accompanyme", name, NULL);
				}
				else {
					BotAI_BotInitialChat(bs, "cmd_accompany", name, carriername, NULL);
				}
				BotSayTeamOrder(bs, teammates[numteammates - i - 1]);
			}
			//
			break;
		}
	}
}


/*
==================
BotCTFOrders
==================
*/
void BotCTFOrders_BothFlagsAtBase(bot_state_t *bs) {
	int numteammates, defenders, attackers, i;
	int teammates[MAX_CLIENTS];
	char name[MAX_NETNAME];

	//sort team mates by travel time to base
	numteammates = BotSortTeamMatesByBaseTravelTime(bs, teammates, sizeof(teammates));
	//sort team mates by CTF preference
	BotSortTeamMatesByCTFPreference(bs, teammates, numteammates);
	//agressive
	//different orders based on the number of team mates
	switch(numteammates)
	{
		case 1: break;
		case 2:
		{
			//the one closest to the base will defend the base
			ClientName(teammates[0], name, sizeof(name));
			BotAI_BotInitialChat(bs, "cmd_defendbase", name, NULL);
			BotSayTeamOrder(bs, teammates[0]);
			//the other will get the flag
			ClientName(teammates[1], name, sizeof(name));
			BotAI_BotInitialChat(bs, "cmd_getflag", name, NULL);
			BotSayTeamOrder(bs, teammates[1]);
			break;
		}
		case 3:
		{
			//the one closest to the base will defend the base
			ClientName(teammates[0], name, sizeof(name));
			BotAI_BotInitialChat(bs, "cmd_defendbase", name, NULL);
			BotSayTeamOrder(bs, teammates[0]);
			//the others should go for the enemy flag
			ClientName(teammates[1], name, sizeof(name));
			BotAI_BotInitialChat(bs, "cmd_getflag", name, NULL);
			BotSayTeamOrder(bs, teammates[1]);
			//
			ClientName(teammates[2], name, sizeof(name));
			BotAI_BotInitialChat(bs, "cmd_getflag", name, NULL);
			BotSayTeamOrder(bs, teammates[2]);
			break;
		}
		default:
		{
			defenders = (int) (float) numteammates * 0.4 + 0.5;
			if (defenders > 1) defenders = 1;
			attackers = (int) (float) numteammates * 0.5 + 0.5;
			for (i = 0; i < defenders; i++) {
				//
				ClientName(teammates[i], name, sizeof(name));
				BotAI_BotInitialChat(bs, "cmd_defendbase", name, NULL);
				BotSayTeamOrder(bs, teammates[i]);
			}
			for (i = 0; i < attackers; i++) {
				//
				ClientName(teammates[numteammates - i - 1], name, sizeof(name));
				BotAI_BotInitialChat(bs, "cmd_getflag", name, NULL);
				BotSayTeamOrder(bs, teammates[numteammates - i - 1]);
			}
			//
			break;
		}
	}
}


/*
==================
BotTeamOrders
==================
*/
void BotTeamOrders(bot_state_t *bs) {
	//no teamplay orders at this time
}


/*
==================
BotTeamAI
==================
*/
void BotTeamAI(bot_state_t *bs) {
	int numteammates, flagstatus;
	char netname[MAX_NETNAME];

	if(!bs) return;

	//
	if (gametype != GT_TEAM && gametype != GT_CTF) return;
	//make sure we've got a valid team leader
	if (!BotValidTeamLeader(bs)) {
		//
		if (!bs->askteamleader_time && !bs->becometeamleader_time) {
			if (bs->entergame_time + 10 > trap_AAS_Time()) {
				bs->askteamleader_time = trap_AAS_Time() + 5 + random() * 10;
			}
			else {
				bs->becometeamleader_time = trap_AAS_Time() + 5 + random() * 10;
			}
		}
		if (bs->askteamleader_time && bs->askteamleader_time < trap_AAS_Time()) {
			//if asked for a team leader and no repsonse
			BotAI_BotInitialChat(bs, "whoisteamleader", NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			bs->askteamleader_time = 0;
			bs->becometeamleader_time = trap_AAS_Time() + 15 + random() * 10;
		}
		if (bs->becometeamleader_time && bs->becometeamleader_time < trap_AAS_Time()) {
			BotAI_BotInitialChat(bs, "iamteamleader", NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			ClientName(bs->client, netname, sizeof(netname));
			strncpy(bs->teamleader, netname, sizeof(bs->teamleader));
			bs->teamleader[sizeof(bs->teamleader)-1] = '\0';
			bs->becometeamleader_time = 0;
		}
		return;
	}
	bs->askteamleader_time = 0;
	bs->becometeamleader_time = 0;

	//return if this bot is NOT the team leader
	ClientName(bs->client, netname, sizeof(netname));
	if (Q_stricmp(netname, bs->teamleader) != 0) return;
	//
	//if the game starts OR a new player comes onto the team OR a player leaves the team
	//
	numteammates = BotNumTeamMates(bs);
	//give orders
	switch(gametype) {
		case GT_TEAM:
		{
			if (bs->numteammates != numteammates || bs->forceorders) {
				bs->teamgiveorders_time = trap_AAS_Time();
				bs->numteammates = numteammates;
				bs->forceorders = qfalse;
			}
			//if it's time to give orders
			if (bs->teamgiveorders_time < trap_AAS_Time() - 5) {
				BotTeamOrders(bs);
				//
				bs->teamgiveorders_time = 0;
			}
			break;
		}
		case GT_CTF:
		{
			//if the number of team mates changed or the flag status changed
			//or someone wants to know what to do
			if (bs->numteammates != numteammates || bs->flagstatuschanged || bs->forceorders) {
				bs->teamgiveorders_time = trap_AAS_Time();
				bs->numteammates = numteammates;
				bs->flagstatuschanged = qfalse;
				bs->forceorders = qfalse;
			}
			//if there were no flag captures the last 3 minutes
			if (bs->lastflagcapture_time < trap_AAS_Time() - 240) {
				bs->lastflagcapture_time = trap_AAS_Time();
				//randomly change the CTF strategy
				if (random() < 0.4) {
					bs->ctfstrategy ^= CTFS_PASSIVE;
					bs->teamgiveorders_time = trap_AAS_Time();
				}
			}
			//if it's time to give orders
			if (bs->teamgiveorders_time && bs->teamgiveorders_time < trap_AAS_Time() - 3) {
				//
				if (BotCTFTeam(bs) == CTF_TEAM_RED) flagstatus = bs->redflagstatus * 2 + bs->blueflagstatus;
				else flagstatus = bs->blueflagstatus * 2 + bs->redflagstatus;
				//
				switch(flagstatus) {
					case 0: BotCTFOrders_BothFlagsAtBase(bs); break;
					case 1: BotCTFOrders_EnemyFlagNotAtBase(bs); break;
					case 2: BotCTFOrders_FlagNotAtBase(bs); break;
					case 3: BotCTFOrders_BothFlagsNotAtBase(bs); break;
				}
				//
				bs->teamgiveorders_time = 0;
			}
			break;
		}
	}
}

