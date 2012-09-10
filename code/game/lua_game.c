// game lib for lua

#include "g_lua.h"

#ifdef G_LUA
// game.Print(string text)
// Prints text to the server console
static int Game_Print(lua_State *L) {
	int		i;
	char	buf[MAX_STRING_CHARS];
	int		n = lua_gettop(L);

	memset(buf, 0, sizeof(buf));

	lua_getglobal(L, "tostring");

	LUA_DEBUG("Game_Print - start: ");

	for(i = 1; i < n; i++)
	{
		const char     *s;

		lua_pushvalue(L, -1);
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);
		s = lua_tostring(L, -1);

		if(s == NULL)
		{
			LUA_DEBUG("Game_Print - return: no string");
			return luaL_error(L, "'tostring' must return a string to 'print'");
		}

		Q_strcat(buf, sizeof(buf), s);

		lua_pop(L, 1);
	}

	G_Printf("%s\n", buf);

	LUA_DEBUG("Game_Print - return: printed string");
	return 0;
}

// game.CenterPrint(integer clientNum, string text)
// Prints text to the center of the screen of the client with client number clientNum. 
// If clientNum is -1 the text gets printed for all clients.
static int Game_CenterPrint(lua_State *L) {
	int			i;
	char		buf[MAX_STRING_CHARS];
	int			n = lua_gettop(L);
	int			clNum = luaL_checknumber(L, 1);

	memset(buf, 0, sizeof(buf));

	lua_getglobal(L, "tostring");

	LUA_DEBUG("Game_CenterPrint - start: ");

	for(i = 1; i < n; i++)
	{
		const char     *s;

		lua_pushvalue(L, -1);
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);
		s = lua_tostring(L, 2);

		if(s == NULL)
		{
			LUA_DEBUG("Game_CenterPrint - return: no string");
			return luaL_error(L, "'tostring' must return a string to 'print'");
		}

		Q_strcat(buf, sizeof(buf), s);

		lua_pop(L, 1);
	}

	
	trap_SendServerCommand(clNum, va("servercprint \"" S_COLOR_WHITE "%s\n\"", buf));

	LUA_DEBUG("Game_CenterPrint - return: printed string");
	return 0;
}

// game.ClientPrint(integer clientNum, string text)
// Prints text to the clients console that has the client number clientNum. 
// If clientNum is -1 the text gets printed to all clients consoles.
static int Game_ClientPrint(lua_State *L) {
	int			i;
	char		buf[MAX_STRING_CHARS];
	int			n = lua_gettop(L);
	int			clNum =  luaL_checknumber(L, 1);
	gentity_t	*player;

	memset(buf, 0, sizeof(buf));

	lua_getglobal(L, "tostring");

	LUA_DEBUG("Game_ClientPrint - start: ");

	for(i = 1; i < n; i++) {
		const char		*s;

		lua_pushvalue(L, -1);
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);
		s = lua_tostring(L, 2);

		if(s == NULL)
		{
			LUA_DEBUG("Game_ClientPrint - return: no string");
			return luaL_error(L, "'tostring' must return a string to 'print'");
		}

		Q_strcat(buf, sizeof(buf), s);

		lua_pop(L, 1);
	}

	if(clNum != -1) {
		player = &g_entities[clNum];
		if(player && player->client)
			G_PrintfClient(player, "%s", buf);
	} else
		G_PrintfClientAll("%s", buf);
	LUA_DEBUG("Game_ClientPrint - return: printed string");
	return 0;
}

// game.MessagePrint(integer clientNum, string text)
// Prints text to the lower right corner of the screen of the client with client number clientNum. 
// If clientNum is -1 the text gets printed for all clients.
static int Game_MessagePrint(lua_State *L) {
	int			i;
	char		buf[MAX_STRING_CHARS];
	int			n = lua_gettop(L);
	int			clNum = luaL_checknumber(L, 1);

	memset(buf, 0, sizeof(buf));

	lua_getglobal(L, "tostring");

	LUA_DEBUG("Game_MessagePrint - start: ");

	for(i = 1; i < n; i++)
	{
		const char     *s;

		lua_pushvalue(L, -1);
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);
		s = lua_tostring(L, 2);

		if(s == NULL)
		{
			LUA_DEBUG("Game_MessagePrint - return: no string");
			return luaL_error(L, "'tostring' must return a string to 'print'");
		}

		Q_strcat(buf, sizeof(buf), s);

		lua_pop(L, 1);
	}

	trap_SendServerCommand(clNum, va("servermsg \"" S_COLOR_WHITE "%s\n\"", buf));

	LUA_DEBUG("Game_MessagePrint - return: printed string");
	return 0;
}

// game.SetGlobal(string name, value)
// Sets a global Lua variable which is called name to value. 
// Creates a new global variable if a variable of name does not exist. 
// value can be of any type.
static int Game_SetGlobal(lua_State *L) {
	char *name;

	name = (char *)luaL_checkstring(L, 1);

	if(!name) return 0;

	lua_pushvalue(L, 2);
	lua_setglobal(L, name);

	return 0;
}

// game.GetGlobal(string name)
// Returns the value of the global variable name. 
// Returns nil if the variable does not exist.
static int Game_GetGlobal(lua_State *L) {
	char *name;

	name = (char *)luaL_checkstring(L, 1);

	if(!name) return 0;

	lua_getglobal(L, name);

	return 1;
}

