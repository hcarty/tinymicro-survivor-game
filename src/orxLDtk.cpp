#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "orx.h"

#include "orxLDtk.h"

namespace
{
  // Convenience types and functions for for wrangling LDtk definitions

  /// Coordinates encoded as JSON arrays and exposed as std::vector

  // 2D integer (pixel or grid) vector
  using intvec = std::vector<int64_t>;

  // 2D double vector
  using doublevec = std::vector<double>;

  template <typename T>
  auto get(const T vec, size_t i) -> orxFLOAT
  {
    return static_cast<orxFLOAT>(vec[i]);
  }

  template <typename T>
  auto getX(const T &vec) -> orxFLOAT
  {
    return get(vec, 0);
  }

  template <typename T>
  auto getY(const T &vec) -> orxFLOAT
  {
    return get(vec, 1);
  }

  template <typename T>
  auto OrxVector(const T &vec) -> orxVECTOR
  {
    return {getX(vec), getY(vec), 0.0};
  }

  template <typename T>
  orxVECTOR OrxVector(T x, T y)
  {
    return {static_cast<orxFLOAT>(x), static_cast<orxFLOAT>(y), 0.0};
  }

  const ldtk::TilesetDefinition &GetTilesetByName(const ldtk::Project &project, const std::string &name)
  {
    for (const auto &tileset : project.defs.tilesets)
    {
      if (tileset.identifier == name)
      {
        return tileset;
      }
    }
    orxASSERT(orxFALSE, "No LDtk tileset named %s in project %s", name.data(), project.iid.data());
    assert(false);
  }

  const ldtk::TilesetDefinition &GetTilesetByID(const ldtk::Project &project, int64_t id)
  {
    for (const auto &tileset : project.defs.tilesets)
    {
      if (tileset.uid == id)
      {
        return tileset;
      }
    }
    orxASSERT(orxFALSE, "No LDtk tileset ID %d in project %s", id, project.iid.data());
    assert(false);
  }

  const std::string &GetTilesetTexturePath(const ldtk::TilesetDefinition &tileset)
  {
    orxASSERT(tileset.relPath, "No texture associated with tileset %s", tileset.identifier.data());
    return *tileset.relPath;
  }

  const ldtk::Level &GetLevelByName(const ldtk::Project &project, const std::string &name)
  {
    for (const auto &level : project.levels)
    {
      if (level.identifier == name)
      {
        return level;
      }
    }
    orxASSERT(orxFALSE, "No LDtk level named %s in project %s", name.data(), project.iid.data());
    assert(false);
  }

  const ldtk::Level &GetLevelByID(const ldtk::Project &project, const std::string &id)
  {
    for (const auto &level : project.levels)
    {
      if (level.iid == id)
      {
        return level;
      }
    }
    orxASSERT(orxFALSE, "No LDtk level with ID %s in project %s", id.data(), project.iid.data());
    assert(false);
  }

  const ldtk::LayerInstance &GetLayerByName(const ldtk::Level &level, const std::string &name)
  {
    if (level.layerInstances)
    {
      for (const auto &layer : *level.layerInstances)
      {
        if (layer.identifier == name)
        {
          return layer;
        }
      }
    }
    orxASSERT(orxFALSE, "No LDtk layer named %s in level %s", name.data(), level.identifier.data());
    assert(false);
  }

  const orxVECTOR LayerOffset(const ldtk::LayerInstance &layer)
  {
    return {static_cast<orxFLOAT>(layer.pxTotalOffsetX), static_cast<orxFLOAT>(layer.pxTotalOffsetY), 0.0};
  }

  bool TileFlipBitTest(const ldtk::TileInstance &tile, size_t index)
  {
    return std::bitset<std::numeric_limits<int64_t>::digits>(tile.f)[index];
  }

  bool TileFlipX(const ldtk::TileInstance &tile)
  {
    return TileFlipBitTest(tile, 0);
  }

  bool TileFlipY(const ldtk::TileInstance &tile)
  {
    return TileFlipBitTest(tile, 1);
  }

  std::tuple<int64_t, int64_t> TileGridPosition(const ldtk::TileInstance &tile, const ldtk::TilesetDefinition &tileset, const ldtk::LayerInstance &layer)
  {
    const int64_t column = (tile.px[0] - layer.pxTotalOffsetX) / tileset.tileGridSize;
    const int64_t row = (tile.px[1] - layer.pxTotalOffsetY) / tileset.tileGridSize;
    return {column, row};
  }

  int64_t GetIntGridVal(const ldtk::LayerInstance &layer, const int64_t column, const int64_t row)
  {
    size_t index = column + layer.cWid * row;
    return layer.intGridCSV[index];
  }

