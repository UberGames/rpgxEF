// Copyright (C) 1999-2000 Id Software, Inc.
//
/**********************************************************************
	UI_QMENU.C

	Quake's menu framework system.
**********************************************************************/
#include "ui_local.h"

sfxHandle_t menu_in_sound;
sfxHandle_t menu_move_sound;
sfxHandle_t menu_out_sound;
sfxHandle_t menu_buzz_sound;
sfxHandle_t menu_null_sound;

vec4_t menu_text_color	    = {1.0, 1.0, 1.0, 1.0};
vec4_t menu_dim_color       = {0.0, 0.0, 0.0, 0.75};
vec4_t color_white	        = {1.00, 1.00, 1.00, 1.00};
vec4_t color_yellow	        = {1.00, 1.00, 0.00, 1.00};
vec4_t color_blue	        = {0.00, 0.00, 1.00, 1.00};
vec4_t color_lightOrange    = {1.00, 0.68, 0.00, 1.00 };
vec4_t color_orange	        = {1.00, 0.43, 0.00, 1.00};
vec4_t color_red		    = {1.00, 0.00, 0.00, 1.00};
vec4_t color_dim		    = {0.00, 0.00, 0.00, 0.25};

// current color scheme
vec4_t pulse_color          = {1.00, 1.00, 1.00, 1.00};
vec4_t text_color_disabled  = {0.50, 0.50, 0.50, 1.00};	// light gray
vec4_t text_color_normal	= {1.00, 0.43, 0.00, 1.00};	// light orange
vec4_t text_color_highlight = {1.00, 1.00, 0.00, 1.00};	// bright yellow
vec4_t listbar_color        = {1.00, 0.43, 0.00, 0.30};	// transluscent orange
vec4_t text_color_status    = {1.00, 1.00, 1.00, 1.00};	// bright white	

// action widget
static void	Action_Init( menuaction_s *a );
static void	Action_Draw( menuaction_s *a );

// radio button widget
static void	RadioButton_Init( menuradiobutton_s *rb );
static void	RadioButton_Draw( menuradiobutton_s *rb );
static sfxHandle_t RadioButton_Key( menuradiobutton_s *rb, int key );

// slider widget
static void Slider_Init( menuslider_s *s );
static sfxHandle_t Slider_Key( menuslider_s *s, int key );
static void	Slider_Draw( menuslider_s *s );

// spin control widget
static void	SpinControl_Init( menulist_s *s );
static void	SpinControl_Draw( menulist_s *s );
static sfxHandle_t SpinControl_Key( menulist_s *l, int key );

// text widget
static void Text_Init( menutext_s *b );

// scrolllist widget
static void	ScrollList_Init( menulist_s *l );
sfxHandle_t ScrollList_Key( menulist_s *l, int key );

// proportional text widget
static void PText_Init( menutext_s *b );
static void PText_Draw( menutext_s *b );

// proportional banner text widget
static void BText_Init( menutext_s *b );
static void BText_Draw( menutext_s *b );

const char *menu_normal_text[MNT_MAX]; 

// BUTTON TEXT and DESCRIPTIONS
const char *menu_button_text[MBT_MAX][2];

/*
=================
Text_Init
=================
*/
static void Text_Init( menutext_s *t )
{
	int	x;
	int	y;
	int	w=0,w2,w3;
	int	h,lines;
	char	buff[512];	
	char	buff2[512];	
	char	buff3[512];	

	buff[0] = '\0';
	buff2[0] = '\0';
	buff3[0] = '\0';

	if (t->focusX)
	{
		x = t->focusX;
		y = t->focusY;
		w = t->focusWidth;
		h =	t->focusHeight;
	}
	else
	{
		// Button text (text can be clicked on)
		if (t->buttontextEnum)
		{
			w = w2 = w3 = 0;

			if (menu_button_text[t->buttontextEnum][0])
			{
				strcpy(buff,menu_button_text[t->buttontextEnum][0]);
				w = UI_ProportionalStringWidth( buff,t->style);
			}

			if (t->buttontextEnum2)
			{
				if (menu_button_text[t->buttontextEnum2][0])
				{
					strcpy(buff2,menu_button_text[t->buttontextEnum2][0]);
					w2 = UI_ProportionalStringWidth( buff2,t->style);
				}
			}

			if (t->buttontextEnum3)
			{
				if (menu_button_text[t->buttontextEnum3][0])
				{
					strcpy(buff3,menu_button_text[t->buttontextEnum3][0]);
					w3 = UI_ProportionalStringWidth( buff3,t->style);
				}
			}

			// Which string is the longest
			if ((w > w2) && (w > w3))
			{
				;
			}
			else if ((w2 > w) && (w2 > w3))
			{
				w = w2;
			}
			else if ((w3 > w) && (w3 > w2))
			{
				w = w3;
			}
		}

		// Normal text
		else if (t->normaltextEnum)
		{
			w = w2 = w3 = 0;

			// Get longest string for width 
			if (menu_normal_text[t->normaltextEnum])
			{
				strcpy(buff,menu_normal_text[t->normaltextEnum]);
				w = UI_ProportionalStringWidth( buff,t->style);
			}

			if (t->normaltextEnum2)
			{
				if (menu_normal_text[t->normaltextEnum2])
				{
					strcpy(buff2,menu_normal_text[t->normaltextEnum2]);
					w2 = UI_ProportionalStringWidth( buff2,t->style);
				}
			}

			if (t->normaltextEnum3)
			{
				if (menu_normal_text[t->normaltextEnum3])
				{
					strcpy(buff3,menu_normal_text[t->normaltextEnum3]);
					w3 = UI_ProportionalStringWidth( buff3,t->style);
				}
			}

			// Which string is the longest
			if ((w > w2) && (w > w3))
			{
				;
			}
			else if ((w2 > w) && (w2 > w3))
			{
				w = w2;
			}
			else if ((w3 > w) && (w3 > w2))
			{
				w = w3;
			}
		}
		// possible label
		else if (t->generic.name)
		{
			strcpy(buff,t->generic.name);

			// Get width
			w = UI_ProportionalStringWidth( buff,t->style);
		}

		x = t->generic.x;
		y = t->generic.y;


		lines = 0;
		if (buff2[0] != '\0')
		{
			++lines;
			if (buff3[0] != '\0')
			{
				++lines;
			}
		}

		// Get height
		if (t->style & UI_TINYFONT)
		{
			h = PROP_TINY_HEIGHT;
		}
		else if (t->style & UI_SMALLFONT)
		{
			h = SMALLCHAR_HEIGHT;
		}
		else if (t->style & UI_BIGFONT)
		{
			h = PROP_BIG_HEIGHT;
		}
		else
		{
			h = SMALLCHAR_HEIGHT;
		}

		h += (lines * (h * 1.25));
	}

	if (t->generic.flags & QMF_RIGHT_JUSTIFY)
	{
		x = x - w;
	}
	else if (t->generic.flags & QMF_CENTER_JUSTIFY)
	{
		x = x - w/2;
	}

	t->generic.left   = x;
	t->generic.right  = x + w;
	t->generic.top    = y;
	t->generic.bottom = y + h;
}

/*
=================
Text_Draw
=================
*/
//static void Text_Draw( menutext_s *t )
static void Text_Draw(menuframework_s *menu, menutext_s *t )
{
	int		x;
	int		y,incY;
	char	buff[512];	
	char	buff2[512];	
	char	buff3[512];	
	float	*color;

	x = t->generic.x;
	y = t->generic.y;

	buff[0] = '\0';
	buff2[0] = '\0';
	buff3[0] = '\0';

	// Button text (text can be clicked on)
	if (t->buttontextEnum)
	{
		strcpy(buff,menu_button_text[t->buttontextEnum][0]);

		if (t->buttontextEnum2)
		{
			strcpy(buff2,menu_button_text[t->buttontextEnum2][0]);
		}

		if (t->buttontextEnum3)
		{
			strcpy(buff3,menu_button_text[t->buttontextEnum3][0]);
		}
	}
	// Normal text
	else if (t->normaltextEnum)
	{
		strcpy(buff,menu_normal_text[t->normaltextEnum]);
		if (t->normaltextEnum2)
		{
			strcpy(buff2,menu_normal_text[t->normaltextEnum2]);
		}

		if (t->normaltextEnum3)
		{
			strcpy(buff3,menu_normal_text[t->normaltextEnum3]);
		}
	}
	// possible label
	else if (t->generic.name)
	{
		strcpy(buff,t->generic.name);
	}

	// possible value
	if (t->string)
	{
		strcat(buff,t->string);
	}

	// Is the text grayed
	if (t->generic.flags & QMF_GRAYED)
	{
		color = colorTable[CT_DKGREY];
	}
	else if (((t->generic.flags & QMF_HIGHLIGHT_IF_FOCUS) && (Menu_ItemAtCursor( t->generic.parent ) == t)) ||
		(t->generic.flags & QMF_HIGHLIGHT))
	{
		if (t->color2)
		{
			color = t->color2;
		}
		else
		{
			color = t->color;
		}

		// If there's a description for this text, print it
		if (t->buttontextEnum)
		{
			if (menu_button_text[t->buttontextEnum][1])
			{
				UI_DrawProportionalString( menu->descX, menu->descY, menu_button_text[t->buttontextEnum][1], UI_LEFT|UI_TINYFONT, colorTable[CT_BLACK]);
			}
		}
	}
	else
	{
		color = t->color;
	}

	UI_DrawProportionalString( x, y, buff, t->style, color );

	if (t->style & UI_TINYFONT)
	{
		incY=PROP_TINY_HEIGHT * 1.15;
	}
	else if (t->style & UI_SMALLFONT)
	{
		incY=PROP_HEIGHT * 1.15;
	}
	else //if (t->style & UI_BIGFONT)
	{
		incY=PROP_BIG_HEIGHT * 1.15;
	}

	// A second line?
	if (buff2[0]!='\0')
	{
		y +=incY;
		UI_DrawProportionalString( x, y, buff2, t->style, color);
	}

	// A third?
	if (buff3[0]!='\0')
	{
		y +=incY;
		UI_DrawProportionalString( x, y, buff3, t->style, color );
	}


}

