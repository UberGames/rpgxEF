// Copyright (C) 1999-2000 Id Software, Inc.
//

// cg_localents.c -- every frame, generate renderer commands for locally
// processed entities, like smoke puffs, gibs, shells, etc.

#include "cg_local.h"

#define	MAX_LOCAL_ENTITIES	512
localEntity_t	cg_localEntities[MAX_LOCAL_ENTITIES];
localEntity_t	cg_activeLocalEntities;		// double linked list
localEntity_t	*cg_freeLocalEntities;		// single linked list

/*
===================
CG_InitLocalEntities

This is called at startup and for tournement restarts
===================
*/
void	CG_InitLocalEntities( void ) {
	int		i;

	memset( cg_localEntities, 0, sizeof( cg_localEntities ) );
	cg_activeLocalEntities.next = &cg_activeLocalEntities;
	cg_activeLocalEntities.prev = &cg_activeLocalEntities;
	cg_freeLocalEntities = cg_localEntities;
	for ( i = 0 ; i < MAX_LOCAL_ENTITIES - 1 ; i++ ) {
		cg_localEntities[i].next = &cg_localEntities[i+1];
	}
}


/*
==================
CG_FreeLocalEntity
==================
*/
void CG_FreeLocalEntity( localEntity_t *le ) {
	if ( !le->prev ) {
		CG_Error( "CG_FreeLocalEntity: not active" );
		return;
	}

	// remove from the doubly linked active list
	le->prev->next = le->next;
	le->next->prev = le->prev;

	// the free list is only singly linked
	le->next = cg_freeLocalEntities;
	cg_freeLocalEntities = le;
}

/*
===================
CG_AllocLocalEntity

Will allways succeed, even if it requires freeing an old active entity
===================
*/
localEntity_t	*CG_AllocLocalEntity( void ) {
	localEntity_t	*le;

	if ( !cg_freeLocalEntities ) {
		// no free entities, so free the one at the end of the chain
		// remove the oldest active entity
		CG_FreeLocalEntity( cg_activeLocalEntities.prev );
	}

	le = cg_freeLocalEntities;
	cg_freeLocalEntities = cg_freeLocalEntities->next;

	memset( le, 0, sizeof( *le ) );

	// link into the active list
	le->next = cg_activeLocalEntities.next;
	le->prev = &cg_activeLocalEntities;
	cg_activeLocalEntities.next->prev = le;
	cg_activeLocalEntities.next = le;
	return le;
}

/*
==================
CG_GetActiveList
Accessor fn
==================
*/
localEntity_t	CG_GetActiveList( void )
{
	return cg_activeLocalEntities;
}

/*
====================================================================================

FRAGMENT PROCESSING

A fragment localentity interacts with the environment in some way (hitting walls),
or generates more localentities along a trail.

====================================================================================
*/


/*
================
CG_ReflectVelocity
================
*/
void CG_ReflectVelocity( localEntity_t *le, trace_t *trace ) {
	vec3_t	velocity;
	float	dot;
	int		hitTime;

	// reflect the velocity on the trace plane
	hitTime = cg.time - cg.frametime + cg.frametime * trace->fraction;
	BG_EvaluateTrajectoryDelta( &le->pos, hitTime, velocity );
	dot = DotProduct( velocity, trace->plane.normal );
	VectorMA( velocity, -2*dot, trace->plane.normal, le->pos.trDelta );

	VectorScale( le->pos.trDelta, le->bounceFactor, le->pos.trDelta );

	VectorCopy( trace->endpos, le->pos.trBase );
	le->pos.trTime = cg.time;


	// check for stop, making sure that even on low FPS systems it doesn't bobble
	if ( trace->allsolid || 
		( trace->plane.normal[2] > 0 && 
		( le->pos.trDelta[2] < 40 || le->pos.trDelta[2] < -cg.frametime * le->pos.trDelta[2] ) ) ) {
		le->pos.trType = TR_STATIONARY;
	} else {

	}
}

/*
=====================================================================

TRIVIAL LOCAL ENTITIES

These only do simple scaling or modulation before passing to the renderer
=====================================================================
*/

/*
====================
CG_AddFadeRGB
====================
*/
void CG_AddFadeRGB( localEntity_t *le ) {
	refEntity_t *re;
	float c;

	re = &le->refEntity;

	c = ( le->endTime - cg.time ) * le->lifeRate;
	c *= 0xff;

	re->shaderRGBA[0] = le->color[0] * c;
	re->shaderRGBA[1] = le->color[1] * c;
	re->shaderRGBA[2] = le->color[2] * c;
	re->shaderRGBA[3] = le->color[3] * c;

	trap_R_AddRefEntityToScene( re );
}

