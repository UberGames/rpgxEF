//
// Created by Walter on 19.06.2020.
//

#include "Chat.h"
#include "../ai_dmq3.h"
#include "../be_ai_chat.h"
#include "../be_ai_move.h"
#include "../inv.h"
#include "Main.h"

namespace ai::chat {

int numActivePlayers() {
  static const auto max_clients = trap_Cvar_VariableIntegerValue("sv_maxclients");

  auto num = 0;
  for (auto i = 0; i < max_clients && i < MAX_CLIENTS; i++) {
    auto config_string = trap_GetConfigstring(CS_PLAYERS + i, MAX_INFO_STRING);
    if (config_string.empty() || Info_ValueForKey(config_string, "n").empty()) { continue; }
    if (std::stoi(Info_ValueForKey(config_string, "t")) == TEAM_SPECTATOR) { continue; }
    num++;
  }
  return num;
}

bool isFirstInRankings(bot_state_t *bs) {
  int i;
  static int maxclis = trap_Cvar_VariableIntegerValue("sv_maxclients");

  for (i = 0; i < maxclis && i < MAX_CLIENTS; i++) {
    auto config_string = trap_GetConfigstring(CS_PLAYERS + i, MAX_INFO_STRING);
    if (config_string.empty() || Info_ValueForKey(config_string, "n").empty()) { continue; }
    if (std::stoi(Info_ValueForKey(config_string, "t")) == TEAM_SPECTATOR) { continue; }
    playerState_t ps;
    BotAI_GetClientState(i, &ps);
    if (bs->cur_ps.persistant[PERS_SCORE] < ps.persistant[PERS_SCORE]) { return false; }
  }
  return true;
}

bool isLastInRankings(bot_state_t *bs) {
  int i, score;
  static int maxclis = trap_Cvar_VariableIntegerValue("sv_maxclients");

  score = bs->cur_ps.persistant[PERS_SCORE];
  for (i = 0; i < maxclis && i < MAX_CLIENTS; i++) {
    auto config_string = trap_GetConfigstring(CS_PLAYERS + i, MAX_INFO_STRING);
    if (config_string.empty() || Info_ValueForKey(config_string, "n").empty()) { continue; }
    if (std::stoi(Info_ValueForKey(config_string, "t")) == TEAM_SPECTATOR) { continue; }
    playerState_t ps;
    BotAI_GetClientState(i, &ps);
    if (score > ps.persistant[PERS_SCORE]) { return false; }
  }
  return true;
}

std::string nameOfFirstClientInRankings() {
  static int maxclis = trap_Cvar_VariableIntegerValue("sv_maxclients");

  auto bestscore = -999999;
  auto bestclient = 0;
  for (auto i = 0; i < maxclis && i < MAX_CLIENTS; i++) {
    auto config_string = trap_GetConfigstring(CS_PLAYERS + i, MAX_INFO_STRING);
    if (config_string.empty() || Info_ValueForKey(config_string, "n").empty()) { continue; }
    if (std::stoi(Info_ValueForKey(config_string, "t")) == TEAM_SPECTATOR) { continue; }
    playerState_t ps;
    BotAI_GetClientState(i, &ps);
    if (ps.persistant[PERS_SCORE] > bestscore) {
      bestscore = ps.persistant[PERS_SCORE];
      bestclient = i;
    }
  }
  auto name = std::string{};
  name.reserve(32);
  EasyClientName(bestclient, name.data(), 32);
  return name;
}

std::string nameOfLastClientInRankings() {
  static int maxclis = trap_Cvar_VariableIntegerValue("sv_maxclients");

  auto worstscore = 999999;
  auto bestclient = 0;
  for (auto i = 0; i < maxclis && i < MAX_CLIENTS; i++) {
    auto confif_string = trap_GetConfigstring(CS_PLAYERS + i, MAX_INFO_STRING);
    if (confif_string.empty() || Info_ValueForKey(confif_string, "n").empty()) { continue; }
    if (std::stoi(Info_ValueForKey(confif_string, "t")) == TEAM_SPECTATOR) { continue; }
    playerState_t ps;
    BotAI_GetClientState(i, &ps);
    if (ps.persistant[PERS_SCORE] < worstscore) {
      worstscore = ps.persistant[PERS_SCORE];
      bestclient = i;
    }
  }
  auto name = std::string{};
  name.reserve(32);
  EasyClientName(bestclient, name.data(), 32);
  return name;
}

std::string randomOpponentName(bot_state_s *bs) {
  static int max_clients = trap_Cvar_VariableIntegerValue("sv_maxclients");

  auto number_of_opponents = 0;
  int opponents[MAX_INFO_STRING]{0};
  for (auto i = 0; i < max_clients && i < MAX_CLIENTS; i++) {
    if (i == bs->client) { continue; }
    auto config_string = trap_GetConfigstring(CS_PLAYERS + i, MAX_INFO_STRING);
    if (config_string.empty() || Info_ValueForKey(config_string, "n").empty()) { continue; }
    if (std::stoi(Info_ValueForKey(config_string, "t")) == TEAM_SPECTATOR) { continue; }
    if (BotSameTeam(bs, i)) { continue; }
    opponents[number_of_opponents] = i;
    number_of_opponents++;
  }
  auto name = std::string{};
  name.reserve(32);
  auto count = random() * number_of_opponents;
  for (auto i = 0; i < number_of_opponents; i++) {
    count--;
    if (count <= 0) {
      EasyClientName(opponents[i], name.data(), 32);
      return name;
    }
  }
  EasyClientName(opponents[0], name.data(), 32);
  return name;
}

std::string mapTitle() {
  char info[1024];
  char mapname[128];

  trap_GetServerinfo(info, sizeof(info));

  strncpy(mapname, Info_ValueForKey(info, "mapname"), sizeof(mapname) - 1);
  mapname[sizeof(mapname) - 1] = '\0';

  return mapname;
}

std::string weaponName(int means_of_death) {
  switch (means_of_death) {
    case MOD_PHASER:
    case MOD_PHASER_ALT: return "Phaser";
    case MOD_CRIFLE:
    case MOD_CRIFLE_SPLASH:
    case MOD_CRIFLE_ALT:
    case MOD_CRIFLE_ALT_SPLASH: return "Compression Rifle";
    case MOD_IMOD:
    case MOD_IMOD_ALT: return "Infinity Modulator";
    case MOD_SCAVENGER:
    case MOD_SCAVENGER_ALT:
    case MOD_SCAVENGER_ALT_SPLASH: return "Scavenger Rifle";
    case MOD_STASIS:
    case MOD_STASIS_ALT: return "Stasis Weapon";
    case MOD_GRENADE:
    case MOD_GRENADE_SPLASH:
    case MOD_GRENADE_ALT_SPLASH: return "Grenade Launcher";
    case MOD_TETRION:
    case MOD_TETRION_ALT: return "Tetryon Disruptor";
    case MOD_DREADNOUGHT:
    case MOD_DREADNOUGHT_ALT: return "Arc Welder";
    case MOD_QUANTUM:
    case MOD_QUANTUM_SPLASH:
    case MOD_QUANTUM_ALT:
    case MOD_QUANTUM_ALT_SPLASH: return "Photon Burst Cannon";
    case MOD_KNOCKOUT: return "Hypo";
    default: return "[unknown weapon]";
  }
}

std::string randomWeaponName() {
  const auto rnd = static_cast<int>(random() * 8.9);
  switch (rnd) {
    case 0: return "Phaser";
    case 1: return "Compression Rifle";
    case 2: return "Infinity Modulator";
    case 3: return "Scavenger Rifle";
    case 4: return "Stasis Weapon";
    case 5: return "Grenade Launcher";
    case 6: return "Tetryon Disruptor";
    case 7: return "Dermal Regenerator";
    default: return "Photon Burst Cannon";
  }
}

bool visibleEnemies(bot_state_s *bs) {
  float vis;
  aas_entityinfo_t entinfo;

  for (auto i = 0; i < MAX_CLIENTS; i++) {

    if (i == bs->client) {
      continue;
    }

    BotEntityInfo(i, &entinfo);

    if (!entinfo.valid) {
      continue;
    }

    //if the enemy isn't dead and the enemy isn't the bot self
    if (EntityIsDead(&entinfo) || entinfo.number == bs->entitynum) {
      continue;
    }

    //if the enemy is invisible and not shooting
    if (EntityIsInvisible(&entinfo) && !EntityIsShooting(&entinfo)) {
      continue;
    }

    //if on the same team
    if (BotSameTeam(bs, i)) continue;
    //check if the enemy is visible
    vis = BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, i);
    return vis > 0;
  }
  return false;
}

