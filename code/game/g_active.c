// Copyright (C) 1999-2000 Id Software, Inc.
//

#include "g_local.h"

extern void SP_misc_ammo_station( gentity_t *ent );
extern void ammo_station_finish_spawning ( gentity_t *self );
//extern qboolean BG_BorgTransporting( playerState_t *ps );

static int lastTimedMessage; //TiM - Moved here from g_local.h

/*
==============
TryUse
==============
*/

#define USE_DISTANCE	64.0f
/**
*	Try and use an entity in the world, directly ahead of us
*/
void TryUse( gentity_t *ent )
{
	gentity_t	*target; //, *newent;
	trace_t		trace;
	vec3_t		src, dest, vf;
	clientSession_t *sess;
	
	if(!ent || !ent->client) return;

	sess = &ent->client->sess;

	VectorCopy( ent->r.currentOrigin, src );
	src[2] += ent->client->ps.viewheight * ent->client->pers.pms_height; //TiM - include height offset for tall players

	AngleVectors( ent->client->ps.viewangles, vf, NULL, NULL );

	//extend to find end of use trace
	VectorMA( src, -6, vf, src );//in case we're inside something?
	VectorMA( src, 134, vf, dest );//128+6

	//Trace ahead to find a valid target
	trap_Trace( &trace, src, vec3_origin, vec3_origin, dest, ent->s.number, MASK_OPAQUE|CONTENTS_BODY|CONTENTS_ITEM|CONTENTS_CORPSE );

	if ( trace.fraction == 1.0f || trace.entityNum < 0 )
	{
		//FIXME: Play a failure sound
		return;
	}

	target = &g_entities[trace.entityNum];

	//Check for a use command
	if ( target && target->use && Q_stricmp("func_usable", target->classname) == 0 )
	{//usable brush
		if ( target->team && atoi( target->team ) != 0 )
		{//usable has a team
			if ( atoi( target->team ) != sess->sessionTeam )
			{//not on my team
				//if ( sess->sessionClass != PC_TECH )
				//{//only a tech can use enemy usables
				//	//FIXME: Play a failure sound
				//	return;
				//}

				//TiM - return
				return;
			}
		}
		//FIXME: play sound?
		target->use( target, ent, ent );
		#ifdef G_LUA
		if(target->luaUse)
			LuaHook_G_EntityUse(target->luaUse, target-g_entities, ent-g_entities, ent-g_entities);
		#endif
		return;
	}
	else if ( target && target->use && Q_stricmp("misc_ammo_station", target->classname) == 0 )
	{//ammo station
		if ( sess->sessionTeam )
		{
			if ( target->team )
			{
				if ( atoi( target->team ) != sess->sessionTeam )
				{
					//FIXME: play sound?
					return;
				}
			}
		}
		target->use( target, ent, ent );
		#ifdef G_LUA
		if(target->luaUse)
			LuaHook_G_EntityUse(target->luaUse, target-g_entities, ent-g_entities, ent-g_entities);
		#endif
		return;
	}
	else if ( (target && target->s.number == ENTITYNUM_WORLD) || (target->s.pos.trType == TR_STATIONARY && !(trace.surfaceFlags & SURF_NOIMPACT) && !target->takedamage) )
	{
		//switch( ent->client->sess.sessionClass )
		//{
		//case PC_TECH://tech can place an ammo station
		//	break;
		//}
		//FIXME: play sound
		return;
	}
	//FIXME: Play a failure sound
}

/*
===============
G_DamageFeedback
===============
*/
/**
*	Called just before a snapshot is sent to the given player.
*	Totals up all damage and generates both the player_state_t
*	damage values to that client for pain blends and kicks, and
*	global pain sound events for all clients.
*/
void P_DamageFeedback( gentity_t *player ) {
	gclient_t	*client;
	float	count;
	vec3_t	angles;
	playerState_t *ps;

	client = player->client;
	ps = &client->ps;
	if ( client->ps.pm_type == PM_DEAD ) {
		return;
	}

	// total points of damage shot at the player this frame
	count = client->damage_blood + client->damage_armor;
	if ( count == 0 ) {
		return;		// didn't take any damage
	}

	if ( count > 255 ) {
		count = 255;
	}

	// send the information to the client

	// world damage (falling, slime, etc) uses a special code
	// to make the blend blob centered instead of positional
	if ( client->damage_fromWorld ) {
		ps->damagePitch = 255;
		ps->damageYaw = 255;

		client->damage_fromWorld = qfalse;
	} else {
		vectoangles( client->damage_from, angles );
		ps->damagePitch = angles[PITCH]/360.0 * 256;
		ps->damageYaw = angles[YAW]/360.0 * 256;
	}

	// play an apropriate pain sound
	if ( (level.time > player->pain_debounce_time) && !(player->flags & FL_GODMODE) ) {
		player->pain_debounce_time = level.time + 700;
		G_AddEvent( player, EV_PAIN, player->health );
		ps->damageEvent++;
	}

	ps->damageCount = client->damage_blood;
	if (ps->damageCount > 255)
	{
		ps->damageCount = 255;
	}

	ps->damageShieldCount = client->damage_armor;
	if (ps->damageShieldCount > 255)
	{
		ps->damageShieldCount = 255;
	}

	//
	// clear totals
	//
	client->damage_blood = 0;
	client->damage_armor = 0;
	client->damage_knockback = 0;
}



/*
=============
P_WorldEffects
=============
*/
/**
*	Check for lava / slime contents and drowning
*/
void P_WorldEffects( gentity_t *ent ) {
	//qboolean	envirosuit;
	int			waterlevel;

	if ( ent->client->noclip ) {
		ent->client->airOutTime = level.time + 12000;	// don't need air
		return;
	}

	waterlevel = ent->waterlevel;

	//envirosuit = ent->client->ps.powerups[PW_BOLTON] > level.time;

	//
	// check for drowning
	//
	if ( waterlevel == 3 && !(ent->watertype&CONTENTS_LADDER)) {
		// envirosuit give air, techs can't drown
		if ( /*envirosuit ||*/ g_classData[ent->client->sess.sessionClass].isMarine/*ent->client->sess.sessionClass == PC_ALPHAOMEGA22*/ ) {
			ent->client->airOutTime = level.time + 10000;
		}

		// if out of air, start drowning
		if ( ent->client->airOutTime < level.time) {
			// drown!
			ent->client->airOutTime += 1000;
			if ( ent->health > 1 ) { //TiM : used to be 0, but to fix red's medic code
				// take more damage the longer underwater
				ent->damage += 2;
				if (ent->damage > 15)
					ent->damage = 15;

				// play a gurp sound instead of a normal pain sound
				if (ent->health <= ent->damage) {
					G_Sound(ent, G_SoundIndex("*drown.wav"));
				} else if (rand()&1) {
					G_Sound(ent, G_SoundIndex("sound/player/gurp1.wav"));
				} else {
					G_Sound(ent, G_SoundIndex("sound/player/gurp2.wav"));
				}

				// don't play a normal pain sound
				ent->pain_debounce_time = level.time + 200;

				G_Damage (ent, NULL, NULL, NULL, NULL,
					ent->damage, DAMAGE_NO_ARMOR, MOD_WATER);
			}
		}
	} else {
		ent->client->airOutTime = level.time + 12000;
		ent->damage = 2;
	}

	//
	// check for sizzle damage (move to pmove?)
	//
	if (waterlevel &&
		(ent->watertype&(CONTENTS_LAVA|CONTENTS_SLIME)) ) {
		if (ent->health > 0
			&& ent->pain_debounce_time < level.time	) {

/*			if ( envirosuit ) {
				G_AddEvent( ent, EV_POWERUP_BATTLESUIT, 0 );
			} else {*/
				if (ent->watertype & CONTENTS_LAVA) {
					G_Damage (ent, NULL, NULL, NULL, NULL,
						30*waterlevel, 0, MOD_LAVA);
				}

				if (ent->watertype & CONTENTS_SLIME) {
					G_Damage (ent, NULL, NULL, NULL, NULL,
						10*waterlevel, 0, MOD_SLIME);
				}
			//}
		}
	}
}



/*
===============
G_SetClientSound
===============
*/
void G_SetClientSound( gentity_t *ent )
{	// 3/28/00 kef -- this is dumb.
	if (ent->waterlevel && (ent->watertype&(CONTENTS_LAVA|CONTENTS_SLIME)) )
		ent->s.loopSound = level.snd_fry;
	else
		ent->s.loopSound = 0;
}



//==============================================================

/*
==============
ClientImpacts
==============
*/
void ClientImpacts( gentity_t *ent, pmove_t *pm ) {
	int		i, j;
	trace_t	trace;
	gentity_t	*other;

	memset( &trace, 0, sizeof( trace ) );
	for (i=0 ; i<pm->numtouch ; i++) {
		for (j=0 ; j<i ; j++) {
			if (pm->touchents[j] == pm->touchents[i] ) {
				break;
			}
		}
		if (j != i) {
			continue;	// duplicated
		}
		other = &g_entities[ pm->touchents[i] ];

		if ( ( ent->r.svFlags & SVF_BOT ) && ( ent->touch ) ) {
			ent->touch( ent, other, &trace );
		}

		if ( !other->touch ) {
			continue;
		}

		other->touch( other, ent, &trace );
	}

}

/*
============
G_TouchTriggers
============
*/
/**
*	Find all trigger entities that ent's current position touches.
*	Spectators will only interact with teleporters.
*/
void	G_TouchTriggers( gentity_t *ent ) {
	int			i, num;
	int			touch[MAX_GENTITIES];
	gentity_t	*hit;
	trace_t		trace;
	vec3_t		mins, maxs;
	static vec3_t	range = { 40, 40, 52 };
	playerState_t *ps;

	if ( !ent || !ent->client ) {
		return;
	}

	ps = &ent->client->ps;

	// dead clients don't activate triggers!
	if ( ps->stats[STAT_HEALTH] <= 0 ) {
		return;
	}

	VectorSubtract( ps->origin, range, mins );
	VectorAdd( ps->origin, range, maxs );

	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	// can't use ent->absmin, because that has a one unit pad
	VectorAdd( ps->origin, ent->r.mins, mins );
	VectorAdd( ps->origin, ent->r.maxs, maxs );

	for ( i=0 ; i<num ; i++ ) {
		hit = &g_entities[touch[i]];

		#ifdef G_LUA
		if(hit->luaTouch)
		{
			LuaHook_G_EntityTouch(hit->luaTouch, hit->s.number, ent->s.number);
		}
		#endif

		if ( !hit->touch && !ent->touch ) {
			continue;
		}
		if ( !( hit->r.contents & CONTENTS_TRIGGER ) ) {
			continue;
		}

		// ignore most entities if a spectator
		if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR /*|| (ps->eFlags&EF_ELIMINATED)*/ )
		{
			//RPG-X: J2J - No clip spectating doesnt need to use transporter entities or door triggers!
//			if( rpg_noclipspectating.integer == 1 )
//				continue;
				
			// this is ugly but adding a new ET_? type will
			// most likely cause network incompatibilities
			if ( hit->s.eType != ET_TELEPORT_TRIGGER &&	hit->touch != Touch_DoorTrigger) 
			{
				continue;
			}
		}

		// use seperate code for determining if an item is picked up
		// so you don't have to actually contact its bounding box
		if ( hit->s.eType == ET_ITEM ) {
			if ( !BG_PlayerTouchesItem( &ent->client->ps, &hit->s, level.time ) ) {
				continue;
			}
		} else {
			if ( !trap_EntityContact( mins, maxs, hit ) ) {
				continue;
			}
		}

		memset( &trace, 0, sizeof(trace) );

		if ( hit->touch ) {
			hit->touch (hit, ent, &trace);
		}

		if ( ( ent->r.svFlags & SVF_BOT ) && ( ent->touch ) ) {
			ent->touch( ent, hit, &trace );
		}
	}
}

/*
=================
SpectatorThink
=================
*/
void SpectatorThink( gentity_t *ent, usercmd_t *ucmd ) {
	pmove_t	pm;
	gclient_t	*client;

	client = ent->client;

	if ( client->sess.spectatorState != SPECTATOR_FOLLOW ) {
		client->ps.pm_type = PM_SPECTATOR;
		client->ps.speed = 400;	// faster than normal

		// set up for pmove
		memset (&pm, 0, sizeof(pm));
		pm.ps = &client->ps;
		pm.cmd = *ucmd;
		pm.tracemask = MASK_PLAYERSOLID & ~CONTENTS_BODY;	// spectators can fly through bodies
		pm.trace = trap_Trace;
		pm.pointcontents = trap_PointContents;

		// perform a pmove
		Pmove (&pm);

		// save results of pmove
		VectorCopy( client->ps.origin, ent->s.origin );

		G_TouchTriggers( ent );
		trap_UnlinkEntity( ent );
	}

	client->oldbuttons = client->buttons;
	client->buttons = ucmd->buttons;

	// attack button cycles through spectators
	if ( ( client->buttons & BUTTON_ATTACK ) && ! ( client->oldbuttons & BUTTON_ATTACK ) ) {
		Cmd_FollowCycle_f( ent, 1 );
	}
	else if ( ( client->buttons & BUTTON_ALT_ATTACK ) && ! ( client->oldbuttons & BUTTON_ALT_ATTACK ) )
	{
		if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ) {
			StopFollowing( ent );
		}
	}
}



