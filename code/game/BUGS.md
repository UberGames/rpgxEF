# Bug Tracker — rpgxEF game module

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
| bg_misc.cpp | ❌ |
| bg_panimate.cpp | ❌ |
| bg_pmove.cpp | ❌ |
| bg_slidemove.cpp | ❌ |
| fx_misc.cpp | ❌ |
| g_active.cpp | ❌ |
| g_arenas.cpp | ❌ |
| g_bot.cpp | ❌ |
| g_breakable.cpp | ❌ |
| g_cinematic.cpp | ❌ |
| g_client.cpp | ❌ |
| g_cmds.cpp | ❌ |
| g_combat.cpp | ❌ |
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