/* Copyright (C) 1999-2000 Id Software, Inc.
*
* q_shared.c -- stateless support routines that are included in each code dll
*/
#include "q_shared.h"

/*vmCvar_t	rpg_medicsrevive;			//RPG-X: RedTechie - To let bg_pmovto work
vmCvar_t	rpg_noclipspectating;		//RPG-X J2J: Defined here to prevent link errors when compiling cgame
qboolean    ClientNCSpec = qtrue;		//RPG-X J2J: Private flag for weather the client chooses to spectate noclip style*/

/* float Q_powf ( float x, int y ); */

float Com_Clamp( float min, float max, float value ) {
	if ( value < min ) {
		return min;
	}
	if ( value > max ) {
		return max;
	}
	return value;
}


/*
============
COM_SkipPath
============
*/
char *COM_SkipPath (char *pathname)
{
	char	*last;

	last = pathname;
	while (*pathname)
	{
		if (*pathname=='/')
			last = pathname+1;
		pathname++;
	}
	return last;
}

/*
============
COM_StripExtension
============
*/
void COM_StripExtension( const char *in, char *out ) {
	while ( *in && *in != '.' ) {
		*out++ = *in++;
	}
	*out = 0;
}


/*
==================
COM_DefaultExtension
==================
*/
void COM_DefaultExtension (char *path, int maxSize, const char *extension ) {
	char	oldPath[MAX_QPATH];
	char    *src;

	/*
	* if path doesn't have a .EXT, append extension
	* (extension should include the .)
	*/
	src = path + strlen(path) - 1;

	while (*src != '/' && src != path) {
		if ( *src == '.' ) {
			return;                 /* it has an extension */
		}
		src--;
	}

	Q_strncpyz( oldPath, path, sizeof( oldPath ) );
	Com_sprintf( path, maxSize, "%s%s", oldPath, extension );
}

/*
============================================================================

BYTE ORDER FUNCTIONS

============================================================================
*/

/* 
*can't just use function pointers, or dll linkage can
* mess up when qcommon is included in multiple places
*/
static short	(*_BigShort) (short l);
static short	(*_LittleShort) (short l);
static int		(*_BigLong) (int l);
static int		(*_LittleLong) (int l);
static float	(*_BigFloat) (float l);
static float	(*_LittleFloat) (float l);


#ifdef _M_IX86
/*
* optimised version for intel stuff...
*/
short	BigShort(short l){return _BigShort(l);}
int		BigLong (int l) {return _BigLong(l);}
float	BigFloat (float l) {return _BigFloat(l);}
#define	LittleShort(l) l
#define LittleLong(l)  l
#define LittleFloat(l) l
#else
/*
* standard smart byte-swap stuff....
*/
short	BigShort(short l){return _BigShort(l);}
short	LittleShort(short l) {return _LittleShort(l);}
int		BigLong (int l) {return _BigLong(l);}
int		LittleLong (int l) {return _LittleLong(l);}
float	BigFloat (float l) {return _BigFloat(l);}
float	LittleFloat (float l) {return _LittleFloat(l);}
//
#endif


short   ShortSwap (short l)
{
	byte    b1,b2;

	b1 = l&255;
	b2 = (l>>8)&255;

	return (b1<<8) + b2;
}

short	ShortNoSwap (short l)
{
	return l;
}

int    LongSwap (int l)
{
	byte    b1,b2,b3,b4;

	b1 = l&255;
	b2 = (l>>8)&255;
	b3 = (l>>16)&255;
	b4 = (l>>24)&255;

	return ((int)b1<<24) + ((int)b2<<16) + ((int)b3<<8) + b4;
}

int	LongNoSwap (int l)
{
	return l;
}

float FloatSwap (float f)
{
	union
	{
		float	f;
		byte	b[4];
	} dat1, dat2;


	dat1.f = f;
	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];
	return dat2.f;
}

float FloatNoSwap (float f)
{
	return f;
}

