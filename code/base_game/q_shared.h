// Copyright (C) 1999-2000 Id Software, Inc.
//
#ifndef __Q_SHARED_H
#define __Q_SHARED_H

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

// q_shared.h -- included first by ALL program modules.
// A user mod should never modify this file

#if defined(ppc) || defined(__ppc) || defined(__ppc__)
#define idppc
#endif

//#define MISSION_PACK //TiM: was commented out

#ifdef _MSC_VER
#pragma warning(disable : 4018)     // signed/unsigned mismatch
#pragma warning(disable : 4032)
#pragma warning(disable : 4051)
#pragma warning(disable : 4057)		// slightly different base types
#pragma warning(disable : 4100)		// unreferenced formal parameter
#pragma warning(disable : 4115)
#pragma warning(disable : 4125)		// decimal digit terminates octal escape sequence
#pragma warning(disable : 4127)		// conditional expression is constant
#pragma warning(disable : 4136)
#pragma warning(disable : 4201)
#pragma warning(disable : 4214)
#pragma warning(disable : 4244)
#pragma warning(disable : 4305)		// truncation from const double to float
#pragma warning(disable : 4310)		// cast truncates constant value
#pragma warning(disable : 4514)
#pragma warning(disable : 4711)		// selected for automatic inline expansion
#pragma warning(disable : 4220)		// varargs matches remaining parameters

#define _CRT_SECURE_NO_WARNINGS

#endif

//Ignore __attribute__ on non-gcc platforms
#ifndef __GNUC__
#ifndef __attribute__
#define __attribute__(x)
#endif
#endif

#if (defined _MSC_VER)
#define Q_EXPORT __declspec(dllexport)
#elif (defined __SUNPRO_C)
#define Q_EXPORT __global
#elif ((__GNUC__ >= 3) && (!__EMX__) && (!sun))
#define Q_EXPORT __attribute__((visibility("default")))
#else
#define Q_EXPORT
#endif

/**********************************************************************
  VM Considerations

  The VM can not use the standard system headers because we aren't really
  using the compiler they were meant for.  We use bg_lib.h which contains
  prototypes for the functions we define for our own use in bg_lib.c.

  When writing mods, please add needed headers HERE, do not start including
  stuff like <stdio.h> in the various .c files that make up each of the VMs
  since you will be including system headers files can will have issues.

  Remember, if you use a C library function that is not defined in bg_lib.c,
  you will have to add your own version for support in the VM.

 **********************************************************************/

// meh somehow preprocessor G_LUA won't work for me
#define G_LUA 1
#define CG_LUA 1

#ifdef Q3_VM
#error "Q3_VM is defined - QVMs are no longer supported by RPG-X2"

#include "bg_lib.h"

#else

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>

#endif

#ifdef _WIN32

//#pragma intrinsic( memset, memcpy )

#endif

// Needed for mods compiled in 64 bit shared objects.
#ifdef Q3_VM
#error "Q3_VM is defined - QVMs are no longer supported by RPG-X2"
	typedef int intptr_t;
#else
	#ifdef _MSC_VER
		#include <stddef.h>
		#define __STDC_FORMAT_MACROS
		#include <inttypes.h>
	#else
		#define __STDC_FORMAT_MACROS
		#include <inttypes.h>
	#endif
#endif

// this is the define for determining if we have an asm version of a C function
#if (defined _M_IX86 || defined __i386__) && !defined __sun__  && !defined __LCC__
#define id386	1
#else
#define id386	0
#endif

// for windows fastcall option

#define	QDECL

//======================= WIN32 DEFINES =================================

#ifdef WIN32

#define	MAC_STATIC

#undef QDECL
#define	QDECL	__cdecl

// buildstring will be incorporated into the version string
#ifdef NDEBUG
#ifdef _M_IX86
#define	CPUSTRING	"win-x86"
#elif defined _M_ALPHA
#define	CPUSTRING	"win-AXP"
#endif
#else
#ifdef _M_IX86
#define	CPUSTRING	"win-x86-debug"
#elif defined _M_ALPHA
#define	CPUSTRING	"win-AXP-debug"
#endif
#endif


#define	PATH_SEP '\\'

#endif

//======================= MAC OS X SERVER DEFINES =====================

#if defined(__MACH__) && defined(__APPLE__)

#define MAC_STATIC

#ifdef __ppc__
#define CPUSTRING	"MacOSXS-ppc"
#elif defined __i386__
#define CPUSTRING	"MacOSXS-i386"
#else
#define CPUSTRING	"MacOSXS-other"
#endif

#define	PATH_SEP	'/'

#define	GAME_HARD_LINKED
#define	CGAME_HARD_LINKED
#define	UI_HARD_LINKED
#define	BOTLIB_HARD_LINKED

#endif

//======================= MAC DEFINES =================================

#ifdef __MACOS__

#define	MAC_STATIC	static

#define	CPUSTRING	"MacOS-PPC"

#define	PATH_SEP ':'

#define	GAME_HARD_LINKED
#define	CGAME_HARD_LINKED
#define	UI_HARD_LINKED
#define	BOTLIB_HARD_LINKED

void Sys_PumpEvents( void );

#endif

//======================= LINUX DEFINES =================================

