// Copyright (C) 1999-2000 Id Software, Inc.
//
/*****************************************************************************
 * name:		ai_dmq3.c
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /StarTrek/Code-DM/game/ai_dmq3.c $
 * $Author: Mgummelt $ 
 * $Revision: 33 $
 * $Modtime: 4/04/01 5:01p $
 * $Date: 4/04/01 5:17p $
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
#include "ai_team.h"
//
#include "chars.h"				//characteristics
#include "inv.h"				//indexes into the inventory
#include "syn.h"				//synonyms
#include "match.h"				//string matching types and vars

#define IDEAL_ATTACKDIST			140
#define WEAPONINDEX_PHASER			2

#define MAX_WAYPOINTS		128
//
bot_waypoint_t botai_waypoints[MAX_WAYPOINTS];
bot_waypoint_t *botai_freewaypoints;

//NOTE: not using a cvars which can be updated because the game should be reloaded anyway
int gametype;		//game type
int maxclients;		//maximum number of clients

vmCvar_t bot_grapple;
vmCvar_t bot_rocketjump;
vmCvar_t bot_fastchat;
vmCvar_t bot_nochat;
vmCvar_t bot_testrchat;
vmCvar_t bot_challenge;

vec3_t lastteleport_origin;		//last teleport event origin
float lastteleport_time;		//last teleport event time
int max_bspmodelindex;			//maximum BSP model index

//CTF flag goals
bot_goal_t ctf_redflag;
bot_goal_t ctf_blueflag;

#ifdef CTF
/*
==================
BotCTFCarryingFlag
==================
*/
int BotCTFCarryingFlag(bot_state_t *bs) {
	if (gametype != GT_CTF) return CTF_FLAG_NONE;

	if (bs->inventory[INVENTORY_REDFLAG] > 0) return CTF_FLAG_RED;
	else if (bs->inventory[INVENTORY_BLUEFLAG] > 0) return CTF_FLAG_BLUE;
	return CTF_FLAG_NONE;
}

/*
==================
BotCTFTeam
==================
*/
int BotCTFTeam(bot_state_t *bs) {
	char info[1024];

	if (gametype != GT_CTF) return CTF_TEAM_NONE;
	if (bs->client < 0 || bs->client >= MAX_CLIENTS) {
		//BotAI_Print(PRT_ERROR, "BotCTFTeam: client out of range\n");
		return qfalse;
	}
	trap_GetConfigstring(CS_PLAYERS+bs->client, info, sizeof(info));
	//
	if (atoi(Info_ValueForKey(info, "t")) == TEAM_RED) return CTF_TEAM_RED;
	else if (atoi(Info_ValueForKey(info, "t")) == TEAM_BLUE) return CTF_TEAM_BLUE;
	return CTF_TEAM_NONE;
}

/*
==================
BotCTFRetreatGoals
==================
*/
void BotCTFRetreatGoals(bot_state_t *bs) {
	//when carrying a flag in ctf the bot should rush to the base
	if (BotCTFCarryingFlag(bs)) {
		//if not already rushing to the base
		if (bs->ltgtype != LTG_RUSHBASE) {
			bs->ltgtype = LTG_RUSHBASE;
			bs->teamgoal_time = trap_AAS_Time() + CTF_RUSHBASE_TIME;
			bs->rushbaseaway_time = 0;
		}
	}
}

/*
==================
EntityIsDead
==================
*/
qboolean EntityIsDead(aas_entityinfo_t *entinfo) {
	playerState_t ps;

	if (entinfo->number >= 0 && entinfo->number < MAX_CLIENTS) {
		//retrieve the current client state
		BotAI_GetClientState( entinfo->number, &ps );
		if (ps.pm_type != PM_NORMAL) return qtrue;
	}
	return qfalse;
}

/*
==================
EntityIsInvisible
==================
*/
qboolean EntityIsInvisible(aas_entityinfo_t *entinfo) {
	if (entinfo->powerups & (1 << PW_GHOST))
	{	// 50% chance of being visible?
		if (((unsigned int)(level.time)/1024)&0x01)		// Every second or so, the bot will see the player, so he doesn't jitter.
		{
			return qtrue;
		}
		else
		{
			return qfalse;
		}
	}
	else if (entinfo->powerups & (1 << PW_INVIS)) 
	{
		return qtrue;
	}
	else if ( entinfo->flags & EF_NODRAW ) 
	{
		return qtrue;
	}
	return qfalse;
}

/*
==================
EntityCarriesFlag
==================
*/
qboolean EntityCarriesFlag(aas_entityinfo_t *entinfo) {
	/*if ( entinfo->powerups & ( 1 << PW_REDFLAG ) ) return qtrue;*/
	//if ( entinfo->powerups & ( 1 << PW_BORG_ADAPT ) ) return qtrue;
	return qfalse;
}

/*
==================
EntityIsShooting
==================
*/
qboolean EntityIsShooting(aas_entityinfo_t *entinfo) {
	if (entinfo->flags & EF_FIRING) {
		return qtrue;
	}
	return qfalse;
}

/*
==================
EntityIsChatting
==================
*/
qboolean EntityIsChatting(aas_entityinfo_t *entinfo) {
	if (entinfo->flags & EF_TALK) {
		return qtrue;
	}
	return qfalse;
}

/*
==================
EntityHasQuad
==================
*/
qboolean EntityHasQuad(aas_entityinfo_t *entinfo) {
	if (entinfo->powerups & (1 << PW_QUAD)) {
		return qtrue;
	}
	return qfalse;
}

/*
==================
BotCTFSeekGoals
==================
*/
void BotCTFSeekGoals(bot_state_t *bs) {
	float rnd;
	int flagstatus, c;

	//when carrying a flag in ctf the bot should rush to the base
	if (BotCTFCarryingFlag(bs)) {
		//if not already rushing to the base
		if (bs->ltgtype != LTG_RUSHBASE) {
			bs->ltgtype = LTG_RUSHBASE;
			bs->teamgoal_time = trap_AAS_Time() + CTF_RUSHBASE_TIME;
			bs->rushbaseaway_time = 0;
		}
		else if (bs->rushbaseaway_time > trap_AAS_Time()) {
			if (BotCTFTeam(bs) == CTF_TEAM_RED) flagstatus = bs->redflagstatus;
			else flagstatus = bs->blueflagstatus;
			//if the flag is back
			if (flagstatus == 0) {
				bs->rushbaseaway_time = 0;
			}
		}
		return;
	}
	//
	if (BotCTFTeam(bs) == CTF_TEAM_RED) flagstatus = bs->redflagstatus * 2 + bs->blueflagstatus;
	else flagstatus = bs->blueflagstatus * 2 + bs->redflagstatus;
	//if the enemy flag is not at it's base
	if (flagstatus == 1) {
		//if Not defending the base
		if (!(bs->ltgtype == LTG_DEFENDKEYAREA &&
			(bs->teamgoal.number == ctf_redflag.number ||
			bs->teamgoal.number == ctf_blueflag.number))) {
			//if not already accompanying someone
			if (bs->ltgtype != LTG_TEAMACCOMPANY) {
				//if there is avisible team mate flag carrier
				c = BotTeamFlagCarrierVisible(bs);
				if (c >= 0) {
					//follow the flag carrier
					//the team mate
					bs->teammate = c;
					//last time the team mate was visible
					bs->teammatevisible_time = trap_AAS_Time();
					//set the time to send a message to the team mates
					bs->teammessage_time = trap_AAS_Time() + 2 * random();
					//get the team goal time
					bs->teamgoal_time = trap_AAS_Time() + TEAM_ACCOMPANY_TIME;
					bs->ltgtype = LTG_TEAMACCOMPANY;
					bs->formation_dist = 3.5 * 32;		//3.5 meter
					bs->arrive_time = 0;
					return;
				}
			}
		}
	}
	//if the base flag is stolen
	else if (flagstatus == 2) {
		//if not already going for the enemy flag
		if (bs->ltgtype != LTG_GETFLAG) {
			//if there's no bot team leader
			if (!BotTeamLeader(bs)) {
				//go for the enemy flag
				bs->ltgtype = LTG_GETFLAG;
				//no team message
				bs->teammessage_time = 1;
				//set the time the bot will stop getting the flag
				bs->teamgoal_time = trap_AAS_Time() + CTF_GETFLAG_TIME;
				return;
			}
		}
	}
	//if both flags not at their bases
	else if (flagstatus == 3) {
		//
		if (bs->ltgtype != LTG_GETFLAG &&
			bs->ltgtype != LTG_TEAMACCOMPANY) {
			//if there is avisible team mate flag carrier
			c = BotTeamFlagCarrierVisible(bs);
			if (c >= 0) {
				//follow the flag carrier
				return;
			}
			else {
				//otherwise attack the enemy base
			}
			return;
		}
	}
	//if the bot is roaming
	if (bs->ctfroam_time > trap_AAS_Time()) return;
	//if already a CTF or team goal
	if (bs->ltgtype == LTG_TEAMHELP ||
			bs->ltgtype == LTG_TEAMACCOMPANY ||
			bs->ltgtype == LTG_DEFENDKEYAREA ||
			bs->ltgtype == LTG_GETFLAG ||
			bs->ltgtype == LTG_RUSHBASE ||
			bs->ltgtype == LTG_RETURNFLAG ||
			bs->ltgtype == LTG_CAMPORDER ||
			bs->ltgtype == LTG_PATROL) {
		return;
	}
	//if the bot has anough aggression to decide what to do
	if (BotAggression(bs) < 50) return;
	//set the time to send a message to the team mates
	bs->teammessage_time = trap_AAS_Time() + 2 * random();
	//get the flag or defend the base
	rnd = random();
	if (rnd < 0.33 && ctf_redflag.areanum && ctf_blueflag.areanum) {
		bs->ltgtype = LTG_GETFLAG;
		//set the time the bot will stop getting the flag
		bs->teamgoal_time = trap_AAS_Time() + CTF_GETFLAG_TIME;
	}
	else if (rnd < 0.66 && ctf_redflag.areanum && ctf_blueflag.areanum) {
		//
		if (BotCTFTeam(bs) == CTF_TEAM_RED) memcpy(&bs->teamgoal, &ctf_redflag, sizeof(bot_goal_t));
		else memcpy(&bs->teamgoal, &ctf_blueflag, sizeof(bot_goal_t));
		//set the ltg type
		bs->ltgtype = LTG_DEFENDKEYAREA;
		//set the time the bot stops defending the base
		bs->teamgoal_time = trap_AAS_Time() + TEAM_DEFENDKEYAREA_TIME;
		bs->defendaway_time = 0;
	}
	else {
		bs->ltgtype = 0;
		//set the time the bot will stop roaming
		bs->ctfroam_time = trap_AAS_Time() + CTF_ROAM_TIME;
	}
#ifdef DEBUG
	BotPrintTeamGoal(bs);
#endif //DEBUG
}

#endif //CTF

/*
==================
BotPointAreaNum
==================
*/
int BotPointAreaNum(vec3_t origin) {
	int areanum, numareas, areas[10];
	vec3_t end;

	areanum = trap_AAS_PointAreaNum(origin);
	if (areanum) return areanum;
	VectorCopy(origin, end);
	end[2] += 10;
	numareas = trap_AAS_TraceAreas(origin, end, areas, NULL, 10);
	if (numareas > 0) return areas[0];
	return 0;
}

/*
==================
ClientName
==================
*/
char *ClientName(int client, char *name, int size) {
	char buf[MAX_INFO_STRING];

	if (client < 0 || client >= MAX_CLIENTS) {
		BotAI_Print(PRT_ERROR, "ClientName: client out of range\n");
		return "[client out of range]";
	}
	trap_GetConfigstring(CS_PLAYERS+client, buf, sizeof(buf));
	strncpy(name, Info_ValueForKey(buf, "n"), size-1);
	name[size-1] = '\0';
	Q_CleanStr( name );
	return name;
}

/*
==================
ClientSkin
==================
*/
char *ClientSkin(int client, char *skin, int size) {
	char buf[MAX_INFO_STRING];

	if (client < 0 || client >= MAX_CLIENTS) {
		BotAI_Print(PRT_ERROR, "ClientSkin: client out of range\n");
		return "[client out of range]";
	}
	trap_GetConfigstring(CS_PLAYERS+client, buf, sizeof(buf));
	strncpy(skin, Info_ValueForKey(buf, "model"), size-1);
	skin[size-1] = '\0';
	return skin;
}

/*
==================
ClientFromName
==================
*/
int ClientFromName(char *name) {
	int i;
	char buf[MAX_INFO_STRING];
	static int maxclis;

	if (!maxclis)
		maxclis = trap_Cvar_VariableIntegerValue("sv_maxclients");
	for (i = 0; i < maxclis && i < MAX_CLIENTS; i++) {
		trap_GetConfigstring(CS_PLAYERS+i, buf, sizeof(buf));
		Q_CleanStr( buf );
		if (!Q_stricmp(Info_ValueForKey(buf, "n"), name)) return i;
	}
	return -1;
}

