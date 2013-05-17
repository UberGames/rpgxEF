// entity lib for lua

#include "g_lua.h"
#include "g_spawn.h"

extern qboolean G_CallSpawn(gentity_t *ent);

#ifdef G_LUA

/***
Module to access entity functions and manage enities.
@module entiy
*/

/***
This is just a function called from lua it should be called before any other model work
this will loop trough all misc_model_breakables and checks their model-string against those listed here
if it finds a match it will apply the associated splashDamage, splashRadius and s.powerups (material of chunks) to the entity
this is the only failsafe way i can think of to do these kind of refit
@function MMBRefit
*/
static int Entity_MMBRefit(lua_State * L)
{
	gentity_t		*MMB;
	char			serverInfo[MAX_TOKEN_CHARS];
	char			*arg2;
	trap_GetServerinfo( serverInfo, sizeof( serverInfo ) );
	arg2 = Info_ValueForKey( serverInfo, "mapname" );

	if(	!Q_stricmp( arg2, "_brig" ) // I stole this directly from g_cmds.c, Cmd_CallVote_f
		|| !Q_stricmp( arg2, "_holodeck_camelot" ) || !Q_stricmp( arg2, "_holodeck_firingrange" ) || !Q_stricmp( arg2, "_holodeck_garden" ) || !Q_stricmp( arg2, "_holodeck_highnoon" ) || !Q_stricmp( arg2, "_holodeck_minigame" ) || !Q_stricmp( arg2, "_holodeck_proton" ) || !Q_stricmp( arg2, "_holodeck_proton2" ) || !Q_stricmp( arg2, "_holodeck_temple" ) || !Q_stricmp( arg2, "_holodeck_warlord" )
		|| !Q_stricmp( arg2, "borg1" ) || !Q_stricmp( arg2, "borg2" ) || !Q_stricmp( arg2, "borg3" ) || !Q_stricmp( arg2, "borg4" ) || !Q_stricmp( arg2, "borg5" ) || !Q_stricmp( arg2, "borg6" )
		|| !Q_stricmp( arg2, "dn1" ) || !Q_stricmp( arg2, "dn2" ) || !Q_stricmp( arg2, "dn3" ) || !Q_stricmp( arg2, "dn4" ) || !Q_stricmp( arg2, "dn5" ) || !Q_stricmp( arg2, "dn6" ) || !Q_stricmp( arg2, "dn8" )
		|| !Q_stricmp( arg2, "forge1" ) || !Q_stricmp( arg2, "forge2" ) || !Q_stricmp( arg2, "forge3" ) || !Q_stricmp( arg2, "forge4" ) || !Q_stricmp( arg2, "forge5" ) || !Q_stricmp( arg2, "forgeboss" )
		|| !Q_stricmp( arg2, "holodeck" )
		|| !Q_stricmp( arg2, "scav1" ) || !Q_stricmp( arg2, "scav2" ) || !Q_stricmp( arg2, "scav3" ) || !Q_stricmp( arg2, "scav3b" ) || !Q_stricmp( arg2, "scav4" ) || !Q_stricmp( arg2, "scav5" ) || !Q_stricmp( arg2, "scavboss" )
		|| !Q_stricmp( arg2, "stasis1" ) || !Q_stricmp( arg2, "stasis2" ) || !Q_stricmp( arg2, "stasis3" )
		|| !Q_stricmp( arg2, "tour/deck01" ) || !Q_stricmp( arg2, "tour/deck02" ) || !Q_stricmp( arg2, "tour/deck03" ) || !Q_stricmp( arg2, "tour/deck04" ) || !Q_stricmp( arg2, "tour/deck05" ) || !Q_stricmp( arg2, "tour/deck08" ) || !Q_stricmp( arg2, "tour/deck09" ) || !Q_stricmp( arg2, "tour/deck10" ) || !Q_stricmp( arg2, "tour/deck11" ) || !Q_stricmp( arg2, "tour/deck15" )
		|| !Q_stricmp( arg2, "tutorial" )
		|| !Q_stricmp( arg2, "voy1" ) || !Q_stricmp( arg2, "voy13" ) || !Q_stricmp( arg2, "voy14" ) || !Q_stricmp( arg2, "voy15" ) || !Q_stricmp( arg2, "voy16" ) || !Q_stricmp( arg2, "voy17" ) || !Q_stricmp( arg2, "voy2" ) || !Q_stricmp( arg2, "voy20" ) || !Q_stricmp( arg2, "voy3" ) || !Q_stricmp( arg2, "voy4" ) || !Q_stricmp( arg2, "voy5" ) || !Q_stricmp( arg2, "voy6" ) || !Q_stricmp( arg2, "voy7" ) || !Q_stricmp( arg2, "voy8" ) || !Q_stricmp( arg2, "voy9" ) ) {
			MMB = NULL; //init MMB here to do sth pointless
	} else {
		return 0; //we are not on one of the supported maps
	}

	while((MMB = G_Find(MMB, FOFS(classname), "misc_model_breakable" )) != NULL  ){//loop while you find these
		//borg maps
		if( !Q_stricmp( MMB->model, "models/mapobjects/borg/blite.md3" )){ //alcove light
			MMB->splashDamage = 75;
			MMB->splashRadius = 75;
			MMB->s.powerups = 3;//glass and metal, may reduce this to glass only 
			MMB->spawnflags = 258;
			G_CallSpawn(MMB);
		}else if( !Q_stricmp( MMB->model, "models/mapobjects/borg/circuit_1.md3" )){ //those things that look like a handle
			MMB->splashDamage = 75;
			MMB->splashRadius = 75;
			MMB->s.powerups = 1;//
			MMB->spawnflags = 263;
			G_CallSpawn(MMB);
		}else if( !Q_stricmp( MMB->model, "models/mapobjects/borg/circuit_2.md3" )){ //the isosceles triangle looking box
			MMB->splashDamage = 75;
			MMB->splashRadius = 75;
			MMB->s.powerups = 1;//
			MMB->spawnflags = 263;
			G_CallSpawn(MMB);
		}else if( !Q_stricmp( MMB->model, "models/mapobjects/borg/circuit_3.md3" )){ //the other triangle looking box
			MMB->splashDamage = 75;
			MMB->splashRadius = 75;
			MMB->s.powerups = 1;//metal
			MMB->spawnflags = 263;
			G_CallSpawn(MMB);
		}else if( !Q_stricmp( MMB->model, "models/mapobjects/borg/vynclumn.md3" )){ //no description needed ^^
			MMB->splashDamage = 9999;
			MMB->splashRadius = 9999;
			MMB->s.powerups = 1;//metal
			MMB->spawnflags = 263;
			G_CallSpawn(MMB);
		}else continue;//we are not looking for this kind of MMB
	}

	return 0;
}


/***
Returns a target entity of ent.
@function GetTarget
@return Target of the entity.
*/
static int Entity_GetTarget(lua_State * L)
{
	lent_t*		lent = NULL;
	gentity_t*	t = NULL;

	LUA_DEBUG("BEGIN - entity.GetTarget");

	lent = Lua_GetEntity(L, 1);
	
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetTarget - entity NULL");

		lua_pushnil(L);
		return 1;
	}

	t = G_PickTarget(lent->e->target);
	if(t == NULL) {
		LUA_DEBUG("ERROR - entity.GetTarget - target NULL");

		lua_pushnil(L);
		return 1;
	}

	Lua_PushEntity(L, t);
	LUA_DEBUG("END - entity.GetTarget");
	return 1;
}

/***
Returns the entity with the brush model bmodelnumber. 
This is the only failsafe way to find brush entities as the 
entity number is different when you load a map local or join a server.
@function FindBModel
@param number Number of the brush model.
*/
static int Entity_FindBModel(lua_State *L) {
	gentity_t*	ent = NULL;
	int			bmodel;
	char		tmp[MAX_QPATH];

	LUA_DEBUG("BEGIN - entity.FindBModel");

	bmodel = luaL_checkint(L, 1);
	sprintf(tmp, "*%d", bmodel);
	ent = G_Find(NULL, FOFS(model), tmp);

	if(ent == NULL) {
		LUA_DEBUG("ERROR - entity.FindBModel - entity NULL");
		lua_pushnil(L);
	} else {
		Lua_PushEntity(L, ent);
	}

	LUA_DEBUG("END - entity.FindBModel");
	return 1;
}

/***
Returns the entity with the entity number entnum.
@function FindNumber
@param number Entity number.
@return Entity for the given entity number.
*/
static int Entity_FindNumber(lua_State * L)
{
	int			entnum;
	gentity_t*	ent = NULL;

	LUA_DEBUG("BEGIN - entity.FindNumber");

	entnum = luaL_checknumber(L, 1);
	ent = &g_entities[entnum];
	
	if(ent == NULL || ent->inuse == qfalse) {
		LUA_DEBUG("ERROR - entity.FindNumber - entity NULL");
		lua_pushnil(L);
	} else {
		Lua_PushEntity(L, ent);
	}

	LUA_DEBUG("END - entity.FindNumber");
	return 1;
}

/***
Returns the first entity found that has a targetname of targetname.
@function Find
@param targetname
@return First entity found that has the given targetname.
*/
static int Entity_Find(lua_State * L)
{
	gentity_t* t = NULL;

	LUA_DEBUG("BEGIN - entity.Find");

	t = G_Find(t, FOFS(targetname), (char *)luaL_checkstring(L, 1));

	if(t == NULL) {
		LUA_DEBUG("ERROR - entity.Find - target NULL");
		lua_pushnil(L);
	} else {
		Lua_PushEntity(L, t);
	}

	LUA_DEBUG("END - entity.Find");
	return 1;
}

/***
Returns the misc_model_breakable entity that has a matching MMB->s.origin.
Requires vector as input.
You can get the s.origin ingame as an admin/developer by pointing at the MMB ingame and using the /getorigin-command.
@function FindMMB
@param vector Origin of misc_model_breakable.
@return Matching entity.
*/
static int Entity_FindMMB(lua_State * L)
{
	gentity_t		*t = NULL, *MMB = NULL;
	vec_t			*vec = NULL, *origin = NULL;
	
	vec = Lua_GetVector(L, 2);

	if(vec == NULL) {
		lua_pushnil(L);
		return 1;
	}

	while((MMB = G_Find(MMB, FOFS(classname), "misc_model_breakable")) != NULL){
		origin = MMB->s.origin;
		if(vec[0] == origin[0] && vec[1] == origin[1] && vec[2] == origin[2]){
			t = MMB;
			break;
		} else {
			continue;
		}
	}
	if(t == NULL) {
		lua_pushnil(L);
	} else {
		Lua_PushEntity(L, t);
	}

	return 1;
}

