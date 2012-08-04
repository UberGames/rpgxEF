// Copyright (C) 1999-2000 Id Software, Inc.
//
// cg_playerstate.c -- this file acts on changes in a new playerState_t
// With normal play, this will be done after local prediction, but when
// following another player or playing back a demo, it will be checked
// when the snapshot transitions like all the other entities

#include "cg_local.h"

/*
==============
CG_CheckAmmo

If the ammo has gone low enough to generate the warning, play a sound
==============
*/
void CG_CheckAmmo( void ) {
	int		i;
	int		total;
	int		previous;
	int		weapons;

	if ( cg.lowAmmoWarning > 2 )
	{//a timed message, draws for a specific amount of time
		if ( cg.lowAmmoWarning > cg.frametime )
		{
			cg.lowAmmoWarning -= cg.frametime;
		}
		else
		{
			cg.lowAmmoWarning = 0;
		}
		return;
	}
	// see about how many seconds of ammo we have remaining
	weapons = cg.snap->ps.stats[ STAT_WEAPONS ];
	total = 0;

	//TiM
	//for ( i = WP_5 ; i < WP_NUM_WEAPONS ; i++ ) {
	for ( i = WP_1 ; i < WP_NUM_WEAPONS ; i++ ) {
		if ( ! ( weapons & ( 1 << i ) ) ) {
			continue;
		}
		switch ( i ) {
		case WP_10:
		case WP_8:
		case WP_1:
		case WP_6:
			total += cg.snap->ps.ammo[i] * 1000;
			break;
		default:
			total += cg.snap->ps.ammo[i] * 200;
			break;
		}
		if ( total >= 5000 ) {
			cg.lowAmmoWarning = 0;
			return;
		}
	}

	previous = cg.lowAmmoWarning;

	if ( total == 0 ) {
		cg.lowAmmoWarning = 2;
	} else {
		cg.lowAmmoWarning = 1;
	}

	// play a sound on transitions
	// RPG-X | Phenix | 13/02/2005
	/*if ( cg.lowAmmoWarning != previous ) {
		trap_S_StartLocalSound( cgs.media.noAmmoSound, CHAN_LOCAL_SOUND );
	}*/
}

/*
==============
CG_DamageFeedback
==============
*/
void CG_DamageFeedback( int yawByte, int pitchByte, int damage, int shielddamage ) {
	float		left, front, up;
	float		kick;
	int			health;
	float		scale;
	vec3_t		dir;
	vec3_t		angles;
	float		dist;
	float		yaw, pitch;

	// show the attacking player's head and name in corner
	cg.attackerTime = cg.time;

	// the lower on health you are, the greater the view kick will be
	health = cg.snap->ps.stats[STAT_HEALTH];
	if ( health < 40 ) {
		scale = 1;
	} else {
		scale = 40.0 / health;
	}

	kick = (damage + shielddamage*0.5) * scale;

	if (kick < 5)
		kick = 5;
	if (kick > 10)
		kick = 10;

	// if yaw and pitch are both 255, make the damage always centered (falling, etc)
	if ( yawByte == 255 && pitchByte == 255 ) {
		cg.damageX = 0;
		cg.damageY = 0;
		cg.v_dmg_roll = 0;
		cg.v_dmg_pitch = -kick;
	} else {
		// positional
		pitch = pitchByte / 255.0 * 360;
		yaw = yawByte / 255.0 * 360;

		angles[PITCH] = pitch;
		angles[YAW] = yaw;
		angles[ROLL] = 0;

		AngleVectors( angles, dir, NULL, NULL );
		VectorSubtract( vec3_origin, dir, dir );

		front = DotProduct (dir, cg.refdef.viewaxis[0] );
		left = DotProduct (dir, cg.refdef.viewaxis[1] );
		up = DotProduct (dir, cg.refdef.viewaxis[2] );

		dir[0] = front;
		dir[1] = left;
		dir[2] = 0;
		dist = VectorLength( dir );
		if ( dist < 0.1 ) {
			dist = 0.1;
		}

		cg.v_dmg_roll = kick * left;
		
		cg.v_dmg_pitch = -kick * front;

		if ( front <= 0.1 ) {
			front = 0.1;
		}
		cg.damageX = -left / front;
		cg.damageY = up / dist;
	}

	// clamp the position
	if ( cg.damageX > 1.0 ) {
		cg.damageX = 1.0;
	}
	if ( cg.damageX < - 1.0 ) {
		cg.damageX = -1.0;
	}

	if ( cg.damageY > 1.0 ) {
		cg.damageY = 1.0;
	}
	if ( cg.damageY < - 1.0 ) {
		cg.damageY = -1.0;
	}

	cg.damageValue = damage * scale;
	if (cg.damageValue > 10)
	{
		cg.damageValue = 1.0;
	}
	else
	{
		cg.damageValue *= 0.1;
	}
	cg.damageShieldValue = shielddamage;
	if (cg.damageShieldValue > 10)
	{
		cg.damageShieldValue = 1.0;
	}
	else
	{
		cg.damageShieldValue *= 0.1;
	}

	cg.v_dmg_time = cg.time + DAMAGE_TIME;
	cg.damageTime = cg.snap->serverTime;
}




