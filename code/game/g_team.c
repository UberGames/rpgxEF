// Copyright (C) 1999-2000 Id Software, Inc.
//

#include "g_local.h"

team_t	initialBorgTeam = TEAM_FREE;
int		borgQueenStartPoint = ENTITYNUM_NONE;

typedef enum _flag_status {
	FLAG_ATBASE = 0,
	FLAG_TAKEN,
	FLAG_DROPPED
} flagStatus_t;

typedef struct teamgame_s
{
	float last_flag_capture;
	int last_capture_team;
	flagStatus_t redStatus;
	flagStatus_t blueStatus;
} teamgame_t;

teamgame_t teamgame;

void Team_SetFlagStatus( int team, flagStatus_t status );

void Team_InitGame(void)
{
	memset(&teamgame, 0, sizeof teamgame);
	teamgame.redStatus = teamgame.blueStatus = -1; // Invalid to force update

	Team_SetFlagStatus( TEAM_RED, FLAG_ATBASE );
	Team_SetFlagStatus( TEAM_BLUE, FLAG_ATBASE );

	// set config strings for what the two teams are for use in the cgame that renders the CTF flags
	trap_SetConfigstring( CS_RED_GROUP, g_team_group_red.string);
	trap_SetConfigstring( CS_BLUE_GROUP, g_team_group_blue.string);

}

int OtherTeam(int team) {
	if (team==TEAM_RED)
		return TEAM_BLUE;
	else if (team==TEAM_BLUE)
		return TEAM_RED;
	return team;
}

const char *TeamName(int team)  {
	if (team==TEAM_RED)
		return "RED";
	else if (team==TEAM_BLUE)
		return "BLUE";
	else if (team==TEAM_SPECTATOR)
		return "SPECTATOR";
	return "FREE";
}

const char *OtherTeamName(int team) {
	if (team==TEAM_RED)
		return "BLUE";
	else if (team==TEAM_BLUE)
		return "RED";
	else if (team==TEAM_SPECTATOR)
		return "SPECTATOR";
	return "FREE";
}

const char *TeamColorString(int team) {
	if (team==TEAM_RED)
		return S_COLOR_RED;
	else if (team==TEAM_BLUE)
		return S_COLOR_BLUE;
	else if (team==TEAM_SPECTATOR)
		return S_COLOR_YELLOW;
	return S_COLOR_WHITE;
}

// NULL for everyone
void QDECL PrintMsg( gentity_t *ent, const char *fmt, ... ) {
	char		msg[1024];
	va_list		argptr;
	char		*p;
	
	va_start (argptr,fmt);
	if (vsprintf (msg, fmt, argptr) > sizeof(msg)) {
		G_Error ( "PrintMsg overrun" );
	}
	va_end (argptr);

	// double quotes are bad
	while ((p = strchr(msg, '"')) != NULL)
		*p = '\'';

	trap_SendServerCommand ( ( (ent == NULL) ? -1 : ent-g_entities ), va("print \"%s\"", msg ));
}

/*
==============
OnSameTeam
==============
*/
qboolean OnSameTeam( gentity_t *ent1, gentity_t *ent2 ) {
	if (( g_gametype.integer == GT_FFA ) || (g_gametype.integer == GT_TOURNAMENT) || (g_gametype.integer == GT_SINGLE_PLAYER))
	{
		return qfalse;
	}

	if ( !ent1->client || !ent2->client ) {
		if ( !ent1->client && !ent2->client )
		{
			if ( ent1->team && ent2->team && atoi( ent1->team ) == atoi( ent2->team ) )
			{
				return qtrue;
			}
		}
		else if ( !ent1->client )
		{
			if ( ent1->team && atoi( ent1->team ) == ent2->client->sess.sessionTeam )
			{
				return qtrue;
			}
		}
		else// if ( !ent2->client )
		{
			if ( ent2->team && ent1->client->sess.sessionTeam == atoi( ent2->team ) )
			{
				return qtrue;
			}
		}

		return qfalse;
	}

	if ( ent1->client->sess.sessionTeam == ent2->client->sess.sessionTeam ) {
		return qtrue;
	}

	return qfalse;
}