/*
=================
BText_Init
=================
*/
static void BText_Init( menutext_s *t )
{
	t->generic.flags |= QMF_INACTIVE;
}

/*
=================
BText_Draw
=================
*/
static void BText_Draw( menutext_s *t )
{
	int		x;
	int		y;
	float*	color;

	x = t->generic.x;
	y = t->generic.y;

	if (t->generic.flags & QMF_GRAYED)
		color = text_color_disabled;
	else
		color = t->color;

	UI_DrawBannerString( x, y, t->string, t->style, color );
}

/*
=================
PText_Init
=================
*/
static void PText_Init( menutext_s *t )
{
	int	x;
	int	y;
	int	w;
	int	h;
	float	sizeScale;

	sizeScale = UI_ProportionalSizeScale( t->style );

	x = t->generic.x;
	y = t->generic.y;
	w = UI_ProportionalStringWidth( t->string,UI_SMALLFONT ) * sizeScale;
	h =	PROP_HEIGHT * sizeScale;

	if( t->generic.flags & QMF_RIGHT_JUSTIFY ) {
		x -= w;
	}
	else if( t->generic.flags & QMF_CENTER_JUSTIFY ) {
		x -= w / 2;
	}

	t->generic.left   = x - PROP_GAP_WIDTH * sizeScale;
	t->generic.right  = x + w + PROP_GAP_WIDTH * sizeScale;
	t->generic.top    = y;
	t->generic.bottom = y + h;
}

/*
=================
PText_Draw
=================
*/
static void PText_Draw( menutext_s *t )
{
	int		x;
	int		y;
	float *	color;
	int		style;

	x = t->generic.x;
	y = t->generic.y;

	if (t->generic.flags & QMF_GRAYED)
		color = text_color_disabled;
	else
		color = t->color;

	style = t->style;
	if( t->generic.flags & QMF_PULSEIFFOCUS ) {
		if( Menu_ItemAtCursor( t->generic.parent ) == t ) {
			style |= UI_PULSE;
		}
		else {
			style |= UI_INVERSE;
		}
	}

	if ((t->generic.flags & QMF_HIGHLIGHT_IF_FOCUS )  && (Menu_ItemAtCursor( t->generic.parent ) == t))
	{
		color = t->color2;
	}

	UI_DrawProportionalString( x, y, t->string, style, color );
}

/*
=================
Bitmap_Init
=================
*/
void Bitmap_Init( menubitmap_s *b )
{
	int	x;
	int	y;
	int	w;
	int	h;

	x = b->generic.x;
	y = b->generic.y;
	w = b->width;
	h =	b->height;
	if( w < 0 ) {
		w = -w;
	}
	if( h < 0 ) {
		h = -h;
	}

	if (b->generic.flags & QMF_RIGHT_JUSTIFY)
	{
		x = x - w;
	}
	else if (b->generic.flags & QMF_CENTER_JUSTIFY)
	{
		x = x - w/2;
	}

	b->generic.left   = x;
	b->generic.right  = x + w;
	b->generic.top    = y;
	b->generic.bottom = y + h;

	b->shader      = 0;
	b->focusshader = 0;
}

/*
=================
Bitmap_Draw
=================
*/
void Bitmap_Draw( menubitmap_s *b )
{
	float	x;
	float	y;
	float	w;
	float	h;
	vec4_t	tempcolor;
	float*	color;
	int		colorI;
	char highlight;
	int		textStyle,incY;
	menuframework_s *menu;

	textStyle = b->textStyle;

	x = b->generic.x;
	y = b->generic.y;
	w = b->width;
	h =	b->height;

	menu = b->generic.parent;
	highlight = 0;

	if (b->generic.flags & QMF_RIGHT_JUSTIFY)
	{
		x = x - w;
	}
	else if (b->generic.flags & QMF_CENTER_JUSTIFY)
	{
		x = x - w/2;
	}

	// used to refresh shader
	if (b->generic.name && !b->shader)
	{
		b->shader = trap_R_RegisterShaderNoMip( b->generic.name );

//		if (!b->shader && b->errorpic)
//		{
///			b->shader = trap_R_RegisterShaderNoMip( b->errorpic );
//		}
	}

	if (b->focuspic && !b->focusshader)
	{
		b->focusshader = trap_R_RegisterShaderNoMip( b->focuspic );
	}

	if (b->generic.flags & QMF_GRAYED)
	{
		if (b->shader)
		{
			trap_R_SetColor( colorMdGrey );
			UI_DrawHandlePic( x, y, w, h, b->shader );
			trap_R_SetColor( NULL );
		}
	}
	else if (((b->generic.flags & QMF_HIGHLIGHT_IF_FOCUS) && (Menu_ItemAtCursor( b->generic.parent ) == b)) ||
		(b->generic.flags & QMF_HIGHLIGHT))
	{	
		if (b->color2)
		{
			trap_R_SetColor( colorTable[b->color2]);
			highlight = 1;
		}

		if ( b->shader ) {
			UI_DrawHandlePic( x, y, w, h, b->shader );
		}

		// If there's a description for this bitmap, print it
		if (menu_button_text[b->textEnum][1])
		{
			UI_DrawProportionalString( menu->descX, menu->descY, menu_button_text[b->textEnum][1], UI_LEFT|UI_TINYFONT, colorTable[CT_BLACK]);
		}

		trap_R_SetColor( NULL );
	}
	else
	{
		if (b->shader)
		{
			if (b->color)
			{
				trap_R_SetColor( colorTable[b->color]);
			}

			UI_DrawHandlePic( x, y, w, h, b->shader );
		}

		if ((b->generic.flags & QMF_PULSE) || ((b->generic.flags & QMF_PULSEIFFOCUS) && (Menu_ItemAtCursor( b->generic.parent ) == b)))
		{	
			if (b->focuscolor)			
			{
				tempcolor[0] = b->focuscolor[0];
				tempcolor[1] = b->focuscolor[1];
				tempcolor[2] = b->focuscolor[2];
				color        = tempcolor;	
			}
			else
				color = pulse_color;
			color[3] = 0.5+0.5*sin(uis.realtime/PULSE_DIVISOR);

			trap_R_SetColor( color );
			UI_DrawHandlePic( x, y, w, h, b->focusshader );
			trap_R_SetColor( NULL );
		}
		else if ((b->generic.flags & QMF_HIGHLIGHT) || ((b->generic.flags & QMF_HIGHLIGHT_IF_FOCUS) && (Menu_ItemAtCursor( b->generic.parent ) == b)))
		{	
			if (b->focuscolor)
			{
				trap_R_SetColor( b->focuscolor );
				UI_DrawHandlePic( x, y, w, h, b->focusshader );
				trap_R_SetColor( NULL );
			}
			else
				UI_DrawHandlePic( x, y, w, h, b->focusshader );
		}
	}


	// If there's text to go on top of the bitmap
	if ((b->textEnum) || (b->textPtr))
	{
		// Bitmap is highlighted, use textcolor2
		if (highlight)
		{
			colorI = ( b->textcolor2 ) ? b->textcolor2 : CT_WHITE;
		}
		// Bitmap is not highlighted, use textcolor
		else
		{
			colorI = ( b->textcolor ) ? b->textcolor : CT_WHITE;
		}

		// Draw button text
		if ((b->textEnum) && (menu_button_text[b->textEnum][0]))
		{
			UI_DrawProportionalString( x + b->textX, y + b->textY , menu_button_text[b->textEnum][0], textStyle, colorTable[colorI]);
		}
		else if (b->textPtr)
		{
			UI_DrawProportionalString( x + b->textX, y + b->textY , b->textPtr, textStyle, colorTable[colorI]);
		}


		// Is there a 2nd line of button text?
		if (b->textEnum2)
		{
			if (textStyle & UI_SMALLFONT)
			{
				incY=PROP_HEIGHT * 1.15;
			}
			else if (textStyle & UI_TINYFONT)
			{
				incY=PROP_TINY_HEIGHT * 1.15;
			}
			else
			{
				incY=PROP_HEIGHT * 1.15;
			}

			// Print second line of text
			UI_DrawProportionalString( x + b->textX, y + b->textY + incY , menu_button_text[b->textEnum2][0], textStyle, colorTable[colorI]);
		}
	}

}

/*
=================
Action_Init
=================
*/
static void Action_Init( menuaction_s *a )
{
	int x,y,w,h;
/*	int	len;

	// calculate bounds
	if (a->generic.name)
		len = strlen(a->generic.name);
	else
		len = 0;

	// left justify text
	a->generic.left   = a->generic.x; 
	a->generic.right  = a->generic.x + len*BIGCHAR_WIDTH;
	a->generic.top    = a->generic.y;
	a->generic.bottom = a->generic.y + BIGCHAR_HEIGHT;
*/
	x = a->generic.x;
	y = a->generic.y;
	w = a->width;
	h =	a->height;

	if (a->generic.flags & QMF_RIGHT_JUSTIFY)
	{
		x = x - w;
	}
	else if (a->generic.flags & QMF_CENTER_JUSTIFY)
	{
		x = x - w/2;
	}

	a->generic.left   = x;
	a->generic.right  = x + w;
	a->generic.top    = y;
	a->generic.bottom = y + h;



}

