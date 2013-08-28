// This file contains game side effects that the designers can place throughout the maps

#include "g_local.h"
#include "g_spawn.h"
#include "g_logger.h"

#define SPARK_STARTOFF		1
/*QUAKED fx_spark (0 0 1) (-8 -8 -8) (8 8 8) STARTOFF
-----DESCRIPTION-----
Emits sparks at the specified point in the specified direction.

Can be toggled by being used, but use with caution as updates every second instead of every 10 seconds, 
which means it sends 10 times the information that an untoggleable steam will send.

-----SPAWNFLAGS-----
1: STARTOFF - Effect will be off at spawn.

-----KEYS-----
"targetname" - toggles on/off whenever used
"target" - ( optional ) direction to aim the sparks in, otherwise, uses the angles set in the editor.
"wait" - interval between events, default 2000 ms (randomly twice as long) 
*/

//------------------------------------------
static void spark_think( gentity_t *ent )
{
	G_AddEvent( ent, EV_FX_SPARK, 0 );
	if(ent->targetname != NULL && ent->targetname[0] != 0) { //toggleable effect needs to be updated more often
		ent->nextthink = level.time + 1000;
	} else {
		ent->nextthink = level.time + 10000; // send a refresh message every 10 seconds
	}
}

//T3h TiM-zor was here
static void spark_use( gentity_t* self, /*@unused@*/ gentity_t* other, /*@unused@*/ gentity_t* activator) {
	
	if ( self->count != 0) {
		self->think = NULL;
		self->nextthink = -1;
		self->count = 0;
	}
	else {
		self->think = spark_think;	
		self->nextthink = level.time + 10000;
		self->count = 1;
	}
}

//------------------------------------------
static void spark_link( gentity_t *ent )
{

	ent->s.time2 = (int)ent->wait;
	if ( ent->target != NULL && ent->target[0] != 0 )
	{
		// try to use the target to orient me.
		gentity_t	*target = NULL;
		vec3_t		dir;

		target = G_Find (target, FOFS(targetname), ent->target);

		if (target == NULL)
		{
			DEVELOPER(G_Printf(S_COLOR_YELLOW "[Enitiy-Error] spark_link: target specified but not found: %s\n", ent->target););
			G_FreeEntity(ent);
			return;
		}
		
		VectorSubtract( target->s.origin, ent->s.origin, dir );
		VectorNormalize( dir );
		vectoangles( dir, ent->r.currentAngles );
		VectorCopy( ent->r.currentAngles, ent->s.angles2 );
		VectorShort(ent->s.angles2);
		VectorCopy( ent->r.currentAngles, ent->s.apos.trBase );
		SnapVector(ent->s.apos.trBase);
	}

	//TiM : for optional length in other functions
	ent->s.time = 10000;

	if ( ( ent->spawnflags & SPARK_STARTOFF ) == 0) {
		G_AddEvent( ent, EV_FX_SPARK, 0 );
		ent->count = 1;

		ent->think = spark_think;	
		ent->nextthink = level.time + 10000;
	}
	else {
		ent->count = 0;

		ent->think = NULL;	
		ent->nextthink = -1;
	}
}

//------------------------------------------
void SP_fx_spark( gentity_t	*ent )
{
	ent->type = ENT_FX_SPARK;

	if (ent->wait <= 0)
	{
		ent->wait = 2000.0;
	}

	SnapVector(ent->s.origin);
	VectorCopy( ent->s.origin, ent->s.pos.trBase );

	// The thing that this is targetting may not be spawned in yet, so wait a bit to try and link to it
	ent->think = spark_link; 
	ent->nextthink = level.time + 2000;
	ent->use = spark_use;

	trap_LinkEntity( ent );
}

/*QUAKED fx_steam (0 0 1) (-8 -8 -8) (8 8 8) STARTOFF 
-----DESCRIPTION-----
Emits steam at the specified point in the specified direction. Will point at a target if one is specified.

Can be toggled but use with caution as updates every second instead of every 10 seconds, 
which means it sends 10 times the information that an untoggleable steam will send.

-----SPAWNFLAGS-----
1: STARTOFF - steam is of at spawn  

-----KEYS-----
"targetname" - toggles on/off whenever used
"damage" - damage to apply when caught in steam vent, default - zero damage (no damage). Don't add this unless you really have to.
*/

#define STEAM_STARTOFF		1
#define STEAM_UNLINKED		999

//------------------------------------------
static void steam_think( gentity_t *ent )
{
	G_AddEvent( ent, EV_FX_STEAM, 0 );
	if(ent->targetname != NULL && ent->targetname[0] != 0) { //toggleable effect needs to be updated more often
		ent->nextthink = level.time + 1000;
	} else {
		ent->nextthink = level.time + 10000; // send a refresh message every 10 seconds
	}

	// FIXME: This may be a bit weird for steam bursts*/
	// If a fool gets in the bolt path, zap 'em
	if ( ent->damage == 0) 
	{
		vec3_t	start, temp;
		trace_t	trace;

		memset(&trace, 0, sizeof(trace));

		VectorSubtract( ent->s.origin2, ent->r.currentOrigin, temp );
		VectorNormalize( temp );
		VectorMA( ent->r.currentOrigin, 1, temp, start );

		trap_Trace( &trace, start, NULL, NULL, ent->s.origin2, -1, MASK_SHOT );//ignore

		if ( trace.fraction < 1.0 )
		{
			if ( trace.entityNum < ENTITYNUM_WORLD )
			{
				gentity_t *victim = &g_entities[trace.entityNum];
				if ( (victim != NULL) && victim->takedamage )
				{
					G_Damage( victim, ent, ent->activator, temp, trace.endpos, ent->damage, 0, MOD_LAVA );
				}
			}
		}
	}

}

//------------------------------------------
static void steam_use( gentity_t *self, /*@unused@*/ gentity_t *other, /*@unused@*/ gentity_t *activator )
{
	if(self->count == STEAM_UNLINKED) {
		return;
	}

	if ( self->count != 0 )
	{
		self->think = NULL;
		self->nextthink = -1;
		self->count = 0;
	}
	else
	{
		self->think = steam_think;
		self->nextthink = level.time + 100;
		self->count = 1;
	}
}

//------------------------------------------
static void steam_link( gentity_t *ent )
{
	gentity_t	*target = NULL;
	vec3_t		dir;

	if (ent->target != NULL && ent->target[0] != 0)
	{
		target = G_Find (target, FOFS(targetname), ent->target);
	}

	if (target == NULL)
	{
		if(ent->target != NULL) {
			DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] steam_link: unable to find target %s\n", ent->target););
		} else {
			DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] steam_link: unable to find target\n"););
		}

		G_FreeEntity(ent);
		return;
	}


	VectorSubtract( target->s.origin, ent->s.origin, dir );
	VectorNormalize(dir);
	VectorCopy(dir, ent->s.angles2);
	VectorShort(ent->s.angles2);
	
	VectorCopy( target->s.origin, ent->s.origin2 );
	SnapVector(ent->s.origin2);

	if(ent->targetname != NULL && ent->targetname[0] != 0) { // toggleable steam needs to be updated more often
		ent->s.time = 1000;
	} else {
		ent->s.time = 10000;
	}
	
	ent->use = steam_use;

	trap_LinkEntity( ent );

	// this actually creates the continuously-spawning steam jet
	if((ent->targetname == NULL) || ((ent->spawnflags & STEAM_STARTOFF) == 0)) {
		G_AddEvent( ent, EV_FX_STEAM, 0 );
	}

	ent->think = steam_think;
	
	if((ent->targetname != NULL) && ((ent->spawnflags & STEAM_STARTOFF) == 0)) { // toggleable steam needs to be updated more often
		ent->nextthink = level.time + 1000;
	} else {
		ent->nextthink = level.time + 10000;
	}

	// This is used as the toggle switch
	if(ent->targetname != NULL && ent->targetname[0] != 0) {
		if((ent->spawnflags & STEAM_STARTOFF) == 0) {
			ent->count = 1;
		} else {
			ent->count = 0;
		}
	}
}

//------------------------------------------
void SP_fx_steam( gentity_t	*ent )
{
	ent->type = ENT_FX_STEAM;

	SnapVector(ent->s.origin);
	VectorCopy( ent->s.origin, ent->s.pos.trBase );
	trap_LinkEntity( ent );

	// Try to apply defaults if nothing was set
	G_SpawnInt( "damage", "0", &ent->damage );

	ent->count = STEAM_UNLINKED; // so it can't be used before it's linked

	ent->think = steam_link;
	ent->nextthink = level.time + 2000;
}

/*QUAKED fx_bolt (0 0 1) (-8 -8 -8) (8 8 8) SPARKS BORG TAPER SMOOTH
-----DESCRIPTION-----
Emits blue ( or borg green ) electric bolts from the specified point to the specified point

Can be toggled by being used, but use with caution as updates every second instead of every 10 seconds, 
which means it sends 10 times the information that an untoggleable steam will send.

-----SPAWNFLAGS-----
1: SPARKS - create impact sparks, probably best used for time delayed bolts
2: BORG - Make the bolts green
4: TAPER
8: SMOOTH

-----KEYS-----
"targetname" - toggles on/off whenever used
"wait" - seconds between bolts (0 is always on, default is 2.0, -1 for random number between 0 and 5), bolts are always on for 0.2 seconds
"damage" - damage per server frame (default 0)
"random" - bolt chaos (0.1 = too calm, 0.5 = default, 1.0 or higher = pretty wicked)
*/


#define BOLT_SPARKS		1
#define BOLT_BORG		2
#define BOLT_TAPER		4
#define BOLT_SMOOTH		8

