// mover lib for lua

#include "g_lua.h"

#ifdef G_LUA
/***
A module for entity movement, especially for mover entities such as doors.
@module mover
*/

/***
Stops translational movement on ent immediately.
@function Halt
@param ent Entity or entity number.
@return Success or failure.
*/
static int Mover_Halt(lua_State *L) {
	lent_t		*lent;
	gentity_t	*ent = NULL;
	int			id = 0;
	
	if(lua_isnumber(L, 1)) {
		id =  luaL_checkint(L, 1);
		if(id < 0 || id > MAX_GENTITIES - 1) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
		ent = &g_entities[id];
		if(ent == NULL) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
	} else {
		lent = Lua_GetEntity(L, 1);
		if(lent == NULL || lent->e == NULL) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
		ent = lent->e;
	}

	LUA_DEBUG("Mover_Halt - start: end=%d", ent->s.number);
	BG_EvaluateTrajectory(&ent->s.pos, level.time, ent->r.currentOrigin);
	VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
	ent->s.pos.trType = TR_STATIONARY;
	ent->s.pos.trTime = level.time;
	ent->nextthink = 0;
	ent->think = NULL;
	ent->nextTrain = NULL;
	trap_LinkEntity(ent);
	LUA_DEBUG("Mover_Halt - return: halted ent");
	lua_pushboolean(L, qtrue);
	return 1;
}

/***
Stops rotational movement on ent immediately.
@function HaltAngles
@param ent Entity or entity number.
@return Success or failure.
*/
static int Mover_HaltAngles(lua_State * L)
{
	lent_t			*lent;
	gentity_t       *ent = NULL;
	int				id = 0;

	if(lua_isnumber(L, 1)) {
		id = luaL_checkint(L, 1);
		if(id < 0 || id > MAX_GENTITIES - 1) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
		ent = &g_entities[id];
		if(ent) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
	} else {
		lent = Lua_GetEntity(L, 1);
		if(lent == NULL || lent->e == NULL) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
		ent = lent->e;
	}

	LUA_DEBUG("Mover_HaltAngles - start: ent=%d", ent->s.number);
	if(ent)
	{
		BG_EvaluateTrajectory(&ent->s.apos, level.time, ent->s.apos.trBase);
		ent->s.apos.trType = TR_STATIONARY;
		ent->s.apos.trTime = level.time;
		trap_LinkEntity(ent);
		LUA_DEBUG("Mover_HaltAngles - return: halted ent");
	}

	lua_pushboolean(L, qtrue);
	return 1;
}

extern void Reached_Train(gentity_t *ent);
extern void Think_SetupTrainTargets(gentity_t *ent);
extern void SetMoverState(gentity_t *ent, moverState_t moverState, int time);

/***
Moves an entity like a func_train entity. Targets have to be path_corner entities.
@function AsTrain
@param mover Entity to move.
@param target path_corner entity to move to.
@param speed Speed to move with to the first path_corner.
@return Success or failure.
 */
