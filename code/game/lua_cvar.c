// lua library for cvars

#include "g_lua.h"
#include <string.h>

#ifdef G_LUA


static int Cvar_Integer(lua_State *L) {
	char *cvar;
	char buf[1024];

	cvar = (char *)luaL_checkstring(L, 1);
	if(!cvar[0]) {
		lua_pushinteger(L, 0);
		return 1;
	}
	if(strstr(Q_strlwr(cvar), "password") != NULL
		|| strstr(Q_strlwr(cvar), "pass") != NULL
		|| strstr(Q_strlwr(cvar), "sql") != NULL) {
			lua_pushinteger(L, 0);
			return 1;
	}
	trap_Cvar_VariableStringBuffer(cvar, buf, sizeof(buf));
	lua_pushinteger(L, atoi(buf));

	return 1;
}

static int Cvar_Value(lua_State *L) {
	char *cvar;
	char buf[1024];

	cvar = (char *)luaL_checkstring(L, 1);
	if(!cvar[0]) {
		lua_pushnumber(L, 0);
		return 1;
	}
	if(strstr(Q_strlwr(cvar), "password") != NULL
		|| strstr(Q_strlwr(cvar), "pass") != NULL
		|| strstr(Q_strlwr(cvar), "sql") != NULL) {
			lua_pushnumber(L, 0);
			return 1;
	}
	trap_Cvar_VariableStringBuffer(cvar, buf, sizeof(buf));
	lua_pushnumber(L, atof(buf));

	return 1;
}

static int Cvar_String(lua_State *L) {
	char *cvar;
	char buf[1024];

	cvar = (char *)luaL_checkstring(L, 1);
	if(!cvar[0]) {
		lua_pushstring(L, "");
		return 1;
	}
	if(strstr(Q_strlwr(cvar), "password") != NULL
		|| strstr(Q_strlwr(cvar), "pass") != NULL
		|| strstr(Q_strlwr(cvar), "sql") != NULL) {
			lua_pushstring(L, "");
			return 1;
	}
	trap_Cvar_VariableStringBuffer(cvar, buf, sizeof(buf));
	lua_pushstring(L, buf);

	return 1;
}

static int Cvar_rpg_phaserdmg(lua_State *L) {
	lua_pushnumber(L, rpg_phaserdmg.integer);

	return 1;
}

static const luaL_Reg lib_cvar[] = {
	{"Integer", Cvar_Integer},
	{"Value", Cvar_Value},
	{"String", Cvar_String},
	{"rpg_phaserdmg", Cvar_rpg_phaserdmg},
	{NULL, NULL}
};

int Luaopen_Cvar(lua_State *L) {
	luaL_register(L, "cvar", lib_cvar);

	return 1;
}
#endif