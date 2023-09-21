#include <algorithm>
#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "orxLDtk.h"

namespace orxldtk
{
    auto RESOURCE_GROUP = "Map";
    auto CONFIG_LDTK_NAME = "LDtk";
    auto CONFIG_COLLISION_LAYER = "CollisionLayer";
    auto CONFIG_COLLISION_RULES = "CollisionRules";
    auto CONFIG_ENTITY_LAYER = "EntityLayer";
    auto CONFIG_LEVELS_OBJECT = "LevelsObject";

    const orxVECTOR IntPointToVector(const ldtk::IntPoint &point)
    {
        return {static_cast<float>(point.x), static_cast<float>(point.y), 0.0f};
    }

    const orxVECTOR FloatPointToVector(const ldtk::FloatPoint &point)
    {
        return {point.x, point.y, 0.0f};
    }

    Source::Source(const ldtk::Project &project,
                   const std::string &levelName,
                   const std::string &collisionLayerName,
                   const std::vector<std::string> collisionRules,
                   const std::string &entityLayerName)
        : project(project),
          world(project.getWorld()),
          level(world.getLevel(levelName)),
          layers(level.allLayers()),
          collisions(level.getLayer(collisionLayerName)),
          collisionRules(collisionRules),
          entities(level.getLayer(entityLayerName))
    {
    }

    void SetTilesetTexture(const ldtk::Tileset &tileset, const Source &source)
    {
        orxConfig_PushSection(tileset.name.data());
        orxConfig_SetString(orxGRAPHIC_KZ_CONFIG_TEXTURE_NAME, tileset.path.data());
        orxVECTOR size = {static_cast<float>(tileset.tile_size),
                          static_cast<float>(tileset.tile_size), orxFLOAT_0};
        orxConfig_SetVector(orxGRAPHIC_KZ_CONFIG_TEXTURE_SIZE, &size);
        orxConfig_PopSection();
    }

    const std::string EntityGraphicSection(const ldtk::Entity &entity)
    {
        return entity.getName() + "Graphic";
    }

    void SetEntityGraphic(const ldtk::Entity &entity, const Source &source)
    {
        auto section = EntityGraphicSection(entity);
        orxConfig_PushSection(section.data());

        // Texture
        orxConfig_SetString(orxGRAPHIC_KZ_CONFIG_TEXTURE_NAME, entity.getTexturePath().data());
        const auto rect = entity.getTextureRect();
        const orxVECTOR origin = {static_cast<float>(rect.x),
                                  static_cast<float>(rect.y), 0.0f};
        const orxVECTOR size = {static_cast<float>(rect.width),
                                static_cast<float>(rect.height), 0.0f};
        orxConfig_SetVector(orxGRAPHIC_KZ_CONFIG_TEXTURE_ORIGIN, &origin);
        orxConfig_SetVector(orxGRAPHIC_KZ_CONFIG_TEXTURE_SIZE, &size);

        // Pivot
        auto pivot = FloatPointToVector(entity.getPivot());
        orxVector_Mul(&pivot, &pivot, &size);
        orxConfig_SetVector(orxGRAPHIC_KZ_CONFIG_PIVOT, &pivot);

        orxConfig_PopSection();
    }

    const std::string EntityBodyName(const ldtk::Entity &entity)
    {
        return entity.getName() + "Body";
    }

    void SetEntityStaticBody(const ldtk::Entity &entity)
    {
        // Body
        const auto bodyName = EntityBodyName(entity);
        orxConfig_PushSection(bodyName.data());
        orxConfig_SetBool("Dynamic", false);
        orxConfig_SetBool("FixedRotation", true);
        auto bodyPartName = std::string(bodyName) + std::string("Part");
        orxConfig_SetString("PartList", bodyPartName.data());
        orxConfig_PopSection();

        // Part
        orxConfig_PushSection(bodyPartName.data());
        orxConfig_SetString("Type", "box");
        orxConfig_SetBool("Solid", true);
        orxConfig_SetString("SelfFlags", "barrier");
        orxConfig_SetString("CheckMask", "dynamic");

        auto size = IntPointToVector(entity.getSize());
        // y pivot
        if (entity.getPivot().y == 1)
            size.fY = -size.fY;
        else if (entity.getPivot().y == 0.5)
            size.fY -= size.fY / 2;
        // x pivot
        if (entity.getPivot().x == 1)
            size.fX = -size.fX;
        else if (entity.getPivot().x == 0.5)
            size.fX -= size.fX / 2;
        orxConfig_SetVector("TopLeft", &orxVECTOR_0);
        orxConfig_SetVector("BottomRight", &size);

        orxConfig_PopSection();
    }

