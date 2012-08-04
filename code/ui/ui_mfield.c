// Copyright (C) 1999-2000 Id Software, Inc.
//
#include "ui_local.h"

/*
===================
MField_Draw

Handles horizontal scrolling and cursor blinking
x, y, are in pixels
===================
*/
void MField_Draw( mfield_t *edit, int x, int y, int style, vec4_t color,int cursor ) {
	int		len;
	int		charw,charh;
	int		drawLen;
	int		prestep;
	int		cursorChar;
	char	str[MAX_STRING_CHARS];

	drawLen = edit->widthInChars;
	len     = strlen( edit->buffer ) + 1;

	// guarantee that cursor will be visible
	if ( len <= drawLen ) 
	{
		prestep = 0;
	} 
	else 
	{
		if ( edit->scroll + drawLen > len ) 
		{
			edit->scroll = len - drawLen;
			if ( edit->scroll < 0 ) 
			{
				edit->scroll = 0;
			}
		}
		prestep = edit->scroll;
	}

	if ( prestep + drawLen > len ) 
	{
		drawLen = len - prestep;
	}

	// extract <drawLen> characters from the field at <prestep>
	if ( drawLen >= MAX_STRING_CHARS ) 
	{
		trap_Error( "drawLen >= MAX_STRING_CHARS" );
	}
	memcpy( str, edit->buffer + prestep, drawLen );
	str[ drawLen ] = 0;

	UI_DrawString( x, y, str, style, color, qtrue );

	// draw the cursor
	if (!cursor) 
	{
		return;
	}

	if ( trap_Key_GetOverstrikeMode() ) 
	{
		cursorChar = 11;
	} 
	else 
	{
		cursorChar = 10;
	}

	if (style & UI_SMALLFONT)
	{
		charh = SMALLCHAR_HEIGHT;
		charw =	SMALLCHAR_WIDTH;
	}
	else if (style & UI_GIANTFONT)
	{
		charh = GIANTCHAR_HEIGHT;
		charw =	GIANTCHAR_WIDTH;
	}
	else
	{
		charh = BIGCHAR_HEIGHT;
		charw =	BIGCHAR_WIDTH;
	}

	str[edit->cursor - prestep ] = '\0';
	len = UI_ProportionalStringWidth( str, UI_SMALLFONT );

	if (style & UI_CENTER)
	{
		x = x + (len/2);
		
		//x = x + ( edit->cursor - prestep ) * charw;
		//len = strlen(str);
		//x -= (len * (charw/2));
		
		//x = x - (len * (charw/2));
		//Com_Printf( "Dist is: %i, len is %i, width = %i\n", x, len, charw );
	}
	else if (style & UI_RIGHT)
	{
		len = strlen(str);
		x = x - len*charw;
	}
	else {
		x = x + len;
		//x = x + ( edit->cursor - prestep ) * charw;
	}

	if(!((uis.realtime/BLINK_DIVISOR) & 1))
	{
		char buff[2];

		buff[0] = (unsigned char)cursorChar; 
		buff[1] = '\0';

		UI_DrawChar( x, y+2, cursorChar, style & ~(UI_CENTER|UI_RIGHT), color );
		//UI_DrawProportionalString ( x, y+2, buff, style, color );
	}
}

/*
================
MField_Paste
================
*/
void MField_Paste( mfield_t *edit ) {
	char	pasteBuffer[64];
	int		pasteLen, i;

	trap_GetClipboardData( pasteBuffer, 64 );

	// send as if typed, so insert / overstrike works properly
	pasteLen = strlen( pasteBuffer );
	for ( i = 0 ; i < pasteLen ; i++ ) {
		MField_CharEvent( edit, pasteBuffer[i] );
	}
}