//------------------------------------------
static void bolt_think( gentity_t *ent )
{
	vec3_t	start, temp;
	trace_t	trace;

	G_AddEvent( ent, EV_FX_BOLT, ent->spawnflags );
	ent->s.time2 = (int)ent->wait;
	if(ent->targetname != NULL && ent->targetname[0] != 0) { //toggleable effect needs to be updated more often
		ent->nextthink = level.time + 1000;
	} else {
		ent->nextthink = level.time + 10000; // send a refresh message every 10 seconds
	}

	// If a fool gets in the bolt path, zap 'em
	if ( ent->damage != 0 ) 
	{
		VectorSubtract( ent->s.origin2, ent->r.currentOrigin, temp );
		VectorNormalize( temp );
		VectorMA( ent->r.currentOrigin, 1, temp, start );

		memset(&trace, 0, sizeof(trace_t));
		trap_Trace( &trace, start, NULL, NULL, ent->s.origin2, -1, MASK_SHOT );//ignore

		if ( trace.fraction < 1.0 )
		{
			if ( trace.entityNum < ENTITYNUM_WORLD )
			{
				gentity_t *victim = &g_entities[trace.entityNum];
				if ( (victim != NULL) && (victim->takedamage == qtrue) )
				{
					G_Damage( victim, ent, ent->activator, temp, trace.endpos, ent->damage, 0, MOD_PHASER_ALT );
				}
			}
		}
	}
	// net optimisations
	SnapVector(ent->s.origin2);
}

//------------------------------------------
static void bolt_use( gentity_t *self, /*@unused@*/ gentity_t *other, /*@unused@*/ gentity_t *activator )
{
	if ( self->count != 0 )
	{
		self->think = NULL;
		self->nextthink = -1;
		self->count = 0;
	}
	else
	{
		self->think = bolt_think;
		self->nextthink = level.time + 200;
		self->count = 1;
	}
}

//------------------------------------------
static void bolt_link( gentity_t *ent )
{
	gentity_t	*target = NULL;
	vec3_t		dir;

	if (ent->target != NULL && ent->target[0] != 0)
	{
		target = G_Find (target, FOFS(targetname), ent->target);
	} else {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] bolt_link: ent->target is NULL\n"););
		G_FreeEntity(ent);
		return;
	}

	if (target == NULL)
	{
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] bolt_link: unable to find target %s\n", ent->target););
		G_FreeEntity(ent);
		return;
	}

	VectorSubtract( target->s.origin, ent->s.origin, dir );
	VectorNormalize( dir );
	vectoangles( dir, ent->s.angles );
	
	VectorCopy( target->s.origin, ent->s.origin2 );
	SnapVector(ent->s.origin2);

	if ( ent->targetname != NULL && ent->targetname[0] != 0 )
	{
		ent->use = bolt_use;
	}

	G_AddEvent( ent, EV_FX_BOLT, ent->spawnflags );
	ent->s.time2 = (int)ent->wait;
	ent->think = bolt_think;	
	ent->nextthink = level.time + 10000;
	trap_LinkEntity( ent );
}

//------------------------------------------
void SP_fx_bolt( gentity_t *ent )
{
	ent->type = ENT_FX_BOLT;

	G_SpawnInt( "damage", "0", &ent->damage );
	G_SpawnFloat( "random", "0.5", &ent->random );
	G_SpawnFloat( "speed", "15.0", &ent->speed );

	// See if effect is supposed to be delayed
	G_SpawnFloat( "wait", "2.0", &ent->wait );

	SnapVector(ent->s.origin);
	VectorCopy( ent->s.origin, ent->s.pos.trBase );

	ent->s.angles2[0] = ent->speed;
	ent->s.angles2[1] = ent->random;

	if (ent->target != NULL && ent->target[0] != 0)
	{
		ent->think = bolt_link;
		ent->nextthink = level.time + 100;
		return;
	}

	trap_LinkEntity( ent );
}


//--------------------------------------------------
/*QUAKED fx_transporter (0 0 1) (-8 -8 -8) (8 8 8)
-----DESCRIPTION-----
Emits transporter pad effect at the specified point. just rest it flush on top of the pad. 

-----SPAWNFLAGS-----
none

-----KEYS-----
none

*/

static void transporter_link( gentity_t *ent )
{
	G_AddEvent( ent, EV_FX_TRANSPORTER_PAD, 0 );
}

//------------------------------------------
void SP_fx_transporter(gentity_t *ent)
{
	ent->type = ENT_FX_TRANSPORTER;

	SnapVector(ent->s.origin);
	VectorCopy( ent->s.origin, ent->s.pos.trBase );

	ent->think = transporter_link; 
	ent->nextthink = level.time + 2000;

	trap_LinkEntity( ent );
}



/*QUAKED fx_drip (0 0 1) (-8 -8 -8) (8 8 8) STARTOFF
-----DESCRIPTION-----
Drips of a fluid that fall down from this point.

Can be toggled by being used, but use with caution as updates every second instead of every 10 seconds, 
which means it sends 10 times the information that an untoggleable steam will send.

-----SPAWNFLAGS-----
1: STARTOFF - effect is off at spawn

-----KEYS-----
"targetname" - toggles on/off whenever used
"damage" - type of drips. 0 = water, 1 = oil, 2 = green
"random" - (0...1) degree of drippiness. 0 = one drip, 1 = Niagara Falls
*/
#define DRIP_STARTOFF 1

//------------------------------------------
static void drip_think( gentity_t *ent )
{
	G_AddEvent( ent, EV_FX_DRIP, 0 );
	ent->nextthink = level.time + 10000; // send a refresh message every 10 seconds
}

//------------------------------------------
void SP_fx_drip( gentity_t	*ent )
{
	ent->type = ENT_FX_DRIP;

	ent->s.time2 = ent->damage; 
	ent->s.angles2[0] = ent->random; 

	SnapVector(ent->s.origin);
	VectorCopy( ent->s.origin, ent->s.pos.trBase );

	ent->think = drip_think; 
	ent->nextthink = level.time + 1000;

	//TiM
	ent->s.powerups = 10000;

	trap_LinkEntity( ent );
}

//TiM - RPG-X FX Funcs
//Most of these were copied from EF SP, and then modified for compatibility with the EF MP engine
/***********************************************************************************/

/*QUAKED fx_fountain (0 0 1) (-8 -8 -8) (8 8 8) STARTOFF
-----DESCRIPTION-----
Fountain-Effect as seen iin the Garden of Scilence holodeck Programm.
This is just one single strain of the original effect (which had all four strains hardcoded)

Use with caution as this refreshes 10 times a second.

-----SPAWNFLAGS-----
1: STARTOFF - Effect spawns in an off state

-----KEYS-----
"targetname" - name of entity when used turns this ent on/off
"target" - link to an info_notnull entity or similar to position where the end point of this FX is
*/
#define FOUNTAIN_STARTOFF 1

static void fountain_think( gentity_t *ent ) 
{
	G_AddEvent( ent, EV_FX_GARDEN_FOUNTAIN_SPURT, 0 );
	ent->nextthink = level.time + 100;	
}

static void fountain_use( gentity_t *self, /*@unused@*/ gentity_t *other, /*@unused@*/ gentity_t *activator )
{
	if ( self->count != 0 )
	{
		self->think = NULL;
		self->nextthink = -1;
		self->count = 0;
	}
	else
	{
		self->think = fountain_think;
		self->nextthink = level.time + 100;
		self->count = 1;
	}
}

void SP_fx_fountain ( gentity_t *ent ) {
	gentity_t	*target = NULL;

	ent->type = ENT_FX_FOUNTAIN;

	if ( ent->target != NULL && ent->target[0] != 0 ) {
		target = G_Find (target, FOFS(targetname), ent->target);
	} else {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] fx_fountain: ent->target is NULL\n"););
		G_FreeEntity(ent);
		return;
	}

	if ( target == NULL ) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] fx_fountain: Unable to find target point: %s\n", ent->target););
		G_FreeEntity(ent);
		return;
	}

	SnapVector( ent->s.origin );
	VectorCopy( ent->s.origin, ent->s.pos.trBase );

	//Set end point to be origin2
	VectorCopy( target->s.origin, ent->s.origin2 );

	ent->use = fountain_use;

	//on and/or off state
	if((ent->spawnflags & FOUNTAIN_STARTOFF) == 0) {
		ent->count = 1;
	} else {
		ent->count = 0;
	}

	if ( ent->count != 0 ) {
		ent->think = fountain_think;
		ent->nextthink = level.time + 100;
	}
	else {
		ent->think = NULL;
		ent->nextthink= -1;
	}

	trap_LinkEntity( ent );
}

/*QUAKED fx_surface_explosion (0 0 1) (-8 -8 -8) (8 8 8) NO_SMOKE LOUDER NODAMAGE
-----DESCRIPTION-----
Creates a triggerable explosion aimed at a specific point.  Always oriented towards viewer.

-----SPAWNFLAGS-----
1: NO_SMOKE - Does not create smoke after explosion
2: LOUDER - Cheap hack to make the explosion sound louder.
4: NODAMAGE - Does no damage

-----KEYS-----
"target" (optional) If no target is specified, the explosion is oriented up
"damage" - Damage per blast, default is 50. Damage falls off based on proximity.
"radius" - blast radius (default 20)
"speed" - camera shake speed (default 12).  Set to zero to turn camera shakes off
"targetname" - triggers explosion when used
*/

//------------------------------------------
static void surface_explosion_use( gentity_t *self, /*@unused@*/ gentity_t *other, /*@unused@*/ gentity_t *activator)
{

	G_AddEvent( self, EV_FX_SURFACE_EXPLOSION, 0 );

	if ( self->splashDamage != 0 ) {
		G_RadiusDamage( self->r.currentOrigin, self, (float)self->splashDamage, (float)self->splashRadius, self, DAMAGE_RADIUS|DAMAGE_ALL_TEAMS, MOD_EXPLOSION );
	}
}

//------------------------------------------
static void surface_explosion_link( gentity_t *ent )
{
	gentity_t	*target = NULL;
	vec3_t		normal;
	
	if(ent->target != NULL && ent->target[0] != 0) {
		target = G_Find (target, FOFS(targetname), ent->target);
	}

	if ( target != NULL )
	{
		VectorSubtract( target->s.origin, ent->s.origin, normal );
		VectorNormalize( normal );
	}
	else
	{
		VectorSet(normal, 0.0f, 0.0f, 1.0f);
	}

	VectorCopy( normal, ent->s.origin2 );

	ent->think = NULL;
	ent->nextthink = -1;
	ent->use = surface_explosion_use;

	trap_LinkEntity( ent );
}

