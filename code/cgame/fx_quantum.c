#include "cg_local.h"
#include "fx_local.h"

void FX_QuantumThink( centity_t *cent, const struct weaponInfo_s *weapon )
{
	vec3_t	line1end, line2end, axis[3], rgb;

	AxisClear( axis ); 

	// convert direction of travel into axis
	if ( VectorNormalize2( cent->currentState.pos.trDelta, axis[0] ) == 0 ) {
		axis[0][2] = 1;
	}

	// spin as it moves
	RotateAroundDirection( axis, cg.time * 0.3f );// * 1.25f );

	VectorMA( cent->lerpOrigin, -24.0f, axis[1], line1end );
	VectorMA( cent->lerpOrigin,  24.0f, axis[1], line2end );
	FX_AddLine( line1end, line2end, 1.0f, random() * 6 + 2, 0.0f, 0.2 + random() * 0.2, 0.0f, 1, cgs.media.yellowParticleShader );

	AxisClear( axis ); 

	// convert direction of travel into axis
	if ( VectorNormalize2( cent->currentState.pos.trDelta, axis[0] ) == 0 ) {
		axis[0][2] = 1;
	}

	// spin as it moves
	RotateAroundDirection( axis, -cg.time * 0.3f );// * 1.25f );

	VectorMA( cent->lerpOrigin, -48.0f, axis[2], line1end );
	VectorMA( cent->lerpOrigin,  48.0f, axis[2], line2end );
	FX_AddLine( line1end, line2end, 1.0f, random() * 5 + 2, 0.0f, 0.1 + random() * 0.2, 0.0f, 1, cgs.media.yellowParticleShader );

	VectorSet( rgb, 1.0f, 0.45f, 0.15f ); // orange

	FX_AddSprite( cent->lerpOrigin, NULL,qfalse,random() * 60 + 30, 4, 0.5f, 0.0f, 0, 0.0f, 1.0f, cgs.media.orangeParticleShader );	
	FX_AddSprite2(cent->lerpOrigin, NULL,qfalse,random() * 10 + 60, 0.0f, 0.1f, 0.1f, rgb, rgb, 0.0f, 0.0f, 1, cgs.media.whiteRingShader );
	FX_AddSprite( cent->lerpOrigin, NULL,qfalse,random() * 16 + 8, 4, 0.5f, 0.0f, 0, 0.0f, 1.0f, cgs.media.yellowParticleShader );
	
	/*
	VectorCopy( cent->lerpOrigin, quantumFlare.worldCoord );
	VectorCopy( colorTable[CT_CYAN], quantumFlare.glowColor );
	VectorCopy( colorTable[CT_CYAN], quantumFlare.streakColor );

	CG_DrawLensFlare( quantumFlare );*/
}

/*
-------------------------
FX_QuantumAltThink
-------------------------
*/
void FX_QuantumAltThink( centity_t *cent, const struct weaponInfo_s *weapon )
{
	vec3_t	line1end, line2end, axis[3], vel, rgb;
	float	scale;

	AxisClear( axis ); 

	// convert direction of travel into axis
	if ( VectorNormalize2( cent->currentState.pos.trDelta, axis[0] ) == 0 ) {
		axis[0][2] = 1;
	}

	// spin as it moves
	RotateAroundDirection( axis, cg.time * 0.3f );// * 1.25f );

	VectorMA( cent->lerpOrigin, -48.0f, axis[1], line1end );
	VectorMA( cent->lerpOrigin,  48.0f, axis[1], line2end );
	FX_AddLine( line1end, line2end, 1.0f, random() * 6 + 2, 0.0f, 0.2 + random() * 0.2, 0.0f, 1, cgs.media.yellowParticleShader );

	VectorMA( cent->lerpOrigin, -48.0f, axis[2], line1end );
	VectorMA( cent->lerpOrigin,  48.0f, axis[2], line2end );
	FX_AddLine( line1end, line2end, 1.0f, random() * 5 + 2, 0.0f, 0.2 + random() * 0.2, 0.0f, 1, cgs.media.yellowParticleShader );

	VectorSet( rgb, 1.0f, 0.45f, 0.15f ); // orange

	FX_AddSprite( cent->lerpOrigin, NULL,qfalse,random() * 60 + 30, 4, 0.5f, 0.0f, 0, 0.0f, 1.0f, cgs.media.orangeParticleShader );	
	FX_AddSprite2(cent->lerpOrigin, NULL,qfalse,random() * 10 + 60, 0.0f, 0.1f, 0.1f, rgb, rgb, 0.0f, 0.0f, 1, cgs.media.whiteRingShader );
	FX_AddSprite( cent->lerpOrigin, NULL,qfalse,random() * 16 + 8, 4, 0.5f, 0.0f, 0, 0.0f, 1.0f, cgs.media.yellowParticleShader );	

	scale = ( 2.0f + cos( cg.time * ( M_PI * 0.001f * 4 ))) * 0.5f;

	// Unlike the main fire, I'm leaving around this center core for a moment as a trail...
	VectorScale( cent->currentState.pos.trDelta, 0.25f, vel );
	FX_AddSprite( cent->lerpOrigin, NULL,qfalse,scale * 8 + 2, scale * -5.0f, 0.8f, 0.0f, 0, 0, 300.0f, cgs.media.sunnyFlareShader);

	// Tack on a sprite trail so we can see the cool tracking at work.
	VectorSet( vel, flrandom(-12, 12), flrandom(-12, 12), flrandom(-12, 12));
	VectorMA( vel, 0.25f, cent->currentState.pos.trDelta, vel);

	if ( rand() & 1 )
		FX_AddSprite( cent->lerpOrigin, vel,qfalse,random() * 12.0f + scale * 14, -10, 0.2f + random() * 0.2f, 0.0, random()*360, 0, 800 + random() * 200.0f, 
						cgs.media.orangeRingShader );
	else
		FX_AddSprite2(cent->lerpOrigin, vel,qfalse,random() * 12.0f + scale * 14, -10, 0.5, 0.0, rgb, rgb, random()*360, 0, 800 + random() * 200.0f, 
						cgs.media.whiteRingShader );
}

