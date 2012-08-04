#ifndef _G_LUA_H
#define _G_LUA_H

#include "g_local.h"

#if (defined __linux__ || defined __WIN32__) // linux or mingw
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#else
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#endif

#define NUM_VMS 3

#if defined __linux__
#define HOSTARCH	"UNIX"
#define EXTENSION	"so"
#elif defined WIN32
#define HOSTARCH	"WIN32"
#define EXTENSION	"dll"
#elif defined __APPLE__
#define HOSTARCH	"UNIX"
#define EXTENSION	"dylib"
#endif

#define Lua_RegisterGlobal(L, n, v) (lua_pushstring(L, v), lua_setglobal(L, n))
#define Lua_RegConstInteger(L, n) (lua_pushstring(L, #n), lua_pushinteger(L, n), lua_settable(L, -3))
#define Lua_RegConstString(L, n) (lua_pushstring(L, #n), lua_pushstring(L, n), lua_settable(L, -3))

typedef struct {
	int				id;
	char			filename[MAX_QPATH];
	char			*code;
	int				code_size;
	int				error;
	lua_State		*L;
} lvm_t;

extern lvm_t *lVM[NUM_VMS];

void QDECL			LUA_DEBUG(const char *fmt, ...);
void QDECL			LUA_LOG(const char *fmt, ...);
qboolean			G_LuaInit(void);
qboolean			G_LuaCall(lvm_t *vm, char *func, int nargs, int nresults);
qboolean			G_LuaResume(lvm_t *vm, lua_State *T, char *func, int nargs);
qboolean			G_LuaGetFunction(lvm_t *vm, char *name);
qboolean			G_LuaGetFunctionT(lua_State *T, char *name);
qboolean			G_LuaStartVM(lvm_t *vm);
void				G_LuaStopVM(lvm_t *vm);
void				G_LuaShutdown(void);
void				G_LuaStatus(gentity_t *ent);
lvm_t				*G_LuaGetVM(lua_State *L);

// lua_entity.c
typedef struct {
	gentity_t *e;
} lent_t;

int			Luaopen_Entity(lua_State *L);
void		Lua_PushEntity(lua_State *L, gentity_t *ent);
lent_t		*Lua_GetEntity(lua_State *L, int argNum);

// lua_weapons.c
int			Luaopen_Weapons(lua_State *L);

// lua_trace.c
typedef struct {
	trace_t *tr;
} ltrace_t;

void Lua_PushTrace(lua_State * L, trace_t * tr);
ltrace_t *Lua_GetTrace(lua_State * L, int argNum);
int Luaopen_Trace(lua_State *L);

// lua_game.c
int			Luaopen_Game(lua_State *L);

// lua_qmath.c
int			Luaopen_Qmath(lua_State *L);

// lua_vector.c
int			Luaopen_Vector(lua_State *L);
void		Lua_PushVector(lua_State *L, vec3_t v);
vec_t		*Lua_GetVector(lua_State *L, int argNum);
int			Lua_IsVector(lua_State *L, int index);
vec3_t		*Lua_GetVectorMisc(lua_State *L, int *index);

// lua_mover.c
int			Luaopen_Mover(lua_State *L);

// lua_cinematic.c
int			Luaopen_Cinematic(lua_State *L);

// lua_sound.c
int			Luaopen_Sound(lua_State *L);

#endif