void Team_SetFlagStatus( int team, flagStatus_t status )
{
	qboolean modified = qfalse;

	switch (team) {
	case TEAM_RED :
		if ( teamgame.redStatus != status ) {
			teamgame.redStatus = status;
			modified = qtrue;
		}
		break;
	case TEAM_BLUE :
		if ( teamgame.blueStatus != status ) {
			teamgame.blueStatus = status;
			modified = qtrue;
		}
		break;
	}

	if (modified) {
		char st[4];

		st[0] = '0' + (int)teamgame.redStatus;
		st[1] = '0' + (int)teamgame.blueStatus;
		st[2] = 0;

		trap_SetConfigstring( CS_FLAGSTATUS, st );
	}
}

void Team_CheckDroppedItem( gentity_t *dropped )
{
	/*if (dropped->item->giTag == PW_REDFLAG)
		Team_SetFlagStatus( TEAM_RED, FLAG_DROPPED );
	else if (dropped->item->giTag == PW_BORG_ADAPT)
		Team_SetFlagStatus( TEAM_BLUE, FLAG_DROPPED );*/
}


/*
================
Team_FragBonuses

Calculate the bonuses for flag defense, flag carrier defense, etc.
Note that bonuses are not cumlative.  You get one, they are in importance
order.
================
*/
void Team_FragBonuses(gentity_t *targ, gentity_t *inflictor, gentity_t *attacker)
{
	//int i;
	//gentity_t *ent;
	//int flag_pw, enemy_flag_pw;
	int otherteam;
	gentity_t *flag, *carrier = NULL;
	char *c;
	vec3_t v1, v2;
	int team;

	// no bonus for fragging yourself
	if (!targ->client || attacker == NULL || !attacker->client || targ == attacker)
		return;

	team = targ->client->sess.sessionTeam;
	otherteam = OtherTeam(targ->client->sess.sessionTeam);
	if (otherteam < 0)
		return; // whoever died isn't on a team

	// same team, if the flag at base, check to he has the enemy flag
	/*if (team == TEAM_RED) {
		flag_pw = PW_REDFLAG;
		enemy_flag_pw = PW_REDFLAG;
	} else {
		flag_pw = PW_REDFLAG;
		enemy_flag_pw = PW_REDFLAG;
	}*/

	// did the attacker frag the flag carrier?
	/*if (targ->client->ps.powerups[enemy_flag_pw]) {
		attacker->client->pers.teamState.lastfraggedcarrier = level.time;
		AddScore(attacker, CTF_FRAG_CARRIER_BONUS);
		attacker->client->pers.teamState.fragcarrier++;
		PrintMsg(NULL, "%s" S_COLOR_WHITE " eliminated %s's flag carrier!\n",
			attacker->client->pers.netname, TeamName(team));

		// the target had the flag, clear the hurt carrier
		// field on the other team
		for (i = 0; i < g_maxclients.integer; i++) {
			ent = g_entities + i;
			if (ent->inuse && ent->client->sess.sessionTeam == otherteam)
				ent->client->pers.teamState.lasthurtcarrier = 0;
		}
		return;
	}*/

	if (targ->client->pers.teamState.lasthurtcarrier &&
		level.time - targ->client->pers.teamState.lasthurtcarrier < CTF_CARRIER_DANGER_PROTECT_TIMEOUT /*&&
		!attacker->client->ps.powerups[flag_pw]*/) {
		// attacker is on the same team as the flag carrier and
		// fragged a guy who hurt our flag carrier
		AddScore(attacker, CTF_CARRIER_DANGER_PROTECT_BONUS);

		attacker->client->pers.teamState.carrierdefense++;
		targ->client->pers.teamState.lasthurtcarrier = 0;

		team = attacker->client->sess.sessionTeam;
		PrintMsg(NULL, "%s" S_COLOR_WHITE " defends %s's flag carrier against an aggressive enemy\n",
			attacker->client->pers.netname, TeamName(team));
		return;
	}

	// flag and flag carrier area defense bonuses

	// we have to find the flag and carrier entities

	// find the flag
	switch (attacker->client->sess.sessionTeam) {
	case TEAM_RED:
		c = "team_CTF_redflag";
		break;
	case TEAM_BLUE:
		c = "team_CTF_blueflag";
		break;		
	default:
		return;
	}

	flag = NULL;
	while ((flag = G_Find (flag, FOFS(classname), c)) != NULL) {
		if (!(flag->flags & FL_DROPPED_ITEM))
			break;
	}

	if (!flag)
		return; // can't find attacker's flag

	// find attacker's team's flag carrier
	/*for (i = 0; i < g_maxclients.integer; i++) {
		carrier = g_entities + i;
		if (carrier->inuse && carrier->client->ps.powerups[flag_pw])
			break;
		carrier = NULL;
	}*/

	// ok we have the attackers flag and a pointer to the carrier

	// check to see if we are defending the base's flag
	VectorSubtract(targ->r.currentOrigin, flag->r.currentOrigin, v1);
	VectorSubtract(attacker->r.currentOrigin, flag->r.currentOrigin, v2);

	if ( ( ( VectorLength(v1) < CTF_TARGET_PROTECT_RADIUS &&
		trap_InPVS(flag->r.currentOrigin, targ->r.currentOrigin ) ) ||
		( VectorLength(v2) < CTF_TARGET_PROTECT_RADIUS &&
		trap_InPVS(flag->r.currentOrigin, attacker->r.currentOrigin ) ) ) &&
		attacker->client->sess.sessionTeam != targ->client->sess.sessionTeam) {

		// we defended the base flag
		AddScore(attacker, CTF_FLAG_DEFENSE_BONUS);
		attacker->client->pers.teamState.basedefense++;
		if (flag->r.svFlags & SVF_NOCLIENT) {
			PrintMsg(NULL, "%s" S_COLOR_WHITE " defends the %s base.\n",
				attacker->client->pers.netname, 
				TeamName(attacker->client->sess.sessionTeam));
		} else {
			PrintMsg(NULL, "%s" S_COLOR_WHITE " defends the %s flag.\n",
				attacker->client->pers.netname, 
				TeamName(attacker->client->sess.sessionTeam));
		}
		return;
	}

	if (carrier && carrier != attacker) {
		VectorSubtract(targ->r.currentOrigin, carrier->r.currentOrigin, v1);
		VectorSubtract(attacker->r.currentOrigin, carrier->r.currentOrigin, v1);

		if ( ( ( VectorLength(v1) < CTF_ATTACKER_PROTECT_RADIUS &&
			trap_InPVS(carrier->r.currentOrigin, targ->r.currentOrigin ) ) ||
			( VectorLength(v2) < CTF_ATTACKER_PROTECT_RADIUS &&
				trap_InPVS(carrier->r.currentOrigin, attacker->r.currentOrigin ) ) ) &&
			attacker->client->sess.sessionTeam != targ->client->sess.sessionTeam) {
			AddScore(attacker, CTF_CARRIER_PROTECT_BONUS);
			attacker->client->pers.teamState.carrierdefense++;
			PrintMsg(NULL, "%s" S_COLOR_WHITE " defends the %s's flag carrier.\n",
				attacker->client->pers.netname, 
				TeamName(attacker->client->sess.sessionTeam));
			return;
		}
	}
}

