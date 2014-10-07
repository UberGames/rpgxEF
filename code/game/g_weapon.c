/* Copyright (C) 1999-2000 Id Software, Inc.
 *
 * g_weapon.c
 * perform the server side effects of a weapon firing
 */

#include "g_local.h"
#include "g_weapon.h"
#include "g_client.h"
#include "g_missile.h"
#include "g_logger.h"
#include "g_lua.h"
#include "g_combat.h"
#include "q_math.h"
#include "bg_lex.h"

weaponConfig_t weaponConfig;

static void G_Weapon_DefaultConfig(void) {
	G_LogFuncBegin();
	weaponConfig.phaser.primary.damage = 55;
	weaponConfig.phaser.primary.range = 2048.0;
	weaponConfig.phaser.primary.variation = 6;
	weaponConfig.phaser.secondary.damage = 55;
	weaponConfig.phaser.secondary.range = 2048.0;
	weaponConfig.phaser.secondary.variation = 6;
	weaponConfig.phaser.secondary.radius = 80.0;
	weaponConfig.crifle.primary.damage = 75;
	weaponConfig.crifle.primary.range = 8192.0;
	weaponConfig.crifle.secondary.damage = 16;
	weaponConfig.crifle.secondary.range = 8192.0;
	weaponConfig.crifle.secondary.size = 1;
	weaponConfig.disruptor.primary.damage = 150;
	weaponConfig.disruptor.primary.range = 2048.0;
	weaponConfig.disruptor.secondary.damage = 150;
	weaponConfig.disruptor.secondary.size = 6;
	weaponConfig.grenade.primary.damage = 75;
	weaponConfig.grenade.primary.size = 4.0;
	weaponConfig.grenade.primary.splash.damage = 100;
	weaponConfig.grenade.primary.splash.radius = 190;
	weaponConfig.grenade.primary.time = 2000;
	weaponConfig.grenade.primary.velocity = 1000.0;
	weaponConfig.grenade.secondary.damage = 80;
	weaponConfig.grenade.secondary.splash.damage = 100;
	weaponConfig.grenade.secondary.splash.radius = 190;
	weaponConfig.grenade.secondary.time = 2500;
	weaponConfig.hyperspanner.primary.rate = 2;
	weaponConfig.hyperspanner.secondary.rate = 4;
	weaponConfig.quantum.primary.damage = 140;
	weaponConfig.quantum.primary.size = 1.0;
	weaponConfig.quantum.primary.splash.damage = 140;
	weaponConfig.quantum.primary.splash.radius = 160;
	weaponConfig.quantum.secondary.damage = 140;
	weaponConfig.quantum.secondary.size = 1.0;
	weaponConfig.quantum.secondary.splash.damage = 140;
	weaponConfig.quantum.secondary.splash.radius = 160;
	weaponConfig.quantum.secondary.think.time = 300;
	weaponConfig.quantum.secondary.think.search.distance = 4096.0;
	weaponConfig.quantum.secondary.think.search.time = 100;
	weaponConfig.tr116.primary.damage = 150;
	weaponConfig.tr116.primary.range = 8192.0;
	G_LogFuncEnd();
}