static int Mover_AsTrain(lua_State * L)
{
	lent_t			*lent, *tlent;
	gentity_t      *ent = NULL;
	gentity_t      *targ = NULL;
	vec3_t          move;
	float           length;
	int				id = 0, tid = 0;

	float           speed = (float)luaL_checknumber(L, 3);

	if(lua_isnumber(L, 1)) {
		id = luaL_checkint(L, 1);
		if(id < 0 || id > MAX_GENTITIES - 1) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
		ent = &g_entities[id];
		if(ent == NULL) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
	} else {
		lent = Lua_GetEntity(L, 1);
		if(lent == NULL || lent->e == NULL) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
		ent = lent->e;
	}
	if(luaL_checkint(L, 2)) {
		tid = luaL_checkint(L, 2);
		if(tid < 0 || tid > MAX_GENTITIES - 1) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
		targ = &g_entities[tid];
		if(targ == NULL) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
	} else {
		tlent = Lua_GetEntity(L, 2);
		if(!tlent || tlent->e == NULL) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
		targ = tlent->e;
	}

	LUA_DEBUG("Mover_AsTrain - start: ent=%d target=%d speed=%f", ent->s.number, targ->s.number, speed);

	if(ent == NULL || targ == NULL)
	{
		LUA_DEBUG("Mover_AsTrain - return: ent or/and target missing");
		lua_pushboolean(L, qfalse);
		return 1;
	}
	if(speed < 1)
	{
		LUA_DEBUG("Mover_AsTrain - moving: speed less than 1 fixed");
		speed = 1;
	}

	if(ent->nextTrain)
	{
		LUA_DEBUG("Mover_AsTrain - pathing: NextTrain=%d ", ent->nextTrain->s.number);
	}

	ent->speed = speed;
	ent->nextTrain = targ;
	ent->reached = Reached_Train;
	ent->target = G_NewString(targ->targetname);

	Think_SetupTrainTargets(ent);

	BG_EvaluateTrajectory(&ent->s.pos, level.time, ent->r.currentOrigin);
	VectorCopy(ent->r.currentOrigin, ent->s.origin);

	VectorCopy(ent->s.origin, ent->pos1);
	VectorCopy(ent->nextTrain->s.origin, ent->pos2);

	VectorSubtract(ent->pos2, ent->pos1, move);
	length = VectorLength(move);

	if(length <= 0.05)
	{
		G_SetOrigin(ent, ent->pos2);
		LUA_DEBUG("Mover_AsTrain - return: snapped to target, length too small length=%f", length);
		lua_pushboolean(L, qtrue);
		return 1;
	}

	ent->s.pos.trDuration = length * 1000 / speed;

	ent->s.loopSound = ent->nextTrain->soundLoop;

	SetMoverState(ent, MOVER_1TO2, level.time);

	LUA_DEBUG("Mover_AsTrain - return: moving to target, length=%f duration=%d", length, ent->s.pos.trDuration);
	lua_pushboolean(L, qtrue);
	return 1;
}

/***
Sets the angles of ent to the specified values. Values are sorted Pitch (around Y-Axis), Yaw (around Z-Axis) and Roll (around X-Axis). These can also be stowed in a vector angles.
@function SetAngles
@param ent Entity or entity number.
@param y Pitch.
@param z Yaw.
@param x Roll.
@return Success or failure.
*/
/***
Sets the angles of ent to the specified value.
@function SetAngles
@param ent Entity or entity number.
@param agles Vector containing the new angles.
@return Success or failure.
*/
static int Mover_SetAngles(lua_State * L)
{
	vec3_t          newAngles;
	lent_t			*lent;
	gentity_t      *ent = NULL;
	vec_t          *target;
	int				id = 0;

	if(lua_isnumber(L, 1)) {
		id = luaL_checkint(L, 1);
		if(id < 0 || id > MAX_GENTITIES - 1) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
		ent = &g_entities[id];
		if(ent == NULL) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
	} else {
		lent = Lua_GetEntity(L, 1);
		if(lent == NULL || lent->e == NULL) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
		ent = lent->e;
	}

	if(Lua_IsVector(L, 2))
	{
		target = Lua_GetVector(L, 2);
		VectorCopy(target, newAngles);
	}
	else
	{
		newAngles[0] = luaL_checkint(L, 2);
		newAngles[1] = luaL_checkint(L, 3);
		newAngles[2] = luaL_checkint(L, 4);
	}
	LUA_DEBUG("Mover_SetAngles - start: ent=%d angles=%s", ent->s.number, vtos(newAngles));
	if(ent)
	{
		VectorCopy(newAngles, ent->s.apos.trBase);
		VectorCopy(newAngles, ent->s.angles);
		trap_LinkEntity(ent);
		LUA_DEBUG("Mover_SetAngles - return: moved");
	}
	lua_pushboolean(L, qtrue);
	return 1;
}

