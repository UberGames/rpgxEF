// Copyright (C) 1999-2000 Id Software, Inc.
//

/*****************************************************************************
 * name:		ai_chat.c
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /StarTrek/Code-DM/game/ai_chat.c $
 * $Author: Mgummelt $
 * $Revision: 7 $
 * $Modtime: 3/09/01 11:52a $
 * $Date: 3/09/01 12:02p $
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
//
#include "chars.h"				//characteristics
#include "inv.h"				//indexes into the inventory
#include "syn.h"				//synonyms
#include "match.h"				//string matching types and vars
//
#include "g_syscalls.h"


/*
==================
BotNumActivePlayers
==================
*/
static int32_t BotNumActivePlayers(void) {
	char buf[MAX_INFO_STRING];
	int32_t i = 0;
	int32_t num = 0;
	static int32_t max_clients;

	if (max_clients == 0) {
		max_clients = trap_Cvar_VariableIntegerValue("sv_maxclients");
	}

	for (; i < max_clients && i < MAX_CLIENTS; i++) {
		trap_GetConfigstring(CS_PLAYERS + i, buf, sizeof(buf));

		//if no config string or no name
		if (strlen(buf) == 0 || strlen(Info_ValueForKey(buf, "n")) == 0) {
			continue;
		}

		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) {
			continue;
		}

		num++;
	}

	return num;
}

/*
==================
BotIsFirstInRankings
==================
*/
static int32_t BotIsFirstInRankings(bot_state_t* bs) {
	char buf[MAX_INFO_STRING];
	int32_t i = 0;
	int32_t score = 0;
	static int32_t max_clients;
	playerState_t ps;

	if (max_clients == 0) {
		max_clients = trap_Cvar_VariableIntegerValue("sv_maxclients");
	}

	score = bs->cur_ps.persistant[PERS_SCORE];
	for (; i < max_clients && i < MAX_CLIENTS; i++) {
		trap_GetConfigstring(CS_PLAYERS + i, buf, sizeof(buf));

		//if no config string or no name
		if (strlen(buf) == 0 || strlen(Info_ValueForKey(buf, "n")) == 0) {
			continue;
		}

		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) {
			continue;
		}

		AI_main_BotAIGetClientState(i, &ps);
		if (score < ps.persistant[PERS_SCORE]) {
			return qfalse;
		}
	}

	return qtrue;
}

/*
==================
BotIsLastInRankings
==================
*/
static int32_t BotIsLastInRankings(bot_state_t* bs) {
	char buf[MAX_INFO_STRING];
	int32_t i = 0;
	int32_t score = 0;
	static int32_t max_clients;
	playerState_t ps;

	if (max_clients == 0) {
		max_clients = trap_Cvar_VariableIntegerValue("sv_maxclients");
	}

	score = bs->cur_ps.persistant[PERS_SCORE];
	for (; i < max_clients && i < MAX_CLIENTS; i++) {
		trap_GetConfigstring(CS_PLAYERS + i, buf, sizeof(buf));

		//if no config string or no name
		if (strlen(buf) == 0 || strlen(Info_ValueForKey(buf, "n")) == 0) {
			continue;
		}

		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) {
			continue;
		}

		AI_main_BotAIGetClientState(i, &ps);
		if (score > ps.persistant[PERS_SCORE]) {
			return qfalse;
		}
	}

	return qtrue;
}

/*
==================
BotFirstClientInRankings
==================
*/
static char* BotFirstClientInRankings(void) {
	char buf[MAX_INFO_STRING];
	static char name[32];
	int32_t i = 0;
	int32_t bestscore = -999999;
	int32_t bestclient = 0;
	static int32_t max_clients;
	playerState_t ps;

	if (max_clients == 0) {
		max_clients = trap_Cvar_VariableIntegerValue("sv_maxclients");
	}

	for (; i < max_clients && i < MAX_CLIENTS; i++) {
		trap_GetConfigstring(CS_PLAYERS + i, buf, sizeof(buf));

		//if no config string or no name
		if (strlen(buf) == 0 || strlen(Info_ValueForKey(buf, "n")) == 0) {
			continue;
		}

		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) {
			continue;
		}

		AI_main_BotAIGetClientState(i, &ps);
		if (ps.persistant[PERS_SCORE] > bestscore) {
			bestscore = ps.persistant[PERS_SCORE];
			bestclient = i;
		}
	}
	EasyClientName(bestclient, name, 32);

	return name;
}

