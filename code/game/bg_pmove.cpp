// Copyright (C) 1999-2000 Id Software, Inc.
//
// bg_pmove.c -- both games player movement code
// takes a playerstate and a usercmd as input and returns a modifed playerstate

#include "q_shared.h"
#include "bg_public.h"
#include "bg_local.h"

pmove_t *pm;
pml_t pml;

// movement parameters
const float pm_stopspeed = 100;
const float pm_duckScale = 0.50;
const float pm_swimScale = 0.50;
const float pm_ladderScale = 0.7f;

const float pm_accelerate = 10;
const float pm_airaccelerate = 1;
const float pm_wateraccelerate = 4;
const float pm_flyaccelerate = 8;

const float pm_friction = 6;
const float pm_waterfriction = 1;
const float pm_flightfriction = 3;
const float pm_evosuitfriction = 0.25; //RPG-X | Phenix | 8/8/2004

int c_pmove = 0;

#define    PHASER_RECHARGE_TIME    100

//RPG-X | Marcin | Big hack but it appears to work | 06/12/2008
#ifdef QAGAME
                                                                                                                        extern vmCvar_t             rpg_rifleDelay;
extern vmCvar_t             rpg_disruptorDelay;
extern vmCvar_t             rpg_photonDelay;
extern vmCvar_t             rpg_altPhotonDelay;
extern vmCvar_t				rpg_TR116Delay;
extern vmCvar_t				rpg_altTricorderDelay;
#define RIFLE_DELAY         rpg_rifleDelay.integer
#define DISRUPTOR_DELAY     rpg_disruptorDelay.integer
#define PHOTON_DELAY        rpg_photonDelay.integer
#define ALT_PHOTON_DELAY    rpg_altPhotonDelay.integer
#define TR116_DELAY			rpg_TR116Delay.integer
#define ALT_TRICORDER_DELAY		rpg_altTricorderDelay.integer
#else
#define RIFLE_DELAY      250
#define DISRUPTOR_DELAY  700
#define PHOTON_DELAY     1600
#define ALT_PHOTON_DELAY 1600
#define TR116_DELAY         500
#define ALT_TRICORDER_DELAY 1000
#endif

//TiM - Copied from the UI module.
//My aim here is to adapt the checks code here so the function can be used both for
//ingame animation, as well as UI animation

// RPG-X UI Required Ones
#define ANIM_IDLE            0
#define ANIM_RUN            1
#define ANIM_WALK            2
#define ANIM_BACK            3
#define ANIM_JUMP            4
#define ANIM_CROUCH            5
#define ANIM_ATTACK            22
//Ingame required ones
#define ANIM_JUMPB            6
#define ANIM_RUNB            7
#define ANIM_WALKB            8
#define ANIM_CROUCHWALK        9
#define ANIM_CROUCHWALKB    10
#define ANIM_SWIM            11
#define ANIM_FLY            12
#define ANIM_TURN            13

/**
*	Checks if the player holding a two handed weapon.
*/
qboolean PM_Holding2HandedWeapon(void) {
  switch (pm->ps->weapon) {
    case WP_7:
    case WP_8:
    case WP_9:
    case WP_6: return qtrue;
  }
  return qfalse;
}

/**
*	Checks if the player is crouching.
*/
qboolean PM_PlayerCrouching(int legsAnim) {
  //switch( pm->ps->legsAnim ) {
  switch ((legsAnim & ~ANIM_TOGGLEBIT)) {
    case BOTH_CROUCH1IDLE:
    case BOTH_CROUCH1WALK:
    case BOTH_CROUCH2IDLE: return qtrue;
  }
  return qfalse;
}

/**
*	Check if player is idling.
*/
qboolean PM_PlayerIdling(int torsoAnim, int legsAnim) {
  //TiM : Cool hacky way to make sure both upper and lower anims are the same
  switch ((legsAnim & ~ANIM_TOGGLEBIT)) //+ ( torsoAnim & ~ANIM_TOGGLEBIT)) >> 1
  {
    case BOTH_STAND1:
    case BOTH_STAND2:
    case BOTH_STAND3:
    case BOTH_STAND4:
    case BOTH_CROWDLOOK3: {
      switch ((torsoAnim & ~ANIM_TOGGLEBIT)) {
        case BOTH_STAND1:
        case BOTH_STAND2:
        case BOTH_STAND3:
        case BOTH_STAND4:
        case BOTH_CROWDLOOK3:
        case TORSO_TRICORDER1:
        case TORSO_HYPOSPRAY1:
        case TORSO_HYPO1:
        case TORSO_DROPWEAP1:
        case TORSO_RAISEWEAP1:
        case TORSO_PADD1:
        case TORSO_COFFEE: return qtrue;
      }
    }
  }
  return qfalse;
}

/**
*	\brief Checks if player is holding a loppable weapon.
*
*	A weapon that can have its
*	firing animation looped,
*	like the PADD or tricorder, etc
*/
qboolean PM_HoldingLoopableWeapon(void) {
  switch (pm->ps->weapon) {
    case WP_13:
    case WP_2:
    case WP_3:
    case WP_11:
    case WP_4: return qtrue;
  }
  return qfalse;
}

/**
*	\brief Checks if player is holding a spillable weapon.
*
*	Player is holding a weapon that
*	shouldn't let players do the
*	'slowing down' anim
*/
qboolean PM_HoldingSpillableWeapon(void) {
  switch (pm->ps->weapon) {
    case WP_4:
    case WP_6:
    case WP_9:
    case WP_8:
    case WP_7: return qtrue;
  }
  return qfalse;
}

/**
*	Check to see if the player is moving at all
*/
qboolean PM_PlayerWalking(int anim) {
  switch (anim & ~ANIM_TOGGLEBIT) {
    case BOTH_WALK1:
    case BOTH_WALK2:
    case BOTH_WALK3:
    case BOTH_WALK4:
    case BOTH_WALK7:
    case LEGS_WALKBACK1: return qtrue;
  }

  return qfalse;
}

/**
*	Check to see if the player is running
*/
qboolean PM_PlayerRunning(int anim) {
  switch (anim & ~ANIM_TOGGLEBIT) {
    case BOTH_RUN1:
    case BOTH_RUN2:
    case LEGS_RUNBACK2: return qtrue;
  }

  return qfalse;
}

/**
*	Check to see if the player is moving while crouching
*/
qboolean PM_PlayerCrouchWalking(int anim) {
  switch (anim & ~ANIM_TOGGLEBIT) {
    case BOTH_CROUCH1WALK: return qtrue;
  }

  return qfalse;
}

/**
*	TiM: An index is defined, and depending
*	on which weapon is active, a specific
*	animation is returned.
*	I could have used pm->ps->weapon instead
*	of manually defining it as an paramter,
*	but I'm going to use this in the UI module,
*	which is out of pm's scope.
*/
int PM_GetAnim(int anim, int weapon, qboolean injured, qboolean upper) {
  playerState_t *ps = pm->ps;
  // Called when player is in idle crouching
  switch (anim) {
    case ANIM_CROUCH:
      //2 handed weapon - "heavy"
      switch (weapon) {
        case WP_7:
        case WP_8:
        case WP_9:
          if (ps->pm_flags & ANIM_ALERT2 && upper)
            return TORSO_WEAPONREADY2;
          else if (upper)
            return BOTH_STAND2;
          else
            return LEGS_KNEEL1;
          break;
          //2 handed weapon - "light"
        case WP_6:
          if (ps->pm_flags & ANIM_ALERT && upper)
            return BOTH_STAND2;
          else if (upper)
            return TORSO_WEAPONREADY2;
          else
            return LEGS_KNEEL1;
          break;
          //1 handed weapon - "phaser"
        case WP_5:
        case WP_10:
          if (upper)
            return TORSO_WEAPONPOSE1;
          else
            return BOTH_CROUCH1IDLE;
          break;
        case WP_4:
          if (upper)
            return TORSO_COFFEE;
          //break;
          //Generic tools - "everything else"
        default: return BOTH_CROUCH2IDLE;
          break;
      }
      break;

      //Called when player is in idle standing
    case ANIM_IDLE:
      //2 handed weapon - "heavy"
      switch (weapon) {
        //case WP_7:
        case WP_8:
        case WP_9:
        case WP_7:
          if (injured)
            return BOTH_INJURED4;
          else {
            if (ps->pm_flags & ANIM_ALERT)
              return BOTH_STAND2;
            else if (ps->pm_flags & ANIM_ALERT2) {
              if (upper)
                return TORSO_WEAPONREADY2;
              else
                return BOTH_STAND2;
            } else
              return BOTH_STAND4;
          }
          break;
          //2 handed weapon - "light"
        case WP_6:
          if (injured)
            return BOTH_INJURED4;
          else {
            if (ps->pm_flags & ANIM_ALERT)
              return BOTH_STAND2;
            else if (ps->pm_flags & ANIM_ALERT2) {
              if (upper)
                return TORSO_WEAPONREADY2;
              else
                return BOTH_STAND2;
            } else
              return BOTH_STAND4;
          }
          break;
          //1 handed weapon - "phaser"
        case WP_5:
        case WP_10:
          if (injured)
            return BOTH_INJURED4;
          else {
            if (ps->pm_flags & ANIM_ALERT && upper)
              return TORSO_WEAPONIDLE1;
            else if (ps->pm_flags & ANIM_ALERT2 && upper)
              return TORSO_WEAPONREADY1;
            else
              return BOTH_STAND1;
          }
          break;
          //Generic tools - "everything else"
        case WP_4:
          if (upper)
            return TORSO_COFFEE;
          else
            return BOTH_STAND1;
          break;
        default:
          if (injured)
            return BOTH_INJURED4;
          else
            return BOTH_STAND1;
          break;
      }
      break;

      //Called when player fires their weapon
    case ANIM_ATTACK:
      //2 handed weapon - "heavy"
      switch (weapon) {
        //case WP_7:
        case WP_8:
        case WP_9:
        case WP_7:
          if (ps->pm_flags & ANIM_ALERT2)
            return BOTH_ATTACK2;
          else
            return BOTH_ATTACK3;
          break;
          //2 handed weapon - "light"
        case WP_6:
          if (ps->pm_flags & ANIM_ALERT2)
            return BOTH_ATTACK2;
          else {
            if (upper)
              return BOTH_ATTACK3;
            else
              return BOTH_ATTACK3;
          }
          break;
          //1 handed weapon - "phaser"
        case WP_5:
        case WP_10:
        case WP_15:
        case WP_13:
          if (upper)
            return TORSO_WEAPONREADY1;
          else
            return BOTH_STAND1;
          break;
          //Other Tools "padd"
        case WP_3:
          if (upper)
            return TORSO_PADD1;
          else
            return BOTH_STAND1;
          break;
          //Other Tools "tricorder"
        case WP_2:
          if (upper) {
            if (!pm->medic)
              return TORSO_TRICORDER1;
            else
              return TORSO_MEDICORDER1;
          } else
            return BOTH_STAND1;
          break;
          //Other: "Medkit"
        case WP_11:
          if (upper)
            return TORSO_ACTIVATEMEDKIT1;
          else
            return BOTH_STAND1;
          break;
          //Other: "Hypo
        case WP_12:
          if (upper)
            return TORSO_HYPO1;
          else
            return BOTH_STAND1;
        case WP_4:
          if (upper)
            return TORSO_COFFEE;
          //break;
        default:
          if (upper)
            return TORSO_WEAPONREADY1;
          else
            return BOTH_STAND1;
          break;
      }
      break;

      //When the player jumps
    case ANIM_JUMP: return BOTH_JUMP1;
      //Wen the player jumps backwards
    case ANIM_JUMPB: return BOTH_JUMPBACK1;

      //When the player runs
    case ANIM_RUN:
      if (injured) {
        return BOTH_RUNINJURED1;
      }

      //2 handed weapons
      switch (weapon) {
        case WP_8:
        case WP_9:
        case WP_6:
        case WP_7:
          if (upper)
            return BOTH_RUN2;
          else
            return BOTH_RUN1;
          break;
        case WP_4:
          if (upper)
            return TORSO_COFFEE;
          //break;
          //EVERYTHING ELSE
        default: return BOTH_RUN1;
      }
      break;

      //When the player runs back
    case ANIM_RUNB:
      //2 handed weapons
      switch (weapon) {
        //case WP_7:
        case WP_8:
        case WP_9:
        case WP_6:
        case WP_7:
          if (upper)
            return BOTH_WALK2;
          else if (injured)
            return LEGS_WALKBACK1;
          else
            return LEGS_RUNBACK2;
          break;
          //EVERYTHING ELSE
        case WP_4:
          if (upper)
            return TORSO_COFFEE;
        default:
          if (upper)
            return BOTH_WALK1;
          else if (injured)
            return LEGS_WALKBACK1;
          else
            return LEGS_RUNBACK2;
          break;
      }
      break;

      //When the player walks
    case ANIM_WALK:
      if (ps->legsTimer > 0 && bg_emoteList[ps->legsTimer].enumName == BOTH_STAND3)
        return BOTH_WALK3;

      //2 handed weapons
      switch (weapon) {
        case WP_8:
        case WP_9:
        case WP_6:
        case WP_7:
          if (ps->pm_flags & ANIM_ALERT)
            return BOTH_WALK2;
          else if (ps->pm_flags & ANIM_ALERT2) {
            if (upper)
              return TORSO_WEAPONREADY2;
            else
              return BOTH_WALK2;
          } else
            return BOTH_WALK4;
          break;
          //Other Tools "everything else"
        case WP_4:
          if (upper)
            return TORSO_COFFEE;
        case WP_5:
        case WP_10:
          if (ps->pm_flags & ANIM_ALERT) {
            if (upper)
              return TORSO_WEAPONIDLE1;
          } else if (ps->pm_flags & ANIM_ALERT2) {
            if (upper)
              return TORSO_WEAPONREADY1;
          }

        default: return BOTH_WALK1;
          break;
      }
      break;

      //When the player walks baaaack
    case ANIM_WALKB:
      //2 handed weapons
      switch (weapon) {
        case WP_8:
        case WP_9:
        case WP_6:
        case WP_7:
          if (ps->pm_flags & ANIM_ALERT) {
            if (upper)
              return BOTH_WALK2;
            else
              return LEGS_WALKBACK1;
          } else if (ps->pm_flags & ANIM_ALERT2) {
            if (upper)
              return TORSO_WEAPONREADY2;
            else
              return LEGS_WALKBACK1;
          } else {
            if (upper)
              return BOTH_WALK4;
            else
              return LEGS_WALKBACK1;
          }
          break;
        case WP_4:
          if (upper)
            return TORSO_COFFEE;
          //break;
        case WP_5:
        case WP_10:
          if (ps->pm_flags & ANIM_ALERT && upper)
            return TORSO_WEAPONIDLE1;
          else if (ps->pm_flags & ANIM_ALERT2 && upper)
            return TORSO_WEAPONREADY1;

          //Other Tools "everything else"
        default:
          if (upper)
            return BOTH_WALK1;
          else
            return LEGS_WALKBACK1;
          break;
      }
      break;

      //When the player crouch walks
    case ANIM_CROUCHWALK:
      //2 handed weapons
      switch (weapon) {
        //case WP_7:
        case WP_6:
        case WP_7:
          if (upper)
            return TORSO_WEAPONREADY2;
          else
            return BOTH_CROUCH1WALK;
          break;
        case WP_8:
        case WP_9:
          if (ps->pm_flags & ANIM_ALERT2 && upper)
            return TORSO_WEAPONREADY2;
          else if (upper)
            return BOTH_WALK2;
          else
            return BOTH_CROUCH1WALK;
          break;
        case WP_4:
          if (upper)
            return TORSO_COFFEE;
          //break;
        case WP_5:
        case WP_10:
          if (ps->pm_flags & ANIM_ALERT && upper)
            return TORSO_WEAPONIDLE1;
          else if (ps->pm_flags & ANIM_ALERT2 && upper)
            return TORSO_WEAPONREADY1;
          //Other Tools "everything else"
        default: return BOTH_CROUCH1WALK;
          break;
      }
      break;

      //When the player crouch walks bak
    case ANIM_CROUCHWALKB:
      //2 handed weapons
      switch (weapon) {
        //case WP_7:
        case WP_8:
        case WP_9:
        case WP_6:
        case WP_7:
          if (ps->pm_flags & ANIM_ALERT2)
            return TORSO_WEAPONREADY2;
          else if (upper)
            return BOTH_WALK2;
          else
            return BOTH_CROUCH1WALK;
          break;
        case WP_4:
          if (upper)
            return TORSO_COFFEE;
        case WP_5:
        case WP_10:
          if (ps->pm_flags & ANIM_ALERT && upper)
            return TORSO_WEAPONIDLE1;
          else if (ps->pm_flags & ANIM_ALERT2 && upper)
            return TORSO_WEAPONREADY1;

          //Other Tools "everything else"
        default: return BOTH_CROUCH1WALK;
          break;
      }
      break;

    case ANIM_SWIM:
      if (!upper) {
        if (pm->cmd.forwardmove
            || pm->cmd.rightmove
            || pm->cmd.upmove) {
          return LEGS_SWIM;
        }
      }

      return BOTH_FLOAT1;

    case ANIM_FLY: return BOTH_FLOAT1;
  }

  return BOTH_STAND1;
}

