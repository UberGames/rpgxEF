//
// full-screen effects like beaming in/out, static from being hit, etc.
//

#ifndef CG_SCREENFX_H_
#define CG_SCREENFX_H_

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
	int32_t		events[MAX_SCREENFX];
	int32_t		cgStartTimes[MAX_SCREENFX];
	int32_t		cgEndTimes[MAX_SCREENFX];
} screenFX_t;

extern screenFX_t theScreenFX;

void CG_AddFullScreenEffect(int32_t screenfx, int32_t clientNum);

void CG_DrawFullScreenFX(void);

#endif /* CG_SCREENFX_H_ */


