// FX Library

#include "cg_local.h"

void FXE_Spray (vec3_t direction, float speed, float variation, float cone, vec3_t velocity)
{
	vec3_t	dir;
	int i;

	//Randomize the direction
	for (i = 0; i < 3; i ++ )
	{
		dir[i] = direction[i] + (cone * crandom());
	}

	VectorNormalize(dir);

	//set the speed
	VectorScale( dir, speed + (variation * crandom()), velocity);
}



localEntity_t *FX_AddLine(vec3_t start, vec3_t end, float stScale, float scale, float dscale, float startalpha, float endalpha, float killTime, qhandle_t shader)
{
	localEntity_t	*le;
	
#ifdef _DEBUG
	if (!shader)
	{
		Com_Printf("FX_AddLine: NULL shader\n");
	}
#endif

	le = CG_AllocLocalEntity();
	le->leType = LE_LINE;

	le->startTime = cg.time;
	le->endTime = le->startTime + killTime;
	le->data.line.width = scale;
	le->data.line.dwidth = dscale;

	le->alpha = startalpha;
	le->dalpha = endalpha - startalpha;

	le->refEntity.data.line.stscale = stScale;
	le->refEntity.data.line.width = scale;

	le->refEntity.customShader = shader;

	// set origin
	VectorCopy ( start, le->refEntity.origin);
	VectorCopy ( end, le->refEntity.oldorigin );

	AxisClear(le->refEntity.axis);
	le->refEntity.shaderRGBA[0] = 0xff;
	le->refEntity.shaderRGBA[1] = 0xff;
	le->refEntity.shaderRGBA[2] = 0xff;
	le->refEntity.shaderRGBA[3] = 0xff;

	le->color[0] = 1.0;
	le->color[1] = 1.0;
	le->color[2] = 1.0;
	le->color[3] = 1.0;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	return(le);
}



localEntity_t *FX_AddLine2(vec3_t start, vec3_t end, float stScale, float width1, float dwidth1, float width2, float dwidth2, 
							float startalpha, float endalpha, vec3_t startRGB, vec3_t endRGB, float killTime, qhandle_t shader)
{
	localEntity_t	*le;
	
#ifdef _DEBUG
	if (!shader)
	{
		Com_Printf("FX_AddLine2: NULL shader\n");
	}
#endif

	le = CG_AllocLocalEntity();
	le->leType = LE_LINE2;

	le->startTime = cg.time;
	le->endTime = le->startTime + killTime;
	le->data.line2.width = width1;
	le->data.line2.dwidth = dwidth1;
	le->data.line2.width2 = width2;
	le->data.line2.dwidth2 = dwidth2;

	le->alpha = startalpha;
	le->dalpha = endalpha - startalpha;
	VectorCopy(startRGB, le->data.line2.startRGB);
	VectorSubtract(endRGB, startRGB, le->data.line2.dRGB);

	le->refEntity.data.line.stscale = stScale;
	le->refEntity.data.line.width = width1;
	le->refEntity.data.line.width2 = width2;

	le->refEntity.customShader = shader;

	// set origin
	VectorCopy ( start, le->refEntity.origin);
	VectorCopy ( end, le->refEntity.oldorigin );

	AxisClear(le->refEntity.axis);
	le->refEntity.shaderRGBA[0] = 0xff;
	le->refEntity.shaderRGBA[1] = 0xff;
	le->refEntity.shaderRGBA[2] = 0xff;
	le->refEntity.shaderRGBA[3] = 0xff;

	le->color[0] = startRGB[0];
	le->color[1] = startRGB[1];
	le->color[2] = startRGB[2];
	le->color[3] = startalpha;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	return(le);
}



