//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     Project data = nlohmann::json::parse(jsonString);

#pragma once

#include <optional>
#include "json.hpp"

#ifndef NLOHMANN_OPT_HELPER
#define NLOHMANN_OPT_HELPER
namespace nlohmann {
    template <typename T>
    struct adl_serializer<std::shared_ptr<T>> {
        static void to_json(json & j, const std::shared_ptr<T> & opt) {
            if (!opt) j = nullptr; else j = *opt;
        }

        static std::shared_ptr<T> from_json(const json & j) {
            if (j.is_null()) return std::make_shared<T>(); else return std::make_shared<T>(j.get<T>());
        }
    };
    template <typename T>
    struct adl_serializer<std::optional<T>> {
        static void to_json(json & j, const std::optional<T> & opt) {
            if (!opt) j = nullptr; else j = *opt;
        }

        static std::optional<T> from_json(const json & j) {
            if (j.is_null()) return std::make_optional<T>(); else return std::make_optional<T>(j.get<T>());
        }
    };
}
#endif

namespace ldtk {
    using nlohmann::json;

    #ifndef NLOHMANN_UNTYPED_ldtk_HELPER
    #define NLOHMANN_UNTYPED_ldtk_HELPER
    inline json get_untyped(const json & j, const char * property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<json>();
        }
        return json();
    }

    inline json get_untyped(const json & j, std::string property) {
        return get_untyped(j, property.data());
    }
    #endif

    #ifndef NLOHMANN_OPTIONAL_ldtk_HELPER
    #define NLOHMANN_OPTIONAL_ldtk_HELPER
    template <typename T>
    inline std::shared_ptr<T> get_heap_optional(const json & j, const char * property) {
        auto it = j.find(property);
        if (it != j.end() && !it->is_null()) {
            return j.at(property).get<std::shared_ptr<T>>();
        }
        return std::shared_ptr<T>();
    }

    template <typename T>
    inline std::shared_ptr<T> get_heap_optional(const json & j, std::string property) {
        return get_heap_optional<T>(j, property.data());
    }
    template <typename T>
    inline std::optional<T> get_stack_optional(const json & j, const char * property) {
        auto it = j.find(property);
        if (it != j.end() && !it->is_null()) {
            return j.at(property).get<std::optional<T>>();
        }
        return std::optional<T>();
    }

    template <typename T>
    inline std::optional<T> get_stack_optional(const json & j, std::string property) {
        return get_stack_optional<T>(j, property.data());
    }
    #endif

    /**
     * Possible values: `Manual`, `AfterLoad`, `BeforeSave`, `AfterSave`
     */
    enum class When : int { AFTER_LOAD, AFTER_SAVE, BEFORE_SAVE, MANUAL };

    struct LdtkCustomCommand {
        std::string command;
        /**
         * Possible values: `Manual`, `AfterLoad`, `BeforeSave`, `AfterSave`
         */
        When when;
    };

    /**
     * Possible values: `Any`, `OnlySame`, `OnlyTags`, `OnlySpecificEntity`
     */
    enum class AllowedRefs : int { ANY, ONLY_SAME, ONLY_SPECIFIC_ENTITY, ONLY_TAGS };

    /**
     * Possible values: `Hidden`, `ValueOnly`, `NameAndValue`, `EntityTile`, `LevelTile`,
     * `Points`, `PointStar`, `PointPath`, `PointPathLoop`, `RadiusPx`, `RadiusGrid`,
     * `ArrayCountWithLabel`, `ArrayCountNoLabel`, `RefLinkBetweenPivots`,
     * `RefLinkBetweenCenters`
     */
    enum class EditorDisplayMode : int { ARRAY_COUNT_NO_LABEL, ARRAY_COUNT_WITH_LABEL, ENTITY_TILE, HIDDEN, LEVEL_TILE, NAME_AND_VALUE, POINTS, POINT_PATH, POINT_PATH_LOOP, POINT_STAR, RADIUS_GRID, RADIUS_PX, REF_LINK_BETWEEN_CENTERS, REF_LINK_BETWEEN_PIVOTS, VALUE_ONLY };

    /**
     * Possible values: `Above`, `Center`, `Beneath`
     */
    enum class EditorDisplayPos : int { ABOVE, BENEATH, CENTER };

    /**
     * Possible values: `ZigZag`, `StraightArrow`, `CurvedArrow`, `ArrowsLine`, `DashedLine`
     */
    enum class EditorLinkStyle : int { ARROWS_LINE, CURVED_ARROW, DASHED_LINE, STRAIGHT_ARROW, ZIG_ZAG };

    enum class TextLanguageMode : int { LANG_C, LANG_HAXE, LANG_JS, LANG_JSON, LANG_LOG, LANG_LUA, LANG_MARKDOWN, LANG_PYTHON, LANG_RUBY, LANG_XML };

    /**
     * This section is mostly only intended for the LDtk editor app itself. You can safely
     * ignore it.
     */
    struct FieldDefinition {
        /**
         * Human readable value type. Possible values: `Int, Float, String, Bool, Color,
         * ExternEnum.XXX, LocalEnum.XXX, Point, FilePath`.<br/>  If the field is an array, this
         * field will look like `Array<...>` (eg. `Array<Int>`, `Array<Point>` etc.)<br/>  NOTE: if
         * you enable the advanced option **Use Multilines type**, you will have "*Multilines*"
         * instead of "*String*" when relevant.
         */
        std::string type;
        /**
         * Optional list of accepted file extensions for FilePath value type. Includes the dot:
         * `.ext`
         */
        std::optional<std::vector<std::string>> acceptFileTypes;
        /**
         * Possible values: `Any`, `OnlySame`, `OnlyTags`, `OnlySpecificEntity`
         */
        AllowedRefs allowedRefs;
        std::optional<int64_t> allowedRefsEntityUid;
        std::vector<std::string> allowedRefTags;
        bool allowOutOfLevelRef;
        /**
         * Array max length
         */
        std::optional<int64_t> arrayMaxLength;
        /**
         * Array min length
         */
        std::optional<int64_t> arrayMinLength;
        bool autoChainRef;
        /**
         * TRUE if the value can be null. For arrays, TRUE means it can contain null values
         * (exception: array of Points can't have null values).
         */
        bool canBeNull;
        /**
         * Default value if selected value is null or invalid.
         */
        nlohmann::json defaultOverride;
        /**
         * User defined documentation for this field to provide help/tips to level designers about
         * accepted values.
         */
        std::optional<std::string> doc;
        bool editorAlwaysShow;
        bool editorCutLongValues;
        std::optional<std::string> editorDisplayColor;
        /**
         * Possible values: `Hidden`, `ValueOnly`, `NameAndValue`, `EntityTile`, `LevelTile`,
         * `Points`, `PointStar`, `PointPath`, `PointPathLoop`, `RadiusPx`, `RadiusGrid`,
         * `ArrayCountWithLabel`, `ArrayCountNoLabel`, `RefLinkBetweenPivots`,
         * `RefLinkBetweenCenters`
         */
        EditorDisplayMode editorDisplayMode;
        /**
         * Possible values: `Above`, `Center`, `Beneath`
         */
        EditorDisplayPos editorDisplayPos;
        double editorDisplayScale;
        /**
         * Possible values: `ZigZag`, `StraightArrow`, `CurvedArrow`, `ArrowsLine`, `DashedLine`
         */
        EditorLinkStyle editorLinkStyle;
        bool editorShowInWorld;
        std::optional<std::string> editorTextPrefix;
        std::optional<std::string> editorTextSuffix;
        /**
         * User defined unique identifier
         */
        std::string identifier;
        /**
         * TRUE if the value is an array of multiple values
         */
        bool isArray;
        /**
         * Max limit for value, if applicable
         */
        std::optional<double> max;
        /**
         * Min limit for value, if applicable
         */
        std::optional<double> min;
        /**
         * Optional regular expression that needs to be matched to accept values. Expected format:
         * `/some_reg_ex/g`, with optional "i" flag.
         */
        std::optional<std::string> regex;
        bool symmetricalRef;
        /**
         * Possible values: &lt;`null`&gt;, `LangPython`, `LangRuby`, `LangJS`, `LangLua`, `LangC`,
         * `LangHaxe`, `LangMarkdown`, `LangJson`, `LangXml`, `LangLog`
         */
        std::optional<TextLanguageMode> textLanguageMode;
        /**
         * UID of the tileset used for a Tile
         */
        std::optional<int64_t> tilesetUid;
        /**
         * Internal enum representing the possible field types. Possible values: F_Int, F_Float,
         * F_String, F_Text, F_Bool, F_Color, F_Enum(...), F_Point, F_Path, F_EntityRef, F_Tile
         */
        std::string fieldDefinitionType;
        /**
         * Unique Int identifier
         */
        int64_t uid;
        /**
         * If TRUE, the color associated with this field will override the Entity or Level default
         * color in the editor UI. For Enum fields, this would be the color associated to their
         * values.
         */
        bool useForSmartColor;
    };

    /**
     * Possible values: `DiscardOldOnes`, `PreventAdding`, `MoveLastOne`
     */
    enum class LimitBehavior : int { DISCARD_OLD_ONES, MOVE_LAST_ONE, PREVENT_ADDING };

    /**
     * If TRUE, the maxCount is a "per world" limit, if FALSE, it's a "per level". Possible
     * values: `PerLayer`, `PerLevel`, `PerWorld`
     */
    enum class LimitScope : int { PER_LAYER, PER_LEVEL, PER_WORLD };

    /**
     * Possible values: `Rectangle`, `Ellipse`, `Tile`, `Cross`
     */
    enum class RenderMode : int { CROSS, ELLIPSE, RECTANGLE, TILE };

    /**
     * This object represents a custom sub rectangle in a Tileset image.
     */
    struct TilesetRectangle {
        /**
         * Height in pixels
         */
        int64_t h;
        /**
         * UID of the tileset
         */
        int64_t tilesetUid;
        /**
         * Width in pixels
         */
        int64_t w;
        /**
         * X pixels coordinate of the top-left corner in the Tileset image
         */
        int64_t x;
        /**
         * Y pixels coordinate of the top-left corner in the Tileset image
         */
        int64_t y;
    };

    /**
     * An enum describing how the the Entity tile is rendered inside the Entity bounds. Possible
     * values: `Cover`, `FitInside`, `Repeat`, `Stretch`, `FullSizeCropped`,
     * `FullSizeUncropped`, `NineSlice`
     */
    enum class TileRenderMode : int { COVER, FIT_INSIDE, FULL_SIZE_CROPPED, FULL_SIZE_UNCROPPED, NINE_SLICE, REPEAT, STRETCH };

    struct EntityDefinition {
        /**
         * Base entity color
         */
        std::string color;
        /**
         * User defined documentation for this element to provide help/tips to level designers.
         */
        std::optional<std::string> doc;
        /**
         * If enabled, all instances of this entity will be listed in the project "Table of content"
         * object.
         */
        bool exportToToc;
        /**
         * Array of field definitions
         */
        std::vector<FieldDefinition> fieldDefs;
        double fillOpacity;
        /**
         * Pixel height
         */
        int64_t height;
        bool hollow;
        /**
         * User defined unique identifier
         */
        std::string identifier;
        /**
         * Only applies to entities resizable on both X/Y. If TRUE, the entity instance width/height
         * will keep the same aspect ratio as the definition.
         */
        bool keepAspectRatio;
        /**
         * Possible values: `DiscardOldOnes`, `PreventAdding`, `MoveLastOne`
         */
        LimitBehavior limitBehavior;
        /**
         * If TRUE, the maxCount is a "per world" limit, if FALSE, it's a "per level". Possible
         * values: `PerLayer`, `PerLevel`, `PerWorld`
         */
        LimitScope limitScope;
        double lineOpacity;
        /**
         * Max instances count
         */
        int64_t maxCount;
        /**
         * Max pixel height (only applies if the entity is resizable on Y)
         */
        std::optional<int64_t> maxHeight;
        /**
         * Max pixel width (only applies if the entity is resizable on X)
         */
        std::optional<int64_t> maxWidth;
        /**
         * Min pixel height (only applies if the entity is resizable on Y)
         */
        std::optional<int64_t> minHeight;
        /**
         * Min pixel width (only applies if the entity is resizable on X)
         */
        std::optional<int64_t> minWidth;
        /**
         * An array of 4 dimensions for the up/right/down/left borders (in this order) when using
         * 9-slice mode for `tileRenderMode`.<br/>  If the tileRenderMode is not NineSlice, then
         * this array is empty.<br/>  See: https://en.wikipedia.org/wiki/9-slice_scaling
         */
        std::vector<int64_t> nineSliceBorders;
        /**
         * Pivot X coordinate (from 0 to 1.0)
         */
        double pivotX;
        /**
         * Pivot Y coordinate (from 0 to 1.0)
         */
        double pivotY;
        /**
         * Possible values: `Rectangle`, `Ellipse`, `Tile`, `Cross`
         */
        RenderMode renderMode;
        /**
         * If TRUE, the entity instances will be resizable horizontally
         */
        bool resizableX;
        /**
         * If TRUE, the entity instances will be resizable vertically
         */
        bool resizableY;
        /**
         * Display entity name in editor
         */
        bool showName;
        /**
         * An array of strings that classifies this entity
         */
        std::vector<std::string> tags;
        /**
         * **WARNING**: this deprecated value is no longer exported since version 1.2.0  Replaced
         * by: `tileRect`
         */
        std::optional<int64_t> tileID;
        double tileOpacity;
        /**
         * An object representing a rectangle from an existing Tileset
         */
        std::optional<TilesetRectangle> tileRect;
        /**
         * An enum describing how the the Entity tile is rendered inside the Entity bounds. Possible
         * values: `Cover`, `FitInside`, `Repeat`, `Stretch`, `FullSizeCropped`,
         * `FullSizeUncropped`, `NineSlice`
         */
        TileRenderMode tileRenderMode;
        /**
         * Tileset ID used for optional tile display
         */
        std::optional<int64_t> tilesetID;
        /**
         * Unique Int identifier
         */
        int64_t uid;
        /**
         * This tile overrides the one defined in `tileRect` in the UI
         */
        std::optional<TilesetRectangle> uiTileRect;
        /**
         * Pixel width
         */
        int64_t width;
    };

    struct EnumValueDefinition {
        /**
         * **WARNING**: this deprecated value is no longer exported since version 1.4.0  Replaced
         * by: `tileRect`
         */
        std::optional<std::vector<int64_t>> tileSrcRect;
        /**
         * Optional color
         */
        int64_t color;
        /**
         * Enum value
         */
        std::string id;
        /**
         * **WARNING**: this deprecated value is no longer exported since version 1.4.0  Replaced
         * by: `tileRect`
         */
        std::optional<int64_t> tileID;
        /**
         * Optional tileset rectangle to represents this value
         */
        std::optional<TilesetRectangle> tileRect;
    };

    struct EnumDefinition {
        std::optional<std::string> externalFileChecksum;
        /**
         * Relative path to the external file providing this Enum
         */
        std::optional<std::string> externalRelPath;
        /**
         * Tileset UID if provided
         */
        std::optional<int64_t> iconTilesetUid;
        /**
         * User defined unique identifier
         */
        std::string identifier;
        /**
         * An array of user-defined tags to organize the Enums
         */
        std::vector<std::string> tags;
        /**
         * Unique Int identifier
         */
        int64_t uid;
        /**
         * All possible enum values, with their optional Tile infos.
         */
        std::vector<EnumValueDefinition> values;
    };

    /**
     * Checker mode Possible values: `None`, `Horizontal`, `Vertical`
     */
    enum class Checker : int { HORIZONTAL, NONE, VERTICAL };

    /**
     * Defines how tileIds array is used Possible values: `Single`, `Stamp`
     */
    enum class TileMode : int { SINGLE, STAMP };

    /**
     * This complex section isn't meant to be used by game devs at all, as these rules are
     * completely resolved internally by the editor before any saving. You should just ignore
     * this part.
     */
    struct AutoLayerRuleDefinition {
        /**
         * If FALSE, the rule effect isn't applied, and no tiles are generated.
         */
        bool active;
        double alpha;
        /**
         * When TRUE, the rule will prevent other rules to be applied in the same cell if it matches
         * (TRUE by default).
         */
        bool breakOnMatch;
        /**
         * Chances for this rule to be applied (0 to 1)
         */
        double chance;
        /**
         * Checker mode Possible values: `None`, `Horizontal`, `Vertical`
         */
        Checker checker;
        /**
         * If TRUE, allow rule to be matched by flipping its pattern horizontally
         */
        bool flipX;
        /**
         * If TRUE, allow rule to be matched by flipping its pattern vertically
         */
        bool flipY;
        /**
         * Default IntGrid value when checking cells outside of level bounds
         */
        std::optional<int64_t> outOfBoundsValue;
        /**
         * Rule pattern (size x size)
         */
        std::vector<int64_t> pattern;
        /**
         * If TRUE, enable Perlin filtering to only apply rule on specific random area
         */
        bool perlinActive;
        double perlinOctaves;
        double perlinScale;
        double perlinSeed;
        /**
         * X pivot of a tile stamp (0-1)
         */
        double pivotX;
        /**
         * Y pivot of a tile stamp (0-1)
         */
        double pivotY;
        /**
         * Pattern width & height. Should only be 1,3,5 or 7.
         */
        int64_t size;
        /**
         * Array of all the tile IDs. They are used randomly or as stamps, based on `tileMode` value.
         */
        std::vector<int64_t> tileIDS;
        /**
         * Defines how tileIds array is used Possible values: `Single`, `Stamp`
         */
        TileMode tileMode;
        /**
         * Max random offset for X tile pos
         */
        int64_t tileRandomXMax;
        /**
         * Min random offset for X tile pos
         */
        int64_t tileRandomXMin;
        /**
         * Max random offset for Y tile pos
         */
        int64_t tileRandomYMax;
        /**
         * Min random offset for Y tile pos
         */
        int64_t tileRandomYMin;
        /**
         * Tile X offset
         */
        int64_t tileXOffset;
        /**
         * Tile Y offset
         */
        int64_t tileYOffset;
        /**
         * Unique Int identifier
         */
        int64_t uid;
        /**
         * X cell coord modulo
         */
        int64_t xModulo;
        /**
         * X cell start offset
         */
        int64_t xOffset;
        /**
         * Y cell coord modulo
         */
        int64_t yModulo;
        /**
         * Y cell start offset
         */
        int64_t yOffset;
    };

    struct AutoLayerRuleGroup {
        bool active;
        /**
         * *This field was removed in 1.0.0 and should no longer be used.*
         */
        std::optional<bool> collapsed;
        std::optional<std::string> color;
        std::optional<TilesetRectangle> icon;
        bool isOptional;
        std::string name;
        std::vector<AutoLayerRuleDefinition> rules;
        int64_t uid;
        bool usesWizard;
    };

    /**
     * IntGrid value definition
     */
    struct IntGridValueDefinition {
        std::string color;
        /**
         * Parent group identifier (0 if none)
         */
        int64_t groupUid;
        /**
         * User defined unique identifier
         */
        std::optional<std::string> identifier;
        std::optional<TilesetRectangle> tile;
        /**
         * The IntGrid value itself
         */
        int64_t value;
    };

    /**
     * IntGrid value group definition
     */
    struct IntGridValueGroupDefinition {
        /**
         * User defined color
         */
        std::optional<std::string> color;
        /**
         * User defined string identifier
         */
        std::optional<std::string> identifier;
        /**
         * Group unique ID
         */
        int64_t uid;
    };

    /**
     * Type of the layer as Haxe Enum Possible values: `IntGrid`, `Entities`, `Tiles`,
     * `AutoLayer`
     */
    enum class Type : int { AUTO_LAYER, ENTITIES, INT_GRID, TILES };

    struct LayerDefinition {
        /**
         * Type of the layer (*IntGrid, Entities, Tiles or AutoLayer*)
         */
        std::string type;
        /**
         * Contains all the auto-layer rule definitions.
         */
        std::vector<AutoLayerRuleGroup> autoRuleGroups;
        std::optional<int64_t> autoSourceLayerDefUid;
        /**
         * **WARNING**: this deprecated value is no longer exported since version 1.2.0  Replaced
         * by: `tilesetDefUid`
         */
        std::optional<int64_t> autoTilesetDefUid;
        /**
         * Allow editor selections when the layer is not currently active.
         */
        bool canSelectWhenInactive;
        /**
         * Opacity of the layer (0 to 1.0)
         */
        double displayOpacity;
        /**
         * User defined documentation for this element to provide help/tips to level designers.
         */
        std::optional<std::string> doc;
        /**
         * An array of tags to forbid some Entities in this layer
         */
        std::vector<std::string> excludedTags;
        /**
         * Width and height of the grid in pixels
         */
        int64_t gridSize;
        /**
         * Height of the optional "guide" grid in pixels
         */
        int64_t guideGridHei;
        /**
         * Width of the optional "guide" grid in pixels
         */
        int64_t guideGridWid;
        bool hideFieldsWhenInactive;
        /**
         * Hide the layer from the list on the side of the editor view.
         */
        bool hideInList;
        /**
         * User defined unique identifier
         */
        std::string identifier;
        /**
         * Alpha of this layer when it is not the active one.
         */
        double inactiveOpacity;
        /**
         * An array that defines extra optional info for each IntGrid value.<br/>  WARNING: the
         * array order is not related to actual IntGrid values! As user can re-order IntGrid values
         * freely, you may value "2" before value "1" in this array.
         */
        std::vector<IntGridValueDefinition> intGridValues;
        /**
         * Group informations for IntGrid values
         */
        std::vector<IntGridValueGroupDefinition> intGridValuesGroups;
        /**
         * Parallax horizontal factor (from -1 to 1, defaults to 0) which affects the scrolling
         * speed of this layer, creating a fake 3D (parallax) effect.
         */
        double parallaxFactorX;
        /**
         * Parallax vertical factor (from -1 to 1, defaults to 0) which affects the scrolling speed
         * of this layer, creating a fake 3D (parallax) effect.
         */
        double parallaxFactorY;
        /**
         * If true (default), a layer with a parallax factor will also be scaled up/down accordingly.
         */
        bool parallaxScaling;
        /**
         * X offset of the layer, in pixels (IMPORTANT: this should be added to the `LayerInstance`
         * optional offset)
         */
        int64_t pxOffsetX;
        /**
         * Y offset of the layer, in pixels (IMPORTANT: this should be added to the `LayerInstance`
         * optional offset)
         */
        int64_t pxOffsetY;
        /**
         * If TRUE, the content of this layer will be used when rendering levels in a simplified way
         * for the world view
         */
        bool renderInWorldView;
        /**
         * An array of tags to filter Entities that can be added to this layer
         */
        std::vector<std::string> requiredTags;
        /**
         * If the tiles are smaller or larger than the layer grid, the pivot value will be used to
         * position the tile relatively its grid cell.
         */
        double tilePivotX;
        /**
         * If the tiles are smaller or larger than the layer grid, the pivot value will be used to
         * position the tile relatively its grid cell.
         */
        double tilePivotY;
        /**
         * Reference to the default Tileset UID being used by this layer definition.<br/>
         * **WARNING**: some layer *instances* might use a different tileset. So most of the time,
         * you should probably use the `__tilesetDefUid` value found in layer instances.<br/>  Note:
         * since version 1.0.0, the old `autoTilesetDefUid` was removed and merged into this value.
         */
        std::optional<int64_t> tilesetDefUid;
        /**
         * Type of the layer as Haxe Enum Possible values: `IntGrid`, `Entities`, `Tiles`,
         * `AutoLayer`
         */
        Type layerDefinitionType;
        /**
         * User defined color for the UI
         */
        std::optional<std::string> uiColor;
        /**
         * Unique Int identifier
         */
        int64_t uid;
    };

    /**
     * In a tileset definition, user defined meta-data of a tile.
     */
    struct TileCustomMetadata {
        std::string data;
        int64_t tileID;
    };

    enum class EmbedAtlas : int { LDTK_ICONS };

    /**
     * In a tileset definition, enum based tag infos
     */
    struct EnumTagValue {
        std::string enumValueID;
        std::vector<int64_t> tileIDS;
    };

    /**
     * The `Tileset` definition is the most important part among project definitions. It
     * contains some extra informations about each integrated tileset. If you only had to parse
     * one definition section, that would be the one.
     */
    struct TilesetDefinition {
        /**
         * Grid-based height
         */
        int64_t cHei;
        /**
         * Grid-based width
         */
        int64_t cWid;
        /**
         * The following data is used internally for various optimizations. It's always synced with
         * source image changes.
         */
        std::optional<std::map<std::string, nlohmann::json>> cachedPixelData;
        /**
         * An array of custom tile metadata
         */
        std::vector<TileCustomMetadata> customData;
        /**
         * If this value is set, then it means that this atlas uses an internal LDtk atlas image
         * instead of a loaded one. Possible values: &lt;`null`&gt;, `LdtkIcons`
         */
        std::optional<EmbedAtlas> embedAtlas;
        /**
         * Tileset tags using Enum values specified by `tagsSourceEnumId`. This array contains 1
         * element per Enum value, which contains an array of all Tile IDs that are tagged with it.
         */
        std::vector<EnumTagValue> enumTags;
        /**
         * User defined unique identifier
         */
        std::string identifier;
        /**
         * Distance in pixels from image borders
         */
        int64_t padding;
        /**
         * Image height in pixels
         */
        int64_t pxHei;
        /**
         * Image width in pixels
         */
        int64_t pxWid;
        /**
         * Path to the source file, relative to the current project JSON file<br/>  It can be null
         * if no image was provided, or when using an embed atlas.
         */
        std::optional<std::string> relPath;
        /**
         * Array of group of tiles selections, only meant to be used in the editor
         */
        std::vector<std::map<std::string, nlohmann::json>> savedSelections;
        /**
         * Space in pixels between all tiles
         */
        int64_t spacing;
        /**
         * An array of user-defined tags to organize the Tilesets
         */
        std::vector<std::string> tags;
        /**
         * Optional Enum definition UID used for this tileset meta-data
         */
        std::optional<int64_t> tagsSourceEnumUid;
        int64_t tileGridSize;
        /**
         * Unique Intidentifier
         */
        int64_t uid;
    };

    /**
     * If you're writing your own LDtk importer, you should probably just ignore *most* stuff in
     * the `defs` section, as it contains data that are mostly important to the editor. To keep
     * you away from the `defs` section and avoid some unnecessary JSON parsing, important data
     * from definitions is often duplicated in fields prefixed with a double underscore (eg.
     * `__identifier` or `__type`).  The 2 only definition types you might need here are
     * **Tilesets** and **Enums**.
     *
     * A structure containing all the definitions of this project
     */
    struct Definitions {
        /**
         * All entities definitions, including their custom fields
         */
        std::vector<EntityDefinition> entities;
        /**
         * All internal enums
         */
        std::vector<EnumDefinition> enums;
        /**
         * Note: external enums are exactly the same as `enums`, except they have a `relPath` to
         * point to an external source file.
         */
        std::vector<EnumDefinition> externalEnums;
        /**
         * All layer definitions
         */
        std::vector<LayerDefinition> layers;
        /**
         * All custom fields available to all levels.
         */
        std::vector<FieldDefinition> levelFields;
        /**
         * All tilesets
         */
        std::vector<TilesetDefinition> tilesets;
    };

    enum class Flag : int { DISCARD_PRE_CSV_INT_GRID, EXPORT_PRE_CSV_INT_GRID_FORMAT, IGNORE_BACKUP_SUGGEST, MULTI_WORLDS, PREPEND_INDEX_TO_LEVEL_FILE_NAMES, USE_MULTILINES_TYPE };

    struct FieldInstance {
        /**
         * Field definition identifier
         */
        std::string identifier;
        /**
         * Optional TilesetRect used to display this field (this can be the field own Tile, or some
         * other Tile guessed from the value, like an Enum).
         */
        std::optional<TilesetRectangle> tile;
        /**
         * Type of the field, such as `Int`, `Float`, `String`, `Enum(my_enum_name)`, `Bool`,
         * etc.<br/>  NOTE: if you enable the advanced option **Use Multilines type**, you will have
         * "*Multilines*" instead of "*String*" when relevant.
         */
        std::string type;
        /**
         * Actual value of the field instance. The value type varies, depending on `__type`:<br/>
         * - For **classic types** (ie. Integer, Float, Boolean, String, Text and FilePath), you
         * just get the actual value with the expected type.<br/>   - For **Color**, the value is an
         * hexadecimal string using "#rrggbb" format.<br/>   - For **Enum**, the value is a String
         * representing the selected enum value.<br/>   - For **Point**, the value is a
         * [GridPoint](#ldtk-GridPoint) object.<br/>   - For **Tile**, the value is a
         * [TilesetRect](#ldtk-TilesetRect) object.<br/>   - For **EntityRef**, the value is an
         * [EntityReferenceInfos](#ldtk-EntityReferenceInfos) object.<br/><br/>  If the field is an
         * array, then this `__value` will also be a JSON array.
         */
        nlohmann::json value;
        /**
         * Reference of the **Field definition** UID
         */
        int64_t defUid;
        /**
         * Editor internal raw values
         */
        std::vector<nlohmann::json> realEditorValues;
    };

    struct EntityInstance {
        /**
         * Grid-based coordinates (`[x,y]` format)
         */
        std::vector<int64_t> grid;
        /**
         * Entity definition identifier
         */
        std::string identifier;
        /**
         * Pivot coordinates  (`[x,y]` format, values are from 0 to 1) of the Entity
         */
        std::vector<double> pivot;
        /**
         * The entity "smart" color, guessed from either Entity definition, or one its field
         * instances.
         */
        std::string smartColor;
        /**
         * Array of tags defined in this Entity definition
         */
        std::vector<std::string> tags;
        /**
         * Optional TilesetRect used to display this entity (it could either be the default Entity
         * tile, or some tile provided by a field value, like an Enum).
         */
        std::optional<TilesetRectangle> tile;
        /**
         * X world coordinate in pixels
         */
        int64_t worldX;
        /**
         * Y world coordinate in pixels
         */
        int64_t worldY;
        /**
         * Reference of the **Entity definition** UID
         */
        int64_t defUid;
        /**
         * An array of all custom fields and their values.
         */
        std::vector<FieldInstance> fieldInstances;
        /**
         * Entity height in pixels. For non-resizable entities, it will be the same as Entity
         * definition.
         */
        int64_t height;
        /**
         * Unique instance identifier
         */
        std::string iid;
        /**
         * Pixel coordinates (`[x,y]` format) in current level coordinate space. Don't forget
         * optional layer offsets, if they exist!
         */
        std::vector<int64_t> px;
        /**
         * Entity width in pixels. For non-resizable entities, it will be the same as Entity
         * definition.
         */
        int64_t width;
    };

    /**
     * This object describes the "location" of an Entity instance in the project worlds.
     */
    struct ReferenceToAnEntityInstance {
        /**
         * IID of the refered EntityInstance
         */
        std::string entityIid;
        /**
         * IID of the LayerInstance containing the refered EntityInstance
         */
        std::string layerIid;
        /**
         * IID of the Level containing the refered EntityInstance
         */
        std::string levelIid;
        /**
         * IID of the World containing the refered EntityInstance
         */
        std::string worldIid;
    };

    /**
     * This object is just a grid-based coordinate used in Field values.
     */
    struct GridPoint {
        /**
         * X grid-based coordinate
         */
        int64_t cx;
        /**
         * Y grid-based coordinate
         */
        int64_t cy;
    };

    /**
     * IntGrid value instance
     */
    struct IntGridValueInstance {
        /**
         * Coordinate ID in the layer grid
         */
        int64_t coordID;
        /**
         * IntGrid value
         */
        int64_t v;
    };

    /**
     * This structure represents a single tile from a given Tileset.
     */
    struct TileInstance {
        /**
         * Alpha/opacity of the tile (0-1, defaults to 1)
         */
        double a;
        /**
         * Internal data used by the editor.<br/>  For auto-layer tiles: `[ruleId, coordId]`.<br/>
         * For tile-layer tiles: `[coordId]`.
         */
        std::vector<int64_t> d;
        /**
         * "Flip bits", a 2-bits integer to represent the mirror transformations of the tile.<br/>
         * - Bit 0 = X flip<br/>   - Bit 1 = Y flip<br/>   Examples: f=0 (no flip), f=1 (X flip
         * only), f=2 (Y flip only), f=3 (both flips)
         */
        int64_t f;
        /**
         * Pixel coordinates of the tile in the **layer** (`[x,y]` format). Don't forget optional
         * layer offsets, if they exist!
         */
        std::vector<int64_t> px;
        /**
         * Pixel coordinates of the tile in the **tileset** (`[x,y]` format)
         */
        std::vector<int64_t> src;
        /**
         * The *Tile ID* in the corresponding tileset.
         */
        int64_t t;
    };

    struct LayerInstance {
        /**
         * Grid-based height
         */
        int64_t cHei;
        /**
         * Grid-based width
         */
        int64_t cWid;
        /**
         * Grid size
         */
        int64_t gridSize;
        /**
         * Layer definition identifier
         */
        std::string identifier;
        /**
         * Layer opacity as Float [0-1]
         */
        double opacity;
        /**
         * Total layer X pixel offset, including both instance and definition offsets.
         */
        int64_t pxTotalOffsetX;
        /**
         * Total layer Y pixel offset, including both instance and definition offsets.
         */
        int64_t pxTotalOffsetY;
        /**
         * The definition UID of corresponding Tileset, if any.
         */
        std::optional<int64_t> tilesetDefUid;
        /**
         * The relative path to corresponding Tileset, if any.
         */
        std::optional<std::string> tilesetRelPath;
        /**
         * Layer type (possible values: IntGrid, Entities, Tiles or AutoLayer)
         */
        std::string type;
        /**
         * An array containing all tiles generated by Auto-layer rules. The array is already sorted
         * in display order (ie. 1st tile is beneath 2nd, which is beneath 3rd etc.).<br/><br/>
         * Note: if multiple tiles are stacked in the same cell as the result of different rules,
         * all tiles behind opaque ones will be discarded.
         */
        std::vector<TileInstance> autoLayerTiles;
        std::vector<EntityInstance> entityInstances;
        std::vector<TileInstance> gridTiles;
        /**
         * Unique layer instance identifier
         */
        std::string iid;
        /**
         * **WARNING**: this deprecated value is no longer exported since version 1.0.0  Replaced
         * by: `intGridCsv`
         */
        std::optional<std::vector<IntGridValueInstance>> intGrid;
        /**
         * A list of all values in the IntGrid layer, stored in CSV format (Comma Separated
         * Values).<br/>  Order is from left to right, and top to bottom (ie. first row from left to
         * right, followed by second row, etc).<br/>  `0` means "empty cell" and IntGrid values
         * start at 1.<br/>  The array size is `__cWid` x `__cHei` cells.
         */
        std::vector<int64_t> intGridCSV;
        /**
         * Reference the Layer definition UID
         */
        int64_t layerDefUid;
        /**
         * Reference to the UID of the level containing this layer instance
         */
        int64_t levelID;
        /**
         * An Array containing the UIDs of optional rules that were enabled in this specific layer
         * instance.
         */
        std::vector<int64_t> optionalRules;
        /**
         * This layer can use another tileset by overriding the tileset UID here.
         */
        std::optional<int64_t> overrideTilesetUid;
        /**
         * X offset in pixels to render this layer, usually 0 (IMPORTANT: this should be added to
         * the `LayerDef` optional offset, so you should probably prefer using `__pxTotalOffsetX`
         * which contains the total offset value)
         */
        int64_t pxOffsetX;
        /**
         * Y offset in pixels to render this layer, usually 0 (IMPORTANT: this should be added to
         * the `LayerDef` optional offset, so you should probably prefer using `__pxTotalOffsetX`
         * which contains the total offset value)
         */
        int64_t pxOffsetY;
        /**
         * Random seed used for Auto-Layers rendering
         */
        int64_t seed;
        /**
         * Layer instance visibility
         */
        bool visible;
    };

    /**
     * Level background image position info
     */
    struct LevelBackgroundPosition {
        /**
         * An array of 4 float values describing the cropped sub-rectangle of the displayed
         * background image. This cropping happens when original is larger than the level bounds.
         * Array format: `[ cropX, cropY, cropWidth, cropHeight ]`
         */
        std::vector<double> cropRect;
        /**
         * An array containing the `[scaleX,scaleY]` values of the **cropped** background image,
         * depending on `bgPos` option.
         */
        std::vector<double> scale;
        /**
         * An array containing the `[x,y]` pixel coordinates of the top-left corner of the
         * **cropped** background image, depending on `bgPos` option.
         */
        std::vector<int64_t> topLeftPx;
    };

    enum class BgPos : int { CONTAIN, COVER, COVER_DIRTY, REPEAT, UNSCALED };

    /**
     * Nearby level info
     */
    struct NeighbourLevel {
        /**
         * A single lowercase character tipping on the level location (`n`orth, `s`outh, `w`est,
         * `e`ast).<br/>  Since 1.4.0, this character value can also be `<` (neighbour depth is
         * lower), `>` (neighbour depth is greater) or `o` (levels overlap and share the same world
         * depth).
         */
        std::string dir;
        /**
         * Neighbour Instance Identifier
         */
        std::string levelIid;
        /**
         * **WARNING**: this deprecated value is no longer exported since version 1.2.0  Replaced
         * by: `levelIid`
         */
        std::optional<int64_t> levelUid;
    };

    /**
     * This section contains all the level data. It can be found in 2 distinct forms, depending
     * on Project current settings:  - If "*Separate level files*" is **disabled** (default):
     * full level data is *embedded* inside the main Project JSON file, - If "*Separate level
     * files*" is **enabled**: level data is stored in *separate* standalone `.ldtkl` files (one
     * per level). In this case, the main Project JSON file will still contain most level data,
     * except heavy sections, like the `layerInstances` array (which will be null). The
     * `externalRelPath` string points to the `ldtkl` file.  A `ldtkl` file is just a JSON file
     * containing exactly what is described below.
     */
    struct Level {
        /**
         * Background color of the level (same as `bgColor`, except the default value is
         * automatically used here if its value is `null`)
         */
        std::string bgColor;
        /**
         * Position informations of the background image, if there is one.
         */
        std::optional<LevelBackgroundPosition> bgPos;
        /**
         * An array listing all other levels touching this one on the world map. Since 1.4.0, this
         * includes levels that overlap in the same world layer, or in nearby world layers.<br/>
         * Only relevant for world layouts where level spatial positioning is manual (ie. GridVania,
         * Free). For Horizontal and Vertical layouts, this array is always empty.
         */
        std::vector<NeighbourLevel> neighbours;
        /**
         * The "guessed" color for this level in the editor, decided using either the background
         * color or an existing custom field.
         */
        std::string smartColor;
        /**
         * Background color of the level. If `null`, the project `defaultLevelBgColor` should be
         * used.
         */
        std::optional<std::string> levelBgColor;
        /**
         * Background image X pivot (0-1)
         */
        double bgPivotX;
        /**
         * Background image Y pivot (0-1)
         */
        double bgPivotY;
        /**
         * An enum defining the way the background image (if any) is positioned on the level. See
         * `__bgPos` for resulting position info. Possible values: &lt;`null`&gt;, `Unscaled`,
         * `Contain`, `Cover`, `CoverDirty`, `Repeat`
         */
        std::optional<BgPos> levelBgPos;
        /**
         * The *optional* relative path to the level background image.
         */
        std::optional<std::string> bgRelPath;
        /**
         * This value is not null if the project option "*Save levels separately*" is enabled. In
         * this case, this **relative** path points to the level Json file.
         */
        std::optional<std::string> externalRelPath;
        /**
         * An array containing this level custom field values.
         */
        std::vector<FieldInstance> fieldInstances;
        /**
         * User defined unique identifier
         */
        std::string identifier;
        /**
         * Unique instance identifier
         */
        std::string iid;
        /**
         * An array containing all Layer instances. **IMPORTANT**: if the project option "*Save
         * levels separately*" is enabled, this field will be `null`.<br/>  This array is **sorted
         * in display order**: the 1st layer is the top-most and the last is behind.
         */
        std::optional<std::vector<LayerInstance>> layerInstances;
        /**
         * Height of the level in pixels
         */
        int64_t pxHei;
        /**
         * Width of the level in pixels
         */
        int64_t pxWid;
        /**
         * Unique Int identifier
         */
        int64_t uid;
        /**
         * If TRUE, the level identifier will always automatically use the naming pattern as defined
         * in `Project.levelNamePattern`. Becomes FALSE if the identifier is manually modified by
         * user.
         */
        bool useAutoIdentifier;
        /**
         * Index that represents the "depth" of the level in the world. Default is 0, greater means
         * "above", lower means "below".<br/>  This value is mostly used for display only and is
         * intended to make stacking of levels easier to manage.
         */
        int64_t worldDepth;
        /**
         * World X coordinate in pixels.<br/>  Only relevant for world layouts where level spatial
         * positioning is manual (ie. GridVania, Free). For Horizontal and Vertical layouts, the
         * value is always -1 here.
         */
        int64_t worldX;
        /**
         * World Y coordinate in pixels.<br/>  Only relevant for world layouts where level spatial
         * positioning is manual (ie. GridVania, Free). For Horizontal and Vertical layouts, the
         * value is always -1 here.
         */
        int64_t worldY;
    };

    struct LdtkTableOfContentEntry {
        std::string identifier;
        std::vector<ReferenceToAnEntityInstance> instances;
    };

    enum class WorldLayout : int { FREE, GRID_VANIA, LINEAR_HORIZONTAL, LINEAR_VERTICAL };

    /**
     * **IMPORTANT**: this type is available as a preview. You can rely on it to update your
     * importers, for when it will be officially available.  A World contains multiple levels,
     * and it has its own layout settings.
     */
    struct World {
        /**
         * Default new level height
         */
        int64_t defaultLevelHeight;
        /**
         * Default new level width
         */
        int64_t defaultLevelWidth;
        /**
         * User defined unique identifier
         */
        std::string identifier;
        /**
         * Unique instance identifer
         */
        std::string iid;
        /**
         * All levels from this world. The order of this array is only relevant in
         * `LinearHorizontal` and `linearVertical` world layouts (see `worldLayout` value).
         * Otherwise, you should refer to the `worldX`,`worldY` coordinates of each Level.
         */
        std::vector<Level> levels;
        /**
         * Height of the world grid in pixels.
         */
        int64_t worldGridHeight;
        /**
         * Width of the world grid in pixels.
         */
        int64_t worldGridWidth;
        /**
         * An enum that describes how levels are organized in this project (ie. linearly or in a 2D
         * space). Possible values: `Free`, `GridVania`, `LinearHorizontal`, `LinearVertical`, `null`
         */
        std::optional<WorldLayout> worldLayout;
    };

    /**
     * This object is not actually used by LDtk. It ONLY exists to force explicit references to
     * all types, to make sure QuickType finds them and integrate all of them. Otherwise,
     * Quicktype will drop types that are not explicitely used.
     */
    struct ForcedRefs {
        std::optional<AutoLayerRuleGroup> autoLayerRuleGroup;
        std::optional<AutoLayerRuleDefinition> autoRuleDef;
        std::optional<LdtkCustomCommand> customCommand;
        std::optional<Definitions> definitions;
        std::optional<EntityDefinition> entityDef;
        std::optional<EntityInstance> entityInstance;
        std::optional<ReferenceToAnEntityInstance> entityReferenceInfos;
        std::optional<EnumDefinition> enumDef;
        std::optional<EnumValueDefinition> enumDefValues;
        std::optional<EnumTagValue> enumTagValue;
        std::optional<FieldDefinition> fieldDef;
        std::optional<FieldInstance> fieldInstance;
        std::optional<GridPoint> gridPoint;
        std::optional<IntGridValueDefinition> intGridValueDef;
        std::optional<IntGridValueGroupDefinition> intGridValueGroupDef;
        std::optional<IntGridValueInstance> intGridValueInstance;
        std::optional<LayerDefinition> layerDef;
        std::optional<LayerInstance> layerInstance;
        std::optional<Level> level;
        std::optional<LevelBackgroundPosition> levelBgPosInfos;
        std::optional<NeighbourLevel> neighbourLevel;
        std::optional<LdtkTableOfContentEntry> tableOfContentEntry;
        std::optional<TileInstance> tile;
        std::optional<TileCustomMetadata> tileCustomMetadata;
        std::optional<TilesetDefinition> tilesetDef;
        std::optional<TilesetRectangle> tilesetRect;
        std::optional<World> world;
    };

    /**
     * Naming convention for Identifiers (first-letter uppercase, full uppercase etc.) Possible
     * values: `Capitalize`, `Uppercase`, `Lowercase`, `Free`
     */
    enum class IdentifierStyle : int { CAPITALIZE, FREE, LOWERCASE, UPPERCASE };

    /**
     * "Image export" option when saving project. Possible values: `None`, `OneImagePerLayer`,
     * `OneImagePerLevel`, `LayersAndLevels`
     */
    enum class ImageExportMode : int { LAYERS_AND_LEVELS, NONE, ONE_IMAGE_PER_LAYER, ONE_IMAGE_PER_LEVEL };

    /**
     * This file is a JSON schema of files created by LDtk level editor (https://ldtk.io).
     *
     * This is the root of any Project JSON file. It contains:  - the project settings, - an
     * array of levels, - a group of definitions (that can probably be safely ignored for most
     * users).
     */
    struct Project {
        /**
         * This object is not actually used by LDtk. It ONLY exists to force explicit references to
         * all types, to make sure QuickType finds them and integrate all of them. Otherwise,
         * Quicktype will drop types that are not explicitely used.
         */
        std::optional<ForcedRefs> forcedRefs;
        /**
         * LDtk application build identifier.<br/>  This is only used to identify the LDtk version
         * that generated this particular project file, which can be useful for specific bug fixing.
         * Note that the build identifier is just the date of the release, so it's not unique to
         * each user (one single global ID per LDtk public release), and as a result, completely
         * anonymous.
         */
        double appBuildID;
        /**
         * Number of backup files to keep, if the `backupOnSave` is TRUE
         */
        int64_t backupLimit;
        /**
         * If TRUE, an extra copy of the project will be created in a sub folder, when saving.
         */
        bool backupOnSave;
        /**
         * Target relative path to store backup files
         */
        std::optional<std::string> backupRelPath;
        /**
         * Project background color
         */
        std::string bgColor;
        /**
         * An array of command lines that can be ran manually by the user
         */
        std::vector<LdtkCustomCommand> customCommands;
        /**
         * Default height for new entities
         */
        int64_t defaultEntityHeight;
        /**
         * Default width for new entities
         */
        int64_t defaultEntityWidth;
        /**
         * Default grid size for new layers
         */
        int64_t defaultGridSize;
        /**
         * Default background color of levels
         */
        std::string defaultLevelBgColor;
        /**
         * **WARNING**: this field will move to the `worlds` array after the "multi-worlds" update.
         * It will then be `null`. You can enable the Multi-worlds advanced project option to enable
         * the change immediately.<br/><br/>  Default new level height
         */
        std::optional<int64_t> defaultLevelHeight;
        /**
         * **WARNING**: this field will move to the `worlds` array after the "multi-worlds" update.
         * It will then be `null`. You can enable the Multi-worlds advanced project option to enable
         * the change immediately.<br/><br/>  Default new level width
         */
        std::optional<int64_t> defaultLevelWidth;
        /**
         * Default X pivot (0 to 1) for new entities
         */
        double defaultPivotX;
        /**
         * Default Y pivot (0 to 1) for new entities
         */
        double defaultPivotY;
        /**
         * A structure containing all the definitions of this project
         */
        Definitions defs;
        /**
         * If the project isn't in MultiWorlds mode, this is the IID of the internal "dummy" World.
         */
        std::string dummyWorldIid;
        /**
         * If TRUE, the exported PNGs will include the level background (color or image).
         */
        bool exportLevelBg;
        /**
         * **WARNING**: this deprecated value is no longer exported since version 0.9.3  Replaced
         * by: `imageExportMode`
         */
        std::optional<bool> exportPNG;
        /**
         * If TRUE, a Tiled compatible file will also be generated along with the LDtk JSON file
         * (default is FALSE)
         */
        bool exportTiled;
        /**
         * If TRUE, one file will be saved for the project (incl. all its definitions) and one file
         * in a sub-folder for each level.
         */
        bool externalLevels;
        /**
         * An array containing various advanced flags (ie. options or other states). Possible
         * values: `DiscardPreCsvIntGrid`, `ExportPreCsvIntGridFormat`, `IgnoreBackupSuggest`,
         * `PrependIndexToLevelFileNames`, `MultiWorlds`, `UseMultilinesType`
         */
        std::vector<Flag> flags;
        /**
         * Naming convention for Identifiers (first-letter uppercase, full uppercase etc.) Possible
         * values: `Capitalize`, `Uppercase`, `Lowercase`, `Free`
         */
        IdentifierStyle identifierStyle;
        /**
         * Unique project identifier
         */
        std::string iid;
        /**
         * "Image export" option when saving project. Possible values: `None`, `OneImagePerLayer`,
         * `OneImagePerLevel`, `LayersAndLevels`
         */
        ImageExportMode imageExportMode;
        /**
         * File format version
         */
        std::string jsonVersion;
        /**
         * The default naming convention for level identifiers.
         */
        std::string levelNamePattern;
        /**
         * All levels. The order of this array is only relevant in `LinearHorizontal` and
         * `linearVertical` world layouts (see `worldLayout` value).<br/>  Otherwise, you should
         * refer to the `worldX`,`worldY` coordinates of each Level.
         */
        std::vector<Level> levels;
        /**
         * If TRUE, the Json is partially minified (no indentation, nor line breaks, default is
         * FALSE)
         */
        bool minifyJSON;
        /**
         * Next Unique integer ID available
         */
        int64_t nextUid;
        /**
         * File naming pattern for exported PNGs
         */
        std::optional<std::string> pngFilePattern;
        /**
         * If TRUE, a very simplified will be generated on saving, for quicker & easier engine
         * integration.
         */
        bool simplifiedExport;
        /**
         * All instances of entities that have their `exportToToc` flag enabled are listed in this
         * array.
         */
        std::vector<LdtkTableOfContentEntry> toc;
        /**
         * This optional description is used by LDtk Samples to show up some informations and
         * instructions.
         */
        std::optional<std::string> tutorialDesc;
        /**
         * **WARNING**: this field will move to the `worlds` array after the "multi-worlds" update.
         * It will then be `null`. You can enable the Multi-worlds advanced project option to enable
         * the change immediately.<br/><br/>  Height of the world grid in pixels.
         */
        std::optional<int64_t> worldGridHeight;
        /**
         * **WARNING**: this field will move to the `worlds` array after the "multi-worlds" update.
         * It will then be `null`. You can enable the Multi-worlds advanced project option to enable
         * the change immediately.<br/><br/>  Width of the world grid in pixels.
         */
        std::optional<int64_t> worldGridWidth;
        /**
         * **WARNING**: this field will move to the `worlds` array after the "multi-worlds" update.
         * It will then be `null`. You can enable the Multi-worlds advanced project option to enable
         * the change immediately.<br/><br/>  An enum that describes how levels are organized in
         * this project (ie. linearly or in a 2D space). Possible values: &lt;`null`&gt;, `Free`,
         * `GridVania`, `LinearHorizontal`, `LinearVertical`
         */
        std::optional<WorldLayout> worldLayout;
        /**
         * This array will be empty, unless you enable the Multi-Worlds in the project advanced
         * settings.<br/><br/> - in current version, a LDtk project file can only contain a single
         * world with multiple levels in it. In this case, levels and world layout related settings
         * are stored in the root of the JSON.<br/> - with "Multi-worlds" enabled, there will be a
         * `worlds` array in root, each world containing levels and layout settings. Basically, it's
         * pretty much only about moving the `levels` array to the `worlds` array, along with world
         * layout related values (eg. `worldGridWidth` etc).<br/><br/>If you want to start
         * supporting this future update easily, please refer to this documentation:
         * https://github.com/deepnight/ldtk/issues/231
         */
        std::vector<World> worlds;
    };
}