/*
=================
Action_Draw
=================
*/
static void Action_Draw( menuaction_s *a )
{
	int		x, y;
	int		style;
//	float*	color;
	int		textColor,buttonColor;

	style = 0;
	if ( a->generic.flags & QMF_GRAYED )
	{
		buttonColor = a->color3;
		textColor = a->textcolor3;
	}
	else if (( a->generic.flags & QMF_PULSEIFFOCUS ) && ( a->generic.parent->cursor == a->generic.menuPosition ))
	{
		buttonColor = a->color2;
		textColor = a->textcolor2;
		style = UI_PULSE;
	}
	else if (( a->generic.flags & QMF_HIGHLIGHT_IF_FOCUS ) && ( a->generic.parent->cursor == a->generic.menuPosition ))
	{
		buttonColor = a->color2;
		textColor = a->textcolor2;
	}
	else if ( a->generic.flags & QMF_BLINK )
	{
		if ( (uis.realtime/BLINK_DIVISOR) & 1 )
		{
			buttonColor = a->color;
			textColor = a->textcolor;
		}
		else
		{
			buttonColor = a->color2;
			textColor = a->textcolor2;
		}
		style = UI_BLINK;
	}
	else	// Just plain old normal
	{
		buttonColor = a->color2;
		textColor = a->textcolor2;
	}

	x = a->generic.x;
	y = a->generic.y;

//	UI_DrawString( x, y, a->generic.name, UI_LEFT|style, color );
	trap_R_SetColor( colorTable[buttonColor]);
	UI_DrawHandlePic(x,y, a->width, a->height, uis.whiteShader);


//	if ( a->generic.parent->cursor == a->generic.menuPosition )
//	{
//		// draw cursor
//		UI_DrawChar( x - BIGCHAR_WIDTH, y, 13, UI_LEFT|UI_BLINK, color);
//	}

	// Draw button text
	if (menu_button_text[a->textEnum][0])
	{
		UI_DrawProportionalString( x + a->textX, y + a->textY, menu_button_text[a->textEnum][0], style | UI_SMALLFONT, colorTable[textColor] );
	}

	// Is there a 2nd line of text?
	if (a->textEnum2)
	{
		int		incY;
		if (style & UI_SMALLFONT)
		{
			incY=PROP_HEIGHT * 1.15;
		}
		else //if (style & UI_TINYFONT)
		{
			incY=PROP_TINY_HEIGHT * 1.15;
		}

		// Print second line of text
		UI_DrawProportionalString( x + a->textX, y + a->textY + incY , menu_button_text[a->textEnum2][0], style, colorTable[textColor]);
	}

}

/*
=================
RadioButton_Init
=================
*/
static void RadioButton_Init( menuradiobutton_s *rb )
{
	int	len;

	// calculate bounds
	if (rb->generic.name)
		len = strlen(rb->generic.name);
	else
		len = 0;

	rb->generic.left   = rb->generic.x - (len+1)*SMALLCHAR_WIDTH;
	rb->generic.right  = rb->generic.x + 6*SMALLCHAR_WIDTH;
	rb->generic.top    = rb->generic.y;
	rb->generic.bottom = rb->generic.y + SMALLCHAR_HEIGHT;
}

/*
=================
RadioButton_Key
=================
*/
static sfxHandle_t RadioButton_Key( menuradiobutton_s *rb, int key )
{
	switch (key)
	{
		case K_MOUSE1:
			if (!(rb->generic.flags & QMF_HASMOUSEFOCUS))
				break;

		case K_JOY1:
		case K_JOY2:
		case K_JOY3:
		case K_JOY4:
		case K_ENTER:
		case K_KP_ENTER:
		case K_KP_LEFTARROW:
		case K_LEFTARROW:
		case K_KP_RIGHTARROW:
		case K_RIGHTARROW:
			rb->curvalue = !rb->curvalue;
			if ( rb->generic.callback )
				rb->generic.callback( rb, QM_ACTIVATED );

			return (menu_move_sound);
	}

	// key not handled
	return 0;
}

/*
=================
RadioButton_Draw
=================
*/
static void RadioButton_Draw( menuradiobutton_s *rb )
{
	int	x;
	int y;
	float *color;
	int	style;
	qboolean focus;

	x = rb->generic.x;
	y = rb->generic.y;

	focus = (rb->generic.parent->cursor == rb->generic.menuPosition);

	if ( rb->generic.flags & QMF_GRAYED )
	{
		color = text_color_disabled;
		style = UI_LEFT|UI_SMALLFONT;
	}
	else if ( focus )
	{
		color = text_color_highlight;
		style = UI_LEFT|UI_PULSE|UI_SMALLFONT;
	}
	else
	{
		color = text_color_normal;
		style = UI_LEFT|UI_SMALLFONT;
	}

	if ( focus )
	{
		// draw cursor
		UI_FillRect( rb->generic.left, rb->generic.top, rb->generic.right-rb->generic.left+1, rb->generic.bottom-rb->generic.top+1, listbar_color ); 
		UI_DrawChar( x, y, 13, UI_CENTER|UI_BLINK|UI_SMALLFONT, color);
	}

	if ( rb->generic.name )
		UI_DrawString( x - SMALLCHAR_WIDTH, y, rb->generic.name, UI_RIGHT|UI_SMALLFONT, color, qtrue );

	if ( !rb->curvalue )
	{
		UI_DrawHandlePic( x + SMALLCHAR_WIDTH, y + 2, 16, 16, uis.rb_off);
		UI_DrawString( x + SMALLCHAR_WIDTH + 16, y, "off", style, color, qtrue );
	}
	else
	{
		UI_DrawHandlePic( x + SMALLCHAR_WIDTH, y + 2, 16, 16, uis.rb_on );
		UI_DrawString( x + SMALLCHAR_WIDTH + 16, y, "on", style, color, qtrue );
	}
}

/*
=================
Slider_Init
=================
*/
static void Slider_Init( menuslider_s *s )
{
	// Labels go on the left side.
	if (s->picX)
	{
		s->generic.left = s->picX;
		s->generic.right = s->generic.x + s->focusWidth;
		s->generic.top  = s->picY;
		s->generic.bottom = s->generic.y + s->focusHeight;
	}
	else
	{
		s->generic.left = s->generic.x;
		s->generic.right = s->generic.x + s->focusWidth;
		s->generic.top    = s->generic.y;
		s->generic.bottom = s->generic.y + s->focusHeight;
	}

	if (s->thumbName)
	{
		s->thumbShader = trap_R_RegisterShaderNoMip( s->thumbName);
	}

	if (s->picName)
	{
		s->picShader = trap_R_RegisterShaderNoMip( s->picName);
	}

	if (s->generic.name)
	{
		s->shader = trap_R_RegisterShaderNoMip( s->generic.name);
	}
}

/*
=================
Slider_Key
=================
*/
static sfxHandle_t Slider_Key( menuslider_s *s, int key )
{
	sfxHandle_t	sound;
	int			x;
	int			oldvalue;

	switch (key)
	{
		case K_MOUSE1:
			x           = uis.cursorx - s->generic.x;
			oldvalue    = s->curvalue;
			s->curvalue = (x/(float)(s->focusWidth)) * (s->maxvalue-s->minvalue) + s->minvalue;

			//TiM | If a default was set, reset it in this case
			if ( x < 0 && s->defvalue ) {
				s->curvalue = s->defvalue;
				sound = menu_move_sound;
				break;
			}

			s->mouseDown = qtrue;

			if (s->curvalue < s->minvalue)
				s->curvalue = s->minvalue;
			else if (s->curvalue > s->maxvalue)
				s->curvalue = s->maxvalue;
			if (s->curvalue != oldvalue)
				sound = menu_move_sound;
			else
				sound = 0;
			break;

		case K_KP_LEFTARROW:
		case K_LEFTARROW:
			if (s->curvalue > s->minvalue)
			{
				s->curvalue--;
				sound = menu_move_sound;
			}
			else
				sound = menu_buzz_sound;
			break;			

		case K_KP_RIGHTARROW:
		case K_RIGHTARROW:
			if (s->curvalue < s->maxvalue)
			{
				s->curvalue++;
				sound = menu_move_sound;
			}
			else
				sound = menu_buzz_sound;
			break;			

		default:
			// key not handled
			sound = 0;
			break;
	}

	if ( sound && s->generic.callback )
		s->generic.callback( s, QM_ACTIVATED );

	return (sound);
}

