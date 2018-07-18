/*
 * Copyright (C) 1999-2000 Id Software, Inc.
 *
 * g_utils.c -- misc utility functions for game module
 *
 */


#include "g_local.h"
#include "g_lua.h"
#include "g_logger.h"
#include "g_combat.h"
#include "g_syscalls.h"

#include <algorithm>

 /**
  * \brief Data structure for a singele shader remap.
  *
  * Data structure for a single shader remap
  *
  */
typedef struct
{
  /** The original shader */
  char oldShader[MAX_QPATH];
  /** The replacement shader */
  char newShader[MAX_QPATH];
  /** time offset */
  float timeOffset;
} shaderRemap_t;

/** The maximum count of shader remaps */
enum g_utilsLimits_e
{
  MAX_SHADER_REMAPS = 128,
  MAXCHOICES = 32
};

/** Current count of remapped shaders  */
static uint8_t remapCount = 0;
/** List of shader remaps */
static shaderRemap_t remappedShaders[MAX_SHADER_REMAPS];

void AddRemap(const char* oldShader, const char* newShader, float timeOffset)
{
  uint8_t i;

  G_LogFuncBegin();

  if(oldShader == NULL)
  {
    G_LocLogger(LL_ERROR, "oldShader == NULL\n");
    G_LogFuncEnd();
    return;
  }

  if(newShader == NULL)
  {
    G_LocLogger(LL_ERROR, "newShader == NULL\n");
    G_LogFuncEnd();
    return;
  }

  for(i = 0; i < remapCount; i++)
  {
    if(Q_stricmp(oldShader, remappedShaders[i].oldShader) == 0)
    {
      /* found it, just update this one */
      strcpy(remappedShaders[i].newShader, newShader);
      remappedShaders[i].timeOffset = timeOffset;

      G_LogFuncEnd();
      return;
    }
  }

  if(remapCount < MAX_SHADER_REMAPS)
  {
    strcpy(remappedShaders[remapCount].newShader, newShader);
    strcpy(remappedShaders[remapCount].oldShader, oldShader);
    remappedShaders[remapCount].timeOffset = timeOffset;
    remapCount++;
  }

  G_LogFuncEnd();
}

/**
 * \brief Builds the shader state config.
 *
 * Builds the shader state config.
 *
 * @return the shader state config
 */
const char *BuildShaderStateConfig(void)
{
  static char	buff[MAX_STRING_CHARS * 4];
  char out[(MAX_QPATH * 2) + 5];
  uint8_t i;

  G_LogFuncBegin();

  memset(buff, 0, MAX_STRING_CHARS);
  for(i = 0; i < remapCount; i++)
  {
    Com_sprintf(out, (MAX_QPATH * 2) + 5, "%s=%s:%5.2f@", remappedShaders[i].oldShader, remappedShaders[i].newShader, remappedShaders[i].timeOffset);
    Q_strcat(buff, sizeof(buff), out);
  }

  G_LogFuncEnd();
  return buff;
}

/*
=========================================================================

model / sound configstring indexes

=========================================================================
*/

/*
 * \brief Finds the index a config string starts at.
 *
 * Finds the index a config string start at.
 *
 * @param name name of the config string
 * @param start look from here
 * @param max look until here
 * @param create create a new config string?
 *
 * @return The config strings index
 */
int32_t G_FindConfigstringIndex(char* name, int32_t start, int32_t max, qboolean create)
{
  int32_t	i = 0;
  char	s[MAX_STRING_CHARS];

  if(name == NULL || name[0] == 0)
  {
    G_LocLogger(LL_ERROR, "name == NULL\n");
    G_LogFuncEnd();
    return 0;
  }

  for(i = 1; i < max; i++)
  {
    trap_GetConfigstring(start + i, s, sizeof(s));
    if(!s[0])
    {
      break;
    }
    if(!strcmp(s, name))
    {
      G_LogFuncEnd();
      return i;
    }
  }

  if(!create)
  {
    G_LogFuncEnd();
    return 0;
  }

  if(i == max)
  {
    /*G_Error( "G_FindConfigstringIndex: overflow" );*/
    //G_Printf( S_COLOR_RED "G_FindConfigstringIndex: Full!! Could not add value: %s\n", name );
    G_LocLogger(LL_ERROR, "G_FindConfigstringIndex: Full!! Could not add value: %s\n", name);
  }

  trap_SetConfigstring(start + i, name);

  G_LogFuncEnd();
  return i;
}

int32_t G_ModelIndex(char* name)
{
  int32_t res = 0;

  G_LogFuncBegin();

  res = G_FindConfigstringIndex(name, CS_MODELS, MAX_MODELS, qtrue);

  G_LogFuncEnd();
  return res;
}

int32_t G_SoundIndex(char* name)
{
  int32_t res = 0;

  G_LogFuncBegin();

  res = G_FindConfigstringIndex(name, CS_SOUNDS, MAX_SOUNDS, qtrue);

  G_LogFuncEnd();
  return res;
}

