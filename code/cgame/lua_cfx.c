#include "cg_lua.h"

#ifdef CG_LUA

void Lua_CFX_ParseMapFxFile(fileHandle_t *f) {
	// TODO
}

void Lua_CFX_LoadMapFxFile(void) {
	char filename[MAX_QPATH];
	fileHandle_t file;

	sprintf(filename, "maps/%s.fx", cgs.mapname);
	trap_FS_FOpenFile(filename, &file, FS_READ);
	if(!file) return;

	Lua_CFX_ParseMapFxFile(&file);
}
#endif