#if 1
/*
=================
Slider_Draw
=================
*/
static void Slider_Draw( menuslider_s *s ) 
{
	int color;
	int	style;
	int textColor=CT_RED;
	int	thumbColor=CT_LTGREY,backgroundColor=CT_DKGREY;
	int	focus,thumbX;
	

	focus = 0;

	if ( s->generic.flags & QMF_GRAYED )
	{
		color = CT_DKGREY;
		style = UI_LEFT;
	}
	else if (Menu_ItemAtCursor( s->generic.parent ) == s)
	{
		color = s->color2;
		style = UI_LEFT|UI_PULSE;
		textColor = s->textcolor2;
		thumbColor = s->thumbColor2;
		backgroundColor = CT_LTGREY;
		focus = 1;
	}
	else
	{
		color = s->color;
		style = UI_LEFT;
		textColor = s->textcolor;
		thumbColor = s->thumbColor;
	}

	//TiM - Reset 
	if ( !trap_Key_IsDown( K_MOUSE1 ) && s->mouseDown ) {
		s->mouseDown = qfalse;
		s->generic.parent->noNewSelecting = qfalse;
	}

	if ( s->mouseDown ) {
		s->curvalue = ((uis.cursorx - s->generic.x)/(float)(s->focusWidth)) * (s->maxvalue-s->minvalue) + s->minvalue;
		s->curvalue = Com_Clamp( s->minvalue, s->maxvalue, s->curvalue);

		s->generic.parent->noNewSelecting = qtrue;
	}

	// Print pic
	if (s->picShader)
	{
		trap_R_SetColor( colorTable[color]);
		UI_DrawHandlePic( s->picX , s->picY,  s->picWidth,  s->picHeight, s->picShader);
		trap_R_SetColor( NULL );
	}

	// Button text
	if (s->textEnum)
	{
		UI_DrawProportionalString(  s->picX + s->textX,  s->picY + s->textY,menu_button_text[s->textEnum][0], UI_SMALLFONT, colorTable[textColor] );
	}

	if ((focus) && (menu_button_text[s->textEnum][1]))
	{
		UI_DrawProportionalString( s->generic.parent->descX, s->generic.parent->descY, menu_button_text[s->textEnum][1], UI_LEFT|UI_TINYFONT, colorTable[CT_BLACK]);
	}

	// clamp thumb
	if (s->maxvalue > s->minvalue)
	{
		s->range = ( s->curvalue - s->minvalue ) / ( float ) ( s->maxvalue - s->minvalue );
		if ( s->range < 0)
		{
			s->range = 0;
		}
		else if ( s->range > 1)
		{
			s->range = 1;
		}
	}
	else
	{
		s->range = 0;
	}

	thumbX = s->generic.x - (s->thumbGraphicWidth / 2) + (s->focusWidth * s->range);
	if (s->range)
	{
		trap_R_SetColor( colorTable[backgroundColor]);
		UI_DrawHandlePic( s->generic.x, s->generic.y, (thumbX - s->generic.x) + 4, 18, uis.whiteShader); // Hard coded height is bad
	}

	//draw slider graph
	if (s->shader)
	{
		trap_R_SetColor( colorTable[color]);
		UI_DrawHandlePic( s->generic.x, s->generic.y, s->width, s->height, s->shader); 
	}

	// Draw thumb and background
	if (s->thumbShader)
	{
		trap_R_SetColor( colorTable[thumbColor]);
		UI_DrawHandlePic( thumbX, s->generic.y, s->thumbWidth, s->thumbHeight, s->thumbShader); 
	}
}
#else
/*
=================
Slider_Draw
=================
*/
static void Slider_Draw( menuslider_s *s )
{
	float *color;
	int	style;
	int	i;
	int x;
	int y;
	qboolean focus;
	
	x =	s->generic.x;
	y = s->generic.y;
	focus = (s->generic.parent->cursor == s->generic.menuPosition);

	style = UI_SMALLFONT;
	if ( s->generic.flags & QMF_GRAYED )
	{
		color = text_color_disabled;
	}
	else if (focus)
	{
		color  = text_color_highlight;
		style |= UI_PULSE;
	}
	else
	{
		color = text_color_normal;
	}

	if ( focus )
	{
		// draw cursor
		UI_FillRect( s->generic.left, s->generic.top, s->generic.right-s->generic.left+1, s->generic.bottom-s->generic.top+1, listbar_color ); 
		UI_DrawChar( x, y, 13, UI_CENTER|UI_BLINK|UI_SMALLFONT, color);
	}

	// draw label
	UI_DrawString( x - SMALLCHAR_WIDTH, y, s->generic.name, UI_RIGHT|style, color );

	// draw slider
	UI_DrawChar( x + SMALLCHAR_WIDTH, y, 128, UI_LEFT|style, color);
	for ( i = 0; i < SLIDER_RANGE; i++ )
		UI_DrawChar( x + (i+2)*SMALLCHAR_WIDTH, y, 129, UI_LEFT|style, color);
	UI_DrawChar( x + (i+2)*SMALLCHAR_WIDTH, y, 130, UI_LEFT|style, color);

	// clamp thumb
	if (s->maxvalue > s->minvalue)
	{
		s->range = ( s->curvalue - s->minvalue ) / ( float ) ( s->maxvalue - s->minvalue );
		if ( s->range < 0)
			s->range = 0;
		else if ( s->range > 1)
			s->range = 1;
	}
	else
		s->range = 0;

	// draw thumb
	if (style & UI_PULSE) {
		style &= ~UI_PULSE;
		style |= UI_BLINK;
	}
	UI_DrawChar( (int)( x + 2*SMALLCHAR_WIDTH + (SLIDER_RANGE-1)*SMALLCHAR_WIDTH* s->range ), y, 131, UI_LEFT|style, color);
}
#endif

/*
=================
SpinControl_Init
=================
*/
static void SpinControl_Init( menulist_s *s ) 
{
	if ((!s->listnames) && (!s->itemnames))
	{
		return;
	}

	// Count number of choices for this spin control
	s->numitems=0;

	if (s->listnames)
	{
		while ( (menu_normal_text[s->listnames[s->numitems]]) != 0 )
		{
			s->numitems++;
		}
	}
	else
	{
		while ( s->itemnames[s->numitems] != 0 )
		{
			s->numitems++;
		}
	}

	if (!s->focusWidth && !s->focusHeight)
	{
		s->generic.left		= s->generic.x;
		s->generic.right	= s->generic.x + MENU_BUTTON_MED_HEIGHT + MENU_BUTTON_MED_WIDTH + MENU_BUTTON_MED_HEIGHT;
		s->generic.top		= s->generic.y;
		s->generic.bottom	= s->generic.y + MENU_BUTTON_MED_HEIGHT;
	}
	else
	{
		s->generic.left		= s->generic.x;
		s->generic.right	= s->generic.x + s->focusWidth;
		s->generic.top		= s->generic.y;
		s->generic.bottom	= s->generic.y + s->focusHeight;
	}
}

static sfxHandle_t	SpinControl_InitListRender( menulist_s* s ) {
	int	bestWidth=0;
	int i;
	int	widthOffset;
	int heightOffset;

	if ( !s->generic.parent->displaySpinList ) {
		if ( (s->generic.flags & QMF_HASMOUSEFOCUS) ) { //we clicked on the button, and the list wasn't open lol
			//in case the data was changed, init the box height data and such again
			{
				//init the data area
				memset( &s->drawList, 0, sizeof( drawList_t ) );

				//first, find the longest string in the list
				for ( i=0; i<s->numitems; i++ ) {
					if ( s->listnames ) {
						if ( UI_ProportionalStringWidth( menu_normal_text[s->listnames[i]], UI_SMALLFONT ) > bestWidth ) {
							bestWidth = UI_ProportionalStringWidth( menu_normal_text[s->listnames[i]], UI_SMALLFONT );
						}
					}
					else {
						if ( UI_ProportionalStringWidth( s->itemnames[i], UI_SMALLFONT ) > bestWidth ) {
							bestWidth = UI_ProportionalStringWidth( s->itemnames[i], UI_SMALLFONT );
						}
					}
				}

				//check for necessary offsetting
				if (!s->listX && !s->listY)
				{
					if ( s->width )
						widthOffset = s->width;
					else
						widthOffset = MENU_BUTTON_MED_WIDTH;

					widthOffset = MENU_BUTTON_MED_HEIGHT + widthOffset - 8 + MENU_BUTTON_MED_HEIGHT + 2; //4

					heightOffset = s->textX;
				}
				else {
					widthOffset = s->listX;
					heightOffset = s->listY;
				}

				//set up the co-ords
				// -1 to account for the border
				s->drawList.left = s->generic.x + widthOffset - 2;
				s->drawList.up = s->generic.y + heightOffset - 2;
				//+1 to account for the border
				s->drawList.right = s->drawList.left + bestWidth + 4;
				s->drawList.down = s->drawList.up + ( SMALLCHAR_HEIGHT * s->numitems ) + 3;

				//if the text is centered, offset the box to match
				if ( s->textFlags & UI_CENTER ) {
					int half = ( s->drawList.right - s->drawList.left) >> 1;
					s->drawList.left -= half;
					s->drawList.right -= half;
				}
				//MIGHTNEEDFIXME: UI_RIGHT...? Eh, If I need it, I'll come back here hehe

				//If we can, cause it's cool, move the list up 1/3 of it's height
				heightOffset = (float)( s->drawList.down - s->drawList.up ) * (float)0.33;
				s->drawList.up -= heightOffset;
				s->drawList.down -= heightOffset;

				//Okay, ugh... error trapping.  It's possible the box could leave the screen,
				//so we'll need to check and account for that
				if ( s->drawList.right > SCREEN_WIDTH ) {
					s->drawList.xOffset = s->drawList.right - SCREEN_WIDTH + 6; //1 for good luck heh

					s->drawList.left -= s->drawList.xOffset;
					s->drawList.right -= s->drawList.xOffset;
				}

				if ( s->drawList.down > SCREEN_HEIGHT ) {
					s->drawList.yOffset = s->drawList.down - SCREEN_HEIGHT + 6;

					s->drawList.up -= s->drawList.yOffset;
					s->drawList.down -= s->drawList.yOffset;
				}

				//also due to offsetting, it may be possible we overshot the top now... which is a prob cause it means our list is too big lol
				if ( s->drawList.up < 0 ) {
					s->drawList.yOffset = 0 - s->drawList.up;
					s->drawList.up += s->drawList.yOffset;
					s->drawList.down += s->drawList.yOffset;
				}

				if ( s->drawList.left < 0 ) {
					s->drawList.xOffset = 0 - s->drawList.left;
					s->drawList.left += s->drawList.xOffset;
					s->drawList.right += s->drawList.xOffset;
				}
			}

			//set the main menu to know to draw the box for this
			s->generic.parent->displaySpinList = s;
			s->generic.parent->noNewSelecting = qtrue;

			return menu_move_sound;
		}
	}
	else {
		//inside the box region, so select whatever we selected
		if ( UI_CursorInRect(s->drawList.left, 
								s->drawList.up, 
								(s->drawList.right - s->drawList.left), 
								(s->drawList.down - s->drawList.up ) ) 
			||
			( ( s->numitems >= MAX_LIST ) && UI_CursorInRect( s->drawList.left, 
								s->drawList.up, 
								(s->drawList.right - s->drawList.left) * 2, 
								(s->drawList.down - s->drawList.up ) ) ) )
		{
			int	selectedNum;

			selectedNum = ( uis.cursory - s->drawList.up+1 ) / SMALLCHAR_HEIGHT;
			
			if ( s->numitems > MAX_LIST && uis.cursorx > s->drawList.right && selectedNum + MAX_LIST < s->numitems ) {
				selectedNum += MAX_LIST;
			}

			if ( selectedNum < 0 ) {
				selectedNum = 0;
			}
			else if ( selectedNum > s->numitems - 1 ) {
				selectedNum = s->numitems - 1;
			}

			s->curvalue = selectedNum;

			s->generic.parent->displaySpinList = NULL;
			s->generic.parent->noNewSelecting = qfalse;

			return menu_out_sound;
		}
		else //clicked outside the box, so just cancel
		{
			s->generic.parent->displaySpinList = NULL;
			s->generic.parent->noNewSelecting = qfalse;
			return menu_out_sound;
		}
	}

	return menu_move_sound;
}

