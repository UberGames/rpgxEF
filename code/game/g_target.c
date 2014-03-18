// Copyright (C) 1999-2000 Id Software, Inc.
//
#include "g_local.h"
#include "bg_list.h"
#include "bg_misc.h"
#include "g_spawn.h"
#include "g_client.h"
#include "g_lua.h"

//#include <windows.h> //TiM : WTF?

//==========================================================

/*QUAKED target_give (1 0 0) (-8 -8 -8) (8 8 8)
-----DESCRIPTION-----
Gives all the weapons specified here in the list.

-----SPAWNFLAGS-----
none

-----KEYS-----
"items" - separated by ' | ', specify the items
EG "WP_5 | WP_14" etc
(Don't forget the spaces!)
*/
static void Use_Target_Give( /*@shared@*/ gentity_t *ent, /*@shared@*/ /*@unused@*/ /*@null@*/  gentity_t *other,  /*@null@*/ /*@shared@*/ gentity_t *activator ) {
	unsigned		i;
	playerState_t*	ps;

	if ( activator == NULL || activator->client == NULL ) {
		return;
	}

	ps = &activator->client->ps;

	for ( i=0; i < MAX_WEAPONS; i++ ) {
		if ( ((unsigned int)(ent->s.time) & (1 << i)) != 0 ) {
			ps->stats[STAT_WEAPONS] ^= ( 1 << i );

			if ( (ps->stats[STAT_WEAPONS] & ( 1 << i )) != 0 ) {
				ps->ammo[i] = 1;
			} else {
				ps->ammo[i] = 0;
			}
			continue;
		}
	}
}
//FIXME: Make the text parsed on load time. saves on resources!!
void SP_target_give( gentity_t *ent ) 
{
	/*@shared@*/ char*		items;
	char*		textPtr;
	char*		token;
	unsigned	weapon;

	ent->type = ENT_TARGET_GIVE;

	G_SpawnString( "items", "", &items );

	if(strcmp(items, "") == 0 && ent->target != NULL) { // spawnTEnt
		items = G_NewString(ent->target);
	}

	if(items == NULL) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] SP_target_give: items and ent->terget are NULL\n"););
		G_FreeEntity(ent);
		return;
	}

	textPtr = items;

	COM_BeginParseSession();

	while (qtrue) {
		token = COM_Parse( &textPtr );
		
		if ( token == NULL || token[0] == 0 ) {
			break;
		}

		if ( Q_stricmpn( token, "|", 1 ) == 0 ) {
			continue;
		}

		if( Q_stricmpn( token, "WP_", 3 ) == 0) {
			int i = GetIDForString( WeaponTable, token );

			if ( i >= 0 ) {
				weapon = (unsigned)i;
				ent->s.time |= (1<<weapon);
			}
		}
	}

	//TiM - remove items per server discretion
	if ( rpg_mapGiveFlags.integer > 0 ) {
		ent->s.time &= rpg_mapGiveFlags.integer;
	}

	ent->use = Use_Target_Give;

	// don't need to send this to clients
	ent->r.svFlags &= SVF_NOCLIENT;
	trap_LinkEntity(ent);
}


//==========================================================

/*QUAKED target_remove_powerups (1 0 0) (-8 -8 -8) (8 8 8)
-----DESCRIPTION-----
takes away all the activators powerups.
Used to drop flight powerups into death puts.

-----SPAWNFLAGS-----
none

-----KEYS-----
none
*/

//hmmm... maybe remove this, not sure.
static void Use_target_remove_powerups( /*@shared@*/ /*@unused@*/ gentity_t *ent, /*@shared@*/ /*@unused@*/  /*@null@*/  gentity_t *other, /*@shared@*/  /*@null@*/  gentity_t *activator ) {
	if ( activator == NULL || activator->client == NULL ) {
		return;
	}

	memset( activator->client->ps.powerups, 0, sizeof( activator->client->ps.powerups ) );
}

void SP_target_remove_powerups( gentity_t *ent ) {
	ent->type = ENT_TARGET_REMOVE_POWERUPS;
	ent->use = Use_target_remove_powerups;
}


//==========================================================

/*QUAKED target_delay (1 0 0) (-8 -8 -8) (8 8 8) SELF
-----DESCRIPTION-----
When used fires it'd target after a delay of 'wait' seconds

-----SPAWNFLAGS-----
1: SELF - use the entity as activator instead of it's own activator when using it's targets (use this flag for targets that are not called by their targetname (e.g. swapname))

-----KEYS-----
"wait" - seconds to pause before firing targets.
"random" - delay variance, total delay = delay +/- random seconds

"luaUse" - lua function to call at the beginning of the delay
luaThink - lua function to call at end of delay
*/
static void Think_Target_Delay( /*@shared@*/ gentity_t *ent ) {
#ifdef G_LUA
	if(ent->luaTrigger != NULL) {
		if(ent->activator != NULL) {
			LuaHook_G_EntityTrigger(ent->luaTrigger, ent->s.number, ent->activator->s.number);
		} else {
			LuaHook_G_EntityTrigger(ent->luaTrigger, ent->s.number, ENTITYNUM_WORLD);
		}
	}
#endif
	if((ent->spawnflags & 1) != 0) {
		G_UseTargets(ent, ent);
	} else {
		G_UseTargets( ent, ent->activator );
	}
}

static void Use_Target_Delay( /*@shared@*/  gentity_t *ent, /*@shared@*/ /*@unused@*/  /*@null@*/  gentity_t *other, /*@shared@*/  /*@null@*/ gentity_t *activator ) {
	ent->nextthink = (int)(level.time + ( ent->wait + ent->random * crandom() ) * 1000);
	ent->think = Think_Target_Delay;
	ent->activator = activator;
}

void SP_target_delay( gentity_t *ent ) {
	ent->type = ENT_TARGET_DELAY;

	if ( ent->wait <= 0.0f ) {
		G_SpawnFloat("delay", "0", &ent->wait);
		if(ent->wait <= 0.0f) {
			ent->wait = 1;
		}
	}
	ent->count = (int)ent->wait;
	ent->use = Use_Target_Delay;

	// don't need to send this to clients
	ent->r.svFlags &= SVF_NOCLIENT;
	trap_LinkEntity(ent);
}

//==========================================================

/*QUAKED target_print (1 0 0) (-8 -8 -8) (8 8 8) redteam blueteam private
-----DESCRIPTION-----
This will display the 'message' in the lower right corner for all reciepients.
By default every client get's the message however this can be limited via spawnflags.

-----SPAWNFLAGS-----
1: redteam - everyone on the red team gets the message
2: blueteam - everyone on the blue team gets the message
4: private - only the activator gets the message

-----KEYS-----
"message"	text to print
*/
static void Use_Target_Print (/*@shared@*/ gentity_t *ent, /*@shared@*/ /*@unused@*/ /*@null@*/ gentity_t *other, /*@shared@*/ /*@null@*/ gentity_t *activator) {
	if ( activator != NULL && activator->client != NULL && ( ent->spawnflags & 4 ) != 0 && ent->message != NULL) {
		trap_SendServerCommand( activator-g_entities, va("servermsg %s", ent->message ));
		return;
	}

	if(ent->message == NULL) {
		return;
	}

	if ( (ent->spawnflags & 3) != 0 ) {
		if ( (ent->spawnflags & 1) != 0 ) {
			G_TeamCommand( TEAM_RED, va("servermsg %s", ent->message) );
		}
		if ( (ent->spawnflags & 2) != 0 ) {
			G_TeamCommand( TEAM_BLUE, va("servermsg %s", ent->message) );
		}
		return;
	}

	trap_SendServerCommand( -1, va("servermsg %s", ent->message ));
}

void SP_target_print( gentity_t *ent ) {
	ent->type = ENT_TARGET_PRINT;
	ent->use = Use_Target_Print;
}


//==========================================================


/*QUAKED target_speaker (1 0 0) (-8 -8 -8) (8 8 8) LOOPED_ON LOOPED_OFF GLOBAL ACTIVATOR 
-----DESCRIPTION-----
A sound-file to play.
By default this will be played once locally. 
The specifics on how to play are set via spawnflags.

Looping Sounds may not be combined with GLOBAL or ACTIVATOR
Multiple identical looping sounds will just increase volume without any speed cost.
Using a looping target_speaker will toggle it's sound on or off.

Using a target_speaker designed to play it's sound once will play that sound.

-----SPAWNFLAGS-----
1: LOOPED_ON - this Speaker will loop it's sound and will be active at spawn.
2: LOOPED_OFF - this Speaker will loop it's sound and will be inactive at spawn.
4: GLOBAL - the sound will be played once globally so every client will hear it.
8: ACTIVATOR - The sound will be played once for the activator only to hear.

-----KEYS-----
"noise" - file to play
"wait" - Seconds between auto triggerings, default = 0 = don't auto trigger
"random" - wait variance, default is 0, delay would be wait +/- random
*/
static void Use_Target_Speaker (/*@shared@*/ gentity_t *ent, /*@shared@*/ /*@unused@*/ /*@null@*/ gentity_t *other, /*@shared@*/ /*@null@*/ gentity_t *activator) {
	if ((ent->spawnflags & 3) != 0) {	// looping sound toggles
		if (ent->s.loopSound != 0) {
			ent->s.loopSound = 0;	// turn it off
		} else {
			ent->s.loopSound = ent->noise_index;	// start it
		}
	} else {	// normal sound
		if ( activator != NULL && (ent->spawnflags & 8) != 0 ) {
			G_AddEvent( activator, EV_GENERAL_SOUND, ent->noise_index );
		} else if ((ent->spawnflags & 4) != 0) {
			G_AddEvent( ent, EV_GLOBAL_SOUND, ent->noise_index );
		} else {
			G_AddEvent( ent, EV_GENERAL_SOUND, ent->noise_index );
		}
	}
}

void SP_target_speaker( gentity_t *ent ) {
	char	buffer[MAX_QPATH];
	char	*s;

	ent->type = ENT_TARGET_SPEAKER;

	G_SpawnFloat( "wait", "0", &ent->wait );
	G_SpawnFloat( "random", "0", &ent->random );

	if ( !G_SpawnString( "noise", "NOSOUND", &s ) && ent->count == 0 ) { // if ent->count then it is a spawned sound, either by spawnEnt or *.spawn
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_speaker without a noise key at %s", vtos( ent->s.origin ) ););
		G_FreeEntity(ent);//let's not error out so that we can use SP maps with their funky speakers.
		return;
	}

	if(ent->count == 0) { // not by spawnTEnt\*.spawn
		// force all client reletive sounds to be "activator" speakers that
		// play on the entity that activates it
		if ( s[0] == '*' ) {
			ent->spawnflags |= 8;
		}

		memset(buffer, 0, sizeof(buffer));

		Q_strncpyz( buffer, s, sizeof(buffer) );
		COM_DefaultExtension( buffer, sizeof(buffer), ".wav");
		ent->noise_index = G_SoundIndex(buffer);
	} else { // by spawnTEnt or *.spawn file
		ent->noise_index = ent->count;
	}

	// a repeating speaker can be done completely client side
	if((ent->spawnflags & 4) == 0) {
		ent->s.eType = ET_SPEAKER;
	} else {
		ent->s.eType = ET_GLOBALSPEAKER;
	}
	ent->s.eventParm = ent->noise_index;
	ent->s.frame = (int)(ent->wait * 10);
	ent->s.clientNum = (int)(ent->random * 10);


	// check for prestarted looping sound
	if ( (ent->spawnflags & 1) != 0 ) {
		ent->s.loopSound = ent->noise_index;
	}

	ent->use = Use_Target_Speaker;

	if ((ent->spawnflags & 4) != 0) {
		ent->r.svFlags |= SVF_BROADCAST;
	}

	VectorCopy( ent->s.origin, ent->s.pos.trBase );

	// must link the entity so we get areas and clusters so
	// the server can determine who to send updates to
	trap_LinkEntity( ent );
}



//==========================================================

/*QUAKED target_laser (0 .5 .8) (-8 -8 -8) (8 8 8) START_ON
-----DESCRIPTION-----
When triggered, fires a laser.  You can either set a target or a direction.

-----SPAWNFLAGS-----
1: START_ON - will be on at spawn

-----KEYS-----
"targetname" - when used will toggle on/off
"target" - point to fire laser at
*/
static void target_laser_think (/*@shared@*/ gentity_t *self) {
	vec3_t	end;
	trace_t	tr;
	vec3_t	point;

	// if pointed at another entity, set movedir to point at it
	if ( self->enemy != NULL ) {
		VectorMA (self->enemy->s.origin, 0.5f, self->enemy->r.mins, point);
		VectorMA (point, 0.5f, self->enemy->r.maxs, point);
		VectorSubtract (point, self->s.origin, self->movedir);
		VectorNormalize (self->movedir);
	}

	// fire forward and see what we hit
	VectorMA (self->s.origin, 2048, self->movedir, end);

	memset(&tr, 0, sizeof(trace_t));
	trap_Trace( &tr, self->s.origin, NULL, NULL, end, self->s.number, CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_CORPSE);

	if ( tr.entityNum != 0 ) {
		// hurt it if we can
		G_Combat_Damage ( &g_entities[tr.entityNum], self, self->activator, self->movedir, 
			tr.endpos, self->damage, DAMAGE_NO_KNOCKBACK, MOD_TARGET_LASER);
	}

	VectorCopy (tr.endpos, self->s.origin2);

	trap_LinkEntity( self );
	self->nextthink = level.time + FRAMETIME;
}

static void target_laser_on (/*@shared@*/ gentity_t *self)
{
	if (self->activator == 0) {
		self->activator = self;
	}

	target_laser_think (self);
}

static void target_laser_off (/*@shared@*/ gentity_t *self)
{
	trap_UnlinkEntity( self );
	self->nextthink = 0;
}

static void target_laser_use (/*@shared@*/ gentity_t *self, /*@shared@*/  /*@unused@*/ gentity_t *other, /*@shared@*/ gentity_t *activator)
{
	if(activator != NULL) {
		self->activator = activator;
	}

	if ( self->nextthink > 0 ) {
		target_laser_off (self);
	} else {
		target_laser_on (self);
	}
}

static void target_laser_start (/*@shared@*/ gentity_t *self)
{
	gentity_t *ent = NULL;

	self->s.eType = ET_BEAM;

	if (self->target != NULL) {
		ent = G_Find (NULL, FOFS(targetname), self->target);
		if (ent == NULL) {
			if(self->classname != NULL && self->target != NULL) {
				DEVELOPER(G_Printf (S_COLOR_YELLOW "[Entity-Error] %s at %s: %s is a bad target\n", self->classname, vtos(self->s.origin), self->target););
			} else {
				DEVELOPER(G_Printf (S_COLOR_YELLOW "[Entity-Error] could not find target\n"););
			}
			G_FreeEntity(self);
			return;
		}
		self->enemy = ent;
	} else {
		G_SetMovedir (self->s.angles, self->movedir);
	}

	self->use = target_laser_use;
	self->think = target_laser_think;

	if ( self->damage == 0 ) {
		self->damage = 1;
	}

	if((self->spawnflags & 1) != 0) {
		target_laser_on (self);
	} else {
		target_laser_off (self);
	}
}

void SP_target_laser (gentity_t *self)
{
	self->type = ENT_TARGET_LASER;

	// let everything else get spawned before we start firing
	self->think = target_laser_start;
	self->nextthink = level.time + FRAMETIME;
}


//==========================================================

static void target_teleporter_use( /*@shared@*/ gentity_t *self, /*@shared@*/ gentity_t *other, /*@shared@*/ gentity_t *activator ) {
	gentity_t	*dest = NULL;
	vec3_t		destPoint;
	vec3_t		tracePoint;
	trace_t		tr;

	if(Q_stricmp(self->swapname, activator->target) == 0) {
		self->flags ^= FL_LOCKED;
		return;
	}

	if((self->flags & FL_LOCKED) != 0) {
		return;
	}

	if (activator == NULL || activator->client == NULL) {
		return;
	}

	dest = 	G_PickTarget( self->target );
	if (dest == NULL) {
		DEVELOPER(G_Printf (S_COLOR_YELLOW "[Entity-Error] Couldn't find teleporter destination\n"););
		G_FreeEntity(self);
		return;
	}

	VectorCopy(dest->s.origin, destPoint);

	if(( self->spawnflags & 2 ) != 0) {
		destPoint[2] += dest->r.mins[2]; 
		destPoint[2] -= other->r.mins[2];
		destPoint[2] += 1;
	} else {
		VectorCopy( dest->s.origin, tracePoint );
		tracePoint[2] -= 4096;

		memset(&tr, 0, sizeof(trace_t));
		trap_Trace( &tr, dest->s.origin, dest->r.mins, dest->r.maxs, tracePoint, dest->s.number, MASK_PLAYERSOLID ); 
		VectorCopy( tr.endpos, destPoint );

		//offset the player's bounding box.
		destPoint[2] -= activator->r.mins[2];

		//add 1 to ensure non-direct collision
		destPoint[2] += 1;
	}

	if(( self->spawnflags & 1 ) != 0) {
		if ( TransDat[activator->client->ps.clientNum].beamTime == 0 ) {
			G_InitTransport( activator->client->ps.clientNum, destPoint, dest->s.angles );				 
		}
	} else {
		TeleportPlayer( activator, destPoint, dest->s.angles, TP_NORMAL );
	}
}