/*
==================
BotLastClientInRankings
==================
*/
static char* BotLastClientInRankings(void) {
	char buf[MAX_INFO_STRING];
	static char name[32];
	int32_t i = 0;
	int32_t worstscore = 999999;
	int32_t bestclient = 0;
	static int32_t max_clients;
	playerState_t ps;

	if (max_clients == 0) {
		max_clients = trap_Cvar_VariableIntegerValue("sv_maxclients");
	}

	for (; i < max_clients && i < MAX_CLIENTS; i++) {
		trap_GetConfigstring(CS_PLAYERS + i, buf, sizeof(buf));

		//if no config string or no name
		if (strlen(buf) == 0 || strlen(Info_ValueForKey(buf, "n")) == 0) {
			continue;
		}

		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) {
			continue;
		}

		AI_main_BotAIGetClientState(i, &ps);
		if (ps.persistant[PERS_SCORE] < worstscore) {
			worstscore = ps.persistant[PERS_SCORE];
			bestclient = i;
		}
	}
	EasyClientName(bestclient, name, 32);

	return name;
}

/*
==================
BotRandomOpponentName
==================
*/
static char* BotRandomOpponentName(bot_state_t* bs) {
	char buf[MAX_INFO_STRING];
	static char name[32];
	int32_t i = 0;
	int32_t count = 0;
	int32_t opponents[MAX_CLIENTS];
	int32_t numopponents = 0;
	static int32_t max_clients;

	if (max_clients == 0) {
		max_clients = trap_Cvar_VariableIntegerValue("sv_maxclients");
	}

	opponents[0] = 0;
	for (; i < max_clients && i < MAX_CLIENTS; i++) {
		if (i == bs->client) {
			continue;
		}

		trap_GetConfigstring(CS_PLAYERS + i, buf, sizeof(buf));

		//if no config string or no name
		if (strlen(buf) == 0 || strlen(Info_ValueForKey(buf, "n")) == 0) {
			continue;
		}

		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) {
			continue;
		}

		//skip team mates
		if (BotSameTeam(bs, i) != 0) {
			continue;
		}

		opponents[numopponents] = i;
		numopponents++;
	}

	count = random() * numopponents;

	for (i = 0; i < numopponents; i++) {
		count--;
		if (count <= 0) {
			EasyClientName(opponents[i], name, sizeof(name));
			return name;
		}
	}
	EasyClientName(opponents[0], name, sizeof(name));

	return name;
}

/*
==================
BotMapTitle
==================
*/

static char* BotMapTitle(void) {
	char info[1024];
	static char mapname[128];

	trap_GetServerinfo(info, sizeof(info));

	strncpy(mapname, Info_ValueForKey(info, "mapname"), sizeof(mapname)-1);
	mapname[sizeof(mapname)-1] = '\0';

	return mapname;
}


/*
==================
BotWeaponNameForMeansOfDeath
==================
*/

static char* BotWeaponNameForMeansOfDeath(int32_t mod) {
	switch (mod) {
		case MOD_PHASER:
		case MOD_PHASER_ALT:			return "Phaser";
		case MOD_CRIFLE:
		case MOD_CRIFLE_SPLASH:
		case MOD_CRIFLE_ALT:
		case MOD_CRIFLE_ALT_SPLASH:		return "Compression Rifle";
		case MOD_IMOD:
		case MOD_IMOD_ALT:				return "Infinity Modulator";
		case MOD_SCAVENGER:
		case MOD_SCAVENGER_ALT:
		case MOD_SCAVENGER_ALT_SPLASH:	return "Scavenger Rifle";
		case MOD_STASIS:
		case MOD_STASIS_ALT:			return "Stasis Weapon";
		case MOD_GRENADE:
		case MOD_GRENADE_SPLASH:
		case MOD_GRENADE_ALT_SPLASH:	return "Grenade Launcher";
		case MOD_TETRION:
		case MOD_TETRION_ALT:			return "Tetryon Disruptor";
		case MOD_DREADNOUGHT:
		case MOD_DREADNOUGHT_ALT:		return "Arc Welder";
		case MOD_QUANTUM:
		case MOD_QUANTUM_SPLASH:
		case MOD_QUANTUM_ALT:
		case MOD_QUANTUM_ALT_SPLASH:	return "Photon Burst Cannon";
		case MOD_KNOCKOUT:				return "Hypo";

		default: return "[unknown weapon]";
	}
}