/*
==================
stristr
==================
*/
char *stristr(char *str, char *charset) {
	int i;

	while(*str) {
		for (i = 0; charset[i] && str[i]; i++) {
			if (toupper(charset[i]) != toupper(str[i])) break;
		}
		if (!charset[i]) return str;
		str++;
	}
	return NULL;
}

/*
==================
EasyClientName
==================
*/
char *EasyClientName(int client, char *buf, int size) {
	int i;
	char *str1, *str2, *ptr, c;
	char name[128];

	strcpy(name, ClientName(client, name, sizeof(name)));
	for (i = 0; name[i]; i++) name[i] &= 127;
	//remove all spaces
	for (ptr = strstr(name, " "); ptr; ptr = strstr(name, " ")) {
		memmove(ptr, ptr+1, strlen(ptr+1)+1);
	}
	//check for [x] and ]x[ clan names
	str1 = strstr(name, "[");
	str2 = strstr(name, "]");
	if (str1 && str2) {
		if (str2 > str1) memmove(str1, str2+1, strlen(str2+1)+1);
		else memmove(str2, str1+1, strlen(str1+1)+1);
	}
	//remove Mr prefix
	if ((name[0] == 'm' || name[0] == 'M') &&
			(name[1] == 'r' || name[1] == 'R')) {
		memmove(name, name+2, strlen(name+2)+1);
	}
	//only allow lower case alphabet characters
	ptr = name;
	while(*ptr) {
		c = *ptr;
		if ((c >= 'a' && c <= 'z') ||
				(c >= '0' && c <= '9') || c == '_') {
			ptr++;
		}
		else if (c >= 'A' && c <= 'Z') {
			*ptr += 'a' - 'A';
			ptr++;
		}
		else {
			memmove(ptr, ptr+1, strlen(ptr + 1)+1);
		}
	}
	strncpy(buf, name, size-1);
	buf[size-1] = '\0';
	return buf;
}

qboolean BotUseMeleeWeapon(bot_state_t *bs) {
	if ( bs->inventory[ENEMY_HORIZONTAL_DIST] < 64 ) 
	{
		/*if ( bs->cur_ps.persistant[PERS_CLASS] == PC_BORG || bs->cur_ps.persistant[PERS_CLASS] == PC_MEDIC )
		{
			return qtrue;
		}*/
	}
	return qfalse;
}
/*
==================
BotChooseWeapon

  MCG - FIXME: This should really take into account:
	Projectile vs. instant?
	gravity on projectile?
	Range to enemy vs range of weapon?
	Some randomness on the weights?

==================
*/
void BotChooseWeapon(bot_state_t *bs) {
	int newweaponnum;

	if (bs->cur_ps.weaponstate == WEAPON_RAISING || bs->cur_ps.weaponstate == WEAPON_DROPPING)
	{
		trap_EA_SelectWeapon(bs->client, bs->weaponnum);
	}
	else
	{
		newweaponnum = trap_BotChooseBestFightWeapon(bs->ws, bs->inventory, BotUseMeleeWeapon(bs));
		if (bs->weaponnum != newweaponnum)
		{
			bs->weaponchange_time = trap_AAS_Time();
		}
		bs->weaponnum = newweaponnum;
		//BotAI_Print(PRT_MESSAGE, "bs->weaponnum = %d\n", bs->weaponnum);
		trap_EA_SelectWeapon(bs->client, bs->weaponnum);
	}
}

/*
==================
BotSetupForMovement
==================
*/
void BotSetupForMovement(bot_state_t *bs) {
	bot_initmove_t initmove;

	memset(&initmove, 0, sizeof(bot_initmove_t));
	VectorCopy(bs->cur_ps.origin, initmove.origin);
	VectorCopy(bs->cur_ps.velocity, initmove.velocity);
	VectorCopy(bs->cur_ps.origin, initmove.viewoffset);
	initmove.viewoffset[2] += bs->cur_ps.viewheight;
	initmove.entitynum = bs->entitynum;
	initmove.client = bs->client;
	initmove.thinktime = bs->thinktime;
	//set the onground flag
	if (bs->cur_ps.groundEntityNum != ENTITYNUM_NONE) initmove.or_moveflags |= MFL_ONGROUND;
	//set the teleported flag
	if ((bs->cur_ps.pm_flags & PMF_TIME_KNOCKBACK) && (bs->cur_ps.pm_time > 0)) {
		initmove.or_moveflags |= MFL_TELEPORTED;
	}
	//set the waterjump flag
	if ((bs->cur_ps.pm_flags & PMF_TIME_WATERJUMP) && (bs->cur_ps.pm_time > 0)) {
		initmove.or_moveflags |= MFL_WATERJUMP;
	}
	//set presence type
	if (bs->cur_ps.pm_flags & PMF_DUCKED) initmove.presencetype = PRESENCE_CROUCH;
	else initmove.presencetype = PRESENCE_NORMAL;
	//
	if (bs->walker > 0.5) initmove.or_moveflags |= MFL_WALK;
	//
	VectorCopy(bs->viewangles, initmove.viewangles);
	//
	trap_BotInitMoveState(bs->ms, &initmove);
}

/*
==================
BotUpdateInventory
==================
*/
void BotUpdateInventory(bot_state_t *bs) {
	//armor
	bs->inventory[INVENTORY_ARMOR] = bs->cur_ps.stats[STAT_ARMOR];

	//weapons
	bs->inventory[INVENTORY_GRENADELAUNCHER] = (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_8)) != 0;
	bs->inventory[INVENTORY_STASIS] = (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_10)) != 0;
	bs->inventory[INVENTORY_PHASER] = (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_5)) != 0;
	bs->inventory[INVENTORY_DREADNOUGHT] = (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_13)) != 0;
	bs->inventory[INVENTORY_IMOD] = (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_1)) != 0;
	bs->inventory[INVENTORY_COMPRESSION] = (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_6)) != 0;
	bs->inventory[INVENTORY_TETRION] = (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_7)) != 0;
	bs->inventory[INVENTORY_SCAVENGER] = (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_4)) != 0;
	bs->inventory[INVENTORY_QUANTUM] = (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_9)) != 0;

	//ammo
	bs->inventory[INVENTORY_GRENADES] = bs->cur_ps.ammo[WP_8];
	bs->inventory[INVENTORY_STASISAMMO] = bs->cur_ps.ammo[WP_10];
	bs->inventory[INVENTORY_PHASERAMMO] = bs->cur_ps.ammo[WP_5];
	bs->inventory[INVENTORY_DREADNOUGHTAMMO] = bs->cur_ps.ammo[WP_13];
	bs->inventory[INVENTORY_IMODAMMO] = bs->cur_ps.ammo[WP_1];
	bs->inventory[INVENTORY_COMPRESSIONAMMO] = bs->cur_ps.ammo[WP_6];
	bs->inventory[INVENTORY_TETRIONAMMO] = bs->cur_ps.ammo[WP_7];
	bs->inventory[INVENTORY_SCAVENGERAMMO] = bs->cur_ps.ammo[WP_4];
	bs->inventory[INVENTORY_QUANTUMAMMO] = bs->cur_ps.ammo[WP_9];

	//powerups
	bs->inventory[INVENTORY_HEALTH] = bs->cur_ps.stats[STAT_HEALTH];
	bs->inventory[INVENTORY_TRANSPORTER] = bs->cur_ps.stats[STAT_HOLDABLE_ITEM] == MODELINDEX_TELEPORTER;
	bs->inventory[INVENTORY_MEDKIT] = bs->cur_ps.stats[STAT_HOLDABLE_ITEM] == MODELINDEX_MEDKIT;
	bs->inventory[INVENTORY_QUAD] = bs->cur_ps.powerups[PW_QUAD] != 0;
	bs->inventory[INVENTORY_ENVIRONMENTSUIT] = bs->cur_ps.powerups[PW_BOLTON] != 0;
	bs->inventory[INVENTORY_HASTE] = bs->cur_ps.powerups[PW_HASTE] != 0;
	bs->inventory[INVENTORY_INVISIBILITY] = bs->cur_ps.powerups[PW_INVIS] != 0;
	bs->inventory[INVENTORY_REGEN] = bs->cur_ps.powerups[PW_LASER] != 0;
	bs->inventory[INVENTORY_FLIGHT] = bs->cur_ps.powerups[PW_FLIGHT] != 0;
	//bs->inventory[INVENTORY_REDFLAG] = bs->cur_ps.powerups[PW_REDFLAG] != 0;
	//bs->inventory[INVENTORY_BLUEFLAG] = bs->cur_ps.powerups[PW_REDFLAG] != 0;
	bs->inventory[INVENTORY_SEEKER] = bs->cur_ps.powerups[PW_FLASHLIGHT] != 0;
	bs->inventory[INVENTORY_SHIELD] = bs->cur_ps.stats[STAT_HOLDABLE_ITEM] == MODELINDEX_SHIELD;
	bs->inventory[INVENTORY_DETPACK] = bs->cur_ps.stats[STAT_HOLDABLE_ITEM] == MODELINDEX_DETPACK;
	//
}

/*
==================
BotUpdateBattleInventory
==================
*/
void BotUpdateBattleInventory(bot_state_t *bs, int enemy) {
	vec3_t dir;
	aas_entityinfo_t entinfo;

	BotEntityInfo(enemy, &entinfo);
	VectorSubtract(entinfo.origin, bs->origin, dir); 
	bs->inventory[ENEMY_HEIGHT] = (int) dir[2];
	dir[2] = 0;
	bs->inventory[ENEMY_HORIZONTAL_DIST] = (int) VectorLength(dir);
	//FIXME: add num visible enemies and num visible team mates to the inventory
}

/*
=========================
BotShouldDetonateDetPack
=========================
*/
#define DETPACK_RADIUS		500

qboolean BotShouldDetonateDetPack(bot_state_t *bs)
{
	int		botNum = 0, detWeight = 0;
	vec3_t	packOrg, dir;
	float	dist;
	aas_entityinfo_t botinfo;

	// find the location of the DetPack
	gentity_t	*detpack = NULL;
	char		*classname = BG_FindClassnameForHoldable(HI_DETPACK);

	if (!classname)
	{
		return qfalse;
	}

	//while ((detpack = G_Find (detpack, FOFS(classname), classname)) != NULL)
	while ((detpack = G_Find (detpack, FOFS(classname), classname)) != NULL)
	{
		VectorCopy(detpack->r.currentOrigin, packOrg);
	}
	
	// determine who would be killed in the blast radius
	for (botNum = 0; botNum < MAX_CLIENTS; botNum++)
	{
		BotEntityInfo(botNum, &botinfo);
		if (!botinfo.valid) continue;

		//calculate the distance towards the enemy
		VectorSubtract(botinfo.origin, packOrg, dir);
		dist = VectorLength(dir);

		if (dist < DETPACK_RADIUS) // bot would get caught in blast radius
		{
			if (BotSameTeam(bs, botNum)) // friendly casualty potential
			{ 
				if (botNum == bs->client) // suicide...  bad
				{
					detWeight--;
				}
				if (EntityCarriesFlag(&botinfo)) // it's my teammate, and he's got the flag!
				{
					detWeight -= 11;
					continue;
				}
				detWeight--;
			}
			else
			{
				if(EntityCarriesFlag(&botinfo)) // mwahaha
				{
					detWeight += 14;
				}
				detWeight++;
			}
		}	
	}

//	Com_Printf("detWeight %d\n", detWeight);
	
	if (detWeight > 0)
	{
		return qtrue;
	}
	return qfalse;
}



/*
==================
BotBattleUseItems
==================
*/
void BotBattleUseItems(bot_state_t *bs) {


	if (bs->inventory[INVENTORY_DETPACK] > 0)
	{
		//  this needs to be in two stages: placement and detonation
		if (bs->ltgtype == LTG_DEFENDKEYAREA)
		{
			if (bs->inventory[INVENTORY_DETPACK_PLACED] == 0) // not placed yet
			{
				bs->inventory[INVENTORY_DETPACK_PLACED] = 1;
				trap_EA_Use(bs->client); // place it
				return;
			}
		}

		if (bs->inventory[INVENTORY_DETPACK_PLACED] == 1) //  placed 
		{
			if (BotShouldDetonateDetPack(bs)) // logic
			{
				bs->inventory[INVENTORY_DETPACK_PLACED] = 0;
				trap_EA_Use(bs->client); // BOOM
				return;
			}
			return;
		}
		return;
	}

	if (bs->inventory[INVENTORY_SHIELD] > 0)
	{
		if (BotWantsToRetreat(bs) && (bs->inventory[INVENTORY_HEALTH] < 50))
		{
			trap_EA_Use(bs->client);
		}
		return;
	}

	if (bs->inventory[INVENTORY_TRANSPORTER] > 0)
	{
		if (!BotCTFCarryingFlag(bs) && (bs->inventory[INVENTORY_HEALTH] < 50))
		{
			trap_EA_Use(bs->client);
			return;
		}
	}

	if (bs->inventory[INVENTORY_MEDKIT] > 0)
	{
		if (bs->inventory[INVENTORY_HEALTH] < 30)
		{
			trap_EA_Use(bs->client);
		}
		return;
	}
}