  const std::optional<const ldtk::FieldInstance> GetFieldByName(const ldtk::EntityInstance &entity, const std::string &name)
  {
    for (const auto &field : entity.fieldInstances)
    {
      if (field.identifier == name)
      {
        return field;
      }
    }
    return std::nullopt;
  }
}

namespace orxldtk
{
  auto RESOURCE_GROUP = "Map";
  auto CONFIG_LDTK_NAME = "LDtk";
  auto CONFIG_COLLISION_LAYER = "CollisionLayer";
  auto CONFIG_ENTITY_LAYER = "EntityLayer";
  auto CONFIG_LEVELS_OBJECT = "LevelsObject";

  const orxVECTOR IntPointToVector(const intvec &point)
  {
    return {getX(point), getY(point), 0.0f};
  }

  const orxVECTOR FloatPointToVector(const doublevec &point)
  {
    return {getX(point), getY(point), 0.0f};
  }

  Source::Source(const ldtk::Project &project,
                 const std::string &levelName, const std::string &collisionLayerName,
                 const std::string &entityLayerName)
      : project(project),
        level(GetLevelByName(project, levelName)),
        layers(level.layerInstances.value()),
        collisions(GetLayerByName(level, collisionLayerName)),
        entities(GetLayerByName(level, entityLayerName))
  {
  }

  void SetTilesetTexture(const ldtk::TilesetDefinition &tileset, const Source &source)
  {
    orxConfig_PushSection(tileset.identifier.data());
    orxConfig_SetString(orxGRAPHIC_KZ_CONFIG_TEXTURE_NAME, GetTilesetTexturePath(tileset).data());
    orxVECTOR size = {static_cast<float>(tileset.tileGridSize),
                      static_cast<float>(tileset.tileGridSize), orxFLOAT_0};
    orxConfig_SetVector(orxGRAPHIC_KZ_CONFIG_TEXTURE_SIZE, &size);
    orxConfig_PopSection();
  }

  const std::string EntityGraphicSection(const ldtk::EntityInstance &entity)
  {
    return entity.identifier + "Graphic";
  }

  void SetEntityGraphic(const ldtk::EntityInstance &entity, const Source &source)
  {
    // We can only proceed if there's a tile associated with this entity
    if (!entity.tile)
    {
      return;
    }

    auto tile = *entity.tile;
    auto tileset = GetTilesetByID(source.project, tile.tilesetUid);

    // We can only proceed if there's a texture associated with this tileset
    if (!tileset.relPath)
    {
      return;
    }

    auto section = EntityGraphicSection(entity);
    orxConfig_PushSection(section.data());

    // Texture
    orxConfig_SetString(orxGRAPHIC_KZ_CONFIG_TEXTURE_NAME, GetTilesetTexturePath(tileset).data());
    const orxVECTOR origin = {static_cast<float>(tile.x),
                              static_cast<float>(tile.y), 0.0f};
    const orxVECTOR size = {static_cast<float>(tile.w),
                            static_cast<float>(tile.h), 0.0f};
    orxConfig_SetVector(orxGRAPHIC_KZ_CONFIG_TEXTURE_ORIGIN, &origin);
    orxConfig_SetVector(orxGRAPHIC_KZ_CONFIG_TEXTURE_SIZE, &size);

    // Pivot
    auto pivot = OrxVector(entity.pivot);
    orxVector_Mul(&pivot, &pivot, &size);
    orxConfig_SetVector(orxGRAPHIC_KZ_CONFIG_PIVOT, &pivot);

    orxConfig_PopSection();
  }

  const std::string EntityBodyName(const ldtk::EntityInstance &entity)
  {
    return entity.identifier + "Body";
  }

  void SetEntityStaticBody(const ldtk::EntityInstance &entity)
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

    orxVECTOR size = {static_cast<orxFLOAT>(entity.width), static_cast<orxFLOAT>(entity.height), 0.0};
    auto pivot = OrxVector(entity.pivot);
    // y pivot
    if (pivot.fY == 1)
      size.fY = -size.fY;
    else if (pivot.fY == 0.5)
      size.fY -= size.fY / 2;
    // x pivot
    if (pivot.fX == 1)
      size.fX = -size.fX;
    else if (pivot.fX == 0.5)
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

  const std::optional<std::string> TileFlip(const ldtk::TileInstance &tile)
  {
    std::optional<std::string> flip = std::nullopt;
    if (TileFlipX(tile) && TileFlipY(tile))
      flip = "both";
    else if (TileFlipX(tile))
      flip = "x";
    else if (TileFlipY(tile))
      flip = "y";
    return flip;
  }

  const std::string TileGraphicSection(const ldtk::TileInstance &tile,
                                       const ldtk::TilesetDefinition &tileset)
  {
    const auto flip = TileFlip(tile);
    const auto suffix = flip ? ".Flip." + flip.value() : "";
    return tileset.identifier + "." + std::string("Tile.") +
           std::to_string(tile.t) + suffix;
  }