    void SetTileStaticBody(const std::string &bodyName)
    {
        orxConfig_PushSection(bodyName.data());
        orxConfig_SetBool("Dynamic", false);
        orxConfig_SetBool("FixedRotation", true);
        auto bodyPartName = std::string(bodyName) + std::string("Part");
        orxConfig_SetString("PartList", bodyPartName.data());
        orxConfig_PopSection();

        orxConfig_PushSection(bodyPartName.data());
        orxConfig_SetString("Type", "box");
        orxConfig_SetBool("Solid", true);
        orxConfig_SetString("SelfFlags", "barrier");
        orxConfig_SetString("CheckMask", "dynamic");
        orxConfig_PopSection();
    }

    const std::optional<std::string> TileFlip(const ldtk::Tile &tile)
    {
        std::optional<std::string> flip = std::nullopt;
        if (tile.flipX && tile.flipY)
            flip = "both";
        else if (tile.flipX)
            flip = "x";
        else if (tile.flipY)
            flip = "y";
        return flip;
    }

    const std::string TileGraphicSection(const ldtk::Tile &tile,
                                         const ldtk::Tileset &tileset)
    {
        const auto flip = TileFlip(tile);
        const auto suffix = flip ? ".Flip." + flip.value() : "";
        return tileset.name + "." + std::string("Tile.") +
               std::to_string(tile.tileId) + suffix;
    }

    const std::string TileGraphicSectionWithInherit(const ldtk::Tile &tile,
                                                    const ldtk::Tileset &tileset)
    {
        return TileGraphicSection(tile, tileset) + "@" + tileset.name;
    }

    const std::string TileObjectSection(const ldtk::Tile &tile,
                                        const ldtk::Tileset &tileset,
                                        const Source &source)
    {
        const auto gridPosition = tile.getGridPosition();
        return tileset.name + "." + source.level.name + "." +
               std::to_string(gridPosition.x) + "." + std::to_string(gridPosition.y);
    }

    void SetTileGraphic(const ldtk::Tile &tile, const ldtk::Tileset &tileset)
    {
        auto id = tile.tileId;
        orxConfig_PushSection(TileGraphicSectionWithInherit(tile, tileset).data());
        const auto origin = IntPointToVector(tileset.getTileTexturePos(id));
        orxConfig_SetVector(orxGRAPHIC_KZ_CONFIG_TEXTURE_ORIGIN, &origin);
        const auto flip = TileFlip(tile);
        if (flip)
            orxConfig_SetString("Flip", flip.value().data());
        orxConfig_PopSection();
    }

    const bool TileSolid(const ldtk::Tile &tile, const Source source)
    {
        auto gridPos = tile.getGridPosition();
        auto gridVal = source.collisions.getIntGridVal(gridPos.x, gridPos.y);
        return std::find(source.collisionRules.begin(), source.collisionRules.end(), gridVal.name) != source.collisionRules.end();
    }

    const std::string TileIntGridName(const ldtk::Tile &tile, const Source source)
    {
        auto gridPos = tile.getGridPosition();
        auto gridVal = source.collisions.getIntGridVal(gridPos.x, gridPos.y);
        return gridVal.name;
    }

    const orxVECTOR TilePosition(const ldtk::Tile &tile)
    {
        const auto position = tile.getWorldPosition();
        return IntPointToVector(position);
    }

    void SetTileObject(const ldtk::Tile &tile, const ldtk::Layer &layer,
                       const std::string &tileBodyName, const Source &source)
    {
        const auto graphicSection = TileGraphicSection(tile, layer.getTileset());
        const auto objectSection =
            TileObjectSection(tile, layer.getTileset(), source);
        const auto tilePosition = TilePosition(tile);
        orxConfig_PushSection(objectSection.data());
        orxConfig_SetString("Group", layer.getName().data());
        orxConfig_SetVector("Position", &tilePosition);
        orxConfig_SetString("Graphic", graphicSection.data());
        orxConfig_SetString("GridName", TileIntGridName(tile, source).data());
        if (TileSolid(tile, source))
            orxConfig_SetString("Body", tileBodyName.data());
        orxConfig_PopSection();
    }

    const std::string AllObjectSection(const std::string &levelName)
    {
        return levelName + ".AllObjects";
    }

    void AddLevelNeighbors(const ldtk::Level &level)
    {
        auto section = AllObjectSection(level.name);
        orxConfig_PushSection(section.data());
        const std::vector directions = {ldtk::Dir::North, ldtk::Dir::East,
                                        ldtk::Dir::South, ldtk::Dir::West};
        for (auto direction : directions)
        {
            try
            {
                for (auto neighbor : level.getNeighbours(direction))
                {
                    auto levelName = AllObjectSection(neighbor->name);
                    const char *c_levelName = levelName.data();
                    orxConfig_AppendListString("Neighbors", &c_levelName, 1);
                }
            }
            catch (std::out_of_range _e)
            {
                // No neighbors in this direction
            }
        }
        orxConfig_PopSection();
    }

