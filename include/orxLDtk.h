#pragma once

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include "orx.h"
#ifdef __GNUC__
#pragma GCC diagnoastic pop
#endif

#include "LDtkLoader/Project.hpp"

namespace orxldtk
{
    struct Source
    {
        const ldtk::Project &project;
        const ldtk::World &world;
        const ldtk::Level &level;
        const std::vector<ldtk::Layer> &layers;
        const ldtk::Layer &collisions;
        const std::vector<std::string> collisionRules;
        const ldtk::Layer &entities;

        Source(const ldtk::Project &project,
               const std::string &levelName,
               const std::string &collisionLayerName,
               const std::vector<std::string> collisionRules,
               const std::string &entityLayerName);
    };

    namespace entity
    {
        std::string DefaultConfigSection(const ldtk::Entity &entity, const Source &source);
        void CreateDefaultConfig(const ldtk::Entity &entity, const Source &source, const std::string sectionName);

        using Callbacks = std::map<std::string, std::function<void(const ldtk::Entity &, const Source &)>>;
        static Callbacks callbacks;
    }

    void Init(const orxldtk::entity::Callbacks &callbacks);
    void Exit();
}

void orxLDtk_Init(const orxldtk::entity::Callbacks &callbacks);
void orxLDtk_Exit();