bool validChatPosition(bot_state_s *bs) {
  vec3_t point, start, end, mins, maxs;
  bsp_trace_t trace;

  //if the bot is dead all positions are valid
  if (BotIsDead(bs)) {
    return true;
  }

  //never start chatting with a powerup
  if (bs->inventory[INVENTORY_QUAD] || bs->inventory[INVENTORY_HASTE] || bs->inventory[INVENTORY_INVISIBILITY] || bs->inventory[INVENTORY_REGEN] || bs->inventory[INVENTORY_FLIGHT]) {
    return false;
  }

  //must be on the ground
  //if (bs->cur_ps.groundEntityNum != ENTITYNUM_NONE) return qfalse;
  //do not chat if in lava or slime
  VectorCopy(bs->origin, point);
  point[2] -= 24;
  if (trap_PointContents(point, bs->entitynum) & (CONTENTS_LAVA | CONTENTS_SLIME)) {
    return false;
  }

  //do not chat if under water
  VectorCopy(bs->origin, point);
  point[2] += 32;
  if (trap_PointContents(point, bs->entitynum) & MASK_WATER) {
    return false;
  }

  //must be standing on the world entity
  VectorCopy(bs->origin, start);
  VectorCopy(bs->origin, end);
  start[2] += 1;
  end[2] -= 10;
  trap_AAS_PresenceTypeBoundingBox(PRESENCE_CROUCH, mins, maxs);
  BotAI_Trace(&trace, start, mins, maxs, end, bs->client, MASK_SOLID);
  if (trace.ent != ENTITYNUM_WORLD) {
    return false;
  }

  //the bot is in a position where it can chat
  return true;
}

