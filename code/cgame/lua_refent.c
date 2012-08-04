#include "cg_lua.h"

#ifdef CG_LUA
#include "cg_lua.h"

#ifdef CG_LUA
static int Refent_GC(lua_State * L)
{

	return 0;
}

static int Refent_ToString(lua_State * L)
{
	rent_t		 *rent;
	refEntity_t  *ent;
	char            buf[MAX_STRING_CHARS];

	rent = Lua_GetRent(L, 1);
	ent = rent->r;
	Com_sprintf(buf, sizeof(buf), "centity: pointer=%p\n", ent);
	lua_pushstring(L, buf);

	return 1;
}

static int Refent_GetRenderfx(lua_State *L) {
	rent_t *rent;

	rent = Lua_GetRent(L, 1);
	lua_pushinteger(L, rent->r->renderfx);

	return 1;
}

static int Refent_SetRenderfx(lua_State *L) {
	rent_t *rent;
	int renderfx;

	rent = Lua_GetRent(L, 1);
	renderfx = (int)luaL_checknumber(L, 2);

	rent->r->renderfx = renderfx;

	return 1;
}

static int Refent_GetType(lua_State *L) {
	rent_t	*rent;
	
	rent = Lua_GetRent(L, 1);
	lua_pushinteger(L, rent->r->reType);

	return 1;
}

static int Refent_SetType(lua_State *L) {
	rent_t *rent;
	refEntityType_t type;

	rent = Lua_GetRent(L, 1);
	type = (refEntityType_t)((int)luaL_checknumber(L, 2));

	if(type < 0 || type >= RT_MAX_REF_ENTITY_TYPE)
		return 1;

	return 1;
}

static int Refent_GetHmodel(lua_State *L) {
	rent_t *rent;

	rent = Lua_GetRent(L, 1);
	lua_pushinteger(L, rent->r->hModel);

	return 1;
}

static int Refent_SetHmodel(lua_State *L) {
	rent_t *rent;
	int		hmodel;

	rent = Lua_GetRent(L, 1);
	hmodel = (int)luaL_checknumber(L, 2);

	rent->r->hModel = hmodel;

	return 1;
}

static int Refent_GetLightingOrigin(lua_State *L) {
	rent_t *rent;

	rent = Lua_GetRent(L, 1);
	Lua_PushVector(L, rent->r->lightingOrigin);

	return 1;
}

static int Refent_SetLightingOrigin(lua_State *L) {
	rent_t *rent;
	vec_t *origin;

	rent = Lua_GetRent(L, 1);
	origin = Lua_GetVector(L, 2);
	
	VectorCopy(origin, rent->r->lightingOrigin);

	return 1;
}

static int Refent_GetShadowPlane(lua_State *L) {
	rent_t *rent;

	rent = Lua_GetRent(L, 1);
	lua_pushnumber(L, rent->r->shadowPlane);

	return 1;
}

static int Refent_SetShadowPlane(lua_State *L) {
	rent_t *rent;
	float sp;

	rent = Lua_GetRent(L, 1);
	sp = (float)luaL_checknumber(L, 2);

	rent->r->shadowPlane = sp;

	return 1;
}

static int Refent_GetAxis0(lua_State *L) {
	rent_t *rent;

	rent = Lua_GetRent(L, 1);
	Lua_PushVector(L, rent->r->axis[0]);

	return 1;
}

static int Refent_SetAxis0(lua_State *L) {
	rent_t *rent;
	vec_t *axis;

	rent = Lua_GetRent(L, 1);
	axis = Lua_GetVector(L, 2);

	VectorCopy(axis, rent->r->axis[0]);

	return 1;
}

static int Refent_GetAxis1(lua_State *L) {
	rent_t *rent;

	rent = Lua_GetRent(L, 1);
	Lua_PushVector(L, rent->r->axis[1]);

	return 1;
}

static int Refent_SetAxis1(lua_State *L) {
	rent_t *rent;
	vec_t *axis;

	rent = Lua_GetRent(L, 1);
	axis = Lua_GetVector(L, 2);

	VectorCopy(axis, rent->r->axis[1]);

	return 1;
}