int32_t G_TricStringIndex(char* name)
{
  int32_t res = 0;

  G_LogFuncBegin();

  res = G_FindConfigstringIndex(name, CS_TRIC_STRINGS, MAX_TRIC_STRINGS, qtrue);

  G_LogFuncEnd();
  return res;
}

//=====================================================================

void G_TeamCommand(team_t team, char* cmd)
{
  int32_t i;

  G_LogFuncBegin();

  if(cmd == NULL)
  {
    G_LocLogger(LL_ERROR, "cmd == NULL\n");
    G_LogFuncEnd();
  }

  for(i = 0; i < level.maxclients; i++)
  {
    if(level.clients[i].pers.connected == CON_CONNECTED)
    {
      if(level.clients[i].sess.sessionTeam == team)
      {
        trap_SendServerCommand(i, cmd);
      }
    }
  }

  G_LogFuncEnd();
}

gentity_t* G_Find(gentity_t* from, size_t fieldofs, const char* match)
{
  char* s = NULL;

  G_LogFuncBegin();

  if(match == NULL)
  {
    G_LocLogger(LL_ERROR, "match == NULL\n");
    G_LogFuncEnd();
    return NULL;
  }

  if(from == NULL)
  {
    from = g_entities;
  }
  else
  {
    from++;
  }

  for(; from < &g_entities[level.num_entities]; from++)
  {
    if(!from->inuse)
    {
      continue;
    }

    s = *(char **)((byte *)from + fieldofs);
    if(s == NULL)
    {
      continue;
    }

    if(Q_stricmp(s, match) == 0)
    {
      G_LogFuncEnd();
      return from;
    }
  }

  G_LogFuncEnd();
  return NULL;
}


/** Maximum number of possible choices for G_PickTarget. */
gentity_t* G_PickTarget(char* targetname)
{
  gentity_t*	ent = NULL;
  int32_t		num_choices = 0;
  int32_t		i = 0;
  gentity_t*	choice[MAXCHOICES];

  G_LogFuncBegin();

  if(targetname == NULL)
  {
    G_LocLogger(LL_ERROR, "G_PickTarget called with NULL targetname\n");
    G_LogFuncEnd();
    return NULL;
  }

  /* BOOKMARK */
  for(; i < MAXCHOICES; i++)
  {
    ent = G_Find(ent, FOFS(targetname), targetname);
    if(ent == NULL)
    {
      break;
    }

    choice[num_choices++] = ent;
    if(num_choices == MAXCHOICES)
    {
      break;
    }
  }

  /*================
  RPG-X Modification
  Phenix
  13/06/2004
  ================*/
  if(num_choices == 0)
  {
    for(i = 0; i < MAXCHOICES; i++)
    {
      ent = G_Find(ent, FOFS(swapname), targetname);
      if(ent == NULL)
      {
        break;
      }

      choice[num_choices++] = ent;

      if(num_choices == MAXCHOICES)
      {
        break;
      }
    }
  }

  if(num_choices == 0)
  {
    for(i = 0; i < MAXCHOICES; i++)
    {
      ent = G_Find(ent, FOFS(truename), targetname);
      if(ent == NULL)
      {
        break;
      }

      choice[num_choices++] = ent;

      if(num_choices == MAXCHOICES)
      {
        break;
      }
    }
  }

  if(num_choices == 0)
  {
    for(i = 0; i < MAXCHOICES; i++)
    {
      ent = G_Find(ent, FOFS(falsename), targetname);
      if(ent == NULL)
      {
        break;
      }

      choice[num_choices++] = ent;

      if(num_choices == MAXCHOICES)
      {
        break;
      }
    }
  }

  /*================
  End Modification
  ================*/

  if(num_choices == 0)
  {
    G_LocLogger(LL_ERROR, "G_PickTarget: target %s not found\n", targetname);
    G_LogFuncEnd();
    return NULL;
  }

  G_LogFuncEnd();
  return choice[rand() % num_choices];
}