// the mac compiler can't handle >32k of locals, so we
// just waste space and make big arrays static...
#ifdef __linux__

#define	MAC_STATIC

#ifdef __i386__
#define	CPUSTRING	"linux-i386"
#elif defined __axp__
#define	CPUSTRING	"linux-alpha"
#else
#define	CPUSTRING	"linux-other"
#endif

#define	PATH_SEP '/'

#endif

#if !defined(UNUSED)
#define UNUSED(x)	((void)(x))
#endif

#define VARNAME(varname) #varname

#define SOUND_DIR		"sound/weapons/"

//=============================================================

typedef uint8_t	byte;

typedef enum {qfalse, qtrue}	qboolean;

typedef int32_t	qhandle_t;
typedef int32_t sfxHandle_t;
typedef int32_t fileHandle_t;
typedef int32_t	clipHandle_t;


#ifndef NULL
#define NULL ((void *)0)
#endif

#define	MAX_QINT			0x7fffffff
#define	MIN_QINT			(-MAX_QINT-1)


// angle indexes
#define	PITCH				0		// up / down
#define	YAW					1		// left / right
#define	ROLL				2		// fall over

// the game guarantees that no string from the network will ever
// exceed MAX_STRING_CHARS
#define	MAX_STRING_CHARS	1024	// max length of a string passed to Cmd_TokenizeString
#define	MAX_STRING_TOKENS	256		// max tokens resulting from Cmd_TokenizeString
#define	MAX_TOKEN_CHARS		1024	// max length of an individual token

#define	MAX_INFO_STRING		1024
#define	MAX_INFO_KEY		1024
#define	MAX_INFO_VALUE		1024

#define	BIG_INFO_STRING		8192  // used for system info key only
#define	BIG_INFO_KEY		8192
#define	BIG_INFO_VALUE		8192


#define	MAX_QPATH			64		// max length of a quake game pathname
#define	MAX_OSPATH			128		// max length of a filesystem pathname

#define	MAX_NAME_LENGTH		32		// max length of a client name

typedef int32_t	pclass_t;

// paramters for command buffer stuffing
typedef enum {
	EXEC_NOW,			// don't return until completed, a VM should NEVER use this,
						// because some commands might cause the VM to be unloaded...
	EXEC_INSERT,		// insert at current position, but don't run yet
	EXEC_APPEND			// add to end of the command buffer (normal case)
} cbufExec_t;


//
// these aren't needed by any of the VMs.  put in another header?
//
#define	MAX_MAP_AREA_BYTES		32		// bit vector of area visibility


// print levels from renderer (FIXME: set up for game / cgame?)
typedef enum {
	PRINT_ALL,
	PRINT_DEVELOPER,		// only print when "developer 1"
	PRINT_WARNING,
	PRINT_ERROR
} printParm_t;


#ifdef ERR_FATAL
#undef ERR_FATAL			// this is be defined in malloc.h
#endif

// parameters to the main Error routine
typedef enum {
	ERR_FATAL,					// exit the entire game with a popup window
	ERR_DROP,					// print to console and disconnect from game
	ERR_SERVERDISCONNECT,		// don't kill server
	ERR_DISCONNECT,				// client disconnected from the server
	ERR_NEED_CD					// pop up the need-cd dialog
} errorParm_t;


// font rendering values used by ui and cgame
#define PROP_GAP_WIDTH			2
#define PROP_SPACE_WIDTH		4
#define PROP_HEIGHT				16

#define PROP_TINY_SIZE_SCALE	1
#define PROP_SMALL_SIZE_SCALE	1
#define PROP_BIG_SIZE_SCALE		1
#define PROP_GIANT_SIZE_SCALE	2

#define PROP_TINY_HEIGHT		11 //10
#define PROP_GAP_TINY_WIDTH		1 //1
#define PROP_SPACE_TINY_WIDTH	3

#define PROP_BIG_HEIGHT			24 //TiM: 24
#define PROP_GAP_BIG_WIDTH		3
#define PROP_SPACE_BIG_WIDTH	6


#define BLINK_DIVISOR			600
#define PULSE_DIVISOR			75

#define UI_LEFT			0x00000000	// default
#define UI_CENTER		0x00000001
#define UI_RIGHT		0x00000002
#define UI_FORMATMASK	0x00000007
#define UI_BOTTOM		0x00000008	//TiM : Y = the bottom of the line
#define UI_SMALLFONT	0x00000010
#define UI_BIGFONT		0x00000020	// default
#define UI_GIANTFONT	0x00000040
#define UI_DROPSHADOW	0x00000800
#define UI_BLINK		0x00001000
#define UI_INVERSE		0x00002000
#define UI_PULSE		0x00004000
#define UI_UNDERLINE	0x00008000
#define UI_TINYFONT		0x00010000
#define UI_SHOWCOLOR	0x00020000
#define UI_FORCECOLOR	0x00040000


/*
==============================================================

MATHLIB

==============================================================
*/


typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

typedef	int32_t	fixed4_t;
typedef	int32_t	fixed8_t;
typedef	int32_t	fixed16_t;

#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

#define NUMVERTEXNORMALS	162
extern	vec3_t	bytedirs[NUMVERTEXNORMALS];

// all drawing is done to a 640*480 virtual screen size
// and will be automatically scaled to the real resolution
#define	SCREEN_WIDTH		640
#define	SCREEN_HEIGHT		480

