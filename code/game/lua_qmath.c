// qmath lib for lua

#include "g_lua.h"

#ifdef G_LUA
/***
Provides access to many mathematical functions.
@module qmath
*/

/***
Get the integer part of a number.
@function abs
@param number Number
@return Integer part of number.
*/
static int Qmath_Abs(lua_State * L)
{
	lua_pushnumber(L, fabs(luaL_checknumber(L, 1)));
	return 1;
}

/***
Get the sine for a number (degree).
@function sin
@param number Number
@return Sine of number.
*/
static int Qmath_Sin(lua_State * L)
{
	lua_pushnumber(L, sin(DEG2RAD(luaL_checknumber(L, 1))));
	return 1;
}

/***
Get the cosine for a number (degree).
@function cos
@param number Number
@return Consine of number.
*/
static int Qmath_Cos(lua_State * L)
{
	lua_pushnumber(L, cos(DEG2RAD(luaL_checknumber(L, 1))));
	return 1;
}

/***
Get the tangent for a number.
@function tan
@param number Number
@return Tangent of number.
*/
static int Qmath_Tan(lua_State * L)
{
	lua_pushnumber(L, tan(DEG2RAD(luaL_checknumber(L, 1))));
	return 1;
}

/***
Get the arcsine for a number.
@function asin
@param number Number
@return arcsine for number.
*/
static int Qmath_Asin(lua_State * L)
{
	lua_pushnumber(L, RAD2DEG(asin(luaL_checknumber(L, 1))));
	return 1;
}

/***
Get the arccosine for a number.
@function acos
@param number Number
@return arccosine of number.
*/
static int Qmath_Acos(lua_State * L)
{
	lua_pushnumber(L, RAD2DEG(acos(luaL_checknumber(L, 1))));
	return 1;
}

/***
Get the arctangent for a number.
@function atan
@param number Number
@return arctangent of number.
*/
static int Qmath_Atan(lua_State * L)
{
	lua_pushnumber(L, RAD2DEG(atan(luaL_checknumber(L, 1))));
	return 1;
}

/***
Get the arctangent for a number. Alternate to atan.
@function atan2
@param number Number
@return arctangent of number.
*/
static int Qmath_Atan2(lua_State * L)
{
	lua_pushnumber(L, RAD2DEG(atan2(luaL_checknumber(L, 1), luaL_checknumber(L, 2))));
	return 1;
}

/***
Get the ceiled value of a number.
@function ceil
@param number Number
@return Ceiled value of number.
*/
static int Qmath_Ceil(lua_State * L)
{
	lua_pushnumber(L, ceil(luaL_checknumber(L, 1)));
	return 1;
}

/***
Get the floored value of a number.
@function floor
@param numerb Number
@return Floored value of number.
*/
static int Qmath_Floor(lua_State * L)
{
	lua_pushnumber(L, floor(luaL_checknumber(L, 1)));
	return 1;
}

