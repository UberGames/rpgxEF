// g_lua.c

#include "g_lua.h"

#ifdef G_LUA

lvm_t       *lVM[NUM_VMS];

void QDECL LUA_DEBUG(const char *fmt, ...)
{
	va_list         argptr;
	char            text[1024];

	if(g_debugLua.integer >= 1)
	{
		va_start(argptr, fmt);
		Com_sprintf(text, sizeof(text), fmt, argptr);
		va_end(argptr);
		G_Printf(S_COLOR_YELLOW "LUA DEBUG:" S_COLOR_WHITE " %s\n", text);
	}
}

void QDECL LUA_LOG(const char *fmt, ...)
{
	va_list         argptr;
	char            buff[1024], string[1024];
	int             min, tens, sec;

	va_start(argptr, fmt);
	Com_sprintf(buff, sizeof(buff), fmt, argptr);
	va_end(argptr);

	if(g_dedicated.integer)
	{
		trap_Printf(buff);
	}

	if(level.logFile)
	{
		sec = level.time / 1000;
		min = sec / 60;
		sec -= min * 60;
		tens = sec / 10;
		sec -= tens * 10;

		Com_sprintf(string, sizeof(string), "%i:%i%i %s", min, tens, sec, buff);

		trap_FS_Write(string, strlen(string), level.logFile);
	}
}

qboolean LoadLuaFile(char *path, int num_vm)
{
	int             flen = 0;
	char           *code;
	fileHandle_t    f;
	lvm_t	       *vm;

	flen = trap_FS_FOpenFile(path, &f, FS_READ);
	if(flen < 0)
	{
		LUA_LOG("Lua: can not open file %s\n", path);
		G_Printf(S_COLOR_YELLOW "Lua: can not open file %s\n", path);
		trap_FS_FCloseFile(f);
		return qfalse;
	}
	/* GSIO01: testing unlimited length for Lua files */
	/*else if(flen > MAX_FSIZE)
	{
		LUA_LOG("Lua: ignoring file %s (too big)\n", path);
		G_Printf(S_COLOR_YELLOW "Lua: ignoring file %s (too big)\n", path);
		trap_FS_FCloseFile(f);
		return qfalse;
	}*/
	else
	{
		code = (char *)malloc(flen + 1);
		if(!code) return qfalse;
		trap_FS_Read(code, flen, f);
		*(code + flen) = '\0';
		trap_FS_FCloseFile(f);

		vm = (lvm_t *) malloc(sizeof(lvm_t));
		if(vm == NULL)
		{
			LUA_LOG("Lua: failed to allocate memory for lua VM\n");
			G_Printf(S_COLOR_YELLOW "Lua: failed to allocate memory for lua VM\n");
			return qfalse;
		}
		memset(vm, 0, sizeof(lvm_t));
		vm->id = -1;
		Q_strncpyz(vm->filename, path, sizeof(vm->filename));
		vm->code = code;
		vm->code_size = flen;
		vm->error = 0;

		if(G_LuaStartVM(vm) == qfalse)
		{
			G_LuaStopVM(vm);
			vm = NULL;
			return qfalse;
		}
		else
		{
			vm->id = num_vm;
			lVM[num_vm] = vm;
			return qtrue;
		}
	}
	//return qfalse;
}

qboolean G_LuaInit()
{
	int             i, /*len,*/ num_vm = 0;
	char            buff[128]; //, *crt;

	int             numdirs;
	int             numFiles;
	char            filename[128];
	char            dirlist[1024];
	char           *dirptr;
	int             dirlen;

	G_Printf("------- G_LuaInit -------\n");

	numFiles = 0;
	trap_Cvar_VariableStringBuffer("mapname", buff, sizeof(buff));
	sprintf(filename, "scripts/lua/%s/", buff);
	numdirs = trap_FS_GetFileList(filename, ".lua", dirlist, 1024);
	dirptr = dirlist;
	for(i = 0; i < numdirs; i++, dirptr += dirlen + 1)
	{
		dirlen = strlen(dirptr);
		strcpy(filename, "scripts/lua/");
		strcat(filename, buff);
		strcat(filename, "/");
		strcat(filename, dirptr);
		numFiles++;

		if(LoadLuaFile(filename, num_vm))
		{
			num_vm++;
			G_Printf("  %s Loaded\n", filename);
		}
		else
		{
			G_Printf("  %s Failed\n", filename);
		}

		if(num_vm >= NUM_VMS)
		{
			LUA_LOG("Lua: too many lua files specified, only the first %d have been loaded\n", NUM_VMS);
			G_Printf(S_COLOR_YELLOW "Lua: too many lua files specified, only the first %d have been loaded\n", NUM_VMS);
			break;
		}
	}

	Com_Printf("%i map files parsed\n", numFiles);

	G_Printf("------- G_LuaInit Finish -------\n");

	return qtrue;
}

