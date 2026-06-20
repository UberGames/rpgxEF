# Bug Tracker — rpgxEF game module

## Workflow

- One bug fixed = one commit. Always commit immediately after fixing a bug, before moving to the next.
- BUGS.md updates (status, new entries) are always part of the bug-fix commit, never a separate commit.
- For each compilation unit: find ALL bugs first and add them to BUGS.md, then stop and wait for the user to direct which ones to fix.

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
| g_breakable.cpp | ✅ |
| g_cinematic.cpp | ❌ |
| g_client.cpp | ✅ |
| g_cmds.cpp | ❌ |
| g_combat.cpp | ✅ |
| g_fx.cpp | ❌ |
| g_items.cpp | ✅ |
| g_log.cpp | ❌ |
| g_lua.cpp | ❌ |
| g_main.cpp | ❌ |
| g_mem.cpp | ❌ |
| g_misc.cpp | ✅ |
| g_missile.cpp | ✅ |
| g_mover.cpp | ✅ |
| g_session.cpp | ✅ |
| g_spawn.cpp | ✅ |
| g_sql.cpp | ✅ |
| g_svcmds.cpp | ✅ |
| g_syscalls.cpp | ✅ |
| g_target.cpp | ✅ |
| g_team.cpp | ✅ |
| g_trigger.cpp | ✅ |
| g_turrets.cpp | ❌ |
| g_ui.cpp | ❌ |
| g_usable.cpp | ✅ |
| g_utils.cpp | ✅ |
| g_weapon.cpp | ✅ |
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

### g_items.cpp

| ID | Status | Description |
|----|--------|-------------|
| B20 | ✅ | `Padd_Remove` (L183): bounds check `i >= PADD_DATA_MAX` comes after `paddData[i].key` is read — when key is not found, `paddData[PADD_DATA_MAX]` (one past end) is accessed before the check triggers |
| B21 | ✅ | `Touch_Item` (L504): `numTotalDropped--` unconditionally decrements for every pickup, including world-spawned items that never incremented the counter — counter goes negative, corrupting the thrown-weapon limit |
| B22 | ✅ | `FinishSpawningItem` (L768): stray debug `Com_Printf("print \"giType %i!\n\"", ...)` spams the server console on every item spawn |

### g_weapon.cpp

| ID | Status | Description |
|----|--------|-------------|
| B23 | ✅ | `WP_FireDisruptor` (L562): primary-fire damage uses `PHASER_DAMAGE` — by design |
| B24 | ✅ | `WP_FireCompressionRifle` (L419): alt-fire damage uses `PHASER_DAMAGE` — by design |
| B25 | ✅ | `WP_FireGrenade` (L724): `&g_entities[foundTripWires[removeMe]] == NULL` — same dead pointer-to-array-element null check as B17; should be `foundTripWires[removeMe] == ENTITYNUM_NONE` |
| B26 | ✅ | `G_Weapon_CalcMuzzlePoint` (L1496): `Q_fabs(viewangles[PITCH] > 89.0f)` — closing paren misplaced; fabs wraps the boolean comparison instead of the angle, so looking straight up (negative pitch) is never caught |

### g_client.cpp

| ID | Status | Description |
|----|--------|-------------|
| B27 | ✅ | Duplicate `#define MAX_SPAWN_POINTS 256` — defined at L154 and again at L189 in the same file |
| B28 | ✅ | `G_Client_Respawn` (~L492): `borg` local variable is always `qfalse`, making the `EV_BORG_TELEPORT` branch permanently dead code |
| B29 | ✅ | `G_Client_UserinfoChanged` (~L923): `if (!ent) return;` where `ent = g_entities + clientNum` — array element address, never NULL; replaced with `ent->client->pers.connected == CON_DISCONNECTED` check |
| B30 | ✅ | `G_Client_UserinfoChanged` (~L1011): `if (qtrue)` — always true (even noted with a `WTF?` comment); `else` branch with `ForceClientSkin(model, "default")` is permanently unreachable |
| B31 | ✅ | `G_Client_Connect` (~L1660): `if (securityID <= 0 \|\| ...)` where `securityID` is `unsigned long` — `<= 0` is only true when exactly `0` since unsigned values can never be negative |
| B32 | ✅ | `G_Client_Begin` MOTD (~L1695): `buf[len] = '\0'` where `buf[16000]` and `len` can reach `16000` — writes one byte past the end of the buffer |
| B33 | ✅ | `G_Client_Spawn` (L1954-1958): `oClass = client->sess.sessionClass;` immediately followed by `if (oClass != client->sess.sessionClass)` — condition is always false; class-change notification never fires |
| B34 | ✅ | `G_Client_Disconnect` (L2197): `if (ent == NULL)` where `ent = &g_entities[clientNum]` — array element address, never NULL; replaced with `pers.connected == CON_DISCONNECTED` re-entry guard |
| B35 | ✅ | `G_Client_Disconnect` (L2290): `memset(ent, 0, sizeof(ent))` — `sizeof(ent)` is pointer size (8 bytes), not struct size; only the first 8 bytes of `gentity_t` are cleared on disconnect, leaving stale data in `r.contents`, `r.svFlags`, `takedamage`, etc. |