namespace ldtk {
    void from_json(const json & j, LdtkCustomCommand & x);
    void to_json(json & j, const LdtkCustomCommand & x);

    void from_json(const json & j, FieldDefinition & x);
    void to_json(json & j, const FieldDefinition & x);

    void from_json(const json & j, TilesetRectangle & x);
    void to_json(json & j, const TilesetRectangle & x);

    void from_json(const json & j, EntityDefinition & x);
    void to_json(json & j, const EntityDefinition & x);

    void from_json(const json & j, EnumValueDefinition & x);
    void to_json(json & j, const EnumValueDefinition & x);

    void from_json(const json & j, EnumDefinition & x);
    void to_json(json & j, const EnumDefinition & x);

    void from_json(const json & j, AutoLayerRuleDefinition & x);
    void to_json(json & j, const AutoLayerRuleDefinition & x);

    void from_json(const json & j, AutoLayerRuleGroup & x);
    void to_json(json & j, const AutoLayerRuleGroup & x);

    void from_json(const json & j, IntGridValueDefinition & x);
    void to_json(json & j, const IntGridValueDefinition & x);

    void from_json(const json & j, IntGridValueGroupDefinition & x);
    void to_json(json & j, const IntGridValueGroupDefinition & x);

    void from_json(const json & j, LayerDefinition & x);
    void to_json(json & j, const LayerDefinition & x);