qboolean G_LuaResume(lvm_t *vm, lua_State *T, char *func, int nargs) {
	int res = lua_resume(T, nargs);

	if(res == LUA_ERRRUN) {
		LUA_LOG("Lua: %s error running lua script: %s\n", func, lua_tostring(T, -1));
		G_Printf(S_COLOR_YELLOW "Lua: %s error running lua script: %s\n", func, lua_tostring(T, -1));
		lua_pop(T, 1);
		vm->error++;
		return qfalse;
	} else if(res == LUA_ERRMEM) {
		LUA_LOG("Lua: memory allocation error #2 ( %s )\n", vm->filename);
		vm->error++;
		return qfalse;
	} else if(res == LUA_ERRERR) {
		LUA_LOG("Lua: traceback error ( %s )\n", vm->filename);
		G_Printf(S_COLOR_YELLOW "Lua: traceback error ( %s )\n", vm->filename);
		vm->error++;
		return qfalse;
	} 
	return qtrue;
}

qboolean G_LuaCall(lvm_t * vm, char *func, int nargs, int nresults)
{
	int             res = lua_pcall(vm->L, nargs, nresults, 0);

	if(res == LUA_ERRRUN)
	{
		LUA_LOG("Lua: %s error running lua script: %s\n", func, lua_tostring(vm->L, -1));
		G_Printf(S_COLOR_YELLOW "Lua: %s error running lua script: %s\n", func, lua_tostring(vm->L, -1));
		lua_pop(vm->L, 1);
		vm->error++;
		return qfalse;
	}
	else if(res == LUA_ERRMEM)
	{
		LUA_LOG("Lua: memory allocation error #2 ( %s )\n", vm->filename);
		vm->error++;
		return qfalse;
	}
	else if(res == LUA_ERRERR)
	{
		LUA_LOG("Lua: traceback error ( %s )\n", vm->filename);
		G_Printf(S_COLOR_YELLOW "Lua: traceback error ( %s )\n", vm->filename);
		vm->error++;
		return qfalse;
	}
	return qtrue;
}

#define SAY_ALL		0
#define SAY_TEAM	1

qboolean G_LuaGetFunctionT(lua_State *T, char *name)
{
	if(T)
	{
		lua_getglobal(T, name);
		if(lua_isfunction(T, -1))
		{
			return qtrue;
		}
		else
		{
			lua_pop(T, 1);
			return qfalse;
		}
	}
	return qfalse;
}

qboolean G_LuaGetFunction(lvm_t * vm, char *name)
{
	if(vm->L)
	{
		lua_getglobal(vm->L, name);
		if(lua_isfunction(vm->L, -1))
		{
			return qtrue;
		}
		else
		{
			lua_pop(vm->L, 1);
			return qfalse;
		}
	}
	return qfalse;
}

