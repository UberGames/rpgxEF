// lua library for trace_t

#include "g_lua.h"

#ifdef G_LUA

#include "g_syscalls.h"

/***
A module allowing to do traces. Documentation under work.
@module trace
*/

static int Trace_GC(lua_State * L)
{

	return 0;
}

static int Trace_ToString(lua_State * L)
{
	ltrace_t     *ltrace;
	trace_t      *trace;
	char         buf[MAX_STRING_CHARS];

	ltrace = Lua_GetTrace(L, 1);
	trace = ltrace->tr;
	Com_sprintf(buf, sizeof(buf), "trace: entity=%i fraction=%f allsolid=%i contents=%i endpos=\"%s\" startsolid=%i surfaceFlags=%i pointer=%p\n", 
											trace->entityNum,
											trace->fraction,
											trace->allsolid,
											trace->contents,
											vtos(trace->endpos),
											trace->startsolid,
											trace->surfaceFlags,
											trace);
	lua_pushstring(L, buf);

	return 1;
}

/***
Does a trace.
@function DoTrace
@param start start-point of the trace.
@param mins minimal distance of trace (nil if unused)
@param maxs maximal distance of trace (nil if unused)
@param end end-point of trace
@param passEnt Number of ents to pass
@param contents Set content flags.
*/
static int Trace_DoTrace(lua_State *L) {
	trace_t *tr;
	vec_t *start, *end, *mins = NULL, *maxs = NULL;
	int passEnt, contents;

	start = Lua_GetVector(L, 1);
	if(!lua_isnil(L, 2))
		mins = Lua_GetVector(L, 2);
	if(!lua_isnil(L, 3))
		maxs = Lua_GetVector(L, 3);
	end = Lua_GetVector(L, 4);
	passEnt = (int)luaL_checknumber(L, 5);
	contents = (int) luaL_checknumber(L, 6);

	tr = (trace_t *)malloc(sizeof(trace_t));
	if(!tr) {
		LUA_DEBUG("Trace_DoTrace - was unable to allocate a trace_t.\n");
		lua_pushnil(L);
		return 1;
	}

	trap_Trace(tr, start, mins, maxs, end, passEnt, contents);

	Lua_PushTrace(L, tr);

	return 1;
}

/***
Frees all memory that was allocated for this trace.
@function FreeTrace
@param trace The trace.
*/
static int Trace_FreeTrace(lua_State *L) {
	ltrace_t *tr;

	tr = Lua_GetTrace(L, 1);
	if(tr && tr->tr)
		free(tr->tr);

	return 1;
}

/***
Check whether the trace has gone only trough solid content (e.g. only inside a wall).
@function GetAllsolid
@return Whether trace was all solid.
*/
static int Trace_GetAllsolid(lua_State *L) {
	ltrace_t *tr;

	tr = Lua_GetTrace(L, 1);
	lua_pushboolean(L, (int)tr->tr->allsolid);

	return 1;
}

/***
Check whether the trace has started in solid contents.
@function GetStartsolid
@return Whether trace started solid.
*/
static int Trace_GetStartsolid(lua_State *L) {
	ltrace_t *tr;

	tr = Lua_GetTrace(L, 1);
	lua_pushboolean(L, (int)tr->tr->startsolid);

	return 1;
}

/***
Get fraction of trace.
@function GetFraction
@return Fraction.
*/
static int Trace_GetFraction(lua_State *L) {
	ltrace_t *tr;

	tr = Lua_GetTrace(L, 1);
	lua_pushnumber(L, tr->tr->fraction);

	return 1;
}

/***
Get end position of the trace.
@function GetEndpos
@return End position of the trace.
*/
static int Trace_GetEndpos(lua_State *L) {
	ltrace_t *tr;

	tr = Lua_GetTrace(L, 1);
	Lua_PushVector(L, tr->tr->endpos);

	return 1;
}

/***
Get the surface flags for the face the trace hit.
@function GetSurfaceFlags
@return Surface flags.
*/
static int Trace_GetSurfaceFlags(lua_State *L) {
	ltrace_t *tr;

	tr = Lua_GetTrace(L, 1);
	lua_pushnumber(L, tr->tr->surfaceFlags);

	return 1;
}

/***
Get content flags for the trace.
@function GetContents
@return Content flags.
*/
static int Trace_GetContents(lua_State *L) {
	ltrace_t *tr;

	tr = Lua_GetTrace(L, 1);
	lua_pushnumber(L, tr->tr->contents);

	return 1;
}

/***
Get entity number for entity the trace hit.
@function GetEntityNum
@return Entity number.
*/
static int Trace_GetEntityNum(lua_State *L) {
	ltrace_t *tr;

	tr = Lua_GetTrace(L, 1);
	lua_pushnumber(L, tr->tr->entityNum);

	return 1;
}