    void from_json(const json & j, TileCustomMetadata & x);
    void to_json(json & j, const TileCustomMetadata & x);

    void from_json(const json & j, EnumTagValue & x);
    void to_json(json & j, const EnumTagValue & x);

    void from_json(const json & j, TilesetDefinition & x);
    void to_json(json & j, const TilesetDefinition & x);

    void from_json(const json & j, Definitions & x);
    void to_json(json & j, const Definitions & x);

    void from_json(const json & j, FieldInstance & x);
    void to_json(json & j, const FieldInstance & x);

    void from_json(const json & j, EntityInstance & x);
    void to_json(json & j, const EntityInstance & x);

    void from_json(const json & j, ReferenceToAnEntityInstance & x);
    void to_json(json & j, const ReferenceToAnEntityInstance & x);

    void from_json(const json & j, GridPoint & x);
    void to_json(json & j, const GridPoint & x);

    void from_json(const json & j, IntGridValueInstance & x);
    void to_json(json & j, const IntGridValueInstance & x);

    void from_json(const json & j, TileInstance & x);
    void to_json(json & j, const TileInstance & x);

    void from_json(const json & j, LayerInstance & x);
    void to_json(json & j, const LayerInstance & x);

    void from_json(const json & j, LevelBackgroundPosition & x);
    void to_json(json & j, const LevelBackgroundPosition & x);

