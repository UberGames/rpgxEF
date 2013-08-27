#include "cg_logger.h"

void QDECL CG_Logger (int level, char* fmt, ...) {
	va_list argptr;
	char	text[1024];

	if(level > cg_logLevel.integer) {
		return;
	}

	va_start (argptr, fmt);
	vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	switch (level)
	{
	case LL_ERROR:
		CG_Printf(S_COLOR_RED "[cgame][error] - %s", text);
		break;
	case LL_WARN:
		CG_Printf(S_COLOR_YELLOW "[cgame][warn ] - %s", text);
		break;
	case LL_INFO:
		CG_Printf("[cgame][info ] - %s", text);
		break;
	case LL_DEBUG:
		CG_Printf("[cgame][debug] - %s", text);
		break;
	case LL_TRACE:
		CG_Printf("[cgame][trace] - %s", text);
		break;
	case LL_ALWAYS:
	default:
		CG_Printf("[cgame]        - %s", text);
		break;
	}
}

void QDECL _CG_LocLogger (const char* file, int line, int level, char* fmt, ...) {
	va_list argptr;
	char	text[1024];

	if(level > cg_logLevel.integer) {
		return;
	}

	va_start (argptr, fmt);
	vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	switch (level)
	{
	case LL_ERROR:
		CG_Printf(S_COLOR_RED "[cgame][error][%s:%d] - ", file, line);
		CG_Printf(S_COLOR_RED "%s", text);
		break;
	case LL_WARN:
		CG_Printf(S_COLOR_YELLOW "[cgame][warn ][%s:%d] - ", file, line);
		CG_Printf(S_COLOR_YELLOW "%s", text);
		break;
	case LL_INFO:
		CG_Printf("[cgame][info ][%s:%d] - ", file, line);
		CG_Printf("%s", text);
		break;
	case LL_DEBUG:
		CG_Printf("[cgame][debug][%s:%d] - ", file, line);
		CG_Printf("%s", text);
		break;
	case LL_TRACE:
		CG_Printf("[cgame][trace][%s:%d] - ", file, line);
		CG_Printf("%s", text);
		break;
	case LL_ALWAYS:
	default:
		CG_Printf("[cgame]       [%s:%d] - ", file, line);
		CG_Printf("%s", text);
		break;
	}
}
