#include "g_local.h"
#include "g_logger.h"

qboolean _G_Assert(void* ptr, const char* varname, const char* function, const char* file, int line) {
	if (ptr == NULL) {
		_G_LocLogger(file, line, LL_ERROR, "%s == NULL!\n", varname);
		_G_LocLogger(file, line, LL_TRACE, "%s - End\n", function);
		return qtrue;
	}

	return qfalse;
}