/*
==================
CG_AddMoveScaleFade
==================
*/
void CG_AddMoveScaleFade( localEntity_t *le )
{
	refEntity_t	*re;
	float		c;
	vec3_t		delta;
	float		len;

	re = &le->refEntity;

	// fade / grow time
	c = ( le->endTime - cg.time ) * le->lifeRate;

	re->shaderRGBA[3] = 0xff * c * le->color[3];

	if ( !( le->leFlags & LEF_PUFF_DONT_SCALE ) ) {
		re->data.sprite.radius = le->data.sprite.radius * ( 1.0 - c ) + 8;
	}

	BG_EvaluateTrajectory( &le->pos, cg.time, re->origin );

	// if the view would be "inside" the sprite, kill the sprite
	// so it doesn't add too much overdraw
	VectorSubtract( re->origin, cg.refdef.vieworg, delta );
	len = VectorLength( delta );
	if ( len < le->data.sprite.radius ) {
		CG_FreeLocalEntity( le );
		return;
	}

	trap_R_AddRefEntityToScene( re );
}


/*
===================
CG_AddScaleFade

For rocket smokes that hang in place, fade out, and are
removed if the view passes through them.
There are often many of these, so it needs to be simple.
===================
*/
void CG_AddScaleFade( localEntity_t *le )
{
	refEntity_t	*re;
	float		c;
	vec3_t		delta;
	float		len;

	re = &le->refEntity;

	// fade / grow time
	c = ( le->endTime - cg.time ) * le->lifeRate;

	re->shaderRGBA[3] = 0xff * c * le->color[3];
	re->data.sprite.radius = le->data.sprite.radius * ( 1.0 - c ) + 8;

	// if the view would be "inside" the sprite, kill the sprite
	// so it doesn't add too much overdraw
	VectorSubtract( re->origin, cg.refdef.vieworg, delta );
	len = VectorLength( delta );
	if ( len < le->data.sprite.radius ) {
		CG_FreeLocalEntity( le );
		return;
	}

	trap_R_AddRefEntityToScene( re );
}


/*
=================
CG_AddFallScaleFade

This is just an optimized CG_AddMoveScaleFade
For blood mists that drift down, fade out, and are
removed if the view passes through them.
There are often 100+ of these, so it needs to be simple.
=================
*/
void CG_AddFallScaleFade( localEntity_t *le )
{
	refEntity_t	*re;
	float		c;
	vec3_t		delta;
	float		len;

	re = &le->refEntity;

	// fade time
	c = ( le->endTime - cg.time ) * le->lifeRate;

	re->shaderRGBA[3] = 0xff * c * le->color[3];

	re->origin[2] = le->pos.trBase[2] - ( 1.0 - c ) * le->pos.trDelta[2];

	re->data.sprite.radius = le->data.sprite.radius * ( 1.0 - c ) + 16;

	// if the view would be "inside" the sprite, kill the sprite
	// so it doesn't add too much overdraw
	VectorSubtract( re->origin, cg.refdef.vieworg, delta );
	len = VectorLength( delta );
	if ( len < le->data.sprite.radius ) {
		CG_FreeLocalEntity( le );
		return;
	}

	trap_R_AddRefEntityToScene( re );
}



/*
================
CG_AddExplosion
================
*/
void CG_AddExplosion( localEntity_t *ex )
{
	refEntity_t	*ent;

	ent = &ex->refEntity;

	// calculate model frame
	if ( ex->lifeRate > 0 ) {
		float frac = (cg.time - ex->startTime) * ex->lifeRate;
		int f = floor(frac);
		if ( f < 0 ) {
			f = 0;
		}

		ent->frame = f + 1;
		ent->oldframe = f;
		ent->backlerp = 1.0 - ( frac - f );
		ent->renderfx |= RF_CAP_FRAMES;
	}
	// Explosions with zero shaders (using model default shader) don't fade, so
	//		allow fading when this flag is set.
	if ( ex->leFlags & LEF_FADE_RGB )
	{
		float frac = (float)( cg.time - ex->startTime )/(float)( ex->endTime - ex->startTime );

		ent->shaderRGBA[0] = 
		ent->shaderRGBA[1] = 
		ent->shaderRGBA[2] = frac * 255;
		ent->shaderRGBA[3] = 255;
	}

	// add the entity
	trap_R_AddRefEntityToScene(ent);

	// add the dlight
	if ( ex->light ) {
		float		light;

		light = (float)( cg.time - ex->startTime ) / ( ex->endTime - ex->startTime );
		if ( light < 0.5 ) {
			light = 1.0;
		} else {
			light = 1.0 - ( light - 0.5 ) * 2;
		}
		light = ex->light * light;
		trap_R_AddLightToScene(ent->origin, light, ex->lightColor[0], ex->lightColor[1], ex->lightColor[2] );
	}
}