/*
==================
BotRandomWeaponName
==================
*/
static char* BotRandomWeaponName(void) {
	int32_t rnd = random() * 8.9;

	switch (rnd) {
		case 0: return "Phaser";
		case 1: return "Compression Rifle";
		case 2: return "Infinity Modulator";
		case 3: return "Scavenger Rifle";
		case 4: return "Stasis Weapon";
		case 5: return "Grenade Launcher";
		case 6: return "Tetryon Disruptor";
		case 7: return "Dermal Regenerator";
		default: return "Photon Burst Cannon";
	}
}

/*
==================
BotVisibleEnemies
==================
*/
static int32_t BotVisibleEnemies(bot_state_t* bs) {
	double vis;
	int32_t i = 0;
	aas_entityinfo_t entinfo;

	for (; i < MAX_CLIENTS; i++) {

		if (i == bs->client) {
			continue;
		}

		AI_main_BotEntityInfo(i, &entinfo);

		if (entinfo.valid == 0) {
			continue;
		}

		//if the enemy isn't dead and the enemy isn't the bot self
		if (EntityIsDead(&entinfo) || entinfo.number == bs->entitynum) {
			continue;
		}

		//if the enemy is invisible and not shooting
		if (EntityIsInvisible(&entinfo) && !EntityIsShooting(&entinfo)) {
			continue;
		}

		//if on the same team
		if (BotSameTeam(bs, i) != 0) {
			continue;
		}

		//check if the enemy is visible
		vis = BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, i);
		if (vis > 0) {
			return qtrue;
		}
	}

	return qfalse;
}

/*
==================
BotValidChatPosition
==================
*/
int BotValidChatPosition(bot_state_t* bs) {
	vec3_t point = { 0, 0, 0 };
	vec3_t start = { 0, 0, 0 };
	vec3_t end = { 0, 0, 0 };
	vec3_t mins = { 0, 0, 0 };
	vec3_t maxs = { 0, 0, 0 };
	bsp_trace_t trace;

	//if the bot is dead all positions are valid
	if (BotIsDead(bs)) {
		return qtrue;
	}

	//never start chatting with a powerup
	if (bs->inventory[INVENTORY_QUAD] || bs->inventory[INVENTORY_HASTE] ||
		bs->inventory[INVENTORY_INVISIBILITY] || bs->inventory[INVENTORY_REGEN] ||
		bs->inventory[INVENTORY_FLIGHT]) {
		return qfalse;
	}

	//must be on the ground
	//if (bs->cur_ps.groundEntityNum != ENTITYNUM_NONE) return qfalse;

	//do not chat if in lava or slime
	VectorCopy(bs->origin, point);
	point[2] -= 24;
	if ((trap_PointContents(point, bs->entitynum) & (CONTENTS_LAVA | CONTENTS_SLIME)) != 0) {
		return qfalse;
	}

	//do not chat if under water
	VectorCopy(bs->origin, point);
	point[2] += 32;
	if ((trap_PointContents(point, bs->entitynum) & MASK_WATER) != 0) {
		return qfalse;
	}

	//must be standing on the world entity
	VectorCopy(bs->origin, start);
	VectorCopy(bs->origin, end);
	start[2] += 1;
	end[2] -= 10;
	trap_AAS_PresenceTypeBoundingBox(PRESENCE_CROUCH, mins, maxs);
	AI_main_BotAITrace(&trace, start, mins, maxs, end, bs->client, MASK_SOLID);

	if (trace.ent != ENTITYNUM_WORLD) {
		return qfalse;
	}

	//the bot is in a position where it can chat
	return qtrue;
}

/*
==================
BotChat_EnterGame
==================
*/
int32_t BotChat_EnterGame(bot_state_t* bs) {
	char name[32];
	double rnd = 0;

	if (bot_nochat.integer != 0) {
		return qfalse;
	}

	if (bs->lastchat_time > trap_AAS_Time() - 3) {
		return qfalse;
	}

	//don't chat in teamplay
	if (TeamPlayIsOn() != 0) {
		return qfalse;
	}

	if (bot_fastchat.integer == 0) {
		if (random() > rnd) {
			return qfalse;
		}
	}
	if (BotNumActivePlayers() <= 1) {
		return qfalse;
	}

	if (BotValidChatPosition(bs) == 0) {
		return qfalse;
	}
	AI_main_BotAIInitialChat(bs, "game_enter",
						 EasyClientName(bs->client, name, 32),	// 0
						 BotRandomOpponentName(bs),				// 1
						 "[invalid var]",						// 2
						 "[invalid var]",						// 3
						 BotMapTitle(),							// 4
						 NULL);
	bs->lastchat_time = trap_AAS_Time();
	bs->chatto = CHAT_ALL;

	return qtrue;
}