    void AddToAllObjects(const std::string &levelName,
                         const std::string &objectSection)
    {
        auto allTilesSection = AllObjectSection(levelName);
        orxConfig_PushSection(allTilesSection.data());
        const orxSTRING c_section = objectSection.data();
        orxConfig_AppendListString("ChildList", &c_section, 1);
        orxConfig_PopSection();
    }

    void AddToAllLevels(const ldtk::Level &level)
    {
        auto levelSection = AllObjectSection(level.name);
        const orxSTRING c_levelSection = levelSection.data();
        orxConfig_PushSection(orxConfig_GetString(CONFIG_LEVELS_OBJECT));
        orxConfig_AppendListString("ChildList", &c_levelSection, 1);
        orxConfig_PopSection();
    }

    const std::string EntitySectionName(const ldtk::Entity &entity,
                                        const bool withInheritance)
    {
        auto name = entity.getName() + "." + entity.iid.str();
        auto inheritance = "@" + entity.getName();
        return withInheritance ? name + inheritance : name;
    }

    orxVECTOR EntityWorldPosition(const ldtk::Entity &entity,
                                  const Source &source)
    {
        auto position = IntPointToVector(entity.getPosition());
        auto levelPosition = IntPointToVector(source.level.position);
        orxVector_Add(&position, &position, &levelPosition);
        return position;
    }

    namespace entity
    {
        void AddConfigFields(const ldtk::Entity &entity)
        {
            for (auto &field : entity.allFields())
            {
                switch (field.type)
                {
                case ldtk::FieldType::String:
                {
                    orxConfig_SetString(
                        field.name.data(),
                        entity.getField<std::string>(field.name).value().data());
                    break;
                }
                case ldtk::FieldType::Bool:
                {
                    orxConfig_SetBool(field.name.data(),
                                      entity.getField<bool>(field.name).value());
                    break;
                }
                case ldtk::FieldType::Int:
                {
                    orxConfig_SetS64(field.name.data(),
                                     entity.getField<int>(field.name).value());
                    break;
                }
                case ldtk::FieldType::Point:
                {
                    orxVECTOR v = orxVECTOR_0;
                    auto point = entity.getField<ldtk::IntPoint>(field.name).value();
                    v.fX = static_cast<float>(point.x);
                    v.fY = static_cast<float>(point.y);
                    orxConfig_SetVector(field.name.data(), &v);
                    break;
                }
                default:
                {
                    orxLOG("Unhandled field %s", field.name.data());
                }
                };
            }
        }

        std::string DefaultConfigSection(const ldtk::Entity &entity, const Source &source)
        {
            auto inherit = std::string("@Object");
            try
            {
                inherit = "@" + entity.getField<std::string>("Inherit").value();
            }
            catch (std::invalid_argument &_e)
            {
            }
            auto section = EntitySectionName(entity, false);
            return section + inherit;
        }

        void CreateDefaultConfig(const ldtk::Entity &entity, const Source &source, const std::string sectionName)
        {
            orxConfig_PushSection((sectionName).data());

            // Set world position
            auto position = EntityWorldPosition(entity, source);
            orxConfig_SetVector("Position", &position);

            // Set graphic
            if (entity.getTexturePath() != "")
            {
                // Entity has an associated graphic
                auto graphicSection = EntityGraphicSection(entity);
                SetEntityGraphic(entity, source);
                orxConfig_SetString("Graphic", graphicSection.data());

                // Set physics body
                SetEntityStaticBody(entity);
                orxConfig_SetString("Body", EntityBodyName(entity).data());
            }
            else
            {
                // No graphic, so give the object a size and pivot directly
                // since they can't be inferred from a graphic
                auto size = IntPointToVector(entity.getSize());
                auto pivot = FloatPointToVector(entity.getPivot());
                orxVector_Mul(&pivot, &pivot, &size);
                orxConfig_SetVector("Size", &size);
                orxConfig_SetVector("Pivot", &pivot);
            }

            AddConfigFields(entity);

            orxConfig_PopSection();
        }

        void AddToEntityList(const ldtk::Entity &entity, const Source &source)
        {
            AddToAllObjects(source.level.name, EntitySectionName(entity, false));
        }
    }

    void CreateEntitySections(const Source &source)
    {
        const std::vector<ldtk::Entity> &entities = source.entities.allEntities();

        for (const auto &entity : entities)
        {
            if (entity::callbacks.contains(entity.getName()))
            {
                auto CreateConfig = entity::callbacks.at(entity.getName());
                CreateConfig(entity, source);
            }
            else
            {
                orxLOG("No specific support for entity %s, adding generic config", entity.getName().data());
                entity::CreateDefaultConfig(entity, source, entity::DefaultConfigSection(entity, source));
                entity::AddToEntityList(entity, source);
            }
        }
    }

