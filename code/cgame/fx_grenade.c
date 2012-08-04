#include "cg_local.h"
#include "fx_local.h"


/*
-------------------------
FX_GrenadeThink
-------------------------
*/ 

void FX_GrenadeThink( centity_t *cent, const struct weaponInfo_s *weapon )
{
	FX_AddSprite( cent->lerpOrigin, NULL, qfalse, 8.0f + random() * 32.0f, 0.0f, 0.75f, 0.75f, 0, 0.0f, 1, cgs.media.dkorangeParticleShader );
	if ( rand() & 1 )
		FX_AddSprite( cent->lerpOrigin, NULL, qfalse, 16.0f + random() * 32.0f, 0.0f, 0.6f, 0.6f, 0, 0.0f, 1, cgs.media.yellowParticleShader );
}

/*
-------------------------
FX_GrenadeHitWall
-------------------------
*/

void FX_GrenadeHitWall( vec3_t origin, vec3_t normal )
{
	trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.grenadeExplodeSound );	
	CG_SurfaceExplosion( origin, normal, 8, 1, qfalse );
}

/*
-------------------------
FX_GrenadeHitPlayer
-------------------------
*/

void FX_GrenadeHitPlayer( vec3_t origin, vec3_t normal )
{
	trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.grenadeExplodeSound );	
	CG_SurfaceExplosion( origin, normal, 8, 1, qfalse );
}

/*
-------------------------
FX_GrenadeExplode
-------------------------
*/

void FX_GrenadeExplode( vec3_t origin, vec3_t normal )
{
	localEntity_t	*le;
	qhandle_t	null = 0;
	vec3_t			direction, org, vel;
	int i;

	VectorSet( direction, 0,0,1 );

	// Add an explosion and tag a light to it
	le = CG_MakeExplosion2( origin, direction, cgs.media.nukeModel, 5, null, 250, qfalse, 25.0f, LEF_FADE_RGB);
	le->light = 150;
	le->refEntity.renderfx |= RF_NOSHADOW;

	VectorSet( le->lightColor, 1.0f, 0.6f, 0.2f );

	// Ground ring
	FX_AddQuad( origin, normal, 5, 100, 1.0, 0.0, random() * 360, 300, cgs.media.bigShockShader );
	// Flare
	VectorMA( origin, 12, direction, org );
	FX_AddSprite( org, NULL, qfalse, 160.0, -160.0, 1.0, 0.0, 0.0, 0.0, 200, cgs.media.sunnyFlareShader );//, FXF_NON_LINEAR_FADE );

	for (i = 0; i < 12; i++)
	{
		float width, length;
		FXE_Spray( normal, 470, 325, 0.5f, vel);
		length = 24.0 + random() * 12;
		width = 0.5 + random() * 2;
		FX_AddTrail( origin, vel, qtrue, length, -length, width, -width, 
						1.0f, 1.0f, 0.5f, 1000.0f,  cgs.media.orangeTrailShader);
	}

	trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.grenadeExplodeSound );	

	// Smoke and impact
//	FX_AddSpawner( origin, normal, NULL, NULL, 100, 25.0f, 2000.0f, (void *) CG_SmokeSpawn, NULL, 1024 );
	CG_ImpactMark( cgs.media.compressionMarkShader, origin, normal, random()*360, 1,1,1,1.0, qfalse, 
				random() * 16 + 48, qfalse );
}


/*void FX_GrenadeShrapnelExplode( vec3_t origin, vec3_t norm )
{
	localEntity_t	*le;
	vec3_t			direction, org, vel;
	int i;

	VectorCopy( norm, direction);

	// Add an explosion and tag a light to it
	le = CG_MakeExplosion2( origin, direction, cgs.media.nukeModel, 5, (qhandle_t)NULL, 250, qfalse, 25.0f, LEF_FADE_RGB);
	le->light = 150;
	le->refEntity.renderfx |= RF_NOSHADOW;

	VectorSet( le->lightColor, 1.0f, 0.6f, 0.2f );

	// Ground ring
	FX_AddQuad( origin, norm, 5, 100, 1.0, 0.0, random() * 360, 300, cgs.media.bigShockShader );
	// Flare
	VectorMA( origin, 12, direction, org );
	FX_AddSprite( org, NULL, qfalse, 160.0, -160.0, 1.0, 0.0, 0.0, 0.0, 200, cgs.media.sunnyFlareShader );//, FXF_NON_LINEAR_FADE );

	for (i = 0; i < 12; i++)
	{
		float width, length;
		FXE_Spray( norm, 470, 325, 0.5f, vel);
		length = 24.0 + random() * 12;
		width = 0.5 + random() * 2;
		FX_AddTrail( origin, vel, qtrue, length, -length, width, -width, 
						1.0f, 1.0f, 0.5f, 1000.0f,  cgs.media.orangeTrailShader);
	}

	trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.grenadeExnull
	// Smoke and impact
	CG_ImpactMark( cgs.media.compressionMarkShader, origin, norm, random()*360, 1,1,1,1.0, qfalse, 
				random() * 16 + 48, qfalse );
}*/