localEntity_t *FX_AddLine3(vec3_t start, vec3_t end, float stScale, float scale, float dscale,  
							float startalpha, float endalpha, vec3_t startRGB, vec3_t endRGB, float killTime, qhandle_t shader)
{
	localEntity_t	*le;
	
#ifdef _DEBUG
	if (!shader)
	{
		Com_Printf("FX_AddLine2: NULL shader\n");
	}
#endif

	le = CG_AllocLocalEntity();
	le->leType = LE_LINE2;

	le->startTime = cg.time;
	le->endTime = le->startTime + killTime;
	le->data.line.width = scale;
	le->data.line.dwidth = dscale;

	le->alpha = startalpha;
	le->dalpha = endalpha - startalpha;
	VectorCopy(startRGB, le->data.line2.startRGB);
	VectorSubtract(endRGB, startRGB, le->data.line2.dRGB);

	le->refEntity.data.line.stscale = stScale;
	le->refEntity.data.line.width = scale;

	le->refEntity.customShader = shader;

	// set origin
	VectorCopy ( start, le->refEntity.origin);
	VectorCopy ( end, le->refEntity.oldorigin );

	AxisClear(le->refEntity.axis);
	le->refEntity.shaderRGBA[0] = 0xff;
	le->refEntity.shaderRGBA[1] = 0xff;
	le->refEntity.shaderRGBA[2] = 0xff;
	le->refEntity.shaderRGBA[3] = 0xff;

	le->color[0] = startRGB[0];
	le->color[1] = startRGB[1];
	le->color[2] = startRGB[2];
	le->color[3] = startalpha;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	return(le);
}

localEntity_t *FX_AddOrientedLine(vec3_t start, vec3_t end, vec3_t normal, float stScale, float scale,
								  float dscale, float startalpha, float endalpha, float killTime, qhandle_t shader)
{
	localEntity_t	*le;
	
#ifdef _DEBUG
	if (!shader)
	{
		Com_Printf("FX_AddLine: NULL shader\n");
	}
#endif

	le = CG_AllocLocalEntity();
	le->leType = LE_OLINE;

	le->startTime = cg.time;
	le->endTime = le->startTime + killTime;
	le->data.line.width = scale;
	le->data.line.dwidth = dscale;

	le->alpha = startalpha;
	le->dalpha = endalpha - startalpha;

	le->refEntity.data.line.stscale = stScale;
	le->refEntity.data.line.width = scale;

	le->refEntity.customShader = shader;

	// set origin
	VectorCopy ( start, le->refEntity.origin);
	VectorCopy ( end, le->refEntity.oldorigin );

	AxisClear(le->refEntity.axis);
	VectorCopy( normal, le->refEntity.axis[0] );
	RotateAroundDirection( le->refEntity.axis, 0); // le->refEntity.data.sprite.rotation );	This is roll in quad land

	le->refEntity.shaderRGBA[0] = 0xff;
	le->refEntity.shaderRGBA[1] = 0xff;
	le->refEntity.shaderRGBA[2] = 0xff;
	le->refEntity.shaderRGBA[3] = 0xff;

	le->color[0] = 1.0;
	le->color[1] = 1.0;
	le->color[2] = 1.0;
	le->color[3] = 1.0;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	return(le);
}

localEntity_t *FX_AddTrail( vec3_t origin, vec3_t velocity, qboolean gravity, float length, float dlength, 
						   float scale, float dscale, float startalpha, float endalpha, 
						   float elasticity, float killTime, qhandle_t shader)
{
	localEntity_t *le;

#ifdef _DEBUG
	if (!shader)
	{
		Com_Printf("FX_AddTrail: NULL shader\n");
	}
#endif

	le = CG_AllocLocalEntity();
	le->leType = LE_TRAIL;

	le->startTime = cg.time;
	le->endTime = le->startTime + killTime;

	le->data.trail.width = scale;
	le->data.trail.dwidth = dscale;
	le->data.trail.length = length;
	le->data.trail.dlength = dlength;

	le->alpha = startalpha;
	le->dalpha = endalpha - startalpha;
	VectorSet(le->data.trail.startRGB, 1, 1, 1);
	VectorSet(le->data.trail.dRGB, 0, 0, 0);

	le->refEntity.data.line.stscale = 1.0;
	le->refEntity.data.line.width = scale;

	le->refEntity.customShader = shader;

	// set origin
	VectorCopy ( origin, le->refEntity.origin);
	// kef -- extrapolate oldorigin based on length of trail and origin?
	if (velocity)
	{
		vec3_t	vel;
		VectorNormalize2(velocity, vel);
		VectorMA(origin, -length, vel, le->refEntity.oldorigin);
	}
	else
	{
		VectorCopy ( origin, le->refEntity.oldorigin );
	}

	AxisClear(le->refEntity.axis);
	le->refEntity.shaderRGBA[0] = 0xff;
	le->refEntity.shaderRGBA[1] = 0xff;
	le->refEntity.shaderRGBA[2] = 0xff;
	le->refEntity.shaderRGBA[3] = 0xff*startalpha;

	le->color[0] = 1.0;
	le->color[1] = 1.0;
	le->color[2] = 1.0;
	le->color[3] = 1.0;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	if (velocity)
	{
		le->leFlags |= LEF_MOVE;
		VectorCopy (origin, le->pos.trBase);
		VectorCopy (velocity, le->pos.trDelta);
		if (gravity)
			le->pos.trType = TR_GRAVITY;
		else
			le->pos.trType = TR_LINEAR;
		le->pos.trTime = cg.time;
		le->pos.trDuration = killTime;

		if (elasticity > 0)
		{
			le->leFlags |= LEF_USE_COLLISION;
			le->bounceFactor = elasticity;
		}
	}

	return(le);
}



