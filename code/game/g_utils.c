/*
 * Copyright (C) 1999-2000 Id Software, Inc.
 *
 * g_utils.c -- misc utility functions for game module
 *
 */


#include "g_local.h"

/**
 * \brief Data structure for a singele shader remap.
 *
 * Data structure for a single shader remap
 *
 */
typedef struct {
	/** The original shader */
	char oldShader[MAX_QPATH];
	/** The replacement shader */
	char newShader[MAX_QPATH];
	/** time offset */
  	float timeOffset;
} shaderRemap_t;

/** The maximum count of shader remaps */
#define MAX_SHADER_REMAPS 128

/** Current count of remapped shaders  */
static int remapCount = 0;
/** List of shader remaps */
static shaderRemap_t remappedShaders[MAX_SHADER_REMAPS];

/**
 * \brief Add a new shader remap.
 *
 * Remaps oldShader with newShader.
 *
 * @param oldShader shader to be remapped
 * @param newShader replacement shader
 * @param timeOffset time offset
 */
void AddRemap(const char *oldShader, const char *newShader, float timeOffset) {
	int i;

	for (i = 0; i < remapCount; i++) {
		if (Q_stricmp(oldShader, remappedShaders[i].oldShader) == 0) {
			/* found it, just update this one */
			strcpy(remappedShaders[i].newShader,newShader);
			remappedShaders[i].timeOffset = timeOffset;
			return;
		}
	}
	if (remapCount < MAX_SHADER_REMAPS) {
		strcpy(remappedShaders[remapCount].newShader,newShader);
		strcpy(remappedShaders[remapCount].oldShader,oldShader);
		remappedShaders[remapCount].timeOffset = timeOffset;
		remapCount++;
	}
}

/**
 * \brief Builds the shader state config.
 *
 * Builds the shader state config.
 *
 * @return the shader state config
 */
const char *BuildShaderStateConfig(void) {
	static char	buff[MAX_STRING_CHARS*4];
	char out[(MAX_QPATH * 2) + 5];
	int i;
  
	memset(buff, 0, MAX_STRING_CHARS);
	for (i = 0; i < remapCount; i++) {
		Com_sprintf(out, (MAX_QPATH * 2) + 5, "%s=%s:%5.2f@", remappedShaders[i].oldShader, remappedShaders[i].newShader, remappedShaders[i].timeOffset);
		Q_strcat( buff, sizeof( buff ), out);
	}
	return buff;
}

/*
=========================================================================

model / sound configstring indexes

=========================================================================
*/

/*
 * \brief Finds the index a config string starts at.
 *
 * Finds the index a config string start at.
 *
 * @param name name of the config string
 * @param start look from here
 * @param max look until here
 * @param create create a new config string?
 * 
 * @return The config strings index
 */
int G_FindConfigstringIndex( char *name, int start, int max, qboolean create ) {
	int		i;
	char	s[MAX_STRING_CHARS];

	if ( !name || !name[0] ) {
		return 0;
	}

	for ( i=1 ; i<max ; i++ ) {
		trap_GetConfigstring( start + i, s, sizeof( s ) );
		if ( !s[0] ) {
			break;
		}
		if ( !strcmp( s, name ) ) {
			return i;
		}
	}

	if ( !create ) {
		return 0;
	}

	if ( i == max ) {
		/*G_Error( "G_FindConfigstringIndex: overflow" );*/
		G_Printf( S_COLOR_RED "G_FindConfigstringIndex: Full!! Could not add value: %s\n", name );
	}

	trap_SetConfigstring( start + i, name );

	return i;
}

/**
 * \brief Get the model index for a model.
 *
 * Get the model index for a model.
 *
 * @param name the model name
 *
 * @return the models index
 */
int G_ModelIndex( char *name ) {
	return G_FindConfigstringIndex (name, CS_MODELS, MAX_MODELS, qtrue);
}

/**
 * \brief Get the sound index for a sound.
 *
 * Get the sound index for a sound.
 *
 * @param name the sound name
 *
 * @return the sounds index
 */
int G_SoundIndex( char *name ) {
	return G_FindConfigstringIndex (name, CS_SOUNDS, MAX_SOUNDS, qtrue);
}

/**
 * \brief Get the tric string index.
 *
 * Get the tric string index.
 *
 * @param name the tric string name
 *
 * @return the tric strings index
 */
int G_TricStringIndex( char *name ) {
	return G_FindConfigstringIndex (name, CS_TRIC_STRINGS, MAX_TRIC_STRINGS, qtrue);
}

//=====================================================================


/**
 * \brief Issue a team command.
 *
 * Issue a team command.
 *
 * @param team the team
 * @param cmd the command
 */