/***
Uses the given entity.
@function Use
@param ent Entity to use.
@return Success or failure.
*/
static int Entity_Use(lua_State * L)
{
	lent_t* lent = NULL;

	LUA_DEBUG("BEGIN - entity.Use");

	lent = Lua_GetEntity(L, 1);

	if(lent == NULL || lent->e == NULL || lent->e->use == NULL) {
		LUA_DEBUG("ERROR - entity.Use - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	if(lent->e->luaUse != NULL) {
		LUA_DEBUG("INFO - entity.Use - calling entity->luaUse");
		LuaHook_G_EntityUse(lent->e->luaUse, lent->e-g_entities, lent->e-g_entities, lent->e-g_entities);
	}
	lent->e->use(lent->e, NULL, lent->e);

	lua_pushboolean(L, qtrue);
	LUA_DEBUG("END - entity.Use");
	return 1;
}

/***
Teleports client to target's position
@function Teleport
@param target Target entity to teleport to.
@return Success or failure.
*/
static int Entity_Teleport(lua_State * L)
{
	lent_t* lent = NULL;
	lent_t* target = NULL;

	LUA_DEBUG("BEGIN - entity.Teleport");

	lent = Lua_GetEntity(L, 1);
	target = Lua_GetEntity(L, 2);

	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.Teleport - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	if(target  == NULL || target->e == NULL) {
		LUA_DEBUG("ERROR - entity.Teleport - target NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	if(lent->e->client != NULL) {
		LUA_DEBUG("INFO - entity.Teleport - calling TeleportPlayer");
		TeleportPlayer(lent->e, target->e->s.origin, target->e->s.angles, TP_NORMAL);
		lua_pushboolean(L, qtrue);
	} else {
		lua_pushboolean(L, qfalse);
	}

	LUA_DEBUG("END - entity.Teleport");
	return 1;
}


/***
Checks if an entity is a rocket.
@function IsRocket
@return True if entity is a rocket else false.
*/
static int Entity_IsRocket(lua_State * L)
{
	lent_t*		lent = NULL;
	qboolean	rocket = qfalse;

	lent = Lua_GetEntity(L, 1);

	if(lent == NULL || lent->e == NULL) {
		lua_pushboolean(L, qfalse);
		return 1;
	}

	if(!Q_stricmp(lent->e->classname, "rocket")) {
		rocket = qtrue;
	}

	lua_pushboolean(L, rocket);
	return 1;
}

/***
Checks if an entity is a grenade.
@function IsGrenade
@return true if entity is a grenade else false.
*/
static int Entity_IsGrenade(lua_State * L)
{
	lent_t*		lent = NULL;
	qboolean	grenade = qfalse;

	lent = Lua_GetEntity(L, 1);

	if(lent == NULL || lent->e == NULL) {
		lua_pushboolean(L, qfalse);
		return 1;
	}

	if(!Q_stricmp(lent->e->classname, "grenade")) {
		grenade = qtrue;
	}

	lua_pushboolean(L, grenade);
	return 1;
}

/***
Tries to spawn a new entity and returns it. If no new entity can be spawned nil is returned.
@function Spawn
@return New entity or nil if failure.
*/
static int Entity_Spawn(lua_State * L)
{
	gentity_t* ent = NULL;

	ent = G_Spawn();

	if(ent == NULL) {
		lua_pushnil(L);
		return 1;
	}

	Lua_PushEntity(L, ent);
	return 1;
}

/***
Returns the entities index number.
@function GetNumber
@return Index number for entity.
*/
static int Entity_GetNumber(lua_State * L)
{
	lent_t* lent = NULL;

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		lua_pushinteger(L, -1);
	} else {
		lua_pushinteger(L, lent->e - g_entities);
	}

	return 1;
}

/***
Checks if an entity is a client
@function IsClient
@return true if entity is a client else return false.
*/
static int Entity_IsClient(lua_State * L)
{
	lent_t* lent = NULL;

	lent = Lua_GetEntity(L, 1);
	
	if(lent  == NULL || lent->e == NULL) {
		lua_pushboolean(L, 0);
		return 1;
	}

	lua_pushboolean(L, lent->e->client != NULL);
	return 1;
}

/***
Returns the display name of a client.
@function GetClientName
@return Display name of a client or nil if entity is not a client.
*/
static int Entity_GetClientName(lua_State * L)
{
	lent_t* lent = NULL;

	lent = Lua_GetEntity(L, 1);

	if(lent == NULL || lent->e == NULL || lent->e->classname == NULL || lent->e->client == NULL) {
		lua_pushnil(L);
		return 1;
	}

	lua_pushstring(L, lent->e->client->pers.netname);
	return 1;
}

static int Entity_Print(lua_State * L)
{
	lent_t*	lent = NULL;
	int		i;
	char	buf[MAX_STRING_CHARS];
	int		n = lua_gettop(L);

	LUA_DEBUG("BEGIN - entity.Print");

	lent = Lua_GetEntity(L, 1);

	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.Print - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	if(lent->e->client == NULL) {
		LUA_DEBUG("ERROR - entity.Print - entity is not a client");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	memset(buf, 0, sizeof(buf));

	lua_getglobal(L, "tostring");
	for(i = 2; i <= n; i++)
	{
		const char     *s;

		lua_pushvalue(L, -1);
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);
		s = lua_tostring(L, -1);

		if(s == NULL) {
			LUA_DEBUG("ERROR - entity.Print - string NULL");
			lua_pushboolean(L, qfalse);
			return 1;
		}

		Q_strcat(buf, sizeof(buf), s);

		lua_pop(L, 1);
	}

	trap_SendServerCommand(lent->e - g_entities, va("print \"%s\n\"", buf));
	LUA_DEBUG("END - entity.Print");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_CenterPrint(lua_State * L)
{
	lent_t	*lent = NULL;
	int		i;
	char	buf[MAX_STRING_CHARS];
	int		n = lua_gettop(L);

	LUA_DEBUG("BEGIN - entity.CenterPrint");

	lent = Lua_GetEntity(L, 1);

	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.CenterPrint - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	if(lent->e->client == NULL) {
		LUA_DEBUG("ERROR - entity.CenterPrint - entity is not a client");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	memset(buf, 0, sizeof(buf));

	lua_getglobal(L, "tostring");
	for(i = 2; i <= n; i++)
	{
		const char     *s;

		lua_pushvalue(L, -1);
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);
		s = lua_tostring(L, -1);

		if(s == NULL) {
			LUA_DEBUG("ERROR - entity.CenterPrint - string NULL");
			lua_pushboolean(L, qfalse);
			return 1;
		}

		Q_strcat(buf, sizeof(buf), s);

		lua_pop(L, 1);
	}

	trap_SendServerCommand(lent->e - g_entities, va("cp \"" S_COLOR_WHITE "%s\n\"", buf));
	LUA_DEBUG("END - entity.CenterPrint");
	lua_pushboolean(L, qtrue);
	return 1;
}

extern qboolean G_ParseField(const char *key, const char *value, gentity_t *ent);
/***
Sets a key of an entity to a value.
@function SetKeyValue
@param key Key to set.
@param value Value to set.
@return Success or failure.
*/
static int Entity_SetKeyValue(lua_State * L) {
	lent_t* lent = NULL;
	char*	key = NULL;
	char*	value = NULL;
	
	LUA_DEBUG("BEGIN - entity.SetKeyValue");

	lent = Lua_GetEntity(L, 1);
	key = (char *)luaL_checkstring(L, 2);
	value = (char *)luaL_checkstring(L, 3);

	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetKeyValue - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	if(key == NULL) {
		LUA_DEBUG("ERROR - entity.SetKeyValue - key NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	if(value == NULL) {
		LUA_DEBUG("ERROR - entity.SetKeyValue - key NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	lua_pushboolean(L, G_ParseField(key, value, lent->e));
	LUA_DEBUG("END - entity.SetKeyValue");
	lua_pushboolean(L, qtrue);
	return 1;
}

/***
Returns the classname of an entity.
@function GetClassName
@return Classname of the entity (or nil).
*/
static int Entity_GetClassName(lua_State * L)
{
	lent_t* lent = NULL;

	LUA_DEBUG("BEGIN - entity.GetClassname");

	lent = Lua_GetEntity(L, 1);

	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetKeyValue - entity NULL");
		lua_pushnil(L);
	} else {
		lua_pushstring(L, lent->e->classname);
	}

	LUA_DEBUG("END - entity.SetKeyValue");
	return 1;
}

/***
Sets the Classname of an entity to name
@param name New classname.
@return Success or failure.
*/
static int Entity_SetClassName(lua_State * L)
{
	lent_t* lent = NULL;

	LUA_DEBUG("BEGIN - entity.SetClassname");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetClassname - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	
	lent->e->classname = (char *)luaL_checkstring(L, 2);
	lua_pushboolean(L, qtrue);
	LUA_DEBUG("END - entity.SetClassname");
	return 1;
}

/***
Returns the targetname of an entity.
@function GetTargetName
@return The targetname of an entity.
*/
static int Entity_GetTargetName(lua_State * L)
{
	lent_t* lent = NULL;

	LUA_DEBUG("BEGIN - entity.GetTargetname");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetTargetname - entity NULL");
		lua_pushnil(L);
	} else {
		lua_pushstring(L, lent->e->targetname);
	}

	LUA_DEBUG("END - entity.GetTargetname");
	return 1;
}

/***
Rotates an entity in the specified directions.
@function Rotate
@param dir Directions.
@return Success or failure.
*/
static int Entity_Rotate(lua_State * L)
{
	lent_t*	lent = NULL;
	vec_t*	vec = NULL;

	LUA_DEBUG("BEGIN - entity.Rotate");

	lent = Lua_GetEntity(L, 1);
	vec = Lua_GetVector(L, 2);

	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.Rotate - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	if(vec == NULL) {
		LUA_DEBUG("ERROR - entity.Rotate - vector NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	lent->e->s.apos.trType = TR_LINEAR;
	lent->e->s.apos.trDelta[0] = vec[0];
	lent->e->s.apos.trDelta[1] = vec[1];
	lent->e->s.apos.trDelta[2] = vec[2];

	lua_pushboolean(L, qtrue);
	LUA_DEBUG("END - entity.Rotate");
	return 1;
}

static int Entity_GC(lua_State * L)
{

	return 0;
}

/***
Prints an entity as string also return said string.
@function ToString
@return Entity string.
*/
static int Entity_ToString(lua_State * L)
{
	lent_t*		lent = NULL;
	gentity_t*	gent = NULL;
	char		buf[MAX_STRING_CHARS];

	LUA_DEBUG("BEGIN - entity.ToString");

	lent = Lua_GetEntity(L, 1);

	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.ToString - entity NULL");
		lua_pushnil(L);
		return 1;
	}

	gent = lent->e;
	Com_sprintf(buf, sizeof(buf), "entity: class=%s name=%s id=%i pointer=%p\n", gent->classname, gent->targetname, gent-g_entities, gent);
	lua_pushstring(L, buf);

	LUA_DEBUG("END - entity.ToString");
	return 1;
}

static void ent_delay(gentity_t *ent) {
	ent->think = 0;
	ent->nextthink = -1;
	G_CallSpawn(ent);
}

/***
Calls the game logic spawn function for the class of ent after a given delay in milliseconds.
@function DelayedCallSpawn
@param ent Entity.
@param delay Delay in milliseconds.
@return Success or failure.
*/
static int Entity_DelayedCallSpawn(lua_State *L) {
	lent_t*	lent = NULL;
	int		delay;

	LUA_DEBUG("BEGIN - entity.DelayedCallSpawn");

	lent = Lua_GetEntity(L, 1);

	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.DelayedCallSpawn - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	if(!Q_stricmp(lent->e->classname, "target_selfdestruct")) {
		LUA_DEBUG("ERROR - entity.DelayedCallSpawn - entity is target_selfdestruct");
		lua_pushboolean(L, qfalse);
		return 1; //we will not selfdestruct this way
	}

	delay = luaL_checkint(L, 2);

	if(!delay) {
		delay = FRAMETIME;
	}

	lent->e->nextthink = delay;
	lent->e->think = ent_delay;
	lua_pushboolean(L, qtrue);
	LUA_DEBUG("END - entity.DelayedCallSpawn");
	return 1;
}

/***
Calls the game logic spawn function for the class of ent.
@function CallSpawn
@param ent Entity.
@return Success or failure.
*/
static int Entity_CallSpawn(lua_State *L) {
	lent_t* lent = NULL;
	qboolean r = qfalse;
	gentity_t* e = NULL;

	LUA_DEBUG("BEGIN - entity.CallSpawn");

	lent = Lua_GetEntity(L, 1);

	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.CallSpawn - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	e = lent->e;

	if(!Q_stricmp(lent->e->classname, "target_selfdestruct")) {
		LUA_DEBUG("ERROR - entity.CallSpawn - entity is target_selfdestruct");
		lua_pushboolean(L, qfalse);
		return 1; //we will not selfdestruct this way
	}

	LUA_DEBUG("INFO - entity.CallSpawn - G_CallSpawn");
	trap_UnlinkEntity(e);
	r = G_CallSpawn(e);
	lua_pushboolean(L, r);
	LUA_DEBUG("END - entity.CallSpawn");
	return 1;
}

/***
Removes all spawn points from the map, that don't have a targetname. 
Note that every map has to have at least one spawnpoint.
If this command removes all spawn points you'll have to spawn at least one new spawn point.
@function RemoveUnnamedSpawns
@return Count of removed spawn points.
*/
extern field_t fields[];
static int Entity_RemoveUnnamedSpawns(lua_State *L) {
	gentity_t *ent;
	int cnt = 0, i;
	
	LUA_DEBUG("BEGIN - entity.RemoveUnnamedSpawns");

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
	LUA_DEBUG("END - entity.RemoveUnnamedSpawns");
	return 1;
}

/***
Removes all spawn points from the map.
Note that every map has to have at least one spawnpoint.
If this command removes all spawn points you'll have to spawn at least one new spawn point.
@function RemoveSpawns
@return Count of removed spawn points.
*/
static int Entity_RemoveSpawns(lua_State *L) {
	gentity_t *ent;
	int cnt = 0, i;
	
	LUA_DEBUG("BEGIN - entity.RemoveSpawns");

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
	LUA_DEBUG("END - entity.RemoveSpawns");
	return 1;
}

/***
Removes all entities with the given classname from the map.
@function RemoveType
@return Count of removed entities (-1 indicates an error).
*/
static int Entity_RemoveType(lua_State *L) {
	int i, cnt = 0;
	char *classname;

	LUA_DEBUG("BEGIN - entity.RemoveType");

	classname = (char *)luaL_checkstring(L , 1);
	if(classname == NULL) {
		LUA_DEBUG("ERROR - entity.RemoveType - classname NULL");
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
	LUA_DEBUG("END - entity.RemoveType");
	return 1;
}

/***
Removes an entity if it is not protected.
@function Remove
@param ent Entity.
@return Success or failure.
*/
static int Entity_Remove(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.Remove");

	lent = Lua_GetEntity(L, 1);

	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.Remove - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	if(lent->e->neverFree) {
		LUA_DEBUG("INFO - entity.Remove - entity->neverFree");
		LUA_DEBUG("END - entity.Remove");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	if(lent->e->client != NULL) {
		LUA_DEBUG("INFO - entity.Remove - entity is client");
		LUA_DEBUG("END - entity.Remove");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	G_FreeEntity(lent->e);
	lua_pushboolean(L, qtrue);
	LUA_DEBUG("END - entity.Remove");
	return 1;
}

/***
Does some setup for entities spawned by script that are to be used as trigger.
@function SetupTrigger
@param ent Entity.
@param x Length along the X-Axis.
@param y Length along the Y-Axis.
@param z Length along the Z-axis.
@return Succcess or failure.
*/
/***
Does some setup for entities spawned by script that are to be used as trigger.
@function SetupTrigger
@param ent Entity.
@param vec Vector containing sizing information.
@return Succcess or failure.
*/
static int Entity_SetupTrigger(lua_State *L) {
	lent_t *lent;
	gentity_t *e;
	vec_t  *vptr;
	vec3_t size;

	LUA_DEBUG("BEGIN - entity.SetupTrigger");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetupTrigger - entity NULL");
		lua_pushboolean(L, qfalse);
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

	LUA_DEBUG("END - entity.SetupTrigger");
	lua_pushboolean(L, qtrue);
	return 1;
}

/***
Returns the origin of an entity as vector.
@function GetOrigin
@return Origin (or nil on failure).
*/
static int Entity_GetOrigin(lua_State *L) {
	lent_t *lent;
	vec3_t	origin;

	LUA_DEBUG("BEGIN - entity.GetOrigin");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetOrigin - entity NULL");
		lua_pushnil(L);
		return 1;
	}

	if(lent->e->r.svFlags & SVF_USE_CURRENT_ORIGIN) {
		VectorCopy(lent->e->r.currentOrigin, origin);
	} else {
		VectorCopy(lent->e->s.origin, origin);
	}

	Lua_PushVector(L, origin);
	LUA_DEBUG("END - entity.GetOrigin");
	return 1;
}

/***
Looks the entity ent. Works with anything that can be locked (doors, turbolifts, usables, ...).
@function Lock
@return Success or failure.
*/
static int Entity_Lock(lua_State *L) {
	lent_t *lent;
	gentity_t *ent;

	LUA_DEBUG("BEGIN - entity.Lock");

	lent = Lua_GetEntity(L, 1);

	if(!lent || !lent->e) {
		LUA_DEBUG("ERROR - entity.Lock - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}

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
		if(ent->flags & FL_LOCKED) {
			LUA_DEBUG("INFO - entity.Lock - already locked");
			LUA_DEBUG("END - entity.Lock");
			lua_pushboolean(L, qtrue);
			return 1;
		}
		ent->flags ^= FL_LOCKED;
	} else {
		LUA_DEBUG("INFO - entity.Lock - entity not valid");
		LUA_DEBUG("END - entity.Lock");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	LUA_DEBUG("END - entity.Lock");
	lua_pushboolean(L, qtrue);
	return 1;
}

/***
Unlooks the entity ent. Works with anything that can be locked (doors, turbolifts, usables, ...).
@function Unlock
@return Success or failure.
*/
static int Entity_Unlock(lua_State *L) {
	lent_t *lent;
	gentity_t *ent;

	LUA_DEBUG("BEGIN - entity.Unlock");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.Unlock - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}

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
		if(ent->flags & FL_LOCKED)  {
			ent->flags ^= FL_LOCKED;
		}
	} else {
		LUA_DEBUG("INFO - entity.Unlock - entity is not valid");
		LUA_DEBUG("END - entity.Unlock");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	LUA_DEBUG("END - entity.Unlock");
	lua_pushboolean(L, qtrue);
	return 1;
}

/***
Check if the entity is locked.
@function IsLocked
@return Whether the entity is locked or not.
*/
static int Entity_IsLocked(lua_State *L) {
	lent_t *lent;
	gentity_t *ent;

	LUA_DEBUG("BEGIN - entity.IsLocked");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.IsLocked - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	ent = lent->e;
	lua_pushboolean(L, (int)(ent->flags & FL_LOCKED));
	LUA_DEBUG("END - entity.IsLocked");
	return 1;
}

/***
Get a luaParm from the entity.
@function GetParm
@param index Parameter index.
@return Value of luaParm or nil.
*/
static int Entity_GetParm(lua_State *L) {
	lent_t *lent;
	gentity_t *ent;
	int parm;
	char *res = NULL;

	LUA_DEBUG("BEGIN - entity.GetParm");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL|| lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetParm");
		lua_pushnil(L);
		return 1;
	}

	ent = lent->e;
	parm = luaL_checkint(L, 2);

	if(parm < 1 || parm > 4) {
		LUA_DEBUG("ERROR - entity.GetParm - parm out of range");
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
		default:
			res = NULL;
			break;
	}

	if(res == NULL) {
		LUA_DEBUG("ERROR - entity.GetParm - res NULL");
		lua_pushnil(L);
	} else {
		lua_pushstring(L, res);
	}
	LUA_DEBUG("END - entity.GetParm");
	return 1;
}

static int Entity_SetParm(lua_State *L) {
	lent_t *lent;
	gentity_t *ent;
	int parm;
	char *parms;
	char *s = NULL;

	LUA_DEBUG("BEGIN - entity.SetParm");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL|| lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetParm - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	ent = lent->e;
	parm = luaL_checkint(L, 2);
	if(parm < 1 || parm > 4) {
		LUA_DEBUG("ERROR - entity.SetParm - parm out of range");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	parms = (char *)luaL_checkstring(L, 3);
	if(!parms) {
		LUA_DEBUG("ERROR - entity.SetParm - parms NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}

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
	} else {
		s = G_NewString(parms);
	}

	LUA_DEBUG("END - entity.SetParm");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetActivator(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetActivator");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL|| lent->e == NULL) {
		lua_pushnil(L);
		LUA_DEBUG("ERROR - entity.GetActivator - entity NULL");
		return 1;
	}

	Lua_PushEntity(L, lent->e->activator);
	LUA_DEBUG("END - entity.GetActivator");
	return 1;
}

static int Entity_SetActivator(lua_State *L) {
	lent_t *lent;
	lent_t *activator;

	LUA_DEBUG("BEGIN - entity.SetActivator");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetActivator - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}

	activator = Lua_GetEntity(L, 2);
	if(!activator || activator->e) {
		lent->e->activator = NULL;
	} else {
		lent->e->activator = activator->e;
	}

	LUA_DEBUG("END - entity.SetActivator");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetAngle(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetAngle");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetAngle - entity NUL");
		lua_pushnumber(L, 0);
		return 1;
	}

	lua_pushnumber(L, lent->e->angle);
	LUA_DEBUG("END - entity.GetAngle");
	return 1;
}

static int Entity_SetAngle(lua_State *L) {
	lent_t *lent;
	float angle;

	LUA_DEBUG("BEGIN - entity.SetAngle");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetAngle - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	angle = (float)luaL_checknumber(L, 2);
	lent->e->angle = angle;

	LUA_DEBUG("END - entity.SetAngle");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetApos1(lua_State *L) {
	lent_t *lent;
	vec3_t null = { 0, 0, 0 };

	LUA_DEBUG("BEGIN - entity.GetApos1");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetApos1 - entity NULL");
		Lua_PushVector(L, null);
		return 1;
	}
	Lua_PushVector(L, lent->e->apos1);

	LUA_DEBUG("END - entity.GetApos1");
	return 1;
}

static int Entity_GetApos2(lua_State *L) {
	lent_t *lent;
	vec3_t null = { 0, 0, 0 };

	LUA_DEBUG("BEGIN - entity.GetApos2");

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		LUA_DEBUG("ERROR - entity.GetApos2 - entity NULL");
		Lua_PushVector(L, null);
		return 1;
	}
	Lua_PushVector(L, lent->e->apos2);

	LUA_DEBUG("END - entity.GetApos2");
	return 1;
}

static int Entity_SetApos1(lua_State *L) {
	lent_t *lent;
	vec_t *vec;

	LUA_DEBUG("BEGIN - entity.SetApos1");
	
	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetApos1 - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	vec = Lua_GetVector(L, 2);
	VectorCopy(vec, lent->e->apos1);

	LUA_DEBUG("END - entity.SetApos1");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_SetApos2(lua_State *L) {
	lent_t *lent;
	vec_t *vec;
	
	LUA_DEBUG("BEGIN - entity.SetApos2");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetApos2 - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	vec = Lua_GetVector(L, 2);
	VectorCopy(vec, lent->e->apos2);

	LUA_DEBUG("END - entity.SetApos2");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetBluename(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetBluename");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL|| lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetBluename - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->bluename);
	}

	LUA_DEBUG("END - entity.GetBluename");
	return 1;
}

static int Entity_SetBluename(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetBluename");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetBluename - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->bluename = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetBluename");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetBluesound(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetBluesound");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetBluesound - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->bluesound);
	}

	LUA_DEBUG("END - entity.GetBluesound");
	return 1;
}

static int Entity_SetBluesound(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetBluesound");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetBluesound - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->bluesound = (char *)luaL_checkstring(L, 2);;

	LUA_DEBUG("END - entity.SetBluesound");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetBooleanstate(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetBooleanstate");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetBooleanstate - entity NULL");
		lua_pushboolean(L, 0);
		return 1;
	}
	lua_pushboolean(L, (int)lent->e->booleanstate);

	LUA_DEBUG("END - entity.GetBooleanstate");
	return 1;
}

static int Entity_SetBooleanstate(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.SetBooleanstate");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetBooleanstate - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->booleanstate = (qboolean)lua_toboolean(L, 2);

	lua_pushboolean(L, qtrue);
	LUA_DEBUG("END - entity.SetBooleanstate");
	return 1;
}

static int Entity_GetClipmask(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetClipmask");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetClipmask - entity NULL");
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->clipmask);

	LUA_DEBUG("END - entity.GetClipmask");
	return 1;
}

static int Entity_SetClipmask(lua_State *L) {
	lent_t *lent;
	int mask;

	LUA_DEBUG("BEGIN - entity.SetClipmask");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetClipmask - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	mask = (int)luaL_checknumber(L, 2);
	lent->e->clipmask = mask;

	LUA_DEBUG("END - entity.SetClipmask");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetCount(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.SetClipmask");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetClipmask - entity NULL");
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->count);

	LUA_DEBUG("END - entity.SetClipmask");
	return 1;
}

static int Entity_SetCount(lua_State *L) {
	lent_t *lent;
	int		count;

	LUA_DEBUG("BEGIN - entity.SetCount");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetCount - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	count = (int)luaL_checknumber(L, 2);
	lent->e->count = count;

	LUA_DEBUG("END - entity.SetCount");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetDamage(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetDamage");

	lent = Lua_GetEntity(L, 1);
	if(!lent || !lent->e) {
		LUA_DEBUG("ERROR - entity.GetDamage - entity NULL");
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->damage);

	LUA_DEBUG("END - entity.GetDamage");
	return 1;
}

static int Entity_SetDamage(lua_State *L) {
	lent_t *lent;
	int		damage;

	LUA_DEBUG("BEGIN - entity.SetDamage");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetDamage - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	damage = (int)luaL_checknumber(L, 2);

	lent->e->damage = damage;

	LUA_DEBUG("END - entity.SetDamage");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetDistance(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetDistance");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetDistance - entity NULL");
		lua_pushnumber(L, 0);
		return 1;
	}
	lua_pushnumber(L, lent->e->distance);

	LUA_DEBUG("END - entity.GetDistance");
	return 1;
}

static int Entity_SetDistance(lua_State *L) {
	lent_t *lent;
	float	distance;

	LUA_DEBUG("BEGIN - entity.SetDistance");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetDistance - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	distance = (float)luaL_checknumber(L, 2);
	lent->e->distance = distance;

	LUA_DEBUG("END - entity.SetDistance");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetEnemy(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetEnemy");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetEnemy - entity NULL");
		lua_pushnil(L);
		return 1;
	}
	Lua_PushEntity(L, lent->e->enemy);

	LUA_DEBUG("END - entity.GetEnemy");
	return 1;
}