static int Refent_GetAxis2(lua_State *L) {
	rent_t *rent;

	rent = Lua_GetRent(L, 1);
	Lua_PushVector(L, rent->r->axis[2]);

	return 1;
}

static int Refent_SetAxis2(lua_State *L) {
	rent_t *rent;
	vec_t *axis;

	rent = Lua_GetRent(L, 1);
	axis = Lua_GetVector(L, 2);

	VectorCopy(axis, rent->r->axis[2]);

	return 1;
}

static int Refent_UseNormAxis(lua_State *L) {
	rent_t *rent;

	rent = Lua_GetRent(L, 1);
	lua_pushboolean(L, (int)(!(rent->r->nonNormalizedAxes)));

	return 1;
}

static int Refent_SetUseNormAxis(lua_State *L) {
	rent_t *rent;
	qboolean b;

	rent = Lua_GetRent(L, 1);
	b = (qboolean)lua_toboolean(L, 2);

	rent->r->nonNormalizedAxes = (qboolean)(!b);

	return 1;
}

static int Refent_GetOrigin(lua_State *L) {
	rent_t *rent;

	rent = Lua_GetRent(L, 1);
	Lua_PushVector(L, rent->r->origin);

	return 1;
}

static int Refent_SetOrigin(lua_State *L) {
	rent_t  *rent;
	vec_t	*origin;

	rent = Lua_GetRent(L, 1);
	origin = Lua_GetVector(L, 1);

	VectorCopy(origin, rent->r->origin);

	return 1;
}

static int Refent_GetFrame(lua_State *L) {
	rent_t *rent;

	rent = Lua_GetRent(L, 1);
	lua_pushinteger(L, rent->r->frame);

	return 1;
}

static int Refent_SetFrame(lua_State *L) {
	rent_t *rent;
	int frame;

	rent = Lua_GetRent(L, 1);
	frame = (int)luaL_checknumber(L, 2);

	rent->r->frame = frame;

	return 1;
}

static int Refent_GetOldOrigin(lua_State *L) {
	rent_t *rent;

	rent = Lua_GetRent(L, 1);
	Lua_PushVector(L, rent->r->oldorigin);

	return 1;
}

static int Refent_SetOldOrigin(lua_State *L) {
	rent_t *rent;
	vec_t  *vec;

	rent = Lua_GetRent(L, 1);
	vec = Lua_GetVector(L, 2);

	VectorCopy(vec, rent->r->oldorigin);

	return 1;
}

static int Refent_GetOldFrame(lua_State *L) {
	rent_t *rent;

	rent = Lua_GetRent(L, 1);
	lua_pushinteger(L, rent->r->oldframe);

	return 1;
}

static int Refent_SetOldFrame(lua_State *L) {
	rent_t *rent;
	int of;

	rent = Lua_GetRent(L, 1);
	of = (int)luaL_checknumber(L, 2);

	rent->r->oldframe = of;

	return 1;
}

static int Refent_GetBacklerp(lua_State *L) {
	rent_t *rent;

	rent = Lua_GetRent(L, 1);
	lua_pushnumber(L, rent->r->backlerp);

	return 1;
}

static int Refent_SetBacklerp(lua_State *L) {
	rent_t *rent;
	float bl;

	rent = Lua_GetRent(L, 1);
	bl = (float)luaL_checknumber(L, 2);

	rent->r->backlerp = bl;

	return 1;
}

static int Refent_GetSkinNum(lua_State *L) {
	rent_t *rent;

	rent = Lua_GetRent(L, 1);
	lua_pushinteger(L, rent->r->skinNum);

	return 1;
}

static int Refent_SetSkinNum(lua_State *L) {
	rent_t *rent;
	int		sn;

	rent = Lua_GetRent(L, 1);
	sn = (int)luaL_checknumber(L, 2);

	rent->r->skinNum = sn;

	return 1;
}

static int Refent_GetCustomSkin(lua_State *L) {
	rent_t *rent;

	rent = Lua_GetRent(L, 1);
	lua_pushinteger(L, rent->r->customSkin);

	return 1;
}

static int Refent_SetCustomSkin(lua_State *L) {
	rent_t *rent;
	int		cs;

	rent = Lua_GetRent(L, 1);
	cs = (int)luaL_checknumber(L, 2);

	rent->r->customSkin = cs;

	return 1;
}