/*
=================
SpinControl_Key
=================
*/
static sfxHandle_t SpinControl_Key( menulist_s *s, int key )
{
	sfxHandle_t	sound;
	qboolean	callback=qfalse;

	sound = 0;
	switch (key)
	{
		case K_MOUSE1:
		case K_ENTER:
		case K_KP_ENTER:
			if ( !s->listshaders && !s->ignoreList ) {
				sound = SpinControl_InitListRender( s );
				if ( !s->generic.parent->displaySpinList ) {
					callback = qtrue;
				}
			}
			else { // for list shaders (ie a crosshair scoller ). not really necessary, but prob a good safety net
				s->curvalue++;
				if (s->curvalue >= s->numitems)
					s->curvalue = 0;

				sound = menu_move_sound;
				callback=qtrue;
			}
			break;
			/*if (!(s->generic.flags & QMF_HASMOUSEFOCUS))
			{
				break;
			}*/

		/*case K_ENTER:
		case K_KP_ENTER:
			s->curvalue++;
			if (s->curvalue >= s->numitems)
				s->curvalue = 0;
			sound = menu_move_sound;
			break;*/
		
		case K_KP_LEFTARROW:
		case K_LEFTARROW:
			if (s->curvalue > 0)
				s->curvalue--;
			else
				s->curvalue = s->numitems-1;

			sound = menu_move_sound;
			callback=qtrue;
				//sound = menu_buzz_sound;
			break;

		case K_KP_RIGHTARROW:
		case K_RIGHTARROW:
			if (s->curvalue < s->numitems-1)
				s->curvalue++;
			else
				s->curvalue = 0;

			sound = menu_move_sound;
			callback=qtrue;
				//sound = menu_buzz_sound;
			break;
	}

	if ( sound && s->generic.callback && callback)
		s->generic.callback( s, QM_ACTIVATED );

	return (sound);
}

/*
=================
SpinControl_Draw
=================
*/
static void SpinControl_Draw( menulist_s *s )
{
	int x,y,listX,buttonColor,buttonTextColor;
	int boxWidth,color;
	char text[64];

	x = s->generic.x;
	y =	s->generic.y;

	//Calculate width of button
	if (s->width)
	{
		boxWidth = s->width;
	}
	else
	{
		boxWidth = MENU_BUTTON_MED_WIDTH;
	}

	// Choose colors
	if (s->generic.flags & QMF_GRAYED)
	{
		buttonColor = CT_DKGREY;
		buttonTextColor = s->textcolor;
	}
	else if (Menu_ItemAtCursor( s->generic.parent ) == s)
	{
		// Button description
		if (menu_button_text[s->textEnum][1])
		{
			UI_DrawProportionalString( s->generic.parent->descX, s->generic.parent->descY, menu_button_text[s->textEnum][1], UI_LEFT|UI_TINYFONT, colorTable[CT_BLACK]);
		}

		buttonColor = s->color2;
		buttonTextColor = s->textcolor2;
	}
	else
	{
		buttonColor = s->color;
		buttonTextColor = s->textcolor;
	}

	// Print little dot to show it was updated
	if (s->updated)
	{
		trap_R_SetColor( colorTable[CT_LTGOLD1]);
		UI_DrawHandlePic(x - 10,y + 6, 8, 8, uis.graphicCircle);
	}

	if ( s->listshaders )
	{
		// Draw the shader instead of the button graphics and text 
		UI_DrawHandlePic( x, y, s->width, s->height, s->listshaders[s->curvalue] );
	}
	else
	{
		// Draw button and button text
		trap_R_SetColor( colorTable[buttonColor]);
		//if (!s->focusHeight && !s->focusWidth)
		//{
		if( s->generic.flags & QMF_ALTERNATE ) {
			UI_DrawHandlePic(x, y, boxWidth, PROP_BIG_HEIGHT, uis.whiteShader ); 
		} else if( s->generic.flags & QMF_ALTERNATE2 ) {
			UI_DrawHandlePic(x, y, boxWidth, PROP_BIG_HEIGHT, uis.graphicButtonLeft );
		} else  {
			UI_DrawHandlePic(x,y, MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);											// left
			UI_DrawHandlePic(x+ boxWidth+ MENU_BUTTON_MED_HEIGHT - 16,y, -MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);	// right
			UI_DrawHandlePic(x + MENU_BUTTON_MED_HEIGHT - 8,y, boxWidth, MENU_BUTTON_MED_HEIGHT, uis.whiteShader);										// middle
		}
		//}
		//else
		//{
		//	UI_DrawHandlePic(x ,y, s->focusWidth, s->focusHeight, uis.whiteShader);
		//}

		if (menu_button_text[s->textEnum][0])
		{
			if(s->generic.flags & QMF_ALTERNATE || s->generic.flags & QMF_ALTERNATE2)
				UI_DrawProportionalString( x + s->textX, y + s->textY, menu_button_text[s->textEnum][0], UI_TINYFONT | s->textFlags, colorTable[buttonTextColor] );
			else
				UI_DrawProportionalString( x + s->textX, y + s->textY, menu_button_text[s->textEnum][0], UI_SMALLFONT | s->textFlags, colorTable[buttonTextColor] );
		}

		// Print current value
		if (s->listnames)
		{
			if ( !strchr( menu_normal_text[s->listnames[s->curvalue]], '\n' ) )
			{
				Q_strncpyz( text, menu_normal_text[s->listnames[s->curvalue]], sizeof( text ) );

				//TiM - Cap its max chars if need be
				if ( s->maxChars && strlen( text ) >= s->maxChars ) {
					text[s->maxChars] = '\0';
					strcat( text, "..." );
				}

				if (!s->listcolor)
				{
					color = CT_WHITE;
				}
				else 
				{
					color = s->listcolor;
				}
				if (!s->listX && !s->listY)
				{
					listX = x + MENU_BUTTON_MED_HEIGHT + boxWidth - 8 + MENU_BUTTON_MED_HEIGHT + 2; //+ 4
					if(s->generic.flags & QMF_ALTERNATE || s->generic.flags & QMF_ALTERNATE2)
						UI_DrawProportionalString( listX, y + 5, text, UI_SMALLFONT | s->listFlags, colorTable[color] );
					else
						UI_DrawProportionalString( listX, y + s->textY, text, UI_SMALLFONT | s->listFlags, colorTable[color] );
				}
				else
				{
					if(s->generic.flags & QMF_ALTERNATE || s->generic.flags & QMF_ALTERNATE2)
						UI_DrawProportionalString( x+ s->listX, y + 5, text, UI_SMALLFONT | s->listFlags, colorTable[color] );
					else
						UI_DrawProportionalString( x+ s->listX, y + s->listY, text, UI_SMALLFONT | s->listFlags, colorTable[color] );
				}
			}
		}
		else if (s->itemnames[0])
		{
			if ( !strchr(s->itemnames[s->curvalue], '\n' ) )
			{
				Q_strncpyz( text, s->itemnames[s->curvalue], sizeof( text ) );

				//TiM - Cap its max chars if need be
				if ( s->maxChars && strlen( text ) >= s->maxChars ) {
					text[s->maxChars+1] = '\0';
					strcat( text, "..." );
				}

				if (!s->listcolor)
				{
					color = CT_WHITE;
				}
				else 
				{
					color = s->listcolor;
				}
				if (!s->listX && !s->listY)
				{
					listX = x + MENU_BUTTON_MED_HEIGHT + boxWidth - 8 + MENU_BUTTON_MED_HEIGHT + 2;//+ 4;
					if(s->generic.flags & QMF_ALTERNATE ||s->generic.flags & QMF_ALTERNATE2)
						UI_DrawProportionalString( listX, y + 5, text, UI_SMALLFONT | s->listFlags, colorTable[color] );
					else
						UI_DrawProportionalString( listX, y + s->textY,text, UI_SMALLFONT | s->listFlags, colorTable[color] );
				}
				else
				{
					if(s->generic.flags & QMF_ALTERNATE || s->generic.flags & QMF_ALTERNATE2)
						UI_DrawProportionalString( x+ s->listX, y + 5, text, UI_SMALLFONT | s->listFlags, colorTable[color] );
					else
						UI_DrawProportionalString( x+ s->listX, y + s->listY,text, UI_SMALLFONT | s->listFlags, colorTable[color] );
				}
			}
		}
	}

}

/*
=================
ScrollList_Init
=================
*/
static void ScrollList_Init( menulist_s *l )
{
	int		w;

	l->oldvalue = 0;
	l->curvalue = 0;
	l->top      = 0;

	if( !l->columns ) {
		l->columns = 1;
		l->seperation = 0;
	}
	else if( !l->seperation ) {
		l->seperation = 3;
	}

	w = ( (l->width + l->seperation) * l->columns - l->seperation) * SMALLCHAR_WIDTH;

	l->generic.left   =	l->generic.x;
	l->generic.top    = l->generic.y;	
	l->generic.right  =	l->generic.x + w;
	l->generic.bottom =	l->generic.y + l->height * SMALLCHAR_HEIGHT;

	if( l->generic.flags & QMF_CENTER_JUSTIFY ) {
		l->generic.left -= w / 2;
		l->generic.right -= w / 2;
	}
}