static int Entity_SetEnemy(lua_State *L) {
	lent_t *lent;
	lent_t *enemy;

	LUA_DEBUG("BEGIN - entity.SetEnemy");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetEnemy - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	
	if(lua_isnil(L, 2)) {
		lent->e->enemy = NULL;
	} else {
		enemy = Lua_GetEntity(L, 2);
		if(enemy == NULL || enemy->e == NULL) {
			lent->e->enemy = NULL;
		} else {
			lent->e->enemy = enemy->e;
		}
	}

	LUA_DEBUG("END - entity.SetEnemy");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetEventTime(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetEventTime");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetEventTime - entity NULL");
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->eventTime);

	LUA_DEBUG("END - entity.GetEventTime");
	return 1;
}

static int Entity_SetEventTime(lua_State *L) {
	lent_t *lent;
	int		eTime;

	LUA_DEBUG("BEGIN - entity.SetEventTime");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetEventTime - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	eTime = (int)luaL_checknumber(L, 2);
	lent->e->eventTime = eTime;

	LUA_DEBUG("END - entity.SetEventTime");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetFalsename(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetFalsename");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		lua_pushnil(L);
		LUA_DEBUG("ERROR - entity.GetFalsename - entity NULL");
		return 1;
	} else {
		lua_pushstring(L, lent->e->falsename);
	}

	LUA_DEBUG("END - entity.GetFalsename");
	return 1;
}

