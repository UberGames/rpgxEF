// vector lib for Lua

#include "g_lua.h"

#if(defined(CG_LUA) || defined(G_LUA))

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

// vector.New()
// Allocates and returns a new vector (0|0|0).
static int Vector_New(lua_State *L) {
	vec_t *v;

	v = (vec_t *)lua_newuserdata(L, sizeof(vec3_t));

	luaL_getmetatable(L, "vector");
	lua_setmetatable(L, -2);

	VectorClear(v);

	return 1;
}

// vector.Construct(float x, float y, float z)
// Allocates and returns a new vector (x|y|z).
static int Vector_Construct(lua_State *L) {
	vec_t *v;

	v = (vec_t *)lua_newuserdata(L, sizeof(vec3_t));

	luaL_getmetatable(L, "vector");
	lua_setmetatable(L, -2);

	v[0] = luaL_optnumber(L, 1, 0);
	v[1] = luaL_optnumber(L, 2, 0);
	v[2] = luaL_optnumber(L, 3, 0);

	return 1;
}

// vector.Set(vector v, float x, float y, float z)
// Set the vector v to the specified values.
static int Vector_Set(lua_State *L) {
	vec_t *v;

	v = Lua_GetVector(L, 1);

	v[0] = luaL_optnumber(L, 2, 0);
	v[0] = luaL_optnumber(L, 3, 0);
	v[0] = luaL_optnumber(L, 4, 0);

	return 1;
}

// vector.Clear(vector vec)
// Clears vector v by setting it to (0|0|0).
static int Vector_Clear(lua_State * L)
{
	vec_t          *a;

	a = Lua_GetVector(L, 1);

	VectorClear(a);

	return 1;
}

// vector.Vector_Add(vector a, vector b, vector c)
// Adds a and b together and stores the result in c.
static int Vector_Add(lua_State *L) {
	vec_t	*a, *b, *c;

	a = Lua_GetVector(L, 1);
	b = Lua_GetVector(L, 2);
	c = Lua_GetVector(L, 3);

	VectorAdd(a, b, c);

	return 1;
}

// vector.Vector_subtract(vector a, vector b, vector c)
// Subtracts b from a and stores the result in c.
static int Vector_Subtract(lua_State *L) {
	vec_t	*a, *b, *c;

	a = Lua_GetVector(L, 1);
	b = Lua_GetVector(L, 2);
	c = Lua_GetVector(L, 3);

	VectorSubtract(a, b, c);

	return 1;
}

// vector.Scale(vector a, float b, vector c)
// Scales a vector by the value of b and stores the result in c.
static int Vector_Scale(lua_State *L) {
	vec_t *a, b, *c;

	a = Lua_GetVector(L, 1);
	b = luaL_checknumber(L, 2);
	c = Lua_GetVector(L, 3);

	VectorScale(a,b,c);

	return 1;
}

// vector.Length(vector a)
// Returns the length of a.
static int Vector_Length(lua_State *L) {
	vec_t	*a;
	vec_t	len;

	a = Lua_GetVector(L,1);

	len = VectorLength(a);
	lua_pushnumber(L, len);

	return 1;
}

// vector.Normalize(vector a)
// Normalizes a
static int Vector_Normalize(lua_State *L) {
	vec_t	*a;
	vec_t	len;

	a = Lua_GetVector(L, 1);

	len = VectorNormalize(a);
	lua_pushnumber(L, len);

	return 1;
}

// vector.NormalizeFast(vector vec)
// Normalzes the vector (faster method)
static int Vector_NormalizeFast(lua_State *L) {
	vec_t *a;

	a = Lua_GetVector(L, 1);

	VectorNormalizeFast(a);

	return 1;
}

// vector.RotateAroundPoint(vector dest, vector dir, vector point, float degrees)
// Rotates point around a given vector.
// * dir vector around which to rotate (must be normalized)
// * point point to be rotated
// * degrees how many degrees to rotate the point by
// * dest point after rotation
static int Vector_RotatePointAround(lua_State *L) {
	vec_t	*dst, *dir, *point;
	vec_t	degrees;

	dst = Lua_GetVector(L, 1);
	dir = Lua_GetVector(L, 2);
	point = Lua_GetVector(L, 3);
	degrees = luaL_checknumber(L, 4);

	RotatePointAroundVector(dst, dir, point, degrees);

	return 1;
}

// vector.Perpendicular(vector dest, vector src)
// Finds a vector perpendicular to the source vector. 
// * src source vector 
// * dest a vector that is perpendicular to src (the result is stored here)
static int Vector_Perpendicular(lua_State *L) {
	vec_t	*dst, *src;

	dst = Lua_GetVector(L, 1);
	src = Lua_GetVector(L, 2);

	PerpendicularVector(dst, src);

	return 1;
}

//What does this do?
static int Vector_VecToAngles(lua_State *L) {
	vec_t *v, *t;
	
	v = Lua_GetVector(L, 1);
	t = Lua_GetVector(L, 2);

	vectoangles(v, t);

	return 1;
} 

//What does this do?
static int Vector_AngleVectors(lua_State *L) {
	vec_t *v, *fwd, *right, *up; 

	v = Lua_GetVector(L, 1);
	fwd = Lua_GetVector(L, 2);
	right = Lua_GetVector(L, 3);
	up = Lua_GetVector(L, 4);

	AngleVectors(v, fwd, right, up);

	return 1;
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

	return 1;
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
	{"New", Vector_New},
	{"Construct", Vector_Construct},
	{"Set", Vector_Set},
	{"Clear", Vector_Clear},
	{"Add", Vector_Add},
	{"Subtract", Vector_Subtract},
	{"Scale", Vector_Scale},
	{"Length", Vector_Length},
	{"Normalize", Vector_Normalize},
	{"NormalizeFast", Vector_NormalizeFast},
	{"RotatePointAround", Vector_RotatePointAround},
	{"Perpendicular", Vector_Perpendicular},
	{"VecToAngles", Vector_VecToAngles },
	{"AngleVectors", Vector_AngleVectors },
	{NULL, NULL}
};

static const luaL_Reg vector_meta[] = {
	{"__index", Vector_Index},
	{"__newindex", Vector_NewIndex},
	{"__add", Vector_AddOperator},
	{"__sub", Vector_SubOperator},
	{"__mul", Vector_DotOperator},
	{"__unm", Vector_NegateOperator},
	{"__gc", Vector_GC},
	{"__tostring", Vector_ToString},
	{NULL, NULL}
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