/*QUAKED target_teleporter (1 0 0) (-8 -8 -8) (8 8 8) VISUAL_FX SUSPENDED DEACTIVATED
-----DESCRIPTION-----
The activator will be instantly teleported away.

-----SPAWNFLAGS-----
1: VISUAL_FX - Instead of instant teleportation with no FX, entity will play the Star Trek style
	transporter effect and teleport over the course of an 8 second cycle.
	
	NB-If using the transporter VISUAL_FX, place the target entity so it's right on top of
	the surface you want the player to appear on.  It's been hardcoded to take this offset into
	account only when the VISUAL_FX flag is on
	
2: SUSPENDED - Unless this is checked, the player will materialise on top of the first solid surface underneath the entity
4: DEACTIVATED - Teleporter will be deactiavted at spawn

-----KEYS-----
"targetname" - Any entities targeting this will activate it when used.
"target" - Name of one or more info_notnull entities that the player teleport to.
"swapname" - Activate/Deactivate (Using entity needs SELF/NOACTIVATOR)
*/
void SP_target_teleporter( gentity_t *self ) {
	self->type = ENT_TARGET_TELEPORTER;

	if (self->targetname == NULL) {
		if(self->classname != NULL) {
			DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] untargeted %s at %s\n", self->classname, vtos(self->s.origin)););
		} else {
			DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] entity is untargeted\n"););
		}
		G_FreeEntity(self);
		return;
	}

	if((self->spawnflags & 4) != 0) {
		self->flags ^= FL_LOCKED;
	}

	self->use = target_teleporter_use;
}

//==========================================================

/*QUAKED target_relay (.5 .5 .5) (-8 -8 -8) (8 8 8) RED_ONLY BLUE_ONLY RANDOM SELF
-----DESCRIPTION-----
This doesn't perform any actions except fire its targets.
It is also a nice function-caller via luaUse.

-----SPAWNFLAGS-----
1: RED_ONLY - Only members from the red team can use this
2: BLUE_ONLY - Only members from the blue team can use this
4: RANDOM - only one of the entities with matching targetname will be fired, not all of them
8: SELF - use the entity as activator instead of it's own activator when using it's targets (use this flag for targets that are not called by their targetname (e.g. swapname))

-----KEYS-----
"targetname" - calling this will fire the entity
"target" - targetname of entities to fire
"luaUse" - lua function to call on use
*/

static void target_relay_use (/*@shared@*/ gentity_t *self, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ gentity_t *activator) {
	if ( ( self->spawnflags & 1 ) != 0 && activator != NULL && activator->client != NULL 
		&& activator->client->sess.sessionTeam != TEAM_RED ) {
			return;
	}
	if ( ( self->spawnflags & 2 ) != 0 && activator != NULL && activator->client != NULL
		&& activator->client->sess.sessionTeam != TEAM_BLUE ) {
			return;
	}

	if(activator == NULL) {
		return;
	}

	if( (self->spawnflags & 4) != 0 ) {
		gentity_t	*ent = NULL;

		ent = G_PickTarget( self->target );
		if ( ent != NULL && ent->use != NULL ) {
			if((self->spawnflags & 8) != 0) {
				ent->use(ent, self, self);
#ifdef G_LUA
				if(self->luaUse != NULL) {
					LuaHook_G_EntityUse(self->luaUse, self->s.number, self->s.number, self->s.number);
				}
#endif
			} else {
				ent->use( ent, self, activator );
#ifdef G_LUA
				if(self->luaUse != NULL) {
					LuaHook_G_EntityUse(self->luaUse, self->s.number, other->s.number, self->s.number);
				}
#endif
			}
		}
		return;
	}
	if((self->spawnflags & 8) != 0) {
		G_UseTargets(self, self);
	} else {
		G_UseTargets (self, activator);
	}
}

void SP_target_relay (gentity_t *self) {
	self->type = ENT_TARGET_RELAY;
	self->use = target_relay_use;
}


//==========================================================

/*QUAKED target_kill (.5 .5 .5) (-8 -8 -8) (8 8 8)
-----DESCRIPTION-----
Kills the activator.

-----SPAWNFLAGS-----
none

-----KEYS-----
"targetanme" - the activator calling this will be telefragged if client
*/
static void target_kill_use( /*@shared@*/ /*@unused@*/ gentity_t *self, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ gentity_t *activator ) {
	if(activator != NULL) {
		G_Combat_Damage ( activator, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, MOD_TELEFRAG);
	}
}

void SP_target_kill( gentity_t *self ) {
	self->type = ENT_TARGET_KILL;

	self->use = target_kill_use;

	// don't need to send this to clients
	self->r.svFlags &= SVF_NOCLIENT;
	trap_LinkEntity(self);
}

static void target_location_linkup(/*@shared@*/ gentity_t *ent)
{
	int i;
	int n;
	//gentity_t *tent;

	if (level.locationLinked) {
		return;
	}

	level.locationLinked = qtrue;

	level.locationHead = NULL;

	trap_SetConfigstring( CS_LOCATIONS, "unknown" );

	for (i = 0, ent = g_entities, n = 1;
		i < level.num_entities;
		i++, ent++) {
			if ((ent->classname != NULL) && (ent->type == ENT_TARGET_LOCATION)) {
				// lets overload some variables!
				ent->health = n; // use for location marking
				trap_SetConfigstring( CS_LOCATIONS + n, ent->message );
				n++;
				ent->nextTrain = level.locationHead;
				level.locationHead = ent;
			}
	}
	// All linked together now
}

/*QUAKED target_location (0 0.5 0) (-8 -8 -8) (8 8 8)
-----DESCRIPTION-----
Location to display in the player-list (usually Tab)
Closest target_location in sight used for the location, if nonein sight, closest in distance

-----SPAWNFLAGS-----
none

-----KEYS-----
"message" - location name to display. Can be colorized using '^X' where X is one of the following numbers
	0:white 1:red 2:green 3:yellow 4:blue 5:cyan 6:magenta 7:white

-----LocEdit-----
target_locations can also be spawned by a <mapname>.locations-file.
While creating this was hard work for many years, a new command makes it quite easy: LocEdit

There are a few basic commands:
/locedit start <type>
	This will open the file. 
	For type set 1 if you'd like to restrict a location so only admins can autorise transportation there.
	Else set 0.

For Type = 0: /locedit add "<location-name>"
For Type = 1: /locedit add <protected> "<location-name>"
	this will add a new location to the list.
	It will grab your current position as well as your yaw-angle (around the Z-Axis) and dump them to the file with the parameters.
	If you set protected 1 only admins can authorise transportation there.
	location-name can be colorized as stated above. You need to put it in "".

/locedit nl
	this will simply add an empty line. If you have to manually edit the file at a later date this will help you get oriented.

/locedit stop
	this will close the file.
*/
void SP_target_location( gentity_t *self ){
	self->type = ENT_TARGET_LOCATION;

	self->think = target_location_linkup;
	self->nextthink = level.time + 200;  // Let them all spawn first

	G_SetOrigin( self, self->s.origin );
}

/*QUAKED target_counter (1.0 0 0) (-4 -4 -4) (4 4 4)
-----DESCRIPTION-----
Acts as an intermediary for an action that takes multiple inputs.
After the counter has been triggered "count" times it will fire all of it's targets and remove itself.

-----SPAWNFLAGS-----
none

-----KEYS-----
"count" - number of usages required before targets are fired. Default is 2
"targetname" - Will reduce count by one.
"target" will be fired once count hit's 0
*/

static void target_counter_use( /*@shared@*/ gentity_t *self, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ gentity_t *activator )
{
	if ( self->count == 0 ) {
		return;
	}

	self->count--;

	if ( self->count != 0 ) {
		return;
	}

	if(activator != NULL) {
		self->activator = activator;
	} else { 
		self->activator = self;
	}

	G_UseTargets( self, activator );
}

void SP_target_counter (gentity_t *self)
{
	self->type = ENT_TARGET_COUNTER;

	self->wait = -1.0f;
	if (self->count == 0) {
		self->count = 2;
	}

	self->use = target_counter_use;

	// don't need to send this to clients
	self->r.svFlags &= SVF_NOCLIENT;
	trap_LinkEntity(self);
}

/*QUAKED target_objective (1.0 0 0) (-4 -4 -4) (4 4 4)
-----DESCRIPTION-----
When used, the objective in the <mapname>.efo with this objective's "count" will be marked as completed
NOTE: the objective with the lowest "count" will be considered the current objective

-----SPAWNFLAGS-----
none

-----KEYS-----
"count" - number of objective (as listed in the maps' <mapname>.efo)
"targetname" - when fired marks objective as complete

*/

// Remove this?
static void target_objective_use( /*@shared@*/ gentity_t *self, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ /*@unused@*/ gentity_t *activator )
{
	gentity_t *tent = NULL;

	tent = G_TempEntity( self->r.currentOrigin, EV_OBJECTIVE_COMPLETE );

	if(tent == NULL) {
		return; // uh ohhhh 
	}

	//Be sure to send the event to everyone
	tent->r.svFlags |= SVF_BROADCAST;
	tent->s.eventParm = self->count;
}

void SP_target_objective (gentity_t *self)
{
	self->type = ENT_TARGET_OBJECTIVE;

	if ( self->count <= 0 ) {
		//FIXME: error msg
		G_FreeEntity( self );
		return;
	}

	if ( self->targetname != NULL )	{
		self->use = target_objective_use;
	}

	level.numObjectives++;
}

/*================
RPG-X Modification
Phenix
13/06/2004
================*/

/*QUAKED target_boolean (.5 .5 .5) (-8 -8 -8) (8 8 8) START_TRUE SWAP_FIRE SELF
-----DESCRIPTION-----
Acts as an if statement. When fired normaly if true it fires one target, if false it fires another.

-----SPAWNFLAGS-----
1: START_TRUE - the boolean starts true.
2: SWAP_FIRE - when the swap command is issued it will also fire the new target.
4: SELF - use the entity as activator instead of it's own activator when using it's targets (use this flag for targets that are not called by their targetanme)

-----KEYS-----
"targetname" - this when fired will fire the target according to which state the boolean is in
"swapname" - this when fired will swap the boolean from one state to the opposite
"truename" - this when fired will swap the boolean's state to true
"falsename" - this when fired will sawp the boolean's state to false
"truetarget" - this will be fired if the boolean is true then the targetname is recieved
"falsetarget" - this will be fired if the boolean is false then the targetname is recieved
*/

static void target_boolean_use (/*@shared@*/ gentity_t *self, /*@shared@*/ gentity_t *other, /*@shared@*/ gentity_t *activator) {

	if ((self == NULL) || (other == NULL) || (activator == NULL)) {
		return;
	}

	if (self->truetarget == NULL || Q_stricmp(self->truetarget,"(NULL)") == 0) {
		G_SpawnString( "truetarget", "DEFAULTTARGET", &self->truetarget );
	}

	if (other->target != NULL && self->targetname != NULL && Q_stricmp(other->target, self->targetname) == 0) {	
		if (self->booleanstate == qtrue) {
			if((self->spawnflags & 4) != 0) {
				self->target = self->truetarget;
				G_UseTargets2( self, self, self->truetarget );
			} else {
				G_UseTargets2( self, activator, self->truetarget );
			}
			return;
		} else {
			if((self->spawnflags & 4) != 0) {
				self->target = self->falsetarget;
				G_UseTargets2( self, self, self->falsetarget );
			} else {
				G_UseTargets2( self, activator, self->falsetarget );
			}
			return;
		}

	} else if (Q_stricmp(other->target, self->truename) == 0) {
		self->booleanstate = qtrue;					//Make the boolean true
		return;
	} else if (Q_stricmp(other->target, self->falsename) == 0) {
		self->booleanstate = qfalse;					//Make the boolean false
		return;

	} else if (Q_stricmp(other->target, self->swapname) == 0) {
		if (self->booleanstate==qtrue) {			//If the boolean is true then swap to false
			self->booleanstate = qfalse;
			if((self->spawnflags & 2) != 0) {
				if((self->spawnflags & 4) != 0) {
					self->target = self->falsetarget;
					G_UseTargets2( self, self, self->falsetarget );
				} else {
					G_UseTargets2( self, activator, self->falsetarget );
				}
			}
		} else {
			self->booleanstate = qtrue;
			if((self->spawnflags & 2) != 0) {
				if((self->spawnflags & 4) != 0) {
					self->target = self->truetarget;
					G_UseTargets2( self, self, self->truetarget );
				} else {
					G_UseTargets2( self, activator, self->truetarget );
				}
			}
		}

		return;
	}
}

void SP_target_boolean (gentity_t *self) {
	self->type = ENT_TARGET_BOOLEAN;

	if (!self->booleanstate && (self->spawnflags & 1) != 0) {
		self->booleanstate = qtrue;
	} else if (!self->booleanstate) {
		self->booleanstate = qfalse;
	}

	self->use = target_boolean_use;

	// don't need to send this to clients
	self->r.svFlags &= SVF_NOCLIENT;
	trap_LinkEntity(self);
}

/*QUAKED target_gravity (.5 .5 .5) (-8 -8 -8) (8 8 8) PLAYER_ONLY MAP_GRAV
-----DESCRIPTION-----
This changes the servers gravity to the ammount set.

-----SPAWNFLAGS-----
1: PLAYER_ONLY - If select this will only change the gravity for the activator. TiM: an actiator eh?
2: MAP_GRAV - Will reset player to the current global gravity.

-----KEYS-----
"gravity" - gravity value (default = g_gravity default = 800)
*/

void target_gravity_use (/*@shared@*/ gentity_t *self, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ gentity_t *activator) 
{
	//CIf spawn flag 1 is set, change gravity to specific user
	if((self->spawnflags & 1) != 0 && activator != NULL && activator->client != NULL)
	{
		if(self->targetname2 != NULL) {
			activator->client->ps.gravity = atoi(self->targetname2);
		} else { // fallback to g_gravity if targetname2 is NULL for some reason
			activator->client->ps.gravity = g_gravity.integer;
		}
		activator->client->SpecialGrav = qtrue;
	}
	//resyncing players grav to map grav.
	else if((self->spawnflags & 2) != 0 && activator != NULL && activator->client != NULL)
	{
		activator->client->ps.gravity = g_gravity.integer;
		activator->client->SpecialGrav = qfalse;
	}
	//Else change gravity for all clients
	else
	{
		if(self->targetname2 == NULL) { // fallback
			trap_Cvar_Set( "g_gravity", va("%d", g_gravity.integer));
		} else {
			trap_Cvar_Set( "g_gravity", self->targetname2 );
		}
	}
}

void SP_target_gravity (gentity_t *self) {
	char *temp;

	self->type = ENT_TARGET_GRAVITY;

	if(!self->tmpEntity) { // check for spawnTEnt
		G_SpawnString("gravity", "800", &temp);
		self->targetname2 = G_NewString(temp);
	}

	if(self->count != 0) { // support for SP
		self->targetname2 = G_NewString(va("%i", self->count));
	}

	self->use = target_gravity_use;

	// don't need to send this to clients
	self->r.svFlags &= SVF_NOCLIENT;
	trap_LinkEntity(self);
}

/*QUAKED target_shake (.5 .5 .5) (-8 -8 -8) (8 8 8)
-----DESCRIPTION-----
When fired every clients monitor will shake as if in an explosion

-----SPAWNFLAGS-----
none

-----KEYS-----
"wait" - Time that the shaking lasts for in seconds
"intensity" - Strength of shake
*/

//move this to FX and do a redirect in spawn?

void target_shake_use (/*@shared@*/ gentity_t *self, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ /*@unused@*/ gentity_t *activator) 
{
	trap_SetConfigstring( CS_CAMERA_SHAKE, va( "%f %i", self->distance, ( (int)(level.time - level.startTime) + (int)( self->wait*1000 ) ) ) );
}

void SP_target_shake (gentity_t *self) {
	self->type = ENT_TARGET_SHAKE;

	//TiM: Phenix, you're a n00b. You should always put default values in. ;P
	G_SpawnFloat( "intensity", "5", &self->distance /*was &self->intensity*/ );
	G_SpawnFloat( "wait", "5", &self->wait );

	self->use = target_shake_use;
}

/*QUAKED target_evosuit (.5 .5 .5) (-8 -8 -8) (8 8 8)
-----DESCRIPTION-----
Grants activating clent the EVA-Suit-Flag with all sideeffects associated.

-----SPAWNFLAGS-----
none

-----KEYS-----
"targetanme" - entity needs to be used
*/

void target_evosuit_use (/*@shared@*/ /*@unused@*/ gentity_t *self, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ gentity_t *activator) 
{

	if(activator == NULL || activator->client == NULL) { 
		return;
	}

	activator->flags ^= FL_EVOSUIT;
	if ((activator->flags & FL_EVOSUIT) == 0)
	{
		G_PrintfClient(activator, "%s\n", "You have taken an EVA Suit off\n");
		activator->client->ps.powerups[PW_EVOSUIT] = 0;
	}        
	else
	{
		G_PrintfClient(activator, "%s\n", "You have put an EVA Suit on\n");
		activator->client->ps.powerups[PW_EVOSUIT] = level.time + 1000000000;
	}
}

void SP_target_evosuit (gentity_t *self) {
	self->type = ENT_TARGET_EVOSUIT;

	self->use = target_evosuit_use;

	// don't need to send this to clients
	self->r.svFlags &= SVF_NOCLIENT;
	trap_LinkEntity(self);
}

//==================================================================================
//
//TiM - Turbolift Ent
//Multiple phases are broken up into multiple think functions
//
//==================================================================================