/***
Get entity the trace hit.
@function GetEntity
@return entity the trace hit.
*/
static int Trace_GetEntity(lua_State *L) {
	ltrace_t *tr;
	gentity_t *ent;

	tr = Lua_GetTrace(L, 1);
	ent = &g_entities[tr->tr->entityNum];
	Lua_PushEntity(L, ent);

	return 1;
}

static const luaL_Reg lib_trace[] = {
	{ "DoTrace",	Trace_DoTrace	},
	{ "FreeTrace",	Trace_FreeTrace	},
	{ NULL,			NULL			}
};

static const luaL_Reg Trace_meta[] = {
	{ "__gc",				Trace_GC				},
	{ "__tostring",			Trace_ToString			},
	{ "GetAllsolid",		Trace_GetAllsolid		},
	{ "GetStartsolid",		Trace_GetStartsolid		},
	{ "GetFraction",		Trace_GetFraction		},
	{ "GetEndpos",			Trace_GetEndpos			},
	{ "GetSurfaceFlags",	Trace_GetSurfaceFlags	},
	{ "GetContents",		Trace_GetContents		},
	{ "GetEntityNum",		Trace_GetEntityNum		},
	{ "GetEntity",			Trace_GetEntity			},
	{ NULL,					NULL					}
};

int Luaopen_Trace(lua_State *L) {
	luaL_newmetatable(L, "game.trace");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	luaL_register(L, NULL, Trace_meta);
	luaL_register(L, "trace", lib_trace);

	/* Add constants */
	lua_pushstring(L, "CONSTANTS");
	lua_newtable(L);
	Lua_RegConstInteger(L, CONTENTS_NONE);
	Lua_RegConstInteger(L, CONTENTS_SOLID);
	Lua_RegConstInteger(L, CONTENTS_LAVA);
	Lua_RegConstInteger(L, CONTENTS_SLIME);
	Lua_RegConstInteger(L, CONTENTS_WATER);
	Lua_RegConstInteger(L, CONTENTS_FOG);
	Lua_RegConstInteger(L, CONTENTS_LADDER);
	Lua_RegConstInteger(L, CONTENTS_AREAPORTAL);
	Lua_RegConstInteger(L, CONTENTS_PLAYERCLIP);
	Lua_RegConstInteger(L, CONTENTS_MONSTERCLIP);
	Lua_RegConstInteger(L, CONTENTS_SHOTCLIP);
	Lua_RegConstInteger(L, CONTENTS_TELEPORTER);
	Lua_RegConstInteger(L, CONTENTS_JUMPPAD);
	Lua_RegConstInteger(L, CONTENTS_ITEM);
	Lua_RegConstInteger(L, CONTENTS_CLUSTERPORTAL);
	Lua_RegConstInteger(L, CONTENTS_DONOTENTER);
	Lua_RegConstInteger(L, CONTENTS_BOTCLIP);
	Lua_RegConstInteger(L, CONTENTS_ORIGIN);
	Lua_RegConstInteger(L, CONTENTS_BODY);
	Lua_RegConstInteger(L, CONTENTS_CORPSE);
	Lua_RegConstInteger(L, CONTENTS_DETAIL);
	Lua_RegConstInteger(L, CONTENTS_STRUCTURAL);
	Lua_RegConstInteger(L, CONTENTS_TRANSLUCENT);
	Lua_RegConstInteger(L, CONTENTS_TRIGGER);
	Lua_RegConstInteger(L, CONTENTS_NODROP);
	Lua_RegConstInteger(L, MASK_ALL);
	Lua_RegConstInteger(L, MASK_SOLID);
	Lua_RegConstInteger(L, MASK_PLAYERSOLID);
	Lua_RegConstInteger(L, MASK_DEADSOLID);
	Lua_RegConstInteger(L, MASK_WATER);
	Lua_RegConstInteger(L, MASK_OPAQUE);
	Lua_RegConstInteger(L, MASK_SHOT);
	Lua_RegConstInteger(L, MASK_ONLYPLAYER);
	Lua_RegConstInteger(L, MASK_BRUSHES);
	lua_settable(L, -3);

	return 1;
}

void Lua_PushTrace(lua_State * L, trace_t * tr)
{
	ltrace_t     *trace;

	trace = (ltrace_t *)lua_newuserdata(L, sizeof(ltrace_t));

	luaL_getmetatable(L, "game.trace");
	lua_setmetatable(L, -2);

	trace->tr = tr;
}

ltrace_t *Lua_GetTrace(lua_State * L, int argNum)
{
	void           *ud;

	ud = luaL_checkudata(L, argNum, "game.trace");
	luaL_argcheck(L, ud != NULL, argNum, "\'trace\' expected");
	return (ltrace_t *) ud;
}
#endif
