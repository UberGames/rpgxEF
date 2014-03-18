#ifndef G_MISSILE_H_
#define G_MISSILE_H_

#include "g_local.h"

void G_Missile_Impact( gentity_t *ent, trace_t *trace);
void tripwireThink(gentity_t* ent);

#endif /* G_MISSILE_H_ */