### g_misc.cpp

| ID | Status | Description |
|----|--------|-------------|
| B36 | ✅ | `SP_info_notnull` (L89): missing `return` after `G_FreeEntity(self)` — `G_FreeEntity` zeroes the slot and sets `classname = "freed"`, but execution continues: the second `strcmp` re-sets classname and `G_SetOrigin` is called on the freed entity |
| B37 | ✅ | `cycleCamera` (L401): if no entity matches `self->target`, both `G_Find` calls return NULL and `setCamera(self, owner->s.number)` dereferences NULL `owner` |

### g_utils.cpp

| ID | Status | Description |
|----|--------|-------------|
| B38 | ✅ | `BuildShaderStateConfig` (L65): `memset(buff, 0, MAX_STRING_CHARS)` only zeroes 1024 bytes of `buff[MAX_STRING_CHARS*4]` (4096 bytes) — should be `sizeof(buff)` |
| B39 | ✅ | `G_Spawn` (L560): `if (&g_entities[foundTripWires[i]] != NULL)` — dead null check on array element address, always true; same pattern as B17/B25 |
| B40 | ✅ | `G_AddEvent` (L712): `playerState_t *ps = &ent->client->ps` dereferences `ent->client` before the `if (ent->client)` guard on L720 — NULL client crashes at the pointer init |
| B41 | ✅ | `G_GetNearestEnt` (L1028) and `G_GetNearestPlayer` (L1086): `if (distance < 0) distance *= -1` — `VectorLength` always returns ≥ 0, dead code |
| B42 | ✅ | `G_GetEntityByTargetname` (L1109), `G_GetEntityByTarget` (L1128), `G_GetEntityByBmodel` (L1147): `if (!&g_entities[i]) continue` — dead null check on array element address; replaced with `inuse` check to skip freed slots |

### g_session.cpp

No bugs found.

### g_sql.cpp

| ID | Status | Description |
|----|--------|-------------|
| B48 | ✅ | Multiple `sqlite3_bind_text` calls (L152, L196, L223, L245, L249, L262, L312, L318, L527): pass `sizeof(uName)` or `sizeof(hashedpw)` as the length argument — these are pointers, so sizeof yields 8 (pointer size) instead of the string length; should be `-1` to let SQLite call strlen internally |
| B49 | ✅ | `G_Sql_UserDB_CheckRight` (L391): `if(right & right)` — a value ANDed with itself is always true for any non-zero input; should be `if(rights & right)` to test whether the fetched bitmask contains the requested right; as written the function always returns `qtrue` if any row exists |
| B50 | ✅ | `G_Sql_UserDB_RemoveRight` (L487): `rights &= right` should be `rights &= ~right` — current code clears all bits except those in `right` (the opposite of removing a right) |

### g_svcmds.cpp

| ID | Status | Description |
|----|--------|-------------|
| B51 | ✅ | `Svcmd_BanUser_f` (L407): `playerNum > MAX_CLIENTS` — off-by-one; allows `playerNum == MAX_CLIENTS` as valid, which is an out-of-range client slot; should be `>=` |
| B52 | ✅ | `G_ProcessIDBans` (L336, L341): reads file into `buffer[16000]` with no cap on `fileLen`, then writes `buffer[fileLen] = '\0'` — if the file is ≥ 16000 bytes the read overflows the buffer and the null terminator is written past the end |
| B53 | ✅ | `Svcmd_RemoveID_f` (L533): `id = &idFilters[atoi(str)]` with no bounds check — a negative or out-of-range index accesses outside `idFilters[MAX_IDFILTERS]`; the subsequent `if (!id)` is a dead null check on an array-element address that never catches this |

### g_syscalls.cpp

| ID | Status | Description |
|----|--------|-------------|
| B54 | ✅ | `trap_GetUserinfo` (L120): `buffer` parameter declared `const char *` but the syscall writes into it — same type mismatch as B10; callers pass a mutable buffer expecting it to be filled |