//------------------------------------------
void SP_fx_surface_explosion( gentity_t *ent )
{
	ent->type = ENT_FX_SURFACE_EXPLOSION;

	if ( (ent->spawnflags & 4) == 0 ){
		G_SpawnInt( "damage", "50", &ent->splashDamage );
		G_SpawnFloat( "radius", "20", &ent->distance ); // was: ent->radius
		ent->splashRadius = 160;
	}

	G_SpawnFloat( "speed", "12", &ent->speed );

	//TiM: Insert relevant params into the entityState struct so it gets passed to the client
	//Hacky this is... mmm yes

	ent->s.angles2[0] = ent->distance; // was: ent->radius
	ent->s.angles2[1] = ent->speed;
	ent->s.time2 = ent->spawnflags;

	VectorCopy( ent->s.origin, ent->s.pos.trBase );

	ent->use = surface_explosion_use;
	ent->think = surface_explosion_link;
	ent->nextthink = 1000;

	trap_LinkEntity( ent );
}

/*QUAKED fx_blow_chunks (0 0 1) (-8 -8 -8) (8 8 8)
-----DESCRIPTION-----
Creates a triggerable chunk spewer that can be aimed at a specific point.

-----SPAWNFLAGS-----
none

-----KEYS-----
"target" - (required) Target to spew chunks at
"targetname" - triggers chunks when used 
"radius" - Average size of a chunk (default 65)

"material" - default is "metal" - choose from this list:
	Metal = 1
	Glass = 2
	Glass Metal = 3
	Wood = 4
	Stone = 5
*/

//"count" - Number of chunks to spew (default 5)
//"speed" - How fast a chunk will move when it get's spewed (default 175)
//------------------------------------------
static void blow_chunks_use( gentity_t *self, /*@unused@*/ gentity_t *other, /*@unused@*/ gentity_t *activator)
{
	self->r.svFlags |= SVF_BROADCAST;
	G_AddEvent( self, EV_FX_CHUNKS, 0 );
}

//------------------------------------------
static void blow_chunks_link( gentity_t *ent )
{
	gentity_t	*target = NULL;

	ent->think = NULL;
	ent->nextthink = -1;

	if(ent->target != NULL && ent->target[0] != 0) {
		target = G_Find (target, FOFS(targetname), ent->target);
	} else {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] blow_chunks_link: ent->target is NULL\n"););
		G_FreeEntity(ent);
		return;
	}

	if ( target == NULL )
	{
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] blow_chunks_link: unable to find target %s\n", ent->target););
		G_FreeEntity(ent);
		return;
	}

	VectorCopy( target->s.origin, ent->s.angles2 );

	trap_LinkEntity( ent );
}

//------------------------------------------
void SP_fx_blow_chunks( gentity_t *ent )
{
	ent->type = ENT_FX_BLOW_CHUNKS;

	G_SpawnFloat( "radius", "65", &ent->distance );
	G_SpawnInt( "material", "1", &ent->s.powerups );

	if(ent->distance <= 0.0f) {
		ent->distance = 65.0f;
	}

	if(ent->s.powerups == 0) {
		ent->s.powerups = 1;
	}

	VectorCopy( ent->s.origin, ent->s.pos.trBase );

	//TiM: Fill entityState
	ent->s.time2 = (int)ent->distance; //Hack. :P The client side chunkfunc wants radius to be an int >.< || was: ent->radius

	ent->use = blow_chunks_use;

	ent->think = blow_chunks_link;
	ent->nextthink = 1000;
}

/*QUAKED fx_smoke (0 0 1) (-8 -8 -8) (8 8 8) STARTOFF
-----DESCRIPTION-----
Emits cloud of thick black smoke from specified point.

-----SPAWNFLAGS-----
1: STARTOFF - Effect will be off at spawn

-----KEYS-----
"target" - optional, if no target is specified, the smoke drifts up
"targetname" - fires only when used
"radius" - size of the smoke puffs (default 16.0)
*/
#define SMOKE_STARTOFF 1

//------------------------------------------
static void smoke_think( gentity_t *ent )
{
	ent->nextthink = level.time + 10000;
	G_AddEvent( ent, EV_FX_SMOKE, 0 );
}

//------------------------------------------
static void smoke_use( gentity_t *self, /*@unused@*/ gentity_t *other, /*@unused@*/ gentity_t *activator)
{
	if ( self->count != 0 )
	{
		self->think = NULL;
		self->nextthink = -1;
		self->count = 0;
	}
	else
	{
		self->think = smoke_think;
		self->nextthink = level.time + 10000;
		self->count = 1;
	}
}

//------------------------------------------
static void smoke_link( gentity_t *ent )
{
	// this link func is used because the target ent may not have spawned in yet, this
	//	will give it a bit of extra time for that to happen.
	//TiM: Ohhhh that's why.  I thought this was weird lol...
	gentity_t	*target = NULL;
	vec3_t		dir;

	if((ent->target != NULL) && (ent->target[0] != 0)) {
		target = G_Find (target, FOFS(targetname), ent->target);
	}

	if (target != NULL)
	{
		VectorSubtract( target->s.origin, ent->s.origin, dir );
		VectorNormalize( dir );
		vectoangles( dir, ent->s.angles2 );
		VectorShort(ent->s.angles2);
	}
	else
	{
		//Hard code to be directly up
		VectorSet( dir, 0.0f, 0.0f, 10.0f );
		VectorNormalize( dir );
		vectoangles( dir, ent->s.angles2 );
		VectorShort(ent->s.angles2);
	}

	if ((ent->targetname != NULL) && (ent->targetname[0] != 0))
	{
		ent->use = smoke_use;
	}

	if ((ent->targetname == NULL) || ((ent->spawnflags & SMOKE_STARTOFF) == 0) )
	{
		ent->think = smoke_think;
		ent->nextthink = level.time + 200;
		ent->count = 1;
	}
	else
	{
		ent->think = NULL;
		ent->nextthink = -1;
		ent->count = 0;
	}

	ent->s.time2 = 10000;

	trap_LinkEntity( ent );
}

//------------------------------------------
void SP_fx_smoke( gentity_t *ent )
{
	ent->type = ENT_FX_SMOKE;

	G_SpawnFloat( "radius", "16.0", &ent->distance ); // was: ent->radius

	VectorCopy( ent->s.origin, ent->s.pos.trBase );
	ent->s.angles2[0] = ent->distance; // was: ent->radius

	ent->think = smoke_link;
	ent->nextthink = level.time + 1000;
	
	trap_LinkEntity( ent );
}

/*QUAKED fx_electrical_explosion (0 0 1) (-8 -8 -8) (8 8 8) x x NODAMAGE
-----DESCRIPTION-----
Creates a triggerable explosion aimed at a specific point

-----SPAWNFLAGS-----
1: NODAMAGE - does no damage

-----KEYS-----
"target" - optional, if no target is specified, the explosion is oriented up
"damage" - Damage per blast, default is 20. Damage falls off based on proximity.
"radius" - blast radius (default 50)
"targetname" - explodes each time it's used
*/
#define ELEC_EXP_NODAMAGE 4

//------------------------------------------
static void electrical_explosion_use( gentity_t *self, /*@unused@*/ gentity_t *other, /*@unused@*/ gentity_t *activator)
{
	G_AddEvent( self, EV_FX_ELECTRICAL_EXPLOSION, 0 );

	if ( self->splashDamage != 0 )
	{
		G_RadiusDamage( self->s.origin, self, (float)self->splashDamage, (float)self->splashRadius, self, 0, MOD_EXPLOSION );
	}
}

//------------------------------------------
static void electrical_explosion_link( gentity_t *ent )
{
	gentity_t	*target = NULL;
	vec3_t		normal;

	if(ent->target != NULL && ent->target[0] != 0) {
		target = G_Find( target, FOFS(targetname), ent->target );
	}

	if ( target != NULL )
	{
		VectorSubtract( target->s.pos.trBase, ent->s.origin, normal );
		VectorNormalize( normal );
	}
	else
	{
		// No target so just shoot up
		VectorSet( normal, 0.0f, 0.0f, 1.0f );
	}

	VectorCopy( normal, ent->s.origin2 );

	ent->think = NULL;
	ent->nextthink = -1;

	trap_LinkEntity( ent );
}

//------------------------------------------
void SP_fx_electrical_explosion( gentity_t *ent )
{
	ent->type = ENT_FX_ELETRICAL_EXPLOSION;

	if ( (ent->spawnflags & 4) == 0 )
	{
		G_SpawnInt( "damage", "20", &ent->splashDamage );
		G_SpawnFloat( "radius", "50", &ent->distance ); // was: ent->radius
		ent->splashRadius = 80;
	}

	VectorCopy( ent->s.origin, ent->s.pos.trBase );
	ent->s.angles2[0] = ent->distance; // was: ent->radius

	ent->think = electrical_explosion_link;
	ent->nextthink = level.time + 1000;

	ent->use = electrical_explosion_use;
	trap_LinkEntity( ent );
}

/*QUAKED fx_phaser (0 0 1) (-8 -8 -8) (8 8 8) NO_SOUND DISRUPTOR LOCKED
-----DESCRIPTION-----
A phaser effect for use as a ship's weapon.

-----SPAWNFLAGS-----
1: NO_SOUND - will not play it's sound
2: DISRUPTOR - will display a green disruptor beam
4: LOCKED - will be locked at spawn

-----KEYS-----
"target" - endpoint
"swapname" - lock/unlock entity
"wait" - how long the phaser fires
"scale" - adjust the effects scale, default: 20
"customSnd" - use a custom sound
"delay" - delay the effect, but not the sound. Can be used to adjust the timing between effect and customSnd
"impact" - set to 1 if you want an impact to be drawn
*/
#define PHASER_FX_NOSOUND 1
#define PHASER_FX_DISRUPTOR 2
#define PHASER_FX_LOCKED 4
#define PHASER_FX_UNLINKED 999