/***
Sets the angles of ent to the specified values. Values are sorted Pitch (around Y-Axis), Yaw (around Z-Axis) and Roll (around X-Axis). These can also be stowed in a vector angles.
@function SetAngles2
@param ent Entity or entity number.
@param y Pitch.
@param z Yaw.
@param x Roll.
@return Success or failure.
*/
/***
Sets the angles of ent to the specified value.
@function SetAngles2
@param vec Vector containing the new angles.
@return Success or failure.
*/
static int Mover_SetAngles2(lua_State * L)
{
	vec3_t          newAngles;
	lent_t			*lent;
	gentity_t      *ent = NULL;
	vec_t          *target;
	int				id = 0;

	if(lua_isnumber(L, 1)) {
		id = luaL_checkint(L, 1);
		if(id < 0 || id > MAX_GENTITIES - 1) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
		ent = &g_entities[id];
		if(ent == NULL) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
	} else {
		lent = Lua_GetEntity(L, 1);
		if(lent == NULL || lent->e == NULL) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
		ent = lent->e;
	}

	if(Lua_IsVector(L, 2))
	{
		target = Lua_GetVector(L, 2);
		VectorCopy(target, newAngles);
	}
	else
	{
		newAngles[0] = luaL_checkint(L, 2);
		newAngles[1] = luaL_checkint(L, 3);
		newAngles[2] = luaL_checkint(L, 4);
	}
	LUA_DEBUG("Mover_SetAngles2 - start: ent=%d angles=%s", ent->s.number, vtos(newAngles));
	if(ent)
	{
		VectorCopy(newAngles, ent->s.angles2);
		trap_LinkEntity(ent);
		LUA_DEBUG("Mover_SetAngles2 - return: moved");
	}
	lua_pushboolean(L, qtrue);
	return 1;
}

/***
Set the position of ent to the specified value.
@function SetPosition
@param ent Entity or entity number.
@param x X coordinates.
@param y Y coordinates.
@param z Z coordinates.
@return Success or failure.
*/
/***
Set the position of ent to the specified value.
@function SetPosition
@param ent Entity or entity number.
@param vec Vector containing the new position.
@return Success or failure.
*/
static int Mover_SetPosition(lua_State * L)
{
	vec3_t          newOrigin;
	lent_t			*lent;
	gentity_t      *ent = NULL;
	vec_t          *target;
	int				id = 0;

	if(lua_isnumber(L, 1)) {
		id = luaL_checkint(L, 1);
		if(id < 0 || id > MAX_GENTITIES - 1) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
		ent = &g_entities[id];
		if(ent == NULL) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
	} else {
		lent = Lua_GetEntity(L, 1);
		if(lent == NULL || lent->e == NULL)
		{
			lua_pushboolean(L, qfalse);
			return 1;
		}
		ent = lent->e;
	}

	if(Lua_IsVector(L, 2))
	{
		target = Lua_GetVector(L, 2);
		VectorCopy(target, newOrigin);
	}
	else
	{
		newOrigin[0] = luaL_checkint(L, 2);
		newOrigin[1] = luaL_checkint(L, 3);
		newOrigin[2] = luaL_checkint(L, 4);
	}
	LUA_DEBUG("Mover_SetPosition - start: ent=%d pos=%s", ent->s.number, vtos(newOrigin));
	if(ent)
	{
		G_SetOrigin(ent, newOrigin);
		trap_LinkEntity(ent);
		LUA_DEBUG("Mover_SetPosition - return: moved");
	}
	lua_pushboolean(L, qtrue);
	return 1;
}

static void SetTrajectoryLinear(trajectory_t * tr, const float speed, const vec3_t endPosition)
{
	vec3_t          delta;
	float           length;

	VectorSubtract(endPosition, tr->trBase, delta);
	length = VectorLength(delta);
	VectorNormalize(delta);
	tr->trDuration = length * 1000 / speed;
	tr->trTime = level.time;
	VectorScale(delta, speed, tr->trDelta);
	tr->trType = TR_LINEAR_STOP;
}

