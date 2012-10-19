// Copyright (C) 1999-2000 Id Software, Inc.
//
// cg_effects.c -- these functions generate localentities, usually as a result
// of event processing

#include "cg_local.h"
#include "fx_local.h"


/*
==================
CG_BubbleTrail

Bullets shot underwater
==================
*/
void CG_BubbleTrail( vec3_t start, vec3_t end, float spacing ) {
	vec3_t		move;
	vec3_t		vec;
	float		len;
	int			i;

	VectorCopy (start, move);
	VectorSubtract (end, start, vec);
	len = VectorNormalize (vec);

	// advance a random amount first
	i = rand() % (int)spacing;
	VectorMA( move, i, vec, move );

	VectorScale (vec, spacing, vec);

	for ( ; i < len; i += spacing ) {
		localEntity_t	*le;
		refEntity_t		*re;

		le = CG_AllocLocalEntity();
		le->leFlags = LEF_PUFF_DONT_SCALE;
		le->leType = LE_MOVE_SCALE_FADE;
		le->startTime = cg.time;
		le->endTime = cg.time + 1000 + random() * 250;
		le->lifeRate = 1.0 / ( le->endTime - le->startTime );

		re = &le->refEntity;
		re->shaderTime = cg.time * 0.001f;

		re->reType = RT_SPRITE;
		re->data.sprite.rotation = 0;
		re->data.sprite.radius = 3;
		re->customShader = cgs.media.waterBubbleShader;
		re->shaderRGBA[0] = 0xff;
		re->shaderRGBA[1] = 0xff;
		re->shaderRGBA[2] = 0xff;
		re->shaderRGBA[3] = 0xff;

		le->color[3] = 1.0;

		le->pos.trType = TR_LINEAR;
		le->pos.trTime = cg.time;
		VectorCopy( move, le->pos.trBase );
		le->pos.trDelta[0] = crandom()*5;
		le->pos.trDelta[1] = crandom()*5;
		le->pos.trDelta[2] = crandom()*5 + 6;

		VectorAdd (move, vec, move);
	}
}

/*
=====================
CG_SmokePuff

Adds a smoke puff or blood trail localEntity.
=====================
*/
localEntity_t *CG_SmokePuff( const vec3_t p, const vec3_t vel, 
				   float radius,
				   float r, float g, float b, float a,
				   float duration,
				   int startTime,
				   int leFlags,
				   qhandle_t hShader ) {
	static int	seed = 0x92;
	localEntity_t	*le;
	refEntity_t		*re;

	le = CG_AllocLocalEntity();
	le->leFlags = leFlags;
	le->data.sprite.radius = radius;

	re = &le->refEntity;
	re->data.sprite.rotation = Q_random( &seed ) * 360;
	re->data.sprite.radius = radius;
	re->shaderTime = startTime * 0.001f;

	le->leType = LE_MOVE_SCALE_FADE;
	le->startTime = startTime;
	le->endTime = startTime + duration;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );
	le->color[0] = r;
	le->color[1] = g; 
	le->color[2] = b;
	le->color[3] = a;


	le->pos.trType = TR_LINEAR;
	le->pos.trTime = startTime;
	VectorCopy( vel, le->pos.trDelta );
	VectorCopy( p, le->pos.trBase );

	VectorCopy( p, re->origin );
	re->customShader = hShader;

	// rage pro can't alpha fade, so use a different shader
	if ( cgs.glconfig.hardwareType == GLHW_RAGEPRO ) {
		re->customShader = cgs.media.smokePuffRageProShader;
		re->shaderRGBA[0] = 0xff;
		re->shaderRGBA[1] = 0xff;
		re->shaderRGBA[2] = 0xff;
		re->shaderRGBA[3] = 0xff;
	} else {
		re->shaderRGBA[0] = le->color[0] * 0xff;
		re->shaderRGBA[1] = le->color[1] * 0xff;
		re->shaderRGBA[2] = le->color[2] * 0xff;
		re->shaderRGBA[3] = 0xff;
	}

	re->reType = RT_SPRITE;
	re->data.sprite.radius = le->data.sprite.radius;

	return le;
}

