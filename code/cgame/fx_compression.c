//Compression rifle weapon effects

#include "cg_local.h"
#include "fx_local.h"

qboolean AltCompressionAftereffect(localEntity_t *le)
{
	localEntity_t *cyl = NULL;
	qhandle_t	shader = cgs.media.compressionAltBlastShader;
	float		percentLife = 1.0 - (le->endTime - cg.time)*le->lifeRate;
	float		alpha = 0.6 - (0.6*percentLife);
	float		length = 20;
	vec3_t		vec2, dir2;

	cyl = FX_AddCylinder(	le->refEntity.origin, 
							le->data.spawner.dir,
							length,// height,
							0,// dheight,
							10,//10+(30*(1-percentLife)),// scale,
							210,// dscale,
							10+(30*percentLife),// scale2,
							210,// dscale2,
							alpha,// startalpha, 
							0.0,// endalpha, 
							500,// killTime, 
							shader,
							15);// bias );
	cyl->leFlags |= LEF_ONE_FRAME;

	VectorMA(le->refEntity.origin, length*2.0, le->data.spawner.dir, vec2);
	VectorScale(le->data.spawner.dir, -1.0, dir2);
	cyl = FX_AddCylinder(	vec2, 
							dir2,
							length,// height,
							0,// dheight,
							10,//10+(30*(1-percentLife)),// scale,
							210,// dscale,
							10+(30*percentLife),// scale2,
							210,// dscale2,
							alpha,// startalpha, 
							0.0,// endalpha, 
							500,// killTime, 
							shader,
							15);// bias );
	cyl->leFlags |= LEF_ONE_FRAME;

	return qtrue;
}

/*
-------------------------
FX_CompressionShot
-------------------------
*/
#define MAXRANGE_CRIFLE		8192
void FX_CompressionShot( vec3_t start, vec3_t dir )
{
	localEntity_t	*le;
	vec3_t			end;
	trace_t			trace;
	qboolean		render_impact = qtrue;
	centity_t		*traceEnt = NULL;
	int			clientNum = -1;

	VectorMA(start, MAXRANGE_CRIFLE, dir, end);
	CG_Trace( &trace, start, NULL, NULL, end, 0, MASK_SHOT );

	// draw the beam
	le = FX_AddLine(start, trace.endpos, 1.0, 2.0, 0.0, 1.0, 1.0, 100.0, cgs.media.prifleBolt);
	le->leFlags |= LEF_ONE_FRAME;

	// draw an impact at the endpoint of the trace
	// If the beam hits a skybox, etc. it would look foolish to add in an explosion
	if ( trace.surfaceFlags & SURF_NOIMPACT ) 
	{
		render_impact = qfalse;
	}
	if ( render_impact )
	{
		traceEnt = &cg_entities[trace.entityNum];
		clientNum = traceEnt->currentState.clientNum;
		if ( (trace.entityNum != ENTITYNUM_WORLD) && (clientNum >= 0 || clientNum < MAX_CLIENTS) )
		{
			FX_CompressionHit(trace.endpos);
		} 
		else 
		{
			FX_CompressionExplosion(start, trace.endpos, trace.plane.normal, qfalse);
		}
	}
}
/*
-------------------------
FX_CompressionShot
-------------------------
*/
void FX_CompressionAltShot( vec3_t start, vec3_t dir )
{
	vec3_t			end, vel = {0,0,0};
	trace_t			trace;
	qboolean		render_impact = qtrue;
	centity_t		*traceEnt = NULL;
	int			clientNum = -1;

	VectorMA(start, MAXRANGE_CRIFLE, dir, end);
	CG_Trace( &trace, start, NULL, NULL, end, cg_entities[cg.predictedPlayerState.clientNum].currentState.number, MASK_SHOT );

	// draw the beam
	FX_AddLine( start, trace.endpos, 1.0f, 3.0f, 0.0f, 1.0f, 0.0f, 350/*125.0f*/, cgs.media.sparkShader );
	FX_AddLine( start, trace.endpos, 1.0f, 6.0f, 20.0f, 0.6f, 0.0f, 800/*175.0f*/, cgs.media.phaserShader);//compressionAltBeamShader );

	FX_AddSpawner( start, dir, vel, NULL, qfalse, 0,
							 0, 500, AltCompressionAftereffect, 10 );

	// draw an impact at the endpoint of the trace
	// If the beam hits a skybox, etc. it would look foolish to add in an explosion
	if ( trace.surfaceFlags & SURF_NOIMPACT ) 
	{
		render_impact = qfalse;
	}
	if ( render_impact )
	{
		traceEnt = &cg_entities[trace.entityNum];
		clientNum = traceEnt->currentState.clientNum;
		if ( (trace.entityNum != ENTITYNUM_WORLD) && (clientNum >= 0 || clientNum < MAX_CLIENTS) )
		{
			FX_CompressionHit(trace.endpos);
		} 
		else 
		{
			FX_CompressionExplosion(start, trace.endpos, trace.plane.normal, qtrue);
		}
	}
}

/*
-------------------------
FX_CompressionExplosion
-------------------------
*/