static void target_turbolift_unlock ( /*@shared@*/ gentity_t *ent )
{
	gentity_t* otherLift;

	//get target deck number lift entity
	otherLift = &g_entities[ent->count];

	//last phase - unlock turbolift doors
	{
		gentity_t *door=NULL;

		if(ent->target != NULL) {
			while ( ( door = G_Find( door, FOFS( targetname ), ent->target )) != NULL  )
			{
				if ( door->type == ENT_FUNC_DOOR )
				{
					door->flags &= ~FL_CLAMPED;
				}
			}
		}

		door = NULL;
		if ( otherLift != NULL && otherLift->target != NULL) 
		{
			while ( ( door = G_Find( door, FOFS( targetname ), otherLift->target )) != NULL  )
			{
				if ( door->type == ENT_FUNC_DOOR )
				{
					door->flags &= ~FL_CLAMPED;
				}
			}
		}
	}

	//reset lifts
	if ( otherLift != NULL) {
		otherLift->count = 0;
	}

	ent->s.time2 = 0;
	if(otherLift != NULL) {
		otherLift->s.time2 = 0;
	}

	ent->count = 0;
	ent->nextthink = 0;
	ent->think = NULL;
}


static void target_turbolift_endMove ( /*@shared@*/ gentity_t *ent )
{
	gentity_t* lights=NULL;
	gentity_t* otherLift=NULL;
	float f = 0;

	otherLift = &g_entities[ent->count];
	if ( !otherLift )
	{
		target_turbolift_unlock( ent );
		return;
	}	

	//unplay move sound
	ent->r.svFlags |= SVF_NOCLIENT;
	otherLift->r.svFlags |= SVF_NOCLIENT;

	//play end sound
	G_Sound( ent, ent->s.otherEntityNum2 );
	G_Sound( otherLift, otherLift->s.otherEntityNum2 );

	//unshow flashy bits
	//find any usables parented to the lift ent, and use them
	{
		if(ent->target != NULL) {
			while ( ( lights = G_Find( lights, FOFS( targetname ), ent->target ) ) != NULL )
			{
				if ( lights->type == ENT_FUNC_USABLE )
				{	
					if(rpg_calcLiftTravelDuration.integer == 0) {
						lights->use( lights, lights, ent );
	#ifdef G_LUA
						if(lights->luaUse != NULL) {
							LuaHook_G_EntityUse(lights->luaUse, lights-g_entities, ent-g_entities, ent-g_entities);
						}
	#endif
					}
					else {
						if(ent->s.eventParm < 0 && lights->targetname2 != NULL) {
							if(Q_stricmp(lights->targetname2, va("%s_dn", ent->target)) == 0) {
								lights->use(lights, lights, ent);
	#ifdef G_LUA
								if(lights->luaUse != NULL) {
									LuaHook_G_EntityUse(lights->luaUse, lights-g_entities, ent-g_entities, ent-g_entities);
								}
	#endif
							}
						} else if(ent->s.eventParm > 0 && lights->targetname2 != NULL) {
							if(Q_stricmp(lights->targetname2, va("%s_up", ent->target)) == 0) {
								lights->use(lights, lights, ent);
	#ifdef G_LUA
								if(lights->luaUse != NULL) {
									LuaHook_G_EntityUse(lights->luaUse, lights-g_entities, ent-g_entities, ent-g_entities);
								}
	#endif
							}
						} else {
							lights->use(lights, lights, ent);
	#ifdef G_LUA
							if(lights->luaUse != NULL) {
								LuaHook_G_EntityUse(lights->luaUse, lights-g_entities, ent-g_entities, ent-g_entities);
							}
	#endif
						}
					}
				}
			}
		}

		lights = NULL;
		if(otherLift != NULL && otherLift->target != NULL) {
			while ( ( lights = G_Find( lights, FOFS( targetname ), otherLift->target ) ) != NULL )
			{
				if ( lights->type == ENT_FUNC_USABLE )
				{
					if(rpg_calcLiftTravelDuration.integer == 0) {
						lights->use( lights, lights, ent );
#ifdef G_LUA
						if(lights->luaUse != NULL) {
							LuaHook_G_EntityUse(lights->luaUse, lights-g_entities, ent-g_entities, ent-g_entities);
						}
#endif
					}
					else {
						if(ent->s.eventParm < 0 && lights->targetname2 != NULL) {
							if(Q_stricmp(lights->targetname2, va("%s_dn", otherLift->target)) == 0) {
								lights->use(lights, lights, ent);
#ifdef G_LUA
								if(lights->luaUse != NULL) {
									LuaHook_G_EntityUse(lights->luaUse, lights-g_entities, ent-g_entities, ent-g_entities);
								}
#endif
							}
						} else if(ent->s.eventParm != 0 && lights->targetname2 != NULL) {
							if(Q_stricmp(lights->targetname2, va("%s_up", otherLift->target)) == 0) {
								lights->use(lights, lights, ent);
#ifdef G_LUA
								if(lights->luaUse != NULL) {
									LuaHook_G_EntityUse(lights->luaUse, lights-g_entities, ent-g_entities, ent-g_entities);
								}
#endif
							}
						} else {
							lights->use(lights, lights, ent);
#ifdef G_LUA
							if(lights->luaUse != NULL) {
								LuaHook_G_EntityUse(lights->luaUse, lights-g_entities, ent-g_entities, ent-g_entities);
							}
#endif
						}
					}
				}
			}
		}
	}

	// check for shader remaps
	if(rpg_calcLiftTravelDuration.integer != 0 || level.overrideCalcLiftTravelDuration != 0) {
		if((ent->truename != NULL && otherLift->truename != NULL) || (ent->falsename != NULL && otherLift->falsename != NULL)) {
			f = (float)(level.time * 0.001f);
			AddRemap(ent->targetShaderName, ent->targetShaderName, f);
			AddRemap(otherLift->targetShaderName, otherLift->targetShaderName, f);
		}
		trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
	}

	//next phase, teleport player
	ent->nextthink = level.time + ent->sound1to2;
	ent->think = target_turbolift_unlock;
}

//TiM - we'll have two sets of teleports, so let's re-use this
static void TeleportPlayers ( gentity_t* ent, gentity_t* targetLift, int numEnts, int *touch )
{
	int			i = 0;
	gentity_t	*player=NULL;
	float		dist;
	vec3_t		temp;
	vec3_t		angles;
	vec3_t		newOrigin;
	vec3_t		viewAng;

	if ( numEnts <= 0 )
		return;

	for ( i = 0; i < numEnts; i++ )
	{
		player = &g_entities[touch[i]];

		if ( !player->client )
			continue;

		//to teleport them, we need two things.  Their distance and angle from the origin
		VectorSubtract( player->client->ps.origin, ent->s.origin, temp );

		//distance + angles
		dist = VectorLength( temp );
		VectorNormalize( temp );
		vectoangles( temp, angles );

		angles[YAW] = AngleNormalize360( angles[YAW] - ent->s.angles[YAW] );

		//now... calc their new origin and view angles
		angles[YAW] = AngleNormalize360( angles[YAW] + targetLift->s.angles[YAW] );
		AngleVectors( angles, temp, NULL, NULL );

		VectorMA( targetLift->s.origin, dist, temp, newOrigin );

		VectorCopy( player->client->ps.viewangles, viewAng );
		viewAng[YAW] = AngleNormalize360( viewAng[YAW] + ( targetLift->s.angles[YAW] - ent->s.angles[YAW] ) );

		TeleportPlayer( player, newOrigin, viewAng, TP_TURBO );
	}
}

static void target_turbolift_TeleportPlayers (/*@shared@*/ gentity_t *ent )
{
	gentity_t	*targetLift;
	vec3_t		mins, maxs;
	float		time;

	//store both sets of data so they can be swapped at the same time
	int			*liftTouch;
	int			*targetLiftTouch;
	int			liftNumEnts;
	int			targetLiftNumEnts;

	//teleport the players
	targetLift = &g_entities[ent->count];

	if ( !targetLift ) {
		target_turbolift_unlock( ent );
		return;
	}

	liftTouch = (int *)malloc(MAX_GENTITIES * sizeof(int));
	if(liftTouch == NULL) {
		target_turbolift_unlock( ent );
		return;
	}
	memset(liftTouch, 0, sizeof(int) * MAX_GENTITIES);

	//scan the turbo region for players
	//in the current lift
	{
		if(!ent->tmpEntity) {
			VectorCopy( ent->r.maxs, maxs );
			VectorCopy( ent->r.mins, mins );
		} else {
			VectorAdd(ent->r.maxs, ent->s.origin, maxs);
			VectorAdd(ent->r.mins, ent->s.origin, mins);
		}

		liftNumEnts = trap_EntitiesInBox( mins, maxs, liftTouch, MAX_GENTITIES );
	}

	targetLiftTouch = (int *)malloc(MAX_GENTITIES * sizeof(int));
	if(targetLiftTouch == NULL) {
		target_turbolift_unlock( ent );
		free(liftTouch);
		return;
	}
	memset(targetLiftTouch, 0, sizeof(int) * MAX_GENTITIES);

	//the target lift
	{
		if(!targetLift->tmpEntity) {
			VectorCopy( targetLift->r.maxs, maxs );
			VectorCopy( targetLift->r.mins, mins );
		} else {
			VectorAdd(targetLift->r.maxs, targetLift->s.origin, maxs);
			VectorAdd(targetLift->r.mins, targetLift->s.origin, mins);
		}

		targetLiftNumEnts = trap_EntitiesInBox( mins, maxs, targetLiftTouch, MAX_GENTITIES );
	}

	//TiM - Teleport the players from the other target to this one
	TeleportPlayers( targetLift, ent, targetLiftNumEnts, targetLiftTouch );

	//TiM - Teleport the main players
	TeleportPlayers( ent, targetLift, liftNumEnts, liftTouch );

	if(rpg_calcLiftTravelDuration.integer != 0) {
		time = (float)(targetLift->health - ent->health);
		if(time < 0)
			time *= -1;
		time *= rpg_liftDurationModifier.value;
		time *= 1000;
		ent->think = target_turbolift_endMove;
		ent->nextthink = level.time + (time * 0.5f);
	} else {
		//first thing's first
		ent->think = target_turbolift_endMove;
		ent->nextthink = level.time + (ent->wait*0.5f);
	}

	free(liftTouch);
	free(targetLiftTouch);
}

static void target_turbolift_startSoundEnd(/*@shared@*/ gentity_t *ent) {
	ent->nextthink = -1;
	
	if(ent->parent != NULL) {
		ent->parent->r.svFlags &= ~SVF_NOCLIENT;
	}
	if(ent->touched != NULL) {
		ent->touched->r.svFlags &= ~SVF_NOCLIENT;
	}
}

static void target_turbolift_startMove (/*@shared@*/ gentity_t *ent )
{
	gentity_t*	lights=NULL;
	/*@shared@*/ gentity_t*	otherLift=NULL;
	gentity_t*  tent=NULL;
	float		time = 0, time2 = 0;
	float f = 0;

	otherLift = &g_entities[ent->count];
	if ( otherLift == NULL )
	{
		target_turbolift_unlock( ent );
		return;
	}	

	//play move sound
	if( rpg_calcLiftTravelDuration.integer != 0 ) {
		time = time2 = (float)(ent->health - otherLift->health);
		if(time < 0)
			time *= -1;
		if(ent->sound2to1 != 0) {
			if( rpg_liftDurationModifier.value * 1000 * time >= ent->distance * 1000 ) {
				tent = G_Spawn();

				if(tent == NULL) {
					return;
				}

				tent->think = target_turbolift_startSoundEnd;
				tent->nextthink = level.time + (ent->distance * 1000);
				tent->parent = ent;
				tent->touched = otherLift;
				G_AddEvent(ent, EV_GENERAL_SOUND, ent->sound2to1);
			}
		} else {
			ent->r.svFlags &= ~SVF_NOCLIENT;
			otherLift->r.svFlags &= ~SVF_NOCLIENT;
		}
	} else {
		ent->r.svFlags					&= ~SVF_NOCLIENT;

		otherLift->r.svFlags			&= ~SVF_NOCLIENT;
	}
	//show flashy bits
	//find any usables parented to the lift ent, and use them
	{
		if(ent->target != NULL) {
			while ( ( lights = G_Find( lights, FOFS( targetname ), ent->target ) ) != NULL )
			{
				if ( lights->type == ENT_FUNC_USABLE )
				{
					if(rpg_calcLiftTravelDuration.integer == 0) {
						lights->use( lights, lights, ent );
#ifdef G_LUA
						if(lights->luaUse != NULL) {
							LuaHook_G_EntityUse(lights->luaUse, lights-g_entities, ent-g_entities, ent-g_entities);
						}
#endif
					}
					else {
						if ( time2 < 0 && lights->targetname2 != NULL ) {
							if(Q_stricmp(lights->targetname2, va("%s_dn", ent->target)) == 0) {
								lights->use(lights, lights, ent );
#ifdef G_LUA
								if(lights->luaUse != NULL) {
									LuaHook_G_EntityUse(lights->luaUse, lights-g_entities, ent-g_entities, ent-g_entities);
								}
#endif
							}
						} else if ( time2 > 0 && lights->targetname2 != NULL) {
							if(Q_stricmp(lights->targetname2, va("%s_up", ent->target)) == 0) {
								lights->use(lights, lights, ent );
#ifdef G_LUA
								if(lights->luaUse != NULL) {
									LuaHook_G_EntityUse(lights->luaUse, lights-g_entities, ent-g_entities, ent-g_entities);
								}
#endif
							}
						} else {
							lights->use( lights, lights, ent);
#ifdef G_LUA
							if(lights->luaUse != NULL) {
								LuaHook_G_EntityUse(lights->luaUse, lights-g_entities, ent-g_entities, ent-g_entities);
							}
#endif
						}
					}
				}
			}
		}

		lights = NULL;
		if(otherLift->target != NULL) {
			while ( ( lights = G_Find( lights, FOFS( targetname ), otherLift->target ) ) != NULL )
			{
				if ( lights->type == ENT_FUNC_USABLE )
				{
					if(rpg_calcLiftTravelDuration.integer == 0) {
						lights->use( lights, lights, ent );
#ifdef G_LUA
						if(lights->luaUse != NULL) {
							LuaHook_G_EntityUse(lights->luaUse, lights-g_entities, ent-g_entities, ent-g_entities);
						}
#endif
					}
					else {
						if(time2 < 0 && lights->targetname2 != NULL) {
							if(Q_stricmp(lights->targetname2, va("%s_dn", otherLift->target)) == 0) {
								lights->use(lights, lights, ent);
#ifdef G_LUA
								if(lights->luaUse != NULL) {
									LuaHook_G_EntityUse(lights->luaUse, lights-g_entities, ent-g_entities, ent-g_entities);
								}
#endif
							}
						} else if(time2 > 0 && lights->targetname2 != NULL) {
							if(Q_stricmp(lights->targetname2, va("%s_up", otherLift->target)) == 0) {
								lights->use(lights, lights, ent);
#ifdef G_LUA
								if(lights->luaUse != NULL) {
									LuaHook_G_EntityUse(lights->luaUse, lights-g_entities, ent-g_entities, ent-g_entities);			
								}
#endif
							}
						} else {
							lights->use(lights, lights, ent);
#ifdef G_LUA
							if(lights->luaUse != NULL) {
								LuaHook_G_EntityUse(lights->luaUse, lights-g_entities, ent-g_entities, ent-g_entities);
							}
#endif
						}
					}
				}
			}
		}
	}

	// check for shader remaps
	if(rpg_calcLiftTravelDuration.integer != 0|| level.overrideCalcLiftTravelDuration != 0) {
		if(time2 < 0 && ent->truename != NULL && otherLift->truename != NULL) {
			f = (float)(level.time * 0.001f);
			AddRemap(ent->targetShaderName, ent->truename, f);
			AddRemap(otherLift->targetShaderName, otherLift->truename, f);
		} else if(time2 >  0 && ent->falsename != NULL && otherLift->falsename != NULL) {
			f = (float)(level.time * 0.001f);
			AddRemap(ent->targetShaderName, ent->falsename, f);
			AddRemap(otherLift->targetShaderName, otherLift->falsename, f);
		}
		trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());	
	}

	if(rpg_calcLiftTravelDuration.integer != 0) {
		ent->s.eventParm = (int)time2;
		time *= rpg_liftDurationModifier.value;
		time *= 1000;
		ent->s.time2 = level.time + time;
		otherLift->s.time2 = level.time + time;
		ent->nextthink = level.time + (time * 0.5f);
		ent->think = target_turbolift_TeleportPlayers;
	} else {
		//sent to the client for client-side rotation
		ent->s.time2 = level.time+ent->wait;
		otherLift->s.time2 = level.time+ent->wait;

		//next phase, teleport player
		ent->nextthink = level.time + (ent->wait*0.5f);
		ent->think = target_turbolift_TeleportPlayers;
	}
}

static void target_turbolift_shutDoors (/*@shared@*/  gentity_t *ent )
{
	gentity_t* door=NULL;
	gentity_t* otherLift=NULL;

	otherLift = &g_entities[ent->count];
	if ( otherLift == NULL )
	{
		target_turbolift_unlock( ent );
		return;
	}

	if(ent->target != NULL) {
		while ( ( door = G_Find( door, FOFS( targetname ), ent->target )) != NULL  )
		{
			if ( door->type == ENT_FUNC_DOOR )
			{
				if ( door->moverState != MOVER_POS1 ) {
					ent->nextthink = level.time + 500;
					return;
				}
			}
		}
	}

	door = NULL;
	if(otherLift->target != NULL) {
		while ( ( door = G_Find( door, FOFS( targetname ), otherLift->target )) != NULL  )
		{
			if ( door->type == ENT_FUNC_DOOR )
			{
				if ( door->moverState != MOVER_POS1 ) {
					ent->nextthink = level.time + 500;
					return;
				}
			}
		}
	}

	//start phase 3
	ent->think = target_turbolift_startMove;
	ent->nextthink = level.time + FRAMETIME;
}