/*
================
Team_CheckHurtCarrier

Check to see if attacker hurt the flag carrier.  Needed when handing out bonuses for assistance to flag
carrier defense.
================
*/
void Team_CheckHurtCarrier(gentity_t *targ, gentity_t *attacker)
{
	//int flag_pw;

	if (!targ->client || !attacker->client)
		return;

	/*if (targ->client->sess.sessionTeam == TEAM_RED)
		flag_pw = PW_REDFLAG;
	else
		flag_pw = PW_REDFLAG;*/

	/*if (targ->client->ps.powerups[flag_pw] &&
		targ->client->sess.sessionTeam != attacker->client->sess.sessionTeam)
		attacker->client->pers.teamState.lasthurtcarrier = level.time;*/
}


gentity_t *Team_ResetFlag(int team)
{
	char *c;
	gentity_t *ent, *rent = NULL;

	switch (team) {
	case TEAM_RED:
		c = "team_CTF_redflag";
		break;
	case TEAM_BLUE:
		c = "team_CTF_blueflag";
		break;
	default:
		return NULL;
	}

	ent = NULL;
	while ((ent = G_Find (ent, FOFS(classname), c)) != NULL) {
		if (ent->flags & FL_DROPPED_ITEM)
			G_FreeEntity(ent);
		else {
			rent = ent;
			RespawnItem(ent);
		}
	}

	Team_SetFlagStatus( team, FLAG_ATBASE );

	return rent;
}