/*
=================
ScrollList_Key
=================
*/
sfxHandle_t ScrollList_Key( menulist_s *l, int key )
{
	int	x;
	int	y;
	int	w;
	int	i;
	int	j;	
	int	c;
	int	cursorx;
	int	cursory;
	int	column;
	int	index;

	switch (key)
	{
		case K_MOUSE1:
			if (l->generic.flags & QMF_HASMOUSEFOCUS)
			{
				// check scroll region
				x = l->generic.x;
				y = l->generic.y;
				w = ( (l->width + l->seperation) * l->columns - l->seperation) * SMALLCHAR_WIDTH;
				if( l->generic.flags & QMF_CENTER_JUSTIFY ) {
					x -= w / 2;
				}
				if (UI_CursorInRect( x, y, w, l->height*SMALLCHAR_HEIGHT ))
				{
					cursorx = (uis.cursorx - x)/SMALLCHAR_WIDTH;
					column = cursorx / (l->width + l->seperation);
					cursory = (uis.cursory - y)/SMALLCHAR_HEIGHT;
					index = column * l->height + cursory;
					if (l->top + index < l->numitems)
					{
						l->oldvalue = l->curvalue;
						l->curvalue = l->top + index;

						if (l->oldvalue != l->curvalue && l->generic.callback)
						{
							l->generic.callback( l, QM_GOTFOCUS );
							return (menu_move_sound);
						}
					}
				}
			
				// absorbed, silent sound effect
				return (menu_null_sound);
			}
			break;

		case K_KP_HOME:
		case K_HOME:
			l->oldvalue = l->curvalue;
			l->curvalue = 0;
			l->top      = 0;

			if (l->oldvalue != l->curvalue && l->generic.callback)
			{
				l->generic.callback( l, QM_GOTFOCUS );
				return (menu_move_sound);
			}
			return (menu_buzz_sound);

		case K_KP_END:
		case K_END:
			l->oldvalue = l->curvalue;
			l->curvalue = l->numitems-1;
			if( l->columns > 1 ) {
				c = (l->curvalue / l->height + 1) * l->height;
				l->top = c - (l->columns * l->height);
			}
			else {
				l->top = l->curvalue - (l->height - 1);
			}
			if (l->top < 0)
				l->top = 0;			

			if (l->oldvalue != l->curvalue && l->generic.callback)
			{
				l->generic.callback( l, QM_GOTFOCUS );
				return (menu_move_sound);
			}
			return (menu_buzz_sound);

		case K_PGUP:
		case K_KP_PGUP:
			if( l->columns > 1 ) {
				return menu_null_sound;
			}

			if (l->curvalue > 0)
			{
				l->oldvalue = l->curvalue;
				l->curvalue -= l->height-1;
				if (l->curvalue < 0)
					l->curvalue = 0;
				l->top = l->curvalue;
				if (l->top < 0)
					l->top = 0;

				if (l->generic.callback)
					l->generic.callback( l, QM_GOTFOCUS );

				return (menu_move_sound);
			}
			return (menu_buzz_sound);

		case K_PGDN:
		case K_KP_PGDN:
			if( l->columns > 1 ) {
				return menu_null_sound;
			}

			if (l->curvalue < l->numitems-1)
			{
				l->oldvalue = l->curvalue;
				l->curvalue += l->height-1;
				if (l->curvalue > l->numitems-1)
					l->curvalue = l->numitems-1;
				l->top = l->curvalue - (l->height-1);
				if (l->top < 0)
					l->top = 0;

				if (l->generic.callback)
					l->generic.callback( l, QM_GOTFOCUS );

				return (menu_move_sound);
			}
			return (menu_buzz_sound);

		case K_KP_UPARROW:
		case K_UPARROW:
			if( l->curvalue == 0 ) {
				return menu_buzz_sound;
			}

			l->oldvalue = l->curvalue;
			l->curvalue--;

			if( l->curvalue < l->top ) {
				if( l->columns == 1 ) {
					l->top--;
				}
				else {
					l->top -= l->height;
				}
			}

			if( l->generic.callback ) {
				l->generic.callback( l, QM_GOTFOCUS );
			}

			return (menu_move_sound);

		case K_KP_DOWNARROW:
		case K_DOWNARROW:
			if( l->curvalue == l->numitems - 1 ) {
				return menu_buzz_sound;
			}

			l->oldvalue = l->curvalue;
			l->curvalue++;

			if( l->curvalue >= l->top + l->columns * l->height ) {
				if( l->columns == 1 ) {
					l->top++;
				}
				else {
					l->top += l->height;
				}
			}

			if( l->generic.callback ) {
				l->generic.callback( l, QM_GOTFOCUS );
			}

			return menu_move_sound;

		case K_KP_LEFTARROW:
		case K_LEFTARROW:
			if( l->columns == 1 ) {
				return menu_null_sound;
			}

			if( l->curvalue < l->height ) {
				return menu_buzz_sound;
			}

			l->oldvalue = l->curvalue;
			l->curvalue -= l->height;

			if( l->curvalue < l->top ) {
				l->top -= l->height;
			}

			if( l->generic.callback ) {
				l->generic.callback( l, QM_GOTFOCUS );
			}

			return menu_move_sound;

		case K_KP_RIGHTARROW:
		case K_RIGHTARROW:
			if( l->columns == 1 ) {
				return menu_null_sound;
			}

			c = l->curvalue + l->height;

			if( c >= l->numitems ) {
				return menu_buzz_sound;
			}

			l->oldvalue = l->curvalue;
			l->curvalue = c;

			if( l->curvalue > l->top + l->columns * l->height - 1 ) {
				l->top += l->height;
			}

			if( l->generic.callback ) {
				l->generic.callback( l, QM_GOTFOCUS );
			}

			return menu_move_sound;
	}

	// cycle look for ascii key inside list items
	if ( !Q_isprint( key ) )
		return (0);

	// force to lower for case insensitive compare
	if ( Q_isupper( key ) )
	{
		key -= 'A' - 'a';
	}

	// iterate list items
	for (i=1; i<=l->numitems; i++)
	{
		j = (l->curvalue + i) % l->numitems;
		c = l->itemnames[j][0];
		if ( Q_isupper( c ) )
		{
			c -= 'A' - 'a';
		}

		if (c == key)
		{
			// set current item, mimic windows listbox scroll behavior
			if (j < l->top)
			{
				// behind top most item, set this as new top
				l->top = j;
			}
			else if (j > l->top+l->height-1)
			{
				// past end of list box, do page down
				l->top = (j+1) - l->height;
			}
			
			if (l->curvalue != j)
			{
				l->oldvalue = l->curvalue;
				l->curvalue = j;
				if (l->generic.callback)
					l->generic.callback( l, QM_GOTFOCUS );
				return ( menu_move_sound );			
			}

			return (menu_buzz_sound);
		}
	}

	return (menu_buzz_sound);
}

/*
=================
ScrollList_Draw
=================
*/
void ScrollList_Draw( menulist_s *l )
{
	int			x;
	int			u;
	int			y;
	int			i;
	int			base;
	int			column;
	float*		color;
	qboolean	hasfocus;
	int			style;

	hasfocus = (l->generic.parent->cursor == l->generic.menuPosition);

	x =	l->generic.x;
	for( column = 0; column < l->columns; column++ ) {
		y =	l->generic.y;
		base = l->top + column * l->height;
		for( i = base; i < base + l->height; i++) {
			if (i >= l->numitems)
				break;

			if (i == l->curvalue)
			{
				u = x - 2;
				if( l->generic.flags & QMF_CENTER_JUSTIFY ) {
					u -= (l->width * SMALLCHAR_WIDTH) / 2 + 1;
				}

				UI_FillRect(u,y,l->width*SMALLCHAR_WIDTH,SMALLCHAR_HEIGHT+2,listbar_color);
				color = text_color_highlight;

				if (hasfocus)
					style = UI_PULSE|UI_LEFT|UI_SMALLFONT;
				else
					style = UI_LEFT|UI_SMALLFONT;
			}
			else
			{
				color = text_color_normal;
				style = UI_LEFT|UI_SMALLFONT;
			}
			if( l->generic.flags & QMF_CENTER_JUSTIFY ) {
				style |= UI_CENTER;
			}
			if( l->generic.flags & QMF_CENTER_JUSTIFY ) {
				style |= UI_CENTER;
			}

			UI_DrawString(
				x,
				y,
				l->itemnames[i],
				style,
				color, qfalse);

			y += SMALLCHAR_HEIGHT;
		}
		x += (l->width + l->seperation) * SMALLCHAR_WIDTH;
	}
}

/*
=================
Menu_AddItem
=================
*/
void Menu_AddItem( menuframework_s *menu, void *item )
{
	menucommon_s	*itemptr;

	if (menu->nitems >= MAX_MENUITEMS)
		trap_Error ("Menu_AddItem: excessive items");

	menu->items[menu->nitems] = item;
	((menucommon_s*)menu->items[menu->nitems])->parent        = menu;
	((menucommon_s*)menu->items[menu->nitems])->menuPosition  = menu->nitems;
	((menucommon_s*)menu->items[menu->nitems])->flags        &= ~QMF_HASMOUSEFOCUS;

	// perform any item specific initializations
	itemptr = (menucommon_s*)item;
	if (!(itemptr->flags & QMF_NODEFAULTINIT))
	{
		switch (itemptr->type)
		{
			case MTYPE_ACTION:
				Action_Init((menuaction_s*)item);
				break;

			case MTYPE_FIELD:
				MenuField_Init((menufield_s*)item);
				break;

			case MTYPE_SPINCONTROL:
				SpinControl_Init((menulist_s*)item);
				break;

			case MTYPE_RADIOBUTTON:
				RadioButton_Init((menuradiobutton_s*)item);
				break;

			case MTYPE_SLIDER:
				Slider_Init((menuslider_s*)item);
				break;

			case MTYPE_BITMAP:
				Bitmap_Init((menubitmap_s*)item);
				break;

			case MTYPE_TEXT:
				Text_Init((menutext_s*)item);
				break;

			case MTYPE_SCROLLLIST:
				ScrollList_Init((menulist_s*)item);
				break;

			case MTYPE_PTEXT:
				PText_Init((menutext_s*)item);
				break;

			case MTYPE_BTEXT:
				BText_Init((menutext_s*)item);
				break;

			default:
				//Com_Printf( S_COLOR_RED "Menu_Init: unknown type %d, ID was %i\n", itemptr->type, itemptr->id );
				trap_Error( va("Menu_Init: unknown type %d, ID was %i", itemptr->type, itemptr->id) );
		}
	}

	menu->nitems++;
}

