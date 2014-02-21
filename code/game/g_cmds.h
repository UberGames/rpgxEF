#ifndef G_CMDS_H
#define G_CMDS_H

#include "g_local.h"

qboolean SetClass( gentity_t *ent, char *s, /*@null@*/ char *teamName, qboolean SaveToCvar );
void DragCheck( void );
pclass_t ValueNameForClass ( char* s );
void BroadcastClassChange( gclient_t *client, pclass_t oldPClass );

/**
 * Request current scoreboard information.
 */
void DeathmatchScoreboardMessage(gentity_t* ent);

/**
 * If the client being followed leaves the game, or you just want to drop
 * to free floating spectator mode
 */
void StopFollowing( gentity_t* ent );

/**
 * Let everyone know about a team change.
 *
 * \param client The client that changed team.
 * \param oldTeam The team the client was in.
 */
void BroadcastTeamChange( gclient_t* client, int oldTeam );

/**
 * Set the team for a player.
 *
 * \param ent A player.
 * \param s The new team.
 * \return Success or fail.
 */
qboolean SetTeam( gentity_t* ent, char* s );

/**
 * Cycle different players.
 */
void Cmd_FollowCycle_f( gentity_t* ent, int dir );


#endif