void G_UseTargets2(gentity_t* ent, gentity_t* activator, char* target)
{
  gentity_t* t = NULL;

  G_LogFuncBegin();

  if(ent == NULL)
  {
    G_LocLogger(LL_ERROR, "ent == NULL\n");
    G_LogFuncEnd();
    return;
  }

  if(ent->targetShaderName != NULL && ent->targetShaderNewName != NULL)
  {
    float f = level.time * 0.001;
    AddRemap(ent->targetShaderName, ent->targetShaderNewName, f);
    trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
  }

  if(target == NULL)
  {
    G_LogFuncEnd();
    return;
  }

  t = NULL;
  while((t = G_Find(t, FOFS(targetname), target)) != NULL)
  {
    if(t == ent)
    {
      G_LocLogger(LL_WARN, "Entity %i used itself.\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
    }
    else
    {
      if(t->use)
      {
        t->use(t, ent, activator);
#ifdef G_LUA
        if(t->luaUse)
        {
          if(activator)
          {
            LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, activator->s.number);
          }
          else
          {
            LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, ENTITYNUM_WORLD);
          }
        }
#endif
      }
    }
    if(!ent->inuse)
    {
      G_LocLogger(LL_WARN, "Entity %i was removed while using targets\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
      G_LogFuncEnd();
      return;
    }
  }

  /*================
  RPG-X Modification
  Phenix
  13/06/2004
  ================*/
  t = NULL;
  while((t = G_Find(t, FOFS(swapname), target)) != NULL)
  {
    if(t == ent)
    {
      G_LocLogger(LL_WARN, "Entity %i used itself.\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
    }
    else
    {
      if(t->use)
      {
        t->use(t, ent, activator);
#ifdef G_LUA
        if(t->luaUse)
        {
          if(activator)
          {
            LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, activator->s.number);
          }
          else
          {
            LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, ENTITYNUM_WORLD);
          }
        }
#endif
      }
    }
    if(!ent->inuse)
    {
      G_LocLogger(LL_WARN, "Entity %i was removed while using targets\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
      G_LogFuncEnd();
      return;
    }
  }

  t = NULL;
  while((t = G_Find(t, FOFS(truename), target)) != NULL)
  {
    if(t == ent)
    {
      G_LocLogger(LL_WARN, "Entity %i used itself.\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
    }
    else
    {
      if(t->use)
      {
        t->use(t, ent, activator);
#ifdef G_LUA
        if(t->luaUse)
        {
          if(activator)
          {
            LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, activator->s.number);
          }
          else
          {
            LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, ENTITYNUM_WORLD);
          }
        }
#endif
      }
    }
    if(!ent->inuse)
    {
      G_LocLogger(LL_WARN, "Entity %i was removed while using targets\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
      G_LogFuncEnd();
      return;
    }
  }

  t = NULL;
  while((t = G_Find(t, FOFS(falsename), target)) != NULL)
  {
    if(t == ent)
    {
      G_LocLogger(LL_WARN, "Entity %i used itself.\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
    }
    else
    {
      if(t->use)
      {
        t->use(t, ent, activator);
#ifdef G_LUA
        if(t->luaUse)
        {
          if(activator)
          {
            LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, activator->s.number);
          }
          else
          {
            LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, ENTITYNUM_WORLD);
          }
        }
#endif
      }
    }
    if(!ent->inuse)
    {
      G_LocLogger(LL_WARN, "Entity %i was removed while using targets\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
      G_LogFuncEnd();
      return;
    }
  }
  /*================
  End Modification
  ================*/

  /*
   * RPG-X | GSIO01 | 11/05/2009 | MOD START
   * target_alert
   */
  t = NULL;
  while((t = G_Find(t, FOFS(bluename), target)) != NULL)
  {
    if(t == ent)
    {
      G_LocLogger(LL_WARN, "Entity %i used itself.\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
    }
    else
    {
      if(t->use)
      {
        t->use(t, ent, ent);
#ifdef G_LUA
        if(t->luaUse)
        {
          if(activator)
          {
            LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, activator->s.number);
          }
          else
          {
            LuaHook_G_EntityUse(t->luaUse, t->s.number, ent->s.number, ENTITYNUM_WORLD);
          }
        }
#endif
      }
    }
    if(!ent->inuse)
    {
      G_LocLogger(LL_WARN, "Entity %i was removed while using targets\n", t->s.number); /* RPG-X | GSIO01 | 22.10.09: a little bit more information for the mapper */
      G_LogFuncEnd();
      return;
    }
  }

  G_LogFuncEnd();
}

void G_UseTargets(gentity_t* ent, gentity_t* activator)
{
  G_LogFuncBegin();

  if(ent == NULL)
  {
    G_LocLogger(LL_ERROR, "ent == NULL\n");
    return;
  }

  G_UseTargets2(ent, activator, ent->target);

  G_LogFuncEnd();
}

float* tv(float x, float y, float z)
{
  static	int32_t	index;
  static	vec3_t	vecs[8];
  float*	v = NULL;

  G_LogFuncBegin();

  /*
   *  use an array so that multiple tempvectors won't collide
   *  for a while
   */
  v = vecs[index];
  index = (index + 1) & 7;

  v[0] = x;
  v[1] = y;
  v[2] = z;

  G_LogFuncEnd();
  return v;
}

char* vtos(const vec3_t v)
{
  static	int32_t	index;
  static	char	str[8][32];
  char*	s = NULL;

  G_LogFuncBegin();

  /* use an array so that multiple vtos won't collide */
  s = str[index];
  index = (index + 1) & 7;

  Com_sprintf(s, 32, "(%i %i %i)", (int)v[0], (int)v[1], (int)v[2]);

  G_LogFuncEnd();
  return s;
}

