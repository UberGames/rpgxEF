//This file contains environmental effects for the designers

#include "cg_local.h"
#include "fx_local.h"

// these flags should be synchronized with the spawnflags in g_fx.c for fx_bolt
#define BOLT_SPARKS		(1<<0)
#define BOLT_BORG		(1<<1)


qboolean SparkThink( localEntity_t *le )
{
	vec3_t	dir, direction, start, end;
	vec3_t	velocity;
	float	scale = 0, alpha = 0;
	int		numSparks = 0, i = 0, j = 0;
	sfxHandle_t snd = cgs.media.envSparkSound1;

	switch(irandom(1, 3))
	{
	case 1:
		snd = cgs.media.envSparkSound1;
		break;
	case 2:
		snd = cgs.media.envSparkSound2;
		break;
	case 3:
		snd = cgs.media.envSparkSound3;
		break;
	}
	trap_S_StartSound (le->refEntity.origin, ENTITYNUM_WORLD, CHAN_BODY, snd );

	CG_InitLensFlare( le->refEntity.origin, 
						40, 40,
						colorTable[CT_YELLOW], 1.2, 2.0, 1600, 200,
						colorTable[CT_YELLOW], 1600, 200, 600, 6,  qtrue, 
						0, 0, qfalse, qtrue, 
						qtrue, 0.7, cg.time, 90, 0, 300);


	VectorCopy(le->data.spawner.dir, dir);

	//AngleVectors( dir, dir, NULL, NULL );
	for ( j = 0; j < 3; j ++ )
		direction[j] = dir[j] + (0.25f * crandom());

	VectorNormalize( direction );

	//trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, cgi_S_RegisterSound( va( "sound/world/ric%d.wav", (rand() & 2)+1) ) );

	numSparks = 8 + (random() * 4.0f);
	
	scale = 0.2f + (random() *0.4);
	VectorMA( le->refEntity.origin, 24.0f + (crandom() * 4.0f), dir, end );

	//One long spark
	FX_AddLine( le->refEntity.origin,
				end,
				1.0f,
				scale,
				0.0f,
				1.0f,
				0.25f,
				125.0f,
				cgs.media.sparkShader );
	
	for ( i = 0; i < numSparks; i++ )
	{	
		scale = 0.2f + (random() *0.4);

		for ( j = 0; j < 3; j ++ )
			direction[j] = dir[j] + (0.25f * crandom());
		
		VectorNormalize(direction);

		VectorMA( le->refEntity.origin, 0.0f + ( random() * 2.0f ), direction, start );
		VectorMA( start, 2.0f + ( random() * 16.0f ), direction, end );

		FX_AddLine( start,
					end,
					1.0f,
					scale,
					0.0f,
					1.0f,
					0.25f,
					125.0f,
					cgs.media.sparkShader );
	}

	if ( rand() & 1 )
	{
		numSparks = 1 + (random() * 2.0f);
		for ( i = 0; i < numSparks; i++ )
		{	
			scale = 0.5f + (random() * 0.5f);

			VectorScale( direction, 250, velocity );

			FX_AddTrail(	start,
							velocity,
							qtrue,
							8.0f,
							-32.0f,
							scale,
							-scale,
							1.0f,
							0.5f,
							0.25f,
							700.0f,
							cgs.media.sparkShader);

		}
	}

	VectorMA( le->refEntity.origin, 1, dir, direction );

	scale = 6.0f + (random() * 8.0f);
	alpha = 0.1 + (random() * 0.4f);

	VectorSet( velocity, 0, 0, 8 );

	FX_AddSprite(	direction, 
					velocity, 
					qfalse, 
					scale,
					scale,
					alpha,
					0.0f,
					random()*45.0f,
					0.0f,
					1000.0f,
					cgs.media.steamShader );

	return qtrue;
}

/*
======================
CG_Spark

Creates a spark effect
======================
*/

void CG_Spark( vec3_t origin, vec3_t normal, int delay, int killTime )
{
	// give it a lifetime of 10 seconds because the refresh thinktime in g_fx.c is 10 seconds
	FX_AddSpawner( origin, normal, NULL, NULL, qfalse, delay, 1.5, killTime, SparkThink, 100 ); //10000

}



qboolean SteamThink( localEntity_t *le )
{
	float	speed = 200;
	vec3_t	direction;
	vec3_t	velocity		= { 0, 0, 128 };
	float	scale, dscale;
	vec3_t	origin;

	//FIXME: Whole lotta randoms...

	VectorCopy( le->data.spawner.dir, direction );
	//AngleVectors( direction, direction, NULL, NULL );
	//VectorNormalize(direction);

	//TiM : Offset by 1.  sometimes it can spawn in walls and the particles act weird then
	VectorMA( le->refEntity.origin, 1, direction, origin );

	direction[0] += (direction[0] * crandom() * le->data.spawner.variance);
	direction[1] += (direction[0] * crandom() * le->data.spawner.variance);
	direction[2] += (direction[0] * crandom() * le->data.spawner.variance);

	VectorScale( direction, speed, velocity );

	scale = 4.0f + (random());
	dscale = scale * 4.0;

	FX_AddSprite(	origin,
					velocity, 
					qfalse, 
					scale, 
					dscale, 
					1.0f, 
					0.0f,
					random() * 360,
					0.25f,
					300,//(len / speed) * 1000, 
					cgs.media.steamShader );

	return qtrue;
}

/*
======================
CG_Steam

Creates a steam effect
======================
*/

void CG_Steam( vec3_t position, vec3_t dir, int killTime )
{
	// give it a lifetime of 10 seconds because the refresh thinktime in g_fx.c is 10 seconds
	FX_AddSpawner( position, dir, NULL, NULL, qfalse, 0, 0.15, killTime, SteamThink, 100 ); //
}

/*
======================
CG_Bolt

Creates a electricity bolt effect
======================
*/
#define DATA_EFFECTS	0
#define DATA_CHAOS		1
#define DATA_RADIUS		2

//-----------------------------
void BoltSparkSpew( vec3_t origin, vec3_t normal )
{
	float	scale = 1.0f + ( random() * 1.0f );
	int		num = 0, i = 0;
	vec3_t	vel;

	trap_R_AddLightToScene( origin, 75 + (rand()&31), 1.0, 0.8, 1.0 );

	// Drop some sparks
	num = (int)(random() * 2) + 2;

	for ( i = 0; i < num; i++ )
	{
		scale = 0.6f + random();
		if ( rand() & 1 )
			FXE_Spray( normal, 70, 80, 0.9f, vel);
		else
			FXE_Spray( normal, 80, 200, 0.5f, vel);

		FX_AddTrail( origin, vel, qfalse, 8.0f + random() * 8, -48.0f,
								scale, -scale, 1.0f, 0.8f, 0.4f, 600.0f, cgs.media.spark2Shader );
	}
}


qboolean BoltFireback( localEntity_t *le)
{
//localEntity_t *FX_AddElectricity( vec3_t origin, vec3_t origin2, float stScale, float scale, float dscale, 
//									float startalpha, float endalpha, float killTime, qhandle_t shader, float deviation );							
	float killTime = (0 == le->data.spawner.delay)?10000:200;

	FX_AddElectricity(le->refEntity.origin, le->data.spawner.dir, 0.2f, 15.0, -15.0, 1.0, 0.5, killTime,
		cgs.media.bolt2Shader, le->data.spawner.variance );
	// is this spawner on a random delay?
	if (le->data.spawner.data1)
	{
		le->data.spawner.delay = flrandom(0,5000);
	}
	return qtrue;
}

//-----------------------------
qboolean BorgBoltFireback( localEntity_t *le)
{
	float killTime = (0 == le->data.spawner.delay)?10000:200;

	FX_AddElectricity(le->refEntity.origin, le->data.spawner.dir, 0.2f, 15.0, -5.0, 1.0, 0.5, killTime,
		cgs.media.borgLightningShaders[1], le->data.spawner.variance );
	// is this spawner on a random delay?
	if (le->data.spawner.data1)
	{
		le->data.spawner.delay = flrandom(0,5000);
	}
	return qtrue;
}

//-----------------------------
qboolean BoltFirebackSparks( localEntity_t *le)
{
	vec3_t	dir;
	float killTime = (0 == le->data.spawner.delay)?10000:200;

	VectorSubtract(le->refEntity.origin, le->data.spawner.dir, dir);
	VectorNormalize(dir);
	FX_AddElectricity(le->refEntity.origin, le->data.spawner.dir, 0.2f, 15.0, -15.0, 1.0, 0.5, killTime,
		cgs.media.bolt2Shader, le->data.spawner.variance );
	BoltSparkSpew(le->data.spawner.dir, dir);
	// is this spawner on a random delay?
	if (le->data.spawner.data1)
	{
		le->data.spawner.delay = flrandom(0,5000);
	}
	return qtrue;
}

//-----------------------------
qboolean BorgBoltFirebackSparks( localEntity_t *le)
{
	vec3_t	dir;
	float killTime = (0 == le->data.spawner.delay)?10000:200;

	VectorSubtract(le->refEntity.origin, le->data.spawner.dir, dir);
	VectorNormalize(dir);
	FX_AddElectricity(le->refEntity.origin, le->data.spawner.dir, 0.2f, 15.0, -15.0, 1.0, 0.5, killTime,
		cgs.media.borgLightningShaders[0], le->data.spawner.variance );
	BoltSparkSpew(le->data.spawner.dir, dir);
	// is this spawner on a random delay?
	if (le->data.spawner.data1)
	{
		le->data.spawner.delay = flrandom(0,5000);
	}
	return qtrue;
}