    void ldtkToConfig(const orxSTRING mapLocation)
    {
        ldtk::Project project;
        {
            // Load map resource
            auto resourceLocation = orxResource_Locate("Map", mapLocation);
            orxASSERT(resourceLocation != orxNULL);
            auto handle = orxResource_Open(resourceLocation, orxFALSE);
            auto size = orxResource_GetSize(handle);
            std::vector<std::uint8_t> bytes(size);
            auto read = orxResource_Read(handle, size, bytes.data(), orxNULL, orxNULL);
            orxResource_Close(handle);
            orxASSERT(size == read);
            project.loadFromMemory(bytes);
        }
        const auto &world = project.getWorld();
        const auto &levels = world.allLevels();
        for (const auto &level : levels)
        {
            AddToAllLevels(level);
            AddLevelNeighbors(level);

            const auto levelName = level.name.data();
            const auto collisionLayerName = orxConfig_GetString(CONFIG_COLLISION_LAYER);
            const auto collisionRuleCount = orxConfig_GetListCount(CONFIG_COLLISION_RULES);
            std::vector<std::string> collisionRules(collisionRuleCount);
            for (auto i = 0; i < collisionRuleCount; i++)
            {
                collisionRules[i] = std::string{orxConfig_GetListString(CONFIG_COLLISION_RULES, i)};
            }
            const auto entityLayerName = orxConfig_GetString(CONFIG_ENTITY_LAYER);
            const auto source = Source(project, levelName, collisionLayerName, collisionRules, entityLayerName);

            // Define entities in the game world
            CreateEntitySections(source);

            // Define physics collision body for tiles
            const auto tileBodyName = std::string("TileCollisionBody");
            SetTileStaticBody(tileBodyName);

            for (const auto &layer : source.layers)
            {
                orxLOG("Processing layer %s", layer.getName().data());

                // Generate config for each tile
                for (const auto &tile : layer.allTiles())
                {
                    // Define texture source for tiles
                    SetTilesetTexture(layer.getTileset(), source);

                    // Generate graphics config in case it does not exist yet
                    const auto graphicSection =
                        TileGraphicSection(tile, layer.getTileset());
                    SetTileGraphic(tile, layer.getTileset());

                    // Generate object config for this tile
                    const auto objectSection =
                        TileObjectSection(tile, layer.getTileset(), source);
                    SetTileObject(tile, layer, tileBodyName, source);

                    // Push all the tiles as children for a single object
                    AddToAllObjects(levelName, objectSection);
                }
            }
        }
    }

    orxSTATUS EventHandler(const orxEVENT *_pstEvent)
    {
        orxSTATUS eResult = orxSTATUS_SUCCESS;

        // Object?
        if (_pstEvent->eType == orxEVENT_TYPE_OBJECT)
        {
            // New object?
            if (_pstEvent->eID == orxOBJECT_EVENT_PREPARE)
            {
                // Is a LDtk object?
                if (orxConfig_HasValue(CONFIG_LDTK_NAME) &&
                    orxConfig_HasValue(CONFIG_COLLISION_LAYER) &&
                    orxConfig_HasValue(CONFIG_COLLISION_RULES) &&
                    orxConfig_HasValue(CONFIG_ENTITY_LAYER))
                {
                    ldtkToConfig(orxConfig_GetString(CONFIG_LDTK_NAME));
                }
            }
        }

        // Done!
        return eResult;
    }

    void Init(const entity::Callbacks &callbacks)
    {
        // Record entity-specific callbacks
        entity::callbacks = callbacks;

        // Registers LDtk event handler
        orxEvent_AddHandler(orxEVENT_TYPE_OBJECT, EventHandler);
        orxEvent_SetHandlerIDFlags(EventHandler, orxEVENT_TYPE_OBJECT, orxNULL, orxEVENT_GET_FLAG(orxOBJECT_EVENT_PREPARE), orxEVENT_KU32_MASK_ID_ALL);

        // Done!
        return;
    }

    void Exit()
    {
        // Clear all callbacks
        entity::callbacks.clear();

        // Unregister LDtk event handler
        orxEvent_RemoveHandler(orxEVENT_TYPE_OBJECT, EventHandler);

        // Done!
        return;
    }
}

void orxFASTCALL orxLDtk_Init(const orxldtk::entity::Callbacks &callbacks)
{
    return orxldtk::Init(callbacks);
}

void orxFASTCALL orxLDtk_Exit()
{
    return orxldtk::Exit();
}