static void phaser_use(gentity_t *ent, /*@unused@*/ gentity_t *other, gentity_t *activator) {
	G_LocLogger(LL_TRACE, "%s - begin\n", __FUNCTION__);

	if(ent->count == PHASER_FX_UNLINKED) {
		G_Logger(LL_DEBUG, "not linked yet\n");
		G_LocLogger(LL_TRACE, "%s - end\n", __FUNCTION__);
		return;
	}

	if(Q_stricmp(ent->swapname, activator->target) == 0) {
		G_Logger(LL_DEBUG, "locking/unlocking\n");
		ent->flags ^= FL_LOCKED;
	} else {
		if((ent->flags & FL_LOCKED) != 0){
			trap_SendServerCommand(activator-g_entities, va("print \"^1Phasers are offline.\n\""));
			G_AddEvent(ent, EV_GENERAL_SOUND, ent->n00bCount);
			G_LocLogger(LL_TRACE, "%s - end\n", __FUNCTION__);
			return;
		}

		if((ent->spawnflags & PHASER_FX_DISRUPTOR) != 0)
		{ 
			G_Logger(LL_DEBUG, "sending disruptor event\n");
			G_AddEvent(ent, EV_FX_DISRUPTOR, 0);
		}
		else 
		{
			G_Logger(LL_DEBUG, "sending phaser event\n");
			G_AddEvent(ent, EV_FX_PHASER, 0);
		}
	}

	G_LocLogger(LL_TRACE, "%s - end\n", __FUNCTION__);
}

static void phaser_link(gentity_t *ent) {
	gentity_t *target = NULL;
	
	G_LocLogger(LL_TRACE, "%s\n", __FUNCTION__);

	if(ent->target != NULL && ent->target[0] != 0) {
		target = G_Find(target, FOFS(targetname), ent->target);
	} else {
		G_LocLogger(LL_ERROR, "[Entity-Error] phaser_link: ent->target is NULL\n");
		G_FreeEntity(ent);
		return;
	}

	if(target == NULL) {
		G_Logger(LL_ERROR, "[Enity-Error] Could not find target %s for fx_phaser at %s!\n", ent->target, vtos(ent->r.currentOrigin));
		G_FreeEntity(ent);
		G_LocLogger(LL_TRACE, "%s - end\n", __FUNCTION__);
		return;
	}

	VectorCopy(target->s.origin, ent->s.origin2);
	SnapVector(ent->s.origin2);

	G_Logger(LL_DEBUG, "target->origin=%s, snaped=%s\n", vtos(target->s.origin), vtos(ent->s.origin2));

	ent->use = phaser_use;
	ent->count = 0;
	trap_LinkEntity(ent);

	G_LocLogger(LL_TRACE, "%s - end\n", __FUNCTION__);
}

void SP_fx_phaser(gentity_t *ent) {
	float	scale = 0.0f;
	char	*sound = NULL;
	int		impact = 0;

	G_LocLogger(LL_TRACE, "%s - begin\n", __FUNCTION__);

	ent->type = ENT_FX_PHASER;
	ent->count = PHASER_FX_UNLINKED;

	if(ent->target == NULL || ent->target[0] == 0) {
		G_Logger(LL_ERROR, "[Entity-Error] fx_phaser at %s without target!\n", vtos(ent->r.currentOrigin));
		G_LocLogger(LL_TRACE, "%s - end\n", __FUNCTION__);
		return;
	}

	G_SpawnFloat("scale", "20", &scale);
	ent->s.angles[0] = scale;
	G_SpawnFloat("delay", "1", &scale);
	ent->s.angles[1] = scale * 1000;
	G_SpawnString("customSnd", "sound/pos_b/phaser.wav", &sound);

	G_Logger(LL_DEBUG, "scale=%f, delay=%f\n", ent->s.angles[0], ent->s.angles[1]);

	if((ent->spawnflags & PHASER_FX_NOSOUND) == 0) {
		ent->s.time = G_SoundIndex(sound);
	} else {
		ent->s.time = G_SoundIndex("NULL");
	}

	G_Logger(LL_DEBUG, "soundindex=%d\n", ent->s.time);

	if(ent->wait <= 0.0f) { 
		ent->s.time2 = 3000;
	} else {
		ent->s.time2 = (int)(ent->wait * 1000.0f);
	}

	G_Logger(LL_DEBUG, "firetime=%d\n", ent->s.time2);

	if((ent->spawnflags & PHASER_FX_LOCKED) != 0) {
		ent->flags |= FL_LOCKED;
	}

	G_SpawnInt("impact", "0", &impact);
	ent->s.angles[2] = (float)impact;

	G_Logger(LL_DEBUG, "impact=%d\n", (int)ent->s.angles2[2]);

	ent->think = phaser_link;
	ent->nextthink = level.time + 1000;
	trap_LinkEntity(ent);

	G_LocLogger(LL_TRACE, "%s - end\n", __FUNCTION__);
}

/*QUAKED fx_torpedo (0 0 1) (-8 -8 -8) (8 8 8) QUANTUM NO_SOUND LOCKED
-----DESCRIPTION-----
A torpedo effect for use as a ship's weapon.
Torpedos can be limited and can be set to any value within their maximum range by admin-command "reloadtorpedos [amount] [targetname]"

-----SPAWNFLAGS-----
1: QUANTUM - set this flag if you whant an quantum fx instead of an photon fx
2: NO_SOUND - Will not play it's sound
4: LOCKED - will be locked at spawn

-----KEYS-----
"target" - used for the calculation of the direction
"swapname" - lock/unlock entity
"wait" - time in seconds till fx can be used again
"noise" - sound to play
"soundNoAmmo" - sound to play if ammo is depleted
"count" - ammount of torpedos that can be fired (defaults to -1 = infinite)
"speed" - a speed modifier (default: 2.5)
*/
#define TORPEDO_FX_QUANTUM 1
#define TORPEDO_FX_NOSOUND 2
#define TORPEDO_FX_LOCKED 4

static void fx_torpedo_use(gentity_t* ent, gentity_t*other, gentity_t *activator);

static void fx_torpedo_think(gentity_t *ent) {
	G_LocLogger(LL_TRACE, "%s\n", __FUNCTION__);

	ent->nextthink = -1;
	ent->use = fx_torpedo_use;
}

static void fx_torpedo_use(gentity_t *ent, /*@unused@*/ gentity_t *other, gentity_t *activator) {
	G_LocLogger(LL_TRACE, "%s - begin\n", __FUNCTION__);

	if(Q_stricmp(ent->swapname, activator->target) == 0) {
		G_Logger(LL_DEBUG, "lock/unlock\n");
		ent->flags ^= FL_LOCKED;
	} else {
		if((ent->flags & FL_LOCKED) != 0){
			trap_SendServerCommand(activator-g_entities, va("print \"^1Torpedo launcher is offline.\n\""));
			G_AddEvent(ent, EV_GENERAL_SOUND, ent->n00bCount);
			G_LocLogger(LL_TRACE, "%s - end\n", __FUNCTION__);
			return;
		}
	
		if(ent->count > 0) {
			ent->count--;
			trap_SendServerCommand(activator-g_entities, va("print \"Torpedos: %i of %i left.\n\"", ent->count, ent->damage));
			G_AddEvent(ent, EV_GENERAL_SOUND, ent->s.time);
		} 

		if(ent->count == 0){
			trap_SendServerCommand(activator-g_entities, "print \"^1Out of Torpedos.\n\"");
			G_LocLogger(LL_TRACE, "%s - end\n", __FUNCTION__);
			return;
		}
	
		G_AddEvent(ent, EV_FX_TORPEDO, ent->spawnflags);
		ent->use = NULL;
		ent->think = fx_torpedo_think;
		ent->nextthink = level.time + ent->wait;
	}

	G_LocLogger(LL_TRACE, "%s - end\n", __FUNCTION__);
}

static void fx_torpedo_link(gentity_t *ent) {
	vec3_t dir;
	gentity_t *target = NULL;

	G_LocLogger(LL_TRACE, "%s - begin\n", __FUNCTION__);

	if(ent->target != NULL && ent->target[0] != 0) {
		target = G_Find(target, FOFS(targetname), ent->target);
	} else {
		G_Logger(LL_ERROR, "[Entity-Error] fx_torpedo_link: ent->target is NULL\n");
		G_FreeEntity(ent);
		G_LocLogger(LL_TRACE, "%s - end\n", __FUNCTION__);
		return;
	}
	
	if(target == NULL) {
		G_Logger(LL_ERROR, "[Entity-Error] Could not find target %s for fx_torpedo at %s!\n", ent->target, vtos(ent->s.origin));
		G_FreeEntity(ent);
		G_LocLogger(LL_TRACE, "%s - end\n", __FUNCTION__);
		return;
	}

	VectorSubtract(target->s.origin, ent->s.origin, dir);
	VectorCopy(target->s.origin, ent->s.origin2);
	VectorNormalize(dir);
	VectorCopy(dir, ent->s.angles);
	trap_LinkEntity(ent);

	G_Logger(LL_DEBUG, "target->origin=%s, direction=%s\n", vtos(ent->s.origin), vtos(ent->s.angles));

	ent->wait = ent->wait ? (ent->wait * 1000.0f) : 0.0f;

	G_Logger(LL_DEBUG, "ent->wait=%f\n", ent->wait);
	
	if(ent->count == 0) {
		ent->count = -1;
	} 

	ent->damage = ent->count;

	if((ent->spawnflags & 4) != 0) {
		ent->flags |= FL_LOCKED;
	}

	ent->s.angles2[0] = (ent->speed <= 0.0f) ? 2.5f : ent->speed;

	G_Logger(LL_DEBUG, "speed=%f\n", ent->s.angles2[0]);

	ent->use = fx_torpedo_use;
	ent->nextthink = -1;

	G_LocLogger(LL_TRACE, "%s - end\n", __FUNCTION__);
}