    void from_json(const json & j, NeighbourLevel & x);
    void to_json(json & j, const NeighbourLevel & x);

    void from_json(const json & j, Level & x);
    void to_json(json & j, const Level & x);

    void from_json(const json & j, LdtkTableOfContentEntry & x);
    void to_json(json & j, const LdtkTableOfContentEntry & x);

    void from_json(const json & j, World & x);
    void to_json(json & j, const World & x);

    void from_json(const json & j, ForcedRefs & x);
    void to_json(json & j, const ForcedRefs & x);

    void from_json(const json & j, Project & x);
    void to_json(json & j, const Project & x);

    void from_json(const json & j, When & x);
    void to_json(json & j, const When & x);

    void from_json(const json & j, AllowedRefs & x);
    void to_json(json & j, const AllowedRefs & x);

    void from_json(const json & j, EditorDisplayMode & x);
    void to_json(json & j, const EditorDisplayMode & x);

    void from_json(const json & j, EditorDisplayPos & x);
    void to_json(json & j, const EditorDisplayPos & x);

    void from_json(const json & j, EditorLinkStyle & x);
    void to_json(json & j, const EditorLinkStyle & x);

    void from_json(const json & j, TextLanguageMode & x);
    void to_json(json & j, const TextLanguageMode & x);

    void from_json(const json & j, LimitBehavior & x);
    void to_json(json & j, const LimitBehavior & x);

    void from_json(const json & j, LimitScope & x);
    void to_json(json & j, const LimitScope & x);

    void from_json(const json & j, RenderMode & x);
    void to_json(json & j, const RenderMode & x);

    void from_json(const json & j, TileRenderMode & x);
    void to_json(json & j, const TileRenderMode & x);

    void from_json(const json & j, Checker & x);
    void to_json(json & j, const Checker & x);

    void from_json(const json & j, TileMode & x);
    void to_json(json & j, const TileMode & x);

    void from_json(const json & j, Type & x);
    void to_json(json & j, const Type & x);

    void from_json(const json & j, EmbedAtlas & x);
    void to_json(json & j, const EmbedAtlas & x);

    void from_json(const json & j, Flag & x);
    void to_json(json & j, const Flag & x);

    void from_json(const json & j, BgPos & x);
    void to_json(json & j, const BgPos & x);

    void from_json(const json & j, WorldLayout & x);
    void to_json(json & j, const WorldLayout & x);

    void from_json(const json & j, IdentifierStyle & x);
    void to_json(json & j, const IdentifierStyle & x);

    void from_json(const json & j, ImageExportMode & x);
    void to_json(json & j, const ImageExportMode & x);

    inline void from_json(const json & j, LdtkCustomCommand& x) {
        x.command = j.at("command").get<std::string>();
        x.when = j.at("when").get<When>();
    }

    inline void to_json(json & j, const LdtkCustomCommand & x) {
        j = json::object();
        j["command"] = x.command;
        j["when"] = x.when;
    }

    inline void from_json(const json & j, FieldDefinition& x) {
        x.type = j.at("__type").get<std::string>();
        x.acceptFileTypes = get_stack_optional<std::vector<std::string>>(j, "acceptFileTypes");
        x.allowedRefs = j.at("allowedRefs").get<AllowedRefs>();
        x.allowedRefsEntityUid = get_stack_optional<int64_t>(j, "allowedRefsEntityUid");
        x.allowedRefTags = j.at("allowedRefTags").get<std::vector<std::string>>();
        x.allowOutOfLevelRef = j.at("allowOutOfLevelRef").get<bool>();
        x.arrayMaxLength = get_stack_optional<int64_t>(j, "arrayMaxLength");
        x.arrayMinLength = get_stack_optional<int64_t>(j, "arrayMinLength");
        x.autoChainRef = j.at("autoChainRef").get<bool>();
        x.canBeNull = j.at("canBeNull").get<bool>();
        x.defaultOverride = get_untyped(j, "defaultOverride");
        x.doc = get_stack_optional<std::string>(j, "doc");
        x.editorAlwaysShow = j.at("editorAlwaysShow").get<bool>();
        x.editorCutLongValues = j.at("editorCutLongValues").get<bool>();
        x.editorDisplayColor = get_stack_optional<std::string>(j, "editorDisplayColor");
        x.editorDisplayMode = j.at("editorDisplayMode").get<EditorDisplayMode>();
        x.editorDisplayPos = j.at("editorDisplayPos").get<EditorDisplayPos>();
        x.editorDisplayScale = j.at("editorDisplayScale").get<double>();
        x.editorLinkStyle = j.at("editorLinkStyle").get<EditorLinkStyle>();
        x.editorShowInWorld = j.at("editorShowInWorld").get<bool>();
        x.editorTextPrefix = get_stack_optional<std::string>(j, "editorTextPrefix");
        x.editorTextSuffix = get_stack_optional<std::string>(j, "editorTextSuffix");
        x.identifier = j.at("identifier").get<std::string>();
        x.isArray = j.at("isArray").get<bool>();
        x.max = get_stack_optional<double>(j, "max");
        x.min = get_stack_optional<double>(j, "min");
        x.regex = get_stack_optional<std::string>(j, "regex");
        x.symmetricalRef = j.at("symmetricalRef").get<bool>();
        x.textLanguageMode = get_stack_optional<TextLanguageMode>(j, "textLanguageMode");
        x.tilesetUid = get_stack_optional<int64_t>(j, "tilesetUid");
        x.fieldDefinitionType = j.at("type").get<std::string>();
        x.uid = j.at("uid").get<int64_t>();
        x.useForSmartColor = j.at("useForSmartColor").get<bool>();
    }

