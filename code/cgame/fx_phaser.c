//Phaser

#include "cg_local.h"
#include "fx_local.h"

/*
-------------------------
FX_PhaserFire
-------------------------
*/

lensFlare_t phaserFlare = { {0.0,0.0,0.0},
									20,
									20,
									{1.0, 0.7, 0.13},
									1.2,
									1.5,
									20,
									300,
									{0.0, 0.0, 0.0},
									20,
									300,
									80,
									5,
									qfalse,
									5,
									40,
									qfalse,
									qfalse,
									qtrue,
									1.0,
									1.0,
									1.0,
									1.0,
									1.0,
									qtrue };

void FX_PhaserFire( vec3_t startpos, vec3_t endpos, vec3_t normal, qboolean spark, qboolean impact, qboolean empty )
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
		beam.customShader = cgs.media.phaserShader;
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
		beam.data.line.width = 2.0f + ( crandom() * 0.6f );
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

/*
-------------------------
FX_PhaserAltFire
-------------------------
*/

#define PHASER_ALT_CONE_LEN	256

void FX_PhaserAltFire( vec3_t start, vec3_t end, vec3_t normal, qboolean spark, qboolean impact, qboolean empty )
{
	float		scale = flrandom(13.0f, 17.0f), scale2 = flrandom(2.0f, 6.0f);
	vec3_t		vel, diff, end2;
	int			i = 0, sparks = 0;
	refEntity_t	beam;
	vec3_t		rgb = { 1,0.6,0.5}, rgb2={1,0.3,0};
	float		len;
	int color;

	VectorSubtract(end, start, diff);
	len = VectorNormalize(diff);

	color = 0xff * flrandom(0.75, 1.0);

	if (empty)
	{	// More faint and shaky line.
		scale *= flrandom(0.25,0.75);
	}

	if (len > PHASER_ALT_CONE_LEN)
	{	// Draw beam in two parts...

		// Draw main beam first.
		VectorMA(start, PHASER_ALT_CONE_LEN, diff, end2);

		// Draw starting cone
		memset( &beam, 0, sizeof( beam ) );
		VectorCopy( start, beam.origin);
		VectorCopy( end2, beam.oldorigin );
		beam.reType = RT_LINE2;
		if (empty)
		{
			beam.customShader = cgs.media.phaserAltEmptyShader;
		}
		else
		{
			beam.customShader = cgs.media.phaserAltShader;
		}
		AxisClear( beam.axis );
		beam.shaderRGBA[0] = 0xff;
		beam.shaderRGBA[1] = 0xff*0.3;
		beam.shaderRGBA[2] = 0;
		beam.shaderRGBA[3] = 0xff;
		beam.data.line.width = scale*0.1;
		beam.data.line.width2 = scale;
		beam.data.line.stscale = 1.0;
		trap_R_AddRefEntityToScene( &beam );

		// Draw big thick normal beam for the rest.
		memset( &beam, 0, sizeof( beam ) );
		VectorCopy( end2, beam.oldorigin);
		VectorCopy( end, beam.origin );
		beam.reType = RT_LINE;
		if (empty)
		{
			beam.customShader = cgs.media.phaserAltEmptyShader;
		}
		else
		{
			beam.customShader = cgs.media.phaserAltShader;
		}
		AxisClear( beam.axis );
		beam.shaderRGBA[0] = 0xff;
		beam.shaderRGBA[1] = 0xff*0.3;
		beam.shaderRGBA[2] = 0;
		beam.shaderRGBA[3] = 0xff;
		beam.data.line.width = scale;
		beam.data.line.stscale = 1.0;
		trap_R_AddRefEntityToScene( &beam );

		// Draw beam core, all one bit.
		memset( &beam, 0, sizeof( beam ) );
		VectorCopy( start, beam.origin);
		VectorCopy( end, beam.oldorigin );
		beam.reType = RT_LINE2;
		beam.customShader = cgs.media.phaserShader;
		AxisClear( beam.axis );
		beam.shaderRGBA[0] = color*0.75f;
		beam.shaderRGBA[1] = 0xff*0.5f;
		beam.shaderRGBA[2] = 0xff*0.5f;
		beam.shaderRGBA[3] = 0xff;
		beam.data.line.width = scale2*0.2;
		beam.data.line.width2 = scale2;
		beam.data.line.stscale = 1.0;
		trap_R_AddRefEntityToScene( &beam );
	}
	else
	{	// Draw beam in two parts...
		// Draw beam first.
		memset( &beam, 0, sizeof( beam ) );
		VectorCopy( start, beam.origin);
		VectorCopy( end, beam.oldorigin );
		beam.reType = RT_LINE2;
		beam.customShader = cgs.media.phaserAltShader;
		AxisClear( beam.axis );
		beam.shaderRGBA[0] = 0xff;
		beam.shaderRGBA[1] = 0xff*0.3;
		beam.shaderRGBA[2] = 0;
		beam.shaderRGBA[3] = 0xff;
		beam.data.line.width = scale*0.1;
		beam.data.line.width2 = scale;
		beam.data.line.stscale = 1.0;
		trap_R_AddRefEntityToScene( &beam );

		// just one beam is never enough
		memset( &beam, 0, sizeof( beam ) );
		VectorCopy( start, beam.origin);
		VectorCopy( end, beam.oldorigin );
		beam.reType = RT_LINE2;
		beam.customShader = cgs.media.phaserShader;
		AxisClear( beam.axis );
		beam.shaderRGBA[0] = color*0.75f;
		beam.shaderRGBA[1] = 0xff*0.5f;
		beam.shaderRGBA[2] = 0xff*0.5f;
		beam.shaderRGBA[3] = 0xff;
		beam.data.line.width = scale2*0.2;
		beam.data.line.width2 = scale2;
		beam.data.line.stscale = 1.0;
		trap_R_AddRefEntityToScene( &beam );
	}
	

	// Phaser beam
//	FX_AddLine( start, end, 1.0f, scale, 0.0f, 0.9f, 0.9f, 2, cgs.media.phaserShader );
//	FX_AddLine( start, end, 1.0f, scale * 0.5f, 0.0f, 0.8f, 0.8f, 2, cgs.media.phaserShader );
	
	// Per frame impact mark
	FX_AddQuad( end, normal, random() * 1.5 + 1.75f, 0.0f, 1.0f, 0.0f, 0.0f, 1, cgs.media.sparkShader );
	FX_AddQuad( end, normal, random() * 5 + 2.75f, 0.0f, 1.0f, 0.0f, 0.0f, 1, cgs.media.yellowParticleShader );

	// Multi frame impacts--never do this when it hits a player because it will just look stupid
	if ( impact )
	{
		FX_AddQuad2( end, normal, random() * 2.0 + 5.0f, 2.5f, 0.6f, 0.0f, rgb, rgb2, 0.0f, 500 + random() * 200, 
					cgs.media.sunnyFlareShader );

		CG_ImpactMark( cgs.media.scavMarkShader, end, normal, random()*360, 1,1,1,0.1, qfalse, 
					random() + 6.0, qfalse );
	}

	// "Fun" sparks
	if ( spark )
	{
		// kef -- fixme. dunno what the deal is with this velocity vector
		VectorClear(vel);
		sparks = (rand() & 3) + 1;

		// Set random starting pos...
		end2[0] = flrandom(-1.0, 1.0) + end[0];
		end2[1] = flrandom(-1.0, 1.0) + end[1];
		end2[2] = flrandom(-1.0, 1.0) + end[2];
		for( i = 0; i < sparks; i++ )
		{	
			scale = 0.5f + (random() * 0.5);
			FXE_Spray( normal, 200, 75, 0.8f, /*1024*/vel);
			FX_AddTrail2(	end2, vel, qfalse,
							8.0f, -8.0f,
							scale, -scale, 0.5f, 0.0f, rgb, rgb2, 0.4f, 500.0f, cgs.media.sparkShader );
		}

		VectorMA(end, -8, diff, end2);
		// Add a hit sprite over everything...
		memset( &beam, 0, sizeof( beam ) );
		VectorCopy( end2, beam.origin);
		beam.reType = RT_SPRITE;
		beam.customShader = cgs.media.sunnyFlareShader;
		AxisClear( beam.axis );
		beam.shaderRGBA[0] = 0xff*1.0f;
		beam.shaderRGBA[1] = 0xff*0.9f;
		beam.shaderRGBA[2] = 0xff*0.8f;
		beam.shaderRGBA[3] = 0xff;
		beam.data.sprite.radius = random()*2.0 + 9.0;
		trap_R_AddRefEntityToScene( &beam );
	}
}