void G_SetMovedir(vec3_t angles, vec3_t movedir)
{
  static vec3_t VEC_UP = { 0, -1, 0 };
  static vec3_t MOVEDIR_UP = { 0, 0, 1 };
  static vec3_t VEC_DOWN = { 0, -2, 0 };
  static vec3_t MOVEDIR_DOWN = { 0, 0, -1 };

  G_LogFuncBegin();

  if(VectorCompare(angles, VEC_UP))
  {
    VectorCopy(MOVEDIR_UP, movedir);
  }
  else if(VectorCompare(angles, VEC_DOWN))
  {
    VectorCopy(MOVEDIR_DOWN, movedir);
  }
  else
  {
    AngleVectors(angles, movedir, NULL, NULL);
  }
  VectorClear(angles);

  G_LogFuncEnd();
}

float vectoyaw(const vec3_t vec)
{
  float	yaw;

  G_LogFuncBegin();

  if(vec[YAW] == 0 && vec[PITCH] == 0)
  {
    yaw = 0;
  }
  else
  {
    if(vec[PITCH])
    {
      yaw = (atan2(vec[YAW], vec[PITCH]) * 180 / M_PI);
    }
    else if(vec[YAW] > 0)
    {
      yaw = 90;
    }
    else
    {
      yaw = 270;
    }
    if(yaw < 0)
    {
      yaw += 360;
    }
  }

  G_LogFuncEnd();
  return yaw;
}

void G_InitGentity(gentity_t* e)
{
  G_LogFuncBegin();

  if(e == NULL)
  {
    G_LocLogger(LL_ERROR, "e == NULL\n");
    G_LogFuncEnd();
    return;
  }

  e->inuse = qtrue;
  e->classname = "noclass";
  e->s.number = e - g_entities;
  e->r.ownerNum = ENTITYNUM_NONE;

  G_LogFuncEnd();
}

gentity_t* G_Spawn(void)
{
  int32_t		i = 0;
  int32_t		force = 0;
  gentity_t*	e = NULL;
  /* RPG-X: RedTechie - Get rid of tripmines first */
  gentity_t*	tripwire = NULL;
  int32_t		foundTripWires[MAX_GENTITIES] = { ENTITYNUM_NONE };
  int32_t		tripcount = 0;

  e = NULL;	/* shut up warning */
  i = 0;		/* shut up warning */
  for(force = 0; force < 2; force++)
  {
    /* if we go through all entities and can't find one to free, */
    /* override the normal minimum times before use */
    e = &g_entities[MAX_CLIENTS];

    if(e == NULL)
    {
      continue;
    }

    for(i = MAX_CLIENTS; i < level.num_entities; i++, e++)
    {
      if(e->inuse)
      {
        continue;
      }

      /* the first couple seconds of server time can involve a lot of */
      /* freeing and allocating, so relax the replacement policy */
      if(!force && e->freetime > level.startTime + 2000 && level.time - e->freetime < 1000)
      {
        continue;
      }

      /* reuse this slot */
      G_InitGentity(e);
      return e;
    }
    if(i != ENTITYNUM_MAX_NORMAL)
    {
      break;
    }
  }

  /* RPG-X: RedTechie DEBUG: SHOW HOW MANY ENT's WE HAVE LEFT */
  /*trap_SendServerCommand( -1, va("print \"^1DEBUG: current:%i total:%i\n\"", i, ENTITYNUM_MAX_NORMAL));*/

  if(i == ENTITYNUM_MAX_NORMAL)
  {
    /* RPG-X: RedTechie - Do some rpg-x house cleaning before we decalre the server dead */
    while((tripwire = G_Find(tripwire, FOFS(classname), "tripwire")) != NULL)
    {
      foundTripWires[tripcount++] = tripwire->s.number;
    }

    if(tripcount != 0)
    {
      for(i = 0; i < tripcount; i++)
      {
        /* remove it... or blow it? */
        if(&g_entities[foundTripWires[i]] != NULL)
        {
          G_FreeEntity(&g_entities[foundTripWires[i]]);
          foundTripWires[i] = ENTITYNUM_NONE;
        }
      }

      G_LogPrintf("RPG-X WARNING: Max entities hit! Removed all tripmines. Restart the server ASAP or suffer a server crash!\n");
      trap_SendServerCommand(-1, "print \"^1RPG-X WARNING: Max entities hit! Removed all tripmines. Restart the server ASAP or suffer a server crash!\n\"");

      if(i == ENTITYNUM_MAX_NORMAL)
      {
        G_Error("G_Spawn: no free entities");
      }
    }
    else
    {
      G_Error("G_Spawn: no free entities");
    }
  }

  /* RPG-X: RedTechie - Update global entity count */
  RPGEntityCount = i;

  /* open up a new slot */
  level.num_entities++;

  /* let the server system know that there are more entities */
  trap_LocateGameData(level.gentities, level.num_entities, sizeof(gentity_t),
    &level.clients[0].ps, sizeof(level.clients[0]));

  G_InitGentity(e);
  G_LogFuncEnd();
  return e;
}

