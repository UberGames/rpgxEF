// entity lib for lua

#include "g_lua.h"
#include "g_spawn.h"

#ifdef G_LUA
// entity.GetTarget(entity ent)
// returns a target entity of ent
static int Entity_GetTarget(lua_State * L)
{
	lent_t     *lent;
	gentity_t      *t = NULL;

	lent = Lua_GetEntity(L, 1);
	t = G_PickTarget(lent->e->target);
	if(!lent || !lent->e || !t) {
		lua_pushnil(L);
		return 1;
	}
	Lua_PushEntity(L, t);

	return 1;
}

// entity.FindBModel(integer bmodelnum)
// Returns the entity with the brush model bmodelnumber. 
// This is the only failsafe way to find brush entities as the 
// entity number is different when you load a map local or join a server.
static int Entity_FindBModel(lua_State *L) {
	gentity_t	*ent;
	int			bmodel;

	bmodel = luaL_checkint(L, 1);
	ent = G_Find(NULL, FOFS(model), va("*%i", bmodel));
	if(!ent)
		lua_pushnil(L);
	else
		Lua_PushEntity(L, ent);

	return 1;
}

// entity.FindNumber(integer entnum)
// Returns the entity with the entity number entnum.
static int Entity_FindNumber(lua_State * L)
{
	int			 entnum;
	gentity_t	*ent;

	entnum = luaL_checknumber(L, 1);
	ent = &g_entities[entnum];
	if(!ent || !ent->inuse)
		lua_pushnil(L);
	else
		Lua_PushEntity(L, ent);

	return 1;
}

// entity.Find(string targetname)
// Returns the first entity found that has a targetname of targetname.
static int Entity_Find(lua_State * L)
{
	gentity_t      *t = NULL;

	t = G_Find(t, FOFS(targetname), (char *)luaL_checkstring(L, 1));
	if(!t)
		lua_pushnil(L);
	else
		Lua_PushEntity(L, t);

	return 1;
}

// entity.Use(entity ent)
// Uses ent.
static int Entity_Use(lua_State * L)
{
	lent_t     *lent;

	lent = Lua_GetEntity(L, 1);

	if(!lent || !lent->e || !lent->e->use) return 1;

	if(lent->e->luaUse)
		LuaHook_G_EntityUse(lent->e->luaUse, lent->e-g_entities, lent->e-g_entities, lent->e-g_entities);
	lent->e->use(lent->e, NULL, lent->e);

	return 1;
}

// entity.Teleport(entity client, entity target)
// Teleports client to target's position
static int Entity_Teleport(lua_State * L)
{
	lent_t     *lent;
	lent_t     *target;


	lent = Lua_GetEntity(L, 1);
	target = Lua_GetEntity(L, 2);

	if(!lent || !lent->e)
		return 1;
	if(!target || !target->e)
		return 1;

	if(lent->e->client)
		TeleportPlayer(lent->e, target->e->s.origin, target->e->s.angles, TP_NORMAL);
	return 1;
}


// entity.IsRocket(entity ent)
// Checks if an entity is a rocket.
static int Entity_IsRocket(lua_State * L)
{
	lent_t     *lent;
	qboolean    rocket = qfalse;

	lent = Lua_GetEntity(L, 1);
	if(lent->e && !Q_stricmp(lent->e->classname, "rocket"))
		rocket = qtrue;

	lua_pushboolean(L, (int)rocket);

	return 1;
}

// entity.IsGrenade(entity ent)
// Checks if an entity is a grenade.
static int Entity_IsGrenade(lua_State * L)
{
	lent_t     *lent;
	qboolean    grenade = qfalse;

	lent = Lua_GetEntity(L, 1);
	if(Q_stricmp(lent->e->classname, "grenade"))
		grenade = qtrue;

	lua_pushboolean(L, grenade);

	return 1;
}

// entity.Spawn()
// Tries to spawn a new entity and returns it. 
// If no new entity can be spawned nil is returned.
static int Entity_Spawn(lua_State * L)
{
	gentity_t *ent;

	ent = G_Spawn();
	Lua_PushEntity(L, ent);

	return 1;
}

// entity.GetNumber(entity ent)
// returns the entities index number
static int Entity_GetNumber(lua_State * L)
{
	lent_t     *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		lua_pushinteger(L, -1);
	else
		lua_pushinteger(L, lent->e - g_entities);

	return 1;
}

// entity.IsClient(entity ent)
// Checks if an entity is a client
static int Entity_IsClient(lua_State * L)
{
	lent_t     *lent;

	lent = Lua_GetEntity(L, 1);
	
	if(!lent || !lent->e) {
		lua_pushboolean(L, 0);
		return 1;
	}

	lua_pushboolean(L, lent->e->client != NULL);

	return 1;
}

// entity.GetClientName(entity ent)
// Returns the display name of a client
static int Entity_GetClientName(lua_State * L)
{
	lent_t     *lent;

	lent = Lua_GetEntity(L, 1);

	if(!lent || !lent->e || !lent->e->classname) {
		lua_pushnil(L);
		return 1;
	}

	lua_pushstring(L, lent->e->client->pers.netname);

	return 1;
}

static int Entity_Print(lua_State * L)
{
	lent_t     *lent;
	int             i;
	char            buf[MAX_STRING_CHARS];
	int             n = lua_gettop(L);

	lent = Lua_GetEntity(L, 1);

	if(!lent|| !lent->e) return 1;

	if(!lent->e->client)
		return luaL_error(L, "\'Print\' must be used with a client entity");

	memset(buf, 0, sizeof(buf));

	lua_getglobal(L, "tostring");
	for(i = 2; i <= n; i++)
	{
		const char     *s;

		lua_pushvalue(L, -1);
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);
		s = lua_tostring(L, -1);

		if(s == NULL)
			return luaL_error(L, "\'tostring\' must return a string to \'print\'");

		Q_strcat(buf, sizeof(buf), s);

		lua_pop(L, 1);
	}

	trap_SendServerCommand(lent->e - g_entities, va("print \"%s\n\"", buf));

	return 1;
}

static int Entity_CenterPrint(lua_State * L)
{
	lent_t     *lent;
	int             i;
	char            buf[MAX_STRING_CHARS];
	int             n = lua_gettop(L);

	lent = Lua_GetEntity(L, 1);

	if(!lent || !lent->e) return 1;

	if(!lent->e->client)
		return luaL_error(L, "\'CenterPrint\' must be used with a client entity");

	memset(buf, 0, sizeof(buf));

	lua_getglobal(L, "tostring");
	for(i = 2; i <= n; i++)
	{
		const char     *s;

		lua_pushvalue(L, -1);
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);
		s = lua_tostring(L, -1);

		if(s == NULL)
			return luaL_error(L, "\'tostring\' must return a string to \'print\'");

		Q_strcat(buf, sizeof(buf), s);

		lua_pop(L, 1);
	}

	trap_SendServerCommand(lent->e - g_entities, va("cp \"" S_COLOR_WHITE "%s\n\"", buf));

	return 1;
}

