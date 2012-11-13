#include "g_local.h"
#include "g_breakable.h"

/**
*	\brief A func_breakables health has sunk to or under zero
*
*	Removes entity entirely blow chunks.
*	If it is repairable it's not removed but made invisible.
*/
void breakable_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath ) 
{
	vec3_t		size, org, dir;
	gentity_t	*te = NULL;
	entityState_t *eState = &self->s;
	entityShared_t *eShared = &self->r;
	
	//RPG-X | GSIO01 | 09/05/2009:
	if(!(self->spawnflags & 256)) {
			eState->frame = 0; 
			self->health = 0;
	}

	self->pain = 0;
	self->die  = 0;
	self->use  = 0;

	self->takedamage = qfalse;

	if( self->target )
	{
		G_UseTargets(self, attacker);
	}

	if ( !(self->spawnflags & 4) )
	{//We don't want to stay solid
		eState->solid = 0;
		eShared->contents = 0;
		self->clipmask = 0;
		if(self->spawnflags & 256 && !strcmp(self->classname, "func_breakable")) {
			eShared->svFlags |= SVF_NOCLIENT;
			eState->eFlags |= EF_NODRAW;
		}
		trap_LinkEntity(self);	
	}

	if ( eShared->bmodel )
	{
		VectorSubtract( eShared->absmax, eShared->absmin, size );
		VectorMA( eShared->absmin, 0.5, size, org );
	}
	else
	{
		VectorSubtract( eShared->maxs, eShared->mins, size );
		VectorCopy( eShared->currentOrigin, org );
	}

	// Create a chunk effect
	te = G_TempEntity( org, EV_FX_CHUNKS );
	VectorSet( te->s.angles2, 0, 0, 1 ); // FIXME: temp direction
	te->s.time2 = VectorNormalize( size );
	te->s.powerups = eState->powerups;

	// Ok, we are allowed to explode, so do it now!
	if ( self->splashDamage > 0 && self->splashRadius > 0 )
	{
		//fixme: what about chain reactions?
		G_RadiusDamage( org, attacker, self->splashDamage, self->splashRadius, self, DAMAGE_RADIUS|DAMAGE_ALL_TEAMS, MOD_EXPLOSION );

		//explosion effect
		te = G_TempEntity( org, EV_MISSILE_MISS );
		VectorSet( dir, 0, 0, 1 );
		te->s.eventParm = DirToByte( dir );
		te->s.weapon = WP_8;
	}

	if ( eShared->bmodel )
	{
		trap_AdjustAreaPortalState( self, qtrue );
	}
	else if ( eState->modelindex2 != -1 && !(self->spawnflags & 8) )
	{
		eState->modelindex = self->s.modelindex2;
		return;
	}
	
	if(!(self->spawnflags & 256))
		G_FreeEntity( self );
	if(self->spawnflags & 256)
		self->count = 0;
}

/**
*	Called when a breakable takes damage
*/
void breakable_pain ( gentity_t *self, gentity_t *attacker, int damage )
{
	if ( self->pain_debounce_time > level.time )
	{
		return;
	}

	if ( self->paintarget )
	{
		G_UseTargets2 ( self, self->activator, self->paintarget );
	}

	if(self->wait == -1)
	{
		self->pain = 0;
		return;
	}

	self->pain_debounce_time = level.time + self->wait;
}

/**
*	Called if a brealable has been used
*/
void breakable_use (gentity_t *self, gentity_t *other, gentity_t *activator)
{
	breakable_die( self, other, activator, self->health, MOD_UNKNOWN );
}