localEntity_t *FX_AddTrail2( vec3_t origin, vec3_t velocity, qboolean gravity, float length, float dlength, 
						   float scale, float dscale, float startalpha, float endalpha, vec3_t startRGB, vec3_t endRGB,
						   float elasticity, float killTime, qhandle_t shader)
{
	localEntity_t *le;

#ifdef _DEBUG
	if (!shader)
	{
		Com_Printf("FX_AddTrail: NULL shader\n");
	}
#endif

	le = CG_AllocLocalEntity();
	le->leType = LE_TRAIL;

	le->startTime = cg.time;
	le->endTime = le->startTime + killTime;

	le->data.trail.width = scale;
	le->data.trail.dwidth = dscale;
	le->data.trail.length = length;
	le->data.trail.dlength = dlength;

	le->alpha = startalpha;
	le->dalpha = endalpha - startalpha;
	VectorCopy(startRGB, le->data.trail.startRGB);
	VectorSubtract(endRGB, startRGB, le->data.trail.dRGB);

	le->refEntity.data.line.stscale = 1.0;
	le->refEntity.data.line.width = scale;

	le->refEntity.customShader = shader;

	// set origin
	VectorCopy ( origin, le->refEntity.origin);
	// kef -- extrapolate oldorigin based on length of trail and origin?
	if (velocity)
	{
		vec3_t	vel;
		VectorNormalize2(velocity, vel);
		VectorMA(origin, -length, vel, le->refEntity.oldorigin);
	}
	else
	{
		VectorCopy ( origin, le->refEntity.oldorigin );
	}

	AxisClear(le->refEntity.axis);
	le->refEntity.shaderRGBA[0] = 0xff*startRGB[0];
	le->refEntity.shaderRGBA[1] = 0xff*startRGB[1];
	le->refEntity.shaderRGBA[2] = 0xff*startRGB[2];
	le->refEntity.shaderRGBA[3] = 0xff*startalpha;

	le->color[0] = 1.0;
	le->color[1] = 1.0;
	le->color[2] = 1.0;
	le->color[3] = 1.0;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	if (velocity)
	{
		le->leFlags |= LEF_MOVE;
		VectorCopy (origin, le->pos.trBase);
		VectorCopy (velocity, le->pos.trDelta);
		if (gravity)
			le->pos.trType = TR_GRAVITY;
		else
			le->pos.trType = TR_LINEAR;
		le->pos.trTime = cg.time;
		le->pos.trDuration = killTime;

		if (elasticity > 0)
		{
			le->leFlags |= LEF_USE_COLLISION;
			le->bounceFactor = elasticity;
		}
	}

	return(le);
}


/*
===============
FX_AddSprite

Adds a view oriented sprite to the FX wrapper render list
===============
*/