void target_turbolift_start ( gentity_t *self )
{
	gentity_t* otherLift;

	//get target deck number lift entity
	otherLift = &g_entities[self->count];

	if ( otherLift == NULL)
	{
		target_turbolift_unlock( self );
		return;
	}

	//phase 1 - lock turbolift doors
	//lock the doors on both lifts
	{
		gentity_t *door=NULL;

		if(self->target != NULL) {
			while ( ( door = G_Find( door, FOFS( targetname ), self->target )) != NULL  )
			{
				if ( door->type == ENT_FUNC_DOOR )
				{
					door->flags |= FL_CLAMPED;
					if ( door->moverState != MOVER_POS1 )
					{
						door->nextthink = level.time;
					}
				}
			}
		}

		door = NULL;
		if(otherLift->target != NULL) {
			while ( ( door = G_Find( door, FOFS( targetname ), otherLift->target )) != NULL  )
			{
				if ( door->type == ENT_FUNC_DOOR )
				{
					door->flags |= FL_CLAMPED;
					if ( door->moverState != MOVER_POS1 )
					{
						door->nextthink = level.time;
					}
				}
			}
		}
	}

	//phase 2 - wait until both doors are shut
	self->think = target_turbolift_shutDoors;
	self->nextthink = level.time + 500;
}

static void target_turbolift_use(/*@shared@*/ gentity_t *self, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ gentity_t *activator)
{
	if(Q_stricmp(self->swapname, activator->target) == 0) {
		if(self->soundPos1 != 0)
			G_AddEvent(self, EV_GENERAL_SOUND, self->soundPos1);
		self->flags ^= FL_LOCKED;
	}

	if((self->flags & FL_LOCKED) != 0) {
		return;
	}

	if ( self->count > 0 )
	{
		trap_SendServerCommand( activator-g_entities, "print \"Unable to comply. The lift is currently in use.\n\" " );
		return;
	}

	trap_SendServerCommand( activator-g_entities, va("lift %d", self->health) );
}

/*
QUAKED target_turbolift (.5 .5 .5) ? x x x x x x x x OFFLINE
-----DESCRIPTION-----
Turbolifts are delayed teleporters that send players between
each other, maintaining their view and position so the transition is seamless.
If you target this entity with a func_usable, upon activating that useable,
a menu will appear to select decks.  If you target any useables with this
entity, they'll be triggered when the sequence starts (ie scrolling light texture brushes).
If rpg_calcLiftTravelDuration is set to one it is possible to have two usables targeted, one for the
up and one for the down driection in order to use this set targetname2 of those to 
<targetname>_up and <targetname>_dn.
If you target any doors with this entity, they will shut and lock for this sequence.
For the angles, the entity's angle must be aimed at the main set of doors to the lift area.

-----SPAWNFLAGS-----
1 - 128: X - Unknown, do not use.
256: OFFLINE - Turbolift is offline at start

-----KEYS-----
"deck" - which deck number this is (You can have multiple lifts of the same deck. Entity fails spawn if not specified)
"deckName" - name of the main features on this deck (Appears in the deck menu, defaults to 'Unknown')
	use either this or a <mapname>.turbolift-file to store the strings, not both simultainously
"wait" - number of seconds to wait until teleporting the players (1000 = 1 second, default 3000)
"soundLoop" - looping sound that plays in the wait period (Defaults to EF SP's sound. '*' for none)
"soundEnd" - sound that plays as the wait period ends. (Defaults to EF SP's sound. '*' for none)
"soundStart - sound that plays when the lift starts moving
"soundStartLength" - how long the start sound is in seconds
"soundDeactivate" - sound to play if player tries to use an deactivated turbolift
"waitEnd" - how long to wait from the lift stopping to the doors opening (default 1000 )
"swapname" - toggles turbolift on/off
"targetShaderName" - lights off shader
"falsename" - lights up
"truename" - lights down
"override" - if set to 1 overrides rpg_calcLiftTravelDuration

-----LUA-----
Retrofit:
Turbolifts are a good thing to retrofit, however they have 2 requirements:
- a Transporter based turbolift (seamless transportation, does not work wit func_train)
- at least 1 usable at any turbolift location
If those are fuffilled you can use the following code at level init to set up the turbolift.
(this is from enterprise-e-v2 and uses the outdated SetKeyValue-Command. Use Set<key> instead)

		game.Print("--Deck 1 ...");
			game.Print("---redirecting usables ...");
				ent = entity.FindBModel(90);
				ent:SetKeyValue("target", "tld1");
				ent:SetKeyValue("luaUse", "turbosound");
				ent = entity.FindBModel(86);
				ent:SetKeyValue("target", "tld1");
				ent:SetKeyValue("luaUse", "turbosound");
				ent = entity.FindBModel(87);
				ent:SetKeyValue("target", "tld1");
				ent:SetKeyValue("luaUse", "turbosound");
				ent = entity.FindBModel(167);
				ent:SetKeyValue("target", "tld1");
				ent:SetKeyValue("luaUse", "turbosound");
				ent = entity.FindBModel(88);
				ent:SetKeyValue("target", "tld1");
				ent:SetKeyValue("luaUse", "turbosound");
				ent = entity.FindBModel(89);
				ent:SetKeyValue("target", "tld1");
				ent:SetKeyValue("luaUse", "turbosound");
			game.Print("---renaming doors ...");
				ent = entity.FindBModel(7);
				ent:SetKeyValue("targetname", "tld1doors");
				ent = entity.FindBModel(8);
				ent:SetKeyValue("targetname", "tld1doors");
			game.Print("---Adding turbolift ...");
				ent = entity.Spawn();
				ent.SetupTrigger(ent, 144, 100, 98);
				ent:SetKeyValue("classname", "target_turbolift");
				ent:SetKeyValue("targetname", "tld1");
				ent:SetKeyValue("target", "tld1doors");
				ent:SetKeyValue("health", "1");
				ent:SetKeyValue("wait", 3000);
				entity.CallSpawn(ent);
				mover.SetPosition(ent, -2976, 8028, 887);
				mover.SetAngles(ent, 0, 270, 0);

Turbolift descriptions have to be added in via <mapname>.turbolift-file.

You may also add in a sound to the usable opening the UI. This is described in func_usable.
*/

void SP_target_turbolift ( gentity_t *self )
{
	int				i;
	char*			loopSound;
	char*			endSound;
	char*			idleSound;
	char*			startSound;
	char*			deactSound;
	int				len;
	fileHandle_t	f = 0;
	char			fileRoute[MAX_QPATH];
	char			mapRoute[MAX_QPATH];
	char			serverInfo[MAX_TOKEN_CHARS];

	if(self == NULL) {
		return;
	}

	self->type = ENT_TARGET_TURBOLIFT;

	//cache the moving sounds
	G_SpawnString( "soundLoop", "sound/movers/plats/turbomove.wav", &loopSound );
	G_SpawnString( "soundEnd", "sound/movers/plats/turbostop.wav", &endSound );
	G_SpawnString( "soundIdle", "100", &idleSound);
	G_SpawnString( "soundStart", "100", &startSound);
	G_SpawnFloat( "soundStartLength", "100", &self->distance);
	G_SpawnString( "soundDeactivate", "100", &deactSound );

	self->s.loopSound				= G_SoundIndex( loopSound ); //looping sound
	self->s.otherEntityNum2			= G_SoundIndex( endSound );	//End Phase sound
	self->n00bCount					= G_SoundIndex( idleSound );
	self->sound2to1					= G_SoundIndex( startSound );
	self->soundPos1					= G_SoundIndex( deactSound );

	if((self->spawnflags & 512) != 0) {
		self->flags ^= FL_LOCKED;
	}

	//get deck num
	G_SpawnInt( "deck", "0", &i );
	//kill the ent if it isn't valid
	if ( i <= 0 && self->tmpEntity == qfalse)
	{
		DEVELOPER(G_Printf( S_COLOR_YELLOW "[Entity-Error] A turbolift entity does not have a valid deck number!\n" ););
		G_FreeEntity( self );
		return;
	}

	if(!self->tmpEntity)
		self->health = i;
	self->count = 0; //target/targetted lift
	G_SpawnFloat( "wait", "3000", &self->wait );
	G_SpawnInt( "waitEnd", "1000", &self->sound1to2 );

	G_SpawnInt("override", "0", &i);
	if(i != 0) {
		level.overrideCalcLiftTravelDuration = i;
	}

	if(self->tmpEntity == qfalse && self->model != NULL) {
		trap_SetBrushModel( self, self->model );
	}
	self->r.contents = CONTENTS_TRIGGER;		// replaces the -1 from trap_SetBrushModel
	self->r.svFlags = SVF_NOCLIENT;
	self->s.eType = ET_TURBOLIFT;				//TiM - Client-side sound FX

	trap_LinkEntity( self );

	VectorCopy( self->r.mins, self->s.angles2 );
	VectorCopy( self->r.maxs, self->s.origin2 );

	VectorAverage( self->r.mins, self->r.maxs, self->s.origin );
	G_SetOrigin( self, self->s.origin );

	//insert code to worry about deck name later
	self->use = target_turbolift_use;

	if ( level.numDecks >= MAX_DECKS ) {
		return;
	}

	//get the map name out of the server data
	memset(serverInfo, 0, MAX_TOKEN_CHARS * sizeof(char));
	trap_GetServerinfo( serverInfo, sizeof( serverInfo ) );

	//TiM - Configure the deck number and description into a config string
	memset(mapRoute, 0, sizeof(char) * MAX_QPATH);
	memset(fileRoute, 0, sizeof(char) * MAX_QPATH);
	Com_sprintf( mapRoute, sizeof( mapRoute ), "maps/%s", Info_ValueForKey( serverInfo, "mapname" ) );
	BG_LanguageFilename( mapRoute, "turbolift", fileRoute );

	//Check for a turbolift cfg
	len = trap_FS_FOpenFile( fileRoute, &f, FS_READ );
	trap_FS_FCloseFile( f );

	//if no file was found, resort to the string system.
	//BUT! we shouldn't rely on this system if we can 
	if ( len <= 0 )
	{
		char		infoString[MAX_TOKEN_CHARS];
		char*		deckNamePtr;
		char		deckName[57];
		gentity_t*	prevDeck=NULL;
		qboolean	deckFound=qfalse;

		while ( ( prevDeck = G_Find( prevDeck, FOFS( classname ), "target_turbolift" ) ) != NULL )
		{
			if ( prevDeck != self && prevDeck->health == self->health )
			{
				deckFound = qtrue;
				break;
			}
		}

		//this deck number hasn't been registered b4
		if ( !deckFound )
		{
			G_SpawnString( "deckName", "Unknown", &deckNamePtr );
			memset(deckName, 0, sizeof(char) * 57);
			Q_strncpyz( deckName, deckNamePtr, sizeof( deckName ) );

			memset(infoString, 0, sizeof(char) * MAX_TOKEN_CHARS);
			trap_GetConfigstring( CS_TURBOLIFT_DATA, infoString, sizeof( infoString ) );

			if ( infoString[0] == 0 )
			{
				Com_sprintf( infoString, sizeof( infoString ), "d%i\\%i\\n%i\\%s\\", level.numDecks, self->health, level.numDecks, deckName );
			}
			else
			{
				Com_sprintf( infoString, sizeof( infoString ), "%sd%i\\%i\\n%i\\%s\\", infoString, level.numDecks, self->health, level.numDecks, deckName );
			}

			trap_SetConfigstring( CS_TURBOLIFT_DATA, infoString );
			level.numDecks++;
		}
	}

	level.numBrushEnts++;
}



/* ==============
END MODIFICATION
===============*/

//RPG-X | GSIO01 | 08/05/2009
/*QUAKED target_doorlock (1 0 0) (-8 -8 -8) (8 8 8) PRIVATE
-----DESCRIPTION-----
Locks/Unlocks a door.

-----SPAWNFLAGS-----
1: PRIVATE - if set, lockMsg/unlockMsg are only printed for activator

-----KEYS-----
"target" - breakable to repair (either it's targetname or it's targetname2)
"lockMsg" - message printed if door gets locked
"unlockMsg" - message printed if door gets unlocked
*/
void target_doorLock_use(/*@shared@*/ gentity_t *ent, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ gentity_t* activator) {
	gentity_t	*target = NULL;

	if(ent->target == NULL) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] Target of target_doorlock at %s is NULL!\n", vtos(ent->s.origin)););
		return;
	}

	target = G_Find(NULL, FOFS(targetname2), ent->target);
	if(target == NULL) { 
		return;
	}

	if((target->flags & FL_LOCKED) == 0) {
		if(ent->swapname != NULL) {
			if((ent->spawnflags & 1) != 0 && activator != NULL && activator->client != NULL) {
				trap_SendServerCommand(activator-g_entities, va("servermsg %s", ent->swapname));
			} else {
				trap_SendServerCommand(-1, va("servermsg %s", ent->swapname));
			}
		}
	} else {
		if(ent->truename != NULL) {
			if((ent->spawnflags & 1) != 0 && activator != NULL && activator->client != NULL) {
				trap_SendServerCommand(activator-g_entities, va("servermsg %s", ent->truename));
			} else {
				trap_SendServerCommand(-1, va("servermsg %s", ent->truename));
			}
		}
	}

	if((target->type == ENT_FUNC_DOOR) || (target->type == ENT_FUNC_DOOR_ROTATING)) {
		target->flags ^= FL_LOCKED;
	} else {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] Target %s of target_doorlock at %s is not a door!\n", ent->target, vtos(ent->s.origin)););
		return;
	}
}

void SP_target_doorLock(gentity_t *ent) {
	char *temp;

	ent->type = ENT_TARGET_DOORLOCK;

	if(ent->target == NULL) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_doorlock at %s without target!\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}

	G_SpawnString("lockMsg", "", &temp);
	ent->swapname = G_NewString(temp); // ent->swapnmae = temp or strcpy(...) screws everthing up
	G_SpawnString("unlockMsg", "", &temp);
	ent->truename = G_NewString(temp);
	ent->use = target_doorLock_use;

	// don't need to send this to clients
	ent->r.svFlags &= SVF_NOCLIENT;
	trap_LinkEntity(ent);
}

//RPG-X | GSIO01 | 11/05/2009 | MOD START
/*QUAKED target_alert (1 0 0) (-8 -8 -8) (8 8 8) SOUND_TOGGLE SOUND_OFF
-----DESCRIPTION-----
This entity acts like 3-Alert-Conditions scripts.

Any of the func_usables that are used as buttons must have the NO_ACTIVATOR spawnflag.

-----SPAWNFLAGS-----
1: SOUND_TOGGLE - if set the alert sound can be toggled on/off by using the alerts trigger again.
2: SOUND_OFF - if SOUND_TOGGLE is set, the alert will be silent at beginning

-----KEYS-----
"greenname" - the trigger for green alert should target this				
"yellowname" - the trigger for yellow alert should target this				
"redname" - the trigger for red alert should target this				
"bluename" - the trigger for blue alert should target this				
"greentarget" - anything that should be toggled when activating green alert	
"yellowtarget" - anything that should be toggled when activating yellow alert
"redtarget" - anything that should be toggled when activating red alert	
"bluetarget" - anything that should be toggled when activating blue alert	
"greensnd" - targetname of target_speaker with sound for green alert		
"yellowsnd" - targetname of target_speaker with sound for yellow alert		
"redsnd" - targetname of target_speaker with sound for red alert			
"bluesnd" - targetname of target_speaker with sound for blue alert

shader remapping:
"greenshader" - shadername of condition green
"yellowshader" - shadername of condition yellow
"redshader" - shadername of condition red
"blueshader" - shadername of condition blue

You can remap multiple shaders by separating them with \n.
Example: "greenshader"	"textures/alert/green1\ntextures/alert/green2"
*/

target_alert_Shaders_s alertShaders;

void target_alert_remapShaders(int target_condition) {
	float f = 0;
	int i;

	switch(target_condition) {
	case 1: // yellow
		for(i = 0; i < alertShaders.numShaders; i++) {
			f = (float)(level.time * 0.001f);
			if(alertShaders.greenShaders == NULL || alertShaders.greenShaders[i] == NULL || alertShaders.yellowShaders == NULL || alertShaders.yellowShaders[i] == NULL) {
				break;
			}
			AddRemap(alertShaders.greenShaders[i], alertShaders.yellowShaders[i], f);
		}
		trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
		break;
	case 2: // red
		for(i = 0; i < alertShaders.numShaders; i++) {
			f = (float)(level.time * 0.001f);
			if(alertShaders.greenShaders == NULL || alertShaders.greenShaders[i] == NULL || alertShaders.redShaders == NULL || alertShaders.redShaders[i] == NULL) {
				break;
			}
			AddRemap(alertShaders.greenShaders[i], alertShaders.redShaders[i], f);
		}
		trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
		break;
	case 3: // blue
		for(i = 0; i < alertShaders.numShaders; i++) {
			f = (float)(level.time * 0.001f);
			if(alertShaders.greenShaders == NULL || alertShaders.greenShaders[i] == NULL || alertShaders.blueShaders == NULL || alertShaders.blueShaders[i] == NULL) {
				break;
			}
			AddRemap(alertShaders.greenShaders[i], alertShaders.blueShaders[i], f);
		}
		trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
		break;
	case 0: // green
	default:
		for(i = 0; i < alertShaders.numShaders; i++) {
			f = (float)(level.time * 0.001f);
			if(alertShaders.greenShaders == NULL ||  alertShaders.greenShaders[i] == NULL) break;
			AddRemap(alertShaders.greenShaders[i], alertShaders.greenShaders[i], f);
		}
		trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
		break;
	}

}