    inline void to_json(json & j, const FieldDefinition & x) {
        j = json::object();
        j["__type"] = x.type;
        j["acceptFileTypes"] = x.acceptFileTypes;
        j["allowedRefs"] = x.allowedRefs;
        j["allowedRefsEntityUid"] = x.allowedRefsEntityUid;
        j["allowedRefTags"] = x.allowedRefTags;
        j["allowOutOfLevelRef"] = x.allowOutOfLevelRef;
        j["arrayMaxLength"] = x.arrayMaxLength;
        j["arrayMinLength"] = x.arrayMinLength;
        j["autoChainRef"] = x.autoChainRef;
        j["canBeNull"] = x.canBeNull;
        j["defaultOverride"] = x.defaultOverride;
        j["doc"] = x.doc;
        j["editorAlwaysShow"] = x.editorAlwaysShow;
        j["editorCutLongValues"] = x.editorCutLongValues;
        j["editorDisplayColor"] = x.editorDisplayColor;
        j["editorDisplayMode"] = x.editorDisplayMode;
        j["editorDisplayPos"] = x.editorDisplayPos;
        j["editorDisplayScale"] = x.editorDisplayScale;
        j["editorLinkStyle"] = x.editorLinkStyle;
        j["editorShowInWorld"] = x.editorShowInWorld;
        j["editorTextPrefix"] = x.editorTextPrefix;
        j["editorTextSuffix"] = x.editorTextSuffix;
        j["identifier"] = x.identifier;
        j["isArray"] = x.isArray;
        j["max"] = x.max;
        j["min"] = x.min;
        j["regex"] = x.regex;
        j["symmetricalRef"] = x.symmetricalRef;
        j["textLanguageMode"] = x.textLanguageMode;
        j["tilesetUid"] = x.tilesetUid;
        j["type"] = x.fieldDefinitionType;
        j["uid"] = x.uid;
        j["useForSmartColor"] = x.useForSmartColor;
    }

    inline void from_json(const json & j, TilesetRectangle& x) {
        x.h = j.at("h").get<int64_t>();
        x.tilesetUid = j.at("tilesetUid").get<int64_t>();
        x.w = j.at("w").get<int64_t>();
        x.x = j.at("x").get<int64_t>();
        x.y = j.at("y").get<int64_t>();
    }

    inline void to_json(json & j, const TilesetRectangle & x) {
        j = json::object();
        j["h"] = x.h;
        j["tilesetUid"] = x.tilesetUid;
        j["w"] = x.w;
        j["x"] = x.x;
        j["y"] = x.y;
    }

    inline void from_json(const json & j, EntityDefinition& x) {
        x.color = j.at("color").get<std::string>();
        x.doc = get_stack_optional<std::string>(j, "doc");
        x.exportToToc = j.at("exportToToc").get<bool>();
        x.fieldDefs = j.at("fieldDefs").get<std::vector<FieldDefinition>>();
        x.fillOpacity = j.at("fillOpacity").get<double>();
        x.height = j.at("height").get<int64_t>();
        x.hollow = j.at("hollow").get<bool>();
        x.identifier = j.at("identifier").get<std::string>();
        x.keepAspectRatio = j.at("keepAspectRatio").get<bool>();
        x.limitBehavior = j.at("limitBehavior").get<LimitBehavior>();
        x.limitScope = j.at("limitScope").get<LimitScope>();
        x.lineOpacity = j.at("lineOpacity").get<double>();
        x.maxCount = j.at("maxCount").get<int64_t>();
        x.maxHeight = get_stack_optional<int64_t>(j, "maxHeight");
        x.maxWidth = get_stack_optional<int64_t>(j, "maxWidth");
        x.minHeight = get_stack_optional<int64_t>(j, "minHeight");
        x.minWidth = get_stack_optional<int64_t>(j, "minWidth");
        x.nineSliceBorders = j.at("nineSliceBorders").get<std::vector<int64_t>>();
        x.pivotX = j.at("pivotX").get<double>();
        x.pivotY = j.at("pivotY").get<double>();
        x.renderMode = j.at("renderMode").get<RenderMode>();
        x.resizableX = j.at("resizableX").get<bool>();
        x.resizableY = j.at("resizableY").get<bool>();
        x.showName = j.at("showName").get<bool>();
        x.tags = j.at("tags").get<std::vector<std::string>>();
        x.tileID = get_stack_optional<int64_t>(j, "tileId");
        x.tileOpacity = j.at("tileOpacity").get<double>();
        x.tileRect = get_stack_optional<TilesetRectangle>(j, "tileRect");
        x.tileRenderMode = j.at("tileRenderMode").get<TileRenderMode>();
        x.tilesetID = get_stack_optional<int64_t>(j, "tilesetId");
        x.uid = j.at("uid").get<int64_t>();
        x.uiTileRect = get_stack_optional<TilesetRectangle>(j, "uiTileRect");
        x.width = j.at("width").get<int64_t>();
    }

    inline void to_json(json & j, const EntityDefinition & x) {
        j = json::object();
        j["color"] = x.color;
        j["doc"] = x.doc;
        j["exportToToc"] = x.exportToToc;
        j["fieldDefs"] = x.fieldDefs;
        j["fillOpacity"] = x.fillOpacity;
        j["height"] = x.height;
        j["hollow"] = x.hollow;
        j["identifier"] = x.identifier;
        j["keepAspectRatio"] = x.keepAspectRatio;
        j["limitBehavior"] = x.limitBehavior;
        j["limitScope"] = x.limitScope;
        j["lineOpacity"] = x.lineOpacity;
        j["maxCount"] = x.maxCount;
        j["maxHeight"] = x.maxHeight;
        j["maxWidth"] = x.maxWidth;
        j["minHeight"] = x.minHeight;
        j["minWidth"] = x.minWidth;
        j["nineSliceBorders"] = x.nineSliceBorders;
        j["pivotX"] = x.pivotX;
        j["pivotY"] = x.pivotY;
        j["renderMode"] = x.renderMode;
        j["resizableX"] = x.resizableX;
        j["resizableY"] = x.resizableY;
        j["showName"] = x.showName;
        j["tags"] = x.tags;
        j["tileId"] = x.tileID;
        j["tileOpacity"] = x.tileOpacity;
        j["tileRect"] = x.tileRect;
        j["tileRenderMode"] = x.tileRenderMode;
        j["tilesetId"] = x.tilesetID;
        j["uid"] = x.uid;
        j["uiTileRect"] = x.uiTileRect;
        j["width"] = x.width;
    }

    inline void from_json(const json & j, EnumValueDefinition& x) {
        x.tileSrcRect = get_stack_optional<std::vector<int64_t>>(j, "__tileSrcRect");
        x.color = j.at("color").get<int64_t>();
        x.id = j.at("id").get<std::string>();
        x.tileID = get_stack_optional<int64_t>(j, "tileId");
        x.tileRect = get_stack_optional<TilesetRectangle>(j, "tileRect");
    }

    inline void to_json(json & j, const EnumValueDefinition & x) {
        j = json::object();
        j["__tileSrcRect"] = x.tileSrcRect;
        j["color"] = x.color;
        j["id"] = x.id;
        j["tileId"] = x.tileID;
        j["tileRect"] = x.tileRect;
    }

    inline void from_json(const json & j, EnumDefinition& x) {
        x.externalFileChecksum = get_stack_optional<std::string>(j, "externalFileChecksum");
        x.externalRelPath = get_stack_optional<std::string>(j, "externalRelPath");
        x.iconTilesetUid = get_stack_optional<int64_t>(j, "iconTilesetUid");
        x.identifier = j.at("identifier").get<std::string>();
        x.tags = j.at("tags").get<std::vector<std::string>>();
        x.uid = j.at("uid").get<int64_t>();
        x.values = j.at("values").get<std::vector<EnumValueDefinition>>();
    }

    inline void to_json(json & j, const EnumDefinition & x) {
        j = json::object();
        j["externalFileChecksum"] = x.externalFileChecksum;
        j["externalRelPath"] = x.externalRelPath;
        j["iconTilesetUid"] = x.iconTilesetUid;
        j["identifier"] = x.identifier;
        j["tags"] = x.tags;
        j["uid"] = x.uid;
        j["values"] = x.values;
    }

    inline void from_json(const json & j, AutoLayerRuleDefinition& x) {
        x.active = j.at("active").get<bool>();
        x.alpha = j.at("alpha").get<double>();
        x.breakOnMatch = j.at("breakOnMatch").get<bool>();
        x.chance = j.at("chance").get<double>();
        x.checker = j.at("checker").get<Checker>();
        x.flipX = j.at("flipX").get<bool>();
        x.flipY = j.at("flipY").get<bool>();
        x.outOfBoundsValue = get_stack_optional<int64_t>(j, "outOfBoundsValue");
        x.pattern = j.at("pattern").get<std::vector<int64_t>>();
        x.perlinActive = j.at("perlinActive").get<bool>();
        x.perlinOctaves = j.at("perlinOctaves").get<double>();
        x.perlinScale = j.at("perlinScale").get<double>();
        x.perlinSeed = j.at("perlinSeed").get<double>();
        x.pivotX = j.at("pivotX").get<double>();
        x.pivotY = j.at("pivotY").get<double>();
        x.size = j.at("size").get<int64_t>();
        x.tileIDS = j.at("tileIds").get<std::vector<int64_t>>();
        x.tileMode = j.at("tileMode").get<TileMode>();
        x.tileRandomXMax = j.at("tileRandomXMax").get<int64_t>();
        x.tileRandomXMin = j.at("tileRandomXMin").get<int64_t>();
        x.tileRandomYMax = j.at("tileRandomYMax").get<int64_t>();
        x.tileRandomYMin = j.at("tileRandomYMin").get<int64_t>();
        x.tileXOffset = j.at("tileXOffset").get<int64_t>();
        x.tileYOffset = j.at("tileYOffset").get<int64_t>();
        x.uid = j.at("uid").get<int64_t>();
        x.xModulo = j.at("xModulo").get<int64_t>();
        x.xOffset = j.at("xOffset").get<int64_t>();
        x.yModulo = j.at("yModulo").get<int64_t>();
        x.yOffset = j.at("yOffset").get<int64_t>();
    }

    inline void to_json(json & j, const AutoLayerRuleDefinition & x) {
        j = json::object();
        j["active"] = x.active;
        j["alpha"] = x.alpha;
        j["breakOnMatch"] = x.breakOnMatch;
        j["chance"] = x.chance;
        j["checker"] = x.checker;
        j["flipX"] = x.flipX;
        j["flipY"] = x.flipY;
        j["outOfBoundsValue"] = x.outOfBoundsValue;
        j["pattern"] = x.pattern;
        j["perlinActive"] = x.perlinActive;
        j["perlinOctaves"] = x.perlinOctaves;
        j["perlinScale"] = x.perlinScale;
        j["perlinSeed"] = x.perlinSeed;
        j["pivotX"] = x.pivotX;
        j["pivotY"] = x.pivotY;
        j["size"] = x.size;
        j["tileIds"] = x.tileIDS;
        j["tileMode"] = x.tileMode;
        j["tileRandomXMax"] = x.tileRandomXMax;
        j["tileRandomXMin"] = x.tileRandomXMin;
        j["tileRandomYMax"] = x.tileRandomYMax;
        j["tileRandomYMin"] = x.tileRandomYMin;
        j["tileXOffset"] = x.tileXOffset;
        j["tileYOffset"] = x.tileYOffset;
        j["uid"] = x.uid;
        j["xModulo"] = x.xModulo;
        j["xOffset"] = x.xOffset;
        j["yModulo"] = x.yModulo;
        j["yOffset"] = x.yOffset;
    }

    inline void from_json(const json & j, AutoLayerRuleGroup& x) {
        x.active = j.at("active").get<bool>();
        x.collapsed = get_stack_optional<bool>(j, "collapsed");
        x.color = get_stack_optional<std::string>(j, "color");
        x.icon = get_stack_optional<TilesetRectangle>(j, "icon");
        x.isOptional = j.at("isOptional").get<bool>();
        x.name = j.at("name").get<std::string>();
        x.rules = j.at("rules").get<std::vector<AutoLayerRuleDefinition>>();
        x.uid = j.at("uid").get<int64_t>();
        x.usesWizard = j.at("usesWizard").get<bool>();
    }

    inline void to_json(json & j, const AutoLayerRuleGroup & x) {
        j = json::object();
        j["active"] = x.active;
        j["collapsed"] = x.collapsed;
        j["color"] = x.color;
        j["icon"] = x.icon;
        j["isOptional"] = x.isOptional;
        j["name"] = x.name;
        j["rules"] = x.rules;
        j["uid"] = x.uid;
        j["usesWizard"] = x.usesWizard;
    }

    inline void from_json(const json & j, IntGridValueDefinition& x) {
        x.color = j.at("color").get<std::string>();
        x.groupUid = j.at("groupUid").get<int64_t>();
        x.identifier = get_stack_optional<std::string>(j, "identifier");
        x.tile = get_stack_optional<TilesetRectangle>(j, "tile");
        x.value = j.at("value").get<int64_t>();
    }

    inline void to_json(json & j, const IntGridValueDefinition & x) {
        j = json::object();
        j["color"] = x.color;
        j["groupUid"] = x.groupUid;
        j["identifier"] = x.identifier;
        j["tile"] = x.tile;
        j["value"] = x.value;
    }

    inline void from_json(const json & j, IntGridValueGroupDefinition& x) {
        x.color = get_stack_optional<std::string>(j, "color");
        x.identifier = get_stack_optional<std::string>(j, "identifier");
        x.uid = j.at("uid").get<int64_t>();
    }

    inline void to_json(json & j, const IntGridValueGroupDefinition & x) {
        j = json::object();
        j["color"] = x.color;
        j["identifier"] = x.identifier;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, LayerDefinition& x) {
        x.type = j.at("__type").get<std::string>();
        x.autoRuleGroups = j.at("autoRuleGroups").get<std::vector<AutoLayerRuleGroup>>();
        x.autoSourceLayerDefUid = get_stack_optional<int64_t>(j, "autoSourceLayerDefUid");
        x.autoTilesetDefUid = get_stack_optional<int64_t>(j, "autoTilesetDefUid");
        x.canSelectWhenInactive = j.at("canSelectWhenInactive").get<bool>();
        x.displayOpacity = j.at("displayOpacity").get<double>();
        x.doc = get_stack_optional<std::string>(j, "doc");
        x.excludedTags = j.at("excludedTags").get<std::vector<std::string>>();
        x.gridSize = j.at("gridSize").get<int64_t>();
        x.guideGridHei = j.at("guideGridHei").get<int64_t>();
        x.guideGridWid = j.at("guideGridWid").get<int64_t>();
        x.hideFieldsWhenInactive = j.at("hideFieldsWhenInactive").get<bool>();
        x.hideInList = j.at("hideInList").get<bool>();
        x.identifier = j.at("identifier").get<std::string>();
        x.inactiveOpacity = j.at("inactiveOpacity").get<double>();
        x.intGridValues = j.at("intGridValues").get<std::vector<IntGridValueDefinition>>();
        x.intGridValuesGroups = j.at("intGridValuesGroups").get<std::vector<IntGridValueGroupDefinition>>();
        x.parallaxFactorX = j.at("parallaxFactorX").get<double>();
        x.parallaxFactorY = j.at("parallaxFactorY").get<double>();
        x.parallaxScaling = j.at("parallaxScaling").get<bool>();
        x.pxOffsetX = j.at("pxOffsetX").get<int64_t>();
        x.pxOffsetY = j.at("pxOffsetY").get<int64_t>();
        x.renderInWorldView = j.at("renderInWorldView").get<bool>();
        x.requiredTags = j.at("requiredTags").get<std::vector<std::string>>();
        x.tilePivotX = j.at("tilePivotX").get<double>();
        x.tilePivotY = j.at("tilePivotY").get<double>();
        x.tilesetDefUid = get_stack_optional<int64_t>(j, "tilesetDefUid");
        x.layerDefinitionType = j.at("type").get<Type>();
        x.uiColor = get_stack_optional<std::string>(j, "uiColor");
        x.uid = j.at("uid").get<int64_t>();
    }