/***
Get the remainder of numerator/denominator.
@functiom fmod
@param n numerator
@param f denominator
@return Remainder of numerator/denominator.
*/
static int Qmath_Fmod(lua_State * L)
{
	lua_pushnumber(L, fmod(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
	return 1;
}

/***
BUGGED
Breaks a given number apart into its integer part and its fractional part. The fractional part is returned while the integer part is assigned to number.
@function modf
@param number Number
@return Array containing integer and fractional part.
*/
/* TODO return an array the current way won't work */
static int Qmath_Modf(lua_State * L)
{
	double          ip;
	double          fp = modf(luaL_checknumber(L, 1), &ip);

	lua_pushnumber(L, ip);
	lua_pushnumber(L, fp);
	return 2;
}

/***
Get the square root of a number.
@function sqrt
@param number Number
@return Square root of number.
*/
static int Qmath_Sqrt(lua_State * L)
{
	lua_pushnumber(L, sqrt(luaL_checknumber(L, 1)));
	return 1;
}

// What's this?? f^n e.g. 2^4 = 2 * 2 * 2 = 8
/***
Returns f raised to the power of n.
@function pow
@param f Number
@param n Number
@return f raised to the power of n.
*/
static int Qmath_Pow(lua_State * L)
{
	lua_pushnumber(L, pow(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
	return 1;
}

/***
Get the logarithm of a number.
@function log
@param number Number
@return Logarithm of number.
*/
static int Qmath_Log(lua_State * L)
{
	lua_pushnumber(L, log(luaL_checknumber(L, 1)));
	return 1;
}

/***
Get logarithm to the base of 10 of number.
@function log10
@param number Number
@return Logarithm to the base of 10 of number.
*/
static int Qmath_Log10(lua_State * L)
{
	lua_pushnumber(L, log10(luaL_checknumber(L, 1)));
	return 1;
}

/***
Get the base-e exponential function of a given number.
@function exp
@param number Number
@return Base-e exponential of number.
*/
static int Qmath_Exp(lua_State * L)
{
	lua_pushnumber(L, exp(luaL_checknumber(L, 1)));
	return 1;
}

/***
Convert radian to degree.
@function deg
@param number Number
@return number converted to degree.
*/
static int Qmath_Deg(lua_State * L)
{
	lua_pushnumber(L, RAD2DEG(luaL_checknumber(L, 1)));
	return 1;
}

/***
Convert degree to radian.
@function rad
@param number Number
@return number converted to radian.
*/
static int Qmath_Rad(lua_State * L)
{
	lua_pushnumber(L, DEG2RAD(luaL_checknumber(L, 1)));
	return 1;
}

/***
BUGGED
Breaks a number into its binary significant and an integral exponent for 2.
@function frexp
@param number
@return Array containing binary significant and integral exponent for 2.
*/
/* TODO return array */
static int Qmath_Frexp(lua_State * L)
{
	int             e;

	lua_pushnumber(L, frexp(luaL_checknumber(L, 1), &e));
	lua_pushnumber(L, e);
	return 2;
}

/***
Get the result from multiplying f by 2 raised to the power of n.
@function ldexp
@param f Number
@param n Number
@return The result from multiplying f by 2 raised to the power of n.
*/
static int Qmath_Ldexp(lua_State * L)
{
	lua_pushnumber(L, ldexp(luaL_checknumber(L, 1), luaL_checkint(L, 2)));
	return 1;
}


/***
Get the lowest value in array[].
@function min
@param array Array of numbers.
@return the lowest value in the array.
*/
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

/***
Get the highest value in array[].
@function min
@param array Array of numbers.
@return the highest value in the array.
*/
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

/***
Get a random integer.
@function rand
@return Random integer.
*/
static int Qmath_Rand(lua_State * L)
{
	lua_pushinteger(L, rand());
	return 1;
}

/***
Get a random floating point number.
@function random
@return Random floating point number.
*/
static int Qmath_Random(lua_State * L)
{
	lua_pushnumber(L, random());
	return 1;
}

/***
Get a random floating point number (using crazy random function).
@function crandom
@return A random floating point number (using crazy random function).
*/
static int Qmath_Crandom(lua_State * L)
{
	lua_pushnumber(L, crandom());
	return 1;
}

/***
Get a random integer from the range of integers defined by and including i and j.
@function irandom
@param i Number, lower limit.
@param j Number, upper limit.
@return A random integer from the range of integers defined by and including i and j.
*/
static int Qmath_Irandom(lua_State * L)
{
	lua_pushnumber(L, irandom(luaL_checkint(L, 1), luaL_checkint(L, 2)));
	return 1;
}

/***
Get a random float from the range of floats defined by and including i and j.
@function flrandom
@param i Number, lower limit.
@param j Number, upper limit.
@return A random float from the range of floats defined by and including i and j.
*/
static int Qmath_FLrandom(lua_State * L)
{
	lua_pushnumber(L, flrandom(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
	return 1;
}

static const luaL_Reg lib_qmath[] = {
	{ "abs",		Qmath_Abs		},
	{ "sin",		Qmath_Sin		},
	{ "cos",		Qmath_Cos		},
	{ "tan",		Qmath_Tan		},
	{ "asin",		Qmath_Asin		},
	{ "acos",		Qmath_Acos		},
	{ "atan",		Qmath_Atan		},
	{ "atan2",		Qmath_Atan2		},
	{ "ceil",		Qmath_Ceil		},
	{ "floor",		Qmath_Floor		},
	{ "fmod",		Qmath_Fmod		},
	{ "modf",		Qmath_Modf		},
	{ "frexp",		Qmath_Frexp		},
	{ "ldexp",		Qmath_Ldexp		},
	{ "sqrt",		Qmath_Sqrt		},
	{ "min",		Qmath_Min		},
	{ "max",		Qmath_Max		},
	{ "log",		Qmath_Log		},
	{ "log10",		Qmath_Log10		},
	{ "exp",		Qmath_Exp		},
	{ "deg",		Qmath_Deg		},
	{ "pow",		Qmath_Pow		},
	{ "rad",		Qmath_Rad		},
	{ "rand",		Qmath_Rand		},
	{ "candom",		Qmath_Crandom	},
	{ "random",		Qmath_Random	},
	{ "irandom",	Qmath_Irandom	},
	{ "flrandom",	Qmath_FLrandom	},
	{ NULL,			NULL			}
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