void target_alert_use(/*@shared@*/ gentity_t *ent, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ gentity_t *activator) {

	gentity_t *healthEnt;

	if(!activator) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_alert_use called with NULL activator.\n"););
		return;
	}
	if(Q_stricmp(activator->target, ent->swapname) == 0) {
		if(ent->damage == 0) {
			if((ent->spawnflags & 1) != 0) {
				if(ent->health != 0) {
					ent->health = 0;
				} else {
					ent->health = 1;
				}
				ent->target = ent->greensound;
				G_UseTargets(ent, ent);
			}
		} else {
			switch(ent->damage) {
			case 1: // yellow
				if(ent->health != 0) {
					ent->target = ent->yellowsound;
					G_UseTargets(ent, ent);
					ent->health = 0;
				}
				ent->target = ent->falsetarget;
				G_UseTargets(ent, ent);
				break;
			case 2: // red
				if(ent->health != 0) {
					ent->target = ent->redsound;
					G_UseTargets(ent, ent);
					ent->health = 0;
				}
				ent->target = ent->paintarget;
				G_UseTargets(ent, ent);
				break;
			case 3: // blue
				if(ent->health != 0) {
					ent->target = ent->bluesound;
					G_UseTargets(ent, ent);
					ent->health = 0;
				} 
				ent->target = ent->targetname2;
				G_UseTargets(ent, ent);
				break;
			}
			if(ent->spawnflags == 0) {
				ent->target = ent->greensound;
				G_UseTargets(ent, ent);
			} else if((ent->spawnflags & 2) != 0) {
				ent->health = 0;
			} else {
				if(ent->spawnflags != 0) {
					ent->target = ent->greensound;
					G_UseTargets(ent, ent);
					ent->health = 1;
				}
			}
			target_alert_remapShaders(0);
			ent->target = ent->truetarget;
			G_UseTargets(ent, ent);
			ent->damage = 0;
		}
	} else if(Q_stricmp(activator->target, ent->truename) == 0) {
		if(ent->damage == 1) {
			if((ent->spawnflags & 1) != 0) {
				if(ent->health != 0) {
					ent->health = 0;
				} else {
					ent->health = 1;
				}
				ent->target = ent->yellowsound;
				G_UseTargets(ent, ent);
			}
		} else {
			switch(ent->damage) {
			case 0: // green
				if(ent->health != 0) {
					ent->target = ent->greensound;
					G_UseTargets(ent, ent);
					ent->health = 0;
				}
				ent->target = ent->truetarget;
				G_UseTargets(ent, ent);
				break;
			case 2: // red
				if(ent->health != 0) {
					ent->target = ent->redsound;
					G_UseTargets(ent, ent);
					ent->health = 0;
				}
				ent->target = ent->paintarget;
				G_UseTargets(ent, ent);
				break;
			case 3: // blue
				if(ent->health != 0) {
					ent->target = ent->bluesound;
					G_UseTargets(ent, ent);
					ent->health = 0;
				}
				ent->target = ent->targetname2;
				G_UseTargets(ent, ent);
				break;
			}
			if(ent->spawnflags == 0) {
				ent->target = ent->yellowsound;
				G_UseTargets(ent, ent);
			} else if((ent->spawnflags & 2) != 0) {
				ent->health = 0;
			} else {
				if(ent->spawnflags != 0) {
					ent->target = ent->yellowsound;
					G_UseTargets(ent, ent);
					ent->health = 1;
				}
			}
			target_alert_remapShaders(1);
			ent->target = ent->falsetarget;
			G_UseTargets(ent, ent);
			ent->damage = 1;
		}
	} else if(Q_stricmp(activator->target, ent->falsename) == 0) {
		if(ent->damage == 2) {
			if((ent->spawnflags & 1) != 0) {
				if(ent->health != 0) {
					ent->health = 0;
				} else {
					ent->health = 1;
				}
				ent->target = ent->redsound;
				G_UseTargets(ent, ent);
			}
		} else {
			switch(ent->damage) {
			case 0: // green
				if(ent->health != 0) {
					ent->target = ent->greensound;
					G_UseTargets(ent, ent);
					ent->health = 0;
				}
				ent->target = ent->truetarget;
				G_UseTargets(ent, ent);
				break;
			case 1: // ryellow
				if(ent->health != 0) {
					ent->target = ent->yellowsound;
					G_UseTargets(ent, ent);
					ent->health = 0;
				}
				ent->target = ent->falsetarget;
				G_UseTargets(ent, ent);
				break;
			case 3: // blue
				if(ent->health != 0) {
					ent->target = ent->bluesound;
					G_UseTargets(ent, ent);
					ent->health = 0;
				}
				ent->target = ent->targetname2;
				G_UseTargets(ent, ent);
				break;
			}
			if(ent->spawnflags == 0) {
				ent->target = ent->redsound;
				G_UseTargets(ent, ent);
			} else if((ent->spawnflags & 2) != 0) {
				ent->health = 0;
			} else {
				if(ent->spawnflags != 0) {
					ent->target = ent->redsound;
					G_UseTargets(ent, ent);
					ent->health = 1;
				}
			}
			target_alert_remapShaders(2);
			ent->target = ent->paintarget;
			G_UseTargets(ent, ent);
			ent->damage = 2;
		}
	} if(Q_stricmp(activator->target, ent->bluename) == 0) {
		if(ent->damage == 3) {
			if((ent->spawnflags & 1) != 0) {
				if(ent->health != 0) {
					ent->health = 0;
				} else {
					ent->health = 1;
				}
				ent->target = ent->bluesound;
				G_UseTargets(ent, ent);
			}
		} else {
			switch(ent->damage) {
			case 0: // green
				if(ent->health != 0) {
					ent->target = ent->greensound;
					G_UseTargets(ent, ent);
					ent->health = 0;
				} 
				ent->target = ent->truetarget;
				G_UseTargets(ent, ent);
				break;
			case 1: // yellow
				if(ent->health != 0) {
					ent->target = ent->yellowsound;
					G_UseTargets(ent, ent);
					ent->health = 0;
				} 
				ent->target = ent->falsetarget;
				G_UseTargets(ent, ent);
				break;
			case 2: // red
				if(ent->health != 0) {
					ent->target = ent->redsound;
					G_UseTargets(ent, ent);
					ent->health = 0;
				} 
				ent->target = ent->paintarget;
				G_UseTargets(ent, ent);
				break;
			}
			if(ent->spawnflags == 0) {
				ent->target = ent->bluesound;
				G_UseTargets(ent, ent);
			} else if((ent->spawnflags & 2) != 0) {
				ent->health = 0;
			} else {
				if(ent->spawnflags != 0) {
					ent->target = ent->bluesound;
					G_UseTargets(ent, ent);
					ent->health = 1;
				}
			}
			target_alert_remapShaders(3);
			ent->target = ent->targetname2;
			G_UseTargets(ent, ent);
			ent->damage = 3;
		}
	}

	//Refresh health ent if it has interconnectivity with target_alert
	healthEnt = G_Find(NULL, FOFS(classname), "target_shiphealth");
	if(healthEnt != NULL){
		if(Q_stricmp(healthEnt->falsename, ent->falsename) == 0){
			if(healthEnt->splashDamage == 0 || healthEnt->splashDamage == 1){
				if(ent->damage == 0) {
					healthEnt->splashDamage = 0;
				} else {
					healthEnt->splashDamage = 1;
				}
			}
		}
	}
	// Free activator if no classname <-- alert command
	if(activator->classname == NULL) {
		G_FreeEntity(activator);
	}
}

void target_alert_parseShaders(/*@shared@*/ gentity_t *ent) {
	char	buffer[BIG_INFO_STRING];
	char	*txtPtr;
	char	*token;
	int		currentNum = 0;

	alertShaders.numShaders = 0;

	memset(buffer, 0, sizeof(buffer));

	// condition green shaders
	if(ent->message == NULL) {
		return;
	}

	Q_strncpyz(buffer, ent->message, strlen(ent->message));
	txtPtr = buffer;
	token = COM_Parse(&txtPtr);
	while(qtrue) {
		if(token == NULL || token[0] == 0) {
			break;
		}
		if(alertShaders.greenShaders == NULL) {
			return;
		}
		alertShaders.greenShaders[alertShaders.numShaders] = G_NewString(token);
		alertShaders.numShaders++;
		if(alertShaders.numShaders > 9) {
			break;
		}
		token = COM_Parse(&txtPtr);
	}

	// condition red shaders
	if(ent->model != NULL) {
		Q_strncpyz(buffer, ent->model, strlen(ent->model));
		txtPtr = buffer;
		token = COM_Parse(&txtPtr);
		while(qtrue) {
			if(token == NULL || token[0] == 0) {
				break;
			}
			if(alertShaders.redShaders == NULL) {
				return;
			}
			alertShaders.redShaders[currentNum] = G_NewString(token);
			currentNum++;
			if(currentNum > 9) { 
				break;
			}
			token = COM_Parse(&txtPtr);
		}

		if(currentNum < alertShaders.numShaders || currentNum > alertShaders.numShaders) {
			G_Printf(S_COLOR_RED "ERROR - target_alert: number of red shaders(%i) does not equal number of green shaders(%i)!\n", currentNum, alertShaders.numShaders);
		}

		currentNum = 0;
	}

	// condition blue shaders
	if(ent->model2 != NULL) {
		Q_strncpyz(buffer, ent->model2, strlen(ent->model2));
		txtPtr = buffer;
		token = COM_Parse(&txtPtr);
		while(qtrue) {
			if(token == NULL || token[0] == 0) {
				break;
			}
			if(alertShaders.blueShaders == NULL) {
				return;
			}
			alertShaders.blueShaders[currentNum] = G_NewString(token);
			currentNum++;
			if(currentNum > 9) {
				break;
			}
			token = COM_Parse(&txtPtr);
		}

		if(currentNum < alertShaders.numShaders || currentNum > alertShaders.numShaders) {
			G_Printf(S_COLOR_RED "ERROR - target_alert: number of blue shaders(%i) does not equal number of green shaders(%i)!\n", currentNum, alertShaders.numShaders);
		}

		currentNum = 0;
	}

	// condition yellow shaders
	if(ent->team != NULL) {
		Q_strncpyz(buffer, ent->team, strlen(ent->team));
		txtPtr = buffer;
		token = COM_Parse(&txtPtr);
		while(qtrue) {
			if(token == NULL || token[0] == 0) { 
				break;
			}
			if(alertShaders.yellowShaders == NULL) {
				return;
			}
			alertShaders.yellowShaders[currentNum] = G_NewString(token);
			currentNum++;
			if(currentNum > 9) {
				break;
			}
			token = COM_Parse(&txtPtr);
		}

		if(currentNum < alertShaders.numShaders || currentNum > alertShaders.numShaders) {
			G_Printf(S_COLOR_RED "ERROR - target_alert: number of yellow shaders(%i) does not equal number of green shaders(%i)!\n", currentNum, alertShaders.numShaders);
		}
	}
}

void SP_target_alert(gentity_t *ent) {
	char		*temp;

	ent->type = ENT_TARGET_ALERT;

	G_SpawnString("greenname", "", &temp);
	ent->swapname = G_NewString(temp);
	G_SpawnString("yellowname", "", &temp);
	ent->truename = G_NewString(temp);
	G_SpawnString("redname", "", &temp);
	ent->falsename = G_NewString(temp);
	G_SpawnString("greentarget", "", &temp);
	ent->truetarget = G_NewString(temp);
	G_SpawnString("yellowtarget", "", &temp);
	ent->falsetarget = G_NewString(temp);
	G_SpawnString("redtarget", "", &temp);
	ent->paintarget = G_NewString(temp);
	G_SpawnString("bluetarget", "", &temp);
	ent->targetname2 = G_NewString(temp);

	if(G_SpawnString("greenshader", "", &temp))
		ent->message = G_NewString(temp);
	if(G_SpawnString("yellowshader", "", &temp))
		ent->team = G_NewString(temp);
	if(G_SpawnString("redshader", "", &temp))
		ent->model = G_NewString(temp);
	if(G_SpawnString("blueshader", "", &temp))
		ent->model2 = G_NewString(temp);

	target_alert_parseShaders(ent);

	if(!ent->swapname || !ent->truename || !ent->falsename || !ent->bluename ||
		!ent->truetarget || !ent->falsetarget || !ent->paintarget || !ent->targetname2) {
			DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] One or more needed keys for target_alert at %s where not set.\n", vtos(ent->s.origin)););
			return;
	}

	if(ent->wait <= 0.0f) {
		ent->wait = 1000;
	} else {
		ent->wait *= 1000;
	}

	ent->use = target_alert_use;

	ent->damage = 0;

	if((ent->spawnflags & 2) == 0) {
		ent->health = 1;
	}

	// don't need to send this to clients
	ent->r.svFlags &= SVF_NOCLIENT;
	trap_LinkEntity(ent);
}
//RPG-X | GSIO01 | 11/05/2009 | MOD END

//RPG-X | GSIO01 | 19/05/2009 | MOD START
/*QUAKED target_warp (1 0 0) (-8 -8 -8) (8 8 8) START_ON START_EJECTED START_WARP SELF
-----DESCRIPTION-----
An entity that manages warp and warpcore.

Any func_usable using this must have NO_ACTIVATOR flag.
Any target_relay, target_delay, or target_boolean using this must have SELF flag.

-----SPAWNFLAGS-----
1: START_ON - If set, warpcore is on at start
2: START_EJECTED - If set, core is ejected at start
4: START_WARP - ship is on warp at start
8: SELF - use this for any entity that is called by sth. other than it's targetname (e.g. swapmname)

-----KEYS-----
"swapWarp" - targetname to toggle warp
"swapCoreState" - targetname to toggle core on/off state
"swapCoreEject" - targetname to toggle core ejected state
"warpTarget" - target to fire when going to warp
"core" - target core(func_train)
"coreSwap" - target for visibility swap (need SELF-flag for this)
"wait" - time before warp can be toggled again after retrieving the core(seconds)
"greensnd" - target_speaker with warp in sound
"yellowsnd" - target_speaker with warp out sound
"redsnd" - target_speaker with core off sound
"bluesnd" - target_speaker with core on sound
"soundDeactivate" - sound to play if going to warp but core is deactivated/ejected
*/
void target_warp_use(/*@shared@*/ gentity_t *ent, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ gentity_t *activator);

void target_warp_reactivate(/*@shared@*/ gentity_t *ent) {
	ent->use = target_warp_use;
	ent->nextthink = -1;
}

void target_warp_use(/*@shared@*/ gentity_t *ent, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ gentity_t *activator) {
	int i;
	qboolean first = qtrue;
	gentity_t *target;

	if(activator == 0) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_warp_use called with NULL activator!\n"););
		return;
	}

	// swapWarp
	if(Q_stricmp(activator->target, ent->truename) == 0) {
		if(ent->n00bCount != 0) {
			ent->target = ent->truetarget;
			G_UseTargets(ent, activator);
			ent->n00bCount = 0;
			ent->target = ent->yellowsound;
			G_UseTargets(ent, activator);
		}
		for(i = 0; i < MAX_GENTITIES; i++) {
			if(&g_entities[i] == NULL) { 
				continue;
			}

			if((g_entities[i].type != ENT_FUNC_TRAIN) && Q_stricmp(g_entities[i].swapname, ent->bluename) == 0) {
				target = &g_entities[i];
				if(target == NULL) {
					continue;
				}

				if((ent->spawnflags & 4) != 0) {
					target->use(target, ent, ent);
#ifdef G_LUA
					if(target->luaUse != NULL) {
						LuaHook_G_EntityUse(target->luaUse, target-g_entities, ent-g_entities, ent-g_entities);
					}
#endif
				} else {
					target->use(target, ent, activator);
#ifdef G_LUA
					if(target->luaUse != NULL) {
						LuaHook_G_EntityUse(target->luaUse, target-g_entities, ent-g_entities, activator-g_entities);
					}
#endif
				}
			} else if((g_entities[i].type == ENT_FUNC_TRAIN) && Q_stricmp(g_entities[i].swapname, ent->bluename) == 0) {
				target = &g_entities[i];
				if(target == NULL) {
					continue;
				}

				if(target->count == 1) {
					target->s.solid = 0;
					target->r.contents = 0;
					target->clipmask = 0;
					target->r.svFlags |= SVF_NOCLIENT;
					target->s.eFlags |= EF_NODRAW;
					target->count = 0;

					if(first){
						ent->target = ent->redsound;
						G_UseTargets(ent, activator);
						first = qfalse;
					}
				} else {
					target->clipmask = CONTENTS_BODY;
					if(target->model != NULL) {
						trap_SetBrushModel( target, target->model );
					}
					target->r.svFlags &= ~SVF_NOCLIENT;
					target->s.eFlags &= ~EF_NODRAW;
					target->clipmask = 0;
					target->count = 1;

					if(first) {
						ent->target = ent->bluesound;
						G_UseTargets(ent, activator);
						first = qfalse;
					}
				}
			}
		}

		if(ent->sound1to2 != 0) {
			ent->sound1to2 = 0;
		} else {
			ent->sound1to2 = 1;
		}
	} else if(Q_stricmp(activator->target, ent->falsename) == 0) { //eject
		if(ent->n00bCount != 0) {
			ent->target = ent->truetarget;
			G_UseTargets(ent, activator);
			ent->n00bCount = 0;
			ent->target = ent->yellowsound;
			G_UseTargets(ent, activator);
		}

		if(ent->sound2to1 != 0) {
			ent->use = NULL;
			ent->think = target_warp_reactivate;
			ent->nextthink =  level.time + (ent->wait * 1000);
		}

		ent->target = ent->falsetarget;
		G_UseTargets(ent, activator);
		if(ent->sound2to1 != 0) {
			ent->sound2to1 = 0;
		} else {
			ent->sound2to1 = 1;
		}
	} else if(Q_stricmp(activator->target, ent->swapname) == 0) { // toggle warp
		if((ent->sound1to2 != 0) && (ent->sound2to1 == 0)) {
			ent->target = ent->truetarget;
			G_UseTargets(ent, activator);

			if(ent->n00bCount != 0) {
				ent->target = ent->yellowsound;
			} else {
				ent->target = ent->greensound;
			}

			G_UseTargets(ent, activator);
			
			if(ent->n00bCount != 0) {
				ent->n00bCount = 0;
			} else {
				ent->n00bCount = 1;
			}
		} else {
			if(ent->soundPos1 != 0) {
				G_AddEvent(ent, EV_GENERAL_SOUND, ent->soundPos1);
			}
		}
	}
}