localEntity_t *FX_AddSprite(vec3_t origin, vec3_t velocity, qboolean gravity, float scale, float dscale, 
							float startalpha, float endalpha, float roll, float elasticity, 
							float killTime, qhandle_t shader)
{
	localEntity_t *le;

#ifdef _DEBUG
	if (!shader)
	{
		Com_Printf("FX_AddSprite: NULL shader\n");
	}
#endif

	// Glow mark

	le = CG_AllocLocalEntity();
	le->leType = LE_VIEWSPRITE;
	le->refEntity.data.sprite.rotation = roll;

	le->startTime = cg.time;
	le->endTime = le->startTime + killTime;

	le->data.sprite.radius = scale;
	le->data.sprite.dradius = dscale;

	le->alpha = startalpha;
	le->dalpha = endalpha - startalpha;
	VectorSet(le->data.sprite.startRGB, 1, 1, 1);
	VectorSet(le->data.sprite.dRGB, 0, 0, 0);

//	le->refEntity.hModel = 0;
	le->refEntity.customShader = shader;

	// set origin
	VectorCopy ( origin, le->refEntity.origin);
	VectorCopy ( origin, le->refEntity.oldorigin );

	le->color[0] = 1.0;
	le->color[1] = 1.0;
	le->color[2] = 1.0;
	le->color[3] = startalpha;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	if (velocity)
	{
		le->leFlags |= LEF_MOVE;
		VectorCopy (origin, le->pos.trBase);
		VectorCopy (velocity, le->pos.trDelta);
		if (gravity)
			le->pos.trType = TR_GRAVITY;
		else
			le->pos.trType = TR_LINEAR;
		le->pos.trTime = cg.time;
		le->pos.trDuration = killTime;

		if (elasticity > 0)
		{
			le->leFlags |= LEF_USE_COLLISION;
			le->bounceFactor = elasticity;
		}
	}

	return(le);
}


/*
===============
FX_AddSprite2

Adds a view oriented sprite to the FX wrapper render list
===============
*/

localEntity_t *FX_AddSprite2(vec3_t origin, vec3_t velocity, qboolean gravity, float scale, float dscale, 
							float startalpha, float endalpha, vec3_t startRGB, vec3_t endRGB, float roll, float elasticity, 
							float killTime, qhandle_t shader)
{
	localEntity_t *le;

#ifdef _DEBUG
	if (!shader)
	{
		Com_Printf("FX_AddSprite: NULL shader\n");
	}
#endif

	// Glow mark

	le = CG_AllocLocalEntity();
	le->leType = LE_VIEWSPRITE;
	le->refEntity.data.sprite.rotation = roll;

	le->startTime = cg.time;
	le->endTime = le->startTime + killTime;

	le->data.sprite.radius = scale;
	le->data.sprite.dradius = dscale;

	le->alpha = startalpha;
	le->dalpha = endalpha - startalpha;
	VectorCopy(startRGB, le->data.sprite.startRGB);
	VectorSubtract(endRGB, startRGB, le->data.sprite.dRGB);

//	le->refEntity.hModel = 0;
	le->refEntity.customShader = shader;

	// set origin
	VectorCopy ( origin, le->refEntity.origin);
	VectorCopy ( origin, le->refEntity.oldorigin );

	le->color[0] = startRGB[0];
	le->color[1] = startRGB[1];
	le->color[2] = startRGB[2];
	le->color[3] = startalpha;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	if (velocity)
	{
		le->leFlags |= LEF_MOVE;
		VectorCopy (origin, le->pos.trBase);
		VectorCopy (velocity, le->pos.trDelta);
		if (gravity)
			le->pos.trType = TR_GRAVITY;
		else
			le->pos.trType = TR_LINEAR;
		le->pos.trTime = cg.time;
		le->pos.trDuration = killTime;

		if (elasticity > 0)
		{
			le->leFlags |= LEF_USE_COLLISION;
			le->bounceFactor = elasticity;
		}
	}

	return(le);
}


/*
===============
FX_AddSprite3

Adds a view oriented sprite to the FX wrapper render list
===============
*/