//-----------------------------
void CG_Bolt( centity_t *cent )
{
	localEntity_t *le = NULL;
	qboolean	bSparks = (qboolean)(cent->currentState.eventParm & BOLT_SPARKS);
	qboolean	bBorg = (qboolean)(cent->currentState.eventParm & BOLT_BORG);
	float		radius = cent->currentState.angles2[0], chaos = cent->currentState.angles2[1];
	float		delay = cent->currentState.time2 * 1000; // the value given by the designer is in seconds
	qboolean	bRandom = qfalse;

	if (delay < 0)
	{
		// random
		delay = flrandom(0.1, 5000.0);
		bRandom = qtrue;
	}
	if (delay > 10000)
	{
		delay = 10000;
	}

	if ( bBorg )
	{
		if (bSparks)
		{
			le = FX_AddSpawner( cent->lerpOrigin, cent->currentState.origin2, NULL, NULL, qfalse, delay,
				chaos, 10000, BorgBoltFirebackSparks, radius );
		}
		else
		{
			le = FX_AddSpawner( cent->lerpOrigin, cent->currentState.origin2, NULL, NULL, qfalse, delay,
				chaos, 10000, BorgBoltFireback, radius );
		}
	}
	else
	{
		if (bSparks)
		{
			le = FX_AddSpawner( cent->lerpOrigin, cent->currentState.origin2, NULL, NULL, qfalse, delay,
				chaos, 10000, BoltFirebackSparks, radius );
		}
		else
		{
			le = FX_AddSpawner( cent->lerpOrigin, cent->currentState.origin2, NULL, NULL, qfalse, delay,
				chaos, 10000, BoltFireback, radius );
		}
	}
	if (bRandom)
	{
		le->data.spawner.data1 = 1;
	}
}

void CG_TransporterPad(vec3_t origin)
{
	FX_TransporterPad(origin);
}

/*
===========================
Drip

Create timed drip effect
===========================
*/

qboolean DripCallback( localEntity_t *le )
{
	localEntity_t *trail = NULL;
	qhandle_t	shader = 0;

	switch (le->data.spawner.data1)
	{
	case 1:
		shader = cgs.media.oilDropShader;
		break;
	case 2:
		shader = cgs.media.greenDropShader;
		break;
	case 0:
	default:
		shader = cgs.media.waterDropShader;
		break;
	}
	trail = FX_AddTrail(le->refEntity.origin, le->pos.trDelta, qfalse, 4, -2, 1, 0, 0.8, 0.4, 0.0,
		300, shader);
	trail->leFlags |= LEF_ONE_FRAME;

	return qtrue;
}

//------------------------------------------------------------------------------
qboolean DripSplash( localEntity_t *le )
{
	float	scale = 1.0f + ( random() * 1.0f );
	int		num = 0, i = 0;
	vec3_t	vel, normal, origin;
	qhandle_t	shader = 0;

	switch (le->data.spawner.data1)
	{
	case 1:
		shader = cgs.media.oilDropShader;
		break;
	case 2:
		shader = cgs.media.greenDropShader;
		break;
	case 0:
	default:
		shader = cgs.media.waterDropShader;
		break;
	}

	VectorCopy(le->data.spawner.dir, normal);
	VectorCopy(le->refEntity.origin, origin);

	// splashing water droplets. which, I'm fairly certain, is an alternative band from Europe.
	num = (int)(random() * 2) + 6;

	for ( i = 0; i < num; i++ )
	{
		scale = 0.6f + random();
		if ( rand() & 1 )
			FXE_Spray( normal, 110, 80, 0.9f, vel);
		else
			FXE_Spray( normal, 150, 150, 0.5f, vel);

		FX_AddTrail( origin, vel, qtrue, 4.0f, 0.0f,
								scale, -scale, 1.0f, 0.8f, 0.4f, 200.0f, shader );
	}

	switch( rand() & 3 )
	{
	case 1:
		trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_BODY, cgs.media.waterDropSound1 );
		break;
	case 2:
		trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_BODY, cgs.media.waterDropSound2 );
		break;
	default:
		trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_BODY, cgs.media.waterDropSound3 );
		break;
	}
	return qtrue;

}

qboolean JackTheDripper( localEntity_t *le )
{
	trace_t		trace;
	vec3_t		vel, down = {0,0,-1}, end, origin, new_origin;
	float		time, dis, diameter = 1.0;
	qhandle_t	shader = 0;
	int			maxDripsPerLifetime = 200; // given a 10 second lifetime
	int			desiredDrips = 1 + (int)(le->data.spawner.variance * (maxDripsPerLifetime-1)); // range of (1...max)
	float		percentLife = 1.0f - (le->endTime - cg.time)*le->lifeRate;
	localEntity_t *splash = NULL;

	switch (le->data.spawner.data1)
	{
	case 1:
		shader = cgs.media.oilDropShader;
		break;
	case 2:
		shader = cgs.media.greenDropShader;
		break;
	case 0:
	default:
		shader = cgs.media.waterDropShader;
		break;
	}

	// do we need to add a drip to maintain our drips-per-second rate?
	while ( (int)(flrandom(percentLife-0.05,percentLife+0.05)*desiredDrips) > le->data.spawner.data2)
	{
		VectorCopy(le->refEntity.origin, origin);

		// the more drips per second, spread them out from our origin point
		fxRandCircumferencePos(origin, down, 10*le->data.spawner.variance, new_origin);

		// Ideally, zero should be used for vel...so just use something sufficiently close
		VectorSet( vel, 0, 0, -0.00001 );

		// Find out where it will hit
		VectorMA( new_origin, 1024, down, end );
		CG_Trace( &trace, new_origin, NULL, NULL, end, 0, MASK_SHOT );
		if ( trace.fraction < 1.0 )
		{
			VectorSubtract( trace.endpos, new_origin, end );
			dis = VectorNormalize( end );

			time = sqrt( 2*dis / DEFAULT_GRAVITY ) * 1000;	// Calculate how long the thing will take to travel that distance

			// Falling drop
			splash = FX_AddParticle( new_origin, vel, qtrue, diameter, 0.0, 0.8, 0.8, 0.0, 0.0, time, shader, DripCallback );
			splash->data.spawner.data1 = le->data.spawner.data1;

			splash = FX_AddSpawner(trace.endpos, trace.plane.normal, vel, NULL, qfalse, time, 0, time + 200, DripSplash, 10);
			splash->data.spawner.data1 = le->data.spawner.data1;
		}
		else
		// Falling a long way so just send one that will fall for 2 secs, but don't spawn a splash
		{
			FX_AddParticle( new_origin, vel, qtrue, diameter, 0.0, 0.8, 0.8, 0.0, 0.0, 2000, shader, 0/*NULL*/ );
		}
		//increase our number-of-drips counter
		le->data.spawner.data2++;
	}
	return qtrue;
}

//------------------------------------------------------------------------------
void CG_Drip(centity_t *cent, int killTime )
{
	vec3_t down = {0,0,-1};
	localEntity_t *le = NULL;

	// clamp variance to [0...1]
	if (cent->currentState.angles2[0] < 0)
	{
		cent->currentState.angles2[0] = 0;
	}
	else if (cent->currentState.angles2[0] > 1)
	{
		cent->currentState.angles2[0] = 1;
	}
	// cent->currentState.angles2[0] is the degree of drippiness
	// cent->currentState.time2 is the type of drip (water, oil, etc.)
	le = FX_AddSpawner( cent->lerpOrigin, down, NULL, NULL, qfalse, 0,
		cent->currentState.angles2[0], killTime, JackTheDripper, cent->currentState.time2 );
	//init our number-of-drips counter
	le->data.spawner.data2 = 0;
}

//------------------------------------------------------------------------------
void CG_Chunks( vec3_t origin, vec3_t dir, float scale, material_type_t type )
{
	int				i, j, k;
	int				numChunks;
	float			baseScale = 1.0f, dist, radius;
	vec3_t			v;
	sfxHandle_t		snd = 0;
	localEntity_t	*le;
	refEntity_t		*re;

	if ( type == MT_NONE )
		return;

	if ( type >= NUM_CHUNK_TYPES )
	{
		CG_Printf( "^6Chunk has invalid material %d!\n", type);
		type = MT_METAL;	//something legal please
	}

	switch( type )
	{
	case MT_GLASS:
	case MT_GLASS_METAL:
		snd = cgs.media.glassChunkSound;
		break;

	case MT_METAL:
	case MT_STONE:
	case MT_WOOD:
	default:
		snd = cgs.media.metalChunkSound;
		break;
	}

	trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_BODY, snd );

	numChunks = irandom( 8, 12 );

	// LOD num chunks
	VectorSubtract( cg.snap->ps.origin, origin, v );
	dist = VectorLength( v );

	if ( dist > 512 )
	{
		numChunks *= 512.0 / dist;		// 1/2 at 1024, 1/4 at 2048, etc.
	}

	// attempt to scale the size of the chunks based on the size of the brush
	radius = baseScale + ( ( scale - 128 ) / 128 );

	for ( i = 0; i < numChunks; i++ )
	{
		le = CG_AllocLocalEntity();
		re = &le->refEntity;

		le->leType = LE_FRAGMENT;
		le->endTime = cg.time + 2000;

		VectorCopy( origin, re->origin );

		for ( j = 0; j < 3; j++ )
		{
			re->origin[j] += crandom() * 12;
		}
		VectorCopy( re->origin, le->pos.trBase );

		//Velocity
		VectorSet( v, crandom(), crandom(), crandom() );
		VectorAdd( v, dir, v );
		VectorScale( v, flrandom( 100, 350 ), le->pos.trDelta );

		//Angular Velocity
		VectorSet( le->angles.trBase, crandom() * 360, crandom() * 360, crandom() * 360 );
		VectorSet( le->angles.trDelta, crandom() * 90, crandom() * 90, crandom() * 90 );

		AxisCopy( axisDefault, re->axis );

		le->data.fragment.radius = flrandom( radius * 0.7f, radius * 1.3f );

		re->nonNormalizedAxes = qtrue;

		if ( type == MT_GLASS_METAL )
		{
			if ( rand() & 1 )
			{
				re->hModel = cgs.media.chunkModels[MT_METAL][irandom(0,5)];
			}
			else
			{
				re->hModel = cgs.media.chunkModels[MT_GLASS][irandom(0,5)];
			}
		}
		else
		{
			re->hModel = cgs.media.chunkModels[type][irandom(0,5)];
		}
		
		le->pos.trType = TR_GRAVITY;
		le->pos.trTime = cg.time;
		le->angles.trType = TR_INTERPOLATE;
		le->angles.trTime = cg.time;
		le->bounceFactor = 0.2f + random() * 0.2f;
		le->leFlags |= LEF_TUMBLE;

		re->shaderRGBA[0] = re->shaderRGBA[1] = re->shaderRGBA[2] = re->shaderRGBA[3] = 255;

		// Make sure that we have the desired start size set
		for( k = 0; k < 3; k++)
		{
			VectorScale( le->refEntity.axis[k], le->data.fragment.radius, le->refEntity.axis[k] );
		}
	}
}