/**
*	Inits a breakable brush entity
*/
void InitBBrush ( gentity_t *ent ) 
{
	float		light;
	vec3_t		color;
	qboolean	lightSet, colorSet;
	entityState_t *eState = &ent->s;

	VectorCopy( eState->origin, ent->pos1 );
	
	trap_SetBrushModel( ent, ent->model );

	ent->die = breakable_die;
	
	// if the "model2" key is set, use a seperate model
	// for drawing, but clip against the brushes
	if ( ent->model2 ) 
	{
		eState->modelindex2 = G_ModelIndex( ent->model2 );
	}

	// if the "color" or "light" keys are set, setup constantLight
	lightSet = G_SpawnFloat( "light", "100", &light );
	colorSet = G_SpawnVector( "color", "1 1 1", color );
	if ( lightSet || colorSet ) 
	{
		int		r, g, b, i;

		r = color[0] * 255;
		if ( r > 255 ) {
			r = 255;
		}
		g = color[1] * 255;
		if ( g > 255 ) {
			g = 255;
		}
		b = color[2] * 255;
		if ( b > 255 ) {
			b = 255;
		}
		i = light / 4;
		if ( i > 255 ) {
			i = 255;
		}
		eState->constantLight = r | ( g << 8 ) | ( b << 16 ) | ( i << 24 );
	}

	eState->eType = ET_MOVER;
	trap_LinkEntity (ent);

	eState->pos.trType = TR_STATIONARY;
}

//RPG-X | GSIO01 | 09/05/2009 SOE
/**
*	Gets called if someone is inside a breakables trigger.
*	Breables only have trigges if they are repairable
*	This function sets the touched gentity_t pointer to the tirgger
*	so it is possible to check if the player is inside the trigger
*	and there fore can repair the breakable.
*
*	\author Ubergames - GSIO01
*	\date 09/05/2009
*/
void Touch_breakable_trigger(gentity_t *ent, gentity_t *other, trace_t *trace) {
	other->touched = ent;
}

/**
*	Spawns a trigger for a breakable.
*	Only gets called if the breakable is repairable.
*/
void breakable_spawn_trigger(gentity_t *ent) {
	vec3_t maxs, mins;
	gentity_t *other;
	int best, i;
	entityShared_t *eShared;

	VectorCopy(ent->r.absmin, mins);
	VectorCopy(ent->r.absmax, maxs);

	for(other = ent->teamchain; other; other=other->teamchain) {
		eShared = &other->r;
		AddPointToBounds(eShared->absmin, mins, maxs);
		AddPointToBounds(eShared->absmax, mins, maxs);
	}

	best = 0;

	for(i = 1; i < 3; i++) {
		if(maxs[i] - mins[i] < maxs[best] - mins[best])
			best = i;
	}

	maxs[best] += 48;
	mins[best] -= 48;

	other = G_Spawn();

	if(!other) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] Unable to spawn trigger for func_breakable at %s!\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}

	eShared = &other->r;

	VectorCopy(maxs, eShared->maxs);
	VectorCopy(mins, eShared->mins);

	eShared->contents = CONTENTS_TRIGGER;
	other->touch = Touch_breakable_trigger;
	other->count = best;
	other->touched = ent;
	other->target = ent->target;
	ent->touched = other;

	trap_LinkEntity(other);
}

//RPG-X | GSIO01 | 09/05/2009 EOE

/*QUAKED func_breakable (0 .8 .5) ? x x x x INVINCIBLE x x x REPAIRABLE NOORIGIN
INVINCIBLE - can only be broken by being used
REPAIRABLE - con be repaired with hyperspanner

When destroyed, fires it's trigger and explodes

"targetname" entities with matching target will fire it
"paintarget" target to fire when hit (but not destroyed)
"wait"		how long minimum to wait between firing paintarget each time hit
"model2"	.md3 model to also draw
"target"	all entities with a matching targetname will be used when this is destoryed
"health"	default is 10

"team" - This cannot take damage from members of this team (2 = blue, 1 = red)

Damage: default is none
"splashDamage" - damage to do (will make it explode on death
"splashRadius" - radius for above damage

"material" - sets the chunk type:
 0 - none (default)
 1 - metal
 2 - glass
 3 - glass and metal
 4 - wood
 5 - stone

Don't know if these work:  
"color"		constantLight color
"light"		constantLight radius

*/
/**
*	Spawnfunction for func_breakable entity.
*/
void SP_func_breakable( gentity_t *self ) 
{
	if(!(self->spawnflags & 1))
	{
		if(!self->health)
		{
			self->health = 10;
		}
	}

	if (self->health) 
	{
		self->takedamage = qtrue; //RPG-X - TiM: qtrue
	}

	//RPG-X | GSIO01 | 09/05/2009: store max health for repairing
	self->damage = self->health;

	G_SoundIndex("sound/weapons/explosions/cargoexplode.wav");//precaching
	self->use = breakable_use;
	self->count = 1; // GSIO01

	if ( self->paintarget )
	{
		self->pain = breakable_pain;
	}

	if (!self->model) {
		G_Error("func_breakable with NULL model\n");
	}
	VectorCopy(self->s.origin, self->pos1);
	trap_LinkEntity(self);
	InitBBrush( self );

	level.numBrushEnts++;
}

