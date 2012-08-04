#ifndef _G_LUA_H
#define _G_LUA_H

#include "cg_local.h"

#if (defined __linux__ || defined __WIN32__) // linux or mingw
#include "../game/lua.h"
#include "../game/lauxlib.h"
#include "../game/lualib.h"
#else
#include "../game/lua.h"
#include "../game/lauxlib.h"
#include "../game/lualib.h"
#endif

#define NUM_VMS 1

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
qboolean			CG_LuaInit(void);
qboolean			CG_LuaCall(lvm_t *vm, char *func, int nargs, int nresults);
qboolean			CG_LuaResume(lvm_t *vm, lua_State *T, char *func, int nargs);
qboolean			CG_LuaGetFunction(lvm_t *vm, char *name);
qboolean			CG_LuaGetFunctionT(lua_State *T, char *name);
qboolean			CG_LuaStartVM(lvm_t *vm);
void				CG_LuaStopVM(lvm_t *vm);
void				CG_LuaShutdown(void);
void				CG_LuaStatus(void);
lvm_t				*CG_LuaGetVM(lua_State *L);

// lua_cgame.c
int			Luaopen_CGame(lua_State *L);

// lua_qmath.c
int			Luaopen_Qmath(lua_State *L);

// lua_vector.c
int			Luaopen_Vector(lua_State *L);
void		Lua_PushVector(lua_State *L, vec3_t v);
vec_t		*Lua_GetVector(lua_State *L, int argNum);
int			Lua_IsVector(lua_State *L, int index);
vec3_t		*Lua_GetVectorMisc(lua_State *L, int *index);

// lua_cfx.c
typedef struct {
	char luaFunc[MAX_QPATH];
} cfx_t;

typedef struct {
	cfx_t **cfx;
	int	    cnt;
} cfxList_t;

void		Lua_CFX_LoadMapFxFile(void);

// lua_cent.c
typedef struct {
	centity_t *e;
} cent_t;

int			Luaopen_Cent(lua_State *L);
void		Lua_PushCent(lua_State *L, centity_t *ent);
cent_t		*Lua_GetCent(lua_State *L, int argNum);
// lua_refent.c
typedef struct {
	refEntity_t *r;
} rent_t;

int			Luaopen_Rent(lua_State *L);
void		Lua_PushRent(lua_State *L, refEntity_t *rent);
rent_t		*Lua_GetRent(lua_State *L, int argNum);

#endif