/*
================
Swap_Init
================
*/
void Swap_Init (void)
{
	byte	swaptest[2] = {1,0};

	/* set the byte swapping variables in a portable manner	*/
	if ( *(short *)swaptest == 1)
	{
		_BigShort = ShortSwap;
		_LittleShort = ShortNoSwap;
		_BigLong = LongSwap;
		_LittleLong = LongNoSwap;
		_BigFloat = FloatSwap;
		_LittleFloat = FloatNoSwap;
	}
	else
	{
		_BigShort = ShortNoSwap;
		_LittleShort = ShortSwap;
		_BigLong = LongNoSwap;
		_LittleLong = LongSwap;
		_BigFloat = FloatNoSwap;
		_LittleFloat = FloatSwap;
	}

}


/*
============================================================================

PARSING

============================================================================
*/

static	char	com_token[MAX_TOKEN_CHARS];
static	int		com_lines;

void COM_BeginParseSession( void )
{
	com_lines = 0;
}

int COM_GetCurrentParseLine( void )
{
	return com_lines;
}

char *COM_Parse( char **data_p )
{
	return COM_ParseExt( data_p, qtrue );
}


/*
==============
COM_Parse

Parse a token out of a string
Will never return NULL, just empty strings

If "allowLineBreaks" is qtrue then an empty
string will be returned if the next token is
a newline.
==============
*/
static char *SkipWhitespace( char *data, qboolean *hasNewLines ) {
	int c;

	while( (c = *data) <= ' ') {
		if( !c ) {
			return NULL;
		}
		if( c == '\n' ) {
			com_lines++;
			*hasNewLines = qtrue;
		}
		data++;
	}

	return data;
}

char *COM_ParseExt( char **data_p, qboolean allowLineBreaks )
{
	int c = 0, len;
	qboolean hasNewLines = qfalse;
	char *data;

	data = *data_p;
	len = 0;
	com_token[0] = 0;

	/* make sure incoming data is valid */
	if ( !data )
	{
		*data_p = NULL;
		return com_token;
	}

	while ( 1 )
	{
		/* skip whitespace */
		data = SkipWhitespace( data, &hasNewLines );
		if ( !data )
		{
			*data_p = NULL;
			return com_token;
		}
		if ( hasNewLines && !allowLineBreaks )
		{
			*data_p = data;
			return com_token;
		}

		c = *data;

		/* skip double slash comments */
		if ( c == '/' && data[1] == '/' )
		{
			while (*data && *data != '\n')
				data++;
		}
		/* skip comments */
		else if ( c=='/' && data[1] == '*' ) 
		{
			while ( *data && ( *data != '*' || data[1] != '/' ) ) 
			{
				data++;
			}
			if ( *data ) 
			{
				data += 2;
			}
		}
		else
		{
			break;
		}
	}

	/* handle quoted strings */
	if (c == '\"')
	{
		data++;
		while (1)
		{
			c = *data++;
			if (c=='\"' || !c)
			{
				com_token[len] = 0;
				*data_p = ( char * ) data;
				return com_token;
			}
			if (len < MAX_TOKEN_CHARS)
			{
				com_token[len] = c;
				len++;
			}
		}
	}

	/* parse a regular word */
	do
	{
		if (len < MAX_TOKEN_CHARS)
		{
			com_token[len] = c;
			len++;
		}
		data++;
		c = *data;
		if ( c == '\n' )
			com_lines++;
	} while (c>32);

	if (len == MAX_TOKEN_CHARS)
	{
		/*		Com_Printf ("Token exceeded %i chars, discarded.\n", MAX_TOKEN_CHARS); */
		len = 0;
	}
	com_token[len] = 0;

	*data_p = ( char * ) data;

	return com_token;
}

/*
===============
COM_ParseString
===============
*/
qboolean COM_ParseString( char **data, char **s )
{
	*s = COM_ParseExt( data, qfalse);

	if ( s[0] == 0 ) {
		Com_Printf( S_COLOR_RED "ERROR: Unexpected End Of File\n");
		return qtrue;
	}

	return qfalse;
}

/*
===============
COM_ParseInt
===============
*/