#define TINYCHAR_WIDTH		(SMALLCHAR_WIDTH)
#define TINYCHAR_HEIGHT		(SMALLCHAR_HEIGHT/2)

#define SMALLCHAR_WIDTH		8
#define SMALLCHAR_HEIGHT	16

#define BIGCHAR_WIDTH		16
#define BIGCHAR_HEIGHT		16

#define	GIANTCHAR_WIDTH		32
#define	GIANTCHAR_HEIGHT	48

typedef enum
{
CT_NONE,
CT_BLACK,
CT_RED,
CT_GREEN,
CT_BLUE,
CT_YELLOW,
CT_MAGENTA,
CT_CYAN,
CT_TEAL,
CT_GOLD,
CT_WHITE,
CT_LTGREY,
CT_MDGREY,
CT_DKGREY,
CT_DKGREY2,

CT_VLTORANGE,
CT_LTORANGE,
CT_DKORANGE,
CT_VDKORANGE,

CT_VLTBLUE1,
CT_LTBLUE1,
CT_DKBLUE1,
CT_VDKBLUE1,

CT_VLTBLUE2,
CT_LTBLUE2,
CT_DKBLUE2,
CT_VDKBLUE2,

CT_VLTBROWN1,
CT_LTBROWN1,
CT_DKBROWN1,
CT_VDKBROWN1,

CT_VLTGOLD1,
CT_LTGOLD1,
CT_DKGOLD1,
CT_VDKGOLD1,

CT_VLTPURPLE1,
CT_LTPURPLE1,
CT_DKPURPLE1,
CT_VDKPURPLE1,

CT_VLTPURPLE2,
CT_LTPURPLE2,
CT_DKPURPLE2,
CT_VDKPURPLE2,

CT_VLTPURPLE3,
CT_LTPURPLE3,
CT_DKPURPLE3,
CT_VDKPURPLE3,

CT_VLTRED1,
CT_LTRED1,
CT_DKRED1,
CT_VDKRED1,

CT_MAX
} ct_table_t;

extern vec4_t colorTable[CT_MAX];

extern	vec4_t		colorBlack;
extern	vec4_t		colorRed;
extern	vec4_t		colorGreen;
extern	vec4_t		colorBlue;
extern	vec4_t		colorYellow;
extern	vec4_t		colorMagenta;
extern	vec4_t		colorCyan;
extern	vec4_t		colorWhite;
extern	vec4_t		colorLtGrey;
extern	vec4_t		colorMdGrey;
extern	vec4_t		colorDkGrey;

#define Q_COLOR_ESCAPE	'^'
#define Q_IsColorString(p)	( p && *(p) == Q_COLOR_ESCAPE && *((p)+1) && *((p)+1) != Q_COLOR_ESCAPE )

#define COLOR_BLACK		'0'
#define COLOR_RED		'1'
#define COLOR_GREEN		'2'
#define COLOR_YELLOW	'3'
#define COLOR_BLUE		'4'
#define COLOR_CYAN		'5'
#define COLOR_MAGENTA	'6'
#define COLOR_WHITE		'7'
#define ColorIndex(c)	( ( (c) - '0' ) & 7 )

#define S_COLOR_BLACK	"^0"
#define S_COLOR_RED		"^1"
#define S_COLOR_GREEN	"^2"
#define S_COLOR_YELLOW	"^3"
#define S_COLOR_BLUE	"^4"
#define S_COLOR_CYAN	"^5"
#define S_COLOR_MAGENTA	"^6"
#define S_COLOR_WHITE	"^7"

extern vec4_t	g_color_table[8];

#define	MAKERGB( v, r, g, b ) v[0]=r;v[1]=g;v[2]=b
#define	MAKERGBA( v, r, g, b, a ) v[0]=r;v[1]=g;v[2]=b;v[3]=a

#define DEG2RAD( a ) ( ( (a) * M_PI ) / 180.0F )
#define RAD2DEG( a ) ( ( (a) * 180.0f ) / M_PI )

struct cplane_s;

extern	vec3_t	vec3_origin;
extern	vec3_t	axisDefault[3];

#define	nanmask (255<<23)

#define	IS_NAN(x) (((*(int32_t *)&x)&nanmask)==nanmask)

signed char ClampChar( int32_t i );
int16_t ClampShort( int32_t i );

// this isn't a real cheap function to call!
int32_t DirToByte( vec3_t dir );
void ByteToDir( int32_t b, vec3_t dir );

#if	1

#define DotProduct(x,y)			((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define VectorSubtract(a,b,c)	((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define VectorAdd(a,b,c)		((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define VectorCopy(a,b)			((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])
#define	VectorScale(v, s, o)	((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))
#define	VectorMA(v, s, b, o)	((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))

#else

#define DotProduct(x,y)			_DotProduct(x,y)
#define VectorSubtract(a,b,c)	_VectorSubtract(a,b,c)
#define VectorAdd(a,b,c)		_VectorAdd(a,b,c)
#define VectorCopy(a,b)			_VectorCopy(a,b)
#define	VectorScale(v, s, o)	_VectorScale(v,s,o)
#define	VectorMA(v, s, b, o)	_VectorMA(v,s,b,o)

#endif