/*QUAKED misc_model_breakable (1 0 0) (-16 -16 -16) (16 16 16) SOLID AUTOANIMATE DEADSOLID NO_DMODEL INVINCIBLE x x x REPAIRABLE
SOLID - Movement is blocked by it, if not set, can still be broken by explosions and shots if it has health
AUTOANIMATE - Will cycle it's anim
DEADSOLID - Stay solid even when destroyed (in case damage model is rather large).
NO_DMODEL - Makes it NOT display a damage model when destroyed, even if one exists
INVINCIBLE - Can only be broken by being used

"model"		arbitrary .md3 file to display
"health"	how much health to have - default is zero (not breakable)  If you don't set the SOLID flag, but give it health, it can be shot but will not block NPCs or players from moving
"targetname" when used, dies and displays damagemodel, if any (if not, removes itself)
"target" What to use when it dies
"paintarget" target to fire when hit (but not destroyed)
"wait"		how long minimum to wait between firing paintarget each time hit

Damage: default is none
"splashDamage" - damage to do (will make it explode on death)
"splashRadius" - radius for above damage

"team" - This cannot take damage from members of this team (2 = blue, 1 = red)

"material" - sets the chunk type:
 0 - none (default)
 1 - metal
 2 - glass
 3 - glass and metal
 4 - wood
 5 - stone

FIXME/TODO: 
set size better?
multiple damage models?
don't throw chunks on pain, or throw level 1 chunks only on pains?
custom explosion effect/sound?
*/
/**
*	Spawnfunction for misc_model_breakable entity.
*/
void SP_misc_model_breakable( gentity_t *ent ) 
{
	char	damageModel[MAX_QPATH];
	int		len;
	entityShared_t *eShared = &ent->r;
	entityState_t *eState = &ent->s;
	
	//Main model
	eState->modelindex = ent->sound2to1 = G_ModelIndex( ent->model );

	if ( ent->spawnflags & 1 )
	{//Blocks movement
		eShared->contents = CONTENTS_BODY;	//Was CONTENTS_SOLID, but only architecture should be this
	}
	else if ( ent->health )
	{//Can only be shot
		eShared->contents = CONTENTS_SHOTCLIP;
	}

	ent->damage = ent->health;

	ent->use = breakable_use;	

	if ( ent->health ) 
	{
		G_SoundIndex("sound/weapons/explosions/cargoexplode.wav");
		ent->takedamage = qtrue;
		ent->pain = breakable_pain;
		ent->die = breakable_die;
	}

	len = strlen( ent->model ) - 4;
	strncpy( damageModel, ent->model, len );
	damageModel[len] = 0;	//chop extension
	
	if (ent->takedamage) {
		//Dead/damaged model
		if( !(ent->spawnflags & 8) ) {	//no dmodel
			strcat( damageModel, "_d1.md3" );
			eState->modelindex2 = G_ModelIndex( damageModel );
		}
	}

	if ( !eShared->mins[0] && !eShared->mins[1] && !eShared->mins[2] )
	{
		VectorSet (eShared->mins, -16, -16, -16);
	}
	if ( !eShared->maxs[0] && !eShared->maxs[1] && !eShared->maxs[2] )
	{
		VectorSet (eShared->maxs, 16, 16, 16);
	}

	if ( ent->spawnflags & 2 )
	{
		eState->eFlags |= EF_ANIM_ALLFAST;
	}

	G_SetOrigin( ent, eState->origin );
	VectorCopy( eState->angles, eState->apos.trBase );

	trap_LinkEntity (ent);
}

//-------------------------------------------------------------------------------
// --------------------------------------------------------------------
//
//   AMMO plugin functions
//
// --------------------------------------------------------------------
void ammo_use( gentity_t *self, gentity_t *other, gentity_t *activator);