/*
=================
Menu_CursorMoved
=================
*/
void Menu_CursorMoved( menuframework_s *m )
{
	void (*callback)( void *self, int notification );
	
	if (m->cursor_prev == m->cursor)
		return;

	if (m->cursor_prev >= 0 && m->cursor_prev < m->nitems)
	{
		callback = ((menucommon_s*)(m->items[m->cursor_prev]))->callback;
		if (callback)
			callback(m->items[m->cursor_prev],QM_LOSTFOCUS);
	}
	
	if (m->cursor >= 0 && m->cursor < m->nitems)
	{
		callback = ((menucommon_s*)(m->items[m->cursor]))->callback;
		if (callback)
			callback(m->items[m->cursor],QM_GOTFOCUS);
	}
}

/*
=================
Menu_SetCursor
=================
*/
void Menu_SetCursor( menuframework_s *m, int cursor )
{
	if (((menucommon_s*)(m->items[cursor]))->flags & (QMF_GRAYED|QMF_INACTIVE))
	{
		// cursor can't go there
		return;
	}

	m->cursor_prev = m->cursor;
	m->cursor      = cursor;

	Menu_CursorMoved( m );
}

/*
=================
Menu_SetCursorToItem
=================
*/
void Menu_SetCursorToItem( menuframework_s *m, void* ptr )
{
	int	i;

	for (i=0; i<m->nitems; i++)
	{
		if (m->items[i] == ptr)
		{
			Menu_SetCursor( m, i );
			return;
		}
	}
}

/*
** Menu_AdjustCursor
**
** This function takes the given menu, the direction, and attempts
** to adjust the menu's cursor so that it's at the next available
** slot.
*/
void Menu_AdjustCursor( menuframework_s *m, int dir ) {
	menucommon_s	*item = NULL;
	qboolean		wrapped = qfalse;

wrap:
	while ( m->cursor >= 0 && m->cursor < m->nitems ) {
		item = ( menucommon_s * ) m->items[m->cursor];
		if (( item->flags & (QMF_GRAYED|QMF_MOUSEONLY|QMF_INACTIVE) ) ) {
			m->cursor += dir;
		}
		else {
			break;
		}
	}

	if ( dir == 1 ) {
		if ( m->cursor >= m->nitems ) {
			if ( m->wrapAround ) {
				if ( wrapped ) {
					m->cursor = m->cursor_prev;
					return;
				}
				m->cursor = 0;
				wrapped = qtrue;
				goto wrap;
			}
			m->cursor = m->cursor_prev;
		}
	}
	else {
		if ( m->cursor < 0 ) {
			if ( m->wrapAround ) {
				if ( wrapped ) {
					m->cursor = m->cursor_prev;
					return;
				}
				m->cursor = m->nitems - 1;
				wrapped = qtrue;
				goto wrap;
			}
			m->cursor = m->cursor_prev;
		}
	}
}

/*
=================
Menu_Draw
=================
*/
void Menu_Draw( menuframework_s *menu )
{
	int				i;
	menucommon_s	*itemptr;

	// draw menu
	for (i=0; i<menu->nitems; i++)
	{
		itemptr = (menucommon_s*)menu->items[i];

		if (itemptr->flags & QMF_HIDDEN)
			continue;

		if (itemptr->ownerdraw)
		{
			// total subclassing, owner draws everything
			itemptr->ownerdraw( itemptr );
		}	
		else 
		{
			switch (itemptr->type)
			{	
				case MTYPE_RADIOBUTTON:
					RadioButton_Draw( (menuradiobutton_s*)itemptr );
					break;

				case MTYPE_FIELD:
					MenuField_Draw( (menufield_s*)itemptr );
					break;
		
				case MTYPE_SLIDER:
					Slider_Draw( (menuslider_s*)itemptr );
					break;
 
				case MTYPE_SPINCONTROL:
					SpinControl_Draw( (menulist_s*)itemptr );
					break;
		
				case MTYPE_ACTION:
					Action_Draw( (menuaction_s*)itemptr );
					break;
		
				case MTYPE_BITMAP:
					Bitmap_Draw( (menubitmap_s*)itemptr );
					break;

				case MTYPE_TEXT:
					Text_Draw( menu,(menutext_s*)itemptr );
					break;

				case MTYPE_SCROLLLIST:
					ScrollList_Draw( (menulist_s*)itemptr );
					break;
				
				case MTYPE_PTEXT:
					PText_Draw( (menutext_s*)itemptr );
					break;

				case MTYPE_BTEXT:
					BText_Draw( (menutext_s*)itemptr );
					break;

				default:
					trap_Error( va("Menu_Draw: unknown type %d", itemptr->type) );
			}
		}

#ifndef NDEBUG
		if( uis.debug ) {
			int	x;
			int	y;
			int	w;
			int	h;

			if( !( itemptr->flags & QMF_INACTIVE ) ) {
				x = itemptr->left;
				y = itemptr->top;
				w = itemptr->right - itemptr->left + 1;
				h =	itemptr->bottom - itemptr->top + 1;

				if (itemptr->flags & QMF_HASMOUSEFOCUS) {
					UI_DrawRect(x, y, w, h, colorYellow );
				}
				else {
					UI_DrawRect(x, y, w, h, colorWhite );
				}
			}
		}
#endif
	}

	itemptr = Menu_ItemAtCursor( menu );
	if ( itemptr && itemptr->statusbar)
		itemptr->statusbar( ( void * ) itemptr );

//=================================================================

	//TiM - Small override.  To make the scroll list render over all else, it must be put here:
	if ( menu->displaySpinList )
	{
		menulist_s	*s;
		int selectedNum;
		int i;

		s = (menulist_s *) menu->displaySpinList;

		//first... the black box
		trap_R_SetColor( colorTable[CT_BLACK] );
		if ( s->numitems < MAX_LIST )
			UI_DrawHandlePic( s->drawList.left, s->drawList.up, ( s->drawList.right - s->drawList.left ), ( s->drawList.down - s->drawList.up ), uis.whiteShader );
		else
			UI_DrawHandlePic( s->drawList.left, s->drawList.up, ( s->drawList.right - s->drawList.left )*2, ( s->drawList.down - s->drawList.up ), uis.whiteShader );

		//next the border outline
		trap_R_SetColor( colorTable[s->color2] );

		//left
		UI_DrawHandlePic( s->drawList.left, s->drawList.up+1, 1, ( s->drawList.down - s->drawList.up ) - 2, uis.whiteShader );
		
		if ( s->numitems < MAX_LIST ) {
			//top
			UI_DrawHandlePic( s->drawList.left, s->drawList.up, (s->drawList.right - s->drawList.left), 1, uis.whiteShader );
			//right
			UI_DrawHandlePic( s->drawList.right-1, s->drawList.up+1, 1, ( s->drawList.down - s->drawList.up ) - 2, uis.whiteShader );
			//down
			UI_DrawHandlePic( s->drawList.left, s->drawList.down-1, (s->drawList.right - s->drawList.left), 1, uis.whiteShader );
		}
		else {
			//top
			UI_DrawHandlePic( s->drawList.left, s->drawList.up, (s->drawList.right - s->drawList.left)*2, 1, uis.whiteShader );
			//right
			UI_DrawHandlePic( (s->drawList.right+((s->drawList.right - s->drawList.left)))-1, s->drawList.up+1, 1, ( s->drawList.down - s->drawList.up ) - 2, uis.whiteShader );
			//down
			UI_DrawHandlePic( s->drawList.left, s->drawList.down-1, (s->drawList.right - s->drawList.left)*2, 1, uis.whiteShader );
		}
	
		//Now the selected section
		selectedNum = ( uis.cursory - s->drawList.up+1 ) / SMALLCHAR_HEIGHT;

		if ( s->numitems > MAX_LIST && uis.cursorx > s->drawList.right && (selectedNum + MAX_LIST ) < s->numitems )
			selectedNum += MAX_LIST;

		if ( selectedNum < 0 ) {
			selectedNum = 0;
		}
		if ( selectedNum >= s->numitems ) {
			selectedNum = s->numitems - 1;
		}
		
		if ( selectedNum >= MAX_LIST )
			UI_DrawHandlePic( s->drawList.left+((s->drawList.right - s->drawList.left) + 1), (s->drawList.up+1) + SMALLCHAR_HEIGHT * (selectedNum-MAX_LIST), (s->drawList.right - s->drawList.left) - 2, SMALLCHAR_HEIGHT+1, uis.whiteShader );
		else	
			UI_DrawHandlePic( s->drawList.left+1, (s->drawList.up+1) + SMALLCHAR_HEIGHT * selectedNum, (s->drawList.right - s->drawList.left) - 2, SMALLCHAR_HEIGHT+1, uis.whiteShader );
	
		//And the text
		for ( i=0; i < MAX_LIST; i++ ) {

			if ( i >= s->numitems )
				break;

			if ( s->listnames ) {
				if ( !( s->textFlags & UI_CENTER ) ) {
					UI_DrawProportionalString( s->drawList.left + 2, (s->drawList.up+2) + SMALLCHAR_HEIGHT * i, menu_normal_text[s->listnames[i]], UI_SMALLFONT|s->textFlags, colorTable[CT_WHITE] );
				
					if ( i + MAX_LIST < s->numitems )
						UI_DrawProportionalString( s->drawList.left + 2 + (s->drawList.right-s->drawList.left), (s->drawList.up+2) + SMALLCHAR_HEIGHT * i, menu_normal_text[s->listnames[i+MAX_LIST]], UI_SMALLFONT|s->textFlags, colorTable[CT_WHITE] );
				}
				else {
					UI_DrawProportionalString( s->generic.x + s->textX - 2, (s->drawList.up+2) + SMALLCHAR_HEIGHT * i, menu_normal_text[s->listnames[i]], UI_SMALLFONT|s->textFlags, colorTable[CT_WHITE] );
				
					if ( i + MAX_LIST < s->numitems )
						UI_DrawProportionalString( s->drawList.left + s->textX + 2 + (s->drawList.right-s->drawList.left), (s->drawList.up+2) + SMALLCHAR_HEIGHT * i, menu_normal_text[s->listnames[i+MAX_LIST]], UI_SMALLFONT|s->textFlags, colorTable[CT_WHITE] );				
				}
			}
			else {
				if ( !( s->textFlags & UI_CENTER ) ) {
					UI_DrawProportionalString( s->drawList.left + 2, (s->drawList.up+2) + SMALLCHAR_HEIGHT * i, s->itemnames[i], UI_SMALLFONT|s->textFlags, colorTable[CT_WHITE] );
				
					if ( i + MAX_LIST < s->numitems )
						UI_DrawProportionalString( s->drawList.left + 2 + (s->drawList.right-s->drawList.left), (s->drawList.up+2) + SMALLCHAR_HEIGHT * i, s->itemnames[i+MAX_LIST], UI_SMALLFONT|s->textFlags, colorTable[CT_WHITE] );					
				}
				else {
					UI_DrawProportionalString( s->generic.x + s->textX + s->drawList.xOffset - 2, (s->drawList.up+2) + SMALLCHAR_HEIGHT * i, s->itemnames[i], UI_SMALLFONT|s->textFlags, colorTable[CT_WHITE] );

					if ( i + MAX_LIST < s->numitems )
						UI_DrawProportionalString( s->drawList.left + s->textX + 2 + (s->drawList.right-s->drawList.left), (s->drawList.up+2) + SMALLCHAR_HEIGHT * i, s->itemnames[i+MAX_LIST], UI_SMALLFONT|s->textFlags, colorTable[CT_WHITE] );					
				}
			}
		}
	}
}