/*
================
CG_Respawn

A respawn happened this snapshot
================
*/
void CG_Respawn( void ) {
	// no error decay on player movement
	cg.thisFrameTeleport = qtrue;

	// display weapons available
	cg.weaponSelectTime = cg.time;

	// select the weapon the server says we are using
	cg.weaponSelect = cg.snap->ps.weapon;
}


/*
==============
CG_CheckPlayerstateEvents
==============
*/
void CG_CheckPlayerstateEvents( playerState_t *ps, playerState_t *ops ) {
	int			i;
	int			event;
	centity_t	*cent;

	if ( ps->externalEvent && ps->externalEvent != ops->externalEvent ) {
		cent = &cg_entities[ ps->clientNum ];
		cent->currentState.event = ps->externalEvent;
		cent->currentState.eventParm = ps->externalEventParm;
		CG_EntityEvent( cent, cent->lerpOrigin );
	}

	cent = &cg.predictedPlayerEntity; // cg_entities[ ps->clientNum ];
	// go through the predictable events buffer
	for ( i = ps->eventSequence - MAX_PS_EVENTS ; i < ps->eventSequence ; i++ ) {
		// if we have a new predictable event
		if ( i >= ops->eventSequence
			// or the server told us to play another event instead of a predicted event we already issued
			// or something the server told us changed our prediction causing a different event
			|| (i > ops->eventSequence - MAX_PS_EVENTS && ps->events[i & (MAX_PS_EVENTS-1)] != ops->events[i & (MAX_PS_EVENTS-1)]) ) {
			
			event = ps->events[ i & (MAX_PS_EVENTS-1) ];
			cent->currentState.event = event;
			cent->currentState.eventParm = ps->eventParms[ i & (MAX_PS_EVENTS-1) ];
			CG_EntityEvent( cent, cent->lerpOrigin );

//			cg.predictableEvents[ i & (MAX_PREDICTED_EVENTS-1) ] = event;

//			cg.eventSequence++;
		}
	}
}