  const std::string TileGraphicSectionWithInherit(const ldtk::TileInstance &tile,
                                                  const ldtk::TilesetDefinition &tileset)
  {
    return TileGraphicSection(tile, tileset) + "@" + tileset.identifier;
  }

  const std::string TileObjectSection(const ldtk::TileInstance &tile,
                                      const ldtk::TilesetDefinition &tileset,
                                      const Source &source)
  {
    return tileset.identifier + "." +
           source.level.identifier + "." +
           std::to_string(tile.px[0]) + "." +
           std::to_string(tile.px[1]);
  }

  void SetTileGraphic(const ldtk::TileInstance &tile, const ldtk::TilesetDefinition &tileset)
  {
    orxConfig_PushSection(TileGraphicSectionWithInherit(tile, tileset).data());
    const auto origin = OrxVector(tile.src);
    orxConfig_SetVector(orxGRAPHIC_KZ_CONFIG_TEXTURE_ORIGIN, &origin);
    const auto flip = TileFlip(tile);
    if (flip)
      orxConfig_SetString("Flip", flip.value().data());
    orxConfig_PopSection();
  }

  const bool TileSolid(const ldtk::TileInstance &tile, const ldtk::TilesetDefinition &tileset, Source source)
  {
    auto [column, row] = TileGridPosition(tile, tileset, source.collisions);
    auto gridVal = GetIntGridVal(source.collisions, column, row);
    return gridVal > 1;
  }

  const orxVECTOR TilePosition(const ldtk::TileInstance &tile, const ldtk::LayerInstance &layer)
  {
    auto position = OrxVector(tile.px);
    auto offset = LayerOffset(layer);
    orxVector_Add(&position, &position, &offset);
    return position;
  }

  void SetTileObject(const ldtk::TileInstance &tile, const ldtk::LayerInstance &layer,
                     const std::string &tileBodyName, const Source &source)
  {
    orxASSERT(layer.tilesetDefUid, "No tileset for layer %s", layer.identifier.data());
    auto tileset = GetTilesetByID(source.project, *layer.tilesetDefUid);
    const auto graphicSection = TileGraphicSection(tile, tileset);
    const auto objectSection =
        TileObjectSection(tile, tileset, source);
    const auto tilePosition = TilePosition(tile, layer);
    orxConfig_PushSection(objectSection.data());
    orxConfig_SetString("Group", layer.identifier.data());
    orxConfig_SetVector("Position", &tilePosition);
    orxConfig_SetString("Graphic", graphicSection.data());
    if (TileSolid(tile, tileset, source))
      orxConfig_SetString("Body", tileBodyName.data());
    orxConfig_PopSection();
  }

  const std::string AllObjectSection(const std::string &levelName)
  {
    return levelName + ".AllObjects";
  }

