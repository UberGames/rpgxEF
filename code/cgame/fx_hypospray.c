#include "cg_local.h"
#include "fx_local.h"


/*
-------------------------
FX_HypoSpray
Redtechie: RPG-X Added
FIXME! FIXME! FIXME! FIXME! Im not spraying in the direction some one shoots me!
TiM: Fixed!  An improperly formatted directional vector was being sent.  it's all good now :)
-------------------------
*/

#define NUM_HYPO_PUFFS	20

void FX_HypoSpray( vec3_t origin, vec3_t dir, qboolean red )  // When not red, it'll be blue
{
	vec3_t	color, vel, accel, angles, work;
	float	scale, dscale;
	int i;
	localEntity_t *le;

	vectoangles( dir, angles );

	for ( i = 0; i < NUM_HYPO_PUFFS; i++ )
	{
		if ( red )
		{
			VectorSet( color, 1.0f, random() * 0.4f, random() * 0.4f ); // mostly red
		}
		else
		{
			VectorSet( color, random() * 0.5f, random() * 0.5f + 0.5f, 1.0f ); // mostly blue
		}

		VectorCopy( angles, work );

		work[0] += crandom() * 12.0f;
		work[1] += crandom() * 12.0f;

		AngleVectors( work, vel, NULL, NULL );

		scale = random() * 256.0f + 128.0f;

		VectorScale( vel, scale, vel );
		VectorScale( vel, random() * -0.3f, accel );

		scale = random() * 4.0f + 2.0f;
		dscale = random() * 64.0f + 24.0f;

		//localEntity_t *FX_AddSprite(vec3_t origin, vec3_t velocity, qboolean gravity, float scale, float dscale, 
		//					float startalpha, float endalpha, float roll, float elasticity, 
		//					float killTime, qhandle_t shader);

		le = FX_AddSprite( origin, vel, qfalse, scale, dscale, 0.8f + random() * 0.2f, 0.0f, crandom() * 50, /*crandom() * 5*/0, 1000, cgs.media.steamShader );
		VectorSet(le->data.sprite.startRGB, random() * 0.5f, random() * 0.5f + 0.5f, 1.0f );// mostly blue
	}
}