qboolean G_LuaStartVM(lvm_t * vm)
{
	int             res = 0;
	char            homepath[MAX_QPATH], gamepath[MAX_QPATH];

	vm->L = luaL_newstate();
	if(!vm->L)
	{
		LUA_LOG("Lua: Lua failed to initialise.\n");
		return qfalse;
	}

	luaL_openlibs(vm->L);

	trap_Cvar_VariableStringBuffer("fs_homepath", homepath, sizeof(homepath));
	trap_Cvar_VariableStringBuffer("fs_game", gamepath, sizeof(gamepath));

	lua_getglobal(vm->L, LUA_LOADLIBNAME);
	if(lua_istable(vm->L, -1))
	{
		lua_pushstring(vm->L, va("%s%s%s%s?.lua;%s%s%s%slualib%slua%s?.lua",
								 homepath, LUA_DIRSEP, gamepath, LUA_DIRSEP,
								 homepath, LUA_DIRSEP, gamepath, LUA_DIRSEP, LUA_DIRSEP, LUA_DIRSEP));
		lua_setfield(vm->L, -2, "path");
		lua_pushstring(vm->L, va("%s%s%s%s?.%s;%s%s%s%slualib%sclibs%s?.%s",
								 homepath, LUA_DIRSEP, gamepath, LUA_DIRSEP, EXTENSION,
								 homepath, LUA_DIRSEP, gamepath, LUA_DIRSEP, LUA_DIRSEP, LUA_DIRSEP, EXTENSION));
		lua_setfield(vm->L, -2, "cpath");
	}
	lua_pop(vm->L, 1);

	Lua_RegisterGlobal(vm->L, "LUA_PATH", va("%s%s%s%s?.lua;%s%s%s%slualib%slua%s?.lua",
											 homepath, LUA_DIRSEP, gamepath, LUA_DIRSEP,
											 homepath, LUA_DIRSEP, gamepath, LUA_DIRSEP, LUA_DIRSEP, LUA_DIRSEP));
	Lua_RegisterGlobal(vm->L, "LUA_CPATH", va("%s%s%s%s?.%s;%s%s%s%slualib%sclibs%s?.%s",
											  homepath, LUA_DIRSEP, gamepath, LUA_DIRSEP, EXTENSION,
											  homepath, LUA_DIRSEP, gamepath, LUA_DIRSEP, LUA_DIRSEP, LUA_DIRSEP, EXTENSION));
	Lua_RegisterGlobal(vm->L, "LUA_DIRSEP", LUA_DIRSEP);

	lua_newtable(vm->L);
	Lua_RegConstInteger(vm->L, CS_PLAYERS);
	Lua_RegConstInteger(vm->L, EXEC_NOW);
	Lua_RegConstInteger(vm->L, EXEC_INSERT);
	Lua_RegConstInteger(vm->L, EXEC_APPEND);
	Lua_RegConstInteger(vm->L, FS_READ);
	Lua_RegConstInteger(vm->L, FS_WRITE);
	Lua_RegConstInteger(vm->L, FS_APPEND);
	Lua_RegConstInteger(vm->L, FS_APPEND_SYNC);
	Lua_RegConstInteger(vm->L, SAY_ALL);
	Lua_RegConstInteger(vm->L, SAY_TEAM);
	Lua_RegConstString(vm->L, HOSTARCH);

	luaopen_base(vm->L);
	luaopen_string(vm->L);
	luaopen_coroutine(vm->L);
	Luaopen_Game(vm->L);
	Luaopen_Qmath(vm->L);
	Luaopen_Mover(vm->L);
	Luaopen_Vector(vm->L);
	Luaopen_Entity(vm->L);
	Luaopen_Cinematic(vm->L);
	Luaopen_Sound(vm->L);
	Luaopen_Trace(vm->L);

	res = luaL_loadbuffer(vm->L, vm->code, vm->code_size, vm->filename);
	if(res == LUA_ERRSYNTAX)
	{
		LUA_LOG("Lua: syntax error during pre-compilation: %s\n", (char *)lua_tostring(vm->L, -1));
		G_Printf(S_COLOR_YELLOW "Lua: syntax error: %s\n", (char *)lua_tostring(vm->L, -1));
		lua_pop(vm->L, 1);
		vm->error++;
		return qfalse;
	}
	else if(res == LUA_ERRMEM)
	{
		LUA_LOG("Lua: memory allocation error #1 ( %s )\n", vm->filename);
		vm->error++;
		return qfalse;
	}

	if(!G_LuaCall(vm, "G_LuaStartVM", 0, 0))
		return qfalse;

	LUA_LOG("Lua: Loading %s\n", vm->filename);
	return qtrue;
}

void G_LuaStopVM(lvm_t * vm)
{
	if(vm == NULL)
		return;
	if(vm->code != NULL)
	{
		free(vm->code);
		vm->code = NULL;
	}
	if(vm->id >= 0)
	{
		if(lVM[vm->id] == vm)
			lVM[vm->id] = NULL;
		if(!vm->error)
		{
			LUA_LOG("Lua: Lua module [%s] unloaded.\n", vm->filename);
		}
	}
	free(vm);
}

