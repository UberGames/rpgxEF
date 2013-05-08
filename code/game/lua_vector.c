// vector lib for Lua

#include "g_lua.h"

#if(defined(CG_LUA) || defined(G_LUA))

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

/***
A module implementing vectors.
@module vector
*/

/***
Create a new vector.
@function New
@return A new vector with x = y = z = 0.
*/
static int Vector_New(lua_State *L) {
	vec_t *v;

	v = (vec_t *)lua_newuserdata(L, sizeof(vec3_t));

	luaL_getmetatable(L, "vector");
	lua_setmetatable(L, -2);

	VectorClear(v);

	return 0;
}

/***
Create a new vector with the given values.
@function Construct
@param x X value.
@param y Y value.
@param z Z value.
@return A new vector with given values for x, y, and z.
*/
static int Vector_Construct(lua_State *L) {
	vec_t *v;

	v = (vec_t *)lua_newuserdata(L, sizeof(vec3_t));

	luaL_getmetatable(L, "vector");
	lua_setmetatable(L, -2);

	v[0] = luaL_optnumber(L, 1, 0);
	v[1] = luaL_optnumber(L, 2, 0);
	v[2] = luaL_optnumber(L, 3, 0);

	return 0;
}

/***
Set vector to given values.
@function Set
@param x New x value.
@param y New y value.
@param z New z value.
*/
static int Vector_Set(lua_State *L) {
	vec_t *v;

	v = Lua_GetVector(L, 1);

	v[0] = luaL_optnumber(L, 2, 0);
	v[0] = luaL_optnumber(L, 3, 0);
	v[0] = luaL_optnumber(L, 4, 0);

	return 0;
}

/***
Clears the vector which means x = y = z = 0.
@function Clear
*/
static int Vector_Clear(lua_State * L)
{
	vec_t          *a;

	a = Lua_GetVector(L, 1);

	VectorClear(a);

	return 0;
}

/***
Add vector b to vector a and return the result.
@function Add
@param a Vector.
@param b Vector.
@return Result.
*/
static int Vector_Add(lua_State *L) {
	vec_t	*a, *b;
	vec3_t	c;

	a = Lua_GetVector(L, 1);
	b = Lua_GetVector(L, 2);

	VectorAdd(a, b, c);

	Lua_PushVector(L, c);
	return 0;
}

/***
Substract vector b from vector a and return the result.
@function Subtract
@param a Vector.
@param b Vector.
@return Result.
*/
static int Vector_Subtract(lua_State *L) {
	vec_t	*a, *b;
	vec3_t	c;

	a = Lua_GetVector(L, 1);
	b = Lua_GetVector(L, 2);

	VectorSubtract(a, b, c);

	Lua_PushVector(L, c);
	return 0;
}

/***
Scale vector a by value b and return the result.
@function Scale.
@param a Vector.
@param b Scaling factor.
@return Result.
*/
static int Vector_Scale(lua_State *L) {
	vec_t *a, b;
	vec3_t c;

	a = Lua_GetVector(L, 1);
	b = luaL_checknumber(L, 2);

	VectorScale(a,b,c);

	Lua_PushVector(L, c);
	return 0;
}

/***
Get the length of the vector.
@function Length
@return Length of given vector.
*/
static int Vector_Length(lua_State *L) {
	vec_t	*a;
	vec_t	len;

	a = Lua_GetVector(L,1);

	len = VectorLength(a);
	lua_pushnumber(L, len);

	return 0;
}

/***
Normalize a vector.
@function Normalize
@return Vector length.
*/
static int Vector_Normalize(lua_State *L) {
	vec_t	*a;
	vec_t	len;

	a = Lua_GetVector(L, 1);

	len = VectorNormalize(a);
	lua_pushnumber(L, len);

	return 0;
}

/***
Normalize a vector (fast method).
@function NormalizeFast
@return Vector length.
*/
static int Vector_NormalizeFast(lua_State *L) {
	vec_t *a;

	a = Lua_GetVector(L, 1);

	VectorNormalizeFast(a);

	return 0;
}

/***
Rotates point around a given vector.
@function RotatePointAround
@param dir Vector to rotate around (must be normalized).
@param point Point to be rotated.
@param degrees How many degrees to rotate.
@param dest Point after rotation.
*/
static int Vector_RotatePointAround(lua_State *L) {
	vec_t	*dst, *dir, *point;
	vec_t	degrees;

	dst = Lua_GetVector(L, 1);
	dir = Lua_GetVector(L, 2);
	point = Lua_GetVector(L, 3);
	degrees = luaL_checknumber(L, 4);

	RotatePointAroundVector(dst, dir, point, degrees);

	return 0;
}

/***
Finds a vector perpendicular to the source vector. 
@function Perpendicular
@param src Source vector. 
@param dest A vector that is perpendicular to src (the result is stored here)
*/
static int Vector_Perpendicular(lua_State *L) {
	vec_t	*dst, *src;

	dst = Lua_GetVector(L, 1);
	src = Lua_GetVector(L, 2);

	PerpendicularVector(dst, src);

	return 0;
}

/***
Convert a vector to angles.
@function VecToAngles
@param vector Vector.
@param angles Vector the results are stored in.
*/
static int Vector_VecToAngles(lua_State *L) {
	vec_t *v, *t;
	
	v = Lua_GetVector(L, 1);
	t = Lua_GetVector(L, 2);

	vectoangles(v, t);

	return 0;
} 

