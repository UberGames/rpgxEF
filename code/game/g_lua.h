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

void QDECL LUA_DEBUG(const char *fmt, ...);
void QDECL LUA_LOG(const char *fmt, ...);
qboolean G_LuaInit(void);
qboolean G_LuaCall(lvm_t *vm, char *func, int nargs, int nresults);
qboolean G_LuaResume(lvm_t *vm, lua_State *T, char *func, int nargs);
qboolean G_LuaGetFunction(lvm_t *vm, char *name);
qboolean G_LuaGetFunctionT(lua_State *T, char *name);
qboolean G_LuaStartVM(lvm_t *vm);
void G_LuaStopVM(lvm_t *vm);
void G_LuaShutdown(void);
void G_LuaStatus(gentity_t *ent);
lvm_t* G_LuaGetVM(lua_State *L);

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

/**
 *	Lua hook for InitGame event.
 *
 *	\param leveltime level time the event occured
 *	\param radomseed a random seed
 *	\param restart is this a map restart?
 */
void LuaHook_G_InitGame(int leveltime, unsigned int randomseed, int restart);

/**
 *	Lua hook for Shutdown event.
 *
 *	\param restart is this a map restart?
 */
void LuaHook_G_Shutdown(int restart);

/**
 *	Lua hook for RunFrame event.
 *
 *	\param leveltime the level time
 */
void LuaHook_G_RunFrame(int leveltime);

/**
 * Lua hook for G_Print function.
 *
 *	\param text text to be printed
 */
void LuaHook_G_Print(char* text);
/**
 * Lua hook for G_ClientPrint function.
 *
 *	\param text text to be printed
 *	\param entnum entity index for client the text gets send to
 */
void LuaHook_G_ClientPrint(char* text, int entnum);

/**
 * Lua hook for entity think function function.
 *
 *	\param function name of function to call
 *	\param entnum entity index of entity the think function was called on
 *	\return success or fail
 */
void LuaHook_G_EntityThink(char* function, int entnum);

/**
 * Lua hook for entity touch function function.
 *
 *	\param function name of function to call
 *	\param entnum entity index of entity the touch function was called on
 *	\param othernum entiy index of touching entity
 *	\return success or fail
 */
void LuaHook_G_EntityTouch(char* function, int entnum, int othernum);

/**
 * Lua hook for entity use function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the use function was called on
 * \param othernum entity index of other entity
 * \param activatornum entity index of activating entity
 */
void LuaHook_G_EntityUse(char* function, int entnum, int othernum, int activatornum);

/**
 * Lua hook for entity hurt function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the hurt function was called on
 * \param inflictornum entity index of inflictor
 * \param attackernum entity index of attacker
 */
void LuaHook_G_EntityHurt(char* function, int entnum, int inflictornum, int attackernum);

/**
 * Lua hook for entity die function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the die function was called on
 * \param inflictornum entity index of inflictor
 * \param attackernum entity index of attacker
 * \param dmg ammount of damage
 * \param mod means of death
 */
void LuaHook_G_EntityDie(char* function, int entnum, int inflictornum, int attackernum, int dmg, int mod);

/**
 * Lua hook for entity free function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the free function was called on
 */
void LuaHook_G_EntityFree(char* function, int entnum);

/**
 * Lua hook for entity trigger function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the trigger function was called on
 * \param othernum entity index of triggering entity
 */
void LuaHook_G_EntityTrigger(char* function, int entnum, int othernum);

/**
 * Lua hook for entity spawn function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the spawn function was called on
 */
void LuaHook_G_EntitySpawn(char* function, int entnum);

/**
 * Lua hook for entity reached function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the reached function was called on
 */
void LuaHook_G_EntityReached(char* function, int entnum);

/**
 * Lua hook for entity reached angular function.
 *
 * \param function name of function to call
 * \param entnum entity index of entity the reached angular function was called on
 */
void LuaHook_G_EntityReachedAngular(char* function, int entnum);

unsigned G_LuaNumThreads(void);

void G_LuaCollectGarbage(void);

#endif