#ifdef __LCC__
#ifdef VectorCopy
#undef VectorCopy
// this is a little hack to get more efficient copies in our interpreter
typedef struct {
	float	v[3];
} vec3struct_t;
#define VectorCopy(a,b)	*(vec3struct_t *)b=*(vec3struct_t *)a;
#endif
#endif

#define VectorClear(a)			((a)[0]=(a)[1]=(a)[2]=0)
#define VectorNegate(a,b)		((b)[0]=-(a)[0],(b)[1]=-(a)[1],(b)[2]=-(a)[2])
#define VectorSet(v, x, y, z)	((v)[0]=(x), (v)[1]=(y), (v)[2]=(z))
#define Vector4Copy(a,b)		((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2],(b)[3]=(a)[3])

extern void init_tonextint(qboolean verbose);
extern float tonextint(float x);

//TiM
unsigned long atoul( const char *string );

#define	SnapVector(v) {v[0] = tonextint(v[0]); v[1] = tonextint(v[1]); v[2]= tonextint(v[2]);}

// just in case you do't want to use the macros
vec_t _DotProduct( const vec3_t v1, const vec3_t v2 );
void _VectorSubtract( const vec3_t veca, const vec3_t vecb, vec3_t out );
void _VectorAdd( const vec3_t veca, const vec3_t vecb, vec3_t out );
void _VectorCopy( const vec3_t in, vec3_t out );
void _VectorScale( const vec3_t in, float scale, vec3_t out );
void _VectorMA( const vec3_t veca, float scale, const vec3_t vecb, vec3_t vecc );

uint32_t ColorBytes3(float r, float g, float b);
uint32_t ColorBytes4(float r, float g, float b, float a);

float NormalizeColor(const vec3_t in, vec3_t out);

float RadiusFromBounds( const vec3_t mins, const vec3_t maxs );
void ClearBounds( vec3_t mins, vec3_t maxs );
void AddPointToBounds( const vec3_t v, vec3_t mins, vec3_t maxs );
int VectorCompare( const vec3_t v1, const vec3_t v2 );
vec_t VectorLength( const vec3_t v );
vec_t VectorLengthSquared( const vec3_t v );
vec_t Distance( const vec3_t p1, const vec3_t p2 );
vec_t DistanceSquared( const vec3_t p1, const vec3_t p2 );
void CrossProduct( const vec3_t v1, const vec3_t v2, vec3_t cross );
vec_t VectorNormalize (vec3_t v);		// returns vector length
void VectorNormalizeFast(vec3_t v);		// does NOT return vector length, uses rsqrt approximation
vec_t VectorNormalize2( const vec3_t v, vec3_t out );
void VectorInverse (vec3_t v);
void Vector4Scale( const vec4_t in, vec_t scale, vec4_t out );
void VectorRotate( vec3_t in, vec3_t matrix[3], vec3_t out );
int32_t Q_log2(int32_t val);

inline auto random()	{ return ((rand () & 0x7fff) / ((float)0x7fff)); }
#define crandom()	(2.0 * (random() - 0.5))

float	flrandom(float min, float max);
int32_t	irandom(int32_t min, int32_t max);

void vectoangles( const vec3_t value1, /*@out@*/ vec3_t angles);
void AnglesToAxis( const vec3_t angles, vec3_t axis[3] );

void AxisClear( vec3_t axis[3] );
void AxisCopy( vec3_t in[3], vec3_t out[3] );

void SetPlaneSignbits( struct cplane_s *out );
int32_t BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, struct cplane_s *plane);

float	AngleMod(float a);
float	LerpAngle (float from, float to, float frac);
float	AngleSubtract( float a1, float a2 );
void	AnglesSubtract( vec3_t v1, vec3_t v2, vec3_t v3 );

float AngleNormalize360 ( float angle );
float AngleNormalize180 ( float angle );
float AngleDelta ( float angle1, float angle2 );

qboolean PlaneFromPoints( vec4_t plane, const vec3_t a, const vec3_t b, const vec3_t c );
void ProjectPointOnPlane( vec3_t dst, const vec3_t p, const vec3_t normal );
void RotatePointAroundVector(vec3_t dst, const vec3_t dir, const vec3_t point, float degrees);
void RotateAroundDirection(vec3_t axis[3], float yaw);
void MakeNormalVectors( const vec3_t forward, vec3_t right, vec3_t up );
// perpendicular vector could be replaced by this

int32_t	PlaneTypeForNormal (vec3_t normal);

void MatrixMultiply(float in1[3][3], float in2[3][3], float out[3][3]);
void AngleVectors( const vec3_t angles, /*@null@*/ /*@out@*/ vec3_t forward, /*@null@*/ /*@out@*/ vec3_t right, /*@null@*/ /*@out@*/ vec3_t up);
void PerpendicularVector( vec3_t dst, const vec3_t src );
void VectorShort(vec3_t vect);
void UnVectorShort(vec3_t vect);


//=============================================

float Com_Clamp( float min, float max, float value );

/*@shared@*/ /*@null@*/ char *COM_SkipPath(/*@null@*/  char *pathname );
void	COM_StripExtension(/*@null@*/  const char *in, /*@null@*/ char *out );
void	COM_DefaultExtension( /*@null@*/ char *path, size_t maxSize, const /*@null@*/ char *extension );

