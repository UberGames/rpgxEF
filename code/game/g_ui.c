/*=======================================================
 *
 * This file contains entities that are used to call UIs
 *
  =======================================================*/

#include "g_local.h"
#include "g_spawn.h"
#include "g_trigger.h"
#include "g_logger.h"

#define UI_DIABLED 1
/*QUAKED ui_transporter (1 0.5 0) (-8 -8 -8) (8 8 8) ? DISABLED
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
static void ui_transporter_think(gentity_t* ent) {
	G_LogFuncBegin();

	if (ent == NULL) {
		G_LocLogger(LL_ERROR, "ent == NULL!\n");
		G_LogFuncEnd();
		return;
	}

	if(ent->activator == NULL || ent->sound1to2 >= 10000) { /* player disconnect or was idle more than 10 seconds */
		ent->sound1to2 = 0;
		ent->count = 0;
		ent->nextthink = -1;
	} else {
		ent->nextthink = level.time + 2500;
		ent->sound1to2 += 2500;
	}

	G_LogFuncEnd();
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
static void ui_transporter_use(gentity_t* ent, gentity_t* other, gentity_t* activator) {
	gentity_t* target = NULL;

	G_LogFuncBegin();

	if (ent == NULL) {
		G_LocLogger(LL_ERROR, "ent == NULL!\n");
		G_LogFuncEnd();
		return;
	}

	if (activator == NULL) {
		G_LocLogger(LL_ERROR, "activator == NULL!\n");
		G_LogFuncEnd();
		return;
	}

	if(Q_stricmp(ent->swapname, activator->target) == 0) {
		ent->flags ^= FL_LOCKED;
	} else {
		if((ent->flags & FL_LOCKED) != 0 || ent->count != 0) {
			G_LogFuncEnd();
			return;
		}

		target = ent->target_ent;
		ent->count = 1; /* in use indicator */
		ent->touched = activator;
		trap_SendServerCommand(activator-g_entities, va("ui_transporter %i", target-g_entities));
		ent->nextthink = level.time + 2500;
	}

	G_LogFuncEnd();
}

/**	
*	\brief Continues setupt of ui_transporter entity after all other entites had time to spawn.
*	\param ent the ui_transporter entity
*	\author Ubergames - GSIO01
*/
static void ui_transporter_setup(gentity_t* ent) {
	gentity_t* target = NULL;

	G_LogFuncBegin();

	if (ent == NULL) {
		G_LocLogger(LL_ERROR, "ent == NULL!\n");
		G_LogFuncEnd();
		return;
	}

	target = G_Find(target, FOFS(targetname), ent->target);

	if(target == NULL) {
		G_LocLogger(LL_ERROR, "ui_transporter without trigger_transporter as target at %s!\n", vtos(ent->s.origin));
		G_FreeEntity(ent);
		G_LogFuncEnd();
		return;
	}

	ent->target_ent = target;
	ent->target_ent->target_ent = ent;

	ent->nextthink = -1;
	ent->think = ui_transporter_think;

	G_LogFuncEnd();
}

void SP_ui_transporter(gentity_t* ent) {
	ent->type = ENT_UI_TRANSPORTER;
	
	G_LogFuncBegin();

	if (ent == NULL) {
		G_LocLogger(LL_ERROR, "ent == NULL!\n");
		G_LogFuncEnd();
		return;
	}

	if(ent->target == NULL) {
		G_LocLogger(LL_ERROR, "ui_transporter without target at %s!\n", vtos(ent->s.origin));
		G_FreeEntity(ent);
		G_LogFuncEnd();
		return;
	}

	if ((ent->spawnflags & UI_DIABLED) != 0) {
		ent->flags ^= FL_LOCKED;
	}

	ent->use = ui_transporter_use;
	ent->think = ui_transporter_setup;
	ent->nextthink = level.time + 500;
	ent->count = 0;
	trap_LinkEntity(ent);

	G_LogFuncEnd();
}