void Team_ResetFlags(void)
{
	Team_ResetFlag(TEAM_RED);
	Team_ResetFlag(TEAM_BLUE);
}

void Team_ReturnFlagSound(gentity_t *ent, int team)
{
	// play powerup spawn sound to all clients
	gentity_t	*te;

	if (ent == NULL) {
		G_Printf ("Warning:  NULL passed to Team_ReturnFlagSound\n");
		return;
	}


	te = G_TempEntity( ent->s.pos.trBase, EV_TEAM_SOUND );
	te->s.eventParm = RETURN_FLAG_SOUND;
	te->s.otherEntityNum = team;

	te->r.svFlags |= SVF_BROADCAST;
}

void Team_ReturnFlag(int team)
{
	Team_ReturnFlagSound(Team_ResetFlag(team), team);
	PrintMsg(NULL, "The %s flag has returned!\n", TeamName(team));
}

void Team_FreeEntity(gentity_t *ent)
{
	/*if (ent->item->giTag == PW_REDFLAG)
		Team_ReturnFlag(TEAM_RED);
	else if (ent->item->giTag == PW_BORG_ADAPT)
		Team_ReturnFlag(TEAM_BLUE);*/
}

/*
==============
Team_DroppedFlagThink

Automatically set in Launch_Item if the item is one of the flags

Flags are unique in that if they are dropped, the base flag must be respawned when they time out
==============
*/
void Team_DroppedFlagThink(gentity_t *ent)
{
	/*if (ent->item->giTag == PW_REDFLAG)
		Team_ReturnFlagSound(Team_ResetFlag(TEAM_RED), TEAM_RED);
	else if (ent->item->giTag == PW_BORG_ADAPT)
		Team_ReturnFlagSound(Team_ResetFlag(TEAM_BLUE), TEAM_BLUE);*/
	// Reset Flag will delete this entity
}

void Team_AddScore( int team, int points )
{
	teamgame.last_flag_capture = level.time;
	teamgame.last_capture_team = team;

	// Increase the team's score
	level.teamScores[team] += points;
}

