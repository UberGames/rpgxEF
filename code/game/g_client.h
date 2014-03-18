#ifndef _G_CLIENT_H
#define _G_CLIENT_H

#include "g_local.h"
#include "g_combat.h"

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
team_t G_Client_PickTeam( int32_t ignoreClientNum );

/**
 * Clients user info changed.
 *
 * \param clientNum Client num.
 */
void G_Client_UserinfoChanged( int32_t clientNum );

/**
 * Client connect.
 *
 * \param clientNum Client num.
 * \param firstTime Indicate whether this was the first connect.
 * \param isBot Indicate if bot or player.
 */
/*@shared@*/ char* G_Client_Connect( int32_t clientNum, qboolean firstTime, qboolean isBot );

/**
 * Initialize client.
 *
 * \param clientNum Client num.
 * \param careAboutWarmup Indicate whether to care about warm up.
 * \param isBot Bot or client?
 * \param first First time?
 */
void G_Client_Begin( int32_t clientNum, qboolean careAboutWarmup, qboolean isBot, qboolean first );

/**
 * Store the clients initial status.
 *
 * \param ent The client.
 */
void G_Client_StoreClientInitialStatus( gentity_t* ent );

/**
 * Spawn client.
 *
 * \param ent Client to spawn
 * \param rpgx_spawn rpgx_spawn
 * \param fromDeath Is this a spawn from death?
 */
void G_Client_Spawn( gentity_t* ent, int32_t rpgx_spawn, qboolean fromDeath );

/**
 * Disconnect client.
 *
 * \param clientNum Client num.
 */
void G_Client_Disconnect( int32_t clientNum );

/**
 *  Check if player is an admin.
 *
 * \param ent the player
 * \return Whether player is admin.
 */
qboolean G_Client_IsAdmin( gentity_t* ent );

/**
 *	Get location message for a client.
 * 
 * \param ent The client.
 * \param loc Location char.
 * \param loclen Length of location char.
 * \return Indicates success or fail.
 */
qboolean G_Client_GetLocationMsg(gentity_t* ent, char* loc, int32_t loclen);

/**
 * Check client statuses.
 */
void G_Client_CheckClientStatus(void);

/**
 * Client command.
 *
 * \param clientNum Client num.
 */
void G_Client_Command( int clientNum );

/**
 * Begin intermission.
 */
void G_Client_BeginIntermission(void);

/**
 * Let the client die.
 *
 * \param self Client.
 * \param inflictor Entity causing death.
 * \param attacker Entity that made inflicotr cause death.
 * \param damage ammount of demage
 * \param mod means of death
 */
void G_Client_Die (gentity_t* self, gentity_t* inflictor, gentity_t* attacker, int damage, int mod);

/**
 * Add score for the client.
 *
 * \param ent The client.
 * \param score Ammount of score to add.
 */
void G_Client_AddScore( gentity_t* ent, int score );

/**
 * Calculate clients ranks.
 *
 * \param fromExit From exit?
 */
void G_Client_CalculateRanks( qboolean fromExit );

/**
 * Set the score for a client.
 *
 * \param ent The client.
 * \param score New score for the client.
 */
void G_Client_SetScore(gentity_t *ent, int32_t score);

/**
 * Toss the weapon and powerups for the killed player.
 *
 * \param self the client.
 * \param Caused by disconnect?
 */
void G_Client_TossClientItems(gentity_t* self, qboolean dis_con);

/**
 * Let a body die.
 *
 * \param self Self.
 * \param inflictor entity that is causing the damage
 * \param attacker entity that caused the inflictor to damage targ
 * \param damage Amount of damage.
 * \param meansOfDeath Means of death.
 */
void body_die(gentity_t* self, gentity_t* inflictor, gentity_t* attacker, int32_t damage, int32_t meansOfDeath);

/**
 * \brief Gets the classname for a given player class.
 * \param pClass The class.
 */
char* G_Client_ClassNameForValue(pclass_t pClass);

/**
 *	Checks if a player is in a Class that is borg.
 *	\param ent the player
 *
 *	\author Ubergames - GSIO01
 *	\date 08/05/2009
 */
qboolean G_Client_IsBorg(gentity_t* ent);

extern clInitStatus_t clientInitialStatus[MAX_CLIENTS];

#endif /* _G_CLIENT_H */