static int Entity_SetFalsename(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetFalsename");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetFalsename - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->falsename = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetFalsename");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetTruename(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetTruename");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetTruename - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->truename);
	}

	LUA_DEBUG("END - entity.GetTruename");
	return 1;
}

static int Entity_SetTruename(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetTruename");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetTruename - entiy NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->truename = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetTruename");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_SetTargetName(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetTargetName");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetTargetName - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->targetname = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetTargetName");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetFalsetarget(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetFalsetarget");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetFalsetarget - entity NULL");
		lua_pushnil(L);
		return 1;
	} else { 
		lua_pushstring(L, lent->e->falsetarget);
	}

	LUA_DEBUG("END - entity.GetFalsetarget");
	return 1;
}

static int Entity_SetFalsetarget(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetFalsetarget");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetFalsetarget - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->falsetarget = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetFalsetarget");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetTruetarget(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetTruetarget");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetTruetarget - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->truetarget);
	}

	LUA_DEBUG("END - entity.GetTruetarget");
	return 1;
}

static int Entity_SetTruetarget(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetTruetarget");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetTruetarget - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->truetarget = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetTruetarget");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetFlags(lua_State *L) {
	lent_t *lent;
	
	LUA_DEBUG("BEGIN - entity.GetFlags");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetFlags - entity NULL");
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->flags);

	LUA_DEBUG("END - entity.GetFlags");
	return 1;
}

static int Entity_SetFlags(lua_State *L) {
	lent_t *lent;
	int		flags;

	LUA_DEBUG("BEGIN - entity.SetFlags");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetFlags - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	flags = (int)luaL_checknumber(L, 2);
	lent->e->flags = flags;

	LUA_DEBUG("END - entity.SetFlags");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetFreeAfterEvent(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetFreeAfterEvent");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetFreeAfterEvent - entity NULL");
		lua_pushboolean(L, 0);
		return 1;
	}
	lua_pushboolean(L, (int)lent->e->freeAfterEvent);

	LUA_DEBUG("END - entity.GetFreeAfterEvent");
	return 1;
}

static int Entity_SetFreeAfterEvent(lua_State *L) {
	lent_t *lent;
	qboolean b;

	LUA_DEBUG("BEGIN - entity.SetFreeAfterEvent");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetFreeAfterEvent - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	b = (qboolean)lua_toboolean(L, 2);
	lent->e->freeAfterEvent = b;

	LUA_DEBUG("END - entity.SetFreeAfterEvent");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetFreetime(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetFreetime");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetFreetime - entity NULL");
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->freetime);

	LUA_DEBUG("END - entity.GetFreetime");
	return 1;
}

static int Entity_GetGreensound(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetGreensound");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetGreensound - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->greensound);
	}

	LUA_DEBUG("END - entity.GetGreensound");
	return 1;
}

static int Entity_SetGreensound(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetGreensound");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetGreensound - entity NLL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->greensound = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetGreensound");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetHealth(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetHealth");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetHealth - entity NULL");
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->health);

	LUA_DEBUG("END - entity.GetHealth");
	return 1;
}

static int Entity_SetHealth(lua_State *L) {
	lent_t *lent;
	int		health;

	LUA_DEBUG("BEGIN - entity.SetHealth");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetHealth - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	health = (int)luaL_checknumber(L, 2);
	lent->e->health = health;

	LUA_DEBUG("END - entity.SetHealth");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetInUse(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetInUse");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetInUse - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lua_pushboolean(L, (int)lent->e->inuse);

	LUA_DEBUG("END - entity.GetInUse");
	return 1;
}

static int Entity_GetLastEnemy(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetLastEnemy");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetLastEnemy - entity NULL");
		lua_pushnil(L);
		return 1;
	}
	if(!lent->e->lastEnemy) {
		lua_pushnil(L);
	} else { 
		Lua_PushEntity(L, lent->e->lastEnemy);
	}

	LUA_DEBUG("END - entity.GetLastEnemy");
	return 1;
}

static int Entity_SetLastEnemy(lua_State *L) {
	lent_t *lent;
	lent_t *lastEnemy;

	LUA_DEBUG("BEGIN - entity.SetLastEnemy");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetLastEnemy - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lastEnemy = Lua_GetEntity(L, 2);
	if(lastEnemy == NULL || lastEnemy->e == NULL) {
		lent->e->lastEnemy = NULL;
	} else {
		lent->e->lastEnemy = lastEnemy->e;
	}

	LUA_DEBUG("END - entity.SetLastEnemy");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetLuaDie(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetLuaDie");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetLuaDie - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->luaDie);
	}

	LUA_DEBUG("END - entity.GetLuaDie");
	return 1;
}