    inline void to_json(json & j, const LayerDefinition & x) {
        j = json::object();
        j["__type"] = x.type;
        j["autoRuleGroups"] = x.autoRuleGroups;
        j["autoSourceLayerDefUid"] = x.autoSourceLayerDefUid;
        j["autoTilesetDefUid"] = x.autoTilesetDefUid;
        j["canSelectWhenInactive"] = x.canSelectWhenInactive;
        j["displayOpacity"] = x.displayOpacity;
        j["doc"] = x.doc;
        j["excludedTags"] = x.excludedTags;
        j["gridSize"] = x.gridSize;
        j["guideGridHei"] = x.guideGridHei;
        j["guideGridWid"] = x.guideGridWid;
        j["hideFieldsWhenInactive"] = x.hideFieldsWhenInactive;
        j["hideInList"] = x.hideInList;
        j["identifier"] = x.identifier;
        j["inactiveOpacity"] = x.inactiveOpacity;
        j["intGridValues"] = x.intGridValues;
        j["intGridValuesGroups"] = x.intGridValuesGroups;
        j["parallaxFactorX"] = x.parallaxFactorX;
        j["parallaxFactorY"] = x.parallaxFactorY;
        j["parallaxScaling"] = x.parallaxScaling;
        j["pxOffsetX"] = x.pxOffsetX;
        j["pxOffsetY"] = x.pxOffsetY;
        j["renderInWorldView"] = x.renderInWorldView;
        j["requiredTags"] = x.requiredTags;
        j["tilePivotX"] = x.tilePivotX;
        j["tilePivotY"] = x.tilePivotY;
        j["tilesetDefUid"] = x.tilesetDefUid;
        j["type"] = x.layerDefinitionType;
        j["uiColor"] = x.uiColor;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, TileCustomMetadata& x) {
        x.data = j.at("data").get<std::string>();
        x.tileID = j.at("tileId").get<int64_t>();
    }

    inline void to_json(json & j, const TileCustomMetadata & x) {
        j = json::object();
        j["data"] = x.data;
        j["tileId"] = x.tileID;
    }

    inline void from_json(const json & j, EnumTagValue& x) {
        x.enumValueID = j.at("enumValueId").get<std::string>();
        x.tileIDS = j.at("tileIds").get<std::vector<int64_t>>();
    }

    inline void to_json(json & j, const EnumTagValue & x) {
        j = json::object();
        j["enumValueId"] = x.enumValueID;
        j["tileIds"] = x.tileIDS;
    }

    inline void from_json(const json & j, TilesetDefinition& x) {
        x.cHei = j.at("__cHei").get<int64_t>();
        x.cWid = j.at("__cWid").get<int64_t>();
        x.cachedPixelData = get_stack_optional<std::map<std::string, nlohmann::json>>(j, "cachedPixelData");
        x.customData = j.at("customData").get<std::vector<TileCustomMetadata>>();
        x.embedAtlas = get_stack_optional<EmbedAtlas>(j, "embedAtlas");
        x.enumTags = j.at("enumTags").get<std::vector<EnumTagValue>>();
        x.identifier = j.at("identifier").get<std::string>();
        x.padding = j.at("padding").get<int64_t>();
        x.pxHei = j.at("pxHei").get<int64_t>();
        x.pxWid = j.at("pxWid").get<int64_t>();
        x.relPath = get_stack_optional<std::string>(j, "relPath");
        x.savedSelections = j.at("savedSelections").get<std::vector<std::map<std::string, nlohmann::json>>>();
        x.spacing = j.at("spacing").get<int64_t>();
        x.tags = j.at("tags").get<std::vector<std::string>>();
        x.tagsSourceEnumUid = get_stack_optional<int64_t>(j, "tagsSourceEnumUid");
        x.tileGridSize = j.at("tileGridSize").get<int64_t>();
        x.uid = j.at("uid").get<int64_t>();
    }

    inline void to_json(json & j, const TilesetDefinition & x) {
        j = json::object();
        j["__cHei"] = x.cHei;
        j["__cWid"] = x.cWid;
        j["cachedPixelData"] = x.cachedPixelData;
        j["customData"] = x.customData;
        j["embedAtlas"] = x.embedAtlas;
        j["enumTags"] = x.enumTags;
        j["identifier"] = x.identifier;
        j["padding"] = x.padding;
        j["pxHei"] = x.pxHei;
        j["pxWid"] = x.pxWid;
        j["relPath"] = x.relPath;
        j["savedSelections"] = x.savedSelections;
        j["spacing"] = x.spacing;
        j["tags"] = x.tags;
        j["tagsSourceEnumUid"] = x.tagsSourceEnumUid;
        j["tileGridSize"] = x.tileGridSize;
        j["uid"] = x.uid;
    }

    inline void from_json(const json & j, Definitions& x) {
        x.entities = j.at("entities").get<std::vector<EntityDefinition>>();
        x.enums = j.at("enums").get<std::vector<EnumDefinition>>();
        x.externalEnums = j.at("externalEnums").get<std::vector<EnumDefinition>>();
        x.layers = j.at("layers").get<std::vector<LayerDefinition>>();
        x.levelFields = j.at("levelFields").get<std::vector<FieldDefinition>>();
        x.tilesets = j.at("tilesets").get<std::vector<TilesetDefinition>>();
    }

    inline void to_json(json & j, const Definitions & x) {
        j = json::object();
        j["entities"] = x.entities;
        j["enums"] = x.enums;
        j["externalEnums"] = x.externalEnums;
        j["layers"] = x.layers;
        j["levelFields"] = x.levelFields;
        j["tilesets"] = x.tilesets;
    }

    inline void from_json(const json & j, FieldInstance& x) {
        x.identifier = j.at("__identifier").get<std::string>();
        x.tile = get_stack_optional<TilesetRectangle>(j, "__tile");
        x.type = j.at("__type").get<std::string>();
        x.value = get_untyped(j, "__value");
        x.defUid = j.at("defUid").get<int64_t>();
        x.realEditorValues = j.at("realEditorValues").get<std::vector<nlohmann::json>>();
    }

    inline void to_json(json & j, const FieldInstance & x) {
        j = json::object();
        j["__identifier"] = x.identifier;
        j["__tile"] = x.tile;
        j["__type"] = x.type;
        j["__value"] = x.value;
        j["defUid"] = x.defUid;
        j["realEditorValues"] = x.realEditorValues;
    }

    inline void from_json(const json & j, EntityInstance& x) {
        x.grid = j.at("__grid").get<std::vector<int64_t>>();
        x.identifier = j.at("__identifier").get<std::string>();
        x.pivot = j.at("__pivot").get<std::vector<double>>();
        x.smartColor = j.at("__smartColor").get<std::string>();
        x.tags = j.at("__tags").get<std::vector<std::string>>();
        x.tile = get_stack_optional<TilesetRectangle>(j, "__tile");
        x.worldX = j.at("__worldX").get<int64_t>();
        x.worldY = j.at("__worldY").get<int64_t>();
        x.defUid = j.at("defUid").get<int64_t>();
        x.fieldInstances = j.at("fieldInstances").get<std::vector<FieldInstance>>();
        x.height = j.at("height").get<int64_t>();
        x.iid = j.at("iid").get<std::string>();
        x.px = j.at("px").get<std::vector<int64_t>>();
        x.width = j.at("width").get<int64_t>();
    }

    inline void to_json(json & j, const EntityInstance & x) {
        j = json::object();
        j["__grid"] = x.grid;
        j["__identifier"] = x.identifier;
        j["__pivot"] = x.pivot;
        j["__smartColor"] = x.smartColor;
        j["__tags"] = x.tags;
        j["__tile"] = x.tile;
        j["__worldX"] = x.worldX;
        j["__worldY"] = x.worldY;
        j["defUid"] = x.defUid;
        j["fieldInstances"] = x.fieldInstances;
        j["height"] = x.height;
        j["iid"] = x.iid;
        j["px"] = x.px;
        j["width"] = x.width;
    }

    inline void from_json(const json & j, ReferenceToAnEntityInstance& x) {
        x.entityIid = j.at("entityIid").get<std::string>();
        x.layerIid = j.at("layerIid").get<std::string>();
        x.levelIid = j.at("levelIid").get<std::string>();
        x.worldIid = j.at("worldIid").get<std::string>();
    }

    inline void to_json(json & j, const ReferenceToAnEntityInstance & x) {
        j = json::object();
        j["entityIid"] = x.entityIid;
        j["layerIid"] = x.layerIid;
        j["levelIid"] = x.levelIid;
        j["worldIid"] = x.worldIid;
    }

    inline void from_json(const json & j, GridPoint& x) {
        x.cx = j.at("cx").get<int64_t>();
        x.cy = j.at("cy").get<int64_t>();
    }

    inline void to_json(json & j, const GridPoint & x) {
        j = json::object();
        j["cx"] = x.cx;
        j["cy"] = x.cy;
    }

    inline void from_json(const json & j, IntGridValueInstance& x) {
        x.coordID = j.at("coordId").get<int64_t>();
        x.v = j.at("v").get<int64_t>();
    }

    inline void to_json(json & j, const IntGridValueInstance & x) {
        j = json::object();
        j["coordId"] = x.coordID;
        j["v"] = x.v;
    }

    inline void from_json(const json & j, TileInstance& x) {
        x.a = j.at("a").get<double>();
        x.d = j.at("d").get<std::vector<int64_t>>();
        x.f = j.at("f").get<int64_t>();
        x.px = j.at("px").get<std::vector<int64_t>>();
        x.src = j.at("src").get<std::vector<int64_t>>();
        x.t = j.at("t").get<int64_t>();
    }

    inline void to_json(json & j, const TileInstance & x) {
        j = json::object();
        j["a"] = x.a;
        j["d"] = x.d;
        j["f"] = x.f;
        j["px"] = x.px;
        j["src"] = x.src;
        j["t"] = x.t;
    }

    inline void from_json(const json & j, LayerInstance& x) {
        x.cHei = j.at("__cHei").get<int64_t>();
        x.cWid = j.at("__cWid").get<int64_t>();
        x.gridSize = j.at("__gridSize").get<int64_t>();
        x.identifier = j.at("__identifier").get<std::string>();
        x.opacity = j.at("__opacity").get<double>();
        x.pxTotalOffsetX = j.at("__pxTotalOffsetX").get<int64_t>();
        x.pxTotalOffsetY = j.at("__pxTotalOffsetY").get<int64_t>();
        x.tilesetDefUid = get_stack_optional<int64_t>(j, "__tilesetDefUid");
        x.tilesetRelPath = get_stack_optional<std::string>(j, "__tilesetRelPath");
        x.type = j.at("__type").get<std::string>();
        x.autoLayerTiles = j.at("autoLayerTiles").get<std::vector<TileInstance>>();
        x.entityInstances = j.at("entityInstances").get<std::vector<EntityInstance>>();
        x.gridTiles = j.at("gridTiles").get<std::vector<TileInstance>>();
        x.iid = j.at("iid").get<std::string>();
        x.intGrid = get_stack_optional<std::vector<IntGridValueInstance>>(j, "intGrid");
        x.intGridCSV = j.at("intGridCsv").get<std::vector<int64_t>>();
        x.layerDefUid = j.at("layerDefUid").get<int64_t>();
        x.levelID = j.at("levelId").get<int64_t>();
        x.optionalRules = j.at("optionalRules").get<std::vector<int64_t>>();
        x.overrideTilesetUid = get_stack_optional<int64_t>(j, "overrideTilesetUid");
        x.pxOffsetX = j.at("pxOffsetX").get<int64_t>();
        x.pxOffsetY = j.at("pxOffsetY").get<int64_t>();
        x.seed = j.at("seed").get<int64_t>();
        x.visible = j.at("visible").get<bool>();
    }

    inline void to_json(json & j, const LayerInstance & x) {
        j = json::object();
        j["__cHei"] = x.cHei;
        j["__cWid"] = x.cWid;
        j["__gridSize"] = x.gridSize;
        j["__identifier"] = x.identifier;
        j["__opacity"] = x.opacity;
        j["__pxTotalOffsetX"] = x.pxTotalOffsetX;
        j["__pxTotalOffsetY"] = x.pxTotalOffsetY;
        j["__tilesetDefUid"] = x.tilesetDefUid;
        j["__tilesetRelPath"] = x.tilesetRelPath;
        j["__type"] = x.type;
        j["autoLayerTiles"] = x.autoLayerTiles;
        j["entityInstances"] = x.entityInstances;
        j["gridTiles"] = x.gridTiles;
        j["iid"] = x.iid;
        j["intGrid"] = x.intGrid;
        j["intGridCsv"] = x.intGridCSV;
        j["layerDefUid"] = x.layerDefUid;
        j["levelId"] = x.levelID;
        j["optionalRules"] = x.optionalRules;
        j["overrideTilesetUid"] = x.overrideTilesetUid;
        j["pxOffsetX"] = x.pxOffsetX;
        j["pxOffsetY"] = x.pxOffsetY;
        j["seed"] = x.seed;
        j["visible"] = x.visible;
    }

    inline void from_json(const json & j, LevelBackgroundPosition& x) {
        x.cropRect = j.at("cropRect").get<std::vector<double>>();
        x.scale = j.at("scale").get<std::vector<double>>();
        x.topLeftPx = j.at("topLeftPx").get<std::vector<int64_t>>();
    }

    inline void to_json(json & j, const LevelBackgroundPosition & x) {
        j = json::object();
        j["cropRect"] = x.cropRect;
        j["scale"] = x.scale;
        j["topLeftPx"] = x.topLeftPx;
    }

    inline void from_json(const json & j, NeighbourLevel& x) {
        x.dir = j.at("dir").get<std::string>();
        x.levelIid = j.at("levelIid").get<std::string>();
        x.levelUid = get_stack_optional<int64_t>(j, "levelUid");
    }

    inline void to_json(json & j, const NeighbourLevel & x) {
        j = json::object();
        j["dir"] = x.dir;
        j["levelIid"] = x.levelIid;
        j["levelUid"] = x.levelUid;
    }

    inline void from_json(const json & j, Level& x) {
        x.bgColor = j.at("__bgColor").get<std::string>();
        x.bgPos = get_stack_optional<LevelBackgroundPosition>(j, "__bgPos");
        x.neighbours = j.at("__neighbours").get<std::vector<NeighbourLevel>>();
        x.smartColor = j.at("__smartColor").get<std::string>();
        x.levelBgColor = get_stack_optional<std::string>(j, "bgColor");
        x.bgPivotX = j.at("bgPivotX").get<double>();
        x.bgPivotY = j.at("bgPivotY").get<double>();
        x.levelBgPos = get_stack_optional<BgPos>(j, "bgPos");
        x.bgRelPath = get_stack_optional<std::string>(j, "bgRelPath");
        x.externalRelPath = get_stack_optional<std::string>(j, "externalRelPath");
        x.fieldInstances = j.at("fieldInstances").get<std::vector<FieldInstance>>();
        x.identifier = j.at("identifier").get<std::string>();
        x.iid = j.at("iid").get<std::string>();
        x.layerInstances = get_stack_optional<std::vector<LayerInstance>>(j, "layerInstances");
        x.pxHei = j.at("pxHei").get<int64_t>();
        x.pxWid = j.at("pxWid").get<int64_t>();
        x.uid = j.at("uid").get<int64_t>();
        x.useAutoIdentifier = j.at("useAutoIdentifier").get<bool>();
        x.worldDepth = j.at("worldDepth").get<int64_t>();
        x.worldX = j.at("worldX").get<int64_t>();
        x.worldY = j.at("worldY").get<int64_t>();
    }

