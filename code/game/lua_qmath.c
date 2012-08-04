// qmath lib for lua

#include "g_lua.h"

#ifdef G_LUA
// qmath.abs(float f)
// Returns the integer part of f.
static int Qmath_Abs(lua_State * L)
{
	lua_pushnumber(L, fabs(luaL_checknumber(L, 1)));
	return 1;
}

// qmath.sin(float degree)
// Returns the sine of degree.
static int Qmath_Sin(lua_State * L)
{
	lua_pushnumber(L, sin(DEG2RAD(luaL_checknumber(L, 1))));
	return 1;
}

// qmath.cos(float degree)
// Returns the cosine of degree.
static int Qmath_Cos(lua_State * L)
{
	lua_pushnumber(L, cos(DEG2RAD(luaL_checknumber(L, 1))));
	return 1;
}

// qmath.tan(float degree)
// Returns the tangent of degree.
static int Qmath_Tan(lua_State * L)
{
	lua_pushnumber(L, tan(DEG2RAD(luaL_checknumber(L, 1))));
	return 1;
}

// qmath.asin(float f)
// Returns the arcsine of f.
static int Qmath_Asin(lua_State * L)
{
	lua_pushnumber(L, RAD2DEG(asin(luaL_checknumber(L, 1))));
	return 1;
}

// qmath.acos(float f)
// Returns the arccosine of f.
static int Qmath_Acos(lua_State * L)
{
	lua_pushnumber(L, RAD2DEG(acos(luaL_checknumber(L, 1))));
	return 1;
}

// qmath.atan(float f)
// Returns the arctangent of f.
static int Qmath_Atan(lua_State * L)
{
	lua_pushnumber(L, RAD2DEG(atan(luaL_checknumber(L, 1))));
	return 1;
}

// qmath.atan2
static int Qmath_Atan2(lua_State * L)
{
	lua_pushnumber(L, RAD2DEG(atan2(luaL_checknumber(L, 1), luaL_checknumber(L, 2))));
	return 1;
}

// qmath.ceil(float f)
// Returns the ceiled value of f.
static int Qmath_Ceil(lua_State * L)
{
	lua_pushnumber(L, ceil(luaL_checknumber(L, 1)));
	return 1;
}

// qmath.floor(float f)
// Returns the floored value of f.
static int Qmath_Floor(lua_State * L)
{
	lua_pushnumber(L, floor(luaL_checknumber(L, 1)));
	return 1;
}

