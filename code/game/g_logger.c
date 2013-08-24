#include "g_logger.h"

void QDECL G_Logger (int level, char* fmt, ...) {
	va_list argptr;
	char	text[1024];

	if(level > g_logLevel.integer && g_logLevel.integer != LL_ALWAYS) {
		return;
	}

	va_start (argptr, fmt);
	vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	switch (level)
	{
	case LL_ERROR:
		G_Printf(S_COLOR_RED "[game][error] %s", text);
		break;
	case LL_WARN:
		G_Printf(S_COLOR_YELLOW "[game][warn] %s", text);
		break;
	case LL_INFO:
		G_Printf("[game][info] %s", text);
		break;
	case LL_DEBUG:
		G_Printf("[game][info] %s", text);
		break;
	case LL_TRACE:
		G_Printf("[game][trace] %s", text);
		break;
	case LL_ALWAYS:
	default:
		G_Printf("[game] %s", text);
		break;
	}
}

void QDECL _G_LocLogger (const char* file, int line, int level, char* fmt, ...) {
	va_list argptr;
	char	text[1024];

	if(level > g_logLevel.integer && g_logLevel.integer != LL_ALWAYS) {
		return;
	}

	va_start (argptr, fmt);
	vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	switch (level)
	{
	case LL_ERROR:
		G_Printf(S_COLOR_RED "[game][error] %s:%d - ", file, line);
		break;
	case LL_WARN:
		G_Printf(S_COLOR_YELLOW "[game][warn] %s:%d - ", file, line);
		break;
	case LL_INFO:
		G_Printf("[game][info] %s:%d - ", file, line);
		break;
	case LL_DEBUG:
		G_Printf("[game][info] %s:%d - ", file, line);
		break;
	case LL_TRACE:
		G_Printf("[game][trace] %s:%d - ", file, line);
		break;
	case LL_ALWAYS:
	default:
		G_Printf("[game] %s:%d - ", file, line);
		break;
	}
}