/*
================
CG_AddSpriteExplosion
================
*/
void CG_AddSpriteExplosion( localEntity_t *le )
{
	refEntity_t	re;
	float c;

	re = le->refEntity;

	c = ( le->endTime - cg.time ) / ( float ) ( le->endTime - le->startTime );
	if ( c > 1 ) {
		c = 1.0;	// can happen during connection problems
	}

	re.shaderRGBA[0] = 0xff;
	re.shaderRGBA[1] = 0xff;
	re.shaderRGBA[2] = 0xff;
	re.shaderRGBA[3] = 0xff * c * 0.33;

	re.reType = RT_SPRITE;
	re.data.sprite.radius = 42 * ( 1.0 - c ) + 30;

	trap_R_AddRefEntityToScene( &re );

	// add the dlight
	if ( le->light ) {
		float		light;

		light = (float)( cg.time - le->startTime ) / ( le->endTime - le->startTime );
		if ( light < 0.5 ) {
			light = 1.0;
		} else {
			light = 1.0 - ( light - 0.5 ) * 2;
		}
		light = le->light * light;
		trap_R_AddLightToScene(re.origin, light, le->lightColor[0], le->lightColor[1], le->lightColor[2] );
	}
}




/*
===================
CG_AddScaleFadeSprite

For trek, oriented sprites like blast rings and the like.
===================
*/
void CG_AddScaleFadeSprite( localEntity_t *le )
{
	refEntity_t	*re;
	float		c;
	vec3_t		delta;
	float		len;

	re = &le->refEntity;

	c = ( le->endTime - cg.time ) / ( float ) ( le->endTime - le->startTime );
	if ( c > 1 ) {
		c = 1.0;	// can happen during connection problems
	}

	// Use the liferate to set the scale over time.
	re->data.sprite.radius = le->data.sprite.radius + (le->lifeRate * (cg.time - le->startTime));
	if (re->data.sprite.radius <= 0)
	{
		CG_FreeLocalEntity( le );
		return;
	}

	// if the view would be "inside" the sprite, kill the sprite
	// so it doesn't add too much overdraw
	VectorSubtract( re->origin, cg.refdef.vieworg, delta );
	len = VectorLength( delta );
	if ( len < le->data.sprite.radius ) {
		CG_FreeLocalEntity( le );
		return;
	}

	// We will assume here that we want additive transparency effects.
	re->shaderRGBA[0] = 0xff * c;
	re->shaderRGBA[1] = 0xff * c;
	re->shaderRGBA[2] = 0xff * c;
	re->shaderRGBA[3] = 0xff * le->color[3];

	re->reType = RT_ORIENTEDSPRITE;

	trap_R_AddRefEntityToScene( re );
}

/*
===================
CG_AddQuad

Of Trek, by Trek, and for Trek
===================
*/
void CG_AddQuad( localEntity_t *le )
{
	refEntity_t	*re;
	float		frac, alpha;
	vec3_t		delta;
	float		len;
	vec3_t		curRGB;

	re = &le->refEntity;

	frac = ( cg.time - le->startTime ) / ( float ) ( le->endTime - le->startTime );
	if ( frac > 1 )
		frac = 1.0;	// can happen during connection problems
	else if (frac < 0)
		frac = 0.0;

	// Use the liferate to set the scale over time.
	re->data.sprite.radius = le->data.sprite.radius + (le->data.sprite.dradius*frac);
	if (re->data.sprite.radius <= 0)
	{
		CG_FreeLocalEntity( le );
		return;
	}

	// if the view would be "inside" the sprite, kill the sprite
	// so it doesn't add too much overdraw
	VectorSubtract( re->origin, cg.refdef.vieworg, delta );
	len = VectorLength( delta );
	if ( len < le->data.sprite.radius ) {
		CG_FreeLocalEntity( le );
		return;
	}

	// Calculate the current alpha.
	alpha = le->alpha + (le->dalpha * frac);
	VectorMA(le->data.sprite.startRGB, frac, le->data.sprite.dRGB, curRGB);
	re->shaderRGBA[0] = 0xff * alpha * curRGB[0];
	re->shaderRGBA[1] = 0xff * alpha * curRGB[1];
	re->shaderRGBA[2] = 0xff * alpha * curRGB[2];
	re->shaderRGBA[3] = 0xff;	

	re->reType = RT_ORIENTEDSPRITE;

	trap_R_AddRefEntityToScene( re );
}


/*
===================
CG_AddLine

For trek, for beams and the like.
===================
*/
void CG_AddLine( localEntity_t *le )
{
	refEntity_t	*re;
	float		frac, alpha;

	re = &le->refEntity;

	frac = (cg.time - le->startTime) / ( float ) ( le->endTime - le->startTime );
	if ( frac > 1 ) 
		frac = 1.0;	// can happen during connection problems
	else if (frac < 0)
		frac = 0.0;

	// Use the liferate to set the scale over time.
	re->data.line.width = le->data.line.width + (le->data.line.dwidth * frac);
	if (re->data.line.width <= 0)
	{
		CG_FreeLocalEntity( le );
		return;
	}

	// We will assume here that we want additive transparency effects.
	alpha = le->alpha + (le->dalpha * frac);
	re->shaderRGBA[0] = 0xff * alpha;
	re->shaderRGBA[1] = 0xff * alpha;
	re->shaderRGBA[2] = 0xff * alpha;
	re->shaderRGBA[3] = 0xff * alpha;	// Yes, we could apply c to this too, but fading the color is better for lines.

	re->reType = RT_LINE;

	trap_R_AddRefEntityToScene( re );
}