void SP_target_warp(gentity_t *ent) {
	char *temp;

	ent->type = ENT_TARGET_WARP;

	G_SpawnString("swapWarp", "", &temp);
	ent->swapname = G_NewString(temp);
	G_SpawnString("swapCoreState", "", &temp);
	ent->truename = G_NewString(temp);
	G_SpawnString("swapCoreEject", "", &temp);
	ent->falsename = G_NewString(temp);
	G_SpawnString("warpTarget", "", &temp);
	ent->truetarget = G_NewString(temp);
	G_SpawnString("core", "", &temp);
	ent->falsetarget = G_NewString(temp);
	G_SpawnString("coreSwap", "", &temp);
	ent->bluename = G_NewString(temp);
	G_SpawnString("soundDeactivate", "100", &temp);
	ent->soundPos1 = G_SoundIndex(temp);

	//set corestate
	ent->sound1to2 = (ent->spawnflags & 1);

	//set ejected state
	ent->sound2to1 = (ent->spawnflags & 2);

	//set warpstate
	ent->n00bCount = (ent->spawnflags & 4);

	ent->use = target_warp_use;

	// don't need to send this to clients
	ent->r.svFlags &= SVF_NOCLIENT;
	trap_LinkEntity(ent);
}

//RPG-X | GSIO01 | 19/05/2009 | MOD END
/*QUAKED target_deactivate (1 0 0) (-8 -8 -8) (8 8 8)
-----DESCRIPTION-----
This entity can be used to de/activate all func_usables with "target" as targetname2.

-----SPAWNFLAGS-----
none

-----KEYS-----
"target" - func_usable to de/activate(targetname2).
*/
void target_deactivate_use(/*@shared@*/ gentity_t *ent, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ /*@unused@*/ gentity_t *activator) {
	gentity_t *target = NULL;

	if(ent->target == NULL) {
		return;
	}

	while((target = G_Find(target, FOFS(targetname2), ent->target)) != NULL) {
		if(target->type == ENT_FUNC_USABLE) {
			target->flags ^= FL_LOCKED;
		}
	}
}

void SP_target_deactivate(/*@shared@*/ gentity_t *ent) {
	ent->type = ENT_TARGET_DEACTIVATE;

	if(ent->target == NULL) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_deactivate at %s without target!\n", vtos(ent->r.currentOrigin)););
		return;
	}

	ent->use = target_deactivate_use;

	// don't need to send this to clients
	ent->r.svFlags &= SVF_NOCLIENT;
	trap_LinkEntity(ent);
}

/*QUAKED target_serverchange (1 0 0) (-8 -8 -8) (8 8 8) START_ON
-----DESCRIPTION-----
This will make any client inside it connect to a different server.
Can be toggled by an usable if the usable has NO_ACTIVATOR spawnflag.

-----SPAWNFLAGS-----
1: START_ON - will allow transfer form spawn on.

-----KEYS-----
"serverNum" - server to connect to (rpg_server<serverNum> cvar)
*/
void target_serverchange_think(/*@shared@*/ gentity_t *ent) {
	
	if(ent->touched == NULL || ent->touched->client == NULL || ent->targetname2 == NULL) {
		return;
	}

	trap_SendServerCommand(ent->touched->client->ps.clientNum, va("cg_connect \"%s\"\n", ent->targetname2));
	ent->nextthink = -1;
}

void target_serverchange_use(/*@shared@*/ gentity_t *ent, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ gentity_t *activator) {
	
	if(activator == NULL || activator->client == NULL) {
		if(ent->s.time2 != 0) {
			ent->s.time2 = 0;
		} else {
			ent->s.time2 = 1;
		}
		return;
	}

	if((activator->flags & FL_LOCKED) != 0) {
		return;
	}
	activator->flags ^= FL_LOCKED;

	if(rpg_serverchange.integer != 0 && ent->s.time2 != 0) {
		ent->think = target_serverchange_think;
		ent->nextthink = level.time + 3000;
		TransDat[activator->client->ps.clientNum].beamTime = level.time + 8000;
		activator->client->ps.powerups[PW_BEAM_OUT] = level.time + 8000;
		ent->touched = activator;
		ent->targetname2 = level.srvChangeData.ip[ent->count];
	}
}

void SP_target_serverchange(/*@shared@*/ gentity_t *ent) {
	int serverNum = 0;

	ent->type = ENT_TARGET_SERVERCHANGE;

	G_SpawnInt("serverNum", "1", &serverNum);
	ent->count = serverNum;

	if(ent->count == 0) {
		ent->count = 1;
	}

	if((ent->spawnflags & 1) != 0) {
		ent->s.time2 = 1;
	}

	ent->use = target_serverchange_use;
	trap_LinkEntity(ent);
}

/*QUAKED target_levelchange (1 0 0) (-8 -8 -8) (8 8 8) 
-----DESCRIPTION-----
This will change the map if rpg_allowSPLevelChange is set to 1.

-----SPAWNFLAGS-----
none

-----KEYS-----
"target" - map to load (for example: borg2)
"wait" - time to wait before levelchange (whole numbers only, -1 for instant levelchange, 0 for default = 5)
*/
void target_levelchange_think(/*@shared@*/ gentity_t *ent) {
	if(ent->target == NULL) {
		return;
	}

	if(ent->count > 0) {
		ent->count--;
		trap_SendServerCommand(-1, va("servercprint \"Mapchange in %i ...\"", ent->count)); 
	} else {
		trap_SendConsoleCommand(EXEC_APPEND, va("devmap \"%s\"", ent->target));
		ent->nextthink = -1;
		return;
	}
	ent->nextthink = level.time + 1000;
}

void target_levelchange_use(/*@shared@*/ gentity_t *ent, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ /*@unused@*/ gentity_t *activator) {
	if(rpg_allowSPLevelChange.integer != 0) {
		ent->think = target_levelchange_think;
		ent->nextthink = level.time + 1000;

		if(ent->count > 0) { //This is anoying if there's no delay so let's do this only if there is
			trap_SendServerCommand(-1, va("servercprint \"Mapchange in %i ...\"", ent->count)); 
		}
	}
}

void SP_target_levelchange(gentity_t *ent) {
	ent->type = ENT_TARGET_LEVELCHANGE;

	if(ent->target == NULL) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_levelchange without target at %s!\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}

	if(ent->wait <= 0.0f) {
		ent->count = 5;
	} else if(ent->wait < -1.0f) {
		ent->count = -1;
	} else {
		ent->count = (int)ent->wait;
	}

	ent->use = target_levelchange_use;
}

/*QUAKED target_holodeck (1 0 0) (-8 -8 -8) (8 8 8)
-----DESCRIPTION-----
CURRENTLY DISABLED
target for ui_holodeck

-----SPAWNFLAGS-----
none

-----KEYS-----
none
*/

void SP_target_holodeck(/*@shared@*/ gentity_t *ent) {
#if 1
	G_FreeEntity(ent);
	return;
#else 
	ent->type = ENT_TARGET_HOLODECK;

	// don't need to send this to clients
	ent->r.svFlags &= SVF_NOCLIENT;
	trap_LinkEntity(ent);
#endif
}

//RPG-X | Harry Young | 15/10/2011 | MOD START
/*QUAKED target_shaderremap (1 0 0) (-8 -8 -8) (8 8 8)
-----DESCRIPTION-----
This will remap the shader "falsename" with shader "truename" and vice versa.
It will save you some vfx-usables.

This Entity only works on RPGXEF

-----SPAWNFLAGS-----
none

------KEYS-----
"targetname" - when used will toggle the shaders
"falsename" - shader taht is ingame at spawn
"truename" - shader that will replace it
*/
void target_shaderremap_think(/*@shared@*/ gentity_t *ent) {
	float f = 0.0f;

	if(ent->falsename == NULL || ent->truename == NULL) {
		return;
	}

	if(ent->spawnflags == 0) {
		f = (float)(level.time) * 0.001f;
		AddRemap(ent->falsename, ent->truename, f);
		ent->spawnflags = 1;
		ent->nextthink = -1;
	} else {
		f = (float)(level.time) * 0.001f;
		AddRemap(ent->falsename, ent->falsename, f);
		ent->spawnflags = 0;
		ent->nextthink = -1;
	}

	trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
}

void target_shaderremap_use(/*@shared@*/ gentity_t *ent, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ /*@unused@*/ gentity_t *activator) {
	ent->think = target_shaderremap_think;
	ent->nextthink = level.time + FRAMETIME; /* level.time + one frame */
}

void SP_target_shaderremap(gentity_t *ent) {

	ent->type = ENT_TARGET_SHADERREMAP;

	if(ent->falsename == NULL) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_shaderremap without falsename-shader at %s!\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}

	if(ent->truename == NULL) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_shaderremap without truename-shader at %s!\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}

	ent->use = target_shaderremap_use;
}
//RPG-X | Harry Young | 15/10/2011 | MOD END

//RPG-X | Harry Young | 25/07/2012 | MOD START 
/*QUAKED target_selfdestruct (1 0 0) (-8 -8 -8) (8 8 8) AUDIO_ON
-----DESCRIPTION-----
DO NOT USE! This just sits here purely for documantation.
This entity manages the self destruct.
For now this should only be used via the selfdestruct console command, however it might be usable from within the radiant at a later date.
Should this thing hit 0 the killing part for everyone outside a target_zone configured as safezone will be done automatically.

-----SPAWNFLAGS-----
1: AUDIO_ON - tells the script to display the countdown

-----KEYS-----
"wait" -  total Countdown-Time in secs
"flags" - are audio warnings 1 or 0?
"bluename" - target_zone this thing affects (multi-ship-maps only) will switch it unsafe at T-50ms
"target" - Things like fx to fire once the countdown hits 0

"damage" - leveltime of countdowns end
*/

void target_selfdestruct_end(/*@shared@*/ gentity_t *ent) {
	G_FreeEntity(ent);
	return;
}

void target_selfdestruct_use(/*@shared@*/ gentity_t *ent, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ /*@unused@*/ gentity_t *activator) {
	if( ent->damage - level.time > 50 ){ //I'm still sceptical about a few things here, so I'll leave this in place
		//with the use-function we're going to init aborts in a fairly simple manner: Fire warning notes...
		trap_SendServerCommand( -1, va("servermsg \"Self Destruct sequence aborted.\""));
		G_AddEvent(ent, EV_GLOBAL_SOUND, G_SoundIndex("sound/voice/selfdestruct/abort.mp3"));
		trap_SendServerCommand(-1, va("selfdestructupdate %i", -1));
		//...and arrange for a thnk to and in 50 ms
		ent->think = target_selfdestruct_end;
		ent->nextthink = level.time + 50;
	}
	return;
}

void target_selfdestruct_think(gentity_t *ent) {
	gentity_t	*client = NULL;	
	gentity_t   *healthEnt, *safezone=NULL;	
	int			entlist[MAX_GENTITIES];
	int			n = 0, num;

		//I've reconsidered. Selfdestruct will fire it's death mode no matter what. Targets are for FX-Stuff.
		healthEnt = G_Find(NULL, FOFS(classname), "target_shiphealth");
		if(healthEnt != NULL && G_Find(healthEnt, FOFS(classname), "target_shiphealth") == NULL ){
			healthEnt->damage = healthEnt->health + healthEnt->splashRadius; //let's use the healthent killfunc if we have just one. makes a lot of stuff easier.
			healthEnt->use(healthEnt, NULL, NULL);
		}else{
			while ((safezone = G_Find( safezone, FOFS( classname ), "target_zone" )) != NULL  ){
				if(Q_stricmp(safezone->targetname, ent->bluename) == 0)
					safezone->n00bCount = 0;
			}
			safezone = NULL;
			while ((safezone = G_Find( safezone, FOFS( classname ), "target_zone" )) != NULL  ){
				// go through all safe zones and tag all safe players
				if(safezone->count == 1 && safezone->n00bCount == 1 && Q_stricmp(safezone->targetname, ent->bluename) != 0) {
					memset(entlist, 0, sizeof(entlist));
					num = trap_EntitiesInBox(safezone->r.mins, safezone->r.maxs, entlist, MAX_GENTITIES);
					for(n = 0; n < num; n++) {
						if(entlist[n] < g_maxclients.integer && g_entities[entlist[n]].client != NULL) {
							while((client = G_Find( client, FOFS( classname ), "player" ))!= NULL){
								if(client->s.number == entlist[n])
									client->client->nokilli = 1;
								trap_SendServerCommand( -1, va("print \"SETTING: %i = %i\n\" ", client->s.number, client->client->nokilli) );
							}
						}
					}
				}
			}

			client = NULL;

			//Loop trough all clients on the server.
			while((client = G_Find( client, FOFS( classname ), "player" ))!= NULL){
				if (client->client->nokilli != 1)
					G_Combat_Damage (client, ent, ent, 0, 0, 999999, 0, MOD_TRIGGER_HURT); //maybe a new message ala "[Charname] did not abandon ship."
			}
			//we may go this way once more so clear clients back.
			client = NULL;
			while((client = G_Find( client, FOFS( classname ), "player" ))){
				client->client->nokilli = 0;
			}
			//let's hear it
			G_AddEvent(ent, EV_GLOBAL_SOUND, G_SoundIndex("sound/weapons/explosions/explode2.wav"));
			//let's be shakey for a sec... I hope lol ^^
			trap_SetConfigstring( CS_CAMERA_SHAKE, va( "%i %i", 9999, ( 1000 + ( level.time - level.startTime ) ) ) );
		}
		if(ent->target != NULL) {
			G_UseTargets(ent, ent);
		}

		trap_SendServerCommand(-1, va("selfdestructupdate %i", -1));
		G_FreeEntity(ent);
		return;
}

void SP_target_selfdestruct(gentity_t *ent) {
	double		ETAmin, ETAsec;
	float		temp;

	ent->type = ENT_TARGET_SELFDESTRUCT;

	if(level.time < 1000.0f){ //failsafe in case someone spawned this in the radiant
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_selfdestruct spawned by level. Removing entity."););
		G_FreeEntity(ent);
		return;
	}
	//There is also a failsafe for lua spawning in lua_entity.c ->callspawn

	//There's a little bit of math to do here so let's do that.
	//convert all times from secs to millisecs if that hasn't been done in an earlier pass.

	if (ent->splashRadius == 0){
		temp = ent->wait * 1000;
		ent->wait = temp;
		temp = ent->count * 1000;
		ent->count = temp;
		temp = ent->n00bCount * 1000;
		ent->n00bCount = temp;
		temp = ent->health * 1000;
		ent->health = temp;
		ent->splashRadius = 1;
	}

	//we' may need the total for something so back it up...
	ent->splashDamage = ent->wait;

	//let's find out when this thing will hit hard
	ent->damage = ent->wait + level.time;

	//time's set so let's let everyone know that we're counting. I'll need to do a language switch here sometime...
	ETAsec = floor(modf((ent->wait / 60000), &ETAmin)*60);
	if(ent->spawnflags == 1) {
		if(ETAsec / 10.0f < 1.0f) { //get leading 0 for secs
			trap_SendServerCommand( -1, va("servermsg \"^1Self Destruct in %.0f:0%.0f\"", ETAmin, ETAsec ));
		} else {
			trap_SendServerCommand( -1, va("servermsg \"^1Self Destruct in %.0f:%.0f\"", ETAmin, ETAsec ));
		}
	} else { 
		if(ETAsec / 10.0f < 1.0f) { //get leading 0 for secs
			trap_SendServerCommand( -1, va("servermsg \"^1Self Destruct in %.0f:0%.0f; There will be no ^1further audio warnings.\"", ETAmin, ETAsec ));
		} else {
			trap_SendServerCommand( -1, va("servermsg \"^1Self Destruct in %.0f:%.0f; There will be no ^1further audio warnings.\"", ETAmin, ETAsec ));
		}
	}
	ent->r.svFlags |= SVF_BROADCAST;
	trap_LinkEntity(ent);

	//Additionally we have some audio files ready to go in english with automatic german counterparts. Play them as well.
	if ((int)(ent->wait) == 1200000) {
		G_AddEvent(ent, EV_GLOBAL_SOUND, G_SoundIndex("sound/voice/selfdestruct/20-a1.mp3"));
	} else if ((int)(ent->wait) == 900000) {
		if (ent->spawnflags == 1) {
			G_AddEvent(ent, EV_GLOBAL_SOUND, G_SoundIndex("sound/voice/selfdestruct/15-a1.mp3"));
		} else {
			G_AddEvent(ent, EV_GLOBAL_SOUND, G_SoundIndex("sound/voice/selfdestruct/15-a0.mp3"));
		}
	} else if ((int)(ent->wait) == 600000) {
		G_AddEvent(ent, EV_GLOBAL_SOUND, G_SoundIndex("sound/voice/selfdestruct/10-a1.mp3"));
	} else if ((int)(ent->wait) == 300000) {
		if (ent->spawnflags == 1) {
			G_AddEvent(ent, EV_GLOBAL_SOUND, G_SoundIndex("sound/voice/selfdestruct/5-a1.mp3"));
		} else {
			G_AddEvent(ent, EV_GLOBAL_SOUND, G_SoundIndex("sound/voice/selfdestruct/5-a0.mp3"));
		}
	} else {
		if (ent->spawnflags == 1) {
			G_AddEvent(ent, EV_GLOBAL_SOUND, G_SoundIndex("sound/voice/selfdestruct/X-a1.mp3"));
		} else {
			G_AddEvent(ent, EV_GLOBAL_SOUND, G_SoundIndex("sound/voice/selfdestruct/X-a0.mp3"));
		}
	}

	// Now all that's left is to plan the next think.

	ent->use = target_selfdestruct_use;
	ent->think = target_selfdestruct_think;
	ent->nextthink = ent->damage;

	if(ent->spawnflags == 1) {
		trap_SendServerCommand(-1, va("selfdestructupdate %.0f", ent->wait));
	}

	ent->wait = 0.0f;

	trap_LinkEntity(ent);
}