qboolean COM_ParseInt( char **data, int *i)
{
	const char *token;
	token = COM_ParseExt( data, qfalse );

	if (token[0] == 0 ) {
		Com_Printf( S_COLOR_RED "ERROR: Unexpected End Of File\n");
		return qtrue;
	}

	*i = atoi(token);
	return qfalse;
}

/*
===============
COM_ParseFloat
===============
*/
qboolean COM_ParseFloat( char **data, float *f ) 
{
	const char	*token;

	token = COM_ParseExt( data, qfalse );
	if ( token[0] == 0 ) 
	{
		Com_Printf( S_COLOR_RED "unexpected EOF\n" );
		return qtrue;
	}

	*f = atof( token );
	return qfalse;
}

/*
===============
COM_ParseVec4
===============
*/
qboolean COM_ParseVec3( char **data, vec3_t vector ) 
{
	const char	*token;
	int i;

	token = COM_Parse( data );
	if ( token[0] == 0 ) 
	{
		Com_Printf( S_COLOR_RED "unexpected EOF\n" );
		return qtrue;
	}

	if ( Q_stricmpn( token, "{", 1 ) ) {
		Com_Printf( S_COLOR_RED "No { found in vector.\n");
		return qtrue;
	}

	for ( i = 0; i < 3; i++ ) {
		token = COM_ParseExt( data, qfalse );
		if ( token[0] == 0 ) {
			Com_Printf( S_COLOR_RED "unexpected EOF\n" );
			return qtrue;
		}

		vector[i] = atof( token );
	}

	/* TiM: Not reeeeally necessary */
	token = COM_ParseExt( data, qfalse );
	if ( token[0] == 0 ) 
	{
		Com_Printf( S_COLOR_RED "unexpected EOF\n" );
		return qtrue;
	}

	if ( Q_stricmpn( token, "}", 1 ) ) {
		Com_Printf( S_COLOR_RED "No } end found in vector.\n");
		return qtrue;
	}

	return qfalse;
}

/*
===============
COM_ParseVec4
===============
*/
qboolean COM_ParseVec4( char **data, vec4_t vector ) 
{
	const char	*token;
	int i;

	token = COM_ParseExt( data, qfalse );
	if ( token[0] == 0 ) 
	{
		Com_Printf( S_COLOR_RED "unexpected EOF\n" );
		return qtrue;
	}

	if ( Q_stricmpn( token, "{", 1 ) ) {
		Com_Printf( S_COLOR_RED "No { found in vector.\n");
		return qtrue;
	}

	for ( i = 0; i < 4; i++ ) {
		token = COM_ParseExt( data, qfalse );
		if ( token[0] == 0 ) {
			Com_Printf( S_COLOR_RED "unexpected EOF\n" );
			return qtrue;
		}

		vector[i] = atof( token );
	}

	/* TiM: Not reeeeally necessary */
	token = COM_ParseExt( data, qfalse );
	if ( token[0] == 0 ) 
	{
		Com_Printf( S_COLOR_RED "unexpected EOF\n" );
		return qtrue;
	}

	if ( Q_stricmpn( token, "}", 1 ) ) {
		Com_Printf( S_COLOR_RED "No } end found in vector.\n");
		return qtrue;
	}

	return qfalse;
}

#if 0
/* no longer used */
/*
===============
COM_ParseInfos
===============
*/
int COM_ParseInfos( char *buf, int max, char infos[][MAX_INFO_STRING] ) {
	char	*token;
	int		count;
	char	key[MAX_TOKEN_CHARS];

	count = 0;

	while ( 1 ) {
		token = COM_Parse( &buf );
		if ( !token[0] ) {
			break;
		}
		if ( strcmp( token, "{" ) ) {
			Com_Printf( "Missing { in info file\n" );
			break;
		}

		if ( count == max ) {
			Com_Printf( "Max infos exceeded\n" );
			break;
		}

		infos[count][0] = 0;
		while ( 1 ) {
			token = COM_ParseExt( &buf, qtrue );
			if ( !token[0] ) {
				Com_Printf( "Unexpected end of info file\n" );
				break;
			}
			if ( !strcmp( token, "}" ) ) {
				break;
			}
			Q_strncpyz( key, token, sizeof( key ) );

			token = COM_ParseExt( &buf, qfalse );
			if ( !token[0] ) {
				strcpy( token, "<NULL>" );
			}
			Info_SetValueForKey( infos[count], key, token );
		}
		count++;
	}

	return count;
}
#endif