void G_LuaShutdown()
{
	int             i;
	lvm_t	       *vm;

	for(i = 0; i < NUM_VMS; i++)
	{
		vm = lVM[i];
		if(vm)
		{
			G_LuaStopVM(vm);
		}
	}
}

void G_LuaStatus(gentity_t * ent)
{
	int             i, cnt = 0;

	for(i = 0; i < NUM_VMS; i++)
		if(lVM[i])
			cnt++;

	if(ent)
	{
		if(cnt == 0)
		{
			G_PrintfClient(ent, "Lua: no scripts loaded.");
			return;
		}
		else if(cnt == 1)
		{
			G_PrintfClient(ent, "Lua: showing lua information ( 1 module loaded )");
		}
		else
		{
			G_PrintfClient(ent, "Lua: showing lua information ( %d modules loaded )", cnt);
		}
		G_PrintfClient(ent, "%-2s %-24s", "VM","Filename");
		G_PrintfClient(ent, "-- ------------------------");
		for(i = 0; i < NUM_VMS; i++)
		{
			if(lVM[i])
			{
				G_PrintfClient(ent, "%2d %-24s", lVM[i]->id, lVM[i]->filename);
			}
		}
		G_PrintfClient(ent, "-- ------------------------");
	}
	else
	{
		if(cnt == 0)
		{
			G_Printf("Lua: no scripts loaded.\n");
			return;
		}
		else if(cnt == 1)
		{
			G_Printf("Lua: showing lua information ( 1 module loaded )\n");
		}
		else
		{
			G_Printf("Lua: showing lua information ( %d modules loaded )\n", cnt);
		}
		G_Printf("%-2s %-24s\n", "VM", "Filename");
		G_Printf("-- ------------------------\n");
		for(i = 0; i < NUM_VMS; i++)
		{
			if(lVM[i])
			{
				G_Printf("%2d %-24s\n", lVM[i]->id, lVM[i]->filename);
			}
		}
		G_Printf("-- ------------------------\n");
	}

}

lvm_t       *G_LuaGetVM(lua_State * L)
{
	int             i;

	for(i = 0; i < NUM_VMS; i++)
		if(lVM[i] && lVM[i]->L == L)
			return lVM[i];
	return NULL;
}

void LuaHook_G_InitGame(int levelTime, int randomSeed, int restart)
{
	int             i;
	lvm_t	       *vm;

	for(i = 0; i < NUM_VMS; i++)
	{
		vm = lVM[i];
		if(vm)
		{
			if(vm->id < 0 )
				continue;
			if(!G_LuaGetFunction(vm, "InitGame"))
				continue;
			lua_pushinteger(vm->L, levelTime);
			lua_pushinteger(vm->L, randomSeed);
			lua_pushinteger(vm->L, restart);
			if(!G_LuaCall(vm, "InitGame", 3, 0))
			{
				continue;
			}
		}
	}
}

void LuaHook_G_Shutdown(int restart)
{
	int             i;
	lvm_t	       *vm;

	for(i = 0; i < NUM_VMS; i++)
	{
		vm = lVM[i];
		if(vm)
		{
			if(vm->id < 0 )
				continue;
			if(!G_LuaGetFunction(vm, "ShutdownGame"))
				continue;
			lua_pushinteger(vm->L, restart);
			if(!G_LuaCall(vm, "ShutdownGame", 1, 0))
			{
				continue;
			}
		}
	}
}

void LuaHook_G_RunFrame(int levelTime)
{
	int             i;
	lvm_t       *vm;

	for(i = 0; i < NUM_VMS; i++)
	{
		vm = lVM[i];
		if(vm)
		{
			if(vm->id < 0 )
				continue;
			if(!G_LuaGetFunction(vm, "RunFrame"))
				continue;
			lua_pushinteger(vm->L, levelTime);
			if(!G_LuaCall(vm, "RunFrame", 1, 0))
			{
				continue;
			}
		}
	}
}