extern qboolean G_ParseField(const char *key, const char *value, gentity_t *ent);
// entity.SetKeyValue(entity ent, string key, string value)
// Sets a key of an entity to a value
static int Entity_SetKeyValue(lua_State * L) {
	lent_t		*lent;
	char		*key, *value;
	
	lent = Lua_GetEntity(L, 1);
	key = (char *)luaL_checkstring(L, 2);
	value = (char *)luaL_checkstring(L, 3);

	if(!lent || !lent->e || !key[0] || !value[0]) {
		lua_pushboolean(L, qfalse);
		return 1;
	}

	lua_pushboolean(L, G_ParseField(key, value, lent->e));
	return 1;
}

// entity.GetClassname(entity ent)
// Returns the classname of an entity
static int Entity_GetClassName(lua_State * L)
{
	lent_t     *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->classname);

	return 1;
}

// entity.SetClassname(entity ent, string name)
// Sets the Classname of an entity to name
static int Entity_SetClassName(lua_State * L)
{
	lent_t     *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->classname = (char *)luaL_checkstring(L, 2);

	return 1;
}

// entity.GetTargetname(entity ent)
// Returns the targetname of an entity
static int Entity_GetTargetName(lua_State * L)
{
	lent_t     *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->targetname);

	return 1;
}

// entity.Rotate(entity ent, vector dir)
// Rotates an entity in the specified directions
static int Entity_Rotate(lua_State * L)
{
	lent_t     *lent;
	vec_t          *vec;

	lent = Lua_GetEntity(L, 1);
	vec = Lua_GetVector(L, 2);

	lent->e->s.apos.trType = TR_LINEAR;
	lent->e->s.apos.trDelta[0] = vec[0];
	lent->e->s.apos.trDelta[1] = vec[1];
	lent->e->s.apos.trDelta[2] = vec[2];

	return 1;
}

static int Entity_GC(lua_State * L)
{

	return 0;
}

// entity.ToString(entity)
// Prints an entity as string
static int Entity_ToString(lua_State * L)
{
	lent_t     *lent;
	gentity_t      *gent;
	char            buf[MAX_STRING_CHARS];

	lent = Lua_GetEntity(L, 1);
	gent = lent->e;
	Com_sprintf(buf, sizeof(buf), "entity: class=%s name=%s id=%i pointer=%p\n", gent->classname, gent->targetname, gent - g_entities,
				gent);
	lua_pushstring(L, buf);

	return 1;
}

extern qboolean G_CallSpawn(gentity_t *ent);

static void ent_delay(gentity_t *ent) {
	ent->think = 0;
	ent->nextthink = -1;
	G_CallSpawn(ent);
}

// entity.DelayedCallSpawn(entity ent, integer delay)
// Calls the game logic spawn function for the class of ent after a delay 	of delay milliseconds.
static int Entity_DelayedCallSpawn(lua_State *L) {
	lent_t *lent;
	int		delay;

	lent = Lua_GetEntity(L, 1);

	if(!lent || !lent->e)
		return 1;

	delay = luaL_checkint(L, 2);

	if(!delay)
		delay = FRAMETIME;

	lent->e->nextthink = delay;
	lent->e->think = ent_delay;

	return 1;
}

// entity.CallSpawn(entity ent)
// Calls the game logic spawn function for the class of ent.
static int Entity_CallSpawn(lua_State *L) {
	lent_t *lent;
	qboolean r = qfalse;
	gentity_t *e = NULL;

	LUA_DEBUG("Entity_CallSpawn - start");

	lent = Lua_GetEntity(L, 1);

	if(lent)
		e = lent->e;

	if(e) {
		LUA_DEBUG("Entity_CallSpawn - G_CallSpawn");
		trap_UnlinkEntity(e);
		r = G_CallSpawn(e);
		lua_pushboolean(L, r);
		return 1;
	} else {
		LUA_DEBUG("Entity_CallSpawn - NULL entity");
	}
	
	lua_pushboolean(L, 0);
	return 1;
}

// entity.RemoveUnnamedSpawns()
// Removes all spawn points from the map, that don't have a targetname.
extern field_t fields[];
static int Entity_RemoveUnnamedSpawns(lua_State *L) {
	gentity_t *ent;
	int cnt = 0, i;
	
	for(i = 0; i < MAX_GENTITIES; i++) {
		if(!&g_entities[i]) continue;
		ent = &g_entities[i];
		if(!ent->classname) continue;
		if(!Q_stricmp(ent->classname, "info_player_deathmatch"))
			if(!ent->targetname) {
				G_FreeEntity(ent);
				cnt++;
			}
	}

	lua_pushnumber(L, cnt);
	return 1;
}

// entity.RemoveSpawns()
// Removes all spawn points from the map.
static int Entity_RemoveSpawns(lua_State *L) {
	gentity_t *ent;
	int cnt = 0, i;
	
	for(i = 0; i < MAX_GENTITIES; i++) {
		if(!&g_entities[i]) continue;
		ent = &g_entities[i];
		if(!ent->classname) continue;
		if(!Q_stricmp(ent->classname, "info_player_deathmatch")) {
			G_FreeEntity(ent);
			cnt++;
		}
	}

	lua_pushnumber(L, cnt);
	return 1;
}

// entity.RemoveType(string classname)
// Removes all entities of type classname from the map.
static int Entity_RemoveType(lua_State *L) {
	int i, cnt = 0;
	char *classname;

	classname = (char *)luaL_checkstring(L , 1);
	if(!classname) {
		lua_pushinteger(L, -1);
		return 1;
	}

	for(i = 0; i < MAX_GENTITIES; i++) {
		if(!strcmp(g_entities[i].classname, classname)) {
			G_FreeEntity(&g_entities[i]);
			cnt++;
		}
	}

	lua_pushinteger(L, cnt);
	return 1;
}

// entity.Remove(entity ent)
// Removes an entity if it is not protected
static int Entity_Remove(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);

	if(!lent || !lent->e) {
		lua_pushboolean(L, 0);
		return 1;
	}

	if(lent->e->neverFree || lent->e->client) {
		lua_pushboolean(L, 0);
		return 1;
	}

	G_FreeEntity(lent->e);
	lua_pushboolean(L, 1);

	return 1;
}

// entity.SetupTrigger(entity ent, float x, float y, float z) or 
// entity.SetupTrigger(entity ent, vector size)
// Does some setup for entities spawned by script that are to be used as trigger.
// * ent the entity
// * x length along the X-Axis
// * y length along the Y-Axis
// * z length along the Z-axis
// * Can also be stowed in a vector size
static int Entity_SetupTrigger(lua_State *L) {
	lent_t *lent;
	gentity_t *e;
	vec_t  *vptr;
	vec3_t size;

	lent = Lua_GetEntity(L, 1);

	if(!lent || !lent->e) {
		return 1;
	}

	e = lent->e;

	if(Lua_IsVector(L, 2)) {
		vptr = Lua_GetVector(L, 2); 
		VectorCopy(vptr, size);
	} else {
		size[0] = luaL_checkint(L, 2);
		size[1] = luaL_checkint(L, 3);
		size[2] = luaL_checkint(L, 4);
	}

	VectorCopy(size, e->r.mins);
	VectorCopy(size, e->r.maxs);
	VectorScale(e->r.mins, -.5, e->r.mins);
	VectorScale(e->r.maxs, .5, e->r.maxs);
	VectorCopy(e->r.mins, e->r.absmin);
	VectorCopy(e->r.maxs, e->r.absmax);

	e->tmpEntity = qtrue;

	return 1;
}