/*
==================
BotChat_ExitGame
==================
*/
int32_t BotChat_ExitGame(bot_state_t* bs) {
	char name[32];
	double rnd = 0;

	if (bot_nochat.integer != 0) {
		return qfalse;
	}

	if (bs->lastchat_time > trap_AAS_Time() - 3) {
		return qfalse;
	}

	//don't chat in teamplay
	if (TeamPlayIsOn() != 0) {
		return qfalse;
	}

	if (bot_fastchat.integer == 0) {
		if (random() > rnd) {
			return qfalse;
		}
	}

	if (BotNumActivePlayers() <= 1) {
		return qfalse;
	}

	AI_main_BotAIInitialChat(bs, "game_exit",
						 EasyClientName(bs->client, name, 32),	// 0
						 BotRandomOpponentName(bs),				// 1
						 "[invalid var]",						// 2
						 "[invalid var]",						// 3
						 BotMapTitle(),							// 4
						 NULL);
	bs->lastchat_time = trap_AAS_Time();
	bs->chatto = CHAT_ALL;

	return qtrue;
}

/*
==================
BotChat_StartLevel
==================
*/
int32_t BotChat_StartLevel(bot_state_t* bs) {
	char name[32];
	double rnd = 0;

	if (bot_nochat.integer != 0) {
		return qfalse;
	}

	if (BotIsObserver(bs)) {
		return qfalse;
	}

	if (bs->lastchat_time > trap_AAS_Time() - 3) {
		return qfalse;
	}

	//don't chat in teamplay
	if (TeamPlayIsOn() != 0) {
		return qfalse;
	}

	if (bot_fastchat.integer == 0) {
		if (random() > rnd) {
			return qfalse;
		}
	}

	if (BotNumActivePlayers() <= 1) {
		return qfalse;
	}
	AI_main_BotAIInitialChat(bs, "level_start",
						 EasyClientName(bs->client, name, 32),	// 0
						 NULL);
	bs->lastchat_time = trap_AAS_Time();
	bs->chatto = CHAT_ALL;

	return qtrue;
}

/*
==================
BotChat_EndLevel
==================
*/
int32_t BotChat_EndLevel(bot_state_t* bs) {
	char name[32];
	double rnd = 0;

	if (bot_nochat.integer != 0) {
		return qfalse;
	}

	if (BotIsObserver(bs)) {
		return qfalse;
	}

	if (bs->lastchat_time > trap_AAS_Time() - 3) {
		return qfalse;
	}

	//don't chat in teamplay
	if (TeamPlayIsOn() != 0) {
		return qfalse;
	}

	if (bot_fastchat.integer == 0) {
		if (random() > rnd) {
			return qfalse;
		}
	}

	if (BotNumActivePlayers() <= 1) {
		return qfalse;
	}

	if (BotIsFirstInRankings(bs) != 0) {
		AI_main_BotAIInitialChat(bs, "level_end_victory",
							 EasyClientName(bs->client, name, 32),	// 0
							 BotRandomOpponentName(bs),				// 1
							 "[invalid var]",						// 2
							 BotLastClientInRankings(),				// 3
							 BotMapTitle(),							// 4
							 NULL);
	} else if (BotIsLastInRankings(bs) != 0) {
		AI_main_BotAIInitialChat(bs, "level_end_lose",
							 EasyClientName(bs->client, name, 32),	// 0
							 BotRandomOpponentName(bs),				// 1
							 BotFirstClientInRankings(),				// 2
							 "[invalid var]",						// 3
							 BotMapTitle(),							// 4
							 NULL);
	} else {
		AI_main_BotAIInitialChat(bs, "level_end",
							 EasyClientName(bs->client, name, 32),	// 0
							 BotRandomOpponentName(bs),				// 1
							 BotFirstClientInRankings(),				// 2
							 BotLastClientInRankings(),				// 3
							 BotMapTitle(),							// 4
							 NULL);
	}
	bs->lastchat_time = trap_AAS_Time();
	bs->chatto = CHAT_ALL;

	return qtrue;
}

