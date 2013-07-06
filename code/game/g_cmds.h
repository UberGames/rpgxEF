#ifndef G_CMDS_H
#define G_CMDS_H

#include "g_local.h"

qboolean SetClass( gentity_t *ent, char *s, /*@null@*/ char *teamName, qboolean SaveToCvar );
void DragCheck( void );
pclass_t ValueNameForClass ( char* s );
void BroadcastClassChange( gclient_t *client, pclass_t oldPClass );

#endif