void G_TeamCommand( team_t team, char *cmd ) {
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected == CON_CONNECTED ) {
			if ( level.clients[i].sess.sessionTeam == team ) {
				trap_SendServerCommand( i, va("%s", cmd ));
			}
		}
	}
}


/**
 *
 * \brief Finds an entity.
 *
 * Searches all active entities for the next one that holds
 * the matching string at fieldofs (use the FOFS() macro) in the structure.
 * Searches beginning at the entity after from, or the beginning if NULL
 * NULL will be returned if the end of the list is reached.
 *
 * @param from search from this entity on
 * @param fieldofs in which field to look
 * @param match string to match
 *
 * @return an matching entity or NULL
 */
gentity_t *G_Find (gentity_t *from, int fieldofs, const char *match)
{
	char	*s;

	if (!from)
		from = g_entities;
	else
		from++;

	for ( ; from < &g_entities[level.num_entities] ; from++)
	{
		if (!from->inuse)
			continue;
		s = *(char **) ((byte *)from + fieldofs);
		if (!s)
			continue;
		if (!Q_stricmp (s, match))
			return from;
	}

	return NULL;
}


/** Maximum number of possible choices for G_PickTarget. */
#define MAXCHOICES 	32

/**
 * \brief Pick a target.
 *
 * Selects a random entity from among the targets.
 *
 * @param targetname the targets targetname
 *
 * @return an entity or NULL
 */
gentity_t *G_PickTarget (char *targetname)
{
	gentity_t	*ent = NULL;
	int		num_choices = 0;
	gentity_t	*choice[MAXCHOICES];

	if (!targetname)
	{
		G_Printf("G_PickTarget called with NULL targetname\n");
		return NULL;
	}

	/* BOOKMARK */
	while(1)
	{
		ent = G_Find (ent, FOFS(targetname), targetname);
		if (!ent)
			break;
		choice[num_choices++] = ent;
		if (num_choices == MAXCHOICES)
			break;
	}

	/*================
	RPG-X Modification
	Phenix
	13/06/2004
	================*/
	if (!num_choices)
	{
		while(1)
		{
			ent = G_Find (ent, FOFS(swapname), targetname);
			if (!ent)
				break;
			choice[num_choices++] = ent;
			if (num_choices == MAXCHOICES)
				break;
		}
	}
	
	if (!num_choices)
	{
		while(1)
		{
			ent = G_Find (ent, FOFS(truename), targetname);
			if (!ent)
				break;
			choice[num_choices++] = ent;
			if (num_choices == MAXCHOICES)
				break;
		}
	}

	if (!num_choices)
	{
		while(1)
		{
			ent = G_Find (ent, FOFS(falsename), targetname);
			if (!ent)
				break;
			choice[num_choices++] = ent;
			if (num_choices == MAXCHOICES)
				break;
		}
	}

	/*================
	End Modification
	================*/

	if (!num_choices)
	{
		G_Printf("G_PickTarget: target %s not found\n", targetname);
		return NULL;
	}

	return choice[rand() % num_choices];
}

/**
 * \brief Use all targets of the given entity.
 *
 * Goes through all entities and calls ther use function if their
 * targetname, swapname, truename, falsename, bluename are matching
 * the target. activator should be set the the inflictor of this function
 * call.
 *
 * @param ent the entity
 * @param activator the activator
 * @param target target to match
 */