void SP_fx_torpedo(gentity_t *ent) {
	char	*sound;

	G_LocLogger(LL_TRACE, "%s - begin\n", __FUNCTION__);

	ent->type = ENT_FX_TORPEDO;

	if(ent->target == NULL || ent->target[0] == 0) {
		G_Logger(LL_ERROR, "[Entity-Error] fx_torpedo at %s without target\n", vtos(ent->s.origin));
		G_FreeEntity(ent);
		G_LocLogger(LL_TRACE, "%s - end\n", __FUNCTION__);
		return;
	}

	G_SpawnString("noise", "sound/rpg_runabout/torp.wav", &sound);
	 if((ent->spawnflags & 2) == 0) {
		ent->s.time = G_SoundIndex(sound);
		G_Logger(LL_DEBUG, "using sound %s with sound index %d\n", sound, ent->s.time);
	} else {
		ent->s.time = G_SoundIndex("NULL");
	}
	

	G_SpawnString("soundNoAmmo", "sound/movers/switches/voyneg.mp3", &sound);
	if((ent->spawnflags & 2) == 0) {
		ent->n00bCount = G_SoundIndex(sound);
		G_Logger(LL_DEBUG, "using soundNoAmmo %s with sound index %d\n", sound, ent->n00bCount);
	} else {
		ent->n00bCount = G_SoundIndex("NULL");
	}

	ent->think = fx_torpedo_link;
	ent->nextthink = level.time + 1000;

	G_LocLogger(LL_TRACE, "%s - end\n", __FUNCTION__);
}

/*QUAKED fx_particle_fire (0 0 1) (-8 -8 -8) (8 8 8) STARTOFF
-----DESCRIPTION-----
A particle based fire effect. Use this sparingly as it is an fps killer.
If you want to use a bunch of fires use fx_fire.

-----SPAWNFLAGS-----
1: STARTOFF - Effect will be off at spawn

-----KEYS-----
"targetname" - toggles effect on/off whenver used, requires 10x more thinks
"size" - how big the fire shoud be (default: 10)
*/
#define PARTICLE_FIRE_STARTOFF 1

static void particleFire_think(gentity_t *ent) {
	G_AddEvent(ent, EV_FX_PARTICLEFIRE, ent->count);
	if (ent->targetname != NULL && ent->targetname[0] != 0) {
		ent->nextthink = level.time + 1000; 
	} else {
		ent->nextthink = level.time + 10000;
	}
}

static void particleFire_use( gentity_t *self, /*@unused@*/ gentity_t *other, /*@unused@*/ gentity_t *activator)
{
	if ( self->count != 0 )
	{
		self->think = NULL;
		self->nextthink = -1;
		self->count = 0;
	}
	else
	{
		self->think = particleFire_think;
		self->nextthink = level.time + 200;
		self->count = 1;
	}
}

void SP_fx_particleFire(gentity_t *ent) {
	int size;

	ent->type = ENT_FX_PARTICLEFIRE;

	G_SpawnInt("size", "10", &size);
	if(size == 0) {
		ent->count = 10;
	} else {
		ent->count = size;
	}

	if (ent->targetname != NULL && ent->targetname[0] != 0) {
		ent->s.time2 = 1000; 
	} else {
		ent->s.time2 = 10000;
	}

	trap_LinkEntity(ent);
	
	if (ent->targetname != NULL && ent->targetname[0] != 0)
	{
		ent->use = particleFire_use;
	}

	if((ent->spawnflags & PARTICLE_FIRE_STARTOFF) == 0) {
		ent->count = 1;
	} else {
		ent->count = 0;
	}

	if (ent->targetname == NULL || ent->targetname[0] == 0 || (ent->spawnflags & PARTICLE_FIRE_STARTOFF) == 0 )
	{
		ent->think = particleFire_think;
		ent->nextthink = level.time + 2000;
	}
	else
	{
		ent->think = NULL;
		ent->nextthink = -1;
	}

}

/*QUAKED fx_fire (0 0 1) (-8 -8 -8) (8 8 8) STARTOFF
-----DESCRIPTION-----
A fire affect based on the adminguns fire effect.

-----SPAWNFLAGS-----
1: STARTOFF - Effect will be off at spawn

-----KEYS-----
"targetname" - toggles effect on/off whenver used, requires 10x more thinks
"size" - how big the fire shoud be (default: 64)
"angles" - fires angles (default: 0 0 0 = UP)
*/
#define FIRE_FX_STARTOFF 1

static void fire_think(gentity_t *ent) {
	G_AddEvent(ent, EV_FX_FIRE, 1);
	if (ent->targetname != NULL && ent->targetname[0] != 0) {
		ent->nextthink = level.time + 1000; 
	} else {
		ent->nextthink = level.time + 10000;
	}
}

static void fire_use( gentity_t *self, /*@unused@*/ gentity_t *other, /*@unused@*/ gentity_t *activator)
{
	if ( self->count != 0 )
	{
		self->think = NULL;
		self->nextthink = -1;
		self->count = 0;
	}
	else
	{
		self->think = fire_think;
		self->nextthink = level.time + 200;
		self->count = 1;
	}
}

void SP_fx_fire(gentity_t *ent) {
	int size;

	ent->type = ENT_FX_FIRE;

	G_SpawnInt("size", "64", &size);
	if(size == 0) {
		ent->s.time = 64;
	} else {
		ent->s.time = size;
	}
	ent->s.angles2[2] = 1.0f;

	if (ent->targetname != NULL && ent->targetname[0] != 0) {
		ent->s.time2 = 1000; 
	} else {
		ent->s.time2 = 10000;
	}

	trap_LinkEntity(ent);

	if (ent->targetname != NULL && ent->targetname[0] != 0)	{
		ent->use = fire_use;
	}

	if((ent->spawnflags & FIRE_FX_STARTOFF) == 0) {
		ent->count = 1;
	} else {
		ent->count = 0;
	}

	if (ent->targetname == NULL || ent->targetname[0] == 0 || (ent->spawnflags & FIRE_FX_STARTOFF) == 0 )
	{
		ent->think = fire_think;
		ent->nextthink = level.time + 2000;
	}
	else
	{
		ent->think = NULL;
		ent->nextthink = -1;
	}
}

// Additional ports from SP by Harry Young

/*QUAKED fx_cooking_steam (0 0 1) (-8 -8 -8) (8 8 8) STARTOFF
-----DESCRIPTION-----
Emits slowly moving steam puffs that rise up from the specified point

-----SPAWNFLAGS-----
1: STARTOFF - Effect will be off at spawn

-----KEYS-----
"targetname" - toggles effect on/off whenver used
"distance" - smoke puff size ( default 3.0 )
*/
#define COOCKING_STEAM_STARTOFF 1

//------------------------------------------
static void cooking_steam_think( gentity_t *ent )
{
	G_AddEvent( ent, EV_FX_COOKING_STEAM, 0 );
	ent->nextthink = level.time + 100;
}

//------------------------------------------
static void cooking_steam_use( gentity_t *self, /*@unused@*/ gentity_t *other, /*@unused@*/ gentity_t *activator)
{
	if ( self->count != 0 )
	{
		self->think = NULL;
		self->nextthink = -1;
		self->count = 0;
	}
	else
	{
		self->think = cooking_steam_think;
		self->nextthink = level.time + 200;
		self->count = 1;
	}
}

//------------------------------------------
void SP_fx_cooking_steam( gentity_t	*ent )
{
	ent->type = ENT_FX_COOKING_STEAM;

	if (ent->distance <= 0.0f) {
		ent->distance = 3.0f;
	}

	ent->s.angles[0] = ent->distance;

	trap_LinkEntity( ent );

	if (ent->targetname != NULL && ent->targetname[0] != 0)	{
		ent->use = cooking_steam_use;
	}

	if((ent->spawnflags & COOCKING_STEAM_STARTOFF) == 0) {
		ent->count = 1;
	} else {
		ent->count = 0;
	}

	if (ent->targetname == NULL || ent->targetname[0] != 0 || (ent->spawnflags & COOCKING_STEAM_STARTOFF) == 0 ) {
		ent->think = cooking_steam_think;
		ent->nextthink = level.time + 2000;
	} else 	{
		ent->think = NULL;
		ent->nextthink = -1;
	}

	VectorCopy( ent->s.origin, ent->s.pos.trBase );
}

/*QUAKED fx_elecfire (0 0 1) (-8 -8 -8) (8 8 8) STARTOFF
-----DESCRIPTION-----
Emits sparks at the specified point in the specified direction
Spawns smoke puffs.

-----SPAWNFLAGS-----
1: STARTOFF - Effect will be off at spawn

-----KEYS-----
"targetname" - toggles effect on/off whenver used
*/
#define ELEC_FIRE_STARTOFF 1

//------------------------------------------
static void electric_fire_think( gentity_t *ent )
{
	G_AddEvent( ent, EV_FX_ELECFIRE, 0 );
	ent->nextthink = level.time + (750 + (random() * 300));
}

//------------------------------------------
static void electric_fire_use( gentity_t *self, /*@unused@*/ gentity_t *other, /*@unused@*/ gentity_t *activator)
{
	if ( self->count != 0 )
	{
		self->think = NULL;
		self->nextthink = -1;
		self->count = 0;
	}
	else
	{
		self->think = electric_fire_think;
		self->nextthink = level.time + 200;
		self->count = 1;
	}
}