//TiM - org is where teh spray originates, and end is where the splash ends
//The fun part is corellating the height and direction to the bezier function... heh
void CG_FountainSpurt( vec3_t org, vec3_t end )
{
	int			/*i,*/ t;
	vec3_t		org1, org2, cpt1, cpt2;
	vec3_t		dir/*, dir2*/;
	//vec3_t		rgb = { 0.4f, 0.7f, 0.8f };
	//float		distance;
	//FXBezier	*fxb;
	localEntity_t	*le;

	// offset table, could have used sin/cos, I suppose
	//TiM - This was for the 4-way fountain. not needed no more
	/*const float m[][2] = { 
						1, 0,
						0, 1,
						-1, 0,
						0, -1 };*/

	// The origin shouldn't be in solid, otherwise the ent won't think.  So, place the spawner above
	//	the solid object, then move the spout spawn points down to where they should be.
	//org[2] -= 56; // magic number stuff
	//TiM... uh O_o I think that was another thing with the 4 way fountain... won't work too well O_o
	//Lessee what happens if we blatently disregard this lol

	// Create four spouts 
	//for ( i = 0; i < 4; i++ ) //TiM No!
	//{
		//TiM - Set a direction we can use to figure out the end from the start.... >.<
		//We'll need the literal direction between X + Y... Z is pretty easy to figure out
		//VectorSubtract( end, org, dir );
		//distance = (dir[0] + dir[1]) * 0.5f; //so get the average

		// Move the spout out from the exact center
		VectorCopy( org, org1 );

		//TiM: No offset for now
		//org1[0] += 35 * m[i][0];
		//org1[1] += 35 * m[i][1];

		// Create our Bezier path control points
		//TiM: judging from the hardcoded values, point 1 is positioned 45% units away from length, on same Z-axis 
		//VectorSet( cpt1, 50 * m[i][0], 50 * m[i][1], 0 );
		//VectorAdd( org1, cpt1, cpt1 );
		//VectorSet( cpt1, org1[0] + 100, org[1] + 100, 0 );

		VectorSet( cpt1, org[0] + ( end[0] - org[0] ) * 0.65f, org[1] + ( end[1] - org[1] ) * 0.65f, org1[2] );
		//VectorAdd( org1, cpt1, cpt1 );

		//Com_Printf("ORG = { %f, %f, %f }, CPT = { %f, %f, %f }\n", org1[0], org1[1], org1[2], cpt1[0], cpt1[1], cpt1[2] );

		//point 2 is positioned
		//TiM - point 2 I guess is the remaining 55%
		//VectorSet( cpt2, 60 * m[i][0], 60 * m[i][1], -78 );
		//VectorAdd( org1, cpt2, cpt2 );

		//VectorSet( cpt2, distance * 0.55, distance * 0.55, -Q_fabs(org1[2] - end[2]) );
		//VectorAdd( org1, cpt2, cpt2 );
		VectorCopy( end, cpt2 );

		// Create the second endpoint--for now just try and use the last control point 
		VectorCopy( cpt2, org2 );

		// Add the main spout
		le = FX_AddBezier( org1, org2, cpt1, cpt2, NULL, NULL, NULL, NULL, 4, 90, 
			cgs.media.fountainShader);

		//if ( fxb )
		//	fxb->SetSTScale( 0.7f );

		// Add a hazy faint spout
		le = FX_AddBezier( org1, org2, cpt1, cpt2, NULL, NULL, NULL, NULL, 10, 200, 
			cgs.media.fountainShader);

		//if ( fxb )
		//	fxb->SetSTScale( 0.7f );

		// Create misty bits at the impact point
		VectorSet( dir, crandom(), crandom(), crandom() + 4 ); // always move mostly up
		VectorScale( dir, random() * 3 + 2, dir );
		FX_AddSprite( org2, dir, qfalse, 20, -8, 0.3f, 0.0, 0, 0, 600, cgs.media.steamShader );

		// ripple shader
		VectorSet( dir, 0, 0, 1 ); // normal
		//VectorSet( dir2, crandom() * 8, crandom() * 8, 0 ); // random drift
		FX_AddQuad( org2, dir, 14.0f, 6.0f + random() * 16.0f, 0.2f, 0.0f, crandom() * 50, 800, cgs.media.rippleShader );

		// Spray from nozzle
		for ( t = 0; t < 2; t++ )
		{
			//VectorSet( dir, 45 * m[i][0] + crandom() * 12, 45 * m[i][1] + crandom() * 12, crandom() * 16 );
			//VectorSet( dir2, -5 * m[i][0], -5 * m[i][1], -95 );
			VectorSet( dir, 0.4 * ( end[0] - org[0] ) + crandom() * 12, 0.4 * ( end[1] - org[1] ) + crandom() * 12, crandom() * 16 );
			//VectorSet( dir2, -0.04 * distance, -0.04 * distance, -95 );

			FX_AddSprite( org1, dir, qtrue, 0.9f, 0.0f, 0.7f, 0.1f, 0.0f, 0.0f, 400.0f, cgs.media.waterDropShader );
		}

		// Impact splashes
		for ( t = 0; t < 3; t++ )
		{
			VectorCopy( org2, org1 );
			org1[0] += crandom() * 2;
			org1[1] += crandom() * 2;
			//VectorSet( dir, m[i][0] * 14 + crandom() * 16, m[i][1] * 14 + crandom() * 16, 50 + random() * 50 );
			VectorSet( dir, ( end[0] - org[0] ) * 0.127 + crandom() * 16, ( end[1] - org[1] ) * 0.127 + crandom() * 16, 50 + random() * 50 );
			//VectorSet( dir2, 0, 0, -250 );
			FX_AddSprite( org1, dir, qtrue, 1.1f, -0.4f, 0.7f, 0.1f, 0.0f, 0.0f, 400.0f, cgs.media.waterDropShader );
		}
	//}
	//Com_Printf( S_COLOR_RED "Rendering Fountain\n" );
}

/*================
CG_ElectricalExplosion
=================*/

void smoke_puffs( vec3_t position, vec3_t dest, vec3_t dir, vec3_t user )
{
	vec3_t direc;

	direc[0] = crandom() * 7;
	direc[1] = crandom() * 7;
	direc[2] = random() * 6 + 8;

	FX_AddSprite( position, direc, qfalse, 6.0f, 10.0f, 0.3f, 0.0f, 0.0f, 0.0f, 2200, cgs.media.steamShader );
}

//------------------------------------------------------------------------------
void electric_spark( vec3_t pos, vec3_t normal, vec3_t dir, vec3_t user )
{
	CG_Spark( pos, normal, 0, 10000 );
}

//------------------------------------------------------------------------------
void CG_ElectricalExplosion( vec3_t start, vec3_t dir, float radius )
{
	localEntity_t	*le;
	localEntity_t	*particle; //FXTrail
	vec3_t			pos, temp/*, angles*/;
	int				i, numSparks;
	float			scale, dscale;

	// Spawn some delayed smoke
	/*FX_AddSpawner( start, dir, NULL, NULL, 150, 40, qfalse, 9000, smoke_puffs );
	vectoangles( dir, angles );
	FX_AddSpawner( start, angles, NULL, NULL, 900, 800, 4000, FXF_DELAY_SPAWN, electric_spark );*/

	// Create the sparks for the explosion
	numSparks = 46 + (random() * 8.0f);
	 
	for ( i = 0; i < numSparks; i++ )
	{	
		scale = 0.7f + random(); //0.2
		dscale = -scale*2;

		particle = FX_AddTrail( start,
								NULL,
								qfalse,
								8.0f + random() * 6,
								-16.0f,
								scale,
								-scale,
								1.0f,
								0.0f,
								0.25f,
								700.0f,
								cgs.media.spark2Shader );
 
		/*if ( particle == NULL )
			return;*/

		//FXE_Spray( dir, 200, 200, 0.3f, 500 + (rand() & 300), (FXPrimitive *) particle );
	}

	// Create some initial smoke puffs
	for (i = 0; i < 12; i++)
	{
		VectorCopy( dir, temp );
		temp[0] += crandom() * 0.5f;
		temp[1] += crandom() * 0.5f;
		temp[2] += crandom() * 0.5f;

		VectorMA( start, random() * 16 + 8, temp, pos );
		VectorScale( temp, random() * 4 + 5, temp );

		FX_AddSprite( pos, temp, qfalse, radius * 5.3f/*16.0*/, 3.0f, 1.0f, 0.0f, 0.0f, 0.0f, 2700 + random() * 600, cgs.media.steamShader );
	}

	// Now place a cool explosion model on top
	VectorSubtract( cg.refdef.vieworg, start, dir );
	VectorNormalize( dir );

	//le = CG_MakeExplosion( start, dir, cgs.media.explosionModel, 6, cgs.media.electricalExplosionSlowShader, 500, qfalse, radius * 0.01f + ( crandom() * 0.3f)  );
	le = CG_MakeExplosion( start, dir, cgs.media.explosionModel, cgs.media.electricalExplosionSlowShader, 500, radius, qfalse );
	le->light = 150;
	//le->refEntity.		radius * 0.01f + ( crandom() * 0.3f)
	
	le->refEntity.renderfx |= RF_NOSHADOW;

	VectorSet( le->lightColor, 0.8f, 0.8f, 1.0f );
}