    inline void to_json(json & j, const Level & x) {
        j = json::object();
        j["__bgColor"] = x.bgColor;
        j["__bgPos"] = x.bgPos;
        j["__neighbours"] = x.neighbours;
        j["__smartColor"] = x.smartColor;
        j["bgColor"] = x.levelBgColor;
        j["bgPivotX"] = x.bgPivotX;
        j["bgPivotY"] = x.bgPivotY;
        j["bgPos"] = x.levelBgPos;
        j["bgRelPath"] = x.bgRelPath;
        j["externalRelPath"] = x.externalRelPath;
        j["fieldInstances"] = x.fieldInstances;
        j["identifier"] = x.identifier;
        j["iid"] = x.iid;
        j["layerInstances"] = x.layerInstances;
        j["pxHei"] = x.pxHei;
        j["pxWid"] = x.pxWid;
        j["uid"] = x.uid;
        j["useAutoIdentifier"] = x.useAutoIdentifier;
        j["worldDepth"] = x.worldDepth;
        j["worldX"] = x.worldX;
        j["worldY"] = x.worldY;
    }

    inline void from_json(const json & j, LdtkTableOfContentEntry& x) {
        x.identifier = j.at("identifier").get<std::string>();
        x.instances = j.at("instances").get<std::vector<ReferenceToAnEntityInstance>>();
    }

    inline void to_json(json & j, const LdtkTableOfContentEntry & x) {
        j = json::object();
        j["identifier"] = x.identifier;
        j["instances"] = x.instances;
    }

    inline void from_json(const json & j, World& x) {
        x.defaultLevelHeight = j.at("defaultLevelHeight").get<int64_t>();
        x.defaultLevelWidth = j.at("defaultLevelWidth").get<int64_t>();
        x.identifier = j.at("identifier").get<std::string>();
        x.iid = j.at("iid").get<std::string>();
        x.levels = j.at("levels").get<std::vector<Level>>();
        x.worldGridHeight = j.at("worldGridHeight").get<int64_t>();
        x.worldGridWidth = j.at("worldGridWidth").get<int64_t>();
        x.worldLayout = get_stack_optional<WorldLayout>(j, "worldLayout");
    }

    inline void to_json(json & j, const World & x) {
        j = json::object();
        j["defaultLevelHeight"] = x.defaultLevelHeight;
        j["defaultLevelWidth"] = x.defaultLevelWidth;
        j["identifier"] = x.identifier;
        j["iid"] = x.iid;
        j["levels"] = x.levels;
        j["worldGridHeight"] = x.worldGridHeight;
        j["worldGridWidth"] = x.worldGridWidth;
        j["worldLayout"] = x.worldLayout;
    }

    inline void from_json(const json & j, ForcedRefs& x) {
        x.autoLayerRuleGroup = get_stack_optional<AutoLayerRuleGroup>(j, "AutoLayerRuleGroup");
        x.autoRuleDef = get_stack_optional<AutoLayerRuleDefinition>(j, "AutoRuleDef");
        x.customCommand = get_stack_optional<LdtkCustomCommand>(j, "CustomCommand");
        x.definitions = get_stack_optional<Definitions>(j, "Definitions");
        x.entityDef = get_stack_optional<EntityDefinition>(j, "EntityDef");
        x.entityInstance = get_stack_optional<EntityInstance>(j, "EntityInstance");
        x.entityReferenceInfos = get_stack_optional<ReferenceToAnEntityInstance>(j, "EntityReferenceInfos");
        x.enumDef = get_stack_optional<EnumDefinition>(j, "EnumDef");
        x.enumDefValues = get_stack_optional<EnumValueDefinition>(j, "EnumDefValues");
        x.enumTagValue = get_stack_optional<EnumTagValue>(j, "EnumTagValue");
        x.fieldDef = get_stack_optional<FieldDefinition>(j, "FieldDef");
        x.fieldInstance = get_stack_optional<FieldInstance>(j, "FieldInstance");
        x.gridPoint = get_stack_optional<GridPoint>(j, "GridPoint");
        x.intGridValueDef = get_stack_optional<IntGridValueDefinition>(j, "IntGridValueDef");
        x.intGridValueGroupDef = get_stack_optional<IntGridValueGroupDefinition>(j, "IntGridValueGroupDef");
        x.intGridValueInstance = get_stack_optional<IntGridValueInstance>(j, "IntGridValueInstance");
        x.layerDef = get_stack_optional<LayerDefinition>(j, "LayerDef");
        x.layerInstance = get_stack_optional<LayerInstance>(j, "LayerInstance");
        x.level = get_stack_optional<Level>(j, "Level");
        x.levelBgPosInfos = get_stack_optional<LevelBackgroundPosition>(j, "LevelBgPosInfos");
        x.neighbourLevel = get_stack_optional<NeighbourLevel>(j, "NeighbourLevel");
        x.tableOfContentEntry = get_stack_optional<LdtkTableOfContentEntry>(j, "TableOfContentEntry");
        x.tile = get_stack_optional<TileInstance>(j, "Tile");
        x.tileCustomMetadata = get_stack_optional<TileCustomMetadata>(j, "TileCustomMetadata");
        x.tilesetDef = get_stack_optional<TilesetDefinition>(j, "TilesetDef");
        x.tilesetRect = get_stack_optional<TilesetRectangle>(j, "TilesetRect");
        x.world = get_stack_optional<World>(j, "World");
    }

    inline void to_json(json & j, const ForcedRefs & x) {
        j = json::object();
        j["AutoLayerRuleGroup"] = x.autoLayerRuleGroup;
        j["AutoRuleDef"] = x.autoRuleDef;
        j["CustomCommand"] = x.customCommand;
        j["Definitions"] = x.definitions;
        j["EntityDef"] = x.entityDef;
        j["EntityInstance"] = x.entityInstance;
        j["EntityReferenceInfos"] = x.entityReferenceInfos;
        j["EnumDef"] = x.enumDef;
        j["EnumDefValues"] = x.enumDefValues;
        j["EnumTagValue"] = x.enumTagValue;
        j["FieldDef"] = x.fieldDef;
        j["FieldInstance"] = x.fieldInstance;
        j["GridPoint"] = x.gridPoint;
        j["IntGridValueDef"] = x.intGridValueDef;
        j["IntGridValueGroupDef"] = x.intGridValueGroupDef;
        j["IntGridValueInstance"] = x.intGridValueInstance;
        j["LayerDef"] = x.layerDef;
        j["LayerInstance"] = x.layerInstance;
        j["Level"] = x.level;
        j["LevelBgPosInfos"] = x.levelBgPosInfos;
        j["NeighbourLevel"] = x.neighbourLevel;
        j["TableOfContentEntry"] = x.tableOfContentEntry;
        j["Tile"] = x.tile;
        j["TileCustomMetadata"] = x.tileCustomMetadata;
        j["TilesetDef"] = x.tilesetDef;
        j["TilesetRect"] = x.tilesetRect;
        j["World"] = x.world;
    }

    inline void from_json(const json & j, Project& x) {
        x.forcedRefs = get_stack_optional<ForcedRefs>(j, "__FORCED_REFS");
        x.appBuildID = j.at("appBuildId").get<double>();
        x.backupLimit = j.at("backupLimit").get<int64_t>();
        x.backupOnSave = j.at("backupOnSave").get<bool>();
        x.backupRelPath = get_stack_optional<std::string>(j, "backupRelPath");
        x.bgColor = j.at("bgColor").get<std::string>();
        x.customCommands = j.at("customCommands").get<std::vector<LdtkCustomCommand>>();
        x.defaultEntityHeight = j.at("defaultEntityHeight").get<int64_t>();
        x.defaultEntityWidth = j.at("defaultEntityWidth").get<int64_t>();
        x.defaultGridSize = j.at("defaultGridSize").get<int64_t>();
        x.defaultLevelBgColor = j.at("defaultLevelBgColor").get<std::string>();
        x.defaultLevelHeight = get_stack_optional<int64_t>(j, "defaultLevelHeight");
        x.defaultLevelWidth = get_stack_optional<int64_t>(j, "defaultLevelWidth");
        x.defaultPivotX = j.at("defaultPivotX").get<double>();
        x.defaultPivotY = j.at("defaultPivotY").get<double>();
        x.defs = j.at("defs").get<Definitions>();
        x.dummyWorldIid = j.at("dummyWorldIid").get<std::string>();
        x.exportLevelBg = j.at("exportLevelBg").get<bool>();
        x.exportPNG = get_stack_optional<bool>(j, "exportPng");
        x.exportTiled = j.at("exportTiled").get<bool>();
        x.externalLevels = j.at("externalLevels").get<bool>();
        x.flags = j.at("flags").get<std::vector<Flag>>();
        x.identifierStyle = j.at("identifierStyle").get<IdentifierStyle>();
        x.iid = j.at("iid").get<std::string>();
        x.imageExportMode = j.at("imageExportMode").get<ImageExportMode>();
        x.jsonVersion = j.at("jsonVersion").get<std::string>();
        x.levelNamePattern = j.at("levelNamePattern").get<std::string>();
        x.levels = j.at("levels").get<std::vector<Level>>();
        x.minifyJSON = j.at("minifyJson").get<bool>();
        x.nextUid = j.at("nextUid").get<int64_t>();
        x.pngFilePattern = get_stack_optional<std::string>(j, "pngFilePattern");
        x.simplifiedExport = j.at("simplifiedExport").get<bool>();
        x.toc = j.at("toc").get<std::vector<LdtkTableOfContentEntry>>();
        x.tutorialDesc = get_stack_optional<std::string>(j, "tutorialDesc");
        x.worldGridHeight = get_stack_optional<int64_t>(j, "worldGridHeight");
        x.worldGridWidth = get_stack_optional<int64_t>(j, "worldGridWidth");
        x.worldLayout = get_stack_optional<WorldLayout>(j, "worldLayout");
        x.worlds = j.at("worlds").get<std::vector<World>>();
    }

    inline void to_json(json & j, const Project & x) {
        j = json::object();
        j["__FORCED_REFS"] = x.forcedRefs;
        j["appBuildId"] = x.appBuildID;
        j["backupLimit"] = x.backupLimit;
        j["backupOnSave"] = x.backupOnSave;
        j["backupRelPath"] = x.backupRelPath;
        j["bgColor"] = x.bgColor;
        j["customCommands"] = x.customCommands;
        j["defaultEntityHeight"] = x.defaultEntityHeight;
        j["defaultEntityWidth"] = x.defaultEntityWidth;
        j["defaultGridSize"] = x.defaultGridSize;
        j["defaultLevelBgColor"] = x.defaultLevelBgColor;
        j["defaultLevelHeight"] = x.defaultLevelHeight;
        j["defaultLevelWidth"] = x.defaultLevelWidth;
        j["defaultPivotX"] = x.defaultPivotX;
        j["defaultPivotY"] = x.defaultPivotY;
        j["defs"] = x.defs;
        j["dummyWorldIid"] = x.dummyWorldIid;
        j["exportLevelBg"] = x.exportLevelBg;
        j["exportPng"] = x.exportPNG;
        j["exportTiled"] = x.exportTiled;
        j["externalLevels"] = x.externalLevels;
        j["flags"] = x.flags;
        j["identifierStyle"] = x.identifierStyle;
        j["iid"] = x.iid;
        j["imageExportMode"] = x.imageExportMode;
        j["jsonVersion"] = x.jsonVersion;
        j["levelNamePattern"] = x.levelNamePattern;
        j["levels"] = x.levels;
        j["minifyJson"] = x.minifyJSON;
        j["nextUid"] = x.nextUid;
        j["pngFilePattern"] = x.pngFilePattern;
        j["simplifiedExport"] = x.simplifiedExport;
        j["toc"] = x.toc;
        j["tutorialDesc"] = x.tutorialDesc;
        j["worldGridHeight"] = x.worldGridHeight;
        j["worldGridWidth"] = x.worldGridWidth;
        j["worldLayout"] = x.worldLayout;
        j["worlds"] = x.worlds;
    }

    inline void from_json(const json & j, When & x) {
        if (j == "AfterLoad") x = When::AFTER_LOAD;
        else if (j == "AfterSave") x = When::AFTER_SAVE;
        else if (j == "BeforeSave") x = When::BEFORE_SAVE;
        else if (j == "Manual") x = When::MANUAL;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const When & x) {
        switch (x) {
            case When::AFTER_LOAD: j = "AfterLoad"; break;
            case When::AFTER_SAVE: j = "AfterSave"; break;
            case When::BEFORE_SAVE: j = "BeforeSave"; break;
            case When::MANUAL: j = "Manual"; break;
            default: throw std::runtime_error("This should not happen");
        }
    }

    inline void from_json(const json & j, AllowedRefs & x) {
        if (j == "Any") x = AllowedRefs::ANY;
        else if (j == "OnlySame") x = AllowedRefs::ONLY_SAME;
        else if (j == "OnlySpecificEntity") x = AllowedRefs::ONLY_SPECIFIC_ENTITY;
        else if (j == "OnlyTags") x = AllowedRefs::ONLY_TAGS;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const AllowedRefs & x) {
        switch (x) {
            case AllowedRefs::ANY: j = "Any"; break;
            case AllowedRefs::ONLY_SAME: j = "OnlySame"; break;
            case AllowedRefs::ONLY_SPECIFIC_ENTITY: j = "OnlySpecificEntity"; break;
            case AllowedRefs::ONLY_TAGS: j = "OnlyTags"; break;
            default: throw std::runtime_error("This should not happen");
        }
    }