//-----------------------------------
//By: RedTechie - Imported/Modifyed from SP
//-----------------------------------
void FX_GrenadeShrapnelExplode( vec3_t origin, vec3_t norm )
{
	localEntity_t	*le;
	//FXTrail			*fx;
	vec3_t			direction, org, vel;
	int				i;

	CG_InitLensFlare( origin, 
						350, 350,
						colorTable[CT_DKRED1], 1.2, 2.0, 1600, 200,
						colorTable[CT_DKRED1], 1600, 200, 800, 20,  qtrue, 
						0, 0, qfalse, qtrue, 
						qfalse, 1.0, cg.time, 90, 0, 300);

	//Orient the explosions to face the camera
	VectorSubtract( cg.refdef.vieworg, origin, direction );
	VectorNormalize( direction );

	VectorMA( origin, 12, direction, org );
	// Add an explosion and tag a light to it
	le = CG_MakeExplosion2( org, direction, cgs.media.explosionModel, 6, cgs.media.surfaceExplosionShader, 700, qfalse, 1.2f + (random()*0.5f),LEF_FADE_RGB ); //RPG-X: RedTechie - Scale use to be 1.2f + (random()*0.3f)
	le->light = 150;
	le->refEntity.renderfx |= RF_NOSHADOW;
	VectorSet( le->lightColor, 1.0f, 0.6f, 0.6f );

	VectorMA( org, 8, norm, direction );
	VectorSet(vel, 0, 0, 8);
	//Some smoke
	FX_AddSprite(	direction, 
					vel, 
					qfalse, 
					20.0f + random()*50.0f,//1.2f + (random()*0.5f),//60.0f - random()*60.0f
					16.0f,
					100.0f,//1.0f
					100.0f,//0.0f
					random()*45.0f,
					-12.0f,
					8000.0f,
					cgs.media.steamShader );


	for ( i = 0; i < 6; i++)
	{	
		float width, length;
		FXE_Spray( norm, 500, 175, 0.8f, vel);//, (FXPrimitive *) fx 
		length = 24.0 + random() * 12;
		width = 0.5 + random() * 2;
		FX_AddTrail( origin, vel, qtrue, length, -length, width, -width, 
						1.0f, 1.0f, 0.5f, 2500.0f,  cgs.media.orangeTrailShader);//RPG-X: RedTechie - Killtime use to be 1000.0f
	
		/*FX_AddTrail( origin, NULL, NULL, 16.0f, -15.0f,
								1.5, -1.5, 1.0f, 1.0f, 0.2f, 1000.0f,  cgs.media.orangeTrailShader, rand() & FXF_BOUNCE );
*/
	/*if ( fx == NULL )
		return;*/

	
	}

	trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.grenadeAltExplodeSnd );	

	CG_ImpactMark( cgs.media.compressionMarkShader, origin, norm, random()*360, 1,1,1,1.0, qfalse, 
					random() * 16 + 48, qfalse );

	CG_ExplosionEffects( origin, 2.0, 350 );
}

qboolean GrenadeBeep(localEntity_t *le)
{
	weaponInfo_t	*weaponInfo = &cg_weapons[WP_8];

	trap_S_StartSound(le->refEntity.origin, ENTITYNUM_WORLD, CHAN_AUTO, weaponInfo->altHitSound);
	return qtrue;
}

/*
-------------------------
FX_GrenadeShrapnelBits
By: RedTechie - From SP
-------------------------
*/

