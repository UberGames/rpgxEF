#ifndef _G_CLIENT_H
#define _G_CLIENT_H

#include "g_local.h"

/**
 * Select a spawnpoint.
 *
 * \param avoidPoint Point to avoid.
 * \param origin Origin.
 * \param angles Angles.
 */
/*@shared@*/ /*@null@*/  gentity_t* G_Client_SelectSpawnPoint ( vec3_t avoidPoint, vec3_t origin, vec3_t angles );

/**
 * Set the clients view angle.
 *
 * \param ent Entity for which to set view angle.
 * \param angle New view angle.
 */
void G_Client_SetViewAngle( gentity_t* ent, vec3_t angle );

/**
 * Respawn client.
 *
 * \param ent Client to respawn.
 */
void G_Client_Respawn(gentity_t *ent);

/**
 * Init the body que.
 */
void G_Client_InitBodyQue(void);

//TiM - Delayed Transport Beam
void G_InitTransport( int32_t clientNum, vec3_t origin, vec3_t angles );

/**
 * Pick a random team.
 *
 * \param ignoreClientNum Client to ignore.
 * \return Random team.
 */
team_t G_Client_PickTeam( int ignoreClientNum );

/**
 * Clients user info changed.
 *
 * \param clientNum Client num.
 */
void G_Client_UserinfoChanged( int32_t clientNum );

#endif /* _G_CLIENT_H */