void LuaHook_G_ClientPrint(char *text, int entnum) {
	int			i;
	lvm_t		*vm;
	gentity_t	*ent;

	if(!entnum) return;

	for(i = 0; i < NUM_VMS; i++) {
		vm = lVM[i];
		if(vm) {
			if(vm->id < 0)
				continue;
			if(!G_LuaGetFunction(vm, "GClientPrint"))
				continue;
			lua_pushstring(vm->L, text);
			ent = &g_entities[entnum];
			if(!ent || !ent->inuse)
				lua_pushnil(vm->L);
			else
				Lua_PushEntity(vm->L, ent);

			if(!G_LuaCall(vm, "GClientPrint", 2, 0))
				continue;
		}
	}
}

void LuaHook_G_Print(char *text)
{
	int         i;
	lvm_t       *vm;

	for(i = 0; i < NUM_VMS; i++)
	{
		vm = lVM[i];
		if(vm)
		{
			if(vm->id < 0 )
				continue;
			if(!G_LuaGetFunction(vm, "GPrint"))
				continue;
			lua_pushstring(vm->L, text);
			if(!G_LuaCall(vm, "GPrint", 1, 0))
			{
				continue;
			}
		}
	}
}

qboolean LuaHook_G_EntityThink(char *function, int entnum)
{
	int         i;
	lvm_t       *vm;
	lua_State	*t;
	gentity_t	*ent;

	for(i = 0; i < NUM_VMS; i++)
	{
		vm = lVM[i];
		if(vm)
		{
			if(vm->id < 0 )
				continue;
			t = lua_newthread(vm->L);
			if(!t) {
				if(!G_LuaGetFunction(vm, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(vm->L);
				else
					Lua_PushEntity(vm->L, ent);
				if(!G_LuaCall(vm, function, 1, 0))
				{
					continue;
				}
			} else {
				if(!G_LuaGetFunctionT(t, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(t);
				else
					Lua_PushEntity(t, ent);
				if(!G_LuaResume(vm, t, function, 1))
					continue;
			}
		}
	}
	return qfalse;
}

qboolean LuaHook_G_EntityTouch(char *function, int entnum, int othernum)
{
	int         i;
	lvm_t       *vm;
	lua_State	*t;
	gentity_t	*ent;
	gentity_t	*other;

	for(i = 0; i < NUM_VMS; i++)
	{
		vm = lVM[i];
		if(vm)
		{
			if(vm->id < 0)
				continue;
			t = lua_newthread(vm->L);
			if(!t) {
				if(!G_LuaGetFunction(vm, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(vm->L);
				else
					Lua_PushEntity(vm->L, ent);
				other = &g_entities[othernum];
				if(!other || !other->inuse)
					lua_pushnil(vm->L);
				else
					Lua_PushEntity(vm->L, other);
				if(!G_LuaCall(vm, function, 2, 0))
				{
					continue;
				}
			} else {
				if(!G_LuaGetFunctionT(t, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(t);
				else
					Lua_PushEntity(t, ent);
				other = &g_entities[othernum];
				if(!other || !other->inuse)
					lua_pushnil(t);
				else
					Lua_PushEntity(t, other);
				if(!G_LuaResume(vm, t, function, 2))
					continue;
			}
		}
	}
	return qfalse;
}

qboolean LuaHook_G_EntityUse(char *function, int entnum, int othernum, int activatornum)
{
	int         i;
	lvm_t       *vm;
	lua_State	*t;
	gentity_t	*ent;
	gentity_t	*other;
	gentity_t	*activator;

	for(i = 0; i < NUM_VMS; i++)
	{
		vm = lVM[i];
		if(vm)
		{
			if(vm->id < 0)
				continue;
			t = lua_newthread(vm->L);
			if(!t) {
				if(!G_LuaGetFunction(vm, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(vm->L);
				else
					Lua_PushEntity(vm->L, ent);
				other = &g_entities[othernum];
				if(!other || !other->inuse)
					lua_pushnil(vm->L);
				else
					Lua_PushEntity(vm->L, other);
				activator = &g_entities[activatornum];
				if(!activator || !activator->inuse)
					lua_pushnil(vm->L);
				else
					Lua_PushEntity(vm->L, activator);
				if(!G_LuaCall(vm, function, 3, 0))
				{
					continue;
				}
			} else {
				if(!G_LuaGetFunctionT(t, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(t);
				else
					Lua_PushEntity(t, ent);
				other = &g_entities[othernum];
				if(!other || !other->inuse)
					lua_pushnil(t);
				else
					Lua_PushEntity(t, other);
				activator = &g_entities[activatornum];
				if(!activator || !activator->inuse)
					lua_pushnil(t);
				else
					Lua_PushEntity(t, activator);
				G_LuaResume(vm, t, function, 3);
			}
		}
	}
	return qfalse;
}

qboolean LuaHook_G_EntityHurt(char *function, int entnum, int inflictornum, int attackernum)
{
	int         i;
	lvm_t       *vm;
	lua_State	*t;
	gentity_t	*ent;
	gentity_t	*inflictor;
	gentity_t	*attacker;

	for(i = 0; i < NUM_VMS; i++)
	{
		vm = lVM[i];
		if(vm)
		{
			if(vm->id < 0)
				continue;
			t = lua_newthread(vm->L);
			if(!t) {
				if(!G_LuaGetFunction(vm, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(vm->L);
				else
					Lua_PushEntity(vm->L, ent);
				inflictor = &g_entities[inflictornum];
				if(!inflictor || !inflictor->inuse)
					lua_pushnil(vm->L);
				else
					Lua_PushEntity(vm->L, inflictor);
				attacker = &g_entities[attackernum];
				if(!attacker || !attacker->inuse)
					lua_pushnil(vm->L);
				else
					Lua_PushEntity(vm->L, attacker);
				if(!G_LuaCall(vm, function, 3, 0))
				{
					continue;
				}
			} else {
				if(!G_LuaGetFunctionT(t, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(t);
				else
					Lua_PushEntity(t, ent);
				inflictor = &g_entities[inflictornum];
				if(!inflictor || !inflictor->inuse)
					lua_pushnil(t);
				else
					Lua_PushEntity(t, inflictor);
				attacker = &g_entities[attackernum];
				if(!attacker || !attacker->inuse)
					lua_pushnil(t);
				else
					Lua_PushEntity(t, attacker);
				if(!G_LuaResume(vm, t, function, 3))
					continue;
			}
		}
	}
	return qfalse;
}

qboolean LuaHook_G_EntityDie(char *function, int entnum, int inflictornum, int attackernum, int dmg, int mod)
{
	int             i;
	lvm_t			*vm;
	lua_State		*t;
	gentity_t	*ent;
	gentity_t	*inflictor;
	gentity_t	*attacker;

	for(i = 0; i < NUM_VMS; i++)
	{
		vm = lVM[i];
		if(vm)
		{
			if(vm->id < 0)
				continue;
			t = lua_newthread(vm->L);
			if(!t) {
				if(!G_LuaGetFunction(vm, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(vm->L);
				else
					Lua_PushEntity(vm->L, ent);
				inflictor = &g_entities[inflictornum];
				if(!inflictor || !inflictor->inuse)
					lua_pushnil(vm->L);
				else
					Lua_PushEntity(vm->L, inflictor);
				attacker = &g_entities[attackernum];
				if(!attacker || !attacker->inuse)
					lua_pushnil(vm->L);
				else
					Lua_PushEntity(vm->L, attacker);
				lua_pushinteger(vm->L, dmg);
				lua_pushinteger(vm->L, mod);
				if(!G_LuaCall(vm, function, 5, 0))
				{
					continue;
				}
			} else {
				if(!G_LuaGetFunctionT(t, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(t);
				else
					Lua_PushEntity(t, ent);
				inflictor = &g_entities[inflictornum];
				if(!inflictor || !inflictor->inuse)
					lua_pushnil(t);
				else
					Lua_PushEntity(t, inflictor);
				attacker = &g_entities[attackernum];
				if(!attacker || !attacker->inuse)
					lua_pushnil(t);
				else
					Lua_PushEntity(t, attacker);
				lua_pushinteger(t, dmg);
				lua_pushinteger(t, mod);
				if(!G_LuaResume(vm, t, function, 5))
					continue;
			}
		}
	}
	return qfalse;
}

qboolean LuaHook_G_EntityFree(char *function, int entnum)
{
	int         i;
	lvm_t       *vm;
	lua_State	*t;
	gentity_t	*ent;

	for(i = 0; i < NUM_VMS; i++)
	{
		vm = lVM[i];
		if(vm)
		{
			if(vm->id < 0)
				continue;
			t = lua_newthread(vm->L);
			if(!t) {
				if(!G_LuaGetFunction(vm, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(vm->L);
				else
					Lua_PushEntity(vm->L, ent);
				if(!G_LuaCall(vm, function, 1, 0))
				{
					continue;
				}
			} else {
				if(!G_LuaGetFunctionT(t, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(t);
				else
					Lua_PushEntity(t, ent);
				if(!G_LuaResume(vm, t, function, 1))
					continue;
			}
		}
	}
	return qfalse;
}

qboolean LuaHook_G_EntityReached(char *function, int entnum) {
	int			i;
	lvm_t		*vm;
	lua_State	*t;
	gentity_t	*ent;

	for(i = 0; i < NUM_VMS; i++) {
		vm = lVM[i];
		if(vm) {
			if(vm->id < 0)
				continue;
			t = lua_newthread(vm->L);
			if(!t) {
				if(!G_LuaGetFunction(vm, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(vm->L);
				else
					Lua_PushEntity(vm->L, ent);
				if(!G_LuaCall(vm, function, 1, 0))
					continue;
			} else {
				if(!G_LuaGetFunctionT(t, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(t);
				else
					Lua_PushEntity(t, ent);
				if(!G_LuaResume(vm, t, function, 1))
					continue;
			}
		}
	}
	return qfalse;
}

qboolean LuaHook_G_EntityReachedAngular(char *function, int entnum) {
	int			i;
	lvm_t		*vm;
	lua_State	*t;
	gentity_t	*ent;

	for(i = 0; i < NUM_VMS; i++) {
		vm = lVM[i];
		if(vm) {
			if(vm->id < 0)
				continue;
			t = lua_newthread(vm->L);
			if(!t) {
				if(!G_LuaGetFunction(vm, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(vm->L);
				else
					Lua_PushEntity(vm->L, ent);
				if(!G_LuaCall(vm, function, 1, 0))
					continue;
			} else {
				if(!G_LuaGetFunctionT(t, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(t);
				else
					Lua_PushEntity(t, ent);
				if(!G_LuaResume(vm, t, function, 1))
					continue;
			}
		}
	}
	return qfalse;
}

qboolean LuaHook_G_EntityTrigger(char *function, int entnum, int othernum)
{
	int         i;
	lvm_t       *vm;
	lua_State	*t;
	gentity_t	*ent;
	gentity_t	*other;

	for(i = 0; i < NUM_VMS; i++)
	{
		vm = lVM[i];
		if(vm)
		{
			if(vm->id < 0)
				continue;
			t = lua_newthread(vm->L);
			if(!t) {
				if(!G_LuaGetFunction(vm, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(vm->L);
				else
					Lua_PushEntity(vm->L, ent);
				other = &g_entities[othernum];
				if(!other || !other->inuse)
					lua_pushnil(vm->L);
				else
					Lua_PushEntity(vm->L, other);
				if(!G_LuaCall(vm, function, 2, 0))
				{
					continue;
				}
			} else {
				if(!G_LuaGetFunctionT(t, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(t);
				else
					Lua_PushEntity(t, ent);
				other = &g_entities[othernum];
				if(!other || !other->inuse)
					lua_pushnil(t);
				else
					Lua_PushEntity(t, other);
				if(!G_LuaResume(vm, t, function, 2))
					continue;
			}
		}
	}
	return qfalse;
}

qboolean LuaHook_G_EntitySpawn(char *function, int entnum)
{
	int         i;
	lvm_t       *vm;
	lua_State	*t;
	gentity_t	*ent;

	for(i = 0; i < NUM_VMS; i++)
	{
		vm = lVM[i];
		if(vm)
		{
			if(vm->id < 0)
				continue;
			t = lua_newthread(vm->L);
			if(!t) {
				if(!G_LuaGetFunction(vm, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(vm->L);
				else
					Lua_PushEntity(vm->L, ent);
				if(!G_LuaCall(vm, function, 1, 0))
				{
					continue;
				}
			} else {
				if(!G_LuaGetFunctionT(t, function))
					continue;
				ent = &g_entities[entnum];
				if(!ent || !ent->inuse)
					lua_pushnil(t);
				else
					Lua_PushEntity(t, ent);
				if(!G_LuaResume(vm, t, function, 1))
					continue;
			}
		}
	}
	return qfalse;
}

#endif