void G_UseTargets2( gentity_t *ent, gentity_t *activator, char *target ) {
	gentity_t		*t;
	
	if ( !ent ) {
		return;
	}

	if (ent->targetShaderName && ent->targetShaderNewName) {
		float f = level.time * 0.001;
		AddRemap(ent->targetShaderName, ent->targetShaderNewName, f);
		trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
	}

	if ( !target ) {
		return;
	}

	t = NULL;
	while ( (t = G_Find (t, FOFS(targetname), target)) != NULL ) {
		if ( t == ent ) {
			G_Printf ("WARNING: Entity %i used itself.\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
		} else {
			if ( t->use ) {
				t->use (t, ent, activator);
				#ifdef G_LUA
				if(t->luaUse)
				{
					if(activator)
					{
						LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, activator->s.number);
					}
					else
					{
						LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, ENTITYNUM_WORLD);
					}
				}
				#endif
			}
		}
		if ( !ent->inuse ) {
			G_Printf("Entity %i was removed while using targets\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
			return;
		}
	}

	/*================
	RPG-X Modification
	Phenix
	13/06/2004
	================*/
	t = NULL;
	while ( (t = G_Find (t, FOFS(swapname), target)) != NULL ) {
		if ( t == ent ) {
			G_Printf ("WARNING: Entity %i used itself.\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
		} else {
			if ( t->use ) {
				t->use (t, ent, activator);
				#ifdef G_LUA
				if(t->luaUse)
				{
					if(activator)
					{
						LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, activator->s.number);
					}
					else
					{
						LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, ENTITYNUM_WORLD);
					}
				}
				#endif
			}
		}
		if ( !ent->inuse ) {
			G_Printf("Entity %i was removed while using targets\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
			return;
		}
	}

	t = NULL;
	while ( (t = G_Find (t, FOFS(truename), target)) != NULL ) {
		if ( t == ent ) {
			G_Printf ("WARNING: Entity %i used itself.\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
		} else {
			if ( t->use ) {
				t->use (t, ent, activator);
				#ifdef G_LUA
				if(t->luaUse)
				{
					if(activator)
					{
						LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, activator->s.number);
					}
					else
					{
						LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, ENTITYNUM_WORLD);
					}
				}
				#endif
			}
		}
		if ( !ent->inuse ) {
			G_Printf("Entity %i was removed while using targets\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
			return;
		}
	}

	t = NULL;
	while ( (t = G_Find (t, FOFS(falsename), target)) != NULL ) {
		if ( t == ent ) {
			G_Printf ("WARNING: Entity %i used itself.\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
		} else {
			if ( t->use ) {
				t->use (t, ent, activator);
				#ifdef G_LUA
				if(t->luaUse)
				{
					if(activator)
					{
						LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, activator->s.number);
					}
					else
					{
						LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, ENTITYNUM_WORLD);
					}
				}
				#endif
			}
		}
		if ( !ent->inuse ) {
			G_Printf("Entity %i was removed while using targets\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
			return;
		}
	}
	/*================
	End Modification
	================*/

	/*
 	 * RPG-X | GSIO01 | 11/05/2009 | MOD START
	 * target_alert
	 */
	t = NULL;
	while( (t = G_Find(t, FOFS(bluename), target)) != NULL ) {
		if ( t == ent ) {
			G_Printf ("WARNING: Entity %i used itself.\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
		} else {
			if ( t->use ) {
				t->use (t, ent, ent);
				#ifdef G_LUA
				if(t->luaUse)
				{
					if(activator)
					{
						LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, activator->s.number);
					}
					else
					{
						LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, ENTITYNUM_WORLD);
					}
				}
				#endif
			}
		}
		if ( !ent->inuse ) {
			G_Printf("Entity %i was removed while using targets\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
			return;
		}
	}
}

/**
 * \brief Use all of the given entity's targets.
 *
 * Use all of the given entity's targets.
 *
 * @param ent the entity
 * @param activator the initiator of the function call
 */
void G_UseTargets( gentity_t *ent, gentity_t *activator ) {
	if ( !ent ) {
		return;
	}
	G_UseTargets2( ent, activator, ent->target );
}


/**
 * \brief Create a temporary vector.
 *
 * This is just a convenience function
 * for making temporary vectors for function calls
 *
 * @param x x-value
 * @param y y-value
 * @param z z-value
 *
 * @return temporary vector
 */
float	*tv( float x, float y, float z ) {
	static	int		index;
	static	vec3_t	vecs[8];
	float	*v;

	/*
 	 *  use an array so that multiple tempvectors won't collide
	 *  for a while
	 */
	v = vecs[index];
	index = (index + 1)&7;

	v[0] = x;
	v[1] = y;
	v[2] = z;

	return v;
}


/**
 * \brief Converts a vector to a string to be printed.
 *
 * This is just a convenience function
 * for printing vectors
 *
 * @param v the vector
 *
 * @return string representation of the vector
 */
char	*vtos( const vec3_t v ) {
	static	int		index;
	static	char	str[8][32];
	char	*s;

	/* use an array so that multiple vtos won't collide */
	s = str[index];
	index = (index + 1)&7;

	Com_sprintf (s, 32, "(%i %i %i)", (int)v[0], (int)v[1], (int)v[2]);

	return s;
}



/**
 * \brief Converts angles to move directions.
 *
 * The editor only specifies a single value for angles (yaw),
 * but we have special constants to generate an up or down direction.
 * Angles will be cleared, because it is being used to represent a direction
 * instead of an orientation.
 *
 * @param angles the angles
 * @param movedir the movedir
 */
void G_SetMovedir( vec3_t angles, vec3_t movedir ) {
	static vec3_t VEC_UP		= {0, -1, 0};
	static vec3_t MOVEDIR_UP	= {0, 0, 1};
	static vec3_t VEC_DOWN		= {0, -2, 0};
	static vec3_t MOVEDIR_DOWN	= {0, 0, -1};

	if ( VectorCompare (angles, VEC_UP) ) {
		VectorCopy (MOVEDIR_UP, movedir);
	} else if ( VectorCompare (angles, VEC_DOWN) ) {
		VectorCopy (MOVEDIR_DOWN, movedir);
	} else {
		AngleVectors (angles, movedir, NULL, NULL);
	}
	VectorClear( angles );
}

