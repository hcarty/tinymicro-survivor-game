#include "LDtkWorld.h"

namespace
{
  void PlayerStart(const ldtk::Entity &entity, const orxldtk::Source &source)
  {
    orxLOG("Creating config for %s", entity.getName().data());
    orxldtk::entity::CreateDefaultConfig(entity, source, "PlayerStart");
  }

  void Treasure(const ldtk::Entity &entity, const orxldtk::Source &source)
  {
    orxLOG("Creating config for %s", entity.getName().data());
    auto section = orxldtk::entity::DefaultConfigSection(entity, source);
    orxldtk::entity::CreateDefaultConfig(entity, source, section);
    orxConfig_PushSection("Runtime");
    const orxCHAR *sectionPointer = section.data();
    orxConfig_AppendListString("TreasureList", &sectionPointer, 1);
    orxConfig_PopSection();
  }

  void MobSpawn(const ldtk::Entity &entity, const orxldtk::Source &source)
  {
    orxLOG("Creating config for %s", entity.getName().data());
    auto position = entity.getPosition();
    auto size = entity.getSize();
    orxCHAR location[256];
    const orxCHAR *locationPtr = location;
    orxString_NPrint(location, sizeof(location), "(%d, %d) ~ (%d, %d)", position.x, position.y, position.x + size.x, position.y + size.y);
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