/*
==================
COM_MatchToken
==================
*/
void COM_MatchToken( char **buf_p, const char *match ) {
	char	*token;

	token = COM_Parse( buf_p );
	if ( strcmp( token, match ) ) {
		Com_Error( ERR_DROP, "MatchToken: %s != %s", token, match );
	}
}


/*
=================
SkipBracedSection

The next token should be an open brace.
Skips until a matching close brace is found.
Internal brace depths are properly skipped.
=================
*/
void SkipBracedSection (char **program) {
	char			*token;
	int				depth=0;

	if (com_token[0]=='{') {	/* for tr_shader which just ate the brace */
		depth = 1;
	}
	do {
		token = COM_ParseExt( program, qtrue );
		if( token[1] == 0 ) {
			if( token[0] == '{' ) {
				depth++;
			}
			else if( token[0] == '}' ) {
				depth--;
			}
		}
	} while( depth && *program );
}

/*
=================
SkipRestOfLine
=================
*/
void SkipRestOfLine ( char **data ) {
	char	*p;
	int		c;

	p = *data;
	while ( (c = *p++) != 0 ) {
		if ( c == '\n' ) {
			com_lines++;
			break;
		}
	}

	*data = p;
}


void Parse1DMatrix (char **buf_p, int x, float *m) {
	char	*token;
	int		i;

	COM_MatchToken( buf_p, "(" );

	for (i = 0 ; i < x ; i++) {
		token = COM_Parse(buf_p);
		m[i] = atof(token);
	}

	COM_MatchToken( buf_p, ")" );
}

void Parse2DMatrix (char **buf_p, int y, int x, float *m) {
	int		i;

	COM_MatchToken( buf_p, "(" );

	for (i = 0 ; i < y ; i++) {
		Parse1DMatrix (buf_p, x, m + i * x);
	}

	COM_MatchToken( buf_p, ")" );
}

void Parse3DMatrix (char **buf_p, int z, int y, int x, float *m) {
	int		i;

	COM_MatchToken( buf_p, "(" );

	for (i = 0 ; i < z ; i++) {
		Parse2DMatrix (buf_p, y, x, m + i * x*y);
	}

	COM_MatchToken( buf_p, ")" );
}


/*
============================================================================

LIBRARY REPLACEMENT FUNCTIONS

============================================================================
*/

int Q_isprint( int c )
{
	if ( c >= 0x20 && c <= 0x7E )
		return ( 1 );
	return ( 0 );
}

int Q_islower( int c )
{
	if (c >= 'a' && c <= 'z')
		return ( 1 );
	return ( 0 );
}

int Q_isupper( int c )
{
	if (c >= 'A' && c <= 'Z')
		return ( 1 );
	return ( 0 );
}

int Q_isalpha( int c )
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return ( 1 );
	return ( 0 );
}

char* Q_strrchr( const char* string, int c )
{
	char cc = c;
	char *s;
	char *sp=(char *)0;

	s = (char*)string;

	while (*s)
	{
		if (*s == cc)
			sp = s;
		s++;
	}
	if (cc == 0)
		sp = s;

	return sp;
}

char* Q_strtok(char* str, const char *tok, int size) {
	char *ptr;
	char *result;
	int i, l;

	if(str == NULL || tok == NULL) {
		return NULL;
	}

	ptr = (char *)str;

	for(i = 0; i < strlen(str); i++) {
		for(l = 0; l < size; l++) {
			if(ptr[i] == tok[l]) {
				result = (char *)malloc(sizeof(char)+(i+1));
				if(result == NULL) {
					return NULL;
				}
				strncpy(result, str, i);
				result[i] = '\0';

				return result;
			}
		}
	}

	return NULL;
}

