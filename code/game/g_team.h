// Copyright (C) 1999-2000 Id Software, Inc.
//

// Prototypes
const char *TeamName(int team);
const char *OtherTeamName(int team);
const char *TeamColorString(int team);

qboolean Team_GetLocationMsg(gentity_t *ent, char *loc, int loclen);
void TeamplayInfoMessage( gentity_t *ent ); // needed for locations ... TODO move from team to somewhere else
void CheckTeamStatus(void); // needed for locations ... TODO move from team to somewhere else
