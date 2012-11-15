// Copyright (C) 1999-2000 Id Software, Inc.
//
// g_combat.c

#include "g_local.h"
#include "g_breakable.h" //RPG-X | GSIO01 | 09/05/2009: needed by G_Repair

extern int	numKilled;
extern void SetClass( gentity_t *ent, char *s, char *teamName, qboolean SaveToCvar );


/*
============
AddScore

Adds score to both the client and his team
============
*/
void AddScore( gentity_t *ent, int score ) {
	if ( !ent )
	{
		return;
	}
	if ( !ent->client ) {
		return;
	}

	if(!ent->client->UpdateScore)
	{
		return;
	}

	ent->client->ps.persistant[PERS_SCORE] += score;
	//don't add score to team score during elimination
	if (g_gametype.integer == GT_TEAM)
	{//this isn't capture score
		level.teamScores[ ent->client->ps.persistant[PERS_TEAM] ] += score;
	}
	CalculateRanks( qfalse );

	//RPG-X: RedTechie - Lets enable score updating without this scores will not be updated
	ent->client->UpdateScore = qfalse;
}

/*
============
SetScore

============
*/
void SetScore( gentity_t *ent, int score ) {

	if ( !ent )
	{
		return;
	}
	if ( !ent->client ) {
		return;
	}

	if(!ent->client->UpdateScore)
	{
		return;
	}

	ent->client->ps.persistant[PERS_SCORE] = score;
	CalculateRanks( qfalse );

	// TiM: send the current scoring to all clients
	SendScoreboardMessageToAllClients();

	//RPG-X: RedTechie - Lets enable score updating without this scores will not be updated
	ent->client->UpdateScore = qfalse;
}
/*
=================
TossClientItems

Toss the weapon and powerups for the killed player
=================
*/
void TossClientItems( gentity_t *self, qboolean dis_con ) {
	gitem_t		*item;
	float		angle;
	int			i;
	int			times;
	gentity_t	*drop;
	playerState_t *ps = &self->client->ps;

	if (self->flags & FL_CLOAK) {
		// remove the invisible powerup if the player is cloaked.
		//RPG-X: RedTechie - Also remove ghost
		ps->powerups[PW_GHOST] = level.time;
		ps->powerups[PW_INVIS] = level.time;
	} 
	if (self->flags & FL_FLY) {
		// remove the flying powerup if the player is flying.
		ps->powerups[PW_FLIGHT] = level.time;
	} 
	//RPG-X | Phenix | 8/8/2004
	if (self->flags & FL_EVOSUIT) {
		// remove the evosuit powerup
		ps->powerups[PW_EVOSUIT] = level.time;
	}

	// drop all the powerups if not in teamplay
	if ( g_gametype.integer != GT_TEAM ) {
		angle = 45;
		for ( i = 1 ; i < PW_NUM_POWERUPS ; i++ ) {
			if ( ps->powerups[ i ] > level.time ) {
				item = BG_FindItemForPowerup( i );
				if ( !item ) {
					continue;
				}

				drop = Drop_Item( self, item, angle );
				// decide how many seconds it has left
				drop->count = ( ps->powerups[ i ] - level.time ) / 1000;
				if ( drop->count < 1 ) {
					drop->count = 1;
				}
				angle += 45;
			}
		}
	}

	// RPG-X | Marcin | 30/12/2008
	// ...
	if (!rpg_allowWeaponDrop.integer || !rpg_dropOnDeath.integer || dis_con) {
		return;
	}

	// Drop ALL weapons in inventory
	for (i = 0; i < WP_NUM_WEAPONS; ++i) {
		// these weapons should not be tossed (hand and null)
		if ( Max_Weapons[i] == NULL) {
			continue;
		}

		//RPG-X | GSIO01 | 08/05/2009: let's make sure we only drop weapons the player has
		item = NULL;
		if(ps->ammo[i]) {
			times = ps->ammo[i];
			item = BG_FindItemForWeapon((weapon_t)i);
			while ( times --> 0 ) { // the 'goes towards' operator :p
				Drop_Item( self, item, 0 );
			}
		}
	}

	// then remove weapons

	for (i = 0; i < WP_NUM_WEAPONS; ++i) {
		ps->stats[STAT_WEAPONS] &= ~i;
		ps->ammo[i] = 0;
	}


}

/*
==================
GibEntity
==================
*/
static void GibEntity( gentity_t *self, int killer ) {
	// Start Disintegration
	G_AddEvent( self, EV_EXPLODESHELL, killer );
	self->takedamage = qfalse;
	self->s.eType = ET_INVISIBLE;
	self->r.contents = 0;
}

/*
==================
body_die
==================
*/
void body_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath ) {
	int			contents;
	contents = trap_PointContents( self->r.currentOrigin, -1 );
	if(rpg_medicsrevive.integer == 1 && !( contents & CONTENTS_NODROP ) && (meansOfDeath != MOD_TRIGGER_HURT)){
		if ( self->health > GIB_HEALTH_IMPOSSIBLE ) {
			return;
		}

	}else{
		if ( self->health > GIB_HEALTH ) {
			return;
		}
	}
	GibEntity( self, 0 );
}


// these are just for logging, the client prints its own messages
char	*modNames[MOD_MAX] = {
	"MOD_UNKNOWN",

	"MOD_WATER",
	"MOD_SLIME",
	"MOD_LAVA",
	"MOD_CRUSH",
	"MOD_TELEFRAG",
	"MOD_FALLING",
	"MOD_SUICIDE",
	"MOD_TARGET_LASER",
	"MOD_TRIGGER_HURT",

	// Trek weapons
	"MOD_PHASER",
	"MOD_PHASER_ALT",
	"MOD_CRIFLE",
	"MOD_CRIFLE_SPLASH",
	"MOD_CRIFLE_ALT",
	"MOD_CRIFLE_ALT_SPLASH",
	"MOD_IMOD",
	"MOD_IMOD_ALT",
	"MOD_SCAVENGER",
	"MOD_SCAVENGER_ALT",
	"MOD_SCAVENGER_ALT_SPLASH",
	"MOD_STASIS",
	"MOD_STASIS_ALT",
	"MOD_GRENADE",
	"MOD_GRENADE_ALT",
	"MOD_GRENADE_SPLASH",
	"MOD_GRENADE_ALT_SPLASH",
	"MOD_TETRYON",
	"MOD_TETRYON_ALT",
	"MOD_DREADNOUGHT",
	"MOD_DREADNOUGHT_ALT",
	"MOD_QUANTUM",
	"MOD_QUANTUM_SPLASH",
	"MOD_QUANTUM_ALT",
	"MOD_QUANTUM_ALT_SPLASH",

	"MOD_DETPACK",
	"MOD_SEEKER"

	//expansion pack
	"MOD_KNOCKOUT",
	"MOD_ASSIMILATE",
	"MOD_BORG",
	"MOD_BORG_ALT",

	"MOD_RESPAWN",
	"MOD_EXPLOSION",
};//must be kept up to date with bg_public, meansOfDeath_t