//!give a player ammo for a weapon 
static int Add_Ammo2 (gentity_t *ent, int weapon, int count)
{
	playerState_t *ps = &ent->client->ps;

	ps->ammo[weapon] += count;

	if ( ps->ammo[weapon] > Max_Weapon(weapon) )
	{
		ps->ammo[weapon] = Max_Weapon(weapon);
		return qfalse;
	}
	return qtrue;
}

//!Shuts down a ammo station
void ammo_shutdown( gentity_t *self )
{
	if (!(self->s.eFlags & EF_ANIM_ONCE))
	{
		self->s.eFlags &= ~ EF_ANIM_ALLFAST;
		self->s.eFlags |= EF_ANIM_ONCE;

		trap_LinkEntity (self);
	}
}

//!Fades out a ammo station
void ammo_fade_out( gentity_t *ent )
{
	G_Sound( ent, G_SoundIndex( "sound/items/respawn1.wav" ) );
	ent->s.eFlags |= EF_ITEMPLACEHOLDER;
	ent->s.eventParm = -1;
	ent->think = G_FreeEntity;
	ent->nextthink = level.time + 1000;
}

//! Think function for the ammo station
void ammo_think( gentity_t *ent )
{
	int dif = 0;
	int	i;

	// Still has ammo to give
	if ( ent->enemy && ent->enemy->client )
	{
		//assume that we'll finish here, if we don't, it will be overridden
		ent->use = ammo_use;	
		ent->think = 0;//qvm complains about using NULL
		ent->nextthink = -1;
		for ( i = 0; i < WP_NUM_WEAPONS && ent->count > 0; i++ )
		{//go through all weapons
			if ( (ent->enemy->client->ps.stats[STAT_WEAPONS]&( 1 << i )) )
			{//has this weapon
				dif = Max_Weapon(i) - ent->enemy->client->ps.ammo[i];//needs ammo?

				if (dif > 2 )
				{
					dif= 2;
				}
				else if (dif < 0) 
				{
					dif= 0;	
				}

				if (ent->count < dif)	// Can't give more than count
				{
					dif = ent->count;
				}

				// Give player ammo 
				if (Add_Ammo2(ent->enemy,i,dif) && (dif!=0))	
				{
					ent->count-=dif;
					if ( ent->splashDamage )
					{
						ent->splashDamage = floor((float)ent->count/10);
					}
					ent->use = 0; /*NULL*/
					ent->think = ammo_think;
					ent->nextthink = level.time + 10;
				}
				else	// User has taken all ammo he can hold
				{
				}
			}
		}
	}

	if (ent->count < 1)
	{
		ammo_shutdown(ent);
		ent->think = ammo_fade_out;
		ent->nextthink = level.time + 3000;
	}
}

//------------------------------------------------------------
//! use function for a ammo station
void ammo_use( gentity_t *self, gentity_t *other, gentity_t *activator)
{
	int dif = 0;
	int	i;

	if (self->think != NULL)
	{
		if (self->use != NULL)
		{
			self->think = 0; /*NULL*/
			self->nextthink = -1;
		}
	}
	else
	{
		if ( other && other->client )
		{
			for ( i = 0; i < WP_NUM_WEAPONS && dif == 0; i++ )
			{//go through all weapons
				if ( (other->client->ps.stats[STAT_WEAPONS]&( 1 << i )) )
				{//has this weapon
					dif = Max_Weapon(i) - other->client->ps.ammo[i];//needs ammo?
				}
			}
		}
		else
		{	// Being triggered to be used up
			dif = 1;
			self->count = 0;
		}

		// Does player already have full ammo?
		if ( dif > 0 )
		{
			G_Sound(self, G_SoundIndex("sound/player/suitenergy.wav") );

			if ((dif >= self->count) || (self->count<1)) // use it all up?
			{
				ammo_shutdown(self);
			}
		}	
		else
		{
			G_Sound(self, G_SoundIndex("sound/weapons/noammo.wav") );
		}
		// Use target when used
		if (self->spawnflags & 8)
		{
			G_UseTargets( self, activator );	
		}

		self->use = 0; /*NULL*/
		self->enemy = other;
		self->think = ammo_think;
		self->nextthink = level.time + 50;
	}	
}