void	COM_BeginParseSession( void );
int32_t		COM_GetCurrentParseLine( void );
/*@shared@*/ /*@null@*/ char *COM_Parse( char **data_p );
/*@shared@*/ /*@null@*/ char *COM_ParseExt( char **data_p, qboolean allowLineBreak );
//int32_t		COM_ParseInfos( char *buf, int32_t max, char infos[][MAX_INFO_STRING] );

//RPG-X: Parsing helping functions
qboolean COM_ParseString( char **data, char **s );
qboolean COM_ParseInt( char **data, int32_t *i);
qboolean COM_ParseFloat( char **data, float *f );
qboolean COM_ParseVec4( char **data, vec4_t vector );
qboolean COM_ParseVec3( char **data, vec3_t vector );

//RPG-X - Powers func for view camera
float Q_powf( float x, int32_t y );
//TiM: For averaging... and stuff
void VectorAverage( vec3_t mins, vec3_t maxs, vec3_t result );

// data is an in/out parm, returns a parsed out token

void	COM_MatchToken( char**buf_p, char *match );

void SkipBracedSection (char **program);
void SkipRestOfLine ( char **data );

void Parse1DMatrix (char **buf_p, int32_t x, float *m);
void Parse2DMatrix (char **buf_p, int32_t y, int32_t x, float *m);
void Parse3DMatrix (char **buf_p, int32_t z, int32_t y, int32_t x, float *m);

void QDECL Com_sprintf (/*@null@*/ char *dest, size_t size, /*@null@*/ const char *fmt, ...) __attribute__ ((format (printf, 3, 4)));


// mode parm for FS_FOpenFile
typedef enum {
	FS_READ,
	FS_WRITE,
	FS_APPEND,
	FS_APPEND_SYNC
} fsMode_t;

typedef enum {
	FS_SEEK_CUR,
	FS_SEEK_END,
	FS_SEEK_SET
} fsOrigin_t;

//=============================================

int32_t Q_isprint( int32_t c );
int32_t Q_islower( int32_t c );
int32_t Q_isupper( int32_t c );
int32_t Q_isalpha( int32_t c );

// portable case insensitive compare
int32_t	Q_stricmp (/*@null@*/ const char *s1, /*@null@*/ const char *s2);
int32_t	Q_strncmp (/*@null@*/ const char *s1, /*@null@*/ const char *s2, int32_t n);
int32_t	Q_stricmpn (/*@null@*/ const char *s1, /*@null@*/ const char *s2, int32_t n);
/*@shared@*/ /*@null@*/ char*	Q_strlwr( /*@null@*/ char *s1 );
/*@shared@*/ /*@null@*/ char*	Q_strupr( /*@null@*/ char *s1 );
/*@shared@*/ /*@null@*/ char*	Q_strrchr( /*@null@*/ const char* string, int32_t c );
/**
 * Goes through a given str searching for one of the given tokens.
 * If it finds one it returns a new string containing everything
 * from str until the found token (Note: remember to free the result
 * if no longer needed). Returns NULL if no token is found. Make sure
 * to adjust str for the next call yourself as this function won't do that.
 */
/*@shared@*/ /*@null@*/ char* Q_strtok(/*@null@*/ char* str, /*@null@*/ const char *tok, size_t size);

// buffer size safe library replacements
void	Q_strncpyz( /*@null@*/ char *dest, /*@null@*/ const char *src, size_t destsize );
void	Q_strcat( /*@null@*/ char *dest, size_t size, /*@null@*/ const char *src );

// strlen that discounts Quake color sequences
int32_t Q_PrintStrlen( /*@null@*/ const char *string );
// removes color sequences from string
/*@shared@*/ /*@null@*/ char *Q_CleanStr( /*@null@*/ char *string );

//=============================================


#ifdef _M_IX86
//
// optimised version for intel stuff...
//
int16_t	BigShort(int16_t l);
int32_t	BigLong (int32_t l);
float	BigFloat (float l);
#define LittleShort(l) l
#define LittleLong(l)  l
#define LittleFloat(l) l
//
#else
//
// standard smart byte-swap stuff...
//
int16_t	BigShort(int16_t l);
int16_t	LittleShort(int16_t l);
int32_t	BigLong (int32_t l);
int32_t	LittleLong (int32_t l);
float	BigFloat (float l);
float	LittleFloat (float l);
//
#endif



void	Swap_Init (void);
/*@shared@*/ char	* QDECL va(char *format, ...) __attribute__ ((format (printf, 1, 2)));

//=============================================

//
// key / value info strings
//
char *Info_ValueForKey( /*@shared@*/ /*@out@*/ const char *s, const char *key );
void Info_RemoveKey( char *s, const char *key );
void Info_RemoveKey_big( char *s, const char *key );
void Info_SetValueForKey( char *s, const char *key, const char *value );
void Info_SetValueForKey_Big( char *s, const char *key, const char *value );
qboolean Info_Validate( const char *s );
void Info_NextPair( const char **s, char key[MAX_INFO_KEY], char value[MAX_INFO_VALUE] );

// this is only here so the functions in q_shared.c and bg_*.c can link
void	QDECL Com_Error( int32_t errlevel, const char *error, ... ) __attribute__ ((format(printf, 2, 3)));
void	QDECL Com_Printf( const char *msg, ... ) __attribute__ ((format (printf, 1, 2)));