//------------------------------------------
void SP_fx_electricfire( gentity_t	*ent )
{
	ent->type = ENT_FX_ELECTRICFIRE;

	if (ent->targetname != NULL && ent->targetname[0] != 0)	{
		ent->use = electric_fire_use;
	}

	if((ent->spawnflags & ELEC_FIRE_STARTOFF) == 0) {
		ent->count = 1;
	} else {
		ent->count = 0;
	}

	if (ent->targetname == NULL || ent->targetname[0] == 0 || (ent->spawnflags & ELEC_FIRE_STARTOFF) == 0 ) {
		ent->think = electric_fire_think;
		ent->nextthink = level.time + 500;
	} else {
		ent->think = NULL;
		ent->nextthink = -1;
	}

	VectorCopy( ent->s.origin, ent->s.pos.trBase );
	
	// Precaching sounds
	G_SoundIndex("sound/ambience/spark1.wav");
	G_SoundIndex("sound/ambience/spark2.wav");
	G_SoundIndex("sound/ambience/spark3.wav");
	G_SoundIndex("sound/ambience/spark4.wav");
	G_SoundIndex("sound/ambience/spark5.wav");
	G_SoundIndex("sound/ambience/spark6.wav");

	trap_LinkEntity( ent );
}

/*QUAKED fx_forge_bolt (0 0 1) (-8 -8 -8) (8 8 8) STARTOFF DELAYED SPARKS PULSE TAPER SMOOTH
-----DESCRIPTION-----
CURRENTLY DISABLED
Emits freaky orange bolts, sending pulses down the length of the beam if desired

-----SPAWNFLAGS-----
1: STARTOFF - effect is initially off
2: DELAYED - bolts are time delayed, otherwise effect continuously fires
4: SPARKS - create impact sparks, probably best used for time delayed bolts
8: PULSE - sends a pulse down the length of the beam.
16: TAPER - Bolt will taper on one end
32: SMOOTH - Bolt texture stretches across whole length, makes short bolts look much better.

-----KEYS-----
"wait" - seconds between bolts, only valid when DELAYED is checked (default 2)
"damage" - damage per server frame (default 0)
"targetname" - toggles effect on/off each time it's used
"random" - bolt chaos (0.1 = too calm, 0.4 = default, 1.0 or higher = pretty wicked)
"radius" - radius of the bolt (3.0 = default) 
*/
#define FORGE_BOLT_STARTOFF 1
#define FORGE_BOLT_DELAYED 2
#define FORGE_BOLT_SPARKS 4
#define FORGE_BOLT_PULSE 8
#define FORGE_BOLT_TAPER 16
#define FORGE_BOLT_SMOOTH 32

//------------------------------------------
static void forge_bolt_think( gentity_t *ent )
{
	G_AddEvent( ent, EV_FX_FORGE_BOLT, ent->spawnflags & FORGE_BOLT_DELAYED );
	ent->nextthink = (int)(level.time + (ent->wait + crandom() * ent->wait * 0.25) * 1000);

	// If a fool gets in the bolt path, zap 'em
	if ( ent->damage != 0 ) 
	{
		vec3_t	start, temp;
		trace_t	trace;

		VectorSubtract( ent->s.origin2, ent->r.currentOrigin, temp );
		VectorNormalize( temp );
		VectorMA( ent->r.currentOrigin, 1, temp, start );

		memset(&trace, 0, sizeof(trace_t));
		trap_Trace( &trace, start, NULL, NULL, ent->s.origin2, -1, MASK_SHOT );//ignore

		if ( trace.fraction < 1.0 )
		{
			if ( trace.entityNum < ENTITYNUM_WORLD )
			{
				gentity_t *victim = &g_entities[trace.entityNum];
				if ( victim != NULL && victim->takedamage == qtrue )
				{
					G_Damage( victim, ent, ent->activator, temp, trace.endpos, ent->damage, 0, MOD_LAVA );
				}
			}
		}
	}
}

//------------------------------------------
static void forge_bolt_use( gentity_t *self, /*@unused@*/ gentity_t *other, /*@unused@*/ gentity_t *activator )
{
	if ( self->count != 0 )
	{
		self->think = NULL;
		self->nextthink = -1;
		self->count = 0;
	}
	else
	{
		self->think = forge_bolt_think;
		self->nextthink = level.time + 200;
		self->count = 1;
	}
}

//------------------------------------------
static void forge_bolt_link( gentity_t *ent )
{
	gentity_t	*target = NULL;
	vec3_t		dir;

	if(ent->target != NULL && ent->target[0] != 0) {
		target = G_Find (target, FOFS(targetname), ent->target);
	} else {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] forge_bolt_link: ent->target is NULL\n"););
		G_FreeEntity(ent);
		return;
	}

	if (target == NULL)	{
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] forge_bolt_link: unable to find target %s\n", ent->target););
		G_FreeEntity(ent);
		return;
	}

	VectorSubtract( target->s.origin, ent->s.origin, dir );
	VectorNormalize( dir );
	vectoangles( dir, ent->s.angles );
	
	VectorCopy( target->s.origin, ent->s.origin2 );

	if ( ent->targetname != NULL && ent->targetname[0] != 0 ) {
		ent->use = forge_bolt_use;
	}

	// This is used as the toggle switch
	if((ent->spawnflags & FORGE_BOLT_STARTOFF) == 0) {
		ent->count = 1;
	} else {
		ent->count = 0;
	}

	if (ent->targetname == NULL || ent->targetname[0] == 0 || (ent->spawnflags & FORGE_BOLT_STARTOFF) == 0 ) {
		ent->think = forge_bolt_think;	
		ent->nextthink = level.time + 1000;
	} else {
		ent->think = NULL;
		ent->nextthink = -1;
	}

	trap_LinkEntity( ent );
}

//------------------------------------------
void SP_fx_forge_bolt( gentity_t *ent )
{
	ent->type = ENT_FX_FORGE_BOLT;

	G_SpawnInt( "damage", "0", &ent->damage );
	G_SpawnFloat( "random", "0.4", &ent->random );
	G_SpawnFloat( "radius", "3.0", &ent->distance );

	// See if effect is supposed to be delayed
	if ( (ent->spawnflags & FORGE_BOLT_DELAYED) != 0 ) {
		G_SpawnFloat( "wait", "2.0", &ent->wait );
	} else {
		// Effect is continuous
		ent->wait = 0.1f;
	}

	VectorCopy( ent->s.origin, ent->s.pos.trBase );
	ent->wait = (float)(level.time + 1000);

	if (ent->target != NULL && ent->target[0] != 0)	{
		ent->think = forge_bolt_link;
		ent->nextthink = level.time + 100;
		return;
	}

	trap_LinkEntity( ent );
}

/*QUAKED fx_plasma (0 0 1) (-8 -8 -8) (8 8 8) STARTOFF
-----DESCRIPTION-----
CURRENTLY DISABLED
Emits plasma jet directed from the specified point to the specified point. Jet size scales based on length.  

-----SPAWNFLAGS-----
1: STARTOFF - Effect will be off at spawn

-----KEYS-----
"target" - (required) Direction of jet
"targetname" - fires only when used
"startRGBA" - starting cone color, Red Green Blue Alpha 
	(default  100 180 255 255) Light-Blue
"finalRGBA" - final cone color, Red Green Blue Alpha 
	(default  0 0 180 0) Blue
"damage" - damage PER FRAME, default zero
*/
#define PLASMA_FX_STARTOFF 1

//------------------------------------------
static void plasma_think( gentity_t *ent )
{
	G_AddEvent( ent, EV_FX_PLASMA, 0 );
	ent->nextthink = level.time + 100;

	// If a fool gets in the plasma cone, fry 'em
	if ( ent->damage != 0 ) {
		vec3_t	start, temp;
		trace_t	trace;

		VectorSubtract( ent->s.origin2, ent->r.currentOrigin, temp );
		VectorNormalize( temp );
		VectorMA( ent->r.currentOrigin, 1, temp, start );
		
		memset(&trace, 0, sizeof(trace_t));
		trap_Trace( &trace, start, NULL, NULL, ent->s.origin2, -1, MASK_SHOT );//ignore

		if ( trace.fraction < 1.0 )
		{
			if ( trace.entityNum < ENTITYNUM_WORLD )
			{
				gentity_t *victim = &g_entities[trace.entityNum];
				if ( victim != NULL && victim->takedamage == qtrue )
				{
					G_Damage( victim, ent, ent->activator, temp, trace.endpos, ent->damage, 0, MOD_LAVA );
				}
			}
		}
	}
}

//------------------------------------------
static void plasma_use( gentity_t *self, /*@unused@*/ gentity_t *other, /*@unused@*/ gentity_t *activator)
{
	if ( self->count != 0 )	{
		self->think = NULL;	
		self->nextthink = -1;
		self->count = 0;
	} else {
		self->think = plasma_think;	
		self->nextthink = level.time + 200;
		self->count = 1;
	}
}

//------------------------------------------
static void plasma_link( gentity_t *ent )
{
	gentity_t	*target = NULL;

	if(ent->target != NULL && ent->target[0] != 0) {
		target = G_Find (target, FOFS(targetname), ent->target);
	} else {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] plasma_link: ent->target is NULL\n"););
		G_FreeEntity(ent);
		return;
	}

	if (target == NULL)
	{
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] plasma_link: unable to find target %s\n", ent->target););
		G_FreeEntity(ent);
		return;
	}

	if (ent->targetname != NULL && ent->targetname[0] != 0 ) {
		ent->use = plasma_use;
	}

	if (ent->targetname == NULL || ent->targetname[0] == 0 || (ent->spawnflags & PLASMA_FX_STARTOFF) == 0 ) {
		ent->think = plasma_think;	
		ent->nextthink = level.time + 200;
	} else {
		ent->think = NULL;
		ent->nextthink = -1;
	}

	trap_LinkEntity( ent );

	VectorCopy( target->s.origin, ent->s.origin2 );
}