/*
=================
ClientInactivityTimer
=================
*/
/**
*	Returns qfalse if the client is dropped
*/
qboolean ClientInactivityTimer( gclient_t *client )
{
	usercmd_t *cmd = &client->pers.cmd;

	if ( ! g_inactivity.integer )
	{
		// give everyone some time, so if the operator sets g_inactivity during
		// gameplay, everyone isn't kicked
		client->inactivityTime = level.time + 60 * 1000;
		client->inactivityWarning = qfalse;
	}
	else if (	cmd->forwardmove ||
				cmd->rightmove ||
				cmd->upmove ||
				(cmd->buttons & BUTTON_ATTACK) ||
				(cmd->buttons & BUTTON_ALT_ATTACK) )
	{
		client->inactivityTime = level.time + g_inactivity.integer * 1000;
		client->inactivityWarning = qfalse;
	}
	else if ( !client->pers.localClient )
	{
		if ( level.time > client->inactivityTime )
		{
			trap_DropClient( client - level.clients, "Dropped due to inactivity" );
			return qfalse;
		}
		if ( level.time > client->inactivityTime - 10000 && !client->inactivityWarning )
		{
			client->inactivityWarning = qtrue;
			trap_SendServerCommand( client - level.clients, "cp \"Ten seconds until inactivity drop!\n\"" );
		}
	}
	return qtrue;
}


/*
==================
TimedMessage

RPG-X - RedTechie: Returns the message requested.
If the message is blank go to next. (Based off of SFEFMOD)

TiM: Huh... O_o.  Damn Red, you're right.  If the admin
puts in values, but not in a consistent consecutive order,
we'll get some mighty painful errors. >.<

I guess what we really need is a for loop that goes
thru, and checks to see if each and every CVAR has a value
currently in it....
==================
*/

//First off let's store the CVAR data in a local array.
//This'll let us perform for loop operations on it.
//Sigh, I wish I knew a quick way to convert variable names
//to strings.  Then this wouldn't be necessary O_o
static char *rpg_message[] = { rpg_message1.string,
							   rpg_message2.string,
							   rpg_message3.string,
							   rpg_message4.string,
							   rpg_message5.string,
							   rpg_message6.string,
							   rpg_message7.string,
							   rpg_message8.string,
							   rpg_message9.string,
							   rpg_message10.string };
/**
*	\author Ubergames
*/
char *TimedMessage( void ){
	int i = lastTimedMessage;

	while ( 1 ) { //Okay, start from the number we want, and loop thru all of them
		if ( i >= 11 ) { //reset loop
			i = 1;
		}

		if ( strlen( rpg_message[i-1] ) >= 1 ) { //i-1 coz arrays start @ 0, but we started @ 1
			lastTimedMessage = i;

			return rpg_message[i-1]; //return the string
		}
	
		if ( i == lastTimedMessage - 1 ) { //okay, we've obviously gone thru the whole loop and come up with nothing. So screw it.
			return NULL;
		}

		//TiM: Cheap hack to stop it freezing if string0 is empty.  THe above condition don't work here
		if ( ((i == 1 && lastTimedMessage == 1) && !strlen( rpg_message[i-1] )) ) {
			return NULL;
		}

		i++;
	}

	//TiM: Hopefully it'll never reach here, but we have this anyway to shut the compiler up
	return "^1RPG-X ERROR: No messages to display";
	

/*
	switch( Msg ){
		case 1:
			if(strlen(rpg_message1.string) >= 1){
				lastTimedMessage = 1;
				return rpg_message1.string;
			}else{
				return TimedMessage( 2 );
			}
			break;

		case 2:
			if(strlen(rpg_message2.string) >= 1){
				lastTimedMessage = 2;
				return rpg_message2.string;
			}else{
				return TimedMessage( 3 );
			}
			break;

		case 3:
			if(strlen(rpg_message3.string) >= 1){
				lastTimedMessage = 3;
				return rpg_message3.string;
			}else{
				return TimedMessage( 4 );
			}
			break;

		case 4:
			if(strlen(rpg_message4.string) >= 1){
				lastTimedMessage = 4;
				return rpg_message4.string;
			}else{
				return TimedMessage( 5 );
			}
			break;

		case 5:
			if(strlen(rpg_message5.string) >= 1){
				lastTimedMessage = 5;
				return rpg_message5.string;
			}else{
				return TimedMessage( 6 );
			}
			break;

		case 6:
			if(strlen(rpg_message6.string) >= 1){
				lastTimedMessage = 6;
				return rpg_message6.string;
			}else{
				return TimedMessage( 7 );
			}
			break;

		case 7:
			if(strlen(rpg_message7.string) >= 1){
				lastTimedMessage = 7;
				return rpg_message7.string;
			}else{
				return TimedMessage( 8 );
			}
			break;

		case 8:
			if(strlen(rpg_message8.string) >= 1){
				lastTimedMessage = 8;
				return rpg_message8.string;
			}else{
				return TimedMessage( 9 );
			}
			break;

		case 9:
			if(strlen(rpg_message9.string) >= 1){
				lastTimedMessage = 9;
				return rpg_message9.string;
			}else{
				return TimedMessage( 10 );
			}
			break;

		case 10:
			if(strlen(rpg_message10.string) >= 1){
				lastTimedMessage = 0; //Start over again
				return rpg_message10.string;
			}else{
				return TimedMessage( 1 );
			}
			break;

		default:
			return "^1RPG-X ERROR: No messages to display";
			break;
	}*/
}


/*
==================
ClientTimerActions
==================
*/
/**
*	Actions that happen once a second
*/
void ClientTimerActions( gentity_t *ent, int msec ) {
	gclient_t	*client;
	char		*message;
	float		messageTime;

	client = ent->client;
	client->timeResidual += msec;

	//RPG-X - RedTechie: Heavily Modifyed Message system (Based off of phenix's old code)
	/*if ( level.time >= (level.message + (rpg_timedmessagetime.integer * 60000)) ) {
		level.message = level.time;
		trap_SendServerCommand( -1, va("cp \"%s\n\"", rpg_timedmessage.string) );
		//trap_SendServerCommand( -1, va( "print \"%s\n\"", rpg_timedmessage ) );
	}*/
	if( rpg_timedmessagetime.value ){
		//Make sure its not less then one //TiM: Well... we can have under 1, just not toooo far under 1
		
		//TiM : Init/reset TimedMessage's value
		if ( lastTimedMessage <= 0 || lastTimedMessage > 10 ) {
			lastTimedMessage = 1;
		}

		if(rpg_timedmessagetime.value < 0.2) { //1
			messageTime = 0.2;
		}else{
			messageTime = rpg_timedmessagetime.value;
		}

		if (level.time > (level.message + (messageTime * 60000)) ) {
			level.message = level.time;
			
			//TiM - There.  So with this working in conjunction with that reset
			//code above, this should be more efficient. :)
			message = TimedMessage(); //Since we're working with a gloabl scope variable, there's no need for this thing to have parameters:)
			lastTimedMessage++;

			/*//Decide what timed message to display
			if( lastTimedMessage && lastTimedMessage != 0 ){
				//A message has been displayed
				if(lastTimedMessage == 1){
					//Display message 2
					message = TimedMessage( 2 );
				}else if(lastTimedMessage == 2){
					//Display message 3
					message = TimedMessage( 3 );
				}else if(lastTimedMessage == 3){
					//Display message 4
					message = TimedMessage( 4 );
				}else if(lastTimedMessage == 4){
					//Display message 5
					message = TimedMessage( 5 );
				}else if(lastTimedMessage == 5){
					//Display message 6
					message = TimedMessage( 6 );
				}else if(lastTimedMessage == 6){
					//Display message 7
					message = TimedMessage( 7 );
				}else if(lastTimedMessage == 7){
					//Display message 8
					message = TimedMessage( 8 );
				}else if(lastTimedMessage == 8){
					//Display message 9
					message = TimedMessage( 9 );
				}else if(lastTimedMessage == 9){
					//Display message 10
					message = TimedMessage( 10 );
				}else{
					//BAD
					message = "^1RPG-X ERROR: lastTimedMessage not set correctly.";
				}
			}else{
				//A message hasnt been displayed yet
				message = TimedMessage( 1 );
			}*/
			
			//Alright send the message now
			if ( message != NULL ) {
				trap_SendServerCommand( -1, va("cp \"%s\n\"", message) );  //Shows the message on their main screen
			}

			//trap_SendServerCommand( -1, va("print \"\n \n%s \n\"", message));  //Shows the message in console
		}
	}



	while ( client->timeResidual >= 1000 ) {
		client->timeResidual -= 1000;

		// regenerate
		//TiM - removed so we can use the REGEN powerup elsewhere
		/*if ( client->ps.powerups[PW_LASER] )
		{
			if ( client->sess.sessionClass != PC_NOCLASS && client->sess.sessionClass != PC_BORG && client->sess.sessionClass != PC_ACTIONHERO )
			{
				if ( ent->health < client->ps.stats[STAT_MAX_HEALTH] )
				{
					ent->health += 2;
					G_AddEvent( ent, EV_POWERUP_REGEN, 0 );
				}
			}
			else if ( ent->health < client->ps.stats[STAT_MAX_HEALTH])
			{
				ent->health += 15;
				if ( ent->health > client->ps.stats[STAT_MAX_HEALTH] * 1.1 )
				{
					ent->health = client->ps.stats[STAT_MAX_HEALTH] * 1.1;
				}
				G_AddEvent( ent, EV_POWERUP_REGEN, 0 );
			}
			else if ( ent->health < client->ps.stats[STAT_MAX_HEALTH] * 2)
			{
				ent->health += 5;
				if ( ent->health > client->ps.stats[STAT_MAX_HEALTH] * 2 )
				{
					ent->health = client->ps.stats[STAT_MAX_HEALTH] * 2;
				}
				G_AddEvent( ent, EV_POWERUP_REGEN, 0 );
			}
		}*/
		/*else if (ent->flags & FL_CLOAK)
		{
			//RPG-X: RedTechie - Health dosnt matter 
			if ( ent->health < 41)
			{
				ent->flags ^= FL_CLOAK;
				ent->client->ps.powerups[PW_INVIS]  = level.time + 1000000000;
			}
		}
		else if (ent->flags & FL_FLY)
		{
			//RPG-X: RedTechie - Health dosnt matter 
			if ( ent->health < 41)
			{
				ent->flags ^= FL_FLY;
				ent->client->ps.powerups[PW_FLIGHT] = level.time + 1000000000;
			}
		}*/
		//else
		//{
			// count down health when over max and not cloaked.
			if ( ent->health > client->ps.stats[STAT_MAX_HEALTH] )
			{
				ent->health--;
			}
		//}

		// THIS USED TO count down armor when over max, more slowly now (every other second)
//		if ( client->ps.stats[STAT_ARMOR] > client->ps.stats[STAT_MAX_HEALTH] && ((int)(level.time/1000))&0x01) {

		// NOW IT ONCE AGAIN counts down armor when over max, once per second
		if ( client->ps.stats[STAT_ARMOR] > client->ps.stats[STAT_MAX_HEALTH]) {
			client->ps.stats[STAT_ARMOR]--;
		}

		// if we've got the seeker powerup, see if we can shoot it at someone
		//if ( client->ps.powerups[PW_FLASHLIGHT] )
		//{
			//vec3_t	seekerPos;

			//TiM - Because I commented out the scav code, I'm going to remove all other ref to it
			/*if (SeekerAcquiresTarget(ent, seekerPos)) // set the client's enemy to a valid target
			{
				FireSeeker( ent, ent->enemy, seekerPos );
				G_AddEvent( ent, EV_POWERUP_SEEKER_FIRE, 0 ); // draw the thingy
			}*/
		//}

		if ( !client->ps.stats[STAT_HOLDABLE_ITEM] )
		{//holding nothing...
			if ( client->ps.stats[STAT_USEABLE_PLACED] > 0 )
			{//we're in some kind of countdown
				//so count down
				client->ps.stats[STAT_USEABLE_PLACED]--;
			}
		}

	}
}

/*
====================
ClientIntermissionThink
====================
*/
void ClientIntermissionThink( gclient_t *client ) {
	client->ps.eFlags &= ~EF_TALK;
	client->ps.eFlags &= ~EF_FIRING;

	// the level will exit when everyone wants to or after timeouts

	// swap and latch button actions
	client->oldbuttons = client->buttons;
	client->buttons = client->pers.cmd.buttons;
	if (g_gametype.integer != GT_SINGLE_PLAYER)
	{
		if ( client->buttons & ( BUTTON_ATTACK | BUTTON_USE_HOLDABLE ) & ( client->oldbuttons ^ client->buttons ) ) {
			client->readyToExit ^= 1;
		}
	}
}

/*
====================
Cmd_Ready_f
====================
*/
/**
*	This function is called from the ui_sp_postgame.c as a result of clicking on the
*	"next" button in non GT_TOURNAMENT games.  This replaces the old system of waiting
*	for the user to click an ATTACK or USE button to signal ready
*	(see ClientIntermissionThink())
*
*	when all clients have signaled ready, the game continues to the next match.
*/
void Cmd_Ready_f (gentity_t *ent)
{
	gclient_t *client;
	client = ent->client;

	client->readyToExit ^= 1;
}