/**
*	Adds a predictable event to playerstate
*/
void PM_AddEvent(int newEvent) {
  BG_AddPredictableEventToPlayerstate(newEvent, 0, pm->ps);
}

/*
==============
PM_Use

Generates a use event
==============
*/
#define USE_DELAY 2000

/**
*	Generates a use event
*/
void PM_Use(void) {
  playerState_t *ps = pm->ps;

  if (ps->useTime > 0)
    ps->useTime -= 100;

  if (ps->useTime > 0) {
    return;
  }

  if (!(pm->cmd.buttons & BUTTON_USE)) {
    pm->useEvent = 0;
    ps->useTime = 0;

    return;
  }

  pm->useEvent = EV_USE;
  ps->useTime = USE_DELAY;
}

/*
===============
PM_AddTouchEnt
===============
*/
/**
*	Adds a touchEnt event.
*/
void PM_AddTouchEnt(int entityNum) {
  int i;

  if (entityNum == ENTITYNUM_WORLD) {
    return;
  }
  if (pm->numtouch == MAXTOUCH) {
    return;
  }

  // see if it is already added
  for (i = 0; i < pm->numtouch; i++) {
    if (pm->touchents[i] == entityNum) {
      return;
    }
  }

  // add it
  pm->touchents[pm->numtouch] = entityNum;
  pm->numtouch++;
}

/**
* Start torso animation
*/

static void PM_StartTorsoAnim(int anim, qboolean overrideEmotes) {
  playerState_t *ps = pm->ps;
  if (ps->stats[EMOTES] & EMOTE_UPPER && !overrideEmotes) {
    return;
  }

  if (ps->pm_type >= PM_DEAD &&
      anim != BOTH_FALLDEATH1INAIR &&
      anim != BOTH_FALLDEATH1LAND) { //TiM: UberHack :P
    return;
  }

  if (ps->stats[TORSOTIMER] > 0) {
    return;        // a high priority animation is running
  }

  ps->stats[TORSOANIM] = ((ps->stats[TORSOANIM] & ANIM_TOGGLEBIT) ^ ANIM_TOGGLEBIT)
      | anim;
}

/**
*	Start leg animation
*/
static void PM_StartLegsAnim(int anim) {
  playerState_t *ps = pm->ps;

  if (ps->pm_type >= PM_DEAD &&
      anim != BOTH_FALLDEATH1INAIR &&
      anim != BOTH_FALLDEATH1LAND) {
    return;
  }

  ps->stats[LEGSANIM] = ((ps->stats[LEGSANIM] & ANIM_TOGGLEBIT) ^ ANIM_TOGGLEBIT)
      | anim;
}