static int Entity_SetLuaDie(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetLuaDie");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetLuaDie - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->luaDie = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetLuaDie");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetLuaEntity(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetLuaEntity");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetLuaEntity - entity NULL");
		lua_pushboolean(L, 0);
		return 1;
	}
	lua_pushboolean(L, (int)lent->e->luaEntity);

	LUA_DEBUG("END - entity.GetLuaEntity");
	return 1;
}

static int Entity_GetLuaFree(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetLuaFree");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetLuaFree - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->luaFree);
	}

	LUA_DEBUG("END - entity.GetLuaFree");
	return 1;
}

static int Entity_SetLuaFree(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetLuaFree");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetLuaFree - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->luaFree = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetLuaFree");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetLuaHurt(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetLuaHurt");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetLuaHurt - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->luaHurt);
	}

	LUA_DEBUG("END - entity.GetLuaHurt");
	return 1;
}

static int Entity_SetLuaHurt(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetLuaHurt");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetLuaHurt - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->luaHurt = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetLuaHurt");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetLuaReached(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetLuaReached");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetLuaReached - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->luaReached);
	}

	LUA_DEBUG("END - entity.GetLuaReached");
	return 1;
}

static int Entity_SetLuaReached(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetLuaReached");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetLuaReached - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->luaReached = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetLuaReached");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetLuaReachedAngular(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetLuaReachedAngular");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetLuaReachedAngular - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->luaReachedAngular);
	}

	LUA_DEBUG("END - entity.GetLuaReachedAngular");
	return 1;
}

static int Entity_SetLuaReachedAngular(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetLuaReachedAngular");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetLuaReachedAngular - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->luaReachedAngular = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetLuaReachedAngular");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetLuaSpawn(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetLuaSpawn");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetLuaSpawn - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->luaSpawn);
	}

	LUA_DEBUG("END - entity.GetLuaSpawn");
	return 1;
}

static int Entity_SetLuaSpawn(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetLuaSpawn");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetLuaSpawn - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->luaSpawn = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetLuaSpawn");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetLuaThink(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetLuaThink");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetLuaThink - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->luaThink);
	}

	LUA_DEBUG("END - entity.GetLuaThink");
	return 1;
}

static int Entity_SetLuaThink(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetLuaThink");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetLuaThink - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->luaThink = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetLuaThink");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetLuaTouch(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetLuaTouch");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetLuaTouch - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->luaTouch);
	}

	LUA_DEBUG("END - entity.GetLuaTouch");
	return 1;
}

static int Entity_SetLuaTouch(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetLuaTouch");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetLuaTouch - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->luaTouch = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetLuaTouch");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetLuaTrigger(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetLuaTrigger");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetLuaTrigger - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->luaTrigger);
	}

	LUA_DEBUG("END - entity.GetLuaTrigger");
	return 1;
}

static int Entity_SetLuaTrigger(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetLuaTrigger");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetLuaTrigger - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->luaTrigger = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetLuaTrigger");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetLuaUse(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetLuaUse");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetLuaUse - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->luaUse);
	}

	LUA_DEBUG("END - entity.GetLuaUse");
	return 1;
}

static int Entity_SetLuaUse(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetLuaUse");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetLuaUse - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->luaUse = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetLuaUse");
	lua_pushboolean(L, qtrue);
	return 1;
}


static int Entity_GetMessage(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetMessage");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetMessage - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->message);
	}

	LUA_DEBUG("END - entity.GetMessage");
	return 1;
}

static int Entity_SetMessage(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetMessage");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetMessage - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->message = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetMessage");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetMethodOfDeath(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetMethodOfDeath");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetMethodOfDeath - entity NULL");
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->methodOfDeath);

	LUA_DEBUG("END - entity.GetMethodOfDeath");
	return 1;
}

static int Entity_SetMethodOfDeath(lua_State *L) {
	lent_t *lent;
	int		mod;

	LUA_DEBUG("BEGIN - entity.SetMethodOfDeath");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetMethodOfDeath - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	mod = (int)luaL_checknumber(L, 2);
	lent->e->methodOfDeath = mod;

	LUA_DEBUG("END - entity.SetMethodOfDeath");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetModel(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.GetModel");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetModel - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->model);
	}

	LUA_DEBUG("END - entity.GetModel");
	return 1;
}

static int Entity_SetModel(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetMode");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetMode - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->model = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetMode");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetModel2(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.GetModel2");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetModel2 - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->model2);
	}

	LUA_DEBUG("END - entity.GetModel2");
	return 1;
}

static int Entity_SetModel2(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetModel2");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetModel2 - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->model2 = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetModel2");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetMovedir(lua_State *L) {
	lent_t *lent;
	vec3_t null = { 0, 0, 0 };

	LUA_DEBUG("BEGIN - entity.GetMovedir");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetMovedir - entity NULL");
		Lua_PushVector(L, null);
		return 1;
	}
	Lua_PushVector(L, lent->e->movedir);

	LUA_DEBUG("END - entity.GetMovedir");
	return 1;
}

static int Entity_SetMovedir(lua_State *L) {
	lent_t	*lent;
	vec_t	*dir;

	LUA_DEBUG("BEGIN - entity.SetMovedir");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetMovedir - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	dir = Lua_GetVector(L, 2);
	VectorCopy(dir, lent->e->movedir);

	LUA_DEBUG("END - entity.SetMovedir");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetMoverstate(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetMoverstate");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetMoverstate - entity NULL");
		lua_pushinteger(L, -1);
		return 1;
	}
	lua_pushinteger(L, (int)lent->e->moverState);

	LUA_DEBUG("END - entity.GetMoverstate");
	return 1;
}

static int Entity_SetMoverstate(lua_State *L) {
	lent_t	*lent;
	moverState_t m;

	LUA_DEBUG("BEGIN - entity.SetMoverstate");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetMoverstate - entiy NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	m = (moverState_t)luaL_checknumber(L, 2);

	lent->e->moverState = m;

	LUA_DEBUG("END - entity.SetMoverstate");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetN00bCount(lua_State *L) {
	lent_t *lent;
	
	LUA_DEBUG("BEGIN - entity.GetN00bCount");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetN00bCount - entity NULL");
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->n00bCount);

	LUA_DEBUG("END - entity.GetN00bCount");
	return 1;
}

static int Entity_SetN00bCount(lua_State *L) {
	lent_t *lent;
	int		cnt;

	LUA_DEBUG("BEGIN - entity.SetN00bCount");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetN00bCount - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	cnt = (int)luaL_checknumber(L, 2);
	lent->e->n00bCount = cnt;

	LUA_DEBUG("END - entity.SetN00bCount");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetWait(lua_State *L) {
	lent_t *lent;
	
	LUA_DEBUG("BEGIN - entity.GetWait");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetWait - entity NULL");
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->wait);

	LUA_DEBUG("END - entity.GetWait");
	return 1;
}

static int Entity_SetWait(lua_State *L) {
	lent_t *lent;
	int		wait;

	LUA_DEBUG("BEGIN - entity.SetWait");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetWait - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	wait = (int)luaL_checknumber(L, 2);

	lent->e->wait = wait;

	LUA_DEBUG("END - entity.SetWait");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetNeverFree(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetNeverFree");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetNeverFree - entity NULL");
		lua_pushboolean(L, 0);
		return 1;
	}
	lua_pushboolean(L, (int)lent->e->neverFree);

	LUA_DEBUG("END - entity.GetNeverFree");
	return 1;
}

static int Entity_SetNeverFree(lua_State *L) {
	lent_t *lent;
	qboolean b;

	LUA_DEBUG("BEGIN - entity.SetNeverFree");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetNeverFree - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	b = (qboolean)lua_toboolean(L, 2);
	lent->e->neverFree = b;

	LUA_DEBUG("END - entity.SetNeverFree");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetNexthink(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetNexthink");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetNexthink - entity NULL");
		lua_pushinteger(L, -1);
		return 1;
	}
	lua_pushinteger(L, lent->e->nextthink);

	LUA_DEBUG("END - entity.GetNexthink");
	return 1;
}

static int Entity_SetNexthink(lua_State *L) {
	lent_t *lent;
	int		next;

	LUA_DEBUG("BEGIN - entity.SetNexthink");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetNexthink - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	next = (int)luaL_checknumber(L, 2);
	lent->e->nextthink = next;

	LUA_DEBUG("END - entity.SetNexthink");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int  Entity_GetNextTrain(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetNextTrain");
	
	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetNextTrain - entity NULL");
		lua_pushnil(L);
		return 1;
	}
	if(lent->e->nextTrain == NULL) {
		lua_pushnil(L);
	} else {
		Lua_PushEntity(L, lent->e->nextTrain);
	}

	LUA_DEBUG("END - entity.GetNextTrain");
	return 1;
}

static int Entity_SetNextTrain(lua_State *L) {
	lent_t *lent;
	lent_t *next;

	LUA_DEBUG("BEGIN - entity.SetNextTrain");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetNextTrain - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	next = Lua_GetEntity(L, 2);
	if(next == NULL || next->e == NULL) {
		lent->e->nextTrain = NULL;
	} else {
		lent->e->nextTrain = next->e;
	}

	LUA_DEBUG("END - entity.SetNextTrain");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetNoiseIndex(lua_State *L) {
	lent_t *lent;
	
	LUA_DEBUG("BEGIN - entity.GetNoiseIndex");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetNoiseIndex - entity NULL");
		lua_pushinteger(L, -1);
		return 1;
	}
	lua_pushinteger(L, lent->e->noise_index);

	LUA_DEBUG("END - entity.GetNoiseIndex");
	return 1;
}

static int Entity_SetNoiseIndex(lua_State *L) {
	lent_t *lent;
	int		idx;

	LUA_DEBUG("BEGIN - entity.SetNoiseIndex");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetNoiseIndex - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	idx = (int)luaL_checknumber(L, 2);
	lent->e->noise_index = idx;

	LUA_DEBUG("END - entity.SetNoiseIndex");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetOldHealth(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetOldHealth");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetOldHealth - entity NULL");
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->old_health);

	LUA_DEBUG("END - entity.GetOldHealth");
	return 1;
}

static int Entity_SetOldHealth(lua_State *L) {
	lent_t *lent;
	int		old_health;

	LUA_DEBUG("BEGIN - entity.SetOldHealth");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetOldHealth - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	old_health = (int)luaL_checknumber(L, 2);
	lent->e->old_health = old_health;

	LUA_DEBUG("END - entity.SetOldHealth");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetPaintarget(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetPaintarget");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetPaintarget - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->paintarget);
	}

	LUA_DEBUG("END - entity.GetPaintarget");
	return 1;
}

static int Entity_SetPaintarget(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetPaintarget");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetPaintarget - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->paintarget = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetPaintarget");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetPainDebounceTime(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetPainDebounceTime");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetPainDebounceTime - entity NULL");
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->pain_debounce_time);

	LUA_DEBUG("END - entity.GetPainDebounceTime");
	return 1;
}