/*
==================
CG_SpawnEffect

Player teleporting in or out

RPG-X: RedTechie Added refEntity_t *ent_legs, refEntity_t *ent_torso, refEntity_t *ent_head
==================
*/
void CG_SpawnEffect( vec3_t org, refEntity_t *ent_legs, refEntity_t *ent_torso, refEntity_t *ent_head ) {
	localEntity_t	*le;
	refEntity_t		*re;

	FX_Transporter(org);
	le = CG_AllocLocalEntity();
	le->leFlags = 0;
	le->leType = LE_FADE_RGB;
	le->startTime = cg.time;
	le->endTime = cg.time + 500;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	le->color[0] = le->color[1] = le->color[2] = le->color[3] = 1.0;
	
	re = &le->refEntity;

	//RPG-X: RedTechie - Playing with transporter crap
	re->shaderTime = cg.time * 0.001f;
	re = &le->refEntity;

	re->reType = RT_MODEL;
	re->shaderTime = cg.time * 0.001f;

	re->customShader = cgs.media.teleportEffectShader;
	re->hModel = cgs.media.teleportEffectModel;
	AxisClear( re->axis );

	VectorCopy( org, re->origin );
	re->origin[2] -= 24;
}

void CG_QFlashEvent( vec3_t org ) {
	localEntity_t	*le;

	le = FX_AddParticle( org, vec3_origin, qfalse, 110.0f, 109.0f, 
							1.0f, 1.0f, 0, 0, 
							290, cgs.media.qFlashSprite,  0 );
	le->leFlags = LEF_SINE_SCALE | LEF_REVERSE_SCALE;

	le->refEntity.renderfx |= RF_DEPTHHACK;
}


/*
====================
CG_MakeExplosion
====================
*/
localEntity_t *CG_MakeExplosion( vec3_t origin, vec3_t dir, 
								qhandle_t hModel, qhandle_t shader,
								int msec, float scale, qboolean isSprite ) {
	float			ang;
	localEntity_t	*ex;
	int				offset;
	vec3_t			tmpVec, newOrigin;

	if ( msec <= 0 ) {
		CG_Error( "CG_MakeExplosion: msec = %i", msec );
	}

	// skew the time a bit so they aren't all in sync
	offset = rand() & 63;

	ex = CG_AllocLocalEntity();
	if ( isSprite ) {
		ex->leType = LE_SPRITE_EXPLOSION;

		// randomly rotate sprite orientation
		ex->refEntity.data.sprite.rotation = rand() % 360;
		VectorScale( dir, 16, tmpVec );
		VectorAdd( tmpVec, origin, newOrigin );
	} else {
		ex->leType = LE_EXPLOSION;
		VectorCopy( origin, newOrigin );

		// set axis with random rotate
		if ( !dir ) {
			AxisClear( ex->refEntity.axis );
		} else {
			ang = rand() % 360;
			VectorCopy( dir, ex->refEntity.axis[0] );
			RotateAroundDirection( ex->refEntity.axis, ang );
		}
	}

	ex->startTime = cg.time - offset;
	ex->endTime = ex->startTime + msec;

	// bias the time so all shader effects start correctly
	ex->refEntity.shaderTime = ex->startTime * 0.001f;

	ex->refEntity.hModel = hModel;
	ex->refEntity.customShader = shader;

	// set origin
	VectorCopy( newOrigin, ex->refEntity.origin );
	VectorCopy( newOrigin, ex->refEntity.oldorigin );

	//Scale the explosion
	if (scale != 1) {
		ex->refEntity.nonNormalizedAxes = qtrue;

		VectorScale( ex->refEntity.axis[0], scale, ex->refEntity.axis[0] );
		VectorScale( ex->refEntity.axis[1], scale, ex->refEntity.axis[1] );
		VectorScale( ex->refEntity.axis[2], scale, ex->refEntity.axis[2] );
	}

	ex->color[0] = ex->color[1] = ex->color[2] = 1.0;

	return ex;
}


localEntity_t *CG_MakeExplosion2( vec3_t origin, vec3_t dir, 
								qhandle_t hModel, int numFrames, qhandle_t shader,
								int msec, qboolean isSprite, float scale, int flags) {
	float			ang;
	localEntity_t	*ex;
	int				offset;
	vec3_t			tmpVec, newOrigin;

	if ( msec <= 0 ) {
		CG_Error( "CG_MakeExplosion: msec = %i", msec );
	}

	// skew the time a bit so they aren't all in sync
	offset = rand() & 63;

	ex = CG_AllocLocalEntity();
	if ( isSprite ) {
		ex->leType = LE_SPRITE_EXPLOSION;

		// randomly rotate sprite orientation
		ex->refEntity.data.sprite.rotation = rand() % 360;
		VectorScale( dir, 16, tmpVec );
		VectorAdd( tmpVec, origin, newOrigin );
	} else {
		ex->leType = LE_EXPLOSION;
		VectorCopy( origin, newOrigin );

		// set axis with random rotate
		if ( !dir ) {
			AxisClear( ex->refEntity.axis );
		} else {
			ang = rand() % 360;
			VectorCopy( dir, ex->refEntity.axis[0] );
			RotateAroundDirection( ex->refEntity.axis, ang );
		}
	}

	ex->startTime = cg.time - offset;
	ex->endTime = ex->startTime + msec;

	// bias the time so all shader effects start correctly
	ex->refEntity.shaderTime = ex->startTime * 0.001f;

	ex->refEntity.hModel = hModel;
	ex->refEntity.customShader = shader;
	ex->lifeRate = (float)numFrames / msec;
	ex->leFlags = flags;

	//Scale the explosion
	if (scale != 1) {
		ex->refEntity.nonNormalizedAxes = qtrue;

		VectorScale( ex->refEntity.axis[0], scale, ex->refEntity.axis[0] );
		VectorScale( ex->refEntity.axis[1], scale, ex->refEntity.axis[1] );
		VectorScale( ex->refEntity.axis[2], scale, ex->refEntity.axis[2] );
	}

	// set origin
	VectorCopy( newOrigin, ex->refEntity.origin );
	VectorCopy( newOrigin, ex->refEntity.oldorigin );

	ex->color[0] = ex->color[1] = ex->color[2] = 1.0;

	return ex;
}