localEntity_t *FX_AddSprite3(vec3_t origin, vec3_t velocity, vec3_t acceleration, float scale, float dscale, 
							float startalpha, float endalpha, float roll, float elasticity, 
							float killTime, qhandle_t shader)
{
	localEntity_t *le;

#ifdef _DEBUG
	if (!shader)
	{
		Com_Printf("FX_AddSprite: NULL shader\n");
	}
#endif

	// Glow mark

	le = CG_AllocLocalEntity();
	le->leType = LE_VIEWSPRITE;
	le->refEntity.data.sprite.rotation = roll;

	le->startTime = cg.time;
	le->endTime = le->startTime + killTime;

	le->data.sprite.radius = scale;
	le->data.sprite.dradius = dscale;

	le->alpha = startalpha;
	le->dalpha = endalpha - startalpha;
	VectorSet(le->data.sprite.startRGB, 1, 1, 1);
	VectorSet(le->data.sprite.dRGB, 0, 0, 0);

//	le->refEntity.hModel = 0;
	le->refEntity.customShader = shader;

	// set origin
	VectorCopy ( origin, le->refEntity.origin);
	VectorCopy ( origin, le->refEntity.oldorigin );

	le->color[0] = 1.0;
	le->color[1] = 1.0;
	le->color[2] = 1.0;
	le->color[3] = startalpha;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	if (velocity)
	{
		le->leFlags |= LEF_MOVE;
		VectorCopy (origin, le->pos.trBase);
		VectorCopy (velocity, le->pos.trDelta);
		if (acceleration) //how do i make this accellerate in the given direction?... lol, bee-fountain on forge3 now ^^
			le->pos.trType = TR_GRAVITY;
		else
			le->pos.trType = TR_LINEAR;
		le->pos.trTime = cg.time;
		le->pos.trDuration = killTime;

		if (elasticity > 0)
		{
			le->leFlags |= LEF_USE_COLLISION;
			le->bounceFactor = elasticity;
		}
	}

	return(le);
}

/*
===============
FX_AddBezier

Adds a Bezier curve to the FX wrapper render list
===============
*/

localEntity_t *FX_AddBezier(vec3_t start, vec3_t end, vec3_t cpoint1, vec3_t cpoint2, vec3_t cpointvel1, vec3_t cpointvel2,
							vec3_t cpointacc1, vec3_t cpointacc2, float width, float killTime, qhandle_t shader)
{
	localEntity_t	*le = CG_AllocLocalEntity();

#ifdef _DEBUG
	if (!shader)
	{
		Com_Printf("FX_AddBezier: NULL shader\n");
	}
#endif

	// just testing beziers
	le->leType = LE_BEZIER;

	le->startTime = cg.time;
	le->endTime = le->startTime + killTime;
	le->data.line.width = width;

	le->alpha = 1.0;
	le->dalpha = -1.0;

	le->refEntity.customShader = shader;

	// set origin
	VectorCopy ( start, le->refEntity.origin);
	VectorCopy ( end, le->refEntity.oldorigin );

	AxisClear(le->refEntity.axis);
	le->refEntity.shaderRGBA[0] = 0xff;
	le->refEntity.shaderRGBA[1] = 0xff;
	le->refEntity.shaderRGBA[2] = 0xff;
	le->refEntity.shaderRGBA[3] = 0xff;

	le->color[0] = 1.0;
	le->color[1] = 1.0;
	le->color[2] = 1.0;
	le->color[3] = 1.0;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	if (cpoint1)
	{
		VectorCopy(cpoint1, le->data.line.control1);
	}
	if (cpoint2)
	{
		VectorCopy(cpoint2, le->data.line.control2);
	}
	if (cpointvel1)
	{
		VectorCopy(cpointvel1, le->data.line.control1_velocity);
	}
	if (cpointvel2)
	{
		VectorCopy(cpointvel2, le->data.line.control2_velocity);
	}
	if (cpointacc1)
	{
		VectorCopy(cpointacc1, le->data.line.control1_acceleration);
	}
	if (cpointacc2)
	{
		VectorCopy(cpointacc2, le->data.line.control2_acceleration);
	}

	return le;
}

/*
===============
FX_AddQuad

Adds a quad to the FX wrapper render list
===============
*/

localEntity_t *FX_AddQuad( vec3_t origin, vec3_t normal, float scale, float dscale,
						   float startalpha, float endalpha, float roll, float killTime, qhandle_t shader )
{
	localEntity_t	*le = CG_AllocLocalEntity();

#ifdef _DEBUG
	if (!shader)
	{
		Com_Printf("FX_AddQuad: NULL shader\n");
	}
#endif

	le->leType = LE_QUAD;
	le->refEntity.data.sprite.rotation = roll;

	le->startTime = cg.time;
	le->endTime = le->startTime + killTime;

	le->data.sprite.radius = scale;
	le->data.sprite.dradius = dscale;

	le->alpha = startalpha;
	le->dalpha = endalpha - startalpha;
	VectorSet(le->data.sprite.startRGB, 1, 1, 1);
	VectorSet(le->data.sprite.dRGB, 0, 0, 0);

//	le->refEntity.hModel = 0;
	le->refEntity.customShader = shader;

	// set origin
	VectorCopy ( origin, le->refEntity.origin);
	VectorCopy ( origin, le->refEntity.oldorigin );

	VectorCopy( normal, le->refEntity.axis[0] );
	RotateAroundDirection( le->refEntity.axis, le->refEntity.data.sprite.rotation );

	le->color[0] = 1.0;
	le->color[1] = 1.0;
	le->color[2] = 1.0;
	le->color[3] = startalpha;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	return(le);
}