/*
==================
BotSetTeleportTime
==================
*/
void BotSetTeleportTime(bot_state_t *bs) {
	if ((bs->cur_ps.eFlags ^ bs->last_eFlags) & EF_TELEPORT_BIT) {
		bs->teleport_time = trap_AAS_Time();
	}
	bs->last_eFlags = bs->cur_ps.eFlags;
}

/*
==================
BotIsDead
==================
*/
qboolean BotIsDead(bot_state_t *bs) {
	return (bs->cur_ps.pm_type == PM_DEAD);
}

/*
==================
BotIsObserver
==================
*/
qboolean BotIsObserver(bot_state_t *bs) {
	char buf[MAX_INFO_STRING];
	if (bs->cur_ps.pm_type == PM_SPECTATOR) return qtrue;
	trap_GetConfigstring(CS_PLAYERS+bs->client, buf, sizeof(buf));
	if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) return qtrue;
	return qfalse;
}

/*
==================
BotIntermission
==================
*/
qboolean BotIntermission(bot_state_t *bs) {
	//NOTE: we shouldn't be looking at the game code...
	if (level.intermissiontime) return qtrue;
	return (bs->cur_ps.pm_type == PM_FREEZE || bs->cur_ps.pm_type == PM_INTERMISSION);
}

/*
==================
BotInLavaOrSlime
==================
*/
qboolean BotInLavaOrSlime(bot_state_t *bs) {
	vec3_t feet;

	VectorCopy(bs->origin, feet);
	feet[2] -= 23;
	return (trap_AAS_PointContents(feet) & (CONTENTS_LAVA|CONTENTS_SLIME));
}

/*
==================
BotCreateWayPoint
==================
*/
bot_waypoint_t *BotCreateWayPoint(char *name, vec3_t origin, int areanum) {
	bot_waypoint_t *wp;
	vec3_t waypointmins = {-8, -8, -8}, waypointmaxs = {8, 8, 8};

	wp = botai_freewaypoints;
	if ( !wp ) {
		BotAI_Print( PRT_WARNING, "BotCreateWayPoint: Out of waypoints\n" );
		return NULL;
	}
	botai_freewaypoints = botai_freewaypoints->next;

	Q_strncpyz( wp->name, name, sizeof(wp->name) );
	VectorCopy(origin, wp->goal.origin);
	VectorCopy(waypointmins, wp->goal.mins);
	VectorCopy(waypointmaxs, wp->goal.maxs);
	wp->goal.areanum = areanum;
	wp->next = NULL;
	wp->prev = NULL;
	return wp;
}

/*
==================
BotFindWayPoint
==================
*/
bot_waypoint_t *BotFindWayPoint(bot_waypoint_t *waypoints, char *name) {
	bot_waypoint_t *wp;

	for (wp = waypoints; wp; wp = wp->next) {
		if (!Q_stricmp(wp->name, name)) return wp;
	}
	return NULL;
}

/*
==================
BotFreeWaypoints
==================
*/
void BotFreeWaypoints(bot_waypoint_t *wp) {
	bot_waypoint_t *nextwp;

	for (; wp; wp = nextwp) {
		nextwp = wp->next;
		wp->next = botai_freewaypoints;
		botai_freewaypoints = wp;
	}
}

/*
==================
BotInitWaypoints
==================
*/
void BotInitWaypoints(void) {
	int i;

	botai_freewaypoints = NULL;
	for (i = 0; i < MAX_WAYPOINTS; i++) {
		botai_waypoints[i].next = botai_freewaypoints;
		botai_freewaypoints = &botai_waypoints[i];
	}
}

/*
==================
TeamPlayIsOn
==================
*/
int TeamPlayIsOn(void) {
	return ( gametype == GT_TEAM || gametype == GT_CTF );
}

/*
==================
BotAggression
==================
*/
float BotAggression(bot_state_t *bs) {
	//if the bot has quad
	if (bs->inventory[INVENTORY_QUAD]) {
		//if the bot is not holding the gauntlet or the enemy is really nearby
		if (bs->inventory[ENEMY_HORIZONTAL_DIST] < 80) {
			return 70;
		}
	}
	//if the enemy is located way higher than the bot
	if (bs->inventory[ENEMY_HEIGHT] > 200) return 0;
	//if the bot is very low on health
	if (bs->inventory[INVENTORY_HEALTH] < 60) return 0;
	//if the bot is low on health
	if (bs->inventory[INVENTORY_HEALTH] < 80) {
		//if the bot has insufficient armor
		if (bs->inventory[INVENTORY_ARMOR] < 40) return 0;
	}

	if (bs->inventory[INVENTORY_DREADNOUGHT] > 0 &&
			bs->inventory[INVENTORY_DREADNOUGHTAMMO] > 0) return 100;

	if (bs->inventory[INVENTORY_TETRION] > 0 &&
			bs->inventory[INVENTORY_TETRIONAMMO] > 0) return 95;

	if (bs->inventory[INVENTORY_QUANTUM] > 0 &&
			bs->inventory[INVENTORY_QUANTUMAMMO] > 0) return 90;

	if (bs->inventory[INVENTORY_STASIS] > 0 &&
			bs->inventory[INVENTORY_STASISAMMO] > 0) return 85;

	if (bs->inventory[INVENTORY_SCAVENGER] > 0 &&
			bs->inventory[INVENTORY_SCAVENGERAMMO] > 0) return 80;

	if (bs->inventory[INVENTORY_GRENADELAUNCHER] > 0 &&
			bs->inventory[INVENTORY_GRENADES] > 0) return 75;

	if (bs->inventory[INVENTORY_IMOD] > 0 &&
			bs->inventory[INVENTORY_IMODAMMO] > 0) return 70;

	if (bs->inventory[INVENTORY_COMPRESSION] > 0 &&
			bs->inventory[INVENTORY_COMPRESSIONAMMO] > 0) return 65;

	//otherwise the bot is not feeling too aggressive
	return 0;
}

/*
==================
BotWantsToRetreat
==================
*/
int BotWantsToRetreat(bot_state_t *bs) {
	aas_entityinfo_t entinfo;

	//always retreat when carrying a CTF flag
	if (BotCTFCarryingFlag(bs)) return qtrue;
	//
	if (bs->enemy >= 0) {
		//if the enemy is carrying a flag
		BotEntityInfo(bs->enemy, &entinfo);
		if (EntityCarriesFlag(&entinfo)) return qfalse;
	}
	//if the bot is getting the flag
	if (bs->ltgtype == LTG_GETFLAG) return qtrue;
	//
	if (BotAggression(bs) < 50) return qtrue;
	return qfalse;
}

/*
==================
BotWantsToChase
==================
*/
int BotWantsToChase(bot_state_t *bs) {
	aas_entityinfo_t entinfo;

	//always retreat when carrying a CTF flag
	if (BotCTFCarryingFlag(bs)) return qfalse;
	//if the enemy is carrying a flag
	BotEntityInfo(bs->enemy, &entinfo);
	if (EntityCarriesFlag(&entinfo)) return qtrue;
	//if the bot is getting the flag
	if (bs->ltgtype == LTG_GETFLAG) return qfalse;
	//
	if (BotAggression(bs) > 50) return qtrue;
	return qfalse;
}

/*
==================
BotWantsToHelp
==================
*/
int BotWantsToHelp(bot_state_t *bs) {
	return qtrue;
}

/*
==================
BotCanAndWantsToRocketJump
==================
*/
int BotCanAndWantsToRocketJump(bot_state_t *bs) {
	float rocketjumper;

	//if rocket jumping is disabled
	if (!bot_rocketjump.integer) return qfalse;
	//if no rocket launcher
	if (bs->inventory[INVENTORY_QUANTUM] <= 0) return qfalse;
	//if low on rockets
	if (bs->inventory[INVENTORY_QUANTUMAMMO] < 1) return qfalse;
	//never rocket jump with the Quad
	if (bs->inventory[INVENTORY_QUAD])
	{
		if ( rpg_selfdamage.integer != 0 )
		{
			return qfalse;
		}
	}
	//if low on health
	if (bs->inventory[INVENTORY_HEALTH] < 50)
	{	//if not full health
		if ( rpg_selfdamage.integer != 0 )
		{
			return qfalse;
		}
	}
	if (bs->inventory[INVENTORY_HEALTH] < 60)
	{	//if the bot has insufficient armor
		if (bs->inventory[INVENTORY_ARMOR] < 20)
		{
			if ( rpg_selfdamage.integer != 0 )
			{
				return qfalse;
			}
		}
	}
	rocketjumper = 1;
	return qtrue;
}

/*
==================
BotGoCamp
==================
*/
void BotGoCamp(bot_state_t *bs, bot_goal_t *goal) {
	float camper;

	//set message time to zero so bot will NOT show any message
	bs->teammessage_time = 0;
	//set the ltg type
	bs->ltgtype = LTG_CAMP;
	//set the team goal
	memcpy(&bs->teamgoal, goal, sizeof(bot_goal_t));
	//get the team goal time
	camper = 0;
	if (camper > 0.99) bs->teamgoal_time = 99999;
	else bs->teamgoal_time = 120 + 180 * camper + random() * 15;
	//set the last time the bot started camping
	bs->camp_time = trap_AAS_Time();
	//the teammate that requested the camping
	bs->teammate = 0;
	//do NOT type arrive message
	bs->arrive_time = 1;
}

/*
==================
BotWantsToCamp
==================
*/
int BotWantsToCamp(bot_state_t *bs) {
	float camper;
	camper = 0;
	return qfalse;
}

/*
==================
BotDontAvoid
==================
*/
void BotDontAvoid(bot_state_t *bs, char *itemname) {
	bot_goal_t goal;
	int num;

	num = trap_BotGetLevelItemGoal(-1, itemname, &goal);
	while(num >= 0) {
		trap_BotRemoveFromAvoidGoals(bs->gs, goal.number);
		num = trap_BotGetLevelItemGoal(num, itemname, &goal);
	}
}

/*
==================
BotGoForPowerups
==================
*/
void BotGoForPowerups(bot_state_t *bs) {

	//don't avoid any of the powerups anymore
	BotDontAvoid(bs, "Quantum Weapon Enhancer");
	BotDontAvoid(bs, "Nano-Regenerative Protoplasmer");
	BotDontAvoid(bs, "Metaphasic Shielding");
	BotDontAvoid(bs, "Temporal Accelerator");
	BotDontAvoid(bs, "Personal Cloaking Device");
	BotDontAvoid(bs, "Seeker Drone");
	BotDontAvoid(bs, "Anti-Gravity Pack");
	//reset the long term goal time so the bot will go for the powerup
	//NOTE: the long term goal type doesn't change
	bs->ltg_time = 0;
}

/*
==================
BotRoamGoal
==================
*/
void BotRoamGoal(bot_state_t *bs, vec3_t goal) {
	int pc, i;
	float len, rnd;
	vec3_t dir, bestorg, belowbestorg;
	bsp_trace_t trace;

	for (i = 0; i < 10; i++) {
		//start at the bot origin
		VectorCopy(bs->origin, bestorg);
		rnd = random();
		if (rnd > 0.25) {
			//add a random value to the x-coordinate
			if (random() < 0.5) bestorg[0] -= 800 * random() + 100;
			else bestorg[0] += 800 * random() + 100;
		}
		if (rnd < 0.75) {
			//add a random value to the y-coordinate
			if (random() < 0.5) bestorg[1] -= 800 * random() + 100;
			else bestorg[1] += 800 * random() + 100;
		}
		//add a random value to the z-coordinate (NOTE: 48 = maxjump?)
		bestorg[2] += 2 * 48 * crandom();
		//trace a line from the origin to the roam target
		BotAI_Trace(&trace, bs->origin, NULL, NULL, bestorg, bs->entitynum, MASK_SOLID);
		//direction and length towards the roam target
		VectorSubtract(trace.endpos, bs->origin, dir);
		len = VectorNormalize(dir);
		//if the roam target is far away anough
		if (len > 200) {
			//the roam target is in the given direction before walls
			VectorScale(dir, len * trace.fraction - 40, dir);
			VectorAdd(bs->origin, dir, bestorg);
			//get the coordinates of the floor below the roam target
			belowbestorg[0] = bestorg[0];
			belowbestorg[1] = bestorg[1];
			belowbestorg[2] = bestorg[2] - 800;
			BotAI_Trace(&trace, bestorg, NULL, NULL, belowbestorg, bs->entitynum, MASK_SOLID);
			//
			if (!trace.startsolid) {
				trace.endpos[2]++;
				pc = trap_PointContents(trace.endpos, bs->entitynum);
				if (!(pc & (CONTENTS_LAVA | CONTENTS_SLIME))) {
					VectorCopy(bestorg, goal);
					return;
				}
			}
		}
	}
	VectorCopy(bestorg, goal);
}