### g_target.cpp

| ID | Status | Description |
|----|--------|-------------|
| B55 | ✅ | `Use_Target_Give` (L23): `playerState_t *ps = &activator->client->ps` initialised before `if (!activator \|\| !activator->client)` guard at L25 — same crash pattern as B40/B44 |
| B56 | ✅ | `SP_target_give` (L81), `SP_target_delay` (L162), `SP_target_kill` (L574), `SP_target_relay` (L690), `SP_target_teleporter` (L830), `SP_target_location` (L874), `SP_target_counter` (L937), `SP_target_doorLock` (L1751), `SP_target_alert` (L2214), `SP_target_warp` (L2386), `SP_target_deactivate` (L2419), `SP_target_holodeck` (L2540): all use `&= SVF_NOCLIENT` — `&=` clears every bit except SVF_NOCLIENT instead of setting it; should be `\|=` |
| B57 | ✅ | `target_turbolift_endMove` (L993), `target_turbolift_TeleportPlayers` (L1154), `target_turbolift_startMove` (L1237), `target_turbolift_shutDoors` (L1380): `otherLift/targetLift = &g_entities[ent->count]; if (!otherLift)` — dead null check on array element address, always false |
| B58 | ✅ | `target_turbolift_startMove` (L1318): `if (lights->luaUse);` — trailing semicolon makes `LuaHook_G_EntityUse` on the next line unconditional, calling the hook even when `lights->luaUse` is NULL |
| B59 | ✅ | `target_alert_parseShaders` (L2090, L2103, L2125, L2147): `Q_strncpyz(buffer, src, strlen(src))` — `Q_strncpyz` treats its third arg as the buffer size and null-terminates at `size-1`, so passing `strlen(src)` drops the last character; should be `sizeof(buffer)` |
| B60 | ✅ | `target_warp_use` (L2274, L2277, L2294): `if (!&g_entities[i]) continue` and `target = &g_entities[i]; if (!target) continue` — dead null checks on array element addresses; should check `g_entities[i].inuse` to skip freed slots |
| B61 | ✅ | `target_serverchange_use` (L2454): `TransDat[ent->client->ps.clientNum]` — `ent` is the target_serverchange entity, not a player; `ent->client` is NULL; should be `activator->client->ps.clientNum` |
| B62 | ✅ | `target_shiphealth_use` (L3058, L3073, L3084, L3095): `alertEnt`/`transEnt`/`turboEnt`/`warpEnt` each assigned from `G_Find(...)` then immediately dereferenced without a null check — if no matching entity exists the dereference crashes |

### g_trigger.cpp

| ID | Status | Description |
|----|--------|-------------|
| B63 | ✅ | `Use_target_push` (L367): `(pm_type != PM_NORMAL) \|\| (pm_type != PM_NOCLIP)` — a value can never equal both simultaneously, so this is always true; the function always returns early and never applies the push velocity; should be `&&` |
| B64 | ✅ | `trigger_teleport_use` (L600-603): missing braces — `ent->flags ^= FL_LOCKED` is not inside the `if(!Q_stricmp(...))` block and runs unconditionally on every call, toggling the lock regardless of whether `swapname` matches |
| B65 | ✅ | `trigger_transporter_touch` (L977): `targetEnt = G_PickTarget(ent->target_ent->target)` result used without null check — if no target is found `VectorAdd(targetEnt->s.origin, ...)` on the next line crashes |

### g_spawn.cpp

| ID | Status | Description |
|----|--------|-------------|
| B43 | ✅ | `G_ParseField` (L562-566): `F_VECTOR4` case reads components 1–3 into `vec` (`vec3_t`) instead of `vec4`, and all four write-back assignments use index `[0]` — only the last value survives, indices 1–3 are never written; `startRGBA`/`finalRGBA` entity fields are always wrong |

### g_mover.cpp