int Team_TouchOurFlag( gentity_t *ent, gentity_t *other, int team ) {
	int i;
	gentity_t *player;
	gclient_t *cl = other->client;
	//int our_flag;
	//int enemy_flag;
	gentity_t	*te;

	/*if (cl->sess.sessionTeam == TEAM_RED) {
		our_flag = PW_REDFLAG;
		enemy_flag = PW_REDFLAG;
	} else {
		our_flag = PW_REDFLAG;
		enemy_flag = PW_REDFLAG;
	}*/

	if ( ent->flags & FL_DROPPED_ITEM ) {
		// hey, its not home.  return it by teleporting it back
		PrintMsg( NULL, "%s" S_COLOR_WHITE " returned the %s flag!\n", 
			cl->pers.netname, TeamName(team));
		AddScore(other, CTF_RECOVERY_BONUS);
		other->client->pers.teamState.flagrecovery++;
		other->client->pers.teamState.lastreturnedflag = level.time;
		//ResetFlag will remove this entity!  We must return zero
		Team_ReturnFlagSound(Team_ResetFlag(team), team);
		return 0;
	}

	// the flag is at home base.  if the player has the enemy
	// flag, he's just won!
	//if (!cl->ps.powerups[enemy_flag])
	//	return 0; // We don't have the flag

	PrintMsg( NULL, "%s" S_COLOR_WHITE " captured the %s flag!\n",
		cl->pers.netname, TeamName(OtherTeam(team)));

	//cl->ps.powerups[enemy_flag] = 0;

	Team_AddScore( team, 1 );

	other->client->pers.teamState.captures++;

	// other gets another 10 frag bonus
	AddScore(other, CTF_CAPTURE_BONUS);

	// If this is the last capture of the match, don't play the capture sound.
	if (level.teamScores[other->client->sess.sessionTeam ] == g_capturelimit.integer)
	{
		te = G_TempEntity( ent->s.pos.trBase, EV_TEAM_SOUND );
		te->s.eventParm = SCORED_FLAG_NO_VOICE_SOUND;
		te->s.otherEntityNum = cl->sess.sessionTeam;
		te->r.svFlags |= SVF_BROADCAST;
	}
	else
	{
		te = G_TempEntity( ent->s.pos.trBase, EV_TEAM_SOUND );
		te->s.eventParm = SCORED_FLAG_SOUND;
		te->s.otherEntityNum = cl->sess.sessionTeam;
		te->r.svFlags |= SVF_BROADCAST;
	}

	// Ok, let's do the player loop, hand out the bonuses
	for (i = 0; i < g_maxclients.integer; i++) {
		player = &g_entities[i];
		if (!player->inuse)
			continue;

		if (player->client->sess.sessionTeam !=
			cl->sess.sessionTeam) {
			player->client->pers.teamState.lasthurtcarrier = -5;
		} else if (player->client->sess.sessionTeam ==
			cl->sess.sessionTeam) {
			if (player != other)
				AddScore(player, CTF_TEAM_BONUS);
			// award extra points for capture assists
			if (player->client->pers.teamState.lastreturnedflag + 
				CTF_RETURN_FLAG_ASSIST_TIMEOUT > level.time) {
				PrintMsg( NULL, 
						"%s" S_COLOR_WHITE " gets an assist for returning the %s flag!\n", 
						player->client->pers.netname,
						TeamName(team));
				AddScore (player, CTF_RETURN_FLAG_ASSIST_BONUS);
				other->client->pers.teamState.assists++;
			}
			if (player->client->pers.teamState.lastfraggedcarrier + 
				CTF_FRAG_CARRIER_ASSIST_TIMEOUT > level.time) {
				PrintMsg( NULL, "%s" S_COLOR_WHITE " gets an assist for eliminating the %s flag carrier!\n", 
						player->client->pers.netname,
						TeamName(OtherTeam(team)));
				AddScore(player, CTF_FRAG_CARRIER_ASSIST_BONUS);
				other->client->pers.teamState.assists++;
			}
		}
	}
	Team_ResetFlags();

	CalculateRanks( qfalse );

	return 0; // Do not respawn this automatically
}

int Team_TouchEnemyFlag( gentity_t *ent, gentity_t *other, int team ) {
	gclient_t *cl = other->client;

	// hey, its not our flag, pick it up
	PrintMsg (NULL, "%s" S_COLOR_WHITE " got the %s flag!\n",
		other->client->pers.netname, TeamName(team));
	AddScore(other, CTF_FLAG_BONUS);

	//if (team == TEAM_RED)
		//cl->ps.powerups[PW_REDFLAG] = INT_MAX; // flags never expire
	//else
		//cl->ps.powerups[PW_BORG_ADAPT] = INT_MAX; // flags never expire

	cl->pers.teamState.flagsince = level.time;

	Team_SetFlagStatus( team, FLAG_TAKEN );
	
	return -1; // Do not respawn this automatically, but do delete it if it was FL_DROPPED
}

int Pickup_Team( gentity_t *ent, gentity_t *other ) {
	int team;
	gclient_t *cl = other->client;
	
	// figure out what team this flag is
	if (strcmp(ent->classname, "team_CTF_redflag") == 0)
		team = TEAM_RED;
	else if (strcmp(ent->classname, "team_CTF_blueflag") == 0)
		team = TEAM_BLUE;
	else {
		PrintMsg ( other, "Don't know what team the flag is on.\n");
		return 0;
	}

	return ((team == cl->sess.sessionTeam) ?
		Team_TouchOurFlag : Team_TouchEnemyFlag)
			(ent, other, team);
}