/*
==================
BotAttackMove
==================
*/
bot_moveresult_t BotAttackMove(bot_state_t *bs, int tfl) {
	int movetype, i;
	float attack_skill, jumper, croucher, dist, strafechange_time;
	float attack_dist, attack_range;
	vec3_t forward, backward, sideward, hordir, up = {0, 0, 1};
	aas_entityinfo_t entinfo;
	bot_moveresult_t moveresult;
	bot_goal_t goal;

	if (bs->attackchase_time > trap_AAS_Time()) {
		//create the chase goal
		goal.entitynum = bs->enemy;
		goal.areanum = bs->lastenemyareanum;
		VectorCopy(bs->lastenemyorigin, goal.origin);
		VectorSet(goal.mins, -8, -8, -8);
		VectorSet(goal.maxs, 8, 8, 8);
		//initialize the movement state
		BotSetupForMovement(bs);
		//move towards the goal
		trap_BotMoveToGoal(&moveresult, bs->ms, &goal, tfl);
		return moveresult;
	}
	//
	memset(&moveresult, 0, sizeof(bot_moveresult_t));
	//
	attack_skill = 1;
	jumper = 1;
	croucher = 1;
	//initialize the movement state
	BotSetupForMovement(bs);
	//get the enemy entity info
	BotEntityInfo(bs->enemy, &entinfo);
	//direction towards the enemy
	VectorSubtract(entinfo.origin, bs->origin, forward);
	//the distance towards the enemy
	dist = VectorNormalize(forward);
	VectorNegate(forward, backward);
	//walk, crouch or jump
	movetype = MOVE_WALK;
	//
	if (bs->attackcrouch_time < trap_AAS_Time() - 1) {
		if (random() < jumper) {
			movetype = MOVE_JUMP;
		}
		//wait at least one second before crouching again
		else if (bs->attackcrouch_time < trap_AAS_Time() - 1 && random() < croucher) {
			bs->attackcrouch_time = trap_AAS_Time() + croucher * 5;
		}
	}
	if (bs->attackcrouch_time > trap_AAS_Time()) movetype = MOVE_CROUCH;
	//if the bot should jump
	if (movetype == MOVE_JUMP) {
		//if jumped last frame
		if (bs->attackjump_time > trap_AAS_Time()) {
			movetype = MOVE_WALK;
		}
		else {
			bs->attackjump_time = trap_AAS_Time() + 1;
		}
	}

	//if using assimilator or alt-fire hypo...
	if ( bs->weaponnum == WP_14 || bs->weaponnum == (WP_12+WP_NUM_WEAPONS) )
	{//get real close
		attack_dist = 16;
		attack_range = 16;
	}
	else
	{
		attack_dist = IDEAL_ATTACKDIST;
		attack_range = 40;
	}

	//increase the strafe time
	bs->attackstrafe_time += bs->thinktime;
	//get the strafe change time
	strafechange_time = 0.4 + (1 - attack_skill) * 0.2;
	if (attack_skill > 0.7) strafechange_time += crandom() * 0.2;
	//if the strafe direction should be changed
	if (bs->attackstrafe_time > strafechange_time) {
		//some magic number :)
		if (random() > 0.935) {
			//flip the strafe direction
			bs->flags ^= BFL_STRAFERIGHT;
			bs->attackstrafe_time = 0;
		}
	}
	//
	for (i = 0; i < 2; i++) {
		hordir[0] = forward[0];
		hordir[1] = forward[1];
		hordir[2] = 0;
		VectorNormalize(hordir);
		//get the sideward vector
		CrossProduct(hordir, up, sideward);
		//reverse the vector depending on the strafe direction
		if (bs->flags & BFL_STRAFERIGHT) VectorNegate(sideward, sideward);
		//randomly go back a little
		if (random() > 0.9) {
			VectorAdd(sideward, backward, sideward);
		}
		else {
			//walk forward or backward to get at the ideal attack distance
			if (dist > attack_dist + attack_range) VectorAdd(sideward, forward, sideward);
			else if (dist < attack_dist - attack_range) VectorAdd(sideward, backward, sideward);
		}
		//perform the movement
		if (trap_BotMoveInDirection(bs->ms, sideward, 400, movetype)) return moveresult;
		//movement failed, flip the strafe direction
		bs->flags ^= BFL_STRAFERIGHT;
		bs->attackstrafe_time = 0;
	}
	//bot couldn't do any usefull movement
//	bs->attackchase_time = AAS_Time() + 6;
	return moveresult;
}

/*
==================
BotSameTeam
==================
*/
int BotSameTeam(bot_state_t *bs, int entnum) {
	char info1[1024], info2[1024];

	if (bs->client < 0 || bs->client >= MAX_CLIENTS) {
		//BotAI_Print(PRT_ERROR, "BotSameTeam: client out of range\n");
		return qfalse;
	}
	if (entnum < 0 || entnum >= MAX_CLIENTS) {
		//BotAI_Print(PRT_ERROR, "BotSameTeam: client out of range\n");
		return qfalse;
	}
	if (gametype == GT_TEAM || gametype == GT_CTF) {
		trap_GetConfigstring(CS_PLAYERS+bs->client, info1, sizeof(info1));
		trap_GetConfigstring(CS_PLAYERS+entnum, info2, sizeof(info2));
		//
		if (atoi(Info_ValueForKey(info1, "t")) == atoi(Info_ValueForKey(info2, "t"))) return qtrue;
	}
	return qfalse;
}

/*
==================
InFieldOfVision
==================
*/
qboolean InFieldOfVision(vec3_t viewangles, float fov, vec3_t angles)
{
	int i;
	float diff, angle;

	for (i = 0; i < 2; i++) {
		angle = AngleMod(viewangles[i]);
		angles[i] = AngleMod(angles[i]);
		diff = angles[i] - angle;
		if (angles[i] > angle) {
			if (diff > 180.0) diff -= 360.0;
		}
		else {
			if (diff < -180.0) diff += 360.0;
		}
		if (diff > 0) {
			if (diff > fov * 0.5) return qfalse;
		}
		else {
			if (diff < -fov * 0.5) return qfalse;
		}
	}
	return qtrue;
}

/*
==================
BotEntityVisible

returns visibility in the range [0, 1] taking fog and water surfaces into account
==================
*/
float BotEntityVisible(int viewer, vec3_t eye, vec3_t viewangles, float fov, int ent) {
	int i, contents_mask, passent, hitent, infog, inwater, otherinfog, pc;
	float fogdist, waterfactor, vis, bestvis;
	bsp_trace_t trace;
	aas_entityinfo_t entinfo;
	vec3_t dir, entangles, start, end, middle;

	//calculate middle of bounding box
	BotEntityInfo(ent, &entinfo);
	VectorAdd(entinfo.mins, entinfo.maxs, middle);
	VectorScale(middle, 0.5, middle);
	VectorAdd(entinfo.origin, middle, middle);
	//check if entity is within field of vision
	VectorSubtract(middle, eye, dir);
	vectoangles(dir, entangles);
	if (!InFieldOfVision(viewangles, fov, entangles)) return 0;
	//
	pc = trap_AAS_PointContents(eye);
	infog = (pc & CONTENTS_SOLID);
	inwater = (pc & (CONTENTS_LAVA|CONTENTS_SLIME|CONTENTS_WATER));
	//
	bestvis = 0;
	for (i = 0; i < 3; i++) {
		//if the point is not in potential visible sight
		//if (!AAS_inPVS(eye, middle)) continue;
		//
		contents_mask = CONTENTS_SOLID|CONTENTS_PLAYERCLIP;
		passent = viewer;
		hitent = ent;
		VectorCopy(eye, start);
		VectorCopy(middle, end);
		//if the entity is in water, lava or slime
		if (trap_AAS_PointContents(middle) & (CONTENTS_LAVA|CONTENTS_SLIME|CONTENTS_WATER)) {
			contents_mask |= (CONTENTS_LAVA|CONTENTS_SLIME|CONTENTS_WATER);
		}
		//if eye is in water, lava or slime
		if (inwater) {
			if (!(contents_mask & (CONTENTS_LAVA|CONTENTS_SLIME|CONTENTS_WATER))) {
				passent = ent;
				hitent = viewer;
				VectorCopy(middle, start);
				VectorCopy(eye, end);
			}
			contents_mask ^= (CONTENTS_LAVA|CONTENTS_SLIME|CONTENTS_WATER);
		}
		//trace from start to end
		BotAI_Trace(&trace, start, NULL, NULL, end, passent, contents_mask);
		//if water was hit
		waterfactor = 1.0;
		if (trace.contents & (CONTENTS_LAVA|CONTENTS_SLIME|CONTENTS_WATER)) {
			//if the water surface is translucent
			if (1) {
				//trace through the water
				contents_mask &= ~(CONTENTS_LAVA|CONTENTS_SLIME|CONTENTS_WATER);
				BotAI_Trace(&trace, trace.endpos, NULL, NULL, end, passent, contents_mask);
				waterfactor = 0.5;
			}
		}
		//if a full trace or the hitent was hit
		if (trace.fraction >= 1 || trace.ent == hitent) {
			//check for fog, assuming there's only one fog brush where
			//either the viewer or the entity is in or both are in
			otherinfog = (trap_AAS_PointContents(middle) & CONTENTS_FOG);
			if (infog && otherinfog) {
				VectorSubtract(trace.endpos, eye, dir);
				fogdist = VectorLength(dir);
			}
			else if (infog) {
				VectorCopy(trace.endpos, start);
				BotAI_Trace(&trace, start, NULL, NULL, eye, viewer, CONTENTS_FOG);
				VectorSubtract(eye, trace.endpos, dir);
				fogdist = VectorLength(dir);
			}
			else if (otherinfog) {
				VectorCopy(trace.endpos, end);
				BotAI_Trace(&trace, eye, NULL, NULL, end, viewer, CONTENTS_FOG);
				VectorSubtract(end, trace.endpos, dir);
				fogdist = VectorLength(dir);
			}
			else {
				//if the entity and the viewer are not in fog assume there's no fog in between
				fogdist = 0;
			}
			//decrease visibility with the view distance through fog
			vis = 1 / ((fogdist * fogdist * 0.001) < 1 ? 1 : (fogdist * fogdist * 0.001));
			//if entering water visibility is reduced
			vis *= waterfactor;
			//
			if (vis > bestvis) bestvis = vis;
			//if pretty much no fog
			if (bestvis >= 0.95) return bestvis;
		}
		//check bottom and top of bounding box as well
		if (i == 0) middle[2] += entinfo.mins[2];
		else if (i == 1) middle[2] += entinfo.maxs[2] - entinfo.mins[2];
	}
	return bestvis;
}

/*
==================
BotFindEnemy
==================
*/
int BotFindEnemy(bot_state_t *bs, int curenemy) {
	int i, healthdecrease;
	float f, dist, curdist, alertness, easyfragger, vis;
	aas_entityinfo_t entinfo, curenemyinfo;
	vec3_t dir, angles;

	alertness = 1;
	easyfragger = 1;
	//check if the health decreased
	healthdecrease = bs->lasthealth > bs->inventory[INVENTORY_HEALTH];
	//remember the current health value
	bs->lasthealth = bs->inventory[INVENTORY_HEALTH];
	//
	if (curenemy >= 0) {
		BotEntityInfo(curenemy, &curenemyinfo);
		if (EntityCarriesFlag(&curenemyinfo)) return qfalse;
		VectorSubtract(curenemyinfo.origin, bs->origin, dir);
		curdist = VectorLength(dir);
	}
	else {
		curdist = 0;
	}
	//
	
	//FIXME: This only finds lowest numbered enemy, not the closest or best!!!
	// 6/15/00 dpk changed this 
	for (i = 0; i < maxclients && i < MAX_CLIENTS; i++) {

		if (i == bs->client) continue;
		//if it's the current enemy
		if (i == curenemy) continue;
		//
		BotEntityInfo(i, &entinfo);
		//
		if (!entinfo.valid) continue;
		//if on the same team
		if (BotSameTeam(bs, i)) continue;
		//if the enemy isn't dead and the enemy isn't the bot self
		if (EntityIsDead(&entinfo) || entinfo.number == bs->entitynum) continue;
		//if the enemy is invisible and not shooting
		if (EntityIsInvisible(&entinfo) && !EntityIsShooting(&entinfo)) {
			continue;
		}
		//if not an easy fragger don't shoot at chatting players
		if (easyfragger < 0.5 && EntityIsChatting(&entinfo)) continue;
		//
		if (lastteleport_time > trap_AAS_Time() - 3) {
			VectorSubtract(entinfo.origin, lastteleport_origin, dir);
			if (VectorLength(dir) < 70) continue;
		}
		//calculate the distance towards the enemy
		VectorSubtract(entinfo.origin, bs->origin, dir);
		dist = VectorLength(dir);
		
	
		//if this entity is not carrying a flag
		if (!EntityCarriesFlag(&entinfo))
//		if (EntityCarriesFlag(&entinfo))
/*		{
			// pick this one!
		}
		else
*/		{
			//if this enemy is further away than the current one
			if (curenemy >= 0 && dist > curdist) continue;
		}		

			//if the bot in too far away for me to notice
			if (dist > 900 + alertness * 4000) continue;
			

			//if the bot's health decreased or the enemy is shooting
			if (curenemy < 0 && (healthdecrease || EntityIsShooting(&entinfo))) f = 360;
			else f = 90 + 90 - (90 - (dist > 810 ? 810 : dist) / 9);
			//check if the enemy is visible
			vis = BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, f, i);
			if (vis <= 0) continue;
			//if the enemy is quite far away, not shooting and the bot is not damaged
			if (curenemy < 0 && dist > 200 && !healthdecrease && !EntityIsShooting(&entinfo))
			{
				//check if we can avoid this enemy
				VectorSubtract(bs->origin, entinfo.origin, dir);
				vectoangles(dir, angles);
				//if the bot isn't in the fov of the enemy
				if (!InFieldOfVision(entinfo.angles, 120, angles)) {
					//update some stuff for this enemy
					BotUpdateBattleInventory(bs, i);
					//if the bot doesn't really want to fight
					if (BotWantsToRetreat(bs)) continue;
				}
			}
//		}
		//found an enemy
		bs->enemy = entinfo.number;
		if (curenemy >= 0) bs->enemysight_time = trap_AAS_Time() - 2;
		else bs->enemysight_time = trap_AAS_Time();
		bs->enemysuicide = qfalse;
		bs->enemydeath_time = 0;
		return qtrue;
	}
	return qfalse;
}