/*
==================
CG_CheckLocalSounds
==================
*/
void CG_CheckLocalSounds( playerState_t *ps, playerState_t *ops )
{
//	int			highScore;

	// The most important thing to know is if you are doing damage.
	//RPG-X - TiM	
	/*if ( ps->persistant[PERS_HITS] > ops->persistant[PERS_HITS] )
	{
		int diffhit, diffshields;

		diffhit = ps->persistant[PERS_HITS] - ops->persistant[PERS_HITS];
		diffshields = ps->persistant[PERS_SHIELDS] - ops->persistant[PERS_SHIELDS];
		if (diffshields > diffhit/2)
		{	// We also hit shields along the way, so consider them "pierced".
			trap_S_StartLocalSound( cgs.media.shieldPierceSound, CHAN_LOCAL_SOUND );
		}
		else
		{	// Shields didn't really stand in our way.
			trap_S_StartLocalSound( cgs.media.hitSound, CHAN_LOCAL_SOUND );
		}
	}
	// The second most important thing to worry about is whether you hurt a friend.
	else if ( ps->persistant[PERS_HITS] < ops->persistant[PERS_HITS] )
	{
		trap_S_StartLocalSound( cgs.media.hitTeamSound, CHAN_LOCAL_SOUND );
	}
	// Finally if all this damage bounced off the shields, indicate this.
	else if (ps->persistant[PERS_SHIELDS] > ops->persistant[PERS_SHIELDS])
	{
		// hit shields and the damage didn't go through
		trap_S_StartLocalSound( cgs.media.shieldHitSound, CHAN_LOCAL_SOUND );
	}*/

	// health changes of more than -1 should make pain sounds
	if ( ps->stats[STAT_HEALTH] < ops->stats[STAT_HEALTH] - 1 ) {
		if ( ps->stats[STAT_HEALTH] > 0 ) {
			CG_PainEvent( &cg.predictedPlayerEntity, ps->stats[STAT_HEALTH] );
		}
	}


	// if we are going into the intermission, don't start any voices
	if ( cg.intermissionStarted ) {
		return;
	}

	// reward sounds
	//RPG-X: RedTechie - No reward or frag limit sounds
	/*if ( ps->persistant[PERS_REWARD_COUNT] > ops->persistant[PERS_REWARD_COUNT] ) {
		switch ( ps->persistant[PERS_REWARD] ) {
		case REWARD_IMPRESSIVE:
			trap_S_StartLocalSound( cgs.media.rewardImpressiveSound, CHAN_ANNOUNCER );
			cg.rewardTime = cg.time;
			cg.rewardShader = cgs.media.medalImpressive;
			cg.rewardCount = ps->persistant[PERS_IMPRESSIVE_COUNT];
			break;
		case REWARD_EXCELLENT:
			trap_S_StartLocalSound( cgs.media.rewardExcellentSound, CHAN_ANNOUNCER );
			cg.rewardTime = cg.time;
			cg.rewardShader = cgs.media.medalExcellent;
			cg.rewardCount = ps->persistant[PERS_EXCELLENT_COUNT];
			break;
		case REWARD_DENIED:
			trap_S_StartLocalSound( cgs.media.rewardDeniedSound, CHAN_ANNOUNCER );
			break;
		case REWARD_FIRST_STRIKE:
			trap_S_StartLocalSound( cgs.media.rewardFirstStrikeSound, CHAN_ANNOUNCER);
			cg.rewardTime = cg.time;
			cg.rewardShader = cgs.media.medalFirstStrike;
			cg.rewardCount = 1;
			break;
		case REWARD_STREAK:
			// Play a different sound depending on how long the streak is.
			cg.rewardTime = cg.time;
			cg.rewardCount = 1;
			if (ps->persistant[PERS_STREAK_COUNT] >= STREAK_CHAMPION)
			{
				trap_S_StartLocalSound( cgs.media.rewardChampionSound, CHAN_ANNOUNCER);
				cg.rewardShader = cgs.media.medalChampion;
			}
			else if (ps->persistant[PERS_STREAK_COUNT] >= STREAK_MASTER)
			{
				trap_S_StartLocalSound( cgs.media.rewardMasterSound, CHAN_ANNOUNCER);
				cg.rewardShader = cgs.media.medalMaster;
			}
			else if (ps->persistant[PERS_STREAK_COUNT] >= STREAK_EXPERT)
			{
				trap_S_StartLocalSound( cgs.media.rewardExpertSound, CHAN_ANNOUNCER);
				cg.rewardShader = cgs.media.medalExpert;
			}
			else if (ps->persistant[PERS_STREAK_COUNT] >= STREAK_ACE)
			{
				trap_S_StartLocalSound( cgs.media.rewardAceSound, CHAN_ANNOUNCER);
				cg.rewardShader = cgs.media.medalAce;
			}
			break;
		default:
			CG_Error( "Bad reward_t" );
		}
	} else {
		// lead changes (only if no reward)
		if ( !cg.warmup && !(cg.predictedPlayerState.introTime > cg.time) )
		{
			// never play lead changes during warmup or holo doors
			if ( ps->persistant[PERS_RANK] != ops->persistant[PERS_RANK] ) {
				if ( cgs.gametype >= GT_TEAM ) {
					if ( ps->persistant[PERS_RANK] == 2 ) {
						trap_S_StartLocalSound( cgs.media.teamsTiedSound, CHAN_ANNOUNCER );
					} else if (  ps->persistant[PERS_RANK] == 0 ) {
						trap_S_StartLocalSound( cgs.media.redLeadsSound, CHAN_ANNOUNCER );
					} else if ( ps->persistant[PERS_RANK] == 1 ) {
						trap_S_StartLocalSound( cgs.media.blueLeadsSound, CHAN_ANNOUNCER );
					}
				} else {
					if (  ps->persistant[PERS_RANK] == 0 ) {
						trap_S_StartLocalSound( cgs.media.takenLeadSound, CHAN_ANNOUNCER );
					} else if ( ps->persistant[PERS_RANK] == RANK_TIED_FLAG ) {
						trap_S_StartLocalSound( cgs.media.tiedLeadSound, CHAN_ANNOUNCER );
					} else if ( ( ops->persistant[PERS_RANK] & ~RANK_TIED_FLAG ) == 0 ) {
						trap_S_StartLocalSound( cgs.media.lostLeadSound, CHAN_ANNOUNCER );
					}
				}
			}
		}
	}

	// timelimit warnings
	if ( cgs.timelimit > 0 ) {
		int		msec;

		msec = cg.time - cgs.levelStartTime;

		if ( cgs.timelimit > 5 && !( cg.timelimitWarnings & 1 ) && msec > (cgs.timelimit - 5) * 60 * 1000 ) {
			cg.timelimitWarnings |= 1;
			trap_S_StartLocalSound( cgs.media.fiveMinuteSound, CHAN_ANNOUNCER );
		}
		if ( !( cg.timelimitWarnings & 2 ) && msec > (cgs.timelimit - 1) * 60 * 1000 ) {
			cg.timelimitWarnings |= 2;
			trap_S_StartLocalSound( cgs.media.oneMinuteSound, CHAN_ANNOUNCER );
		}
		if ( !( cg.timelimitWarnings & 4 ) && msec > ( cgs.timelimit * 60 + 2 ) * 1000 ) {
			cg.timelimitWarnings |= 4;
			trap_S_StartLocalSound( cgs.media.suddenDeathSound, CHAN_ANNOUNCER );
		}
	}

	// fraglimit warnings
	if ( cgs.fraglimit > 0 && cgs.gametype != GT_CTF ) {
		highScore = cgs.scores1;
		if ( cgs.fraglimit > 3 && !( cg.fraglimitWarnings & 1 ) && highScore == (cgs.fraglimit - 3) ) {
			cg.fraglimitWarnings |= 1;
			trap_S_StartLocalSound( cgs.media.threeFragSound, CHAN_ANNOUNCER );
		}
		if ( cgs.fraglimit > 2 && !( cg.fraglimitWarnings & 2 ) && highScore == (cgs.fraglimit - 2) ) {
			cg.fraglimitWarnings |= 2;
			trap_S_StartLocalSound( cgs.media.twoFragSound, CHAN_ANNOUNCER );
		}
		if ( !( cg.fraglimitWarnings & 4 ) && highScore == (cgs.fraglimit - 1) ) {
			cg.fraglimitWarnings |= 4;
			trap_S_StartLocalSound( cgs.media.oneFragSound, CHAN_ANNOUNCER );
		}
	}*/
}