// entity.GetOrigin(entity ent)
// Returns the origin of an entity as vector
static int Entity_GetOrigin(lua_State *L) {
	lent_t *lent;
	vec3_t	origin;

	lent = Lua_GetEntity(L, 1);

	if(!lent || !lent->e) {
		lua_pushnil(L);
		return 1;
	}

	if(lent->e->r.svFlags & SVF_USE_CURRENT_ORIGIN)
		VectorCopy(lent->e->r.currentOrigin, origin);
	else
		VectorCopy(lent->e->s.origin, origin);

	Lua_PushVector(L, origin);

	return 1;
}

// ent.Lock(entity ent)
// Looks the entity ent. Works with anything that can be locked (doors, turbolifts, usables, ...).
static int Entity_Lock(lua_State *L) {
	lent_t *lent;
	gentity_t *ent;

	lent = Lua_GetEntity(L, 1);

	if(!lent || !lent->e) return 1;

	ent = lent->e;

	if(!strncmp(ent->classname, "func_door", 9) ||
		!strncmp(ent->classname, "func_door_rotating", 18) ||
		!strncmp(ent->classname, "target_teleporter", 17) ||
		!strncmp(ent->classname, "target_turbolift", 16) ||
		!strncmp(ent->classname, "func_usable", 11) ||
		!strncmp(ent->classname, "target_serverchange", 19) ||
		!strncmp(ent->classname, "trigger_teleport", 16) ||
		!strncmp(ent->classname, "ui_transporter", 14) ||
		!strncmp(ent->classname, "ui_holodeck", 11)
		) {
		if(ent->flags & FL_LOCKED) return 1;
		ent->flags ^= FL_LOCKED;
	}
	return 1;
}

// ent.Unlock(entity ent)
// Unlooks the entity ent. Works with anything that can be locked (doors, turbolifts, usables, ...).
static int Entity_Unlock(lua_State *L) {
	lent_t *lent;
	gentity_t *ent;

	lent = Lua_GetEntity(L, 1);

	if(!lent || !lent->e) return 1;

	ent = lent->e;

	if(!strncmp(ent->classname, "func_door", 9) ||
		!strncmp(ent->classname, "func_door_rotating", 18) ||
		!strncmp(ent->classname, "target_teleporter", 17) ||
		!strncmp(ent->classname, "target_turbolift", 16) ||
		!strncmp(ent->classname, "func_usable", 11) ||
		!strncmp(ent->classname, "target_serverchange", 19) ||
		!strncmp(ent->classname, "trigger_teleport", 16) ||
		!strncmp(ent->classname, "ui_transporter", 14) ||
		!strncmp(ent->classname, "ui_holodeck", 11)
		) {
		if(ent->flags & FL_LOCKED) 
			ent->flags ^= FL_LOCKED;
	}
	return 1;
}

static int Entity_IsLocked(lua_State *L) {
	lent_t *lent;
	gentity_t *ent;

	lent = Lua_GetEntity(L, 1);

	if(!lent || lent->e) return 1;

	ent = lent->e;

	lua_pushboolean(L, (int)(ent->flags & FL_LOCKED));
	return 1;
}

static int Entity_GetParm(lua_State *L) {
	lent_t *lent;
	gentity_t *ent;
	int parm;
	char *res = NULL;

	lent = Lua_GetEntity(L, 1);
	
	if(!lent || !lent->e) {
		lua_pushnil(L);
		return 1;
	}

	ent = lent->e;

	parm = luaL_checkint(L, 2);

	if(!parm || parm < 0 || parm > 4) {
		lua_pushnil(L);
		return 1;
	}

	switch(parm) {
		case 1:
			res = ent->luaParm1;
			break;
		case 2:
			res = ent->luaParm2;
			break;
		case 3:
			res = ent->luaParm3;
			break;
		case 4:
			res = ent->luaParm4;
			break;
	}

	if(!res) {
		lua_pushnil(L);
	} else {
		lua_pushstring(L, res);
	}
	return 1;
}

static int Entity_SetParm(lua_State *L) {
	lent_t *lent;
	gentity_t *ent;
	int parm;
	char *parms;
	char *s = NULL;

	lent = Lua_GetEntity(L, 1);

	if(!lent || !lent->e) return 1;

	ent = lent->e;

	parm = luaL_checkint(L, 2);

	if(!parm || parm < 0 || parm > 4) return 1;

	parms = (char *)luaL_checkstring(L, 3);

	if(!parms) return 1;

	switch(parm) {
		case 1:
			s = ent->luaParm1;
			break;
		case 2:
			s = ent->luaParm2;
			break;
		case 3:
			s = ent->luaParm3;
			break;
		case 4:
			s = ent->luaParm4;
			break;
	}

	if(s) {
		// check if the new string fits into the existing one
		if(strlen(s) > (strlen(parms) + 1)) {
			// it fits so copy it
			strncpy(s, parms, sizeof(s));
		} else {
			// it does not fit in so alloc a new string
			s = G_NewString(parms);
		}
	} else 
		s = G_NewString(parms);
	return 1;
}

static int Entity_GetActivator(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushnil(L);
		return 1;
	}
	Lua_PushEntity(L, lent->e->activator);

	return 1;
}

static int Entity_SetActivator(lua_State *L) {
	lent_t *lent;
	lent_t *activator;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	activator = Lua_GetEntity(L, 2);
	if(!activator || activator->e)
		lent->e->activator = NULL;
	else
		lent->e->activator = activator->e;

	return 1;
}

static int Entity_GetAngle(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushnumber(L, 0);
		return 1;
	}
	lua_pushnumber(L, lent->e->angle);

	return 1;
}

static int Entity_SetAngle(lua_State *L) {
	lent_t *lent;
	float angle;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	angle = (float)luaL_checknumber(L, 2);
	lent->e->angle = angle;

	return 1;
}

static int Entity_GetApos1(lua_State *L) {
	lent_t *lent;
	vec3_t null = { 0, 0, 0 };

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		Lua_PushVector(L, null);
		return 1;
	}
	Lua_PushVector(L, lent->e->apos1);

	return 1;
}

static int Entity_GetApos2(lua_State *L) {
	lent_t *lent;
	vec3_t null = { 0, 0, 0 };

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		Lua_PushVector(L, null);
		return 1;
	}
	Lua_PushVector(L, lent->e->apos2);

	return 1;
}

static int Entity_SetApos1(lua_State *L) {
	lent_t *lent;
	vec_t *vec;
	
	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	vec = Lua_GetVector(L, 2);

	VectorCopy(vec, lent->e->apos1);

	return 1;
}

static int Entity_SetApos2(lua_State *L) {
	lent_t *lent;
	vec_t *vec;
	
	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	vec = Lua_GetVector(L, 2);

	VectorCopy(vec, lent->e->apos2);

	return 1;
}

static int Entity_GetBluename(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->bluename);

	return 1;
}

static int Entity_SetBluename(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->bluename = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetBluesound(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->bluesound);

	return 1;
}

static int Entity_SetBluesound(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->bluesound = (char *)luaL_checkstring(L, 2);;

	return 1;
}

