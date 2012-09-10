#include "g_local.h"

extern void InitMover( gentity_t *ent );
extern gentity_t *G_TestEntityPosition( gentity_t *ent );
void func_usable_use (gentity_t *self, gentity_t *other, gentity_t *activator);

/**
 * \brief Try to get solid again.
 *
 * Thinkfunc: Try to get solid again. If not possible (e.g. it's blocked)
 * wait a frame and try again.
 *
 * @param self the entity itself
 */
void func_wait_return_solid( gentity_t *self )
{
	/* once a frame, see if it's clear. */
	self->clipmask = CONTENTS_BODY;
	if ( !(self->spawnflags&16) || G_TestEntityPosition( self ) == NULL )
	{
		trap_SetBrushModel( self, self->model );
		InitMover( self );
		VectorCopy( self->s.origin, self->s.pos.trBase );
		VectorCopy( self->s.origin, self->r.currentOrigin );
		self->r.svFlags &= ~SVF_NOCLIENT;
		self->s.eFlags &= ~EF_NODRAW;
		self->use = func_usable_use;
		self->clipmask = 0;
		/*
		if ( self->s.eFlags & EF_ANIM_ONCE )
		{//Start our anim
			self->s.frame = 0;
		}
		*/
		if ( !(self->spawnflags&1) && !(self->spawnflags & 1024) )
		{/* START_OFF doesn't effect area portals */
			trap_AdjustAreaPortalState( self, qfalse );
		}
	}
	else
	{
		self->clipmask = 0;
		self->think = func_wait_return_solid;
		self->nextthink = level.time + FRAMETIME;
	}
}

/**
 * \brief Default think function for the func_usable entity.
 *
 * Default think function for the func_usable entity.
 *
 * @param self the entity itself
 */
void func_usable_think( gentity_t *self )
{
	if ( self->spawnflags & 8 )
	{
		/*self->r.svFlags |= SVF_PLAYER_USABLE;*/ /* Replace the usable flag */
		self->use = func_usable_use;
		self->think = 0; /*NULL*/
		self->nextthink = -1;
	}
}

/**
 * \brief Default use function for the func_usable entity.
 *
 * Defualt use function for the func_usable entity.
 *
 * @param self the entity itself
 * @param another entity
 * @param activator the activating entity
 */
void func_usable_use (gentity_t *self, gentity_t *other, gentity_t *activator)
{/* Toggle on and off */
	/* Remap shader */
	if(self->targetShaderName && self->targetShaderNewName) {
		float f = level.time * 0.001;
		AddRemap(self->targetShaderName, self->targetShaderNewName, f);
		trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
	}


	/* RPG-X | GSIO01 | 09/05/2009: */
	if(self->spawnflags & 256) { /* ADMINS_ONLY */
		if(!IsAdmin(activator))
			return;
	}

	if(self->flags & FL_LOCKED) /*  Deactivated */
		return;

	if ( self->spawnflags & 8 )		/* fixme: uurrgh!!!!  that's disgusting */
	{/* ALWAYS_ON */
		/* Remove the ability to use the entity directly */
		/*self->r.svFlags &= ~SVF_PLAYER_USABLE;*/
		/*also remove ability to call any use func at all!*/
		self->use = 0; /*NULL*/
		
		if(self->target && self->target[0])
		{
			if(self->spawnflags & 512)
				G_UseTargets(self, self);
			else
				G_UseTargets(self, activator);
		}
		
		if ( self->wait )
		{
			self->think = func_usable_think;
			self->nextthink = level.time + ( self->wait * 1000 );
		}

		return;
	}
	else if ( !self->count )
	{/*become solid again*/
		self->count = 1;
		func_wait_return_solid( self );
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
			if(self->spawnflags & 512)
				G_UseTargets(self, self);
			else
				G_UseTargets(self, activator);
		}
		self->think = 0; /*NULL*/
		self->nextthink = -1;
		if ( !(self->spawnflags&1) && !(self->spawnflags & 1024))
		{/* START_OFF doesn't effect area portals */
			trap_AdjustAreaPortalState( self, qtrue );
		}
	}
}

/**
 * \brief Default pain function for func_usable entity.
 *
 * Default pain function for func_sable entity.
 *
 * @param self the entity itself
 * @param attacker the attacking entiy
 * @param damage the ammount of damage
 */
void func_usable_pain(gentity_t *self, gentity_t *attacker, int damage)
{
	self->use( self, attacker, attacker );
}

/**
 * \brief Default die function for the func_usable entity.
 *
 * Default die function for the func_usable entity.
 *
 * @param self the entity itself
 * @param inflictor the inflictor
 * @param attacker the attacker
 * @param damage ammount of damage
 * @param mod means of death
 */
void func_usable_die(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod)
{
	self->takedamage = qfalse;
	self->use( self, inflictor, attacker );
}

