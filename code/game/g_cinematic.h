// g_cinematic.h

#ifndef _G_CINEMATIC_H
#define _G_CINEMATIC_H

void Cinematic_ActivateCameraMode(gentity_t *ent, gentity_t *target);
void Cinematic_DeactivateCameraMode(gentity_t *ent);
void Cinematic_ActivateGlobalCameraMode(gentity_t *target);
void Cinematic_DeactivateGlobalCameraMode(void);

#endif // _G_CINEMATIC_H