/**
 * \brief Get the yaw from a vector.
 *
 * Get the yaw from a vector.
 *
 * @param vec the vector
 *
 * @return the yaw
 */
float vectoyaw( const vec3_t vec ) {
	float	yaw;
	
	if (vec[YAW] == 0 && vec[PITCH] == 0) {
		yaw = 0;
	} else {
		if (vec[PITCH]) {
			yaw = ( atan2( vec[YAW], vec[PITCH]) * 180 / M_PI );
		} else if (vec[YAW] > 0) {
			yaw = 90;
		} else {
			yaw = 270;
		}
		if (yaw < 0) {
			yaw += 360;
		}
	}

	return yaw;
}

/**
 * \brief Init the entity.
 *
 * Inits a given game entity.
 *
 * @param e the entity
 */
void G_InitGentity( gentity_t *e ) {
	e->inuse = qtrue;
	e->classname = "noclass";
	e->s.number = e - g_entities;
	e->r.ownerNum = ENTITYNUM_NONE;
}

/**
 * \brief Spawns a new entity.
 *
 * Either finds a free entity, or allocates a new one.
 * The slots from 0 to MAX_CLIENTS-1 are always reserved for clients, 
 * and will never be used by anything else.
 * Try to avoid reusing an entity that was recently freed, because it
 * can cause the client to think the entity morphed into something else
 * instead of being removed and recreated, which can cause interpolated
 * angles and bad trails.
 *
 * @return a new entity or NULL
 */
gentity_t *G_Spawn( void ) {
	int			i, force;
	gentity_t	*e;
	/* RPG-X: RedTechie - Get rid of tripmines first */
	gentity_t	*tripwire = NULL;
	int			foundTripWires[MAX_GENTITIES] = {ENTITYNUM_NONE};
	int			tripcount = 0;

	e = NULL;	/* shut up warning */
	i = 0;		/* shut up warning */
	for ( force = 0 ; force < 2 ; force++ ) {
		/* if we go through all entities and can't find one to free, */
		/* override the normal minimum times before use */
		e = &g_entities[MAX_CLIENTS];
		for ( i = MAX_CLIENTS ; i<level.num_entities ; i++, e++) {
			if ( e->inuse ) {
				continue;
			}

			/* the first couple seconds of server time can involve a lot of */
			/* freeing and allocating, so relax the replacement policy */
			if ( !force && e->freetime > level.startTime + 2000 && level.time - e->freetime < 1000 ) {
				continue;
			}

			/* reuse this slot */
			G_InitGentity( e );
			return e;
		}
		if ( i != ENTITYNUM_MAX_NORMAL ) {
			break;
		}
	}
	
	/* RPG-X: RedTechie DEBUG: SHOW HOW MANY ENT's WE HAVE LEFT */
	/*trap_SendServerCommand( -1, va("print \"^1DEBUG: current:%i total:%i\n\"", i, ENTITYNUM_MAX_NORMAL));*/

	if ( i == ENTITYNUM_MAX_NORMAL ) {
		/* RPG-X: RedTechie - Do some rpg-x house cleaning before we decalre the server dead */
		while ( (tripwire = G_Find( tripwire, FOFS(classname), "tripwire" )) != NULL )
		{
			foundTripWires[tripcount++] = tripwire->s.number;
		}
		
		if(tripcount != 0){
			for ( i = 0; i < tripcount; i++ )
			{
				/* remove it... or blow it? */
				if ( &g_entities[foundTripWires[i]] != NULL )
				{
					G_FreeEntity( &g_entities[foundTripWires[i]] );
					foundTripWires[i] = ENTITYNUM_NONE;
				}
			}
			
			G_LogPrintf("RPG-X WARNING: Max entities hit! Removed all tripmines. Restart the server ASAP or suffer a server crash!\n");
			trap_SendServerCommand( -1, "print \"^1RPG-X WARNING: Max entities hit! Removed all tripmines. Restart the server ASAP or suffer a server crash!\n\"");
			
			if ( i == ENTITYNUM_MAX_NORMAL ) {
				G_Error( "G_Spawn: no free entities" );
			}
		}else{
			G_Error( "G_Spawn: no free entities" );
		}
	}
	
	/* RPG-X: RedTechie - Update global entity count */
	RPGEntityCount = i;

	/* open up a new slot */
	level.num_entities++;

	/* let the server system know that there are more entities */
	trap_LocateGameData( level.gentities, level.num_entities, sizeof( gentity_t ), 
		&level.clients[0].ps, sizeof( level.clients[0] ) );

	G_InitGentity( e );
	return e;
}