static int Entity_GetBooleanstate(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushboolean(L, 0);
		return 1;
	}
	lua_pushboolean(L, (int)lent->e->booleanstate);

	return 1;
}

static int Entity_SetBooleanstate(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->booleanstate = (qboolean)lua_toboolean(L, 2);

	return 1;
}

static int Entity_GetClipmask(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->clipmask);

	return 1;
}

static int Entity_SetClipmask(lua_State *L) {
	lent_t *lent;
	int mask;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	mask = (int)luaL_checknumber(L, 2);

	lent->e->clipmask = mask;

	return 1;
}

static int Entity_GetCount(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->count);

	return 1;
}

static int Entity_SetCount(lua_State *L) {
	lent_t *lent;
	int		count;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	count = (int)luaL_checknumber(L, 2);

	lent->e->count = count;

	return 1;
}

static int Entity_GetDamage(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->damage);

	return 1;
}

static int Entity_SetDamage(lua_State *L) {
	lent_t *lent;
	int		damage;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	damage = (int)luaL_checknumber(L, 2);

	lent->e->damage = damage;

	return 1;
}

static int Entity_GetDistance(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushnumber(L, 0);
		return 1;
	}
	lua_pushnumber(L, lent->e->distance);

	return 1;
}

static int Entity_SetDistance(lua_State *L) {
	lent_t *lent;
	float	distance;

	lent = Lua_GetEntity(L, 1);
	if(!lent || ! lent->e)
		return 1;
	distance = (float)luaL_checknumber(L, 2);

	lent->e->distance = distance;

	return 1;
}

static int Entity_GetEnemy(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushnil(L);
		return 1;
	}
	Lua_PushEntity(L, lent->e->enemy);

	return 1;
}

static int Entity_SetEnemy(lua_State *L) {
	lent_t *lent;
	lent_t *enemy;

	lent = Lua_GetEntity(L, 1);
	if(!lent || lent->e)
		return 1;
	if(lua_isnil(L, 2)) {
		lent->e->enemy = NULL;
	} else {
		enemy = Lua_GetEntity(L, 2);
		if(!enemy || !enemy->e) {
			lent->e->enemy = NULL;
		} else {
			lent->e->enemy = enemy->e;
		}
	}

	return 1;
}

static int Entity_GetEventTime(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->eventTime);

	return 1;
}

static int Entity_SetEventTime(lua_State *L) {
	lent_t *lent;
	int		eTime;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		return 1;
	eTime = (int)luaL_checknumber(L, 2);

	lent->e->eventTime = eTime;

	return 1;
}

static int Entity_GetFalsename(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->falsename);

	return 1;
}

static int Entity_SetFalsename(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->falsename = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_SetTargetName(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		return 1;
	lent->e->targetname = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetFalsetarget(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->falsetarget);

	return 1;
}

static int Entity_SetFalsetarget(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->falsetarget = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetFlags(lua_State *L) {
	lent_t *lent;
	
	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->flags);

	return 1;
}

static int Entity_SetFlags(lua_State *L) {
	lent_t *lent;
	int		flags;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		return 1;
	flags = (int)luaL_checknumber(L, 2);
	
	lent->e->flags = flags;

	return 1;
}

static int Entity_GetFreeAfterEvent(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushboolean(L, 0);
		return 1;
	}
	lua_pushboolean(L, (int)lent->e->freeAfterEvent);

	return 1;
}

static int Entity_SetFreeAfterEvent(lua_State *L) {
	lent_t *lent;
	qboolean b;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	b = (qboolean)lua_toboolean(L, 2);
	
	lent->e->freeAfterEvent = b;

	return 1;
}

static int Entity_GetFreetime(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->freetime);

	return 1;
}

static int Entity_GetGreensound(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->greensound);

	return 1;
}

static int Entity_SetGreensound(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		return 1;
	lent->e->greensound = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetHealth(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->health);

	return 1;
}

static int Entity_SetHealth(lua_State *L) {
	lent_t *lent;
	int		health;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	health = (int)luaL_checknumber(L, 2);

	lent->e->health = health;

	return 1;
}

static int Entity_GetInUse(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushboolean(L, 0);
		return 1;
	}
	lua_pushboolean(L, (int)lent->e->inuse);

	return 1;
}

static int Entity_GetLastEnemy(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushnil(L);
		return 1;
	}
	if(!lent->e->lastEnemy)
		lua_pushnil(L);
	else
		Lua_PushEntity(L, lent->e->lastEnemy);

	return 1;
}

static int Entity_SetLastEnemy(lua_State *L) {
	lent_t *lent;
	lent_t *lastEnemy;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lastEnemy = Lua_GetEntity(L, 2);
	if(!lastEnemy || !lastEnemy->e) 
		return 1;

	lent->e->lastEnemy = lastEnemy->e;

	return 1;
}

static int Entity_GetLuaDie(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->luaDie);

	return 1;
}

static int Entity_SetLuaDie(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->luaDie = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetLuaEntity(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushboolean(L, 0);
		return 1;
	}
	lua_pushboolean(L, (int)lent->e->luaEntity);

	return 1;
}

static int Entity_GetLuaFree(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->luaFree);

	return 1;
}

static int Entity_SetLuaFree(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->luaFree = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetLuaHurt(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->luaHurt);

	return 1;
}

static int Entity_SetLuaHurt(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->luaHurt = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetLuaReached(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->luaReached);

	return 1;
}

static int Entity_SetLuaReached(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->luaReached = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetLuaReachedAngular(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->luaReachedAngular);

	return 1;
}

static int Entity_SetLuaReachedAngular(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->luaReachedAngular = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetLuaSpawn(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->luaSpawn);

	return 1;
}

static int Entity_SetLuaSpawn(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->luaSpawn = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetLuaThink(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->luaThink);

	return 1;
}

static int Entity_SetLuaThink(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->luaThink = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetLuaTouch(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->luaTouch);

	return 1;
}

static int Entity_SetLuaTouch(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->luaTouch = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetLuaTrigger(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->luaTrigger);

	return 1;
}

static int Entity_SetLuaTrigger(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->luaTrigger = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetLuaUse(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->luaUse);

	return 1;
}

static int Entity_SetLuaUse(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->luaUse = (char *)luaL_checkstring(L, 2);

	return 1;
}


static int Entity_GetMessage(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->message);

	return 1;
}

static int Entity_SetMessage(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->message = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetMethodOfDeath(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->methodOfDeath);

	return 1;
}

static int Entity_SetMethodOfDeath(lua_State *L) {
	lent_t *lent;
	int		mod;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	mod = (int)luaL_checknumber(L, 2);

	lent->e->methodOfDeath = mod;

	return 1;
}

static int Entity_GetModel(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || lent->e) 
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->model);

	return 1;
}

static int Entity_SetModel(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->model = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetModel2(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->model2);

	return 1;
}

static int Entity_SetModel2(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->model2 = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetMovedir(lua_State *L) {
	lent_t *lent;
	vec3_t null = { 0, 0, 0 };

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		Lua_PushVector(L, null);
		return 1;
	}
	Lua_PushVector(L, lent->e->movedir);

	return 1;
}

static int Entity_SetMovedir(lua_State *L) {
	lent_t	*lent;
	vec_t	*dir;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	dir = Lua_GetVector(L, 2);

	VectorCopy(dir, lent->e->movedir);

	return 1;
}