//------------------------------------------
void SP_fx_plasma( gentity_t *ent )
{
	int t;

	ent->type = ENT_FX_PLASMA;

	G_SpawnVector4("startRGBA", "100 180 255 255", ent->startRGBA); 
	G_SpawnVector4("finalRGBA", "0 0 180 0", ent->finalRGBA);
	G_SpawnInt( "damage", "0", &ent->damage );

	// Convert from range of 0-255 to 0-1
	for (t=0; t < 3; t++) {
		ent->startRGBA[t] = ent->startRGBA[t] / 255.0f;
		ent->finalRGBA[t] = ent->finalRGBA[t] / 255.0f;
	}
		ent->startRGBA[3] = (float)ent->s.weapon;
		ent->finalRGBA[3] = (float)ent->s.powerups;
	
	VectorCopy( ent->s.origin, ent->s.pos.trBase );

	// This is used as the toggle switch
	if((ent->spawnflags & PLASMA_FX_STARTOFF) == 0) {
		ent->count = 1;
	} else {
		ent->count = 0;
	}
	
	trap_LinkEntity( ent );

	ent->think = plasma_link;
	ent->nextthink = level.time + 500;
}

/*QUAKED fx_energy_stream (0 0 1) (-8 -8 -8) (8 8 8) STARTOFF
-----DESCRIPTION-----
CURRENTLY DISABLED
Creates streaming particles that travel between two points--for Stasis level. ONLY orients vertically.

-----SPAWNFLAGS-----
1: STARTOFF - Effect will be off at spawn

-----KEYS-----
"damage" - amount of damage to player when standing in the stream (default 0)
"target" - (required) End point for particle stream.
"targetname" - toggle effect on/off each time used.
*/
#define ENERGY_STREAM_STARTOFF 1

//------------------------------------------
static void stream_think( gentity_t *ent )
{
	G_AddEvent( ent, EV_FX_STREAM, 0 );
	ent->nextthink = level.time + 150;

	// If a fool gets in the bolt path, zap 'em
	if ( ent->damage != 0) 
	{
		vec3_t	start, temp;
		trace_t	trace;

		VectorSubtract( ent->s.origin2, ent->r.currentOrigin, temp );
		VectorNormalize( temp );
		VectorMA( ent->r.currentOrigin, 1, temp, start );

		memset(&trace, 0, sizeof(trace_t));
		trap_Trace( &trace, start, NULL, NULL, ent->s.origin2, -1, MASK_SHOT );//ignore

		if ( trace.fraction < 1.0 )
		{
			if ( trace.entityNum < ENTITYNUM_WORLD )
			{
				gentity_t *victim = &g_entities[trace.entityNum];
				if ( victim != NULL && victim->takedamage == qtrue )
				{
					G_Damage( victim, ent, ent->activator, temp, trace.endpos, ent->damage, 0, MOD_LAVA );
				}
			}
		}
	}
}

//------------------------------------------
static void stream_use( gentity_t *self, /*@unused@*/ gentity_t *other, /*@unused@*/ gentity_t *activator)
{
	if ( self->count != 0 )	{
		self->think = stream_think;
		self->nextthink = level.time + 200;
		self->count = 0;
	} else {
		self->think = NULL;
		self->nextthink = -1;
		self->count = 1;
	}
}

//------------------------------------------
static void stream_link( gentity_t *ent )
{
	gentity_t	*target = NULL;

	if(ent->target != NULL && ent->target[0] != 0) {
		target = G_Find (target, FOFS(targetname), ent->target);
	} else {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] stream_link: ent->target is NULL\n"););
		G_FreeEntity(ent);
		return;
	}

	if (target == NULL) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] stream_link: unable to find target %s\n", ent->target););
		G_FreeEntity(ent);
		return;
	}

	VectorCopy( target->s.origin, ent->s.origin2 );
	
	if (ent->targetname == NULL || ent->targetname[0] == 0 || (ent->spawnflags & ENERGY_STREAM_STARTOFF) == 0 ) {
		ent->think = stream_think;
		ent->nextthink = level.time + 200;
	} else if ( (ent->spawnflags & 1) != 0 ) {
		ent->think = NULL;
		ent->nextthink = -1;
	}

	trap_LinkEntity( ent );
}

//------------------------------------------
void SP_fx_stream( gentity_t *ent )
{
	ent->type = ENT_FX_STREAM;

	G_SpawnInt( "damage", "0", &ent->damage );

	if (ent->targetname != NULL && ent->targetname[0] != 0) {
		ent->use = stream_use;
	}

	if((ent->spawnflags & ENERGY_STREAM_STARTOFF) == 0) {
		ent->count = 1;
	} else {
		ent->count = 0;
	}

	VectorCopy( ent->s.origin, ent->s.pos.trBase );

	ent->think = stream_link;
	ent->nextthink = level.time + 1000;

	trap_LinkEntity( ent );
}

/*QUAKED fx_transporter_stream (0 0 1) (-8 -8 -8) (8 8 8) STARTOFF
-----DESCRIPTION-----
CURRENTLY DISABLED
Creates streaming particles that travel between two points--for forge level.

-----SPAWNFLAGS-----
1: STARTOFF - Effect will be off at spawn

-----KEYS-----
"target" - (required) End point for particle stream.
"targetname" - fires only when used
*/
#define TRANSPORTER_STREAM_STARTOFF 1

//------------------------------------------
static void transporter_stream_think( gentity_t *ent )
{
	G_AddEvent( ent, EV_FX_TRANSPORTER_STREAM, 0 );
	ent->nextthink = level.time + 150;
}

//------------------------------------------
static void transporter_stream_use( gentity_t *self, /*@unused@*/ gentity_t *other, /*@unused@*/ gentity_t *activator)
{
	if ( self->count != 0) {
		self->think = transporter_stream_think;
		self->nextthink = level.time + 200;
		self->count = 0;
	} else {
		self->think = NULL;
		self->nextthink = -1;
		self->count = 1;
	}
}

//------------------------------------------
static void transporter_stream_link( gentity_t *ent )
{
	gentity_t	*target = NULL;

	if(ent->target != NULL && ent->target[0] != 0) {
		target = G_Find (target, FOFS(targetname), ent->target);
	} else {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] transporter_stream_link: ent->target is NULL\n"););
		G_FreeEntity(ent);
		return;
	}

	if (target == NULL) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] transporter_stream_link: unable to find target %s\n", ent->target););
		G_FreeEntity(ent);
		return;
	}

	if (ent->targetname != NULL && ent->targetname[0] != 0)	{
		ent->use = transporter_stream_use;
	}

	if((ent->spawnflags & 1) == 0) {
		ent->count = 1;
	} else {
		ent->count = 0;
	}

	if (ent->targetname == NULL || ent->targetname[0] == 0 || (ent->spawnflags & TRANSPORTER_STREAM_STARTOFF) == 0 )	{
		ent->think = transporter_stream_think;
		ent->nextthink = level.time + 200;
	} else {
		ent->think = NULL;
		ent->nextthink = -1;
	}

	VectorCopy( target->s.origin, ent->s.origin2 );

	trap_LinkEntity( ent );
}

//------------------------------------------
void SP_fx_transporter_stream( gentity_t *ent )
{
	ent->type = ENT_FX_TRANSPORTER_STREAM;

	VectorCopy( ent->s.origin, ent->s.pos.trBase );

	ent->think = transporter_stream_link;
	ent->nextthink = 1000;

	trap_LinkEntity( ent );
}

/*QUAKED fx_explosion_trail (0 0 1) (-8 -8 -8) (8 8 8)
-----DESCRIPTION-----
CURRENTLY DISABLED
Creates a triggerable explosion aimed at a specific point.  Always oriented towards viewer.

-----SPAWNFLAGS-----
none

-----KEYS-----
"target" - (required) end point for the explosion
"damage" - Damage per blast, default is 150. Damage falls off based on proximity.
"radius" - blast radius/explosion size (default 80)
"targetname" - triggers explosion when used
*/

//------------------------------------------
static void explosion_trail_use( gentity_t *self, /*@unused@*/ gentity_t *other, /*@unused@*/ gentity_t *activator)
{
	G_AddEvent( self, EV_FX_EXPLOSION_TRAIL, 0 );
}

//------------------------------------------
static void explosion_trail_link( gentity_t *ent )
{
	gentity_t	*target = NULL;

	ent->think = NULL;
	ent->nextthink = -1;

	if(ent->target != NULL && ent->target[0] != 0) {
		target = G_Find (target, FOFS(targetname), ent->target);
	} else {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] explosion_trail_link: ent->target is NULL\n"););
		G_FreeEntity(ent);
		return;
	}

	if (target == NULL) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] explosion_trail_link: unable to find target %s\n", ent->target););
		G_FreeEntity(ent);
		return;
	}

	VectorCopy( target->s.origin, ent->s.origin2 );

	trap_LinkEntity( ent );
}

//------------------------------------------
void SP_fx_explosion_trail( gentity_t *ent )
{
	ent->type = ENT_FX_EXPLOSION_TRAIL;

	G_SpawnInt( "damage", "150", &ent->splashDamage );
	G_SpawnFloat( "radius", "80", &ent->distance );

	VectorCopy( ent->s.origin, ent->s.pos.trBase );

	ent->use = explosion_trail_use;

	ent->think = explosion_trail_link;
	ent->nextthink = 1000;

	ent->splashRadius = 160;

	trap_LinkEntity( ent );
}

/*QUAKED fx_borg_energy_beam (0 0 1) (-8 -8 -8) (8 8 8) STARTOFF CONE
-----DESCRIPTION-----
CURRENTLY DISABLED
A borg tracing beam that either carves out a cone or swings like a pendulum, sweeping across an area. 

-----SPAWNFLAGS-----
1: STARTOFF - The trace beam will start when used.
2: CONE - Beam traces a cone, default trace shape is a pendulum, sweeping across an area.

-----KEYS-----
"radius" - Radius of the area to trace (default 30)
"speed" - How fast the tracer beam moves (default 100)
"startRGBA" - Effect color specified in RED GREEN BLUE ALPHA (default 0 255 0 128)
"target" - (required) End point for trace beam, should be placed at the very center of the trace area.
"targetname" - fires only when used
*/
#define BORG_ENERGY_BEAM_STARTOFF 1
#define BORG_ENERGY_BEAM_CONE 2