//RPG-X | GSIO01 | 09/05/2009:
void FX_PhaserFire2(vec3_t startpos, vec3_t endpos, vec3_t normal, qboolean impact, float scale)
{
	refEntity_t		beam;
	//float			size;
	//vec3_t			velocity;
	//int				sparks;
	vec3_t			rgb = { 1,0.9,0.6}, rgb2={1,0.3,0};

	// Draw beam first.
	memset( &beam, 0, sizeof( beam ) );
	VectorCopy( startpos, beam.origin);
	VectorCopy( endpos, beam.oldorigin );
	beam.reType = RT_LINE;
	beam.customShader = cgs.media.phaserShader;
	AxisClear( beam.axis );
	beam.shaderRGBA[0] = 0xff;
	beam.shaderRGBA[1] = 0xff;
	beam.shaderRGBA[2] = 0xff;
	beam.shaderRGBA[3] = 0xff;
	beam.data.line.width = scale + ( crandom() * 0.6f );
	beam.data.line.stscale = 5.0;
	trap_R_AddRefEntityToScene( &beam );
	if (impact)
	{
		FX_AddQuad2( endpos, normal, (0.75f * scale) + random() * .75 + 1.0f, 0.0f, 0.5f, 0.0f, rgb, rgb2, rand() % 360, 300 + random() * 200, 
			cgs.media.sunnyFlareShader );
	}
}

qboolean PhaserFX_Think(localEntity_t *le) {
	vec3_t	dir;
	qboolean impact = qfalse;
	le->data.spawner.nextthink = cg.time;
	VectorSubtract(le->data.spawner.dir, le->refEntity.origin, dir);
	VectorNormalize(dir);
	if(le->data.spawner.data2 == 1)
		impact = qtrue;

	FX_PhaserFire2(le->refEntity.origin, le->data.spawner.dir, dir, impact, le->data.spawner.data1);

	return qtrue;
}

void CG_PhaserFX(centity_t *cent) {
	localEntity_t *le;
	le = FX_AddSpawner(cent->currentState.origin, cent->currentState.origin2, NULL, NULL, qfalse, 0, 0, cent->currentState.time2, PhaserFX_Think, 10);
	le->data.spawner.data1 = cent->currentState.angles[0];
	le->data.spawner.data2 = cent->currentState.angles[2];
	le->data.spawner.nextthink = cg.time + (int)cent->currentState.angles[1];
}

qboolean TorpedoQFX_Think(localEntity_t *le)
{
	vec3_t	line1end, line2end, axis[3], rgb, vel, dis;
	float	dist;

	VectorSubtract(le->refEntity.origin, le->addOrigin, dis);
	dist = VectorLength(dis);
	if(dist < 0)
		dist *= -1;
	if(dist < 10)
		le->data.spawner.nextthink = le->endTime;

	AxisClear( axis ); 

	// convert direction of travel into axis
	if ( VectorNormalize2( le->data.spawner.dir, axis[0] ) == 0 ) {
		axis[0][2] = 1;
	}

	// spin as it moves
	RotateAroundDirection( axis, cg.time * 0.3f );// * 1.25f );

	VectorMA( le->refEntity.origin, -48.0f, axis[1], line1end ); // -24 is to high
	VectorMA( le->refEntity.origin,  48.0f, axis[1], line2end ); // 24 is to high
	FX_AddLine( line1end, line2end, 1.0f, random() * 18 + 2, 0.0f, 0.2 + random() * 0.2, 0.0f, 1, cgs.media.quantumGlow ); // replaced yellowParticleShader

	AxisClear( axis ); 

	// convert direction of travel into axis
	if ( VectorNormalize2( le->data.spawner.dir, axis[0] ) == 0 ) {
		axis[0][2] = 1;
	}

	// spin as it moves
	RotateAroundDirection( axis, -cg.time * 0.1f );// * 1.25f );

	VectorMA( le->refEntity.origin, 0.0f/*-128.0f*/, axis[2], line1end ); // -48 to high
	VectorMA( le->refEntity.origin,  128.0f, axis[2], line2end ); // 48 to high
	FX_AddLine( line1end, line2end, 1.0f, random() * 25 + 2, 0.0f, /*0.1 + random() * 0.2*/0.0f, 0.0f, 1, cgs.media.quantumGlow);
	//FX_AddSprite(line1end, NULL, qfalse, random() * 90 + 30, 4, 1.0f, 0.0f, 0, 0.0f, 1.0f, cgs.media.photonStar);

	VectorSet( rgb, 1.0f, 0.45f, 0.15f ); // orange

	FX_AddSprite( le->refEntity.origin, NULL,qfalse,random() * 60 + 30, 4, 0.5f, 0.0f, 0, 0.0f, 1.0f, cgs.media.quantumRays);	
	//FX_AddSprite2(le->refEntity.origin, NULL,qfalse,random() * 10 + 60, 0.0f, 0.1f, 0.1f, rgb, rgb, 0.0f, 0.0f, 1, cgs.media.whiteRingShader);
	FX_AddSprite( le->refEntity.origin, NULL,qfalse,random() * 40 + 8, 4, 0.5f, 0.0f, 0, 0.0f, 1.0f, cgs.media.quantumGlow );	

	VectorCopy(le->data.spawner.dir, vel);
	VectorNormalize(vel);
	VectorScale(vel, le->refEntity.oldorigin[0], vel);
	VectorAdd(le->refEntity.origin, vel, le->refEntity.origin);
	//le->data.spawner.nextthink = cg.time + 100;

	return qtrue;
}

qboolean TorpedoPFX_Think(localEntity_t *le)
{
	vec3_t	line1end, line2end, axis[3], rgb, vel, dis;
	float	dist;

	VectorSubtract(le->refEntity.origin, le->addOrigin, dis);
	dist = VectorLength(dis);
	if(dist < 0)
		dist *= -1;
	if(dist < 10)
		le->data.spawner.nextthink = le->endTime;

	AxisClear( axis ); 

	// convert direction of travel into axis
	if ( VectorNormalize2( le->data.spawner.dir, axis[0] ) == 0 ) {
		axis[0][2] = 1;
	}

	// spin as it moves
	RotateAroundDirection( axis, cg.time * 0.3f );// * 1.25f );

	VectorMA( le->refEntity.origin, -48.0f, axis[1], line1end ); // -24 is to high
	VectorMA( le->refEntity.origin,  48.0f, axis[1], line2end ); // 24 is to high
	FX_AddLine( line1end, line2end, 1.0f, random() * 18 + 2, 0.0f, 0.2 + random() * 0.2, 0.0f, 1, cgs.media.photonGlow ); // replaced yellowParticleShader

	AxisClear( axis ); 

	// convert direction of travel into axis
	if ( VectorNormalize2( le->data.spawner.dir, axis[0] ) == 0 ) {
		axis[0][2] = 1;
	}

	// spin as it moves
	RotateAroundDirection( axis, -cg.time * 0.1f );// * 1.25f );

	VectorMA( le->refEntity.origin, 0.0f/*-128.0f*/, axis[2], line1end ); // -48 to high
	VectorMA( le->refEntity.origin,  128.0f, axis[2], line2end ); // 48 to high
	FX_AddLine( line1end, line2end, 1.0f, random() * 25 + 2, 0.0f, /*0.1 + random() * 0.2*/0.0f, 0.0f, 1, cgs.media.photonGlow);
	//FX_AddSprite(line1end, NULL, qfalse, random() * 90 + 30, 4, 1.0f, 0.0f, 0, 0.0f, 1.0f, cgs.media.photonStar);

	VectorSet( rgb, 1.0f, 0.45f, 0.15f ); // orange

	FX_AddSprite( le->refEntity.origin, NULL,qfalse,random() * 60 + 30, 4, 0.5f, 0.0f, 0, 0.0f, 1.0f, cgs.media.photonRay);	
	//FX_AddSprite2(le->refEntity.origin, NULL,qfalse,random() * 10 + 60, 0.0f, 0.1f, 0.1f, rgb, rgb, 0.0f, 0.0f, 1, cgs.media.whiteRingShader);
	FX_AddSprite( le->refEntity.origin, NULL,qfalse,random() * 40 + 8, 4, 0.5f, 0.0f, 0, 0.0f, 1.0f, cgs.media.photonGlow );	

	VectorCopy(le->data.spawner.dir, vel);
	VectorNormalize(vel);
	VectorScale(vel, le->refEntity.oldorigin[0], vel);
	VectorAdd(le->refEntity.origin, vel, le->refEntity.origin);
	//le->data.spawner.nextthink = cg.time + 100;
	
	return qtrue;
}

void CG_TorpedoFX(centity_t *cent) {
	localEntity_t	*le;
	if(cent->currentState.eventParm & 1) { // quantum fx
		le = FX_AddSpawner(cent->currentState.origin, cent->currentState.angles, NULL, NULL, qfalse, 0, 0, 10000, TorpedoQFX_Think, 10);
	} else { // photon fx
		le = FX_AddSpawner(cent->currentState.origin, cent->currentState.angles, NULL, NULL, qfalse, 0, 0, 10000, TorpedoPFX_Think, 10);
	}
	le->refEntity.oldorigin[0] = cent->currentState.angles2[0];
	VectorCopy(cent->currentState.origin2, le->addOrigin);
}

qboolean ParitcleFire_CreateParticles(localEntity_t *le) {

	return qtrue;
}

qboolean ParticleFire_Think(localEntity_t *le) {
	vec3_t	velocity;
	vec3_t	origin;
	vec3_t	dir = { 0, 0 , 15 };
	float	speed;
	int i;
	vec3_t	startRGB	= { 1, 0.2, 0 };
	vec3_t	endRGB		= {	1, 0.9, 0.7 };

	dir[2] = Com_Clamp( 0.85f, 1.0f, dir[2] );

	for ( i = 0; i < 3; i++ )
	{
		velocity[i] = dir[i] + ( 0.2f * crandom());
	}

	VectorMA( le->refEntity.origin, 1, le->data.spawner.dir, origin);

	speed = le->data.spawner.data1 * 2.4;

	VectorScale( velocity, speed, velocity ); //speed

	FX_AddSprite2(	origin,
					velocity, 
					qfalse, //accel
					le->data.spawner.data1 + (flrandom(0.4,2) * le->data.spawner.data1 ),  
					le->data.spawner.data1 + (crandom() * le->data.spawner.data1 * 0.5f), 
					0.8, 
					0.0,
					startRGB,
					endRGB,
					0.1f,//16.0f + random() * 45.0f,
					0.1f,
					3500, 
					cgs.media.fireParticle ); //flags

	le->data.spawner.nextthink = cg.time + 750; // don't generate to many or we will get low fps

	return qtrue;
}