static int Entity_GetMoverstate(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, -1);
		return 1;
	}
	lua_pushinteger(L, (int)lent->e->moverState);

	return 1;
}

static int Entity_SetMoverstate(lua_State *L) {
	lent_t	*lent;
	moverState_t m;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	m = (moverState_t)luaL_checknumber(L, 2);

	lent->e->moverState = m;

	return 1;
}

static int Entity_GetN00bCount(lua_State *L) {
	lent_t *lent;
	
	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->n00bCount);

	return 1;
}

static int Entity_SetN00bCount(lua_State *L) {
	lent_t *lent;
	int		cnt;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	cnt = (int)luaL_checknumber(L, 2);

	lent->e->n00bCount = cnt;

	return 1;
}

static int Entity_GetWait(lua_State *L) {
	lent_t *lent;
	
	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->wait);

	return 1;
}

static int Entity_SetWait(lua_State *L) {
	lent_t *lent;
	int		wait;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	wait = (int)luaL_checknumber(L, 2);

	lent->e->wait = wait;

	return 1;
}

static int Entity_GetNeverFree(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushboolean(L, 0);
		return 1;
	}
	lua_pushboolean(L, (int)lent->e->neverFree);

	return 1;
}

static int Entity_SetNeverFree(lua_State *L) {
	lent_t *lent;
	qboolean b;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	b = (qboolean)lua_toboolean(L, 2);

	lent->e->neverFree = b;

	return 1;
}

static int Entity_GetNexthink(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, -1);
		return 1;
	}
	lua_pushinteger(L, lent->e->nextthink);

	return 1;
}

static int Entity_SetNexthink(lua_State *L) {
	lent_t *lent;
	int		next;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		return 1;
	next = (int)luaL_checknumber(L, 2);

	lent->e->nextthink = next;

	return 1;
}

static int  Entity_GetNextTrain(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushnil(L);
		return 1;
	}
	if(!lent->e->nextTrain) {
		lua_pushnil(L);
	} else {
		Lua_PushEntity(L, lent->e->nextTrain);
	}

	return 1;
}

static int Entity_SetNextTrain(lua_State *L) {
	lent_t *lent;
	lent_t *next;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	next = Lua_GetEntity(L, 2);
	if(!next || !next->e)
		lent->e->nextTrain = NULL;
	else
		lent->e->nextTrain = next->e;

	return 1;
}

static int Entity_GetNoiseIndex(lua_State *L) {
	lent_t *lent;
	
	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, -1);
		return 1;
	}
	lua_pushinteger(L, lent->e->noise_index);

	return 1;
}

static int Entity_SetNoiseIndex(lua_State *L) {
	lent_t *lent;
	int		idx;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	idx = (int)luaL_checknumber(L, 2);

	lent->e->noise_index = idx;

	return 1;
}

static int Entity_GetOldHealth(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->old_health);

	return 1;
}

static int Entity_SetOldHealth(lua_State *L) {
	lent_t *lent;
	int		old_health;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	old_health = (int)luaL_checknumber(L, 2);

	lent->e->old_health = old_health;

	return 1;
}

static int Entity_GetPaintarget(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->paintarget);

	return 1;
}

static int Entity_SetPaintarget(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->paintarget = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetPainDebounceTime(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->pain_debounce_time);

	return 1;
}

static int Entity_SetPainDebounceTime(lua_State *L) {
	lent_t *lent;
	int		pdb;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	pdb = (int)luaL_checknumber(L, 2);

	lent->e->pain_debounce_time = pdb;

	return 1;
}

static int Entity_GetParent(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushnil(L);
		return 1;
	}
	if(!lent->e->parent)
		lua_pushnil(L);
	else
		Lua_PushEntity(L, lent->e->parent);

	return 1;
}

static int Entity_SetParent(lua_State *L) {
	lent_t *lent;
	lent_t *parent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	parent = Lua_GetEntity(L, 2);
	if(!parent || !parent->e)
		lent->e->parent = NULL;
	else
		lent->e->parent = parent->e;

	return 1;
}

static int Entity_GetPhysicsBounce(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushnumber(L, 0);
		return 1;
	}
	lua_pushnumber(L, lent->e->physicsBounce);

	return 1;
}

static int Entity_SetPhysicsBounce(lua_State *L) {
	lent_t *lent;
	float	pb;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	pb = (float)luaL_checknumber(L, 2);

	lent->e->physicsBounce = pb;

	return 1;
}

static int Entity_GetPhysicsObject(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushboolean(L, 0);
		return 1;
	}
	lua_pushboolean(L, (int)lent->e->physicsObject);

	return 1;
}

static int Entity_SetPhysicsObject(lua_State *L) {
	lent_t *lent;
	qboolean b;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	b = (qboolean)lua_toboolean(L, 2);

	lent->e->physicsObject = b;

	return 1;
}

static int Entity_GetPos1(lua_State *L) {
	lent_t *lent;
	vec3_t	null = { 0, 0, 0 };

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		Lua_PushVector(L, null);
		return 1;
	}
	Lua_PushVector(L, lent->e->pos1);

	return 1;
}

static int Entity_SetPos1(lua_State *L) {
	lent_t	*lent;
	vec_t	*vec;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	vec = Lua_GetVector(L, 2);

	VectorCopy(vec, lent->e->pos1);

	return 1;
}

static int Entity_GetPos2(lua_State *L) {
	lent_t *lent;
	vec3_t	null = { 0, 0, 0 };

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		Lua_PushVector(L, null);
		return 1;
	}
	Lua_PushVector(L, lent->e->pos2);

	return 1;
}

static int Entity_SetPos2(lua_State *L) {
	lent_t	*lent;
	vec_t	*vec;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	vec = Lua_GetVector(L, 2);

	VectorCopy(vec, lent->e->pos2);

	return 1;
}

static int Entity_GetPrevTrain(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushnil(L);
		return 1;
	}
	if(!lent->e->prevTrain)
		lua_pushnil(L);
	else
		Lua_PushEntity(L, lent->e->prevTrain);

	return 1;
}

static int Entity_SetPrevTrain(lua_State *L) {
	lent_t *lent;
	lent_t *prev;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	prev = Lua_GetEntity(L, 2);
	if(!prev || !prev->e)
		lent->e->prevTrain = NULL;
	else
		lent->e->prevTrain = prev->e;

	return 1;
}

static int Entity_GetRandom(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L,1);
	if(!lent || !lent->e) {
		lua_pushnumber(L, 0);
		return 1;
	}
	lua_pushnumber(L, lent->e->random);

	return 1;
}

static int Entity_SetRandom(lua_State *L) {
	lent_t *lent;
	float	rand;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	rand = (float)luaL_checknumber(L, 2);

	lent->e->random = rand;

	return 1;
}

static int Entity_GetRedsound(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->redsound);

	return 1;
}

static int Entity_SetRedsound(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->redsound = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetSound1To2(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, -1);
		return 1;
	}
	lua_pushinteger(L, lent->e->sound1to2);

	return 1;
}