extern void DetonateDetpack(gentity_t *ent);

/*
==================
player_die
Heavly Modifyed By: RedTechie
RPG-X: Marcin: a little bit modified - 30/12/2008
==================
*/
extern char *ClassNameForValue( pclass_t pClass );
extern qboolean IsAdmin( gentity_t *ent);
void player_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath ) {
	//---------------------
	//RPG-X: RedTechie - Check to see if medics revive people and not respawn if true use my fake death insead :)
	//---------------------
	int			contents;

	//RPG-X: RedTechie - Make sure there not getting killed by a trigger kill or the medics wont be able to heal them
	contents = trap_PointContents( self->r.currentOrigin, -1 );

	if(rpg_medicsrevive.integer == 1 && !( contents & CONTENTS_NODROP ) && (meansOfDeath != MOD_TRIGGER_HURT)){
		int			anim;
		char		*classname = NULL;
		gentity_t	*detpack = NULL;
		int			killer;
		char		*killerName, *obit;
		gentity_t	*ent;
		int			i;
		playerState_t *ps = &self->client->ps;

		//RPG-X: RedTechie - Blow up a detpack if some one placed it and died 
		classname = BG_FindClassnameForHoldable(HI_DETPACK);
		if (classname)
		{
			while ((detpack = G_Find (detpack, FOFS(classname), classname)) != NULL)
			{
				if (detpack->parent == self)
				{
					detpack->think = DetonateDetpack;		// Detonate next think.
					detpack->nextthink = level.time;
				}
			}
		}


		//RPG-X: Redtechie - Do some score keeping witch we commented out and log
		if ( attacker ) {
			killer = attacker->s.number;
			if ( attacker->client ) {
				killerName = attacker->client->pers.netname;
			} else {
				killerName = "<non-client>";
			}
		} else {
			killer = ENTITYNUM_WORLD;
			killerName = "<world>";
		}

		if ( killer < 0 || killer >= MAX_CLIENTS ) {
			killer = ENTITYNUM_WORLD;
			killerName = "<world>";
		}

		if ( meansOfDeath < 0 || meansOfDeath >= sizeof( modNames ) / sizeof( modNames[0] ) ) {
			obit = "<bad obituary>";
		} else {
			obit = modNames[ meansOfDeath ];
		}

		G_LogPrintf("Kill: %i %i %i: %s killed %s by %s\n", 
			killer, self->s.number, meansOfDeath, killerName, 
			self->client->pers.netname, obit );

		G_LogWeaponKill(killer, meansOfDeath);
		G_LogWeaponDeath(self->s.number, self->s.weapon);
		if (attacker && attacker->client && attacker->inuse)
		{
			G_LogWeaponFrag(killer, self->s.number);
		}

		if ( meansOfDeath != MOD_RESPAWN && meansOfDeath != MOD_CUSTOM_DIE )
		{
			// broadcast the death event to everyone
			ent = G_TempEntity( self->r.currentOrigin, EV_OBITUARY );
			ent->s.eventParm = meansOfDeath;
			ent->s.otherEntityNum = self->s.number;
			ent->s.otherEntityNum2 = killer;
			ent->r.svFlags = SVF_BROADCAST;	// send to everyone
		}

		self->enemy = attacker;

		ps->persistant[PERS_KILLED]++;
		if (self == attacker)
		{
			self->client->pers.teamState.suicides++;
		} else {
			//RPG-X | Phenix | 06/04/2005
			// N00b Protection, you kill two people and puff your auto n00b!

			if ( attacker ) {
				if ( attacker->client ) {
					if ( IsAdmin( attacker ) == qfalse ) {
						attacker->n00bCount++;

						attacker->client->fraggerTime = level.time + (rpg_fraggerSpawnDelay.integer * 1000);

						if (rpg_kickAfterXkills.integer < 1)
						{
							trap_SendServerCommand( attacker-g_entities, va("print \"^7Server: You have been caught n00bing, you have been temporary put in the n00b class.\n\"" ) );
						} else {
							trap_SendServerCommand( attacker-g_entities, va("print \"^7Server: You have been caught n00bing, %i more times and you will be kicked.\n\"", (rpg_kickAfterXkills.integer - attacker->n00bCount) ) );
						}

						if ((attacker->n00bCount >= rpg_kickAfterXkills.integer) && (rpg_kickAfterXkills.integer != 0))
						{
							trap_DropClient( attacker->s.number, "Kicked: Do Not N00b!" );
						} else {
							for( i=0; g_classData[i].consoleName[0] && i < MAX_CLASSES; i++ ) 
							{
								if ( g_classData[i].isn00b) {
									char	conName[64];
									trap_Cvar_VariableStringBuffer( va( "rpg_%sPass", conName ), conName, sizeof(conName) ); 

									Q_strncpyz(attacker->client->origClass, ClassNameForValue( attacker->client->sess.sessionClass ), sizeof(attacker->client->origClass));
									attacker->client->n00bTime = level.time + 10000;
									SetClass( attacker, conName, NULL, qfalse );
									break;
								}
							}
						}
					}
				}
			}
		}

		//RPG-X: RedTechie no noclip
		if ( self->client->noclip ) {
			self->client->noclip = qfalse;
		}

		//RPG-X: RedTechie - Toss items
		//RPG-X: Marcin - not when respawning - 30/12/2008
		if ( meansOfDeath != MOD_RESPAWN ) {
			TossClientItems( self, qfalse );
		}

		ps->pm_type = PM_DEAD;

		self->takedamage = qfalse;

		ps->weapon = WP_0;
		ps->weaponstate = WEAPON_READY;
		self->r.contents = CONTENTS_CORPSE;

		//-TiM

		self->s.loopSound = 0;

		self->r.maxs[2] = -8;

		//RPG-X: RedTechie - Wait....forever
		self->client->respawnTime = level.time + 1000000000;

		//Clear powerups
		//TiM - Bookmark
		//memset( ps->powerups, 0, sizeof(ps->powerups) );

		//Play death sound
		//RPG-X: RedTechie - No pain sound when they change class
		if(meansOfDeath != MOD_RESPAWN){
			G_AddEvent( self, irandom(EV_DEATH1, EV_DEATH3), killer );
			//if we died from falling, add a nice "splat' sound lol
			if ( meansOfDeath == MOD_FALLING ) {
				G_AddEvent( self, EV_SPLAT, killer );
			}
		}

		//RPG-X : Model system - Death animations now based on vector hit
		if (meansOfDeath == MOD_FALLING ) {
			anim = BOTH_FALLDEATH1LAND;
		}
		else if ( self->waterlevel == 3 ) {
			anim = BOTH_FLOAT2;
		}
		else {
			if (meansOfDeath == MOD_PHASER || meansOfDeath == MOD_PHASER_ALT ) {
				if (self->client->lasthurt_location & LOCATION_FRONT ) {
					anim = BOTH_DEATHBACKWARD1;
				}
				else if ( self->client->lasthurt_location & LOCATION_BACK ) {
					anim = BOTH_DEATHFORWARD2;
				}
				else if ( self->client->lasthurt_location & LOCATION_LEFT ) {
					anim = BOTH_DEATH2;
				}
				else if ( self->client->lasthurt_location & LOCATION_RIGHT ) {
					anim = BOTH_DEATH2;
				}
				else {
					anim = BOTH_DEATH1;
				}
			}
			else {
				if (self->client->lasthurt_location & LOCATION_FRONT ) {
					anim = BOTH_DEATHBACKWARD2;
				}
				else if ( self->client->lasthurt_location & LOCATION_BACK ) {
					anim = BOTH_DEATHFORWARD1;
				}
				else if ( self->client->lasthurt_location & LOCATION_LEFT ) {
					anim = BOTH_DEATHFORWARD2;
				}
				else if ( self->client->lasthurt_location & LOCATION_RIGHT ) {
					anim = BOTH_DEATHFORWARD2;
				}
				else {
					anim = BOTH_DEATH1;
				}
			}
		}

		//TiM
		ps->stats[LEGSANIM] = 
			( ( ps->stats[LEGSANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;
		ps->stats[TORSOANIM] = 
			( ( ps->stats[TORSOANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;

		trap_LinkEntity (self);

		BG_PlayerStateToEntityState( &self->client->ps, &self->s, qtrue );	

		ClientUserinfoChanged( self->s.clientNum );

		ClientEndFrame( self );

		G_StoreClientInitialStatus( self );
		//---------------------
		//RPG-X: RedTechie - If it dose equal 0 use regular die
		//---------------------
	}else{
		gentity_t	*ent;
		int			anim;
		int			killer;
		int			i;
		char		*killerName, *obit;
		gentity_t	*detpack = NULL;
		char		*classname = NULL;
		int			BottomlessPitDeath;
		static		int deathNum;
		playerState_t *ps = &self->client->ps;

		if ( ps->pm_type == PM_DEAD ) {
			return;
		}

		if ( level.intermissiontime ) {
			return;
		}

		//RPG-X: RedTechie - Trying to make sure player dies when there health is 1 without medics revive turned on
		//RPG-X | Phenix | 05/04/2005 - Read learn that "=" sets where "==" is an if statement!!!
		if (self->health == 1) {
			self->health = 0;
		}

		ps->pm_type = PM_DEAD;
		//need to copy health here because pm_type was getting reset to PM_NORMAL if ClientThink_real was called before the STAT_HEALTH was updated
		ps->stats[STAT_HEALTH] = self->health;

		// check if we are in a NODROP Zone and died from a TRIGGER HURT
		//  if so, we assume that this resulted from a fall to a "bottomless pit" and 
		//  treat it differently...  
		//
		//  Any problems with other places in the code?
		//
		BottomlessPitDeath = 0;	// initialize

		contents = trap_PointContents( self->r.currentOrigin, -1 );
		if ( ( contents & CONTENTS_NODROP ) && (meansOfDeath ==	MOD_TRIGGER_HURT) )
		{
			BottomlessPitDeath = 1;
		}

		// similarly, if El Corpso here has already dropped a detpack, blow it up
		classname = BG_FindClassnameForHoldable(HI_DETPACK);
		if (classname)
		{
			while ((detpack = G_Find (detpack, FOFS(classname), classname)) != NULL)
			{
				if (detpack->parent == self)
				{
					detpack->think = DetonateDetpack;		// Detonate next think.
					detpack->nextthink = level.time;
				}
			}
		}

		if ( attacker ) {
			killer = attacker->s.number;
			if ( attacker->client ) {
				killerName = attacker->client->pers.netname;
			} else {
				killerName = "<non-client>";
			}
		} else {
			killer = ENTITYNUM_WORLD;
			killerName = "<world>";
		}

		if ( killer < 0 || killer >= MAX_CLIENTS ) {
			killer = ENTITYNUM_WORLD;
			killerName = "<world>";
		}

		if ( meansOfDeath < 0 || meansOfDeath >= sizeof( modNames ) / sizeof( modNames[0] ) ) {
			obit = "<bad obituary>";
		} else {
			obit = modNames[ meansOfDeath ];
		}

		G_LogPrintf("Kill: %i %i %i: %s killed %s by %s\n", 
			killer, self->s.number, meansOfDeath, killerName, 
			self->client->pers.netname, obit );

		G_LogWeaponKill(killer, meansOfDeath);
		G_LogWeaponDeath(self->s.number, self->s.weapon);
		if (attacker && attacker->client && attacker->inuse)
		{
			G_LogWeaponFrag(killer, self->s.number);
		}

		if ( meansOfDeath != MOD_RESPAWN )
		{
			// broadcast the death event to everyone
			ent = G_TempEntity( self->r.currentOrigin, EV_OBITUARY );
			ent->s.eventParm = meansOfDeath;
			ent->s.otherEntityNum = self->s.number;
			ent->s.otherEntityNum2 = killer;
			ent->r.svFlags = SVF_BROADCAST;	// send to everyone
		}

		self->enemy = attacker;

		ps->persistant[PERS_KILLED]++;
		if (self == attacker)
		{
			self->client->pers.teamState.suicides++;
		}

		//RPG-X: Redtechie - No awards or score calculations
		////////////////////////////////////////////////////////////////////////
		if (attacker && attacker->client) 
		{
			if ( attacker == self ) 
			{
				if ( meansOfDeath != MOD_RESPAWN )
				{//just changing class
					AddScore( attacker, -1 );
				}
			} 
			else 
			{
				attacker->client->pers.teamState.frags++;
				AddScore( attacker, 1 );

				// Check to see if the player is on a streak.
				attacker->client->streakCount++;

				attacker->client->lastKillTime = level.time;
			}
		} 
		else 
		{
			if ( meansOfDeath != MOD_RESPAWN )
			{//not just changing class
				AddScore( self, -1 );
			}
		}
		////////////////////////////////////////////////////////////////////////

		//RPG-X: Redtechie - agian no need
		// Add team bonuses
		//Team_FragBonuses(self, inflictor, attacker);

		// if client is in a nodrop area, don't drop anything (but return CTF flags!)
		if ( !( contents & CONTENTS_NODROP ) /*&& self->client->sess.sessionClass != PC_ACTIONHERO*/ && meansOfDeath != MOD_SUICIDE && meansOfDeath != MOD_RESPAWN ) 
		{//action hero doesn't drop stuff
			//don't drop stuff in specialty mode
			if ( meansOfDeath != MOD_RESPAWN ) {
				TossClientItems( self, qfalse );
			}
		}

		Cmd_Score_f( self );		// show scores
		// send updated scores to any clients that are following this one,
		// or they would get stale scoreboards
		for ( i = 0 ; i < level.maxclients ; i++ ) {
			gclient_t	*client;

			client = &level.clients[i];
			if ( client->pers.connected != CON_CONNECTED ) {
				continue;
			}
			if ( client->sess.sessionTeam != TEAM_SPECTATOR ) {
				continue;
			}
			if ( client->sess.spectatorClient == self->s.number ) {
				Cmd_Score_f( g_entities + i );
			}
		}

		self->takedamage = qtrue;	// can still be gibbed

		self->s.weapon = WP_0;
		self->s.powerups = 0;
		self->r.contents = CONTENTS_CORPSE;

		self->s.loopSound = 0;

		self->r.maxs[2] = -8;

		// don't allow respawn until the death anim is done
		// g_forcerespawn may force spawning at some later time
		self->client->respawnTime = level.time + 1700;

		// We always want to see the body for special animations, so make sure not to gib right away:
		if (meansOfDeath==MOD_CRIFLE_ALT || 
			meansOfDeath==MOD_DETPACK || 
			meansOfDeath==MOD_QUANTUM_ALT || 
			meansOfDeath==MOD_DREADNOUGHT_ALT ||
			meansOfDeath==MOD_PHASER_ALT)//RPG-X: RedTechie - Added phaser alt disnt
		{	self->health=0;}

		//RPG-X : Model system - Death animations now based on vector hit
		if (meansOfDeath == MOD_FALLING ) {
			anim = BOTH_FALLDEATH1LAND;
		}
		else if ( self->waterlevel == 3 ) {
			anim = BOTH_FLOAT2;
		}
		else {
			if (meansOfDeath == MOD_PHASER || meansOfDeath == MOD_PHASER_ALT ) {
				if (self->client->lasthurt_location & LOCATION_FRONT ) {
					anim = BOTH_DEATHBACKWARD1;
				}
				else if ( self->client->lasthurt_location & LOCATION_BACK ) {
					anim = BOTH_DEATHFORWARD2;
				}
				else if ( self->client->lasthurt_location & LOCATION_LEFT ) {
					anim = BOTH_DEATH2;
				}
				else if ( self->client->lasthurt_location & LOCATION_RIGHT ) {
					anim = BOTH_DEATH2;
				}
				else {
					anim = BOTH_DEATH1;
				}
			}
			else {
				if (self->client->lasthurt_location & LOCATION_FRONT ) {
					anim = BOTH_DEATHBACKWARD2;
				}
				else if ( self->client->lasthurt_location & LOCATION_BACK ) {
					anim = BOTH_DEATHFORWARD1;
				}
				else if ( self->client->lasthurt_location & LOCATION_LEFT ) {
					anim = BOTH_DEATHFORWARD2;
				}
				else if ( self->client->lasthurt_location & LOCATION_RIGHT ) {
					anim = BOTH_DEATHFORWARD2;
				}
				else {
					anim = BOTH_DEATH1;
				}
			}
		}

		ps->stats[LEGSANIM] = 
			( ( ps->stats[LEGSANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;
		ps->stats[TORSOANIM] = 
			( ( ps->stats[TORSOANIM] & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;

		if ( (BottomlessPitDeath==1) && (killer == ENTITYNUM_WORLD))
		{
			//G_AddEvent( self, EV_FALL_FAR, killer ); ?? Need to play falling SF now, or 
			// use designer trigger??
			//FIXME: need *some* kind of death anim!
		}
		else
		{
			// normal death

			switch(meansOfDeath)
			{
			case MOD_PHASER_ALT: //RPG-X: RedTechie - Added better effect for alt phaser
				if(rpg_phaserdisintegrates.integer == 1){//RPG-X: RedTechie - Check to see if we want this
					G_AddEvent( self, EV_DISINTEGRATION, killer );
					ps->powerups[PW_DISINTEGRATE] = level.time + 100000;
					VectorClear( ps->velocity );
					self->takedamage = qfalse;
					self->r.contents = 0;
				}
				break;
			case MOD_CRIFLE_ALT:
				break;
			case MOD_QUANTUM_ALT:
				G_AddEvent( self, EV_DISINTEGRATION2, killer );
				ps->powerups[PW_EXPLODE] = level.time + 100000;
				VectorClear( ps->velocity );
				self->takedamage = qfalse;
				self->r.contents = 0;
				break;
			case MOD_SCAVENGER_ALT:
			case MOD_SCAVENGER_ALT_SPLASH:
			case MOD_GRENADE:
			case MOD_GRENADE_ALT:
			case MOD_GRENADE_SPLASH:
			case MOD_GRENADE_ALT_SPLASH:
			case MOD_QUANTUM:
			case MOD_QUANTUM_SPLASH:
			case MOD_QUANTUM_ALT_SPLASH:
			case MOD_DETPACK:
				G_AddEvent( self, EV_EXPLODESHELL, killer );
				ps->powerups[PW_EXPLODE] = level.time + 100000;
				VectorClear( ps->velocity );
				self->takedamage = qfalse;
				self->r.contents = 0;
				break;
			case MOD_DREADNOUGHT:
			case MOD_DREADNOUGHT_ALT:
				G_AddEvent( self, EV_ARCWELD_DISINT, killer);
				ps->powerups[PW_ARCWELD_DISINT] = level.time + 100000;
				VectorClear( ps->velocity );
				self->takedamage = qfalse;
				self->r.contents = 0;
				break;
			case MOD_FALLING:
				G_AddEvent( self, EV_SPLAT, killer );
				break;
			default:
				G_AddEvent( self, irandom(EV_DEATH1, EV_DEATH3), killer );
				break;
			}

			// the body can still be gibbed
			self->die = body_die;

		}
		// globally cycle through the different death animations
		deathNum = ( deathNum + 1 ) % 3;

		trap_LinkEntity (self);
	}//RPG-X: RedTechie - End of my if statment for medics revive check
}//RPG-X: RedTechie - End of void

#define	BORG_ADAPT_NUM_HITS 10
static qboolean G_CheckBorgAdaptation( gentity_t *targ, int mod )
{
	int	weapon = 0;

	if ( !targ->client )
	{
		return qfalse;
	}

	switch( mod )
	{
	//other kinds of damage
	case MOD_UNKNOWN:
	case MOD_WATER:
	case MOD_SLIME:
	case MOD_LAVA:
	case MOD_CRUSH:
	case MOD_TELEFRAG:
	case MOD_FALLING:
	case MOD_SUICIDE:
	case MOD_RESPAWN:
	case MOD_TARGET_LASER:
	case MOD_TRIGGER_HURT:
	case MOD_DETPACK:
	case MOD_MAX:
	case MOD_KNOCKOUT:
	case MOD_EXPLOSION:
		return qfalse;
		break;
	// Trek weapons
	case MOD_PHASER:
	case MOD_PHASER_ALT:
		weapon = WP_5;
		break;
	case MOD_CRIFLE:
	case MOD_CRIFLE_SPLASH:
	case MOD_CRIFLE_ALT:
	case MOD_CRIFLE_ALT_SPLASH:
		weapon = WP_6;
		break;
	case MOD_SCAVENGER:
	case MOD_SCAVENGER_ALT:
	case MOD_SCAVENGER_ALT_SPLASH:
	case MOD_SEEKER:
		weapon = WP_4;
		break;
	case MOD_STASIS:
	case MOD_STASIS_ALT:
		weapon = WP_10;
		break;
	case MOD_GRENADE:
	case MOD_GRENADE_ALT:
	case MOD_GRENADE_SPLASH:
	case MOD_GRENADE_ALT_SPLASH:
		weapon = WP_8;
		break;
	case MOD_TETRION:
	case MOD_TETRION_ALT:
		weapon = WP_7;
		break;
	case MOD_DREADNOUGHT:
	case MOD_DREADNOUGHT_ALT:
		weapon = WP_13;
		break;
	case MOD_QUANTUM:
	case MOD_QUANTUM_SPLASH:
	case MOD_QUANTUM_ALT:
	case MOD_QUANTUM_ALT_SPLASH:
		weapon = WP_9;
		break;
	case MOD_IMOD:
	case MOD_IMOD_ALT:
		weapon = WP_3;
		break;
	case MOD_ASSIMILATE:
	case MOD_BORG:
	case MOD_BORG_ALT:
		return qtrue;
		break;
	}

	level.borgAdaptHits[weapon]++;
	switch(weapon) {
	case WP_5:
		if(level.borgAdaptHits[WP_5] > rpg_adaptPhaserHits.integer)
			return qtrue;
		break;
	case WP_6:
		if(level.borgAdaptHits[WP_6] > rpg_adaptCrifleHits.integer)
			return qtrue;
		break;
	case WP_10:
		if(level.borgAdaptHits[WP_10] > rpg_adaptDisruptorHits.integer)
			return qtrue;
		break;
	case WP_8:
		if(level.borgAdaptHits[WP_8] > rpg_adaptGrenadeLauncherHits.integer)
			return qtrue;
		break;
	case WP_7:
		if(level.borgAdaptHits[WP_7] > rpg_adaptTR116Hits.integer)
			return qtrue;
		break;
	case WP_9:
		if(level.borgAdaptHits[WP_9] > rpg_adaptPhotonHits.integer)
			return qtrue;
		break;
	default:
		return qfalse;
	}
	return qfalse;
}

/* 
============
G_LocationDamage
============
*/
static int G_LocationDamage(vec3_t point, gentity_t* targ, gentity_t* attacker, int take) {
	vec3_t bulletPath;
	vec3_t bulletAngle;

	int clientHeight;
	int clientFeetZ;
	int clientRotation;
	int bulletHeight;
	int bulletRotation;	// Degrees rotation around client.
	// used to check Back of head vs. Face
	int impactRotation;


	// First things first.  If we're not damaging them, why are we here? 
	if (!take) 
		return 0;

	// Point[2] is the REAL world Z. We want Z relative to the clients feet

	// Where the feet are at [real Z]
	clientFeetZ  = targ->r.currentOrigin[2] + targ->r.mins[2];	
	// How tall the client is [Relative Z]
	clientHeight = targ->r.maxs[2] - targ->r.mins[2];
	// Where the bullet struck [Relative Z]
	bulletHeight = point[2] - clientFeetZ;

	// Get a vector aiming from the client to the bullet hit 
	VectorSubtract(targ->r.currentOrigin, point, bulletPath); 
	// Convert it into PITCH, ROLL, YAW
	vectoangles(bulletPath, bulletAngle);

	clientRotation = targ->client->ps.viewangles[YAW];
	bulletRotation = bulletAngle[YAW];

	impactRotation = abs(clientRotation-bulletRotation);

	impactRotation += 45; // just to make it easier to work with
	impactRotation = impactRotation % 360; // Keep it in the 0-359 range

	if (impactRotation < 90) {
		targ->client->lasthurt_location = LOCATION_BACK;
	}
	else if (impactRotation < 180) {
		targ->client->lasthurt_location = LOCATION_RIGHT;
	}
	else if (impactRotation < 270) {
		targ->client->lasthurt_location = LOCATION_FRONT;
	}
	else if (impactRotation < 360) {
		targ->client->lasthurt_location = LOCATION_LEFT;
	}
	else {
		targ->client->lasthurt_location = LOCATION_NONE;
	}

	// The upper body never changes height, just distance from the feet
	if (bulletHeight > clientHeight - 2) {
		targ->client->lasthurt_location |= LOCATION_HEAD;
	}
	else if (bulletHeight > clientHeight - 8) {
		targ->client->lasthurt_location |= LOCATION_FACE;
	}
	else if (bulletHeight > clientHeight - 10) {
		targ->client->lasthurt_location |= LOCATION_SHOULDER;
	}
	else if (bulletHeight > clientHeight - 16) {
		targ->client->lasthurt_location |= LOCATION_CHEST;
	}
	else if (bulletHeight > clientHeight - 26) {
		targ->client->lasthurt_location |= LOCATION_STOMACH;
	}
	else if (bulletHeight > clientHeight - 29) {
		targ->client->lasthurt_location |= LOCATION_GROIN;
	}
	else if (bulletHeight < 4) {
		targ->client->lasthurt_location |= LOCATION_FOOT;
	}
	else {
		// The leg is the only thing that changes size when you duck,
		// so we check for every other parts RELATIVE location, and
		// whats left over must be the leg. 
		targ->client->lasthurt_location |= LOCATION_LEG;
	}

	// Check the location ignoring the rotation info
	switch ( targ->client->lasthurt_location & 
		~(LOCATION_BACK | LOCATION_LEFT | LOCATION_RIGHT | LOCATION_FRONT) )
	{
	case LOCATION_HEAD:
		take *= 1.8;
		break;
	case LOCATION_FACE:
		if (targ->client->lasthurt_location & LOCATION_FRONT)
			take *= 5.0; // Faceshots REALLY suck
		else
			take *= 1.8;
		break;
	case LOCATION_SHOULDER:
		if (targ->client->lasthurt_location & (LOCATION_FRONT | LOCATION_BACK))
			take *= 1.4; // Throat or nape of neck
		else
			take *= 1.1; // Shoulders
		break;
	case LOCATION_CHEST:
		if (targ->client->lasthurt_location & (LOCATION_FRONT | LOCATION_BACK))
			take *= 1.3; // Belly or back
		else
			take *= 0.8; // Arms
		break;
	case LOCATION_STOMACH:
		take *= 1.2;
		break;
	case LOCATION_GROIN:
		if (targ->client->lasthurt_location & LOCATION_FRONT)
			take *= 1.3; // Groin shot
		break;
	case LOCATION_LEG:
		take *= 0.7;
		break;
	case LOCATION_FOOT:
		take *= 0.5;
		break;

	}

	return take;
}

/*
============
T_Damage

targ		entity that is being damaged
inflictor	entity that is causing the damage
attacker	entity that caused the inflictor to damage targ
example: targ=monster, inflictor=rocket, attacker=player

dir			direction of the attack for knockback
point		point at which the damage is being inflicted, used for headshots
damage		amount of damage being inflicted
knockback	force to be applied against targ as a result of the damage

inflictor, attacker, dir, and point can be NULL for environmental effects

dflags		these flags are used to control how T_Damage works
DAMAGE_RADIUS			damage was indirect (from a nearby explosion)
DAMAGE_NO_ARMOR			armor does not protect from this damage
DAMAGE_NO_KNOCKBACK		do not affect velocity, just view angles
DAMAGE_NO_PROTECTION	kills godmode, armor, everything
============
*/

void G_Damage( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker,
			  vec3_t dir, vec3_t point, int damage, int dflags, int mod ) {
	gclient_t	*client;
	int			take=0;
	int			knockback;
	qboolean	bFriend = qfalse;

	if(!targ) return;

#ifdef G_LUA
	if(targ->luaHurt && !targ->client)
	{
		LuaHook_G_EntityHurt(targ->luaHurt, targ->s.number, inflictor->s.number, attacker->s.number);
	}
#endif

	if (!targ->takedamage) {
		return;
	}

	// the intermission has allready been qualified for, so don't
	// allow any extra scoring
	if ( level.intermissionQueued ) {
		return;
	}

	if ( !inflictor ) {
		inflictor = &g_entities[ENTITYNUM_WORLD];
	}
	if ( !attacker ) {
		attacker = &g_entities[ENTITYNUM_WORLD];
	}

	// shootable doors / buttons don't actually have any health
	if ( (targ->s.eType == ET_MOVER && Q_stricmp("func_breakable", targ->classname) != 0 && Q_stricmp("misc_model_breakable", targ->classname) != 0) ||
		(targ->s.eType == ET_MOVER_STR && Q_stricmp("func_breakable", targ->classname) != 0 && Q_stricmp("misc_model_breakable", targ->classname) != 0)) //RPG-X | GSIO01 | 13/05/2009 
	{
		if ( !Q_stricmp( targ->classname, "func_forcefield" ) )
		{
			if ( targ->pain )
			{
				targ->pain( targ, inflictor, take );
			}
		}
		else if ( targ->use && (targ->moverState == MOVER_POS1 || targ->moverState == ROTATOR_POS1) && Q_stricmp(targ->classname, "func_door") && Q_stricmp(targ->classname, "func_door_rotating") ) 
		{
			targ->use( targ, inflictor, attacker );
		}
		return;
	}

	//RPG-X | GSIO01 | 08/05/2009: as we put borg adaption back in we need this again
	if ( rpg_borgAdapt.integer > -1 && G_CheckBorgAdaptation( targ, mod ) && IsBorg(targ) )
	{
		//flag targ for adaptation effect
		targ->client->ps.powerups[PW_BORG_ADAPT] = level.time + 250;
		if(rpg_adaptUseSound.integer == 1)
			G_AddEvent(targ, EV_ADAPT_SOUND, 0);
		return;
	}

	// multiply damage times dmgmult
	damage *= g_dmgmult.value;

	// reduce damage by the attacker's handicap value
	// unless they are rocket jumping
	if ( attacker->client && attacker != targ ) {
		damage = damage * attacker->client->ps.stats[STAT_MAX_HEALTH] / 100;
	}

	client = targ->client;

	if ( client ) 
	{
		if ( client->noclip ) {
			return;
		}
	}

	if ( !dir ) {
		dflags |= DAMAGE_NO_KNOCKBACK;
	} else {
		VectorNormalize(dir);
	}

	knockback = damage;
	if ( knockback > 200 ) {
		knockback = 200;
	}
	if ( targ->flags & FL_NO_KNOCKBACK ) {
		knockback = 0;
	}
	if ( dflags & DAMAGE_NO_KNOCKBACK ) {
		knockback = 0;
	}

	knockback = floor( knockback*g_dmgmult.value ) ;

	// figure momentum add, even if the damage won't be taken
	if ( knockback && targ->client ) 
	{
		//if it's non-radius damage knockback from a teammate, don't do it if the damage won't be taken
		if ( (dflags&DAMAGE_ALL_TEAMS) || (dflags&DAMAGE_RADIUS) || g_friendlyFire.integer || !attacker->client ) 
		{
			vec3_t	kvel;
			float	mass;

			mass = 200;

			if (targ->client->ps.powerups[PW_FLIGHT])
			{
				mass *= 0.375;
			}

			if(dir) {
				VectorScale (dir, g_knockback.value * (float)knockback / mass, kvel);
				VectorAdd (targ->client->ps.velocity, kvel, targ->client->ps.velocity);
			}

			// set the timer so that the other client can't cancel
			// out the movement immediately
			if ( !targ->client->ps.pm_time ) {
				int		t;

				t = knockback * 2;
				if ( t < 50 ) {
					t = 50;
				}
				if ( t > 200 ) {
					t = 200;
				}
				targ->client->ps.pm_time = t;
				targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
			}
		}
	}

	// if TF_NO_FRIENDLY_FIRE is set, don't do damage to the target
	// if the attacker was on the same team
	// check for completely getting out of the damage
	if ( !(dflags & DAMAGE_NO_PROTECTION) ) {
		if ( !(dflags&DAMAGE_ALL_TEAMS) && mod != MOD_TELEFRAG && mod != MOD_DETPACK && targ != attacker ) 
		{
			if ( attacker->client && targ->client )
			{//this only matters between clients
				if ( !g_friendlyFire.integer ) 
				{//friendly fire is not allowed
					return;
				}
			}
			else
			{//team damage between non-clients is never legal
				return;
			}
		}

		// check for godmode
		if ( targ->flags & FL_GODMODE ) {
			return;
		}
	}

	// always give half damage if hurting self
	// calculated after knockback, so rocket jumping works
	if ( rpg_selfdamage.integer != 0 )
	{
		if ( targ == attacker)
		{
			damage *= 0.5;
		}
		if ( damage < 1 )
		{
			damage = 1;
		}
	}
	else
	{
		if ( targ == attacker)
		{
			damage *= 0.0;
		}
		if ( damage < 1 )
		{
			damage = 0;
		}
	}

	take = damage;

	// save some from armor
	//RPG-X: - RedTechie No armor in RPG
	//asave = CheckArmor (targ, take, dflags);
	//take -= asave;

	if ( g_debugDamage.integer ) {
		G_Printf( "%i: client:%i health:%i damage:%i armor:<n/a>\n", level.time, targ->s.number,
			targ->health, take );
	}

	// add to the damage inflicted on a player this frame
	// the total will be turned into screen blends and view angle kicks
	// at the end of the frame
	if ( client ) {
		if ( attacker ) {
			client->ps.persistant[PERS_ATTACKER] = attacker->s.number;
		} else {
			client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;
		}
		//RPG-X: - RedTechie no armor in RPG
		client->damage_blood += take;
		client->damage_knockback += knockback;
		if ( dir ) {
			VectorCopy ( dir, client->damage_from );
			client->damage_fromWorld = qfalse;
		} else {
			VectorCopy ( targ->r.currentOrigin, client->damage_from );
			client->damage_fromWorld = qtrue;
		}
	}

	if (targ->client) {
		// set the last client who damaged the target
		targ->client->lasthurt_client = attacker->s.number;
		targ->client->lasthurt_mod = mod;

		// Modify the damage for location damage
		if (point && targ && targ->health > 1 && attacker && take)
			take = G_LocationDamage(point, targ, attacker, take);
		else
			targ->client->lasthurt_location = LOCATION_NONE;
	}

	// do the damage
	if (take > 0 )
	{
		// add to the attacker's hit counter
		if ( (MOD_TELEFRAG != mod) && attacker->client && targ != attacker && targ->health > 0 )
		{//don't telefrag since damage would wrap when sent as a short and the client would think it's a team dmg.
			if (bFriend)
			{
				attacker->client->ps.persistant[PERS_HITS] -= damage;
			}
			else if (targ->classname && strcmp(targ->classname, "holdable_shield") // no stupid hit noise when players shoot a shield -- dpk
				&& strcmp(targ->classname, "holdable_detpack")) // or the detpack either
			{
				attacker->client->ps.persistant[PERS_HITS] += damage;
			}
		}

		targ->health = targ->health - take;

		//RPG-X: RedTechie - If medicrevive is on then health only goes down to 1 so we can simulate fake death
		if(rpg_medicsrevive.integer == 1 && Q_stricmp("func_breakable", targ->classname) && Q_stricmp("misc_model_breakable", targ->classname ) ){
			if(targ->health <= 0){
				targ->health = 1;
			}
		}else {
			if(rpg_medicsrevive.integer != 1) {
				if (targ->health == 1) { //RPG-X: RedTechie: Ok regular die now kills the player at 1 health not 0
					targ->health = 0;
				}
			}
		}

		if ( targ->client )
		{
			targ->client->ps.stats[STAT_HEALTH] = targ->health;
		}

		//RPG-X: RedTechie - Custum medicrevive code
		if(rpg_medicsrevive.integer == 1 && targ->s.eType == ET_PLAYER ){
			if(targ->health == 1 ){ //TiM : Added Client to try and fix this stupid crashy bug
				client->ps.stats[STAT_WEAPONS] = ( 1 << WP_0 ); //?!!!!!
				client->ps.stats[STAT_HOLDABLE_ITEM] = HI_NONE;
				targ->health = 1;
				player_die( targ, inflictor, attacker, take, mod );
			}
		}else{
			if ( targ->health <= 0 ) {
				if ( client )
					targ->flags |= FL_NO_KNOCKBACK;

				if (targ->health < -999)
					targ->health = -999;

#ifdef G_LUA
				if(targ->luaDie && !targ->client)
				{
					LuaHook_G_EntityDie(targ->luaDie, targ->s.number, inflictor->s.number, attacker->s.number, take, mod);
				}
#endif

				targ->enemy = attacker;
				targ->die (targ, inflictor, attacker, take, mod);
				return;
			} 

			if ( targ->pain ) {
				targ->pain (targ, attacker, take);
			}
		}
		G_LogWeaponDamage(attacker->s.number, mod, take);
	}

}


/*
============
CanDamage

Returns qtrue if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/
qboolean CanDamage (gentity_t *targ, vec3_t origin) {
	vec3_t	dest;
	trace_t	tr;
	vec3_t	midpoint;

	// use the midpoint of the bounds instead of the origin, because
	// bmodels may have their origin is 0,0,0
	VectorAdd (targ->r.absmin, targ->r.absmax, midpoint);
	VectorScale (midpoint, 0.5, midpoint);

	VectorCopy (midpoint, dest);
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0)
		return qtrue;

	// this should probably check in the plane of projection, 
	// rather than in world coordinate, and also include Z
	VectorCopy (midpoint, dest);
	dest[0] += 15.0;
	dest[1] += 15.0;
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0)
		return qtrue;

	VectorCopy (midpoint, dest);
	dest[0] += 15.0;
	dest[1] -= 15.0;
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0)
		return qtrue;

	VectorCopy (midpoint, dest);
	dest[0] -= 15.0;
	dest[1] += 15.0;
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0)
		return qtrue;

	VectorCopy (midpoint, dest);
	dest[0] -= 15.0;
	dest[1] -= 15.0;
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0)
		return qtrue;


	return qfalse;
}


/*
============
G_RadiusDamage
============
*/
extern void tripwireThink ( gentity_t *ent );
qboolean G_RadiusDamage ( vec3_t origin, gentity_t *attacker, float damage, float radius,
						 gentity_t *ignore, int dflags, int mod) {
	float		points, dist;
	gentity_t	*ent;
	int			entityList[MAX_GENTITIES];
	int			numListedEntities;
	vec3_t		mins, maxs;
	vec3_t		v;
	vec3_t		dir;
	int			i, e;
	qboolean	hitClient = qfalse;

	if ( radius < 1 ) {
		radius = 1;
	}

	for ( i = 0 ; i < 3 ; i++ ) {
		mins[i] = origin[i] - radius;
		maxs[i] = origin[i] + radius;
	}

	numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

	for ( e = 0 ; e < numListedEntities ; e++ ) {
		ent = &g_entities[entityList[ e ]];

		if (ent == ignore)
			continue;
		if (!ent->takedamage)
			continue;
		if ( ignore != NULL && ignore->parent != NULL && ent->parent == ignore->parent )
		{
			if ( ignore->think == tripwireThink && ent->think == tripwireThink )
			{//your own tripwires do not fire off other tripwires of yours.
				continue;
			}
		}

		// find the distance from the edge of the bounding box
		for ( i = 0 ; i < 3 ; i++ ) {
			if ( origin[i] < ent->r.absmin[i] ) {
				v[i] = ent->r.absmin[i] - origin[i];
			} else if ( origin[i] > ent->r.absmax[i] ) {
				v[i] = origin[i] - ent->r.absmax[i];
			} else {
				v[i] = 0;
			}
		}

		dist = VectorLength( v );
		if ( dist >= radius ) {
			continue;
		}

		points = damage * ( 1.0 - dist / radius );

		if( !CanDamage (ent, origin) ) {
			//no LOS to ent
			if ( !(dflags & DAMAGE_HALF_NOTLOS) ) {
				//not allowed to do damage without LOS
				continue;
			} else {
				//do 1/2 damage if no LOS but within rad
				points *= 0.5;
			}
		}

		if( LogAccuracyHit( ent, attacker ) ) {
			hitClient = qtrue;
		}
		VectorSubtract (ent->r.currentOrigin, origin, dir);
		// push the center of mass higher than the origin so players
		// get knocked into the air more
		dir[2] += 24;
		G_Damage (ent, NULL, attacker, dir, origin, (int)points, dflags|DAMAGE_RADIUS, mod);
	}

	return hitClient;
}

/*
============
IsBorg
RPG-X | GSIO01 | 08/05/2009 
============
*/
/**
*	Checks if a player is in a Class that is borg.
*	\param ent the player
*
*	\author Ubergames - GSIO01
*	\date 08/05/2009
*/
qboolean IsBorg(gentity_t *ent) {
	if(!ent)
		return qfalse;
	if(!ent->client)
		return qfalse;
	if(g_classData[ent->client->sess.sessionClass].isBorg)
		return qtrue;
	else
		return qfalse;
}

extern void InitBBrush(gentity_t *ent);
extern void SP_misc_model_breakable(gentity_t* self);
/*
============
G_Repair
RPG-X | GSIO01 | 09/05/2009 
============
*/
void G_Repair(gentity_t *ent, gentity_t *tr_ent, float rate) {
	float		distance;
	vec3_t		help, forward;
	int			i;
	float		max = 0;

	// if count isn't 0 the breakable is not damaged and if target is no breakable it does not make sense to go on
	if(tr_ent->count != 0 || strstr(tr_ent->classname, "breakable") == NULL) {
		return; 
	}

	if(!(tr_ent->spawnflags & 256)) { // no REPAIRABLE flag set
		return;
	}

	// check if player is near the breakable
	if(tr_ent->spawnflags & 512) {
		VectorSubtract(tr_ent->s.angles2, ent->r.currentOrigin, help);
		max = tr_ent->n00bCount;
	} else {
		VectorSubtract(tr_ent->s.origin, ent->r.currentOrigin, help);
		for(i = 0; i < 3; i++) {
			if(tr_ent->r.maxs[i] > max) {
				max = tr_ent->r.maxs[i];
			}
		}
	}
	distance = VectorLength(help);

	//G_Printf("goodDst=%f, curDst=%f\n", 80 + max, distance);
	if(distance > 80 + max) {
		return;
	}

	// check if the player is facing it
	AngleVectors(ent->client->ps.viewangles, forward, NULL, NULL);
	if(DotProduct(help, forward) < 0.4) {
		return;
	}

	// check wheter the breakable still needs to be repaired
	if(tr_ent->health < tr_ent->damage) {
		// still not repaired of let's go on
		tr_ent->health += rate;
	} else {
		if(!strcmp(tr_ent->classname, "func_breakable")) {
			// else restore it
			tr_ent->s.solid = CONTENTS_BODY;
			trap_SetBrushModel(tr_ent, tr_ent->model);
			tr_ent->r.svFlags &= ~SVF_NOCLIENT;
			tr_ent->s.eFlags &= ~EF_NODRAW;
			InitBBrush(tr_ent);
			tr_ent->health = tr_ent->damage;

			if(tr_ent->health) {
				tr_ent->takedamage = qtrue;
			}

			tr_ent->use = breakable_use;

			if(tr_ent->paintarget) {
				tr_ent->pain = breakable_pain;
			}

			tr_ent->clipmask = 0;
			tr_ent->count = 1;

			if(tr_ent->target) {
				G_UseTargets2(tr_ent, tr_ent, tr_ent->target);
			}
		} else if(!strcmp(tr_ent->classname, "misc_model_breakable")) {
			tr_ent->health = tr_ent->damage;
			SP_misc_model_breakable(tr_ent);
		}
	}
}