| ID | Status | Description |
|----|--------|-------------|
| B44 | ✅ | `G_TryPushingEntity` (L67): `playerState_t *ps = &check->client->ps` declared unconditionally at function top, but `check->client` can be NULL for ET_ITEM entities (which the caller permits); `if (check->client)` guards appear later (L83, L95, L101, L112) — same crash pattern as B40 |
| B45 | ✅ | `G_MoverPush` (L77): `if (pushed_p > &pushed[MAX_GENTITIES])` — off-by-one: when `pushed_p == &pushed[MAX_GENTITIES]` the check is false, then L80 writes `pushed_p->ent` one slot past the array end; must be `>=` |
| B46 | ✅ | `InitMover` (L1039-1040): "angular movement" copy block uses `s.pos.trType` / `s.pos.trBase` instead of `s.apos.trType` / `s.apos.trBase` — overwrites the translational position base with `apos1` and leaves `s.apos.trBase` uninitialised for all entities using `InitMover` |
| B47 | ✅ | `Reached_AdvancedMover` (L3150): `G_Printf(S_COLOR_RED "DEBUG: ent->apos1: ...")` left in production code — spams the server console on every mover waypoint arrival, same pattern as B22 |

### g_usable.cpp

| ID | Status | Description |
|----|--------|-------------|
| B66 | ✅ | `func_usable_pain` (L158): `self->use(self, attacker, attacker)` called without null check — `ALWAYS_ON` entities set `self->use = NULL` at L99 after first trigger; a pain event before `func_usable_think` restores it crashes on the null pointer |
| B67 | ✅ | `func_usable_die` (L175): same null function pointer call on `self->use` — same root cause as B66 |
| B68 | ✅ | `G_SetupUsablesStrings` (L468): `buffer[file_len] = '0'` writes ASCII `'0'` (0x30) instead of null terminator `'\0'` (0x00) — buffer is never null-terminated, causing string operations to read past the file data |
| B69 | ✅ | `G_SetupUsablesStrings` (L460, L467): buffer allocated as 32000 bytes but `file_len` is never capped before `trap_FS_Read`; .usables files larger than 32000 bytes overflow the buffer |
| B70 | ✅ | `G_SetupUsablesStrings` (L538-540, L551): no bounds check on `j` against `MAX_ENTSCANNABLES` before writing `level.g_entScannables[j]`, and no bounds check on `i` against `MAX_SCANNABLES` before writing `level.g_scannables[i]` — malformed or oversized .usables files cause out-of-bounds writes |

### g_breakable.cpp

| ID | Status | Description |
|----|--------|-------------|
| B71 | ✅ | `breakable_die` (L86-89): `self->spawnflags` is read after `G_FreeEntity(self)` in the second `if` — the two conditions are mutually exclusive so no bad write occurs in practice, but reading a freed entity pointer is undefined behaviour; should be `else if` |
| B72 | ✅ | `ammo_use` (L616-623): `if (self->use != NULL)` inner check is always true when called via the use pointer — dead code that causes the `else` dispensing branch to be unreachable whenever a think is already scheduled, silently cancelling the cycle without restarting it |

### g_cmds.cpp