/**
 * \brief Free an entity.
 *
 * Marks the entity as free.
 *
 * @param ed entity to free
 */
void G_FreeEntity( gentity_t *ed ) {
	trap_UnlinkEntity (ed);		/* unlink from world */

	if ( ed->neverFree ) {
		return;
	}

	#ifdef G_LUA
	/* Lua API callbacks */
	if(ed->luaFree && !ed->client)
	{
		LuaHook_G_EntityFree(ed->luaFree, ed->s.number);
	}
	#endif

	memset (ed, 0, sizeof(*ed));
	ed->classname = "freed";
	ed->freetime = level.time;
	ed->inuse = qfalse;
}

/**
 * \brief Spawn an temporary entity.
 *
 * Spawns an event entity that will be auto-removed
 * The origin will be snapped to save net bandwidth, so care
 * must be taken if the origin is right on a surface (snap towards start vector first)
 *
 * @param origin the origin
 * @param event the event to use for this entity
 *
 * @return the temporary entity
 */
gentity_t *G_TempEntity( vec3_t origin, int event ) {
	gentity_t		*e;
	vec3_t		snapped;

	e = G_Spawn();
	e->s.eType = ET_EVENTS + event;

	e->classname = "tempEntity";
	e->eventTime = level.time;
	e->freeAfterEvent = qtrue;

	VectorCopy( origin, snapped );
	SnapVector( snapped );		/* save network bandwidth */
	G_SetOrigin( e, snapped );

	/* find cluster for PVS */
	trap_LinkEntity( e );

	return e;
}



/*
==============================================================================

Kill box

==============================================================================
*/

/**
 * \brief Kill all that would be inside a new one.
 *
 * Kills all entities that would touch the proposed new positioning
 * of ent.  Ent should be unlinked before calling this!
 * 
 * @param ent the entity
 */
void G_KillBox (gentity_t *ent) {
	int		i, num;
	int		touch[MAX_GENTITIES];
	gentity_t	*hit;
	vec3_t		mins, maxs;

	VectorAdd( ent->client->ps.origin, ent->r.mins, mins );
	VectorAdd( ent->client->ps.origin, ent->r.maxs, maxs );
	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	for (i=0 ; i<num ; i++) {
		hit = &g_entities[touch[i]];
		if ( !hit->client ) {
			continue;
		}
	
		/* nail it */
		G_Damage ( hit, ent, ent, NULL, NULL,
			100000, DAMAGE_NO_PROTECTION, MOD_TELEFRAG);
	}

}

/**
 * \author J2J
 * \brief Push all entities away that are inside a new entity.
 *
 * Basically does teh same as G_KillBox except it will
 * push players and other entities away instead of killing them.
 *
 * @param ent the entity
 *
 * @return was an ent moved?
 */
qboolean G_MoveBox (gentity_t *ent )
{
	int		i, num;
	int		touch[MAX_GENTITIES];
	gentity_t	*hit;
	vec3_t		mins, maxs;
	vec3_t		dir;
	qboolean	movedPlayer = qfalse;
	
	VectorAdd( ent->client->ps.origin, ent->r.mins, mins );
	VectorAdd( ent->client->ps.origin, ent->r.maxs, maxs );

	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	for (i=0 ; i<num ; i++) 
	{
		hit = &g_entities[touch[i]];
		if ( !hit->client || hit->client->ps.clientNum == ent->client->ps.clientNum ) 
		{
			continue;
		}
		
		VectorSet( dir, 0, hit->client->ps.viewangles[YAW], 0 );
		AngleVectors( dir, hit->client->ps.velocity, NULL, NULL );
		VectorScale( hit->client->ps.velocity, -150, hit->client->ps.velocity );
		hit->client->ps.pm_time = 160;		/* hold time */
		hit->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
		
		movedPlayer = qtrue;
	}

	return movedPlayer;	
}

//==============================================================================

/**
 * \brief Adds a new Predictable event.
 *
 * Use for non-pmove events that would also be predicted on the
 * client side: jumppads and item pickups
 * Adds an event+parm and twiddles the event counter
 *
 * @param ent the entity
 * @param event the event
 * @param eventParm any parameters for the event
 */
void G_AddPredictableEvent( gentity_t *ent, int event, int eventParm ) {
	if ( !ent->client ) {
		return;
	}
	BG_AddPredictableEventToPlayerstate( event, eventParm, &ent->client->ps );
}


/**
 * \brief Add a new event.
 * 
 * Adds an event+parm and twiddles the event counter
 *
 * @param ent the entity
 * @param event the event
 * @param eventParm parameter for the event
 */