/*
==========================================================

CVARS (console variables)

Many variables can be used for cheating purposes, so when
cheats is zero, force all unspecified variables to their
default values.
==========================================================
*/

#define	CVAR_ARCHIVE		1	// set to cause it to be saved to vars.rc
								// used for system variables, not for player
								// specific configurations
#define	CVAR_USERINFO		2	// sent to server on connect or change
#define	CVAR_SERVERINFO		4	// sent in response to front end requests
#define	CVAR_SYSTEMINFO		8	// these cvars will be duplicated on all clients
#define	CVAR_INIT			16	// don't allow change from console at all,
								// but can be set from the command line
#define	CVAR_LATCH			32	// will only change when C code next does
								// a Cvar_Get(), so it can't be changed
								// without proper initialization.  modified
								// will be set, even though the value hasn't
								// changed yet
#define	CVAR_ROM			64	// display only, cannot be set by user at all
#define	CVAR_USER_CREATED	128	// created by a set command
#define	CVAR_TEMP			256	// can be set even when cheats are disabled, but is not archived
#define CVAR_CHEAT			512	// can not be changed if cheats are disabled
#define CVAR_NORESTART		1024	// do not clear when a cvar_restart is issued

// nothing outside the Cvar_*() functions should modify these fields!
typedef struct cvar_s {
	char		*name;
	char		*string;
	char		*resetString;		// cvar_restart will reset to this value
	char		*latchedString;		// for CVAR_LATCH vars
	int32_t		flags;
	qboolean	modified;			// set each time the cvar is changed
	int32_t		modificationCount;	// incremented each time the cvar is changed
	float		value;				// atof( string )
	int32_t		integer;			// atoi( string )
	struct cvar_s *next;
} cvar_t;

#define	MAX_CVAR_VALUE_STRING	256

typedef int32_t	cvarHandle_t;

// the modules that run in the virtual machine can't access the cvar_t directly,
// so they must ask for structured updates
typedef struct {
	cvarHandle_t	handle;
	int32_t			modificationCount;
	float		value;
	int32_t			integer;
	char		string[MAX_CVAR_VALUE_STRING];
} vmCvar_t;

/*
==============================================================

COLLISION DETECTION

==============================================================
*/

#include "surfaceflags.h"			// shared with the q3map utility

// plane types are used to speed some tests
// 0-2 are axial planes
#define	PLANE_X			0
#define	PLANE_Y			1
#define	PLANE_Z			2
#define	PLANE_NON_AXIAL	3


// plane_t structure
// !!! if this is changed, it must be changed in asm code too !!!
typedef struct cplane_s {
	vec3_t	normal;
	float	dist;
	byte	type;			// for fast side tests: 0,1,2 = axial, 3 = nonaxial
	byte	signbits;		// signx + (signy<<1) + (signz<<2), used as lookup during collision
	byte	pad[2];
} cplane_t;


// a trace is returned when a box is swept through the world
typedef struct {
	qboolean	allsolid;	// if true, plane is not valid
	qboolean	startsolid;	// if true, the initial point was in a solid area
	float		fraction;	// time completed, 1.0 = didn't hit anything
	vec3_t		endpos;		// final position
	cplane_t	plane;		// surface normal at impact, transformed to world space
	int32_t			surfaceFlags;	// surface hit
	int32_t			contents;	// contents on other side of surface hit
	int32_t			entityNum;	// entity the contacted sirface is a part of
} trace_t;

// trace->entityNum can also be 0 to (MAX_GENTITIES-1)
// or ENTITYNUM_NONE, ENTITYNUM_WORLD


// markfragments are returned by CM_MarkFragments()
typedef struct {
	int32_t		firstPoint;
	int32_t		numPoints;
} markFragment_t;



typedef struct {
	vec3_t		origin;
	vec3_t		axis[3];
} orientation_t;

//=====================================================================


// in order from highest priority to lowest
// if none of the catchers are active, bound key strings will be executed
#define KEYCATCH_CONSOLE	1
#define	KEYCATCH_UI			2
#define	KEYCATCH_MESSAGE	4


// sound channels
// channel 0 never willingly overrides
// other channels will allways override a playing sound on that channel
typedef enum {
	CHAN_AUTO,
	CHAN_LOCAL,		// menu sounds, etc
	CHAN_WEAPON,
	CHAN_VOICE,
	CHAN_ITEM,
	CHAN_BODY,
	CHAN_LOCAL_SOUND,	// chat messages, etc
	CHAN_ANNOUNCER,		// announcer voices, etc
	CHAN_MENU1		// menu stuff, etc
} soundChannel_t;


/*
========================================================================

  ELEMENTS COMMUNICATED ACROSS THE NET

========================================================================
*/

#define	ANGLE2SHORT(x)	((int32_t)((x)*65536/360) & 65535)
#define	SHORT2ANGLE(x)	((x)*(360.0/65536))

#define	SNAPFLAG_RATE_DELAYED	1
#define	SNAPFLAG_NOT_ACTIVE		2	// snapshot used during connection and for zombies
#define SNAPFLAG_SERVERCOUNT	4	// toggled every map_restart so transitions can be detected