void FX_CompressionExplosion( vec3_t start, vec3_t origin, vec3_t normal, qboolean altfire )
{
	localEntity_t	*le;
	vec3_t			dir;
	vec3_t			velocity;  //, shot_dir;
	vec3_t			hitpos;
	float			scale, dscale;
	int				i, j, numSparks;
	weaponInfo_t	*weaponInfo = &cg_weapons[WP_6];
	float distance;	

	vec3_t color = {0.7, 0.43, 0.44};

	int size = 2;

	//FX_CompressionHit( origin ); //TiM: let's test if the rifle doesn't make stuff explode when its shot :)
	//return;
	
	//Sparks
	//TiM: Calc spark count off proximity to effect
	VectorSubtract ( cg.refdef.vieworg, origin, dir );
	distance = VectorNormalize( dir );
	distance = 50 * ( 1.0f - (distance / 128) ) ;
	distance = Com_Clamp( 25, 50, distance );

	numSparks = distance + (random() * 4.0f); //4
	
	if (altfire)
	{
		numSparks *= 1.5f;
	}
	for ( i = 0; i < numSparks; i++ )
	{	
		scale = 10.0f + (random() * 1.0f); //.25
		dscale = -scale;

				//Randomize the direction
		for (j = 0; j < 3; j ++ )
		{
			//if ( j !=5 )
				//dir[j] = normal[j] + (0.75 * crandom());
			//else
				dir[j] = normal[j] + (-1 * crandom()); //0.75
		}

		VectorNormalize(dir);

		//set the speed
		VectorScale( dir, 200 + (50 * crandom()), velocity); //200

		le = FX_AddTrail( origin,
								velocity,
								qtrue, //qtrue
								12.0f,//4
								-12.0f,//4
								scale,
								-scale,
								1.0f,
								1.0f,
								0.5f,
								1000.0f, //1000
								cgs.media.orangeStarShader);

//		FXE_Spray( normal, 200, 50, 0.4f, le);
	}

	VectorMA( origin, 8, normal, dir );
	VectorSet(velocity, 0, 0, 8);
/*
	FX_AddSprite(	dir, 
					velocity, 
					qfalse, 
					(altfire?50.0f:32.0f),
					16.0f,
					1.0f,
					0.0f,
					random()*45.0f,
					0.0f,
					(altfire?1300.0f:1000.0f),
					cgs.media.steamShader );
*/
	//Orient the explosions to face the camera
	VectorSubtract( cg.refdef.vieworg, origin, dir );
	VectorNormalize( dir );

	if (!altfire)
	{
		CG_InitLensFlare( origin, 
						350, 350,
						color, 1.2, 2.0, 1600, 200,
						color, 1600, 200, 800, 20,  qtrue, 
						0, 0, qfalse, qtrue, 
						qfalse, 1.0, cg.time, 0, 0, 210);

		
		VectorMA(origin, size, normal, hitpos);

		FX_AddSprite( hitpos, NULL, qfalse, size * size * 15.0f, -150.0f, 
				1.0f, 0.0f, 360*random(), 0, 400, cgs.media.liteRedParticleShader );

		FX_AddSprite( hitpos, NULL, qfalse, size * size * 25.0f, -150.0f, 
			1.0f, 0.0f, 0.0f, 0, 400, cgs.media.liteRedParticleStreakShader );


		le = CG_MakeExplosion2( origin, dir, cgs.media.explosionModel, 5, cgs.media.electricalExplosionSlowShader, 
								475, qfalse, 1.2f + ( crandom() * 0.3f), LEF_NONE);
		le->light = 150;
		le->refEntity.renderfx |= RF_NOSHADOW;
		VectorSet( le->lightColor, 0.8f, 0.8f, 1.0f );

		CG_ImpactMark( cgs.media.compressionMarkShader, origin, normal, random()*360, 1,1,1,1, qfalse, 12, qfalse );

		//Shake the camera
		CG_ExplosionEffects( origin, 1, 200 );
	}
	else
	{
		le = CG_MakeExplosion2( origin, dir, cgs.media.explosionModel, 5, cgs.media.electricalExplosionSlowShader, 
								500, qfalse, 2.2f + ( crandom() * 0.4f), LEF_NONE);
		le->light = 200;
		le->refEntity.renderfx |= RF_NOSHADOW;
		VectorSet( le->lightColor, 0.8f, 0.8f, 1.0f );

		CG_ImpactMark( cgs.media.compressionMarkShader, origin, normal, random()*360, 1,1,1,1, qfalse, 28, qfalse );

		//Shake the camera
		CG_ExplosionEffects( origin, 2, 240 );
	}

	// nice explosion sound at the point of impact
	trap_S_StartSound(origin, ENTITYNUM_WORLD, CHAN_AUTO, weaponInfo->mainHitSound);
}

/*
-------------------------
FX_CompressionHit
-------------------------
*/