static int Entity_SetPainDebounceTime(lua_State *L) {
	lent_t *lent;
	int		pdb;

	LUA_DEBUG("BEGIN - entity.SetPainDebounceTime");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetPainDebounceTime - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	pdb = (int)luaL_checknumber(L, 2);

	lent->e->pain_debounce_time = pdb;

	LUA_DEBUG("END - entity.SetPainDebounceTime");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetParent(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetParent");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetParent - entity NULL");
		lua_pushnil(L);
		return 1;
	}
	if(lent->e->parent == NULL) {
		lua_pushnil(L);
	} else {
		Lua_PushEntity(L, lent->e->parent);
	}

	LUA_DEBUG("END - entity.GetParent");
	return 1;
}

static int Entity_SetParent(lua_State *L) {
	lent_t *lent;
	lent_t *parent;

	LUA_DEBUG("BEGIN - entity.SetParent");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetParent - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	parent = Lua_GetEntity(L, 2);
	if(parent == NULL || parent->e == NULL) {
		lent->e->parent = NULL;
	} else {
		lent->e->parent = parent->e;
	}

	LUA_DEBUG("END - entity.SetParent");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetPhysicsBounce(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetPhysicsBounce");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetPhysicsBounce - entity NULL");
		lua_pushnumber(L, 0);
		return 1;
	}
	lua_pushnumber(L, lent->e->physicsBounce);

	LUA_DEBUG("END - entity.GetPhysicsBounce");
	return 1;
}

static int Entity_SetPhysicsBounce(lua_State *L) {
	lent_t *lent;
	float	pb;

	LUA_DEBUG("BEGIN - entity.SetPhysicsBounce");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetPhysicsBounce - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	pb = (float)luaL_checknumber(L, 2);
	lent->e->physicsBounce = pb;

	LUA_DEBUG("END - entity.SetPhysicsBounce");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetPhysicsObject(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetPhysicsObject");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetPhysicsObject - entity NULL");
		lua_pushboolean(L, 0);
		return 1;
	}
	lua_pushboolean(L, (int)lent->e->physicsObject);

	LUA_DEBUG("END - entity.GetPhysicsObject");
	return 1;
}

static int Entity_SetPhysicsObject(lua_State *L) {
	lent_t *lent;
	qboolean b;

	LUA_DEBUG("BEGIN - entity.SetPhysicsObject");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetPhysicsObject - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	b = (qboolean)lua_toboolean(L, 2);
	lent->e->physicsObject = b;

	LUA_DEBUG("END - entity.SetPhysicsObject");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetPos1(lua_State *L) {
	lent_t *lent;
	vec3_t	null = { 0, 0, 0 };

	LUA_DEBUG("BEGIN - entity.GetPos1");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetPos1 - entity NULL");
		Lua_PushVector(L, null);
		return 1;
	}
	Lua_PushVector(L, lent->e->pos1);

	LUA_DEBUG("END - entity.GetPos1");
	return 1;
}

static int Entity_SetPos1(lua_State *L) {
	lent_t	*lent;
	vec_t	*vec;

	LUA_DEBUG("BEGIN - entity.SetPos1");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetPos1 - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	vec = Lua_GetVector(L, 2);

	VectorCopy(vec, lent->e->pos1);

	LUA_DEBUG("END - entity.SetPos1");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetPos2(lua_State *L) {
	lent_t *lent;
	vec3_t	null = { 0, 0, 0 };

	LUA_DEBUG("BEGIN - entity.GetPos2");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetPos2 - entity NULL");
		Lua_PushVector(L, null);
		return 1;
	}
	Lua_PushVector(L, lent->e->pos2);

	LUA_DEBUG("END - entity.GetPos2");
	return 1;
}

static int Entity_SetPos2(lua_State *L) {
	lent_t	*lent;
	vec_t	*vec;

	LUA_DEBUG("BEGIN - entity.SetPos2");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetPos2 - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	vec = Lua_GetVector(L, 2);
	VectorCopy(vec, lent->e->pos2);

	LUA_DEBUG("END - entity.SetPos2");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetPrevTrain(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetPrevTrain");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetPrevTrain - entity NULL");
		lua_pushnil(L);
		return 1;
	}
	if(lent->e->prevTrain == NULL) {
		lua_pushnil(L);
	} else {
		Lua_PushEntity(L, lent->e->prevTrain);
	}

	LUA_DEBUG("END - entity.GetPrevTrain");
	return 1;
}

static int Entity_SetPrevTrain(lua_State *L) {
	lent_t *lent;
	lent_t *prev;

	LUA_DEBUG("BEGIN - entity.SetPrevTrain");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetPrevTrain - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	prev = Lua_GetEntity(L, 2);
	if(prev == NULL || prev->e == NULL) {
		lent->e->prevTrain = NULL;
	} else {
		lent->e->prevTrain = prev->e;
	}

	LUA_DEBUG("END - entity.SetPrevTrain");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetRandom(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetRandom");

	lent = Lua_GetEntity(L,1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetRandom - entity NULL");
		lua_pushnumber(L, 0);
		return 1;
	}
	lua_pushnumber(L, lent->e->random);

	LUA_DEBUG("END - entity.GetRandom");
	return 1;
}

static int Entity_SetRandom(lua_State *L) {
	lent_t *lent;
	float	rand;

	LUA_DEBUG("BEGIN - entity.SetRandom");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetRandom - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	rand = (float)luaL_checknumber(L, 2);
	lent->e->random = rand;

	LUA_DEBUG("END - entity.SetRandom");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetRedsound(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetRedsound");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetRedsound - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->redsound);
	}

	LUA_DEBUG("END - entity.GetRedsound");
	return 1;
}

static int Entity_SetRedsound(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetRedsound");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetRedsound - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->redsound = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetRedsound");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetSound1To2(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetSound1To2");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetSound1To2 - entity NULL");
		lua_pushinteger(L, -1);
		return 1;
	}
	lua_pushinteger(L, lent->e->sound1to2);

	LUA_DEBUG("END - entity.GetSound1To2");
	return 1;
}

static int Entity_SetSound1To2(lua_State *L) {
	lent_t *lent;
	int		s1to2;

	LUA_DEBUG("BEGIN - entity.SetSound1To2");

	lent  = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetSound1To2 - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	s1to2 = (int)luaL_checknumber(L, 2);
	lent->e->sound1to2 = s1to2;

	LUA_DEBUG("END - entity.SetSound1To2");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetSound2To1(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetSound2To1");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetSound2To1 - entity NULL");
		lua_pushinteger(L, -1);
		return 1;
	}
	lua_pushinteger(L, lent->e->sound2to1);

	LUA_DEBUG("END - entity.GetSound2To1");
	return 1;
}

static int Entity_SetSound2To1(lua_State *L) {
	lent_t *lent;
	int		s2to1;

	LUA_DEBUG("BEGIN - entity.SetSound2To1");

	lent  = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetSound2To1 - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	s2to1 = (int)luaL_checknumber(L, 2);
	lent->e->sound2to1 = s2to1;

	LUA_DEBUG("END - entity.SetSound2To1");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetSoundLoop(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetSoundLoop");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetSoundLoop - entity NULL");
		lua_pushinteger(L, -1);
		return 1;
	}
	lua_pushinteger(L, lent->e->soundLoop);

	LUA_DEBUG("END - entity.GetSoundLoop");
	return 1;
}

static int Entity_SetSoundLoop(lua_State *L) {
	lent_t *lent;
	int		sl;

	LUA_DEBUG("BEGIN - entity.SetSoundLoop");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetSoundLoop - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	sl = (int)luaL_checknumber(L, 2);
	lent->e->soundLoop = sl;

	LUA_DEBUG("END - entity.SetSoundLoop");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetSoundPos1(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetSoundPos1");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetSoundPos1 - entity NULL");
		lua_pushinteger(L, -1);
		return 1;
	}
	lua_pushinteger(L, lent->e->soundPos1);

	LUA_DEBUG("END - entity.GetSoundPos1");
	return 1;
}

static int Entity_SetSoundPos1(lua_State *L) {
	lent_t *lent;
	int		sp1;

	LUA_DEBUG("BEGIN - entity.SetSoundPos1");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetSoundPos1 - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	sp1 = (int)luaL_checknumber(L, 2);
	lent->e->soundPos1 = sp1;

	LUA_DEBUG("END - entity.SetSoundPos1");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetSoundPos2(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetSoundPos2");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetSoundPos2 - entity NULL");
		lua_pushinteger(L, -1);
		return 1;
	}
	lua_pushinteger(L, lent->e->soundPos2);

	LUA_DEBUG("END - entity.GetSoundPos2");
	return 1;
}

static int Entity_SetSoundPos2(lua_State *L) {
	lent_t *lent;
	int		sp2;

	LUA_DEBUG("BEGIN - entity.SetSoundPos2");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetSoundPos2 - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	sp2 = (int)luaL_checknumber(L, 2);

	lent->e->soundPos2 = sp2;

	LUA_DEBUG("END - entity.SetSoundPos2");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetSpawnflags(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetSpawnflags");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetSpawnflags - entity NULL");
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->spawnflags);

	LUA_DEBUG("END - entity.GetSpawnflags");
	return 1;
}

static int Entity_SetSpawnflags(lua_State *L) {
	lent_t *lent;
	int		sp;

	LUA_DEBUG("BEGIN - entity.SetSpawnflags");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetSpawnflags - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	sp = (int)luaL_checknumber(L, 2);
	lent->e->spawnflags = sp;

	LUA_DEBUG("END - entity.SetSpawnflags");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetSpeed(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetSpeed");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetSpeed - entity NULL");
		lua_pushnumber(L, 0);
		return 1;
	}
	lua_pushnumber(L, lent->e->speed);

	LUA_DEBUG("END - entity.GetSpeed");
	return 1;
}

static int Entity_SetSpeed(lua_State *L) {
	lent_t *lent;
	float	speed;

	LUA_DEBUG("BEGIN - entity.SetSpeed");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetSpeed - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	speed = (float)luaL_checknumber(L, 2);
	lent->e->speed = speed;

	LUA_DEBUG("END - entity.SetSpeed");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetSplashDamage(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetSplashDamage");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetSplashDamage - entity NULL");
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->splashDamage);

	LUA_DEBUG("END - entity.GetSplashDamage");
	return 1;
}

static int Entity_SetSplashDamage(lua_State *L) {
	lent_t *lent;
	int		dmg;

	LUA_DEBUG("BEGIN - entity.SetSplashDamage");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetSplashDamage - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	dmg = (int)luaL_checknumber(L, 2);
	lent->e->splashDamage = dmg;

	LUA_DEBUG("END - entity.SetSplashDamage");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetSplashMethodOfDeath(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetSplashMethodOfDeath");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetSplashMethodOfDeath - entity NULL");
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->splashMethodOfDeath);

	LUA_DEBUG("END - entity.GetSplashMethodOfDeath");
	return 1;
}