//
// per-level limits
//
#define	MAX_CLIENTS			128		// absolute limit
#define MAX_LOCATIONS		64
#define GENTITYNUM_BITS		11		// XTRA
#define	MAX_GENTITIES		(1<<GENTITYNUM_BITS)

// entitynums are communicated with GENTITY_BITS, so any reserved
// values thatare going to be communcated over the net need to
// also be in this range
#define	ENTITYNUM_NONE		(MAX_GENTITIES-1)
#define	ENTITYNUM_WORLD		(MAX_GENTITIES-2)
#define	ENTITYNUM_MAX_NORMAL	(MAX_GENTITIES-2)

#define MAX_MODELS_BITS		9	// XTRA
#define	MAX_MODELS			(1<<MAX_MODELS_BITS)		// these are sent over the net as 8 bits <--- who cares xD
#define	MAX_SOUNDS			256		// so they cannot be blindly increased

//RPG-X: TiM - Tricorder InfoStrings
#define MAX_TRIC_STRINGS	64
#define	MAX_DECOYS			64 //128

#define	MAX_CONFIGSTRINGS	4096

// these are the only configstrings that the system reserves, all the
// other ones are strictly for servergame to clientgame communication
#define	CS_SERVERINFO		0		// an info string with all the serverinfo cvars
#define	CS_SYSTEMINFO		1		// an info string for server system to client system configuration (timescale, etc)

#define	RESERVED_CONFIGSTRINGS	2	// game can't modify below this, only the system can

#define	MAX_GAMESTATE_CHARS 64000

typedef struct {
	int32_t			stringOffsets[MAX_CONFIGSTRINGS];
	char		stringData[MAX_GAMESTATE_CHARS];
	int32_t			dataCount;
} gameState_t;

//=========================================================

// bit field limits
#define	MAX_STATS				16
#define	MAX_PERSISTANT			16
#define	MAX_POWERUPS			16 //RPG-X:TiM
#define	MAX_WEAPONS				16

#define	MAX_PS_EVENTS			4


// Used in introTime below...
#define TIME_INTRO		5000


// playerState_t is the information needed by both the client and server
// to predict player motion and actions
// nothing outside of pmove should modify these, or some degree of prediction error
// will occur

// you can't add anything to this without modifying the code in msg.c

// playerState_t is a full superset of entityState_t as it is used by players,
// so if a playerState_t is transmitted, the entityState_t can be fully derived
// from it.
typedef struct playerState_s {
	int32_t			commandTime;	// cmd->serverTime of last executed command
	int32_t			pm_type;
	int32_t			bobCycle;		// for view bobbing and footstep generation
	int32_t			pm_flags;		// ducked, jump_held, etc
	int32_t			pm_time;

	vec3_t			origin;
	vec3_t			velocity;
	int32_t			weaponTime;
	int32_t			rechargeTime;		// for the phaser
	int16_t			useTime;			// use debounce

	int32_t			introTime;			// for the holodoor

	int32_t			gravity;
	int32_t			speed;
	int32_t			delta_angles[3];	// add to command angles to get view direction
									// changed by spawns, rotating objects, and teleporters

	int32_t			groundEntityNum;// ENTITYNUM_NONE = in air

	int32_t			legsTimer;		// don't change low priority animations until this runs out
	int32_t			legsAnim;		// mask off ANIM_TOGGLEBIT

	int32_t			torsoTimer;		// don't change low priority animations until this runs out
	int32_t			torsoAnim;		// mask off ANIM_TOGGLEBIT

	int32_t			movementDir;	// a number 0 to 7 that represents the reletive angle
								// of movement to the view angle (axial and diagonals)
								// when at rest, the value will remain unchanged
								// used to twist the legs during strafing

	int32_t			eFlags;			// copied to entityState_t->eFlags

	int32_t			eventSequence;	// pmove generated events
	int32_t			events[MAX_PS_EVENTS];
	int32_t			eventParms[MAX_PS_EVENTS];

	int32_t			externalEvent;	// events set on player from another source
	int32_t			externalEventParm;
	int32_t			externalEventTime;

	int32_t			clientNum;		// ranges from 0 to MAX_CLIENTS-1
	int32_t			weapon;			// copied to entityState_t->weapon
	int32_t			weaponstate;

	vec3_t			viewangles;		// for fixed views
	int32_t			viewheight;

	// damage feedback
	int32_t			damageEvent;	// when it changes, latch the other parms
	int32_t			damageYaw;
	int32_t			damagePitch;
	int32_t			damageCount;
	int32_t			damageShieldCount;

	int32_t			stats[MAX_STATS];
	int32_t			persistant[MAX_PERSISTANT];	// stats that aren't cleared on death
	int32_t			powerups[MAX_POWERUPS];	// level.time that the powerup runs out
	int32_t			ammo[MAX_WEAPONS];

	// not communicated over the net at all
	int32_t			ping;			// server to game info for scoreboard

	int32_t			entityEventSequence;

} playerState_t;


//====================================================================