/*
==================
BotChat_Death
==================
*/
int32_t BotChat_Death(bot_state_t* bs) {
	char name[32];
	double rnd = 0;

	if (bot_nochat.integer != 0) {
		return qfalse;
	}

	if (bs->lastchat_time > trap_AAS_Time() - 3) {
		return qfalse;
	}

	//if fast chatting is off
	if (bot_fastchat.integer == 0) {
		if (random() > rnd) {
			return qfalse;
		}
	}

	if (BotNumActivePlayers() <= 1) {
		return qfalse;
	}

	if (bs->lastkilledby >= 0 && bs->lastkilledby < MAX_CLIENTS) {
		EasyClientName(bs->lastkilledby, name, 32);
	} else {
		strcpy(name, "[world]");
	}

	if (TeamPlayIsOn() != 0 && BotSameTeam(bs, bs->lastkilledby) != 0) {
		if (bs->lastkilledby == bs->client) {
			return qfalse;
		}

		AI_main_BotAIInitialChat(bs, "death_teammate", name, NULL);
		bs->chatto = CHAT_TEAM;
	} else {
		//don't chat in teamplay
		if (TeamPlayIsOn() != 0) {
			return qfalse;
		}

		if (bs->botdeathtype == MOD_WATER) {
			AI_main_BotAIInitialChat(bs, "death_drown", BotRandomOpponentName(bs), NULL);
		} else if (bs->botdeathtype == MOD_SLIME) {
			AI_main_BotAIInitialChat(bs, "death_slime", BotRandomOpponentName(bs), NULL);
		} else if (bs->botdeathtype == MOD_LAVA) {
			AI_main_BotAIInitialChat(bs, "death_lava", BotRandomOpponentName(bs), NULL);
		} else if (bs->botdeathtype == MOD_FALLING) {
			AI_main_BotAIInitialChat(bs, "death_cratered", BotRandomOpponentName(bs), NULL);
		} else if (bs->botsuicide || //all other suicides by own weapon
				   bs->botdeathtype == MOD_CRUSH ||
				   bs->botdeathtype == MOD_SUICIDE ||
				   bs->botdeathtype == MOD_RESPAWN ||
				   bs->botdeathtype == MOD_TARGET_LASER ||
				   bs->botdeathtype == MOD_TRIGGER_HURT ||
				   bs->botdeathtype == MOD_UNKNOWN ||
				   bs->botdeathtype == MOD_EXPLOSION)
				   AI_main_BotAIInitialChat(bs, "death_suicide", BotRandomOpponentName(bs), NULL);
		else if (bs->botdeathtype == MOD_TELEFRAG) {
			AI_main_BotAIInitialChat(bs, "death_telefrag", name, NULL);
		} else {
			if (random() < trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_CHAT_INSULT, 0, 1)) {
				AI_main_BotAIInitialChat(bs, "death_insult",
									 name,												// 0
									 BotWeaponNameForMeansOfDeath(bs->botdeathtype),		// 1
									 NULL);
			} else {
				AI_main_BotAIInitialChat(bs, "death_praise",
									 name,												// 0
									 BotWeaponNameForMeansOfDeath(bs->botdeathtype),		// 1
									 NULL);
			}
		}
		bs->chatto = CHAT_ALL;
	}
	bs->lastchat_time = trap_AAS_Time();

	return qtrue;
}

/*
==================
BotChat_Kill
==================
*/
int32_t BotChat_Kill(bot_state_t* bs) {
	char name[32];
	double rnd = 0;

	if (bot_nochat.integer != 0) {
		return qfalse;
	}

	if (bs->lastchat_time > trap_AAS_Time() - 3) {
		return qfalse;
	}

	//if fast chat is off
	if (bot_fastchat.integer == 0) {
		if (random() > rnd) {
			return qfalse;
		}
	}

	if (bs->lastkilledplayer == bs->client) {
		return qfalse;
	}

	if (BotNumActivePlayers() <= 1) {
		return qfalse;
	}

	if (BotValidChatPosition(bs) == 0) {
		return qfalse;
	}

	if (BotVisibleEnemies(bs) != 0) {
		return qfalse;
	}

	EasyClientName(bs->lastkilledplayer, name, 32);

	bs->chatto = CHAT_ALL;
	if (TeamPlayIsOn() != 0 && BotSameTeam(bs, bs->lastkilledplayer) != 0) {
		AI_main_BotAIInitialChat(bs, "kill_teammate", name, NULL);
		bs->chatto = CHAT_TEAM;
	} else {
		//don't chat in teamplay
		if (TeamPlayIsOn() != 0) {
			return qfalse;
		} else if (bs->enemydeathtype == MOD_TELEFRAG) {
			AI_main_BotAIInitialChat(bs, "kill_telefrag", name, NULL);
		} else if (random() < trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_CHAT_INSULT, 0, 1)) { //choose between insult and praise
			AI_main_BotAIInitialChat(bs, "kill_insult", name, NULL);
		} else {
			AI_main_BotAIInitialChat(bs, "kill_praise", name, NULL);
		}
	}
	bs->lastchat_time = trap_AAS_Time();

	return qtrue;
}