/*
-------------------------
FX_QuantumHitWall
-------------------------
*/
void FX_QuantumHitWall( vec3_t origin, vec3_t normal )
{
	localEntity_t	*le = NULL;
	vec3_t			dir, org;
	vec3_t			vel;
	float			scale;
	int				i;
	weaponInfo_t	*weaponInfo = &cg_weapons[WP_9];

	CG_InitLensFlare( origin, 
					400, 400,
					colorTable[CT_YELLOW], 1.2, 2.0, 1600, 200,
					colorTable[CT_YELLOW], 1600, 200, 800, 35,  qtrue, 
					0, 0, qfalse, qtrue, 
					qfalse, 1.0, cg.time, 0, 0, 200);

	for ( i = 0; i < 12; i++ )
	{
		VectorSet( dir, normal[0] + crandom() * 2, normal[1] + crandom() * 2, normal[2] + crandom() );
		VectorNormalize( dir );
		scale = random() * 300 + 300;
		VectorScale( dir, scale, vel );
		vel[2] += 300;
		if ( rand() & 1 )
		{
			// FX_AddParticle( origin, vel, qfalse, random() * 14 + 2, -2.0, 0.9, 0.1, 0.0, 0.0, 300 + random() * 100, cgs.media.yellowParticleShader, explosionTailThink );
			scale = random()*14+2;
			// Instead of the particle code, which seems redundant and doesn't fade real well, try adding the projectile...
			le=FX_AddSprite(origin, vel, qfalse, scale, -scale, 0.9, 0.5, 0.0, 0.0, 200 + random() * 100, cgs.media.yellowParticleShader);
			// ...with a trail that overlaps it exactly.
			FX_AddTrail(origin, vel, qfalse, 80, -40, scale, -scale, 0.8, 0.4, 0.0, 300, cgs.media.orangeTrailShader);
		}
		else
		{
			// FX_AddParticle( origin, vel, qfalse, random() * 14 + 2, -2.0, 0.9, 0.1, 0.0, 0.0, 450 + random() * 200, cgs.media.sunnyFlareShader, explosionTailThink );
			scale = random()*14+6;
			// Instead of the particle code, which seems redundant and doesn't fade real well, try adding the projectile...
			le=FX_AddSprite(origin, vel, qfalse, scale, -scale, 0.9, 0.5, 0.0, 0.0, 350 + random() * 150, cgs.media.sunnyFlareShader);
			// ...with a trail that overlaps it exactly.
			FX_AddTrail(origin, vel, qfalse, 80, -40, scale, -scale, 0.8, 0.4, 0.0, 500, cgs.media.orangeTrailShader);
		}
	}
	// Always face the camera
	VectorSubtract( cg.refdef.vieworg, origin, dir );
	VectorNormalize( dir );

	// Main explosion, tag with light

	le = CG_MakeExplosion2( origin, normal, (qhandle_t)0, 1, cgs.media.quantumExplosionShader, 600, qtrue, 3 + crandom(), 0 );
	le->light = 150;
	le->refEntity.renderfx |= RF_NOSHADOW;
	VectorSet( le->lightColor, 1.0f, 1.0f, 0.6f );

	// Create sphere
	CG_MakeExplosion2( origin, dir, cgs.media.explosionModel, 5, cgs.media.quantumFlashShader, 150, qfalse, 4.6f + ( crandom() * 0.3f), 0 );

	// Make an offset explosion
	for ( i = 0; i < 3; i++ ) {
		org[i] = origin[i] + crandom() * 4;
	}
	
	CG_MakeExplosion( org, dir, 0, cgs.media.quantumExplosionShader, 700, 1, qtrue );
	CG_ImpactMark( cgs.media.compressionMarkShader, origin, normal, random()*360, 1,1,1,1.0, qfalse, 
					random() * 16 + 48, qfalse );

	CG_ExplosionEffects( origin, 3.0f, 256 );

	// One big bright quick flash
	FX_AddSprite( origin, NULL, qfalse, 100, -100, 1.0, 1.0, 0, 0, 300, cgs.media.sunnyFlareShader);


	trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, weaponInfo->mainHitSound );
}