// qmath.fmod(float f, float n)
// Returns the remainder of f=n.
static int Qmath_Fmod(lua_State * L)
{
	lua_pushnumber(L, fmod(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
	return 1;
}

// qmath.modf(float f)
// Breaks f apart into its integer part and its fractional part.
// The fractional part is returned while the integer part is assigned to f.
static int Qmath_Modf(lua_State * L)
{
	double          ip;
	double          fp = modf(luaL_checknumber(L, 1), &ip);

	lua_pushnumber(L, ip);
	lua_pushnumber(L, fp);
	return 2;
}

// qmath.sqrt(float f)
// Returns the square root of f.
static int Qmath_Sqrt(lua_State * L)
{
	lua_pushnumber(L, sqrt(luaL_checknumber(L, 1)));
	return 1;
}

// qmath.pow(float f, float n)
// What's this??
static int Qmath_Pow(lua_State * L)
{
	lua_pushnumber(L, pow(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
	return 1;
}

// qmath.log(float f)
// Returns logarithm of f.
static int Qmath_Log(lua_State * L)
{
	lua_pushnumber(L, log(luaL_checknumber(L, 1)));
	return 1;
}

// qmath.log10(float f)
// Returns logarithm to base 10 of f.
static int Qmath_Log10(lua_State * L)
{
	lua_pushnumber(L, log10(luaL_checknumber(L, 1)));
	return 1;
}

// qmath.exp(float number)
// What's this??
static int Qmath_Exp(lua_State * L)
{
	lua_pushnumber(L, exp(luaL_checknumber(L, 1)));
	return 1;
}

// qmath.deg(float radian)
// Converts radian to degree.
static int Qmath_Deg(lua_State * L)
{
	lua_pushnumber(L, RAD2DEG(luaL_checknumber(L, 1)));
	return 1;
}

// qmath.rad(float dgree)
// Converts degree to radian.
static int Qmath_Rad(lua_State * L)
{
	lua_pushnumber(L, DEG2RAD(luaL_checknumber(L, 1)));
	return 1;
}

// qmath.frexp(float f)
// Breaks f into its binary significant and an integral exponent for 2.
// x = significant 2exponent
static int Qmath_Frexp(lua_State * L)
{
	int             e;

	lua_pushnumber(L, frexp(luaL_checknumber(L, 1), &e));
	lua_pushnumber(L, e);
	return 2;
}

// qmath.ldexp(float f, float n)
// Returns the result from multiplying f by 2 raised to the power of n.
static int Qmath_Ldexp(lua_State * L)
{
	lua_pushnumber(L, ldexp(luaL_checknumber(L, 1), luaL_checkint(L, 2)));
	return 1;
}


// qmath.min(int array[])
// Return the lowest value in array[].
static int Qmath_Min(lua_State * L)
{
	int             n = lua_gettop(L);	/* number of arguments */
	lua_Number      dmin = luaL_checknumber(L, 1);
	int             i;

	for(i = 2; i <= n; i++)
	{
		lua_Number      d = luaL_checknumber(L, i);

		if(d < dmin)
			dmin = d;
	}
	lua_pushnumber(L, dmin);
	return 1;
}

// qmath.max(int array[])
// Return the highest value in array[].
static int Qmath_Max(lua_State * L)
{
	int             n = lua_gettop(L);	/* number of arguments */
	lua_Number      dmax = luaL_checknumber(L, 1);
	int             i;

	for(i = 2; i <= n; i++)
	{
		lua_Number      d = luaL_checknumber(L, i);

		if(d > dmax)
			dmax = d;
	}
	lua_pushnumber(L, dmax);
	return 1;
}

// qmath.rand()
// Returns a random integer.
static int Qmath_Rand(lua_State * L)
{
	lua_pushinteger(L, rand());
	return 1;
}

// qmath.random()
// Returns a random float.
static int Qmath_Random(lua_State * L)
{
	lua_pushnumber(L, random());
	return 1;
}

// qmath.crandom()
// Returns random floats (crazy random function).
static int Qmath_Crandom(lua_State * L)
{
	lua_pushnumber(L, crandom());
	return 1;
}

static const luaL_Reg lib_qmath[] = {
	{"abs", Qmath_Abs},
	{"sin", Qmath_Sin},
	{"cos", Qmath_Cos},
	{"tan", Qmath_Tan},
	{"asin", Qmath_Asin},
	{"acos", Qmath_Acos},
	{"atan", Qmath_Atan},
	{"atan2", Qmath_Atan2},
	{"ceil", Qmath_Ceil},
	{"floor", Qmath_Floor},
	{"fmod", Qmath_Fmod},
	{"modf", Qmath_Modf},
	{"frexp", Qmath_Frexp},
	{"ldexp", Qmath_Ldexp},
	{"sqrt", Qmath_Sqrt},
	{"min", Qmath_Min},
	{"max", Qmath_Max},
	{"log", Qmath_Log},
	{"log10", Qmath_Log10},
	{"exp", Qmath_Exp},
	{"deg", Qmath_Deg},
	{"pow", Qmath_Pow},
	{"rad", Qmath_Rad},
	{"rand", Qmath_Rand},
	{"random", Qmath_Random},
	{"crandom", Qmath_Crandom},
	{NULL, NULL}
};

int Luaopen_Qmath(lua_State * L)
{
	luaL_register(L, "qmath", lib_qmath);
	lua_pushnumber(L, M_PI);
	lua_setfield(L, -2, "pi");
	lua_pushnumber(L, HUGE_VAL);
	lua_setfield(L, -2, "huge");
	return 1;
}
#endif