/*
==================
BotChat_EnemySuicide
==================
*/
int32_t BotChat_EnemySuicide(bot_state_t* bs) {
	char name[32];
	double rnd = 0;

	if (bot_nochat.integer != 0) {
		return qfalse;
	}

	if (bs->lastchat_time > trap_AAS_Time() - 3) {
		return qfalse;
	}

	if (BotNumActivePlayers() <= 1) {
		return qfalse;
	}

	//don't chat in teamplay
	if (TeamPlayIsOn() != 0) {
		return qfalse;
	}

	//if fast chat is off
	if (bot_fastchat.integer == 0) {
		if (random() > rnd) {
			return qfalse;
		}
	}
	if (BotValidChatPosition(bs) == 0) {
		return qfalse;
	}

	if (BotVisibleEnemies(bs) != 0) {
		return qfalse;
	}

	if (bs->enemy >= 0) {
		EasyClientName(bs->enemy, name, 32);
	} else {
		strcpy(name, "");
	}
	AI_main_BotAIInitialChat(bs, "enemy_suicide", name, NULL);
	bs->lastchat_time = trap_AAS_Time();
	bs->chatto = CHAT_ALL;

	return qtrue;
}

/*
==================
BotChat_HitTalking
==================
*/
int32_t BotChat_HitTalking(bot_state_t* bs) {
	char name[32];
	char* weap = NULL;
	int32_t lasthurt_client = 0;
	double rnd = 0;

	if (bot_nochat.integer != 0) {
		return qfalse;
	}

	if (bs->lastchat_time > trap_AAS_Time() - 3) {
		return qfalse;
	}

	if (BotNumActivePlayers() <= 1) {
		return qfalse;
	}

	lasthurt_client = g_entities[bs->client].client->lasthurt_client;
	if (lasthurt_client == 0) {
		return qfalse;
	}

	if (lasthurt_client == bs->client) {
		return qfalse;
	}

	if (lasthurt_client < 0 || lasthurt_client >= MAX_CLIENTS) {
		return qfalse;
	}

	//don't chat in teamplay
	if (TeamPlayIsOn() != 0) {
		return qfalse;
	}

	//if fast chat is off
	if (bot_fastchat.integer == 0) {
		if (random() > rnd * 0.5) {
			return qfalse;
		}
	}
	if (BotValidChatPosition(bs) == 0) {
		return qfalse;
	}

	ClientName(g_entities[bs->client].client->lasthurt_client, name, sizeof(name));
	weap = BotWeaponNameForMeansOfDeath(g_entities[bs->client].client->lasthurt_client);

	AI_main_BotAIInitialChat(bs, "hit_talking", name, weap, NULL);
	bs->lastchat_time = trap_AAS_Time();
	bs->chatto = CHAT_ALL;

	return qtrue;
}

/*
==================
BotChat_HitNoDeath
==================
*/
int32_t BotChat_HitNoDeath(bot_state_t* bs) {
	char name[32];
	char* weap = NULL;
	double rnd = 0;
	int32_t lasthurt_client = g_entities[bs->client].client->lasthurt_client;
	aas_entityinfo_t entinfo;

	if (lasthurt_client == 0) {
		return qfalse;
	}

	if (lasthurt_client == bs->client) {
		return qfalse;
	}

	if (lasthurt_client < 0 || lasthurt_client >= MAX_CLIENTS) {
		return qfalse;
	}

	if (bot_nochat.integer != 0) {
		return qfalse;
	}

	if (bs->lastchat_time > trap_AAS_Time() - 3) {
		return qfalse;
	}

	if (BotNumActivePlayers() <= 1) {
		return qfalse;
	}

	//don't chat in teamplay
	if (TeamPlayIsOn() != 0) {
		return qfalse;
	}

	//if fast chat is off
	if (bot_fastchat.integer == 0) {
		if (random() > rnd * 0.5) {
			return qfalse;
		}
	}

	if (BotValidChatPosition(bs) == 0) {
		return qfalse;
	}

	if (BotVisibleEnemies(bs) != 0) {
		return qfalse;
	}

	AI_main_BotEntityInfo(bs->enemy, &entinfo);
	if (EntityIsShooting(&entinfo)) {
		return qfalse;
	}

	ClientName(lasthurt_client, name, sizeof(name));
	weap = BotWeaponNameForMeansOfDeath(g_entities[bs->client].client->lasthurt_mod);

	AI_main_BotAIInitialChat(bs, "hit_nodeath", name, weap, NULL);
	bs->lastchat_time = trap_AAS_Time();
	bs->chatto = CHAT_ALL;

	return qtrue;
}

