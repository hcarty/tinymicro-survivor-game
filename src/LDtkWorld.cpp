#include "LDtkWorld.h"

namespace
{
  void PlayerStart(const ldtk::EntityInstance &entity, const orxldtk::Source &source)
  {
    orxLOG("Creating config for %s", entity.identifier.data());
    orxldtk::entity::CreateDefaultConfig(entity, source, "PlayerStart");
  }

  void Treasure(const ldtk::EntityInstance &entity, const orxldtk::Source &source)
  {
    orxLOG("Creating config for %s", entity.identifier.data());
    auto section = orxldtk::entity::DefaultConfigSection(entity, source);
    orxldtk::entity::CreateDefaultConfig(entity, source, section);
    orxConfig_PushSection("Runtime");
    const orxCHAR *sectionPointer = section.data();
    orxConfig_AppendListString("TreasureList", &sectionPointer, 1);
    orxConfig_PopSection();
  }

  void MobSpawn(const ldtk::EntityInstance &entity, const orxldtk::Source &source)
  {
    orxLOG("Creating config for %s", entity.identifier.data());
    auto position = orxldtk::entity::WorldPosition(entity, source);
    auto size = orxldtk::entity::Size(entity);
    orxCHAR location[256];
    const orxCHAR *locationPtr = location;
    orxString_NPrint(location, sizeof(location), "(%g, %g) ~ (%g, %g)", position.fX, position.fY, position.fX + size.fX, position.fY + size.fY);
    orxConfig_PushSection("Mob");
    orxConfig_AppendListString("Position", &locationPtr, 1);
    orxConfig_PopSection();
  }
}

orxldtk::entity::Callbacks ldtkworld::GetCallbacks()
{
  return {
      {"Mob_spawn", MobSpawn},
      {"Player_start", PlayerStart},
      {"Treasure", Treasure},
  };
}