void CG_ParticleFire(vec3_t origin, int killtime, int size) {
	localEntity_t *le;
	le = FX_AddSpawner(origin, NULL, NULL, NULL, qfalse, 0, 0, killtime, ParticleFire_Think, 10);
	//le->data.spawner.data1 = size;
}

qboolean ShowTrigger_Think(localEntity_t *le) {
	vec4_t RGBA = { 0, 1, 0, 0.75 };
	vec3_t a, b, c, d;

	VectorCopy(le->refEntity.lightingOrigin, a);
	c[0] = b[0] = le->lightColor[0];
	c[2] = d[2] = le->lightColor[2];
	b[1] = c[1] = d[1] = a[1];
	d[0] = a[0];
	b[2] = a[2];

	FX_AddLine2(a, b, 0.5, 0.5, 0, 0.5, 0, 1, 1, RGBA, RGBA, 10000, cgs.media.whiteShader);
	FX_AddLine2(b, c, 0.5, 0.5, 0, 0.5, 0, 1, 1, RGBA, RGBA, 10000, cgs.media.whiteShader);
	FX_AddLine2(c, d, 0.5, 0.5, 0, 0.5, 0, 1, 1, RGBA, RGBA, 10000, cgs.media.whiteShader);
	FX_AddLine2(d, a, 0.5, 0.5, 0, 0.5, 0, 1, 1, RGBA, RGBA, 10000, cgs.media.whiteShader);

	return qtrue;
}

qboolean ShowTrigger_Think2(localEntity_t *le) {
	vec4_t RGBA = { 0, 1, 0, 0.75 };
	vec3_t e, f, g, h;

	VectorCopy(le->lightColor, g);
	h[0] = f[0] = le->refEntity.lightingOrigin[0];
	f[2] = e[2] = le->refEntity.lightingOrigin[2];
	e[1] = f[1] = h[1] = g[1];
	e[0] = g[0];
	h[2] = g[2];

	FX_AddLine2(f, e, 0.5, 0.5, 0, 0.5, 0, 1, 1, RGBA, RGBA, 10000, cgs.media.whiteShader);
	FX_AddLine2(e, g, 0.5, 0.5, 0, 0.5, 0, 1, 1, RGBA, RGBA, 10000, cgs.media.whiteShader);
	FX_AddLine2(g, h, 0.5, 0.5, 0, 0.5, 0, 1, 1, RGBA, RGBA, 10000, cgs.media.whiteShader);
	FX_AddLine2(h, f, 0.5, 0.5, 0, 0.5, 0, 1, 1, RGBA, RGBA, 10000, cgs.media.whiteShader);

	return qtrue;
}

qboolean ShowTrigger_Think3(localEntity_t *le) {
	vec4_t RGBA = { 0, 1, 0, 0.75 };
	vec3_t a, b, c, d, e, f, g, h;

	VectorCopy(le->refEntity.lightingOrigin, a);
	VectorCopy(le->lightColor, g);
	d[0] = f[0] = h[0] = a[0];
	b[0] = c[0] = e[0] = g[0];
	e[1] = f[1] = h[1] = g[1];
	b[1] = c[1] = d[1] = a[1];
	b[2] = f[2] = e[2] = a[2];
	h[2] = d[2] = c[2] = g[2];
 
	FX_AddLine2(a, f, 0.5, 0.5, 0, 0.5, 0, 1, 1, RGBA, RGBA, 10000, cgs.media.whiteShader);
	FX_AddLine2(d, h, 0.5, 0.5, 0, 0.5, 0, 1, 1, RGBA, RGBA, 10000, cgs.media.whiteShader);
	FX_AddLine2(b, e, 0.5, 0.5, 0, 0.5, 0, 1, 1, RGBA, RGBA, 10000, cgs.media.whiteShader);
	FX_AddLine2(c, g, 0.5, 0.5, 0, 0.5, 0, 1, 1, RGBA, RGBA, 10000, cgs.media.whiteShader);

	return qtrue;
}

void CG_ShowTrigger(centity_t *cent) {
	localEntity_t *le;
	if(cent->currentState.eventParm) {
		le = FX_AddSpawner(cent->currentState.origin, NULL, NULL, NULL, qfalse, 0, 0, 10000, ShowTrigger_Think, 0);
		VectorCopy(cent->currentState.apos.trBase, le->lightColor);
		VectorCopy(cent->currentState.pos.trBase, le->refEntity.lightingOrigin);
		le = FX_AddSpawner(cent->currentState.origin, NULL, NULL, NULL, qfalse, 0, 0, 10000, ShowTrigger_Think2, 0);
		VectorCopy(cent->currentState.apos.trBase, le->lightColor);
		VectorCopy(cent->currentState.pos.trBase, le->refEntity.lightingOrigin);
		le = FX_AddSpawner(cent->currentState.origin, NULL, NULL, NULL, qfalse, 0, 0, 10000, ShowTrigger_Think3, 0);
		VectorCopy(cent->currentState.apos.trBase, le->lightColor);
		VectorCopy(cent->currentState.pos.trBase, le->refEntity.lightingOrigin);
	} else {
		le = FX_AddSpawner(cent->currentState.origin, NULL, NULL, NULL, qfalse, 0, 0, 10000, ShowTrigger_Think, 0);
		VectorCopy(cent->currentState.origin2, le->lightColor);
		VectorCopy(cent->currentState.angles2, le->refEntity.lightingOrigin);
		le = FX_AddSpawner(cent->currentState.origin, NULL, NULL, NULL, qfalse, 0, 0, 10000, ShowTrigger_Think2, 0);
		VectorCopy(cent->currentState.origin2, le->lightColor);
		VectorCopy(cent->currentState.angles2, le->refEntity.lightingOrigin);
		le = FX_AddSpawner(cent->currentState.origin, NULL, NULL, NULL, qfalse, 0, 0, 10000, ShowTrigger_Think3, 0);
		VectorCopy(cent->currentState.origin2, le->lightColor);
		VectorCopy(cent->currentState.angles2, le->refEntity.lightingOrigin);
	}
}

//RPG-X | Harry Young | 03.12.2011
void FX_DisruptorFire2(vec3_t startpos, vec3_t endpos, vec3_t normal, qboolean impact, float scale)
{
	refEntity_t		beam;
	//float			size;
	//vec3_t			velocity;
	//int				sparks;
	vec3_t			rgb = { 1,0.9,0.6}, rgb2={1,0.3,0};

	// Draw beam first.
	memset( &beam, 0, sizeof( beam ) );
	VectorCopy( startpos, beam.origin);
	VectorCopy( endpos, beam.oldorigin );
	beam.reType = RT_LINE;
	beam.customShader = cgs.media.disruptorBeam;
	AxisClear( beam.axis );
	beam.shaderRGBA[0] = 0xff;
	beam.shaderRGBA[1] = 0xff;
	beam.shaderRGBA[2] = 0xff;
	beam.shaderRGBA[3] = 0xff;
	beam.data.line.width = scale + ( crandom() * 0.6f );
	beam.data.line.stscale = 5.0;
	trap_R_AddRefEntityToScene( &beam );
	if (impact)
	{
		FX_AddQuad2( endpos, normal, (0.75f * scale) + random() * .75 + 1.0f, 0.0f, 0.5f, 0.0f, rgb, rgb2, rand() % 360, 300 + random() * 200, 
			cgs.media.disruptorStreak );
	}
}

qboolean DisruptorFX_Think(localEntity_t *le) {
	vec3_t	dir;
	qboolean impact = qfalse;
	le->data.spawner.nextthink = cg.time;
	VectorSubtract(le->data.spawner.dir, le->refEntity.origin, dir);
	VectorNormalize(dir);
	if(le->data.spawner.data2 == 1)
		impact = qtrue;

	FX_DisruptorFire2(le->refEntity.origin, le->data.spawner.dir, dir, impact, le->data.spawner.data1);

	return qtrue;
}

void CG_DisruptorFX(centity_t *cent) {
	localEntity_t *le;
	le = FX_AddSpawner(cent->currentState.origin, cent->currentState.origin2, NULL, NULL, qfalse, 0, 0, cent->currentState.time2, DisruptorFX_Think, 10);
	le->data.spawner.data1 = cent->currentState.angles[0];
	le->data.spawner.data2 = cent->currentState.angles[2];
	le->data.spawner.nextthink = cg.time + (int)cent->currentState.angles[1];
}

// Additional ports from SP by Harry Young

/*
===========================
Laser

Create directed laser shot
===========================
*/
void CG_SmallSpark( vec3_t origin, vec3_t normal )
{
	vec3_t	dir, direction, start, end, velocity;
	float	scale;
	int		numSparks;

	AngleVectors( normal, normal, NULL, NULL );
	
	int j;
	for ( j = 0; j < 3; j ++ )
		normal[j] = normal[j] + (0.1f * crandom());

	VectorNormalize( normal );

	numSparks = 6 + (random() * 4.0f );
	
	int i;
	for ( i = 0; i < numSparks; i++ )
	{	
		scale = 0.1f + (random() *0.2f );

		for ( j = 0; j < 3; j ++ )
			dir[j] = normal[j] + (0.7f * crandom());

		VectorMA( origin, 0.0f + ( random() * 0.5f ), dir, start );
		VectorMA( start, 1.0f + ( random() * 1.5f ), dir, end );

		FX_AddLine( start,
					end,
					1.0f,
					scale,
					0.0f,
					1.0f,
					0.7f,
					4.0f,
					cgs.media.sparkShader );
	}

	VectorMA( origin, 1, normal, direction );

	scale = 2.0f + (random() * 3.0f );
	float alpha = 0.6f + (random() * 0.4f );

	VectorSet( velocity, crandom() * 2, crandom() * 2, 8 + random() * 4 );
	VectorMA( velocity, 5, normal, velocity );

	FX_AddSprite(	direction, 
					velocity, 
					qfalse, 
					scale,
					scale,
					alpha,
					0.0f,
					random() * 45.0f,
					0.0f,
					1000.0f,
					cgs.media.steamShader );
}