/*
===========
Team_GetLocation

Report a location for the player. Uses placed nearby target_location entities
============
*/
gentity_t *Team_GetLocation(gentity_t *ent)
{
	gentity_t		*eloc, *best;
	float			bestlen, len;
	vec3_t			origin;

	best = NULL;
	bestlen = 3*8192.0*8192.0;

	VectorCopy( ent->r.currentOrigin, origin );

	for (eloc = level.locationHead; eloc; eloc = eloc->nextTrain) {
		len = ( origin[0] - eloc->r.currentOrigin[0] ) * ( origin[0] - eloc->r.currentOrigin[0] )
			+ ( origin[1] - eloc->r.currentOrigin[1] ) * ( origin[1] - eloc->r.currentOrigin[1] )
			+ ( origin[2] - eloc->r.currentOrigin[2] ) * ( origin[2] - eloc->r.currentOrigin[2] );

		if ( len > bestlen ) {
			continue;
		}

		if ( !trap_InPVS( origin, eloc->r.currentOrigin ) ) {
			continue;
		}

		bestlen = len;
		best = eloc;
	}

	return best;
}


/*
===========
Team_GetLocation

Report a location for the player. Uses placed nearby target_location entities
============
*/
qboolean Team_GetLocationMsg(gentity_t *ent, char *loc, int loclen)
{
	gentity_t *best;

	best = Team_GetLocation( ent );
	
	if (!best)
		return qfalse;

	if (best->count) {
		if (best->count < 0)
			best->count = 0;
		if (best->count > 7)
			best->count = 7;
		Com_sprintf(loc, loclen, "%c%c%s" S_COLOR_WHITE, Q_COLOR_ESCAPE, best->count + '0', best->message );
	} else
		Com_sprintf(loc, loclen, "%s", best->message);

	return qtrue;
}


/*---------------------------------------------------------------------------*/

/*
================
SelectRandomDeathmatchSpawnPoint

go to a random point that doesn't telefrag
================
*/
#define	MAX_TEAM_SPAWN_POINTS	32
gentity_t *SelectRandomTeamSpawnPoint( gentity_t *ent, int teamstate, team_t team ) {
	gentity_t	*spot;
	int			count;
	int			selection;
	gentity_t	*spots[MAX_TEAM_SPAWN_POINTS];
	char		*classname;

	if (teamstate == TEAM_BEGIN) {
		if (team == TEAM_RED)
			classname = "team_CTF_redplayer";
		else if (team == TEAM_BLUE)
			classname = "team_CTF_blueplayer";
		else
			return NULL;
	} else {
		if (team == TEAM_RED)
			classname = "team_CTF_redspawn";
		else if (team == TEAM_BLUE)
			classname = "team_CTF_bluespawn";
		else
			return NULL;
	}
	count = 0;

	spot = NULL;

	while ((spot = G_Find (spot, FOFS(classname), classname)) != NULL) {
		if ( teamstate != TEAM_BEGIN) {
			if ( spot->spawnflags & 1 ) {
				//not an active spawn point
				continue;
			}
		}
		if ( SpotWouldTelefrag( spot ) ) {
			continue;
		}
		spots[ count ] = spot;
		if (++count == MAX_TEAM_SPAWN_POINTS)
			break;
	}

	if ( !count ) {	// no spots that won't telefrag
		return G_Find( NULL, FOFS(classname), classname);
	}

	selection = rand() % count;
	return spots[ selection ];
}


/*
===========
SelectCTFSpawnPoint

============
*/
gentity_t *SelectCTFSpawnPoint ( gentity_t *ent, team_t team, int teamstate, vec3_t origin, vec3_t angles ) {
	gentity_t	*spot;

	spot = SelectRandomTeamSpawnPoint ( ent, teamstate, team );

	if (!spot) {
		return SelectSpawnPoint( vec3_origin, origin, angles );
	}

	VectorCopy (spot->s.origin, origin);
	origin[2] += 9;
	VectorCopy (spot->s.angles, angles);

	return spot;
}

/*---------------------------------------------------------------------------*/
/*static int QDECL SortClients( const void *a, const void *b ) {
	return *(int *)a - *(int *)b;
}*/