/*
=================
MField_KeyDownEvent

Performs the basic line editing functions for the console,
in-game talk, and menu fields

Key events are used for non-printable characters, others are gotten from char events.
=================
*/
void MField_KeyDownEvent( mfield_t *edit, int key ) {
	int		len;

	// shift-insert is paste
	if ( ( ( key == K_INS ) || ( key == K_KP_INS ) ) && trap_Key_IsDown( K_SHIFT ) ) {
		MField_Paste( edit );
		return;
	}

	len = strlen( edit->buffer );

	if ( key == K_DEL || key == K_KP_DEL ) {
		if ( edit->cursor < len ) {
			memmove( edit->buffer + edit->cursor, 
				edit->buffer + edit->cursor + 1, len - edit->cursor );
		}
		return;
	}

	if ( key == K_RIGHTARROW || key == K_KP_RIGHTARROW ) 
	{
		if ( edit->cursor < len ) {
			edit->cursor++;
		}
		if ( edit->cursor >= edit->scroll + edit->widthInChars && edit->cursor <= len )
		{
			edit->scroll++;
		}
		return;
	}

	if ( key == K_LEFTARROW || key == K_KP_LEFTARROW ) 
	{
		if ( edit->cursor > 0 ) {
			edit->cursor--;
		}
		if ( edit->cursor < edit->scroll )
		{
			edit->scroll--;
		}
		return;
	}

	if ( key == K_HOME || key == K_KP_HOME || ( tolower(key) == 'a' && trap_Key_IsDown( K_CTRL ) ) ) {
		edit->cursor = 0;
		edit->scroll = 0;
		return;
	}

	if ( key == K_END || key == K_KP_END || ( tolower(key) == 'e' && trap_Key_IsDown( K_CTRL ) ) ) {
		edit->cursor = len;
		edit->scroll = len - edit->widthInChars + 1;
		if (edit->scroll < 0)
			edit->scroll = 0;
		return;
	}

	if ( key == K_INS || key == K_KP_INS ) {
		trap_Key_SetOverstrikeMode( !trap_Key_GetOverstrikeMode() );
		return;
	}
}

/*
==================
MField_CharEvent
==================
*/
void MField_CharEvent( mfield_t *edit, int ch ) {
	int		len;

	if ( ch == 'v' - 'a' + 1 ) {	// ctrl-v is paste
		MField_Paste( edit );
		return;
	}

	if ( ch == 'c' - 'a' + 1 ) {	// ctrl-c clears the field
		MField_Clear( edit );
		return;
	}

	len = strlen( edit->buffer );

	if ( ch == 'h' - 'a' + 1 )	{	// ctrl-h is backspace
		if ( edit->cursor > 0 ) {
			memmove( edit->buffer + edit->cursor - 1, 
				edit->buffer + edit->cursor, len + 1 - edit->cursor );
			edit->cursor--;
			if ( edit->cursor < edit->scroll )
			{
				edit->scroll--;
			}
		}
		return;
	}

	if ( ch == 'a' - 'a' + 1 ) {	// ctrl-a is home
		edit->cursor = 0;
		edit->scroll = 0;
		return;
	}

	if ( ch == 'e' - 'a' + 1 ) {	// ctrl-e is end
		edit->cursor = len;
		edit->scroll = edit->cursor - edit->widthInChars + 1;
		if (edit->scroll < 0)
			edit->scroll = 0;
		return;
	}

	//
	// ignore any other non printable chars
	//
	if ( ch < 32 ) {
		return;
	}

	if ( !trap_Key_GetOverstrikeMode() ) {	
		if ((edit->cursor == MAX_EDIT_LINE - 1) || (edit->maxchars && edit->cursor >= edit->maxchars))
			return;
	} else {
		// insert mode
		if (( len == MAX_EDIT_LINE - 1 ) || (edit->maxchars && len >= edit->maxchars))
			return;
		memmove( edit->buffer + edit->cursor + 1, edit->buffer + edit->cursor, len + 1 - edit->cursor );
	}

	edit->buffer[edit->cursor] = ch;
	if (!edit->maxchars || edit->cursor < edit->maxchars-1)
		edit->cursor++;

	if ( edit->cursor >= edit->widthInChars )
	{
		edit->scroll++;
	}

	if ( edit->cursor == len + 1) {
		edit->buffer[edit->cursor] = 0;
	}
}