| ID | Status | Description |
|----|--------|-------------|
| B73 | ✅ | `Cmd_Give_f` (L306): loop decrements `i` from `bg_numGiveItems-1` to `-1`; the "not found" check `if (i >= bg_numGiveItems)` is always false after the loop — should be `if (i < 0)` |
| B74 | ✅ | `OnSameClass` (L1276-1279): `sess1 = &client1->sess; sess2 = &client2->sess` both dereference `client1`/`client2` before the null checks on those pointers at L1279 — same crash pattern as B40/B55 |
| B75 | ✅ | `G_Say` (L1510): `other = &g_entities[j]; if (!other)` — dead null check on array element address; `other` can never be NULL |
| B76 | ✅ | `Cmd_GameCommand_f` (L1655): `order > sizeof(gc_orders) / sizeof(char *)` — off-by-one; `sizeof(gc_orders)/sizeof(char*)` is 7, so `order == 7` passes the check and `gc_orders[7]` is an out-of-bounds read; should be `>=` |
| B77 | ✅ | `Cmd_Vote_f` (L1824): `if (msg[0] == 'y' \|\| msg[1] == 'Y' \|\| msg[1] == '1')` — second and third conditions check `msg[1]` instead of `msg[0]`; typing "Y" or "1" is incorrectly treated as "no" |
| B78 | ✅ | `Cmd_ForceVote_f` (L1880): same `msg[1]` typo as B77 — "Y" and "1" votes are misread |
| B79 | ✅ | `Cmd_ForceKill_f` (L2194): `if (targetNum >= level.maxclients) return` has no lower-bound guard beyond the explicit `== -1` check above it; a `targetNum` of e.g. `-2` passes through and `g_entities[targetNum]` is an out-of-bounds access |
| B80 | ✅ | `Cmd_ForceKill_f` (L2201): `return` inside the "kill all" loop (`targetNum == -1`) exits the entire function on the first entity that fails a check — all subsequent client slots are skipped; should be `continue` |
| B81 | ✅ | `Cmd_ForceKill_f` (L2222): `target = &g_entities[p]; if (target == NULL)` — dead null check on array element address; `target` is never NULL |
| B82 | ❌ | `Cmd_UnDrag_f` (L2662): `char arg[MAX_TOKEN_CHARS]; if (arg == NULL)` — dead null check on a stack array; the "stop all my drags" branch is permanently unreachable; should be `if (!arg[0])` |
| B83 | ❌ | `Cmd_disarm_f` (L2788): same dead null check on a stack array `arg` — "just mine" tripwire branch is permanently unreachable; should be `if (!arg[0])` |
| B84 | ❌ | `Cmd_disarm_f` (L2809): `if (&g_entities[foundTripWires[i]] == NULL)` — dead null check on array element address; same pattern as B17/B25/B39 |
| B85 | ❌ | `Cmd_Admins_f` (L3044): `g_classData[target->client->sess.sessionClass].isAdmin` accessed inside loop over all client slots without a null check on `target->client` — crashes for spectators or incompletely connected clients |
| B86 | ❌ | `Cmd_ForceRank_f` (L2969): `OldScore = other->client->ps.persistant[PERS_SCORE]` dereferences `other->client` before the null check on `other->client` at L2971 — same crash pattern as B40/B55 |
| B87 | ❌ | `Cmd_n00b_f` (L3330): `target = g_entities + targetNum` with no bounds check on `targetNum` before use; `target->client` then accessed at L3337 without a null check |
| B88 | ❌ | `Cmd_ForceModel_f` (L3451): `for (j = MAX_CLIENTS; j > -1; j--)` starts at `j = MAX_CLIENTS` — one past the last valid client slot; `g_entities[MAX_CLIENTS]` is out-of-range for the client array; should be `MAX_CLIENTS - 1` |
| B89 | ❌ | `Cmd_MeActionLocal_f` (L5181): `OtherPlayer->client->noAdminChat` accessed before the validity check on `OtherPlayer->client` at L5188 — crashes if `OtherPlayer` is a non-client entity |
| B90 | ❌ | `Cmd_fxGun_f` (~L4514): `memset(fxGunData, 0, sizeof(fxGunData))` where `fxGunData` is a pointer — `sizeof` yields pointer size (8 bytes) instead of the struct size; only the first 8 bytes of `fxGunData_t` are cleared; same pattern at L4516, L4519, L4540, L4559, L4601, L4623, L4648, L4660, L4680, L4703 |
| B91 | ❌ | `Cmd_reloadtorpedos_f` (L6008): `return` inside the `while` loop exits the entire function after the first torpedo entity is processed — all subsequent matching entities are skipped; should be `continue` |
| B92 | ❌ | `Cmd_alert_f` (L5623): `if (!te)` after calling `alertEnt->use(alertEnt, NULL, te)` — `te` is a local pointer and is never set to NULL by `G_FreeEntity`; dead null check |
| B93 | ❌ | `Cmd_getEntByTargetname_f` (L6274): `if (!&g_entities[i]) continue` — dead null check on array element address; same pattern as B17/B25/B39/B42 |
| B94 | ❌ | `Cmd_setOrigin` (L6393): `ent2 = &g_entities[i]; if (!ent2)` — dead null check on array element address |
| B95 | ❌ | `Cmd_UiTransporterLoc_f` (L7091): `trTrigger = &g_entities[entNum]; if (!trTrigger)` — dead null check on array element address |
| B96 | ❌ | `Cmd_UiTransporterExt_f` (L7170): same dead null check `trTrigger = &g_entities[entNum]; if (!trTrigger)` |
| B97 | ❌ | `Cmd_SqlLogin_f` (L7212, SQL build only): `if (!uName[0] && !pwd[0])` tests the buffers before `trap_Argv` fills them (at L7226-7227) — reads uninitialized stack memory; logout and usage-print branches never work correctly |
| B98 | ❌ | `Cmd_spawnTentity_f` (L6661): error path for `fx_spark` with too few args calls `G_FreeEntity(ent)` — frees the calling player entity instead of `newEnt`; all other error paths in the same function correctly free `newEnt` |
| B99 | ❌ | `Cmd_spawnTentity_f` (L6855): `target_doorlock` optional-arg block calls `trap_Argv(5, arg, sizeof(arg))` twice — once for `swapname` (lockMsg) and again for `truename` (unlockMsg); unlockMsg always receives the same value as lockMsg; second call should be `trap_Argv(6, ...)` |