void G_FreeEntity(gentity_t* ed)
{
  G_LogFuncBegin();

  if(ed == NULL)
  {
    G_LocLogger(LL_ERROR, "ed == NULL\n");
    G_LogFuncEnd();
    return;
  }

  trap_UnlinkEntity(ed);		/* unlink from world */

  if(ed->neverFree)
  {
    return;
  }

#ifdef G_LUA
  /* Lua API callbacks */
  if(ed->luaFree != NULL && !ed->client)
  {
    LuaHook_G_EntityFree(ed->luaFree, ed->s.number);
  }
#endif

  memset(ed, 0, sizeof(*ed));
  ed->classname = "freed";
  ed->freetime = level.time;
  ed->inuse = qfalse;
  ed->type = EntityType::ENT_FREE;

  G_LogFuncEnd();
}

gentity_t* G_TempEntity(vec3_t origin, int event)
{
  gentity_t*	e = NULL;
  vec3_t		snapped;

  G_LogFuncBegin();

  e = G_Spawn();

  if(e == NULL)
  {
    G_LocLogger(LL_ERROR, "e == NULL\n");
    G_LogFuncEnd();
    return NULL;
  }
  e->s.eType = ET_EVENTS + event;

  e->classname = "tempEntity";
  e->eventTime = level.time;
  e->freeAfterEvent = qtrue;

  VectorCopy(origin, snapped);
  SnapVector(snapped);		/* save network bandwidth */
  G_SetOrigin(e, snapped);

  /* find cluster for PVS */
  trap_LinkEntity(e);

  G_LogFuncEnd();
  return e;
}



/*
==============================================================================

Kill box

==============================================================================
*/

void G_KillBox(gentity_t* ent)
{
  int32_t		i = 0;
  int32_t		num = 0;
  int32_t		touch[MAX_GENTITIES];
  gentity_t*	hit = NULL;
  vec3_t		mins = { 0, 0, 0 };
  vec3_t		maxs = { 0, 0, 0 };

  G_LogFuncBegin();

  VectorAdd(ent->client->ps.origin, ent->r.mins, mins);
  VectorAdd(ent->client->ps.origin, ent->r.maxs, maxs);
  num = trap_EntitiesInBox(mins, maxs, touch, MAX_GENTITIES);

  for(i = 0; i < num; i++)
  {
    hit = &g_entities[touch[i]];
    if(hit->client == NULL)
    {
      continue;
    }

    /* nail it */
    G_Combat_Damage(hit, ent, ent, NULL, NULL,
      100000, DAMAGE_NO_PROTECTION, MOD_TELEFRAG);
  }

  G_LogFuncEnd();
}

qboolean G_MoveBox(gentity_t* ent)
{
  int32_t		i = 0;
  int32_t		num = 0;
  int32_t		touch[MAX_GENTITIES];
  gentity_t*	hit = 0;
  vec3_t		mins = { 0, 0, 0 };
  vec3_t		maxs = { 0, 0, 0 };
  vec3_t		dir = { 0, 0, 0 };
  qboolean	movedPlayer = qfalse;

  G_LogFuncBegin();

  if(ent == NULL)
  {
    G_LocLogger(LL_ERROR, "ent == NULL\n");
    G_LogFuncEnd();
    return qfalse;
  }

  VectorAdd(ent->client->ps.origin, ent->r.mins, mins);
  VectorAdd(ent->client->ps.origin, ent->r.maxs, maxs);

  num = trap_EntitiesInBox(mins, maxs, touch, MAX_GENTITIES);

  for(i = 0; i < num; i++)
  {
    hit = &g_entities[touch[i]];

    if(hit == NULL)
    {
      continue;
    }

    if(hit->client == NULL || hit->client->ps.clientNum == ent->client->ps.clientNum)
    {
      continue;
    }

    VectorSet(dir, 0, hit->client->ps.viewangles[YAW], 0);
    AngleVectors(dir, hit->client->ps.velocity, NULL, NULL);
    VectorScale(hit->client->ps.velocity, -150, hit->client->ps.velocity);
    hit->client->ps.pm_time = 160;		/* hold time */
    hit->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;

    movedPlayer = qtrue;
  }

  G_LogFuncEnd();
  return movedPlayer;
}

//==============================================================================

void G_AddPredictableEvent(gentity_t* ent, int32_t event, int32_t eventParm)
{
  G_LogFuncBegin();

  if(ent == NULL)
  {
    G_LocLogger(LL_ERROR, "ent == NULL\n");
    G_LogFuncEnd();
    return;
  }

  if(ent->client == NULL)
  {
    G_LocLogger(LL_ERROR, "ent->client == NULL\n");
    G_LogFuncEnd();
    return;
  }

  BG_AddPredictableEventToPlayerstate(event, eventParm, &ent->client->ps);

  G_LogFuncEnd();
}