/*
==================
BotTeamFlagCarrierVisible
==================
*/
int BotTeamFlagCarrierVisible(bot_state_t *bs) {
	int i;
	float vis;
	aas_entityinfo_t entinfo;

	for (i = 0; i < maxclients && i < MAX_CLIENTS; i++) {
		if (i == bs->client) continue;
		//
		BotEntityInfo(i, &entinfo);
		//if this player is active
		if (!entinfo.valid) continue;
		//if this player is carrying a flag
		if (!EntityCarriesFlag(&entinfo)) continue;
		//if the flag carrier is not on the same team
		if (!BotSameTeam(bs, i)) continue;
		//if the flag carrier is not visible
		vis = BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, i);
		if (vis <= 0) continue;
		//
		return i;
	}
	return -1;
}

/*
==================
BotAimAtEnemy

  MCG - FIXME: This is not set up at all correctly in the following areas:
	Needs to consider if weapon is an alt weapon for aim and accuracy as well as functionality
	Need to consider range?
	Grenade Primary and alt as well as Scavenger alt need to take into account gravity
	Needs to pick our new weapons correctly to determine which ones they should:
		lead with (projectiles have speed)
		decay aim purposely (instant hit weaps)
		shoot "around corners" (bouncers, mines, splashdamage?)

==================
*/
void BotAimAtEnemy(bot_state_t *bs) {
	int i, enemyvisible;
	float dist, f, aim_skill, aim_accuracy, speed, reactiontime;
	vec3_t dir, bestorigin, end, start, groundtarget, cmdmove, enemyvelocity;
	vec3_t mins = {-4,-4,-4}, maxs = {4, 4, 4};
	weaponinfo_t wi;
	aas_entityinfo_t entinfo;
	bot_goal_t goal;
	bsp_trace_t trace;
	vec3_t target;

	//if the bot has no enemy
	if (bs->enemy < 0) return;
	//
	//BotAI_Print(PRT_MESSAGE, "client %d: aiming at client %d\n", bs->entitynum, bs->enemy);
	//
	aim_skill = 1;
	aim_accuracy = 1;
	//
	if (aim_skill > 0.95)
	{
		//don't aim too early
		reactiontime = 0.5;
		if (bs->enemysight_time > trap_AAS_Time() - reactiontime) return;
		if (bs->teleport_time > trap_AAS_Time() - reactiontime) return;
	}

	//get the weapon information
	trap_BotGetWeaponInfo(bs->ws, bs->weaponnum, &wi);
	//get the weapon specific aim accuracy and or aim skill

	if (wi.number == WP_8) {
		aim_accuracy = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_AIM_ACCURACY_GRENADELAUNCHER, 0, 1);
		aim_skill = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_AIM_SKILL_GRENADELAUNCHER, 0, 1);
	}
	if (wi.number == WP_10) {
		aim_accuracy = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_AIM_ACCURACY_STASIS, 0, 1);
		aim_skill = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_AIM_SKILL_STASIS, 0, 1);
	}
	if (wi.number == WP_5) {
		aim_accuracy = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_AIM_ACCURACY_PHASER, 0, 1);
	}
	if (wi.number == WP_1) {
		aim_accuracy = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_AIM_ACCURACY_IMOD, 0, 1);
	}
	if (wi.number == WP_6) {
		aim_accuracy = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_AIM_ACCURACY_COMPRESSION, 0, 1);
	}
	if (wi.number == WP_7) {
		aim_accuracy = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_AIM_ACCURACY_TETRION, 0, 1);
	}
	if (wi.number == WP_13) {
		aim_accuracy = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_AIM_ACCURACY_DREADNOUGHT, 0, 1);
	}
	if (wi.number == WP_9) {
		aim_accuracy = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_AIM_ACCURACY_QUANTUM, 0, 1);
		aim_skill = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_AIM_SKILL_QUANTUM, 0, 1);
	}
	if (wi.number == WP_4) {
		aim_accuracy = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_AIM_ACCURACY_SCAVENGER, 0, 1);
	}

	//
	if (aim_accuracy <= 0) aim_accuracy = 0.0001;
	//get the enemy entity information
	BotEntityInfo(bs->enemy, &entinfo);
	//if the enemy is invisible then shoot crappy most of the time
	if (EntityIsInvisible(&entinfo)) {
		if (random() > 0.1) aim_accuracy *= 0.4;
	}
	//
	VectorSubtract(entinfo.origin, entinfo.lastvisorigin, enemyvelocity);
	VectorScale(enemyvelocity, 1 / entinfo.update_time, enemyvelocity);
	//enemy origin and velocity is remembered every 0.5 seconds
	if (bs->enemyposition_time < trap_AAS_Time()) {
		//
		bs->enemyposition_time = trap_AAS_Time() + 0.5;
		VectorCopy(enemyvelocity, bs->enemyvelocity);
		VectorCopy(entinfo.origin, bs->enemyorigin);
	}
	//if not extremely skilled
	if (aim_skill < 0.9) {
		VectorSubtract(entinfo.origin, bs->enemyorigin, dir);
		//if the enemy moved a bit
		if (VectorLength(dir) > 48) {
			//if the enemy changed direction
			if (DotProduct(bs->enemyvelocity, enemyvelocity) < 0) {
				//aim accuracy should be worse now
				aim_accuracy *= 0.7;
			}
		}
	}
	//check visibility of enemy
	enemyvisible = BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, bs->enemy);
	//if the enemy is visible
	if (enemyvisible) {
		//
		VectorCopy(entinfo.origin, bestorigin);
		bestorigin[2] += 8;
		//get the start point shooting from
		//NOTE: the x and y projectile start offsets are ignored
		VectorCopy(bs->origin, start);
		start[2] += bs->cur_ps.viewheight;
		start[2] += wi.offset[2];
		//
		BotAI_Trace(&trace, start, mins, maxs, bestorigin, bs->entitynum, MASK_SHOT);
		//if the enemy is NOT hit
		if (trace.fraction <= 1 && trace.ent != entinfo.number) {
			bestorigin[2] += 16;
		}
		//if it is not an instant hit weapon the bot might want to predict the enemy
		if (wi.speed) {
			//
			VectorSubtract(bestorigin, bs->origin, dir);
			dist = VectorLength(dir);
			VectorSubtract(entinfo.origin, bs->enemyorigin, dir);
			//if the enemy is NOT pretty far away and strafing just small steps left and right
			if (!(dist > 100 && VectorLength(dir) < 32)) {
				//if skilled anough do exact prediction
				if (aim_skill > 0.8 &&
						//if the weapon is ready to fire
						bs->cur_ps.weaponstate == WEAPON_READY) {
					aas_clientmove_t move;
					vec3_t origin;

					VectorSubtract(entinfo.origin, bs->origin, dir);
					//distance towards the enemy
					dist = VectorLength(dir);
					//direction the enemy is moving in
					VectorSubtract(entinfo.origin, entinfo.lastvisorigin, dir);
					//
					VectorScale(dir, 1 / entinfo.update_time, dir);
					//
					VectorCopy(entinfo.origin, origin);
					origin[2] += 1;
					//
					VectorClear(cmdmove);
					//AAS_ClearShownDebugLines();
					trap_AAS_PredictClientMovement(&move, bs->enemy, origin,
														PRESENCE_CROUCH, qfalse,
														dir, cmdmove, 0,
														dist * 10 / wi.speed, 0.1, 0, 0, qfalse);
					VectorCopy(move.endpos, bestorigin);
					//BotAI_Print(PRT_MESSAGE, "%1.1f predicted speed = %f, frames = %f\n", trap_AAS_Time(), VectorLength(dir), dist * 10 / wi.speed);
				}
				//if not that skilled do linear prediction
				else if (aim_skill > 0.4) {
					VectorSubtract(entinfo.origin, bs->origin, dir);
					//distance towards the enemy
					dist = VectorLength(dir);
					//direction the enemy is moving in
					VectorSubtract(entinfo.origin, entinfo.lastvisorigin, dir);
					dir[2] = 0;
					//
					speed = VectorNormalize(dir) / entinfo.update_time;
					//botimport.Print(PRT_MESSAGE, "speed = %f, wi->speed = %f\n", speed, wi->speed);
					//best spot to aim at
					VectorMA(entinfo.origin, (dist / wi.speed) * speed, dir, bestorigin);
				}
			}
		}
		//if the projectile does radial damage
		if (aim_skill > 0.6 && wi.proj.damagetype & DAMAGETYPE_RADIAL) {
			//if the enemy isn't standing significantly higher than the bot
			if (entinfo.origin[2] < bs->origin[2] + 16) {
				//try to aim at the ground in front of the enemy
				VectorCopy(entinfo.origin, end);
				end[2] -= 64;
				BotAI_Trace(&trace, entinfo.origin, NULL, NULL, end, entinfo.number, MASK_SHOT);
				//
				VectorCopy(bestorigin, groundtarget);
				if (trace.startsolid) groundtarget[2] = entinfo.origin[2] - 16;
				else groundtarget[2] = trace.endpos[2] - 8;
				//trace a line from projectile start to ground target
				BotAI_Trace(&trace, start, NULL, NULL, groundtarget, bs->entitynum, MASK_SHOT);
				//if hitpoint is not vertically too far from the ground target
				if (fabs(trace.endpos[2] - groundtarget[2]) < 50) {
					VectorSubtract(trace.endpos, groundtarget, dir);
					//if the hitpoint is near anough the ground target
					if (VectorLength(dir) < 60) {
						VectorSubtract(trace.endpos, start, dir);
						//if the hitpoint is far anough from the bot
						if (VectorLength(dir) > 100) {
							//check if the bot is visible from the ground target
							trace.endpos[2] += 1;
							BotAI_Trace(&trace, trace.endpos, NULL, NULL, entinfo.origin, entinfo.number, MASK_SHOT);
							if (trace.fraction >= 1) {
								//botimport.Print(PRT_MESSAGE, "%1.1f aiming at ground\n", AAS_Time());
								VectorCopy(groundtarget, bestorigin);
							}
						}
					}
				}
			}
		}
		bestorigin[0] += 20 * crandom() * (1 - aim_accuracy);
		bestorigin[1] += 20 * crandom() * (1 - aim_accuracy);
		bestorigin[2] += 10 * crandom() * (1 - aim_accuracy);
	}
	else {
		//
		VectorCopy(bs->lastenemyorigin, bestorigin);
		bestorigin[2] += 8;
		//if the bot is skilled anough
		if (aim_skill > 0.5) {
			//do prediction shots around corners
			if (wi.number == WP_10 ||
				wi.number == WP_8)
			{
				//create the chase goal
				goal.entitynum = bs->client;
				goal.areanum = bs->areanum;
				VectorCopy(bs->eye, goal.origin);
				VectorSet(goal.mins, -8, -8, -8);
				VectorSet(goal.maxs, 8, 8, 8);
				//
				if (trap_BotPredictVisiblePosition(bs->lastenemyorigin, bs->lastenemyareanum, &goal, TFL_DEFAULT, target)) {
					VectorSubtract(target, bs->eye, dir);
					if (VectorLength(dir) > 80) {
						VectorCopy(target, bestorigin);
						bestorigin[2] -= 20;
					}
				}
				aim_accuracy = 1;
			}
		}
	}
	//
	if (enemyvisible) {
		BotAI_Trace(&trace, bs->eye, NULL, NULL, bestorigin, bs->entitynum, MASK_SHOT);
		VectorCopy(trace.endpos, bs->aimtarget);
	}
	else {
		VectorCopy(bestorigin, bs->aimtarget);
	}
	//get aim direction
	VectorSubtract(bestorigin, bs->eye, dir);
	// kef -- fixme. i'm guessing this is listing all of the instant-hit weapons?
	if (wi.number == WP_5 ||
		wi.number == WP_1) {
		//distance towards the enemy
		dist = VectorLength(dir);
		if (dist > 150) dist = 150;
		f = 0.6 + dist / 150 * 0.4;
		aim_accuracy *= f;
	}
	//add some random stuff to the aim direction depending on the aim accuracy
	if (aim_accuracy < 0.8) {
		VectorNormalize(dir);
		for (i = 0; i < 3; i++) dir[i] += 0.3 * crandom() * (1 - aim_accuracy);
	}
	//set the ideal view angles
	vectoangles(dir, bs->ideal_viewangles);
	//take the weapon spread into account for lower skilled bots
	bs->ideal_viewangles[PITCH] += 6 * wi.vspread * crandom() * (1 - aim_accuracy);
	bs->ideal_viewangles[PITCH] = AngleMod(bs->ideal_viewangles[PITCH]);
	bs->ideal_viewangles[YAW] += 6 * wi.hspread * crandom() * (1 - aim_accuracy);
	bs->ideal_viewangles[YAW] = AngleMod(bs->ideal_viewangles[YAW]);
	//if the bots should be really challenging
	//if the bot is really accurate and has the enemy in view for some time
	if (aim_accuracy > 0.9 && bs->enemysight_time < trap_AAS_Time() - 1)
	{
		//set the view angles directly
		if (bs->ideal_viewangles[PITCH] > 180)
		{
			bs->ideal_viewangles[PITCH] -= 360;
		}
		VectorCopy(bs->ideal_viewangles, bs->viewangles);
		trap_EA_View(bs->client, bs->viewangles);
	}
}

