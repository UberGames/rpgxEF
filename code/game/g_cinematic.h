#ifndef _G_CIN_H_
#define _G_CIN_H_
#include "g_local.h"

void Cinematic_ActivateCameraMode(gentity_t *ent, gentity_t *target);
void Cinematic_DeactivateCameraMode(gentity_t *ent);
void Cinematic_ActivateGlobalCameraMode(gentity_t *target);
void Cinematic_DeactivateGlobalCameraMode(void);

#endif //_G_CIN_H_