bool enterGame(bot_state_s *bs) {
  char name[32];
  float rnd;

  if (bot_nochat.integer) {
    return false;
  }

  if (bs->lastchat_time > trap_AAS_Time() - 3) {
    return false;
  }

  //don't chat in teamplay
  if (TeamPlayIsOn()) {
    return false;
  }

  rnd = 0;
  if (!bot_fastchat.integer) {
    if (random() > rnd) {
      return false;
    }
  }

  if (numActivePlayers() <= 1) {
    return false;
  }

  if (!validChatPosition(bs)) {
    return false;
  }

  main::BotInitialChat(bs, "game_enter",
                       EasyClientName(bs->client, name, 32),// 0
                       randomOpponentName(bs).c_str(),      // 1
                       "[invalid var]",                     // 2
                       "[invalid var]",                     // 3
                       mapTitle().c_str(),                  // 4
                       nullptr);
  bs->lastchat_time = trap_AAS_Time();
  bs->chatto = CHAT_ALL;
  return qtrue;
}

bool exitGame(bot_state_s *bs) {
  char name[32];
  float rnd;

  if (bot_nochat.integer) { return false; }
  if (bs->lastchat_time > trap_AAS_Time() - 3) { return false; }
  //don't chat in teamplay
  if (TeamPlayIsOn()) { return false; }
  rnd = 0;
  if (!bot_fastchat.integer) {
    if (random() > rnd) { return false; }
  }
  if (numActivePlayers() <= 1) { return false; }
  //
  main::BotInitialChat(bs, "game_exit",
                       EasyClientName(bs->client, name, 32),// 0
                       randomOpponentName(bs).c_str(),      // 1
                       "[invalid var]",                     // 2
                       "[invalid var]",                     // 3
                       mapTitle().c_str(),                  // 4
                       NULL);
  bs->lastchat_time = trap_AAS_Time();
  bs->chatto = CHAT_ALL;
  return true;
}

bool startLevel(bot_state_s *bs) {
  char name[32];
  float rnd;

  if (bot_nochat.integer) { return false; }
  if (BotIsObserver(bs)) { return false; }
  if (bs->lastchat_time > trap_AAS_Time() - 3) { return false; }
  //don't chat in teamplay
  if (TeamPlayIsOn()) { return false; }
  rnd = 0;
  if (!bot_fastchat.integer) {
    if (random() > rnd) { return false; }
  }
  if (numActivePlayers() <= 1) { return false; }
  BotAI_BotInitialChat(bs, "level_start",
                       EasyClientName(bs->client, name, 32),// 0
                       nullptr);
  bs->lastchat_time = trap_AAS_Time();
  bs->chatto = CHAT_ALL;
  return true;
}

}// namespace ai::chat