/*
==================
BotCheckAttack
==================
*/
void BotCheckAttack(bot_state_t *bs) {
	float points, reactiontime, fov, firethrottle;
	bsp_trace_t bsptrace;
	//float selfpreservation;
	vec3_t forward, right, start, end, dir, angles;
	weaponinfo_t wi;
	bsp_trace_t trace;
	aas_entityinfo_t entinfo;
	vec3_t mins = {-8, -8, -8}, maxs = {8, 8, 8};

	if (bs->enemy < 0) return;
	//
	reactiontime = 0;
	if (bs->enemysight_time > trap_AAS_Time() - reactiontime) return;
	if (bs->teleport_time > trap_AAS_Time() - reactiontime) return;
	//if changing weapons
	if (bs->weaponchange_time > trap_AAS_Time() - 0.1) return;
	//check fire throttle characteristic
	if (bs->firethrottlewait_time > trap_AAS_Time()) return;
	firethrottle = 1;
	if (bs->firethrottleshoot_time < trap_AAS_Time()) {
		if (random() > firethrottle) {
			bs->firethrottlewait_time = trap_AAS_Time() + firethrottle;
			bs->firethrottleshoot_time = 0;
		}
		else {
			bs->firethrottleshoot_time = trap_AAS_Time() + 1 - firethrottle;
			bs->firethrottlewait_time = 0;
		}
	}
	//
	BotEntityInfo(bs->enemy, &entinfo);
	VectorSubtract(entinfo.origin, bs->eye, dir);
	//
	if (VectorLength(dir) < 100) fov = 120;
	else fov = 50;
	/*
	//if the enemy isn't visible
	if (!BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, fov, bs->enemy)) {
		//botimport.Print(PRT_MESSAGE, "enemy not visible\n");
		return;
	}*/
	vectoangles(dir, angles);
	if (!InFieldOfVision(bs->viewangles, fov, angles)) return;
	BotAI_Trace(&bsptrace, bs->eye, NULL, NULL, bs->aimtarget, bs->client, CONTENTS_SOLID|CONTENTS_PLAYERCLIP);
	if (bsptrace.fraction < 1 && bsptrace.ent != bs->enemy) return;

	//get the weapon info
	trap_BotGetWeaponInfo(bs->ws, bs->weaponnum, &wi);
	//get the start point shooting from
	VectorCopy(bs->origin, start);
	start[2] += bs->cur_ps.viewheight;
	AngleVectors(bs->viewangles, forward, right, NULL);
	start[0] += forward[0] * wi.offset[0] + right[0] * wi.offset[1];
	start[1] += forward[1] * wi.offset[0] + right[1] * wi.offset[1];
	start[2] += forward[2] * wi.offset[0] + right[2] * wi.offset[1] + wi.offset[2];
	//end point aiming at
	VectorMA(start, 1000, forward, end);
	//a little back to make sure not inside a very close enemy
	VectorMA(start, -12, forward, start);
	BotAI_Trace(&trace, start, mins, maxs, end, bs->entitynum, MASK_SHOT);
	//if won't hit the enemy
	if (trace.ent != bs->enemy) {
		//if the entity is a client
		if (trace.ent > 0 && trace.ent <= MAX_CLIENTS) {
			//if a teammate is hit
			if (BotSameTeam(bs, trace.ent)) return;
		}
		//if the projectile does a radial damage
		if (wi.proj.damagetype & DAMAGETYPE_RADIAL) {
			if (trace.fraction * 1000 < wi.proj.radius) {
				points = (wi.proj.damage - 0.5 * trace.fraction * 1000) * 0.5;
				if (points > 0) {
//					selfpreservation = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_SELFPRESERVATION, 0, 1);
//					if (random() < selfpreservation) return;
					return;
				}
			}
			//FIXME: check if a teammate gets radial damage
		}
	}
	//if fire has to be release to activate weapon
	if (wi.flags & WFL_FIRERELEASED) {
		if (bs->flags & BFL_ATTACKED)
		{// check in here to either call the alt_fire or not!!  also, adjust the bots weapon...  hackery?
			if (bs->weaponnum > WP_NUM_WEAPONS) // it's an alt_fire!
			{
				bs->weaponnum -= WP_NUM_WEAPONS;
				trap_EA_Alt_Attack(bs->client);
			}
			else
			{
				trap_EA_Attack(bs->client);
			}
		}
	}
	else
	{ // check in here to either call the alt_fire or not!!  also, adjust the bots weapon...  hackery?
		if (bs->weaponnum > WP_NUM_WEAPONS) // it's an alt_fire!
		{
			bs->weaponnum -= WP_NUM_WEAPONS;
			trap_EA_Alt_Attack(bs->client);
		}
		else
		{
			trap_EA_Attack(bs->client);
		}
	}
	bs->flags ^= BFL_ATTACKED;
}

/*
==================
BotMapScripts
==================
*/
void BotMapScripts(bot_state_t *bs) {
	char info[1024];
	char mapname[128];
	int i, shootbutton;
	float aim_accuracy;
	aas_entityinfo_t entinfo;
	vec3_t dir;

	trap_GetServerinfo(info, sizeof(info));

	strncpy(mapname, Info_ValueForKey( info, "mapname" ), sizeof(mapname)-1);
	mapname[sizeof(mapname)-1] = '\0';

	if (!Q_stricmp(mapname, "q3tourney6")) {
		vec3_t mins = {700, 204, 672}, maxs = {964, 468, 680};
		vec3_t buttonorg = {304, 352, 920};
		//NOTE: NEVER use the func_bobbing in q3tourney6
		bs->tfl &= ~TFL_FUNCBOB;
		//if the bot is below the bounding box
		if (bs->origin[0] > mins[0] && bs->origin[0] < maxs[0]) {
			if (bs->origin[1] > mins[1] && bs->origin[1] < maxs[1]) {
				if (bs->origin[2] < mins[2]) {
					return;
				}
			}
		}
		shootbutton = qfalse;
		//if an enemy is below this bounding box then shoot the button
		for (i = 0; i < maxclients && i < MAX_CLIENTS; i++) {

			if (i == bs->client) continue;
			//
			BotEntityInfo(i, &entinfo);
			//
			if (!entinfo.valid) continue;
			//if the enemy isn't dead and the enemy isn't the bot self
			if (EntityIsDead(&entinfo) || entinfo.number == bs->entitynum) continue;
			//
			if (entinfo.origin[0] > mins[0] && entinfo.origin[0] < maxs[0]) {
				if (entinfo.origin[1] > mins[1] && entinfo.origin[1] < maxs[1]) {
					if (entinfo.origin[2] < mins[2]) {
						//if there's a team mate below the crusher
						if (BotSameTeam(bs, i)) {
							shootbutton = qfalse;
							break;
						}
						else {
							shootbutton = qtrue;
						}
					}
				}
			}
		}
		if (shootbutton) {
			bs->flags |= BFL_IDEALVIEWSET;
			VectorSubtract(buttonorg, bs->eye, dir);
			vectoangles(dir, bs->ideal_viewangles);
			aim_accuracy = 1;
			bs->ideal_viewangles[PITCH] += 8 * crandom() * (1 - aim_accuracy);
			bs->ideal_viewangles[PITCH] = AngleMod(bs->ideal_viewangles[PITCH]);
			bs->ideal_viewangles[YAW] += 8 * crandom() * (1 - aim_accuracy);
			bs->ideal_viewangles[YAW] = AngleMod(bs->ideal_viewangles[YAW]);
			//
			if (InFieldOfVision(bs->viewangles, 20, bs->ideal_viewangles))
			{// check in here to either call the alt_fire or not!!  also, adjust the bots weapon...  hackery?
				if (bs->weaponnum > WP_NUM_WEAPONS) // it's an alt_fire!
				{
					bs->weaponnum -= WP_NUM_WEAPONS;
					trap_EA_Alt_Attack(bs->client);
				}
				else
				{
					trap_EA_Attack(bs->client);
				}
			}
		}
	}
}

/*
==================
BotEntityToActivate
==================
*/
//#define OBSTACLEDEBUG

int BotEntityToActivate(int entitynum) {
	int i, ent, cur_entities[10];
	char model[MAX_INFO_STRING], tmpmodel[128];
	char target[128], classname[128];
	float health;
	char targetname[10][128];
	aas_entityinfo_t entinfo;

	BotEntityInfo(entitynum, &entinfo);
	Com_sprintf(model, sizeof( model ), "*%d", entinfo.modelindex);
	for (ent = trap_AAS_NextBSPEntity(0); ent; ent = trap_AAS_NextBSPEntity(ent)) {
		if (!trap_AAS_ValueForBSPEpairKey(ent, "model", tmpmodel, sizeof(tmpmodel))) continue;
		if (!strcmp(model, tmpmodel)) break;
	}
	if (!ent) {
		BotAI_Print(PRT_ERROR, "BotEntityToActivate: no entity found with model %s\n", model);
		return 0;
	}
	trap_AAS_ValueForBSPEpairKey(ent, "classname", classname, sizeof(classname));
	if (!classname[0]) {
		BotAI_Print(PRT_ERROR, "BotEntityToActivate: entity with model %s has no classname\n", model);
		return 0;
	}
	//if it is a door
	if (!strcmp(classname, "func_door")) {
		if (trap_AAS_FloatForBSPEpairKey(ent, "health", &health)) {
			//if health the door must be shot to open
			if (health) return ent;
		}
	}
	//get the targetname so we can find an entity with a matching target
	if (!trap_AAS_ValueForBSPEpairKey(ent, "targetname", targetname[0], sizeof(targetname[0]))) {
#ifdef OBSTACLEDEBUG
		BotAI_Print(PRT_ERROR, "BotEntityToActivate: entity with model \"%s\" has no targetname\n", model);
#endif //OBSTACLEDEBUG
		return 0;
	}
	//only allows single activation chains, tree-like activation can be added back in
	cur_entities[0] = trap_AAS_NextBSPEntity(0);
	for (i = 0; i >= 0 && i < 10;) {
		for (ent = cur_entities[i]; ent; ent = trap_AAS_NextBSPEntity(ent)) {
			if (!trap_AAS_ValueForBSPEpairKey(ent, "target", target, sizeof(target))) continue;
			if (!strcmp(targetname[i], target)) {
				cur_entities[i] = trap_AAS_NextBSPEntity(ent);
				break;
			}
		}
		if (!ent) {
			BotAI_Print(PRT_ERROR, "BotEntityToActivate: no entity with target \"%s\"\n", targetname[i]);
			i--;
			continue;
		}
		if (!trap_AAS_ValueForBSPEpairKey(ent, "classname", classname, sizeof(classname))) {
			BotAI_Print(PRT_ERROR, "BotEntityToActivate: entity with target \"%s\" has no classname\n", targetname[i]);
			continue;
		}
		if (!strcmp(classname, "func_button")) {
			//BSP button model
			return ent;
		}
		else if (!strcmp(classname, "trigger_multiple")) {
			//invisible trigger multiple box
			return ent;
		}
		else {
			i--;
		}
	}
	BotAI_Print(PRT_ERROR, "BotEntityToActivate: unknown activator with classname \"%s\"\n", classname);
	return 0;
}

