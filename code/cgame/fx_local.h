
#define DEFAULT_DEVIATION	0.5

/*
 * fx_*.c
 */

void FXE_Spray (vec3_t direction, float speed, float variation, float cone, vec3_t velocity);
localEntity_t *FX_AddLine(vec3_t start, vec3_t end, float stScale, float scale, float dscale, 
							float startalpha, float endalpha, float killTime, qhandle_t shader);
localEntity_t *FX_AddLine2(vec3_t start, vec3_t end, float stScale, float width1, float dwidth1, float width2, float dwidth2, 
							float startalpha, float endalpha, vec3_t startRGB, vec3_t endRGB, float killTime, qhandle_t shader);
localEntity_t *FX_AddLine3(vec3_t start, vec3_t end, float stScale, float scale, float dscale, 
							float startalpha, float endalpha, vec3_t startRGB, vec3_t endRGB, float killTime, qhandle_t shader);
localEntity_t *FX_AddOrientedLine(vec3_t start, vec3_t end, vec3_t normal, float stScale, float scale,
								  float dscale, float startalpha, float endalpha, float killTime, qhandle_t shader);
localEntity_t *FX_AddTrail( vec3_t origin, vec3_t velocity, qboolean gravity, float length, float dlength, 
							float scale, float dscale, float startalpha, float endalpha, 
							float elasticity, float killTime, qhandle_t shader);
localEntity_t *FX_AddTrail2( vec3_t origin, vec3_t velocity, qboolean gravity, float length, float dlength, 
							float scale, float dscale, float startalpha, float endalpha, vec3_t startRGB, vec3_t endRGB,
							float elasticity, float killTime, qhandle_t shader);
localEntity_t *FX_AddSprite(vec3_t origin, vec3_t velocity, qboolean gravity, float scale, float dscale, 
							float startalpha, float endalpha, float roll, float elasticity, 
							float killTime, qhandle_t shader);
localEntity_t *FX_AddSprite2(vec3_t origin, vec3_t velocity, qboolean gravity, float scale, float dscale, 
							float startalpha, float endalpha, vec3_t startRGB, vec3_t endRGB, float roll, float elasticity, 
							float killTime, qhandle_t shader);
localEntity_t *FX_AddSprite3(vec3_t origin, vec3_t velocity, vec3_t acceleration, float scale, float dscale, 
							float startalpha, float endalpha, float roll, float elasticity, 
							float killTime, qhandle_t shader);
localEntity_t *FX_AddBezier(vec3_t start, vec3_t end, vec3_t cpoint1, vec3_t cpoint2, vec3_t cpointvel1,
							vec3_t cpointvel2,vec3_t cpointacc1, vec3_t cpointacc2, float width,
							float killTime, qhandle_t shader);
localEntity_t *FX_AddQuad( vec3_t origin, vec3_t normal, float scale, float dscale,
						   float startalpha, float endalpha, float roll, float killTime, qhandle_t shader );
localEntity_t *FX_AddQuad2( vec3_t origin, vec3_t normal, float scale, float dscale, float startalpha, float endalpha, 
						   vec3_t startRGB, vec3_t endRGB, float roll, float killTime, qhandle_t shader );
localEntity_t *FX_AddCylinder(	vec3_t start, 
									vec3_t normal,
									float height,
									float dheight,
									float scale,
									float dscale,
									float scale2,
									float dscale2,
									float startalpha, 
									float endalpha, 
									float killTime, 
									qhandle_t shader,
									float bias );
localEntity_t *FX_AddCylinder2(	vec3_t start, 
									vec3_t normal,
									float height,
									float dheight,
									float scale,
									float dscale,
									float scale2,
									float dscale2,
									float startalpha, 
									float endalpha, 
									vec3_t startRGB,
									vec3_t endRGB,
									float killTime, 
									qhandle_t shader,
									float bias );
localEntity_t *FX_AddCylinder(	vec3_t start, 
									vec3_t normal,
									float height,
									float dheight,
									float scale,
									float dscale,
									float scale2,
									float dscale2,
									float startalpha, 
									float endalpha, 
									float killTime, 
									qhandle_t shader,
									float bias );