/*QUAKED ui_msd (1 0.5 0) (-8 -8 -8) (8 8 8) ? DISABLED
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
static void ui_msd_use(gentity_t* ent, gentity_t* other, gentity_t* activator) {
	gentity_t *target, *temp = NULL;
	int32_t maxhull = 0;
	int32_t currhull = 0;
	int32_t maxshield = 0;
	int32_t currshield = 0;
	int32_t shieldstate = 0;
	int32_t warpstate= -2;
	int32_t turbostate= -2;
	int32_t transstate= -2;
	int32_t alertstate= -2;
	const char* model = NULL;

	G_LogFuncBegin();

	if (ent == NULL) {
		G_LocLogger(LL_ERROR, "ent == NULL!\n");
		G_LogFuncEnd();
		return;
	}

	if (activator == NULL) {
		G_LocLogger(LL_ERROR, "activator == NULL!\n");
		G_LogFuncEnd();
		return;
	}

	if(Q_stricmp(ent->swapname, activator->target) == 0) {
		ent->flags ^= FL_LOCKED;
	} else {
		if((ent->flags & FL_LOCKED) != 0) {
			G_LogFuncEnd();
			return;
		}

		target = ent->target_ent;

		maxhull = target->health;
		currhull = target->count;
		maxshield = target->splashRadius;
		currshield = target->n00bCount;
		shieldstate = target->splashDamage;

		if(target->falsetarget != NULL){
			while((temp = G_Find(temp, FOFS(truename), target->falsetarget)) != NULL){
				if(temp->type == ENT_TARGET_WARP) {
					G_LogFuncEnd();
					break;
				}
			}
			if(temp != NULL){
				if(temp->sound2to1 != 0) {
					//core ejected, we don't really care if it is online
					warpstate = 2;
				}

				if(temp->sound2to1 == 0 && temp->sound1to2 != 0) {
					//not ejected and not deactivated -> core active
					warpstate = 1;
				}

				if(temp->sound2to1 == 0 && temp->sound1to2 == 0) {
					//not ejected and deactivated -> core inactive
					warpstate = 0;
				}
				temp = NULL;
			}
		}

		if(target->bluename != NULL){
			while((temp = G_Find(temp, FOFS(swapname), target->bluename)) != NULL){
				if(temp->type == ENT_TARGET_TURBOLIFT) { 
					break;
				}
			}
			if(temp != NULL){
				if ((temp->flags & FL_LOCKED) != 0) {
					turbostate = 0;
				} else {
					turbostate = 1;
				}
				temp = NULL;
			}
		}

		if(target->bluesound != NULL){
			while((temp = G_Find(temp, FOFS(swapname), target->bluesound)) != NULL){
				if(temp->type == ENT_UI_TRANSPORTER) {
					break;
				}
			}
			if(temp != NULL){
				if ((temp->flags & FL_LOCKED) != 0) {
					transstate = 0;
				} else {
					transstate = 1;
				}
				temp = NULL;
			}
		}

		if(target->falsename != NULL){
			while((temp = G_Find(temp, FOFS(falsename), target->falsename)) != NULL){
				if(temp->type == ENT_TARGET_ALERT) break;
			}
			if(temp != NULL) {
				alertstate = temp->damage;
				temp = NULL;
			}
		}
		model = target->model;
		trap_SendServerCommand(activator-g_entities, va("ui_msd %i %i %i %i %i %i %i %i %i %s", maxhull, currhull, maxshield, currshield, shieldstate, warpstate, turbostate, transstate, alertstate, model));
		//Debugging G_Printf(S_COLOR_YELLOW "ui_msd G %i %i %i %i %i %i %i %i %i\n", maxhull, currhull, maxshield, currshield, shieldstate, warpstate, turbostate, transstate, alertstate);
	}

	G_LogFuncEnd();
}

/**	
*	\brief Continues setupt of ui_msd entity after all other entites had time to spawn.
*	\param ent the ui_msd entity
*	\author Ubergames - GSIO01
*/
static void ui_msd_setup(gentity_t* ent) {
	gentity_t* target = NULL;

	G_LogFuncBegin();

	if (ent == NULL) {
		G_LocLogger(LL_ERROR, "ent == NULL!\n");
		G_LogFuncEnd();
		return;
	}

	while((target = G_Find(target, FOFS(targetname), ent->target)) != NULL){
		if(target->type == ENT_TARGET_SHIPHEALTH) {
			break;
		}
	}

	if(target == NULL) {
		G_LocLogger(LL_ERROR, "ui_msd without target_shiphealth as target at %s! Removing Entity.\n", vtos(ent->s.origin));
		G_FreeEntity(ent);
		G_LogFuncEnd();
		return;
	}

	ent->target_ent = target;
	ent->target_ent->target_ent = ent;

	ent->nextthink = -1;
	ent->think = 0;

	G_LogFuncEnd();
}

