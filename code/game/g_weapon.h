#ifndef _G_WEAPON_H
#define _G_WEAPON_H

#include "g_local.h"

vec3_t	forward;
vec3_t	right;
vec3_t	up;
vec3_t	muzzle;

struct weaponConfigPhaserP_s {
	int32_t damage;
	double range;
	int32_t variation;
};
struct weaponConfigPhaserS_s {
	int32_t damage;
	double radius;
	double range;
	int32_t variation;
};

struct weaponConfigPhaser_s {
	struct weaponConfigPhaserP_s primary;
	struct weaponConfigPhaserS_s secondary;
};

struct weaponConfigCRifleP_s {
	int32_t damage;
	double range;
};

struct weaponConfigCRifleS_s {
	int32_t damage;
	double range;
	int32_t size;
};

struct weaponConfigCRifle_s {
	struct weaponConfigCRifleP_s primary;
	struct weaponConfigCRifleS_s secondary;
};

struct weaponConfigDisruptorP_s {
	int32_t damage;
	int32_t range;
};

struct weaponConfigDisruptorS_s {
	int32_t damage;
	int32_t size;
};

struct weaponConfigDisruptor_s {
	struct weaponConfigDisruptorP_s primary;
	struct weaponConfigDisruptorS_s secondary;
};

struct weaponConfigSplash_s {
	int32_t radius;
	int32_t damage;
};

struct weaponConfigGrenadeP_s {
	int32_t damage;
	double velocity;
	int32_t time;
	double size;
	struct weaponConfigSplash_s splash;
};

struct weaponConfigGrenadeS_s {
	int32_t damage;
	int32_t time;
	struct weaponConfigSplash_s splash;
};

struct weaponConfigGrenade_s {
	struct weaponConfigGrenadeP_s primary;
	struct weaponConfigGrenadeS_s secondary;
};

struct weaponConfigTR116P_s {
	int32_t damage;
	int32_t range;
};

struct weaponConfigTR116_s {
	struct weaponConfigTR116P_s primary;
};

struct weaponConfigQuantumP_s {
	int32_t damage;
	double size;
	struct weaponConfigSplash_s splash;
};

struct weaponConfigQuantumSThinkSearch_s {
	int32_t time;
	double distance;
};

struct weaponConfigQuantumSThink_s {
	int32_t time;
	struct weaponConfigQuantumSThinkSearch_s search;
};

struct weaponConfigQuantumS_s {
	int32_t damage;
	double size;
	struct weaponConfigSplash_s splash;
	struct weaponConfigQuantumSThink_s think;
};

struct weaponConfigQuantum_s {
	struct weaponConfigQuantumP_s primary;
	struct weaponConfigQuantumS_s secondary;
};

struct weaponConfigHyperspannerP_s {
	int32_t rate;
};

struct weaponConfigHyperspannerS_s {
	int32_t rate;
};

struct weaponConfigHyperspanner_s {
	struct weaponConfigHyperspannerP_s primary;
	struct weaponConfigHyperspannerS_s secondary;
};

typedef struct weaponConfig_s weaponConfig_t;
struct weaponConfig_s {
	struct weaponConfigPhaser_s phaser;
	struct weaponConfigCRifle_s crifle;
	struct weaponConfigDisruptor_s disruptor;
	struct weaponConfigGrenade_s grenade;
	struct weaponConfigTR116_s tr116;
	struct weaponConfigQuantum_s quantum;
	struct weaponConfigHyperspanner_s hyperspanner;
};

void G_Weapon_LoadConfig(void);

#endif /* _G_WEAPON_H */