/*
===================
CG_AddOLine

For trek, for rectangles.
===================
*/
void CG_AddOLine( localEntity_t *le )
{
	refEntity_t	*re;
	float		frac, alpha;

	re = &le->refEntity;

	frac = (cg.time - le->startTime) / ( float ) ( le->endTime - le->startTime );
	if ( frac > 1 ) 
		frac = 1.0;	// can happen during connection problems
	else if (frac < 0)
		frac = 0.0;

	// Use the liferate to set the scale over time.
	re->data.line.width = le->data.line.width + (le->data.line.dwidth * frac);
	if (re->data.line.width <= 0)
	{
		CG_FreeLocalEntity( le );
		return;
	}

	// We will assume here that we want additive transparency effects.
	alpha = le->alpha + (le->dalpha * frac);
	re->shaderRGBA[0] = 0xff * alpha;
	re->shaderRGBA[1] = 0xff * alpha;
	re->shaderRGBA[2] = 0xff * alpha;
	re->shaderRGBA[3] = 0xff * alpha;	// Yes, we could apply c to this too, but fading the color is better for lines.

	re->reType = RT_ORIENTEDLINE;

	trap_R_AddRefEntityToScene( re );
}

/*
===================
CG_AddLine2

For trek, for beams and the like.
===================
*/
void CG_AddLine2( localEntity_t *le )
{
	refEntity_t	*re;
	float		frac, alpha;
	vec3_t		curRGB;

	re = &le->refEntity;

	frac = (cg.time - le->startTime) / ( float ) ( le->endTime - le->startTime );
	if ( frac > 1 ) 
		frac = 1.0;	// can happen during connection problems
	else if (frac < 0)
		frac = 0.0;

	// Use the liferate to set the scale over time.
	re->data.line.width = le->data.line2.width + (le->data.line2.dwidth * frac);
	re->data.line.width2 = le->data.line2.width2 + (le->data.line2.dwidth2 * frac);
	if (re->data.line.width <= 0)
	{
		CG_FreeLocalEntity( le );
		return;
	}

	// We will assume here that we want additive transparency effects.
	alpha = le->alpha + (le->dalpha * frac);
	VectorMA(le->data.line2.startRGB, frac, le->data.line2.dRGB, curRGB);
	re->shaderRGBA[0] = 0xff * alpha * curRGB[0];
	re->shaderRGBA[1] = 0xff * alpha * curRGB[1];
	re->shaderRGBA[2] = 0xff * alpha * curRGB[2];
	re->shaderRGBA[3] = 0xff * alpha;	// Yes, we could apply c to this too, but fading the color is better for lines.

	re->reType = RT_LINE2;

	trap_R_AddRefEntityToScene( re );
}



/*
===================
CG_AddTrail

For trek, for sparks and the like.
===================
*/
void CG_AddTrail( localEntity_t *le ) 
{
	refEntity_t	*re;
	float frac, length, alpha;
	vec3_t	dir;
	trace_t	trace;
	vec3_t	curRGB;

	re = &le->refEntity;

	frac = (cg.time - le->startTime) / ( float ) ( le->endTime - le->startTime );
	if ( frac > 1 ) 
		frac = 1.0;	// can happen during connection problems
	else if (frac < 0)
		frac = 0.0;

	// Use the liferate to set the scale over time.
	re->data.line.width = le->data.trail.width + (le->data.trail.dwidth * frac);
	if (re->data.line.width <= 0)
	{
		CG_FreeLocalEntity( le );
		return;
	}

	if (!(le->leFlags & LEF_MOVE))
	{
		return;
	}

	// kef -- do these two lines _before_ copying origin into oldorigin
	VectorSubtract(re->oldorigin, re->origin, dir);
	VectorNormalize(dir);

	VectorCopy(re->origin, re->oldorigin);
	BG_EvaluateTrajectory( &le->pos, cg.time, re->origin );

	if (le->leFlags & LEF_USE_COLLISION)
	{
		// trace a line from previous position to new position
		CG_Trace( &trace, re->oldorigin, NULL, NULL, re->origin, -1, CONTENTS_SOLID );

		if ( trace.fraction != 1.0 ) 
		{	// Hit something.
			// if it is in a nodrop zone, remove it
			// this keeps gibs from waiting at the bottom of pits of death
			// and floating levels
			if ( trap_CM_PointContents( trace.endpos, 0 ) & CONTENTS_NODROP ) {
				CG_FreeLocalEntity( le );
				return;
			}

			// reflect the velocity on the trace plane
			CG_ReflectVelocity( le, &trace );
		}
		VectorSubtract(re->oldorigin, re->origin, dir);
		VectorNormalize(dir);
	}

	// Set the length based on the velocity of the bit.
	length = le->data.trail.length + (le->data.trail.dlength * frac);
	if (length <= 0)
	{
		CG_FreeLocalEntity( le );
		return;
	}

	VectorMA(re->origin, length, dir, re->oldorigin);

	// We will assume here that we want additive transparency effects.
	alpha = le->alpha + (le->dalpha * frac);
	VectorMA(le->data.trail.startRGB, frac, le->data.trail.dRGB, curRGB);
	re->shaderRGBA[0] = 0xff * alpha * curRGB[0];
	re->shaderRGBA[1] = 0xff * alpha * curRGB[1];
	re->shaderRGBA[2] = 0xff * alpha * curRGB[2];
	re->shaderRGBA[3] = 0xff;	// Yes, we could apply c to this too, but fading the color is better for lines.

	re->reType = RT_LINE;

	trap_R_AddRefEntityToScene( re );
}



