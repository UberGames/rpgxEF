// g_lua.c

#include "cg_lua.h"

#ifdef CG_LUA

lvm_t       *lVM[NUM_VMS];
fileHandle_t lualog;

void QDECL LUA_DEBUG(const char *fmt, ...)
{
	va_list         argptr;
	char            text[1024];

	if(cg_debugLua.integer >= 1)
	{
		va_start(argptr, fmt);
		Com_sprintf(text, sizeof(text), fmt, argptr);
		va_end(argptr);
		CG_Printf(S_COLOR_YELLOW "LUA DEBUG:" S_COLOR_WHITE " %s\n", text);
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

	if(cg_logLua.integer) {
		sec = cg.time / 1000;
		min = sec / 60;
		sec -= min * 60;
		tens = sec / 10;
		sec -= tens * 10;

		Com_sprintf(string, sizeof(string), "%i:%i%i %s", min, tens, sec, buff);

		trap_FS_Write(string, strlen(string), lualog);
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
		CG_Printf(S_COLOR_YELLOW "Lua: can not open file %s\n", path);
		trap_FS_FCloseFile(f);
		return qfalse;
	}
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
			CG_Printf(S_COLOR_YELLOW "Lua: failed to allocate memory for lua VM\n");
			return qfalse;
		}
		memset(vm, 0, sizeof(lvm_t));
		vm->id = -1;
		Q_strncpyz(vm->filename, path, sizeof(vm->filename));
		vm->code = code;
		vm->code_size = flen;
		vm->error = 0;

		if(CG_LuaStartVM(vm) == qfalse)
		{
			CG_LuaStopVM(vm);
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

qboolean CG_LuaInit()
{
	char fxfilename[MAX_QPATH];
	fileHandle_t fxfile;
	
	CG_Printf("------- CG_LuaInit -------\n");

	// read map fx file
	

	// open log file
	if(cg_logLua.integer) {
		trap_FS_FOpenFile("cg_lua.log", &lualog, FS_APPEND);
	}

	CG_Printf("------- CG_LuaInit Finish -------\n");

	return qtrue;
}

qboolean CG_LuaResume(lvm_t *vm, lua_State *T, char *func, int nargs) {
	int res = lua_resume(T, nargs);

	if(res == LUA_ERRRUN) {
		LUA_LOG("Lua: %s error running lua script: %s\n", func, lua_tostring(T, -1));
		CG_Printf(S_COLOR_YELLOW "Lua: %s error running lua script: %s\n", func, lua_tostring(T, -1));
		lua_pop(T, 1);
		vm->error++;
		return qfalse;
	} else if(res == LUA_ERRMEM) {
		LUA_LOG("Lua: memory allocation error #2 ( %s )\n", vm->filename);
		vm->error++;
		return qfalse;
	} else if(res == LUA_ERRERR) {
		LUA_LOG("Lua: traceback error ( %s )\n", vm->filename);
		CG_Printf(S_COLOR_YELLOW "Lua: traceback error ( %s )\n", vm->filename);
		vm->error++;
		return qfalse;
	} 
	return qtrue;
}

qboolean CG_LuaCall(lvm_t * vm, char *func, int nargs, int nresults)
{
	int             res = lua_pcall(vm->L, nargs, nresults, 0);

	if(res == LUA_ERRRUN)
	{
		LUA_LOG("Lua: %s error running lua script: %s\n", func, lua_tostring(vm->L, -1));
		CG_Printf(S_COLOR_YELLOW "Lua: %s error running lua script: %s\n", func, lua_tostring(vm->L, -1));
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
		CG_Printf(S_COLOR_YELLOW "Lua: traceback error ( %s )\n", vm->filename);
		vm->error++;
		return qfalse;
	}
	return qtrue;
}

#define SAY_ALL		0
#define SAY_TEAM	1

qboolean CG_LuaGetFunctionT(lua_State *T, char *name)
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

qboolean CG_LuaGetFunction(lvm_t * vm, char *name)
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

qboolean CG_LuaStartVM(lvm_t * vm)
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
	Luaopen_Qmath(vm->L);
	Luaopen_Vector(vm->L);

	res = luaL_loadbuffer(vm->L, vm->code, vm->code_size, vm->filename);
	if(res == LUA_ERRSYNTAX)
	{
		LUA_LOG("Lua: syntax error during pre-compilation: %s\n", (char *)lua_tostring(vm->L, -1));
		CG_Printf(S_COLOR_YELLOW "Lua: syntax error: %s\n", (char *)lua_tostring(vm->L, -1));
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

	if(!CG_LuaCall(vm, "CG_LuaStartVM", 0, 0))
		return qfalse;

	LUA_LOG("Lua: Loading %s\n", vm->filename);
	return qtrue;
}

void CG_LuaStopVM(lvm_t * vm)
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

void CG_LuaShutdown()
{
	int             i;
	lvm_t	       *vm;

	for(i = 0; i < NUM_VMS; i++)
	{
		vm = lVM[i];
		if(vm)
		{
			CG_LuaStopVM(vm);
		}
	}

	trap_FS_FCloseFile(lualog);
}

void CG_LuaStatus(void)
{
	int             i, cnt = 0;

	for(i = 0; i < NUM_VMS; i++)
		if(lVM[i])
			cnt++;

	if(cnt == 0)
	{
		CG_Printf("Lua: no scripts loaded.\n");
		return;
	}
	else if(cnt == 1)
	{
		CG_Printf("Lua: showing lua information ( 1 module loaded )\n");
	}
	else
	{
		CG_Printf("Lua: showing lua information ( %d modules loaded )\n", cnt);
	}
	CG_Printf("%-2s %-24s\n", "VM", "Filename");
	CG_Printf("-- ------------------------\n");
	for(i = 0; i < NUM_VMS; i++)
	{
		if(lVM[i])
		{
			CG_Printf("%2d %-24s\n", lVM[i]->id, lVM[i]->filename);
		}
	}
	CG_Printf("-- ------------------------\n");

}

lvm_t *CG_LuaGetVM(lua_State * L)
{
	int             i;

	for(i = 0; i < NUM_VMS; i++)
		if(lVM[i] && lVM[i]->L == L)
			return lVM[i];
	return NULL;
}


#endif