/**
*	Continues the legs animation.
*/
static void PM_ContinueLegsAnim(int anim, qboolean overrideEmote) {
  playerState_t *ps = pm->ps;

  //override to return to idle after moving in an emote
  if ((ps->stats[EMOTES] & EMOTE_LOWER)
      && (!(ps->stats[EMOTES] & EMOTE_CLAMP_BODY) && !(ps->stats[EMOTES] & EMOTE_CLAMP_ALL)) && !overrideEmote) {
    if (ps->legsTimer > 0 && (ps->stats[LEGSANIM] & ~ANIM_TOGGLEBIT) != bg_emoteList[ps->legsTimer].enumName
        && (ps->stats[LEGSANIM] & ~ANIM_TOGGLEBIT) != BOTH_GET_UP1) {
      int anim2 =
          PM_GetAnim(ANIM_IDLE, ps->weapon, (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse, qfalse);

      ps->stats[LEGSANIM] = ((ps->stats[LEGSANIM] & ANIM_TOGGLEBIT) ^ ANIM_TOGGLEBIT) | anim2;
    }
  }

  if ((ps->stats[EMOTES] & EMOTE_CLAMP_BODY || ps->stats[EMOTES] & EMOTE_CLAMP_ALL) && !overrideEmote) { //EMOTE_LOWER
    return;
  }

  if ((ps->stats[LEGSANIM] & ~ANIM_TOGGLEBIT) == anim) {
    return;
  }

  if (ps->stats[LEGSTIMER] > 0 && !overrideEmote) { //legsTimer
    return;        // a high priority animation is running
  }
  PM_StartLegsAnim(anim);
}

/**
*	Continues the torso animation
*/
static void PM_ContinueTorsoAnim(int anim, qboolean overrideEmote) {
  playerState_t *ps = pm->ps;

  if (ps->stats[EMOTES] & EMOTE_UPPER && !overrideEmote) {
    return;
  }

  if ((ps->stats[TORSOANIM] & ~ANIM_TOGGLEBIT) == anim) {
    return;
  }

  if (ps->stats[TORSOTIMER] > 0) {
    return;        // a high priority animation is running
  }
  PM_StartTorsoAnim(anim, overrideEmote);
}

/**
*	Force a legs animation
*/
static void PM_ForceLegsAnim(int anim) {
  playerState_t *ps = pm->ps;

  //OMFG UBERHACK
  //I'm lazy... client revive spawns players 1 unit over the ground.
  //THat small fall enacts this, and subsequently screws up client revive animations
  if ((ps->stats[LEGSANIM] & ~ANIM_TOGGLEBIT) != BOTH_GET_UP1) {
    ps->stats[EMOTES] &= ~EMOTE_MASK_LOWER;
    ps->stats[LEGSTIMER] = 0; //legsTimer
  }

  PM_StartLegsAnim(anim);
}

/**
*	Force a torso animation
*/
static void PM_ForceTorsoAnim(int anim, qboolean overrideEmotes) {
  playerState_t *ps = pm->ps;

  if (overrideEmotes && (ps->stats[TORSOANIM] & ~ANIM_TOGGLEBIT) != BOTH_GET_UP1) {
    ps->stats[EMOTES] &= ~EMOTE_MASK_UPPER;
    ps->stats[TORSOTIMER] = 0;
  }

  PM_StartTorsoAnim(anim, overrideEmotes);
}

/*
==================
PM_ClipVelocity
==================
*/
/**
*	Slide off of the impacting surface
*/
void PM_ClipVelocity(vec3_t in, vec3_t normal, vec3_t out, float overbounce) {
  float backoff;
  float change;
  int i;

  backoff = DotProduct (in, normal);

  if (backoff < 0) {
    backoff *= overbounce;
  } else {
    backoff /= overbounce;
  }

  for (i = 0; i < 3; i++) {
    change = normal[i] * backoff;
    out[i] = in[i] - change;
  }
}


/*
==================
PM_Friction
==================
*/
/**
*	Handles both ground friction and water friction
*/
static void PM_Friction(void) {
  vec3_t vec;
  float *vel;
  float speed, newspeed, control;
  float drop;
  playerState_t *ps = pm->ps;

  vel = ps->velocity;

  VectorCopy(vel, vec);
  if (pml.walking) {
    vec[2] = 0;    // ignore slope movement
  }

  speed = VectorLength(vec);
  if (speed < 1) {
    vel[0] = 0;
    vel[1] = 0;        // allow sinking underwater
    // FIXME: still have z friction underwater?
    return;
  }

  drop = 0;

  // apply ground friction
  if ((pm->watertype & CONTENTS_LADDER) || pm->waterlevel <= 1) {
    if ((pm->watertype & CONTENTS_LADDER) || (pml.walking && !(pml.groundTrace.surfaceFlags & SURF_SLICK))) {
      // if getting knocked back, no friction
      if (!(ps->pm_flags & PMF_TIME_KNOCKBACK)) {
        control = speed < pm_stopspeed ? pm_stopspeed : speed;
        drop += control * pm_friction * pml.frametime;
      }
    }
  }

  // apply water friction even if just wading
  if (pm->waterlevel && !(pm->watertype & CONTENTS_LADDER)) {
    drop += speed * pm_waterfriction * pm->waterlevel * pml.frametime;
  }

  //RPG-X | Phenix | 8/8/2004
  //Apply EVOSUIT friction (small)
  if (ps->powerups[PW_EVOSUIT]) {
    drop += speed * pm_evosuitfriction * pml.frametime;
  }

  // apply flying friction
  if (ps->powerups[PW_FLIGHT] || ps->pm_type == PM_SPECTATOR) {
    drop += speed * pm_flightfriction * pml.frametime;
  }

  // scale the velocity
  newspeed = speed - drop;
  if (newspeed < 0) {
    newspeed = 0;
  }
  newspeed /= speed;

  vel[0] = vel[0] * newspeed;
  vel[1] = vel[1] * newspeed;
  vel[2] = vel[2] * newspeed;
}


/*
==============
PM_Accelerate
==============
*/
/**
*	Handles user intended acceleration
*/
static void PM_Accelerate(vec3_t wishdir, float wishspeed, float accel) {
#if 1
  // q2 style
  int i;
  float addspeed, accelspeed, currentspeed;

  currentspeed = DotProduct (pm->ps->velocity, wishdir);
  addspeed = wishspeed - currentspeed;
  if (addspeed <= 0) {
    return;
  }
  accelspeed = accel * pml.frametime * wishspeed;
  if (accelspeed > addspeed) {
    accelspeed = addspeed;
  }

  for (i = 0; i < 3; i++) {
    pm->ps->velocity[i] += accelspeed * wishdir[i];
  }
#else
                                                                                                                          // proper way (avoids strafe jump maxspeed bug), but feels bad
	vec3_t		wishVelocity;
	vec3_t		pushDir;
	float		pushLen;
	float		canPush;

	VectorScale( wishdir, wishspeed, wishVelocity );
	VectorSubtract( wishVelocity, pm->ps->velocity, pushDir );
	pushLen = VectorNormalize( pushDir );

	canPush = accel*pml.frametime*wishspeed;
	if (canPush > pushLen) {
		canPush = pushLen;
	}

	VectorMA( pm->ps->velocity, canPush, pushDir, pm->ps->velocity );
#endif
}



/*
============
PM_CmdScale
============
*/
/**
*	\return the scale factor to apply to cmd movements
*
*	This allows the clients to use axial -127 to 127 values for all directions
*	without getting a sqrt(2) distortion in speed.
*/
static float PM_CmdScale(usercmd_t *cmd) {
  int max;
  float total;
  float scale;

  max = abs(cmd->forwardmove);
  if (abs(cmd->rightmove) > max) {
    max = abs(cmd->rightmove);
  }
  if (abs(cmd->upmove) > max) {
    max = abs(cmd->upmove);
  }
  if (!max) {
    return 0;
  }

  total = sqrt(cmd->forwardmove * cmd->forwardmove
                   + cmd->rightmove * cmd->rightmove + cmd->upmove * cmd->upmove);
  scale = (float) pm->ps->speed * max / (127.0 * total);

  return scale;
}


/*
================
PM_SetMovementDir
================
*/
/**
*	Determines the rotation of the legs reletive
*	to the facing dir
*/
static void PM_SetMovementDir(void) {
  playerState_t *ps = pm->ps;
  usercmd_t *cmd = &pm->cmd;

  if (cmd->forwardmove || cmd->rightmove) {
    if (cmd->rightmove == 0 && cmd->forwardmove > 0) {
      ps->movementDir = 0;
    } else if (cmd->rightmove < 0 && cmd->forwardmove > 0) {
      ps->movementDir = 1;
    } else if (cmd->rightmove < 0 && cmd->forwardmove == 0) {
      ps->movementDir = 2;
    } else if (cmd->rightmove < 0 && cmd->forwardmove < 0) {
      ps->movementDir = 3;
    } else if (cmd->rightmove == 0 && cmd->forwardmove < 0) {
      ps->movementDir = 4;
    } else if (cmd->rightmove > 0 && cmd->forwardmove < 0) {
      ps->movementDir = 5;
    } else if (cmd->rightmove > 0 && cmd->forwardmove == 0) {
      ps->movementDir = 6;
    } else if (cmd->rightmove > 0 && cmd->forwardmove > 0) {
      ps->movementDir = 7;
    }
  } else {
    // if they aren't actively going directly sideways,
    // change the animation to the diagonal so they
    // don't stop too crooked
    if (ps->movementDir == 2) {
      ps->movementDir = 1;
    } else if (ps->movementDir == 6) {
      ps->movementDir = 7;
    }
  }
}


/*
=============
PM_CheckJump
=============
*/
/**
*	Checks if jumping is allowed
*/
static qboolean PM_CheckJump(void) {
  playerState_t *ps = pm->ps;

  if (ps->pm_flags & PMF_RESPAWNED) {
    return qfalse;        // don't allow jump until all buttons are up
  }

  if (pm->cmd.upmove < 10) {
    // not holding jump
    return qfalse;
  }

  // must wait for jump to be released
  if (ps->pm_flags & PMF_JUMP_HELD) {
    // clear upmove so cmdscale doesn't lower running speed
    pm->cmd.upmove = 0;
    return qfalse;
  }

  pml.groundPlane = qfalse;        // jumping away
  pml.walking = qfalse;
  ps->pm_flags |= PMF_JUMP_HELD;

  ps->groundEntityNum = ENTITYNUM_NONE;
  ps->velocity[2] = JUMP_VELOCITY;

  PM_AddEvent(EV_JUMP);

  if (pm->cmd.forwardmove >= 0) {
    PM_ForceLegsAnim(PM_GetAnim(ANIM_JUMP,
                                ps->weapon,
                                (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                qfalse)); //BOTH_JUMP
    if (ps->weaponstate == WEAPON_READY)
      PM_ForceTorsoAnim(PM_GetAnim(ANIM_JUMP,
                                   ps->weapon,
                                   (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                   qtrue), qtrue);
    ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
  } else {
    if (ps->weaponstate == WEAPON_READY)
      PM_ForceTorsoAnim(PM_GetAnim(ANIM_JUMPB,
                                   ps->weapon,
                                   (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                   qfalse), qtrue);
    PM_ForceLegsAnim(PM_GetAnim(ANIM_JUMPB,
                                ps->weapon,
                                (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                qtrue)); //LEGS_JUMPB
    ps->pm_flags |= PMF_BACKWARDS_JUMP;
  }

  return qtrue;
}

/*
=============
PM_CheckWaterJump
=============
*/
/**
*	Checks if jumping out of water is allowed
*/
static qboolean PM_CheckWaterJump(void) {
  vec3_t spot;
  int cont;
  vec3_t flatforward;
  playerState_t *ps = pm->ps;

  if (ps->pm_time) {
    return qfalse;
  }

  // check for water jump
  if (pm->waterlevel != 2) {
    return qfalse;
  }

  if (pm->watertype & CONTENTS_LADDER) {
    if (ps->velocity[2] <= 0)
      return qfalse;
  }

  flatforward[0] = pml.forward[0];
  flatforward[1] = pml.forward[1];
  flatforward[2] = 0;
  VectorNormalize(flatforward);

  VectorMA (ps->origin, 30, flatforward, spot);
  spot[2] += 4;
  cont = pm->pointcontents(spot, ps->clientNum);
  if (!(cont & CONTENTS_SOLID)) {
    return qfalse;
  }

  spot[2] += 16;
  cont = pm->pointcontents(spot, ps->clientNum);
  if (cont) {
    return qfalse;
  }

  // jump out of water
  VectorScale (pml.forward, 200, ps->velocity);
  ps->velocity[2] = 350;

  ps->pm_flags |= PMF_TIME_WATERJUMP;
  ps->pm_time = 2000;

  return qtrue;
}

//============================================================================


/*
===================
PM_WaterJumpMove
===================
*/
/**
*	Flying out of the water
*/
static void PM_WaterJumpMove(void) {
  playerState_t *ps = pm->ps;
  // waterjump has no control, but falls

  PM_StepSlideMove(qtrue);

  ps->velocity[2] -= ps->gravity * pml.frametime;
  if (ps->velocity[2] < 0) {
    // cancel as soon as we are falling down again
    ps->pm_flags &= ~PMF_ALL_TIMES;
    ps->pm_time = 0;
  }
}

/*
===================
PM_WaterMove

===================
*/
/**
*	Handles movement in water
*/
static void PM_WaterMove(void) {
  int i;
  vec3_t wishvel;
  float wishspeed;
  vec3_t wishdir;
  float scale;
  float vel;
  playerState_t *ps = pm->ps;

  if (PM_CheckWaterJump()) {
    PM_WaterJumpMove();
    return;
  }
#if 0
                                                                                                                          // jump = head for surface
	if ( pm->cmd.upmove >= 10 ) {
		if (ps->velocity[2] > -300) {
			if ( pm->watertype == CONTENTS_WATER ) {
				ps->velocity[2] = 100;
			} else if (pm->watertype == CONTENTS_SLIME) {
				ps->velocity[2] = 80;
			} else {
				ps->velocity[2] = 50;
			}
		}
	}
#endif
  PM_Friction();

  scale = PM_CmdScale(&pm->cmd);
  //
  // user intentions
  //
  if (!scale) {
    wishvel[0] = 0;
    wishvel[1] = 0;
    if (pm->watertype & CONTENTS_LADDER) {
      wishvel[2] = 0;
    } else {
      wishvel[2] = -60;        // sink towards bottom
    }
  } else {
    for (i = 0; i < 3; i++) {
      wishvel[i] = scale * pml.forward[i] * pm->cmd.forwardmove + scale * pml.right[i] * pm->cmd.rightmove;
    }
    wishvel[2] += scale * pm->cmd.upmove;
  }

  VectorCopy (wishvel, wishdir);
  wishspeed = VectorNormalize(wishdir);

  if (pm->watertype & CONTENTS_LADDER)    //ladder
  {
    if (wishspeed > ps->speed * pm_ladderScale) {
      wishspeed = ps->speed * pm_ladderScale;
    }
    PM_Accelerate(wishdir, wishspeed, pm_flyaccelerate);
  } else {
    if (wishspeed > ps->speed * pm_swimScale) {
      wishspeed = ps->speed * pm_swimScale;
    }
    PM_Accelerate(wishdir, wishspeed, pm_wateraccelerate);
  }

  // make sure we can go up slopes easily under water
  if (pml.groundPlane && DotProduct(ps->velocity, pml.groundTrace.plane.normal) < 0) {
    vel = VectorLength(ps->velocity);
    // slide along the ground plane
    PM_ClipVelocity(ps->velocity, pml.groundTrace.plane.normal,
                    ps->velocity, OVERCLIP);

    VectorNormalize(ps->velocity);
    VectorScale(ps->velocity, vel, ps->velocity);
  }

  PM_SlideMove(qfalse);
}



/*
===================
PM_FlyMove

Only with the flight powerup
TiM: Good... if this handles
spectators too, I'm sunk >.<
Oh crap... it does lol
===================
*/
/**
*	Handles fly movement (e.g. flight powerup or spectator movement)
*/
static void PM_FlyMove(void) {
  int i;
  vec3_t wishvel;
  float wishspeed;
  vec3_t wishdir;
  float scale;
  playerState_t *ps = pm->ps;

  // normal slowdown
  PM_Friction();

  scale = PM_CmdScale(&pm->cmd);
  //
  // user intentions
  //
  if (!scale) {
    wishvel[0] = 0;
    wishvel[1] = 0;
    wishvel[2] = 0;
  } else {
    for (i = 0; i < 3; i++) {
      if (ps->pm_type == PM_SPECTATOR) {
        wishvel[i] = scale * pml.forward[i] * pm->cmd.forwardmove + scale * pml.right[i] * pm->cmd.rightmove;
      } else {
        //TiM - Vertical is no longer a hardcoded constant direction
        wishvel[i] = scale * pml.forward[i] * pm->cmd.forwardmove +
            scale * pml.right[i] * pm->cmd.rightmove +
            scale * pml.up[i] * pm->cmd.upmove;

        //TiM - Set directions
        PM_SetMovementDir();
      }
    }

    if (ps->pm_type == PM_SPECTATOR)
      wishvel[2] += scale * pm->cmd.upmove;
  }

  VectorCopy (wishvel, wishdir);
  wishspeed = VectorNormalize(wishdir);

  PM_Accelerate(wishdir, wishspeed, pm_flyaccelerate);

  if (ps->weaponstate == WEAPON_READY)
    PM_ContinueTorsoAnim(PM_GetAnim(ANIM_FLY,
                                    ps->weapon,
                                    (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                    qtrue), qfalse);
  PM_ContinueLegsAnim(PM_GetAnim(ANIM_FLY,
                                 ps->weapon,
                                 (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                 qfalse), qtrue);

  PM_StepSlideMove(qfalse);
}


/*
===================
PM_AirMove

===================
*/
/**
*	Handles movement during air time (e.g. falling)
*/
static void PM_AirMove(void) {
  int i;
  vec3_t wishvel;
  float fmove, smove;
  vec3_t wishdir;
  float wishspeed;
  float scale;
  usercmd_t cmd;

  PM_Friction();

  fmove = pm->cmd.forwardmove;
  smove = pm->cmd.rightmove;

  cmd = pm->cmd;
  scale = PM_CmdScale(&cmd);

  // set the movementDir so clients can rotate the legs for strafing
  PM_SetMovementDir();

  // project moves down to flat plane
  pml.forward[2] = 0;
  pml.right[2] = 0;
  VectorNormalize(pml.forward);
  VectorNormalize(pml.right);

  for (i = 0; i < 2; i++) {
    wishvel[i] = pml.forward[i] * fmove + pml.right[i] * smove;
  }
  wishvel[2] = 0;

  VectorCopy (wishvel, wishdir);
  wishspeed = VectorNormalize(wishdir);
  wishspeed *= scale;

  // not on ground, so little effect on velocity
  PM_Accelerate(wishdir, wishspeed, pm_airaccelerate);

  // we may have a ground plane that is very steep, even
  // though we don't have a groundentity
  // slide along the steep plane
  if (pml.groundPlane) {
    PM_ClipVelocity(pm->ps->velocity, pml.groundTrace.plane.normal,
                    pm->ps->velocity, OVERCLIP);
  }

  PM_StepSlideMove(qtrue);
}

/*
===================
PM_WalkMove
===================
*/
/**
*	Handles walk movement
*/
static void PM_WalkMove(void) {
  int i;
  vec3_t wishvel;
  float fmove, smove;
  vec3_t wishdir;
  float wishspeed;
  float scale;
  usercmd_t cmd;
  float accelerate;
  float vel;
  playerState_t *ps = pm->ps;

  if (pm->waterlevel > 2 && DotProduct(pml.forward, pml.groundTrace.plane.normal) > 0) {
    // begin swimming
    PM_WaterMove();
    return;
  }

  if (PM_CheckJump()) {
    // jumped away
    if (pm->waterlevel > 1) {
      PM_WaterMove();
    } else {
      PM_AirMove();
    }
    return;
  }

  PM_Friction();

  fmove = pm->cmd.forwardmove;
  smove = pm->cmd.rightmove;

  cmd = pm->cmd;
  scale = PM_CmdScale(&cmd);

  // set the movementDir so clients can rotate the legs for strafing
  PM_SetMovementDir();

  // project moves down to flat plane
  pml.forward[2] = 0;
  pml.right[2] = 0;

  // project the forward and right directions onto the ground plane
  PM_ClipVelocity(pml.forward, pml.groundTrace.plane.normal, pml.forward, OVERCLIP);
  PM_ClipVelocity(pml.right, pml.groundTrace.plane.normal, pml.right, OVERCLIP);
  //
  VectorNormalize(pml.forward);
  VectorNormalize(pml.right);

  for (i = 0; i < 3; i++) {
    wishvel[i] = pml.forward[i] * fmove + pml.right[i] * smove;
  }
  // when going up or down slopes the wish velocity should Not be zero

  VectorCopy (wishvel, wishdir);
  wishspeed = VectorNormalize(wishdir);
  wishspeed *= scale;

  // clamp the speed lower if ducking
  if (ps->pm_flags & PMF_DUCKED) {
    if (wishspeed > ps->speed * pm_duckScale) {
      wishspeed = ps->speed * pm_duckScale;
    }
  }

  // clamp the speed lower if wading or walking on the bottom
  if (pm->waterlevel) {
    float waterScale;

    waterScale = pm->waterlevel / 3.0;
    waterScale = 1.0 - (1.0 - pm_swimScale) * waterScale;
    if (wishspeed > ps->speed * waterScale) {
      wishspeed = ps->speed * waterScale;
    }
  }

  // when a player gets hit, they temporarily lose
  // full control, which allows them to be moved a bit
  if ((pml.groundTrace.surfaceFlags & SURF_SLICK) || ps->pm_flags & PMF_TIME_KNOCKBACK) {
    accelerate = pm_airaccelerate;
  } else {
    accelerate = pm_accelerate;
  }

  PM_Accelerate(wishdir, wishspeed, accelerate);

  if ((pml.groundTrace.surfaceFlags & SURF_SLICK) || ps->pm_flags & PMF_TIME_KNOCKBACK) {
    ps->velocity[2] -= ps->gravity * pml.frametime;
  }

  vel = VectorLength(ps->velocity);

  // slide along the ground plane
  PM_ClipVelocity(ps->velocity, pml.groundTrace.plane.normal,
                  ps->velocity, OVERCLIP);

  // don't decrease velocity when going up or down a slope
  VectorNormalize(ps->velocity);
  VectorScale(ps->velocity, vel, ps->velocity);

  // don't do anything if standing still
  if (!ps->velocity[0] && !ps->velocity[1]) {
    return;
  }

  PM_StepSlideMove(qfalse);
}


/*
==============
PM_DeadMove
==============
*/
/**
*	Handles movement while dead
*/
static void PM_DeadMove(void) {
  float forward;
  playerState_t *ps = pm->ps;

  if (!pml.walking) {
    return;
  }

  // extra friction

  forward = VectorLength(ps->velocity);
  forward -= 20;
  if (forward <= 0) {
    VectorClear (ps->velocity);
  } else {
    VectorNormalize(ps->velocity);
    VectorScale (ps->velocity, forward, ps->velocity);
  }
}


/*
===============
PM_NoclipMove
===============
*/
/**
*	Handles noclip movement
*/
static void PM_NoclipMove(void) {
  float speed, drop, friction, control, newspeed;
  int i;
  vec3_t wishvel;
  float fmove, smove;
  vec3_t wishdir;
  float wishspeed;
  float scale;
  playerState_t *ps = pm->ps;

  ps->viewheight = DEFAULT_VIEWHEIGHT;

  // friction

  speed = VectorLength(ps->velocity);
  if (speed < 1) {
    VectorCopy (vec3_origin, ps->velocity);
  } else {
    drop = 0;

    friction = pm_friction * 1.5;    // extra friction
    control = speed < pm_stopspeed ? pm_stopspeed : speed;
    drop += control * friction * pml.frametime;

    // scale the velocity
    newspeed = speed - drop;
    if (newspeed < 0)
      newspeed = 0;
    newspeed /= speed;

    VectorScale (ps->velocity, newspeed, ps->velocity);
  }

  // accelerate
  scale = PM_CmdScale(&pm->cmd);

  fmove = pm->cmd.forwardmove;
  smove = pm->cmd.rightmove;

  for (i = 0; i < 3; i++)
    wishvel[i] = pml.forward[i] * fmove + pml.right[i] * smove;
  wishvel[2] += pm->cmd.upmove;

  VectorCopy (wishvel, wishdir);
  wishspeed = VectorNormalize(wishdir);
  wishspeed *= scale;

  PM_Accelerate(wishdir, wishspeed, pm_accelerate);

  // move
  VectorMA (ps->origin, pml.frametime, ps->velocity, ps->origin);
}


/*
===================
PM_FreezeMove
===================
*/
/**
*	Handles movement whole freezed
*/
static void PM_FreezeMove(void) {
  trace_t trace;
  short temp, i;
  vec3_t moveto;
  playerState_t *ps = pm->ps;

  pm->mins[0] = DEFAULT_MINS_0; //-15
  pm->mins[1] = DEFAULT_MINS_1;

  pm->maxs[0] = DEFAULT_MAXS_0; //15
  pm->maxs[1] = DEFAULT_MAXS_1;

  pm->mins[2] = MINS_Z;
  // stand up if possible
  if (ps->pm_flags & PMF_DUCKED) {
    // try to stand up
    pm->maxs[2] = 36; //32
    pm->trace(&trace, ps->origin, pm->mins, pm->maxs, ps->origin, ps->clientNum, pm->tracemask);
    if (!trace.allsolid)
      ps->pm_flags &= ~PMF_DUCKED;
  }

  if (ps->pm_flags & PMF_DUCKED && (!(ps->stats[EMOTES] & EMOTE_LOWER) && !ps->powerups[PW_FLIGHT]
      && !((ps->powerups[PW_EVOSUIT]) && (ps->gravity == 0)))) {
    pm->maxs[2] = 16;
    ps->viewheight = CROUCH_VIEWHEIGHT;
  } else {
    if (ps->stats[EMOTES] & EMOTE_LOWER && ps->legsTimer > 0) {
      pm->maxs[2] = bg_emoteList[ps->legsTimer].hitBoxHeight;
      ps->viewheight = bg_emoteList[ps->legsTimer].viewHeight;
    } else {
      ps->viewheight = DEFAULT_VIEWHEIGHT;
    }
  }

  // circularly clamp the angles with deltas
  for (i = 0; i < 3; i++) {
    temp = pm->cmd.angles[i] + ps->delta_angles[i];
    if (i == PITCH) {
      // don't let the player look up or down more than 90 degrees
      if (temp > 16000) {
        ps->delta_angles[i] = 16000 - pm->cmd.angles[i];
        temp = 16000;
      } else if (temp < -16000) {
        ps->delta_angles[i] = -16000 - pm->cmd.angles[i];
        temp = -16000;
      }
    }
  }

  VectorCopy (ps->origin, moveto);
  moveto[2] -= 16;

  // test the player position if they were a stepheight higher
  pm->trace(&trace, ps->origin, pm->mins, pm->maxs, moveto, ps->clientNum, pm->tracemask);
  if (trace.fraction
      < 1.0) {    // Something just below, snap to it, to prevent a little "hop" after the holodeck fades in.
    VectorCopy (trace.endpos, ps->origin);
    ps->groundEntityNum = trace.entityNum;

    // Touch it.
    PM_AddTouchEnt(trace.entityNum);

  }
}


//============================================================================

//RPG-X | GSIO01 | 20/05/2009:
/**
*	Get the corresponding landing sound for each surface type
*/
static int PM_LandsoundForSurface(int fallType) {
  if (pm->ps->stats[PW_INVIS])
    return 0;

  switch (fallType) {
    case 1:
      if (pml.groundTrace.surfaceFlags & SURF_GRASS)
        return EV_FALL_MEDIUM_GRASS;
      else if (pml.groundTrace.surfaceFlags & SURF_GRAVEL)
        return EV_FALL_MEDIUM_GRAVEL;
      else if (pml.groundTrace.surfaceFlags & SURF_SNOW)
        return EV_FALL_MEDIUM_SNOW;
      else if (pml.groundTrace.surfaceFlags & SURF_WOOD)
        return EV_FALL_MEDIUM_WOOD;
      else
        return EV_FALL_MEDIUM;
      break;
    case 2:
      if (pml.groundTrace.surfaceFlags & SURF_GRASS)
        return EV_FALL_FAR_GRASS;
      else if (pml.groundTrace.surfaceFlags & SURF_GRAVEL)
        return EV_FALL_FAR_GRAVEL;
      else if (pml.groundTrace.surfaceFlags & SURF_SNOW)
        return EV_FALL_FAR_SNOW;
      else if (pml.groundTrace.surfaceFlags & SURF_WOOD)
        return EV_FALL_FAR_WOOD;
      else
        return EV_FALL_FAR;
      break;
    default:
      if (pml.groundTrace.surfaceFlags & SURF_GRASS)
        return EV_FALL_SHORT_GRASS;
      else if (pml.groundTrace.surfaceFlags & SURF_GRAVEL)
        return EV_FALL_SHORT_GRAVEL;
      else if (pml.groundTrace.surfaceFlags & SURF_SNOW)
        return EV_FALL_SHORT_SNOW;
      else if (pml.groundTrace.surfaceFlags & SURF_WOOD)
        return EV_FALL_SHORT_WOOD;
      else
        return EV_FALL_SHORT;
      break;
  }
}

/*
================
PM_FootstepForSurface
================
*/
/**
*	\return an event number apropriate for the groundsurface
*/
static int PM_FootstepForSurface(void) {
  //cloaked people make no noise
  if (pml.groundTrace.surfaceFlags & SURF_NOSTEPS || pm->ps->stats[PW_INVIS]) {
    return 0;
  }
  if (pml.groundTrace.surfaceFlags & SURF_METALSTEPS) {
    return EV_FOOTSTEP_METAL;
  }
  //RPG-X | GSIO01 | 20.05.2009 | START MOD
  if (pml.groundTrace.surfaceFlags & SURF_GRASS) {
    return EV_FOOTSTEP_GRASS;
  }
  if (pml.groundTrace.surfaceFlags & SURF_GRAVEL) {
    return EV_FOOTSTEP_GRAVEL;
  }
  if (pml.groundTrace.surfaceFlags & SURF_SNOW) {
    return EV_FOOTSTEP_SNOW;
  }
  if (pml.groundTrace.surfaceFlags & SURF_WOOD) {
    return EV_FOOTSTEP_WOOD;
  }
  //RPG-X | GSIO01 | 20.05.2009 | END MOD
  return EV_FOOTSTEP;
}


/*
=================
PM_CrashLand
=================
*/
/**
*	Check for hard landings that generate sound events
*/
static void PM_CrashLand(void) {
  float delta;
  float dist;
  float vel, acc;
  float t;
  float a, b, c, den;
  playerState_t *ps = pm->ps;

  // calculate the exact velocity on landing
  dist = ps->origin[2] - pml.previous_origin[2];
  vel = pml.previous_velocity[2];
  acc = -ps->gravity;

  a = acc / 2;
  b = vel;
  c = -dist;

  den = b * b - 4 * a * c;
  if (den < 0) {
    return;
  }
  t = (-b - sqrt(den)) / (2 * a);

  delta = vel + t * acc;
  delta = delta * delta * 0.0001;

  // ducking while falling doubles damage
  if (ps->pm_flags & PMF_DUCKED) {
    delta *= 2;
  }

  // never take falling damage if completely underwater
  if (pm->waterlevel == 3) {
    return;
  }

  // reduce falling damage if there is standing water
  if (pm->waterlevel == 2) {
    delta *= 0.25;
  }
  if (pm->waterlevel == 1) {
    delta *= 0.5;
  }

  if (delta < 1) {
    return;
  }

  // create a local entity event to play the sound

  // SURF_NODAMAGE is used for bounce pads where you don't ever
  // want to take damage or play a crunch sound
  if (!(pml.groundTrace.surfaceFlags & SURF_NODAMAGE)) {
    if (delta > 55
        || ps->stats[STAT_HEALTH] <= 1) { //60 //TiM a bit hacky, but I want this to play any time we fall when dead
      PM_AddEvent(PM_LandsoundForSurface(2)); //GSIO01 | 20/05/2009
    } else if (delta > 35) { //40
      // this is a pain grunt, so don't play it if dead
      if (ps->stats[STAT_HEALTH] > 1) { //0
        PM_AddEvent(PM_LandsoundForSurface(1)); //GSIO01 | 20/05/2009
      }
    } else if (delta > 5) { //7
      PM_AddEvent(PM_LandsoundForSurface(0)); //GSIO01 | 20/05/2009
    } else {
      PM_AddEvent(PM_FootstepForSurface());
    }
  }

  // start footstep cycle over
  ps->bobCycle = 0; //TiM: was commented out... :P
}

/*
=============
PM_CorrectAllSolid
=============
*/
static void PM_CorrectAllSolid(void) {
  if (pm->debugLevel) {
    Com_Printf("%i:allsolid\n", c_pmove);
  }

  // FIXME: jitter around

  pm->ps->groundEntityNum = ENTITYNUM_NONE;
  pml.groundPlane = qfalse;
  pml.walking = qfalse;
}


/*
=============
PM_GroundTraceMissed
=============
*/
/**
*	The ground trace didn't hit a surface, so we are in freefall
*/
static void PM_GroundTraceMissed(void) {
  trace_t trace;
  vec3_t point;
  playerState_t *ps = pm->ps;

  if (ps->groundEntityNum != ENTITYNUM_NONE) {
    // we just transitioned into freefall
    if (pm->debugLevel) {
      Com_Printf("%i:lift\n", c_pmove);
    }

    // if they aren't in a jumping animation and the ground is a ways away, force into it
    // if we didn't do the trace, the player would be backflipping down staircases
    VectorCopy(ps->origin, point);
    point[2] -= 64;

    pm->trace(&trace, ps->origin, pm->mins, pm->maxs, point, ps->clientNum, pm->tracemask);
    if (trace.fraction == 1.0 && (ps->stats[LEGSANIM] & ~ANIM_TOGGLEBIT) != BOTH_GET_UP1) {
      if (pm->cmd.forwardmove >= 0) {
        PM_ForceLegsAnim(PM_GetAnim(ANIM_JUMP,
                                    ps->weapon,
                                    (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                    qfalse));
        if (ps->weaponstate == WEAPON_READY)
          PM_ForceTorsoAnim(PM_GetAnim(ANIM_JUMP,
                                       ps->weapon,
                                       (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                       qtrue),
                            qtrue);
        ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
      } else {
        PM_ForceLegsAnim(PM_GetAnim(ANIM_JUMPB,
                                    ps->weapon,
                                    (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                    qfalse));
        if (ps->weaponstate == WEAPON_READY)
          PM_ForceTorsoAnim(PM_GetAnim(ANIM_JUMPB,
                                       ps->weapon,
                                       (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                       qtrue),
                            qtrue);
        ps->pm_flags |= PMF_BACKWARDS_JUMP;
      }

    }
  }

  ps->groundEntityNum = ENTITYNUM_NONE;
  pml.groundPlane = qfalse;
  pml.walking = qfalse;
}


/*
=============
PM_GroundTrace
=============
*/
/**
*	Does ad trace to the ground
*/
static void PM_GroundTrace(void) {
  vec3_t point;
  trace_t trace;
  playerState_t *ps = pm->ps;

  point[0] = ps->origin[0];
  point[1] = ps->origin[1];
  point[2] = ps->origin[2] - 0.25;

  pm->trace(&trace, ps->origin, pm->mins, pm->maxs, point, ps->clientNum, pm->tracemask);
  pml.groundTrace = trace;

  // do something corrective if the trace starts in a solid...
  if (trace.allsolid) {
    PM_CorrectAllSolid();
    return;
  }

  // if the trace didn't hit anything, we are in free fall
  if (trace.fraction == 1.0) {
    PM_GroundTraceMissed();
    pml.groundPlane = qfalse;
    pml.walking = qfalse;
    return;
  }

  // check if getting thrown off the ground
  if (ps->velocity[2] > 0 && DotProduct(ps->velocity, trace.plane.normal) > 10
      && (ps->stats[LEGSANIM] & ~ANIM_TOGGLEBIT) != BOTH_GET_UP1) {
    if (pm->debugLevel) {
      Com_Printf("%i:kickoff\n", c_pmove);
    }
    // go into jump animation
    if (pm->cmd.forwardmove >= 0) {
      PM_ForceLegsAnim(PM_GetAnim(ANIM_JUMP,
                                  ps->weapon,
                                  (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                  qfalse));
      if (ps->weaponstate == WEAPON_READY)
        PM_ForceTorsoAnim(PM_GetAnim(ANIM_JUMP,
                                     ps->weapon,
                                     (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                     qtrue),
                          qtrue);
      ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
    } else {
      PM_ForceLegsAnim(PM_GetAnim(ANIM_JUMPB,
                                  ps->weapon,
                                  (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                  qfalse));
      if (ps->weaponstate == WEAPON_READY)
        PM_ForceTorsoAnim(PM_GetAnim(ANIM_JUMPB,
                                     ps->weapon,
                                     (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                     qtrue),
                          qtrue);
      ps->pm_flags |= PMF_BACKWARDS_JUMP;
    }

    ps->groundEntityNum = ENTITYNUM_NONE;
    pml.groundPlane = qfalse;
    pml.walking = qfalse;
    return;
  }

  // slopes that are too steep will not be considered onground
  if (trace.plane.normal[2] < MIN_WALK_NORMAL) {
    if (pm->debugLevel) {
      Com_Printf("%i:steep\n", c_pmove);
    }
    // FIXME: if they can't slide down the slope, let them
    // walk (sharp crevices)
    ps->groundEntityNum = ENTITYNUM_NONE;
    pml.groundPlane = qtrue;
    pml.walking = qfalse;
    return;
  }

  pml.groundPlane = qtrue;
  pml.walking = qtrue;

  // hitting solid ground will end a waterjump
  if (ps->pm_flags & PMF_TIME_WATERJUMP) {
    ps->pm_flags &= ~(PMF_TIME_WATERJUMP | PMF_TIME_LAND);
    ps->pm_time = 0;
  }

  if (ps->groundEntityNum == ENTITYNUM_NONE) {
    // just hit the ground
    if (pm->debugLevel) {
      Com_Printf("%i:Land\n", c_pmove);
    }

    PM_CrashLand();

    // don't do landing time if we were just going down a slope
    if (pml.previous_velocity[2] < -200) {
      // don't allow another jump for a little while
      ps->pm_flags |= PMF_TIME_LAND;
      ps->pm_time = 250;
    }
  }

  ps->groundEntityNum = trace.entityNum;

  PM_AddTouchEnt(trace.entityNum);
}


/*
=============
PM_SetWaterLevel	FIXME: avoid this twice?  certainly if not moving
=============
*/
/**
*	Set water level
*/
static void PM_SetWaterLevel(void) {
  vec3_t point;
  int cont;
  int sample1;
  int sample2;
  playerState_t *ps = pm->ps;

  //
  // get waterlevel, accounting for ducking
  //
  pm->waterlevel = 0;
  pm->watertype = 0;

  point[0] = ps->origin[0];
  point[1] = ps->origin[1];
  point[2] = ps->origin[2] + MINS_Z + 1;
  cont = pm->pointcontents(point, ps->clientNum);

  if (cont & (MASK_WATER | CONTENTS_LADDER)) {
    sample2 = ps->viewheight - MINS_Z;
    sample1 = sample2 / 2;

    pm->watertype = cont;
    pm->waterlevel = 1;
    point[2] = ps->origin[2] + MINS_Z + sample1;
    cont = pm->pointcontents(point, ps->clientNum);
    if (cont & (MASK_WATER | CONTENTS_LADDER)) {
      pm->waterlevel = 2;
      point[2] = ps->origin[2] + MINS_Z + sample2;
      cont = pm->pointcontents(point, ps->clientNum);
      if (cont & (MASK_WATER | CONTENTS_LADDER)) {
        pm->waterlevel = 3;
      }
    }
  }

}



/*
==============
PM_CheckDuck
==============
*/
/**
*	Sets mins, maxs, and pm->ps->viewheight
*/
static void PM_CheckDuck(void) {
  trace_t trace;
  playerState_t *ps = pm->ps;

  pm->mins[0] = DEFAULT_MINS_0; //-15
  pm->mins[1] = DEFAULT_MINS_1;

  pm->maxs[0] = DEFAULT_MAXS_0;
  pm->maxs[1] = DEFAULT_MAXS_1;

  pm->mins[2] = MINS_Z;

  if (ps->pm_type == PM_DEAD) {
    pm->maxs[2] = -8;
    ps->viewheight = DEAD_VIEWHEIGHT;
    return;
  }

  if (pm->cmd.upmove < 0) {    // duck
    ps->pm_flags |= PMF_DUCKED;
  } else {    // stand up if possible
    if (ps->pm_flags & PMF_DUCKED) {
      // try to stand up
      pm->maxs[2] = 36; //32
      pm->trace(&trace, ps->origin, pm->mins, pm->maxs, ps->origin, ps->clientNum, pm->tracemask);
      if (!trace.allsolid)
        ps->pm_flags &= ~PMF_DUCKED;
    }
  }

  if (ps->pm_flags & PMF_DUCKED && !((ps->stats[EMOTES] & EMOTE_LOWER) || ps->powerups[PW_FLIGHT]
      || (ps->powerups[PW_EVOSUIT] && ps->gravity == 0))) {
    pm->maxs[2] = 16;
    ps->viewheight = CROUCH_VIEWHEIGHT;
  } else {
    if (ps->stats[EMOTES] & EMOTE_LOWER && ps->legsTimer > 0) {
      pm->maxs[2] = bg_emoteList[ps->legsTimer].hitBoxHeight;
      ps->viewheight = bg_emoteList[ps->legsTimer].viewHeight;
    } else {
      pm->maxs[2] = 36;
      pm->mins[2] = MINS_Z;
      ps->viewheight = DEFAULT_VIEWHEIGHT;
    }
  }
}



//===================================================================

//static qboolean ps->didFly;
//static

/*
===============
PM_Footsteps
===============
*/
/**
*	Does what it name suggests it handles footsteps.
*/
static void PM_Footsteps(void) {
  float bobmove;
  int old;
  qboolean footstep;
  playerState_t *ps = pm->ps;

  //
  // calculate speed and cycle to be used for
  // all cyclic walking effects
  //
  pm->xyspeed = sqrt(ps->velocity[0] * ps->velocity[0]
                         + ps->velocity[1] * ps->velocity[1]);

  pm->xyzspeed = sqrt(ps->velocity[0] * ps->velocity[0] //XVel - left + right
                          + ps->velocity[1] * ps->velocity[1]               //YVel - forward + back
                          + ps->velocity[2] * ps->velocity[2]);               //ZVel - up + down

  //RPG-X : TiM *****************************************************
  //Cheesy Halo style death flying!
  if (ps->stats[STAT_HEALTH] <= 1 && !ps->powerups[PW_QUAD] && !ps->powerups[PW_BEAM_OUT]) { //if dead
    /*TiM: clip brushes register as ENTITYNUM_NONE
		(So if they landed on a shuttle model for instance, the fall anim would still loop O_o )
		so they gotta be moving as well to trigger this*/

    if (ps->groundEntityNum == ENTITYNUM_NONE && pm->xyzspeed && pm->waterlevel < 2) {

      ps->pm_flags |= ANIM_DIDFLY;

      PM_ContinueLegsAnim(BOTH_FALLDEATH1INAIR, qtrue);

      if (ps->weaponstate == WEAPON_READY) {
        PM_ContinueTorsoAnim(BOTH_FALLDEATH1INAIR, qtrue);
      }
    } else {
      if (ps->pm_flags & ANIM_DIDFLY) {
        //TiM: Save flags.  Use anim nums if possible
        PM_ContinueLegsAnim(BOTH_FALLDEATH1LAND, qtrue);

        if (ps->weaponstate == WEAPON_READY) {
          PM_ContinueTorsoAnim(BOTH_FALLDEATH1LAND, qtrue);
        }

      }
    }
    return;
  } else { //Reset splat boolean
    if (ps->pm_flags & ANIM_DIDFLY && ps->pm_type != PM_DEAD) {
      ps->pm_flags &= ~ANIM_DIDFLY;
    }
  }

  //RPG-X : TiM *****************************************************
  //Ladder Animations

  //If not on ladder, reset
  if (!(pm->watertype & CONTENTS_LADDER)) {
    if (ps->pm_flags & ANIM_ONLADDER) {
      ps->pm_flags &= ~(ANIM_ONLADDER);
    }

    if ((!(ps->stats[EMOTES] & EMOTE_UPPER) || !(ps->stats[EMOTES] & EMOTE_LOWER))
        && ps->stats[EMOTES] & EMOTE_CLAMP_BODY) {
      ps->stats[EMOTES] &= ~EMOTE_CLAMP_BODY;
    }
  }

  //If on ladder, but not touching the ground
  if ((pm->watertype & CONTENTS_LADDER) && (ps->groundEntityNum == ENTITYNUM_NONE)) {
    if (!(ps->pm_flags & ANIM_ONLADDER)) {
      ps->pm_flags |= ANIM_ONLADDER;
      ps->stats[EMOTES] |= EMOTE_CLAMP_BODY;
    }
  }

  //Transition anim to get off ladder
  if ((pm->watertype & CONTENTS_LADDER) && (ps->groundEntityNum != ENTITYNUM_NONE)
      && (ps->pm_flags & ANIM_ONLADDER)) {//We JUST hit a ladder on the ground
    PM_ContinueLegsAnim(BOTH_OFFLADDER_BOT1, qtrue);

    if (ps->weaponstate == WEAPON_READY) {
      PM_ContinueTorsoAnim(BOTH_OFFLADDER_BOT1, qtrue);
    }
    ps->stats[EMOTES] &= ~EMOTE_CLAMP_BODY;

    return;
  }

  //Transition anim to get on ladder
  if ((pm->watertype & CONTENTS_LADDER) && (ps->groundEntityNum != ENTITYNUM_NONE)
      && !(ps->pm_flags & ANIM_ONLADDER)) {//We JUST hit a ladder on the ground
    PM_ContinueLegsAnim(BOTH_ONLADDER_BOT1, qtrue);

    if (ps->weaponstate == WEAPON_READY) {
      PM_ContinueTorsoAnim(BOTH_ONLADDER_BOT1, qtrue);
    }

    return;
  }

  if (ps->groundEntityNum == ENTITYNUM_NONE) {
    if (pm->watertype & CONTENTS_LADDER) {//FIXME: check for watertype, save waterlevel for whether to play
      //the get off ladder transition anim

      if (ps->velocity[2]) {//going up or down it
        int anim;
        if (ps->velocity[2] > 0) {
          anim = BOTH_LADDER_UP1;
        } else {
          anim = BOTH_LADDER_DWN1;
        }
        PM_ContinueLegsAnim(anim, qtrue);

        if (ps->weaponstate == WEAPON_READY) {
          PM_ContinueTorsoAnim(anim, qtrue);
        }
        if (fabs(ps->velocity[2]) > 5) {
          bobmove = 0.005 * fabs(ps->velocity[2]);    // climbing bobs slow
          if (bobmove > 0.3)
            bobmove = 0.3F;
        }
      } else {
        PM_ContinueLegsAnim(BOTH_LADDER_IDLE, qtrue);

        if (ps->weaponstate == WEAPON_READY) {
          PM_ContinueTorsoAnim(BOTH_LADDER_IDLE, qtrue);
        }
      }
      return;
    }//******************************************************************
    else {
      // airborne leaves position in cycle intact, but doesn't advance
      if (pm->waterlevel > 2) { //TiM: swimming is more hardcore now //1
        if (ps->weaponstate == WEAPON_READY)
          PM_ContinueTorsoAnim(PM_GetAnim(ANIM_SWIM,
                                          ps->weapon,
                                          (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                          qtrue), qtrue);
        PM_ContinueLegsAnim(PM_GetAnim(ANIM_SWIM,
                                       ps->weapon,
                                       (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                       qfalse),
                            qtrue);
      }

      return;
    }
  }


  // if not trying to move
  if ((!ps->speed || pm->xyspeed < 1.0f || !(pm->cmd.forwardmove || pm->cmd.rightmove))
      && pm->waterlevel < 3
      && !ps->powerups[PW_FLIGHT] && !((ps->powerups[PW_EVOSUIT]) && (ps->gravity == 0))
      ) {
    if (pm->xyspeed > 1.0f && !(ps->pm_flags & PMF_DUCKED)
        && !(ps->stats[EMOTES] & EMOTE_LOWER)) { //TiM: When you want to duck, you will duck. no delays
      if (!(pm->cmd.buttons & BUTTON_WALKING) && !(ps->pm_flags & PMF_DUCKED)) {
        if (ps->weaponstate == WEAPON_READY && !PM_HoldingSpillableWeapon()) {
          PM_ContinueTorsoAnim(BOTH_RUN1STOP, qtrue); //BOTH_RUN1STOP
        }
        PM_ContinueLegsAnim(BOTH_RUN1STOP, qtrue);
      }

      return;
    } else { // <5
      ps->bobCycle = 0;    // start at beginning of cycle again
      if (ps->pm_flags & PMF_DUCKED && !(ps->stats[EMOTES] & EMOTE_LOWER)) {

        if (ps->weaponstate == WEAPON_READY) {
          PM_ContinueTorsoAnim(PM_GetAnim(ANIM_CROUCH,
                                          ps->weapon,
                                          (qboolean) (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH),
                                          qtrue), qfalse);
        }
        PM_ContinueLegsAnim(PM_GetAnim(ANIM_CROUCH,
                                       ps->weapon,
                                       (qboolean) (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH),
                                       qfalse), qtrue);

      } else {

        if (ps->weaponstate == WEAPON_READY)
          PM_ContinueTorsoAnim(PM_GetAnim(ANIM_IDLE,
                                          ps->weapon,
                                          (qboolean) (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH),
                                          qtrue), qfalse);
        PM_ContinueLegsAnim(PM_GetAnim(ANIM_IDLE,
                                       ps->weapon,
                                       (qboolean) (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH),
                                       qfalse), qfalse);

      }
    }
    return;
  }

  footstep = qfalse;

  //TiM : Kill this when swimming as it screws up animations
  //Also... kill when speed is 0.. running on the spot is silly lol
  //Also, disable when flying.  It looks ludricrous if we run upside down lol
  if (pm->waterlevel == 3 || ps->speed == 0 || ps->powerups[PW_FLIGHT] > 0 || pm->xyspeed < 1.0f
      || ((ps->powerups[PW_EVOSUIT]) && (ps->gravity == 0))) {
    return;
  }

  if (ps->pm_flags & PMF_DUCKED) {
    bobmove = 0.5;    // ducked characters bob much faster
    //HACK coz this damn thing screws up crouch firing anims otherwise T_T
    if (ps->weaponstate == WEAPON_READY) {
      PM_ContinueTorsoAnim(PM_GetAnim(ANIM_CROUCHWALK,
                                      ps->weapon,
                                      (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                      qtrue), qfalse);
    }

    PM_ContinueLegsAnim(PM_GetAnim(ANIM_CROUCHWALK,
                                   ps->weapon,
                                   (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                   qfalse), qtrue);
    // ducked characters never play footsteps
  } else if (ps->pm_flags & PMF_BACKWARDS_RUN) {
    if (!(pm->cmd.buttons & BUTTON_WALKING)) {
      bobmove = 0.4;    // faster speeds bob faster
      footstep = qtrue;

      if (ps->weaponstate == WEAPON_READY)
        PM_ContinueTorsoAnim(PM_GetAnim(ANIM_RUNB,
                                        ps->weapon,
                                        (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                        qtrue),
                             qfalse);
      PM_ContinueLegsAnim(PM_GetAnim(ANIM_RUNB,
                                     ps->weapon,
                                     (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                     qfalse),
                          qtrue); //LEGS_BACK
    } else {
      bobmove = 0.3;

      if (ps->weaponstate == WEAPON_READY)
        PM_ContinueTorsoAnim(PM_GetAnim(ANIM_WALKB,
                                        ps->weapon,
                                        (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH ? qtrue : qfalse),
                                        qtrue),
                             qfalse);
      PM_ContinueLegsAnim(PM_GetAnim(ANIM_WALKB,
                                     ps->weapon,
                                     (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                     qfalse),
                          qtrue); //LEGS_BACK
    }

  } else {

    if (!(pm->cmd.buttons & BUTTON_WALKING)) {
      bobmove = 0.4;    // faster speeds bob faster
      footstep = qtrue;

      if (ps->weaponstate == WEAPON_READY)
        PM_ContinueTorsoAnim(PM_GetAnim(ANIM_RUN,
                                        ps->weapon,
                                        (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                        qtrue),
                             qfalse);
      PM_ContinueLegsAnim(PM_GetAnim(ANIM_RUN,
                                     ps->weapon,
                                     (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                     qfalse),
                          qtrue); //LEGS_RUN

    } else {
      bobmove = 0.3;    // walking bobs slow //0.3
      if (ps->weaponstate == WEAPON_READY)
        PM_ContinueTorsoAnim(PM_GetAnim(ANIM_WALK,
                                        ps->weapon,
                                        (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                        qtrue),
                             qfalse);

      PM_ContinueLegsAnim(PM_GetAnim(ANIM_WALK,
                                     ps->weapon,
                                     (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                     qfalse),
                          qtrue); //LEGS_WALK
    }
  }

  // check for footstep / splash sounds
  old = ps->bobCycle;
  ps->bobCycle = (int) (old + bobmove * pml.msec) & 255;

  // if we just crossed a cycle boundary, play an apropriate footstep event
  if (((old + 64) ^ (ps->bobCycle + 64)) & 128 && !ps->stats[PW_INVIS]) {
    if (pm->watertype & CONTENTS_LADDER) {// on ladder
      if (!pm->noFootsteps) {
        PM_AddEvent(EV_FOOTSTEP_METAL);
      }
    } else if (pm->waterlevel == 0) {
      // on ground will only play sounds if running
      if (footstep && !pm->noFootsteps) {
        PM_AddEvent(PM_FootstepForSurface());
      }
    } else if (pm->waterlevel == 1) {
      // splashing
      PM_AddEvent(EV_FOOTSPLASH);
    } else if (pm->waterlevel == 2) {
      // wading / swimming at surface
      PM_AddEvent(EV_SWIM);
    }
  }
}

/*
==============
PM_WaterEvents
==============
*/
/**
*	Generate sound events for entering and leaving water
*/
static void PM_WaterEvents(void) {        // FIXME?
  if (pm->watertype & CONTENTS_LADDER || pm->ps->stats[PW_INVIS])    //fake water for ladder
  {
    return;
  }
  //
  // if just entered a water volume, play a sound
  //
  if (!pml.previous_waterlevel && pm->waterlevel) {
    PM_AddEvent(EV_WATER_TOUCH);
  }

  //
  // if just completely exited a water volume, play a sound
  //
  if (pml.previous_waterlevel && !pm->waterlevel) {
    PM_AddEvent(EV_WATER_LEAVE);
  }

  //
  // check for head just going under water
  //
  if (pml.previous_waterlevel != 3 && pm->waterlevel == 3) {
    PM_AddEvent(EV_WATER_UNDER);
  }

  //
  // check for head just coming out of water
  //
  if (pml.previous_waterlevel == 3 && pm->waterlevel != 3) {
    PM_AddEvent(EV_WATER_CLEAR);
  }
}


/*
===============
PM_BeginWeaponChange
===============
*/
/**
*	Begins weapon change
*/
static void PM_BeginWeaponChange(int weapon) {
  playerState_t *ps = pm->ps;

  if (weapon <= WP_0 || weapon >= WP_NUM_WEAPONS) {
    return;
  }

  if (!(ps->stats[STAT_WEAPONS] & (1 << weapon))) {
    return;
  }

  if (ps->weaponstate == WEAPON_DROPPING) {
    return;
  }

  PM_AddEvent(EV_CHANGE_WEAPON);
  ps->weaponstate = WEAPON_DROPPING;
  ps->weaponTime += 200;
  PM_ForceTorsoAnim(TORSO_DROPWEAP1, qfalse);
}


/*
===============
PM_FinishWeaponChange
===============
*/
/**
*	Finishs weapon change
*/
static void PM_FinishWeaponChange(void) {
  int weapon;
  playerState_t *ps = pm->ps;

  weapon = pm->cmd.weapon;
  if (weapon < WP_0 || weapon >= WP_NUM_WEAPONS) {
    weapon = WP_0;
  }

  if (!(ps->stats[STAT_WEAPONS] & (1 << weapon))) {
    weapon = WP_0;
  }

  ps->weapon = weapon;
  ps->weaponstate = WEAPON_RAISING;
  ps->weaponTime += 250;
  PM_ForceTorsoAnim(TORSO_RAISEWEAP1, qfalse);
}


/*
==============
PM_TorsoAnimation

==============
*/
/**
*	Once handled torso animation
*/
static void PM_TorsoAnimation(void) {
  return;
}

#define PHASER_AMMO_PER_SHOT            1
#define PHASER_ALT_AMMO_PER_SHOT        2

//! alt ammo usage
int altAmmoUsage[WP_NUM_WEAPONS] =
    {
        0,                //!<WP_0,
        1,                //!<WP_5,
        8,                //!<WP_6,
        3,                //!<WP_1,
        5,                //!<WP_4,
        1,                //!<WP_10,
        1,                //!<WP_8,
        2,                //!<WP_7,
        2,                //!<WP_9,
        5                //!<WP_13,
    };

/*
==============
PM_Weapon
==============
*/
/**
*	Generates weapon events and modifes the weapon counter
*/
static void PM_Weapon(void) {
  int addTime;
  qboolean altfired = qfalse;
  playerState_t *ps = pm->ps;

  // don't allow attack until all buttons are up
  if (ps->pm_flags & PMF_RESPAWNED) {
    return;
  }

  // ignore if spectator
  if (ps->persistant[PERS_TEAM] == TEAM_SPECTATOR) {
    return;
  }

  ps->rechargeTime -= pml.msec;

  // check for dead player
  if (ps->stats[STAT_HEALTH] <= 0) {
    ps->weapon = WP_0;
    return;
  }

  // check for item using
  if (pm->cmd.buttons & BUTTON_USE_HOLDABLE) {
    if (!(ps->pm_flags
        & PMF_USE_ITEM_HELD)) {
      int tag = bg_itemlist[ps->stats[STAT_HOLDABLE_ITEM]].giTag;
      ps->pm_flags |= PMF_USE_ITEM_HELD;
      PM_AddEvent(EV_USE_ITEM0 + tag);
      // if we're placing the detpack, don't remove it from our "inventory"
      if ((HI_DETPACK == tag) /* || (HI_TRANSPORTER == tag)) */ &&
          (IT_HOLDABLE == bg_itemlist[ps->stats[STAT_HOLDABLE_ITEM]].giType)) {
        // are we placing it?
        if (2 == ps->stats[STAT_USEABLE_PLACED]) {
          // we've placed the first stage of a 2-stage transporter
        } else if (ps->stats[STAT_USEABLE_PLACED]) {
          // we already placed it, we're activating it.
          ps->stats[STAT_HOLDABLE_ITEM] = 0;
        }
      } else {
        ps->stats[STAT_HOLDABLE_ITEM] = 0;
      }
      return;
    }
  } else {
    ps->pm_flags &= ~PMF_USE_ITEM_HELD;
  }


  // make weapon function
  if (ps->weaponTime > 0) {
    ps->weaponTime -= pml.msec;
  }

  // check for weapon change
  // can't change if weapon is firing, but can change
  // again if lowering or raising
  if (ps->weaponTime <= 0 || ps->weaponstate != WEAPON_FIRING) {
    if (ps->weapon != pm->cmd.weapon) {
      PM_BeginWeaponChange(pm->cmd.weapon);
    }
  }

  if (ps->weaponTime > 0) {
    return;
  }

  // change weapon if time
  if (ps->weaponstate == WEAPON_DROPPING) {
    PM_FinishWeaponChange();
    return;
  }

  if (ps->weaponstate == WEAPON_RAISING) {
    ps->weaponstate = WEAPON_READY;

    PM_StartTorsoAnim(PM_GetAnim(ANIM_IDLE,
                                 ps->weapon,
                                 (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                 qtrue),
                      qfalse);

    return;
  }

  // check for fire
  if (!(pm->cmd.buttons & (BUTTON_ATTACK | BUTTON_ALT_ATTACK))) {
    ps->weaponTime = 0;
    ps->weaponstate = WEAPON_READY;

    return;
  }

  // take an ammo away if not infinite
  if (ps->ammo[ps->weapon] != -1) {
    if (pm->cmd.buttons & BUTTON_ALT_ATTACK) {
      altfired = qtrue;
    } else {
      // check for out of ammo
      if (!ps->ammo[ps->weapon]) {
        if (ps->weapon == WP_5) // phaser out of ammo is special case
        {
          ps->ammo[ps->weapon] = 0;
        }
      }
    }
  }

  if (ps->weapon != WP_14 && ps->weapon != WP_4 && ps->weapon != WP_1) {
    //Little hack.  I like the idle poses for these when it crouches :)
    if (((ps->weapon == WP_5)
        || (ps->weapon == WP_6)
        || (ps->weapon == WP_10)
        || (ps->weapon == WP_7))
        && (ps->pm_flags & PMF_DUCKED)) {
      PM_ForceTorsoAnim(PM_GetAnim(ANIM_CROUCH,
                                   ps->weapon,
                                   (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                   qtrue),
                        qfalse);
    } else {
      PM_ContinueTorsoAnim(PM_GetAnim(ANIM_ATTACK,
                                      ps->weapon,
                                      (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) ? qtrue : qfalse,
                                      qtrue),
                           qfalse);
    }

    //Put in this scope, so holding down the trigger on these 'no-anim' weapons won't lock
    //other animations.
    //Bots were locked in the jumping anim >.<

    ps->weaponstate = WEAPON_FIRING;
  } else {
    ps->weaponstate = WEAPON_READY;

  }

  // fire weapon
  if (pm->cmd.buttons & BUTTON_ALT_ATTACK) {
    if (altfired) // it's either a legally altfirable non-phaser, or it's a phaser with ammo left
    {
      PM_AddEvent(EV_ALT_FIRE); // fixme, because I'm deducting ammo earlier, the last alt-fire shot turns into a main fire
    } else {
      PM_AddEvent(EV_FIRE_EMPTY_PHASER);
    }
    switch (ps->weapon) {
      default:
      case WP_5: addTime = 100;
        //If the phaser has been fired, delay the next recharge time
        ps->rechargeTime = PHASER_RECHARGE_TIME;
        break;
      case WP_13: addTime = 0; //500
        break;
      case WP_8: addTime = 600;//RPG-X: RedTechie use to be 700
        break;
      case WP_10: addTime = DISRUPTOR_DELAY;
        break;
      case WP_4: addTime = 0; //700
        break;
      case WP_9: addTime = ALT_PHOTON_DELAY;
        break;
      case WP_1: addTime = 460; //700
        break;
      case WP_6: addTime = 100;
        break;
      case WP_7: addTime = 500; //RPG-X: RedTechie - Use to be 1200
        break;
      case WP_12:
        //RPG-X: RedTechie - Admins get faster alt fire for steam effects
        if (pm->admin) {
          addTime = 80;
        } else {
          addTime = 1000;
        }
        break;
      case WP_14: addTime = 2000;
        break;
      case WP_11: addTime = 0; //1000
        break;
      case WP_2:
        if (pm->admin) {
          addTime = ALT_TRICORDER_DELAY;
        } else {
          addTime = 0;
        }
        break;
      case WP_3: addTime = 0; //500
        break;
      case WP_15: addTime = 0; //1000
        break;
    }
  } else {
    if (ps->ammo[ps->weapon]) {
      PM_AddEvent(EV_FIRE_WEAPON);
    } else {
      PM_AddEvent(EV_FIRE_EMPTY_PHASER);
    }
    switch (ps->weapon) {
      default:
      case WP_5: addTime = 100;
        //If the phaser has been fired, delay the next recharge time
        ps->rechargeTime = PHASER_RECHARGE_TIME;
        break;
      case WP_13: addTime = 1000; //1000
        break;
      case WP_8: addTime = 460;//RPG-X: RedTechie use to be 700
        break;
      case WP_1: addTime = 460;
        break;
      case WP_10: addTime = 100;
        break;
      case WP_4: addTime = 0; //100
        break;
      case WP_9: addTime = PHOTON_DELAY;
        break;
      case WP_6: addTime = RIFLE_DELAY;
        break;
      case WP_7: addTime = TR116_DELAY; //RPG-X: RedTechie - Use to be 1200
        break;
      case WP_12: addTime = 1000;
        break;
      case WP_14: addTime = 2000; //1000
        break;
      case WP_11: addTime = 0; //1000
        break;
      case WP_2: addTime = ALT_TRICORDER_DELAY; //1000
        break;
      case WP_3: addTime = 0; //500
        break;
      case WP_15: addTime = 0; //1000
        break;
    }
  }

  if (ps->powerups[PW_HASTE]) {
    addTime /= 1.3;
  }

  ps->weaponTime += addTime;
}

/*
================
PM_Animate
================
*/
static void PM_Animate(void) {
  playerState_t *ps = pm->ps;

  if (pm->cmd.buttons & BUTTON_GESTURE) {
    if (ps->pm_type < PM_DEAD) {
      if (!(ps->eFlags & EF_TALKING)) {
        ps->eFlags |= EF_TALKING;
      }
    }
  } else {
    if ((ps->eFlags & EF_TALKING)) {
      ps->eFlags &= ~EF_TALKING;
    }
  }
}

/*
================
PM_DropTimers
================
*/
static void PM_DropTimers(void) {
  int newFlags;
  playerState_t *ps = pm->ps;

  // drop misc timing counter
  if (ps->pm_time) {
    if (pml.msec >= ps->pm_time) {
      ps->pm_flags &= ~PMF_ALL_TIMES;
      ps->pm_time = 0;
    } else {
      ps->pm_time -= pml.msec;
    }
  }

  //Count down the legs anim timer
  if (ps->stats[LEGSTIMER] > 0) { //legsTimer
    ps->stats[LEGSTIMER] -= pml.msec;
    if (ps->stats[LEGSTIMER] < 0) {
      ps->stats[LEGSTIMER] = 0;
    }
  }

  //if legs anim timer hit 0
  if (ps->stats[LEGSTIMER] == 0) {
    if ((ps->stats[EMOTES] & EMOTE_LOWER) &&
        !(ps->stats[EMOTES] & EMOTE_LOOP_LOWER)) {
      ps->stats[EMOTES] &= ~EMOTE_MASK_LOWER;

      if (ps->legsAnim > 0) {
        PM_ForceLegsAnim(bg_emoteList[ps->legsAnim].enumName);

        //TiM - Remove any data about torsos here.  it's not necessary and it invalidates the check below
        newFlags = (bg_emoteList[ps->legsAnim].animFlags | bg_emoteList[ps->legsAnim].bodyFlags);
        newFlags &= ~EMOTE_MASK_UPPER;
        ps->stats[EMOTES] |= newFlags;
        ps->legsTimer = ps->legsAnim;
      } else {
        ps->legsTimer = 0;
        ps->legsAnim = 0;
      }
    }
  }

  if (ps->stats[TORSOTIMER] > 0) { //torsoTimer
    ps->stats[TORSOTIMER] -= pml.msec;
    if (ps->stats[TORSOTIMER] < 0) {
      ps->stats[TORSOTIMER] = 0;
    }
  }

  if (ps->stats[TORSOTIMER] == 0) {
    if ((ps->stats[EMOTES] & EMOTE_UPPER) &&
        !(ps->stats[EMOTES] & EMOTE_LOOP_UPPER)) {
      ps->stats[EMOTES] &= ~EMOTE_MASK_UPPER;

      if (ps->torsoAnim > 0) {
        PM_ForceTorsoAnim(bg_emoteList[ps->torsoAnim].enumName, qtrue);

        //TiM - Remove any data about legs here.  it's not necessary and it invalidates any subsequent checks
        newFlags = (bg_emoteList[ps->torsoAnim].animFlags | bg_emoteList[ps->torsoAnim].bodyFlags);
        newFlags &= ~EMOTE_MASK_LOWER;
        ps->stats[EMOTES] |= newFlags;
      } else {
        ps->torsoTimer = 0;
        ps->torsoAnim = 0;
      }
    }
  }
}

/*
================
PM_UpdateViewAngles
================
*/
/**
*	This can be used as another entry point when only the viewangles
*	are being updated isntead of a full move
*/
void PM_UpdateViewAngles(playerState_t *ps, const usercmd_t *cmd) {
  short temp;
  int i;

  if (ps->pm_type == PM_INTERMISSION) {
    return;        // no view changes at all
  }

  if (ps->pm_type == PM_CCAM) {
    return;
  }

  if (ps->pm_type != PM_SPECTATOR && ps->stats[STAT_HEALTH]
      <= 1) { //RPG-X: RedTechie - This use to be 0 but in rpg-x with or without medics revive 1 health means you die!
    return;        // no view changes at all
  }


  //TiM - Bookmark
  //With a flag here, a change to the client side player rotation code there,
  //we could actually make it EVA suit users could rotate fully in all directions
  //when in space. :)

  // circularly clamp the angles with deltas
  for (i = 0; i < 3; i++) {
    temp = cmd->angles[i] + ps->delta_angles[i];
    if (i == PITCH && !pm->ps->powerups[PW_FLIGHT] && !((pm->ps->powerups[PW_EVOSUIT]) && (pm->ps->gravity == 0))) {
      // don't let the player look up or down more than 90 degrees
      if (temp > 16000) {
        ps->delta_angles[i] = 16000 - cmd->angles[i];
        temp = 16000;
      } else if (temp < -16000) {
        ps->delta_angles[i] = -16000 - cmd->angles[i];
        temp = -16000;
      }
    }

    ps->viewangles[i] = SHORT2ANGLE(temp);
  }
}

/*
================
PmoveSingle

================
*/
void PmoveSingle(pmove_t *pmove) {
  playerState_t *ps = pmove->ps;

  pm = pmove;


  // this counter lets us debug movement problems with a journal
  // by setting a conditional breakpoint fot the previous frame
  c_pmove++;

  // clear results
  pm->numtouch = 0;
  pm->watertype = 0;
  pm->waterlevel = 0;

  if (ps->stats[STAT_HEALTH] <= 0) {
    pm->tracemask &= ~CONTENTS_BODY;    // corpses can fly through bodies
  }

  // make sure walking button is clear if they are running, to avoid
  // proxy no-footsteps cheats
  if (abs(pm->cmd.forwardmove) > 64 || abs(pm->cmd.rightmove) > 64) {
    pm->cmd.buttons &= ~BUTTON_WALKING;
  }

  // set the talk balloon flag
  if (pm->cmd.buttons & BUTTON_TALK) {
    ps->eFlags |= EF_TALK;
  } else {
    ps->eFlags &= ~EF_TALK;
  }

  // set the firing flag for continuous beam weapons
  if (!(ps->pm_flags & PMF_RESPAWNED) &&
      ps->pm_type != PM_INTERMISSION &&
      ps->pm_type != PM_CCAM &&
      ((pm->cmd.buttons & BUTTON_ATTACK) || (pm->cmd.buttons & BUTTON_ALT_ATTACK)) &&
      (ps->ammo[ps->weapon] || ps->weapon == WP_5)) {
    if (((ps->weapon == WP_5) && (!ps->ammo[ps->weapon])) || (!(pm->cmd.buttons & BUTTON_ALT_ATTACK))) {
      ps->eFlags &= ~EF_ALT_FIRING;
    } else {
      ps->eFlags |= EF_ALT_FIRING;
    }

    // This flag should always get set, even when alt-firing
    ps->eFlags |= EF_FIRING;
  } else {
    ps->eFlags &= ~EF_FIRING;
    ps->eFlags &= ~EF_ALT_FIRING;
  }

  // clear the respawned flag if attack and use are cleared
  if (ps->stats[STAT_HEALTH] > 0 &&
      !(pm->cmd.buttons & (BUTTON_ATTACK | BUTTON_USE_HOLDABLE))) {
    ps->pm_flags &= ~PMF_RESPAWNED;
  }

  // if talk button is down, dissallow all other input
  // this is to prevent any possible intercept proxy from
  // adding fake talk balloons
  if (pmove->cmd.buttons & BUTTON_TALK) {
    pmove->cmd.buttons = 0;
    pmove->cmd.forwardmove = 0;
    pmove->cmd.rightmove = 0;
    if (pmove->cmd.upmove) {
      if (pmove->cmd.upmove > 0) {
        pmove->cmd.upmove = 1;
      } else {
        pmove->cmd.upmove = -1;//allow a tiny bit to keep the duck anim
      }
    }
  }

  //ph34r teh cheezy hack
  //Disable the ability to jump when getting up animating.
  //This is for the purporse of the medics revive animation
  if ((ps->stats[LEGSANIM] & ~ANIM_TOGGLEBIT) == BOTH_GET_UP1) {
    pm->cmd.upmove = 0;
  }

  //If the legs are playing a non infinite loop, disable the movement keys.
  //Else the player slides on their feet :S
  if ((ps->stats[EMOTES] & EMOTE_LOWER && (ps->stats[EMOTES] & EMOTE_CLAMP_BODY))
      || (ps->stats[EMOTES] & EMOTE_CLAMP_ALL)) { //EMOTE_LOWER
    pmove->cmd.forwardmove = 0;
    pmove->cmd.rightmove = 0;
  }

  if (ps->stats[EMOTES] & (EMOTE_LOWER | EMOTE_UPPER)) {
    if (pmove->cmd.buttons & MASK_KILL_EMOTES || pmove->cmd.upmove != 0) {
      ps->stats[EMOTES] &= ~EMOTE_MASK_BOTH;

      ps->legsTimer = 0;
      ps->stats[LEGSTIMER] = 0;
    }
  }

  //TiM: Injured system
  //Disable the ability to toggle walking,
  //and slow player down. :)
  if (ps->stats[STAT_HEALTH] <= INJURED_MODE_HEALTH) {
    pm->cmd.buttons &= ~BUTTON_WALKING;
  }

  // clear all pmove local vars
  memset(&pml, 0, sizeof(pml));

  // determine the time
  pml.msec = pmove->cmd.serverTime - ps->commandTime;
  if (pml.msec < 1) {
    pml.msec = 1;
  } else if (pml.msec > 200) {
    pml.msec = 200;
  }
  ps->commandTime = pmove->cmd.serverTime;

  // save old org in case we get stuck
  VectorCopy (ps->origin, pml.previous_origin);

  // save old velocity for crashlanding
  VectorCopy (ps->velocity, pml.previous_velocity);

  pml.frametime = pml.msec * 0.001;

  if (ps->pm_type == PM_FREEZE /*|| ps->introTime > pm->cmd.serverTime*/ ) {
    PM_FreezeMove();
    return;        // no movement at all
  }

  // update the viewangles
  PM_UpdateViewAngles(ps, &pm->cmd);

  AngleVectors(ps->viewangles, pml.forward, pml.right, pml.up);

  if (pm->cmd.upmove < 10) {
    // not holding jump
    ps->pm_flags &= ~PMF_JUMP_HELD;
  }

  // decide if backpedaling animations should be used
  if (pm->cmd.forwardmove < 0) {
    ps->pm_flags |= PMF_BACKWARDS_RUN;
  } else if (pm->cmd.forwardmove > 0 || (pm->cmd.forwardmove == 0 && pm->cmd.rightmove)) {
    ps->pm_flags &= ~PMF_BACKWARDS_RUN;
  }

  if (ps->pm_type >= PM_DEAD) {
    pm->cmd.forwardmove = 0;
    pm->cmd.rightmove = 0;
    pm->cmd.upmove = 0;
  }

  if (ps->pm_type == PM_SPECTATOR) {
    PM_CheckDuck();
    PM_FlyMove();
    PM_DropTimers();
    return;
  }

  if (ps->pm_type == PM_SPECTATOR) {
    PM_CheckDuck();
    PM_FlyMove();
    PM_DropTimers();
    return;
  }

  if (ps->pm_type == PM_NOCLIP) {
    PM_NoclipMove();
    PM_DropTimers();
    return;
  }

  if (ps->pm_type == PM_CCAM) {
    return;
  }

  if (ps->pm_type == PM_INTERMISSION) {
    return;        // no movement at all
  }

  // set watertype, and waterlevel
  PM_SetWaterLevel();
  if (!(pm->watertype
      & CONTENTS_LADDER)) {//Don't want to remember this for ladders, is only for waterlevel change events (sounds)
    pml.previous_waterlevel = pmove->waterlevel;
  }

  // set mins, maxs, and viewheight
  PM_CheckDuck();

  // set groundentity
  PM_GroundTrace();

  if (ps->pm_type == PM_DEAD) {
    PM_DeadMove();
  }

  if (ps->powerups[PW_FLIGHT]) {
    // flight powerup doesn't allow jump and has different friction
    PM_FlyMove();
  } else if (((ps->powerups[PW_EVOSUIT]) && (ps->gravity == 0))) {
    //RPG-X | Phenix | 8/8/2004
    //The player is in 0 G and is wearing an evo suit...
    PM_FlyMove();
  } else if (ps->pm_flags & PMF_TIME_WATERJUMP) {
    PM_WaterJumpMove();
  } else if (pm->waterlevel > 1) {
    // swimming
    PM_WaterMove();
  } else if (pml.walking) {
    // walking on ground
    PM_WalkMove();
  } else {
    // airborne
    PM_AirMove();
  }

  PM_Animate();

  // set groundentity, watertype, and waterlevel
  PM_GroundTrace();
  PM_SetWaterLevel();

  // weapons
  PM_Weapon();

  PM_Use();

  // torso animation
  PM_TorsoAnimation();

  // footstep events / legs animations
  PM_Footsteps();

  // entering / leaving water splashes
  PM_WaterEvents();

  //PM_DoEmote();

  PM_DropTimers();

  // snap some parts of playerstate to save network bandwidth
  SnapVector(ps->velocity);
}


/*
================
Pmove
================
*/
/**
*	Can be called by either the server or the client
*/
void Pmove(pmove_t *pmove) {
  int finalTime;

  finalTime = pmove->cmd.serverTime;

  if (finalTime < pmove->ps->commandTime) {
    return;    // should not happen
  }

  if (finalTime > pmove->ps->commandTime + 1000) {
    pmove->ps->commandTime = finalTime - 1000;
  }

  // chop the move up if it is too long, to prevent framerate
  // dependent behavior
  while (pmove->ps->commandTime != finalTime) {
    int msec;

    msec = finalTime - pmove->ps->commandTime;

    if (msec > 66) {
      msec = 66;
    }
    pmove->cmd.serverTime = pmove->ps->commandTime + msec;
    PmoveSingle(pmove);
  }

}