/*
=============
Q_strncpyz

Safe strncpy that ensures a trailing zero
=============
*/
void Q_strncpyz( char *dest, const char *src, int destsize ) {

	if ( !dest ) {
		Com_Error( ERR_FATAL, "Q_strncpyz: NULL dest. src was %s", src );
		return;
	}
	if ( !src ) {
		Com_Error( ERR_FATAL, "Q_strncpyz: NULL src. dest was %s", dest );
		return;
	}
	if ( destsize < 1 ) {
		Com_Error( ERR_FATAL, "Q_strncpyz: destsize < 1. src was %s,\ndest was %s", src, dest );
		return;
	}

	strncpy( dest, src, destsize-1 );
	dest[destsize-1] = 0;
}

int Q_stricmpn (const char *s1, const char *s2, int n) {
	int		c1, c2;

	/* bk001129 - moved in 1.17 fix not in id codebase */
	if ( s1 == NULL ) {
		if ( s2 == NULL )
			return 0;
		else
			return -1;
	}
	else if ( s2==NULL )
		return 1;



	do {
		c1 = *s1++;
		c2 = *s2++;

		if (!n--) {
			return 0;		/* strings are equal until end point */
		}

		if (c1 != c2) {
			if (c1 >= 'a' && c1 <= 'z') {
				c1 -= ('a' - 'A');
			}
			if (c2 >= 'a' && c2 <= 'z') {
				c2 -= ('a' - 'A');
			}
			if (c1 != c2) {
				return c1 < c2 ? -1 : 1;
			}
		}
	} while (c1);

	return 0;		/* strings are equal */
}

int Q_strncmp (const char *s1, const char *s2, int n) {
	int		c1, c2;

	do {
		c1 = *s1++;
		c2 = *s2++;

		if (!n--) {
			return 0;		/* strings are equal until end point */
		}

		if (c1 != c2) {
			return c1 < c2 ? -1 : 1;
		}
	} while (c1);

	return 0;		/* strings are equal */
}

int Q_stricmp (const char *s1, const char *s2) {
	return (s1 && s2) ? Q_stricmpn (s1, s2, 99999) : -1;
}


char *Q_strlwr( char *s1 ) {
	char	*s;

	s = s1;
	while ( *s ) {
		*s = tolower(*s);
		s++;
	}
	return s1;
}

char *Q_strupr( char *s1 ) {
	char	*s;

	s = s1;
	while ( *s ) {
		*s = toupper(*s);
		s++;
	}
	return s1;
}

/* never goes past bounds or leaves without a terminating 0 */
void Q_strcat( char *dest, int size, const char *src ) {
	int		l1;

	l1 = strlen( dest );
	if ( l1 >= size ) {
		Com_Error( ERR_FATAL, "Q_strcat: already overflowed" );
	}
	Q_strncpyz( dest + l1, src, size - l1 );
}


int Q_PrintStrlen( const char *string ) {
	int			len;
	const char	*p;

	if( !string ) {
		return 0;
	}

	len = 0;
	p = string;
	while( *p ) {
		if( Q_IsColorString( p ) ) {
			p += 2;
			continue;
		}
		p++;
		len++;
	}

	return len;
}


char *Q_CleanStr( char *string ) {
	char*	d;
	char*	s;
	int		c;

	if(string == NULL) {
		return string;
	} 

	s = string;
	d = string;
	while ((c = *s) != 0 ) {
		if ( Q_IsColorString( s ) ) {
			s++;
		}		
		else if ( c >= 0x20 && c <= 0x7E ) {
			*d++ = c;
		}
		s++;
	}
	*d = '\0';

	return string;
}

void QDECL Com_sprintf( char *dest, int size, const char *fmt, ...) {
	int			len;
	va_list		argptr;
	char		*bigbuffer;

	bigbuffer = (char *)malloc(sizeof(char)*(size+1));
	if(!bigbuffer) {
		Com_Printf("Com_sprintf: could not allocate %u bytes for BigBuffer\n", sizeof(char)*(size+1));
		return;
	}

	sprintf(bigbuffer, " ");

	va_start (argptr,fmt);
	len = vsprintf (bigbuffer,fmt,argptr);
	va_end (argptr);
	if ( len >= size + 1 ) {
		free(bigbuffer);
		Com_Error( ERR_FATAL, "Com_sprintf: overflowed bigbuffer" );
		return;
	}
	if (len >= size) {
		Com_Printf ("Com_sprintf: overflow of %i in %i\n", len, size);
		Com_Printf("Fmt: %s\n", fmt);
		Com_Printf("BigBuffer: %s\n", bigbuffer);
	}
	Q_strncpyz (dest, bigbuffer, size );
}

