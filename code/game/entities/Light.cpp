#include "Light.h"

namespace game::entities
{
  Light::Light(GameSharedEntity* sharedEntity, Entity* parent) : Entity{EntityType::ENT_LIGHT, "light", sharedEntity, parent}
  {
    free();
  }
}