void G_AddEvent(gentity_t* ent, int32_t event, int32_t eventParm)
{
  int32_t			bits;
  playerState_t*	ps = NULL;

  G_LogFuncBegin();

  if(ent == NULL)
  {
    G_LocLogger(LL_ERROR, "ent == NULL\n");
    G_LogFuncEnd();
    return;
  }

  ps = &ent->client->ps;

  if(event == 0)
  {
    G_LocLogger(LL_ERROR, "G_AddEvent: zero event added for entity %i\n", ent->s.number);
    G_LogFuncEnd();
    return;
  }

  /* clients need to add the event in playerState_t instead of entityState_t */
  if(ent->client)
  {
    bits = ps->externalEvent & EV_EVENT_BITS;
    bits = (bits + EV_EVENT_BIT1) & EV_EVENT_BITS;
    ps->externalEvent = event | bits;
    ps->externalEventParm = eventParm;
    ps->externalEventTime = level.time;
  }
  else
  {
    bits = ent->s.event & EV_EVENT_BITS;
    bits = (bits + EV_EVENT_BIT1) & EV_EVENT_BITS;
    ent->s.event = event | bits;
    ent->s.eventParm = eventParm;
  }
  ent->eventTime = level.time;

  G_LogFuncEnd();
}

void G_Sound(gentity_t* ent, int32_t soundIndex)
{
  gentity_t* te = NULL;

  G_LogFuncBegin();

  if(ent == NULL)
  {
    G_LocLogger(LL_ERROR, "ent == NULL\n");
    G_LogFuncEnd();
    return;
  }

  te = G_TempEntity(ent->r.currentOrigin, EV_GENERAL_SOUND);

  if(te == NULL)
  {
    G_LocLogger(LL_ERROR, "te == NULL\n");
    G_LogFuncEnd();
    return;
  }

  te->s.eventParm = soundIndex;

  G_LogFuncEnd();
}


//==============================================================================

void G_SetOrigin(gentity_t* ent, vec3_t origin)
{
  G_LogFuncBegin();

  if(ent == NULL)
  {
    G_LocLogger(LL_ERROR, "ent == NULL\n");
    G_LogFuncEnd();
    return;
  }

  // lets try this to fix setting origins for brush ents without origin brush
  VectorCopy(origin, ent->pos1); // needs testing

  VectorCopy(origin, ent->s.pos.trBase);
  ent->s.pos.trType = TR_STATIONARY;
  ent->s.pos.trTime = 0;
  ent->s.pos.trDuration = 0;
  VectorClear(ent->s.pos.trDelta);

  VectorCopy(origin, ent->r.currentOrigin);
  VectorCopy(origin, ent->s.origin); /* RPG-X | GSIO01 | 24.08.2009 */

  G_LogFuncEnd();
}

void G_SetAngles(gentity_t* ent, vec3_t angles)
{
  G_LogFuncBegin();

  if(ent == NULL)
  {
    G_LocLogger(LL_ERROR, "ent == NULL\n");
    G_LogFuncEnd();
    return;
  }

  VectorCopy(angles, ent->s.apos.trBase);
  ent->s.apos.trType = TR_STATIONARY;
  ent->s.apos.trTime = 0;
  ent->s.apos.trDuration = 0;
  VectorClear(ent->s.apos.trDelta);

  VectorCopy(angles, ent->r.currentAngles);
  VectorCopy(angles, ent->s.angles);

  G_LogFuncEnd();
}

/**
 * \brief Get list of entities around a given origin,
 *
 * Given an origin and a radius, return all entities that are in use that are within the list
 *
 * @param origin the origin
 * @param radius the radius to look in around
 * @param ignore entity to ignore
 * @param takeDamage only list ents that may get damaged?
 * @param ent_list the resulting list
 *
 * @return the number of found entities in the list
 */