/*
============
va

does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
FIXME: make this buffer size safe someday
============
*/
char	* QDECL va( const char *format, ... ) {
	va_list		argptr;
	static char		string[2][32000];	/* in case va is called by nested functions */
	static int		index = 0;
	char	*buf;

	buf = string[index & 1];
	index++;

	va_start (argptr, format);
	vsprintf (buf, format,argptr);
	va_end (argptr);

	return buf;
}


/*
=====================================================================

INFO STRINGS

=====================================================================
*/

/*
===============
Info_ValueForKey

Searches the string for the given
key and returns the associated value, or an empty string.
FIXME: overflow check?
===============
*/
const char *Info_ValueForKey( const char *s, const char *key ) {
	char	pkey[BIG_INFO_KEY];
	static	char value[2][BIG_INFO_VALUE];	/* use two buffers so compares
											work without stomping on each other */
	static	int	valueindex = 0;
	char	*o;

	if ( !s || !key ) {
		return "";
	}

	if ( strlen( s ) >= BIG_INFO_STRING ) {
		Com_Error( ERR_DROP, "Info_ValueForKey: oversize infostring" );
	}

	valueindex ^= 1;
	if (*s == '\\')
		s++;
	while (1)
	{
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return "";
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value[valueindex];

		while (*s != '\\' && *s)
		{
			*o++ = *s++;
		}
		*o = 0;

		if (!Q_stricmp (key, pkey) )
			return value[valueindex];

		if (!*s)
			break;
		s++;
	}

	return "";
}


/*
===================
Info_NextPair

Used to itterate through all the key/value pairs in an info string
===================
*/
void Info_NextPair( const char **head, char key[MAX_INFO_KEY], char value[MAX_INFO_VALUE] ) {
	char	*o;
	const char	*s;

	s = *head;

	if ( *s == '\\' ) {
		s++;
	}
	key[0] = 0;
	value[0] = 0;

	o = key;
	while ( *s != '\\' ) {
		if ( !*s ) {
			*o = 0;
			*head = s;
			return;
		}
		*o++ = *s++;
	}
	*o = 0;
	s++;

	o = value;
	while ( *s != '\\' && *s ) {
		*o++ = *s++;
	}
	*o = 0;

	*head = s;
}


/*
===================
Info_RemoveKey
===================
*/
void Info_RemoveKey( char *s, const char *key ) {
	char	*start;
	char	pkey[MAX_INFO_KEY];
	char	value[MAX_INFO_VALUE];
	char	*o;

	if ( strlen( s ) >= MAX_INFO_STRING ) {
		Com_Error( ERR_DROP, "Info_RemoveKey: oversize infostring" );
	}

	memset(pkey, 0, sizeof(pkey));
	memset(value, 0, sizeof(value));

	if (strchr (key, '\\')) {
		return;
	}

	while (1)
	{
		start = s;
		if (*s == '\\')
			s++;
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;
		while (*s != '\\' && *s)
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;

		if (!strcmp (key, pkey) )
		{
			strcpy (start, s);	/* remove this part */
			return;
		}

		if (!*s)
			return;
	}

}

/*
===================
Info_RemoveKey_Big
===================
*/
void Info_RemoveKey_Big( char *s, const char *key ) {
	char	*start;
	char	pkey[BIG_INFO_KEY];
	char	value[BIG_INFO_VALUE];
	char	*o;

	if ( strlen( s ) >= BIG_INFO_STRING ) {
		Com_Error( ERR_DROP, "Info_RemoveKey_Big: oversize infostring" );
	}

	if (strchr (key, '\\')) {
		return;
	}

	while (1)
	{
		start = s;
		if (*s == '\\')
			s++;
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;
		while (*s != '\\' && *s)
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;

		if (!strcmp (key, pkey) )
		{
			strcpy (start, s);	/* remove this part */
			return;
		}

		if (!*s)
			return;
	}

}