/*
-------------------------
FX_QuantumAltHitWall
-------------------------
*/
void FX_QuantumAltHitWall( vec3_t origin, vec3_t normal )
{
	localEntity_t	*le = NULL;
	vec3_t			dir, org;
	vec3_t			vel;
	float			scale;
	int				i;
	vec3_t	RGB={1.0, 0.6, 0.3}, RGB2={1.0, 0.3, 0.0};
	weaponInfo_t	*weaponInfo = &cg_weapons[WP_9];

	CG_InitLensFlare( origin, 
				500, 500,
				colorTable[CT_YELLOW], 1.2, 2.0, 1600, 200,
				colorTable[CT_YELLOW], 1600, 200, 800, 35,  qtrue, 
				1600, 200, qfalse, qfalse, 
				qfalse, 1.0, cg.time, 0, 0, 350);

	for ( i = 0; i < 12; i++ )
	{
		VectorSet( dir, normal[0] + crandom() * 2, normal[1] + crandom() * 2, normal[2] + crandom() );
		VectorNormalize( dir );
		scale = random() * 500 + 500;
		VectorScale( dir, scale, vel );
		vel[2] += 300;
		if ( rand() & 1 )
		{
			// FX_AddParticle( origin, vel, qfalse, random() * 14 + 2, -2.0, 0.9, 0.1, 0.0, 0.0, 300 + random() * 100, cgs.media.yellowParticleShader, explosionTailThink );
			scale = random()*14+2;
			// Instead of the particle code, which seems redundant and doesn't fade real well, try adding the projectile...
			le=FX_AddSprite2(origin, vel, qfalse, scale, -scale, 0.9, 0.5, RGB, RGB2, 0.0, 0.0, 200 + random() * 100, cgs.media.yellowParticleShader);
			// ...with a trail that overlaps it exactly.
			FX_AddTrail2(origin, vel, qfalse, 80, -40, scale, -scale, 0.8, 0.4, RGB, RGB2, 0.0, 300, cgs.media.orangeTrailShader);
		}
		else
		{
			// FX_AddParticle( origin, vel, qfalse, random() * 14 + 2, -2.0, 0.9, 0.1, 0.0, 0.0, 450 + random() * 200, cgs.media.sunnyFlareShader, explosionTailThink );
			scale = random()*14+6;
			// Instead of the particle code, which seems redundant and doesn't fade real well, try adding the projectile...
			le=FX_AddSprite2(origin, vel, qfalse, scale, -scale, 0.9, 0.5, RGB, RGB2, 0.0, 0.0, 350 + random() * 150, cgs.media.sunnyFlareShader);
			// ...with a trail that overlaps it exactly.
			FX_AddTrail2(origin, vel, qfalse, 80, -40, scale, -scale, 0.8, 0.4, RGB, RGB2, 0.0, 500, cgs.media.orangeTrailShader);
		}
	}
	// Always face the camera
	VectorSubtract( cg.refdef.vieworg, origin, dir );
	VectorNormalize( dir );

	// Main explosion, tag with light

	le = CG_MakeExplosion2( origin, normal, (qhandle_t)0, 1, cgs.media.quantumExplosionShader, 600, qtrue, 3 + crandom(), 0 );
	le->light = 150;
	le->refEntity.renderfx |= RF_NOSHADOW;
	VectorSet( le->lightColor, 1.0f, 1.0f, 0.6f );

	// Create sphere
	CG_MakeExplosion2( origin, dir, cgs.media.explosionModel, 5, cgs.media.quantumFlashShader, 150, qfalse, 5.4f + ( crandom() * 0.3f), 0 );

	// Make an offset explosion
	for ( i = 0; i < 3; i++ ) {
		org[i] = origin[i] + crandom() * 4;
	}
	
	CG_MakeExplosion( org, dir, 0, cgs.media.quantumExplosionShader, 700, 1, qtrue );
	CG_ImpactMark( cgs.media.compressionMarkShader, origin, normal, random()*360, 1,1,1,1.0, qfalse, 
					random() * 16 + 48, qfalse );

	CG_ExplosionEffects( origin, 3.0f, 256 );

	// One big bright quick flash
	FX_AddSprite( origin, NULL, qfalse, 200, -200, 1.0, 1.0, 0, 0, 400, cgs.media.sunnyFlareShader);

	trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, weaponInfo->altHitSound );
}

