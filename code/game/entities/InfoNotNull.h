#pragma once
#include "../Entity.h"

namespace game::entities
{
  class InfoNotNull : public Entity
  {
  public:
    InfoNotNull(GameSharedEntity* sharedEntity, Entity* parent = nullptr);

    void think() override {}
    void touch(const Entity&, trace_t*) override {}
    void use(const Entity&, const Entity&) override {}
    void pain(const Entity&, int32_t) override {}
    void die(const Entity&, const Entity&, int, int) override {}
  };
}