void CG_FireLaser( vec3_t start, vec3_t end, vec3_t normal, vec3_t laserRGB, float alpha )
{
	vec3_t	dir, right, up, angles, work, pos,
			sRGB;
	float	scale = 1.0f;
	int		life = 0;

	// Orient the laser spray
	VectorSubtract( end, start, dir );
	VectorNormalize( dir );

	VectorMA( end, 0.5f, normal, pos );
	MakeNormalVectors( normal, right, up );
	
	VectorSet( sRGB, 1.0f, 0.8f, 0.8f );	

	FX_AddSprite2( start, NULL, qfalse, 
					1.75f, 1.0f, 
					alpha, 0.0f, 
					laserRGB, laserRGB, 
					0.0f, 
					0.0f, 
					200, 
					cgs.media.waterDropShader );

	FX_AddLine3( start, end, 
					1.0f, 
					3.0f, 5.0f, 
					alpha, 0.0f, 
					laserRGB, laserRGB, 
					125, 
					cgs.media.whiteLaserShader ); 

	FX_AddLine( start, end, 
					1.0f, 
					0.3f, 5.0f, 
					random() * 0.4 + 0.4, 0.1f,
					125,
					cgs.media.whiteLaserShader );

	// Doing all of this extra stuff would look weird if it hits a player ent.
	//if ( !hit_ent )
	//{
		FX_AddQuad2( pos, normal, 
					3.5f, 1.0f, 
					alpha, 0.0f, 
					laserRGB, laserRGB, 
					0.0f, 
					200, 
					cgs.media.waterDropShader );
		int t;
		for ( t=0; t < 2; t ++ )
		{
			VectorMA( pos, crandom() * 0.5f, right, work );
			VectorMA( work, crandom() * 0.5f, up, work );

			scale = crandom() * 0.5f + 1.75f;
			life = crandom() * 300 + 2100;

			VectorSet( sRGB, 1.0f, 0.7f, 0.2f );
			FX_AddQuad2( work, normal,
					scale, -0.1f, 
					1.0f, 0.0f, 
					sRGB, sRGB, 
					0,
					life, 
					cgs.media.waterDropShader );
		}

		FX_AddQuad( pos, normal, 
					scale * 2.5f, 0.0f, 
					1.0f, 0.0f, 
					0, 
					life * 2, 
					cgs.media.smokeShader );
	
		vectoangles( normal, angles );
		CG_SmallSpark( end, angles );
	/*}
	else
	{
		// However, do add a little smoke puff
		FX_AddSprite2( pos, NULL, qfalse, 
						2.0f, 1.0f, 
						alpha, 0.0f, 
						laserRGB, laserRGB, 
						0.0f, 
						0.0f, 
						200, 
						cgs.media.waterDropShader );

		VectorMA( end, 1, normal, dir );
		scale = 1.0f + (random() * 3.0f);

		CG_Smoke( dir, normal, scale, 12.0f );
	}*/
}

//------------------------------------------------------------------------------
void CG_AimLaser( vec3_t start, vec3_t end, vec3_t normal )
{
	vec3_t		lRGB = {1.0,0.0,0.0};

	// Beam
	FX_AddLine3( start, end, 
					1.0f, 
					5.5f, 5.0f, 
					random() * 0.2 + 0.2, 0.1f,
					lRGB, lRGB,
					150,
					cgs.media.whiteLaserShader );

	FX_AddLine( start, end, 
					1.0f, 
					0.3f, 5.0f, 
					random() * 0.4 + 0.4, 0.1f,
					125,
					cgs.media.whiteLaserShader );

	// Flare at the start point
	FX_AddSprite( start, NULL, qfalse, 
					1.5 + random() * 4, 0.0,
					0.1f,0.0, 
					0.0,
					0.0,
					100,
					cgs.media.borgEyeFlareShader );

	// endpoint flare
	FX_AddSprite( end, NULL, qfalse, 
					2.5 + random() * 4, 0.0,
					0.1f,0.0, 
					0.0,
					0.0,
					100,
					cgs.media.borgEyeFlareShader );

	// oriented impact flare
	FX_AddQuad( end, normal, 
					1.5 + random() * 2, 0.0, 
					1.0, 0.0, 
					0.0, 
					120, 
					cgs.media.borgEyeFlareShader );
}

/*
======================
CG_CookingSteam

Creates a basic cooking steam effect
======================
*/
void CG_CookingSteam( vec3_t origin, float radius )
{
	vec3_t dir;

	VectorSet( dir, crandom()*2, crandom()*2, crandom() + radius); // always move mostly up
	VectorScale( dir, random() * 5 + 2, dir );

	FX_AddSprite( origin, dir, qfalse, radius, radius * 2, 0.4F, 0.0, 0, 0, 1000, cgs.media.steamShader );
}

/*
======================
CG_ElectricFire

Creates an electric fire effect
======================
*/

void CG_ElectricFire( vec3_t origin, vec3_t normal )
{
	void	*particle;
	vec3_t	dir, direction, start, end;
	vec3_t	velocity;
	float	scale, alpha;
	int		numSparks, i, j;

	AngleVectors( normal, normal, NULL, NULL);

	numSparks = 4 + (random() * 8.0f);
	
	for ( i = 0; i < numSparks; i++ )
	{	
		scale = 0.3f + (random() *0.4);

		for ( j = 0; j < 3; j ++ )
			dir[j] = normal[j] + (0.4f * crandom());
		
		VectorNormalize(dir);

		VectorMA( origin, -1.0f + ( random() * 2.0f ), dir, start );
		VectorMA( start, 2.0f + ( random() * 12.0f ), dir, end );

		FX_AddLine( start,
					end,
					1.0f,
					scale,
					0.0f,
					1.0f,
					0.0f,
					75.0f,
					cgs.media.sparkShader );
	}
 
	scale = 0.5f + (random() * 0.5f);

	VectorScale( normal, 300, velocity );

	particle = FX_AddTrail( start,
							velocity,
							qtrue,
							6.0f,
							-24.0f,
							scale,
							-scale,
							1.0f,
							0.5f,
							0.0f,
							200.0f,
							cgs.media.sparkShader);

	if ( particle == NULL )
		return;

	VectorMA( origin, 1, normal, direction );
	VectorSet( velocity, 0, 0, 8 );

	FXE_Spray( dir, 200, 200, 0.2f, velocity);

	VectorMA( origin, 1, normal, direction );
	VectorSet( velocity, 0, 0, 8 );

	for ( i = 0; i < 3; i++)
	{
		scale = 6.0f + (random() * 8.0f);
		alpha = 0.1 + (random() * 0.4f);

		FX_AddSprite( direction, 
					velocity, 
					qfalse, 
					scale,
					scale,
					alpha,
					0.0,
					random()*45.0f,
					0.0f,
					1000.0f,
					cgs.media.steamShader );

		VectorMA( velocity, 9.0, normal, velocity);
	}
}

/*
======================
CG_ForgeBolt

Creates an orange electricity bolt effect with a pulse that travels down the beam
======================
*/

/*void ForgeBoltFireback( vec3_t start, vec3_t end, vec3_t velocity, vec3_t user )
{
	FX_AddElectricity( start, end, 1.0, user[DATA_RADIUS], 5.0, 1.0, 0.0, 200, cgs.media.pjBoltShader, 
						(int)user[DATA_EFFECTS], user[DATA_CHAOS] );
}

//---------------------------------------------------
bool ForgeBoltPulse( FXPrimitive *fx, centity_t *ent )
{
	vec3_t			origin, new_org;
	trace_t			trace;
	qboolean		remove = qfalse;

	VectorCopy( fx->m_origin, origin );
	fx->UpdateOrigin();
	VectorCopy( fx->m_origin, new_org );

	CG_Trace( &trace, origin, NULL, NULL, new_org, -1, CONTENTS_SOLID );

	if ( trace.fraction < 1.0f && !trace.startsolid && !trace.allsolid )
	{
		// The effect hit something, presumably a barrier, so kill it
		remove = qtrue;
		return false;
	}

	vec3_t		normal, rgb1 ={ 1.0F, 0.5F, 0.4F}, rgb2 ={ 1.0F, 1.0F, 0.3F};//, org;
	FXCylinder	*fxc;

	// Convert the direction of travel in to a normal;
	VectorCopy( fx->m_velocity, normal );
	VectorNormalize( normal );
	VectorScale( normal, -1, normal );

	fxc = FX_AddCylinder( new_org, normal, 16, 0, 16 - random() * 8, 0, 32 + random() * 24, 0, 0.2F, 0.2F, rgb1, rgb1, 1, cgs.media.psychicShader, 0.6F );

	if ( fxc == NULL )
		return false;

	fxc->SetFlags( FXF_WRAP );
	fxc->SetSTScale( Q_irand(1,3) );

	fxc = FX_AddCylinder( new_org, normal, 8, 0, 12 - random() * 8, 0, 24 + random() * 24, 0, 0.2F, 0.2F, rgb2, rgb2, 1, cgs.media.psychicShader, 0.6F );

	if ( fxc == NULL )
		return false;

	fxc->SetFlags( FXF_WRAP );
	fxc->SetSTScale( Q_irand(1,2) );

	return true;
}

//-----------------------------
/*void CG_ForgeBolt( centity_t *cent )
{
	qboolean	pulse;
	int			effects;
	float		chaos, radius;
	
	// Set up all of the parms
	pulse = (cent->gent->spawnflags & 8) ? qtrue : qfalse;
	effects = (cent->gent->spawnflags & 16) ? FXF_TAPER : 0;
	effects = (cent->gent->spawnflags & 32) ? (FXF_WRAP | effects) : effects;
	chaos = cent->gent->random;
	radius = cent->gent->radius;

	// Delayed bolt that should "work" a while
	if ( cent->gent->spawnflags & 2 )
	{
		vec3_t data;

		// This sucks, but the spawn function needs some extra bits of info
		data[DATA_EFFECTS] = effects;
		data[DATA_CHAOS] = chaos;
		data[DATA_RADIUS] = radius;

		FX_AddSpawner( cent->lerpOrigin, cent->currentState.origin2, NULL, data, 70, random() * 25, 450, (void *) ForgeBoltFireback );
	}
	else
	{
		FX_AddElectricity( cent->lerpOrigin, cent->currentState.origin2, 1.0, radius, 5.0, 1.0, 0.0, 200, cgs.media.pjBoltShader, 
						effects, chaos );

		if ( rand() & 1 )
			FX_AddElectricity( cent->currentState.origin2, cent->lerpOrigin, 1.0, radius * 2, 5.0, 1.0, 0.0, 200, cgs.media.pjBoltShader, 
						effects, chaos );

		if ( cg.time > cent->gent->delay && pulse )
		{
			vec3_t	dir;
			float	amt;

			VectorSubtract( cent->currentState.origin2, cent->lerpOrigin, dir );
			VectorNormalize( dir );
			amt = 200 + random() * 100;
			VectorScale( dir, amt, dir );

			FX_AddParticle( cent, cent->lerpOrigin, dir, NULL, 16, 0.0, 1.0, 1.0,
						0.0, 0.0, 6000, cgs.media.ltblueParticleShader, FXF_NODRAW, ForgeBoltPulse );

			cent->gent->delay = cg.time + 500;
		}
	}

	// Bolt that generates sparks at the impact point
	if ( cent->gent->spawnflags & 4 )
	{
		vec3_t	dir;

		VectorSubtract( cent->lerpOrigin, cent->currentState.origin2, dir );
		VectorNormalize( dir );

		BoltSparkSpew( cent->currentState.origin2, dir, cgs.media.dkorangeParticleShader );
	}
}

/*
===========================
Plasma

Create directed and scaled plasma jet
===========================
*/

