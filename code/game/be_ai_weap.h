// Copyright (C) 1999-2000 Id Software, Inc.
//

/*****************************************************************************
 * name:		be_ai_weap.h
 *
 * desc:		weapon AI
 *
 * $Archive: /StarTrek/Code-DM/game/be_ai_weap.h $
 * $Author: Mgummelt $
 * $Revision: 2 $
 * $Modtime: 3/12/01 3:08p $
 * $Date: 3/12/01 4:43p $
 *
 *****************************************************************************/

#ifndef BE_AI_WEAP_H_
#define BE_AI_WEAP_H_

//projectile flags
enum be_ai_projectileFlag_e {
	PFL_WINDOWDAMAGE = 1,	//!<projectile damages through window
	PFL_RETURN = 2			//!<set when projectile returns to owner
};

//weapon flags
enum be_ai_weaponFlag_e {
	WFL_FIRERELEASED = 1		//!<set when projectile is fired with key-up event
};

//damage types
enum be_ai_damageType_e {
	DAMAGETYPE_IMPACT = 1,		//!<damage on impact
	DAMAGETYPE_RADIAL = 2,		//!<radial damage
	DAMAGETYPE_VISIBLE = 4		//!<damage to all entities visible to the projectile
};

typedef struct projectileinfo_s {
	char name[MAX_STRINGFIELD];
	char model[MAX_STRINGFIELD];
	int32_t flags;
	double gravity;
	int32_t damage;
	double radius;
	int32_t visdamage;
	int32_t damagetype;
	int32_t healthinc;
	double push;
	double detonation;
	double bounce;
	double bouncefric;
	double bouncestop;
} projectileinfo_t;

typedef struct weaponinfo_s {
	int32_t valid;					//!<true if the weapon info is valid
	int32_t number;									//!<number of the weapon
	char name[MAX_STRINGFIELD];
	char model[MAX_STRINGFIELD];
	int32_t level;
	int32_t weaponindex;
	int32_t flags;
	char projectile[MAX_STRINGFIELD];
	int32_t numprojectiles;
	double hspread;
	double vspread;
	double speed;
	double acceleration;
	vec3_t recoil;
	vec3_t offset;
	vec3_t angleoffset;
	double extrazvelocity;
	int32_t ammoamount;
	int32_t ammoindex;
	double activate;
	double reload;
	double spinup;
	double spindown;
	projectileinfo_t proj;						//!<pointer to the used projectile
} weaponinfo_t;

//!setup the weapon AI
int BotSetupWeaponAI(void);
//!shut down the weapon AI
void BotShutdownWeaponAI(void);
//!returns the best weapon to fight with
int BotChooseBestFightWeapon(int weaponstate, int *inventory, qboolean meleeRange);
//!returns the information of the current weapon
void BotGetWeaponInfo(int weaponstate, int weapon, weaponinfo_t *weaponinfo);
//!loads the weapon weights
int BotLoadWeaponWeights(int weaponstate, char *filename);
//!returns a handle to a newly allocated weapon state
int BotAllocWeaponState(void);
//!frees the weapon state
void BotFreeWeaponState(int weaponstate);
//!resets the whole weapon state
void BotResetWeaponState(int weaponstate);

#endif /* BE_AI_WEAP_H_ */