/*
==================
Info_Validate

Some characters are illegal in info strings because they
can mess up the server's parsing
==================
*/
qboolean Info_Validate( const char *s ) {
	if ( strchr( s, '\"' ) ) {
		return qfalse;
	}
	if ( strchr( s, ';' ) ) {
		return qfalse;
	}
	return qtrue;
}

/*
==================
Info_SetValueForKey

Changes or adds a key/value pair
==================
*/
void Info_SetValueForKey( char *s, const char *key, const char *value ) {
	char	newi[MAX_INFO_STRING];

	if ( strlen( s ) >= MAX_INFO_STRING ) {
		Com_Error( ERR_DROP, "Info_SetValueForKey: oversize infostring" );
	}

	if (strchr (key, '\\') || strchr (value, '\\'))
	{
		Com_Printf ("Can't use keys or values with a \\\n");
		return;
	}

	if (strchr (key, ';') || strchr (value, ';'))
	{
		Com_Printf ("Can't use keys or values with a semicolon\n");
		return;
	}

	if (strchr (key, '\"') || strchr (value, '\"'))
	{
		Com_Printf ("Can't use keys or values with a \"\n");
		return;
	}

	Info_RemoveKey (s, key);
	if (!value || !strlen(value))
		return;

	Com_sprintf (newi, sizeof(newi), "\\%s\\%s", key, value);

	if (strlen(newi) + strlen(s) > MAX_INFO_STRING)
	{
		Com_Printf ("Info string length exceeded\n");
		return;
	}

	strcat (s, newi);
}

/*
==================
Info_SetValueForKey_Big

Changes or adds a key/value pair
==================
*/
void Info_SetValueForKey_Big( char *s, const char *key, const char *value ) {
	char	newi[BIG_INFO_STRING];

	if ( strlen( s ) >= BIG_INFO_STRING ) {
		Com_Error( ERR_DROP, "Info_SetValueForKey: oversize infostring" );
	}

	if (strchr (key, '\\') || strchr (value, '\\'))
	{
		Com_Printf ("Can't use keys or values with a \\\n");
		return;
	}

	if (strchr (key, ';') || strchr (value, ';'))
	{
		Com_Printf ("Can't use keys or values with a semicolon\n");
		return;
	}

	if (strchr (key, '\"') || strchr (value, '\"'))
	{
		Com_Printf ("Can't use keys or values with a \"\n");
		return;
	}

	Info_RemoveKey_Big (s, key);
	if (!value || !strlen(value))
		return;

	Com_sprintf (newi, sizeof(newi), "\\%s\\%s", key, value);

	if (strlen(newi) + strlen(s) > BIG_INFO_STRING)
	{
		Com_Printf ("BIG Info string length exceeded\n");
		return;
	}

	strcat (s, newi);
}

/*
========================================================================

String ID Tables (imported from single player by RPG-X: J2J)

========================================================================
*/


/*
-------------------------
GetIDForString 
-------------------------
*/

#define VALIDSTRING( a )	( ( a != NULL ) && ( a[0] != NULL ) )

int GetIDForString ( stringID_table_t *table, const char *string )
{
	int	index = 0;

	while ( ( table[index].name != NULL ) &&
		( table[index].name[0] != 0 ) )
	{
		if ( !Q_stricmp( table[index].name, string ) )
			return table[index].id;

		index++;
	}

	return -1;
}

/*
-------------------------
GetStringForID
-------------------------
*/

const char *GetStringForID( stringID_table_t *table, int id )
{
	int	index = 0;

	while ( ( table[index].name != NULL ) &&
		( table[index].name[0] != 0 )/*VALIDSTRING( table[index].name )*/ )/* RPG-X: RedTechie - Compile errors Fixed */
	{
		if ( table[index].id == id )
			return table[index].name;

		index++;
	}

	return NULL;
}


/*====================================================================*/


