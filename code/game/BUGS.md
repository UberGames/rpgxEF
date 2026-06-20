# Bug Tracker — rpgxEF game module

## Workflow

- One bug fixed = one commit. Always commit immediately after fixing a bug, before moving to the next.

## Progress

| File | Status |
|------|--------|
| ai/Chat.cpp | ❌ |
| ai/Main.cpp | ❌ |
| ai_chat.cpp | ✅ |
| ai_cmd.cpp | ❌ |
| ai_dmnet.cpp | ❌ |
| ai_dmq3.cpp | ❌ |
| ai_main.cpp | ❌ |
| ai_team.cpp | ❌ |
| bg_misc.cpp | ✅ |
| bg_panimate.cpp | ❌ |
| bg_pmove.cpp | ❌ |
| bg_slidemove.cpp | ❌ |
| fx_misc.cpp | ❌ |
| g_active.cpp | ✅ |
| g_arenas.cpp | ❌ |
| g_bot.cpp | ❌ |
| g_breakable.cpp | ❌ |
| g_cinematic.cpp | ❌ |
| g_client.cpp | ❌ |
| g_cmds.cpp | ❌ |
| g_combat.cpp | ✅ |
| g_fx.cpp | ❌ |
| g_items.cpp | ❌ |
| g_log.cpp | ❌ |
| g_lua.cpp | ❌ |
| g_main.cpp | ❌ |
| g_mem.cpp | ❌ |
| g_misc.cpp | ❌ |
| g_missile.cpp | ❌ |
| g_mover.cpp | ❌ |
| g_session.cpp | ❌ |
| g_spawn.cpp | ❌ |
| g_sql.cpp | ❌ |
| g_svcmds.cpp | ❌ |
| g_syscalls.cpp | ❌ |
| g_target.cpp | ❌ |
| g_team.cpp | ❌ |
| g_trigger.cpp | ❌ |
| g_turrets.cpp | ❌ |
| g_ui.cpp | ❌ |
| g_usable.cpp | ❌ |
| g_utils.cpp | ❌ |
| g_weapon.cpp | ❌ |
| list.cpp | ❌ |
| lua_cinematic.cpp | ❌ |
| lua_cvar.cpp | ❌ |
| lua_entity.cpp | ❌ |
| lua_game.cpp | ❌ |
| lua_mover.cpp | ❌ |
| lua_qmath.cpp | ❌ |
| lua_sound.cpp | ❌ |
| lua_trace.cpp | ❌ |
| lua_vector.cpp | ❌ |
| lua_weapons.cpp | ❌ |
| md5.cpp | ❌ |
| q_math.cpp | ❌ |
| q_shared.cpp | ❌ |

---

## Bugs

### ai_chat.cpp

| ID | Status | Description |
|----|--------|-------------|
| B1 | ✅ | `BotChat_HitTalking` (L736): passes `lasthurt_client` (client index) to `BotWeaponNameForMeansOfDeath` instead of `lasthurt_mod` — always produces `"[unknown weapon]"` |
| B2 | ✅ | `BotChatTest` (L1073): same copy-paste error as B1 — `lasthurt_client` passed instead of `lasthurt_mod` |
| B3 | ✅ | `BotChat_HitNoKill` (L817): `bs->enemy` can be `-1`; `g_entities[bs->enemy].client` is accessed without bounds or null check |
| B4 | ✅ | All `BotChat_*` functions: `rnd = 0` makes `random() > rnd` true ~100% of the time, effectively disabling bot chat unless `bot_fastchat` is set |

### bg_misc.cpp

| ID | Status | Description |
|----|--------|-------------|
| B5 | ✅ | `EndWord` (L1843): inverted loop condition `!*pos` means the function always returns immediately without advancing — should be `*pos` |
| B6 | ✅ | `BG_EvaluateTrajectory` (L1184) and `BG_EvaluateTrajectoryDelta` (L1223): error messages print `tr->trTime` instead of `tr->trType` |
| B7 | ✅ | `bg_giveItem` (L944, L946): duplicate `"coffee"` entry wastes a slot and `bg_numGiveItems = 24` counts it twice |
| B8 | ✅ | `BG_ParseRankNames` (L1723): `sizeof(rankNames)` on a pointer parameter yields 8 bytes instead of the full array size — should be `sizeof(rankNames_t) * MAX_RANKS` |
| B9 | ✅ | `BG_ParseItemsText` (L1459): `while (buffer)` never exits at EOF since `COM_ParseExt` leaves `buffer` pointing at the null terminator, not NULL |
| B10 | ✅ | `BG_LanguageFilename` (L1490): `finalName` parameter declared `const char*` but written to via `Com_sprintf` — type mismatch and undefined behaviour |

### g_combat.cpp

| ID | Status | Description |
|----|--------|-------------|
| B11 | ✅ | `modNames` (L228): missing comma between `"MOD_SEEKER"` and `"MOD_KNOCKOUT"` causes silent string concatenation — all entries from `MOD_KNOCKOUT` onwards are shifted, breaking obituary logging |
| B12 | ✅ | `TossClientItems` (L148): `~i` used instead of `~(1 << i)` when clearing `STAT_WEAPONS` bitmask — weapons are never properly removed from player on death |
| B13 | ✅ | `G_Damage` (L1054): `inflictor->s.number` and `attacker->s.number` dereferenced in Lua hook before the null checks for `inflictor`/`attacker` (L1068-1073) |
| B14 | ✅ | `G_Damage` (L1082): `func_forcefield` pain callback passed `take` which is still `0` at that point — force field always receives 0 damage |
| B15 | ✅ | `G_Damage` (L1116): `client == NULL` check inside `if (client)` block is always false — dead code, likely masking a logic error |

### g_active.cpp

| ID | Status | Description |
|----|--------|-------------|
| B16 | ✅ | `AddPlayerToDetHits` (L702): missing `break` after filling an empty slot — loop runs to end, then recycling code always overwrites a second slot; recycling while-loop bound check is also wrong (`curHit == &detHits[0]` never triggers, risks out-of-bounds write) |
| B17 | ✅ | `flushDecoys` (L1306): `if (&g_entities[foundDecoys[removeMe]] == NULL)` — address of array element can never be NULL; should be `if (foundDecoys[removeMe] == ENTITYNUM_NONE)` |
| B18 | ✅ | `CreateShield` (L1098): x-axis branch sets `maxs[2] = height` but y-axis branch (and origin shift) uses `height >> 1` — x-axis shield is 1.5× taller than intended |
| B19 | ✅ | `ClientThink_real` (L1908): `memset(&currentCoord, 0, sizeof(currentCoord.origin))` only clears 12 bytes (the origin vec3) — `angles` field of the struct is not zeroed, leaving stale teleport angles |