/***
Rotates ent with a given speed (in degrees per second) to the specified values. Values are sorted Pitch (around Y-Axis), Yaw (around Z-Axis) and Roll (around X-Axis). These can also be stowed in a vector angles.
@function ToAngles
@param ent Entity or entity number.
@param speed Speed to rotate with.
@param y Pitch.
@param z Yaw.
@param x Roll.
@return Succes or failure.
*/
/***
Rotates ent with a given speed (in degrees per second) to the specified values. 
@function ToAngles
@param ent Entity or entity number.
@param speed Speed to rotate with.
@param vec Vector conataining target angles.
@return Success or failure.
*/
static int Mover_ToAngles(lua_State * L)
{
	vec3_t          newAngles;
	lent_t			*lent;
	gentity_t      *ent = NULL;
	float           speed;
	vec_t          *target;
	int				id = 0;

	if(lua_isnumber(L, 1)) {
		id = luaL_checkint(L, 1);
		if(id < 0 || id > MAX_GENTITIES - 1) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
		ent = &g_entities[id];
		if(ent == NULL) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
	} else {
		lent = Lua_GetEntity(L, 1);
		if(lent == NULL || lent->e == NULL) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
		ent = lent->e;
	}

	speed = (float)luaL_checknumber(L, 2);

	if(Lua_IsVector(L, 3))
	{
		target = Lua_GetVector(L, 3);
		VectorCopy(target, newAngles);
	}
	else
	{
		newAngles[0] = luaL_checkint(L, 3);
		newAngles[1] = luaL_checkint(L, 4);
		newAngles[2] = luaL_checkint(L, 5);
	}

	LUA_DEBUG("Mover_ToAngles - start: ent=%d angles=%s speed=%f", ent->s.number, vtos(newAngles), speed);
	if(ent)
	{
		BG_EvaluateTrajectory(&ent->s.apos, level.time, ent->s.apos.trBase);
		SetTrajectoryLinear(&ent->s.apos, speed, newAngles);
		ent->moverState = MOVER_LUA;
		trap_LinkEntity(ent);
		LUA_DEBUG("Mover_ToAngles - return: moving");
	}
	lua_pushboolean(L, qtrue);
	return 1;
}

/***
Moves ent with a given speed to the specified values. Can also be stowed in a vector pos.
@function ToPosition
@param ent Entity or entity number.
@param speed Speed to move with.
@param x X coordinates.
@param y Y coordinates.
@param z Z coordinates.
@return Success or failure.
*/
static int Mover_ToPosition(lua_State * L)
{
	vec3_t          newOrigin;
	lent_t			*lent;
	gentity_t      *ent = NULL;
	float           speed;
	vec_t          *target;
	int				id = 0;

	if(lua_isnumber(L, 1)) {
		id = luaL_checkint(L, 1);
		if(id < 0 ||id > MAX_GENTITIES - 1) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
		ent = &g_entities[id];
		if(ent == NULL) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
	} else {
		lent = Lua_GetEntity(L,1);
		if(lent == NULL || lent->e == NULL) {
			lua_pushboolean(L, qfalse);
			return 1;
		}
		ent = lent->e;
	}

	speed = (float)luaL_checknumber(L, 2);

	if(Lua_IsVector(L, 3))
	{
		target = Lua_GetVector(L, 3);
		VectorCopy(target, newOrigin);
	}
	else
	{
		newOrigin[0] = luaL_checkint(L, 3);
		newOrigin[1] = luaL_checkint(L, 4);
		newOrigin[2] = luaL_checkint(L, 5);
	}
	

	LUA_DEBUG("Mover_ToPosition - start: ent=%d pos=%s speed=%f", ent->s.number, vtos(newOrigin), speed);
	if(ent)
	{
		BG_EvaluateTrajectory(&ent->s.pos, level.time, ent->s.pos.trBase);
		SetTrajectoryLinear(&ent->s.pos, speed, newOrigin);
		ent->moverState = MOVER_LUA;
		trap_LinkEntity(ent);
		LUA_DEBUG("Mover_ToPosition - return: moving");
	}
	lua_pushboolean(L, qtrue);
	return 1;
}

static const luaL_Reg lib_mover[] = {
	{ "Halt",			Mover_Halt			},
	{ "HaltAngles",		Mover_HaltAngles	},
	{ "AsTrain",		Mover_AsTrain		},
	{ "SetPosition",	Mover_SetPosition	},
	{ "SetOrigin",		Mover_SetPosition	},
	{ "ToPosition",		Mover_ToPosition	},
	{ "SetAngles",		Mover_SetAngles		},
	{ "SetAngles2",		Mover_SetAngles2	},
	{ "ToAngles",		Mover_ToAngles		},
	{ NULL,				NULL				}
};

int Luaopen_Mover(lua_State *L) {
	luaL_register(L, "mover", lib_mover);
	return 1;
}
#endif
