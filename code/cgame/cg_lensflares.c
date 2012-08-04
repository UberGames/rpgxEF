/***************************************************
Copyright TiM - UberGames, 2005

cg_lensflares.c - Stores all of the functions 
required to draw a dynamic lensflare ingame

The flare is broken up and drawn in separate passes:

Ambient Glow - Colored hazy glow (controlled by a vec4_t)
that expands around the core

Direct Glow - Much stronger glow that surrounds the
core directly

White Core - The actual white, focussed part of the flare

Anamorphic Streak - Horizontal line running through
the flare (Current rage in lensflare FX)

Lens Reflections - Circular parts that go in the opposite dir
of the flare itself
***************************************************/

//#include "cg_local.h"
//#include "cg_text.h"