/*
===================
CG_AddViewSprite

For trek, view sprites like smoke and the like.
===================
*/
void CG_AddViewSprite( localEntity_t *le )
{
	refEntity_t	*re;
	float		frac, alpha;
	vec3_t		delta;
	float		len;
	trace_t		trace;
	vec3_t		curRGB;

	re = &le->refEntity;

	frac = ( cg.time - le->startTime ) / ( float ) ( le->endTime - le->startTime );
	if ( frac > 1 )
		frac = 1.0;	// can happen during connection problems
	else if (frac < 0)
		frac = 0.0;

	// Use the liferate to set the scale over time.
	re->data.sprite.radius = le->data.sprite.radius + (le->data.sprite.dradius*frac);
	if (re->data.sprite.radius <= 0)
	{
		CG_FreeLocalEntity( le );
		return;
	}

	if (le->leFlags & LEF_MOVE)
	{
		VectorCopy(re->origin, re->oldorigin);
		BG_EvaluateTrajectory( &le->pos, cg.time, re->origin );

		if (le->leFlags & LEF_USE_COLLISION)
		{
			// trace a line from previous position to new position
			CG_Trace( &trace, re->oldorigin, NULL, NULL, re->origin, -1, CONTENTS_SOLID );

			if ( trace.fraction != 1.0 ) 
			{	// Hit something.
				// if it is in a nodrop zone, remove it
				// this keeps gibs from waiting at the bottom of pits of death
				// and floating levels
				if ( trap_CM_PointContents( trace.endpos, 0 ) & CONTENTS_NODROP ) {
					CG_FreeLocalEntity( le );
					return;
				}

				// reflect the velocity on the trace plane
				CG_ReflectVelocity( le, &trace );
			}
		}
	}

	// if the view would be "inside" the sprite, kill the sprite
	// so it doesn't add too much overdraw
	VectorSubtract( re->origin, cg.refdef.vieworg, delta );
	len = VectorLength( delta );
	if ( len < le->data.sprite.radius ) {
		CG_FreeLocalEntity( le );
		return;
	}

	// Calculate the current alpha.
	alpha = le->alpha + (le->dalpha * frac);
	VectorMA(le->data.sprite.startRGB, frac, le->data.sprite.dRGB, curRGB);
	re->shaderRGBA[0] = 0xff * alpha * curRGB[0];
	re->shaderRGBA[1] = 0xff * alpha * curRGB[1];
	re->shaderRGBA[2] = 0xff * alpha * curRGB[2];
	re->shaderRGBA[3] = 0xff;	

	re->reType = RT_SPRITE;

	trap_R_AddRefEntityToScene( re );
}


/*
===================
CG_AddBezier

For trek, for the imod and the...uh...imod.

===================
*/
void CG_AddBezier( localEntity_t *le )
{
	refEntity_t	*re;
	float		frac, alpha;
	float		t = (cg.time - le->startTime)*0.001; // time elapsed since beginning of effect
	vec3_t		vTempPos;

	re = &le->refEntity;

	frac = (cg.time - le->startTime) / ( float ) ( le->endTime - le->startTime );
	if ( frac > 1 ) 
		frac = 1.0;	// can happen during connection problems
	else if (frac < 0)
		frac = 0.0;

	// Use the liferate to set the scale over time.
	re->data.bezier.width = le->data.line.width + (le->data.line.dwidth * frac);
	if (re->data.bezier.width <= 0)
	{
		CG_FreeLocalEntity( le );
		return;
	}

	// We will assume here that we want additive transparency effects.
	alpha = le->alpha + (le->dalpha * frac);
	re->shaderRGBA[0] = 0xff * alpha;
	re->shaderRGBA[1] = 0xff * alpha;
	re->shaderRGBA[2] = 0xff * alpha;
	re->shaderRGBA[3] = 0xff;	// Yes, we could apply c to this too, but fading the color is better for lines.

	re->reType = RT_BEZIER;

	// the refEntity only stores the two control points, so we need to update them here with 
	//the control_velocity and control_acceleration, then store the results in refEntity. 
	//use (cg.time - le->startTime) as a value for elapsed time t, plug it into the position formula:
	//
	// x = x0 + (v0 * t) + (0.5 * a * t * t)
	//
	//...where x is the position at time t, x0 is initial control point position, v0 is control point velocity,
	//and a is control point acceleration
	//

	// update control point 1
	VectorMA(le->data.line.control1, t, le->data.line.control1_velocity, vTempPos);
	VectorMA(vTempPos, (0.5*t*t), le->data.line.control1_acceleration, re->data.bezier.control1);

	// update control point 2
	VectorMA(le->data.line.control2, t, le->data.line.control2_velocity, vTempPos);
	VectorMA(vTempPos, (0.5*t*t), le->data.line.control2_acceleration, re->data.bezier.control2);

	trap_R_AddRefEntityToScene( re );
}