typedef struct detHit_s
{
	gentity_t	*detpack;
	gentity_t	*player;
	int			time;
} detHit_t;

#define MAX_DETHITS		32		// never define this to be 0

detHit_t	detHits[MAX_DETHITS];
qboolean	bDetInit = qfalse;

extern qboolean FinishSpawningDetpack( gentity_t *ent, int itemIndex );
//-----------------------------------------------------------------------------DECOY TEMP
extern qboolean FinishSpawningDecoy( gentity_t *ent, int itemIndex );
//-----------------------------------------------------------------------------DECOY TEMP
void DetonateDetpack(gentity_t *ent);

#define DETPACK_DAMAGE			750
#define DETPACK_RADIUS			500

/**
*	The detpack has been shot
*/
void detpack_shot( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath )
{
	int i = 0;
	gentity_t *ent = NULL;

	//so we can't be blown up by things we're blowing up
	self->takedamage = 0;

	G_TempEntity(self->s.origin, EV_GRENADE_EXPLODE);
	G_RadiusDamage( self->s.origin, self->parent?self->parent:self, DETPACK_DAMAGE*0.125, DETPACK_RADIUS*0.25,
		self, DAMAGE_ALL_TEAMS, MOD_DETPACK );
	// we're blowing up cuz we've been shot, so make sure we remove ourselves
	//from our parent's inventory (so to speak)
	for (i = 0; i < MAX_CLIENTS; i++)
	{
		if (((ent = &g_entities[i])!=NULL) && ent->inuse && (self->parent == ent))
		{
			ent->client->ps.stats[STAT_USEABLE_PLACED] = 0;
			ent->client->ps.stats[STAT_HOLDABLE_ITEM] = 0;
			break;
		}
	}
	G_FreeEntity(self);
}

/**
*	Place the detpack
*/
qboolean PlaceDetpack(gentity_t *ent)
{
	gentity_t	*detpack = NULL;
	static gitem_t *detpackItem = NULL;
	float		detDistance = 80;
	trace_t		tr;
	vec3_t		fwd, right, up, end, mins = {-16,-16,-16}, maxs = {16,16,16};
	playerState_t *ps = &ent->client->ps;

	if (detpackItem == NULL)
	{
		detpackItem = BG_FindItemForHoldable(HI_DETPACK);
	}

	// make sure our detHit info is init'd
	if (!bDetInit)
	{
		memset(detHits, 0, MAX_DETHITS*sizeof(detHit_t));
		bDetInit = 1;
	}

	// can we place this in front of us?
	AngleVectors (ps->viewangles, fwd, right, up);
	fwd[2] = 0;
	VectorMA(ps->origin, detDistance, fwd, end);
	trap_Trace (&tr, ps->origin, mins, maxs, end, ent->s.number, MASK_SHOT );
	if (tr.fraction > 0.9)
	{
		// got enough room so place the detpack
		detpack = G_Spawn();
		G_SpawnItem(detpack, detpackItem);
		detpack->physicsBounce = 0.0f;//detpacks are *not* bouncy
		VectorMA(ps->origin, detDistance + mins[0], fwd, detpack->s.origin);
		if ( !FinishSpawningDetpack(detpack, detpackItem - bg_itemlist) )
		{
			return qfalse;
		}
		VectorNegate(fwd, fwd);
		vectoangles(fwd, detpack->s.angles);
		detpack->think = DetonateDetpack;
		detpack->nextthink = level.time + 120000;	// if not used after 2 minutes it blows up anyway
		detpack->parent = ent;
		return qtrue;
	}
	else
	{
		// no room
		return qfalse;
	}
}

/**
*	Was a player hit by a detpack.
*/
qboolean PlayerHitByDet(gentity_t *det, gentity_t *player)
{
	int i = 0;

	if (!bDetInit)
	{
		// detHit stuff not initialized. who knows what's going on?
		return qfalse;
	}
	for (i = 0; i < MAX_DETHITS; i++)
	{
		if ( (detHits[i].detpack == det) && (detHits[i].player == player) )
		{
			return qtrue;
		}
	}
	return qfalse;
}

/**
*	Addes a player to the detpack hits
*/
void AddPlayerToDetHits(gentity_t *det, gentity_t *player)
{
	int			i = 0;
	detHit_t	*lastHit = NULL, *curHit = NULL;

	for (i = 0; i < MAX_DETHITS; i++)
	{
		if (0 == detHits[i].time)
		{
			// empty slot. add our player here.
			detHits[i].detpack = det;
			detHits[i].player = player;
			detHits[i].time = level.time;
		}
		lastHit = &detHits[i];
	}
	// getting here means we've filled our list of detHits, so begin recycling them, starting with the oldest hit.
	curHit = &detHits[0];
	while (lastHit->time < curHit->time)
	{
		lastHit = curHit;
		curHit++;
		// just a safety check here
		if (curHit == &detHits[0])
		{
			break;
		}
	}
	curHit->detpack = det;
	curHit->player = player;
	curHit->time = level.time;
}

/**
*	Clear the hits for this detpack
*/
void ClearThisDetpacksHits(gentity_t *det)
{
	int			i = 0;

	for (i = 0; i < MAX_DETHITS; i++)
	{
		if (detHits[i].detpack == det)
		{
			detHits[i].player = NULL;
			detHits[i].detpack = NULL;
			detHits[i].time = 0;
		}
	}
}

void DetpackBlammoThink(gentity_t *ent)
{
	int i = 0, lifetime = 3000;	// how long (in msec) the shockwave lasts
	int			knockback = 400;
	float		curBlastRadius = 50.0*ent->count, radius = 0;
	vec3_t		vTemp;
	trace_t		tr;
	gentity_t	*pl = NULL;

	if (ent->count++ > (lifetime*0.01))
	{
		ClearThisDetpacksHits(ent);
		G_FreeEntity(ent);
		return;
	}

	// get a list of players within the blast radius at this time.
	// only hit the ones we can see from the center of the explosion.
	for (i=0; i<MAX_CLIENTS; i++)
	{
		if ( g_entities[i].client && g_entities[i].takedamage)
		{
			pl = &g_entities[i];
			VectorSubtract(pl->s.pos.trBase, ent->s.origin, vTemp);
			radius = VectorNormalize(vTemp);
			if ( (radius <= curBlastRadius) && !PlayerHitByDet(ent, pl) )
			{
				// within the proper radius. do we have LOS to the player?
				trap_Trace (&tr, ent->s.origin, NULL, NULL, pl->s.pos.trBase, ent->s.number, MASK_SHOT );
				if (tr.entityNum == i)
				{
					// oh yeah. you're gettin' hit.
					AddPlayerToDetHits(ent, pl);
					VectorMA(pl->client->ps.velocity, knockback, vTemp, pl->client->ps.velocity);
					// make sure the player goes up some
					if (pl->client->ps.velocity[2] < 100)
					{
						pl->client->ps.velocity[2] = 100;
					}
					if ( !pl->client->ps.pm_time )
					{
						int		t;

						t = knockback * 2;
						if ( t < 50 ) {
							t = 50;
						}
						if ( t > 200 ) {
							t = 200;
						}
						pl->client->ps.pm_time = t;
						pl->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
					}
				}
			}
			// this was just for testing. looks pretty neat, though.
/*
			else
			{
				VectorMA(ent->s.origin, curBlastRadius, vTemp, vTemp);
				G_TempEntity(vTemp, EV_FX_STEAM);
			}
*/
		}
	}

	ent->nextthink = level.time + FRAMETIME;
}

