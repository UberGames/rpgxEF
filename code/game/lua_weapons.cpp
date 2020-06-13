// lua library for weapons

#include "g_lua.h"

#ifdef G_LUA

/***
A module for everything converning weapons.
@module weapons
*/

#include "g_weapon.h"

/***
Get the forward vector.
@function GetForward
@return Forward vector.
*/
static int weapon_GetForward(lua_State *L) {
  Lua_PushVector(L, forward);

  return 1;
}

/***
Get the right vector.
@function GetRight.
@return Right vector.
*/
static int weapon_GetRight(lua_State *L) {
  Lua_PushVector(L, right);

  return 1;
}

/***
Get the up vector.
@function GetUp
@return Up vector.
*/
static int weapon_GetUp(lua_State *L) {
  Lua_PushVector(L, up);

  return 1;
}

/***
Get the muzzle point.
@function GetMuzzle
@return Muzzle point.
*/
static int weapon_GetMuzzle(lua_State *L) {
  Lua_PushVector(L, muzzle);

  return 1;
}

/***
Do damage to an entity.
@function Damage
@param target Target entity.
@param inflictor Inflicting entity. Can be nil.
@param attacker Attacking entity. Can be nil.
@param dir Direction for knockback. Can be nil.
@param point Point. Can be nil.
@param damage Ammount of damage.
@param dflags Damage flags.
@param mod Means of death.
@return Success or failure.
*/
static int weapon_Damage(lua_State *L) {
  lent_t *lent;
  gentity_t *target = NULL;
  gentity_t *inflictor = NULL;
  gentity_t *attacker = NULL;
  vec_t *dir, *point;
  int damage, dflags, mod;

  lent = Lua_GetEntity(L, 1);
  if (lent == NULL || lent->e == NULL) {
    lua_pushboolean(L, qfalse);
    return 0;
  }
  target = Lua_GetEntity(L, 1)->e;

  lent = Lua_GetEntity(L, 2);
  if (lent != NULL && lent->e != NULL) {
    inflictor = lent->e;
  }

  lent = Lua_GetEntity(L, 3);
  if (lent != NULL && lent->e != NULL) {
    attacker = lent->e;
  }

  dir = Lua_GetVector(L, 4);
  point = Lua_GetVector(L, 5);
  damage = (int) luaL_checknumber(L, 6);
  dflags = (int) luaL_checknumber(L, 7);
  mod = (int) luaL_checknumber(L, 8);

  G_Damage(target, inflictor, attacker, dir, point, damage, dflags, mod);

  lua_pushboolean(L, qtrue);
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
  lua_newtable(L);
  luaL_setfuncs(L, lib_weapons, 0);
  lua_pushvalue(L, -1);
  lua_setglobal(L, "weapons");
  return 1;
}
#endif