//------------------------------------------
static void borg_energy_beam_think( gentity_t *ent )
{
	G_AddEvent( ent, EV_FX_BORG_ENERGY_BEAM, 0 );
	ent->nextthink = level.time + 100;
}

//------------------------------------------
static void borg_energy_beam_use( gentity_t *self, /*@unused@*/ gentity_t *other, /*@unused@*/ gentity_t *activator)
{
	if ( self->count != 0 )	{
		self->think = borg_energy_beam_think;
		self->nextthink = level.time + 200;
		self->count = 0;
	} else {
		self->think = NULL;
		self->nextthink = -1;
		self->count = 1;
	}
}

//------------------------------------------
static void borg_energy_beam_link( gentity_t *ent )
{
	gentity_t	*target = NULL;

	if(ent->target != NULL && ent->target[0] != 0) {
		target = G_Find (target, FOFS(targetname), ent->target);
	} else {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] borg_energy_beam_link: ent->target is NULL\n"););
		G_FreeEntity(ent);
		return;
	}

	if (target == NULL) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] borg_energy_beam_link: unable to find target %s\n", ent->target););
		G_FreeEntity(ent);
		return;
	}

	if (ent->targetname != NULL && ent->targetname[0] != 0) {
		ent->use = borg_energy_beam_use;
	}

	if((ent->spawnflags & 1) == 0) {
		ent->count = 1;
	} else {
		ent->count = 0;
	}

	if (ent->targetname == NULL || ent->targetname[0] == 0 || (ent->spawnflags & BORG_ENERGY_BEAM_STARTOFF) == 0 ) {
		ent->think = borg_energy_beam_think;
		ent->nextthink = level.time + 200;
	} else {
		ent->think = NULL;
		ent->nextthink = -1;
	}

	VectorCopy( target->s.origin, ent->s.origin2 );
	VectorCopy( target->s.origin, ent->pos1 );

	trap_LinkEntity( ent );
}

//------------------------------------------
void SP_fx_borg_energy_beam( gentity_t *ent )
{
	int t;

	ent->type = ENT_FX_BORG_ENERGY_BEAM;

	G_SpawnFloat( "radius", "30", &ent->distance );
	G_SpawnFloat( "speed", "100", &ent->speed );
	G_SpawnVector4("startRGBA", "0 255 0 128", ent->startRGBA);

	// Convert from range of 0-255 to 0-1
	for (t=0; t < 4; t++) {
		ent->startRGBA[t] = ent->startRGBA[t] / 255;
	}

	VectorCopy( ent->s.origin, ent->s.pos.trBase );

	ent->think = borg_energy_beam_link;
	ent->nextthink = 1000;

	trap_LinkEntity( ent );
}

/*QUAKED fx_shimmery_thing (0 0 1) (-8 -8 -8) (8 8 8) STARTOFF TAPER 
-----DESCRIPTION-----
Creates a shimmering cone or cylinder of colored light that stretches between two points.  Looks like a teleporter type thing. 

-----SPAWNFLAGS-----
1: STARTOFF - Effect turns on when used.
2: TAPER - Cylinder tapers toward the top, creating a conical effect

-----KEYS-----
"radius" - radius of the cylinder or of the base of the cone. (default 10)
"target" - (required) End point for stream.
"targetname" - fires only when used
"wait" - how long in ms to stay on before turning itself off ( default 2 seconds (2000 ms), -1 to disable auto shut off )
*/
#define SHIMMERY_THING_STARTOFF 1
#define SHIMMERY_THING_TAPER 2

//------------------------------------------
static void shimmery_thing_think( gentity_t *ent )
{
	G_AddEvent( ent, EV_FX_SHIMMERY_THING, 0 );
	if ( ent->wait >= 0.0f ) {
		ent->nextthink = level.time + ent->wait;
	} else {
		ent->nextthink = -1;
	}
}

//------------------------------------------
static void shimmery_thing_use( gentity_t *self, /*@unused@*/ gentity_t *other, /*@unused@*/ gentity_t *activator)
{
	if ( self->count != 0 ) {
		self->think = shimmery_thing_think;
		self->nextthink = level.time + 200;
		self->count = 0;
	} else {
		self->think = NULL;
		self->nextthink = -1;
		self->count = 1;
	}
}

//------------------------------------------
static void shimmery_thing_link( gentity_t *ent )
{
	gentity_t	*target = NULL;

	if(ent->target != NULL && ent->target[0] != 0) {
		target = G_Find (target, FOFS(targetname), ent->target);
	} else {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] shimmery_thing_link: ent->target is NULL\n"););
		G_FreeEntity(ent);
		return;
	}

	if(target == NULL) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] shimmery_thing_link: unable to find target %s\n", ent->target););
		G_FreeEntity(ent);
		return;
	}

	if (ent->targetname != NULL && ent->targetname[0] != 0)	{
		ent->use = shimmery_thing_use;
	}

	if((ent->spawnflags & SHIMMERY_THING_STARTOFF) == 0) {
		ent->count = 1;
	} else {
		ent->count = 0;
	}

	if((ent->spawnflags & SHIMMERY_THING_TAPER) != 0) {
		ent->s.angles[2] = 2.0f;
	}

	if (ent->targetname == NULL || ent->targetname[0] == 0 || (ent->spawnflags & SHIMMERY_THING_STARTOFF) == 0 ) {
		ent->think = shimmery_thing_think;
		ent->nextthink = level.time + 200;
	} else {
		ent->think = NULL;
		ent->nextthink = -1;
	}

	VectorCopy( target->s.origin, ent->s.origin2 );

	trap_LinkEntity( ent );
}

//------------------------------------------
void SP_fx_shimmery_thing( gentity_t *ent )
{
	ent->type = ENT_FX_SHIMMERY_THING;

	G_SpawnFloat( "radius", "10", &ent->s.angles[1] );
	if ( ent->wait <= 0.0f ) {
		ent->wait = 2000.0f;
	}

	if ( (ent->spawnflags & 4) != 0 ) { // backwards capability for sp, keep -1 in definitions for unity
		ent->wait = -1.0f;
	}

	ent->think = shimmery_thing_link;
	ent->nextthink = level.time + 1000;

	trap_LinkEntity( ent );
}

/*QUAKED fx_borg_bolt (0 0 1) (-8 -8 -8) (8 8 8) STARTOFF NO_PROXIMITY_FX
-----DESCRIPTION-----
CURRENTLY DISABLED
Emits yellow electric bolts from the specified point to the specified point.
Emits showers of sparks if the endpoints are sufficiently close.
Has an Ugly FT-Think, so don't use unless needed

-----SPAWNFLAGS-----
STARTOFF - effect is initially off
NO_PROXIMITY_FX - Will deactivate proximity-fx associated with this. Check it if you don't use movers as else the entity thinks EVERY frame (like on borg2)

-----KEYS-----
"target" - (required) end point of the beam. Can be a func_train, info_notnull, etc.
"message" - moves start point of the beam to this ent's origin. Only useful if the beam connects 2 movers.
"targetname" - toggles effect on/off each time it's used
*/

#define BORG_BOLT_FX_STARTOFF 1
#define BORG_BOLT_FX_NO_PROXIMITY_FX 2

//------------------------------------------
static void borg_bolt_think( gentity_t *ent )
{
	if ((ent->spawnflags & BORG_BOLT_FX_NO_PROXIMITY_FX) != 0)	{
		G_AddEvent( ent, EV_FX_BORG_BOLT, 0 );
		ent->nextthink = level.time + 100 + random() * 25;
	} else {
		G_AddEvent( ent, EV_FX_BORG_BOLT, 1 );
		ent->nextthink = level.time + 50;
	}
}

//------------------------------------------
static void borg_bolt_use( gentity_t *self, /*@unused@*/ gentity_t *other, /*@unused@*/ gentity_t *activator )
{

	if ( self->count != 0) {
		self->think = NULL;
		self->nextthink = -1;
		self->count = 0;
	} else {
		self->think = borg_bolt_think;
		self->nextthink = level.time + 200;
		self->count = 1;
	}
}

//------------------------------------------
static void borg_bolt_link( gentity_t *ent )
{
	gentity_t	*target = NULL;
	gentity_t	*target2 = NULL;

	if(ent->target != NULL && ent->target[0] != 0) {
		target = G_Find (target, FOFS(targetname), ent->target);
	} else {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] borg_bolt_link: ent->target is NULL\n"););
		G_FreeEntity(ent);
		return;
	}

	if (target == NULL)	{
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] borg_bolt_link: unable to find target %s\n", ent->target););
		G_FreeEntity(ent);
		return;
	}
	VectorCopy( target->s.origin, ent->s.origin2 );

	if (ent->message != NULL && ent->message[0] != 0) {
		target2 = G_Find (target2, FOFS(targetname), ent->message);

		if (target2 == NULL) {
			DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Warning] borg_bolt_link: unable to find target2 %s falling back to using ent's origin\n", ent->target););
		} else {
			VectorCopy( target2->s.origin, ent->s.origin );
		}
	}

	if ( ent->targetname != NULL && ent->targetname[0] != 0 ) {
		ent->use = borg_bolt_use;
	}

	// This is used as the toggle switch
	if((ent->spawnflags & BORG_BOLT_FX_STARTOFF) == 0) {
		ent->count = 1;
	} else {
		ent->count = 0;
	}

	if (ent->targetname == NULL || ent->targetname[0] == 0 || (ent->spawnflags & BORG_BOLT_FX_STARTOFF) == 0 ) {
		ent->think = borg_bolt_think;	
		ent->nextthink = level.time + 1000;
	} else {
		ent->think = NULL;
		ent->nextthink = -1;
	}

	G_SoundIndex( "sound/enemies/borg/borgtaser.wav" );
	trap_LinkEntity( ent );
}

//------------------------------------------
void SP_fx_borg_bolt( gentity_t *ent )
{
	ent->type = ENT_FX_BORG_BOLT;

	ent->think = borg_bolt_link;
	ent->nextthink = level.time + 1000;

	trap_LinkEntity( ent );
}