void G_AddEvent( gentity_t *ent, int event, int eventParm ) {
	int		bits;
	playerState_t *ps = &ent->client->ps;

	if ( !event ) {
		G_Printf( "G_AddEvent: zero event added for entity %i\n", ent->s.number );
		return;
	}

	/* clients need to add the event in playerState_t instead of entityState_t */
	if ( ent->client ) {
		bits = ps->externalEvent & EV_EVENT_BITS;
		bits = ( bits + EV_EVENT_BIT1 ) & EV_EVENT_BITS;
		ps->externalEvent = event | bits;
		ps->externalEventParm = eventParm;
		ps->externalEventTime = level.time;
	} else {
		bits = ent->s.event & EV_EVENT_BITS;
		bits = ( bits + EV_EVENT_BIT1 ) & EV_EVENT_BITS;
		ent->s.event = event | bits;
		ent->s.eventParm = eventParm;
	}
	ent->eventTime = level.time;
}


/**
 * \brief Makes an entity to play a non looping sound.
 *
 * Makes an entity to play a non looping sound.
 *
 * @param ent the entity
 * @param soundIndex the sounds index
 */
void G_Sound( gentity_t *ent, int soundIndex ) {
	gentity_t	*te;

	te = G_TempEntity( ent->r.currentOrigin, EV_GENERAL_SOUND );
	te->s.eventParm = soundIndex;
}


//==============================================================================


/**
 * \brief Set the Origin of an entity.
 *
 * Sets the pos trajectory for a fixed position
 *
 * @param ent the entity
 * @param origin the new origin
 */
void G_SetOrigin( gentity_t *ent, vec3_t origin ) {
	// lets try this to fix setting origins for brush ents without origin brush
	VectorCopy(origin, ent->pos1); // needs testing
	
	VectorCopy( origin, ent->s.pos.trBase );
	ent->s.pos.trType = TR_STATIONARY;
	ent->s.pos.trTime = 0;
	ent->s.pos.trDuration = 0;
	VectorClear( ent->s.pos.trDelta );

	VectorCopy( origin, ent->r.currentOrigin );
	VectorCopy( origin, ent->s.origin); /* RPG-X | GSIO01 | 24.08.2009 */
}

/**
 * \brief Set the angles of an entity.
 *
 * Sets the pos trajectory for a fixed angular position
 *
 * @param ent the entity
 * @param angles the new angles
 */
void G_SetAngles(gentity_t *ent, vec3_t angles) {
	VectorCopy(angles, ent->s.apos.trBase);
	ent->s.apos.trType = TR_STATIONARY;
	ent->s.apos.trTime = 0;
	ent->s.apos.trDuration = 0;
	VectorClear(ent->s.apos.trDelta);

	VectorCopy(angles, ent->r.currentAngles);
	VectorCopy(angles, ent->s.angles);
}

/**
 * \brief Get list of entities around a given origin,
 *
 * Given an origin and a radius, return all entities that are in use that are within the list
 *
 * @param origin the origin
 * @param radius the radius to look in around
 * @param ignore entity to ignore
 * @param takeDamage only list ents that may get damaged?
 * @param ent_list the resulting list
 *
 * @return the number of found entities in the list
 */
int G_RadiusList ( vec3_t origin, float radius,	list_p ignore, qboolean takeDamage, list_p ent_list)					  
{
	float		dist;
	gentity_t*	ent;
	gentity_t*	t = NULL;
	int			entityList[MAX_GENTITIES];
	int			numListedEntities;
	int			i, e;
	vec3_t		mins, maxs;
	vec3_t		v;
	list_iter_p	iter;
	container_p c;
	qboolean	n = qfalse;

	if(ent_list == NULL) {
		return 0;
	}

	if ( radius < 1 ) 
	{
		radius = 1;
	}

	for ( i = 0 ; i < 3 ; i++ ) 
	{
		mins[i] = origin[i] - radius;
		maxs[i] = origin[i] + radius;
	}

	numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

	for ( e = 0 ; e < numListedEntities ; e++ ) 
	{
		n = qfalse;
		ent = &g_entities[entityList[e]];

		if (!(ent->inuse) || ent->takedamage != takeDamage) {
			continue;
		}

		if(ignore != NULL) {
			iter = list_iterator(ignore, LIST_FRONT);
			for(c = list_next(iter); c != NULL; c = list_next(iter)) {
				t = c->data;

				if(t == NULL) {
					continue;
				}

				if(t == ent) {
					n = qtrue;
					break;
				}
			}
			destroy_iterator(iter);
		}

		if(n == qtrue) {
			continue;
		}

		/* find the distance from the edge of the bounding box */
		for ( i = 0 ; i < 3 ; i++ ) 
		{
			if ( origin[i] < ent->r.absmin[i] ) 
			{
				v[i] = ent->r.absmin[i] - origin[i];
			} else if ( origin[i] > ent->r.absmax[i] ) 
			{
				v[i] = origin[i] - ent->r.absmax[i];
			} else 
			{
				v[i] = 0;
			}
		}

		dist = VectorLength( v );
		if ( dist >= radius ) 
		{
			continue;
		}
		
		/* ok, we are within the radius, add us to the incoming list */
		list_append_ptr(ent_list, ent, LT_DATA);
	}
	/*  we are done, return how many we found */
	return ent_list->length;
}