/*QUAKED target_zone (1 0 0) ? SPAWN_SAFE SHIP
-----DESCRIPTION-----
A generic zone used for entities that need a generic pointer in the world. It needs to be specialized by the team-value.

-----SPAWNFLAGS-----
Note: Spawnflags will only work with the system they are attached to
1: SPAWN_SAFE - For safezone only: Entity is spawned in it's safe configurartion
2: SHIP - For safezone only: will mark this safezone as a ship safezone

-----KEYS-----
"targetname" - used to link with, some types require this for toggling
"count" - specifies this zone's type:

	0 - none, will free entity
	1 - safezone for target_selfdestruct and target_shiphealth
	2 - display zone for target_shiphealth (HUD overlay)
	4 - sound zone

-----USAGE-----
As safezone:
Usage for Escape Pods and similar:
Fill your escape pod Interior with this trigger and have it targeted by a func_usable/target_relay/target_delay to toggle it between safe and unsafe states.

Usage for multiple ships (and stations) like on rpg_runabout:
Surround your entire ship with this trigger (or it's seperate elements with one each) and set it to STARTON and SHIP (spawnflags = 3). 
Have it's targetname match the targetname of it's target_shiphealth-counterpart exactly (case-dependent) to automatically switch this safezone to unsafe should it be about to die.
In case of a selfdestruct you will need to enter the targetname to automatically switch it to unsafe 50ms prior to the countdowns end.
To get the correct one use the /safezonelist-command 
*/

void target_safezone_use(/*@shared@*/ gentity_t *ent, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ /*@unused@*/ gentity_t *activator){
	//a client used this, so let's set this thing to active
	if(ent->n00bCount == 1) {
		ent->n00bCount = 0;
	} else {
		ent->n00bCount = 1;
	}
}

void SP_target_zone(gentity_t *ent) {

	ent->type = ENT_TARGET_ZONE;

	if(ent->targetname == NULL || ent->targetname[0] == 0) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_zone without targetname at %s, removing entity.\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}

	if(Q_stricmp(ent->classname, "target_zone") != 0){
		ent->count = 1;
		ent->classname = "target_zone";
	}
	
	if(ent->count == 0) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_zone without specified class by it's count-value at %s, removing entity.\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}

	if(strcmp(ent->classname, "target_zone") != 0){
		ent->count = 1;
		ent->classname = "target_zone";
	}

	if(ent->luaEntity == qfalse && ent->model != NULL) {
		trap_SetBrushModel(ent, ent->model);
	}

	if(ent->count == 1) {
		ent->use = target_safezone_use;
	}

	if(ent->count == 1 && (ent->spawnflags & 1) != 0) {
		ent->n00bCount = 1;
	}

	ent->r.contents = CONTENTS_NONE;
	ent->r.svFlags |= SVF_NOCLIENT;
	trap_LinkEntity(ent);
}

/*QUAKED target_shiphealth (1 0 0) (-8 -8 -8) (8 8 8)
-----DESCRIPTION-----
This Entity manages a ships health. Ship Health is reduced via administrative/delegable console command "/shipdamage [damage]"
Repairing is based on a % per minute basis for both shields and hull.
The entity features interconnectivity with other systems such as warpdrive or turbolift with a random yet incresing chance to turn them off whenever hulldamage occurs. This includes Shields.
Further more the entity will automatically toggle red alert should it be any other and will activate shields if alert is set to any but green.
If hull health hit's 0 it will kill any client outside an active safezone.

-----SPAWNFLAGS-----
none

-----KEYS-----
Required Keys (If any of them are not given the entity will be removed at spawn):
targetname: Name of the Ship/Station this entity represents. See target_zone for additional use of this key.
health: Total Hull strength
splashRadius: total shield strenght
angle: Hull repair in % per minute
speed: Shield repair in % per minute (only active if shield's aren't fried)

greensound: Things to fire every time damage occurs (like FX)
falsetarget: truename/swapCoreState for target_warp
bluename: swapname for target_turbolift
bluesound: swapname for ui_transporter
falsename: falsename/redname for target_alert

paintarget: target_zones configured as MSD-Display-Zones this thing shoud communicate with

"model" - path to a shader with a MSD-Display (ship) to show. Default will be the Daedalus Class

We're sponsoring a varayity, which were created by Alexander Richardson.
The shaders for these are stowed in scripts/msd.shader in the pakX.pk3.
It contains two versions: One for Texturing in Level design (like a display) and opne for the UI.
To retrieve such an image simply look for the MSD-Folder in your radiants texture browser
For personalized MSD's see segment below.

Ship-Classname || Online Source || Shader-Name (for <type> insert gfx for UI-Shader and textures for texture shader)
Constellation Class || http://lcarsgfx.wordpress.com/2012/09/12/constellation-sisyphus/ || <type>/msd/constellation
Danube Runabout || http://lcarsgfx.wordpress.com/2012/06/30/the-blue-danube/ || <type>/msd/runabout
Nova Class || http://lcarsgfx.wordpress.com/2012/06/13/can-you-tell-what-it-is-yet-2/ || <type>/msd/nova
Galaxy Class || http://lcarsgfx.wordpress.com/2012/06/10/galaxy-class-redux-an-update/ || <type>/msd/galaxy
Daedalus Class || http://lcarsgfx.wordpress.com/2011/12/10/daedalus-father-of-icarus/ || <type>/msd/daedalus
Nebula Class || http://lcarsgfx.wordpress.com/2011/12/08/entering-the-nebula-part-2/ || <type>/msd/nebula
Intrepid Class || http://lcarsgfx.wordpress.com/2011/05/16/an-intrepid-undertaking/ || <type>/msd/intrepid
USCM (Alien) || http://lcarsgfx.wordpress.com/2010/08/10/in-space-no-one-can-hear-you-scream/ || <type>/msd/conestoga
Olympic Class || http://lcarsgfx.wordpress.com/2010/09/11/the-olympic-class/ || <type>/msd/olympic
Steamrunner Class || http://lcarsgfx.wordpress.com/2010/08/15/full-steam-ahead/ || <type>/msd/steamrunner
Oberth Class || http://lcarsgfx.wordpress.com/2010/08/12/im-a-doctor-not-a-science-vessel/ || <type>/msd/oberth
Soverign Class || http://lcarsgfx.wordpress.com/2010/03/01/sovereign-of-the-stars/ || <type>/msd/soverign
Excelsior Class (Retro Design) || http://lcarsgfx.wordpress.com/2010/01/01/retro-excelsior/ || <type>/msd/excelsior-retro
Excelsior Class || http://lcarsgfx.wordpress.com/2009/12/28/excelsior-class/ || <type>/msd/excelsior
Springfield Class || http://lcarsgfx.wordpress.com/2009/12/25/not-the-springfield-from-the-simpsons/ || <type>/msd/springfield
Defiant Class (8 Decks) || http://lcarsgfx.wordpress.com/2009/12/10/scaling-the-defiant/ || <type>/msd/defiant8
Defiant Class (4 Decks) || http://lcarsgfx.wordpress.com/2009/12/06/the-face-of-defiance/ || <type>/msd/defiant4
Miranda Class || http://lcarsgfx.wordpress.com/2009/12/05/miranda/ || <type>/msd/miranda
Centaur Class || http://lcarsgfx.wordpress.com/2009/12/05/centaur-comes-galloping/ ||  <type>/msd/centaur
Constitution Class || http://lcarsgfx.wordpress.com/2009/11/28/its-a-constitution/ || <type>/msd/constitution
Ambassador Class || http://lcarsgfx.wordpress.com/2009/11/27/having-the-ambassador-round-for-dinner/ || <type>/msd/ambassador
Cern Class || http://lcarsgfx.wordpress.com/2009/11/23/cern-class-by-john-eaves/ || <type>/msd/cern
Akira Class || http://lcarsgfx.wordpress.com/2009/11/21/akira-ra-ra-ra/ || <type>/msd/akira
Norway Class || http://lcarsgfx.wordpress.com/2009/11/21/norway-or-no-way/ || <type>/msd/norway
New Orleans Class || http://lcarsgfx.wordpress.com/2009/11/16/the-new-orleans/ || <type>/msd/neworleans
Cheyenne Class || http://lcarsgfx.wordpress.com/2009/11/16/cheyenne-class-msd/ || <type>/msd/cheyenne
Sabre Class || http://lcarsgfx.wordpress.com/2009/11/07/sabre-rattling/ || <type>/msd/sabre

-----Personalized MSD's-----
Alexander is doing personalized variations og his MSD's in terms of Ship-Names and Registry-Numbers
(as long as they do not have suffix-letters). If you'd like one you may contact him via E-Mail and request such a modification.
In that request please also ask hom for a resulution of 2000px across (long side) as the game requires
images to be displayed as MSD's to be roughly 2:1 and 2000 px is near the upper limit of what the game can handle.
Also please ask him to give you the image as an *.jpg-file.

Once you have the file put it in a subfolder (e.g. gfx) in either baseEF or RPG-X2.
After that create a scripts/msd_shipname_registry.shader file (registry is optional, 
however it is useful in avoiding collitions with ships of similar names)
In that file add the following short script: 

gfx/msd/akira //this will be the path to the image for the UI
{
	{
		map textures/msd/akira.jpg //this will be the image you will use
		blendFunc add //this will remove the black background. I might find a better solution...
	}
}

textures/msd/akira //this will be the image you will use for texturing
{
	surfaceparm nolightmap
	surfaceparm nomarks
	{
		map textures/msd/akira.jpg //this will be the image you will use
	}
	{
		map textures/engineering/glass1.tga //this segment creates the glass effect to make it look like a display
		blendfunc gl_one gl_one_minus_src_color
		rgbGen identity
		tcMod scale 3 3
		tcGen environment
	}
}

For distribution put both files (including their relative paths) in a *.pk3 file.
*/

void target_shiphealth_die(/*@shared@*/ gentity_t *ent){
	//we're dying
	int			n = 0, num;
	int			entlist[MAX_GENTITIES];
	gentity_t	*client = NULL, *safezone=NULL;

	while ((safezone = G_Find( safezone, FOFS( classname ), "target_zone" )) != NULL  ){
		if(Q_stricmp(safezone->targetname, ent->targetname) == 0)
			safezone->n00bCount = 0;
	}
	safezone = NULL;
	while ((safezone = G_Find( safezone, FOFS( classname ), "target_zone" )) != NULL  ){
		// go through all safe zones and tag all safe players
		if(safezone->count == 1 && safezone->n00bCount == 1 && Q_stricmp(safezone->targetname, ent->bluename) != 0) {
			num = trap_EntitiesInBox(safezone->r.mins, safezone->r.maxs, entlist, MAX_GENTITIES);
			for(n = 0; n < num; n++) {
				if(entlist[n] < g_maxclients.integer && g_entities[entlist[n]].client) {
					while((client = G_Find( client, FOFS( classname ), "player" )) != NULL){
						if(client->s.number == entlist[n])
							client->client->nokilli = 1;
					}
				}
			}
		}
	}

	client = NULL;

	//Loop trough all clients on the server.
	while((client = G_Find( client, FOFS( classname ), "player" )) != NULL){
		if (client->client && client->client->nokilli != 1) {
			G_Combat_Damage (client, ent, ent, 0, 0, 999999, 0, MOD_TRIGGER_HURT); //maybe a new message ala "[Charname] did not abandon ship."
		}
	}
	//we may go this way once more so clear clients back.
	client = NULL;
	while((client = G_Find( client, FOFS( classname ), "player" )) != NULL) {
		if(client->client != NULL) {
			client->client->nokilli = 0;
		}
	}
	//let's hear it
	G_AddEvent(ent, EV_GLOBAL_SOUND, G_SoundIndex("sound/weapons/explosions/explode2.wav"));
	//let's be shakey for a sec... I hope lol ^^
	trap_SetConfigstring( CS_CAMERA_SHAKE, va( "%i %i", 9999, ( 1000 + ( level.time - level.startTime ) ) ) );
	ent->count = 0;
	ent->nextthink = -1;
}

void target_shiphealth_use(/*@shared@*/ gentity_t *ent, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ /*@unused@*/ gentity_t *activator) {
	double		NSS, NHS, SD, HD, BT;
	int			n = 0, num;
	int			entlist[MAX_GENTITIES];
	gentity_t	*alertEnt, *warpEnt, *turboEnt, *transEnt, *msdzone=NULL, *client=NULL;

	if(ent->damage <= 0) { //failsave
		return;
	} else {
		if(ent->splashDamage == 1) { //shields are active so we're just bleeding trough on the hull
			BT = ((1.0 - (ent->count * pow(ent->health, -1))) / 10.0);
			SD = (ent->damage - ceil(ent->damage * BT));

			if(SD > ent->n00bCount) { //we're draining the shields...
				HD = (ent->damage - ent->n00bCount);
				NHS = (ent->count - HD);
				ent->n00bCount = 0;
				ent->splashDamage = -2;
			} else { //shields will survive so let's just bleed trough
				HD = floor(ent->damage * BT);
				NHS = (ent->count - HD);
				NSS = (ent->n00bCount - SD);
				ent->n00bCount = NSS;
			}
		} else { //shields are off, guess where the blow goes...
			NHS = (ent->count - ent->damage);
		}
	ent->count = NHS;
	ent->damage = 0;
	}
	
	//enough math, let's trigger things

	//go to red alert if we are not, this will also activate the shields
	if(ent->falsename != NULL) {
		alertEnt = G_Find(NULL, FOFS(falsename), ent->falsename);
		if(alertEnt->damage != 2) {
			ent->target = ent->falsename;
			G_UseTargets(ent, ent);
		}
	} else {
		if(ent->splashDamage == 0) {
			ent->splashDamage = 1;
		}
	}

	//time to fire the FX
	ent->target = ent->greensound;
	G_UseTargets(ent, ent);

	//disable UI_Transporter if need be.
	if(ent->bluesound != NULL){
		transEnt = G_Find(NULL, FOFS(swapname), ent->bluesound);
		if ((transEnt->flags & FL_LOCKED) == 0){
			if((ent->count * pow(ent->health, -1)) < flrandom(0.1 , 0.4)){
				ent->target = ent->bluesound;
				G_UseTargets(ent, ent);
			}
		}
	}

	//disable target_turbolift if need be.
	if(ent->bluename != NULL){
		turboEnt = G_Find(NULL, FOFS(swapname), ent->bluename);
		if ((turboEnt->flags & FL_LOCKED) == 0){
			if((ent->count * pow(ent->health, -1)) < flrandom(0.1 , 0.4)){
				ent->target = ent->bluename;
				G_UseTargets(ent, ent);
			}
		}
	}

	//disable target_warp if need be.
	if(ent->falsetarget != NULL){
		warpEnt = G_Find(NULL, FOFS(truename), ent->falsetarget);
		if ((warpEnt->sound1to2 != 0) && (warpEnt->sound2to1 == 0)) {
			if((ent->count * pow(ent->health, -1)) < flrandom(0.1 , 0.4)) {
				ent->target = ent->falsetarget;
				G_UseTargets(ent, ent);
			}
		}
	}

	//disable shield-subsystem if need be.
	if((ent->count * pow(ent->health, -1)) < flrandom(0.1 , 0.4)) {
		ent->n00bCount = 0;
		ent->splashDamage = -1;
	}

	//let's reset the repair-timer
	ent->nextthink = level.time + 60000;

	//refresh clients HUD Display
	//first zero out all clients that are connected to this one
	while((client = G_Find(client, FOFS(classname), "player")) != NULL){
		if(client->client != NULL && client->client->myship == ent->s.number)
			trap_SendServerCommand( client->s.number, va("shiphealthupdate 0 0 0 "));
	}

	client = NULL;

	//now let's loop trough our zones and find the clients to send the info to
	while ((msdzone = G_Find( msdzone, FOFS( classname ), "target_zone" )) != NULL  ){
		// go through all safe zones and tag all safe players
		if(msdzone->count == 2 && Q_stricmp(msdzone->targetname, ent->paintarget)) {
			num = trap_EntitiesInBox(msdzone->r.mins, msdzone->r.maxs, entlist, MAX_GENTITIES);
			for(n = 0; n < num; n++) {
				if(entlist[n] < g_maxclients.integer && g_entities[entlist[n]].client) {
					while((client = G_Find( client, FOFS( classname ), "player" ))!= NULL){
						if(client->s.number == entlist[n]){
							trap_SendServerCommand( client->s.number, va("shiphealthupdate %.0f %.0f %i ", floor(ent->count / ent->health), floor(ent->n00bCount / ent->splashRadius), ent->splashDamage));
							client->client->myship = ent->s.number;
						}
					}
				}
			}
		}
	}

	//if we hit 0 blow in 50 ms 
	if(ent->count <= 0){
		ent->think = target_shiphealth_die;
		ent->nextthink = level.time + 50;
	}

	return;
}	