/*
==================
BotSetMovedir
==================
*/
vec3_t VEC_UP		= {0, -1,  0};
vec3_t MOVEDIR_UP	= {0,  0,  1};
vec3_t VEC_DOWN		= {0, -2,  0};
vec3_t MOVEDIR_DOWN	= {0,  0, -1};

void BotSetMovedir(vec3_t angles, vec3_t movedir) {
	if (VectorCompare(angles, VEC_UP)) {
		VectorCopy(MOVEDIR_UP, movedir);
	}
	else if (VectorCompare(angles, VEC_DOWN)) {
		VectorCopy(MOVEDIR_DOWN, movedir);
	}
	else {
		AngleVectors(angles, movedir, NULL, NULL);
	}
}

/*
==================
BotModelMinsMaxs

this is ugly
==================
*/
void BotModelMinsMaxs(int modelindex, int eType, vec3_t mins, vec3_t maxs) {
	gentity_t *ent;
	int i;

	ent = &g_entities[0];
	for (i = 0; i < level.num_entities; i++, ent++) {
		if ( !ent->inuse ) {
			continue;
		}
		if ( ent->s.eType != eType) {
			continue;
		}
		if (ent->s.modelindex == modelindex) {
			VectorAdd(ent->r.currentOrigin, ent->r.mins, mins);
			VectorAdd(ent->r.currentOrigin, ent->r.maxs, maxs);
			return;
		}
	}
	VectorClear(mins);
	VectorClear(maxs);
}

/*
==================
BotAIBlocked

very basic handling of bots being blocked by other entities
check what kind of entity is blocking the bot and try to activate
it otherwise try to walk around the entity
before the bot ends in this part of the AI it should predict which doors to open,
which buttons to activate etc.
==================
*/
void BotAIBlocked(bot_state_t *bs, bot_moveresult_t *moveresult, int activate) {
	int movetype, ent, i, areas[10], numareas, modelindex;
	char classname[128], model[128];
	float lip, dist, health, angle;
	vec3_t hordir, size, start, end, mins, maxs, sideward, angles;
	vec3_t movedir, origin, goalorigin, bboxmins, bboxmaxs;
	vec3_t up = {0, 0, 1}, extramins = {1, 1, 1}, extramaxs = {-1, -1, -1};
	aas_entityinfo_t entinfo;
	//bsp_trace_t bsptrace;
#ifdef OBSTACLEDEBUG
	char netname[MAX_NETNAME];
	char buf[128];
#endif

	if (!moveresult->blocked) {
		bs->notblocked_time = trap_AAS_Time();
		return;
	}
	//
	BotEntityInfo(moveresult->blockentity, &entinfo);
#ifdef OBSTACLEDEBUG
	ClientName(bs->client, netname, sizeof(netname));
	BotAI_Print(PRT_MESSAGE, "%s: I'm blocked by model %d\n", netname, entinfo.modelindex);
#endif
	//if blocked by a bsp model and the bot wants to activate it if possible
	if (entinfo.modelindex > 0 && entinfo.modelindex <= max_bspmodelindex && activate) {
		//find the bsp entity which should be activated in order to remove
		//the blocking entity
		ent = BotEntityToActivate(entinfo.number);
		if (!ent) {
			strcpy(classname, "");
#ifdef OBSTACLEDEBUG
			BotAI_Print(PRT_MESSAGE, "%s: can't find activator for blocking entity\n", ClientName(bs->client, netname, sizeof(netname)));
#endif
		}
		else {
			trap_AAS_ValueForBSPEpairKey(ent, "classname", classname, sizeof(classname));
#ifdef OBSTACLEDEBUG
			ClientName(bs->client, netname, sizeof(netname));
			BotAI_Print(PRT_MESSAGE, "%s: I should activate %s\n", netname, classname);
#endif
		}
		if (!strcmp(classname, "func_button")) {
			//create a bot goal towards the button
			trap_AAS_ValueForBSPEpairKey(ent, "model", model, sizeof(model));
			modelindex = atoi(model+1);
			//if the model is not loaded
			if (!modelindex) return;
			VectorClear(angles);
			BotModelMinsMaxs(modelindex, ET_MOVER, mins, maxs);
			//get the lip of the button
			trap_AAS_FloatForBSPEpairKey(ent, "lip", &lip);
			if (!lip) lip = 4;
			//get the move direction from the angle
			trap_AAS_FloatForBSPEpairKey(ent, "angle", &angle);
			VectorSet(angles, 0, angle, 0);
			BotSetMovedir(angles, movedir);
			//button size
			VectorSubtract(maxs, mins, size);
			//button origin
			VectorAdd(mins, maxs, origin);
			VectorScale(origin, 0.5, origin);
			//touch distance of the button
			dist = fabs(movedir[0]) * size[0] + fabs(movedir[1]) * size[1] + fabs(movedir[2]) * size[2];
			dist *= 0.5;
			//
			trap_AAS_FloatForBSPEpairKey(ent, "health", &health);
			//if the button is shootable
			if (health) {
				//FIXME: walk to a point where the button is visible and shoot at the button
				//calculate the goal origin
				VectorMA(origin, -dist, movedir, goalorigin);
				//
				VectorSubtract(goalorigin, bs->origin, movedir);
				vectoangles(movedir, moveresult->ideal_viewangles);
				moveresult->flags |= MOVERESULT_MOVEMENTVIEW;
				moveresult->flags |= MOVERESULT_MOVEMENTWEAPON;
				//select the machinegun and shoot
				trap_EA_SelectWeapon(bs->client, WEAPONINDEX_PHASER);
				if (bs->cur_ps.weapon == WEAPONINDEX_PHASER) {
					trap_EA_Attack(bs->client);
				}
				return;
			}
			else {
				//add bounding box size to the dist
				trap_AAS_PresenceTypeBoundingBox(PRESENCE_CROUCH, bboxmins, bboxmaxs);
				for (i = 0; i < 3; i++) {
					if (movedir[i] < 0) dist += fabs(movedir[i]) * fabs(bboxmaxs[i]);
					else dist += fabs(movedir[i]) * fabs(bboxmins[i]);
				}
				//calculate the goal origin
				VectorMA(origin, -dist, movedir, goalorigin);
				//
				VectorCopy(goalorigin, start);
				start[2] += 24;
				VectorCopy(start, end);
				end[2] -= 100;
				numareas = trap_AAS_TraceAreas(start, end, areas, NULL, 10);
				//
				for (i = 0; i < numareas; i++) {
					if (trap_AAS_AreaReachability(areas[i])) {
						break;
					}
				}
				if (i < numareas) {
					//
#ifdef OBSTACLEDEBUG
					if (bs->activatemessage_time < trap_AAS_Time()) {
						Com_sprintf(buf, sizeof(buf), "I have to activate a button at %1.1f %1.1f %1.1f in area %d\n",
										goalorigin[0], goalorigin[1], goalorigin[2], areas[i]);
						trap_EA_Say(bs->client, buf);
						bs->activatemessage_time = trap_AAS_Time() + 5;
					}
#endif //OBSTACLEDEBUG
					//
					VectorCopy(origin, bs->activategoal.origin);
					bs->activategoal.areanum = areas[i];
					VectorSubtract(mins, origin, bs->activategoal.mins);
					VectorSubtract(maxs, origin, bs->activategoal.maxs);
					//
					for (i = 0; i < 3; i++)
					{
						if (movedir[i] < 0) bs->activategoal.maxs[i] += fabs(movedir[i]) * fabs(extramaxs[i]);
						else bs->activategoal.mins[i] += fabs(movedir[i]) * fabs(extramins[i]);
					} //end for
					//
					bs->activategoal.entitynum = entinfo.number;
					bs->activategoal.number = 0;
					bs->activategoal.flags = 0;
					bs->activate_time = trap_AAS_Time() + 10;
					AIEnter_Seek_ActivateEntity(bs);
					return;
				}
				else {
#ifdef OBSTACLEDEBUG
					if (!numareas) BotAI_Print(PRT_MESSAGE, "button not in an area\n");
					else BotAI_Print(PRT_MESSAGE, "button area has no reachabilities\n");
#endif //OBSTACLEDEBUG
					if (bs->ainode == AINode_Seek_NBG) bs->nbg_time = 0;
					else if (bs->ainode == AINode_Seek_LTG) bs->ltg_time = 0;
				}
			}
		}
		else if (!strcmp(classname, "func_door")) {
			//shoot at the shootable door
			trap_AAS_ValueForBSPEpairKey(ent, "model", model, sizeof(model));
			modelindex = atoi(model+1);
			//if the model is not loaded
			if (!modelindex) return;
			VectorClear(angles);
			BotModelMinsMaxs(modelindex, ET_MOVER, mins, maxs);
			//door origin
			VectorAdd(mins, maxs, origin);
			VectorScale(origin, 0.5, origin);
			//
			VectorSubtract(origin, bs->eye, movedir);
			vectoangles(movedir, moveresult->ideal_viewangles);
			moveresult->flags |= MOVERESULT_MOVEMENTVIEW;
			moveresult->flags |= MOVERESULT_MOVEMENTWEAPON;
			//select the machinegun and shoot
			trap_EA_SelectWeapon(bs->client, WEAPONINDEX_PHASER);
			if (bs->cur_ps.weapon == WEAPONINDEX_PHASER) {
				trap_EA_Attack(bs->client);
			}
			return;
		}
	}
	//just some basic dynamic obstacle avoidance code
	hordir[0] = moveresult->movedir[0];
	hordir[1] = moveresult->movedir[1];
	hordir[2] = 0;
	//if no direction just take a random direction
	if (VectorNormalize(hordir) < 0.1) {
		VectorSet(angles, 0, 360 * random(), 0);
		AngleVectors(angles, hordir, NULL, NULL);
	}
	//
	//if (moveresult->flags & MOVERESULT_ONTOPOFOBSTACLE) movetype = MOVE_JUMP;
	//else
	movetype = MOVE_WALK;
	//if there's an obstacle at the bot's feet and head then
	//the bot might be able to crouch through
	VectorCopy(bs->origin, start);
	start[2] += 18;
	VectorMA(start, 5, hordir, end);
	VectorSet(mins, -16, -16, -24);
	VectorSet(maxs, 16, 16, 4);
	//
	//bsptrace = AAS_Trace(start, mins, maxs, end, bs->entitynum, MASK_PLAYERSOLID);
	//if (bsptrace.fraction >= 1) movetype = MOVE_CROUCH;
	//get the sideward vector
	CrossProduct(hordir, up, sideward);
	//
	if (bs->flags & BFL_AVOIDRIGHT) VectorNegate(sideward, sideward);
	//try to crouch straight forward?
	if (movetype != MOVE_CROUCH || !trap_BotMoveInDirection(bs->ms, hordir, 400, movetype)) {
		//perform the movement
		if (!trap_BotMoveInDirection(bs->ms, sideward, 400, movetype)) {
			//flip the avoid direction flag
			bs->flags ^= BFL_AVOIDRIGHT;
			//flip the direction
			VectorNegate(sideward, sideward);
			//move in the other direction
			trap_BotMoveInDirection(bs->ms, sideward, 400, movetype);
		}
	}
	//
	if (bs->notblocked_time < trap_AAS_Time() - 0.4) {
		//just reset goals and hope the bot will go into another direction
		//is this still needed??
		if (bs->ainode == AINode_Seek_NBG) bs->nbg_time = 0;
		else if (bs->ainode == AINode_Seek_LTG) bs->ltg_time = 0;
	}
}

