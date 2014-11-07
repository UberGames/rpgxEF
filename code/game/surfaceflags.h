// Copyright (C) 1999-2000 Id Software, Inc.
//
// This file must be identical in the quake and utils directories

// contents flags are seperate bits
// a given brush can contribute multiple content bits

// these definitions also need to be in q_shared.h!

#ifndef SURFACEFLAGS_H_
#define SURFACEFLAGS_H_

enum contents_e {
	CONTENTS_NONE = 0,
	CONTENTS_SOLID = 1,		// an eye is never valid in a solid
	CONTENTS_LAVA = 8,
	CONTENTS_SLIME = 16,
	CONTENTS_WATER = 32,
	CONTENTS_FOG = 64,
	CONTENTS_LADDER = 128,

	CONTENTS_AREAPORTAL = 0x8000,

	CONTENTS_PLAYERCLIP = 0x10000,
	CONTENTS_MONSTERCLIP = 0x20000,
	CONTENTS_SHOTCLIP = 0x40000,	//!< These are not needed if CONTENTS_SOLID is included

	//q3 bot specific contents types
	CONTENTS_TELEPORTER = 0x40000,
	CONTENTS_JUMPPAD = 0x80000,	//!< needed for bspc
	CONTENTS_ITEM = 0x80000,	//!< Items can be touched but do not block movement (like triggers) but can be hit by the infoString trace
	CONTENTS_CLUSTERPORTAL = 0x100000,
	CONTENTS_DONOTENTER = 0x200000,
	CONTENTS_BOTCLIP = 0x400000,

	CONTENTS_ORIGIN = 0x1000000,	//!< removed before bsping an entity

	CONTENTS_BODY = 0x2000000,	//!< should never be on a brush, only in game
	CONTENTS_CORPSE = 0x4000000,
	CONTENTS_DETAIL = 0x8000000,	//!< brushes not used for the bsp
	CONTENTS_STRUCTURAL = 0x10000000,	//!< brushes used for the bsp
	CONTENTS_TRANSLUCENT = 0x20000000,	//!< don't consume surface fragments inside
	CONTENTS_TRIGGER = 0x40000000,
	CONTENTS_NODROP = 0x80000000	//!< don't leave bodies or items (death fog, lava)
};

enum surfaceFlags_e {
	SURF_NODAMAGE = 0x1,		//!< never give falling damage
	SURF_SLICK = 0x2,		//!< effects game physics
	SURF_SKY = 0x4,		//!< lighting from environment map
	SURF_NOIMPACT = 0x10,	//!< don't make missile explosions
	SURF_NOMARKS = 0x20,	//!< don't leave missile marks
	SURF_FLESH = 0x40,	//!< make flesh sounds and effects
	SURF_NODRAW = 0x80,	//!< don't generate a drawsurface at all
	SURF_HINT = 0x100,	//!< make a primary bsp splitter
	SURF_SKIP = 0x200,	//!< completely ignore, allowing non-closed brushes
	SURF_NOLIGHTMAP = 0x400,	//!< surface doesn't need a lightmap
	SURF_POINTLIGHT = 0x800,	//!< generate lighting info at vertexes
	SURF_METALSTEPS = 0x1000,	//!< clanking footsteps
	SURF_NOSTEPS = 0x2000,	//!< no footstep sounds
	SURF_NONSOLID = 0x4000,	//!< don't collide against curves with this set
	SURF_LIGHTFILTER = 0x8000,	//!< act as a light filter during q3map -light
	SURF_ALPHASHADOW = 0x10000,	//!< do per-pixel light shadow casting in q3map
	SURF_NODLIGHT = 0x20000,	//!< don't dlight even if solid (solid lava, skies)
	SURF_FORCEFIELD = 0x40000,	//!< the surface in question is a forcefield
	//RPG-X | GSIO01 | 20/05/2009 | START MOD
	SURF_GRASS = 0x80000,		//!< grass, use grass footsteps etc.
	SURF_GRAVEL = 0x100000,	//!< gravel, use grass footsteps etc.
	SURF_SNOW = 0x200000,	//!< snow, use grass footsteps etc.
	SURF_WOOD = 0x400000	//!< wood, use grass footsteps etc.
	//RPG-X | GSIO01 | 20/05/2009 | END MOD
};

#endif /* SURFACEFLAGS_H_ */