/*QUAKED func_usable (0 .5 .8) ? STARTOFF AUTOANIM x ALWAYS_ON NOBLOCKCHECK x x x ADMIN_ONLY NO_ACTIVATOR NO_AREAPORTAL DEACTIVATED
START_OFF - the wall will not be there
AUTOANIM - If useing an md3, it will animate
ALWAYS_ON - Doesn't toggle on and off when used, just fires target
NOBLOCKCHECK - Will NOT turn on while something is inside it unless this is checked
ADMIN_ONLY - can only be used by admins
NO_ACTIVATOR - use the ent itself instead the player as activator
NO_AREAPORTAL - don't affect areaportals
DEACTIVATED - start deactivated

A bmodel that can be used directly by the player's "activate" button

"targetname"	When used, will toggle on and off
"target"		Will fire this target every time it is toggled OFF
"model2"		.md3 model to also draw
"color"			constantLight color
"light"			constantLight radius
"wait"			amount of time before the object is usable again (only valid with ALWAYS_ON flag)
"health"		if it has health, it will be used whenever shot at/killed - if you want it to only be used once this way, set health to 1
"messageNum"	the number relating to the message string that will display when the player scans this usable with a tricorder

"team" - This can only be used by this team (2 = blue, 1 = red)
*/

/**
 * \brief The spawn function of the func_usable entity.
 *
 * The spawn function of the func_usable entity.
 *
 * @param self the entity itself
 */
void SP_func_usable( gentity_t *self ) 
{
	trap_SetBrushModel( self, self->model );
	InitMover( self );
	VectorCopy( self->s.origin, self->s.pos.trBase );
	VectorCopy( self->s.origin, self->r.currentOrigin );

	/*Com_Printf( "PointOrigin: { %f, %f, %f }\n", self->s.origin[0], self->s.origin[1], self->s.origin[2] );*/

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

	self->use = func_usable_use;

	if ( self->health )
	{
		self->takedamage = qtrue;
		self->die = func_usable_die;
		self->pain = func_usable_pain;
	}
	
	/*if the map has a usables file, get the data of of that*/
	if ( rpg_scannablePanels.integer )
	{
		if ( level.hasScannableFile )
		{
			int i = 0;
			int j = 0;
			int	messageNum=0;
			int entityNum = self - g_entities;

			self->s.weapon = 0;

			/*TiM - check for a message integer*/
			G_SpawnInt( "messageNum", "0", &messageNum );

			if(self->luaEntity)
				messageNum = self->damage;

			if ( messageNum > 0 )
			{
				while( i < MAX_SCANNABLES )
				{
					if ( level.g_scannables[i] == 0 )
						break;

					if ( level.g_scannables[i] == messageNum )
					{
						self->s.weapon = i+1; /*+1 offset so 0 becomes the default error*/
						break;
					}

					i++;
				}
			}

			/*if no match was found, try the entities list if possible*/
			if ( self->s.weapon == 0 && level.hasEntScannableFile )
			{
				i=0;
				while( i < MAX_ENTSCANNABLES )
				{
					if ( !level.g_entScannables[i][0] && !level.g_entScannables[i][1] )
						break; 

					if( level.g_entScannables[i][0] == entityNum )
					{
						j=0;
						while ( j < MAX_SCANNABLES )
						{
							if ( level.g_scannables[j] == 0 )
								break;

							if ( level.g_scannables[j] == level.g_entScannables[i][1] )
							{
								self->s.weapon = j+1;
								i = MAX_ENTSCANNABLES+1; /*break the outer loop hehe*/
								break;
							}

							j++;
						}
					}
					
					i++;
				}
			}

			if ( self->s.weapon > 0 )
			{
				self->s.eType = ET_TRIC_STRING;
				
				/*DEBUG*/
				/*G_Printf( S_COLOR_RED "Entry found for entity: %i!\n", self-g_entities );*/

				/*Set bounding box for maxs/mins on tricorder rendering*/
				VectorCopy( self->r.mins, self->s.origin2 ); /*mins dimension*/
				VectorCopy( self->r.maxs, self->s.angles2 ); /*maxs*/
			}
		}
		else
		{
			/*
 			 * TiM: Humm... hopefully in my infinite hackiness, this won't screw anything up lol.
			 * Apparently this is the cause of Atlantis dying. The amount of scannable ents is making too
			 * much data for Q3 to handle.
			 * I've CVAR'd it off for now. Hopefully if/when the map gets fixed, it can be put back online.
			 */
			if ( self->message ) { /* If there is a message here, we gotz to get it over to CG */
				self->s.eType = ET_TRIC_STRING; /* Set a special type we can use to identify this over on CG */

				/*Com_Printf( S_COLOR_RED "USABLE MESSAGE: %s\n", self->message );*/
				self->s.time2 = G_TricStringIndex( self->message ); /* encode the message into an info string */
				/* Set bounding box for maxs/mins on tricorder rendering */
				VectorCopy( self->r.mins, self->s.origin2 ); /* mins dimension */
				VectorCopy( self->r.maxs, self->s.angles2 ); /* maxs */
			}
		}
	}

	if(self->spawnflags & 2048)
		self->flags ^= FL_LOCKED;

	trap_LinkEntity (self);

	level.numBrushEnts++;
}