static int Refent_GetCustomShader(lua_State *L) {
	rent_t *rent;

	rent = Lua_GetRent(L, 1);
	lua_pushinteger(L, rent->r->customShader);

	return 1;
}

static int Refent_SetCustomShader(lua_State *L) {
	rent_t *rent;
	int		cs;

	rent = Lua_GetRent(L, 1);
	cs = (int)luaL_checknumber(L, 2);

	rent->r->customShader = cs;

	return 1;
}


static const luaL_Reg Refentity_ctor[] = {
	{NULL,						NULL}
};

static const luaL_Reg Refentity_meta[] = {
	{"__gc",						Refent_GC},
	{"__tostring",					Refent_ToString},
	{"GetType",						Refent_GetType},
	{"SetType",						Refent_SetType},
	{"GetRenderfx",					Refent_GetRenderfx},
	{"SetRenderfx",					Refent_SetRenderfx},
	{"GetHmodel",					Refent_GetHmodel},
	{"SetHmodel",					Refent_SetHmodel},
	{"GetLightingOrigin",			Refent_GetLightingOrigin},
	{"SetLightingOrigin",			Refent_SetLightingOrigin},
	{"GetShadowPlane",				Refent_GetShadowPlane},
	{"SetShadowPlane",				Refent_SetShadowPlane},
	{"GetAxis0",					Refent_GetAxis0},
	{"SetAxis0",					Refent_SetAxis0},
	{"GetAxis1",					Refent_GetAxis1},
	{"SetAxis1",					Refent_SetAxis1},
	{"GetAxis2",					Refent_GetAxis2},
	{"SetAxis2",					Refent_SetAxis2},
	{"UseNormalizedAxis",			Refent_UseNormAxis},
	{"SetUseNormalizedAxis",		Refent_SetUseNormAxis},
	{"GetOrigin",					Refent_GetOrigin},
	{"GetBeamFrom",					Refent_GetOrigin},
	{"SetOrigin",					Refent_SetOrigin},
	{"SetBeamFrom",					Refent_SetOrigin},
	{"GetFrame",					Refent_GetFrame},
	{"GetModelBeamDiameter",		Refent_GetFrame},
	{"SetFrame",					Refent_SetFrame},
	{"SetModelBeamDiameter",		Refent_SetFrame},
	{"GetOldOrigin",				Refent_GetOldOrigin},
	{"GetModelBeamTo",				Refent_GetOldOrigin},
	{"SetOldOrigin",				Refent_SetOldOrigin},
	{"SetModelBeamTo",				Refent_SetOldOrigin},
	{"GetOldFrame",					Refent_GetOldFrame},
	{"SetOldFrame",					Refent_SetOldFrame},
	{"GetBacklerp",					Refent_GetBacklerp},
	{"SetBacklerp",					Refent_SetBacklerp},
	{"GetSkinNum",					Refent_GetSkinNum},
	{"SetSkinNum",					Refent_SetSkinNum},
	{"GetCustomSkin",				Refent_GetCustomSkin},
	{"SetCustomSkin",				Refent_SetCustomSkin},
	{"GetCustomShader",				Refent_GetCustomShader},
	{"SetCustomShader",				Refent_SetCustomShader},

	{NULL, NULL}
};

int Luaopen_Rent(lua_State * L)
{
	luaL_newmetatable(L, "cgame.refentity");

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	luaL_register(L, NULL, Refentity_meta);
	luaL_register(L, "refentity", Refentity_ctor);

	return 1;
}

void Lua_PushRent(lua_State * L, refEntity_t * rent)
{
	rent_t     *refent;

	if(!rent)
		lua_pushnil(L);
	else {
		refent = (rent_t *)lua_newuserdata(L, sizeof(cent_t));
		luaL_getmetatable(L, "cgame.refentity");
		lua_setmetatable(L, -2);
		refent->r = rent;
	}
}

rent_t *Lua_GetRent(lua_State * L, int argNum)
{
	void           *ud;

	ud = luaL_checkudata(L, argNum, "cgame.refentity");
	luaL_argcheck(L, ud != NULL, argNum, "\'refentity\' expected");
	return (rent_t *) ud;
}
#endif

#endif
