/*=======================================================
 *
 * This file contains entities that are used to call UIs
 *
  =======================================================*/

#include "g_local.h"

extern void InitTrigger(gentity_t *self);

/*QUAKED ui_transporter (.5 .5 .5) ? DISABLED
-----DESCRIPTION-----
Opens the transporter UI.

-----SPAWNFLAGS-----
1: DISABLED	- Entity is disabled at spawn

-----KEYS-----
"swapname" - enables/disables entity(NO_ACTIVATOR/SELF flag must be checked for any entity using this)
"target" - trigger_transporter to use with this ui_transporter
*/
/**
*	\brief Think function for ui_transporter entity.
*	\param ent the ui_transporter entity
*	\author Ubergames - GSIO01
*/
void ui_transporter_think(gentity_t *ent) {
	if(!ent->activator || ent->sound1to2 >= 10000) { /* player disconnect or was idle more than 10 seconds */
		ent->sound1to2 = 0;
		ent->count = 0;
		ent->nextthink = -1;
	} else {
		ent->nextthink = level.time + 2500;
		ent->sound1to2 += 2500;
	}
}

/**
*	\brief Use function for ui_transporter entity.
*
*	Either either (de)activates entity or opens up the transporter UI.
*
*	\param ent the ui_transporter entity
*	\param activator the entity that has used the ui_transporter entity
*	\param other other entity
*
*	\author Ubergames - GSIO01
*/
void ui_transporter_use(gentity_t *ent, gentity_t *other, gentity_t *activator) {
	gentity_t *target;
	if(!Q_stricmp(ent->swapname, activator->target)) {
		ent->flags ^= FL_LOCKED;
	} else {
		if(ent->flags & FL_LOCKED || ent->count) return;
		target = ent->target_ent;
		ent->count = 1; /* in use indicator */
		ent->touched = activator;
		trap_SendServerCommand(activator-g_entities, va("ui_transporter %i", target-g_entities));
		ent->nextthink = level.time + 2500;
	}
}

/**	
*	\brief Continues setupt of ui_transporter entity after all other entites had time to spawn.
*
*	\param ent the ui_transporter entity
*
*	\author Ubergames - GSIO01
*/
void ui_transporter_setup(gentity_t *ent) {
	gentity_t *target = NULL;

	target = G_Find(target, FOFS(targetname), ent->target);

	if(!target) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] ui_transporter without trigger_transporter as target at %s!\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}

	ent->target_ent = target;
	ent->target_ent->target_ent = ent;

	ent->nextthink = -1;
	ent->think = ui_transporter_think;
}

/**
*	\brief Spawn function of ui_transporter entity.
*
*	\param ent the ui_transporter entity
*
*	\author GSIO01
*/
void SP_ui_transporter(gentity_t *ent) {
	
	if(!ent->target) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] ui_transporter without target at %s!\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}

	if(ent->spawnflags & 1)
		ent->flags ^= FL_LOCKED;
	ent->use = ui_transporter_use;
	ent->think = ui_transporter_setup;
	ent->nextthink = level.time + 500;
	ent->count = 0;
	trap_LinkEntity(ent);
}

/*QUAKED ui_msd (.5 .5 .5) ? DISABLED
-----DESCRIPTION-----
Opens a Master Systems Display. It will display data grabbed from a target_shiphealth.

-----SPAWNFLAGS-----
1: DISABLED	- Entity is disabled at spawn

-----KEYS-----
"swapname" - enables/disables entity(NO_ACTIVATOR/SELF flag must be checked for any entity using this)
"target" - target_shiphealth to draw info from
*/

/**
*	\brief Use function for ui_msd entity.
*
*	Either either (de)activates entity or opens up the MSD.
*
*	\param ent the ui_msd entity
*	\param activator the entity that has used the ui_msd entity
*	\param other other entity
*
*	\author Ubergames - Harry Young
*/
void ui_msd_use(gentity_t *ent, gentity_t *other, gentity_t *activator) {
	gentity_t *target, *temp = NULL;
	int maxhull, currhull, maxshield, currshield, shieldstate, warpstate= -2, turbostate= -2, transstate= -2, alertstate= -2;

	if(!Q_stricmp(ent->swapname, activator->target)) {
		ent->flags ^= FL_LOCKED;
	} else {
		if(ent->flags & FL_LOCKED) return;
		target = ent->target_ent;

		maxhull = target->health;
		currhull = target->count;
		maxshield = target->splashRadius;
		currshield = target->n00bCount;
		shieldstate = target->splashDamage;
		if(target->falsetarget){
			while((temp = G_Find(temp, FOFS(targetname), target->falsetarget)) != NULL){
				if(!Q_stricmp(temp->classname, "target_warp")) break;
			}
			if(temp){
				if(temp->sound1to2)//warp active
					warpstate = 1;
				else if(temp->sound2to1)//warp ejected
					warpstate = -1;
				else//not online && not ejected -->offline
					warpstate = 0;
			temp = NULL;
			}
		}
		if(target->bluename){
			while((temp = G_Find(temp, FOFS(targetname), target->bluename)) != NULL){
				if(!Q_stricmp(temp->classname, "target_turbolift")) break;
			}
			if(temp){
				if (temp->flags & FL_LOCKED)
					turbostate = 0;
				else
					turbostate = 1;
			temp = NULL;
			}
		}
		if(target->bluesound){
			while((temp = G_Find(temp, FOFS(targetname), target->bluesound)) != NULL){
				if(!Q_stricmp(temp->classname, "ui_transporter")) break;
			}
			if(temp){
				if (temp->flags & FL_LOCKED)
					transstate = 0;
				else
					transstate = 1;
			temp = NULL;
			}
		}
		if(target->falsename){
			while((temp = G_Find(temp, FOFS(targetname), target->falsename)) != NULL){
				if(!Q_stricmp(temp->classname, "target_alert")) break;
			}
			if(temp){
			alertstate = temp->damage;
			temp = NULL;
			}
		}
		trap_SendServerCommand(activator-g_entities, va("ui_msd %i %i %i %i %i %i %i %i %i", maxhull, currhull, maxshield, currshield, shieldstate, warpstate, turbostate, transstate, alertstate));
	}
}

