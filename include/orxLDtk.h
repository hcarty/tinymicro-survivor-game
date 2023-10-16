#pragma once

#include "orx.h"

#include "LDtk/LDtk.h"

namespace orxldtk
{
  struct Source
  {
    const ldtk::Project &project;
    const ldtk::Level &level;
    const std::vector<ldtk::LayerInstance> &layers;
    const ldtk::LayerInstance &collisions;
    const ldtk::LayerInstance &entities;

    Source(const ldtk::Project &project,
           const std::string &levelName, const std::string &collisionLayerName,
           const std::string &entityLayerName);
  };

  namespace entity
  {
    orxVECTOR WorldPosition(const ldtk::EntityInstance &entity, const Source &source);
    orxVECTOR Size(const ldtk::EntityInstance &entity);

    std::string DefaultConfigSection(const ldtk::EntityInstance &entity, const Source &source);
    void CreateDefaultConfig(const ldtk::EntityInstance &entity, const Source &source, const std::string sectionName);

    using Callbacks = std::map<std::string, std::function<void(const ldtk::EntityInstance &, const Source &)>>;
    static Callbacks callbacks;
  }

  void Init(const orxldtk::entity::Callbacks &callbacks);
  void Exit();
}

void orxLDtk_Init(const orxldtk::entity::Callbacks &callbacks);
void orxLDtk_Exit();