  void AddLevelNeighbors(const ldtk::Project &project, const ldtk::Level &level)
  {
    auto section = AllObjectSection(level.identifier);
    orxConfig_PushSection(section.data());
    for (auto neighbor : level.neighbours)
    {
      auto neighborLevel = GetLevelByID(project, neighbor.levelIid);
      auto levelName = AllObjectSection(neighborLevel.identifier);
      const char *c_levelName = levelName.data();
      orxConfig_AppendListString("Neighbors", &c_levelName, 1);
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
    auto levelSection = AllObjectSection(level.identifier);
    const orxSTRING c_levelSection = levelSection.data();
    orxConfig_PushSection(orxConfig_GetString(CONFIG_LEVELS_OBJECT));
    orxConfig_AppendListString("ChildList", &c_levelSection, 1);
    orxConfig_PopSection();
  }

  const std::string EntitySectionName(const ldtk::EntityInstance &entity,
                                      const bool withInheritance)
  {
    auto name = entity.identifier + "." + entity.iid;
    auto inheritance = "@" + entity.identifier;
    return withInheritance ? name + inheritance : name;
  }

  namespace entity
  {
    orxVECTOR WorldPosition(const ldtk::EntityInstance &entity, const Source &source)
    {
      auto position = OrxVector(entity.px[0], entity.px[1]);
      // auto levelPosition = IntPointToVector(source.level.position);
      // orxVector_Add(&position, &position, &levelPosition);
      return position;
    }

    orxVECTOR Size(const ldtk::EntityInstance &entity)
    {
      auto size = OrxVector(entity.width, entity.height);
      return size;
    }

    void SetFieldConfig(const ldtk::FieldInstance &field)
    {
      if (field.type == "Int")
      {
        orxConfig_SetS64(field.identifier.data(), field.value.template get<orxS64>());
      }
      else if (field.type == "Float")
      {
        orxConfig_SetFloat(field.identifier.data(), field.value.template get<orxFLOAT>());
      }
      else if (field.type == "Bool")
      {
        orxConfig_SetBool(field.identifier.data(), field.value.template get<orxBOOL>());
      }
      else if (field.type == "String")
      {
        orxConfig_SetString(field.identifier.data(), field.value.template get<std::string>().data());
      }
      else
      {
        orxLOG("Field %s of type %s is not supported yet", field.identifier.data(), field.type.data());
      }
    }

    void AddConfigFields(const ldtk::EntityInstance &entity)
    {
      for (auto &field : entity.fieldInstances)
      {
        SetFieldConfig(field);
      }
    }

    std::string DefaultConfigSection(const ldtk::EntityInstance &entity, const Source &source)
    {
      auto inherit = std::string("@Object");
      auto field = GetFieldByName(entity, "Inherit");
      if (field)
      {
        inherit = "@" + (*field).value.template get<std::string>();
      }
      auto section = EntitySectionName(entity, false);
      return section + inherit;
    }

    void CreateDefaultConfig(const ldtk::EntityInstance &entity, const Source &source, const std::string sectionName)
    {
      orxConfig_PushSection(sectionName.data());

      // Set world position
      auto position = entity::WorldPosition(entity, source);
      orxConfig_SetVector("Position", &position);

      // Set graphic
      if (entity.tile)
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
        auto size = OrxVector(entity.width, entity.height);
        auto pivot = OrxVector(entity.pivot);
        orxVector_Mul(&pivot, &pivot, &size);
        orxConfig_SetVector("Size", &size);
        orxConfig_SetVector("Pivot", &pivot);
      }

      AddConfigFields(entity);

      orxConfig_PopSection();
    }

    void AddToEntityList(const ldtk::EntityInstance &entity, const Source &source)
    {
      AddToAllObjects(source.level.identifier, EntitySectionName(entity, false));
    }
  }

  void CreateEntitySections(const Source &source)
  {
    for (const auto &entity : source.entities.entityInstances)
    {
      if (entity::callbacks.contains(entity.identifier))
      {
        auto CreateConfig = entity::callbacks.at(entity.identifier);
        CreateConfig(entity, source);
      }
      else
      {
        orxLOG("No specific support for entity %s, adding generic config", entity.identifier.data());
        entity::CreateDefaultConfig(entity, source, entity::DefaultConfigSection(entity, source));
        entity::AddToEntityList(entity, source);
      }
    }
  }

  ldtk::Project LoadLDtkProject(const orxSTRING mapLocation)
  {
    // Load map resource
    auto resourceLocation = orxResource_Locate("Map", mapLocation);
    orxASSERT(resourceLocation != orxNULL);
    auto handle = orxResource_Open(resourceLocation, orxFALSE);
    auto size = orxResource_GetSize(handle);
    std::vector<orxCHAR> bytes(size);
    auto read = orxResource_Read(handle, size, bytes.data(), orxNULL, orxNULL);
    orxResource_Close(handle);
    orxASSERT(size == read);
    return nlohmann::json::parse(bytes);
  }

  void ldtkToConfig(const orxSTRING mapLocation)
  {
    auto project = LoadLDtkProject(mapLocation);
    for (const auto &level : project.levels)
    {
      AddToAllLevels(level);
      AddLevelNeighbors(project, level);

      const auto levelName = level.identifier.data();
      const auto collisionLayerName = orxConfig_GetString(CONFIG_COLLISION_LAYER);
      const auto entityLayerName = orxConfig_GetString(CONFIG_ENTITY_LAYER);
      const auto source = Source(project, levelName, collisionLayerName, entityLayerName);

      // Define entities in the game world
      CreateEntitySections(source);

      // Define physics collision body for tiles
      const auto tileBodyName = std::string("TileCollisionBody");
      SetTileStaticBody(tileBodyName);

      for (const auto &layer : source.layers)
      {
        orxLOG("Processing layer %s", layer.identifier.data());

        if (layer.tilesetDefUid)
        {
          auto tileset = GetTilesetByID(project, *layer.tilesetDefUid);

          // Generate config for each tile
          for (const auto &tile : layer.autoLayerTiles)
          {
            // Define texture source for tiles
            SetTilesetTexture(tileset, source);

            // Generate graphics config in case it does not exist yet
            // const auto graphicSection =
            //     TileGraphicSection(tile, tileset);
            SetTileGraphic(tile, tileset);

            // Generate object config for this tile
            const auto objectSection =
                TileObjectSection(tile, tileset, source);
            SetTileObject(tile, layer, tileBodyName, source);

            // Push all the tiles as children for a single object
            AddToAllObjects(levelName, objectSection);
          }
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