/***
Calculate forward, right, and up vectors from given angles.
@function AngleVectors
@param angles Angles.
@param fwd Vector to store forward.
@param right Vector to store right.
@param up Vector to store up.
*/
static int Vector_AngleVectors(lua_State *L) {
	vec_t *v, *fwd, *right, *up; 

	v = Lua_GetVector(L, 1);
	fwd = Lua_GetVector(L, 2);
	right = Lua_GetVector(L, 3);
	up = Lua_GetVector(L, 4);

	AngleVectors(v, fwd, right, up);

	return 0;
}

static int Vector_Index(lua_State *L) {
	vec_t		*v;
	const char	*i;

	v = Lua_GetVector(L, 1);
	i = luaL_checkstring(L, 2);

	switch(*i) {
		case '0': case 'x': case 'r': lua_pushnumber(L, v[0]); break;
		case '1': case 'y': case 'g': lua_pushnumber(L, v[1]); break;
		case '2': case 'z': case 'b': lua_pushnumber(L, v[2]); break;
		default: lua_pushnil(L); break;
	}

	return 0;
}

static int Vector_NewIndex(lua_State *L) {
	vec_t			*v;
	const char		*i;
	vec_t			t;

	v = Lua_GetVector(L, 1);
	i = luaL_checkstring(L, 2);
	t = luaL_checknumber(L, 3);

	switch(*i) {
		case '0': case 'x': case 'r': v[0] = t; break;
		case '1': case 'y': case 'g': v[1] = t; break;
		case '2': case 'z': case 'b': v[2] = t; break;
	}

	return 1;
}

static int Vector_AddOperator(lua_State *L) {
	vec_t	*a, *b;
	vec3_t	c;

	a = Lua_GetVector(L, 1);
	b = Lua_GetVector(L, 2);

	VectorAdd(a, b, c);

	Lua_PushVector(L, c);

	return 1;
}

static int Vector_SubOperator(lua_State *L) {
	vec_t	*a, *b;
	vec3_t	c;

	a = Lua_GetVector(L, 1);
	b = Lua_GetVector(L, 2);

	VectorSubtract(a, b, c);

	Lua_PushVector(L, c);

	return 1;
}

static int Vector_DotOperator(lua_State *L) {
	vec_t	*a, *b;

	a = Lua_GetVector(L, 1);
	b = Lua_GetVector(L, 2);

	lua_pushnumber(L, DotProduct(a,b));

	return 1;
}

static int Vector_NegateOperator(lua_State *L) {
	vec_t	*a;
	vec3_t	b;

	a = Lua_GetVector(L, 1);

	VectorNegate(a, b);

	Lua_PushVector(L, b);

	return 1;
}

static int Vector_GC(lua_State *L) {
	return 0;
}

static int Vector_ToString(lua_State *L) {
	vec_t	*vec;

	vec = Lua_GetVector(L,1);
	lua_pushstring(L, va("(%i %i %i)", (int)vec[0], (int)vec[1], (int)vec[2]));

	return 1;
}

static const luaL_Reg vector_ctor[] = {
	{ "New",					Vector_New			 },
	{ "Construct",			Vector_Construct		 },
	{ "RotatePointAround",	Vector_RotatePointAround },
	{ "Perpendicular",		Vector_Perpendicular	 },
	{ "VecToAngles",		Vector_VecToAngles		 },
	{ "AngleVectors",		Vector_AngleVectors		 },
	{ NULL,					NULL					 }
};

static const luaL_Reg vector_meta[] = {
	{ "__index",			Vector_Index		},
	{ "__newindex",		Vector_NewIndex			},
	{ "__add",			Vector_AddOperator		},
	{ "__sub",			Vector_SubOperator		},
	{ "__mul",			Vector_DotOperator		},
	{ "__unm",			Vector_NegateOperator	},
	{ "__gc",			Vector_GC				},
	{ "__tostring",		Vector_ToString			},
	{ "Set",				Vector_Set			},
	{ "Length",			Vector_Length			},
	{ "Normalize",		Vector_Normalize		},
	{ "NormalizeFast",	Vector_NormalizeFast	},
	{ "Add",				Vector_Add			},
	{ "Subtract",		Vector_Subtract			},	
	{ "Scale",			Vector_Scale			},
	{ "Clear",			Vector_Clear			},
	{NULL,				NULL					}
};

int Luaopen_Vector(lua_State *L) {
	luaL_newmetatable(L, "vector");

	luaL_register(L, NULL, vector_meta);
	luaL_register(L, "vector", vector_ctor);

	return 1;
}

void Lua_PushVector(lua_State *L, vec3_t v) {
	vec_t	*vec;

	vec = (vec_t *)lua_newuserdata(L, sizeof(vec3_t));

	luaL_getmetatable(L, "vector");
	lua_setmetatable(L, -2);

	VectorCopy(v, vec);
}

vec_t *Lua_GetVector(lua_State * L, int argNum)
{
	void           *ud;

	ud = luaL_checkudata(L, argNum, "vector");
	luaL_argcheck(L, ud != NULL, argNum, "`vector' expected");
	return (vec_t *) ud;
}

int Lua_IsVector(lua_State * L, int idx)
{
	void           *ud;

	if(lua_isuserdata(L, idx))
	{
		if(lua_getmetatable(L, idx))
		{
			ud = luaL_checkudata(L, idx, "vector");
			return (ud != NULL);
		}
	}
	return 0;
}

#endif