void FX_CompressionHit( vec3_t origin )
{
	FX_AddSprite( origin, 
					NULL,
					qfalse,
					32.0f,
					-32.0f,
					1.0f,
					1.0f,
					random()*360,
					0.0f,
					250.0f,
					cgs.media.prifleImpactShader );

	//FIXME: Play an impact sound with a body
//	trap_S_StartSound (origin, NULL, 0, cgi_S_RegisterSound ("sound/weapons/prifle/fire.wav") );
}

void FX_PrifleBeamFire( vec3_t startpos, vec3_t endpos, vec3_t normal, qboolean spark, qboolean impact, qboolean empty )
{
	refEntity_t		beam;
	sfxHandle_t		sfx;
	float			size;
	vec3_t			velocity;
	int				sparks;
	vec3_t			rgb = { 1,0.9,0.6}, rgb2={1,0.3,0};

	//vec3_t			rgb3 = { 1.0, 1.0, 1.0 };

	sfx = 0;

	// Draw beam first.
	memset( &beam, 0, sizeof( beam ) );

	VectorCopy( startpos, beam.origin);
	VectorCopy( endpos, beam.oldorigin );
	beam.reType = RT_LINE;
	if (empty)
	{
		beam.customShader = cgs.media.phaserEmptyShader;
	}
	else
	{
		beam.customShader = cgs.media.prifleBeam;
	}
	AxisClear( beam.axis );
	beam.shaderRGBA[0] = 0xff;
	beam.shaderRGBA[1] = 0xff;
	beam.shaderRGBA[2] = 0xff;
	beam.shaderRGBA[3] = 0xff;
	if (empty)
	{
		beam.data.line.width = 1.0f + ( crandom() * 0.6f );
	}
	else
	{
		beam.data.line.width = 2.5f + ( crandom() * 0.6f );
	}
	beam.data.line.stscale = 5.0;
	trap_R_AddRefEntityToScene( &beam );

	// Now draw the hit graphic
 
	// no explosion at LG impact, it is added with the beam

	if ( sfx )
	{
		Com_Printf("playing %s\n", "phaser sound");
		trap_S_StartSound( endpos, ENTITYNUM_WORLD, CHAN_AUTO, sfx );
	}

	//
	// impact mark
	//
	if (impact)
	{
		if (!empty)
		{	// normal.
			CG_ImpactMark( cgs.media.scavMarkShader, endpos, normal, random()*360, 1,1,1,0.2, qfalse, 
						random() + 1, qfalse );
			
			//VectorCopy( endpos, phaserFlare.worldCoord );

			/*CG_InitLensFlare( endpos,
									80,
									80,
									rgb,
									1.2,
									1.5,
									1600,
									200,
									colorTable[CT_BLACK],
									1600,
									200,
									80,
									5,
									qfalse,
									5,
									40,
									qfalse,
									qfalse,
									qfalse,
									1.0,
									1.0,
									200.0,
									200.0,
									200.0 );*/
			
			//CG_InitLensFlare( endpos, 
			//		30, 30,
			//		rgb, 1.2, 2.0, 1600, 200,
			//		colorTable[CT_BLACK], 1600, 200, 410, 15, qfalse, 
			//		0, 0, qfalse, qtrue, 
			//		qfalse, 1.0, cg.time, 0, 0, 50);

			//TiM : Add your basic cheesy 'seen-way-too-much-in-movies-these-days' anamorphic lens streak :)
			//CG_DrawLensFlare( &phaserFlare );
			//FX_AddSprite( endpos, NULL, qfalse, random() * 1.25 + 5.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 50.0, cgs.media.blueParticleStreakShader ); //1.5f
			
			//FX_AddQuad2(  endpos, normal, random() * 1.25 + 8.0f, 0.0f, 1.0f, 1.0f, rgb3, rgb3, 270, 50.0, cgs.media.blueParticleStreakShader );
			//eh... looked bad :P

			FX_AddQuad2( endpos, normal, random() * 1.25 + 1.5f, 0.0f, 1.0f, 0.0f, rgb, rgb2, rand() % 360, 500 + random() * 200, 
						cgs.media.sunnyFlareShader );
		}
		else
		{	// Wuss hit when empty.
			FX_AddQuad2( endpos, normal, random() * .75 + 1.0f, 0.0f, 0.5f, 0.0f, rgb, rgb2, rand() % 360, 300 + random() * 200, 
						cgs.media.sunnyFlareShader );
		}
	}

	// "Fun" sparks...  Not when empty.
	if ( spark && !empty)
	{
		sparks = (rand() & 1) + 1;
		for(;sparks>0;sparks--)
		{	
			size = 0.2f + (random() * 0.4);
			FXE_Spray( normal, 200, 75, 0.8f, velocity);
			if (rand() & LEF_USE_COLLISION)
			{	// This spark bounces.
				FX_AddTrail( endpos, velocity, qtrue, 5.0f, -15.0f,
										size, -size, 1.0f, 0.5f, 0.4f, 500.0f, cgs.media.sparkShader);
			}
			else
			{
				FX_AddTrail( endpos, velocity, qtrue, 5.0f, -15.0f,
										size, -size, 1.0f, 0.5f, 0.0, 500.0f, cgs.media.sparkShader);
			}
		}
	}
}