/**	
*	\brief Continues setupt of ui_msd entity after all other entites had time to spawn.
*
*	\param ent the ui_msd entity
*
*	\author Ubergames - GSIO01
*/
void ui_msd_setup(gentity_t *ent) {
	gentity_t *target = NULL;

	while((target = G_Find(target, FOFS(targetname), ent->target)) != NULL){
		if(!Q_stricmp(target->classname, "target_shiphealth")) break;
	}

	if(!target) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] ui_msd without target_shiphealth as target at %s! Removing Entity.\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}

	ent->target_ent = target;
	ent->target_ent->target_ent = ent;

	ent->nextthink = -1;
	ent->think = 0;
}

/**
*	\brief Spawn function of ui_msd entity.
*
*	\param ent the ui_msd entity
*
*	\author GSIO01
*/
void SP_ui_msd(gentity_t *ent) {
	
	if(!ent->target) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] ui_msd without target at %s! Removing Entity.\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}

	if(ent->spawnflags & 1)
		ent->flags ^= FL_LOCKED;
	ent->use = ui_msd_use;
	ent->think = ui_msd_setup;
	ent->nextthink = level.time + 500;
	ent->count = 0;
	trap_LinkEntity(ent);
}

/*QUAKED ui_holodeck (.5 .5 .5) ? DISABLED
-----Description-----
Will open  the holodeck UI once this is implemented. For now this will not spawn.

-----SPAWNFLAGS-----
1: DISABLED	Entity is disabled at spawn

-----KEYS-----
"swapname" - enables/disables entity(NO_ACTIVATOR/SELF flag must be checked for any entity using this)
"target" - trigger_holodeck to use with this ui_holodeck
*/
void ui_holodeck_think(gentity_t *ent) {
	if(!ent->activator || ent->sound1to2 >= 10000) { /* player disconnect or was idle more than 10 seconds */
		ent->sound1to2 = 0;
		ent->count = 0;
		ent->nextthink = -1;
	} else {
		ent->nextthink = level.time + 2500;
		ent->sound1to2 += 2500;
	}
}

void ui_holodeck_use(gentity_t *ent, gentity_t *other, gentity_t *activator) {
	gentity_t *target;
	if(!Q_stricmp(ent->swapname, activator->target)) {
		ent->flags ^= FL_LOCKED;
	} else {
		if(ent->flags & FL_LOCKED || ent->count) return;
		target = ent->target_ent;
		ent->count = 1; /* in use indicator */
		ent->touched = activator;
		trap_SendServerCommand(activator-g_entities, va("ui_holodeck %i", target-g_entities));
		ent->nextthink = level.time + 2500;
	}
}

void ui_holodeck_setup(gentity_t *ent) {
	gentity_t *target;

	target = G_Find(NULL, FOFS(targetname), ent->target);

	if(!target) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] ui_holodekc without trigger_holodeck as target at %s!\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
		return;
	}

	ent->target_ent = target;
	ent->target_ent->target_ent = ent;

	ent->nextthink = -1;
	ent->think = ui_holodeck_think;
}

void SP_ui_holodeck(gentity_t *ent) {
	if(!ent->target) {
		DEVELOPER(G_Printf(S_COLOR_YELLOW "[Entity-Error] ui_holodeck without target at %s!\n", vtos(ent->s.origin)););
		G_FreeEntity(ent);
	}

	if(ent->spawnflags & 1)
		ent->flags ^= FL_LOCKED;
	ent->use = ui_holodeck_use;
	ent->think = ui_holodeck_setup;
	ent->nextthink = level.time + 500;
	ent->count = 0;
	trap_LinkEntity(ent);
}

