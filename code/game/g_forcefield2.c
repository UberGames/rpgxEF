#include "g_local.h"

extern void	InitMover( gentity_t *ent );

extern gentity_t	*G_TestEntityPosition( gentity_t *ent );
void func_forcefield2_use (gentity_t *self, gentity_t *other, gentity_t *activator);
void func_forcefield2_wait_return_solid( gentity_t *self )
{
	//once a frame, see if it's clear.
	self->clipmask = CONTENTS_BODY;
	if ( !(self->spawnflags&16) || G_TestEntityPosition( self ) == NULL )
	{
		trap_SetBrushModel( self, self->model );
		InitMover( self );
		VectorCopy( self->s.origin, self->s.pos.trBase );
		VectorCopy( self->s.origin, self->r.currentOrigin );
		self->r.svFlags &= ~SVF_NOCLIENT;
		self->s.eFlags &= ~EF_NODRAW;
		self->use = func_forcefield2_use;
		self->clipmask = 0;
		/*
		if ( self->s.eFlags & EF_ANIM_ONCE )
		{//Start our anim
			self->s.frame = 0;
		}
		*/
		if ( !(self->spawnflags&1) )
		{//START_OFF doesn't effect area portals
			trap_AdjustAreaPortalState( self, qfalse );
		}
	}
	else
	{
		self->clipmask = 0;
		self->think = func_forcefield2_wait_return_solid;
		self->nextthink = level.time + FRAMETIME;
	}
}

void func_forcefield2_think( gentity_t *self )
{
	if ( self->spawnflags & 8 )
	{
		//self->r.svFlags |= SVF_PLAYER_USABLE;	//Replace the usable flag
		self->use = func_forcefield2_use;
		self->think = 0/*NULL*/;
		self->nextthink = -1;
	}
}

void func_forcefield2_use (gentity_t *self, gentity_t *other, gentity_t *activator)
{//Toggle on and off

	if ( !self->count )
	{//become solid again
		self->count = 1;
		func_forcefield2_wait_return_solid( self );
	}
	else
	{
		self->s.solid = 0;
		self->r.contents = 0;
		self->clipmask = 0;
		self->r.svFlags |= SVF_NOCLIENT;
		self->s.eFlags |= EF_NODRAW;
		self->count = 0;

		if(self->target && self->target[0])
		{
			G_UseTargets(self, activator);
		}
		self->think = 0/*NULL*/;
		self->nextthink = -1;
		if ( !(self->spawnflags&1) )
		{//START_OFF doesn't effect area portals
			trap_AdjustAreaPortalState( self, qtrue );
		}
	}
}

void func_forcefield2_pain(gentity_t *self, gentity_t *attacker, int damage)
{
	self->use( self, attacker, attacker );
}

void func_forcefield2_die(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod)
{
	self->takedamage = qfalse;
	self->use( self, inflictor, attacker );
}

/*
QUAKED func_forcefield2 (0 .5 .8) ? STARTOFF AUTOANIM x ALWAYS_ON NOBLOCKCHECK x x x
START_OFF - the forcefield will not be there
AUTOANIM - If useing an md3, it will animate
ALWAYS_ON - Doesn't toggle on and off when used, just fires target
NOBLOCKCHECK - Will NOT turn on while something is inside it unless this is checked

A bmodel that can be used directly by the player's "activate" button

"targetname" - When used, will toggle on and off
"target"	Will fire this target every time it is toggled OFF
"model2"	.md3 model to also draw
"color"		constantLight color
"light"		constantLight radius
"wait"		amount of time before the object is usable again (only valid with ALWAYS_ON flag)
"health"	if it has health, it will be used whenever shot at/killed - if you want it to only be used once this way, set health to 1
*/

void SP_func_forcefield2( gentity_t *self ) 
{
	trap_SetBrushModel( self, self->model );
	InitMover( self );
	VectorCopy( self->s.origin, self->s.pos.trBase );
	VectorCopy( self->s.origin, self->r.currentOrigin );

	self->count = 1;
	if (self->spawnflags & 1)
	{
		self->s.solid = 0;
		self->r.contents = 0;
		self->clipmask = 0;
		self->r.svFlags |= SVF_NOCLIENT;
		self->s.eFlags |= EF_NODRAW;
		self->count = 0;
	}

	if (self->spawnflags & 2)
	{
		self->s.eFlags |= EF_ANIM_ALLFAST;
	}

	/*
	if (self->spawnflags & 4)
	{//FIXME: need to be able to do change to something when it's done?  Or not be usable until it's done?
		self->s.eFlags |= EF_ANIM_ONCE;
	}
	*/

	self->use = func_forcefield2_use;

	if ( self->health )
	{
		self->takedamage = qtrue;
		self->die = func_forcefield2_die;
		self->pain = func_forcefield2_pain;
	}

	trap_LinkEntity (self);

	self->team = "3";
}