/**
*	Detonate a detpack
*/
void DetonateDetpack(gentity_t *ent)
{
	// find all detpacks. the one whose parent is ent...blow up
	gentity_t	*detpack = NULL;
	char		*classname = BG_FindClassnameForHoldable(HI_DETPACK);

	if (!classname)
	{
		return;
	}
	while ((detpack = G_Find (detpack, FOFS(classname), classname)) != NULL)
	{
		if (detpack->parent == ent)
		{
			// found it. BLAMMO!
			// play explosion sound to all clients
			gentity_t	*te = NULL;

			te = G_TempEntity( detpack->s.pos.trBase, EV_GLOBAL_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/weapons/explosions/detpakexplode.wav" );//cgs.media.detpackExplodeSound
			te->r.svFlags |= SVF_BROADCAST;

			//so we can't be blown up by things we're blowing up
			detpack->takedamage = 0;

			G_AddEvent(detpack, EV_DETPACK, 0);
			G_RadiusDamage( detpack->s.origin, detpack->parent, DETPACK_DAMAGE, DETPACK_RADIUS,
				detpack, DAMAGE_HALF_NOTLOS|DAMAGE_ALL_TEAMS, MOD_DETPACK );
			// just turn the model invisible and let the entity think for a bit to deliver a shockwave
			//G_FreeEntity(detpack);
			detpack->classname = NULL;
			detpack->s.modelindex = 0;
			detpack->think = DetpackBlammoThink;
			detpack->count = 1;
			detpack->nextthink = level.time + FRAMETIME;
			return;
		}
		else if (detpack == ent)	// if detpack == ent, we're blowing up this detpack cuz it's been sitting too long
		{
			detpack_shot(detpack, NULL, NULL, 0, 0);
			return;
		}
	}
	// hmm. couldn't find it.
	detpack = NULL;
}





#define SHIELD_HEALTH				250
#define SHIELD_HEALTH_DEC			10		// 25 seconds
#define MAX_SHIELD_HEIGHT			1022 //254
#define MAX_SHIELD_HALFWIDTH		1023 //255
#define SHIELD_HALFTHICKNESS		4
#define SHIELD_PLACEDIST			64


static qhandle_t	shieldAttachSound=0;
static qhandle_t	shieldActivateSound=0;
static qhandle_t	shieldDamageSound=0;
//RPG-X: - RedTechie Added shild ZAPZPZAAAAAAppPPP sound!
static qhandle_t	shieldMurderSound=0;
static qhandle_t	shieldDeactivateSound=0;


/**
*	Remove a forcefield
*/
void ShieldRemove(gentity_t *self)
{
	self->think = G_FreeEntity;
	self->nextthink = level.time + 300;

	self->s.eFlags |= EF_ITEMPLACEHOLDER;

	// Play raising sound...
	G_AddEvent(self, EV_GENERAL_SOUND, shieldDeactivateSound);

	return;
}


/**
*	The think function of a forcefield
*	Does not do much anymore, once, counted down the health of a forcefield and removed
*	it when healt got equal or below zero.
*/
void ShieldThink(gentity_t *self)
{
	self->s.eFlags &= ~(EF_ITEMPLACEHOLDER | EF_NODRAW);
//	self->health -= SHIELD_HEALTH_DEC;
//	self->nextthink = level.time + 1000;
	self->nextthink = 0;
	//if (self->health <= 0)
	//{
	//	ShieldRemove(self);
	//}
	return;
}

/**
*	The shield was damaged to below zero health.
*/
void ShieldDie(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod)
{
	// Play damaging sound...
	G_AddEvent(self, EV_GENERAL_SOUND, shieldDamageSound);

	ShieldRemove(self);
}


/**
*	The shield had damage done to it.  Make it flicker.
*/
void ShieldPain(gentity_t *self, gentity_t *attacker, int damage)
{
	// Set the itemplaceholder flag to indicate the the shield drawing that the shield pain should be drawn.
	self->s.eFlags |= EF_ITEMPLACEHOLDER;
	self->think = ShieldThink;
	self->nextthink = level.time + 400;

	// Play damaging sound...
	G_AddEvent(self, EV_GENERAL_SOUND, shieldDamageSound);

	return;
}


/**
*	Try to turn the shield back on after a delay.
*/
void ShieldGoSolid(gentity_t *self)
{
	trace_t		tr;
	//gentity_t  *other;
	//other = G_Spawn();
	
	// see if we're valid
	//self->health--;
	if (self->health <= 0)
	{
		ShieldRemove(self);
		return;
	}

	trap_Trace (&tr, self->r.currentOrigin, self->r.mins, self->r.maxs, self->r.currentOrigin, self->s.number, CONTENTS_BODY );
	if(tr.startsolid)
	{	// gah, we can't activate yet
		self->nextthink = level.time + 200;
		self->think = ShieldGoSolid;
		trap_LinkEntity(self);
	}
	else
	{ // get hard... huh-huh...
		self->r.contents = CONTENTS_SOLID;
		self->s.eFlags &= ~(/*EF_NODRAW | */EF_ITEMPLACEHOLDER);
		self->nextthink = level.time + 1000;
		self->think = ShieldThink;
		self->takedamage = qtrue;//RPG-X: - RedTechie use to be qtrue //TiM - made true again. should be okay so long as the health isn't decremented
		
		trap_LinkEntity(self);


		// Play raising sound...
		//TiM No more
		//G_AddEvent(self, EV_GENERAL_SOUND, shieldActivateSound);
	}

	return;
}

/**
*	Turn the shield off to allow a friend to pass through.
*/
//RPG-X J2J EDIT here:
void ShieldGoNotSolid(gentity_t *self)
{
	// make the shield non-solid very briefly
	self->r.contents = CONTENTS_NONE;
//	self->s.eFlags |= EF_NODRAW;					//Commenting this should make it look like the player passes through..
	// nextthink needs to have a large enough interval to avoid excess accumulation of Activate messages
	self->nextthink = level.time + 200;
	self->think = ShieldGoSolid;

	//TiM - Make the field visible
	self->s.eFlags |= EF_ITEMPLACEHOLDER;

	self->takedamage = qfalse;
	trap_LinkEntity(self);

	// Play raising sound...
	G_AddEvent(self, EV_GENERAL_SOUND, shieldActivateSound);
}


/**
*	Somebody (a player) has touched the shield.  See if it is a "friend".
*/
void ShieldTouch(gentity_t *self, gentity_t *other, trace_t *trace)
{
	if ( !other || !other->client )
		return;

/*	if (g_gametype.integer >= GT_TEAM)
	{ // let teammates through
		// compare the parent's team to the "other's" team
		if (( self->parent->client) && (other->client))
		{
			if ( self->s.otherEntityNum2 == other->client->sess.sessionTeam || other->client->sess.sessionClass == PC_ALPHAOMEGA22 )
			{
				ShieldGoNotSolid(self);
			}
		}
	}
	else*/
	{//let the person who dropped the shield through
		if (IsAdmin(other) || (rpg_borgAdapt.integer && rpg_borgMoveThroughFields.integer && IsBorg(other))/*other->client->sess.sessionClass == PC_ADMIN*/ )
		{
			ShieldGoNotSolid(self);
		}
		//RPG-X:J2J Damage for shields 
		else
		{
			if ( (int)(self->s.angles[PITCH]) == 0 )
			{
				vec3_t	dir;

				//OPTIMIZE ME: If anyone can figure a quick, non hacky way to get the normal vector
				//of the side of the forcefield they touch, PLEASE put it here lol. THis way works, but feels very half-assed lol
				//Get the directional vector
				VectorSubtract( self->r.currentOrigin, other->r.currentOrigin, dir);
				VectorNormalize(dir);
				//depending on angle, negate the perpendicular direction (else they zap back sideways)
				//and set the other as absolute 
				if ( self->s.angles[YAW] == 0 ) {
					dir[1] = 0.0;
					if ( dir[0] < 0 )
						dir[0] = -1.0;
					else
						dir[0] = 1.0;
				}
				else {
					dir[0] = 0.0;
					if ( dir[1] < 0 )
						dir[1] = -1.0;
					else
						dir[1] = 1.0;
				}

				//Invert it otherwise we'd be like, sucked into the field lol
				VectorScale(dir, -1, dir);
				//Un-normalize it to represent a scalar quantity
				VectorScale( dir, 50, dir);
				//Copy it straight to our velocity (this will mean the player will literally be thrown back)
				VectorCopy( dir, other->client->ps.velocity );
				other->client->ps.pm_time = 160;		// hold time
				other->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;

				other->flags |= EF_MOVER_STOP;	//Attempt to not let non admins thru.
				//RPG-X: RedTechie - Added code for zap sound also a cvar to control damage a non admin walks into if cvar is set to 0 disables health from being drained (happens every 1 second)
				//if ( level.time >= level.message + 1000 ) {
				//	level.message = level.time;
				G_AddEvent(self, EV_GENERAL_SOUND, shieldMurderSound);//RPG-X: RedTechie - ZAPtacular! sound to my ears
				if(rpg_forcefielddamage.integer != 0){
					if(rpg_forcefielddamage.integer > 999){
						rpg_forcefielddamage.integer = 999;
					}
					other->health -= rpg_forcefielddamage.integer;
					
					//RPG-X: RedTechie - Fixed free ent if medic revive on
					if(rpg_medicsrevive.integer == 1){
						if(other->health <= 1){
							other->client->ps.stats[STAT_WEAPONS] = ( 1 << WP_0 );
							other->client->ps.stats[STAT_HOLDABLE_ITEM] = HI_NONE;
							other->client->ps.stats[STAT_HEALTH] = other->health = 1;
							player_die (other, other, other, 1, MOD_FORCEFIELD);
						}
					}else{
						if(other->health <= 1){
							other->client->ps.stats[STAT_HEALTH] = other->health = 0;
							player_die (other, other, other, 100000, MOD_FORCEFIELD);
						}
					}
				}
				//}
			}


			//TiM: make it flicker when touched, and then throw bak the person
			self->s.eFlags		|= EF_ITEMPLACEHOLDER;
			self->nextthink		= level.time + 150;
			self->think			= ShieldThink;
		}

	}
}

/**
*	After a short delay, create the shield by expanding in all directions.
*/
void CreateShield(gentity_t *ent)
{
	trace_t		tr;
	vec3_t		mins, maxs, end, posTraceEnd, negTraceEnd, start;
	int			height, posWidth, negWidth, halfWidth = 0;
	qboolean	xaxis;
	int			paramData = 0;
	//static int	shieldID;
	//gentity_t   *emitter;

	// trace upward to find height of shield
	VectorCopy(ent->r.currentOrigin, end);
	end[2] += MAX_SHIELD_HEIGHT;
	trap_Trace (&tr, ent->r.currentOrigin, NULL, NULL, end, ent->s.number, MASK_SHOT );
	height = (int)(MAX_SHIELD_HEIGHT * tr.fraction);

	// use angles to find the proper axis along which to align the shield
	VectorSet(mins, -SHIELD_HALFTHICKNESS, -SHIELD_HALFTHICKNESS, 0);
	VectorSet(maxs, SHIELD_HALFTHICKNESS, SHIELD_HALFTHICKNESS, height);
	VectorCopy(ent->r.currentOrigin, posTraceEnd);
	VectorCopy(ent->r.currentOrigin, negTraceEnd);

	if ((int)(ent->s.angles[YAW]) == 0) // shield runs along y-axis
	{	
		ent->s.eFlags |= EF_SHIELD_BOX_Y;
		posTraceEnd[1]+=MAX_SHIELD_HALFWIDTH;
		negTraceEnd[1]-=MAX_SHIELD_HALFWIDTH;
		xaxis = qfalse;
	}
	else  // shield runs along x-axis
	{
		ent->s.eFlags |= EF_SHIELD_BOX_X;
		posTraceEnd[0]+=MAX_SHIELD_HALFWIDTH;
		negTraceEnd[0]-=MAX_SHIELD_HALFWIDTH;
		xaxis = qtrue;
	}

	// trace horizontally to find extend of shield
	// positive trace
	VectorCopy(ent->r.currentOrigin, start);
	start[2] += (height>>1);
	trap_Trace (&tr, start, 0, 0, posTraceEnd, ent->s.number, MASK_SHOT );
	posWidth = MAX_SHIELD_HALFWIDTH * tr.fraction;
	// negative trace
	trap_Trace (&tr, start, 0, 0, negTraceEnd, ent->s.number, MASK_SHOT );
	negWidth = MAX_SHIELD_HALFWIDTH * tr.fraction;

	// kef -- monkey with dimensions and place origin in center
	halfWidth = (posWidth + negWidth)>>1;
	if (xaxis)
	{
		ent->r.currentOrigin[0] = ent->r.currentOrigin[0] - negWidth + halfWidth;
	}
	else
	{
		ent->r.currentOrigin[1] = ent->r.currentOrigin[1] - negWidth + halfWidth;
	}
	ent->r.currentOrigin[2] += (height>>1);

	// set entity's mins and maxs to new values, make it solid, and link it
	if (xaxis)
	{
		VectorSet(ent->r.mins, -halfWidth, -SHIELD_HALFTHICKNESS, -(height>>1));
		VectorSet(ent->r.maxs, halfWidth, SHIELD_HALFTHICKNESS, height);
	}
	else
	{
		VectorSet(ent->r.mins, -SHIELD_HALFTHICKNESS, -halfWidth, -(height>>1));
		VectorSet(ent->r.maxs, SHIELD_HALFTHICKNESS, halfWidth, height>>1);
	}
	ent->clipmask = MASK_SHOT;

	// scanable forcefield
	/*if(rpg_scannableForceField.integer) {
		VectorCopy(maxs, ent->s.origin2);
		VectorCopy(mins, ent->s.angles2);
		trap_LinkEntity(ent);
	}*/

	// Information for shield rendering.

//	xaxis - 1 bit
//	height - 0-254 8 bits //10
//	posWidth - 0-255 8 bits //10
//  negWidth - 0 - 255 8 bits

	paramData = (xaxis << 30) | ((height&1023) << 20) | ((posWidth&1023) << 10) | (negWidth&1023); //24 16 8
	ent->s.time2 = paramData;

	if ( ent->s.otherEntityNum2 == TEAM_RED )
	{
		ent->team = "1";
	}
	else if ( ent->s.otherEntityNum2 == TEAM_BLUE )
	{
		ent->team = "2";
	}
	
	ent->health = ceil(SHIELD_HEALTH*g_dmgmult.value);
	
	ent->s.time = ent->health;//???
	ent->pain = ShieldPain;
	ent->die = ShieldDie;
	ent->touch = ShieldTouch;
	
	ent->r.svFlags |= SVF_SHIELD_BBOX;

	// see if we're valid
	trap_Trace (&tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, ent->r.currentOrigin, ent->s.number, CONTENTS_BODY );

	if (tr.startsolid)
	{	// Something in the way!
		// make the shield non-solid very briefly
		ent->r.contents = CONTENTS_NONE;
		ent->s.eFlags |= EF_NODRAW;
		// nextthink needs to have a large enough interval to avoid excess accumulation of Activate messages
		ent->nextthink = level.time + 200;
		ent->think = ShieldGoSolid;
		ent->takedamage = qfalse;
		trap_LinkEntity(ent);
	}
	else
	{	// Get solid.
		ent->r.contents = CONTENTS_PLAYERCLIP|CONTENTS_SHOTCLIP;//CONTENTS_SOLID;

		ent->nextthink = level.time + 400; //1000
		ent->think = ShieldThink;

		ent->takedamage = qtrue;//RPG-X: - RedTechie Use to be qtrue //TiM - made true again. should be okay so long as the health isn't decremented
		trap_LinkEntity(ent);

		ent->s.eFlags |= EF_ITEMPLACEHOLDER;

		// Play raising sound...
		G_AddEvent(ent, EV_GENERAL_SOUND, shieldActivateSound);
	}

	return;
}

// After a short delay, create the shield by expanding in all directions.
//void CreateFlatShield(gentity_t *ent)
//{
//	trace_t		tr;
//	vec3_t		mins, maxs, end, posTraceEnd, negTraceEnd, start;
//	int			height, posWidth, negWidth, halfWidth = 0;
//	qboolean	xaxis;
//	int			paramData = 0;
//	static int	shieldID;
//
//	// trace upward to find height of shield
//	VectorCopy(ent->r.currentOrigin, end);
//	if ( (int)(ent->s.angles[YAW]) == 0 )
//	{
//		if ( ent->timestamp )
//			end[1] += MAX_SHIELD_HEIGHT;
//		else
//			end[1] -= MAX_SHIELD_HEIGHT;
//	}
//	else
//	{
//		if ( ent->timestamp )
//			end[0] += MAX_SHIELD_HEIGHT;
//		else
//			end[0] -= MAX_SHIELD_HEIGHT;
//	}
//	
//	trap_Trace (&tr, ent->r.currentOrigin, NULL, NULL, end, ent->s.number, MASK_SHOT );
//	height = (int)(MAX_SHIELD_HEIGHT * tr.fraction);
//
//	// use angles to find the proper axis along which to align the shield
//	if ( (int)(ent->s.angles[YAW]) == 0 ) {
//		VectorSet(mins, -SHIELD_HALFTHICKNESS, 0, -SHIELD_HALFTHICKNESS);
//		VectorSet(maxs, SHIELD_HALFTHICKNESS, height, SHIELD_HALFTHICKNESS);
//	}
//	else
//	{
//		VectorSet(mins, 0, -SHIELD_HALFTHICKNESS, -SHIELD_HALFTHICKNESS);
//		VectorSet(maxs, height, SHIELD_HALFTHICKNESS, SHIELD_HALFTHICKNESS);
//	}
//	VectorCopy(ent->r.currentOrigin, posTraceEnd);
//	VectorCopy(ent->r.currentOrigin, negTraceEnd);
//
//	if ((int)(ent->s.angles[YAW]) == 0) // shield runs along y-axis
//	{
//		//posTraceEnd[1]+=MAX_SHIELD_HALFWIDTH;
//		//negTraceEnd[1]-=MAX_SHIELD_HALFWIDTH;
//		if ( ent->timestamp )
//		{
//			posTraceEnd[0]+=MAX_SHIELD_HALFWIDTH;
//			negTraceEnd[0]-=MAX_SHIELD_HALFWIDTH;
//		}
//		else
//		{
//			posTraceEnd[0]-=MAX_SHIELD_HALFWIDTH;
//			negTraceEnd[0]+=MAX_SHIELD_HALFWIDTH;			
//		}
//		xaxis = qfalse;
//	}
//	else  // shield runs along x-axis
//	{
//		//posTraceEnd[0]+=MAX_SHIELD_HALFWIDTH;
//		//negTraceEnd[0]-=MAX_SHIELD_HALFWIDTH;
//		if ( ent->timestamp )
//		{
//			posTraceEnd[1]+=MAX_SHIELD_HALFWIDTH;
//			negTraceEnd[1]-=MAX_SHIELD_HALFWIDTH;
//		}
//		else
//		{
//			posTraceEnd[1]-=MAX_SHIELD_HALFWIDTH;
//			negTraceEnd[1]+=MAX_SHIELD_HALFWIDTH;			
//		}
//
//		xaxis = qtrue;
//	}
//
//	// trace horizontally to find extend of shield
//	// positive trace
//	VectorCopy(ent->r.currentOrigin, start);
//	if ( (int)(ent->s.angles[YAW]) == 0 )
//	{
//		if ( ent->timestamp )
//			start[1] += (height>>1);
//		else
//			start[1] -= (height>>1);
//	}
//	else
//	{	
//		if ( ent->timestamp )
//			start[0] += (height>>1);
//		else
//			start[0] -= (height>>1);
//	}
//	trap_Trace (&tr, start, 0, 0, posTraceEnd, ent->s.number, MASK_SHOT );
//	posWidth = MAX_SHIELD_HALFWIDTH * tr.fraction;
//	// negative trace
//	trap_Trace (&tr, start, 0, 0, negTraceEnd, ent->s.number, MASK_SHOT );
//	negWidth = MAX_SHIELD_HALFWIDTH * tr.fraction;
//
//	// kef -- monkey with dimensions and place origin in center
//	halfWidth = (posWidth + negWidth)>>1;
//	if (xaxis)
//	{
//		ent->r.currentOrigin[0] = ent->r.currentOrigin[0] - negWidth + halfWidth;
//	}
//	else
//	{
//		ent->r.currentOrigin[1] = ent->r.currentOrigin[1] - negWidth + halfWidth;
//	}
//	if ( (int)(ent->s.angles[YAW]) == 0 )
//	{
//		if ( ent->timestamp )
//			ent->r.currentOrigin[1] += (height>>1);
//		else
//			ent->r.currentOrigin[1] -= (height>>1);
//	}
//	else
//	{
//		if ( ent->timestamp )
//			ent->r.currentOrigin[0] += (height>>1);
//		else
//			ent->r.currentOrigin[0] -= (height>>1);
//	}
//
//	// set entity's mins and maxs to new values, make it solid, and link it
//	if (xaxis)
//	{
//		VectorSet(ent->r.mins, -halfWidth, -(height>>1), -SHIELD_HALFTHICKNESS);
//		VectorSet(ent->r.maxs, halfWidth, height, SHIELD_HALFTHICKNESS);
//	}
//	else
//	{
//		VectorSet(ent->r.mins, -(height>>1), -halfWidth, -SHIELD_HALFTHICKNESS );
//		VectorSet(ent->r.maxs, height>>1, halfWidth, SHIELD_HALFTHICKNESS );
//	}
//	ent->clipmask = MASK_SHOT;
//
//	// Information for shield rendering.
//
////	vertical - 1 bit
////	xaxis - 1 bit
////	height - 0-254 8 bits //10
////	posWidth - 0-255 8 bits //10
////  negWidth - 0 - 255 8 bits
//
//	paramData = (ent->timestamp << 32 ) | (1 << 31) |(xaxis << 30) | (height << 20) | (posWidth << 10) | (negWidth); //24 16 8
//	ent->s.time2 = paramData;
//
//	if ( ent->s.otherEntityNum2 == TEAM_RED )
//	{
//		ent->team = "1";
//	}
//	else if ( ent->s.otherEntityNum2 == TEAM_BLUE )
//	{
//		ent->team = "2";
//	}
//	
//	//RPG-X: - RedTechie no shield count down
//	if ( g_pModSpecialties.integer != 0 )
//	{
//		//ent->health = ceil(SHIELD_HEALTH*4*g_dmgmult.value);
//	}
//	else
//	{
//		ent->health = ceil(SHIELD_HEALTH*g_dmgmult.value);
//	}
//	
//	
//	ent->s.time = ent->health;//???
//	ent->pain = ShieldPain;
//	ent->die = ShieldDie;
//	ent->touch = ShieldTouch;
//	
//	ent->r.svFlags |= SVF_SHIELD_BBOX;
//
//	// see if we're valid
//	trap_Trace (&tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, ent->r.currentOrigin, ent->s.number, CONTENTS_BODY );
//
//	if (tr.startsolid)
//	{	// Something in the way!
//		// make the shield non-solid very briefly
//		ent->r.contents = CONTENTS_NONE;
//		ent->s.eFlags |= EF_NODRAW;
//		// nextthink needs to have a large enough interval to avoid excess accumulation of Activate messages
//		ent->nextthink = level.time + 200;
//		ent->think = ShieldGoSolid;
//		ent->takedamage = qfalse;
//		trap_LinkEntity(ent);
//	}
//	else
//	{	// Get solid.
//		ent->r.contents = CONTENTS_PLAYERCLIP|CONTENTS_SHOTCLIP;//CONTENTS_SOLID;
//
//		ent->nextthink = level.time + 400; //1000
//		ent->think = ShieldThink;
//
//		ent->takedamage = qtrue;//RPG-X: - RedTechie Use to be qtrue //TiM - made true again. should be okay so long as the health isn't decremented
//		trap_LinkEntity(ent);
//
//		ent->s.eFlags |= EF_ITEMPLACEHOLDER;
//
//		// Play raising sound...
//		G_AddEvent(ent, EV_GENERAL_SOUND, shieldActivateSound);
//	}
//
//	return;
//}

/**
*	Place a forcefield
*/
qboolean PlaceShield(gentity_t *playerent)
{
	static const gitem_t *shieldItem = NULL;
	gentity_t	*shield = NULL;
	trace_t		tr;
	vec3_t		fwd, pos, dest, mins = {-16,-16, 0}, maxs = {16,16,16};
	playerState_t *ps = &playerent->client->ps;

	if (shieldAttachSound==0)
	{
		shieldAttachSound		= G_SoundIndex("sound/weapons/detpacklatch.wav");
		shieldActivateSound		= G_SoundIndex("sound/movers/forcefield/forcefieldon.wav"); //"sound/movers/forceup.wav"
		shieldDamageSound		= G_SoundIndex("sound/ambience/spark5.wav");
		shieldMurderSound		= G_SoundIndex("sound/movers/forcefield/forcefieldtouch.wav"); //RPG-X: - RedTechie Added shild ZAP! sound //sound/world/electro.wav
		shieldDeactivateSound	= G_SoundIndex("sound/movers/forcefield/forcefieldoff.wav");
		shieldItem = BG_FindItemForHoldable(HI_SHIELD);
	}

	// can we place this in front of us?
	AngleVectors (ps->viewangles, fwd, NULL, NULL);
	fwd[2] = 0;
	VectorMA(ps->origin, SHIELD_PLACEDIST, fwd, dest);
	trap_Trace (&tr, ps->origin, mins, maxs, dest, playerent->s.number, MASK_SHOT );
	if (tr.fraction > 0.9)
	{//room in front
		VectorCopy(tr.endpos, pos);
		// drop to floor
		VectorSet( dest, pos[0], pos[1], pos[2] - 4096 );
		trap_Trace( &tr, pos, mins, maxs, dest, playerent->s.number, MASK_SOLID );
		if ( !tr.startsolid && !tr.allsolid )
		{
			// got enough room so place the portable shield
			shield = G_Spawn();

			if(!shield || !shieldItem) return qfalse;

			// Figure out what direction the shield is facing.
			if (fabs(fwd[0]) > fabs(fwd[1]))
			{	// shield is north/south, facing east.
				shield->s.angles[YAW] = 0;
			}
			else
			{	// shield is along the east/west axis, facing north
				shield->s.angles[YAW] = 90;
			}
			shield->think = CreateShield;
			shield->nextthink = level.time + 500;	// power up after .5 seconds
			shield->parent = playerent;

			// Set team number.
			shield->s.otherEntityNum2 = playerent->client->sess.sessionTeam;

			shield->s.eType = ET_USEABLE;
			shield->s.modelindex =  HI_SHIELD;	// this'll be used in CG_Useable() for rendering.
			shield->classname = shieldItem->classname;

			shield->r.contents = CONTENTS_TRIGGER;

			shield->touch = 0;
			// using an item causes it to respawn
			shield->use = 0; //Use_Item;

			// allow to ride movers
			shield->s.groundEntityNum = tr.entityNum;

			G_SetOrigin( shield, tr.endpos );

			shield->s.origin2[0] = rpg_forceFieldColor.integer;

			shield->s.eFlags &= ~EF_NODRAW;
			shield->r.svFlags &= ~SVF_NOCLIENT;

			// add frequency
			/*if(rpg_scannableForceField.integer) {
				if(rpg_forceFieldFreq.integer == -1) {
					shield->s.apos.trBase[0] = -1;
				} else {
					shield->s.apos.trBase[0] = rpg_forceFieldFreq.value;
				}
			}*/

			trap_LinkEntity (shield);

			// Play placing sound...
			G_AddEvent(shield, EV_GENERAL_SOUND, shieldAttachSound);

			return qtrue;
		}
	}

	//GARRRGH Screw this! It'll take too long!
	//TiM - For a horizontal field, aim it at a vertical plane
	//if ( tr.fraction != 1.0 && tr.plane.normal[2] == 0.0f )
	//{
	//	qboolean	positive;

	//	//VectorCopy(tr.endpos, pos);
	//	// drop to floor
	//	if ( Q_fabs(tr.plane.normal[0] ) > Q_fabs(tr.plane.normal[1]) )
	//	{
	//		//VectorSet( dest, (tr.plane.normal[0]>0 ? pos[0] + 4096 : pos[0] - 4096), pos[1], pos[2] );
	//		positive = (tr.plane.normal[0]>0);
	//	}
	//	else
	//	{
	//		//VectorSet( dest, pos[0], (tr.plane.normal[1]>0 ? pos[1] + 4096 : pos[1] - 4096), pos[2] );
	//		positive = (tr.plane.normal[1]>0);
	//	}

	//	//trap_Trace( &tr, pos, mins, maxs, dest, playerent->s.number, MASK_SOLID );

	//	if ( !tr.startsolid && !tr.allsolid )
	//	{
	//		// got enough room so place the portable shield
	//		shield = G_Spawn();

	//		// Figure out what direction the shield is facing.
	//		if (fabs(fwd[0]) > fabs(fwd[1]))
	//		{	// shield is north/south, facing east.
	//			shield->s.angles[YAW] = 0;
	//		}
	//		else
	//		{	// shield is along the east/west axis, facing north
	//			shield->s.angles[YAW] = 90;
	//		}
	//		shield->s.angles[PITCH] = 90;
	//		shield->timestamp = positive;

	//		shield->think = CreateFlatShield;
	//		shield->nextthink = level.time + 500;	// power up after .5 seconds
	//		shield->parent = playerent;

	//		// Set team number.
	//		shield->s.otherEntityNum2 = playerent->client->sess.sessionTeam;

	//		shield->s.eType = ET_USEABLE;
	//		shield->s.modelindex =  HI_SHIELD;	// this'll be used in CG_Useable() for rendering.
	//		shield->classname = shieldItem->classname;

	//		shield->r.contents = CONTENTS_TRIGGER;

	//		shield->touch = 0;
	//		// using an item causes it to respawn
	//		shield->use = 0; //Use_Item;

	//		// allow to ride movers
	//		shield->s.groundEntityNum = tr.entityNum;

	//		G_SetOrigin( shield, tr.endpos );

	//		shield->s.eFlags &= ~EF_NODRAW;
	//		shield->r.svFlags &= ~SVF_NOCLIENT;

	//		trap_LinkEntity (shield);

	//		// Play placing sound...
	//		G_AddEvent(shield, EV_GENERAL_SOUND, shieldAttachSound);

	//		return qtrue;
	//	}
	//}
	// no room
	return qfalse;
}



//-------------------------------------------------------------- DECOY ACTIVITIES
/**
*	Think function for decoys, decoys are spawnchars in RPG-X
*/
void DecoyThink(gentity_t *ent)
{
	ent->s.apos =(ent->parent)->s.apos;					// Update Current Rotation
	ent->nextthink = level.time + irandom(2000, 6000);	// Next think between 2 & 8 seconds

	(ent->count) --;									// Count Down
	if (ent->count<0)			G_FreeEntity(ent);		// Time To Erase The Ent
}

//TiM : I was just able to spawn 600 copies of me...
//my fps died and my PC started making weird noises
//We'd better instigate a limit to our spawning here...
//Borrowed from the tripMines
/**
*	Safety function to limit ammount of decoys spawned at one time and overall.
*	Stops spawning if to many are spawned a a time, stop spawning is a limit was hit.
*	\author Ubergames - TiM
*/
void flushDecoys( gentity_t	*ent ) {
	gentity_t	*decoy = NULL;
	int			foundDecoys[MAX_GENTITIES] = {ENTITYNUM_NONE};
	int			lowestTimeStamp;
	int			orgCount;
	int			decoyCount=0;
	int			removeMe;
	int			i;

	//limit to 10 placed at any one time
	//see how many there are now
	while ( (decoy = G_Find( decoy, FOFS(classname), "decoy" )) != NULL )
	{
		if ( decoy->parent != ent )
		//if ( decoy->s.clientNum != ent->client->ps.clientNum )
		{
			continue;
		}
		foundDecoys[decoyCount++] = decoy->s.clientNum;
	}
	
	//G_Printf("Found %i decoys\n", decoyCount++ ) ;

	//now remove first ones we find until there are only 9 left
	decoy = NULL;
	orgCount = decoyCount;
	lowestTimeStamp = level.time;
	
	//RPG-X: TiM - Let's limit it to say... 64 decoys per player
	while ( decoyCount > 64 ) //9
	{
		removeMe = -1;
		for ( i = 0; i < orgCount; i++ )
		{
			if ( foundDecoys[i] == ENTITYNUM_NONE )
			{
				continue;
			}
			decoy = &g_entities[foundDecoys[i]];
		
			if ( decoy && decoy->timestamp < lowestTimeStamp )
			{
				removeMe = i;
				lowestTimeStamp = decoy->timestamp;
			}
		}

		if ( removeMe != -1 )
		{
			//remove it... or blow it?
			if ( &g_entities[foundDecoys[removeMe]] == NULL )
			{
				break;
			}
			else
			{
				G_FreeEntity( &g_entities[foundDecoys[removeMe]] );
			}
			foundDecoys[removeMe] = ENTITYNUM_NONE;
			decoyCount--;
		}
		else
		{
			break;
		}	
	}
}
 
/**
*	entities spawn non solid and through this function,
*	they'll become solid once nothing's detected in their boundaries. :)
*	\author TiM
*/
void Decoy_CheckForSolidity( gentity_t	*ent ) {
	int			i, num;
	int			touch[MAX_GENTITIES];
	qboolean	canGoSolid=qtrue;
	gentity_t	*hit;
	vec3_t		mins, maxs;

	VectorAdd( ent->s.origin, ent->r.mins, mins );
	VectorAdd( ent->s.origin, ent->r.maxs, maxs );

	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	for ( i = 0; i < num; i++ ) {
		hit = &g_entities[touch[i]];

		if ( hit  && hit->client ) {
			canGoSolid = qfalse;
			break;
		}	
	}

	if ( canGoSolid ) {
		ent->r.contents = MASK_PLAYERSOLID;
		ent->nextthink = 0;
		ent->think = 0;
	}
	else {
		ent->nextthink = level.time + 1000;
		ent->r.contents = CONTENTS_NONE;
	}
}

/**
*	Use function for decoy, removes it if activator is an player and admin
*/
void DecoyUse ( gentity_t *self, gentity_t *other, gentity_t *activator ) {
	if ( !activator || !IsAdmin( activator ) || !activator->client )
		return;

	G_FreeEntity( self );

}

/**
*	Spawn a char
*/	
qboolean PlaceDecoy(gentity_t *ent)
{
	gentity_t	*decoy = NULL;
	static gitem_t *decoyItem = NULL;
	//float		detDistance = 80;	// Distance the object will be placed from player
	//trace_t		tr;
	vec3_t		/*fwd, right, up, end,*/ mins = {-16,-16,-24}; //, maxs = {16,16,16};
	char		userinfo[MAX_INFO_STRING];
	int			i;

	if (decoyItem == NULL)
	{
		decoyItem = BG_FindItemForHoldable(HI_DECOY);
	}

	// can we place this in front of us?
	//AngleVectors (ent->client->ps.viewangles, fwd, right, up);
	//fwd[2] = 0;
	//VectorMA(ent->client->ps.origin, detDistance, fwd, end);
	//trap_Trace (&tr, ent->client->ps.origin, mins, maxs, end, ent->s.number, MASK_SHOT );
	//if ( /*!tr.allsolid && !tr.startsolid && tr.fraction > 0.9*/ qtrue )
	{
		//If we just hit our 129th decoy (...holy crap), reset the counter
		if ( level.decoyIndex >= MAX_CLIENTS ) {
			level.decoyIndex = 0;
		}
		//Now check if there is already a decoy with the same eventParm index.  If there is, terminate it
		{	
			gentity_t	*oldDecoy;

			for ( i = 0; i<level.num_entities; i++ ) {
			oldDecoy = &g_entities[i];

				if ( !Q_stricmp( oldDecoy->classname, "decoy" ) && oldDecoy->s.eventParm == level.decoyIndex ) {
					G_FreeEntity( oldDecoy );
					break;
				}
			}
		}

		//--------------------------- SPAWN AND PLACE DECOY ON GROUND
		decoy = G_Spawn();

		G_SpawnItem(decoy, decoyItem);				// Generate it as an item, temporarly
		decoy->physicsBounce = 0.0f;//decoys are *not* bouncy
		//VectorMA(ent->client->ps.origin, detDistance + mins[0], fwd, decoy->s.origin);
		VectorCopy( ent->client->ps.origin, decoy->s.origin );
		decoy->r.mins[2] = mins[2];//keep it off the floor
		//VectorNegate(fwd, fwd);					// ???  What does this do??
		//vectoangles(fwd, decoy->s.angles);
		VectorCopy( ent->client->ps.viewangles, decoy->s.angles );
		if ( !FinishSpawningDecoy(decoy, decoyItem - bg_itemlist) )
		{
			return qfalse;		// Drop to ground and trap to clients
		}
		decoy->s.clientNum = ent->client->ps.clientNum;
		decoy->s.number = decoy-g_entities;

		//--------------------------- SPECIALIZED DECOY SETUP
		//decoy->think = DecoyThink;
		//decoy->count = 12;						// about 1 minute before dissapear
		//decoy->nextthink = level.time + 4000;	// think after 4 seconds
		decoy->parent = ent;

		(decoy->s).eType = (ent->s).eType;		// set to type PLAYER
		(decoy->s).eFlags= (ent->s).eFlags;
		(decoy->s).eFlags |= EF_ITEMPLACEHOLDER;// set the HOLOGRAM FLAG to ON
		(decoy->s).powerups = (ent->s).powerups;

		//TiM - attach the rotate flag if we need to
		if ( (ent->s).powerups & ( 1<<PW_FLIGHT ) || ent->client->ps.gravity == 0 )
			(decoy->s).eFlags |= EF_FULL_ROTATE;

		decoy->s.eFlags &= ~(EF_FIRING | EF_ALT_FIRING);

		decoy->s.weapon = ent->s.weapon;		// get Player's Wepon Type
//		decoy->s.constantLight = 10 + (10 << 8) + (10 << 16) + (9 << 24);

		//decoy->s.pos.trBase[2] += 24;			// shift up to adjust origin of body
		decoy->s.apos = ent->s.apos;			// copy angle of player to decoy
		decoy->s.pos  = ent->s.pos;

		//decoy->s.legsAnim = BOTH_STAND1;			// Just standing TORSO_STAND
		//decoy->s.torsoAnim = BOTH_STAND1;
		//TiM: Set it's anim to whatever anims we're playing right now
		decoy->s.legsAnim = ent->client->ps.stats[LEGSANIM];
		decoy->s.torsoAnim= ent->client->ps.stats[TORSOANIM];

		decoy->timestamp = level.time;

		//--------------------------- WEAPON ADJUST
		//if (decoy->s.weapon==WP_5 /*|| decoy->s.weapon==WP_13*/)
		//	decoy->s.weapon = WP_1;

		//   The Phaser and Dreadnought (Arc Welder) weapons are rendered on the
		//   client side differently, and cannot be used by the decoy
		decoy->classname = "decoy";
		//flushDecoys( ent );

		//TiM-Set up data for transmission to client
		decoy->s.eventParm = level.decoyIndex;
		decoy->r.contents = CONTENTS_SOLID;	//has to start off solid, or CGame won't realise this

		VectorSet( decoy->r.mins, DEFAULT_MINS_0, DEFAULT_MINS_1, DEFAULT_MINS_2 );
		VectorSet( decoy->r.maxs, DEFAULT_MAXS_0, DEFAULT_MAXS_1, DEFAULT_MAXS_2 );

		decoy->nextthink = level.time + FRAMETIME;
		decoy->think = Decoy_CheckForSolidity;
		decoy->use = DecoyUse;

		trap_GetUserinfo( ent->client->ps.clientNum, userinfo, sizeof(userinfo) );
		{
			char buffer[MAX_TOKEN_CHARS];
			char model[64];
			char height[9];
			char weight[9];
			char offset[6];
			
			//TiM - ensure that we encapsulate this data better or else it sometimes
			//becomes null
			Q_strncpyz( model, Info_ValueForKey( userinfo, "model" ), sizeof( model ) );
			Q_strncpyz( height, Info_ValueForKey( userinfo, "height" ), sizeof( height ) );
			Q_strncpyz( weight, Info_ValueForKey( userinfo, "weight" ), sizeof( weight ) );
			Q_strncpyz( offset, Info_ValueForKey( userinfo, "modelOffset" ), sizeof( offset ) );

			Com_sprintf( buffer, sizeof( buffer ), "model\\%s\\height\\%s\\weight\\%s\\moOf\\%s\\c\\%i", 
													model,
													height,
													weight,
													offset,
													ent->client->sess.sessionClass );

			trap_SetConfigstring( CS_DECOYS + level.decoyIndex, buffer );

			//G_Printf( S_COLOR_BLUE "level.decoyIndex: %i\n", level.decoyIndex );
			//G_Printf( S_COLOR_BLUE "%s\n", buffer );
		}
		level.decoyIndex++;

		return qtrue;						// SUCCESS
	}
	/*else
	{
		return qfalse;						// FAILURE: no room
	}*/
}
//-------------------------------------------------------------- DECOY ACTIVITIES

void G_Rematerialize( gentity_t *ent )
{
	playerState_t *ps = &ent->client->ps;

	ent->client->teleportTime = level.time + ( 15 * 1000 );
	ps->stats[STAT_USEABLE_PLACED] = 15;

	ent->flags &= ~FL_NOTARGET;
	ent->takedamage = qtrue;
	ent->r.contents = MASK_PLAYERSOLID;
	ent->s.eFlags &= ~EF_NODRAW;
	ps->eFlags &= ~EF_NODRAW;
	TeleportPlayer( ent, ps->origin, ps->viewangles, TP_BORG );
	//ps->stats[STAT_USEABLE_PLACED] = 0;
	//take it away
	ps->stats[STAT_HOLDABLE_ITEM] = 0;
}

void G_GiveHoldable( gclient_t *client, holdable_t item )
{
	gitem_t	*holdable = BG_FindItemForHoldable( item );

	client->ps.stats[STAT_HOLDABLE_ITEM] = holdable - bg_itemlist;//teleport spots should be on other side of map
	RegisterItem( holdable );
}

/*
================
ClientEvents
================
*/
/**
*	Events will be passed on to the clients for presentation,
*	but any server game effects are handled here
*/
void ClientEvents( gentity_t *ent, int oldEventSequence ) {
	int		i;
	int		event;
	gclient_t *client;
	int		damage;
	playerState_t *ps;

	client = ent->client;
	ps = &client->ps;

	if ( oldEventSequence < ps->eventSequence - MAX_PS_EVENTS ) {
		oldEventSequence = ps->eventSequence - MAX_PS_EVENTS;
	}
	for ( i = oldEventSequence ; i < ps->eventSequence ; i++ ) {
		event = ps->events[ i & (MAX_PS_EVENTS-1) ];

		switch ( event ) {
		case EV_FALL_MEDIUM:
		case EV_FALL_FAR:
			if ( ent->s.eType != ET_PLAYER ) {
				break;		// not in the player model
			}
			if ( g_dmflags.integer & DF_NO_FALLING ) {
				break;
			}
			if ( rpg_selfdamage.integer != 0 )
			{
				if ( event == EV_FALL_FAR )
				{
					damage = 110; //10 -TiM : Make the falling more realistc!
				}
				else
				{
					damage = 90; //5
				}
			}
			else
			{
				damage = 0;
			}
			ent->pain_debounce_time = level.time + 200;	// no normal pain sound
			G_Damage (ent, NULL, NULL, NULL, NULL, damage, DAMAGE_ARMOR_PIERCING, MOD_FALLING);

			break;

		case EV_FIRE_WEAPON:
			FireWeapon( ent, qfalse );
			break;

		case EV_ALT_FIRE:
			FireWeapon( ent, qtrue );
			break;

		case EV_FIRE_EMPTY_PHASER:
			FireWeapon( ent, qfalse );
			break;

		case EV_USE_ITEM1:		// transporter

			//------------------------------------------------------- DROP FLAGS
			/*if      ( ps->powerups[PW_REDFLAG] ) {
				Team_ReturnFlag(TEAM_RED);
			}*/
			/*else if ( ps->powerups[PW_BORG_ADAPT] ) {
				Team_ReturnFlag(TEAM_BLUE);
			}*/
			//ps->powerups[PW_BORG_ADAPT] = 0;
			//ps->powerups[PW_REDFLAG]  = 0;
			//------------------------------------------------------- DROP FLAGS
			if ( qfalse/*client->sess.sessionClass == PC_BORG*/ )
			{
				if ( !ps->stats[STAT_USEABLE_PLACED] )
				{//go into free-roaming mode
					gentity_t	*tent;
					//FIXME: limit this to 10 seconds
					ent->flags |= FL_NOTARGET;
					ent->takedamage = qfalse;
					ent->r.contents = CONTENTS_PLAYERCLIP;
					ent->s.eFlags |= EF_NODRAW;
					ps->eFlags |= EF_NODRAW;
					ps->stats[STAT_USEABLE_PLACED] = 2;
					client->teleportTime = level.time + 10000;
					tent = G_TempEntity( ps->origin, EV_BORG_TELEPORT );
					tent->s.clientNum = ent->s.clientNum;
				}
				else
				{//come out of free-roaming mode, teleport to current position
					G_Rematerialize( ent );
				}
			}
			else// get rid of transporter and go to random spawnpoint
			{
				//vec3_t	origin, angles;
				//gentity_t	*tent;

				//SelectSpawnPoint( ps->origin, origin, angles );

				//if(TransDat[ps->clientNum].pUsed == qfalse)
				//TiM: Since we purge the vectors each cycle.  I'll save us some memory by using the vectors themselves as a check.
				if ( TransDat[ps->clientNum].beamTime == 0 && 
					VectorCompare( vec3_origin, TransDat[ps->clientNum].storedCoord[TPT_PORTABLE].origin ) &&
					 VectorCompare( vec3_origin, TransDat[ps->clientNum].storedCoord[TPT_PORTABLE].angles ) )
				{
					VectorCopy( ps->origin, TransDat[ps->clientNum].storedCoord[TPT_PORTABLE].origin );
					VectorCopy( ps->viewangles, TransDat[ps->clientNum].storedCoord[TPT_PORTABLE].angles );
					//VectorCopy(ps->origin, TransDat[ps->clientNum].pTransCoord);
					//VectorCopy(ps->viewangles, TransDat[ps->clientNum].pTransCoordRot);
					//TransDat[ps->clientNum].pUsed = qtrue;
					trap_SendServerCommand( ent-g_entities, va("chat \"Site to Site Transporter Location Confirmed.\nPress again to Energize.\"", Q_COLOR_ESCAPE));
					//trap_SendConsoleCommand( EXEC_APPEND, va("echo Site to Site Transporter Location Confirmed.\necho Press again to Energize.") );
					ps->stats[STAT_HOLDABLE_ITEM] = BG_FindItemForHoldable( HI_TRANSPORTER ) - bg_itemlist;
					ps->stats[STAT_USEABLE_PLACED] = 2; // = 1
					break;
				}

				//TeleportPlayer( ent, TransDat[ps->clientNum].pTransCoord, TransDat[ps->clientNum].pTransCoordRot, TP_NORMAL );
				if ( TransDat[ps->clientNum].beamTime == 0 && level.time > ps->powerups[PW_QUAD] ) {
					G_InitTransport( ps->clientNum, TransDat[ps->clientNum].storedCoord[TPT_PORTABLE].origin,
									TransDat[ps->clientNum].storedCoord[TPT_PORTABLE].angles );

					memset( &TransDat[ps->clientNum].storedCoord[TPT_PORTABLE], 0, sizeof( TransDat[ps->clientNum].storedCoord[TPT_PORTABLE]) );
				}
				else {
					trap_SendServerCommand( ent-g_entities, va("chat \"Unable to comply. Already within transport cycle.\"", Q_COLOR_ESCAPE));
				}

				ps->stats[STAT_USEABLE_PLACED] = 0;
				
				if (g_classData[client->sess.sessionClass].isMarine/* client->sess.sessionClass == PC_ALPHAOMEGA22*/ )
				{
					client->teleportTime = level.time + ( 3 * 1000 ); // 15 * 1000
					ps->stats[STAT_USEABLE_PLACED] = 1; // = 1
				}
					
			}
			break;

		case EV_USE_ITEM2:		// medkit
			// New set of rules.  You get either 100 health, or an extra 25, whichever is higher.
			// Give 1/4 health.
			ent->health += ps->stats[STAT_MAX_HEALTH]*0.25;

			if (ent->health < ps->stats[STAT_MAX_HEALTH])
			{	// If that doesn't bring us up to 100, make it go up to 100.
				ent->health = ps->stats[STAT_MAX_HEALTH];
			}
			else if (ent->health > ps->stats[STAT_MAX_HEALTH]*2)
			{	// Otherwise, 25 is all you get.  Just make sure we don't go above 200.
				ent->health = ps->stats[STAT_MAX_HEALTH]*2;
			}
			break;

		case EV_USE_ITEM3:		// detpack
			// if we haven't placed it yet, place it
			if (0 == ps->stats[STAT_USEABLE_PLACED])
			{
				if ( PlaceDetpack(ent) )
				{
					ps->stats[STAT_USEABLE_PLACED] = 1;
					trap_SendServerCommand( ent-g_entities, "cp \"CHARGE PLACED\"" );
				}
				else
				{//couldn't place it
					ps->stats[STAT_HOLDABLE_ITEM] = (BG_FindItemForHoldable( HI_DETPACK ) - bg_itemlist);
					trap_SendServerCommand( ent-g_entities, "cp \"NO ROOM TO PLACE CHARGE\"" );
				}
			}
			else
			{
				//turn off invincibility since you are going to do massive damage
				//ps->powerups[PW_GHOST] = 0;//NOTE: this means he can respawn and get a detpack 10 seconds later
				// ok, we placed it earlier. blow it up.
				ps->stats[STAT_USEABLE_PLACED] = 0;
				DetonateDetpack(ent);
			}
			break;

		case EV_USE_ITEM4:		// portable shield
			if ( !PlaceShield(ent) )	// fixme if we fail, perhaps just spawn it as a pickup
			{//couldn't place it
				ps->stats[STAT_HOLDABLE_ITEM] = (BG_FindItemForHoldable( HI_SHIELD ) - bg_itemlist);
				trap_SendServerCommand( ent-g_entities, "cp \"NO ROOM TO PLACE FORCE FIELD\"" );
			}
			else
			{
				trap_SendServerCommand( ent-g_entities, "cp \"FORCE FIELD PLACED\"" );
				//if ( client->sess.sessionClass == PC_ADMIN )
			//	{
				//	client->teleportTime = level.time + ( 30 * 1000 );
				//	ps->stats[STAT_USEABLE_PLACED] = 30;
				//}
			}
			break;

		case EV_USE_ITEM5:		// decoy
			if ( !PlaceDecoy(ent) )
			{//couldn't place it
				ps->stats[STAT_HOLDABLE_ITEM] = (BG_FindItemForHoldable( HI_DECOY ) - bg_itemlist);
				trap_SendServerCommand( ent-g_entities, "cp \"NO ROOM TO PLACE DECOY\"" );
			}
			else
			{
				trap_SendServerCommand( ent-g_entities, "cp \"DECOY PLACED\"" );
			}
			break;

		default:
			break;
		}
	}

}

void BotTestSolid(vec3_t origin);

/*
=============
ThrowWeapon

RPG-X
Marcin 03/12/2008
=============
*/
/**
*	Throw a weapon away.
*	\author Ubergames - Marcin
*	\date 03/12/2008
*/
void ThrowWeapon( gentity_t *ent, char *txt )
{
	gclient_t	*client;
	usercmd_t	*ucmd;
	gitem_t		*item;
	gentity_t	*drop;
	byte i;
	playerState_t *ps;

	client = ent->client;
	ucmd = &ent->client->pers.cmd;
	ps = &client->ps;

    if ( rpg_allowWeaponDrop.integer == 0) {
        return;
    }

	if ( numTotalDropped >= MAX_DROPPED ) {
		WARNING(("RPG-X Warning: maximum of dropped items of %i reached.\n", MAX_DROPPED));
		return;
	}

    if ( ps->weapon == WP_1 || ( ucmd->buttons & BUTTON_ATTACK )) {
		return;
    }

	numTotalDropped++;

	item = BG_FindItemForWeapon( ps->weapon );

    // admins don't lose weapon when thrown
    if ( IsAdmin( ent ) == qfalse ) {
		ps->ammo[ ps->weapon ] -= 1;
		if (ps->ammo[ ps->weapon ] <= 0) {
			ps->stats[STAT_WEAPONS] &= ~( 1 << ps->weapon );
			ps->weapon = WP_1;
			for ( i = WP_NUM_WEAPONS - 1 ; i > 0 ; i-- ) {
				if ( ps->stats[STAT_WEAPONS] & ( 1 << i ) ) {
					ps->weapon = i;
					break;
				}
			}
		}
    }

	drop = DropWeapon( ent, item, 0, FL_DROPPED_ITEM | FL_THROWN_ITEM, txt );
    drop->parent = ent;
	drop->count = 1;
}


/*
==============
SendPendingPredictableEvents
==============
*/
void SendPendingPredictableEvents( playerState_t *ps ) {
	gentity_t *t;
	int event, seq;
	int extEvent;

	// if there are still events pending
	if ( ps->entityEventSequence < ps->eventSequence ) {
		// create a temporary entity for this event which is sent to everyone
		// except the client generated the event
		seq = ps->entityEventSequence & (MAX_PS_EVENTS-1);
		event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
		// set external event to zero before calling BG_PlayerStateToEntityState
		extEvent = ps->externalEvent;
		ps->externalEvent = 0;
		// create temporary entity for event
		t = G_TempEntity( ps->origin, event );
		BG_PlayerStateToEntityState( ps, &t->s, qtrue );
		t->s.eType = ET_EVENTS + event;
		// send to everyone except the client who generated the event
		t->r.svFlags |= SVF_NOTSINGLECLIENT;
		t->r.singleClient = ps->clientNum;
		// set back external event
		ps->externalEvent = extEvent;
	}
}

void ClientCamThink(gentity_t *client) {
	if(!client->client->cam) return;
	G_SetOrigin(client, client->client->cam->s.origin);
	SetClientViewAngle(client, client->client->cam->s.angles);
	trap_LinkEntity(client);
}

/*
==============
ClientThink
==============
*/
/**
*	This will be called once for each client frame, which will
*	usually be a couple times for each server frame on fast clients.
*
*	If "g_synchronousClients 1" is set, this will be called exactly
*	once for each server frame, which makes for smooth demo recording.
*/
void ClientThink_real( gentity_t *ent ) {
	gclient_t	*client;
	pmove_t		pm;
	vec3_t		oldOrigin;
	int			oldEventSequence;
	int			msec;
	usercmd_t	*ucmd;
	playerState_t *ps;

	client = ent->client;
	ps = &client->ps;

	// don't think if the client is not yet connected (and thus not yet spawned in)
	if (client->pers.connected != CON_CONNECTED) {
		return;
	}
	// mark the time, so the connection sprite can be removed
	ucmd = &client->pers.cmd;

	// sanity check the command time to prevent speedup cheating
	if ( ucmd->serverTime > level.time + 200 ) {
		ucmd->serverTime = level.time + 200;
//		G_Printf("serverTime <<<<<\n" );
	}
	if ( ucmd->serverTime < level.time - 1000 ) {
		ucmd->serverTime = level.time - 1000;
//		G_Printf("serverTime >>>>>\n" );
	}

	msec = ucmd->serverTime - ps->commandTime;
	// following others may result in bad times, but we still want
	// to check for follow toggles
	if ( msec < 1 && client->sess.spectatorState != SPECTATOR_FOLLOW ) {
		return;
	}
	if ( msec > 200 ) {
		msec = 200;
	}

	//
	// check for exiting intermission
	//
	if ( level.intermissiontime ) {
		ClientIntermissionThink( client );
		return;
	}

	if (ent->flags & FL_CCAM) {
		ClientCamThink( ent );
		return;
	}

	// Don't move while under intro sequence.
	/*if (ps->introTime > level.time)
	{	// Don't be visible either.
		ent->s.eFlags |= EF_NODRAW;
		SetClientViewAngle( ent, ent->s.angles );
		ucmd->buttons = 0;
		ucmd->weapon = 0;
//		ucmd->angles[0] = ucmd->angles[1] = ucmd->angles[2] = 0;
		ucmd->forwardmove = ucmd->rightmove = ucmd->upmove = 0;
//		return;
	}*/

	// spectators don't do much
	if ( client->sess.sessionTeam == TEAM_SPECTATOR /*|| (ps->eFlags&EF_ELIMINATED) */) {
		if ( client->sess.spectatorState == SPECTATOR_SCOREBOARD ) {
			return;
		}
		SpectatorThink( ent, ucmd );
		return;
	}

	// check for inactivity timer, but never drop the local client of a non-dedicated server
	if ( !ClientInactivityTimer( client ) ) {
		return;
	}

	//TiM - If we're null content... see what's up
	if ( ent->r.contents == CONTENTS_NONE ) {
		if ( !G_MoveBox( ent ) ) {
			if ( ps->stats[STAT_HEALTH] > 1 )
				ent->r.contents = CONTENTS_BODY;
			else
				ent->r.contents = CONTENTS_CORPSE;
		}
		//else {
		//	ent->r.contents = CONTENTS_NONE;
		//}
	}

	// clear the rewards if time
	/*if ( level.time > client->rewardTime ) {
		ps->eFlags &= ~EF_AWARD_MASK;
	}*/
	
	//RPG-X: Checked to see if medics revive is on if so do as following
	if(rpg_medicsrevive.integer == 1){
		if ( client->noclip ) {
			ps->pm_type = PM_NOCLIP;
		} else if ( ps->stats[STAT_HEALTH] == 1 ) {
			ps->pm_type = PM_DEAD;
		} else {
			ps->pm_type = PM_NORMAL;
		}
	}else{
		if ( client->noclip ) {
			ps->pm_type = PM_NOCLIP;
		} else if ( ps->stats[STAT_HEALTH] <= 0 ) {
			ps->pm_type = PM_DEAD;
		} else {
			ps->pm_type = PM_NORMAL;
		}
	}
	
	//RPG-X: J2J & Phenix - For the gravity ent
	if(client->SpecialGrav != qtrue)
	{
		ps->gravity = g_gravity.value;
	}

	// set speed
	ps->speed = g_speed.value;

	if ( ps->powerups[PW_HASTE] )
	{
		ps->speed *= 1.5;
	}
	else if (ps->powerups[PW_FLIGHT] )
	{//flying around
		ps->speed *= 1.3;
	}
	else if ( ps->stats[STAT_HEALTH] <= 20 ) {
		ps->speed *= 0.55;
	}
	
	if (( ps->powerups[PW_EVOSUIT] ) && ( ps->gravity == 0 ))
	{//Evosuit time.. RPG-X | Phenix | 8/8/2004
		ps->speed *= 1.3;
	}
	
	//RPG-X: Redtechie - n00bie stay.....good boy!
	if ( g_classData[client->sess.sessionClass].isn00b /*client->sess.sessionClass == PC_N00B*/ ){
     ps->speed = 0;
	}

	/*if ( client->sess.sessionClass == PC_HEAVY )
	{
		ps->speed *= 0.75;
	}

	if ( client->sess.sessionClass == PC_BORG )
	{
		if ( BG_BorgTransporting( &client->ps ) )
		{
			ps->speed *= 1.5;
		}
		else if ( !BG_BorgTransporting( &client->ps ) )
		{
			ps->speed *= 0.75;
		}
	}*/

	//TiM : SP Style Transporter. :)
	//first check to see if we should be beaming
	if ( level.time < TransDat[ps->clientNum].beamTime ) {

		//if we're past the mid point of each materialization cycle, make it
		//so bullets and other players will pass thru the transportee. :)
		if ( (level.time > TransDat[ps->clientNum].beamTime - 6000) &&
			( level.time < TransDat[ps->clientNum].beamTime - 2000 ) ) {
			if ( ps->stats[STAT_HEALTH] > 1 ) {
				ent->r.contents = CONTENTS_NONE;
			}
			//ent->r.contents = CONTENTS_CORPSE;
		}
		else {
			if ( ps->stats[STAT_HEALTH] > 1 ) {
				ent->r.contents = MASK_PLAYERSOLID;
				//ent->r.contents = CONTENTS_BODY;
			}
		}

		//If we're half-way thru the cycle, teleport the player now
		if ( level.time > TransDat[ps->clientNum].beamTime - 4000 && 
		!TransDat[ps->clientNum].beamed ) {
			TeleportPlayer( ent, TransDat[ps->clientNum].currentCoord.origin, 
									TransDat[ps->clientNum].currentCoord.angles, 
									TP_TRI_TP );

			TransDat[ps->clientNum].beamed = qtrue;
		}
	}
	else {
		//all done, let's reset :)
		if ( TransDat[ps->clientNum].beamTime > 0 ) {
			TransDat[ps->clientNum].beamTime = 0;
			ps->powerups[PW_BEAM_OUT] = 0;
			ps->powerups[PW_QUAD] = 0;
			TransDat[ps->clientNum].beamed = qfalse;

			memset( &TransDat[ps->clientNum].currentCoord, 0, 
					sizeof( TransDat[ps->clientNum].currentCoord.origin ) );

			if(g_entities[ps->clientNum].flags & FL_CLAMPED) {
				//reset everything if player was beamed by trigger_transporter
				g_entities[ps->clientNum].flags ^= FL_CLAMPED;
			}
		}
	}

	//TiM : Freeze their movement if they're halfway through a transport cycle
	if ( level.time < TransDat[ps->clientNum].beamTime && 
		level.time > TransDat[ps->clientNum].beamTime - 4000 )
	{
		vec3_t	endPoint;
		trace_t	tr;
		VectorSet( endPoint, ps->origin[0], ps->origin[1], ps->origin[2] - 48 );
		//Do a trace down.  If we're near ground, just re-enable gravity.  Else we we get weird animations O_o
		trap_Trace( &tr, ps->origin, NULL, NULL, endPoint, ps->clientNum, CONTENTS_SOLID );

		if ( tr.fraction == 1.0 ) {
			ps->gravity = 0;
			ps->velocity[2] = 0;
		}

		ps->speed = 0;
		
		ps->velocity[0] = ps->velocity[1] = 0.0;
	}

	// set up for pmove
	oldEventSequence = ps->eventSequence;

	memset (&pm, 0, sizeof(pm));

	pm.ps = &client->ps;
	pm.cmd = *ucmd;
	if ( pm.ps->pm_type == PM_DEAD ) {
		pm.tracemask = MASK_PLAYERSOLID & ~CONTENTS_BODY;
	}
	else {
		pm.tracemask = MASK_PLAYERSOLID;
	}

	pm.trace = trap_Trace;
	pm.pointcontents = trap_PointContents;
	pm.debugLevel = g_debugMove.integer;
	pm.noFootsteps = ( g_dmflags.integer & DF_NO_FOOTSTEPS ) > 0;
	pm.pModDisintegration = qfalse;

	//pm.admin = IsAdmin(ent); // we use this way now the old way didn't work for adminlogin
	// y call a function though???
	pm.admin = g_classData[client->sess.sessionClass].isAdmin || client->LoggedAsAdmin;
	//pm.admin = g_classData[client->sess.sessionClass].isAdmin;
	pm.medic = g_classData[client->sess.sessionClass].isMedical;
	pm.borg	 = g_classData[client->sess.sessionClass].isBorg;

	VectorCopy( ps->origin, oldOrigin );

	// perform a pmove
	Pmove (&pm);

	// save results of pmove
	if ( ps->eventSequence != oldEventSequence ) {
		ent->eventTime = level.time;
	}
	BG_PlayerStateToEntityState( ps, &ent->s, qtrue );

	SendPendingPredictableEvents( ps );

	// use the snapped origin for linking so it matches client predicted versions
	VectorCopy( ent->s.pos.trBase, ent->r.currentOrigin );

	VectorCopy (pm.mins, ent->r.mins);
	VectorCopy (pm.maxs, ent->r.maxs);

	ent->waterlevel = pm.waterlevel;
	ent->watertype = pm.watertype;

	// execute client events
	ClientEvents( ent, oldEventSequence );

	if ( pm.useEvent )
	{		//TODO: Use
		TryUse( ent );
	}

	// link entity now, after any personal teleporters have been used
	trap_LinkEntity (ent);
//	if ( !client->noclip ) {
		G_TouchTriggers( ent );
//	}

	// NOTE: now copy the exact origin over otherwise clients can be snapped into solid
	VectorCopy( ps->origin, ent->r.currentOrigin );

	//test for solid areas in the AAS file
	BotTestSolid(ent->r.currentOrigin);

	// touch other objects
	ClientImpacts( ent, &pm );

	// save results of triggers and client events
	if (ps->eventSequence != oldEventSequence) {
		ent->eventTime = level.time;
	}

	// swap and latch button actions
	client->oldbuttons = client->buttons;
	client->buttons = ucmd->buttons;
	client->latched_buttons |= client->buttons & ~client->oldbuttons;

	// check for respawning
	if ( client->ps.stats[STAT_HEALTH] <= 0 ) {
		// wait for the attack button to be pressed
		if ( level.time > client->respawnTime ) {
			// forcerespawn is to prevent users from waiting out powerups
			//RPG-X: RedTechie - No forcerespawn
			/*if ( g_forcerespawn.integer > 0 &&
				( level.time - client->respawnTime ) > g_forcerespawn.integer * 1000 ) {
				respawn( ent );
				return;
			}*/

			// pressing attack or use is the normal respawn method
			if ( ucmd->buttons & ( BUTTON_ATTACK | BUTTON_USE_HOLDABLE ) ) {
				respawn( ent );
				return;
			}

			//in assimilation and elimination, always force respawn
			/*if ( level.time - client->respawnTime > 1300 && //NOTE: when killed, client->respawnTime = level.time + 1700, so this is 3000 ms
				( g_pModAssimilation.integer || g_pModElimination.integer ) )
			{
				respawn( ent );
				return;
			}*/
		}
		return;
	}

	// perform once-a-second actions
	ClientTimerActions( ent, msec );

	if ( client->teleportTime > 0 && client->teleportTime < level.time )
	{
		//if ( client->sess.sessionClass == PC_BORG )
		//{
		//	if ( BG_BorgTransporting( ps ) )
		//	{
		//		G_Rematerialize( ent );
		//	}
		//	else
		//	{
		//		G_GiveHoldable( client, HI_TRANSPORTER );
		//		ps->stats[STAT_USEABLE_PLACED] = 0;
		//		client->teleportTime = 0;
		//	}
		//}
		//else if ( client->sess.sessionClass == PC_DEMO )
		//{
		//	G_GiveHoldable( client, HI_DETPACK );
		//	ps->stats[STAT_USEABLE_PLACED] = 0;
		//	client->teleportTime = 0;
		//}
		/*else */if ( g_classData[client->sess.sessionClass].isMarine /*client->sess.sessionClass == PC_ALPHAOMEGA22*/ )
		{
			G_GiveHoldable( client, HI_TRANSPORTER );
			ps->stats[STAT_USEABLE_PLACED] = 0;
			client->teleportTime = 0;
		}
		else if ( g_classData[client->sess.sessionClass].isAdmin /*client->sess.sessionClass == PC_ADMIN*/ )
		{
			G_GiveHoldable( client, HI_SHIELD );
			ps->stats[STAT_USEABLE_PLACED] = 0;
			client->teleportTime = 0;
		}
	}

}

/*
==================
ClientThink

==================
*/
/**
*	A new command has arrived from the client
*/
void ClientThink( int clientNum ) {
	gentity_t *ent;

	ent = g_entities + clientNum;
	trap_GetUsercmd( clientNum, &ent->client->pers.cmd );

	// mark the time we got info, so we can display the
	// phone jack if they don't get any for a while
	ent->client->lastCmdTime = level.time;

	if ( !g_synchronousClients.integer ) {
		ClientThink_real( ent );
	}
}


void G_RunClient( gentity_t *ent ) {
	if ( !g_synchronousClients.integer ) {
		return;
	}
	ent->client->pers.cmd.serverTime = level.time;
	ClientThink_real( ent );
}


/*
==================
SpectatorClientEndFrame

==================
*/
void SpectatorClientEndFrame( gentity_t *ent ) {
	gclient_t	*cl;
	clientSession_t *sess = &ent->client->sess;
	playerState_t *ps = &ent->client->ps;

	// if we are doing a chase cam or a remote view, grab the latest info
	if ( sess->spectatorState == SPECTATOR_FOLLOW ) {
		int		clientNum;

		clientNum = sess->spectatorClient;

		// team follow1 and team follow2 go to whatever clients are playing
		if ( clientNum == -1 ) {
			clientNum = level.follow1;
		} else if ( clientNum == -2 ) {
			clientNum = level.follow2;
		}
		if ( clientNum >= 0 ) {
			cl = &level.clients[ clientNum ];
			if ( cl->pers.connected == CON_CONNECTED && cl->sess.sessionTeam != TEAM_SPECTATOR ) {
				ent->client->ps = cl->ps;
				ps->pm_flags |= PMF_FOLLOW;
				return;
			} else {
				// drop them to free spectators unless they are dedicated camera followers
				if ( sess->spectatorClient >= 0 ) {
					sess->spectatorState = SPECTATOR_FREE;
					ClientBegin( ent->client - level.clients, qfalse, qfalse, qfalse );
				}
			}
		}
	}

	if ( sess->spectatorState == SPECTATOR_SCOREBOARD ) {
		ps->pm_flags |= PMF_SCOREBOARD;
	} else {
		ps->pm_flags &= ~PMF_SCOREBOARD;
	}
}

/*
==============
ClientEndFrame
==============
*/
/**
*	Called at the end of each server frame for each connected client
*	A fast client will have multiple ClientThink for each ClientEdFrame,
*	while a slow client may have multiple ClientEndFrame between ClientThink.
*/
void ClientEndFrame( gentity_t *ent ) {
	int			i;
	clientPersistant_t	*pers;
	playerState_t *ps = &ent->client->ps;

	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR /*|| (ps->eFlags&EF_ELIMINATED)*/ ) {
		SpectatorClientEndFrame( ent );
		ent->client->noclip = qtrue;
		return;
	}

	pers = &ent->client->pers;

	// turn off any expired powerups
	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
		if ( ps->powerups[ i ] < level.time ) {
			ps->powerups[ i ] = 0;
		}
	}

	// save network bandwidth
#if 0
	if ( !g_synchronousClients->integer && ps->pm_type == PM_NORMAL ) {
		// FIXME: this must change eventually for non-sync demo recording
		VectorClear( ps->viewangles );
	}
#endif

	//
	// If the end of unit layout is displayed, don't give
	// the player any normal movement attributes
	//
	if ( level.intermissiontime ) {
		return;
	}

	// burn from lava, etc
	P_WorldEffects (ent);

	// apply all the damage taken this frame
	P_DamageFeedback (ent);

	// add the EF_CONNECTION flag if we haven't gotten commands recently
	if ( level.time - ent->client->lastCmdTime > 1000 ) {
		ent->s.eFlags |= EF_CONNECTION;
	} else {
		ent->s.eFlags &= ~EF_CONNECTION;
	}

	ps->stats[STAT_HEALTH] = ent->health;	// FIXME: get rid of ent->health...

	G_SetClientSound (ent);

	// set the latest infor
	BG_PlayerStateToEntityState( ps, &ent->s, qtrue );
	SendPendingPredictableEvents( ps );

}