static int Entity_SetSplashMethodOfDeath(lua_State *L) {
	lent_t *lent;
	int		mod;

	LUA_DEBUG("BEGIN - entity.SetSplashMethodOfDeath");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetSplashMethodOfDeath - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	mod = (int)luaL_checknumber(L, 2);
	lent->e->splashMethodOfDeath = mod;

	LUA_DEBUG("END - entity.SetSplashMethodOfDeath");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetSplashRadius(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetSplashRadius");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetSplashRadius - entity NULL");
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, lent->e->splashRadius);

	LUA_DEBUG("END - entity.GetSplashRadius");
	return 1;
}

static int Entity_SetSplashRadius(lua_State *L) {
	lent_t *lent;
	int		radius;

	LUA_DEBUG("BEGIN - entity.SetSplashRadius");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetSplashRadius - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	radius = (int)luaL_checknumber(L, 2);
	lent->e->splashRadius = radius;

	LUA_DEBUG("END - entity.SetSplashRadius");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetSwapname(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetSwapname");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetSwapname - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->swapname);
	}

	LUA_DEBUG("END - entity.GetSwapname");
	return 1;
}

static int Entity_SetSwapname(lua_State *L) {
	lent_t	*lent;

	LUA_DEBUG("BEGIN - entity.SetSwapname");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetSwapname - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->swapname = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetSwapname");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetTakedamage(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetTakedamage");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetTakedamage - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lua_pushboolean(L, (int)lent->e->takedamage);

	LUA_DEBUG("END - entity.GetTakedamage");
	return 1;
}

static int Entity_SetTakedamage(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.SetTakedamage");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetTakedamage - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->takedamage = (qboolean)lua_toboolean(L, 2);

	LUA_DEBUG("END - entity.SetTakedamage");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_SetTarget(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.SetTarget");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetTarget - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->target = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetTarget");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetTargetname2(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.SetTarget");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("BEGIN - entity.SetTarget");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->targetname2);
	}

	LUA_DEBUG("BEGIN - entity.SetTarget");
	return 1;
}

static int Entity_SetTargetname2(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.SetTargetname2");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetTargetname2 - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->targetname2 = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetTargetname2");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetTargetShaderName(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetTargetShaderName");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetTargetShaderName - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->targetShaderName);
	}

	LUA_DEBUG("END - entity.GetTargetShaderName");
	return 1;
}

static int Entity_SetTargetShaderName(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.SetTargetShaderName");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetTargetShaderName - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->targetShaderName = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetTargetShaderName");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetTargetShaderNewName(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetTargetShaderNewName");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetTargetShaderNewName - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->targetShaderNewName);
	}

	LUA_DEBUG("END - entity.GetTargetShaderNewName");
	return 1;
}

static int Entity_SetTargetShaderNewName(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.SetTargetShaderNewName");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetTargetShaderNewName - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->targetShaderNewName = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetTargetShaderNewName");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetTargetEnt(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetTargetEnt");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetTargetEnt - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		Lua_PushEntity(L, lent->e->target_ent);
	}

	LUA_DEBUG("END - entity.GetTargetEnt");
	return 1;
}

static int Entity_SetTargetEnt(lua_State *L) {
	lent_t *lent;
	lent_t *targ;

	LUA_DEBUG("BEGIN - entity.SetTargetEnt");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetTargetEnt - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	targ = Lua_GetEntity(L, 2);
	if(targ == NULL || targ->e == NULL) {
		lent->e->target_ent = NULL;
	} else {
		lent->e->target_ent = targ->e;
	}

	LUA_DEBUG("END - entity.SetTargetEnt");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetTeam(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetTeam");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetTeam - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushstring(L, lent->e->team);
	}

	LUA_DEBUG("END - entity.GetTeam");
	return 1;
}

static int Entity_SetTeam(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.SetTeam");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetTeam - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	lent->e->team = (char *)luaL_checkstring(L, 2);

	LUA_DEBUG("END - entity.SetTeam");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetTeamchain(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetTeamchain");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetTeamchain - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		Lua_PushEntity(L, lent->e->teamchain);
	}

	LUA_DEBUG("END - entity.GetTeamchain");
	return 1;
}

static int Entity_SetTeamchain(lua_State *L) {
	lent_t *lent;
	lent_t *team;

	LUA_DEBUG("BEGIN - entity.SetTeamchain");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetTeamchain - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	team = Lua_GetEntity(L, 2);
	if(team == NULL || team->e == NULL) {
		lent->e->teamchain = NULL;
	} else {
		lent->e->teamchain = team->e;
	}

	LUA_DEBUG("END - entity.SetTeamchain");
	lua_pushboolean(L, qtrue);
	return 1;
}

static int Entity_GetTeammaster(lua_State *L) {
	lent_t *lent;

	LUA_DEBUG("BEGIN - entity.GetTeammaster");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.GetTeammaster - entity NULL");
		lua_pushnil(L);
		return 1;
	} else {
		Lua_PushEntity(L, lent->e->teammaster);
	}

	LUA_DEBUG("END - entity.GetTeammaster");
	return 1;
}