std::vector<gentity_t*> G_RadiusList(vec3_t origin, double radius, const std::vector<gentity_t*>& ignore, qboolean takeDamage)
{
  gentity_t*	ent = nullptr;
  int32_t		entityList[MAX_GENTITIES];
  vec3_t		mins = { 0, 0, 0 };
  vec3_t		maxs = { 0, 0, 0 };
  vec3_t		v = { 0, 0, 0 };
  std::vector<gentity_t*> result;

  if(radius < 1)
  {
    radius = 1;
  }

  for(auto i = 0; i < 3; i++)
  {
    mins[i] = origin[i] - radius;
    maxs[i] = origin[i] + radius;
  }

  auto numListedEntities = trap_EntitiesInBox(mins, maxs, entityList, MAX_GENTITIES);

  for(auto e = 0; e < numListedEntities; e++)
  {
    auto n = qfalse;
    ent = &g_entities[entityList[e]];

    if(ent == nullptr)
    {
      continue;
    }

    if(!(ent->inuse) || ent->takedamage != takeDamage)
    {
      continue;
    }

    if(!ignore.empty())
    {
      if(std::any_of(ignore.begin(), ignore.end(), [&ent](gentity_t* t) { return t == ent; }))
      {
        n = qtrue;
        break;
      }
    }

    if(n == qtrue)
    {
      continue;
    }

    /* find the distance from the edge of the bounding box */
    for(auto i = 0; i < 3; i++)
    {
      if(origin[i] < ent->r.absmin[i])
      {
        v[i] = ent->r.absmin[i] - origin[i];
      }
      else if(origin[i] > ent->r.absmax[i])
      {
        v[i] = origin[i] - ent->r.absmax[i];
      }
      else
      {
        v[i] = 0;
      }
    }

    auto dist = VectorLength(v);
    if(dist >= radius)
    {
      continue;
    }

    /* ok, we are within the radius, add us to the incoming list */
    result.push_back(ent);
  }

  return result;
}

std::vector<gentity_t*> G_RadiusListOfTypes(const std::vector<std::string>& classnames, vec3_t origin, double radius, const std::vector<gentity_t*>& ignore)
{
  int32_t		entityList[MAX_GENTITIES];
  vec3_t		mins = { 0, 0, 0 };
  vec3_t		maxs = { 0, 0, 0 };
  vec3_t		v = { 0, 0, 0 };

  if(classnames.empty())
  {
    G_LocLogger(LL_ERROR, "classnames is empty\n");
    return {};
  }

  std::vector<gentity_t*> result;

  if(radius < 1)
  {
    radius = 1;
  }

  for(auto i = 0; i < 3; i++)
  {
    mins[i] = origin[i] - radius;
    maxs[i] = origin[i] + radius;
  }

  auto numListedEntities = trap_EntitiesInBox(mins, maxs, entityList, MAX_GENTITIES);

  for(auto e = 0; e < numListedEntities; e++)
  {
    auto ent = &g_entities[entityList[e]];

    if(ent == NULL)
    {
      continue;
    }

    if(!(ent->inuse))
    {
      continue;
    }

    if(!ignore.empty() && std::any_of(ignore.begin(), ignore.end(), [&ent](gentity_t* t) { return t == ent; }))
    {
      continue;
    }

    if(std::all_of(classnames.begin(), classnames.end(), [&ent](const std::string& c)
    { 
      return strcmp(ent->classname, c.data()) != 0;
    }))
    {
      continue;
    }

    /* find the distance from the edge of the bounding box */
    for(auto i = 0; i < 3; i++)
    {
      if(origin[i] < ent->r.absmin[i])
      {
        v[i] = ent->r.absmin[i] - origin[i];
      }
      else if(origin[i] > ent->r.absmax[i])
      {
        v[i] = origin[i] - ent->r.absmax[i];
      }
      else
      {
        v[i] = 0;
      }
    }

    auto dist = VectorLength(v);
    if(dist >= radius)
    {
      continue;
    }

    /* ok, we are within the radius, add us to the incoming list */
    result.push_back(ent);
  }

  /*  we are done, return how many we found */
  return result;
}

/**
 * \brief Find the nearest entity
 *
 * Find the nearest entity.
 *
 * @param classname filter the results by this classname
 * @param origin the origin
 * @param the radoius to look in around
 * @param ignore entity to ignore
 * @param takeDamage only return an entity that may take damage?
 *
 * @return the nearest entity
 */
gentity_t* G_GetNearestEnt(char* classname, vec3_t origin, double radius, std::vector<gentity_t*>& ignore, qboolean takeDamage)
{
  gentity_t*	nearest = NULL;
  double		distance = 0.0;
  double		minDist = 0.0;
  vec3_t		dist = { 0, 0, 0 };

  G_LogFuncBegin();

  if(radius <= 0.0)
  { /* we don't care how far it is away */
    radius = 9999999;
  }
  minDist = radius;

  auto entList = G_RadiusList(origin, radius, ignore, takeDamage);

  for(auto t : entList)
  {
    if(t == nullptr)
    {
      continue;
    }

    if(t->s.origin[0] || t->s.origin[1] || t->s.origin[2])
    {
      VectorSubtract(origin, t->s.origin, dist);
    }
    else if(t->r.currentOrigin[0] || t->r.currentOrigin[1] || t->r.currentOrigin[2])
    {
      VectorSubtract(origin, t->r.currentOrigin, dist);
    }
    else if(t->s.pos.trBase[0] || t->s.pos.trBase[1] || t->s.pos.trBase[2])
    {
      VectorSubtract(origin, t->s.pos.trBase, dist);
    }
    else
    { /* wow none of above ... well then assume it's origin is 0 0 0*/
      VectorCopy(origin, dist);
    }
    distance = VectorLength(dist);
    if(distance < 0)
    {
      distance *= -1;
    }
    if(distance < minDist)
    {
      if(classname && !Q_stricmp(classname, t->classname))
      {
        minDist = distance;
        nearest = t;
      }
      else if(!classname)
      {
        minDist = distance;
        nearest = t;
      }
    }
  }

  G_LogFuncEnd();
  return nearest;
}