static int Entity_SetSound1To2(lua_State *L) {
	lent_t *lent;
	int		s1to2;

	lent  = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	s1to2 = (int)luaL_checknumber(L, 2);

	lent->e->sound1to2 = s1to2;

	return 1;
}

static int Entity_GetSound2To1(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, -1);
		return 1;
	}
	lua_pushinteger(L, lent->e->sound2to1);

	return 1;
}

static int Entity_SetSound2To1(lua_State *L) {
	lent_t *lent;
	int		s2to1;

	lent  = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	s2to1 = (int)luaL_checknumber(L, 2);

	lent->e->sound2to1 = s2to1;

	return 1;
}

static int Entity_GetSoundLoop(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, -1);
		return 1;
	}
	lua_pushinteger(L, lent->e->soundLoop);

	return 1;
}

static int Entity_SetSoundLoop(lua_State *L) {
	lent_t *lent;
	int		sl;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	sl = (int)luaL_checknumber(L, 2);

	lent->e->soundLoop = sl;

	return 1;
}

static int Entity_GetSoundPos1(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, -1);
		return 1;
	}
	lua_pushinteger(L, lent->e->soundPos1);

	return 1;
}

static int Entity_SetSoundPos1(lua_State *L) {
	lent_t *lent;
	int		sp1;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	sp1 = (int)luaL_checknumber(L, 2);

	lent->e->soundPos1 = sp1;

	return 1;
}

static int Entity_GetSoundPos2(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, -1);
		return 1;
	}
	lua_pushinteger(L, lent->e->soundPos2);

	return 1;
}

static int Entity_SetSoundPos2(lua_State *L) {
	lent_t *lent;
	int		sp2;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	sp2 = (int)luaL_checknumber(L, 2);

	lent->e->soundPos2 = sp2;

	return 1;
}

static int Entity_GetSpawnflags(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->spawnflags);

	return 1;
}

static int Entity_SetSpawnflags(lua_State *L) {
	lent_t *lent;
	int		sp;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	sp = (int)luaL_checknumber(L, 2);

	lent->e->spawnflags = sp;

	return 1;
}

static int Entity_GetSpeed(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushnumber(L, 0);
		return 1;
	}
	lua_pushnumber(L, lent->e->speed);

	return 1;
}

static int Entity_SetSpeed(lua_State *L) {
	lent_t *lent;
	float	speed;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	speed = (float)luaL_checknumber(L, 2);

	lent->e->speed = speed;

	return 1;
}

static int Entity_GetSplashDamage(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->splashDamage);

	return 1;
}

static int Entity_SetSplashDamage(lua_State *L) {
	lent_t *lent;
	int		dmg;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	dmg = (int)luaL_checknumber(L, 2);

	lent->e->splashDamage = dmg;

	return 1;
}

static int Entity_GetSplashMethodOfDeath(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->splashMethodOfDeath);

	return 1;
}

static int Entity_SetSplashMethodOfDeath(lua_State *L) {
	lent_t *lent;
	int		mod;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	mod = (int)luaL_checknumber(L, 2);

	lent->e->splashMethodOfDeath = mod;

	return 1;
}

static int Entity_GetSplashRadius(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->splashRadius);

	return 1;
}

static int Entity_SetSplashRadius(lua_State *L) {
	lent_t *lent;
	int		radius;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	radius = (int)luaL_checknumber(L, 2);

	lent->e->splashRadius = radius;

	return 1;
}

static int Entity_GetSwapname(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->swapname);

	return 1;
}

static int Entity_SetSwapname(lua_State *L) {
	lent_t	*lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->swapname = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetTakedamage(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		lua_pushboolean(L, 0);
		return 1;
	}
	lua_pushboolean(L, (int)lent->e->takedamage);

	return 1;
}

static int Entity_SetTakedamage(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->takedamage = (qboolean)lua_toboolean(L, 2);

	return 1;
}

static int Entity_SetTarget(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->target = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetTargetname2(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->targetname2);

	return 1;
}

static int Entity_SetTargetname2(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->targetname2 = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetTargetShaderName(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->targetShaderName);

	return 1;
}

static int Entity_SetTargetShaderName(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->targetShaderName = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetTargetShaderNewName(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->targetShaderNewName);

	return 1;
}

static int Entity_SetTargetShaderNewName(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->targetShaderNewName = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetTargetEnt(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) 
		lua_pushnil(L);
	else
		Lua_PushEntity(L, lent->e->target_ent);

	return 1;
}

static int Entity_SetTargetEnt(lua_State *L) {
	lent_t *lent;
	lent_t *targ;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	targ = Lua_GetEntity(L, 2);
	if(!targ || !targ->e)
		return 1;

	lent->e->target_ent = targ->e;

	return 1;
}

static int Entity_GetTeam(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		lua_pushnil(L);
	else
		lua_pushstring(L, lent->e->team);

	return 1;
}

static int Entity_SetTeam(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	lent->e->team = (char *)luaL_checkstring(L, 2);

	return 1;
}

static int Entity_GetTeamchain(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		lua_pushnil(L);
	else
		Lua_PushEntity(L, lent->e->teamchain);

	return 1;
}

static int Entity_SetTeamchain(lua_State *L) {
	lent_t *lent;
	lent_t *team;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	team = Lua_GetEntity(L, 2);
	if(!team || !team->e)
		return 1;

	lent->e->teamchain = team->e;

	return 1;
}

static int Entity_GetTeammaster(lua_State *L) {
	lent_t *lent;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		lua_pushnil(L);
	else
		Lua_PushEntity(L, lent->e->teammaster);

	return 1;
}

static int Entity_SetTeammaster(lua_State *L) {
	lent_t *lent;
	lent_t *team;

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e)
		return 1;
	team = Lua_GetEntity(L, 2);
	if(!team || !team->e)
		return 1;

	lent->e->teammaster = team->e;

	return 1;
}

static const luaL_Reg Entity_ctor[] = {
	{"Spawn",					Entity_Spawn},
	{"Find",					Entity_Find},
	{"FindNumber",				Entity_FindNumber},
	{"FindBModel",				Entity_FindBModel},
	{"GetTarget",				Entity_GetTarget},
	{"CallSpawn",				Entity_CallSpawn},
	{"DelayedCallSpawn",		Entity_DelayedCallSpawn },
	{"Remove",					Entity_Remove},
	{"RemoveUnnamedSpawns",		Entity_RemoveUnnamedSpawns},
	{"RemoveSpawns",			Entity_RemoveSpawns},
	{"RemoveType",				Entity_RemoveType},
	{"Use",						Entity_Use},
	{NULL,						NULL}
};