/*
===============
FX_AddQuad2

Adds a quad to the FX wrapper render list
===============
*/

localEntity_t *FX_AddQuad2( vec3_t origin, vec3_t normal, float scale, float dscale, float startalpha, float endalpha, 
						   vec3_t startRGB, vec3_t endRGB, float roll, float killTime, qhandle_t shader )
{
	localEntity_t	*le = CG_AllocLocalEntity();

#ifdef _DEBUG
	if (!shader)
	{
		Com_Printf("FX_AddQuad: NULL shader\n");
	}
#endif

	le->leType = LE_QUAD;
	le->refEntity.data.sprite.rotation = roll;

	le->startTime = cg.time;
	le->endTime = le->startTime + killTime;

	le->data.sprite.radius = scale;
	le->data.sprite.dradius = dscale;
	VectorCopy(startRGB, le->data.sprite.startRGB);
	VectorSubtract(endRGB, startRGB, le->data.sprite.dRGB);

	le->alpha = startalpha;
	le->dalpha = endalpha - startalpha;

//	le->refEntity.hModel = 0;
	le->refEntity.customShader = shader;

	// set origin
	VectorCopy ( origin, le->refEntity.origin);
	VectorCopy ( origin, le->refEntity.oldorigin );

	VectorCopy( normal, le->refEntity.axis[0] );
	RotateAroundDirection( le->refEntity.axis, le->refEntity.data.sprite.rotation );

	le->color[0] = startRGB[0];
	le->color[1] = startRGB[1];
	le->color[2] = startRGB[2];
	le->color[3] = startalpha;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	return(le);
}



/*
===============
FX_AddCylinder

Adds a cylinder to the FX wrapper render list
===============
*/

//NOTENOTE: The reigning king of parameters!
#define DEFAULT_ST_SCALE	1.0f

localEntity_t *FX_AddCylinder(	vec3_t start, 
									vec3_t normal,
									float height,
									float dheight,
									float scale,
									float dscale,
									float scale2,
									float dscale2,
									float startalpha, 
									float endalpha, 
									float killTime, 
									qhandle_t shader,
									float bias )
{
	localEntity_t	*le = CG_AllocLocalEntity();

#ifdef _DEBUG
	if (!shader)
	{
		Com_Printf("FX_AddCylinder: NULL shader\n");
	}
#endif

	le->leType = LE_CYLINDER;
	le->refEntity.data.cylinder.height = height;
	le->refEntity.data.cylinder.width = scale;
	le->refEntity.data.cylinder.width2 = scale2;

	le->startTime = cg.time;
	le->endTime = le->startTime + killTime;

	le->data.cylinder.height = height;
	le->data.cylinder.dheight = dheight;
	le->data.cylinder.width = scale;
	le->data.cylinder.dwidth = dscale;
	le->data.cylinder.width2 = scale2;
	le->data.cylinder.dwidth2 = dscale2;

	le->alpha = startalpha;
	le->dalpha = endalpha - startalpha;

	le->refEntity.customShader = shader;

	le->refEntity.data.cylinder.bias = bias;
	le->refEntity.data.cylinder.stscale = 1.0;
	le->refEntity.data.cylinder.wrap = qtrue;

	// set origin
	VectorCopy ( start, le->refEntity.origin);
	VectorCopy ( start, le->refEntity.oldorigin );

	VectorCopy( normal, le->refEntity.axis[0] );
	RotateAroundDirection( le->refEntity.axis, 0);

	le->color[0] = 1.0;
	le->color[1] = 1.0;
	le->color[2] = 1.0;
	le->color[3] = startalpha;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	return(le);
}
/*
===============
FX_AddCylinder2

Adds a cylinder to the FX wrapper render list
Overloaded for RGB
===============
*/

