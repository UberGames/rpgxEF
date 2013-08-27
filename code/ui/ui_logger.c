#include "ui_logger.h"

void QDECL UI_Logger (int level, char* fmt, ...) {
	va_list argptr;
	char	text[1024];

	if(level > ui_logLevel.integer) {
		return;
	}

	va_start (argptr, fmt);
	vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	switch (level)
	{
	case LL_ERROR:
		Com_Printf(S_COLOR_RED "[ui][error] - %s", text);
		break;
	case LL_WARN:
		Com_Printf(S_COLOR_YELLOW "[ui][warn ] - %s", text);
		break;
	case LL_INFO:
		Com_Printf("[ui][info ] - %s", text);
		break;
	case LL_DEBUG:
		Com_Printf("[ui][debug] - %s", text);
		break;
	case LL_TRACE:
		Com_Printf("[ui][trace] - %s", text);
		break;
	case LL_ALWAYS:
	default:
		Com_Printf("[ui]        - %s", text);
		break;
	}
}

void QDECL _UI_LocLogger (const char* file, int line, int level, char* fmt, ...) {
	va_list argptr;
	char	text[1024];

	if(level > ui_logLevel.integer) {
		return;
	}

	va_start (argptr, fmt);
	vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	switch (level)
	{
	case LL_ERROR:
		Com_Printf(S_COLOR_RED "[ui][error][%s:%d] - ", file, line);
		Com_Printf(S_COLOR_RED "%s", text);
		break;
	case LL_WARN:
		Com_Printf(S_COLOR_YELLOW "[ui][warn ][%s:%d] - ", file, line);
		Com_Printf(S_COLOR_YELLOW "%s", text);
		break;
	case LL_INFO:
		Com_Printf("[ui][info ][%s:%d] - ", file, line);
		Com_Printf("%s", text);
		break;
	case LL_DEBUG:
		Com_Printf("[ui][debug][%s:%d] - ", file, line);
		Com_Printf("%s", text);
		break;
	case LL_TRACE:
		Com_Printf("[ui][trace][%s:%d] - ", file, line);
		Com_Printf("%s", text);
		break;
	case LL_ALWAYS:
	default:
		Com_Printf("[ui]       [%s:%d] - ", file, line);
		Com_Printf("%s", text);
		break;
	}
}
