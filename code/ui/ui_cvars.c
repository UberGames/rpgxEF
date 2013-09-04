#include "ui_local.h"
#include "ui_logger.h"


//===================================================================
//
// Functions to set Cvars from menus
//
//===================================================================


/*
=================
InvertMouseCallback
=================
*/
void InvertMouseCallback( void *s, int32_t notification )
{
	UI_LogFuncBegin();
	menulist_s *box = (menulist_s *) s;

	if (notification != QM_ACTIVATED){
		UI_LogFuncEnd();
		return;
	}

	if ( box->curvalue )
		trap_Cvar_SetValue( "m_pitch", -fabs( trap_Cvar_VariableValue( "m_pitch" ) ) );
	else
		trap_Cvar_SetValue( "m_pitch", fabs( trap_Cvar_VariableValue( "m_pitch" ) ) );
	UI_LogFuncEnd();

}

/*
=================
MouseSpeedCallback
=================
*/
void MouseSpeedCallback( void *s, int32_t notification )
{
	UI_LogFuncBegin();
	menuslider_s	*slider = (menuslider_s *) s;

	if (notification != QM_ACTIVATED){
		UI_LogFuncEnd();
		return;
	}

	trap_Cvar_SetValue( "sensitivity", slider->curvalue );
	UI_LogFuncEnd();
}

/*
=================
SmoothMouseCallback
=================
*/
void SmoothMouseCallback( void *s, int32_t notification )
{
	UI_LogFuncBegin();
	menulist_s *box = (menulist_s *) s;

	if (notification != QM_ACTIVATED){
		UI_LogFuncEnd();
		return;
	}

	trap_Cvar_SetValue( "m_filter", box->curvalue );
	UI_LogFuncEnd();
}

/*
=================
GammaCallback
=================
*/
void GammaCallback( void *s, int32_t notification )
{
	UI_LogFuncBegin();
	menuslider_s *slider = ( menuslider_s * ) s;

	if (notification != QM_ACTIVATED){
		UI_LogFuncEnd();
		return;
	}

	trap_Cvar_SetValue( "r_gamma", slider->curvalue / 10.0f );
	UI_LogFuncEnd();
}


/*
=================
IntensityCallback
=================
*/
void IntensityCallback( void *s, int32_t notification )
{
	UI_LogFuncBegin();
	menuslider_s *slider = ( menuslider_s * ) s;

	if (notification != QM_ACTIVATED){
		UI_LogFuncEnd();
		return;
	}

	trap_Cvar_SetValue( "r_intensity", slider->curvalue / 10.0f );
	UI_LogFuncEnd();
}

/*
=================
ScreensizeCallback
=================
*/
void ScreensizeCallback( void *s, int32_t notification )
{
	UI_LogFuncBegin();
	menuslider_s *slider = ( menuslider_s * ) s;

	if (notification != QM_ACTIVATED){
		UI_LogFuncEnd();
		return;
	}

	trap_Cvar_SetValue( "cg_viewsize", slider->curvalue);
	UI_LogFuncEnd();
}

/*
=================
AlwaysRunCallback
=================
*/
void AlwaysRunCallback( void *s, int32_t notification )
{
	UI_LogFuncBegin();
	menulist_s *s_alwaysrun_box = ( menulist_s * ) s;

	if (notification != QM_ACTIVATED){
		UI_LogFuncEnd();
		return;
	}

	trap_Cvar_SetValue( "cl_run", s_alwaysrun_box->curvalue );
	UI_LogFuncEnd();
}


/*
=================
AutoswitchCallback
=================
*/
void AutoswitchCallback( void *unused, int32_t notification )
{
	UI_LogFuncBegin();
	static menulist_s	s_autoswitch_box;

	if (notification != QM_ACTIVATED){
		UI_LogFuncEnd();
		return;
	}

	trap_Cvar_SetValue( "cg_autoswitch", s_autoswitch_box.curvalue );
	UI_LogFuncEnd();
}


/*
=================
AnisotropicFilteringCallback
=================
*/
void AnisotropicFilteringCallback( void *s, int32_t notification )
{
	UI_LogFuncBegin();
	menulist_s *anisotropicfiltering = ( menulist_s * ) s;

	if (notification != QM_ACTIVATED){
		UI_LogFuncEnd();
		return;
	}

	trap_Cvar_SetValue( "r_ext_texture_filter_anisotropic", anisotropicfiltering->curvalue );
	UI_LogFuncEnd();
}

/*
=================
JoyXButtonCallback
=================
*/
void JoyXButtonCallback( void *s, int32_t notification )
{
	UI_LogFuncBegin();
	menulist_s *box = (menulist_s *) s;

	if (notification != QM_ACTIVATED){
		UI_LogFuncEnd();
		return;
	}

	trap_Cvar_SetValue( "joy_xbutton", box->curvalue );
	UI_LogFuncEnd();
}

/*
=================
JoyYButtonCallback
=================
*/
void JoyYButtonCallback( void *s, int32_t notification )
{
	UI_LogFuncBegin();
	menulist_s *box = (menulist_s *) s;

	if (notification != QM_ACTIVATED){
		UI_LogFuncEnd();
		return;
	}

	trap_Cvar_SetValue( "joy_ybutton", box->curvalue );
	UI_LogFuncEnd();
}