//NOTENOTE: The reigning king of parameters!
#define DEFAULT_ST_SCALE	1.0f

localEntity_t *FX_AddCylinder2(	vec3_t start, 
									vec3_t normal,
									float height,
									float dheight,
									float scale,
									float dscale,
									float scale2,
									float dscale2,
									float startalpha, 
									float endalpha,
									vec3_t startRGB,
									vec3_t endRGB, 
									float killTime, 
									qhandle_t shader,
									float bias )
{
	localEntity_t	*le = CG_AllocLocalEntity();

#ifdef _DEBUG
	if (!shader)
	{
		Com_Printf("FX_AddCylinder: NULL shader\n");
	}
#endif

	le->leType = LE_CYLINDER;
	le->refEntity.data.cylinder.height = height;
	le->refEntity.data.cylinder.width = scale;
	le->refEntity.data.cylinder.width2 = scale2;

	le->startTime = cg.time;
	le->endTime = le->startTime + killTime;

	le->data.cylinder.height = height;
	le->data.cylinder.dheight = dheight;
	le->data.cylinder.width = scale;
	le->data.cylinder.dwidth = dscale;
	le->data.cylinder.width2 = scale2;
	le->data.cylinder.dwidth2 = dscale2;

	le->alpha = startalpha;
	le->dalpha = endalpha - startalpha;

	le->refEntity.customShader = shader;

	le->refEntity.data.cylinder.bias = bias;
	le->refEntity.data.cylinder.stscale = 1.0;
	le->refEntity.data.cylinder.wrap = qtrue;

	// set origin
	VectorCopy ( start, le->refEntity.origin);
	VectorCopy ( start, le->refEntity.oldorigin );

	VectorCopy( normal, le->refEntity.axis[0] );
	RotateAroundDirection( le->refEntity.axis, 0);

	le->color[0] = startRGB[0];
	le->color[1] = startRGB[1];
	le->color[2] = startRGB[2];
	le->color[3] = startalpha;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	return(le);
}



/*
===============
FX_AddElectricity

Adds a electricity bolt to the scene
===============
*/

localEntity_t *FX_AddElectricity( vec3_t origin, vec3_t origin2, float stScale, float scale, float dscale, 
									float startalpha, float endalpha, float killTime, qhandle_t shader, float deviation )
{
	localEntity_t *le;

#ifdef _DEBUG
	if (!shader)
	{
		Com_Printf("FX_AddElectricity: NULL shader\n");
	}
#endif

	le = CG_AllocLocalEntity();
	le->leType = LE_ELECTRICITY;

	// set origin
	VectorCopy (origin, le->refEntity.origin);
	VectorCopy (origin2, le->refEntity.oldorigin );

	le->refEntity.data.electricity.stscale = stScale;
	le->refEntity.data.electricity.deviation = deviation;
	le->data.electricity.width = scale;
	le->data.electricity.dwidth = dscale;

	le->alpha = startalpha;
	le->dalpha = endalpha - startalpha;

	le->startTime = cg.time;
	le->endTime = le->startTime + killTime;

	le->refEntity.customShader = shader;

	le->color[0] = 1.0;
	le->color[1] = 1.0;
	le->color[2] = 1.0;
	le->color[3] = startalpha;

	return(le);
}
/*
===============
FX_AddParticle

Adds a particle (basically, a sprite with an optional think function) to the FX wrapper render list
===============
*/

localEntity_t *FX_AddParticle( vec3_t origin, vec3_t velocity, qboolean gravity, float scale, float dscale, 
							float startalpha, float endalpha, float roll, float elasticity, 
							float killTime, qhandle_t shader, qboolean (*thinkFn)(localEntity_t *le) )
{
	localEntity_t *le;

#ifdef _DEBUG
	if (!shader)
	{
		Com_Printf("FX_AddParticle: NULL shader\n");
	}
#endif

	// Glow mark

	le = CG_AllocLocalEntity();
	le->leType = LE_PARTICLE;
	le->refEntity.data.sprite.rotation = roll;

	le->startTime = cg.time;
	le->endTime = le->startTime + killTime;

	le->data.particle.radius = scale;
	le->data.particle.dradius = dscale;

	le->alpha = startalpha;
	le->dalpha = endalpha - startalpha;

//	le->refEntity.hModel = 0;
	le->refEntity.customShader = shader;

	// set origin
	VectorCopy ( origin, le->refEntity.origin);
	VectorCopy ( origin, le->refEntity.oldorigin );

	le->color[0] = 1.0;
	le->color[1] = 1.0;
	le->color[2] = 1.0;
	le->color[3] = startalpha;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	// wacky think function stuff
	le->data.particle.thinkFn = thinkFn;

	if (velocity)
	{
		le->leFlags |= LEF_MOVE;
		VectorCopy (origin, le->pos.trBase);
		VectorCopy (velocity, le->pos.trDelta);
		if (gravity)
			le->pos.trType = TR_GRAVITY;
		else
			le->pos.trType = TR_LINEAR;
		le->pos.trTime = cg.time;
		le->pos.trDuration = killTime;

		if (elasticity > 0)
		{
			le->leFlags |= LEF_USE_COLLISION;
			le->bounceFactor = elasticity;
		}
	}

	return(le);
}

