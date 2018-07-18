#include "Model.h"

namespace game::entities
{
  Model::Model(GameSharedEntity* sharedEntity, Entity* parent): Entity{EntityType::ENT_MISC_MODEL, "misc_model", sharedEntity, parent}
  {
    free();
  }
}