/*
-------------------------
CG_ExplosionEffects

Used to find the player and shake the camera if close enough
intensity ranges from 1 (minor tremble) to 16 (major quake)
-------------------------
*/

void CG_ExplosionEffects( vec3_t origin, int intensity, int radius)
{
	//FIXME: When exactly is the vieworg calculated in relation to the rest of the frame?s

	vec3_t	dir;
	float	dist, intensityScale;
	float	realIntensity;

	VectorSubtract( cg.refdef.vieworg, origin, dir );
	dist = VectorNormalize( dir );

	//Use the dir to add kick to the explosion

	if ( dist > radius )
		return;

	intensityScale = 1 - ( dist / (float) radius );
	realIntensity = intensity * intensityScale;

	CG_CameraShake( realIntensity, 500, qfalse );
}

/*
-------------------------
CG_Smoke
TiM: Ported from EF SP
-------------------------
*/

qboolean SmokeThink( localEntity_t *le )
{
	vec3_t	velocity/*, accel*/;
	vec3_t	origin;
	vec3_t	dir;
	float	speed;
	int i;

	VectorCopy( le->data.spawner.dir, dir );
	//clamp the smoke vector
	//Smoke should always go up
	dir[2] = Com_Clamp( 0.85f, 1.0f, dir[2] );

	for ( i = 0; i < 3; i++ )
	{
		velocity[i] = dir[i] + ( 0.2f * crandom());
	}

	VectorMA( le->refEntity.origin, 1, le->data.spawner.dir, origin);

	//slow down the smoke the smaller it gets
	//else it scatters too much
	speed = le->data.spawner.data1 * 2.4;

	VectorScale( velocity, speed, velocity ); //speed

	FX_AddSprite(	origin,
					velocity, 
					qfalse, //accel
					le->data.spawner.data1 + (crandom() * le->data.spawner.data1 * 0.5f ),  
					le->data.spawner.data1 + (crandom() * le->data.spawner.data1), 
					0.8, 
					0.0,
					16.0f + random() * 45.0f,
					0.5f,
					7000, 
					cgs.media.smokeShader ); //flags

	return qtrue;
}

/*
======================
CG_Smoke

Creates a smoke effect
======================
*/

void CG_Smoke( vec3_t position, vec3_t dir, int killTime, int radius )
{
	FX_AddSpawner( position, dir, NULL, NULL, qfalse, 0, 0.15, killTime, SmokeThink, radius ); //
}

/*
======================
FireThink

Engage fire effect
RPG-X | Marcin | 24/12/2008
======================
*/
qboolean FireThink( localEntity_t *le )
{
	vec3_t	direction;
	vec3_t	origin;

	VectorCopy( le->data.spawner.dir, direction );
	VectorMA( le->refEntity.origin, 1, direction, origin );

	origin[2] += 60.0f / (80.0f / le->data.spawner.data1); // extra offset

	FX_AddSprite(	origin,
					0, 
					qfalse, 
					le->data.spawner.data1, 
					0, 
					1.0f, 
					0.0f,
					0,
					0.5f,
					600,
					cgs.media.fireShader );

	return qtrue;
}

/*
======================
CG_Fire

Creates a fire effect
RPG-X | Marcin | 24/12/2008
======================
*/

void CG_Fire( vec3_t position, vec3_t direction, int killTime, int radius, int fxEnt )
{
	if(fxEnt)
		FX_AddSpawner( position, direction, NULL, NULL, qfalse, 500, 0, killTime + 1000, FireThink, radius );
	else
		FX_AddSpawner( position, direction, NULL, NULL, qfalse, 500, 0, killTime, FireThink, radius );
}

