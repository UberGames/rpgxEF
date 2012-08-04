// lua library for weapons

#include "g_lua.h"

#ifdef G_LUA

#include "g_weapon.h"

static int weapon_GetForward(lua_State *L) {
	Lua_PushVector(L, forward);

	return 1;
}

static int weapon_GetRight(lua_State *L) {
	Lua_PushVector(L, right);

	return 1;
}

static int weapon_GetUp(lua_State *L) {
	Lua_PushVector(L, up);

	return 1;
}

static int weapon_GetMuzzle(lua_State *L) {
	Lua_PushVector(L, muzzle);

	return 1;
}

static int weapon_Damage(lua_State *L) {
	gentity_t *target, *inflictor, *attacker;
	vec_t *dir, *point;
	int damage, dflags, mod;

	target = Lua_GetEntity(L, 1)->e;
	inflictor = Lua_GetEntity(L, 2)->e;
	attacker = Lua_GetEntity(L, 3)->e;
	dir = Lua_GetVector(L, 4);
	point = Lua_GetVector(L, 5);
	damage = (int)luaL_checknumber(L, 6);
	dflags = (int)luaL_checknumber(L, 7);
	mod = (int)luaL_checknumber(L, 8);

	G_Damage(target, inflictor, attacker, dir, point, damage, dflags, mod);

	return 0;
}

static const luaL_Reg lib_weapons[] = {
	{"GetForward", weapon_GetForward},
	{"GetRight", weapon_GetRight},
	{"GetUp", weapon_GetUp},
	{"GetMuzzle", weapon_GetMuzzle},
	{"Damage", weapon_Damage},
	{NULL, NULL}
};

int Luaopen_Weapons(lua_State *L) {
	luaL_register(L, "weapons", lib_weapons);

	return 1;
}
#endif