/*
==================
MField_Clear
==================
*/
void MField_Clear( mfield_t *edit ) {
	edit->buffer[0] = 0;
	edit->cursor = 0;
	edit->scroll = 0;
}

/*
==================
MenuField_Init
==================
*/
void MenuField_Init( menufield_s* m ) {
	int	l;
	int	w;
	int	h;

	MField_Clear( &m->field );

	if (m->field.style & UI_TINYFONT)
	{
		w = TINYCHAR_WIDTH;
		h = TINYCHAR_HEIGHT;
	}
	else if (m->field.style & UI_BIGFONT)
	{
		w = BIGCHAR_WIDTH;
		h = BIGCHAR_HEIGHT;
	}
	else if (m->field.style & UI_GIANTFONT)
	{
		w = GIANTCHAR_WIDTH;
		h = GIANTCHAR_HEIGHT;
	}	
	else 
	{
		w = SMALLCHAR_WIDTH;
		h = SMALLCHAR_HEIGHT;
	}

	//TiM: This is leftover from Quake III.  We'll be using this variable elsewhere now :)
	/*if (m->generic.name) {
		l = (strlen( m->generic.name )+1) * w;		
	}
	else {*/
		l = 0;
	//}

	if ( m->field.style & UI_CENTER ) {
		m->generic.right  = m->generic.x + (w + ( m->field.widthInChars*w ) ) / 2;
		m->generic.bottom = m->generic.y + h;
		m->generic.left   = m->generic.x - ( ( m->generic.right - m->generic.x ) );
		m->generic.top    = m->generic.y;
	}
	else {
		m->generic.left   = m->generic.x - l;
		m->generic.top    = m->generic.y;
		m->generic.right  = m->generic.x + w + m->field.widthInChars*w;
		m->generic.bottom = m->generic.y + h;
	}

	if ( m->generic.name ) {
		m->generic.bottom += MENU_BUTTON_MED_HEIGHT + 4;
	}
}