/*
==================
CheckHealthInfoMessage

Sends Health Changes to proper clients

Format:
	clientNum health

==================
*/
void CheckHealthInfoMessage( void ) 
{
	char		entry[1024];
	char		string[1400];
	int			stringlength;
	int			i, j, t;
	gentity_t	*player, *ent;
	int			sendToCnt, cnt, sentCnt;
	int			h;
	int			clients[MAX_CLIENTS];
	int			sendToClients[MAX_CLIENTS];

	//only send this to medics or spectators or adminz
	for (i = 0, sendToCnt = 0; i < g_maxclients.integer; i++) 
	{

		if ( level.clients[i].pers.connected == CON_CONNECTED && level.clients[i].ps.stats[STAT_HEALTH] > 0 &&//make sure they've actually spawned in already
			(level.clients[i].sess.sessionTeam == TEAM_SPECTATOR || g_classData[level.clients[i].sess.sessionClass].isMedical || g_classData[level.clients[i].sess.sessionClass].isAdmin ) )
		{
			sendToClients[sendToCnt++] = i;
		}
	}

	if ( !sendToCnt )
	{//no-one to send to
		return;
	}

	//only send those clients whose health has changed this cycle
	//NB: there's a prob with client 0 in here....
	for (i = 0, cnt = 0; i < g_maxclients.integer; i++) 
	{
		player = g_entities + i;
		if ( player->inuse && player->old_health != player->health && ( player->health > 0 || player->old_health > 0 )) 
		{
			clients[cnt++] = i;
			player->old_health = player->health;
		}
	}

	if ( !cnt )
	{//no-one relevant changed health
		return;
	}

	for ( t = 0; t < sendToCnt; t++ )
	{
		ent = g_entities + sendToClients[t];
		sentCnt = 0;

		// send the latest information on all clients
		string[0] = 0;
		stringlength = 0;

		for (i = 0; i < cnt; i++) 
		{
			player = g_entities + clients[i];
			//RPG-X | Phenix | 05/03/2005
			/*if ( ent->client->sess.sessionTeam == player->client->sess.sessionTeam && ent->client->pers.teamInfo )
			{//already sent this player's health to this client because teaminfo is on
				continue;
			}*/

			if ( ent == player )
			{//don't send the ent his own health
				continue;
			}

			//send this one
			sentCnt++;

			h = player->health;
			if (h < 0) h = 0;

			Com_sprintf (entry, sizeof(entry), " %i %i", clients[i], h);
			j = strlen(entry);
			if (stringlength + j > sizeof(string))
				break;
			strcpy (string + stringlength, entry);
			stringlength += j;
		}

		if ( sentCnt )
		{
			trap_SendServerCommand( sendToClients[t], va("hinfo %i%s", sentCnt, string) );
			//trap_SendServerCommand( -1, va( "print \"^1hinfo %i%s\n\"", sentCnt, string ));
		}
	}
}

/*
==================
TeamplayLocationsMessage

Format:
	clientNum location health armor weapon powerups

==================
*/
void TeamplayInfoMessage( gentity_t *ent ) {
	char		entry[1024];
	char		string[1400];
	int			stringlength;
	int			i, j;
	gentity_t	*player;
	int			cnt;
	//int			h, a;
	int			clients[TEAM_MAXOVERLAY];

	//TiM : Send data regardless
	/*if ( ! ent->client->pers.teamInfo )
		return;*/

	//don't bother sending during intermission?
	if ( level.intermissiontime )
		return;

	// figure out what client should be on the display
	// we are limited to 8, but we want to use the top eight players
	// but in client order (so they don't keep changing position on the overlay)
	for (i = 0, cnt = 0; i < g_maxclients.integer && cnt < TEAM_MAXOVERLAY; i++) {
		player = g_entities + level.sortedClients[i];
		if (player->inuse && player->client->sess.sessionTeam == 
			ent->client->sess.sessionTeam ) {
			clients[cnt++] = level.sortedClients[i];
		}
	}

	// We have the top eight players, sort them by clientNum
	//TiM
	//qsort( clients, cnt, sizeof( clients[0] ), SortClients );

	// send the latest information on all clients
	string[0] = 0;
	stringlength = 0;

	for (i = 0, cnt = 0; i < g_maxclients.integer && cnt < TEAM_MAXOVERLAY; i++) {
		player = g_entities + i;
//RPG-X | Phenix | 05/03/2005
		if (player->inuse /*&& player->client->sess.sessionTeam == 
			ent->client->sess.sessionTeam*/ ) {

			/*h = player->client->ps.stats[STAT_HEALTH];
			a = player->client->ps.stats[STAT_ARMOR];
			if (h < 0) h = 0;
			if (a < 0) a = 0;*/

			//to counter for the fact we could pwn the server doing this, remove all superfluous data

			Com_sprintf (entry, sizeof(entry),
				" %i %i ", //%i %i %i %i
//				level.sortedClients[i], player->client->pers.teamState.location, h, a, 
				i, player->client->pers.teamState.location/*, h, a, 
				player->client->ps.weapon, player->s.powerups*/);
			j = strlen(entry);
			if (stringlength + j > sizeof(string))
				break;
			strcpy (string + stringlength, entry);
			stringlength += j;
			cnt++;
		}
	}

	trap_SendServerCommand( ent-g_entities, va("tinfo %i%s", cnt, string) );
}

