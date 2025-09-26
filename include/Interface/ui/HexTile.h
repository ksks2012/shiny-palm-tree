#pragma once
#include "HexCoordinate.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <SDL2/SDL.h>

/**
 * Terrain types for hexagonal tiles, inspired by Roman warfare
 */
enum class TerrainType {
    PLAIN,          // Basic terrain, 1 movement cost
    FOREST,         // +20% evasion, -10% ranged accuracy, 2 movement cost
    MOUNTAIN,       // +30% defense, -1 movement range, 3 movement cost
    RIVER,          // Impassable for infantry, passable for cavalry with penalty
    SWAMP,          // 3 movement cost, -20% movement speed
    CITY_WALL,      // +50% defense, restricted entry
    ROAD,           // 0.5 movement cost, +10% movement speed
    BRIDGE,         // Allows crossing rivers, destructible
    CAMP,           // Supply point, restores morale/HP
    FORTIFICATION   // Buildable by engineers, +40% defense
};

/**
 * Special tile properties and tags for Roman-themed gameplay
 */
struct TileProperties {
    bool passable = true;
    bool buildable = false;          // Can engineers build fortifications?
    bool hidden = false;             // Line of sight blocker (forests)
    bool supplyPoint = false;        // Restores HP/morale
    bool destructible = false;       // Can be destroyed (bridges, walls)
    int movementCost = 1;           // Movement points required
    int defensiveBonus = 0;         // Percentage defense bonus
    int evasionBonus = 0;           // Percentage evasion bonus
    int rangedAccuracyPenalty = 0;  // Percentage ranged accuracy penalty
    std::string specialTag;         // Custom tag for scripting
};

/**
 * Events and triggers for dynamic battlefield scenarios
 */
struct TileEvent {
    std::string eventId;
    std::string triggerCondition;   // "unit_enter", "turn_X", "player_action"
    std::string action;             // "spawn_enemy", "terrain_change", "buff_units"
    std::unordered_map<std::string, std::string> parameters;
    bool triggered = false;
    int turnDelay = 0;             // Delay before event can trigger again
};

/**
 * Individual hexagonal tile with all properties and state
 * Supports Roman warfare mechanics and historical battle scenarios
 */
class HexTile {
public:
    HexTile(const HexCoordinate& coord, TerrainType terrain = TerrainType::PLAIN);
    
    // Core properties
    const HexCoordinate& getCoordinate() const { return coordinate_; }
    TerrainType getTerrainType() const { return terrainType_; }
    void setTerrainType(TerrainType terrain);
    
    // Height and elevation (0-3 levels for Roman hill tactics)
    int getHeight() const { return height_; }
    void setHeight(int height) { height_ = std::max(0, std::min(3, height)); }
    
    // Tile properties
    const TileProperties& getProperties() const { return properties_; }
    TileProperties& getProperties() { return properties_; }
    void setProperties(const TileProperties& props) { properties_ = props; }
    
    // Movement and combat calculations
    int getMovementCost() const { return properties_.movementCost; }
    bool isPassable() const { return properties_.passable; }
    int getDefensiveBonus() const { return properties_.defensiveBonus; }
    int getEvasionBonus() const { return properties_.evasionBonus; }
    int getRangedAccuracyPenalty() const { return properties_.rangedAccuracyPenalty; }
    
    // Line of sight and visibility
    bool blocksLineOfSight() const { return properties_.hidden; }
    bool isVisible() const { return visible_; }
    void setVisible(bool visible) { visible_ = visible; }
    
    // Unit occupancy
    bool isOccupied() const { return occupied_; }
    void setOccupied(bool occupied) { occupied_ = occupied; }
    const std::string& getOccupantId() const { return occupantId_; }
    void setOccupant(const std::string& unitId) { 
        occupantId_ = unitId; 
        occupied_ = !unitId.empty(); 
    }
    
    // Highlighting for UI feedback
    bool isHighlighted() const { return highlighted_; }
    void setHighlighted(bool highlighted) { highlighted_ = highlighted; }
    SDL_Color getHighlightColor() const { return highlightColor_; }
    void setHighlightColor(const SDL_Color& color) { highlightColor_ = color; }
    
    // Events and triggers
    void addEvent(const TileEvent& event);
    void removeEvent(const std::string& eventId);
    const std::vector<TileEvent>& getEvents() const { return events_; }
    std::vector<TileEvent> checkTriggeredEvents(const std::string& condition, 
                                               const std::unordered_map<std::string, std::string>& context);
    
    // Engineering and construction (Roman military engineering)
    bool canBuild() const { return properties_.buildable && !occupied_; }
    void buildFortification();
    void buildBridge(); // Convert river to passable
    void destroy();     // Destroy structures
    
    // Special Roman warfare mechanics
    bool isSupplyPoint() const { return properties_.supplyPoint; }
    bool isFormationTile() const { return formationTile_; } // Part of a military formation
    void setFormationTile(bool formation) { formationTile_ = formation; }
    
    // Serialization for map editor
    std::string toJSON() const;
    static HexTile fromJSON(const std::string& json);

private:
    HexCoordinate coordinate_;
    TerrainType terrainType_;
    int height_ = 0;
    TileProperties properties_;
    
    // State
    bool visible_ = true;
    bool occupied_ = false;
    bool highlighted_ = false;
    bool formationTile_ = false;
    std::string occupantId_;
    SDL_Color highlightColor_ = {255, 255, 0, 128}; // Yellow highlight by default
    
    // Events
    std::vector<TileEvent> events_;
    
    // Initialize default properties based on terrain type
    void initializeTerrainProperties();
};

/**
 * Utility functions for terrain and tile management
 */
class HexTileUtils {
public:
    // Get terrain properties
    static TileProperties getDefaultProperties(TerrainType terrain);
    static SDL_Color getTerrainColor(TerrainType terrain);
    static std::string getTerrainName(TerrainType terrain);
    static std::string getTerrainDescription(TerrainType terrain);
    
    // Combat calculations
    static int calculateMovementCost(const HexTile& from, const HexTile& to);
    static int calculateDefenseModifier(const HexTile& tile, int attackerHeight);
    static bool canSeeThrough(const HexTile& from, const HexTile& to, 
                             const std::vector<HexTile>& path);
    
    // Roman warfare specific
    static bool canFormTestudo(const std::vector<HexCoordinate>& formation); // Testudo formation check
    static std::vector<HexCoordinate> getLegionFormation(const HexCoordinate& center); // 3x3 legion formation
    static bool canBuildSiegeWorks(const HexTile& tile); // Check if siege works can be built
};
