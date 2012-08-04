// Copyright (C) 2000 Raven Software
//
// g_roff.c -- The main code behind setting up
// and managing 'ROFF' code in EF

#include "g_local.h"
#include "g_roff.h"
// The list of precached ROFFs
roff_list_t	roffs[MAX_ROFFS];
int			num_roffs = 0;
#define		ROFF_DIR	"maps/roffs"

//extern void	Q3_TaskIDComplete( gentity_t *ent, taskID_t taskType );


//-------------------------------------------------------
// G_LoadRoff
//
// Does the fun work of loading and caching a roff file
//	If the file is already cached, it just returns an
//	ID to the cached file.
//
// TiM: Modded it so it conforms to ANSI C parameters
//-------------------------------------------------------

int G_LoadRoff( const char *fileName )
{
	char			file[MAX_QPATH];
	byte			data[2*1024*1024];
	int				len, i, roff_id = 0;
	fileHandle_t	f;
	roff_hdr_t		*header;
	move_rotate_t	*mem;
	move_rotate_t	*roff_data;

	// Before even bothering with all of this, make sure we have a place to store it.
	if ( num_roffs >= MAX_ROFFS )
	{
		G_Printf( S_COLOR_RED "MAX_ROFFS count exceeded.  Skipping load of .ROF '%s'\n", fileName );
		return roff_id;
	}

	// The actual path
	sprintf( file, "%s/%s.rof", ROFF_DIR, fileName );

	// See if I'm already precached
	for ( i = 0; i < num_roffs; i++ )
	{
		if ( Q_stricmp( file, roffs[i].fileName ) == 0 )
		{
			// Good, just return me...avoid zero index
			return i + 1;
		}
	}

#ifdef _DEBUG
	Com_Printf( S_COLOR_GREEN"Caching ROF: '%s'\n", file );
#endif

	// Read the file in one fell swoop
	len = trap_FS_FOpenFile( fileName, &f, FS_READ );

	if ( len <= 0 )
	{
		G_Printf( S_COLOR_RED "Could not open .ROF file '%s'\n", fileName );
		return roff_id;
	}

	trap_FS_Read( &data, len, f );
	data[len+1] = '\0';

	// Now let's check the header info...
	header = (roff_hdr_t *)((byte *)data);

	// ..and make sure it's reasonably valid
	if ( strncmp( header->sHeader, "ROFF", 4 ) !=0 || header->lVersion != ROFF_VERSION || header->fCount <= 0 )
	{
		G_Printf( S_COLOR_RED "Bad header data in .ROF file '%s'\n", fileName );
	}
	else
	{
		// Cool, the file seems to be valid
		int	count = (int)header->fCount;

		// Ask the game to give us some memory to store this pooch
		mem = roffs[num_roffs].data = (move_rotate_t *) G_Alloc( count * sizeof( move_rotate_t ) );

		if ( mem )
		{
			// The allocation worked, so stash this stuff off so we can reference the data later if needed
			roffs[num_roffs].fileName	= G_NewString( file );
			roffs[num_roffs].frames		= count;

			// Step past the header to get to the goods
			roff_data = ( move_rotate_t *)&header[1];

			// Copy all of the goods into our ROFF cache
			for ( i = 0; i < count; i++, roff_data++, mem++ )
			{
				// Copy just the delta position and orientation which can be applied to anything at a later point
				VectorCopy( roff_data->origin_delta, mem->origin_delta );
				VectorCopy( roff_data->rotate_delta, mem->rotate_delta );
			}

			// Done loading this roff, so save off an id to it..increment first to avoid zero index
			roff_id = ++num_roffs;
		}
	}

	trap_FS_FCloseFile( f );

	return roff_id;
}


//-------------------------------------------------------
// G_Roff
//
// Handles applying the roff data to the specified ent
//-------------------------------------------------------

void G_Roff( gentity_t *ent )
{
	int roff_id = G_LoadRoff( ent->roff );

	if ( !roff_id )
	{
		// Couldn't cache this rof
		return;
	}

	// The ID is one higher than the array index
	move_rotate_t	*roff	= &roffs[roff_id - 1].data[ent->roff_ctr];
	int				frames	= roffs[roff_id - 1].frames;

#ifdef _DEBUG
	Com_Printf( S_COLOR_GREEN"ROFF dat: o:<%.2f %.2f %.2f> a:<%.2f %.2f %.2f>\n", 
					roff->origin_delta[0], roff->origin_delta[1], roff->origin_delta[2],
					roff->rotate_delta[0], roff->rotate_delta[1], roff->rotate_delta[2] );
#endif

	// Set up the angle interpolation
	//-------------------------------------
	VectorScale( roff->rotate_delta, ROFF_SAMPLE_RATE, ent->s.apos.trDelta );
	VectorCopy( ent->pos2, ent->s.apos.trBase );
	ent->s.apos.trTime = level.time;
	ent->s.apos.trType = TR_LINEAR;

	// Store what the next apos->trBase should be
	VectorAdd( ent->pos2, roff->rotate_delta, ent->pos2 );


	// Set up the origin interpolation
	//-------------------------------------
	VectorScale( roff->origin_delta, ROFF_SAMPLE_RATE, ent->s.pos.trDelta );
	VectorCopy( ent->pos1, ent->s.pos.trBase );
	ent->s.pos.trTime = level.time;
	ent->s.pos.trType = TR_LINEAR;

	// Store what the next apos->trBase should be
	VectorAdd( ent->pos1, roff->origin_delta, ent->pos1 );

	// See if the ROFF playback is done
	//-------------------------------------
	if ( ++ent->roff_ctr >= frames )
	{
		// We are done, so let me think no more, then tell the task that we're done.
		ent->next_roff_time = 0;

		// Stop any rotation or movement.
		VectorClear( ent->s.pos.trDelta );
		VectorClear( ent->s.apos.trDelta );

		return;
	}

	// Lock me to a 10hz update rate
	ent->next_roff_time = level.time + 100;
}

/*QUAKED func_roff_mover (0 .5 .8) ? START_ON LOOP
Loads all of the movement data from an external ROFF file and applies
it to this entity, creating much more fluid motion.

"model2"	.md3 model to also draw
"angle"		determines the opening direction
"target"	once the animation ends, these targetted entities will be triggered
"wait"		from activation, how many seconds to wait before moving
"endwait"	from the end of the animation, how many seconds to wait b4 firing its targets
"color"		constantLight color
"light"		constantLight radius
"noise"		looping sound file that plays as it animates.
*/
void SP_func_roff_mover ( gentity_t *ent )
{
	ent->think = G_Roff;

}