static const luaL_Reg Entity_meta[] = {
	{"__gc",						Entity_GC},
	{"__tostring",					Entity_ToString},
	{"GetNumber",					Entity_GetNumber},
	{"IsClient",					Entity_IsClient},
	{"GetClientname",				Entity_GetClientName},
	{"Print",						Entity_Print},
	{"CenterPrint",				Entity_CenterPrint},

	{"GetClassname",				Entity_GetClassName}, // args: none; return: string
	{"SetClassname",				Entity_SetClassName}, // args: string; return: nothing

	{"GetTargetname",				Entity_GetTargetName}, // args: none; return: string
	{"SetTargetname",				Entity_SetTargetName}, // args: string; return: nothing

	{"Rotate",						Entity_Rotate},

	{"IsRocket",					Entity_IsRocket},
	{"IsGrenade",					Entity_IsGrenade},
	{"Teleport",					Entity_Teleport},
	{"SetKeyValue",					Entity_SetKeyValue},
	{"GetOrigin",					Entity_GetOrigin},
	{"SetupTrigger",				Entity_SetupTrigger},

	{"Lock",						Entity_Lock},
	{"Unlock",						Entity_Unlock},
	{"IsLocked",					Entity_IsLocked},

	{"GetParm",						Entity_GetParm},
	{"SetParm",						Entity_SetParm},

	{"GetActivator",				Entity_GetActivator}, // args: none; return: ent
	{"SetActivator",				Entity_SetActivator}, // args: ent; return: nothing

	{"GetAngle",					Entity_GetAngle}, // args: none; return: float
	{"SetAngle",					Entity_SetAngle}, // args: float; return: nothing

	{"GetApos1",					Entity_GetApos1}, // args: none; return: vec
	{"GetApos2",					Entity_GetApos2}, // args: none; return: vec

	{"SetApos1",					Entity_SetApos1}, // args: vec; return: nothing
	{"SetApos2",					Entity_SetApos2}, // args: vec; return: nothing

	{"GetBluename",					Entity_GetBluename}, // args: none; return: string
	{"SetBluename",					Entity_SetBluename}, // args: string; return: nothing

	{"GetBluesound",				Entity_GetBluesound}, // args: none; return: string
	{"SetBluesound",				Entity_SetBluesound}, // args: string; return: nothing

	{"GetBooleanstate",				Entity_GetBooleanstate}, // args: none; return: bool
	{"SetBooleanstate",				Entity_SetBooleanstate}, // args: bool; return: nothing

	{"GetClipmask",					Entity_GetClipmask}, // args: none; return: int
	{"SetClipmask",					Entity_SetClipmask}, // args: int; return: nothing

	{"GetCount",					Entity_GetCount}, // args: none; return: int
	{"SetCount",					Entity_SetCount}, // args: int; return: nothing

	{"GetDamage",					Entity_GetDamage}, // args: none; return: int
	{"SetDamage",					Entity_SetDamage}, // args: int; return: nothing

	{"GetDistance",					Entity_GetDistance}, // args: none; return: float
	{"SetDistance",					Entity_SetDistance}, // args: float; return: nothing

	{"GetEnemy",					Entity_GetEnemy}, // args: none; return: ent
	{"SetEnemy",					Entity_SetEnemy}, // args: ent; return: nothing

	{"GetEventTime",				Entity_GetEventTime}, // args: none; return: int
	{"SetEventTime",				Entity_SetEventTime}, // args: int; return: nothing

	{"GetFalsename",				Entity_GetFalsename}, // args: none; return: string
	{"SetFalsename",				Entity_SetFalsename}, // args: string; return: nothing

	{"GetFalsetarget",				Entity_GetFalsetarget}, // args: none; return: string
	{"SetFalsetarget",				Entity_SetFalsetarget}, // args: string; return: nothing

	{"GetFlags",					Entity_GetFlags}, // args: none; return: int
	{"SetFlags",					Entity_SetFlags}, // args: int; return: nothing

	{"GetFreeAfterEvent",			Entity_GetFreeAfterEvent}, // args: none; return: bool
	{"SetFreeAfterEvent",			Entity_SetFreeAfterEvent}, // args: bool; return: nothing

	{"GetFreetime",					Entity_GetFreetime}, // args: none; return: int

	{"GetGreensound",				Entity_GetGreensound}, // args: none; return: string
	{"SetGreensound",				Entity_SetGreensound}, // args: string; return: nothing

	{"GetHealth",					Entity_GetHealth}, // args: none; return: int
	{"SetHealth",					Entity_SetHealth}, // args: int; return: nothing

	{"GetInUse",					Entity_GetInUse}, // args: none; return: bool

	{"GetLastEnemy",				Entity_GetLastEnemy}, // args: none; return: ent
	{"SetLastEnemy",				Entity_SetLastEnemy}, // args: ent; return: nothing

	{"GetLuaDie",					Entity_GetLuaDie}, // args: none; return: string
	{"SetLuaDie",					Entity_SetLuaDie}, // args: string; return: nothing

	{"GetLuaEntity",				Entity_GetLuaEntity}, // args: none; return: bool

	{"GetLuaFree",					Entity_GetLuaFree}, // args: none; return: string
	{"SetLuaFree",					Entity_SetLuaFree},  // args: string; return: nothing

	{"GetLuaHurt",					Entity_GetLuaHurt}, // args: none; return: string
	{"SetLuaHurt",					Entity_SetLuaHurt}, // args: string; return: nothing

	{"GetLuaReached",				Entity_GetLuaReached}, // args: none; return: string
	{"SetLuaReached",				Entity_SetLuaReached}, // args: string; return: nothing

	{"GetLuaReachedAngular",		Entity_GetLuaReachedAngular}, // args: none; return: string
	{"SetLuaReachedAngular",		Entity_SetLuaReachedAngular}, // args: string; return: nothing

	{"GetLuaSpawn",					Entity_GetLuaSpawn}, // args: none; return: string
	{"SetLuaSpawn",					Entity_SetLuaSpawn}, // args: string; return: nothing

	{"GetLuaThink",					Entity_GetLuaThink}, // args: none; return: string
	{"SetLuaThink",					Entity_SetLuaThink}, // args: string; return: nothing

	{"GetLuaTouch",					Entity_GetLuaTouch}, // args: none; return: string
	{"SetLuaTouch",					Entity_SetLuaTouch}, // args: string; return: nothing

	{"GetLuaTrigger",				Entity_GetLuaTrigger}, // args: none; return: string
	{"SetLuaTrigger",				Entity_SetLuaTrigger}, // args: string; return: nothing

	{"GetLuaUse",					Entity_GetLuaUse}, // args: none; return: string
	{"SetLuaUse",					Entity_SetLuaUse}, // args: string; return: nothing

	{"GetMessage",					Entity_GetMessage}, // args: none; return: string
	{"SetMessage",					Entity_SetMessage}, // args: string; return: nothing

	{"GetMethodOfDeath",			Entity_GetMethodOfDeath}, // args: none; return: int
	{"SetMethodOfDeath",			Entity_SetMethodOfDeath}, // args: int; return: nothing

	{"GetModel",					Entity_GetModel}, // args: none; return: string
	{"SetModel",					Entity_SetModel}, // args: string; return: nothing

	{"GetModel2",					Entity_GetModel2}, // args: none; return: string
	{"SetModel2",					Entity_SetModel2}, // args: string; return: nothing

	{"GetMovedir",					Entity_GetMovedir}, // args: none; return: vec
	{"SetMovedir",					Entity_SetMovedir}, // args: vec; return: nothing

	{"GetMoverstate",				Entity_GetMoverstate}, // args: none; return: int
	{"SetMoverstate",				Entity_SetMoverstate}, // args: int; return: nothing

	{"GetN00bCount",				Entity_GetN00bCount}, // args: none; return: int
	{"SetN00bCount",				Entity_SetN00bCount}, // args: int; return: nothing

	{"GetWait",						Entity_GetWait}, // args: none; return: int
	{"SetWait",						Entity_SetWait}, // args: int; return: nothing

	{"GetNeverFree",				Entity_GetNeverFree}, // args: none; return: bool
	{"SetNeverFree",				Entity_SetNeverFree}, // args: bool; return: nothing

	{"GetNextthink",				Entity_GetNexthink}, // args: none; return: int
	{"SetNextthink",				Entity_SetNexthink}, // args: int; return: nothing

	{"GetNextTrain",				Entity_GetNextTrain}, // args: none; return: ent
	{"SetNextTrain",				Entity_SetNextTrain}, // args: ent; return: nothing

	{"GetNoiseIndex",				Entity_GetNoiseIndex}, // args: none; return: int
	{"SetNoiseIndex",				Entity_SetNoiseIndex}, // args: int; return: nothing

	{"GetOldHealth",				Entity_GetOldHealth}, // args: none; return: int
	{"SetOldHealth",				Entity_SetOldHealth}, // args: int; return: nothing

	{"GetPaintarget",				Entity_GetPaintarget}, // args: none; return: string
	{"SetPaintarget",				Entity_SetPaintarget}, // args: string; return: nothing

	{"GetPainDebounceTime",			Entity_GetPainDebounceTime}, // args: none; return: int
	{"SetPainDebounceTime",			Entity_SetPainDebounceTime}, // args: int; return: nothing

	{"GetParent",					Entity_GetParent}, // args: none; return: ent
	{"SetParent",					Entity_SetParent}, // args: ent; return: nothing

	{"GetPhysicsBounce",			Entity_GetPhysicsBounce}, // args: none; return: float
	{"SetPhysicsBounce",			Entity_SetPhysicsBounce}, // args: float; return: nothing

	{"GetPhysicsObject",			Entity_GetPhysicsObject}, // args: none; return: bool
	{"SetPhysicsObject",			Entity_SetPhysicsObject}, // args: bool; return: nothing

	{"GetPos1",						Entity_GetPos1}, // args: none; return: vec
	{"SetPos1",						Entity_SetPos1}, // args: vec; return: nothing

	{"GetPos2",						Entity_GetPos2}, // args: none; return: vec
	{"SetPos2",						Entity_SetPos2}, // args: vec; return: nothing

	{"GetPrevTrain",				Entity_GetPrevTrain}, // args: none; return: ent
	{"SetPrevTrain",				Entity_SetPrevTrain}, // args: ent; return: nothing

	{"GetRandom",					Entity_GetRandom}, // args: none; return: float
	{"SetRandom",					Entity_SetRandom}, // args: float; return: nothing

	{"GetRedsound",					Entity_GetRedsound}, // args: none; return: string
	{"SetRedsound",					Entity_SetRedsound}, // args: string; return: nothing

	{"GetSound1To2",				Entity_GetSound1To2}, // args: none; return: int
	{"SetSound1To2",				Entity_SetSound1To2}, // args: int; return: nothing

	{"GetSound2To1",				Entity_GetSound2To1}, // args: none; return: int
	{"SetSound2To1",				Entity_SetSound2To1}, // args: int return: nothing

	{"GetSoundLoop",				Entity_GetSoundLoop}, // args: none; return: int
	{"SetSoundLoop",				Entity_SetSoundLoop}, // args: int; return: nothing

	{"GetSoundPos1",				Entity_GetSoundPos1}, // args: none; return: int
	{"SetSoundPos1",				Entity_SetSoundPos1}, // args: int; return: nothing

	{"GetSoundPos2",				Entity_GetSoundPos2}, // args: none; return: int
	{"SetSoundPos2",				Entity_SetSoundPos2}, // args: int; return: nothing

	{"GetSpawnflags",				Entity_GetSpawnflags}, // args: none; return: int
	{"SetSpawnflags",				Entity_SetSpawnflags}, // args: int; return: nothing

	{"GetSpeed",					Entity_GetSpeed}, // args: none; return: float
	{"SetSpeed",					Entity_SetSpeed}, // args: float; return: nothing

	{"GetSplashDamage",				Entity_GetSplashDamage}, // args: none; return: int
	{"SetSplashDamage",				Entity_SetSplashDamage}, // args: int; return: nothing

	{"GetSplashMethodOfDeath",		Entity_GetSplashMethodOfDeath}, // args: none; return: int
	{"SetSplashMethodOfDeath",		Entity_SetSplashMethodOfDeath}, // args: int; return: nothing

	{"GetSplashRadius",				Entity_GetSplashRadius}, // args: none; return: int
	{"SetSplashRadius",				Entity_SetSplashRadius}, // args: int; return: nothing

	{"GetSwapname",					Entity_GetSwapname}, // args: none; return: string
	{"SetSwapname",					Entity_SetSwapname}, // args: string; return: nothing

	{"GetTakedamage",				Entity_GetTakedamage}, // args: none; return: bool
	{"SetTakedamage",				Entity_SetTakedamage}, // args: bool; return: nothing

	{"SetTarget",					Entity_SetTarget}, // args: string; return: nothing

	{"GetTargetname2",				Entity_GetTargetname2}, // args: none; return: string
	{"SetTargetname2",				Entity_SetTargetname2}, // args: string; return: nothing

	{"GetTargetShaderName",			Entity_GetTargetShaderName}, // args: none; return: string
	{"SetTargetShaderName",			Entity_SetTargetShaderName}, // args: string; return: nothing

	{"GetTargetShaderNewName",		Entity_GetTargetShaderNewName}, // args: none; return: string
	{"SetTargetShaderNewName",		Entity_SetTargetShaderNewName}, // args: string; return: nothing

	{"GetTargetEnt",				Entity_GetTargetEnt}, // args: none; return: ent
	{"SetTargetEnt",				Entity_SetTargetEnt}, // args: ent; return: nothing

	{"GetTeam",						Entity_GetTeam}, // args: none; return: string
	{"SetTeam",						Entity_SetTeam}, // args: string; return nothing

	{"GetTeamchain",				Entity_GetTeamchain}, // args: none; return: ent
	{"SetTeamchain",				Entity_SetTeamchain}, // args: ent; return: nothing

	{"GetTeammaster",				Entity_GetTeammaster},
	{"SetTeammaster",				Entity_SetTeammaster},

	{NULL, NULL}
};

/*void dummy(gentity_t *ent) {
	ent->timestamp;
}*/

int Luaopen_Entity(lua_State * L)
{
	luaL_newmetatable(L, "game.entity");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	luaL_register(L, NULL, Entity_meta);
	luaL_register(L, "entity", Entity_ctor);

	return 1;
}

void Lua_PushEntity(lua_State * L, gentity_t * ent)
{
	lent_t     *lent;

	if(!ent || !ent->inuse)
		lua_pushnil(L);
	else {
		lent = (lent_t *)lua_newuserdata(L, sizeof(lent_t));
		luaL_getmetatable(L, "game.entity");
		lua_setmetatable(L, -2);
		lent->e = ent;
	}
}

lent_t *Lua_GetEntity(lua_State * L, int argNum)
{
	void           *ud;

	ud = luaL_checkudata(L, argNum, "game.entity");
	luaL_argcheck(L, ud != NULL, argNum, "\'entity\' expected");
	return (lent_t *) ud;
}
#endif