qboolean FX_QuantumSparkle( localEntity_t *le)
{
	int		t, i;
	vec3_t	org, v;

	for ( i = 0; i < 4; i ++ )
	{
		VectorCopy( le->refEntity.origin, org );

		for ( t = 0; t < 3; t++ )
		{
			org[t] = le->refEntity.origin[t] + crandom() * 12;
			v[t] = crandom() * 18.0f;
		}

		FX_AddSprite( org, v, qfalse, random() * 1 + 1, -4, 0.5f, 1.0f, 0.0f, 0.0f, 125 + random() * 100, cgs.media.yellowParticleShader);
	}
	return qtrue;
}

void FX_QuantumFizzles( vec3_t origin )
{
	float	v;
	vec3_t	dir, vel, org;
	int		i;

	for ( i = 0; i < 32; i++ )
	{
		v = random() * 6.0f + 6.0f;

		VectorSet( dir, crandom(), crandom(), crandom() );
		VectorNormalize( dir );
		VectorScale( dir, v, vel );

		org[0] = origin[0] + dir[0] * 48;
		org[1] = origin[1] + dir[1] * 48;
		org[2] = origin[2] + dir[2] * 64;

		FX_AddSpawner( org, dir, vel, NULL, qfalse, 125, 10 + random() * 30, 200 + random() * 400, FX_QuantumSparkle, 1024 );
	}
}

void FX_QuantumColumns( vec3_t origin )
{
	vec3_t		dir, bottom, top;
	vec3_t		sizeMin = {-4, -4, -1};
	vec3_t		sizeMax = {-4, -4, 1};
	trace_t		trace;
	localEntity_t	*le;

	//Orient the explosions to face the camera
	VectorSubtract( cg.refdef.vieworg, origin, dir );
	VectorNormalize( dir );

	//=== Sound === 
	trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.quantumBoom );	

	//=== columns === 
	VectorCopy( origin, bottom );
	bottom[2] -= 256;

	trap_CM_BoxTrace( &trace, origin, bottom, sizeMin, sizeMax, 0, MASK_OPAQUE );
	VectorCopy( trace.endpos, bottom );

	VectorCopy( origin, top );
	top[2] += 256;

	trap_CM_BoxTrace( &trace, origin, top, sizeMin, sizeMax, 0, MASK_OPAQUE );
	VectorCopy( trace.endpos, top );

	//found floor and ceiling, now do columns and ring explosions:
	//ceiling
	VectorSet( dir, 0, 0, -1 );

	le = FX_AddCylinder( top, dir, top[2] - origin[2], (origin[2] - top[2]), 40, 100, 20, 50, 1.0, 0.0, 1000, cgs.media.quantumRingShader, 1.5 );

	le->refEntity.data.cylinder.wrap = qtrue;
	le->refEntity.data.cylinder.stscale = 6;

	//floor
	VectorSet( dir, 0, 0, 1 );

	le = FX_AddCylinder( bottom, dir, origin[2] - bottom[2], (bottom[2] - origin[2]), 40, 100, 20, 50, 1.0, 0.0, 1000, cgs.media.quantumRingShader, 1.5 );
	le->refEntity.data.cylinder.wrap = qtrue;
	le->refEntity.data.cylinder.stscale = 6;

	FX_QuantumFizzles( origin );

	// Main explosion, tag with light

	le = CG_MakeExplosion2( origin, dir, (qhandle_t)0, 1, cgs.media.quantumExplosionShader, 600, qtrue, 3 + crandom(), 0 );
	le->light = 150;
	le->refEntity.renderfx |= RF_NOSHADOW;
	VectorSet( le->lightColor, 1.0f, 1.0f, 0.6f );


}