/*
=================
Menu_ItemAtCursor
=================
*/
void *Menu_ItemAtCursor( menuframework_s *m )
{
	if ( m->cursor < 0 || m->cursor >= m->nitems )
		return 0;

	return m->items[m->cursor];
}

/*
=================
Menu_ActivateItem
=================
*/
sfxHandle_t Menu_ActivateItem( menuframework_s *s, menucommon_s* item ) {
	if ( item->callback ) {
		item->callback( item, QM_ACTIVATED );
		if( !( item->flags & QMF_SILENT ) ) {
			return menu_move_sound;
		}
	}

	return 0;
}

/*
=================
Menu_DefaultKey
=================
*/
sfxHandle_t Menu_DefaultKey( menuframework_s *m, int key )
{
	sfxHandle_t		sound = 0;
	menucommon_s	*item;
	int				cursor_prev;

	// menu system keys
	switch ( key )
	{
		case K_MOUSE2:
		case K_ESCAPE:
			//TiM - Escape from spin menu if we're in it
			if ( m->displaySpinList ) {
				m->displaySpinList = NULL;
				m->noNewSelecting = qfalse;
				
				return menu_out_sound;
			}
			else {
				UI_PopMenu();
				return menu_out_sound;
			}
	}

	if (!m || !m->nitems)
		return 0;

	// route key stimulus to widget
	item = Menu_ItemAtCursor( m );
	if (item && !(item->flags & (QMF_GRAYED|QMF_INACTIVE)))
	{
		switch (item->type)
		{
			case MTYPE_SPINCONTROL:
				sound = SpinControl_Key( (menulist_s*)item, key );
				break;

			case MTYPE_RADIOBUTTON:
				sound = RadioButton_Key( (menuradiobutton_s*)item, key );
				break;

			case MTYPE_SLIDER:
				sound = Slider_Key( (menuslider_s*)item, key );
				break;

			case MTYPE_SCROLLLIST:
				sound = ScrollList_Key( (menulist_s*)item, key );
				break;

			case MTYPE_FIELD:
				sound = MenuField_Key( (menufield_s*)item, &key );
				break;
		}

		if (sound) {
			// key was handled
			return sound;		
		}
	}

	// default handling
	switch ( key )
	{
#ifndef NDEBUG
		case K_F11:
			uis.debug ^= 1;
			break;

		case K_F12:
			trap_Cmd_ExecuteText(EXEC_APPEND, "screenshot\n");
			break;
#endif
		case K_KP_UPARROW:
		case K_UPARROW:
			cursor_prev    = m->cursor;
			m->cursor_prev = m->cursor;
			m->cursor--;
			Menu_AdjustCursor( m, -1 );
			if ( cursor_prev != m->cursor ) {
				Menu_CursorMoved( m );
				sound = menu_move_sound;
			}
			break;

		case K_TAB:
		case K_KP_DOWNARROW:
		case K_DOWNARROW:
			cursor_prev    = m->cursor;
			m->cursor_prev = m->cursor;
			m->cursor++;
			Menu_AdjustCursor( m, 1 );
			if ( cursor_prev != m->cursor ) {
				Menu_CursorMoved( m );
				sound = menu_move_sound;
			}
			break;

		case K_MOUSE1:
		case K_MOUSE3:
		case K_MOUSE4:
		case K_MOUSE5:
			if (item)
				if ((item->flags & QMF_HASMOUSEFOCUS) && !(item->flags & (QMF_GRAYED|QMF_INACTIVE)))
					return (Menu_ActivateItem( m, item ));
			break;

		case K_JOY1:
		case K_JOY2:
		case K_JOY3:
		case K_JOY4:
		case K_AUX1:
		case K_AUX2:
		case K_AUX3:
		case K_AUX4:
		case K_AUX5:
		case K_AUX6:
		case K_AUX7:
		case K_AUX8:
		case K_AUX9:
		case K_AUX10:
		case K_AUX11:
		case K_AUX12:
		case K_AUX13:
		case K_AUX14:
		case K_AUX15:
		case K_AUX16:
		case K_KP_ENTER:
		case K_ENTER:
			if (item)
				if (!(item->flags & (QMF_MOUSEONLY|QMF_GRAYED|QMF_INACTIVE)))
					return (Menu_ActivateItem( m, item ));
			break;
	}

	return sound;
}

/*
=================
Menu_Cache
=================
*/
void Menu_Cache( void )
{
	int i;
	static  const char *smallNumbers[]= 
	{
	"gfx/2d/numbers/s_zero.tga",
	"gfx/2d/numbers/s_one.tga",
	"gfx/2d/numbers/s_two.tga",
	"gfx/2d/numbers/s_three.tga",
	"gfx/2d/numbers/s_four.tga",
	"gfx/2d/numbers/s_five.tga",
	"gfx/2d/numbers/s_six.tga",
	"gfx/2d/numbers/s_seven.tga",
	"gfx/2d/numbers/s_eight.tga",
	"gfx/2d/numbers/s_nine.tga",
	"gfx/2d/numbers/s_nine.tga",
	};

	uis.charset			= trap_R_RegisterShaderNoMip( "gfx/2d/charsgrid_med");
	uis.charsetProp		= trap_R_RegisterShaderNoMip("gfx/2d/chars_medium");
	uis.charsetPropTiny = trap_R_RegisterShaderNoMip("gfx/2d/chars_tiny");
	uis.charsetPropBig  = trap_R_RegisterShaderNoMip("gfx/2d/chars_big");
	uis.charsetPropB	= trap_R_RegisterShaderNoMip( "gfx/2d/chars_medium" );
	uis.cursor          = trap_R_RegisterShaderNoMip( "menu/common/cursor" );
	
	//trek doens't use these
	//uis.rb_on           = trap_R_RegisterShaderNoMip( "menu/controls/switch_on" );
	//uis.rb_off          = trap_R_RegisterShaderNoMip( "menu/controls/switch_off" );

	uis.whiteShader = trap_R_RegisterShaderNoMip( "white" );
/*	if ( uis.glconfig.hardwareType == GLHW_RAGEPRO ) 
	{
		// the blend effect turns to shit with the normal 
		uis.menuBackShader	= trap_R_RegisterShaderNoMip( "menubackRagePro" );
	} 
	else 
	{
		uis.menuBackShader	= trap_R_RegisterShaderNoMip( "menuback" );
	}
*/
	uis.menuBackShader = trap_R_RegisterShaderNoMip( "gfx/2d/conback.tga" );

	// When hitting enter
	menu_in_sound	= trap_S_RegisterSound( "sound/interface/button2.wav" );
	// When dragging mouse over buttons
	menu_move_sound	= trap_S_RegisterSound( "sound/interface/button1.wav" );
	menu_out_sound	= trap_S_RegisterSound( "sound/interface/button2.wav" );
	menu_buzz_sound	= trap_S_RegisterSound( "sound/interface/button2.wav" );

	// need a nonzero sound, make an empty sound for this
	menu_null_sound = -1;

	// Common menu graphics
	uis.graphicCircle = trap_R_RegisterShaderNoMip("menu/common/circle.tga");
	uis.graphicButtonLeftEnd = trap_R_RegisterShaderNoMip("menu/common/barbuttonleft.tga");

	uis.graphicBracket1CornerLU =  trap_R_RegisterShaderNoMip("menu/common/corner_lu.tga");
	uis.corner_12_18		= trap_R_RegisterShaderNoMip("menu/common/corner_ll_12_18.tga");
	uis.halfroundr_22		= trap_R_RegisterShaderNoMip( "menu/common/halfroundr_22.tga" );

	uis.graphicButtonLeft = trap_R_RegisterShaderNoMip("menu/new/bar2.tga");

	for (i=0;i<10;++i)
	{
		uis.smallNumbers[i] =  trap_R_RegisterShaderNoMip(smallNumbers[i]);
	}

	uis.menu_choice1_snd	= trap_S_RegisterSound( "sound/interface/button3.wav" );
	uis.menu_datadisp1_snd	= trap_S_RegisterSound( "sound/interface/button4.wav" );
	uis.menu_datadisp2_snd	= trap_S_RegisterSound( "sound/interface/button7.wav" );
	uis.menu_datadisp3_snd	= trap_S_RegisterSound( "sound/interface/button5.wav" );

}
	
/*
=================
Mouse_Hide
=================
*/
void Mouse_Hide(void)
{
	uis.cursorDraw = qfalse;
}

/*
=================
Mouse_Show
=================
*/
void Mouse_Show(void)
{
	uis.cursorDraw = qtrue;
}
