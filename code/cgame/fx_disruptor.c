#include "cg_local.h"
#include "fx_local.h"

/*
-------------------------
FX_OrientedBolt

Creates new bolts for a while
-------------------------
*/

void FX_OrientedBolt( vec3_t start, vec3_t end, vec3_t dir )
{
	vec3_t	mid;

	VectorSubtract( end, start, mid );
	VectorScale( mid, 0.1f + (random() * 0.8), mid );
	VectorAdd( start, mid, mid );
	VectorMA(mid, 3.0f + (random() * 10.0f), dir, mid );

	//FX_AddElectricity( mid, start, 0.5, 0.75 + random() * 0.75, 0.0, 1.0, 0.5, 300.0f + random() * 300, cgs.media.bolt2Shader, DEFAULT_DEVIATION);
	//FX_AddElectricity( mid, end, 0.5, 0.75 + random() * 0.75, 1.0, 1.0, 0.5, 300.0f + random() * 300, cgs.media.bolt2Shader, DEFAULT_DEVIATION);

	FX_AddElectricity( mid, start, 0.5, 0.75 + random() * 0.75, 0.0, 1.0, 0.5, 300.0f + random() * 300, cgs.media.borgLightningShaders[2], DEFAULT_DEVIATION);
	FX_AddElectricity( mid, end, 0.5, 0.75 + random() * 0.75, 1.0, 1.0, 0.5, 300.0f + random() * 300, cgs.media.borgLightningShaders[3], DEFAULT_DEVIATION);
}

/*
-------------------------
FX_DisruptorDischarge

Fun "crawling" electricity ( credit goes to Josh for this one )
-------------------------
*/

void FX_DisruptorDischarge( vec3_t origin, vec3_t normal, int count, float dist_out, float dist_side )
{
	trace_t	trace;
	vec3_t	org, dir, dest;
	vec3_t	vr;
	int		i;
	int		discharge = dist_side;

	vectoangles( normal, dir );
	dir[ROLL] += random() * 360;

	for (i = 0;	i < count; i++)
	{
		//Move out a set distance
		VectorMA( origin, dist_out, normal, org );
		
		//Even out the hits
		dir[ROLL] += (360 / count) + (rand() & 31);
		AngleVectors( dir, NULL, vr, NULL );

		//Move to the side in a random direction
		discharge += (int)( crandom() * 8.0f );
		VectorMA( org, discharge, vr, org );

		//Trace back to find a surface
		VectorMA( org, -dist_out * 3, normal, dest );

		CG_Trace( &trace, org, NULL, NULL, dest, 0, MASK_SHOT );
		
		//No surface found, start over
		if (trace.fraction == 1) 
			continue;

		//Connect the two points with bolts
		FX_OrientedBolt( origin, trace.endpos, normal );

		//TiM : Aww screw it.  Add a lens flare. ^_^
		CG_InitLensFlare( trace.endpos, 
					10, 10,
					colorTable[CT_GREEN], 1.2, 2.0, 1600, 500,
					colorTable[CT_GREEN], 1600, 500, 100, 5,  qtrue, 
					0, 0, qfalse, qtrue, 
					qfalse, 1.0, cg.time, 0, 0, 300.0f + random() * 300);
	}
}

/*
-------------------------
FX_DisruptorWeaponHitWall

Main fire impact
-------------------------
*/

#define NUM_DISCHARGES		6
#define	DISCHARGE_DIST		8
#define	DISCHARGE_SIDE_DIST	24

void FX_DisruptorWeaponHitWall( vec3_t origin, vec3_t dir, int size )
{
	vec3_t			vel, /*accel,*/ hitpos, direction, org;
	//int				i, t;
	weaponInfo_t	*weaponInfo = &cg_weapons[WP_10];

	CG_InitLensFlare( origin, 
					375, 375,
					colorTable[CT_GREEN], 1.2, 2.0, 1600, 200,
					colorTable[CT_GREEN], 1600, 200, 800, 20,  qtrue, 
					0, 0, qfalse, qtrue, 
					qfalse, 1.0, cg.time, 0, 0, 200);

	// Generate "crawling" electricity		// eh, don't it doesn't look that great.
	FX_DisruptorDischarge( origin, dir, NUM_DISCHARGES, DISCHARGE_DIST, DISCHARGE_SIDE_DIST );

	VectorMA(origin, size, dir, hitpos);

	// Set an oriented residual glow effect
	FX_AddQuad( hitpos, dir, size * size * 15.0f, -150.0f, 
				1.0f, 0.0f, 0, 300, cgs.media.greenParticleShader );

	CG_ImpactMark( cgs.media.scavMarkShader, origin, dir, random()*360, 1,1,1,0.6, qfalse, 
					size * 12 + 1, qfalse );

	FX_AddSprite( hitpos, NULL, qfalse, size * size * 15.0f, -150.0f, 
				1.0f, 0.0f, 360*random(), 0, 400, cgs.media.greenParticleShader );

/*	FX_AddSprite( hitpos, NULL, qfalse, size * size * 15.0f, -150.0f, 
				1.0f, 0.0f, 360*random(), 0, 400, cgs.media.greenParticleStreakShader ); */

	FX_AddSprite( hitpos, NULL, qfalse, size * size * 25.0f, -150.0f, 
				1.0f, 0.0f, 0.0f, 0, 400, cgs.media.greenParticleStreakShader );

	VectorSubtract( cg.refdef.vieworg, origin, direction );
	VectorNormalize( direction );

	VectorMA( origin, 12, direction, org );
	VectorMA( org, 8, dir, direction );
	VectorSet(vel, 0, 0, 32 ); //8

	FX_AddSprite( origin, 
						vel, qfalse, 
						random() * 4 + 2, 12,
						0.6 + random() * 0.4, 0.0,
						random() * 180, 
						0.0, 
						random() * 200 + 1200, //300
						cgs.media.steamShader );

	//FX_AddSprite(

	// Only play the impact sound and throw off the purple particles when it's the main projectile
/*	if ( size < 3 )
		return;

	for ( i = 0; i < 4; i++ )
	{
		for ( t = 0; t < 3; t++ )
			vel[t] = ( dir[t] + crandom() * 0.9 ) * ( random() * 100 + 250 );

		VectorScale( vel, -2.2, accel );
		FX_AddSprite( hitpos, vel, qfalse, random() * 8 + 8, 0, 1.0, 0.0, 0.0, 0.0, 200, cgs.media.purpleParticleShader );

	}*/
	trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, weaponInfo->mainHitSound );
}

void FX_DisruptorBeamFire( vec3_t startpos, vec3_t endpos, vec3_t normal, qboolean spark, qboolean impact, qboolean empty )
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
		beam.customShader = cgs.media.disruptorBeam;
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
		beam.data.line.width = 1.5f + ( crandom() * 0.6f );
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