static qboolean G_Weapon_ParseConfigPhaser(bgLex* lexer) {
	G_LogFuncBegin();

	G_Assert(lexer, qfalse);

	bgLex_lex(lexer);
	if (lexer->morphem.type == LMT_SYMBOL && lexer->morphem.data.symbol == LSYM_POINT) {
		bgLex_lex(lexer);
		if (lexer->morphem.type == LMT_SYMBOL) {
			if (lexer->morphem.data.symbol == LSYM_WCONF_PRIMARY) {
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL || lexer->morphem.data.symbol != LSYM_POINT) {
					G_Logger(LL_ERROR, "Expected '.' at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL) {
					G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				switch (lexer->morphem.data.symbol) {
					case LSYM_WCONF_DAMAGE:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_INT) {
							weaponConfig.phaser.primary.damage = lexer->morphem.data.numInteger;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					case LSYM_WCONF_VARIATION:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_INT) {
							weaponConfig.phaser.primary.variation = lexer->morphem.data.numInteger;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					case LSYM_WCONF_RANGE:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_DOUBLE) {
							weaponConfig.phaser.primary.range = lexer->morphem.data.numDouble;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected double value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					default:
						G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
						G_LogFuncEnd();
						return qfalse;
				}
			} else if (lexer->morphem.data.symbol == LSYM_WCONF_SECONDARY) {
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL || lexer->morphem.data.symbol != LSYM_POINT) {
					G_Logger(LL_ERROR, "Expected '.' at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL) {
					G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				switch (lexer->morphem.data.symbol) {
					case LSYM_WCONF_DAMAGE:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_INT) {
							weaponConfig.phaser.secondary.damage = lexer->morphem.data.numInteger;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					case LSYM_WCONF_VARIATION:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_INT) {
							weaponConfig.phaser.secondary.variation = lexer->morphem.data.numInteger;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					case LSYM_WCONF_RANGE:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_DOUBLE) {
							weaponConfig.phaser.secondary.range = lexer->morphem.data.numDouble;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected double value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					case LSYM_WCONF_RADIUS:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_DOUBLE) {
							weaponConfig.phaser.secondary.radius = lexer->morphem.data.numDouble;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected double value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					default:
						G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
						G_LogFuncEnd();
						return qfalse;
				}
			} else {
				G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
				G_LogFuncEnd();
				return qfalse;
			}
		} else {
			G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
			G_LogFuncEnd();
			return qfalse;
		}
	} else {
		G_Logger(LL_ERROR, "Expected '.' at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
		G_LogFuncEnd();
		return qfalse;
	}
}

static qboolean G_Weapon_ParseConfigCRifle(bgLex* lexer) {
	G_LogFuncBegin();

	G_Assert(lexer, qfalse);

	bgLex_lex(lexer);
	if (lexer->morphem.type == LMT_SYMBOL && lexer->morphem.data.symbol == LSYM_POINT) {
		bgLex_lex(lexer);
		if (lexer->morphem.type == LMT_SYMBOL) {
			if (lexer->morphem.data.symbol == LSYM_WCONF_PRIMARY) {
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL || lexer->morphem.data.symbol != LSYM_POINT) {
					G_Logger(LL_ERROR, "Expected '.' at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL) {
					G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				switch (lexer->morphem.data.symbol) {
					case LSYM_WCONF_DAMAGE:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_INT) {
							weaponConfig.crifle.primary.damage = lexer->morphem.data.numInteger;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					case LSYM_WCONF_RADIUS:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_DOUBLE) {
							weaponConfig.crifle.primary.range = lexer->morphem.data.numDouble;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected double value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					default:
						G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
						G_LogFuncEnd();
						return qfalse;
				}
			} else if (lexer->morphem.data.symbol == LSYM_WCONF_SECONDARY) {
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL || lexer->morphem.data.symbol != LSYM_POINT) {
					G_Logger(LL_ERROR, "Expected '.' at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL) {
					G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				switch (lexer->morphem.data.symbol) {
					case LSYM_WCONF_DAMAGE:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_INT) {
							weaponConfig.crifle.secondary.damage = lexer->morphem.data.numInteger;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					case LSYM_WCONF_SIZE:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_INT) {
							weaponConfig.crifle.secondary.size = lexer->morphem.data.numInteger;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					case LSYM_WCONF_RANGE:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_DOUBLE) {
							weaponConfig.crifle.secondary.range = lexer->morphem.data.numDouble;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected double value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					default:
						G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
						G_LogFuncEnd();
						return qfalse;
				}
			} else {
				G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
				G_LogFuncEnd();
				return qfalse;
			}
		} else {
			G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
			G_LogFuncEnd();
			return qfalse;
		}
	} else {
		G_Logger(LL_ERROR, "Expected '.' at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
		G_LogFuncEnd();
		return qfalse;
	}
}

static qboolean G_Weapon_ParseConfigGrenade(bgLex* lexer) {
	G_LogFuncBegin();

	G_Assert(lexer, qfalse);

	bgLex_lex(lexer);
	if (lexer->morphem.type == LMT_SYMBOL && lexer->morphem.data.symbol == LSYM_POINT) {
		bgLex_lex(lexer);
		if (lexer->morphem.type == LMT_SYMBOL) {
			if (lexer->morphem.data.symbol == LSYM_WCONF_PRIMARY) {
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL || lexer->morphem.data.symbol != LSYM_POINT) {
					G_Logger(LL_ERROR, "Expected '.' at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL) {
					G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				switch (lexer->morphem.data.symbol) {
					case LSYM_WCONF_DAMAGE:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_INT) {
							weaponConfig.grenade.primary.damage = lexer->morphem.data.numInteger;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					case LSYM_WCONF_TIME:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_INT) {
							weaponConfig.grenade.primary.time = lexer->morphem.data.numInteger;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					case LSYM_WCONF_VELOCITY:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_DOUBLE) {
							weaponConfig.grenade.primary.velocity = lexer->morphem.data.numDouble;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected double value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					case LSYM_WCONF_SIZE:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_DOUBLE) {
							weaponConfig.grenade.primary.size = lexer->morphem.data.numDouble;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected double value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					case LSYM_WCONF_SPLASH:
						bgLex_lex(lexer);
						if (lexer->morphem.type != LMT_SYMBOL || lexer->morphem.data.symbol != LSYM_POINT) {
							G_Logger(LL_ERROR, "Unexpected token at wepons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						bgLex_lex(lexer);
						if (lexer->morphem.type != LMT_SYMBOL) {
							G_Logger(LL_ERROR, "Unexpected token at wepons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						if (lexer->morphem.data.symbol == LSYM_WCONF_DAMAGE) {
							bgLex_lex(lexer);
							if (lexer->morphem.type == LMT_INT) {
								weaponConfig.grenade.primary.splash.damage = lexer->morphem.data.numInteger;
								G_LogFuncEnd();
								return qtrue;
							} else {
								G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
								G_LogFuncEnd();
								return qfalse;
							}
						} else if (lexer->morphem.data.symbol == LSYM_WCONF_RADIUS) {
							bgLex_lex(lexer);
							if (lexer->morphem.type == LMT_INT) {
								weaponConfig.grenade.primary.splash.radius = lexer->morphem.data.numInteger;
								G_LogFuncEnd();
								return qtrue;
							} else {
								G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
								G_LogFuncEnd();
								return qfalse;
							}
						} else {
							G_Logger(LL_ERROR, "Expected 'damage' or 'radius' at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					default:
						G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
						G_LogFuncEnd();
						return qfalse;
				}
			} else if (lexer->morphem.data.symbol == LSYM_WCONF_SECONDARY) {
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL || lexer->morphem.data.symbol != LSYM_POINT) {
					G_Logger(LL_ERROR, "Expected '.' at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL) {
					G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				switch (lexer->morphem.data.symbol) {
					case LSYM_WCONF_DAMAGE:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_INT) {
							weaponConfig.grenade.secondary.damage = lexer->morphem.data.numInteger;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					case LSYM_WCONF_TIME:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_INT) {
							weaponConfig.grenade.secondary.time = lexer->morphem.data.numInteger;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					case LSYM_WCONF_SPLASH:
						bgLex_lex(lexer);
						if (lexer->morphem.type != LMT_SYMBOL || lexer->morphem.data.symbol != LSYM_POINT) {
							G_Logger(LL_ERROR, "Unexpected token at wepons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						bgLex_lex(lexer);
						if (lexer->morphem.type != LMT_SYMBOL) {
							G_Logger(LL_ERROR, "Unexpected token at wepons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						if (lexer->morphem.data.symbol == LSYM_WCONF_DAMAGE) {
							bgLex_lex(lexer);
							if (lexer->morphem.type == LMT_INT) {
								weaponConfig.grenade.secondary.splash.damage = lexer->morphem.data.numInteger;
								G_LogFuncEnd();
								return qtrue;
							} else {
								G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
								G_LogFuncEnd();
								return qfalse;
							}
						} else if (lexer->morphem.data.symbol == LSYM_WCONF_RADIUS) {
							bgLex_lex(lexer);
							if (lexer->morphem.type == LMT_INT) {
								weaponConfig.grenade.secondary.splash.radius = lexer->morphem.data.numInteger;
								G_LogFuncEnd();
								return qtrue;
							} else {
								G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
								G_LogFuncEnd();
								return qfalse;
							}
						} else {
							G_Logger(LL_ERROR, "Expected 'damage' or 'radius' at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					default:
						G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
						G_LogFuncEnd();
						return qfalse;
				}
			} else {
				G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
				G_LogFuncEnd();
				return qfalse;
			}
		} else {
			G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
			G_LogFuncEnd();
			return qfalse;
		}
	} else {
		G_Logger(LL_ERROR, "Expected '.' at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
		G_LogFuncEnd();
		return qfalse;
	}
}

static qboolean G_Weapon_ParseConfigDisruptor(bgLex* lexer) {
	G_LogFuncBegin();

	G_Assert(lexer, qfalse);

	bgLex_lex(lexer);
	if (lexer->morphem.type == LMT_SYMBOL && lexer->morphem.data.symbol == LSYM_POINT) {
		bgLex_lex(lexer);
		if (lexer->morphem.type == LMT_SYMBOL) {
			if (lexer->morphem.data.symbol == LSYM_WCONF_PRIMARY) {
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL || lexer->morphem.data.symbol != LSYM_POINT) {
					G_Logger(LL_ERROR, "Expected '.' at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL) {
					G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				switch (lexer->morphem.data.symbol) {
					case LSYM_WCONF_DAMAGE:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_INT) {
							weaponConfig.disruptor.primary.damage = lexer->morphem.data.numInteger;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					case LSYM_WCONF_RANGE:
						if (lexer->morphem.type == LMT_INT) {
							weaponConfig.disruptor.primary.range = lexer->morphem.data.numInteger;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					default:
						G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
						G_LogFuncEnd();
						return qfalse;
				}
			} else if (lexer->morphem.data.symbol == LSYM_WCONF_SECONDARY) {
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL || lexer->morphem.data.symbol != LSYM_POINT) {
					G_Logger(LL_ERROR, "Expected '.' at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL) {
					G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				switch (lexer->morphem.data.symbol) {
					case LSYM_WCONF_DAMAGE:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_INT) {
							weaponConfig.disruptor.secondary.damage = lexer->morphem.data.numInteger;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					case LSYM_WCONF_SIZE:
						if (lexer->morphem.type == LMT_INT) {
							weaponConfig.disruptor.secondary.size = lexer->morphem.data.numInteger;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					default:
						G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
						G_LogFuncEnd();
						return qfalse;
				}
			} else {
				G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
				G_LogFuncEnd();
				return qfalse;
			}
		} else {
			G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
			G_LogFuncEnd();
			return qfalse;
		}
	} else {
		G_Logger(LL_ERROR, "Expected '.' at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
		G_LogFuncEnd();
		return qfalse;
	}
}

static qboolean G_Weapon_ParseConfigHyperspanner(bgLex* lexer) {
	G_LogFuncBegin();

	G_Assert(lexer, qfalse);

	bgLex_lex(lexer);
	if (lexer->morphem.type == LMT_SYMBOL && lexer->morphem.data.symbol == LSYM_POINT) {
		bgLex_lex(lexer);
		if (lexer->morphem.type == LMT_SYMBOL) {
			if (lexer->morphem.data.symbol == LSYM_WCONF_PRIMARY) {
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL || lexer->morphem.data.symbol != LSYM_POINT) {
					G_Logger(LL_ERROR, "Expected '.' at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL) {
					G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				switch (lexer->morphem.data.symbol) {
					case LSYM_WCONF_RATE:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_INT) {
							weaponConfig.hyperspanner.primary.rate = lexer->morphem.data.numInteger;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					default:
						G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
						G_LogFuncEnd();
						return qfalse;
				}
			} else if (lexer->morphem.data.symbol == LSYM_WCONF_SECONDARY) {
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL || lexer->morphem.data.symbol != LSYM_POINT) {
					G_Logger(LL_ERROR, "Expected '.' at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				bgLex_lex(lexer);
				if (lexer->morphem.type != LMT_SYMBOL) {
					G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_LogFuncEnd();
					return qfalse;
				}
				switch (lexer->morphem.data.symbol) {
					case LSYM_WCONF_RATE:
						bgLex_lex(lexer);
						if (lexer->morphem.type == LMT_INT) {
							weaponConfig.hyperspanner.secondary.rate = lexer->morphem.data.numInteger;
							G_LogFuncEnd();
							return qtrue;
						} else {
							G_Logger(LL_ERROR, "Expected integer value at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
							G_LogFuncEnd();
							return qfalse;
						}
						break;
					default:
						G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
						G_LogFuncEnd();
						return qfalse;
				}
			} else {
				G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
				G_LogFuncEnd();
				return qfalse;
			}
		} else {
			G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
			G_LogFuncEnd();
			return qfalse;
		}
	} else {
		G_Logger(LL_ERROR, "Expected '.' at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
		G_LogFuncEnd();
		return qfalse;
	}
}

static qboolean G_Weapon_ParseConfigTR116(bgLex* lexer) {
	return qfalse;
}

static qboolean G_Weapon_ParseConfigQuantum(bgLex* lexer) {
	return qfalse;
}

void G_Weapon_LoadConfig(void) {
	fileHandle_t f = 0;
	bgLex* lexer = NULL;
	char* buffer = NULL;
	int32_t len = 0;

	G_LogFuncBegin();

	len = trap_FS_FOpenFile("weapon.cfg", &f, FS_READ);
	if (len == 0) {
		G_LocLogger(LL_ERROR, "weapon.cfg not found or empty! Using defaults.\n");
		G_Weapon_DefaultConfig();
		G_LogFuncEnd();
		return;
	}

	buffer = (char*)malloc(len + 1);
	if (buffer == NULL) {
		G_LocLogger(LL_ERROR, "Was unable to allocate %i byte.\n", (len + 1));
		G_Weapon_DefaultConfig();
		trap_FS_FCloseFile(f);
		G_LogFuncEnd();
		return;
	}
	memset(buffer, 0, len + 1);

	trap_FS_Read(buffer, len, f);
	trap_FS_FCloseFile(f);

	lexer = bgLex_create(buffer);
	if (lexer == NULL) {
		G_LocLogger(LL_ERROR, "Could not create new bgLex to lex weapons.cfg! Using defauls.\n");
		G_Weapon_DefaultConfig();
		free(buffer);
		G_LogFuncEnd();
		return;
	}

	while (bgLex_lex(lexer) != 0) {
		if (lexer->morphem.type == LMT_SYMBOL) {
			switch (lexer->morphem.data.symbol) {
				case LSYM_WCONF_PHASER:
					if (G_Weapon_ParseConfigPhaser(lexer) == qfalse) {
						G_Weapon_DefaultConfig();
						bgLex_destroy(lexer);
						free(buffer);
						G_LogFuncEnd();
						return;
					}
					break;
				case LSYM_WCONF_CRIFLE:
					if (G_Weapon_ParseConfigCRifle(lexer) == qfalse) {
						G_Weapon_DefaultConfig();
						bgLex_destroy(lexer);
						free(buffer);
						G_LogFuncEnd();
						return;
					}
					break;
				case LSYM_WCONF_GRENADE:
					if (G_Weapon_ParseConfigGrenade(lexer) == qfalse) {
						G_Weapon_DefaultConfig();
						bgLex_destroy(lexer);
						free(buffer);
						G_LogFuncEnd();
						return;
					}
					break;
				case LSYM_WCONF_DISRUPTOR:
					if (G_Weapon_ParseConfigDisruptor(lexer) == qfalse) {
						G_Weapon_DefaultConfig();
						bgLex_destroy(lexer);
						free(buffer);
						G_LogFuncEnd();
						return;
					}
					break;
				case LSYM_WCONF_TR116:
					if (G_Weapon_ParseConfigTR116(lexer) == qfalse) {
						G_Weapon_DefaultConfig();
						bgLex_destroy(lexer);
						free(buffer);
						G_LogFuncEnd();
						return;
					}
					break;
				case LSYM_WCONF_QUANTUM:
					if (G_Weapon_ParseConfigQuantum(lexer) == qfalse) {
						G_Weapon_DefaultConfig();
						bgLex_destroy(lexer);
						free(buffer);
						G_LogFuncEnd();
						return;
					}
					break;
				case LSYM_WCONF_HYPERSPANNER:
					if (G_Weapon_ParseConfigHyperspanner(lexer) == qfalse) {
						G_Weapon_DefaultConfig();
						bgLex_destroy(lexer);
						free(buffer);
						G_LogFuncEnd();
						return;
					}
					break;
				default:
					G_Logger(LL_ERROR, "Unexpected token at weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
					G_Weapon_DefaultConfig();
					bgLex_destroy(lexer);
					free(buffer);
					G_LogFuncEnd();
					return;
			}
		} else {
			G_LocLogger(LL_ERROR, "Unexpected token in weapons.cfg:%d:%d!\n", lexer->morphem.line, lexer->morphem.column);
			G_Weapon_DefaultConfig();
			bgLex_destroy(lexer);
			free(buffer);
			G_LogFuncEnd();
			return;
		}
	}
}

#define MAX_BEAM_HITS 4

#define DMG_VAR	(flrandom(0.8,1.2))

/* Phaser */
/* I'll keep this comment just because it's funny lol :D */
/* RPG-X: TiM - Increased to a standard 0.5 second
 * burst - Phenix GOING DOWN - TiM GOING UP we had
 * complaints when this was put down :P */
#define	PHASER_DAMAGE				rpg_phaserDamage.integer
#define PHASER_ALT_RADIUS			80 			 /* RPG-X: TiM - Increased to a near instant kill */

/* Compression Rifle */
#define	CRIFLE_DAMAGE				rpg_rifleDamage.integer
#define CRIFLE_ALTDAMAGE			rpg_rifleAltDamage.integer

/* Stasis Weapon */
#define STASIS_DAMAGE				rpg_disruptorDamage.integer

/* Grenade Launcher */
#define GRENADE_DAMAGE				rpg_grenadeDamage.integer
#define GRENADE_SPLASH_RAD			190
#define GRENADE_SPLASH_DAM			100
#define GRENADE_ALT_DAMAGE			rpg_grenadeAltDamage.integer

/* Tetrion Disruptor */
#define TETRION_DAMAGE				rpg_tr116Damage.integer

/* Quantum Burst */
#define QUANTUM_DAMAGE				rpg_photonDamage.integer	
#define QUANTUM_SPLASH_DAM			rpg_photonDamage.integer
#define QUANTUM_SPLASH_RAD			160
#define QUANTUM_ALT_DAMAGE			rpg_photonAltDamage.integer	
#define QUANTUM_ALT_SPLASH_DAM		rpg_photonAltDamage.integer
#define QUANTUM_ALT_SPLASH_RAD		80

void G_Weapon_SnapVectorTowards(vec3_t v, vec3_t to) {
	int32_t i;

	G_LogFuncBegin();

	for (i = 0; i < 3; i++) {
		if (to[i] <= v[i]) {
			v[i] = (int32_t)v[i];
		} else {
			v[i] = (int32_t)v[i] + 1;
		}
	}

	G_LogFuncEnd();
}




/*
----------------------------------------------
PLAYER WEAPONS
----------------------------------------------
*/

/*
----------------------------------------------
HYPERSPANNER
----------------------------------------------
*/


#define HYPERSPANNER_RATE			2
#define HYPERSPANNER_ALT_RATE		4

/**
 * @brief Handles weapon fire of the Hyperspanner.
 * @param ent the player
 * @param alt_fire was this alt fire mode?
 * @todo additional logging messages
 */
static void WP_FireHyperspanner(gentity_t* ent, qboolean alt_fire) {
	double		modifier = 0.0;
	struct list validEnts;
	struct list	classnames;
	list_iter_p iter = NULL;
	container_p cont = NULL;
	gentity_t*	e = NULL;
	gentity_t*	nearest = NULL;
	int32_t		count = 0;
	double		nearestd = 65000.0;
	vec3_t		dVec = { 0, 0, 0 };
	vec3_t		end = { 0, 0, 0 };
	vec3_t		mins = { -40, -40, 0 };
	vec3_t		maxs = { 40, 40, 0 };

	G_LogFuncBegin();

	G_Assert(ent, (void)0);

	/* prepare lists */
	list_init(&classnames, free);
	list_init(&validEnts, free);
	classnames.append(&classnames, "func_breakable", LT_STRING, strlen("func_breakable") + 1);
	classnames.append(&classnames, "misc_model_breakable", LT_STRING, strlen("misc_model_breakable") + 1);

	/* find all vlaid entities in range */
	count = G_RadiusListOfTypes(&classnames, ent->r.currentOrigin, 512, NULL, &validEnts);
	classnames.clear(&classnames);

	if (count > 0) {
		trace_t tr;

		memset(&tr, 0, sizeof(trace_t));
		iter = validEnts.iterator(&validEnts, LIST_FRONT);

		for (cont = validEnts.next(iter); cont != NULL; cont = validEnts.next(iter)) {
			e = cont->data;

			// TODO: fix problems with small distance
			if ((e->spawnflags & 512) != 0) {
				VectorSubtract(ent->r.currentOrigin, e->s.angles2, dVec);
				VectorMA(e->s.angles2, 1024, dVec, end);
				trap_Trace(&tr, e->s.angles2, mins, maxs, end, e->s.number, MASK_SHOT);
			} else {
				VectorSubtract(ent->r.currentOrigin, e->s.origin, dVec);
				VectorMA(e->s.origin, 1024, dVec, end);
				trap_Trace(&tr, e->s.origin, mins, maxs, end, e->s.number, MASK_SHOT);
			}

			if (tr.entityNum != ent->s.number) {
				continue;
			}

			if ((e->spawnflags & 512) != 0) {
				VectorSubtract(ent->r.currentOrigin, e->s.angles2, dVec);
			} else {
				VectorSubtract(ent->r.currentOrigin, e->s.origin, dVec);
			}

			if (VectorLength(dVec) < nearestd) {
				nearest = e;
				nearestd = VectorLength(dVec);
			}
		}
	} else {
		G_LogFuncEnd();
		return;
	}

	if (nearest == NULL || nearest->inuse == qfalse) {
		validEnts.clear(&validEnts);
		G_LogFuncEnd();
		return;
	}

	/* determine the repair rate modifier */
	if (rpg_repairModifier.value < 0) {
		modifier = 1;
	} else {
		modifier = rpg_repairModifier.value;
	}

	/* call G_Repair */
	if (alt_fire) {
		G_Combat_Repair(ent, nearest, HYPERSPANNER_ALT_RATE * modifier);
	} else {
		G_Combat_Repair(ent, nearest, HYPERSPANNER_RATE * modifier);
	}

	validEnts.clear(&validEnts);

	G_LogFuncEnd();
}

/*
----------------------------------------------
PHASER
----------------------------------------------
*/

#define MAXRANGE_PHASER				2048 /* This is the same as the range MAX_BEAM_RANGE	2048 */
#define NUM_PHASER_TRACES 			3
#define BEAM_VARIATION				6
#define PHASER_POINT_BLANK			96
#define PHASER_POINT_BLANK_FRAC		((float)PHASER_POINT_BLANK / (float)MAXRANGE_PHASER)

/**
 * @brief Handles weapon fire of the phaser.
 * @param ent the player
 * @param alt_fire was this alt fire mode?
 * @todo additional logging messages
 */
static void WP_FirePhaser(gentity_t* ent, qboolean alt_fire) {
	trace_t		tr;
	vec3_t		end;
	gentity_t*	traceEnt = NULL;
	int32_t		trEnts[NUM_PHASER_TRACES];
	int32_t		i = 0;
	int32_t		damage = 0;
	double		trEntFraction[NUM_PHASER_TRACES];

	G_LogFuncBegin();

	G_Assert(ent, (void)0);

	VectorMA(muzzle, MAXRANGE_PHASER, forward, end);
	/* Add a subtle variation to the beam weapon's endpoint */
	for (i = 0; i < 3; i++) {
		end[i] += crandom() * BEAM_VARIATION;
	}

	for (i = 0; i < NUM_PHASER_TRACES; i++) {
		trEnts[i] = -1;
		trEntFraction[i] = 0.0;
	}

	/* Find out who we've hit */
	memset(&tr, 0, sizeof(trace_t));
	trap_Trace(&tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT);
	if (tr.entityNum != (MAX_GENTITIES - 1)) {
		trEnts[0] = tr.entityNum;
		trEntFraction[0] = tr.fraction;
	}

	if (alt_fire && (ent->client->ps.ammo[WP_5] > 0)) {	/*
		 * Use the ending point of the thin trace to do two more traces,
		 * one on either side, for actual damaging effect.
		 */
		vec3_t vUp = { 0, 0, 1 };
		vec3_t vRight = { 0, 0, 0 };

		CrossProduct(forward, vUp, vRight);
		VectorNormalize(vRight);
		VectorCopy(tr.endpos, end);
		trap_Trace(&tr, muzzle, NULL, NULL, end, ent->s.number, (CONTENTS_PLAYERCLIP | CONTENTS_BODY));
		if ((tr.entityNum != (MAX_GENTITIES - 1)) && (tr.entityNum != trEnts[0])) {
			trEnts[1] = tr.entityNum;
			trEntFraction[1] = tr.fraction;
		}

		trap_Trace(&tr, muzzle, NULL, NULL, end, ent->s.number, (CONTENTS_PLAYERCLIP | CONTENTS_BODY));
		if ((tr.entityNum != (MAX_GENTITIES - 1)) && (tr.entityNum != trEnts[0]) && (tr.entityNum != trEnts[1])) {
			trEnts[2] = tr.entityNum;
			trEntFraction[2] = tr.fraction;
		}
	}

	for (i = 0; i < NUM_PHASER_TRACES; i++) {
		if (-1 == trEnts[i]) {
			continue;
		}
		traceEnt = &g_entities[trEnts[i]];

		if (traceEnt == NULL) {
			G_LocLogger(LL_WARN, "traceEnt == NULL\n");
			continue;
		}

		if (traceEnt->takedamage && ((rpg_dmgFlags.integer & 1) != 0)) {
			/*damage = (float)PHASER_DAMAGE*DMG_VAR*s_quadFactor;*/ /* No variance on phaser */
			damage = PHASER_DAMAGE;

			if (trEntFraction[i] <= PHASER_POINT_BLANK_FRAC) {
				/* Point blank!  Do up to double damage. */
				damage += damage * (1.0 - (trEntFraction[i] / PHASER_POINT_BLANK_FRAC));
			} else {
				/* Normal range */
				damage -= (int32_t)(trEntFraction[i] * 5.0);
			}

			if (ent->client->ps.ammo[WP_5] == 0) {
				damage *= .35; /* weak out-of-ammo phaser */
			}

			if (damage > 0) {
				if (alt_fire) {
					G_Combat_Damage(traceEnt, ent, ent, forward, tr.endpos, damage,
									DAMAGE_NO_KNOCKBACK | DAMAGE_NOT_ARMOR_PIERCING, MOD_PHASER_ALT);
				} else {
					G_Combat_Damage(traceEnt, ent, ent, forward, tr.endpos, damage,
									DAMAGE_NO_KNOCKBACK | DAMAGE_ARMOR_PIERCING, MOD_PHASER);
				}
			}
		}
	}

	G_LogFuncEnd();
}


/*
----------------------------------------------
COMPRESSION RIFLE
----------------------------------------------
*/

#define MAXRANGE_CRIFLE		8192
#define CRIFLE_SIZE			1  /* RPG-X | Marcin | 04/12/2008 */

/**
 * @brief Fires a new compression rifle bullet.
 * @param ent the player
 * @param start start point
 * @param end end point
 */
static void FirePrifleBullet(gentity_t* ent, vec3_t start, vec3_t dir) {
	gentity_t* bolt = NULL;

	G_LogFuncBegin();

	G_Assert(ent, (void)0);

	bolt = G_Spawn();
	G_Assert(bolt, (void)0);

	bolt->classname = "prifle_proj";
	bolt->nextthink = level.time + 10000;
	bolt->think = G_FreeEntity;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_6;
	bolt->r.ownerNum = ent->s.number;
	bolt->parent = ent;

	/* Flags effect as being the full beefy version for the player */
	bolt->count = 0;

	if ((rpg_dmgFlags.integer & 2) != 0) {
		bolt->damage = CRIFLE_DAMAGE * DMG_VAR;
	} else {
		bolt->damage = 0;
	}

	bolt->splashDamage = 0;
	bolt->splashRadius = 0;
	bolt->methodOfDeath = MOD_CRIFLE;
	bolt->clipmask = MASK_SHOT;

	/* Set the size of the missile up */
	VectorSet(bolt->r.maxs, CRIFLE_SIZE >> 1, CRIFLE_SIZE, CRIFLE_SIZE >> 1);
	VectorSet(bolt->r.mins, -CRIFLE_SIZE >> 1, -CRIFLE_SIZE, -CRIFLE_SIZE >> 1);

	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - 10; 			/* move a bit on the very first frame */
	VectorCopy(start, bolt->s.pos.trBase);
	SnapVector(bolt->s.pos.trBase);			/* save net bandwidth */
	VectorScale(dir, rpg_rifleSpeed.integer, bolt->s.pos.trDelta);
	SnapVector(bolt->s.pos.trDelta);			/* save net bandwidth */
	VectorCopy(start, bolt->r.currentOrigin);

	G_LogFuncEnd();
}

/**
 * @brief Handles weapon fire of the compression rifle.
 * @param ent the player
 * @param alt_fire was this alt fire mode?
 * @todo additonal logging messages
 */
static void WP_FireCompressionRifle(gentity_t* ent, qboolean alt_fire) {
	G_LogFuncBegin();

	G_Assert(ent, (void)0);

	if (!alt_fire) {
		vec3_t dir = { 0, 0, 0 };
		vec3_t angles = { 0, 0, 0 };
		vec3_t temp_ang = { 0, 0, 0 };
		vec3_t temp_org = { 0, 0, 0 };
		vec3_t start = { 0, 0, 0 };

		VectorCopy(forward, dir);
		VectorCopy(muzzle, start);

		vectoangles(dir, angles);
		VectorSet(temp_ang, angles[0], angles[1], angles[2]);
		AngleVectors(temp_ang, dir, NULL, NULL);

		/* FIXME:  These offsets really don't work like they should */
		VectorMA(start, 0, right, temp_org);
		VectorMA(temp_org, 0, up, temp_org);
		FirePrifleBullet(ent, temp_org, dir); /* temp_org */

		G_LogWeaponFire(ent->s.number, WP_6);
	} else {
		trace_t		tr;
		vec3_t		end = { 0, 0, 0 };
		gentity_t*  traceEnt = NULL;
		int32_t		damage = 0;

		VectorMA(muzzle, MAXRANGE_PHASER, forward, end);

		/* Find out who we've hit */
		memset(&tr, 0, sizeof(trace_t));
		trap_Trace(&tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT);

		if (tr.entityNum == (MAX_GENTITIES - 1)) {
			G_LogFuncEnd();
			return;
		}

		traceEnt = &g_entities[tr.entityNum];

		G_Assert(traceEnt, (void)0);

		if (traceEnt->takedamage && ((rpg_dmgFlags.integer & 2) != 0)) {
			damage = (double)PHASER_DAMAGE;

			if (tr.fraction <= PHASER_POINT_BLANK_FRAC) {
				/* Point blank!  Do up to double damage. */
				damage += damage * (1.0 - (tr.fraction / PHASER_POINT_BLANK_FRAC));
			} else {	/* Normal range */
				damage -= (int32_t)(tr.fraction * 5.0);
			}

			if (damage > 0) {
				G_Combat_Damage(traceEnt, ent, ent, forward, tr.endpos, damage,
								DAMAGE_NO_KNOCKBACK | DAMAGE_ARMOR_PIERCING, MOD_CRIFLE_ALT); /* GSIO01: was MOD_PHASER */
			}
		}
	}

	G_LogFuncEnd();
}

/*
----------------------------------------------
SCAVENGER
----------------------------------------------
*/
#define SCAV_SIZE		3
#define SCAV_ALT_SIZE	6

/*
----------------------------------------------
STASIS
----------------------------------------------
*/

#define STASIS_MAIN_MISSILE_BIG		1

/**
 * \brief Fires a disruptor missile.
 *
 * Creates a disruptor bullet entity and sets it up.
 *
 * @param the player
 * @param origin the start point
 * @param dir the direction
 * @param size the size
 */
static void FireDisruptorMissile(gentity_t* ent, vec3_t origin, vec3_t dir, int32_t size) {
	gentity_t*	bolt = NULL;
	int32_t		boltsize = 0;

	G_LogFuncBegin();

	G_Assert(ent, (void)0);

	bolt = G_Spawn();
	G_Assert(bolt, (void)0);

	bolt->classname = "disruptor_projectile";
	bolt->nextthink = level.time + 10000;
	bolt->think = G_FreeEntity;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_10;
	bolt->r.ownerNum = ent->s.number;
	bolt->parent = ent;

	if ((rpg_dmgFlags.integer & 32) != 0) {
		bolt->damage = STASIS_DAMAGE*DMG_VAR;
	} else {
		bolt->damage = 0;
	}

	bolt->splashDamage = 0;
	bolt->splashRadius = 0;
	bolt->methodOfDeath = MOD_STASIS_ALT; /* GSIO01: was MOD_TETRION_ALT */
	bolt->clipmask = MASK_SHOT;

	/* Set the size of the missile up */
	boltsize = 3 * size;
	VectorSet(bolt->r.maxs, boltsize >> 1, boltsize, boltsize >> 1);
	boltsize = -boltsize;
	VectorSet(bolt->r.mins, boltsize >> 1, boltsize, boltsize >> 1);

	/* There are going to be a couple of different sized projectiles, so store 'em here */
	bolt->count = size;
	/* kef -- need to keep the size in something that'll reach the cgame side */
	bolt->s.time2 = size;

	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time;
	VectorCopy(origin, bolt->s.pos.trBase);
	SnapVector(bolt->s.pos.trBase);			/* save net bandwidth */

	VectorScale(dir, rpg_disruptorSpeed.integer + (50 * size), bolt->s.pos.trDelta); /* RPG-X | Marcin | 05/12/2008 */

	SnapVector(bolt->s.pos.trDelta);			/* save net bandwidth */
	VectorCopy(origin, bolt->r.currentOrigin);
	/* Used by trails */
	VectorCopy(origin, bolt->pos1);
	VectorCopy(origin, bolt->pos2);
	/* kef -- need to keep the origin in something that'll reach the cgame side */
	VectorCopy(origin, bolt->s.angles2);
	SnapVector(bolt->s.angles2);			/* save net bandwidth */

	G_LogFuncEnd();
}

/**
 * @brief Handles firing of the dirsuptor.
 * @param ent the player
 * @param alt_fire was this alt fire mode?
 */
static void WP_FireDisruptor(gentity_t* ent, qboolean alt_fire) {
	G_LogFuncBegin();

	G_Assert(ent, (void)0);

	/* This was moved out of the FireWeapon switch statement below to keep things more consistent */
	if (!alt_fire) {
		trace_t		tr;
		vec3_t		end = { 0, 0, 0 };
		gentity_t*	traceEnt = NULL;
		int32_t		damage = 0;

		VectorMA(muzzle, MAXRANGE_PHASER, forward, end);

		/* Find out who we've hit */
		memset(&tr, 0, sizeof(trace_t));
		trap_Trace(&tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT);

		if (tr.entityNum == (MAX_GENTITIES - 1)) {
			G_Logger(LL_DEBUG, "Invalid entity number.\n");
			G_LogFuncEnd();
			return;
		}

		traceEnt = &g_entities[tr.entityNum];

		G_Assert(traceEnt, (void)0);

		if (traceEnt->takedamage && ((rpg_dmgFlags.integer & 32) != 0)) {
			damage = (double)PHASER_DAMAGE;

			if (tr.fraction <= PHASER_POINT_BLANK_FRAC) {
				/* Point blank!  Do up to double damage. */
				damage += damage * (1.0 - (tr.fraction / PHASER_POINT_BLANK_FRAC));
			} else {	/* Normal range */
				damage -= (int32_t)(tr.fraction*5.0);
			}

			if (damage > 0) {
				G_Combat_Damage(traceEnt, ent, ent, forward, tr.endpos, damage,
								DAMAGE_NO_KNOCKBACK | DAMAGE_ARMOR_PIERCING, MOD_STASIS); /* GSIO01: was MOD_TETRION_ALT */
			}
		}
	} else {
		FireDisruptorMissile(ent, muzzle, forward, STASIS_MAIN_MISSILE_BIG);
	}

	G_LogWeaponFire(ent->s.number, WP_10);

	G_LogFuncEnd();
}

/*
----------------------------------------------
GRENADE LAUNCHER
----------------------------------------------
*/

#define GRENADE_VELOCITY		1000
#define GRENADE_TIME			2000
#define GRENADE_SIZE			4
#define GRENADE_ALT_TIME		2500

/**
 * \brief Exploding a grenade.
 *
 * Handles all damage and visual effects for a exploding grenade.
 *
 * @param ent the grenade
 */
static void grenadeExplode(gentity_t* ent) {
	vec3_t pos = { 0, 0, 0 };

	G_LogFuncBegin();

	G_Assert(ent, (void)0);

	VectorSet(pos, ent->r.currentOrigin[0], ent->r.currentOrigin[1], ent->r.currentOrigin[2] + 8);

	G_TempEntity(pos, EV_GRENADE_EXPLODE);

	/* splash damage (doesn't apply to person directly hit) */
	if (ent->splashDamage > 0) {
		G_Combat_RadiusDamage(pos, ent->parent, ent->splashDamage, ent->splashRadius, NULL, 0, ent->splashMethodOfDeath);
	}
	G_FreeEntity(ent);

	G_LogFuncEnd();
}

/**
 * @brief Handles grenade shrapnels.
 * @param ent the grenade
 */
void grenadeSpewShrapnel(gentity_t* ent) {
	gentity_t* tent = NULL;

	G_LogFuncBegin();

	G_Assert(ent, (void)0);

	tent = G_TempEntity(ent->r.currentOrigin, EV_GRENADE_SHRAPNEL_EXPLODE);
	G_Assert(tent, (void)0);

	tent->s.eventParm = DirToByte(ent->pos1);

	// just do radius dmg for altfire
	G_Combat_RadiusDamage(ent->r.currentOrigin, ent->parent, ent->splashDamage, ent->splashRadius,
						  ent, 0, ent->splashMethodOfDeath);

	G_FreeEntity(ent);

	G_LogFuncEnd();
}

/**
 * @brief Handles firing the grenade launcher.
 * @param ent the player
 * @param alt_fire was this alt fire mode?
 * @todo additional logging messages
 */
static void WP_FireGrenade(gentity_t* ent, qboolean alt_fire) {
	gentity_t*	grenade = NULL;
	gentity_t*	tripwire = NULL;
	gentity_t*	tent = NULL;
	vec3_t		dir = { 0, 0, 0 };
	vec3_t		start = { 0, 0, 0 };
	vec3_t		end = { 0, 0, 0 };
	int32_t		tripcount = 0;
	int32_t		foundTripWires[MAX_GENTITIES] = { ENTITYNUM_NONE };
	int32_t		tripcount_org = 0;
	int32_t		lowestTimeStamp = 0;
	int32_t		removeMe = 0;
	int32_t		i = 0;
	trace_t		tr;

	G_LogFuncBegin();

	G_Assert(ent, (void)0);

	VectorCopy(forward, dir);
	VectorCopy(muzzle, start);

	if (RPGEntityCount != ENTITYNUM_MAX_NORMAL - 20) {
		if (alt_fire) {
			/* RPG-X: RedTechie - Moved here to stop entities from being sucked up */
			grenade = G_Spawn();
			G_Assert(grenade, (void)0);

			/* kef -- make sure count is 0 so it won't get its bounciness removed like the tetrion projectile */
			grenade->count = 0;

			/* RPG-X: RedTechie - Forced Tripwires */
			if (rpg_invisibletripmines.integer == 1) {
				/*
				 * limit to 10 placed at any one time
				 * see how many there are now
				 */

				while ((tripwire = G_Find(tripwire, FOFS(classname), "tripwire")) != NULL) {
					if (tripwire->parent != ent) {
						continue;
					}
					foundTripWires[tripcount++] = tripwire->s.number;
				}

				/* now remove first ones we find until there are only 9 left */
				tripwire = NULL;
				tripcount_org = tripcount;
				lowestTimeStamp = level.time;

				/* RPG-X: RedTechie - Added 51 tripwires for each person */
				while (tripcount > 50) { /* 9 */
					removeMe = -1;

					for (i = 0; i < tripcount_org; i++) {
						if (foundTripWires[i] == ENTITYNUM_NONE) {
							continue;
						}
						tripwire = &g_entities[foundTripWires[i]];

						if (tripwire == NULL) {
							G_LocLogger(LL_WARN, "tripwire == NULL\n");
							continue;
						}

						if (tripwire && tripwire->timestamp < lowestTimeStamp) {
							removeMe = i;
							lowestTimeStamp = tripwire->timestamp;
						}
					}

					if (removeMe != -1) {
						/* remove it... or blow it? */
						if (&g_entities[foundTripWires[removeMe]] == NULL) {
							break;
						} else {
							G_FreeEntity(&g_entities[foundTripWires[removeMe]]);
						}
						foundTripWires[removeMe] = ENTITYNUM_NONE;
						tripcount--;
					} else {
						break;
					}
				}

				/* now make the new one */
				grenade->classname = "tripwire";
				if ((rpg_dmgFlags.integer & 8) != 0) {
					grenade->splashDamage = GRENADE_SPLASH_DAM * 2;
					grenade->splashRadius = GRENADE_SPLASH_RAD * 2;
				} else {
					grenade->splashDamage = 0;
					grenade->splashRadius = 0;

				}

				grenade->s.pos.trType = TR_LINEAR;
				grenade->nextthink = level.time + 1000; /* How long 'til she blows */
				grenade->count = 1; 			/* tell it it's a tripwire for when it sticks */
				grenade->timestamp = level.time; 	/* remember when we placed it */
				grenade->s.otherEntityNum2 = ent->client->sess.sessionTeam;
			} else {
				grenade->classname = "grenade_alt_projectile";

				if ((rpg_dmgFlags.integer & 8) != 0) {
					grenade->splashDamage = GRENADE_SPLASH_DAM;
					grenade->splashRadius = GRENADE_SPLASH_RAD;
				} else {
					grenade->splashDamage = 0;
					grenade->splashRadius = 0;
				}

				grenade->s.pos.trType = TR_GRAVITY;
				grenade->nextthink = level.time + GRENADE_ALT_TIME; /* How long 'til she blows */
			}
			grenade->think = grenadeSpewShrapnel;
			grenade->s.eFlags |= EF_MISSILE_STICK;
			VectorScale(dir, 1000, grenade->s.pos.trDelta);

			grenade->damage = (rpg_dmgFlags.integer & 8) ? (GRENADE_ALT_DAMAGE*DMG_VAR) : (grenade->damage = 0);
			grenade->methodOfDeath = MOD_GRENADE_ALT;
			grenade->splashMethodOfDeath = MOD_GRENADE_ALT_SPLASH;
			grenade->s.eType = ET_ALT_MISSILE;

			/* RPG-X: RedTechie - Moved here to stop entities from being sucked up */
			grenade->r.svFlags = SVF_USE_CURRENT_ORIGIN;
			grenade->s.weapon = WP_8;
			grenade->r.ownerNum = ent->s.number;
			grenade->parent = ent;

			VectorSet(grenade->r.mins, -GRENADE_SIZE, -GRENADE_SIZE, -GRENADE_SIZE);
			VectorSet(grenade->r.maxs, GRENADE_SIZE, GRENADE_SIZE, GRENADE_SIZE);

			grenade->clipmask = MASK_SHOT;

			grenade->s.pos.trTime = level.time;		/* move a bit on the very first frame */
			VectorCopy(start, grenade->s.pos.trBase);
			SnapVector(grenade->s.pos.trBase);		/* save net bandwidth */

			SnapVector(grenade->s.pos.trDelta);		/* save net bandwidth */
			VectorCopy(start, grenade->r.currentOrigin);

			VectorCopy(start, grenade->pos2);
		} else {
			/* RPG-X: RedTechie - Check to see if there admin if so grant them effects gun */
			if (G_Client_IsAdmin(ent) && (rpg_effectsgun.integer == 1)) {
				VectorMA(muzzle, MAXRANGE_CRIFLE, forward, end);
				trap_Trace(&tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT);

				/*
				 * TiM : FX Gun additional effects.
				 * Okay... screw the generic args. it's giving me a headache
				 * Case in this case... harhar is teh solution
				 */
				if (ent->client->fxGunData.eventNum > 0) {
					fxGunData_t *fxGunData = &ent->client->fxGunData;

					/* set the entity event */
					tent = G_TempEntity(tr.endpos, fxGunData->eventNum);

					G_Assert(tent, (void)0);

					/* based on the event, add additional args */
					switch (fxGunData->eventNum) {
						/* sparks */
						case EV_FX_SPARK:
							/* Direction vector based off of trace normal */
							VectorCopy(tr.plane.normal, tent->s.angles2);
							VectorShort(tent->s.angles2);

							/* spark interval */
							tent->s.time2 = fxGunData->arg_float1;
							/* spark time length */
							tent->s.time = fxGunData->arg_int2;
							break;
						case EV_FX_STEAM:
							/* Direction vector based off of trace normal */
							VectorCopy(tr.plane.normal, tent->s.angles2);
							VectorShort(tent->s.angles2);
							/* time length */
							tent->s.time = fxGunData->arg_int2;
							break;
						case EV_FX_FIRE:
							VectorCopy(tr.plane.normal, tent->s.angles2);
							VectorShort(tent->s.angles2);
							tent->s.time = fxGunData->arg_int1;
							tent->s.time2 = fxGunData->arg_int2;
							break;
						case EV_FX_SHAKE:
							VectorCopy(tr.plane.normal, tent->s.angles2);
							VectorShort(tent->s.angles2);
							tent->s.time = fxGunData->arg_int1;
							tent->s.time2 = fxGunData->arg_int2;
							break;
						case EV_FX_CHUNKS:
							/* normal direction */
							VectorCopy(tr.plane.normal, tent->s.angles2);
							VectorShort(tent->s.angles2);

							/* scale/radius */
							tent->s.time2 = fxGunData->arg_int1;
							/* material type */
							tent->s.powerups = fxGunData->arg_int2;
							break;
						case EV_FX_DRIP:
							/* type of drip */
							tent->s.time2 = fxGunData->arg_int1;
							/* degree of drippiness */
							tent->s.angles2[0] = fxGunData->arg_float1;
							/* length of effect */
							tent->s.powerups = fxGunData->arg_int2;
							break;
						case EV_FX_SMOKE:
							/* Direction vector based off of trace normal */
							VectorCopy(tr.plane.normal, tent->s.angles2);
							VectorShort(tent->s.angles2);
							/* smoke radius */
							tent->s.time = fxGunData->arg_int1;
							/* killtime  */
							tent->s.time2 = fxGunData->arg_int2;

							/* set ent origin for dir calcs */
							VectorCopy(tent->s.origin, tent->s.origin2);
							/* VectorMA( tent->s.origin2, 6, tr.plane.normal, tent->s.origin2 ); */
							tent->s.origin2[2] += 6;
							break;
						case EV_FX_SURFACE_EXPLOSION:
							/* radius */
							tent->s.angles2[0] = fxGunData->arg_float1;
							/* camera shake */
							tent->s.angles2[1] = fxGunData->arg_float2;
							/* orient the dir to the plane we shot at */
							VectorCopy(tr.plane.normal, tent->s.origin2);
							/* Meh... generic hardcoded data for the rest lol */
							tent->s.time2 = 0;
							break;
						case EV_FX_ELECTRICAL_EXPLOSION:
							/* Set direction */
							VectorCopy(tr.plane.normal, tent->s.origin2);
							/* Set Radius */
							tent->s.angles2[0] = fxGunData->arg_float1;
							break;
					}

					/* Little hack to make the Detpack sound global */
					if (fxGunData->eventNum == EV_DETPACK) {
						gentity_t* te = NULL;

						te = G_TempEntity(tr.endpos, EV_GLOBAL_SOUND);

						G_Assert(te, (void)0);

						te->s.eventParm = G_SoundIndex("sound/weapons/explosions/detpakexplode.wav");
						te->r.svFlags |= SVF_BROADCAST;
					}
				} else {
					tent = G_TempEntity(tr.endpos, EV_EFFECTGUN_SHOOT);

					G_Assert(tent, (void)0);
				}

				tent->s.eFlags |= EF_FIRING;

			} else {
				/* RPG-X: RedTechie - Moved here to stop entities from being sucked up */
				grenade = G_Spawn();

				G_Assert(grenade, (void)0);

				/* kef -- make sure count is 0 so it won't get its bounciness removed like the tetrion projectile */
				grenade->count = 0;
				grenade->classname = "grenade_projectile";
				grenade->nextthink = level.time + GRENADE_TIME; /* How long 'til she blows */
				grenade->think = grenadeExplode;
				grenade->s.eFlags |= EF_BOUNCE_HALF;
				VectorScale(dir, GRENADE_VELOCITY, grenade->s.pos.trDelta);
				grenade->s.pos.trType = TR_GRAVITY;

				if ((rpg_dmgFlags.integer & 8) != 0) {
					grenade->damage = GRENADE_DAMAGE*DMG_VAR;
					grenade->splashDamage = GRENADE_SPLASH_DAM;
					grenade->splashRadius = GRENADE_SPLASH_RAD;
				} else {
					grenade->damage = 0;
					grenade->splashDamage = 0;
					grenade->splashRadius = 0;
				}
				grenade->methodOfDeath = MOD_GRENADE;
				grenade->splashMethodOfDeath = MOD_GRENADE_SPLASH;
				grenade->s.eType = ET_MISSILE;

				/* RPG-X: RedTechie - Moved here to stop entities from being sucked up */
				grenade->r.svFlags = SVF_USE_CURRENT_ORIGIN;
				grenade->s.weapon = WP_8;
				grenade->r.ownerNum = ent->s.number;
				grenade->parent = ent;

				VectorSet(grenade->r.mins, -GRENADE_SIZE, -GRENADE_SIZE, -GRENADE_SIZE);
				VectorSet(grenade->r.maxs, GRENADE_SIZE, GRENADE_SIZE, GRENADE_SIZE);

				grenade->clipmask = MASK_SHOT;

				grenade->s.pos.trTime = level.time;		/* move a bit on the very first frame */
				VectorCopy(start, grenade->s.pos.trBase);
				SnapVector(grenade->s.pos.trBase);		/* save net bandwidth */

				SnapVector(grenade->s.pos.trDelta);		/* save net bandwidth */
				VectorCopy(start, grenade->r.currentOrigin);

				VectorCopy(start, grenade->pos2);
			}
		}

		G_LogWeaponFire(ent->s.number, WP_8);
	} else {
		G_LogPrintf("RPG-X WARNING: Max entities about to be hit! Restart the server ASAP or suffer a server crash!\n");
		trap_SendServerCommand(-1, va("print \"^1RPG-X WARNING: Max entities about to be hit! Restart the server ASAP or suffer a server crash!\n\""));
	}

	G_LogFuncEnd();
}

/*
----------------------------------------------
TETRION
----------------------------------------------
*/

#define TETRION_ALT_SIZE	6

#define MAX_TR_116_DIST		8192
#define MAX_TRACES			24 /* Number of traces thru walls we'll do before we give up lol */

/**
 * \brief Fire a TR116 bullet.
 *
 * Creates and sets up an TR116 bullet entity.
 *
 * @param ent the player
 * @param start the start point
 * @param dir the direction
 */
static void WP_FireTR116Bullet(gentity_t* ent, vec3_t start, vec3_t dir) {
	gentity_t*	traceEnt = NULL;
	vec3_t 		end = { 0, 0, 0 }; 		/* end-point in trace */
	vec3_t 		traceFrom = { 0, 0, 0 };
	trace_t		tr;

	G_LogFuncBegin();

	G_Assert(ent, (void)0);

	VectorCopy(start, traceFrom);
	VectorMA(traceFrom, MAX_TR_116_DIST, dir, end); /* set trace end point */

	memset(&tr, 0, sizeof(trace_t));
	trap_Trace(&tr, traceFrom, NULL, NULL, end, ent->s.number, CONTENTS_BODY); /* MASK_SHOT - TiM - Goes thru everything but players */

	if (tr.entityNum < ENTITYNUM_MAX_NORMAL) {

		traceEnt = &g_entities[tr.entityNum];

		G_Assert(traceEnt, (void)0);

		if (traceEnt->takedamage && (rpg_dmgFlags.integer & 4) != 0) {
			G_Combat_Damage(traceEnt, ent, ent, dir, tr.endpos, TETRION_DAMAGE, 0, MOD_TETRION_ALT);
		}
	}

	G_LogFuncEnd();
}

/* (RPG-X: J2J MOdified to make it look and feel like tr116 */
/* RPG-X: TiM - Modified even furthur */
/**
 * @brief Handles firing of the TR116 rifle.
 * @param ent the player
 * @param alt_fire was this alt fire mode?
 * @todo rename me?
 */
static void WP_FireTR116(gentity_t* ent, qboolean alt_fire) {
	G_LogFuncBegin();

	G_Assert(ent, (void)0);

	vec3_t	dir = { 0, 0, 0 };
	vec3_t	start = { 0, 0, 0 };

	VectorCopy(forward, dir);
	VectorCopy(muzzle, start);

	WP_FireTR116Bullet(ent, start, dir);

	G_LogWeaponFire(ent->s.number, WP_7);

	G_LogFuncEnd();
}


/*
----------------------------------------------
QUANTUM BURST
----------------------------------------------
*/

#define QUANTUM_SIZE			1 
#define QUANTUM_ALT_THINK_TIME	300
#define QUANTUM_ALT_SEARCH_TIME	100
#define QUANTUM_ALT_SEARCH_DIST	4096

/**
 * \brief Fires a Quantum Burst.
 *
 * Creates and sets up an Quantum Burst projectile.
 *
 * @param ent the player
 * @param start the start point
 * @param dir the direction
 */
static void FireQuantumBurst(gentity_t* ent, vec3_t start, vec3_t dir) {
	gentity_t* bolt = NULL;

	G_LogFuncBegin();

	G_Assert(ent, (void)0);

	bolt = G_Spawn();
	G_Assert(bolt, (void)0);

	bolt->classname = "quantum_projectile";

	bolt->nextthink = level.time + 6000;
	bolt->think = G_FreeEntity;

	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_9;
	bolt->r.ownerNum = ent->s.number;
	bolt->parent = ent;

	if ((rpg_dmgFlags.integer & 16) != 0) {
		bolt->damage = QUANTUM_DAMAGE*DMG_VAR;
		bolt->splashDamage = QUANTUM_SPLASH_DAM;
		bolt->splashRadius = QUANTUM_SPLASH_RAD;
	} else {
		bolt->damage = 0;
		bolt->splashDamage = 0;
		bolt->splashRadius = 0;
	}

	bolt->methodOfDeath = MOD_QUANTUM;
	bolt->splashMethodOfDeath = MOD_QUANTUM_SPLASH;
	bolt->clipmask = MASK_SHOT;

	VectorSet(bolt->r.mins, -QUANTUM_SIZE, -QUANTUM_SIZE, -QUANTUM_SIZE);
	VectorSet(bolt->r.maxs, QUANTUM_SIZE, QUANTUM_SIZE, QUANTUM_SIZE);

	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time;		/* move a bit on the very first frame */
	VectorCopy(start, bolt->s.pos.trBase);
	SnapVector(bolt->s.pos.trBase);		/* save net bandwidth */

	VectorScale(dir, rpg_photonSpeed.integer, bolt->s.pos.trDelta);

	SnapVector(bolt->s.pos.trDelta);		/* save net bandwidth */
	VectorCopy(start, bolt->r.currentOrigin);
	VectorCopy(start, bolt->pos1);

	G_LogFuncEnd();
}

/**
 * @brief Search a target for quantum burst alt fire projectile.
 * @param ent the projectile
 * @param start start point
 * @param end end point
 */
static qboolean SearchTarget(gentity_t* ent, vec3_t start, vec3_t end) {
	trace_t		tr;
	gentity_t*	traceEnt = NULL;
	vec3_t		fwd = { 0, 0, 0 };

	G_LogFuncBegin();

	G_Assert(ent, qfalse);

	memset(&tr, 0, sizeof(trace_t));
	trap_Trace(&tr, start, NULL, NULL, end, ent->s.number, MASK_SHOT);
	traceEnt = &g_entities[tr.entityNum];

	G_Assert(traceEnt, qfalse);

	if (traceEnt->takedamage && traceEnt->client) {
		ent->target_ent = traceEnt;
		VectorSubtract(ent->target_ent->r.currentOrigin, ent->r.currentOrigin, fwd);
		VectorNormalize(fwd);
		VectorScale(fwd, rpg_altPhotonSpeed.integer, ent->s.pos.trDelta);
		VectorCopy(fwd, ent->movedir);
		SnapVector(ent->s.pos.trDelta);	/* save net bandwidth */
		VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
		ent->s.pos.trTime = level.time;
		ent->nextthink = level.time + QUANTUM_ALT_THINK_TIME;

		G_LogFuncEnd();
		return qtrue;
	}

	G_LogFuncEnd();
	return qfalse;
}

/**
 * @brief Alt quantum burst projectile think functiom.
 * @param ent the projectile
 */
static void WP_QuantumAltThink(gentity_t* ent) {
	vec3_t start = { 0, 0, 0 };
	vec3_t newdir = { 0, 0, 0 };
	vec3_t targetdir = { 0, 0, 0 };
	vec3_t lup = { 0, 0, 1 };
	vec3_t lright = { 0, 0, 0 };
	vec3_t search = { 0, 0, 0 };
	double dot = 0.0;
	double dot2 = 0.0;

	G_LogFuncBegin();

	G_Assert(ent, (void)0);

	ent->health--;
	if (ent->health <= 0) {
		G_FreeEntity(ent);
		G_Logger(LL_DEBUG, "entities health <= 0\n");
		G_LogFuncEnd();
		return;
	}

	if (ent->target_ent) {
		/* Already have a target, start homing. */

		if (ent->health <= 0 || !ent->inuse) {
			/* No longer target this */
			ent->target_ent = NULL;
			ent->nextthink = level.time + 1000;
			ent->health -= 5;

			G_Logger(LL_DEBUG, "entities health <= 0 or entitiy not in use\n");
			G_LogFuncEnd();
			return;
		}
		VectorSubtract(ent->target_ent->r.currentOrigin, ent->r.currentOrigin, targetdir);
		VectorNormalize(targetdir);

		/* Now the rocket can't do a 180 in space, so we'll limit the turn to about 45 degrees. */
		dot = DotProduct(targetdir, ent->movedir);
		/* a dot of 1.0 means right-on-target. */
		if (dot < 0.0) {
			/* Go in the direction opposite, start a 180. */
			CrossProduct(ent->movedir, lup, lright);
			dot2 = DotProduct(targetdir, lright);

			if (dot2 > 0) {
				/* Turn 45 degrees right. */
				VectorAdd(ent->movedir, lright, newdir);
			} else {
				/* Turn 45 degrees left. */
				VectorSubtract(ent->movedir, lright, newdir);
			}

			/* Yeah we've adjusted horizontally, but let's split the difference vertically, so we kinda try to move towards it. */
			newdir[2] = (targetdir[2] + ent->movedir[2]) * 0.5;
			VectorNormalize(newdir);
		} else if (dot < 0.7) {	/* Need about one correcting turn.  Generate by meeting the target direction "halfway". */
			/* Note, this is less than a 45 degree turn, but it is sufficient.  We do this because the rocket may have to go UP. */
			VectorAdd(ent->movedir, targetdir, newdir);
			VectorNormalize(newdir);
		} else {	/* else adjust to right on target. */
			VectorCopy(targetdir, newdir);
		}

		VectorScale(newdir, rpg_altPhotonSpeed.integer, ent->s.pos.trDelta);
		VectorCopy(newdir, ent->movedir);
		SnapVector(ent->s.pos.trDelta);			/* save net bandwidth */
		VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
		SnapVector(ent->s.pos.trBase);
		ent->s.pos.trTime = level.time;

		/* Home at a reduced frequency. */
		ent->nextthink = level.time + QUANTUM_ALT_THINK_TIME;	/* Nothing at all spectacular happened, continue. */
	} else {	/* Search in front of the missile for targets. */
		VectorCopy(ent->r.currentOrigin, start);
		CrossProduct(ent->movedir, lup, lright);

		/* Search straight ahead. */
		VectorMA(start, QUANTUM_ALT_SEARCH_DIST, ent->movedir, search);

		/* Add some small randomness to the search Z height, to give a bit of variation to where we are searching. */
		search[2] += flrandom(-QUANTUM_ALT_SEARCH_DIST*0.075, QUANTUM_ALT_SEARCH_DIST*0.075);

		if (SearchTarget(ent, start, search)) {
			G_Logger(LL_DEBUG, "no valid entity found\n");
			G_LogFuncEnd();
			return;
		}

		/* Search to the right. */
		VectorMA(search, QUANTUM_ALT_SEARCH_DIST*0.1, lright, search);
		if (SearchTarget(ent, start, search)) {
			G_Logger(LL_DEBUG, "no valid entity found\n");
			G_LogFuncEnd();
			return;
		}

		/* Search to the left. */
		VectorMA(search, -QUANTUM_ALT_SEARCH_DIST*0.2, lright, search);
		if (SearchTarget(ent, start, search)) {
			G_Logger(LL_DEBUG, "no valid entity found\n");
			G_LogFuncEnd();
			return;
		}

		/* Search at a higher rate than correction. */
		ent->nextthink = level.time + QUANTUM_ALT_SEARCH_TIME;	/* Nothing at all spectacular happened, continue. */

	}

	G_LogFuncEnd();
	return;
}

/**
 * @brief Fire quantum burst alt fire mode.
 * @param ent the player
 * @param start start point
 * @param dir the direction
 */
static void FireQuantumBurstAlt(gentity_t* ent, vec3_t start, vec3_t dir) {
	gentity_t* bolt = NULL;

	G_LogFuncBegin();

	G_Assert(ent, (void)0);

	bolt = G_Spawn();
	G_Assert(bolt, (void)0);

	bolt->classname = "quantum_alt_projectile";

	bolt->nextthink = level.time + 100;
	bolt->think = WP_QuantumAltThink;
	bolt->health = 25;		/* 10 seconds. */

	bolt->s.eType = ET_ALT_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_9;
	bolt->r.ownerNum = ent->s.number;
	bolt->parent = ent;
	bolt->s.eFlags |= EF_ALT_FIRING;

	if ((rpg_dmgFlags.integer & 16) != 0) {
		bolt->damage = QUANTUM_ALT_DAMAGE*DMG_VAR;
		bolt->splashDamage = QUANTUM_ALT_SPLASH_DAM;
		bolt->splashRadius = QUANTUM_ALT_SPLASH_RAD;
	} else {
		bolt->damage = 0;
		bolt->splashDamage = 0;
		bolt->splashRadius = 0;
	}

	bolt->methodOfDeath = MOD_QUANTUM_ALT;
	bolt->splashMethodOfDeath = MOD_QUANTUM_ALT_SPLASH;
	bolt->clipmask = MASK_SHOT;

	VectorSet(bolt->r.mins, -QUANTUM_SIZE, -QUANTUM_SIZE, -QUANTUM_SIZE);
	VectorSet(bolt->r.maxs, QUANTUM_SIZE, QUANTUM_SIZE, QUANTUM_SIZE);

	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time;		/* move a bit on the very first frame */
	VectorCopy(start, bolt->s.pos.trBase);
	SnapVector(bolt->s.pos.trBase);

	VectorScale(dir, rpg_altPhotonSpeed.integer, bolt->s.pos.trDelta);
	VectorCopy(dir, bolt->movedir);

	SnapVector(bolt->s.pos.trDelta);			/* save net bandwidth */
	VectorCopy(start, bolt->r.currentOrigin);

	G_LogFuncEnd();
}

/**
 * @brief Handles firing of the quatum burst.
 * @param ent the player
 * @param alt_fire was this alt fire mode?
 */
static void WP_FireQuantumBurst(gentity_t* ent, qboolean alt_fire) {
	vec3_t dir = { 0, 0, 0 };
	vec3_t start = { 0, 0, 0 };

	G_LogFuncBegin();

	G_Assert(ent, (void)0);

	VectorCopy(forward, dir);
	VectorCopy(muzzle, start);

	if (alt_fire) {
		FireQuantumBurstAlt(ent, start, dir);
	} else {
		FireQuantumBurst(ent, start, dir);
	}

	G_LogWeaponFire(ent->s.number, WP_9);

	G_LogFuncEnd();
}

qboolean G_Weapon_LogAccuracyHit(gentity_t* target, gentity_t* attacker) {
	G_LogFuncBegin();

	G_Assert(target, qfalse);
	G_Assert(attacker, qfalse);

	if (!target->takedamage) {
		G_Logger(LL_DEBUG, "target does not take damage\n");
		G_LogFuncEnd();
		return qfalse;
	}

	if (target == attacker) {
		G_Logger(LL_DEBUG, "target = attacker\n");
		G_LogFuncEnd();
		return qfalse;
	}

	if (!target->client) {
		G_Logger(LL_DEBUG, "target not a client\n");
		G_LogFuncEnd();
		return qfalse;
	}

	if (!attacker->client) {
		G_Logger(LL_DEBUG, "attacker not a client\n");
		G_LogFuncEnd();
		return qfalse;
	}

	if (target->client->ps.stats[STAT_HEALTH] <= 0) {
		G_Logger(LL_DEBUG, "target is dead\n");
		G_LogFuncEnd();
		return qfalse;
	}

	G_LogFuncEnd();
	return qtrue;
}

#define MAX_FORWARD_TRACE	8192

/**
 * @brief Corrects the forward vector.
 * @param ent entity
 * @param fwd the forward vector
 * @param muzzlePoint the muzzle point
 * @param projsize projsize
 */
static void CorrectForwardVector(gentity_t* ent, vec3_t fwd, vec3_t muzzlePoint, float projsize) {
	trace_t		tr;
	vec3_t		end = { 0, 0, 0 };
	vec3_t		eyepoint = { 0, 0, 0 };
	vec3_t		mins = { 0, 0, 0 };
	vec3_t		maxs = { 0, 0, 0 };

	G_LogFuncBegin();

	G_Assert(ent, (void)0);

	/* Find the eyepoint. */
	VectorCopy(ent->client->ps.origin, eyepoint);
	eyepoint[2] += ent->client->ps.viewheight;

	memset(&tr, 0, sizeof(trace_t));

	/* First we must trace from the eyepoint to the muzzle point, to make sure that we have a legal muzzle point. */
	if (projsize > 0) {
		VectorSet(mins, -projsize, -projsize, -projsize);
		VectorSet(maxs, projsize, projsize, projsize);
		trap_Trace(&tr, eyepoint, mins, maxs, muzzlePoint, ent->s.number, MASK_SHOT);
	} else {
		trap_Trace(&tr, eyepoint, NULL, NULL, muzzlePoint, ent->s.number, MASK_SHOT);
	}

	if (tr.fraction < 1.0) {	/* We hit something here...  Stomp the muzzlePoint back to the eye... */
		VectorCopy(eyepoint, muzzlePoint);
		/* Keep the forward vector where it is, 'cause straight forward from the eyeball is right where we want to be. */
	} else {
		/* figure out what our crosshairs are on... */
		VectorMA(eyepoint, MAX_FORWARD_TRACE, forward, end);
		trap_Trace(&tr, eyepoint, NULL, NULL, end, ent->s.number, MASK_SHOT);

		/* ...and have our new forward vector point at it */
		VectorSubtract(tr.endpos, muzzlePoint, fwd);
		VectorNormalize(fwd);
	}

	G_LogFuncEnd();
}

/*
===============
G_Weapon_CalcMuzzlePoint

set muzzle location relative to pivoting eye
===============
*/

/**
 * @brief Table containing the muzzle points for all weapons.
 */
static vec3_t WP_MuzzlePoint[WP_NUM_WEAPONS] =
{/*	Fwd,	right,		up. 		*/
	{ 0, 0, 0 },	/* WP_0, */
	{ 29, 2, -4 },	/* WP_5, */
	{ 25, 7, -10 },	/* WP_6, */
	{ 25, 4, -5 },	/* WP_1, */
	{ 10, 14, -8 },	/* WP_4, */
	{ 25, 5, -8 },	/* WP_10, */
	{ 25, 5, -10 },	/* WP_8, */
	{ 0, 0, 0 },	/* WP_7, */ /*{22,	4.5,	-8	}, //TiM : Visual FX aren't necessary now, so just screw it */
	{ 5, 6, -6 },	/* WP_9, */
	{ 29, 2, -4 },	/* WP_13, */
	{ 29, 2, -4 },	/* WP_12, */
	{ 29, 2, -4 },	/* WP_14 */
	{ 27, 8, -10 },	/* WP_11 */
	{ 29, 2, -4 },	/* WP_2, */
	{ 29, 2, -4 },	/* WP_3, */
	{ 29, 2, -4 },	/* WP_15, */
	/*	{25,	7,			-10	},*/	/* WP_7 */
};


/**
 * @brief Table containing the size of each weapons projectiles.
 */
static double WP_ShotSize[WP_NUM_WEAPONS] =
{
	0,				/* WP_0, */
	0,				/* WP_5, */
	0,				/* WP_6, */
	0,				/* WP_1, */
	SCAV_SIZE,			/* WP_4, */
	STASIS_MAIN_MISSILE_BIG * 3,	/* WP_10, */
	GRENADE_SIZE,			/* WP_8, */
	6,				/* WP_7, */
	QUANTUM_SIZE,			/* WP_9, */
	0,				/* WP_13, */
	0,				/* WP_12, */
	0,				/* WP_14 */
	0,				/* WP_11 */
	0,				/* WP_2, */
	0,				/* WP_3, */
	0,				/* WP_15, */
	/*	0, */				/* WP_7 */
};

/**
 * @brief Table containing the size of each weapons alt projectiles.
 */
static double WP_ShotAltSize[WP_NUM_WEAPONS] =
{
	0,				/* WP_0, */
	PHASER_ALT_RADIUS,		/* WP_5, */
	0,				/* WP_6, */
	0,				/* WP_1, */
	SCAV_ALT_SIZE,			/* WP_4, */
	STASIS_MAIN_MISSILE_BIG * 3,	/* WP_10, */
	GRENADE_SIZE,			/* WP_8, */
	TETRION_ALT_SIZE,		/* WP_7, */
	QUANTUM_SIZE,			/* WP_9, */
	0,				/* WP_13, */
	0,				/* WP_12, */
	0,				/* WP_14 */
	0,				/* WP_11 */
	0,				/* WP_2 */
	0,				/* WP_3, */
	0,				/* WP_15, */
	/*	0,*/				/* WP_7 */
};

/**
 * @brief Calculate the muzzle point for weapons.
 * @param ent The player.
 * @param fwd Forward vector.
 * @param rt Right vector.
 * @param vup Up vector.
 * @param muzzlePoint The muzzle point-
 * @param projsize The projectile size.
 */
void G_Weapon_CalcMuzzlePoint(gentity_t* ent, vec3_t fwd, vec3_t rt, vec3_t vup, vec3_t muzzlePoint, double projsize) {
	int32_t weapontype;

	G_LogFuncBegin();

	G_Assert(ent, (void)0);

	weapontype = ent->s.weapon;
	VectorCopy(ent->s.pos.trBase, muzzlePoint);

#if 1
	if (weapontype > WP_0 && weapontype < WP_NUM_WEAPONS) {
		/* Use the table to generate the muzzlepoint; */
		{	/* Crouching.  Use the add-to-Z method to adjust vertically. */
			VectorMA(muzzlePoint, WP_MuzzlePoint[weapontype][0], fwd, muzzlePoint);
			VectorMA(muzzlePoint, WP_MuzzlePoint[weapontype][1], rt, muzzlePoint);
			if (ent->client->ps.eFlags & EF_FULL_ROTATE && Q_fabs(ent->client->ps.viewangles[PITCH] > 89.0f)) {
				muzzlePoint[2] -= 20 + WP_MuzzlePoint[weapontype][2];
			} else {
				muzzlePoint[2] += ent->client->ps.viewheight + WP_MuzzlePoint[weapontype][2];
				/* VectorMA(muzzlePoint, ent->client->ps.viewheight + WP_MuzzlePoint[weapontype][2], vup, muzzlePoint);*/
			}
		}
	}
#else	/* Test code */
	muzzlePoint[2] += ent->client->ps.viewheight;/* By eyes */
	muzzlePoint[2] += g_debugUp.value;
	VectorMA(muzzlePoint, g_debugForward.value, fwd, muzzlePoint);
	VectorMA(muzzlePoint, g_debugRight.value, rt, muzzlePoint);
#endif

	CorrectForwardVector(ent, fwd, muzzlePoint, projsize);
	SnapVector(muzzlePoint);

	G_LogFuncEnd();
}


RPGX_SiteTOSiteData TransDat[MAX_CLIENTS];

/**
 * @brief Handles firing of the Tricorder.
 * @param ent the player
 * @param alt_fire was this alt fire mode?
 */
static void WP_TricorderScan(gentity_t* ent, qboolean alt_fire) {
	gentity_t*	tr_ent = NULL;
	trace_t		tr;
	vec3_t		mins = { 0, 0, 0 };
	vec3_t		maxs = { 0, 0, 0 };
	vec3_t		end = { 0, 0, 0 };
	int32_t		clientNum = ent->client->ps.clientNum;

	G_LogFuncBegin();

	G_Assert(ent, (void)0);

	if (rpg_rangetricorder.integer < 32) {
		G_Logger(LL_DEBUG, "rpg_rangetricorder < 32\n");
		G_LogFuncEnd();
		return;
	}

	/* Fix - Changed || to && in the below if statement! */
	if (G_Client_IsAdmin(ent) == qfalse) {
		G_Logger(LL_DEBUG, "player not an admin\n");
		G_LogFuncEnd();
		return;
	}

	memset(&tr, 0, sizeof(trace_t));

	VectorMA(muzzle, rpg_rangetricorder.integer, forward, end);

	VectorSet(maxs, 6, 6, 6);
	VectorScale(maxs, -1, mins);

	/*
	 * TiM: I don't think performing a volume trace here is really needed.
	 * It is after all based on the player's current view.
	 * TiM: No, I was wrong! They're better coz it means errant n00bs or bots can't dodge them as easily!
	 */
	trap_Trace(&tr, muzzle, mins, maxs, end, ent->s.number, MASK_SHOT);
	/*trap_Trace ( &tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );*/
	tr_ent = &g_entities[tr.entityNum];

	/* BOOKMARK J2J */
	if (alt_fire) {
		/*
		 * RPG-X: J2J - New Transporter Tricorder Code (custom spawn points)
		 */
		/* if( TransDat[clientNum].Used == qfalse )*/
		if (VectorCompare(vec3_origin, TransDat[clientNum].storedCoord[TPT_TRICORDER].origin) &&
			VectorCompare(vec3_origin, TransDat[clientNum].storedCoord[TPT_TRICORDER].angles)) {
			/*VectorCopy(ent->client->ps.origin, TransDat[clientNum].TransCoord);*/
			/*VectorCopy(ent->client->ps.viewangles, TransDat[clientNum].TransCoordRot);*/
			VectorCopy(ent->client->ps.origin, TransDat[clientNum].storedCoord[TPT_TRICORDER].origin);
			VectorCopy(ent->client->ps.viewangles, TransDat[clientNum].storedCoord[TPT_TRICORDER].angles);
			/*TransDat[clientNum].Used = qtrue;*/
		}

		if (tr_ent && tr_ent->client && tr_ent->health > 0) {
			/*gentity_t	*tent;*/
			/*
			 * TiM: If we're already in a transport sequence, don't try another one.
			 * For starters, this screws up the visual FX, and secondly, I'm betting
			 * if u actually tried this, you'd atomically disperse the transportee in a very painful way O_o
			 */
			if (TransDat[tr_ent->client->ps.clientNum].beamTime > level.time) {
				trap_SendServerCommand(ent - g_entities, "chat \"Unable to comply. Subject is already within a transport cycle.\"");
				G_Logger(LL_DEBUG, "target already in transport cycle\n");
				G_LogFuncEnd();
				return;
			}

			trap_SendServerCommand(ent - g_entities, "chat \"Energizing.\"");

			G_InitTransport(tr_ent->client->ps.clientNum, TransDat[clientNum].storedCoord[TPT_TRICORDER].origin,
							TransDat[clientNum].storedCoord[TPT_TRICORDER].angles);				return;
		}
		/* If they clicked within 5 seconds ago */
		if ((level.time - TransDat[clientNum].LastClick) <= 5000) {
			VectorCopy(ent->client->ps.origin, TransDat[clientNum].storedCoord[TPT_TRICORDER].origin);
			VectorCopy(ent->client->ps.viewangles, TransDat[clientNum].storedCoord[TPT_TRICORDER].angles);

			/*VectorCopy(ent->client->ps.origin, TransDat[clientNum].TransCoord);*/
			/*VectorCopy(ent->client->ps.viewangles, TransDat[clientNum].TransCoordRot);*/
			TransDat[clientNum].LastClick = level.time - 5000;
			trap_SendServerCommand(ent - g_entities, "chat \"Location Confirmed.\"");
			/*trap_SendConsoleCommand( EXEC_APPEND, va("echo Location Confirmed.") );*/
		} else {
			trap_SendServerCommand(ent - g_entities, "chat \"Click again to confirm Transporter Location.\"");
			/*trap_SendConsoleCommand( EXEC_APPEND, va("echo Click again to confirm Transporter Location.") );*/
			TransDat[clientNum].LastClick = level.time;
		}

		G_LogFuncEnd();
	}
}

/**
 * @brief Handles firing of the hypospray.
 * @param ent the player
 * @param alt_fire was this alt fire mode?
 */
static void WP_SprayVoyagerHypo(gentity_t* ent, qboolean alt_fire) {
	gentity_t*		tr_ent = NULL;
	trace_t			tr;
	vec3_t			mins = { 0, 0, 0 };
	vec3_t			maxs = { 0, 0, 0 };
	vec3_t			end = { 0, 0, 0 };
	gentity_t*		t_ent = NULL;
	playerState_t *	tr_entPs = NULL;

	G_LogFuncBegin();

	G_Assert(ent, (void)0);

	if (rpg_rangehypo.integer < 8) /*32*/
	{
		G_Logger(LL_DEBUG, "rpg_rangehypo.integer < 8");
		G_LogFuncEnd();
		return;
	}

	VectorMA(muzzle, rpg_rangehypo.integer, forward, end);

	VectorSet(maxs, 6, 6, 6);
	VectorScale(maxs, -1, mins);

	memset(&tr, 0, sizeof(trace_t));
	trap_Trace(&tr, muzzle, mins, maxs, end, ent->s.number, MASK_OPAQUE | CONTENTS_BODY | CONTENTS_ITEM | CONTENTS_CORPSE); /*MASK_SHOT*/

	if (rpg_effectsgun.integer == 1 && G_Client_IsAdmin(ent) && alt_fire == qtrue && ent->s.weapon == WP_12) {
		if (RPGEntityCount != ENTITYNUM_MAX_NORMAL - 20) {
			t_ent = G_TempEntity(muzzle, EV_HYPO_PUFF);
			t_ent->s.eventParm = qfalse; /* TiM: Event parm is holding a qboolean value for color of spray */
			VectorCopy(forward, t_ent->s.angles2); /* TiM: Holds the directional vector.  This is passed to CG so it can be rendered right */

			G_LogFuncEnd();
			return;
		} else {
			G_LogPrintf("RPG-X WARNING: Max entities about to be hit! Restart the server ASAP or suffer a server crash!\n");
			trap_SendServerCommand(-1, va("print \"^1RPG-X WARNING: Max entities about to be hit! Restart the server ASAP or suffer a server crash!\n\""));
		}
	}

	tr_ent = &g_entities[tr.entityNum];
	G_Assert(tr_ent, (void)0);

	/* RPG-X: RedTechie - Medics can revive dead people */
	if ((tr_ent && tr_ent->client) && (tr_ent->health == 1) && (tr_ent->client->ps.pm_type == PM_DEAD)) {
		tr_entPs = &tr_ent->client->ps;
		if (rpg_medicsrevive.integer == 1) {
			G_Client_Spawn(tr_ent, 1, qtrue);

			/* TiM : Hard coded emote.  Makes the player play a 'get up' animation :) */
			/* G_MoveBox( tr_ent ); */
			tr_ent->r.contents = CONTENTS_NONE;
			tr_entPs->stats[LEGSANIM] = ((tr_entPs->stats[LEGSANIM] & ANIM_TOGGLEBIT) ^ ANIM_TOGGLEBIT) | BOTH_GET_UP1;
			tr_entPs->stats[TORSOANIM] = ((tr_entPs->stats[LEGSANIM] & ANIM_TOGGLEBIT) ^ ANIM_TOGGLEBIT) | BOTH_GET_UP1;
			tr_entPs->stats[EMOTES] |= EMOTE_BOTH | EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH;
			tr_entPs->stats[TORSOTIMER] = 1700;
			tr_entPs->stats[LEGSTIMER] = 1700;
			tr_entPs->legsAnim = 0;
			tr_entPs->torsoAnim = 0;
			tr_entPs->torsoTimer = 0;
			tr_entPs->legsTimer = 0;

			/*tr_entPs->stats[STAT_WEAPONS] = ( 1 << WP_0 );*/
			/*tr_entPs->stats[STAT_HOLDABLE_ITEM] = HI_NONE;*/
		}
		/* RPG-X: RedTechie - Regular functions still work */
	} else if (tr_ent && tr_ent->client && tr_ent->health > 0) {
		tr_entPs = &tr_ent->client->ps;
		if (alt_fire && rpg_hypoMelee.integer) { /* alt fire and hypo melee enabled */
			tr_ent->health = 0;
			G_Client_Die(tr_ent, ent, ent, 100, MOD_KNOCKOUT);
			G_LogWeaponFire(ent->s.number, WP_12);
		} else { /* else just heal */
			if (tr_ent->health < tr_entPs->stats[STAT_MAX_HEALTH]) {
				tr_ent->health = tr_entPs->stats[STAT_MAX_HEALTH];
			}
		}
	}
	/* TiM- else, use it on yourself */
	else {
		ent->health = ent->client->ps.stats[STAT_MAX_HEALTH];
	}

	G_LogFuncEnd();
}

/*
===============
FireWeapon
===============
*/

#define ACCURACY_TRACKING_DELAY		100 /* in ms */
#define NUM_FAST_WEAPONS			3

/**
 * @brief Fire the players weapons.
 * Does some basic setup and then calls specialized functions for the weapon currently used by the player-
 * @param ent The player.
 * @param alt_fire Whether alt fire is used.
 */
void FireWeapon(gentity_t* ent, qboolean alt_fire) {
	double projsize;

	G_LogFuncEnd();

	G_Assert(ent, (void)0);

	ent->client->pers.teamState.lastFireTime = level.time;

	/* set aiming directions */
	AngleVectors(ent->client->ps.viewangles, forward, right, up);

	if (alt_fire) {
		projsize = WP_ShotAltSize[ent->s.weapon];
	} else {
		projsize = WP_ShotSize[ent->s.weapon];
	}
	G_Weapon_CalcMuzzlePoint(ent, forward, right, up, muzzle, projsize);

#ifdef G_LUA
	LuaHook_G_FireWeapon(ent - g_entities, muzzle, forward, (int)alt_fire, ent->s.weapon);
#endif

	/* fire the specific weapon */
	switch (ent->s.weapon) {
		/* Player weapons */
		case WP_5:
			WP_FirePhaser(ent, alt_fire);
			break;
		case WP_6:
			WP_FireCompressionRifle(ent, alt_fire);
			break;
		case WP_1:
			if (G_Client_IsAdmin(ent) && alt_fire)
				WP_FireGrenade(ent, qfalse);
			break;
		case WP_4:
			break;
		case WP_10:
			WP_FireDisruptor(ent, alt_fire);
			break;
		case WP_8:
			WP_FireGrenade(ent, alt_fire);
			break;
		case WP_7:
			WP_FireTR116(ent, alt_fire);
			break;
		case WP_13:
			WP_SprayVoyagerHypo(ent, alt_fire);
			break;
		case WP_9:
			WP_FireQuantumBurst(ent, alt_fire);
			break;
		case WP_2:
			WP_TricorderScan(ent, alt_fire);
			break;
		case WP_3:
			break;
		case WP_15:
			WP_FireHyperspanner(ent, alt_fire);
			break;
		case WP_12:
			WP_SprayVoyagerHypo(ent, alt_fire);
			break;
		case WP_14:
			break;
		case WP_11:
			break;
		default:
			break;
	}

	G_LogFuncEnd();
}

