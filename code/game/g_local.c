#include "g_local.h"
#include "g_logger.h"

qboolean _G_Assert(void* ptr, const char* varname, const char* function) {
	if (ptr == NULL) {
		G_LocLogger(LL_ERROR, "%s == NULL!\n", varname);
		G_LocLogger(LL_TRACE, "%s - End\n", function);
		return qtrue;
	}

	return qfalse;
}