static int Entity_SetTeammaster(lua_State *L) {
	lent_t *lent;
	lent_t *team;

	LUA_DEBUG("BEGIN - entity.SetTeammaster");

	lent = Lua_GetEntity(L, 1);
	if(lent == NULL || lent->e == NULL) {
		LUA_DEBUG("ERROR - entity.SetTeammaster - entity NULL");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	team = Lua_GetEntity(L, 2);
	if(team == NULL || team->e == NULL) {
		lent->e->teammaster = NULL;
	} else {
		lent->e->teammaster = team->e;
	}

	LUA_DEBUG("END - entity.SetTeammaster");
	lua_pushboolean(L, qtrue);
	return 1;
}

static const luaL_Reg Entity_ctor[] = {
	{ "Spawn",					Entity_Spawn				},
	{ "Find",					Entity_Find					},
	{ "FindMMB",				Entity_FindMMB				},
	{ "FindNumber",				Entity_FindNumber			},
	{ "FindBModel",				Entity_FindBModel			},
	{ "GetTarget",				Entity_GetTarget			},
	{ "MMBRefit",				Entity_MMBRefit				},
	{ "CallSpawn",				Entity_CallSpawn			},
	{ "DelayedCallSpawn",		Entity_DelayedCallSpawn		},
	{ "Remove",					Entity_Remove				},
	{ "RemoveUnnamedSpawns",	Entity_RemoveUnnamedSpawns	},
	{ "RemoveSpawns",			Entity_RemoveSpawns			},
	{ "RemoveType",				Entity_RemoveType			},
	{ "Use",					Entity_Use					},
	{ NULL,						NULL						}
};

static const luaL_Reg Entity_meta[] = {
	{ "__gc",						Entity_GC						},
	{ "__tostring",					Entity_ToString					},
	{ "GetNumber",					Entity_GetNumber				},
	{ "IsClient",					Entity_IsClient					},
	{ "GetClientname",				Entity_GetClientName			},
	{ "Print",						Entity_Print					},
	{ "CenterPrint",				Entity_CenterPrint				},

	{ "GetClassname",				Entity_GetClassName				}, // args: none; return: string
	{ "SetClassname",				Entity_SetClassName				}, // args: string; return: nothing
		
	{ "GetTargetname",				Entity_GetTargetName			}, // args: none; return: string
	{ "SetTargetname",				Entity_SetTargetName			}, // args: string; return: nothing

	{ "Rotate",						Entity_Rotate					},

	{ "IsRocket",					Entity_IsRocket					},
	{ "IsGrenade",					Entity_IsGrenade				},
	{ "Teleport",					Entity_Teleport					},
	{ "SetKeyValue",				Entity_SetKeyValue				},
	{ "GetOrigin",					Entity_GetOrigin				},
	{ "SetupTrigger",				Entity_SetupTrigger				},

	{ "Lock",						Entity_Lock						},
	{ "Unlock",						Entity_Unlock					},
	{ "IsLocked",					Entity_IsLocked					},

	{ "GetParm",					Entity_GetParm					},
	{ "SetParm",					Entity_SetParm					},

	{ "GetActivator",				Entity_GetActivator				}, // args: none; return: ent
	{ "SetActivator",				Entity_SetActivator				}, // args: ent; return: nothing

	{ "GetAngle",					Entity_GetAngle					}, // args: none; return: float
	{ "SetAngle",					Entity_SetAngle					}, // args: float; return: nothing

	{ "GetApos1",					Entity_GetApos1					}, // args: none; return: vec
	{ "GetApos2",					Entity_GetApos2					}, // args: none; return: vec

	{ "SetApos1",					Entity_SetApos1					}, // args: vec; return: nothing
	{ "SetApos2",					Entity_SetApos2					}, // args: vec; return: nothing

	{ "GetBluename",				Entity_GetBluename				}, // args: none; return: string
	{ "SetBluename",				Entity_SetBluename				}, // args: string; return: nothing

	{ "GetBluesound",				Entity_GetBluesound				}, // args: none; return: string
	{ "SetBluesound",				Entity_SetBluesound				}, // args: string; return: nothing

	{ "GetBooleanstate",			Entity_GetBooleanstate			}, // args: none; return: bool
	{ "SetBooleanstate",			Entity_SetBooleanstate			}, // args: bool; return: nothing

	{ "GetClipmask",				Entity_GetClipmask				}, // args: none; return: int
	{ "SetClipmask",				Entity_SetClipmask				}, // args: int; return: nothing

	{ "GetCount",					Entity_GetCount					}, // args: none; return: int
	{ "SetCount",					Entity_SetCount					}, // args: int; return: nothing

	{ "GetDamage",					Entity_GetDamage				}, // args: none; return: int
	{ "SetDamage",					Entity_SetDamage				}, // args: int; return: nothing

	{ "GetDistance",				Entity_GetDistance				}, // args: none; return: float
	{ "SetDistance",				Entity_SetDistance				}, // args: float; return: nothing

	{ "GetEnemy",					Entity_GetEnemy					}, // args: none; return: ent
	{ "SetEnemy",					Entity_SetEnemy					}, // args: ent; return: nothing

	{ "GetEventTime",				Entity_GetEventTime				}, // args: none; return: int
	{ "SetEventTime",				Entity_SetEventTime				}, // args: int; return: nothing

	{ "GetFalsename",				Entity_GetFalsename				}, // args: none; return: string
	{ "SetFalsename",				Entity_SetFalsename				}, // args: string; return: nothing

	{ "GetTruename",				Entity_GetTruename				}, // args: none; return: string
	{ "SetTruename",				Entity_SetTruename				}, // args: string; return: nothing

	{ "GetFalsetarget",				Entity_GetFalsetarget			}, // args: none; return: string
	{ "SetFalsetarget",				Entity_SetFalsetarget			}, // args: string; return: nothing

	{ "GetTruetarget",				Entity_GetTruetarget			}, // args: none; return: string
	{ "SetTruetarget",				Entity_SetTruetarget			}, // args: string; return: nothing

	{ "GetFlags",					Entity_GetFlags					}, // args: none; return: int
	{ "SetFlags",					Entity_SetFlags					}, // args: int; return: nothing

	{ "GetFreeAfterEvent",			Entity_GetFreeAfterEvent		}, // args: none; return: bool
	{ "SetFreeAfterEvent",			Entity_SetFreeAfterEvent		}, // args: bool; return: nothing

	{ "GetFreetime",				Entity_GetFreetime				}, // args: none; return: int

	{ "GetGreensound",				Entity_GetGreensound			}, // args: none; return: string
	{ "SetGreensound",				Entity_SetGreensound			}, // args: string; return: nothing

	{ "GetHealth",					Entity_GetHealth				}, // args: none; return: int
	{ "SetHealth",					Entity_SetHealth				}, // args: int; return: nothing

	{ "GetInUse",					Entity_GetInUse					}, // args: none; return: bool

	{ "GetLastEnemy",				Entity_GetLastEnemy				}, // args: none; return: ent
	{ "SetLastEnemy",				Entity_SetLastEnemy				}, // args: ent; return: nothing
 
	{ "GetLuaDie",					Entity_GetLuaDie				}, // args: none; return: string
	{ "SetLuaDie",					Entity_SetLuaDie				}, // args: string; return: nothing

	{ "GetLuaEntity",				Entity_GetLuaEntity				}, // args: none; return: bool
 
	{ "GetLuaFree",					Entity_GetLuaFree				}, // args: none; return: string
	{ "SetLuaFree",					Entity_SetLuaFree				},  // args: string; return: nothing

	{ "GetLuaHurt",					Entity_GetLuaHurt				}, // args: none; return: string
	{ "SetLuaHurt",					Entity_SetLuaHurt				}, // args: string; return: nothing

	{ "GetLuaReached",				Entity_GetLuaReached			}, // args: none; return: string
	{ "SetLuaReached",				Entity_SetLuaReached			}, // args: string; return: nothing

	{ "GetLuaReachedAngular",		Entity_GetLuaReachedAngular		}, // args: none; return: string
	{ "SetLuaReachedAngular",		Entity_SetLuaReachedAngular		}, // args: string; return: nothing

	{ "GetLuaSpawn",				Entity_GetLuaSpawn				}, // args: none; return: string
	{ "SetLuaSpawn",				Entity_SetLuaSpawn				}, // args: string; return: nothing

	{ "GetLuaThink",				Entity_GetLuaThink				}, // args: none; return: string
	{ "SetLuaThink",				Entity_SetLuaThink				}, // args: string; return: nothing

	{ "GetLuaTouch",				Entity_GetLuaTouch				}, // args: none; return: string
	{ "SetLuaTouch",				Entity_SetLuaTouch				}, // args: string; return: nothing

	{ "GetLuaTrigger",				Entity_GetLuaTrigger			}, // args: none; return: string
	{ "SetLuaTrigger",				Entity_SetLuaTrigger			}, // args: string; return: nothing

	{ "GetLuaUse",					Entity_GetLuaUse				}, // args: none; return: string
	{ "SetLuaUse",					Entity_SetLuaUse				}, // args: string; return: nothing

	{ "GetMessage",					Entity_GetMessage				}, // args: none; return: string
	{ "SetMessage",					Entity_SetMessage				}, // args: string; return: nothing
 
	{ "GetMethodOfDeath",			Entity_GetMethodOfDeath			}, // args: none; return: int
	{ "SetMethodOfDeath",			Entity_SetMethodOfDeath			}, // args: int; return: nothing

	{ "GetModel",					Entity_GetModel					}, // args: none; return: string
	{ "SetModel",					Entity_SetModel					}, // args: string; return: nothing

	{ "GetModel2",					Entity_GetModel2				}, // args: none; return: string
	{ "SetModel2",					Entity_SetModel2				}, // args: string; return: nothing

	{ "GetMovedir",					Entity_GetMovedir				}, // args: none; return: vec
	{ "SetMovedir",					Entity_SetMovedir				}, // args: vec; return: nothing

	{ "GetMoverstate",				Entity_GetMoverstate			}, // args: none; return: int
	{ "SetMoverstate",				Entity_SetMoverstate			}, // args: int; return: nothing

	{ "GetN00bCount",				Entity_GetN00bCount				}, // args: none; return: int
	{ "SetN00bCount",				Entity_SetN00bCount				}, // args: int; return: nothing

	{ "GetWait",					Entity_GetWait					}, // args: none; return: int
	{ "SetWait",					Entity_SetWait					}, // args: int; return: nothing

	{ "GetNeverFree",				Entity_GetNeverFree				}, // args: none; return: bool
	{ "SetNeverFree",				Entity_SetNeverFree				}, // args: bool; return: nothing

	{ "GetNextthink",				Entity_GetNexthink				}, // args: none; return: int
	{ "SetNextthink",				Entity_SetNexthink				}, // args: int; return: nothing

	{ "GetNextTrain",				Entity_GetNextTrain				}, // args: none; return: ent
	{ "SetNextTrain",				Entity_SetNextTrain				}, // args: ent; return: nothing

	{ "GetNoiseIndex",				Entity_GetNoiseIndex			}, // args: none; return: int
	{ "SetNoiseIndex",				Entity_SetNoiseIndex			}, // args: int; return: nothing

	{ "GetOldHealth",				Entity_GetOldHealth				}, // args: none; return: int
	{ "SetOldHealth",				Entity_SetOldHealth				}, // args: int; return: nothing

	{ "GetPaintarget",				Entity_GetPaintarget			}, // args: none; return: string
	{ "SetPaintarget",				Entity_SetPaintarget			}, // args: string; return: nothing

	{ "GetPainDebounceTime",		Entity_GetPainDebounceTime		}, // args: none; return: int
	{ "SetPainDebounceTime",		Entity_SetPainDebounceTime		}, // args: int; return: nothing

	{ "GetParent",					Entity_GetParent				}, // args: none; return: ent
	{ "SetParent",					Entity_SetParent				}, // args: ent; return: nothing

	{ "GetPhysicsBounce",			Entity_GetPhysicsBounce			}, // args: none; return: float
	{ "SetPhysicsBounce",			Entity_SetPhysicsBounce			}, // args: float; return: nothing

	{ "GetPhysicsObject",			Entity_GetPhysicsObject			}, // args: none; return: bool
	{ "SetPhysicsObject",			Entity_SetPhysicsObject			}, // args: bool; return: nothing

	{ "GetPos1",					Entity_GetPos1					}, // args: none; return: vec
	{ "SetPos1",					Entity_SetPos1					}, // args: vec; return: nothing

	{ "GetPos2",					Entity_GetPos2					}, // args: none; return: vec
	{ "SetPos2",					Entity_SetPos2					}, // args: vec; return: nothing

	{ "GetPrevTrain",				Entity_GetPrevTrain				}, // args: none; return: ent
	{ "SetPrevTrain",				Entity_SetPrevTrain				}, // args: ent; return: nothing

	{ "GetRandom",					Entity_GetRandom				}, // args: none; return: float
	{ "SetRandom",					Entity_SetRandom				}, // args: float; return: nothing

	{ "GetRedsound",				Entity_GetRedsound				}, // args: none; return: string
	{ "SetRedsound",				Entity_SetRedsound				}, // args: string; return: nothing

	{ "GetSound1To2",				Entity_GetSound1To2				}, // args: none; return: int
	{ "SetSound1To2",				Entity_SetSound1To2				}, // args: int; return: nothing

	{ "GetSound2To1",				Entity_GetSound2To1				}, // args: none; return: int
	{ "SetSound2To1",				Entity_SetSound2To1				}, // args: int return: nothing

	{ "GetSoundLoop",				Entity_GetSoundLoop				}, // args: none; return: int
	{ "SetSoundLoop",				Entity_SetSoundLoop				}, // args: int; return: nothing

	{ "GetSoundPos1",				Entity_GetSoundPos1				}, // args: none; return: int
	{ "SetSoundPos1",				Entity_SetSoundPos1				}, // args: int; return: nothing

	{ "GetSoundPos2",				Entity_GetSoundPos2				}, // args: none; return: int
	{ "SetSoundPos2",				Entity_SetSoundPos2				}, // args: int; return: nothing

	{ "GetSpawnflags",				Entity_GetSpawnflags			}, // args: none; return: int
	{ "SetSpawnflags",				Entity_SetSpawnflags			}, // args: int; return: nothing

	{ "GetSpeed",					Entity_GetSpeed					}, // args: none; return: float
	{ "SetSpeed",					Entity_SetSpeed					}, // args: float; return: nothing

	{ "GetSplashDamage",			Entity_GetSplashDamage			}, // args: none; return: int
	{ "SetSplashDamage",			Entity_SetSplashDamage			}, // args: int; return: nothing

	{ "GetSplashMethodOfDeath",		Entity_GetSplashMethodOfDeath	}, // args: none; return: int
	{ "SetSplashMethodOfDeath",		Entity_SetSplashMethodOfDeath	}, // args: int; return: nothing

	{ "GetSplashRadius",			Entity_GetSplashRadius			}, // args: none; return: int
	{ "SetSplashRadius",			Entity_SetSplashRadius			}, // args: int; return: nothing

	{ "GetSwapname",				Entity_GetSwapname				}, // args: none; return: string
	{ "SetSwapname",				Entity_SetSwapname				}, // args: string; return: nothing

	{ "GetTakedamage",				Entity_GetTakedamage			}, // args: none; return: bool
	{ "SetTakedamage",				Entity_SetTakedamage			}, // args: bool; return: nothing

	{ "SetTarget",					Entity_SetTarget				}, // args: string; return: nothing

	{ "GetTargetname2",				Entity_GetTargetname2			}, // args: none; return: string
	{ "SetTargetname2",				Entity_SetTargetname2			}, // args: string; return: nothing

	{ "GetTargetShaderName",		Entity_GetTargetShaderName		}, // args: none; return: string
	{ "SetTargetShaderName",		Entity_SetTargetShaderName		}, // args: string; return: nothing

	{ "GetTargetShaderNewName",		Entity_GetTargetShaderNewName	}, // args: none; return: string
	{ "SetTargetShaderNewName",		Entity_SetTargetShaderNewName	}, // args: string; return: nothing

	{ "GetTargetEnt",				Entity_GetTargetEnt				}, // args: none; return: ent
	{ "SetTargetEnt",				Entity_SetTargetEnt				}, // args: ent; return: nothing

	{ "GetTeam",					Entity_GetTeam					}, // args: none; return: string
	{ "SetTeam",					Entity_SetTeam					}, // args: string; return nothing

	{ "GetTeamchain",				Entity_GetTeamchain				}, // args: none; return: ent
	{ "SetTeamchain",				Entity_SetTeamchain				}, // args: ent; return: nothing

	{ "GetTeammaster",				Entity_GetTeammaster			},
	{ "SetTeammaster",				Entity_SetTeammaster			},

	{ NULL,							NULL							}
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