/*
===============
FX_AddSpawner

Adds a spawner -- basically, a local entity with a think function. Spawners don't have any rendered entities
associated with them inherently, but the spawner's think fn probably generates them.
===============
*/
localEntity_t *FX_AddSpawner( vec3_t origin, vec3_t dir, vec3_t velocity, vec3_t user, qboolean gravity, int delay,
							 float variance, float killTime, qboolean (*thinkFn)(localEntity_t *le), int radius )
{
	localEntity_t *le = NULL;
	
	if (NULL == thinkFn)
	{
		// a spawner with no think fn is silly. and useless.
		return NULL;
	}
	le = CG_AllocLocalEntity();

	le->leType = LE_SPAWNER;

	le->data.spawner.data1 = radius;
	le->data.spawner.delay = delay;
	le->data.spawner.nextthink = cg.time + delay;
	le->startTime = cg.time;
	// if we want the spawner to hang around forever, we use a killtime of 0 and the think fn keeps adjusting it.
	//thing is, we still need it to not get culled right here, so give it an arbitrary endTime somewhere in the future.
	if (0 == killTime)
	{
		le->endTime = le->startTime + 10000;
		le->data.spawner.dontDie = qtrue;
	}
	else
	{
		le->endTime = le->startTime + killTime;
	}

	le->data.spawner.variance = variance;
	if(dir)
		VectorCopy(dir, le->data.spawner.dir);
	// set origin
	VectorCopy ( origin, le->refEntity.origin);
	VectorCopy ( origin, le->refEntity.oldorigin );

	// maybe employ the user variable here, like in singleplayer? or in the think fn?
	le->color[0] = 1.0;
	le->color[1] = 1.0;
	le->color[2] = 1.0;
	le->color[3] = 1.0;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	// wacky think function stuff
	le->data.spawner.thinkFn = thinkFn;

	if (velocity)
	{
		le->leFlags |= LEF_MOVE;
		VectorCopy (origin, le->pos.trBase);
		if(velocity)
			VectorCopy (velocity, le->pos.trDelta);
		if (gravity)
			le->pos.trType = TR_GRAVITY;
		else
			le->pos.trType = TR_LINEAR;
		le->pos.trTime = cg.time;
		// we better not ever have a spawner with a velocity that we expect to last forever, so just plain
		//assigning killTime here _should_ be ok
		le->pos.trDuration = killTime;

//		if (elasticity > 0)
//		{
//			le->leFlags |= LEF_USE_COLLISION;
//			le->bounceFactor = elasticity;
//		}
	}

	return (le);
}

// provide the center of the circle, a normal out from it (normalized, please), and the radius.
//out will then become a random position on the radius of the circle.
void fxRandCircumferencePos(vec3_t center, vec3_t normal, float radius, vec3_t out)
{
	float		rnd = flrandom(0, 2*M_PI);
	float		s = sin(rnd);
	float		c = cos(rnd);
	vec3_t		vTemp, radialX, radialY;

	vTemp[0]=0.57735;
	vTemp[1]=0.57735;
	vTemp[2]=0.57735;
	CrossProduct(normal, vTemp, radialX);
	CrossProduct(normal, radialX, radialY);
	VectorScale(radialX, radius, radialX);
	VectorScale(radialY, radius, radialY);
	VectorMA(center, s, radialX, out);
	VectorMA(out, c, radialY, out);
}