/*
===================
CG_AddCylinder

For trek, cylinder primitive.
===================
*/
void CG_AddCylinder( localEntity_t *le )
{
	refEntity_t	*re;
	float		frac, alpha;

	re = &le->refEntity;

	frac = ( cg.time - le->startTime ) / ( float ) ( le->endTime - le->startTime );
	if ( frac > 1 )
		frac = 1.0;	// can happen during connection problems
	else if (frac < 0)
		frac = 0.0;

	// Use the liferate to set the scale over time.
	re->data.cylinder.height = le->data.cylinder.height + (le->data.cylinder.dheight*frac);
	if (re->data.cylinder.height <= 0)
	{
		CG_FreeLocalEntity( le );
		return;
	}
	re->data.cylinder.width = le->data.cylinder.width + (le->data.cylinder.dwidth*frac);
	if (re->data.cylinder.width <= 0)
	{
		CG_FreeLocalEntity( le );
		return;
	}
	re->data.cylinder.width2 = le->data.cylinder.width2 + (le->data.cylinder.dwidth2*frac);
	if (re->data.cylinder.width2 <= 0)
	{
		CG_FreeLocalEntity( le );
		return;
	}

	// Calculate the current alpha.
	alpha = le->alpha + (le->dalpha * frac);
	re->shaderRGBA[0] = 0xff * alpha;
	re->shaderRGBA[1] = 0xff * alpha;
	re->shaderRGBA[2] = 0xff * alpha;
	re->shaderRGBA[3] = 0xff;	

	re->reType = RT_CYLINDER;

	trap_R_AddRefEntityToScene( re );
}



/*
===================
CG_AddElectricity

For trek, electricity primitive.
===================
*/

#define DEFAULT_DEVIATION	0.5

void CG_AddElectricity( localEntity_t *le ) {
	refEntity_t	*re;
	float		frac, alpha;

	re = &le->refEntity;

	frac = ( cg.time - le->startTime ) / ( float ) ( le->endTime - le->startTime );
	if ( frac > 1 )
		frac = 1.0;	// can happen during connection problems
	else if (frac < 0)
		frac = 0.0;

	re->data.electricity.width = le->data.electricity.width + (le->data.electricity.dwidth*frac);
	
	// Calculate the current alpha.
	alpha = le->alpha + (le->dalpha * frac);
	re->shaderRGBA[0] = 0xff * alpha;
	re->shaderRGBA[1] = 0xff * alpha;
	re->shaderRGBA[2] = 0xff * alpha;
	re->shaderRGBA[3] = 0xff;	

	re->reType = RT_ELECTRICITY;

	trap_R_AddRefEntityToScene( re );
}