/**
*	\brief loads usable strings
*
*	Loads the usable strings.
*	\return sucessfully loaded?
*	\author Ubergames - TiM
*/
qboolean G_SetupUsablesStrings( void )
{
	char			*serverInfo;
	char			fileRoute[MAX_QPATH];
	char			*buffer;
	int				file_len;
	char			*textPtr, *token;
	fileHandle_t	f;
	int				i, j;

	level.hasScannableFile = qfalse;
	level.hasEntScannableFile = qfalse;

	serverInfo = (char *)malloc(MAX_TOKEN_CHARS * sizeof(char));
	if(!serverInfo) {
		return qfalse;
	}	

	/* get the map name out of the server data */
	trap_GetServerinfo( serverInfo, MAX_TOKEN_CHARS * sizeof(char) );

	/* setup the file route */
	Com_sprintf( fileRoute, sizeof( fileRoute ), "maps/%s.usables", Info_ValueForKey( serverInfo, "mapname" ) );

	file_len = trap_FS_FOpenFile( fileRoute, &f, FS_READ );

	free(serverInfo);

	/* It's assumed most maps won't have this feature, so just exit 'gracefully' */
	if ( file_len<=1 )
	{
		trap_FS_FCloseFile( f );
		/* G_Printf( S_COLOR_YELLOW "WARNING: No file named %s was found.\n", fileRoute ); */
		return qfalse;
	}

	buffer = (char *)malloc(32000 * sizeof(char));
	if(!buffer) {
		trap_FS_FCloseFile(f);
		return qfalse;
	}

	/* fill the buffer with the file data */
	trap_FS_Read( buffer, file_len, f );
	buffer[file_len] = '0';

	trap_FS_FCloseFile( f );
	
	if ( !buffer[0] )
	{
		G_Printf( S_COLOR_RED "ERROR: Attempted to load %s, but no data was inside!\n", fileRoute );
		free(buffer);
		return qfalse;
	}

	G_Printf( "Usables file %s located. Proceeding to load scan data.\n", fileRoute );

	COM_BeginParseSession();
	textPtr = buffer;

	i = 0;	/* used for the main arrays indices */

	while( 1 )
	{
		token = COM_Parse( &textPtr );
		if ( !token[0] )
			break;

		if ( !Q_strncmp( token, "UsableDescriptions", 18 ) )
		{
			token = COM_Parse( &textPtr );
			if ( Q_strncmp( token, "{", 1 ) != 0 )
			{
				G_Printf( S_COLOR_RED "ERROR: UsableDescriptions had no opening brace ( { )!\n" );
				continue;
			}

			level.hasScannableFile = qtrue;

			token = COM_Parse( &textPtr );

			/* expected format is 'id' <space> 'string' */
			while ( Q_strncmp( token, "}", 1 ) )
			{
				if ( !token[0] )
					break;

				if ( !Q_strncmp( token, "UsableEntities", 14 ) )
				{
					token = COM_Parse( &textPtr );
					if ( Q_strncmp( token, "{", 1 ) )
					{
						G_Printf( S_COLOR_RED "ERROR: UsableEntities had no opening brace ( { )!\n" );
						continue;
					}

					level.hasEntScannableFile = qtrue;

					token = COM_Parse( &textPtr );

					j = 0;
					while( Q_strncmp( token, "}", 1 ) )
					{
						if ( !token[0] )
							break;

						if ( token[0] != 'e' )
						{
							SkipRestOfLine( &textPtr );
							continue;
						}

						token++; /* skip the 'e' */

						level.g_entScannables[j][0] = atoi( token );
						token = COM_ParseExt( &textPtr, qfalse );
						level.g_entScannables[j][1] = atoi( token );

						/* there's no way clients are scannable in here, so just validate the entry b4 proceeding */
						if ( level.g_entScannables[j][0] > MAX_CLIENTS-1 && level.g_entScannables[j][1] > 0 )
							j++;

						token = COM_Parse( &textPtr );
					}
				}
				else
				{
					level.g_scannables[i] = atoi( token );

					/* ensure a valid number was passed, else ignore it */
					if ( level.g_scannables[i] > 0 )
						i++;

					/* we don't need the text on the server side */
					SkipRestOfLine( &textPtr );

					token = COM_Parse( &textPtr );
				}
			}
		}
	}

	free(buffer);
	return qtrue;
}