/*
==================
BotChat_HitNoKill
==================
*/
int32_t BotChat_HitNoKill(bot_state_t* bs) {
	char name[32];
	char* weap = NULL;
	double rnd = 0;
	aas_entityinfo_t entinfo;

	if (bot_nochat.integer != 0) {
		return qfalse;
	}

	if (bs->lastchat_time > trap_AAS_Time() - 3) {
		return qfalse;
	}

	if (BotNumActivePlayers() <= 1) {
		return qfalse;
	}

	//don't chat in teamplay
	if (TeamPlayIsOn() != 0) {
		return qfalse;
	}

	//if fast chat is off
	if (bot_fastchat.integer == 0) {
		if (random() > rnd * 0.5) {
			return qfalse;
		}
	}

	if (BotValidChatPosition(bs) == 0) {
		return qfalse;
	}

	if (BotVisibleEnemies(bs) != 0) {
		return qfalse;
	}

	AI_main_BotEntityInfo(bs->enemy, &entinfo);
	if (EntityIsShooting(&entinfo)) {
		return qfalse;
	}

	ClientName(bs->enemy, name, sizeof(name));
	weap = BotWeaponNameForMeansOfDeath(g_entities[bs->enemy].client->lasthurt_mod);

	AI_main_BotAIInitialChat(bs, "hit_nokill", name, weap, NULL);
	bs->lastchat_time = trap_AAS_Time();
	bs->chatto = CHAT_ALL;

	return qtrue;
}

/*
==================
BotChat_Random
==================
*/
int32_t BotChat_Random(bot_state_t* bs) {
	double rnd = 0;
	char name[32];

	if (bot_nochat.integer != 0) {
		return qfalse;
	}

	if (BotIsObserver(bs)) {
		return qfalse;
	}

	if (bs->lastchat_time > trap_AAS_Time() - 3) {
		return qfalse;
	}

	//don't chat in teamplay
	if (TeamPlayIsOn() != 0) {
		return qfalse;
	}

	//don't chat when doing something important :)
	if (bs->ltgtype == LTG_TEAMHELP || bs->ltgtype == LTG_TEAMACCOMPANY || bs->ltgtype == LTG_RUSHBASE) {
		return qfalse;
	}

	if (random() > bs->thinktime * 0.1) {
		return qfalse;
	}

	if (bot_fastchat.integer == 0) {
		if (random() > rnd) {
			return qfalse;
		}

		if (random() > 0.25) {
			return qfalse;
		}
	}

	if (BotNumActivePlayers() <= 1) {
		return qfalse;
	}

	if (BotValidChatPosition(bs) == 0) {
		return qfalse;
	}
	
	if (bs->lastkilledplayer == bs->client) {
		strcpy(name, BotRandomOpponentName(bs));
	} else {
		EasyClientName(bs->lastkilledplayer, name, sizeof(name));
	}
	
	if (random() < trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_CHAT_MISC, 0, 1)) {
		AI_main_BotAIInitialChat(bs, "random_misc",
							 BotRandomOpponentName(bs),	// 0
							 name,						// 1
							 "[invalid var]",			// 2
							 "[invalid var]",			// 3
							 BotMapTitle(),				// 4
							 BotRandomWeaponName(),		// 5
							 NULL);
	} else {
		AI_main_BotAIInitialChat(bs, "random_insult",
							 BotRandomOpponentName(bs),	// 0
							 name,						// 1
							 "[invalid var]",			// 2
							 "[invalid var]",			// 3
							 BotMapTitle(),				// 4
							 BotRandomWeaponName(),		// 5
							 NULL);
	}
	bs->lastchat_time = trap_AAS_Time();
	bs->chatto = CHAT_ALL;

	return qtrue;
}

/*
==================
BotChatTime
==================
*/
double BotChatTime(bot_state_t* bs) {
	return 4000;
}