// Alert-Stuff... I don't know. I feel like removing this and stick to the entity spawning and setup.
// game.AlertSetup(entity ent, string greentarget, string yellowtarget, string redtarget, string bluetarget,
//					string greensound, string yellowsound, string redsound, string bluesound, integer mode)
static int Game_AlertSetup(lua_State *L) {

	if(luaAlertState) {
		lua_pushboolean(L, 0);
		return 1;
	}

	luaAlertState = (luaAlertState_t *)malloc(sizeof(luaAlertState_t));
	if(!luaAlertState) {
		lua_pushboolean(L, 0);
		return 1;
	}

	luaAlertState->targets[0] = (char *)luaL_checkstring(L, 1);
	luaAlertState->targets[1] = (char *)luaL_checkstring(L, 2);
	luaAlertState->targets[2] = (char *)luaL_checkstring(L, 3);
	luaAlertState->targets[3] = (char *)luaL_checkstring(L, 4);

	luaAlertState->sounds[0] = (char *)luaL_checkstring(L, 5);
	luaAlertState->sounds[1] = (char *)luaL_checkstring(L, 6);
	luaAlertState->sounds[2] = (char *)luaL_checkstring(L, 7);
	luaAlertState->sounds[3] = (char *)luaL_checkstring(L, 8);

	luaAlertState->mode = luaL_checkint(L, 9);

	luaAlertState->cond = 0;

	lua_pushboolean(L, 1);
	return 1;
}

static int Game_AlertAddShader(lua_State *L) {
	int cond;
	char *shader;

	if(!luaAlertState) {
		lua_pushboolean(L, 0);
		return 1;
	}

	cond = luaL_checkint(L, 1);
	if(cond < 0 || cond > 3) {
		lua_pushboolean(L, 0);
		return 1;
	}

	shader = (char *)luaL_checkstring(L, 2);
	if(!shader || (strlen(shader) + 1) > MAX_QPATH) {
		lua_pushboolean(L, 0);
		return 1;
	}

	if(!luaAlertState->shaders[cond]) {
		luaAlertState->shaders[cond] = (char *)malloc(sizeof(char) * (strlen(shader) + 1));
		if(!luaAlertState->shaders[cond]) {
			lua_pushboolean(L, 0);
			return 1;
		}
		strncpy(luaAlertState->shaders[cond], shader, sizeof(luaAlertState->shaders[cond]));
	} else {
		void *tmp = realloc(luaAlertState->shaders[cond], sizeof(char) * (strlen(luaAlertState->shaders[cond]) +
												strlen(shader) + 1));
		if(!tmp){
			lua_pushboolean(L, 0);
			return 1;
		}
		luaAlertState->shaders[cond] = tmp;
		Com_sprintf(luaAlertState->shaders[cond], sizeof(luaAlertState->shaders[cond]), "%s\n%s", luaAlertState->shaders[cond], shader);
	}

	lua_pushboolean(L, 1);
	return 1;
}

// game.Alert(entity ent, integer target, boolean silent)
static int Game_Alert(lua_State *L) {
	/*lent_t *lent;
	int current, target;
	qboolean silent;*/

	return 0;
}

// game.LevelTime()
// Returns the current level time in milliseconds.
static int Game_Leveltime(lua_State * L)
{
	lua_pushinteger(L, level.time);

	LUA_DEBUG("Game_Leveltime - start/return: leveltime=%d", level.time);
	return 1;
}

static int Game_Damage(lua_State *L) {
	lent_t *lent;
	gentity_t *targ = NULL, *inflictor = NULL, *attacker = NULL;
	vec_t *dir = NULL, *point = NULL;
	int damage = 0, dflags = 0, mod = 0;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) return 1;
	targ = lent->e;
	if(!lua_isnil(L, 2)) {
		lent = Lua_GetEntity(L, 2);
		if(lent && lent->e)
			inflictor = lent->e;
	}
	if(!lua_isnil(L, 3)) {
		lent = Lua_GetEntity(L, 3);
		if(lent && lent->e)
			attacker = lent->e;
	}
	if(!lua_isnil(L, 4))
		dir = Lua_GetVector(L, 4);
	if(!lua_isnil(L, 5))
		point = Lua_GetVector(L, 5);
	damage = (int)luaL_checknumber(L, 6);
	dflags = (int)luaL_checknumber(L, 7);
	mod = (int)luaL_checknumber(L, 8);

	G_Damage(targ, inflictor, attacker, dir, point, damage, dflags, mod);

	return 1;
}

static int Game_Repair(lua_State *L) {
	lent_t *lent;
	float rate;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) return 1;
	
	rate = (float)luaL_checknumber(L, 2);

	G_Repair(lent->e, NULL, rate); // FIXME ... trance ent?

	return 1;
}

static const luaL_Reg lib_game[] = {
	{"Damage", Game_Damage},
	{"Repair", Game_Repair},
	{"Print", Game_Print},
	{"MessagePrint", Game_MessagePrint},
	{"CenterPrint", Game_CenterPrint},
	{"ClientPrint", Game_ClientPrint},
	{"GetTime", Game_Leveltime},

	{"AlertSetup", Game_AlertSetup},
	{"AlertAddShader", Game_AlertAddShader},
	{"Alert", Game_Alert},

	{"SetGlobal", Game_SetGlobal},
	{"GetGlobal", Game_GetGlobal},
	{NULL, NULL}
};

int Luaopen_Game(lua_State *L) {
	luaL_register(L, "game", lib_game);
	lua_pushliteral(L, "_GAMEVERSION");
	lua_pushliteral(L, GAMEVERSION);

	return 1;
}
#endif