//TiM - Modified to work with RPG-X
void CheckTeamStatus(void)
{
	int i;
	gentity_t *loc, *ent;

	if (level.time - level.lastTeamLocationTime > TEAM_LOCATION_UPDATE_TIME) {

		level.lastTeamLocationTime = level.time;

		for (i = 0; i < g_maxclients.integer; i++) {
			ent = g_entities + i;
			if (ent->inuse /*&& 
				(ent->client->sess.sessionTeam == TEAM_RED ||
				ent->client->sess.sessionTeam == TEAM_BLUE)*/ ) {
				loc = Team_GetLocation( ent );
				if (loc)
					ent->client->pers.teamState.location = loc->health;
				else
					ent->client->pers.teamState.location = 0;
			}
		}

		for (i = 0; i < g_maxclients.integer; i++) {
			ent = g_entities + i;
			if (ent->inuse /*&& 
				(ent->client->sess.sessionTeam == TEAM_RED ||
				ent->client->sess.sessionTeam == TEAM_BLUE)*/) {
				TeamplayInfoMessage( ent );
			}
		}

		CheckHealthInfoMessage();
	}
}

/*-----------------------------------------------------------------*/

/*QUAKED team_CTF_redplayer (1 0 0) (-16 -16 -16) (16 16 32) BORGQUEEN
Only in CTF games.  Red players spawn here at game start.

BORGQUEEN - The player that is the Borg Queen will spawn here
*/
void SP_team_CTF_redplayer( gentity_t *ent ) {
	if ( ent->spawnflags & 1 )
	{
		initialBorgTeam = TEAM_RED;
		borgQueenStartPoint = ent->s.number;
	}
}


/*QUAKED team_CTF_blueplayer (0 0 1) (-16 -16 -16) (16 16 32) BORGQUEEN
Only in CTF games.  Blue players spawn here at game start.

BORGQUEEN - The player that is the Borg Queen will spawn here
*/
void SP_team_CTF_blueplayer( gentity_t *ent ) {
	if ( ent->spawnflags & 1 )
	{
		initialBorgTeam = TEAM_BLUE;
		borgQueenStartPoint = ent->s.number;
	}
}


void spawnpoint_toggle_active( gentity_t *ent, gentity_t *other, gentity_t *activator )
{
	ent->spawnflags ^= 1;
}
/*QUAKED team_CTF_redspawn (1 0 0) (-16 -16 -24) (16 16 32) STARTOFF
potential spawning position for red team in CTF games, AFTER game start
Targets will be fired when someone spawns in on them.

STARTOFF - won't be considered as a spawn point until used

targetname - when used, toggles between active and incative spawn point
*/
void SP_team_CTF_redspawn(gentity_t *ent) {
	if ( ent->targetname )
	{
		ent->use = spawnpoint_toggle_active;
	}
}

/*QUAKED team_CTF_bluespawn (0 0 1) (-16 -16 -24) (16 16 32) STARTOFF
potential spawning position for blue team in CTF games, AFTER game start
Targets will be fired when someone spawns in on them.

STARTOFF - won't be considered as a spawn point until used

targetname - when used, toggles between active and incative spawn point
*/
void SP_team_CTF_bluespawn(gentity_t *ent) {
	if ( ent->targetname )
	{
		ent->use = spawnpoint_toggle_active;
	}
}