    inline void from_json(const json & j, EditorDisplayMode & x) {
        if (j == "ArrayCountNoLabel") x = EditorDisplayMode::ARRAY_COUNT_NO_LABEL;
        else if (j == "ArrayCountWithLabel") x = EditorDisplayMode::ARRAY_COUNT_WITH_LABEL;
        else if (j == "EntityTile") x = EditorDisplayMode::ENTITY_TILE;
        else if (j == "Hidden") x = EditorDisplayMode::HIDDEN;
        else if (j == "LevelTile") x = EditorDisplayMode::LEVEL_TILE;
        else if (j == "NameAndValue") x = EditorDisplayMode::NAME_AND_VALUE;
        else if (j == "Points") x = EditorDisplayMode::POINTS;
        else if (j == "PointPath") x = EditorDisplayMode::POINT_PATH;
        else if (j == "PointPathLoop") x = EditorDisplayMode::POINT_PATH_LOOP;
        else if (j == "PointStar") x = EditorDisplayMode::POINT_STAR;
        else if (j == "RadiusGrid") x = EditorDisplayMode::RADIUS_GRID;
        else if (j == "RadiusPx") x = EditorDisplayMode::RADIUS_PX;
        else if (j == "RefLinkBetweenCenters") x = EditorDisplayMode::REF_LINK_BETWEEN_CENTERS;
        else if (j == "RefLinkBetweenPivots") x = EditorDisplayMode::REF_LINK_BETWEEN_PIVOTS;
        else if (j == "ValueOnly") x = EditorDisplayMode::VALUE_ONLY;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const EditorDisplayMode & x) {
        switch (x) {
            case EditorDisplayMode::ARRAY_COUNT_NO_LABEL: j = "ArrayCountNoLabel"; break;
            case EditorDisplayMode::ARRAY_COUNT_WITH_LABEL: j = "ArrayCountWithLabel"; break;
            case EditorDisplayMode::ENTITY_TILE: j = "EntityTile"; break;
            case EditorDisplayMode::HIDDEN: j = "Hidden"; break;
            case EditorDisplayMode::LEVEL_TILE: j = "LevelTile"; break;
            case EditorDisplayMode::NAME_AND_VALUE: j = "NameAndValue"; break;
            case EditorDisplayMode::POINTS: j = "Points"; break;
            case EditorDisplayMode::POINT_PATH: j = "PointPath"; break;
            case EditorDisplayMode::POINT_PATH_LOOP: j = "PointPathLoop"; break;
            case EditorDisplayMode::POINT_STAR: j = "PointStar"; break;
            case EditorDisplayMode::RADIUS_GRID: j = "RadiusGrid"; break;
            case EditorDisplayMode::RADIUS_PX: j = "RadiusPx"; break;
            case EditorDisplayMode::REF_LINK_BETWEEN_CENTERS: j = "RefLinkBetweenCenters"; break;
            case EditorDisplayMode::REF_LINK_BETWEEN_PIVOTS: j = "RefLinkBetweenPivots"; break;
            case EditorDisplayMode::VALUE_ONLY: j = "ValueOnly"; break;
            default: throw std::runtime_error("This should not happen");
        }
    }

    inline void from_json(const json & j, EditorDisplayPos & x) {
        if (j == "Above") x = EditorDisplayPos::ABOVE;
        else if (j == "Beneath") x = EditorDisplayPos::BENEATH;
        else if (j == "Center") x = EditorDisplayPos::CENTER;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const EditorDisplayPos & x) {
        switch (x) {
            case EditorDisplayPos::ABOVE: j = "Above"; break;
            case EditorDisplayPos::BENEATH: j = "Beneath"; break;
            case EditorDisplayPos::CENTER: j = "Center"; break;
            default: throw std::runtime_error("This should not happen");
        }
    }

    inline void from_json(const json & j, EditorLinkStyle & x) {
        if (j == "ArrowsLine") x = EditorLinkStyle::ARROWS_LINE;
        else if (j == "CurvedArrow") x = EditorLinkStyle::CURVED_ARROW;
        else if (j == "DashedLine") x = EditorLinkStyle::DASHED_LINE;
        else if (j == "StraightArrow") x = EditorLinkStyle::STRAIGHT_ARROW;
        else if (j == "ZigZag") x = EditorLinkStyle::ZIG_ZAG;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const EditorLinkStyle & x) {
        switch (x) {
            case EditorLinkStyle::ARROWS_LINE: j = "ArrowsLine"; break;
            case EditorLinkStyle::CURVED_ARROW: j = "CurvedArrow"; break;
            case EditorLinkStyle::DASHED_LINE: j = "DashedLine"; break;
            case EditorLinkStyle::STRAIGHT_ARROW: j = "StraightArrow"; break;
            case EditorLinkStyle::ZIG_ZAG: j = "ZigZag"; break;
            default: throw std::runtime_error("This should not happen");
        }
    }

    inline void from_json(const json & j, TextLanguageMode & x) {
        if (j == "LangC") x = TextLanguageMode::LANG_C;
        else if (j == "LangHaxe") x = TextLanguageMode::LANG_HAXE;
        else if (j == "LangJS") x = TextLanguageMode::LANG_JS;
        else if (j == "LangJson") x = TextLanguageMode::LANG_JSON;
        else if (j == "LangLog") x = TextLanguageMode::LANG_LOG;
        else if (j == "LangLua") x = TextLanguageMode::LANG_LUA;
        else if (j == "LangMarkdown") x = TextLanguageMode::LANG_MARKDOWN;
        else if (j == "LangPython") x = TextLanguageMode::LANG_PYTHON;
        else if (j == "LangRuby") x = TextLanguageMode::LANG_RUBY;
        else if (j == "LangXml") x = TextLanguageMode::LANG_XML;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const TextLanguageMode & x) {
        switch (x) {
            case TextLanguageMode::LANG_C: j = "LangC"; break;
            case TextLanguageMode::LANG_HAXE: j = "LangHaxe"; break;
            case TextLanguageMode::LANG_JS: j = "LangJS"; break;
            case TextLanguageMode::LANG_JSON: j = "LangJson"; break;
            case TextLanguageMode::LANG_LOG: j = "LangLog"; break;
            case TextLanguageMode::LANG_LUA: j = "LangLua"; break;
            case TextLanguageMode::LANG_MARKDOWN: j = "LangMarkdown"; break;
            case TextLanguageMode::LANG_PYTHON: j = "LangPython"; break;
            case TextLanguageMode::LANG_RUBY: j = "LangRuby"; break;
            case TextLanguageMode::LANG_XML: j = "LangXml"; break;
            default: throw std::runtime_error("This should not happen");
        }
    }

    inline void from_json(const json & j, LimitBehavior & x) {
        if (j == "DiscardOldOnes") x = LimitBehavior::DISCARD_OLD_ONES;
        else if (j == "MoveLastOne") x = LimitBehavior::MOVE_LAST_ONE;
        else if (j == "PreventAdding") x = LimitBehavior::PREVENT_ADDING;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const LimitBehavior & x) {
        switch (x) {
            case LimitBehavior::DISCARD_OLD_ONES: j = "DiscardOldOnes"; break;
            case LimitBehavior::MOVE_LAST_ONE: j = "MoveLastOne"; break;
            case LimitBehavior::PREVENT_ADDING: j = "PreventAdding"; break;
            default: throw std::runtime_error("This should not happen");
        }
    }

    inline void from_json(const json & j, LimitScope & x) {
        if (j == "PerLayer") x = LimitScope::PER_LAYER;
        else if (j == "PerLevel") x = LimitScope::PER_LEVEL;
        else if (j == "PerWorld") x = LimitScope::PER_WORLD;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const LimitScope & x) {
        switch (x) {
            case LimitScope::PER_LAYER: j = "PerLayer"; break;
            case LimitScope::PER_LEVEL: j = "PerLevel"; break;
            case LimitScope::PER_WORLD: j = "PerWorld"; break;
            default: throw std::runtime_error("This should not happen");
        }
    }

    inline void from_json(const json & j, RenderMode & x) {
        if (j == "Cross") x = RenderMode::CROSS;
        else if (j == "Ellipse") x = RenderMode::ELLIPSE;
        else if (j == "Rectangle") x = RenderMode::RECTANGLE;
        else if (j == "Tile") x = RenderMode::TILE;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const RenderMode & x) {
        switch (x) {
            case RenderMode::CROSS: j = "Cross"; break;
            case RenderMode::ELLIPSE: j = "Ellipse"; break;
            case RenderMode::RECTANGLE: j = "Rectangle"; break;
            case RenderMode::TILE: j = "Tile"; break;
            default: throw std::runtime_error("This should not happen");
        }
    }

    inline void from_json(const json & j, TileRenderMode & x) {
        if (j == "Cover") x = TileRenderMode::COVER;
        else if (j == "FitInside") x = TileRenderMode::FIT_INSIDE;
        else if (j == "FullSizeCropped") x = TileRenderMode::FULL_SIZE_CROPPED;
        else if (j == "FullSizeUncropped") x = TileRenderMode::FULL_SIZE_UNCROPPED;
        else if (j == "NineSlice") x = TileRenderMode::NINE_SLICE;
        else if (j == "Repeat") x = TileRenderMode::REPEAT;
        else if (j == "Stretch") x = TileRenderMode::STRETCH;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const TileRenderMode & x) {
        switch (x) {
            case TileRenderMode::COVER: j = "Cover"; break;
            case TileRenderMode::FIT_INSIDE: j = "FitInside"; break;
            case TileRenderMode::FULL_SIZE_CROPPED: j = "FullSizeCropped"; break;
            case TileRenderMode::FULL_SIZE_UNCROPPED: j = "FullSizeUncropped"; break;
            case TileRenderMode::NINE_SLICE: j = "NineSlice"; break;
            case TileRenderMode::REPEAT: j = "Repeat"; break;
            case TileRenderMode::STRETCH: j = "Stretch"; break;
            default: throw std::runtime_error("This should not happen");
        }
    }

    inline void from_json(const json & j, Checker & x) {
        if (j == "Horizontal") x = Checker::HORIZONTAL;
        else if (j == "None") x = Checker::NONE;
        else if (j == "Vertical") x = Checker::VERTICAL;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const Checker & x) {
        switch (x) {
            case Checker::HORIZONTAL: j = "Horizontal"; break;
            case Checker::NONE: j = "None"; break;
            case Checker::VERTICAL: j = "Vertical"; break;
            default: throw std::runtime_error("This should not happen");
        }
    }

    inline void from_json(const json & j, TileMode & x) {
        if (j == "Single") x = TileMode::SINGLE;
        else if (j == "Stamp") x = TileMode::STAMP;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const TileMode & x) {
        switch (x) {
            case TileMode::SINGLE: j = "Single"; break;
            case TileMode::STAMP: j = "Stamp"; break;
            default: throw std::runtime_error("This should not happen");
        }
    }

    inline void from_json(const json & j, Type & x) {
        if (j == "AutoLayer") x = Type::AUTO_LAYER;
        else if (j == "Entities") x = Type::ENTITIES;
        else if (j == "IntGrid") x = Type::INT_GRID;
        else if (j == "Tiles") x = Type::TILES;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const Type & x) {
        switch (x) {
            case Type::AUTO_LAYER: j = "AutoLayer"; break;
            case Type::ENTITIES: j = "Entities"; break;
            case Type::INT_GRID: j = "IntGrid"; break;
            case Type::TILES: j = "Tiles"; break;
            default: throw std::runtime_error("This should not happen");
        }
    }

    inline void from_json(const json & j, EmbedAtlas & x) {
        if (j == "LdtkIcons") x = EmbedAtlas::LDTK_ICONS;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const EmbedAtlas & x) {
        switch (x) {
            case EmbedAtlas::LDTK_ICONS: j = "LdtkIcons"; break;
            default: throw std::runtime_error("This should not happen");
        }
    }

    inline void from_json(const json & j, Flag & x) {
        if (j == "DiscardPreCsvIntGrid") x = Flag::DISCARD_PRE_CSV_INT_GRID;
        else if (j == "ExportPreCsvIntGridFormat") x = Flag::EXPORT_PRE_CSV_INT_GRID_FORMAT;
        else if (j == "IgnoreBackupSuggest") x = Flag::IGNORE_BACKUP_SUGGEST;
        else if (j == "MultiWorlds") x = Flag::MULTI_WORLDS;
        else if (j == "PrependIndexToLevelFileNames") x = Flag::PREPEND_INDEX_TO_LEVEL_FILE_NAMES;
        else if (j == "UseMultilinesType") x = Flag::USE_MULTILINES_TYPE;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const Flag & x) {
        switch (x) {
            case Flag::DISCARD_PRE_CSV_INT_GRID: j = "DiscardPreCsvIntGrid"; break;
            case Flag::EXPORT_PRE_CSV_INT_GRID_FORMAT: j = "ExportPreCsvIntGridFormat"; break;
            case Flag::IGNORE_BACKUP_SUGGEST: j = "IgnoreBackupSuggest"; break;
            case Flag::MULTI_WORLDS: j = "MultiWorlds"; break;
            case Flag::PREPEND_INDEX_TO_LEVEL_FILE_NAMES: j = "PrependIndexToLevelFileNames"; break;
            case Flag::USE_MULTILINES_TYPE: j = "UseMultilinesType"; break;
            default: throw std::runtime_error("This should not happen");
        }
    }

    inline void from_json(const json & j, BgPos & x) {
        if (j == "Contain") x = BgPos::CONTAIN;
        else if (j == "Cover") x = BgPos::COVER;
        else if (j == "CoverDirty") x = BgPos::COVER_DIRTY;
        else if (j == "Repeat") x = BgPos::REPEAT;
        else if (j == "Unscaled") x = BgPos::UNSCALED;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const BgPos & x) {
        switch (x) {
            case BgPos::CONTAIN: j = "Contain"; break;
            case BgPos::COVER: j = "Cover"; break;
            case BgPos::COVER_DIRTY: j = "CoverDirty"; break;
            case BgPos::REPEAT: j = "Repeat"; break;
            case BgPos::UNSCALED: j = "Unscaled"; break;
            default: throw std::runtime_error("This should not happen");
        }
    }

    inline void from_json(const json & j, WorldLayout & x) {
        if (j == "Free") x = WorldLayout::FREE;
        else if (j == "GridVania") x = WorldLayout::GRID_VANIA;
        else if (j == "LinearHorizontal") x = WorldLayout::LINEAR_HORIZONTAL;
        else if (j == "LinearVertical") x = WorldLayout::LINEAR_VERTICAL;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const WorldLayout & x) {
        switch (x) {
            case WorldLayout::FREE: j = "Free"; break;
            case WorldLayout::GRID_VANIA: j = "GridVania"; break;
            case WorldLayout::LINEAR_HORIZONTAL: j = "LinearHorizontal"; break;
            case WorldLayout::LINEAR_VERTICAL: j = "LinearVertical"; break;
            default: throw std::runtime_error("This should not happen");
        }
    }

    inline void from_json(const json & j, IdentifierStyle & x) {
        if (j == "Capitalize") x = IdentifierStyle::CAPITALIZE;
        else if (j == "Free") x = IdentifierStyle::FREE;
        else if (j == "Lowercase") x = IdentifierStyle::LOWERCASE;
        else if (j == "Uppercase") x = IdentifierStyle::UPPERCASE;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const IdentifierStyle & x) {
        switch (x) {
            case IdentifierStyle::CAPITALIZE: j = "Capitalize"; break;
            case IdentifierStyle::FREE: j = "Free"; break;
            case IdentifierStyle::LOWERCASE: j = "Lowercase"; break;
            case IdentifierStyle::UPPERCASE: j = "Uppercase"; break;
            default: throw std::runtime_error("This should not happen");
        }
    }

    inline void from_json(const json & j, ImageExportMode & x) {
        if (j == "LayersAndLevels") x = ImageExportMode::LAYERS_AND_LEVELS;
        else if (j == "None") x = ImageExportMode::NONE;
        else if (j == "OneImagePerLayer") x = ImageExportMode::ONE_IMAGE_PER_LAYER;
        else if (j == "OneImagePerLevel") x = ImageExportMode::ONE_IMAGE_PER_LEVEL;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const ImageExportMode & x) {
        switch (x) {
            case ImageExportMode::LAYERS_AND_LEVELS: j = "LayersAndLevels"; break;
            case ImageExportMode::NONE: j = "None"; break;
            case ImageExportMode::ONE_IMAGE_PER_LAYER: j = "OneImagePerLayer"; break;
            case ImageExportMode::ONE_IMAGE_PER_LEVEL: j = "OneImagePerLevel"; break;
            default: throw std::runtime_error("This should not happen");
        }
    }
}