/*void FX_BlowBits( vec3_t start, vec3_t end, vec3_t dir, vec3_t user )
{
	vec3_t	diff, org;
	float	len;
//	FXLine	*fx;

	VectorSubtract( end, start, diff );
	len = VectorNormalize( diff ) * ( 0.2 + random() * 0.3 );
	VectorMA( start, len, diff, org );

	//fx = 
		FX_AddLine( end, start, (int)(random() * 3.2f), 2.0f + random() * 2, 0, 0.5f, 0.1f, 150 + random() * 150, cgs.media.orangeTrailShader ); 

	//if ( fx == NULL )
	//	return;
	
	//fx->SetFlags( FXF_SHRINK );

	FX_AddQuad( end, dir, NULL, NULL, 1.0f, 64.0f, 1.0, 0.0, random() * 360.0f, 0.0f, 0.0, 200, cgs.media.orangeRingShader );
	// FX_AddQuad( end, dir, NULL, NULL, 20.0, -15.0, 0.6, 0.4, 0.0,0.0,0.0,450, cgs.media.borgEyeFlareShader );
}
*/
#define FX_GRENADE_ALT_STICK_TIME		2500
void FX_GrenadeShrapnelBits( vec3_t start  )
{
	vec3_t	zero = {0, 0, 0};
	// check G_MissileStick() to make sure this killtime coincides with that nextthink
	FX_AddSpawner( start, zero, NULL, NULL, qfalse, 300,
							 0, FX_GRENADE_ALT_STICK_TIME, GrenadeBeep, 10 );
}


/*
-------------------------
FX_fxfunc_Explosion
-------------------------
*/
void FX_fxfunc_Explosion( vec3_t start, vec3_t origin, vec3_t normal )
{
	localEntity_t	*le;
	vec3_t			dir;
	vec3_t			velocity;
//	vec3_t			end;
//	trace_t			trace;
	float			scale, dscale;
	int				i, j, numSparks;
	//weaponInfo_t	*weaponInfo = &cg_weapons[WP_6];
	//float	scale, dscale;
//	int		s;
//	vec3_t	new_org;

	//Sparks
	numSparks = 20 + (random() * 4.0f);//4

	for ( i = 0; i < numSparks; i++ )
	{	
		scale = 0.25f + (random() * 1.0f);
		dscale = -scale;

				//Randomize the direction
		for (j = 0; j < 3; j ++ )
		{
			dir[j] = normal[j] + (0.75 * crandom());
		}

		VectorNormalize(dir);

		//set the speed
		VectorScale( dir, 200 + (50 * crandom()), velocity);

		le = FX_AddTrail( origin,
								velocity,
								qtrue,
								4.0f,
								-4.0f,
								scale,
								-scale,
								1.0f,
								1.0f,
								0.5f,
								1000.0f,
								cgs.media.sparkShader);

	}

	VectorMA( origin, 8, normal, dir );
	VectorSet(velocity, 0, 0, 8);

	// Smoke puffs
	FX_AddSprite(	dir,
					velocity, 
					qfalse, 
					20.0f + random()*60.0f,//2.2f + ( crandom() * 0.9f),//60.0f - random()*60.0f
					16.0f,
					100.0f,//1.0f
					100.0f,//0.0f
					random()*45.0f,
					-12.0f,
					8000.0f,
					cgs.media.steamShader );

	//Orient the explosions to face the camera
	VectorSubtract( cg.refdef.vieworg, origin, dir );
	VectorNormalize( dir );

	le = CG_MakeExplosion2( origin, dir, cgs.media.explosionModel, 5, cgs.media.electricalExplosionSlowShader, 475, qfalse, 2.2f + ( crandom() * 0.9f), LEF_NONE);//RPG-X: RedTechie - Scale use to be - 1.2f + ( crandom() * 0.3f)
	le->light = 150;
	le->refEntity.renderfx |= RF_NOSHADOW;
	VectorSet( le->lightColor, 0.8f, 0.8f, 1.0f );

	CG_ImpactMark( cgs.media.compressionMarkShader, origin, normal, random()*360, 1,1,1,1.0, qfalse, 
					random() * 16 + 48, qfalse );
	//CG_ImpactMark( cgs.media.compressionMarkShader, origin, normal, random()*360, 1,1,1,1, qfalse, 12, qfalse );

	//Shake the camera
	CG_ExplosionEffects( origin, 2, 400 );

	// nice explosion sound at the point of impact
	trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.grenadeAltExplodeSnd );
	//trap_S_StartSound(origin, ENTITYNUM_WORLD, CHAN_AUTO, weaponInfo->mainHitSound);
}


/*
-------------------------
FX_fxfunc_Shot
-------------------------
*/
#define MAXRANGE_CRIFLE		8192
void FX_fxfunc_Shot( vec3_t start, vec3_t dir )
{
	vec3_t			end;
	trace_t			trace;
	
	VectorMA(start, MAXRANGE_CRIFLE, dir, end);
	CG_Trace( &trace, start, NULL, NULL, end, 0, MASK_SHOT );
	
	//FX_CompressionExplosion(start, trace.endpos, trace.plane.normal, qfalse );
	FX_fxfunc_Explosion(start, trace.endpos, trace.plane.normal);
}