/*
==================
MenuField_Draw
==================
*/
void MenuField_Draw( menufield_s *f )
{
	int			x;
	int			y;
	int			w;
	int			h;
	int			style;
	qboolean	focus;
	int			color,titleColor;
	menuframework_s *menu;
	int offset;

	x =	f->generic.x;
	y =	f->generic.y;

	if ( f->generic.name )
		offset = MENU_BUTTON_MED_HEIGHT + 4;
	else
		offset = 0;

	if (f->field.style & UI_TINYFONT)
	{
		w = TINYCHAR_WIDTH;
		h = TINYCHAR_HEIGHT;
		//style = UI_TINYFONT;
		style = f->field.style;
	}
	else if (f->field.style & UI_BIGFONT)
	{
		w = BIGCHAR_WIDTH;
		h = BIGCHAR_HEIGHT;
		//style = UI_BIGFONT;
		style = f->field.style;
	}
	else if (f->field.style & UI_GIANTFONT)
	{
		w = GIANTCHAR_WIDTH;
		h = GIANTCHAR_HEIGHT;
		//style = UI_GIANTFONT;
		style = f->field.style;
	}	
	else 
	{
		w = SMALLCHAR_WIDTH;
		h = SMALLCHAR_HEIGHT;
		//style = UI_SMALLFONT;
		style = f->field.style;
	}


	if (Menu_ItemAtCursor( f->generic.parent ) == f) 
	{
		focus = qtrue;
		style |= UI_SHOWCOLOR;
	}
	else {
		focus = qfalse;
	}

	if ( f->generic.name && ( f->field.style & UI_CENTER ) ) {
		int width;
		int	color;
		
		width = ( ( (MENU_BUTTON_MED_HEIGHT*2) - 16 ) + (strlen(f->generic.name) * w) + 2*w);

		//Draw a bar to place the title
		if ( focus )
			trap_R_SetColor( colorTable[CT_LTPURPLE1] );
		else if ( f->generic.flags & QMF_GRAYED )
			trap_R_SetColor( colorMdGrey );
		else
			trap_R_SetColor( colorTable[CT_DKPURPLE1] );

		UI_DrawHandlePic( x - ( width >> 1), y, 19, 19, uis.graphicButtonLeftEnd );
		UI_DrawHandlePic( ( x + ( width >> 1) ) - 19, y, -19, 19, uis.graphicButtonLeftEnd );
		UI_DrawHandlePic( ( x - ( width >> 1) ) + (19-8), y, width - ( (19*2) - 16 ), 19, uis.whiteShader );
	
		if ( focus )
			color = CT_WHITE;
		else
			color = CT_BLACK;

		UI_DrawProportionalString( x, y + 2, f->generic.name, UI_CENTER | UI_SMALLFONT, colorTable[color] );
	}

	if (f->generic.flags & QMF_GRAYED)
	{
		color = CT_DKGREY;
	}
	else if (focus)
	{
		color = f->field.textcolor2;
	}
	else
	{
		color = f->field.textcolor;
	}

	// draw higlighted box
	if ( focus )
	{
		UI_FillRect( f->generic.left, f->generic.top+offset, f->generic.right-f->generic.left+1, 
			(f->generic.bottom-f->generic.top+1)-offset, listbar_color ); 

		// Print description
		if (menu_button_text[f->field.titleEnum][1])
		{
			menu = f->generic.parent;
			UI_DrawProportionalString( menu->descX, menu->descY, menu_button_text[f->field.titleEnum][1], UI_LEFT|UI_TINYFONT, colorTable[CT_BLACK]);
		}
	}

	if ( f->field.titleEnum ) 
	{
		if (f->field.titlecolor)
		{ 
			titleColor = f->field.titlecolor;
		}
		else
		{ 
			titleColor = CT_BLACK;
		}

		if ( f->generic.flags & QMF_GRAYED )
			titleColor = CT_DKGREY;

		UI_DrawProportionalString(  x - 5, y, menu_button_text[f->field.titleEnum][0],UI_RIGHT | UI_SMALLFONT, colorTable[titleColor]);	
	}// TiM 5 = 10 previously

	//TiM: This is leftover from Quake III.  We'll be using this variable elsewhere now :)
	/*if ( f->generic.name ) 
	{
		UI_DrawString( x - w, y, f->generic.name, style|UI_RIGHT, colorTable[color] );
	}*/

	MField_Draw( &f->field, x, y+offset, style, colorTable[color],focus );

}

/*
==================
MenuField_Key
==================
*/
sfxHandle_t MenuField_Key( menufield_s* m, int* key )
{
	int keycode;

	keycode = *key;

	switch ( keycode )
	{
		case K_KP_ENTER:
		case K_ENTER:
		case K_JOY1:
		case K_JOY2:
		case K_JOY3:
		case K_JOY4:
			// have enter go to next cursor point
			*key = K_TAB;
			break;

		case K_TAB:
		case K_KP_DOWNARROW:
		case K_DOWNARROW:
		case K_KP_UPARROW:
		case K_UPARROW:
			break;

		default:
			if ( keycode & K_CHAR_FLAG )
			{
				keycode &= ~K_CHAR_FLAG;

				if ((m->generic.flags & QMF_UPPERCASE) && Q_islower( keycode ))
					keycode -= 'a' - 'A';
				else if ((m->generic.flags & QMF_LOWERCASE) && Q_isupper( keycode ))
					keycode -= 'A' - 'a';
				else if ((m->generic.flags & QMF_NUMBERSONLY) && Q_isalpha( keycode ))
					return (menu_buzz_sound);

				MField_CharEvent( &m->field, keycode);
			}
			else
				MField_KeyDownEvent( &m->field, keycode );

			//TiM - Send a callback call just in case
			if ( m->generic.callback != NULL ) 
				m->generic.callback( &m, QM_ACTIVATED );

			break;
	}

	return (0);
}