void target_shiphealth_think(/*@shared@*/ gentity_t *ent) {
	//this will do the healing each minute
	int			NSS, NHS;
	int			n = 0, num;
	int			entlist[MAX_GENTITIES];
	gentity_t*	alertEnt;
	gentity_t	*msdzone=NULL, *client=NULL;

	//We have interconnectivity with target_alert here in that at condition green we regenerate twice as fast
	//so let's find the entity
	if(ent->falsename != NULL) {
		alertEnt = G_Find(NULL, FOFS(falsename), ent->falsename);
	} else {
		alertEnt = G_Find(NULL, FOFS(classname), "target_alert");
	}

	if(alertEnt == NULL){ //failsave in case we don't have a target_alert present
		alertEnt = G_Spawn();
		alertEnt->damage = 0;
	}

	// Hull Repair
	if(ent->count < ent->health){
		if(alertEnt->damage == 0) { //condition green
			NHS = (ent->count + (ent->health * ent->angle / 100));
		} else {
			NHS = (ent->count + (ent->health * ent->angle / 200));
		}

		if(NHS > ent->health) {
			ent->count = ent->health;
		} else {
			ent->count = NHS;
		}
	}

	// Shield Repair
	if(ent->splashDamage != -1){ //skip if shields are toast
		if(ent->n00bCount < ent->splashRadius){
			if(alertEnt != NULL && alertEnt->damage == 0) { //condition green
				NSS = (ent->n00bCount + (ent->splashRadius * ent->speed / 100));
				ent->splashDamage = 0;
			} else {
				NSS = (ent->n00bCount + (ent->splashRadius * ent->speed / 200));
				ent->splashDamage = 1;
			}

			if(NSS > ent->splashRadius)
				ent->n00bCount = ent->splashRadius;
			else
				ent->n00bCount = NSS;
		}
	}

	//shield reenstatement
	if(ent->splashDamage == -1) { //else we don't need to run this
		if((ent->count * pow(ent->health, -1)) > 0.5) {
			if(alertEnt != NULL && alertEnt->damage == 0 && (alertEnt->type == ENT_TARGET_ALERT)) {
				ent->splashDamage = 0;
			} else {
				ent->splashDamage = 1;
			}
		} else {
			if((ent->count * pow(ent->health, -1) * flrandom(0, 1)) > 0.75){
				if(alertEnt != NULL && alertEnt->damage == 0 && (alertEnt->type == ENT_TARGET_ALERT)) {
					ent->splashDamage = 0;
				} else {
					ent->splashDamage = 1;
				}
			}
		}
	}
	ent->nextthink = level.time + 60000;

	//refresh clients HUD Display
	//first zero out all clients that are connected to this one
	while((client = G_Find(client, FOFS(classname), "player")) != NULL){
		if(client->client != NULL && client->client->myship == ent->s.number)
			trap_SendServerCommand( client->s.number, va("shiphealthupdate 0 0 0 "));
	}

	client = NULL;

	//now let's loop trough our zones and find the clients to send the info to
	while ((msdzone = G_Find( msdzone, FOFS( classname ), "target_zone" )) != NULL  ) {
		// go through all safe zones and tag all safe players
		if(msdzone->count == 2 && Q_stricmp(msdzone->targetname, ent->paintarget)) {
			num = trap_EntitiesInBox(msdzone->r.mins, msdzone->r.maxs, entlist, MAX_GENTITIES);
			for(n = 0; n < num; n++) {
				if(entlist[n] < g_maxclients.integer && g_entities[entlist[n]].client) {
					while((client = G_Find( client, FOFS( classname ), "player" ))!= NULL){
						if(client->s.number == entlist[n]){
							trap_SendServerCommand( client->s.number, va("shiphealthupdate %.0f %.0f %i ", floor(ent->count / ent->health), floor(ent->n00bCount / ent->splashRadius), ent->splashDamage));
							client->client->myship = ent->s.number;
						}
					}
				}
			}
		}
	}

	return;
}


void SP_target_shiphealth(gentity_t *ent) {
	ent->type = ENT_TARGET_SHIPHEALTH;

	if(ent->targetname == NULL || ent->health == 0 || ent->splashRadius == 0 || !ent->angle || !ent->speed){
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_shiphealth at %s is missing one or more parameters, removing entity.\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}

	//we need to put the total health in for the current
	ent->count = ent->health;
	ent->n00bCount = ent->splashRadius;

	//now for the shieldindicator I need to know if we have an alertEnt available
	if(G_Find(NULL, FOFS(classname), "target_alert")) {
		ent->splashDamage = 0;
	} else {
		ent->splashDamage = 1;
	}

	//let's make sure we have something to return as model
	if(ent->model == NULL)
		ent->model = "gfx/msd/daedalus";

	ent->think = target_shiphealth_think;
	ent->use = target_shiphealth_use;
	ent->nextthink = level.time + 60000;

	trap_LinkEntity(ent);
}

/*QUAKED target_sequence (1 0 0) (-8 -8 -8) (8 8 8) NO_INIT_DELAY DISALLOW_ABORTS LOOP AUTO_INIT FREE_AFTER_SEQUENCE ABORT_AFTER_SEQUENCE
-----DESCRIPTION-----
A sequence manager, mostly to kill off multiple target_delays.
Will fire specified targets in a fixed order.
The time intervall between each firing can be specified.
If an intervall is 0 (or specifically not > 0) the sequence is ended.
The sequence is also ended once the entity is used a 2nd time (unless disallowed).
If the sequence is ended it does not pick up at it's last point but rather at it's start mark
Entities are always fired as noactivator.

-----SPAWNFLAGS-----
1: NO_INIT_DELAY - The first target will be fired without any delay (damage will simply not be read).
2: DISALLOW_ABORTS - Disallows aborting the sequence by a 2nd use of the entity.
4: LOOP - Instead of ending after the frst pass it starts back at stage 1.
8: AURO_INIT - starts 1st stage when spawned. Overwrites NO_INIT_DELAY as target entities may not have been spawned.
16: FREE_AFTER_SEQUENCE - removes entity after 1 sequence has either run trough or been aborted. 
32: ABORT_AFTER_SEQUENCE - once an abort is issued the sequence runs trough and comes to a halt, else it halts directly

-----KEYS-----
"targetname" - used to init the sequence
"message" - Message to display when sequence is initialised
"model" - Message to display when sequence is aborted
"model2" - Message to display while in an inabortable sequence

Pos | Target | Delay (in ms, 1 s = 1000 ms)
1 | bluename | count
2 | swapname | health
3 | falsename | splashDamage
4 | falsetarget | splashRadius

-----USAGE-----
For more than 4 iterations:
If you plan a sequence that has more than 5 iterations simply have 
the 5th iteration of entity a target entity b and have entity b
have NO_INIIT_DELAY on.

For looping have the last target_sequence target the first (if you don't want the 
initial init to be instand run it trough a target_delay)

If you want the entire sequence to be inabortable I recomend you use 
the following lua script hooked via luaUse on your usable. Make sure that every 
target_sequence has a unique name on the map:

function sequencecheck (ent, other, activator)
	if ent.GetDamage(entity.Find("targetname(1)")) > 0 then
		if ent.GetDamage(entity.Find("targetname(2)")) > 0 then
			if ent.GetDamage(entity.Find("targetname(n)")) > 0 then
				entity.Use(entity.Find("targetname(1)"));
			else
				game.MessagePrint(activator, "=C= Unable to comply, sequence is already in progress");
			end
		else
			game.MessagePrint(activator, "=C= Unable to comply, sequence is already in progress");
		end
	else
		game.MessagePrint(activator, "=C= Unable to comply, sequence is already in progress");
	end
end
*/

// ent->damage will be used to keep track of the iteration, will transfer as next targetname to fire

void target_sequence_think(/*@shared@*/ gentity_t *ent){

	// we've called a think, so we're in sequence
	// let's figure out what to fire:
	switch(ent->damage) {
	case 1: // bluename | count
		G_UseTargets2(ent, NULL, ent->bluename);
		ent->damage = 2;
		break;
	case 2: // swapname | health
		G_UseTargets2(ent, NULL, ent->swapname);
		ent->damage = 3;
		break;
	case 3: // falsename | splashDamage
		G_UseTargets2(ent, NULL, ent->falsename);
		ent->damage = 4;
		break;
	case 4: // falsetarget | splashRadius
		G_UseTargets2(ent, NULL, ent->falsetarget);
		// end of line, we need to check how to continue concerning looping
		if((ent->spawnflags & 4) == 0 || ent->n00bCount == 1){
			//no looping, 0 out
			ent->damage = 0;
			ent->nextthink = -1;
			ent->n00bCount = 0;
			if((ent->spawnflags & 16) != 0){
				//we have sequenced trough and need to free the entity. Leave a DEV-Note for safety
				DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Info] target_sequence at %s has run trough, removing entity.\n", vtos(ent->s.origin)););
				G_FreeEntity(ent);
			}
			return;
		} else {
			// looping, set up pass 1 delay
			ent->damage = 1;
			ent->nextthink = level.time + ent->count;
			return;
		}
	default:
		//falisafe
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_sequence at %s hit indefined state, please inform ubergames, removing entity.\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}

	// we've fired our targets, now set up for the next pass
	// we have 3 things to check for:
	// 1: is target and delay 0? -> end of sequence
	// 2: is either target or delay 0? -> warn DEV and free
	// 3: is target and delay present (else-checked)? -> set up
	switch(ent->damage) {
	case 1: // bluename | count
		if ( ent->bluename == NULL && ent->count <= 0 ) {
			// end of sequence, do we loop?
			if((ent->spawnflags & 4) == 0 || ent->n00bCount == 1){
				//no looping, 0 out
				ent->damage = 0;
				ent->nextthink = -1;
				ent->n00bCount = 0;
				if((ent->spawnflags & 16) != 0){
					//we have sequenced trough and need to free the entity. Leave a DEV-Note for safety
					DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Info] target_sequence at %s has run trough, removing entity.\n", vtos(ent->s.origin)););
					G_FreeEntity(ent);
				}
				return;
			} else {
				// looping, set up pass 1 delay
				ent->damage = 1;
				ent->nextthink = level.time + ent->count;
				return;
			}
		} else if ( ent->bluename == NULL || ent->count <= 0 ) {
			// one without the other does not work, inform dev and free ent
			DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_sequence at %s is missing bluename or count in stage 1, removing entity.\n", vtos(ent->s.origin)););
			G_FreeEntity(ent);
			return;
		} else { 
			// we have everything we need, so continue for next think
			ent->nextthink = level.time + ent->count;
			return;
		}
	case 2: // swapname | health
		if ( ent->swapname == NULL && ent->health <= 0 ) {
			// end of sequence, do we loop?
			if((ent->spawnflags & 4) == 0 || ent->n00bCount == 1){
				//no looping, 0 out
				ent->damage = 0;
				ent->nextthink = -1;
				ent->n00bCount = 0;
				if((ent->spawnflags & 16) != 0){
					//we have sequenced trough and need to free the entity. Leave a DEV-Note for safety
					DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Info] target_sequence at %s has run trough, removing entity.\n", vtos(ent->s.origin)););
					G_FreeEntity(ent);
				}
				return;
			} else {
				// looping, set up pass 1 delay
				ent->damage = 1;
				ent->nextthink = level.time + ent->count;
				return;
			}
		} else if ( ent->swapname == NULL || ent->health <= 0 ) {
			// one without the other does not work, inform dev and free ent
			DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_sequence at %s is missing swapname or health in stage 2, removing entity.\n", vtos(ent->s.origin)););
			G_FreeEntity(ent);
			return;
		} else { 
			// we have everything we need, so continue for next think
			ent->nextthink = level.time + ent->health;
			return;
		}
	case 3: // falsename | splashDamage
		if ( ent->falsename == NULL && ent->splashDamage <= 0 ) {
			// end of sequence, do we loop?
			if((ent->spawnflags & 4) == 0 || ent->n00bCount == 1){
				//no looping, 0 out
				ent->damage = 0;
				ent->nextthink = -1;
				ent->n00bCount = 0;
				if((ent->spawnflags & 16) != 0){
					//we have sequenced trough and need to free the entity. Leave a DEV-Note for safety
					DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Info] target_sequence at %s has run trough, removing entity.\n", vtos(ent->s.origin)););
					G_FreeEntity(ent);
				}
				return;
			} else {
				// looping, set up pass 1 delay
				ent->damage = 1;
				ent->nextthink = level.time + ent->count;
				return;
			}
		} else if ( ent->falsename == NULL || ent->splashDamage <= 0 ) {
			// one without the other does not work, inform dev and free ent
			DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_sequence at %s is missing falsename or splashDamage in stage 3, removing entity.\n", vtos(ent->s.origin)););
			G_FreeEntity(ent);
			return;
		} else { 
			// we have everything we need, so continue for next think
			ent->nextthink = level.time + ent->splashDamage;
			return;
		}
	case 4: // falsetarget | splashRadius
		if ( ent->falsetarget == NULL && ent->splashRadius <= 0 ) {
			// end of sequence, do we loop?
			if((ent->spawnflags & 4) == 0 || ent->n00bCount == 1){
				//no looping, 0 out
				ent->damage = 0;
				ent->nextthink = -1;
				ent->n00bCount = 0;
				if((ent->spawnflags & 16) != 0){
					//we have sequenced trough and need to free the entity. Leave a DEV-Note for safety
					DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Info] target_sequence at %s has run trough, removing entity.\n", vtos(ent->s.origin)););
					G_FreeEntity(ent);
				}
				return;
			} else {
				// looping, set up pass 1 delay
				ent->damage = 1;
				ent->nextthink = level.time + ent->count;
				return;
			}
		} else if ( ent->falsetarget == NULL || ent->splashRadius <= 0 ) {
			// one without the other does not work, inform dev and free ent
			DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_sequence at %s is missing falsetarget or splashRadius in stage 4, removing entity.\n", vtos(ent->s.origin)););
			G_FreeEntity(ent);
			return;
		} else { 
			// we have everything we need, so continue for next think
			ent->nextthink = level.time + ent->splashRadius;
			return;
		}
	default:
		//falisafe
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_sequence at %s hit indefined state, please inform ubergames, removing entity.\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}
}


void target_sequence_use(/*@shared@*/ gentity_t *ent, /*@shared@*/ /*@unused@*/ gentity_t *other, /*@shared@*/ gentity_t *activator){

	// let's see if we are in sequence
	if (ent->damage == 0){
		//we are not, so let's init the sequence, first print message if there is
		if ( activator != NULL && activator->client != NULL && ent->message != NULL) {
			trap_SendServerCommand( activator-g_entities, va("servermsg %s", ent->message ));
		}
		//do we fire our 1st target imediately or do we delay?
		if((ent->spawnflags & 1) == 0){
			//we delay so let's set up
			ent->nextthink = level.time + ent->count;
			ent->damage = 1;
		} else {
			// we don't delay, so let's fire the targets and set up for stage 2
			ent->target = ent->bluename;
			G_UseTargets(ent, NULL);
			ent->nextthink = level.time + ent->count;
			ent->damage = 2;
		}
	} else {
		// we are in a sequence, so can we abort?
		if((ent->spawnflags & 2) == 0 && ent->n00bCount == 0){
			//we can, so let's abort
			if ( activator != NULL && activator->client != NULL && ent->model != NULL) {
				trap_SendServerCommand( activator-g_entities, va("servermsg %s", ent->model ));
			}
			// do we abort now or later?
			if((ent->spawnflags & 32) == 0){
				ent->nextthink = -1;
				ent->damage = 0;
				if((ent->spawnflags & 16) != 0){
					//we have sequenced trough and need to free the entity. Leave a DEV-Note for safety
					DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Info] target_sequence at %s has run trough, removing entity.\n", vtos(ent->s.origin)););
					G_FreeEntity(ent);
				}
			} else{
				ent->n00bCount = 1;//later
			}
			return;
		} else {
			// we can't, put out info if there is
			if ( activator != NULL && activator->client != NULL && ent->model2 != NULL) {
				trap_SendServerCommand( activator-g_entities, va("servermsg %s", ent->model2 ));
			}
		}
	}
}

void SP_target_sequence(gentity_t *ent) {
	ent->type = ENT_TARGET_SEQUENCE;

	if(ent->targetname == NULL){
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_sequence at %s without targetname, removing entity.\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}

	ent->n00bCount = 0;
	ent->think = target_sequence_think;
	ent->use = target_sequence_use;
	//do we auto-init?
	if ((ent->spawnflags & 8) == 0){
		// we do not, so don't plan a think
		ent->damage = 0;
		ent->nextthink = -1;
	} else {
		// we do so plan think for 1st target time after making sure that we have everything at hand
		if ( ent->bluename == NULL || ent->count <= 0 ) {
			// one without the other does not work, inform dev and free ent
			DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_sequence at %s is missing bluename or count in stage 1, removing entity.\n", vtos(ent->s.origin)););
			G_FreeEntity(ent);
			return;
		}
		ent->damage = 1;
		ent->nextthink = level.time + ent->count;
	}

	trap_LinkEntity(ent);
}