void SP_ui_msd(gentity_t* ent) {
	G_LogFuncBegin();

	if (ent == NULL) {
		G_LocLogger(LL_ERROR, "ent == NULL!\n");
		G_LogFuncEnd();
		return;
	}

	ent->type = ENT_UI_MSD;
	
	if(!ent->target) {
		G_LocLogger(LL_ERROR, "ui_msd without target at %s! Removing Entity.\n", vtos(ent->s.origin));
		G_FreeEntity(ent);
		G_LogFuncEnd();
		return;
	}

	if ((ent->spawnflags & UI_DIABLED) != 0) {
		ent->flags ^= FL_LOCKED;
	}

	ent->use = ui_msd_use;
	ent->think = ui_msd_setup;
	ent->nextthink = level.time + 500;
	ent->count = 0;
	trap_LinkEntity(ent);

	G_LogFuncEnd();
}

/*QUAKED ui_holodeck (1 0.5 0) (-8 -8 -8) (8 8 8) ? DISABLED
-----Description-----
Will open  the holodeck UI once this is implemented. For now this will not spawn.

-----SPAWNFLAGS-----
1: DISABLED	Entity is disabled at spawn

-----KEYS-----
"swapname" - enables/disables entity(NO_ACTIVATOR/SELF flag must be checked for any entity using this)
"target" - trigger_holodeck to use with this ui_holodeck
*/
static void ui_holodeck_think(gentity_t *ent) {
	G_LogFuncBegin();

	if (ent == NULL) {
		G_LocLogger(LL_ERROR, "ent == NULL!\n");
		G_LogFuncEnd();
		return;
	}

	if(ent->activator != NULL|| ent->sound1to2 >= 10000) { /* player disconnect or was idle more than 10 seconds */
		ent->sound1to2 = 0;
		ent->count = 0;
		ent->nextthink = -1;
	} else {
		ent->nextthink = level.time + 2500;
		ent->sound1to2 += 2500;
	}

	G_LogFuncEnd();
}

static void ui_holodeck_use(gentity_t* ent, gentity_t* other, gentity_t* activator) {
	gentity_t* target = NULL;

	G_LogFuncBegin();

	if (ent == NULL) {
		G_LocLogger(LL_ERROR, "ent == NULL!\n");
		G_LogFuncEnd();
		return;
	}

	if (activator == NULL) {
		G_LocLogger(LL_ERROR, "activator == NULL!\n");
		G_LogFuncEnd();
		return;
	}

	if(Q_stricmp(ent->swapname, activator->target) == 0) {
		ent->flags ^= FL_LOCKED;
	} else {
		if((ent->flags & FL_LOCKED) != 0 || ent->count != 0) {
			G_LogFuncEnd();
			return;
		}

		target = ent->target_ent;
		ent->count = 1; /* in use indicator */
		ent->touched = activator;
		trap_SendServerCommand(activator-g_entities, va("ui_holodeck %i", target-g_entities));
		ent->nextthink = level.time + 2500;
	}

	G_LogFuncEnd();
}

static void ui_holodeck_setup(gentity_t* ent) {
	gentity_t* target = NULL;

	G_LogFuncBegin();

	if (ent == NULL) {
		G_LocLogger(LL_ERROR, "ent == NULL!\n");
		G_LogFuncEnd();
		return;
	}

	target = G_Find(NULL, FOFS(targetname), ent->target);

	if(target == NULL) {
		G_LocLogger(LL_ERROR, "ui_holodekc without trigger_holodeck as target at %s!\n", vtos(ent->s.origin));
		G_FreeEntity(ent);
		G_LogFuncEnd();
		return;
	}

	ent->target_ent = target;
	ent->target_ent->target_ent = ent;

	ent->nextthink = -1;
	ent->think = ui_holodeck_think;

	G_LogFuncEnd();
}

void SP_ui_holodeck(gentity_t* ent) {

	G_LogFuncBegin();

	if (ent == NULL) {
		G_LocLogger(LL_ERROR, "ent == NULL!\n");
		G_LogFuncEnd();
		return;
	}

	ent->type = ENT_UI_HOLODECK;

	if(ent->target == NULL) {
		G_LocLogger(LL_ERROR, "ui_holodeck without target at %s!\n", vtos(ent->s.origin));
		G_FreeEntity(ent);
		G_LogFuncEnd();
		return;
	}

	if ((ent->spawnflags & UI_DIABLED) != 0) {
		ent->flags ^= FL_LOCKED;
	}

	ent->use = ui_holodeck_use;
	ent->think = ui_holodeck_setup;
	ent->nextthink = level.time + 500;
	ent->count = 0;
	trap_LinkEntity(ent);

	G_LogFuncEnd();
}

