#include "cg_lua.h"

#ifdef CG_LUA
static int Cent_GC(lua_State * L)
{

	return 0;
}

static int Cent_ToString(lua_State * L)
{
	cent_t     *cent;
	centity_t  *ent;
	char            buf[MAX_STRING_CHARS];

	cent = Lua_GetCent(L, 1);
	ent = cent->e;
	Com_sprintf(buf, sizeof(buf), "centity: id=%d pointer=%p\n", ent - cg_entities, ent);
	lua_pushstring(L, buf);

	return 1;
}

static const luaL_Reg Centity_ctor[] = {
	{NULL,						NULL}
};

static const luaL_Reg Centity_meta[] = {
	{"__gc",						Cent_GC},
	{"__tostring",					Cent_ToString},

	{NULL, NULL}
};

/*void dummy(gentity_t *ent) {
	ent->timestamp;
}*/

int Luaopen_Cent(lua_State * L)
{
	luaL_newmetatable(L, "cgame.centity");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	luaL_register(L, NULL, Centity_meta);
	luaL_register(L, "centity", Centity_ctor);

	return 1;
}

void Lua_PushCent(lua_State * L, centity_t * ent)
{
	cent_t     *cent;

	if(!ent)
		lua_pushnil(L);
	else {
		cent = (cent_t *)lua_newuserdata(L, sizeof(cent_t));
		luaL_getmetatable(L, "cgame.centity");
		lua_setmetatable(L, -2);
		cent->e = ent;
	}
}

cent_t *Lua_GetCent(lua_State * L, int argNum)
{
	void           *ud;

	ud = luaL_checkudata(L, argNum, "cgame.centity");
	luaL_argcheck(L, ud != NULL, argNum, "\'centity\' expected");
	return (cent_t *) ud;
}
#endif