void CG_Plasma( vec3_t start, vec3_t end, vec3_t sRGB, vec3_t eRGB, int startalpha, int endalpha )
{
	vec3_t	v, sp;
	float	detail, len, salpha = (startalpha / 255) , ealpha = (endalpha / 255);

	//detail = FX_DetailLevel( start, 16, 1200 );
	//if ( detail == 0 )
	//	return;

	// Orient the plasma
	VectorSubtract( end, start, v );
	len = VectorNormalize( v );
	VectorMA( start, 0.5f, v, sp );

	// Stash a quad at the base to make the effect look a bit more solid
	//FX_AddQuad( sp, v, NULL, NULL, len * 0.36f, 0.0f, salpha, salpha, sRGB, sRGB, 0.0f, 45.0f, 0.0f, 200, cgs.media.prifleImpactShader );
	
	// Add a subtle, random flutter to the cone direction
	v[0] += crandom() * 0.04;
	v[1] += crandom() * 0.04;
	v[2] += crandom() * 0.04;
	
	// Wanted the effect to be scalable based on the length of the jet. 
	FX_AddCylinder2( start, v, len * 0.05, len * 2.0f, len * 0.16f, len * 0.32f, len * 0.40f, len * 0.64f, 
					salpha, ealpha, sRGB, eRGB, 200, cgs.media.plasmaShader, 0.3f );
	FX_AddCylinder2( start, v, len * 0.05, len * 4.0f, len * 0.16f, len * 0.32f, len * 0.28f, len * 0.64f, 
					salpha, ealpha, sRGB, eRGB, 200, cgs.media.plasmaShader, 0.2f );
	FX_AddCylinder2( start, v, len * 0.25, len * 8.0f, len * 0.20f, len * 0.32f, len * 0.02f, len * 0.32f, 
					salpha, ealpha, sRGB, eRGB, 200, cgs.media.plasmaShader, 0.1f );
}

/*
======================
CG_ParticleStream

particle stream fx for STASIS level
======================
*/

/*bool particle_stream_think( FXPrimitive *fx, centity_t *ent )
{
	vec3_t old_org;

	// Make it flicker. . .always safe to do this
	fx->m_scale = random() * 12 + 2;
	fx->m_alpha = random() * 0.4 + 0.6;

	// If the ent was somehow removed, we don't want to continue any further.
	if ( !ent )
		return false;

	// Stash the old position so that we can draw a trailer line
	VectorCopy( fx->m_origin, old_org );

	// Update the position of the particle.
	fx->m_origin[0]  = cos(cg.time * 0.01 + fx->m_velocity[0]) * fx->m_velocity[1] + ent->lerpOrigin[0];
	fx->m_origin[1]  = sin(cg.time * 0.01 + fx->m_velocity[0]) * fx->m_velocity[1] + ent->lerpOrigin[1];
	fx->m_origin[2] += (fx->m_velocity[2] * cg.frametime * 0.001);

	FX_AddLine( fx->m_origin, old_org, 1.0f, 2.0f, -4.0f, 0.6f, 0.0, 500, cgs.media.IMOD2Shader );

	return true;
}

//------------------------------------------------------------------------------
void CG_ParticleStream( centity_t *cent )
{
	vec3_t	vel, org, dir;
	float	len, time;

	// This effect will currently only travel directly up or down--never sideways
	VectorSubtract( cent->currentState.origin2, cent->lerpOrigin, dir );
	len = VectorNormalize( dir );

	// since the movement direction is limited, use the velocity var a bit more efficiently
	vel[0] = random() * 360;						// random position around the cylinder
	vel[1] = random() > 0.9  ? 20 : 6;				// random radius
	vel[2] = dir[2] * 120 + dir[2] * random() * 50;	// random velocity (up or down)

	// Set the particle position
	org[0] = cos(cg.time * 0.01 + vel[0]) * vel[1] + cent->lerpOrigin[0];
	org[1] = sin(cg.time * 0.01 + vel[0]) * vel[1] + cent->lerpOrigin[1];
	org[2] = cent->lerpOrigin[2];

	// Calculate how long the thing should live based on it's velocity and the distance it has to travel
	time = len / vel[2] * 1000;

	// Use a couple of different kinds to break up the monotony
	if ( rand() & 1 )
	{
		FX_AddParticle( cent, org, vel, NULL, 16, 0.0, 1.0, 1.0,
						0.0, 0.0, time, cgs.media.ltblueParticleShader, 0, particle_stream_think );
	}
	else
	{
		FX_AddParticle( cent, org, vel, NULL, 16, 0.0, 1.0, 1.0,
						0.0, 0.0, time, cgs.media.purpleParticleShader, 0, particle_stream_think );
	}
}

/*
======================
CG_TransporterStream

particle stream fx for forge level
The particles will accelerate up to the half-way point of the cylinder, then deccelerate til they hit their target
======================
*/

/*void CG_TransporterStream( centity_t *cent )
{
	vec3_t	vel, accel, dir, pos, right, up;
	float	len, time, acceleration, scale, dis, vf;
	int		t;
	VectorSubtract( cent->currentState.origin2, cent->lerpOrigin, dir );
	len = VectorNormalize( dir );
	MakeNormalVectors( dir, right, up );

	for ( t=0; t < 3; t++ )
	{
		// Create start offset within a circular radius
		VectorMA( cent->lerpOrigin, 8 * crandom(), right, pos );
		VectorMA( pos, 8 * crandom(), up, pos );

		acceleration = 80 + random() * 50;
		VectorScale( dir, acceleration, accel ); // acceleration vector
		VectorScale( dir, 0.0001, vel );		// Ideally, vel would be zero, so just make it really small

		dis = ( len * 0.8f );					// the two segs will be overlapping to cover up the middle

		// This is derived from dis = (vi)(t)  +  (1/2)(a)(t)^2 where the inital velocity (vi) = zero
		time = sqrt( 2  / acceleration * dis );	// Calculate how long the thing will take to travel that distance
		
		scale = 1.5f + random() * 4;

		// These will spawn at the base and accelerate towards the middle
		if ( rand() & 1 )
		{
			FX_AddSprite3( pos, vel, accel, 
						scale, 0.0f, 
						1.0f, 0.0f, 
						0.0f, 
						0.0f, 
						time * 1000, 
						cgs.media.orangeParticleShader );
		}
		else
		{
			FX_AddSprite3( pos, vel, accel, 
						scale, 0.0f, 
						1.0f, 0.0f, 
						0.0f, 
						0.0f, 
						time * 1000, 
						cgs.media.dkorangeParticleShader );
		}

		// These will be spawned somewhere in the middle and deccelerate till they reach the end of their target
		VectorMA( pos, len - dis, dir, pos );
		VectorScale( accel, -1, accel );

		vf = sqrt( 2 * dis * acceleration ); // calculate the how fast it would be moving at the end of its path
		VectorScale( dir, vf, vel );		//	this will be the _initial_ velocity for those starting in the middle

		if ( rand() & 1 )
		{
			FX_AddSprite3( pos, vel, accel, 
						scale, 0.0f, 
						0.0f, 1.0f, 
						0.0f, 
						0.0f, 
						time * 1000, 
						cgs.media.orangeParticleShader );
		}
		else
		{
			FX_AddSprite3( pos, vel, accel, 
						scale, 0.0f, 
						0.0f, 1.0f, 
						0.0f, 
						0.0f, 
						time * 1000, 
						cgs.media.dkorangeParticleShader );
		}
	}
}

/*
-------------------------
CG_ExplosionTrail
-------------------------
*/

