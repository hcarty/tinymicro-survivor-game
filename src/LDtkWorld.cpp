#include "LDtkWorld.h"

namespace
{
  void PlayerStart(const ldtk::Entity &entity, const orxldtk::Source &source)
  {
    orxLOG("Creating config for %s", entity.getName().data());
    orxldtk::entity::CreateDefaultConfig(entity, source, "PlayerStart");
  }
}

orxldtk::entity::Callbacks ldtkworld::GetCallbacks()
{
  return {
      {"Player_start", PlayerStart}};
}