int G_RadiusListOfTypes(list_p classnames, vec3_t origin, float radius, list_p ignore, list_p ent_list) {
	float		dist;
	gentity_t	*ent;
	gentity_t	*t = NULL;
	int			entityList[MAX_GENTITIES];
	int			numListedEntities;
	vec3_t		mins, maxs;
	vec3_t		v;
	int			i, e;
	qboolean	valid = qfalse;
	list_iter_p	iter;
	container_p c;
	qboolean	n = qfalse;

	if(ent_list == NULL) {
		return 0;
	}

	if(classnames == NULL || classnames->length == 0) {
		return 0;
	}

	if ( radius < 1 ) 
	{
		radius = 1;
	}

	for ( i = 0 ; i < 3 ; i++ ) 
	{
		mins[i] = origin[i] - radius;
		maxs[i] = origin[i] + radius;
	}

	numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

	for ( e = 0 ; e < numListedEntities ; e++ ) 
	{
		n = qfalse;
		ent = &g_entities[entityList[e]];

		if(!(ent->inuse)) {
			continue;
		}

		if(ignore != NULL) {
			iter = list_iterator(ignore, LIST_FRONT);
			for(c = list_next(iter); c != NULL; c = list_next(iter)) {
				t = c->data;

				if(t == NULL) {
					continue;
				}

				if(ent == t) {
					n = qtrue;
					break;
				}
			}
			destroy_iterator(iter);
		}

		if(n == qtrue) {
			continue;
		}

		iter = list_iterator(classnames, LIST_FRONT);
		for(c = list_next(iter); c != NULL; c = list_next(iter)) {
			if(!strcmp(ent->classname, (char*)c->data)) {
				valid = qtrue;
				break;
			}
		}
		destroy_iterator(iter);

		if(!valid) {
			continue;
		}

		/* find the distance from the edge of the bounding box */
		for ( i = 0 ; i < 3 ; i++ ) 
		{
			if ( origin[i] < ent->r.absmin[i] ) 
			{
				v[i] = ent->r.absmin[i] - origin[i];
			} else if ( origin[i] > ent->r.absmax[i] ) 
			{
				v[i] = origin[i] - ent->r.absmax[i];
			} else 
			{
				v[i] = 0;
			}
		}

		dist = VectorLength( v );
		if ( dist >= radius ) 
		{
			continue;
		}
		
		/* ok, we are within the radius, add us to the incoming list */
		list_append_ptr(ent_list, ent, LT_DATA);

		valid = qfalse;
	}
	/*  we are done, return how many we found */
	return ent_list->length;
}

/**
 * \brief Find the nearest entity
 *
 * Find the nearest entity.
 *
 * @param classname filter the results by this classname
 * @param origin the origin
 * @param the radoius to look in around
 * @param ignore entity to ignore
 * @param takeDamage only return an entity that may take damage?
 *
 * @return the nearest entity
 */
gentity_t *G_GetNearestEnt(char *classname, vec3_t origin, float radius, list_p ignore, qboolean takeDamage) {
	gentity_t*	nearest = NULL;
	gentity_t*	t = NULL;
	float		distance, minDist;
	vec3_t		dist;
	struct list entList;
	list_iter_p iter;
	container_p c;

	if(!radius) { /* we don't care how far it is away */
		radius = 9999999;
	}
	minDist = radius;

	list_init(&entList, free);
	G_RadiusList(origin, radius, ignore, takeDamage, &entList);

	iter = list_iterator(&entList, LIST_FRONT);
	for(c = list_next(iter); c != NULL; c = list_next(iter)) {
		t = c->data;

		if(t == NULL) {
			continue;
		}

		if(t->s.origin[0] || t->s.origin[1] || t->s.origin[2]) {
			VectorSubtract(origin, t->s.origin, dist);
		} else if(t->r.currentOrigin[0] || t->r.currentOrigin[1] || t->r.currentOrigin[2]) {
			VectorSubtract(origin, t->r.currentOrigin, dist);
		} else if(t->s.pos.trBase[0] || t->s.pos.trBase[1] || t->s.pos.trBase[2]) {
			VectorSubtract(origin, t->s.pos.trBase, dist);
		} else { /* wow none of above ... well then assume it's origin is 0 0 0*/
			VectorCopy(origin, dist);
		}
		distance = VectorLength(dist);
		if(distance < 0) {
			distance *= -1;
		}
		if(distance < minDist) {
			if(classname && !Q_stricmp(classname, t->classname)) {
				minDist = distance;
				nearest = t;
			} else if(!classname) {
				minDist = distance;
				nearest = t;
			}
		}
	}
	destroy_iterator(iter);
	list_clear(&entList);

	return nearest;
}