/*qboolean explosionTrailThink( localEntity_t	*fx )
{
	localEntity_t	*le=0;
	vec3_t			direction, origin, new_org, angles, dir;
	trace_t			trace;
	float			scale;
	int				i;
	qboolean		remove = qfalse;

	VectorCopy( fx->m_origin, origin );
	fx->UpdateOrigin();
	VectorCopy( fx->m_origin, new_org );

	CG_Trace( &trace, origin, NULL, NULL, new_org, -1, CONTENTS_SOLID );

	if ( trace.fraction < 1.0f && !trace.startsolid && !trace.allsolid )
	{
		// The effect hit something, presumably a barrier, so kill it
		// When the effect gets killed like this, it dies quickly and looks a bit thin.
		// Maybe something else should be done as well...
		remove = qtrue;
		//FIXME: FX_RemoveEffect( fx );
		return qfalse;
	}

	scale = 80 * 0.03f;

	VectorSubtract( new_org, origin, dir );
	VectorNormalize( dir );
	vectoangles( dir, angles );
		
	//Orient the explosions to face the camera
	VectorSubtract( cg.refdef.vieworg, origin, direction );
	VectorNormalize( direction );

	for ( i = 0; i < 3 + (int)remove * 6; i++)
	{
		angles[2] = crandom() * 360;

		AngleVectors( angles, NULL, dir, NULL );
		VectorMA( origin, random() * 50.0f, dir, new_org );

		le = CG_MakeExplosion( new_org, direction, cgs.media.explosionModel, 6, cgs.media.surfaceExplosionShader, 400 + (int)remove * 800, qfalse, random() * 1.0 + 0.8 );//random() * 1.0 + 1.0 );
	}

	le->light = 150;
	VectorSet( le->lightColor, 64, 192, 255 );

	//Shake the camera and damage everything in an area
	CG_ExplosionEffects( origin, 3.0f, 600 );
	G_RadiusDamage( origin, ent->gent, 150, 80, NULL, MOD_UNKNOWN );

	return qtrue;
}

//------------------------------------------------------------------------------
void CG_ExplosionTrail( centity_t *cent )
{
	vec3_t			dir;
	float			len;

	VectorSubtract( cent->currentState.origin2, cent->currentState.origin, dir );
	len = VectorNormalize( dir );
	VectorScale( dir, 325, dir );

	FX_AddParticle( cent->currentState.origin, dir, qfalse, 16, 0.0, 1.0, 1.0,
						0.0, 0.0, 6000, cgs.media.ltblueParticleShader, explosionTrailThink );
}

/*
----------------------
CG_BorgEnergyBeam

A scanning type beam
----------------------
*/

/*void CG_BorgEnergyBeam( centity_t *cent )
{
	vec3_t		normal, angles, base, dir, dir2, rgb;
	float		len, alpha;

	VectorSubtract( cent->currentState.origin2, cent->lerpOrigin, normal );
	len = VectorNormalize( normal );
	vectoangles( normal, angles );
	alpha = Vector4to3( cent->gent->startRGBA, rgb );

	// Code to make the thing "snap" when it's doing the beam slices
	if ( abs( cent->gent->pos2[0] ) >= cent->gent->radius )
	{
		// Snap back to start and move to the next slice
		cent->gent->pos2[0] = cent->gent->radius;
		cent->gent->pos2[1] -= ( cg.frametime * 0.0003 * cent->gent->speed );
	}

	// The slice has moved past the end so snap back to the first slice position
	if ( abs( cent->gent->pos2[1] ) >= cent->gent->radius )
	{
		cent->gent->pos2[1] = cent->gent->radius;
	}

	// Always move across the slice
	cent->gent->pos2[0] -= ( cg.frametime * 0.001 * cent->gent->speed );


	if ( cent->gent->spawnflags & 2 )
	{
		// Trace a cone
		angles[2] = cent->gent->angle;
	}

	AngleVectors( angles, NULL, dir, dir2 );

	if ( cent->gent->spawnflags & 2 )
	{
		// Cone
		VectorMA( cent->currentState.origin2, cent->gent->radius, dir, base );
	}
	else
	{
		// Swinging pendulum
		VectorMA( cent->currentState.origin2, cent->gent->radius * ( sin( cent->gent->angle * 0.03f )), dir, base );
		VectorMA( base, cent->gent->radius * ( cos( cent->gent->angle * 0.003f )), dir2, base );
		// Do "snapping" beam slices
//		VectorMA( cent->currentState.origin2, cent->gent->pos2[0], dir, base );
//		VectorMA( base, cent->gent->pos2[1], dir2, base );
	}

	// Main trace laser
	FX_AddLine( cent->lerpOrigin, base, 64, 0.8f, 5.0f, alpha, 0.0, rgb, rgb, 120, cgs.media.whiteLaserShader );
	// Faint trail at base.  Is this really adding anything useful?
	FX_AddLine( cent->gent->pos1, base, 1, 1.0, 2.0, alpha * 0.2, 0.0, rgb, rgb, 1000, cgs.media.whiteLaserShader );
	// Faint trace cone, adds a bit of meat to the effect
	FX_AddTri( cent->lerpOrigin, cent->gent->pos1, base, alpha * 0.2, 0.0, rgb, rgb, 800, cgs.media.solidWhiteShader );
	// Laser impact point
	FX_AddSprite( base, NULL, NULL, random() * 2, 0.0, alpha, 0.0, rgb, rgb, 0.0, 0.0, 100, cgs.media.waterDropShader );

	VectorCopy( base, cent->gent->pos1 );
	cent->gent->angle += cent->gent->speed * 0.08f;
}

/*
----------------------
CG_ShimmeryThing

Creates column or cone of shimmering lines
Kind of looks like a teleporter effect
----------------------
*/

void CG_ShimmeryThing( vec3_t start, vec3_t end, vec3_t content )
{
	vec3_t	normal, angles, base, top, dir;
	float	len;
	int		i;
	int		taper = content[2];

	VectorSubtract( end, start, normal );
	len = VectorNormalize( normal );
	vectoangles( normal, angles );

	for ( i=0; i < 2; i++)
	{
		// Spawn the shards of light around a cylinder
		angles[2] = crandom() * 360;
		AngleVectors( angles, NULL, dir, NULL );

		// See if the effect should be tapered at the top
		if ( taper == 2 )
		{
			VectorMA( start, content[1] * 0.25f, dir, top );
		}
		else
		{
			VectorMA( start, content[1], dir, top );
		}

		VectorMA( end, content[1], dir, base );

		// Use a couple of different kinds to break up the monotony..
		if ( rand() & 1 )
		{
			FX_AddLine( top, base, 1.0f, len * 0.008f, len * 0.19f, 0.3f, 0.0f, random() * 200 + 600, cgs.media.ltblueParticleShader );
		}
		else
		{
			FX_AddLine( top, base, 1.0f, len * 0.008f, len * 0.19f, 0.2f, 0.0f, random() * 200 + 600, cgs.media.spark2Shader );
		}
	}
}

/*
----------------------
CG_Borg_Bolt

Yellow bolts that spark when the endpoints get close together
----------------------
*/

void CG_Borg_Bolt_static( centity_t *cent )
{
	vec3_t	diff;
	float	len;

	// Get the midpoint of the seg
	VectorSubtract( cent->currentState.origin2, cent->currentState.origin, diff );
	len = VectorNormalize( diff );

	// Use this to scale down the width of the bolts.  Otherwise, they will look pretty fairly nasty when they
	//	get too short.
	len = len / 32;

	FX_AddElectricity( cent->currentState.origin, cent->currentState.origin2, 1.0, len, 5.0, 1.0, 0.0, 200, cgs.media.phaserShader, 0 );
}

void CG_Borg_Bolt_dynamic( centity_t *cent )
{
	vec3_t	diff, neworg;
	float	len;


	// Get the midpoint of the seg
	VectorSubtract( cent->currentState.origin2, cent->currentState.origin, diff );
	len = VectorNormalize( diff );
	VectorMA( cent->currentState.origin, len * 0.5, diff, neworg );

	// If the length is pretty short, then spawn a glow spark
	if ( len > 0 && len < 12 )
	{
		int		ct, t;
		vec3_t	angles, dir, vel;
		localEntity_t	*particle;

		FX_AddSprite( neworg, NULL, qfalse, random() * (128 / len) + 12, 16.0, 0.6f, 0.0, 0.0, 0.0, 50, 
					cgs.media.yellowParticleShader );

		vectoangles( dir, angles );

		ct = 12 - len;

		// fun sparks
		for ( t=0; t < ct; t++ )
		{
			angles[1] = random() * 360;
			AngleVectors( angles, dir, NULL, NULL );
			dir[2] = random() * 0.3f;
			VectorScale( dir, 300, vel );

			particle = FX_AddTrail( neworg, NULL, qfalse, 8.0f + random() * 6, -16.0f, 1, -1,
							1.0f, 0.0f, 0.25f, 50, cgs.media.yellowParticleShader );

			if ( particle == NULL )
				return;

			FXE_Spray( dir, 100, 150, 0.5f, vel );
		}

		// If it's really short, spark and make a noise.  Tried this without the if (len>0... and it was way
		//	too obnoxious
		if ( len <= 5 )
		{
			trap_S_StartSound( neworg, 0, 0, trap_S_RegisterSound( "sound/enemies/borg/borgtaser.wav") );
		}
	}

	// Use this to scale down the width of the bolts.  Otherwise, they will look pretty fairly nasty when they
	//	get too short.
	len = len / 32;

	FX_AddElectricity( cent->currentState.origin, cent->currentState.origin2, 1.0, len, 5.0, 1.0, 0.0, 50, cgs.media.phaserShader, 0 );
}

/*
----------------------
CG_StasisDoor

Does Fade-Effect for func_stasis_door
----------------------
*/

void CG_StasisDoor(centity_t *cent, qboolean close) {
	localEntity_t *le;
	
	return;

	FX_AddQuad(cent->currentState.origin, NULL, 10, 100, 1, 1, 0, 1000, cgs.media.photonGlow);

	le = CG_AllocLocalEntity();
	le->leType = LE_STASISDOOR;

	VectorCopy(cent->lerpOrigin, le->refEntity.origin);
	VectorCopy(cent->lerpOrigin, le->refEntity.oldorigin);
	AnglesToAxis( cent->lerpAngles, le->refEntity.axis );

	le->refEntity.renderfx = RF_NOSHADOW | RF_FORCE_ENT_ALPHA;
	le->refEntity.skinNum = 0;

	le->refEntity.hModel = cgs.media.stasisDoorModel;

	trap_R_AddRefEntityToScene(&le->refEntity);
}