/*
==================
BotChatTest
==================
*/
void BotChatTest(bot_state_t* bs) {
	char name[32];
	char* weap = NULL;
	int32_t i = 0;
	int num = trap_BotNumInitialChats(bs->cs, "game_enter");

	for (; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "game_enter",
							 EasyClientName(bs->client, name, 32),	// 0
							 BotRandomOpponentName(bs),				// 1
							 "[invalid var]",						// 2
							 "[invalid var]",						// 3
							 BotMapTitle(),							// 4
							 NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "game_exit");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "game_exit",
							 EasyClientName(bs->client, name, 32),	// 0
							 BotRandomOpponentName(bs),				// 1
							 "[invalid var]",						// 2
							 "[invalid var]",						// 3
							 BotMapTitle(),							// 4
							 NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "level_start");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "level_start",
							 EasyClientName(bs->client, name, 32),	// 0
							 NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "level_end_victory");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "level_end_victory",
							 EasyClientName(bs->client, name, 32),	// 0
							 BotRandomOpponentName(bs),				// 1
							 BotFirstClientInRankings(),				// 2
							 BotLastClientInRankings(),				// 3
							 BotMapTitle(),							// 4
							 NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "level_end_lose");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "level_end_lose",
							 EasyClientName(bs->client, name, 32),	// 0
							 BotRandomOpponentName(bs),				// 1
							 BotFirstClientInRankings(),				// 2
							 BotLastClientInRankings(),				// 3
							 BotMapTitle(),							// 4
							 NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "level_end");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "level_end",
							 EasyClientName(bs->client, name, 32),	// 0
							 BotRandomOpponentName(bs),				// 1
							 BotFirstClientInRankings(),				// 2
							 BotLastClientInRankings(),				// 3
							 BotMapTitle(),							// 4
							 NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	EasyClientName(bs->lastkilledby, name, sizeof(name));
	num = trap_BotNumInitialChats(bs->cs, "death_drown");
	for (i = 0; i < num; i++) {
		//
		AI_main_BotAIInitialChat(bs, "death_drown", name, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "death_slime");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "death_slime", name, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "death_lava");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "death_lava", name, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "death_cratered");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "death_cratered", name, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "death_suicide");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "death_suicide", name, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "death_telefrag");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "death_telefrag", name, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "death_gauntlet");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "death_gauntlet",
							 name,												// 0
							 BotWeaponNameForMeansOfDeath(bs->botdeathtype),		// 1
							 NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "death_rail");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "death_rail",
							 name,												// 0
							 BotWeaponNameForMeansOfDeath(bs->botdeathtype),		// 1
							 NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "death_bfg");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "death_bfg",
							 name,												// 0
							 BotWeaponNameForMeansOfDeath(bs->botdeathtype),		// 1
							 NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "death_insult");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "death_insult",
							 name,												// 0
							 BotWeaponNameForMeansOfDeath(bs->botdeathtype),		// 1
							 NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "death_praise");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "death_praise",
							 name,												// 0
							 BotWeaponNameForMeansOfDeath(bs->botdeathtype),		// 1
							 NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}
	
	EasyClientName(bs->lastkilledplayer, name, 32);
	
	num = trap_BotNumInitialChats(bs->cs, "kill_gauntlet");
	for (i = 0; i < num; i++) {
		//
		AI_main_BotAIInitialChat(bs, "kill_gauntlet", name, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "kill_rail");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "kill_rail", name, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "kill_telefrag");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "kill_telefrag", name, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "kill_insult");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "kill_insult", name, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "kill_praise");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "kill_praise", name, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "enemy_suicide");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "enemy_suicide", name, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	ClientName(g_entities[bs->client].client->lasthurt_client, name, sizeof(name));
	weap = BotWeaponNameForMeansOfDeath(g_entities[bs->client].client->lasthurt_client);
	num = trap_BotNumInitialChats(bs->cs, "hit_talking");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "hit_talking", name, weap, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "hit_nodeath");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "hit_nodeath", name, weap, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "hit_nokill");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "hit_nokill", name, weap, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}
	
	if (bs->lastkilledplayer == bs->client) {
		strcpy(name, BotRandomOpponentName(bs));
	} else {
		EasyClientName(bs->lastkilledplayer, name, sizeof(name));
	}
	
	num = trap_BotNumInitialChats(bs->cs, "random_misc");
	for (i = 0; i < num; i++) {
		//
		AI_main_BotAIInitialChat(bs, "random_misc",
							 BotRandomOpponentName(bs),	// 0
							 name,						// 1
							 "[invalid var]",			// 2
							 "[invalid var]",			// 3
							 BotMapTitle(),				// 4
							 BotRandomWeaponName(),		// 5
							 NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}

	num = trap_BotNumInitialChats(bs->cs, "random_insult");
	for (i = 0; i < num; i++) {
		AI_main_BotAIInitialChat(bs, "random_insult",
							 BotRandomOpponentName(bs),	// 0
							 name,						// 1
							 "[invalid var]",			// 2
							 "[invalid var]",			// 3
							 BotMapTitle(),				// 4
							 BotRandomWeaponName(),		// 5
							 NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_ALL);
	}
}