void CG_CheckDamageDealt(playerState_t *ps, playerState_t *ops)
{
	static		int damagetime;
	static		int damageamount;

	if (cg_reportDamage.integer)
	{
		if (ps->persistant[PERS_HITS] > ops->persistant[PERS_HITS])
		{	// We did some damage this frame.
			if (damagetime+1000 < cg.time)
			{	// Start a new tally.
				damageamount = ps->persistant[PERS_HITS] - ops->persistant[PERS_HITS];
				damagetime = cg.time;
			}
			else
			{	// Add to a tally that's already here.
				damageamount += ps->persistant[PERS_HITS] - ops->persistant[PERS_HITS];
			}
		}

		// Report the sum of damage done this second.
		if (damageamount > 0 && (damagetime+1000 <= cg.time))
		{
			Com_Printf("Damage this second:  %d\n", damageamount);
			damageamount = 0;
		}
	}			
}



/*
===============
CG_TransitionPlayerState

===============
*/
void CG_TransitionPlayerState( playerState_t *ps, playerState_t *ops ) {
	// check for changing follow mode
	if ( ps->clientNum != ops->clientNum ) {
		cg.thisFrameTeleport = qtrue;
		// make sure we don't get any unwanted transition effects
		*ops = *ps;
	}

	// damage events (player is getting wounded)
	if ( ps->damageEvent != ops->damageEvent && (ps->damageCount || ps->damageShieldCount)) {
		CG_DamageFeedback( ps->damageYaw, ps->damagePitch, ps->damageCount, ps->damageShieldCount);
	}

	// respawning
	if ( ps->persistant[PERS_SPAWN_COUNT] != ops->persistant[PERS_SPAWN_COUNT] ) {
		CG_Respawn();
	}

/*	if ( cg.mapRestart ) {  //q3 update -not tested yet
		CG_Respawn();
		cg.mapRestart = qfalse;
	}
*/
	if ( cg.snap->ps.pm_type != PM_INTERMISSION 
		&& ps->persistant[PERS_TEAM] != TEAM_SPECTATOR /*&& !(ps->eFlags&EF_ELIMINATED)*/) {
		CG_CheckLocalSounds( ps, ops );
	}

	// check for going low on ammo
	CG_CheckAmmo();

	// run events
	CG_CheckPlayerstateEvents( ps, ops );

	// smooth the ducking viewheight change
	if ( ps->viewheight != ops->viewheight ) {
		cg.duckChange = ps->viewheight - ops->viewheight;
		cg.duckTime = cg.time;
	}

#ifdef _DEBUG
	CG_CheckDamageDealt(ps, ops);
#endif //_DEBUG
}