/**
 * \brief Find the nearest player.
 *
 * Find the nearest player
 *
 * @param origin the origin
 * @param radius the radius to look in around
 * @param ignore entity to ignore
 *
 * @return the nearest player
 */
gentity_t* G_GetNearestPlayer(vec3_t origin, double radius, const std::vector<gentity_t*>& ignore)
{
  gentity_t*	nearest = NULL;
  double		distance = 0.0;
  double		minDist = 0.0;
  vec3_t		dist = { 0, 0, 0 };

  G_LogFuncBegin();

  if(radius <= 0.0)
  {
    radius = 999999;
  }
  minDist = radius;

  auto entList = G_RadiusList(origin, radius, ignore, qtrue);

  for(auto t : entList)
  {
    if(t == NULL)
    {
      continue;
    }

    if(t->client)
    {
      VectorSubtract(origin, t->r.currentOrigin, dist);
      distance = VectorLength(dist);
      if(distance < 0)
        distance *= -1;
      if(distance < minDist)
      {
        minDist = distance;
        nearest = t;
      }
    }
  }

  G_LogFuncEnd();
  return nearest;
}

int32_t G_GetEntityByTargetname(const char* targetname, std::vector<gentity_t*>& entities)
{
  int32_t i = 0;
  gentity_t* t = NULL;

  G_LogFuncBegin();

  if(entities.empty())
  {
    G_LocLogger(LL_ERROR, "entities is empty\n");
    G_LogFuncEnd();
    return 0;
  }

  if(targetname == NULL)
  {
    G_LogFuncEnd();
    return 0;
  }

  for(i = MAX_GENTITIES - 1; i > -1; i--)
  {
    if(&g_entities[i] == NULL)
    {
      continue;
    }

    t = &g_entities[i];

    if(t->targetname && !Q_strncmp(t->targetname, targetname, strlen(targetname)))
    {
      entities.push_back(t);
    }
  }

  G_LogFuncEnd();
  return entities.size();
}

std::vector<gentity_t*> G_GetEntityByTarget(const char* target)
{
  uint16_t i = 0;
  gentity_t* t = NULL;
  std::vector<gentity_t*> entities;

  G_LogFuncBegin();

  if(entities.empty())
  {
    G_LocLogger(LL_ERROR, "entities is empty\n");
    G_LogFuncEnd();
    return {};
  }

  if(target == NULL)
  {
    G_LogFuncEnd();
    return {};
  }

  for(i = MAX_GENTITIES - 1; i > -1; i--)
  {
    if(&g_entities[i] == NULL)
    {
      continue;
    }

    t = &g_entities[i];

    if(t->target && !Q_strncmp(t->target, target, strlen(target)))
    {
      entities.push_back(t);
    }
  }

  return entities;
}

int32_t G_GetEntityByBmodel(char* bmodel, std::vector<gentity_t*>& entities)
{
  uint16_t i = 0;
  gentity_t* t = NULL;

  G_LogFuncBegin();

  if(entities.empty())
  {
    G_LocLogger(LL_ERROR, "entities is empty\n");
    G_LogFuncEnd();
    return 0;
  }

  if(bmodel == NULL)
  {
    G_LogFuncEnd();
    return 0;
  }

  for(i = MAX_GENTITIES - 1; i > -1; i--)
  {
    if(&g_entities[i] == NULL)
    {
      continue;
    }

    t = &g_entities[i];

    if(t->model && !Q_strncmp(t->model, bmodel, strlen(bmodel)))
    {
      entities.push_back(t);
    }
  }

  G_LogFuncEnd();
  return entities.size();
}

/**
*	Checks if the line of sight between two entities is blocked.
*
*	\author Ubergames - Phenix
*	\date 2/8/2004
*
*	@param ent1 entity one
*	@param ent2 entity two
*
*	@return is line of sight blocked?
*/
qboolean LineOfSight(gentity_t* ent1, gentity_t* ent2)
{
  trace_t trace;

  G_LogFuncBegin();

  memset(&trace, 0, sizeof(trace_t));
  trap_Trace(&trace, ent1->s.pos.trBase, NULL, NULL, ent2->s.pos.trBase, ent1->s.number, MASK_SHOT);

  if(trace.contents & CONTENTS_SOLID)
  {
    G_LogFuncEnd();
    return qfalse;
  }

  G_LogFuncEnd();
  return qtrue;
}

