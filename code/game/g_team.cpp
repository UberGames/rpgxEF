// Copyright (C) 1999-2000 Id Software, Inc.
//

#include "g_local.h"

const char *TeamName(int team)  {
	if (team==TEAM_SPECTATOR)
		return "SPECTATOR";
	return "FREE";
}

const char *OtherTeamName(int team) {
	if (team==TEAM_SPECTATOR)
		return "SPECTATOR";
	return "FREE";
}

const char *TeamColorString(int team) {
	if (team==TEAM_SPECTATOR)
		return S_COLOR_YELLOW;
	return S_COLOR_WHITE;
}

/*---------------------------------------------------------------------------*/