/*
==================
BotCheckConsoleMessages
==================
*/
void BotCheckConsoleMessages(bot_state_t *bs) {
	char botname[MAX_NETNAME], message[MAX_MESSAGE_SIZE], netname[MAX_NETNAME], *ptr;
	float chat_reply;
	int context, handle;
	bot_consolemessage_t m;
	bot_match_t match;

	//the name of this bot
	ClientName(bs->client, botname, sizeof(botname));
	//
	while((handle = trap_BotNextConsoleMessage(bs->cs, &m)) != 0) {
		//if the chat state is flooded with messages the bot will read them quickly
		if (trap_BotNumConsoleMessages(bs->cs) < 10) {
			//if it is a chat message the bot needs some time to read it
			if (m.type == CMS_CHAT && m.time > trap_AAS_Time() - (1 + random())) break;
		}
		//
		ptr = m.message;
		//if it is a chat message then don't unify white spaces and don't
		//replace synonyms in the netname
		if (m.type == CMS_CHAT) {
			//
			if (trap_BotFindMatch(m.message, &match, MTCONTEXT_REPLYCHAT)) {
				ptr = m.message + match.variables[MESSAGE].offset;
			}
		}
		//unify the white spaces in the message
		trap_UnifyWhiteSpaces(ptr);
		//replace synonyms in the right context
		context = CONTEXT_NORMAL|CONTEXT_NEARBYITEM|CONTEXT_NAMES;
		if (BotCTFTeam(bs) == CTF_TEAM_RED) context |= CONTEXT_CTFREDTEAM;
		else context |= CONTEXT_CTFBLUETEAM;
		trap_BotReplaceSynonyms(ptr, context);
		//if there's no match
		if (!BotMatchMessage(bs, m.message)) {
			//if it is a chat message
			if (m.type == CMS_CHAT && !bot_nochat.integer) {
				//
				if (!trap_BotFindMatch(m.message, &match, MTCONTEXT_REPLYCHAT)) {
					trap_BotRemoveConsoleMessage(bs->cs, handle);
					continue;
				}
				//don't use eliza chats with team messages
				if (match.subtype & ST_TEAM) {
					trap_BotRemoveConsoleMessage(bs->cs, handle);
					continue;
				}
				//
				trap_BotMatchVariable(&match, NETNAME, netname, sizeof(netname));
				trap_BotMatchVariable(&match, MESSAGE, message, sizeof(message));
				//if this is a message from the bot self
				if (!Q_stricmp(netname, botname)) {
					trap_BotRemoveConsoleMessage(bs->cs, handle);
					continue;
				}
				//unify the message
				trap_UnifyWhiteSpaces(message);
				//
				trap_Cvar_Update(&bot_testrchat);
				if (bot_testrchat.integer) {
					//
					trap_BotLibVarSet("bot_testrchat", "1");
					//if bot replies with a chat message
					if (trap_BotReplyChat(bs->cs, message, context, CONTEXT_REPLY,
															NULL, NULL,
															NULL, NULL,
															NULL, NULL,
															botname, netname)) {
						BotAI_Print(PRT_MESSAGE, "------------------------\n");
					}
					else {
						BotAI_Print(PRT_MESSAGE, "**** no valid reply ****\n");
					}
				}
				//if at a valid chat position and not chatting already
				else if (bs->ainode != AINode_Stand && BotValidChatPosition(bs)) {
					chat_reply = 0;
					if (random() < 1.5 / (NumBots()+1) && random() < chat_reply) {
						//if bot replies with a chat message
						if (trap_BotReplyChat(bs->cs, message, context, CONTEXT_REPLY,
																NULL, NULL,
																NULL, NULL,
																NULL, NULL,
																botname, netname)) {
							//remove the console message
							trap_BotRemoveConsoleMessage(bs->cs, handle);
							bs->stand_time = trap_AAS_Time() + BotChatTime(bs);
							AIEnter_Stand(bs);
							//EA_Say(bs->client, bs->cs.chatmessage);
							break;
						}
					}
				}
			}
		}
		//remove the console message
		trap_BotRemoveConsoleMessage(bs->cs, handle);
	}
}

/*
==================
BotCheckEvents
==================
*/
void BotCheckEvents(bot_state_t *bs, entityState_t *state) {
	int event;
	char buf[128];
	//
	//NOTE: this sucks, we're accessing the gentity_t directly
	//but there's no other fast way to do it right now
	if (bs->entityeventTime[state->number] == g_entities[state->number].eventTime) {
		return;
	}
	bs->entityeventTime[state->number] = g_entities[state->number].eventTime;
	//if it's an event only entity
	if (state->eType > ET_EVENTS) {
		event = (state->eType - ET_EVENTS) & ~EV_EVENT_BITS;
	}
	else {
		event = state->event & ~EV_EVENT_BITS;
	}
	//
	switch(event) {
		//client obituary event
		case EV_OBITUARY:
		{
			int target, attacker, mod;

			target = state->otherEntityNum;
			attacker = state->otherEntityNum2;
			mod = state->eventParm;
			//
			if (target == bs->client) {
				bs->botdeathtype = mod;
				bs->lastkilledby = attacker;
				//
				if (target == attacker) bs->botsuicide = qtrue;
				else bs->botsuicide = qfalse;
				//
				bs->num_deaths++;
			}
			//else if this client was killed by the bot
			else if (attacker == bs->client) {
				bs->enemydeathtype = mod;
				bs->lastkilledplayer = target;
				bs->killedenemy_time = trap_AAS_Time();
				//
				bs->num_kills++;
			}
			else if (attacker == bs->enemy && target == attacker) {
				bs->enemysuicide = qtrue;
			}
			break;
		}

		case EV_GLOBAL_SOUND:
		{
			if (state->eventParm < 0 || state->eventParm > MAX_SOUNDS) {
				BotAI_Print(PRT_ERROR, "EV_GLOBAL_SOUND: eventParm (%d) out of range\n", state->eventParm);
				break;
			}
			else {
				trap_GetConfigstring(CS_SOUNDS + state->eventParm, buf, sizeof(buf));
				if (!strcmp(buf, "sound/items/poweruprespawn.wav")) {
					//powerup respawned... go get it
					BotGoForPowerups(bs);
				}
			}
			break;
		}

		case EV_TEAM_SOUND:
		{
			if (state->eventParm < 0 || state->eventParm > MAX_TEAM_SOUNDS) {
				BotAI_Print(PRT_ERROR, "EV_TEAM_SOUND: eventParm (%d) out of range\n", state->eventParm);
				break;
			}

			if (state->eventParm == RETURN_FLAG_SOUND)
			{
				if (state->otherEntityNum == TEAM_RED)
				{
					//red flag is returned
					bs->redflagstatus = 0;
					bs->flagstatuschanged = qtrue;
				}
				else
				{
					//blue flag is returned
					bs->blueflagstatus = 0;
					bs->flagstatuschanged = qtrue;
				}
			}
			break;
		}



		case EV_PLAYER_TELEPORT_IN:
		{
			VectorCopy(state->origin, lastteleport_origin);
			lastteleport_time = trap_AAS_Time();
			break;
		}
		case EV_GENERAL_SOUND:
		{
			//if this sound is played on the bot
			if (state->number == bs->client) {
				if (state->eventParm < 0 || state->eventParm > MAX_SOUNDS) {
					BotAI_Print(PRT_ERROR, "EV_GENERAL_SOUND: eventParm (%d) out of range\n", state->eventParm);
					break;
				}
				//check out the sound
				trap_GetConfigstring(CS_SOUNDS + state->eventParm, buf, sizeof(buf));
				//if falling into a death pit
				if (!strcmp(buf, "*falling1.wav")) {
					//if the bot has a personal teleporter
					if (bs->inventory[INVENTORY_TRANSPORTER] > 0) {
						//use the holdable item
						trap_EA_Use(bs->client);
					}
				}
			}
			break;
		}
	}
}

/*
==================
BotCheckSnapshot
==================
*/
void BotCheckSnapshot(bot_state_t *bs) {
	int ent;
	entityState_t state;

	//
	ent = 0;
	while( ( ent = BotAI_GetSnapshotEntity( bs->client, ent, &state ) ) != -1 ) {
		//check the entity state for events
		BotCheckEvents(bs, &state);
	}
	//check the player state for events
	BotAI_GetEntityState(bs->client, &state);
	//copy the player state events to the entity state
	state.event = bs->cur_ps.externalEvent;
	state.eventParm = bs->cur_ps.externalEventParm;
	//
	BotCheckEvents(bs, &state);
}

/*
==================
BotCheckAir
==================
*/
void BotCheckAir(bot_state_t *bs) {
	if (bs->inventory[INVENTORY_ENVIRONMENTSUIT] <= 0) {
		if (trap_AAS_PointContents(bs->eye) & (CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA)) {
			return;
		}
	}
	bs->lastair_time = trap_AAS_Time();
}

/*
==================
BotDeathmatchAI
==================
*/
void BotDeathmatchAI(bot_state_t *bs, float thinktime) {
	char gender[144], name[144], buf[144];
	char userinfo[MAX_INFO_STRING];
	int i;

	//if the bot has just been setup
	if (bs->setupcount > 0) {
		bs->setupcount--;
		if (bs->setupcount > 0) return;
		//get the gender characteristic
		trap_Characteristic_String(bs->character, CHARACTERISTIC_GENDER, gender, sizeof(gender));
		//set the bot gender
		trap_GetUserinfo(bs->client, userinfo, sizeof(userinfo));
		Info_SetValueForKey(userinfo, "sex", gender);
		trap_SetUserinfo(bs->client, userinfo);
		//set the team
		if ( g_gametype.integer != GT_TOURNAMENT ) {
			Com_sprintf(buf, sizeof(buf), "team %s", bs->settings.team);
			trap_EA_Command(bs->client, buf);
		}
		//set the chat gender
		if (gender[0] == 'm') trap_BotSetChatGender(bs->cs, CHAT_GENDERMALE);
		else if (gender[0] == 'f')  trap_BotSetChatGender(bs->cs, CHAT_GENDERFEMALE);
		else  trap_BotSetChatGender(bs->cs, CHAT_GENDERLESS);
		//set the chat name
		ClientName(bs->client, name, sizeof(name));
		trap_BotSetChatName(bs->cs, name);
		//
		bs->lastframe_health = bs->inventory[INVENTORY_HEALTH];
		bs->lasthitcount = bs->cur_ps.persistant[PERS_HITS];
		//
		bs->setupcount = 0;
	}
	//no ideal view set
	bs->flags &= ~BFL_IDEALVIEWSET;
	//set the teleport time
	BotSetTeleportTime(bs);
	//update some inventory values
	BotUpdateInventory(bs);
	//check the console messages
	BotCheckConsoleMessages(bs);
	//check out the snapshot
	BotCheckSnapshot(bs);
	//check for air
	BotCheckAir(bs);
	//if not in the intermission and not in observer mode
	if (!BotIntermission(bs) && !BotIsObserver(bs)) {
		//do team AI
		BotTeamAI(bs);
	}
	//if the bot has no ai node
	if (!bs->ainode) {
		AIEnter_Seek_LTG(bs);
	}
	//if the bot entered the game less than 8 seconds ago
	if (!bs->entergamechat && bs->entergame_time > trap_AAS_Time() - 8) {
		if (BotChat_EnterGame(bs)) {
			bs->stand_time = trap_AAS_Time() + BotChatTime(bs);
			AIEnter_Stand(bs);
		}
		bs->entergamechat = qtrue;
	}
	//reset the node switches from the previous frame
	BotResetNodeSwitches();
	//execute AI nodes
	for (i = 0; i < MAX_NODESWITCHES; i++) {
		if (bs->ainode(bs)) break;
	}
	//if the bot removed itself :)
	if (!bs->inuse) return;
	//if the bot executed too many AI nodes
	if (i >= MAX_NODESWITCHES) {
		trap_BotDumpGoalStack(bs->gs);
		trap_BotDumpAvoidGoals(bs->gs);
		BotDumpNodeSwitches(bs);
		ClientName(bs->client, name, sizeof(name));
		BotAI_Print(PRT_ERROR, "%s at %1.1f switched more than %d AI nodes\n", name, trap_AAS_Time(), MAX_NODESWITCHES);
	}
	//
	bs->lastframe_health = bs->inventory[INVENTORY_HEALTH];
	bs->lasthitcount = bs->cur_ps.persistant[PERS_HITS];
}

/*
==================
BotSetupDeathmatchAI
==================
*/
void BotSetupDeathmatchAI(void) {
	int ent, modelnum;
	char model[128];

	gametype = trap_Cvar_VariableIntegerValue("g_gametype");
	maxclients = trap_Cvar_VariableIntegerValue("sv_maxclients");

	trap_Cvar_Register(&bot_rocketjump, "bot_rocketjump", "1", 0);
	trap_Cvar_Register(&bot_grapple, "bot_grapple", "0", 0);
	trap_Cvar_Register(&bot_fastchat, "bot_fastchat", "0", 0);
	trap_Cvar_Register(&bot_nochat, "bot_nochat", "0", 0);
	trap_Cvar_Register(&bot_testrchat, "bot_testrchat", "0", 0);
	trap_Cvar_Register(&bot_challenge, "bot_challenge", "0", 0);
	//
	if (gametype == GT_CTF) {
		if (trap_BotGetLevelItemGoal(-1, "Red Flag", &ctf_redflag) < 0)
			BotAI_Print(PRT_WARNING, "CTF without Red Flag\n");
		if (trap_BotGetLevelItemGoal(-1, "Blue Flag", &ctf_blueflag) < 0)
			BotAI_Print(PRT_WARNING, "CTF without Blue Flag\n");
	}

	max_bspmodelindex = 0;
	for (ent = trap_AAS_NextBSPEntity(0); ent; ent = trap_AAS_NextBSPEntity(ent)) {
		if (!trap_AAS_ValueForBSPEpairKey(ent, "model", model, sizeof(model))) continue;
		if (model[0] == '*') {
			modelnum = atoi(model+1);
			if (modelnum > max_bspmodelindex)
				max_bspmodelindex = modelnum;
		}
	}
	//initialize the waypoint heap
	BotInitWaypoints();
}

/*
==================
BotShutdownDeathmatchAI
==================
*/
void BotShutdownDeathmatchAI(void) {
}