/*
===================
CG_AddParticle

For trek, special explosion stuff sometimes wants these
===================
*/
static void CG_AddParticle( localEntity_t *le ) 
{
	refEntity_t	*re;
	float		frac, alpha;
	vec3_t		delta, dir;
	float		len;
	trace_t		trace;

	re = &le->refEntity;

	//safety check - since this renders over all, make sure we can't see this thru a wall
	if ( re->renderfx & RF_DEPTHHACK ) {
		CG_Trace( &trace, re->origin, NULL, NULL, cg.refdef.vieworg, cg.predictedPlayerState.clientNum, MASK_SOLID );

		if ( trace.fraction != 1.0 )
			return;

	}

	frac = ( cg.time - le->startTime ) / ( float ) ( le->endTime - le->startTime );
	if ( le->leFlags & LEF_SINE_SCALE ) {
		//frac = 1.0-(0.5f * sin( 4.0f * frac + 0.75f ) + 0.5f); //TiM: Sine calc //+ 1.5f
		frac = 1.0-(0.65 * sin(  3.0 * frac +0.75 ) + 0.35);
	}

	if ( frac > 1 )
		frac = 1.0;	// can happen during connection problems
	else if (frac < 0)
		frac = 0.0;

	//CG_Printf( "%f\n", frac );

	// Use the liferate to set the scale over time.
	if ( !(le->leFlags & LEF_REVERSE_SCALE) ) 
		re->data.sprite.radius = le->data.particle.radius + (le->data.particle.dradius*frac);
	else
		re->data.sprite.radius = le->data.particle.radius - (le->data.particle.dradius*frac);
	if (re->data.sprite.radius <= 0)
	{
		CG_FreeLocalEntity( le );
		return;
	}

	if (le->leFlags & LEF_MOVE)
	{
		// kef -- do these two lines _before_ copying origin into oldorigin
		VectorSubtract(re->oldorigin, re->origin, dir);
		VectorNormalize(dir);

		VectorCopy(re->origin, re->oldorigin);
		BG_EvaluateTrajectory( &le->pos, cg.time, re->origin );

		if (le->leFlags & LEF_USE_COLLISION)
		{
			// trace a line from previous position to new position
			CG_Trace( &trace, re->oldorigin, NULL, NULL, re->origin, -1, CONTENTS_SOLID );

			if ( trace.fraction != 1.0 ) 
			{	// Hit something.
				// if it is in a nodrop zone, remove it
				// this keeps gibs from waiting at the bottom of pits of death
				// and floating levels
				if ( trap_CM_PointContents( trace.endpos, 0 ) & CONTENTS_NODROP ) {
					CG_FreeLocalEntity( le );
					return;
				}

				// reflect the velocity on the trace plane
				CG_ReflectVelocity( le, &trace );
			}
			VectorSubtract(re->oldorigin, re->origin, dir);
			VectorNormalize(dir);
		}
	}

	// if the view would be "inside" the sprite, kill the sprite
	// so it doesn't add too much overdraw
	VectorSubtract( re->origin, cg.refdef.vieworg, delta );
	len = VectorLength( delta );
	if ( len < le->data.particle.radius ) {
		CG_FreeLocalEntity( le );
		return;
	}

	// kef -- here's where I, in my infinite wisdom, have decided to emulate the singleplayer
	//particle think function
	VectorNegate(dir, le->data.particle.dir);
	if (le->data.particle.thinkFn)
	{
		le->data.particle.thinkFn(le);
	}

	// Calculate the current alpha.
	alpha = le->alpha + (le->dalpha * frac);
	re->shaderRGBA[0] = 0xff * alpha;
	re->shaderRGBA[1] = 0xff * alpha;
	re->shaderRGBA[2] = 0xff * alpha;
	re->shaderRGBA[3] = 0xff;	

	re->reType = RT_SPRITE;

	trap_R_AddRefEntityToScene( re );
}

static void CG_AddSpawner( localEntity_t *le ) 
{
	refEntity_t	*re;
	vec3_t		dir;
	trace_t		trace;

	re = &le->refEntity;
	if (le->leFlags & LEF_MOVE)
	{
		// kef -- do these two lines _before_ copying origin into oldorigin
		VectorSubtract(re->oldorigin, re->origin, dir);
		VectorNormalize(dir);

		VectorCopy(re->origin, re->oldorigin);
		BG_EvaluateTrajectory( &le->pos, cg.time, re->origin );

		if (le->leFlags & LEF_USE_COLLISION)
		{
			// trace a line from previous position to new position
			CG_Trace( &trace, re->oldorigin, NULL, NULL, re->origin, -1, CONTENTS_SOLID );

			if ( trace.fraction != 1.0 ) 
			{	// Hit something.
				// if it is in a nodrop zone, remove it
				// this keeps gibs from waiting at the bottom of pits of death
				// and floating levels
				if ( trap_CM_PointContents( trace.endpos, 0 ) & CONTENTS_NODROP ) {
					CG_FreeLocalEntity( le );
					return;
				}

				// reflect the velocity on the trace plane
				CG_ReflectVelocity( le, &trace );
			}
			VectorSubtract(re->oldorigin, re->origin, dir);
			VectorNormalize(dir);
		}
	}

	// kef -- here's where I, in my infinite wisdom, have decided to emulate the singleplayer
	//particle think function
	if (cg.time < le->data.spawner.nextthink)
	{
		return;
	}
	le->data.spawner.nextthink = cg.time + (le->data.spawner.delay + 
		(le->data.spawner.delay*flrandom(-le->data.spawner.variance,le->data.spawner.variance)));

	if (le->data.spawner.thinkFn)
	{
		le->data.spawner.thinkFn(le);
	}
	if (le->data.spawner.dontDie)
	{
		le->endTime = le->endTime + 10000;
	}
}