/**
 * \brief Find the nearest player.
 *
 * Find the nearest player
 *
 * @param origin the origin
 * @param radius the radius to look in around
 * @param ignore entity to ignore
 *
 * @return the nearest player
 */
gentity_t *G_GetNearestPlayer(vec3_t origin, float radius, list_p ignore ) {
	gentity_t*	nearest = NULL;
	gentity_t*	t;
	float		distance, minDist;
	vec3_t		dist;
	struct list entList;
	list_iter_p iter;
	container_p c;

	if(!radius) {
		radius = 999999;
	}
	minDist = radius;

	list_init(&entList, free);
	G_RadiusList(origin, radius, ignore, qtrue, &entList);
	
	iter = list_iterator(&entList, LIST_FRONT);
	for(c = list_next(iter); c != NULL; c = list_next(iter)) {
		t = c->data;

		if(t == NULL) {
			continue;
		}

		if(t->client) {
			VectorSubtract(origin, t->r.currentOrigin, dist);
			distance = VectorLength(dist);
			if(distance < 0)
				distance *= -1;
			if(distance < minDist) {
				minDist = distance;
				nearest = t;
			}
		}
	}
	destroy_iterator(iter);
	list_clear(&entList);

	return nearest;
}

/**
 * \author GSIO01
 * \brief Get all entities with the specified targetname.
 *
 * Get all entities with the specified targetname.
 *
 * @param targetname the targetname
 * @param entities the result
 *
 * @return number of entities found
 */
int G_GetEntityByTargetname(const char *targetname, gentity_t *entities[MAX_GENTITIES]) {
	int i;
	int cnt = 0;
	gentity_t *t;

	for(i = MAX_GENTITIES - 1; i > -1; i--) {
		if(!&g_entities[i]) continue;
		t = &g_entities[i];
		if(t->targetname && !Q_strncmp(t->targetname, targetname, strlen(targetname))) {
			entities[cnt] = t;
			cnt++;
		}
	}

	return cnt;
}

/**
 * \author GSIO01
 * \brief Get all entities with specified target.
 * 
 *  Get all entities matching the specifie target.
 *
 *  @param target target the entities should have
 *  @param entities the result
 *
 *  @return number of matches found
 */
int G_GetEntityByTarget(const char *target, gentity_t *entities[MAX_GENTITIES]) {
	int i;
	int cnt = 0;
	gentity_t *t;

	for(i = MAX_GENTITIES - 1; i > -1; i--) {
		if(!&g_entities[i]) continue;
		t = &g_entities[i];
		if(t->target && !Q_strncmp(t->target, target, strlen(target))) {
			entities[cnt] = t;
			cnt++;
		}
	}

	return cnt;
}

/**
 * \author GSIO01
 * \brief Get all entities with specified brush model
 *
 * Get all entities matching the specified brush model.
 * Normally this only shoud be one entity.
 *
 * @param bmodel brush model to match
 * @param entities the result
 *
 * @return number of matches found
 */
int G_GetEntityByBmodel(char *bmodel, gentity_t *entities[MAX_GENTITIES]) {
	int i;
	int cnt = 0;
	gentity_t *t;

	for(i = MAX_GENTITIES - 1; i > -1; i--) {
		if(!&g_entities[i]) continue;
		t = &g_entities[i];
		if(t->model && !Q_strncmp(t->model, bmodel, strlen(bmodel))) {
			entities[cnt] = t;
			cnt++;
		}
	}

	return cnt;
}

/**
*	Checks if the line of sight between two entities is blocked.
*
*	\author Ubergames - Phenix
*	\date 2/8/2004
*
*	@param ent1 entity one
*	@param ent2 entity two
*
*	@return is line of sight blocked?
*/
qboolean LineOfSight( gentity_t *ent1, gentity_t *ent2 ) {
	trace_t         trace;

	trap_Trace (&trace, ent1->s.pos.trBase, NULL, NULL, ent2->s.pos.trBase, ent1->s.number, MASK_SHOT );

	if ( trace.contents & CONTENTS_SOLID ) {
		return qfalse;
	}
	
	return qtrue;
}