localEntity_t *FX_AddElectricity( vec3_t origin, vec3_t origin2, float stScale, float scale, float dscale, 
									float startalpha, float endalpha, float killTime, qhandle_t shader, float deviation );
localEntity_t *FX_AddParticle(	vec3_t origin, vec3_t velocity, qboolean gravity, float scale, float dscale, 
								float startalpha, float endalpha, float roll, float elasticity, 
								float killTime, qhandle_t shader, qboolean (*thinkFn)(localEntity_t *le));

localEntity_t *FX_AddSpawner( vec3_t origin, vec3_t dir, vec3_t velocity, vec3_t user, qboolean gravity, int delay,
							 float variance, float killTime, qboolean (*thinkFn)(localEntity_t *le), int radius );

/*
 * phaser
 */
void FX_PhaserFire( vec3_t start, vec3_t end, vec3_t normal, qboolean spark, qboolean impact, qboolean empty );
void FX_PhaserAltFire( vec3_t start, vec3_t end, vec3_t normal, qboolean spark, qboolean impact, qboolean empty );


/*
 * compression rifle
 */
void FX_CompressionShot( vec3_t start, vec3_t end );
void FX_CompressionAltShot( vec3_t start, vec3_t end );
void FX_CompressionExplosion( vec3_t start, vec3_t origin, vec3_t normal, qboolean altfire );
void FX_CompressionHit( vec3_t origin );
void FX_PrifleBeamFire( vec3_t startpos, vec3_t endpos, vec3_t normal, qboolean spark, qboolean impact, qboolean empty );

void FX_ProbeBeam( vec3_t origin, vec3_t dir, int clientNum, qboolean alt_fire );
void FX_RegenBeam( vec3_t origin, vec3_t dir, int clientNum, qboolean alt_fire );

/*
 * tetrion disruptor
 */
void FX_TetrionShot( vec3_t start, vec3_t forward );

/*
 * Scavenger Rifle
 */
void FX_HypoSpray( vec3_t origin, vec3_t dir, qboolean red );

/*
 * Grenade launcher
 */
void FX_GrenadeThink( centity_t *cent, const struct weaponInfo_s *weapon );
void FX_GrenadeHitWall( vec3_t origin, vec3_t normal );
void FX_GrenadeHitPlayer( vec3_t origin, vec3_t normal );
void FX_GrenadeExplode( vec3_t origin, vec3_t normal );
void FX_GrenadeShrapnelExplode( vec3_t origin, vec3_t norm );
void FX_GrenadeShrapnelBits( vec3_t start);
void FX_fxfunc_Explosion( vec3_t start, vec3_t origin, vec3_t normal );
void FX_fxfunc_Shot( vec3_t start, vec3_t dir );

/*
 * detpack
 */
void FX_Detpack(vec3_t origin);


/*
 * Disruptor Weapon
 */
void FX_DisruptorBeamFire( vec3_t startpos, vec3_t endpos, vec3_t normal, qboolean spark, qboolean impact, qboolean empty );
void FX_DisruptorWeaponHitWall( vec3_t origin, vec3_t dir, int size );

/*
 * Quantum Burst
 */
void FX_QuantumThink( centity_t *cent, const struct weaponInfo_s *weapon );
void FX_QuantumAltThink( centity_t *cent, const struct weaponInfo_s *weapon );
void FX_QuantumHitWall( vec3_t origin, vec3_t normal );
void FX_QuantumAltHitWall( vec3_t origin, vec3_t normal );
void FX_QuantumColumns( vec3_t origin );

/*
 * transporter
 */
void FX_Transporter(vec3_t origin);
void FX_TransporterPad( vec3_t origin );
void FX_SPTransporterLensFlares( centity_t* cent, vec3_t headVector, int startTime );


/* Holdable, portable shield item */
void FX_DrawPortableShield(centity_t *cent);



/* Shield */
void FX_PlayerShieldHit( centity_t *cent );


/*
 * Miscellaneous FX
 */
void FX_Disruptor( vec3_t org, float length );
void FX_ExplodeBits( vec3_t org);

void FX_qFlash( centity_t* cent, vec3_t org, int timeIndex );

/*
 * sin table
 */
void fxRandCircumferencePos(vec3_t center, vec3_t normal, float radius, vec3_t out);