/*
================
CG_AddFragment
================
*/
void CG_AddFragment( localEntity_t *le ) {
	vec3_t	newOrigin;
	trace_t	trace;
	int k;

	if ( le->pos.trType == TR_STATIONARY ) {
		// sink into the ground if near the removal time
		int		t;
		float	oldZ;
		
		t = le->endTime - cg.time;
		if ( t < SINK_TIME ) {
			// we must use an explicit lighting origin, otherwise the
			// lighting would be lost as soon as the origin went
			// into the ground
			VectorCopy( le->refEntity.origin, le->refEntity.lightingOrigin );
			le->refEntity.renderfx |= RF_LIGHTING_ORIGIN;
			oldZ = le->refEntity.origin[2];
			le->refEntity.origin[2] -= 16 * ( 1.0 - (float)t / SINK_TIME );
			trap_R_AddRefEntityToScene( &le->refEntity );
			le->refEntity.origin[2] = oldZ;
		} else {
			trap_R_AddRefEntityToScene( &le->refEntity );
		}

		return;
	}

	// calculate new position
	BG_EvaluateTrajectory( &le->pos, cg.time, newOrigin );

	// trace a line from previous position to new position
	CG_Trace( &trace, le->refEntity.origin, NULL, NULL, newOrigin, 0 /*le->ownerGentNum*/, CONTENTS_SOLID );
	if ( trace.fraction == 1.0 ) {
		// still in free fall
		VectorCopy( newOrigin, le->refEntity.origin );

		if ( le->leFlags & LEF_TUMBLE ) {
			vec3_t angles;

			BG_EvaluateTrajectory( &le->angles, cg.time, angles );
			AnglesToAxis( angles, le->refEntity.axis );
			for(k = 0; k < 3; k++)
			{
				VectorScale(le->refEntity.axis[k], le->data.fragment.radius, le->refEntity.axis[k]);
			}

		}

		trap_R_AddRefEntityToScene( &le->refEntity );

		return;
	}

	// if it is in a nodrop zone, remove it
	// this keeps gibs from waiting at the bottom of pits of death
	// and floating levels
	if ( trap_CM_PointContents( trace.endpos, 0 ) & CONTENTS_NODROP ) {
		CG_FreeLocalEntity( le );
		return;
	}

	// reflect the velocity on the trace plane
	CG_ReflectVelocity( le, &trace );
	//FIXME: if LEF_TUMBLE, change avelocity too?

	trap_R_AddRefEntityToScene( &le->refEntity );
}


//==============================================================================

/*
===================
CG_AddLocalEntities

===================
*/
void CG_AddLocalEntities( void ) {
	localEntity_t	*le, *next;

	// walk the list backwards, so any new local entities generated
	// (trails, marks, etc) will be present this frame
	le = cg_activeLocalEntities.prev;
	for ( ; le != &cg_activeLocalEntities ; le = next ) {
		// grab next now, so if the local entity is freed we
		// still have it
		next = le->prev;

		if ( cg.time >= le->endTime ) {
			CG_FreeLocalEntity( le );
			continue;
		}

		if (le->leFlags & LEF_ONE_FRAME)
		{	// If this flag is set, only render one single frame, no more.
			if (le->leFlags & LEF_ONE_FRAME_DONE) 
			{
				CG_FreeLocalEntity( le );
				continue;
			}
			else
			{
				le->leFlags |= LEF_ONE_FRAME_DONE;
			}
		}

		switch ( le->leType ) {
		default:
			CG_Error( "Bad leType: %i", le->leType );
			break;

		case LE_MARK:
			break;

		case LE_SPRITE_EXPLOSION:
			CG_AddSpriteExplosion( le );
			break;

		case LE_EXPLOSION:
			CG_AddExplosion( le );
			break;

		case LE_MOVE_SCALE_FADE:		// water bubbles
			CG_AddMoveScaleFade( le );
			break;

		case LE_FADE_RGB:				// teleporters, railtrails
			CG_AddFadeRGB( le );
			break;

		case LE_FALL_SCALE_FADE: // gib blood trails
			CG_AddFallScaleFade( le );
			break;

		case LE_SCALE_FADE:		// rocket trails
			CG_AddScaleFade( le );
			break;

		case LE_SCALE_FADE_SPRITE:		// Trek type for oriented poly sprites.
			CG_AddScaleFadeSprite( le );
			break;

		case LE_LINE:					// Trek type for beams.
			CG_AddLine( le );
			break;

		case LE_LINE2:					// Trek type for beams, with taper support.
			CG_AddLine2( le );
			break;

		case LE_OLINE:					// Trek type for rectangles
			CG_AddOLine( le );
			break;

		case LE_TRAIL:					// Trek type for sparks.
			CG_AddTrail( le );
			break;

		case LE_VIEWSPRITE:				// Trek primitive for camera-facing sprites.
			CG_AddViewSprite( le );
			break;

		case LE_BEZIER:
			CG_AddBezier( le );
			break;

		case LE_QUAD:
			CG_AddQuad( le );
			break;

		case LE_CYLINDER:
			CG_AddCylinder(le);
			break;

		case LE_ELECTRICITY:
			CG_AddElectricity(le);
			break;

		case LE_PARTICLE:
			CG_AddParticle(le);
			break;

		case LE_SPAWNER:
			CG_AddSpawner(le);
			break;

		case LE_FRAGMENT:
			CG_AddFragment(le);
			break;

		case LE_STASISDOOR:
			if(le->refEntity.data.cylinder.wrap) {
				le->refEntity.shaderRGBA[3] = (1 - (le->endTime - cg.time)/1000) * 255;
			} else {
				le->refEntity.shaderRGBA[3] = ((le->endTime - cg.time)/1000) * 255;
			}
			trap_R_AddRefEntityToScene(&le->refEntity);
			break;
		}
	}
}




