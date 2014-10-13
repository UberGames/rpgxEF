// Copyright (C) 1999-2000 Id Software, Inc.
//
// bg_local.h -- local definitions for the bg (both games) files

#ifndef BG_LOCAL_H_
#define BG_LOCAL_H_

#ifdef _MSC_VER
#include <stddef.h>
#include <stdint.h>
#else
#include <stdint.h>
#endif

static const double	MIN_WALK_NORMAL = 0.7;		// can't walk on very steep slopes

static const uint32_t STEPSIZE = 18;

static const uint32_t JUMP_VELOCITY = 270;

static const uint32_t TIMER_LAND = 130;
static const uint32_t TIMER_GESTURE = (34 * 66 + 50);


static const double OVERCLIP = 1.001;

// all of the locals will be zeroed before each
// pmove, just to make damn sure we don't have
// any differences when running on client or server
typedef struct {
	vec3_t		forward, right, up;
	double		frametime;

	int32_t		msec;

	qboolean	walking;
	qboolean	groundPlane;
	trace_t		groundTrace;

	double		impactSpeed;

	vec3_t		previous_origin;
	vec3_t		previous_velocity;
	int32_t		previous_waterlevel;
} pml_t;

extern	pmove_t* pm;
extern	pml_t pml;

extern	int32_t	c_pmove;

void PM_ClipVelocity( vec3_t in, vec3_t normal, vec3_t out, double overbounce );
void PM_AddTouchEnt( int32_t entityNum );
void PM_AddEvent( int32_t newEvent );

qboolean PM_SlideMove( qboolean gravity );
void PM_StepSlideMove( qboolean gravity );

#endif /* BG_LOCAL_H_ */



