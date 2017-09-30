#include "g_cinematic.h"
#include "g_local.h"
#include "g_client.h"
#include "g_spawn.h"
#include "g_syscalls.h"

extern void InitMover( gentity_t *ent );

/*QUAKED cinematic_camera (0 0.5 0) (-4 -4 -4) (4 4 4)
-----DESCRIPTION-----
Camera for cinematic. Normally spawn by Lua script.

-----SPAWNFLAGS-----
none

-----KEYS-----
none

-----LUA-----
To be written later.
*/
void SP_cinematic_camera(gentity_t *ent) {
	ent->type = ENT_CINEMATIC_CAMMERA;
	trap_LinkEntity(ent);
	InitMover(ent);
}

void Cinematic_ActivateCameraMode(gentity_t *ent, gentity_t *target) {
	gclient_t *client;

	if(!ent || !ent->client || (ent->flags & FL_CCAM)) return;
	client = ent->client;

	ent->flags ^= FL_CCAM;
	client->ps.pm_type = PM_CCAM;
	client->cam = target;
	VectorCopy(client->ps.viewangles, client->origViewAngles);
	VectorCopy(ent->r.currentOrigin, client->origOrigin);
	G_Client_SetViewAngle(ent, target->s.angles);
	G_SetOrigin(ent, target->r.currentOrigin);
	VectorCopy(target->r.currentOrigin, ent->client->ps.origin);
	trap_LinkEntity(ent);
}

void Cinematic_DeactivateCameraMode(gentity_t *ent) {
	gclient_t *client;

	if(!ent || !ent->client || !(ent->flags & FL_CCAM)) return;
	client =  ent->client;

	client->cam = NULL;

	G_Printf("resetting origin to %s\n", vtos(client->origOrigin));

	G_SetOrigin(ent, client->origOrigin);
	VectorCopy(client->origOrigin, ent->client->ps.origin);
	G_Client_SetViewAngle(ent, client->origViewAngles);
	trap_LinkEntity(ent);

}

void Cinematic_ActivateGlobalCameraMode(gentity_t *target) {
	int i;
	gentity_t *ent;

	for(i = 0; i < g_maxclients.integer; i++) {
		ent = g_entities + i;
		if(!ent || !ent->client) continue;
		Cinematic_ActivateCameraMode(ent, target);
	}
}

void Cinematic_DeactivateGlobalCameraMode(void) {
	int i;
	gentity_t *ent;

	for(i = 0; i < g_maxclients.integer; i++) {
		ent = g_entities + i;
		if(!ent || !ent->client) continue;
		Cinematic_DeactivateCameraMode(ent);
	}
}