//
// usercmd_t->button bits, many of which are generated by the client system,
// so they aren't game/cgame only definitions
//
#define	BUTTON_ATTACK		1
#define	BUTTON_TALK			2			// displays talk balloon and disables actions
#define	BUTTON_USE_HOLDABLE	4
#define	BUTTON_GESTURE		8
#define	BUTTON_WALKING		16			// walking can't just be infered from MOVE_RUN
										// because a key pressed late in the frame will
										// only generate a small move value for that frame
										// walking will use different animations and
										// won't generate footsteps

#define BUTTON_ALT_ATTACK	32
#define	BUTTON_USE			64			// the ol' use key returns!
#define	BUTTON_ANY			128			// any key whatsoever

#define	MOVE_RUN			120			// if forwardmove or rightmove are >= MOVE_RUN,
										// then BUTTON_WALKING should be set

#define MASK_KILL_EMOTES	( BUTTON_ATTACK /*| BUTTON_WALKING*/ | BUTTON_ALT_ATTACK )


// usercmd_t is sent to the server each client frame
typedef struct usercmd_s {
	int32_t		serverTime;
	byte	buttons;
	byte	weapon;
	int32_t		angles[3];
	signed char	forwardmove, rightmove, upmove;
} usercmd_t;

//===================================================================

// if entityState->solid == SOLID_BMODEL, modelindex is an inline model number
#define	SOLID_BMODEL	0xffffff

typedef enum {
	TR_STATIONARY,
	TR_INTERPOLATE,				// non-parametric, but interpolate between snapshots
	TR_LINEAR,
	TR_LINEAR_STOP,
	TR_SINE,					// value = base + sin( time / duration ) * delta
	TR_GRAVITY
} trType_t;

typedef struct {
	trType_t	trType;
	int32_t		trTime;
	int32_t		trDuration;			// if non 0, trTime + trDuration = stop time
	vec3_t	trBase;
	vec3_t	trDelta;			// velocity, etc
//	vec3_t	trDelta2;			// Additional vector data.
} trajectory_t;

// entityState_t is the information conveyed from the server
// in an update message about entities that the client will
// need to render in some way
// Different eTypes may use the information in different ways
// The messages are delta compressed, so it doesn't really matter if
// the structure size is fairly large

typedef struct entityState_s {
	int32_t		number;			// entity index
	int32_t		eType;			// entityType_t
	int32_t		eFlags;

	trajectory_t	pos;	// for calculating position
	trajectory_t	apos;	// for calculating angles

	int32_t		time;
	int32_t		time2;

	vec3_t	origin;
	vec3_t	origin2;

	vec3_t	angles;
	vec3_t	angles2;

	int32_t		otherEntityNum;	// shotgun sources, etc
	int32_t		otherEntityNum2;

	int32_t		groundEntityNum;	// -1 = in air

	int32_t		constantLight;	// r + (g<<8) + (b<<16) + (intensity<<24)
	int32_t		loopSound;		// constantly loop this sound

	int32_t		modelindex;
	int32_t		modelindex2;
	int32_t		clientNum;		// 0 to (MAX_CLIENTS - 1), for players and corpses
	int32_t		frame;

	int32_t		solid;			// for client side prediction, trap_linkentity sets this properly

	int32_t		event;			// impulse events -- muzzle flashes, footsteps, etc
	int32_t		eventParm;

	// for players
	int32_t		powerups;		// bit flags
	int32_t		weapon;			// determines weapon and flash model, etc
	int32_t		legsAnim;		// mask off ANIM_TOGGLEBIT
	int32_t		torsoAnim;		// mask off ANIM_TOGGLEBIT

#ifdef MISSION_PACK
	int32_t		generic1;
	int32_t		generic2;
#endif
} entityState_t;

typedef enum {
	CA_UNINITIALIZED,
	CA_DISCONNECTED, 	// not talking to a server
	CA_AUTHORIZING,		// not used any more, was checking cd key
	CA_CONNECTING,		// sending request packets to the server
	CA_CHALLENGING,		// sending challenge packets to the server
	CA_CONNECTED,		// netchan_t established, getting gamestate
	CA_LOADING,			// only during cgame initialization, never during main loop
	CA_PRIMED,			// got gamestate, waiting for first frame
	CA_ACTIVE,			// game views should be displayed
	CA_CINEMATIC		// playing a cinematic or a static pic, not connected to a server
} connstate_t;

typedef enum {
	AIGT_SINGLE_PLAYER,	// Q3A single player mode
	AIGT_TEAM,			// any team game
	AIGT_OTHER			// anything else!
} aiGametype_t;

//extern	vmCvar_t	rpg_medicsrevive;			//RPG-X: RedTechie - To let bg_pmovto work
//extern	vmCvar_t	rpg_noclipspectating;			//Added here (by rpg-x:j2j) so it could be seen in bg_pmove.c
//extern  qboolean    ClientNCSpec;					//Private flag for weather the client chooses to spectate noclip style (rpg-x:j2j)

/*
========================================================================

String ID Tables (imported from single player by RPG-X: J2J)

========================================================================
*/

//Strictly not directly related to string Id tables, but is only used (so far) in the animation parsing string table.
#define ENUM2STRING(arg)   #arg,arg

typedef struct stringID_table_s
{
	/*@null@*/ char* name;
	int32_t		id;
} stringID_table_t;

int32_t GetIDForString ( stringID_table_t *table, const char *string );
const char *GetStringForID( stringID_table_t *table, int32_t id );

#endif	// __Q_SHARED_H
