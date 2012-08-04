// cinematic lib for lua

#include "g_lua.h"
#include "g_cinematic.h"

#ifdef G_LUA

// cinematic.Activate(entity ent, entity target)
// Activates Camera on ent and points it at target.
static int Cinematic_Activate(lua_State *L) {
	lent_t *ent;
	lent_t *target;

	ent = Lua_GetEntity(L, 1);
	if(!ent) return 0;

	target = Lua_GetEntity(L, 2);
	if(!target) return 0;

	Cinematic_ActivateCameraMode(ent->e, target->e);

	return 0;
}

// cinematic.Deactivate(entity ent)
// Deactivates camera on ent.
static int Cinematic_Deactivate(lua_State *L) {
	lent_t *ent;

	ent = Lua_GetEntity(L, 1);
	if(!ent) return 0;

	Cinematic_DeactivateCameraMode(ent->e);

	return 0;
}

// cinematic.ActivateGlobal(entity target)
// Activates broadcasting of target.
static int Cinematic_ActivateGlobal(lua_State *L) {
	lent_t *target;

	target = Lua_GetEntity(L, 2);
	if(!target) return 0;

	Cinematic_ActivateGlobalCameraMode(target->e);

	return 0;
}

// cinematic.DeactivateGlobal()
// Deactivates broadcasting.
static int Cinematic_DeactivateGlobal(lua_State *L) {
	Cinematic_DeactivateGlobalCameraMode();
	return 0;
}

static const luaL_Reg lib_cinematic[] = {
	{"Activate", Cinematic_Activate},
	{"Deactivate", Cinematic_Deactivate},
	{"ActivateGlobal", Cinematic_ActivateGlobal},
	{"DeactivateGlobal", Cinematic_DeactivateGlobal},
	{NULL, NULL}
};

int Luaopen_Cinematic(lua_State *L) {
	luaL_register(L, "cinematic", lib_cinematic);
	return 1;
}
#endif //G_LUA
