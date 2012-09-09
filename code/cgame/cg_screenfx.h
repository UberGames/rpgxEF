//
// full-screen effects like beaming in/out, static from being hit, etc.
//

enum screenfx_e
{
	SCREENFX_HIT,
	SCREENFX_HALFSHIELDHIT,
	SCREENFX_FULLSHIELDHIT,
	SCREENFX_TRANSPORTER,
	SCREENFX_SP_TRANSPORTER_IN,
	SCREENFX_SP_TRANSPORTER_OUT,
	MAX_SCREENFX	
};

typedef struct screenFX_s
{
	int		events[MAX_SCREENFX];
	int		cgStartTimes[MAX_SCREENFX];
	int		cgEndTimes[MAX_SCREENFX];
} screenFX_t;

extern screenFX_t theScreenFX;

void CG_AddFullScreenEffect(int screenfx, int clientNum);

void CG_DrawFullScreenFX();

