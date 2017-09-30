#include "g_logger.h"

void QDECL G_Logger (int log_level, char* fmt, ...) {
	va_list argptr;
	char	text[1024];

	if(log_level > g_logLevel.integer) {
		return;
	}

	va_start (argptr, fmt);
	vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	switch (log_level)
	{
	case LL_ERROR:
		G_Printf(S_COLOR_RED "[game][error] - %s", text);
		break;
	case LL_WARN:
		G_Printf(S_COLOR_YELLOW "[game][warn ] - %s", text);
		break;
	case LL_INFO:
		G_Printf("[game][info ] - %s", text);
		break;
	case LL_DEBUG:
		G_Printf("[game][debug] - %s", text);
		break;
	case LL_TRACE:
		G_Printf("[game][trace] - %s", text);
		break;
	case LL_ALWAYS:
	default:
		G_Printf("[game]        - %s", text);
		break;
	}

	if(fmt[strlen(fmt) - 1] != '\n') {
		G_Printf("\n");
	}
}

void QDECL _G_LocLogger (const char* file, int line, int log_level, char* fmt, ...) {
	va_list argptr;
	char	text[1024];

	if(log_level > g_logLevel.integer) {
		return;
	}

	va_start (argptr, fmt);
	vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	switch (log_level)
	{
	case LL_ERROR:
		G_Printf(S_COLOR_RED "[game][error][%s:%d] - ", file, line);
		G_Printf(S_COLOR_RED "%s", text);
		break;
	case LL_WARN:
		G_Printf(S_COLOR_YELLOW "[game][warn ][%s:%d] - ", file, line);
		G_Printf(S_COLOR_YELLOW "%s", text);
		break;
	case LL_INFO:
		G_Printf("[game][info ][%s:%d] - ", file, line);
		G_Printf("%s", text);
		break;
	case LL_DEBUG:
		G_Printf("[game][debug][%s:%d] - ", file, line);
		G_Printf("%s", text);
		break;
	case LL_TRACE:
		G_Printf("[game][trace][%s:%d] - ", file, line);
		G_Printf("%s", text);
		break;
	case LL_ALWAYS:
	default:
		G_Printf("[game]       [%s:%d] - ", file, line);
		G_Printf("%s", text);
		break;
	}

	if(fmt[strlen(fmt) - 1] != '\n') {
		G_Printf("\n");
	}
}