/**
*	Finishs off the spawning of an ammo station.
*/
void ammo_station_finish_spawning ( gentity_t *self )
{
	self->s.eFlags &= ~EF_ITEMPLACEHOLDER;
	self->think = 0; /*NULL*/
	self->nextthink = -1;
	self->use = ammo_use;
}
//------------------------------------------------------------
/*QUAKED misc_ammo_station (1 0 0) (-16 -16 -16) (16 16 16) 

"health" - how much health the model has - default 60 (zero makes non-breakable)
"target" - what to use when it dies
"paintarget" - target to fire when hit (but not destroyed)
"count" - the amount of health given when used (default 1000)

"team" - This cannot take damage from members of this team and only members of this team can use it (2 = blue, 1 = red)
*/
//------------------------------------------------------------
/**
* Spawnfunction for misc_ammo_station entity
*/
void SP_misc_ammo_station( gentity_t *ent )
{
	if (!ent->health)
	{
		ent->health = 60;
	}

	if ( !ent->count )
	{
		ent->count = 1000;
	}

	ent->s.powerups = 3;//material

	if ( ent->team && atoi(ent->team) == 1 )
	{
		ent->s.modelindex = G_ModelIndex( "models/mapobjects/dn/powercell2.md3" );
	}
	else
	{
		ent->s.modelindex = G_ModelIndex( "models/mapobjects/dn/powercell.md3" );
	}
	ent->r.contents = CONTENTS_CORPSE;

	// Set a generic use function
	ent->use = ammo_use;	
	G_SoundIndex( "sound/player/suitenergy.wav" );

	if ( ent->health ) 
	{
		ent->takedamage = qtrue;
		ent->pain = breakable_pain;
		ent->die  = breakable_die;
	}

	//FIXME: if set at an angle (say, on a floor), BBOX will be wrong
	VectorSet( ent->r.mins, -16, -16, -16 );
	VectorSet( ent->r.maxs, 16, 16, 16 );
	G_SetOrigin( ent, ent->s.origin );
	VectorCopy( ent->s.angles, ent->s.apos.trBase );
	trap_LinkEntity (ent);
}


//RPG-X | GSIO01 | 09/05/2009 SOE
/*QUAKED target_repair (1 0 0) (-8 -8 -8) (8 8 8)
Repairs a func_breakable.

"target" breakable to repair (targetname2)
*/
/**
*	Use function of target_repair entity.
*	Repairs it's target entity (stored in ent->lastEnemy)
*	if it is damaged.
*/
void target_repair_use(gentity_t *ent, gentity_t *other, gentity_t *activator) {
	gentity_t *target;

	target = ent->lastEnemy;

	if(!(target->spawnflags & 256)) {
		return;
	}

	target->r.contents = CONTENTS_BODY;

	trap_SetBrushModel(target, target->model);

	target->r.svFlags &= ~SVF_NOCLIENT;
	target->s.eFlags &= ~EF_NODRAW;
	InitBBrush(target);

	target->health = target->damage;
	target->takedamage = qtrue;
	target->use = breakable_use;

	if(target->paintarget)
		target->pain = breakable_pain;

	target->clipmask = 0;
	target->count = 1;
}

/**
*	Link function finishes off spawning of the entity.
*/
void target_repair_link(gentity_t *ent) {
	gentity_t *target;

	ent->nextthink = -1;

	target = G_Find(NULL, FOFS(targetname), ent->target);
	if(!target) {
		target = G_Find(NULL, FOFS(targetname2), ent->target);
		if(!target) {
			DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_repair at %s with an unfound target: %s\n", vtos(ent->s.origin), ent->target););
			return;
		}
	}

	ent->lastEnemy = target;

	if(Q_stricmp(target->classname, "func_breakable")) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_repair at %s with an invalid target entity %s\n", vtos(ent->s.origin), target->classname););
		return;
	}

	ent->use = target_repair_use;
}

/**
*	Spawn function of target_repair entity
*/
void SP_target_repair(gentity_t *ent) {
	if(!ent->target) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] target_repair without target at %s\n", vtos(ent->s.origin)););
		return;
	}

	ent->think = target_repair_link;
	ent->nextthink = level.time + 1000; // give the breakables some time so init
}
//RPG-X | GSIO01 | 09/05/2009 EOE
